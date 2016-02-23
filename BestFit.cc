#include "BestFit.h"
#include "ansi.h"

BestFit::~BestFit()
{
    while (!areas.empty()) {
		Area  *ap = areas.back();
		areas.pop_back();
		delete ap;
	}
}

Area *BestFit::alloc(int wanted)
{
    require(wanted > 0);		// minstens "iets",
    require(wanted <= size);	// maar niet meer dan we kunnen hebben.


    updateStats();
    if(areas.empty()){
        return 0;
    }
    // Search thru all available free areas
    Area  *ap = searcher(wanted);		// first attempt
    if (ap) {					// success ?
        return ap;
    }

    return 0;


}

void BestFit::free(Area *ap)
{
    areas.push_back(ap);	// de lazy version

}

Area *BestFit::searcher(int wanted)
{
    require(wanted > 0);		// has to be "something",
	require(wanted <= size);	// but not more than can exist,
	require(!areas.empty());	// provided we do have something to give

    //the area that fits the best
    Area*  bestArea = 0;
    ALiterator  next = areas.end();

    for(ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
        Area  *area = *i;
        if(area->getSize() >= wanted){
            if(area->getSize() == wanted){
                //area is ideal, so return
                areas.erase(i);
                return area;
            }

            if(bestArea == 0 ||bestArea->getSize() > area->getSize()){
                //this area is a better area
                bestArea = area;
                next = i;
            }
        }
    }
    if(bestArea != 0) {
        next = areas.erase(next);
        if(bestArea->getSize() > wanted) {		    // Larger than needed ?
            Area  *rp = bestArea->split(wanted);	// Split into two parts (updating sizes)
            areas.insert(next, rp);			        // Insert remainder before "next" area
        }
    }

    return  bestArea;
}

// Try to join fragmented freespace
bool	BestFit::reclaim()
{
    bool  changed = false;	// did we change anything ?

    // Sort resource map by area address
    areas.sort( Area::orderByAddress() );	// WARNING: expensive N*log(N) operation !

    // Search thru all available areas for matches
    ALiterator  i = areas.begin();
    Area  *ap = *i;				// the current candidate ...
    for (++i ; i != areas.end() ;) {
        Area  *bp = *i;			// ... match ap with ...
        if (bp->getBase() == (ap->getBase() + ap->getSize()))
        {
            // oke, bp matches ap ...
            ALiterator  next = areas.erase(i);	// remove bp from the list
            ap->join(bp);			// append area bp to ap (and destroy bp)
            ++mergers;				// update statistics
            changed = true;			// yes we changed something
            i = next;				// revive the 'i' iterator
        } else {
            ap = bp;				// move on to next area
            ++i;
        }
    }

    ++reclaims;						// update statistics
    return changed;
}


// Update statistics
void	BestFit::updateStats()
{
    ++qcnt;									// number of 'alloc's
    qsum  += areas.size();					// length of resource map
    qsum2 += (areas.size() * areas.size());	// same: squared
}