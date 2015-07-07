#include <file_t.h>

aos::static_vector<aos::string<20>,10> sfs_readdir (const char *path);

int sfs_ls(char const  *path)
{
	printf(">>>>>>> in the function ls\n");

	aos::static_vector<aos::string<20>,10> des = sfs_readdir(path);
	for(int i=0;i<des.size();i++)
		printf(">>>>>>> ls:%s\n",des[i].c_str());

	return 0;

}
