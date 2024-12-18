#! /bin/sh

# for anyone not using make, this script probably wont do much for you
# it is mainly for my own convenience as i dont like doing things manually

# $0 make  [config]   | Build the project (in one command)
# $0 clean [...]      | Clean specific stuff
# $0 help  [command]  | Show help message

# help command
if [ -z "$1" ] || [ "$1" = "help" ] || [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    if [ -z "$2" ]; then
        # general help message
        echo "$0 make  [config]      | Build Tilize (with [config])"
        echo "$0 clean [group]       | Remove unwanted files (from [group])"
        echo "$0 help  [command]     | Show help message (for [command])"
    elif [ "$2" = "make" ]; then
        # make help message
        echo "$0 make debug   nosdl  | Builds configuration Debug_NoSDL"
        echo "$0 make release nosdl  | Builds configuration Release_NoSDL"
        echo "$0 make debug   sdl    | Builds configuration Debug_SDL"
        echo "$0 make release sdl    | Builds configuration Release_SDL"
        echo "$0 make all            | Builds all configurations"
    elif [ "$2" = "clean" ]; then
        # clean help message
        echo "$0 clean binaries      | Remove binaries and object files"
        echo "$0 clean premake       | Remove files generated by premake"
        echo "$0 clean dependencies  | Remove downloaded dependencies"
        echo "$0 clean build         | Remove binaries, objects and premake files"
        echo "$0 clean all           | Remove all the above files"
    elif [ "$2" = "help" ]; then
        # help help message
        echo "$0 help make           | Show help message for \"make\" command"
        echo "$0 help clean          | Show help message for \"clean\" command"
        echo "$0 help help           | Show this message"
    fi
    exit 0
fi

# make command
if [ "$1" = "make" ] || [ "$1" = "build" ]; then
    # builds specified configurations
    function run_premake_and_build () {
        echo "Running premake"
        premake5 gmake2
        for conf in $@
        do
            echo "Building $conf"
            make "config=$conf"
        done
        return 0
    }

    if [ -z "$2" ] || ( [ "$2" = "debug" ] && ( [ -z "$3" ] || [ "$3" = "nosdl" ] ) ); then
        run_premake_and_build debug_nosdl
    elif [ "$2" = "debug" ] && [ "$3" = "sdl" ]; then
        run_premake_and_build debug_sdl
    elif [ "$2" = "release" ] && ( [ -z "$3" ] || [ "$3" = "nosdl" ] ); then
        run_premake_and_build release_nosdl
    elif [ "$2" = "release" ] && [ "$3" = "sdl" ]; then
        run_premake_and_build release_sdl
    elif [ "$2" = "nosdl" ]; then
        run_premake_and_build debug_nosdl
    elif [ "$2" = "sdl" ]; then
        run_premake_and_build debug_sdl
    elif [ "$2" = "all" ]; then
        run_premake_and_build debug_nosdl release_nosdl debug_sdl release_sdl
    fi
    exit 0
fi

# clean command
if [ "$1" = "clean" ]; then
    # deletes all specified files
    function delete_files () {
        for f in $@
        do
            echo "Deleting $f"
            rm -rf "$f"
        done
        return 0
    }

    if [ -z "$2" ] || [ "$2" = "binaries" ] || [ "$2" = "bin" ]; then
        delete_files bin obj
    elif [ "$2" = "premake" ]; then
        delete_files Makefile Tilize.make
    elif [ "$2" = "dependencies" ] || [ "$2" = "dep" ]; then
        delete_files src/cJSON.* src/tinycthread.* src/stb*.h
    elif [ "$2" = "build" ]; then
        delete_files bin obj Makefile Tilize.make
    elif [ "$2" = "all" ]; then
        delete_files bin obj Makefile Tilize.make src/cJSON.* src/tinycthread.* src/stb*.h
    fi
    exit 0
fi
