## Formatting style guide 

The .clang-format file was generated with the command:

clang-format-3.6 -style=LLVM -dump-config  > .clang-format

and after that modified to have ColumnLimit be 100 instead of 80.

To format the source code files you could run

    cd /path/to/src/st_exp_protobuf/c++ && find . -type f -name '*.h' -o -name '*.cc' -exec clang-format-3.6 -i {} \;

## The crickconvert command

The command crickconvert can be use to convert our internal Crick file format to the st_exp_protobuf file format.

Choosing a tile size of 510 and an overlap of 1 seems to give a nice user experience in OpenSeadragon

    ./c++/crickconvert/crickconvert  --tile-size 510 --overlap 1 file.crick file.st_exp_protobuf
