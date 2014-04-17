#include "search.h"


/* status bar stuff */
static void search_init_statsbar(FILE *f);
static void search_show_statsbar(FILE *f, unsigned int num, unsigned int denom);
/* search help functions */
static void search_create_tree(search_configuration *conf, position *pos, search_node *node, unsigned int depth);
static rating_t search_update_ratings(search_node *node, player_t player, unsigned int depth);
static rating_t search_mainsearch_recurse(position *pos, unsigned int depth, unsigned int maxdepth, uint64 *numnodes);
static void *search_mainsearch_thread(void *param);
static int search_collect_result(search_configuration *conf);
static void search_basic(search_configuration *conf);



#define SEARCH_STATS_BAR_LENGTH		40
static unsigned int search_statsbar_blockcount;
static unsigned int search_statsbar_perccount;

static void search_init_statsbar(FILE *f)
{
	unsigned int i;


	/* check parameters */
	ASSERT(f != (FILE *)0);
	/* reset counters */
	search_statsbar_blockcount = 0;
	search_statsbar_perccount  = 0;
	/* print status bar in start condition */
	for (i = 0; i < SEARCH_STATS_BAR_LENGTH; i++)
	{
		fprintf(f, "-");
	}
	/* print percentage counter */
	fprintf(f, "   0.0%%");
	fflush(stdout);
}

static void search_show_statsbar(FILE *f, unsigned int num, unsigned int denom)
{
	unsigned int i, newperc, newblocks;

	
	/* check parameters */
	ASSERT(f != (FILE *)0);
	ASSERT(denom != 0);
	ASSERT(num <= denom);

	/* determine new percentage counter */
	newperc   = (1000 * num) / denom;
	/* check if update of percentage counter necessary */
	if (newperc > search_statsbar_perccount)
	{
		/* remove old percentage counter */
		for (i = 0; i < 7; i++)
		{
			fprintf(f, "\b");
		}
		/* determine new block counter */
		newblocks = (SEARCH_STATS_BAR_LENGTH * num) / denom;
		/* check if update of status bar necessary */
		if (newblocks > search_statsbar_blockcount)
		{
			/* remove part of old status bar */
			for (i = SEARCH_STATS_BAR_LENGTH; i > search_statsbar_blockcount; i--)
			{
				fprintf(f, "\b");
			}
			/* print new "done" chars */
			for (; i < newblocks; i++)
			{
				fprintf(f, "+");
			}
			/* print remaining "todo" chars */
			for (; i < SEARCH_STATS_BAR_LENGTH; i++)
			{
				fprintf(f, "-");
			}
			/* set new status bar counter */
			search_statsbar_blockcount = newblocks;
		}
		/* print new percentage counter */
		fprintf(f, " %5.1f%%", newperc / 10.0);
		/* set new  percentage counter */
		search_statsbar_perccount = newperc;
		/* make sure all changes are displayed */
		fflush(f);
	}
}



static void search_create_tree(search_configuration *conf, position *pos, search_node *node, unsigned int depth)
{
	move *m;
	movelist ml;
	unsigned int i;


	/* check parameters */
	ASSERT(conf != (search_configuration *)0);
	ASSERT(pos  != (position *)0);
	ASSERT(node != (search_node *)0);

	/* update node counter */
	conf->result->numnodes++;

	if (depth < conf->precalcdepth)
	{
		switch (move_generate(pos, &ml))
		{
			case MOVESLEFT:
			{
				for (i = 0; i < ML_SIZE(ml); i++)
				{
					m = &ML_ENTRY(ml, i);
					move_apply(pos, m);
					node->children[i] = (search_node *)w_malloc(sizeof(search_node));
					memcpy(&(node->children[i]->m), m, sizeof(move));
					search_create_tree(conf, pos, node->children[i], depth + 1);
					move_rollback(pos, m);
				}
				node->numchildren = ML_SIZE(ml);
				break;
			}
			case STALEMATE:
			{
				node->rating      = RATING_INIT_INF;
				node->numchildren = 0;
				break;
			}
			case CHECKMATE:
			{
				if (pos->player == WHITE)
				{
					node->rating = RATING_INIT_BLACK;
				}
				else
				{
					node->rating = RATING_INIT_WHITE;
				}
				node->numchildren = 0;
				break;
			}
			default:
			{
				/* invalid return value from move generator */
				ASSERT(0);
			}
		}
	}
	else
	{
		switch (move_movesleft(pos))
		{
			case MOVESLEFT:
			{
				memcpy(&(node->pos), pos, sizeof(position));
				node->rating   = RATING_INIT_INF;
				node->nextleaf = conf->nextleaf;
				conf->nextleaf = node;
				conf->numleafnodes++;
				break;
			}
			case STALEMATE:
			{
				node->rating = RATING_INIT_INF;
				break;
			}
			case CHECKMATE:
			{
				if (pos->player == WHITE)
				{
					node->rating = RATING_INIT_BLACK;
				}
				else
				{
					node->rating = RATING_INIT_WHITE;
				}
				break;
			}
			default:
			{
				/* invalid return value from move generator */
				ASSERT(0);
			}
		}
		node->numchildren = 0;
	}
}



static rating_t search_update_ratings(search_node *node, player_t player, unsigned int depth)
{
	unsigned int i;
	rating_t r, bestrating;


	/* check parameters */
	ASSERT(node != (search_node *)0);

	if (node->numchildren > 0)
	{
		/* find best rating of children */
		if (player == WHITE)
		{
			bestrating = RATING_INIT_BLACK;
			for (i = 0; i < node->numchildren; i++)
			{
				r = search_update_ratings(node->children[i], BLACK, depth + 1);
				if (r > bestrating) /* get max */
				{
					bestrating = r;
				}
			}
		}
		else
		{
			bestrating = RATING_INIT_WHITE;
			for (i = 0; i < node->numchildren; i++)
			{
				r = search_update_ratings(node->children[i], WHITE, depth + 1);
				if (r < bestrating) /* get min */
				{
					bestrating = r;
				}
			}
		}
		/* update rating of best child to get rating of this node */
		RATING_UPDATE(bestrating);
		/* save rating and return it to the next higher node */
		node->rating = bestrating;
		return(bestrating);
	}
	else
	{
		return(node->rating);
	}
	/* keep compiler happy */
	return(RATING_INIT_INF);
}



static rating_t search_mainsearch_recurse(position *pos, unsigned int depth, unsigned int maxdepth, uint64 *numnodes)
{
	move *m;
	movelist ml;
	unsigned int i;
	rating_t r, bestrating;


	/* check parameters */
	ASSERT(pos != (position *)0);
	ASSERT(depth <= maxdepth);

	/* update node counter */
	(*numnodes)++;

	if (depth < maxdepth)
	{
		switch (move_generate(pos, &ml))
		{
			case MOVESLEFT:
			{
				if (pos->player == WHITE)
				{
					/* apply moves one by one and go into recursion */
					bestrating = RATING_INIT_BLACK;
					for (i = 0; i < ML_SIZE(ml); i++)
					{
						m = &ML_ENTRY(ml, i);
						move_apply(pos, m);
						r = search_mainsearch_recurse(pos, depth + 1, maxdepth, numnodes);
						if (r > bestrating)	/* get max */
						{
							bestrating = r;
						}
						move_rollback(pos, m);
					}
				}
				else
				{
					/* apply moves one by one and go into recursion */
					bestrating = RATING_INIT_WHITE;
					for (i = 0; i < ML_SIZE(ml); i++)
					{
						m = &ML_ENTRY(ml, i);
						move_apply(pos, m);
						r = search_mainsearch_recurse(pos, depth + 1, maxdepth, numnodes);
						if (r < bestrating)	/* get min */
						{
							bestrating = r;
						}
						move_rollback(pos, m);
					}
				}
				RATING_UPDATE(bestrating);
				return(bestrating);
			}
			case STALEMATE:
			{
				return(RATING_INIT_INF);
			}
			case CHECKMATE:
			{
				if (pos->player == WHITE)
				{
					return(RATING_INIT_BLACK);
				}
				else
				{
					return(RATING_INIT_WHITE);
				}
			}
			default:
			{
				/* invalid return value from move generator */
				ASSERT(0);
			}
		}
	}
	else
	{
		switch (move_movesleft(pos))
		{
			case MOVESLEFT:
			{
				return(RATING_INIT_INF);
			}
			case STALEMATE:
			{
				return(RATING_INIT_INF);
			}
			case CHECKMATE:
			{
				if (pos->player == WHITE)
				{
					return(RATING_INIT_BLACK);
				}
				else
				{
					return(RATING_INIT_WHITE);
				}
			}
			default:
			{
				/* invalid return value from move generator */
				ASSERT(0);
			}
		}
	}
	/* keep compiler happy */
	return(RATING_INIT_INF);
}



