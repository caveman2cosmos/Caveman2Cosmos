#include "CvGameCoreDLL.h"
#include "CvMapGenerator.h"
#include "CvFractal.h"

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


CvMapGenerator::CvMapGenerator() : m_bUseDefaultMapScript(true)
{
}


CvMapGenerator::~CvMapGenerator()
{
}


bool CvMapGenerator::canPlaceBonusAt(BonusTypes eBonus, int iX, int iY, bool bIgnoreLatitude)
{
	PROFILE_FUNC();

	CvArea* pArea;
	CvPlot* pPlot;
	CvPlot* pLoopPlot;
	int iDX, iDY;
	int iI;

	pPlot = GC.getMap().plot(iX, iY);
	pArea = pPlot->area();

	if (!(pPlot->canHaveBonus(eBonus, bIgnoreLatitude)))
	{
		return false;
	}

	{
		bool result = false;
		if (Cy::call_override<bool>(gDLL->getPythonIFace()->getMapScriptModule(), "canPlaceBonusAt", Cy::Args() << pPlot, result))
		{
			return result;
		}
	}

	if (!GC.getGame().isOption(GAMEOPTION_MORE_RESOURCES))
	{
		for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pLoopPlot = plotDirection(iX, iY, ((DirectionTypes)iI));

			if (pLoopPlot != NULL)
			{
				if ((pLoopPlot->getBonusType() != NO_BONUS) && (pLoopPlot->getBonusType() != eBonus))
				{
					return false;
				}
			}
		}
	}

	const CvBonusInfo& pInfo = GC.getBonusInfo(eBonus);
	int iBonusClassType = pInfo.getBonusClassType();
	const CvBonusClassInfo& pClassInfo = GC.getBonusClassInfo((BonusClassTypes) iBonusClassType);

	if (pPlot->isWater()
	&& GC.getMap().getNumBonusesOnLand(eBonus) * 100 / (GC.getMap().getNumBonuses(eBonus) + 1) < pInfo.getMinLandPercent())
	{
		return false;
	}
	// Make sure there are no bonuses of the same class (but a different type) nearby:
	int iRange0 = pClassInfo.getUniqueRange();
	if (iRange0 > 0)
	{
		iRange0 += (GC.getMap().getWorldSize() + 1) / 2;

		if (GC.getGame().isOption(GAMEOPTION_MORE_RESOURCES))
		{
			iRange0 /= 2;
		}
		for (iDX = -(iRange0); iDX <= iRange0; iDX++)
		{
			for (iDY = -(iRange0); iDY <= iRange0; iDY++)
			{
				if (iDX || iDY)
				{
					pLoopPlot = plotXY(iX, iY, iDX, iDY);

					if (pLoopPlot != NULL && pLoopPlot->area() == pArea)
					{
						BonusTypes eOtherBonus = pLoopPlot->getBonusType();
						if (eOtherBonus == eBonus || eOtherBonus != NO_BONUS && GC.getBonusInfo(eOtherBonus).getBonusClassType() == pInfo.getBonusClassType())
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
	int iRange1 = pInfo.getUniqueRange();

	if (iRange1 > 0)
	{
		iRange1 += GC.getMap().getWorldSize();

		if (GC.getGame().isOption(GAMEOPTION_MORE_RESOURCES))
		{
			iRange1 /= 2;
		}
		if (iRange1 > iRange0)
		{
			for (iDX = -(iRange1); iDX <= iRange1; iDX++)
			{
				for (iDY = -(iRange1); iDY <= iRange1; iDY++)
				{
					if (iDY < -iRange0 || iDY > iRange0 || iDX < -iRange0 || iDX > iRange0)
					{
						pLoopPlot = plotXY(iX, iY, iDX, iDY);

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

	CvPlot* pPlot;

	FAssertMsg(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");
	FAssertMsg(GC.getImprovementInfo(eImprovement).isGoody(), "ImprovementType eImprovement is expected to be a goody");

	if (GC.getGame().isOption(GAMEOPTION_NO_GOODY_HUTS))
	{
		return false;
	}

	pPlot = GC.getMap().plot(iX, iY);

	if (!(pPlot->canHaveImprovement(eImprovement, NO_TEAM))) 
	{
		return false;
	}

	{
		bool result = false;
		if (Cy::call_override<bool>(gDLL->getPythonIFace()->getMapScriptModule(), "canPlaceGoodyAt", Cy::Args() << pPlot, result))
		{
			return result;
		}
	}

	if (pPlot->getImprovementType() != NO_IMPROVEMENT) 
	{
		return false;
	}

	if (pPlot->getBonusType() != NO_BONUS)
	{
		return false;
	}

	if (pPlot->isImpassable()) 
	{
		return false;
	}

/************************************************************************************************/
/* Afforess	Mountains Start		 08/03/09                                           		 */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (pPlot->isPeak()) 
	{
		return false;
	}
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                             */
/************************************************************************************************/

	int iUniqueRange = GC.getImprovementInfo(eImprovement).getGoodyUniqueRange();
	for (int iDX = -iUniqueRange; iDX <= iUniqueRange; iDX++) 
	{
		for (int iDY = -iUniqueRange; iDY <= iUniqueRange; iDY++)
		{
			CvPlot *pLoopPlot	= plotXY(iX, iY, iDX, iDY);
			if (pLoopPlot != NULL && pLoopPlot->getImprovementType() == eImprovement) 
			{
				return false;
			}
		}
	}

	return true;
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

	if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "addLakes"))
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

	if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "addRivers"))
	{
		return;
	}

	gDLL->NiTextOut("Adding Rivers...");

	for (int iPass = 0; iPass < 4; iPass++)
	{
		int iRiverSourceRange = iPass <= 1 || !GC.getGame().isOption(GAMEOPTION_MORE_RIVERS)
			? GC.getDefineINT("RIVER_SOURCE_MIN_RIVER_RANGE")
			: GC.getDefineINT("RIVER_SOURCE_MIN_RIVER_RANGE") / 2;

		int iSeaWaterRange = iPass <= 1 || !GC.getGame().isOption(GAMEOPTION_MORE_RIVERS)
			? GC.getDefineINT("RIVER_SOURCE_MIN_SEAWATER_RANGE")
			: GC.getDefineINT("RIVER_SOURCE_MIN_SEAWATER_RANGE") / 2;

		int iRand = 8;
		int iPPRE = GC.getDefineINT("PLOTS_PER_RIVER_EDGE");
		if (GC.getGame().isOption(GAMEOPTION_MORE_RIVERS))
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

	int iOtherRiverID = pStartPlot->getRiverID();
	if (iOtherRiverID != -1 && iOtherRiverID != iThisRiverID)
	{
		return; // Another river already exists here; can't branch off of an existing river!
	}

	CvPlot *pRiverPlot = NULL;
	CvPlot *pAdjacentPlot = NULL;

	CardinalDirectionTypes eBestCardinalDirection = NO_CARDINALDIRECTION;

	if (eLastCardinalDirection==CARDINALDIRECTION_NORTH) 
	{
		pRiverPlot = pStartPlot;
		if (pRiverPlot == NULL)
		{
			return;
		}
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_EAST);
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
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_SOUTH);
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
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_EAST);
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
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_SOUTH);
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

		//FAssertMsg(false, "Illegal direction type"); 
		// River is starting here, set the direction in the next step
		pRiverPlot = pStartPlot;

		CardinalDirectionTypes result;
		if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "getRiverStartCardinalDirection", Cy::Args() << pRiverPlot, result))
		{
			eBestCardinalDirection = result;
		}
	}

	if (pRiverPlot == NULL)
	{
		return; // The river has flowed off the edge of the map. All is well.
	}
	else if (pRiverPlot->hasCoastAtSECorner())
	{
		return; // The river has flowed into the ocean. All is well.
	}

	if (eBestCardinalDirection == NO_CARDINALDIRECTION)
	{
		int iBestValue = MAX_INT;

		for (int iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; iI++)
		{
			if (getOppositeCardinalDirection((CardinalDirectionTypes)iI) != eOriginalCardinalDirection)
			{
				if (getOppositeCardinalDirection((CardinalDirectionTypes)iI) != eLastCardinalDirection)
				{
					CvPlot* pAdjacentPlot;
					pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), ((CardinalDirectionTypes)iI));
					if (pAdjacentPlot != NULL)
					{
						int iValue = getRiverValueAtPlot(pAdjacentPlot);
						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							eBestCardinalDirection = (CardinalDirectionTypes)iI;
						}
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
	
	bool bSuccess = false;

	// randomize the order of directions
	int aiShuffle[NUM_CARDINALDIRECTION_TYPES];
	shuffleArray(aiShuffle, NUM_CARDINALDIRECTION_TYPES, GC.getGame().getMapRand());

	// make two passes, once for each flow direction of the river
	int iNWFlowPass = GC.getGame().getMapRandNum(2, "addRiver");
	for (int iPass = 0; !bSuccess && iPass <= 1; iPass++)
	{
		// try placing a river edge in each direction, in random order
		for (int iI = 0; !bSuccess && iI < NUM_CARDINALDIRECTION_TYPES; iI++)
		{
			CardinalDirectionTypes eRiverDirection = NO_CARDINALDIRECTION;
			CvPlot *pRiverPlot = NULL;
			
			switch (aiShuffle[iI])
			{
			case CARDINALDIRECTION_NORTH:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_NORTH);
					eRiverDirection = CARDINALDIRECTION_WEST;
				}
				else 
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_NORTHWEST);
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
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_NORTH);
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
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_WEST);
					eRiverDirection = CARDINALDIRECTION_EAST;
				}
				break;

			case CARDINALDIRECTION_WEST:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_WEST);
					eRiverDirection = CARDINALDIRECTION_NORTH;
				}
				else 
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_NORTHWEST);
					eRiverDirection = CARDINALDIRECTION_SOUTH;
				}
				break;

			default:
				FAssertMsg(false, "invalid cardinal direction");
			}
			
			if (pRiverPlot != NULL && !pRiverPlot->hasCoastAtSECorner())
			{
				// try to make the river
				doRiver(pRiverPlot, eRiverDirection, eRiverDirection, -1);

				// if it succeeded, then we will be a river now!
				if (pFreshWaterPlot->isRiver())
				{
					bSuccess = true;
				}
			}
		}
	}

	return bSuccess;
}


