/*

Set of functions to manipulate NetHack's Random Number Generators

*/

#ifndef NLERND_H
#define NLERND_H

#include "nletypes.h"

void nle_init_lgen_state();
void nle_swap_to_lgen(void);
void nle_swap_to_core(void);

void nle_set_seed(nle_ctx_t *, unsigned long, unsigned long, boolean);
void nle_get_seed(nle_ctx_t *, unsigned long *, unsigned long *, boolean *);

#endif