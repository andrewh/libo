/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011, The Regents of
  the University of California (Regents). 
  Permission to use, copy, modify, distribute, and distribute modified versions
  of this software and its documentation without fee and without a signed
  licensing agreement, is hereby granted, provided that the above copyright
  notice, this paragraph and the following two paragraphs appear in all copies,
  modifications, and distributions.

  IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
  OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
  BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
  HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

/** 	\file osc_expr_parser.y
	\author John MacCallum

*/
%code top{

#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libgen.h>
	//#ifndef WIN_VERSION
	//#include <Carbon.h>
	//#include <CoreServices.h>
	//#endif
#include "osc_expr_builtins.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_funcall.h"
#include "osc_expr_ast_function.h"
#include "osc_expr_ast_unaryop.h"
#include "osc_expr_ast_binaryop.h"
#include "osc_expr_ast_value.h"
#include "osc_expr_ast_arraysubscript.h"
#include "osc_expr_ast_aseq.h"
#include "osc_expr_ast_ternarycond.h"
	//#include "osc_expr_ast_sugar.h"
#include "osc_expr_ast_let.h"
#include "osc_expr_ast_list.h"
#include "osc_error.h"
#include "osc_expr_parser.h"
#include "osc_expr_scanner.h"
#include "osc_util.h"
#include "osc_atom_u.r"
#include "osc_hashtab.h"

	//#define OSC_EXPR_PARSER_DEBUG
#ifdef OSC_EXPR_PARSER_DEBUG
#define PP(fmt, ...)printf(fmt, ##__VA_ARGS__)
#else
#define PP(fmt, ...)
#endif

#ifdef __cplusplus
extern "C" int osc_expr_scanner_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf);
#else
int osc_expr_scanner_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf);
#endif

}
%code requires{
#include "osc.h"
#include "osc_mem.h"
#include "osc_atom_u.h"
#include "osc_expr_ast_expr.h"

#ifdef __cplusplus
#define YY_DECL extern "C" int osc_expr_scanner_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf)
#else
#define YY_DECL int osc_expr_scanner_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf)
#endif
	//t_osc_err osc_expr_parser_parseString(char *ptr, t_osc_expr **f);
#ifdef __cplusplus
extern "C"{
#endif
//t_osc_err osc_expr_parser_parseExpr(char *ptr, t_osc_expr **f);
t_osc_err osc_expr_parser_parseExpr(char *ptr, t_osc_expr_ast_expr **ast);
//t_osc_err osc_expr_parser_parseFunction(char *ptr, t_osc_expr_rec **f);
#ifdef __cplusplus
}
#endif
}

