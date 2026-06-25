#pragma once

#ifndef UNITS_REPO_H
#define UNITS_REPO_H

// Named lookups over the global CvUnitInfo array.
//
// Mirrors BuildingsRepo/BuildsRepo: rebuilt after XML load and after every
// cvInternalGlobals::updateReplacements() (replacement pointer swaps can
// change CvUnitInfo fields for a given UnitTypes id).

class UnitsRepo
	: private bst::noncopyable
{
public:
	static UnitsRepo& get();

	// Rebuild every index from GC.m_paUnitInfo. Idempotent.
	void rebuild();

	// Units with isFound() set (city-founders / settlers).
	// Sorted ascending by UnitTypes id.
	const std::vector<UnitTypes>& founderUnits() const;

	// Units whose isPrereqAndBuilding(eBuilding) is true.
	// Returns an empty vector for NO_BUILDING or out-of-range eBuilding.
	const std::vector<UnitTypes>& unitsRequiringBuilding(BuildingTypes eBuilding) const;

private:
	UnitsRepo();
	~UnitsRepo();

	std::vector<UnitTypes> m_founderUnits;
	// m_unitsRequiringBuilding[b] = units whose isPrereqAndBuilding(b) is true.
	std::vector<std::vector<UnitTypes> > m_unitsRequiringBuilding;
	std::vector<UnitTypes> m_emptyBucket;
};

#endif // UNITS_REPO_H
