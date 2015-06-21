/* -*- mode: c++; -*- */
#ifndef TYPES_H 
#define TYPES_H 

using int8_t = __INT8_TYPE__;
using int16_t = __INT16_TYPE__;
using int32_t = __INT32_TYPE__;
using int64_t = __INT64_TYPE__;

using int_fast8_t = __INT_FAST8_TYPE__;
using int_fast16_t = __INT_FAST16_TYPE__;
using int_fast32_t = __INT_FAST32_TYPE__;
using int_fast64_t = __INT_FAST64_TYPE__;

using int_least8_t = __INT_LEAST8_TYPE__;
using int_least16_t = __INT_LEAST16_TYPE__;
using int_least32_t = __INT_LEAST32_TYPE__;
using int_least64_t = __INT_LEAST64_TYPE__;

using intmax_t = __INTMAX_TYPE__;
using intptr_t = __INTPTR_TYPE__;

using uint8_t = __UINT8_TYPE__;
using uint16_t = __UINT16_TYPE__;
using uint32_t = __UINT32_TYPE__;
using uint64_t = __UINT64_TYPE__;

using uint_fast8_t = __UINT_FAST8_TYPE__;
using uint_fast16_t = __UINT_FAST16_TYPE__;
using uint_fast32_t = __UINT_FAST32_TYPE__;
using uint_fast64_t = __UINT_FAST64_TYPE__;

using uint_least8_t = __UINT_LEAST8_TYPE__;
using uint_least16_t = __UINT_LEAST16_TYPE__;
using uint_least32_t = __UINT_LEAST32_TYPE__;
using uint_least64_t = __UINT_LEAST64_TYPE__;

using uintmax_t = __UINTMAX_TYPE__;
using uintptr_t = __UINTPTR_TYPE__;

using size_t = __SIZE_TYPE__;
using ptrdiff_t = __PTRDIFF_TYPE__;

using phys_addr_t = uint32_t;

using uid_t = uint32_t;
using gid_t = uint32_t;
using mode_t = uint32_t;
using __time_t =int64_t;

#ifdef __cplusplus
namespace aos {  
    template<class T, T v>
    struct integral_constant {
	static constexpr T value = v;
	using value_type = T;
	using type = integral_constant;
	constexpr operator value_type() const noexcept { return value; }
	constexpr value_type operator()() const noexcept { return value; } 
    };
    typedef integral_constant<bool,true> true_type;
    typedef integral_constant<bool,false> false_type;
    template<bool B, class T = void> struct enable_if {}; 
    template<class T> struct enable_if<true, T> { using type = T; };
    template<bool B, class T = void> using enable_if_t = typename enable_if<B,T>::type;
    template<class T, class U> struct is_same : false_type {}; 
    template<class T>          struct is_same<T, T> : true_type {};

    template<class T> struct remove_reference      {using type = T;};
    template<class T> struct remove_reference<T&>  {using type = T;};
    template<class T> struct remove_reference<T&&> {using type = T;};
    template<class T> using remove_reference_t = typename remove_reference<T>::type;
    
    template<typename T> constexpr T&& forward(remove_reference_t<T>& t) noexcept { return static_cast<T&&>(t); }    
    template<typename T> constexpr T&& forward(remove_reference_t<T>&& t) noexcept { return static_cast<T&&>(t); }


    template<typename T> constexpr remove_reference_t<T>&& move(T&& t) noexcept {return static_cast<remove_reference_t<T>&&>(t);}

}
#endif

#ifndef __cplusplus
#define NULL (0) // use nullptr in C++
#endif

#endif /* TYPES_H */
