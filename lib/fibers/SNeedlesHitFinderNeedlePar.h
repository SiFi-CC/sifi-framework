// @(#)lib/fibers:$Id$

#ifndef SNEEDLESHITFINDERNEEDLEPAR_H
#define SNEEDLESHITFINDERNEEDLEPAR_H

#include "SCalContainer.h"
#include "SFibersLookup.h"

struct SLookupChannel;

class SNeedlesHitFinderNeedlePar : public SCalContainer<2>
{
public:
    using SCalContainer::SCalContainer;

    SLookupChannel* createChannel() const override { return new SFibersChannel; }
};
#endif /* SNEEDLESHITFINDERNEEDLEPAR_H */

