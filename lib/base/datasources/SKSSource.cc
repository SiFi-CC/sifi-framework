
// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SKSSource.h"
#include "SUnpacker.h"

#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>

static Int_t static_var = 5;
static Int_t nSamples;

/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
SKSSource::SKSSource(uint16_t subevent) : SDataSource()
    , subevent(subevent), input(), istream(), buffer_size(0)
{
}

/**
 * Open source and init respective unpacker.
 *
 * \return success
 */
bool SKSSource::open()
{
//     istream.open(input.c_str(), std::ios::binary);
    //std::ifstream input(path_name);
    istream.open(input.c_str(), std::ios::in);
//     istream.open(input);
    if (!istream.is_open()) {
        std::cerr << "##### Error in SKSSource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }

//     std::string csvLine = " ";
//     std::string tmp = " ";
    //static Int_t nSamples;

    nSamples = getNSamples();
    std::cout << "\n\n-----------" << std::endl;
    std::cout << "Number of samples: " << nSamples << std::endl;
    std::cout <<  "----------\n" << std::endl;
// 
//     
//     istream >> tmp >> tmp >> nSamples;
//     std::getline(istream, csvLine);
//     std::getline(istream, csvLine);
//     std::getline(istream, csvLine);
// 
//     std::cout << "\n\n-----------------------------------------" << std::endl;
//     std::cout << "Number of samples: " << nSamples << std::endl;
//     std::cout <<  "-----------------------------------------\n" << std::endl;
//     
    
    
    if (unpackers.size() == 0)
        return false;

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
    
        bool res = unpackers[subevent]->init();
        if (!res) {
            printf("Forced unpacker %#x not initalized\n", subevent);
            abort();
        }
    }
    else
    {
        std::map<uint16_t, SUnpacker *>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
        {
            bool res = iter->second->init();
            if (!res) {
                printf("Unpacker %#x not initalized\n", iter->first);
                abort();
            }
        }
    }
    return true;
}

bool SKSSource::close()
{
    if (subevent != 0x0000)
    {
        if (unpackers[subevent])
            unpackers[subevent]->finalize();
        else
            abort();
    }
    else
    {
        std::map<uint16_t, SUnpacker *>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
            iter->second->finalize();
    }

    istream.close();
    return true;
}

bool SKSSource::readCurrentEvent()
{
    if (unpackers.size() == 0)
    return false;
    
// Version 1 - csvRow is a vector    
    std::string csvLine  = " ";
    std::string tmp      = " ";
    std::vector <std::string> csvRow;
    std::string csvElement;
    void* buffer[nSamples*sizeof(float)];
    for (Int_t i = 0; i < nSamples; i++)
    {
        csvRow.clear();
        getline(istream, csvLine);
        std::stringstream stream(csvLine);
        
        while(getline(stream, csvElement, ','))
        {
            csvRow.push_back(csvElement);
        }
        buffer[i] = (void*)&(csvRow.at(1));
        std::cout << buffer[i] << " " << &buffer[i] << std::endl;
        //std::cout << csvRow.at(0) << " " << csvRow.at(1) << " " << csvRow.at(2) << " " << csvRow.at(3) << " " << csvRow.at(4) << std::endl;
    }
//     buffer_size = nSamples;

    
// Version 2 - csvRow is a Float_t   
//     Int_t osc_ch  = 1; //change to parameter from params.txt
//     std::string csvLine  = " ";
//     void* buffer[buffer_size];
//     Float_t csvRow[5];
//     std::string csvElement;
//     float csvSignal[nSamples];
// 
//     for (Int_t i = 0; i < nSamples; i++)
//     {
//         //csvRow.clear();
//         std::getline(istream, csvLine);
//         std::stringstream stream(csvLine);
//         int j = 0;
//         while(j<5)
//         {
//             std::getline(stream, csvElement, ',');
//             //csvRow[j] = std::stof(csvElement);
//             j++;
//         }
//                 
//         csvSignal[i] =static_cast<float>(csvRow[osc_ch]);
// //         buffer[i] = &csvSignal[i]; 
//         buffer[i] = static_cast<void*>(&csvSignal[i]);
//         std::cout << buffer[i] << " " << &buffer[i] << std::endl; //type 
//     }

//     void* buffer[buffer_size];
//     istream.read((char*)&buffer, buffer_size);
    
    
    
    
    bool flag = istream.good();

    if (!flag)
        return false;

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
        // TODO must pass event number to the execute
        unpackers[subevent]->execute(0, 0, subevent, buffer, buffer_size);
    }
    else
    {
        for (const auto & u : unpackers)
            u.second->execute(0, 0, u.first, buffer, buffer_size);
    }

    return true;
}

/**
 * Set input for the source.
 *
 * \param filename input file name
 * \param length length of buffer to read
 */
void SKSSource::setInput(const std::string& filename, size_t length) {
    input = filename;
    buffer_size = length;
}

int SKSSource::getNSamples()
{
    std::string csvLine = " ";
    std::string tmp = " ";
    static Int_t nnSamples = 0;
    istream >> tmp >> tmp >> nnSamples;
    std::getline(istream, csvLine);
    std::getline(istream, csvLine);
    std::getline(istream, csvLine);
    std::cout << "\n\n-----------------------------------------" << std::endl;
    std::cout << "Number of samples: " << nnSamples << std::endl;
    std::cout <<  "-----------------------------------------\n" << std::endl;
    return nnSamples;
}
