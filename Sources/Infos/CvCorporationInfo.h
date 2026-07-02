#pragma once

#ifndef CV_CORPORATION_INFO_H
#define CV_CORPORATION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCorporationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCorporationInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE----------------------------------------
public:

	CvCorporationInfo();
	virtual ~CvCorporationInfo();

	int getChar() const;
	// TGA_INDEXATION 01/21/08 MRGENIE
	int getTGAIndex() const;

	void setChar(int i);
	int getHeadquarterChar() const;
	void setHeadquarterChar(int i);
	TechTypes getTechPrereq() const { return m_iTechPrereq; }
	int getFreeUnit() const;
	int getSpreadFactor() const;
	int getSpreadCost() const;
	int getMaintenance() const;
	int getMissionType() const;
	void setMissionType(int iNewType);

	int getBonusProduced() const;

	const char* getMovieFile() const;
	const char* getMovieSound() const;
	const char* getSound() const;

	const std::vector<BonusTypes>& getPrereqBonuses() const;

	// Arrays
	int getHeadquarterCommerce(int i) const;
	int* getHeadquarterCommerceArray() const;
	int getCommerceProduced(int i) const;
	int* getCommerceProducedArray() const;
	int getYieldProduced(int i) const;
	int* getYieldProducedArray() const;

	bool read(CvXMLLoadUtility* pXML);

	TechTypes getObsoleteTech() const;
	int getSpread() const;
	int getHealth() const;
	int getHappiness() const;
	int getMilitaryProductionModifier() const;
	int getFreeXP() const;
	int getPrereqGameOption() const;

	int getPrereqBuilding(int i) const;
	int getPrereqBuildingVectorSize() const;
	CvString getPrereqBuildingNamesVectorElement(const int i) const;
	int getPrereqBuildingValuesVectorElement(const int i) const;

	bool isCompetingCorporation(int i) const;
	int getCompetingCorporationVectorSize() const;
	CvString getCompetingCorporationNamesVectorElement(const int i) const;
	bool getCompetingCorporationValuesVectorElement(const int i) const;

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	bool readPass3();
	int getYieldChange(int i) const;
	int* getYieldChangeArray() const;
	int getCommerceChange(int i) const;
	int* getCommerceChangeArray() const;

	void copyNonDefaults(const CvCorporationInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

private:

	std::vector<int> m_aiCategories;

	CvPropertyManipulators m_PropertyManipulators;

protected:
	// TGA_INDEXATION 01/21/08 MRGENIE
	int m_iTGAIndex;

	int m_iChar;
	int m_iHeadquarterChar;
	TechTypes m_iTechPrereq;
	int m_iFreeUnit;
	int m_iSpreadFactor;
	int m_iSpreadCost;
	int m_iMaintenance;
	int m_iMissionType;
	int m_iBonusProduced;

	TechTypes m_iObsoleteTech;
	int m_iSpread;
	int m_iHealth;
	int m_iHappiness;
	int m_iFreeXP;
	int m_iMilitaryProductionModifier;
	int m_iPrereqGameOption;

	int* m_paiPrereqBuilding;
	std::vector<CvString> m_aszPrereqBuildingforPass3;
	std::vector<int> m_aiPrereqBuildingforPass3;

	bool* m_pabCompetingCorporation;
	std::vector<CvString> m_aszCompetingCorporationforPass3;
	std::vector<bool> m_abCompetingCorporationforPass3;

	int* m_piYieldChange;
	int* m_piCommerceChange;

	CvString m_szMovieFile;
	CvString m_szMovieSound;
	CvString m_szSound;

	std::vector<BonusTypes> m_vPrereqBonuses;

	// Arrays
	int* m_paiHeadquarterCommerce;
	int* m_paiCommerceProduced;
	int* m_paiYieldProduced;
};

#endif // CV_CORPORATION_INFO_H
