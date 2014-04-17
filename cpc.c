#include "cpc.h"



/* this is the interface to the cpc parser */
extern int cpc_yyparse();
extern FILE *cpc_yyin, *cpc_yyout;
extern unsigned int cpc_column;
extern unsigned int cpc_row;
extern const char *cpc_filename;
extern cpc *cpc_mainlist;

/* prototypes for static helper functions */
static void cpc_release_single_entry(problem *p);



int cpc_load(cpc *c, const char *filename)
{
	/* check parameters */
	ASSERT(c != (cpc *)0);
	ASSERT(filename != (const char *)0);

	/* initialize file handles */
	if ((cpc_yyin = fopen(filename, "r")) == (FILE *)0)
	{
		fprintf(stderr, "unable to open file %s (%s)\n\n", filename, strerror(errno));
		return(1);
	}
	cpc_yyout = stderr;

	/* initialize information to parser */
	c->numentries = 1;
	c->p = (problem **)w_malloc(sizeof(problem *));
	c->p[0] = (problem *)w_malloc(sizeof(problem));
	c->p[0]->valid = 0;

	cpc_column = 1;
	cpc_row    = 1;
	cpc_filename = filename;
	cpc_mainlist = c;

	/* start parser */
	if (cpc_yyparse() != 0)
	{
		return(1);
	}

	/* remove last (and unused) entry from list */
	cpc_release_single_entry(c->p[c->numentries - 1]);
	c->numentries--;
	c->p = (problem **)realloc(c->p, c->numentries * sizeof(problem *));

	/* cleanup */
	fclose(cpc_yyin);

	/* return with success */	
	return(0);
}



void cpc_store(cpc *c, FILE *f)
{
	unsigned int i;


	/* check parameters */
	ASSERT(c != (cpc *)0);
	ASSERT(f != (FILE *)0);

	/* print all problems and their valid entries */
	for (i = 0; i < c->numentries; i++)
	{
		fprintf(f, "problem\n{\n");
		if ((c->p[i]->valid & CPC_ENTRY_VALID_DESCRIPTION) != 0)
		{
			fprintf(f, "\tdescription = \"%s\";\n", c->p[i]->description);
		}
		if ((c->p[i]->valid & CPC_ENTRY_VALID_FEN) != 0)
		{
			fprintf(f, "\tposition = [%s];\n", c->p[i]->pstr);
		}
		if ((c->p[i]->valid & CPC_ENTRY_VALID_SOLUTION) != 0)
		{
			fprintf(f, "\tsolution = \"%s\";\n", c->p[i]->solution);
		}
		if ((c->p[i]->valid & CPC_ENTRY_VALID_DEPTH) != 0)
		{
			fprintf(f, "\tdepth = %i;\n", c->p[i]->depth);
		}
		if ((c->p[i]->valid & CPC_ENTRY_VALID_NODES) != 0)
		{
			fprintf(f, "\tnodes = %i;\n", c->p[i]->nodes);
		}
		fprintf(f, "}\n");
	}
}



void cpc_start(cpc *c)
{
	unsigned int i, j;	/* TODO */
	clock_t t1, t2;


	/* check parameters */
	ASSERT(c != (cpc *)0);

	for (i = 0; i < c->numentries; i++)
	{
		t1 = clock();
		for (j = 0; j < ((1 + i) * 0xffffff); j++);	/* TODO */
		t2 = clock();
		/* save time */
		c->p[i]->time = t2 - t1;
		c->p[i]->valid |= CPC_ENTRY_VALID_TIME;
	}
}



void cpc_showstats(cpc *c, FILE *f)
{
	unsigned int i;
	float msecs;
	clock_t tsum;
	unsigned int nsum;
	


	/* check parameters */
	ASSERT(c != (cpc *)0);
	ASSERT(f != (FILE *)0);

	/* reset sums */
	tsum = 0;
	nsum = 0;

	/* print results */
	for (i = 0; i < c->numentries; i++)
	{
		if (((c->p[i]->valid & CPC_ENTRY_VALID_TIME)  != 0) &&
			((c->p[i]->valid & CPC_ENTRY_VALID_NODES) != 0))
		{
			msecs = (1.0 * c->p[i]->time) / (CLOCKS_PER_SEC / 1000);
			fprintf(f, "%i) %u nodes visited in %f seconds (%f kN/s)\n",
				i + 1, c->p[i]->nodes, msecs/1000.0, c->p[i]->nodes/msecs);
			/* calculate sums */
			tsum += c->p[i]->time;
			nsum += c->p[i]->nodes;
		}
		else
		{
			fprintf(f, "%i) <no statistics available>\n", i + 1);
		}
	}
	fprintf(f, "----------------------------------------\n");
	msecs = (1.0 * tsum) / (CLOCKS_PER_SEC / 1000);
	fprintf(f, "total: %u nodes visited in %f seconds (%f kN/s)\n\n",
		nsum, msecs/1000.0, nsum/msecs);
}



static void cpc_release_single_entry(problem *p)
{
	if ((p->valid & CPC_ENTRY_VALID_DESCRIPTION) != 0)
	{
		w_free(p->description);
	}
	if ((p->valid & CPC_ENTRY_VALID_FEN) != 0)
	{
		w_free(p->pstr);
	}
	if ((p->valid & CPC_ENTRY_VALID_SOLUTION) != 0)
	{
		w_free(p->solution);
	}
	w_free(p);
}



void cpc_release(cpc *c)
{
	unsigned int i;


	/* check parameters */
	ASSERT(c != (cpc *)0);

	for (i = 0; i < c->numentries; i++)
	{
		cpc_release_single_entry(c->p[i]);
	}
	w_free(c->p);
}
