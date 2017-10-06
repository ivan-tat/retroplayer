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

## 2. Install minimal DJGPP development kit

### Download archives

Download the following files from [DJGPP project's page](http://www.delorie.com/pub/djgpp/):

| File                 | Description                       |
| :------------------- | :-------------------------------- |
| v2/djdev205.zip      | DJGPP development kit 2.05        |
| v2gnu/bsh205bbr4.zip | GNU Bash 2.05b for DJGPP V2       |
| v2gnu/fil41br2.zip   | GNU fileutils 4.1 for DJGPP V2    |
| v2gnu/fnd4233br5.zip | GNU findutils 4.2.33 for DJGPP V2 |
| v2gnu/grep228b.zip   | GNU Grep 2.28 for DJGPP V2        |
| v2gnu/sed44b.zip     | GNU sed 4.4 for DJGPP V2          |
| v2gnu/txt20br2.zip   | GNU Textutils 2.0 for DJGPP V2    |

```sh
for f in \
v2/djdev205.zip \
v2gnu/bsh205bbr4.zip \
v2gnu/fil41br2.zip \
v2gnu/fnd4233br5.zip \
v2gnu/grep228b.zip \
v2gnu/sed44b.zip \
v2gnu/txt20br2.zip; do \
   wget -c -U "" http://www.delorie.com/pub/djgpp/current/$f
done
```

### Extract archives

Install downloaded minimal DJGPP development kit to `c:\lang\djgpp`.

```sh
ENV_DRIVE_C_DIR=~/.dosemu/drive_c
ENV_DJGPP_DIR=$ENV_DRIVE_C_DIR/lang/djgpp
mkdir -p $ENV_DJGPP_DIR
for f in \
djdev205.zip \
bsh205bbr4.zip \
fil41br2.zip \
fnd4233br5.zip \
grep228b.zip \
sed44b.zip \
txt20br2.zip; do \
   unzip -u -o $f -d $ENV_DJGPP_DIR
done
```

### Set DOS environment variables

Create `c:\djgpp.bat` DOS shell script following instructions in `djgpp/readme.1st` file.

```sh
cat > $ENV_DRIVE_C_DIR/djgpp.bat <<EOF
@echo off
set DJGPP=c:\lang\djgpp\djgpp.env
set PATH=c:\lang\djgpp\bin;%PATH%
EOF
```

## 3. Install Open Watcom tools

### Download archive

Download [binary archive with a latest build of Open Watcom tools](https://github.com/open-watcom/travis-ci-ow-builds/archive/master.zip) from GitHub.

```sh
wget -c https://github.com/open-watcom/travis-ci-ow-builds/archive/master.zip
```

This script downloads `master.zip` containing `travis-ci-ow-builds-master` directory.

### Extract archive

Install Open Watcom tools in `/opt/watcom`.

```sh
ENV_WATCOM_DIR=$ENV_DRIVE_C_DIR/lang/watcom
TMPDIR=`mktemp -d`
unzip master.zip -d $TMPDIR
sudo mv $TMPDIR/travis-ci-ow-builds-master /opt/watcom-2.0b
rmdir $TMPDIR
sudo ln -s -r /opt/watcom-2.0b /opt/watcom
ln -s -r /opt/watcom $ENV_WATCOM_DIR
```

### Set environment variables

Follow instruction in `readme.txt` and create a script for convenance.

#### ...for DOS (if you want to build using DOS environment)

Create `c:\watcom.bat` DOS shell script to run Open Watcom tools from a DOS shell.

```sh
cat > $ENV_DRIVE_C_DIR/watcom.bat <<EOF
@echo off
set WATCOM=c:\lang\watcom
set PATH=%WATCOM%\binw;%PATH%
set EDPATH=%WATCOM%\eddat
set INCLUDE=%WATCOM%\h
EOF
```

#### ...for GNU/Linux (if you want to build using GNU/Linux environment)

Append to `~/.bashrc` or other shell script at your option the initialization of some environment variables.

```sh
cat >> ~/.bashrc <<EOF
export WATCOM=/opt/watcom
case \`uname -m\` in
x86_64)
    export PATH=\$WATCOM/binl64:\$WATCOM/binl:\$PATH
    ;;
*)
    export PATH=\$WATCOM/binl:\$PATH
    ;;
esac
export EDPATH=\$WATCOM/eddat
export INCLUDE=\$WATCOM/h
EOF
```

## 4. Install abandoned Pascal compiler for DOS

   1) Find binary files of it and install them manually (preferrably in `c:\lang\tp` or similar);

   2) Create `c:\tp.bat` DOS shell script to update DOS environment variables (usually PATH) to use it.

This step will be removed in the future.

# Setup project files

## 1. Install Git

```sh
sudo apt-get install git
```

## 2. Download project

Download or clone project to `~/Projects/retroplayer` (or any other at your option) and make a symbolic link `c:\projects\rp` to it.

```sh
PROJ_DIR=~/Projects/retroplayer
ENV_PROJ_DIR=$ENV_DRIVE_C_DIR/projects/rp
mkdir -p $PROJ_DIR
git clone https://github.com/ivan-tat/retroplayer.git $PROJ_DIR
mkdir -p `dirname $ENV_PROJ_DIR`
ln -s $PROJ_DIR $ENV_PROJ_DIR
```

# Prepare to build and test

## Configure DOSEMU

Create `c:\init.bat` DOS shell script to set DOS environment variables.

```sh
cat > $ENV_DRIVE_C_DIR/init.bat <<EOF
@echo off
call c:\djgpp.bat
call c:\watcom.bat
call c:\tp.bat
cd c:\projects\rp
c:
EOF
```

Create `c:\make.bat` DOS shell script for quick compiling.

```sh
cat > $ENV_DRIVE_C_DIR/make.bat <<EOF
@echo off
call init.bat
call make.bat
EOF
```

# Build project

```sh
cd $PROJ_DIR
./make
```

Start DOSEMU and type `make`.

# Test project

```sh
cd $PROJ_DIR
./test
```

# Clean project

```sh
cd $PROJ_DIR
./clean
```
