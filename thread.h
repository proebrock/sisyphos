#ifndef __THREAD_H__
#define __THREAD_H__



#include <pthread.h>

#include "globals.h"



#define thread	pthread_t
#define mutex	pthread_mutex_t



extern int thread_create(thread *t, void *(*thread_routine)(void *), void *arg);
extern int thread_join(thread *t);

extern int mutex_create(mutex *m);
extern int mutex_lock(mutex *m);
extern int mutex_unlock(mutex *m);
extern int mutex_destroy(mutex *m);



#endif	/* __THREAD_H__ */
