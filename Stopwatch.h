#pragma once
#ifndef	__Stopwatch_h__
#define	__Stopwatch_h__	4.1
//#ident	"@(#)Stopwatch.h	4.1	AKK	20150124"

/** @file Stopwatch.h
 *  @brief Defines the class Stopwatch that can measure the CPU time usage of your program.
 *
 *  The exact mechanism used depends on your platform
 *  and should be chosen by changing the header file.
 *  On win32 there is only one mechanism available: 'GPT32'.
 *  On unix the mechanism are (in order of preference):
 *  	'RUSAGE', 'TIMES', 'CLOCK' and 'TIME'.
 *
 *  @note
 *  Since version 2.3 we try to use the platform dependent defines
 *  to make the choice automatically. YMMV.
 *
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 4.1	2015/01/24
 */


// Select the time measuring method best matching the abilities of your platform
// by setting one #define to 1, and all others to 0.
// We tried to let the compiler make the choice automagically
// but success is not guarantied.

// Hack to handle MaxOS which fails to define __unix__
#if	!defined(__unix__) && defined(__APPLE__)
#  define __unix__ 1
#endif

// Options available on: win32 (DevC++, Code::Blocks, Cygwin?) ...
#define	S_GPT32	 __WIN32__	///< win32: GetProcessTimes(handle,...)
// NOTE:
// 	This works with Code::Blocks and DevC++
// 	Cygwin has not been tested yet.

// Options available on: unix, linux, macos? and maybe others ...
#define	S_RUSAGE __unix__	///< unix: use: getrusage(2)	// the favorite: preferable over times()
#define	S_TIMES		0	///< unix: use: times(2)		// a bit crude, but still better than clock()
#define	S_CLOCK		0	///< unix: use: clock(3)		// the CPU time used by the program, better than time()
#define S_TIME      0   ///< unix: use: time(2)			// rotten! wallclock time passed
						///< which has no true relation with the CPU time used

// - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Sanity checks for your choice
#if	(S_RUSAGE+S_TIMES+S_CLOCK+S_TIME+S_GPT32) == 0
# error  You forgot to select a timer method
#endif
#if ((S_RUSAGE+S_TIMES+S_CLOCK+S_TIME) > 0) && (S_GPT32 == 1)
# warning  You have selected cross-platform timer methods
#endif


// Linux includes to get the right typedefs
#if	 S_RUSAGE
# include <sys/resource.h>		// for: getrusage(2)
#endif
#if  S_TIMES
# ifdef  __unix__
#  include <sys/times.h>		// for: times(2)
# else
#  include <times.h>
# endif
#endif
#if  S_CLOCK || S_TIME
# ifdef  __unix__
#  include <time.h>				// for: clock(3) or time(2)
# else	/* DevC++ */
#  include <sys/time.h>			// for: clock(3) or time(2)
# endif
#endif
// The same for Win32
#if  S_GPT32
# include <windows.h>			// DevC++, CodeBlocks
#endif


/// @class Stopwatch
/// Een CPU-tijd waarnemer class.
class	Stopwatch
{
public:
	/// Create and initialize a stopwatch
	Stopwatch() : total_time(0), ticking(false) { init(); }

	void	start();		///< start the timer
	void	stop();			///< stop the timer
	void	report() const;	///< report the times
	void	reset();		///< reset the timer

	double	gettotal() const	{ return total_time; }	///< returns the total time

private:
	void	init();			// initializer

	double	total_time;		// total cpu time used
	bool	ticking;		// current state

	// The data for timekeeping depends on the timer mechanism
#if	S_RUSAGE
	struct rusage   ruse0;	// start specs
	struct rusage   ruse1;	// stop specs
	struct rusage   ruset;	// totals
	struct timeval  total; 	// to calculate user+system time
#endif
#if S_TIMES
	long		tps;		// ticks per second
	struct tms  tms0;		// start specs
	struct tms  tms1;		// stop specs
	struct tms  tmst;		// totals
	clock_t		totals;		// usr+sys/self+child
#endif
#if S_CLOCK
	clock_t clock0;			// start specs
	clock_t clock1;			// stop specs
	clock_t clockt;			// totals
#endif
#if S_TIME
	time_t  time0;			// start
	time_t  time1;			// stop
	time_t  timet;			// total
#endif
#if S_GPT32		// win32
	HANDLE	handle;			// CurrentProcessId
	// The Creation, Exit, Kernel and User times in 100ns ticks
	FILETIME	fromCT, fromET, fromKT, fromUT;
	FILETIME	uptoCT, uptoET, uptoKT, uptoUT;
	long long	w32_user, w32_kernel; // cumulatief
	long long	w32_total;
#endif
};

#endif	/*Stopwatch_h*/
// vim:sw=4:ai:aw:ts=4:
