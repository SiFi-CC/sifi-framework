// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SHLDSOURCE_H
#define SHLDSOURCE_H

#include "sifi_export.h"

#include "SDataSource.h"

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>

#include <cstddef>
#include <fstream>
#include <string>

//hadaq DABC libraries
#include "hadaq/api.h"
#include "dabc/Url.h"
#include "dabc/api.h"

struct HLDHit
{
    Int_t fiberID = -100;
    Double_t time_l = -100;
    Double_t time_r = -100;
    Double_t qdc_l = -100;
    Double_t qdc_r = -100;

    void print() const
    {
        printf("HLD: fiberID = %i, time_l = %f, time_r = %f, qdc_l = %f, qdc_r = %f\n", fiberID,
               time_l, time_r, qdc_l, qdc_r);
    }
};

/**
 * Extends SDataSource to read data from the PMI setup.
 */
class SIFI_EXPORT SHLDSource : public SDataSource
{
public:
    explicit SHLDSource(uint16_t subevent);

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;
    virtual void setInput(const std::string& filename, size_t length = 0);

private:
    uint16_t subevent;     ///< subevent id
    std::string input;     ///< source file name
    std::ifstream istream; ///< input file stream

    //ROOT HLD based
    Long64_t acqT0L, acqT0R; //acquisition T_0
    TTree*   tree; //FiberCoincidences
    Int_t GlobalFiberNumber;
    Int_t PhotonsRoiR;
    Int_t PhotonsRoiL;
    Long64_t TimeStampR; //ns
    Long64_t TimeStampL; //ns
};
#endif /* SHLDSOURCE_H */
