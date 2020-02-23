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


class SParContainer;

class SPar
{
    friend class SParManager;       ///< MParManager is friend
protected:
    // members
    SParContainer * parcont;        ///< Pointer to object of parameter container

public:
    // constructor
    SPar();
    // destructor
    virtual ~SPar();

    // methods
    /// Clear parameters
    virtual void clear() = 0;
    virtual void print() const;

protected:
    /// Get parameters from container
    /// \param parcont pointer to container object
    /// \return success
    virtual bool getParams(SParContainer * parcont) = 0;
    /// Put parameters into container
    /// \param parcont pointer to container object
    /// \return success
    virtual bool putParams(SParContainer * parcont) const = 0;
};

#endif /* SPAR_H */
