//------------------------------------------------------------------------------------------------
//  FILE:    CvEventInfo.cpp
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
#include "CvEventInfo.h"



//////////////////////////////////////////////////////////////////////////
//
//	CvEventInfo
//	Events
//
//
CvEventInfo::CvEventInfo() :
	m_bQuest(false),
	m_bGlobal(false),
	m_bTeam(false),
	m_bCityEffect(false),
	m_bOtherPlayerCityEffect(false),
	m_bGoldToPlayer(false),
	m_bGoldenAge(false),
	m_bDeclareWar(false),
	m_bDisbandUnit(false),
	m_bGameSpeedScale(false),
	m_iGold(0),
	m_iRandomGold(0),
	m_iCulture(0),
	m_iEspionagePoints(0),
	m_iTech(NO_TECH),
	m_iTechPercent(0),
	m_iTechCostPercent(0),
	m_iTechMinTurnsLeft(0),
	m_iPrereqTech(NO_TECH),
	m_iFreeUnit(NO_UNIT),
	m_iNumUnits(0),
	m_iUnitExperience(0),
	m_iUnitImmobileTurns(0),
	m_iBuilding(NO_BUILDING),
	m_iBuildingChange(0),
	m_iHappy(0),
	m_iHealth(0),
	m_iHurryAnger(0),
	m_iHappyTurns(0),
	m_iFood(0),
	m_iFoodPercent(0),
	m_iFeature(NO_FEATURE),
	m_iFeatureChange(0),
	m_iImprovement(NO_IMPROVEMENT),
	m_iImprovementChange(0),
	m_iBonus(NO_BONUS),
	m_iBonusChange(0),
	m_iRoute(NO_ROUTE),
	m_iRouteChange(0),
	m_iBonusRevealed(NO_BONUS),
	m_iBonusGift(NO_BONUS),
	m_iConvertOwnCities(0),
	m_iConvertOtherCities(0),
	m_iMaxNumReligions(-1),
	m_iOurAttitudeModifier(0),
	m_iAttitudeModifier(0),
	m_iTheirEnemyAttitudeModifier(0),
	m_iPopulationChange(0),
	m_iRevoltTurns(0),
	m_iMinPillage(0),
	m_iMaxPillage(0),
	m_iFreeUnitSupport(0),
	m_iInflationModifier(0),
	m_iSpaceProductionModifier(0),
	m_iAIValue(0),
	m_piTechFlavorValue(NULL),
	m_piPlotExtraYields(NULL),
	m_piFreeSpecialistCount(NULL),
	m_piAdditionalEventChance(NULL),
	m_piAdditionalEventTime(NULL),
	m_piClearEventChance(NULL),
	m_piUnitCombatPromotions(NULL),
	m_piUnitPromotions(NULL),
	m_piCommerceModifier(NULL),
	m_piYieldModifier(NULL),
	m_iPrereqGameOption(NO_GAMEOPTION),
	m_iRevolutionIndexChange(0)
{ }


CvEventInfo::~CvEventInfo()
{
	SAFE_DELETE_ARRAY(m_piTechFlavorValue);
	SAFE_DELETE_ARRAY(m_piPlotExtraYields);
	SAFE_DELETE_ARRAY(m_piFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_piAdditionalEventChance);
	SAFE_DELETE_ARRAY(m_piAdditionalEventTime);
	SAFE_DELETE_ARRAY(m_piClearEventChance);
	SAFE_DELETE_ARRAY(m_piUnitCombatPromotions);
	SAFE_DELETE_ARRAY(m_piUnitPromotions);
	SAFE_DELETE_ARRAY(m_piCommerceModifier);
	SAFE_DELETE_ARRAY(m_piYieldModifier);
}


bool CvEventInfo::isGlobal() const
{
	return m_bGlobal;
}


bool CvEventInfo::isQuest() const
{
	return m_bQuest;
}


bool CvEventInfo::isTeam() const
{
	return m_bTeam;
}


bool CvEventInfo::isCityEffect() const
{
	return m_bCityEffect;
}


bool CvEventInfo::isOtherPlayerCityEffect() const
{
	return m_bOtherPlayerCityEffect;
}


bool CvEventInfo::isGoldToPlayer() const
{
	return m_bGoldToPlayer;
}


bool CvEventInfo::isGoldenAge() const
{
	return m_bGoldenAge;
}


bool CvEventInfo::isDeclareWar() const
{
	return m_bDeclareWar;
}


bool CvEventInfo::isDisbandUnit() const
{
	return m_bDisbandUnit;
}


int CvEventInfo::getGold() const
{
	return m_iGold;
}


int CvEventInfo::getRandomGold() const
{
	return m_iRandomGold;
}


int CvEventInfo::getCulture() const
{
	return m_iCulture;
}


int CvEventInfo::getEspionagePoints() const
{
	return m_iEspionagePoints;
}


int CvEventInfo::getTech() const
{
	return m_iTech;
}


int CvEventInfo::getTechPercent() const
{
	return m_iTechPercent;
}


int CvEventInfo::getTechCostPercent() const
{
	return m_iTechCostPercent;
}


int CvEventInfo::getTechMinTurnsLeft() const
{
	return m_iTechMinTurnsLeft;
}


int CvEventInfo::getFreeUnit() const
{
	return m_iFreeUnit;
}


int CvEventInfo::getNumUnits() const
{
	return m_iNumUnits;
}


int CvEventInfo::getUnitExperience() const
{
	return m_iUnitExperience;
}


int CvEventInfo::getUnitImmobileTurns() const
{
	return m_iUnitImmobileTurns;
}


int CvEventInfo::getBuilding() const
{
	return m_iBuilding;
}


int CvEventInfo::getBuildingChange() const
{
	return m_iBuildingChange;
}


int CvEventInfo::getCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}


int CvEventInfo::getYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldModifier ? m_piYieldModifier[i] : 0;
}


