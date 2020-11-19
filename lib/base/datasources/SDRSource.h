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

#include "SiFiConfig.h"
#include "sifi_export.h"

#include "SRootSource.h"
#include "SFibersStackCalSim.h"

#include <ComptonCameraHitClass.hh>
#include <DR_EventHandler.hh>

#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>
#include <TVector3.h>

#include <cstddef>
#include <fstream>
#include <string>

class DRSiFiCCSetup;
class DRSiPMModel;

class TChain;

/* declare branches here and define in constructor */
struct TREE_Events
{
    TClonesArray* fHitArray;
};

struct TREE_Address
{
    int m;
    int l;
    int f;
    char s;
};

struct TREE_hit
{
    int counts;
    float time;
};

struct TREE_simdata {
    float x, y, z;
};

struct TREE_all {
    TREE_Address address;
    TREE_Events events;
    TREE_hit data;
    TVector3 pos;
    SFibersStackCalSim::InteractionType type;
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
    virtual void addInput(const std::string& filename) override;

  protected:
    TChain* chain2;
    TChain* chain3;

  private:
    uint16_t subevent; ///< subevent id

    std::map<int, int> fiber_map;
    std::map<int, TREE_Address> sipm_map;
    std::map<int,TVector3> * fPxPosPhot;
    std::map<int,TVector3> * fPxPosElec;
    std::map<int,TVector3> * fPxPosScin;
    
    TREE_all tree;
    DRSiFiCCSetup * ccsetup;
    DRSiPMModel * pmmodel;

};

#endif /* SDRSOURCE_H */
