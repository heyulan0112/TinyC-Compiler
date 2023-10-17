/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

//#include <sstream>
//using namespace std;
//ostringstream cout;

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
static TokenTypee token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * exp(void);
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);
static TreeNode * pow_stmt(void);
static TreeNode * addeq_stmt(void);
static TreeNode * while_stmt(void);
static TreeNode * dowhile_stmt(void);
static TreeNode * for_stmt(void);

static void syntaxError(char * message)
{
	fprintf(listing, "\r\n>>> ");
	fprintf(listing, "Syntax error at line %d: %s", lineno, message);
	Error = TRUE;
}

static void match (TokenTypee expected)
{
	if (token == expected) token = getToken();
	else {
		syntaxError("unexpected token -> ");
		printToken(token, tokenString);
		fprintf(listing, "      ");
	}
}

TreeNode * stmt_sequence(void)
{
	bool semiflag = true;
	if (token == FOR || token == WHILE)
		semiflag = false;
	TreeNode * t = statement();
	if (semiflag)
		match(SEMI);
	TreeNode * p = t;
	while ((token != ENDFILE) && (token != END) &&
		(token != ELSE) && (token != UNTIL) &&
		(token != ENDWHILE) && (token != RBOUND) && (token != ENDDO))
	{
		TreeNode * q;
		bool semiflag2 = true;
		if (token == FOR || token == WHILE)
			semiflag2 = false;
		q = statement();
		if (semiflag2)
			match(SEMI);
		if (q != NULL)
		{
			if (t == NULL) t = p = q;
			else /* now p cannot be NULL either */
			{
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}
//P394 
//lineno: 961
TreeNode * statement(void)
{
	TreeNode * t = NULL;
	switch (token) {
	case IF: t = if_stmt(); break;
	case REPEAT: t = repeat_stmt(); break;
	case ID: t = assign_stmt(); break;
	case READ: t = read_stmt(); break;
	case WRITE: t = write_stmt(); break;
	case WHILE:t = while_stmt(); break;
	case DO:t = dowhile_stmt(); break;
	case FOR:t = for_stmt(); break;
	default:
		syntaxError("unexpected token -> ");
		printToken(token, tokenString);
		token = getToken();
		break;
	} /* end case */
	return t;
}


//P394 
//lineno: 977
TreeNode * if_stmt(void)
{
	TreeNode * t = newStmtNode(IfK);
	match(IF);
	match(LPAREN);
	if (t != NULL) t->child[0] = exp();
	match(RPAREN);
	if (t != NULL) t->child[1] = stmt_sequence();
	if (token == ELSE) {
		match(ELSE);
		if (t != NULL) t->child[2] = stmt_sequence();
	}
	return t;
}

//P394 
//lineno:991
TreeNode * repeat_stmt(void)
{
	TreeNode * t = newStmtNode(RepeatK);
	match(REPEAT);
	if (t != NULL) t->child[0] = stmt_sequence();
	match(UNTIL);
	if (t != NULL) t->child[1] = exp();
	return t;
}

TreeNode * assign_stmt(void)
{
	TreeNode * t = newStmtNode(AssignK);
	if ((t != NULL) && (token == ID))
		t->attr.name = copyString(tokenString);
	match(ID);
	match(ASSIGN);
	if (t != NULL) t->child[0] = exp();
	return t;
}

TreeNode * read_stmt(void)
{
	TreeNode * t = newStmtNode(ReadK);
	match(READ);
	if ((t != NULL) && (token == ID))
		t->attr.name = copyString(tokenString);
	match(ID);
	return t;
}

TreeNode * write_stmt(void)
{
	TreeNode * t = newStmtNode(WriteK);
	match(WRITE);
	if (t != NULL) t->child[0] = exp();
	return t;
}

TreeNode * exp(void)
{
	TreeNode * t = simple_exp();
	if ((token == LT) || (token == EQ)) {
		TreeNode * p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
		}
		match(token);
		if (t != NULL)
			t->child[1] = simple_exp();
	}
	return t;
}

TreeNode * simple_exp(void)
{
	TreeNode *t = term();//左运算数
	while ((token == PLUS) || (token == MINUS))
	{
		TreeNode * p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
			match(token);
			t->child[1] = term();//右运算数
		}
	}
	return t;
}

TreeNode * term(void)
{
	TreeNode * t = factor();
	while ((token == TIMES) || (token == OVER))
	{
		TreeNode * p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
			match(token);
			p->child[1] = factor();
		}
	}
	return t;
}

TreeNode * factor(void)
{
	TreeNode * t = NULL;
	switch (token) {
	case NUM:
		t = newExpNode(ConstK);
		if ((t != NULL) && (token == NUM))
			t->attr.val = atoi(tokenString);
		match(NUM);
		break;
	case ID:
		t = newExpNode(IdK);
		if ((t != NULL) && (token == ID))
			t->attr.name = copyString(tokenString);
		match(ID);
		break;
	case LPAREN:
		match(LPAREN);
		t = exp();
		match(RPAREN);
		break;
	case POWER:
		t = pow_stmt();
		break;
	default:
		syntaxError("unexpected token -> ");
		printToken(token, tokenString);
		token = getToken();
		break;
	}
	return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode * parse(void)
{
	TreeNode * t;
	token = getToken();
	t = stmt_sequence();
	if (token != ENDFILE)
		syntaxError("Code ends before file\n");
	return t;
}

TreeNode * while_stmt(void)
{
	TreeNode * t = newStmtNode(WhileK);
	match(WHILE);
	if (t != NULL) t->child[0] = exp();
	match(DO);
	if (t != NULL) t->child[1] = stmt_sequence();
	match(ENDWHILE);
	return t;
}
static TreeNode * dowhile_stmt(void)
{
	TreeNode * t = newStmtNode(doWhileK);
	match(DO);
	match(LBOUND);
	if (t != NULL) t->child[0] = stmt_sequence();
	match(RBOUND);
	match(WHILE);
	match(LPAREN);
	if (t != NULL) t->child[1] = exp();
	match(RPAREN);
	return t;
}
static TreeNode * for_stmt(void) 
{
	TreeNode * t = newStmtNode(ForK);
	match(FOR);
	if ((t != NULL) && (token == ID))
		t->attr.val = token;
	match(ID);
	match(ASSIGN);
	if (t != NULL) t->child[0] = simple_exp();
	switch (token)
	{
	case TO:
		match(TO);
		if (t != NULL) t->child[1] = simple_exp();
		match(DO);
		if (t != NULL) t->child[2] = stmt_sequence();
		match(ENDDO);
		break;
	case DOWNTO:
		match(DOWNTO);
		if (t != NULL) t->child[1] = simple_exp();
		match(DO);
		if (t != NULL) t->child[2] = stmt_sequence();
		match(ENDDO);
		break;
	}
	return t;
}
static TreeNode * pow_stmt(void)
{
	TreeNode * t = factor();
	if (token == POWER)
	{
		TreeNode * p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
			match(token);
			p->child[1] = factor();
		}
	}
	return t;
}
static TreeNode * addeq_stmt(void)
{
	TreeNode * t = newStmtNode(AddeqK);
	if ((t != NULL) && (token == ID))
		t->attr.name = copyString(tokenString);
	match(ID);
	match(ADDEQ);
	if (t != NULL) t->child[0] = exp();
	return t;
}