::Example generates simple document with code snipets and some plots.

::How to build:
::1. go to build directory
::2. use your c copiler to build example.c from there
::3. call builded executable to generate pdf and other files

mkdir build
cd build
gcc ../src/example.c -o ./example-build
./example-build.exe
