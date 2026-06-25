#pragma once

#ifndef CV_CIVIC_OPTION_INFO_H
#define CV_CIVIC_OPTION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCivicOptionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCivicOptionInfo
	: public CvInfoBase
	, private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCivicOptionInfo();
	virtual ~CvCivicOptionInfo();

//	bool getTraitNoUpkeep(int i) const;
	bool isPolicy() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvCivicOptionInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	bool* m_pabTraitNoUpkeep;
	bool m_bPolicy;
};

#endif // CV_CIVIC_OPTION_INFO_H
