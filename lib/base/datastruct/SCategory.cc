// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SCategory.h"

#include "SLocator.h" // for SLocator

#include <TArrayI.h> // for TArrayI
#include <TClonesArray.h>

#include <cassert> // for assert
#include <cstdio>  // for printf
#include <iostream>

/**
 * \class SCategory
\ingroup lib_base

A Category abstract interface

*/

/// null object
TObject* pNullSCategoryPtr = nullptr;

/// Default consructor
SCategory::SCategory() : TObject(), data(nullptr)
{
    header.clear();
    index.clear();
}

/**
 * Constructor
 *
 * \param name name of the category
 * \param dim number of dimensions
 * \param sizes array of sizes of dimensions
 * \param simulation set tru if category for simulation data
 */
SCategory::SCategory(const char* name, size_t dim, size_t* sizes, bool simulation)
    : TObject(), data(nullptr)
{
    header.clear();
    index.clear();
    setup(name, dim, sizes, simulation);

    header.writable = kTRUE;
}

/// Destructor
SCategory::~SCategory()
{
    if (data) delete data;
}

/**
 * Setup category
 *
 * \param name name of the category
 * \param dim number of dimensions
 * \param sizes array of sizes of dimensions
 * \param simulation set true if category for simulation data
 */
void SCategory::setup(const char* name, size_t dim, size_t* sizes, bool simulation)
{
    assert(data == nullptr);
    header.name = name;
    header.dim = dim;
    header.simulation = simulation;
    header.dim_sizes.Set(dim);
    header.dim_offsets.Set(dim);

    header.dim_offsets[0] = 1;
    header.dim_sizes[0] = sizes[0];

    header.data_size = sizes[0];
    for (size_t i = 1; i < dim; ++i)
    {
        header.dim_offsets[i] = header.dim_offsets[i - 1] * sizes[i - 1];
        header.dim_sizes[i] = sizes[i];
        header.data_size *= sizes[i];
    }

    data = new TClonesArray(name, header.data_size);

    printf("Category %s created with linear size of %lu\n", name, header.data_size);
}

// /**
//  * Returns the object at the address from n locator.
//  * Dimension of n must match dimension of the container.
//  *
//  * \param n locator
//  * \return pointer to an object
//  */
// TObject * SCategory::operator[](const SLocator & n)
// {
//     if (!checkDim(header.dim))     return nullptr;
//
//     size_t pos = loc2pos(n);
//     Int_t p = index.getMapIndex(pos);
//     if (p < 0) return nullptr;
//     return data->ConstructedAt(p);
// }

/**
 * Returns a reference to an object at the address from locator n.
 * Dimension of n must match dimension of the container.
 *
 * \param n locator object
 * \return reference to a slot
 */
TObject*& SCategory::getSlot(const SLocator& n)
{
    if (!checkDim(n)) return pNullSCategoryPtr;

    size_t pos = loc2pos(n);
    if (!index.setMapIndex(pos, pos))
    {
        std::cerr << "Category " << header.name << " was already compressed, can't add new slots."
                  << std::endl;
        return pNullSCategoryPtr;
    }

    if (!getObject(n)) {};

    return data->operator[](pos);
}

/**
 * Create a new slot at the end of the array and returns it reference.
 * Awailable only for 1d categories.
 *
 * \bug assure that it really creates new slot, not n+1 which migh overlap
 * with existing slot.
 * \return reference to a slot
 */
TObject*& SCategory::getNewSlot()
{
    if (header.dim != 1)
    {
        std::cerr << "getNewSlot allowed only for linear categories." << std::endl;
        return pNullSCategoryPtr;
    }

    SLocator loc(1);
    loc[0] = data->GetEntries();
    return getSlot(loc);
}

/**
 * Returns an object at the address from locator n.
 * Dimension of n must match dimension of the container.
 *
 * \param n locator object
 * \return pointer to the object
 */
TObject* SCategory::getObject(const SLocator& n)
{
    if (!checkDim(n)) return pNullSCategoryPtr;

    size_t pos = loc2pos(n);
    Int_t p = index.getMapIndex(pos);
    if (p < 0) return nullptr;
    return data->At(p);
}

/**
 * Returns an object at the index i.
 *
 * \param i index
 * \return pointer to the object
 */
TObject* SCategory::getObject(Int_t i)
{
    if (!index.isCompressed()) { compress(); }

    return data->At(i);
}

/**
 * Prints info abou the category.
 */
void SCategory::print() const
{
    printf("Category: %s  length=%lu  sim=%d\n", header.name.Data(), header.data_size,
           header.simulation);
    printf("  index: objects=%zu  compressed=%d\n", index.size(), index.isCompressed());
    printf("  %d objects in the category:\n", data ? data->GetEntries() : 0);
}

/**
 * Compress the category to reduce size in the memnory. After compression it is
 * not possible to add new slots.
 */
void SCategory::compress()
{
    if (!header.writable) return;

    if (data) data->Compress();
    index.compress();
}

/**
 * Clear all objects and call Clear() methods of the stored objects.
 */
void SCategory::clear()
{
    if (data) data->Clear("C");
    index.clear();
}

/**
 * Check whether the locator loc dimension fits to the container dimension
 *
 * \param loc locator
 * \return success
 */
bool SCategory::checkDim(const SLocator& loc)
{
    if (loc.getDim() != header.dim)
    {
        std::cerr << "Dimension of locator = " << loc.getDim()
                  << " does not fit to category of = " << header.dim << std::endl;
        return false;
    }
    return true;
}

/**
 * Translate n-dimension locator loc into linear i coordinate.
 *
 * \param loc locator
 * \return linear coordinate of the array
 */
int SCategory::loc2pos(const SLocator& loc)
{
    size_t pos = 0;

    for (size_t i = 0; i < header.dim; ++i)
    {
        pos += loc.at(i) * header.dim_offsets[i];
    }
    return pos;
}
