
#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvBonusInfo.h"
#include "CvFractal.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvMapGenerator.h"
#include "CvPlot.h"
#include "CvPython.h"

//
// static
//
CvMapGenerator* CvMapGenerator::m_pInst = NULL;

//
// singleton accessor
//
CvMapGenerator& CvMapGenerator::GetInstance()
{
	if (m_pInst==NULL)
	{
		m_pInst = new CvMapGenerator;
	}
	return *m_pInst;
}


CvMapGenerator::CvMapGenerator() { }

CvMapGenerator::~CvMapGenerator() { }


bool CvMapGenerator::canPlaceBonusAt(BonusTypes eBonus, int iX, int iY, bool bIgnoreLatitude)
{
	PROFILE_FUNC();

	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	if (!pPlot->canHaveBonus(eBonus, bIgnoreLatitude))
	{
		return false;
	}

	{
		bool result = false;
		if (Cy::call_override<bool>(GC.getMap().getMapScript(), "canPlaceBonusAt", Cy::Args() << pPlot, result))
		{
			return result;
		}
	}

	const CvBonusInfo& bonus = GC.getBonusInfo(eBonus);
	const int iBonusClass = bonus.getBonusClassType();

	if (pPlot->isWater()
	&& GC.getMap().getNumBonusesOnLand(eBonus) * 100 / (GC.getMap().getNumBonuses(eBonus) + 1) < bonus.getMinLandPercent())
	{
		return false;
	}
	// Make sure there are no bonuses of the same class (but a different type) nearby
	const CvArea* pArea = pPlot->area();
	int iRange0 = GC.getBonusClassInfo((BonusClassTypes)iBonusClass).getUniqueRange();
	if (iRange0 > 0)
	{
		iRange0 += (GC.getMap().getWorldSize() + 1) / 2;

		if (GC.getGame().isOption(GAMEOPTION_MAP_MORE_RESOURCES))
		{
			iRange0 /= 2;
		}
		for (int iDX = -(iRange0); iDX <= iRange0; iDX++)
		{
			for (int iDY = -(iRange0); iDY <= iRange0; iDY++)
			{
				if (iDX || iDY)
				{
					const CvPlot* pLoopPlot = plotXY(iX, iY, iDX, iDY);

					if (pLoopPlot != NULL && pLoopPlot->area() == pArea)
					{
						const BonusTypes eOtherBonus = pLoopPlot->getBonusType();
						if (eOtherBonus == eBonus || eOtherBonus != NO_BONUS && GC.getBonusInfo(eOtherBonus).getBonusClassType() == iBonusClass)
						{
							return false;
						}
					}
				}
			}
		}
	}
	else if (iRange0 < 0) { iRange0 = 0; }

	// Make sure there are none of the same bonus nearby:
	int iRange1 = bonus.getUniqueRange();

	if (iRange1 > 0)
	{
		iRange1 += GC.getMap().getWorldSize();

		if (GC.getGame().isOption(GAMEOPTION_MAP_MORE_RESOURCES))
		{
			iRange1 /= 2;
		}
		if (iRange1 > iRange0)
		{
			for (int iDX = -(iRange1); iDX <= iRange1; iDX++)
			{
				for (int iDY = -(iRange1); iDY <= iRange1; iDY++)
				{
					if (iDY < -iRange0 || iDY > iRange0 || iDX < -iRange0 || iDX > iRange0)
					{
						const CvPlot* pLoopPlot = plotXY(iX, iY, iDX, iDY);

						if (pLoopPlot != NULL && pLoopPlot->area() == pArea && pLoopPlot->getBonusType() == eBonus)
						{
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}


bool CvMapGenerator::canPlaceGoodyAt(ImprovementTypes eImprovement, int iX, int iY)
{
	PROFILE_FUNC();
	FAssertMsg(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");
	FAssertMsg(GC.getImprovementInfo(eImprovement).isGoody(), "ImprovementType eImprovement is expected to be a goody");

	if (GC.getGame().isOption(GAMEOPTION_MAP_NO_GOODY_HUTS))
	{
		return false;
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	if (!pPlot->canHaveImprovement(eImprovement, NO_TEAM))
	{
		return false;
	}

	{
		bool result = false;
		if (Cy::call_override<bool>(GC.getMap().getMapScript(), "canPlaceGoodyAt", Cy::Args() << pPlot, result))
		{
			return result;
		}
	}

	if (pPlot->getImprovementType() != NO_IMPROVEMENT || pPlot->getBonusType() != NO_BONUS)
	{
		return false;
	}

	if (pPlot->isImpassable() || pPlot->isPeak())
	{
		return false;
	}

	const int iUniqueRange = GC.getImprovementInfo(eImprovement).getGoodyUniqueRange();
	return algo::none_of(pPlot->rect(iUniqueRange, iUniqueRange),
		bind(&CvPlot::getImprovementType, _1) == eImprovement
	);
}


void CvMapGenerator::addGameElements()
{
	addRivers();
	gDLL->logMemState("CvMapGen after add rivers");

	addLakes();
	gDLL->logMemState("CvMapGen after add lakes");

	addFeatures();
	gDLL->logMemState("CvMapGen after add features");

	addBonuses();
	gDLL->logMemState("CvMapGen after add bonuses");

	addGoodies();
	gDLL->logMemState("CvMapGen after add goodies");

	// Call for Python to make map modifications after it's been generated
	afterGeneration();
}


void CvMapGenerator::addLakes()
{
	PROFILE("CvMapGenerator::addLakes");

	if (Cy::call_override(GC.getMap().getMapScript(), "addLakes"))
	{
		return;
	}
	gDLL->NiTextOut("Adding Lakes...");

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		gDLL->callUpdater();
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
		FAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

		if (!pLoopPlot->isWater() && !pLoopPlot->isCoastal() && !pLoopPlot->isRiver()
		&& GC.getGame().getMapRandNum(GC.getDefineINT("LAKE_PLOT_RAND"), "addLakes") == 0)
		{
			pLoopPlot->setPlotType(PLOT_OCEAN);
		}
	}
}

void CvMapGenerator::addRivers()
{
	PROFILE("CvMapGenerator::addRivers");

	if (Cy::call_override(GC.getMap().getMapScript(), "addRivers"))
	{
		return;
	}

	gDLL->NiTextOut("Adding Rivers...");

	for (int iPass = 0; iPass < 4; iPass++)
	{
		int iRiverSourceRange = iPass <= 1 || !GC.getGame().isOption(GAMEOPTION_MAP_MORE_RIVERS)
			? GC.getDefineINT("RIVER_SOURCE_MIN_RIVER_RANGE")
			: GC.getDefineINT("RIVER_SOURCE_MIN_RIVER_RANGE") / 2;

		int iSeaWaterRange = iPass <= 1 || !GC.getGame().isOption(GAMEOPTION_MAP_MORE_RIVERS)
			? GC.getDefineINT("RIVER_SOURCE_MIN_SEAWATER_RANGE")
			: GC.getDefineINT("RIVER_SOURCE_MIN_SEAWATER_RANGE") / 2;

		int iRand = 8;
		int iPPRE = GC.getDefineINT("PLOTS_PER_RIVER_EDGE");
		if (GC.getGame().isOption(GAMEOPTION_MAP_MORE_RIVERS))
		{
			iRand = GC.getDefineINT("RIVER_RAND_ON_MORE_RIVERS");
			iPPRE /= std::max(1, GC.getDefineINT("PLOTS_PER_RIVER_EDGE_DIVISOR"));
		}
		iPPRE = std::max(1, iPPRE);
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			gDLL->callUpdater();
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
			FAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

			if (pLoopPlot->isWater())
				continue;

			if(iPass == 0 && (pLoopPlot->isHills() || pLoopPlot->isPeak())
			|| iPass == 1 && !pLoopPlot->isCoastal() && GC.getGame().getMapRandNum(iRand, "addRivers") == 0
			|| iPass == 2 && (pLoopPlot->isHills() || pLoopPlot->isPeak()) && pLoopPlot->area()->getNumRiverEdges() < 1 + pLoopPlot->area()->getNumTiles() / iPPRE
			|| iPass == 3 && pLoopPlot->area()->getNumRiverEdges() < 1 + pLoopPlot->area()->getNumTiles() / iPPRE)
			{
				if (!GC.getMap().findWater(pLoopPlot, iRiverSourceRange, true) && !GC.getMap().findWater(pLoopPlot, iSeaWaterRange, false))
				{
					CvPlot* pStartPlot = pLoopPlot->getInlandCorner();
					if (pStartPlot != NULL)
					{
						doRiver(pStartPlot);
					}
				}
			}
		}
	}
}

// pStartPlot = the plot at whose SE corner the river is starting
//
void CvMapGenerator::doRiver(CvPlot *pStartPlot, CardinalDirectionTypes eLastCardinalDirection, CardinalDirectionTypes eOriginalCardinalDirection, int iThisRiverID)
{
	if (iThisRiverID == -1)
	{
		iThisRiverID = GC.getMap().getNextRiverID();
		GC.getMap().incrementNextRiverID();
	}

	{
		const int iOtherRiverID = pStartPlot->getRiverID();
		if (iOtherRiverID != -1 && iOtherRiverID != iThisRiverID)
		{
			return; // Another river already exists here; can't branch off of an existing river!
		}
	}

	CvPlot *pRiverPlot = NULL;

	CardinalDirectionTypes eBestCardinalDirection = NO_CARDINALDIRECTION;

	if (eLastCardinalDirection==CARDINALDIRECTION_NORTH)
	{
		pRiverPlot = pStartPlot;
		if (pRiverPlot == NULL)
		{
			return;
		}
		const CvPlot* pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_EAST);

		if ((pAdjacentPlot == NULL) || pRiverPlot->isWOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}
		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setWOfRiver(true, eLastCardinalDirection);
		pRiverPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_NORTH);
	}
	else if (eLastCardinalDirection==CARDINALDIRECTION_EAST)
	{
		pRiverPlot = plotCardinalDirection(pStartPlot->getX(), pStartPlot->getY(), CARDINALDIRECTION_EAST);
		if (pRiverPlot == NULL)
		{
			return;
		}
		const CvPlot* pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_SOUTH);

		if ((pAdjacentPlot == NULL) || pRiverPlot->isNOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}
		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setNOfRiver(true, eLastCardinalDirection);
	}
	else if (eLastCardinalDirection==CARDINALDIRECTION_SOUTH)
	{
		pRiverPlot = plotCardinalDirection(pStartPlot->getX(), pStartPlot->getY(), CARDINALDIRECTION_SOUTH);
		if (pRiverPlot == NULL)
		{
			return;
		}

		const CvPlot* pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_EAST);
		if ((pAdjacentPlot == NULL) || pRiverPlot->isWOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}
		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setWOfRiver(true, eLastCardinalDirection);
	}
	else if (eLastCardinalDirection==CARDINALDIRECTION_WEST)
	{
		pRiverPlot = pStartPlot;
		if (pRiverPlot == NULL)
		{
			return;
		}
		const CvPlot* pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_SOUTH);

		if ((pAdjacentPlot == NULL) || pRiverPlot->isNOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}
		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setNOfRiver(true, eLastCardinalDirection);
		pRiverPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_WEST);
	}
	else
	{
		// River is starting here, set the direction in the next step
		pRiverPlot = pStartPlot;

		CardinalDirectionTypes result;
		if (Cy::call_override(GC.getMap().getMapScript(), "getRiverStartCardinalDirection", Cy::Args() << pRiverPlot, result))
		{
			eBestCardinalDirection = result;
		}
	}

	if (pRiverPlot == NULL)
	{
		return; // The river has flowed off the edge of the map. All is well.
	}
	if (pRiverPlot->hasCoastAtSECorner())
	{
		return; // The river has flowed into the ocean. All is well.
	}

	if (eBestCardinalDirection == NO_CARDINALDIRECTION)
	{
		int iBestValue = MAX_INT;

		for (int iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; iI++)
		{
			const int iOppositeDir = ((iI + 2) % NUM_CARDINALDIRECTION_TYPES);

			if (iOppositeDir != eOriginalCardinalDirection && iOppositeDir != eLastCardinalDirection)
			{
				CvPlot* pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), (CardinalDirectionTypes)iI);
				if (pAdjacentPlot != NULL)
				{
					const int iValue = getRiverValueAtPlot(pAdjacentPlot);
					if (iValue < iBestValue)
					{
						iBestValue = iValue;
						eBestCardinalDirection = (CardinalDirectionTypes)iI;
					}
				}
			}
		}
	}

	if (eBestCardinalDirection != NO_CARDINALDIRECTION)
	{
		if  (eOriginalCardinalDirection	== NO_CARDINALDIRECTION)
		{
			eOriginalCardinalDirection = eBestCardinalDirection;
		}
		doRiver(pRiverPlot, eBestCardinalDirection, eOriginalCardinalDirection, iThisRiverID);
	}
}
//Note from Blake:
//Iustus wrote this function, it ensures that a new river actually
//creates fresh water on the passed plot. Quite useful really
//Altouh I veto'd it's use since I like that you don't always
//get fresh water starts.
// pFreshWaterPlot = the plot we want to give a fresh water river
//
bool CvMapGenerator::addRiver(CvPlot* pFreshWaterPlot)
{
	FAssertMsg(pFreshWaterPlot != NULL, "NULL plot parameter");

	// cannot have a river flow next to water
	if (pFreshWaterPlot->isWater())
	{
		return false;
	}

	// if it already has a fresh water river, then success! we done
	if (pFreshWaterPlot->isRiver())
	{
		return true;
	}

	// randomize the order of directions
	int aiShuffle[NUM_CARDINALDIRECTION_TYPES];
	shuffleArray(aiShuffle, NUM_CARDINALDIRECTION_TYPES, GC.getGame().getMapRand());

	// make two passes, once for each flow direction of the river
	const int iNWFlowPass = GC.getGame().getMapRandNum(2, "addRiver");
	for (int iPass = 0; iPass <= 1; iPass++)
	{
		// try placing a river edge in each direction, in random order
		foreach_(const int iDirection, aiShuffle)
		{
			CardinalDirectionTypes eRiverDirection = NO_CARDINALDIRECTION;
			CvPlot *pRiverPlot = NULL;

			switch (iDirection)
			{
			case CARDINALDIRECTION_NORTH:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = plotDirection(pFreshWaterPlot, DIRECTION_NORTH);
					eRiverDirection = CARDINALDIRECTION_WEST;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot, DIRECTION_NORTHWEST);
					eRiverDirection = CARDINALDIRECTION_EAST;
				}
				break;

			case CARDINALDIRECTION_EAST:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = pFreshWaterPlot;
					eRiverDirection = CARDINALDIRECTION_NORTH;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot, DIRECTION_NORTH);
					eRiverDirection = CARDINALDIRECTION_SOUTH;
				}
				break;

			case CARDINALDIRECTION_SOUTH:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = pFreshWaterPlot;
					eRiverDirection = CARDINALDIRECTION_WEST;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot, DIRECTION_WEST);
					eRiverDirection = CARDINALDIRECTION_EAST;
				}
				break;

			case CARDINALDIRECTION_WEST:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = plotDirection(pFreshWaterPlot, DIRECTION_WEST);
					eRiverDirection = CARDINALDIRECTION_NORTH;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot, DIRECTION_NORTHWEST);
					eRiverDirection = CARDINALDIRECTION_SOUTH;
				}
				break;

			default:
				FErrorMsg("invalid cardinal direction");
			}

			if (pRiverPlot != NULL && !pRiverPlot->hasCoastAtSECorner())
			{
				// try to make the river
				doRiver(pRiverPlot, eRiverDirection, eRiverDirection, -1);

				// if it succeeded, then we will be a river now!
				if (pFreshWaterPlot->isRiver())
				{
					return true;
				}
			}
		}
	}

	return false;
}


