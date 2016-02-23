#pragma once
#ifndef	__NextFit2_h__
#define	__NextFit2_h__	2.1
//#ident	"@(#)NextFit2.h	2.1	AKK	20090208"

/** @file NextFit2.h
 *  @brief The class that implements the eager version of the NextFit algorithm.
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 2.1	2009/02/08
 */

#include "NextFit.h"

/// @class NextFit2
/// Het NextFit algorithme gebruikt het eerste
/// gevonden bruikbare gebied in de resource map.
/// Dit is de eager versie.
class	NextFit2 : public NextFit
{
public:

	explicit	// see: http://en.cppreference.com/w/cpp/language/explicit
	/// @param cflag	initial status of check-mode
	/// @param type		name of this algorithm (default=NextFit2)
	NextFit2(bool cflag=false, const char *type = "NextFit (eager)")
		: NextFit(cflag, type) {}

	/// The application returns an area to freespace
	/// @param ap	The area returned to free space
	void	 free(Area *ap);	// application returns space
};

#endif	/*NextFit2_h*/
// vim:sw=4:ai:aw:ts=4:
