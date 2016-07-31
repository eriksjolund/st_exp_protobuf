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

#ifndef COMMON_TYPEDEFS_INCLUDE_COMMON_TYPEDEFS_COMMON_TYPEDEFS_H
#define COMMON_TYPEDEFS_INCLUDE_COMMON_TYPEDEFS_COMMON_TYPEDEFS_H

#include <cstdint>
#include <utility>
#include <limits>

namespace common_typedefs {

typedef uint16_t gene_id_t;


// The type spot_coord_t was first meant to be used as a logical grid
// coordinate.  A natural choice for this would be to use
// integers (for instance uint16_t).
//
// The file formats of the datasets provided under these URLs
// http://www.spatialtranscriptomicsresearch.org/wp-content/uploads/2016/07/Rep1_MOB_count_matrix-1.tsv
// http://www.spatialtranscriptomicsresearch.org/wp-content/uploads/2016/07/Rep1_MOB_aligned.json_.gz
//
// do not contain any information about logical grid coordinates but instead
// provide the image pixel coordinates of the spots.
//
// To be able to parse those file formats without changing to much
// of the code we let spot_coord_t be a float.



 typedef float spot_coord_t;



typedef uint32_t spot_id_t;
typedef uint32_t hits_t;

typedef std::pair<spot_coord_t, spot_coord_t> two_dimensional_spot_coord_t;

// Typedefs related to the image tiles

typedef uint32_t tile_coord_t;
typedef uint32_t tile_id_t;
typedef uint32_t level_t;
typedef uint32_t pixel_dimensions_t;

// TODO: investigate this further. Do we need to require is_iec559?
static_assert(std::numeric_limits<float>::is_iec559,
              "IEC 559/IEEE 754 floating-point types are required");

typedef float physical_dimensions_t;

struct TilesSpec {
  TilesSpec(common_typedefs::pixel_dimensions_t whole_image_width,
            common_typedefs::pixel_dimensions_t whole_image_height,
            common_typedefs::pixel_dimensions_t tile_size,
            common_typedefs::pixel_dimensions_t overlap)
      : whole_image_width_(whole_image_width),
        whole_image_height_(whole_image_height), tile_size_(tile_size),
        overlap_(overlap) {}

  common_typedefs::pixel_dimensions_t whole_image_width() const {
    return whole_image_width_;
  };
  common_typedefs::pixel_dimensions_t whole_image_height() const {
    return whole_image_height_;
  };
  common_typedefs::pixel_dimensions_t tile_size() const { return tile_size_; };
  common_typedefs::pixel_dimensions_t overlap() const { return overlap_; };

private:
  common_typedefs::pixel_dimensions_t whole_image_width_ = 0;
  common_typedefs::pixel_dimensions_t whole_image_height_ = 0;
  common_typedefs::pixel_dimensions_t tile_size_ = 0;
  common_typedefs::pixel_dimensions_t overlap_ = 0;
};
}

#endif   // COMMON_TYPEDEFS_INCLUDE_COMMON_TYPEDEFS_COMMON_TYPEDEFS_H
