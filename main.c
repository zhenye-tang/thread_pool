#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "thread_pool.h"

#define LOOP_COUNT          (20000)
static int g_value = 0;
static pthread_mutex_t lock;
static void test_func(void *arg)
{
    long value = (long)arg;
    pthread_mutex_lock(&lock);
    g_value += value;
    pthread_mutex_unlock(&lock);
}

int main(void) 
{
    int value = 0;
    thread_pool_t pool = thread_pool_create(5);
    pthread_mutex_init(&lock, NULL);

    for(long i = 0; i < LOOP_COUNT; i++)
    {
        value += i;
        thread_pool_add_job(pool, test_func, (void *)i);
    }

    sleep(2);
    thread_pool_destory(pool);

    if(g_value != value)
        printf("test error !!!!!!!!!\n");
    else
        printf("test success !!!!!!!!!!!\n");

    printf("g_value = %d value = %d.\n", g_value, value);

    return 0;
}