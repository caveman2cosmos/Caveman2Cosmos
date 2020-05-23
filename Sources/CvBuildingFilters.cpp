//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvBuildingFilters.cpp
//
//  PURPOSE: Classes to filter buildings
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvPlayerAI.h"

void BuildingFilterBase::Activate()
{
	setActive(true);
}

void BuildingFilterBase::Deactivate()
{
	setActive(false);
}

bool BuildingFilterBase::isActive()
{
	return m_bActive;
}

bool BuildingFilterBase::setActive(bool bActive)
{
	bool bChanged = m_bActive ^ bActive;
	m_bActive = bActive;
	return bChanged;
}

bool BuildingFilterBase::isFiltered(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	return !m_bActive || (m_bInvert ^ isFilteredBuilding(pPlayer, pCity, eBuilding));
}

BuildingFilterBase::~BuildingFilterBase()
{

}

bool BuildingFilterCanBuild::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		return pCity->canConstruct(eBuilding, false, m_bShowSomeUnconstructable);
	}
	return pPlayer->canConstruct(eBuilding, false, m_bShowSomeUnconstructable);
}

bool BuildingFilterIsWonder::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	return isLimitedWonder(eBuilding);
}

bool BuildingFilterIsGreatWonder::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	return isWorldWonder(eBuilding);
}

bool BuildingFilterIsNationalWonder::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	return isNationalWonder(eBuilding);
}

BuildingFilterIsCommerce::BuildingFilterIsCommerce(CommerceTypes eCommerce, bool bInvert) : BuildingFilterBase(bInvert)
{
	m_eCommerce = eCommerce;
}

bool BuildingFilterIsCommerce::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		return pCity->getAdditionalCommerceTimes100ByBuilding(m_eCommerce, eBuilding) > 0;
	}
	return ((GC.getBuildingInfo(eBuilding).getCommerceChange(m_eCommerce) > 0) || 
		(GC.getBuildingInfo(eBuilding).getCommercePerPopChange(m_eCommerce) > 0) || 
		(GC.getBuildingInfo(eBuilding).getCommerceModifier(m_eCommerce) > 0)|| 
		(GC.getBuildingInfo(eBuilding).getSpecialistExtraCommerce(m_eCommerce) > 0) || 
		(GC.getBuildingInfo(eBuilding).getGlobalCommerceModifier(m_eCommerce) > 0));
}

BuildingFilterIsYield::BuildingFilterIsYield(YieldTypes eYield, bool bInvert) : BuildingFilterBase(bInvert)
{
	m_eYield = eYield;
}

bool BuildingFilterIsYield::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		return pCity->getAdditionalYieldByBuilding(m_eYield, eBuilding, true) > 0;
	}
	return ((GC.getBuildingInfo(eBuilding).getYieldChange(m_eYield) > 0) || 
		(GC.getBuildingInfo(eBuilding).getYieldPerPopChange(m_eYield) > 0) || 
		(GC.getBuildingInfo(eBuilding).getYieldModifier(m_eYield) > 0)	|| 
		(GC.getBuildingInfo(eBuilding).getAreaYieldModifier(m_eYield) > 0) || 
		(GC.getBuildingInfo(eBuilding).getGlobalYieldModifier(m_eYield) > 0) || 
		(GC.getBuildingInfo(eBuilding).getGlobalSeaPlotYieldChange(m_eYield) > 0) || 
		(GC.getBuildingInfo(eBuilding).getSeaPlotYieldChange(m_eYield) > 0));
}

bool BuildingFilterIsHappiness::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	//TB Note: isn't the following checking only buildings already built?
	if (pCity)
	{
		return pCity->getAdditionalHappinessByBuilding(eBuilding) > 0;
	}
	bool bTechHapp = false;
	if (GC.getBuildingInfo(eBuilding).getNumTechHappinessTypes() > 0)
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GC.getBuildingInfo(eBuilding).getTechHappinessType(iI) > 0)
			{
				bTechHapp = true;
			}
		}
	}
	return ((GC.getBuildingInfo(eBuilding).getHappiness() > 0) || 
		(GC.getBuildingInfo(eBuilding).getAreaHappiness() > 0) || 
		(GC.getBuildingInfo(eBuilding).getGlobalHappiness() > 0) ||
		bTechHapp);
}

