#ifndef __CPC_H__
#define __CPC_H__



#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include "globals.h"
#include "position.h"
#include "wrapper.h"



#define CPC_ENTRY_VALID_FEN				0x01
#define CPC_ENTRY_VALID_DESCRIPTION		0x02
#define CPC_ENTRY_VALID_SOLUTION		0x04
#define CPC_ENTRY_VALID_DEPTH			0x08
#define CPC_ENTRY_VALID_NODES			0x10
#define CPC_ENTRY_VALID_TIME			0x20



/* type of one list entry of a chess problem collection */
struct problem_t
{
	/* position as fen string */
	char *pstr;

	/* short description to problem */
	char *description;
	/* solution as string */
	char *solution;
	/* depth of solution */
	unsigned int depth;
	/* number of nodes to search */
	unsigned int nodes;

	/* running time to get result */
	clock_t time;

	/* determine the valid parts of this problem information */
	unsigned char valid;
};
#define problem struct problem_t

/* a list of chess problems: a chess problem collection */
struct cpc_t
{
	/* pointer list to problems */
	problem **p;
	/* number of entries in list */
	unsigned int numentries;
};
#define cpc struct cpc_t



extern int  cpc_load(cpc *c, const char *filename);
extern void cpc_store(cpc *c, FILE *f);
extern void cpc_start(cpc *c);
extern void cpc_showstats(cpc *c, FILE *f);
extern void cpc_release(cpc *c);



#endif	/* __CPC_H__ */