int CvEventInfo::getHappy() const
{
	return m_iHappy;
}


int CvEventInfo::getHealth() const
{
	return m_iHealth;
}


int CvEventInfo::getHurryAnger() const
{
	return m_iHurryAnger;
}


int CvEventInfo::getHappyTurns() const
{
	return m_iHappyTurns;
}


int CvEventInfo::getFood() const
{
	return m_iFood;
}


int CvEventInfo::getFoodPercent() const
{
	return m_iFoodPercent;
}


int CvEventInfo::getFeature() const
{
	return m_iFeature;
}


int CvEventInfo::getFeatureChange() const
{
	return m_iFeatureChange;
}


int CvEventInfo::getImprovementChange() const
{
	return m_iImprovementChange;
}


int CvEventInfo::getBonus() const
{
	return m_iBonus;
}


int CvEventInfo::getBonusChange() const
{
	return m_iBonusChange;
}


int CvEventInfo::getRoute() const
{
	return m_iRoute;
}


int CvEventInfo::getRouteChange() const
{
	return m_iRouteChange;
}


int CvEventInfo::getBonusRevealed() const
{
	return m_iBonusRevealed;
}


int CvEventInfo::getBonusGift() const
{
	return m_iBonusGift;
}


int CvEventInfo::getConvertOwnCities() const
{
	return m_iConvertOwnCities;
}


int CvEventInfo::getConvertOtherCities() const
{
	return m_iConvertOtherCities;
}


int CvEventInfo::getMaxNumReligions() const
{
	return m_iMaxNumReligions;
}


int CvEventInfo::getOurAttitudeModifier() const
{
	return m_iOurAttitudeModifier;
}


int CvEventInfo::getAttitudeModifier() const
{
	return m_iAttitudeModifier;
}


int CvEventInfo::getTheirEnemyAttitudeModifier() const
{
	return m_iTheirEnemyAttitudeModifier;
}


int CvEventInfo::getPopulationChange() const
{
	return m_iPopulationChange;
}


int CvEventInfo::getRevoltTurns() const
{
	return m_iRevoltTurns;
}


int CvEventInfo::getMinPillage() const
{
	return m_iMinPillage;
}


int CvEventInfo::getMaxPillage() const
{
	return m_iMaxPillage;
}


int CvEventInfo::getFreeUnitSupport() const
{
	return m_iFreeUnitSupport;
}


int CvEventInfo::getInflationModifier() const
{
	return m_iInflationModifier;
}


int CvEventInfo::getSpaceProductionModifier() const
{
	return m_iSpaceProductionModifier;
}


int CvEventInfo::getAIValue() const
{
	return m_iAIValue;
}


int CvEventInfo::getAdditionalEventChance(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), i);
	return m_piAdditionalEventChance ? m_piAdditionalEventChance[i] : 0;
}


int CvEventInfo::getAdditionalEventTime(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), i);
	return m_piAdditionalEventTime ? m_piAdditionalEventTime[i] : 0;
}


int CvEventInfo::getClearEventChance(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), i);
	return m_piClearEventChance ? m_piClearEventChance[i] : 0;
}


int CvEventInfo::getTechFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piTechFlavorValue ? m_piTechFlavorValue[i] : 0;
}


int CvEventInfo::getPlotExtraYield(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piPlotExtraYields ? m_piPlotExtraYields[i] : 0;
}


int CvEventInfo::getFreeSpecialistCount(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	return m_piFreeSpecialistCount ? m_piFreeSpecialistCount[i] : 0;
}


int CvEventInfo::getUnitCombatPromotion(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return m_piUnitCombatPromotions ? m_piUnitCombatPromotions[i] : -1;
}


int CvEventInfo::getUnitPromotion(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), i);
	return m_piUnitPromotions ? m_piUnitPromotions[i] : -1;
}


const CvWString& CvEventInfo::getWorldNews(int i) const
{
	FASSERT_BOUNDS(0, getNumWorldNews(), i);
	return m_aszWorldNews[i];
}


int CvEventInfo::getNumWorldNews() const
{
	return m_aszWorldNews.size();
}


int CvEventInfo::getBuildingYieldChange(int iBuilding, int iYield) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingYieldChange& it, m_aBuildingYieldChanges)
	{
		if (it.eBuilding == (BuildingTypes)iBuilding && it.eYield == (YieldTypes)iYield)
		{
			return it.iChange;
		}
	}

	return 0;
}


int CvEventInfo::getNumBuildingYieldChanges() const
{
	return m_aBuildingYieldChanges.size();
}


int CvEventInfo::getBuildingCommerceChange(int iBuilding, int iCommerce) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceChanges)
	{
		if (it.eBuilding == (BuildingTypes)iBuilding && it.eCommerce == (CommerceTypes)iCommerce)
		{
			return it.iChange;
		}
	}

	return 0;
}


int CvEventInfo::getNumBuildingCommerceChanges() const
{
	return m_aBuildingCommerceChanges.size();
}


int CvEventInfo::getBuildingCommerceModifier(int iBuilding, int iCommerce) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceModifiers)
	{
		if (it.eBuilding == (BuildingTypes)iBuilding && it.eCommerce == (CommerceTypes)iCommerce)
		{
			return it.iChange;
		}
	}

	return 0;
}


int CvEventInfo::getNumBuildingCommerceModifiers() const
{
	return m_aBuildingCommerceModifiers.size();
}


int CvEventInfo::getNumBuildingHappyChanges() const
{
	return m_aBuildingHappyChanges.size();
}


int CvEventInfo::getBuildingHappyChange(int iBuilding) const
{
	PROFILE_EXTRA_FUNC();
	for (BuildingChangeArray::const_iterator it = m_aBuildingHappyChanges.begin(); it != m_aBuildingHappyChanges.end(); ++it)
	{
		if ((*it).first == (BuildingTypes)iBuilding)
		{
			return (*it).second;
		}
	}

	return 0;
}


