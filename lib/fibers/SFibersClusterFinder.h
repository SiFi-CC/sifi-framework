
#ifndef SFIBERSCLUSTERFINDER_H
#define SFIBERSCLUSTERFINDER_H

#include "SCalContainer.h"
#include "STask.h"

class SCategory;
class SFibersGeomPar;
class SFibersClusterFinderPar;

class SFibersClusterFinder : public STask
{
protected:
    // members
    SCategory* catFibersHit{nullptr};                         ///< fibers cal category
    SCategory* catFibersCluster{nullptr};                     ///< fibers cluster category
    SFibersClusterFinderPar* pClusterFinderPar{nullptr}; ///< cluster finder parameters
    SFibersGeomPar* pGeomPar;

public:
    // constructor
    SFibersClusterFinder() = default;
    // destructor
    virtual ~SFibersClusterFinder() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SFIBERSCLUSTERFINDER_H */
