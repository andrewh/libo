/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2013-14, The Regents of
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

#include <string.h>
#include "osc_expr_builtin.h"
#include "osc_expr_funcrec.h"
#include "osc_expr_oprec.h"
#include "osc_expr_funcrec.r"
#include "osc_expr_oprec.r"
#include "osc_expr_ast_funcall.h"
#include "osc_expr_ast_function.h"
#include "osc_expr_ast_binaryop.h"
#include "osc_expr_ast_arraysubscript.h"
#include "osc_expr_ast_value.h"
#include "osc_expr_privatedecls.h" 
#include "osc_message_iterator_u.h"
#include "osc_expr_builtin_typedfuncdecls.h"

int _osc_expr_sign(double f);
int osc_expr_builtin_list(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_aseq(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

int osc_expr_builtin_call_binary_func(t_osc_expr_ast_funcall *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

#define OSC_EXPR_BUILTIN_BINARYOP_DECL(funcname, typetag, lhs, rhs, res)\
	int osc_expr_builtin_##funcname##_##typetag(t_osc_atom_u *lhs, t_osc_atom_u *rhs, t_osc_atom_u *res)

#ifdef OSC_EXPR_BUILTIN_BINARYOP_CHECKTYPES
#define OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, lhs, rhs)			\
	if(osc_atom_u_getTypetag(lhs) != typetag || osc_atom_u_getTypetag(rhs) != typetag){	\
		return 1;						\
	}
#else
#define OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, lhs, rhs)
#endif


#define OSC_EXPR_BUILTIN_BINARYOP(op, funcname, typetag, type)			\
	OSC_EXPR_BUILTIN_BINARYOP_DECL(funcname, typetag, a, b, y){	\
		OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, a, b);\
		osc_atom_u_set##type (y, osc_atom_u_get##type (a) op osc_atom_u_get##type (b)); \
		return 0;\
	}

#define OSC_EXPR_BUILTIN_BINARY_CFUNC(funcname, cfuncname, typetag, argtype, restype, casttype) \
	OSC_EXPR_BUILTIN_BINARYOP_DECL(funcname, typetag, a, b, y){\
		OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, a, b);\
		osc_atom_u_set##restype(y, (casttype)cfuncname(osc_atom_u_get##argtype(a), osc_atom_u_get##argtype(b)));\
		return 0;\
	}

OSC_EXPR_BUILTIN_BINARYOP(+, add, c, Int8);
OSC_EXPR_BUILTIN_BINARYOP(+, add, C, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(+, add, u, Int16);
OSC_EXPR_BUILTIN_BINARYOP(+, add, U, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(+, add, i, Int32);
OSC_EXPR_BUILTIN_BINARYOP(+, add, I, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(+, add, h, Int64);
OSC_EXPR_BUILTIN_BINARYOP(+, add, H, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(+, add, f, Float);
OSC_EXPR_BUILTIN_BINARYOP(+, add, d, Double);

OSC_EXPR_BUILTIN_BINARYOP(-, sub, c, Int8);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, C, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, u, Int16);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, U, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, i, Int32);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, I, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, h, Int64);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, H, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, f, Float);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, d, Double);

OSC_EXPR_BUILTIN_BINARYOP(*, mul, c, Int8);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, C, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, u, Int16);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, U, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, i, Int32);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, I, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, h, Int64);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, H, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, f, Float);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, d, Double);

OSC_EXPR_BUILTIN_BINARYOP(/, div, c, Int8);
OSC_EXPR_BUILTIN_BINARYOP(/, div, C, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(/, div, u, Int16);
OSC_EXPR_BUILTIN_BINARYOP(/, div, U, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(/, div, i, Int32);
OSC_EXPR_BUILTIN_BINARYOP(/, div, I, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(/, div, h, Int64);
OSC_EXPR_BUILTIN_BINARYOP(/, div, H, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(/, div, f, Float);
OSC_EXPR_BUILTIN_BINARYOP(/, div, d, Double);

OSC_EXPR_BUILTIN_BINARYOP(<, lt, c, Int8);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, C, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, u, Int16);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, U, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, i, Int32);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, I, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, h, Int64);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, H, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, f, Float);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, d, Double);

OSC_EXPR_BUILTIN_BINARYOP(>, gt, c, Int8);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, C, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, u, Int16);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, U, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, i, Int32);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, I, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, h, Int64);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, H, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, f, Float);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, d, Double);

OSC_EXPR_BUILTIN_BINARYOP(<=, le, c, Int8);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, C, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, u, Int16);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, U, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, i, Int32);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, I, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, h, Int64);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, H, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, f, Float);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, d, Double);

OSC_EXPR_BUILTIN_BINARYOP(>=, ge, c, Int8);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, C, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, u, Int16);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, U, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, i, Int32);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, I, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, h, Int64);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, H, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, f, Float);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, d, Double);

OSC_EXPR_BUILTIN_BINARYOP(%, mod, c, Int8);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, C, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, u, Int16);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, U, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, i, Int32);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, I, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, h, Int64);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, H, UInt64);
OSC_EXPR_BUILTIN_BINARY_CFUNC(mod, fmodf, f, Float, Float, float);
OSC_EXPR_BUILTIN_BINARY_CFUNC(mod, fmod, d, Double, Double, double);
/*
OSC_EXPR_BUILTIN_BINARYOP_DECL(mod, f, a, b, y)
{
	OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, lhs, rhs);
	osc_atom_u_setFloat(y, fmodf(osc_atom_u_getFloat(a), osc_atom_u_getFloat(b)));
	return 0;
}
OSC_EXPR_BUILTIN_BINARYOP_DECL(mod, d, a, b, y)
{
	OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, lhs, rhs);
	osc_atom_u_setDouble(y, fmod(osc_atom_u_getDouble(a), osc_atom_u_getDouble(b)));
	return 0;
}
 */

OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, c, Double, Int8, int8_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, C, Double, UInt8, uint8_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, u, Double, Int16, int16_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, U, Double, UInt16, uint16_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, i, Double, Int32, int32_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, I, Double, UInt32, uint32_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, h, Double, Int64, int64_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, H, Double, UInt64, uint64_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, powf, f, Double, Float, float);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, d, Double, Double, double);

#define OSC_EXPR_BUILTIN_ASEQ_TYPED(typetag, type, ctype)		\
	int osc_expr_builtin_aseq_##typetag(t_osc_atom_u *_min, t_osc_atom_u *_max, t_osc_atom_u *_step, int n, t_osc_atom_ar_u *out){ \
		ctype min = osc_atom_u_get##type(_min);\
		ctype max = osc_atom_u_get##type(_max);\
		ctype step = osc_atom_u_get##type(_step);\
		for(int i = 0; i < n; i++){\
			osc_atom_u_set##type(osc_atom_array_u_get(out, i), min);\
			min += step;\
		}\
		return 0;\
	}

OSC_EXPR_BUILTIN_ASEQ_TYPED(c, Int8, int8_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(C, UInt8, uint8_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(u, Int16, int16_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(U, UInt16, uint16_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(i, Int32, int32_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(I, UInt32, uint32_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(h, Int64, int64_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(H, UInt64, uint64_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(f, Float, float)
OSC_EXPR_BUILTIN_ASEQ_TYPED(d, Double, double)

int osc_expr_builtin_aseq(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	t_osc_atom_u *start_a = osc_atom_array_u_get(argv[0], 0);
	t_osc_atom_u *end_a = osc_atom_array_u_get(argv[1], 0);
	t_osc_atom_u *step_a = osc_atom_array_u_get(argv[2], 0);

	double start = osc_atom_u_getDouble(start_a);
	double end = osc_atom_u_getDouble(end_a);
	double step = osc_atom_u_getDouble(step_a);

	if(step == 0){
		// error
		return 1;
	}
	if(_osc_expr_sign(end - start) != _osc_expr_sign(step)){
		// error
		return 1;
	}
	if(start > end){
		start_a = end_a;
		end_a = osc_atom_array_u_get(argv[0], 0);
		double tmp = start;
		start = end;
		end = start;
	}
	int n = (int)((end - start) / fabs(step)) + 1;
	t_osc_expr_funcrec *rec = osc_expr_ast_funcall_getFuncRec(ast);
	int (*f)(t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*, int, t_osc_atom_ar_u*) = (int (*)(t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*, int, t_osc_atom_ar_u*))osc_expr_funcrec_getFuncForTypetag(rec, osc_atom_u_getTypetag(start_a));
	*out = osc_atom_array_u_alloc(n);
	return f(start_a, end_a, step_a, n, *out);
}

#define osc_expr_builtin_lval_NULL NULL
#define OSC_EXPR_BUILTIN_DEFOP(op, opcode, func, lvalfunc, vtab, lhs, rhs, return, assoc, prec, docstring) \
t_osc_expr_oprec _osc_expr_builtin_op_##func = {\
	#op,\
	2,\
	(char *[]){#lhs, #rhs},			\
	NULL,\
	1,\
	(char *[]){#return},\
	NULL,\
	docstring,\
	assoc,\
	prec,\
	0,\
	opcode\
};\
static t_osc_expr_funcrec _osc_expr_builtin_func_##func = {\
	#func,\
	2,\
	(char *[]){#lhs, #rhs},			\
	NULL,\
	0,\
	1,\
	(char *[]){#return},\
	NULL,\
	docstring,\
	osc_expr_builtin_call_binary_func,\
	osc_expr_builtin_lval_##lvalfunc,	\
	vtab,\
	2,\
	(unsigned int []){0, 1},\
	0,\
	2,\
	(unsigned int []){0, 1},\
	0\
};

//OSC_EXPR_BUILTIN_DEFOP(=, '=', assign, NULL, NULL, _lval, _rval, _rval, OSC_EXPR_PARSER_ASSOC_RIGHT, 16, "Assigns _rval to _lval and returns _rval");
OSC_EXPR_BUILTIN_DEFOP(<, '<', lt, NULL, OSC_EXPR_BUILTIN_LT_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is less than _b or NULL otherwise");
OSC_EXPR_BUILTIN_DEFOP(>, '>', gt, NULL, OSC_EXPR_BUILTIN_GT_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is greater than _b or NULL otherwise");
OSC_EXPR_BUILTIN_DEFOP(<=, OSC_EXPR_LTE, le, NULL, OSC_EXPR_BUILTIN_LE_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is less than or equal to _b or NULL otherwise");
OSC_EXPR_BUILTIN_DEFOP(>=, OSC_EXPR_GTE, ge, NULL, OSC_EXPR_BUILTIN_GE_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is greater than or equal to _b or NULL otherwise");
OSC_EXPR_BUILTIN_DEFOP(+, '+', add, NULL, OSC_EXPR_BUILTIN_ADD_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 6, "Returns the sum of its arguments");
OSC_EXPR_BUILTIN_DEFOP(-, '-', sub, NULL, OSC_EXPR_BUILTIN_SUB_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 6, "Returns the difference of its arguments");
OSC_EXPR_BUILTIN_DEFOP(*, '*', mul, NULL, OSC_EXPR_BUILTIN_MUL_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 5, "Returns the product of its arguments");
OSC_EXPR_BUILTIN_DEFOP(/, '/', div, NULL, OSC_EXPR_BUILTIN_DIV_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 5, "Returns the quotient of its arguments");
OSC_EXPR_BUILTIN_DEFOP(%, '%', mod, NULL, OSC_EXPR_BUILTIN_MOD_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 5, "Returns the remainder of _a divided by _b");
OSC_EXPR_BUILTIN_DEFOP(^, '^', pow, NULL, OSC_EXPR_BUILTIN_POW_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 4, "Returns _a raised to the power of _b");
//OSC_EXPR_BUILTIN_DEFOP(., '.', lookup, lookup, NULL, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 2, "Returns the value bound to _b in a subbundle associated with _a");

static t_osc_expr_oprec _osc_expr_builtin_op_assign = {
	"=",
	2,
	(char *[]){"_lval", "_rval"},
	NULL,
	1,
	(char *[]){"_rval"},
	NULL,
	"Assigns _rval to _lval and returns _rval",
	OSC_EXPR_PARSER_ASSOC_RIGHT,
	16,
	0,
	'='
};

static t_osc_expr_funcrec _osc_expr_builtin_func_assign = {
	"assign",
	2,
	(char *[]){"_lval", "_rval"},
	NULL,
	0,
	1,
	(char *[]){"_rval"},
	NULL,
	"Assigns _rval to _lval and returns _rval",
	osc_expr_builtin_assign,
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_apply = {
	"apply",
	2,
	(char *[]){"_function", "_args"},
	NULL,
	1,
	1,
	(char *[]){"_result"},
	NULL,
	"Applies _function to _args and returns the result",
	osc_expr_builtin_apply,
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_nth = {
	"nth",
	2,
	(char *[]){"_list", "_index"},
	NULL,
	0,
	1,
	(char *[]){"_item"},
	NULL,
	"Returns the nth item in a list (counting from 0)",
	osc_expr_builtin_nth,
	osc_expr_builtin_lval_nth,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_oprec _osc_expr_builtin_op_lookup = {
	".",
	2,
	(char *[]){"_a", "_b"},
	NULL,
	1,
	(char *[]){"_y"},
	NULL,
	"Returns the value bound to _b in a subbundle associated with _a",
	OSC_EXPR_PARSER_ASSOC_LEFT,
	2,
	0,
	'.'
};

static t_osc_expr_funcrec _osc_expr_builtin_func_lookup = {
	"lookup",
	2,
	(char *[]){"_a", "_b"},
	NULL,
	0,
	1,
	(char *[]){"_y"},
	NULL,
	"Returns the value bound to _b in a subbundle associated with _a",
	osc_expr_builtin_lookup,
	osc_expr_builtin_lval_lookup,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_list = {
	"list",
	1,
	(char *[]){"_listelems"},
	NULL,
	1,
	1,
	(char *[]){"_list"},
	NULL,
	"Returns a list containing all elements passed as arguments",
	osc_expr_builtin_list,
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_aseq = {
	"aseq",
	3,
	(char *[]){"_min", "_max", "_step"},
	NULL,
	0,
	1,
	(char *[]){"_aseq"},
	NULL,
	"Returns an arithmetic sequence counting from min to max.",
	osc_expr_builtin_aseq,
	NULL,
	OSC_EXPR_BUILTIN_ASEQ_VTAB,
	0,
	NULL,
	0,
	3,
	(unsigned int []){0, 1, 2},
	0,
};

static t_osc_expr_funcrec _osc_expr_builtin_func_if = {
	"if",
	3,
	(char *[]){"_test", "_then", "_else"},
	NULL,
	0,
	1,
	(char *[]){"_result"},
	NULL,
	"Evaluates _then if _test is true, and _else if it is false. Returns the result of the evaluated expression(s).",
	osc_expr_builtin_if,
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

t_osc_expr_oprec *osc_expr_builtin_op_add = &_osc_expr_builtin_op_add;
t_osc_expr_oprec *osc_expr_builtin_op_assign = &_osc_expr_builtin_op_assign;

t_osc_expr_funcrec *osc_expr_builtin_func_add = &_osc_expr_builtin_func_add;
t_osc_expr_funcrec *osc_expr_builtin_func_assign = &_osc_expr_builtin_func_assign;
t_osc_expr_funcrec *osc_expr_builtin_func_apply = &_osc_expr_builtin_func_apply;
t_osc_expr_funcrec *osc_expr_builtin_func_nth = &_osc_expr_builtin_func_nth;
t_osc_expr_funcrec *osc_expr_builtin_func_list = &_osc_expr_builtin_func_list;
t_osc_expr_funcrec *osc_expr_builtin_func_aseq = &_osc_expr_builtin_func_aseq;
t_osc_expr_funcrec *osc_expr_builtin_func_if = &_osc_expr_builtin_func_if;
t_osc_expr_funcrec *osc_expr_builtin_func_lookup = &_osc_expr_builtin_func_lookup;

static t_osc_expr_funcrec *osc_expr_builtin_fsymtab[] = {
	&_osc_expr_builtin_func_assign,
	&_osc_expr_builtin_func_lt,
	&_osc_expr_builtin_func_gt,
	&_osc_expr_builtin_func_le,
	&_osc_expr_builtin_func_ge,
	&_osc_expr_builtin_func_add,
	&_osc_expr_builtin_func_sub,
	&_osc_expr_builtin_func_mul,
	&_osc_expr_builtin_func_div,
	&_osc_expr_builtin_func_mod,
	&_osc_expr_builtin_func_pow,
	&_osc_expr_builtin_func_lookup,
	&_osc_expr_builtin_func_apply,
	&_osc_expr_builtin_func_nth,
	&_osc_expr_builtin_func_list,
	&_osc_expr_builtin_func_aseq,
	&_osc_expr_builtin_func_if,
};

static t_osc_expr_oprec *osc_expr_builtin_osymtab[] = {
	&_osc_expr_builtin_op_assign,
	&_osc_expr_builtin_op_lt,
	&_osc_expr_builtin_op_gt,
	&_osc_expr_builtin_op_le,
	&_osc_expr_builtin_op_ge,
	&_osc_expr_builtin_op_add,
	&_osc_expr_builtin_op_sub,
	&_osc_expr_builtin_op_mul,
	&_osc_expr_builtin_op_div,
	&_osc_expr_builtin_op_mod,
	&_osc_expr_builtin_op_pow,
	&_osc_expr_builtin_op_lookup,
};

// this array can be indexed by either the ascii char of an operator,
// or in the case of multi character operators such as <=, by the 
// token defined in osc_expr_parser.y
static t_osc_expr_oprec *osc_expr_builtin_opcodeToOpRec[128] = {
	NULL	,// 0
	NULL, //&osc_expr_builtin_op_eq	,
	NULL, //&osc_expr_builtin_op_neq	,
	&_osc_expr_builtin_op_le	,
	&_osc_expr_builtin_op_ge	,
	NULL, //&_osc_expr_builtin_op_nullcoalesce,
	NULL, //&osc_expr_builtin_op_add1,
	NULL, //&osc_expr_builtin_op_sub1,
	NULL, //&osc_expr_builtin_op_addassign,
	NULL, //&osc_expr_builtin_op_subassign,
	NULL, //&osc_expr_builtin_op_mulassign, // 10
	NULL, //&osc_expr_builtin_op_divassign, 
	NULL, //&osc_expr_builtin_op_modassign,
	NULL, //&osc_expr_builtin_op_powassign,
	NULL, //&osc_expr_builtin_op_nullcoalesceassign,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 20
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 30
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtin_op_not	,
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtin_op_mod	,
	NULL, //&osc_expr_builtin_op_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	&_osc_expr_builtin_op_mul	,
	&_osc_expr_builtin_op_add	,
	NULL	,
	&_osc_expr_builtin_op_sub	,
	&_osc_expr_builtin_op_lookup,
	&_osc_expr_builtin_op_div	,
	NULL	,
	NULL	,
	NULL	,// 50
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	&_osc_expr_builtin_op_lt	,// 60
	&_osc_expr_builtin_op_assign	,
	&_osc_expr_builtin_op_gt	,
	NULL,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 70
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
		NULL	,// 80
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 90
	NULL	,
	NULL	,
	NULL	,
	&_osc_expr_builtin_op_pow	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
		NULL	,// 100
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
		NULL	,// 110
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 120
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtin_op_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

static t_osc_expr_funcrec *osc_expr_builtin_opcodeToFuncRec[128] = {
	NULL	,// 0
	NULL, //&osc_expr_builtin_func_eq	,
	NULL, //&osc_expr_builtin_func_neq	,
	&_osc_expr_builtin_func_le	,
	&_osc_expr_builtin_func_ge	,
	NULL    ,
	NULL    ,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 10
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 20
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 30
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtin_func_not	,
	NULL	,
	NULL	,
	NULL	,
	&_osc_expr_builtin_func_mod	,
	NULL, //&osc_expr_builtin_func_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	&_osc_expr_builtin_func_mul	,
	&_osc_expr_builtin_func_add	,
	NULL	,
	&_osc_expr_builtin_func_sub	,
	&_osc_expr_builtin_func_lookup,
	&_osc_expr_builtin_func_div	,
	NULL	,
	NULL	,
	NULL	,// 50
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	&_osc_expr_builtin_func_lt	, // 60
	&_osc_expr_builtin_func_assign	,
	&_osc_expr_builtin_func_gt	,
	NULL, //&osc_expr_builtin_func_nullcoalesce,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 70
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 80
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 90
	NULL	,
	NULL	,
	NULL	,
	&_osc_expr_builtin_func_pow	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 100
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 110
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 120
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtin_func_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

static char *osc_expr_builtin_opcodeToString[128] = {
	NULL	,// 0
	NULL, //&osc_expr_builtin_func_eq	,
	NULL, //&osc_expr_builtin_func_neq	,
	"<="	,
	">="	,
	NULL    ,
	NULL    ,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 10
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 20
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 30
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtin_func_not	,
	NULL	,
	NULL	,
	NULL	,
	"%"	,
	NULL, //&osc_expr_builtin_func_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	"*"	,
	"+"	,
	NULL	,
	"-",
	".",
	"/",
	NULL	,
	NULL	,
	NULL	,// 50
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	"<",
        "="     ,
	">",
	NULL, //&osc_expr_builtin_func_nullcoalesce,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 70
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 80
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 90
	NULL	,
	NULL	,
	NULL	,
	"^",
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 100
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 110
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 120
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtin_func_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

t_osc_expr_funcrec *osc_expr_builtin_lookupFunction(char *name)
{
	const int n = sizeof(osc_expr_builtin_fsymtab) / sizeof(t_osc_expr_funcrec*);
	for(int i = 0; i < n; i++){
		if(!strcmp(name, osc_expr_funcrec_getName(osc_expr_builtin_fsymtab[i]))){
			return osc_expr_builtin_fsymtab[i];
		}
	}
	return NULL;
}

t_osc_expr_oprec *osc_expr_builtin_lookupOperator(char *op)
{
	const int n = sizeof(osc_expr_builtin_osymtab) / sizeof(t_osc_expr_oprec*);
	for(int i = 0; i < n; i++){
		if(!strcmp(op, osc_expr_oprec_getName(osc_expr_builtin_osymtab[i]))){
			return osc_expr_builtin_osymtab[i];
		}
	}
	return NULL;
}

t_osc_expr_oprec *osc_expr_builtin_lookupOperatorForOpcode(char op)
{
	if(op > 0 && op < sizeof(osc_expr_builtin_opcodeToFuncRec) / sizeof(char *)){
		char *str = osc_expr_builtin_opcodeToString[(int)op];
		t_osc_expr_oprec *or = osc_expr_builtin_lookupOperator(str);
		return or;
	}
	return NULL;
}

t_osc_expr_funcrec *osc_expr_builtin_lookupFunctionForOperator(t_osc_expr_oprec *op)
{
	char bytecode = osc_expr_oprec_getBytecode(op);
	return osc_expr_builtin_lookupFunctionForOpcode(bytecode);
}

t_osc_expr_funcrec *osc_expr_builtin_lookupFunctionForOpcode(char op)
{
	if(op > 0 && op < sizeof(osc_expr_builtin_opcodeToFuncRec) / sizeof(t_osc_expr_funcrec*)){
		return osc_expr_builtin_opcodeToFuncRec[(int)op];
	}
	return NULL;
}
typedef int (*t_osc_expr_builtin_binary_func)(t_osc_atom_u *, t_osc_atom_u *, t_osc_atom_u *);
int osc_expr_builtin_call_binary_func(t_osc_expr_ast_funcall *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	// scalars should have been expanded, so all arg lists should be the same length
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_expr_funcrec *rec = osc_expr_ast_funcall_getFuncRec(f);
		t_osc_atom_u *a = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *b = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		t_osc_expr_builtin_binary_func f = (t_osc_expr_builtin_binary_func)osc_expr_funcrec_getFuncForTypetag(rec, osc_atom_u_getTypetag(a));
		if(f){
			int ret = f(a, b, y);
			if(ret){
				return ret;
			}
		}else{
			return 1;
		}
	}
	return 0;
}

//////////////////////////////////////////////////
// built in c functions
//////////////////////////////////////////////////


// binary op that produces a numeric value
/*
#define OSC_EXPR_BUILTIN_BINARYOP_CASE(tt, type, op) case tt: osc_atom_u_set##type (y, osc_atom_u_get##type (l) op osc_atom_u_get##type (r)); break;

#define OSC_EXPR_BUILTIN_BINARYOP_SWITCH_INT(op)		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('i', Int32, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('c', Int8, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('C', UInt8, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('u', Int16, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('U', UInt16, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('I', UInt32, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('h', Int64, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('H', UInt64, op);

#define OSC_EXPR_BUILTIN_BINARYOP_SWITCH_FLOAT(op)		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('f', Float, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('d', Double, op);

#define OSC_EXPR_BUILTIN_BINARYOP_SWITCH_NUMERIC(op)	\
	OSC_EXPR_BUILTIN_BINARYOP_SWITCH_INT(op);	\
	OSC_EXPR_BUILTIN_BINARYOP_SWITCH_FLOAT(op);

// binary op that has to be called as a function
#define OSC_EXPR_BUILTIN_BINARYFUNC_CASE(tt, type, func) case tt: osc_atom_u_set##type (y, func (osc_atom_u_get##type (l), osc_atom_u_get##type (r))); break;

#define OSC_EXPR_BUILTIN_BINARYFUNC_SWITCH_INT(func)		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('i', Int32, func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('c', Int8, func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('C', UInt8, func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('u', Int16, func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('U', UInt16, func);	\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('I', UInt32, func);	\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('h', Int64, func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('H', UInt64, func);

#define OSC_EXPR_BUILTIN_BINARYFUNC_SWITCH_FLOAT(float_func, double_func)		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('f', Float, float_func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('d', Double, double_func);

#define OSC_EXPR_BUILTIN_BINARYFUNC_SWITCH_NUMERIC(int_func, float_func, double_func) \
	OSC_EXPR_BUILTIN_BINARYFUNC_SWITCH_INT(int_func);	\
	OSC_EXPR_BUILTIN_BINARYFUNC_SWITCH_FLOAT(float_func, double_func);

// binary op that produces a bool value
#define OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE(tt, type, op) case tt: osc_atom_u_setBool (y, osc_atom_u_get##type (l) op osc_atom_u_get##type (r)); break;

#define OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_INT(op)		\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('i', Int32, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('c', Int8, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('C', UInt8, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('u', Int16, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('U', UInt16, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('I', UInt32, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('h', Int64, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('H', UInt64, op);

#define OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_FLOAT(op)		\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('f', Float, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('d', Double, op);

#define OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_NUMERIC(op)	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_INT(op);		\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_FLOAT(op);

int osc_expr_builtin_add(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	// all argument vectors have been expanded to be the same length
	// all types have been harmonized
	// arity has been checked
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_SWITCH_NUMERIC(+);
			// string
			// bundle
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_sub(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_SWITCH_NUMERIC(-);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_mul(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_SWITCH_NUMERIC(*);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_div(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_SWITCH_NUMERIC(/);
			// timetag
		default:
			;
		}
	}

	return 0;
}

int osc_expr_builtin_mod(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_CASE('c', Int8, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('C', UInt8, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('u', Int16, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('U', UInt16, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('i', Int32, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('I', UInt32, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('h', Int64, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('H', UInt64, %);
			OSC_EXPR_BUILTIN_BINARYFUNC_CASE('f', Float, fmodf);
			OSC_EXPR_BUILTIN_BINARYFUNC_CASE('d', Double, fmod);
			// timetag
		default:
			;
		}
	}

	return 0;
}

int osc_expr_builtin_pow(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		double l = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], i));
		double r = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], i));
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[0], i))){			
		case 'c':
			osc_atom_u_setInt8(y, (int8_t)pow(l, r));
			break;
		case 'C':
			osc_atom_u_setUInt8(y, (uint8_t)pow(l, r));
			break;
		case 'u':
			osc_atom_u_setInt16(y, (int16_t)pow(l, r));
			break;
		case 'U':
			osc_atom_u_setUInt16(y, (uint16_t)pow(l, r));
			break;
		case 'i':
			osc_atom_u_setInt32(y, (int32_t)pow(l, r));
			break;
		case 'I':
			osc_atom_u_setUInt32(y, (uint32_t)pow(l, r));
			break;
		case 'h':
			osc_atom_u_setInt64(y, (int64_t)pow(l, r));
			break;
		case 'H':
			osc_atom_u_setUInt64(y, (uint64_t)pow(l, r));
			break;
		case 'f':
			osc_atom_u_setFloat(y, powf(l, r));
			break;
		case 'd':
			osc_atom_u_setDouble(y, pow(l, r));
			break;
			// timetag
		default:
			;
		}
	}

	return 0;
}

int osc_expr_builtin_lt(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_NUMERIC(<);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_gt(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_NUMERIC(>);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_le(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_NUMERIC(<=);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_ge(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_NUMERIC(>=);
			// timetag
		default:
			;
		}
	}
	return 0;
}
*/

int osc_expr_builtin_list(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int outlen = 0;
	int i;
	for(i = 0; i < argc; i++){
		outlen += osc_atom_array_u_getLen(argv[i]);
	}
	if(outlen <= 0){
		return 0;
	}
	*out = osc_atom_array_u_alloc(outlen);
	int pos = 0;
	for(i = 0; i < argc; i++){
		osc_atom_array_u_copyInto(out, argv[i], pos);
		pos += osc_atom_array_u_getLen(argv[i]);
	}
	return 0;
}

int _osc_expr_sign(double f)
{
	if(f == 0){
		return 0;
	}else if(f < 0){
		return -1;
	}else{
		return 1;
	}
}

//////////////////////////////////////////////////
// special functions
//////////////////////////////////////////////////
void printbndl(t_osc_bndl_u *bndl)
{
	long len = osc_bundle_u_nformat(NULL, 0, bndl, 0);
	char buf[len + 1];
	osc_bundle_u_nformat(buf, len + 1, bndl, 0);
	printf("%s\n", buf);
}

int osc_expr_specFunc_apply(t_osc_expr_ast_funcall *f, 
			    t_osc_expr_lexenv *lexenv, 
			    t_osc_bndl_u *oscbndl,
			    t_osc_atom_ar_u **out)
{
	t_osc_expr_ast_expr *first_expr = osc_expr_ast_funcall_getArgs(f);
	t_osc_expr_ast_expr *rest_expr = osc_expr_ast_expr_next(first_expr);
	t_osc_atom_ar_u *first = NULL;
	int ret = 0;
	ret = osc_expr_ast_expr_evalInLexEnv(first_expr, lexenv, oscbndl, &first);
	t_osc_atom_u *a = osc_atom_array_u_get(first, 0);
	if(ret){
		goto out;
	}
	if(!first){
		ret = 1;
		goto out;
	}

	t_osc_expr_ast_function *func = NULL;
	switch(osc_atom_u_getTypetag(a)){
	case OSC_EXPR_TYPETAG:
		{
			t_osc_expr_ast_expr *e = osc_atom_u_getExpr(a);
			if(osc_expr_ast_expr_getNodetype(e) == OSC_EXPR_AST_NODETYPE_FUNCTION){
				func = (t_osc_expr_ast_function *)e;
			}
		}
		break;
	case 's':
		printf("string: %s\n", osc_atom_u_getStringPtr(a));
		break;
	default:
		printf("found a '%c' (%d) as the first arg to apply()\n", osc_atom_u_getTypetag(a), osc_atom_u_getTypetag(a));
		ret = 1;
		goto out;
	}
	t_osc_expr_ast_value *lambdalist = osc_expr_ast_function_getLambdaList(func);
	t_osc_expr_lexenv *my_lexenv = NULL;
	osc_expr_lexenv_deepCopy(&my_lexenv, lexenv);
	while(lambdalist && rest_expr){
		t_osc_atom_ar_u *ar = NULL;
		int ret = osc_expr_ast_expr_evalInLexEnv(rest_expr, lexenv, oscbndl, &ar);
		t_osc_atom_u *param = osc_expr_ast_value_getIdentifier(lambdalist);
		osc_expr_lexenv_bind(my_lexenv, osc_atom_u_getStringPtr(param), ar);
		lambdalist = (t_osc_expr_ast_function *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist);
		rest_expr = osc_expr_ast_expr_next(rest_expr);
	}
	while(lambdalist){
		t_osc_atom_u *param = osc_expr_ast_value_getIdentifier(lambdalist);
		osc_expr_lexenv_bind(my_lexenv, osc_atom_u_getStringPtr(param), NULL);
		lambdalist = (t_osc_expr_ast_function *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist);
	}
	t_osc_expr_ast_expr *exprlist = osc_expr_ast_function_getExprs(func);
	while(exprlist){
		int ret = osc_expr_ast_expr_evalInLexEnv(exprlist, my_lexenv, oscbndl, out);
		exprlist = osc_expr_ast_expr_next(exprlist);
		if(exprlist){
			osc_atom_array_u_free(*out);
			*out = NULL;
		}
	}
	osc_expr_lexenv_free(my_lexenv);
 out:
	osc_atom_array_u_free(first);
	return ret;
}

int osc_expr_specFunc_assign(t_osc_expr_ast_funcall *f, 
			     t_osc_expr_lexenv *lexenv, 
			     t_osc_bndl_u *oscbndl,
			     t_osc_atom_ar_u **out)
{
	if(!oscbndl){
		return 1;
	}
	t_osc_msg_u *at = NULL;
	t_osc_atom_u **ar = NULL;
	long len = 0;
	t_osc_expr_ast_expr *lval = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)f);
	t_osc_expr_ast_expr *rval = osc_expr_ast_expr_next(lval);
	t_osc_err ret = 0;
	switch(osc_expr_ast_expr_getNodetype(lval)){
	case OSC_EXPR_AST_NODETYPE_VALUE:
		{
			osc_expr_ast_expr_evalLvalInLexEnv(lval, lexenv, oscbndl, &at, &len, &ar);
			osc_expr_ast_expr_evalInLexEnv(rval, lexenv, oscbndl, out);
			if(at){
				osc_message_u_clearArgs(at);
				osc_message_u_setArgArrayCopy(at, *out);
			}else{
				t_osc_msg_u *m = osc_message_u_allocWithArray(osc_atom_u_getStringPtr(osc_expr_ast_value_getOSCAddress((t_osc_expr_ast_value *)lval)), *out);
				osc_bundle_u_addMsg(oscbndl, m);
			}
		}
		break;
	case OSC_EXPR_AST_NODETYPE_ARRAYSUBSCRIPT:
		{
			int ret = osc_expr_ast_expr_evalLvalInLexEnv(lval, lexenv, oscbndl, &at, &len, &ar);
			if(at){
				int ret = osc_expr_ast_expr_evalInLexEnv(rval, lexenv, oscbndl, out);
				for(int i = 0; i < len; i++){
					osc_atom_u_copyValue(ar[i], osc_atom_array_u_get(*out, i));
				}
			}else{
				// error
				ret = 1;
				goto cleanup;
			}
		}
		break;
	case OSC_EXPR_AST_NODETYPE_FUNCALL:
	case OSC_EXPR_AST_NODETYPE_BINARYOP:
		{
			t_osc_expr_ast_expr *llval = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)lval);
			t_osc_expr_ast_expr *rlval = osc_expr_ast_expr_next(llval);
			osc_expr_ast_expr_evalLvalInLexEnv(llval, lexenv, oscbndl, &at, &len, &ar);
			char *address = osc_atom_u_getStringPtr(osc_expr_ast_value_getOSCAddress((t_osc_expr_ast_value *)rlval));
			if(at){
				osc_expr_ast_expr_evalInLexEnv(rval, lexenv, oscbndl, out);
				t_osc_bndl_u *b = NULL;
				for(int i = 0; i < len; i++){
					if(osc_atom_u_getTypetag(ar[i]) == OSC_BUNDLE_TYPETAG){
						b = osc_atom_u_getBndl(ar[i]);
						break;
					}
				}
				if(!b){
					b = osc_bundle_u_alloc();
					osc_message_u_appendBndl_u(at, b, 1);
				}
				t_osc_msg_u *m = osc_message_u_allocWithAddress(address);
				for(int i = 0; i < osc_atom_array_u_getLen(*out); i++){
					t_osc_atom_u *copy = NULL;
					osc_atom_u_copy(&copy, osc_atom_array_u_get(*out, i));
					osc_message_u_appendAtom(m, copy);
				}
				osc_bundle_u_addMsg(b, m);
			}else{
				t_osc_bndl_u *b = osc_bundle_u_alloc();
				t_osc_atom_u *a = osc_atom_u_alloc();
				osc_atom_u_setBndl_u(a, b, 1);
				t_osc_expr_ast_value *v = osc_expr_ast_value_allocLiteral(a);
				t_osc_expr_ast_funcall *assign = osc_expr_ast_funcall_alloc(osc_expr_builtin_func_assign, 2, osc_expr_ast_expr_copy(llval), v);
				t_osc_atom_array_u *tmp = NULL;
				ret = osc_expr_specFunc_assign(assign, lexenv, oscbndl, &tmp);
				if(tmp){
					osc_atom_array_u_free(tmp);
				}
				osc_expr_ast_funcall_free((t_osc_expr_ast_expr *)assign);
				if(ret){
					goto cleanup;
				}
				ret = osc_expr_specFunc_assign(f, lexenv, oscbndl, out);
			}
		}
		break;
	}
 cleanup:
	if(ar){
		osc_mem_free(ar);
	}
	return ret;
}

int osc_expr_specFunc_lval_lookup(t_osc_expr_ast_funcall *f, 
				  t_osc_expr_lexenv *lexenv, 
				  t_osc_bndl_u *oscbndl,
				  t_osc_msg_u **assign_target,
				  long *nlvals,
				  t_osc_atom_u ***lvals)
{
	t_osc_expr_ast_expr *lhs = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)f);
	t_osc_expr_ast_expr *rhs = osc_expr_ast_expr_next(lhs);
	if(rhs == NULL){
		rhs = lhs;
		lhs = NULL;
	}
	t_osc_bndl_u *b = NULL;
	if(lhs){
		t_osc_msg_u *at = NULL;
		t_osc_atom_u **ar = NULL;
		long len = 0;
		osc_expr_ast_expr_evalLvalInLexEnv(lhs, lexenv, oscbndl, &at, &len, &ar);
		if(ar){
			for(int i = 0; i < len; i++){
				t_osc_atom_u *a = ar[i];
				if(osc_atom_u_getTypetag(a) == OSC_BUNDLE_TYPETAG){
					b = osc_atom_u_getBndl(a);
					break;
				}
			}
		}else{
			return 1;
		}
		osc_mem_free(ar);
	}else{
		b = oscbndl;
	}
	if(b){
		return osc_expr_ast_value_evalLvalInLexEnv(rhs, lexenv, b, assign_target, nlvals, lvals);
	}else{
		return 1;
	}
	return 0;
}

int osc_expr_specFunc_lookup(t_osc_expr_ast_funcall *f, 
			     t_osc_expr_lexenv *lexenv, 
			     t_osc_bndl_u *oscbndl,
			     t_osc_atom_ar_u **out)
{
	t_osc_msg_u *assign_target = NULL;
	long nlvals = 0;
	t_osc_atom_u **lvals = NULL;
	osc_expr_specFunc_lval_lookup(f, lexenv, oscbndl, &assign_target, &nlvals, &lvals);
	if(lvals){
		*out = osc_atom_array_u_alloc(nlvals);
		for(int i = 0; i < nlvals; i++){
			t_osc_atom_u *dest = osc_atom_array_u_get(*out, i);
			osc_atom_u_copy(&dest, lvals[i]);
		}
		osc_mem_free(lvals);
	}else{
		return 1;
	}
	return 0;
}

// nth doesn't seem like it needs to be a special function, but it does
// because one of its arguments needs to be evaluated in an lval context
// and the other must not be. 
int osc_expr_specFunc_lval_nth(t_osc_expr_ast_funcall *f, 
			       t_osc_expr_lexenv *lexenv, 
			       t_osc_bndl_u *oscbndl,
			       t_osc_msg_u **assign_target,
			       long *nlvals,
			       t_osc_atom_u ***lvals)
{
	t_osc_expr_ast_expr *vals_expr = osc_expr_ast_funcall_getArgs(f);
	t_osc_expr_ast_expr *idxs_expr = osc_expr_ast_expr_next(vals_expr);
	long nlvals_tmp = 0;
	int ret = osc_expr_ast_expr_evalLvalInLexEnv(vals_expr, lexenv, oscbndl, assign_target, &nlvals_tmp, lvals);
	if(ret){
		return ret;
	}
	if(!(*lvals)){
		// error
		return 1;
	}
	t_osc_atom_ar_u *idxs = NULL;
	osc_expr_ast_expr_evalInLexEnv(idxs_expr, lexenv, oscbndl, &idxs);
	if(!idxs){
		// error
		return 1;
	}
	long nidxs = osc_atom_array_u_getLen(idxs);
	*nlvals = nidxs;
	t_osc_atom_u *lvals_tmp[nlvals_tmp];
	memcpy(lvals_tmp, *lvals, nlvals_tmp * sizeof(t_osc_atom_u *));
	memset(*lvals, '\0', nlvals_tmp * sizeof(t_osc_atom_u*));
	for(int i = 0; i < nidxs; i++){
		int idx = osc_atom_u_getInt(osc_atom_array_u_get(idxs, i));
		if(idx >= nlvals_tmp){
			// index out of bounds error
			return 1;
		}
		(*lvals)[i] = lvals_tmp[idx];
	}
	osc_atom_array_u_free(idxs);
	return 0;
}

int osc_expr_specFunc_nth(t_osc_expr_ast_funcall *f, 
			     t_osc_expr_lexenv *lexenv, 
			     t_osc_bndl_u *oscbndl,
			     t_osc_atom_ar_u **out)
{
	// can't just call osc_expr_specFunc_lval_nth() because this may be a list literal or something
	// that's not part of the bundle
	t_osc_expr_ast_expr *vals_expr = osc_expr_ast_funcall_getArgs(f);
	t_osc_expr_ast_expr *idxs_expr = osc_expr_ast_expr_next(vals_expr);
	t_osc_atom_ar_u *vals = NULL;
	osc_expr_ast_expr_evalInLexEnv(vals_expr, lexenv, oscbndl, &vals);
	if(!vals){
		// error 
		return 1;
	}
	t_osc_atom_ar_u *idxs = NULL;
	osc_expr_ast_expr_evalInLexEnv(idxs_expr, lexenv, oscbndl, &idxs);
	if(!idxs){
		// error 
		return 1;
	}
	int nvals = osc_atom_array_u_getLen(vals);
	int n = osc_atom_array_u_getLen(idxs);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		int idx = osc_atom_u_getInt(osc_atom_array_u_get(idxs, i));
		if(idx >= nvals){
			// index out of bounds error
			return 1;
		}
		t_osc_atom_u *a = osc_atom_array_u_get(*out, i);
		osc_atom_u_copyValue(a, osc_atom_array_u_get(vals, idx));
	}
	osc_atom_array_u_free(vals);
	osc_atom_array_u_free(idxs);
	return 0;
}



// these are dummy functions---they're used to do a pointer comparison in osc_expr_ast_funcall_evalInLexEnv
OSC_EXPR_BUILTIN_DECL(apply){return 0;}
OSC_EXPR_BUILTIN_DECL(map){return 0;}
OSC_EXPR_BUILTIN_DECL(lreduce){return 0;}
OSC_EXPR_BUILTIN_DECL(rreduce){return 0;}
OSC_EXPR_BUILTIN_DECL(assign){return 0;}
OSC_EXPR_BUILTIN_DECL(if){return 0;}
OSC_EXPR_BUILTIN_DECL(emptybundle){return 0;}
OSC_EXPR_BUILTIN_DECL(bound){return 0;}
OSC_EXPR_BUILTIN_DECL(exists){return 0;}
OSC_EXPR_BUILTIN_DECL(getaddresses){return 0;}
OSC_EXPR_BUILTIN_DECL(delete){return 0;}
OSC_EXPR_BUILTIN_DECL(getmsgcount){return 0;}
OSC_EXPR_BUILTIN_DECL(value){return 0;}
OSC_EXPR_BUILTIN_DECL(quote){return 0;}
OSC_EXPR_BUILTIN_DECL(eval){return 0;}
OSC_EXPR_BUILTIN_DECL(tokenize){return 0;}
OSC_EXPR_BUILTIN_DECL(gettimetag){return 0;}
OSC_EXPR_BUILTIN_DECL(settimetag){return 0;}
OSC_EXPR_BUILTIN_DECL(lookup){return 0;}
OSC_EXPR_BUILTIN_LVAL_DECL(lookup){return 0;}
OSC_EXPR_BUILTIN_DECL(nth){return 0;}
OSC_EXPR_BUILTIN_LVAL_DECL(nth){return 0;}

/*
// special functions: bypass scalar expansion and type promotion and have access to the bundle
assign
nth
bound
exists
emptybundle
getaddresses
delete
getmsgcount
eval
apply
map
lreduce
rreduce
quote
lambda
gettimetag
settimetag
lookup
if

// typed builtins: builtins subject to scalar expansion and type promotion

// Num a => a -> a
math.h...

// Num a => (a, a) -> a
add
sub
mul
div
lt
le
gt
ge
eq
neq
and
or
mod
pow
min
max

//// (a, a, a) -> [a]
aseq

//// bool -> bool
not

//// [] -> int
length

//// [] -> []
reverse
sort
list

//// a -> [char]
typetags

//// [a] -> [a] -> a
dot

//// [a, a, a] -> [a, a, a] -> [a, a, a]
cross

//// [a] -> a
l2norm

//// string -> int
strcmp

//// string -> [string]
split
tokenize

//// a -> b
cast
float32
float64
int8
int16
int32
int64
uint8
uint16
uint32
uint64
string

// user defined: functions implemented in the expression language
add1: add(1)
sub1: sub(1)
product: lreduce(mul)
sum: lreduce(add)
cumsum: 
mean: lambda(l){sum(l) / length(l);}
median: 
nfill: lambda(n, v){map(lambda(x){v}, [0:n-1])}
range: lambda(l){max(l) - min(l)}
interleave: map(list)
first: nth(, 0)
last: lambda(l){nth(l, length(l) - 1)}
rest: lambda(l){nth(l, [1:length(l) - 1])}
butlast: lambda(l){nth(l, [0:length(l) - 2])}
extrema: 
clip:
scale:
mtof:
ftom:
sign:
identity: lambda(x){x}

// constants
pi
twopi
halfpi
quarterpi
oneoverpi
twooverpi
degtorad
radtodeg
e
lntwo
lnten
logtwoe
logtene
sqrttwo
sqrthalf

// deprecated/removed
assign_to_index
join
prog1
prog2
progn
value
 */