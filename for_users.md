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
a path to the image file where all the different *tiles*, or rectangles in the explanation from earlier, are shown,
the dimensions of those tiles, and the colors used.

I am honestly too lazy to write about this right now, so uhh,
use the `make-config.lua` script if you want to make your own configuration files without typing json by hand,
and for anything else just ask me, I'll add it here eventually.

## QNA

If you have any questions, ask them in an issue and I will try to add the answer here.
