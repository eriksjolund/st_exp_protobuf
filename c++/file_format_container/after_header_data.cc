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

#include <file_format_container/after_header_data.h>
#include <file_format_common_protobuf/fileformat_common.pb.h>
#include <limits>
#include <vector>
#include <cassert>
#include <QFile>

#include <QFileDevice>
#include <QByteArray>
#include <QTemporaryFile>
#include <QDebug>
namespace {
void writePaddingZeroes(QFileDevice &file_device) {
  qint64 pos = file_device.pos();
  qint64 zeroes_to_add = 8 - (pos % 8);
  if (zeroes_to_add == 8) {
    // It was already aligned.
    zeroes_to_add = 0;
  }

  file_device.write(QByteArray(zeroes_to_add, '\0'));
}
void write_byte_array(const QByteArray bytearray, QIODevice &iodevice) {
  qint64 bytes_written = iodevice.write(bytearray);
  if (bytes_written != bytearray.size()) {
    throw std::runtime_error("could not write to io device");
  }
}

void write_char_vector(const std::vector<char> content, QIODevice &iodevice) {
  write_byte_array(QByteArray(&content[0], content.size()), iodevice);
}
}

std::vector<char> messageToCharVector(const ::google::protobuf::Message &message) {
  std::vector<char> result;
  result.resize(message.ByteSize());
  message.SerializeToArray(&result[0], result.size());
  return result;
}

void append_file_region(fileformat_common_proto::FileRegion *fileregion_builder,
                        std::vector<char> content, AfterHeaderData &after_header_data) {
  assert(fileregion_builder);
  assert(content.size() > 0);
  uint64_t position = after_header_data.appendFileRegion(&content[0], content.size());
  fileregion_builder->set_regionoffset(position);
  fileregion_builder->set_regionsize(content.size());
}

void append_message_configure_file_region(fileformat_common_proto::FileRegion *fileregion_builder,
                                          const ::google::protobuf::Message &message,
                                          AfterHeaderData &after_header_data) {
  append_file_region(fileregion_builder, messageToCharVector(message), after_header_data);
}

class AfterHeaderData::impl {
public:
  impl(const std::vector<char> magic_file_format_identifier);
  // Returns the position of where the message was written.
  // The zeroes are written before message for padding.

  void setHeader(const char *ptr, uint64_t length);
  void write_result_file(const std::string &filename);

  uint64_t appendFileRegion(const char *ptr, uint64_t length);

private:
  bool result_file_was_written_ = false;
  const std::string magic_fileformat_identifier_;
  QByteArray header_;
  QTemporaryFile data_;
  const std::vector<char> magic_file_format_identifier_;
};

AfterHeaderData::impl::impl(const std::vector<char> magic_file_format_identifier)
    : magic_file_format_identifier_(magic_file_format_identifier) {
  assert(magic_file_format_identifier_.size() % 8 == 0); // Easy way to get word-alignedness
  assert(magic_file_format_identifier_.size() != 0); // We should use a magic file format identifier
  data_.setAutoRemove(true);
  data_.open();
}

uint64_t AfterHeaderData::impl::appendFileRegion(const char *ptr, uint64_t length) {
  writePaddingZeroes(data_);
  qint64 pos = data_.pos();
  typedef uint64_t conversion_target;
  assert(length <= std::numeric_limits<conversion_target>::max());
  assert(length >= std::numeric_limits<conversion_target>::min());
  data_.write(ptr, static_cast<conversion_target>(length));
  typedef uint64_t conversion_target2;
  assert(pos <= std::numeric_limits<conversion_target2>::max());
  assert(pos >= std::numeric_limits<conversion_target2>::min());
  return static_cast<conversion_target2>(pos);
}

void AfterHeaderData::impl::setHeader(const char *ptr, uint64_t length) {
  assert(length < std::numeric_limits<int>::max());
  int header_size = static_cast<int>(length);
  header_ = QByteArray(ptr, header_size);
}

void AfterHeaderData::impl::write_result_file(const std::string &filename) {
  assert(!result_file_was_written_);
  assert(!header_.isEmpty());
  QFile file(filename.c_str());
  if (file.exists()) {
    throw std::runtime_error("file already exists");
  }
  if (file.open(QIODevice::ReadWrite)) {
    write_char_vector(magic_file_format_identifier_, file);
    fileformat_common_proto::HeaderSize header_size_message;
    header_size_message.set_headersize(header_.size());
    qDebug() << "size of preheader (headersize) = "
             << messageToCharVector(header_size_message).size();

    qDebug() << "size of header = " << header_.size();
    write_char_vector(messageToCharVector(header_size_message), file);
    writePaddingZeroes(file);
    write_byte_array(header_, file);
    writePaddingZeroes(file);
    data_.seek(0); // Rewind to the start of the file
    qint64 data_size = data_.size();
    qint64 chunk_size = 4096;
    char read_buffer[chunk_size];
    qint64 total_bytes_read = 0;
    while (total_bytes_read < data_size) {
      qint64 bytes_read = data_.read(&read_buffer[0], chunk_size);
      if (bytes_read <= 0) {
        throw std::runtime_error("error reading file regions");
      }
      qint64 bytes_written3 = file.write(&read_buffer[0], bytes_read);
      if (bytes_written3 != bytes_read) {
        throw std::runtime_error("error writing file regions");
      }
      total_bytes_read += bytes_read;
    }
  } else {
    throw std::runtime_error("could not create file");
  }
  result_file_was_written_ = true;
  return;
}

AfterHeaderData::AfterHeaderData(const std::vector<char> magic_file_format_identifier)
    : pimpl_(new impl(std::move(magic_file_format_identifier))) {}

uint64_t AfterHeaderData::appendFileRegion(const char *ptr, uint64_t length) {
  pimpl_->appendFileRegion(ptr, length);
}

void AfterHeaderData::setHeader(const char *ptr, uint64_t length) {
  pimpl_->setHeader(ptr, length);
}

void AfterHeaderData::write_result_file(const std::string &filepath) {
  pimpl_->write_result_file(filepath);
}

AfterHeaderData::~AfterHeaderData() {}
