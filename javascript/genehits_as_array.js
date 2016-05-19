// The message GeneHit in the file
//  ../protobuf_schema/st_exp.proto
// stores the gene hit data in a compact sparse format
// because a lot of the gen hit counts are zero.
//
// This function converts the compact sparse data to
// to an array where each element stores the gene hit count
// of a spot. The element indice is the same as the spot_id.

function genehits_as_array(gene_hit_message, number_of_spots) {
    // TODO: Investigate this. Not so important but,
    // maybe it is faster to first create an array of only zeroes and then overwrite the values
    // not equal to zero..
    var gene_hit_array = [];
    for (var i = 0; i < gene_hit_message.hits.length; i++) {
        var zeroes_skipped = gene_hit_message.zeroHitsSkipped[i];
        for (var j = 0; j < zeroes_skipped; j++) {
            gene_hit_array.push(0);
        }
        gene_hit_array.push(gene_hit_message.hits[i]);
    }
    var number_of_zeroes_in_the_end = number_of_spots - gene_hit_array.length;
    for (var i = 0; i < number_of_zeroes_in_the_end; i++) {
        gene_hit_array.push(0);
    }
    return gene_hit_array;
}

