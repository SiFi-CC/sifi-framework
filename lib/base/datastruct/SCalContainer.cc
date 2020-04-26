// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SCalContainer.h"
#include "SParManager.h"
#include "SLookup.h"

#include <iostream>
#include <sstream>

#include <cassert>

/** \class SCalContainer
\ingroup lib_base

SLookup is an abstract class to hold container and geometry parameters.

It must be derivated and pure virtual members defined.

The parameters are parsed from text file in SLookupManager and stored in the
SCalContainer. The getParam() method reads content of the SCalContainer and
fills variables inside the SLookup object. The putParam method allows to update
parameters in the container and write to param file.

\sa SFibersStackCalibratorPar
\sa SFibersStackDigitizerPar
\sa SFibersStackGeomPar
*/

uint SCalPar::read(const char* data)
{
    uint n;
    int cnt = sscanf(data, "%f %f %f%n", &par0, &par1, &par2, &n);
    assert(cnt == 3);
    return n;
}

uint SCalPar::write(char* data, size_t n) const
{
    int cnt = snprintf(data, n, "%7.6g %7.6g %7.6g", par0, par1, par2);
    if (cnt < 0) return cnt;
    if (cnt < n) return 0;
    return cnt;
}

void SCalPar::print(const char * prefix)
{
    printf("%s %f %f %f", prefix, par0, par1, par2);
    if (prefix[0] != '\0') putchar('\n');
}

SCalContainer::SCalContainer(const std::string& container) :
    container(container), is_init(false)
{
}

void SCalContainer::fromContainer()
{
    SContainer * lc = pm()->getContainer(container);
    if (!lc) throw "No lookup container.";

    const std::vector<std::string> & lv = lc->lines;

    for (auto line: lv)
    {
        SLookupChannel * chan = createChannel();
        int cnt = chan->read(line.c_str());
        SCalPar cp;
        cp.read(line.c_str() + cnt);

        calpars.insert({chan->quick_hash(), cp});
    }

    is_init = true;
}

void SCalContainer::toContainer() const
{
    SContainer * sc = pm()->getContainer(container);
    if (!sc) throw "No lookup container.";

    sc->lines.clear();

    const int len = 1024;
    char buff[len];

    for (auto calpar: calpars)
    {
        SLookupChannel * chan = dynamic_cast<SLookupChannel *>(createChannel());
        chan->fromHash(calpar.first);
        chan->write(buff, len);
        std::string s("  ");
        s += buff;
        s += "\t\t";
        calpar.second.write(buff, len);
        s += buff;
        sc->lines.push_back(s);
    }
}

SCalPar& SCalContainer::getPar(const SLookupChannel * channel) {
    if (!is_init) fromContainer();

    uint64_t hash = channel->quick_hash();
    map_type::iterator it = calpars.find(hash);

    assert(it != calpars.end());
    return it->second;
}

void SCalContainer::print()
{
    if (!is_init) fromContainer();

    printf("[%s]\n", container.c_str());
    for (auto calpar: calpars)
    {
        SLookupChannel * chan = createChannel();
        chan->fromHash(calpar.first);
        chan->print("");
        calpar.second.print("  ");
    }

}
