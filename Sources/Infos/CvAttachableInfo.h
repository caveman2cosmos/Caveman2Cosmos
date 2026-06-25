#pragma once

#ifndef CV_ATTACHABLE_INFO_H
#define CV_ATTACHABLE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAttachableInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAttachableInfo
	: public CvInfoBase
	, public CvScalableInfo
	, private bst::noncopyable
{
public:

	CvAttachableInfo();
	virtual ~CvAttachableInfo();

	DllExport const char* getPath() const { return m_szPath; }
	void setPath(const char* szVal) { m_szPath = szVal; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvAttachableInfo* pClassInfo);

private:
	CvString m_szPath;
	float m_fUpdateRate;
};

#endif // CV_ATTACHABLE_INFO_H
