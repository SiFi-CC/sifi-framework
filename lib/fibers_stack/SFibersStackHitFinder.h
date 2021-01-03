
#ifndef SFIBERSSTACKHITFINDER_H
#define SFIBERSSTACKHITFINDER_H

#include "STask.h"
#include "SCalContainer.h"

class SCategory;

class SFibersStackHitFinder : public STask
{
protected:
    // members
    SCategory * catFibersCal{nullptr};              ///< fibers cal category
    SCategory * catFibersHit{nullptr};              ///< fibers hit category
    SCalContainer<2> * pHitFinderFiberPar{nullptr}; ///< hit finder fiber parameters

public:
    // constructor
    SFibersStackHitFinder() = default;
    // destructor
    virtual ~SFibersStackHitFinder() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SFIBERSSTACKHITFINDER_H */
