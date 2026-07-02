//------------------------------------------------------------------------------------------------
//  FILE:    CvEventTriggerInfo.cpp
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
#include "CvEventTriggerInfo.h"


//////////////////////////////////////////////////////////////////////////
//
//	CvEventTriggerInfo
//	Event triggers
//
//
CvEventTriggerInfo::CvEventTriggerInfo() :
	m_iPercentGamesActive(0),
	m_iProbability(0),
	m_iNumUnits(0),
	m_iNumBuildings(0),
	m_iNumUnitsGlobal(0),
	m_iNumBuildingsGlobal(0),
	m_iNumPlotsRequired(0),
	m_iPlotType(NO_PLOT),
	m_iNumReligions(0),
	m_iNumCorporations(0),
	m_iOtherPlayerShareBorders(0),
	m_iOtherPlayerHasTech(NO_TECH),
	m_iCivic(NO_CIVIC),
	m_iMinPopulation(0),
	m_iMaxPopulation(0),
	m_iMinMapLandmass(0),
	m_iMinOurLandmass(0),
	m_iMaxOurLandmass(-1),
	m_iMinDifficulty(NO_HANDICAP),
	m_iAngry(0),
	m_iUnhealthy(0),
	m_iUnitDamagedWeight(0),
	m_iUnitDistanceWeight(0),
	m_iUnitExperienceWeight(0),
	m_iMinTreasury(0),
	m_bSinglePlayer(false),
	m_bTeam(false),
	m_bRecurring(false),
	m_bGlobal(false),
	m_bPickPlayer(false),
	m_bOtherPlayerWar(false),
	m_bOtherPlayerHasReligion(false),
	m_bOtherPlayerHasOtherReligion(false),
	m_bOtherPlayerAI(false),
	m_bPickCity(false),
	m_bPickOtherPlayerCity(false),
	m_bShowPlot(true),
	m_iCityFoodWeight(0),
	m_bUnitsOnPlot(false),
	m_bOwnPlot(false),
	m_bPickReligion(false),
	m_bStateReligion(false),
	m_bHolyCity(false),
	m_bPickCorporation(false),
	m_bHeadquarters(false),
	m_bProbabilityUnitMultiply(false),
	m_bProbabilityBuildingMultiply(false),
	m_bPrereqEventCity(false)
{
}


CvEventTriggerInfo::~CvEventTriggerInfo()
{
}


int CvEventTriggerInfo::getPercentGamesActive() const
{
	return m_iPercentGamesActive;
}


int CvEventTriggerInfo::getProbability() const
{
	return m_iProbability;
}


int CvEventTriggerInfo::getUnitRequired(int i) const
{
	return m_aiUnitsRequired[i];
}


int CvEventTriggerInfo::getNumUnitsRequired() const
{
	return (int)m_aiUnitsRequired.size();
}


int CvEventTriggerInfo::getBuildingRequired(int i) const
{
	return m_aiBuildingsRequired[i];
}


int CvEventTriggerInfo::getNumBuildingsRequired() const
{
	return (int)m_aiBuildingsRequired.size();
}


int CvEventTriggerInfo::getNumUnits() const
{
	return m_iNumUnits;
}


int CvEventTriggerInfo::getNumBuildings() const
{
	return m_iNumBuildings;
}


int CvEventTriggerInfo::getNumUnitsGlobal() const
{
	return m_iNumUnitsGlobal;
}


int CvEventTriggerInfo::getNumBuildingsGlobal() const
{
	return m_iNumBuildingsGlobal;
}


int CvEventTriggerInfo::getNumPlotsRequired() const
{
	return m_iNumPlotsRequired;
}


int CvEventTriggerInfo::getPlotType() const
{
	return m_iPlotType;
}


int CvEventTriggerInfo::getNumReligions() const
{
	return m_iNumReligions;
}


int CvEventTriggerInfo::getNumCorporations() const
{
	return m_iNumCorporations;
}


int CvEventTriggerInfo::getOtherPlayerShareBorders() const
{
	return m_iOtherPlayerShareBorders;
}


int CvEventTriggerInfo::getOtherPlayerHasTech() const
{
	return m_iOtherPlayerHasTech;
}


int CvEventTriggerInfo::getCivic() const
{
	return m_iCivic;
}


int CvEventTriggerInfo::getMinPopulation() const
{
	return m_iMinPopulation;
}


