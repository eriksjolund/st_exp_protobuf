#include <crick_reader/crick_reader.h>
#include <string>

#include <QTransform>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <QString>
#include <QTemporaryDir>

#include <@Qt5Core_VERSION_STRING@/QtGui/private/qzipreader_p.h>

#include <parse_feature_json/json_parse.h>

namespace {
std::array<double, 9> parseAlignmentMatrix(const QJsonArray &matrix) {
  std::vector<double> vec;
  for (const auto &entry : matrix) {
    if (!entry.isDouble()) {
      QString error_str =
          QString("could not read double number from \"alignment_matrix\". The "
                  "string was %1").arg(entry.toString());
      throw std::runtime_error(qPrintable(error_str));
    }
    vec.push_back(entry.toDouble());
  }
  std::array<double, 9> array;
  std::copy_n(vec.begin(), 9, array.begin());
  return array;
}
std::string parseImageFilepath(const QString &name, const QTemporaryDir &tmpdir,
                               const QJsonObject &obj) {
  QString result = tmpdir.path() + QDir::separator() + obj[name].toString();
  return result.toStdString();
}
QString combinePath(const QString &name, const QTemporaryDir &tmpdir) {
  QString path = tmpdir.path() + QDir::separator() + name;
  return path;
}
}

class CrickReader::impl {
public:
  impl(const QString &filepath);
  ParsedImageAlignment parsedImageAlignment() const;
  data_model::Everything parsedFeatures() const;

private:
  QTemporaryDir dir_;
};

data_model::Everything CrickReader::impl::parsedFeatures() const {
  //  auto result = std::make_unique< data_model::Everything >();
  FILE *fp;
  fp = fopen(qPrintable(combinePath(QString("features.json"), dir_)), "r");
  if (fp == NULL) {
    throw std::runtime_error("Error while opening the file.\n");
  }
  return json_parse(fp);
}

ParsedImageAlignment CrickReader::impl::parsedImageAlignment() const {
  QString path = combinePath(QString("imagealignment.json"), dir_);
  QFile loadFile(path);
  if (!loadFile.open(QIODevice::ReadOnly)) {
    QString error_str = QString("could not open %1").arg(path);
    throw std::runtime_error(error_str.toStdString());
  }
  QByteArray byte_array = loadFile.readAll();
  QJsonDocument json_doc(QJsonDocument::fromJson(byte_array));
  if (!json_doc.isObject()) {
    QString error_str = QString("wrong format in %1").arg(path);
    throw std::runtime_error(error_str.toStdString());
  }
  auto obj = json_doc.object();
  ParsedImageAlignment res;
  res.alignment_matrix =
      parseAlignmentMatrix(obj["alignment_matrix"].toArray());
  res.red_image = parseImageFilepath("figure_red", dir_, obj);
  res.blue_image = parseImageFilepath("figure_blue", dir_, obj);
  return res;
}

CrickReader::impl::impl(const QString &filepath) {
  dir_.setAutoRemove(true);
  QZipReader zip_reader(filepath);
  if (dir_.isValid()) {
    if (zip_reader.extractAll(dir_.path())) {
      zip_reader.close();
    } else {
      throw std::runtime_error("QZipReader::extractAll() failed");
    }
  } else {
    throw std::runtime_error("temp dir is not valid");
  }
}

CrickReader::CrickReader(const std::string &filepath)
    : pimpl_(std::make_unique<CrickReader::impl>(
          QString::fromStdString(filepath))) {}

CrickReader::~CrickReader() {}
ParsedImageAlignment CrickReader::parsedImageAlignment() const {
  return pimpl_->parsedImageAlignment();
}

data_model::Everything CrickReader::parsedFeatures() const {
  return pimpl_->parsedFeatures();
}
