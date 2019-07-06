# Overview

This section describes how to prepare a build environment for compiling this project. Some actions must be run as *root* or using `sudo`. I expect that you know what you are doing. And remember – one needs to be a hero to compile this stuff. I will use Debian GNU/Linux as a base OS if you don't mind.

The compilation is done in two steps:

   1) C and assembler files compilation in a GNU/Linux environment using *Open Watcom tools*;

   2) Pascal files compilation in a DOS environment using *DOSEMU* and *DJGPP development kit*.

This split was made because we're going to drop old Pascal compiler and DOS environment and use a clean cross-compiling toolchain in the future. For now it is possible to run the first step in a DOS environment too.

To test functionality of the player we'll use *DOSBox* because of DOSEMU incompatibility issues.

# Setup build environment for GNU/Linux

## 1. Install DOS environment

```sh
sudo apt-get install dosemu dosbox
```

Run DOSEMU and then exit typing `exitemu`. This will create a default DOSEMU configuration under `~/.dosemu` directory. The drive C: is in `~/.dosemu/drive_c` directory. Now when we know that, we can use DOS paths here.

## 2. Install DJGPP tools

Install minimal DJGPP development kit into `c:\lang\djgpp` directory and create `c:\djgpp.bat` DOS shell configuration script following instructions in `c:\lang\djgpp\readme.1st` file.

Use script [scripts/dosemu/djgpp](../scripts/dosemu/djgpp) for that.

## 3. Install Open Watcom tools

Download binary archive with a latest build of Open Watcom tools from GitHub and properly place it's contents into `/opt/watcom`.

If you want to build using DOS environment then create `c:\watcom.bat` DOS shell configuration script following instructions in `c:\lang\watcom\readme.1st` file.

If you want to build using GNU/Linux environment then set needed environment variables in `~/.bashrc` (you must re-login to use them).

Use script [scripts/dosemu/watcom](../scripts/dosemu/watcom) for that.

## 4. Install abandoned Pascal compiler for DOS

   1) Find binary files of it and install them manually (preferrably in `c:\lang\tp` or similar);

   2) Create `c:\tp.bat` DOS shell script to update DOS environment variables (usually PATH) to use it.

This step will be removed in the future.

Use script [scripts/dosemu/tp](../scripts/dosemu/tp) for that.

# Setup project files

## 1. Install Git

```sh
sudo apt-get install git
```

## 2. Download project

Download or clone project to `~/Projects/retroplayer` (or any other at your option) and make a symbolic link `c:\projects\rp` to it.

Use script [scripts/dosemu/project](../scripts/dosemu/project) for that.

# Prepare to build and test

## Configure DOSEMU

Create `c:\init.bat` DOS shell script to set all needed DOS environment variables and to switch to `c:\projects\rp` directory.

Use script [scripts/dosemu/dosemu](../scripts/dosemu/dosemu) for that.

# Build project

## Debug build

```sh
cd ~/Projects/retroplayer
make all DEBUG=1
```

Start DOSEMU. When you are in DOS project's directory type:

```sh
make all "DEBUG=1"
```

## Release build

```sh
cd ~/Projects/retroplayer
make all
```

Start DOSEMU. When you are in DOS project's directory type:

```sh
make all
```

**Note**: You must clean project's folder each time you want to build other type of target (when switching between *release* and *debug* versions).

# Test project

```sh
cd ~/Projects/retroplayer
./test
```

# Clean project

```sh
cd ~/Projects/retroplayer
make clean
```
