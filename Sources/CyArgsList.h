#pragma once
#ifndef CyArgsList_h__
#define CyArgsList_h__

#include "CvGameCoreDLL.h"

//
// type for input args to python functions
//
class CyArgsList
{
public:
	enum
	{
		MAX_CY_ARGS = 20
	};
	CyArgsList() : m_iCnt(0), m_aList() {}
	DllExport void add(int i);
	void add(uint ui) { add((int)ui); }
	DllExport void add(float f);
	DllExport void add(const char* s);					// null-terminated string
	DllExport void add(const wchar_t* s);				// null-terminated widestring
	DllExport void add(const char* s, int iLength);		// makes a data string
	void add(const uint8_t* s, int iLength);		// makes a list
	void add(const int* s, int iLength);		// makes a list
	void add(const float* s, int iLength);		// makes a list

	template < class Ty >
	void add(const std::vector<Ty>& s)
	{
		if(!s.empty())
		{
			add(&s[0], s.size());
		}
		else
		{
			Ty s0[1];
			add(s0, 0);
		}
	}

	DllExport void add(void* p);
	DllExport void* makeFunctionArgs();

	// For PyObject pointers
	void add(PyObject* p) { add((void*)p); }
	// For general pointers not covered above
	template < class PtrTy_ >
	void add(const PtrTy_* ptr) { add(gDLL->getPythonIFace()->makePythonObject(ptr)); }

	int size() const { return m_iCnt; }
	void push_back(void* p) { FAssertMsg(m_iCnt < MAX_CY_ARGS, "increase cyArgsList::MAX_CY_ARGS"); m_aList[m_iCnt++] = p; }
	void clear() { m_iCnt = 0; }
	void* m_aList[MAX_CY_ARGS];
protected:
	int m_iCnt;
};
#endif // CyArgsList_h__
