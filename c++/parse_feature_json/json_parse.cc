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

#include <parse_feature_json/json_parse.h>
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

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

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <zlib.h>

#include <feature_datamodel/datamodel.h>

namespace {
int my_strncmp(const unsigned char *s1, const char *s2, size_t n) {
  return strncmp(reinterpret_cast<const char *>(s1), s2, n);
}

struct ParserContext {
  enum class State { X_COORD, Y_COORD, HITS, BARCODE, GENE, NONE };
  data_model::Everything *data_model;
  State state = State::NONE;
  data_model::spot_coord_t tmp_x;
  data_model::spot_coord_t tmp_y;
  std::string tmp_barcode;
  data_model::hits_t tmp_hits;
  data_model::gene_id_t tmp_gene_id;
};

ParserContext *getContext(void *ctx) {
  assert(ctx);
  return static_cast<ParserContext *>(ctx);
}

ParserContext::State getState(const unsigned char *stringVal, size_t stringLen) {
  if (stringLen == 1) {
    if (*stringVal == 'x') {
      return ParserContext::State::X_COORD;
    }
    if (*stringVal == 'y') {
      return ParserContext::State::Y_COORD;
    }
  }
  if (stringLen == 4) {
    if (my_strncmp(stringVal, "hits", 4) == 0) {
      return ParserContext::State::HITS;
    }
    if (my_strncmp(stringVal, "gene", 4) == 0) {
      return ParserContext::State::GENE;
    }
  }
  if (stringLen == 7 && my_strncmp(stringVal, "barcode", 7) == 0) {
    return ParserContext::State::BARCODE;
  }
  throw std::runtime_error("Not recognized json map key");

  assert(false);
}

static int reformat_null(void * /* ctx */) {
  assert(false);
  return true;
}

static int reformat_boolean(void * /* ctx */, int /* boolean */) {
  assert(false);
  return true;
}

static int reformat_number(void *ctx, const char *s, size_t l) {
  auto context(getContext(ctx));
  assert(context->state != ParserContext::State::NONE);
  unsigned long num = std::stoul(std::string(reinterpret_cast<const char *>(s), l));
  assert(num >= 0);
  if (context->state == ParserContext::State::X_COORD ||
      context->state == ParserContext::State::Y_COORD) {
    assert(std::numeric_limits<data_model::spot_coord_t>::max() >= num);
    if (context->state == ParserContext::State::X_COORD) {
      context->tmp_x = num;
      return true;
    }
    if (context->state == ParserContext::State::Y_COORD) {
      context->tmp_y = num;
      return true;
    }
  }
  if (context->state == ParserContext::State::HITS) {
    assert(std::numeric_limits<data_model::hits_t>::max() >= num);
    context->tmp_hits = num;
    return true;
  }
  assert(false);
  return true;
}

static int reformat_string(void *ctx, const unsigned char *stringVal, size_t stringLen) {
  auto context(getContext(ctx));
  assert(context->state != ParserContext::State::NONE);
  auto &data_model(*(context->data_model));
  if (context->state == ParserContext::State::BARCODE) {
    context->tmp_barcode = std::string(reinterpret_cast<const char *>(stringVal), stringLen);
    return true;
  }
  if (context->state == ParserContext::State::GENE) {
    std::string gene(reinterpret_cast<const char *>(stringVal), stringLen);
    auto &gene_id_map(data_model.gene_id_map);
    auto it = gene_id_map.find(gene);
    if (it == gene_id_map.end()) {
      const auto len = data_model.gene_names.size();
      context->tmp_gene_id = len;
      gene_id_map[gene] = len;
      data_model.gene_names.push_back(gene);
    } else {
      context->tmp_gene_id = gene_id_map[gene];
    }
    return true;
  }
  assert(false);
  return true;
}

static int reformat_map_key(void *ctx, const unsigned char *stringVal, size_t stringLen) {
  assert(stringLen > 0);
  auto context(getContext(ctx));
  context->state = getState(stringVal, stringLen);
  assert(context->state != ParserContext::State::NONE);
  return true;
}

static int reformat_start_map(void * /* ctx */) { return true; }

inline data_model::spot_id_t
find_or_create_spot(const data_model::spot_coord_t &x_coord,
                    const data_model::spot_coord_t &y_coord, const std::string &barcode,
                    data_model::spot_coord_to_spot_id_map_T &spot_coord_to_spot_id_map,
                    std::vector<data_model::Spot> &spots) {
  data_model::two_dimensional_spot_coord_t two_dimensional_coord(x_coord, y_coord);
  auto it = spot_coord_to_spot_id_map.find(two_dimensional_coord);
  if (it == spot_coord_to_spot_id_map.end()) {
    assert(spots.size() == spot_coord_to_spot_id_map.size());
    data_model::spot_id_t current_spot = spots.size();
    spots.push_back(data_model::Spot{x_coord, y_coord, barcode});
    spot_coord_to_spot_id_map[two_dimensional_coord] = current_spot;
    return current_spot;
  } else {
    data_model::spot_id_t current_spot = it->second;
    if (spots[current_spot].barcode != barcode) {
      throw std::runtime_error("json has already had this coordinate but then "
                               "it was another barcode");
    }
    return current_spot;
  }
}

static int reformat_end_map(void *ctx) {
  auto context(getContext(ctx));
  auto &coord_map(context->data_model->spot_coord_to_spot_id_map);
  auto &spots(context->data_model->spots);
  //  GeneHits gene_hits(context->tmp_gene_id, context->tmp_hits);
  data_model::spot_id_t current_spot =
      find_or_create_spot(context->tmp_x, context->tmp_y, context->tmp_barcode, coord_map, spots);
  auto &genes(context->data_model->genes);
  if (context->data_model->genes.size() == context->tmp_gene_id) {
    genes.push_back(data_model::Gene());
  }
  assert(context->tmp_gene_id < context->data_model->genes.size());
  assert(context->tmp_gene_id < genes.size());
  assert(context->tmp_gene_id >= 0);
  if (context->tmp_hits != 0) {
    genes[context->tmp_gene_id].non_zero_hits.push_back(
        data_model::NonZeroHit{current_spot, context->tmp_hits});
  }
  return true;
}

static int reformat_start_array(void * /* ctx */) { return true; }

static int reformat_end_array(void * /* ctx */) { return true; }

static yajl_callbacks callbacks = {reformat_null, reformat_boolean, NULL, NULL, reformat_number,
                                   reformat_string, reformat_start_map, reformat_map_key,
                                   reformat_end_map, reformat_start_array, reformat_end_array};

void checkStatus(const yajl_status &status, yajl_gen g, yajl_handle handle,
                 const unsigned char *jsonText, size_t jsonTextLength) {
  if (status != yajl_status_ok) {
    unsigned char *str = yajl_get_error(handle, 1, jsonText, jsonTextLength);
    std::string error_str(reinterpret_cast<const char *>(str));
    yajl_free_error(handle, str);
    yajl_gen_free(g);
    yajl_free(handle);
    throw std::runtime_error(error_str);
  }
}

class JsonParse {
public:
  JsonParse();
  ~JsonParse();
  data_model::Everything json_parse(FILE *file);

private:
  yajl_handle hand;
  yajl_gen g;
};

JsonParse::JsonParse() {}
JsonParse::~JsonParse() {
  yajl_gen_free(g);
  yajl_free(hand);
}
data_model::Everything JsonParse::json_parse(FILE *file) {
  data_model::Everything everything;

  ParserContext parser_context;
  parser_context.data_model = &everything;

  hand = yajl_alloc(&callbacks, NULL, (void *)&parser_context);

  /* generator config */
  size_t rd;
  g = yajl_gen_alloc(NULL);

  yajl_status stat;
  yajl_gen_config(g, yajl_gen_beautify, 1);
  yajl_gen_config(g, yajl_gen_validate_utf8, 1);
  /* ok.  open file.  let's read and parse */

  /* and let's allow comments by default */
  yajl_config(hand, yajl_allow_comments, 1);
  /* check arguments.*/

  static unsigned char fileData[65536];

  for (;;) {
    rd = fread((void *)fileData, 1, sizeof(fileData) - 1, file);

    if (rd == 0) {
      if (!feof(file)) {
        throw std::runtime_error("error on file read.\n");
      }
      break;
    }
    fileData[rd] = 0;

    stat = yajl_parse(hand, fileData, rd);
    if (stat != yajl_status_ok)
      break;
    {
      const unsigned char *buf;
      size_t len;
      yajl_gen_get_buf(g, &buf, &len);
      fwrite(buf, 1, len, stdout);
      yajl_gen_clear(g);
    }
  }
  stat = yajl_complete_parse(hand);
  if (stat != yajl_status_ok) {
    unsigned char *str = yajl_get_error(hand, 1, fileData, rd);
    std::string error_str(reinterpret_cast<const char *>(str));
    yajl_free_error(hand, str);
    throw std::runtime_error(error_str);
  }
  return everything;
}
}

data_model::Everything json_parse(FILE *f) {
  JsonParse jp;
  return jp.json_parse(f);
}
