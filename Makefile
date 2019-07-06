## Supported environments:
##   * GNU/Linux
##   * DOS/DJGPP

error_not_implemented = $(error This function is not implemented for current environment)

ifneq "$(DJGPP)" ""
HOST_MODE   := DJGPP
SHELL       = bash.exe
else
HOST_MODE   := native
endif
srcdir      := src

.DEFAULT_GOAL := empty

.PHONY: empty
empty: show_banner
	@echo 'Usage:'; \
	echo '    make [ all | clean ]'

.PHONY: show_banner
show_banner:
	@echo 'Hint: Running in $(HOST_MODE) mode.'

.PHONY: all

ifeq "$(HOST_MODE)" "native"

all: show_banner $(srcdir)/Makefile
	make -C $(srcdir) -f Makefile $@
	./make.sh $@

else ifeq "$(HOST_MODE)" "DJGPP"

all: show_banner $(srcdir)/Makefile.dos
	make -C $(srcdir) -f Makefile.dos $@

else
$(error_not_implemented)
endif

.PHONY: clean
clean: show_banner
ifeq "$(HOST_MODE)" "native"
	make -C $(srcdir) $@
else
	$(error_not_implemented)
endif