%{

	//#define YYDEBUG 1
	//int yydebug = 1;

// this is a dummy so that the compiler won't complain.  we pass the hard-coded
// value of 1 to osc_expr_scanner_lex() inside of osc_expr_parser_lex() down below.
int alloc_atom = 1;

int osc_expr_parser_lex(YYSTYPE *yylval_param, YYLTYPE *llocp, yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf){
	return osc_expr_scanner_lex(yylval_param, llocp, yyscanner, 1, buflen, buf);
}

/*
t_osc_err osc_expr_parser_parseExpr(char *ptr, t_osc_expr **f)
{
	//printf("parsing %s\n", ptr);
	int len = strlen(ptr);
	int alloc = 0;

	// expressions really have to end with a semicolon, but it's nice to write single
	// expressions without one (or to leave it off the last one), so we add one to the
	// end of the string here just in case.
	if(ptr[len - 1] != ';'){
		char *tmp = osc_mem_alloc(len + 2);
		memcpy(tmp, ptr, len + 1);
		tmp[len] = ';';
		tmp[len + 1] = '\0';
		ptr = tmp;
		alloc = 1;
	}

	yyscan_t scanner;
	osc_expr_scanner_lex_init(&scanner);
	YY_BUFFER_STATE buf_state = osc_expr_scanner__scan_string(ptr, scanner);
	osc_expr_scanner_set_out(NULL, scanner);
	t_osc_expr *exprstack = NULL;
	t_osc_expr *tmp_exprstack = NULL;
	long buflen = 0;
	char *buf = NULL;
	t_osc_err ret = osc_expr_parser_parse(&exprstack, &tmp_exprstack, scanner, ptr, &buflen, &buf);
	osc_expr_scanner__delete_buffer(buf_state, scanner);
	osc_expr_scanner_lex_destroy(scanner);
	if(tmp_exprstack){
		if(exprstack){
			osc_expr_appendExpr(exprstack, tmp_exprstack);
		}else{
			exprstack = tmp_exprstack;
		}
	}
	//osc_expr_parser_foldConstants(exprstack);
	*f = exprstack;
	if(alloc){
		osc_mem_free(ptr);
	}
	return ret;
}
*/

t_osc_err osc_expr_parser_parseExpr(char *ptr, t_osc_expr_ast_expr **ast)
{
	yyscan_t scanner;
	osc_expr_scanner_lex_init(&scanner);
	YY_BUFFER_STATE buf_state = osc_expr_scanner__scan_string(ptr, scanner);
	osc_expr_scanner_set_out(NULL, scanner);
	t_osc_expr_ast_expr *exprstack = NULL;
	t_osc_hashtab *lexenv = osc_hashtab_new(0, NULL);
	long buflen = 0;
	char *buf = NULL;
	t_osc_err ret = osc_expr_parser_parse(&exprstack, lexenv, scanner, ptr, &buflen, &buf);
	osc_expr_scanner__delete_buffer(buf_state, scanner);
	osc_expr_scanner_lex_destroy(scanner);
	*ast = exprstack;
	return ret;
}

void osc_expr_error_formatLocation(YYLTYPE *llocp, char *input_string, char **buf)
{
	int len = strlen(input_string);
	if(llocp->first_column >= len || llocp->last_column >= len){
		*buf = osc_mem_alloc(len + 1);
		strncpy(*buf, input_string, len + 1);
		return;
	}
	char s1[len * 2];
	char s2[len * 2];
	char s3[len * 2];
	memcpy(s1, input_string, llocp->first_column);
	s1[llocp->first_column] = '\0';
	memcpy(s2, input_string + llocp->first_column, llocp->last_column - llocp->first_column);
	s2[llocp->last_column - llocp->first_column] = '\0';
	memcpy(s3, input_string + llocp->last_column, len - llocp->last_column);
	s3[len - llocp->last_column] = '\0';
	*buf = osc_mem_alloc(len * 3 + 24); // way too much
	sprintf(*buf, "%s\n-->                %s\n%s\n", s1, s2, s3);
}

void osc_expr_error(YYLTYPE *llocp,
		    char *input_string,
		    t_osc_err errorcode,
		    const char * const moreinfo_fmt,
		    ...)
{
	char *loc = NULL;
	osc_expr_error_formatLocation(llocp, input_string, &loc);
	int loclen = 0;
	if(loc){
		loclen = strlen(loc);
	}
	va_list ap;
	va_start(ap, moreinfo_fmt);
	char more[256];
	memset(more, '\0', sizeof(more));
	int more_len = 0;
	if(ap){
		more_len += vsnprintf(more, 256, moreinfo_fmt, ap);
		va_end(ap);
	}
	if(loclen || more_len){
		char buf[loclen + more_len];
		char *ptr = buf;
		if(loclen){
			ptr += sprintf(ptr, "%s\n", loc);
		}
		if(more_len){
			ptr += sprintf(ptr, "%s\n", more);
		}
		osc_error_handler(__FILE__, //basename(__FILE__), // basename() seems to crash under cygwin...
				  NULL,
				  -1,
				  errorcode,
				  buf);
	}else{
	  osc_error_handler(__FILE__,//basename(__FILE__),
				  NULL,
				  -1,
				  errorcode,
				  "");
	}
	if(loc){
		osc_mem_free(loc);
	}
}

void yyerror(YYLTYPE *llocp, t_osc_expr_ast_expr **exprstack, t_osc_hashtab *lexenv, void *scanner, char *input_string, long *buflen, char **buf, char const *e)
{
	osc_expr_error(llocp, input_string, OSC_ERR_EXPPARSE, e);
}

void osc_expr_parser_reportUnknownFunctionError(YYLTYPE *llocp,
						 char *input_string,
						 char *function_name)
{
	osc_expr_error(llocp,
		       input_string,
		       OSC_ERR_EXPPARSE,
		       "unknown function \"%s\"",
		       function_name);
}

void osc_expr_parser_reportInvalidLvalError(YYLTYPE *llocp,
					    char *input_string,
					    char *lvalue)
{
	osc_expr_error(llocp,
		       input_string,
		       OSC_ERR_EXPPARSE,
		       "\"%s\" is not a valid target for assignment (invalid lvalue)\n",
		       lvalue);
}

t_osc_expr_ast_expr *osc_expr_parser_reduceBinaryOp(YYLTYPE *llocp,
						    char *input_string,
						    t_osc_expr_ast_expr *left,
						    char opcode,
						    t_osc_expr_ast_expr *right)
{
	/*
	t_osc_expr_rec *r = osc_expr_func_opcodeToOpRec[(int)opcode];
	if(!r){
		return NULL;
	}
	t_osc_expr_ast_binaryop *bo = osc_expr_ast_binaryop_alloc(r, left, right);
	t_osc_expr_ast_funcall *fc = osc_expr_ast_binaryop_toFuncall(r, left, right);
	return (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)bo, (t_osc_expr_ast_expr *)fc);
	*/
	t_osc_expr_oprec *r = osc_expr_builtins_lookupOperatorForOpcode(opcode);
	if(!r){
		return NULL;
	}
	return (t_osc_expr_ast_expr *)osc_expr_ast_binaryop_alloc(r, left, right);
}

t_osc_expr_ast_expr *osc_expr_parser_reduceCompoundAssign(YYLTYPE *llocp,
							  char *input_string,
							  t_osc_expr_ast_expr *left,
							  char compound_assign_opcode,
							  t_osc_expr_ast_expr *right,
							  char opcode)
{
	/*
	t_osc_expr_rec *r = osc_expr_func_opcodeToOpRec[(int)compound_assign_opcode];
	if(!r){
		// this should _never_ happen since this function is only called with a hard coded 
		// opcode identified by the grammar
		return NULL;
	}
	t_osc_expr_ast_binaryop *bo = osc_expr_ast_binaryop_alloc(r, left, right);
	t_osc_expr_ast_funcall *fc = osc_expr_ast_binaryop_toFuncall(osc_expr_func_opcodeToOpRec[(int)opcode], left, right);
	t_osc_expr_ast_funcall *assign = osc_expr_ast_funcall_alloc(&osc_expr_rec_assign, 2, osc_expr_ast_expr_copy(left), fc);
	return (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)bo, (t_osc_expr_ast_expr *)assign);
	*/
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_parser_reducePrefixFunction(YYLTYPE *llocp,
							  char *input_string,
							  char *function_name,
							  t_osc_expr_ast_expr *arglist)
{
	t_osc_expr_funcrec *r = osc_expr_builtins_lookupFunction(function_name);
	if(!r){
		osc_expr_parser_reportUnknownFunctionError(llocp, input_string, function_name);
		return NULL;
	}
	return (t_osc_expr_ast_expr *)osc_expr_ast_funcall_allocWithList(r, arglist);
}

void osc_expr_parser_printlexenv(char *key, void *val, void *context)
{
	printf("%s: %d\n", key, (int)val);
}

void osc_expr_parser_changeCountForVarInLexEnv(t_osc_hashtab *lexenv, char *var, int incdec)
{
	int varlen = strlen(var);
	void *count = osc_hashtab_lookup(lexenv, varlen, var);
	if(count){
		osc_hashtab_store(lexenv, varlen, var, (void *)(((int)count) + incdec));
	}else{
		if(incdec < 0){
			printf("%s:%d: just tried to unbind a lexical var that doesn't exist!\n", __func__, __LINE__);
		}else{
			osc_hashtab_store(lexenv, varlen, var, (void *)1);
		}
	}
}

void osc_expr_parser_addVarToLexEnv(t_osc_hashtab *lexenv, char *var)
{
	osc_expr_parser_changeCountForVarInLexEnv(lexenv, var, 1);
}

void osc_expr_parser_removeVarFromLexEnv(t_osc_hashtab *lexenv, char *var)
{
	osc_expr_parser_changeCountForVarInLexEnv(lexenv, var, -1);
}

int osc_expr_parser_varIsBoundInLexEnv(t_osc_hashtab *lexenv, char *var)
{
	int varlen = strlen(var);
	void *count = osc_hashtab_lookup(lexenv, varlen, var);
	return (int)count;
}

%}

%define "api.pure"
%locations
%require "2.4.2"

// replace this bullshit with a struct...
%parse-param{t_osc_expr_ast_expr **exprstack}
%parse-param{t_osc_hashtab *lexenv}
%parse-param{void *scanner}
%parse-param{char *input_string}
%parse-param{long *buflen}
%parse-param{char **buf}

%lex-param{void *scanner}
%lex-param{int alloc_atom}
%lex-param{long *buflen}
%lex-param{char **buf}

%union {
	t_osc_expr_ast_expr *expr;
	t_osc_atom_u *atom;
	t_osc_msg_u *msg;
	t_osc_bndl_u *bndl;
}

%type <expr>expr expns function funcall oscaddress literal aseq unaryop binaryop ternarycond exprlist value lambdalist let varlist
%type <msg>message messages
%type <bndl>bundle
%token <atom>OSC_EXPR_NUM OSC_EXPR_STRING OSC_EXPR_OSCADDRESS OSC_EXPR_IDENTIFIER
%nonassoc OSC_EXPR_LAMBDA OSC_EXPR_LET
 //%type <func>function
 //%type <arg>arg args 
 //%type <atom> OSC_EXPR_QUOTED_EXPR parameters parameter
 //%nonassoc <atom>OSC_EXPR_NUM OSC_EXPR_STRING OSC_EXPR_OSCADDRESS OSC_EXPR_LAMBDA

// low to high precedence
// adapted from http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B

%precedence oscaddress_prec

// level 16
%right '=' OSC_EXPR_ADDASSIGN 8 OSC_EXPR_SUBASSIGN 9 OSC_EXPR_MULASSIGN 10 OSC_EXPR_DIVASSIGN 11 OSC_EXPR_MODASSIGN 12 OSC_EXPR_POWASSIGN 13

// level 15
%right OSC_EXPR_TERNARY_COND OSC_EXPR_NULLCOALESCE 5 OSC_EXPR_NULLCOALESCEASSIGN 14 '?' ':'

// level 14
%left OSC_EXPR_OR 124

// level 13
%left OSC_EXPR_AND 38

// level 9
%left OSC_EXPR_EQ 1 OSC_EXPR_NEQ 2

// level 8
%left '<' '>' OSC_EXPR_LTE 3 OSC_EXPR_GTE 4

// level 6
%left '+' '-'

// level 5
%left '*' '/' '%'
%left '^' 

// level 3
%right OSC_EXPR_PREFIX_INC OSC_EXPR_PREFIX_DEC  '!'

// level 2
%left OSC_EXPR_INC 6 OSC_EXPR_DEC 7 OSC_EXPR_FUNCALL '(' ')' '[' ']' 

%start expns

%%

literal:    
	OSC_EXPR_NUM {
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_value_allocLiteral($1);
	}
	| OSC_EXPR_STRING {
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_value_allocLiteral($1);
	}
;

// these are the only allowable lvalues 
oscaddress:
	OSC_EXPR_OSCADDRESS {
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_value_allocOSCAddress($1);
	}
	| oscaddress '.' OSC_EXPR_OSCADDRESS {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, '.', (t_osc_expr_ast_expr *)osc_expr_ast_value_allocOSCAddress($3));
  	}
	| oscaddress '[' '[' exprlist ']' ']' {
		/*
		t_osc_expr_ast_arraysubscript *as = osc_expr_ast_arraysubscript_alloc($1, $4);
		t_osc_expr_ast_funcall *fc = osc_expr_ast_arraysubscript_toFuncall($1, $4);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)as, (t_osc_expr_ast_expr *)fc);
		*/
		$$ = NULL;
	}
	| oscaddress '[' '[' aseq ']' ']' {
		/*
		t_osc_expr_ast_arraysubscript *as = osc_expr_ast_arraysubscript_alloc($1, $4);
		t_osc_expr_ast_funcall *fc = osc_expr_ast_arraysubscript_toFuncall($1, $4);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)as, (t_osc_expr_ast_expr *)fc);
		*/
		$$ = NULL;
	}