int CvEventInfo::getNumBuildingHealthChanges() const
{
	return m_aBuildingHealthChanges.size();
}


// Direct sparse-iteration accessors. See header for the rationale.
const BuildingChangeArray& CvEventInfo::getBuildingHappyChanges() const
{
	return m_aBuildingHappyChanges;
}

const BuildingChangeArray& CvEventInfo::getBuildingHealthChanges() const
{
	return m_aBuildingHealthChanges;
}

const std::vector<BuildingYieldChange>& CvEventInfo::getBuildingYieldChanges() const
{
	return m_aBuildingYieldChanges;
}

const std::vector<BuildingCommerceChange>& CvEventInfo::getBuildingCommerceChanges() const
{
	return m_aBuildingCommerceChanges;
}

const std::vector<BuildingCommerceChange>& CvEventInfo::getBuildingCommerceModifiers() const
{
	return m_aBuildingCommerceModifiers;
}


int CvEventInfo::getBuildingHealthChange(int iBuilding) const
{
	PROFILE_EXTRA_FUNC();
	for (BuildingChangeArray::const_iterator it = m_aBuildingHealthChanges.begin(); it != m_aBuildingHealthChanges.end(); ++it)
	{
		if ((*it).first == (BuildingTypes)iBuilding)
		{
			return (*it).second;
		}
	}

	return 0;
}


const CvProperties* CvEventInfo::getProperties() const
{
	return &m_Properties;
}


const CvProperties* CvEventInfo::getPropertiesAllCities() const
{
	return &m_PropertiesAllCities;
}


const char* CvEventInfo::getPythonCallback() const
{
	return m_szPythonCallback;
}


const char* CvEventInfo::getPythonExpireCheck() const
{
	return m_szPythonExpireCheck;
}


const char* CvEventInfo::getPythonCanDo() const
{
	return m_szPythonCanDo;
}


const char* CvEventInfo::getPythonHelp() const
{
	return m_szPythonHelp;
}


const wchar_t* CvEventInfo::getUnitNameKey() const
{
	return m_szUnitName;
}


const wchar_t* CvEventInfo::getQuestFailTextKey() const
{
	return m_szQuestFailText;
}


const wchar_t* CvEventInfo::getLocalInfoTextKey() const
{
	return m_szLocalInfoText;
}


const wchar_t* CvEventInfo::getOtherPlayerPopup() const
{
	return m_szOtherPlayerPopup;
}


int CvEventInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}

int CvEventInfo::getRevolutionIndexChange() const
{
	return m_iRevolutionIndexChange;
}


int CvEventInfo::getAdditionalEventChanceVectorSize() const						{ return m_aszAdditionalEventChanceforPass3.size(); }

CvString CvEventInfo::getAdditionalEventChanceNamesVectorElement(int i) const	{ return m_aszAdditionalEventChanceforPass3[i]; }

int CvEventInfo::getAdditionalEventChanceValuesVectorElement(int i) const		{ return m_aiAdditionalEventChanceforPass3[i]; }


int CvEventInfo::getAdditionalEventTimeVectorSize() const					{ return m_aszAdditionalEventTimeforPass3.size(); }

CvString CvEventInfo::getAdditionalEventTimeNamesVectorElement(int i) const	{ return m_aszAdditionalEventTimeforPass3[i]; }

int CvEventInfo::getAdditionalEventTimeValuesVectorElement(int i) const		{ return m_aiAdditionalEventTimeforPass3[i]; }


int CvEventInfo::getClearEventChanceVectorSize() const						{ return m_aszClearEventChanceforPass3.size(); }

CvString CvEventInfo::getClearEventChanceNamesVectorElement(int i) const	{ return m_aszClearEventChanceforPass3[i]; }

int CvEventInfo::getClearEventChanceValuesVectorElement(int i) const		{ return m_aiClearEventChanceforPass3[i]; }



void CvEventInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_bQuest);
	CheckSum(iSum, m_bGlobal);
	CheckSum(iSum, m_bTeam);
	CheckSum(iSum, m_bCityEffect);
	CheckSum(iSum, m_bOtherPlayerCityEffect);
	CheckSum(iSum, m_bGoldToPlayer);
	CheckSum(iSum, m_bGoldenAge);
	CheckSum(iSum, m_bDeclareWar);
	CheckSum(iSum, m_bDisbandUnit);
	CheckSum(iSum, m_bGameSpeedScale);
	CheckSum(iSum, m_iGold);
	CheckSum(iSum, m_iRandomGold);
	CheckSum(iSum, m_iCulture);
	CheckSum(iSum, m_iEspionagePoints);
	CheckSum(iSum, m_iTech);
	CheckSum(iSum, m_iTechPercent);
	CheckSum(iSum, m_iTechCostPercent);
	CheckSum(iSum, m_iTechMinTurnsLeft);
	CheckSum(iSum, m_iPrereqTech);
	CheckSum(iSum, m_iFreeUnit);
	CheckSum(iSum, m_iNumUnits);
	CheckSum(iSum, m_iUnitExperience);
	CheckSum(iSum, m_iUnitImmobileTurns);
	CheckSum(iSum, m_iBuilding);
	CheckSum(iSum, m_iBuildingChange);
	CheckSum(iSum, m_iHappy);
	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iHurryAnger);
	CheckSum(iSum, m_iHappyTurns);
	CheckSum(iSum, m_iFood);
	CheckSum(iSum, m_iFoodPercent);
	CheckSum(iSum, m_iFeature);
	CheckSum(iSum, m_iFeatureChange);
	CheckSum(iSum, m_iImprovement);
	CheckSum(iSum, m_iImprovementChange);
	CheckSum(iSum, m_iBonus);
	CheckSum(iSum, m_iBonusChange);
	CheckSum(iSum, m_iRoute);
	CheckSum(iSum, m_iRouteChange);
	CheckSum(iSum, m_iBonusRevealed);
	CheckSum(iSum, m_iBonusGift);
	CheckSum(iSum, m_iConvertOwnCities);
	CheckSum(iSum, m_iConvertOtherCities);
	CheckSum(iSum, m_iMaxNumReligions);
	CheckSum(iSum, m_iOurAttitudeModifier);
	CheckSum(iSum, m_iAttitudeModifier);
	CheckSum(iSum, m_iTheirEnemyAttitudeModifier);
	CheckSum(iSum, m_iPopulationChange);
	CheckSum(iSum, m_iRevoltTurns);
	CheckSum(iSum, m_iMinPillage);
	CheckSum(iSum, m_iMaxPillage);
	CheckSum(iSum, m_iFreeUnitSupport);
	CheckSum(iSum, m_iInflationModifier);
	CheckSum(iSum, m_iSpaceProductionModifier);
	CheckSum(iSum, m_iAIValue);

	CheckSumI(iSum, GC.getNumFlavorTypes(), m_piTechFlavorValue);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piPlotExtraYields);
	CheckSumI(iSum, GC.getNumSpecialistInfos(), m_piFreeSpecialistCount);
	CheckSumI(iSum, GC.getNumEventInfos(), m_piAdditionalEventChance);
	CheckSumI(iSum, GC.getNumEventInfos(), m_piAdditionalEventTime);
	CheckSumI(iSum, GC.getNumEventInfos(), m_piClearEventChance);
	CheckSumI(iSum, GC.getNumUnitCombatInfos(), m_piUnitCombatPromotions);
	CheckSumI(iSum, GC.getNumUnitInfos(), m_piUnitPromotions);

	foreach_(const BuildingYieldChange& it, m_aBuildingYieldChanges)
	{
		CheckSum(iSum, it.eBuilding);
		CheckSum(iSum, it.eYield);
		CheckSum(iSum, it.iChange);
	}

	foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceChanges)
	{
		CheckSum(iSum, it.eBuilding);
		CheckSum(iSum, it.eCommerce);
		CheckSum(iSum, it.iChange);
	}

	foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceModifiers)
	{
		CheckSum(iSum, it.eBuilding);
		CheckSum(iSum, it.eCommerce);
		CheckSum(iSum, it.iChange);
	}

	CheckSumC(iSum, m_aBuildingHappyChanges);
	CheckSumC(iSum, m_aBuildingHealthChanges);

	//CheckSumC(iSum, m_szUnitName);
	//CheckSumC(iSum, m_szOtherPlayerPopup);

	m_Properties.getCheckSum(iSum);
	m_PropertiesAllCities.getCheckSum(iSum);

	CheckSumC(iSum, m_szPythonCallback);
	CheckSumC(iSum, m_szPythonExpireCheck);
	CheckSumC(iSum, m_szPythonCanDo);
	CheckSumC(iSum, m_szPythonHelp);

	CheckSum(iSum, m_iPrereqGameOption);
	CheckSum(iSum, m_iRevolutionIndexChange);
}


