#pragma once
#ifndef	__Allocator_h__
#define	__Allocator_h__ 2.1
//#ident	"@(#)Allocator.h	2.1	AKK	20090222"

#include "Area.h"	// voor: class Area

/** @file Allocator.h
 *  @brief The Allocator interface.
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 2.1	2009/02/22
 */

/** @class Allocator
 * Beschrijft de interface van een geheugenbeheer class/module.
 */
class Allocator
{
protected:

	bool		 cflag;	///< "check" mode on/off;
						///< When check mode is enabled various extra sanitychecks are
						///< expected to be activated (which may cost some performance).

	const char	*type;	///< De naam van het algoritme b.v. "FirstFit", "Buddy", etc

	int			 size;	///< Hoeveel geheugen we beheren.
						///< Uitgedrukt in een willekeurige eenheid

	/// The constructor of derived classes should use this constructor
	/// to set common data.
	/// @param cflag	initiele toestand van de checkmode vlag
	/// @param type		naam van het algorithme
	Allocator(bool cflag, const char *type);

public:

	virtual ~Allocator() {} // ivm inheritance

	/// Vraag de naam van het algorithme op.
	const char	*getType()	const	{ return type; }

	/// Hoeveel geheugen beheren we.
	int			getSize()	const	{ return size; }

	/// Verander de check-mode.
	void		setCheck(bool c)	{ cflag = c; }

	/// Stel de omvang van het te beheren gebied in.
	// Afgeleide classes MOGEN setSize zelf herdefinieren
	// zolang ze deze methode maar aanroepen via
	//		Allocator::setSize(...);
	virtual void setSize(int size);

	// Afgeleide classes MOETEN de volgende methodes zelf definieren.
	virtual Area *alloc(int wanted) = 0;	///< Application vraagt om geheugen
	virtual void  free(Area *) = 0;			///< Application geeft een Area weer terug aan geheugenbeheer
	virtual void  report() = 0;				///< Report performance statistics

	// ... en hier komen straks misschien nog andere functies ...
	// ... om b.v. de overhead te bepalen ...
	// ... of de fragmentatie graad ...
	//

};

#endif	/*Allocator_h*/
// vim:sw=4:ai:aw:ts=4:
