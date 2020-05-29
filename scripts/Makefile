## Tools used:
## * GNU core utilities
## * git

ASPP_URL	= https://gitlab.com/ivan-tat/aspp.git
ASPP_DIR	= aspp
ASPP_TOOL	= aspp.sh

.DEFAULT_GOAL = empty

.PHONY: \
 empty \
 download_aspp \
 download_all \
 update_aspp \
 update_all

empty:
	@echo 'Usage:'; \
	echo '	make [ <action>_<tool_name> ... ]'; \
	echo 'Where:'; \
	echo '<action> is one of these:'; \
	echo '	download'; \
	echo '	update'; \
	echo '<tool_name> is one of these:'; \
	echo '	aspp'; \
	echo '	all - for all tools'

$(ASPP_DIR):
	git clone $(ASPP_URL) $(ASPP_DIR)

$(ASPP_DIR)/aspp.sh: $(ASPP_DIR)

$(ASPP_TOOL): $(ASPP_DIR)/aspp.sh
	ln -s $< $@

download_aspp: $(ASPP_TOOL)

download_all: download_aspp

update_aspp: $(ASPP_DIR)
	git -C $< checkout

update_all: update_aspp
