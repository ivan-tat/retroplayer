## Supported environments:
##   * GNU/Linux
##
## Notes:
##   * Debian packages used: coreutils, sed, findutils.

ifneq "$(DJGPP)" ""
$(error This script does not support DJGPP DOS environment)
endif

error_not_implemented = $(error This function is not implemented for current environment.)
error_not_known_params = $(error This function is not implemented for selected parameters.)
info_done = @echo Done.

DEFS :=
ifeq "$(SYSDEBUG)" "1"
DEFS += SYSDEBUG=1
endif
ifeq "$(DEBUG)" "1"
DEFS += DEBUG=1
endif
ifeq "$(LINKER_TPC)" "1"
DEFS += LINKER_TPC=1
endif

HOST_TYPE       := native
INCLUDE_DIRS    := .
DEPDIR          := .deps
DEPEXT          := d
DEPAS_DOS       := ../scripts/aspp/aspp
DEPAFLAGS_DOS   := -E -MM -I $(INCLUDE_DIRS)
#DEPCC           := gcc
DEPCC           := owcc
DEPCDEFS        :=
ifeq "$(DEPCC)" "gcc"
ifneq "$(DEFS)" ""
DEPCDEFS        := $(patsubst %,-D %,$(DEFS))
endif
DEPCFLAGS       := $(DEPCDEFS) -E -MM -I $(INCLUDE_DIRS)
else ifeq "$(DEPCC)" "owcc"
ifneq "$(DEFS)" ""
DEPCDEFS        := $(patsubst %,-D%,$(DEFS))
endif
DEPCFLAGS       := -zq -E -MM $(DEPCDEFS) -I $(INCLUDE_DIRS)
else
$(error_not_known_params)
endif
DEPCC_DOS       := $(DEPCC)
DEPCFLAGS_DOS   := $(DEPCFLAGS)
ASMTMPEXT_DOS   := as_
AS_DOS          := wasm
ifeq "$(DEBUG)" "1"
ifeq "$(LINKER_TPC)" "1"
ADEBUG_DOS      :=
else
ADEBUG_DOS      := -d1
endif
else
ADEBUG_DOS      :=
endif
AFLAGS_DOS      := $(ADEBUG_DOS) -zld -zq -i=$(INCLUDE_DIRS)
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
CDEFS_DOS       :=
ifeq "$(CC_DOS)" "gcc"
ifneq "$(DEFS)" ""
CDEFS_DOS       := $(patsubst %,-D %,$(DEFS))
endif
ifeq "$(DEBUG)" "1"
CDEBUG_DOS      := -g
else
CDEBUG_DOS      :=
endif
CFLAGS_DOS      := $(CDEFS_DOS) $(CDEBUG_DOS) -I $(INCLUDE_DIRS)
else ifeq "$(CC_DOS)" "wcc"
ifneq "$(DEFS)" ""
CDEFS_DOS       := $(patsubst %,-d%,$(DEFS))
endif
ifeq "$(DEBUG)" "1"
ifeq "$(LINKER_TPC)" "1"
CDEBUG_DOS      :=
else
CDEBUG_DOS      := -d3 -hw
endif
else
CDEBUG_DOS      :=
endif
CFLAGS_DOS      := $(CDEFS_DOS) $(CDEBUG_DOS) -3 -fp3 -ml -oi -oc -os -q -r -s -we -zdp -zff -zgf -zl -zld -zls -zp=1 -zq -zu -i=$(INCLUDE_DIRS)
else
$(error_not_known_params)
endif
WLIB            := wlib
WLIBFLAGS       := -q
CLIB_WATCOM     := $(WATCOM)/lib286/dos/clibl.lib
AR              := wlib
ARFLAGS         := -q -t -zld
LD              := wlink
LFLAGS          := option quiet
ifeq "$(DEBUG)" "1"
ifeq "$(LINKER_TPC)" "1"
else
LFLAGS          += debug Watcom All
endif
endif

# Description of options for "wasm":
# -zld      suppress file dependency info in object file
# -zq       operate quietly
# DEBUG:
# -d1       line number debugging support

# Description of options for "owcc":
# -E        preprocess source file
# -M[M]D    output autodepend make rule
# -I <dir>  include directory
# -zq       operate quietly

