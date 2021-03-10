
#ifndef SFIBERSSTACKCLUSTERFINDER_H
#define SFIBERSSTACKCLUSTERFINDER_H

#include "SCalContainer.h"
#include "STask.h"

class SCategory;
class SFibersStackGeomPar;
class SFibersStackClusterFinderPar;

class SFibersStackClusterFinder : public STask
{
protected:
    // members
    SCategory* catFibersHit{nullptr};                         ///< fibers cal category
    SCategory* catFibersCluster{nullptr};                     ///< fibers cluster category
    SFibersStackClusterFinderPar* pClusterFinderPar{nullptr}; ///< cluster finder parameters
    SFibersStackGeomPar* pGeomPar;

public:
    // constructor
    SFibersStackClusterFinder() = default;
    // destructor
    virtual ~SFibersStackClusterFinder() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SFIBERSSTACKCLUSTERFINDER_H */
