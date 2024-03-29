#pragma once

#include <types.h>

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