bool BuildingFilterIsHealth::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		return pCity->getAdditionalHealthByBuilding(eBuilding) > 0;
	}
	bool bTechHealth = false;
	if (GC.getBuildingInfo(eBuilding).getNumTechHealthTypes() > 0)
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GC.getBuildingInfo(eBuilding).getTechHealthType(iI) > 0)
			{
				bTechHealth = true;
			}
		}
	}
	return ((GC.getBuildingInfo(eBuilding).getHealth() > 0) || 
		(GC.getBuildingInfo(eBuilding).getAreaHealth() > 0) || 
		(GC.getBuildingInfo(eBuilding).getGlobalHealth() > 0) ||
		bTechHealth);
}

bool BuildingFilterIsUnhappiness::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		return pCity->getAdditionalHappinessByBuilding(eBuilding) < 0;
	}
	bool bTechUnHapp = false;
	if (GC.getBuildingInfo(eBuilding).getNumTechHappinessTypes() > 0)
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GC.getBuildingInfo(eBuilding).getTechHappinessType(iI) < 0)
			{
				bTechUnHapp = true;
			}
		}
	}

	return ((GC.getBuildingInfo(eBuilding).getHappiness() < 0) ||
		(GC.getBuildingInfo(eBuilding).getAreaHappiness() < 0) ||
		(GC.getBuildingInfo(eBuilding).getGlobalHappiness() < 0) ||
		bTechUnHapp);
}

bool BuildingFilterIsUnhealthiness::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		return pCity->getAdditionalHealthByBuilding(eBuilding) < 0;
	}
	bool bTechUnHealth = false;
	if (GC.getBuildingInfo(eBuilding).getNumTechHealthTypes() > 0)
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GC.getBuildingInfo(eBuilding).getTechHealthType(iI) < 0)
			{
				bTechUnHealth = true;
			}
		}
	}
	return ((GC.getBuildingInfo(eBuilding).getHealth() < 0) || 
		(GC.getBuildingInfo(eBuilding).getAreaHealth() < 0) || 
		(GC.getBuildingInfo(eBuilding).getGlobalHealth() < 0) ||
		bTechUnHealth);
}

bool BuildingFilterIsMilitary::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) // not finished
{
	return ((GC.getBuildingInfo(eBuilding).getMilitaryProductionModifier() > 0) || 
		(GC.getBuildingInfo(eBuilding).getFreeExperience() > 0) ||
		(GC.getBuildingInfo(eBuilding).getFreePromotion() != NO_PROMOTION) || 
		(GC.getBuildingInfo(eBuilding).getFreePromotion_2() != NO_PROMOTION) ||
		(GC.getBuildingInfo(eBuilding).getFreePromotion_3() != NO_PROMOTION) || 
		(GC.getBuildingInfo(eBuilding).getNumUnitCombatRetrainTypes() > 0) ||
		(GC.getBuildingInfo(eBuilding).getNumUnitCombatProdModifiers() > 0) ||
		(GC.getBuildingInfo(eBuilding).getNumFreePromoTypes() > 0) ||
		(GC.getBuildingInfo(eBuilding).getNumUnitCombatOngoingTrainingDurations() > 0) ||
		(GC.getBuildingInfo(eBuilding).isAnyUnitCombatFreeExperience()) ||
		(GC.getBuildingInfo(eBuilding).isAnyDomainFreeExperience()));
}

bool BuildingFilterIsCityDefense::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	bool bSAD = false;
	bool bSIN = false;

	if (GC.getGame().isOption(GAMEOPTION_SAD))
	{
		bSAD = (GC.getBuildingInfo(eBuilding).getLocalDynamicDefense() > 0);
	}

	if (GC.getGame().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		bSIN =  ((GC.getBuildingInfo(eBuilding).getFrontSupportPercentModifier() > 0) || 
		(GC.getBuildingInfo(eBuilding).getShortRangeSupportPercentModifier() > 0) || 
		(GC.getBuildingInfo(eBuilding).getMediumRangeSupportPercentModifier() > 0) || 
		(GC.getBuildingInfo(eBuilding).getLongRangeSupportPercentModifier() > 0) || 
		(GC.getBuildingInfo(eBuilding).getFlankSupportPercentModifier() > 0)) ;
	}

	return ((GC.getBuildingInfo(eBuilding).getDefenseModifier() > 0) || 
		(GC.getBuildingInfo(eBuilding).getAllCityDefenseModifier() > 0) ||
		(GC.getBuildingInfo(eBuilding).getAdjacentDamagePercent() > 0) || 
		(GC.getBuildingInfo(eBuilding).getBombardDefenseModifier() > 0) ||
		(GC.getBuildingInfo(eBuilding).getNumUnitCombatRepelModifiers() > 0) || 
		(GC.getBuildingInfo(eBuilding).getLocalCaptureProbabilityModifier() > 0) || 
		(GC.getBuildingInfo(eBuilding).getLocalCaptureResistanceModifier() > 0) || 
		(GC.getBuildingInfo(eBuilding).getNationalCaptureResistanceModifier() > 0) || 
		bSAD || 
		bSIN || 
		(GC.getBuildingInfo(eBuilding).getRiverDefensePenalty() < 0) || 
		(GC.getBuildingInfo(eBuilding).getLocalRepel() > 0) || 
		(GC.getBuildingInfo(eBuilding).getMinDefense() > 0) || 
		(GC.getBuildingInfo(eBuilding).getBuildingDefenseRecoverySpeedModifier() > 0) || 
		(GC.getBuildingInfo(eBuilding).getCityDefenseRecoverySpeedModifier() > 0) ||
		(GC.getBuildingInfo(eBuilding).getNumUnitCombatRepelAgainstModifiers() > 0) ||
		(GC.getBuildingInfo(eBuilding).getNumUnitCombatDefenseAgainstModifiers() > 0));
}

