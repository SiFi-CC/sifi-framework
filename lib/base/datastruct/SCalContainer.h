// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SCALCONTAINER_H
#define SCALCONTAINER_H

#include "sifi_export.h"

#include "SLookup.h"

#include <TArrayD.h>
#include <TArrayF.h>
#include <TArrayI.h>

#include <map>
#include <unordered_map>
#include <vector>

/**
 * Template that contains basic calibation parameters. Consist of a N-parameters array:
 *  * #par,
 * which interpretation can be any, depends of the user.
 *
 * This structure must provie interface to read and write cal pars to a
 * container. Each deriving class must also provide proper interface.
 *  * read() - reads cal pars from container
 *  * write() - writes cal pars to the container
 *  * print() - print cal par values
 */
template <int N> class SIFI_EXPORT SCalPar
{
private:
    float par[N]; ///< various parameters

public:
    virtual ~SCalPar() {}
    virtual uint read(const char* buffer);
    virtual uint write(char* buffer, size_t n) const;
    virtual void print(bool newline = true, const char* prefix = 0);
    float& operator[](int n);
    const float operator[](int n) const;
};

class SVirtualCalContainer
{
public:
    /**
     * Interface for accessing calibration container channel.
     * \return lookup channel base class
     */
    virtual SLookupChannel* createChannel() const = 0;

protected:
    /// Read data from container (parse ascii form)
    virtual void fromContainer() = 0;
    /// Write data to container (generate ascii form)
    virtual void toContainer() const = 0;

    /* Have access to fromContainer() and toContainer() to SParManager */
    friend void SParManager::writeContainers(const std::vector<std::string>& conts);
};

/**
 * This class stores calibration parameters. It uses obejct of base class
 * SLookupChannel as a key, and SCalPar as value for calibration. This is
 * because the lookup table maps physical address (subevtid, channel) into
 * virtual address (SCalContainer) and this virtual address is a key to
 * individual calibration parameters.
 */
template <int N> class SIFI_EXPORT SCalContainer : public SVirtualCalContainer
{
protected:
    std::string name;                      ///< container name
    std::map<size_t, SCalPar<N>*> calpars; ///< individual calibration parameters
    bool is_init;                          ///< is container init
    /// default channel value
    SCalPar<N>* def{nullptr}; //!
public:
    // constructor
    explicit SCalContainer(const std::string& container);
    virtual ~SCalContainer();

    /// return empty object of Lookup channel
    /// \sa SLookupTable::createChannel()
    /// \return empty lookup channel
    virtual SLookupChannel* createChannel() const { return new SLookupChannel; }

    SCalPar<N>* getPar(const SLookupChannel* channel);

    virtual void print();
    /// Set default value for the new channel
    /// \param d default value
    virtual void setDefault(SCalPar<N>* d) { def = d; }

protected:
    void fromContainer();
    void toContainer() const;
};

#endif /* SCALCONTAINER_H */
