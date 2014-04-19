%{
	#include <stdio.h>

	#include "globals.h"
	#include "cpc.h"

	/* we want verbose error messages from parser */
	#define YYERROR_VERBOSE

	/* this is the interface to the scanner */
	extern int yylex();

	/* local variables for better error tracking */
	unsigned int cpc_column;
	unsigned int cpc_row;
	const char *cpc_filename;
	/* pointer to list to add problems to */
	cpc *cpc_mainlist;
	/* last entry in cpc main list */
	#define CENTRY	cpc_mainlist->p[cpc_mainlist->numentries - 1]

	/* local function prototypes */
	int yyerror(const char *s);
%}



/* prefix for all functions and variables in this parser */
%name-prefix="cpc_yy"



%union
{
	/* using a static data structure for data transfer between scanner and parser;
	   this is faster than dynamically allocated memory, but token longer than
	   4095 will be chopped off */
	char s[4096];
	long int  n;
}

/* data types of tokens */
%type <s>	FEN
%type <s>	STRING
%type <n>	NUM

/* define tokens */
%token ASSIGN
%token DEPTH
%token DESCRIPTION
%token END_BLOCK
%token FEN
%token NODES
%token NUM
%token POSITION
%token PROBLEM
%token SEMICOLON
%token SOLUTION
%token START_BLOCK
%token STRING
%token NONSENS

/* define start symbol */
%start file



%%



file			: problem
				| file problem
				;

problem			: PROBLEM START_BLOCK block END_BLOCK
				{
					/* add new entry in pointer list */
					cpc_mainlist->numentries++;
					cpc_mainlist->p = (problem **)realloc(cpc_mainlist->p, cpc_mainlist->numentries * sizeof(problem *));
					/* create new entry and initalize */
					CENTRY = (problem *)malloc(sizeof(problem));
					CENTRY->valid = 0;
				}
				;

block			: line SEMICOLON
				| block line SEMICOLON
				;

line			: POSITION ASSIGN FEN
				{
					if (strlen($3) != 0)
					{
						CENTRY->pstr =(char *)malloc((strlen($3) + 1) * sizeof(char));
						strcpy(CENTRY->pstr, $3);
						CENTRY->valid |= CPC_ENTRY_VALID_FEN;
					}
				}
				| DESCRIPTION ASSIGN STRING
				{
					if (strlen($3) != 0)
					{
						CENTRY->description = (char *)malloc((strlen($3) + 1) * sizeof(char));
						strcpy(CENTRY->description, $3);
						CENTRY->valid |= CPC_ENTRY_VALID_DESCRIPTION;
					}
				}
				| SOLUTION ASSIGN STRING
				{
					if (strlen($3) != 0)
					{
						CENTRY->solution = (char *)malloc((strlen($3) + 1) * sizeof(char));
						strcpy(CENTRY->solution, $3);
						CENTRY->valid |= CPC_ENTRY_VALID_SOLUTION;
					}
				}
				| DEPTH ASSIGN NUM
				{
					CENTRY->depth = $3;
					CENTRY->valid |= CPC_ENTRY_VALID_DEPTH;
				}
				| NODES ASSIGN NUM
				{
					CENTRY->nodes = $3;
					CENTRY->valid |= CPC_ENTRY_VALID_NODES;
				}
				;



%%



int yyerror(const char *s)
{
	/* check parameters */
	ASSERT(s != (const char *)0);

	/* output error message */
	fprintf(stderr, "file \"%s\", row %i, around column %i:\n%s\n\n", cpc_filename, cpc_row, cpc_column, s);

	/* return */
	return(1);
}

