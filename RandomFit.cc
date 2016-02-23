//#ident	"@(#)RandomFit.cc	2.1	AKK	20090208"
/** @file RandomFit.cc
 * De implementatie van RandomFit.
 */

#include "main.h"
#include "RandomFit.h"

#include <cstdlib>		// voor: rand(3)


// Iemand vraagt om 'wanted' geheugen
Area	*RandomFit::alloc(int wanted)
{
	require(wanted > 0);		// minstens "iets",
	require(wanted <= size);	// maar niet meer dan we kunnen hebben.
	int  base = 0;
	if (wanted < size) {		// valt er wat te "gokken" ?
		base = rand();				// gooi dan de dobbelsteen
		base %= (size - wanted);	// maar blijf binnen de grenzen...
	}
	// else er valt niets te gokken.

	return new Area(base, wanted);
	// Pas op:
	// We garanderen niet dat dit gebied echt vrij is
	// het is en blijft een "gokje" algoritme  8-)
}


// iemand levert een gebied weer in
void	RandomFit::free(Area *ap)
{
	// Omdat RandomFit helemaal niets doet aan boekhouding
	// kunnen we hier volstaan met ...
	delete  ap;
}

void	RandomFit::report()
{
	std::cout << type << ": 0 reclaims with 0 mergers\n";
}

// vim:sw=4:ai:aw:ts=4:
