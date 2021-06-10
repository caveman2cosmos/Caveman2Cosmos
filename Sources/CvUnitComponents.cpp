//------------------------------------------------------------------------------------------------------
//
//  CLASS: UnitCompCommander
//
//------------------------------------------------------------------------------------------------------
UnitCompCommander::UnitCompCommander() // Used when loading save
{
	m_iControlPoints = 0;
	m_iControlPointsLeft = 0;
	m_iCommandRange = 0;
}
UnitCompCommander::~UnitCompCommander() { }

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
//
//  CLASS: X
//
//------------------------------------------------------------------------------------------------------