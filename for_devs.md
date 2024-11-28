# For Developers

If you want to help with developing Tilize, here is everything you need to know:

First off, read [this](for_users.md) first, it might answer some questions before you start to have them.

## Building

Tilize uses [premake](https://premake.github.io/), meaning *once you have installed it*, you can create build files for whatever program you prefer,
be that a Makefile like me, a solution for Visual Studio, or something else (provided premake supports it).


If you are on linux specifically, you can also use the `util.sh` script for building and cleaning the project.
Simply run `./util.sh help` to figure out what you can do.

### Dependencies

When you first run `premake`, the command will also install some files to your disk.
These are required to build the project, so don't let that confuse you.

In case you wonder why I'm downloading the dependencies to the project directory and don't require you to have them installed locally,
the answer is simplicity, I want both the experience of using the software and developing it to be as simple as possible,
and any extra external dependency would make compiling the project harder.

### IMPORTANT

If you build one of the SDL versions, you will not be able to close the program with `ctrl+c` or anything else short of htop or whatever you use to send `SIGKILL`.
I will come around to fixing this eventually, but that is the reason why the default version people can download is the NoSDL version.

## Languages used

### Tilize

The code for the main application is written in standard complient c11 with minimal external dependencies.

The only external dependency Tilize uses is SDL2, which can be disabled at build time in case you don't have it installed.
Just use one of the `NoSDL` configurations, and you won't have to worry.

If you are using `gcc` or `clang`, premake already takes care of ensuring that the `-Wpedantic` flag is set, so it gives a warning/an error if you code outside the c11 standard.
If you are not using either, figure it out yourself, I'm not omniscient.

### Scripts

For scripts or other code files, it's prefered to use Lua.
The reason is not because of the language itself, but to keep Github from just saying *other* in the *languages* section,
thereby introducing more gray than is already here cause of the c code.

## QNA

If you have any questions, ask them in an issue and I will try to add the answer here.
