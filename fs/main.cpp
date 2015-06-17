#include <file_t.h>

HDD hd{}; // http://stackoverflow.com/a/23005015
file_t& ROOT =hd.blocks[0].change_type<file_t>("/",0777,file_type::D,3,false);

