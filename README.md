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

## Tips for building

There are some macros that may be defined to enable or disable specific behaviour at compiletime:
- `GET_THREADS_SUPPORTED`: Whether or not the program can figure out the amount of available threads (used for `-j` option)
- `GUI_SUPPORTED`: Whether or not SDL2 is accessible (to show the gui)

If you wish to specify, you have to give them a value of `0` or non-zero, as the conditional compilation is done using a `#if`, not a `#ifndef` or `#ifdef`.  
A value of `0` indicates something is not supported, a non-zero value indicates it is.

### Makefile configurations

The following configurations are in the `Makefile` for ease of use:
- `all`: Builds with SDL2 support and figures out if `GET_THREADS_SUPPORTED` should be set at compiletime
- `nosdl`: Builds without SDL2 support and figures out if `GET_THREADS_SUPPORTED` should be set at compiletime

## Credit

This project uses [cJSON](https://github.com/DaveGamble/cJSON) by [DaveGamble](https://github.com/DaveGamble) and others, distributed under the [MIT License](https://github.com/DaveGamble/cJSON/blob/master/LICENSE),  
[tinycthread](https://github.com/tinycthread/tinycthread), distributed under a license which's name i cant find, it's [here](https://github.com/tinycthread/tinycthread/blob/master/README.txt) though,  
and parts of [stb](https://github.com/nothings/stb), distributed under the [MIT License and the Unlicense](https://github.com/nothings/stb/blob/master/LICENSE) (where you just choose which one you prefer, and cause I don't have opinions on that I'll just show both :3).

The [rounded_6x6](resources/round_6x6.png) and [rounded_24x24](resources/round_24x24.png) fonts are from [here](https://frostyfreeze.itch.io/pixel-bitmap-fonts-png-xml) and released under CC0.
