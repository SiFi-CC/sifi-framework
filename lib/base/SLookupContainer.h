// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SLOOKUPCONTAINER_H
#define SLOOKUPCONTAINER_H

#include <TArrayI.h>
#include <TArrayF.h>
#include <TArrayD.h>

#include <vector>
#include <map>

typedef std::vector<std::string> LookupVector;

class SLookupContainer
{
protected:
    // members
    std::string container;      ///< container name
    LookupVector lv;

public:
    // constructor
    SLookupContainer(const std::string & container);
    // destructor
    virtual ~SLookupContainer() {}

    void addLine(const std::string & line) { lv.push_back(line); }
    const LookupVector & getLines() const { return lv; }

    void print();
};

#endif /* SLOOKUPCONTAINER_H */
