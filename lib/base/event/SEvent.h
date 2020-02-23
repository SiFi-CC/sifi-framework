// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SEVENT_H
#define SEVENT_H

#include "SCategory.h"

#include <TObject.h>

class TObjArray;

class SEvent : public TObject
{
public:
    SEvent();
    virtual ~SEvent();

    void addCategory(SCategory::Cat cat, SCategory * address);
    SCategory * getCategory(SCategory::Cat cat);

    void clearCategories();

private:
    TObjArray * categories;

    ClassDef(SEvent, 1);
};

#endif /* SEVENT_H */
