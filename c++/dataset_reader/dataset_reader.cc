#include <dataset_reader/dataset_reader.h>
#include <string>

#include <QTransform>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <QString>
#include <QTemporaryDir>

#include <parse_feature_json/json_parse.h>
#include <parse_count_matrix/parse_count_matrix.h>


class DatasetReader::impl {
public:
  impl(const std::string &transformation_txt_filepath,
       const std::string &hitcount_filepath,
       const HitCountFileFormat &hitcount_fileformat);
  ParsedImageAlignment parsedImageAlignment() const;
  data_model::Everything parsedFeatures() const;

private:
  //  QTemporaryDir dir_;
  const std::string transformation_txt_filepath_;
  const std::string hitcount_filepath_;
  const HitCountFileFormat hitcount_fileformat_;
};

data_model::Everything DatasetReader::impl::parsedFeatures() const {
  //  auto result = std::make_unique< data_model::Everything >();

  switch (hitcount_fileformat_) {
  case HitCountFileFormat::kJson : {
    FILE *fp;
    fp = fopen(hitcount_filepath_.c_str(), "r");
    if (fp == NULL) {
      throw std::runtime_error("Error while opening the file.\n");
    }
    return json_parse(fp);
  };
  case HitCountFileFormat::kCountMatrixTsv : {
    return parse_count_matrix(hitcount_filepath_);
  };
  }
  throw std::runtime_error("programming error" );
}

ParsedImageAlignment DatasetReader::impl::parsedImageAlignment() const {
  QFile transformation_txt_file(transformation_txt_filepath_.c_str());
  if (!transformation_txt_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString error_str = QString("could not open %1").arg(transformation_txt_filepath_.c_str());
    throw std::runtime_error(error_str.toStdString());
  }
  QTextStream stream(&transformation_txt_file);
  std::vector< double > numbers;
  while (!stream.atEnd()) {
    double num;
    stream >> num;
    if (stream.status() != QTextStream::Ok) {
      throw std::runtime_error("Parsing alignment file failed");
    }
    numbers.push_back(num);
    if (numbers.size() == 9) {
      break;
    }
  }
  if (numbers.size() != 9) {
    throw std::runtime_error("Too wrong number of numbers in alignment file");
  }
  ParsedImageAlignment res;
  std::copy_n(numbers.begin(), 9, res.alignment_matrix.begin());
  for (auto num : numbers) {
    qDebug() << "alllg " << num;
  }

  
  return res;
}

DatasetReader::impl::impl(const std::string &transformation_txt_filepath,
			  const std::string &hitcount_filepath,
			  const HitCountFileFormat &hitcount_fileformat) : transformation_txt_filepath_(transformation_txt_filepath),
									   hitcount_filepath_(hitcount_filepath),
									   hitcount_fileformat_(hitcount_fileformat)
								    
{
}

DatasetReader::DatasetReader(const std::string &transformation_txt_filepath,
			     const std::string &hitcount_filepath,
			     const HitCountFileFormat &hitcount_fileformat)
  : pimpl_(std::make_unique<DatasetReader::impl>(
						 transformation_txt_filepath,
						 hitcount_filepath,
						 hitcount_fileformat)) {}

DatasetReader::~DatasetReader() {}
ParsedImageAlignment DatasetReader::parsedImageAlignment() const {
  return pimpl_->parsedImageAlignment();
}

data_model::Everything DatasetReader::parsedFeatures() const {
  return pimpl_->parsedFeatures();
}
