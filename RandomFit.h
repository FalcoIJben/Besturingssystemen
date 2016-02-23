#pragma once
#ifndef	__RandomFit_h__
#define	__RandomFit_h__	2.1
//#ident	"@(#)RandomFit.h	2.1	AKK	20090208"

/** @file RandomFit.h
 *  @brief The class that implements a dummy RandomFit algorithm.
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 2.1	2009/02/08
 */

#include "Allocator.h"


/// @class RandomFit
/// De Random Fit allocator geeft een nep-resultaat terug.
/// Het enige dat klopt is dat het toegewezen gebied binnen
/// de grenzen van het "geheugen" valt en de juiste omvang
/// heeft. De rest is puur "dobbelsteen".
class	RandomFit : public Allocator
{
public:

	explicit	// see: http://en.cppreference.com/w/cpp/language/explicit
	/// @param cflag	initial status of check-mode
	/// @param type		name of this algorithm (default=RandomFit)
	RandomFit(bool cflag=false, const char *type = "RandomFit")
		: Allocator(cflag, type) {}

	// Geheugen omvang instellen is hier niet nodig
	//		void	setSize( int new_size );
	// RandomFit gebruikt de default implementatie van Allocator.

	/// Ask for an area of at least 'wanted' units
	/// @returns	An area or 0 if not enough freespace available
	Area	*alloc(int wanted);	// gebied vragen

	/// The application returns an area to freespace
	/// @param ap	The area returned to free space
	void	 free(Area *ap);	// gebied teruggeven

	void	report();			///< report statistics (dummy)
};

// vim:sw=4:ai:aw:ts=4:
#endif	/*RandomFit_h*/
