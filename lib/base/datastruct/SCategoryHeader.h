// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SCATEGORYHEADER_H
#define SCATEGORYHEADER_H

#include "sifi_export.h"

#include <TArrayI.h>
#include <TObject.h>
#include <TString.h>

/**
 * \class SCategoryHeader
\ingroup lib_base

A header object for the category

*/

class SCategoryHeader : public TObject
{
  public:
    // members
    TString name;        ///< name of the category
    Bool_t simulation;   ///< simulation category
    UInt_t dim;          ///< dimensions
    TArrayI dim_sizes;   ///< dimension sizes
    TArrayI dim_offsets; ///< dimension offsets
    ULong_t data_size;   ///< data array size
    Bool_t writable;     ///< is writable

  public:
    // constructor
    SCategoryHeader() : TObject() { clear(); }

    /// Clears all members
    void clear();

  private:
    ClassDefOverride(SCategoryHeader, 1);
};

#endif /* SCATEGORYHEADER_H */
