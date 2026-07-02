//------------------------------------------------------------------------------------------------
//  FILE:    CvCivilizationInfo.cpp
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvHeritageInfo.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInfoUtil.h"
#include "CvPlayerAI.h"
#include "CvPython.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilityModTools.h"
#include "CheckSum.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvCivilizationInfo.h"


//======================================================================================================
//					CvCivilizationInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivilizationInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivilizationInfo::CvCivilizationInfo():
m_iDefaultPlayerColor(NO_PLAYERCOLOR),
m_iArtStyleType(NO_ARTSTYLE),
m_iUnitArtStyleType(NO_UNIT_ARTSTYLE),
m_iNumCityNames(0),
m_iNumLeaders(0),
m_iSelectionSoundScriptId(0),
m_iActionSoundScriptId(0),
m_iDerivativeCiv(NO_CIVILIZATION),
m_bPlayable(false),
m_bAIPlayable(false),
m_piCivilizationInitialCivics(NULL),
m_pbLeaders(NULL),
m_pbCivilizationFreeTechs(NULL),
m_pbCivilizationDisableTechs(NULL),
m_paszCityNames(NULL),
m_iSpawnRateModifier(0),
m_iSpawnRateNPCPeaceModifier(0),
m_bStronglyRestricted(false)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivilizationInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivilizationInfo::~CvCivilizationInfo()
{
	SAFE_DELETE_ARRAY(m_piCivilizationInitialCivics);
	SAFE_DELETE_ARRAY(m_pbLeaders);
	SAFE_DELETE_ARRAY(m_pbCivilizationFreeTechs);
	SAFE_DELETE_ARRAY(m_pbCivilizationDisableTechs);
	SAFE_DELETE_ARRAY(m_paszCityNames);
	GC.removeDelayedResolution((int*)&m_iDerivativeCiv);
}


void CvCivilizationInfo::reset()
{
	CvInfoBase::reset();
	m_aszAdjective.clear();
	m_aszShortDescription.clear();
}



int CvCivilizationInfo::getDefaultPlayerColor() const
{
	return m_iDefaultPlayerColor;
}


int CvCivilizationInfo::getArtStyleType() const
{
	return m_iArtStyleType;
}


int CvCivilizationInfo::getUnitArtStyleType() const
{
	return m_iUnitArtStyleType;
}


int CvCivilizationInfo::getNumCityNames() const
{
	return m_iNumCityNames;
}


int CvCivilizationInfo::getNumLeaders() const// the number of leaders the Civ has, this is needed so that random leaders can be generated easily
{
	return m_iNumLeaders;
}


int CvCivilizationInfo::getSelectionSoundScriptId() const
{
	return m_iSelectionSoundScriptId;
}


int CvCivilizationInfo::getActionSoundScriptId() const
{
	return m_iActionSoundScriptId;
}


bool CvCivilizationInfo::isAIPlayable() const
{
	return m_bAIPlayable;
}


bool CvCivilizationInfo::isPlayable() const
{
	return m_bPlayable;
}


const wchar_t* CvCivilizationInfo::getShortDescription(uint uiForm)
{
	PROFILE_EXTRA_FUNC();
	while(m_aszShortDescription.size() <= uiForm)
	{
		m_aszShortDescription.push_back(gDLL->getObjectText(m_szShortDescriptionKey, m_aszShortDescription.size()));
	}

	return m_aszShortDescription[uiForm];
}


const wchar_t* CvCivilizationInfo::getShortDescriptionKey() const
{
	return m_szShortDescriptionKey;
}


const wchar_t* CvCivilizationInfo::getAdjective(uint uiForm)
{
	PROFILE_EXTRA_FUNC();
	while(m_aszAdjective.size() <= uiForm)
	{
		m_aszAdjective.push_back(gDLL->getObjectText(m_szAdjectiveKey, m_aszAdjective.size()));
	}

	return m_aszAdjective[uiForm];
}


const wchar_t* CvCivilizationInfo::getAdjectiveKey() const
{
	return m_szAdjectiveKey;
}


const char* CvCivilizationInfo::getFlagTexture() const
{
	return ARTFILEMGR.getCivilizationArtInfo( getArtDefineTag() )->getPath();
}


