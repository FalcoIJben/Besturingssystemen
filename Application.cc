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
#include "BestFit.h"	// De pseudo applicatie

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

///returnt nu de grootte die vergeten is
// actie: geef een gekregen gebied weer terug (onze versie van 'delete')
int	Application::vergeetOudste()
{
    int grootte = 0;
	require(! objecten.empty());	// hebben we eigenlijk wel wat ?
	Area  *ap = objecten.front();	// het oudste gebied opzoeken
	if (vflag)						// vertel wat we gaan doen
		cout << "Vrijgeven " << (*ap) << endl;
	objecten.pop_front();			// gebied uit de lijst halen
    grootte = ap->getSize();
	beheerder->free(ap);			// en vrij geven
	return grootte;
}
///returnt nu de grootte die vergeten is
// actie: geef een gekregen gebied weer terug (onze versie van 'delete')
int	Application::vergeetRandom()
{
	require(! objecten.empty());	// hebben we eigenlijk wel wat ?
    int grootte = 0;
	Area  *ap = objecten.front();	// het oudste gebied alvast opzoeken

	int  n = objecten.size();		// valt er wat te kiezen?
	if (n > 1) {
		int  m = randint(0, n);		// kies een index
		// en zoek dat element op
		ALiterator  i;
		for (i = objecten.begin() ; (m > 0) && (i != objecten.end()) ; ++i, --m) {
			;
		}
		ap = *i;	                // het slachtoffer
		objecten.erase(i);			// uit de lijst halen
	} else {
		objecten.pop_front();		// het oudste gebied uit de lijst halen
	}

	if (vflag) {
		// vertel wat we gaan doen
		cout << "Vrijgeven " << (*ap) << endl;
	}

    grootte = ap->getSize();
	beheerder->free(ap);			// en het gebied weer vrij geven
	return grootte;
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

///onze eigen vraagkans
bool	Application::berekendeVraagkans(int r, int extrakans )
{
    return (((r >> 5) % 18) < (12+extrakans) );
    //bijna het zelfde als de normale vraagkans,
    //maar hierbij kan de kans varieren van 12 v/d 18 keer tot 17 v/d 18 keer

}

int     Application::berekendeGrootte(bool veelGroteObjecten, bool veelKleineObjecten)
{
    //de maximale grootte avn een klein object is 0,5% van alles
    int halfMaxSize = size/200;

    //een groot object heeft een vaste grootte
    int grootObjectGrootte = halfMaxSize + size/350;

    if(veelGroteObjecten == veelKleineObjecten) {
        //geen voorkeur over hoe groot de objecten zijn

        if(!randint(0,4)) { //dit kan omdat 0 false is, en de rest true
            //groot object
            return grootObjectGrootte;
        } else {
            //klein object
            return randint(3, halfMaxSize );
        }
    } else if(veelGroteObjecten) {
        //veel grote objecten
        if(!randint(0,2)) {
            //klein object
            return randint(3, halfMaxSize );
        } else {
            //groot object
            return grootObjectGrootte;
        }
    } else {
        //veel kleine objecten
        if(!randint(0,20)) {
            //groot object
            return grootObjectGrootte;
        } else {
            //klein object
            return randint(3, halfMaxSize );
        }
    }
}
///return of er ebject is verwijderd
bool    Application::vergeetOudsteKleinObject()
{
    //vergeet het oudste bericht

    require(! objecten.empty());
	Area  *ap = objecten.front();

	//na 100 keer te hebben gezocht naar het oudste bericht, doet die niks.
	int maalProberen = 100;

	ALiterator  i;
    for (i = objecten.begin() ; (maalProberen > 0) && (i != objecten.end()) ; ++i, --maalProberen) {
        ap = *i;

        if(ap->getSize() < size/200) {    //als het object kleiner is da de maximale grootte van een klein object, is het een klein object
            if (vflag)						// vertel wat we doen
                cout << "Vrijgeven " << (*ap) << endl;
            objecten.erase(i);
            beheerder->free(ap);
            return true;
        }
    }
    return false;
}
///return of er ebject is verwijderd
bool    Application::vergeetRandomGrootObject()
{
    //vergeet random gebruiker, omdat deze bijvoorbeeld disconnect
    //als het na tien keer proberen nie tlukt om ene groot object te vinden kapt die ermee
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
		int maalProberen = 100;
		for (i = objecten.begin() ; (maalProberen > 0) && (i != objecten.end()) ; ++i, --m) {
            ap = *i;
            if(ap->getSize() > size/200) { //als het object groter is da de maximale grootte van een klein object, is het een klein object
                if (vflag)						// vertel wat we doen
                    cout << "Vrijgeven " << (*ap) << endl;
                objecten.erase(i);
                beheerder->free(ap);
            return true;
            }
		}
	} else {
        if(ap->getSize() > size/200) {
            objecten.pop_front();		// het oudste gebied uit de lijst halen
            return true;
        }

	}
	return false;

}