void CvMapGenerator::addFeatures()
{
	PROFILE("CvMapGenerator::addFeatures");

	if (Cy::call_override(GC.getMap().getMapScript(), "addFeatures"))
	{
		return;
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(iI);
		FAssert(pPlot != NULL);

		for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
		{
			if (pPlot->canHaveFeature((FeatureTypes)iJ))
			{
				if (GC.getGame().getMapRandNum(10000, "addFeaturesAtPlot") < GC.getFeatureInfo((FeatureTypes)iJ).getAppearanceProbability())
				{
					pPlot->setFeatureType((FeatureTypes)iJ);
				}
			}
		}
	}
}

void CvMapGenerator::addBonuses()
{
	PROFILE("CvMapGenerator::addBonuses");
	gDLL->NiTextOut("Adding Bonuses...");

	if (Cy::call_override(GC.getMap().getMapScript(), "addBonuses"))
	{
		return; // Python override
	}
	const int iNumMapBonuses = GC.getNumMapBonuses();
	for (int iOrder = 0; iOrder < 16; iOrder++) // 16 orders are more than enough
	{
		for (int iI = 0; iI < iNumMapBonuses; iI++)
		{
			const BonusTypes eBonus = GC.getMapBonus(iI);
			gDLL->callUpdater();
			if (GC.getBonusInfo(eBonus).getPlacementOrder() == iOrder
			&& !Cy::call_override(GC.getMap().getMapScript(), "addBonusType", Cy::Args() << static_cast<int>(eBonus)))
			{
				if (GC.getBonusInfo(eBonus).isOneArea())
				{
					addUniqueBonusType(eBonus);
				}
				else
				{
					addNonUniqueBonusType(eBonus);
				}
			}
		}
	}
}

void CvMapGenerator::addUniqueBonusType(BonusTypes eBonus)
{
	PROFILE_EXTRA_FUNC();
	int iBonusCount = calculateNumBonusesToAdd(eBonus);
	if (iBonusCount == 0)
	{
		return;
	}
	const CvBonusInfo& bonus = GC.getBonusInfo(eBonus);
	FAssertMsg(bonus.isOneArea(), "addUniqueBonusType called with non-unique bonus type");

	const int iWorldSize = (int)GC.getMap().getWorldSize();
	int iGroupRange;
	int iGroupRand;
	int iMaxCluster;
	setBonusClusterValues(bonus, iWorldSize, iGroupRange, iGroupRand, iMaxCluster);
	if (iMaxCluster > iBonusCount)
		iMaxCluster = iBonusCount;

	int* piAreaTried = new int[GC.getMap().getNumAreas()];

	for (int iI = 0; iI < GC.getMap().getNumAreas(); iI++)
	{
		piAreaTried[iI] = FFreeList::INVALID_INDEX;
	}
	const bool bIgnoreLatitude = GC.getGame().pythonIsBonusIgnoreLatitudes();

	while (true)
	{
		int iBestValue = 0;
		CvArea *pBestArea = NULL;

		foreach_(CvArea * pLoopArea, GC.getMap().areas())
		{
			bool bTried = false;

			for (int iI = 0; iI < GC.getMap().getNumAreas(); iI++)
			{
				if (pLoopArea->getID() == piAreaTried[iI])
				{
					bTried = true;
					break;
				}
			}

			if (!bTried)
			{
				int iNumUniqueBonusesOnArea = pLoopArea->countNumUniqueBonusTypes() + 1; // number of unique bonuses starting on the area, plus this one
				int iNumTiles = pLoopArea->getNumTiles();
				int iValue = iNumTiles / iNumUniqueBonusesOnArea;

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestArea = pLoopArea;
				}
			}
		}

		if (pBestArea == NULL)
		{
			break; // can't place bonus on any area
		}
		for (int iI = 0; iI < GC.getMap().getNumAreas(); iI++)
		{
			if (piAreaTried[iI] == FFreeList::INVALID_INDEX)
			{
				piAreaTried[iI] = pBestArea->getID();
				break;
			}
		}
		// Place the bonuses:
		placeBonusWithCluster(eBonus, iGroupRange, iGroupRand, iMaxCluster, bIgnoreLatitude, iBonusCount, pBestArea);
		if (iBonusCount == 0) break;
	}
	SAFE_DELETE_ARRAY(piAreaTried);
}

