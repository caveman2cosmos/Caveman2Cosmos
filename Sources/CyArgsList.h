#pragma once

#ifndef CyArgsList_h
#define CyArgsList_h

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
	CyArgsList()
		: m_iCnt(0), m_aList() {}
	DllExport void	add(int i);
	void			add(uint ui) { add((int)ui); }
	DllExport void	add(float f);
	DllExport void	add(const char* s); // null-terminated string
	DllExport void	add(const wchar* s); // null-terminated widestring
	DllExport void	add(const char* s, int iLength); // makes a data string
	void			add(const byte* s, int iLength); // makes a list
	void			add(const int* s, int iLength); // makes a list
	void			add(const float* s, int iLength); // makes a list
	DllExport void	add(void* p);
	DllExport void* makeFunctionArgs();
	int				size() const { return m_iCnt; }
	void			push_back(void* p)
	{
		FAssertMsg(m_iCnt < MAX_CY_ARGS, "increase cyArgsList::MAX_CY_ARGS");
		m_aList[m_iCnt++] = p;
	}
	void  clear() { m_iCnt = 0; }
	void* m_aList[MAX_CY_ARGS];

  protected:
	int m_iCnt;
};

#endif // CyArgsList_h
