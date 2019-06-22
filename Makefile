ifeq (,$(DJGPP))
DJGPP = no
else
DJGPP = yes
endif

empty: show_banner
	@echo 'Usage:'
	@echo "    make [all|clean]"

.PHONY: show_banner
show_banner:
	@echo "Running in a DJGPP DOS environment: $(DJGPP)"

.PHONY: all
all: show_banner
	./make.sh

.PHONY: clean
clean: show_banner
	./clean.sh