;

value:
	literal
	| oscaddress %prec oscaddress_prec
	| OSC_EXPR_IDENTIFIER %prec oscaddress_prec {
		if(!osc_expr_parser_varIsBoundInLexEnv(lexenv, osc_atom_u_getStringPtr($1))){
			printf("undefined variable %s\n", osc_atom_u_getStringPtr($1));
			return 1;
		}
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_value_allocIdentifier($1);
	}
;

aseq:
	expr ':' expr {
		/*
		t_osc_expr_ast_aseq *parsed = (t_osc_expr_ast_aseq *)osc_expr_ast_aseq_alloc($1, $3, NULL);
		t_osc_expr_ast_funcall *f = osc_expr_ast_aseq_toFuncall($1, $3, NULL);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)parsed, (t_osc_expr_ast_expr *)f);
		*/
		$$ = NULL;
	}
	| expr ':' expr ':' expr {
		/*
		t_osc_expr_ast_aseq *parsed = (t_osc_expr_ast_aseq *)osc_expr_ast_aseq_alloc($1, $5, $3);
		t_osc_expr_ast_funcall *f = osc_expr_ast_aseq_toFuncall($1, $5, $3);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)parsed, (t_osc_expr_ast_expr *)f);
		*/
		$$ = NULL;
	}
