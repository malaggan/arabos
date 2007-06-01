# Copyright (C) 2007  Mohammad Nabil 
# mohammad (dot) nabil (dot) h (at) gmail (dot) com
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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

build: $(MODULES:%.$(MODULE_SUFFIX)=%.build) kernel.k

kernel.k: $(wildcard $(MODULES:%.$(MODULE_SUFFIX)=%/*.o))
	@echo
	@echo linking
	@ld $(MODULES:%.$(MODULE_SUFFIX)=%/*.o) -o kernel.k $(LDFLAGS)
	@echo linking success
	

install:
	@echo
#	@tar -czf iso/kernel.tgz kernel.k 
#	need to check if "iso" folder exists
	@cp kernel.k iso/kernel.tgz
	@echo
	@echo kernel was put in iso
	@echo
	@./makeiso
	@echo iso made
	@echo
	
clean: $(MODULES:%.$(MODULE_SUFFIX)=%.clean)
	@echo
	@echo dependecies files cleaned.