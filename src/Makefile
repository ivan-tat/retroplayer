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
ifeq "$(SYSDEBUG_IOBUF)" "1"
DEFS += SYSDEBUG_IOBUF=1
endif
ifeq "$(SYSDEBUG_ENV)" "1"
DEFS += SYSDEBUG_ENV=1
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
ifeq "$(DEBUG_COMPILE)" "1"
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
ifeq "$(DEBUG_COMPILE)" "1"
CDEBUG_DOS      := -g
else
CDEBUG_DOS      :=
endif
CFLAGS_DOS      := $(CDEFS_DOS) $(CDEBUG_DOS) -I $(INCLUDE_DIRS)
else ifeq "$(CC_DOS)" "wcc"
ifneq "$(DEFS)" ""
CDEFS_DOS       := $(patsubst %,-d%,$(DEFS))
endif
ifeq "$(DEBUG_COMPILE)" "1"
ifeq "$(LINKER_TPC)" "1"
CDEBUG_DOS      := -oi -oc -os
else
CDEBUG_DOS      := -d3 -hw -od
endif
else
CDEBUG_DOS      := -oi -oc -os
endif
CFLAGS_DOS      := $(CDEFS_DOS) $(CDEBUG_DOS) -3 -fp3 -ml -q -r -s -we -zdp -zff -zgf -zl -zld -zls -zp=1 -zq -zu -i=$(INCLUDE_DIRS)
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
ifeq "$(DEBUG_COMPILE)" "1"
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
LIBS_C =
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
	echo '    SYSDEBUG_IOBUF=1'; \
	echo '    SYSDEBUG_ENV=1'; \
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

## For "C-source -> native executable" rules:
## 1. Convert "main : main.c defs.h" into "main main.$(DEPEXT) : main.c defs.h"
## 2. Optionally: Replace "/" with "\" for DOS.

## For "C-source -> DOS executable" rules:
## 1. Convert "main : main.c defs.h" into "main.exe main.$(DEPEXT) : main.c defs.h"
## 2. Optionally: Replace "/" with "\" for DOS.

ifeq "$(DEPCC)" "gcc"

define make_c_exec_deps =
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPCC) $(DEPCFLAGS) -MT $*.o -MF $@.$$$$ $<; \
	sed -e 's,\($*\)\.o[ :]*,\1 $@ : $*.c,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$
endef
define make_c_exec_dos_deps =
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPCC) $(DEPCFLAGS) -MT $*.o -MF $@.$$$$ $<; \
	sed -e 's,\($*\)\.o[ :]*,\1.exe $@ : $*.c,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$
endef

else ifeq "$(DEPCC)" "owcc"

define make_c_exec_deps =
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPCC) $(DEPCFLAGS) -MT $*.o -MF $@.$$$$ $< > /dev/null; \
	sed -e 's,\($*\)\.o[ :]*\($(*F)\)\.c,$(patsubst %.c,%,$<) $@ : $<,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$
endef
define make_c_exec_dos_deps =
	set -e; $(RM) $@; mkdir -p $(@D); \
	$(DEPCC) $(DEPCFLAGS) -MT $*.o -MF $@.$$$$ $< > /dev/null; \
	sed -e 's,\($*\)\.o[ :]*\($(*F)\)\.c,$(patsubst %.c,%.exe,$<) $@ : $<,g' < $@.$$$$ > $@; \
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
 i8d086.o \
 i8m086.o

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
i8m086.obj: i8m086.o
	cp $< $@

ow_OBJS += $(ow_OBJS_TP)

OBJS += $(ow_OBJS)

##=----------------------=##
## Custom C library files ##
##=----------------------=##

cc_DIR := cc

cc_SRCS := $(shell find $(cc_DIR) -type f \( -name '*.asm' -o -name '*.c' \))

cc_OBJS_C := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(cc_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(cc_SRCS)))

cc_DEPS := \
 $(patsubst %.asm,$(DEPDIR)/%.$(DEPEXT),$(filter %.asm,$(cc_SRCS))) \
 $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(cc_SRCS)))

cc_TMPS :=

## Turbo Pascal target files:
cc_OBJS_TP := $(cc_OBJS_C:.o=.obj)
cc_OBJS := $(cc_OBJS_C) $(cc_OBJS_TP)
cc_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(cc_SRCS)))

SRCS += $(cc_SRCS)
OBJS += $(cc_OBJS)
DEPS += $(cc_DEPS)
TMPS += $(cc_TMPS)

##=----------------=##
## Custom C library ##
##=----------------=##

CCLIB := ./cc.lib

ifeq "$(LINKER_TPC)" "1"
else
LIBS_C += $(CCLIB)
endif
LIBS += $(CCLIB)

$(CCLIB): $(cc_OBJS_TP) $(ow_OBJS)
	$(RM) $@; \
	$(AR) $(ARFLAGS) -o $@ $(patsubst %,+%,$^)

##=--------------------=##
## Player library files ##
##=--------------------=##

p_DIRS := \
 . \
 dos \
 hw \
 hw/sb \
 hw/vga \
 main \
 main/asm \
 startup

p_SRCS := $(shell find $(p_DIRS) -maxdepth 1 -type f \( -name '*.asm' -o -name '*.c' \))
p_OBJS_C := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(p_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(p_SRCS)))
p_DEPS := \
 $(patsubst %.asm,$(DEPDIR)/%.$(DEPEXT),$(filter %.asm,$(p_SRCS))) \
 $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(p_SRCS)))
p_TMPS :=

## Turbo Pascal target files:
p_OBJS_TP := $(p_OBJS_C:.o=.obj)
p_OBJS := $(p_OBJS_C) $(p_OBJS_TP)
p_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(p_SRCS)))

SRCS += $(p_SRCS)
OBJS += $(p_OBJS)
DEPS += $(p_DEPS)
TMPS += $(p_TMPS)

##=--------------=##
## Player library ##
##=--------------=##

PLIB := ./p.lib

ifeq "$(LINKER_TPC)" "1"
else
LIBS_C += $(PLIB)
endif
LIBS += $(PLIB)

$(PLIB): $(p_OBJS_TP)
	$(RM) $@; \
	$(AR) $(ARFLAGS) -o $@ $(patsubst %,+%,$^)

##=------------------=##
## Binary executables ##
##=------------------=##

## > test.exe

test_main_SRCS := tests/test.c
test_extra_SRCS :=
test_SRCS := $(test_main_SRCS) $(test_extra_SRCS)
test_OBJS := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(test_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(test_SRCS)))
test_EXES := $(test_main_SRCS:.c=.exe)
test_DEPS := \
 $(patsubst %.asm,$(DEPDIR)/%.$(DEPEXT),$(filter %.asm,$(test_SRCS))) \
 $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(test_SRCS)))
test_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(test_SRCS)))

SRCS += $(test_SRCS)
OBJS += $(test_OBJS)
ifeq "$(LINKER_TPC)" "1"
else
EXES += $(test_EXES)
endif
DEPS += $(test_DEPS)
TMPS += $(test_TMPS)

## Dependency generation rules:
$(DEPDIR)/$(test_main_SRCS:.c=.$(DEPEXT)): $(test_SRCS)
	$(make_c_exec_dos_deps)

## Build rules
$(test_EXES): $(test_OBJS) | $(CCLIB) $(PLIB)
	$(LD) $(LFLAGS) $(patsubst %,library %,$|) $(patsubst %,file %,$^) format dos option nodefaultlibs,start=_start_asm,dosseg,stack=16k,fullheader,map=$(@:.exe=.map) name $@

## > testcrt.exe

testcrt_main_SRCS := tests/testcrt.c
testcrt_extra_SRCS :=
testcrt_SRCS := $(testcrt_main_SRCS) $(testcrt_extra_SRCS)
testcrt_OBJS := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(testcrt_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(testcrt_SRCS)))
testcrt_EXES := $(testcrt_main_SRCS:.c=.exe)
testcrt_DEPS := \
 $(patsubst %.asm,$(DEPDIR)/%.$(DEPEXT),$(filter %.asm,$(testcrt_SRCS))) \
 $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(testcrt_SRCS)))
testcrt_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(testcrt_SRCS)))

SRCS += $(testcrt_SRCS)
OBJS += $(testcrt_OBJS)
ifeq "$(LINKER_TPC)" "1"
else
EXES += $(testcrt_EXES)
endif
DEPS += $(testcrt_DEPS)
TMPS += $(testcrt_TMPS)

## Dependency generation rules:
$(DEPDIR)/$(testcrt_main_SRCS:.c=.$(DEPEXT)): $(testcrt_SRCS)
	$(make_c_exec_dos_deps)

## Build rules
$(testcrt_EXES): $(testcrt_OBJS) | $(CCLIB) $(PLIB)
	$(LD) $(LFLAGS) $(patsubst %,library %,$|) $(patsubst %,file %,$^) format dos option nodefaultlibs,start=_start_asm,dosseg,stack=16k,fullheader,map=$(@:.exe=.map) name $@

## > testexec.exe

testexec_main_SRCS := tests/testexec.c
testexec_extra_SRCS :=
testexec_SRCS := $(testexec_main_SRCS) $(testexec_extra_SRCS)
testexec_OBJS := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(testexec_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(testexec_SRCS)))
testexec_EXES := $(testexec_main_SRCS:.c=.exe)
testexec_DEPS := \
 $(patsubst %.asm,$(DEPDIR)/%.$(DEPEXT),$(filter %.asm,$(testexec_SRCS))) \
 $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(testexec_SRCS)))
testexec_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(testexec_SRCS)))

SRCS += $(testexec_SRCS)
OBJS += $(testexec_OBJS)
ifeq "$(LINKER_TPC)" "1"
else
EXES += $(testexec_EXES)
endif
DEPS += $(testexec_DEPS)
TMPS += $(testexec_TMPS)

## Dependency generation rules:
$(DEPDIR)/$(testexec_main_SRCS:.c=.$(DEPEXT)): $(testexec_SRCS)
	$(make_c_exec_dos_deps)

## Build rules
$(testexec_EXES): $(testexec_OBJS) | $(CCLIB) $(PLIB)
	$(LD) $(LFLAGS) $(patsubst %,library %,$|) $(patsubst %,file %,$^) format dos option nodefaultlibs,start=_start_asm,dosseg,stack=16k,fullheader,map=$(@:.exe=.map) name $@

##=------------------=##
## Player executables ##
##=------------------=##

## > smalls3m.exe

smalls3m_main_SRCS := player/smalls3m.c
smalls3m_extra_SRCS :=
smalls3m_SRCS := $(smalls3m_main_SRCS) $(smalls3m_extra_SRCS)
smalls3m_OBJS_C := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(smalls3m_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(smalls3m_SRCS)))
smalls3m_EXES := $(smalls3m_main_SRCS:.c=.exe)
smalls3m_DEPS := \
 $(patsubst %.asm,$(DEPDIR)/%.$(DEPEXT),$(filter %.asm,$(smalls3m_SRCS))) \
 $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(smalls3m_SRCS)))
smalls3m_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(smalls3m_SRCS)))

## Turbo Pascal target files:
smalls3m_OBJS_TP := $(smalls3m_OBJS_C:.o=.obj)
smalls3m_OBJS := $(smalls3m_OBJS_C) $(smalls3m_OBJS_TP)
smalls3m_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(smalls3m_SRCS)))

SRCS += $(smalls3m_SRCS)
OBJS += $(smalls3m_OBJS)
ifeq "$(LINKER_TPC)" "1"
else
EXES += $(smalls3m_EXES)
endif
DEPS += $(smalls3m_DEPS)
TMPS += $(smalls3m_TMPS)

## Dependency generation rules:
$(DEPDIR)/$(smalls3m_main_SRCS:.c=.$(DEPEXT)): $(smalls3m_SRCS)
	$(make_c_exec_dos_deps)

## Build rules
$(smalls3m_EXES): $(smalls3m_OBJS_TP) | $(CCLIB) $(PLIB)
	$(LD) $(LFLAGS) $(patsubst %,library %,$|) $(patsubst %,file %,$^) format dos option nodefaultlibs,start=_start_asm,dosseg,stack=16k,fullheader,map=$(@:.exe=.map) name $@

## > playosci.exe

playosci_main_SRCS := player/playosci.c
playosci_extra_SRCS :=
playosci_SRCS := $(playosci_main_SRCS) $(playosci_extra_SRCS)
playosci_OBJS_C := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(playosci_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(playosci_SRCS)))
playosci_EXES := $(playosci_main_SRCS:.c=.exe)
playosci_DEPS := \
 $(patsubst %.asm,$(DEPDIR)/%.$(DEPEXT),$(filter %.asm,$(playosci_SRCS))) \
 $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(playosci_SRCS)))
playosci_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(playosci_SRCS)))

## Turbo Pascal target files:
playosci_OBJS_TP := $(playosci_OBJS_C:.o=.obj)
playosci_OBJS := $(playosci_OBJS_C) $(playosci_OBJS_TP)
playosci_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(playosci_SRCS)))

SRCS += $(playosci_SRCS)
OBJS += $(playosci_OBJS)
ifeq "$(LINKER_TPC)" "1"
else
EXES += $(playosci_EXES)
endif
DEPS += $(playosci_DEPS)
TMPS += $(playosci_TMPS)

## Dependency generation rules:
$(DEPDIR)/$(playosci_main_SRCS:.c=.$(DEPEXT)): $(playosci_SRCS)
	$(make_c_exec_dos_deps)

## Build rules
$(playosci_EXES): $(playosci_OBJS_TP) | $(CCLIB) $(PLIB)
	$(LD) $(LFLAGS) $(patsubst %,library %,$|) $(patsubst %,file %,$^) format dos option nodefaultlibs,start=_start_asm,dosseg,stack=16k,fullheader,map=$(@:.exe=.map) name $@

## > plays3m.exe

plays3m_main_SRCS := player/plays3m.c
plays3m_extra_SRCS := \
 player/screen.c \
 $(wildcard player/w_*.c)
plays3m_SRCS := $(plays3m_main_SRCS) $(plays3m_extra_SRCS)
plays3m_OBJS_C := \
 $(patsubst %.asm,%.o,$(filter %.asm,$(plays3m_SRCS))) \
 $(patsubst %.c,%.o,$(filter %.c,$(plays3m_SRCS)))
plays3m_EXES := $(plays3m_main_SRCS:.c=.exe)
plays3m_DEPS := \
 $(patsubst %.asm,$(DEPDIR)/%.$(DEPEXT),$(filter %.asm,$(plays3m_SRCS))) \
 $(patsubst %.c,$(DEPDIR)/%.$(DEPEXT),$(filter %.c,$(plays3m_SRCS)))
plays3m_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(plays3m_SRCS)))

## Turbo Pascal target files:
plays3m_OBJS_TP := $(plays3m_OBJS_C:.o=.obj)
plays3m_OBJS := $(plays3m_OBJS_C) $(plays3m_OBJS_TP)
plays3m_TMPS += $(patsubst %.c,%.$(ASMTMPEXT_DOS),$(filter %.c,$(plays3m_SRCS)))

SRCS += $(plays3m_SRCS)
OBJS += $(plays3m_OBJS)
ifeq "$(LINKER_TPC)" "1"
else
EXES += $(plays3m_EXES)
endif
DEPS += $(plays3m_DEPS)
TMPS += $(plays3m_TMPS)

## Dependency generation rules:
$(DEPDIR)/$(plays3m_main_SRCS:.c=.$(DEPEXT)): $(plays3m_SRCS)
	$(make_c_exec_dos_deps)

## Build rules
$(plays3m_EXES): $(plays3m_OBJS_TP) | $(CCLIB) $(PLIB)
	$(LD) $(LFLAGS) $(patsubst %,library %,$|) $(patsubst %,file %,$^) format dos option nodefaultlibs,start=_start_asm,dosseg,stack=16k,fullheader,map=$(@:.exe=.map) name $@

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
all: autogenfiles $(INCS) $(OBJS) $(LIBS_C) $(EXES)
	$(info_done)

.PHONY: clean
clean:
	$(remove_deps)
	$(RM) $(INCS) $(OBJS) $(LIBS) $(EXES) $(TMPS)
	find . -type f \( -name '*.err' -o -name '*.exe' -o -name '*.map' -o -name '*.tpu' \) -delete
	$(info_done)
