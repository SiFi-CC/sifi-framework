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
#include "SLookup.h"
#include "SParManager.h"

#include <iostream>
#include <sstream>

#include <cassert>
#include <cstring>

#include <experimental/iterator>

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
template <int N> uint SCalPar<N>::read(const char* buffer)
{
    int cnt = 0;
    std::istringstream sstr(buffer);

    while (sstr >> par[cnt] && cnt < N)
        ++cnt;

    assert(cnt == N);
    return cnt;
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
template <int N> uint SCalPar<N>::write(char* buffer, size_t n) const
{
    std::ostringstream sstr;
    for (const auto& v : par)
    {
        sstr.width(15);
        sstr.setf(std::ios::scientific, std::ios::floatfield);
        sstr.fill(' ');
        sstr << v;
    }

    if (sstr.str().length() >= n) return -sstr.str().length();

    std::strcpy(buffer, sstr.str().c_str());

    return 0;
}

template <int N> float & SCalPar<N>::operator[](int n) {
    if (n < 0 or n > N-1) {
        std::cerr << "SCalPar<N>::operator[]: Index n=" << n << " outside range(0;" << N-1 << ")" << std::endl;
        abort();
    }
    return par[n];
}

template <int N> const float SCalPar<N>::operator[](int n) const {
    if (n < 0 or n > N-1) {
        std::cerr << "SCalPar<N>::operator[]: Index n=" << n << " outside range(0;" << N-1 << ")" << std::endl;
        abort();
    }
    return par[n];
}

/**
 * Prints the calibration parameters. See SLookupChannel::print() for details.
 *
 * \sa SLookupChannel
 * \param newline puts newline on the end
 * \param prefix a text which should be displayed before the content of the
 * channel params. If prefix is empty, then
 */
template <int N> void SCalPar<N>::print(bool newline, const char* prefix)
{
    std::cout << prefix << " ";
    std::copy(std::begin(par), std::end(par),
              std::experimental::make_ostream_joiner(std::cout, " "));
    if (newline) putchar('\n');
}

/**
 * Constructor
 *
 * \param container container name
 */
template <int N>
SCalContainer<N>::SCalContainer(const std::string& container) : name(container), is_init(false)
{
}

template <int N> SCalContainer<N>::~SCalContainer()
{
    for (auto& p : calpars)
        delete p.second;
}

/**
 * Read cal parameters from respective SContainer. Uses lookup channel hash as
 * a key for the map, and SCalPar or derivied for a value.
 *
 * \sa SLookupTable::fromContainer()
 */
template <int N> void SCalContainer<N>::fromContainer()
{
    SContainer* lc = pm()->getContainer(name);
    if (!lc) throw "No lookup container.";

    for (const auto& line : lc->lines)
    {
        SLookupChannel* chan = createChannel();
        int cnt = chan->read(line.c_str());
        SCalPar<N>* cp = new SCalPar<N>;
        cp->read(line.c_str() + cnt);

        calpars.insert({chan->quickHash(), cp});
        delete chan;
    }

    is_init = true;
}

/**
 * Write cal parameters to respective SContainer.
 *
 * \sa fromContainer()
 * \sa SLookupTable::toContainer()
 */
template <int N> void SCalContainer<N>::toContainer() const
{
    SContainer* sc = pm()->getContainer(name);
    if (!sc) throw "No lookup container.";

    sc->lines.clear();

    const int len = 1024;
    char buff[len];

    for (auto calpar : calpars)
    {
        SLookupChannel* chan = dynamic_cast<SLookupChannel*>(createChannel());
        chan->fromHash(calpar.first);
        chan->write(buff, len);
        std::string s("  ");
        s += buff;
        s += "\t\t";
        calpar.second->write(buff, len);
        s += buff;
        sc->lines.push_back(s);
    }
}

/**
 * Get single SCalPar object for given channel described by SLookupChannel
 * object.
 *
 * \todo Probably we need to develop better mechanism here, maybe converting
 * constructor?
 *
 * \param channel channel object
 * \return calibration parameter object
 */
template <int N> SCalPar<N>* SCalContainer<N>::getPar(const SLookupChannel* channel)
{
    if (!is_init) fromContainer();

    uint64_t hash = channel->quickHash();
    auto it = calpars.find(hash);

    if (it == calpars.end())
    {
        if (def)
        {
            SCalPar<N>* cp = new SCalPar<N>;
            *cp = *def;
            calpars.insert({channel->quickHash(), cp});
            return cp;
        }
    }
    else
    {
        assert(it != calpars.end());
    }

    return it->second;
}

/**
 * Print all parameters from given cal container.
 */
template <int N> void SCalContainer<N>::print()
{
    if (!is_init) fromContainer();

    printf("[%s]\n", name.c_str());
    for (auto& calpar : calpars)
    {
        SLookupChannel* chan = createChannel();
        chan->fromHash(calpar.first);
        chan->print(false, "");
        calpar.second->print(true, "  ");
    }
}

template class SCalPar<2>;
template class SCalPar<3>;
template class SCalPar<6>;
template class SCalContainer<2>;
template class SCalContainer<3>;
template class SCalContainer<6>;
