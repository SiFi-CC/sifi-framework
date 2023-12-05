// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include <iostream>

#include "SDetectorGeomPar.h"
#include "SParContainer.h"

#include <cstdio> // for printf, putchar
#include <memory> // for allocator
#include <numeric> // for accumulate

/**
 * \class SDetectorGeomPar
\ingroup lib_fibers

A container for Fibers Stack geometry parameters //TODO rename

\sa SPar

*/

/**
 * Constructor
 */
SDetectorGeomPar::SDetectorGeomPar() : SPar(), mods(nullptr) {}

/**
 * Destructor
 */
SDetectorGeomPar::~SDetectorGeomPar() { clear(); }

/**
 * Clear parameters
 */
void SDetectorGeomPar::clear()
{
    delete[] mods;
    modules = 0;
}

/**
 * Get parameters
 *
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SDetectorGeomPar::getParams(SParContainer* parcont)
{
    if (!parcont->fill("nModules", modules)) return false;

    if (modules) delete[] mods;
    mods = new SingleModule[modules];

    // get module position
    TArrayF _mp;
    if (!parcont->fill("fModulePosition", _mp)) return false;
    if (_mp.GetSize() != (modules*3))
    {
        std::cerr << "Size of fModulePosition doesn't match nModules * 3" << std::endl;
        return false;
    }

    // get bottom board position
    TArrayF _bbp;
    if (!parcont->fill("fBottomBoardPosition", _bbp)) return false;
    if (_bbp.GetSize() != (modules*3))
    {
        std::cerr << "Size of fBottomBoardPosition doesn't match nModules * 3" << std::endl;
        return false;
    }

    // get top board position
    TArrayF _tbp;
    if (!parcont->fill("fTopBoardPosition", _tbp)) return false;
    if (_tbp.GetSize() != (modules*3))
    {
        std::cerr << "Size of fTopBoardPosition doesn't match nModules * 3" << std::endl;
        return false;
    }

    //get bottom board rotation angle
    TArrayF _bbr;
    if (!parcont->fill("fBottomBoardZRotationAngle", _bbr)) return false;
    if (_bbr.GetSize() != modules)
    {
        std::cerr << "Size of fBottomBoardZRotationAngle doesn't match nModules" << std::endl;
        return false;
    }

    //get top board rotation angle
    TArrayF _tbr;
    if (!parcont->fill("fTopBoardZRotationAngle", _tbr)) return false;
    if (_tbr.GetSize() != modules)
    {
        std::cerr << "Size of fTopBoardZRotationAngle doesn't match nModules" << std::endl;
        return false;
    }

    // get number of SiPM layers
    TArrayI _sl;
    if (!parcont->fill("nSiPMLayers", _sl)) return false;
    if (_sl.GetSize() != modules)
    {
        std::cerr << "Size of nSiPMLayers doesn't match nModules" << std::endl;
        return false;
    }

    // get number of fiber layers
    TArrayI _fl;
    if (!parcont->fill("nFiberLayers", _fl)) return false;
    if (_fl.GetSize() != modules)
    {
        std::cerr << "Size of nFiberLayers doesn't match nModules" << std::endl;
        return false;
    }

    Int_t n_SiPMLayers = _sl.GetSum();

    // get number of SiPMs in each layer
    TArrayI _s;
    if (!parcont->fill("nSiPMsPerLayer", _s)) return false;
    if (_s.GetSize() != (n_SiPMLayers))
    {
        std::cerr << "Size of nSiPMsPerLayer doesn't match the sum of nSiPMLayers" << std::endl;
        return false;
    }

    Int_t n_FiberLayers = _fl.GetSum();

    // get number of fibers in each layer
    TArrayI _f;
    if (!parcont->fill("nFibersPerLayer", _f)) return false;
    if (_f.GetSize() != (n_FiberLayers))
    {
        std::cerr << "Size of nFibersPerLayer doesn't match the sum of nFiberLayers" << std::endl;
        return false;
    }

    Int_t n_SiPMs = _s.GetSum() * 2; // two boards

    // get SiPM local positions: ID, x, y, z
    TArrayF _slp;
    if (!parcont->fill("fSiPMLocalPosition", _slp)) return false;
    if (_slp.GetSize() != (n_SiPMs*4))
    {
        std::cerr << "Size of fSiPMLocalPosition doesn't match the sum of n_SiPMs * 4" << std::endl;
        return false;
    }

    Int_t n_Fibers = _f.GetSum();

    // get fibers local positions: ID, x, y, z
    TArrayF _flp;
    if (!parcont->fill("fFiberLocalPosition", _flp)) return false;
    if (_flp.GetSize() != (n_Fibers*4))
    {
        std::cerr << "Size of fFiberLocalPosition doesn't match the sum of n_Fibers * 4" << std::endl;
        return false;
    }

    Int_t cnt_sipm_layers = 0;
    Int_t cnt_fiber_layers = 0;
    const Int_t pos_len = 3;
    for (Int_t m = 0; m < modules; ++m)
    {
        mods[m].modulePosition.SetXYZ(
            _mp[m * pos_len],
            _mp[m * pos_len + 1],
            _mp[m * pos_len + 2]);
        mods[m].bottomBoardPosition.SetXYZ(
            _bbp[m * pos_len],
            _bbp[m * pos_len + 1],
            _bbp[m * pos_len + 2]);
        mods[m].topBoardPosition.SetXYZ(
            _tbp[m * pos_len],
            _tbp[m * pos_len + 1],
            _tbp[m * pos_len + 2]);
        mods[m].bottomBoardZRotationAngle = _bbr[m];
        mods[m].topBoardZRotationAngle = _tbr[m];
        mods[m].numSiPMLayers = _sl[m];
        mods[m].numFiberLayers = _fl[m];
        mods[m].numSiPMsPerLayer.Set(mods[m].numSiPMLayers);
        mods[m].numFibersPerLayer.Set(mods[m].numFiberLayers);

        for (Int_t sl = 0; sl < _sl[m]; ++sl)
        {
            Int_t sipms = _s[cnt_sipm_layers * m + sl];
            mods[m].numSiPMsPerLayer[sl] = sipms;
        }

        for (Int_t fl = 0; fl < _fl[m]; ++fl)
        {
            Int_t fibers = _f[cnt_fiber_layers * m + fl];
            mods[m].numFibersPerLayer[fl] = fibers;
        }

        cnt_sipm_layers += _sl[m];
        cnt_fiber_layers += _fl[m];
    }

    const Int_t vec_len = 4;
    for (Int_t s = 0; s < n_SiPMs; ++s)
    {
        localPositionSiPM[s] = TVector3(
            _slp[s * vec_len + 1],
            _slp[s * vec_len + 2],
            _slp[s * vec_len + 3]);
    }

    for (Int_t f = 0; f < n_Fibers; ++f)
    {
        localPositionFiber[f] = TVector3(
            _flp[f * vec_len + 1],
            _flp[f * vec_len + 2],
            _flp[f * vec_len + 3]);
    }

    return true;
}

/**
 * Put parameters
 *
 * \sa SPar::putParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SDetectorGeomPar::putParams(SParContainer* /*parcont*/) const { return true; }

