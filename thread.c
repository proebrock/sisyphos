#include "thread.h"



int thread_create(thread *t, void *(*thread_routine)(void *), void *arg)
{
	/* check parameters */
	ASSERT(t              != (thread *)0);
	ASSERT(thread_routine != (void *(*)(void *))0);
	ASSERT(arg            != (void *)0);

	/* create thread */
	if (pthread_create(t, (pthread_attr_t *)0, thread_routine, arg) != 0)
	{
		return(0);
	}
	/* return with success */
	return(1);
}



int thread_join(thread *t)
{
	/* check parameters */
	ASSERT(t != (thread *)0);

	/* wait for thread cancelation */
	if (pthread_join(*t, (void **)0) != 0)
	{
		return(0);
	}
	/* return with success */
	return(1);
}



int mutex_create(mutex *m)
{
	/* check parameters */
	ASSERT(m != (mutex *)0);

	/* initialize mutex */
	if (pthread_mutex_init(m, (const pthread_mutexattr_t *)0) != 0)
	{
		return(0);
	}
	/* return with success */
	return(1);
}



int mutex_lock(mutex *m)
{
	/* check parameters */
	ASSERT(m != (mutex *)0);

	/* lock mutex */
	if (pthread_mutex_lock(m) != 0)
	{
		return(0);
	}
	/* return with success */
	return(1);
}



int mutex_unlock(mutex *m)
{
	/* check parameters */
	ASSERT(m != (mutex *)0);

	/* unlock mutex */
	if (pthread_mutex_unlock(m) != 0)
	{
		return(0);
	}
	/* return with success */
	return(1);
}



int mutex_destroy(mutex *m)
{
	/* check parameters */
	ASSERT(m != (mutex *)0);

	/* unlock mutex */
	if (pthread_mutex_destroy(m) != 0)
	{
		return(0);
	}
	/* return with success */
	return(1);
}

