#pragma once

#ifndef CyArgsList_h
#define CyArgsList_h

template < class Ty_ >
class CyArrayArg
{
	typedef Ty_ value_type;
	
	CyArrayArg(const value_type* vals, int len) : vals(vals), len(len) {}

	const value_type* vals;
	int len;
};
//
// type for input args to python functions
//
class CyArgsList
{
public:
	CyArgsList() : m_nArgs(0) {}

	// Exe/Dll interface (DO NOT CHANGE)
	DllExport void add(int i);
	DllExport void add(float f);
	DllExport void add(const char* s);					// null-terminated string
	DllExport void add(const wchar* s);					// null-terminated widestring
	DllExport void add(const char* s, int iLength);		// makes a data string
	DllExport void add(void* p);
	DllExport void* makeFunctionArgs();

	// Extended interface
	void add(uint ui) { add((int)ui); }
	void add(const byte* s, int iLength);		// makes a list
	void add(const int* s, int iLength);		// makes a list
	void add(const float* s, int iLength);		// makes a list
	template < class Ty_ >
	void add(const CyArrayArg<Ty_>& arr) { add(arr.vals, arr.len); }

	int size() const { return m_nArgs; }
	void clear() { m_nArgs = 0; }

	template < class Ty_ >
	CyArgsList& operator<<(const Ty_& arg) { add(arg); return *this; }

private:
	static const int MAX_CY_ARGS = 20;

	void push_back(void* p) 
	{
		FAssertMsg(m_nArgs + 1 < MAX_CY_ARGS, "Exceeded maximum allowed number of Python arguments for a function");
		m_aList[m_nArgs++] = p;
	}

	void* m_aList[MAX_CY_ARGS];
	int m_nArgs;
};

#endif	// CyArgsList_h