void CvMapGenerator::addFeatures()
{
	PROFILE("CvMapGenerator::addFeatures");

	if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "addFeatures"))
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

	if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "addBonuses"))
	{
		return; // Python override
	}

	for (int iOrder = 0; iOrder < GC.getNumBonusInfos(); iOrder++)
	{
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			gDLL->callUpdater();
			if (GC.getBonusInfo((BonusTypes)iI).getPlacementOrder() == iOrder)
			{
				if (!Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "addBonusType", Cy::Args() << iI))
				{
					if (GC.getBonusInfo((BonusTypes)iI).isOneArea())
					{
						addUniqueBonusType((BonusTypes)iI);
					}
					else
					{
						addNonUniqueBonusType((BonusTypes)iI);
					}
				}
			}
		}
	}
}

void CvMapGenerator::addUniqueBonusType(BonusTypes eBonus)
{
	int iBonusCount = calculateNumBonusesToAdd(eBonus);
	if (iBonusCount == 0)
	{
		return;
	}

	int* piAreaTried = new int[GC.getMap().getNumAreas()];

	for (int iI = 0; iI < GC.getMap().getNumAreas(); iI++)
	{
		piAreaTried[iI] = FFreeList::INVALID_INDEX;
	}
	const CvBonusInfo& pBonusInfo = GC.getBonusInfo(eBonus);

	const bool bIgnoreLatitude = GC.getGame().pythonIsBonusIgnoreLatitudes();

	int iGroupRange = pBonusInfo.getGroupRange();
	const int iGroupRand = pBonusInfo.getGroupRand();
	if (iGroupRange > 0)
	{
		if (iGroupRand < 1) iGroupRange = 0;
		else iGroupRange += GC.getMap().getWorldSize() / 3;
	}
	FAssertMsg(pBonusInfo.isOneArea(), "addUniqueBonusType called with non-unique bonus type");

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

		int* piShuffle = shuffle(GC.getMap().numPlots(), GC.getGame().getMapRand());

		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			CvPlot* pPlot = GC.getMap().plotByIndex(piShuffle[iI]);
			FAssertMsg(pPlot != NULL, "addUniqueBonusType(): pPlot is null");

			if (pBestArea == pPlot->area() && canPlaceBonusAt(eBonus, pPlot->getX(), pPlot->getY(), bIgnoreLatitude))
			{
				pPlot->setBonusType(eBonus);
				iBonusCount--;

				for (int iDX = -iGroupRange; iDX <= iGroupRange; iDX++)
				{
					if (iBonusCount == 0) break;

					for (int iDY = -iGroupRange; iDY <= iGroupRange; iDY++)
					{
						if (iBonusCount == 0) break;

						CvPlot* pLoopPlot	= plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);

						if (pLoopPlot != NULL && (pLoopPlot->area() == pBestArea)
						&& pLoopPlot->canHaveBonus(eBonus, bIgnoreLatitude)
						&& GC.getGame().getMapRandNum(100, "addUniqueBonusType") < iGroupRand)
						{
							pLoopPlot->setBonusType(eBonus);
							iBonusCount--;
						}
					}
				}
				if (iBonusCount == 0) break;
			}
		}
		SAFE_DELETE_ARRAY(piShuffle);
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
	const CvBonusInfo& pBonusInfo = GC.getBonusInfo(eBonus);

	int iGroupRange = pBonusInfo.getGroupRange();
	const int iGroupRand = pBonusInfo.getGroupRand();
	if (iGroupRange > 0)
	{
		if (iGroupRand < 1) iGroupRange = 0;
		else iGroupRange += GC.getMap().getWorldSize() / 3;
	}
	int* piShuffle = shuffle(GC.getMap().numPlots(), GC.getGame().getMapRand());

	const bool bIgnoreLatitude = GC.getGame().pythonIsBonusIgnoreLatitudes();

	CvPlot* pPlot = NULL;
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pPlot = GC.getMap().plotByIndex(piShuffle[iI]);
		if (canPlaceBonusAt(eBonus, pPlot->getX(), pPlot->getY(), bIgnoreLatitude))
		{
			pPlot->setBonusType(eBonus);
			iBonusCount--;

			for (int iDX = -iGroupRange; iDX <= iGroupRange; iDX++)
			{
				if (iBonusCount == 0) break;

				for (int iDY = -iGroupRange; iDY <= iGroupRange; iDY++)
				{
					if (iBonusCount == 0) break;

					CvPlot* pLoopPlot	= plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);

					if (pLoopPlot != NULL
					&& pLoopPlot->canHaveBonus(eBonus, bIgnoreLatitude)
					&& GC.getGame().getMapRandNum(100, "addNonUniqueBonusType") < iGroupRand)
					{
						pLoopPlot->setBonusType(eBonus);
						iBonusCount--;
					}
				}
			}
			FAssertMsg(iBonusCount >= 0, "iBonusCount must be >= 0");

			if (iBonusCount == 0) break;
		}
	}
	SAFE_DELETE_ARRAY(piShuffle);
}