;

function: 
	OSC_EXPR_LAMBDA '(' lambdalist ')' '{' expns '}' {
		//osc_hashtab_foreach(lexenv, osc_expr_parser_printlexenv, NULL);
		t_osc_expr_ast_expr *v = $3;
		while(v){
			osc_expr_parser_removeVarFromLexEnv(lexenv, osc_atom_u_getStringPtr(osc_expr_ast_value_getValue((t_osc_expr_ast_value *)v)));
			v = osc_expr_ast_expr_next(v);
		}
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_function_alloc((t_osc_expr_ast_value *)$3, $6);
	}
;

lambdalist: {$$ = NULL;}
	| OSC_EXPR_IDENTIFIER {
		osc_expr_parser_addVarToLexEnv(lexenv, osc_atom_u_getStringPtr($1));
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_value_allocIdentifier($1);
	}
	| lambdalist ',' OSC_EXPR_IDENTIFIER {
		osc_expr_parser_addVarToLexEnv(lexenv, osc_atom_u_getStringPtr($3));
		osc_expr_ast_expr_append($1, (t_osc_expr_ast_expr *)osc_expr_ast_value_allocIdentifier($3));
		$$ = $1;
	}
;

let:
	OSC_EXPR_LET '(' varlist ')' '{' expns '}' {
		/*
		t_osc_expr_ast_expr *v = $3;
		t_osc_expr_ast_value *lambdalist = NULL;
		t_osc_expr_ast_expr *rval = NULL;
		int nbindings = 0;
		while(v){
			t_osc_expr_ast_binaryop *b = (t_osc_expr_ast_binaryop *)v;
			if(!lambdalist){
				lambdalist = (t_osc_expr_ast_value *)osc_expr_ast_binaryop_getLeftArg(b);
			}else{
				osc_expr_ast_expr_append((t_osc_expr_ast_expr *)lambdalist, osc_expr_ast_binaryop_getLeftArg(b));
			}
			if(!rval){
				rval = osc_expr_ast_binaryop_getRightArg(b);
			}else{
				osc_expr_ast_expr_append((t_osc_expr_ast_expr *)rval, osc_expr_ast_binaryop_getRightArg(b));
			}
			osc_expr_parser_removeVarFromLexEnv(lexenv, osc_atom_u_getStringPtr(osc_expr_ast_value_getValue((t_osc_expr_ast_value *)osc_expr_ast_binaryop_getLeftArg(b))));
			v = osc_expr_ast_expr_next(v);
			nbindings++;
		}
		t_osc_expr_ast_function *lambda = osc_expr_ast_function_alloc(lambdalist, $6);
		osc_expr_ast_expr_append((t_osc_expr_ast_expr *)lambda, rval);
		t_osc_expr_ast_funcall *apply = osc_expr_ast_funcall_allocWithList(&osc_expr_rec_apply, (t_osc_expr_ast_expr *)lambda);
		t_osc_expr_ast_let *let = osc_expr_ast_let_alloc($3, $6);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)let, (t_osc_expr_ast_expr *)apply);
		*/
		$$ = NULL;
	}
