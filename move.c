#include "move.h"

#include <string.h>

#include "globals.h"
#include "chess.h"
#include "board.h"
#include "lut.h"



#ifndef NDEBUG
	#define CHECKMOVECONSISTENCY(M)						\
	ASSERT((M)->type < NUMMOVETYPES);	 				\
	switch ((M)->type)									\
	{													\
		case MOVE:										\
		{												\
			ASSERT((M)->from   < NUMFIELDS);			\
			ASSERT((M)->to     < NUMFIELDS);			\
			ASSERT((M)->fpiece < NUMPIECES);			\
			break;										\
		}												\
		case CAPTURE:									\
		{												\
			ASSERT((M)->from   < NUMFIELDS);			\
			ASSERT((M)->to     < NUMFIELDS);			\
			ASSERT((M)->fpiece < NUMPIECES);			\
			ASSERT((M)->tpiece < NUMPIECES);			\
			break;										\
		}												\
		case ENPASSANT:									\
		{												\
			ASSERT((M)->from   < NUMFIELDS);			\
			ASSERT((M)->to     < NUMFIELDS);			\
			break;										\
		}												\
		case PROM_MOVE:									\
		{												\
			ASSERT((M)->from   < NUMFIELDS);			\
			ASSERT((M)->to     < NUMFIELDS);			\
			ASSERT((M)->promto < NUMPIECES);			\
			break;										\
		}												\
		case PROM_CAPTURE:								\
		{												\
			ASSERT((M)->from   < NUMFIELDS);			\
			ASSERT((M)->to     < NUMFIELDS);			\
			ASSERT((M)->tpiece < NUMPIECES);			\
			ASSERT((M)->promto < NUMPIECES);			\
			break;										\
		}												\
	}
#else
	#define CHECKMOVECONSISTENCY(M)
#endif



void move_clear(move *m)
{
	/* check parameters */
	ASSERT(m != (move *)0);

	/* reset move to invalid move */
	m->type		= NUMMOVETYPES;
	m->from		= 0;
	m->to		= 0;
	m->fpiece	= 0;
	m->tpiece	= 0;
	m->promto	= 0;
}



/* print move to output stream */
void move_print(FILE *stream, const move *m)
{
	/* check parameters */
	ASSERT(m      != (move *)0);
	ASSERT(stream != (FILE *)0);

	/* check ranges of move parameters */
	CHECKMOVECONSISTENCY(m);

	switch (m->type)
	{
		case MOVE:
		{
			PIECE_PRINT(stream, WHITE, m->fpiece);
			FLD_PRINT(stream, m->from);
			fprintf(stream, "-");
			FLD_PRINT(stream, m->to);
			break;
		}
		case CAPTURE:
		{
			PIECE_PRINT(stream, WHITE, m->fpiece);
			FLD_PRINT(stream, m->from);
			fprintf(stream, "x");
			FLD_PRINT(stream, m->to);
			fprintf(stream, " (");
			PIECE_PRINT(stream, WHITE, m->tpiece);
			fprintf(stream, " captured)");
			break;
		}
		case CASTLING_OO:
		{
			fprintf(stream, "O-O");
			break;
		}
		case CASTLING_OOO:
		{
			fprintf(stream, "O-O-O");
			break;
		}
		case ENPASSANT:
		{
			PIECE_PRINT(stream, WHITE, PAWN);
			FLD_PRINT(stream, m->from);
			fprintf(stream, "x");
			FLD_PRINT(stream, m->to);
			fprintf(stream, " (e.p.)");
			break;
		}
		case PROM_MOVE:
		{
			PIECE_PRINT(stream, WHITE, PAWN);
			FLD_PRINT(stream, m->from);
			fprintf(stream, "-");
			FLD_PRINT(stream, m->to);
			fprintf(stream, " =");
			PIECE_PRINT(stream, WHITE, m->promto);
			break;
		}
		case PROM_CAPTURE:
		{
			PIECE_PRINT(stream, WHITE, PAWN);
			FLD_PRINT(stream, m->from);
			fprintf(stream, "x");
			FLD_PRINT(stream, m->to);
			fprintf(stream, " =");
			PIECE_PRINT(stream, WHITE, m->promto);
			fprintf(stream, " (");
			PIECE_PRINT(stream, WHITE, m->tpiece);
			fprintf(stream, " captured)");
			break;
		}
	}
}



