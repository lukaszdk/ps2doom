// TBD : ver se é preciso mais alguma coisa

#include "z_zone.h"
#include "i_system.h"

static struct
{
  void *cache;
#ifdef TIMEDIAG
  int locktic;
#endif
  int locks;
} *cachelump;


/*
 * W_LockLumpNum
 *
 * This copies the lump into a malloced memory region and returns its address
 * instead of returning a pointer into the memory mapped area
 *
 */
const void* W_LockLumpNum(int lump)
{
  size_t len = W_LumpLength(lump);
  //printf(" (len = W_LumpLength(lump)) : lump = %d, len = %d\n", lump, len);    // cosmito : debug
  const void *data = W_CacheLumpNum(lump);
  //printf("data : %p\n\n", data);    // cosmito : debug
  if (!cachelump[lump].cache) {             // o cache nao esta a ser alocado. no prboom faz-se assim
 /*
 prboom :

D_DoomMainSetup/W_Init/W_InitCache

void W_InitCache(void)
{
  // set up caching
  cachelump = calloc(numlumps, sizeof *cachelump);
  if (!cachelump)
    I_Error ("W_Init: Couldn't allocate lumpcache");

etc
*/

    // read the lump in
    Z_Malloc(len, PU_CACHE, &cachelump[lump].cache);
    memcpy(cachelump[lump].cache, data, len);
  }

  /* cph - if wasn't locked but now is, tell z_zone to hold it */
  if (cachelump[lump].locks <= 0) {
    Z_ChangeTag(cachelump[lump].cache,PU_STATIC);
#ifdef TIMEDIAG
    cachelump[lump].locktic = gametic;
#endif
    // reset lock counter
    cachelump[lump].locks = 1;
  } else {
    // increment lock counter
    cachelump[lump].locks += 1;
  }

#ifdef SIMPLECHECKS
  if (!((cachelump[lump].locks+1) & 0xf))
    lprintf(LO_DEBUG, "W_CacheLumpNum: High lock on %8s (%d)\n",
      lumpinfo[lump].name, cachelump[lump].locks);
#endif

  return cachelump[lump].cache;
}


void W_UnlockLumpNum(int lump)
{
	if (cachelump[lump].locks == -1)
		return; // this lump is memory mapped

#ifdef SIMPLECHECKS
	if (cachelump[lump].locks == 0)
		fprintf(stderr, "W_UnlockLumpNum: Excess unlocks on %8s\n",
		lumpinfo[lump].name);
#endif
	cachelump[lump].locks -= 1;
	/* cph - Note: must only tell z_zone to make purgeable if currently locked,
	* else it might already have been purged
	*/
	if (cachelump[lump].locks == 0)
		Z_ChangeTag(cachelump[lump].cache, PU_CACHE);
}
