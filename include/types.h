/* -*- mode: c++; -*- */
#ifndef TYPES_H 
#define TYPES_H 

typedef __INT8_TYPE__ int8_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT64_TYPE__ int64_t;

typedef __INT_FAST8_TYPE__ int_fast8_t;
typedef __INT_FAST16_TYPE__ int_fast16_t;
typedef __INT_FAST32_TYPE__ int_fast32_t;
typedef __INT_FAST64_TYPE__ int_fast64_t;

typedef __INT_LEAST8_TYPE__ int_least8_t;
typedef __INT_LEAST16_TYPE__ int_least16_t;
typedef __INT_LEAST32_TYPE__ int_least32_t;
typedef __INT_LEAST64_TYPE__ int_least64_t;

typedef __INTMAX_TYPE__ intmax_t;
typedef __INTPTR_TYPE__ intptr_t;

typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;

typedef __UINT_FAST8_TYPE__ uint_fast8_t;
typedef __UINT_FAST16_TYPE__ uint_fast16_t;
typedef __UINT_FAST32_TYPE__ uint_fast32_t;
typedef __UINT_FAST64_TYPE__ uint_fast64_t;

typedef __UINT_LEAST8_TYPE__ uint_least8_t;
typedef __UINT_LEAST16_TYPE__ uint_least16_t;
typedef __UINT_LEAST32_TYPE__ uint_least32_t;
typedef __UINT_LEAST64_TYPE__ uint_least64_t;

typedef __UINTMAX_TYPE__ uintmax_t;
typedef __UINTPTR_TYPE__ uintptr_t;

typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;

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

    template< class T > struct remove_pointer      {typedef T type;};
    template< class T > struct remove_pointer<T*>  {typedef T type;};
    template< class T > using remove_pointer_t = typename remove_pointer<T>::type;
    
    template<typename T> constexpr T&&
    forward(remove_reference_t<T>& t) noexcept
    { return static_cast<T&&>(t); }
    
    template<typename T> constexpr T&&
    forward(remove_reference_t<T>&& t) noexcept
    { return static_cast<T&&>(t); }

    template<typename T>
    constexpr remove_reference_t<T>&&
    move(T&& t) noexcept
    { return static_cast<remove_reference_t<T>&&>(t); }
    
}
#endif

#ifndef __cplusplus
#define NULL (0) // use nullptr in C++
#endif

#endif /* TYPES_H */
