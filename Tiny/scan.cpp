/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
//#include "parameter.h"

/* states in scanner DFA */
typedef enum
{
	START, INASSIGN, INCOMMENT, INNUM, INID, DONE
}
StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];


/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{
	if (!(linepos < bufsize))
	{
		lineno++;
		if (fgets(lineBuf,255, source))
		{
			if (EchoSource) 
				fprintf(listing, "%4d: %s", lineno, lineBuf);
			bufsize = strlen(lineBuf);
			linepos = 0;
			return lineBuf[linepos++];
		}
		else
		{
			EOF_flag = TRUE;
			return EOF;
		}
	}
	else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{
	if (!EOF_flag) linepos--;
}

/* lookup table of reserved words */
static struct
{
	char* str;
	TokenTypee tok;
} reservedWords[MAXRESERVED]
= { {"if",IF},{"enddo",ENDDO},{"else",ELSE},{"end",END},
   {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
   {"write",WRITE},{"while",WHILE},{"do",DO},{"for",FOR},{"endwhile",ENDWHILE},{"to",TO},{"downto",DOWNTO},
   {"^",POWER}, {"+=",ADDEQ},{"then",THEN} };


/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenTypee reservedLookup(char * s)
{
	int i;
	for (i = 0; i < MAXRESERVED; i++)
		if (!strcmp(s, reservedWords[i].str))
			return reservedWords[i].tok;
	return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the
 * next token in source file
 */
TokenTypee getToken(void)
{  /* index for storing into tokenString */
	int tokenStringIndex = 0;
	/* holds current token to be returned */
	TokenTypee currentToken;
	/* current state - always begins at START */
	StateType state = START;
	/* flag to indicate save to tokenString */
	int save;
	while (state != DONE)
	{
		int c = getNextChar();
		save = TRUE;
		switch (state)
		{
		case START:
			if (isdigit(c))
				state = INNUM;
			else if (isalpha(c))
				state = INID;
			else if (c == ':')
				state = INASSIGN;
			else if ((c == ' ') || (c == '\t') || (c == '\n'))
				save = FALSE;
			else if (c == '{')
			{
				save = FALSE;
				state = INCOMMENT;
			}
			else
			{
				state = DONE;
				switch (c)
				{
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					break;
				case '=':
					currentToken = EQ;
					break;
				case '<':
					currentToken = LT;
					break;
				case '+':
					currentToken = PLUS;
					break;
				case '+=':
					currentToken = ADDEQ;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '*':
					currentToken = TIMES;
					break;
				case '/':
					currentToken = OVER;
					break;
				case '^':
					currentToken = POWER;
					break;
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case ';':
					currentToken = SEMI;
					break;
				case '[':
					currentToken = LBOUND;
					break;
				case ']':
					currentToken = RBOUND;
					break;
				default:
					currentToken = ERRORR;
					break;
				}
			}
			break;
		case INCOMMENT:
			save = FALSE;
			if (c == EOF)
			{
				state = DONE;
				currentToken = ENDFILE;
			}
			else if (c == '}') state = START;
			break;
		case INASSIGN:
			state = DONE;
			if (c == '=')
				currentToken = ASSIGN;
			else
			{ /* backup in the input */
				ungetNextChar();
				save = FALSE;
				currentToken = ERRORR;
			}
			break;
		case INNUM:
			if (!isdigit(c))
			{ /* backup in the input */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = NUM;
			}
			break;
		case INID:
			if (!isalpha(c))
			{ /* backup in the input */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = ID;
			}
			break;
		case DONE:
		default: /* should never happen */
			fprintf(listing, "Scanner Bug: state= %d\n", state);
			state = DONE;
			currentToken = ERRORR;
			break;
		}
		if ((save) && (tokenStringIndex <= MAXTOKENLEN))
			tokenString[tokenStringIndex++] = (char)c;
		if (state == DONE)
		{
			tokenString[tokenStringIndex] = '\0';
			if (currentToken == ID)
				currentToken = reservedLookup(tokenString);
		}
	}
	if (TraceScan) {
		fprintf(listing, "\t%d: ", lineno);
		printToken(currentToken, tokenString);
	}
	return currentToken;
} /* end getToken */

