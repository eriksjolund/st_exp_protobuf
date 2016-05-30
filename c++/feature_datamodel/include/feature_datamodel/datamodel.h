//
// Copyright (C) 2015 st_exp_protobuf contributors
// Copyright (C) 2016 Karolinska institutet
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 

#ifndef FEATURE_DATAMODEL_INCLUDE_FEATURE_DATAMODEL_DATAMODEL_H
#define FEATURE_DATAMODEL_INCLUDE_FEATURE_DATAMODEL_DATAMODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>
#include <unordered_map>
//#include <map>
#include <limits>
#include <cstdint>
#include <cassert>
#include <vector>
#include <string.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <fstream>
#include <err.h>

namespace data_model {

typedef uint16_t gene_id_t;
typedef uint16_t spot_coord_t;

typedef uint32_t spot_id_t;
typedef uint32_t hits_t;

typedef std::pair<spot_coord_t, spot_coord_t> two_dimensional_spot_coord_t;

struct pair_hash {
  inline std::size_t
  operator()(const std::pair<spot_coord_t, spot_coord_t> &v) const {
    // Maybe it is good with prime numbers. 977 is a prime number.
    return v.first * 977 + v.second;
  }
};

struct NonZeroHit {
  spot_id_t pos_index;
  hits_t hits;
};

struct Spot {
  spot_coord_t xcoord_grid;
  spot_coord_t ycoord_grid;
  std::string barcode;
};

struct Gene {
  std::vector<NonZeroHit> non_zero_hits;
};

typedef std::unordered_map<two_dimensional_spot_coord_t, spot_id_t, pair_hash>
    spot_coord_to_spot_id_map_T;
struct Everything {
  ::std::unordered_map<std::string, gene_id_t> gene_id_map;
  std::vector<std::string> gene_names;
  spot_coord_to_spot_id_map_T spot_coord_to_spot_id_map;
  std::vector<Gene> genes;
  std::vector<Spot> spots;
};

}

#endif   // FEATURE_DATAMODEL_INCLUDE_FEATURE_DATAMODEL_DATAMODEL_H
