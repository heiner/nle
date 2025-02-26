#include "nlernd.h"
#include "hack.h"
#include "isaac64.h"

/* See rng.c. */
struct rnglist_t {
    int FDECL((*fn), (int));
    boolean init;
    isaac64_ctx rng_state;
};
extern struct rnglist_t rnglist[];
extern int FDECL(whichrng, (int FDECL((*fn), (int) )));

/* See hacklib.c. */
extern int FDECL(set_random, (unsigned long, int FDECL((*fn), (int) )));

/* An appropriate version of this must always be provided in
   port-specific code somewhere. It returns a number suitable
   as seed for the random number generator */
extern unsigned long NDECL(sys_random_seed);

/* NLE settings contains the initial RNG seeds */
extern nle_settings settings;

/*
 * Initializes the random number generator.
 * Originally in hacklib.c.
 */
void
init_random(int FDECL((*fn), (int) ))
{
    if (settings.initial_seeds.use_init_seeds) {
        set_random(settings.initial_seeds.seeds[whichrng(fn)], fn);
        has_strong_rngseed = settings.initial_seeds.reseed;
    } else {
        set_random(sys_random_seed(), fn);
    }
}

static struct isaac64_ctx nle_lgen_state; /* State of the level generation RNG */
static struct isaac64_ctx nle_core_state; /* State of the core RNG */
static bool lgen_initialised = false;

/* Seeding function to initialise the fixed-level rng.
   Borrowed from init_isaac64 in NetHack's rnd.c */
void nle_init_lgen_state()
{
    if(settings.initial_seeds.use_lgen_seed) {

        unsigned long seed = settings.initial_seeds.lgen_seed;

        unsigned char new_rng_state[sizeof seed];
        unsigned i;

        for (i = 0; i < sizeof seed; i++) {
            new_rng_state[i] = (unsigned char) (seed & 0xFF);
            seed >>= 8;
        }

        isaac64_init(&nle_lgen_state, new_rng_state,
                    (int) sizeof seed);

        lgen_initialised = true;
    } else {
        lgen_initialised = false; 
    }
}

void nle_swap_to_lgen(void)
{
    if(lgen_initialised) {
        int core_rng = whichrng(rn2);

        /* stash the current core state */
        nle_core_state = rnglist[core_rng].rng_state;
    
        /* copy the current lgen state */
        rnglist[core_rng].rng_state = nle_lgen_state;        
    }
}

void nle_swap_to_core(void)
{
    if(lgen_initialised) {
        int core_rng = whichrng(rn2);

        /* stash the current lgen state */
        nle_lgen_state = rnglist[core_rng].rng_state;

        /* restore the core state */
        rnglist[core_rng].rng_state = nle_core_state;
    }
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