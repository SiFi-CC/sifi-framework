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

#include "SSiPMsGeomPar.h"
#include "SParContainer.h"

#include <cstdio> // for printf, putchar
#include <memory> // for allocator

/**
 * \class SSiPMsGeomPar
\ingroup lib_fibers

A container for SiPM geometry parameters

\sa SPar

*/

/**
 * Constructor
 */
SSiPMsGeomPar::SSiPMsGeomPar() : SPar(), mods(nullptr) {}

/**
 * Destructor
 */
SSiPMsGeomPar::~SSiPMsGeomPar() { clear(); }

/**
 * Clear parameters
 */
void SSiPMsGeomPar::clear()
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
bool SSiPMsGeomPar::getParams(SParContainer* parcont)
{

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

    // get sipms
    TArrayI _s;
    if (!parcont->fill("nSiPMs", _s)) return false;
    if (_s.GetSize() != (n_layers))
    {
        std::cerr << "Size of nSiPMs doesn't match nModules*nLayers" << std::endl;
        return false;
    }

    TArrayF _sox;
    if (!parcont->fill("fSiPMOffsetX", _sox)) return false;
    if (_sox.GetSize() != (n_layers))
    {
        std::cerr << "Size of fSiPMOffsetX doesn't match nModules" << std::endl;
        return false;
    }

    TArrayF _soy;
    if (!parcont->fill("fSiPMOffsetY", _soy)) return false;
    if (_soy.GetSize() != (n_layers))
    {
        std::cerr << "Size of fSiPMOffsetY doesn't match nModules" << std::endl;
        return false;
    }

    TArrayF _soz;
    if (!parcont->fill("fSiPMOffsetZ", _soz)) return false;
    if (_soz.GetSize() != (n_layers))
    {
        std::cerr << "Size of fSiPMOffsetZ doesn't match nModules" << std::endl;
        return false;
    }

    TArrayF _ssp;
    if (!parcont->fill("fSiPMsPitch", _ssp)) return false;
    if (_ssp.GetSize() != (n_layers))
    {
        std::cerr << "Size of fSiPMPitch doesn't match nModules" << std::endl;
        return false;
    }

    TArrayF _spx;
    if (!parcont->fill("fSiPMOffsetZ", _spx)) return false;
//     if (_spx.GetSize() != (_s[_l]))
//     {
//         std::cerr << "Size of fSiPMOffsetZ doesn't match nModules" << std::endl;
//         return false;
//     }
    
/*
    TArrayF sipms_x; ///< SiPM X positions in a layer
    Float_t sipms_y_bottom; ///< SiPM Y position on bottom side
    Float_t sipms_y_top; ///< SiPM Y position on top side
    Float_t getSiPMsX(Int_t m, Int_t l) const;
    Float_t getSiPMsYBottom(Int_t m, Int_t l) const;
    Float_t getSiPMsYTop(Int_t m, Int_t l) const;
    */
    
    
    int cnt_layers = 0;
    for (int m = 0; m < modules; ++m)
    {
        mods[m].module_z = _mz[m];
        mods[m].layers = _l[m];
        mods[m].sipms.Set(mods[m].layers);
//         mods[m].layer_rotation.Set(mods[m].layers);
        mods[m].sipm_offset_x.Set(mods[m].layers);
        mods[m].sipm_offset_y.Set(mods[m].layers);
        mods[m].sipm_offset_z.Set(mods[m].layers);
        mods[m].sipms_pitch.Set(mods[m].layers);

        for (int l = 0; l < _l[m]; ++l)
        {
            Int_t sipms = _s[cnt_layers * m + l];
//             Int_t layrot = _lr[cnt_layers * m + l];
            Float_t fox = _sox[cnt_layers * m + l];
            Float_t foy = _soy[cnt_layers * m + l];
            Float_t foz = _soz[cnt_layers * m + l];
            Float_t fsp = _ssp[cnt_layers * m + l];

            mods[m].sipms[l] = sipms;
//             mods[m].layer_rotation[l] = layrot;
            mods[m].sipm_offset_x[l] = fox;
            mods[m].sipm_offset_y[l] = foy;
            mods[m].sipm_offset_z[l] = foz;
            mods[m].sipms_pitch[l] = fsp;
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
bool SSiPMsGeomPar::putParams(SParContainer* /*parcont*/) const { return true; }

/**
 * Print parameters
 */
void SSiPMsGeomPar::print() const
{
    printf("Number of modules = %d\n", modules);
    for (int m = 0; m < modules; ++m)
    {
        printf(" [%2d] module z: %f", m, mods[m].module_z);
        printf(" +++\n layers = %d\n", mods[m].layers);
        printf(" sipms:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %2d", mods[m].sipms[l]);
//         printf("\n layrot:");
//         for (int l = 0; l < mods[m].layers; ++l)
//             printf(" %#.2f", mods[m].layer_rotation[l]);
//         printf("\n  off x:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %#.2f", mods[m].sipm_offset_x[l]);
        printf("\n  off y:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %#.2f", mods[m].sipm_offset_y[l]);
        printf("\n  off z:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %#.2f", mods[m].sipm_offset_z[l]);
        printf("\n  pitch:");
        for (int l = 0; l < mods[m].layers; ++l)
            printf(" %#.2f", mods[m].sipms_pitch[l]);
        putchar('\n');
    }
}

/**
 * Get module z
 *
 * \param m module
 * \return z-coordinate
 */
Float_t SSiPMsGeomPar::getModuleZ(Int_t m) const
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
Int_t SSiPMsGeomPar::getLayers(Int_t m) const
{
    if (mods and m < modules)
        return mods[m].layers;
    else
        return -1;
}

/**
 * Get number of SiPMs in the layer
 *
 * \param m module
 * \param l layer
 * \return number of SiPMs
 */
Int_t SSiPMsGeomPar::getSiPMs(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].sipms[l];
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
// Float_t SFibersGeomPar::getLayerRotation(Int_t m, Int_t l) const
// {
//     if (mods and m < modules and l < mods[m].layers)
//         return mods[m].layer_rotation[l];
//     else
//         return -10000.;
// }

/**
 * Get SiPMs offset X
 *
 * \param m module
 * \param l layer
 * \return offset X
 */
Float_t SSiPMsGeomPar::getSiPMOffsetX(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].sipm_offset_x[l];
    else
        return -10000.;
}

/**
 * Get SiPMs offset Y
 *
 * \param m module
 * \param l layer
 * \return offset Y
 */
Float_t SSiPMsGeomPar::getSiPMOffsetY(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].sipm_offset_y[l];
    else
        return -10000.;
}

/**
 * Get SiPMs offset Z
 *
 * \param m module
 * \param l layer
 * \return offset Z
 */
Float_t SSiPMsGeomPar::getSiPMOffsetZ(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].sipm_offset_z[l];
    else
        return -10000.;
}

/**
 * Get SiPMs pitch in a layer
 *
 * \param m module
 * \param l layer
 * \return SiPMs pitch
 */
Float_t SSiPMsGeomPar::getSiPMsPitch(Int_t m, Int_t l) const
{
    if (mods and m < modules and l < mods[m].layers)
        return mods[m].sipms_pitch[l];
    else
        return -10000.;
}
