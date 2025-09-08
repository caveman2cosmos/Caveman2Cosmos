// plotGroup.cpp


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvDLLFAStarIFaceBase.h"

//#define VALIDATION_FOR_PLOT_GROUPS

int CvPlotGroup::m_allocationSeqForSession = 0;

// Public Functions...

CvPlotGroup::CvPlotGroup()
	: m_sessionAllocSeq(0)
	, m_sessionRecalcSeq(0)
	, m_iID(0)
	, m_eOwner(NO_PLAYER)
	, m_paiNumBonuses(NULL)
	, m_seedPlotX(0)
	, m_seedPlotY(0)
	, m_zobristHashes()
	, m_numPlots(0)
	, m_numCities(-1)
{
	reset(0, NO_PLAYER, true);
}


CvPlotGroup::~CvPlotGroup()
{
	uninit();
}


void CvPlotGroup::init(int iID, PlayerTypes eOwner, CvPlot* pPlot, bool bRecalculateBonuses)
{
	//--------------------------------
	// Init saved data
	reset(iID, eOwner);

	//--------------------------------
	// Init non-saved data
	m_zobristHashes.allNodesHash = 0;
	m_zobristHashes.resourceNodesHash = 0;

	//--------------------------------
	// Init other game data
	addPlot(pPlot, bRecalculateBonuses);
}


void CvPlotGroup::uninit()
{
	SAFE_DELETE_ARRAY(m_paiNumBonuses);
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvPlotGroup::reset(int iID, PlayerTypes eOwner, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	m_eOwner = eOwner;
	m_numCities = -1;
	m_numPlots = 0;
	m_sessionAllocSeq = m_allocationSeqForSession++;
	m_sessionRecalcSeq = m_recalcSeqForSession;

	if (!bConstructorCall)
	{
		SAFE_DELETE_ARRAY(m_paiNumBonuses);
	}
}


void CvPlotGroup::addPlot(CvPlot* pPlot, bool bRecalculateBonuses)
{
	PROFILE_FUNC();

	pPlot->setPlotGroup(getOwner(), this, bRecalculateBonuses);

	//	Add the zobrist contribution of this plot to the hash
	m_zobristHashes.allNodesHash ^= pPlot->getZobristContribution();
	if ( pPlot->isCity() ||
		 (pPlot->getImprovementType() != NO_IMPROVEMENT && pPlot->getBonusType() != NO_BONUS) )
	{
		m_zobristHashes.resourceNodesHash ^= pPlot->getZobristContribution();
	}

	if ( m_numCities != -1 && pPlot->isCity() && pPlot->getPlotCity()->getOwner() == getOwner())
	{
		m_numCities++;
	}

	m_numPlots++;

	if ( m_seedPlotX == -1 )
	{
		m_seedPlotX = pPlot->getX();
		m_seedPlotY = pPlot->getY();
	}
}


void CvPlotGroup::removePlot(CvPlot* pPlot, bool bRecalculateBonuses)
{
	PROFILE_FUNC();

	FAssert(pPlot->getPlotGroup(m_eOwner) == this);
	if ( pPlot->getPlotGroup(m_eOwner) == this )
	{
		pPlot->setPlotGroup(getOwner(), NULL, bRecalculateBonuses);

		if ( --m_numPlots == 0 )
		{
			GET_PLAYER(getOwner()).deletePlotGroup(getID());
		}
		else
		{
			if ( m_numCities != -1 && pPlot->isCity() && pPlot->getPlotCity()->getOwner() == getOwner() )
			{
				m_numCities--;
			}

			//	Remove the zobrist contribution of this plot to the hash
			m_zobristHashes.allNodesHash ^= pPlot->getZobristContribution();
			if ( pPlot->isCity() ||
				 (pPlot->getImprovementType() != NO_IMPROVEMENT && pPlot->getBonusType() != NO_BONUS) )
			{
				m_zobristHashes.resourceNodesHash ^= pPlot->getZobristContribution();
			}
		}
	}
}

#ifdef _DEBUG
void CvPlotGroup::Validate()
{
//#if 0
//	CLLNode<XYCoords>* pPlotNode = headPlotsNode();
//
//	while (pPlotNode != NULL)
//	{
//		CvPlot* pPlot = GC.getMap().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY);
//
//		FAssert(pPlot->getPlotGroup(m_eOwner) == this);
//		pPlotNode = nextPlotsNode(pPlotNode);
//	}
//#endif
}
#endif

CvPlot* CvPlotGroup::getRepresentativePlot() const
{
	PROFILE_EXTRA_FUNC();
	CvPlot*	result = NULL;

	if ( m_seedPlotX != -1 && m_seedPlotY != -1 )
	{
		result = GC.getMap().plot(m_seedPlotX, m_seedPlotY);

		if ( result != NULL && result->getPlotGroup(m_eOwner) != this )
		{
			result = NULL;
		}
	}

	if ( result == NULL )
	{
		for(int iI = 0; iI < GC.getMap().getGridWidth(); iI++)
		{
			for(int iJ = 0; iJ < GC.getMap().getGridHeight(); iJ++)
			{
				result = GC.getMap().plotSorenINLINE(iI, iJ);

				if ( result->getPlotGroup(m_eOwner) == this )
				{
					m_seedPlotX = iI;
					m_seedPlotY = iJ;

					return result;
				}
			}
		}

		return NULL;
	}

	return result;
}

typedef struct buildRemovedPlotListParams
{
	CLinkList<XYCoords> removedPlots;
	int					groupGenNumber;
} buildRemovedPlotListParams;

static bool buildRemovedPlotList(CvPlotGroup* onBehalfOf, CvPlot* pLoopPlot, void* params)
{
	buildRemovedPlotListParams* parm = (buildRemovedPlotListParams*)params;

	//	If the plot was not visited in the generational check above it is no longer
	//	part of this plot group
	if ( pLoopPlot->m_groupGenerationNumber != parm->groupGenNumber)
	{
		parm->removedPlots.insertAtEnd(XYCoords(*pLoopPlot));
	}

	return true;
}

typedef struct buildAllPlotListParams
{
	CLinkList<XYCoords> allPlots;
} buildAllPlotListParams;

static bool buildAllPlotList(CvPlotGroup* onBehalfOf, CvPlot* pLoopPlot, void* params)
{
	buildAllPlotListParams* parm = (buildAllPlotListParams*)params;

	parm->allPlots.insertAtEnd(XYCoords(*pLoopPlot));

	//OutputDebugString(CvString::format("Enumerated plot: (%d,%d)\n", xy.iX,xy.iY).c_str());

	return true;
}

void CvPlotGroup::recalculatePlots()
{
	PROFILE_FUNC();

	if ( m_bulkRecalcStartSeq != -1 && m_bulkRecalcStartSeq < m_sessionRecalcSeq )
	{
		//	This one has already been done
		return;
	}

	m_sessionRecalcSeq = ++m_recalcSeqForSession;

#ifdef _DEBUG
	Validate();
#endif

	PlayerTypes eOwner = getOwner();
	CvPlot* pPlot = getRepresentativePlot();
	if (pPlot == NULL)
	{
		return;
	}

	static int groupGenNumber = 0;

	plotGroupCheckInfo	checkInfo;

	checkInfo.hashInfo.allNodesHash = 0;
	checkInfo.hashInfo.resourceNodesHash = 0;
	checkInfo.groupGenerationNumber = ++groupGenNumber;

	//	Check whether the same set of cities and bonuses are still in the connected region
	//	If they are then there is no material change and we don't need to recalculate
	gDLL->getFAStarIFace()->SetData(&GC.getPlotGroupFinder(), &checkInfo);
	gDLL->getFAStarIFace()->GeneratePath(&GC.getPlotGroupFinder(), pPlot->getX(), pPlot->getY(), -1, -1, false, eOwner);

	if (checkInfo.hashInfo.allNodesHash == m_zobristHashes.allNodesHash)
	{
		return;
	}

	//	If the city+resource network has not changed then bonuses dont need to be
	//	recalculated and we can just use the pathing to update the plot groups on
	//	nodes that have been removed
	if (checkInfo.hashInfo.resourceNodesHash == m_zobristHashes.resourceNodesHash)
	{
		PROFILE_BEGIN("CvPlotGroup::recalculatePlots (unchanged bonuses)",IDPLOT);

		buildRemovedPlotListParams removedPlotParams;

		removedPlotParams.removedPlots.clear();
		removedPlotParams.groupGenNumber = groupGenNumber;

		plotEnumerator(buildRemovedPlotList, &removedPlotParams);

		PROFILE_END(IDPLOT);

		//	Just update the plot group for the removed plots
		for (CLLNode<XYCoords>* pPlotNode = removedPlotParams.removedPlots.head();
			pPlotNode != NULL;
			pPlotNode = removedPlotParams.removedPlots.next(pPlotNode))
		{
			pPlot = pPlotNode->m_data.plot();

			FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

			removePlot(pPlot, false);
			pPlot->updatePlotGroup(eOwner);
		}

		for (CLLNode<XYCoords>* pPlotNode = removedPlotParams.removedPlots.head();
			pPlotNode != NULL;
			pPlotNode = removedPlotParams.removedPlots.next(pPlotNode))
		{
			pPlot = GC.getMap().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY);

			FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

			if ( pPlot->getPlotGroup(m_eOwner) == NULL )
			{
				colorRegion(pPlot, m_eOwner, false);
			}
			//pPlot->updatePlotGroup(eOwner, false, false);
		}
	}
	else
	{
		PROFILE("CvPlotGroup::recalculatePlots update");

		m_numCities = -1;

		buildAllPlotListParams allPlotParams;

		allPlotParams.allPlots.clear();

		plotEnumerator(buildAllPlotList, &allPlotParams);

		//	Set them all to no plot group but without any update of bonuses for now
		for (CLLNode<XYCoords>* pPlotNode = allPlotParams.allPlots.head();
			pPlotNode != NULL;
			pPlotNode = allPlotParams.allPlots.next(pPlotNode))
		{
			PROFILE("CvPlotGroup::recalculatePlots update 1");

			pPlot = GC.getMap().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY);

			FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

			pPlot->setPlotGroup(eOwner, NULL, false);
			//OutputDebugString(CvString::format("Nulled plot group for: (%d,%d)\n", pPlot->getX(), pPlot->getY()).c_str());
		}

		const int iStartingAllocSeq = m_allocationSeqForSession;

		//	Construct new plot groups (still without bonus adjustment)
		for (CLLNode<XYCoords>* pPlotNode = allPlotParams.allPlots.head();
			pPlotNode != NULL;
			pPlotNode = allPlotParams.allPlots.next(pPlotNode))
		{
			PROFILE("CvPlotGroup::recalculatePlots update 2");

			pPlot = GC.getMap().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY);

			FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

			if ( pPlot->getPlotGroup(m_eOwner) == NULL )
			{
				colorRegion(pPlot, m_eOwner, false);
			}
			//pPlot->updatePlotGroup(eOwner, true, false);
		}

		//	Find the largest resulting new plot group
		int iLargest = 0;
		CvPlotGroup* pLargestGroup = NULL;

		for (CLLNode<XYCoords>* pPlotNode = allPlotParams.allPlots.head();
			pPlotNode != NULL;
			pPlotNode = allPlotParams.allPlots.next(pPlotNode))
		{
			PROFILE("CvPlotGroup::recalculatePlots update 2");

			pPlot = GC.getMap().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY);

			FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

			CvPlotGroup* pPlotGroup = pPlot->getPlotGroup(eOwner);

			if ( pPlotGroup != NULL && pPlotGroup->m_sessionAllocSeq >= iStartingAllocSeq && pPlotGroup->m_numPlots > iLargest )
			{
				pLargestGroup = pPlotGroup;
				iLargest = pLargestGroup->m_numPlots;
			}
		}

		//	Go through the new set picking one of the new plot groups to
		//	be replaced by a shrunken version of the original - its members will
		//	end up just staying with the original and necessitating no bonus recalculation
		//	Others will get bonus adjustment to their new plot groups
		CvPlotGroup* newTransitionGroup = pLargestGroup;
		int iPlotsTransferred = 0;

		m_numCities = 0;

		for (CLLNode<XYCoords>* pPlotNode = allPlotParams.allPlots.head();
			pPlotNode != NULL;
			pPlotNode = allPlotParams.allPlots.next(pPlotNode))
		{
			PROFILE("CvPlotGroup::recalculatePlots update 3");

			pPlot = GC.getMap().plotSorenINLINE(pPlotNode->m_data.iX, pPlotNode->m_data.iY);

			FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

			CvPlotGroup* plotGroup = pPlot->getPlotGroup(eOwner);

			if ( NULL == newTransitionGroup )
			{
				newTransitionGroup = plotGroup;
			}

			if ( plotGroup != NULL && newTransitionGroup == plotGroup )
			{
				//	Put this one back where it came from with no recalculation
				pPlot->setPlotGroup(eOwner, this, false);

				iPlotsTransferred++;
				newTransitionGroup->m_numPlots--;

				if ( pPlot->getPlotCity() != NULL && pPlot->getPlotCity()->getOwner() == m_eOwner )
				{
					m_numCities++;
				}
			}
			else
			{
				//	Transiently put it back so we can take it out again WITH bonus
				//	recalculation
				pPlot->setPlotGroup(eOwner, this, false);
				pPlot->setPlotGroup(eOwner, plotGroup);
			}
		}

		if ( newTransitionGroup != NULL )
		{
			if ( newTransitionGroup->m_numPlots > 0 )
			{
				mergeIn(newTransitionGroup,true);
			}
			else
			{
				GET_PLAYER(eOwner).deletePlotGroup(newTransitionGroup->getID());
			}
		}
	}

