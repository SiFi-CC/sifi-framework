// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SEvent.h"
#include "SiFi.h"

#include <TObjArray.h>

#include <iostream>
#include <map>


SEvent::SEvent() : TObject()
{
    categories = new TObjArray(SCategory::CatLimitDoNotUse * 2);
}

SEvent::~SEvent()
{
    categories->Clear("C");
    delete categories;
}

void SEvent::addCategory(SCategory::Cat cat, SCategory* address)
{
    int pos = SiFi::getCategoryIndex(cat, sifi()->isSimulation());PR(categories->At(pos));
    if (!categories->At(pos))
//         delete categories->At(pos);
        categories->AddAt(address, pos);
}

SCategory * SEvent::getCategory(SCategory::Cat cat)
{
    int pos = SiFi::getCategoryIndex(cat, sifi()->isSimulation());
    return (SCategory *)categories->At(pos);
}

void SEvent::clearCategories()
{
    size_t n = categories->GetEntries();
    for (uint i = 0; i < n; ++i)
    {
        if (categories->At(i))
            ((SCategory *)categories->At(i))->clear();
    }
}
