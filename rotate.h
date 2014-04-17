#ifndef __ROTATE_H__
#define __ROTATE_H__



#include "globals.h"
#include "chess.h"



#define NORTH			0
#define WEST			1
#define NORTHWEST		2
#define NORTHEAST		3
#define NUMDIRECTIONS	4



/*
** index conversion macros for different rotated boards:
**
** ROT_N_W	- from field on NORTH aligned board get same
**            field on WEST aligned board
** ROT_W_N	- from field on WEST aligned board get same
**            field on NORTH aligned board
**
** ROT_N_NW	- from field on NORTH aligned board get same
**            field on NORTHWEST aligned board
** ROT_NW_N	- from field on NORTHWEST aligned board get same
**            field on NORTH aligned board
** ROT_N_NE	- from field on NORTH aligned board get same
**            field on NORTHEAST aligned board
** ROT_NE_N	- from field on NORTHEAST aligned board get same
**            field on NORTH aligned board
*/
#define ROT_N_W		0
#define ROT_W_N		1
#define ROT_N_NW	2
#define ROT_NW_N	3
#define ROT_N_NE	4
#define ROT_NE_N	5
#define NUMROTMODES	6


/*
** from an index on one board, get index on board with
** different rotation; mode defines the rotation, e.g. ROT_N_NW
*/
#define ROTATE_POS(MODE, INDEX)	lut_rotate[(MODE)][(INDEX)]

/*
** rotate a board S, given as a 64-bit variable into
** a board D (64-bit variable, too) with mode R;
** H is a help variable of type "field_t"
*/
#define ROTATE_BOARD(S, D, H, R)		\
	(D) = UINT64CONST(0);				\
	while (1)							\
	{									\
		if ((S) == 0)					\
			break;						\
		CLEARNEXTBIT((S), (H));			\
		(H) = ROTATE_POS((R), (H));		\
		SETBIT((D), (H));				\
	}




extern const field_t lut_rotate[NUMROTMODES][NUMFIELDS];



#ifndef NDEBUG
/*
** check rotation functions (and therefore the
** consistency of hardcoded lookup tables!)
*/
extern void rotate_checktables();
#endif



#endif	/* __ROTATE_H__ */
