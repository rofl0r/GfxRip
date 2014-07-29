#! /bin/sh
#
#  Sets up the BGui 2.0 package for building under Unix, converting text
#  files from CR/LF to LF format.


echo "Converting BGui 2.0 files to Unix format..."

find . -type f "(" \
   -name "*.c" -o -name "*.cfg" -o -name "*.cpp" -o -name "*.dep" -o \
   -name "*.h" -o -name "*.hin" -o -name "*.in" -o -name "*.inc" -o \
   -name "*.m4" -o -name "*.mft" -o -name "*.s" -o -name "*.sh" -o \
   -name "*.spec" -o -name "*.pl" -o -name "*.txt" -o -name "*._tx" -o \
   -name "makefile.*" -o -name "readme.*" \
   ")" \
   -exec sh helper.sh {} \;

cp src/Makefile.unx src/Makefile

echo "Done!"
echo
echo "Now run make while in the src directory"
