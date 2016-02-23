//#ident	"@(#)Application.cc	4.1	AKK	20150124"
/** @file Application.cc
 * De implementatie van Application.
 */

// Unix/Linux includes
#include <cstdlib>			// srand(3), rand(3), exit(2)

// Our own includes
#include "main.h"			// common global stuff
#include "Stopwatch.h"		// De cpu tijd meter
#include "Application.h"	// De pseudo applicatie

// introduce std shorthands
using std::cout;
using std::cerr;
using std::endl;
using std::flush;

// ===================================================================

/// Een, globale, hulp functie die de kans berekent
/// dat de applicatie geheugenruimte wil aanvragen.
inline // deze 'inline' is alleen maar wat extra optimalisatie
bool	vraagkans(int r)
{
	return (((r >> 5) % 16) < 7);
	// De "vraag" kans is 7 op 16.
	// N.B. Bij veel 'random' generatoren zijn de laatste
	// bitjes niet even random als de rest. Daarom kijken
	// we hier naar een paar bitjes "halverwege" de
	// gegeven 'r' en niet naar de "laatste" bitjes.
	//		>> = schuif bitjes naar rechts
	//		%  = modulo = rest na deling
}

// Utility:
// Returns a random integer in the range [min,max)
// i.e. from min (inclusive) upto max (exclusive)
inline
int randint(int min, int max)
{
	int  m = (max - min);	// bepaal bereik
	int  r = rand();
	r %= m;					// rest na deling
	return r + min;
}

// ===================================================================


// Initieer een "Application" die geheugen vraagt aan
// de gegeven beheerder, waarbij we de beschikking
// hebben over totaal 'size' eenheden geheugen.
Application::Application(Allocator *beheerder, int size)
	: beheerder(beheerder), size(size)
	, vflag(false), tflag(true)
	, err_teller(0), oom_teller(0)
{
	// nooit iets geloven ...
	require(beheerder != 0);
	require(size > 0);
}


// actie: na afloop alles netjes opruimen.
Application::~Application()
{
	vflag = false; // svp het opruimen in stilte doen
	// zijn er nog objecten over?
	while (!objecten.empty())
		vergeetOudste();
}


// actie: vraag om geheugen (onze versie van 'new')
void	Application::vraagGeheugen(int omvang)
{
	if (vflag)
		cout << "Vraag " << omvang << ", " << flush;

	// Deze interne controle overslaan als we aan het testen zijn.
	if (!tflag)
		require((0 < omvang) && (omvang <= size));	// is de 'omvang' wel geldig ?

	// Vraag de beheerder om geheugen
	Area  *ap = beheerder->alloc(omvang);

	// Allocator reported out of memory?
	if (ap == 0) {
		if (vflag)
			cout << AC_RED "out of memory" AA_RESET << endl;
		++oom_teller;	// out-of-memory teller bijwerken
		return;
	}

	if (vflag)	// vertel welk gebied we kregen
		cout << "kreeg " << (*ap) << endl;

	// Nu moeten we eerst controlen of er geen overlap
	// bestaat met gebieden die we al eerder hadden gekregen ...
	for (Area* xp : objecten)
	{
		if (ap->overlaps(xp)) {     // Oeps!
			// Dit zou eigenlijk een "fatal error" moeten zijn,
			// maar bij de RandomFit zal dit wel vaker gebeuren
			// dus alleen maar melden dat het fout is ...
			if (vflag) {
				cout << AC_RED "Oeps, het nieuwe gebied overlapt met " << (*xp) << AA_RESET << endl;
			}
			++err_teller;	// fouten teller bijwerken
			break;			// verder zoeken is niet meer zo zinvol ...
							// ... en levert alleen maar meer uitvoer op.
		}
	}

	// Het gekregen gebied moeten we natuurlijk wel onthouden.
	objecten.push_back(ap);
}


