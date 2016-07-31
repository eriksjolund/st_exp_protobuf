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

#ifndef CREATE_ST_EXP_PROTOBUF_PARSE_ARGS_H
#define CREATE_ST_EXP_PROTOBUF_PARSE_ARGS_H

#include <QtGlobal>
#include <QCommandLineParser>

#include <common_typedefs/common_typedefs.h>

#include <dataset_reader/dataset_reader.h>

enum class ParseResult { kExit, kContinue };

struct Options {
  common_typedefs::pixel_dimensions_t tile_size = 510;
  common_typedefs::pixel_dimensions_t overlap = 1;
  float spot_circle_radius = 0;
  std::string output_filepath;
  std::string photo_filepath;
  std::string hitcount_filepath;
  std::string transformation_txt_filepath;
  HitCountFileFormat hitcount_fileformat = HitCountFileFormat::kCountMatrixTsv;
};

ParseResult parseArgs(int argc, char *argv[], Options *options);

#endif   // CREATE_ST_EXP_PROTOBUF_PARSE_ARGS_H
