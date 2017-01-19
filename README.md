# retroplayer

This is a simple 16-bit DOS text-mode S3M (Scream Tracker 3) player with expanded memory (EMS) support for a family of Sound Blaster sound cards. It was written by Andre Baresel ("Cyder of Green Apple" as he wrote) in 1994 and distributed as a part of "Sound Blaster Programming Information Version 0.90 - 1995 by Andre Baresel & Craig Jackson" in a zip-archive named `sblast09.zip`. Yes, it's old enough.

It was written in Pascal with a lot of hard-coded assembling. The source code is ugly and obfuscated - but it compiles and even works. To make all things work you will need DOS environment and some retro compilers. As a host OS I use GNU/Linux.

When I was playing around this program at university I was able to implement these nice features on Am486DX-100 processor:

* Loadable sound card drivers (for Sound Blasters and for my ESS AudioDrive);

* 16-bit mixing;

* Sample filtering.

My interests in this project:

* Just for fun;

* Clean code: split code, remove all unneeded assembler optimizations and obfuscations;

* Make it portable: completely rewrite in C;

* Keep it simple for portability;

* Practice Git;

* Practice good programming technics.

Plans for the future:

* Implement more module formats support: MOD, XM (Fast Tracker 2), IT (Impulse Tracker);

* Port to DOS 32-bit protected mode;

* Port to GNU/Linux;

## Links

* [DOSEMU](http://www.dosemu.org/) (DOS emulator with LFN support)

* [DOSBox](http://www.dosbox.com/) (DOS emulator with good hardware support)

* [DJGPP](http://www.delorie.com/djgpp/) (DOS ports of many GNU development utilities)

* [Open Watcom](http://www.openwatcom.org/) (optimizing C/C++ and assembler compilers for DOS)

* [The Doszip Commander](https://sourceforge.net/projects/doszip/) (good NC clone for DOS with LFN support)

* Turbo Pascal 7.0 compiler for DOS (will be dropped)

* Turbo Assembler 3.0 compiler for DOS (dropped, use `wasm`)

* Sound Blaster Programming Information Version 0.90 (1995) by Andre Baresel & Craig Jackson

* TECH Help! 6.0 (1994) by Flambeaux Software
