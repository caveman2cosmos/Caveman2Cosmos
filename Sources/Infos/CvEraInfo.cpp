//------------------------------------------------------------------------------------------------
//  FILE:    CvEraInfo.cpp
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
#include "CvEraInfo.h"


//------------------------------------------------------------------------------------------------------
//
//  CvEraInfo
//

CvEraInfo::CvEraInfo() :
m_iStartingUnitMultiplier(0),
m_iStartingDefenseUnits(0),
m_iStartingWorkerUnits(0),
m_iStartingExploreUnits(0),
m_iAdvancedStartPoints(0),
m_iStartingGold(0),
m_iFreePopulation(0),
m_iStartPercent(0),
m_iGrowthPercent(0),
m_iTrainPercent(0),
m_iConstructPercent(0),
m_iCreatePercent(0),
m_iResearchPercent(0),
m_iBuildPercent(0),
m_iImprovementPercent(0),
m_iGreatPeoplePercent(0),
m_iAnarchyPercent(0),
m_iEventChancePerTurn(0),
m_iSoundtrackSpace(0),
m_iNumSoundtracks(0),
m_iCuttingEdgeCutsTechCostModifier(0),
m_bNoGoodies(false),
m_bNoAnimals(false),
m_bNoBarbUnits(false),
m_bNoBarbCities(false),
m_bFirstSoundtrackFirst(false),
m_paiCitySoundscapeSciptIds(NULL),
m_paiSoundtracks(NULL)
,m_iInitialCityMaintenancePercent(0)
{ }


CvEraInfo::~CvEraInfo()
{
	SAFE_DELETE_ARRAY(m_paiCitySoundscapeSciptIds);
	SAFE_DELETE_ARRAY(m_paiSoundtracks);
}


int CvEraInfo::getStartingUnitMultiplier() const
{
	return m_iStartingUnitMultiplier;
}


int CvEraInfo::getStartingDefenseUnits() const
{
	return m_iStartingDefenseUnits;
}


int CvEraInfo::getStartingWorkerUnits() const
{
	return m_iStartingWorkerUnits;
}


int CvEraInfo::getStartingExploreUnits() const
{
	return m_iStartingExploreUnits;
}


int CvEraInfo::getAdvancedStartPoints() const
{
	return m_iAdvancedStartPoints;
}


int CvEraInfo::getStartingGold() const
{
	return m_iStartingGold;
}


int CvEraInfo::getFreePopulation() const
{
	return m_iFreePopulation;
}


int CvEraInfo::getStartPercent() const
{
	return m_iStartPercent;
}


int CvEraInfo::getGrowthPercent() const
{
	return m_iGrowthPercent;
}


int CvEraInfo::getTrainPercent() const
{
	return m_iTrainPercent;
}


int CvEraInfo::getConstructPercent() const
{
	return m_iConstructPercent;
}


int CvEraInfo::getCreatePercent() const
{
	return m_iCreatePercent;
}


int CvEraInfo::getResearchPercent() const
{
	return m_iResearchPercent;
}


int CvEraInfo::getBuildPercent() const
{
	return m_iBuildPercent;
}


int CvEraInfo::getImprovementPercent() const
{
	return m_iImprovementPercent;
}


int CvEraInfo::getGreatPeoplePercent() const
{
	return m_iGreatPeoplePercent;
}


int CvEraInfo::getAnarchyPercent() const
{
	return m_iAnarchyPercent;
}


int CvEraInfo::getEventChancePerTurn() const
{
	return m_iEventChancePerTurn;
}


int CvEraInfo::getSoundtrackSpace() const
{
	return m_iSoundtrackSpace;
}


bool CvEraInfo::isFirstSoundtrackFirst() const
{
	return m_bFirstSoundtrackFirst;
}


int CvEraInfo::getNumSoundtracks() const
{
	return m_iNumSoundtracks;
}


int CvEraInfo::getCuttingEdgeCutsTechCostModifier() const
{
	return m_iCuttingEdgeCutsTechCostModifier;
}


