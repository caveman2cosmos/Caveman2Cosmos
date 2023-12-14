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

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvHeritageInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;
	void doPostLoadCaching(uint32_t eThis);

	inline void setMissionType(const int iNewType) { m_iMissionType = iNewType; }
	inline int getMissionType() const { return m_iMissionType; }
	inline bool needLanguage() const { return m_bNeedLanguage; }

	inline const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }
	inline const IDValueMap<EraTypes, CommerceArray>& getEraCommerceChanges100() const { return m_eraCommerceChanges; }

private:
	int m_iMissionType;
	bool m_bNeedLanguage;

	CvPropertyManipulators m_PropertyManipulators;
	IDValueMap<EraTypes, CommerceArray> m_eraCommerceChanges;
};
#endif