void CvMapGenerator::addGoodies()
{
	PROFILE("CvMapGenerator::addGoodies");

	if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "addGoodies"))
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
	int i;

	for (i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		pPlot->setFeatureType(NO_FEATURE);
	}
}

void CvMapGenerator::eraseBonuses()
{
	int i;

	for (i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		pPlot->setBonusType(NO_BONUS);
	}
}

void CvMapGenerator::eraseGoodies()
{
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

	Cy::call_optional(gDLL->getPythonIFace()->getMapScriptModule(), "beforeGeneration");

	if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "generateRandomMap"))
	{
		return; // Python override
	}

	gDLL->NiTextOut(CvString::format("Generating Random Map %S, %S...", gDLL->getMapScriptName().GetCString(), GC.getWorldInfo(GC.getMap().getWorldSize()).getDescription()).c_str());

	generatePlotTypes();
	generateTerrain();
}

void CvMapGenerator::generatePlotTypes()
{
	int* paiPlotTypes = new int[GC.getMap().numPlots()];

	int iNumPlots = GC.getMap().numPlots();

	std::vector<int> plotTypesOut;
	if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "generatePlotTypes", plotTypesOut))
	{
		// Python override
		FAssertMsg((int)plotTypesOut.size() == iNumPlots, "python generatePlotTypes() should return list with length numPlots");
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			paiPlotTypes[iI] = plotTypesOut[iI];
		}
	}
	else
	{
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			paiPlotTypes[iI] = PLOT_LAND;
		}
	}

	setPlotTypes(paiPlotTypes);

	SAFE_DELETE_ARRAY(paiPlotTypes);
}

