
#ifndef SFIBERSSTACKHITFINDER_H
#define SFIBERSSTACKHITFINDER_H

#include "SCalContainer.h"
#include "STask.h"

class SCategory;
class SFibersStackGeomPar;
class SFibersStackHitFinderPar;

class SFibersStackHitFinder : public STask
{
  protected:
    // members
    SCategory* catFibersCal{nullptr};                 ///< fibers cal category
    SCategory* catFibersHit{nullptr};                 ///< fibers hit category
    SCalContainer<3>* pHitFinderFiberPar{nullptr};    ///< hit finder fiber parameters
    SFibersStackHitFinderPar* pHitFinderPar{nullptr}; ///< hit finder parameters
    SFibersStackGeomPar* pGeomPar;

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
