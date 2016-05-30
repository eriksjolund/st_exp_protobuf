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

#include <reorder_feature_datamodel/reorder.h>

std::vector<size_t> indexes_ordered(const std::vector<size_t> &values) {
  std::vector<size_t> indices(values.size());
  std::iota(begin(indices), end(indices), static_cast<size_t>(0));
  std::sort(begin(indices), end(indices),
            [&](size_t a, size_t b) { return (values[a] < values[b]); });
  return indices;
}

// Currently the sorting order is first row-wise and the column-wise.
// We might get better compression characteristics of the experiment data
// by ordering the tiles in this scheme instead:
//
// http://wiki.openstreetmap.org/wiki/QuadTiles
//
// The advantage would be that a continuous range of spot IDs would
// identify spots that are more closely grouped together (physically).
// As the distance between the spots in general are smaller, we could assume
// that also the gene hit counts would look more similar.
//
// Sorting with C++11
// http://stackoverflow.com/a/10581051/757777
std::vector<size_t> spots_ordered(const std::vector<data_model::Spot> &values) {
  std::vector<size_t> indices(values.size());
  std::iota(begin(indices), end(indices), static_cast<size_t>(0));
  std::sort(begin(indices), end(indices), [&](size_t a, size_t b) {
    if (values[a].ycoord_grid != values[b].ycoord_grid) {
      return (values[a].ycoord_grid < values[b].ycoord_grid);
    }
    assert(values[a].xcoord_grid != values[b].xcoord_grid);
    return values[a].xcoord_grid < values[b].xcoord_grid;

  });
  return indices;
}

data_model::Everything reorder_spots(const std::vector<size_t> &ordered_spot_ids,
                                     const data_model::Everything &everything_input) {
  data_model::Everything everything = everything_input;
  std::vector<size_t> index_of_indexes(indexes_ordered(ordered_spot_ids));
  for (auto &gene : everything.genes) {
    std::vector<data_model::NonZeroHit> non_zero_hits_reordered;
    for (const auto &non_zero_hit : gene.non_zero_hits) {
      non_zero_hits_reordered.push_back(data_model::NonZeroHit{
          static_cast<data_model::spot_id_t>(index_of_indexes[non_zero_hit.pos_index]),
          non_zero_hit.hits});
    }
    sort(non_zero_hits_reordered.begin(), non_zero_hits_reordered.end(),
         [](const data_model::NonZeroHit &a, const data_model::NonZeroHit &b)
             -> bool { return a.pos_index < b.pos_index; });
    gene.non_zero_hits = non_zero_hits_reordered;
  }
  std::vector<data_model::Spot> spots_reordered;
  for (auto &iter : ordered_spot_ids) {
    const auto &spot(everything.spots[iter]);
    spots_reordered.push_back(spot);
  }
  everything.spots = spots_reordered;
  return everything;
}
