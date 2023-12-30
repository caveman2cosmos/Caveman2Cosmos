#pragma once

#ifndef Cv_PlayerOption_Info
#define Cv_PlayerOption_Info

#include "CvGameCoreDLL.h"
#include "CvInfos.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPlayerOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerOptionInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:
	CvPlayerOptionInfo();
	virtual ~CvPlayerOptionInfo();

	DllExport bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvPlayerOptionInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	bool m_bDefault;
};

#endif