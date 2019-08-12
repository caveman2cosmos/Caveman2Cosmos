#include "CvGameCoreDLL.h"

//======================================================================================================
//					CvVictoryInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvVictoryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvVictoryInfo::CvVictoryInfo() :
m_iPopulationPercentLead(0),
m_iLandPercent(0),
m_iMinLandPercent(0),
m_iReligionPercent(0),
m_iCityCulture(NO_CULTURELEVEL),
m_iNumCultureCities(0),
m_iTotalCultureRatio(0),
m_iVictoryDelayTurns(0),
m_bTargetScore(false),
m_bEndScore(false),
m_bConquest(false),
m_bDiploVote(false),
m_bPermanent(false)
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
#if defined QC_MASTERY_VICTORY
,m_bTotalVictory(false)
#endif
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvVictoryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvVictoryInfo::~CvVictoryInfo()
{
}

int CvVictoryInfo::getPopulationPercentLead() const
{
	return m_iPopulationPercentLead;
}

int CvVictoryInfo::getLandPercent() const
{
	return m_iLandPercent;
}

int CvVictoryInfo::getMinLandPercent() const
{
	return m_iMinLandPercent;
}

int CvVictoryInfo::getReligionPercent() const
{
	return m_iReligionPercent;
}

int CvVictoryInfo::getCityCulture() const
{
	return m_iCityCulture;
}

int CvVictoryInfo::getNumCultureCities() const
{
	return m_iNumCultureCities;
}

int CvVictoryInfo::getTotalCultureRatio() const
{
	return m_iTotalCultureRatio;
}

int CvVictoryInfo::getVictoryDelayTurns() const
{
	return m_iVictoryDelayTurns;
}

bool CvVictoryInfo::isTargetScore() const
{
	return m_bTargetScore;
}

bool CvVictoryInfo::isEndScore() const
{
	return m_bEndScore;
}

bool CvVictoryInfo::isConquest() const
{
	return m_bConquest;
}

bool CvVictoryInfo::isDiploVote() const
{
	return m_bDiploVote;
}

bool CvVictoryInfo::isPermanent() const
{
	return m_bPermanent;
}

const char* CvVictoryInfo::getMovie() const
{
	return m_szMovie;
}
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
#if defined QC_MASTERY_VICTORY
bool CvVictoryInfo::isTotalVictory() const
{
	return m_bTotalVictory;
}
#endif
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/


//
// read from xml
//
bool CvVictoryInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bTargetScore, L"bTargetScore");
	pXML->GetOptionalChildXmlValByName(&m_bEndScore, L"bEndScore");
	pXML->GetOptionalChildXmlValByName(&m_bConquest, L"bConquest");
	pXML->GetOptionalChildXmlValByName(&m_bDiploVote, L"bDiploVote");
	pXML->GetOptionalChildXmlValByName(&m_bPermanent, L"bPermanent");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationPercentLead, L"iPopulationPercentLead");
	pXML->GetOptionalChildXmlValByName(&m_iLandPercent, L"iLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iMinLandPercent, L"iMinLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iReligionPercent, L"iReligionPercent");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CityCulture");
	m_iCityCulture = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iNumCultureCities, L"iNumCultureCities");
	pXML->GetOptionalChildXmlValByName(&m_iTotalCultureRatio, L"iTotalCultureRatio");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryDelayTurns, L"iVictoryDelayTurns");
	pXML->GetOptionalChildXmlValByName(m_szMovie, L"VictoryMovie");

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
#if defined QC_MASTERY_VICTORY
	pXML->GetOptionalChildXmlValByName(&m_bTotalVictory, L"bTotalVictory");
#endif
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	return true;
}

void CvVictoryInfo::copyNonDefaults(CvVictoryInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (isTargetScore() == bDefault) m_bTargetScore = pClassInfo->isTargetScore();
	if (isEndScore() == bDefault) m_bEndScore = pClassInfo->isEndScore();
	if (isConquest() == bDefault) m_bConquest = pClassInfo->isConquest();
	if (isDiploVote() == bDefault) m_bDiploVote = pClassInfo->isDiploVote();
	if (isPermanent() == bDefault) m_bPermanent = pClassInfo->isPermanent();

	if (getPopulationPercentLead() == iDefault) m_iPopulationPercentLead = pClassInfo->getPopulationPercentLead();
	if (getLandPercent() == iDefault) m_iLandPercent = pClassInfo->getLandPercent();
	if (getMinLandPercent() == iDefault) m_iMinLandPercent = pClassInfo->getMinLandPercent();
	if (getReligionPercent() == iDefault) m_iReligionPercent = pClassInfo->getReligionPercent();

	if (getCityCulture() == iTextDefault) m_iCityCulture = pClassInfo->getCityCulture();

	if (getNumCultureCities() == iDefault) m_iNumCultureCities = pClassInfo->getNumCultureCities();
	if (getTotalCultureRatio() == iDefault) m_iTotalCultureRatio = pClassInfo->getTotalCultureRatio();
	if (getVictoryDelayTurns() == iDefault) m_iVictoryDelayTurns = pClassInfo->getVictoryDelayTurns();

	if (getMovie() == cDefault) m_szMovie = pClassInfo->getMovie();
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	#if defined QC_MASTERY_VICTORY
	if (isTotalVictory() == bDefault) m_bTotalVictory = pClassInfo->isTotalVictory();
	#endif
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
}

void CvVictoryInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iPopulationPercentLead);
	CheckSum(iSum, m_iLandPercent);
	CheckSum(iSum, m_iMinLandPercent);
	CheckSum(iSum, m_iReligionPercent);
	CheckSum(iSum, m_iCityCulture);
	CheckSum(iSum, m_iNumCultureCities);
	CheckSum(iSum, m_iTotalCultureRatio);
	CheckSum(iSum, m_iVictoryDelayTurns);

	CheckSum(iSum, m_bTargetScore);
	CheckSum(iSum, m_bEndScore);
	CheckSum(iSum, m_bConquest);
	CheckSum(iSum, m_bDiploVote);
	CheckSum(iSum, m_bPermanent);
}

//======================================================================================================
//					CvHandicapInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvHandicapInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvHandicapInfo::CvHandicapInfo() :
m_iFreeWinsVsBarbs(0),
m_iAnimalAttackProb(0),
m_iStartingLocationPercent(0),
m_iAdvancedStartPointsMod(0),
m_iStartingGold(0),
m_iFreeUnits(0),
m_iUnitCostPercent(0),
m_iTrainPercent(0),
m_iConstructPercent(0),
m_iResearchPercent(0),
m_iDistanceMaintenancePercent(0),
m_iNumCitiesMaintenancePercent(0),
m_iMaxNumCitiesMaintenance(0),
m_iColonyMaintenancePercent(0),
m_iMaxColonyMaintenance(0),
m_iCorporationMaintenancePercent(0),
m_iCivicUpkeepPercent(0),
m_iInflationPercent(0),
m_iHealthBonus(0),
m_iHappyBonus(0),
m_iAttitudeChange(0),
m_iNoTechTradeModifier(0),
m_iTechTradeKnownModifier(0),
m_iUnownedTilesPerGameAnimal(0),
m_iUnownedTilesPerBarbarianUnit(0),
m_iUnownedWaterTilesPerBarbarianUnit(0),
m_iUnownedTilesPerBarbarianCity(0),
m_iBarbarianCreationTurnsElapsed(0),
m_iBarbarianCityCreationTurnsElapsed(0),
m_iBarbarianCityCreationProb(0),
m_iAnimalCombatModifier(0),
m_iBarbarianCombatModifier(0),
m_iAIAnimalCombatModifier(0),
m_iAIBarbarianCombatModifier(0),
m_iStartingDefenseUnits(0),
m_iStartingWorkerUnits(0),
m_iStartingExploreUnits(0),
m_iAIStartingUnitMultiplier(0),
m_iAIStartingDefenseUnits(0),
m_iAIStartingWorkerUnits(0),
m_iAIStartingExploreUnits(0),
m_iBarbarianInitialDefenders(0),
m_iAIDeclareWarProb(0),
m_iAIWorkRateModifier(0),
m_iAIGrowthPercent(0),
m_iAITrainPercent(0),
m_iAIWorldTrainPercent(0),
m_iAIConstructPercent(0),
m_iAIWorldConstructPercent(0),
m_iAICreatePercent(0),
m_iAIResearchPercent(0),
m_iAIWorldCreatePercent(0),
m_iAICivicUpkeepPercent(0),
m_iAIUnitCostPercent(0),
m_iAIUnitSupplyPercent(0),
m_iAIUnitUpgradePercent(0),
m_iAIInflationPercent(0),
m_iAIWarWearinessPercent(0),
m_iAIPerEraModifier(0),
m_iAIAdvancedStartPercent(0),
m_iNumGoodies(0),
m_piGoodies(NULL),
m_pbFreeTechs(NULL),
m_pbAIFreeTechs(NULL)
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iRevolutionIndexPercent(0)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
,m_Percent()
,m_PropertyManipulators()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvHandicapInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvHandicapInfo::~CvHandicapInfo()
{
	SAFE_DELETE_ARRAY(m_piGoodies);
	SAFE_DELETE_ARRAY(m_pbFreeTechs);
	SAFE_DELETE_ARRAY(m_pbAIFreeTechs);
}

int CvHandicapInfo::getFreeWinsVsBarbs() const
{
	return m_iFreeWinsVsBarbs;
}

int CvHandicapInfo::getAnimalAttackProb() const
{
	return m_iAnimalAttackProb;
}

int CvHandicapInfo::getStartingLocationPercent() const
{
	return m_iStartingLocationPercent;
}

int CvHandicapInfo::getAdvancedStartPointsMod() const
{
	return m_iAdvancedStartPointsMod;
}

int CvHandicapInfo::getStartingGold() const
{
	return m_iStartingGold;
}

int CvHandicapInfo::getFreeUnits() const
{
	return m_iFreeUnits;
}

int CvHandicapInfo::getUnitCostPercent() const
{
	return m_iUnitCostPercent;
}

int CvHandicapInfo::getTrainPercent() const
{
	return m_iTrainPercent;
}

int CvHandicapInfo::getConstructPercent() const
{
	return m_iConstructPercent;
}

int CvHandicapInfo::getResearchPercent() const
{
	return m_iResearchPercent;
}

int CvHandicapInfo::getDistanceMaintenancePercent() const
{
	return m_iDistanceMaintenancePercent;
}

int CvHandicapInfo::getNumCitiesMaintenancePercent() const
{
	return m_iNumCitiesMaintenancePercent;
}

int CvHandicapInfo::getMaxNumCitiesMaintenance() const
{
	return m_iMaxNumCitiesMaintenance;
}

int CvHandicapInfo::getColonyMaintenancePercent() const
{
	return m_iColonyMaintenancePercent;
}

int CvHandicapInfo::getMaxColonyMaintenance() const
{
	return m_iMaxColonyMaintenance;
}

int CvHandicapInfo::getCorporationMaintenancePercent() const
{
	return m_iCorporationMaintenancePercent;
}

int CvHandicapInfo::getCivicUpkeepPercent() const
{
	return m_iCivicUpkeepPercent;
}

int CvHandicapInfo::getInflationPercent() const
{
	return m_iInflationPercent;
}

int CvHandicapInfo::getHealthBonus() const
{
	return m_iHealthBonus;
}

int CvHandicapInfo::getHappyBonus() const
{
	return m_iHappyBonus;
}

int CvHandicapInfo::getAttitudeChange() const
{
	return m_iAttitudeChange;
}

int CvHandicapInfo::getNoTechTradeModifier() const
{
	return m_iNoTechTradeModifier;
}

int CvHandicapInfo::getTechTradeKnownModifier() const
{
	return m_iTechTradeKnownModifier;
}

int CvHandicapInfo::getUnownedTilesPerGameAnimal() const
{
	return m_iUnownedTilesPerGameAnimal;
}

int CvHandicapInfo::getUnownedTilesPerBarbarianUnit() const
{
	return m_iUnownedTilesPerBarbarianUnit;
}

int CvHandicapInfo::getUnownedWaterTilesPerBarbarianUnit() const
{
	return m_iUnownedWaterTilesPerBarbarianUnit;
}

int CvHandicapInfo::getUnownedTilesPerBarbarianCity() const
{
	return m_iUnownedTilesPerBarbarianCity;
}

int CvHandicapInfo::getBarbarianCreationTurnsElapsed() const
{
	return m_iBarbarianCreationTurnsElapsed;
}

int CvHandicapInfo::getBarbarianCityCreationTurnsElapsed() const
{
	return m_iBarbarianCityCreationTurnsElapsed;
}

int CvHandicapInfo::getBarbarianCityCreationProb() const
{
	return m_iBarbarianCityCreationProb;
}

int CvHandicapInfo::getAnimalCombatModifier() const
{
	return m_iAnimalCombatModifier;
}

int CvHandicapInfo::getBarbarianCombatModifier() const
{
	return m_iBarbarianCombatModifier;
}

int CvHandicapInfo::getAIAnimalCombatModifier() const
{
	return m_iAIAnimalCombatModifier;
}

int CvHandicapInfo::getAIBarbarianCombatModifier() const
{
	return m_iAIBarbarianCombatModifier;
}

int CvHandicapInfo::getStartingDefenseUnits() const
{
	return m_iStartingDefenseUnits;
}

int CvHandicapInfo::getStartingWorkerUnits() const
{
	return m_iStartingWorkerUnits;
}

int CvHandicapInfo::getStartingExploreUnits() const
{
	return m_iStartingExploreUnits;
}

int CvHandicapInfo::getAIStartingUnitMultiplier() const
{
	return m_iAIStartingUnitMultiplier;
}

int CvHandicapInfo::getAIStartingDefenseUnits() const
{
	return m_iAIStartingDefenseUnits;
}

int CvHandicapInfo::getAIStartingWorkerUnits() const
{
	return m_iAIStartingWorkerUnits;
}

int CvHandicapInfo::getAIStartingExploreUnits() const
{
	return m_iAIStartingExploreUnits;
}

int CvHandicapInfo::getBarbarianInitialDefenders() const
{
	return m_iBarbarianInitialDefenders;
}

int CvHandicapInfo::getAIDeclareWarProb() const
{
	return m_iAIDeclareWarProb;
}

int CvHandicapInfo::getAIWorkRateModifier() const
{
	return m_iAIWorkRateModifier;
}

int CvHandicapInfo::getAIGrowthPercent() const
{
	return m_iAIGrowthPercent;
}

int CvHandicapInfo::getAITrainPercent() const
{
	return m_iAITrainPercent;
}

int CvHandicapInfo::getAIWorldTrainPercent() const
{
	return m_iAIWorldTrainPercent;
}

int CvHandicapInfo::getAIConstructPercent() const
{
	return m_iAIConstructPercent;
}

int CvHandicapInfo::getAIWorldConstructPercent() const
{
	return m_iAIWorldConstructPercent;
}

int CvHandicapInfo::getAICreatePercent() const
{
	return m_iAICreatePercent;
}

int CvHandicapInfo::getAIResearchPercent() const
{
	return m_iAIResearchPercent;
}

int CvHandicapInfo::getAIWorldCreatePercent() const
{
	return m_iAIWorldCreatePercent;
}

int CvHandicapInfo::getAICivicUpkeepPercent() const
{
	return m_iAICivicUpkeepPercent;
}

int CvHandicapInfo::getAIUnitCostPercent() const
{
	return m_iAIUnitCostPercent;
}

int CvHandicapInfo::getAIUnitSupplyPercent() const
{
	return m_iAIUnitSupplyPercent;
}

int CvHandicapInfo::getAIUnitUpgradePercent() const
{
	return m_iAIUnitUpgradePercent;
}

int CvHandicapInfo::getAIInflationPercent() const
{
	return m_iAIInflationPercent;
}

int CvHandicapInfo::getAIWarWearinessPercent() const
{
	return m_iAIWarWearinessPercent;
}

int CvHandicapInfo::getAIPerEraModifier() const
{
	return m_iAIPerEraModifier;
}

int CvHandicapInfo::getAIAdvancedStartPercent() const
{
	return m_iAIAdvancedStartPercent;
}

int CvHandicapInfo::getNumGoodies() const
{
	return m_iNumGoodies;
}

CvPropertyManipulators* CvHandicapInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}

// Arrays

