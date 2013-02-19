#!/bin/sh

ZIPDIR=hartikainen_kokkala_str_proc
rm -f $ZIPDIR.zip
rm -rf $ZIPDIR
mkdir $ZIPDIR
mkdir $ZIPDIR/input

cp *.cpp *.hpp README CMakeLists.txt $ZIPDIR
cp input/robinson.txt input/robinson_patterns.txt input/musketeers.txt input/musketeers_patterns.txt $ZIPDIR/input

zip -r $ZIPDIR.zip $ZIPDIR 
