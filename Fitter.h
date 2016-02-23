#pragma once
#ifndef	__Fitter_h__
#define	__Fitter_h__	4.1
//#ident	"@(#)Fitter.h	4.1	AKK	20150124"

/** @file Fitter.h
 *  @brief The baseclass for all "fit" algorithms
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 4.1	2015/01/24
 */

// c++ headers
#include <list>			// de STL std::list<> container
// project headers
#include "Allocator.h"

// shorthands
typedef	std::list<Area*>	AreaList;		///< Een "Arealist container"
typedef	AreaList::iterator	ALiterator;		///< Een "AreaList container Iterator"



/// @class Fitter
/// A Fitter baseclass for various implementations
/// of the members of the "fit" algorithm family.
class	Fitter : public Allocator
{

public:

	/// @param cflag	initial status of check-mode
	/// @param type		the name of the algorithm
	Fitter(bool cflag, const char *type);

	void	 setSize(int new_size);	///< initialize memory size

	void	 report();				///< report statistics

protected:

	/// This is the actual function that searches for free space
	virtual	 Area 	*searcher(int) = 0;		// A "pure-virtual function"

	/// This function is called when the searcher can not find space.
	/// It tries to reclaim fragmented space by merging adjacent free areas.
	/// @returns true if adjacent areas could be merged
	virtual	 bool	  reclaim() = 0;		// A "pure-virtual function"


	// Counters to maintain some simple statistics
	long		reclaims;	///< how often we have tried to reclaim fragmented space
	long		mergers;	///< how often we could merge fragmented space

	// Counters to calculate the average size of the resource map
	long long	qcnt;	///< number of allocs tried
	long long	qsum;	///< sum of areas.size()
	long long	qsum2;	///< sum of areas.size() squared

};


// vim:sw=4:ai:aw:ts=4:
#endif	/*Fitter_h*/