int CvHandicapInfo::getGoodies(int i) const
{
	FAssertMsg(i < getNumGoodies(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piGoodies[i];
}

int CvHandicapInfo::isFreeTechs(int i) const
{
	FAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbFreeTechs ? m_pbFreeTechs[i] : false;
}

int CvHandicapInfo::isAIFreeTechs(int i) const
{
	FAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbAIFreeTechs ? m_pbAIFreeTechs[i] : false;
}

int CvHandicapInfo::getPercent(int iID) const
{
	return m_Percent.getValue(iID);
}

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
int CvHandicapInfo::getRevolutionIndexPercent() const
{
	return m_iRevolutionIndexPercent;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

void CvHandicapInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iFreeWinsVsBarbs);
	CheckSum(iSum, m_iAnimalAttackProb);
	CheckSum(iSum, m_iStartingLocationPercent);
	CheckSum(iSum, m_iAdvancedStartPointsMod);
	CheckSum(iSum, m_iStartingGold);
	CheckSum(iSum, m_iFreeUnits);
	CheckSum(iSum, m_iUnitCostPercent);
	CheckSum(iSum, m_iTrainPercent);
	CheckSum(iSum, m_iConstructPercent);
	CheckSum(iSum, m_iResearchPercent);
	CheckSum(iSum, m_iDistanceMaintenancePercent);
	CheckSum(iSum, m_iNumCitiesMaintenancePercent);
	CheckSum(iSum, m_iMaxNumCitiesMaintenance);
	CheckSum(iSum, m_iColonyMaintenancePercent);
	CheckSum(iSum, m_iMaxColonyMaintenance);
	CheckSum(iSum, m_iCorporationMaintenancePercent);
	CheckSum(iSum, m_iCivicUpkeepPercent);
	CheckSum(iSum, m_iInflationPercent);
	CheckSum(iSum, m_iHealthBonus);
	CheckSum(iSum, m_iHappyBonus);
	CheckSum(iSum, m_iAttitudeChange);
	CheckSum(iSum, m_iNoTechTradeModifier);
	CheckSum(iSum, m_iTechTradeKnownModifier);
	CheckSum(iSum, m_iUnownedTilesPerGameAnimal);
	CheckSum(iSum, m_iUnownedTilesPerBarbarianUnit);
	CheckSum(iSum, m_iUnownedWaterTilesPerBarbarianUnit);
	CheckSum(iSum, m_iUnownedTilesPerBarbarianCity);
	CheckSum(iSum, m_iBarbarianCreationTurnsElapsed);
	CheckSum(iSum, m_iBarbarianCityCreationTurnsElapsed);
	CheckSum(iSum, m_iBarbarianCityCreationProb);
	CheckSum(iSum, m_iAnimalCombatModifier);
	CheckSum(iSum, m_iBarbarianCombatModifier);
	CheckSum(iSum, m_iAIAnimalCombatModifier);
	CheckSum(iSum, m_iAIBarbarianCombatModifier);

	CheckSum(iSum, m_iStartingDefenseUnits);
	CheckSum(iSum, m_iStartingWorkerUnits);
	CheckSum(iSum, m_iStartingExploreUnits);
	CheckSum(iSum, m_iAIStartingUnitMultiplier);
	CheckSum(iSum, m_iAIStartingDefenseUnits);
	CheckSum(iSum, m_iAIStartingWorkerUnits);
	CheckSum(iSum, m_iAIStartingExploreUnits);
	CheckSum(iSum, m_iBarbarianInitialDefenders);
	CheckSum(iSum, m_iAIDeclareWarProb);
	CheckSum(iSum, m_iAIWorkRateModifier);
	CheckSum(iSum, m_iAIGrowthPercent);
	CheckSum(iSum, m_iAITrainPercent);
	CheckSum(iSum, m_iAIWorldTrainPercent);
	CheckSum(iSum, m_iAIConstructPercent);
	CheckSum(iSum, m_iAIWorldConstructPercent);
	CheckSum(iSum, m_iAICreatePercent);
	CheckSum(iSum, m_iAIResearchPercent);
	CheckSum(iSum, m_iAIWorldCreatePercent);
	CheckSum(iSum, m_iAICivicUpkeepPercent);
	CheckSum(iSum, m_iAIUnitCostPercent);
	CheckSum(iSum, m_iAIUnitSupplyPercent);
	CheckSum(iSum, m_iAIUnitUpgradePercent);
	CheckSum(iSum, m_iAIInflationPercent);
	CheckSum(iSum, m_iAIWarWearinessPercent);
	CheckSum(iSum, m_iAIPerEraModifier);
	CheckSum(iSum, m_iAIAdvancedStartPercent);
	CheckSum(iSum, m_iNumGoodies);

	m_Percent.getCheckSum(iSum);

	m_PropertyManipulators.getCheckSum(iSum);

	// Arrays

	CheckSumI(iSum, getNumGoodies(), m_piGoodies);
	CheckSumI(iSum, GC.getNumTechInfos(), m_pbFreeTechs);
	CheckSumI(iSum, GC.getNumTechInfos(), m_pbAIFreeTechs);

	CheckSum(iSum, m_iRevolutionIndexPercent);
}

bool CvHandicapInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int j;

	pXML->GetOptionalChildXmlValByName(&m_iFreeWinsVsBarbs, L"iFreeWinsVsBarbs");
	pXML->GetOptionalChildXmlValByName(&m_iAnimalAttackProb, L"iAnimalAttackProb");
	pXML->GetOptionalChildXmlValByName(&m_iStartingLocationPercent, L"iStartingLocPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartPointsMod, L"iAdvancedStartPointsMod");
	pXML->GetOptionalChildXmlValByName(&m_iStartingGold, L"iGold");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnits, L"iFreeUnits");
	pXML->GetOptionalChildXmlValByName(&m_iUnitCostPercent, L"iUnitCostPercent");
	pXML->GetOptionalChildXmlValByName(&m_iTrainPercent, L"iTrainPercent");
	pXML->GetOptionalChildXmlValByName(&m_iConstructPercent, L"iConstructPercent");
	pXML->GetOptionalChildXmlValByName(&m_iResearchPercent, L"iResearchPercent");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenancePercent, L"iDistanceMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenancePercent, L"iNumCitiesMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iMaxNumCitiesMaintenance, L"iMaxNumCitiesMaintenance");
	pXML->GetOptionalChildXmlValByName(&m_iColonyMaintenancePercent, L"iColonyMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iMaxColonyMaintenance, L"iMaxColonyMaintenance");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenancePercent, L"iCorporationMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iCivicUpkeepPercent, L"iCivicUpkeepPercent");
	pXML->GetOptionalChildXmlValByName(&m_iInflationPercent, L"iInflationPercent");
	pXML->GetOptionalChildXmlValByName(&m_iHealthBonus, L"iHealthBonus");
	pXML->GetOptionalChildXmlValByName(&m_iHappyBonus, L"iHappyBonus");
	pXML->GetOptionalChildXmlValByName(&m_iAttitudeChange, L"iAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoTechTradeModifier, L"iNoTechTradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTechTradeKnownModifier, L"iTechTradeKnownModifier");
	pXML->GetOptionalChildXmlValByName(&m_iUnownedTilesPerGameAnimal, L"iUnownedTilesPerGameAnimal");
	pXML->GetOptionalChildXmlValByName(&m_iUnownedTilesPerBarbarianUnit, L"iUnownedTilesPerBarbarianUnit");
	pXML->GetOptionalChildXmlValByName(&m_iUnownedWaterTilesPerBarbarianUnit, L"iUnownedWaterTilesPerBarbarianUnit");
	pXML->GetOptionalChildXmlValByName(&m_iUnownedTilesPerBarbarianCity, L"iUnownedTilesPerBarbarianCity");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianCreationTurnsElapsed, L"iBarbarianCreationTurnsElapsed");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianCityCreationTurnsElapsed, L"iBarbarianCityCreationTurnsElapsed");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianCityCreationProb, L"iBarbarianCityCreationProb");
	pXML->GetOptionalChildXmlValByName(&m_iAnimalCombatModifier, L"iAnimalBonus");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianCombatModifier, L"iBarbarianBonus");
	pXML->GetOptionalChildXmlValByName(&m_iAIAnimalCombatModifier, L"iAIAnimalBonus");
	pXML->GetOptionalChildXmlValByName(&m_iAIBarbarianCombatModifier, L"iAIBarbarianBonus");
	pXML->GetOptionalChildXmlValByName(&m_iStartingDefenseUnits, L"iStartingDefenseUnits");
	pXML->GetOptionalChildXmlValByName(&m_iStartingWorkerUnits, L"iStartingWorkerUnits");
	pXML->GetOptionalChildXmlValByName(&m_iStartingExploreUnits, L"iStartingExploreUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAIStartingUnitMultiplier, L"iAIStartingUnitMultiplier");
	pXML->GetOptionalChildXmlValByName(&m_iAIStartingDefenseUnits, L"iAIStartingDefenseUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAIStartingWorkerUnits, L"iAIStartingWorkerUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAIStartingExploreUnits, L"iAIStartingExploreUnits");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianInitialDefenders, L"iBarbarianDefenders");
	pXML->GetOptionalChildXmlValByName(&m_iAIDeclareWarProb, L"iAIDeclareWarProb");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorkRateModifier, L"iAIWorkRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAIGrowthPercent, L"iAIGrowthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAITrainPercent, L"iAITrainPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorldTrainPercent, L"iAIWorldTrainPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIConstructPercent, L"iAIConstructPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorldConstructPercent, L"iAIWorldConstructPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAICreatePercent, L"iAICreatePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIResearchPercent, L"iAIResearchPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorldCreatePercent, L"iAIWorldCreatePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAICivicUpkeepPercent, L"iAICivicUpkeepPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIUnitCostPercent, L"iAIUnitCostPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIUnitSupplyPercent, L"iAIUnitSupplyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIUnitUpgradePercent, L"iAIUnitUpgradePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIInflationPercent, L"iAIInflationPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWarWearinessPercent, L"iAIWarWearinessPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIPerEraModifier, L"iAIPerEraModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAIAdvancedStartPercent, L"iAIAdvancedStartPercent");

	if (pXML->TryMoveToXmlFirstChild(L"Goodies"))
	{
		CvString* pszGoodyNames = NULL;
		pXML->SetStringList(&pszGoodyNames, &m_iNumGoodies);

		if (m_iNumGoodies > 0)
		{
			m_piGoodies = new int[m_iNumGoodies];

			for (j=0;j<m_iNumGoodies;j++)
			{
				m_piGoodies[j] = pXML->GetInfoClass(pszGoodyNames[j]);
			}
		}
		else
		{
			m_piGoodies = NULL;
		}

		pXML->MoveToXmlParent();

		SAFE_DELETE_ARRAY(pszGoodyNames);
	}

	pXML->SetVariableListTagPair(&m_pbFreeTechs, L"FreeTechs", GC.getNumTechInfos());
	pXML->SetVariableListTagPair(&m_pbAIFreeTechs, L"AIFreeTechs", GC.getNumTechInfos());

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iRevolutionIndexPercent, L"iRevolutionIndexPercent");
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	if (pXML->TryMoveToXmlFirstChild(L"Percents"))
	{
		m_Percent.read(pXML);

		pXML->MoveToXmlParent();
	}

	m_PropertyManipulators.read(pXML);

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvHandicapInfo::copyNonDefaults(CvHandicapInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getFreeWinsVsBarbs() == iDefault) m_iFreeWinsVsBarbs = pClassInfo->getFreeWinsVsBarbs();
	if (getAnimalAttackProb() == iDefault) m_iAnimalAttackProb = pClassInfo->getAnimalAttackProb();
	if (getStartingLocationPercent() == iDefault) m_iStartingLocationPercent = pClassInfo->getStartingLocationPercent();
	if (getAdvancedStartPointsMod() == iDefault) m_iAdvancedStartPointsMod = pClassInfo->getAdvancedStartPointsMod();
	if (getStartingGold() == iDefault) m_iStartingGold = pClassInfo->getStartingGold();
	if (getFreeUnits() == iDefault) m_iFreeUnits = pClassInfo->getFreeUnits();
	if (getUnitCostPercent() == iDefault) m_iUnitCostPercent = pClassInfo->getUnitCostPercent();
	if (getTrainPercent() == iDefault) m_iTrainPercent = pClassInfo->getTrainPercent();
	if (getConstructPercent() == iDefault) m_iConstructPercent = pClassInfo->getConstructPercent();
	if (getResearchPercent() == iDefault) m_iResearchPercent = pClassInfo->getResearchPercent();
	if (getDistanceMaintenancePercent() == iDefault) m_iDistanceMaintenancePercent = pClassInfo->getDistanceMaintenancePercent();
	if (getNumCitiesMaintenancePercent() == iDefault) m_iNumCitiesMaintenancePercent = pClassInfo->getNumCitiesMaintenancePercent();
	if (getMaxNumCitiesMaintenance() == iDefault) m_iMaxNumCitiesMaintenance = pClassInfo->getMaxNumCitiesMaintenance();
	if (getColonyMaintenancePercent() == iDefault) m_iColonyMaintenancePercent = pClassInfo->getColonyMaintenancePercent();
	if (getMaxColonyMaintenance() == iDefault) m_iMaxColonyMaintenance = pClassInfo->getMaxColonyMaintenance();
	if (getCorporationMaintenancePercent() == iDefault) m_iCorporationMaintenancePercent = pClassInfo->getCorporationMaintenancePercent();
	if (getCivicUpkeepPercent() == iDefault) m_iCivicUpkeepPercent = pClassInfo->getCivicUpkeepPercent();
	if (getInflationPercent() == iDefault) m_iInflationPercent = pClassInfo->getInflationPercent();
	if (getHealthBonus() == iDefault) m_iHealthBonus = pClassInfo->getHealthBonus();
	if (getHappyBonus() == iDefault) m_iHappyBonus = pClassInfo->getHappyBonus();
	if (getAttitudeChange() == iDefault) m_iAttitudeChange = pClassInfo->getAttitudeChange();
	if (getNoTechTradeModifier() == iDefault) m_iNoTechTradeModifier = pClassInfo->getNoTechTradeModifier();
	if (getTechTradeKnownModifier() == iDefault) m_iTechTradeKnownModifier = pClassInfo->getTechTradeKnownModifier();
	if (getUnownedTilesPerGameAnimal() == iDefault) m_iUnownedTilesPerGameAnimal = pClassInfo->getUnownedTilesPerGameAnimal();
	if (getUnownedTilesPerBarbarianUnit() == iDefault) m_iUnownedTilesPerBarbarianUnit = pClassInfo->getUnownedTilesPerBarbarianUnit();
	if (getUnownedWaterTilesPerBarbarianUnit() == iDefault) m_iUnownedWaterTilesPerBarbarianUnit = pClassInfo->getUnownedWaterTilesPerBarbarianUnit();
	if (getUnownedTilesPerBarbarianCity() == iDefault) m_iUnownedTilesPerBarbarianCity = pClassInfo->getUnownedTilesPerBarbarianCity();
	if (getBarbarianCreationTurnsElapsed() == iDefault) m_iBarbarianCreationTurnsElapsed = pClassInfo->getBarbarianCreationTurnsElapsed();
	if (getBarbarianCityCreationTurnsElapsed() == iDefault) m_iBarbarianCityCreationTurnsElapsed = pClassInfo->getBarbarianCityCreationTurnsElapsed();
	if (getBarbarianCityCreationProb() == iDefault) m_iBarbarianCityCreationProb = pClassInfo->getBarbarianCityCreationProb();
	if (getAnimalCombatModifier() == iDefault) m_iAnimalCombatModifier = pClassInfo->getAnimalCombatModifier();
	if (getBarbarianCombatModifier() == iDefault) m_iBarbarianCombatModifier = pClassInfo->getBarbarianCombatModifier();
	if (getAIAnimalCombatModifier() == iDefault) m_iAIAnimalCombatModifier = pClassInfo->getAIAnimalCombatModifier();
	if (getAIBarbarianCombatModifier() == iDefault) m_iAIBarbarianCombatModifier = pClassInfo->getAIBarbarianCombatModifier();
	if (getStartingDefenseUnits() == iDefault) m_iStartingDefenseUnits = pClassInfo->getStartingDefenseUnits();
	if (getStartingWorkerUnits() == iDefault) m_iStartingWorkerUnits = pClassInfo->getStartingWorkerUnits();
	if (getStartingExploreUnits() == iDefault) m_iStartingExploreUnits = pClassInfo->getStartingExploreUnits();
	if (getAIStartingUnitMultiplier() == iDefault) m_iAIStartingUnitMultiplier = pClassInfo->getAIStartingUnitMultiplier();
	if (getAIStartingDefenseUnits() == iDefault) m_iAIStartingDefenseUnits = pClassInfo->getAIStartingDefenseUnits();
	if (getAIStartingWorkerUnits() == iDefault) m_iAIStartingWorkerUnits = pClassInfo->getAIStartingWorkerUnits();
	if (getAIStartingExploreUnits() == iDefault) m_iAIStartingExploreUnits = pClassInfo->getAIStartingExploreUnits();
	if (getBarbarianInitialDefenders() == iDefault) m_iBarbarianInitialDefenders = pClassInfo->getBarbarianInitialDefenders();
	if (getAIDeclareWarProb() == iDefault) m_iAIDeclareWarProb = pClassInfo->getAIDeclareWarProb();
	if (getAIWorkRateModifier() == iDefault) m_iAIWorkRateModifier = pClassInfo->getAIWorkRateModifier();
	if (getAIGrowthPercent() == iDefault) m_iAIGrowthPercent = pClassInfo->getAIGrowthPercent();
	if (getAITrainPercent() == iDefault) m_iAITrainPercent = pClassInfo->getAITrainPercent();
	if (getAIWorldTrainPercent() == iDefault) m_iAIWorldTrainPercent = pClassInfo->getAIWorldTrainPercent();
	if (getAIConstructPercent() == iDefault) m_iAIConstructPercent = pClassInfo->getAIConstructPercent();
	if (getAIWorldConstructPercent() == iDefault) m_iAIWorldConstructPercent = pClassInfo->getAIWorldConstructPercent();
	if (getAICreatePercent() == iDefault) m_iAICreatePercent = pClassInfo->getAICreatePercent();
	if (getAIResearchPercent() == iDefault) m_iAIResearchPercent = pClassInfo->getAIResearchPercent();
	if (getAIWorldCreatePercent() == iDefault) m_iAIWorldCreatePercent = pClassInfo->getAIWorldCreatePercent();
	if (getAICivicUpkeepPercent() == iDefault) m_iAICivicUpkeepPercent = pClassInfo->getAICivicUpkeepPercent();
	if (getAIUnitCostPercent() == iDefault) m_iAIUnitCostPercent = pClassInfo->getAIUnitCostPercent();
	if (getAIUnitSupplyPercent() == iDefault) m_iAIUnitSupplyPercent = pClassInfo->getAIUnitSupplyPercent();
	if (getAIUnitUpgradePercent() == iDefault) m_iAIUnitUpgradePercent = pClassInfo->getAIUnitUpgradePercent();
	if (getAIInflationPercent() == iDefault) m_iAIInflationPercent = pClassInfo->getAIInflationPercent();
	if (getAIWarWearinessPercent() == iDefault) m_iAIWarWearinessPercent = pClassInfo->getAIWarWearinessPercent();
	if (getAIPerEraModifier() == iDefault) m_iAIPerEraModifier = pClassInfo->getAIPerEraModifier();
	if (getAIAdvancedStartPercent() == iDefault) m_iAIAdvancedStartPercent = pClassInfo->getAIAdvancedStartPercent();

	CvString szDebugBuffer;
	szDebugBuffer.Format("copyNonDefaults handicap type %s", getType());
	gDLL->logMsg("CvHandicapInfo_copyNonDefaults.log", szDebugBuffer.c_str());

	if ( pClassInfo->getNumGoodies() >= 0 )
	{
		int iNumGoodiesOld = m_iNumGoodies;
		if (GC.isXMLLogging())
		{
			szDebugBuffer.Format("Previous goody count: %i, from pClassInfo: %i", iNumGoodiesOld, pClassInfo->getNumGoodies());
			gDLL->logMsg("CvHandicapInfo_copyNonDefaults.log", szDebugBuffer.c_str());
		}

		CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
		for ( int i = 0; i < pClassInfo->getNumGoodies(); i++ )
		{
			if (!(pCurrentUnitClass->isDuplicate(getNumGoodies(), &m_piGoodies[0], pClassInfo->getGoodies(i))))
			{
				if (GC.isXMLLogging())
				{
					szDebugBuffer.Format("New goody located? Index %i, ID %i == %s", i, pClassInfo->getGoodies(i), (pClassInfo->getGoodies(i) != (int) NO_GOODY ? GC.getGoodyInfo((GoodyTypes) pClassInfo->getGoodies(i)).getType() : "NO_GOODY"));
					gDLL->logMsg("CvHandicapInfo_copyNonDefaults.log", szDebugBuffer.c_str());
				}

				if (pClassInfo->getGoodies(i) != NO_GOODY)
					m_iNumGoodies++;
			}
		}
		if (GC.isXMLLogging())
		{
			szDebugBuffer.Format("Added: %i (vs. %i before).", m_iNumGoodies - iNumGoodiesOld, iNumGoodiesOld);
			gDLL->logMsg("CvHandicapInfo_copyNonDefaults.log", szDebugBuffer.c_str());
		}

		int* m_piGoodiesTemp = new int[m_iNumGoodies];

		for ( int i = 0; i < m_iNumGoodies; i++ )
		{
			if ( i < iNumGoodiesOld )
			{
				if (GC.isXMLLogging())
				{
					szDebugBuffer.Format("* Processing goody index %i, ID %i == %s", i, pClassInfo->getGoodies(i), GC.getGoodyInfo((GoodyTypes) m_piGoodies[i]).getType());
					gDLL->logMsg("CvHandicapInfo_copyNonDefaults.log", szDebugBuffer.c_str());
				}
				m_piGoodiesTemp[i] = m_piGoodies[i];
			}
			else
			{
				if (!(pCurrentUnitClass->isDuplicate(getNumGoodies(), &m_piGoodiesTemp[0], pClassInfo->getGoodies(i))))
				{
					m_piGoodiesTemp[i] = pClassInfo->getGoodies(i);
					if (GC.isXMLLogging())
					{
						szDebugBuffer.Format("* ADDING new goody %i", pClassInfo->getGoodies(i));
						gDLL->logMsg("CvHandicapInfo_copyNonDefaults.log", szDebugBuffer.c_str());
					}
				}
			}
		}

		SAFE_DELETE_ARRAY(m_piGoodies);
		m_piGoodies = new int[m_iNumGoodies];
		if (GC.isXMLLogging())
		{
			szDebugBuffer.Format("=== %s: %i goodies ===", getType(), m_iNumGoodies);
			gDLL->logMsg("CvHandicapInfo_copyNonDefaults.log", szDebugBuffer.c_str());

			for (int i = 0; i < m_iNumGoodies; i++)
			{
				szDebugBuffer.Format("* Goody %i: %s", i, (m_piGoodiesTemp[i] != (int) NO_GOODY ? GC.getGoodyInfo((GoodyTypes) m_piGoodiesTemp[i]).getType() : "NO_GOODY"));
				gDLL->logMsg("CvHandicapInfo_copyNonDefaults.log", szDebugBuffer.c_str());
			}
		}

		for ( int i = 0; i < m_iNumGoodies; i++ )
		{
			m_piGoodies[i] = m_piGoodiesTemp[i];
		}
		SAFE_DELETE_ARRAY(m_piGoodiesTemp);
		SAFE_DELETE(pCurrentUnitClass);
	}

	for ( int i = 0; i < GC.getNumTechInfos(); i++)
	{
		if ( isFreeTechs(i) == (int)bDefault && pClassInfo->isFreeTechs(i) != (int)bDefault)
		{
			if ( NULL == m_pbFreeTechs )
			{
				CvXMLLoadUtility::InitList(&m_pbFreeTechs,GC.getNumTechInfos(),bDefault);
			}
			m_pbFreeTechs[i] = pClassInfo->isFreeTechs(i);
		}

		if ( isAIFreeTechs(i) == (int)bDefault && pClassInfo->isAIFreeTechs(i) != (int)bDefault)
		{
			if ( NULL == m_pbAIFreeTechs )
			{
				CvXMLLoadUtility::InitList(&m_pbAIFreeTechs,GC.getNumTechInfos(),bDefault);
			}
			m_pbAIFreeTechs[i] = pClassInfo->isAIFreeTechs(i);
		}
	}
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getRevolutionIndexPercent() == iDefault) m_iRevolutionIndexPercent = pClassInfo->getRevolutionIndexPercent();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	m_Percent.copyNonDefaults(&(pClassInfo->m_Percent), pXML);

	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);
}

//======================================================================================================
//					CvGameSpeedInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvGameSpeedInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvGameSpeedInfo::CvGameSpeedInfo() :
m_iGrowthPercent(0),
m_iTrainPercent(0),
m_iConstructPercent(0),
m_iCreatePercent(0),
m_iResearchPercent(0),
m_iBuildPercent(0),
m_iImprovementPercent(0),
m_iGreatPeoplePercent(0),
m_iAnarchyPercent(0),
m_iBarbPercent(0),
m_iFeatureProductionPercent(0),
m_iUnitDiscoverPercent(0),
m_iUnitHurryPercent(0),
m_iUnitTradePercent(0),
m_iUnitGreatWorkPercent(0),
m_iGoldenAgePercent(0),
m_iHurryPercent(0),
m_iHurryConscriptAngerPercent(0),
m_iInflationOffset(0),
m_iInflationPercent(0),
//ls612: Begin
m_iGoldModifier(0),
m_iOccupationTimePopulationPercent(0),
//ls612: End
m_iVictoryDelayPercent(0),
m_iNumTurnIncrements(0),
m_pGameTurnInfo(NULL),
m_bEndDatesCalculated(false)
/************************************************************************************************/
/* Afforess					  Start		 12/13/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iUnitMovementPercent(0)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
//TB GameSpeed begin
,m_iTraitGainPercent(0)
//TB GameSpeed end
,m_Percent()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvGameSpeedInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvGameSpeedInfo::~CvGameSpeedInfo()
{
	SAFE_DELETE_ARRAY(m_pGameTurnInfo);
}

int CvGameSpeedInfo::getGrowthPercent() const
{
	return m_iGrowthPercent;
}

int CvGameSpeedInfo::getTrainPercent() const
{
	return m_iTrainPercent;
}

int CvGameSpeedInfo::getConstructPercent() const
{
	return m_iConstructPercent;
}

int CvGameSpeedInfo::getCreatePercent() const
{
	return m_iCreatePercent;
}

int CvGameSpeedInfo::getResearchPercent() const
{
	return m_iResearchPercent;
}

int CvGameSpeedInfo::getBuildPercent() const
{
	return m_iBuildPercent;
}

int CvGameSpeedInfo::getImprovementPercent() const
{
	return m_iImprovementPercent;
}

int CvGameSpeedInfo::getGreatPeoplePercent() const
{
	return m_iGreatPeoplePercent;
}

int CvGameSpeedInfo::getAnarchyPercent() const
{
	return m_iAnarchyPercent;
}

int CvGameSpeedInfo::getBarbPercent() const
{
	return m_iBarbPercent;
}

int CvGameSpeedInfo::getFeatureProductionPercent() const
{
	return m_iFeatureProductionPercent;
}

int CvGameSpeedInfo::getUnitDiscoverPercent() const
{
	return m_iUnitDiscoverPercent;
}

int CvGameSpeedInfo::getUnitHurryPercent() const
{
	return m_iUnitHurryPercent;
}

int CvGameSpeedInfo::getUnitTradePercent() const
{
	return m_iUnitTradePercent;
}

int CvGameSpeedInfo::getUnitGreatWorkPercent() const
{
	return m_iUnitGreatWorkPercent;
}

int CvGameSpeedInfo::getGoldenAgePercent() const
{
	return m_iGoldenAgePercent;
}

int CvGameSpeedInfo::getHurryPercent() const
{
	return m_iHurryPercent;
}

int CvGameSpeedInfo::getHurryConscriptAngerPercent() const
{
	return m_iHurryConscriptAngerPercent;
}

int CvGameSpeedInfo::getInflationOffset() const
{
	return m_iInflationOffset;
}

int CvGameSpeedInfo::getInflationPercent() const
{
	return m_iInflationPercent;
}

//ls612: Begin
int CvGameSpeedInfo::getGoldModifier() const
{
	return m_iGoldModifier;
}

int CvGameSpeedInfo::getOccupationTimePopulationPercent() const
{
	return m_iOccupationTimePopulationPercent;
}
//ls612: End

int CvGameSpeedInfo::getVictoryDelayPercent() const
{
	return m_iVictoryDelayPercent;
}

int CvGameSpeedInfo::getNumTurnIncrements() const
{
	return m_iNumTurnIncrements;
}

/************************************************************************************************/
/* Afforess					  Start		 12/13/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
int CvGameSpeedInfo::getUnitMovementPercent() const
{
	return m_iUnitMovementPercent;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

CvDateIncrement& CvGameSpeedInfo::getDateIncrement(int iIndex)
{
	return m_aIncrements[iIndex];
}

std::vector<CvDateIncrement>& CvGameSpeedInfo::getIncrements()
{
	return m_aIncrements;
}

bool CvGameSpeedInfo::getEndDatesCalculated()
{
	return m_bEndDatesCalculated;
}

void CvGameSpeedInfo::setEndDatesCalculated(bool bCalculated)
{
	m_bEndDatesCalculated = bCalculated;
}

GameTurnInfo& CvGameSpeedInfo::getGameTurnInfo(int iIndex) const
{
	return m_pGameTurnInfo[iIndex];
}

void CvGameSpeedInfo::allocateGameTurnInfos(const int iSize)
{
	m_pGameTurnInfo = new GameTurnInfo[iSize];
}

//TB GameSpeed begin
int CvGameSpeedInfo::getTraitGainPercent() const
{
	return m_iTraitGainPercent;
}
//TB GameSpeed end

int CvGameSpeedInfo::getPercent(int iID) const
{
	return m_Percent.getValue(iID);
}

bool CvGameSpeedInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();
	//				OutputDebugStringW(pXML->GetXmlTagName());

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int j, iTempVal;

	pXML->GetOptionalChildXmlValByName(&m_iGrowthPercent, L"iGrowthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iTrainPercent, L"iTrainPercent");
	pXML->GetOptionalChildXmlValByName(&m_iConstructPercent, L"iConstructPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCreatePercent, L"iCreatePercent");
	pXML->GetOptionalChildXmlValByName(&m_iResearchPercent, L"iResearchPercent");
	pXML->GetOptionalChildXmlValByName(&m_iBuildPercent, L"iBuildPercent");
	pXML->GetOptionalChildXmlValByName(&m_iImprovementPercent, L"iImprovementPercent");
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeoplePercent, L"iGreatPeoplePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAnarchyPercent, L"iAnarchyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iBarbPercent, L"iBarbPercent");
	pXML->GetOptionalChildXmlValByName(&m_iFeatureProductionPercent, L"iFeatureProductionPercent");
	pXML->GetOptionalChildXmlValByName(&m_iUnitDiscoverPercent, L"iUnitDiscoverPercent");
	pXML->GetOptionalChildXmlValByName(&m_iUnitHurryPercent, L"iUnitHurryPercent");
	pXML->GetOptionalChildXmlValByName(&m_iUnitTradePercent, L"iUnitTradePercent");
	pXML->GetOptionalChildXmlValByName(&m_iUnitGreatWorkPercent, L"iUnitGreatWorkPercent");
	pXML->GetOptionalChildXmlValByName(&m_iGoldenAgePercent, L"iGoldenAgePercent");
	pXML->GetOptionalChildXmlValByName(&m_iHurryPercent, L"iHurryPercent");
	pXML->GetOptionalChildXmlValByName(&m_iHurryConscriptAngerPercent, L"iHurryConscriptAngerPercent");
	pXML->GetOptionalChildXmlValByName(&m_iInflationOffset, L"iInflationOffset");
	pXML->GetOptionalChildXmlValByName(&m_iInflationPercent, L"iInflationPercent");
	//ls612: Begin
	pXML->GetOptionalChildXmlValByName(&m_iGoldModifier, L"iGoldModifier");
	pXML->GetOptionalChildXmlValByName(&m_iOccupationTimePopulationPercent, L"iOccupationTurnsPopulationPercent");
	//ls612: End
	pXML->GetOptionalChildXmlValByName(&m_iVictoryDelayPercent, L"iVictoryDelayPercent");

	if (pXML->TryMoveToXmlFirstChild(L"GameTurnInfos"))
	{
		m_bEndDatesCalculated = false;
		m_iNumTurnIncrements = pXML->GetXmlChildrenNumber(L"GameTurnInfo");
		char szLog[1000];
		sprintf(szLog, ":: %i", m_iNumTurnIncrements);
		OutputDebugString(szLog);
		if (pXML->TryMoveToXmlFirstChild(L"GameTurnInfo"))
		{
			allocateGameTurnInfos(getNumTurnIncrements());

			// loop through each tag
			for (j=0;j<getNumTurnIncrements();j++)
			{
				CvDateIncrement inc;
				pXML->GetOptionalChildXmlValByName(&iTempVal, L"iMonthIncrement");
				getGameTurnInfo(j).iMonthIncrement = iTempVal;
				inc.m_iIncrementMonth = iTempVal;
				pXML->GetOptionalChildXmlValByName(&iTempVal, L"iTurnsPerIncrement");
				getGameTurnInfo(j).iNumGameTurnsPerIncrement = iTempVal;
				inc.m_iendTurn = iTempVal + (m_aIncrements.empty() ? 0 : m_aIncrements[m_aIncrements.size()-1].m_iendTurn);
				pXML->GetOptionalChildXmlValByName(&iTempVal, L"iDayIncrement");
				inc.m_iIncrementDay = iTempVal;
				m_aIncrements.push_back(inc);

				// if we cannot set the current xml node to it's next sibling then we will break out of the for loop
				// otherwise we will continue looping
				if (!pXML->TryMoveToXmlNextSibling(L"GameTurnInfo"))
				{
					break;
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

/************************************************************************************************/
/* Afforess					  Start		 12/13/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iUnitMovementPercent, L"iUnitMovementPercent");
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	//TB GameSpeed begin
	pXML->GetOptionalChildXmlValByName(&m_iTraitGainPercent, L"iTraitGainPercent");
	//TB GameSpeed end

	if (pXML->TryMoveToXmlFirstChild(L"Percents"))
	{
		m_Percent.read(pXML);

		pXML->MoveToXmlParent();
	}

	return true;
}

void CvGameSpeedInfo::copyNonDefaults(CvGameSpeedInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getGrowthPercent() == iDefault) m_iGrowthPercent = pClassInfo->getGrowthPercent();
	if (getTrainPercent() == iDefault) m_iTrainPercent = pClassInfo->getTrainPercent();
	if (getConstructPercent() == iDefault) m_iConstructPercent = pClassInfo->getConstructPercent();
	if (getCreatePercent() == iDefault) m_iCreatePercent = pClassInfo->getCreatePercent();
	if (getResearchPercent() == iDefault) m_iResearchPercent = pClassInfo->getResearchPercent();
	if (getBuildPercent() == iDefault) m_iBuildPercent = pClassInfo->getBuildPercent();
	if (getImprovementPercent() == iDefault) m_iImprovementPercent = pClassInfo->getImprovementPercent();
	if (getGreatPeoplePercent() == iDefault) m_iGreatPeoplePercent = pClassInfo->getGreatPeoplePercent();
	if (getAnarchyPercent() == iDefault) m_iAnarchyPercent = pClassInfo->getAnarchyPercent();
	if (getBarbPercent() == iDefault) m_iBarbPercent = pClassInfo->getBarbPercent();
	if (getFeatureProductionPercent() == iDefault) m_iFeatureProductionPercent = pClassInfo->getFeatureProductionPercent();
	if (getUnitDiscoverPercent() == iDefault) m_iUnitDiscoverPercent = pClassInfo->getUnitDiscoverPercent();
	if (getUnitHurryPercent() == iDefault) m_iUnitHurryPercent = pClassInfo->getUnitHurryPercent();
	if (getUnitTradePercent() == iDefault) m_iUnitTradePercent = pClassInfo->getUnitTradePercent();
	if (getUnitGreatWorkPercent() == iDefault) m_iUnitGreatWorkPercent = pClassInfo->getUnitGreatWorkPercent();
	if (getGoldenAgePercent() == iDefault) m_iGoldenAgePercent = pClassInfo->getGoldenAgePercent();
	if (getHurryPercent() == iDefault) m_iHurryPercent = pClassInfo->getHurryPercent();
	if (getHurryConscriptAngerPercent() == iDefault) m_iHurryConscriptAngerPercent = pClassInfo->getHurryConscriptAngerPercent();
	if (getInflationOffset() == iDefault) m_iInflationOffset = pClassInfo->getInflationOffset();
	if (getInflationPercent() == iDefault) m_iInflationPercent = pClassInfo->getInflationPercent();
	//ls612: Begin
	if (getGoldModifier() == iDefault) m_iGoldModifier = pClassInfo->getGoldModifier();
	if (getOccupationTimePopulationPercent() == iDefault) m_iOccupationTimePopulationPercent = pClassInfo->getOccupationTimePopulationPercent();
	//ls612: End
	if (getVictoryDelayPercent() == iDefault) m_iVictoryDelayPercent = pClassInfo->getVictoryDelayPercent();

	if (getNumTurnIncrements() == iDefault)
	{
		m_iNumTurnIncrements = pClassInfo->getNumTurnIncrements();
		allocateGameTurnInfos(getNumTurnIncrements());
		// loop through each tag
		for (int j = 0; j < getNumTurnIncrements(); j++)
		{
			m_pGameTurnInfo[j] = pClassInfo->getGameTurnInfo(j);
			m_aIncrements[j] = pClassInfo->getDateIncrement(j);
		}
	}
	//TB GameSpeed begin
	if (getTraitGainPercent() == iDefault) m_iTraitGainPercent = pClassInfo->getTraitGainPercent();
	//TB GameSpeed end

	m_Percent.copyNonDefaults(&(pClassInfo->m_Percent), pXML);
}

void CvGameSpeedInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iGrowthPercent);
	CheckSum(iSum, m_iTrainPercent);
	CheckSum(iSum, m_iConstructPercent);
	CheckSum(iSum, m_iCreatePercent);
	CheckSum(iSum, m_iResearchPercent);
	CheckSum(iSum, m_iBuildPercent);
	CheckSum(iSum, m_iImprovementPercent);
	CheckSum(iSum, m_iGreatPeoplePercent);
	CheckSum(iSum, m_iAnarchyPercent);
	CheckSum(iSum, m_iBarbPercent);
	CheckSum(iSum, m_iFeatureProductionPercent);
	CheckSum(iSum, m_iUnitDiscoverPercent);
	CheckSum(iSum, m_iUnitHurryPercent);
	CheckSum(iSum, m_iUnitTradePercent);
	CheckSum(iSum, m_iUnitGreatWorkPercent);
	CheckSum(iSum, m_iGoldenAgePercent);
	CheckSum(iSum, m_iHurryPercent);
	CheckSum(iSum, m_iHurryConscriptAngerPercent);
	CheckSum(iSum, m_iInflationOffset);
	CheckSum(iSum, m_iInflationPercent);
	//ls612: Begin
	CheckSum(iSum, m_iGoldModifier);
	CheckSum(iSum, m_iOccupationTimePopulationPercent);
	//ls612: End
	CheckSum(iSum, m_iVictoryDelayPercent);

	for (int j = 0; j < m_iNumTurnIncrements; j++)
	{
		CheckSum(iSum, m_pGameTurnInfo[j].iMonthIncrement);
		CheckSum(iSum, m_pGameTurnInfo[j].iNumGameTurnsPerIncrement);
		CheckSum(iSum, m_aIncrements[j].m_iIncrementDay);
	}

	//TB GameSpeed begin
	CheckSum(iSum, m_iTraitGainPercent);
	//TB GameSpeed end

	m_Percent.getCheckSum(iSum);
}


//======================================================================================================
//					CvTurnTimerInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTurnTimerInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTurnTimerInfo::CvTurnTimerInfo() :
m_iBaseTime(0),
m_iCityBonus(0),
m_iUnitBonus(0),
m_iFirstTurnMultiplier(0)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTurnTimerInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTurnTimerInfo::~CvTurnTimerInfo()
{
}

int CvTurnTimerInfo::getBaseTime() const
{
	return m_iBaseTime;
}

int CvTurnTimerInfo::getCityBonus() const
{
	return m_iCityBonus;
}

int CvTurnTimerInfo::getUnitBonus() const
{
	return m_iUnitBonus;
}

int CvTurnTimerInfo::getFirstTurnMultiplier() const
{
	return m_iFirstTurnMultiplier;
}

bool CvTurnTimerInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iBaseTime, L"iBaseTime");
	pXML->GetOptionalChildXmlValByName(&m_iCityBonus, L"iCityBonus");
	pXML->GetOptionalChildXmlValByName(&m_iUnitBonus, L"iUnitBonus");
	pXML->GetOptionalChildXmlValByName(&m_iFirstTurnMultiplier, L"iFirstTurnMultiplier");

	return true;
}

void CvTurnTimerInfo::copyNonDefaults(CvTurnTimerInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getBaseTime() == iDefault) m_iBaseTime = pClassInfo->getBaseTime();
	if (getCityBonus() == iDefault) m_iCityBonus = pClassInfo->getCityBonus();
	if (getUnitBonus() == iDefault) m_iUnitBonus = pClassInfo->getUnitBonus();
	if (getFirstTurnMultiplier() == iDefault) m_iFirstTurnMultiplier = pClassInfo->getFirstTurnMultiplier();
}

void CvTurnTimerInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iBaseTime);
	CheckSum(iSum, m_iCityBonus);
	CheckSum(iSum, m_iUnitBonus);
	CheckSum(iSum, m_iFirstTurnMultiplier);
}

//======================================================================================================
//					CvWorldInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvWorldInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvWorldInfo::CvWorldInfo() :
m_iDefaultPlayers(0),
m_iUnitNameModifier(0),
m_iTargetNumCities(0),
m_iNumFreeBuildingBonuses(0),
m_iBuildingClassPrereqModifier(0),
m_iMaxConscriptModifier(0),
m_iWarWearinessModifier(0),
m_iGridWidth(0),
m_iGridHeight(0),
m_iTerrainGrainChange(0),
m_iFeatureGrainChange(0),
m_iResearchPercent(0),
m_iTradeProfitPercent(0),
m_iDistanceMaintenancePercent(0),
m_iNumCitiesMaintenancePercent(0),
m_iColonyMaintenancePercent(0),
m_iCorporationMaintenancePercent(0),
m_iNumCitiesAnarchyPercent(0),
m_iAdvancedStartPointsMod(0)
/************************************************************************************************/
/* Afforess					  Start		 02/27/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iCommandersLevelThresholdsPercent(0)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
,m_Percent()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvWorldInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvWorldInfo::~CvWorldInfo()
{
}

int CvWorldInfo::getDefaultPlayers() const
{
	return m_iDefaultPlayers;
}

int CvWorldInfo::getUnitNameModifier() const
{
	return m_iUnitNameModifier;
}

int CvWorldInfo::getTargetNumCities() const
{
	return m_iTargetNumCities;
}

int CvWorldInfo::getNumFreeBuildingBonuses() const
{
	return m_iNumFreeBuildingBonuses;
}

int CvWorldInfo::getBuildingClassPrereqModifier() const
{
	return m_iBuildingClassPrereqModifier;
}

int CvWorldInfo::getMaxConscriptModifier() const
{
	return m_iMaxConscriptModifier;
}

int CvWorldInfo::getWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}

int CvWorldInfo::getGridWidth() const
{
	return m_iGridWidth;
}

int CvWorldInfo::getGridHeight() const
{
	return m_iGridHeight;
}

int CvWorldInfo::getTerrainGrainChange() const
{
	return m_iTerrainGrainChange;
}

int CvWorldInfo::getFeatureGrainChange() const
{
	return m_iFeatureGrainChange;
}

int CvWorldInfo::getResearchPercent() const
{
	return m_iResearchPercent;
}

int CvWorldInfo::getTradeProfitPercent() const
{
	return m_iTradeProfitPercent;
}

int CvWorldInfo::getDistanceMaintenancePercent() const
{
	return m_iDistanceMaintenancePercent;
}

int CvWorldInfo::getNumCitiesMaintenancePercent() const
{
	return m_iNumCitiesMaintenancePercent;
}

int CvWorldInfo::getColonyMaintenancePercent() const
{
	return m_iColonyMaintenancePercent;
}

int CvWorldInfo::getCorporationMaintenancePercent() const
{
	return m_iCorporationMaintenancePercent;
}

int CvWorldInfo::getNumCitiesAnarchyPercent() const
{
	return m_iNumCitiesAnarchyPercent;
}

int CvWorldInfo::getAdvancedStartPointsMod() const
{
	return m_iAdvancedStartPointsMod;
}

/************************************************************************************************/
/* Afforess					  Start		 02/27/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
int CvWorldInfo::getCommandersLevelThresholdsPercent() const
{
	return m_iCommandersLevelThresholdsPercent;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

int CvWorldInfo::getPercent(int iID) const
{
	return m_Percent.getValue(iID);
}

bool CvWorldInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iDefaultPlayers, L"iDefaultPlayers");
	pXML->GetOptionalChildXmlValByName(&m_iUnitNameModifier, L"iUnitNameModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTargetNumCities, L"iTargetNumCities");
	pXML->GetOptionalChildXmlValByName(&m_iNumFreeBuildingBonuses, L"iNumFreeBuildingBonuses");
	pXML->GetOptionalChildXmlValByName(&m_iBuildingClassPrereqModifier, L"iBuildingClassPrereqModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxConscriptModifier, L"iMaxConscriptModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessModifier, L"iWarWearinessModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGridWidth, L"iGridWidth");
	pXML->GetOptionalChildXmlValByName(&m_iGridHeight, L"iGridHeight");
	pXML->GetOptionalChildXmlValByName(&m_iTerrainGrainChange, L"iTerrainGrainChange");
	pXML->GetOptionalChildXmlValByName(&m_iFeatureGrainChange, L"iFeatureGrainChange");
	pXML->GetOptionalChildXmlValByName(&m_iResearchPercent, L"iResearchPercent");
	pXML->GetOptionalChildXmlValByName(&m_iTradeProfitPercent, L"iTradeProfitPercent");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenancePercent, L"iDistanceMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenancePercent, L"iNumCitiesMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iColonyMaintenancePercent, L"iColonyMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenancePercent, L"iCorporationMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesAnarchyPercent, L"iNumCitiesAnarchyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartPointsMod, L"iAdvancedStartPointsMod");
/************************************************************************************************/
/* Afforess					  Start		 02/27/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iCommandersLevelThresholdsPercent, L"iCommandersLevelThresholdsPercent");
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	if (pXML->TryMoveToXmlFirstChild(L"Percents"))
	{
		m_Percent.read(pXML);

		pXML->MoveToXmlParent();
	}

	return true;
}

void CvWorldInfo::copyNonDefaults(CvWorldInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getDefaultPlayers() == iDefault) m_iDefaultPlayers = pClassInfo->getDefaultPlayers();
	if (getUnitNameModifier() == iDefault) m_iUnitNameModifier = pClassInfo->getUnitNameModifier();
	if (getTargetNumCities() == iDefault) m_iTargetNumCities = pClassInfo->getTargetNumCities();
	if (getNumFreeBuildingBonuses() == iDefault) m_iNumFreeBuildingBonuses = pClassInfo->getNumFreeBuildingBonuses();
	if (getBuildingClassPrereqModifier() == iDefault) m_iBuildingClassPrereqModifier = pClassInfo->getBuildingClassPrereqModifier();
	if (getMaxConscriptModifier() == iDefault) m_iMaxConscriptModifier = pClassInfo->getMaxConscriptModifier();
	if (getWarWearinessModifier() == iDefault) m_iWarWearinessModifier = pClassInfo->getWarWearinessModifier();
	if (getGridWidth() == iDefault) m_iGridWidth = pClassInfo->getGridWidth();
	if (getGridHeight() == iDefault) m_iGridHeight = pClassInfo->getGridHeight();
	if (getTerrainGrainChange() == iDefault) m_iTerrainGrainChange = pClassInfo->getTerrainGrainChange();
	if (getFeatureGrainChange() == iDefault) m_iFeatureGrainChange = pClassInfo->getFeatureGrainChange();
	if (getResearchPercent() == iDefault) m_iResearchPercent = pClassInfo->getResearchPercent();
	if (getTradeProfitPercent() == iDefault) m_iTradeProfitPercent = pClassInfo->getTradeProfitPercent();
	if (getDistanceMaintenancePercent() == iDefault) m_iDistanceMaintenancePercent = pClassInfo->getDistanceMaintenancePercent();
	if (getNumCitiesMaintenancePercent() == iDefault) m_iNumCitiesMaintenancePercent = pClassInfo->getNumCitiesMaintenancePercent();
	if (getColonyMaintenancePercent() == iDefault) m_iColonyMaintenancePercent = pClassInfo->getColonyMaintenancePercent();
	if (getCorporationMaintenancePercent() == iDefault) m_iCorporationMaintenancePercent = pClassInfo->getCorporationMaintenancePercent();
	if (getNumCitiesAnarchyPercent() == iDefault) m_iNumCitiesAnarchyPercent = pClassInfo->getNumCitiesAnarchyPercent();
	if (getAdvancedStartPointsMod() == iDefault) m_iAdvancedStartPointsMod = pClassInfo->getAdvancedStartPointsMod();
/************************************************************************************************/
/* Afforess					  Start		 02/27/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getCommandersLevelThresholdsPercent() == iDefault) m_iCommandersLevelThresholdsPercent = pClassInfo->getCommandersLevelThresholdsPercent();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	m_Percent.copyNonDefaults(&(pClassInfo->m_Percent), pXML);

}

void CvWorldInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iDefaultPlayers);
	CheckSum(iSum, m_iUnitNameModifier);
	CheckSum(iSum, m_iTargetNumCities);
	CheckSum(iSum, m_iNumFreeBuildingBonuses);
	CheckSum(iSum, m_iBuildingClassPrereqModifier);
	CheckSum(iSum, m_iMaxConscriptModifier);
	CheckSum(iSum, m_iWarWearinessModifier);
	CheckSum(iSum, m_iGridWidth);
	CheckSum(iSum, m_iGridHeight);
	CheckSum(iSum, m_iTerrainGrainChange);
	CheckSum(iSum, m_iFeatureGrainChange);
	CheckSum(iSum, m_iResearchPercent);
	CheckSum(iSum, m_iTradeProfitPercent);
	CheckSum(iSum, m_iDistanceMaintenancePercent);
	CheckSum(iSum, m_iNumCitiesMaintenancePercent);
	CheckSum(iSum, m_iColonyMaintenancePercent);
	CheckSum(iSum, m_iCorporationMaintenancePercent);
	CheckSum(iSum, m_iNumCitiesAnarchyPercent);
	CheckSum(iSum, m_iAdvancedStartPointsMod);

	CheckSum(iSum, m_iCommandersLevelThresholdsPercent);

	m_Percent.getCheckSum(iSum);
}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/

//======================================================================================================
//					CvMapInfo
//======================================================================================================

CvMapInfo::CvMapInfo() :
m_iGridWidth(0),
m_iGridHeight(0),
m_iWrapX(-1),
m_iWrapY(-1),
m_szInitialWBMap(NULL),
m_szMapScript(NULL),
//m_pabTechShareMaps(NULL),
m_szStartingEvent(NULL),
m_iSwitchType(0)
{
}

CvMapInfo::~CvMapInfo()
{
}

int CvMapInfo::getGridWidth() const
{
	return m_iGridWidth;
}

int CvMapInfo::getGridHeight() const
{
	return m_iGridHeight;
}

int CvMapInfo::getWrapX() const
{
	return m_iWrapX;
}

int CvMapInfo::getWrapY() const
{
	return m_iWrapY;
}

const CvString CvMapInfo::getInitialWBMap() const
{
	return m_szInitialWBMap;
}

const CvString CvMapInfo::getMapScript() const
{
	return m_szMapScript;
}

//bool CvMapInfo::isTechShareMap(MapTypes eMap) const
//{
//	return m_pabTechShareMaps[eMap];
//}

const TCHAR* CvMapInfo::getStartingEvent() const
{
	return m_szStartingEvent;
}

int CvMapInfo::getSwitchType() const
{
	return m_iSwitchType;
}

bool CvMapInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iGridWidth, L"iGridWidth");
	pXML->GetOptionalChildXmlValByName(&m_iGridHeight, L"iGridHeight");
	pXML->GetOptionalChildXmlValByName(&m_iWrapX, L"bWrapX");
	pXML->GetOptionalChildXmlValByName(&m_iWrapY, L"bWrapY");
	pXML->GetOptionalChildXmlValByName(m_szInitialWBMap, L"InitialWBMap");
	pXML->GetOptionalChildXmlValByName(m_szMapScript, L"MapScript");
	pXML->GetOptionalChildXmlValByName(m_szStartingEvent, L"StartingEvent");

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"SwitchType");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	return true;
}

bool CvMapInfo::readPass2(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	//pXML->SetVariableListTagPair(&m_pabTechShareMaps, L"SharedTechs", sizeof(CvMapInfo), GC.getNumMapInfos());

	return true;
}

bool CvMapInfo::readPass3()
{
	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}

	if (m_aszExtraXMLforPass3[0].length() > 0)
		m_iSwitchType = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);
	else
		m_iSwitchType = NO_MAPSWITCH;

	m_aszExtraXMLforPass3.clear();

	return true;
}

//======================================================================================================
//					CvMapSwitchInfo
//======================================================================================================

CvMapSwitchInfo::CvMapSwitchInfo() :
m_iMap(NO_MAP),
m_szEvent(NULL),
m_bManual(false)
{
}

CvMapSwitchInfo::~CvMapSwitchInfo()
{
}

int CvMapSwitchInfo::getMap() const
{
	return m_iMap;
}

const TCHAR* CvMapSwitchInfo::getEvent() const
{
	return m_szEvent;
}

bool CvMapSwitchInfo::isManual() const
{
	return m_bManual;
}

bool CvMapSwitchInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Map");
	m_iMap = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(m_szEvent, L"SwitchEvent");
	pXML->GetOptionalChildXmlValByName(&m_bManual, L"bManual");

	return true;
}

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

//======================================================================================================
//					CvClimateInfo
//======================================================================================================
CvClimateInfo::CvClimateInfo() :
m_iDesertPercentChange(0),
m_iJungleLatitude(0),
m_iHillRange(0),
m_iPeakPercent(0),
m_fSnowLatitudeChange(0.0f),
m_fTundraLatitudeChange(0.0f),
m_fGrassLatitudeChange(0.0f),
m_fDesertBottomLatitudeChange(0.0f),
m_fDesertTopLatitudeChange(0.0f),
m_fIceLatitude(0.0f),
m_fRandIceLatitude(0.0f)
{
}

CvClimateInfo::~CvClimateInfo()
{
}

int CvClimateInfo::getDesertPercentChange() const
{
	return m_iDesertPercentChange;
}

int CvClimateInfo::getJungleLatitude() const
{
	return m_iJungleLatitude;
}

int CvClimateInfo::getHillRange() const
{
	return m_iHillRange;
}

int CvClimateInfo::getPeakPercent() const
{
	return m_iPeakPercent;
}

float CvClimateInfo::getSnowLatitudeChange() const
{
	return m_fSnowLatitudeChange;
}

float CvClimateInfo::getTundraLatitudeChange() const
{
	return m_fTundraLatitudeChange;
}

float CvClimateInfo::getGrassLatitudeChange() const
{
	return m_fGrassLatitudeChange;
}

float CvClimateInfo::getDesertBottomLatitudeChange() const
{
	return m_fDesertBottomLatitudeChange;
}

float CvClimateInfo::getDesertTopLatitudeChange() const
{
	return m_fDesertTopLatitudeChange;
}

float CvClimateInfo::getIceLatitude() const
{
	return m_fIceLatitude;
}

float CvClimateInfo::getRandIceLatitude() const
{
	return m_fRandIceLatitude;
}

bool CvClimateInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iDesertPercentChange, L"iDesertPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iJungleLatitude, L"iJungleLatitude");
	pXML->GetOptionalChildXmlValByName(&m_iHillRange, L"iHillRange");
	pXML->GetOptionalChildXmlValByName(&m_iPeakPercent, L"iPeakPercent");

	pXML->GetOptionalChildXmlValByName(&m_fSnowLatitudeChange, L"fSnowLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fTundraLatitudeChange, L"fTundraLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fGrassLatitudeChange, L"fGrassLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fDesertBottomLatitudeChange, L"fDesertBottomLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fDesertTopLatitudeChange, L"fDesertTopLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fIceLatitude, L"fIceLatitude");
	pXML->GetOptionalChildXmlValByName(&m_fRandIceLatitude, L"fRandIceLatitude");

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvClimateInfo::copyNonDefaults(CvClimateInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getDesertPercentChange() == iDefault) m_iDesertPercentChange = pClassInfo->getDesertPercentChange();
	if (getJungleLatitude() == iDefault) m_iJungleLatitude = pClassInfo->getJungleLatitude();
	if (getHillRange() == iDefault) m_iHillRange = pClassInfo->getHillRange();
	if (getPeakPercent() == iDefault) m_iPeakPercent = pClassInfo->getPeakPercent();

	if (getSnowLatitudeChange() == fDefault) m_fSnowLatitudeChange = pClassInfo->getSnowLatitudeChange();
	if (getTundraLatitudeChange() == fDefault) m_fTundraLatitudeChange = pClassInfo->getTundraLatitudeChange();
	if (getGrassLatitudeChange() == fDefault) m_fGrassLatitudeChange = pClassInfo->getGrassLatitudeChange();
	if (getDesertBottomLatitudeChange() == fDefault) m_fDesertBottomLatitudeChange = pClassInfo->getDesertBottomLatitudeChange();
	if (getDesertTopLatitudeChange() == fDefault) m_fDesertTopLatitudeChange = pClassInfo->getDesertTopLatitudeChange();
	if (getIceLatitude() == fDefault) m_fIceLatitude = pClassInfo->getIceLatitude();
	if (getRandIceLatitude() == fDefault) m_fRandIceLatitude = pClassInfo->getRandIceLatitude();
}

//======================================================================================================
//					CvSeaLevelInfo
//======================================================================================================
CvSeaLevelInfo::CvSeaLevelInfo() :
m_iSeaLevelChange(0)
{
}

CvSeaLevelInfo::~CvSeaLevelInfo()
{
}

int CvSeaLevelInfo::getSeaLevelChange() const
{
	return m_iSeaLevelChange;
}

bool CvSeaLevelInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iSeaLevelChange, L"iSeaLevelChange");

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvSeaLevelInfo::copyNonDefaults(CvSeaLevelInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getSeaLevelChange() == iDefault) m_iSeaLevelChange = pClassInfo->getSeaLevelChange();
}

//======================================================================================================
//					CvVoteInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvVoteInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvVoteInfo::CvVoteInfo() :
m_iPopulationThreshold(0),
m_iStateReligionVotePercent(0),
m_iTradeRoutes(0),
m_iMinVoters(0),
m_bSecretaryGeneral(false),
m_bVictory(false),
m_bFreeTrade(false),
m_bNoNukes(false),
m_bCityVoting(false),
m_bCivVoting(false),
m_bDefensivePact(false),
m_bOpenBorders(false),
m_bForcePeace(false),
m_bForceNoTrade(false),
m_bForceWar(false),
m_bAssignCity(false),
m_pbForceCivic(NULL),
m_abVoteSourceTypes(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvVoteInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvVoteInfo::~CvVoteInfo()
{
	SAFE_DELETE_ARRAY(m_pbForceCivic);
	SAFE_DELETE_ARRAY(m_abVoteSourceTypes);
}

int CvVoteInfo::getPopulationThreshold() const
{
	return m_iPopulationThreshold;
}

int CvVoteInfo::getStateReligionVotePercent() const
{
	return m_iStateReligionVotePercent;
}

int CvVoteInfo::getTradeRoutes() const
{
	return m_iTradeRoutes;
}

int CvVoteInfo::getMinVoters() const
{
	return m_iMinVoters;
}

bool CvVoteInfo::isSecretaryGeneral() const
{
	return m_bSecretaryGeneral;
}

bool CvVoteInfo::isVictory() const
{
	return m_bVictory;
}

bool CvVoteInfo::isFreeTrade() const
{
	return m_bFreeTrade;
}

bool CvVoteInfo::isNoNukes() const
{
	return m_bNoNukes;
}

bool CvVoteInfo::isCityVoting() const
{
	return m_bCityVoting;
}

bool CvVoteInfo::isCivVoting() const
{
	return m_bCivVoting;
}

bool CvVoteInfo::isDefensivePact() const
{
	return m_bDefensivePact;
}

bool CvVoteInfo::isOpenBorders() const
{
	return m_bOpenBorders;
}

bool CvVoteInfo::isForcePeace() const
{
	return m_bForcePeace;
}

bool CvVoteInfo::isForceNoTrade() const
{
	return m_bForceNoTrade;
}

bool CvVoteInfo::isForceWar() const
{
	return m_bForceWar;
}

bool CvVoteInfo::isAssignCity() const
{
	return m_bAssignCity;
}

bool CvVoteInfo::isForceCivic(int i) const
{
	FAssertMsg(i < GC.getNumCivicInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbForceCivic ? m_pbForceCivic[i] : false;
}

bool CvVoteInfo::isVoteSourceType(int i) const
{
	FAssertMsg(i < GC.getNumVoteSourceInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abVoteSourceTypes ? m_abVoteSourceTypes[i] : false;
}

bool CvVoteInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iPopulationThreshold, L"iPopulationThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionVotePercent, L"iStateReligionVotePercent");
	pXML->GetOptionalChildXmlValByName(&m_iTradeRoutes, L"iTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iMinVoters, L"iMinVoters");

	pXML->GetOptionalChildXmlValByName(&m_bSecretaryGeneral, L"bSecretaryGeneral");
	pXML->GetOptionalChildXmlValByName(&m_bVictory, L"bVictory");
	pXML->GetOptionalChildXmlValByName(&m_bFreeTrade, L"bFreeTrade");
	pXML->GetOptionalChildXmlValByName(&m_bNoNukes, L"bNoNukes");
	pXML->GetOptionalChildXmlValByName(&m_bCityVoting, L"bCityVoting");
	pXML->GetOptionalChildXmlValByName(&m_bCivVoting, L"bCivVoting");
	pXML->GetOptionalChildXmlValByName(&m_bDefensivePact, L"bDefensivePact");
	pXML->GetOptionalChildXmlValByName(&m_bOpenBorders, L"bOpenBorders");
	pXML->GetOptionalChildXmlValByName(&m_bForcePeace, L"bForcePeace");
	pXML->GetOptionalChildXmlValByName(&m_bForceNoTrade, L"bForceNoTrade");
	pXML->GetOptionalChildXmlValByName(&m_bForceWar, L"bForceWar");
	pXML->GetOptionalChildXmlValByName(&m_bAssignCity, L"bAssignCity");

	pXML->SetVariableListTagPair(&m_pbForceCivic, L"ForceCivics",  GC.getNumCivicInfos());
	pXML->SetVariableListTagPair(&m_abVoteSourceTypes, L"DiploVotes",  GC.getNumVoteSourceInfos());

	return true;
}

void CvVoteInfo::copyNonDefaults(CvVoteInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getPopulationThreshold() == iDefault) m_iPopulationThreshold = pClassInfo->getPopulationThreshold();
	if (getStateReligionVotePercent() == iDefault) m_iStateReligionVotePercent = pClassInfo->getStateReligionVotePercent();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (getMinVoters() == iDefault) m_iMinVoters = pClassInfo->getMinVoters();
	if (isSecretaryGeneral() == bDefault) m_bSecretaryGeneral = pClassInfo->isSecretaryGeneral();
	if (isVictory() == bDefault) m_bVictory = pClassInfo->isVictory();
	if (isFreeTrade() == bDefault) m_bFreeTrade = pClassInfo->isFreeTrade();
	if (isNoNukes() == bDefault) m_bNoNukes = pClassInfo->isNoNukes();
	if (isCityVoting() == bDefault) m_bCityVoting = pClassInfo->isCityVoting();
	if (isCivVoting() == bDefault) m_bCivVoting = pClassInfo->isCivVoting();
	if (isDefensivePact() == bDefault) m_bDefensivePact = pClassInfo->isDefensivePact();
	if (isOpenBorders() == bDefault) m_bOpenBorders = pClassInfo->isOpenBorders();
	if (isForcePeace() == bDefault) m_bForcePeace = pClassInfo->isForcePeace();
	if (isForceNoTrade() == bDefault) m_bForceNoTrade = pClassInfo->isForceNoTrade();
	if (isForceWar() == bDefault) m_bForceWar = pClassInfo->isForceWar();
	if (isAssignCity() == bDefault) m_bAssignCity = pClassInfo->isAssignCity();

	for ( int i = 0; i < GC.getNumCivicInfos(); i++ )
	{
		if ( isForceCivic(i) == bDefault && pClassInfo->isForceCivic(i) != bDefault)
		{
			if ( NULL == m_pbForceCivic )
			{
				CvXMLLoadUtility::InitList(&m_pbForceCivic,GC.getNumCivicInfos(),bDefault);
			}
			m_pbForceCivic[i] = pClassInfo->isForceCivic(i);
		}
	}

	for ( int i = 0; i < GC.getNumVoteSourceInfos(); i++ )
	{
		if ( isVoteSourceType(i) == bDefault && pClassInfo->isVoteSourceType(i) != bDefault)
		{
			if ( NULL == m_abVoteSourceTypes )
			{
				CvXMLLoadUtility::InitList(&m_abVoteSourceTypes,GC.getNumVoteSourceInfos(),bDefault);
			}
			m_abVoteSourceTypes[i] = pClassInfo->isVoteSourceType(i);
		}
	}
}

void CvVoteInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iPopulationThreshold);
	CheckSum(iSum, m_iStateReligionVotePercent);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iMinVoters);

	CheckSum(iSum, m_bSecretaryGeneral);
	CheckSum(iSum, m_bVictory);
	CheckSum(iSum, m_bFreeTrade);
	CheckSum(iSum, m_bNoNukes);
	CheckSum(iSum, m_bCityVoting);
	CheckSum(iSum, m_bCivVoting);
	CheckSum(iSum, m_bDefensivePact);
	CheckSum(iSum, m_bOpenBorders);
	CheckSum(iSum, m_bForcePeace);
	CheckSum(iSum, m_bForceNoTrade);
	CheckSum(iSum, m_bForceWar);
	CheckSum(iSum, m_bAssignCity);

	// Arrays

	CheckSum(iSum, m_pbForceCivic, GC.getNumCivicInfos());
	CheckSum(iSum, m_abVoteSourceTypes, GC.getNumVoteSourceInfos());
}

//======================================================================================================
//					CvProjectInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvProjectInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvProjectInfo::CvProjectInfo() :
m_iVictoryPrereq(NO_VICTORY),
m_iTechPrereq(NO_TECH),
m_iAnyoneProjectPrereq(NO_PROJECT),
m_iMaxGlobalInstances(0),
m_iMaxTeamInstances(0),
m_iProductionCost(0),
m_iNukeInterception(0),
m_iTechShare(0),
//DPII < Maintenance Modifiers >
m_iGlobalMaintenanceModifier(0),
m_iDistanceMaintenanceModifier(0),
m_iNumCitiesMaintenanceModifier(0),
m_iConnectedCityMaintenanceModifier(0),
//DPII < Maintenance Modifiers >
m_iEveryoneSpecialUnit(NO_SPECIALUNIT),
m_iEveryoneSpecialBuilding(NO_SPECIALBUILDING),
m_iVictoryDelayPercent(0),
m_iSuccessRate(0),
m_bSpaceship(false),
m_bAllowsNukes(false),
m_piBonusProductionModifier(NULL),
m_piVictoryThreshold(NULL),
m_piVictoryMinThreshold(NULL),
m_piProjectsNeeded(NULL)
/************************************************************************************************/
/* Afforess					  Start		 01/02/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iWorldHappiness(0)
,m_iGlobalHappiness(0)
,m_iWorldHealth(0)
,m_iGlobalHealth(0)
,m_iWorldTradeRoutes(0)
,m_iInflationModifier(0)
,m_bTechShareWithHalfCivs(false)
,m_piCommerceModifier(NULL)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvProjectInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvProjectInfo::~CvProjectInfo()
{
	SAFE_DELETE_ARRAY(m_piBonusProductionModifier);
	SAFE_DELETE_ARRAY(m_piVictoryThreshold);
	SAFE_DELETE_ARRAY(m_piVictoryMinThreshold);
	SAFE_DELETE_ARRAY(m_piProjectsNeeded);
}

int CvProjectInfo::getVictoryPrereq() const
{
	return m_iVictoryPrereq;
}

int CvProjectInfo::getTechPrereq() const
{
	return m_iTechPrereq;
}

int CvProjectInfo::getAnyoneProjectPrereq() const
{
	return m_iAnyoneProjectPrereq;
}

void CvProjectInfo::setAnyoneProjectPrereq(int i)
{
	m_iAnyoneProjectPrereq = i;
}

int CvProjectInfo::getMaxGlobalInstances() const
{
	return m_iMaxGlobalInstances;
}

int CvProjectInfo::getMaxTeamInstances() const
{
	return m_iMaxTeamInstances;
}

int CvProjectInfo::getProductionCost() const
{
	return m_iProductionCost;
}

int CvProjectInfo::getNukeInterception() const
{
	return m_iNukeInterception;
}

int CvProjectInfo::getTechShare() const
{
	return m_iTechShare;
}

//DPII < Maintenance Modifiers >
int CvProjectInfo::getGlobalMaintenanceModifier() const
{
	return m_iGlobalMaintenanceModifier;
}

int CvProjectInfo::getDistanceMaintenanceModifier() const
{
	return m_iDistanceMaintenanceModifier;
}

int CvProjectInfo::getNumCitiesMaintenanceModifier() const
{
	return m_iNumCitiesMaintenanceModifier;

}

int CvProjectInfo::getConnectedCityMaintenanceModifier() const
{
	return m_iConnectedCityMaintenanceModifier;
}
//DPII < Maintenance Modifiers >

int CvProjectInfo::getEveryoneSpecialUnit() const
{
	return m_iEveryoneSpecialUnit;
}

int CvProjectInfo::getEveryoneSpecialBuilding() const
{
	return m_iEveryoneSpecialBuilding;
}

int CvProjectInfo::getVictoryDelayPercent() const
{
	return m_iVictoryDelayPercent;
}

int CvProjectInfo::getSuccessRate() const
{
	return m_iSuccessRate;
}

bool CvProjectInfo::isSpaceship() const
{
	return m_bSpaceship;
}

bool CvProjectInfo::isAllowsNukes() const
{
	return m_bAllowsNukes;
}

const char* CvProjectInfo::getMovieArtDef() const
{
	return m_szMovieArtDef;
}

const TCHAR* CvProjectInfo::getCreateSound() const
{
	return m_szCreateSound;
}

void CvProjectInfo::setCreateSound(const TCHAR* szVal)
{
	m_szCreateSound = szVal;
}

// Arrays

int CvProjectInfo::getBonusProductionModifier(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piBonusProductionModifier ? m_piBonusProductionModifier[i] : 0;
}

int CvProjectInfo::getVictoryThreshold(int i) const
{
	FAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piVictoryThreshold ? m_piVictoryThreshold[i] : 0;
}

int CvProjectInfo::getVictoryMinThreshold(int i) const
{
	FAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (m_piVictoryMinThreshold && m_piVictoryMinThreshold[i] != 0)
	{
		return m_piVictoryMinThreshold[i];
	}

	return getVictoryThreshold(i);
}

int CvProjectInfo::getProjectsNeeded(int i) const
{
	FAssertMsg(i < GC.getNumProjectInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piProjectsNeeded ? m_piProjectsNeeded[i] : false;
}

/************************************************************************************************/
/* Afforess					  Start		 01/02/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
int CvProjectInfo::getWorldHappiness() const
{
	return m_iWorldHappiness;
}

int CvProjectInfo::getGlobalHappiness() const
{
	return m_iGlobalHappiness;
}

int CvProjectInfo::getWorldHealth() const
{
	return m_iWorldHealth;
}

int CvProjectInfo::getGlobalHealth() const
{
	return m_iGlobalHealth;
}

int CvProjectInfo::getWorldTradeRoutes() const
{
	return m_iWorldTradeRoutes;
}

int CvProjectInfo::getInflationModifier() const
{
	return m_iInflationModifier;
}

bool CvProjectInfo::isTechShareWithHalfCivs() const
{
	return m_bTechShareWithHalfCivs;
}

int CvProjectInfo::getCommerceModifier(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}

int* CvProjectInfo::getCommerceModifierArray() const
{
	return m_piCommerceModifier;
}

// Vectors

int CvProjectInfo::getMapCategoryType(int i) const
{
	return m_aiMapCategoryTypes[i];
}

int CvProjectInfo::getNumMapCategoryTypes() const
{
	return (int)m_aiMapCategoryTypes.size();
}

bool CvProjectInfo::isMapCategoryType(int i)
{
	if (find(m_aiMapCategoryTypes.begin(), m_aiMapCategoryTypes.end(), i) == m_aiMapCategoryTypes.end())
	{
		return false;
	}
	return true;
}

int CvProjectInfo::getProjectsNeededVectorSize()					{return m_aszProjectsNeededforPass3.size();}
CvString CvProjectInfo::getProjectsNeededNamesVectorElement(int i)	{return m_aszProjectsNeededforPass3[i];}
int CvProjectInfo::getProjectsNeededValuesVectorElement(int i)		{return m_aiProjectsNeededforPass3[i];}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

bool CvProjectInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"VictoryPrereq");
	m_iVictoryPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereq");
	m_iTechPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iMaxGlobalInstances, L"iMaxGlobalInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxTeamInstances, L"iMaxTeamInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iProductionCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_iNukeInterception, L"iNukeInterception");
	pXML->GetOptionalChildXmlValByName(&m_iTechShare, L"iTechShare");
	//DPII < Maintenance Modifiers >
	pXML->GetOptionalChildXmlValByName(&m_iGlobalMaintenanceModifier, L"iGlobalMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenanceModifier, L"iDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenanceModifier, L"iNumCitiesMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iConnectedCityMaintenanceModifier, L"iConnectedCityMaintenanceModifier");
	//DPII < Maintenance Modifiers >
	pXML->GetOptionalChildXmlValByName(szTextVal, L"EveryoneSpecialUnit");
	m_iEveryoneSpecialUnit = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"EveryoneSpecialBuilding");
	m_iEveryoneSpecialBuilding = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bSpaceship, L"bSpaceship");
	pXML->GetOptionalChildXmlValByName(&m_bAllowsNukes, L"bAllowsNukes");
	pXML->GetOptionalChildXmlValByName(m_szMovieArtDef, L"MovieDefineTag");

	pXML->SetVariableListTagPair(&m_piBonusProductionModifier, L"BonusProductionModifiers", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piVictoryThreshold, L"VictoryThresholds",  GC.getNumVictoryInfos());
	pXML->SetVariableListTagPair(&m_piVictoryMinThreshold, L"VictoryMinThresholds",  GC.getNumVictoryInfos());
	pXML->GetOptionalChildXmlValByName(&m_iVictoryDelayPercent, L"iVictoryDelayPercent");
	pXML->GetOptionalChildXmlValByName(&m_iSuccessRate, L"iSuccessRate");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CreateSound");
	setCreateSound(szTextVal);

/************************************************************************************************/
/* Afforess					  Start		 01/02/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iWorldHappiness, L"iWorldHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalHappiness, L"iGlobalHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iWorldHealth, L"iWorldHealth");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalHealth, L"iGlobalHealth");
	pXML->GetOptionalChildXmlValByName(&m_iWorldTradeRoutes, L"iWorldTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_bTechShareWithHalfCivs, L"bTechShareWithHalfCivs");
	pXML->GetOptionalChildXmlValByName(&m_iInflationModifier, L"iInflationModifier");

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"PrereqProjects"))
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
						m_aszProjectsNeededforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiProjectsNeededforPass3.push_back(iTemp);
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

	// Vectors
	pXML->SetOptionalIntVector(&m_aiMapCategoryTypes, L"MapCategoryTypes");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"AnyonePrereqProject");
	m_aszExtraXMLforPass3.push_back(szTextVal);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	return true;
}

// bool CvProjectInfo::readPass2(CvXMLLoadUtility* pXML)
// {
	// CvString szTextVal;
// /************************************************************************************************/
// /* XMLCOPY								 10/14/07								MRGENIE	  */
// /*																							  */
// /*																							  */
// /************************************************************************************************/
	// if (!CvInfoBase::read(pXML))
	// {
		// return false;
	// }
