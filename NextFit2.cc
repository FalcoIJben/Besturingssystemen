//#ident	"@(#)NextFit2.cc	2.1	AKK	20090208"
/** @file NextFit2.cc
 * De implementatie van NextFit2.
 */

#include "NextFit2.h"


// Iemand levert een gebied weer in
void	NextFit2::free(Area *ap)
{
	require(ap != 0);

	// Find the right place to insert ap
	ALiterator  next = areas.end();
	int  merged = 0;				// Counter: we merged 'ap' with some existing areas
	for (ALiterator  i = areas.begin() ; i != areas.end() ;) {
		Area  *bp = *i;					// match new ap with existing bp ...
		if (cflag)
			check(!ap->overlaps(bp));    // sanity check
		// Does older area bp match new free area ap?
		if ( (bp->getBase() == (ap->getBase() + ap->getSize()))		// ap before bp ?
		  || (ap->getBase() == (bp->getBase() + bp->getSize())) )	// bp before ap ?
		{
			// Yes, found a match.
			// Remove bp from the list and prevent that our 'cursor' ends up in limbo.
			if (i == cursor) {				// NB 'erase' would invalidate 'cursor'
				cursor = next = areas.erase(i);
			} else {
				next = areas.erase(i);
			}
			// join the two area's
			if (ap->getBase() < bp->getBase()) {
				ap->join(bp);    			// append area bp to ap (and destroy bp)
			} else {
				bp->join(ap);				// append area ap to bp (and destroy ap)
				ap = bp;					// and use bp as the new ap
			}
			++mergers;						// update statistics
			++merged;						// count how many "ends" we merged
			if (merged == 2) {				// did merge both ends
				areas.insert(next, ap);		// then insert ap before next
				return;						// and leave
			}
			// now try to match with the other end of 'ap'
			i = next;						// must revive the 'i' iterator here
		} else {
			++i;							// move on to next area in the freelist
		}
	}
	// Found no further matches
	areas.insert(next, ap);
}

// vim:sw=4:ai:aw:ts=4:
