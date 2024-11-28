# For Users

If you want to use Tilize, here is everything you need to know:

## Downloading

First off, you have to download Tilize of course.

### Windows

If you are using an x86_64 Windows computer (which you probably are if you don't know what that means),
you can go to the [Releases](https://github.com/IOKG04/Tilize/releases) page, select the top entry, the newest version,
scroll down a bit and download `Tilize_win.zip`.
Once you unzip it, all you need to do is open it in the command prompt, however you do that nowadays,
and then run `.\Tilize help` to confirm your installation is good.

### Not Windows

Because I currently don't have binaries for non Windows users, you have two options,
either you install the Windows version and run it through a compatibility layer, such as [Wine](https://www.winehq.org/),
or you can build the project from source, instructions for which you can find [here](for_devs.md#building).

## Using

As mentioned already in the Downloading section for Windows, Tilize is a command line utility.
You provide different options to it, controlling how it does it's job, and it does that job.

Here, that job is *Tilizing* an image, or in other words, splitting it up into a bunch of rectangles,
comparing each of those to a list of other, predefined rectangles,
and replacing the initial rectangle with whatever rectangle fits best.

### Options

Here is a select list of options you will most likely want to use, and how/why to use them:

First off we have the `-o` option.
This one tells `Tilize` where to put the image it creates, which could come in handy if you want to get something out of running the program.
Assuming you haven't built the program from source, with SDL support, you should always provide this option,
as otherwise all calculations will be thrown away the second they're done.  
If you want to output to a file called `myfile.png`, you would have to add `-o myfile.png` after the `Tilize` when running the command.

Secondly, the `-c` option allows you to specify the configuration file used, or with the explanation above, it specifies which other rectangles are compared against.
I'll go into configuration files a bit more [later](#configurations), but in the meantime feel free to try out the examples provided in the `resources/` directory.
Configuration files are those ending in `.json` or `.cfg`.  
If you wish to change the configuration file to `myconfig.json`, you need to add `-c myconfig.json` to your invocation.

Lastly, the `-j` option. This one lets you use more threads, more parts of your computer, thereby making the program run faster.  
I would advise always having this enabled, by adding `-j` to the `Tilize` command.

There are more options of course, you can see them by executing `Tilize help`.

## Configurations

Configurations contain mainly three things:
a path to the image file (relative to the configuration file) where all the different *tiles*, or rectangles in the explanation from earlier, are shown,
the dimensions of those tiles,
and the colors used for them.

### Pattern images

Pattern images are the images containing info about the tiles, namely which ones exist.
A pattern image is usually a black and white image of some amount of smaller images,
the rectangles from the explanation earlier which I'll stop refering to now, each with the same dimensions.  
These dimensions are the width and height of a tile, which you will need for later.

While usually just black and white, where black represents the "background" and white the "foreground",
you can use any image as a pattern image. Under the hood, it just compares the red channel to `128`.
If it's less than that, the pixel is part of the background. If not, foreground.

### `config-maker.lua`

If you want to make your own configuration, I would recommend running [config-maker.lua](resources/config-maker.lua), as it will handle all the json stuff.

To use it, run `lua config-maker.lua` or `./config-maker.lua` if you are on a unix system.
As the first argument, tell it how you want to call your new configuration,
for example, to get a configuration called `myconfig.json`, the command would be `lua config-maker.lua myconfig.json`.

After that, you will be asked for a couple things, here is what to enter:  
- `Location of pattern?`: Here you will want to type in the path to the image file containing the different patterns.
  For whatever reason relative paths or ones containing `~` dont work, so you will have to use an absolute path.
- `Tile dimensions?`: Here you enter how big your tiles are. This depends on the pattern image you created.
- `Color [number]?`: Here you are asked to input all the colors you want in your configuration.
  White is `FFFFFF`, black is `000000`. If you don't know the hex codes of your favorite colors,
  I recommend [googling "color picker"](https://www.google.com/search?q=color+picker) and then copying the `HEX` value.
  Be careful to not copy the `#` though, as `config-maker.lua` does not support it as of writing.  
  Once you've put in all the colors you want, press `Enter`/`Return` without any input.
- `Background` and `Foreground color?`: Here you can tell the program that the background (or foreground) should be a single color
  and that it shouldn't use whatever is technically best.
  This can be useful if you want to create ASCII like art or anything else where it's good to have one dominant color.  
  If you want all colors to be picked from, leave the lines empty.

After that, you should be done, the program takes care of the rest.

If you intend on sharing the configuration, copy the pattern image it uses too.
It will automatically be copied to the same directory as the configuration.

## QNA

If you have any questions, ask them in an issue and I will try to add the answer here.
