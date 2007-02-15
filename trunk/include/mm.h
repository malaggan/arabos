#ifndef MM_H
#define MM_H

// page table entry format :
// addr/avail to os/zero/zero/dirty/accessed/page cache disable/pg write throuhg(1) back(0)/user,supervisor/write,read/present

// page  dir entry
// addr/avail to os/zero/page size/zero/accessed/page cache disable/pg write throuhg(1) back(0)/user,supervisor/write,read/present

typedef long page_table_entry; // (physical address with flags)	 // can it be a bit-fielded struct ?
typedef page_table_entry* /*[1024]*/ page_table_t;
typedef page_table_t* /*[1024]*/ page_directory_t;

extern page_directory_t kernel_page_dir;

void init_paging();
inline void enable_paging();

#endif // MM_H 
