
#ifndef SNEEDLESHITFINDER_H
#define SNEEDLESHITFINDER_H

#include "sifi_export.h"

#include "STask.h"

class SCategory;
class SFibersGeomPar;
class SFibersHitFinderPar;
class SDetectorGeomPar;
class SPixelDetectorGeomPar;
class SMultiFibersLookupTable;
template <int N> class SCalContainer;

class SNeedlesHitFinder : public STask
{
protected:
    // members
    SCategory* catSiPMClus{nullptr};                ///< SiPM cluster input category
    SCategory* catFibersHit{nullptr};               ///< needle hit output category
    SCalContainer<2>* pHitFinderNeedlePar{nullptr}; ///< hit finder needle parameters
    SFibersHitFinderPar* pHitFinderPar{nullptr};    ///< hit finder parameters
    SMultiFibersLookupTable* sipmNeedleLookup{nullptr}; ///< SiPM-to-needle lookup
    SCalContainer<2>* pFloodmapPositions{nullptr};  ///< needle floodmap positions
    SPixelDetectorGeomPar* pDetectorGeomPar{nullptr};   ///< detector geometry container
    SFibersGeomPar* pGeomPar{nullptr};              ///< geometry container

public:
    // constructor
    SNeedlesHitFinder() = default;
    // destructor
    virtual ~SNeedlesHitFinder() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SNEEDLESHITFINDER_H */