void    Application::chatroomscenario(int aantal, bool vflag, bool veelGroteObjecten, bool veelKleineObjecten)
{
    int av = 0;
    int ab = 0;

    int aantalVerbindingen = 0; // het aantal verbindingen moet minstens 1 zijn, anders kunnen er geen berichten worden verzonden
    int aantalBerichten = 0;
    int verbindingGrootte = size/200 + size/350;

    bool old_vflag = this->vflag;
	this->vflag = vflag;	// verbose mode aan/uit

	oom_teller = 0;			// reset failure counter
	err_teller = 0;			// reset error counter

	int extrakans = 2 * veelGroteObjecten + 3 * veelKleineObjecten; //kan gewoon :D

    srand(1); ///seed

    Stopwatch  klok;		// Een stopwatch om de tijd te meten
	klok.start();			// -----------------------------------
	for (int  i = 0 ; i < aantal ; ++i) {
        if(!aantalVerbindingen) { //als het aantal vebindingen 0 is
            vraagGeheugen(verbindingGrootte);
            aantalVerbindingen++;
            av++;
            //cout << "Nieuwe verbinding" << endl;
        } else {
            int  r = rand();

            if (objecten.empty() || berekendeVraagkans(r, extrakans) ) {    // Gooi de gemanipuleerde dobbelsteen
                int grootte = berekendeGrootte(veelGroteObjecten, veelKleineObjecten);
                if(grootte == verbindingGrootte) {
                    //een verbinding
                    av++;
                    //cout << "Nieuwe verbinding" << endl;
                    if(aantalVerbindingen < 70) {                             //maximaal 70 verbindingen, daarna kan niemand meer joinen
                        aantalVerbindingen++;
                        vraagGeheugen(grootte);
                    }
                } else {
                    //een bericht
                    aantalBerichten++;
                    ab++;
                    //cout << "Nieuw bericht" << endl;
                    if(aantalBerichten > 100) {                             //maximaal 100 berichten geladen op de server, daarna worden ze weg gegooid
                        vergeetOudsteKleinObject();
                        aantalBerichten--;
                    }
                    vraagGeheugen(grootte);
                }
            } else if(!objecten.empty()){                                   // hoeft eigenlijk niet deze check, maar we doen het voor de zekerheid, net zoals in random scenario
                int ri = randint(1,4);
                switch(ri) {
                    case 1:
                        if(vergeetOudste() == verbindingGrootte) {  //vergeet de een bericht of verbinding
                            aantalVerbindingen--;
                        } else {
                            aantalBerichten--;
                        }
                        break;
                    case 2:
                        if(vergeetRandom() == verbindingGrootte) {  //vergeet een bericht of verbinding. een bericht kan bijv. verwijdert zijn door gebruiker
                            aantalVerbindingen--;
                        } else {
                            aantalBerichten--;
                        }
                        break;
                    case 3:
                        if(vergeetRandomGrootObject()) { //vergeet verbinding
                            aantalVerbindingen--;
                        }
                        break;
                    default:
                        if(vergeetOudsteKleinObject()) { //vergeet bericht
                            aantalBerichten--;
                        }
                        break;
                }


            }
            //else -> we doen lekker niks (dit hoort niet voor te komen)
        }

	}
	klok.stop();			// -----------------------------------
	klok.report();			// Vertel de gemeten processor tijd
	cout << endl;
	beheerder->report();	// en de statistieken van de geheugenbeheer zelf

	//bereken factoren


	cout << "aantal berichten           = " << ab << endl;
	cout << "aantal verbindingen        = " << av << endl << endl;

	cout << "Factor B" << endl;
    Fitter* f = dynamic_cast<Fitter*>(beheerder);
    if(f != 0) {
        f->reportTooSmallSpacesCount();            /// een ruimte is te klein, als deze kleiner dan 3 is
    }

    cout << "Factor C" << endl;
    cout << "aantal objecten in gebruik = " << objecten.size() << endl;
    cout << "totaal aantal ruimtes      = " << beheerder->getSize() << endl;


}





// vim:sw=4:ai:aw:ts=4:
