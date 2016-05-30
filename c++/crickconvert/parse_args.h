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

#ifndef CRICKCONVERT_PARSE_ARGS_H
#define CRICKCONVERT_PARSE_ARGS_H

#include <QtGlobal>
#include <QCommandLineParser>

#include <common_typedefs/common_typedefs.h>

enum ParseResult { kExit, kContinue };

struct Options {
  common_typedefs::pixel_dimensions_t tile_size = 254;
  common_typedefs::pixel_dimensions_t overlap = 1;
  std::string crick_filepath;
  std::string output_filepath;
};

ParseResult parseArgs(int argc, char *argv[], Options *options);

#endif   // CRICKCONVERT_PARSE_ARGS_H
