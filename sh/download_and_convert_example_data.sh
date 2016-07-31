#!/bin/sh

# Run this script from the c++ build directory of st_exp_protobuf.
# It will download about 200 Mb of data into the current working directory
# and then create st_exp_protobuf files (of total size 334 Mb)


download() {
    return
    url=http://www.spatialtranscriptomicsresearch.org/wp-content/uploads/2016/07/$1
    if ! wget -q $url; then
      echo Error: could not download $url
    else
      echo downloaded $url
    fi
}

convert_to_binary() {
  countmatrix=$1
  photo=$2
  transformation=$3
  for i in 2 9 12; do
      if [ $transformation = "Rep${i}_MOB_transformation.txt" ]; then
          transformation=Rep${i}_MOB_transformaton.txt
      fi
  done  
  download $countmatrix
  download $photo  
  download $transformation
  c++/create_st_exp_protobuf/create_st_exp_protobuf $countmatrix $photo $transformation ${countmatrix}_with_photo.st_exp_protobuf -c 95
}


for i in `seq 1 4`; do
  convert_to_binary Rep${i}_MOB_count_matrix-1.tsv HE_Rep${i}.jpg Rep${i}_MOB_transformation.txt
done

for i in `seq 5 12`; do
  convert_to_binary Rep${i}_MOB_count_matrix-1.tsv HE_Rep${i}_MOB.jpg Rep${i}_MOB_transformation.txt
done

for i in `seq 1 4` ; do
  convert_to_binary Layer${i}_BC_count_matrix-1.tsv HE_layer${i}_BC.jpg Layer${i}_BC_transformation.txt
done
