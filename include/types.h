/* -*- mode: c++; -*- */
#ifndef TYPES_H 
#define TYPES_H 

typedef unsigned char uint8_t;
typedef          char int8_t;

typedef unsigned short uint16_t;
typedef          short int16_t;

typedef unsigned long uint32_t;
typedef          long  int32_t;

typedef unsigned long long uint64_t;
typedef          long long int64_t;

typedef __SIZE_TYPE__ size_t;

typedef uint32_t* uintptr_t;
typedef uint32_t phys_addr_t;

typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t mode_t;
typedef int64_t  __time_t;

#ifdef __cplusplus
namespace aos {  
    template<class T, T v>
    struct integral_constant {
	static constexpr T value = v;
	typedef T value_type;
	typedef integral_constant type;
	constexpr operator value_type() const noexcept { return value; }
	constexpr value_type operator()() const noexcept { return value; } 
    };
    typedef integral_constant<bool,true> true_type;
    typedef integral_constant<bool,false> false_type;
    template<bool B, class T = void>
    struct enable_if {};
 
    template<class T>
    struct enable_if<true, T> { typedef T type; };
    template< bool B, class T = void >
    using enable_if_t = typename enable_if<B,T>::type;
    template<class T, class U>
    struct is_same : false_type {}; 
    template<class T>
    struct is_same<T, T> : true_type {};

    template< class T > struct remove_reference      {typedef T type;};
    template< class T > struct remove_reference<T&>  {typedef T type;};
    template< class T > struct remove_reference<T&&> {typedef T type;};
    template< class T > using remove_reference_t = typename remove_reference<T>::type;
    
    template<typename _Tp> constexpr _Tp&&
    forward(remove_reference_t<_Tp>& __t) noexcept
    { return static_cast<_Tp&&>(__t); }
    
    template<typename _Tp> constexpr _Tp&&
    forward(remove_reference_t<_Tp>&& __t) noexcept
    { return static_cast<_Tp&&>(__t); }
    
}
#endif

#ifndef __cplusplus
#define NULL (0) // use nullptr in C++
#endif

#endif /* TYPES_H */
