
// An example of how to extract some information out of the data file.
// Only the header, the gene names and the spot information (e.g. coordinates) are extracted.
//


var ProtoBuf = dcodeIO.ProtoBuf;
var builder = ProtoBuf.newBuilder();
ProtoBuf.loadProtoFile("st_exp.proto", builder);

var protobuf_message_parsers = {};
protobuf_message_parsers.header_size_message = builder.build("fileformat_common_proto").HeaderSize;
protobuf_message_parsers.header_message = builder.build("st_exp_proto").Header;
protobuf_message_parsers.gene_hit = builder.build("st_exp_proto").GeneHit;
protobuf_message_parsers.spots = builder.build("st_exp_proto").Spots;
protobuf_message_parsers.gene_names = builder.build("st_exp_proto").GeneNames;

function read_fileregion(callback, file_region, local_st_file) {
    var start_pos = local_st_file.file_regions_start_pos + file_region.regionOffset.low;
    var blob = local_st_file.file.slice(start_pos, start_pos + file_region.regionSize.low);
    var file_reader = new FileReader();
    file_reader.onloadend = function(evt) {
        if (evt.target.readyState == FileReader.DONE) {
            callback(evt.target.result);
        }
    }
    file_reader.readAsArrayBuffer(blob);
}

function parse_file(local_st_file) {
    var file_reader = new FileReader();
    var magic = [ 'S', 'T', '-', 'E', 'X', 'P', '\0', '\0' ];
    file_reader.onloadend = function(local_st_file, evt) {
        if (evt.target.readyState == FileReader.DONE) {
            var len = magic.length;
            for (var i = 0; i < len; i++) {
                if (magic[i] != evt.target.result[i]) {
                    console.log(" magic mismatch " + i );
                    // TODO do proper error handling
                }
            }
            var endslice = evt.target.result.slice(magic.length, magic.length + 5); // 5 is the serialized byte length of the HeaderSize message
            var msg = window.protobuf_message_parsers.header_size_message.decode(endslice);
            var startpos =  magic.length + 5 + 3;  // 3 is just padding
            var header_blob = local_st_file.file.slice(startpos, startpos + msg.headerSize);
            var tmp_pos = startpos + msg.headerSize;
            if (tmp_pos % 8 != 0) {
                tmp_pos = tmp_pos + 8 - (tmp_pos % 8);
            }
            local_st_file.file_regions_start_pos = tmp_pos;
            // TODO. This code could make use of async.js. We would like to have callback being
            // being called when all parts have been parsed.
            //
            // Another thing, the deep nesting could be avoided by assigning
            // by storing the functions in variables.
            file_reader.onloadend = function(local_st_file, evt) {
                if (evt.target.readyState == FileReader.DONE) {
                    local_st_file.parsed_header = window.protobuf_message_parsers.header_message.decode(evt.target.result);
                    read_fileregion(function (local_st_file, buffer) {
                        local_st_file.spots = window.protobuf_message_parsers.spots.decode(buffer);
                    }.bind(null, local_st_file), local_st_file.parsed_header.commonData.spots, local_st_file);
                    read_fileregion(function (local_st_file2, buffer) {
                        local_st_file2.gene_names_array = window.protobuf_message_parsers.gene_names.decode(buffer).geneNames;
                        local_st_file2.gene_names_dictionary = {};
                        for (var i = 0; i < local_st_file2.gene_names_array.length; i++) {
                            local_st_file2.gene_names_dictionary[ local_st_file2.gene_names_array[i] ] = i;
                        }
                    }.bind(null, local_st_file), local_st_file.parsed_header.commonData.geneNames, local_st_file);
                }
            }.bind(null, local_st_file);
            file_reader.readAsArrayBuffer(header_blob);
        }
    }.bind(null, local_st_file);
    var preheader_blob = local_st_file.file.slice(0, magic.length + 8);
    file_reader.readAsArrayBuffer(preheader_blob);
}

