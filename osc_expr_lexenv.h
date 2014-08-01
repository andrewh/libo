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

/** 	\file osc_expr_lexenv.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_LEXENV_H__
#define __OSC_EXPR_LEXENV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_hashtab.h"
typedef t_osc_hashtab t_osc_expr_lexenv;
#include "osc_expr_ast_expr.h"

void osc_expr_lexenv_dtor(char *key, void *val);
t_osc_expr_lexenv *osc_expr_lexenv_alloc(void);
void osc_expr_lexenv_free(t_osc_expr_lexenv *lexenv);
void osc_expr_lexenv_printlexenv_cb(char *key, void *val, void *context);
void osc_expr_lexenv_copy(t_osc_expr_lexenv **dest, t_osc_expr_lexenv *src);
void osc_expr_lexenv_deepCopy(t_osc_expr_lexenv **dest, t_osc_expr_lexenv *src);
void osc_expr_lexenv_bind(t_osc_expr_lexenv *lexenv, char *varname, t_osc_expr_ast_expr *val);
t_osc_expr_ast_expr *osc_expr_lexenv_lookup(t_osc_expr_lexenv *lexenv, char *varname);

#ifdef __cplusplus
}
#endif

#endif