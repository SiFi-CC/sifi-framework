
#ifndef SFIBERSHITFINDER_H
#define SFIBERSHITFINDER_H

#include "SCalContainer.h"
#include "STask.h"

class SCategory;
class SFibersGeomPar;
class SFibersHitFinderPar;

class SFibersHitFinder : public STask
{
protected:
    // members
    SCategory* catFibersCal{nullptr};                 ///< fibers cal category
    SCategory* catFibersHit{nullptr};                 ///< fibers hit category
    SCalContainer<3>* pHitFinderFiberPar{nullptr};    ///< hit finder fiber parameters
    SFibersHitFinderPar* pHitFinderPar{nullptr}; ///< hit finder parameters
    SFibersGeomPar* pGeomPar;

public:
    // constructor
    SFibersHitFinder() = default;
    // destructor
    virtual ~SFibersHitFinder() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SFIBERSHITFINDER_H */
