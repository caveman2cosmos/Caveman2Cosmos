#pragma once
#ifndef CV_HERITAGEINFO_H
#define CV_HERITAGEINFO_H

#include "CvInfos.h"
#include "IDValuemap.h"

class CvXMLLoadUtility;

class CvHeritageInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvHeritageInfo();
	virtual ~CvHeritageInfo();

	void setMissionType(const int iNewType) { m_iMissionType = iNewType; }
	int getMissionType() const { return m_iMissionType; }
	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }
	const IDValueMap<TechTypes, CommerceArray>& getTechCommerceChanges100() const { return m_techCommerceChanges; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvHeritageInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;
	void doPostLoadCaching(uint32_t eThis);

private:
	int m_iMissionType;
	CvPropertyManipulators m_PropertyManipulators;
	IDValueMap<TechTypes, CommerceArray> m_techCommerceChanges;
};
#endif