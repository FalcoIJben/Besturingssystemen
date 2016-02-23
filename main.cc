//#ident	"@(#)main.cc	4.1	AKK	20150124"
/** @file main.cc
 * The main program.
 *
 * It reads the options given and then, in sucession,
 * runs various memory administration algoritms.
 */

// The Unix/Linux includes
#include <vector>	// std::vector<T>
#include <cstdlib>	// exit(2), atexit(3), atol(3), EXIT_SUCCESS, EXIT_FAILURE
#include <getopt.h>	// int getopt(3) en char *optarg
// Zie ook manuals: exit(3), atol(3) en getopt(3)

// En onze eigen includes
#include "main.h"	// includes several other includes
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

// ===================================================================

// Informatie over de diverse geheugenbeheer algoritmes:
#include "RandomFit.h"	// de RandomFit allocator
#include "FirstFit.h"	// de FirstFit allocator (lazy version)
#include "FirstFit2.h"	// de FirstFit2 allocator (eager version)
#include "NextFit.h"	// de NextFit allocator (lazy version)
#include "NextFit2.h"	// de NextFit2 allocator (eager version)
// TODO:
// .... voeg hier je eigen variant(en) toe ....
// bijvoorbeeld:
//#include "BestFit.h"		// pas de naam aan aan jouw versie
//#include "BestFit2.h"		// pas de naam aan aan jouw versie
//#include "WorstFit.h"		// pas de naam aan aan jouw versie
//#include "WorstFit2.h"		// pas de naam aan aan jouw versie
//#include "PowerOfTwo.h"	// pas de naam aan aan jouw versie
//#include "McKusickK.h"	// pas de naam aan aan jouw versie
//#include "Buddy.h"		// pas de naam aan aan jouw versie
//enz


// ===================================================================
// Het hoofdprogramma.
// ===================================================================
// Handelt de meegegeven opties af, kies geheugen beheerders, enz
// en dan gaan we wat spelen.

// Globale hulpvariabelen voor 'main' en 'doOptions'

// wat gaan we doen
int			  size = 10240;			///< de omvang van het beheerde geheugen
bool		  cflag = false;		///< laat de allocator foute 'free' acties detecteren
									///< (voor sommige algorithmes is dit duur)

// operationele informatie
vector<Allocator*>	beheerders;		///< de gekozen beheerders
int			  aantal = 10000;		///< hoe vaak doen we iets met dat geheugen
bool		  tflag = false;		///< 'true' als we de code willen "testen"
									///< anders wordt er "gemeten".
bool		  vflag = false;		///< vertel wat er allemaal gebeurt


/// Vertel welke opties dit programma kent
void	tellOptions(const char *progname)
{
	cout << "Usage: " << progname << " "
	     AS_UNDERLINE "options" AA_RESET ", valid options are:" << endl;

	// Algemeen
	cout << "\t-s size\t\tsize of memory being administrated\n";
	cout << "\t-a count\tnumber of actions (current=" << aantal << ")\n";
	cout << "\t-t\t\ttoggle test mode (current=" << (tflag ? "on" : "off") << ")\n";
	cout << "\t-v\t\ttoggle verbose mode (current=" << (vflag ? "on" : "off") << ")\n";
	cout << "\t-c\t\ttoggle check mode (current=" << (cflag ? "on" : "off") << ")\n";

	// De fitter groep
	cout << "\t-r\t\tuse the random allocator\n";
	cout << "\t-f\t\tuse the first fit allocator (lazy)\n";
	cout << "\t-F\t\tuse the first fit allocator (eager)\n";
	cout << "\t-n\t\tuse the next fit allocator (lazy)\n";
	cout << "\t-N\t\tuse the next fit allocator (eager)\n";
	// TODO:
	// Fitter groep
	//cout << "\t-b\t\tuse the best fit allocator (lazy)\n";
	//cout << "\t-B\t\tuse the best fit allocator (eager)\n";
	//cout << "\t-w\t\tuse the worst fit allocator (lazy)\n";
	//cout << "\t-W\t\tuse the worst fit allocator (eager)\n";
	// TODO:
	// De power-of-2 groep
	//cout << "\t-p\t\tuse power of 2 allocator\n";
	//cout << "\t-m\t\tuse mckusick/karols allocator\n";
	//cout << "\t-2\t\tuse buddy algorithm\n";
}



/// Analyseert de opties die de gebruiker mee gaf.
/// Kan/zal diverse globale variabelen veranderen !
void	doOptions(int argc, char *argv[])
{
	// TODO:
	char  options[] = "s:a:tvcrfFnN"; // De opties die we willen herkennen
	// Als je algoritmes toevoegt dan moet je de string hierboven uitbreiden.
	// (Vergeet niet om de tellOptions functie hiervoor ook aan te passen)
	// Als je alle algoritmes zou realiseren dan wordt
	// het iets in de trant van: "s:a:tvcrfFnNbBwWpm2"
	//
	// Algemene opties:
	// "s:" staat voor: -s xxx = omvang van het beheerde geheugen (in "eenheden")
	// "a:" staat voor: -a xxx = aantal alloc/free acties (aka new/delete)
	// "t"  staat voor: -t = code testen (in plaats van performance meten)
	// "v"  staat voor: -v = verbose mode (vertel wat er allemaal gebeurt)
	// "c"  staat voor: -c = check mode (bewaak 'free' acties)
	//
	// Opties om een beheeralgoritme uit te kiezen ...
	// r  staat voor: -r = random-fit allocator
	// f  staat voor: -f = first-fit allocator (lazy)
	// F  staat voor: -F = first-fit allocator (eager)
	// n  staat voor; -n = next-fit allocator (lazy)
	// N  staat voor; -N = next-fit allocator (eager)
	// b  staat voor; -b = best-fit allocator (lazy)
	// B  staat voor; -b = best-fit allocator (eager)
	// w  staat voor; -w = worst-fit allocator (lazy)
	// W  staat voor; -w = worst-fit allocator (eager)
	//  enz
	// 2  staat voor: -2 = buddy allocator
	//
	// Voor meer informatie, zie: man 3 getopt
	//

	int  opt = 0; // Het "huidige" optie karakter (of -1).
	do {
		// Haal een optie uit argc/argv
		// (en zet zonodig het bijbehorende argument in 'optarg')
		opt = getopt(argc, argv, options);
		switch (opt) {	// welke optie is dit?
				// ALGEMEEN
			case 's': // the size of the (imaginary) memory being managed
				size = atol(optarg);   // NB atol = "ascii to long"
				break;
			case 'a': // the number of alloc/free actions
				aantal = atol(optarg);
				break;
			case 't': // toggle test mode
				tflag = !tflag;
				break;
			case 'v': // toggle verbose mode
				vflag = !vflag;
				break;
			case 'c': // toggle check mode
				cflag = !cflag;
				break;

				// ALGORITMES
			case 'r': // -r = RandomFit allocator gevraagd
				beheerders.push_back( new RandomFit );
				break;
			case 'f': // -f = FirstFit allocator gevraagd (lazy)
				beheerders.push_back( new FirstFit );
				break;
			case 'F': // -F = FirstFit allocator gevraagd (eager)
				beheerders.push_back( new FirstFit2 );
				break;
			case 'n': // -n = NextFit allocator gevraagd
				beheerders.push_back( new NextFit );
				break;
			case 'N': // -n = NextFit2 allocator gevraagd
				beheerders.push_back( new NextFit2 );
				break;
			// TODO:
			/*
			case 'b': // -b = BestFit allocator gevraagd
				beheerders.push_back( new BestFit );
				break;
			case 'B': // -B = BestFit2 allocator gevraagd
				beheerders.push_back( new BestFit2 );
				break;
			case 'w': // -w = WorstFit allocator gevraagd
				beheerders.push_back( new WorstFit );
				break;
			case 'W': // -W = WorstFit2 allocator gevraagd
				beheerders.push_back( new WorstFit2 );
				break;
				// enz
			case '2':	// -2 = buddy allocator gevraagd
				beheerders.push_back( new Buddy );
				break;
			*/

			case -1: // = einde opties
				return; // klaar met optie analyze

			default: // eh? iets onbekends gevonden (of zelf een case vergeten!)
				// foutmelding geven ...
				cerr << AC_RED "Found unknown option '" << char(optopt) << "'" AA_RESET << endl;
				tellOptions(argv[0]);
				exit(EXIT_FAILURE);
		}
	} while (opt != -1);     // tot einde opties
}

// ===================================================================
#include "Application.h"	// De pseudo applicatie


/// Programma executie begint hier ...
int  main(int argc, char *argv[])
{
	// Hier begint het echte werk
	try {
		// De meegegeven opties afhandelen.
		// Neveneffect: dit zal diverse globale variabelen veranderen!
		doOptions(argc, argv);

		// --- sanity checks ---
		// Omvang van het beheerde geheugen
		check(size > 0);
		// Het aantal allocaties moet minstens 2 zijn
		// (ivm berekening standaard deviatie!)
		// Bij een random-scenario is dit helaas geen garantie
		// van correctheid omdat er ook nog 'free' acties zijn.
		if (aantal < 2) {
			cerr << AC_RED "Het aantal moet minstens 2 zijn" AA_RESET "\n";
			exit(EXIT_FAILURE);
		}
		// Is er wel een geheugen-beheerder module gekozen ?
		if (beheerders.empty()) {
			// Ahum, gebruiker's foutje ...
			cerr << AC_RED "Eh, geen geheugen-beheerder gekozen ..." AA_RESET "\n";
			tellOptions(argv[0]);	// vertel wat kan.
			exit(EXIT_FAILURE);
		}
		// --- oke ---

		// For all chosen allocators do
		for (Allocator* beheerder : beheerders)
		{
			// Stel de geheugen-beheerder in ...
			beheerder->setSize(size);
			beheerder->setCheck(cflag);

			// ... en maak dan een pseudo-applicatie met die beheerder
			Application  *mp = new Application(beheerder, size);

			if (tflag) {    // De -t optie gezien ?
				cout << AC_BLUE "Testing " << beheerder->getType()
					 << " with " << size << " units\n" AA_RESET;
				mp->testing(); // ga dan de code testen
			} else {
				cout << AC_BLUE "Measuring " << beheerder->getType()
					 << " doing " << aantal << " calls on " << size << " units\n" AA_RESET;
				mp->randomscenario(aantal, vflag);
				// TODO:
				// .. vervang straks 'randomscenario' door iets toepasselijkers
				// zodat je ook voorspelbare scenarios kan afhandelen.
			}

			// Nu alles weer netjes opruimen
			delete  mp;
			delete  beheerder;
		}// for-each

	} catch (const char *error) {
		cerr << AC_RED "OEPS: " << error << AA_RESET << endl;
		exit(EXIT_FAILURE);
	} catch (const std::exception& error) {
		cerr << AC_RED << error.what() << AA_RESET << endl;
		exit(EXIT_FAILURE);
	} catch (...) {
		cerr << AC_RED "OEPS: Some unexpected exception occurred" AA_RESET << endl;
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

// vim:sw=4:ai:aw:ts=4:
