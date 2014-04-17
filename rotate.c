#include "rotate.h"



const field_t lut_rotate[NUMROTMODES][NUMFIELDS] =
{
	/* NORTH -> WEST */
	{
		 7, 15, 23, 31, 39, 47, 55, 63,
		 6, 14, 22, 30, 38, 46, 54, 62,
		 5, 13, 21, 29, 37, 45, 53, 61,
		 4, 12, 20, 28, 36, 44, 52, 60,
		 3, 11, 19, 27, 35, 43, 51, 59,
		 2, 10, 18, 26, 34, 42, 50, 58,
		 1,  9, 17, 25, 33, 41, 49, 57,
		 0,  8, 16, 24, 32, 40, 48, 56
	},
	/* WEST -> NORTH */
	{
		56, 48, 40, 32, 24, 16,  8,  0,
		57, 49, 41, 33, 25, 17,  9,  1,
		58, 50, 42, 34, 26, 18, 10,  2,
		59, 51, 43, 35, 27, 19, 11,  3,
		60, 52, 44, 36, 28, 20, 12,  4,
		61, 53, 45, 37, 29, 21, 13,  5,
		62, 54, 46, 38, 30, 22, 14,  6,
		63, 55, 47, 39, 31, 23, 15,  7
	},
	/* NORTH -> NORTHWEST */
	{
		 0,  2,  5,  9, 14, 20, 27, 35,
		 1,  4,  8, 13, 19, 26, 34, 42,
		 3,  7, 12, 18, 25, 33, 41, 48,
		 6, 11, 17, 24, 32, 40, 47, 53,
		10, 16, 23, 31, 39, 46, 52, 57,
		15, 22, 30, 38, 45, 51, 56, 60,
		21, 29, 37, 44, 50, 55, 59, 62,
		28, 36, 43, 49, 54, 58, 61, 63
	},
	/* NORTHWEST -> NORTH */
	{
		 0,  8,  1, 16,  9,  2, 24, 17,
		10,  3, 32, 25, 18, 11,  4, 40,
		33, 26, 19, 12,  5, 48, 41, 34,
		27, 20, 13,  6, 56, 49, 42, 35,
		28, 21, 14,  7, 57, 50, 43, 36,
		29, 22, 15, 58, 51, 44, 37, 30,
		23, 59, 52, 45, 38, 31, 60, 53,
		46, 39, 61, 54, 47, 62, 55, 63
	},
	/* NORTH -> NORTHEAST */
	{
		35, 27, 20, 14,  9,  5,  2,  0,
		42, 34, 26, 19, 13,  8,  4,  1,
		48, 41, 33, 25, 18, 12,  7,  3,
		53, 47, 40, 32, 24, 17, 11,  6,
		57, 52, 46, 39, 31, 23, 16, 10,
		60, 56, 51, 45, 38, 30, 22, 15,
		62, 59, 55, 50, 44, 37, 29, 21,
		63, 61, 58, 54, 49, 43, 36, 28
	},
	/* NORTHEAST -> NORTH */
	{
		  7, 15,  6, 23, 14,  5, 31, 22,
		 13,  4, 39, 30, 21, 12,  3, 47,
		 38, 29, 20, 11,  2, 55, 46, 37,
		 28, 19, 10,  1, 63, 54, 45, 36,
		 27, 18,  9,  0, 62, 53, 44, 35,
		 26, 17,  8, 61, 52, 43, 34, 25,
		 16, 60, 51, 42, 33, 24, 59, 50,
		 41, 32, 58, 49, 40, 57, 48, 56
	}
};



#ifndef NDEBUG
/*
** check rotation functions (and therefore the
** consistency of hardcoded lookup tables!)
*/
void rotate_checktables()
{
	int i, j, k;


	for (i = 0; i < NUMROTMODES; i++)
	{
		k = 0;
		for (j = 0; j < NUMFIELDS; j++)
		{
			/* result of index on rotated board must be between 0..63 */
			ASSERTMSG(ROTATE_POS(i, j) <= NUMFIELDS, "error in hardcoded luts");
			k += ROTATE_POS(i, j);
		}
		/* sum of all indices must be sum_{i=0}^{63}i=2016 */
		ASSERTMSG(k == 2016, "error in hardcoded luts");
	}
	for (i = 0; i < NUMROTMODES; i += 2)
	{
		for (j = 0; j < NUMFIELDS; j++)
		{
			/* rotation of an index forward and backwards again
			   must give the initial index! */
			ASSERTMSG(j == ROTATE_POS(i, ROTATE_POS(i + 1, j)), "error in hardcoded luts");
		}
	}
}
#endif
