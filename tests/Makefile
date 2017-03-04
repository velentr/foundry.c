MAKE	?=	make
SUBDIRS	=	binheap list pheap
CLEAN	=	$(addsuffix .clean,$(SUBDIRS))

.PHONY: all $(SUBDIRS)

all: $(SUBDIRS)

clean: $(CLEAN)

$(SUBDIRS):
	$(MAKE) -k -C $@

$(CLEAN):
	$(MAKE) -k -C $(basename $@) clean