// /************************************************************************************************/
// /* XMLCOPY								 END												  */
// /************************************************************************************************/

	// pXML->SetVariableListTagPair(&m_piProjectsNeeded, L"PrereqProjects", sizeof(GC.getProjectInfo((ProjectTypes)0)), GC.getNumProjectInfos());

	// pXML->GetChildXmlValByName(szTextVal, L"AnyonePrereqProject");
	// m_iAnyoneProjectPrereq = GC.getInfoTypeForString(szTextVal);

	// return true;
// }

void CvProjectInfo::copyNonDefaults(CvProjectInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getVictoryPrereq() == iTextDefault) m_iVictoryPrereq = pClassInfo->getVictoryPrereq();
	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getEveryoneSpecialUnit() == iTextDefault) m_iEveryoneSpecialUnit = pClassInfo->getEveryoneSpecialUnit();
	if (getEveryoneSpecialBuilding() == iTextDefault) m_iEveryoneSpecialBuilding = pClassInfo->getEveryoneSpecialBuilding();

	if (getMaxGlobalInstances() == -1) m_iMaxGlobalInstances = pClassInfo->getMaxGlobalInstances();
	if (getMaxTeamInstances() == -1) m_iMaxTeamInstances = pClassInfo->getMaxTeamInstances();
	if (getProductionCost() == iDefault) m_iProductionCost = pClassInfo->getProductionCost();
	if (getNukeInterception() == iDefault) m_iNukeInterception = pClassInfo->getNukeInterception();
	if (getTechShare() == iDefault) m_iTechShare = pClassInfo->getTechShare();

	if (isSpaceship() == bDefault) m_bSpaceship = pClassInfo->isSpaceship();
	if (isAllowsNukes() == bDefault) m_bAllowsNukes = pClassInfo->isAllowsNukes();

	if (getMovieArtDef() == cDefault) m_szMovieArtDef = pClassInfo->getMovieArtDef();

	for ( int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		if (getBonusProductionModifier(i) == iDefault && pClassInfo->getBonusProductionModifier(i) != iDefault)
		{
			if ( NULL == m_piBonusProductionModifier )
			{
				CvXMLLoadUtility::InitList(&m_piBonusProductionModifier,GC.getNumBonusInfos(),iDefault);
			}
			m_piBonusProductionModifier[i] = pClassInfo->getBonusProductionModifier(i);
		}
	}
	for ( int i = 0; i < GC.getNumVictoryInfos(); i++)
	{
		if (getVictoryThreshold(i) == iDefault && pClassInfo->getVictoryThreshold(i) != iDefault)
		{
			if ( NULL == m_piVictoryThreshold )
			{
				CvXMLLoadUtility::InitList(&m_piVictoryThreshold,GC.getNumVictoryInfos(),iDefault);
			}
			m_piVictoryThreshold[i] = pClassInfo->getVictoryThreshold(i);
		}
		if (getVictoryMinThreshold(i) == iDefault && pClassInfo->getVictoryMinThreshold(i) != iDefault)
		{
			if ( NULL == m_piVictoryMinThreshold )
			{
				CvXMLLoadUtility::InitList(&m_piVictoryMinThreshold,GC.getNumVictoryInfos(),iDefault);
			}
			m_piVictoryMinThreshold[i] = pClassInfo->getVictoryMinThreshold(i);
		}
	}
	if (getVictoryDelayPercent() == iDefault) m_iVictoryDelayPercent = pClassInfo->getVictoryDelayPercent();
	if (getSuccessRate() == iDefault) m_iSuccessRate = pClassInfo->getSuccessRate();

	if (getCreateSound() == cDefault) setCreateSound(pClassInfo->getCreateSound());


	if (m_iAnyoneProjectPrereq == iTextDefault) m_iAnyoneProjectPrereq = pClassInfo->getAnyoneProjectPrereq();