#ifdef VALIDATION_FOR_PLOT_GROUPS
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if ( pLoopPlot->getPlotGroupId(m_eOwner) != -1 && pLoopPlot->getPlotGroup(m_eOwner) == NULL )
		{
			::MessageBox(NULL, "Invalid plot group id found after recalc of specific plot group!", "CvGameCoreDLL", MB_OK);
		}
	}
#endif
}

int CvPlotGroup::getID() const
{
	return m_iID;
}


void CvPlotGroup::setID(int iID)
{
	m_iID = iID;
}


int CvPlotGroup::getNumBonuses(const BonusTypes eBonus) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);
	return (m_paiNumBonuses == NULL ? 0 : m_paiNumBonuses[eBonus]);
}


bool CvPlotGroup::hasBonus(const BonusTypes eBonus) const
{
	return getNumBonuses(eBonus) > 0;
}


void CvPlotGroup::changeNumBonuses(const BonusTypes eBonus, const int iChange)
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);

	if (iChange != 0)
	{
		if (m_paiNumBonuses == NULL)
		{
			m_paiNumBonuses = new int[GC.getNumBonusInfos()];
			memset(m_paiNumBonuses, 0, sizeof(int)*GC.getNumBonusInfos());
		}

		m_paiNumBonuses[eBonus] += iChange;

		foreach_(CvCity* pLoopCity, GET_PLAYER(getOwner()).cities())
		{
			if (pLoopCity->plotGroup(getOwner()) == this)
			{
				pLoopCity->changeNumBonuses(eBonus, iChange);
			}
		}
	}
}

