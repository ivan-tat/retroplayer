# Supported environments:
#   * GNU/Linux
#
# Description:
#   Extract all needed files form a library.

ifeq "$(WATCOM)" ""
$(error WATCOM environment variable is not set)
endif
CLIB            := $(WATCOM)/lib286/dos/clibl.lib
TMP             := tmp
OBJEXT          := o
WLIB            := wlib
WLIBFLAGS       := -q
DA              := wdis
DFLAGS          :=

LIBLIST         := $(TMP)/$(patsubst %.lib,%.txt,$(notdir $(CLIB)))
OBJS            :=

.DEFAULT_GOAL := empty

.PHONY: empty
empty:
	@echo 'Usage:'; \
	echo '    make [ all | list | files | clean ]'

# "list": List contents of a library in human readable form.
$(LIBLIST): $(CLIB)
	$(WLIB) "$<" | \
	grep -E "^([a-zA-Z0-9_]+)\\.+([a-zA-Z0-9_.]+)$$" | \
	sed -r -e "s/^([a-zA-Z0-9_]+)\\.+([a-zA-Z0-9_.]+)$$/\\2 \\1/" | \
	sort > "$@"

# "files"
extract_noext_obj = $(WLIB) $(WLIBFLAGS) -d=$(@D) $< ':$(patsubst %.$(OBJEXT),%,$(@F))'
extract_asm_obj = $(WLIB) $(WLIBFLAGS) -d=$(@D) $< ':$(patsubst %.$(OBJEXT),%.asm,$(@F))'
extract_c_obj = $(WLIB) $(WLIBFLAGS) -d=$(@D) $< ':$(patsubst %.$(OBJEXT),%.c,$(@F))'

noext_obj_names :=
asm_obj_names :=
c_obj_names :=

# Watcom specific
noext_obj_names += i4d i4m i8d086 i8m086
c_obj_names += xmsg.c cstart.c
# <conio.h>
c_obj_names += inp.c outp.c
# <i86.h>
noext_obj_names += dointr
c_obj_names += intr.c enable.c disable.c delay.c
# <env.h>
c_obj_names += getenv.c
# <dos.h>
c_obj_names += dosret.c d_getvec.c d_setvec.c
asm_obj_names += error086.asm
c_obj_names += dosexter.c
asm_obj_names += gtime086.asm
c_obj_names += exv.c exve.c
c_obj_names += creatdos.c opendos.c filedos.c
asm_obj_names += io086.asm
# <io.h>
c_obj_names += open.c close.c _clse.c read.c write.c
# <stdio.h>
c_obj_names += iob.c iobaddr.c
c_obj_names += fopen.c fclose.c fread.c fwrite.c
# <stdlib.h>
c_obj_names += atexit.c exit.c _exit.c abort.c
c_obj_names += atoi.c atol.c istable.c strtol.c errno.c
noext_obj_names += crwdata
c_obj_names += pspptr.c
# <string.h>
c_obj_names += memcmp.c memcpy.c memset.c
c_obj_names += strchr.c strlen.c strncmp.c strncpy.c strnicmp.c

noext_objs := $(patsubst %,$(TMP)/%.$(OBJEXT),$(noext_obj_names))
asm_objs := $(patsubst %.asm,$(TMP)/%.$(OBJEXT),$(asm_obj_names))
c_objs := $(patsubst %.c,$(TMP)/%.$(OBJEXT),$(c_obj_names))

OBJS += $(noext_objs) $(asm_objs) $(c_objs)
LSTS = $(OBJS:.$(OBJEXT)=.lst)
ASMS = $(OBJS:.$(OBJEXT)=.asm)

FILES = $(OBJS) $(LSTS) $(ASMS)

# rules

$(TMP):
	mkdir -p $(TMP)

$(noext_objs): $(CLIB)
	$(extract_noext_obj)

$(asm_objs): $(CLIB)
	$(extract_asm_obj)

$(c_objs): $(CLIB)
	$(extract_c_obj)

%.lst: %.$(OBJEXT)
	$(DA) -l="$@" "$<"

# Make object files compatible with Pascal linker.
%.asm: %.$(OBJEXT)
	segname="$(patsubst %.$(OBJEXT),%,$(<F))_TEXT"; \
	$(DA) $(DFLAGS) -a "$<" | \
	sed -r -e "s/(^DGROUP[[:space:]]+GROUP[[:space:]]+)CONST,CONST2,(_DATA)/\1\2/;s/^CONST[2]?([[:space:]]+(SEGMENT[[:space:]]+.+*|ENDS[[:space:]]*)$$)/_DATA\\1/;s/^_TEXT([[:space:]]+(SEGMENT[[:space:]]+|ENDS[[:space:]]*))/$$segname\\1/;s/([[:space:]]|,|:|-|\\+)(0[a-f]{1}[[:xdigit:]]{0,7}|[1-9]{1}[[:xdigit:]]{0,7})([[:xdigit:]]{8}H)/\\10\\3/g;s/(.+\\,DGROUP:)CONST$$/\\1_DATA/;s/(^[[:space:]]*ASSUME[[:space:]]+.+:)_TEXT/\\1$$segname/g" > "$@"


.PHONY: list
list: $(TMP) $(LIBLIST)

.PHONY: files
files: $(TMP) $(FILES)

.PHONY: all
all: list files

.PHONY: clean
clean:
	$(RM) $(LIBLIST) $(FILES)