/************************************************************************************************/
/* Afforess					  Start		 07/30/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getWorldHappiness() == iDefault) m_iWorldHappiness = pClassInfo->getWorldHappiness();
	if (getGlobalHappiness() == iDefault) m_iGlobalHappiness= pClassInfo->getGlobalHappiness();
	if (getWorldHealth() == iDefault) m_iWorldHealth = pClassInfo->getWorldHealth();
	if (getGlobalHealth() == iDefault) m_iGlobalHealth = pClassInfo->getGlobalHealth();
	if (isTechShareWithHalfCivs() == bDefault) m_bTechShareWithHalfCivs = pClassInfo->isTechShareWithHalfCivs();
	if (getWorldTradeRoutes() == iDefault) m_iWorldTradeRoutes = pClassInfo->getWorldTradeRoutes();
	if (getInflationModifier() == iDefault) m_iInflationModifier = pClassInfo->getInflationModifier();

	if (!m_iGlobalMaintenanceModifier) m_iGlobalMaintenanceModifier = pClassInfo->getGlobalMaintenanceModifier();
	if (!m_iDistanceMaintenanceModifier) m_iDistanceMaintenanceModifier = pClassInfo->getDistanceMaintenanceModifier();
	if (!m_iNumCitiesMaintenanceModifier) m_iNumCitiesMaintenanceModifier = pClassInfo->getNumCitiesMaintenanceModifier();
	if (!m_iConnectedCityMaintenanceModifier) m_iConnectedCityMaintenanceModifier = pClassInfo->getConnectedCityMaintenanceModifier();

	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( m_piCommerceModifier[j] == iDefault )
		{
			m_piCommerceModifier[j] = pClassInfo->getCommerceModifier(j);
		}
	}

	for ( int i = 0; i < pClassInfo->getProjectsNeededVectorSize(); i++ )
	{
		m_aiProjectsNeededforPass3.push_back(pClassInfo->getProjectsNeededValuesVectorElement(i));
		m_aszProjectsNeededforPass3.push_back(pClassInfo->getProjectsNeededNamesVectorElement(i));
	}

	if (getNumMapCategoryTypes() == 0)
	{
		m_aiMapCategoryTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumMapCategoryTypes(); i++)
		{
			m_aiMapCategoryTypes.push_back(pClassInfo->getMapCategoryType(i));
		}
	}



}

bool CvProjectInfo::readPass3()
{
	m_piProjectsNeeded = new int[GC.getNumProjectInfos()];
	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		m_piProjectsNeeded[iI] = 0;
	}
	if (!m_aiProjectsNeededforPass3.empty() && !m_aszProjectsNeededforPass3.empty())
	{
		int iNumLoad = m_aiProjectsNeededforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszProjectsNeededforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumEventInfos())
				m_piProjectsNeeded[iTempIndex] = m_aiProjectsNeededforPass3[iI];
		}
		m_aszProjectsNeededforPass3.clear();
		m_aiProjectsNeededforPass3.clear();
	}

	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}
	m_iAnyoneProjectPrereq = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);

	return true;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

void CvProjectInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iWorldHappiness);
	CheckSum(iSum, m_iGlobalHappiness);
	CheckSum(iSum, m_iWorldHealth);
	CheckSum(iSum, m_iGlobalHealth);
	CheckSum(iSum, m_iWorldTradeRoutes);
	CheckSum(iSum, m_iInflationModifier);
	CheckSum(iSum, m_bTechShareWithHalfCivs);

	CheckSum(iSum, m_piCommerceModifier, NUM_COMMERCE_TYPES);

	CheckSum(iSum, m_iVictoryPrereq);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iAnyoneProjectPrereq);
	CheckSum(iSum, m_iMaxGlobalInstances);
	CheckSum(iSum, m_iMaxTeamInstances);
	CheckSum(iSum, m_iProductionCost);
	CheckSum(iSum, m_iNukeInterception);
	CheckSum(iSum, m_iTechShare);

	CheckSum(iSum, m_iGlobalMaintenanceModifier);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iConnectedCityMaintenanceModifier);

	CheckSum(iSum, m_iEveryoneSpecialUnit);
	CheckSum(iSum, m_iEveryoneSpecialBuilding);
	CheckSum(iSum, m_iVictoryDelayPercent);
	CheckSum(iSum, m_iSuccessRate);

	CheckSum(iSum, m_bSpaceship);
	CheckSum(iSum, m_bAllowsNukes);

	// Arrays

	CheckSum(iSum, m_piBonusProductionModifier, GC.getNumBonusInfos());
	CheckSum(iSum, m_piVictoryThreshold, GC.getNumVictoryInfos());
	CheckSum(iSum, m_piVictoryMinThreshold, GC.getNumVictoryInfos());
	CheckSum(iSum, m_piProjectsNeeded, GC.getNumProjectInfos());

	// Vectors

	CheckSumC(iSum, m_aiMapCategoryTypes);

}

//======================================================================================================
//					CvSpaceShipInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpaceShipInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpaceShipInfo::CvSpaceShipInfo() :
m_eSpaceShipInfoType(SPACE_SHIP_INFO_TYPE_NONE),
m_iPartNumber(-1),
m_iArtType(-1),
m_iEventCode(-1),
m_eProjectType(NO_PROJECT),
m_eCameraUpAxis(AXIS_X)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpaceShipInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpaceShipInfo::~CvSpaceShipInfo()
{
}

const TCHAR* CvSpaceShipInfo::getNodeName()
{
	return m_szNodeName;
}

void CvSpaceShipInfo::setNodeName(const TCHAR* szVal)
{
	m_szNodeName = szVal;
}

const TCHAR* CvSpaceShipInfo::getProjectName()
{
	return m_szProjectName;
}

void CvSpaceShipInfo::setProjectName(const TCHAR* szVal)
{
	m_szProjectName = szVal;
	m_eProjectType = (ProjectTypes) GC.getInfoTypeForString(m_szProjectName, true);
}

ProjectTypes CvSpaceShipInfo::getProjectType()
{
	return m_eProjectType;
}

AxisTypes CvSpaceShipInfo::getCameraUpAxis()
{
	return m_eCameraUpAxis;
}

SpaceShipInfoTypes CvSpaceShipInfo::getSpaceShipInfoType()
{
	return m_eSpaceShipInfoType;
}

int CvSpaceShipInfo::getPartNumber()
{
	return m_iPartNumber;
}

int CvSpaceShipInfo::getArtType()
{
	return m_iArtType;
}

int CvSpaceShipInfo::getEventCode()
{
	return m_iEventCode;
}

bool CvSpaceShipInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, L"NodeName");
	setNodeName(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, L"ProjectName");
	setProjectName(szTextVal);

	//up axis
	pXML->GetChildXmlValByName(szTextVal, L"CameraUpAxis");
	if(szTextVal.CompareNoCase("AXIS_X") == 0)
		m_eCameraUpAxis = AXIS_X;
	else if(szTextVal.CompareNoCase("AXIS_Y") == 0)
		m_eCameraUpAxis = AXIS_Y;
	else if(szTextVal.CompareNoCase("AXIS_Z") == 0)
		m_eCameraUpAxis = AXIS_Z;
	else
	{
		FAssertMsg(false, "[Jason] Unknown Axis Type.");
	}

	pXML->GetOptionalChildXmlValByName(&m_iPartNumber, L"PartNumber");
	pXML->GetOptionalChildXmlValByName(&m_iArtType, L"ArtType");
	pXML->GetOptionalChildXmlValByName(&m_iEventCode, L"EventCode");

	//type
	pXML->GetChildXmlValByName(szTextVal, L"InfoType");
	if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_FILENAME") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_FILENAME;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_ALPHA_CENTAURI") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_ALPHA_CENTAURI;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_LAUNCH") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_LAUNCH;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_LAUNCHED") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_LAUNCHED;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_ZOOM_IN") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_ZOOM_IN;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_ZOOM_MOVE") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_ZOOM_MOVE;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_OFF") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_OFF;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_APPEAR") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_APPEAR;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_PREVIEW") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_PREVIEW;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_ON;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_LIGHT_OFF") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_LIGHT_OFF;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_GANTRY_SMOKE_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_GANTRY_SMOKE_ON;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_IN_SPACE_SMOKE_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_IN_SPACE_SMOKE_ON;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_IN_GAME_SMOKE_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_IN_GAME_SMOKE_ON;
	else
	{
		FAssertMsg(false, "[Jason] Unknown SpaceShipInfoType.");
	}

	return true;
}
void CvSpaceShipInfo::copyNonDefaults(CvSpaceShipInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getNodeName() == cDefault) setNodeName(pClassInfo->getNodeName());
	if (getProjectName() == cDefault) setProjectName(pClassInfo->getProjectName());

	if (getPartNumber() == iDefault) m_iPartNumber = pClassInfo->getPartNumber();
	if (getArtType() == iDefault) m_iArtType = pClassInfo->getArtType();
	if (getEventCode() == iDefault) m_iEventCode = pClassInfo->getEventCode();

//	if (getCameraUpAxis() == cDefault) m_eCameraUpAxis = pClassInfo->getCameraUpAxis();
//	if (getSpaceShipInfoType() == cDefault) m_eSpaceShipInfoType = pClassInfo->getSpaceShipInfoType();
}

//------------------------------------------------------------------------------------------------------
//
//  CvCultureLevelInfo
//

CvCultureLevelInfo::CvCultureLevelInfo() :
m_iCityDefenseModifier(0),
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
m_iCityRadius(1),
m_iMaxWorldWonders(1),
m_iMaxTeamWonders(1),
m_iMaxNationalWonders(1),
m_iMaxNationalWondersOCC(1),
m_iPrereqGameOption(NO_GAMEOPTION),
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/
m_paiSpeedThreshold(NULL)
{
}

CvCultureLevelInfo::~CvCultureLevelInfo()
{
	SAFE_DELETE_ARRAY(m_paiSpeedThreshold);
}

int CvCultureLevelInfo::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier;
}

int CvCultureLevelInfo::getSpeedThreshold(int i) const
{
	FAssert(i < GC.getNumGameSpeedInfos());
	FAssert(i > -1);
	return m_paiSpeedThreshold ? m_paiSpeedThreshold[i] : 0;
}

/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
int CvCultureLevelInfo::getCityRadius() const
{
	return m_iCityRadius;
}

int CvCultureLevelInfo::getMaxWorldWonders() const
{
	return m_iMaxWorldWonders;
}

int CvCultureLevelInfo::getMaxTeamWonders() const
{
	return m_iMaxTeamWonders;
}

int CvCultureLevelInfo::getMaxNationalWonders() const
{
	return m_iMaxNationalWonders;
}

int CvCultureLevelInfo::getMaxNationalWondersOCC() const
{
	return m_iMaxNationalWondersOCC;
}

int CvCultureLevelInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/

bool CvCultureLevelInfo::read(CvXMLLoadUtility* pXml)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXml))
	{
		return false;
	}

	pXml->GetOptionalChildXmlValByName(&m_iCityDefenseModifier, L"iCityDefenseModifier");
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXml->GetOptionalChildXmlValByName(&m_iCityRadius, L"iCityRadius", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxWorldWonders, L"iMaxWorldWonders", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxTeamWonders, L"iMaxTeamWonders", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxNationalWonders, L"iMaxNationalWonders", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxNationalWondersOCC, L"iMaxNationalWondersOCC", 1);
	CvString szTextVal;
	pXml->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXml->GetInfoClass(szTextVal);
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/
	pXml->SetVariableListTagPair(&m_paiSpeedThreshold, L"SpeedThresholds", GC.getNumGameSpeedInfos());

	return true;
}

void CvCultureLevelInfo::copyNonDefaults(CvCultureLevelInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getCityDefenseModifier() == iDefault) m_iCityDefenseModifier = pClassInfo->getCityDefenseModifier();
	if (getCityRadius() == 1) m_iCityRadius = pClassInfo->getCityRadius();
	if (getMaxWorldWonders() == 1) m_iMaxWorldWonders = pClassInfo->getMaxWorldWonders();
	if (getMaxTeamWonders() == 1) m_iMaxTeamWonders = pClassInfo->getMaxTeamWonders();
	if (getMaxNationalWonders() == 1) m_iMaxNationalWonders = pClassInfo->getMaxNationalWonders();
	if (getMaxNationalWondersOCC() == 1) m_iMaxNationalWondersOCC = pClassInfo->getMaxNationalWondersOCC();
	if (getPrereqGameOption() == NO_GAMEOPTION) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();

	for ( int i = 0; i < GC.getNumGameSpeedInfos(); i++ )
	{
		if ( getSpeedThreshold(i) == iDefault && pClassInfo->getSpeedThreshold(i) != iDefault)
		{
			if ( NULL == m_paiSpeedThreshold )
			{
				CvXMLLoadUtility::InitList(&m_paiSpeedThreshold,GC.getNumGameSpeedInfos(),iDefault);
			}
			m_paiSpeedThreshold[i] = pClassInfo->getSpeedThreshold(i);
		}
	}
}

void CvCultureLevelInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iCityDefenseModifier);
	CheckSum(iSum, m_iCityRadius);
	CheckSum(iSum, m_iMaxWorldWonders);
	CheckSum(iSum, m_iMaxTeamWonders);
	CheckSum(iSum, m_iMaxNationalWonders);
	CheckSum(iSum, m_iMaxNationalWondersOCC);
	CheckSum(iSum, m_iPrereqGameOption);

	CheckSum(iSum, m_paiSpeedThreshold, GC.getNumGameSpeedInfos());
}

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
m_iBeelineStingsTechCostModifier(0),
m_bNoGoodies(false),
m_bNoAnimals(false),
m_bNoBarbUnits(false),
m_bNoBarbCities(false),
m_bFirstSoundtrackFirst(false),
m_paiCitySoundscapeSciptIds(NULL),
m_paiSoundtracks(NULL)
/************************************************************************************************/
/* Afforess					  Start		 09/12/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iInitialCityMaintenancePercent(0)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
{
}

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

int CvEraInfo::getBeelineStingsTechCostModifier() const
{
	return m_iBeelineStingsTechCostModifier;
}

const TCHAR* CvEraInfo::getAudioUnitVictoryScript() const
{
	return m_szAudioUnitVictoryScript;
}

const TCHAR* CvEraInfo::getAudioUnitDefeatScript() const
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
/************************************************************************************************/
/* Afforess					  Start		 09/12/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
int CvEraInfo::getInitialCityMaintenancePercent() const
{
	return m_iInitialCityMaintenancePercent;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
// Arrays

int CvEraInfo::getSoundtracks(int i) const
{
	FAssertMsg(i < getNumSoundtracks(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
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
	MEMORY_TRACE_FUNCTION();

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
	pXML->GetOptionalChildXmlValByName(&m_iBeelineStingsTechCostModifier, L"iBeelineStingsTechCostModifier");

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

	pXML->SetVariableListTagPairForAudioScripts(&m_paiCitySoundscapeSciptIds, L"CitySoundscapes", GC.getCitySizeTypes(), GC.getNumCitySizeTypes());

	return true;
}

void CvEraInfo::copyNonDefaults(CvEraInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

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
	if (getBeelineStingsTechCostModifier() == iDefault) m_iBeelineStingsTechCostModifier = pClassInfo->getBeelineStingsTechCostModifier();
	if (getEventChancePerTurn() == iDefault) m_iEventChancePerTurn = pClassInfo->getEventChancePerTurn();
	if (getSoundtrackSpace() == iDefault) m_iSoundtrackSpace = pClassInfo->getSoundtrackSpace();
	if (isFirstSoundtrackFirst() == bDefault) m_bFirstSoundtrackFirst = pClassInfo->isFirstSoundtrackFirst();
	if (getAudioUnitVictoryScript() == cDefault) m_szAudioUnitVictoryScript = pClassInfo->getAudioUnitVictoryScript();
	if (getAudioUnitDefeatScript() == cDefault) m_szAudioUnitDefeatScript = pClassInfo->getAudioUnitDefeatScript();
/************************************************************************************************/
/* Afforess					  Start		 09/12/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getInitialCityMaintenancePercent() == iDefault) m_iInitialCityMaintenancePercent = pClassInfo->getInitialCityMaintenancePercent();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
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

void CvEraInfo::getCheckSum(unsigned int& iSum)
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
	CheckSum(iSum, m_iBeelineStingsTechCostModifier);

	CheckSum(iSum, m_bNoGoodies);
	CheckSum(iSum, m_bNoAnimals);
	CheckSum(iSum, m_bNoBarbUnits);
	CheckSum(iSum, m_bNoBarbCities);
}

//////////////////////////////////////////////////////////////////////////
// CvGameText
//////////////////////////////////////////////////////////////////////////

// static
int CvGameText::NUM_LANGUAGES = 11;
CvWString CvGameText::m_szLanguage = L"";

int CvGameText::getNumLanguages() const
{
	return NUM_LANGUAGES;
}
void CvGameText::setNumLanguages(int iNum)
{
	//NUM_LANGUAGES = iNum;
}

CvGameText::CvGameText() :
	m_szGender("N"),
	m_szPlural("false")
{
}

const wchar* CvGameText::getText() const
{
	return m_szText;
}

void CvGameText::setText(const wchar* szText)
{
	m_szText = szText;
}

bool CvGameText::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	//pXML->TryMoveToXmlFirstChild(); // Move down to Child level
	pXML->GetChildXmlValByName(m_szType, L"Tag");
	//pXML->GetXmlVal(m_szType);		// TAG

	//static const int iMaxNumLanguages = GC.getDefineINT("MAX_NUM_LANGUAGES");
	//int iNumLanguages = NUM_LANGUAGES ? NUM_LANGUAGES : iMaxNumLanguages + 1;

	int iFallback = 0;

	CvWString wszTextVal;

	while (true)
	{
		switch (iFallback)
		{
			case 0:
				iFallback++;
				if (pXML->TryMoveToXmlFirstChild(m_szLanguage))
				{
					break;
				}

			case 1:
				// fallback to English
				iFallback++;
				if (pXML->TryMoveToXmlFirstChild(L"English"))
				{
					break;
				}

			case 2:
				// fallback to first tag after Tag
				iFallback++;
				if (!pXML->TryMoveToXmlFirstChild())
				{
					return false;
				}
				else if (!pXML->TryMoveToXmlNextSibling())
				{
					pXML->MoveToXmlParent();
					return false;
				}
				break;

			default:
				// no success, discard the text
				return false;
		}

		//CvString szTextVal;

		// we are now on the chosen language tag (or a fallback)
		if (!pXML->GetOptionalChildXmlValByName(wszTextVal, L"Text", L""))
		{
			pXML->GetXmlVal(wszTextVal);

		}

		// if there is content, we succeeded and break the loop
		if (!wszTextVal.empty())
		{
			setText(wszTextVal);
			break;
		}
		else
		{
			// next fallback, but undo the SetToChild first
			pXML->MoveToXmlParent();
		}
	}

	// GENDER
	if (pXML->GetOptionalChildXmlValByName(wszTextVal, L"Gender", L""))
	{
		setGender(wszTextVal);
	}

	// PLURAL
	if (pXML->GetOptionalChildXmlValByName(wszTextVal, L"Plural", L""))
	{
		setPlural(wszTextVal);
	}

	// old code
	/*int j=0;
	for (j = 0; j < iNumLanguages; j++)
	{
		pXML->SkipToNextVal();	// skip comments

		if (!pXML->TryMoveToXmlNextSibling(); || j == iMaxNumLanguages)
		{
			NUM_LANGUAGES = j;
			break;
		}
		if (j == GAMETEXT.getCurrentLanguage()) // Only add appropriate language Text
		{
			// TEXT
			if (pXML->GetChildXmlValByName(wszTextVal, L"Text"))
			{
				setText(wszTextVal);
			}
			else
			{
				pXML->GetXmlVal(wszTextVal);
				setText(wszTextVal);
				if (NUM_LANGUAGES > 0)
				{
					break;
				}
			}

			// GENDER
			if (pXML->GetChildXmlValByName(wszTextVal, L"Gender"))
			{
				setGender(wszTextVal);
			}

			// PLURAL
			if (pXML->GetChildXmlValByName(wszTextVal, L"Plural"))
			{
				setPlural(wszTextVal);
			}
			if (NUM_LANGUAGES > 0)
			{
				break;
			}
		}
	}*/

	pXML->MoveToXmlParent(); // Move back up to Parent

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	CvDiplomacyTextInfo
//
//

CvDiplomacyTextInfo::CvDiplomacyTextInfo() :
m_iNumResponses(0),
m_pResponses(NULL)
{
}

// note - Response member vars allocated by CvXmlLoadUtility
void CvDiplomacyTextInfo::init(int iNum)
{
	uninit();
	m_pResponses = new Response[iNum];
	m_iNumResponses=iNum;
}

void CvDiplomacyTextInfo::uninit()
{
	SAFE_DELETE_ARRAY(m_pResponses);
}

int CvDiplomacyTextInfo::getNumResponses() const
{
	return m_iNumResponses;
}

bool CvDiplomacyTextInfo::getCivilizationTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumCivilizationInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i].m_pbCivilizationTypes ? m_pResponses[i].m_pbCivilizationTypes[j] : false;
}

bool CvDiplomacyTextInfo::getLeaderHeadTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumLeaderHeadInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i].m_pbLeaderHeadTypes ? m_pResponses[i].m_pbLeaderHeadTypes[j] : false;
}

bool CvDiplomacyTextInfo::getAttitudeTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_ATTITUDE_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i].m_pbAttitudeTypes ? m_pResponses[i].m_pbAttitudeTypes[j] : false;
}

bool CvDiplomacyTextInfo::getDiplomacyPowerTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_DIPLOMACYPOWER_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i].m_pbDiplomacyPowerTypes ? m_pResponses[i].m_pbDiplomacyPowerTypes[j] : false;
}

int CvDiplomacyTextInfo::getNumDiplomacyText(int i) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pResponses[i].m_iNumDiplomacyText;
}

const TCHAR* CvDiplomacyTextInfo::getDiplomacyText(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < getNumDiplomacyText(i), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i].m_paszDiplomacyText[j];
}

bool CvDiplomacyTextInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int j;
	pXML->GetChildXmlValByName(szTextVal, L"Type");

	if ( pXML->TryMoveToXmlFirstChild(L"Responses") )
	{
		int iIndexVal = pXML->GetXmlChildrenNumber(L"Response");
		init(iIndexVal);

		for (j = 0; j < iIndexVal; j++)
		{
			if (j == 0)
			{
				pXML->TryMoveToXmlFirstChild();
			}
			// Civilizations
			pXML->SetVariableListTagPair(&m_pResponses[j].m_pbCivilizationTypes, L"Civilizations", GC.getNumCivilizationInfos());
			// Leaders
			pXML->SetVariableListTagPair(&m_pResponses[j].m_pbLeaderHeadTypes, L"Leaders", GC.getNumLeaderHeadInfos());
			// AttitudeTypes
			pXML->SetVariableListTagPair(&m_pResponses[j].m_pbAttitudeTypes, L"Attitudes", NUM_ATTITUDE_TYPES);
			// PowerTypes
			pXML->SetVariableListTagPair(&m_pResponses[j].m_pbDiplomacyPowerTypes, L"DiplomacyPowers", GC.getDiplomacyPowerTypes(), NUM_DIPLOMACYPOWER_TYPES);
			// DiplomacyText
			if (pXML->TryMoveToXmlFirstChild(L"DiplomacyText"))
			{
				pXML->SetStringList(&m_pResponses[j].m_paszDiplomacyText, &m_pResponses[j].m_iNumDiplomacyText);
				pXML->MoveToXmlParent();
			}

			if (!pXML->TryMoveToXmlNextSibling())
			{
				break;
			}
		}

		pXML->MoveToXmlParent();
	}
	pXML->MoveToXmlParent();

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	CvQuestInfo			Misc\CIV4QuestInfos.xml
//
//
CvQuestInfo::CvQuestInfo() :
m_iNumQuestMessages(0),
m_iNumQuestLinks(0),
m_iNumQuestSounds(0),
m_paszQuestMessages(NULL),
m_pQuestLinks(NULL),
m_paszQuestSounds(NULL)
{
	m_szQuestScript = "NONE";
}

CvQuestInfo::~CvQuestInfo()
{
	reset();
}


void CvQuestInfo::reset()
{
	CvInfoBase::reset();
	SAFE_DELETE_ARRAY(m_paszQuestMessages);
	SAFE_DELETE_ARRAY(m_pQuestLinks);
	SAFE_DELETE_ARRAY(m_paszQuestSounds);
}

bool CvQuestInfo::initQuestLinks(int iNum)
{
	reset();
	if ( iNum > 0 )
	{
		m_pQuestLinks = new QuestLink[iNum];
		m_iNumQuestLinks = iNum;
		return true;
	}
	return false;
}

const TCHAR* CvQuestInfo::getQuestObjective() const
{
	return m_szQuestObjective;
}

const TCHAR* CvQuestInfo::getQuestBodyText() const
{
	return m_szQuestBodyText;
}

int CvQuestInfo::getNumQuestMessages() const
{
	return m_iNumQuestMessages;
}

const TCHAR* CvQuestInfo::getQuestMessages(int iIndex) const
{
	return m_paszQuestMessages ? m_paszQuestMessages[iIndex] : "";
}

int CvQuestInfo::getNumQuestLinks() const
{
	return m_iNumQuestLinks;
}

const TCHAR* CvQuestInfo::getQuestLinkType(int iIndex)  const
{
	return m_pQuestLinks[iIndex].m_szQuestLinkType;
}

const TCHAR* CvQuestInfo::getQuestLinkName(int iIndex)  const
{
	return m_pQuestLinks[iIndex].m_szQuestLinkName;
}

int CvQuestInfo::getNumQuestSounds() const
{
	return m_iNumQuestSounds;
}

const TCHAR* CvQuestInfo::getQuestSounds(int iIndex) const
{
	return m_paszQuestSounds ? m_paszQuestSounds[iIndex] : "";
}

const TCHAR* CvQuestInfo::getQuestScript() const
{
	return m_szQuestScript;
}

void CvQuestInfo::setQuestObjective(const TCHAR* szText)
{
	m_szQuestObjective = szText;
}

void CvQuestInfo::setQuestBodyText(const TCHAR* szText)
{
	m_szQuestBodyText = szText;
}

void CvQuestInfo::setNumQuestMessages(int iNum)
{
	m_iNumQuestMessages = iNum;
}

void CvQuestInfo::setQuestMessages(int iIndex, const TCHAR* szText)
{
	m_paszQuestMessages[iIndex] = szText;
}

void CvQuestInfo::setNumQuestSounds(int iNum)
{
	m_iNumQuestSounds = iNum;
}

void CvQuestInfo::setQuestSounds(int iIndex, const TCHAR* szText)
{
	m_paszQuestSounds[iIndex] = szText;
}

void CvQuestInfo::setQuestScript(const TCHAR* szText)
{
	m_szQuestScript = szText;
}

bool CvQuestInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"QuestObjective");
	setQuestObjective(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"QuestBodyText");
	setQuestBodyText(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"QuestMessages"))
	{
		pXML->SetStringList(&m_paszQuestMessages, &m_iNumQuestMessages);
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"QuestLinks"))
	{
		int iNum;
		iNum = pXML->GetXmlChildrenNumber(L"QuestLink");

		if (initQuestLinks(iNum))
		{
			int i;
			for (i=0; i<m_iNumQuestLinks; i++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"QuestLinkType");
				m_pQuestLinks[i].m_szQuestLinkType = szTextVal;

				pXML->GetChildXmlValByName(szTextVal, L"QuestLinkName");
				m_pQuestLinks[i].m_szQuestLinkName = szTextVal;

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}
		}
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"QuestSounds"))
	{
		pXML->SetStringList(&m_paszQuestSounds, &m_iNumQuestSounds);
		pXML->MoveToXmlParent();
	}

	pXML->GetChildXmlValByName(szTextVal, L"QuestScript");
	setQuestScript(szTextVal);

	return true;
}
void CvQuestInfo::copyNonDefaults(CvQuestInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getQuestObjective() == cDefault) setQuestObjective(pClassInfo->getQuestObjective());
	if (getQuestBodyText() == cDefault) setQuestBodyText(pClassInfo->getQuestBodyText());

	// Add new QuestMessages
	if ( pClassInfo->getNumQuestMessages() != 0 )
	{
		CvString* m_paszNewMessages = new CvString[pClassInfo->getNumQuestMessages()];
		for ( int i = 0; i < pClassInfo->getNumQuestMessages(); i++)
		{
			m_paszNewMessages[i] = pClassInfo->getQuestMessages(i);
		}

		CvXMLLoadUtilityModTools* pCurrentInfoClass = new CvXMLLoadUtilityModTools;
		pCurrentInfoClass->StringArrayExtend(&m_paszQuestMessages, &m_iNumQuestMessages,
										 &m_paszNewMessages, pClassInfo->getNumQuestMessages());
		delete pCurrentInfoClass;
		SAFE_DELETE_ARRAY(m_paszNewMessages)
	}

	if ( pClassInfo->getNumQuestLinks() > 0 )
	{
		int m_iNumQuestSoundsTemp = m_iNumQuestSounds + pClassInfo->getNumQuestLinks();
		QuestLink* m_pQuestLinksTemp = new QuestLink[m_iNumQuestSoundsTemp];
		int iCurrentClass = 0;

		for ( int i = 0; i < m_iNumQuestSoundsTemp; i++ )
		{
			if ( i < pClassInfo->getNumQuestLinks() )
			{
				m_pQuestLinksTemp[i].m_szQuestLinkType = pClassInfo->getQuestLinkType(i);
				m_pQuestLinksTemp[i].m_szQuestLinkName = pClassInfo->getQuestLinkName(i);
			}
			else
			{
				m_pQuestLinksTemp[i].m_szQuestLinkType = getQuestLinkType(iCurrentClass);
				m_pQuestLinksTemp[i].m_szQuestLinkName = getQuestLinkName(iCurrentClass);
				iCurrentClass++;
			}
		}
		SAFE_DELETE_ARRAY(m_pQuestLinks);
		m_pQuestLinks = new QuestLink[m_iNumQuestSoundsTemp];

		for ( int i = 0; i < m_iNumQuestSoundsTemp; i++)
		{
			m_pQuestLinks[i].m_szQuestLinkType = m_pQuestLinksTemp[i].m_szQuestLinkType;
			m_pQuestLinks[i].m_szQuestLinkName = m_pQuestLinksTemp[i].m_szQuestLinkName;
		}
		SAFE_DELETE_ARRAY(m_pQuestLinksTemp);
	}

	// Add new QuestSounds
	if ( pClassInfo->getNumQuestSounds() != 0 )
	{
		CvString* m_paszNewSounds = new CvString[pClassInfo->getNumQuestSounds()];
		for ( int i = 0; i < pClassInfo->getNumQuestSounds(); i++)
		{
			m_paszNewSounds[i] = pClassInfo->getQuestSounds(i);
		}

		CvXMLLoadUtilityModTools* pCurrentInfoClass = new CvXMLLoadUtilityModTools;
		pCurrentInfoClass->StringArrayExtend(&m_paszQuestSounds, &m_iNumQuestSounds,
										 &m_paszNewSounds, pClassInfo->getNumQuestSounds());
		delete pCurrentInfoClass;
		SAFE_DELETE_ARRAY(m_paszNewSounds)
	}

	if (getQuestScript() == cDefault) setQuestScript(pClassInfo->getQuestScript());
}

//////////////////////////////////////////////////////////////////////////
//
//	CvTutorialMessage
//
CvTutorialMessage::CvTutorialMessage() :
m_iNumTutorialScripts(0),
m_paszTutorialScripts(NULL)
{
	m_szTutorialMessageText = "No Text";
	m_szTutorialMessageImage = "No Text";
	m_szTutorialMessageSound = "No Text";
}

CvTutorialMessage::~CvTutorialMessage()
{
	SAFE_DELETE_ARRAY(m_paszTutorialScripts);
}

const TCHAR* CvTutorialMessage::getText() const
{
	return m_szTutorialMessageText;
}

