
#include "SFibersStackHitFinder.h"

#include "SCategory.h"
#include "SFibersStackCal.h"
#include "SFibersStackHit.h"
#include "SLocator.h"
#include "SiFi.h"

#include <RtypesCore.h>

#include <cstdio>
#include <iostream>

/**
 * \class SFibersStackHitFinder
\ingroup lib_fibers_stack

A calibraror task for Fibers Stack.

Takes MGeantFibersStackRaw data and applies calibration. See MTask for the
interface description.

*/

/**
 * Default constructor
 */
SFibersStackHitFinder::SFibersStackHitFinder()
    : STask(), catFibersCal(nullptr),
      catFibersHit(nullptr) //,  pHitFinderPar(nullptr)
{
}

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SFibersStackHitFinder::init()
{

    // get Cal category
    catFibersCal = sifi()->getCategory(SCategory::CatFibersStackCal);
    if (!catFibersCal)
    {
        std::cerr << "No CatFibersStackCal category"
                  << "\n";
        return false;
    }

    // create Hit category
    catFibersHit = sifi()->buildCategory(SCategory::CatFibersStackHit);
    if (!catFibersHit)
    {
        std::cerr << "No CatFibersStackHit category"
                  << "\n";
        return false;
    }

    //     get calibrator parameters
    //     pHitFinderPar =
    //     dynamic_cast<SCalContainer*>(pm()->getCalibrationContainer("SFibersStackCalibratorPar"));
    //     if (!pCalibratorPar)
    //     {
    //         std::cerr << "Parameter container 'SFibersStackCalibratorPar' was
    //         not obtained!" << std::endl; exit(EXIT_FAILURE);
    //     }

    return true;
}

/**
 * Execute task
 *
 * \sa STask::execute()
 * \return success
 */
bool SFibersStackHitFinder::execute()
{
    int size = catFibersCal->getEntries(); // changed from Raw
    for (int i = 0; i < size; ++i)
    {
        SFibersStackCal* pCal =
            dynamic_cast<SFibersStackCal*>(catFibersCal->getObject(i));
        if (!pCal)
        {
            printf("FibersStackCal doesn't exists!\n");
            continue;
        }
        Int_t mod = 0;
        Int_t lay = 0;
        Int_t fib = 0;
        pCal->getAddress(mod, lay, fib);

        // calc laboratory coordinates from digi data
        Float_t u = pCal->getU();
        Float_t y = pCal->getY();
        Float_t qdc_l = pCal->getQDCL();
        Float_t qdc_r = pCal->getQDCR();
        Float_t time_l = pCal->getTimeL();
        Float_t time_r = pCal->getTimeR();

        if (fabs(time_l) < 0.0001 or fabs(time_r) < 0.0001) continue;

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersStackHit* pHit =
            dynamic_cast<SFibersStackHit*>(catFibersHit->getObject(loc));
        if (!pHit)
        {
            pHit =
                reinterpret_cast<SFibersStackHit*>(catFibersHit->getSlot(loc));
            new (pHit) SFibersStackHit;
            pHit->Clear();
        }

        pHit->setAddress(mod, lay, fib);
        //         pHit->setU(lab_u);
        //         pHit->setY(lab_y);
        //         pHit->setQDC(energy_l, energy_r);
        //         pHit->setTime(time_l, time_r);

        // pHit->getAddress();
        Float_t v = 3e8 * 1e3 * 1e-9 / 1.82; // c * mm/m * ns/s / n_scint
        Float_t L = 0.1;                     // m
        Float_t hitPosTime;
        hitPosTime = ((time_l - time_r) * v + L) / 2 - L / 2;
        pHit->setZt(hitPosTime);
    }

    return true;
}

/**
 * Finalize task
 *
 * \sa STask::finalize()
 * \return success
 */
bool SFibersStackHitFinder::finalize() { return true; }
