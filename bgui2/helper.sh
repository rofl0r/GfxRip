#! /bin/sh
#
#  Sets up the BGui 2.0 package for building under Unix, converting text
#  files from CR/LF to LF format.

echo $1
mv $1 _tmpfile
tr -d \\\r < _tmpfile > $1
rm _tmpfile