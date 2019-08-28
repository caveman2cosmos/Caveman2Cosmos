#pragma once

#ifndef FASSERT_H
#define FASSERT_H

// Only compile in FAssert's if FASSERT_ENABLE is defined.  By default, however, let's key off of
// _DEBUG.  Sometimes, however, it's useful to enable asserts in release builds, and you can do that
// simply by changing the following lines to define FASSERT_ENABLE or using project settings to override
#if defined(_DEBUG) && !defined(FASSERT_ENABLE)
#define FASSERT_ENABLE
#endif

#if defined(FASSERT_ENABLE) || !defined(_DEBUG)
#ifdef WIN32
bool FAssertDlg( const char*, const char*, const char*, unsigned int, bool& );
#endif
#endif

#ifdef FASSERT_ENABLE
#ifdef WIN32

#define FAssert( expr )	\
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
{ \
	if( FAssertDlg( #expr, 0, __FILE__, __LINE__, bIgnoreAlways ) ) \
{ _asm int 3 } \
} \
}

#define FAssertMsg( expr, msg ) \
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
{ \
	if( FAssertDlg( #expr, msg, __FILE__, __LINE__, bIgnoreAlways ) ) \
{ _asm int 3 } \
} \
}

#define FErrorMsg( msg )	\
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways ) \
{ \
	if( FAssertDlg( "Error", 0, __FILE__, __LINE__, bIgnoreAlways ) ) \
{ _asm int 3 } \
} \
}

#else
// Non Win32 platforms--just use built-in FAssert
#define FAssert( expr )	FAssert( expr )
#define FAssertMsg( expr, msg )	FAssert( expr )
#define FErrorMsg( msg ) FAssert( false )

#endif

#else
// FASSERT_ENABLE not defined
#define FAssert( expr )
#define FAssertMsg( expr, msg )
#define FErrorMsg( msg )

#endif

#if defined(FASSERT_ENABLE) || !defined(_DEBUG)
#ifdef WIN32

//This AssertMsg is also shown in non debug builds
#define FAssertMsg2( expr, msg ) \
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
{ \
	if( FAssertDlg( #expr, msg, __FILE__, __LINE__, bIgnoreAlways ) ) \
{ _asm int 3 } \
} \
}
#endif
#endif

#endif // FASSERT_H
