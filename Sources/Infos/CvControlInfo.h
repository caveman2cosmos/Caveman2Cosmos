#pragma once

#ifndef CV_CONTROL_INFO_H
#define CV_CONTROL_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvControlInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvControlInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvControlInfo();
	virtual ~CvControlInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvControlInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
};

#endif // CV_CONTROL_INFO_H
