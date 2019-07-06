## HINT: (?) Replace all "/" with "\" in paths and filenames when using DJGPP.

ifneq (,$(DJGPP))
HOST_MODE := DJGPP
else
HOST_MODE := native
endif

srcdir := src

ifeq (DJGPP,$(HOST_MODE))
tmp := $(subst /,\,$(srcdir))
srcdir := $(tmp)
endif

.PHONY: empty
empty: show_banner
	@echo 'Usage:'
	@echo '    make [ all | clean ]'

.PHONY: show_banner
show_banner:
	@echo "Hint: Running in a $(HOST_MODE) mode."

#cd $(srcdir); make $@; cd ..
.PHONY: all
all: show_banner
ifeq (native,$(HOST_MODE))
	make -C $(srcdir) $@
endif
	./make.sh $@

.PHONY: clean
clean: show_banner
ifeq (native,$(HOST_MODE))
	make -C $(srcdir) $@
endif
	./clean.sh
