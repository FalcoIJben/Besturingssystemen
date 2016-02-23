#pragma once
#ifndef	__main_h__
#define	__main_h__	4.1
//#ident	"@(#)main.h	4.1	AKK	20150124"

/** @file main.h
 *  @brief Stuff needed in several places
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 4.1	2015/01/24
 */

// ------------------------------------------------------
// Collected here in one place for convenience
// ------------------------------------------------------

// c++ headers
#include <iostream>		// de STL std::cin, std::cout en std::cerr
// eigen headers
#include "common.h"		// platform stuff
#include "ansi.h"		// ansi color codes
#include "asserts.h"	// require(...), notreached(), etc
// project headers
#include "Area.h"		// class Area
#include "Allocator.h"	// baseclass Allocator

// vim:sw=4:ai:aw:ts=4:
#endif	/*main_h*/
