#! /bin/sh

if [ "$1" == "all" ]; then
    echo "Clearing dependencies"
    rm -f src/cJSON.*
    rm -f src/tinycthread.*
    rm -f src/stb*.h

    echo "Clearing premake5 files"
    rm -f Makefile Tilize.make
fi

echo "Clearing bin/"
rm -rf bin/
echo "Clearing obj/"
rm -rf obj/
