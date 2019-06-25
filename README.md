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

* Implement software mixing for AdLib instruments;

* Implement more module formats support: MOD, XM (Fast Tracker 2), IT (Impulse Tracker);

* Port to DOS 32-bit protected mode;

* Port to GNU/Linux;

## Links

* [GNU coding standards](https://www.gnu.org/prep/standards/)

* [GNU Make](https://www.gnu.org/software/make/) (a tool which controls the generation of executables and other non-source files of a program from the program's source files)

* [FreeDOS](http://www.freedos.org/) (a complete, free, DOS-compatible operating system)

* [GnuDOS](https://www.gnu.org/software/gnudos/) (a library designed to help new users of the GNU system, who are coming from a DOS background)

* [DOSEMU](http://www.dosemu.org/) (DOS emulator with LFN support)

* [DOSBox](http://www.dosbox.com/) (DOS emulator with good hardware support)

* [The Doszip Commander](https://sourceforge.net/projects/doszip/) (good NC clone for DOS with LFN support)

* [DJGPP](http://www.delorie.com/djgpp/) (DOS ports of many GNU development utilities)

* [Open Watcom](http://www.openwatcom.org/) (original optimizing C/C++ and assembler compilers for DOS)

* [Open Watcom V2](https://github.com/open-watcom) (fork of Open Watcom compilers for various platforms)

* [Free Pascal](http://freepascal.org/) (advanced open source Pascal compiler for Pascal and Object Pascal for various platforms)

* Borland/Turbo Pascal 7.0 compiler for DOS (will be dropped)

* Sound Blaster Programming Information Version 0.90 (1995) by Andre Baresel & Craig Jackson

* [Sound Blaster Series Hardware Programming Guide](https://pdos.csail.mit.edu/6.828/2008/readings/hardware/SoundBlaster.pdf) (1993-1996) by Creative Technology Ltd.

* [Sound Programming](https://soundprogramming.net/) by Jason Champion (collection of synthesizer, sampler, sequencer, drum machine, keyboard, and guitar synth manuals, specifications, and demos)

* [TECH Help! 6.0](http://www.techhelpmanual.com/) (1994) by Flambeaux Software (DOS Technical Reference and PC, AT, and PS/2 BIOS and Hardware References)

* [The Dark Corner BBS](http://www.dcee.net/) (sound and music file formats description, sound hardware programming, old players and much more)

* [SWAG](http://www.retroarchive.org/swag/) (a free collection of source code and program examples for the PASCAL programming language)