;

varlist: {$$ = NULL;}
	| OSC_EXPR_IDENTIFIER '=' expr {
		/*
		osc_expr_parser_addVarToLexEnv(lexenv, osc_atom_u_getStringPtr($1));
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_binaryop_alloc(&osc_expr_rec_op_assign, (t_osc_expr_ast_expr *)osc_expr_ast_value_allocIdentifier($1), $3);
		*/
		$$ = NULL;
  	}
	| varlist ',' OSC_EXPR_IDENTIFIER '=' expr {
		/*
		osc_expr_parser_addVarToLexEnv(lexenv, osc_atom_u_getStringPtr($3));
		osc_expr_ast_expr_append($1, (t_osc_expr_ast_expr *)osc_expr_ast_binaryop_alloc(&osc_expr_rec_op_assign, (t_osc_expr_ast_expr *)osc_expr_ast_value_allocIdentifier($3), $5));
		$$ = $1;
		*/
		$$ = NULL;
  	}
;
	

message:
	OSC_EXPR_OSCADDRESS {
		$$ = osc_message_u_allocWithAddress(osc_atom_u_getStringPtr($1));
		if(alloc_atom){
			osc_mem_free($1);
		}
	}
	| OSC_EXPR_OSCADDRESS ':' expns {
		printf("message\n");
	}
