## TODO: Make automatic dependencies of assembler source files.
## HINT: (?) Replace all "/" with "\" in paths and filenames when using DJGPP.

ifneq (,$(DJGPP))
DOS_HOST := yes
else
DOS_HOST :=
endif

INCLUDE_DIRS := .

## Temporary assembler source file extension (without a dot)
ASMTMPEXT := as_

ifneq (,$(DOS_HOST))
tmp := $(subst /,\,$(INCLUDE_DIRS))
INCLUDE_DIRS := $(tmp)
endif

##=----------------------------=##
## Dependency files compilation ##
##=----------------------------=##

## Dependency file's extension (without a dot)

## GNU C Compiler "gcc" (default):
#DEPEXT = d
## Open Watcom C Compiler with GNU syntax "owcc":
## HINT: Do not use "d" as a file's extension when using Open Watcom "owcc" v2.0 beta - it will delete present "*.d" file after a compilation step!
DEPEXT = dep

#DEPCC     = gcc
#DEPCFLAGS = -E -MM -I $(INCLUDE_DIRS)

DEPCC     = owcc
DEPCFLAGS = -zq -E -MM -I $(INCLUDE_DIRS)

##=-----------=##
## Compilation ##
##=-----------=##

AS     := wasm
AFLAGS := -zld -zq -i=$(INCLUDE_DIRS)

DA     := wdis
DFLAGS := -a

#CC     := gcc
#CFLAGS := -I $(INCLUDE_DIRS)

CC     := wcc
CFLAGS := -3 -fp3 -ml -oi -oc -os -q -r -s -zdp -zff -zgf -zl -zld -zls -zp=1 -zq -zu -i=$(INCLUDE_DIRS)

##=--------------=##
## Default target ##
##=--------------=##

.PHONY: empty
empty:
	@echo "Usage:"
	@echo '    make [ all | clean ]'

##=-----------------=##
## Compilation rules ##
##=-----------------=##

## HINT: ORDER MATTERS!

## Native linking:

%.o: %.c
ifeq (wcc,$(CC))
	$(CC) $(CFLAGS) $< -fo=$@
endif
ifeq (gcc,$(CC))
	$(CC) $(CFLAGS) -c $< -o $@
endif

%.o: %.$(ASMTMPEXT)
ifeq (wasm,$(AS))
	$(AS) $(AFLAGS) $< -fo=$@
endif

%.o: %.asm
ifeq (wasm,$(AS))
	$(AS) $(AFLAGS) $< -fo=$@
endif

## Turbo Pascal linking:

%.$(ASMTMPEXT): %.o
ifeq (wdis,$(DA))
	set -e; rm -f $*.$(ASMTMPEXT); \
	$(DA) $(DFLAGS) $< | sed -r -e 's/(^DGROUP[[:space:]]+GROUP[[:space:]]+)CONST,CONST2,(_DATA)/\1\2/;s/^CONST[2]?([[:space:]]+(SEGMENT[[:space:]]+.+*|ENDS[[:space:]]*)$$)/_DATA\1/;s/([[:space:]]|,|:|-|\+)(0[a-f]{1}[[:xdigit:]]{0,7}|[1-9]{1}[[:xdigit:]]{0,7})([[:xdigit:]]{8}H)/\10\3/g;s/(.+\,DGROUP:)CONST$$/\1_DATA/' > $*.$(ASMTMPEXT)
endif

%.obj: %.asm
ifeq (wasm,$(AS))
	$(AS) $(AFLAGS) $< -fo=$@
endif

%.obj: %.$(ASMTMPEXT)
ifeq (wasm,$(AS))
	$(AS) $(AFLAGS) $< -fo=$@
endif

##=---------------------------=##
## Dependency generation rules ##
##=---------------------------=##

## 1. Convert "main.o : main.c defs.h" into "main.o main.$(DEPEXT) : main.c defs.h"
## 2. Add "main.$(ASMTMPEXT) : main.o" dependency.
## 3. Add "main.obj : main.$(ASMTMPEXT)" dependency.
## 4. Optionally: Replace "/" with "\" for DOS.

ifeq (gcc,$(DEPCC))
DEP_SED_SCRIPT = -e 's,\($*\)\.o[ :]*,\1.o $@ : $*.c,g'
endif
ifeq (owcc,$(DEPCC))
DEP_SED_SCRIPT = -e 's,\($*\)\.o[ :]*\($(*F)\),\1.o $@ : $*,g'
endif

ifeq (gcc,$(DEPCC))
%.$(DEPEXT): %.c
	set -e; rm -f $@; \
	$(DEPCC) $(DEPCFLAGS) -MT $*.o -MF $@.$$$$ $<; \
	echo $*.$(ASMTMPEXT) : $*.o >> $@.$$$$; \
	echo $*.obj : $*.$(ASMTMPEXT) >> $@.$$$$; \
	sed $(DEP_SED_SCRIPT) < $@.$$$$ > $@; \
	rm -f $@.$$$$
endif
ifeq (owcc,$(DEPCC))
%.$(DEPEXT): %.c
	set -e; rm -f $@; \
	$(DEPCC) $(DEPCFLAGS) -MT $*.o -MF $@.$$$$ $< > /dev/null; \
	echo $*.$(ASMTMPEXT) : $*.o >> $@.$$$$; \
	echo $*.obj : $*.$(ASMTMPEXT) >> $@.$$$$; \
	sed $(DEP_SED_SCRIPT) < $@.$$$$ > $@; \
	rm -f $@.$$$$
endif

##=---------=##
## Variables ##
##=---------=##

## Naming template:
## <name>_SRCS - source files
## <name>_OBJS - object files
## <name>_DEPS - dependency files
## <name>_TMPS - temporary files

SRCS =
OBJS =
DEPS =
TMPS =

##=--------------=##
## Custom C files ##
##=--------------=##

## FIXME: Manual dependencies of assembler source files
cc/dos/int24.obj \
cc/dos/int24.o: cc/dos/int24.asm
cc/i86/dointr.obj \
cc/i86/dointr.o: cc/i86/dointr.asm
cc/i86/intr.obj \
cc/i86/intr.o: cc/i86/intr.asm

cc_SRCS := \
 $(wildcard cc/*.asm) \
 $(wildcard cc/*.c) \
 $(wildcard cc/conio/*.asm) \
 $(wildcard cc/conio/*.c) \
 $(wildcard cc/ctype/*.asm) \
 $(wildcard cc/ctype/*.c) \
 $(wildcard cc/dos/*.asm) \
 $(wildcard cc/dos/*.c) \
 $(wildcard cc/errno/*.asm) \
 $(wildcard cc/errno/*.c) \
 $(wildcard cc/i86/*.asm) \
 $(wildcard cc/i86/*.c) \
 $(wildcard cc/io/*.asm) \
 $(wildcard cc/io/*.c) \
 $(wildcard cc/malloc/*.asm) \
 $(wildcard cc/malloc/*.c) \
 $(wildcard cc/stdio/*.asm) \
 $(wildcard cc/stdio/*.c) \
 $(wildcard cc/stdlib/*.asm) \
 $(wildcard cc/stdlib/*.c) \
 $(wildcard cc/string/*.asm) \
 $(wildcard cc/string/*.c) \
 $(wildcard cc/unistd/*.asm) \
 $(wildcard cc/unistd/*.c)

cc_OBJS := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(cc_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(cc_SRCS)))

cc_DEPS := $(patsubst %.c,%.$(DEPEXT),$(filter %.c,$(cc_SRCS)))
cc_TMPS :=

## Turbo Pascal target files:
cc_OBJS_TP := $(cc_OBJS:.o=.obj)
cc_OBJS += $(cc_OBJS_TP)
cc_TMPS += $(patsubst %.c,%.$(ASMTMPEXT),$(filter %.c,$(cc_SRCS)))

SRCS += $(cc_SRCS)
OBJS += $(cc_OBJS)
DEPS += $(cc_DEPS)
TMPS += $(cc_TMPS)

##=------------=##
## Common files ##
##=------------=##

## FIXME: Manual dependencies of assembler source files
hw/cpu.obj \
hw/cpu.o: hw/cpu.asm
hw/isr.obj \
hw/isr.o: hw/isr.asm
hw/line.obj \
hw/line.o: hw/line.asm
main/asm/_mixer.obj \
main/asm/_mixer.o: main/asm/_mixer.asm main/asm/_mixer.def
main/asm/sampler.obj \
main/asm/sampler.o: main/asm/sampler.asm main/asm/sampler.def
startup/ints.obj \
startup/ints.o: startup/ints.asm

common_SRCS := \
 $(wildcard *.asm) \
 $(wildcard *.c) \
 $(wildcard dos/*.asm) \
 $(wildcard dos/*.c) \
 $(wildcard hw/*.asm) \
 $(wildcard hw/*.c) \
 $(wildcard hw/sb/*.asm) \
 $(wildcard hw/sb/*.c) \
 $(wildcard main/*.asm) \
 $(wildcard main/*.c) \
 $(wildcard main/asm/*.asm) \
 $(wildcard main/asm/*.c) \
 $(wildcard player/*.asm) \
 $(wildcard player/*.c) \
 $(wildcard player/asm/*.asm) \
 $(wildcard player/asm/*.c) \
 $(wildcard startup/*.asm) \
 $(wildcard startup/*.c)

common_OBJS := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(common_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(common_SRCS)))

common_DEPS := $(patsubst %.c,%.$(DEPEXT),$(filter %.c,$(common_SRCS)))
common_TMPS :=

## Turbo Pascal target files:
common_OBJS_TP := $(common_OBJS:.o=.obj)
common_OBJS += $(common_OBJS_TP)
common_TMPS += $(patsubst %.c,%.$(ASMTMPEXT),$(filter %.c,$(common_SRCS)))

SRCS += $(common_SRCS)
OBJS += $(common_OBJS)
DEPS += $(common_DEPS)
TMPS += $(common_TMPS)

##=------------------------------=##
## Include dependency information ##
##=------------------------------=##

ifneq (,$(DOS_HOST))
#SRCS := $(subst /,\,$(SRCS))
#OBJS := $(subst /,\,$(OBJS))
#DEPS := $(subst /,\,$(DEPS))
#TMPS := $(subst /,\,$(TMPS))
endif

include $(DEPS)

##=-------=##
## Targets ##
##=-------=##

SUCCESS := 'Done.'

all: $(OBJS)
	@echo $(SUCCESS)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(DEPS) $(TMPS)
	@echo $(SUCCESS)
