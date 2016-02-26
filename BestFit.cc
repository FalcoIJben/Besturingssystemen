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

void  BestFit::setSize(int new_size)
{
	require(areas.empty());					// prevent changing the size when the freelist is nonempty
	Fitter::setSize(new_size);
	areas.push_back(new Area(0, new_size));	// and create the first free area (i.e. "all")
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
    if (ap) {					        // success ?
        return ap;
    }

    return 0;


}
///inserts area into areas
void BestFit::free(Area *ap)
{
    //voeg op juiste plek toe
    for(ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
        Area  *area = *i;
        if(ap->getSize() < area->getSize()) {
            areas.insert(i, ap);
            return;
            //i = areas.end();
        }
    }
    areas.push_back(ap);
}


Area *BestFit::searcher(int wanted)
{
    require(wanted > 0);		// has to be "something",
	require(wanted <= size);	// but not more than can exist,
	require(!areas.empty());	// provided we do have something to give

    for(ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
        Area  *area = *i;
        if(wanted  <= area->getSize()) {
            if(wanted != area->getSize()) {
                if(area->getSize() > wanted) {		    // Larger than needed ?
                    Area  *rp = area->split(wanted);    // Split into two parts (updating sizes)
                    free(rp);	                        // Insert remainder into area
                }
            }
            areas.erase(i);
            return area;
        }
    }
    //geen area groot genoeg
    return 0;
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
///????
void	BestFit::updateStats()
{
    ++qcnt;									// number of 'alloc's
    qsum  += areas.size();					// length of resource map
    qsum2 += (areas.size() * areas.size());	// same: squared
}
