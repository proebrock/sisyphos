#ifndef __SEARCH_H__
#define __SEARCH_H__


#include <string.h>

#include "globals.h"
#include "wrapper.h"
#include "thread.h"
#include "chess.h"
#include "move.h"
#include "movelist.h"
#include "board.h"
#include "position.h"



typedef int16 rating_t;

#define RATING_INIT_BASE	 30000
#define RATING_INIT_WHITE	 RATING_INIT_BASE
#define RATING_INIT_INF		 0
#define RATING_INIT_BLACK	-RATING_INIT_BASE

#define RATING_MOVES_TILL_MATE(R)	((RATING_INIT_BASE) - abs(R))

#define RATING_PRINT_MOVES_TILL_MATE(S, R)														\
if ((R) > 0)																					\
{																								\
	fprintf((S), "white is able to checkmate in %i halfmoves\n", RATING_MOVES_TILL_MATE(R));	\
}																								\
else if ((R) < 0)																				\
{																								\
	fprintf((S), "black is able to checkmate in %i halfmoves\n", RATING_MOVES_TILL_MATE(R));	\
}																								\
else																							\
{																								\
	fprintf((S), "no checkmate possible within search depth\n");								\
}

#define RATING_UPDATE(R)											\
if ((R) != 0)														\
{																	\
	/* make sure never to reach RATING_INIT_INF with update */		\
	ASSERT(abs(R) > 2);												\
	(R) = (R) - (((R) >> 15) << 1) - 1;								\
}



struct search_node_t;
struct search_node_t
{
	position pos;
	move m;
	rating_t rating;
	unsigned int numchildren;
	struct search_node_t *children[MAX_MOVELIST_ENTRIES];
	struct search_node_t *nextleaf;
};
#define search_node struct search_node_t



typedef enum
{
	/* no search */
	SEARCH_MODE_NONE,
	/* search to given depth, save result as far as known and stop search */
	SEARCH_MODE_DEPTH,
	/* search to given depth, determine full result */
	SEARCH_MODE_DEPTH_FULL,
	/* depth limited iterative deepening */
	SEARCH_MODE_ITERATIVE_TO_MAXDEPTH,
	/* iterative deepening without depth limit */
	SEARCH_MODE_ITERATIVE_UNLIMITED
} search_mode;

struct search_problem_t
{
	/* initial search position */
	position pos;
	/* depth to search */
	unsigned int depth;
	/* search mode */
	search_mode mode;
};
#define search_problem struct search_problem_t



struct search_result_t
{
	/* rating: mate in ... */
	rating_t rating;
	/* moves towards mate: example solution */
	movelist solution;
	/* all highest valued moves for current player */
	movelist bestmoves;
	/* number of nodes searched */
	uint64 numnodes;
	/* time used for search process */
	uint64 time;
};
#define search_result struct search_result_t



struct search_configuration_t
{
	/* search input: the problem */
	search_problem *problem;

	/* search output: the result */
	search_result *result;

	/* depth of pre-calculation */
	unsigned int precalcdepth;

	/* number of threads to start for the main search */
	unsigned int numthreads;

	/* pointer to root node of precalculated search tree */
	search_node *rootnode;

	/* mutex for exclusive access of worker threads to "nextleaf" and "numleafnodesleft" */
	mutex workerlock;
	/* number of leaf nodes in total */
	unsigned int numleafnodes;
	/* number of leaf nodes left to do */
	unsigned int numleafnodesdone;
	/* number of leaf nodes left to do */
	unsigned int numleafnodesleft;
	/* pointer to next leaf node to process */
	search_node *nextleaf;
};
#define search_configuration struct search_configuration_t



extern void search_main(search_result *mainresult, search_problem *mainproblem);



#endif	/* __SEARCH_H__ */
