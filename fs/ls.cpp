#include <file_t.h>


void sfs_ls(char const  *path)
{
  aos::vector<aos::string> des=sfs_readdir(path);
  for(int i=0;i<des.size();i++)
  	printf("%s\n",des[i]);

  
}

