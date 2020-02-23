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

class SLookupContainer;
class SLookupTable;

class SLookupChannel
{
public:
    UInt_t m, l, s;
    virtual void setAddress(const char * address);
    virtual void print(const char * prefix = 0);
};

class SLookupBoard
{
private:
    UInt_t addr, nchan;
    SLookupChannel ** channels;

public:
    SLookupBoard(UInt_t addr, UInt_t nchan);
    virtual ~SLookupBoard();

    void setAddress(UInt_t chan, const char * line) { channels[chan]->setAddress(line); }
    void setChannel(UInt_t chan, SLookupChannel * c) { channels[chan] = c; }
    SLookupChannel * getChannel(UInt_t chan) { return channels[chan]; }

    virtual void print();
};

class SLookupTable
{
protected:
    std::string container;
    uint a_min, a_max, channels;
    bool is_init;

    SLookupBoard ** boards;

public:
    // constructor
    SLookupTable(const std::string & container, UInt_t addr_min, UInt_t addr_max, UInt_t channels = 49);
    // destructor
    virtual ~SLookupTable();

    virtual SLookupChannel * initial() = 0;

    SLookupChannel * getAddress(UInt_t addr, UInt_t chan) {
        if (!is_init) init();
        return boards[addr-a_min]->getChannel(chan);
    }

    virtual void print();

protected:
    void init();

};

#endif /* SLOOKUP_H */
