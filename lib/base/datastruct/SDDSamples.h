// @(#)lib/base/datastruct:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SDDSAMPLES_H
#define SDDSAMPLES_H

#include <TObject.h>

class DDSignal
{
  public:
    Float_t fAmp;    ///< Amplitude [mV]
    Float_t fT0;     ///< Time T0 [ns]
    Float_t fTOT;    ///< Time over threshold [ns]
    Float_t fCharge; ///< Uncalibrated charge [a.u.]
    Float_t fPE;     ///< Calibrated charge [P.E.] or [keV]
    Float_t fBL;
    Float_t fBL_sigma;
    Int_t fPileUp;

  public:
    DDSignal();

    /// Sets signal amplitude in mV
    void SetAmplitude(Float_t amp) { fAmp = amp; };
    /// Sets time T0 in ns
    void SetT0(Float_t t0) { fT0 = t0; }
    /// Sets time over threshold in mV
    void SetTOT(Float_t tot) { fTOT = tot; };
    /// Sets uncalibrated charge, i.e. signal integral
    void SetCharge(Float_t charge) { fCharge = charge; };
    /// Sets value of charge calibrated to PE or keV
    void SetPE(Float_t cal) { fPE = cal; };
    /// Returns signal amplitude in mV
    Float_t GetAmplitude(void) const { return fAmp; };
    /// Returns time T0 in ns
    Float_t GetT0(void) const { return fT0; };
    /// Retursns time over threshold in ns
    Float_t GetTOT(void) const { return fTOT; };
    /// Returns uncalibrated charge, i.e. signal integral
    Float_t GetCharge(void) const { return fCharge; };
    /// Returns value of calibrated charge in PE or keV
    Float_t GetPE(void) const { return fPE; };

    void Clear(void);
    void Print(void) const;
};

class SDDSamples : public TObject
{
  protected:
    // members
    Int_t module; ///< address - module
    Int_t layer;  ///< address - layer
    Int_t fiber;  ///< address - fiber

    Float_t samples_l[1024];
    Float_t samples_r[1024];

    DDSignal signal_l;
    DDSignal signal_r;

  public:
    // constructor
    SDDSamples();

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "");

    // methods
    /// Set address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void setAddress(Int_t m, Int_t l, Int_t f)
    {
        module = m;
        layer = l;
        fiber = f;
    }
    /// Get address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void getAddress(Int_t& m, Int_t& l, Int_t& f) const
    {
        m = module;
        l = layer;
        f = fiber;
    }

    /// Set coordinate along fibers
    /// \param samples samples
    /// \param length number of samples
    void fillSamplesL(Float_t* samples_l, size_t length);
    void fillSamplesR(Float_t* samples_r, size_t length);

    Float_t* getSamplesL() { return samples_l; }
    Float_t* getSamplesR() { return samples_r; }
    DDSignal* getSignalL() { return &signal_l; }
    DDSignal* getSignalR() { return &signal_r; }

    void print() const;

    ClassDef(SDDSamples, 1); // container for fibers stack raw data
};

#endif /* SDDSAMPLES_H */
