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

class SDDSamples : public TObject
{
protected:
    // members
    Int_t module;       ///< address - module
    Int_t layer;        ///< address - layer
    Int_t fiber;        ///< address - fiber

    Float_t samples[1024];          ///< u-coord in the lab system

public:
    // constructor
    SDDSamples();

    // destructor
    ~SDDSamples();

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "");

    // methods
    /// Set address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void setAddress(Int_t m, Int_t l, Int_t f) { module = m; layer = l; fiber = f; }
    /// Get address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void getAddress(Int_t & m, Int_t & l, Int_t & f) const { m = module; l = layer; f = fiber; }

    /// Set coordinate along fibers
    /// \param samples samples
    /// \param length number of samples
    void fillSamples(Float_t * samples, size_t length);

    Float_t * getSamples() { return samples; }

    void print() const;

    ClassDef(SDDSamples, 1);   // container for fibers stack raw data
};

#endif /* SDDSAMPLES_H */
