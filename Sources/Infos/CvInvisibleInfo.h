#pragma once

#ifndef CV_INVISIBLE_INFO_H
#define CV_INVISIBLE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInvisibleInfo
//
//  DESC:   Contains info about Invisible Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInvisibleInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvInvisibleInfo();
	virtual ~CvInvisibleInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvInvisibleInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	void setChar(int i);
	int getChar() const { return m_iChar; }

	int getFontButtonIndex() const { return m_iFontButtonIndex; }

	bool isIntrinsic() const { return m_bIntrinsic; }

private:
	int m_iChar;
	int m_iFontButtonIndex;
	bool m_bIntrinsic;
};

#endif // CV_INVISIBLE_INFO_H
