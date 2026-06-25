#pragma once

#ifndef CV_COLOR_INFO_H
#define CV_COLOR_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvColorInfo
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvColorInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvColorInfo();
	virtual ~CvColorInfo();

	DllExport const NiColorA& getColor() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvColorInfo* pClassInfo);

protected:
	NiColorA m_Color;
};

#endif // CV_COLOR_INFO_H
