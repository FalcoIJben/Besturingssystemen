//#ident	"@(#)Stopwatch.cc	2.2	AKK	20120722"
/** @file Stopwatch.cc
 * De implementatie van Stopwatch.
 */

#include <iostream>     // std::cerr
#include <cstdio>		// for: printf()

#include "asserts.h"	// for: require()
#include "Stopwatch.h"


// platform/timer dependent includes
#if S_TIMES
# include <unistd.h>	// for: getconf()
#endif
#if S_TIME
# include <time.h>
#endif


void	Stopwatch::init()
{
#if S_TIMES
	tps = sysconf(_SC_CLK_TCK);	// ticks-per-second (usually 100)
#endif
#if	S_GPT32
	handle = GetCurrentProcess();
#endif
	reset();
}


void	Stopwatch::start()
{
	require(ticking == false);	// stopwatch already started?
	ticking = true;
	// Record start state
#if	S_RUSAGE
	(void) getrusage(RUSAGE_SELF, &ruse0);
#endif
#if	S_TIMES
	(void) ::times(&tms0);
#endif
#if	S_CLOCK
	clock0 = clock();
#endif
#if S_TIME
	(void) time(&time0);
#endif
#if	S_GPT32
	if (!handle) { init(); }
	GetProcessTimes(handle, &fromCT, &fromET, &fromKT, &fromUT);
#endif
}


#if	S_RUSAGE
#define	R_USAGE_QUOTUM	1000000			// i.e. the number of microseconds in a second
// Since a timeval has to separate parts, doing
// arithmetic on them may produce abnormal part values.
void	normalizeTimeval(timeval& x)
{
	while (x.tv_sec < 0) {					// "borrow" from usec
		x.tv_sec  += 1;
		x.tv_usec -= R_USAGE_QUOTUM;
	}
	while (x.tv_usec < 0) {					// "borrow" from sec
		x.tv_sec  -= 1;
		x.tv_usec += R_USAGE_QUOTUM;
	}
	while (x.tv_usec >= R_USAGE_QUOTUM) {	// "carry" usec to sec
		x.tv_sec  += 1;
		x.tv_usec -= R_USAGE_QUOTUM;
	}
}
#endif


void	Stopwatch::stop()
{
	// Record stop state,
	// then calculate the difference with the start state
#if	S_RUSAGE
	(void) getrusage(RUSAGE_SELF, &ruse1);

	// Calculate user CPU time difference
	ruse1.ru_utime.tv_sec  -= ruse0.ru_utime.tv_sec;		// seconds part
	ruse1.ru_utime.tv_usec -= ruse0.ru_utime.tv_usec;		// microseconds part
	normalizeTimeval(ruse1.ru_utime);
	// Calculate system CPU time difference
	ruse1.ru_stime.tv_sec  -= ruse0.ru_stime.tv_sec;		// seconds part
	ruse1.ru_stime.tv_usec -= ruse0.ru_stime.tv_usec;		// microseconds part
	normalizeTimeval(ruse1.ru_stime);

	// Calculate total user CPU time usage
	ruset.ru_utime.tv_sec  += ruse1.ru_utime.tv_sec;		// seconds part
	ruset.ru_utime.tv_usec += ruse1.ru_utime.tv_usec;		// microseconds part
	normalizeTimeval(ruset.ru_utime);
	check(ruset.ru_utime.tv_sec >= 0);						// sanity check
	// Calculate total system CPU time usage
	ruset.ru_stime.tv_sec  += ruse1.ru_stime.tv_sec;		// seconds part
	ruset.ru_stime.tv_usec += ruse1.ru_stime.tv_usec;		// microseconds part
	normalizeTimeval(ruset.ru_stime);
	check(ruset.ru_stime.tv_sec >= 0);						// sanity check

	// Update totals over all
	total.tv_sec  += ruset.ru_utime.tv_sec  + ruset.ru_stime.tv_sec ;
	total.tv_usec += ruset.ru_utime.tv_usec + ruset.ru_stime.tv_usec;
	normalizeTimeval(total);
	// and summerize that as a grand total
	total_time  = double(total.tv_usec) / double(R_USAGE_QUOTUM);
	total_time += double(total.tv_sec);
#endif
#if	S_TIMES
	(void) ::times(&tms1);
	// Calculate the time differences
	tms1.tms_utime  -= tms0.tms_utime;		// self user cpu time
	tms1.tms_stime  -= tms0.tms_stime;		// self system cpu time
	tms1.tms_cutime -= tms0.tms_cutime;		// children user cpu time
	tms1.tms_cstime -= tms0.tms_cstime;		// children system cpu time
	// Add these times to the total times
	tmst.tms_utime  += tms1.tms_utime;		// self user cpu time
	tmst.tms_stime  += tms0.tms_stime;		// self system cpu time
	tmst.tms_cutime += tms1.tms_cutime;		// children user cpu time
	tmst.tms_cstime += tms1.tms_cstime;		// children system cpu time
	// Update totals
	totals += tmst.tms_utime + tmst.tms_stime + tmst.tms_cutime + tmst.tms_cstime;
	total_time = double(totals) / double(tps);
#endif
#if	S_CLOCK
	clock1 = clock();
	clockt += clock1 - clock0;
	total_time = double(clockt) / double(CLOCKS_PER_SEC);
#endif
#if S_TIME
	(void) time(&time1);
	timet += time1 - time0;
	total_time = timet;
#endif
#if	S_GPT32
	if (GetProcessTimes(handle, &uptoCT, &uptoET, &uptoKT, &uptoUT) != 0)
	{
		LARGE_INTEGER	temp;
		long long		user, kernel;

		temp.LowPart  = uptoKT.dwLowDateTime ;
		temp.HighPart = uptoKT.dwHighDateTime;
		kernel = temp.QuadPart;

		temp.LowPart  = uptoUT.dwLowDateTime ;
		temp.HighPart = uptoUT.dwHighDateTime;
		user = temp.QuadPart;

		temp.LowPart  = fromKT.dwLowDateTime ;
		temp.HighPart = fromKT.dwHighDateTime;
		kernel -= temp.QuadPart;

		temp.LowPart  = fromUT.dwLowDateTime ;
		temp.HighPart = fromUT.dwHighDateTime;
		user -= temp.QuadPart;

		// Have to convert from 100ns units to milli-secs later
		w32_user   += user  ;
		w32_kernel += kernel;
		// update total
		w32_total   = w32_user + w32_kernel;
		total_time  = double(w32_total) / 10000000.0;
	} else {
		DWORD  errorno = GetLastError();
		std::cerr << "OOPS: GetProcessTimes failed (errno=" << errorno << ")\n";
		exit(9);
	}
#endif

	// Do the sanity check AFTER the timing
	// to prevent interfering with the times recored
	require(ticking == true);		// stopwatch not started?

	ticking = false;
}


