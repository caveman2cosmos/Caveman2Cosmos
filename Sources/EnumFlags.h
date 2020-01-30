#pragma once

#ifndef EnumFlags_h__
#define EnumFlags_h__

// Taken from newer version of winnt.h


// Define operator overloads to enable bit operations on enum values that are
// used to define flags. Use DECLARE_FLAGS(YOUR_TYPE) to enable these
// operators on YOUR_TYPE.

// Moved here from objbase.w.

// Templates are defined here in order to avoid a dependency on C++ <type_traits> header file,
// or on compiler-specific constructs.
extern "C++" {
	template <size_t S>
	struct _ENUM_FLAG_INTEGER_FOR_SIZE;

	template <>
	struct _ENUM_FLAG_INTEGER_FOR_SIZE<1>
	{
		typedef INT8 type;
	};

	template <>
	struct _ENUM_FLAG_INTEGER_FOR_SIZE<2>
	{
		typedef INT16 type;
	};

	template <>
	struct _ENUM_FLAG_INTEGER_FOR_SIZE<4>
	{
		typedef INT32 type;
	};

	template <>
	struct _ENUM_FLAG_INTEGER_FOR_SIZE<8>
	{
		typedef INT64 type;
	};

	// used as an approximation of std::underlying_type<T>
	template <class T>
	struct _ENUM_FLAG_SIZED_INTEGER
	{
		typedef typename _ENUM_FLAG_INTEGER_FOR_SIZE<sizeof(T)>::type type;
	};
}

#define DECLARE_FLAGS(ENUMTYPE) \
extern "C++" { \
inline ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) throw() { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) | ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) throw() { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) |= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) throw() { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) & ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) throw() { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) &= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE operator ~ (ENUMTYPE a) throw() { return ENUMTYPE(~((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)); } \
inline ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) throw() { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) ^ ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) throw() { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) ^= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
}

#endif // EnumFlags_h__
