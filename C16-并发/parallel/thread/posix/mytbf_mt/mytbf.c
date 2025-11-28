/***********************************************************************
 * @file     mytbf.c
 * @brief
 *           令牌桶库实现
 *
 * @author   lzy (lllzzzyyy@buaa.edu.cn)
 * @url      https://lzyyyyyy.fun
 *
 * @date     2024-05-26
 *
 ***********************************************************************/

#include "mytbf.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct mytbf_st
{
    int             cps;
    int             burst;
    int             token;
    int             pos;
    pthread_mutex_t mut;
    pthread_cond_t  cond;
};

static struct mytbf_st *job[MYTBF_MAX];
static pthread_mutex_t  mut_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_t        tid_alarm;
static pthread_once_t   init_once = PTHREAD_ONCE_INIT;
static int              inited    = 0;
// static __sighandler_t   alrm_handler_save;   //!!! 保存原来的alarm行为


static int get_free_pos_unlock( )
{
    int i;

    for (i = 0; i < MYTBF_MAX; i++)
    {
        if (NULL == job[i])
            return i;
    }

    return -1;
}

static void *thr_alrm(void *p)
{
    int i;

    //  alarm(1);
    while (1)
    {
        pthread_mutex_lock(&mut_job);
        for (i = 0; i < MYTBF_MAX; i++)
        {
            if (NULL != job[i])
            {
                pthread_mutex_lock(&job[i]->mut);
                job[i]->token += job[i]->cps;
                if (job[i]->token > job[i]->burst)
                    job[i]->token = job[i]->burst;
                pthread_cond_broadcast(&job[1]->cond);
                pthread_mutex_unlock(&job[i]->mut);
            }
        }
        pthread_mutex_unlock(&mut_job);
        sleep(1);
    }
}

static void module_unload( )
{
    int i;

    // signal(SIGALRM, alrm_handler_save);
    // alarm(0);

    pthread_cancel(tid_alarm);
    pthread_join(tid_alarm, NULL);

    for (i = 0; i < MYTBF_MAX; i++)
    {
        if (job[i] != NULL)
        {
            mytbf_destroy(job[i]);
        }
    }
    pthread_mutex_destroy(&mut_job);
}

static void module_load( )
{
    // alrm_handler_save = signal(SIGALRM, alrm_handler);
    // alarm(1);


    int err;

    err = pthread_create(&tid_alarm, NULL, thr_alrm, NULL);
    if (err)
    {
        fprintf(stderr, "pthread_create():%s\n", strerror(err));
        exit(1);
    }

    atexit(module_unload);   //!!! atexit()
}

mytbf_t *mytbf_init(int cps, int burst)
{
    struct mytbf_st *me;
    int              pos;

    /*
        if (!inited)
        {
            module_load( );
            inited = 1;
        }
    */

    pthread_once(&init_once, module_load);

    me = malloc(sizeof(*me));
    if (NULL == me)
        return NULL;

    me->cps   = cps;
    me->burst = burst;
    me->token = 0;
    pthread_mutex_init(&me->mut, NULL);
    pthread_cond_init(&me->cond, NULL);

    pthread_mutex_lock(&mut_job);
    pos = get_free_pos_unlock( );
    if (pos < 0)
    {
        pthread_mutex_unlock(&mut_job);
        free(me);
        return NULL;
    }
    me->pos  = pos;
    job[pos] = me;
    pthread_mutex_unlock(&mut_job);

    return me;
}

static int min(int a, int b)
{
    return a < b ? a : b;
}

int mytbf_fetchtoken(mytbf_t *ptr, int size)
{
    if (size <= 0)
        return -EINVAL;

    struct mytbf_st *me = ptr;
    int              n;

    pthread_mutex_lock(&me->mut);
    while (me->token <= 0)
    {
        pthread_cond_wait(&me->cond,
                          &me->mut);   // 查询法 相当于下面几句注释的功能
        /*
            轮询法
            pthread_mutex_unlock(&me->mut);
            sched_yield( );
            pthread_mutex_lock(&me->mut);
        */
    }

    n = min(size, me->token);

    me->token -= n;
    pthread_mutex_unlock(&me->mut);

    return n;
}

int mytbf_returntoken(mytbf_t *ptr, int size)
{
    if (size <= 0)
        return -EINVAL;

    struct mytbf_st *me = ptr;

    pthread_mutex_lock(&me->mut);
    me->token += size;
    if (me->token > me->burst)
        me->token = me->burst;
    pthread_cond_broadcast(&me->cond);
    pthread_mutex_unlock(&me->mut);

    return size;
}

int mytbf_destroy(mytbf_t *ptr)
{
    struct mytbf_st *me = ptr;

    pthread_mutex_lock(&mut_job);
    job[me->pos] = NULL;
    pthread_mutex_unlock(&mut_job);

    pthread_mutex_destroy(&me->mut);
    pthread_cond_destroy(&me->cond);
    free(ptr);

    return 0;
}
