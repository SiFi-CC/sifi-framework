// @(#)lib/fibers:$Id$

#include <iostream>

#include "SPixelDetectorGeomPar.h"
#include "SParContainer.h"

#include <cstdio> // for printf, putchar
#include <memory> // for allocator
#include <numeric> // for accumulate

/**
 * \class SPixelDetectorGeomPar
\ingroup lib_fibers

A container for detector geometry parameters

\sa SPar

*/

/**
 * Constructor
 */
SPixelDetectorGeomPar::SPixelDetectorGeomPar() : SPar(), mod(nullptr) {}

/**
 * Destructor
 */
SPixelDetectorGeomPar::~SPixelDetectorGeomPar() { clear(); }

/**
 * Clear parameters
 */
void SPixelDetectorGeomPar::clear()
{
    delete[] mod;
    mod = nullptr;
}

/**
 * Get parameters
 *
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SPixelDetectorGeomPar::getParams(SParContainer* parcont)
{
    // get module position
    TArrayF _mp;
    if (!parcont->fill("fModulePosition", _mp)) return false;
    if (_mp.GetSize() != 3)
    {
        std::cerr << "Size of fModulePosition must be 3" << std::endl;
        return false;
    }

    // get board position
    TArrayF _bp;
    if (!parcont->fill("fBoardPosition", _bp)) return false;
    if (_bp.GetSize() != 3)
    {
        std::cerr << "Size of fBoardPosition must be 3" << std::endl;
        return false;
    }

    // get number of SiPM layers
    TArrayI _sl;
    if (!parcont->fill("nSiPMLayers", _sl)) return false;
    if (_sl.GetSize() != 1)
    {
        std::cerr << "Size of nSiPMLayers must be 1" << std::endl;
        return false;
    }

    // get number of needle layers
    TArrayI _fl;
    if (!parcont->fill("nNeedleLayers", _fl)) return false;
    if (_fl.GetSize() != 1)
    {
        std::cerr << "Size of nNeedleLayers must be 1" << std::endl;
        return false;
    }

    // get number of SiPMs in each layer
    TArrayI _s;
    if (!parcont->fill("nSiPMsPerLayer", _s)) return false;
    if (_s.GetSize() != _sl[0])
    {
        std::cerr << "Size of nSiPMsPerLayer doesn't match nSiPMLayers" << std::endl;
        return false;
    }

    // get number of needles in each layer
    TArrayI _f;
    if (!parcont->fill("nNeedlesPerLayer", _f)) return false;
    if (_f.GetSize() != _fl[0])
    {
        std::cerr << "Size of nNeedlesPerLayer doesn't match nNeedleLayers" << std::endl;
        return false;
    }

    if (mod) delete[] mod;
    mod = new SingleModule[1];

    mod[0].modulePosition.SetXYZ(_mp[0], _mp[1], _mp[2]);
    mod[0].boardPosition.SetXYZ(_bp[0], _bp[1], _bp[2]);
    mod[0].numSiPMLayers = _sl[0];
    mod[0].numNeedleLayers = _fl[0];
    mod[0].numSiPMsPerLayer.Set(mod[0].numSiPMLayers);
    mod[0].numNeedlesPerLayer.Set(mod[0].numNeedleLayers);

    for (Int_t sl = 0; sl < _sl[0]; ++sl)
    {
        mod[0].numSiPMsPerLayer[sl] = _s[sl];
    }

    for (Int_t fl = 0; fl < _fl[0]; ++fl)
    {
        mod[0].numNeedlesPerLayer[fl] = _f[fl];
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
bool SPixelDetectorGeomPar::putParams(SParContainer* /*parcont*/) const { return true; }

/**
 * Print parameters
 */
void SPixelDetectorGeomPar::print() const
{
    if (!mod) return;

    printf("Single-module detector geometry\n");
    printf(" module position: (%f, %f, %f)\n", mod[0].modulePosition.X(), mod[0].modulePosition.Y(), mod[0].modulePosition.Z());
    printf(" board position: (%f, %f, %f)\n", mod[0].boardPosition.X(), mod[0].boardPosition.Y(), mod[0].boardPosition.Z());
    printf(" number of SiPM layers = %d\n", mod[0].numSiPMLayers);
    printf(" number of SiPMs per layer:");
    for (int sl = 0; sl < mod[0].numSiPMLayers; ++sl)
        printf(" %2d", mod[0].numSiPMsPerLayer[sl]);
    printf("\n number of needle layers = %d\n", mod[0].numNeedleLayers);
    printf(" number of needles per layer:");
    for (int fl = 0; fl < mod[0].numNeedleLayers; ++fl)
        printf(" %2d", mod[0].numNeedlesPerLayer[fl]);
    putchar('\n');
}

/**
 * Get module position
 *
 * \return position (TVector3)
 */
TVector3 SPixelDetectorGeomPar::getModulePosition() const { return mod[0].modulePosition; }

TVector3 SPixelDetectorGeomPar::getBoardPosition() const { return mod[0].boardPosition; }

Int_t SPixelDetectorGeomPar::getNumSiPMLayers() const { return mod[0].numSiPMLayers; }

Int_t SPixelDetectorGeomPar::getNumNeedleLayers() const { return mod[0].numNeedleLayers; }

Int_t SPixelDetectorGeomPar::getNumSiPMsPerLayer(Int_t layer) const
{
    if (layer < mod[0].numSiPMLayers)
        return mod[0].numSiPMsPerLayer[layer];

    std::cerr << "Could not obtain the number of SiPMs per layer!" << std::endl;
    std::abort();
}

Int_t SPixelDetectorGeomPar::getNumNeedlesPerLayer(Int_t layer) const
{
    if (layer < mod[0].numNeedleLayers)
        return mod[0].numNeedlesPerLayer[layer];

    std::cerr << "Could not obtain the number of needles per layer!" << std::endl;
    std::abort();
}

TVector3 SPixelDetectorGeomPar::getModulePosition(Int_t module) const
{
    if (module != 0)
    {
        std::cerr << "This container only supports module 0" << std::endl;
        std::abort();
    }
    return getModulePosition();
}

/**
 * Get board position
 *
 * \return position (TVector3)
 */
TVector3 SPixelDetectorGeomPar::getBoardPosition(Int_t module) const
{
    if (module != 0)
    {
        std::cerr << "This container only supports module 0" << std::endl;
        std::abort();
    }
    return getBoardPosition();
}

/**
 * Get number of SiPM layers in the module
 *
 * \return number of layers
 */
Int_t SPixelDetectorGeomPar::getNumSiPMLayers(Int_t module) const
{
    if (module != 0)
    {
        std::cerr << "This container only supports module 0" << std::endl;
        std::abort();
    }
    return getNumSiPMLayers();
}

/**
 * Get number of needle layers in the module
 *
 * \return number of layers
 */
Int_t SPixelDetectorGeomPar::getNumNeedleLayers(Int_t module) const
{
    if (module != 0)
    {
        std::cerr << "This container only supports module 0" << std::endl;
        std::abort();
    }
    return getNumNeedleLayers();
}

/**
 * Get number of SiPMs in each layer
 *
 * \return number of needles
 */
Int_t SPixelDetectorGeomPar::getNumSiPMsPerLayer(Int_t module, Int_t layer) const
{
    if (module != 0)
    {
        std::cerr << "This container only supports module 0" << std::endl;
        std::abort();
    }
    return getNumSiPMsPerLayer(layer);
}

/**
 * Get number of needles in each layer
 *
 * \return number of needles
 */
Int_t SPixelDetectorGeomPar::getNumNeedlesPerLayer(Int_t module, Int_t layer) const
{
    if (module != 0)
    {
        std::cerr << "This container only supports module 0" << std::endl;
        std::abort();
    }
    return getNumNeedlesPerLayer(layer);
}

/**
 * Get swSiPMID from address
 *
 */
void SPixelDetectorGeomPar::getswSiPMIDFromAddress(Int_t& id, Int_t module, Int_t layer, Int_t element, char side) const
{
    if (module != 0)
    {
        std::cerr << "This container only supports module 0" << std::endl;
        std::abort();
    }
    
    // side parameter is kept for compatibility, but is always treated as 'l'
    id = 0;
    for (Int_t sl = 0; sl < layer; ++sl)
    {
        id += mod[0].numSiPMsPerLayer[sl];
    }

    id += element;
}

/**
 * Get swNeedleID from address
 *
 */
void SPixelDetectorGeomPar::getswNeedleIDFromAddress(Int_t& id, Int_t module, Int_t layer, Int_t needle) const
{
    if (module != 0)
    {
        std::cerr << "This container only supports module 0" << std::endl;
        std::abort();
    }

    id = 0;
    for (Int_t fl = 0; fl < layer; ++fl)
    {
        id += mod[0].numNeedlesPerLayer[fl];
    }

    id += needle;
}

/**
 * Get address from swSiPMID
 *
 */
bool SPixelDetectorGeomPar::getAddressFromswSiPMID(Int_t id, Int_t& module, Int_t& layer, Int_t& element, char& side) const
{
    if (id < 0) return false;

    // side is always 'l'
    side = 'l';

    // module is always 0
    module = 0;

    Int_t totalSiPMsInLayer = 0;
    for (Int_t sl = 0; sl < mod[0].numSiPMLayers; ++sl)
    {
        if (id < totalSiPMsInLayer + mod[0].numSiPMsPerLayer[sl])
        {
            layer = sl;
            element = id - totalSiPMsInLayer;
            return true;
        }
        totalSiPMsInLayer += mod[0].numSiPMsPerLayer[sl];
    }
    return false;
}

/**
 * Get address from swNeedleID
 *
 */
bool SPixelDetectorGeomPar::getAddressFromswNeedleID(Int_t id, Int_t& module, Int_t& layer, Int_t& needle) const
{
    if (id < 0) return false;

    // module is always 0
    module = 0;

    Int_t totalNeedlesInLayer = 0;
    for (Int_t fl = 0; fl < mod[0].numNeedleLayers; ++fl)
    {
        if (id < totalNeedlesInLayer + mod[0].numNeedlesPerLayer[fl])
        {
            layer = fl;
            needle = id - totalNeedlesInLayer;
            return true;
        }
        totalNeedlesInLayer += mod[0].numNeedlesPerLayer[fl];
    }
    return false;
}
