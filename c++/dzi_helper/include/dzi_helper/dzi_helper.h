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

#ifndef DZI_HELPER_INCLUDE_DZI_HELPER_DZI_HELPER_H
#define DZI_HELPER_INCLUDE_DZI_HELPER_DZI_HELPER_H

#include <cstdint>

#include <common_typedefs/common_typedefs.h>

struct XYLevel {
  common_typedefs::tile_coord_t x;
  common_typedefs::tile_coord_t y;
  common_typedefs::level_t level;
};

common_typedefs::tile_id_t
tileId(common_typedefs::tile_coord_t x, common_typedefs::tile_coord_t y,
       common_typedefs::level_t level,
       common_typedefs::pixel_dimensions_t width,
       common_typedefs::pixel_dimensions_t height,
       common_typedefs::pixel_dimensions_t tile_size);
XYLevel xyLevel(common_typedefs::tile_id_t tile_id,
                common_typedefs::pixel_dimensions_t width,
                common_typedefs::pixel_dimensions_t height,
                common_typedefs::pixel_dimensions_t tile_size);
common_typedefs::tile_id_t
numTiles(common_typedefs::pixel_dimensions_t width,
         common_typedefs::pixel_dimensions_t height,
         common_typedefs::pixel_dimensions_t tile_size);
common_typedefs::level_t numLevels(common_typedefs::pixel_dimensions_t width,
                                   common_typedefs::pixel_dimensions_t height);

#endif   // DZI_HELPER_INCLUDE_DZI_HELPER_DZI_HELPER_H
