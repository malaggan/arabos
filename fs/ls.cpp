#include <file_t.h>


void sfs_ls(char const  *path)
{
	auto des=sfs_readdir(path);
	for(int i=0;i<des.size();i++)
		printf("%s\n",des[i]);


}
