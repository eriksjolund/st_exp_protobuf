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
#include "parse_args.h"
#include <crick_reader/crick_reader.h>
#include <stdlib.h>
#include <serialize_to_st_exp_protobuf/serialize_to_st_exp_protobuf.h>

#include "vips_tiles_container.h"

int main(int argc, char *argv[]) {
  try {
    Options options;
    ParseResult parse_result = parseArgs(argc, argv, &options);
    if (parse_result == kExit) {
      return EXIT_SUCCESS;
    }
    CrickReader crick_reader(options.crick_filepath);
    CrickReaderInterface *crick_reader_interface = &crick_reader;
    ParsedImageAlignment imagA = crick_reader.parsedImageAlignment();
    const std::vector<std::string> image_filepaths = {imagA.red_image, imagA.blue_image};
    VipsTilesContainer vips_tiles_container{image_filepaths, options.tile_size, options.overlap};

    serialize_to_st_exp_protobuf(crick_reader_interface, vips_tiles_container.get_tiles_spec_vec(),
                                 vips_tiles_container.get_tiles_vec(), options.output_filepath,
                                 options.tile_size, options.overlap);
  } catch (std::exception &e) {
    std::cerr << "exception caught: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
