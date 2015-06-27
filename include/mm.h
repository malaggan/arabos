/* Copyright (C) 2007  Mohammad Nabil
   mohammad (dot) nabil (dot) h (at) gmail (dot) com

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/

#ifndef MM_H
#define MM_H

#include <types.h>
#include <liballoc.h>

#ifdef __cplusplus
extern "C" {
#endif

// page table entry format :
// addr/avail to os/zero/zero/dirty/accessed/page cache disable/pg write throuhg(1) back(0)/user,supervisor/write,read/present

// page  dir entry
// addr/avail to os/zero/page size/zero/accessed/page cache disable/pg write throuhg(1) back(0)/user,supervisor/write,read/present

	typedef union
	{
		unsigned long value;
		struct {
			unsigned ixPageTable:10;
			unsigned ixPage:10;
			unsigned offset:12;
		} data ;
	} __attribute__((packed)) linear_addr_t;

	void printk (const char *, ...);

#ifdef __cplusplus
} extern "C++" {
#include <stl/new.hh>

	  enum CacheMode {WriteBack = 0, WriteThrough};
	  enum Privilage {Supervisor = 0, User};
	  enum Access {ReadOnly=0, ReadWrite};
	  enum PageSize {_4K=0, _4M};

	  class PageTableEntry
	  {
		  unsigned long value;
	  public:
		  PageTableEntry();
		  PageTableEntry(unsigned long aValue);

		  unsigned long getValue() const;

		  phys_addr_t getPageBase() const;
		  void setPageBase(phys_addr_t pageBase);

		  unsigned int getExtra() const;
		  void setExtra(unsigned int extra);

		  bool getGlobal() const;
		  void setGlobal(bool global);

		  bool getPat() const;
		  void setPat(bool pat);

		  unsigned int getDirty() const;
		  unsigned int getAccessed() const;

		  bool getCahceEnabled() const;
		  void setCahceEnabled(bool enabled);

		  CacheMode getCacheMode() const;
		  void setCacheMode(CacheMode mode);

		  Privilage getPrivilage() const;
		  void setPrivilage(Privilage mode);

		  Access getAccess() const;
		  void setAccess(Access mode);

		  bool getPresent() const;
		  void setPresent(bool present);

		  void print() const;
	  };

	  class PageDirectoryEntry
	  {
		  unsigned long value;
	  public:
		  PageDirectoryEntry();
		  PageDirectoryEntry(unsigned long aValue);

		  unsigned long getValue() const;

		  phys_addr_t getPageTableBase() const;
		  void setPageTableBase(phys_addr_t pageTableBase);

		  unsigned int getExtra() const;
		  void setExtra(unsigned int extra);

		  bool getGlobal() const;
		  void setGlobal(bool global);

		  PageSize getPageSize() const;
		  void setPageSize(PageSize pageSize);

		  //bit 6 reserved

		  unsigned int getAccessed() const;

		  bool getCahceEnabled() const;
		  void setCahceEnabled(bool enabled);

		  CacheMode getCacheMode() const;
		  void setCacheMode(CacheMode mode);

		  Privilage getPrivilage() const;
		  void setPrivilage(Privilage mode);

		  Access getAccess() const;
		  void setAccess(Access mode);

		  bool getPresent() const;
		  void setPresent(bool present);

		  void print() const;
	  };
} extern "C" {
#endif // __cplusplus

	  void init_paging();

#ifdef __cplusplus
}
#endif

#endif // MM_H
