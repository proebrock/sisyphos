#ifndef __LUT_H__
#define __LUT_H__


#include "rotate.h"
#include "globals.h"
#include "wrapper.h"
#include "chess.h"



#define ROOKLUT_ENTRY(PATTERN, POSITION)	\
	lut_rook[((PATTERN) << 3) | (POSITION)]



/*
** lookup tables for knight, rook and king moves
*/
extern uint64 *lut_knight;
extern uint8  *lut_rook;
extern uint64 *lut_king;



/*
** if using diagonal bitboards (NW, NE), we have to isolate a
** diagonal line: if x is the field number of a bishop on the
** NW bitboard, we get all pieces on this line by:
**
** (allpieces[NW] >> diag_shift[x]) & diag_valid_bits[x]
*/
extern const uint8 diag_shift[NUMFIELDS];
extern const uint8 diag_valid_bits[NUMFIELDS];



/* initialize lookup tables */
extern void lut_init();
/* release lookup table memory */
extern void lut_exit();



#endif /* __LUT_H__ */
