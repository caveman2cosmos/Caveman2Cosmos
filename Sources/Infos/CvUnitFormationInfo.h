#pragma once

#ifndef CV_UNIT_FORMATION_INFO_H
#define CV_UNIT_FORMATION_INFO_H

#include "CvInfoBase.h"

class CvUnitFormationInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	DllExport CvUnitFormationInfo();
	virtual ~CvUnitFormationInfo();

	DllExport const char* getFormationType() const;
	int getNumEventTypes() const;
	const EntityEventTypes &getEventType(int index) const;
	DllExport const std::vector<EntityEventTypes> & getEventTypes() const;

	DllExport int getNumUnitEntries() const;
	DllExport const CvUnitEntry &getUnitEntry(int index) const;
	DllExport void addUnitEntry(const CvUnitEntry &unitEntry);
	int getNumGreatUnitEntries() const;
	DllExport const CvUnitEntry &getGreatUnitEntry(int index) const;
	int getNumSiegeUnitEntries() const;
	DllExport const CvUnitEntry &getSiegeUnitEntry(int index) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvUnitFormationInfo* pClassInfo);

	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------

protected:

	CvString m_szFormationType;
	std::vector<EntityEventTypes>	m_vctEventTypes;		//!< The list of EntityEventTypes that this formation is intended for

	std::vector<CvUnitEntry> m_vctUnitEntries;
	std::vector<CvUnitEntry> m_vctGreatUnitEntries;
	std::vector<CvUnitEntry> m_vctSiegeUnitEntries;
};

#endif // CV_UNIT_FORMATION_INFO_H
