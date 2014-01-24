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

#include <string.h>
#include <stdio.h>
#include "osc_expr_oprec.r"
#include "osc_expr_oprec.h"

char *osc_expr_oprec_getName(t_osc_expr_oprec *r)
{
	if(r){
		return r->name;
	}
	return NULL;
}

int osc_expr_oprec_getInputArity(t_osc_expr_oprec *r)
{
	if(r){
		return r->input_arity;
	}
	return -1;
}

char **osc_expr_oprec_getParamNames(t_osc_expr_oprec *r)
{
	if(r){
		return r->param_names;
	}
	return NULL;
}

int *osc_expr_oprec_getParamTypes(t_osc_expr_oprec *r)
{
	if(r){
		return r->param_types;
	}
	return NULL;
}

int osc_expr_oprec_getOutputArity(t_osc_expr_oprec *r)
{
	if(r){
		return r->output_arity;
	}
	return -1;
}

char **osc_expr_oprec_getOutputNames(t_osc_expr_oprec *r)
{
	if(r){
		return r->output_names;
	}
	return NULL;
}

int *osc_expr_oprec_getOutputTypes(t_osc_expr_oprec *r)
{
	if(r){
		return r->output_types;
	}
	return NULL;
}

char *osc_expr_oprec_getDocstring(t_osc_expr_oprec *r)
{
	if(r){
		return r->docstring;
	}
	return NULL;
}

int osc_expr_oprec_getAssociativity(t_osc_expr_oprec *r)
{
	if(r){
		return r->associativity;
	}
	return -1;
}

int osc_expr_oprec_getPrecedence(t_osc_expr_oprec *r)
{
	if(r){
		return r->precedence;
	}
	return -1;
}

int osc_expr_oprec_getFixity(t_osc_expr_oprec *r)
{
	if(r){
		return r->fixity;
	}
	return -1;
}

char osc_expr_oprec_getBytecode(t_osc_expr_oprec *r)
{
	if(r){
		return r->bytecode;
	}
	return -1;
}