int CvEventTriggerInfo::getMaxPopulation() const
{
	return m_iMaxPopulation;
}


int CvEventTriggerInfo::getMinMapLandmass() const
{
	return m_iMinMapLandmass;
}


int CvEventTriggerInfo::getMinOurLandmass() const
{
	return m_iMinOurLandmass;
}


int CvEventTriggerInfo::getMaxOurLandmass() const
{
	return m_iMaxOurLandmass;
}


int CvEventTriggerInfo::getMinDifficulty() const
{
	return m_iMinDifficulty;
}


int CvEventTriggerInfo::getAngry() const
{
	return m_iAngry;
}


int CvEventTriggerInfo::getUnhealthy() const
{
	return m_iUnhealthy;
}


int CvEventTriggerInfo::getUnitDamagedWeight() const
{
	return m_iUnitDamagedWeight;
}


int CvEventTriggerInfo::getUnitDistanceWeight() const
{
	return m_iUnitDistanceWeight;
}


int CvEventTriggerInfo::getUnitExperienceWeight() const
{
	return m_iUnitExperienceWeight;
}


int CvEventTriggerInfo::getMinTreasury() const
{
	return m_iMinTreasury;
}


int CvEventTriggerInfo::getEvent(int i) const
{
	return m_aiEvents[i];
}


int CvEventTriggerInfo::getNumEvents() const
{
	return (int)m_aiEvents.size();
}


int CvEventTriggerInfo::getPrereqEvent(int i) const
{
	return m_aiPrereqEvents[i];
}


int CvEventTriggerInfo::getNumPrereqEvents() const
{
	return (int)m_aiPrereqEvents.size();
}


int CvEventTriggerInfo::getPrereqOrTechs(int i) const
{
	return m_aiPrereqOrTechs[i];
}


int CvEventTriggerInfo::getNumPrereqOrTechs() const
{
	return (int)m_aiPrereqOrTechs.size();
}


int CvEventTriggerInfo::getPrereqAndTechs(int i) const
{
	return m_aiPrereqAndTechs[i];
}


int CvEventTriggerInfo::getNumPrereqAndTechs() const
{
	return (int)m_aiPrereqAndTechs.size();
}


int CvEventTriggerInfo::getObsoleteTech(int i) const
{
	return m_aiObsoleteTechs[i];
}


int CvEventTriggerInfo::getNumObsoleteTechs() const
{
	return (int)m_aiObsoleteTechs.size();
}


int CvEventTriggerInfo::getFeatureRequired(int i) const
{
	return m_aiFeaturesRequired[i];
}


int CvEventTriggerInfo::getNumFeaturesRequired() const
{
	return (int)m_aiFeaturesRequired.size();
}


int CvEventTriggerInfo::getTerrainRequired(int i) const
{
	return m_aiTerrainsRequired[i];
}


int CvEventTriggerInfo::getNumTerrainsRequired() const
{
	return (int)m_aiTerrainsRequired.size();
}


int CvEventTriggerInfo::getImprovementRequired(int i) const
{
	return m_aiImprovementsRequired[i];
}


int CvEventTriggerInfo::getNumImprovementsRequired() const
{
	return (int)m_aiImprovementsRequired.size();
}


int CvEventTriggerInfo::getBonusRequired(int i) const
{
	return m_aiBonusesRequired[i];
}


int CvEventTriggerInfo::getNumBonusesRequired() const
{
	return (int)m_aiBonusesRequired.size();
}


int CvEventTriggerInfo::getRouteRequired(int i) const
{
	return m_aiRoutesRequired[i];
}


int CvEventTriggerInfo::getNumRoutesRequired() const
{
	return (int)m_aiRoutesRequired.size();
}


int CvEventTriggerInfo::getReligionRequired(int i) const
{
	return m_aiReligionsRequired[i];
}


int CvEventTriggerInfo::getNumReligionsRequired() const
{
	return (int)m_aiReligionsRequired.size();
}


int CvEventTriggerInfo::getCorporationRequired(int i) const
{
	return m_aiCorporationsRequired[i];
}


int CvEventTriggerInfo::getNumCorporationsRequired() const
{
	return (int)m_aiCorporationsRequired.size();
}


