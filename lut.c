#include "lut.h"

#include <stdlib.h>



const uint8 diag_shift[NUMFIELDS] =
{
	 0,  1,  1,  3,  3,  3,  6,  6,
	 6,  6, 10, 10, 10, 10, 10, 15,
	15, 15, 15, 15, 15, 21, 21, 21,
	21, 21, 21, 21, 28, 28, 28, 28,
	28, 28, 28, 28, 36, 36, 36, 36,
	36, 36, 36, 43, 43, 43, 43, 43,
	43, 49, 49, 49, 49, 49, 54, 54,
	54, 54, 58, 58, 58, 61, 61, 63
};
const uint8 diag_valid_bits[NUMFIELDS] =
{
	0x01, 0x03, 0x03, 0x07, 0x07, 0x07, 0x0f, 0x0f,
	0x0f, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f, 0x7f, 0x7f,
	0x7f, 0x7f, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x7f, 0x7f,
	0x7f, 0x7f, 0x7f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x0f, 0x0f,
	0x0f, 0x0f, 0x07, 0x07, 0x07, 0x03, 0x03, 0x01
};



uint64 *lut_knight;
uint8  *lut_rook;
uint64 *lut_king;



/* initialize lookup tables */
void lut_init()
{
	int i, j, k;
	uint64 x;
	uint8 y;

	/*
	** allocate and generate king knight lookup table:
	** knight moves two steps forward one sideways (8 moves in total),
	** make sure that he does not cross the border of the board
	*/
	lut_knight = (uint64 *)w_malloc(NUMFIELDS * sizeof(uint64));
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			x = 0;
			i-=2; j--;	/* (-2/-1) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			j+=2;		/* (-2/+1) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			i+=4;		/* (+2/+1) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			j-=2;		/* (+2/-1) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			i-=3; j--;	/* (-1/-2) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			j+=4;		/* (-1/+2) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			i+=2;		/* (+1/+2) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			j-=4;		/* (+1/-2) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			i--; j+=2;	/* (0/0) */
			lut_knight[(i << 3) | j] = x;
		}
	}

	/*
	** allocate and generate rook lookup table
	** this lookup table contains two indices: the first index (0..255)
	** determines the fields of other pieces in the line, the second
	** index the the field of the rook on the line
	*/
	lut_rook = (uint8 *)w_malloc(256 * 8 * sizeof(uint8));
	for (i = 0; i < 0xff; i++)
	{
		for (j = 0; j < 8; j++)
		{
			y = 0;
			/* move rook to the left till it reaches first other piece
			   or the left border of the line */
			for (k = 1; ; k++)
			{
				if ((j + k) > 7)
					break;
				SETBIT(y, j + k);
				if (TESTBIT(i, j + k))
					break;
			}
			/* move rook to the right till it reaches first other piece
			   or the right border of the line */
			for (k = -1; ; k--)
			{
				if ((j + k) < 0)
					break;
				SETBIT(y, j + k);
				if (TESTBIT(i, j + k))
					break;
			}
			/* store result into lookup table */
			ROOKLUT_ENTRY(i, j) = y;
		}
	}

	/*
	** allocate and generate king lookup table:
	** king moves one step in the eight different directions,
	** make sure that he does not cross the border of the board
	*/
	lut_king = (uint64 *)w_malloc(NUMFIELDS * sizeof(uint64));
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			x = 0;
			i--; j--;	/* (-1/-1) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			j++;		/* (-1/0) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			j++;		/* (-1/+1) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			i++;		/* (0/+1) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			j -= 2;		/* (0/-1) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			i++;		/* (+1/-1) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			j++;		/* (+1/0) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			j++;		/* (+1/+1) */
			if ( ((i & ~0x7) == 0) && ((j & ~0x7) == 0) )
				SETBIT(x, (i << 3) | j);
			i--; j--;	/* (0/0) */
			lut_king[(i << 3) | j] = x;
		}
	}
}



/* release lookup table memory */
void lut_exit()
{
	w_free((void *)lut_knight);
	w_free((void *)lut_rook);
	w_free((void *)lut_king);
}

