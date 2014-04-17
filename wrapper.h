#ifndef __WRAPPER_H__
#define __WRAPPER_H__



#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"



/* logging and error handling */
typedef enum LOG_LVL_T
{
	LOG_IGNORE,
	LOG_DEBUG,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
} LOG_LVL;
extern void die();
extern void print_log(LOG_LVL ll, const char *fmt, ...); 

/* wrapper for library and system funtions */
extern int w_fclose(FILE *stream);
extern FILE *w_fopen(LOG_LVL ll, char *path, char *mode);
extern void w_free(void *ptr);
extern void *w_malloc(size_t size);



#endif	/* __WRAPPER_H__ */
