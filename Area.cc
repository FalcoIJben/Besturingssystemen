//#ident	"@(#)Area.cc	3.1	AKK	20131214"
/** @file Area.cc
 * De implementatie van Area.
 */

// Eigen includes
#include "asserts.h"	// for: require()
#include "Area.h"		// class Area


// Maak een Area ...
Area::Area(int base, int size)
	: base(base), size(size)
{
	require(base >= 0);
	require(size > 0);
	// ... en bereken meteen het laatste adres binnen dit gebied
	ends = base + size - 1;
}


// Overlappen de twee gebieden elkaar ?
bool	Area::overlaps(const Area *xp) const
{
	require(xp != 0);
	return  (  ((xp->base <= base) && (base <= xp->ends))		// 'this' begint  IN xp !
	        || ((xp->base <= ends) && (ends <= xp->ends)) );	// 'this' eindigt IN xp !
}


// Splits dit gebied op in een stuk ter grote van 'gevraagd'
// en maak een nieuwe Area voor de rest.
Area	*Area::split(int gevraagd)
{
	require(gevraagd > 0);		// sanity check
	require(gevraagd < size);	// er moet wel iets overblijven

	Area  *rp = new Area(base + gevraagd, size - gevraagd);
	size = gevraagd;			// pas je eigen omvang aan
	ends = base + size - 1;		// must update this too!
	return  rp;
}

// Plak gebied 'xp' achter dit gebied
// en gooi daarna de 'xp' descriptor weg
void	Area::join(Area *xp)
{
	require(xp != 0);					// sanity check
	require(xp->base == (base + size)); // xp moet op dit gebied aansluiten
	size += xp->size;					// dit gebied wordt groter
	ends = base + size - 1;				// must update this too!
	delete  xp;							// deze descriptor kan nu weg.
}

// vim:sw=4:ai:aw:ts=4:
