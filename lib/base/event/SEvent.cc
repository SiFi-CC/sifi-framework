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

#include <cstddef>     // for size_t
#include <sys/types.h> // for uint

/**
 * Constructor. Initializes array of objects, The array has doubel size of the
 * number of available categories to handle data and sim objects.
 */
SEvent::SEvent() : TObject() { categories = new TObjArray(SCategory::CatLimitDoNotUse * 2); }

SEvent::~SEvent()
{
    categories->Clear("C");
    delete categories;
}

/**
 * Register a category in the event.
 *
 * \param catid category ID
 * \param category pointer to the category
 */
void SEvent::addCategory(SCategory::Cat catid, SCategory* category)
{
    int pos = SiFi::getCategoryIndex(catid);
    if (!categories->At(pos)) categories->AddAt(category, pos);
}

/**
 * Retrieve teh category from the event.
 *
 * \param catid category id
 * \return category object pointer
 */
SCategory* SEvent::getCategory(SCategory::Cat catid)
{
    int pos = SiFi::getCategoryIndex(catid);
    return dynamic_cast<SCategory*>(categories->At(pos));
}

/**
 * Clear all categories. This function is used in event by event reading.
 */
void SEvent::clearCategories()
{
    size_t n = categories->GetEntries();
    for (uint i = 0; i < n; ++i)
    {
        if (categories->At(i)) dynamic_cast<SCategory*>(categories->At(i))->clear();
    }
}