void CvPlotGroup::plotEnumerator(bool (*pfFunc)(CvPlotGroup* onBehalfOf, CvPlot*, void*), void* param)
{
	CvPlot*	pStartPlot;
	bool bSeedValid = false;
	int expectedPlotsRemaining = m_numPlots;

	if ( m_seedPlotX != -1 && m_seedPlotY != -1 )
	{
		pStartPlot = GC.getMap().plot(m_seedPlotX, m_seedPlotY);

		if ( pStartPlot != NULL && pStartPlot->getPlotGroup(m_eOwner) == this )
		{
			bSeedValid = true;
		}
		else
		{
			m_seedPlotX = -1;
			m_seedPlotY = -1;
		}
	}

	if ( !bSeedValid )
	{
		pStartPlot = GC.getMap().plotSorenINLINE(GC.getMap().getGridWidth()/2,GC.getMap().getGridHeight()/2);
	}

	int iMaxXDistance = GC.getMap().getGridWidth();
	int iMaxYDistance = GC.getMap().getGridHeight();

	if (GC.getMap().isWrapX())
	{
		iMaxXDistance /= 2;
	}

	if (GC.getMap().isWrapY())
	{
		iMaxYDistance /= 2;
	}

	int iMinX = pStartPlot->getX() - iMaxXDistance + (GC.getMap().getGridWidth()%2 == 0 ? 1 : 0);
	int iMaxX = pStartPlot->getX() + iMaxXDistance;
	int iMinY = pStartPlot->getY() - iMaxYDistance + (GC.getMap().getGridHeight()%2 == 0 ? 1 : 0);
	int iMaxY = pStartPlot->getY() + iMaxYDistance;

	for(int iRadius = 0;
			iRadius <= std::max(iMaxXDistance, iMaxYDistance) && expectedPlotsRemaining > 0;
			iRadius++)
	{
		//	Examine the ring of plots iDistance away from us (this is a square)
		for(int iPerimeter = -iRadius;
		    iPerimeter < iRadius || (iRadius == 0 && iPerimeter == 0);
			iPerimeter++)
		{
			for( int iSide = 0; iSide < 4; iSide++ )
			{
				int iX = pStartPlot->getX();
				int iY = pStartPlot->getY();

				switch(iSide)
				{
				case 0:
					//	North side
					iX = iX - iPerimeter;
					iY = iY + iRadius;
					break;
				case 1:
					//	East side
					iX = iX + iRadius;
					iY = iY + iPerimeter;
					break;
				case 2:
					//	South side
					iX = iX + iPerimeter;
					iY = iY - iRadius;
					break;
				case 3:
					//	West side
					iX = iX - iRadius;
					iY = iY - iPerimeter;
					break;
				}

				if ( iX < iMinX || iY < iMinY || iX > iMaxX || iY > iMaxY)
				{
					continue;
				}

				CvPlot* pLoopPlot = GC.getMap().plot(iX,iY);

				if ( pLoopPlot != NULL && pLoopPlot->getPlotGroup(m_eOwner) == this )
				{
					expectedPlotsRemaining--;
					if ( m_seedPlotX == -1 )
					{
						m_seedPlotX = pLoopPlot->getX();
						m_seedPlotY = pLoopPlot->getY();
					}

					if ( !(*pfFunc)(this, pLoopPlot, param) )
					{
						return;
					}
				}

				if ( iRadius == 0 )
				{
					break;
				}
			}
		}
	}
}