void CvMapGenerator::addNonUniqueBonusType(BonusTypes eBonus)
{
	int iBonusCount = calculateNumBonusesToAdd(eBonus);
	if (iBonusCount == 0)
	{
		return;
	}
	const int iWorldSize = (int)GC.getMap().getWorldSize();
	int iGroupRange;
	int iGroupRand;
	int iMaxCluster;
	setBonusClusterValues(GC.getBonusInfo(eBonus), iWorldSize, iGroupRange, iGroupRand, iMaxCluster);
	if (iMaxCluster > iBonusCount)
		iMaxCluster = iBonusCount;
	placeBonusWithCluster(eBonus, iGroupRange, iGroupRand, iMaxCluster, GC.getGame().pythonIsBonusIgnoreLatitudes(), iBonusCount);
}

void CvMapGenerator::setBonusClusterValues(const CvBonusInfo& bonus, const int iWorldSize, int& iGroupRange, int& iGroupRand, int& iMaxCluster)
{
	iGroupRange = bonus.getGroupRange();
	iGroupRand = bonus.getGroupRand();
	if (iGroupRange > 0)
	{
		if (iGroupRand < 1)
		{
			iGroupRange = 0;
		}
		else if (iWorldSize / 3 > 0) // Scale by worldsize
		{
			iGroupRange += iWorldSize / 3; // increase range
			iGroupRand -= iGroupRand * iGroupRange / (iGroupRange + 4); // decrease chance
			if (iGroupRand < 0)
				iGroupRand = 1;
		}
	}
	// Max clustering
	iMaxCluster = iGroupRange + (iWorldSize + 1) / 2;
}

