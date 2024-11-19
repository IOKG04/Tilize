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
 TIlize [[options]] make-config         | Starts a dialogue to create a config with [options] (---)
 Tilize help                            | Show this message

Global options:
 -h                                     | Show a help message (---)
 -o [file]                              | Save output to [file] (---)

Tilize options:
 -c [file]                              | Use [file] as configuration
 -j=[number]                            | Use [number] threads (default = 1)
 -s                                     | Don't show GUI (---)
 -v                                     | Print extra info (---)

make-config options:
 -t[=true/false]                        | Enable/Disable true color display (---)

If the same option is provided multiple times, the last one is used.
```
If something is followed by `(---)`, it is not implemented yet

## Credit

This project uses [cJSON](https://github.com/DaveGamble/cJSON) by [DaveGamble](https://github.com/DaveGamble) and others, distributed under the [MIT License](https://github.com/DaveGamble/cJSON/blob/master/LICENSE),  
and [tinycthread](https://github.com/tinycthread/tinycthread), distributed under a license which's name i cant find, it's [here](https://github.com/tinycthread/tinycthread/blob/master/README.txt) though.

The [rounded_6x6](resources/round_6x6.png) and [rounded_24x24](resources/round_24x24.png) fonts are from [here](https://frostyfreeze.itch.io/pixel-bitmap-fonts-png-xml) and released under CC0.
