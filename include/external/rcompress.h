#ifndef RCOMPRESS_H
#define RCOMPRESS_H

// The following file contains code copied and modified from https://github.com/raysan5/raylib/blob/master/src/rcore.c
// It is under the following license:
// Copyright (c) 2013-2025 Ramon Santamaria (@raysan5)

// This software is provided "as-is", without any express or implied warranty. In no event 
// will the authors be held liable for any damages arising from the use of this software.

// Permission is granted to anyone to use this software for any purpose, including commercial 
// applications, and to alter it and redistribute it freely, subject to the following restrictions:

//   1. The origin of this software must not be misrepresented; you must not claim that you 
//   wrote the original software. If you use this software in a product, an acknowledgment 
//   in the product documentation would be appreciated but is not required.

//   2. Altered source versions must be plainly marked as such, and must not be misrepresented
//   as being the original software.

//   3. This notice may not be removed or altered from any source distribution.


#include "stdio.h"
#include "stdlib.h"

#define SINFL_IMPLEMENTATION
#define SINFL_NO_SIMD
#include "sinfl.h"

#define SDEFL_IMPLEMENTATION
#include "sdefl.h"

#define MAX_DECOMPRESSION_SIZE 64 // Maximum size allocated for decompression in MB


// Compress data (DEFLATE algorithm)
unsigned char *CompressData(const unsigned char *data, int dataSize, int *compDataSize)
{
    #define COMPRESSION_QUALITY_DEFLATE  8

    unsigned char *compData = NULL;


    // Compress data and generate a valid DEFLATE stream
    struct sdefl *sdefl = calloc(1, sizeof(struct sdefl));   // WARNING: Possible stack overflow, struct sdefl is almost 1MB
    int bounds = sdefl_bound(dataSize);
    compData = (unsigned char *)calloc(bounds, 1);

    *compDataSize = sdeflate(sdefl, compData, data, dataSize, COMPRESSION_QUALITY_DEFLATE);   // Compression level 8, same as stbiw
    free(sdefl);

    // TRACELOG(LOG_INFO, "SYSTEM: Compress data: Original size: %i -> Comp. size: %i", dataSize, *compDataSize);


    return compData;
}

// Decompress data (DEFLATE algorithm)
unsigned char *DecompressData(const unsigned char *compData, int compDataSize, int *dataSize)
{
    unsigned char *data = NULL;


    // Decompress data from a valid DEFLATE stream
    data = (unsigned char *)calloc(MAX_DECOMPRESSION_SIZE*1024*1024, 1);
    int length = sinflate(data, MAX_DECOMPRESSION_SIZE*1024*1024, compData, compDataSize);

    // WARNING: RL_REALLOC can make (and leave) data copies in memory, be careful with sensitive compressed data!
    // TODO: Use a different approach, create another buffer, copy data manually to it and wipe original buffer memory
    unsigned char *temp = (unsigned char *)realloc(data, length);

#ifdef TLOG_H
    if (temp != NULL) data = temp;
    else TLog("SYSTEM: Failed to re-allocate required decompression memory\n");
#endif

    *dataSize = length;

    // TRACELOG(LOG_INFO, "SYSTEM: Decompress data: Comp. size: %i -> Original size: %i", compDataSize, *dataSize);


    return data;
}

#endif