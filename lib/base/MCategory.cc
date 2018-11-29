// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $MAPTSYS/LICENSE.                         *
 * For the list of contributors see $MAPTSYS/README/CREDITS.             *
 *************************************************************************/

#include <iostream>

#include <TBuffer.h>
#include <TClass.h>
#include <TClonesArray.h>

#include "MCategory.h"

/** \class MCategory
\ingroup lib_base

A Category abstract interface

*/

/// null object
TObject * pNullMCategoryPtr = nullptr;

/// Default consructor
MCategory::MCategory() : data(nullptr)
{
    header.clear();
    index.clear();
}

/** Constructor
 * \param name name of the category
 * \param dim number of dimensions
 * \param sizes array of sizes of dimensions
 * \param simulation set tru if category for simulation data
 */
MCategory::MCategory(const char * name, size_t dim, size_t * sizes, bool simulation) : data(nullptr)
{
    header.clear();
    index.clear();
    setup(name, dim, sizes, simulation);

    header.writable = kTRUE;
}

/// Destructor
MCategory::~MCategory()
{
    if (data) delete data;
}

/** Setup category
 * \param name name of the category
 * \param dim number of dimensions
 * \param sizes array of sizes of dimensions
 * \param simulation set true if category for simulation data
 */
void MCategory::setup(const char * name, size_t dim, size_t * sizes, bool simulation)
{
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
        header.dim_offsets[i] = header.dim_offsets[i-1] * sizes[i-1];
        header.dim_sizes[i] = sizes[i];
        header.data_size *= sizes[i];
    }

    data = new TClonesArray(name, header.data_size);

    printf("Category %s created with linear size of %ld\n", name, header.data_size);
}

/** Returns the object at the address from n locator.
 * Dimension of n must match dimension of the container.
 * \param n locator
 * \return pointer to an object
 */
// TObject * MCategory::operator[](const MLocator & n)
// {
//     if (!checkDim(header.dim))     return nullptr;
// 
//     size_t pos = loc2pos(n);
//     Int_t p = index.getMapIndex(pos);
//     if (p < 0) return nullptr;
//     return data->ConstructedAt(p);
// }

/** Returns a reference to an object at the address from locator n.
 * Dimension of n must match dimension of the container.
 * \param n locator object
 * \return reference to a slot
 */
TObject *& MCategory::getSlot(const MLocator & n)
{
    if (!checkDim(n))     return pNullMCategoryPtr;

    size_t pos = loc2pos(n);
    if (!index.setMapIndex(pos, pos))
    {
        std::cerr << "Category " << header.name << " was already compressed, can't add new slots." << std::endl;
        return pNullMCategoryPtr;
    }

    if (!getObject(n))  ++entries;

    return data->operator[](pos);
}

/** Create a new slot at the end of the array and returns it reference.
 * Awailable only for 1d categories.
 * \bug assure that it really creates new slot, not n+1 which migh overlap
 * with existing slot.
 * \return reference to a slot
 */
TObject *& MCategory::getNewSlot()
{
    if (header.dim != 1)
    {
        std::cerr << "getNewSlot allowed only for linear categories." << std::endl;
        return pNullMCategoryPtr;
    }

    MLocator loc(1);
    loc[0] = data->GetEntries();
    return getSlot(loc);
}

/** Returns an object at the address from locator n.
 * Dimension of n must match dimension of the container.
 * \param n locator object
 * \return pointer to the object
 */
TObject * MCategory::getObject(const MLocator & n)
{
    if (!checkDim(n))     return pNullMCategoryPtr;

    size_t pos = loc2pos(n);
    Int_t p = index.getMapIndex(pos);
    if (p < 0) return nullptr;
    return data->At(p);
}

/** Returns an object at the index i.
 * \param i index
 * \return pointer to the object
 */
TObject * MCategory::getObject(Int_t i)
{
    return data->At(i);
}

/**
 * Prints info abou the category.
 */
void MCategory::print() const
{
    printf("Category: %s  length=%ld  sim=%d\n", header.name.Data(), header.data_size, header.simulation);
    printf("  index: objects=%ld  compressed=%d\n", index.size(), index.isCompressed());
    printf("  %d objects in the category:\n", data->GetEntries());
}

/**
 * Compress the category to reduce size in the memnory. After compression it is
 * not possible to add new slots.
 */
void MCategory::compress()
{
    if (!header.writable)   return;

    data->Compress();
    index.compress();
}

/**
 * Clear all objects and call Clear() methods of the stored objects.
 */
void MCategory::clear()
{
    if (data) data->Clear("C");
    index.clear();
}

/** Check whether the locator loc dimension fits to the container dimension
 * \param loc locator
 * \return success
 */
bool MCategory::checkDim(const MLocator& loc)
{
    if (loc.getDim() != header.dim)
    {
        std::cerr << "Dimension of locator = " << loc.getDim() << " does not fit to category of = " << header.dim << std::endl;
        return false;
    }
    return true;
}

/** Translate n-dimension locator loc into linear i coordinate.
 * \param loc locator
 * \return linear coordinate of the array
 */
int MCategory::loc2pos(const MLocator& loc)
{
    size_t pos = 0;

    for (size_t i = 0; i < header.dim; ++i)
    {//printf("[%d] loc_at=%d   dim_siz=%d\n", i, loc.at(i), header.dim_sizes[i]);
        pos += loc.at(i) * header.dim_offsets[i];
    }//printf("pos=%d\n", pos);
    return pos;
}

void MCategory::Streamer(TBuffer &R__b)
{
    // Stream an object of class HLinearCategory.
    Char_t clase[200];
    if (R__b.IsReading())
    {
        Version_t R__v = R__b.ReadVersion();
        TObject::Streamer(R__b);
        header.Streamer(R__b);
        index.Streamer(R__b);
        R__b.ReadString(clase,200);
        if ( data && strcmp(clase,data->GetClass()->GetName())==0)
            data->Clear();
        else {
            delete data;
            data = new TClonesArray(clase);
        }
        data->Streamer(R__b);
        R__b >> entries;
    } else {
        R__b.WriteVersion(MCategory::IsA());
        TObject::Streamer(R__b);
        header.Streamer(R__b);
        index.Streamer(R__b);
        strcpy(clase, data->GetClass()->GetName());
        R__b.WriteString(clase);
        data->Streamer(R__b);
        R__b << entries;
    }
}


ClassImp(MCategory);
