#include "nlernd.h"
#include "hack.h"

/* See rng.c. */
extern int FDECL(whichrng, (int FDECL((*fn), (int) )));

/* See hacklib.c. */
extern int FDECL(set_random, (unsigned long, int FDECL((*fn), (int) )));

/* An appropriate version of this must always be provided in
   port-specific code somewhere. It returns a number suitable
   as seed for the random number generator */
extern unsigned long NDECL(sys_random_seed);

nle_seeds_init_t *nle_seeds_init;

/*
 * Initializes the random number generator.
 * Originally in hacklib.c.
 */
void
init_random(int FDECL((*fn), (int) ))
{
#ifdef NLE_ALLOW_SEEDING
    if (nle_seeds_init) {
        set_random(nle_seeds_init->seeds[whichrng(fn)], fn);
        has_strong_rngseed = nle_seeds_init->reseed;
        return;
    }
#endif
    set_random(sys_random_seed(), fn);
}

void
nle_set_seed(nle_ctx_t *nle, unsigned long core, unsigned long disp,
             boolean reseed)
{
    /* Keep up to date with rnglist[] in rnd.c. */
    set_random(core, rn2);
    set_random(disp, rn2_on_display_rng);

    /* Determines logic in reseed_random() in hacklib.c. */
    has_strong_rngseed = reseed;
};

extern unsigned long nle_seeds[];

void
nle_get_seed(nle_ctx_t *nle, unsigned long *core, unsigned long *disp,
             boolean *reseed)
{
    *core = nle_seeds[0];
    *disp = nle_seeds[1];
    *reseed = has_strong_rngseed;
}