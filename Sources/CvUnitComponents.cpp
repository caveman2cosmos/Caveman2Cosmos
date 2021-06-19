//------------------------------------------------------------------------------------------------------
//  CLASS: UnitCompCommander
//------------------------------------------------------------------------------------------------------
UnitCompCommander::UnitCompCommander() // Used when loading save
{
	m_iControlPoints = 0;
	m_iControlPointsLeft = 0;
	m_iCommandRange = 0;
}
UnitCompCommander::~UnitCompCommander() { }

UnitCompCommander::UnitCompCommander(CvUnitInfo* unitInfo) // Used when unit becomes commander
{
	m_iControlPoints = unitInfo->getControlPoints();
	m_iControlPointsLeft = m_iControlPoints;

	m_iCommandRange = unitInfo->getCommandRange();
}

void UnitCompCommander::changeControlPoints(const int iChange)
{
	m_iControlPoints += iChange;
	m_iControlPointsLeft += iChange;
}

void UnitCompCommander::changeControlPointsLeft(const int iChange)
{
	m_iControlPointsLeft += iChange;
}

void UnitCompCommander::restoreControlPoints()
{
	m_iControlPointsLeft = m_iControlPoints;
}


void UnitCompCommander::changeCommandRange(const int iChange)
{
	m_iCommandRange += iChange;
}

//------------------------------------------------------------------------------------------------------
//  CLASS: UnitCompWorker
//------------------------------------------------------------------------------------------------------
UnitCompWorker::UnitCompWorker()
{
	m_iHillsWorkModifier = 0;
	m_iPeaksWorkModifier = 0;
	m_iWorkModifier = 0;
}
UnitCompWorker::~UnitCompWorker()
{
	m_extraBuilds.clear(); // Toffer - Not sure if this is needed, or even the correct way to free memory from vector...
}

UnitCompWorker::UnitCompWorker(CvUnitInfo* unitInfo) // Used when unit becomes commander
{
	m_iHillsWorkModifier = unitInfo->getHillsWorkModifier();
	m_iPeaksWorkModifier = unitInfo->getPeaksWorkModifier();
	m_iWorkModifier = 0;
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
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBuild)
	return find(m_extraBuilds.begin(), m_extraBuilds.end(), eBuild) != m_extraBuilds.end();
}

void UnitCompWorker::setExtraBuild(BuildTypes eBuild, bool bNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBuild)

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

//------------------------------------------------------------------------------------------------------
//  CLASS: X
//------------------------------------------------------------------------------------------------------