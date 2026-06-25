#pragma once

#ifndef BUILDS_REPO_H
#define BUILDS_REPO_H

// Named lookups over the global CvBuildInfo array.
//
// Mirrors BuildingsRepo: rebuilt after XML load and after every
// cvInternalGlobals::updateReplacements() (replacement pointer swaps can
// change CvBuildInfo fields for a given BuildTypes id).

class BuildsRepo
	: private bst::noncopyable
{
public:
	static BuildsRepo& get();

	// Rebuild every index from GC.m_paBuildInfo. Idempotent.
	void rebuild();

	// Builds whose CvBuildInfo::getImprovement() != NO_IMPROVEMENT.
	// Sorted ascending by BuildTypes id.
	const std::vector<BuildTypes>& improvementBuilds() const;

	// Builds whose CvBuildInfo::getRoute() != NO_ROUTE.
    // Sorted ascending by BuildTypes id.
    const std::vector<BuildTypes>& routeBuilds() const;

    // Builds whose produced improvement has getCulture() > 0.
    // Used by the worker AI to consider culture-extending improvements on bonus tiles
    // outside city radius, in addition to improvements that directly trade the bonus.
    // Sorted ascending by BuildTypes id.
    const std::vector<BuildTypes>& cultureBuilds() const;

private:
	BuildsRepo();
	~BuildsRepo();

	std::vector<BuildTypes> m_improvementBuilds;
	std::vector<BuildTypes> m_routeBuilds;
	std::vector<BuildTypes> m_cultureBuilds;
};

#endif // BUILDS_REPO_H
