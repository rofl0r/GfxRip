@echo off
rem
rem  Sets up the BGui 2.0 package for building with djgpp 
rem  and converting text files from LF to CR/LF format.
rem

echo Converting BGui2.0 files to DOS (djgpp/MSVC) format...

copy src\Makefile.dos src\makefile

utod src/*.c
utod src/*.h
utod src/makefile
utod docs/*
utod *.txt

echo
echo Done!  Run make to make the DOS version, or load MSVC
echo with the bgui2.dsw workspace to compile the libs.