;

messages:
	message {
		printf("message\n");
	}
	| messages ',' message {
		printf("messages\n");
	}
;

bundle:
	'{' '}' { 
		$$ = NULL; 
	}
	| '{' messages '}' {
		printf("yep\n");
  	}
;

unaryop:
// prefix not
	'!' expr {
		/*
		t_osc_expr_ast_unaryop *parsed = osc_expr_ast_unaryop_allocLeft(&osc_expr_rec_op_not, $2);
		t_osc_expr_ast_funcall *func = osc_expr_ast_funcall_alloc(&osc_expr_rec_not, 1, $2);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)parsed, (t_osc_expr_ast_expr *)func);
		*/
		$$ = NULL;
	}
// prefix inc/dec
	| OSC_EXPR_INC oscaddress %prec OSC_EXPR_PREFIX_INC {
		/*
		t_osc_expr_ast_unaryop *parsed = osc_expr_ast_unaryop_allocLeft(&osc_expr_rec_op_add1, (t_osc_expr_ast_expr *)$2);
		t_osc_expr_ast_funcall *fc = osc_expr_ast_funcall_alloc(&osc_expr_rec_add1, 1, (t_osc_expr_ast_expr *)$2);
		t_osc_expr_ast_funcall *assign = osc_expr_ast_funcall_alloc(&osc_expr_rec_assign, 2, osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)$2), fc);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)parsed, (t_osc_expr_ast_expr *)assign);
		*/
		$$ = NULL;
	}
	| OSC_EXPR_DEC oscaddress %prec OSC_EXPR_PREFIX_DEC {
		/*
		t_osc_expr_ast_unaryop *parsed = osc_expr_ast_unaryop_allocLeft(&osc_expr_rec_op_sub1, (t_osc_expr_ast_expr *)$2);
		t_osc_expr_ast_funcall *fc = osc_expr_ast_funcall_alloc(&osc_expr_rec_sub1, 1, (t_osc_expr_ast_expr *)$2);
		t_osc_expr_ast_funcall *assign = osc_expr_ast_funcall_alloc(&osc_expr_rec_assign, 2, osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)$2), fc);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)parsed, (t_osc_expr_ast_expr *)assign);
		*/
		$$ = NULL;
	}