const char* CvCivilizationInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}



int CvCivilizationInfo::getCivilizationInitialCivics(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), i);
	return (m_piCivilizationInitialCivics ? m_piCivilizationInitialCivics[i] : -1);
}


void CvCivilizationInfo::setCivilizationInitialCivics(int iCivicOption, int iCivic)
{
	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), iCivicOption);
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), iCivic);

	if ( NULL == m_piCivilizationInitialCivics )
	{
		CvXMLLoadUtility::InitList(&m_piCivilizationInitialCivics,GC.getNumCivicOptionInfos(),-1);
	}

	m_piCivilizationInitialCivics[iCivicOption] = iCivic;
}


bool CvCivilizationInfo::isLeaders(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumLeaderHeadInfos(), i);
	return m_pbLeaders ? m_pbLeaders[i] : false;
}


int CvCivilizationInfo::getNumCivilizationBuildings() const
{
	return (int)m_aiCivilizationBuildings.size();
}

int CvCivilizationInfo::getCivilizationBuilding(int i) const
{
	return m_aiCivilizationBuildings[i];
}

bool CvCivilizationInfo::isCivilizationBuilding(int i) const
{
	return algo::any_of_equal(m_aiCivilizationBuildings, i);
}


bool CvCivilizationInfo::isCivilizationFreeTechs(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), i);
	return m_pbCivilizationFreeTechs ? m_pbCivilizationFreeTechs[i] : false;
}


bool CvCivilizationInfo::isCivilizationDisableTechs(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), i);
	return m_pbCivilizationDisableTechs ? m_pbCivilizationDisableTechs[i] : false;
}


const CvArtInfoCivilization* CvCivilizationInfo::getArtInfo() const
{
	return ARTFILEMGR.getCivilizationArtInfo(getArtDefineTag());
}


const char* CvCivilizationInfo::getButton() const
{
	const CvArtInfoCivilization* pArtInfoCivilization = getArtInfo();
	return pArtInfoCivilization ? pArtInfoCivilization->getButton() : NULL;
}


std::string CvCivilizationInfo::getCityNames(int i) const
{
	FASSERT_BOUNDS(0, getNumCityNames(), i);
	return m_paszCityNames[i];
}


//TB Tags

int CvCivilizationInfo::getSpawnRateModifier() const
{
	return m_iSpawnRateModifier;
}


int CvCivilizationInfo::getSpawnRateNPCPeaceModifier() const
{
	return m_iSpawnRateNPCPeaceModifier;
}


bool CvCivilizationInfo::isStronglyRestricted() const
{
	return m_bStronglyRestricted;
}


void CvCivilizationInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iDerivativeCiv);
	CheckSum(iSum, m_bAIPlayable);
	CheckSum(iSum, m_bPlayable);
	CheckSum(iSum, m_iSpawnRateModifier);
	CheckSum(iSum, m_iSpawnRateNPCPeaceModifier);
	CheckSum(iSum, m_bStronglyRestricted);
	CheckSumI(iSum, GC.getNumCivicOptionInfos(), m_piCivilizationInitialCivics);
	CheckSumI(iSum, GC.getNumLeaderHeadInfos(), m_pbLeaders);
	CheckSumI(iSum, GC.getNumTechInfos(), m_pbCivilizationFreeTechs);
	CheckSumI(iSum, GC.getNumTechInfos(), m_pbCivilizationDisableTechs);
	CheckSumC(iSum, m_aiCivilizationBuildings);
}


