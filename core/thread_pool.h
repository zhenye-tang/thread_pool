/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-6      tzy          first implementation
 */

#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#define THREAD_STACK_SIZE   (4096)
#define TP_MALLOC           malloc
#define TP_FREE             free

typedef struct thread_pool* thread_pool_t;
typedef void (*thread_pool_job)(void *job_ctx);

#define TP_ASSERT(EX)        \
if(!(EX))                    \
{                            \
    while(1);                \
}                            \

thread_pool_t thread_pool_create(int thread_num);
int thread_pool_destory(thread_pool_t tp);
int thread_pool_add_job(thread_pool_t tp, thread_pool_job job, void *job_ctx);

#ifdef __cplusplus
}
#endif
#endif //__THREAD_POOL_H__