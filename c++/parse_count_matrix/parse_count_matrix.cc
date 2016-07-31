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

// Parts of this file has been copied from
// https://github.com/lloyd/yajl/blob/2.1.0/reformatter/json_reformat.c
// that has this license:

/*
 * Copyright (c) 2007-2014, Lloyd Hilaiel <me@lloyd.io>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <parse_count_matrix/parse_count_matrix.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>
#include <unordered_map>
#include <limits>
#include <cstdint>
#include <cassert>
#include <vector>
#include <string.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <fstream>
#include <err.h>

#include <QString>
#include <QStringList>
#include <QFile>
#include <QIODevice>

#include <QByteArray>
#include <QDebug>
#include <QRegExp>

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <feature_datamodel/datamodel.h>

QStringList parseGeneNames(QIODevice &qiodevice) {
  if (qiodevice.atEnd()) {
    throw std::runtime_error("wrong format of count matrix file (empty file)");
  }
  QByteArray line = qiodevice.readLine();
  QString line_str(line);
  line_str.remove( QRegExp("[\\n\\r]+$")); // Remove trailing \n and \r
  return line_str.split('\t', QString::SkipEmptyParts);
}

struct SpotAndHitCounts {
  QString pixel_coordinate_as_string;
  std::vector< data_model::hits_t > hit_counts;
};

std::vector< SpotAndHitCounts > parseSpotsAndHitCounts(QIODevice &qiodevice) {
  std::vector< SpotAndHitCounts > result;
  while (!qiodevice.atEnd()) {
    SpotAndHitCounts entry;
    QString line = QString(qiodevice.readLine());

    line.remove( QRegExp("[\\n\\r]+$")); // Remove trailing \n and \r
    QStringList str_list = line.split('\t', QString::SkipEmptyParts);
    if (str_list.empty()) {
      throw std::runtime_error("wrong format of count matrix file (empty list)");
    }
    entry.pixel_coordinate_as_string = str_list.takeFirst();
    for (auto &str : str_list) {
      bool ok;
      int num = str.toInt(&ok);
      if (!ok ||
	  std::numeric_limits<data_model::hits_t>::min() > num ||
	  std::numeric_limits<data_model::hits_t>::max() < num
	  ) {
	throw std::runtime_error("wrong format of count matrix file (number out of range)");
      }
      data_model::hits_t hit_count = static_cast<data_model::hits_t>(num);
      entry.hit_counts.push_back(hit_count);
    }
    result.push_back(entry);

  }
  return result;
}

data_model::spot_coord_t parse_spot_coord_number(const QString &str) {
  bool ok;
  float num = str.toFloat(&ok);
  if (!ok) {
      throw std::runtime_error("Could not parse spot coord");
  }
  if (std::numeric_limits<data_model::spot_coord_t>::min() > num ||
      std::numeric_limits<data_model::spot_coord_t>::max() < num) {
      throw std::runtime_error("spot coord out of range");
  }
  return static_cast<data_model::spot_coord_t>(num);    
}

data_model::Everything parse_count_matrix_from_qiodevice(QIODevice &qiodevice) {
  data_model::Everything everything;
  QStringList gene_names = parseGeneNames(qiodevice);
  std::vector< SpotAndHitCounts > spots_and_hit_counts = parseSpotsAndHitCounts(qiodevice);
  {
    data_model::gene_id_t gene_id = 0;
    for (auto &gene_name : gene_names) {
      everything.gene_names.push_back(qPrintable(gene_name));
      everything.gene_id_map[qPrintable(gene_name)] = gene_id;
      everything.genes.push_back(data_model::Gene{});
      ++gene_id;
    }
  }
  {
    data_model::spot_id_t spot_id = 0;
    for (auto &spots_and_hit_count : spots_and_hit_counts) {
      QStringList list = spots_and_hit_count.pixel_coordinate_as_string.split('x');
      if (list.size() != 2) {
	throw std::runtime_error("Could not parse pixel coordinates");
      }
      data_model::spot_coord_t x_coord = parse_spot_coord_number(list[0]);
      data_model::spot_coord_t y_coord = parse_spot_coord_number(list[1]);    

      data_model::two_dimensional_spot_coord_t two_dimensional_coord(x_coord, y_coord);
      everything.spot_coord_to_spot_id_map[two_dimensional_coord] = spot_id;
      everything.spots.push_back(data_model::Spot{ x_coord, y_coord, "" /*empty barcode*/});
      {
	data_model::gene_id_t gene_id = 0;
	for (auto &hit_count : spots_and_hit_count.hit_counts) {
	  if (hit_count != 0) {
	    everything.genes[gene_id].non_zero_hits.push_back(data_model::NonZeroHit{spot_id, hit_count});
	  }	
	  ++gene_id;
	}
      }
      ++spot_id;
    }

  }
  return everything;
}

data_model::Everything parse_count_matrix(const std::string &filepath) {
  QFile count_matrix_file(filepath.c_str());
  if (!count_matrix_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString error_str = QString("could not open %1").arg(filepath.c_str());
    throw std::runtime_error(error_str.toStdString());
  }
  return parse_count_matrix_from_qiodevice(count_matrix_file);
}
