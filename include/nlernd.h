/*

Set of functions to manipulate NetHack's Random Number Generators

*/

#ifndef NLERND_H
#define NLERND_H

#include "nletypes.h"

void init_lgen_state(unsigned long);
void nle_swap_to_lgen(void);
void nle_swap_to_core(void);

void nle_set_seed(nle_ctx_t *, unsigned long, unsigned long, boolean);
void nle_get_seed(nle_ctx_t *, unsigned long *, unsigned long *, boolean *);

#endif