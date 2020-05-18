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

#include "sifi_export.h"

#include <TObject.h>

class SIFI_EXPORT SFibersStackRaw : public TObject
{
protected:
    // members
    Int_t module;       ///< address - module
    Int_t layer;        ///< address - layer
    Int_t fiber;        ///< address - fiber

    Float_t u;          ///< u-coord in the lab system
    Float_t y;          ///< y-coord in the lab system

    Float_t qdc_l;      ///< qdc value
    Float_t time_l;     ///< time value

    Float_t qdc_r;      ///< qdc value
    Float_t time_r;     ///< time value

public:
    // constructor
    SFibersStackRaw();
    virtual ~SFibersStackRaw() = default;

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
    /// \param ql qdc on the left side of the fiber
    /// \param qr qdc on the right side of teh fiber
    void setQDC(Float_t ql, Float_t qr) { qdc_l = ql; qdc_r = qr; }
    /// Set Time value
    /// \param tl time on the left side of fiber
    /// \param tr time on the right side of fiber
    void setTime(Float_t tl, Float_t tr) { time_l = tl; time_r = tr; }
    /// Set QDC value
    /// \param a adc
    void setQDCL(Float_t q) { qdc_l = q; }
    /// Set Time value
    /// \param t time
    void setTimeL(Float_t t) { time_l = t; }
    /// Set QDC value
    /// \param a adc
    void setQDCR(Float_t q) { qdc_r = q; }
    /// Set Time value
    /// \param t time
    void setTimeR(Float_t t) { time_r = t; }

    /// Get cooridnate along fibers
    /// \return transverse coordinate
    Float_t getU() const { return u; }
    /// Get cooridnate along layers
    /// \return longnitudal coordinate
    Float_t getY() const { return y; }
    /// Get QDC value
    /// \return qdc
    Float_t getQDCL() const { return qdc_l; }
    /// Get Time value
    /// \return time
    Float_t getTimeL() const { return time_l; }
    /// Get QDC value
    /// \return qdc
    Float_t getQDCR() const { return qdc_r; }
    /// Get Time value
    /// \return time
    Float_t getTimeR() const { return time_r; }

    void print() const;

    ClassDef(SFibersStackRaw, 1);   // container for fibers stack raw data
};

#endif /* SFIBERSSTACKRAW_H */
