//#ident	"@(#)Fitter.cc	2.2	AKK	20140130"
/** @file Fitter.cc
 * De implementatie van de Fitter baseclass.
 */

#include <cmath>		// for: sqrt(3) [needs -lm]
#include "ansi.h"		// ansi color codes

#include "Fitter.h"


Fitter::Fitter(bool cflag, const char *type)
	: Allocator(cflag, type)
	, reclaims(0), mergers(0)
	, qcnt(0), qsum(0), qsum2(0)
{
}


// Initializes how much memory we own
void  Fitter::setSize(int new_size)
{
	require(new_size > 0);					// must be a meaningfull value
	Allocator::setSize(new_size);			// inform the Allocator baseclass about the new size
	reclaims = mergers = 0;					// clear the statistics
	qcnt = qsum = qsum2 = 0;				// and these too
}


// Report statistics
void	Fitter::report()
{
	std::cout << type << ": " << reclaims << " reclaims, " << mergers << " mergers\n";

	require(qcnt > 0);			// prevent divide-thru-zero
	double	avg = qsum / qcnt;	// calculate the average resource map length

	require(qcnt > 1);			// prevent divide-thru-zero
	double	stdev				// calculate the standard deviation
		= sqrt(
			  (qsum2 / (qcnt - 1))
			  -
			  (qcnt / (qcnt - 1)) * (((qsum / qcnt) * (qsum / qcnt)))
		  );
	// also see: http://en.wikipedia.org/wiki/Standard_deviation
	std::cout << type << ": average " << avg << " areas, stdev " << stdev << " areas\n";
	// Assuming a normal distribution, then:
	// 68% of the time the map length will be avg +/- stdev		[one-sigma]
	// 95% of the time the map length will be avg +/- (2*stdev)	[two-sigma]
	// 99.7% of the time it will be within 3-sigma
}


// vim:sw=4:ai:aw:ts=4:
