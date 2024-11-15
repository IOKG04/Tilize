# `c04_template`

> basically just a `Makefile`

This is a little project template so I can start my projects without having to spend 30 minutes making a `Makefile` each time.

If you want to, you can use it too.

## Project structure

A project made with this template contains a directory with source code, where all files ending in `.c` will be built into one binary,
and a resources directory containing things that could be used at runtime, meaning they are copied to the same place as the binary,
so you can just `zip -r bin.zip bin` and got your whole project ready to give to others.

### Building the project

To build a project made with this template, simply run `make` in the project root.
If that doesn't work, you might have to adapt the `Makefile` cause your PC isn't the same as mine,
sorry for this template being specific.

### `Makefile` configuration

By default the build process will use `gcc` with `-Wall` and `-std=gnu11` and no optimizations or libraries.
This is of course configurable.

You can also change how the output binary is named, or what directories the build process uses, etc.

Once you reach the lines
```
# ###
# CONFIGURATION END
```
you have reached the end of that configurable space.
You're still allowed to edit it of course, but it won't be as easy as just changing names anymore.

## License

This template is licensed under [MIT No Attribution](https://choosealicense.com/licenses/mit-0/), though if you wish to, I won't say no to credit of course.
