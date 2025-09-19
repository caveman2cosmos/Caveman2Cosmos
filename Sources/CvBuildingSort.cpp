//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvBuildingSort.cpp
//
//  PURPOSE: Sorting classes for buildings
//
//------------------------------------------------------------------------------------------------

#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvBuildingInfo.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvPlayer.h"

bool BuildingSortBase::isLesserBuilding(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding1, BuildingTypes eBuilding2)
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

bool BuildingSortBase::isInverse() const
{
	return m_bInvert;
}

bool BuildingSortBase::setInverse(bool bInvert)
{
	const bool bChanged = bInvert != m_bInvert;
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

int BuildingSortCommerce::getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const
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
		const CvBuildingInfo& kInfo = GC.getBuildingInfo(eBuilding);
		int iYieldCommerce = kInfo.getYieldChange(YIELD_COMMERCE) + kInfo.getYieldPerPopChange(YIELD_COMMERCE) + kInfo.getYieldModifier(YIELD_COMMERCE) / 5;
		int iCommerce = kInfo.getCommerceChange(m_eCommerce) + kInfo.getCommercePerPopChange(m_eCommerce) + kInfo.getCommerceModifier(m_eCommerce) / 5;
		iCommerce += iYieldCommerce * pPlayer->getCommercePercent(m_eCommerce);
		return iCommerce;
	}
}

BuildingSortYield::BuildingSortYield(YieldTypes eYield, bool bInvert) : BuildingSortBase(bInvert)
{
	m_eYield = eYield;
}

int BuildingSortYield::getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->getAdditionalYieldByBuilding(m_eYield, eBuilding);
	}
	else
	{
		const CvBuildingInfo& kInfo = GC.getBuildingInfo(eBuilding);
		return kInfo.getYieldChange(m_eYield) + kInfo.getYieldPerPopChange(m_eYield) + kInfo.getYieldModifier(m_eYield) / 5;
	}
}

int BuildingSortHappiness::getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->getAdditionalHappinessByBuilding(eBuilding);
	}
	else
	{
		const CvBuildingInfo& kInfo = GC.getBuildingInfo(eBuilding);
		return kInfo.getHappiness() + kInfo.getAreaHappiness() + kInfo.getGlobalHappiness();
	}
}

int BuildingSortHealth::getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->getAdditionalHealthByBuilding(eBuilding);
	}
	else
	{
		const CvBuildingInfo& kInfo = GC.getBuildingInfo(eBuilding);
		return kInfo.getHealth() + kInfo.getAreaHealth() + kInfo.getGlobalHealth();
	}
}

int BuildingSortCost::getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const
{
	if (pCity)
	{
		return pCity->getProductionNeeded(eBuilding) - pCity->getProgressOnBuilding(eBuilding);
	}
	else
	{
		return pPlayer->getProductionNeeded(eBuilding);
	}
}

int BuildingSortName::getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const
{
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	// Get the localized name/description of the building
	CvWString szName = kBuilding.getDescription();

	if (szName.empty())
		return 0;

	// Encode the first few characters of the name into an integer
	// This way we can return an int value that represents the alphabetical order
	int value = 0;
	for (int i = 0; i < std::min((int)szName.length(), 3); i++)
	{
		// Convert character to uppercase so sorting is case-insensitive
		wchar_t c = towupper(szName[i]);
		// Shift previous value and add current character code
		value = value * 256 + (int)c;
	}
	return -value;
}

// name sorting defaults to A first
bool BuildingSortName::isLesserBuilding(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding1, BuildingTypes eBuilding2) const
{
	if (m_bInvert)
		return wcscmp(GC.getBuildingInfo(eBuilding1).getDescription(), GC.getBuildingInfo(eBuilding2).getDescription()) > 0;
	else
		return wcscmp(GC.getBuildingInfo(eBuilding1).getDescription(), GC.getBuildingInfo(eBuilding2).getDescription()) < 0;
}

int BuildingSortProperty::getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	const CvBuildingInfo& kInfo = GC.getBuildingInfo(eBuilding);
	int iSum = kInfo.getProperties()->getValueByProperty(m_eProperty) + kInfo.getPropertiesAllCities()->getValueByProperty(m_eProperty);

	foreach_(const CvPropertySource* pSource, kInfo.getPropertyManipulators()->getSources())
	{
		if (pSource->getProperty() == m_eProperty)
		{
			iSum += pSource->getSourcePredict(pCity->getGameObject(), pCity->getProperties()->getValueByProperty(m_eProperty));
		}
	}
	return iSum;
}

BuildingSortList::BuildingSortList(CvPlayer *pPlayer, CvCity *pCity)
{
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < NUM_BUILDING_SORT; i++)
	{
		SAFE_DELETE(m_apBuildingSort[i]);
	}
}

void BuildingSortList::init()
{
	PROFILE_EXTRA_FUNC();
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

BuildingSortTypes BuildingSortList::getActiveSort() const
{
	return m_eActiveSort;
}

bool BuildingSortList::setActiveSort(BuildingSortTypes eActiveSort)
{
	FASSERT_BOUNDS(0, NUM_BUILDING_SORT, eActiveSort);
	const bool bChanged = m_eActiveSort != eActiveSort;
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