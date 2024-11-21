#! /bin/sh

echo "Clearing dependencies"
rm -f src/cJSON.*
rm -f src/tinycthread.*
rm -f src/stb*.h
echo "Done"

echo "Clearing bin/"
rm -rf bin/
echo "Done"

echo "Clearing premake5 files"
rm -f Makefile Tilize.make
echo "Done"