// actie: geef een gekregen gebied weer terug (onze versie van 'delete')
void	Application::vergeetOudste()
{
	require(! objecten.empty());	// hebben we eigenlijk wel wat ?
	Area  *ap = objecten.front();	// het oudste gebied opzoeken
	if (vflag)						// vertel wat we gaan doen
		cout << "Vrijgeven " << (*ap) << endl;
	objecten.pop_front();			// gebied uit de lijst halen
	beheerder->free(ap);			// en vrij geven
}

// actie: geef een gekregen gebied weer terug (onze versie van 'delete')
void	Application::vergeetRandom()
{
	require(! objecten.empty());	// hebben we eigenlijk wel wat ?

	Area  *ap = objecten.front();	// het oudste gebied alvast opzoeken

	int  n = objecten.size();		// valt er wat te kiezen?
	if (n > 1) {
		int  m = randint(0, n);		// kies een index
		// en zoek dat element op
		ALiterator  i;
		for (i = objecten.begin() ; (m > 0) && (i != objecten.end()) ; ++i, --m) {
			;
		}
		ap = *i;					// het slachtoffer
		objecten.erase(i);			// uit de lijst halen
	} else {
		objecten.pop_front();		// het oudste gebied uit de lijst halen
	}

	if (vflag) {
		// vertel wat we gaan doen
		cout << "Vrijgeven " << (*ap) << endl;
	}

	beheerder->free(ap);			// en het gebied weer vrij geven
}


// =========== DE BEHEER MODULE TESTEN ==========

// Elke test die hieronder uitgevoerd wordt zou een 'assert' failure
// moeten veroorzaken die de normale executie-volgorde afbreekt.
void	Application::testing()
{
	Area  *ap = 0;
	Area  *bp = 0;

	tflag = true;		// Zet de sanity-check in 'vraagGeheugen' even uit.

	err_teller = 0;		// Reset the Error teller.
	oom_teller = 0;		// Reset the Out-Of-Memory teller.

	int	 failed_steps = 0;
	int  fase = 0;		// Houdt bij hoe ver we zijn in de test procedure.

	// Na elke 'test' komen we weer "hier" terug voor de volgende test.
	while (fase >= 0) {
		try {
			cerr << "Stap " << (++fase) << ":\n";
			switch (fase) {
				case 1:
					cerr << "Om niets vragen ...\n";
					vraagGeheugen(0);   		// dit mag niet kunnen ...
					// ... maar de controle heeft gefaald
					cerr << AC_RED "TEST FAILED" AA_RESET "\n";
					++failed_steps;
					cerr << "Stap " << (++fase) << ":\n";
					/*FALLTHRU*/				// ga door naar de volgende test
				case 2:
					cerr << "Te veel vragen ...\n";
					vraagGeheugen(size + 1);	// dit mag niet kunnen ...
					// ... maar de controle heeft gefaald
					cerr << AC_RED "TEST FAILED" AA_RESET "\n";
					++failed_steps;
					cerr << "Stap " << (++fase) << ":\n";
					/*FALLTHRU*/				// ga door naar de volgende test
				case 3:
					cerr << "Te weinig vragen ...\n";
					vraagGeheugen(-1);
					cerr << AC_RED "TEST FAILED" AA_RESET "\n";
					++failed_steps;
					cerr << "Stap " << (++fase) << ":\n";
					/*FALLTHRU*/
				case 4:
					cerr << "Om alles vragen ...\n";
					++failed_steps;
					vraagGeheugen(size);   			// PAS OP: dit mag dus wel!
					// Zou geen problemen mogen geven
					--failed_steps;
					cerr << AC_GREEN "OKE, TEST SUCCEEDED" AA_RESET "\n";
					cerr << "Stap " << (++fase) << ":\n";
					/*FALLTHRU*/
				case 5:
					cerr << "Ruimte weer vrijgeven ...\n";
					++failed_steps;
					vergeetOudste(); 				// geheugen van stap 4 weer vrijgeven
					// Zou geen problemen mogen geven
					--failed_steps;
					cerr << AC_GREEN "OKE, TEST SUCCEEDED" AA_RESET "\n";
					cerr << "Stap " << (++fase) << ":\n";
					/*FALLTHRU*/
				case 6:
					cerr << "Een gebied twee keer vrijgeven ...\n";
					// Vraag om geheugen
					ap = beheerder->alloc(size / 2);	// dit moet altijd kunnen
					bp = new Area(*ap);					// dupliceer ap
					beheerder->free(ap);				// het origineel vrij geven
					beheerder->free(bp);				// en de kopie vrij geven
						// NB This test may cause a memory leak if
						// 'beheerder->free' does not delete 'bp'
					cerr << AC_RED "TEST FAILED" AA_RESET "\n";
					++failed_steps;
					//cerr << "Stap " << ( ++fase ) << ":\n";
					/*FALLTHRU*/
					// Voeg zonodig nog andere testcases toe
				default:
					if (failed_steps > 0)
						cerr << AC_GREEN "Einde code testen " AC_RED "with "
								<< failed_steps << " errors" AA_RESET "\n";
					else
						cerr << AC_GREEN "Einde code testen, alles OKE" AA_RESET "\n";
					fase = -1;		// einde test loop
					tflag = false;	// Puur "voor het geval dat"
					break;
			}
		} catch (const std::logic_error& error) {
			cerr << error.what() << endl;
			cerr << AC_GREEN "OKE, TEST SUCCEEDED" AA_RESET "\n";
		}
	}//while fase
}


// =========== DE BEHEER MODULE METEN =============

// Deze methode doet "zomaar" wat onzin acties die niet echt
// overeenkomen met het echte gedrag van een programma.
// Voeg straks zelf een andere methode toe die wel wat zinnigers doet.
void	Application::randomscenario(int aantal, bool vflag)
{
	bool old_vflag = this->vflag;
	this->vflag = vflag;	// verbose mode aan/uit

	oom_teller = 0;			// reset failure counter
	err_teller = 0;			// reset error counter

	// Door srand hier aan te roepen met een "seed" waarde
	// krijg je altijd een herhaling van hetzelfde scenario.
	// Je kan elke seed waarde dan zien als de code voor "een scenario".
	// Handig voor het testen/meten, maar bedenk wel dat deze scenario's
	// nooit gelijkwaardig zijn aan het gedrag van een echt systeem.
	srand(1);   // (zie: man 3 rand)

	// Nu komt het eigenlijke werk:
	Stopwatch  klok;		// Een stopwatch om de tijd te meten
	klok.start();			// -----------------------------------
	for (int  x = 0 ; x < aantal ; ++x) {	// Doe nu tig-keer "iets".
		int  r = rand();					// Gooi de dobbelsteen
		if (objecten.empty()				// Als we nog niets hebben of
		  || vraagkans(r) )					// we kiezen voor ruimte aanvragen
		{
			r = rand();						// Gooi de dobbelsteen nog eens
			r %= (size / 100);				// maximaal 1% van alles
			vraagGeheugen(r + 1);			// maar minstens 1 eenheid
		} else								// Anders: vrijgeven mits ...
		if (!objecten.empty()) {			// ... we iets hebben
			vergeetRandom();				// geef een gebied weer terug
		}
		// else
			// dan doen we een keer niets
	}
	klok.stop();			// -----------------------------------
	klok.report();			// Vertel de gemeten processor tijd
	beheerder->report();	// en de statistieken van de geheugenbeheer zelf

	// Evaluatie
	if ((oom_teller > 0) || (err_teller > 0) ) {	// some errors occured
	    cout << AC_RED "De allocater faalde " << oom_teller << " keer";
	    cout << " en maakte " << err_teller << " fouten\n" AA_RESET;
	} else {										// no problems
	    cout << AC_GREEN "De allocater faalde " << oom_teller << " keer";
	    cout << " en maakte " << err_teller << " fouten\n" AA_RESET;
	}

	this->vflag = old_vflag; // turn on verbose output again
}


// TODO:
// Schrijf je eigen scenario routine die zich meer gedraagt als een
// echte applicatie. En vergeet niet Application.h ook aan te passen.


// vim:sw=4:ai:aw:ts=4:
