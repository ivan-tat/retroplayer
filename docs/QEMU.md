# Using QEMU virtual machine to build project

Here we setup QEMU virtual machine with NFS support and emulated DOS environment. We hold project specific files on the host and do compilation in to steps:

1. Compile the first part of project on the host using host's native tools;

2. Compile the remaining part of project shared by NFS in SSH session in QEMU virtual machine using native DOS tools.

**Important**: In recent Debian GNU/Linux (valid for 9/stretch with any kernel from 4.9 and above) the QEMU DPMI functionality is broken because of specific kernel-patches, so we have to use old version (8/jessie) or use a custom build environment using QEMU with appropriate Linux kernel (not even Debian-based). See below for more details.

# 1. Setup build environment for QEMU virtual machine

## 1.1. Download boot image

Here is an example script to download Debian GNU/Linux 8 (jessie) boot image (works fine with *dosemu*):

```sh
IMAGE_URL='https://cdimage.debian.org/cdimage/archive/8.11.0/i386/iso-cd'
IMAGE_FILE='debian-8.11.0-i386-netinst.iso'
wget -c $IMAGE_URL/$IMAGE_FILE
wget -c $IMAGE_URL/SHA512SUMS
sha512sum --ignore-missing -c SHA512SUMS
```

## 1.2. Install QEMU build environment

### 1.2.1. [Client] Install OS in QEMU virtual machine

You can use [scripts/qemu/build-qemu-install.sh](../scripts/qemu/build-qemu-install.sh) script for that. See configuration file [scripts/qemu/build.conf](../scripts/qemu/build.conf). Change it if needed. You may want to change IPv4 address according to your network configuration. Use `ifconfig` to check current network setup.

### 1.2.2. [Client] Install dosemu in QEMU virtual machine

Launch installed OS in QEMU virtual machine, login and type:

```sh
sudo apt install dosemu
```

You can use [scripts/qemu/build-qemu-run.sh](../scripts/qemu/build-qemu-run.sh) script to launch QEMU virtual machine.

## 1.3. Setup NFS server

### 1.3.1. Install NFS server package

```sh
sudo apt install nfs-kernel-server
```

### 1.3.2. Create NFS folder to share project's files

```sh
mkdir -p /srv/nfs/
chown 1000:1000 /srv/nfs
```

### 1.3.3. Add this folder to NFS export list

Edit the file `/etc/exports` - add a line:

```
/srv/nfs 127.0.0.1(rw,sync,no_subtree_check,all_squash,insecure,anonuid=1000,anongid=1000)
```

or if above does not works then try this:

```
/srv/nfs 127.0.0.1(rw,sync,no_subtree_check,no_root_squash,insecure)
```

### 1.3.4. Share all NFS folders

```sh
sudo exportfs -av
```

## 1.4. [Client] Setup NFS client inside QEMU virtual machine

Run QEMU virtual machine. You can use [scripts/qemu/build-qemu-run.sh](../scripts/qemu/build-qemu-run.sh) script for that.

### 1.4.1. [Client] Install NFS client package

```sh
sudo apt install nfs-common
```

### 1.4.2. [Client] Add static NFS mount point

Edit `/etc/fstab` - add a line:

```
192.168.1.130:/srv/nfs /mnt/nfs nfs hard 0 0
```

**Hint**: Change IPv4 address as set in [scripts/qemu/build.conf](../scripts/qemu/build.conf) file.

### 1.4.3. [Client] Mount NFS folder

```sh
sudo mount -a
```

## 1.5. Setup shared NFS folder

### 1.5.1. Move project-specific files into `/srv/nfs`

Move *dosemu* configuration and *retroplayer* folder to the shared NFS folder, make symbolic links back to original folders:

```sh
sudo mv ~/.dosemu /srv/nfs
sudo mv ~/Projects/retroplayer /srv/nfs
ln -s /srv/nfs/.dosemu ~/.dosemu
ln -s /srv/nfs/retroplayer ~/Projects/retroplayer
```

Now everything from `/srv/nfs` host's folder is available in QEMU virtual machine in `/mnt/nfs` folder.

# 2. Using QEMU build environment

## 2.1. Enter QEMU build environment

### 2.1.1. Start QEMU virtual machine

You can use [scripts/qemu/build-qemu-run.sh](../scripts/qemu/build-qemu-run.sh) script for that.

### 2.1.2. Log in QEMU virtual machine via SSH from the host

Wait for virtual machine to boot the installed OS then do login to it via SSH.

You can use [scripts/qemu/build-qemu-ssh.sh](../scripts/qemu/build-qemu-ssh.sh) script for that. Enter QEMU virtual machine user's password to login.

### 2.1.3. Start dosemu

In SSH session start *dosemu* in dumb mode (text console):

```sh
dosemu -t
```

Now we are in a virtual DOS environment. Type `init` to setup current DOS path and all environment variables for current project. This should be done once per *dosemu* console session.

## 2.2. Clean the project directory tree on the host

Type `clean` in the project's folder.

## 2.3. Compile on the host

In the project's folder on the host type `make` to compile what is possible on the host.

## 2.4. [Client] Compile in SSH session

Type `make` to compile the rest of the project.

## 2.5. Test compiled binaries

In the project's folder on the host type `test`. Ths will launch DOS environment with tests.

## 2.6. [Client] Close QEMU build environment

### 2.6.1. [Client] Leave dosemu session

In SSH session type `exitemu` to close opened *dosemu* session.

### 2.6.2. [Client] Shutdown QEMU virtual machine

Type `sudo shutdown -h now` and wait QEMU virtual machine to shutdown. Current SSH session will be closed automatically.
