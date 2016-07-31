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

#ifndef DATASET_READER_INCLUDE_DATASET_READER_DATASET_READER_H
#define DATASET_READER_INCLUDE_DATASET_READER_DATASET_READER_H
#include <memory>
#include <string>
#include <array>

#include <feature_datamodel/datamodel.h>
#include <crick_reader_interface/crick_reader_interface.h>


enum class HitCountFileFormat { kCountMatrixTsv, kJson };

class DatasetReader : public CrickReaderInterface {
 public:
  DatasetReader(const std::string &transformation_txt_filepath,
		const std::string &hitcount_filepath,
		const HitCountFileFormat &hitcount_fileformat);
  ParsedImageAlignment parsedImageAlignment() const override;
  data_model::Everything parsedFeatures() const override;
  ~DatasetReader();

 private:
  class impl;
  std::unique_ptr<impl> pimpl_;
};

#endif   // DATASET_READER_INCLUDE_DATASET_READER_DATASET_READER_H
