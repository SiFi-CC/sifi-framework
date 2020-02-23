// @(#)lib/base/datastruct:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SDDSamples.h"

/** \class SDDSamples
\ingroup lib_fibers_stack

A container for Fibers Stack Raw data

*/

/** Constructor
 */
SDDSamples::SDDSamples() : TObject()
{
    Clear();
}

/** Destructor
 */
SDDSamples::~SDDSamples()
{

}

/** Clear object
 * Parameter options are ignored, for ROOT compatibility.
 * \param opt options
 */
void SDDSamples::Clear(Option_t* opt)
{
    module = -1;
    layer = -1;
    fiber = -1;

    memset(samples, 0, 1024 * sizeof(float));
}

void SDDSamples::fillSamples(float* samples, size_t length)
{
    size_t limit = length <= 1024 ? length : 1024;
    memcpy(this->samples, samples, limit*sizeof(float));
}

/** Print category
 */
void SDDSamples::print() const
{
    printf("fiber m,l,f=%d,%d,%d\n", module, layer, fiber);
    printf("samples:");
    for (int i = 0; i < 1024; ++i)
        printf(" %f", samples[i]);
    printf("\n");
}