bool BuildingFilterIsProperty::isFilteredBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	CvBuildingInfo& kInfo = GC.getBuildingInfo(eBuilding);
	if ((kInfo.getProperties()->getValueByProperty(m_eProperty) != 0) || (kInfo.getPropertiesAllCities()->getValueByProperty(m_eProperty)))
		return true;

	CvPropertyManipulators* pMani = kInfo.getPropertyManipulators();
	int iNum = pMani->getNumSources();
	for (int i=0; i<iNum; i++)
	{
		if (pMani->getSource(i)->getProperty() == m_eProperty)
			return true;
	}

	iNum = pMani->getNumInteractions();
	for (int i=0; i<iNum; i++)
	{
		if (pMani->getInteraction(i)->getSourceProperty() == m_eProperty)
			return true;
		if (pMani->getInteraction(i)->getTargetProperty() == m_eProperty)
			return true;
	}

	iNum = pMani->getNumPropagators();
	for (int i=0; i<iNum; i++)
	{
		if (pMani->getPropagator(i)->getProperty() == m_eProperty)
			return true;
	}

	return false;
}

BuildingFilterList::BuildingFilterList(CvPlayer *pPlayer, CvCity *pCity)
{
	m_pPlayer = pPlayer;
	m_pCity = pCity;

	for (int i = 0; i < NUM_BUILDING_FILTERS; i++)
	{
		m_apBuildingFilters[i] = NULL;
	}
}

