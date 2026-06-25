#pragma once

#ifndef CV_SLIDE_SHOW_RANDOM_INFO_H
#define CV_SLIDE_SHOW_RANDOM_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSlideShowRandomInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSlideShowRandomInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSlideShowRandomInfo();
	virtual ~CvSlideShowRandomInfo();

	DllExport const char* getPath();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvSlideShowRandomInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szPath;
};

#endif // CV_SLIDE_SHOW_RANDOM_INFO_H