void CvMapGenerator::placeBonusWithCluster(const BonusTypes eBonus, const int iGroupRange, const int iGroupRand, const int iMaxCluster, const bool bIgnoreLatitude, int& iBonusCount, const CvArea *pBestArea)
{
	PROFILE_EXTRA_FUNC();
	int* piShuffle = shuffle(GC.getMap().numPlots(), GC.getGame().getMapRand());

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(piShuffle[iI]);
		FAssertMsg(pPlot != NULL, "placeBonusWithCluster(): pPlot is null");

		if ((pBestArea == NULL || pBestArea == pPlot->area())
		&& canPlaceBonusAt(eBonus, pPlot->getX(), pPlot->getY(), bIgnoreLatitude))
		{
			pPlot->setBonusType(eBonus);
			iBonusCount--;
			int iCluster = 0;
			bool bBreakCluster = false;

			for (int iDX = -iGroupRange; iDX <= iGroupRange; iDX++)
			{
				if (iBonusCount == 0 || bBreakCluster) break;

				for (int iDY = -iGroupRange; iDY <= iGroupRange; iDY++)
				{
					if (iBonusCount == 0) break;

					CvPlot* pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);

					if (pLoopPlot != NULL && (pBestArea == NULL || pLoopPlot->area() == pBestArea)
					&& pLoopPlot->canHaveBonus(eBonus, bIgnoreLatitude)
					&& GC.getGame().getMapRandNum(100, "0-99") < iGroupRand)
					{
						pLoopPlot->setBonusType(eBonus);
						iBonusCount--;
						if (++iCluster == iMaxCluster)
						{
							bBreakCluster = true;
							break;
						}
					}
				}
			}
			if (iBonusCount == 0) break;
		}
	}
	SAFE_DELETE_ARRAY(piShuffle);
}


void CvMapGenerator::addGoodies()
{
	PROFILE("CvMapGenerator::addGoodies");

	if (Cy::call_override(GC.getMap().getMapScript(), "addGoodies"))
	{
		return; // Python override
	}

	gDLL->NiTextOut("Adding Goodies...");

	if (GC.getEraInfo(GC.getGame().getStartEra()).isNoGoodies())
	{
		return;
	}

	int iNumPlots = GC.getMap().numPlots();
	int* piShuffle = shuffle(iNumPlots, GC.getGame().getMapRand());

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		if (GC.getImprovementInfo((ImprovementTypes)iI).isGoody() && GC.getImprovementInfo((ImprovementTypes)iI).getTilesPerGoody() > 0)
		{
			for (int iJ = 0; iJ < iNumPlots; iJ++)
			{
				gDLL->callUpdater();
				CvPlot *pPlot = GC.getMap().plotByIndex(piShuffle[iJ]);
				FAssertMsg(pPlot, "pPlot is expected not to be NULL");
				if (!(pPlot->isWater()))
				{
					CvArea *pArea = GC.getMap().getArea(pPlot->getArea());
					FAssertMsg(pArea, "pArea is expected not to be NULL");
					if (pArea->getNumImprovements((ImprovementTypes)iI) < ((pArea->getNumTiles() + (GC.getImprovementInfo((ImprovementTypes)iI).getTilesPerGoody() / 2)) / GC.getImprovementInfo((ImprovementTypes) iI).getTilesPerGoody()))
					{
						if (canPlaceGoodyAt(((ImprovementTypes)iI), pPlot->getX(), pPlot->getY()))
						{
							pPlot->setImprovementType((ImprovementTypes)iI);
						}
					}
				}
			}
		}
	}

	SAFE_DELETE_ARRAY(piShuffle);
}


