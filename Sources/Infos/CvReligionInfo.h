#pragma once

#ifndef CV_RELIGION_INFO_H
#define CV_RELIGION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvReligionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvReligionInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE----------------------------------------
public:

	CvReligionInfo();
	virtual ~CvReligionInfo();

	int getChar() const;
	// TGA_INDEXATION 01/21/08 MRGENIE
	int getTGAIndex() const;
	//void setTGAIndex(int i);

	void setChar(int i);
	int getHolyCityChar() const;
	void setHolyCityChar(int i);
	TechTypes getTechPrereq() const { return m_iTechPrereq; }
	int getFreeUnit() const;
	int getNumFreeUnits() const;
	int getSpreadFactor() const;
	int getMissionType() const;
	void setMissionType(int iNewType);

	const char* getTechButton() const;
	const char* getGenericTechButton() const;
	const char* getMovieFile() const;
	const char* getMovieSound() const;
	const char* getSound() const;
	const char* getButtonDisabled() const;

	void setAdjectiveKey(const char* szVal);
	const wchar_t* getAdjectiveKey() const;
	std::wstring pyGetAdjectiveKey() const { return getAdjectiveKey(); }

	// Arrays

	int getGlobalReligionCommerce(int i) const;
	int* getGlobalReligionCommerceArray() const;
	int getHolyCityCommerce(int i) const;
	int* getHolyCityCommerceArray() const;
	int getStateReligionCommerce(int i) const;
	int* getStateReligionCommerceArray() const;
	int getFlavorValue(int i) const;

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	const std::vector<BuildingTypes>& getShrineBuildings() const { return m_shrineBuildings; }
	void addShrineBuilding(BuildingTypes eBuilding) { m_shrineBuildings.push_back(eBuilding); }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvReligionInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

private:


	CvPropertyManipulators m_PropertyManipulators;

	// TGA_INDEXATION 01/21/08 MRGENIE
	int m_iTGAIndex;

	int m_iChar;
	int m_iHolyCityChar;
	TechTypes m_iTechPrereq;
	int m_iFreeUnit;
	int m_iNumFreeUnits;
	int m_iSpreadFactor;
	int m_iMissionType;

	CvString m_szTechButton;
	CvString m_szGenericTechButton;
	CvString m_szMovieFile;
	CvString m_szMovieSound;
	CvString m_szSound;
	CvWString m_szAdjectiveKey;

	// Arrays

	int* m_paiGlobalReligionCommerce;
	int* m_paiHolyCityCommerce;
	int* m_paiStateReligionCommerce;
	int* m_piFlavorValue;

	std::vector<int> m_aiCategories;
	std::vector<BuildingTypes> m_shrineBuildings;
};

#endif // CV_RELIGION_INFO_H
