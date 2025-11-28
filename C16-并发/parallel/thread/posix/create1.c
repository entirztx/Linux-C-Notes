#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *func(void *p)
{
    puts("Thread is working!");
    pthread_exit(NULL);
    //  return NULL;
}

/**
 * @brief   创建一个简单的进程
 * @param   argc
 * @param   argv
 * @return  int
 */

int main(int argc, char **argv)
{
    pthread_t tid;
    int       err;

    puts("Begin!");

    err = pthread_create(&tid, NULL, func, NULL);
    if (err)
    {
        fprintf(stderr, "pthread_create():%s\n", strerror(err));
        exit(1);
    }

    pthread_join(tid, NULL);
    puts("End!");
    exit(0);
}
