//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvBuildingFilters.cpp
//
//  PURPOSE: Classes to filter buildings
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvBuildingFilters.h"
#include "CvBuildingInfo.h"
#include "CvBugOptions.h"
#include "CvCity.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvPlayerAI.h"

void BuildingFilterBase::Activate()
{
	setActive(true);
}

void BuildingFilterBase::Deactivate()
{
	setActive(false);
}

bool BuildingFilterBase::isActive() const
{
	return m_bActive;
}

bool BuildingFilterBase::setActive(bool bActive)
{
	const bool bChanged = m_bActive ^ bActive;
	m_bActive = bActive;
	return bChanged;
}

bool BuildingFilterBase::isFiltered(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	return !m_bActive || (m_bInvert ^ isFilteredBuilding(pPlayer, pCity, eBuilding));
}

BuildingFilterBase::~BuildingFilterBase()
{

}

bool BuildingFilterCanBuild::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->canConstruct(eBuilding, false, m_bShowSomeUnconstructable);
	}
	return pPlayer->canConstruct(eBuilding, false, m_bShowSomeUnconstructable);
}

bool BuildingFilterIsWonder::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	return isLimitedWonder(eBuilding);
}

bool BuildingFilterIsGreatWonder::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	return isWorldWonder(eBuilding);
}

bool BuildingFilterIsNationalWonder::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	return isNationalWonder(eBuilding);
}

BuildingFilterIsCommerce::BuildingFilterIsCommerce(CommerceTypes eCommerce, bool bInvert) : BuildingFilterBase(bInvert), m_eCommerce(eCommerce) {}

bool BuildingFilterIsCommerce::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->getAdditionalCommerceTimes100ByBuilding(m_eCommerce, eBuilding) > 0;
	}
	const CvBuildingInfo& buildingInfo = GC.getBuildingInfo(eBuilding);
	return buildingInfo.getCommerceChange(m_eCommerce) > 0
		|| buildingInfo.getCommercePerPopChange(m_eCommerce) > 0
		|| buildingInfo.getCommerceModifier(m_eCommerce) > 0
		|| buildingInfo.getSpecialistExtraCommerce(m_eCommerce) > 0
		|| buildingInfo.getGlobalCommerceModifier(m_eCommerce) > 0;
}

BuildingFilterIsYield::BuildingFilterIsYield(YieldTypes eYield, bool bInvert) : BuildingFilterBase(bInvert), m_eYield(eYield) {}

bool BuildingFilterIsYield::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->getAdditionalYieldByBuilding(m_eYield, eBuilding, true) > 0;
	}
	const CvBuildingInfo& info = GC.getBuildingInfo(eBuilding);

	foreach_(const PlotArray& pair, info.getPlotYieldChanges())
	{
		if (pair.second[m_eYield] > 0)
		{
			return true;
		}
	}
	foreach_(const TerrainArray& pair, info.getTerrainYieldChanges())
	{
		if (pair.second[m_eYield] > 0)
		{
			return true;
		}
	}
	return info.getYieldChange(m_eYield) > 0
		|| info.getYieldPerPopChange(m_eYield) > 0
		|| info.getYieldModifier(m_eYield) > 0
		|| info.getAreaYieldModifier(m_eYield) > 0
		|| info.getGlobalYieldModifier(m_eYield) > 0
		|| info.getGlobalSeaPlotYieldChange(m_eYield) > 0;
}

bool BuildingFilterIsHappiness::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->getAdditionalHappinessByBuilding(eBuilding) > 0;
	}
	const CvBuildingInfo& buildingInfo = GC.getBuildingInfo(eBuilding);
	foreach_(const TechModifier& modifier, buildingInfo.getTechHappinessChanges())
	{
		if (modifier.second > 0)
			return true;
	}
	return buildingInfo.getHappiness() > 0
		|| buildingInfo.getAreaHappiness() > 0
		|| buildingInfo.getGlobalHappiness() > 0;
}

bool BuildingFilterIsHealth::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->getAdditionalHealthByBuilding(eBuilding) > 0;
	}
	const CvBuildingInfo& buildingInfo = GC.getBuildingInfo(eBuilding);
	foreach_(const TechModifier& modifier, buildingInfo.getTechHealthChanges())
	{
		if (modifier.second > 0)
			return true;
	}
	return buildingInfo.getHealth() > 0
		|| buildingInfo.getAreaHealth() > 0
		|| buildingInfo.getGlobalHealth() > 0;
}

bool BuildingFilterIsUnhappiness::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->getAdditionalHappinessByBuilding(eBuilding) < 0;
	}
	const CvBuildingInfo& buildingInfo = GC.getBuildingInfo(eBuilding);
	foreach_(const TechModifier& modifier, buildingInfo.getTechHappinessChanges())
	{
		if (modifier.second < 0)
			return true;
	}
	return buildingInfo.getHappiness() < 0
		|| buildingInfo.getAreaHappiness() < 0
		|| buildingInfo.getGlobalHappiness() < 0;
}

