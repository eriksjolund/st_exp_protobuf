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

#include <cstdint>
#include <stdexcept>

#include <iostream>
#include <dzi_helper/dzi_helper.h>
#include <vips_tiles/vips_tiles.h>
#include "parse_args.h"
#include <crick_reader/crick_reader.h>
#include <tiles_spec_interface/tiles_spec_interface.h>
#include <tiles_interface/tiles_interface.h>
#include <stdlib.h>
#include <serialize_to_capnproto/capnproto_genes_serialize.h>

class VipsTilesContainer {
public:
  void add_vips_tile(const std::string &image_path, common_typedefs::pixel_dimensions_t tile_size,
                     common_typedefs::pixel_dimensions_t overlap) {
    vips_tiles_vec_.push_back(std::make_unique<VipsTiles>(image_path, tile_size, overlap));
  }
  std::vector<TilesInterface *> get_tiles_vec() {
    std::vector<TilesInterface *> res;
    for (auto &vips_tiles : vips_tiles_vec_) {
      res.push_back(vips_tiles.get());
    }
    return res;
  }
  std::vector<TilesSpecInterface *> get_tiles_spec_vec() {
    std::vector<TilesSpecInterface *> res;
    for (auto &vips_tiles : vips_tiles_vec_) {
      res.push_back(vips_tiles.get());
    }
    return res;
  }

private:
  std::vector<std::unique_ptr<VipsTiles>> vips_tiles_vec_;
};

int main(int argc, char *argv[]) {
  try {
    Options options;
    ParseResult parse_result = parseArgs(argc, argv, &options);
    if (parse_result == kExit) {
      return EXIT_SUCCESS;
    }
    CrickReader crick_reader(options.crick_filepath);
    ParsedImageAlignment imagA = crick_reader.parsedImageAlignment();

    CrickReaderInterface *crick_reader_interface = &crick_reader;

    capnproto_genes_serialize(crick_reader_interface, vips_tiles_container.get_tiles_spec_vec(),
                              vips_tiles_container.get_tiles_vec(), options.output_filepath);
  } catch (std::exception &e) {
    std::cerr << "exception caught: " << e.what() << '\n';
  }
}

VipsTilesContainer vips_tiles_container;

vips_tiles_container.add_vips_tile(imagA.red_image, options.tile_size, options.overlap);
vips_tiles_container.add_vips_tile(imagA.blue_image, options.tile_size, options.overlap);
