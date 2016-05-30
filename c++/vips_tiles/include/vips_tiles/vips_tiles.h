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

#ifndef VIPS_TILES_INCLUDE_VIPS_TILES_VIPS_TILES_H
#define VIPS_TILES_INCLUDE_VIPS_TILES_VIPS_TILES_H

#include <tiles_spec_interface/tiles_spec_interface.h>
#include <tiles_interface/tiles_interface.h>
#include <common_typedefs/common_typedefs.h>
#include <memory>
#include <string>

class VipsTiles : public TilesSpecInterface, public TilesInterface {
public:
  VipsTiles(const std::string &image_path,
            common_typedefs::pixel_dimensions_t tile_size,
            common_typedefs::pixel_dimensions_t overlap);
  common_typedefs::TilesSpec tiles_spec() const override;
  std::vector<char> tile(common_typedefs::tile_id_t id) const override;
  ~VipsTiles();

private:
  class impl;
  std::unique_ptr<impl> pimpl_;
};

#endif   // VIPS_TILES_INCLUDE_VIPS_TILES_VIPS_TILES_H
