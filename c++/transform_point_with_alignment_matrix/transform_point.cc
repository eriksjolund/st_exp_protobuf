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

#include <transform_point_with_alignment_matrix/transform_point.h>

#include <common_typedefs/common_typedefs.h>

#include <QDebug>
#include <QTransform>
#include <QPointF>

QPointF translate_point(const std::array<double, 9> &alignment_matrix,
                        common_typedefs::spot_coord_t xcoord,
                        common_typedefs::spot_coord_t ycoord) {
  std::vector<qreal> qreal_vec;
  for (const auto &a : alignment_matrix) {
    qreal_vec.push_back(static_cast<qreal>(a));
  }
  QTransform transform(qreal_vec[0], qreal_vec[3], qreal_vec[6], qreal_vec[1], qreal_vec[4],
                       qreal_vec[7], qreal_vec[2], qreal_vec[5], qreal_vec[8]);
  {
    // Bug reported 13 March 2016
    // https://bugreports.qt.io/browse/QTBUG-51837
    // type() should not be needed here.
    QTransform::TransformationType notused = transform.type();
  }
  return transform.map(QPointF(xcoord, ycoord));
}
