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

#include <vips_tiles/vips_tiles.h>

#include <cassert>
#include <string>
#include <vector>
#include <QProcess>
#include <QTemporaryDir>

#include <QDomDocument>
#include <QFile>
#include <QDomNodeList>
#include <QDomElement>
#include <QDebug>
#include <QString>
#include <QImage>
#include <common_typedefs/common_typedefs.h>

#include <dzi_helper/dzi_helper.h>

namespace {

common_typedefs::pixel_dimensions_t getValue(const QDomElement &elem, const QString &name) {
  if (!elem.hasAttribute(name)) {
    throw std::runtime_error(qPrintable(QString("failed to find attribute: ") + name));
  }
  bool ok;
  int val = elem.attribute(name).toInt(&ok);
  if (!ok) {
    throw std::runtime_error(qPrintable(QString("failed to get value: ") + name));
  }
  return val;
}

common_typedefs::TilesSpec readXML(const QString &filename) {
  QDomDocument doc;
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)) {
    throw std::runtime_error("failed to parse dom");
  }

  auto elem = doc.documentElement().firstChildElement("Size");
  if (elem.isNull()) {
    throw std::runtime_error("failed to find elem \"Size\"");
  }

  return common_typedefs::TilesSpec{getValue(elem, "Width"), getValue(elem, "Height"),
                                    getValue(doc.documentElement(), "TileSize"),
                                    getValue(doc.documentElement(), "Overlap")};
}

common_typedefs::TilesSpec get_tiles_spec(const QString &image_path,
                                          common_typedefs::pixel_dimensions_t tile_size,
                                          common_typedefs::pixel_dimensions_t overlap) {
  QImage image(image_path);
  if (image.isNull()) {
    throw std::runtime_error("Image could not be loaded from the specified path");
  }
  return common_typedefs::TilesSpec(image.width(), image.height(), tile_size, overlap);
}
}

class VipsTiles::impl {
public:
  impl(const QString &image_path, common_typedefs::pixel_dimensions_t tile_size,
       common_typedefs::pixel_dimensions_t overlap)
      : tiles_spec_(get_tiles_spec(image_path, tile_size, overlap)) {
    // TODO: Check if we need to set permissions explictly for this dir.
    // No other user should have access to the directory
    dir_.setAutoRemove(true);
    if (dir_.isValid()) {
      QString outpath = dir_.path() + QDir::separator() + "aaa";
      QStringList vips_arguments{
          "dzsave", image_path, outpath, "--layout", "dz", "--overlap", QString::number(overlap),
          "--tile-size", QString::number(tile_size), "--container", "fs", "--suffix=.jpg[Q=85]"};
      //
      qDebug() << "vips command line arguments:" << vips_arguments;
      int ret = QProcess::execute("vips", vips_arguments);
      if (ret == 0) {
        tiles_spec_ = readXML(outpath + ".dzi");
        assert(tiles_spec_.tile_size() == tile_size);
        assert(tiles_spec_.overlap() == overlap);
      } else {
        throw std::runtime_error("vips command failed");
      }
    }
  }
  virtual ~impl() {}

  common_typedefs::TilesSpec tiles_spec() const { return tiles_spec_; }

  QByteArray tile(common_typedefs::tile_id_t tile_id) const {
    XYLevel xy_level = xyLevel(tile_id, tiles_spec_.whole_image_width(),
                               tiles_spec_.whole_image_height(), tiles_spec_.tile_size());
    QString outpath = dir_.path() + QDir::separator() + "aaa" + "_files" + QDir::separator() +
                      QString("%1%2%3_%4.jpg")
                          .arg(xy_level.level)
                          .arg(QDir::separator())
                          .arg(xy_level.x)
                          .arg(xy_level.y);
    QFile file(outpath);
    if (!file.open(QIODevice::ReadOnly)) {
      throw std::runtime_error(qPrintable(outpath));
    }
    QByteArray file_content = file.readAll();
    return file_content;
  }

private:
  QTemporaryDir dir_;
  common_typedefs::TilesSpec tiles_spec_;
};

VipsTiles::VipsTiles(const std::string &image_path, common_typedefs::pixel_dimensions_t tile_size,
                     common_typedefs::pixel_dimensions_t overlap)
    : pimpl_(std::make_unique<VipsTiles::impl>(QString::fromStdString(image_path), tile_size,
                                               overlap)) {}

VipsTiles::~VipsTiles() {}

common_typedefs::TilesSpec VipsTiles::tiles_spec() const { return pimpl_->tiles_spec(); }

std::vector<char> VipsTiles::tile(common_typedefs::tile_id_t id) const {
  QByteArray array = pimpl_->tile(id);
  return std::vector<char>(array.begin(), array.end());
}
