#pragma once

#ifndef CV_MPOPTION_INFO_H
#define CV_MPOPTION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMPOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMPOptionInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:
	CvMPOptionInfo();
	virtual ~CvMPOptionInfo();

	bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvMPOptionInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	bool m_bDefault;
};

#endif // CV_MPOPTION_INFO_H
