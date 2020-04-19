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

#include <iostream>

/** \class SDDSamples
\ingroup lib_fibers_stack

A container for Fibers Stack Raw data

*/

/** Constructor
 */
SDDSamples::SDDSamples() : TObject() { Clear(); }

/** Destructor
 */
SDDSamples::~SDDSamples() {}

/** Clear object
 * Parameter options are ignored, for ROOT compatibility.
 * \param opt options
 */
void SDDSamples::Clear(Option_t* opt) {
    module = -1;
    layer = -1;
    fiber = -1;

    memset(samples, 0, 1024 * sizeof(float));
    signal.Clear();
}

void SDDSamples::fillSamples(float* samples, size_t length) {
    size_t limit = length <= 1024 ? length : 1024;
    memcpy(this->samples, samples, limit * sizeof(float));
}

/** Print category
 */
void SDDSamples::print() const {
    printf("fiber m,l,f=%d,%d,%d\n", module, layer, fiber);
    printf("samples:");
    for (int i = 0; i < 1024; ++i)
        printf(" %f", samples[i]);
    printf("\n");
}

//------------------------------------------------------------------
/// Default constructor.
DDSignal::DDSignal()
    : fAmp(-100), fT0(-100), fTOT(-100), fCharge(-100), fPE(-100), fBL(-100.),
      fBL_sigma(0), fPileUp(0) {}

//------------------------------------------------------------------
/// Sets all values of signals parameters to their default values.
void DDSignal::Clear(void) {
    fAmp = -100.;
    fT0 = -100.;
    fTOT = -100.;
    fCharge = -100.;
    fPE = -100.;
    fBL = -100;
    fBL_sigma = -100;
    fPileUp = -100;
}
//------------------------------------------------------------------
/// Prints details of the DDSignal class object.
void DDSignal::Print(void) {
    std::cout << "\n\n------------------------------------------------"
              << std::endl;
    std::cout << "This is Print() for DDSignalBase class object" << std::endl;
    std::cout << "Amplitude = " << fAmp << " mV" << std::endl;
    std::cout << "T0 = " << fT0 << " ns" << std::endl;
    std::cout << "Time over threshold = " << fTOT << " ns" << std::endl;
    std::cout << "Charge (signal integral) = " << fCharge << std::endl;
    std::cout << "Calibrated charge = " << fPE << " P.E. / keV" << std::endl;
    std::cout << "Baseline = " << fBL << " +- " << fBL_sigma << " keV"
              << std::endl;
    std::cout << "PileUp = " << fPileUp << "" << std::endl;
    std::cout << "------------------------------------------------\n"
              << std::endl;
    return;
}
//------------------------------------------------------------------