static bool countCitiesCallback(CvPlotGroup* onBehalfOf, CvPlot* pLoopPlot, void* dummy)
{
	const CvCity* pCity = pLoopPlot->getPlotCity();

	if (pCity != NULL && pCity->getOwner() == onBehalfOf->getOwner())
	{
		onBehalfOf->m_numCities++;
	}

	return true;
}


int CvPlotGroup::getNumCities()
{
	if ( m_numCities == -1 )
	{
		m_numCities = 0;

		plotEnumerator(countCitiesCallback, NULL);
	}

	return m_numCities;
}

void CvPlotGroup::read(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// Init saved data
	reset();

	WRAPPER_READ(wrapper, "CvPlotGroup", &m_iID);

	WRAPPER_READ(wrapper, "CvPlotGroup", (int*)&m_eOwner);

	bool arrayPresent = true;
	WRAPPER_READ_DECORATED(wrapper, "CvPlotGroup", &arrayPresent, "bonusesPresent");
	if ( arrayPresent )
	{
		if ( m_paiNumBonuses == NULL )
		{
			m_paiNumBonuses = new int[GC.getNumBonusInfos()];
		}
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlotGroup", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonuses);
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiNumBonuses);
	}
	WRAPPER_READ(wrapper, "CvPlotGroup", &m_numPlots);

	if ( m_paiNumBonuses != NULL )
	{
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			if ( m_paiNumBonuses[iI] != 0 )
			{
				break;
			}
		}

		if ( iI == GC.getNumBonusInfos() )
		{
			SAFE_DELETE_ARRAY(m_paiNumBonuses);
		}
	}

	WRAPPER_READ(wrapper, "CvPlotGroup", &m_seedPlotX);
	WRAPPER_READ(wrapper, "CvPlotGroup", &m_seedPlotY);

	WRAPPER_READ_OBJECT_END(wrapper);
}


void CvPlotGroup::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvPlotGroup", m_iID);
	WRAPPER_WRITE(wrapper, "CvPlotGroup", m_eOwner);

	WRAPPER_WRITE_DECORATED(wrapper, "CvPlotGroup", (bool)(m_paiNumBonuses != NULL), "bonusesPresent");

	if (m_paiNumBonuses != NULL)
	{
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlotGroup", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonuses);
	}

	WRAPPER_WRITE(wrapper, "CvPlotGroup", m_numPlots);
	WRAPPER_WRITE(wrapper, "CvPlotGroup", m_seedPlotX);
	WRAPPER_WRITE(wrapper, "CvPlotGroup", m_seedPlotY);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}


