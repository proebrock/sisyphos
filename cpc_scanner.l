%{
	#include <string.h>

	#include "cpc_parser.h"

	/* interface to parser */
	extern unsigned int cpc_column;
	extern unsigned int cpc_row;
	

	/* we don't need yyunput and want to get rid of stupid error messages */
	#define YY_NO_UNPUT
	/* before processing of every rule: add size of current token to variable */
	#define YY_USER_ACTION cpc_column+=yyleng;
%}



/* we want a not interactive scanner */
%option never-interactive
/* prefix for all functions and variables in this scanner */
%option prefix="cpc_yy"



/* numbers */
DGT			[0-9]
NUM			0|("-"?[1-9][0-9]*)

/* spaces */
NEWLINE		[\n]
SPACE		[ \t]

/* strings */
COMMENT		"//"
STRING		\"[^"]*\"

/* fen notation */
WFIGURE		[PNBRQK]
BFIGURE		[pnbrqk]
FIGURE		{WFIGURE}|{BFIGURE}
COORD		[1-8]
FENFLD		{FIGURE}|{COORD}
FENPOS		{FENFLD}+("/"{FENFLD}+){7}
FENPLAYER	[w]|[b]
FENCASTLE	"-"|(K?Q?k?q?)
FENEP		"-"|([a-h][36])
FEN			"["{FENPOS}{SPACE}+{FENPLAYER}{SPACE}+{FENCASTLE}{SPACE}+{FENEP}{SPACE}+{NUM}{SPACE}+{NUM}"]"



%%



{SPACE}						{
								/* ignore spaces */
							}
{COMMENT}.*{NEWLINE}		{
								/* ignore comment lines */
								cpc_column = 1;
								cpc_row++;
							}
{NEWLINE}					{
								/* ignore newlines */
								cpc_column = 1;
								cpc_row++;
							}
"depth"						{
								return(DEPTH);
							}
"description"				{
								return(DESCRIPTION);
							}
"nodes"						{
								return(NODES);
							}
"position"					{
								return(POSITION);
							}
"problem"					{
								return(PROBLEM);
							}
"solution"					{
								return(SOLUTION);
							}
"="							{
								return(ASSIGN);
							}
";"							{
								return(SEMICOLON);
							}
"{"							{
								return(START_BLOCK);
							}
"}"							{
								return(END_BLOCK);
							}
{NUM}						{
								cpc_yylval.n = strtol(yytext, NULL, 10);
								return(NUM);
							}
{FEN}						{
								/* using a static data structure for data transfer between scanner and parser;
								   this is faster than dynamically allocated memory, but token longer than
								   4095 will be chopped off */
								strncpy(cpc_yylval.s, yytext, 4095);
								cpc_yylval.s[4095] = 0;
								return(FEN);
							}
{STRING}					{
								/* using a static data structure for data transfer between scanner and parser;
								   this is faster than dynamically allocated memory, but token longer than
								   4095 will be chopped off */
								strncpy(cpc_yylval.s, &yytext[1], 4095);
								cpc_yylval.s[strlen(cpc_yylval.s) - 1] = 0;
								return(STRING);
							}
.							{
								return(NONSENS);
							}



%%



int yywrap()
{
	/* end of file: exit scanner */
	return(1);
}