bool BuildingFilterIsUnhealthiness::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->getAdditionalHealthByBuilding(eBuilding) < 0;
	}
	const CvBuildingInfo& buildingInfo = GC.getBuildingInfo(eBuilding);
	foreach_(const TechModifier& modifier, buildingInfo.getTechHealthChanges())
	{
		if (modifier.second < 0)
			return true;
	}
	return buildingInfo.getHealth() < 0
		|| buildingInfo.getAreaHealth() < 0
		|| buildingInfo.getGlobalHealth() < 0;
}

bool BuildingFilterIsMilitary::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const // not finished
{
	const CvBuildingInfo& buildingInfo = GC.getBuildingInfo(eBuilding);
	return buildingInfo.getMilitaryProductionModifier() > 0
		|| buildingInfo.getFreeExperience() > 0
		|| buildingInfo.getNumUnitCombatRetrainTypes() > 0
		|| buildingInfo.getNumUnitCombatProdModifiers() > 0
		|| !buildingInfo.getFreePromoTypes().empty()
		|| buildingInfo.getNumUnitCombatOngoingTrainingDurations() > 0
		|| !buildingInfo.getUnitCombatFreeExperience().empty()
		|| buildingInfo.isAnyDomainFreeExperience();
}

bool BuildingFilterIsCityDefense::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	const CvBuildingInfo& buildingInfo = GC.getBuildingInfo(eBuilding);
	if (GC.getGame().isOption(GAMEOPTION_SAD))
	{
		if (buildingInfo.getLocalDynamicDefense() > 0)
			return true;
	}
#ifdef STRENGTH_IN_NUMBERS
	if (GC.getGame().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		if(buildingInfo.getFrontSupportPercentModifier() > 0
		|| buildingInfo.getShortRangeSupportPercentModifier() > 0
		|| buildingInfo.getMediumRangeSupportPercentModifier() > 0
		|| buildingInfo.getLongRangeSupportPercentModifier() > 0
		|| buildingInfo.getFlankSupportPercentModifier() > 0)
		{
			return true;
		}
	}
#endif
	return buildingInfo.getDefenseModifier() > 0
		|| buildingInfo.getAllCityDefenseModifier() > 0
		|| buildingInfo.getAdjacentDamagePercent() > 0
		|| buildingInfo.getBombardDefenseModifier() > 0
		|| buildingInfo.getNumUnitCombatRepelModifiers() > 0
		|| buildingInfo.getLocalCaptureProbabilityModifier() > 0
		|| buildingInfo.getLocalCaptureResistanceModifier() > 0
		|| buildingInfo.getNationalCaptureResistanceModifier() > 0
		|| buildingInfo.getRiverDefensePenalty() < 0
		|| buildingInfo.getLocalRepel() > 0
		|| buildingInfo.getMinDefense() > 0
		|| buildingInfo.getBuildingDefenseRecoverySpeedModifier() > 0
		|| buildingInfo.getCityDefenseRecoverySpeedModifier() > 0
		|| buildingInfo.getNumUnitCombatRepelAgainstModifiers() > 0
		|| buildingInfo.getNumUnitCombatDefenseAgainstModifiers() > 0;
}

bool BuildingFilterIsProperty::isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	const CvBuildingInfo& kInfo = GC.getBuildingInfo(eBuilding);
	if ((kInfo.getProperties()->getValueByProperty(m_eProperty) != 0) || (kInfo.getPropertiesAllCities()->getValueByProperty(m_eProperty)))
		return true;

	const CvPropertyManipulators* pMani = kInfo.getPropertyManipulators();
	foreach_(const CvPropertySource* pSource, pMani->getSources())
	{
		if (pSource->getProperty() == m_eProperty)
			return true;
	}

	foreach_(const CvPropertyInteraction* pInteraction, pMani->getInteractions())
	{
		if (pInteraction->getSourceProperty() == m_eProperty)
			return true;
		if (pInteraction->getTargetProperty() == m_eProperty)
			return true;
	}

	foreach_(const CvPropertyPropagator* pPropagator, pMani->getPropagators())
	{
		if (pPropagator->getProperty() == m_eProperty)
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
	m_apBuildingFilters[BUILDING_FILTER_HIDE_UNBUILDABLE]->setActive(getBugOptionBOOL("CityScreen__HideUnconstructableBuildings", false));
}

BuildingFilterList::~BuildingFilterList()
{
	for (int i = 0; i < NUM_BUILDING_FILTERS; i++)
	{
		SAFE_DELETE(m_apBuildingFilters[i]);
	}
}

bool BuildingFilterList::isFilterActive(BuildingFilterTypes i) const
{
	FASSERT_BOUNDS(0, NUM_BUILDING_FILTERS, i)
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
	FASSERT_BOUNDS(0, NUM_BUILDING_FILTERS, i)
	return m_apBuildingFilters[i]->setActive(bActive);
}

bool BuildingFilterList::isFiltered(BuildingTypes eBuilding) const
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

			algo::for_each(kLoopPlayer.cities(), CvCity::fn::setBuildingListFilterActive(eFilter, bActive));
		}
	}
}