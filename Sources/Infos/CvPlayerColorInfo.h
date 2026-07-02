#pragma once

#ifndef CV_PLAYER_COLOR_INFO_H
#define CV_PLAYER_COLOR_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPlayerColorInfo (ADD to Python)
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerColorInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvPlayerColorInfo();
	virtual ~CvPlayerColorInfo();

	DllExport int getColorTypePrimary() const;
	DllExport int getColorTypeSecondary() const;
	int getTextColorType() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvPlayerColorInfo* pClassInfo);

protected:

	int m_iColorTypePrimary;
	int m_iColorTypeSecondary;
	int m_iTextColorType;
};

#endif // CV_PLAYER_COLOR_INFO_H