// Begin EmperorFool: Events with Images
const char* CvEventTriggerInfo::getEventArt() const
{
	if (m_szEventArt.empty())
	{
		return NULL;
	}

	return m_szEventArt;
}

// End EmperorFool: Events with Images

bool CvEventTriggerInfo::isSinglePlayer() const
{
	return m_bSinglePlayer;
}


bool CvEventTriggerInfo::isTeam() const
{
	return m_bTeam;
}


const CvWString& CvEventTriggerInfo::getText(int i) const
{
	FASSERT_BOUNDS(0, (int)m_aszText.size(), i);
	return m_aszText[i];
}


int CvEventTriggerInfo::getTextEra(int i) const
{
	FASSERT_BOUNDS(0, (int)m_aiTextEra.size(), i);
	return m_aiTextEra[i];
}


int CvEventTriggerInfo::getNumTexts() const
{
	FAssert(m_aiTextEra.size() == m_aszText.size());
	return m_aszText.size();
}


const CvWString& CvEventTriggerInfo::getWorldNews(int i) const
{
	FASSERT_BOUNDS(0, getNumWorldNews(), i);
	return m_aszWorldNews[i];
}


int CvEventTriggerInfo::getNumWorldNews() const
{
	return m_aszWorldNews.size();
}


bool CvEventTriggerInfo::isRecurring() const
{
	return m_bRecurring;
}


bool CvEventTriggerInfo::isGlobal() const
{
	return m_bGlobal;
}


bool CvEventTriggerInfo::isPickPlayer() const
{
	return m_bPickPlayer;
}


bool CvEventTriggerInfo::isOtherPlayerWar() const
{
	return m_bOtherPlayerWar;
}


bool CvEventTriggerInfo::isOtherPlayerHasReligion() const
{
	return m_bOtherPlayerHasReligion;
}


bool CvEventTriggerInfo::isOtherPlayerHasOtherReligion() const
{
	return m_bOtherPlayerHasOtherReligion;
}


bool CvEventTriggerInfo::isOtherPlayerAI() const
{
	return m_bOtherPlayerAI;
}


bool CvEventTriggerInfo::isPickCity() const
{
	return m_bPickCity;
}


bool CvEventTriggerInfo::isPickOtherPlayerCity() const
{
	return m_bPickOtherPlayerCity;
}


bool CvEventTriggerInfo::isShowPlot() const
{
	return m_bShowPlot;
}


int CvEventTriggerInfo::getCityFoodWeight() const
{
	return m_iCityFoodWeight;
}


bool CvEventTriggerInfo::isUnitsOnPlot() const
{
	return m_bUnitsOnPlot;
}


bool CvEventTriggerInfo::isOwnPlot() const
{
	return m_bOwnPlot;
}


bool CvEventTriggerInfo::isPickReligion() const
{
	return m_bPickReligion;
}


bool CvEventTriggerInfo::isStateReligion() const
{
	return m_bStateReligion;
}


bool CvEventTriggerInfo::isHolyCity() const
{
	return m_bHolyCity;
}


bool CvEventTriggerInfo::isPickCorporation() const
{
	return m_bPickCorporation;
}


bool CvEventTriggerInfo::isHeadquarters() const
{
	return m_bHeadquarters;
}


bool CvEventTriggerInfo::isProbabilityUnitMultiply() const
{
	return m_bProbabilityUnitMultiply;
}


bool CvEventTriggerInfo::isProbabilityBuildingMultiply() const
{
	return m_bProbabilityBuildingMultiply;
}


bool CvEventTriggerInfo::isPrereqEventCity() const
{
	return m_bPrereqEventCity;
}


const CvProperties* CvEventTriggerInfo::getPrereqMinProperties() const
{
	return &m_PrereqMinProperties;
}


const CvProperties* CvEventTriggerInfo::getPrereqMaxProperties() const
{
	return &m_PrereqMaxProperties;
}


const CvProperties* CvEventTriggerInfo::getPrereqPlayerMinProperties() const
{
	return &m_PrereqPlayerMinProperties;
}


const CvProperties* CvEventTriggerInfo::getPrereqPlayerMaxProperties() const
{
	return &m_PrereqPlayerMaxProperties;
}


const char* CvEventTriggerInfo::getPythonCallback() const
{
	return m_szPythonCallback;
}


const char* CvEventTriggerInfo::getPythonCanDo() const
{
	return m_szPythonCanDo;
}