static void *search_mainsearch_thread(void *param)
{
	search_configuration *conf;
	search_node *node;
	int nodedone;
	uint64 numnodes;

	
	/* check parameters */
	ASSERT(param != (void *)0);

	conf = (search_configuration *)param;
	nodedone = 0;
	while (1)
	{
		/* we need exclusive access to job list */
		mutex_lock(&(conf->workerlock));
		if (nodedone)
		{
			/* if a job has been finished: update status bar */
			conf->numleafnodesdone++;
			conf->result->numnodes += numnodes;
			search_show_statsbar(stdout, conf->numleafnodesdone, conf->numleafnodes);
		}
		if (conf->numleafnodesleft == 0)
		{
			/* no jobs left: cancel this thread */
			mutex_unlock(&(conf->workerlock));
			return((void *)0);
		}
		/* there should be a next search node because counter is not yet 0! */
		ASSERT(conf->nextleaf != (search_node *)0);
		/* get node of current job */
		node = conf->nextleaf;
		/* prepare next job in list */
		conf->nextleaf = conf->nextleaf->nextleaf;
		/* we have taken this job so theres one less to do */
		conf->numleafnodesleft--;
		/* release exclusive access to job list */
		mutex_unlock(&(conf->workerlock));

		/* descend into tree to solve the problem */
		numnodes = 0;
		node->rating = search_mainsearch_recurse(&(node->pos), 0, conf->problem->depth - conf->precalcdepth, &numnodes);
		nodedone = 1;
	}
	/* keep compiler happy */
	return((void *)0);
}



static int search_collect_result(search_configuration *conf)
{
	unsigned int i, bestpos;
	search_node *node;
	player_t player;
	rating_t bestrating;


	/* check parameters */
	ASSERT(conf   != (search_configuration *)0);

	node   = conf->rootnode;
	player = conf->problem->pos.player;

	while(1)
	{
		/* search node is zero pointer: get out */
		if (node == (search_node *)0)
		{
			break;
		}
		/* no children left: stop descending into tree */
		if (node->numchildren == 0)
		{
			break;
		}
		/* init: first child has the best rating*/
		bestrating = node->children[0]->rating;
		bestpos    = 0;
		/* find child with the best rating (dependend from player) */
		if (player == WHITE)
		{
			for (i = 1; i < node->numchildren; i++)
			{
				if (node->children[i]->rating > bestrating)
				{
					bestrating = node->children[i]->rating;
					bestpos    = i;
				}
			}
		}
		else
		{
			for (i = 1; i < node->numchildren; i++)
			{
				if (node->children[i]->rating < bestrating)
				{
					bestrating = node->children[i]->rating;
					bestpos    = i;
				}
			}
		}
		/* save all best moves (only for root nodes) */
		if (node == conf->rootnode)
		{
			for (i = 0; i < node->numchildren; i++)
			{
				if (node->children[i]->rating == bestrating)
				{
					ML_ADD_MOVE(conf->result->bestmoves, node->children[i]->m);
				}
			}
		}
		/* next turn has the other player */
		player = OTHERPL(player);
		/* descend into tree */
		node = node->children[bestpos];
		/* save resulting move */
		ML_ADD_MOVE(conf->result->solution, node->m);
	}

	/* return with success */
	return(1);
}



static void search_destroy_tree(search_node *node)
{
	unsigned int i;


	/* check parameters */
	ASSERT(node != (search_node *)0);

	for (i = 0; i < node->numchildren; i++)
	{
		search_destroy_tree(node->children[i]);
		w_free(node->children[i]);
	}
}



