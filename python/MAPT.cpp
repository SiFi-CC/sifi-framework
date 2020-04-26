/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <TString.h>

#include "MMAPTManager.h"
#include "MDetectorManager.h"
#include "MTaskManager.h"

#include "MGeantTrack.h"
#include "MGeantFibersRaw.h"
#include "MFibersStackCalSim.h"

#include "MFibersStackDetector.h"

#include "MAPT.h"

#include <iostream>

MAPT::MAPT()
    : dataManager(nullptr), detm(nullptr), catGeantTrack(nullptr)
    , catGeantFibersRaw(nullptr), catFibersStackCalSim(nullptr)
{
}

MAPT::~MAPT()
{
}

int MAPT::initAnalysis(const std::string & file, int events)
{
    TString oname = file;

    if (oname.EndsWith(".root"))
        oname.ReplaceAll(".root", "_ana.root");
    else
        oname.Append("_ana.root");

    dataManager = MMAPTManager::instance();
    dataManager->setSimulation(true);
    dataManager->setInputFileName(file);
    dataManager->open();

    // initialize detectors
    detm = MDetectorManager::instance();
    detm->addDetector(new MFibersStackDetector("FibersStack"));
    detm->initTasks();
//     detm->initParameterContainers();
    detm->initCategories();

    dataManager->openCategory(MCategory::CatGeantTrack, true);
    dataManager->openCategory(MCategory::CatGeantFibersRaw, true);
    dataManager->openCategory(MCategory::CatFibersStackCal, true);

    dataManager->setOutputFileName(oname.Data());
    dataManager->book();
    dataManager->print();

    ev_limit = events < dataManager->getEntries() ? events : dataManager->getEntries();
    std::cout << dataManager->getEntries() << " events, analyze " << ev_limit << std::endl;

    return 0;
}

int MAPT::getEvent(long i)
{
    printf("dupa = %ld\n", i);
    dataManager->getEntry(i);
    catGeantTrack = dataManager->getCategory(MCategory::CatGeantTrack);
    if (!catGeantTrack)
    {
    }

    catGeantFibersRaw = dataManager->getCategory(MCategory::CatGeantFibersRaw);
    if (!catGeantFibersRaw)
    {
    }

    catFibersStackCalSim = dataManager->getCategory(MCategory::CatFibersStackCal);
    if (!catFibersStackCalSim)
    {
    }

    return i;
}

// void MAPT::printEvent() const
// {
//     dataManager->printEvent();
// }


int MAPT::getGeantTrackNum() const
{
    if (catGeantTrack)
        return catGeantTrack->getEntries();
    else
        return -1;
}

int MAPT::getGeantFibersRawNum() const
{
if (catGeantFibersRaw)
        return catGeantFibersRaw->getEntries();
    else
        return -1;
}

int MAPT::getFibersStackCalSimNum() const
{
if (catFibersStackCalSim)
        return catFibersStackCalSim->getEntries();
    else
        return -1;
}

int MAPT::getFibersStackCalNum() const
{
    return getFibersStackCalSimNum();
}

MGeantTrack * MAPT::getGeantTrack(int i) const
{
    return (MGeantTrack *) catGeantTrack->getObject(i);
}

MGeantFibersRaw * MAPT::getGeantFibersRaw(int i) const
{
    return (MGeantFibersRaw *) catGeantFibersRaw->getObject(i);
}

MFibersStackCalSim * MAPT::getFibersStackCalSim(int i) const
{
    return (MFibersStackCalSim *) catFibersStackCalSim->getObject(i);
}

MFibersStackCal * MAPT::getFibersStackCal(int i) const
{
    return (MFibersStackCal *) catFibersStackCalSim->getObject(i);
}

void MAPT::fill()
{
    dataManager->fill();
}

void MAPT::save()
{
    dataManager->save();
}

