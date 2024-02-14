#ifndef SFIBERTOSIPMPAR_H
#define SFIBERTOSIPMPAR_H

#include "SCalContainer.h"
#include "SFibersLookup.h"

struct SLookupChannel;

class SFiberToSiPMPar : public SCalContainer<6>
{
public:
    using SCalContainer::SCalContainer;

    SLookupChannel* createChannel() const override { return new SFibersChannel; }
};
#endif /* SFIBERTOSIPMPAR_H */


