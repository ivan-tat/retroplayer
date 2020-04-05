# Definitions

* VM - a QEMU virtual machine;
* If a shell command is preceeded with `#` - it means that this command must be run under user *root* or under normal user using `sudo` command.
* If a shell command is preceeded with `$` - it means that this command may be run under normal user (not *root*).

# Using VM to build the project

Here we setup a VM with NFS support and emulated DOS environment. We hold project specific files on the host and compile in to steps:

1. Compile the first part of the project on the host using host's native tools;
2. Compile the remaining part of the project shared by NFS in SSH console in a VM using native DOS tools.

**Important**: In recent Debian GNU/Linux (valid for 9/stretch with any kernel from 4.9 and above) the QEMU DPMI functionality is broken because of specific kernel-patches, so we have to use old version (8/jessie) or use a custom build environment using QEMU with appropriate Linux kernel (not even Debian-based). See below for more details.

# Prepare the VM build environment

Download boot image. Here is an example script to download Debian GNU/Linux 8 (jessie) boot image (works fine with *dosemu*):

```sh
_URL='https://cdimage.debian.org/cdimage/archive/8.11.0/i386/iso-cd'
wget -c $_URL/debian-8.11.0-i386-netinst.iso $_URL/SHA512SUMS
sha512sum --ignore-missing -c SHA512SUMS
```

## Install OS in VM

You can use [scripts/qemu/build-qemu-install.sh](../scripts/qemu/build-qemu-install.sh) script for that. See configuration file [scripts/qemu/build.conf](../scripts/qemu/build.conf). Change it if needed. You may want to change IPv4 address according to your network configuration. Use `ifconfig` to check current network setup.

You may install `sudo` package if needed.

### How-to: Start VM

You can use [scripts/qemu/build-qemu-run.sh](../scripts/qemu/build-qemu-run.sh) script for that.

You will be prompted to log in. When the VM is active it must be switched off correctly or you may damage your VM.

### How-to: Stop VM

When you logged in as a user in VM or via SSH console to VM, type:

```sh
# shutdown -h now
```

and wait VM to shutdown. All active SSH sessions (if any) to the VM will be closed automatically.

## Install *dosemu* in VM

Start VM, log in as *root* and type:

```sh
# apt install dosemu
```

## Setup NFS server on the host

Install NFS server package on the host:

```sh
# apt install nfs-kernel-server
```

Create NFS folder on the host to share project's files with VM:

```sh
# mkdir -p /srv/nfs
# chown 1000:1000 /srv/nfs
```

To add this folder to NFS export list open the file `/etc/exports` in editor:

```sh
# nano /etc/exports
```

and add a line:

```
/srv/nfs 127.0.0.1(rw,sync,no_subtree_check,all_squash,insecure,anonuid=1000,anongid=1000)
```

or, if above does not work for some reason, try this line:

```
/srv/nfs 127.0.0.1(rw,sync,no_subtree_check,no_root_squash,insecure)
```

Save it and close. To share all NFS folders type:

```sh
# exportfs -av
```

## Setup NFS client inside VM

Start VM, log in as *root* and install NFS client package inside it:

```sh
# apt install nfs-common
```

Open file `/etc/fstab` in editor:

```sh
# nano /etc/fstab
```

and add a line to add static NFS mount point:

```
192.168.1.130:/srv/nfs /mnt/nfs nfs hard 0 0
```

**Hint**: Change IPv4 address as set in [scripts/qemu/build.conf](../scripts/qemu/build.conf) file.

Now mount NFS folder in VM:

```sh
# mount -a
```

## Setup shared NFS folder: move project-specific files into `/srv/nfs`

On the host move *dosemu* configuration and *retroplayer* folder to the shared NFS folder (replace `<user>` with normal user's name where *dosemu* configuration is stored):

```sh
# mv /home/<user>/.dosemu /srv/nfs
# mv /home/<user>/Projects/retroplayer /srv/nfs
```

and make symbolic links back to original folders:

```sh
$ ln -s /srv/nfs/.dosemu /home/<user>/.dosemu
$ ln -s /srv/nfs/retroplayer /home/<user>/Projects/retroplayer
```

Now the content of `/srv/nfs` host's folder is accessible from `/mnt/nfs` VM's folder.

# Using VM build environment

## Enter VM build environment

Start VM. Wait for VM to boot the installed OS then log in as normal user to it via SSH from the host. You can use [scripts/qemu/build-qemu-ssh.sh](../scripts/qemu/build-qemu-ssh.sh) script for that. Enter VM user's password to log in.

In SSH session start *dosemu* in dumb mode (text console):

```sh
$ dosemu -t
```

**Note**: On a virtual DOS environment startup the file `autoexec.bat` is automatically executed. If you used specified scripts earlier to set up DOS environment then `init.bat` script is also executed and the current folder is set to our project's home path. Otherwise you must call `init.bat` script manually. To do so just type `init` and you're done. This should be done once per *dosemu* console session.

Now when DOS paths and all environment variables for current project are set we are ready to compile the project.

## Compile the project in SSH console

In the project's folder type `make all` or any other commands you want to.

## Stop VM from SSH console

In SSH console type `exitemu` to close opened *dosemu* session then shutdown VM (use `su` or `sudo` command in SSH console to act as *root*).

# Links

* [Debian GNU/Linux](https://www.debian.org/) - a free operating system for your computer;
* [sudo](http://www.sudo.ws/) - utility to provide limited super user privileges to specific users ([package](https://pkgs.org/search/?q=sudo));
* [GNU wget](https://www.gnu.org/software/wget/) - a free software for retrieving files using HTTP, HTTPS, FTP and FTPS the most widely-used Internet protocols ([package](https://pkgs.org/search/?q=wget));
* [QEMU](https://www.qemu.org/) - a generic and open source machine emulator and virtualizer ([package](https://pkgs.org/search/?q=qemu));
