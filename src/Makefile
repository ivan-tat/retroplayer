## TODO: Make automatic dependencies of assembler source files.
## HINT: (?) Replace all "/" with "\" in paths and filenames when using DJGPP.

## Debian packages used: coreutils, sed, findutils.

error_not_implemented = $(error This function is not implemented for current environment.)
error_not_known_params = $(error This function is not implemented for selected parameters.)
info_done = $(info Done.)

ifneq "$(DJGPP)" ""
HOST_TYPE := DJGPP
else
HOST_TYPE := native
endif

INCLUDE_DIRS := .

## Temporary assembler source file extension (without a dot)
ASMTMPEXT := as_

ifeq "$(HOST_TYPE)" "DJGPP"
tmp := $(subst /,\,$(INCLUDE_DIRS))
INCLUDE_DIRS := $(tmp)
endif

##=----------------------------=##
## Dependency files compilation ##
##=----------------------------=##

DEPDIR_NATIVE := .deps
DEPDIR_DOS    := _deps.dos

ifeq "$(HOST_TYPE)" "native"
DEPDIR := $(DEPDIR_NATIVE)
else
DEPDIR := $(DEPDIR_DOS)
endif

ifeq "$(HOST_TYPE)" "native"
remove_deps = if [ $(DEPDIR) != . ]; then rm -rf $(DEPDIR); fi
#else ifeq "$(HOST_TYPE)" "DJGPP"
#remove_deps = if not "$(DEPDIR)"=="." deltree /y $(DEPDIR)
else
$(error_not_implemented)
endif

## Dependency file's extension (without a dot)

## GNU C Compiler "gcc" (default):
DEPEXT = d
## Open Watcom C Compiler with GNU syntax "owcc":
## HINT: Do not use "d" as a file's extension when using Open Watcom "owcc" v2.0 beta - it will delete present "*.d" file after a compilation step (if they are in the same directory as source files)!
#DEPEXT = dep

#DEPCC     := gcc
#DEPCFLAGS = -E -MM -I $(INCLUDE_DIRS)
DEPCC     := owcc
DEPCFLAGS = -zq -E -MM -I $(INCLUDE_DIRS)

##=-----------=##
## Compilation ##
##=-----------=##

AS     := wasm
AFLAGS = -zld -zq -i=$(INCLUDE_DIRS)

DA     := wdis
DFLAGS := -a

#CC_HOST_EXEC     := gcc
#CFLAGS_HOST_EXEC = -lm -I $(INCLUDE_DIRS)
CC_HOST_EXEC     := wcl386
CFLAGS_HOST_EXEC = -q -3 -fp3 -fpi87 -om -bcl=linux -i=$(INCLUDE_DIRS)

ifeq "$(CC_HOST_EXEC)" "gcc"
make_c_exec = $(CC_HOST_EXEC) $(CFLAGS_HOST_EXEC) $< -o $@
else ifeq "$(CC_HOST_EXEC)" "wcl386"
make_c_exec = $(CC_HOST_EXEC) $(CFLAGS_HOST_EXEC) -fo=$*.o -fe=$@ $< && $(RM) $*.o
else
$(error_not_known_params)
endif

#CC     := gcc
#CFLAGS = -I $(INCLUDE_DIRS)
CC     := wcc
CFLAGS = -3 -fp3 -ml -oi -oc -os -q -r -s -zdp -zff -zgf -zl -zld -zls -zp=1 -zq -zu -i=$(INCLUDE_DIRS)

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
ifeq "$(CC)" "wcc"
	$(CC) $(CFLAGS) $< -fo=$@
else ifeq "$(CC)" "gcc"
	$(CC) $(CFLAGS) -c $< -o $@
else
$(error_not_known_params)
endif

## Used multiple times:
ifeq "$(AS)" "wasm"
make_asm_obj = $(AS) $(AFLAGS) $< -fo=$@
else
$(error_not_known_params)
endif

%.o: %.$(ASMTMPEXT)
	$(make_asm_obj)

%.o: %.asm
	$(make_asm_obj)