void CvMapGenerator::eraseRivers()
{
	PROFILE_EXTRA_FUNC();
	int i;

	for (i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		if (pPlot->isNOfRiver())
		{
			pPlot->setNOfRiver(false, NO_CARDINALDIRECTION);
		}
		if (pPlot->isWOfRiver())
		{
			pPlot->setWOfRiver(false, NO_CARDINALDIRECTION);
		}
	}
}

void CvMapGenerator::eraseFeatures()
{
	PROFILE_EXTRA_FUNC();
	int i;

	for (i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		pPlot->setFeatureType(NO_FEATURE);
	}
}

void CvMapGenerator::eraseBonuses()
{
	PROFILE_EXTRA_FUNC();
	int i;

	for (i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		pPlot->setBonusType(NO_BONUS);
	}
}

void CvMapGenerator::eraseGoodies()
{
	PROFILE_EXTRA_FUNC();
	int i;

	for (i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		if (pPlot->isGoody())
		{
			pPlot->removeGoody();
		}
	}
}

//------------------------------------------------------------------------------------------------
//
// Call python function to generate random map
// It will call applyMapData when it's done
//

void CvMapGenerator::generateRandomMap()
{
	PROFILE("generateRandomMap()");

	Cy::call_optional(GC.getMap().getMapScript(), "beforeGeneration");

	if (Cy::call_override(GC.getMap().getMapScript(), "generateRandomMap"))
	{
		return; // Python override
	}

	gDLL->NiTextOut(CvString::format("Generating Random Map %S, %S...", gDLL->getMapScriptName().GetCString(), GC.getWorldInfo(GC.getMap().getWorldSize()).getDescription()).c_str());

	generatePlotTypes();
	generateTerrain();
}

void CvMapGenerator::generatePlotTypes()
{
	std::vector<int> plotTypes;
	if (Cy::call_override(GC.getMap().getMapScript(), "generatePlotTypes", plotTypes))
	{
		FAssertMsg((int)plotTypes.size() == GC.getMap().numPlots(), "python generatePlotTypes() should return list with length numPlots");
	}
	else
	{
		plotTypes.resize(GC.getMap().numPlots(), PLOT_LAND);
	}

	setPlotTypes(plotTypes);
}

void CvMapGenerator::generateTerrain()
{
	PROFILE("generateTerrain()");

	std::vector<int> terrainMapOut;
	if (Cy::call_override(GC.getMap().getMapScript(), "generateTerrainTypes", terrainMapOut))
	{
		 // Python override
		int iNumPlots = GC.getMap().numPlots();
		FAssertMsg((int)terrainMapOut.size() == iNumPlots, "python generateTerrain() should return a list with length CyMap().getNumPoints()");
		// Generate terrain for each plot
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			gDLL->callUpdater();
			GC.getMap().plotByIndex(iI)->setTerrainType(((TerrainTypes)(terrainMapOut[iI])), false, false);
		}
	}
}

