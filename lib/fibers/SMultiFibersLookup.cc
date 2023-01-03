// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SMultiFibersLookup.h"

#include <cassert>    // for assert
#include <inttypes.h> // for SCNu8

/**
 * \class SFibersLookupTable
\ingroup lib_fibers

A unpacker task.

\sa STask
*/

/**
 *
 *
 *
 *
 */
void tokenize(std::string str, std::vector<std::string> &token_v, char delimiter = '/'){
    size_t start = str.find_first_not_of(delimiter), end=start;
    while (start != std::string::npos) {
        // Find next occurence of delimiter
        end = str.find(delimiter, start);
        // Push back the token found into vector
        token_v.push_back(str.substr(start, end-start));
        // Skip all occurences of the delimiter to find new start
        start = str.find_first_not_of(delimiter, end);
    }
}
/**
 * buffer contains only the max four fiber addresses
 *
 *
 */
uint SMultiFibersChannel::read(const char* buffer)
{
    std::string strBuffer = std::string(buffer);
    std::replace( strBuffer.begin(), strBuffer.end(), ')', '\0');
    std::vector<std::string> segment;
    tokenize(strBuffer, segment, '(');
    for(int i=0; i < segment.size(); ++i) {
            std::vector<std::string> token;
            tokenize(segment[i], token, ',');
            if(segment[i].find(",,,") == std::string::npos) continue;
            if(token[0]!="" && token[1]!="" && token[2]!="" && token[3]!="") {
                vecFiberAssociations.push_back(token);
            }
    }
    return 0; //unused, just for backwards compatibility
}

uint SMultiFibersChannel::write(char* buffer, size_t n) const
{
//    uint cnt = snprintf(buffer, n, "%3d  %3d  %3d   %c", m, l, s, side);
//    if (cnt < 0) return cnt;
//    if (cnt < n) return 0;
//    return cnt;
    return 0;
}

void SMultiFibersChannel::print(bool newline, const char* prefix) const
{
    for(int i=0; i < vecFiberAssociations.size(); ++i) {
        for(int j=0; j < vecFiberAssociations[i].size(); ++j) {
            printf("%s ", vecFiberAssociations[i][j].c_str() );
        }
    }
    printf("\n");
}

uint64_t SMultiFibersChannel::quickHash() const
{
//    return SLookupChannel::quickHash() | (uint64_t)side << 32;
    return 0;
}

void SMultiFibersChannel::fromHash(uint64_t hash)
{
//    SLookupChannel::fromHash(hash);
//    side = hash >> 32 & 0xff;
}