# Description of options for "wcc":
# -3        386 instructions
# -ecc      set default calling convention to __cdecl
# -ecp      set default calling convention to __pascal
# -ecw      set default calling convention to __watcall (default)
# -fp3      generate floating-point code (80387 FPU code)
# -ml       memory model (large - large code/large data)
# -oc       disable <call followed by return> to <jump> optimization
#           reason: "wdis" incorrectly writes "je near ptr <near_extern_label>"
#           (without "near ptr")
# -oi       expand intrinsic functions inline
# -os       favor code size over execution time in optimizations
# -q        operate quietly (display only error messages)
# -r        save/restore segment registers across calls
# -s        remove stack overflow checks
# -we       treat all warnings as errors
# -zdp      DS is pegged to DGROUP
# -zff      FS floats (i.e. not fixed to a segment)
# -zgf      GS floats (i.e. not fixed to a segment)
# -zl       remove default library information
# -zld      remove file dependency information
# -zls      remove automatically inserted symbols
# -zp=1     pack structure members with alignment (1)
# -zu       SS != DGROUP (i.e., do not assume stack is in data segment)
# -i=<dir>  include directory
# DEBUG:
# -d2       symbolic information
# -db       generate browsing information
# -hc       generate Codeview debugging information
# -hd       generate DWARF debugging information
# -hw       generate Watcom debugging information

# Description of options for "wcl386":
# -3r       386 register calling conventions
# -fpi87    inline 80x87
# -fp3      generate 387 floating-point code
# -bcl=<os> compile and link for OS
# -q        operate quietly
# -om       generate inline math functions
# -i=<dir>  include directory

# Description of options for "wlib":
# -q        don't print header
# -t        trim THEADR pathnames
# -zld      strip file dependency info

## Naming template:
## <name>_DIR(S) - local directory(s)
## <name>_INCS - source files to be included in other source files
## <name>_SRCS - source files
## <name>_OBJS - object files files
## <name>_LIBS - library files
## <name>_EXES - executable files
## <name>_DEPS - dependency files
## <name>_TMPS - temporary files

INCS =
SRCS =
OBJS =
LIBS =
EXES =
DEPS =
TMPS =

.DEFAULT_GOAL := empty

.PHONY: empty
empty:
	@echo 'Usage:'; \
	echo '    make [ option ... ] [ target ] [ option ... ]'; \
	echo 'Where:'; \
	echo '    option: in the form "name=value"'; \
	echo '    target: "autogenfiles" | "all" | "clean"'; \
	echo 'Supported options:'; \
	echo '    SYSDEBUG=1'; \
	echo '    DEBUG=1'; \
	echo '    LINKER_TPC=1'

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