void CvMapGenerator::generateTerrain()
{
	PROFILE("generateTerrain()");

	std::vector<int> terrainMapOut;
	if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "generateTerrainTypes", terrainMapOut))
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

	Cy::call_optional(gDLL->getPythonIFace()->getMapScriptModule(), "afterGeneration");
}

void CvMapGenerator::setPlotTypes(const int* paiPlotTypes)
{
	CvPlot* pLoopPlot;
	int iNumPlots;

	iNumPlots = GC.getMap().numPlots();

	for (int iI = 0; iI < iNumPlots; iI++)
	{
		gDLL->callUpdater();
		GC.getMap().plotByIndex(iI)->setPlotType(((PlotTypes)(paiPlotTypes[iI])), false, false);
	}

	GC.getMap().recalculateAreas();

	for (int iI = 0; iI < iNumPlots; iI++)
	{
		gDLL->callUpdater();
		pLoopPlot = GC.getMap().plotByIndex(iI);

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

int CvMapGenerator::getRiverValueAtPlot(CvPlot* pPlot)
{
	CvPlot* pAdjacentPlot;
	CvRandom riverRand;
	int iSum;
	int iI;

	FAssert(pPlot != NULL);

	long result = 0;
	if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "getRiverAltitude", Cy::Args() << pPlot, result))
	{
		FAssertMsg(result >= 0, "python getRiverAltitude() must return >= 0");
		if (result >= 0)
		{
			return result;
		}
	}

	iSum = result;

	iSum += ((NUM_PLOT_TYPES - pPlot->getPlotType()) * 20);

	for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			iSum += (NUM_PLOT_TYPES - pAdjacentPlot->getPlotType());
		}
		else
		{
			iSum += (NUM_PLOT_TYPES * 10);
		}
	}

	riverRand.init((pPlot->getX() * 43251267) + (pPlot->getY() * 8273903));

	iSum += (riverRand.get(10, "River Rand"));

	return iSum;
}

int CvMapGenerator::calculateNumBonusesToAdd(BonusTypes eBonusType)
{
	const CvBonusInfo& pBonusInfo = GC.getBonusInfo(eBonusType);

	// Calculate iBonusCount, the amount of this bonus to be placed:
	int iBaseCount =
	(
		pBonusInfo.getConstAppearance() +
		GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance1(), "calculateNumBonusesToAdd-1") +
		GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance2(), "calculateNumBonusesToAdd-2") +
		GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance3(), "calculateNumBonusesToAdd-3") +
		GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance4(), "calculateNumBonusesToAdd-4")
	);
	iBaseCount += iBaseCount * GC.getMap().getWorldSize() / 4; // Scale by map size

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

	if (GC.getGame().isOption(GAMEOPTION_MORE_RESOURCES))
	{
		iBaseCount *= (GC.getDefineINT("BONUS_COUNT_PERCENTAGE_MODIFIER_ON_MORE_RESOURCES") + 100);
		iBaseCount /= 100;
	}
	iBaseCount /= 100;

	if (iBaseCount < 1) { return 1; }
	return iBaseCount;
}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/

void CvMapGenerator::setUseDefaultMapScript(bool bTrue)
{
	m_bUseDefaultMapScript = bTrue;
}

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/