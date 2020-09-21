
#ifndef SFIBERSSTACKHITFINDER_H
#define SFIBERSSTACKHITFINDER_H

#include "STask.h"

class SCategory;
class SCalContainer;
class SFibersStackCalibratorPar;
class SFibersStackCalibrator; //?

//class SFibersStackCalibrator : public STask
class SFibersStackHitFinder : public STask
{
protected:
    // members
    SCategory * catFibersCal;                   ///< fibers cal category
    SCategory * catFibersHit;                   ///< fibers hit category
    //SCalContainer * pHitFinderPar; ///< hit finder parameters
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
