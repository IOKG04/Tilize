# Tilize

> Do you think your images have too many distinct pixel groups? This might help :3

## Function

Once this is done, which it isn't as of writing this, this tool should be able to take an image,
split it into n by m rectangles, then choose which of a given number of patterns and color combinations
preserves that rectangle best, and then replace it with that, resulting in an image made entirely of tiles.

For now though, that isn't the case yet. I need to build the software first.

## Usage

The following is a goal, as of now, barely anything of this exists
```
Usage:
 Tilize [[options]] [file]              | Tilizes [file] with [options]
 Tilize help                            | Show this message

Options:
 -o [file]                              | Save output to [file]
 -c [file]                              | Use [file] as configuration
 -j[=number]                            | Use multiple threads ([number] if provided, otherwise maximum amount available)
 -s                                     | Don't show GUI (---)
 -v                                     | Print extra info (---)

If the same option is provided multiple times, the last one is used.
```
If something is followed by `(---)`, it is not implemented yet

To make configurations, a script will be provided in the "near" future.

## Building

To build this project, it is required that you have [premake5](https://premake.github.io/) installed.

If you have it, just do what you would do with any project that uses premake. 

The different configurations are
- `debug_nosdl`
- `debug_sdl`
- `release_nosdl`
- `release_sdl`
the ones ending in `nosdl` should work on any system supporting the c11 standard,
the ones ending in `sdl` will require `SDL2` to be installed in order to show a GUI.

## Credit

This project uses [cJSON](https://github.com/DaveGamble/cJSON) by [DaveGamble](https://github.com/DaveGamble) and others, distributed under the [MIT License](https://github.com/DaveGamble/cJSON/blob/master/LICENSE),  
[tinycthread](https://github.com/tinycthread/tinycthread), distributed under a license which's name i cant find, it's [here](https://github.com/tinycthread/tinycthread/blob/master/README.txt) though,  
and parts of [stb](https://github.com/nothings/stb), distributed under the [MIT License and the Unlicense](https://github.com/nothings/stb/blob/master/LICENSE) (where you just choose which one you prefer, and cause I don't have opinions on that I'll just show both :3).

The [rounded_6x6](resources/round_6x6.png) and [rounded_24x24](resources/round_24x24.png) fonts are from [here](https://frostyfreeze.itch.io/pixel-bitmap-fonts-png-xml) and released under CC0.
