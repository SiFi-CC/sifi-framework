// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SLOOKUP_H
#define SLOOKUP_H

#include "sifi_export.h"

#include "SParManager.h"

#include <Rtypes.h>

#include <cassert>
#include <cstdint>

class SLookupTable;

/**
 * Describes basic mapped address structure in the framework. Consist of three
 * address components:
 *  * #m - module
 *  * #l - layer
 *  * #s - strip.
 *
 * This structure must provie interface to read and write mapping to a
 * container. Each deriving class must also provide proper interface.
 *  * read() - reads lookup data from container
 *  * write() - writes lookup data to the container
 *  * print() - print lookup values of the lookup table
 *  * quickHash() - generates hash from the address
 *  * fromHash() - decodes the hash into address
 */
struct SIFI_EXPORT SLookupChannel
{
    virtual ~SLookupChannel() = default;

    ///@{
    uint8_t m, l, s;    ///< component of virtual address
    ///}@

    virtual uint read(const char * buffer);
    virtual uint write(char * buffer, size_t n) const;
    virtual void print(bool newline = true, const char * prefix = 0) const;
    virtual uint64_t quickHash() const;
    virtual void fromHash(uint64_t hash);
};

/**
 * Each address in the lookup table has corresponding object of SLookupBoard.
 * The object is quite generic thus do not need to be subclass in the detectors.
 * Two functions setChannel() and getChannel() provide actuall functionality
 * required by the user.
 */
class SLookupBoard
{
private:
    uint addr;                      ///< board address
    uint nchan;                     ///< number of channels
    SLookupChannel ** channels;     ///< array of channels

public:
    SLookupBoard(uint addr, uint nchan);
    SLookupBoard(const SLookupBoard &) = delete;
    SLookupBoard & operator=(const SLookupBoard &) = delete;

    virtual ~SLookupBoard();

    /**
     * Set channel object at given channel location. If #chan is larger than
     * number of channels, aborts the execution.
     *
     * \param chan channel number
     * \param c channel mapping object
     */
    void setChannel(uint chan, SLookupChannel * c) {
        assert(chan < nchan);
        channels[chan] = c;
    }

    /**
     * Fetch the channel mapping object for given channel number. If channel
     * number is outside the allwoed range, the executions aborts.
     *
     * \param chan channel number
     * \return channel mapping object
     */
    SLookupChannel * getChannel(uint chan) {
        assert(chan < nchan);
        return channels[chan];
    }

    virtual void print();
};

/**
 * The lookup table maps physical front-end board address and channels to
 * abstract addressing used in the framework. The mapping contains then two
 * parts:
 * * mapped addresses (2 components)
 * * mapping (variable number of values).
 *
 * The default implementation provides three mapping addreses: module, layer,
 * channel. The respective entry in the lookup table looks following:
 *
 *     addres channel        module layer channel
 *
 * where:
 *  * `address` - a hex value of board address (in te unpacker it is equivalent
 *    to subevent id.
 *  * `channel` - physical channel on the board
 *  * `module` - abstract module value
 *  * `layer` - abstract layer value
 *  * `channel` - abstract channel value
 *
 * Respective detectors can use this scheme or provide it's own
 * mapping. The virtual mapping for each detector/system is an individual
 * issue, but at least one dimension of mapping must exists.
 *
 * While parsing the lookup table, the parser creates object of SLookupBoard for
 * each unique address appearing in the table, which fit the address range
 * defiend by #a_min, #a_max.
 * See documentation of existing detectors in the framework for details.
 */
class SIFI_EXPORT SLookupTable
{
protected:
    std::string container;          ///< container name
    uint a_min;                     ///< lower boundary of address range
    uint a_max;                     ///< upper boundary of address range
    uint channels;                  ///< maximal number of channels
    bool is_init;                   ///< set if container was initialized

    SLookupBoard ** boards;         ///< array of boards in a given range

public:
    SLookupTable(const std::string & container, uint addr_min, uint addr_max, uint channels = 49);
    /// Do not allow for copying the lookup table
    SLookupTable(const SLookupTable &) = delete;
    SLookupTable & operator=(const SLookupTable &) = delete;

    // destructor
    virtual ~SLookupTable();

    /**
     * To simplify mechanism of lookup table and remove need to sublass
     * SLookupBoard, we need mechanism to provide empty object of Lookup
     * Channel. This function should return a empty opbject of desired
     * Channel lookup. When need to have custom version of SLookupChannel, the
     * SLookupTable must be also subclass and the proper version of this
     * function provided.
     *
     * \return empty object of channel lookup class
     */
    virtual SLookupChannel * createChannel() const { return new SLookupChannel; }

    SLookupChannel * getAddress(uint addr, uint chan);

    virtual void print();

protected:
    void fromContainer();
    void toContainer() const;

    /* Have access to fromContainer() and toContainer() to SParManager */
    friend void SParManager::writeContainers(const std::vector<std::string> & conts) const;
};

#endif /* SLOOKUP_H */
