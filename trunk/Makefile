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

default: all
.PHONY: all clean link install build

RM := (ls FILE && rm FILE) > /dev/null 2>&1 || true

SUBDIRS = boot console kernel lib mm init i386

.PHONY: subdirs $(SUBDIRS) 
subdirs: $(SUBDIRS) 
	@echo
	@$(subst FILE,errors_summary,$(RM))
	@find -name error_log | xargs cat > errors_summary
	@cat errors_summary
	@echo
$(SUBDIRS) :     
	@( [ -d obj ] || mkdir obj )
	@( export DIR=$@ ; $(MAKE) -C $@ build_$@ --no-print-directory --warn-undefined-variables )

# params
ISO_FILE := grub.iso
KERN_BIN := kernel.k
LINKER_SCRIPT := kernel.ld
KERNEL_MAP := kernel.map

# -s for strip all sybols, -x for discard local symbols
LDFLAGS := -T$(LINKER_SCRIPT) -Map $(KERNEL_MAP)
DBG := -gdwarf-2 -DDBG_DWARF2
#DBG := -gstabs -DDBG_STABS
INCLUDE := ../include -I../include/c++ -I../include/c++/c++

#####!!!!!!!!!!!!!!!############

export CC := gcc
export CPP := g++
export LINT := SPLINT
export AS := nasm
export ASFLAGS := -felf

# !-ansi for __asm__
# -mno-stack-arg-probe for alloca
# -g for debug symbols, -O for optimization
# i use pedantic to require gcc extensions to use __extenstion__ or use __ prefix to ease finding non-ansi stuff
# -Wno-unused-parameter to temporarily stops warning about non-used params
# -Wconversion -Wpacked
# i substituted '-ffreestanding' for "-nostdinc -mno-stack-arg-probe -fno-builtin"
export CFLAGS := -Wall -Wextra -Wfloat-equal -Wshadow\
	-Wpadded -Winline -nostdinc\
	-Wunreachable-code -c -pedantic -Wno-unused-parameter\
	-I$(INCLUDE) -std=c99 -ffreestanding -fno-stack-protector $(DBG)

#-nostartfiles -nostdlib -fno-rtti -fno-exceptions
# since i didn't specify -nostartfiles, a fucn called _init will be created to initiate construtors of global objs
# i think i should call it myself, since there is no main, that if the function returns w/out calling main
export CPPFLAGS := \
	-I$(INCLUDE) -Wunreachable-code -c -pedantic -Wno-unused-parameter\
	-Wall -Wextra -Wfloat-equal -Wshadow -nostdinc\
	-Wpadded -Winline -fno-stack-protector\
	-nostartfiles -nostdlib -fno-rtti -fno-exceptions $(DBG) \
	-Weffc++ -Wnon-virtual-dtor -Wold-style-cast 

#####!!!!!!!!!!!!!!!############

all: subdirs link install runBochs

link: $(SUBDIRS)
	@echo linking
	ld obj/*.o -o $(KERN_BIN) $(LDFLAGS)
	@echo

$(KERN_BIN): link

install: $(KERN_BIN)
#	@tar -czf iso/kernel.tgz $(KERN_BIN)
	cp $(KERN_BIN) iso/kernel.tgz	
	./makeiso $(ISO_FILE) 	
	@echo
	
# bochsNOGUI or bochsGUI depending if we want the output to go to the console
# make sure we run it in a subshell so it doesn't get confused with the *.c \
# rules in MakefileCommon
runBochs: 
	@echo building runBochs...
	( gcc -DBOCHSRC=\"bochsGUI\" -Wno-format run.c -o runBochs )
	@echo

files := $(KERN_BIN) iso/kernel.tgz runBochs $(ISO_FILE) $(KERNEL_MAP)
del := $(foreach file,$(files),$(subst FILE,$(file),$(RM));)
clean: $(foreach DIR,$(SUBDIRS),$(DIR).clean)
	@$(del)
	@$(subst FILE,obj/*,$(RM))
	@echo clean done
	@echo
	
%.clean:
	@$(MAKE) -C $* clean_files --no-print-directory

# How to build the Bochs disk image ( from the MTI os lab )
#$(OBJDIR)/kern/bochs.img: $(OBJDIR)/kern/kernel $(OBJDIR)/boot/boot
#	@echo + mk $@
#	$(V)dd if=/dev/zero of=$(OBJDIR)/kern/bochs.img~ count=10000 2>/dev/null
#	$(V)dd if=$(OBJDIR)/boot/boot of=$(OBJDIR)/kern/bochs.img~ conv=notrunc 2>/dev/null
#	$(V)dd if=$(OBJDIR)/kern/kernel of=$(OBJDIR)/kern/bochs.img~ seek=1 conv=notrunc 2>/dev/null
#	$(V)mv $(OBJDIR)/kern/bochs.img~ $(OBJDIR)/kern/bochs.img
