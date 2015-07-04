#pragma once

#include <printk.h>

#define assert(cond) if(!(cond)){printk(ERROR "assertion failure %s:%d: \"" #cond "\"",__FILE__,__LINE__);}
