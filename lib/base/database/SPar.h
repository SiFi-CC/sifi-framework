// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SPAR_H
#define SPAR_H

#include "sifi_export.h"

#include "SDatabase.h"

#include <string>

class SParContainer;

class SIFI_EXPORT SPar
{
public:
    // constructor
    SPar() = default;
    // destructor
    virtual ~SPar() = default;

    // methods
    /// Clear parameters
    virtual void clear() = 0;
    /// Print parameters
    virtual void print() const {};

protected:
    /// Get parameters from container
    /// \param parcont pointer to container object
    /// \return success
    virtual bool getParams(SParContainer* parcont) = 0;
    /// Put parameters into container
    /// \param parcont pointer to container object
    /// \return success
    virtual bool putParams(SParContainer* parcont) const = 0;

    friend SPar* SDatabase::getParContainer(const std::string&);
};

#endif /* SPAR_H */
