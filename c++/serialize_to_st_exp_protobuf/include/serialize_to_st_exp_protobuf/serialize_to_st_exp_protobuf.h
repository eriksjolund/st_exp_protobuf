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

#ifndef SERIALIZE_TO_ST_EXP_PROTOBUF_INCLUDE_SERIALIZE_TO_ST_EXP_PROTOBUF_SERIALIZE_TO_ST_EXP_PROTOBUF_H
#define SERIALIZE_TO_ST_EXP_PROTOBUF_INCLUDE_SERIALIZE_TO_ST_EXP_PROTOBUF_SERIALIZE_TO_ST_EXP_PROTOBUF_H
#ifndef PROTOBUF_GENES_SERIALIZE_H
#define PROTOBUF_GENES_SERIALIZE_H

#include <string>
#include <crick_reader_interface/crick_reader_interface.h>
#include <tiles_interface/tiles_interface.h>
#include <tiles_spec_interface/tiles_spec_interface.h>
#include <common_typedefs/common_typedefs.h>

namespace data_model {
struct Everything;
}

void serialize_to_st_exp_protobuf(
    const CrickReaderInterface *crick_reader_interface,
    const std::vector<TilesSpecInterface *> &tiles_spec_iface_vec,
    const std::vector<TilesInterface *> &tiles_iface_vec,
    const std::string &filename, common_typedefs::pixel_dimensions_t tile_size,
    common_typedefs::pixel_dimensions_t overlap);

#endif

#endif   // SERIALIZE_TO_ST_EXP_PROTOBUF_INCLUDE_SERIALIZE_TO_ST_EXP_PROTOBUF_SERIALIZE_TO_ST_EXP_PROTOBUF_H