// postfix inc/dec
	| oscaddress OSC_EXPR_INC {
		/*
		t_osc_expr_ast_unaryop *parsed = osc_expr_ast_unaryop_allocRight(&osc_expr_rec_op_add1, (t_osc_expr_ast_expr *)$1);
		t_osc_expr_ast_funcall *fc = osc_expr_ast_funcall_alloc(&osc_expr_rec_add1, 1, (t_osc_expr_ast_expr *)$1);
		t_osc_expr_ast_funcall *assign = osc_expr_ast_funcall_alloc(&osc_expr_rec_assign, 2, osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)$1), fc);
		t_osc_expr_ast_funcall *prog1 = osc_expr_ast_funcall_alloc(&osc_expr_rec_prog1, 2, osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)$1), assign);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)parsed, (t_osc_expr_ast_expr *)prog1);
		*/
		$$ = NULL;
	}
	| oscaddress OSC_EXPR_DEC {
		/*
		t_osc_expr_ast_unaryop *parsed = osc_expr_ast_unaryop_allocRight(&osc_expr_rec_op_sub1, (t_osc_expr_ast_expr *)$1);
		t_osc_expr_ast_funcall *fc = osc_expr_ast_funcall_alloc(&osc_expr_rec_sub1, 1, (t_osc_expr_ast_expr *)$1);
		t_osc_expr_ast_funcall *assign = osc_expr_ast_funcall_alloc(&osc_expr_rec_assign, 2, osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)$1), fc);
		t_osc_expr_ast_funcall *prog1 = osc_expr_ast_funcall_alloc(&osc_expr_rec_prog1, 2, osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)$1), assign);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)parsed, (t_osc_expr_ast_expr *)prog1);
		*/
		$$ = NULL;
	}
;

binaryop:
	// Infix operators
	oscaddress '=' expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, '=', $3);
 	}
	| expr '+' expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, '+', $3);
 	}
	| expr '-' expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, '-', $3);
 	}
	| expr '*' expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, '*', $3);
 	}
	| expr '/' expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, '/', $3);
 	}
	| expr '%' expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, '%', $3);
 	}
	| expr '^' expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, '^', $3);
 	}
	| expr OSC_EXPR_EQ expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, OSC_EXPR_EQ, $3);
 	}
	| expr OSC_EXPR_NEQ expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, OSC_EXPR_NEQ, $3);
 	}
	| expr '<' expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, '<', $3);
 	}
	| expr OSC_EXPR_LTE expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, OSC_EXPR_LTE, $3);
 	}
	| expr '>' expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, '>', $3);
 	}
	| expr OSC_EXPR_GTE expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, OSC_EXPR_GTE, $3);
 	}
	| expr OSC_EXPR_AND expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, OSC_EXPR_AND, $3);
 	}
	| expr OSC_EXPR_OR expr {
		$$ = osc_expr_parser_reduceBinaryOp(&yylloc, input_string, $1, OSC_EXPR_OR, $3);
 	}
	| oscaddress OSC_EXPR_NULLCOALESCE expr {
		/*
		t_osc_expr_ast_binaryop *bo = osc_expr_ast_binaryop_alloc(osc_expr_func_opcodeToOpRec[(int)OSC_EXPR_NULLCOALESCE], (t_osc_expr_ast_expr *)$1, $3);
		t_osc_expr_ast_funcall *bound = osc_expr_ast_funcall_alloc(&osc_expr_rec_bound, 1, (t_osc_expr_ast_expr *)$1);
		t_osc_expr_ast_funcall *fc = osc_expr_ast_funcall_alloc(&osc_expr_rec_if, 3, (t_osc_expr_ast_expr *)bound, osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)$1), $3);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)bo, (t_osc_expr_ast_expr *)fc);
		*/
		$$ = NULL;
	}
	| oscaddress OSC_EXPR_ADDASSIGN expr {
		$$ = osc_expr_parser_reduceCompoundAssign(&yylloc, input_string, $1, OSC_EXPR_ADDASSIGN, $3, '+');
 	}
	| oscaddress OSC_EXPR_SUBASSIGN expr {
		$$ = osc_expr_parser_reduceCompoundAssign(&yylloc, input_string, $1, OSC_EXPR_SUBASSIGN, $3, '-');
 	}
	| oscaddress OSC_EXPR_MULASSIGN expr {
		$$ = osc_expr_parser_reduceCompoundAssign(&yylloc, input_string, $1, OSC_EXPR_MULASSIGN, $3, '*');
 	}
	| oscaddress OSC_EXPR_DIVASSIGN expr {
		$$ = osc_expr_parser_reduceCompoundAssign(&yylloc, input_string, $1, OSC_EXPR_DIVASSIGN, $3, '/');
 	}
	| oscaddress OSC_EXPR_MODASSIGN expr {
		$$ = osc_expr_parser_reduceCompoundAssign(&yylloc, input_string, $1, OSC_EXPR_MODASSIGN, $3, '%');
 	}
	| oscaddress OSC_EXPR_POWASSIGN expr {
		$$ = osc_expr_parser_reduceCompoundAssign(&yylloc, input_string, $1, OSC_EXPR_POWASSIGN, $3, '^');
 	}
 	| oscaddress OSC_EXPR_NULLCOALESCEASSIGN expr {
		/*
		t_osc_expr_ast_binaryop *bo = osc_expr_ast_binaryop_alloc(osc_expr_func_opcodeToOpRec[(int)OSC_EXPR_NULLCOALESCEASSIGN], (t_osc_expr_ast_expr *)$1, $3);
		t_osc_expr_ast_funcall *bound = osc_expr_ast_funcall_alloc(&osc_expr_rec_bound, 1, (t_osc_expr_ast_expr *)$1);
		t_osc_expr_ast_funcall *fc = osc_expr_ast_funcall_alloc(&osc_expr_rec_if, 3, (t_osc_expr_ast_expr *)bound, osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)$1), $3);
		t_osc_expr_ast_funcall *assign = osc_expr_ast_funcall_alloc(&osc_expr_rec_assign, 2, osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)$1), fc);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)bo, (t_osc_expr_ast_expr *)assign);
		*/
		$$ = NULL;
 	}
