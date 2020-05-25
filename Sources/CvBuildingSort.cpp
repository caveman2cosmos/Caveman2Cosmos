//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvBuildingSort.cpp
//
//  PURPOSE: Sorting classes for buildings
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"

bool BuildingSortBase::isLesserBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding1, BuildingTypes eBuilding2)
{
	int iVal1;
	int iVal2;

	stdext::hash_map<BuildingTypes, int>::iterator it = m_mapValueCache.find(eBuilding1);
	if (it == m_mapValueCache.end())
	{
		iVal1 = getBuildingValue(pPlayer, pCity, eBuilding1);
		m_mapValueCache[eBuilding1] = iVal1;
	}
	else
		iVal1 = it->second;

	it = m_mapValueCache.find(eBuilding2);
	if (it == m_mapValueCache.end())
	{
		iVal2 = getBuildingValue(pPlayer, pCity, eBuilding2);
		m_mapValueCache[eBuilding2] = iVal2;
	}
	else
		iVal2 = it->second;

	// To keep the strict weak ordering for sorting, the result of the comparison cannot just be inverted, equal must always be false
	if (m_bInvert)
		return iVal1 < iVal2;
	else
		return iVal1 > iVal2;
}

bool BuildingSortBase::isInverse()
{
	return m_bInvert;
}

bool BuildingSortBase::setInverse(bool bInvert)
{
	bool bChanged = bInvert != m_bInvert;
	m_bInvert = bInvert;
	return bChanged;
}

void BuildingSortBase::deleteCache()
{
	m_mapValueCache.clear();
}

BuildingSortBase::~BuildingSortBase()
{

}

BuildingSortCommerce::BuildingSortCommerce(CommerceTypes eCommerce, bool bInvert) : BuildingSortBase(bInvert)
{
	m_eCommerce = eCommerce;
}

int BuildingSortCommerce::getBuildingValue(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		int iYieldCommerce = pCity->getAdditionalYieldByBuilding(YIELD_COMMERCE, eBuilding);
		int iCommerce = pCity->getAdditionalCommerceTimes100ByBuilding(m_eCommerce, eBuilding);
		iCommerce += iYieldCommerce * pPlayer->getCommercePercent(m_eCommerce);
		if (m_eCommerce == COMMERCE_GOLD)
			iCommerce += pCity->getSavedMaintenanceTimes100ByBuilding(eBuilding);
		return iCommerce;
	}
	else
	{
		int iYieldCommerce = GC.getBuildingInfo(eBuilding).getYieldChange(YIELD_COMMERCE) + GC.getBuildingInfo(eBuilding).getYieldPerPopChange(YIELD_COMMERCE) + GC.getBuildingInfo(eBuilding).getYieldModifier(YIELD_COMMERCE) / 5;
		int iCommerce = GC.getBuildingInfo(eBuilding).getCommerceChange(m_eCommerce) + GC.getBuildingInfo(eBuilding).getCommercePerPopChange(m_eCommerce) + GC.getBuildingInfo(eBuilding).getCommerceModifier(m_eCommerce) / 5;
		iCommerce += iYieldCommerce * pPlayer->getCommercePercent(m_eCommerce);
		return iCommerce;
	}
}

BuildingSortYield::BuildingSortYield(YieldTypes eYield, bool bInvert) : BuildingSortBase(bInvert)
{
	m_eYield = eYield;
}

int BuildingSortYield::getBuildingValue(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		return pCity->getAdditionalYieldByBuilding(m_eYield, eBuilding);
	}
	else
	{
		return GC.getBuildingInfo(eBuilding).getYieldChange(m_eYield) + GC.getBuildingInfo(eBuilding).getYieldPerPopChange(m_eYield) + GC.getBuildingInfo(eBuilding).getYieldModifier(m_eYield) / 5;
	}
}

int BuildingSortHappiness::getBuildingValue(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		return pCity->getAdditionalHappinessByBuilding(eBuilding);
	}
	else
	{
		return GC.getBuildingInfo(eBuilding).getHappiness() + GC.getBuildingInfo(eBuilding).getAreaHappiness() + GC.getBuildingInfo(eBuilding).getGlobalHappiness();
	}
}

int BuildingSortHealth::getBuildingValue(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		return pCity->getAdditionalHealthByBuilding(eBuilding);
	}
	else
	{
		return GC.getBuildingInfo(eBuilding).getHealth() + GC.getBuildingInfo(eBuilding).getAreaHealth() + GC.getBuildingInfo(eBuilding).getGlobalHealth();
	}
}

int BuildingSortCost::getBuildingValue(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (pCity)
	{
		return pCity->getProductionNeeded(eBuilding) - pCity->getBuildingProduction(eBuilding);
	}
	else
	{
		return pPlayer->getProductionNeeded(eBuilding);
	}
}

// dummy
int BuildingSortName::getBuildingValue(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	return 0;
}

// name sorting defaults to A first
bool BuildingSortName::isLesserBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding1, BuildingTypes eBuilding2)
{
	if (m_bInvert)
		return wcscmp(GC.getBuildingInfo(eBuilding1).getDescription(), GC.getBuildingInfo(eBuilding2).getDescription()) > 0;
	else
		return wcscmp(GC.getBuildingInfo(eBuilding1).getDescription(), GC.getBuildingInfo(eBuilding2).getDescription()) < 0;
}

int BuildingSortProperty::getBuildingValue(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	CvBuildingInfo& kInfo = GC.getBuildingInfo(eBuilding);
	int iSum = kInfo.getProperties()->getValueByProperty(m_eProperty) + kInfo.getPropertiesAllCities()->getValueByProperty(m_eProperty);

	CvPropertyManipulators* pMani = kInfo.getPropertyManipulators();
	int iNum = pMani->getNumSources();
	for (int i=0; i<iNum; i++)
	{
		CvPropertySource* pSource = pMani->getSource(i);
		if (pSource->getProperty() == m_eProperty)
		{
			iSum += pSource->getSourcePredict(pCity->getGameObject(), pCity->getProperties()->getValueByProperty(m_eProperty));
		}
	}
	return iSum;
}

BuildingSortList::BuildingSortList(CvPlayer *pPlayer, CvCity *pCity)
{
	m_pPlayer = pPlayer;
	m_pCity = pCity;

	for (int i = 0; i < NUM_BUILDING_SORT; i++)
	{
		m_apBuildingSort[i] = NULL;
	}

	m_eActiveSort = BUILDING_SORT_COST;
}

BuildingSortList::~BuildingSortList()
{
	for (int i = 0; i < NUM_BUILDING_SORT; i++)
	{
		SAFE_DELETE(m_apBuildingSort[i]);
	}
}

void BuildingSortList::init()
{
	for (int i = 0; i < NUM_BUILDING_SORT; i++)
	{
		SAFE_DELETE(m_apBuildingSort[i]);
	}

	m_apBuildingSort[BUILDING_SORT_NAME] = new BuildingSortName();
	m_apBuildingSort[BUILDING_SORT_COST] = new BuildingSortCost(true);
	m_apBuildingSort[BUILDING_SORT_SCIENCE] = new BuildingSortCommerce(COMMERCE_RESEARCH);
	m_apBuildingSort[BUILDING_SORT_CULTURE] = new BuildingSortCommerce(COMMERCE_CULTURE);
	m_apBuildingSort[BUILDING_SORT_ESPIONAGE] = new BuildingSortCommerce(COMMERCE_ESPIONAGE);
	m_apBuildingSort[BUILDING_SORT_GOLD] = new BuildingSortCommerce(COMMERCE_GOLD);
	m_apBuildingSort[BUILDING_SORT_FOOD] = new BuildingSortYield(YIELD_FOOD);
	m_apBuildingSort[BUILDING_SORT_PRODUCTION] = new BuildingSortYield(YIELD_PRODUCTION);
	m_apBuildingSort[BUILDING_SORT_HAPPINESS] = new BuildingSortHappiness();
	m_apBuildingSort[BUILDING_SORT_HEALTH] = new BuildingSortHealth();
	m_apBuildingSort[BUILDING_SORT_CRIME] = new BuildingSortProperty((PropertyTypes)GC.getInfoTypeForString("PROPERTY_CRIME"), true);
	m_apBuildingSort[BUILDING_SORT_FLAMMABILITY] = new BuildingSortProperty((PropertyTypes)GC.getInfoTypeForString("PROPERTY_FLAMMABILITY"), true);
}

int BuildingSortList::getNumSort()
{
	return NUM_BUILDING_SORT;
}

BuildingSortTypes BuildingSortList::getActiveSort()
{
	return m_eActiveSort;
}

bool BuildingSortList::setActiveSort(BuildingSortTypes eActiveSort)
{
	FAssertMsg(eActiveSort < NUM_BUILDING_SORT, "Index out of bounds");
	FAssertMsg(eActiveSort > -1, "Index out of bounds");
	bool bChanged = m_eActiveSort != eActiveSort;
	m_eActiveSort = eActiveSort;
	return bChanged;
}

void BuildingSortList::setCity(CvCity *pCity)
{
	m_pCity = pCity;
}

void BuildingSortList::setPlayer(CvPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
}

bool BuildingSortList::operator ()(BuildingTypes eBuilding1, BuildingTypes eBuilding2)
{
	return m_apBuildingSort[m_eActiveSort]->isLesserBuilding(m_pPlayer, m_pCity, eBuilding1, eBuilding2);
}

void BuildingSortList::deleteCache()
{
	m_apBuildingSort[m_eActiveSort]->deleteCache();
}