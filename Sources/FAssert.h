#pragma once

#ifndef FASSERT_H
#define FASSERT_H

// Macro helpers
#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

// Only compile in FAssert's if FASSERT_ENABLE is defined.  By default, however, let's key off of
// _DEBUG.  Sometimes, however, it's useful to enable asserts in release builds, and you can do that
// simply by changing the following lines to define FASSERT_ENABLE or using project settings to override
#if (defined(_DEBUG) || defined(__COVERITY__)) && !defined(FASSERT_ENABLE)
#define FASSERT_ENABLE
#endif

#if defined(FASSERT_ENABLE) || !defined(_DEBUG)
#ifdef WIN32
bool FAssertDlg( const char*, const char*, const char*, unsigned int, const char*, bool& );
#endif
#endif

#ifdef FASSERT_ENABLE
#if defined(__COVERITY__)
//#define FAssert( expr )	if( !(expr) ) throw std::exception(#expr);
//#define FAssertMsg( expr, msg )	FAssert( expr )

#define FAssertRecalcMsg( expr, msg ) FAssert( expr )
#define FAssertOptionMsg( option, expr, msg ) FAssert( GC.getGame().isOption(option) && expr )
#define FAssertOptionRecalcMsg( option, expr, msg) FAssert( GC.getGame().isOption(option) && expr )
#define FErrorMsg( msg ) FAssert( false )
#define FEnsure( expr ) { if( !(expr) ) throw std::exception(#expr); }
#define FEnsureMsg( expr, msg ) { if( !(expr) ) throw std::exception(#expr); }
#elif defined(WIN32)
//Calvitix - temporary remove assertions (set bIgnoreAlways to true)
#define FAssert( expr )	\
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
	{ \
		if( FAssertDlg( #expr, 0, __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) { _asm int 3 } \
	} \
}

#define FAssertMsg( expr, msg ) \
{ \
	static bool bIgnoreAlways = true; \
	if( !bIgnoreAlways && !(expr) ) \
	{ \
		if( FAssertDlg( #expr, msg, __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) { _asm int 3 } \
	} \
}

#define FAssertRecalcMsg( expr, msg ) \
{ \
	static bool bIgnoreAlways = true; \
	if( !bIgnoreAlways && !(expr) ) \
	{ \
		if( FAssertDlg( #expr, CvString::format("%s\r\n\r\nPlease recalculate modifiers!", msg).c_str(), __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) { _asm int 3 } \
	} \
}

#define FAssertOptionMsg( option, expr, msg ) \
{ \
	static bool bIgnoreAlways = true; \
	if( !bIgnoreAlways && GC.getGame().isOption(option) && !(expr) ) \
	{ \
		if( FAssertDlg( #expr, CvString::format("Option: %s\r\n%s", #option, msg).c_str(), __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) { _asm int 3 } \
	} \
}

#define FAssertOptionRecalcMsg( option, expr, msg) \
{ \
	static bool bIgnoreAlways = true; \
	if( !bIgnoreAlways && GC.getGame().isOption(option) && !(expr) ) \
	{ \
		if( FAssertDlg( #expr, CvString::format("Option: %s\r\n%s\r\n\r\nPlease recalculate modifiers!",  #option, msg).c_str(), __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) { _asm int 3 } \
	} \
}

#define FErrorMsg( msg )	\
{ \
	static bool bIgnoreAlways = true; \
	if( !bIgnoreAlways ) \
	{ \
		if( FAssertDlg( "ERROR", msg, __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) { _asm int 3 } \
	} \
}


#define FEnsure( expr )	\
{ \
	static bool bIgnoreAlways = true; \
	if( !(expr) ) \
	{ \
		if( FAssertDlg( #expr, 0, __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) { _asm int 3 } \
		throw std::exception(#expr); \
	} \
}

#define FEnsureMsg( expr, msg ) \
{ \
	static bool bIgnoreAlways = true; \
	if( !(expr) ) \
	{ \
		if( FAssertDlg( #expr, msg, __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) { _asm int 3 } \
		throw std::exception(#expr); \
	} \
}

#else
// Non Win32 platforms--just use built-in FAssert
#define FAssert( expr )	FAssert( expr )
#define FAssertMsg( expr, msg )	FAssert( expr )
#define FAssertRecalcMsg( expr, msg ) FAssert( expr )
#define FAssertOptionMsg( option, expr, msg ) FAssert( GC.getGame().isOption(option) && expr )
#define FAssertOptionRecalcMsg( option, expr, msg) FAssert( GC.getGame().isOption(option) && expr )
#define FErrorMsg( msg ) FAssertMsg( false, msg )
#define FEnsure( expr ) { if( !(expr) ) throw std::exception(#expr); }
#define FEnsureMsg( expr, msg ) { if( !(expr) ) throw std::exception(#expr); }

#endif

// An instance of this class will mark a scope so that other functions can check that they are or are not called from within it.
template < int ID >
struct AssertScope
{
	AssertScope() { ++m_scopedepth; }
	~AssertScope()
	{
		--m_scopedepth;
		FAssertMsg(m_scopedepth >= 0, "Mismatched scopes");
	}

	static int m_scopedepth;
};

template < int ID >
int AssertScope<ID>::m_scopedepth = 0;

// Scores for use with AssertScope
enum AssertScopeTypes
{
	CvSelectionGroup_CvUnit_LOOP,
	CvPlot_CvUnit_LOOP,
	CvSelectionGroup_LOOP
};

#define FAssertDeclareScope(_id_) AssertScope<_id_> CONCATENATE(__AssertScope_##_id_##_, __LINE__)
#define FAssertInScope(_id_) FAssertMsg(AssertScope<_id_>::m_scopedepth != 0, "Expected to be in a " #_id_ " scope!")
#define FAssertNotInScope(_id_) FAssertMsg(AssertScope<_id_>::m_scopedepth == 0, "Expected to not be in a " #_id_ " scope!")

#define FASSERT_BOUNDS(lower, upper, index) \
	FAssertMsg(index >= lower, (bst::format("Index value (%d) is expected to be >= %d") % index % lower).str().c_str()); \
	FAssertMsg(index < upper, (bst::format("Index value (%d) is expected to be < %d") % index % upper).str().c_str());

#define FASSERT_NOT_NEGATIVE(value) \
	FAssertMsg(value >= 0, (bst::format("Value (%d) is expected to be >= 0") % value).str().c_str())

#else
// FASSERT_ENABLE not defined
#define FAssert( expr )
#define FAssertMsg( expr, msg )
#define FAssertRecalcMsg( expr, msg )
#define FAssertOptionMsg( option, expr, msg )
#define FAssertOptionRecalcMsg( option, expr, msg)
#define FErrorMsg( msg )
#define FEnsure( expr )
#define FEnsureMsg( expr, msg )

#define FAssertDeclareScope(_id_)
#define FAssertInScope(_id_)
#define FAssertNotInScope(_id_)

#define FASSERT_BOUNDS(lower, upper, index)
#define FASSERT_NOT_NEGATIVE(value)

#endif

#if defined(FASSERT_ENABLE) || !defined(_DEBUG)
#ifdef WIN32

// This AssertMsg is also shown in non debug builds
#define FAssertMsg2( expr, msg ) \
{ \
	static bool bIgnoreAlways = false; \
	if( !bIgnoreAlways && !(expr) ) \
{ \
	if( FAssertDlg( #expr, msg, __FILE__, __LINE__, __FUNCTION__, bIgnoreAlways ) ) \
{ _asm int 3 } \
} \
}
#endif
#endif

// Static assert implementation for C++03, from https://stackoverflow.com/a/1980156/6402065
/**
 * Usage:
 *
 * <code>STATIC_ASSERT(expression, message)</code>
 *
 * When the static assertion test fails, a compiler error message that somehow
 * contains the "STATIC_ASSERTION_FAILED_AT_LINE_xxx_message" is generated.
 *
 * /!\ message has to be a valid C++ identifier, that is to say it must not
 * contain space characters, cannot start with a digit, etc.
 *
 * STATIC_ASSERT(true, this_message_will_never_be_displayed);
 */

#define STATIC_ASSERT(expression, message)\
  struct CONCATENATE(__static_assertion_at_line_, __LINE__)\
  {\
	implementation::StaticAssertion<static_cast<bool>((expression))> CONCATENATE(CONCATENATE(CONCATENATE(STATIC_ASSERTION_FAILED_AT_LINE_, __LINE__), _), message);\
  };\
  typedef implementation::StaticAssertionTest<sizeof(CONCATENATE(__static_assertion_at_line_, __LINE__))> CONCATENATE(__static_assertion_test_at_line_, __LINE__)

 // note that we wrap the non existing type inside a struct to avoid warning
 // messages about unused variables when static assertions are used at function
 // scope
 // the use of sizeof makes sure the assertion error is not ignored by SFINAE

namespace implementation {

	template <bool>
	struct StaticAssertion;

	template <>
	struct StaticAssertion<true>
	{
	}; // StaticAssertion<true>

	template<int i>
	struct StaticAssertionTest
	{
	}; // StaticAssertionTest<int>

} // namespace implementation

#endif // FASSERT_H
