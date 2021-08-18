#pragma once

#ifndef CvString_h
#define CvString_h

#include <string>
#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

//
// simple string classes, based on stl, but with a few helpers
//
// DON'T add any data members or virtual functions to these classes, so they stay the same size as their stl counterparts
//
// Mustafa Thamer
// Firaxis Games, copyright 2005
//

// wide string
class CvWString : public std::wstring
{
public:
	CvWString() {}
	~CvWString() {}

	CvWString(const CvWString& s) { *this = s; 	}
	// cppcheck-suppress noExplicitConstructor
	CvWString(const std::string& s) { Copy(s.c_str()); }
	// cppcheck-suppress noExplicitConstructor
	CvWString(const char* s) { operator=(s); }
	// cppcheck-suppress noExplicitConstructor
	CvWString(const wchar_t* s) { operator=(s); }
//	CvWString(const __wchar_t* s) { if (s) *this = s; }
	// cppcheck-suppress noExplicitConstructor
	CvWString(const std::wstring& s) { assign(s.c_str()); }

	void Copy(const char* s)
	{
		if (s == NULL || s[0] == '\0' )
		{
			clear();
			return;
		}
		const size_t len = strlen(s);
		const int size_needed = MultiByteToWideChar(CP_UTF8, 0, s, len, nullptr, 0);
		std::wstring result;
		result.resize(size_needed);
		MultiByteToWideChar(CP_UTF8, 0, s, len, &result[0], size_needed);
		assign(result);
	}

	// FString compatibility
	const wchar_t* GetCString() const 	{ return c_str(); }

	// implicit conversion
	operator const wchar_t*() const 	{ return c_str(); }

	// operators
	wchar_t& operator[](int i) { return std::wstring::operator[](i);	}
	wchar_t& operator[](std::wstring::size_type i) { return std::wstring::operator[](i);	}
	const wchar_t operator[](int i) const { return std::wstring::operator[](i);	}
	CvWString& operator=( const wchar_t* s) { if (s) assign(s); else clear();	return *this; }
	CvWString& operator=( const std::wstring& s) { assign(s.c_str());	return *this; }
	CvWString& operator=( const std::string& w) { Copy(w.c_str());	return *this; }
	CvWString& operator=( const CvWString& w) { assign(w.c_str());	return *this; }
	CvWString& operator=(const char* w) { if (w) Copy(w); else clear();	return *this; }

	void Format( LPCWSTR lpszFormat, ... );

	// static helpers
	static bool formatv(std::wstring& out, const wchar_t* fmt, va_list args);
	static bool format(std::wstring & out, const wchar_t* fmt, ...);
	static CvWString format(const wchar_t* fmt, ...);
	static std::wstring formatv(const wchar_t* fmt, va_list args);
};


//#define WIDEPTR(s) (s ? CvWString(s).c_str() : NULL)

inline CvWString operator+( const CvWString& s, const CvWString& t) { return CvWString((std::wstring&)s + (std::wstring&)t); }
inline CvWString operator+( const CvWString& s, const wchar_t* t) { return CvWString((std::wstring&)s + std::wstring(t)); }
inline CvWString operator+( const wchar_t* s, const CvWString& t) { return CvWString(std::wstring(s) + std::wstring(t)); }
//CvString operator+( const CvString& s, const CvString& t) { return (std::string&)s + (std::string&)t; }

class CvWStringBuffer
{
public:
	CvWStringBuffer()
	{
		m_pBuffer = NULL;
		m_iLength = 0;
		m_iCapacity = 0;
	}

	~CvWStringBuffer()
	{
		SAFE_DELETE_ARRAY(m_pBuffer);
	}

	void append(wchar_t character)
	{
		int newLength = m_iLength + 1;
		ensureCapacity(newLength + 1);
		m_pBuffer[m_iLength] = character;
		m_pBuffer[m_iLength + 1] = 0; //null character
		m_iLength = newLength;
	}

	void append(const wchar_t* szCharacters)
	{
		if(szCharacters == NULL)
			return;

		int inputLength = wcslen(szCharacters);
		int newLength = m_iLength + inputLength;
		ensureCapacity(newLength + 1);

		//append data
		memcpy(m_pBuffer + m_iLength, szCharacters, sizeof(wchar_t) * (inputLength + 1)); //null character
		m_iLength = newLength;
	}

	void append(const CvWString &szString)
	{
		append(szString.GetCString());
	}

	void append(const CvWStringBuffer &szStringBuffer)
	{
		append(szStringBuffer.m_pBuffer);
	}

	void assign(const CvWString &szString)
	{
		assign(szString.GetCString());
	}

	void assign(const wchar_t* szCharacters)
	{
		clear();
		append(szCharacters);
	}

	void clear()
	{
		if(m_pBuffer != NULL)
		{
			m_iLength = 0;
			m_pBuffer[0] = 0; //null character
		}
	}

	bool isEmpty() const
	{
		if(m_iLength == 0)
			return true;
		else
			return false;
	}

	const wchar_t* getCString()
	{
		ensureCapacity(1);
		return m_pBuffer;
	}

private:
	void ensureCapacity(int newCapacity)
	{
		if(newCapacity > m_iCapacity)
		{
			m_iCapacity = 2 * newCapacity; //grow by %100
			wchar_t* newBuffer = new wchar_t[m_iCapacity];

			//copy data
			if(m_pBuffer != NULL)
			{
				memcpy(newBuffer, m_pBuffer, sizeof(wchar_t) * (m_iLength + 1)); //null character
				//erase old memory
				SAFE_DELETE_ARRAY(m_pBuffer);
			}
			else
			{
				newBuffer[0] = 0; //null character
			}

			m_pBuffer = newBuffer;
		}
	}

	wchar_t* m_pBuffer;
	int m_iLength;
	int m_iCapacity;
};


class CvString : public std::string
{
public:
	CvString() {}

	explicit CvString(int iLen) { reserve(iLen); }
	// cppcheck-suppress noExplicitConstructor
	CvString(const char* s) { operator=(s); }
	// cppcheck-suppress noExplicitConstructor
	CvString(const std::string& s) { assign(s.c_str()); }
	// cppcheck-suppress noExplicitConstructor
	CvString(const std::wstring& s) { Copy(s.c_str()); }		// don't want accidental conversions down to narrow strings

	~CvString() {}

	void Convert(const std::wstring& w) { Copy(w.c_str());	}
	void Copy(const wchar_t* w)
	{
		if (w == NULL || w[0] == '\0')
		{
			clear();
			return;
		}
		const size_t len = wcslen(w);
		const int size_needed = WideCharToMultiByte(CP_UTF8, 0, w, len, nullptr, 0, nullptr, nullptr);
		std::string result;
		result.resize(size_needed);
		WideCharToMultiByte(CP_UTF8, 0, w, len, &result[0], size_needed, nullptr, nullptr);
		assign(result);
	}

	// implicit conversion
	operator const char*() const 	{ return c_str(); }
	//	operator const CvWString() const 	{ return CvWString(c_str()); }

	// operators
	char& operator[](int i) { return std::string::operator[](i);	}
	char& operator[](std::string::size_type i) { return std::string::operator[](i);	}
	const char operator[](int i) const { return std::string::operator[](i);	}
	const CvString& operator=( const char* s) { if (s) assign(s); else clear();	return *this; }
	const CvString& operator=( const std::string& s) { assign(s.c_str());	return *this; }
//	const CvString& operator=( const std::wstring& w) { Copy(w.c_str());	return *this; }		// don't want accidental conversions down to narrow strings
//	const CvString& operator=( const wchar_t* w) { Copy(w);	return *this; }

	// FString compatibility
	bool IsEmpty() const { return empty();	}
	const char* GetCString() const 	{ return c_str(); }							// convert
	int CompareNoCase( const char* lpsz ) const { return stricmp(lpsz, c_str()); }
	int CompareNoCase( const char* lpsz, int iLength ) const { return strnicmp(lpsz, c_str(), iLength);  }
	void Format( LPCSTR lpszFormat, ... );
	int GetLength() const { return size(); }
	int Replace( char chOld, char chNew );
/************************************************************************************************/
/* DEBUG_IS_MODULAR_ART                    05/12/08                                Faichele     */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int Replace( const CvString& searchString, const CvString& replaceString);
/************************************************************************************************/
/* DEBUG_IS_MODULAR_ART                    END                                                  */
/************************************************************************************************/

	void getTokens(const CvString& delimiters, std::vector<CvString>& tokensOut) const;

	// static helpers
	static bool formatv(std::string& out, const char * fmt, va_list args);
	static bool format(std::string & out, const char * fmt, ...);
	static CvString format(const char * fmt, ...);
	static std::string formatv(const char * fmt, va_list args);
};

//////////////////////////////////////////////////////////////////////////
// INLINES
// Don't move these into a cpp file, since I don't want CvString to be part of the DLL, MT
//////////////////////////////////////////////////////////////////////////

inline int CvString::Replace( char chOld, char chNew )
{
	int i, iCnt = 0;
	for(i=0;i<(int)size();i++)
	{
		if ((*this)[i] == chOld)
		{
			replace(i, 1, std::string(1, chNew) );
			iCnt++;
		}
	}
	return iCnt;
}

/************************************************************************************************/
/* DEBUG_IS_MODULAR_ART                    05/12/08                                Faichele     */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
inline int CvString::Replace(const CvString& searchString, const CvString& replaceString)
{
	int iCnt = 0;
	std::string::size_type pos = this->find(searchString, 0);
	int intLengthSearch = searchString.length();

	while(std::string::npos != pos)
	{
		this->replace(pos, intLengthSearch, replaceString);
		pos = this->find(searchString, pos + intLengthSearch);
		iCnt++;
	}

	return iCnt;
}

/************************************************************************************************/
/* DEBUG_IS_MODULAR_ART                    END                                                  */
/************************************************************************************************/
inline void CvString::getTokens(const CvString& delimiters, std::vector<CvString>& tokensOut) const
{
	//tokenizer code taken from http://www.digitalpeer.com/id/simple

	// skip delimiters at beginning.
	size_type lastPos = find_first_not_of(delimiters, 0);

	// find first "non-delimiter".
	size_type pos = find_first_of(delimiters, lastPos);

	while (CvString::npos != pos || CvString::npos != lastPos)
	{
		// found a token, parse it.
		CvString token(substr(lastPos, pos - lastPos));
		tokensOut.push_back(token);

		// skip delimiters.  Note the "not_of"
		lastPos = find_first_not_of(delimiters, pos);

		// find next "non-delimiter"
		pos = find_first_of(delimiters, lastPos);
	}
}

//
// static
//
inline bool CvString::formatv(std::string & out, const char * fmt, va_list args)
{
	char buf[2048];
	char * pbuf = buf;
	int attempts = 0;
	bool success = false;
	const int kMaxAttempts = 40;

	do
	{
		int maxlen = 2047+2048*attempts;
		int len = _vsnprintf(pbuf,maxlen,fmt,args);
		attempts++;
		success = (len>=0 && len<=maxlen);
		if (!success)
		{
			if (pbuf!=buf)
				delete [] pbuf;
			pbuf = new char[2048+2048*attempts];
		}
	}
	while (!success && attempts<kMaxAttempts);

	if ( attempts==kMaxAttempts )
	{
		// dxPrintNL( "CvString::formatv - Max reallocs occurred while formatting string. Result is likely truncated!", 0 );
	}

	if (success)
		out = pbuf;
	else
		out.clear();

	if (pbuf!=buf)
		delete [] pbuf;

	return success;
}

//
// static
//
inline bool CvWString::formatv(std::wstring& out, const wchar_t* fmt, va_list args)
{
	wchar_t buf[2048];
	wchar_t* pbuf = buf;
	int attempts = 0;
	bool success = false;
	const int kMaxAttempts = 40;

	do
	{
		int maxlen = 2047+2048*attempts;
		int len = _vsnwprintf(pbuf,maxlen,fmt,args);
		attempts++;
		success = (len>=0 && len<=maxlen);
		if (!success)
		{
			if (pbuf!=buf)
				delete [] pbuf;
			pbuf = new wchar_t[2048+2048*attempts];
		}
	}
	while (!success && attempts<kMaxAttempts);

	if ( attempts==kMaxAttempts )
	{
		// dxPrintNL( "CvString::formatv - Max reallocs occurred while formatting string. Result is likely truncated!", 0 );
	}

	if (success)
		out = pbuf;
	else
		out.clear();

	if (pbuf!=buf)
		delete [] pbuf;

	return success;
}


//
// static
//
inline std::wstring CvWString::formatv(const wchar_t* fmt, va_list args)
{
	std::wstring result;
	formatv( result, fmt, args );
	return result;
}

//
// static
//
inline CvWString CvWString::format(const wchar_t* fmt, ...)
{
	std::wstring result;
	va_list args;
	va_start(args,fmt);
	formatv(result,fmt,args);
	va_end(args);
	return CvWString(result);
}

//
// static
//
inline bool CvWString::format(std::wstring& out, const wchar_t* fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	bool r = formatv(out,fmt,args);
	va_end(args);
	return r;
}

//
//
//
inline void CvWString::Format( LPCWSTR lpszFormat, ... )
{
	std::wstring result;
	va_list args;
	va_start(args,lpszFormat);
	formatv(result,lpszFormat,args);
	va_end(args);
	*this = result;
}

//
// static
//
inline std::string CvString::formatv(const char * fmt, va_list args)
{
	std::string result;
	formatv( result, fmt, args );
	return result;
}
//
// static
//
inline CvString CvString::format(const char * fmt, ...)
{
	std::string result;
	va_list args;
	va_start(args,fmt);
	formatv(result,fmt,args);
	va_end(args);
	return CvString(result);
}

//
// static
//
inline bool CvString::format(std::string & out, const char * fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	bool r = formatv(out,fmt,args);
	va_end(args);
	return r;
}


//
//
//
inline void CvString::Format( LPCSTR lpszFormat, ... )
{
	std::string result;
	va_list args;
	va_start(args,lpszFormat);
	formatv(result,lpszFormat,args);
	va_end(args);
	*this = result;
}

#endif	// CvString_h

