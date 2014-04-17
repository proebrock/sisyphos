#include "chess.h"



const char * const piecenames[NUMPLAYERS] = { "PNBRQK", "pnbrqk"};
const char * const castlenames = "KQkq";



#ifndef NDEBUG

void print_mask_as_board(uint64 mask)
{
	field_t i, j;


	/* print header line */
	fprintf(stderr, "\n  -----------------\n");

	/* print chess field */
	for (i = 7; i < NUMFIELDS; i += 8)
	{
		fprintf(stderr, "%i| ", 8 - (i >> 3));
		for (j = 0; j < 8; j++)
		{
			if (TESTBIT(mask, i - j))
			{
				fprintf(stderr, "# ");
			}
			else
			{
				fprintf(stderr, ". ");
			}
		}
		fprintf(stderr, "|\n");
	}

	/* print footer lines */
	fprintf(stderr, "  -----------------\n");
	fprintf(stderr, "   A B C D E F G H\n\n");
}

#endif	/* NDEBUG */