static void search_basic(search_configuration *conf)
{
	thread *workers;
	rating_t rating;
	unsigned int i;
	time_t start, stop;


	/* check parameters */
	ASSERT(conf   != (search_configuration *)0);

	/* get start time */
	start = time(NULL);

	/* determine search depths */
	conf->precalcdepth = MIN(3, conf->problem->depth);
	printf("(%i/%i) ", conf->precalcdepth, conf->problem->depth);

	/* initialize search configuration */
	conf->numleafnodes		= 0;
	conf->numleafnodesdone	= 0;
	conf->numleafnodesleft	= 0;
	conf->nextleaf			= (search_node *)0;

	/* initalize search result */
	conf->result->rating = RATING_INIT_INF;
	ML_CLEAR(conf->result->solution);
	ML_CLEAR(conf->result->bestmoves);
	conf->result->numnodes = 0;
	conf->result->time     = 0;

	/* create search tree */
	conf->rootnode = (search_node *)w_malloc(sizeof(search_node));
	search_create_tree(conf, &(conf->problem->pos), conf->rootnode, 0);

	/* check if basic search tree contains the solution */
	rating = search_update_ratings(conf->rootnode, conf->problem->pos.player, 0);
	if (rating != RATING_INIT_INF)
	{
		conf->result->rating = rating;
		search_collect_result(conf);
		printf("result already found in precalculated tree");
	}
	/* check if no further search necessary */
	else if (conf->problem->depth <= conf->precalcdepth)
	{
		printf("no result found in precalculated tree, no main search necessary");
	}
	/* if not, start the main search */
	else
	{
		/* all leaf nodes are searched again in main search: no not count them twice */
		conf->result->numnodes -= conf->numleafnodes;
		/* prepare stats bar */
		search_init_statsbar(stdout);
		/* prepare threads */
		conf->numleafnodesleft = conf->numleafnodes;
		workers = (thread *)w_malloc(conf->numthreads * sizeof(thread));
		mutex_create(&(conf->workerlock));
		/* create and start all working threads */
		for (i = 0; i < conf->numthreads; i++)
		{
			thread_create(&(workers[i]), search_mainsearch_thread, (void *)conf);
		}
		/* supend main thread until all jobs have been finished */
		for (i = 0; i < conf->numthreads; i++)
		{
			thread_join(&(workers[i]));
		}
		/* workers should have finished all jobs */
		ASSERT(conf->numleafnodesleft == 0);
		ASSERT(conf->numleafnodes     == conf->numleafnodesdone);
		ASSERT(conf->nextleaf         == (search_node *)0);
		/* clean up */
		mutex_destroy(&(conf->workerlock));
		w_free(workers);
		/* extract result from search tree */
		rating = search_update_ratings(conf->rootnode, conf->problem->pos.player, 0);
		if (rating != RATING_INIT_INF)
		{
			conf->result->rating = rating;
			search_collect_result(conf);
		}
	}
	printf(" (");
	si_print(stdout, conf->result->numnodes);
	printf("N)\n");

	/* clean up */
	search_destroy_tree(conf->rootnode);
	w_free(conf->rootnode);

	/* get total time */
	stop = time(NULL);
	conf->result->time = stop - start;
}



void search_main(search_result *mainresult, search_problem *mainproblem)
{
	search_problem problem;
	search_configuration conf;
	search_result result;


	/* check parameters */
	ASSERT(mainresult  != (search_result *)0);
	ASSERT(mainproblem != (search_problem *)0);

	/* initialize search configuration */
	conf.numthreads			= 4;

	/* branch: different search modes */
	switch (mainproblem->mode)
	{
		case SEARCH_MODE_NONE:
		{
			/* nothing to do */
			break;
		}
		case SEARCH_MODE_DEPTH:
		{
			/* initalize search configuration */
			conf.problem = mainproblem;
			conf.result  = mainresult;
			/* execute the search */
			search_basic(&conf);
			/* that is it */
			break;
		}
		case SEARCH_MODE_DEPTH_FULL:
		{
			/* initalize search configuration */
			conf.problem = mainproblem;
			conf.result  = mainresult;
			/* execute the search */
			search_basic(&conf);
			/* check if an interesting result was found */
			if (mainresult->rating != RATING_INIT_INF)
			{
				/* check if result is incomplete */
				if ((int)ML_SIZE(mainresult->solution) < RATING_MOVES_TILL_MATE(mainresult->rating))
				{
					/* create local copies of problem and result */
					memcpy(&problem, mainproblem, sizeof(search_problem));
					memcpy(&result,  mainresult,  sizeof(search_result));
					/* initalize search configuration */
					conf.problem = &problem;
					conf.result  = &result;
					/* continue search until result is complete */
					while ((int)ML_SIZE(mainresult->solution) < RATING_MOVES_TILL_MATE(mainresult->rating))
					{
						/* decrease search depth and adapt precalculation depth */
						problem.depth -= ML_SIZE(result.solution);
						/* transfer current solution moves to problem */
						ML_APPLY(problem.pos, result.solution);
						ML_CLEAR(result.solution);
						/* initiate new search */
						search_basic(&conf);
						/* transfer result */
						ML_COPY(mainresult->solution, result.solution);
					}
				}
			}
			/* that is it */
			break;
		}
		default:
		{
			/* not implemented or invalid search */
			ASSERT(0);
		}
	}

}

