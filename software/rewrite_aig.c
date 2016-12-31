#include "aiger.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define USAGE \
"usage: aigrewrite [-h][-v][<input>]\n" \
"\n" \
"Rewrite an And Inverter Graph (AIGER format).\n"\
"Give an aag file as input"

static int verbose;
static aiger * src, *dst;

static void die (const char *fmt, ...) {
  va_list ap;
  fputs ("*** [aigrewrite] ", stderr);
  va_start (ap, fmt);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  fputc ('\n', stderr);
  fflush (stderr);
  exit (1);
}

static void msg (const char *fmt, ...) {
  va_list ap;
  if (!verbose)
    return;
  fputs ("[aigrewrite] ", stderr);
  va_start (ap, fmt);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  fputc ('\n', stderr);
  fflush (stderr);
}


int main (int argc, char ** argv) {
  const char * input, * output, * err;
  aiger_and * a;
  aiger_and * b;
  aiger_and * c;
  aiger_and * a_copy;
  aiger_and * b_copy;
  aiger_and * c_copy;
  aiger_and * e;
  int repeated_literal;
  int non_repeated_literal1;
  int non_repeated_literal2;
  int non_rep_lit_on_comp_block;
  int non_rep_lit_on_non_comp_block;
  int rep_output_literal1;
  int rep_output_literal2;
  int r,s;
  int k;
  unsigned j,out,tmp;
  int i, ok;
  char * srcname, *dstname;
  int found=0;

  input = output = 0;

  for (i = 1; i < argc; i++) {
    if (!strcmp (argv[i], "-h")) { printf ("%s", USAGE); exit (0); }
    else if (argv[i][0] == '-')
      die ("invalid command line option '%s'", argv[i]);
    else if (output) die ("too many arguments");
    else if (input) output = argv[i];
    else input = argv[i];
  }

  src = aiger_init ();
  if (input) {
    msg ("reading '%s'", input);
    err = aiger_open_and_read_from_file (src, input);
  } else {
    msg ("reading '<stdin>'");
    err = aiger_read_from_file (src, stdin);
  }

  if (err) die ("read error: %s", err);

  msg ("read MILOA %u %u %u %u %u", 
    src->maxvar,
    src->num_inputs,
    src->num_latches,
    src->num_outputs,
    src->num_ands);

  dst = aiger_init();
  		for (i=0; i<src->num_inputs; i++){
			aiger_add_input(dst,src->inputs[i].lit,src->inputs[i].name);
		}
		for (i = 0; i < src->num_latches; i++) {
    			aiger_add_latch (dst, src->latches[i].lit, 
                        src->latches[i].next,
                        src->latches[i].name);
    			aiger_add_reset (dst, src->latches[i].lit, src->latches[i].reset);
  		}
		
		
/*Code for AIG rewriting*/
    for (i= src->num_ands-1; i>=0; i--){
	    a = src->ands + i;
	    //left child
	    for (r=0; r<src->num_ands;r++){
		 b = src->ands + r;
		 if(aiger_lit2var(a->rhs0)==aiger_lit2var(b->lhs))
		 	break;
		 else{
			b = NULL;
		 }
	    }
	    //right child
	    for (s=0; s<src->num_ands;s++){
		 c = src->ands + s;
		 if(aiger_lit2var(a->rhs1)==aiger_lit2var(c->lhs))
		 	break;
		 else{
			c = NULL;
		 }
	    } 
	    if((b != NULL) && (c != NULL)){
		    if((((b->rhs0 == c->rhs0) && (repeated_literal = b->rhs0) && (non_repeated_literal1 = b->rhs1) && (non_repeated_literal2 = c->rhs1) && (rep_output_literal1 = b->lhs) && (rep_output_literal2 = c->lhs)) || ((b->rhs1 == c->rhs1) && (repeated_literal = b->rhs1) && (non_repeated_literal1 = c->rhs0) && (non_repeated_literal2 = c->rhs0) && (rep_output_literal1 = b->lhs) && (rep_output_literal2 = c->lhs)) || ((b->rhs0 == c->rhs1) && (repeated_literal = b->rhs0) && (non_repeated_literal1 = b->rhs1) && (non_repeated_literal2 = c->rhs0) && (rep_output_literal1 = b->lhs) && (rep_output_literal2 = c->lhs)) || ((b->rhs1 == c->rhs0) && (repeated_literal = b->rhs1) && (non_repeated_literal1 = b->rhs0) && (non_repeated_literal2 = c->rhs1) && (rep_output_literal1 = b->lhs) && (rep_output_literal2 = c->lhs))) && ((a->rhs0==b->lhs) && (a->rhs1==c->lhs)))
		    {   
			found=1;
			a_copy=src->ands+i;
			b_copy=src->ands+r;
			c_copy=src->ands+s;
			aiger_add_and(dst,rep_output_literal1,non_repeated_literal1,non_repeated_literal2);
			tmp++;
			aiger_add_and(dst,rep_output_literal2,repeated_literal,rep_output_literal1);
			tmp++;
	            }
                   //logic for structure 2 ( ab~c)
		    else if((((b->rhs0 == c->rhs0) && (repeated_literal = b->rhs0) && (non_repeated_literal1 = b->rhs1) && (non_repeated_literal2 = c->rhs1) && (rep_output_literal1 = b->lhs) && (rep_output_literal2 = c->lhs)) || ((b->rhs1 == c->rhs1) && (repeated_literal = b->rhs1) && (non_repeated_literal1 = c->rhs0) && (non_repeated_literal2 = c->rhs0) && (rep_output_literal1 = b->lhs) && (rep_output_literal2 = c->lhs)) || ((b->rhs0 == c->rhs1) && (repeated_literal = b->rhs0) && (non_repeated_literal1 = b->rhs1) && (non_repeated_literal2 = c->rhs0) && (rep_output_literal1 = b->lhs) && (rep_output_literal2 = c->lhs)) || ((b->rhs1 == c->rhs0) && (repeated_literal = b->rhs1) && (non_repeated_literal1 = b->rhs0) && (non_repeated_literal2 = c->rhs1) && (rep_output_literal1 = b->lhs) && (rep_output_literal2 = c->lhs))) && (!(a->rhs0==b->lhs+1) != !(a->rhs1==c->lhs+1)))
		    {
	   	      found = 2;
		      if(a->rhs0==b->lhs+1){
			if(repeated_literal == b->rhs0){
				non_rep_lit_on_comp_block = b->rhs1;
			}
			else if(repeated_literal == b->rhs1){
				non_rep_lit_on_comp_block = b->rhs0;
			}
			if(repeated_literal == c->rhs0){
				non_rep_lit_on_non_comp_block = c->rhs1;
			}
			else if(repeated_literal == c->rhs1){
				non_rep_lit_on_non_comp_block = c->rhs0;
			}
		      }
		      else if (a->rhs1 == c->lhs+1){
			if(repeated_literal == c->rhs0){
				non_rep_lit_on_comp_block = c->rhs1;
			}
			else if(repeated_literal == c->rhs1){
				non_rep_lit_on_comp_block = c->rhs0;			
			}
			if(repeated_literal == b->rhs0){
				non_rep_lit_on_non_comp_block = b->rhs1;
			}
			else if(repeated_literal == b->rhs1){
				non_rep_lit_on_non_comp_block = b->rhs0;
			}	
		      }
		      a_copy=src->ands+i;
		      b_copy=src->ands+r;
		      c_copy=src->ands+s;
		     aiger_add_and(dst,rep_output_literal1,non_rep_lit_on_non_comp_block,repeated_literal);
			tmp++;
		     aiger_add_and(dst,rep_output_literal2,non_rep_lit_on_comp_block+1,rep_output_literal1);
			tmp++;	
		    }
	    }
    }
    if(found ==1){
		//adding additional gates
    	for (r=0; r<src->num_ands; r++){
				e=src->ands+r;			
				if (!((e==a_copy) || (e==b_copy) || (e==c_copy))){
					aiger_add_and(dst,e->lhs-2,e->rhs0-2,e->rhs1);
				}
    	}
		//adding outputs
		//one output case
		out = e->lhs;
		srcname = src->outputs[0].name;
		aiger_add_output (dst, out-2, srcname);
    }
	
    if(found ==2){
		//adding additional gates
    	for (r=0; r<src->num_ands; r++){
				e=src->ands+r;			
				if (!((e==a_copy) || (e==b_copy) || (e==c_copy))){
					aiger_add_and(dst,e->lhs-2,e->rhs0-2,e->rhs1);
				}
    	}
		//adding outputs
		//one output case
		out = e->lhs;
		srcname = src->outputs[0].name;
		aiger_add_output (dst, out-2, srcname);
    }
    //if no structure found output same AIG
    if(found==0){
	//adding and gates
	for (i=0; i<src->num_ands; i++){
			a = src->ands + i;
			aiger_add_and(dst,a->lhs,a->rhs0,a->rhs1);
	}
	for (i=0; i<src->num_outputs; i++)
    	{
		      out = src->outputs[i].lit;
		      srcname = src->outputs[i].name;
		      aiger_add_output (dst, out, srcname);
    	}
    }
    

  if (output) {
    msg ("writing '%s'", output);
    ok = aiger_open_and_write_to_file (dst, output);
  } else {
    msg ("writing '<stdout>'", output);
    ok = aiger_write_to_file (dst, 
           (isatty (1) ? aiger_ascii_mode : aiger_binary_mode), stdout);
  }
  if (!ok) die ("write error");

  aiger_reset (src);
  aiger_reset (dst);

  return 0;
}
