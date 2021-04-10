// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SPARCONTAINER_H
#define SPARCONTAINER_H

#include "sifi_export.h"

#include "SDatabase.h"

#include <TArrayD.h>
#include <TArrayF.h>
#include <TArrayI.h>

#include <map>
#include <vector>

typedef std::pair<std::string, std::vector<std::string>> TypeDataField;
typedef std::map<std::string, TypeDataField> ParMap;

class SIFI_EXPORT SParContainer
{
protected:
    // members
    ParMap parameters;     ///< holds pairs of parameter string: key-values
    std::string container; ///< container name
    size_t line_split{8};  ///< how many parameters per line in file

public:
    // constructor
    explicit SParContainer(const std::string& name);
    // destructor
    virtual ~SParContainer() = default;

    // methods
    virtual bool add(const std::string& name, Int_t val);
    virtual bool add(const std::string& name, Float_t val);
    virtual bool add(const std::string& name, Double_t val);
    virtual bool add(const std::string& name, const TArrayI& val);
    virtual bool add(const std::string& name, const TArrayF& val);
    virtual bool add(const std::string& name, const TArrayD& val);

    virtual bool fill(const std::string& name, Int_t& val);
    virtual bool fill(const std::string& name, Float_t& val);
    virtual bool fill(const std::string& name, Double_t& val);
    virtual bool fill(const std::string& name, TArrayI& val);
    virtual bool fill(const std::string& name, TArrayF& val);
    virtual bool fill(const std::string& name, TArrayD& val);

    bool initParam(const std::string& name, const std::string& type,
                   const std::vector<std::string>& values);
    void print() const;

    void clear();

protected:
    void fromContainer(SContainer* sc);
    void toContainer(SContainer* sc) const;

    // methods
    /// Set line split
    /// \param n number of values per line
    void setLineSplit(size_t n) { line_split = n; }

    friend SPar* SDatabase::getParContainer(const std::string&);
};

#endif /* SPARCONTAINER_H */
