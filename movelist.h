#ifndef __MOVELIST_H__
#define	__MOVELIST_H__



/*
** maximum number of entries in movelist:
** movelist is static for performance reasons!
** a move list must be able to keep all legal
** moves of a position!
*/
#define MAX_MOVELIST_ENTRIES	128


struct movelist_t
{
	/* static array of moves in movelist */
	move m[MAX_MOVELIST_ENTRIES];
	/* number of entries in movelist */
	unsigned int len;
};
#define movelist struct movelist_t



/* clear movelist ML */
#define ML_CLEAR(ML)		(ML).len = 0;
/* movelist entry from ML with index I (random access) */
#define ML_ENTRY(ML, I)		((ML).m[I])
/* number of entries in movelist ML */
#define ML_SIZE(ML)			((ML).len)
/* check if movelist ML is empty */
#define ML_EMPTY(ML)		((ML).len == 0)
/* add move M to movelist ML */
#define ML_ADD_MOVE(ML, M)	memcpy(&((ML).m[(ML).len++]), &(M), sizeof(move));
/* undo last move in movelist ML  */
#define ML_UNDO(ML)			(ML).len--;
/* apply movelist ML to position POS */
#define ML_APPLY(POS, ML)														\
{																				\
	unsigned int ml_apply_index;												\
	for (ml_apply_index = 0; ml_apply_index < (ML).len; ml_apply_index++)		\
	{																			\
		move_apply(&(POS), &((ML).m[ml_apply_index]));							\
	}																			\
}
/* copy moves from movelist MLSRC to movelist MLDEST */
#define ML_COPY(MLDEST, MLSRC)													\
{																				\
	unsigned int ml_copy_index;													\
	for (ml_copy_index = 0; ml_copy_index < (MLSRC).len; ml_copy_index++)		\
	{																			\
		ML_ADD_MOVE(MLDEST, (MLSRC).m[ml_copy_index]);							\
	}																			\
}
/* print movelist ML to stream S */
#define ML_PRINT(S, ML)															\
{																				\
	unsigned int ml_print_index;												\
	for (ml_print_index = 0; ml_print_index < ((ML).len - 1); ml_print_index++)	\
	{																			\
		move_print((S), &((ML).m[ml_print_index]));								\
		fprintf((S), ", ");														\
	}																			\
	move_print((S), &((ML).m[ml_print_index]));									\
}
/* get pointer to next free move entry in movelist ML  */
#define ML_NXT_FREE(ML)		(&((ML).m[(ML).len++]))



#endif	/* __MOVELIST_H__ */
