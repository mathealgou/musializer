# Musializer

<p align=center>
  <img src="./resources/logo/logo-256.png">
</p>

[![no-build build](https://github.com/mathealgou/musializer/actions/workflows/nob.yaml/badge.svg)](https://github.com/mathealgou/musializer/actions/workflows/nob.yaml)

> [!WARNING]
> This is a fork of [https://github.com/tsoding/musializer](https://github.com/tsoding/musializer)

The project aims to make a tool for creating beautiful music visualizations and rendering high quality videos of them.

*Please, read [CONTRIBUTING.md](CONTRIBUTING.md) before making a PR.*

## Supported Audio Formats

- wav
- ogg
- mp3
- qoa
- xm
- mod
- flac

## Download Binaries

-  [Released Binaries](https://github.com/mathealgou/musializer/releases/latest)

## Build from Source

External Dependencies:
- [ffmpeg](https://ffmpeg.org/) executable available in `PATH` environment variable. It is called as a child process during the rendering of the videos. So if you don't plan to render any videos it's completely **optional**.

We are using Custom Build System written entirely in C called `nob`. [nob.c](./nob.c) is the program that builds Musializer. For more info on this Build System see the [nob.h repo](https://github.com/tsoding/nob.h).

Before using `nob` you need to bootstrap it. Just compile it with the available C compiler. On Linux it's usually `$ cc -o nob nob.c` on Windows with MSVC from within `vcvarsall.bat` it's `$ cl.exe nob.c`. You only need to boostrap it once. After the bootstrap you can just keep running the same executable over and over again. It even tries to rebuild itself if you modify [nob.c](./nob.c) (which may fail sometimes, so in that case be ready to reboostrap it).

I really recommend to read [nob.c](./nob.c) and [nob.h](https://github.com/tsoding/nob.h) to get an idea of how it all actually works. The Build System is a work in progress, so if something breaks be ready to dive into it.

### Linux and OpenBSD

```console
$ cc -o nob nob.c # ONLY ONCE!!!
$ ./nob
$ ./build/musializer
```

If the build fails because of missing header files, you may need to install the X11 dev packages.

On Debian, Ubuntu, etc, do this:

```console
$ sudo apt install libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev
```

On other distro's, use the appropriate package manager.

### Windows MSVC

From within `vcvarsall.bat` do

```console
> cl.exe nob.c # ONLY ONCE!!!
> nob.exe
> build\musializer.exe
```

### Cross Compilation from Linux to Windows using MinGW-w64

Install [MinGW-w64](https://www.mingw-w64.org/) from your distro repository.

Edit `./build/config.h` and set `MUSIALIZER_TARGET_WIN64_MINGW` instead of `MUSIALIZER_TARGET_LINUX`.

```console
$ ./nob
$ wine ./build/musializer.exe
```

