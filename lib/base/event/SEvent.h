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

#include "sifi_export.h"

#include "SCategory.h"

#include <TObject.h>

class TObjArray;

/**
 * All the data are kelt in a single tree in a root file. This structure is
 * written or read from the file, and contains description of the tree
 * composition.
 */
class SEvent : public TObject
{
public:
    SEvent();
    virtual ~SEvent();

    void addCategory(SCategory::Cat catid, SCategory* category);
    SCategory* getCategory(SCategory::Cat catid);

    void clearCategories();

private:
    TObjArray* categories; ///< Array of categories

    ClassDef(SEvent, 1);
};

#endif /* SEVENT_H */
