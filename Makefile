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
TARGET_MK   := Makefile
else ifeq "$(HOST_MODE)" "DJGPP"
TARGET_MK   := Makefile.dos
else
$(error_unsupported_environment)
endif

.DEFAULT_GOAL := empty

.PHONY: empty all clean show_banner
empty all clean: show_banner $(srcdir)/$(TARGET_MK)
	@make -C $(srcdir) -f $(TARGET_MK) $@

show_banner:
	@echo 'Hint: Running in $(HOST_MODE) mode.'
