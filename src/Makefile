## HINT: (?) Replace all "/" with "\" in paths and filenames when using DJGPP.

## Debian packages used: coreutils, sed, findutils.

error_not_implemented = $(error This function is not implemented for current environment.)
error_not_known_params = $(error This function is not implemented for selected parameters.)
info_done = @echo Done.

ifneq "$(DJGPP)" ""
HOST_TYPE       := DJGPP
else
HOST_TYPE       := native
endif

INCLUDE_DIRS    := .

ifeq "$(HOST_TYPE)" "DJGPP"
tmp := $(subst /,\,$(INCLUDE_DIRS))
INCLUDE_DIRS := $(tmp)
endif

DEPDIR_NATIVE   := .deps
DEPDIR_DOS      := _deps.dos

ifeq "$(HOST_TYPE)" "native"
DEPDIR := $(DEPDIR_NATIVE)
else
DEPDIR := $(DEPDIR_DOS)
endif

DEPEXT          := d
DEPAS_DOS		:= ../scripts/aspp/aspp
DEPAFLAGS_DOS	:= -E -MM -I $(INCLUDE_DIRS)
#DEPCC           := gcc
DEPCC           := owcc
ifeq "$(DEPCC)" "gcc"
DEPCFLAGS       := -E -MM -I $(INCLUDE_DIRS)
else ifeq "$(DEPCC)" "owcc"
DEPCFLAGS       := -zq -E -MM -I $(INCLUDE_DIRS)
else
$(error_not_known_params)
endif
DEPCC_DOS       := $(DEPCC)
DEPCFLAGS_DOS   := $(DEPCFLAGS)
ASMTMPEXT_DOS   := as_
AS_DOS          := wasm
AFLAGS_DOS      := -zld -zq -i=$(INCLUDE_DIRS)
DA_DOS          := wdis
DFLAGS_DOS      := -a
#CC              := gcc
CC              := wcl386
ifeq "$(CC)" "gcc"
CFLAGS          := -lm -I $(INCLUDE_DIRS)
else ifeq "$(CC)" "wcl386"
CFLAGS          := -q -3r -fp3 -fpi87 -om -bcl=linux -i=$(INCLUDE_DIRS)
else
$(error_not_known_params)
endif
#CC_DOS          := gcc
CC_DOS          := wcc
ifeq "$(CC_DOS)" "gcc"
ifeq "$(DEBUG)" "1"
CDEBUG_DOS      := -D DEBUG=1
else
CDEBUG_DOS      :=
endif
CFLAGS_DOS      := $(CDEBUG_DOS) -I $(INCLUDE_DIRS)
else ifeq "$(CC_DOS)" "wcc"
ifeq "$(DEBUG)" "1"
CDEBUG_DOS      := -dDEBUG=1
else
CDEBUG_DOS      :=
endif
CFLAGS_DOS      := $(CDEBUG_DOS) -3 -fp3 -ml -oi -oc -os -q -r -s -zdp -zff -zgf -zl -zld -zls -zp=1 -zq -zu -i=$(INCLUDE_DIRS)
else
$(error_not_known_params)
endif

# Description of options for "wasm":
# -zld      suppress file dependency info in object file
# -zq       operate quietly

# Description of options for "owcc":
# -E        preprocess source file
# -M[M]D    output autodepend make rule
# -I <dir>  include directory
# -zq       operate quietly

# Description of options for "wcc":
# -3        386 instructions
# -fp3      generate floating-point code (80387 FPU code)
# -ml       memory model (large - large code/large data)
# -ecc      set default calling convention to __cdecl
# -ecp      set default calling convention to __pascal
# -ecw      set default calling convention to __watcall (default)
# -oc       disable <call followed by return> to <jump> optimization
#           reason: "wdis" incorrectly writes "je near ptr <near_extern_label>"
#           (without "near ptr")
# -oi       expand intrinsic functions inline
# -os       favor code size over execution time in optimizations
# -q        operate quietly (display only error messages)
# -r        save/restore segment registers across calls
# -s        remove stack overflow checks
# -zdp      DS is pegged to DGROUP
# -zff      FS floats (i.e. not fixed to a segment)
# -zgf      GS floats (i.e. not fixed to a segment)
# -zl       remove default library information
# -zld      remove file dependency information
# -zls      remove automatically inserted symbols
# -zp=1     pack structure members with alignment (1)
# -zu       SS != DGROUP (i.e., do not assume stack is in data segment)
# -i=<dir>  include directory

