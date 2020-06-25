// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SDRSOURCE_H
#define SDRSOURCE_H

#include "sifi_export.h"
#include "SiFiConfig.h"

#include "SRootSource.h"

#include "DR_EventHandler.hh"

#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>
#include <TVector3.h>

#include <cstddef>
#include <string>
#include <fstream>

/* declare branches here and define in constructor */
struct TREE_Events {
    std::vector<TVector3> * fElectronPositions;
};

struct TREE_Address {
    // something here
    int m;
    int l;
    int f;
    char s;
};

struct TREE_all {
    TREE_Address address;
    TREE_Events events;
};

/**
 * Extends SDataSOurce to read data from Desktop Digitizer.
 */
class SIFI_EXPORT SDRSource : public SRootSource
{
public:
    explicit SDRSource(/*uint16_t subevent*/);

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;
    virtual void setInput(const std::string & filename, size_t length);

protected:
    TChain * chain2;
    TChain * chain3;
private:
    uint16_t subevent;          ///< subevent id
    std::string input;          ///< source file name
    std::ifstream istream;      ///< input file stream
    size_t buffer_size;         ///< data buffer size

    TREE_all tree;
};

#endif /* SDRSOURCE_H */
