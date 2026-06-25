#pragma once

#ifndef CV_CIVILIZATION_INFO_H
#define CV_CIVILIZATION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCivilizationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoCivilization;
class CvCivilizationInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCivilizationInfo();
	virtual ~CvCivilizationInfo();
	virtual void reset();

	DllExport int getDefaultPlayerColor() const;
	int getArtStyleType() const;
	int getUnitArtStyleType() const;
	int getNumCityNames() const;
	// the number of leaders the Civ has, this is needed so that random leaders can be generated easily
	int getNumLeaders() const;
	int getSelectionSoundScriptId() const;
	int getActionSoundScriptId() const;

	DllExport bool isAIPlayable() const;
	DllExport bool isPlayable() const;

	std::wstring pyGetShortDescription(uint uiForm) { return getShortDescription(uiForm); }
	DllExport const wchar_t* getShortDescription(uint uiForm = 0);
	const wchar_t* getShortDescriptionKey() const;
	const std::wstring pyGetShortDescriptionKey() const { return getShortDescriptionKey(); }

	std::wstring pyGetAdjective(uint uiForm) { return getAdjective(uiForm); }
	DllExport const wchar_t* getAdjective(uint uiForm = 0);
	const wchar_t* getAdjectiveKey() const;
	const std::wstring pyGetAdjectiveKey() const { return getAdjectiveKey(); }

	DllExport const char* getFlagTexture() const;
	const char* getArtDefineTag() const;

	int getCivilizationInitialCivics(int i) const;
	void setCivilizationInitialCivics(int iCivicOption, int iCivic);

	DllExport bool isLeaders(int i) const;

	bool isCivilizationFreeTechs(int i) const;
	bool isCivilizationDisableTechs(int i) const;

	int getNumCivilizationBuildings() const;
	int getCivilizationBuilding(int i) const;
	bool isCivilizationBuilding(int i) const;

	std::string getCityNames(int i) const;

	const CvArtInfoCivilization* getArtInfo() const;
	const char* getButton() const;

	CivilizationTypes getDerivativeCiv() const { return m_iDerivativeCiv; }

	//TB Tags
	//int
	int getSpawnRateModifier() const;
	int getSpawnRateNPCPeaceModifier() const;
	// Means that the civilization cannot build or train anything which is not specified
	// as allowed in Unit or Building Info by the EnabledCivilization tag.  Generally used for NPC players.
	bool isStronglyRestricted() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvCivilizationInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	int m_iDefaultPlayerColor;
	int m_iArtStyleType;
	int m_iUnitArtStyleType;
	int m_iNumCityNames;
	int m_iNumLeaders;
	int m_iSelectionSoundScriptId;
	int m_iActionSoundScriptId;
	CivilizationTypes m_iDerivativeCiv;
	// TB Tags
	int m_iSpawnRateModifier;
	int m_iSpawnRateNPCPeaceModifier;

	bool m_bStronglyRestricted;
	// ! TB Tags
	bool m_bAIPlayable;
	bool m_bPlayable;

	CvString m_szArtDefineTag;
	CvWString m_szShortDescriptionKey;
	CvWString m_szAdjectiveKey;

	int* m_piCivilizationInitialCivics;

	bool* m_pbLeaders;
	bool* m_pbCivilizationFreeTechs;
	bool* m_pbCivilizationDisableTechs;

	CvString* m_paszCityNames;

	std::vector<int> m_aiCivilizationBuildings;

	mutable std::vector<CvWString> m_aszShortDescription;
	mutable std::vector<CvWString> m_aszAdjective;
};

#endif // CV_CIVILIZATION_INFO_H
