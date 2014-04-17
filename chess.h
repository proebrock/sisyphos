#ifndef __CHESS_H__
#define __CHESS_H__



#include "globals.h"



/* players */
typedef uint8			player_t;
#define WHITE			0
#define BLACK			1
#define NUMPLAYERS		2
#define OTHERPL(P)		(player_t)TOGGLED(P)



/* field offsets on bitboard */
typedef uint8		field_t;
#define A1			63
#define B1			62
#define C1			61
#define D1			60
#define E1			59
#define F1			58
#define G1			57
#define H1			56
#define A2			55
#define B2			54
#define C2			53
#define D2			52
#define E2			51
#define F2			50
#define G2			49
#define H2			48
#define A3			47
#define B3			46
#define C3			45
#define D3			44
#define E3			43
#define F3			42
#define G3			41
#define H3			40
#define A4			39
#define B4			38
#define C4			37
#define D4			36
#define E4			35
#define F4			34
#define G4			33
#define H4			32
#define A5			31
#define B5			30
#define C5			29
#define D5			28
#define E5			27
#define F5			26
#define G5			25
#define H5			24
#define A6			23
#define B6			22
#define C6			21
#define D6			20
#define E6			19
#define F6			18
#define G6			17
#define H6			16
#define A7			15
#define B7			14
#define C7			13
#define D7			12
#define E7			11
#define F7			10
#define G7			9
#define H7			8
#define A8			7
#define B8			6
#define C8			5
#define D8			4
#define E8			3
#define F8			2
#define G8			1
#define H8			0
#define NUMFIELDS	64



/* special parts of the board as 64-bit values */
#define ROW_1		UINT64CONST(0xff00000000000000)
#define ROW_2		UINT64CONST(0x00ff000000000000)
#define ROW_3		UINT64CONST(0x0000ff0000000000)
#define ROW_4		UINT64CONST(0x000000ff00000000)
#define ROW_5		UINT64CONST(0x00000000ff000000)
#define ROW_6		UINT64CONST(0x0000000000ff0000)
#define ROW_7		UINT64CONST(0x000000000000ff00)
#define ROW_8		UINT64CONST(0x00000000000000ff)

#define COL_A		UINT64CONST(0x8080808080808080)
#define COL_B		UINT64CONST(0x4040404040404040)
#define COL_C		UINT64CONST(0x2020202020202020)
#define COL_D		UINT64CONST(0x1010101010101010)
#define COL_E		UINT64CONST(0x0808080808080808)
#define COL_F		UINT64CONST(0x0404040404040404)
#define COL_G		UINT64CONST(0x0202020202020202)
#define COL_H		UINT64CONST(0x0101010101010101)



/* piece numbers */
typedef uint8			piece_t;
#define PAWN			0
#define KNIGHT			1
#define BISHOP			2
#define ROOK			3
#define QUEEN			4
#define KING			5
#define NUMPIECES		6



/* invalid value */
#define INVALID			0xff



/* piece names for input/output */
extern const char * const piecenames[NUMPLAYERS];
extern const char * const castlenames;

/* get from field in char notation field in integer notation */
/* e.g. b4 (C1 = 'b', C2 = '4') --> 6 + 4 * 8 = 38                 */
#define FLD_CHR2INT(C1, C2)		(('h' - (C1)) + (('8' - (C2)) << 3))

/* print field to specified output stream */
#define FLD_PRINT(S, P)	\
	fprintf((S), "%c%c", 'h' - ((P) & 7), '8' - ((P) >> 3))

/* print one character piece name to specified output stream */
#define PIECE_PRINT(S, PL, PC)	\
	fprintf((S), "%c", piecenames[(PL)][(PC)]);



#ifndef NDEBUG
extern void print_mask_as_board(uint64 mask);
#endif	/* NDEBUG */



#endif	/* __CHESS_H__ */
