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

#ifndef FILE_FORMAT_CONTAINER_INCLUDE_FILE_FORMAT_CONTAINER_AFTER_HEADER_DATA_H
#define FILE_FORMAT_CONTAINER_INCLUDE_FILE_FORMAT_CONTAINER_AFTER_HEADER_DATA_H

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <file_format_common_protobuf/fileformat_common.pb.h>

class AfterHeaderData {
public:
  explicit AfterHeaderData(
      const std::vector<char> magic_file_format_identifier);
  uint64_t appendFileRegion(const char *ptr, uint64_t length);
  void setHeader(const char *ptr, uint64_t length);
  void write_result_file(const std::string &filename);

  virtual ~AfterHeaderData();

private:
  class impl;
  std::unique_ptr<impl> pimpl_;
};

void append_file_region(fileformat_common_proto::FileRegion *fileregion_builder,
                        std::vector<char> content,
                        AfterHeaderData &after_header_data);

void append_message_configure_file_region(
    fileformat_common_proto::FileRegion *fileregion_builder,
    const ::google::protobuf::Message &message,
    AfterHeaderData &after_header_data);

std::vector<char>
messageToCharVector(const ::google::protobuf::Message &message);

#endif   // FILE_FORMAT_CONTAINER_INCLUDE_FILE_FORMAT_CONTAINER_AFTER_HEADER_DATA_H