int move_read(move *m, const char *str)
{
	/* check parameters */
	ASSERT(m   != (move *)0);
	ASSERT(str != (const char *)0);

	/* castling moves ? */
	if (strcmp(str, "O-O") == 0)
	{
		m->type = CASTLING_OO;
		return(1);
	}
	else if (strcmp(str, "O-O-O") == 0)
	{
		m->type = CASTLING_OOO;
		return(1);
	}

	/* TODO */
}



/* apply move to chess board */
void move_apply_fast(position *p, player_t player, const move *m)
{
	int8 offset;
#ifndef NDEBUG
	player_t gplayer;
	piece_t  gpiece;
#endif


	/* check parameters */
	ASSERT(p != (position *)0);
	ASSERT(m != (move *)0);

	/* check ranges of move parameters */
	CHECKMOVECONSISTENCY(m);

	switch (m->type)
	{
		case MOVE:
		{
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, m->from, &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == m->fpiece);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, m->to,   &gpiece, &gplayer) == 0);
#endif
			board_fastclearfld(&p->b, m->from, m->fpiece, player);
			board_setfld(      &p->b, m->to,   m->fpiece, player);
			break;
		}
		case CAPTURE:
		{
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, m->from, &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == m->fpiece);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, m->to  , &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == m->tpiece);
			ASSERT(gplayer == OTHERPL(player));
#endif
			board_fastclearfld(&p->b, m->from, m->fpiece, player);
			board_fastclearfld(&p->b, m->to,   m->tpiece, OTHERPL(player));
			board_setfld(      &p->b, m->to,   m->fpiece, player);
			break;
		}
		case CASTLING_OO:
		{
			offset = (56 * player);
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, (field_t)(E1 - offset), &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == KING);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, (field_t)(F1 - offset), &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, (field_t)(G1 - offset), &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, (field_t)(H1 - offset), &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == ROOK);
			ASSERT(gplayer == player);
#endif
			board_fastclearfld(&p->b, (field_t)(E1 - offset), KING, player);
			board_fastclearfld(&p->b, (field_t)(H1 - offset), ROOK, player);
			board_setfld      (&p->b, (field_t)(G1 - offset), KING, player);
			board_setfld      (&p->b, (field_t)(F1 - offset), ROOK, player);
			break;
		}
		case CASTLING_OOO:
		{
			offset = (56 * player);
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, A1 - offset, &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == ROOK);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, (field_t)(B1 - offset), &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, (field_t)(C1 - offset), &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, (field_t)(D1 - offset), &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, (field_t)(E1 - offset), &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == KING);
			ASSERT(gplayer == player);
#endif
			board_fastclearfld(&p->b, (field_t)(E1 - offset), KING, player);
			board_fastclearfld(&p->b, (field_t)(A1 - offset), ROOK, player);
			board_setfld      (&p->b, (field_t)(C1 - offset), KING, player);
			board_setfld      (&p->b, (field_t)(D1 - offset), ROOK, player);
			break;
		}
		case ENPASSANT:
		{
			offset = 8 - (16 * player); 
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, m->from,                   &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == PAWN);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, (field_t)(m->to + offset), &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == PAWN);
			ASSERT(gplayer == OTHERPL(player));
			ASSERT(board_getfld(&p->b, m->to,                     &gpiece, &gplayer) == 0);
#endif
			board_fastclearfld(&p->b, m->from,                   PAWN, player);
			board_fastclearfld(&p->b, (field_t)(m->to + offset), PAWN, OTHERPL(player));
			board_setfld(      &p->b, m->to,                     PAWN, player);
			break;
		}
		case PROM_MOVE:
		{
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, m->from, &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == PAWN);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, m->to,   &gpiece, &gplayer) == 0);
#endif
			board_fastclearfld(&p->b, m->from, PAWN,      player);
			board_setfld(      &p->b, m->to,   m->promto, player);
			break;
		}
		case PROM_CAPTURE:
		{
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, m->from, &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == PAWN);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, m->to,   &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == m->tpiece);
			ASSERT(gplayer == OTHERPL(player));
#endif
			board_fastclearfld(&p->b, m->from, PAWN,      player);
			board_fastclearfld(&p->b, m->to,   m->tpiece, OTHERPL(player));
			board_setfld(      &p->b, m->to,   m->promto, player);
			break;
		}
	}
	board_refresh(&p->b);
}



/* rollback/undo move from chess board */
void move_rollback_fast(position *p, player_t player, const move *m)
{
	uint8 offset;
#ifndef NDEBUG
	player_t gplayer;
	piece_t  gpiece;
#endif


	/* check parameters */
	ASSERT(p != (position *)0);
	ASSERT(m != (move *)0);

	/* check ranges of move parameters */
	CHECKMOVECONSISTENCY(m);

	switch (m->type)
	{
		case MOVE:
		{
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, m->from, &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, m->to,   &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == m->fpiece);
			ASSERT(gplayer == player);
#endif
			board_fastclearfld(&p->b, m->to,   m->fpiece, player);
			board_setfld(      &p->b, m->from, m->fpiece, player);
			break;
		}
		case CAPTURE:
		{
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, m->from, &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, m->to,   &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == m->fpiece);
			ASSERT(gplayer == player);
#endif
			board_fastclearfld(&p->b, m->to,   m->fpiece, player);
			board_setfld(      &p->b, m->from, m->fpiece, player);
			board_setfld(      &p->b, m->to,   m->tpiece, OTHERPL(player));
			break;
		}
		case CASTLING_OO:
		{
			offset = (56 * player);
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, (field_t)(E1 - offset), &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, (field_t)(F1 - offset), &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == ROOK);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, (field_t)(G1 - offset), &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == KING);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, (field_t)(H1 - offset), &gpiece, &gplayer) == 0);
#endif
			board_fastclearfld(&p->b, (field_t)(G1 - offset), KING, player);
			board_fastclearfld(&p->b, (field_t)(F1 - offset), ROOK, player);
			board_setfld      (&p->b, (field_t)(E1 - offset), KING, player);
			board_setfld      (&p->b, (field_t)(H1 - offset), ROOK, player);
			break;
		}
		case CASTLING_OOO:
		{
			offset = (56 * player);
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, (field_t)(A1 - offset), &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, (field_t)(B1 - offset), &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, (field_t)(C1 - offset), &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == KING);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, (field_t)(D1 - offset), &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == ROOK);
			ASSERT(gplayer == player);
			ASSERT(board_getfld(&p->b, (field_t)(E1 - offset), &gpiece, &gplayer) == 0);
#endif
			board_fastclearfld(&p->b, (field_t)(C1 - offset), KING, player);
			board_fastclearfld(&p->b, (field_t)(D1 - offset), ROOK, player);
			board_setfld      (&p->b, (field_t)(E1 - offset), KING, player);
			board_setfld      (&p->b, (field_t)(A1 - offset), ROOK, player);
			break;
		}
		case ENPASSANT:
		{
			offset = 8 - (16 * player); 
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, m->from,                   &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, (field_t)(m->to + offset), &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, m->to,                     &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == PAWN);
			ASSERT(gplayer == player);
#endif
			board_fastclearfld(&p->b, m->to,                     PAWN, player);
			board_setfld(      &p->b, (field_t)(m->to + offset), PAWN, OTHERPL(player));
			board_setfld(      &p->b, m->from,                   PAWN, player);
			break;
		}
		case PROM_MOVE:
		{
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, m->from, &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, m->to,   &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == m->promto);
			ASSERT(gplayer == player);
#endif
			board_fastclearfld(&p->b, m->to,   m->promto, player);
			board_setfld(      &p->b, m->from, PAWN,      player);
			break;
		}
		case PROM_CAPTURE:
		{
#ifndef NDEBUG
			/* check move preconditions */
			ASSERT(board_getfld(&p->b, m->from, &gpiece, &gplayer) == 0);
			ASSERT(board_getfld(&p->b, m->to,   &gpiece, &gplayer) == 1);
			ASSERT(gpiece  == m->promto);
			ASSERT(gplayer == player);
#endif
			board_fastclearfld(&p->b, m->to,   m->promto, player);
			board_setfld(      &p->b, m->from, PAWN,      player);
			board_setfld(      &p->b, m->to,   m->tpiece, OTHERPL(player));
			break;
		}
	}
	board_refresh(&p->b);
}



/* macros to keep following code readable, functions would have too
   much overhead!(?) */
#define ALLPIECES		(p->b.allpieces[NORTH])
#define NOPIECES		(~(ALLPIECES))
#define EXTRACT_MMOV(FROM, TO, FPIECE)								\
	while (mov != 0)												\
	{																\
		CLEARNEXTBIT(mov, r);										\
		m->type   = MOVE;											\
		m->from   = (FROM);											\
		m->to     = (TO);											\
		m->fpiece = (FPIECE);										\
		move_apply_fast(p, ME, m);									\
		GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], kingpos);	\
		if (!board_attacked(&p->b, kingpos, ENEMY))					\
		{															\
			move_rollback_fast(p, ME, m);							\
			m = ML_NXT_FREE(*ml);									\
		}															\
		else														\
		{															\
			move_rollback_fast(p, ME, m);							\
		}															\
	}
#define EXTRACT_CMOV(FROM, TO, FPIECE)								\
	while (mov != 0)												\
	{																\
		CLEARNEXTBIT(mov, r);										\
		m->type   = CAPTURE;										\
		m->from   = (FROM);											\
		m->to     = (TO);											\
		m->fpiece = (FPIECE);										\
		m->tpiece = board_getpiece(&p->b, r);						\
		move_apply_fast(p, ME, m);									\
		GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], kingpos);	\
		if (!board_attacked(&p->b, kingpos, ENEMY))					\
		{															\
			move_rollback_fast(p, ME, m);							\
			m = ML_NXT_FREE(*ml);									\
		}															\
		else														\
		{															\
			move_rollback_fast(p, ME, m);							\
		}															\
	}
#define EXTRACT_PMMOV(FROM, TO)										\
	while (mov != 0)												\
	{																\
		CLEARNEXTBIT(mov, r);										\
		m->type   = PROM_MOVE;										\
		m->from   = (FROM);											\
		m->to     = (TO);											\
		m->promto = QUEEN;											\
		move_apply_fast(p, ME, m);									\
		GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], kingpos);	\
		if (!board_attacked(&p->b, kingpos, ENEMY))					\
		{															\
			move_rollback_fast(p, ME, m);							\
			n = m;													\
			m = ML_NXT_FREE(*ml);									\
			memcpy(m, n, sizeof(move));								\
			m->promto = KNIGHT;										\
			m = ML_NXT_FREE(*ml);									\
			memcpy(m, n, sizeof(move));								\
			m->promto = ROOK;										\
			m = ML_NXT_FREE(*ml);									\
			memcpy(m, n, sizeof(move));								\
			m->promto = BISHOP;										\
			m = ML_NXT_FREE(*ml);									\
		}															\
		else														\
		{															\
			move_rollback_fast(p, ME, m);							\
		}															\
	}
#define EXTRACT_PCMOV(FROM, TO)										\
	while (mov != 0)												\
	{																\
		CLEARNEXTBIT(mov, r);										\
		m->type   = PROM_CAPTURE;									\
		m->from   = (FROM);											\
		m->to     = (TO);											\
		m->tpiece = board_getpiece(&p->b, r);						\
		m->promto = QUEEN;											\
		move_apply_fast(p, ME, m);									\
		GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], kingpos);	\
		if (!board_attacked(&p->b, kingpos, ENEMY))					\
		{															\
			move_rollback_fast(p, ME, m);							\
			n = m;													\
			m = ML_NXT_FREE(*ml);									\
			memcpy(m, n, sizeof(move));								\
			m->promto = KNIGHT;										\
			m = ML_NXT_FREE(*ml);									\
			memcpy(m, n, sizeof(move));								\
			m->promto = ROOK;										\
			m = ML_NXT_FREE(*ml);									\
			memcpy(m, n, sizeof(move));								\
			m->promto = BISHOP;										\
			m = ML_NXT_FREE(*ml);									\
		}															\
		else														\
		{															\
			move_rollback_fast(p, ME, m);							\
		}															\
	}

/* generate all legal moves to current position on board */
int move_generate(position *p, movelist *ml)
{
	uint64 mask, mov, t;
	field_t q, r, kingpos;
	move *m, *n;


	/* check parameters */
	ASSERT(p  != (const position *)0);
	ASSERT(ml != (movelist *)0);

	CHECKBOARDCONSISTENCY(&p->b);

	/* prepare empty movelist */
	ML_CLEAR(*ml);
	m = ML_NXT_FREE(*ml);

	/*
	*******
	* PAWN
	*******
	*/
	if (ME == WHITE)
	{
		/* first check if there are any pawns of this color at all */
		if ((mask = p->b.pieces[PAWN] & p->b.occupied[WHITE]) != 0)
		{
			/* move all pawns one step forward on empty fields */
			mask = (mask >> 8) & NOPIECES;
			/* target rows 3-7: pawn single step moves */
			mov = mask & (ROW_3 | ROW_4 | ROW_5 | ROW_6 | ROW_7);
			EXTRACT_MMOV(r + 8, r, PAWN);
			/* target row 3: pawn double step moves */
			mov = ((mask & ROW_3) >> 8) & NOPIECES;
			EXTRACT_MMOV(r + 16, r, PAWN);
			/* target row 8: pawn promotion moves */
			mov = mask & ROW_8;
			EXTRACT_PMMOV(r + 8, r);

			/* side step of all pawns from B-H to A-G */
			mask = ((p->b.pieces[PAWN] & p->b.occupied[WHITE] & ~COL_A) >> 7) & p->b.occupied[BLACK];
			/* target rows 3-7: pawn capture moves */
			mov = mask & (ROW_3 | ROW_4 | ROW_5 | ROW_6 | ROW_7);
			EXTRACT_CMOV(r + 7, r, PAWN);
			/* target row 8: pawn promotion capture moves */
			mov = mask & ROW_8;
			EXTRACT_PCMOV(r + 7, r);

			/* side step of all pawns from A-G to B-H */
			mask = ((p->b.pieces[PAWN] & p->b.occupied[WHITE] & ~COL_H) >> 9) & p->b.occupied[BLACK];
			/* target rows 3-7: pawn capture moves */
			mov = mask & (ROW_3 | ROW_4 | ROW_5 | ROW_6 | ROW_7);
			EXTRACT_CMOV(r + 9, r, PAWN);
			/* target row 8: pawn promotion capture moves */
			mov = mask & ROW_8;
			EXTRACT_PCMOV(r + 9, r);
		}
	}
	else
	{
		/* first check if there are any pawns of this color at all */
		if ((mask = p->b.pieces[PAWN] & p->b.occupied[BLACK]) != 0)
		{
			/* move all pawns one step forward on empty fields */
			mask = (mask << 8) & NOPIECES;
			/* target rows 6-2: pawn single step moves */
			mov = mask & (ROW_6 | ROW_5 | ROW_4 | ROW_3 | ROW_2);
			EXTRACT_MMOV(r - 8, r, PAWN);
			/* target row 6: pawn double step moves */
			mov = ((mask & ROW_6) << 8) & NOPIECES;
			EXTRACT_MMOV(r - 16, r, PAWN);
			/* target row 1: pawn promotion moves */
			mov = mask & ROW_1;
			EXTRACT_PMMOV(r - 8, r);

			/* side step of all pawns from B-H to A-G */
			mask = ((p->b.pieces[PAWN] & p->b.occupied[BLACK] & ~COL_A) << 9) & p->b.occupied[WHITE];
			/* target rows 6-2: pawn capture moves */
			mov = mask & (ROW_6 | ROW_5 | ROW_4 | ROW_3 | ROW_2);
			EXTRACT_CMOV(r - 9, r, PAWN);
			/* target row 1: pawn promotion capture moves */
			mov = mask & ROW_1;
			EXTRACT_PCMOV(r - 9, r);

			/* side step of all pawns from A-G to B-H */
			mask = ((p->b.pieces[PAWN] & p->b.occupied[BLACK] & ~COL_H) << 7) & p->b.occupied[WHITE];
			/* target rows 6-2: pawn capture moves */
			mov = mask & (ROW_6 | ROW_5 | ROW_4 | ROW_3 | ROW_2);
			EXTRACT_CMOV(r - 7, r, PAWN);
			/* target row 1: pawn promotion capture moves */
			mov = mask & ROW_1;
			EXTRACT_PCMOV(r - 7, r);
		}
	}

	/*
	**********
	* KNIGHT
	*********
	*/
	/* generate knight moves and capture moves using lookup table */
	t = p->b.pieces[KNIGHT] & p->b.occupied[ME];
	/* extract moves */
	while (t != 0)
	{
		/* isolate all my knights */
		CLEARNEXTBIT(t, q);
		/* generate move bitmap */
		mask = board_mask_n(q);
		/* moves */
		mov = mask & NOPIECES;
		EXTRACT_MMOV(q, r, KNIGHT);
		/* capture moves */
		mov = mask & p->b.occupied[ENEMY];
		EXTRACT_CMOV(q, r, KNIGHT);
	}

	/*
	***************
	* BISHOP/QUEEN
	***************
	*/
	/* generate bishop and diagonal queen moves */
	t = (p->b.pieces[BISHOP] | p->b.pieces[QUEEN]) & p->b.occupied[ME];
	while (t != 0)
	{
		/* isolate all my bishops and queens */
		CLEARNEXTBIT(t, q);
		/* generate move bitmap */
		mask = board_mask_b(&p->b, q);
		/* moves */
		mov = mask & NOPIECES;
		EXTRACT_MMOV(q, r, (TESTBIT(p->b.pieces[QUEEN], q) ? QUEEN : BISHOP));
		/* capture moves */
		mov = mask & p->b.occupied[ENEMY];
		EXTRACT_CMOV(q, r, (TESTBIT(p->b.pieces[QUEEN], q) ? QUEEN : BISHOP));
	}

	/*
	*************
	* ROOK/QUEEN
	*************
	*/
	/* generate rook and (horizontal and vertical) queen moves */
	t = (p->b.pieces[ROOK] | p->b.pieces[QUEEN]) & p->b.occupied[ME];
	while (t != 0)
	{
		/* isolate all my rooks and queens */
		CLEARNEXTBIT(t, q);
		/* generate move bitmap */
		mask = board_mask_r(&p->b, q);
		/* moves */
		mov = mask & NOPIECES;
		EXTRACT_MMOV(q, r, (TESTBIT(p->b.pieces[QUEEN], q) ? QUEEN : ROOK));
		/* capture moves */
		mov = mask & p->b.occupied[ENEMY];
		EXTRACT_CMOV(q, r, (TESTBIT(p->b.pieces[QUEEN], q) ? QUEEN : ROOK));
	}

	/*
	*******
	* KING
	*******
	*/
	/* there is only one king to be isolated... */
	GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], q);
	/* generate move bitmap */
	mask = board_mask_k(q);
	/* moves */
	mov = mask & NOPIECES;
	EXTRACT_MMOV(q, r, KING);
	/* capture moves */
	mov = mask & p->b.occupied[ENEMY];
	EXTRACT_CMOV(q, r, KING);

	ML_UNDO(*ml);

	/* no moves in movelist? */
	if (ML_EMPTY(*ml))
	{
		GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], kingpos);
		if (!board_attacked(&p->b, kingpos, ENEMY))
		{
			return(STALEMATE);
		}
		else
		{
			return(CHECKMATE);
		}
	}
	return(MOVESLEFT);
}



#define EXIST_MMOV(FROM, TO, FPIECE)								\
	while (mov != 0)												\
	{																\
		CLEARNEXTBIT(mov, r);										\
		m.type   = MOVE;											\
		m.from   = (FROM);											\
		m.to     = (TO);											\
		m.fpiece = (FPIECE);										\
		move_apply_fast(p, ME, &m);									\
		GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], kingpos);	\
		if (!board_attacked(&p->b, kingpos, ENEMY))					\
		{															\
			move_rollback_fast(p, ME, &m);							\
			return(MOVESLEFT);										\
		}															\
		else														\
		{															\
			move_rollback_fast(p, ME, &m);							\
		}															\
	}
#define EXIST_CMOV(FROM, TO, FPIECE)								\
	while (mov != 0)												\
	{																\
		CLEARNEXTBIT(mov, r);										\
		m.type   = CAPTURE;											\
		m.from   = (FROM);											\
		m.to     = (TO);											\
		m.fpiece = (FPIECE);										\
		m.tpiece = board_getpiece(&p->b, r);						\
		move_apply_fast(p, ME, &m);									\
		GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], kingpos);	\
		if (!board_attacked(&p->b, kingpos, ENEMY))					\
		{															\
			move_rollback_fast(p, ME, &m);							\
			return(MOVESLEFT);										\
		}															\
		else														\
		{															\
			move_rollback_fast(p, ME, &m);							\
		}															\
	}
#define EXIST_PMMOV(FROM, TO)										\
	while (mov != 0)												\
	{																\
		CLEARNEXTBIT(mov, r);										\
		m.type   = PROM_MOVE;										\
		m.from   = (FROM);											\
		m.to     = (TO);											\
		m.promto = QUEEN;											\
		move_apply_fast(p, ME, &m);									\
		GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], kingpos);	\
		if (!board_attacked(&p->b, kingpos, ENEMY))					\
		{															\
			move_rollback_fast(p, ME, &m);							\
			return(MOVESLEFT);										\
		}															\
		else														\
		{															\
			move_rollback_fast(p, ME, &m);							\
		}															\
	}
#define EXIST_PCMOV(FROM, TO)										\
	while (mov != 0)												\
	{																\
		CLEARNEXTBIT(mov, r);										\
		m.type   = PROM_CAPTURE;									\
		m.from   = (FROM);											\
		m.to     = (TO);											\
		m.tpiece = board_getpiece(&p->b, r);						\
		m.promto = QUEEN;											\
		move_apply_fast(p, ME, &m);									\
		GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], kingpos);	\
		if (!board_attacked(&p->b, kingpos, ENEMY))					\
		{															\
			move_rollback_fast(p, ME, &m);							\
		}															\
		else														\
		{															\
			move_rollback_fast(p, ME, &m);							\
			return(MOVESLEFT);										\
		}															\
	}

int move_movesleft(position *p)
{
	uint64 mask, mov, t;
	field_t q, r, kingpos;
	move m;


	/* check parameters */
	ASSERT(p  != (const position *)0);

	CHECKBOARDCONSISTENCY(&p->b);

	/*
	*******
	* PAWN
	*******
	*/
	if (ME == WHITE)
	{
		/* first check if there are any pawns of this color at all */
		if ((mask = p->b.pieces[PAWN] & p->b.occupied[WHITE]) != 0)
		{
			/* move all pawns one step forward on empty fields */
			mask = (mask >> 8) & NOPIECES;
			/* target rows 3-7: pawn single step moves */
			mov = mask & (ROW_3 | ROW_4 | ROW_5 | ROW_6 | ROW_7);
			EXIST_MMOV(r + 8, r, PAWN);
			/* target row 3: pawn double step moves */
			mov = ((mask & ROW_3) >> 8) & NOPIECES;
			EXIST_MMOV(r + 16, r, PAWN);
			/* target row 8: pawn promotion moves */
			mov = mask & ROW_8;
			EXIST_PMMOV(r + 8, r);

			/* side step of all pawns from B-H to A-G */
			mask = ((p->b.pieces[PAWN] & p->b.occupied[WHITE] & ~COL_A) >> 7) & p->b.occupied[BLACK];
			/* target rows 3-7: pawn capture moves */
			mov = mask & (ROW_3 | ROW_4 | ROW_5 | ROW_6 | ROW_7);
			EXIST_CMOV(r + 7, r, PAWN);
			/* target row 8: pawn promotion capture moves */
			mov = mask & ROW_8;
			EXIST_PCMOV(r + 7, r);

			/* side step of all pawns from A-G to B-H */
			mask = ((p->b.pieces[PAWN] & p->b.occupied[WHITE] & ~COL_H) >> 9) & p->b.occupied[BLACK];
			/* target rows 3-7: pawn capture moves */
			mov = mask & (ROW_3 | ROW_4 | ROW_5 | ROW_6 | ROW_7);
			EXIST_CMOV(r + 9, r, PAWN);
			/* target row 8: pawn promotion capture moves */
			mov = mask & ROW_8;
			EXIST_PCMOV(r + 9, r);
		}
	}
	else
	{
		/* first check if there are any pawns of this color at all */
		if ((mask = p->b.pieces[PAWN] & p->b.occupied[BLACK]) != 0)
		{
			/* move all pawns one step forward on empty fields */
			mask = (mask << 8) & NOPIECES;
			/* target rows 6-2: pawn single step moves */
			mov = mask & (ROW_6 | ROW_5 | ROW_4 | ROW_3 | ROW_2);
			EXIST_MMOV(r - 8, r, PAWN);
			/* target row 6: pawn double step moves */
			mov = ((mask & ROW_6) << 8) & NOPIECES;
			EXIST_MMOV(r - 16, r, PAWN);
			/* target row 1: pawn promotion moves */
			mov = mask & ROW_1;
			EXIST_PMMOV(r - 8, r);

			/* side step of all pawns from B-H to A-G */
			mask = ((p->b.pieces[PAWN] & p->b.occupied[BLACK] & ~COL_A) << 9) & p->b.occupied[WHITE];
			/* target rows 6-2: pawn capture moves */
			mov = mask & (ROW_6 | ROW_5 | ROW_4 | ROW_3 | ROW_2);
			EXIST_CMOV(r - 9, r, PAWN);
			/* target row 1: pawn promotion capture moves */
			mov = mask & ROW_1;
			EXIST_PCMOV(r - 9, r);

			/* side step of all pawns from A-G to B-H */
			mask = ((p->b.pieces[PAWN] & p->b.occupied[BLACK] & ~COL_H) << 7) & p->b.occupied[WHITE];
			/* target rows 6-2: pawn capture moves */
			mov = mask & (ROW_6 | ROW_5 | ROW_4 | ROW_3 | ROW_2);
			EXIST_CMOV(r - 7, r, PAWN);
			/* target row 1: pawn promotion capture moves */
			mov = mask & ROW_1;
			EXIST_PCMOV(r - 7, r);
		}
	}

	/*
	**********
	* KNIGHT
	*********
	*/
	/* generate knight moves and capture moves using lookup table */
	t = p->b.pieces[KNIGHT] & p->b.occupied[ME];
	/* extract moves */
	while (t != 0)
	{
		/* isolate all my knights */
		CLEARNEXTBIT(t, q);
		/* generate move bitmap */
		mask = board_mask_n(q);
		/* moves */
		mov = mask & NOPIECES;
		EXIST_MMOV(q, r, KNIGHT);
		/* capture moves */
		mov = mask & p->b.occupied[ENEMY];
		EXIST_CMOV(q, r, KNIGHT);
	}

	/*
	***************
	* BISHOP/QUEEN
	***************
	*/
	/* generate bishop and diagonal queen moves */
	t = (p->b.pieces[BISHOP] | p->b.pieces[QUEEN]) & p->b.occupied[ME];
	while (t != 0)
	{
		/* isolate all my bishops and queens */
		CLEARNEXTBIT(t, q);
		/* generate move bitmap */
		mask = board_mask_b(&p->b, q);
		/* moves */
		mov = mask & NOPIECES;
		EXIST_MMOV(q, r, (TESTBIT(p->b.pieces[QUEEN], q) ? QUEEN : BISHOP));
		/* capture moves */
		mov = mask & p->b.occupied[ENEMY];
		EXIST_CMOV(q, r, (TESTBIT(p->b.pieces[QUEEN], q) ? QUEEN : BISHOP));
	}

	/*
	*************
	* ROOK/QUEEN
	*************
	*/
	/* generate rook and (horizontal and vertical) queen moves */
	t = (p->b.pieces[ROOK] | p->b.pieces[QUEEN]) & p->b.occupied[ME];
	while (t != 0)
	{
		/* isolate all my rooks and queens */
		CLEARNEXTBIT(t, q);
		/* generate move bitmap */
		mask = board_mask_r(&p->b, q);
		/* moves */
		mov = mask & NOPIECES;
		EXIST_MMOV(q, r, (TESTBIT(p->b.pieces[QUEEN], q) ? QUEEN : ROOK));
		/* capture moves */
		mov = mask & p->b.occupied[ENEMY];
		EXIST_CMOV(q, r, (TESTBIT(p->b.pieces[QUEEN], q) ? QUEEN : ROOK));
	}

	/*
	*******
	* KING
	*******
	*/
	/* there is only one king to be isolated... */
	GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], q);
	/* generate move bitmap */
	mask = board_mask_k(q);
	/* moves */
	mov = mask & NOPIECES;
	EXIST_MMOV(q, r, KING);
	/* capture moves */
	mov = mask & p->b.occupied[ENEMY];
	EXIST_CMOV(q, r, KING);

	/* no moves found: checkmate or stalemate ? */
	GETNEXTBIT(p->b.pieces[KING] & p->b.occupied[ME], kingpos);
	if (!board_attacked(&p->b, kingpos, ENEMY))
	{
		return(STALEMATE);
	}
	else
	{
		return(CHECKMATE);
	}
}



void move_apply(position *p, const move *m)
{
	move_apply_fast(p, ME, m);
	ME = ENEMY;
	ENEMY = OTHERPL(ENEMY);
}



void move_rollback(position *p, const move *m)
{
	ME = ENEMY;
	ENEMY = OTHERPL(ENEMY);
	move_rollback_fast(p, ME, m);
}

