
#ifndef SFIBERSCLUSTERFINDER_H
#define SFIBERSCLUSTERFINDER_H

#include "STask.h"

class SCategory;
class SFibersGeomPar;
class SFibersClusterFinderPar;

class SFibersClusterFinder : public STask
{
protected:
    // members
    SCategory* catFibersHit{nullptr};                    ///< fibers cal category
    SCategory* catFibersCluster{nullptr};                ///< fibers cluster category
    SFibersClusterFinderPar* pClusterFinderPar{nullptr}; ///< cluster finder parameters
    SFibersGeomPar* pGeomPar;                            ///< geometry parameters

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
