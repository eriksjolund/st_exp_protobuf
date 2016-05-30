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

#ifndef REORDER_FEATURE_DATAMODEL_INCLUDE_REORDER_FEATURE_DATAMODEL_REORDER_H
#define REORDER_FEATURE_DATAMODEL_INCLUDE_REORDER_FEATURE_DATAMODEL_REORDER_H

#include <algorithm>
#include <map>
#include <vector>
#include <feature_datamodel/datamodel.h>

std::vector<size_t> indexes_ordered(const std::vector<size_t> &values);

std::vector<size_t> spots_ordered(const std::vector<data_model::Spot> &values);

template <typename T>
std::vector<size_t> sort_non_zero_hits(
    const std::vector<data_model::NonZeroHit> &values,
    const std::vector<size_t> &index_of_index,
    const data_model::spot_coord_to_spot_id_map_T &spot_coord_to_spot_id_map);

data_model::Everything
reorder_spots(const std::vector<size_t> &ordered_spot_indexes,
              const data_model::Everything &everything_input);

#endif   // REORDER_FEATURE_DATAMODEL_INCLUDE_REORDER_FEATURE_DATAMODEL_REORDER_H
