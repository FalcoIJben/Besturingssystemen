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




}

bool BestFit::reclaim()
{


}

