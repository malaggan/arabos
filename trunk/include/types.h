#ifndef TYPES_H 
#define TYPES_H 

typedef unsigned char uint8_t;
typedef          char int8_t;

typedef unsigned short uint16_t;
typedef          short int16_t;

typedef unsigned long uint32_t;
typedef          long  int32_t;

#ifndef __cplusplus
typedef unsigned long long uint64_t;
typedef          long long int64_t;
#endif

typedef uint32_t size_t;

typedef uint32_t* uintptr_t;
typedef uint32_t phys_addr_t;

#define NULL (0)

#endif /* TYPES_H */
