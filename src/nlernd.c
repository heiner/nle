#include "nlernd.h"
#include "hack.h"
#include "isaac64.h"

/* See rng.c. */
struct rnglist_t {
    int FDECL((*fn), (int) );
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

unsigned long nle_seeds[] = { 0L, 0L, 0L };

/* State of the level generation RNG */
static struct isaac64_ctx nle_lgen_state;

/* State of the core RNG */
static struct isaac64_ctx nle_core_state;

/* Some flags to help  manage the lgen seed */
static bool lgen_initialised = false;
static bool lgen_active = false;

/* Seeding function to initialise the fixed-level rng state.
   Borrowed from init_isaac64 in NetHack's rnd.c */
void
nle_init_lgen_state(unsigned long seed)
{
    unsigned char new_rng_state[sizeof seed];
    unsigned i;

    for (i = 0; i < sizeof seed; i++) {
        new_rng_state[i] = (unsigned char) (seed & 0xFF);
        seed >>= 8;
    }

    isaac64_init(&nle_lgen_state, new_rng_state, (int) sizeof seed);
}

void
nle_init_lgen_rng()
{
    if (settings.initial_seeds.use_lgen_seed) {
        nle_init_lgen_state(settings.initial_seeds.lgen_seed);
        lgen_initialised = true;
    } else {
        lgen_initialised = false;
    }
    /* Even if we didn't use it, stash the seed */
    nle_seeds[2] = settings.initial_seeds.lgen_seed;
}

void
nle_swap_to_lgen(void)
{
    if (lgen_initialised && !lgen_active) {
        int core_rng = whichrng(rn2);

        /* stash the current core state */
        nle_core_state = rnglist[core_rng].rng_state;

        /* copy the current lgen state */
        rnglist[core_rng].rng_state = nle_lgen_state;

        /* since we want nle_swap_to_lgen and swap_to_core to be
           called in the correct sequence we ignore subsequent
           calls to this function. */
        lgen_active = true;
    }
}

void
nle_swap_to_core(void)
{
    if (lgen_initialised && lgen_active) {
        int core_rng = whichrng(rn2);

        /* stash the current lgen state */
        nle_lgen_state = rnglist[core_rng].rng_state;

        /* restore the core state */
        rnglist[core_rng].rng_state = nle_core_state;

        /* since we want nle_swap_to_lgen and swap_to_core to be
           called in the correct sequence we ignore subsequent
           calls to this function. */
        lgen_active = false;
    }
}

void
nle_set_seed(nle_ctx_t *nle, unsigned long core, unsigned long disp,
             boolean reseed, unsigned long lgen)
{
    /* Keep up to date with rnglist[] in rnd.c. */
    set_random(core, rn2);
    set_random(disp, rn2_on_display_rng);

    /* Determines logic in reseed_random() in hacklib.c. */
    has_strong_rngseed = reseed;

    nle_init_lgen_state(lgen);
    lgen_initialised = true;
    nle_seeds[2] = lgen;
};

void
nle_get_seed(nle_ctx_t *nle, unsigned long *core, unsigned long *disp,
             boolean *reseed, unsigned long *lgen, bool *lgen_in_use)
{
    *core = nle_seeds[0];
    *disp = nle_seeds[1];
    *reseed = has_strong_rngseed;
    *lgen = nle_seeds[2];
    *lgen_in_use = lgen_initialised;
}