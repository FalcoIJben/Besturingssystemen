//#ident	"@(#)FirstFit.cc	2.2	AKK	20140130"
/** @file FirstFit.cc
 * De implementatie van FirstFit.
 */

#include "FirstFit.h"
#include "ansi.h"


// Clean up dead stuff
FirstFit::~FirstFit()
{
	while (!areas.empty()) {
		Area  *ap = areas.back();
		areas.pop_back();
		delete ap;
	}
}

// Initializes how much memory we own
void  FirstFit::setSize(int new_size)
{
	require(areas.empty());					// prevent changing the size when the freelist is nonempty
	Fitter::setSize(new_size);
	areas.push_back(new Area(0, new_size));	// and create the first free area (i.e. "all")
}

// Print the current freelist for debugging
void	FirstFit::dump()
{
	std::cerr << AC_BLUE << type << "::areas";
	for (ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
		std::cerr << ' ' << **i;
	}
	std::cerr << AA_RESET << std::endl;
}


// Application wants 'wanted' memory
Area  *FirstFit::alloc(int wanted)
{
	require(wanted > 0);		// has to be "something",
	require(wanted <= size);	// but not more than can exist

	updateStats();				// update resource map statistics

	if(areas.empty()) {		// iff we have nothing
		return 0;    			// give up immediately
	}

	// Search thru all available free areas
	Area  *ap = 0;
	ap = searcher(wanted);		// first attempt
	if(ap) {					// success ?
		return ap;
	}
	if(reclaim()) {			// could we reclaim fragmented freespace ?
		ap = searcher(wanted);	// then make a second attempt
		if(ap) {				// success ?
			return ap;
		}
	}
	// Alas, failed to allocate anything
	//dump();//DEBUG
	return 0;					// inform caller we failed
}


// Application returns an area no longer needed
void	FirstFit::free(Area *ap)
{
	require(ap != 0);
	if (cflag) {
		// EXPENSIVE: check for overlap with all registered free areas
		for(ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
			check(!ap->overlaps(*i));    // the sanity check
		}
	}
	areas.push_back(ap);	// add discarded "old" object to the end of free list
}


// ----- internal utilities -----

// Search for an area with at least 'wanted' memory
Area  *FirstFit::searcher(int wanted)
{
	require(wanted > 0);		// has to be "something",
	require(wanted <= size);	// but not more than can exist,
	require(!areas.empty());	// provided we do have something to give

	// Search thru all available areas
	for(ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
		Area  *ap = *i;					// Candidate item
		if(ap->getSize() >= wanted) {	// Large enough?
			// Yes, use this area;
			// The 'erase' operation below invalidates the 'i' iterator
			// but it does return a valid iterator to the next element.
			ALiterator  next = areas.erase(i);	// Remove this element from the freelist
			if(ap->getSize() > wanted) {		// Larger than needed ?
				Area  *rp = ap->split(wanted);	// Split into two parts (updating sizes)
				areas.insert(next, rp);			// Insert remainder before "next" area
			}
			return  ap;
		}
	}
	return  0; // report failure
}


// We have run out of usefull areas;
// Try to reclaim space by joining fragmented freespace
bool	FirstFit::reclaim()
{
	require(!areas.empty());		// sanity check

	// Sort resource map by area address
	areas.sort(Area::orderByAddress());	// WARNING: expensive N*log(N) operation !

	// Search thru all free areas for matches between successive elements
	bool  changed = false;
	ALiterator  i = areas.begin();
	Area  *ap = *i;					// The current candidate ...
	for(++i ; i != areas.end() ;) {
		Area  *bp = *i;				// ... match it with.
		if(bp->getBase() == (ap->getBase() + ap->getSize())) {
			// Oke; bp matches ap ... [i.e. bp follows ap]
			ALiterator  next = areas.erase(i);	// remove bp from the list
			ap->join(bp);			// append area bp to ap (and destroy bp)
			++mergers;				// update statistics
			changed = true;			// we changed something
			i = next;				// revive the 'i' iterator
			// and now try match ap with whatever followed bp
		} else {
			ap = bp;				// move on to next free area
			++i;
		}
	}
	++reclaims;	// update statistics ("reclaims attempted")
	return changed;
}

// Update statistics
void	FirstFit::updateStats()
{
	++qcnt;									// number of 'alloc's
	qsum  += areas.size();					// length of resource map
	qsum2 += (areas.size() * areas.size());	// same: squared
}

// vim:sw=4:ai:aw:ts=4:
