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

namespace {
QString errorText(const QCommandLineParser &parser, const QString &error_str) {
  QString error_text(error_str);
  error_text.append("\n\n");
  error_text.append(parser.helpText());
  return error_text;
}

void parseULongOption(const QCommandLineOption &cmdline_option, const QCommandLineParser &parser,
                      const QString &name, uint32_t *save) {
  if (parser.isSet(cmdline_option)) {
    bool ok;
    uint32_t value = parser.value(cmdline_option).toULong(&ok);

    if (!ok) {
      throw std::runtime_error(qPrintable(
          errorText(parser, QString("Could not parse the value given as %1").arg(name))));
    }
    *save = value;
  }
}
}

ParseResult parseArgs(int argc, char *argv[], Options *options) {
  QCoreApplication app(argc, argv);
  app.setApplicationName("convert-st-prototype-data");
  app.setApplicationVersion("1.0");

  QCommandLineParser parser;
  parser.setApplicationDescription("Tool for converting the ST prototype data, st viewer proto");
  parser.addPositionalArgument("crickpath", "path to the crick file");
  parser.addPositionalArgument("outputpath", "filepath to the new binary file");
  QCommandLineOption tile_size_opt(QStringList() << "t"
                                                 << "tile-size",
                                   "size of tile (recommendation: (tile-size + 2 * overlap)  should be a power of 2 for efficiency", "pixels");
  parser.addOption(tile_size_opt);
  QCommandLineOption overlap_opt(QStringList() << "p"
                                               << "overlap",
                                 "number of pixels to overlap", "pixels");
  parser.addOption(overlap_opt);
  const QCommandLineOption helpOption = parser.addHelpOption();
  const QCommandLineOption versionOption = parser.addVersionOption();
  if (!parser.parse(QCoreApplication::arguments())) {
    throw std::runtime_error(qPrintable(errorText(parser, parser.errorText())));
  }
  if (parser.positionalArguments().size() != 2) {
    throw std::runtime_error(qPrintable(errorText(parser, "Wrong number of arguments")));
  }
  options->crick_filepath = parser.positionalArguments()[0].toStdString();
  options->output_filepath = parser.positionalArguments()[1].toStdString();
  if (parser.isSet(versionOption)) {
    std::cout << qPrintable(QCoreApplication::applicationName()) << " "
              << qPrintable(QCoreApplication::applicationVersion());
    return kExit;
  }
  if (parser.isSet(helpOption)) {
    std::cout << qPrintable(parser.helpText()) << "\n";
    return kExit;
  }

  parseULongOption(tile_size_opt, parser, "--tile-size", &(options->tile_size));
  parseULongOption(overlap_opt, parser, "--overlap", &(options->overlap));
  return kContinue;
}
