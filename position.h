#ifndef __POSITION_H__
#define __POSITION_H__



#include <stdio.h>

#include "board.h"



struct position_t
{
	/* information about pieces on the chess board */
	board b;
	/*
	** halfmoves since beginning of the game,
	** starting with 0 (white hasn't made the first move yet)
	*/
	uint16 halfmoves;
	/*
	** halfmoves since the last capture of a piece or the last
	** movement of a pawn: used to control the 50-moves rule
	*/
	uint16 lastcap;
	/*
	** if the last move was a two-step move with a pawn,
	** this variable describes the field to make an en passant
	** move to: "enpassantfield" contains the number on the bit
	** board or INVALID if no en passant move possible
	*/
	field_t epfield;
	/*
	** next player to move and his enemy: WHITE or BLACK
	*/
	player_t player;
	player_t enemy;
	/*
	** castling status (what castling possiblilities are left): KQkq
	** (white king side, white queen side, black king side, black queen side)
	*/
	uint8 castlestate[4];
};
#define position struct position_t



/* macros to keep code more readable */
#define	ME				p->player
#define ENEMY			p->enemy



/* empty board (no pieces) */
extern void position_clear(position *p);
/* setup start position on board */
extern void position_reset(position *p);
/* read position info from string in Forsyth-Edwards Notation (FEN)
   return 1 on success, 0 in error case (the state of the board
   is undefined in case of error!) */
extern int position_readfen(position *p, const char *fen);

/* print current position to output stream */
extern void position_print(const position *p, FILE *stream);



#endif	/* __POSITION_H__ */
