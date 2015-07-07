#include <file_t.h>
#include <ata.h>
/*
  uint16_t *r=reinterpret_cast<uint16_t*>(&(hd.blocks[0].change_type<file_t>("/",0777,file_type::D,3,false)));
  write(r,0);
  file_t* ROOT =reinterpret_cast<file_t*>(r);*/
HDD hd{}; // http://stackoverflow.com/a/23005015
block_t ROOT; //=(new file_t("/",0777,file_type::D,1,false));
//uint16_t *r=reinterpret_cast<uint16_t*>(ROOT);

void main()
{/*
   printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in main fs\n ");
   uint16_t data[256];
   read(data,0);
   file_t* p =reinterpret_cast<file_t*>(data);
   aos::string<20> st="/";
   if(p->name== st){
   //*p= (uint16_t)malloc(sizeof(block_t);
   *ROOT=*p;
   printf(DEBUG"ROOT hard INIT name:%s",ROOT.file.name.c_str());
   }else{
   printf(DEBUG"ROOT new INIT name:%s",ROOT.file.name.c_str());
   ROOT.file.name="/";
   }*/



}
