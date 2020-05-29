# Overview

This section describes how to prepare a build environment to compile this project. Some actions must be run as *root* or using `sudo`. I expect that you know what you are doing. And remember – one needs to be a hero to compile this stuff. I will use Debian GNU/Linux as a base OS if you don't mind.

Initially the compilation process was done in two steps:

   1) C and assembler files compilation into object files in a GNU/Linux environment using native GNU/Linux tools and *Open Watcom tools* for GNU/Linux;

   2) Pascal files compilation and object files linking in a DOS environment using *QEMU*, *DOSEMU*, *DJGPP development kit* and native Pascal linker.

This split initially was made because we were going to drop old Pascal compiler under DOS environment and use a clean cross-compiling GNU/Linux toolchain in the future. It was possible to run the first step in a DOS environment too, but now you should avoid this, because it needs some modifications to scripts, it's weird, slow and tools used in DOS are heavy and hungry for memory (RAM & HDD).

For now it is possible to compile the whole project in a GNU/Linux environment (only a first step above) without the use of native Pascal linker.

To test functionality of the player it is better to use *DOSBox* because of DOSEMU incompatibility issues.

# Setup build environment for GNU/Linux

## 1. Install DOS environment

```sh
sudo apt-get install dosemu dosbox
```

Run DOSEMU and then exit typing `exitemu`. This will create a default DOSEMU configuration under `~/.dosemu` directory. The drive C: is in `~/.dosemu/drive_c` directory. Now when we know that, we can use DOS paths here.

## 2. Install DJGPP tools

Install minimal DJGPP development kit into `c:\lang\djgpp` directory and create `c:\djgpp.bat` DOS shell configuration script following instructions in `c:\lang\djgpp\readme.1st` file.

Use script [scripts/setup/djgpp](../scripts/setup/djgpp) for that.

## 3. Install Open Watcom tools

Download binary archive with a latest build of Open Watcom tools from GitHub and properly place it's contents into `/opt/watcom`.

If you want to build using DOS environment then create `c:\watcom.bat` DOS shell configuration script following instructions in `c:\lang\watcom\readme.1st` file.

If you want to build using GNU/Linux environment then set needed environment variables in `~/.bashrc` (you must re-login to use them).

Use script [scripts/setup/watcom](../scripts/setup/watcom) for that.

## 4. Install abandoned Pascal compiler for DOS (if you plan to use it)

*NOTE*: This step is not neccessary.

   1) Find binary files of it and install them manually (preferrably in `c:\lang\tp` or similar);

   2) Create `c:\tp.bat` DOS shell script to update DOS environment variables (usually PATH) to use it.

Use script [scripts/setup/tp](../scripts/setup/tp) for that.

# Setup project files

## 1. Install Git

```sh
sudo apt-get install git
```

## 2. Download project

Download or clone project to `~/Projects/retroplayer` (or any other at your option) and make a symbolic link `c:\projects\rp` to it.

Use script [scripts/setup/project](../scripts/setup/project) for that.

## Download used tools

Download used tools and place them in the project's tree as described below:

File | Source
--- | ---
`scripts/aspp.sh` | https://gitlab.com/ivan-tat/aspp

You can do this by typing `make download_all` in the `scripts` folder.

# Prepare to build and test

## Configure DOSEMU

Create `c:\init.bat` DOS shell script to set all needed DOS environment variables and to switch to `c:\projects\rp` directory.

Use script [scripts/setup/dosemu](../scripts/setup/dosemu) for that.

# Build project

## Configure

```sh
cd ~/Projects/retroplayer
./configure
```

Select all needed flags and press <Ok>. This will save configuration files to build the project.

**Note**: You must clean project's folder each time you change the build configuration.

## Build

```sh
cd ~/Projects/retroplayer
make all
```

## Build in DOS DJGPP environment

Start DOSEMU. When you are in DOS project's directory type:

```sh
make all
```

This is only useful if you're using DOS native tools to build the project files.

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
