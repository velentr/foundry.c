MAKE	?=	make
SUBDIRS	=	bheap list
CLEAN	=	$(addsuffix .clean,$(SUBDIRS))

.PHONY: all $(SUBDIRS)

all: $(SUBDIRS)

clean: $(CLEAN)

$(SUBDIRS):
	$(MAKE) -k -C $@

$(CLEAN):
	$(MAKE) -k -C $(basename $@) clean
