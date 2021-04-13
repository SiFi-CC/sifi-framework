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

#include <Rtypes.h>  // for THashConsistencyHolder, ClassDef
#include <TObject.h> // for TObject
#include <TString.h>

#include <map>

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * \class SRootFileHeader
\ingroup lib_base_util

A header object for the category

*/

class SRootFileHeader : public TObject
{
public:
    std::map<SCategory::Cat, TString> catName; ///< category type
public:
    // constructor
    SRootFileHeader() = default;

private:
    ClassDef(SRootFileHeader, 1);
};

#endif /* SROOTFILEHEADER_H */
