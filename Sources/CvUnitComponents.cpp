#include "CvUnitComponents.h"
#include "CvInfos.h"
//------------------------------------------------------------------------------------------------------
//  CLASS: UnitCompCommander
//------------------------------------------------------------------------------------------------------
UnitCompCommander::UnitCompCommander(const CvUnit* unit, short iCP, short iCPL, short iCR) // Used when loading save
{
	m_unit = unit;
	m_iControlPoints = iCP;
	m_iControlPointsLeft = iCPL;
	m_iCommandRange = iCR;
	m_bReady = m_iControlPointsLeft > 0;
}
UnitCompCommander::~UnitCompCommander() { }

UnitCompCommander::UnitCompCommander(const CvUnit* unit, CvUnitInfo* unitInfo) // Used when unit becomes commander
{
	m_unit = unit;
	m_iControlPoints = unitInfo->getControlPoints();
	m_iControlPointsLeft = m_iControlPoints;
	m_iCommandRange = unitInfo->getCommandRange();
	m_bReady = m_iControlPointsLeft > 0;

	FAssertMsg(m_bReady, "A commander with no CP is no commmander at all...");
}


void UnitCompCommander::changeControlPoints(const int iChange)
{
	if (iChange != 0)
	{
		m_iControlPoints += iChange;
		changeControlPointsLeft(iChange);

		FAssertMsg(m_iControlPoints > 0, "A commander with no CP is no commmander at all...");
	}
}

void UnitCompCommander::changeControlPointsLeft(const int iChange)
{
	if (iChange != 0)
	{
		const bool bWasReady = m_bReady;

		m_iControlPointsLeft += iChange;
		m_bReady = m_iControlPointsLeft > 0;

		if (bWasReady != m_bReady)
		{
			m_unit->plot()->countCommander(m_bReady, m_unit);
		}
	}
}

void UnitCompCommander::restoreControlPoints()
{
	if (m_iControlPointsLeft < m_iControlPoints)
	{
		changeControlPointsLeft(m_iControlPoints - m_iControlPointsLeft);
	}
}

void UnitCompCommander::changeCommandRange(const int iChange)
{
	if (iChange != 0)
	{
		if (m_bReady)
		{
			m_unit->plot()->countCommander(false, m_unit);
		}
		m_iCommandRange += iChange;

		if (m_bReady)
		{
			m_unit->plot()->countCommander(true, m_unit);
		}
	}
}

//------------------------------------------------------------------------------------------------------
//  CLASS: UnitCompWorker
//------------------------------------------------------------------------------------------------------
UnitCompWorker::UnitCompWorker()
{
	reset(true);
}
UnitCompWorker::~UnitCompWorker()
{
	m_extraBuilds.clear(); // Toffer - Not sure if this is needed, or even the correct way to free memory from vector...
	m_extraWorkModForBuild.clear();
}

UnitCompWorker::UnitCompWorker(CvUnitInfo* unitInfo) // Used when unit becomes commander
{
	m_iHillsWorkModifier = unitInfo->getHillsWorkModifier();
	m_iPeaksWorkModifier = unitInfo->getPeaksWorkModifier();
	reset(false);
}

void UnitCompWorker::reset(const bool bBlanc)
{
	if (bBlanc)
	{
		m_iHillsWorkModifier = 0;
		m_iPeaksWorkModifier = 0;
	}
	m_iWorkModifier = 0;
	m_iAssignedCity = -1;
}

void UnitCompWorker::changeHillsWorkModifier(const int iChange)
{
	m_iHillsWorkModifier += iChange;
}

void UnitCompWorker::changePeaksWorkModifier(const int iChange)
{
	m_iPeaksWorkModifier += iChange;
}

void UnitCompWorker::changeWorkModifier(const int iChange)
{
	m_iWorkModifier += iChange;
}

bool UnitCompWorker::hasExtraBuild(const BuildTypes eBuild) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBuild);
	return algo::any_of_equal(m_extraBuilds, eBuild);
}

void UnitCompWorker::setExtraBuild(BuildTypes eBuild, bool bNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBuild);

	std::vector<BuildTypes>::iterator itr = find(m_extraBuilds.begin(), m_extraBuilds.end(), eBuild);

	if (bNewValue)
	{
		if (itr == m_extraBuilds.end())
		{
			m_extraBuilds.push_back(eBuild);
		}
	}
	else if (itr != m_extraBuilds.end())
	{
		m_extraBuilds.erase(itr);
	}
}

void UnitCompWorker::changeExtraWorkModForBuild(const BuildTypes eBuild, const short iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBuild);
	if (iChange == 0) return;

	std::map<BuildTypes, short>::const_iterator itr = m_extraWorkModForBuild.find(eBuild);

	if (itr == m_extraWorkModForBuild.end())
	{
		m_extraWorkModForBuild.insert(std::make_pair(eBuild, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_extraWorkModForBuild.erase(itr->first);
	}
	else // change unit mod
	{
		m_extraWorkModForBuild[itr->first] += iChange;
	}
}

int UnitCompWorker::getExtraWorkModForBuild(const BuildTypes eBuild) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBuild);
	std::map<BuildTypes, short>::const_iterator itr = m_extraWorkModForBuild.find(eBuild);
	return itr != m_extraWorkModForBuild.end() ? itr->second : 0;
}
//------------------------------------------------------------------------------------------------------
//  CLASS: X
//------------------------------------------------------------------------------------------------------