const char* CvEventTriggerInfo::getPythonCanDoCity() const
{
	return m_szPythonCanDoCity;
}


const char* CvEventTriggerInfo::getPythonCanDoUnit() const
{
	return m_szPythonCanDoUnit;
}


// bool vector without delayed resolution
int CvEventTriggerInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}


int CvEventTriggerInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}


bool CvEventTriggerInfo::isNotOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiNotOnGameOptions, i);
}


int CvEventTriggerInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}


int CvEventTriggerInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}


bool CvEventTriggerInfo::isOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiOnGameOptions, i);
}


void CvEventTriggerInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iPercentGamesActive);
	CheckSum(iSum, m_iProbability);
	CheckSum(iSum, m_iNumUnits);
	CheckSum(iSum, m_iNumBuildings);
	CheckSum(iSum, m_iNumUnitsGlobal);
	CheckSum(iSum, m_iNumBuildingsGlobal);
	CheckSum(iSum, m_iNumPlotsRequired);
	CheckSum(iSum, m_iPlotType);
	CheckSum(iSum, m_iNumReligions);
	CheckSum(iSum, m_iNumCorporations);
	CheckSum(iSum, m_iOtherPlayerShareBorders);
	CheckSum(iSum, m_iOtherPlayerHasTech);
	CheckSum(iSum, m_iCivic);
	CheckSum(iSum, m_iMinPopulation);
	CheckSum(iSum, m_iMaxPopulation);
	CheckSum(iSum, m_iMinMapLandmass);
	CheckSum(iSum, m_iMinOurLandmass);
	CheckSum(iSum, m_iMaxOurLandmass);
	CheckSum(iSum, m_iMinDifficulty);
	CheckSum(iSum, m_iAngry);
	CheckSum(iSum, m_iUnhealthy);
	CheckSum(iSum, m_iUnitDamagedWeight);
	CheckSum(iSum, m_iUnitDistanceWeight);
	CheckSum(iSum, m_iUnitExperienceWeight);
	CheckSum(iSum, m_iMinTreasury);

	CheckSumC(iSum, m_aiUnitsRequired);
	CheckSumC(iSum, m_aiBuildingsRequired);
	CheckSumC(iSum, m_aiPrereqOrTechs);
	CheckSumC(iSum, m_aiPrereqAndTechs);
	CheckSumC(iSum, m_aiObsoleteTechs);
	CheckSumC(iSum, m_aiEvents);
	CheckSumC(iSum, m_aiPrereqEvents);
	CheckSumC(iSum, m_aiFeaturesRequired);
	CheckSumC(iSum, m_aiTerrainsRequired);
	CheckSumC(iSum, m_aiImprovementsRequired);
	CheckSumC(iSum, m_aiBonusesRequired);
	CheckSumC(iSum, m_aiRoutesRequired);
	CheckSumC(iSum, m_aiReligionsRequired);
	CheckSumC(iSum, m_aiCorporationsRequired);

	CheckSum(iSum, m_bSinglePlayer);
	CheckSum(iSum, m_bTeam);
	CheckSum(iSum, m_bRecurring);
	CheckSum(iSum, m_bGlobal);
	CheckSum(iSum, m_bPickPlayer);
	CheckSum(iSum, m_bOtherPlayerWar);
	CheckSum(iSum, m_bOtherPlayerHasReligion);
	CheckSum(iSum, m_bOtherPlayerHasOtherReligion);
	CheckSum(iSum, m_bOtherPlayerAI);
	CheckSum(iSum, m_bPickCity);
	CheckSum(iSum, m_bPickOtherPlayerCity);
	CheckSum(iSum, m_bShowPlot);
	CheckSum(iSum, m_iCityFoodWeight);
	CheckSum(iSum, m_bUnitsOnPlot);
	CheckSum(iSum, m_bOwnPlot);
	CheckSum(iSum, m_bPickReligion);
	CheckSum(iSum, m_bStateReligion);
	CheckSum(iSum, m_bHolyCity);
	CheckSum(iSum, m_bPickCorporation);
	CheckSum(iSum, m_bHeadquarters);
	CheckSum(iSum, m_bProbabilityUnitMultiply);
	CheckSum(iSum, m_bProbabilityBuildingMultiply);
	CheckSum(iSum, m_bPrereqEventCity);

	m_PrereqMinProperties.getCheckSum(iSum);
	m_PrereqMaxProperties.getCheckSum(iSum);
	m_PrereqPlayerMinProperties.getCheckSum(iSum);
	m_PrereqPlayerMaxProperties.getCheckSum(iSum);

	CheckSumC(iSum, m_szPythonCallback);
	CheckSumC(iSum, m_szPythonCanDo);
	CheckSumC(iSum, m_szPythonCanDoCity);
	CheckSumC(iSum, m_szPythonCanDoUnit);

	CheckSumC(iSum, m_aiNotOnGameOptions);
	CheckSumC(iSum, m_aiOnGameOptions);
}