bool CvCivilizationInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szShortDescriptionKey, L"ShortDescription");
	pXML->GetOptionalChildXmlValByName(m_szAdjectiveKey, L"Adjective");
	pXML->GetOptionalTypeEnum(m_iDefaultPlayerColor, L"DefaultPlayerColor");
	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");
	pXML->GetOptionalTypeEnum(m_iArtStyleType, L"ArtStyleType");
	pXML->GetOptionalTypeEnum(m_iUnitArtStyleType, L"UnitArtStyleType");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivilizationSelectionSound");
	m_iSelectionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivilizationActionSound");
	m_iActionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;

	// set the current xml node to it's next sibling and then
	pXML->GetOptionalChildXmlValByName(&m_bPlayable, L"bPlayable");
	pXML->GetOptionalChildXmlValByName(&m_bAIPlayable, L"bAIPlayable");

	if (pXML->TryMoveToXmlFirstChild(L"Cities"))
	{
		pXML->SetStringList(&m_paszCityNames, &m_iNumCityNames);
		pXML->MoveToXmlParent();
	}

	pXML->SetOptionalVector(&m_aiCivilizationBuildings, L"FreeBuildings");

	pXML->SetVariableListTagPair(&m_pbCivilizationFreeTechs, L"FreeTechs", GC.getNumTechInfos());
	pXML->SetVariableListTagPair(&m_pbCivilizationDisableTechs, L"DisableTechs", GC.getNumTechInfos());

	if (pXML->TryMoveToXmlFirstChild(L"InitialCivics"))
	{
		if (const int iNumSibs = pXML->GetXmlChildrenNumber())
		{
			CvXMLLoadUtility::InitList(&m_piCivilizationInitialCivics, GC.getNumCivicOptionInfos());
			if (pXML->GetChildXmlVal(szTextVal))
			{
				for (int j = 0; j < iNumSibs; j++)
				{
					const CivicTypes eCivic = (CivicTypes)pXML->GetInfoClass(szTextVal);//, true);
					if ( eCivic != NO_CIVIC )
					{
						const CivicOptionTypes eCivicOption = (CivicOptionTypes)GC.getCivicInfo(eCivic).getCivicOptionType();

						if ( eCivicOption != NO_CIVICOPTION )
						{
							FAssertMsg((eCivicOption < GC.getNumCivicOptionInfos()),"Bad default civic");
							m_piCivilizationInitialCivics[eCivicOption] = eCivic;
						}
					}

					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}
		else
		{
			SAFE_DELETE_ARRAY(m_piCivilizationInitialCivics);
		}

		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCivilizationInitialCivics);
	}

	pXML->SetVariableListTagPair(&m_pbLeaders, L"Leaders", GC.getNumLeaderHeadInfos());

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivilizationSelectionSound");

	//TB Tags
	pXML->GetOptionalChildXmlValByName(&m_iSpawnRateModifier, L"iSpawnRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iSpawnRateNPCPeaceModifier, L"iSpawnRateNPCPeaceModifier");
	pXML->GetOptionalChildXmlValByName(&m_bStronglyRestricted, L"bStronglyRestricted");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iDerivativeCiv, L"DerivativeCiv");

	return true;
}


