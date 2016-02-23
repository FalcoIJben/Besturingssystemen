#pragma once
#ifndef	__Area_h__
#define	__Area_h__	4.1
//#ident	"@(#)Area.h	4.1	AKK	20150124"

/** @file Area.h
 *  @brief The memory area descriptor.
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 4.1	2015/01/24
 */

#include <iostream>		// for: std::ostream
#include "asserts.h"	// for: require()

/// @class Area
/// Beschrijving van een geheugen gebied.
/// De eenheid is eigenlijk arbitrair, bv. bytes of woorden.
/// Omdat het puur om een administratie over het geheugen gaat,
/// kunnen we hier gewoon 'int' gebruiken i.p.v. 'void *' o.i.d.
class	Area
{
	/// An output operator to print an area description
	friend std::ostream  &operator<<(std::ostream &os, const Area &a) {
		return os << "Area(" << a.base << "..." << a.ends << ':' << a.size << ')';
	}

private:

	int	 base;	// het start "adres"
	int	 size;	// de omvang van het gebied
	int	 ends;	// het laatste "adres" binnen het gebied

public:

	/// Maak een area.
	/// @param	base	start adres
	/// @param	size	omvang van het gebied
	Area(int base, int size);

	// Maak de attribuutwaardes beschikbaar
	int		getBase() const { return base; }	///< Vertel het begin adres
	int		getSize() const { return size; }	///< Vertel de omvang
	int		getLast() const { return ends; }	///< Vertel het laatste adres binnen het gebied

	/// Overlappen deze twee area's elkaar?
	/// @param	xp	Area waarmee vergeleken wordt
	bool	overlaps(const Area *xp) const;

	/// Maak het gebied kleiner en maak van de rest een nieuwe area.
	/// @param	size	Reduceer dit gebied tot deze omvang
	/// @returns		Een nieuwe area voor de rest
	Area  *split(int size);

	/// Voeg dit gebied samen met het gegeven gebied
	/// en ruim daarna de gegeven descriptor op.
	/// @param	xp	De area die we samenvoegen met deze area
	/// @note	Het object waar xp naar verwijst wordt gedelete!
	void   join(Area *xp);


	// ====== !! Nu komt wat C++ magie !! ======

	// Dit zijn "classes" die zich gedragen als functies!
	// In c++ jargon een 'functor'
	// Zie:
	// http://www.parashift.com/c%2B%2B-faq-lite/pointers-to-members.html
	// en vooral:
	// http://www.parashift.com/c%2B%2B-faq-lite/functor-vs-functionoid.html
	//
	// Kijk bijvoorbeeld in FirstFit::reclaim() om te zien
	// hoe ze gebruikt kunnen worden.

	/// @class orderByAddress
	/// Sorting functor for Area's by address
	class orderByAddress
	{
	public:
		inline 	/// "Call" operator to compare two areas by address
		bool operator()(const Area *ap, const Area *bp) {
			require((ap != 0) && (bp != 0));
			return ap->base < bp->base;
		}
	};

	/// @class orderBySizeAscending
	/// Sorting functor for Area's by size ascending
	class orderBySizeAscending
	{
	public:
		inline	/// "Call" operator to compare two areas by size ascending
		bool operator()(const Area *ap, const Area *bp) {
			require((ap != 0) && (bp != 0));
			return ap->size < bp->size;
		}
	};

	/// @class orderBySizeDescending
	/// Sorting functor for Area's by size descending
	class orderBySizeDescending
	{
	public:
		inline	/// "Call" operator to compare two areas by size descending
		bool operator()(const Area *ap, const Area *bp) {
			require((ap != 0) && (bp != 0));
			return ap->size >= bp->size;
		}
	};

};


// vim:sw=4:ai:aw:ts=4:
#endif	/*Area_h*/