bool CvEventTriggerInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iPercentGamesActive, L"iPercentGamesActive");
	pXML->GetOptionalChildXmlValByName(&m_iProbability, L"iWeight");
	pXML->GetOptionalChildXmlValByName(&m_iNumUnits, L"iNumUnits");
	pXML->GetOptionalChildXmlValByName(&m_iNumBuildings, L"iNumBuildings");
	pXML->GetOptionalChildXmlValByName(&m_iNumUnitsGlobal, L"iNumUnitsGlobal");
	pXML->GetOptionalChildXmlValByName(&m_iNumBuildingsGlobal, L"iNumBuildingsGlobal");
	pXML->GetOptionalChildXmlValByName(&m_iNumPlotsRequired, L"iNumPlotsRequired");
	pXML->GetOptionalChildXmlValByName(&m_iPlotType, L"iPlotType", NO_PLOT);
	pXML->GetOptionalChildXmlValByName(&m_iNumReligions, L"iNumReligions");
	pXML->GetOptionalChildXmlValByName(&m_iNumCorporations, L"iNumCorporations");
	pXML->GetOptionalChildXmlValByName(&m_iOtherPlayerShareBorders, L"iOtherPlayerShareBorders");
	pXML->GetOptionalChildXmlValByName(&m_iMinPopulation, L"iMinPopulation");
	pXML->GetOptionalChildXmlValByName(&m_iMaxPopulation, L"iMaxPopulation");
	pXML->GetOptionalChildXmlValByName(&m_iMinMapLandmass, L"iMinMapLandmass");
	pXML->GetOptionalChildXmlValByName(&m_iMinOurLandmass, L"iMinOurLandmass");
	pXML->GetOptionalChildXmlValByName(&m_iMaxOurLandmass, L"iMaxOurLandmass", -1);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MinDifficulty");
	m_iMinDifficulty = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iAngry, L"iAngry");
	pXML->GetOptionalChildXmlValByName(&m_iUnhealthy, L"iUnhealthy");
	pXML->GetOptionalChildXmlValByName(&m_iUnitDamagedWeight, L"iUnitDamagedWeight");
	pXML->GetOptionalChildXmlValByName(&m_iUnitDistanceWeight, L"iUnitDistanceWeight");
	pXML->GetOptionalChildXmlValByName(&m_iUnitExperienceWeight, L"iUnitExperienceWeight");
	pXML->GetOptionalChildXmlValByName(&m_iMinTreasury, L"iMinTreasury");
	pXML->SetOptionalVector(&m_aiUnitsRequired, L"UnitsRequired");
	pXML->SetOptionalVector(&m_aiBuildingsRequired, L"BuildingsRequired");
	pXML->SetOptionalVector(&m_aiPrereqOrTechs, L"OrPreReqs");
	pXML->SetOptionalVector(&m_aiPrereqAndTechs, L"AndPreReqs");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"OtherPlayerHasTech");
	m_iOtherPlayerHasTech = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Civic");
	m_iCivic = pXML->GetInfoClass(szTextVal);

	pXML->SetOptionalVector(&m_aiObsoleteTechs, L"ObsoleteTechs");
	pXML->SetOptionalVector(&m_aiEvents, L"Events");
	pXML->SetOptionalVector(&m_aiPrereqEvents, L"PrereqEvents");
	pXML->SetOptionalVector(&m_aiFeaturesRequired, L"FeaturesRequired");
	pXML->SetOptionalVector(&m_aiTerrainsRequired, L"TerrainsRequired");
	pXML->SetOptionalVector(&m_aiImprovementsRequired, L"ImprovementsRequired");
	pXML->SetOptionalVector(&m_aiBonusesRequired, L"BonusesRequired");
	pXML->SetOptionalVector(&m_aiRoutesRequired, L"RoutesRequired");
	pXML->SetOptionalVector(&m_aiReligionsRequired, L"ReligionsRequired");
	pXML->SetOptionalVector(&m_aiCorporationsRequired, L"CorporationsRequired");

	m_aszText.clear();
	m_aiTextEra.clear();
	if (pXML->TryMoveToXmlFirstChild(L"TriggerTexts"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszText.push_back(szTextVal);
						pXML->GetNextXmlVal(szTextVal);
						m_aiTextEra.push_back(pXML->GetInfoClass(szTextVal));

						pXML->MoveToXmlParent();

					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}

				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	m_aszWorldNews.clear();
	if (pXML->TryMoveToXmlFirstChild(L"WorldNewsTexts"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->GetChildXmlVal(szTextVal))
			{
				for (int j=0; j<iNumSibs; ++j)
				{
					m_aszWorldNews.push_back(szTextVal);
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

// Begin EmperorFool: Events with Images
	pXML->GetOptionalChildXmlValByName(m_szEventArt, L"EventArt");
// End EmperorFool: Events with Images
	pXML->GetOptionalChildXmlValByName(&m_bSinglePlayer, L"bSinglePlayer");
	pXML->GetOptionalChildXmlValByName(&m_bTeam, L"bTeam");
	pXML->GetOptionalChildXmlValByName(&m_bRecurring, L"bRecurring");
	pXML->GetOptionalChildXmlValByName(&m_bGlobal, L"bGlobal");
	pXML->GetOptionalChildXmlValByName(&m_bPickPlayer, L"bPickPlayer");
	pXML->GetOptionalChildXmlValByName(&m_bOtherPlayerWar, L"bOtherPlayerWar");
	pXML->GetOptionalChildXmlValByName(&m_bOtherPlayerHasReligion, L"bOtherPlayerHasReligion");
	pXML->GetOptionalChildXmlValByName(&m_bOtherPlayerHasOtherReligion, L"bOtherPlayerHasOtherReligion");
	pXML->GetOptionalChildXmlValByName(&m_bOtherPlayerAI, L"bOtherPlayerAI");
	pXML->GetOptionalChildXmlValByName(&m_bPickCity, L"bPickCity");
	pXML->GetOptionalChildXmlValByName(&m_bPickOtherPlayerCity, L"bPickOtherPlayerCity");
	pXML->GetOptionalChildXmlValByName(&m_bShowPlot, L"bShowPlot", true);
	pXML->GetOptionalChildXmlValByName(&m_iCityFoodWeight, L"iCityFoodWeight");
	pXML->GetOptionalChildXmlValByName(&m_bUnitsOnPlot, L"bUnitsOnPlot");
	pXML->GetOptionalChildXmlValByName(&m_bOwnPlot, L"bOwnPlot");
	pXML->GetOptionalChildXmlValByName(&m_bPickReligion, L"bPickReligion");
	pXML->GetOptionalChildXmlValByName(&m_bStateReligion, L"bStateReligion");
	pXML->GetOptionalChildXmlValByName(&m_bHolyCity, L"bHolyCity");
	pXML->GetOptionalChildXmlValByName(&m_bPickCorporation, L"bPickCorporation");
	pXML->GetOptionalChildXmlValByName(&m_bHeadquarters, L"bHeadquarters");
	pXML->GetOptionalChildXmlValByName(&m_bProbabilityUnitMultiply, L"bProbabilityUnitMultiply");
	pXML->GetOptionalChildXmlValByName(&m_bProbabilityBuildingMultiply, L"bProbabilityBuildingMultiply");
	pXML->GetOptionalChildXmlValByName(&m_bPrereqEventCity, L"bPrereqEventPlot");

	m_PrereqMinProperties.read(pXML, L"PrereqMinProperties");
	m_PrereqMaxProperties.read(pXML, L"PrereqMaxProperties");
	m_PrereqPlayerMinProperties.read(pXML, L"PrereqPlayerMinProperties");
	m_PrereqPlayerMaxProperties.read(pXML, L"PrereqPlayerMaxProperties");

	pXML->GetOptionalChildXmlValByName(m_szPythonCallback, L"PythonCallback");
	pXML->GetOptionalChildXmlValByName(m_szPythonCanDo, L"PythonCanDo");
	pXML->GetOptionalChildXmlValByName(m_szPythonCanDoCity, L"PythonCanDoCity");
	pXML->GetOptionalChildXmlValByName(m_szPythonCanDoUnit, L"PythonCanDoUnit");

	// bool vector without delayed resolution
	pXML->SetOptionalVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	return true;
}


void CvEventTriggerInfo::copyNonDefaults(const CvEventTriggerInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPercentGamesActive() == iDefault) m_iPercentGamesActive = pClassInfo->getPercentGamesActive();
	if (getProbability() == iDefault) m_iProbability = pClassInfo->getProbability();
	if (getNumUnits() == iDefault) m_iNumUnits = pClassInfo->getNumUnits();
	if (getNumBuildings() == iDefault) m_iNumBuildings = pClassInfo->getNumBuildings();
	if (getNumUnitsGlobal() == iDefault) m_iNumUnitsGlobal = pClassInfo->getNumUnitsGlobal();
	if (getNumBuildingsGlobal() == iDefault) m_iNumBuildingsGlobal = pClassInfo->getNumBuildingsGlobal();
	if (getNumPlotsRequired() == iDefault) m_iNumPlotsRequired = pClassInfo->getNumPlotsRequired();
	if (getPlotType() == NO_PLOT) m_iPlotType = pClassInfo->getPlotType();
	if (getNumReligions() == iDefault) m_iNumReligions = pClassInfo->getNumReligions();
	if (getNumCorporations() == iDefault) m_iNumCorporations = pClassInfo->getNumCorporations();
	if (getOtherPlayerShareBorders() == iDefault) m_iOtherPlayerShareBorders = pClassInfo->getOtherPlayerShareBorders();
	if (getMinPopulation() == iDefault) m_iMinPopulation = pClassInfo->getMinPopulation();
	if (getMaxPopulation() == iDefault) m_iMaxPopulation = pClassInfo->getMaxPopulation();
	if (getMinMapLandmass() == iDefault) m_iMinMapLandmass = pClassInfo->getMinMapLandmass();
	if (getMinOurLandmass() == iDefault) m_iMinOurLandmass = pClassInfo->getMinOurLandmass();
	if (getMaxOurLandmass() == -1) m_iMaxOurLandmass = pClassInfo->getMaxOurLandmass();
	if (getMinDifficulty() == iTextDefault) m_iMinDifficulty = pClassInfo->getMinDifficulty();
	if (getAngry() == iDefault) m_iAngry = pClassInfo->getAngry();
	if (getUnhealthy() == iDefault) m_iUnhealthy = pClassInfo->getUnhealthy();
	if (getUnitDamagedWeight() == iDefault) m_iUnitDamagedWeight = pClassInfo->getUnitDamagedWeight();
	if (getUnitDistanceWeight() == iDefault) m_iUnitDistanceWeight = pClassInfo->getUnitDistanceWeight();
	if (getUnitExperienceWeight() == iDefault) m_iUnitExperienceWeight = pClassInfo->getUnitExperienceWeight();
	if (getMinTreasury() == iDefault) m_iMinTreasury = pClassInfo->getMinTreasury();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiUnitsRequired, pClassInfo->m_aiUnitsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiBuildingsRequired, pClassInfo->m_aiBuildingsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqOrTechs, pClassInfo->m_aiPrereqOrTechs);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqAndTechs, pClassInfo->m_aiPrereqAndTechs);

	if (getOtherPlayerHasTech() == iTextDefault) m_iOtherPlayerHasTech = pClassInfo->getOtherPlayerHasTech();
	if (getCivic() == iTextDefault) m_iCivic = pClassInfo->getCivic();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiObsoleteTechs, pClassInfo->m_aiObsoleteTechs);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiEvents, pClassInfo->m_aiEvents);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqEvents, pClassInfo->m_aiPrereqEvents);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiFeaturesRequired, pClassInfo->m_aiFeaturesRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTerrainsRequired, pClassInfo->m_aiTerrainsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiImprovementsRequired, pClassInfo->m_aiImprovementsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiBonusesRequired, pClassInfo->m_aiBonusesRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiRoutesRequired, pClassInfo->m_aiRoutesRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiReligionsRequired, pClassInfo->m_aiReligionsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCorporationsRequired, pClassInfo->m_aiCorporationsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTextEra, pClassInfo->m_aiTextEra);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aszText, pClassInfo->m_aszText);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aszWorldNews, pClassInfo->m_aszWorldNews);

	if (isSinglePlayer() == bDefault) m_bSinglePlayer = pClassInfo->isSinglePlayer();
	if (isTeam() == bDefault) m_bTeam = pClassInfo->isTeam();
	if (isRecurring() == bDefault) m_bRecurring = pClassInfo->isRecurring();
	if (isGlobal() == bDefault) m_bGlobal = pClassInfo->isGlobal();
	if (isPickPlayer() == bDefault) m_bPickPlayer = pClassInfo->isPickPlayer();
	if (isOtherPlayerWar() == bDefault) m_bOtherPlayerWar = pClassInfo->isOtherPlayerWar();
	if (isOtherPlayerHasReligion() == bDefault) m_bOtherPlayerHasReligion = pClassInfo->isOtherPlayerHasReligion();
	if (isOtherPlayerHasOtherReligion() == bDefault) m_bOtherPlayerHasOtherReligion = pClassInfo->isOtherPlayerHasOtherReligion();
	if (isOtherPlayerAI() == bDefault) m_bOtherPlayerAI = pClassInfo->isOtherPlayerAI();
	if (isPickCity() == bDefault) m_bPickCity = pClassInfo->isPickCity();
	if (isPickOtherPlayerCity() == bDefault) m_bPickOtherPlayerCity = pClassInfo->isPickOtherPlayerCity();
	if (isShowPlot()) m_bShowPlot = pClassInfo->isShowPlot();
	if (getCityFoodWeight() == iDefault) m_iCityFoodWeight = pClassInfo->getCityFoodWeight();
	if (isUnitsOnPlot() == bDefault) m_bUnitsOnPlot = pClassInfo->isUnitsOnPlot();
	if (isOwnPlot() == bDefault) m_bOwnPlot = pClassInfo->isOwnPlot();
	if (isPickReligion() == bDefault) m_bPickReligion = pClassInfo->isPickReligion();
	if (isStateReligion() == bDefault) m_bStateReligion = pClassInfo->isStateReligion();
	if (isHolyCity() == bDefault) m_bHolyCity = pClassInfo->isHolyCity();
	if (isPickCorporation() == bDefault) m_bPickCorporation = pClassInfo->isPickCorporation();
	if (isHeadquarters() == bDefault) m_bHeadquarters = pClassInfo->isHeadquarters();
	if (isProbabilityUnitMultiply() == bDefault) m_bProbabilityUnitMultiply = pClassInfo->isProbabilityUnitMultiply();
	if (isProbabilityBuildingMultiply() == bDefault) m_bProbabilityBuildingMultiply = pClassInfo->isProbabilityBuildingMultiply();
	if (isPrereqEventCity() == bDefault) m_bPrereqEventCity = pClassInfo->isPrereqEventCity();

	m_PrereqMinProperties.copyNonDefaults(pClassInfo->getPrereqMinProperties());
	m_PrereqMaxProperties.copyNonDefaults(pClassInfo->getPrereqMaxProperties());
	m_PrereqPlayerMinProperties.copyNonDefaults(pClassInfo->getPrereqPlayerMinProperties());
	m_PrereqPlayerMaxProperties.copyNonDefaults(pClassInfo->getPrereqPlayerMaxProperties());

	if (getPythonCallback() == cDefault) m_szPythonCallback = pClassInfo->getPythonCallback();
	if (getPythonCanDo() == cDefault) m_szPythonCanDo = pClassInfo->getPythonCanDo();
	if (getPythonCanDoCity() == cDefault) m_szPythonCanDoCity = pClassInfo->getPythonCanDoCity();
	if (getPythonCanDoUnit() == cDefault) m_szPythonCanDoUnit = pClassInfo->getPythonCanDoUnit();
	// bool vector without delayed resolution
	if (getNumNotOnGameOptions() == 0)
	{
		m_aiNotOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnGameOptions(); i++)
		{
			m_aiNotOnGameOptions.push_back(pClassInfo->getNotOnGameOption(i));
		}
	}
	if (getNumOnGameOptions() == 0)
	{
		m_aiOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumOnGameOptions(); i++)
		{
			m_aiOnGameOptions.push_back(pClassInfo->getOnGameOption(i));
		}
	}
}

