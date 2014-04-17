#ifndef __BOARD_H__
#define __BOARD_H__



#include <stdio.h>

#include "globals.h"
#include "rotate.h"



struct board_t
{
	/*
	** fields occupied with either white pieces or black pieces
	** are marked with a "1", empty fields with "0"
	*/
	uint64 occupied[NUMPLAYERS];
	/*
	** fields occupied with white pieces or black pieces,
	** saved in different directions: NORTH, WEST, NORTHWEST, NORTHEAST;
	** (be careful: the NORTH component is valid after call of board_refresh)
	*/
	uint64 allpieces[NUMDIRECTIONS];
	/*
	** fields with a special type of piece (black or white) on them
	** are marked with a "1",  empty fields with "0"
	*/
	uint64 pieces[NUMPIECES];
};
#define board struct board_t



/* enough tripwire to catch inconsistent board states... */
#ifndef NDEBUG
	extern void check_board_consistency(const board *b);
	#define CHECKBOARDCONSISTENCY(B)	check_board_consistency(B);
#else
	#define CHECKBOARDCONSISTENCY(B)
#endif



/* set piece on specific field on board */
extern void board_setfld(board *b, field_t fld, piece_t piece, player_t player);
/* clear specific field on board: much faster, if we have information
   about the piece on this field */
extern void board_fastclearfld(board *b, field_t fld, piece_t piece, player_t player);
/* clear specific field on board */
extern void board_clearfld(board *b, field_t fld);
/* get piece type and owning player of a piece on field fld;
   returns 1, if field is occupied, 0 otherwise */
extern int board_getfld(const board *b, field_t fld, piece_t *piece, player_t *player);
/* get piece type of piece at field fld */
extern piece_t board_getpiece(const board *b, field_t fld);
/* recalculates some values, should be called after every changes */
extern void board_refresh(board *b);

/* empty board (no pieces) */
extern void board_clear(board *b);
/* setup start field on board */
extern void board_reset(board *b);
/* read board info from string in Forsyth-Edwards Notation (FEN)
   return 1 on success, 0 in error case (the state of the board
   is undefined in case of error!) */
extern int board_readfen(board *b, const char *fen, const char **endpos);

/* print current board to output stream */
extern void board_print(const board *b, FILE *stream);

/*
** the following functions and macros return to a specific
** board configuration a bitmask, containing information about
** piece movement; to get moves or capture moves, calculate:
**
** normal moves:  mask & NOPIECES;
** capture moves: mask & b->occupied[ENEMY];
*/
#define board_mask_n(POS)		lut_knight[(POS)]
extern uint64 board_mask_b(const board *b, field_t fld);
extern uint64 board_mask_r(const board *b, field_t fld);
#define board_mask_k(POS)		lut_king[(POS)]

/* return 1, if field is attacked by any piece of player, otherwise 0 */
extern int board_attacked(const board *b, field_t fld, player_t player);



#endif	/* __BOARD_H__ */