static bool zobristHashSetter(CvPlotGroup* onBehalfOf, CvPlot* pLoopPlot, void* params)
{
	plotGroupHashInfo* parm = (plotGroupHashInfo*)params;

	parm->allNodesHash ^= pLoopPlot->getZobristContribution();
	if ( pLoopPlot->isCity() ||
		 (pLoopPlot->getImprovementType() != NO_IMPROVEMENT && pLoopPlot->getBonusType() != NO_BONUS) )
	{
		parm->resourceNodesHash ^= pLoopPlot->getZobristContribution();
	}

	return true;
}

//	Calculate the hashes after a fresh game load
void CvPlotGroup::RecalculateHashes()
{
	//	Set up the Zobrist hashes
	plotEnumerator(zobristHashSetter, &m_zobristHashes);
}

typedef struct plotGroupMergerParams
{
	CvPlotGroup* mergeTo;
	bool		 bRecalculateBonuses;
} plotGroupMergerParams;

static bool plotGroupMerger(CvPlotGroup* onBehalfOf, CvPlot* pLoopPlot, void* params)
{
	plotGroupMergerParams* parm = (plotGroupMergerParams*)params;

	parm->mergeTo->addPlot(pLoopPlot, parm->bRecalculateBonuses);

	return true;
}

void CvPlotGroup::mergeIn(CvPlotGroup* from, bool bRecalculateBonuses)
{
	PROFILE_FUNC();

	plotGroupMergerParams params;

	params.mergeTo = this;
	params.bRecalculateBonuses = bRecalculateBonuses;

	from->plotEnumerator(plotGroupMerger, &params);

	GET_PLAYER(getOwner()).deletePlotGroup(from->getID());
}

void CvPlotGroup::colorRegion(CvPlot* pStartPlot, PlayerTypes eOwner, bool bRecalculateBonuses)
{
	FAssert(pStartPlot->getPlotGroup(eOwner) == NULL);

	colorRegionInternal(pStartPlot, eOwner, NULL, bRecalculateBonuses);
}

CvPlotGroup* CvPlotGroup::colorRegionInternal(CvPlot* pPlot, PlayerTypes eOwner, CvPlotGroup* pPlotGroup, bool bRecalculateBonuses)
{
	PROFILE_FUNC();

	if (pPlot->isTradeNetwork(GET_PLAYER(eOwner).getTeam()))
	{
		std::vector<CvPlot*> queue;

		queue.reserve(100);
		queue.push_back(pPlot);

		int iWaterMark = 0;

		while( iWaterMark < (int) queue.size() )
		{
			CvPlot* pLoopPlot = queue[iWaterMark++];

			if ( pLoopPlot->getPlotGroup(eOwner) == NULL )
			{
				if ( pPlotGroup == NULL )
				{
					pPlotGroup = GET_PLAYER(eOwner).initPlotGroup(pLoopPlot, bRecalculateBonuses);
				}
				else
				{
					pPlotGroup->addPlot(pLoopPlot, bRecalculateBonuses);
				}

				foreach_(CvPlot* pAdjacentPlot, pLoopPlot->adjacent())
				{
					if (pLoopPlot->isTradeNetworkConnected(pAdjacentPlot, GET_PLAYER(eOwner).getTeam()))
					{
						CvPlotGroup* pAdjacentPlotGroup = pAdjacentPlot->getPlotGroup(eOwner);

						if (pAdjacentPlotGroup != NULL)
						{
							if ( pAdjacentPlotGroup != pPlotGroup )
							{
								if ( pPlotGroup->getLengthPlots() > pAdjacentPlotGroup->getLengthPlots() )
								{
									pPlotGroup->mergeIn(pAdjacentPlotGroup, bRecalculateBonuses);
								}
								else
								{
									pAdjacentPlotGroup->mergeIn(pPlotGroup, bRecalculateBonuses);

									pPlotGroup = pAdjacentPlotGroup;
								}
							}
						}
						else
						{
							queue.push_back(pAdjacentPlot);
						}
					}
				}
			}
		}
	}

	return pPlotGroup;
}

int CvPlotGroup::m_bulkRecalcStartSeq = -1;
int CvPlotGroup::m_recalcSeqForSession = 0;

void CvPlotGroup::startBulkRecalculate()
{
	m_bulkRecalcStartSeq = m_recalcSeqForSession;
}

void CvPlotGroup::endBulkRecalculate()
{
	m_bulkRecalcStartSeq = -1;
}
