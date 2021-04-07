// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SCONTAINER_H
#define SCONTAINER_H

#include "sifi_export.h"

#include "SLookup.h"

#include <TArrayD.h>
#include <TArrayF.h>
#include <TArrayI.h>

#include <map>
#include <unordered_map>
#include <vector>

/**
 * Template that contains basic calibation parameters. Consist of a N-parameters array:
 *  * #par,
 * which interpretation can be any, depends of the user.
 *
 * This structure must provie interface to read and write cal pars to a
 * container. Each deriving class must also provide proper interface.
 *  * read() - reads cal pars from container
 *  * write() - writes cal pars to the container
 *  * print() - print cal par values
 */
/**
 * Stores content of the container read out from the disk.
 */
struct SIFI_EXPORT SContainer
{
    std::string name;               ///< container name
    std::vector<std::string> lines; ///< lines read out from the container
    bool updated{false};

    SContainer() = default;
    SContainer(const SContainer&) = default;

    /// Print the contaner name and content
    void print() const
    {
        printf("Container Table [%s]\n", name.c_str());
        for (auto line : lines)
        {
            printf("%s\n", line.c_str());
        }
    }
};

#endif /* SCALCONTAINER_H */