bool CvEventInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTextVal;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bQuest, L"bQuest");
	pXML->GetOptionalChildXmlValByName(&m_bGlobal, L"bGlobal");
	pXML->GetOptionalChildXmlValByName(&m_bTeam, L"bTeam");
	pXML->GetOptionalChildXmlValByName(&m_bCityEffect, L"bPickCity");
	pXML->GetOptionalChildXmlValByName(&m_bOtherPlayerCityEffect, L"bPickOtherPlayerCity");
	pXML->GetOptionalChildXmlValByName(&m_bGoldToPlayer, L"bGoldToPlayer");
	pXML->GetOptionalChildXmlValByName(&m_bGoldenAge, L"bGoldenAge");
	pXML->GetOptionalChildXmlValByName(&m_bDeclareWar, L"bDeclareWar");
	pXML->GetOptionalChildXmlValByName(&m_iGold, L"iGold");
	pXML->GetOptionalChildXmlValByName(&m_iRandomGold, L"iRandomGold");
	pXML->GetOptionalChildXmlValByName(&m_iCulture, L"iCulture");
	pXML->GetOptionalChildXmlValByName(&m_iEspionagePoints, L"iEspionagePoints");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"Tech");
	m_iTech = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iTechPercent, L"iTechPercent");
	pXML->GetOptionalChildXmlValByName(&m_iTechCostPercent, L"iTechCostPercent");
	pXML->GetOptionalChildXmlValByName(&m_iTechMinTurnsLeft, L"iTechMinTurnsLeft");
	pXML->GetOptionalTypeEnum(m_iPrereqTech, L"PrereqTech");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeUnit");
	m_iFreeUnit = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iNumUnits, L"iNumFreeUnits");
	pXML->GetOptionalChildXmlValByName(&m_bDisbandUnit, L"bDisbandUnit");
	pXML->GetOptionalChildXmlValByName(&m_bGameSpeedScale, L"bGameSpeedScale");
	pXML->GetOptionalChildXmlValByName(&m_iUnitExperience, L"iUnitExperience");
	pXML->GetOptionalChildXmlValByName(&m_iUnitImmobileTurns, L"iUnitImmobileTurns");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"Building");
	m_iBuilding = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iBuildingChange, L"iBuildingChange");

	pXML->GetOptionalChildXmlValByName(&m_iHappy, L"iHappy");
	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHurryAnger, L"iHurryAnger");
	pXML->GetOptionalChildXmlValByName(&m_iHappyTurns, L"iHappyTurns");
	pXML->GetOptionalChildXmlValByName(&m_iFood, L"iFood");
	pXML->GetOptionalChildXmlValByName(&m_iFoodPercent, L"iFoodPercent");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FeatureType");
	m_iFeature = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iFeatureChange, L"iFeatureChange");

	pXML->GetOptionalTypeEnum(m_iImprovement, L"ImprovementType");
	pXML->GetOptionalChildXmlValByName(&m_iImprovementChange, L"iImprovementChange");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusType");
	m_iBonus = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iBonusChange, L"iBonusChange");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"RouteType");
	m_iRoute = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iRouteChange, L"iRouteChange");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusRevealed");
	m_iBonusRevealed = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusGift");
	m_iBonusGift = pXML->GetInfoClass(szTextVal);

	pXML->SetVariableListTagPair(&m_piTechFlavorValue, L"TechFlavors", GC.getNumFlavorTypes());
	pXML->SetVariableListTagPair(&m_piPlotExtraYields, L"PlotExtraYields", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_piFreeSpecialistCount, L"FreeSpecialistCounts", GC.getNumSpecialistInfos());

	pXML->GetOptionalChildXmlValByName(&m_iConvertOwnCities, L"iConvertOwnCities");
	pXML->GetOptionalChildXmlValByName(&m_iConvertOtherCities, L"iConvertOtherCities");
	pXML->GetOptionalChildXmlValByName(&m_iMaxNumReligions, L"iMaxNumReligions", -1);
	pXML->GetOptionalChildXmlValByName(&m_iOurAttitudeModifier, L"iOurAttitudeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAttitudeModifier, L"iAttitudeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTheirEnemyAttitudeModifier, L"iTheirEnemyAttitudeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationChange, L"iPopulationChange");
	pXML->GetOptionalChildXmlValByName(&m_iRevoltTurns, L"iRevoltTurns");
	pXML->GetOptionalChildXmlValByName(&m_iMinPillage, L"iMinPillage");
	pXML->GetOptionalChildXmlValByName(&m_iMaxPillage, L"iMaxPillage");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitSupport, L"iFreeUnitSupport");
	pXML->GetOptionalChildXmlValByName(&m_iInflationModifier, L"iInflationMod");
	pXML->GetOptionalChildXmlValByName(&m_iSpaceProductionModifier, L"iSpaceProductionMod");
	pXML->GetOptionalChildXmlValByName(&m_iAIValue, L"iAIValue");

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifier);
	}

	CvString* pszPromotions = NULL;
	FAssertMsg(NULL == m_piUnitCombatPromotions, "Memory leak");
	CvXMLLoadUtility::InitList(&m_piUnitCombatPromotions,GC.getNumUnitCombatInfos(),-1);
	pXML->SetVariableListTagPair(&pszPromotions, L"UnitCombatPromotions", GC.getNumUnitCombatInfos(), "NONE");
	if ( pszPromotions != NULL )
	{
		for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
		{
			m_piUnitCombatPromotions[i] = pXML->GetInfoClass(pszPromotions[i]);
		}
		SAFE_DELETE_ARRAY(pszPromotions);
	}

	FAssertMsg(NULL == m_piUnitPromotions, "Memory leak");
	CvXMLLoadUtility::InitList(&m_piUnitPromotions, GC.getNumUnitInfos(), -1);
	pXML->SetVariableListTagPair(&pszPromotions, L"PromoteUnits", GC.getNumUnitInfos(), "NONE");
	if (pszPromotions != NULL)
	{
		for (int i = 0; i < GC.getNumUnitInfos(); ++i)
		{
			m_piUnitPromotions[i] = pXML->GetInfoClass(pszPromotions[i]);
		}
		SAFE_DELETE_ARRAY(pszPromotions);
	}

	pXML->GetOptionalChildXmlValByName(m_szUnitName, L"UnitName");
	pXML->GetOptionalChildXmlValByName(m_szOtherPlayerPopup, L"OtherPlayerPopup");
	pXML->GetOptionalChildXmlValByName(m_szQuestFailText, L"QuestFailText");
	pXML->GetOptionalChildXmlValByName(m_szLocalInfoText, L"LocalInfoText");

	m_Properties.read(pXML);
	m_PropertiesAllCities.read(pXML, L"PropertiesAllCities");

	pXML->GetOptionalChildXmlValByName(m_szPythonCallback, L"PythonCallback");
	pXML->GetOptionalChildXmlValByName(m_szPythonExpireCheck, L"PythonExpireCheck");
	pXML->GetOptionalChildXmlValByName(m_szPythonCanDo, L"PythonCanDo");
	pXML->GetOptionalChildXmlValByName(m_szPythonHelp, L"PythonHelp");

	m_aszWorldNews.clear();
	if (pXML->TryMoveToXmlFirstChild(L"WorldNewsTexts"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

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

	m_aBuildingYieldChanges.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingExtraYields"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingYieldChange kChange;
						kChange.eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(szTextVal);
						kChange.eYield = (YieldTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(&kChange.iChange);
						m_aBuildingYieldChanges.push_back(kChange);

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

	m_aBuildingCommerceChanges.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingExtraCommerces"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingCommerceChange kChange;
						kChange.eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(szTextVal);
						kChange.eCommerce = (CommerceTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(&kChange.iChange);
						m_aBuildingCommerceChanges.push_back(kChange);

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

	m_aBuildingCommerceModifiers.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingCommerceModifiers"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingCommerceChange kChange;
						kChange.eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(szTextVal);
						kChange.eCommerce = (CommerceTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(&kChange.iChange);
						m_aBuildingCommerceModifiers.push_back(kChange);

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

	m_aBuildingHappyChanges.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingExtraHappies"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						const BuildingTypes eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						int iChange;
						pXML->GetNextXmlVal(&iChange);
						m_aBuildingHappyChanges.push_back(std::make_pair(eBuilding, iChange));

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

	m_aBuildingHealthChanges.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingExtraHealths"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						const BuildingTypes eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						int iChange;
						pXML->GetNextXmlVal(&iChange);
						m_aBuildingHealthChanges.push_back(std::make_pair(eBuilding, iChange));

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
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iRevolutionIndexChange, L"iRevolutionIndexChange");

	if (pXML->TryMoveToXmlFirstChild(L"AdditionalEvents"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = 0;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszAdditionalEventChanceforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiAdditionalEventChanceforPass3.push_back(iTemp);
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
	if (pXML->TryMoveToXmlFirstChild(L"EventTimes"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = 0;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszAdditionalEventTimeforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiAdditionalEventTimeforPass3.push_back(iTemp);
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
	if (pXML->TryMoveToXmlFirstChild(L"ClearEvents"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = 0;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszClearEventChanceforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiClearEventChanceforPass3.push_back(iTemp);
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
	return true;
}


void CvEventInfo::copyNonDefaults(const CvEventInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;

	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isQuest() == bDefault) m_bQuest = pClassInfo->isQuest();
	if (isGlobal() == bDefault) m_bGlobal = pClassInfo->isGlobal();
	if (isTeam() == bDefault) m_bTeam = pClassInfo->isTeam();
	if (isCityEffect() == bDefault) m_bCityEffect = pClassInfo->isCityEffect();
	if (isOtherPlayerCityEffect() == bDefault) m_bOtherPlayerCityEffect = pClassInfo->isOtherPlayerCityEffect();
	if (isGoldToPlayer() == bDefault) m_bGoldToPlayer = pClassInfo->isGoldToPlayer();
	if (isGoldenAge() == bDefault) m_bGoldenAge = pClassInfo->isGoldenAge();
	if (isDeclareWar() == bDefault) m_bDeclareWar = pClassInfo->isDeclareWar();

	if (getGold() == iDefault) m_iGold = pClassInfo->getGold();
	if (getRandomGold() == iDefault) m_iRandomGold = pClassInfo->getRandomGold();
	if (getCulture() == iDefault) m_iCulture = pClassInfo->getCulture();
	if (getEspionagePoints() == iDefault) m_iEspionagePoints = pClassInfo->getEspionagePoints();
	if (getTech() == iTextDefault) m_iTech = pClassInfo->getTech();
	if (getTechPercent() == iDefault) m_iTechPercent = pClassInfo->getTechPercent();
	if (getTechCostPercent() == iDefault) m_iTechCostPercent = pClassInfo->getTechCostPercent();
	if (getTechMinTurnsLeft() == iDefault) m_iTechMinTurnsLeft = pClassInfo->getTechMinTurnsLeft();
	if (getPrereqTech() == iTextDefault) m_iPrereqTech = pClassInfo->getPrereqTech();
	if (getFreeUnit() == iTextDefault) m_iFreeUnit = pClassInfo->getFreeUnit();
	if (getNumUnits() == iDefault) m_iNumUnits = pClassInfo->getNumUnits();
	if (m_bDisbandUnit == bDefault) m_bDisbandUnit = pClassInfo->isDisbandUnit();
	if (m_bGameSpeedScale == bDefault) m_bGameSpeedScale = pClassInfo->isGameSpeedScale();
	if (getUnitExperience() == iDefault) m_iUnitExperience = pClassInfo->getUnitExperience();
	if (getUnitImmobileTurns() == iDefault) m_iUnitImmobileTurns = pClassInfo->getUnitImmobileTurns();
	if (getBuilding() == iTextDefault) m_iBuilding = pClassInfo->getBuilding();
	if (getBuildingChange() == iDefault) m_iBuildingChange = pClassInfo->getBuildingChange();
	if (getHappy() == iDefault) m_iHappy = pClassInfo->getHappy();
	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getHurryAnger() == iDefault) m_iHurryAnger = pClassInfo->getHurryAnger();
	if (getHappyTurns() == iDefault) m_iHappyTurns = pClassInfo->getHappyTurns();
	if (getFood() == iDefault) m_iFood = pClassInfo->getFood();
	if (getFoodPercent() == iDefault) m_iFoodPercent = pClassInfo->getFoodPercent();
	if (getFeature() == iTextDefault) m_iFeature = pClassInfo->getFeature();
	if (getFeatureChange() == iDefault) m_iFeatureChange = pClassInfo->getFeatureChange();
	if (getImprovement() == iTextDefault) m_iImprovement = pClassInfo->getImprovement();
	if (getImprovementChange() == iDefault) m_iImprovementChange = pClassInfo->getImprovementChange();
	if (getBonus() == iTextDefault) m_iBonus = pClassInfo->getBonus();
	if (getBonusChange() == iDefault) m_iBonusChange = pClassInfo->getBonusChange();
	if (getRouteChange() == iDefault) m_iRouteChange = pClassInfo->getRouteChange();
	if (getRoute() == iTextDefault) m_iRoute = pClassInfo->getRoute();
	if (getBonusRevealed() == iTextDefault) m_iBonusRevealed = pClassInfo->getBonusRevealed();
	if (getBonusGift() == iTextDefault) m_iBonusGift = pClassInfo->getBonusGift();

	for ( int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		if (getTechFlavorValue(i) == iDefault && pClassInfo->getTechFlavorValue(i) != iDefault)
		{
			if ( NULL == m_piTechFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piTechFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piTechFlavorValue[i] = pClassInfo->getTechFlavorValue(i);
		}
	}
	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( getCommerceModifier(j) == iDefault && pClassInfo->getCommerceModifier(j) != iDefault)
		{
			if ( NULL == m_piCommerceModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceModifier,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceModifier[j] = pClassInfo->getCommerceModifier(j);
		}
	}
	for ( int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if ( getYieldModifier(i) == iDefault && pClassInfo->getYieldModifier(i) != iDefault)
		{
			if ( NULL == m_piYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piYieldModifier[i] = pClassInfo->getYieldModifier(i);
		}
		if (getPlotExtraYield(i) == iDefault && pClassInfo->getPlotExtraYield(i) != iDefault)
		{
			if ( NULL == m_piPlotExtraYields )
			{
				CvXMLLoadUtility::InitList(&m_piPlotExtraYields,NUM_YIELD_TYPES,iDefault);
			}
			m_piPlotExtraYields[i] = pClassInfo->getPlotExtraYield(i);
		}
	}
	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++)
	{
		if (getFreeSpecialistCount(i) == iDefault && pClassInfo->getFreeSpecialistCount(i) != iDefault)
		{
			if ( NULL == m_piFreeSpecialistCount )
			{
				CvXMLLoadUtility::InitList(&m_piFreeSpecialistCount,GC.getNumSpecialistInfos(),iDefault);
			}
			m_piFreeSpecialistCount[i] = pClassInfo->getFreeSpecialistCount(i);
		}
	}

	if (getConvertOwnCities() == iDefault) m_iConvertOwnCities = pClassInfo->getConvertOwnCities();
	if (getConvertOtherCities() == iDefault) m_iConvertOtherCities = pClassInfo->getConvertOtherCities();
	if (getMaxNumReligions() == -1) m_iMaxNumReligions = pClassInfo->getMaxNumReligions();
	if (getOurAttitudeModifier() == iDefault) m_iOurAttitudeModifier = pClassInfo->getOurAttitudeModifier();
	if (getAttitudeModifier() == iDefault) m_iAttitudeModifier = pClassInfo->getAttitudeModifier();
	if (getTheirEnemyAttitudeModifier() == iDefault) m_iTheirEnemyAttitudeModifier = pClassInfo->getTheirEnemyAttitudeModifier();
	if (getPopulationChange() == iDefault) m_iPopulationChange = pClassInfo->getPopulationChange();
	if (getRevoltTurns() == iDefault) m_iRevoltTurns = pClassInfo->getRevoltTurns();
	if (getMinPillage() == iDefault) m_iMinPillage = pClassInfo->getMinPillage();
	if (getMaxPillage() == iDefault) m_iMaxPillage = pClassInfo->getMaxPillage();
	if (getFreeUnitSupport() == iDefault) m_iFreeUnitSupport = pClassInfo->getFreeUnitSupport();
	if (getInflationModifier() == iDefault) m_iInflationModifier = pClassInfo->getInflationModifier();
	if (getSpaceProductionModifier() == iDefault) m_iSpaceProductionModifier = pClassInfo->getSpaceProductionModifier();
	if (getAIValue() == iDefault) m_iAIValue = pClassInfo->getAIValue();

	for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
	{
		if (m_piUnitCombatPromotions[i] == iTextDefault)
		{
			m_piUnitCombatPromotions[i] = pClassInfo->getUnitCombatPromotion(i);
		}
	}

	for (int i = 0; i < GC.getNumUnitInfos(); ++i)
	{
		if (m_piUnitPromotions[i] == iTextDefault)
		{
			m_piUnitPromotions[i] = pClassInfo->getUnitPromotion(i);
		}
	}

	if (getUnitNameKey() == wDefault) m_szUnitName = pClassInfo->getUnitNameKey();
	if (getOtherPlayerPopup() == wDefault) m_szOtherPlayerPopup = pClassInfo->getOtherPlayerPopup();
	if (getQuestFailTextKey() == wDefault) m_szQuestFailText = pClassInfo->getQuestFailTextKey();
	if (getLocalInfoTextKey() == wDefault) m_szLocalInfoText = pClassInfo->getLocalInfoTextKey();

	m_Properties.copyNonDefaults(pClassInfo->getProperties());
	m_PropertiesAllCities.copyNonDefaults(pClassInfo->getPropertiesAllCities());

	if (getPythonCallback() == cDefault) m_szPythonCallback = pClassInfo->getPythonCallback();
	if (getPythonExpireCheck() == cDefault) m_szPythonExpireCheck = pClassInfo->getPythonExpireCheck();
	if (getPythonCanDo() == cDefault) m_szPythonCanDo = pClassInfo->getPythonCanDo();
	if (getPythonHelp() == cDefault) m_szPythonHelp = pClassInfo->getPythonHelp();

	CvWString szwTextVal;
	for ( int i = 0; i < pClassInfo->getNumWorldNews(); i++)
	{
		szwTextVal = pClassInfo->getWorldNews(i);
		m_aszWorldNews.push_back(szwTextVal);
	}

	bool bNoDuplicate = true;
	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
		foreach_(const BuildingYieldChange& it, m_aBuildingYieldChanges)
		{
			if (it.eBuilding == (BuildingTypes)iBuilding)
			{
				//obviously some modder already set this Building to some value
				//we don't want to overwrite his settings with the older(assuming he added
				//his tag on purpose)
				bNoDuplicate = false;
				break;
			}
		}
		if ( bNoDuplicate )
		{
			for ( int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				if ( pClassInfo->getBuildingYieldChange(iBuilding, iYield) != 0)
				{
					BuildingYieldChange kChange;
					kChange.eBuilding = (BuildingTypes)iBuilding;
					kChange.eYield = (YieldTypes)iYield;
					kChange.iChange = pClassInfo->getBuildingYieldChange(iBuilding, iYield);
					m_aBuildingYieldChanges.push_back(kChange);
				}
			}
		}
	}

	bNoDuplicate = true;
	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
		foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceChanges)
		{
			if (it.eBuilding == (BuildingTypes)iBuilding)
			{
				//obviously some modder already set this Building to some value
				//we don't want to overwrite his settings with the older(assuming he added
				//his tag on purpose)
				bNoDuplicate = false;
				break;
			}
		}
		if ( bNoDuplicate )
		{
			for ( int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; iCommerce++)
			{
				if ( pClassInfo->getBuildingCommerceChange(iBuilding, iCommerce) != 0)
				{
					BuildingCommerceChange kChange;
					kChange.eBuilding = (BuildingTypes)iBuilding;
					kChange.eCommerce = (CommerceTypes)iCommerce;
					kChange.iChange = pClassInfo->getBuildingCommerceChange(iBuilding, iCommerce);
					m_aBuildingCommerceChanges.push_back(kChange);
				}
			}
		}
	}

	bNoDuplicate = true;
	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
		foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceModifiers)
		{
			if (it.eBuilding == (BuildingTypes)iBuilding)
			{
				//obviously some modder already set this Building to some value
				//we don't want to overwrite his settings with the older(assuming he added
				//his tag on purpose)
				bNoDuplicate = false;
				break;
			}
		}
		if ( bNoDuplicate )
		{
			for ( int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; iCommerce++)
			{
				if ( pClassInfo->getBuildingCommerceModifier(iBuilding, iCommerce) != 0)
				{
					BuildingCommerceChange kChange;
					kChange.eBuilding = (BuildingTypes)iBuilding;
					kChange.eCommerce = (CommerceTypes)iCommerce;
					kChange.iChange = pClassInfo->getBuildingCommerceModifier(iBuilding, iCommerce);
					m_aBuildingCommerceModifiers.push_back(kChange);
				}
			}
		}
	}

	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
		if ( pClassInfo->getBuildingHappyChange(iBuilding) != 0)
		{
			//if the new value is set, we assume the modder did this on purpose
			//so we only add the old value, if the new isn't 0(as with all the new loading system)
			if ( getBuildingHappyChange(iBuilding) == 0 )
			{
				const BuildingTypes eBuilding = (BuildingTypes)iBuilding;
				const int iChange = pClassInfo->getBuildingHappyChange(iBuilding);
				m_aBuildingHappyChanges.push_back(std::make_pair(eBuilding, iChange));
			}
		}
	}

	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
		if ( pClassInfo->getBuildingHealthChange(iBuilding) != 0)
		{
			//if the new value is set, we assume the modder did this on purpose
			//so we only add the old value, if the new isn't 0(as with all the new loading system)
			if ( getBuildingHealthChange(iBuilding) == 0 )
			{
				const BuildingTypes eBuilding = (BuildingTypes)iBuilding;
				const int iChange = pClassInfo->getBuildingHealthChange(iBuilding);
				m_aBuildingHealthChanges.push_back(std::make_pair(eBuilding, iChange));
			}
		}
	}

	if (getPrereqGameOption() == iTextDefault) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();
	if (getRevolutionIndexChange() == iDefault) m_iRevolutionIndexChange = pClassInfo->getRevolutionIndexChange();

	for ( int i = 0; i < pClassInfo->getAdditionalEventChanceVectorSize(); i++ )
	{
		m_aiAdditionalEventChanceforPass3.push_back(pClassInfo->getAdditionalEventChanceValuesVectorElement(i));
		m_aszAdditionalEventChanceforPass3.push_back(pClassInfo->getAdditionalEventChanceNamesVectorElement(i));
	}
	for ( int i = 0; i < pClassInfo->getAdditionalEventTimeVectorSize(); i++ )
	{
		m_aiAdditionalEventTimeforPass3.push_back(pClassInfo->getAdditionalEventTimeValuesVectorElement(i));
		m_aszAdditionalEventTimeforPass3.push_back(pClassInfo->getAdditionalEventTimeNamesVectorElement(i));
	}
	for ( int i = 0; i < pClassInfo->getClearEventChanceVectorSize(); i++ )
	{
		m_aiClearEventChanceforPass3.push_back(pClassInfo->getClearEventChanceValuesVectorElement(i));
		m_aszClearEventChanceforPass3.push_back(pClassInfo->getClearEventChanceNamesVectorElement(i));
	}
}



bool CvEventInfo::readPass3()
{
	PROFILE_EXTRA_FUNC();
	m_piAdditionalEventChance = new int[GC.getNumEventInfos()];
	m_piAdditionalEventTime = new int[GC.getNumEventInfos()];
	m_piClearEventChance = new int[GC.getNumEventInfos()];
	for (int iI = 0; iI < GC.getNumEventInfos(); iI++)
	{
		m_piAdditionalEventChance[iI] = 0;
		m_piAdditionalEventTime[iI] = 0;
		m_piClearEventChance[iI] = 0;
	}
	if (!m_aiAdditionalEventChanceforPass3.empty() && !m_aszAdditionalEventChanceforPass3.empty())
	{
		int iNumLoad = m_aiAdditionalEventChanceforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszAdditionalEventChanceforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumEventInfos())
				m_piAdditionalEventChance[iTempIndex] = m_aiAdditionalEventChanceforPass3[iI];
		}
		m_aszAdditionalEventChanceforPass3.clear();
		m_aiAdditionalEventChanceforPass3.clear();
	}
	if (!m_aiAdditionalEventTimeforPass3.empty() && !m_aszAdditionalEventTimeforPass3.empty())
	{
		int iNumLoad = m_aiAdditionalEventTimeforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszAdditionalEventTimeforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumEventInfos())
				m_piAdditionalEventTime[iTempIndex] = m_aiAdditionalEventTimeforPass3[iI];
		}
		m_aszAdditionalEventTimeforPass3.clear();
		m_aiAdditionalEventTimeforPass3.clear();
	}
	if (!m_aiClearEventChanceforPass3.empty() && !m_aszClearEventChanceforPass3.empty())
	{
		int iNumLoad = m_aiClearEventChanceforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszClearEventChanceforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumEventInfos())
				m_piClearEventChance[iTempIndex] = m_aiClearEventChanceforPass3[iI];
		}
		m_aszClearEventChanceforPass3.clear();
		m_aiClearEventChanceforPass3.clear();
	}
	return true;
}

