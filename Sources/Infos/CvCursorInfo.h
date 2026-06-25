#pragma once

#ifndef CV_CURSOR_INFO_H
#define CV_CURSOR_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCursorInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCursorInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCursorInfo();
	virtual ~CvCursorInfo();

	DllExport const char* getPath();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvCursorInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szPath;
};

#endif // CV_CURSOR_INFO_H
