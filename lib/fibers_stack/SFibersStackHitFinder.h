
#ifndef SFIBERSSTACKHITFINDER_H
#define SFIBERSSTACKHITFINDER_H

#include "STask.h"
#include "SCalContainer.h"
#include "SLookup.h"

class SCategory;
class SFibersStackHitFinderPar;

class SFibersStackHitFinder : public STask
{
protected:
    // members
    SCategory * catFibersCal;                   ///< fibers cal category
    SCategory * catFibersHit;                   ///< fibers hit category
    SCalContainer * pHitFinderFiberPar;         ///< hit finder fiber parameters
    SFibersStackHitFinderPar * pHitFinderPar;   ///< hit finder parameters
    //SFibersStackLookupTable * pLookUp;        ///< lookup table. needed?
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