const TCHAR* CvTutorialMessage::getImage() const
{
	return m_szTutorialMessageImage;
}

const TCHAR* CvTutorialMessage::getSound() const
{
	return m_szTutorialMessageSound;
}

void CvTutorialMessage::setText(const TCHAR* szText)
{
	m_szTutorialMessageText = szText;
}

void CvTutorialMessage::setImage(const TCHAR* szText)
{
	m_szTutorialMessageImage = szText;
}

void CvTutorialMessage::setSound(const TCHAR* szText)
{
	m_szTutorialMessageSound = szText;
}

int CvTutorialMessage::getNumTutorialScripts() const
{
	return m_iNumTutorialScripts;
}

const TCHAR* CvTutorialMessage::getTutorialScriptByIndex(int i) const
{
	return m_paszTutorialScripts[i];
}

bool CvTutorialMessage::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;

	if (pXML->GetChildXmlValByName(szTextVal, L"TutorialMessageText"))
		setText(szTextVal);

	if (pXML->GetChildXmlValByName(szTextVal, L"TutorialMessageImage"))
		setImage(szTextVal);

	if (pXML->GetChildXmlValByName(szTextVal, L"TutorialMessageSound"))
		setSound(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"TutorialScripts"))
	{
		pXML->SetStringList(&m_paszTutorialScripts, &m_iNumTutorialScripts);
		pXML->MoveToXmlParent();
	}

	return true;
}


CvTutorialInfo::CvTutorialInfo() :
m_iNumTutorialMessages(0),
m_paTutorialMessages(NULL)
{
	m_szNextTutorialInfoType = "NONE";
}

CvTutorialInfo::~CvTutorialInfo()
{
	resetMessages();
}

const TCHAR* CvTutorialInfo::getNextTutorialInfoType()
{
	return m_szNextTutorialInfoType;
}

void CvTutorialInfo::setNextTutorialInfoType(const TCHAR* szVal)
{
	m_szNextTutorialInfoType = szVal;
}

bool CvTutorialInfo::initTutorialMessages(int iNum)
{
	resetMessages();
	m_paTutorialMessages = new CvTutorialMessage[iNum];
	m_iNumTutorialMessages = iNum;
	return true;
}

void CvTutorialInfo::resetMessages()
{
	SAFE_DELETE_ARRAY(m_paTutorialMessages);
	m_iNumTutorialMessages = 0;
}

int CvTutorialInfo::getNumTutorialMessages() const
{
	return m_iNumTutorialMessages;
}

const CvTutorialMessage* CvTutorialInfo::getTutorialMessage(int iIndex) const
{
	return &m_paTutorialMessages[iIndex];
}

bool CvTutorialInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	CvString szTextVal;

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NextTutorialInfoType");
	setNextTutorialInfoType(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"TutorialMessages"))
	{
		int iNum;
		iNum = pXML->GetXmlChildrenNumber(L"TutorialMessage");
		if ( iNum > 0 )
		{
			pXML->TryMoveToXmlFirstChild(L"TutorialMessage");
			initTutorialMessages(iNum);
			for (int i = 0; i<m_iNumTutorialMessages; i++)
			{
				if (!m_paTutorialMessages[i].read(pXML))
				{
					return false;
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}
	return true;
}
void CvTutorialInfo::copyNonDefaults(CvTutorialInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getNextTutorialInfoType() == cDefault) setNextTutorialInfoType(pClassInfo->getNextTutorialInfoType());

/*	if ( pClassInfo->getNumTutorialMessages() > 0 )
	{
		int m_iNumTutorialMessagesTemp = m_iNumTutorialMessages + pClassInfo->getNumTutorialMessages();
		CvTutorialMessage* m_paTutorialMessagesTemp = new CvTutorialMessage[m_iNumTutorialMessagesTemp];
		int iCurrentClass = 0;

		for ( int i = 0; i < m_iNumTutorialMessagesTemp; i++ )
		{
			if ( i < pClassInfo->getNumTutorialMessages() )
			{
				m_paTutorialMessagesTemp[i] = pClassInfo->getTutorialMessage(i);
			}
			else
			{
				m_paTutorialMessagesTemp[i] = getTutorialMessage(iCurrentClass);
				iCurrentClass++;
			}
		}
		SAFE_DELETE_ARRAY(m_paTutorialMessages);
		m_paTutorialMessages = new CvTutorialMessage[m_iNumTutorialMessagesTemp];

		for ( int i = 0; i < m_iNumTutorialMessagesTemp; i++ )
		{
			m_paTutorialMessages[i] = m_paTutorialMessagesTemp[i];
		}
		SAFE_DELETE_ARRAY(m_paTutorialMessagesTemp);
	}
*/
}

//////////////////////////////////////////////////////////////////////////
//
//	CvGameOptionInfo
//	Game options and their default values
//
//
CvGameOptionInfo::CvGameOptionInfo() :
m_bDefault(false),
m_bVisible(true)
{
}

CvGameOptionInfo::~CvGameOptionInfo()
{
	for (int i=0; i<(int)m_aEnforcesGameOptionOnTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aEnforcesGameOptionOnTypes[i]));
	}
	for (int i=0; i<(int)m_aEnforcesGameOptionOffTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aEnforcesGameOptionOffTypes[i]));
	}
}

bool CvGameOptionInfo::getDefault() const
{
	return m_bDefault;
}

bool CvGameOptionInfo::getVisible() const
{
	return m_bVisible;
}

int CvGameOptionInfo::getNumEnforcesGameOptionOnTypes() const
{
	return (int)m_aEnforcesGameOptionOnTypes.size();
}

GameOptionTypeBool& CvGameOptionInfo::isEnforcesGameOptionOnType(int iOption)
{
	FAssertMsg(iOption < (int)m_aEnforcesGameOptionOnTypes.size(), "Index out of bounds");
	FAssertMsg(iOption > -1, "Index out of bounds");

	return m_aEnforcesGameOptionOnTypes[iOption];
}

int CvGameOptionInfo::getNumEnforcesGameOptionOffTypes() const
{
	return (int)m_aEnforcesGameOptionOffTypes.size();
}

GameOptionTypeBool& CvGameOptionInfo::isEnforcesGameOptionOffType(int iOption)
{
	FAssertMsg(iOption < (int)m_aEnforcesGameOptionOffTypes.size(), "Index out of bounds");
	FAssertMsg(iOption > -1, "Index out of bounds");

	return m_aEnforcesGameOptionOffTypes[iOption];
}

bool CvGameOptionInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	CvString szTextVal;

	pXML->GetOptionalChildXmlValByName(&m_bDefault, L"bDefault");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible", true);


	if(pXML->TryMoveToXmlFirstChild(L"EnforcesGameOptionOnTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"EnforcesGameOptionOnType" );
		m_aEnforcesGameOptionOnTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"EnforcesGameOptionOnType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"GameOptionType");
					pXML->GetChildXmlValByName(&(m_aEnforcesGameOptionOnTypes[i].bBool), L"bEnforcesGameOptionOn");
					GC.addDelayedResolution((int*)&(m_aEnforcesGameOptionOnTypes[i].eGameOption), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"EnforcesGameOptionOnType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"EnforcesGameOptionOffTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"EnforcesGameOptionOffType" );
		m_aEnforcesGameOptionOffTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"EnforcesGameOptionOffType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"GameOptionType");
					pXML->GetChildXmlValByName(&(m_aEnforcesGameOptionOffTypes[i].bBool), L"bEnforcesGameOptionOff");
					GC.addDelayedResolution((int*)&(m_aEnforcesGameOptionOffTypes[i].eGameOption), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"EnforcesGameOptionOffType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}
	return true;
}

void CvGameOptionInfo::copyNonDefaults(CvGameOptionInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
	if (getVisible()) m_bVisible = pClassInfo->getVisible();

	//TB's Tags
	if (getNumEnforcesGameOptionOnTypes() == 0)
	{
		int iNum = pClassInfo->getNumEnforcesGameOptionOnTypes();
		m_aEnforcesGameOptionOnTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aEnforcesGameOptionOnTypes[i].bBool = pClassInfo->m_aEnforcesGameOptionOnTypes[i].bBool;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aEnforcesGameOptionOnTypes[i].eGameOption), (int*)&(pClassInfo->m_aEnforcesGameOptionOnTypes[i].eGameOption));
		}
	}

	if (getNumEnforcesGameOptionOffTypes() == 0)
	{
		int iNum = pClassInfo->getNumEnforcesGameOptionOffTypes();
		m_aEnforcesGameOptionOffTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aEnforcesGameOptionOffTypes[i].bBool = pClassInfo->m_aEnforcesGameOptionOffTypes[i].bBool;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aEnforcesGameOptionOffTypes[i].eGameOption), (int*)&(pClassInfo->m_aEnforcesGameOptionOffTypes[i].eGameOption));
		}
	}
}

void CvGameOptionInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_bDefault);
	CheckSum(iSum, m_bVisible);

	//TB's Tags
	int i;
	int iNumElements;

	iNumElements = m_aEnforcesGameOptionOnTypes.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aEnforcesGameOptionOnTypes[i].eGameOption);
		CheckSum(iSum, m_aEnforcesGameOptionOnTypes[i].bBool);
	}

	iNumElements = m_aEnforcesGameOptionOffTypes.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aEnforcesGameOptionOffTypes[i].eGameOption);
		CheckSum(iSum, m_aEnforcesGameOptionOffTypes[i].bBool);
	}
}

//////////////////////////////////////////////////////////////////////////
//
//	CvMPOptionInfo
//	Multiplayer options and their default values
//
//
CvMPOptionInfo::CvMPOptionInfo() :
m_bDefault(false)
{
}

CvMPOptionInfo::~CvMPOptionInfo()
{
}

bool CvMPOptionInfo::getDefault() const
{
	return m_bDefault;
}

bool CvMPOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bDefault, L"bDefault");

	return true;
}

void CvMPOptionInfo::copyNonDefaults(CvMPOptionInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
}

void CvMPOptionInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_bDefault);
}

//////////////////////////////////////////////////////////////////////////
//
//	CvForceControlInfo
//	Forced Controls and their default values
//
//
CvForceControlInfo::CvForceControlInfo() :
m_bDefault(false)
{
}

CvForceControlInfo::~CvForceControlInfo()
{
}

bool CvForceControlInfo::getDefault() const
{
	return m_bDefault;
}

bool CvForceControlInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bDefault, L"bDefault");

	return true;
}

void CvForceControlInfo::copyNonDefaults(CvForceControlInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
}

void CvForceControlInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_bDefault);
}

//////////////////////////////////////////////////////////////////////////
//
//	CvPlayerOptionInfo
//	Player options and their default values
//
//
CvPlayerOptionInfo::CvPlayerOptionInfo() :
m_bDefault(false)
{
}

CvPlayerOptionInfo::~CvPlayerOptionInfo()
{
}

bool CvPlayerOptionInfo::getDefault() const
{
	return m_bDefault;
}

bool CvPlayerOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bDefault, L"bDefault");

	return true;
}

void CvPlayerOptionInfo::copyNonDefaults(CvPlayerOptionInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
}

void CvPlayerOptionInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_bDefault);
}

//////////////////////////////////////////////////////////////////////////
//
//	CvGraphicOptionInfo
//	Graphic options and their default values
//
//
CvGraphicOptionInfo::CvGraphicOptionInfo() :
m_bDefault(false)
{
}

CvGraphicOptionInfo::~CvGraphicOptionInfo()
{
}

bool CvGraphicOptionInfo::getDefault() const
{
	return m_bDefault;
}

bool CvGraphicOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bDefault, L"bDefault");

	return true;
}

void CvGraphicOptionInfo::copyNonDefaults(CvGraphicOptionInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
}

void CvGraphicOptionInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_bDefault);
}

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
const TCHAR* CvEventTriggerInfo::getEventArt() const
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
	FAssert(i >= 0 && i < (int)m_aszText.size());
	return m_aszText[i];
}

int CvEventTriggerInfo::getTextEra(int i) const
{
	FAssert(i >= 0 && i < (int)m_aiTextEra.size());
	return m_aiTextEra[i];
}

int CvEventTriggerInfo::getNumTexts() const
{
	FAssert(m_aiTextEra.size() == m_aszText.size());
	return m_aszText.size();
}

const CvWString& CvEventTriggerInfo::getWorldNews(int i) const
{
	FAssert(i >= 0 && i < (int)m_aszWorldNews.size());
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

CvProperties* CvEventTriggerInfo::getPrereqMinProperties()
{
	return &m_PrereqMinProperties;
}

CvProperties* CvEventTriggerInfo::getPrereqMaxProperties()
{
	return &m_PrereqMaxProperties;
}

CvProperties* CvEventTriggerInfo::getPrereqPlayerMinProperties()
{
	return &m_PrereqPlayerMinProperties;
}

CvProperties* CvEventTriggerInfo::getPrereqPlayerMaxProperties()
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

bool CvEventTriggerInfo::isNotOnGameOption(int i)
{
	FAssert (i > -1 && i < GC.getNumGameOptions());
	if (find(m_aiNotOnGameOptions.begin(), m_aiNotOnGameOptions.end(), i) == m_aiNotOnGameOptions.end())
	{
		return false;
	}
	return true;
}

int CvEventTriggerInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}

int CvEventTriggerInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}

bool CvEventTriggerInfo::isOnGameOption(int i)
{
	FAssert (i > -1 && i < GC.getNumGameOptions());
	if (find(m_aiOnGameOptions.begin(), m_aiOnGameOptions.end(), i) == m_aiOnGameOptions.end())
	{
		return false;
	}
	return true;
}

//TBSpot
void CvEventTriggerInfo::getCheckSum(unsigned int& iSum)
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
	MEMORY_TRACE_FUNCTION();

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
	pXML->SetOptionalIntVector(&m_aiUnitsRequired, L"UnitsRequired");
	pXML->SetOptionalIntVector(&m_aiBuildingsRequired, L"BuildingsRequired");
	pXML->SetOptionalIntVector(&m_aiPrereqOrTechs, L"OrPreReqs");
	pXML->SetOptionalIntVector(&m_aiPrereqAndTechs, L"AndPreReqs");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"OtherPlayerHasTech");
	m_iOtherPlayerHasTech = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Civic");
	m_iCivic = pXML->GetInfoClass(szTextVal);

	pXML->SetOptionalIntVector(&m_aiObsoleteTechs, L"ObsoleteTechs");
	pXML->SetOptionalIntVector(&m_aiEvents, L"Events");
	pXML->SetOptionalIntVector(&m_aiPrereqEvents, L"PrereqEvents");
	pXML->SetOptionalIntVector(&m_aiFeaturesRequired, L"FeaturesRequired");
	pXML->SetOptionalIntVector(&m_aiTerrainsRequired, L"TerrainsRequired");
	pXML->SetOptionalIntVector(&m_aiImprovementsRequired, L"ImprovementsRequired");
	pXML->SetOptionalIntVector(&m_aiBonusesRequired, L"BonusesRequired");
	pXML->SetOptionalIntVector(&m_aiRoutesRequired, L"RoutesRequired");
	pXML->SetOptionalIntVector(&m_aiReligionsRequired, L"ReligionsRequired");
	pXML->SetOptionalIntVector(&m_aiCorporationsRequired, L"CorporationsRequired");

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
	pXML->SetOptionalIntVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalIntVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	return true;
}

