#include "board.h"

#include <ctype.h>
#include <string.h>

#include "lut.h"



/* macros to keep following code more readable */
#define ALLPIECES		b->allpieces[NORTH]
#define NOPIECES		~ALLPIECES



#ifndef NDEBUG
void check_board_consistency(const board *b)
{
	uint64 mask1, mask2;
	field_t h;
	int i, j;

	/* fields occupied by either WHITE or BLACK: should be all fields */
	ASSERT(ALLPIECES == (b->occupied[WHITE] ^ b->occupied[BLACK]));

	/* correct rotations of board ? */
	mask1 = b->allpieces[WEST];
	ROTATE_BOARD(mask1, mask2, h, ROT_W_N);
	ASSERT(b->allpieces[NORTH] == mask2);
	mask1 = b->allpieces[NORTHWEST];
	ROTATE_BOARD(mask1, mask2, h, ROT_NW_N);
	ASSERT(b->allpieces[NORTH] == mask2);
	mask1 = b->allpieces[NORTHEAST];
	ROTATE_BOARD(mask1, mask2, h, ROT_NE_N);
	ASSERT(b->allpieces[NORTH] == mask2);

	/* every piece on board has at least one type */
	for (mask2 = 0, i = 0; i < NUMPIECES; i++)
	{
		mask2 |= b->pieces[i];
	}
	ASSERT(ALLPIECES == mask2);

	/* every piece on board has no more than one type */
	for (i = 1; i < NUMPIECES; i++)
	{
		for (j = 0; j < i; j++)
		{
			ASSERT((b->pieces[i] & b->pieces[j]) == 0);
		}
	}
}
#endif



/* set piece on specific field on board */
void board_setfld(board *b, field_t fld, piece_t piece, player_t player)
{
	uint64 mask;


	/* check parameters */
	ASSERT(b != (board *)0);
	ASSERT(fld    < NUMFIELDS);
	ASSERT(piece  < NUMPIECES);
	ASSERT(player < NUMPLAYERS);

	mask = BITMASK(fld);
	b->pieces[piece]    |= mask;
	b->occupied[player] |= mask;

	SETBIT(b->allpieces[WEST],      ROTATE_POS(ROT_N_W,  fld));
	SETBIT(b->allpieces[NORTHWEST], ROTATE_POS(ROT_N_NW, fld));
	SETBIT(b->allpieces[NORTHEAST], ROTATE_POS(ROT_N_NE, fld));
}



/* clear specific field on board: much faster, if we have information
   about the piece and owning player at this field */
void board_fastclearfld(board *b, field_t fld, piece_t piece, player_t player)
{
	uint64 mask;


	/* check parameters */
	ASSERT(b != (board *)0);
	ASSERT(fld    < NUMFIELDS);
	ASSERT(piece  < NUMPIECES);
	ASSERT(player < NUMPLAYERS);

	mask = ~BITMASK(fld);
	b->pieces[piece]    &= mask;
	b->occupied[player] &= mask;

	CLEARBIT(b->allpieces[WEST],      ROTATE_POS(ROT_N_W,  fld));
	CLEARBIT(b->allpieces[NORTHWEST], ROTATE_POS(ROT_N_NW, fld));
	CLEARBIT(b->allpieces[NORTHEAST], ROTATE_POS(ROT_N_NE, fld));
}



/* clear specific field on board */
void board_clearfld(board *b, field_t fld)
{
	uint64 mask;
	int i;


	/* check parameters */
	ASSERT(b != (board *)0);
	ASSERT(fld < NUMFIELDS);

	/* we do not know the piece: clear all pieces at this field */
	mask = ~BITMASK(fld);
	for (i = 0; i < NUMPIECES; i++)
	{
		b->pieces[i] &= mask;
	}
	/* we do not know the player color: clear pieces of every color */
	b->occupied[WHITE] &= mask;
	b->occupied[BLACK] &= mask;

	CLEARBIT(b->allpieces[WEST],      ROTATE_POS(ROT_N_W,  fld));
	CLEARBIT(b->allpieces[NORTHWEST], ROTATE_POS(ROT_N_NW, fld));
	CLEARBIT(b->allpieces[NORTHEAST], ROTATE_POS(ROT_N_NE, fld));
}



/* get piece type and owning player of a piece on field fld;
   returns 1, if field is occupied, 0 otherwise */
int board_getfld(const board *b, field_t fld, piece_t *piece, player_t *player)
{
	int i;


	/* check parameters */
	ASSERT(b != (board *)0);
	ASSERT(fld    < NUMFIELDS);
	ASSERT(piece  != (piece_t  *)0);
	ASSERT(player != (player_t *)0);

	/* determine piece color */
	if (TESTBIT(b->occupied[WHITE], fld))
	{
		*player = WHITE;
	}
	else if (TESTBIT(b->occupied[BLACK], fld))
	{
		*player = BLACK;
	}
	else
	{
		/* field is empty */
		return(0);
	}

	/* determine piece type */
	for (i = 0; i < NUMPIECES; i++)
	{
		if (TESTBIT(b->pieces[i], fld))
		{
			*piece = i;
			return(1);
		}
	}

	/* if this assertion triggers, we have found an piece in
	   b->occupied, but no entry in p->pieces and therefore
	   a board inconsistency! */
	ASSERTMSG(0, "unable to determine piece type");

	/* make compiler happy */
	return(0);
}



/* get piece type of piece at field fld */
piece_t board_getpiece(const board *b, field_t fld)
{
	int i;


	/* check parameters */
	ASSERT(b != (board *)0);
	ASSERT(fld    < NUMFIELDS);

	/* determine piece type */
	for (i = 0; i < NUMPIECES; i++)
	{
		if (TESTBIT(b->pieces[i], fld))
		{
			return(i);
		}
	}
	/* piece not found: no piece at fld ? */
	ASSERTMSG(0, "unable to determine piece type");
	/* make compiler happy */
	return(INVALID);
}



/* recalculates some values, should be called after every changes */
void board_refresh(board *b)
{
	/* calculate often used value */
	ALLPIECES = b->occupied[WHITE] | b->occupied[BLACK];

	CHECKBOARDCONSISTENCY(b);
}



/* empty board (no pieces) */
void board_clear(board *b)
{
	field_t i;


	/* check parameters */
	ASSERT(b != (board *)0);

	/* clear board */
	for (i = 0; i < NUMFIELDS; i++)
	{
		board_clearfld(b, i);
	}

	board_refresh(b);
}



/* setup start situation on board */
void board_reset(board *b)
{
	field_t i;


	/* check parameters */
	ASSERT(b != (board *)0);

	/* clear board */
	for (i = 0; i < NUMFIELDS; i++)
	{
		board_clearfld(b, i);
	}

	/* set up board: white pieces */
	board_setfld(b, A1, ROOK,   WHITE);
	board_setfld(b, B1, KNIGHT, WHITE);
	board_setfld(b, C1, BISHOP, WHITE);
	board_setfld(b, D1, QUEEN,  WHITE);
	board_setfld(b, E1, KING,   WHITE);
	board_setfld(b, F1, BISHOP, WHITE);
	board_setfld(b, G1, KNIGHT, WHITE);
	board_setfld(b, H1, ROOK,   WHITE);
	for (i = A2; i >= H2; i--)
	{
		board_setfld(b, i, PAWN, WHITE);
	}

	/* set up board: black pieces */
	board_setfld(b, A8, ROOK,   BLACK);
	board_setfld(b, B8, KNIGHT, BLACK);
	board_setfld(b, C8, BISHOP, BLACK);
	board_setfld(b, D8, QUEEN,  BLACK);
	board_setfld(b, E8, KING,   BLACK);
	board_setfld(b, F8, BISHOP, BLACK);
	board_setfld(b, G8, KNIGHT, BLACK);
	board_setfld(b, H8, ROOK,   BLACK);
	for (i = A7; i >= H7; i--)
	{
		board_setfld(b, i, PAWN, BLACK);
	}

	board_refresh(b);
}



/* read board info from string in Forsyth-Edwards Notation (FEN)
   return 1 on success, 0 in error case (the state of the board
   is undefined in case of error!) */
int board_readfen(board *b, const char *fen, const char **endpos)
{
	unsigned int n, i;
	field_t fld;


	/* check parameters */
	ASSERT(b      != (board *)0);
	ASSERT(fen    != (const char *)0);
	ASSERT(endpos != (const char **)0);

	/* clear board (and set other game parameters as well) */
	board_clear(b);

	/* get board setup */
	fld = A8;
	n = 0;
	while (1)
	{
		/* found a piece */
		if (isalpha(fen[n]))
		{
			/* search letter found at position in piece names */
			i = 0;
			while(1)
			{
				if (i == strlen(piecenames[WHITE]))
				{
					/* found: unknown character in fen board definition */
					return(0);
				}
				if (piecenames[WHITE][i] == toupper(fen[n]))
				{
					/* found: set piece definition and stop further searching */
					board_setfld(b, fld, (piece_t)i, (player_t)(isupper(fen[n]) ? WHITE : BLACK));
					break;
				}
				i++;
			}
			fld--;
		}
		/* found a counter of empty fields */
		else if (isdigit(fen[n]))
		{
			fld -= (fen[n] - '0');
		}
		/* found a separator */
		else if (fen[n] == '/')
		{
			if (((fld + 1) & 0x7) != 0)
			{
				/* hitting a separator fld+1 must be in (0, 8, 16, ..., 48) */
				return(0);
			}
			else
			{
				fld += 16;
			}
		}
		/* found end of board setup */
		else if (fen[n] == ' ')
		{
			if (fld + 1 != H1)
			{
				/* board setup incomplete */
				return(0);
			}
			else
			{
				/* leave board setup and go on */
				break;
			}
		}
		/* found an error */
		else
		{
			return(0);
		}
		n++;
	}

	board_refresh(b);
	
	/* return with success */
	*endpos = (char *)&fen[n];
	return(1);
}



/* print current board to output stream */
void board_print(const board *b, FILE *stream)
{
	field_t i, j;
	piece_t piece;
	player_t player;


	/* check parameters */
	ASSERT(b != (board *)0);
	ASSERT(stream != (FILE *)0);

	CHECKBOARDCONSISTENCY(b);

	/* print header line */
	fprintf(stream, "\n  -----------------\n");

	/* print chess field */
	for (i = 7; i < NUMFIELDS; i += 8)
	{
		fprintf(stream, "%i| ", 8 - (i >> 3));
		for (j = 0; j < 8; j++)
		{
			if (board_getfld(b, (field_t)(i - j), &piece, &player) == 1)
			{
				PIECE_PRINT(stream, player, piece);
				fprintf(stream, " ");
			}
			else
			{
				fprintf(stream, ". ");
			}
		}
		fprintf(stream, "|\n");
	}

	/* print footer lines */
	fprintf(stream, "  -----------------\n");
	fprintf(stream, "   A B C D E F G H\n\n");
}



/*
** the following functions return to a specific
** board configuration a bitmask, containing information about
** piece movement; to get moves or capture moves, calculate:
**
** normal moves:  mask & NOPIECES;
** capture moves: mask & b->occupied[ENEMY];
*/
uint64 board_mask_b(const board *b, field_t fld)
{
	field_t q, r, s;
	uint64 t, u, v;


	/* check parameters */
	ASSERT(b != (const board *)0);
	ASSERT(fld < NUMFIELDS);

	/* diagonal A8-H1 */
	q = ROTATE_POS(ROT_N_NW, fld);
	r = diag_valid_bits[q];
	s = diag_shift[q];
	u = (((uint64)ROOKLUT_ENTRY(((b->allpieces[NORTHWEST]) >> s) & r, q - s)) & r) << s;
	ROTATE_BOARD(u, t, r, ROT_NW_N);
	/* diagonal A1-H8 */
	q = ROTATE_POS(ROT_N_NE, fld);
	r = diag_valid_bits[q];
	s = diag_shift[q];
	u = (((uint64)ROOKLUT_ENTRY(((b->allpieces[NORTHEAST]) >> s) & r, q - s)) & r) << s;
	ROTATE_BOARD(u, v, r, ROT_NE_N);

	return(t | v);
}

uint64 board_mask_r(const board *b, field_t fld)
{
	field_t r;
	uint64 t, u, v;


	/* check parameters */
	ASSERT(b  != (const board *)0);
	ASSERT(fld    < NUMFIELDS);

	/* horizontal moves */
	r = fld & ~7;
	t = ((uint64)ROOKLUT_ENTRY((b->allpieces[NORTH] >> r) & UINT64CONST(0xff), (fld & 7))) << r;
	/* vertical moves */
	fld = ROTATE_POS(ROT_N_W, fld);
	r = fld & ~7;
	u = ((uint64)ROOKLUT_ENTRY(((b->allpieces[WEST]) >> r) & UINT64CONST(0xff), (fld & 7))) << r;
	/* rotate board with vertical moves back and merge the two boards */
	ROTATE_BOARD(u, v, r, ROT_W_N);

	return(t | v);
}



/* return 1, if field is attacked by any piece of player, otherwise 0 */
int board_attacked(const board *b, field_t fld, player_t player)
{
	uint64 mask;


	/* check parameters */
	ASSERT(b != (board *)0);
	CHECKBOARDCONSISTENCY(b);
	ASSERT(fld    < NUMFIELDS);
	ASSERT(player < NUMPLAYERS);

	/* convert field to bitmask */
	mask = BITMASK(fld);

	/* is WHITE the attacker ? */
	if (player == WHITE)
	{
		/* is there a white pawn on column B-H attacking field ? */
		if ((((b->pieces[PAWN] & b->occupied[WHITE] & ~COL_A) >> 7) & mask) != 0)
		{
			 return(1);
		}
		/* is there a white pawn on column A-G attacking field ? */
		if ((((b->pieces[PAWN] & b->occupied[WHITE] & ~COL_H) >> 9) & mask) != 0)
		{
			return(1);
		}
	}
	else
	{
		/* is there a black pawn on column B-H attacking field ? */
		if ((((b->pieces[PAWN] & b->occupied[BLACK] & ~COL_A) << 9) & mask) != 0)
		{
			 return(1);
		}
		/* is there a black pawn on column A-G attacking field ? */
		if ((((b->pieces[PAWN] & b->occupied[BLACK] & ~COL_H) << 7) & mask) != 0)
		{
			 return(1);
		}
	}
	/* imagine a knight would stand on fld: move it and look,
	   if it hits a knight of player */
	if ((board_mask_n(fld) & b->pieces[KNIGHT] & b->occupied[player]) != 0)
	{
		return(1);
	}
	/* move from fld in diagonal directions, if we hit a
	   bishop or queen of player, fld is under attack */
	if ((board_mask_b(b, fld) & (b->pieces[BISHOP] | b->pieces[QUEEN]) & b->occupied[player]) != 0)
	{
		return(1);
	}
	/* move from fld in vert. and horiz. directions, if we hit a
	   rook or queen of player, fld is under attack */
	if ((board_mask_r(b, fld) & (b->pieces[ROOK] | b->pieces[QUEEN]) & b->occupied[player]) != 0)
	{
		return(1);
	}
	/* imagine a king would stand on fld: move it and look,
	   if it hits a king of player */
	if ((board_mask_k(fld) & b->pieces[KING] & b->occupied[player]) != 0)
	{
		return(1);
	}

	/* there are no attackers */
	return(0);
}