void	Stopwatch::reset()
{
	require(ticking == false);	// not while measuring time!

	total_time = 0;
#if	S_RUSAGE
	ruset.ru_utime.tv_sec  = ruset.ru_stime.tv_sec  = 0;
	ruset.ru_utime.tv_usec = ruset.ru_stime.tv_usec = 0;
	total.tv_sec = total.tv_usec = 0;
#endif
#if	S_TIMES
	tmst.tms_utime = tmst.tms_stime = tmst.tms_cutime = tmst.tms_cstime = 0;
	totals = 0;
#endif
#if	S_CLOCK
	clockt = 0;
#endif
#if S_TIME
	timet = 0;
#endif
#if	S_GPT32
	w32_user = w32_kernel = w32_total = 0;
#endif
}


#if 0 // Maybe this information will be used later
struct rusage
{
	struct timeval ru_utime; /* user CPU time used */
	struct timeval ru_stime; /* system CPU time used */
	long   ru_maxrss;	/* maximum resident set size */
	...					/* some attributes not supported on linux */
	long   ru_minflt;	/* page reclaims (soft page faults) */
	long   ru_majflt;	/* page faults (hard page faults) */
	long   ru_inblock;	/* block input operations */
	long   ru_oublock;	/* block output operations */
	...					/* some attributes not supported on linux */
	long   ru_nvcsw;	/* voluntary context switches */
	long   ru_nivcsw;	/* involuntary context switches */
};
#endif


void	Stopwatch::report() const
{
	require(ticking == false);	// not while measuring time!
#if	S_RUSAGE
	std::printf("rusage: user %d.%06d sec, system %d.%06d sec, total %d.%06d\n",
	            int(ruset.ru_utime.tv_sec), int(ruset.ru_utime.tv_usec),
	            int(ruset.ru_stime.tv_sec), int(ruset.ru_stime.tv_usec),
	            int(total.tv_sec),			int(total.tv_usec)
	           );
#endif
#if	S_TIMES
	std::printf("times: main: user %.2f sec, system %.2f sec;"
	            " children: user %.2f sec, system %.2f sec;"
	            " total %.2f\n",
	            (double(tmst.tms_utime)  / double(tps)),
	            (double(tmst.tms_stime)  / double(tps)),
	            (double(tmst.tms_cutime) / double(tps)),
	            (double(tmst.tms_cstime) / double(tps)),
	            (double(totals) / double(tps)));
#endif
#if	S_CLOCK
	// clock_t is expressed in CLOCKS_PER_SEC (1000000?)
	std::printf("clock: realtime %.2f sec, at %ld ticks/sec\n",
	            total_time, CLOCKS_PER_SEC);
#endif
#if S_TIME
	std::printf("time: %ld seconds passed\n", timet);
#endif
#if	S_GPT32
	// win32 ticks are 100ns each (i.e. 10,000,000 ticks per second)
	double  u = double(w32_user)   / 10000000.0;
	double  k = double(w32_kernel) / 10000000.0;
	std::printf("gpt: user %I64d ticks, kernel %I64d ticks, total %I64d (1tick=100ns)\n",
	            w32_user, w32_kernel, w32_total);
	std::printf("gpt: user %.2f sec, kernel %.2f sec, total %.2f\n", u, k, total_time);
#endif
}

// vim:sw=4:ai:aw:ts=4:
