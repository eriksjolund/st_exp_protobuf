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

#include <dzi_helper/dzi_helper.h>

#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>

namespace {
bool isPowerOfTwo(unsigned int num) { return !(num == 0) && !(num & (num - 1)); }
template <typename T> T divIntegerCeiling(T numerator, T denominator) {
  assert(std::numeric_limits<T>::is_integer);
  assert(std::is_unsigned<T>::value);
  return numerator / denominator + (numerator % denominator > 0);
}
common_typedefs::pixel_dimensions_t scaledTileSize(common_typedefs::level_t num_levels,
                                                   common_typedefs::level_t level,
                                                   common_typedefs::pixel_dimensions_t tile_size) {
  assert(std::is_unsigned<common_typedefs::level_t>::value);
  unsigned int factor = 1;
  factor = factor << (num_levels - level - 1);
  return tile_size * factor;
}
common_typedefs::tile_id_t numTilesLevel(common_typedefs::pixel_dimensions_t width,
                                         common_typedefs::pixel_dimensions_t height,
                                         common_typedefs::level_t level,
                                         common_typedefs::pixel_dimensions_t tile_size) {
  common_typedefs::level_t num_levels = numLevels(width, height);

  common_typedefs::pixel_dimensions_t scaled_tile_size =
      scaledTileSize(num_levels, level, tile_size);
  auto num_tiles =
      divIntegerCeiling(width, scaled_tile_size) * divIntegerCeiling(height, scaled_tile_size);
  assert(num_tiles <= std::numeric_limits<common_typedefs::tile_id_t>::max());
  return static_cast<common_typedefs::tile_id_t>(num_tiles);
}
}

common_typedefs::tile_id_t
tileId(common_typedefs::tile_coord_t x_coord, common_typedefs::tile_coord_t y_coord,
       common_typedefs::level_t level, common_typedefs::pixel_dimensions_t width,
       common_typedefs::pixel_dimensions_t height, common_typedefs::pixel_dimensions_t tile_size) {
  //  assert(isPowerOfTwo(tile_size));
  assert(std::is_unsigned<common_typedefs::level_t>::value);
  //  assert(isPowerOfTwo(tile_size));

  common_typedefs::tile_id_t result = 0;
  for (common_typedefs::level_t i = 0; i < level; ++i) {
    result += numTilesLevel(width, height, i, tile_size);
  }
  common_typedefs::level_t num_levels = numLevels(width, height);
  common_typedefs::pixel_dimensions_t scaled_tile_size =
      scaledTileSize(num_levels, level, tile_size);
  auto num_columns = divIntegerCeiling(width, scaled_tile_size);
  result += num_columns * y_coord + x_coord;
  return result;
}

XYLevel xyLevel(common_typedefs::tile_id_t tile_id, common_typedefs::pixel_dimensions_t width,
                common_typedefs::pixel_dimensions_t height,
                common_typedefs::pixel_dimensions_t tile_size) {
  //  assert(isPowerOfTwo(tile_size));
  assert(tile_id < numTiles(width, height, tile_size));
  common_typedefs::level_t num_levels = numLevels(width, height);
  common_typedefs::tile_id_t tile_count = 0;
  common_typedefs::level_t level = 0;
  while (true) {
    auto tiles_in_level = numTilesLevel(width, height, level, tile_size);
    if (tile_count + tiles_in_level > tile_id) {
      break;
    }
    tile_count += tiles_in_level;
    if (level + 1 == num_levels) {
      break;
    }
    ++level;
  }
  common_typedefs::pixel_dimensions_t scaled_tile_size =
      scaledTileSize(num_levels, level, tile_size);
  auto num_columns = divIntegerCeiling(width, scaled_tile_size);
  auto tiles_current_level = tile_id - tile_count;
  common_typedefs::tile_coord_t x = tiles_current_level % num_columns;
  common_typedefs::tile_coord_t y = tiles_current_level / num_columns;
  return XYLevel{x, y, level};
}

common_typedefs::tile_id_t numTiles(common_typedefs::pixel_dimensions_t width,
                                    common_typedefs::pixel_dimensions_t height,
                                    common_typedefs::pixel_dimensions_t tile_size) {
  //  assert(isPowerOfTwo(tile_size));
  auto num_levels = numLevels(width, height);
  common_typedefs::tile_id_t sum = 0;
  for (common_typedefs::level_t level = 0; level < num_levels; ++level) {
    sum += numTilesLevel(width, height, level, tile_size);
  }
  return sum;
}

common_typedefs::level_t numLevels(common_typedefs::pixel_dimensions_t width,
                                   common_typedefs::pixel_dimensions_t height) {
  assert(width > 0);
  assert(height > 0);
  return static_cast<common_typedefs::level_t>(std::ceil(log2(std::max(width, height)))) + 1;
}
