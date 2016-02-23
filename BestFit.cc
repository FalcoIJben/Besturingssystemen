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


}

void BestFit::free(Area *ap)
{


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

            if(bestArea->getSize() > area->getSize() || bestArea == 0){
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

bool BestFit::reclaim()
{


}

