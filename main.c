#include <stdio.h>
#include <stdlib.h>



#include "rotate.h"
#include "lut.h"
#include "search.h"



int main(int argc, char *argv[])
{
#if 0
	position pos;
	movelist ml;

	/* make compiler happy */
	argc = argc;
	argv = argv;

	/* print intro */
	fprintf(stdout, "-------------------------------------------------------------------\n");
	fprintf(stdout, "sisyphos - chess problem solver\n");
	fprintf(stdout, "Copyright (C) 2004-2014  Philipp Roebrock (philipp.roebrock@gmx.de)\n");
	fprintf(stdout, "-------------------------------------------------------------------\n");

	/* initializisation */
#ifndef NDEBUG
	check_bitoperations();
	rotate_checktables();
#endif
	lut_init();

	position_readfen(&pos, "8/5N2/pkBK4/p7/p6p/P6p/8/8 w - - 0 1");
	position_print(&pos, stdout);
	ML_CLEAR(ml);
	printf("-%i-:\n", move_generate(&pos, &ml));
	ML_PRINT(stdout, ml);
	printf("\n\n");

	move_apply(&pos, &ML_ENTRY(ml, 6));
	position_print(&pos, stdout);
	ML_CLEAR(ml);
	printf("-%i-:\n", move_generate(&pos, &ml));
	ML_PRINT(stdout, ml);
	printf("\n\n");

	move_apply(&pos, &ML_ENTRY(ml, 16));
	position_print(&pos, stdout);
	ML_CLEAR(ml);
	printf("-%i-:\n", move_generate(&pos, &ml));
	ML_PRINT(stdout, ml);
	printf("\n\n");
#endif

	search_problem problem;
	search_result result;


	/* make compiler happy */
	argc = argc;
	argv = argv;

	/* print intro */
	fprintf(stdout, "--------------------------------------------------------------------\n");
	fprintf(stdout, "sisyphos - chess problem solver\n");
	fprintf(stdout, "Copyright (C) 2004-2014  Philipp Roebrock (philipp.roebrock@gmx.net)\n");
	fprintf(stdout, "--------------------------------------------------------------------\n");

	/* initializisation */
#ifndef NDEBUG
	check_bitoperations();
	rotate_checktables();
#endif
	lut_init();

	/* init problem */
	/*position_readfen(&(problem.pos), "2k4r/ppp3p1/1n1q4/2p1p1Pp/4P2P/Q1PPBr2/PP3P2/3R1RK1 b - - 0 1");*/							/*  61 9 */
	position_readfen(&(problem.pos), "8/8/5pp1/5k2/8/4QK2/8/8 w - - 0 1");							/*  61 9 */
	position_print(&(problem.pos), stdout);
	problem.depth = 9;
	problem.mode  = SEARCH_MODE_DEPTH_FULL;

	/* search */
	search_main(&result, &problem);

	/* print results */
	printf("\nresult          ");
	RATING_PRINT_MOVES_TILL_MATE(stdout, result.rating);
	if (!ML_EMPTY(result.solution))
	{
		printf("solution        ");
		ML_PRINT(stdout, result.solution);
		printf("\n");
		printf("bestmoves       ");
		ML_PRINT(stdout, result.bestmoves);
		printf("\n");
	}
	printf("nodes           %lli\n", result.numnodes);
	if (result.time> 0)
	{
		printf("time consumed   %llis\n", result.time);
		printf("speed           ");
		si_print(stdout, result.numnodes / result.time);
		printf("Ns^(-1)\n");
	}

#if 0
	700 Chess Problems  Selected from the Compositions of Mrs. W. J. Baird
	position_readfen(&(problem.pos), "8/5N2/pkBK4/p7/p6p/P6p/8/8 w - - 0 1");						/* 630 9 3.5GN */

	"Canadian Chess Problems"
	position_readfen(&(problem.pos), "8/8/8/1p4p1/pP2N3/P4P1p/5K2/7k w - - 0 1"); 						/* title 13  much! */
	position_readfen(&(problem.pos), "2k4r/ppp3p1/1n1q4/2p1p1Pp/4P2P/Q1PPBr2/PP3P2/3R1RK1 b - - 0 1");	/* 202   11  much! */
	position_readfen(&(problem.pos), "1rn3k1/ppqNr1p1/2p1bp1B/3pN3/8/5RQ1/P4PPP/6K1 w - - 0 1"); 		/* 203    7 61.0GN */

	"Schachminiaturen - Eine Problemsammlung" by Oscar Blumenthal
	position_readfen(&(problem.pos), "8/8/8/8/8/Q7/4p3/5k1K w - - 0 1");							/*  15 9  20.3GN */
	position_readfen(&(problem.pos), "8/8/8/8/8/K7/N6B/k7 w - - 0 1");								/*  16 9  39.3MN */
	position_readfen(&(problem.pos), "8/8/5pp1/5k2/8/4QK2/8/8 w - - 0 1");							/*  61 9   3.7GN */
	position_readfen(&(problem.pos), "8/8/7R/3k2B1/8/3K4/4P3/8 w - - 0 1");							/*  62 9   6.3GN */
	position_readfen(&(problem.pos), "8/8/4Q3/8/8/8/4p1r1/4K1kb w - - 0 1");						/* 174 9 317.7GN */
	position_readfen(&(problem.pos), "8/8/8/8/8/1p5p/b2Q3K/1k1B4 w - - 0 1");						/* 175 9  33.2GN */
	position_readfen(&(problem.pos), "8/1P6/2k4N/2N5/P1K5/8/8/8 w - - 0 1");						/* 176 9   1.5GN */
	position_readfen(&(problem.pos), "8/8/8/7Q/2k4p/4P3/P6K/2N5 w - - 0 1");						/* 339 9   7.7GN */
	position_readfen(&(problem.pos), "8/8/8/p7/8/8/R6p/2K2Rbk w - - 0 1");							/* 340 9  30.2GN */
	position_readfen(&(problem.pos), "8/8/8/8/4p3/5pNN/4p3/2K1k3 w - - 0 1");						/* 341 9 628.2MN */
	position_readfen(&(problem.pos), "8/8/8/8/6p1/3KNkB1/4N3/7b w - - 0 1");						/* 342 9   9.4GN */
	position_readfen(&(problem.pos), "8/8/8/4p3/3kB1K1/8/5PN1/2R5 w - - 0 1");						/* 359 9   9.3GN */
	position_readfen(&(problem.pos), "2N5/8/8/8/8/R1p5/b7/k1K5 w - - 0 1");							/* 360 9   1.5GN */

	"Schachminiaturen - Neue Folge" by Oscar Blumenthal
	position_readfen(&(problem.pos), "7k/7P/5P2/5K2/8/8/8/8 w - - 0 1");							/*  14 9   7.0MN */
	position_readfen(&(problem.pos), "8/7N/7N/8/7k/5K2/6P1/8 w - - 0 1");							/*  68 9  54.1MN */
	position_readfen(&(problem.pos), "8/8/3K4/3N4/3k4/1R1P4/8/8 w - - 0 1");						/*  69 9   1.1GN */
	position_readfen(&(problem.pos), "k1K5/8/8/B1P1N3/8/8/8/8 w - - 0 1");							/*  70 9  72.1MN */
	position_readfen(&(problem.pos), "8/3Np3/6R1/3k4/8/3PK3/8/8 w - - 0 1");						/* 194 9   4.5GN */
	position_readfen(&(problem.pos), "6R1/8/2B2p2/4p3/5k2/3P4/4K3/8 w - - 0 1");					/* 352 9  11.2GN */
	position_readfen(&(problem.pos), "3KQ3/8/7p/5pkP/8/5P2/8/8 w - - 0 1");							/* 353 9   2.2GN */
	position_readfen(&(problem.pos), "8/8/8/1R3K2/2pk4/2N5/1PP5/8 w - - 0 1");						/* 354 9   1.7GN */
	position_readfen(&(problem.pos), "8/K1N1p3/2k1N3/4P3/8/8/1P6/8 w - - 0 1");						/* 355 9 278.5MN */
	position_readfen(&(problem.pos), "2n5/kpK4R/8/1P2n3/8/8/8/8 w - - 0 1");						/* 356 9  14.4GN */
	position_readfen(&(problem.pos), "7k/8/5N1P/8/8/5bp1/8/B5K1 w - - 0 1");						/* 357 9   5.2GN */
	position_readfen(&(problem.pos), "2R5/5K2/8/3kB3/6P1/5P2/5P2/8 w - - 0 1");						/* 358 9   4.3GN */
	position_readfen(&(problem.pos), "8/kPpp4/P7/3K4/8/1N6/8/8 w - - 0 1");							/* 359 9 729.8MN */
	position_readfen(&(problem.pos), "8/4R3/8/1B1k4/4p1N1/8/8/6BK w - - 0 1");						/* 360 9   9.5GN */
	position_readfen(&(problem.pos), "8/3B4/8/8/3k4/2NN3P/3K4/8 w - - 0 1");						/* 374 9   2.1GN */
	position_readfen(&(problem.pos), "8/1p6/1B4R1/1k2P3/8/1K6/8/8 w - - 0 1");						/* 392 9   1.5GN */

	"seventy-five chess problems" by John Thursby, Trinitiy College Cambridge 1883
	position_readfen(&(problem.pos), "8/N2b2B1/1P5Q/2pk2K1/N7/3p4/n2P4/4R3 w - - 0 1");				/* #01 3 */
	position_readfen(&(problem.pos), "1B5K/2np4/Rn1Np3/2p1k3/1P6/2P5/7P/7Q w - - 0 1");				/* #02 3 */
	position_readfen(&(problem.pos), "1Nb3qB/3p1pP1/1p1K1k1P/1Pp3pP/1p2Pp2/8/1P6/1RQ5 w - - 0 1");	/* #03 3 */
	position_readfen(&(problem.pos), "8/2p4N/2B1p3/2pPkP2/2Q2p2/4b2K/7B/8 w - - 0 1");				/* #04 3 */
	position_readfen(&(problem.pos), "8/4Np2/1p1P1rp1/1Pp1k1P1/2BR1p1n/B4R2/3P2pN/6K1 w - - 0 1");	/* #05 3 */
	position_readfen(&(problem.pos), "K7/8/p1p5/r1pR2nR/b1kPp3/1N3P2/P2B4/1Q6 w - - 0 1");			/* #06 3 */
	position_readfen(&(problem.pos), "B7/4pR2/2p1P3/R1bN1b2/1p2k3/1P3N2/1rP5/B1nQ3K w - - 0 1");	/* #07 3 */
	position_readfen(&(problem.pos), "2b2Q2/1pR1Np2/5bR1/4kp2/3p1N2/3p2n1/3P2BB/K7 w - - 0 1");		/* #08 3 */
	position_readfen(&(problem.pos), "3B4/6K1/3p4/P2k4/3N4/4P3/NP3P2/3B4 w - - 0 1");				/* #26 5 */
	position_readfen(&(problem.pos), "1K1B1Qn1/p2p2pr/k2P1n1p/8/2P5/8/8/2N1b3 w - - 0 1");			/* #27 5 */
	position_readfen(&(problem.pos), "7K/6B1/2NNkp2/6P1/2P5/8/6P1/3Q4 w - - 0 1");					/* #28 5 */
	position_readfen(&(problem.pos), "1r2n3/1n1pp1p1/4k3/3NP1pR/ppKP4/5QP1/B7/8 w - - 0 1");		/* #67 7   4.1GN */
	position_readfen(&(problem.pos), "4bN1N/3p3R/3P2p1/2ppk1p1/3r1RP1/6PB/K7/Q7 w - - 0 1");		/* #69 7 913.2MN */
	position_readfen(&(problem.pos), "6K1/2p1k3/N1P2N2/1P5P/6p1/2P3P1/2p1P3/2B1R3 w - - 0 1");		/* #70 7 171.7MN */
#endif

#if 0
	very basic checks

	/* blacks move: black is checkmate in 0 halfmoves */
	search_maxdepthsearch(&result, "4R1k1/8/6K1/8/8/8/8/8 b - - 0 1", 4);
	/* whites move: black is checkmate in 1 halfmoves */
	search_maxdepthsearch(&result, "6k1/8/4R1K1/8/8/8/8/8 w - - 0 1", 4);
	/* blacks move: black is checkmate in 2 halfmoves */
	search_maxdepthsearch(&result, "5k2/8/4R1K1/8/8/8/8/8 b - - 0 1", 4);
	/* whites move: black is checkmate in 3 halfmoves */
	search_maxdepthsearch(&result, "5k2/8/4R3/6K1/8/8/8/8 w - - 0 1", 4);

	/* whites move: white is checkmate in 0 halfmoves */
	search_maxdepthsearch(&result, "4r1K1/8/6k1/8/8/8/8/8 w - - 0 1", 4);
	/* blacks move: white is checkmate in 1 halfmoves */
	search_maxdepthsearch(&result, "6K1/8/4r1k1/8/8/8/8/8 b - - 0 1", 4);
	/* whites move: white is checkmate in 2 halfmoves */
	search_maxdepthsearch(&result, "5K2/8/4r1k1/8/8/8/8/8 w - - 0 1", 4);
	/* blacks move: white is checkmate in 3 halfmoves */
	search_maxdepthsearch(&result, "5K2/8/4r3/6k1/8/8/8/8 b - - 0 1", 4);
#endif

#if 0
	/* checkmate in 7: 1. Kf5 e5 2. Kg6 e4 3. Bxe4 Kg4 4. Bf5# */
	position_readfen(&(problem.pos), "8/4p3/8/8/4K3/5BBk/4P2P/8 w - - 0 1");
	/* checkmate in 7: 1. Ra3 b4 2. Ra4 b3 3. Rh4 Kxh4 4. Nf3# */
	position_readfen(&(problem.pos), "2K4R/5p2/4pNp1/1p1pP1k1/3Np3/2R1P1p1/1P4P1/8 w - - 0 1");
	/* checkmate in 7: 1. Qg1 f5 2. Qf2+ Kg4 3. Qg3+ Kh5 4. Qg5# */
	position_readfen(&(problem.pos), "8/8/5p2/4p3/2K1Nk2/8/8/4Q3 w - - 0 1");
	/* checkmate in 7: 1. Ka2 e3 2. Ra1 e2 3. Nb1 Bd2 4. Nxd2# */
	position_readfen(&(problem.pos), "8/8/8/8/4p3/2N3R1/5p2/K2Rbk2 w - - 0 1");
	/* checkmate in 7: 1. Kc3 e5 2. Rd4+ exd4+ 3. Kc4 d3 4. cxd3# */
	position_readfen(&(problem.pos), "8/2BR4/4p3/8/4k1P1/5p2/2PK1P2/8 w - - 0 1");
#endif

	/* cleanup */
	lut_exit();

	/* graceful exit */
	exit(EXIT_SUCCESS);
}
