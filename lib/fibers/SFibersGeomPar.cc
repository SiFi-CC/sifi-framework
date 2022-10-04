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

#include "SFibersGeomPar.h"
#include "SParContainer.h"

#include <cstdio> // for printf, putchar
#include <memory> // for allocator

/**
 * \class SFibersGeomPar
\ingroup lib_fibers

A container for Fibers Stack geometry parameters

\sa SPar

*/

/**
 * Constructor
 */
SFibersGeomPar::SFibersGeomPar() : SPar(), mods(nullptr) {}

/**
 * Destructor
 */
SFibersGeomPar::~SFibersGeomPar() { clear(); }

/**
 * Clear parameters
 */
void SFibersGeomPar::clear()
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
bool SFibersGeomPar::getParams(SParContainer* parcont)
{
    if (!parcont->fill("fType", type)) return false;

    if (!parcont->fill("nModules", modules)) return false;

    if (modules) delete[] mods;
    mods = new SingleModule[modules];

    // get module z
    TArrayF _mz;
    if (!parcont->fill("fModuleZ", _mz)) return false;
    if (_mz.GetSize() != (modules))
    {
        std::cerr << "Size of fModuleZ doesn't match nModules" << std::endl;
        return false;
    }

    // get layers
    TArrayI _l;
    if (!parcont->fill("nLayers", _l)) return false;
    if (_l.GetSize() != modules)
    {
        std::cerr << "Size of nLayers doesn't match nModules" << std::endl;
        return false;
    }

    int n_layers = _l.GetSum();

    // get fibers
    TArrayI _f;
    if (!parcont->fill("nFibers", _f)) return false;
    if (_f.GetSize() != (n_layers))
    {
        std::cerr << "Size of nFibers doesn't match nModules*nLayers" << std::endl;
        return false;
    }

    TArrayF _lr;
    if (!parcont->fill("fLayerRotation", _lr)) return false;
    if (_lr.GetSize() != (n_layers))
    {
        std::cerr << "Size of fLayerRotation doesn't match nModules" << std::endl;
        return false;
    }

    TArrayF _fox;
    if (!parcont->fill("fFiberOffsetX", _fox)) return false;
    if (_fox.GetSize() != (n_layers))
    {
        std::cerr << "Size of fFiberOffsetX doesn't match nModules" << std::endl;
        return false;
    }

    TArrayF _foy;
    if (!parcont->fill("fFiberOffsetY", _foy)) return false;
    if (_foy.GetSize() != (n_layers))
    {
        std::cerr << "Size of fFiberOffsetY doesn't match nModules" << std::endl;
        return false;
    }

    TArrayF _foz;
    if (!parcont->fill("fFiberOffsetZ", _foz)) return false;
    if (_foz.GetSize() != (n_layers))
    {
        std::cerr << "Size of fFiberOffsetZ doesn't match nModules" << std::endl;
        return false;
    }

    TArrayF _fsp;
    if (!parcont->fill("fFibersPitch", _fsp)) return false;
    if (_fsp.GetSize() != (n_layers))
    {
        std::cerr << "Size of fFibersPitch doesn't match nModules" << std::endl;
        return false;
    }

    int cnt_layers = 0;
    for (int m = 0; m < modules; ++m)
    {
        mods[m].module_z = _mz[m];
        mods[m].layers = _l[m];
        mods[m].fibers.Set(mods[m].layers);
        mods[m].layer_rotation.Set(mods[m].layers);
        mods[m].fiber_offset_x.Set(mods[m].layers);
        mods[m].fiber_offset_y.Set(mods[m].layers);
        mods[m].fiber_offset_z.Set(mods[m].layers);
        mods[m].fibers_pitch.Set(mods[m].layers);

        for (int l = 0; l < _l[m]; ++l)
        {
            Int_t fibers = _f[cnt_layers * m + l];
            Int_t layrot = _lr[cnt_layers * m + l];
            Float_t fox = _fox[cnt_layers * m + l];
            Float_t foy = _foy[cnt_layers * m + l];
            Float_t foz = _foz[cnt_layers * m + l];
            Float_t fsp = _fsp[cnt_layers * m + l];

            mods[m].fibers[l] = fibers;
            mods[m].layer_rotation[l] = layrot;
            mods[m].fiber_offset_x[l] = fox;
            mods[m].fiber_offset_y[l] = foy;
            mods[m].fiber_offset_z[l] = foz;
            mods[m].fibers_pitch[l] = fsp;
        }
        cnt_layers += _l[m];
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
bool SFibersGeomPar::putParams(SParContainer* /*parcont*/) const { return true; }

/**
 * Print parameters
 */
void SFibersGeomPar::print() const
{
    printf("Geometry type: %d   Number of modules = %d\n", type, modules);
    for (int m = 0; m < modules; ++m)
    {
        printf(" [%2d] module z: %f", m, mods[m].module_z);
        printf(" +++\n layers = %d\n", mods[m].layers);
        printf(" fibers:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %2d", mods[m].fibers[l]);
        printf("\n layrot:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %#.2f", mods[m].layer_rotation[l]);
        printf("\n  off x:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %#.2f", mods[m].fiber_offset_x[l]);
        printf("\n  off y:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %#.2f", mods[m].fiber_offset_y[l]);
        printf("\n  off z:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %#.2f", mods[m].fiber_offset_z[l]);
        printf("\n  pitch:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %#.2f", mods[m].fibers_pitch[l]);
        putchar('\n');
    }
}

/**
 * Get module z
 *
 * \param m module
 * \return z-coordinate
 */
Float_t SFibersGeomPar::getModuleZ(Int_t m) const
{
    if (mods and m < modules)
        return mods[m].module_z;
    else
        return -1;
}

/**
 * Get number of layers in the module
 *
 * \param m module
 * \return number of layers
 */
Int_t SFibersGeomPar::getLayers(Int_t m) const
{
    if (mods and m < modules)
        return mods[m].layers;
    else
        return -1;
}

/**
 * Get number of fibers in the layer
 *
 * \param m module
 * \param l layer
 * \return number of fibers
 */
Int_t SFibersGeomPar::getFibers(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].fibers[l];
    else
        return -1;
}

/**
 * Get layer rotation
 *
 * \param m module
 * \param l layer
 * \return layer rotation
 */
Float_t SFibersGeomPar::getLayerRotation(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].layer_rotation[l];
    else
        return -10000.;
}

/**
 * Get fibers offset X
 *
 * \param m module
 * \param l layer
 * \return offset X
 */
Float_t SFibersGeomPar::getFiberOffsetX(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].fiber_offset_x[l];
    else
        return -10000.;
}

/**
 * Get layers offset Y
 *
 * \param m module
 * \param l layer
 * \return offset Y
 */
Float_t SFibersGeomPar::getFiberOffsetY(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].fiber_offset_y[l];
    else
        return -10000.;
}

/**
 * Get layers offset Z
 *
 * \param m module
 * \param l layer
 * \return offset Z
 */
Float_t SFibersGeomPar::getFiberOffsetZ(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].fiber_offset_z[l];
    else
        return -10000.;
}

/**
 * Get fibers pitch in a layer
 *
 * \param m module
 * \param l layer
 * \return fibers pitch
 */
Float_t SFibersGeomPar::getFibersPitch(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].fibers_pitch[l];
    else
        return -10000.;
}
