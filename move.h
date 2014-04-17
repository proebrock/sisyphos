#ifndef __MOVE_H__
#define __MOVE_H__



#include <stdio.h>

#include "position.h"



/* move types */
typedef uint8			mtype_t;
#define MOVE			0
#define CAPTURE			1
#define CASTLING_OO		2
#define CASTLING_OOO	3
#define ENPASSANT		4
#define PROM_MOVE		5
#define PROM_CAPTURE	6
#define NUMMOVETYPES	7



/*
**
** -----   !!! IMPORTANT !!!   -----
**
** valid fields in "move" for different move types,
** the other fields (marked with '-') are undefined:
**
** ----------------------------------------
** 			|	0	1	2	3	4	5	6
** ----------------------------------------
** type		|	X	X	X	X	X	X	X
** from		|	X	X	-	-	X	X	X
** to		|	X	X	-	-	X	X	X
** fpiece	|	X	X	-	-	-	-	-
** tpiece	|	-	X	-	-	-	-	X
** promto	|	-	-	-	-	-	X	X
** ----------------------------------------
**
** with the trick of a special define
**
** #define promto fpiece
**
** we can save an explicit variable promto, which is used
** only in cases when fpiece is undefined!
**
*/

struct move_t
{
	mtype_t type;
	field_t from;
	field_t to;
	piece_t fpiece;
	piece_t tpiece;
/*	piece_t promto; */
};
#define move struct move_t

#define promto fpiece



/* results for move generation */
#define MOVESLEFT	0
#define STALEMATE	1
#define CHECKMATE	2



#include "movelist.h"



/* reset move to invalid move */
extern void move_clear(move *m);

/* print move to output stream */
extern void move_print(FILE *stream, const move *m);
/* read move from string */
extern int  move_read(move *m, const char *str);

/* apply move to chess board */
extern void move_apply_fast(position *p, player_t player, const move *m);
extern void move_apply(position *p, const move *m);
/* rollback/undo move from chess board */
extern void move_rollback_fast(position *p, player_t player, const move *m);
extern void move_rollback(position *p, const move *m);

/* generate all legal moves to current position on board */
extern int move_generate(position *p, movelist *ml);
extern int move_movesleft(position *p);



#endif	/* __MOVE_H__ */