# Description of options for "wcl386":
# -3r       386 register calling conventions
# -fpi87    inline 80x87
# -fp3      generate 387 floating-point code
# -bcl=<os> compile and link for OS
# -q        operate quietly
# -om       generate inline math functions
# -i=<dir>  include directory

## Naming template:
## <name>_DIR(S) - local directory(s)
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

.DEFAULT_GOAL := empty
.PHONY: empty
empty:
	@echo "Usage:"
	@echo '    make [ all | clean ]'

##=----------------=##
## Dependency files ##
##=----------------=##

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

## For "C-source -> object" rules:
## 1. Convert "main.o : main.c defs.h" into "main.o main.$(DEPEXT) : main.c defs.h"
## 2. Add "main.$(ASMTMPEXT_DOS) : main.o" dependency.
## 3. Add "main.obj : main.$(ASMTMPEXT_DOS)" dependency.
## 4. Optionally: Replace "/" with "\" for DOS.

ifeq "$(DEPCC_DOS)" "gcc"
define make_c_obj_dos_deps =
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPCC_DOS) $(DEPCFLAGS_DOS) -MT $*.o -MF $@.$$$$ $<; \
	echo $*.$(ASMTMPEXT_DOS) : $*.o >> $@.$$$$; \
	echo $*.obj : $*.$(ASMTMPEXT_DOS) >> $@.$$$$; \
	sed -e 's,\($*\)\.o[ :]*,\1.o $@ : $*.c,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$
endef
else ifeq "$(DEPCC_DOS)" "owcc"
define make_c_obj_dos_deps =
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPCC_DOS) $(DEPCFLAGS_DOS) -MT $*.o -MF $@.$$$$ $< > /dev/null; \
	echo $*.$(ASMTMPEXT_DOS) : $*.o >> $@.$$$$; \
	echo $*.obj : $*.$(ASMTMPEXT_DOS) >> $@.$$$$; \
	sed -e 's,\($*\)\.o[ :]*\($(*F)\),\1.o $@ : $*,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$
endef
else
$(error_not_known_params)
endif

## Global dependency generation rules:

$(DEPDIR)/%.$(DEPEXT): %.asm
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPAS_DOS) $(DEPAFLAGS_DOS) -MT $*.o -MF $@.$$$$ $<; \
	sed -e 's,\($*\)\.o\([ :]*\),\1.o $@\2,' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

$(DEPDIR)/%.$(DEPEXT): %.c
	$(make_c_obj_dos_deps)

ifeq "$(HOST_TYPE)" "native"
remove_deps = if [ $(DEPDIR) != . ]; then rm -rf $(DEPDIR); fi
#else ifeq "$(HOST_TYPE)" "DJGPP"
#remove_deps = if not "$(DEPDIR)"=="." deltree /y $(DEPDIR)
else
$(error_not_implemented)
endif

##=-----------=##
## Compilation ##
##=-----------=##

ifeq "$(CC)" "gcc"
make_c_exec = $(CC) $(CFLAGS) $< -o $@
else ifeq "$(CC)" "wcl386"
make_c_exec = $(CC) $(CFLAGS) -fo=$*.o -fe=$@ $< && $(RM) $*.o
else
$(error_not_known_params)
endif

## HINT: RULES' ORDER MATTERS!

%.o: %.c
ifeq "$(CC_DOS)" "wcc"
	$(CC_DOS) $(CFLAGS_DOS) $< -fo=$@
else ifeq "$(CC_DOS)" "gcc"
	$(CC_DOS) $(CFLAGS_DOS) -c $< -o $@
else
$(error_not_known_params)
endif

## Used multiple times:
ifeq "$(AS_DOS)" "wasm"
make_asm_obj = $(AS_DOS) $(AFLAGS_DOS) $< -fo=$@
else
$(error_not_known_params)
endif

%.o: %.$(ASMTMPEXT_DOS)
	$(make_asm_obj)

%.o: %.asm
	$(make_asm_obj)

## Turbo Pascal linking:

%.$(ASMTMPEXT_DOS): %.o
ifeq "$(DA_DOS)" "wdis"
	set -e; $(RM) $*.$(ASMTMPEXT_DOS); \
	$(DA_DOS) $(DFLAGS_DOS) $< | sed -r -e 's/(^DGROUP[[:space:]]+GROUP[[:space:]]+)CONST,CONST2,(_DATA)/\1\2/;s/^CONST[2]?([[:space:]]+(SEGMENT[[:space:]]+.+*|ENDS[[:space:]]*)$$)/_DATA\1/;s/([[:space:]]|,|:|-|\+)(0[a-f]{1}[[:xdigit:]]{0,7}|[1-9]{1}[[:xdigit:]]{0,7})([[:xdigit:]]{8}H)/\10\3/g;s/(.+\,DGROUP:)CONST$$/\1_DATA/' > $*.$(ASMTMPEXT_DOS)
else
$(error_not_known_params)
endif

%.obj: %.asm
	$(make_asm_obj)

%.obj: %.$(ASMTMPEXT_DOS)
	$(make_asm_obj)

##=-------------------------=##
## Helper executable scripts ##
##=-------------------------=##

scripts_DIR := main/scripts

## Dependency generation rules:
$(DEPDIR)/$(scripts_DIR)/%.$(DEPEXT): $(scripts_DIR)/%.c
	$(make_c_exec_deps)

## Build rules
$(scripts_DIR)/%: $(scripts_DIR)/%.c
	$(make_c_exec)

#scripts_INCS :=
scripts_SRCS := $(wildcard $(scripts_DIR)/*.c)
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
main/%.inc: $(scripts_DIR)/%
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

#cc_INCS :=
cc_SRCS := $(shell find $(cc_DIR) -type f \( -name '*.asm' -o -name '*.c' \))

cc_OBJS := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(cc_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(cc_SRCS)))

cc_DEPS := \
 $(patsubst %.asm,$(DEPDIR)/%.$(DEPEXT),$(filter %.asm,$(cc_SRCS))) \
 $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(cc_SRCS)))

cc_TMPS :=

## Turbo Pascal target files:
cc_OBJS_TP := $(cc_OBJS:.o=.obj)
cc_OBJS += $(cc_OBJS_TP)
cc_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(cc_SRCS)))

#INCS += $(cc_INCS)
SRCS += $(cc_SRCS)
OBJS += $(cc_OBJS)
DEPS += $(cc_DEPS)
TMPS += $(cc_TMPS)

##=------------=##
## Common files ##
##=------------=##

common_DIRS := \
 . \
 dos \
 hw \
 hw/sb \
 main \
 main/asm \
 player \
 player/asm \
 startup


#common_INCS :=

common_SRCS := \
 $(wildcard $(patsubst %,%/*.asm,$(common_DIRS))) \
 $(wildcard $(patsubst %,%/*.c,$(common_DIRS)))

common_OBJS := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(common_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(common_SRCS)))

common_DEPS := \
 $(patsubst %.asm,$(DEPDIR)/%.$(DEPEXT),$(filter %.asm,$(common_SRCS))) \
 $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(common_SRCS)))

common_TMPS :=

## Turbo Pascal target files:
common_OBJS_TP := $(common_OBJS:.o=.obj)
common_OBJS += $(common_OBJS_TP)
common_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(common_SRCS)))

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

.PHONY: all
all: $(INCS) $(OBJS)
	$(info_done)

.PHONY: clean
clean:
	$(remove_deps)
	$(RM) $(INCS) $(OBJS) $(TMPS)
	$(info_done)
