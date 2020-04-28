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

/**
 * \class SCalContainer
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

/**
 * Read parameters from a container. See SLookupChannel::read() for details how to
 * implement.
 *
 * \sa SLookupChannel
 * \param buffer string containing data to parse
 * \return number of parsed elements.
 */
uint SCalPar::read(const char* buffer)
{
    uint n;
    int cnt = sscanf(buffer, "%f %f %f%n", &par0, &par1, &par2, &n);
    assert(cnt == 3);
    return n;
}

/**
 * Exports the parameters to a container. See SLookupChannel::write() for
 * details how to implement.
 *
 * \sa SLookupChannel
 * \param[out] buffer buffer to be written
 * \param n buffer length
 * \return number of written bytes.
 */
uint SCalPar::write(char* buffer, size_t n) const
{
    int cnt = snprintf(buffer, n, "%7.6g %7.6g %7.6g", par0, par1, par2);
    if (cnt < 0) return cnt;
    if (cnt < n) return 0;
    return cnt;
}

/**
 * Prints the calibration parameters. See SLookupChannel::print() for details. 
 *
 * \sa SLookupChannel
 * \param newline puts newline on the end
 * \param prefix a text which should be displayed before the content of the
 * channel params. If prefix is empty, then
 */
void SCalPar::print(bool newline, const char * prefix)
{
    printf("%s %f %f %f", prefix, par0, par1, par2);
    if (newline) putchar('\n');
}

/**
 * Constructor
 *
 * \param container container name
 */
SCalContainer::SCalContainer(const std::string& container)
    : name(container), is_init(false)
{
}

/**
 * Read cal parameters from respective SContainer. Uses lookup channel hash as
 * a key for the map, and SCalPar or derivied for a value.
 *
 * \sa SLookupTable::fromContainer()
 */
void SCalContainer::fromContainer()
{
    SContainer * lc = pm()->getContainer(name);
    if (!lc) throw "No lookup container.";

    const std::vector<std::string> & lv = lc->lines;

    for (auto line: lv)
    {
        SLookupChannel * chan = createChannel();
        int cnt = chan->read(line.c_str());
        SCalPar cp;
        cp.read(line.c_str() + cnt);

        calpars.insert({chan->quickHash(), cp});
    }

    is_init = true;
}

/**
 * Write cal parameters to respective SContainer.
 *
 * \sa fromContainer()
 * \sa SLookupTable::toContainer()
 */
void SCalContainer::toContainer() const
{
    SContainer * sc = pm()->getContainer(name);
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

/**
 * Get single SCalPar object fr given channel described by SLookupChannel
 * object.
 *
 * \todo Probably we need to develop better mechanism here, maybe converting
 * constructor?
 *
 * \param channel channel object
 * \return calibration parameter object
 */
SCalPar& SCalContainer::getPar(const SLookupChannel * channel) {
    if (!is_init) fromContainer();

    uint64_t hash = channel->quickHash();
    std::map<size_t, SCalPar>::iterator it = calpars.find(hash);

    assert(it != calpars.end());
    return it->second;
}

/**
 * Print all parameters from given cal container.
 */
void SCalContainer::print()
{
    if (!is_init) fromContainer();

    printf("[%s]\n", name.c_str());
    for (auto calpar: calpars)
    {
        SLookupChannel * chan = createChannel();
        chan->fromHash(calpar.first);
        chan->print("");
        calpar.second.print("  ");
    }

}