// Allows for user-defined Python Actions for map generation after it's already been created
void CvMapGenerator::afterGeneration()
{
	PROFILE("CvMapGenerator::afterGeneration");

	Cy::call_optional(GC.getMap().getMapScript(), "afterGeneration");
}

void CvMapGenerator::setPlotTypes(const std::vector<int>& plotTypes)
{
	PROFILE_EXTRA_FUNC();
	const int iNumPlots = GC.getMap().numPlots();

	for (int iI = 0; iI < iNumPlots; iI++)
	{
		gDLL->callUpdater();
		GC.getMap().plotByIndex(iI)->setPlotType((PlotTypes)plotTypes[iI], false, false);
	}

	GC.getMap().recalculateAreas();

	for (int iI = 0; iI < iNumPlots; iI++)
	{
		gDLL->callUpdater();
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->isWater())
		{
			if (pLoopPlot->isAdjacentToLand())
			{
				pLoopPlot->setTerrainType(((TerrainTypes)(GC.getDefineINT("SHALLOW_WATER_TERRAIN"))), false, false);
			}
			else
			{
				pLoopPlot->setTerrainType(((TerrainTypes)(GC.getDefineINT("DEEP_WATER_TERRAIN"))), false, false);
			}
		}
	}
}

// Protected functions:

int CvMapGenerator::getRiverValueAtPlot(CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	FAssert(pPlot != NULL);

	long result = 0;
	if (Cy::call_override(GC.getMap().getMapScript(), "getRiverAltitude", Cy::Args() << pPlot, result))
	{
		FAssertMsg(result >= 0, "python getRiverAltitude() must return >= 0");
		if (result >= 0)
		{
			return result;
		}
	}

	int iSum = result + (NUM_PLOT_TYPES - pPlot->getPlotType()) * 20;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		const CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			iSum += (NUM_PLOT_TYPES - pAdjacentPlot->getPlotType());
		}
		else
		{
			iSum += (NUM_PLOT_TYPES * 10);
		}
	}

	CvRandom riverRand;
	riverRand.init((pPlot->getX() * 43251267) + (pPlot->getY() * 8273903));

	return iSum + riverRand.get(10, "River Rand");
}

int CvMapGenerator::calculateNumBonusesToAdd(BonusTypes eBonusType)
{
	PROFILE_EXTRA_FUNC();
	const CvBonusInfo& pBonusInfo = GC.getBonusInfo(eBonusType);

	int iBaseCount =
	(
		pBonusInfo.getConstAppearance() +
		GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance1(), "calculateNumBonusesToAdd-1") +
		GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance2(), "calculateNumBonusesToAdd-2") +
		GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance3(), "calculateNumBonusesToAdd-3") +
		GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance4(), "calculateNumBonusesToAdd-4")
	);

	iBaseCount += GC.getGame().countCivPlayersAlive() * pBonusInfo.getPercentPerPlayer(); // Toffer: Should imo be removed.

	// Calculate iNumPossible, the number of plots that are eligible to have this bonus:
	bool bIgnoreLatitude = GC.getGame().pythonIsBonusIgnoreLatitudes();

	if (pBonusInfo.getTilesPer() > 0)
	{
		int iNumPossible = 0;
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			if (GC.getMap().plotByIndex(iI)->canHaveBonus(eBonusType, bIgnoreLatitude))
			{
				iNumPossible++;
			}
		}
		iBaseCount += iNumPossible * 1000 / (pBonusInfo.getTilesPer() * (GC.getMap().getWorldSize() + 7)); // Density scaled by map size, less dense on large maps.
	}

	if (GC.getGame().isOption(GAMEOPTION_MAP_MORE_RESOURCES))
	{
		iBaseCount *= (GC.getDefineINT("BONUS_COUNT_PERCENTAGE_MODIFIER_ON_MORE_RESOURCES") + 100);
		iBaseCount /= 100;
	}
	iBaseCount /= 100;

	if (iBaseCount < 1) { return 1; }
	return iBaseCount;
}