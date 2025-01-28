
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

ctest -C Debug

valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --undef-value-errors=no --log-file=log ./your_program


getenv内部的所有参数都需要更改