## Turbo Pascal linking:

%.$(ASMTMPEXT): %.o
ifeq "$(DA)" "wdis"
	set -e; $(RM) $*.$(ASMTMPEXT); \
	$(DA) $(DFLAGS) $< | sed -r -e 's/(^DGROUP[[:space:]]+GROUP[[:space:]]+)CONST,CONST2,(_DATA)/\1\2/;s/^CONST[2]?([[:space:]]+(SEGMENT[[:space:]]+.+*|ENDS[[:space:]]*)$$)/_DATA\1/;s/([[:space:]]|,|:|-|\+)(0[a-f]{1}[[:xdigit:]]{0,7}|[1-9]{1}[[:xdigit:]]{0,7})([[:xdigit:]]{8}H)/\10\3/g;s/(.+\,DGROUP:)CONST$$/\1_DATA/' > $*.$(ASMTMPEXT)
else
$(error_not_known_params)
endif

%.obj: %.asm
	$(make_asm_obj)

%.obj: %.$(ASMTMPEXT)
	$(make_asm_obj)

##=---------------------------=##
## Dependency generation rules ##
##=---------------------------=##

## For "C-source -> object" rules:
## 1. Convert "main.o : main.c defs.h" into "main.o main.$(DEPEXT) : main.c defs.h"
## 2. Add "main.$(ASMTMPEXT) : main.o" dependency.
## 3. Add "main.obj : main.$(ASMTMPEXT)" dependency.
## 4. Optionally: Replace "/" with "\" for DOS.

ifeq "$(DEPCC)" "gcc"
define make_c_obj_deps =
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPCC) $(DEPCFLAGS) -MT $*.o -MF $@.$$$$ $<; \
	echo $*.$(ASMTMPEXT) : $*.o >> $@.$$$$; \
	echo $*.obj : $*.$(ASMTMPEXT) >> $@.$$$$; \
	sed -e 's,\($*\)\.o[ :]*,\1.o $@ : $*.c,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$
endef
else ifeq "$(DEPCC)" "owcc"
define make_c_obj_deps =
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPCC) $(DEPCFLAGS) -MT $*.o -MF $@.$$$$ $< > /dev/null; \
	echo $*.$(ASMTMPEXT) : $*.o >> $@.$$$$; \
	echo $*.obj : $*.$(ASMTMPEXT) >> $@.$$$$; \
	sed -e 's,\($*\)\.o[ :]*\($(*F)\),\1.o $@ : $*,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$
endef
else
$(error_not_known_params)
endif

## For "C-source -> executable" rules:
## 1. Convert "main : main.c defs.h" into "main main.$(DEPEXT) : main.c defs.h"
## 2. Optionally: Replace "/" with "\" for DOS.

ifeq "$(DEPCC)" "gcc"
define make_c_exec_deps =
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPCC) $(DEPCFLAGS) -MT $*.o -MF $@.$$$$ $<; \
	sed -e 's,\($*\)\.o[ :]*,\1 $@ : $*.c,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$
endef
else ifeq "$(DEPCC)" "owcc"
define make_c_exec_deps =
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPCC) $(DEPCFLAGS) -MT $*.o -MF $@.$$$$ $< > /dev/null; \
	sed -e 's,\($*\)\.o[ :]*\($(*F)\)\.c,$(patsubst %.c,%,$<) $@ : $<,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$
endef
else
$(error_not_known_params)
endif

## Global dependency generation rules:
$(DEPDIR)/%.$(DEPEXT): %.c
	$(make_c_obj_deps)

##=---------=##
## Variables ##
##=---------=##

## Naming template:
## <name>_INCS - source files to be included in other source files
## <name>_SRCS - source files
## <name>_OBJS - object files and binary executable files
## <name>_DEPS - dependency files
## <name>_TMPS - temporary files

INCS =
SRCS =
OBJS =
DEPS =
TMPS =

##=--------------------------=##
## Helper executables scripts ##
##=--------------------------=##

HELPER_EXECS_DIR := main/scripts

## Dependency generation rules:
$(DEPDIR)/$(HELPER_EXECS_DIR)/%.$(DEPEXT): $(HELPER_EXECS_DIR)/%.c
	$(make_c_exec_deps)

## Build rules
$(HELPER_EXECS_DIR)/%: $(HELPER_EXECS_DIR)/%.c
	$(make_c_exec)

#scripts_INCS :=
scripts_SRCS := $(wildcard $(HELPER_EXECS_DIR)/*.c)
scripts_OBJS := $(patsubst %.c,%,$(filter %.c,$(scripts_SRCS)))
scripts_DEPS := $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(scripts_SRCS)))
#scripts_TMPS :=

#INCS += $(scripts_INCS)
SRCS += $(scripts_SRCS)
OBJS += $(scripts_OBJS)
DEPS += $(scripts_DEPS)
#TMPS += $(scripts_TMPS)

## Auto-generated source files
## ===========================

## Build rules
main/%.inc: $(HELPER_EXECS_DIR)/%
	./$< > $@

autogen_INCS := \
 main/wramp.inc \
 main/wsinus.inc \
 main/wsquare.inc

#autogen_SRCS :=
#autogen_OBJS :=
#autogen_DEPS :=
#autogen_TMPS :=

INCS += $(autogen_INCS)
#SRCS += $(autogen_SRCS)
#OBJS += $(autogen_OBJS)
#DEPS += $(autogen_DEPS)
#TMPS += $(autogen_TMPS)

##=--------------=##
## Custom C files ##
##=--------------=##

cc_DIR := cc

## FIXME: Manual dependencies of assembler source files
$(cc_DIR)/dos/int24.obj \
$(cc_DIR)/dos/int24.o: $(cc_DIR)/dos/int24.asm
$(cc_DIR)/i86/dointr.obj \
$(cc_DIR)/i86/dointr.o: $(cc_DIR)/i86/dointr.asm
$(cc_DIR)/i86/intr.obj \
$(cc_DIR)/i86/intr.o: $(cc_DIR)/i86/intr.asm

#cc_INCS :=
cc_SRCS := $(shell find $(cc_DIR) -type f \( -name '*.asm' -o -name '*.c' \))

cc_OBJS := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(cc_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(cc_SRCS)))

cc_DEPS := $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(cc_SRCS)))
cc_TMPS :=

## Turbo Pascal target files:
cc_OBJS_TP := $(cc_OBJS:.o=.obj)
cc_OBJS += $(cc_OBJS_TP)
cc_TMPS += $(patsubst %.c,%.$(ASMTMPEXT),$(filter %.c,$(cc_SRCS)))

#INCS += $(cc_INCS)
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

#common_INCS :=
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

common_DEPS := $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(common_SRCS)))
common_TMPS :=

## Turbo Pascal target files:
common_OBJS_TP := $(common_OBJS:.o=.obj)
common_OBJS += $(common_OBJS_TP)
common_TMPS += $(patsubst %.c,%.$(ASMTMPEXT),$(filter %.c,$(common_SRCS)))

#INCS += $(common_INCS)
SRCS += $(common_SRCS)
OBJS += $(common_OBJS)
DEPS += $(common_DEPS)
TMPS += $(common_TMPS)

##=------------------------------=##
## Include dependency information ##
##=------------------------------=##

ifeq "$(HOST_TYPE)" "DJGPP"
#INCS := $(subst /,\,$(INCS))
#SRCS := $(subst /,\,$(SRCS))
#OBJS := $(subst /,\,$(OBJS))
#DEPS := $(subst /,\,$(DEPS))
#TMPS := $(subst /,\,$(TMPS))
endif

ifeq "$(MAKECMDGOALS)" "clean"
else
include $(DEPS)
endif

##=-------=##
## Targets ##
##=-------=##

all: $(INCS) $(OBJS)
	$(info_done)

.PHONY: clean
clean:
	$(remove_deps)
	$(RM) $(INCS) $(OBJS) $(TMPS)
	$(info_done)
