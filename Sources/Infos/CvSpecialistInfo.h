#pragma once

#ifndef CV_SPECIALIST_INFO_H
#define CV_SPECIALIST_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpecialistInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialistInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpecialistInfo();
	virtual ~CvSpecialistInfo();

	int getGreatPeopleUnitType() const;
	int getGreatPeopleRateChange() const;
	int getMissionType() const;
	void setMissionType(int iNewType);
	// Afforess 03/26/10
	int getHealthPercent() const;
	int getHappinessPercent() const;

	bool isSlave() const;

	int getExperience() const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	bool isVisible() const;

	// Arrays

	int getYieldChange(int i) const;
	const int* getYieldChangeArray() const;
	int getCommerceChange(int i) const;
	int getFlavorValue(int i) const;

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	const char* getTexture() const;

	//Team Project (1)
	//TB Specialist Tags
	// int
	int getInsidiousness() const;
	int getInvestigation() const;

	//int getNumTechHappinessTypes() const;
	int getTechHappiness(TechTypes eTech) const;
	const IDValueMap<TechTypes, int>& getTechHappinessTypes() const { return m_aTechHappinessTypes; }

	//int getNumTechHealthTypes() const;
	int getTechHealth(TechTypes eTech) const;
	const IDValueMap<TechTypes, int>& getTechHealthTypes() const { return m_aTechHealthTypes; }

	int getNumUnitCombatExperienceTypes() const;
	const UnitCombatModifier& getUnitCombatExperienceType(int iUnitCombat) const;

	//TB Specialist Tags end

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvSpecialistInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	CvPropertyManipulators m_PropertyManipulators;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iGreatPeopleUnitType;
	int m_iGreatPeopleRateChange;
	int m_iMissionType;
	// Afforess 03/26/10
	int m_iHealthPercent;
	int m_iHappinessPercent;

	int m_iExperience;
	//TB Specialist Tags
	int m_iInsidiousness;
	int m_iInvestigation;

	// Afforess 03/26/10
	bool m_bSlave;

	bool m_bVisible;

	// Arrays
	int* m_piYieldChange;
	int* m_piCommerceChange;
	int* m_piFlavorValue;

	//Team Project (1)
	//TB Specialist Tags
	// int vector utilizing struct with delayed resolution
	IDValueMap<TechTypes, int> m_aTechHappinessTypes;
	IDValueMap<TechTypes, int> m_aTechHealthTypes;
	std::vector<UnitCombatModifier> m_aUnitCombatExperienceTypes;
	std::vector<UnitCombatModifier> m_aUnitCombatExperienceTypesNull;

	std::vector<int> m_aiCategories;

	CvString m_szTexture;
};

#endif // CV_SPECIALIST_INFO_H
