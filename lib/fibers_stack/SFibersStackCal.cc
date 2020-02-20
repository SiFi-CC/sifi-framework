// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackCal.h"

/** \class SFibersStackCal
\ingroup lib_fibers_stack

A container for Fibers Stack Calibrated data

*/

/** Default constructor
 */
SFibersStackCal::SFibersStackCal() : TObject()
{
    Clear();
}

/** Default destructor
 */
SFibersStackCal::~SFibersStackCal()
{

}

/** Clear object.
 * Parameter options are ignored, for ROOT compatibility.
 * \param opt options
 */
void SFibersStackCal::Clear(Option_t * opt)
{
    module = -1;
    layer = -1;
    fiber = -1;

    u = 0;
    y = 0;
    e_dep = 0.0;
}

/** Print object.
 */
void SFibersStackCal::print() const
{
    printf("fiber m,l,f=%d,%d,%d  u,y=%f,%f  Eloss=%f\n", module, layer, fiber, u, y, e_dep);
}

ClassImp(SFibersStackCal);
