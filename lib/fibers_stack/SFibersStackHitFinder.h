
#ifndef SFIBERSSTACKHITFINDER_H
#define SFIBERSSTACKHITFINDER_H

#include "STask.h"
#include "SCalContainer.h"
#include "SLookup.h"
#include "SFibersStackHitFinderPar.h"

class SCategory;

//class SFibersStackCalibrator : public STask
class SFibersStackHitFinder : public STask
{
protected:
    // members
    SCategory * catFibersCal;                   ///< fibers cal category
    SCategory * catFibersHit;                   ///< fibers hit category
    //SCalContainer * pHitFinderPar; ///< hit finder parameters
    SFibersStackHitFinderPar * pHitFinderPar; ///< hit finder parameters
    //SFibersStackLookupTable * pLookUp;          ///< lookup table. needed?
public:
    // constructor
    SFibersStackHitFinder();
    // destructor
    virtual ~SFibersStackHitFinder() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SFIBERSSTACKHITFINDER_H */