void CvCivilizationInfo::copyNonDefaults(const CvCivilizationInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const int iDefault = 0;
	const int iTextDefault = -1;
	const bool bDefault = false;
	const CvString cDefault = CvString::format("").GetCString();
	const CvWString wDefault = CvWString::format(L"").GetCString();

	// must be before we set the InfoBaseClass else it can't find the button to to corresponding arttag
	if ( getArtDefineTag() == cDefault ) // "ArtDefineTag"
	{
		m_szArtDefineTag = pClassInfo->getArtDefineTag();
	}

	CvInfoBase::copyNonDefaults(pClassInfo);

	if ( isPlayable() == bDefault ) // "bPlayable"
	{
		m_bPlayable = (pClassInfo->isPlayable());
	}
	if ( isAIPlayable() == bDefault ) // "bAIPlayable"
	{
		m_bAIPlayable = (pClassInfo->isAIPlayable());
	}

	if ( getShortDescriptionKey() == wDefault )
	{
		m_szShortDescriptionKey = pClassInfo->getShortDescriptionKey();
	}

	if ( getAdjectiveKey() == wDefault ) // "Adjective"
	{
		m_szAdjectiveKey = pClassInfo->getAdjectiveKey();
	}

	if ( getDefaultPlayerColor() == NO_PLAYERCOLOR ) // "DefaultPlayerColor"
	{
		m_iDefaultPlayerColor = (pClassInfo->getDefaultPlayerColor());
	}
	if ( getArtStyleType() == NO_ARTSTYLE ) // "ArtStyleType"
	{
		m_iArtStyleType = (pClassInfo->getArtStyleType());
	}
	if ( getUnitArtStyleType() == NO_UNIT_ARTSTYLE ) // "UnitArtStyleType"
	{
		m_iUnitArtStyleType = (pClassInfo->getUnitArtStyleType());
	}
	if ( getSelectionSoundScriptId() == AUDIOTAG_NONE ) // "CivilizationSelectionSound"
	{
		m_iSelectionSoundScriptId = (pClassInfo->getSelectionSoundScriptId());
	}
	if ( getActionSoundScriptId() == AUDIOTAG_NONE ) // "CivilizationActionSound"
	{
		m_iActionSoundScriptId = (pClassInfo->getActionSoundScriptId());
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCivilizationBuildings, pClassInfo->m_aiCivilizationBuildings);

	for ( int i = 0; i < GC.getNumTechInfos(); i++)
	{
		if ( isCivilizationFreeTechs(i) == bDefault && pClassInfo->isCivilizationFreeTechs(i) != bDefault)
		{
			if ( NULL == m_pbCivilizationFreeTechs )
			{
				CvXMLLoadUtility::InitList(&m_pbCivilizationFreeTechs,GC.getNumTechInfos(),bDefault);
			}
			m_pbCivilizationFreeTechs[i] = pClassInfo->isCivilizationFreeTechs(i);
		}
		if ( isCivilizationDisableTechs(i) == bDefault && pClassInfo->isCivilizationDisableTechs(i) != bDefault)
		{
			if ( NULL == m_pbCivilizationDisableTechs )
			{
				CvXMLLoadUtility::InitList(&m_pbCivilizationDisableTechs,GC.getNumTechInfos(),bDefault);
			}
			m_pbCivilizationDisableTechs[i] = pClassInfo->isCivilizationDisableTechs(i);
		}
	}

	for ( int i = 0; i < GC.getNumCivicOptionInfos(); i++)
	{
		if ( getCivilizationInitialCivics(i) == -1 && pClassInfo->getCivilizationInitialCivics(i) != -1 )
		{
			if ( NULL == m_pbCivilizationDisableTechs )
			{
				CvXMLLoadUtility::InitList(&m_piCivilizationInitialCivics,GC.getNumCivicOptionInfos(),-1);
			}
			m_piCivilizationInitialCivics[i] = pClassInfo->getCivilizationInitialCivics(i);
		}
	}

	for ( int i = 0; i < GC.getNumLeaderHeadInfos(); i++)
	{
		if ( isLeaders(i) == bDefault && pClassInfo->isLeaders(i) != bDefault)
		{
			if ( NULL == m_pbLeaders )
			{
				CvXMLLoadUtility::InitList(&m_pbLeaders,GC.getNumLeaderHeadInfos(),bDefault);
			}
			m_pbLeaders[i] = pClassInfo->isLeaders(i);
		}
	}

	//TB Tags
	//int
	if ( getSpawnRateModifier() == iDefault )
	{
		m_iSpawnRateModifier = (pClassInfo->getSpawnRateModifier());
	}
	if ( getSpawnRateNPCPeaceModifier() == iDefault )
	{
		m_iSpawnRateNPCPeaceModifier = (pClassInfo->getSpawnRateNPCPeaceModifier());
	}
	//bool
	if ( isStronglyRestricted() == bDefault )
	{
		m_bStronglyRestricted = (pClassInfo->isStronglyRestricted());
	}

	// First we check if there are different Unique Names in the Modules(we want to keep all of them)
	// So we have to set the Arraysize properly, knowing the amount of Unique Names
	if ( pClassInfo->getNumCityNames() != 0 )
	{
		CvString* m_paszOldNames = new CvString[pClassInfo->getNumCityNames()];
		for ( int i = 0; i < pClassInfo->getNumCityNames(); i++)
		{
			m_paszOldNames[i] = pClassInfo->getCityNames(i);
		}

		CvXMLLoadUtilityModTools::StringArrayExtend(&m_paszCityNames, &m_iNumCityNames, &m_paszOldNames, pClassInfo->getNumCityNames());
		SAFE_DELETE_ARRAY(m_paszOldNames)
	}

	GC.copyNonDefaultDelayedResolution((int*)&m_iDerivativeCiv, (int*)&pClassInfo->m_iDerivativeCiv);
}