/**
 * Print parameters
 */
void SDetectorGeomPar::print() const
{
    printf("Number of modules = %d\n", modules);
    for (int m = 0; m < modules; ++m)
    {
        printf(" [%2d] module position: (%f, %f, %f)", m, mods[m].modulePosition.X(), mods[m].modulePosition.Y(), mods[m].modulePosition.Z());
        printf("\n [%2d] bottom board position: (%f, %f, %f)", m, mods[m].bottomBoardPosition.X(), mods[m].bottomBoardPosition.Y(), mods[m].bottomBoardPosition.Z());
        printf("\n [%2d] top board position: (%f, %f, %f)", m, mods[m].topBoardPosition.X(), mods[m].topBoardPosition.Y(), mods[m].topBoardPosition.Z());

        printf("\n number of SiPM layers = %d\n", mods[m].numSiPMLayers);
        printf(" number of SiPMs per layer:");
        for (int sl = 0; sl < mods[m].numSiPMLayers; ++sl)
            printf(" %2d", mods[m].numSiPMsPerLayer[sl]);
        printf("\n number of fiber layers = %d\n", mods[m].numFiberLayers);
        printf(" number of SiPMs per layer:");
        for (int fl = 0; fl < mods[m].numFiberLayers; ++fl)
            printf(" %2d", mods[m].numFibersPerLayer[fl]);
        putchar('\n');
    }
}

/**
 * Get module position
 *
 * \param m module
 * \return position (TVector3)
 */
TVector3 SDetectorGeomPar::getModulePosition(Int_t m) const
{
    if (mods and m < modules)
        return mods[m].modulePosition;
    else
    {
        std::cerr << "Could not obtain module position!" << std::endl;
        std::abort();
    }
}

/**
 * Get bottom board position
 *
 * \param m module
 * \return position
 */
TVector3 SDetectorGeomPar::getBottomBoardPosition(Int_t m) const
{
    if (mods and m < modules)
        return mods[m].bottomBoardPosition;
    else
    {
        std::cerr << "Could not obtain bottom board position!" << std::endl;
        std::abort();
    }
}

/**
 * Get top board position
 *
 * \param m module
 * \return position
 */
TVector3 SDetectorGeomPar::getTopBoardPosition(Int_t m) const
{
    if (mods and m < modules)
        return mods[m].topBoardPosition;
    else
    {
        std::cerr << "Could not obtain top board position!" << std::endl;
        std::abort();
    }
}

/**
 * Get rotation angle around the Z axis for the bottom board
 *
 * \param m module
 * \return rotation angle
 */
Float_t SDetectorGeomPar::getBottomBoardZRotationAngle(Int_t m) const
{
    if (mods and m < modules)
        return mods[m].bottomBoardZRotationAngle;
    else
    {
        std::cerr << "Could not obtain bottom board rotation angle!" << std::endl;
        std::abort();
    }
}

/**
 * Get rotation angle around the Z axis for the top board
 *
 * \param m module
 * \return rotation angle
 */
Float_t SDetectorGeomPar::getTopBoardZRotationAngle(Int_t m) const
{
    if (mods and m < modules)
        return mods[m].topBoardZRotationAngle;
    else
    {
        std::cerr << "Could not obtain top board rotation angle!" << std::endl;
        std::abort();
    }
}

/**
 * Get number of SiPM layers in the module
 *
 * \param m module
 * \return number of layers
 */
Int_t SDetectorGeomPar::getNumSiPMLayers(Int_t m) const
{
    if (mods and m < modules)
        return mods[m].numSiPMLayers;
    else
    {
        std::cerr << "Could not obtain top board rotation angle!" << std::endl;
        std::abort();
    }
}

/**
 * Get number of fiber layers in the module
 *
 * \param m module
 * \return number of layers
 */
Int_t SDetectorGeomPar::getNumFiberLayers(Int_t m) const
{
    if (mods and m < modules)
        return mods[m].numFiberLayers;
    else
    {
        std::cerr << "Could not obtain top board rotation angle!" << std::endl;
        std::abort();
    }
}

/**
 * Get number of SiPMs in each layer
 *
 * \param m module
 * \param l layer
 * \return number of fibers
 */
Int_t SDetectorGeomPar::getNumSiPMsPerLayer(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].numSiPMLayers)
        return mods[m].numSiPMsPerLayer[l];
    else
    {
        std::cerr << "Could not obtain the number of SiPMs per layer!" << std::endl;
        std::abort();
    }
}

/**
 * Get number of fibers in each layer
 *
 * \param m module
 * \param l layer
 * \return number of fibers
 */
Int_t SDetectorGeomPar::getNumFibersPerLayer(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].numFiberLayers)
        return mods[m].numFibersPerLayer[l];
    else
    {
        std::cerr << "Could not obtain the number of fibers per layer!" << std::endl;
        std::abort();
    }
}
//TODO should I add some checks if m, l, f are not greater than they should be (what would happen if I put layer = 40?)?
void SDetectorGeomPar::getswSiPMIDFromAddress(Int_t& id, Int_t m, Int_t l, Int_t f, char s) const
{
    id = 0;

    Int_t nBottomBoardSiPMs = 0; // all SiPMs in all bottom boards

    for (Int_t i = 0; i < modules; ++i)
    {
        nBottomBoardSiPMs += mods[i].numSiPMsPerLayer.GetSum();
    }

    for (Int_t i = 0; i < m; ++i) // loop over all SiPMs in the bottom boards of the modules smaller than address(module)
    {
        for (Int_t sl = 0; sl < mods[i].numSiPMLayers; ++sl)
        {
            id += mods[i].numSiPMsPerLayer[sl];
        }
    }

    for (Int_t sl = 0 ; sl < l ; ++sl)
    {
        id += mods[m].numSiPMsPerLayer[sl];
    }

    id += f;

    if (s == 'r') id += nBottomBoardSiPMs;
}

void SDetectorGeomPar::getswFiberIDFromAddress(Int_t& id, Int_t m, Int_t l, Int_t f) const
{
    id = 0;

    for (Int_t i = 0; i < m; ++i) // loop over all fibers in modules smaller than address(module)
    {
        for (Int_t fl = 0; fl < mods[i].numFiberLayers; ++fl)
        {
            id += mods[i].numFibersPerLayer[fl];
        }
    }


    for (Int_t fl = 0 ; fl < l ; ++fl)
    {
        id += mods[m].numFibersPerLayer[fl];
    }

    id += f;
}

