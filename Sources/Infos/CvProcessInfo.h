#pragma once

#ifndef CV_PROCESS_INFO_H
#define CV_PROCESS_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvProcessInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProcessInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvProcessInfo();
	virtual ~CvProcessInfo();

	TechTypes getTechPrereq() const { return m_iTechPrereq; }

	// Arrays

	int getProductionToCommerceModifier(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvProcessInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	TechTypes m_iTechPrereq;

	// Arrays
	int* m_paiProductionToCommerceModifier;
};

#endif // CV_PROCESS_INFO_H
