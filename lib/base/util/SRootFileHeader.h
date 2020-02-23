// @(#)lib/base/util:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SROOTFILEHEADER_H
#define SROOTFILEHEADER_H

#include "SCategory.h"

#include <TArrayI.h>
#include <TMap.h>
#include <TString.h>

#include "map"

/** \class SRootFileHeader
\ingroup lib_base_util

A header object for the category

*/

typedef std::map<SCategory::Cat, TString> CatNameMap;

class SRootFileHeader : public TObject
{
public:
    CatNameMap  catName;
public:
    // constructor
    SRootFileHeader() : TObject() {}

private:
    ClassDef(SRootFileHeader,1);
};

#endif /* SROOTFILEHEADER_H */
