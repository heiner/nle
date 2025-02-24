/*

Set of functions to manipulate NetHack's Random Number Generators

*/

#ifndef NLERND_H
#define NLERND_H

#include "nletypes.h"

void nle_set_seed(nle_ctx_t *, unsigned long, unsigned long, boolean);
void nle_get_seed(nle_ctx_t *, unsigned long *, unsigned long *, boolean *);

#endif