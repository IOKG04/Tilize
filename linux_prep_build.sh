#! /bin/sh

echo "Clearing dependencies"
rm -f src/cJSON.*
rm -f src/tinycthread.*
rm -f src/stb*.h
echo

echo "Downloading dependencies"
echo "cJSON"
curl -s -o src/cJSON.c https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.c
curl -s -o src/cJSON.h https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.h
echo "tinycthread"
curl -s -o src/tinycthread.c https://raw.githubusercontent.com/tinycthread/tinycthread/refs/heads/master/source/tinycthread.c
curl -s -o src/tinycthread.h https://raw.githubusercontent.com/tinycthread/tinycthread/refs/heads/master/source/tinycthread.h
echo "stb*"
curl -s -o src/stb_image.h https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h
curl -s -o src/stb_image_write.h https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image_write.h
echo

echo "Invoking premake5"
premake5 gmake2
