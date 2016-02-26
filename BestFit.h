#ifndef BESTFIT_H_INCLUDED
#define BESTFIT_H_INCLUDED



#include "Fitter.h"

/// @class BestFit
/// Het BestFit algorithme gebruikt het kleinst
/// gevonden bruikbare gebied in de resource map dat groot genoeg is.
class BestFit : public Fitter
{

    public:
    ///constructor
    BestFit(bool cflag=false, const char *type = "BestFit (lazy)")
		: Fitter(cflag, type) {}

    ///destuctor
    ~BestFit();

    void	 setSize(int new_size);	///< initialize memory size


	/// Ask for an area of at least 'wanted' units.
	/// @returns	An area or 0 if not enough freespace available
	virtual  Area	*alloc(int wanted);	// application asks for space

	/// The application returns an area to freespace.
	/// @param ap	The area returned to free space
	virtual  void	free(Area *ap);

    protected:

    //areas small to large
    std::list<Area*>  areas;



    Area 	*searcher(int);

	/// This function is called when the searcher can not find space.
	/// It tries to reclaim fragmented space by merging adjacent free areas.
	/// @returns true if free areas could be merged, false if no adjacent areas exist
	virtual	 bool	  reclaim();


	virtual  void	updateStats();	///< update resource map statistics






};



#endif // BESTFIT_H_INCLUDED
