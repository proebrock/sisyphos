#include "position.h"

#include <ctype.h>
#include <string.h>

#include "globals.h"



/* empty board (no pieces) */
void position_clear(position *p)
{
	/* check parameters */
	ASSERT(p != (position *)0);

	/* clear board */
	board_clear(&p->b);

	/* set other game parameters */
	ME                = WHITE;
	ENEMY             = BLACK;
	p->castlestate[0] = 0;
	p->castlestate[1] = 0;
	p->castlestate[2] = 0;
	p->castlestate[3] = 0;
	p->epfield        = INVALID;
	p->halfmoves      = 0;
	p->lastcap        = 0;
}



/* setup start position on board */
void position_reset(position *p)
{
	/* check parameters */
	ASSERT(p != (position *)0);

	/* set up board */
	board_reset(&p->b);

	/* set other game parameters */
	ME                = WHITE;
	ENEMY             = BLACK;
	p->castlestate[0] = 1;
	p->castlestate[1] = 1;
	p->castlestate[2] = 1;
	p->castlestate[3] = 1;
	p->epfield        = INVALID;
	p->halfmoves      = 0;
	p->lastcap        = 0;
}



/* read position info from string in Forsyth-Edwards Notation (FEN)
   return 1 on success, 0 in error case (the state of the board
   is undefined in case of error!) */
int position_readfen(position *p, const char *fen)
{
	unsigned int n, i;


	/* check parameters */
	ASSERT(p   != (position *)0);
	ASSERT(fen != (const char *)0);

	/* clear position */
	position_clear(p);

	/* read board info */
	if (board_readfen(&p->b, fen, &fen) != 1)
		return(0);

	/* determine who has to make the next move */
	for (n = 0; fen[n] == ' '; n++);
	if (fen[n] == 'w')
	{
		ME    = WHITE;
		ENEMY = BLACK;
	}
	else if (fen[n] == 'b')
	{
		ME    = BLACK;
		ENEMY = WHITE;
	}
	else
	{
		return(0);
	}
	n++;

	/* determine castling status */
	for (; fen[n] == ' '; n++);
	if (fen[n] != '-')
	{
		/* check every letter in description */
		for (; fen[n] != ' '; n++)
		{
			/* search letter of fen castling status in castling names */
			i = 0;
			while (1)
			{
				if (i == strlen(castlenames))
				{
					/* found: unknown character in fen castling status */
					return(0);
				}
				if (castlenames[i] == fen[n])
				{
					/* found: set castling status and stop further searching */
					p->castlestate[i] = 1;
					break;
				}
				i++;
			}
		}
	}
	else
	{
		n++;
	}

	/* determine en passant field, if available */
	for (; fen[n] == ' '; n++);
	if (fen[n] != '-')
	{
		if (fen[n]     >= 'a' && fen[n]     <= 'h' &&
			fen[n + 1] >= '1' && fen[n + 1] <= '8')
		{
			p->epfield = FLD_CHR2INT(fen[n], fen[n + 1]);
			n += 2;
		}
		else
		{
			return(0);
		}
	}
	else
	{
		p->epfield = INVALID;
		n++;
	}

	/* determine number of moves since last capture */
	for (; fen[n] == ' '; n++);
	for (i = 0; fen[n] != ' '; n++)
	{
		if (isdigit(fen[n]))
		{
			i = (10 * i) + (fen[n] - '0');
		}
		else
		{
			return(0);
		}
	}
	p->lastcap = i;

	/* determine halfmove number */
	for (; fen[n] == ' '; n++);
	for (i = 0; ((n < strlen(fen)) && (fen[n] != ' ')); n++)
	{
		if (isdigit(fen[n]))
		{
			i = (10 * i) + (fen[n] - '0');
		}
		else
		{
			return(0);
		}
	}
	/* p->halfmoves = 2 * (i - 1) + ME; */
	p->halfmoves = (uint16)((i - 1) << 2) | (uint16)ME;

	/* return with success */
	return(1);
}



/* print current position to output stream */
void position_print(const position *p, FILE *stream)
{
	int i, j;


	/* check parameters */
	ASSERT(p != (position *)0);
	ASSERT(stream != (FILE *)0);

	/* print header lines with status information */
	fprintf(stream, "\n%i. (%i.) ", p->halfmoves, p->lastcap);
	fprintf(stream, "%s  ", (ME == WHITE) ? "white" : "black");
	for (i = 0, j = 0; i <= 3; i++)
	{
		if (p->castlestate[i] == 1)
		{
			fprintf(stream, "%c", castlenames[i]);
			j = 1;
		}
	}
	if (j == 0)
	{
		fprintf(stream, "-");
	}
	fprintf(stream, "  ");
	if (p->epfield == INVALID)
	{
		fprintf(stream, "-");
	}
	else
	{
		FLD_PRINT(stream, p->epfield);
	}

	/* print board */
	board_print(&p->b, stream);
}




