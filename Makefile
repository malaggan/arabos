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


default: all
.PHONY: all clean link install build TAGS cloc

TAGS:
	find . -name '*.c' -or -name '*.h' -or -name '*.hh' -or -name '*.cpp' -or -name '*.S' | xargs etags	
# source code statistics:
cloc:
	find . -name '*.c' -or -name '*.h' -or -name '*.hh' -or -name '*.cpp' -or -name '*.S' | xargs cloc	

RM := (ls FILE && rm FILE) > /dev/null 2>&1 || true
JOBS := 4

SUBDIRS = boot console kernel lib mm init i386

.PHONY: subdirs $(SUBDIRS)
subdirs:
	-@$(MAKE) --jobs=$(JOBS) --keep-going --load-average=0.75 --no-print-directory $(SUBDIRS) 2>&1 | grep -iv 'make\[2\]' | sed 's/make\[1\]: \*\*\* //'
	@echo
	@$(subst FILE,errors_summary,$(RM))
	@$(FIND) -name error_log | xargs cat > errors_summary
	@cat errors_summary
	@echo
$(SUBDIRS) :
	@( [ -d obj ] || mkdir obj )
	@( export DIR=$@ ; $(MAKE) -C $@ build_$@ --no-print-directory --warn-undefined-variables )

# params
ISO_FILE := grub.iso
ISO_FLAGS := -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table
KERN_BIN := kernel.k
LINKER_SCRIPT := kernel.ld
KERNEL_MAP := kernel.map

UNHOSTED := -nostartfiles -nostdinc -nostdlib -ffreestanding
# -s for strip all sybols, -x for discard local symbols
LDFLAGS := $(UNHOSTED) -Wl,-T$(LINKER_SCRIPT) -Wl,-Map -Wl,$(KERNEL_MAP) # -Wl,-print-memory-usage
#DBG := -gdwarf-2 -DDBG_DWARF2
DBG := -O0 -ggdb3 -DDBG_DWARF2 #-gstabs -DDBG_STABS
INCLUDE := include -I../include -I../include/c++ -I../include/c++/c++

#####!!!!!!!!!!!!!!!############d
export CC := ~/opt/bin/i386-elf-gcc
export AS := ~/opt/bin/i386-elf-gcc
export CXX := ~/opt/bin/i386-elf-g++
export LINT := SPLINT
#export ASFLAGS := -felf
export FIND := find

export ASFLAGS := -c -m32 -I$(INCLUDE) -ggdb3 -DDBG_DWARF2 #-gstabs -DDBG_STABS

# !-ansi for __asm__
# -mno-stack-arg-probe for alloca
# i use pedantic to require gcc extensions to use __extenstion__ or use __ prefix to ease finding non-ansi stuff
# -Wno-unused-parameter to temporarily stops warning about non-used params
# -Wconversion -Wpacked
# i substituted '-ffreestanding' for "-nostdinc -mno-stack-arg-probe -fno-builtin"
export CFLAGS := -c -O0 -m32 -std=c11 -Wc99-c11-compat\
	$(UNHOSTED) -I$(INCLUDE) $(DBG) -fno-stack-protector\
	-Wall -Wextra -pedantic -Wfloat-equal -Wshadow -Wpadded -Winline\
	-Wunreachable-code -Wno-unused-parameter

#WARN: do not use inline impl of classes in header files as it creates sections that breaks the stabs lookup
#-nostartfiles -nostdlib -fno-rtti -fno-exceptions
# since i didn't specify -nostartfiles, a fucn called _init will be created to initiate construtors of global objs
# i think i should call it myself, since there is no main, that if the function returns w/out calling main
export CXXFLAGS := -c -O0 -m32 -std=c++14 -Wc++14-compat\
	$(UNHOSTED) -fno-rtti -fno-exceptions -I$(INCLUDE) $(DBG)\
	-Wall -Wextra -pedantic -Wunreachable-code -Wno-unused-parameter\
	-Wfloat-equal -Wshadow -Wpadded -Winline -fno-stack-protector\
	-Weffc++ -Wnon-virtual-dtor -Wold-style-cast

#####!!!!!!!!!!!!!!!############

all: cloc TAGS subdirs link install #runBochs


## http://wiki.osdev.org/Calling_Global_Constructors
CRTI_OBJ=obj/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o) # for this to work, we need 32bit gcc (a cross compiler if our system is not 32bit)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=obj/crtn.o

OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(filter-out $(CRTI_OBJ) $(CRTN_OBJ),$(wildcard obj/*.o)) $(CRTEND_OBJ) $(CRTN_OBJ)

#myos.kernel: $(OBJ_LINK_LIST)
#	$(CC) -o myos.kernel $(OBJ_LINK_LIST) -nostdlib -lgcc
##

link: $(SUBDIRS)
	@echo linking
	$(CC) $(OBJ_LINK_LIST) -o $(KERN_BIN) $(LDFLAGS)
	~/opt/bin/i386-elf-nm $(KERN_BIN) > kernel.sym # for bochs internal debugger
	@echo

$(KERN_BIN): link

install: $(KERN_BIN)
#	@tar -czf iso/kernel.tgz $(KERN_BIN)
	cp $(KERN_BIN) iso/kernel.tgz
	mkisofs $(ISO_FLAGS) -o $(ISO_FILE) iso >/dev/null 2>&1
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

debug:
	~/opt/bin/i386-elf-gdb --tui kernel.k -x gdbscript




# How to build the Bochs disk image ( from the MTI os lab )
#$(OBJDIR)/kern/bochs.img: $(OBJDIR)/kern/kernel $(OBJDIR)/boot/boot
#	@echo + mk $@
#	$(V)dd if=/dev/zero of=$(OBJDIR)/kern/bochs.img~ count=10000 2>/dev/null
#	$(V)dd if=$(OBJDIR)/boot/boot of=$(OBJDIR)/kern/bochs.img~ conv=notrunc 2>/dev/null
#	$(V)dd if=$(OBJDIR)/kern/kernel of=$(OBJDIR)/kern/bochs.img~ seek=1 conv=notrunc 2>/dev/null
#	$(V)mv $(OBJDIR)/kern/bochs.img~ $(OBJDIR)/kern/bochs.img
