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

typedef unsigned long phys_addr_t;

void printk (const char *, ...);

#ifdef __cplusplus
} extern "C++" {
inline int SET_BIT(const int x, const int b) {return (x|(1<<b)); }
inline int CLR_BIT(const int x, const int b) {return (x&~(1<<b)); }
inline int GET_BIT(const int x, const int b) {return (x&(1<<b))>0?1:0; }
inline int SET_BIT(const int x, const int b, const int v) {return (v?SET_BIT(x,b):CLR_BIT(x,b)); }

enum CacheMode {WriteBack = 0, WriteThrough};
enum Privilage {Supervisor = 0, User};
enum Access {ReadOnly=0, ReadWrite};
enum PageSize {_4K=0, _4M};

class PageTableEntry
{
    unsigned long value;
public:
    PageTableEntry():value(0){}
    PageTableEntry(unsigned long aValue):value(aValue){}

    unsigned long getValue() const {return value;}

    phys_addr_t getPageBase() const { return value & ((~0)<<12); };
    void setPageBase(phys_addr_t pageBase) { value = (value & ~((~0)<<12)) | (pageBase & ((~0)<<12)); };

    unsigned int getExtra() const { return (value >> 9) & 0x7;};
    void setExtra(unsigned int extra) { value = (value & ((~0)<<9)) | (extra & ((~0)>>3)); };

    bool getGlobal() const { return GET_BIT(value,8); };
    void setGlobal(bool global) { value=SET_BIT(value,8,global); };

    bool getPat() const { return GET_BIT(value,7); };
    void setPat(bool pat) { value=SET_BIT(value,7,pat); };

    unsigned int getDirty() const { return GET_BIT(value,6); };
    unsigned int getAccessed() const { return GET_BIT(value,5); };

    bool getCahceEnabled() const { return !GET_BIT(value,4); };
    void setCahceEnabled(bool enabled) { value=SET_BIT(value,4,!enabled); };

    CacheMode getCacheMode() const { return (CacheMode)GET_BIT(value,3); };
    void setCacheMode(CacheMode mode) { value=SET_BIT(value,3,mode); };

    Privilage getPrivilage() const { return (Privilage)GET_BIT(value,2); };
    void setPrivilage(Privilage mode) { value=SET_BIT(value,2,mode); };

    Access getAccess() const { return (Access)GET_BIT(value,1); };
    void setAccess(Access mode) { value=SET_BIT(value,1,mode); };

    bool getPresent() const { return GET_BIT(value,0); };
    void setPresent(bool present) { if(!present) value=0; value=SET_BIT(value,0,present); };

    void print() const
    {
        printk("PTE PB:0x%x, EX:0x%x, G:%d, PAT:%d, D:%d, A:%d, PCD:%d, PWT:%d, US:%d, RW:%d, P:%d\n",getPageBase(),getExtra(),getGlobal(),
                getPat(), getDirty(), getAccessed(), !getCahceEnabled(), getCacheMode(), getPrivilage(), getAccess(),getPresent() );
    }
};

class PageDirectoryEntry
{
    unsigned long value;
public:
    PageDirectoryEntry():value(0){}
    PageDirectoryEntry(unsigned long aValue):value(aValue){}

    unsigned long getValue() const {return value;}

    phys_addr_t getPageTableBase() const { return value & ((~0)<<12); };
    void setPageTableBase(phys_addr_t pageTableBase) { value = (value & ~((~0)<<12)) | (pageTableBase & ((~0)<<12)); };

    unsigned int getExtra() const { return (value >> 9) & 0x7;};
    void setExtra(unsigned int extra) { value = (value & ((~0)<<9)) | (extra & ((~0)>>3)); };

    bool getGlobal() const { return GET_BIT(value,8); }; // ignored
    void setGlobal(bool global) { value=SET_BIT(value,8,global); };

    PageSize getPageSize() const { return (PageSize)GET_BIT(value,7); };
    void setPageSize(PageSize pageSize) { value=SET_BIT(value,7,pageSize); };

    //bit 6 reserved
    
    unsigned int getAccessed() const { return GET_BIT(value,5); };

    bool getCahceEnabled() const { return !GET_BIT(value,4); };
    void setCahceEnabled(bool enabled) { value=SET_BIT(value,4,!enabled); };

    CacheMode getCacheMode() const { return (CacheMode)GET_BIT(value,3); };
    void setCacheMode(CacheMode mode) { value=SET_BIT(value,3,mode); };

    Privilage getPrivilage() const { return (Privilage)GET_BIT(value,2); };
    void setPrivilage(Privilage mode) { value=SET_BIT(value,2,mode); };

    Access getAccess() const { return (Access)GET_BIT(value,1); };
    void setAccess(Access mode) { value=SET_BIT(value,1,mode); };

    bool getPresent() const { return GET_BIT(value,0); };
    void setPresent(bool present) { if(!present) value=0; value=SET_BIT(value,0,present); };

    void print() const
    {
        printk("PTE PB:0x%x, EX:0x%x, G:%d, PS:%d, A:%d, PCD:%d, PWT:%d, US:%d, RW:%d, P:%d\n",getPageTableBase(),getExtra(),getGlobal(),
                getPageSize(), getAccessed(), !getCahceEnabled(), getCacheMode(), getPrivilage(), getAccess(),getPresent() );
    }
};
} extern "C" {
#endif // __cplusplus

void init_paging();

#ifdef __cplusplus
}
#endif

#endif // MM_H 
