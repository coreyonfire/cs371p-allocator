#!/bin/bash

files="html/index.html makefile Allocator.h Allocator.log \
TestAllocator.c++ TestAllocator.out"

tempdir=verifytemp

if [ ! -e "Allocator.zip" ]
then
  echo "Error: Couldn't find Allocator.zip"
else
  echo "Found Allocator.zip."
  [ -d $tempdir ] || mkdir $tempdir
  cd $tempdir
  echo "Extracting the archive..."
  unzip -n ../Allocator.zip
  echo "Done."
  for f in $files
  do
    echo -n "Checking for $f... "
    if [ ! -e "$f" ]
    then
      echo "ERROR: file missing from the current folder."
    else
      echo "Present."
    fi
  done
  cd ..
fi
