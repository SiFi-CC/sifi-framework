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

    Float_t adc_l;      ///< adc value
    Float_t time_l;     ///< time value

    Float_t adc_r;      ///< adc value
    Float_t time_r;     ///< time value

public:
    // constructor
    SFibersStackRaw();

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
    /// \param a adc
    void setADC(Float_t al, Float_t ar) { adc_l = al; adc_r = ar; }
    /// Set Time value
    /// \param t time
    void setTime(Float_t tl, Float_t tr) { time_l = tl; time_r = tr; }
    /// Set ADC value
    /// \param a adc
    void setADCL(Float_t a) { adc_l = a; }
    /// Set Time value
    /// \param t time
    void setTimeL(Float_t t) { time_l = t; }
    /// Set ADC value
    /// \param a adc
    void setADCR(Float_t a) { adc_r = a; }
    /// Set Time value
    /// \param t time
    void setTimeR(Float_t t) { time_r = t; }

    /// Get cooridnate along fibers
    /// \return transverse coordinate
    Float_t getU() const { return u; }
    /// Get cooridnate along layers
    /// \return longnitudal coordinate
    Float_t getY() const { return y; }
    /// Get ADC value
    /// \return adc
    Float_t getADCL() const { return adc_l; }
    /// Get Time value
    /// \return time
    Float_t getTimeL() const { return time_l; }
    /// Get ADC value
    /// \return adc
    Float_t getADCR() const { return adc_r; }
    /// Get Time value
    /// \return time
    Float_t getTimeR() const { return time_r; }

    void print() const;

    ClassDef(SFibersStackRaw, 1);   // container for fibers stack raw data
};

#endif /* SFIBERSSTACKRAW_H */
