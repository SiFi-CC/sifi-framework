// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include <iostream>

#include <TBuffer.h>
#include <TClass.h>
#include <TClonesArray.h>

#include "SCategoryHeader.h"

void SCategoryHeader::clear()
{
    name.Clear();
    simulation = kFALSE;
    dim = 0;
    dim_sizes.Set(0);
    dim_offsets.Set(0);
    data_size = 0;
    writable = kFALSE;
}

void SCategoryHeader::Streamer(TBuffer &R__b)
{
    // Stream an object of class HLinearCategory.
    Char_t clase[200];
    if (R__b.IsReading())
    {
        TObject::Streamer(R__b);
        R__b >> name;
        R__b >> simulation;
        R__b >> dim;
        Int_t a, b;
        dim_sizes.Set(dim);
        dim_offsets.Set(dim);
        for (int i = 0; i < dim; ++i)
        {
            R__b >> a >> b;
            dim_sizes[i] = a;
            dim_offsets[i] = b;
        }
        R__b >> data_size;
        R__b >> writable;
    } else {
        TObject::Streamer(R__b);
        R__b << name;
        R__b << simulation;
        R__b << dim;
        Int_t a, b;
        for (int i = 0; i < dim; ++i)
        {
            a = dim_sizes[i];
            b = dim_offsets[i];
            R__b << a << b;
        }
        R__b << data_size;
        R__b << writable;
    }
}
