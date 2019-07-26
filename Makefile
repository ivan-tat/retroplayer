## Supported environments:
##   * GNU/Linux
##   * DOS/DJGPP

error_unsupported_environment = $(error Current environment is not supported)

ifneq "$(DJGPP)" ""
HOST_MODE   := DJGPP
SHELL       = bash.exe
else
HOST_MODE   := native
endif
srcdir      := src
ifeq "$(HOST_MODE)" "native"
TARGET_MK   := $(srcdir)/Makefile
else ifeq "$(HOST_MODE)" "DJGPP"
TARGET_MK   := $(srcdir)/Makefile.dos
else
$(error_unsupported_environment)
endif

.DEFAULT_GOAL := empty

.PHONY: empty autogenfiles all clean show_banner

empty autogenfiles clean: $(TARGET_MK) | show_banner
	@make -C $(<D) -f $(<F) $@

all: $(TARGET_MK) | show_banner autogenfiles
	@make -C $(<D) -f $(<F) $@

show_banner:
	@echo 'Hint: Running in $(HOST_MODE) mode.'
