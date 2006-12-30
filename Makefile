#### REMEMBER TO USE SPLINT ####

# this is left empty
MODULE_NAME :=

# this too is left empty
SRCS :=

SHELL := /bin/bash

include MakefileCommon

.PHONY: all clean

# can be anything
MODULE_SUFFIX := mod

MODULES := boot.$(MODULE_SUFFIX) \
		console.$(MODULE_SUFFIX) \
		kernel.$(MODULE_SUFFIX) \
		lib.$(MODULE_SUFFIX) \
		mm.$(MODULE_SUFFIX) \
		init.$(MODULE_SUFFIX) \
		i386.$(MODULE_SUFFIX)

all: build install

%.build:	
	@echo ================================================================
	@echo -	building $*
	@cd $* && make build_$* --no-print-directory
	@echo -	compilation success
	
%.clean:
	@cd $*; make clean_files --no-print-directory

build: $(MODULES:%.mod=%.build) kernel.k

kernel.k: $(wildcard $(MODULES:%.$(MODULE_SUFFIX)=%/*.o))
	@echo
	@echo linking
	@ld $(MODULES:%.mod=%/*.o) -o kernel.k $(LDFLAGS)
	@echo linking success
	

install:
	@echo
	@tar -czf iso/kernel.tgz kernel.k 
#	@cp kernel.k iso/kernel.tgz
	@echo
	@echo kernel was put in iso
	@echo
	@./makeiso
	@echo iso made
	@echo
	
clean: $(MODULES:%.mod=%.clean)
	@echo
	@echo dependecies files cleaned.
