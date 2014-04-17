#include "wrapper.h"



void die()
{
	exit(EXIT_FAILURE);
}



void print_log(LOG_LVL ll, const char *fmt, ...) 
{
	va_list ap;


	/* check parameters */
	ASSERT(fmt != (const char *)0);

	/* print message */
	if (ll != LOG_IGNORE)
	{
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}

	/* stop program */
	if ((ll == LOG_ERROR) || (ll == LOG_FATAL))
	{
		die();
	}
}



int w_fclose(FILE *stream)
{
	int r;


	/* check parameters */
	ASSERT(stream != (FILE *)0);

	if ((r = fclose(stream)) != 0)
	{
		print_log(LOG_ERROR, "unable to close file (%s)\n", strerror(errno));
	}
	return(r);
}



FILE *w_fopen(LOG_LVL ll, char *path, char *mode)
{
	FILE *r;
	

	/* check parameters */
	ASSERT(path != (char *)0);
	ASSERT(mode != (char *)0);

	if ((r = fopen(path, mode)) == (FILE *)0)
	{
		print_log(ll, "unable to open file \"%s\" (%s)\n", path, strerror(errno));
	}
	return(r);
}



void w_free(void *ptr)
{
	/* check parameters */
	ASSERT(ptr != (void *)0);

	free(ptr);
}



void *w_malloc(size_t size)
{
	void *ptr;


	if ((ptr = malloc(size)) == (void *)0)
	{
		print_log(LOG_FATAL, "unable to allocate memory\n");
	}
	return(ptr);
}