const char* CvEraInfo::getAudioUnitVictoryScript() const
{
	return m_szAudioUnitVictoryScript;
}


const char* CvEraInfo::getAudioUnitDefeatScript() const
{
	return m_szAudioUnitDefeatScript;
}


bool CvEraInfo::isNoGoodies() const
{
	return m_bNoGoodies;
}


bool CvEraInfo::isNoAnimals() const
{
	return m_bNoAnimals;
}


bool CvEraInfo::isNoBarbUnits() const
{
	return m_bNoBarbUnits;
}


bool CvEraInfo::isNoBarbCities() const
{
	return m_bNoBarbCities;
}


int CvEraInfo::getInitialCityMaintenancePercent() const
{
	return m_iInitialCityMaintenancePercent;
}



// Arrays

int CvEraInfo::getSoundtracks(int i) const
{
	FASSERT_BOUNDS(0, getNumSoundtracks(), i);
	return m_paiSoundtracks ? m_paiSoundtracks[i] : -1;
}


int CvEraInfo::getCitySoundscapeSciptId(int i) const
{
//	FAssertMsg(i < ?, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiCitySoundscapeSciptIds ? m_paiCitySoundscapeSciptIds[i] : -1;
}


bool CvEraInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bNoGoodies, L"bNoGoodies");
	pXML->GetOptionalChildXmlValByName(&m_bNoAnimals, L"bNoAnimals");
	pXML->GetOptionalChildXmlValByName(&m_bNoBarbUnits, L"bNoBarbUnits");
	pXML->GetOptionalChildXmlValByName(&m_bNoBarbCities, L"bNoBarbCities");
	pXML->GetOptionalChildXmlValByName(&m_iStartingUnitMultiplier, L"iStartingUnitMultiplier");
	pXML->GetOptionalChildXmlValByName(&m_iStartingDefenseUnits, L"iStartingDefenseUnits");
	pXML->GetOptionalChildXmlValByName(&m_iStartingWorkerUnits, L"iStartingWorkerUnits");
	pXML->GetOptionalChildXmlValByName(&m_iStartingExploreUnits, L"iStartingExploreUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartPoints, L"iAdvancedStartPoints");
	pXML->GetOptionalChildXmlValByName(&m_iStartingGold, L"iStartingGold");
	pXML->GetOptionalChildXmlValByName(&m_iFreePopulation, L"iFreePopulation");
	pXML->GetOptionalChildXmlValByName(&m_iStartPercent, L"iStartPercent");
	pXML->GetOptionalChildXmlValByName(&m_iGrowthPercent, L"iGrowthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iTrainPercent, L"iTrainPercent");
	pXML->GetOptionalChildXmlValByName(&m_iConstructPercent, L"iConstructPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCreatePercent, L"iCreatePercent");
	pXML->GetOptionalChildXmlValByName(&m_iResearchPercent, L"iResearchPercent");
	pXML->GetOptionalChildXmlValByName(&m_iBuildPercent, L"iBuildPercent");
	pXML->GetOptionalChildXmlValByName(&m_iImprovementPercent, L"iImprovementPercent");
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeoplePercent, L"iGreatPeoplePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAnarchyPercent, L"iAnarchyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iEventChancePerTurn, L"iEventChancePerTurn");
	pXML->GetOptionalChildXmlValByName(&m_iSoundtrackSpace, L"iSoundtrackSpace");
	pXML->GetOptionalChildXmlValByName(&m_bFirstSoundtrackFirst, L"bFirstSoundtrackFirst");
	pXML->GetOptionalChildXmlValByName(m_szAudioUnitVictoryScript, L"AudioUnitVictoryScript");
	pXML->GetOptionalChildXmlValByName(m_szAudioUnitDefeatScript, L"AudioUnitDefeatScript");
	pXML->GetOptionalChildXmlValByName(&m_iInitialCityMaintenancePercent, L"iInitialCityMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iCuttingEdgeCutsTechCostModifier, L"iCuttingEdgeCutsTechCostModifier");

	if (m_iInitialCityMaintenancePercent < 0)
	{
		m_iInitialCityMaintenancePercent = 0;
	}

	if (pXML->TryMoveToXmlFirstChild(L"EraInfoSoundtracks"))
	{
		CvString* pszSoundTrackNames = NULL;
		pXML->SetStringList(&pszSoundTrackNames, &m_iNumSoundtracks);

		if (m_iNumSoundtracks > 0)
		{
			m_paiSoundtracks = new int[m_iNumSoundtracks];

			int j;
			for (j=0;j<m_iNumSoundtracks;j++)
			{
				m_paiSoundtracks[j] = ((!gDLL->getAudioDisabled()) ? gDLL->getAudioTagIndex(pszSoundTrackNames[j], AUDIOTAG_2DSCRIPT) : -1);
			}
		}
		else
		{
			m_paiSoundtracks = NULL;
		}

		pXML->MoveToXmlParent();

		SAFE_DELETE_ARRAY(pszSoundTrackNames);
	}

	pXML->SetVariableListTagPairForAudioScripts(&m_paiCitySoundscapeSciptIds, L"CitySoundscapes", GC.getNumCitySizeTypes());

	return true;
}


void CvEraInfo::copyNonDefaults(const CvEraInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iAudioDefault = -1;  //all audio is default -1
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isNoGoodies() == bDefault) m_bNoGoodies = pClassInfo->isNoGoodies();
	if (isNoAnimals() == bDefault) m_bNoAnimals = pClassInfo->isNoAnimals();
	if (isNoBarbUnits() == bDefault) m_bNoBarbUnits = pClassInfo->isNoBarbUnits();
	if (isNoBarbCities() == bDefault) m_bNoBarbCities = pClassInfo->isNoBarbCities();
	if (getStartingUnitMultiplier() == iDefault) m_iStartingUnitMultiplier = pClassInfo->getStartingUnitMultiplier();
	if (getStartingDefenseUnits() == iDefault) m_iStartingDefenseUnits = pClassInfo->getStartingDefenseUnits();
	if (getStartingWorkerUnits() == iDefault) m_iStartingWorkerUnits = pClassInfo->getStartingWorkerUnits();
	if (getStartingExploreUnits() == iDefault) m_iStartingExploreUnits = pClassInfo->getStartingExploreUnits();
	if (getAdvancedStartPoints() == iDefault) m_iAdvancedStartPoints = pClassInfo->getAdvancedStartPoints();
	if (getStartingGold() == iDefault) m_iStartingGold = pClassInfo->getStartingGold();
	if (getFreePopulation() == iDefault) m_iFreePopulation = pClassInfo->getFreePopulation();
	if (getStartPercent() == iDefault) m_iStartPercent = pClassInfo->getStartPercent();
	if (getGrowthPercent() == iDefault) m_iGrowthPercent = pClassInfo->getGrowthPercent();
	if (getTrainPercent() == iDefault) m_iTrainPercent = pClassInfo->getTrainPercent();
	if (getConstructPercent() == iDefault) m_iConstructPercent = pClassInfo->getConstructPercent();
	if (getCreatePercent() == iDefault) m_iCreatePercent = pClassInfo->getCreatePercent();
	if (getResearchPercent() == iDefault) m_iResearchPercent = pClassInfo->getResearchPercent();
	if (getBuildPercent() == iDefault) m_iBuildPercent = pClassInfo->getBuildPercent();
	if (getImprovementPercent() == iDefault) m_iImprovementPercent = pClassInfo->getImprovementPercent();
	if (getGreatPeoplePercent() == iDefault) m_iGreatPeoplePercent = pClassInfo->getGreatPeoplePercent();
	if (getAnarchyPercent() == iDefault) m_iAnarchyPercent = pClassInfo->getAnarchyPercent();
	if (getCuttingEdgeCutsTechCostModifier() == iDefault) m_iCuttingEdgeCutsTechCostModifier = pClassInfo->getCuttingEdgeCutsTechCostModifier();
	if (getEventChancePerTurn() == iDefault) m_iEventChancePerTurn = pClassInfo->getEventChancePerTurn();
	if (getSoundtrackSpace() == iDefault) m_iSoundtrackSpace = pClassInfo->getSoundtrackSpace();
	if (isFirstSoundtrackFirst() == bDefault) m_bFirstSoundtrackFirst = pClassInfo->isFirstSoundtrackFirst();
	if (getAudioUnitVictoryScript() == cDefault) m_szAudioUnitVictoryScript = pClassInfo->getAudioUnitVictoryScript();
	if (getAudioUnitDefeatScript() == cDefault) m_szAudioUnitDefeatScript = pClassInfo->getAudioUnitDefeatScript();
	if (m_iInitialCityMaintenancePercent == iDefault) m_iInitialCityMaintenancePercent = pClassInfo->getInitialCityMaintenancePercent();

	if (m_iInitialCityMaintenancePercent < 0)
	{
		m_iInitialCityMaintenancePercent = 0;
	}

	if ( pClassInfo->getNumSoundtracks() != 0 )
	{
		int iNumSoundTracks = getNumSoundtracks() + pClassInfo->getNumSoundtracks();
		int* m_paiSoundtracksTemp = new int[iNumSoundTracks];
		for ( int i = 0; i < iNumSoundTracks; i++)
		{
			if ( i < getNumSoundtracks() )
			{
				m_paiSoundtracksTemp[i] = getSoundtracks(i);
			}
			else
			{
				m_paiSoundtracksTemp[i] = pClassInfo->getSoundtracks(i - getNumSoundtracks());
			}
		}
		SAFE_DELETE_ARRAY(m_paiSoundtracks);
		m_paiSoundtracks = new int[iNumSoundTracks];
		for ( int i = 0; i < iNumSoundTracks; i++)
		{
			m_paiSoundtracks[i] = m_paiSoundtracksTemp[i];
		}
		SAFE_DELETE_ARRAY(m_paiSoundtracksTemp);
	}

	for ( int i = 0; i < GC.getNumCitySizeTypes(); i++)
	{
		if ( getCitySoundscapeSciptId(i) == iAudioDefault && pClassInfo->getCitySoundscapeSciptId(i) != iAudioDefault)
		{
			if ( NULL == m_paiCitySoundscapeSciptIds )
			{
				CvXMLLoadUtility::InitList(&m_paiCitySoundscapeSciptIds,GC.getNumCitySizeTypes(),iAudioDefault);
			}
			m_paiCitySoundscapeSciptIds[i] = pClassInfo->getCitySoundscapeSciptId(i);
		}
	}
}


void CvEraInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iStartingUnitMultiplier);
	CheckSum(iSum, m_iStartingDefenseUnits);
	CheckSum(iSum, m_iStartingWorkerUnits);
	CheckSum(iSum, m_iStartingExploreUnits);
	CheckSum(iSum, m_iAdvancedStartPoints);
	CheckSum(iSum, m_iStartingGold);
	CheckSum(iSum, m_iFreePopulation);
	CheckSum(iSum, m_iStartPercent);
	CheckSum(iSum, m_iGrowthPercent);
	CheckSum(iSum, m_iTrainPercent);
	CheckSum(iSum, m_iConstructPercent);
	CheckSum(iSum, m_iCreatePercent);
	CheckSum(iSum, m_iResearchPercent);
	CheckSum(iSum, m_iBuildPercent);
	CheckSum(iSum, m_iImprovementPercent);
	CheckSum(iSum, m_iGreatPeoplePercent);
	CheckSum(iSum, m_iAnarchyPercent);
	CheckSum(iSum, m_iEventChancePerTurn);
	CheckSum(iSum, m_iCuttingEdgeCutsTechCostModifier);

	CheckSum(iSum, m_bNoGoodies);
	CheckSum(iSum, m_bNoAnimals);
	CheckSum(iSum, m_bNoBarbUnits);
	CheckSum(iSum, m_bNoBarbCities);
}