;

ternarycond:
	expr '?' expr ':' expr %prec OSC_EXPR_TERNARY_COND {
		/*
		t_osc_expr_ast_ternarycond *parsed = osc_expr_ast_ternarycond_alloc($1, $3, $5);
		t_osc_expr_ast_funcall *funcall = osc_expr_ast_funcall_alloc(&osc_expr_rec_if, 3, $1, $3, $5);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)parsed, (t_osc_expr_ast_expr *)funcall);
		*/
		$$ = NULL;
	}
;

funcall:
// prefix function call
	OSC_EXPR_IDENTIFIER '(' exprlist ')' %prec OSC_EXPR_FUNCALL {
		$$ = osc_expr_parser_reducePrefixFunction(&yylloc,
							  input_string,
							  osc_atom_u_getStringPtr($1),
							  $3);
		osc_atom_u_free($1);
		if(!$$){
			osc_expr_ast_expr_free($$);
			$$ = NULL;
			return 1;
		}
  	}
;

expr:
	';' {
		$$ = osc_expr_ast_expr_alloc();
	}
	| value
	| function
	| funcall
	| unaryop 
	| binaryop
	| ternarycond
	| '[' aseq ']' {// %prec oscaddress_prec //{
		$$ = $2;
		osc_expr_ast_expr_setBrackets($$, '[', ']');
	}
	| '[' exprlist ']' {
		/*
		t_osc_expr_ast_funcall *f = osc_expr_ast_funcall_allocWithList(&osc_expr_rec_list, $2);
		$$ = (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc((t_osc_expr_ast_expr *)osc_expr_ast_list_alloc($2), (t_osc_expr_ast_expr *)f);
		osc_expr_ast_expr_setBrackets($$, '[', ']');
		*/
		$$ = NULL;
	}
	| '(' expr ')' {
		$$ = $2;
		osc_expr_ast_expr_setBrackets($$, '(', ')');
  	}
	| bundle {
		printf("bundle...\n");
  	}
	| let
;

exprlist:
	{ $$ = NULL;}
	| expr
	| exprlist ',' expr {
		osc_expr_ast_expr_append($1, $3);
		$$ = $1;
	}
;

expns: 
	expr {
		$$ = $1;
		*exprstack = $1;
	}
	| expns expr {
		osc_expr_ast_expr_append($1, $2);
		*exprstack = $1;
		$$ = $1;
 	}
;

