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

#include "sifi_export.h"

#include <Rtypes.h>     // for THashConsistencyHolder, ClassDef
#include <RtypesCore.h> // for Float_t, Int_t, Option_t
#include <TObject.h>

#include <cstddef> // for size_t

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * Set of functions used in the analysis by Katarzyna Rusiecka. Imported here
 * for compatiblity. Most likely will be deprecated in teh future.
 */
class SIFI_EXPORT SDDSignal
{
public:
    Float_t fAmp;      ///< Amplitude [mV]
    Float_t fT0;       ///< Time T0 [ns]
    Float_t fTOT;      ///< Time over threshold [ns]
    Float_t fCharge;   ///< Uncalibrated charge [a.u.]
    Float_t fPE;       ///< Calibrated charge [P.E.] or [keV]
    Float_t fBL;       ///< extracted base line
    Float_t fBL_sigma; ///< sigma of the baseline
    Int_t fPileUp;     ///< pile up flag
    Int_t fVeto;       ///< veto flag

public:
    SDDSignal();

    /// Sets signal amplitude in mV
    /// \param amp amplitude of the signal
    void SetAmplitude(Float_t amp) { fAmp = amp; };
    /// Sets time T0 in ns
    /// \param t0 start time
    void SetT0(Float_t t0) { fT0 = t0; }
    /// Sets time over threshold in mV
    /// \param tot time over threshold
    void SetTOT(Float_t tot) { fTOT = tot; };
    /// Sets uncalibrated charge, i.e. signal integral
    /// \param charge integrated charge
    void SetCharge(Float_t charge) { fCharge = charge; };
    /// Sets value of charge calibrated to PE or keV
    /// \param  cal charge
    void SetPE(Float_t cal) { fPE = cal; };
    /// Sets baseline value
    /// \param bl baseline
    void SetBL(Float_t bl) { fBL = bl; };
    /// Sets baselibe sigma value
    /// \param bl_sig baseline sigma
    void SetBLSigma(Float_t bl_sig) { fBL_sigma = bl_sig; };
    /// Sets pile up flag
    /// \param pileup pile up flag
    void SetPileUp(Int_t pileup) { fPileUp = pileup; };
    /// Sets veto flag
    /// \param veto veto flag
    void SetVeto(Int_t veto) { fVeto = veto; };

    /// Returns signal amplitude in mV
    /// \return amplitude of the signal
    Float_t GetAmplitude() const { return fAmp; };
    /// Returns time T0 in ns
    /// \return start time
    Float_t GetT0() const { return fT0; };
    /// Retursns time over threshold in ns
    /// \return time over threshold
    Float_t GetTOT() const { return fTOT; };
    /// Returns uncalibrated charge, i.e. signal integral
    /// \return integrated charge
    Float_t GetCharge() const { return fCharge; };
    /// Returns value of calibrated charge in PE or keV
    /// \return calibrated charge
    Float_t GetPE() const { return fPE; };
    /// Returns baseline value
    /// \return baseline
    Float_t GetBL() const { return fBL; };
    /// Returns baseline sigma
    /// \return baseline sigma
    Float_t GetBLSigma() const { return fBL_sigma; };
    /// Returns pile up flag
    /// \return pile up flag
    Int_t GetPileUp() const { return fPileUp; };
    /// Returns veto flag
    /// \return veto flaf
    Int_t GetVeto() const { return fVeto; };

    void Clear(void);
    void Print(void) const;
};

/**
 * Stores samples for each event. It practicly copies teh DD fiels into the tree
 * producing huge output. In the future using of this class will be deprecated.
 */
class SIFI_EXPORT SDDSamples : public TObject
{
protected:
    // members
    Int_t module; ///< address - module
    Int_t layer;  ///< address - layer
    Int_t fiber;  ///< address - fiber

    Float_t samples_l[1024]; ///< samples read on ou the left side
    Float_t samples_r[1024]; ///< samples read on ou the right side

    SDDSignal signal_l; ///< signal from the left side, backward comp.
    SDDSignal signal_r; ///< signal from the right side, backward comp.

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

    /// Fill samples for the left side
    /// \param samples samples
    /// \param length number of samples
    void fillSamplesL(Float_t* samples, size_t length);
    /// Fill samples for the right side
    /// \param samples samples
    /// \param length number of sample
    void fillSamplesR(Float_t* samples, size_t length);

    /// Get samples from the left side
    /// \return samples
    Float_t* getSamplesL() { return samples_l; }
    /// Get samples from the right side
    /// \return samples
    Float_t* getSamplesR() { return samples_r; }
    /// Get signal from the left side
    /// \return signal data
    SDDSignal* getSignalL() { return &signal_l; }
    /// Get signal from the right side
    /// \return signal data
    SDDSignal* getSignalR() { return &signal_r; }

    void print() const;

    ClassDef(SDDSamples, 1); // container for fibers stack raw data
};

#endif /* SDDSAMPLES_H */
