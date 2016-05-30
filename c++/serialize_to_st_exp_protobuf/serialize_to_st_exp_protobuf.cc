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

#include <serialize_to_st_exp_protobuf/serialize_to_st_exp_protobuf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>
#include <map>
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
#include <iostream>

#include <algorithm>

#include <QPointF>
#include <QFile>
#include <QByteArray>

#include <feature_datamodel/datamodel.h>

#include <reorder_feature_datamodel/reorder.h>
#include <crick_reader_interface/crick_reader_interface.h>
#include <dzi_helper/dzi_helper.h>
#include <transform_point_with_alignment_matrix/transform_point.h>
#include <sstream>
#include <cmath>

#include <QTemporaryFile>
#include <QImage>
#include <QDebug>
#include <file_format_container/after_header_data.h>

#include <file_format_st_exp_protobuf/file_format_st_exp_protobuf.h>

#include <file_format_common_protobuf/fileformat_common.pb.h>
#include <st_exp_protobuf/fullsize_image.pb.h>
#include <st_exp_protobuf/st_exp.pb.h>
#include <st_exp_protobuf/tiled_image.pb.h>

namespace {

// First element represent hits for gene with geneid 0
// Second element represent hits for gene with geneid 1
// and so forth.
typedef std::vector<data_model::hits_t> SpotHit;

void build_spots(const std::vector<data_model::Spot> &spots,
                 const std::array<double, 9> &alignment_matrix, ::st_exp_proto::Spots &spots_pb) {

  for (auto &iter : spots) {
    auto spot_pb = spots_pb.add_spots();
    spot_pb->set_barcode(iter.barcode);
    spot_pb->set_xcoordgrid(iter.xcoord_grid);
    spot_pb->set_ycoordgrid(iter.ycoord_grid);

    // TODO: fix this. This function should not be dependent of
    // QPointF (Qt5Gui). Rewrite translate_point.
    QPointF spot_in_physical_coordinates =
        translate_point(alignment_matrix, iter.xcoord_grid, iter.ycoord_grid);
    spot_pb->set_xcoordphyscial(
        static_cast<common_typedefs::physical_dimensions_t>(spot_in_physical_coordinates.x()));
    spot_pb->set_ycoordphyscial(
        static_cast<common_typedefs::physical_dimensions_t>(spot_in_physical_coordinates.y()));
  };
}
std::vector<char> readFileIntoCharVector(const std::string &filepath) {
  QFile file(filepath.c_str());
  if (!file.open(QIODevice::ReadOnly)) {
    throw std::runtime_error("Could not open file in the function readFileIntoCharVector");
  }
  QByteArray byte_array = file.readAll();
  return std::vector<char>{byte_array.begin(), byte_array.end()};
}

std::vector<char> create_serialized_spots(const std::vector<data_model::Spot> &spots,
                                          const std::array<double, 9> &alignment_matrix) {
  ::st_exp_proto::Spots spots_pb;
  build_spots(spots, alignment_matrix, spots_pb);
  return messageToCharVector(spots_pb);
}
void build_gene_names(const std::vector<std::string> &gene_names,
                      ::st_exp_proto::GeneNames &gene_names_pb) {
  for (auto &iter : gene_names) {
    gene_names_pb.add_genenames(iter);
  };
}
std::vector<char> create_gene_names(const std::vector<std::string> &gene_names) {
  ::st_exp_proto::GeneNames gene_names_pb;
  build_gene_names(gene_names, gene_names_pb);
  return messageToCharVector(gene_names_pb);
}
std::vector<char> create_gene_hit(const std::vector<std::string> &gene_names) {
  ::st_exp_proto::GeneNames gene_names_pb;
  build_gene_names(gene_names, gene_names_pb);
  return messageToCharVector(gene_names_pb);
}

void build_genehit(const data_model::Gene &gene, ::st_exp_proto::GeneHit &genehit_pb) {
  data_model::spot_id_t current_spot_id = 0;
  for (const auto &iter : gene.non_zero_hits) {
    genehit_pb.add_zerohitsskipped(iter.pos_index - current_spot_id);
    current_spot_id = iter.pos_index + 1;
    genehit_pb.add_hits(iter.hits);
  }
}

std::vector<char> create_genehit(const data_model::Gene &gene) {
  ::st_exp_proto::GeneHit gene_hit_pb;
  build_genehit(gene, gene_hit_pb);
  return messageToCharVector(gene_hit_pb);
}

void buildPhoto(::fullsize_image_proto::FullSizeImage *full_size_image,
                AfterHeaderData &after_header_data, ::fullsize_image_proto::PhotoType photo_type,
                ::fullsize_image_proto::ImageFileFormat image_file_format,
                const std::string &image_filepath,
                const std::array<float, 4> &image_alignment_relative_physical_dimension) {
  assert(full_size_image);
  full_size_image->set_imagefileformat(image_file_format);
  full_size_image->set_phototype(photo_type);

  QImage image(image_filepath.c_str());
  full_size_image->set_imageheight(image.height());
  full_size_image->set_imagewidth(image.width());

  assert(image_alignment_relative_physical_dimension.size() == 4);
  for (const auto &matrix_element : image_alignment_relative_physical_dimension) {
    full_size_image->add_imagealignment(matrix_element);
  }
  // TODO: Here we could also set the optional interestingRectangle

  append_file_region(full_size_image->mutable_photofiledata(),
                     readFileIntoCharVector(image_filepath), after_header_data);
}

void buildTileConversion(::tiled_image_proto::TileConversion *tile_conversion,
                         AfterHeaderData &after_header_data,
                         const std::vector<TilesSpecInterface *> &tiles_spec_iface_vec,
                         const std::vector<TilesInterface *> &tiles_iface_vec,
                         common_typedefs::pixel_dimensions_t tile_size,
                         common_typedefs::pixel_dimensions_t overlap) {
  assert(tile_conversion);
  tile_conversion->set_tilesize(tile_size);
  tile_conversion->set_tileoverlap(overlap);
  // TODO: we should take the image file format as input parameter to this
  // function
  tile_conversion->set_imagefileformat(::fullsize_image_proto::ImageFileFormat::JPEG);
  assert(tiles_spec_iface_vec.size() == tiles_iface_vec.size());
  int image_nr = 0;
  common_typedefs::tile_id_t tile_id = 0;
  for (auto &tiles_iface : tiles_iface_vec) {
    common_typedefs::TilesSpec tiles_spec(tiles_spec_iface_vec[image_nr]->tiles_spec());
    const auto num_tiles = numTiles(tiles_spec.whole_image_width(), tiles_spec.whole_image_height(),
                                    tiles_spec.tile_size());
    auto tiledimage = tile_conversion->add_tiledimages();
    assert(tiledimage);
    for (common_typedefs::tile_id_t index = 0; index < num_tiles; ++index) {
      auto tile = tiledimage->add_tiles();
      assert(tile);
      append_file_region(tile, tiles_iface->tile(index), after_header_data);
    }
    ++image_nr;
  }
}

std::vector<SpotHit> convert_to_spot_wise_gene_hits(data_model::spot_id_t num_spots,
                                                    const std::vector<data_model::Gene> &genes) {
  // Initialize the result to have gene hit being zero
  SpotHit::size_type length = genes.size();
  std::vector<SpotHit> result(num_spots, SpotHit(length, 0));
  data_model::gene_id_t gene_index = 0;
  for (const auto &gene : genes) {
    for (const auto &non_zero_hit : gene.non_zero_hits) {
      auto &spot_hit = result[non_zero_hit.pos_index];
      spot_hit[gene_index] = non_zero_hit.hits;
    }
    ++gene_index;
  }
  return result;
}

std::vector<char> create_serialized_totalreads(std::vector<SpotHit> &spot_hits) {
  ::st_exp_proto::CountPerSpot count_per_spot_pb;
  for (SpotHit &spot_hit : spot_hits) {
    data_model::hits_t sum = 0;
    for (auto &hit : spot_hit) {
      sum += hit;
    }
    count_per_spot_pb.add_count(sum);
  }
  return messageToCharVector(count_per_spot_pb);
}

std::vector<char> create_serialized_uniquegenes(std::vector<SpotHit> &spot_hits,
                                                int unique_gene_definition_cutoff) {
  ::st_exp_proto::CountPerSpot count_per_spot_pb;
  for (SpotHit &spot_hit : spot_hits) {
    // unique_genes does not in the strict sense have the unit of "hits"
    // but we use it as it should match the data type.
    data_model::hits_t unique_genes = 0;
    for (auto &hit : spot_hit) {
      if (hit >= unique_gene_definition_cutoff) {
        ++unique_genes;
      }
    }
    count_per_spot_pb.add_count(unique_genes);
  }
  return messageToCharVector(count_per_spot_pb);
}

void build_readcounts(data_model::spot_id_t num_spots, const std::vector<data_model::Gene> &genes,
                      ::st_exp_proto::ReadCounts *readcounts_pb,
                      AfterHeaderData &after_header_data) {
  assert(readcounts_pb);
  std::vector<SpotHit> spot_hits = convert_to_spot_wise_gene_hits(num_spots, genes);
  append_file_region(readcounts_pb->mutable_totalreads(), create_serialized_totalreads(spot_hits),
                     after_header_data);
  for (int i = 1; i <= 100; ++i) {
    auto uniquegenes_pb = readcounts_pb->add_uniquegenes();
    append_file_region(uniquegenes_pb, create_serialized_uniquegenes(spot_hits, i),
                       after_header_data);
  }
}
}

