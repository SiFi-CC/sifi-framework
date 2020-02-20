// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKRAW_H
#define SFIBERSSTACKRAW_H

#include <TObject.h>

class SFibersStackRaw : public TObject
{
protected:
    // members
    Int_t module;       ///< address - module
    Int_t layer;        ///< address - layer
    Int_t fiber;        ///< address - fiber

    Float_t u;          ///< u-coord in the lab system
    Float_t y;          ///< y-coord in the lab system

    Float_t adc;        ///< adc value

public:
    // constructor
    SFibersStackRaw();

    // destructor
    ~SFibersStackRaw();

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
    /// \param _u transverse coordinate
    void setU(Float_t _u) { u = _u; }
    /// Set coordinate along layers
    /// \param _y longnitudal coordinate
    void setY(Float_t _y) { y = _y; }
    /// Set ADC value
    /// \param e adc
    void setADC(Float_t e) { adc = e; }

    /// Get cooridnate along fibers
    /// \return transverse coordinate
    Float_t getU() const { return u; }
    /// Get cooridnate along layers
    /// \return longnitudal coordinate
    Float_t getY() const { return y; }
    /// Get ADC value
    /// \return adc
    Float_t getADC() const { return adc; }

    void print() const;

    ClassDef(SFibersStackRaw, 1);   // container for fibers stack raw data
};

#endif /* SFIBERSSTACKRAW_H */