void CvEventTriggerInfo::copyNonDefaults(CvEventTriggerInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

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

	for ( int i = 0; i < pClassInfo->getNumUnitsRequired(); i++)
	{
		if ( pClassInfo->getUnitRequired(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumUnitsRequired(), &m_aiUnitsRequired[0], pClassInfo->getUnitRequired(i))))
			{
				m_aiUnitsRequired.push_back(pClassInfo->getUnitRequired(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}

	for ( int i = 0; i < pClassInfo->getNumBuildingsRequired(); i++)
	{
		if ( pClassInfo->getBuildingRequired(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumBuildingsRequired(), &m_aiBuildingsRequired[0], pClassInfo->getBuildingRequired(i))))
			{
				m_aiBuildingsRequired.push_back(pClassInfo->getBuildingRequired(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}

	for ( int i = 0; i < pClassInfo->getNumPrereqOrTechs(); i++)
	{
		if ( pClassInfo->getPrereqOrTechs(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumPrereqOrTechs(), &m_aiPrereqOrTechs[0], pClassInfo->getPrereqOrTechs(i))))
			{
				m_aiPrereqOrTechs.push_back(pClassInfo->getPrereqOrTechs(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}

	for ( int i = 0; i < pClassInfo->getNumPrereqAndTechs(); i++)
	{
		if ( pClassInfo->getPrereqAndTechs(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumPrereqAndTechs(), &m_aiPrereqAndTechs[0], pClassInfo->getPrereqAndTechs(i))))
			{
				m_aiPrereqAndTechs.push_back(pClassInfo->getPrereqAndTechs(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}

	if (getOtherPlayerHasTech() == iTextDefault) m_iOtherPlayerHasTech = pClassInfo->getOtherPlayerHasTech();
	if (getCivic() == iTextDefault) m_iCivic = pClassInfo->getCivic();

	for ( int i = 0; i < pClassInfo->getNumObsoleteTechs(); i++)
	{
		if ( pClassInfo->getObsoleteTech(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumObsoleteTechs(), &m_aiObsoleteTechs[0], pClassInfo->getObsoleteTech(i))))
			{
				m_aiObsoleteTechs.push_back(pClassInfo->getObsoleteTech(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}

	for ( int i = 0; i < pClassInfo->getNumEvents(); i++)
	{
		if ( pClassInfo->getEvent(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumEvents(), &m_aiEvents[0], pClassInfo->getEvent(i))))
			{
				m_aiEvents.push_back(pClassInfo->getEvent(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}

	for ( int i = 0; i < pClassInfo->getNumPrereqEvents(); i++)
	{
		if ( pClassInfo->getPrereqEvent(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumPrereqEvents(), &m_aiPrereqEvents[0], pClassInfo->getPrereqEvent(i))))
			{
				m_aiPrereqEvents.push_back(pClassInfo->getPrereqEvent(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}

	for ( int i = 0; i < pClassInfo->getNumFeaturesRequired(); i++)
	{
		if ( pClassInfo->getFeatureRequired(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumFeaturesRequired(), &m_aiFeaturesRequired[0], pClassInfo->getFeatureRequired(i))))
			{
				m_aiFeaturesRequired.push_back(pClassInfo->getFeatureRequired(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}

	for ( int i = 0; i < pClassInfo->getNumTerrainsRequired(); i++)
	{
		if ( pClassInfo->getTerrainRequired(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumTerrainsRequired(), &m_aiTerrainsRequired[0], pClassInfo->getTerrainRequired(i))))
			{
				m_aiTerrainsRequired.push_back(pClassInfo->getTerrainRequired(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}
	for ( int i = 0; i < pClassInfo->getNumImprovementsRequired(); i++)
	{
		if ( pClassInfo->getImprovementRequired(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumImprovementsRequired(), &m_aiImprovementsRequired[0], pClassInfo->getImprovementRequired(i))))
			{
				m_aiImprovementsRequired.push_back(pClassInfo->getImprovementRequired(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}
	for ( int i = 0; i < pClassInfo->getNumBonusesRequired(); i++)
	{
		if ( pClassInfo->getBonusRequired(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumBonusesRequired(), &m_aiBonusesRequired[0], pClassInfo->getBonusRequired(i))))
			{
				m_aiImprovementsRequired.push_back(pClassInfo->getBonusRequired(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}
	for ( int i = 0; i < pClassInfo->getNumRoutesRequired(); i++)
	{
		if ( pClassInfo->getRouteRequired(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumRoutesRequired(), &m_aiRoutesRequired[0], pClassInfo->getRouteRequired(i))))
			{
				m_aiImprovementsRequired.push_back(pClassInfo->getRouteRequired(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}
	for ( int i = 0; i < pClassInfo->getNumReligionsRequired(); i++)
	{
		if ( pClassInfo->getReligionRequired(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumReligionsRequired(), &m_aiReligionsRequired[0], pClassInfo->getReligionRequired(i))))
			{
				m_aiImprovementsRequired.push_back(pClassInfo->getReligionRequired(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}
	for ( int i = 0; i < pClassInfo->getNumCorporationsRequired(); i++)
	{
		if ( pClassInfo->getCorporationRequired(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumCorporationsRequired(), &m_aiCorporationsRequired[0], pClassInfo->getCorporationRequired(i))))
			{
				m_aiImprovementsRequired.push_back(pClassInfo->getCorporationRequired(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}

	for ( int i = 0; i < pClassInfo->getNumTexts(); i++)
	{
		if ( pClassInfo->getText(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumTexts(), &m_aszText[0], pClassInfo->getText(i))))
			{
				m_aszText.push_back(pClassInfo->getText(i));
				m_aiTextEra.push_back(pClassInfo->getTextEra(i));
			}
			else //If dupe exist, we want to overwrite whatever it is(assuming the modder changed the Era on purpose)
			{
				m_aszText[i] = pClassInfo->getText(i);
				m_aiTextEra[i] = pClassInfo->getTextEra(i);
			}
			delete pCurrentUnitClass;
		}
	}

	for ( int i = 0; i < pClassInfo->getNumWorldNews(); i++)
	{
		if ( pClassInfo->getWorldNews(i) != NULL)
		{
			CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
			if (!(pCurrentUnitClass->isDuplicate(getNumWorldNews(), &m_aszWorldNews[0], pClassInfo->getWorldNews(i))))
			{
				m_aszWorldNews.push_back(pClassInfo->getWorldNews(i));
			}
			delete pCurrentUnitClass;
			//no need to do anything if a dupe has been found!
		}
	}

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

	m_PrereqMinProperties.copyNonDefaults(pClassInfo->getPrereqMinProperties(), pXML);
	m_PrereqMaxProperties.copyNonDefaults(pClassInfo->getPrereqMaxProperties(), pXML);
	m_PrereqPlayerMinProperties.copyNonDefaults(pClassInfo->getPrereqPlayerMinProperties(), pXML);
	m_PrereqPlayerMaxProperties.copyNonDefaults(pClassInfo->getPrereqPlayerMaxProperties(), pXML);

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
	m_iGold(0),
	m_iRandomGold(0),
	m_iCulture(0),
	m_iEspionagePoints(0),
	m_iTech(NO_TECH),
	m_iTechPercent(0),
	m_iTechCostPercent(0),
	m_iTechMinTurnsLeft(0),
	m_iPrereqTech(NO_TECH),
	m_iUnitClass(NO_UNITCLASS),
	m_iNumUnits(0),
	m_iUnitExperience(0),
	m_iUnitImmobileTurns(0),
	m_iBuildingClass(NO_BUILDINGCLASS),
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
	m_iUnitPromotion(NO_PROMOTION),
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
	m_piUnitClassPromotions(NULL),
	m_piCommerceModifier(NULL),
	m_piYieldModifier(NULL)
/************************************************************************************************/
/* Afforess					  Start		 01/20/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	,m_iPrereqGameOption(NO_GAMEOPTION)
	,m_iRevolutionIndexChange(0)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
{
}

CvEventInfo::~CvEventInfo()
{
	SAFE_DELETE_ARRAY(m_piTechFlavorValue);
	SAFE_DELETE_ARRAY(m_piPlotExtraYields);
	SAFE_DELETE_ARRAY(m_piFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_piAdditionalEventChance);
	SAFE_DELETE_ARRAY(m_piAdditionalEventTime);
	SAFE_DELETE_ARRAY(m_piClearEventChance);
	SAFE_DELETE_ARRAY(m_piUnitCombatPromotions);
	SAFE_DELETE_ARRAY(m_piUnitClassPromotions);
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

int CvEventInfo::getPrereqTech() const
{
	return m_iPrereqTech;
}

int CvEventInfo::getUnitClass() const
{
	return m_iUnitClass;
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

int CvEventInfo::getBuildingClass() const
{
	return m_iBuildingClass;
}

int CvEventInfo::getBuildingChange() const
{
	return m_iBuildingChange;
}

int CvEventInfo::getCommerceModifier(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}

int CvEventInfo::getYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
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

int CvEventInfo::getImprovement() const
{
	return m_iImprovement;
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

int CvEventInfo::getUnitPromotion() const
{
	return m_iUnitPromotion;
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
	FAssert (i >= 0 && i < GC.getNumEventInfos());
	return m_piAdditionalEventChance ? m_piAdditionalEventChance[i] : 0;
}

int CvEventInfo::getAdditionalEventTime(int i) const
{
	FAssert (i >= 0 && i < GC.getNumEventInfos());
	return m_piAdditionalEventTime ? m_piAdditionalEventTime[i] : 0;
}

int CvEventInfo::getClearEventChance(int i) const
{
	FAssert (i >= 0 && i < GC.getNumEventInfos());
	return m_piClearEventChance ? m_piClearEventChance[i] : 0;
}

int CvEventInfo::getTechFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piTechFlavorValue ? m_piTechFlavorValue[i] : 0;
}

int CvEventInfo::getPlotExtraYield(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPlotExtraYields ? m_piPlotExtraYields[i] : 0;
}

int CvEventInfo::getFreeSpecialistCount(int i) const
{
	FAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFreeSpecialistCount ? m_piFreeSpecialistCount[i] : 0;
}

int CvEventInfo::getUnitCombatPromotion(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitCombatPromotions ? m_piUnitCombatPromotions[i] : -1;
}

int CvEventInfo::getUnitClassPromotion(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitClassPromotions ? m_piUnitClassPromotions[i] : -1;
}

const CvWString& CvEventInfo::getWorldNews(int i) const
{
	FAssert(i >= 0 && i < (int)m_aszWorldNews.size());
	return m_aszWorldNews[i];
}

int CvEventInfo::getNumWorldNews() const
{
	return m_aszWorldNews.size();
}

int CvEventInfo::getBuildingYieldChange(int iBuildingClass, int iYield) const
{
	for (std::vector<BuildingYieldChange>::const_iterator it = m_aBuildingYieldChanges.begin(); it != m_aBuildingYieldChanges.end(); ++it)
	{
		if ((*it).eBuildingClass == (BuildingClassTypes)iBuildingClass && (*it).eYield == (YieldTypes)iYield)
		{
			return (*it).iChange;
		}
	}

	return 0;
}

int CvEventInfo::getNumBuildingYieldChanges() const
{
	return m_aBuildingYieldChanges.size();
}

int CvEventInfo::getBuildingCommerceChange(int iBuildingClass, int iCommerce) const
{
	for (std::vector<BuildingCommerceChange>::const_iterator it = m_aBuildingCommerceChanges.begin(); it != m_aBuildingCommerceChanges.end(); ++it)
	{
		if ((*it).eBuildingClass == (BuildingClassTypes)iBuildingClass && (*it).eCommerce == (CommerceTypes)iCommerce)
		{
			return (*it).iChange;
		}
	}

	return 0;
}

int CvEventInfo::getNumBuildingCommerceChanges() const
{
	return m_aBuildingCommerceChanges.size();
}

int CvEventInfo::getBuildingCommerceModifier(int iBuildingClass, int iCommerce) const
{
	for (std::vector<BuildingCommerceChange>::const_iterator it = m_aBuildingCommerceModifiers.begin(); it != m_aBuildingCommerceModifiers.end(); ++it)
	{
		if ((*it).eBuildingClass == (BuildingClassTypes)iBuildingClass && (*it).eCommerce == (CommerceTypes)iCommerce)
		{
			return (*it).iChange;
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

int CvEventInfo::getBuildingHappyChange(int iBuildingClass) const
{
	for (BuildingChangeArray::const_iterator it = m_aBuildingHappyChanges.begin(); it != m_aBuildingHappyChanges.end(); ++it)
	{
		if ((*it).first == (BuildingClassTypes)iBuildingClass)
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

int CvEventInfo::getBuildingHealthChange(int iBuildingClass) const
{
	for (BuildingChangeArray::const_iterator it = m_aBuildingHealthChanges.begin(); it != m_aBuildingHealthChanges.end(); ++it)
	{
		if ((*it).first == (BuildingClassTypes)iBuildingClass)
		{
			return (*it).second;
		}
	}

	return 0;
}

CvProperties* CvEventInfo::getProperties()
{
	return &m_Properties;
}

CvProperties* CvEventInfo::getPropertiesAllCities()
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

const wchar* CvEventInfo::getUnitNameKey() const
{
	return m_szUnitName;
}

const wchar* CvEventInfo::getQuestFailTextKey() const
{
	return m_szQuestFailText;
}

const wchar* CvEventInfo::getLocalInfoTextKey() const
{
	return m_szLocalInfoText;
}

const wchar* CvEventInfo::getOtherPlayerPopup() const
{
	return m_szOtherPlayerPopup;
}

/************************************************************************************************/
/* Afforess					  Start		 01/20/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
int CvEventInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}
int CvEventInfo::getRevolutionIndexChange() const
{
	return m_iRevolutionIndexChange;
}

int CvEventInfo::getAdditionalEventChanceVectorSize()					{return m_aszAdditionalEventChanceforPass3.size();}
CvString CvEventInfo::getAdditionalEventChanceNamesVectorElement(int i)	{return m_aszAdditionalEventChanceforPass3[i];}
int CvEventInfo::getAdditionalEventChanceValuesVectorElement(int i)		{return m_aiAdditionalEventChanceforPass3[i];}

int CvEventInfo::getAdditionalEventTimeVectorSize()					{return m_aszAdditionalEventTimeforPass3.size();}
CvString CvEventInfo::getAdditionalEventTimeNamesVectorElement(int i)	{return m_aszAdditionalEventTimeforPass3[i];}
int CvEventInfo::getAdditionalEventTimeValuesVectorElement(int i)		{return m_aiAdditionalEventTimeforPass3[i];}

int CvEventInfo::getClearEventChanceVectorSize()					{return m_aszClearEventChanceforPass3.size();}
CvString CvEventInfo::getClearEventChanceNamesVectorElement(int i)	{return m_aszClearEventChanceforPass3[i];}
int CvEventInfo::getClearEventChanceValuesVectorElement(int i)		{return m_aiClearEventChanceforPass3[i];}

/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

void CvEventInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_bQuest);
	CheckSum(iSum, m_bGlobal);
	CheckSum(iSum, m_bTeam);
	CheckSum(iSum, m_bCityEffect);
	CheckSum(iSum, m_bOtherPlayerCityEffect);
	CheckSum(iSum, m_bGoldToPlayer);
	CheckSum(iSum, m_bGoldenAge);
	CheckSum(iSum, m_bDeclareWar);
	CheckSum(iSum, m_bDisbandUnit);
	CheckSum(iSum, m_iGold);
	CheckSum(iSum, m_iRandomGold);
	CheckSum(iSum, m_iCulture);
	CheckSum(iSum, m_iEspionagePoints);
	CheckSum(iSum, m_iTech);
	CheckSum(iSum, m_iTechPercent);
	CheckSum(iSum, m_iTechCostPercent);
	CheckSum(iSum, m_iTechMinTurnsLeft);
	CheckSum(iSum, m_iPrereqTech);
	CheckSum(iSum, m_iUnitClass);
	CheckSum(iSum, m_iNumUnits);
	CheckSum(iSum, m_iUnitExperience);
	CheckSum(iSum, m_iUnitImmobileTurns);
	CheckSum(iSum, m_iBuildingClass);
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
	CheckSum(iSum, m_iUnitPromotion);
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
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_piUnitClassPromotions);

	for (std::vector<BuildingYieldChange>::iterator it = m_aBuildingYieldChanges.begin(); it != m_aBuildingYieldChanges.end(); ++it)
	{
		CheckSum(iSum, (*it).eBuildingClass);
		CheckSum(iSum, (*it).eYield);
		CheckSum(iSum, (*it).iChange);
	}

	for (std::vector<BuildingCommerceChange>::iterator it = m_aBuildingCommerceChanges.begin(); it != m_aBuildingCommerceChanges.end(); ++it)
	{
		CheckSum(iSum, (*it).eBuildingClass);
		CheckSum(iSum, (*it).eCommerce);
		CheckSum(iSum, (*it).iChange);
	}

	for (std::vector<BuildingCommerceChange>::iterator it = m_aBuildingCommerceModifiers.begin(); it != m_aBuildingCommerceModifiers.end(); ++it)
	{
		CheckSum(iSum, (*it).eBuildingClass);
		CheckSum(iSum, (*it).eCommerce);
		CheckSum(iSum, (*it).iChange);
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
	MEMORY_TRACE_FUNCTION();

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
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_iPrereqTech = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"UnitClass");
	m_iUnitClass = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iNumUnits, L"iNumFreeUnits");
	pXML->GetOptionalChildXmlValByName(&m_bDisbandUnit, L"bDisbandUnit");
	pXML->GetOptionalChildXmlValByName(&m_iUnitExperience, L"iUnitExperience");
	pXML->GetOptionalChildXmlValByName(&m_iUnitImmobileTurns, L"iUnitImmobileTurns");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"BuildingClass");
	m_iBuildingClass = pXML->GetInfoClass(szTextVal);
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

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ImprovementType");
	m_iImprovement = pXML->GetInfoClass(szTextVal);
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

	pXML->SetVariableListTagPair(&m_piTechFlavorValue, L"TechFlavors", GC.getFlavorTypes(), GC.getNumFlavorTypes());
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
	pXML->GetOptionalChildXmlValByName(szTextVal, L"UnitPromotion");
	m_iUnitPromotion = pXML->GetInfoClass(szTextVal);
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

	FAssertMsg(NULL == m_piUnitClassPromotions, "Memory leak");
	CvXMLLoadUtility::InitList(&m_piUnitClassPromotions,GC.getNumUnitClassInfos(),-1);
	pXML->SetVariableListTagPair(&pszPromotions, L"UnitClassPromotions", GC.getNumUnitClassInfos(), "NONE");
	if ( pszPromotions != NULL )
	{
		for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
		{
			m_piUnitClassPromotions[i] = pXML->GetInfoClass(pszPromotions[i]);
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

	m_aBuildingYieldChanges.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingExtraYields"))
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
						BuildingYieldChange kChange;
						kChange.eBuildingClass = (BuildingClassTypes)pXML->GetInfoClass(szTextVal);
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
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingCommerceChange kChange;
						kChange.eBuildingClass = (BuildingClassTypes)pXML->GetInfoClass(szTextVal);
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
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingCommerceChange kChange;
						kChange.eBuildingClass = (BuildingClassTypes)pXML->GetInfoClass(szTextVal);
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
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingClassTypes eBuildingClass = (BuildingClassTypes)pXML->GetInfoClass(szTextVal);
						int iChange;
						pXML->GetNextXmlVal(&iChange);
						m_aBuildingHappyChanges.push_back(std::make_pair(eBuildingClass, iChange));

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
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingClassTypes eBuildingClass = (BuildingClassTypes)pXML->GetInfoClass(szTextVal);
						int iChange;
						pXML->GetNextXmlVal(&iChange);
						m_aBuildingHealthChanges.push_back(std::make_pair(eBuildingClass, iChange));

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

/************************************************************************************************/
/* Afforess					  Start		 01/20/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
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
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	return true;
}

void CvEventInfo::copyNonDefaults(CvEventInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

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
	if (getUnitClass() == iTextDefault) m_iUnitClass = pClassInfo->getUnitClass();
	if (getNumUnits() == iDefault) m_iNumUnits = pClassInfo->getNumUnits();
	if (isDisbandUnit() == bDefault) m_bDisbandUnit = pClassInfo->isDisbandUnit();
	if (getUnitExperience() == iDefault) m_iUnitExperience = pClassInfo->getUnitExperience();
	if (getUnitImmobileTurns() == iDefault) m_iUnitImmobileTurns = pClassInfo->getUnitImmobileTurns();
	if (getBuildingClass() == iTextDefault) m_iBuildingClass = pClassInfo->getBuildingClass();
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
	if (getUnitPromotion() == iTextDefault) m_iUnitPromotion = pClassInfo->getUnitPromotion();
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

	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		if (m_piUnitClassPromotions[i] == iTextDefault)
		{
			m_piUnitClassPromotions[i] = pClassInfo->getUnitClassPromotion(i);
		}
	}

	if (getUnitNameKey() == wDefault) m_szUnitName = pClassInfo->getUnitNameKey();
	if (getOtherPlayerPopup() == wDefault) m_szOtherPlayerPopup = pClassInfo->getOtherPlayerPopup();
	if (getQuestFailTextKey() == wDefault) m_szQuestFailText = pClassInfo->getQuestFailTextKey();
	if (getLocalInfoTextKey() == wDefault) m_szLocalInfoText = pClassInfo->getLocalInfoTextKey();

	m_Properties.copyNonDefaults(pClassInfo->getProperties(), pXML);
	m_PropertiesAllCities.copyNonDefaults(pClassInfo->getPropertiesAllCities(), pXML);

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
	for ( int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
	{
		for (std::vector<BuildingYieldChange>::const_iterator it = m_aBuildingYieldChanges.begin(); it != m_aBuildingYieldChanges.end(); ++it)
		{
			if ((*it).eBuildingClass == (BuildingClassTypes)iBuildingClass)
			{
				//obviously some modder already set this buildingclass to some value
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
				if ( pClassInfo->getBuildingYieldChange(iBuildingClass, iYield) != 0)
				{
					BuildingYieldChange kChange;
					kChange.eBuildingClass = (BuildingClassTypes)iBuildingClass;
					kChange.eYield = (YieldTypes)iYield;
					kChange.iChange = pClassInfo->getBuildingYieldChange(iBuildingClass, iYield);
					m_aBuildingYieldChanges.push_back(kChange);
				}
			}
		}
	}

	bNoDuplicate = true;
	for ( int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
	{
		for (std::vector<BuildingCommerceChange>::const_iterator it = m_aBuildingCommerceChanges.begin(); it != m_aBuildingCommerceChanges.end(); ++it)
		{
			if ((*it).eBuildingClass == (BuildingClassTypes)iBuildingClass)
			{
				//obviously some modder already set this buildingclass to some value
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
				if ( pClassInfo->getBuildingYieldChange(iBuildingClass, iCommerce) != 0)
				{
					BuildingCommerceChange kChange;
					kChange.eBuildingClass = (BuildingClassTypes)iBuildingClass;
					kChange.eCommerce = (CommerceTypes)iCommerce;
					kChange.iChange = pClassInfo->getBuildingCommerceChange(iBuildingClass, iCommerce);
					m_aBuildingCommerceChanges.push_back(kChange);
				}
			}
		}
	}

	bNoDuplicate = true;
	for ( int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
	{
		for (std::vector<BuildingCommerceChange>::const_iterator it = m_aBuildingCommerceModifiers.begin(); it != m_aBuildingCommerceModifiers.end(); ++it)
		{
			if ((*it).eBuildingClass == (BuildingClassTypes)iBuildingClass)
			{
				//obviously some modder already set this buildingclass to some value
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
				if ( pClassInfo->getBuildingCommerceModifier(iBuildingClass, iCommerce) != 0)
				{
					BuildingCommerceChange kChange;
					kChange.eBuildingClass = (BuildingClassTypes)iBuildingClass;
					kChange.eCommerce = (CommerceTypes)iCommerce;
					kChange.iChange = pClassInfo->getBuildingCommerceModifier(iBuildingClass, iCommerce);
					m_aBuildingCommerceModifiers.push_back(kChange);
				}
			}
		}
	}

	for ( int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
	{
		if ( pClassInfo->getBuildingHappyChange(iBuildingClass) != 0)
		{
			//if the new value is set, we assume the modder did this on purpose
			//so we only add the old value, if the new isn't 0(as with all the new loading system)
			if ( getBuildingHappyChange(iBuildingClass) == 0 )
			{
				BuildingClassTypes eBuildingClass = (BuildingClassTypes)iBuildingClass;
				int iChange = pClassInfo->getBuildingHappyChange(iBuildingClass);
				m_aBuildingHappyChanges.push_back(std::make_pair(eBuildingClass, iChange));
			}
		}
	}

	for ( int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
	{
		if ( pClassInfo->getBuildingHealthChange(iBuildingClass) != 0)
		{
			//if the new value is set, we assume the modder did this on purpose
			//so we only add the old value, if the new isn't 0(as with all the new loading system)
			if ( getBuildingHealthChange(iBuildingClass) == 0 )
			{
				BuildingClassTypes eBuildingClass = (BuildingClassTypes)iBuildingClass;
				int iChange = pClassInfo->getBuildingHealthChange(iBuildingClass);
				m_aBuildingHealthChanges.push_back(std::make_pair(eBuildingClass, iChange));
			}
		}
	}

/************************************************************************************************/
/* Afforess					  Start		 01/20/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
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
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
}

/************************************************************************************************/
/* Afforess					  Start		 07/30/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvEventInfo::readPass3()
{
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
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/


//======================================================================================================
//					CvEspionageMissionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvEspionageMissionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvEspionageMissionInfo::CvEspionageMissionInfo() :
/************************************************************************************************/
/* Afforess					  Start		 01/31/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
m_bNuke(false),
m_bRevolt(false),
m_bDisablePower(false),
m_iWarWearinessCounter(0),
m_iPrereqGameOption(NO_GAMEOPTION),
m_iSabatogeResearchCostFactor(0),
m_iRemoveReligionsCostFactor(0),
m_iRemoveCorporationsCostFactor(0),
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
m_iCost(0),
m_bIsPassive(false),
m_bIsTwoPhases(false),
m_bTargetsCity(false),
m_bSelectPlot(false),
m_iTechPrereq(NO_TECH),
m_iVisibilityLevel(0),
m_bInvestigateCity(false),
m_bSeeDemographics(false),
m_bNoActiveMissions(false),
m_bSeeResearch(false),
m_bDestroyImprovement(false),
m_iDestroyBuildingCostFactor(0),
m_iDestroyUnitCostFactor(0),
m_iDestroyProjectCostFactor(0),
m_iDestroyProductionCostFactor(0),
m_iBuyUnitCostFactor(0),
m_iBuyCityCostFactor(0),
m_iStealTreasuryTypes(0),
m_iCityInsertCultureAmountFactor(0),
m_iCityInsertCultureCostFactor(0),
m_iCityPoisonWaterCounter(0),
m_iCityUnhappinessCounter(0),
m_iCityRevoltCounter(0),
m_iBuyTechCostFactor(0),
m_iSwitchCivicCostFactor(0),
m_iSwitchReligionCostFactor(0),
m_iPlayerAnarchyCounter(0),
m_iCounterespionageNumTurns(0),
m_iCounterespionageMod(0),
m_iDifficultyMod(0)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvEspionageMissionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvEspionageMissionInfo::~CvEspionageMissionInfo()
{
}

int CvEspionageMissionInfo::getCost() const
{
	return m_iCost;
}

bool CvEspionageMissionInfo::isPassive() const
{
	return m_bIsPassive;
}

bool CvEspionageMissionInfo::isTwoPhases() const
{
	return m_bIsTwoPhases;
}

bool CvEspionageMissionInfo::isTargetsCity() const
{
	return m_bTargetsCity;
}

bool CvEspionageMissionInfo::isSelectPlot() const
{
	return m_bSelectPlot;
}

int CvEspionageMissionInfo::getTechPrereq() const
{
	return m_iTechPrereq;
}

int CvEspionageMissionInfo::getVisibilityLevel() const
{
	return m_iVisibilityLevel;
}

bool CvEspionageMissionInfo::isInvestigateCity() const
{
	return m_bInvestigateCity;
}

bool CvEspionageMissionInfo::isSeeDemographics() const
{
	return m_bSeeDemographics;
}

bool CvEspionageMissionInfo::isNoActiveMissions() const
{
	return m_bNoActiveMissions;
}

bool CvEspionageMissionInfo::isSeeResearch() const
{
	return m_bSeeResearch;
}

bool CvEspionageMissionInfo::isDestroyImprovement() const
{
	return m_bDestroyImprovement;
}

int CvEspionageMissionInfo::getDestroyBuildingCostFactor() const
{
	return m_iDestroyBuildingCostFactor;
}

int CvEspionageMissionInfo::getDestroyUnitCostFactor() const
{
	return m_iDestroyUnitCostFactor;
}

int CvEspionageMissionInfo::getDestroyProjectCostFactor() const
{
	return m_iDestroyProjectCostFactor;
}

int CvEspionageMissionInfo::getDestroyProductionCostFactor() const
{
	return m_iDestroyProductionCostFactor;
}

int CvEspionageMissionInfo::getBuyUnitCostFactor() const
{
	return m_iBuyUnitCostFactor;
}

int CvEspionageMissionInfo::getBuyCityCostFactor() const
{
	return m_iBuyCityCostFactor;
}

int CvEspionageMissionInfo::getStealTreasuryTypes() const
{
	return m_iStealTreasuryTypes;
}

int CvEspionageMissionInfo::getCityInsertCultureAmountFactor() const
{
	return m_iCityInsertCultureAmountFactor;
}

int CvEspionageMissionInfo::getCityInsertCultureCostFactor() const
{
	return m_iCityInsertCultureCostFactor;
}

int CvEspionageMissionInfo::getCityPoisonWaterCounter() const
{
	return m_iCityPoisonWaterCounter;
}

int CvEspionageMissionInfo::getCityUnhappinessCounter() const
{
	return m_iCityUnhappinessCounter;
}

int CvEspionageMissionInfo::getCityRevoltCounter() const
{
	return m_iCityRevoltCounter;
}

int CvEspionageMissionInfo::getBuyTechCostFactor() const
{
	return m_iBuyTechCostFactor;
}

int CvEspionageMissionInfo::getSwitchCivicCostFactor() const
{
	return m_iSwitchCivicCostFactor;
}

int CvEspionageMissionInfo::getSwitchReligionCostFactor() const
{
	return m_iSwitchReligionCostFactor;
}

int CvEspionageMissionInfo::getPlayerAnarchyCounter() const
{
	return m_iPlayerAnarchyCounter;
}

int CvEspionageMissionInfo::getCounterespionageNumTurns() const
{
	return m_iCounterespionageNumTurns;
}

int CvEspionageMissionInfo::getCounterespionageMod() const
{
	return m_iCounterespionageMod;
}

int CvEspionageMissionInfo::getDifficultyMod() const
{
	return m_iDifficultyMod;
}

/************************************************************************************************/
/* Afforess					  Start		 01/31/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvEspionageMissionInfo::isNuke() const
{
	return m_bNuke;
}

bool CvEspionageMissionInfo::isRevolt() const
{
	return m_bRevolt;
}

bool CvEspionageMissionInfo::isDisablePower() const
{
	return m_bDisablePower;
}

int CvEspionageMissionInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}

int CvEspionageMissionInfo::getWarWearinessCounter() const
{
	return m_iWarWearinessCounter;
}

int CvEspionageMissionInfo::getSabatogeResearchCostFactor() const
{
	return m_iSabatogeResearchCostFactor;
}

int CvEspionageMissionInfo::getRemoveReligionsCostFactor() const
{
	return m_iRemoveReligionsCostFactor;
}

int CvEspionageMissionInfo::getRemoveCorporationsCostFactor() const
{
	return m_iRemoveCorporationsCostFactor;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

bool CvEspionageMissionInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_bIsPassive, L"bIsPassive");
	pXML->GetOptionalChildXmlValByName(&m_bIsTwoPhases, L"bIsTwoPhases");
	pXML->GetOptionalChildXmlValByName(&m_bTargetsCity, L"bTargetsCity");
	pXML->GetOptionalChildXmlValByName(&m_bSelectPlot, L"bSelectPlot");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereq");
	m_iTechPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iVisibilityLevel, L"iVisibilityLevel");
	pXML->GetOptionalChildXmlValByName(&m_bInvestigateCity, L"bInvestigateCity");
	pXML->GetOptionalChildXmlValByName(&m_bSeeDemographics, L"bSeeDemographics");
	pXML->GetOptionalChildXmlValByName(&m_bNoActiveMissions, L"bNoActiveMissions");
	pXML->GetOptionalChildXmlValByName(&m_bSeeResearch, L"bSeeResearch");

	pXML->GetOptionalChildXmlValByName(&m_bDestroyImprovement, L"bDestroyImprovement");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyBuildingCostFactor, L"iDestroyBuildingCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyUnitCostFactor, L"iDestroyUnitCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyProjectCostFactor, L"iDestroyProjectCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyProductionCostFactor, L"iDestroyProductionCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iBuyUnitCostFactor, L"iBuyUnitCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iBuyCityCostFactor, L"iBuyCityCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iStealTreasuryTypes, L"iStealTreasuryTypes");
	pXML->GetOptionalChildXmlValByName(&m_iCityInsertCultureAmountFactor, L"iCityInsertCultureAmountFactor");
	pXML->GetOptionalChildXmlValByName(&m_iCityInsertCultureCostFactor, L"iCityInsertCultureCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iCityPoisonWaterCounter, L"iCityPoisonWaterCounter");
	pXML->GetOptionalChildXmlValByName(&m_iCityUnhappinessCounter, L"iCityUnhappinessCounter");
	pXML->GetOptionalChildXmlValByName(&m_iCityRevoltCounter, L"iCityRevoltCounter");
	pXML->GetOptionalChildXmlValByName(&m_iBuyTechCostFactor, L"iBuyTechCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iSwitchCivicCostFactor, L"iSwitchCivicCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iSwitchReligionCostFactor, L"iSwitchReligionCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iPlayerAnarchyCounter, L"iPlayerAnarchyCounter");
	pXML->GetOptionalChildXmlValByName(&m_iCounterespionageNumTurns, L"iCounterespionageNumTurns");
	pXML->GetOptionalChildXmlValByName(&m_iCounterespionageMod, L"iCounterespionageMod");
	pXML->GetOptionalChildXmlValByName(&m_iDifficultyMod, L"iDifficultyMod");
/************************************************************************************************/
/* Afforess					  Start		 01/31/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bNuke, L"bNuke");
	pXML->GetOptionalChildXmlValByName(&m_bRevolt, L"bRevolt");
	pXML->GetOptionalChildXmlValByName(&m_bDisablePower, L"bDisablePower");
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessCounter, L"iWarWearinessCounter");
	pXML->GetOptionalChildXmlValByName(&m_iSabatogeResearchCostFactor, L"iSabatogeResearchCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iRemoveReligionsCostFactor, L"iRemoveReligionsCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iRemoveCorporationsCostFactor, L"iRemoveCorporationsCostFactor");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	return true;
}

void CvEspionageMissionInfo::copyNonDefaults(CvEspionageMissionInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getCost() == iDefault) m_iCost = pClassInfo->getCost();
	if (isPassive() == bDefault) m_bIsPassive = pClassInfo->isPassive();
	if (isTwoPhases() == bDefault) m_bIsTwoPhases = pClassInfo->isTwoPhases();
	if (isTargetsCity() == bDefault) m_bTargetsCity = pClassInfo->isTargetsCity();
	if (isSelectPlot() == bDefault) m_bSelectPlot = pClassInfo->isSelectPlot();

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();

	if (getVisibilityLevel() == iDefault) m_iVisibilityLevel = pClassInfo->getVisibilityLevel();
	if (isInvestigateCity() == bDefault) m_bInvestigateCity = pClassInfo->isInvestigateCity();
	if (isSeeDemographics() == bDefault) m_bSeeDemographics = pClassInfo->isSeeDemographics();
	if (isNoActiveMissions() == bDefault) m_bNoActiveMissions = pClassInfo->isNoActiveMissions();
	if (isSeeResearch() == bDefault) m_bSeeResearch = pClassInfo->isSeeResearch();

	if (isDestroyImprovement() == bDefault) m_bDestroyImprovement = pClassInfo->isDestroyImprovement();
	if (getDestroyBuildingCostFactor() == iDefault) m_iDestroyBuildingCostFactor = pClassInfo->getDestroyBuildingCostFactor();
	if (getDestroyUnitCostFactor() == iDefault) m_iDestroyUnitCostFactor = pClassInfo->getDestroyUnitCostFactor();
	if (getDestroyProjectCostFactor() == iDefault) m_iDestroyProjectCostFactor = pClassInfo->getDestroyProjectCostFactor();
	if (getDestroyProductionCostFactor() == iDefault) m_iDestroyProductionCostFactor = pClassInfo->getDestroyProductionCostFactor();
	if (getBuyUnitCostFactor() == iDefault) m_iBuyUnitCostFactor = pClassInfo->getBuyUnitCostFactor();
	if (getBuyCityCostFactor() == iDefault) m_iBuyCityCostFactor = pClassInfo->getBuyCityCostFactor();
	if (getStealTreasuryTypes() == iDefault) m_iStealTreasuryTypes = pClassInfo->getStealTreasuryTypes();
	if (getCityInsertCultureAmountFactor() == iDefault) m_iCityInsertCultureAmountFactor = pClassInfo->getCityInsertCultureAmountFactor();
	if (getCityInsertCultureCostFactor() == iDefault) m_iCityInsertCultureCostFactor = pClassInfo->getCityInsertCultureCostFactor();
	if (getCityPoisonWaterCounter() == iDefault) m_iCityPoisonWaterCounter = pClassInfo->getCityPoisonWaterCounter();
	if (getCityUnhappinessCounter() == iDefault) m_iCityUnhappinessCounter = pClassInfo->getCityUnhappinessCounter();
	if (getCityRevoltCounter() == iDefault) m_iCityRevoltCounter = pClassInfo->getCityRevoltCounter();
	if (getBuyTechCostFactor() == iDefault) m_iBuyTechCostFactor = pClassInfo->getBuyTechCostFactor();
	if (getSwitchCivicCostFactor() == iDefault) m_iSwitchCivicCostFactor = pClassInfo->getSwitchCivicCostFactor();
	if (getSwitchReligionCostFactor() == iDefault) m_iSwitchReligionCostFactor = pClassInfo->getSwitchReligionCostFactor();
	if (getPlayerAnarchyCounter() == iDefault) m_iPlayerAnarchyCounter = pClassInfo->getPlayerAnarchyCounter();
	if (getCounterespionageNumTurns() == iDefault) m_iCounterespionageNumTurns = pClassInfo->getCounterespionageNumTurns();
	if (getCounterespionageMod() == iDefault) m_iCounterespionageMod = pClassInfo->getCounterespionageMod();
	if (getDifficultyMod() == iDefault) m_iDifficultyMod = pClassInfo->getDifficultyMod();
/************************************************************************************************/
/* Afforess					  Start		 01/31/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (isNuke() == bDefault) m_bNuke = pClassInfo->isNuke();
	if (isRevolt() == bDefault) m_bRevolt = pClassInfo->isRevolt();
	if (isDisablePower() == bDefault) m_bDisablePower = pClassInfo->isDisablePower();
	if (getPrereqGameOption() == iTextDefault) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();
	if (getWarWearinessCounter() == iDefault) m_iWarWearinessCounter = pClassInfo->getWarWearinessCounter();
	if (getSabatogeResearchCostFactor() == iDefault) m_iSabatogeResearchCostFactor = pClassInfo->getSabatogeResearchCostFactor();
	if (getRemoveReligionsCostFactor() == iDefault) m_iRemoveReligionsCostFactor = pClassInfo->getRemoveReligionsCostFactor();
	if (getRemoveCorporationsCostFactor() == iDefault) m_iRemoveCorporationsCostFactor = pClassInfo->getRemoveCorporationsCostFactor();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
}

void CvEspionageMissionInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iCost);
	CheckSum(iSum, m_bIsPassive);
	CheckSum(iSum, m_bIsTwoPhases);
	CheckSum(iSum, m_bTargetsCity);
	CheckSum(iSum, m_bSelectPlot);

	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iVisibilityLevel);
	CheckSum(iSum, m_bInvestigateCity);
	CheckSum(iSum, m_bSeeDemographics);
	CheckSum(iSum, m_bNoActiveMissions);
	CheckSum(iSum, m_bSeeResearch);

	CheckSum(iSum, m_bDestroyImprovement);
	CheckSum(iSum, m_iDestroyBuildingCostFactor);
	CheckSum(iSum, m_iDestroyUnitCostFactor);
	CheckSum(iSum, m_iDestroyProjectCostFactor);
	CheckSum(iSum, m_iDestroyProductionCostFactor);
	CheckSum(iSum, m_iBuyUnitCostFactor);
	CheckSum(iSum, m_iBuyCityCostFactor);
	CheckSum(iSum, m_iStealTreasuryTypes);
	CheckSum(iSum, m_iCityInsertCultureAmountFactor);
	CheckSum(iSum, m_iCityInsertCultureCostFactor);
	CheckSum(iSum, m_iCityPoisonWaterCounter);
	CheckSum(iSum, m_iCityUnhappinessCounter);
	CheckSum(iSum, m_iCityRevoltCounter);
	CheckSum(iSum, m_iBuyTechCostFactor);
	CheckSum(iSum, m_iSwitchCivicCostFactor);
	CheckSum(iSum, m_iSwitchReligionCostFactor);
	CheckSum(iSum, m_iPlayerAnarchyCounter);
	CheckSum(iSum, m_iCounterespionageNumTurns);
	CheckSum(iSum, m_iCounterespionageMod);
	CheckSum(iSum, m_iDifficultyMod);
}

CvVoteSourceInfo::CvVoteSourceInfo() :
	m_iVoteInterval(0),
	m_iFreeSpecialist(NO_SPECIALIST),
	m_iCivic(NO_CIVIC),
	m_aiReligionYields(NULL),
	m_aiReligionCommerces(NULL)
{

}

CvVoteSourceInfo::~CvVoteSourceInfo()
{
	SAFE_DELETE_ARRAY(m_aiReligionYields);
	SAFE_DELETE_ARRAY(m_aiReligionCommerces);
}

int CvVoteSourceInfo::getVoteInterval() const
{
	return m_iVoteInterval;
}

int CvVoteSourceInfo::getFreeSpecialist() const
{
	return m_iFreeSpecialist;
}

int CvVoteSourceInfo::getCivic() const
{
	return m_iCivic;
}

int CvVoteSourceInfo::getReligionYield(int i) const
{
	FAssert(i >= 0 && i < NUM_YIELD_TYPES);
	return m_aiReligionYields[i];
}

int CvVoteSourceInfo::getReligionCommerce(int i) const
{
	FAssert(i >= 0 && i < NUM_COMMERCE_TYPES);
	return m_aiReligionCommerces[i];
}

const CvWString CvVoteSourceInfo::getPopupText() const
{
	return gDLL->getText(m_szPopupText);
}

const CvWString CvVoteSourceInfo::getSecretaryGeneralText() const
{
	return gDLL->getText(m_szSecretaryGeneralText);
}

const CvString& CvVoteSourceInfo::getCopyPopupText() const
{
	return m_szPopupText;
}

const CvString& CvVoteSourceInfo::getCopySecretaryGeneralText() const
{
	return m_szSecretaryGeneralText;
}

bool CvVoteSourceInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iVoteInterval, L"iVoteInterval");
	pXML->GetOptionalChildXmlValByName(m_szPopupText, L"PopupText");
	pXML->GetOptionalChildXmlValByName(m_szSecretaryGeneralText, L"SecretaryGeneralText");

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeSpecialist");
	m_iFreeSpecialist = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Civic");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"ReligionYields"))
	{
		pXML->SetYields(&m_aiReligionYields);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_aiReligionYields);
	}

	if (pXML->TryMoveToXmlFirstChild(L"ReligionCommerces"))
	{
		pXML->SetCommerce(&m_aiReligionCommerces);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_aiReligionCommerces);
	}

	return true;
}

void CvVoteSourceInfo::copyNonDefaults(CvVoteSourceInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getVoteInterval() == iDefault) m_iVoteInterval = pClassInfo->getVoteInterval();
	if (getPopupText() == wDefault) m_szPopupText = pClassInfo->getCopyPopupText();
	if (getSecretaryGeneralText() == wDefault) m_szSecretaryGeneralText = pClassInfo->getCopySecretaryGeneralText();

	if (getFreeSpecialist() == iTextDefault) m_iFreeSpecialist = pClassInfo->getFreeSpecialist();

	if ( getCivic() == iTextDefault ) m_iCivic = pClassInfo->getCivic();

	for ( int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if ( getReligionYield(i) == iDefault )
		{
			m_aiReligionYields[i] = pClassInfo->getReligionYield(i);
		}
	}

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++)
	{
		if ( getReligionCommerce(i) == iDefault )
		{
			m_aiReligionCommerces[i] = pClassInfo->getReligionCommerce(i);
		}
	}
}

void CvVoteSourceInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iVoteInterval);
	CheckSum(iSum, m_iFreeSpecialist);
	CheckSum(iSum, m_iCivic);

	CheckSum(iSum, m_aiReligionYields, NUM_YIELD_TYPES);
	CheckSum(iSum, m_aiReligionCommerces, NUM_COMMERCE_TYPES);

	CheckSumC(iSum, m_szPopupText);
	CheckSumC(iSum, m_szSecretaryGeneralText);
}

bool CvVoteSourceInfo::readPass3()
{
	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}

/************************************************************************************************/
/* XMLCOPY								 10/12/07								MRGENIE	  */
/*																							  */
/* Assuming the modder purposly added an entry to this tag, we want to take the last enty set   */
/* by the modder and not the first as set by firaxis											*/
/************************************************************************************************/
/*
	m_iCivic = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);
*/
	int iTextDefault = -1;
	int iSize = m_aszExtraXMLforPass3.size();
	for ( int i = 0; i < iSize; i++ )
	{
		if ( GC.getInfoTypeForString(m_aszExtraXMLforPass3[iSize - ( i + 1)]) != iTextDefault)
		{
			m_iCivic = GC.getInfoTypeForString(m_aszExtraXMLforPass3[iSize - ( i + 1)]);
			break;
		}
	}
	m_aszExtraXMLforPass3.clear();

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPropertyInfo
//
//  DESC:   Contains info about generic properties which can be added to buildings
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvPropertyInfo::CvPropertyInfo() :
								m_iChar(0),
								m_iFontButtonIndex(0),
								m_bSourceDrain(false),
								m_bOAType(false),
								m_iAIWeight(0),
								m_iOperationalRangeMin(-500),
								m_iOperationalRangeMax(500),
								m_iTargetLevel(0),
								m_iTrainReluctance(1),
								m_eAIScaleType(AISCALE_NONE),
								m_PropertyManipulators()
{
	for (int i=0; i < NUM_GAMEOBJECTS; i++)
	{
		for (int j=0; j < NUM_GAMEOBJECTS; j++)
		{
			m_aaiChangePropagator[i][j] = 0;
		}
	}
}

CvPropertyInfo::~CvPropertyInfo()
{
	for (int i=0; i<(int)m_aPropertyBuildings.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aPropertyBuildings[i].eBuilding));
	}

	for (int i=0; i<(int)m_aPropertyPromotions.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aPropertyPromotions[i].ePromotion));
	}
}

bool CvPropertyInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}


	pXML->GetOptionalChildXmlValByName(m_szValueDisplayText, L"ValueDisplayText");
	pXML->GetOptionalChildXmlValByName(m_szChangeDisplayText, L"ChangeDisplayText");
	pXML->GetOptionalChildXmlValByName(m_szChangeAllCitiesDisplayText, L"ChangeAllCitiesDisplayText");
	pXML->GetOptionalChildXmlValByName(m_szPrereqMinDisplayText, L"PrereqMinDisplayText");
	pXML->GetOptionalChildXmlValByName(m_szPrereqMaxDisplayText, L"PrereqMaxDisplayText");
	pXML->GetOptionalChildXmlValByName(&m_iAIWeight, L"iAIWeight");
	pXML->GetOptionalChildXmlValByName(&m_bSourceDrain, L"bSourceDrain");
	pXML->GetOptionalChildXmlValByName(&m_bOAType, L"bOAType");
	pXML->GetOptionalChildXmlValByName(&m_iOperationalRangeMin, L"iOperationalRangeMin", -500);
	pXML->GetOptionalChildXmlValByName(&m_iOperationalRangeMax, L"iOperationalRangeMax", 500);
	pXML->GetOptionalChildXmlValByName(&m_iTargetLevel, L"iTargetLevel", 0);
	pXML->GetOptionalChildXmlValByName(&m_iTrainReluctance, L"iTrainReluctance", 1);
	pXML->GetOptionalChildXmlValByName(&m_iFontButtonIndex, L"FontButtonIndex");
	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"AIScaleType");
	m_eAIScaleType = (AIScaleTypes) pXML->GetInfoClass(szTextVal);

	if(pXML->TryMoveToXmlFirstChild(L"ChangePropagators"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"ChangePropagator"))
			{
				do
				{
					int iChangePercent;
					pXML->GetChildXmlValByName(szTextVal, L"GameObjectTypeFrom");
					int eFrom = pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"GameObjectTypeTo");
					int eTo = pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&iChangePercent, L"iChangePercent");
					m_aaiChangePropagator[eFrom][eTo] = iChangePercent;
				}
				while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PropertyBuildings"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"PropertyBuilding" );
		m_aPropertyBuildings.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"PropertyBuilding"))
			{
				do
				{
					pXML->GetChildXmlValByName(&(m_aPropertyBuildings[i].iMinValue), L"iMinValue");
					pXML->GetChildXmlValByName(&(m_aPropertyBuildings[i].iMaxValue), L"iMaxValue");
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					GC.addDelayedResolution((int*)&(m_aPropertyBuildings[i].eBuilding), szTextVal);
					i++;
				}
				while(pXML->TryMoveToXmlNextSibling(L"PropertyBuilding"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PropertyPromotions"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"PropertyPromotion" );
		m_aPropertyPromotions.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PropertyPromotion"))
			{
				do
				{
					pXML->GetChildXmlValByName(&(m_aPropertyPromotions[i].iMinValue), L"iMinValue");
					pXML->GetChildXmlValByName(&(m_aPropertyPromotions[i].iMaxValue), L"iMaxValue");
					pXML->GetChildXmlValByName(szTextVal, L"PromotionType");
					GC.addDelayedResolution((int*)&(m_aPropertyPromotions[i].ePromotion), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PropertyPromotion"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	m_aTargetLevelbyEraTypes.clear();
	if (pXML->TryMoveToXmlFirstChild(L"TargetLevelbyEraTypes"))
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
						EraTypes eEra = (EraTypes)pXML->GetInfoClass(szTextVal);
						int iLevel;
						pXML->GetNextXmlVal(&iLevel);
						m_aTargetLevelbyEraTypes.push_back(std::make_pair(eEra, iLevel));

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

	m_PropertyManipulators.read(pXML);

	return true;
}

void CvPropertyInfo::copyNonDefaults(CvPropertyInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getAIWeight() == iDefault) m_iAIWeight = pClassInfo->getAIWeight();
	if (isSourceDrain() == false) m_bSourceDrain = pClassInfo->isSourceDrain();
	if (isOAType() == false) m_bOAType = pClassInfo->isOAType();
	if (getOperationalRangeMin() == -500) m_iOperationalRangeMin = pClassInfo->getOperationalRangeMin();
	if (getOperationalRangeMax() == 500) m_iOperationalRangeMax = pClassInfo->getOperationalRangeMax();
	if (getTargetLevel() == 0) m_iTargetLevel = pClassInfo->getTargetLevel();
	if (getTrainReluctance() == 1) m_iTrainReluctance = pClassInfo->getTrainReluctance();
	if (getAIScaleType() == AISCALE_NONE) m_eAIScaleType = pClassInfo->getAIScaleType();
	if (getValueDisplayText() == NULL || getValueDisplayText() == wDefault) m_szValueDisplayText = pClassInfo->getValueDisplayText();
	if (getChangeDisplayText() == NULL || getChangeDisplayText() == wDefault) m_szChangeDisplayText = pClassInfo->getChangeDisplayText();
	if (getChangeAllCitiesDisplayText() == NULL || getChangeAllCitiesDisplayText() == wDefault) m_szChangeAllCitiesDisplayText = pClassInfo->getChangeAllCitiesDisplayText();
	if (getPrereqMinDisplayText() == NULL || getPrereqMinDisplayText() == wDefault) m_szPrereqMinDisplayText = pClassInfo->getPrereqMinDisplayText();
	if (getPrereqMaxDisplayText() == NULL || getPrereqMaxDisplayText() == wDefault) m_szPrereqMaxDisplayText = pClassInfo->getPrereqMaxDisplayText();
	if (getFontButtonIndex() == 0) m_iFontButtonIndex = pClassInfo->getFontButtonIndex();

	for (int i=0; i < NUM_GAMEOBJECTS; i++)
	{
		for (int j=0; j < NUM_GAMEOBJECTS; j++)
		{
			if (m_aaiChangePropagator[i][j] == 0)
			{
				m_aaiChangePropagator[i][j] = pClassInfo->getChangePropagator((GameObjectTypes)i,(GameObjectTypes)j);
			}
		}
	}

	if (getNumPropertyBuildings() == 0)
	{
		int iNum = pClassInfo->getNumPropertyBuildings();
		m_aPropertyBuildings.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPropertyBuildings[i] = pClassInfo->getPropertyBuilding(i);
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPropertyBuildings[i].eBuilding), (int*)&(pClassInfo->getPropertyBuilding(i).eBuilding));
		}
	}

	if (getNumPropertyPromotions() == 0)
	{
		int iNum = pClassInfo->getNumPropertyPromotions();
		m_aPropertyPromotions.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPropertyPromotions[i] = pClassInfo->getPropertyPromotion(i);
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPropertyPromotions[i].ePromotion), (int*)&(pClassInfo->getPropertyPromotion(i).ePromotion));
		}
	}

	if (getNumTargetLevelbyEraTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumTargetLevelbyEraTypes(); i++)
		{
			EraTypes eEra = ((EraTypes)i);
			int iChange = pClassInfo->getTargetLevelbyEraType(i);
			m_aTargetLevelbyEraTypes.push_back(std::make_pair(eEra, iChange));
		}
	}

	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);
}

void CvPropertyInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_bSourceDrain);
	CheckSum(iSum, m_bOAType);
	CheckSum(iSum, m_iAIWeight);
	CheckSum(iSum, m_eAIScaleType);
	CheckSum(iSum, m_iOperationalRangeMin);
	CheckSum(iSum, m_iOperationalRangeMax);
	CheckSum(iSum, m_iTargetLevel);
	CheckSum(iSum, m_iTrainReluctance);
	CheckSumC(iSum, m_aTargetLevelbyEraTypes);

	for (int i=0; i < NUM_GAMEOBJECTS; i++)
	{
		for (int j=0; j < NUM_GAMEOBJECTS; j++)
		{
			CheckSum(iSum, m_aaiChangePropagator[i][j]);
		}
	}

	for (int i=0; i<(int)m_aPropertyBuildings.size(); i++)
	{
		CheckSum(iSum, m_aPropertyBuildings[i].iMinValue);
		CheckSum(iSum, m_aPropertyBuildings[i].iMaxValue);
		CheckSum(iSum, (int)m_aPropertyBuildings[i].eBuilding);
	}

	for (int i=0; i<(int)m_aPropertyPromotions.size(); i++)
	{
		CheckSum(iSum, m_aPropertyPromotions[i].iMinValue);
		CheckSum(iSum, m_aPropertyPromotions[i].iMaxValue);
		CheckSum(iSum, (int)m_aPropertyPromotions[i].ePromotion);
	}

	m_PropertyManipulators.getCheckSum(iSum);
}

int CvPropertyInfo::getChar() const
{
	return m_iChar;
}

void CvPropertyInfo::setChar(int i)
{
	m_iChar = i;
}

int CvPropertyInfo::getFontButtonIndex() const
{
	return m_iFontButtonIndex;
}

bool CvPropertyInfo::isSourceDrain() const
{
	return m_bSourceDrain;
}

bool CvPropertyInfo::isOAType() const
{
	return m_bOAType;
}

int	CvPropertyInfo::getAIWeight() const
{
	return m_iAIWeight;
}

int	CvPropertyInfo::getOperationalRangeMin() const
{
	return m_iOperationalRangeMin;
}

int	CvPropertyInfo::getOperationalRangeMax() const
{
	return m_iOperationalRangeMax;
}

int	CvPropertyInfo::getTargetLevel() const
{
	return m_iTargetLevel;
}

int	CvPropertyInfo::getTrainReluctance() const
{
	return m_iTrainReluctance;
}

AIScaleTypes CvPropertyInfo::getAIScaleType() const
{
	return m_eAIScaleType;
}

CvWString CvPropertyInfo::getValueDisplayText() const
{
	return m_szValueDisplayText;
}

CvWString CvPropertyInfo::getChangeDisplayText() const
{
	return m_szChangeDisplayText;
}

CvWString CvPropertyInfo::getChangeAllCitiesDisplayText() const
{
	return m_szChangeAllCitiesDisplayText;
}

CvWString CvPropertyInfo::getPrereqMinDisplayText() const
{
	return m_szPrereqMinDisplayText;
}

CvWString CvPropertyInfo::getPrereqMaxDisplayText() const
{
	return m_szPrereqMaxDisplayText;
}

int CvPropertyInfo::getChangePropagator(const GameObjectTypes eFrom, const GameObjectTypes eTo) const
{
	FAssertMsg(eFrom < NUM_GAMEOBJECTS, "Index out of bounds");
	FAssertMsg(eFrom > -1, "Index out of bounds");
	FAssertMsg(eTo < NUM_GAMEOBJECTS, "Index out of bounds");
	FAssertMsg(eTo > -1, "Index out of bounds");
	return m_aaiChangePropagator[eFrom][eTo];
}

CvPropertyManipulators* CvPropertyInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}

PropertyBuilding& CvPropertyInfo::getPropertyBuilding(int index)
{
	FAssertMsg(index < (int)m_aPropertyBuildings.size(), "Index out of bounds");
	FAssertMsg(index > -1, "Index out of bounds");
	return m_aPropertyBuildings[index];
}

int CvPropertyInfo::getNumPropertyBuildings() const
{
	return (int)m_aPropertyBuildings.size();
}

PropertyPromotion& CvPropertyInfo::getPropertyPromotion(int index)
{
	FAssertMsg(index < (int)m_aPropertyPromotions.size(), "Index out of bounds");
	FAssertMsg(index > -1, "Index out of bounds");
	return m_aPropertyPromotions[index];
}

int CvPropertyInfo::getNumPropertyPromotions() const
{
	return (int)m_aPropertyPromotions.size();
}

int CvPropertyInfo::getNumTargetLevelbyEraTypes() const
{
	return m_aTargetLevelbyEraTypes.size();
}

int CvPropertyInfo::getTargetLevelbyEraType(int iIndex) const
{
	for (EraArray::const_iterator it = m_aTargetLevelbyEraTypes.begin(); it != m_aTargetLevelbyEraTypes.end(); ++it)
	{
		if ((*it).first == (EraTypes)iIndex)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPropertyInfo::isTargetLevelbyEraType(int iIndex)
{
	for (EraArray::const_iterator it = m_aTargetLevelbyEraTypes.begin(); it != m_aTargetLevelbyEraTypes.end(); ++it)
	{
		if ((*it).first == (EraTypes)iIndex)
		{
			return true;
		}
	}
	return false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaClassInfo
//
//  DESC:   Contains info about Idea Classes
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvIdeaClassInfo::CvIdeaClassInfo() :
m_bInitialized(true)
{
}

CvIdeaClassInfo::~CvIdeaClassInfo()
{
}

bool CvIdeaClassInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	//Uncomment when needed
	//CvString szTextVal;

	return true;
}

bool CvIdeaClassInfo::readPass2(CvXMLLoadUtility* pXML)
{
	//Uncomment when needed
	//CvString szTextVal;


	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	return true;
}

void CvIdeaClassInfo::copyNonDefaults(CvIdeaClassInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	//Uncomment all of the following as needed
	//bool bDefault = false;
	//int iDefault = 0;
	//int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	//int iAudioDefault = -1;  //all audio is default -1
	//float fDefault = 0.0f;
	//CvString cDefault = CvString::format("").GetCString();

	//CvInfoBase::copyNonDefaults(pClassInfo, pXML);

}

void CvIdeaClassInfo::copyNonDefaultsReadPass2(CvIdeaClassInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver)
{
}

void CvIdeaClassInfo::getCheckSum(unsigned int& iSum)
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaInfo
//
//  DESC:   Contains info about Ideas
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvIdeaInfo::CvIdeaInfo() :
m_eIdeaClass(NO_IDEACLASS)
{
}

CvIdeaInfo::~CvIdeaInfo()
{
}

IdeaClassTypes CvIdeaInfo::getIdeaClass() const
{
	return m_eIdeaClass;
}

bool CvIdeaInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;

	if ( pXML->GetOptionalChildXmlValByName(szTextVal, L"IdeaClassType") )
		m_eIdeaClass = (IdeaClassTypes)pXML->GetInfoClass(szTextVal);
	else
		m_eIdeaClass = NO_IDEACLASS;

	return true;
}

bool CvIdeaInfo::readPass2(CvXMLLoadUtility* pXML)
{
	//Uncomment when needed
	//CvString szTextVal;


	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	return true;
}

void CvIdeaInfo::copyNonDefaults(CvIdeaInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	//Uncomment all of the following as needed
	//bool bDefault = false;
	//int iDefault = 0;
	//int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	//int iAudioDefault = -1;  //all audio is default -1
	//float fDefault = 0.0f;
	//CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getIdeaClass() == NO_IDEACLASS) m_eIdeaClass = pClassInfo->getIdeaClass();

}

void CvIdeaInfo::copyNonDefaultsReadPass2(CvIdeaInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver)
{
}

void CvIdeaInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_eIdeaClass);
}

