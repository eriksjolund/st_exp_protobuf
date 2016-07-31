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

#include "parse_args.h"

#include <iostream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
namespace {
  QString errorText(const QCommandLineParser &parser, const QString &error_str) {
    QString error_text(error_str);
    error_text.append("\n\n");
    error_text.append(parser.helpText());
    return error_text;
  }

  void parseULongOption(const QCommandLineOption &cmdline_option, const QCommandLineParser &parser,
			uint32_t *save) {
    if (parser.isSet(cmdline_option)) {
      bool ok;
      uint32_t value = parser.value(cmdline_option).toULong(&ok);
      if (!ok) {
	const auto names{cmdline_option.names()};
	assert(names.size() == 2);
	throw std::runtime_error(qPrintable(
					    errorText(parser, QString("Could not parse the value given as --%1").arg(names[1]))));
      }
      *save = value;
    }
  }
  void parseFloatOption(const QCommandLineOption &cmdline_option, const QCommandLineParser &parser,
			float *save) {
    if (parser.isSet(cmdline_option)) {
      bool ok;
      float value = parser.value(cmdline_option).toFloat(&ok);
      if (!ok) {
	const auto names{cmdline_option.names()};
	assert(names.size() == 2);
	throw std::runtime_error(qPrintable(
					    errorText(parser, QString("Could not parse the value given as --%1").arg(names[1]))));
      }
      *save = value;
    }
  }

  HitCountFileFormat parseHitCountFileFormatString(const QString &str) {
    HitCountFileFormat result = HitCountFileFormat::kCountMatrixTsv;
    if (str == "JSON") {
      result = HitCountFileFormat::kJson;
    } else if (str == "countmatrix") {
      result = HitCountFileFormat::kCountMatrixTsv;
    } else {
      throw std::runtime_error("Unknown hitcount file type (allowed values: countmatrix, JSON)");
    }
    return result;
  }
}

ParseResult parseArgs(int argc, char *argv[], Options *options) {
  QCoreApplication app(argc, argv);
  app.setApplicationName("convert-st-prototype-data");
  app.setApplicationVersion("1.0");

  QCommandLineParser parser;
  parser.setApplicationDescription("Tool for converting the ST prototype data, st viewer proto");
  parser.addPositionalArgument("hit-count-filepath", "path to the hit count file (either count_matrix-1.tsv or aligned JSON");
  parser.addPositionalArgument("photo-filepath", "filepath to the histology photo file");
  parser.addPositionalArgument("transformation-filepath", "filepath to the transformation file");
  parser.addPositionalArgument("outputpath", "filepath to the new binary file");
  parser.addPositionalArgument("spot-circle-radius", "");

  QCommandLineOption spot_circle_radius_opt(QStringList() << "c"
					    << "spot-circle-radius",
					    "spot circle radius", "pixels", "300");
  parser.addOption(spot_circle_radius_opt);

  
  QCommandLineOption hitcount_fileformat_opt(QStringList() << "w"
					     << "hit-count-fileformat",
					     "fileformat for the  hit counts (either", "(possible values : JSON, countmatrix)", "countmatrix");

  parser.addOption(hitcount_fileformat_opt);

  
  QCommandLineOption tile_size_opt(QStringList() << "t"
				   << "tile-size",
                                   "size of tile (recommendation: (tile-size + 2 * overlap)  should be a power of 2 for efficiency", "pixels");
  parser.addOption(tile_size_opt);
  QCommandLineOption overlap_opt(QStringList() << "p"
				 << "overlap",
                                 "number of pixels to overlap", "pixels");
  parser.addOption(overlap_opt);
  qDebug() << "nn" << hitcount_fileformat_opt.names();
  const QCommandLineOption helpOption = parser.addHelpOption();
  const QCommandLineOption versionOption = parser.addVersionOption();
  if (!parser.parse(QCoreApplication::arguments())) {
    throw std::runtime_error(qPrintable(errorText(parser, parser.errorText())));
  }
  if (parser.positionalArguments().size() != 4) {
    throw std::runtime_error(qPrintable(errorText(parser, "Wrong number of arguments")));
  }
  options->hitcount_filepath = parser.positionalArguments()[0].toStdString();
  options->photo_filepath = parser.positionalArguments()[1].toStdString();
  options->transformation_txt_filepath = parser.positionalArguments()[2].toStdString();
  options->output_filepath = parser.positionalArguments()[3].toStdString();

  options->hitcount_fileformat = parseHitCountFileFormatString( parser.value(hitcount_fileformat_opt));

  if (parser.isSet(versionOption)) {
    std::cout << qPrintable(QCoreApplication::applicationName()) << " "
              << qPrintable(QCoreApplication::applicationVersion());
    return ParseResult::kExit;
  }
  if (parser.isSet(helpOption)) {
    std::cout << qPrintable(parser.helpText()) << "\n";
    return ParseResult::kExit;
  }

  parseULongOption(tile_size_opt, parser, &(options->tile_size));
  parseULongOption(overlap_opt, parser, &(options->overlap));
  parseFloatOption(spot_circle_radius_opt, parser, &(options->spot_circle_radius));
  return ParseResult::kContinue;
}
