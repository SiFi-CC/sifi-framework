// @(#)lib/fibers:$Id$

#ifndef SNEEDLESFLOODMAPPOS_H
#define SNEEDLESFLOODMAPPOS_H

#include "SCalContainer.h"
#include "SFibersLookup.h"

struct SLookupChannel;

class SNeedlesFloodmapPos : public SCalContainer<2>
{
public:
    using SCalContainer::SCalContainer;

    SLookupChannel* createChannel() const override { return new SFibersChannel; }
};
#endif /* SNEEDLESFLOODMAPPOS_H */