scripts_SRCS := $(wildcard $(scripts_DIR)/*.c)
scripts_OBJS := $(patsubst %.c,%,$(filter %.c,$(scripts_SRCS)))
scripts_DEPS := $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(scripts_SRCS)))

SRCS += $(scripts_SRCS)
OBJS += $(scripts_OBJS)
DEPS += $(scripts_DEPS)

##=---------------------------=##
## Auto-generated source files ##
##=---------------------------=##

## Build rules
main/%.inc: $(scripts_DIR)/%
	./$< > $@

autogen_INCS := \
 main/wramp.inc \
 main/wsinus.inc \
 main/wsquare.inc

INCS += $(autogen_INCS)

##=--------------------=##
## Auto-generated files ##
##=--------------------=##

.PHONY: autogenfiles
autogenfiles: $(INCS)
	$(info_done)

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

ifeq "$(LINKER_TPC)" "1"

## Turbo Pascal linking:

%.$(ASMTMPEXT_DOS): %.o
ifeq "$(DA_DOS)" "wdis"
	set -e; $(RM) $*.$(ASMTMPEXT_DOS); \
	$(DA_DOS) $(DFLAGS_DOS) $< | sed -r -e 's/(^DGROUP[[:space:]]+GROUP[[:space:]]+)CONST,CONST2,(_DATA)/\1\2/;s/^CONST[2]?([[:space:]]+(SEGMENT[[:space:]]+.+*|ENDS[[:space:]]*)$$)/_DATA\1/;s/([[:space:]]|,|:|-|\+|\()(0[a-f]{1}[[:xdigit:]]{0,7}|[1-9]{1}[[:xdigit:]]{0,7})([[:xdigit:]]{8}H)/\10\3/g;s/(.+\,DGROUP:)CONST$$/\1_DATA/' > $*.$(ASMTMPEXT_DOS)
else
$(error_not_known_params)
endif

else

## Watcom linking:

%.$(ASMTMPEXT_DOS): %.o
ifeq "$(DA_DOS)" "wdis"
	set -e; $(RM) $*.$(ASMTMPEXT_DOS); \
	$(DA_DOS) $(DFLAGS_DOS) $< | sed -r -e 's/([[:space:]]|,|:|-|\+|\()(0[a-f]{1}[[:xdigit:]]{0,7}|[1-9]{1}[[:xdigit:]]{0,7})([[:xdigit:]]{8}H)/\10\3/g' > $*.$(ASMTMPEXT_DOS)
else
$(error_not_known_params)
endif

endif

%.obj: %.asm
	$(make_asm_obj)

ifeq "$(LINKER_TPC)" "1"

## Turbo Pascal linking:

%.obj: %.$(ASMTMPEXT_DOS)
	$(make_asm_obj)

else

## Watcom linking:

%.obj: %.o
	cp $< $@

endif

##=----------------------------=##
## Open Watcom C specific files ##
##=----------------------------=##

ow_OBJS := \
 i4d.o \
 i4m.o \
 i8d086.o

## Build rules
$(ow_OBJS): $(CLIB_WATCOM)
	$(WLIB) $(WLIBFLAGS) $< ':$(patsubst %.o,%.asm,$@)'

## Turbo Pascal target files:
ow_OBJS_TP := $(ow_OBJS:.o=.obj)

## Build rules
i4d.obj: i4d.o
	cp $< $@
i4m.obj: i4m.o
	cp $< $@
i8d086.obj: i8d086.o
	cp $< $@

ow_OBJS += $(ow_OBJS_TP)

OBJS += $(ow_OBJS)

##=--------------=##
## Custom C files ##
##=--------------=##

cc_DIR := cc

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

SRCS += $(cc_SRCS)
OBJS += $(cc_OBJS)
DEPS += $(cc_DEPS)
TMPS += $(cc_TMPS)

##=----------------=##
## Custom C library ##
##=----------------=##

cclib_DIR := $(cc_DIR)

CCLIB := $(cclib_DIR)/cc.lib

LIBS += $(CCLIB)

$(CCLIB): $(cc_OBJS_TP)
	$(RM) $@; \
	$(AR) $(ARFLAGS) -o $@ $(patsubst %,+%,$(cc_OBJS_TP))

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

SRCS += $(common_SRCS)
OBJS += $(common_OBJS)
DEPS += $(common_DEPS)
TMPS += $(common_TMPS)

##=------------------=##
## Binary executables ##
##=------------------=##

## All tests

tests_DIRS := tests
tests_SRCS := $(wildcard $(patsubst %,%/*.c,$(tests_DIRS)))
tests_OBJS := $(patsubst %.c,%.o,$(filter %.c,$(tests_SRCS)))
tests_EXES := $(patsubst %.c,%.exe,$(filter %.c,$(tests_SRCS)))
tests_DEPS := $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(tests_SRCS)))
tests_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(tests_SRCS)))

SRCS += $(tests_SRCS)
OBJS += $(tests_OBJS)
ifeq "$(LINKER_TPC)" "1"
else
EXES += $(tests_EXES)
endif
DEPS += $(tests_DEPS)
TMPS += $(tests_TMPS)

## Common objects for all tests

tests_common_OBJS := \
 startup/dosexe16.o \
 i4d.o \
 i4m.o \
 i8d086.o \
 commdbg.o \
 sysdbg.o \
 startup.o \
 startup/ints.o \
 hw/cpu.o

## test.exe

test_exe_SRCS := $(filter %/test.c,$(tests_SRCS))
test_exe_OBJS := $(patsubst %.c,%.o,$(filter %.c,$(test_exe_SRCS)))
test_exe_EXES := $(patsubst %.c,%.exe,$(filter %.c,$(test_exe_SRCS)))

$(test_exe_EXES): $(CCLIB)
	$(LD) $(LFLAGS) library $< $(patsubst %,file %,$(tests_common_OBJS)) $(patsubst %,file %,$(test_exe_OBJS)) format dos option nodefaultlibs,start=_start_asm,dosseg,stack=16k,map,fullheader name $@

##=------------------------------=##
## Include dependency information ##
##=------------------------------=##

ifeq "$(MAKECMDGOALS)" "all"
include $(DEPS)
endif

##=-------=##
## Targets ##
##=-------=##

.PHONY: all
all: autogenfiles $(INCS) $(OBJS) $(LIBS) $(EXES)
	$(info_done)

.PHONY: clean
clean:
	$(remove_deps)
	$(RM) $(INCS) $(OBJS) $(LIBS) $(EXES) $(TMPS)
	find . -type f \( -name '*.err' -o -name '*.exe' -o -name '*.map' -o -name '*.tpu' \) -delete
	$(info_done)
