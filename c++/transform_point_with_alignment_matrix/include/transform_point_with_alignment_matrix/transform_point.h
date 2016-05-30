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

#ifndef TRANSFORM_POINT_WITH_ALIGNMENT_MATRIX_INCLUDE_TRANSFORM_POINT_WITH_ALIGNMENT_MATRIX_TRANSFORM_POINT_H
#define TRANSFORM_POINT_WITH_ALIGNMENT_MATRIX_INCLUDE_TRANSFORM_POINT_WITH_ALIGNMENT_MATRIX_TRANSFORM_POINT_H

#include <array>
#include <QTransform>
#include <QPointF>

#include <common_typedefs/common_typedefs.h>

QPointF translate_point(const std::array<double, 9> &alignment_matrix,
                        common_typedefs::spot_coord_t xcoord,
                        common_typedefs::spot_coord_t ycoord);

#endif   // TRANSFORM_POINT_WITH_ALIGNMENT_MATRIX_INCLUDE_TRANSFORM_POINT_WITH_ALIGNMENT_MATRIX_TRANSFORM_POINT_H
