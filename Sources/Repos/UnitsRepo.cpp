//------------------------------------------------------------------------------------------------
//  FILE:    UnitsRepo.cpp
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "UnitsRepo.h"
#include "CvGlobals.h"
#include "CvInfos.h"

UnitsRepo& UnitsRepo::get()
{
	static UnitsRepo s_instance;
	return s_instance;
}

UnitsRepo::UnitsRepo()
{
}

UnitsRepo::~UnitsRepo()
{
}

void UnitsRepo::rebuild()
{
	PROFILE_EXTRA_FUNC();

	const int iNumUnits = GC.getNumUnitInfos();
	const int iNumBuildings = GC.getNumBuildingInfos();

	m_founderUnits.clear();
	m_unitsRequiringBuilding.clear();
	m_unitsRequiringBuilding.resize(iNumBuildings);

	for (int iI = 0; iI < iNumUnits; ++iI)
	{
		const CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes)iI);

		if (kUnit.isFound())
		{
			m_founderUnits.push_back((UnitTypes)iI);
		}

		// isPrereqAndBuilding(int) is per-building; sweep the building space once
		// to bucket this unit under each building it requires as a prereq.
		for (int iB = 0; iB < iNumBuildings; ++iB)
		{
			if (kUnit.isPrereqAndBuilding(iB))
			{
				m_unitsRequiringBuilding[iB].push_back((UnitTypes)iI);
			}
		}
	}
}

const std::vector<UnitTypes>& UnitsRepo::founderUnits() const
{
	return m_founderUnits;
}

const std::vector<UnitTypes>& UnitsRepo::unitsRequiringBuilding(BuildingTypes eBuilding) const
{
	if (eBuilding < 0 || (int)eBuilding >= (int)m_unitsRequiringBuilding.size())
	{
		return m_emptyBucket;
	}
	return m_unitsRequiringBuilding[eBuilding];
}
