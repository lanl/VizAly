/*
 *                    Copyright (C) 2015, UChicago Argonne, LLC
 *                               All Rights Reserved
 *
 *                               Generic IO (ANL-15-066)
 *                     Hal Finkel, Argonne National Laboratory
 *
 *                              OPEN SOURCE LICENSE
 *
 * Under the terms of Contract No. DE-AC02-06CH11357 with UChicago Argonne,
 * LLC, the U.S. Government retains certain rights in this software.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the names of UChicago Argonne, LLC or the Department of Energy
 *      nor the names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 * *****************************************************************************
 *
 *                                  DISCLAIMER
 * THE SOFTWARE IS SUPPLIED “AS IS” WITHOUT WARRANTY OF ANY KIND.  NEITHER THE
 * UNTED STATES GOVERNMENT, NOR THE UNITED STATES DEPARTMENT OF ENERGY, NOR
 * UCHICAGO ARGONNE, LLC, NOR ANY OF THEIR EMPLOYEES, MAKES ANY WARRANTY,
 * EXPRESS OR IMPLIED, OR ASSUMES ANY LEGAL LIABILITY OR RESPONSIBILITY FOR THE
 * ACCURACY, COMPLETENESS, OR USEFULNESS OF ANY INFORMATION, DATA, APPARATUS,
 * PRODUCT, OR PROCESS DISCLOSED, OR REPRESENTS THAT ITS USE WOULD NOT INFRINGE
 * PRIVATELY OWNED RIGHTS.
 *
 * *****************************************************************************
 */

#include <GenericIO.h>
#include <utils/octree.hpp>

#include <stdint.h>
#include <sstream>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <string.h>

template <class T>
void read_gio(char* file_name, std::string var_name, T*& data, int field_count)
{
    gio::GenericIO reader(file_name);
    reader.openAndReadHeader(gio::GenericIO::MismatchAllowed);
    int num_ranks = reader.readNRanks();
    uint64_t max_size = 0;
    uint64_t rank_size[num_ranks];
    for (int i = 0; i < num_ranks; ++i)
    {
        rank_size[i] = reader.readNumElems(i);
        if (max_size < rank_size[i])
            max_size = rank_size[i];
    }
    T* rank_data = new T[max_size * field_count + reader.requestedExtraSpace() / sizeof(T)];
    int64_t offset = 0;
    reader.addScalarizedVariable(var_name, rank_data, field_count,
                                 gio::GenericIO::VarHasExtraSpace);
    for (int i = 0; i < num_ranks; ++i)
    {
        reader.readData(i, false);
        std::copy(rank_data, rank_data + rank_size[i]*field_count, data + offset);
        offset += rank_size[i] * field_count;
    }
    delete [] rank_data;
    reader.close();
}


template <class T>
void read_gio_rank( gio::GenericIO reader, int rank, std::string var_name, T*& data, int field_count)
{
    int num_ranks = reader.readNRanks();
    uint64_t max_size = reader.readNumElems(rank);
    if (rank >= num_ranks)
        return;

    data = new T[max_size * field_count + reader.requestedExtraSpace() / sizeof(T)];
    reader.addScalarizedVariable(var_name, data, field_count, gio::GenericIO::VarHasExtraSpace);

    reader.readDataSection(rank, false);
    reader.close();
}


template <class T>
void read_gio_rankLeaf( gio::GenericIO reader, int rank, int leaf, std::string var_name, T*& data, int field_count)
{
    int num_ranks = reader.readNRanks();
    uint64_t max_size = reader.readNumElems(rank);
    if (rank >= num_ranks)
        return;

    GIOOctree tempOctree = reader.getOctree();
    size_t num_particles = tempOctree.getCount(rank, leaf);
    size_t offset = tempOctree.getOffset(rank, leaf);

    data = new T[max_size * num_particles + reader.requestedExtraSpace() / sizeof(T)];
    reader.addScalarizedVariable(var_name, data, num_particles, gio::GenericIO::VarHasExtraSpace);

    reader.readDataSection(offset, num_particles, rank, false);
    reader.close();
}



extern "C" int64_t get_elem_num(char* file_name);

extern "C" void read_gio_float (char* file_name, char* var_name, float* data, int field_count);
extern "C" void read_gio_double(char* file_name, char* var_name, double* data, int field_count);
extern "C" void read_gio_int32 (char* file_name, char* var_name, int* data, int field_count);
extern "C" void read_gio_int64 (char* file_name, char* var_name, int64_t* data, int field_count);
enum var_type
{
    float_type = 0,
    double_type = 1,
    int32_type = 2,
    int64_type = 3,
    type_not_found = 9,
    var_not_found = 10
};
extern "C" var_type get_variable_type(char* file_name, char* var_name);
extern "C" int get_variable_field_count(char* file_name, char* var_name);
extern "C" void inspect_gio(char* file_name);

extern "C" int64_t get_num_variables(char* file_name);
extern "C" char* get_variable(char* file_name, int var);
extern "C" char* get_octree(char* file_name);
extern "C" int* get_octree_leaves(char* file_name, int extents[]);
extern "C" int get_num_octree_leaves(char* file_name, int extents[]);
