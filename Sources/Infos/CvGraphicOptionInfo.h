#pragma once

#ifndef CV_GRAPHIC_OPTION_INFO_H
#define CV_GRAPHIC_OPTION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGraphicOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGraphicOptionInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:
	CvGraphicOptionInfo();
	virtual ~CvGraphicOptionInfo();

	DllExport bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvGraphicOptionInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	bool m_bDefault;
};

#endif // CV_GRAPHIC_OPTION_INFO_H
