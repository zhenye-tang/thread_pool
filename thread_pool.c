/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-6      tzy          first implementation
 */

#include "thread_pool.h"
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

typedef enum thread_pool_state
{
    THREAD_POOL_DESTORY = 0,
    THREAD_POOL_INIT,
    THREAD_POOL_RUNING,
}thread_pool_state_t;

struct job
{
    thread_pool_job exec;
    void *job_ctx;
    struct job *next;
};

struct job_queue
{
    struct job *head;
    struct job *tail;
    pthread_mutex_t lock;
};

struct sync
{
    int atomic;
    int destory;
    pthread_mutex_t lock;
    pthread_cond_t cond;
};

struct thread_pool
{
    struct job_queue *queue;
    pthread_t *thread;
    int thread_num;
    thread_pool_state_t state;
    struct sync sync;
};

static void wait_signal(struct sync *sync)
{
    pthread_mutex_lock(&sync->lock);
    while(!sync->atomic && !sync->destory)
    {
        pthread_cond_wait(&sync->cond, &sync->lock);
    }
    sync->atomic -= 1;
    pthread_mutex_unlock(&sync->lock);
}

static void post_signal(struct sync *sync)
{
    pthread_mutex_lock(&sync->lock);
    sync->atomic += 1;
    pthread_mutex_unlock(&sync->lock);
    pthread_cond_signal(&sync->cond);
}

static void broadcast_signal(struct sync *sync)
{
    sync->destory = 1;
    pthread_cond_broadcast(&sync->cond);
}

static void init_signal(struct sync *sync)
{
    sync->destory = sync->atomic = 0;
    /* init sync signal */
    pthread_cond_init(&sync->cond, NULL);
    pthread_mutex_init(&sync->lock, NULL);
}

static void destory_signal(struct sync *sync)
{
    sync->destory = sync->atomic = 0;
    pthread_mutex_destroy(&sync->lock);
    pthread_cond_destroy(&sync->cond);
}

static int job_enqueue(struct job_queue* queue, struct job *job)
{
    int err = -1;
    struct job *_job = TP_MALLOC(sizeof(struct job));
    if(_job)
    {
        *_job = *job;
        _job->next = NULL;
        pthread_mutex_lock(&queue->lock);
        if(queue->head == NULL)
        {
            queue->head = _job;
            queue->tail = _job;
        }
        else
        {
             queue->tail->next = _job;
             queue->tail = _job;
        }
        pthread_mutex_unlock(&queue->lock);
        err = 0;
    }

    return err;
}

static struct job *job_dequeue(struct job_queue* queue)
{
    struct job *job = NULL;
    pthread_mutex_lock(&queue->lock);
    if(queue->head != NULL)
    {
        job = queue->head;
        queue->head = queue->head->next;
        if(queue->head == NULL)
            queue->tail = NULL;
    }
    pthread_mutex_unlock(&queue->lock);

    return job;
}

static int job_queue_destory(struct job_queue* queue)
{
    struct job *job;

    pthread_mutex_lock(&queue->lock);
    while(queue->head)
    {
        job = queue->head;
        queue->head = queue->head->next;
        TP_FREE(job);
    }
    pthread_mutex_destroy(&queue->lock);
    TP_FREE(queue);

    return 0;
}

static struct job_queue* job_queue_create(void)
{
    struct job_queue *queue = TP_MALLOC(sizeof(struct job_queue));
    if(queue)
    {
        memset(queue, 0, sizeof(struct job_queue));
        pthread_mutex_init(&queue->lock, NULL);
    }

    return queue;
}

static void *thread_pool_exec(void *prma)
{
    thread_pool_t tp = (thread_pool_t)prma;
    struct job *job;
    while(tp->state)
    {
        wait_signal(&tp->sync);
        if(tp->state && (job = job_dequeue(tp->queue)))
        {
            job->exec(job->job_ctx);
            TP_FREE(job);
        }
    }

    return (void *)"by";
}

int thread_pool_destory(thread_pool_t tp)
{
    TP_ASSERT(tp);
    tp->state = THREAD_POOL_DESTORY;
    /* weakup all thread */
    broadcast_signal(&tp->sync);
    for(int i = 0; i < tp->thread_num; i++)
    {
        pthread_join(tp->thread[i], NULL);
    }
    destory_signal(&tp->sync);
    TP_FREE(tp->thread);
    job_queue_destory(tp->queue);
    TP_FREE(tp);

    return 0;
}

thread_pool_t thread_pool_create(int thread_num)
{
    thread_pool_t tp = NULL;
    pthread_attr_t attr;
    int success = (
            (tp = TP_MALLOC(sizeof(*tp))) &&
            (tp->queue = job_queue_create()) &&
            (tp->thread = TP_MALLOC(thread_num * sizeof(pthread_t)))
    );

    if(success)
    {
        memset(tp->thread, 0, thread_num * sizeof(pthread_t));
        tp->thread_num = thread_num;
        /* set thread attribute */
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE);
        init_signal(&tp->sync);
        tp->state = THREAD_POOL_RUNING;

        for(int i = 0; i < thread_num; i++)
        {
            if(pthread_create(&tp->thread[i], &attr, thread_pool_exec, tp) != 0)
            {
                tp->state = THREAD_POOL_INIT;
                tp->thread_num = i;
                break;
            }
        }

        if(tp->state != THREAD_POOL_RUNING)
        {
            thread_pool_destory(tp);
            tp = NULL;
        }
    }
    else
    {
        if(tp && tp->queue)
            job_queue_destory(tp->queue);
        TP_FREE(tp);
        tp = NULL;
    }

    return tp;
}

int thread_pool_add_job(thread_pool_t tp, thread_pool_job job, void *job_ctx)
{
    int err = -1;
    TP_ASSERT(tp);
    TP_ASSERT(tp->state == THREAD_POOL_RUNING);

    struct job _job = {
        .exec = job,
        .job_ctx = job_ctx,
    };

    if((err = job_enqueue(tp->queue, &_job)) == 0)
        post_signal(&tp->sync);

    return err;
}