void SDetectorGeomPar::getAddressFromswSiPMID(Int_t id, Int_t& m, Int_t& l, Int_t& f, char& s) const
{
    Int_t sipmOffset = 0; // all SiPMs in the bottom boards of the modules smaller than address(module) plus all SiPMs in all bottom boards if the swSiPMID is on a top board
    Int_t bottomSiPMs = 0;

    for (Int_t i = 0; i < modules; ++i)
    {
        bottomSiPMs += mods[i].numSiPMsPerLayer.GetSum();
    }

    if (id > bottomSiPMs)
    {
        sipmOffset += bottomSiPMs;
        s = 'r';
    } else {
        s = 'l';
    }

    for (Int_t i = 0; i < modules; ++i)
    {
        Int_t nSiPMsInModule = mods[i].numSiPMsPerLayer.GetSum();
        if (id < nSiPMsInModule + sipmOffset)
        {
            m = i;
            Int_t totalSiPMs = 0; // SiPMs scanned so far
            for (Int_t sl = 0; sl < mods[i].numFiberLayers; ++sl)
            {
                if (id - sipmOffset < totalSiPMs + mods[i].numSiPMsPerLayer[sl])
                {
                    l = sl;
                    f = id - sipmOffset - totalSiPMs;
                    break;
                }
                totalSiPMs += mods[i].numSiPMsPerLayer[sl];
            }
            break;
        }
        sipmOffset += nSiPMsInModule;
    }
}

void SDetectorGeomPar::getAddressFromswFiberID(Int_t id, Int_t& m, Int_t& l, Int_t& f) const
{
    Int_t fiberOffset = 0; // all fibers in the modules smaller than address(module)

    for (Int_t i = 0; i < modules; ++i)
    {
        Int_t nFibersInModule = mods[i].numFibersPerLayer.GetSum();
        if (id < nFibersInModule + fiberOffset)
        {
            m = i;
            Int_t totalFibers = 0; // fibers scanned so far
            for (Int_t fl = 0; fl < mods[i].numFiberLayers; ++fl)
            {
                if (id - fiberOffset < totalFibers + mods[i].numFibersPerLayer[fl])
                {
                    l = fl;
                    f = id - fiberOffset - totalFibers;
                    break;
                }
                totalFibers += mods[i].numFibersPerLayer[fl];
            }
            break;
        }
        fiberOffset += nFibersInModule;
    }
}

TVector3 SDetectorGeomPar::getLocalSiPMPosition (Int_t id) const
{
    TVector3 localPosition;

    auto it = localPositionSiPM.find(id);

    if (it != localPositionSiPM.end())
    {
        localPosition.SetXYZ(it->second.X(), it->second.Y(), it->second.Z());
    } else {
        std::cerr << "Could not obtain swSiPMID = " << id << "!" << std::endl;
        std::abort();
    }

    return localPosition;
}

TVector3 SDetectorGeomPar::getLocalFiberPosition(Int_t id) const
{
    TVector3 localPosition;

    auto it = localPositionFiber.find(id);

    if (it != localPositionFiber.end())
    {
        localPosition.SetXYZ(it->second.X(), it->second.Y(), it->second.Z());
    } else {
        std::cerr << "Could not obtain swFiberID = " << id << "!" << std::endl;
        std::abort();
    }

    return localPosition;
}

TVector3 SDetectorGeomPar::getGlobalSiPMPosition(Int_t id) const
{
    // SiPM address
    Int_t m;
    Int_t l;
    Int_t f; //TODO rename? it's not a fiber but a sipm (in SFibersLookup there is something like element)
    char s;

    getAddressFromswSiPMID(id, m, l, f, s);

    TVector3 localPosition = getLocalSiPMPosition(id);

    if (s == 'l')
    {
        localPosition.RotateZ(mods[m].bottomBoardZRotationAngle);
        return localPosition + mods[m].bottomBoardPosition;
    } else {
        localPosition.RotateZ(mods[m].topBoardZRotationAngle);
        return localPosition + mods[m].topBoardPosition;
    }
}

TVector3 SDetectorGeomPar::getGlobalSiPMPosition(Int_t m, Int_t l, Int_t f, char s) const
{
    Int_t id;

    getswSiPMIDFromAddress(id, m, l, f, s);

    TVector3 localPosition = getLocalSiPMPosition(id);

    if (s == 'l')
    {
        localPosition.RotateZ(mods[m].bottomBoardZRotationAngle);
        return localPosition + mods[m].bottomBoardPosition;
    } else {
        localPosition.RotateZ(mods[m].topBoardZRotationAngle);
        return localPosition + mods[m].topBoardPosition;
    }
}

TVector3 SDetectorGeomPar::getGlobalFiberPosition(Int_t id) const
{
    // fiber address
    Int_t m;
    Int_t l;
    Int_t f;

    getAddressFromswFiberID(id, m, l, f);

    TVector3 localPosition = getLocalFiberPosition(id);

    return localPosition + mods[m].modulePosition;
}

TVector3 SDetectorGeomPar::getGlobalFiberPosition(Int_t m, Int_t l, Int_t f) const
{
    Int_t id;

    getswFiberIDFromAddress(id, m, l, f);

    TVector3 localPosition = getLocalFiberPosition(id);

    return localPosition + mods[m].modulePosition;
}
