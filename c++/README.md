
The .clang-format file was generated with the command:

clang-format-3.6 -style=LLVM -dump-config  > .clang-format

and after that modified to have ColumnLimit be 100 instead of 80.

To format the source code files you could run

    cd /path/to/src/st_exp_protobuf/c++ && find . -type f -name '*.h' -o -name '*.cc' -exec clang-format-3.6 -i {} \;

