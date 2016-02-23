#pragma once
#ifndef	__FirstFit2_h__
#define	__FirstFit2_h__
//#ident	"@(#)FirstFit2.h	2.1	AKK	20090208"

/** @file FirstFit2.h
 *  @brief The class that implements the eager version of the FirstFit algorithm.
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 2.1	2009/02/08
 */

#include "FirstFit.h"


/// @class FirstFit2
/// Het FirstFit algorithme gebruikt het eerste
/// gevonden bruikbare gebied in de resource map.
/// Dit is de eager versie (probeert meteen alles op
/// te ruimen en houdt daarom de gebieden gesorteerd
/// op adres)
class	FirstFit2 : public FirstFit
{
public:

	explicit	// see: http://en.cppreference.com/w/cpp/language/explicit
	/// @param cflag	initial status of check-mode
	/// @param type		name of this algorithm (default=FirstFit2)
	FirstFit2(bool cflag=false, const char *type = "FirstFit (eager)")
		: FirstFit(cflag, type) {}

	/// The application returns an area to freespace
	/// @param ap	The area returned to free space
	virtual  void	 free(Area *ap);

};

#endif	/*FirstFit2_h*/
// vim:sw=4:ai:aw:ts=4:
