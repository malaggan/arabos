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


## The following is the configuration for building GCC cross compiler (given that binutils are in prefix/target/bin)
# http://gcc.gnu.org/install/configure.html
# Notes: i786 is pentium 4 until core i7

## necessary on Mac
#export CC=/usr/bin/gcc-4.2
#export CXX=/usr/bin/g++-4.2
#export CPP=/usr/bin/cpp-4.2
#export LD=/usr/bin/gcc-4.2
#export PREFIX=/Users/malaggan/Desktop/arabos/cross
#export TARGET=i386-elf
## NEVER set PATH until all is compiled: #export PATH=$PREFIX/i386-elf/bin/:$PATH
## make install will install to $PREFIX/bin, $PREFIX/lib,...
#  --prefix=PREFIX         install in PREFIX
## binutils  
#  --target=$TARGET --prefix=$PREFIX --disable-nls --enable-lto  && make && make install # Installs the file to $PREFIX
##unused: --enable-gold --enable-plugins
## gcc
# ./configure --prefix=$PREFIX --target=$TARGET --disable-nls --enable-languages=c,c++ --without-headers --enable-lto && make all-gcc && make install-gcc
# for .ctors (if i don't include libgcc .ctors don't get generated): make all-target-libgcc && make install-target-libgcc
##unused: --with-stabs --disable-threads --enable-__cxa_atexit --enable-initfini-array --disable-libssp --disable-libgomp --disable-shared 
## cp cc1 and ccplus1
## GDB
# ../gdb-7.4.1/configure --prefix=$PREFIX --target=$TARGET && make all-gdb && make install-gdb
## Bochs
## using RFB (VNC viewer) solves all the problems:
#../bochs-svn/configure --prefix=$PREFIX --enable-sb16 --enable-all-optimizations --enable-cpu-level=6 --enable-gdb-stub --with-rfb --with-nogui # don't forget to export CC,CXX,CPP,LD from above, and to add -arch i386 to $(CC) line in the Makefile generated
## before finding out about RFB i used .conf.macosx, which didn' work
#  CXXFLAGS="-arch i386" CFLAGS="-arch i386" ../bochs-svn/configure --prefix=$PREFIX --with-wx --enable-all-optimizations --enable-cpu-level=6 --enable-gdb-stub --DEBUG?? --enable-plugins && make -j 8 all && make install
## I also need to edit the generated Makefile and put -arch i386 in $(CC) and $(CXX) lines, I also need to remove -lldtl and that that follows from the goal 'bochs'
## run:
# ../bochs-2.5.1/bochs 
# ../cross/bin/i386-elf-gdbtui kernel.k -x gdbscript -d /Users/malaggan/Desktop/arabos/trunk/
export PATH:=../cross/i386-elf/bin/:$(PATH)

default: all
.PHONY: all clean link install build

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

# -s for strip all sybols, -x for discard local symbols
LDFLAGS := -T$(LINKER_SCRIPT) -Map $(KERNEL_MAP)
#DBG := -gdwarf-2 -DDBG_DWARF2
DBG := -gstabs -DDBG_STABS
INCLUDE := include -I../include -I../include/c++ -I../include/c++/c++

#####!!!!!!!!!!!!!!!############d
export CC := gcc -m32
#export AS := nasm #as
export CPP := g++ -m32
export LINT := SPLINT
#export ASFLAGS := -felf
export FIND := find

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

#WARN: do not use inline impl of classes in header files as it creates sections that breaks the stabs lookup
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

all: subdirs link install #runBochs

link: $(SUBDIRS)
	@echo linking
	ld -melf_i386 obj/*.o -o $(KERN_BIN) $(LDFLAGS)
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
	gdbtui kernel.k -x gdbscript

# How to build the Bochs disk image ( from the MTI os lab )
#$(OBJDIR)/kern/bochs.img: $(OBJDIR)/kern/kernel $(OBJDIR)/boot/boot
#	@echo + mk $@
#	$(V)dd if=/dev/zero of=$(OBJDIR)/kern/bochs.img~ count=10000 2>/dev/null
#	$(V)dd if=$(OBJDIR)/boot/boot of=$(OBJDIR)/kern/bochs.img~ conv=notrunc 2>/dev/null
#	$(V)dd if=$(OBJDIR)/kern/kernel of=$(OBJDIR)/kern/bochs.img~ seek=1 conv=notrunc 2>/dev/null
#	$(V)mv $(OBJDIR)/kern/bochs.img~ $(OBJDIR)/kern/bochs.img
