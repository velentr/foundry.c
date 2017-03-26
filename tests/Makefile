MAKE	?=	make
SUBDIRS	=	binheap bresenham kmp list pheap rbtree
CLEAN	=	$(addsuffix .clean,$(SUBDIRS))

.PHONY: all $(SUBDIRS)

all: $(SUBDIRS)

clean: $(CLEAN)

$(SUBDIRS):
	$(MAKE) -k -C $@

$(CLEAN):
	$(MAKE) -k -C $(basename $@) clean
