#pragma once

#ifndef CV_FORCE_CONTROL_INFO_H
#define CV_FORCE_CONTROL_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvForceControlInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvForceControlInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:
	CvForceControlInfo();
	virtual ~CvForceControlInfo();

	bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvForceControlInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	bool m_bDefault;
};

#endif // CV_FORCE_CONTROL_INFO_H
