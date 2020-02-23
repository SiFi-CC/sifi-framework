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

#include <TArrayI.h>
#include <TArrayF.h>
#include <TArrayD.h>

#include <vector>
#include <map>

typedef std::pair<std::string, std::vector<std::string> > TypeDataField;
typedef std::map<std::string, TypeDataField> ParMap;

class SParContainer
{
protected:
    // members
    ParMap parameters;          ///< holds pairs of parameter string: key-values
    std::string container;      ///< container name
    size_t line_split;          ///< how many parameters per line in file

public:
    // constructor
    SParContainer(const std::string & container);
    // destructor
    virtual ~SParContainer() {}

    // methods
    virtual bool add(const std::string & name, Int_t val);
    virtual bool add(const std::string & name, Float_t val);
    virtual bool add(const std::string & name, Double_t val);
    virtual bool add(const std::string & name, const TArrayI & val);
    virtual bool add(const std::string & name, const TArrayF & val);
    virtual bool add(const std::string & name, const TArrayD & val);

    virtual bool fill(const std::string & name, Int_t & val);
    virtual bool fill(const std::string & name, Float_t & val);
    virtual bool fill(const std::string & name, Double_t & val);
    virtual bool fill(const std::string & name, TArrayI & val);
    virtual bool fill(const std::string & name, TArrayF & val);
    virtual bool fill(const std::string & name, TArrayD & val);

    bool initParam(const std::string & name, const std::string & type, const std::vector<std::string> & values);

    void print();

protected:
    // methods
    /// Set line split
    /// \param n number of values per line
    void setLineSplit(size_t n) { line_split = n; }
};

#endif /* SPARCONTAINER_H */