void BuildingFilterList::init()
{
	for (int i = 0; i < NUM_BUILDING_FILTERS; i++)
	{
		SAFE_DELETE(m_apBuildingFilters[i]);
	}

	m_apBuildingFilters[BUILDING_FILTER_HIDE_BASIC_INVISIBLE] = new BuildingFilterCanBuild(true);
	if (m_pCity)
		m_apBuildingFilters[BUILDING_FILTER_HIDE_BASIC_INVISIBLE]->Activate();
	m_apBuildingFilters[BUILDING_FILTER_HIDE_BUILDABLE] = new BuildingFilterCanBuild(false, true);
	m_apBuildingFilters[BUILDING_FILTER_HIDE_UNBUILDABLE] = new BuildingFilterCanBuild();
	m_apBuildingFilters[BUILDING_FILTER_HIDE_GREAT_WONDER] = new BuildingFilterIsGreatWonder(true);
	m_apBuildingFilters[BUILDING_FILTER_HIDE_NATIONAL_WONDER] = new BuildingFilterIsNationalWonder(true);
	m_apBuildingFilters[BUILDING_FILTER_HIDE_NORMAL] = new BuildingFilterIsWonder();
	m_apBuildingFilters[BUILDING_FILTER_SHOW_SCIENCE] = new BuildingFilterIsCommerce(COMMERCE_RESEARCH);
	m_apBuildingFilters[BUILDING_FILTER_SHOW_FOOD] = new BuildingFilterIsYield(YIELD_FOOD);
	m_apBuildingFilters[BUILDING_FILTER_SHOW_CULTURE] = new BuildingFilterIsCommerce(COMMERCE_CULTURE);
	m_apBuildingFilters[BUILDING_FILTER_SHOW_ESPIONAGE] = new BuildingFilterIsCommerce(COMMERCE_ESPIONAGE);
	m_apBuildingFilters[BUILDING_FILTER_SHOW_GOLD] = new BuildingFilterIsCommerce(COMMERCE_GOLD);
	m_apBuildingFilters[BUILDING_FILTER_SHOW_PRODUCTION] = new BuildingFilterIsYield(YIELD_PRODUCTION);
	m_apBuildingFilters[BUILDING_FILTER_SHOW_HAPPINESS] = new BuildingFilterIsHappiness();
	m_apBuildingFilters[BUILDING_FILTER_SHOW_HEALTH] = new BuildingFilterIsHealth();
	m_apBuildingFilters[BUILDING_FILTER_SHOW_MILITARY] = new BuildingFilterIsMilitary();
	m_apBuildingFilters[BUILDING_FILTER_SHOW_CITY_DEFENSE] = new BuildingFilterIsCityDefense();
	m_apBuildingFilters[BUILDING_FILTER_HIDE_UNHAPPINESS] = new BuildingFilterIsUnhappiness(true);
	m_apBuildingFilters[BUILDING_FILTER_HIDE_UNHEALTHINESS] = new BuildingFilterIsUnhealthiness(true);
	m_apBuildingFilters[BUILDING_FILTER_SHOW_CRIME] = new BuildingFilterIsProperty((PropertyTypes)GC.getInfoTypeForString("PROPERTY_CRIME"));
	m_apBuildingFilters[BUILDING_FILTER_SHOW_FLAMMABILITY] = new BuildingFilterIsProperty((PropertyTypes)GC.getInfoTypeForString("PROPERTY_FLAMMABILITY"));
	m_apBuildingFilters[BUILDING_FILTER_SHOW_EDUCATION] = new BuildingFilterIsProperty((PropertyTypes)GC.getInfoTypeForString("PROPERTY_EDUCATION"));
	m_apBuildingFilters[BUILDING_FILTER_SHOW_DISEASE] = new BuildingFilterIsProperty((PropertyTypes)GC.getInfoTypeForString("PROPERTY_DISEASE"));
	m_apBuildingFilters[BUILDING_FILTER_SHOW_AIR_POLLUTION] = new BuildingFilterIsProperty((PropertyTypes)GC.getInfoTypeForString("PROPERTY_AIR_POLLUTION"));
	m_apBuildingFilters[BUILDING_FILTER_SHOW_WATER_POLLUTION] = new BuildingFilterIsProperty((PropertyTypes)GC.getInfoTypeForString("PROPERTY_WATER_POLLUTION"));
	m_apBuildingFilters[BUILDING_FILTER_SHOW_TOURISM] = new BuildingFilterIsProperty((PropertyTypes)GC.getInfoTypeForString("PROPERTY_TOURISM"));
	m_apBuildingFilters[BUILDING_FILTER_HIDE_UNBUILDABLE]->setActive(getBugOptionBOOL("RoMSettings__HideUnconstructableBuildings", false));
}

BuildingFilterList::~BuildingFilterList()
{
	for (int i = 0; i < NUM_BUILDING_FILTERS; i++)
	{
		SAFE_DELETE(m_apBuildingFilters[i]);
	}
}

int BuildingFilterList::getNumFilters()
{
	return NUM_BUILDING_FILTERS;
}

bool BuildingFilterList::isFilterActive(BuildingFilterTypes i)
{
	FAssertMsg(i < NUM_BUILDING_FILTERS, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_apBuildingFilters[i]->isActive();
}

void BuildingFilterList::setCity(CvCity *pCity)
{
	m_pCity = pCity;
}

void BuildingFilterList::setPlayer(CvPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
}

bool BuildingFilterList::setFilterActive(BuildingFilterTypes i, bool bActive)
{
	FAssertMsg(i < NUM_BUILDING_FILTERS, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_apBuildingFilters[i]->setActive(bActive);
}

bool BuildingFilterList::isFiltered(BuildingTypes eBuilding)
{
	for (int i = 0; i < NUM_BUILDING_FILTERS; i++)
	{
		if (!m_apBuildingFilters[i]->isFiltered(m_pPlayer, m_pCity, eBuilding))
			return false;
	}
	return true;
}

void BuildingFilterList::setFilterActiveAll(BuildingFilterTypes eFilter, bool bActive)
{
	for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive())
		{
			kLoopPlayer.setBuildingListFilterActive(eFilter, bActive);
			int iIter;
			for (CvCity* pCity = kLoopPlayer.firstCity(&iIter); NULL != pCity; pCity = kLoopPlayer.nextCity(&iIter))
			{
				pCity->setBuildingListFilterActive(eFilter, bActive);
			}
		}
	}
}