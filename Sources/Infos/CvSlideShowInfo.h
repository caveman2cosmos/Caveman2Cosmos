#pragma once

#ifndef CV_SLIDE_SHOW_INFO_H
#define CV_SLIDE_SHOW_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSlideShowInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSlideShowInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSlideShowInfo();
	virtual ~CvSlideShowInfo();

	DllExport const char* getPath()				{ return m_szPath; }
	DllExport const char* getTransitionType()	{ return m_szTransitionType; }
	DllExport float getStartTime()				{ return m_fStartTime; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvSlideShowInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	float m_fStartTime;
	CvString m_szPath;
	CvString m_szTransitionType;
};

#endif // CV_SLIDE_SHOW_INFO_H