void serialize_to_st_exp_protobuf(const CrickReaderInterface *crick_reader_interface,
                                  const std::vector<TilesSpecInterface *> &tiles_spec_iface_vec,
                                  const std::vector<TilesInterface *> &tiles_iface_vec,
                                  const std::string &filepath,
                                  common_typedefs::pixel_dimensions_t tile_size,
                                  common_typedefs::pixel_dimensions_t overlap) {
  assert(crick_reader_interface);
  assert(tiles_spec_iface_vec.size() == tiles_iface_vec.size());

  data_model::Everything data_model_everything_input(crick_reader_interface->parsedFeatures());
  std::vector<size_t> ordered_spot_ids(spots_ordered(data_model_everything_input.spots));
  data_model::Everything data_model_everything =
      reorder_spots(ordered_spot_ids, data_model_everything_input);
  const auto image_alignment_matrix(
      crick_reader_interface->parsedImageAlignment().alignment_matrix);

  AfterHeaderData after_header_data(magic_fileformat_identifier_st_exp_protobuf);

  st_exp_proto::Header header_pb;
  auto common_data(header_pb.mutable_commondata());

  // physical_dimensions is not used right now (it's empty), but none the less
  // it needs to be initialized.
  auto physical_dimensions = common_data->mutable_physicaldimensions();

  append_file_region(common_data->mutable_spots(),
                     create_serialized_spots(data_model_everything.spots, image_alignment_matrix),
                     after_header_data);
  append_file_region(common_data->mutable_genenames(),
                     create_gene_names(data_model_everything.gene_names), after_header_data);
  build_readcounts(data_model_everything.spots.size(), data_model_everything.genes,
                   common_data->mutable_readcounts(), after_header_data);

  for (const auto &gene : data_model_everything.genes) {
    auto gene_hit = common_data->add_genehits();
    append_file_region(gene_hit, create_genehit(gene), after_header_data);
  }
  const std::array<float, 4> identity_matrix{1.0, 0.0, 0.0, 1.0};
  ParsedImageAlignment imagA = crick_reader_interface->parsedImageAlignment();
  buildPhoto(header_pb.add_images(), after_header_data, ::fullsize_image_proto::PhotoType::HE,
             ::fullsize_image_proto::ImageFileFormat::JPEG, imagA.blue_image, identity_matrix);
  buildPhoto(header_pb.add_images(), after_header_data, ::fullsize_image_proto::PhotoType::SPOTS,
             ::fullsize_image_proto::ImageFileFormat::JPEG, imagA.red_image, identity_matrix);

  buildTileConversion(header_pb.add_tileconversions(), after_header_data, tiles_spec_iface_vec,
                      tiles_iface_vec, tile_size, overlap);

  std::vector<char> header_as_vector_char = messageToCharVector(header_pb);
  assert(header_as_vector_char.size() > 0);
  after_header_data.setHeader(&header_as_vector_char[0], header_as_vector_char.size());

  after_header_data.write_result_file(filepath);
}
