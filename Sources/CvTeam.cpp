// team.cpp

#include "CvGameCoreDLL.h"
#include "CvGameAI.h"
#include "CvDiploParameters.h"
#include "CvInitCore.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"

// Public Functions...
#pragma warning( disable : 4355 )
CvTeam::CvTeam() : m_GameObject(this),
m_Properties(this)
{
	m_aiStolenVisibilityTimer = new int[MAX_TEAMS];
	m_aiWarWearinessTimes100 = new int[MAX_TEAMS];
	m_aiTechShareCount = new int[MAX_TEAMS];
	m_aiCommerceFlexibleCount = new int[NUM_COMMERCE_TYPES];
	m_aiExtraMoves = new int[NUM_DOMAIN_TYPES];

	m_aiEspionagePointsAgainstTeam = new int[MAX_TEAMS];
	m_aiCounterespionageTurnsLeftAgainstTeam = new int[MAX_TEAMS];
	m_aiCounterespionageModAgainstTeam = new int[MAX_TEAMS];

	m_abAtWar = new bool[MAX_TEAMS];
	m_abHasMet = new bool[MAX_TEAMS];
	m_abPermanentWarPeace = new bool[MAX_TEAMS];
	m_abOpenBorders = new bool[MAX_TEAMS];
	m_abDefensivePact = new bool[MAX_TEAMS];
	m_abForcePeace = new bool[MAX_TEAMS];
	m_abVassal = new bool[MAX_TEAMS];
	m_abCanLaunch = NULL;

	m_abIsRebelAgainst = new bool[MAX_TEAMS];

	m_paiRouteChange = NULL;
	m_paiProjectCount = NULL;
	m_paiProjectDefaultArtTypes = NULL;
	m_pavProjectArtTypes = NULL;
	m_paiProjectMaking = NULL;
	m_paiBuildingCount = NULL;
	m_paiUnitCount = NULL;
	m_paiObsoleteBuildingCount = NULL;
	m_paiResearchProgress = NULL;
	m_paiTechCount = NULL;
	m_paiTerrainTradeCount = NULL;
	m_aiVictoryCountdown = NULL;
	m_aiForceTeamVoteEligibilityCount = NULL;

	m_pabHasTech = NULL;
	m_pabNoTradeTech = NULL;

	m_ppaaiImprovementYieldChange = NULL;

	m_ppiBuildingCommerceChange = NULL;
	m_ppiBuildingYieldChange = NULL;
	m_ppiBuildingSpecialistChange = NULL;
	m_ppiBuildingCommerceModifier = NULL;
	m_ppiBuildingYieldModifier = NULL;
	m_paiTechExtraBuildingHappiness = NULL;
	m_paiTechExtraBuildingHealth = NULL;
	m_abEmbassy = new bool[MAX_TEAMS];
	m_abLimitedBorders = new bool[MAX_TEAMS];
	m_abFreeTrade = new bool[MAX_TEAMS];
	m_paiFreeSpecialistCount = NULL;
	m_paiUnitStrengthChange = NULL;

	reset((TeamTypes)0, true);
}


CvTeam::~CvTeam()
{
	uninit();

	SAFE_DELETE_ARRAY(m_aiStolenVisibilityTimer);
	SAFE_DELETE_ARRAY(m_aiWarWearinessTimes100);
	SAFE_DELETE_ARRAY(m_aiTechShareCount);
	SAFE_DELETE_ARRAY(m_aiCommerceFlexibleCount);
	SAFE_DELETE_ARRAY(m_aiExtraMoves);
	SAFE_DELETE_ARRAY(m_aiEspionagePointsAgainstTeam);
	SAFE_DELETE_ARRAY(m_aiCounterespionageTurnsLeftAgainstTeam);
	SAFE_DELETE_ARRAY(m_aiCounterespionageModAgainstTeam);
	SAFE_DELETE_ARRAY(m_abAtWar);
	SAFE_DELETE_ARRAY(m_abHasMet);
	SAFE_DELETE_ARRAY(m_abPermanentWarPeace);
	SAFE_DELETE_ARRAY(m_abOpenBorders);
	SAFE_DELETE_ARRAY(m_abDefensivePact);
	SAFE_DELETE_ARRAY(m_abForcePeace);
	SAFE_DELETE_ARRAY(m_abVassal);
	SAFE_DELETE_ARRAY(m_abIsRebelAgainst);
	SAFE_DELETE_ARRAY(m_abEmbassy);
	SAFE_DELETE_ARRAY(m_abLimitedBorders);
	SAFE_DELETE_ARRAY(m_abFreeTrade);
}


void CvTeam::init(TeamTypes eID)
{
	//--------------------------------
	// Init saved data
	reset(eID);
	//--------------------------------
	// Init non-saved data
	//--------------------------------
	// Init other game data
	AI_init();

	if( GC.getGame().isFinalInitialized() )
	{
		//logMsg("   Checking for declarations of war against reset team %d", (int)getID() );

		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if( iI != getID() )
			{
				if( GET_TEAM((TeamTypes)iI).isMinorCiv() || (isMinorCiv() && GET_TEAM((TeamTypes)iI).isAlive()))
				{
					GET_TEAM((TeamTypes)iI).declareWar(getID(), false, WARPLAN_LIMITED);
					GET_TEAM((TeamTypes)iI).setHasMet(getID(),false);
					setHasMet((TeamTypes)iI,false);
					//logMsg("   Minor team %d declared war, using war plan %d", iI, (int)GET_TEAM((TeamTypes)iI).AI_getWarPlan(getID()) );
				}
				//TBNOTE: THIS WILL NEED TO BE ADJUSTED so that it's not so patently sweeping for all NPCs IF AN NPC IS
				//SUPPOSED TO BE FRIENDLY WITH PLAYERS or any other NPC faction!
				if( GET_TEAM((TeamTypes)iI).isNPC() || (isNPC() && GET_TEAM((TeamTypes)iI).isAlive()))
				{
					if (GC.getGame().isOption(GAMEOPTION_PEACE_AMONG_NPCS) && GET_TEAM((TeamTypes)iI).isNPC() && isNPC())
					{
						continue;
					}
					else
					{
						GET_TEAM((TeamTypes)iI).declareWar(getID(), false, WARPLAN_LIMITED);
					}
					//logMsg("   Barb team %d declared war, using war plan %d", iI, (int)GET_TEAM((TeamTypes)iI).AI_getWarPlan(getID()) );
				}
			}
		}
	}
}


void CvTeam::uninit()
{
	SAFE_DELETE_ARRAY(m_abCanLaunch);
	SAFE_DELETE_ARRAY(m_paiRouteChange);
	SAFE_DELETE_ARRAY(m_paiProjectCount);
	SAFE_DELETE_ARRAY(m_paiProjectDefaultArtTypes);
	SAFE_DELETE_ARRAY(m_pavProjectArtTypes);
	SAFE_DELETE_ARRAY(m_paiProjectMaking);
	SAFE_DELETE_ARRAY(m_paiBuildingCount);
	SAFE_DELETE_ARRAY(m_paiUnitCount);
	SAFE_DELETE_ARRAY(m_paiObsoleteBuildingCount);
	SAFE_DELETE_ARRAY(m_paiResearchProgress);
	SAFE_DELETE_ARRAY(m_paiTechCount);
	SAFE_DELETE_ARRAY(m_paiTerrainTradeCount);
	SAFE_DELETE_ARRAY(m_aiVictoryCountdown);
	SAFE_DELETE_ARRAY(m_aiForceTeamVoteEligibilityCount);
	SAFE_DELETE_ARRAY(m_pabHasTech);
	SAFE_DELETE_ARRAY(m_pabNoTradeTech);
	SAFE_DELETE_ARRAY2(m_ppaaiImprovementYieldChange, GC.getNumImprovementInfos());
	SAFE_DELETE_ARRAY(m_paiTechExtraBuildingHappiness);
	SAFE_DELETE_ARRAY(m_paiTechExtraBuildingHealth);
	SAFE_DELETE_ARRAY(m_paiFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_paiUnitStrengthChange);
	SAFE_DELETE_ARRAY2(m_ppiBuildingCommerceChange, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBuildingYieldChange, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBuildingSpecialistChange, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBuildingYieldModifier, GC.getNumBuildingInfos());
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvTeam::reset(TeamTypes eID, bool bConstructorCall)
{
	int iI, iJ;

	//--------------------------------
	// Uninit class
	uninit();

	m_iNumMembers = 0;
	m_iAliveCount = 0;
	m_iEverAliveCount = 0;
	m_iNumCities = 0;
	m_iTotalPopulation = 0;
	m_iTotalLand = 0;
	m_iNukeInterception = 0;
	m_iExtraWaterSeeFromCount = 0;
	m_iMapTradingCount = 0;
	m_iTechTradingCount = 0;
	m_iGoldTradingCount = 0;
	m_iCanPassPeaksCount = 0;
	m_iMoveFastPeaksCount = 0;
	m_iCanFoundOnPeaksCount = 0;
	m_iRebaseAnywhereCount = 0;
	m_iForeignTradeModifier = 0;
	m_iTradeModifier = 0;
	m_iTradeMissionModifier = 0;
	m_iCorporationRevenueModifier = 0;
	m_iCorporationMaintenanceModifier = 0;
	m_iEmbassyTradingCount = 0;
	m_iLimitedBordersTradingCount = 0;
	m_iCanFarmDesertCount = 0;
	m_iOpenBordersTradingCount = 0;
	m_iDefensivePactTradingCount = 0;
	m_iPermanentAllianceTradingCount = 0;
	m_iVassalTradingCount = 0;
	m_iBridgeBuildingCount = 0;
	m_iIrrigationCount = 0;
	m_iIgnoreIrrigationCount = 0;
	m_iWaterWorkCount = 0;
	m_iVassalPower = 0;
	m_iMasterPower = 0;
	m_iEnemyWarWearinessModifier = 0;
	m_iRiverTradeCount = 0;
	m_iEspionagePointsEver = 0;
	m_iLastRoundOfValidImprovementCacheUpdate = 0;

	m_bMapCentering = false;
	m_bCapitulated = false;

	m_eID = eID;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiStolenVisibilityTimer[iI] = 0;
		m_aiWarWearinessTimes100[iI] = 0;
		m_aiTechShareCount[iI] = 0;
		m_aiEspionagePointsAgainstTeam[iI] = 0;
		m_aiCounterespionageTurnsLeftAgainstTeam[iI] = 0;
		m_aiCounterespionageModAgainstTeam[iI] = 0;
		m_abHasMet[iI] = false;
		m_abAtWar[iI] = false;
		m_abPermanentWarPeace[iI] = false;
		m_abOpenBorders[iI] = false;
		m_abLimitedBorders[iI] = false;
		m_abFreeTrade[iI] = false;
		m_abEmbassy[iI] = false;
		m_abDefensivePact[iI] = false;
		m_abForcePeace[iI] = false;
		m_abVassal[iI] = false;
		m_abIsRebelAgainst[iI] = false;

		if (!bConstructorCall && getID() != NO_TEAM)
		{
			CvTeam& kLoopTeam = GET_TEAM((TeamTypes) iI);
			kLoopTeam.m_aiStolenVisibilityTimer[getID()] = 0;
			kLoopTeam.m_aiWarWearinessTimes100[getID()] = 0;
			kLoopTeam.m_aiTechShareCount[getID()] = 0;
			kLoopTeam.m_aiEspionagePointsAgainstTeam[getID()] = 0;
			kLoopTeam.m_aiCounterespionageTurnsLeftAgainstTeam[getID()] = 0;
			kLoopTeam.m_aiCounterespionageModAgainstTeam[getID()] = 0;
			kLoopTeam.m_abHasMet[getID()] = false;
			kLoopTeam.m_abAtWar[getID()] = false;
			kLoopTeam.m_abPermanentWarPeace[getID()] = false;
			kLoopTeam.m_abOpenBorders[getID()] = false;
			kLoopTeam.m_abEmbassy[getID()] = false;
			kLoopTeam.m_abLimitedBorders[getID()] = false;
			kLoopTeam.m_abFreeTrade[getID()] = false;
			kLoopTeam.m_abDefensivePact[getID()] = false;
			kLoopTeam.m_abForcePeace[getID()] = false;
			kLoopTeam.m_abVassal[getID()] = false;
		}
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiCommerceFlexibleCount[iI] = 0;
	}

	for (iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiExtraMoves[iI] = 0;
	}

	if (!bConstructorCall)
	{
		FAssertMsg(m_abCanLaunch==NULL, "about to leak memory, CvTeam::m_abCanLaunch");
		m_abCanLaunch = new bool[GC.getNumVictoryInfos()];
		for (iI = 0; iI < GC.getNumVictoryInfos(); iI++)
		{
			m_abCanLaunch[iI] = false;
		}

		FAssertMsg(m_paiRouteChange==NULL, "about to leak memory, CvTeam::m_paiRouteChange");
		m_paiRouteChange = new int[GC.getNumRouteInfos()];
		for (iI = 0; iI < GC.getNumRouteInfos(); iI++)
		{
			m_paiRouteChange[iI] = 0;
		}

		FAssertMsg(m_paiProjectCount==NULL, "about to leak memory, CvPlayer::m_paiProjectCount");
		m_paiProjectCount = new int [GC.getNumProjectInfos()];
		FAssertMsg(m_paiProjectDefaultArtTypes==NULL, "about to leak memory, CvPlayer::m_paiProjectDefaultArtTypes");
		m_paiProjectDefaultArtTypes = new int [GC.getNumProjectInfos()];
		FAssertMsg(m_pavProjectArtTypes==NULL, "about to leak memory, CvPlayer::m_pavProjectArtTypes");
		m_pavProjectArtTypes = new std::vector<int> [GC.getNumProjectInfos()];
		FAssertMsg(m_paiProjectMaking==NULL, "about to leak memory, CvPlayer::m_paiProjectMaking");
		m_paiProjectMaking = new int [GC.getNumProjectInfos()];
		for (iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			m_paiProjectCount[iI] = 0;
			m_paiProjectDefaultArtTypes[iI] = 0;
			m_paiProjectMaking[iI] = 0;
		}

		FAssertMsg(m_paiUnitCount==NULL, "about to leak memory, CvTeam::m_paiUnitCount");
		m_paiUnitCount = new int [GC.getNumUnitInfos()];
		for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			m_paiUnitCount[iI] = 0;
		}

		FAssertMsg(m_paiBuildingCount==NULL, "about to leak memory, CvTeam::m_paiBuildingCount");
		m_paiBuildingCount = new int [GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_paiBuildingCount[iI] = 0;
		}

		FAssertMsg(m_paiObsoleteBuildingCount==NULL, "about to leak memory, CvTeam::m_paiObsoleteBuildingCount");
		m_paiObsoleteBuildingCount = new int[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_paiObsoleteBuildingCount[iI] = 0;
		}

		FAssertMsg(m_paiResearchProgress==NULL, "about to leak memory, CvPlayer::m_paiResearchProgress");
		m_paiResearchProgress = new int [GC.getNumTechInfos()];
		FAssertMsg(m_paiTechCount==NULL, "about to leak memory, CvPlayer::m_paiTechCount");
		m_paiTechCount = new int [GC.getNumTechInfos()];
		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_paiResearchProgress[iI] = 0;
			m_paiTechCount[iI] = 0;
		}

		FAssertMsg(m_paiTerrainTradeCount==NULL, "about to leak memory, CvTeam::m_paiTerrainTradeCount");
		m_paiTerrainTradeCount = new int[GC.getNumTerrainInfos()];
		for (iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			m_paiTerrainTradeCount[iI] = 0;
		}

		FAssertMsg(m_aiVictoryCountdown==NULL, "about to leak memory, CvTeam::m_aiVictoryCountdown");
		m_aiVictoryCountdown = new int[GC.getNumVictoryInfos()];
		for (iI = 0; iI < GC.getNumVictoryInfos(); iI++)
		{
			m_aiVictoryCountdown[iI] = -1;
		}

		FAssertMsg(m_paiTechExtraBuildingHappiness==NULL, "about to leak memory, m_paiTechExtraBuildingHappiness"); //Afforess
		m_paiTechExtraBuildingHappiness = new int[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_paiTechExtraBuildingHappiness[iI] = 0;
		}

		FAssertMsg(m_paiTechExtraBuildingHealth==NULL, "about to leak memory, m_paiTechExtraBuildingHealth");
		m_paiTechExtraBuildingHealth = new int[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_paiTechExtraBuildingHealth[iI] = 0;
		}

		FAssertMsg(m_pabHasTech==NULL, "about to leak memory, CvTeam::m_pabHasTech");
		m_pabHasTech = new bool[GC.getNumTechInfos()];
		FAssertMsg(m_pabNoTradeTech==NULL, "about to leak memory, CvTeam::m_pabNoTradeTech");
		m_pabNoTradeTech = new bool[GC.getNumTechInfos()];
		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_pabHasTech[iI] = false;
			m_pabNoTradeTech[iI] = false;
		}

		FAssertMsg(m_ppaaiImprovementYieldChange==NULL, "about to leak memory, CvTeam::m_ppaaiImprovementYieldChange");
		m_ppaaiImprovementYieldChange = new int*[GC.getNumImprovementInfos()];
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_ppaaiImprovementYieldChange[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiImprovementYieldChange[iI][iJ] = 0;
			}
		}

		FAssertMsg(m_aiForceTeamVoteEligibilityCount==NULL, "about to leak memory, CvTeam::m_aiForceTeamVoteEligibilityCount");
		m_aiForceTeamVoteEligibilityCount = new int[GC.getNumVoteSourceInfos()];
		for (iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_aiForceTeamVoteEligibilityCount[iI] = 0;
		}

		m_aeRevealedBonuses.clear();

		FAssertMsg(m_ppiBuildingCommerceChange==NULL, "about to leak memory, CvTeam::m_ppiBuildingCommerceChange");
		m_ppiBuildingCommerceChange = new int*[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppiBuildingCommerceChange[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppiBuildingCommerceChange[iI][iJ] = 0;
			}
		}

		FAssertMsg(m_ppiBuildingYieldChange==NULL, "about to leak memory, CvTeam::m_ppiBuildingYieldChange");
		m_ppiBuildingYieldChange = new int*[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppiBuildingYieldChange[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppiBuildingYieldChange[iI][iJ] = 0;
			}
		}

		FAssertMsg(m_ppiBuildingSpecialistChange==NULL, "about to leak memory, CvTeam::m_ppiBuildingSpecialistChange");
		m_ppiBuildingSpecialistChange = new int*[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppiBuildingSpecialistChange[iI] = new int[GC.getNumSpecialistInfos()];
			for (iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
			{
				m_ppiBuildingSpecialistChange[iI][iJ] = 0;
			}
		}
		FAssertMsg(m_paiFreeSpecialistCount==NULL, "about to leak memory, CvTeam::m_paiFreeSpecialistCount");
		m_paiFreeSpecialistCount = new int[GC.getNumSpecialistInfos()];
		for (iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			m_paiFreeSpecialistCount[iJ] = 0;
		}

		FAssertMsg(m_ppiBuildingCommerceModifier==NULL, "about to leak memory, CvTeam::m_ppiBuildingCommerceModifier");
		m_ppiBuildingCommerceModifier = new int*[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppiBuildingCommerceModifier[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppiBuildingCommerceModifier[iI][iJ] = 0;
			}
		}

		FAssertMsg(m_ppiBuildingYieldModifier==NULL, "about to leak memory, CvTeam::m_ppiBuildingYieldModifier");
		m_ppiBuildingYieldModifier = new int*[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppiBuildingYieldModifier[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppiBuildingYieldModifier[iI][iJ] = 0;
			}
		}
		FAssertMsg(m_paiUnitStrengthChange==NULL, "about to leak memory, CvTeam::m_paiUnitStrengthChange");
		m_paiUnitStrengthChange = new int[GC.getNumUnitInfos()];
		for (iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
		{
			m_paiUnitStrengthChange[iJ] = 0;
		}

		AI_reset(false);
	}
	m_Properties.clear();
}

//
// for clearing data stored in plots and cities for this team
//
void CvTeam::resetPlotAndCityData()
{
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		pLoopPlot->setRevealedOwner(getID(), NO_PLAYER);
		pLoopPlot->setRevealedImprovementType(getID(), NO_IMPROVEMENT);
		pLoopPlot->setRevealedRouteType(getID(), NO_ROUTE);
		pLoopPlot->setRevealed(getID(), false, false, getID(), true);

		CvCity* pLoopCity = pLoopPlot->getPlotCity();
		if (pLoopCity != NULL)
		{
			pLoopCity->setRevealed(getID(), false);
			pLoopCity->setEspionageVisibility(getID(), false, true);
		}
	}
}

bool CvTeam::isRebel() const
{
	bool bValid = false;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).isRebel())
			{
				bValid = true;
			}
			else
			{
				return false;
			}
		}
	}
	return bValid;
}

bool CvTeam::isSingleCityTeam() const
{
	int iCities = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);

		if (kPlayer.getTeam() == getID() && kPlayer.isAlive())
		{
			iCities += GET_PLAYER((PlayerTypes)iI).getNumCities();
		}
		if (iCities > 1)
		{
			return false;
		}
	}
	return true;
}

bool CvTeam::isRebelAgainst( TeamTypes eTeam ) const
{
	if (eTeam >= 0 && eTeam < MAX_TEAMS)
	{
		return m_abIsRebelAgainst[(int)eTeam];
	}
	return false;
}

void CvTeam::setRebelAgainst(TeamTypes eTeam, bool bNewValue)
{
	if (eTeam >= 0 && eTeam < MAX_TEAMS)
		m_abIsRebelAgainst[(int)eTeam] = bNewValue;
}

int CvTeam::countRebelAgainst() const
{
	int count = 0;
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (isRebelAgainst((TeamTypes) iI)) count++;
	}
	return count;
}

int CvTeam::getNumMilitaryUnits() const
{
	int count = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			count += GET_PLAYER((PlayerTypes)iI).getNumMilitaryUnits();
		}
	}
	return count;
}

void CvTeam::addTeam(TeamTypes eTeam)
{
	CLLNode<TradeData>* pNode;
	CvWString szBuffer;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() != getID() && GET_PLAYER((PlayerTypes)iI).getTeam() != eTeam
		&& GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(getID()) && GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(eTeam))
		{
			MEMORY_TRACK_EXEMPT();

			szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_PERMANENT_ALLIANCE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
			AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIRALLIANCE", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
		}
	}
	szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_PERMANENT_ALLIANCE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

	CvDeal* pLoopDeal;
	int iLoop;
	for (pLoopDeal = GC.getGame().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGame().nextDeal(&iLoop))
	{
		if (GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == getID() && GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == eTeam
		||  GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == eTeam   && GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == getID())
		{
			bool bValid = true;

			if (pLoopDeal->getFirstTrades() != NULL)
			{
				for (pNode = pLoopDeal->getFirstTrades()->head(); pNode; pNode = pLoopDeal->getFirstTrades()->next(pNode))
				{
					if(pNode->m_data.m_eItemType == TRADE_OPEN_BORDERS
					|| pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT
					|| pNode->m_data.m_eItemType == TRADE_PEACE_TREATY
					|| pNode->m_data.m_eItemType == TRADE_VASSAL
					|| pNode->m_data.m_eItemType == TRADE_RITE_OF_PASSAGE
					|| pNode->m_data.m_eItemType == TRADE_FREE_TRADE_ZONE
					|| pNode->m_data.m_eItemType == TRADE_SURRENDER)
					{
						bValid = false;
					}
				}
			}
			if (pLoopDeal->getSecondTrades() != NULL)
			{
				for (pNode = pLoopDeal->getSecondTrades()->head(); pNode; pNode = pLoopDeal->getSecondTrades()->next(pNode))
				{
					if(pNode->m_data.m_eItemType == TRADE_OPEN_BORDERS
					|| pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT
					|| pNode->m_data.m_eItemType == TRADE_RITE_OF_PASSAGE
					|| pNode->m_data.m_eItemType == TRADE_FREE_TRADE_ZONE
					|| pNode->m_data.m_eItemType == TRADE_PEACE_TREATY
					|| pNode->m_data.m_eItemType == TRADE_VASSAL
					|| pNode->m_data.m_eItemType == TRADE_SURRENDER)
					{
						bValid = false;
					}
				}
			}
			if (!bValid)
			{
				pLoopDeal->kill();
			}
		}
	}
	shareItems(eTeam);
	GET_TEAM(eTeam).shareItems(getID());

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isHasMet((TeamTypes)iI))
			{
				meet((TeamTypes)iI, false);
			}
			else if (isHasMet((TeamTypes)iI))
			{
				GET_TEAM(eTeam).meet((TeamTypes)iI, false);
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isAtWar((TeamTypes)iI))
			{
				declareWar((TeamTypes)iI, false, GET_TEAM(eTeam).AI_getWarPlan((TeamTypes)iI));
			}
			else if (isAtWar((TeamTypes)iI))
			{
				GET_TEAM(eTeam).declareWar((TeamTypes)iI, false, AI_getWarPlan((TeamTypes)iI));
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isPermanentWarPeace((TeamTypes)iI))
			{
				setPermanentWarPeace((TeamTypes)iI, true);
			}
			else if (isPermanentWarPeace((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setPermanentWarPeace((TeamTypes)iI, true);
			}

			if (GET_TEAM(eTeam).isOpenBorders((TeamTypes)iI))
			{
				setOpenBorders((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setOpenBorders(getID(), true);
			}
			else if (isOpenBorders((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setOpenBorders((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setOpenBorders(eTeam, true);
			}

			if (GET_TEAM(eTeam).isLimitedBorders((TeamTypes)iI))
			{
				setLimitedBorders(((TeamTypes)iI), true);
				GET_TEAM((TeamTypes)iI).setLimitedBorders(getID(), true);
			}
			else if (isLimitedBorders((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setLimitedBorders((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setLimitedBorders(eTeam, true);
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isHasEmbassy((TeamTypes)iI))
			{
				setHasEmbassy(((TeamTypes)iI), true);
				GET_TEAM((TeamTypes)iI).setHasEmbassy(getID(), true);
			}
			else if (isDefensivePact((TeamTypes)iI) || isOpenBorders((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setHasEmbassy((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setHasEmbassy(eTeam, true);
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isDefensivePact((TeamTypes)iI))
			{
				setDefensivePact((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setDefensivePact(getID(), true);
			}
			else if (isDefensivePact((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setDefensivePact((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setDefensivePact(eTeam, true);
			}

			if (GET_TEAM(eTeam).isForcePeace((TeamTypes)iI))
			{
				setForcePeace((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setForcePeace(getID(), true);
			}
			else if (isForcePeace((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setForcePeace((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setForcePeace(eTeam, true);
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isVassal((TeamTypes)iI))
			{
				setVassal((TeamTypes)iI, true, isCapitulated());
			}
			else if (isVassal((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setVassal((TeamTypes)iI, true, isCapitulated());
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM((TeamTypes)iI).isVassal(eTeam))
			{
				GET_TEAM((TeamTypes)iI).setVassal(getID(), true, GET_TEAM((TeamTypes)iI).isCapitulated());
			}
			else if (GET_TEAM((TeamTypes)iI).isVassal(getID()))
			{
				GET_TEAM((TeamTypes)iI).setVassal(eTeam, true, GET_TEAM((TeamTypes)iI).isCapitulated());
			}
		}
	}
	shareCounters(eTeam);
	GET_TEAM(eTeam).shareCounters(getID());

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
		{
			GET_PLAYER((PlayerTypes)iI).setTeam(getID());
		}
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		pLoopPlot->changeVisibilityCount(getID(), pLoopPlot->getVisibilityCount(eTeam), NO_INVISIBLE, false);

		for (int iJ = 0; iJ < GC.getNumInvisibleInfos(); iJ++)
		{
			if (!GC.getGame().isOption(GAMEOPTION_HIDE_AND_SEEK))
			{
				pLoopPlot->changeInvisibleVisibilityCount(getID(), ((InvisibleTypes)iJ), pLoopPlot->getInvisibleVisibilityCount(eTeam, (InvisibleTypes)iJ), 0);
			}
			else
			{
				InvisibleTypes eInvisible = (InvisibleTypes) iJ;
				for (int iK = 0; iK < pLoopPlot->getNumPlotTeamVisibilityIntensity(); iK++)
				{
					if (pLoopPlot->getPlotTeamVisibilityIntensity(iK).eInvisibility == eInvisible &&
						pLoopPlot->getPlotTeamVisibilityIntensity(iK).eTeam == eTeam)
					{
						pLoopPlot->changeInvisibleVisibilityCount(getID(), eInvisible, pLoopPlot->getPlotTeamVisibilityIntensity(iK).iCount, pLoopPlot->getPlotTeamVisibilityIntensity(iK).iIntensity);
					}
				}
			}
		}
		if (pLoopPlot->isRevealed(eTeam, false))
		{
			pLoopPlot->setRevealed(getID(), true, false, eTeam, false);
		}
	}
	GC.getGame().updatePlotGroups();

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID())
		{
			GET_TEAM((TeamTypes)iI).setWarWeariness(getID(), ((GET_TEAM((TeamTypes)iI).getWarWeariness(getID()) + GET_TEAM((TeamTypes)iI).getWarWeariness(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).setStolenVisibilityTimer(getID(), ((GET_TEAM((TeamTypes)iI).getStolenVisibilityTimer(getID()) + GET_TEAM((TeamTypes)iI).getStolenVisibilityTimer(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setAtWarCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getAtWarCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getAtWarCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setAtPeaceCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getAtPeaceCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getAtPeaceCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setHasMetCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getHasMetCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getHasMetCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setDefensivePactCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getDefensivePactCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getDefensivePactCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setShareWarCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getShareWarCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getShareWarCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setWarSuccess(getID(), ((GET_TEAM((TeamTypes)iI).AI_getWarSuccess(getID()) + GET_TEAM((TeamTypes)iI).AI_getWarSuccess(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setEnemyPeacetimeTradeValue(getID(), ((GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeTradeValue(getID()) + GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeTradeValue(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setEnemyPeacetimeGrantValue(getID(), ((GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeGrantValue(getID()) + GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeGrantValue(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).setEspionagePointsAgainstTeam( getID(), std::max(GET_TEAM((TeamTypes)iI).getEspionagePointsAgainstTeam(getID()), GET_TEAM((TeamTypes)iI).getEspionagePointsAgainstTeam(eTeam)));

			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				GET_TEAM((TeamTypes)iI).AI_setWarPlan(getID(), NO_WARPLAN, false);
				GET_TEAM((TeamTypes)iI).AI_setWarPlan(eTeam, NO_WARPLAN, false);
			}
		}
	}
	AI_updateWorstEnemy();
	AI_updateAreaStragies();
	GC.getGame().updateScore(true);
}


void CvTeam::shareItems(TeamTypes eTeam)
{
	int iI, iJ, iK;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	for (iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if (GET_TEAM(eTeam).isHasTech((TechTypes)iI))
		{
			setHasTech(((TechTypes)iI), true, NO_PLAYER, true, false);
		}
	}

	for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
	{
		if (GET_TEAM(eTeam).isForceRevealedBonus((BonusTypes)iI))
		{
			setForceRevealedBonus((BonusTypes)iI, true);
		}
	}

	for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
	{
		setEspionagePointsAgainstTeam((TeamTypes)iTeam, std::max(GET_TEAM(eTeam).getEspionagePointsAgainstTeam((TeamTypes)iTeam), getEspionagePointsAgainstTeam((TeamTypes)iTeam)));
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
		{
			foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				for (iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
				{
					if (pLoopCity->getNumActiveBuilding((BuildingTypes)iJ) > 0)
					{
						if (GC.getBuildingInfo((BuildingTypes)iJ).isTeamShare())
						{
							for (iK = 0; iK < MAX_PLAYERS; iK++)
							{
								if (GET_PLAYER((PlayerTypes)iK).getTeam() == getID() && GET_PLAYER((PlayerTypes)iK).isAlive())
								{
									GET_PLAYER((PlayerTypes)iK).processBuilding(((BuildingTypes)iJ), pLoopCity->getNumBuilding((BuildingTypes)iJ), pLoopCity->area());
								}
							}
						}
						processBuilding(((BuildingTypes)iJ), pLoopCity->getNumBuilding((BuildingTypes)iJ));
					}
				}
			}
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
		{
			GET_PLAYER((PlayerTypes)iI).AI_updateBonusValue();
		}
	}
}


void CvTeam::shareCounters(TeamTypes eTeam)
{
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getID()) && (iI != eTeam))
		{
			if (GET_TEAM(eTeam).getWarWeariness((TeamTypes)iI) > getWarWeariness((TeamTypes)iI))
			{
				setWarWeariness(((TeamTypes)iI), GET_TEAM(eTeam).getWarWeariness((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).getStolenVisibilityTimer((TeamTypes)iI) > getStolenVisibilityTimer((TeamTypes)iI))
			{
				setStolenVisibilityTimer(((TeamTypes)iI), GET_TEAM(eTeam).getStolenVisibilityTimer((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getAtWarCounter((TeamTypes)iI) > AI_getAtWarCounter((TeamTypes)iI))
			{
				AI_setAtWarCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getAtWarCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getAtPeaceCounter((TeamTypes)iI) > AI_getAtPeaceCounter((TeamTypes)iI))
			{
				AI_setAtPeaceCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getAtPeaceCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getHasMetCounter((TeamTypes)iI) > AI_getHasMetCounter((TeamTypes)iI))
			{
				AI_setHasMetCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getHasMetCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getOpenBordersCounter((TeamTypes)iI) > AI_getOpenBordersCounter((TeamTypes)iI))
			{
				AI_setOpenBordersCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getOpenBordersCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getDefensivePactCounter((TeamTypes)iI) > AI_getDefensivePactCounter((TeamTypes)iI))
			{
				AI_setDefensivePactCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getDefensivePactCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getShareWarCounter((TeamTypes)iI) > AI_getShareWarCounter((TeamTypes)iI))
			{
				AI_setShareWarCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getShareWarCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getWarSuccess((TeamTypes)iI) > AI_getWarSuccess((TeamTypes)iI))
			{
				AI_setWarSuccess(((TeamTypes)iI), GET_TEAM(eTeam).AI_getWarSuccess((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getEnemyPeacetimeTradeValue((TeamTypes)iI) > AI_getEnemyPeacetimeTradeValue((TeamTypes)iI))
			{
				AI_setEnemyPeacetimeTradeValue(((TeamTypes)iI), GET_TEAM(eTeam).AI_getEnemyPeacetimeTradeValue((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getEnemyPeacetimeGrantValue((TeamTypes)iI) > AI_getEnemyPeacetimeGrantValue((TeamTypes)iI))
			{
				AI_setEnemyPeacetimeGrantValue(((TeamTypes)iI), GET_TEAM(eTeam).AI_getEnemyPeacetimeGrantValue((TeamTypes)iI));
			}

			GET_TEAM(eTeam).AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN, false);
		}
	}

	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		int iExtraProjects = GET_TEAM(eTeam).getProjectCount((ProjectTypes)iI) - getProjectCount((ProjectTypes)iI);
		if (iExtraProjects > 0)
		{
			changeProjectCount((ProjectTypes)iI, iExtraProjects);
			GC.getGame().incrementProjectCreatedCount((ProjectTypes)iI, -iExtraProjects);
		}

		changeProjectMaking(((ProjectTypes)iI), GET_TEAM(eTeam).getProjectMaking((ProjectTypes)iI));
	}

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		changeUnitCount(((UnitTypes)iI), GET_TEAM(eTeam).getUnitCount((UnitTypes)iI));
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		changeBuildingCount((BuildingTypes)iI, GET_TEAM(eTeam).getBuildingCount((BuildingTypes)iI));
	}

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		// Overflow from techs this team already has can cause bugged behavior
		if (!isHasTech((TechTypes)iI)
		&& GET_TEAM(eTeam).getResearchProgress((TechTypes)iI) > getResearchProgress((TechTypes)iI))
		{
			setResearchProgress(((TechTypes)iI), GET_TEAM(eTeam).getResearchProgress((TechTypes)iI), getLeaderID());
		}
		// Clear no tech trade if it is false for other team
		// Fixes bug where if, with no tech brokering, team A trades a tech to team B, then later joins B in
		// a permanent alliance.  Previous code would block the AB alliance from "brokering" the tech, even
		// though A had researched it on their own.
		if (GET_TEAM(eTeam).isHasTech((TechTypes)iI) && !GET_TEAM(eTeam).isNoTradeTech((TechTypes)iI))
		{
			setNoTradeTech(((TechTypes)iI), false);
		}
	}
}


void CvTeam::processBuilding(BuildingTypes eBuilding, int iChange, bool bReligiouslyDisabling)
{
	PROFILE_FUNC();

	if (!bReligiouslyDisabling)
	{
		for (int i = 0; i < GC.getNumVoteSourceInfos(); ++i)
		{
			if (GC.getBuildingInfo(eBuilding).getVoteSourceType() == i)
			{
				changeForceTeamVoteEligibilityCount((VoteSourceTypes)i, (GC.getBuildingInfo(eBuilding).isForceTeamVoteEligible()) ? iChange : 0);
			}
		}
		if (iChange > 0 && GC.getBuildingInfo(eBuilding).isMapCentering())
		{
			setMapCentering(true);
		}
	}
	changeEnemyWarWearinessModifier(GC.getBuildingInfo(eBuilding).getEnemyWarWearinessModifier() * iChange);
}


void CvTeam::doTurn()
{
	MEMORY_TRACE_FUNCTION();
	PROFILE("CvTeam::doTurn()")

	FAssertMsg(isAlive(), "isAlive is expected to be true");

	AI_doTurnPre();

	// TB - Keep this valid for barbs only for now.
	// We may need new rules here for aliens and such, but there's really no need to do anything for animals regarding technology unless we want to represent evolution somehow... lol.
	// Toffer - Neanderthals get thechs without this, so isHominid() is not necessary. May be a bug that they do, but they do.
	if (isBarbarian())
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (!isHasTech((TechTypes)iI))
			{
				int iPossibleCount = 0;
				int iCount = 0;
				for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
				{
					if (GET_TEAM((TeamTypes)iJ).isAlive())
					{
						if (GET_TEAM((TeamTypes)iJ).isHasTech((TechTypes)iI))
						{
							iCount++;
						}
						iPossibleCount++;
					}
				}
				if (iCount > 0)
				{
					FAssertMsg(iPossibleCount > 0, "iPossibleCount is expected to be greater than 0");

					changeResearchProgress((TechTypes)iI, std::max((getResearchCost((TechTypes)iI) * GC.getDefineINT("BARBARIAN_FREE_TECH_PERCENT") * iCount) / (100 * iPossibleCount), 1), getLeaderID());
				}
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (getStolenVisibilityTimer((TeamTypes)iI) > 0)
			{
				changeStolenVisibilityTimer(((TeamTypes)iI), -1);
			}

			if (getCounterespionageTurnsLeftAgainstTeam((TeamTypes) iI) > 0)
			{
				changeCounterespionageTurnsLeftAgainstTeam((TeamTypes) iI, -1);
			}

			if (getCounterespionageTurnsLeftAgainstTeam((TeamTypes) iI) == 0)
			{
				setCounterespionageModAgainstTeam((TeamTypes) iI, 0);
			}
		}
	}
	doWarWeariness();

	testCircumnavigated();

	AI_doTurnPost();
}


void CvTeam::updateYield()
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).updateYield();
			}
		}
	}
}


void CvTeam::updatePowerHealth()
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).updatePowerHealth();
			}
		}
	}
}


void CvTeam::updateCommerce()
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).updateCommerce();
			}
		}
	}
}


bool CvTeam::canChangeWarPeace(TeamTypes eTeam, bool bAllowVassal) const
{
	if (eTeam == getID())
	{
		return false;
	}

	if (isPermanentWarPeace(eTeam) || GET_TEAM(eTeam).isPermanentWarPeace(getID()))
	{
		return false;
	}

	if( isMinorCiv() || GET_TEAM(eTeam).isMinorCiv() )
	{
		return false; // No dealing with minor civs
	}

	for (int iLoopTeam = 0; iLoopTeam < MAX_PC_TEAMS; ++iLoopTeam)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iLoopTeam);
		if (kLoopTeam.isAlive())
		{
			if (kLoopTeam.isVassal(getID()) && kLoopTeam.isPermanentWarPeace(eTeam)
			|| kLoopTeam.isVassal(eTeam) && kLoopTeam.isPermanentWarPeace(getID()))
			{
				return false;
			}
		}
	}

	if (isAVassal())
	{
		return false;
	}

	if (bAllowVassal)
	{
		if (GET_TEAM(eTeam).isVassal(getID()))
		{
			return false;
		}
	}
	else if (GET_TEAM(eTeam).isAVassal())
	{
		return false;
	}
	return true;
}


bool CvTeam::canDeclareWar(TeamTypes eTeam) const
{
	if (eTeam == getID())
	{
		return false;
	}

	if (!(isAlive()) || !(GET_TEAM(eTeam).isAlive()))
	{
		return false;
	}

	if (isAtWar(eTeam))
	{
		return false;
	}

	if (!isHasMet(eTeam))
	{
		return false;
	}

	if (isForcePeace(eTeam))
	{
		return false;
	}

	for (int i = 0; i < MAX_TEAMS; ++i)
	{
		if (i != eTeam && i != getID() && GET_TEAM(eTeam).isVassal((TeamTypes)i))
		{
			if (isForcePeace((TeamTypes)i))
			{
				return false;
			}
		}
	}

	if (!canChangeWarPeace(eTeam, true))
	{
		return false;
	}

	if(GC.getUSE_CAN_DECLARE_WAR_CALLBACK())
	{
		if (!Cy::call<bool>(PYGameModule, "canDeclareWar", Cy::Args() << getID() << eTeam))
		{
			return false;
		}
	}

	return true;
}

/// \brief Version of canDeclareWar which ignores temporary peace treaties.
///
/// This function is for AIs considering who to start war preparations against, so they're future
/// plans aren't unnecessarily affected by current conditions.
///
/// Could not change definition of canDeclareWar, some sporadic crash-inducing compatibility issue
/// with the DLL it seems.  Lost a lot of time tracking down the source of the crash, it really is
/// just from adding bool bWhatever = false to canDeclareWar in CvTeam.h.  So, that's why there's
/// this overlapping second function.
bool CvTeam::canEventuallyDeclareWar(TeamTypes eTeam) const
{
	if (eTeam == getID())
	{
		return false;
	}

	if (!(isAlive()) || !(GET_TEAM(eTeam).isAlive()))
	{
		return false;
	}

	if (isAtWar(eTeam))
	{
		return false;
	}

	if (!isHasMet(eTeam))
	{
		return false;
	}

	if (!canChangeWarPeace(eTeam, true))
	{
		return false;
	}

	if(GC.getUSE_CAN_DECLARE_WAR_CALLBACK())
	{
		if (!Cy::call<bool>(PYGameModule, "canDeclareWar", Cy::Args() << getID() << eTeam))
		{
			return false;
		}
	}

	return true;
}


void CvTeam::declareWar(TeamTypes eTeam, bool bNewDiplo, WarPlanTypes eWarPlan, bool bCancelPacts)
{
	PROFILE_FUNC();

	//CLLNode<TradeData>* pNode;
	CvDiploParameters* pDiplo;
	CvDeal* pLoopDeal;
	CvWString szBuffer;
	//bool bCancelDeal;
	int iLoop;
	int iI, iJ;

	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");

	if (GC.getGame().isOption(GAMEOPTION_PEACE_AMONG_NPCS) && isNPC() && GET_TEAM(eTeam).isNPC())
	{
		if (isAtWar(eTeam))
		{
			setAtWar(eTeam, false);
			GET_TEAM(eTeam).setAtWar(getID(), false);
		}
		return;
	}

	bool bInFull = ((!GET_TEAM(eTeam).isNPC() || GET_TEAM(eTeam).isBarbarian()) && (!isNPC() || isBarbarian()));
	if (!isAtWar(eTeam))
	{
		//FAssertMsg((isHuman() || isMinorCiv() || GET_TEAM(eTeam).isMinorCiv() || isBarbarian() || GET_TEAM(eTeam).isBarbarian() || AI_isSneakAttackReady(eTeam) || (GET_TEAM(eTeam).getAtWarCount(true) > 0) || !(GC.getGame().isFinalInitialized()) || gDLL->GetWorldBuilderMode() || getVassalCount() > 0  || isAVassal() || GET_TEAM(eTeam).getVassalCount() > 0  || GET_TEAM(eTeam).isAVassal() || getDefensivePactCount() > 0), "Possibly accidental AI war!!!");

		if( gTeamLogLevel >= 1 )
		{
			logBBAI("  Team %d (%S) declares war on team %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eTeam);
		}

		for (pLoopDeal = GC.getGame().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGame().nextDeal(&iLoop))
		{
			if (((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == getID()) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == eTeam)) ||
					((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == eTeam) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == getID())))
			{
				pLoopDeal->kill();
			}
		}

		if (bInFull)
		{
			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)) && GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					GET_PLAYER((PlayerTypes)iI).updatePlunder(-1, false);
				}

				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getPledgedSecretaryGeneralVote() == eTeam)
					{
						GET_PLAYER((PlayerTypes)iI).setPledgedSecretaryGeneralVote(NO_TEAM);
					}
				}

				if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam && GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getPledgedSecretaryGeneralVote() == getID())
					{
						GET_PLAYER((PlayerTypes)iI).setPledgedSecretaryGeneralVote(NO_TEAM);
					}
				}
			}
		}

		FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");
		setAtWar(eTeam, true);
		GET_TEAM(eTeam).setAtWar(getID(), true);

		// Plot danger cache
		GC.getMap().invalidateIsTeamBorderCache(eTeam);
		GC.getMap().invalidateIsTeamBorderCache(getID());


		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)) && GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				GET_PLAYER((PlayerTypes)iI).updatePlunder(1, false);
			}
		}

		meet(eTeam, false);

		AI_setAtPeaceCounter(eTeam, 0);
		GET_TEAM(eTeam).AI_setAtPeaceCounter(getID(), 0);

		AI_setShareWarCounter(eTeam, 0);
		GET_TEAM(eTeam).AI_setShareWarCounter(getID(), 0);

		GET_TEAM(eTeam).AI_setWarPlan(getID(), ((isNPC() || isMinorCiv()) ? WARPLAN_ATTACKED : WARPLAN_ATTACKED_RECENT), true, ((!GET_TEAM(eTeam).isNPC() || GET_TEAM(eTeam).isBarbarian()) && (!isNPC() || isBarbarian())) ? true : false);

		if (!isNPC() && !isMinorCiv() && !GET_TEAM(eTeam).isMinorCiv() && !GET_TEAM(eTeam).isNPC())
		{
			for (iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive()
				&& !GET_TEAM(eTeam).isAtWar((TeamTypes)iI) && GET_TEAM(eTeam).AI_isChosenWar((TeamTypes)iI))
				{
					GET_TEAM(eTeam).AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN);
				}
			}
		}

		if (NO_WARPLAN != eWarPlan)
		{
			AI_setWarPlan(eTeam, eWarPlan, true, bInFull);
		}

		FAssert(!(AI_isSneakAttackPreparing(eTeam)));
		if ((AI_getWarPlan(eTeam) == NO_WARPLAN) || AI_isSneakAttackPreparing(eTeam))
		{
			if (isHuman())
			{
				AI_setWarPlan(eTeam, WARPLAN_TOTAL);
			}
			else if (isMinorCiv() || isNPC() || (GET_TEAM(eTeam).getAtWarCount(true) == 1))
			{
				AI_setWarPlan(eTeam, WARPLAN_LIMITED);
			}
			else
			{
				AI_setWarPlan(eTeam, WARPLAN_DOGPILE);
			}
		}

		if (bInFull)
		{
			GC.getMap().verifyUnitValidPlot();
		}

		//isolated NPC war declarations as it happens now on each load process at a point when such validation cannot take place in the environment that isn't fully initialized yet.
		if (!GET_TEAM(eTeam).isNPC())
		{
			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					GET_PLAYER((PlayerTypes)iI).verifyUnitStacksValid();
				}
			}
		}

		GC.getGame().AI_makeAssignWorkDirty();

		if ((getID() == GC.getGame().getActiveTeam()) || (eTeam == GC.getGame().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}

		if (!isNPC() && !GET_TEAM(eTeam).isNPC() && !isMinorCiv() && !GET_TEAM(eTeam).isMinorCiv())
		{
			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive()
						&& GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
						{
							GET_PLAYER((PlayerTypes)iI).AI_setFirstContact((PlayerTypes) iJ, true);
							GET_PLAYER((PlayerTypes)iJ).AI_setFirstContact((PlayerTypes) iI, true);
						}
					}
				}
			}

			if (GC.getGame().isOption(GAMEOPTION_RUTHLESS_AI))
			{
				for (iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						//Player is on our team
						if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
						{
							//Calculate players we just backstabbed
							for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
							{
								if (GET_PLAYER((PlayerTypes)iJ).isAlive())
								{
									//Player is on the team we are declaring war on
									if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
									{
										if (GET_PLAYER((PlayerTypes)iJ).AI_getAttitude((PlayerTypes)iI) >= ATTITUDE_PLEASED)
										{
											GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount(((PlayerTypes)iI), MEMORY_BACKSTAB, 1);
										}
									}
								}
							}
							//Calculate players whose friend we just backstabbed
							for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
							{
								if (GET_PLAYER((PlayerTypes)iJ).isAlive())
								{
									//Player is not on the team we declared war on, and not on our team
									if (GET_PLAYER((PlayerTypes)iJ).getTeam() != eTeam && GET_PLAYER((PlayerTypes)iJ).getTeam() != getID())
									{
										//Friends with the leader of the team we declared war on
										if (GET_PLAYER((PlayerTypes)iJ).AI_getAttitude(GET_TEAM(eTeam).getLeaderID()) >= ATTITUDE_PLEASED)
										{
											GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount(((PlayerTypes)iI), MEMORY_BACKSTAB_FRIEND, 1);
										}
									}
								}
							}
						}
					}
				}
			}

			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive())
						{
							if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
							{
								if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
								{
									GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount(((PlayerTypes)iI), MEMORY_DECLARED_WAR, 1);

								}
								else if (GET_PLAYER((PlayerTypes)iJ).getTeam() != getID())
								{
									if (GET_TEAM(GET_PLAYER((PlayerTypes)iJ).getTeam()).isHasMet(eTeam))
									{
										if ((GET_TEAM(GET_PLAYER((PlayerTypes)iJ).getTeam()).AI_getAttitude(eTeam) >= ATTITUDE_PLEASED) && !(GET_TEAM(GET_PLAYER((PlayerTypes)iJ).getTeam()).isAtWar(eTeam)))
										{
											GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount(((PlayerTypes)iI), MEMORY_DECLARED_WAR_ON_FRIEND, 1);
										}
									}
								}
							}
						}
					}
				}
			}
		}

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if ((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam))
				{
					GET_PLAYER((PlayerTypes)iI).updateWarWearinessPercentAnger();
				}
			}
		}

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if ((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam))
				{
					GET_PLAYER((PlayerTypes)iI).updatePlotGroups();
				}
			}
		}

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if ((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam))
				{
					GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
				}
			}
		}

		// Attitude cache
		if (GC.getGame().isFinalInitialized())
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if( GET_PLAYER((PlayerTypes)iI).isAlive() )
				{
					if( GET_PLAYER((PlayerTypes)iI).getTeam() == getID() || GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam
						|| GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isAtWar(getID()) || GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isAtWar(eTeam) )
					{
						for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
						{
							if( GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).getTeam() != GET_PLAYER((PlayerTypes)iI).getTeam() )
							{
								if( GET_PLAYER((PlayerTypes)iJ).getTeam() == getID() || GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam )
								{
									GET_PLAYER((PlayerTypes)iJ).AI_invalidateAttitudeCache((PlayerTypes)iI);
									GET_PLAYER((PlayerTypes)iI).AI_invalidateAttitudeCache((PlayerTypes)iJ);
								}
							}
						}
					}
				}
			}
		}

		if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
		{
			if (bNewDiplo)
			{
				if (!isHuman())
				{
					for (iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
							{
								if (GET_PLAYER(getLeaderID()).canContact((PlayerTypes)iI))
								{
									if (GET_PLAYER((PlayerTypes)iI).isHuman())
									{
										pDiplo = new CvDiploParameters(getLeaderID());
										FAssertMsg(pDiplo != NULL, "pDiplo must be valid");
										pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_DECLARE_WAR"));
										pDiplo->setAIContact(true);
										gDLL->beginDiplomacy(pDiplo, ((PlayerTypes)iI));
									}
								}
							}
						}
					}
				}
			}

			if (!isNPC() && !(GET_TEAM(eTeam).isNPC()) &&
				  !isMinorCiv() && !(GET_TEAM(eTeam).isMinorCiv()))
			{
				for (iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						MEMORY_TRACK_EXEMPT();

						if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_DECLARED_WAR_ON", GET_TEAM(eTeam).getName().GetCString());
							AddDLLMessage(((PlayerTypes)iI), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DECLAREWAR", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
						}
						else if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_DECLARED_WAR_ON_YOU", getName().GetCString());
							AddDLLMessage(((PlayerTypes)iI), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DECLAREWAR", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
						}
						else if (GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(getID()) && GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(eTeam))
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_DECLARED_WAR", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
							AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIRDECLAREWAR", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
						}
					}
				}

				szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_DECLARES_WAR", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
			}
		}

		CvEventReporter::getInstance().changeWar(true, getID(), eTeam);

		if (!isNPC() && !GET_TEAM(eTeam).isNPC() && !isMinorCiv() && !GET_TEAM(eTeam).isMinorCiv())
		{
			if (GC.getDefineINT("BBAI_ALLIANCE_OPTION") != 1)
			{
				cancelDefensivePacts();
			}

			for (iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (iI != getID() && iI != eTeam && GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM((TeamTypes)iI).isDefensivePact(eTeam))
					{
						GET_TEAM((TeamTypes)iI).declareWar(getID(), bNewDiplo, WARPLAN_DOGPILE);
					}
					else if (GET_TEAM((TeamTypes)iI).isDefensivePact(getID()))
					{
						// For alliance option.  This teams pacts are canceled above if not using alliance option.
						GET_TEAM((TeamTypes)iI).declareWar(eTeam, bNewDiplo, WARPLAN_DOGPILE);
					}
				}
			}

			if (GC.getDefineINT("BBAI_ALLIANCE_OPTION") != 1)
			{
				GET_TEAM(eTeam).cancelDefensivePacts();
			}

			for (iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM((TeamTypes)iI).isVassal(eTeam) || GET_TEAM(eTeam).isVassal((TeamTypes)iI))
					{
						declareWar((TeamTypes)iI, bNewDiplo, AI_getWarPlan(eTeam));
					}
					else if (GET_TEAM((TeamTypes)iI).isVassal(getID()) || isVassal((TeamTypes)iI))
					{
						GET_TEAM((TeamTypes)iI).declareWar(eTeam, bNewDiplo, WARPLAN_DOGPILE);
					}
				}
			}
		}
	}
}

void CvTeam::makePeace(TeamTypes eTeam, bool bBumpUnits)
{
	CvWString szBuffer;
	int iI;

	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");

	if (isAtWar(eTeam))
	{
		if (gTeamLogLevel >= 1 && isAlive() && GET_TEAM(eTeam).isAlive())
		{
			logBBAI("Team %d (%S) and team %d (%S) make peace", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eTeam, GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).getCivilizationDescription(0));
		}

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			//	Check isEverAlive() here rather than isAlive() since it's just an optimization (updatePlunder() will
			//	do nothing for a truely dead player as they have no units), and this method is called by REV Python
			//	in response to a plyer setAlive (false) event notification DURING the processing tof that player's
			//	death.  If we do not treat THAT player as alive here their blockades will not be lifted
			if (((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)) && GET_PLAYER((PlayerTypes)iI).isEverAlive())
			{
				GET_PLAYER((PlayerTypes)iI).updatePlunder(-1, false);
			}
		}

		FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");
		setAtWar(eTeam, false);
		GET_TEAM(eTeam).setAtWar(getID(), false);

		// Plot danger cache
		GC.getMap().invalidateIsTeamBorderCache(eTeam);
		GC.getMap().invalidateIsTeamBorderCache(getID());

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)) && GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				GET_PLAYER((PlayerTypes)iI).updatePlunder(1, false);
			}
		}

		AI_setAtWarCounter(eTeam, 0);
		GET_TEAM(eTeam).AI_setAtWarCounter(getID(), 0);

		AI_setWarSuccess(eTeam, 0);
		GET_TEAM(eTeam).AI_setWarSuccess(getID(), 0);

		AI_setWarPlan(eTeam, NO_WARPLAN);
		GET_TEAM(eTeam).AI_setWarPlan(getID(), NO_WARPLAN);

		if (bBumpUnits)
		{
			GC.getMap().verifyUnitValidPlot();
		}

		GC.getGame().AI_makeAssignWorkDirty();

		if ((getID() == GC.getGame().getActiveTeam()) || (eTeam == GC.getGame().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if ((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam))
				{
					GET_PLAYER((PlayerTypes)iI).updateWarWearinessPercentAnger();
				}
			}
		}

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if ((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam))
				{
					GET_PLAYER((PlayerTypes)iI).updatePlotGroups();
				}
			}
		}

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if ((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam))
				{
					GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
				}
			}
		}

		// Attitude cache
		if (GC.getGame().isFinalInitialized())
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if( GET_PLAYER((PlayerTypes)iI).isAlive() )
				{
					if( GET_PLAYER((PlayerTypes)iI).getTeam() == getID() || GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam
						|| GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isAtWar(getID()) || GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isAtWar(eTeam) )
					{
						for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
						{
							if( GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).getTeam() != GET_PLAYER((PlayerTypes)iI).getTeam() )
							{
								if( GET_PLAYER((PlayerTypes)iJ).getTeam() == getID() || GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam )
								{
									GET_PLAYER((PlayerTypes)iJ).AI_invalidateAttitudeCache((PlayerTypes)iI);
									GET_PLAYER((PlayerTypes)iI).AI_invalidateAttitudeCache((PlayerTypes)iJ);
								}
							}
						}
					}
				}
			}
		}

		// Silence announcement when one team has no units or cities
		// Needed because civs are now set to peace with all when dieing since they keep at war state when being reborn
		bool bHasUnitsOrCities  = (getNumCities() > 0);
		bool bHasUnitsOrCities2 = (GET_TEAM(eTeam).getNumCities() > 0);

		// Could be homeless rebel, must check
		if( !bHasUnitsOrCities )
		{
			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
					{
						if( GET_PLAYER((PlayerTypes)iI).getNumUnits() > 0 )
						{
							bHasUnitsOrCities = true;
							break;
						}
					}
				}
			}
		}

		if( !bHasUnitsOrCities2 )
		{
			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
					{
						if( GET_PLAYER((PlayerTypes)iI).getNumUnits() > 0 )
						{
							bHasUnitsOrCities2 = true;
							break;
						}
					}
				}
			}
		}

		// Anounce only if neither is minor
		if( !isMinorCiv() && !GET_TEAM(eTeam).isMinorCiv() )
		{
			// Announce if both are really alive
			if( bHasUnitsOrCities && bHasUnitsOrCities2 )
			{
				for (iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						MEMORY_TRACK_EXEMPT();

						if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_MADE_PEACE_WITH", GET_TEAM(eTeam).getName().GetCString());
							AddDLLMessage(((PlayerTypes)iI), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_MAKEPEACE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
						}
						else if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_MADE_PEACE_WITH", getName().GetCString());
							AddDLLMessage(((PlayerTypes)iI), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_MAKEPEACE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
						}
						else if (GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(getID()) && GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(eTeam))
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_MADE_PEACE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
							AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIRMAKEPEACE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
						}
					}
				}

				szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_MADE_PEACE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
			}
		}

		CvEventReporter::getInstance().changeWar(false, getID(), eTeam);

		for (iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (iI != getID() && iI != eTeam)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM((TeamTypes)iI).isVassal(eTeam))
					{
						GET_TEAM((TeamTypes)iI).makePeace(getID(), bBumpUnits);
					}
					else if (GET_TEAM((TeamTypes)iI).isVassal(getID()))
					{
						GET_TEAM((TeamTypes)iI).makePeace(eTeam, bBumpUnits);
					}
				}
			}
		}

	}
}


bool CvTeam::canContact(TeamTypes eTeam) const
{
	int iI, iJ;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
						{
							if (GET_PLAYER((PlayerTypes)iI).canContact((PlayerTypes)iJ))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}


void CvTeam::meet(TeamTypes eTeam, bool bNewDiplo)
{
	if (!isHasMet(eTeam))
	{
		makeHasMet(eTeam, bNewDiplo);
		GET_TEAM(eTeam).makeHasMet(getID(), bNewDiplo);

		if (gTeamLogLevel >= 2 && GC.getGame().isFinalInitialized() && eTeam != getID() && isAlive() && GET_TEAM(eTeam).isAlive())
		{
			logBBAI("    Team %d (%S) meets team %d (%S)", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eTeam, GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).getCivilizationDescription(0) );
		}
	}
}


void CvTeam::signOpenBorders(TeamTypes eTeam)
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	if (!isAtWar(eTeam) && (getID() != eTeam))
	{
		setTradeItem(&item, TRADE_OPEN_BORDERS);

		if (GET_PLAYER(getLeaderID()).canTradeItem(GET_TEAM(eTeam).getLeaderID(), item) && GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).canTradeItem(getLeaderID(), item))
		{
			ourList.clear();
			theirList.clear();

			ourList.insertAtEnd(item);
			theirList.insertAtEnd(item);

			GC.getGame().implementDeal(getLeaderID(), (GET_TEAM(eTeam).getLeaderID()), &ourList, &theirList);
		}
	}
}

void CvTeam::signDefensivePact(TeamTypes eTeam)
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	if (!isAtWar(eTeam) && (getID() != eTeam))
	{
		setTradeItem(&item, TRADE_DEFENSIVE_PACT);

		if (GET_PLAYER(getLeaderID()).canTradeItem(GET_TEAM(eTeam).getLeaderID(), item) && GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).canTradeItem(getLeaderID(), item))
		{
			ourList.clear();
			theirList.clear();

			ourList.insertAtEnd(item);
			theirList.insertAtEnd(item);

			GC.getGame().implementDeal(getLeaderID(), (GET_TEAM(eTeam).getLeaderID()), &ourList, &theirList);
		}
	}
}

bool CvTeam::canSignDefensivePact(TeamTypes eTeam) const
{
	if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY) && !isHasEmbassy(eTeam))
	{
		return false;
	}

	for (int iTeam = 0; iTeam < MAX_PC_TEAMS; ++iTeam)
	{
		if (iTeam != getID() && iTeam != eTeam)
		{
			CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeam);
			if (kLoopTeam.isPermanentWarPeace(eTeam) != kLoopTeam.isPermanentWarPeace(getID()))
			{
				return false;
			}

			if (isPermanentWarPeace((TeamTypes)iTeam) != GET_TEAM(eTeam).isPermanentWarPeace((TeamTypes)iTeam))
			{
				return false;
			}
		}
	}
	return true;
}

int CvTeam::getAssets() const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getAssets();
		}
	}

	return iCount;
}

int CvTeam::getPower(bool bIncludeVassals) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive() && (kLoopPlayer.getTeam() == getID() || (bIncludeVassals && GET_TEAM(kLoopPlayer.getTeam()).isVassal(getID()))))
		{
			iCount += kLoopPlayer.getPower();

			if (GET_TEAM(kLoopPlayer.getTeam()).isVassal(getID()))
			{
				iCount -= kLoopPlayer.getTechPower();
			}
		}
	}

	return iCount;
}

int CvTeam::getDefensivePower(TeamTypes eExcludeTeam) const
{
	int iCount = 0;

	FAssert(eExcludeTeam != getID());

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (kLoopTeam.isAlive() && !kLoopTeam.isAVassal())
		{
			// K-Mod: added "eExcludeTeam" argument, so that defensive power can take into account the cancelation of pacts.
			if (iI != eExcludeTeam && (getID() == iI || isVassal((TeamTypes)iI) || isDefensivePact((TeamTypes)iI)))
			{
				iCount += kLoopTeam.getPower(true);
			}
		}
	}

	return iCount;
}

int CvTeam::getEnemyPower() const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (kLoopTeam.isAlive() && (getID() != iI && isAtWar((TeamTypes)iI)))
		{
			iCount += kLoopTeam.getPower(false);
		}
	}

	return iCount;
}

int CvTeam::getNumNukeUnits() const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive() && (kLoopPlayer.getTeam() == getID() || GET_TEAM(kLoopPlayer.getTeam()).isVassal(getID())))
		{
			iCount += kLoopPlayer.getNumNukeUnits();
		}
	}

	return iCount;
}

// Toffer - Used by barbarian unit spawning "CvGame::createBarbarianUnits()",
//		and for the new game starting units "CvPlayer::addStartUnitAI(...)".
// Barbs don't need the bonus req for units, but they must have the tech that enables it.
bool CvTeam::isUnitBonusEnabledByTech(const CvUnitInfo& unit, const bool bNoWorldBonuses) const
{
	if (unit.getPrereqAndBonus() != NO_BONUS)
	{
		if (!isHasTech((TechTypes)GC.getBonusInfo((BonusTypes)unit.getPrereqAndBonus()).getTechCityTrade())
		|| bNoWorldBonuses // Toffer - TODO - Add maxGlobalInstances tag to CvBonusInfo.
		&& GC.getBonusInfo((BonusTypes)unit.getPrereqAndBonus()).getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_CULTURE"))
		{
			return false;
		}
	}
	bool bMet = true;
	for (int iI = 0; iI < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); ++iI)
	{
		if (NO_BONUS != unit.getPrereqOrBonuses(iI))
		{
			if (isHasTech((TechTypes)GC.getBonusInfo((BonusTypes)unit.getPrereqOrBonuses(iI)).getTechCityTrade())
			&& (!bNoWorldBonuses || GC.getBonusInfo((BonusTypes)unit.getPrereqOrBonuses(iI)).getBonusClassType() != GC.getInfoTypeForString("BONUSCLASS_CULTURE")))
			{
				bMet = true;
				break;
			}
			bMet = false;
		}
	}
	return bMet;
}


int CvTeam::getVotes(VoteTypes eVote, VoteSourceTypes eVoteSource) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.getTeam() == getID() && kLoopPlayer.isAlive())
		{
			iCount += kLoopPlayer.getVotes(eVote, eVoteSource);
		}
	}

	return iCount;
}

bool CvTeam::isVotingMember(VoteSourceTypes eVoteSource) const
{
	return (getVotes(NO_VOTE, eVoteSource) > 0);
}

bool CvTeam::isFullMember(VoteSourceTypes eVoteSource) const
{
	if (isForceTeamVoteEligible(eVoteSource))
	{
		return true;
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.getTeam() == getID() && kLoopPlayer.isAlive())
		{
			if (!kLoopPlayer.isFullMember(eVoteSource))
			{
				return false;
			}
		}
	}

	return true;
}

int CvTeam::getAtWarCount(bool bIgnoreMinors, bool bIgnoreVassals) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (!bIgnoreMinors || !(GET_TEAM((TeamTypes)iI).isMinorCiv()))
			{
				if( !bIgnoreVassals || !(GET_TEAM((TeamTypes)iI).isAVassal()))
				{
					if (isAtWar((TeamTypes)iI))
					{
						FAssert(iI != getID());
						FAssert(!(AI_isSneakAttackPreparing((TeamTypes)iI)));
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}

int CvTeam::getWarPlanCount(WarPlanTypes eWarPlan, bool bIgnoreMinors) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (!bIgnoreMinors || !(GET_TEAM((TeamTypes)iI).isMinorCiv()))
			{
				if (AI_getWarPlan((TeamTypes)iI) == eWarPlan)
				{
					FAssert(iI != getID());
					iCount++;
				}
			}
		}
	}

	return iCount;
}


int CvTeam::getAnyWarPlanCount(bool bIgnoreMinors) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (!bIgnoreMinors || !(GET_TEAM((TeamTypes)iI).isMinorCiv()))
			{
				if (AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
				{
					FAssert(iI != getID());
					iCount++;
				}
			}
		}
	}

	FAssert(iCount >= getAtWarCount(bIgnoreMinors));

	return iCount;
}


int CvTeam::getChosenWarCount(bool bIgnoreMinors) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (!bIgnoreMinors || !(GET_TEAM((TeamTypes)iI).isMinorCiv()))
			{
				if (AI_isChosenWar((TeamTypes)iI))
				{
					FAssert(iI != getID());
					iCount++;
				}
			}
		}
	}

	return iCount;
}


int CvTeam::getHasMetCivCount(bool bIgnoreMinors) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isHasMet((TeamTypes)iI))
		{
			if (!bIgnoreMinors || !GET_TEAM((TeamTypes)iI).isMinorCiv())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


bool CvTeam::hasMetHuman() const
{
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && GET_TEAM((TeamTypes)iI).isHuman() && isHasMet((TeamTypes)iI))
		{
			return true;
		}
	}

	return false;
}


int CvTeam::getDefensivePactCount(TeamTypes eTeam) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isDefensivePact((TeamTypes)iI))
		{
			if (NO_TEAM == eTeam || GET_TEAM(eTeam).isHasMet((TeamTypes)iI))
			{
				iCount++;
			}
		}
	}

	return iCount;
}

int CvTeam::getVassalCount(TeamTypes eTeam) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		const CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (kLoopTeam.isAlive() && iI != getID() && kLoopTeam.isVassal(getID()))
		{
			if (NO_TEAM == eTeam || GET_TEAM(eTeam).isHasMet((TeamTypes)iI))
			{
				iCount++;
			}
		}
	}

	return iCount;
}

bool CvTeam::isAVassal() const
{
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isVassal((TeamTypes)iI))
		{
			FAssert(iI != getID());
			return true;
		}
	}

	return false;
}

bool CvTeam::canVassalRevolt(TeamTypes eMaster) const
{
	FAssert(NO_TEAM != eMaster);

	const CvTeam& kMaster = GET_TEAM(eMaster);

	if (isVassal(eMaster))
	{
		if (100 * getTotalLand(false) < GC.getDefineINT("VASSAL_REVOLT_OWN_LOSSES_FACTOR") * getVassalPower())
		{
			return true;
		}

		if (100 * kMaster.getTotalLand() < GC.getDefineINT("VASSAL_REVOLT_MASTER_LOSSES_FACTOR") * getMasterPower())
		{
			return true;
		}
	}

	if (GC.getDefineINT("FREE_VASSAL_LAND_PERCENT") < 0 ||
		100 * getTotalLand(false) < kMaster.getTotalLand(false) * GC.getDefineINT("FREE_VASSAL_LAND_PERCENT"))
	{
		return false;
	}

	if (GC.getDefineINT("FREE_VASSAL_POPULATION_PERCENT") < 0 ||
		100 * getTotalPopulation(false) < kMaster.getTotalPopulation(false) * GC.getDefineINT("FREE_VASSAL_POPULATION_PERCENT"))
	{
		return false;
	}

	return true;
}


int CvTeam::getCurrentMasterPower(bool bIncludeVassals) const
{
	if( isAVassal() )
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isVassal((TeamTypes)iI))
			{
				return GET_TEAM((TeamTypes)iI).getPower(bIncludeVassals);
			}
		}
	}

	// Should never get here
	FAssert(false);
	return 0;
}

bool CvTeam::isMasterPlanningLandWar(const CvArea* pArea) const
{
	if( !isAVassal() )
	{
		return false;
	}

	if( (pArea->getAreaAIType(getID()) == AREAAI_OFFENSIVE) || (pArea->getAreaAIType(getID()) == AREAAI_DEFENSIVE) || (pArea->getAreaAIType(getID()) == AREAAI_MASSING) )
	{
		return true;
	}

	for( int iI = 0; iI < MAX_PC_TEAMS; iI++ )
	{
		if( isVassal((TeamTypes)iI) )
		{
			if( GET_TEAM((TeamTypes)iI).getAnyWarPlanCount(true) > 0 )
			{
				if( (pArea->getAreaAIType((TeamTypes)iI) == AREAAI_OFFENSIVE) || (pArea->getAreaAIType((TeamTypes)iI) == AREAAI_DEFENSIVE) || (pArea->getAreaAIType((TeamTypes)iI) == AREAAI_MASSING) )
				{
					return true;
				}
				else if( pArea->getAreaAIType((TeamTypes)iI) == AREAAI_NEUTRAL )
				{
					// Master has no presence here
					if( (pArea->getNumCities() - countNumCitiesByArea(pArea)) > 2 )
					{
						return (GC.getGame().getSorenRandNum((isCapitulated() ? 6 : 4),"Vassal land war") == 0);
					}
				}
			}
			else if( GET_TEAM((TeamTypes)iI).isHuman() )
			{
				if( GC.getBBAI_HUMAN_VASSAL_WAR_BUILD() )
				{
					if( (pArea->getNumCities() - countNumCitiesByArea(pArea) - GET_TEAM((TeamTypes)iI).countNumCitiesByArea(pArea)) > 2 )
					{
						return (GC.getGame().getSorenRandNum(4,"Vassal land war") == 0);
					}
				}
			}

			break;
		}
	}

	return false;
}

bool CvTeam::isMasterPlanningSeaWar(const CvArea* pArea) const
{
	if( !isAVassal() )
	{
		return false;
	}

	if( (pArea->getAreaAIType(getID()) == AREAAI_ASSAULT) || (pArea->getAreaAIType(getID()) == AREAAI_ASSAULT_ASSIST) || (pArea->getAreaAIType(getID()) == AREAAI_ASSAULT_MASSING) )
	{
		return true;
	}

	for( int iI = 0; iI < MAX_PC_TEAMS; iI++ )
	{
		if( isVassal((TeamTypes)iI) )
		{
			if( GET_TEAM((TeamTypes)iI).getAnyWarPlanCount(true) > 0 )
			{
				if( (pArea->getAreaAIType((TeamTypes)iI) == AREAAI_ASSAULT) || (pArea->getAreaAIType((TeamTypes)iI) == AREAAI_ASSAULT_ASSIST) || (pArea->getAreaAIType((TeamTypes)iI) == AREAAI_ASSAULT_MASSING) )
				{
					return (GC.getGame().getSorenRandNum((isCapitulated() ? 3 : 2),"Vassal sea war") == 0);
				}
				else if( pArea->getAreaAIType((TeamTypes)iI) == AREAAI_NEUTRAL )
				{
					// Master has no presence here
					return false;
				}

			}
			else if( GET_TEAM((TeamTypes)iI).isHuman() )
			{
				return false;
			}

			break;
		}
	}

	return false;
}


int CvTeam::getUnitMaking(UnitTypes eIndex) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getUnitMaking(eIndex);
		}
	}
	return iCount;
}


int CvTeam::getUnitCountPlusMaking(UnitTypes eIndex) const
{
	return (getUnitCount(eIndex) + getUnitMaking(eIndex));
}


int CvTeam::getBuildingMaking(BuildingTypes eBuilding) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getBuildingMaking(eBuilding);
		}
	}

	return iCount;
}


int CvTeam::getBuildingCountPlusMaking(BuildingTypes eIndex) const
{
	return (getBuildingCount(eIndex) + getBuildingMaking(eIndex));
}


int CvTeam::getHasReligionCount(ReligionTypes eReligion) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getHasReligionCount(eReligion);
		}
	}

	return iCount;
}


int CvTeam::getHasCorporationCount(CorporationTypes eCorporation) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getHasCorporationCount(eCorporation);
		}
	}

	return iCount;
}

int CvTeam::processedTeamCulture() const
{
	const unsigned long long iCount = countTotalCulture();
	return (iCount < MAX_INT) ? (int)iCount : MAX_INT;
}


unsigned long long CvTeam::countTotalCulture() const
{
	unsigned long long iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).countTotalCulture();
		}
	}

	return iCount;
}


int CvTeam::countNumUnitsByArea(const CvArea* pArea) const
{
	PROFILE_FUNC();

	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += pArea->getUnitsPerPlayer((PlayerTypes)iI);
		}
	}

	return iCount;
}


int CvTeam::countNumCitiesByArea(const CvArea* pArea) const
{
	PROFILE_FUNC();

	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += pArea->getCitiesPerPlayer((PlayerTypes)iI);
		}
	}

	return iCount;
}


int CvTeam::countTotalPopulationByArea(const CvArea* pArea) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += pArea->getPopulationPerPlayer((PlayerTypes)iI);
		}
	}

	return iCount;
}


int CvTeam::countPowerByArea(const CvArea* pArea) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += pArea->getPower((PlayerTypes)iI);
		}
	}

	return iCount;
}


int CvTeam::countEnemyPowerByArea(const CvArea* pArea) const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() != getID()
		&& AI_getWarPlan(GET_PLAYER((PlayerTypes)iI).getTeam()) != NO_WARPLAN)
		{
			iCount += pArea->getPower((PlayerTypes)iI);
		}
	}
	return iCount;
}

int CvTeam::countEnemyPopulationByArea(const CvArea* pArea) const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() != getID())
		{
			const WarPlanTypes eWarPlan = AI_getWarPlan(GET_PLAYER((PlayerTypes)iI).getTeam());
			if( eWarPlan != NO_WARPLAN )
			{
				int iContribution = pArea->getPopulationPerPlayer((PlayerTypes)iI);

				if ( eWarPlan == WARPLAN_LIMITED && (isMinorCiv() || GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isMinorCiv()) )
				{
					iContribution /= 2;
				}

				iCount += iContribution;
			}
		}
	}
	return iCount;
}

int CvTeam::countNumAIUnitsByArea(const CvArea* pArea, UnitAITypes eUnitAI) const
{
	PROFILE_FUNC();

	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iCount += pArea->getNumAIUnits(((PlayerTypes)iI), eUnitAI);
		}
	}

	return iCount;
}

int CvTeam::countEnemyDangerByArea(const CvArea* pArea, TeamTypes eEnemyTeam ) const
{
	PROFILE_FUNC();

	int iCount = 0;
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot != NULL && pLoopPlot->area() == pArea && pLoopPlot->getTeam() == getID())
		{
			//	Koshling - don't count animals - they are not actively attacking us so they shouldn't trigger things like a defensive stance
			iCount += pLoopPlot->plotCount(PUF_canDefendEnemyNoAnimal, getLeaderID(), 0, NULL, NO_PLAYER, eEnemyTeam, PUF_isVisible, getLeaderID());
		}
	}
	return iCount;
}


int CvTeam::getResearchCost(TechTypes eTech) const
{
	FAssertMsg(eTech != NO_TECH, "Tech is not assigned a valid value");

	//This whole function needs to be converted to an unsigned long long.  That's quite a task but should be possible and if it doesn, we should be able to make it to the end of the game with the base modified even at the extremest points.
	int iInitialCost = GC.getTechInfo(eTech).getResearchCost();
	if (iInitialCost < 0)
	{
		return -1;
	}

	unsigned long long iCost = (unsigned long long)iInitialCost;

	iCost *= 100;

	int iBeelineStingsTechCostModifier = 0;
	if (GC.getGame().isOption(GAMEOPTION_BEELINE_STINGS))
	{
		int iTechEra = GC.getTechInfo(eTech).getEra();
		int iPlayerEra = MAXINT;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iPlayerEra = (int)GET_PLAYER((PlayerTypes)iI).getCurrentEra();
				break;
			}
		}
		FAssertMsg(iPlayerEra != MAXINT, "No player found on team!");

		if (iTechEra < iPlayerEra)
		{
			for (int iI = 0; iI < GC.getNumEraInfos(); iI++)
			{
				int iEvalEra = iI;
				EraTypes eEvalEra = (EraTypes)iI;
				if (iEvalEra >= iTechEra && iEvalEra < iPlayerEra)
				{
					iBeelineStingsTechCostModifier += GC.getEraInfo(eEvalEra).getBeelineStingsTechCostModifier();
					//just need to add the tag iBeelineStingsModifier to Era Infos.
				}
				else if (iEvalEra >= iPlayerEra)
				{
					break;
				}
			}
		}
	}

	int iModifier = GC.getTECH_COST_MODIFIER();
	iCost *= iModifier;
	iCost /= 100;

	iModifier = GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getResearchPercent();
	iCost *= iModifier;
	iCost /= 100;

	iModifier = GC.getHandicapInfo(getHandicapType()).getResearchPercent();
	iCost *= iModifier;
	iCost /= 100;

	iModifier = GC.getWorldInfo(GC.getMap().getWorldSize()).getResearchPercent();
	iCost *= iModifier;
	iCost /= 100;

	iModifier = GC.getEraInfo((EraTypes)GC.getTechInfo(eTech).getEra()).getResearchPercent();
	iCost *= iModifier;
	iCost /= 100;

	iModifier = iBeelineStingsTechCostModifier + 100;
	iCost *= iModifier;
	iCost /= 100;

	iModifier = std::max(0, (GC.getDefineINT("TECH_COST_EXTRA_TEAM_MEMBER_MODIFIER") * getNumMembers()));
	iCost *= iModifier;
	iCost /= 100;

	if (!isHuman() && !isNPC())
	{
		iModifier = GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIResearchPercent();
		iCost *= iModifier;
		iCost /= 100;
	}

	if (GC.getGame().isOption(GAMEOPTION_UPSCALED_RESEARCH_COSTS))
	{
		iModifier = GC.getUPSCALED_RESEARCH_COST_MODIFIER() + 100;
		iCost *= iModifier;
		iCost /= 100;
	}

	iCost /= 100;

	int iFinalCost = MAX_INT;
	if (iCost < MAX_INT)
	{
		iFinalCost = (int)iCost;
	}
	return std::max(1, iFinalCost);
}


int CvTeam::getResearchLeft(TechTypes eTech) const
{
	return std::max(0, (getResearchCost(eTech) - getResearchProgress(eTech)));
}


bool CvTeam::hasHolyCity(ReligionTypes eReligion) const
{
	FAssertMsg(eReligion != NO_RELIGION, "Religion is not assigned a valid value");

	const CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);

	if (pHolyCity != NULL)
	{
		return (pHolyCity->getTeam() == getID());
	}

	return false;
}


bool CvTeam::hasHeadquarters(CorporationTypes eCorporation) const
{
	FAssertMsg(eCorporation != NO_CORPORATION, "Corporation is not assigned a valid value");

	const CvCity* pHeadquarters = GC.getGame().getHeadquarters(eCorporation);

	if (pHeadquarters != NULL)
	{
		return (pHeadquarters->getTeam() == getID());
	}

	return false;
}

bool CvTeam::hasBonus(BonusTypes eBonus) const
{
	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).hasBonus(eBonus))
		{
			return true;
		}
	}

	return false;
}

bool CvTeam::isBonusObsolete(BonusTypes eBonus) const
{
	TechTypes eObsoleteTech = (TechTypes) GC.getBonusInfo(eBonus).getTechObsolete();
	if (eObsoleteTech != NO_TECH && isHasTech(eObsoleteTech))
	{
		return true;
	}
	return false;
}


bool CvTeam::isHuman() const
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
		{
			return true;
		}
	}

	return false;
}


bool CvTeam::isBarbarian() const
{
	return (getID() == BARBARIAN_TEAM);
}

bool CvTeam::isNPC() const
{
	return (getID() >= MAX_PC_TEAMS);
}

bool CvTeam::isHominid() const
{
	return (getID() == BARBARIAN_TEAM || getID() == NEANDERTHAL_TEAM);
}


bool CvTeam::isMinorCiv() const
{
	bool bValid = false;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).isMinorCiv())
			{
				bValid = true;
			}
			else
			{
				return false;
			}
		}
	}

	return bValid;
}

void CvTeam::setIsMinorCiv(bool bNewValue, bool bDoBarbCivCheck)
{
	if (bNewValue != isMinorCiv())
	{
		if (isAlive())
		{
			if (bNewValue)
			{
				logMsg("Switching team %d to minor", getID());
			}
			else
			{
				logMsg("Switching minor team %d to full", getID());
			}
		}
		else
		{
			if (bNewValue)
			{
				logMsg("Setting non-alive team %d to minor", getID());
			}
			else
			{
				logMsg("Setting non-alive minor team %d to full", getID());
			}
		}
		bool abHasMet[MAX_TEAMS];
		TeamTypes eBarbCivVictim = NO_TEAM;

		// Have to check hasMet status first because of vassal states ... dow on master causes hasMet for vassal
		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			abHasMet[iI] = isHasMet((TeamTypes)iI);
		}

		if (isAlive())
		{
			if (bDoBarbCivCheck && !bNewValue)
			{
				int iMaxVal = 0;
				for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
				{
					if (iJ != getID())
					{
						int iValue = GET_TEAM(getID()).AI_getBarbarianCivWarVal((TeamTypes)iJ, 12);
						if (iValue > iMaxVal)
						{
							logMsg("    BarbCiv team %d is considering declaring war against victim Team %d", getID(), iJ);
							CvCity* pCapital = GET_PLAYER(getLeaderID()).getCapitalCity();

							if (pCapital == NULL || pCapital->plot()->isHasPathToPlayerCity(getID(), GET_TEAM((TeamTypes)iJ).getLeaderID()))
							{
								iMaxVal = iValue;
								eBarbCivVictim = (TeamTypes)iJ;
							}
						}
					}
				}
				logMsg("    BarbCiv team %d will declare war against victim Team %d", getID(), eBarbCivVictim);
			}
		}

		if (bNewValue)
		{
			// Convert all team members
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					GC.getInitCore().setMinorNationCiv((PlayerTypes)iI, bNewValue);
				}
			}
			// Declare war on all outside teams
			declareWarAsMinor();
		}
		else
		{
			// Keep war againt those this team has met
			for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
			{
				if (iI != getID() && GET_TEAM((TeamTypes)iI).isAlive() && !GET_TEAM((TeamTypes)iI).isMinorCiv())
				{
					if (abHasMet[iI])
					{
						if (GC.getGame().isOption(GAMEOPTION_START_AS_MINORS))
						{
							if (isAlive() && !isAtWar((TeamTypes)iI))
							{
								declareWar((TeamTypes)iI, true, NO_WARPLAN);
							}
						}
						else
						{
							// Does other player want to keep war with us?
							bool bPeace = true;
							bool bPlanWar = false;

							const CvPlayer& kPlayer = GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID());
							int iCount = 0;
							foreach_(const CvUnit* pLoopUnit, kPlayer.units())
							{
								if (pLoopUnit->plot()->getTeam() == getID())
								{
									iCount++;
								}
							}

							if (GET_TEAM((TeamTypes)iI).isHuman())
							{
								if (iCount > 2)
								{
									bPeace = false;
								}
							}
							else
							{
								if (!isAtWar((TeamTypes)iI))
								{
									FAssertMsg(false, "!isAtWar((TeamTypes)iI); Shouldn't happen here");
									bPeace = false;
								}
								else if (GET_TEAM((TeamTypes)iI).AI_minorKeepWarVal(getID()) > 0)
								{
									if (GET_TEAM((TeamTypes)iI).AI_endWarVal(getID()) < GET_TEAM(getID()).AI_endWarVal((TeamTypes)iI) / 2)
									{
										if (iCount > 2 || GET_TEAM((TeamTypes)iI).AI_getWarSuccess(getID()) > GC.getDefineINT("WAR_SUCCESS_CITY_CAPTURING"))
										{
											bPeace = false;
										}
										else
										{
											bPlanWar = true;
										}
									}
								}
							}

							if (bPeace)
							{
								makePeace((TeamTypes)iI, false);

								setHasMet( (TeamTypes)iI, false );
								GET_TEAM((TeamTypes)iI).setHasMet( getID(), false );

								if (bPlanWar)
								{
									GET_TEAM((TeamTypes)iI).AI_setWarPlan(getID(),WARPLAN_LIMITED);
									logMsg("    Team %d decides to plan war against ex-minor", iI);
								}
							}
							else
							{
								logMsg("    Team %d decides to keep war against ex-minor", iI);
							}
						}
					}
					else
					{
						if (isAtWar((TeamTypes)iI) && !GET_TEAM((TeamTypes)iI).isMinorCiv())
						{
							makePeace((TeamTypes)iI, false);
							setHasMet((TeamTypes)iI, false);
							GET_TEAM((TeamTypes)iI).setHasMet(getID(), false);
						}
					}

					if (isAtWar((TeamTypes)iI))
					{
						AI_setWarPlan((TeamTypes)iI, WARPLAN_LIMITED, true);
						GET_TEAM((TeamTypes)iI).AI_setWarPlan(getID(), WARPLAN_LIMITED, true);
					}
				}
			}

			if (eBarbCivVictim != NO_TEAM)
			{
				if (!isAtWar(eBarbCivVictim))
				{
					if (getAnyWarPlanCount(true) == 0)
					{
						int iCount = 0;
						foreach_(const CvUnit* pLoopUnit, GET_PLAYER(getLeaderID()).units())
						{
							if (pLoopUnit->plot()->getTeam() == eBarbCivVictim)
							{
								iCount++;
							}
						}

						if (iCount > 2 || GET_TEAM((TeamTypes)iI).AI_getWarSuccess(getID()) > GC.getDefineINT("WAR_SUCCESS_CITY_CAPTURING"))
						{
							logMsg("  Barb civ %d decides to keep war on victim Team %d", getID(), eBarbCivVictim);
							declareWar((TeamTypes)iI, true, WARPLAN_TOTAL);
						}
						else
						{
							logMsg("  Barb civ %d begins preparing for war on victim Team %d", getID(), eBarbCivVictim);
							// Prepare for war with victim
							AI_setWarPlan(eBarbCivVictim,WARPLAN_TOTAL);
						}
					}
				}
				else
				{
					AI_setWarPlan(eBarbCivVictim, WARPLAN_TOTAL, true);
				}
			}

			// Convert all team members
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					GC.getInitCore().setMinorNationCiv((PlayerTypes)iI, bNewValue);
				}
			}
		}
		// Update graphics to reflect changes
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GlobeInfo_DIRTY_BIT, true);
	}
	return;
}

void	CvTeam::declareWarAsMinor()
{
	if ( isAlive() )
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			// Declare war on everyone except barbs, barbs always do their own declaring
			if( iI != getID() && !isAtWar((TeamTypes)iI) )
			{
				bool bHasMet = isHasMet((TeamTypes)iI);

				declareWar((TeamTypes)iI, false, WARPLAN_LIMITED);
				if( !bHasMet )
				{
					// Declaring war creates contacts, remove those the player didn't have already
					setHasMet( (TeamTypes)iI, false );
					GET_TEAM((TeamTypes)iI).setHasMet( getID(), false );
				}
			}

			if( isAtWar((TeamTypes)iI) )
			{
				AI_setWarPlan((TeamTypes)iI, WARPLAN_LIMITED, true);
				GET_TEAM((TeamTypes)iI).AI_setWarPlan(getID(), WARPLAN_LIMITED, true);
			}
		}
	}
}


PlayerTypes CvTeam::getLeaderID() const
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			return ((PlayerTypes)iI);
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			return ((PlayerTypes)iI);
		}
	}

	return NO_PLAYER;
}


PlayerTypes CvTeam::getSecretaryID() const
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			return ((PlayerTypes)iI);
		}
	}

	return getLeaderID();
}


HandicapTypes CvTeam::getHandicapType() const
{
	int iGameHandicap = 0;
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iGameHandicap += GET_PLAYER((PlayerTypes)iI).getHandicapType();
			iCount++;
		}
	}

	if (iCount > 0)
	{
		FAssertMsg((iGameHandicap / iCount) >= 0, "(iGameHandicap / iCount) is expected to be non-negative (invalid Index)");
		return ((HandicapTypes)(iGameHandicap / iCount));
	}
	else
	{
		return ((HandicapTypes)(GC.getDefineINT("STANDARD_HANDICAP")));
	}
}


CvWString CvTeam::getName() const
{
	CvWString szBuffer;
	bool bFirst = true;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			setListHelp(szBuffer, L"", GET_PLAYER((PlayerTypes)iI).getName(), L"/", bFirst);
			bFirst = false;
		}
	}

	return szBuffer;
}


int CvTeam::getNumMembers() const
{
	return m_iNumMembers;
}


void CvTeam::changeNumMembers(int iChange)
{
	m_iNumMembers += iChange;
	FAssert(getNumMembers() >= 0);
}


int CvTeam::getAliveCount() const
{
	return m_iAliveCount;
}


int CvTeam::isAlive() const
{
	return (getAliveCount() > 0);
}


void CvTeam::changeAliveCount(int iChange)
{
	m_iAliveCount += iChange;
	FAssert(getAliveCount() >= 0);

	// free vassals
	if (0 == m_iAliveCount)
	{
		for (int iTeam = 0; iTeam < MAX_TEAMS; iTeam++)
		{
			if (iTeam != getID())
			{
				CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeam);
				if (kLoopTeam.isAlive() && kLoopTeam.isVassal(getID()))
				{
					kLoopTeam.setVassal(getID(), false, false);
				}
			}
		}
	}
}


int CvTeam::getEverAliveCount() const
{
	return m_iEverAliveCount;
}


int CvTeam::isEverAlive() const
{
	return (getEverAliveCount() > 0);
}


void CvTeam::changeEverAliveCount(int iChange)
{
	m_iEverAliveCount += iChange;
	FAssert(getEverAliveCount() >= 0);
}


int CvTeam::getNumCities() const
{
	return m_iNumCities;
}


void CvTeam::changeNumCities(int iChange)
{
	m_iNumCities += iChange;
	FAssert(getNumCities() >= 0);
}


int CvTeam::getTotalPopulation(bool bCheckVassals) const
{
	int iVassalPop = 0;

	if (bCheckVassals)
	{
		if (isAVassal())
		{
			return m_iTotalPopulation / 2;
		}

		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if ((TeamTypes)iI != getID())
			{
				CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
				if (kLoopTeam.isAlive() && kLoopTeam.isVassal(getID()))
				{
					iVassalPop += kLoopTeam.getTotalPopulation(false) / 2;
				}
			}
		}
	}

	return (m_iTotalPopulation + iVassalPop);
}


void CvTeam::changeTotalPopulation(int iChange)
{
	m_iTotalPopulation += iChange;
	FAssert(getTotalPopulation() >= 0);
}


int CvTeam::getTotalLand(bool bCheckVassals) const
{
	int iVassalLand = 0;

	if (bCheckVassals)
	{
		if (isAVassal())
		{
			return m_iTotalLand / 2;
		}

		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if ((TeamTypes)iI != getID())
			{
				CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
				if (kLoopTeam.isAlive() && kLoopTeam.isVassal(getID()))
				{
					iVassalLand += kLoopTeam.getTotalLand(false) / 2;
				}
			}
		}
	}

	return (m_iTotalLand + iVassalLand);
}


void CvTeam::changeTotalLand(int iChange)
{
	m_iTotalLand += iChange;
	FAssert(getTotalLand() >= 0);
}


int CvTeam::getNukeInterception() const
{
	return m_iNukeInterception;
}


void CvTeam::changeNukeInterception(int iChange)
{
	m_iNukeInterception += iChange;
	FAssert(getNukeInterception() >= 0);
}


int CvTeam::getForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource) const
{
	return m_aiForceTeamVoteEligibilityCount[eVoteSource];
}


bool CvTeam::isForceTeamVoteEligible(VoteSourceTypes eVoteSource) const
{
	return ((getForceTeamVoteEligibilityCount(eVoteSource) > 0) && !isMinorCiv());
}


void CvTeam::changeForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource, int iChange)
{
	m_aiForceTeamVoteEligibilityCount[eVoteSource] += iChange;
	FAssert(getForceTeamVoteEligibilityCount(eVoteSource) >= 0);
}


int CvTeam::getExtraWaterSeeFromCount() const
{
	return m_iExtraWaterSeeFromCount;
}


bool CvTeam::isExtraWaterSeeFrom() const
{
	return (getExtraWaterSeeFromCount() > 0);
}


void CvTeam::changeExtraWaterSeeFromCount(int iChange)
{
	if (iChange != 0)
	{
		GC.getMap().updateSight(false);

		m_iExtraWaterSeeFromCount = (m_iExtraWaterSeeFromCount + iChange);
		FAssert(getExtraWaterSeeFromCount() >= 0);

		GC.getMap().updateSight(true);
	}
}


int CvTeam::getMapTradingCount() const
{
	return m_iMapTradingCount;
}


bool CvTeam::isMapTrading()	const
{
	return (getMapTradingCount() > 0);
}


void CvTeam::changeMapTradingCount(int iChange)
{
	m_iMapTradingCount = (m_iMapTradingCount + iChange);
	FAssert(getMapTradingCount() >= 0);
}


int CvTeam::getTechTradingCount() const
{
	return m_iTechTradingCount;
}


bool CvTeam::isTechTrading() const
{
	return (getTechTradingCount() > 0);
}


void CvTeam::changeTechTradingCount(int iChange)
{
	m_iTechTradingCount = (m_iTechTradingCount + iChange);
	FAssert(getTechTradingCount() >= 0);
}


int CvTeam::getGoldTradingCount() const
{
	return m_iGoldTradingCount;
}


bool CvTeam::isGoldTrading() const
{
	return (getGoldTradingCount() > 0);
}


void CvTeam::changeGoldTradingCount(int iChange)
{
	m_iGoldTradingCount = (m_iGoldTradingCount + iChange);
	FAssert(getGoldTradingCount() >= 0);
}


int CvTeam::getOpenBordersTradingCount() const
{
	return m_iOpenBordersTradingCount;
}


bool CvTeam::isOpenBordersTrading() const
{
	return (getOpenBordersTradingCount() > 0);
}


void CvTeam::changeOpenBordersTradingCount(int iChange)
{
	m_iOpenBordersTradingCount = (m_iOpenBordersTradingCount + iChange);
	FAssert(getOpenBordersTradingCount() >= 0);
}

int CvTeam::getDefensivePactTradingCount() const
{
	return m_iDefensivePactTradingCount;
}


bool CvTeam::isDefensivePactTrading() const
{
	return (getDefensivePactTradingCount() > 0);
}


void CvTeam::changeDefensivePactTradingCount(int iChange)
{
	m_iDefensivePactTradingCount = (m_iDefensivePactTradingCount + iChange);
	FAssert(getDefensivePactTradingCount() >= 0);
}


int CvTeam::getPermanentAllianceTradingCount() const
{
	return m_iPermanentAllianceTradingCount;
}


bool CvTeam::isPermanentAllianceTrading() const
{
	if (!(GC.getGame().isOption(GAMEOPTION_PERMANENT_ALLIANCES)))
	{
		return false;
	}

	return (getPermanentAllianceTradingCount() > 0);
}


void CvTeam::changePermanentAllianceTradingCount(int iChange)
{
	m_iPermanentAllianceTradingCount = (m_iPermanentAllianceTradingCount + iChange);
	FAssert(getPermanentAllianceTradingCount() >= 0);
}


int CvTeam::getVassalTradingCount() const
{
	return m_iVassalTradingCount;
}


bool CvTeam::isVassalStateTrading() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_VASSAL_STATES))
	{
		return false;
	}

	return (getVassalTradingCount() > 0);
}


void CvTeam::changeVassalTradingCount(int iChange)
{
	m_iVassalTradingCount += iChange;
	FAssert(getVassalTradingCount() >= 0);
}


int CvTeam::getBridgeBuildingCount() const
{
	return m_iBridgeBuildingCount;
}


bool CvTeam::isBridgeBuilding()	const
{
	return (getBridgeBuildingCount() > 0);
}


void CvTeam::changeBridgeBuildingCount(int iChange)
{
	if (iChange != 0)
	{
		m_iBridgeBuildingCount = (m_iBridgeBuildingCount + iChange);
		FAssert(getBridgeBuildingCount() >= 0);

		if (GC.IsGraphicsInitialized())
		{
			gDLL->getEngineIFace()->MarkBridgesDirty();
		}
	}
}


int CvTeam::getIrrigationCount() const
{
	return m_iIrrigationCount;
}


bool CvTeam::isIrrigation() const
{
	return (getIrrigationCount() > 0);
}


void CvTeam::changeIrrigationCount(int iChange)
{
	if (iChange != 0)
	{
		m_iIrrigationCount = (m_iIrrigationCount + iChange);
		FAssert(getIrrigationCount() >= 0);

		GC.getMap().updateIrrigated();
	}
}


int CvTeam::getIgnoreIrrigationCount() const
{
	return m_iIgnoreIrrigationCount;
}


bool CvTeam::isIgnoreIrrigation() const
{
	return (getIgnoreIrrigationCount() > 0);
}


void CvTeam::changeIgnoreIrrigationCount(int iChange)
{
	m_iIgnoreIrrigationCount = (m_iIgnoreIrrigationCount + iChange);
	FAssert(getIgnoreIrrigationCount() >= 0);
}


int CvTeam::getWaterWorkCount() const
{
	return m_iWaterWorkCount;
}


bool CvTeam::isWaterWork() const
{
	return (getWaterWorkCount() > 0);
}


void CvTeam::changeWaterWorkCount(int iChange)
{
	if (iChange != 0)
	{
		m_iWaterWorkCount = (m_iWaterWorkCount + iChange);
		FAssert(getWaterWorkCount() >= 0);

		AI_makeAssignWorkDirty();
	}
}

int CvTeam::getVassalPower() const
{
	return m_iVassalPower;
}

void CvTeam::setVassalPower(int iPower)
{
	m_iVassalPower = iPower;
}

int CvTeam::getMasterPower() const
{
	return m_iMasterPower;
}

void CvTeam::setMasterPower(int iPower)
{
	m_iMasterPower = iPower;
}

int CvTeam::getEnemyWarWearinessModifier() const
{
	int iEnemyWarWearinessTotal = m_iEnemyWarWearinessModifier;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iEnemyWarWearinessTotal += GET_PLAYER((PlayerTypes)iI).getNationalEnemyWarWearinessModifier();
			}
		}
	}
	//return m_iEnemyWarWearinessModifier;
	return iEnemyWarWearinessTotal;
}

void CvTeam::changeEnemyWarWearinessModifier(int iChange)
{
	m_iEnemyWarWearinessModifier += iChange;
}

bool CvTeam::isMapCentering() const
{
	return m_bMapCentering;
}


void CvTeam::setMapCentering(bool bNewValue)
{
	if (isMapCentering() != bNewValue)
	{
		m_bMapCentering = bNewValue;

		if (getID() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
		}
	}
}


TeamTypes CvTeam::getID() const
{
	return m_eID;
}


int CvTeam::getStolenVisibilityTimer(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "iIndex is expected to be within maximum bounds (invalid Index)");

	return m_aiStolenVisibilityTimer[eIndex];
}


bool CvTeam::isStolenVisibility(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "iIndex is expected to be within maximum bounds (invalid Index)");

	return (getStolenVisibilityTimer(eIndex) > 0);
}


void CvTeam::setStolenVisibilityTimer(TeamTypes eIndex, int iNewValue)
{
	CvPlot* pLoopPlot;
	bool bOldStolenVisibility;
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (getStolenVisibilityTimer(eIndex) != iNewValue)
	{
		bOldStolenVisibility = isStolenVisibility(eIndex);

		m_aiStolenVisibilityTimer[eIndex] = iNewValue;
		FAssert(getStolenVisibilityTimer(eIndex) >= 0);

		if (bOldStolenVisibility != isStolenVisibility(eIndex))
		{
			for (iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				pLoopPlot = GC.getMap().plotByIndex(iI);

				if (pLoopPlot->isVisible(eIndex, false))
				{
					pLoopPlot->changeStolenVisibilityCount(getID(), ((isStolenVisibility(eIndex)) ? 1 : -1));
				}
			}
		}
	}
}


void CvTeam::changeStolenVisibilityTimer(TeamTypes eIndex, int iChange)
{
	setStolenVisibilityTimer(eIndex, (getStolenVisibilityTimer(eIndex) + iChange));
}


int CvTeam::getWarWeariness(TeamTypes eIndex) const
{
	return getWarWearinessTimes100(eIndex)/100;
}

int CvTeam::getWarWearinessTimes100(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiWarWearinessTimes100[eIndex];
}


void CvTeam::setWarWeariness(TeamTypes eIndex, int iNewValue)
{
	setWarWearinessTimes100(eIndex, 100*iNewValue);
}

void CvTeam::setWarWearinessTimes100(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiWarWearinessTimes100[eIndex] = std::max(0, iNewValue);
}


void CvTeam::changeWarWeariness(TeamTypes eIndex, int iChange)
{
	changeWarWearinessTimes100(eIndex, 100*iChange);
}

void CvTeam::changeWarWearinessTimes100(TeamTypes eIndex, int iChange)
{
	FAssert(eIndex >= 0 && eIndex < MAX_TEAMS);
	setWarWearinessTimes100(eIndex, getWarWearinessTimes100(eIndex) + iChange);
}

void CvTeam::changeWarWeariness(TeamTypes eOtherTeam, const CvPlot& kPlot, int iFactor)
{
	changeWarWearinessTimes100(eOtherTeam, kPlot, iFactor*100);
}

void CvTeam::changeWarWearinessTimes100(TeamTypes eOtherTeam, const CvPlot& kPlot, int iFactor)
{
	PROFILE_FUNC();

	int iOurCulture = kPlot.countFriendlyCulture(getID());
	int iTheirCulture = kPlot.countFriendlyCulture(eOtherTeam);

	int iRatio = 100;
	if (0 != iOurCulture + iTheirCulture)
	{
		iRatio = (100 * iTheirCulture) / (iOurCulture + iTheirCulture);
	}
	// Rebels don't feel as if they are fighting on foreign soil
	if (isRebelAgainst(eOtherTeam))
	{
		iRatio = std::min(40, iRatio);
	}
	else if (isRebel())
	{
		iRatio = std::min(60,iRatio);
	}
	changeWarWearinessTimes100(eOtherTeam, iRatio * iFactor);
}


int CvTeam::getTechShareCount(int iIndex) const
{
	FAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(iIndex < MAX_TEAMS, "iIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiTechShareCount[iIndex];
}


bool CvTeam::isTechShare(int iIndex) const
{
	return (getTechShareCount(iIndex) > 0);
}


void CvTeam::changeTechShareCount(int iIndex, int iChange)
{
	FAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(iIndex < MAX_TEAMS, "iIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiTechShareCount[iIndex] = (m_aiTechShareCount[iIndex] + iChange);
		FAssert(getTechShareCount(iIndex) >= 0);

		if (isTechShare(iIndex))
		{
			updateTechShare();
		}
	}
}


int CvTeam::getCommerceFlexibleCount(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCommerceFlexibleCount[eIndex];
}


bool CvTeam::isCommerceFlexible(CommerceTypes eIndex) const
{
	return (getCommerceFlexibleCount(eIndex) > 0);
}


void CvTeam::changeCommerceFlexibleCount(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCommerceFlexibleCount[eIndex] = (m_aiCommerceFlexibleCount[eIndex] + iChange);
		FAssert(getCommerceFlexibleCount(eIndex) >= 0);

		if (getID() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvTeam::getExtraMoves(DomainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiExtraMoves[eIndex];
}


void CvTeam::changeExtraMoves(DomainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiExtraMoves[eIndex] += iChange;
	FAssert(getExtraMoves(eIndex) >= 0);
}


bool CvTeam::isHasMet(TeamTypes eIndex)	const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	//FAssert((eIndex != getID()) || m_abHasMet[eIndex]);
	return m_abHasMet[eIndex];
}

// For barbarian and minor civs
void CvTeam::setHasMet( TeamTypes eIndex, bool bNewValue )
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	if (bNewValue != isHasMet(eIndex)) m_abHasMet[eIndex] = bNewValue;
	return;
}

void CvTeam::makeHasMet(TeamTypes eIndex, bool bNewDiplo)
{
	CvDiploParameters* pDiplo;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isHasMet(eIndex))
	{
		m_abHasMet[eIndex] = true;

		if (!isNPC() && !GET_TEAM(eIndex).isNPC())
		{
			updateTechShare();

			if (GET_TEAM(eIndex).isHuman())
			{
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive()
					&&	GET_PLAYER((PlayerTypes)iI).getTeam() == getID()
					&& !GET_PLAYER((PlayerTypes)iI).isHuman())
					{
						GET_PLAYER((PlayerTypes)iI).clearResearchQueue();
						GET_PLAYER((PlayerTypes)iI).AI_makeProductionDirty();
					}
				}
			}
		}

		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive()
			&& (GET_TEAM((TeamTypes)iI).isVassal(getID()) || isVassal((TeamTypes)iI)))
			{
				GET_TEAM((TeamTypes)iI).meet(eIndex, bNewDiplo);
			}
		}

		if (getID() == GC.getGame().getActiveTeam() || eIndex == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			// RevolutionDCM - redraw espionage percent buttons
			// gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true); // Toffer - don't think this is needed.
		}
		// Move reporting to Python before diplo popup to all war declarations on first contact
		// report event to Python, along with some other key state
		CvEventReporter::getInstance().firstContact(getID(), eIndex);

		if (bNewDiplo && GC.getGame().isFinalInitialized() && !gDLL->GetWorldBuilderMode()
		&& !isHuman() && !isAtWar(eIndex))
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if(GET_PLAYER((PlayerTypes)iI).isAlive()
				&& GET_PLAYER((PlayerTypes)iI).getTeam() == eIndex
				&& GET_PLAYER(getLeaderID()).canContact((PlayerTypes)iI)
				&& GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					pDiplo = new CvDiploParameters(getLeaderID());
					FAssertMsg(pDiplo != NULL, "pDiplo must be valid");
					pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_FIRST_CONTACT"));
					pDiplo->setAIContact(true);
					gDLL->beginDiplomacy(pDiplo, ((PlayerTypes)iI));
				}
			}
		}
	}
}


bool CvTeam::isAtWar(TeamTypes eIndex) const
{
	if ( eIndex == NO_TEAM )
	{
		return false;
	}
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abAtWar[eIndex];
}


void CvTeam::setAtWar(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if ( m_abAtWar[eIndex] != bNewValue )
	{
		//	Koshling - have seen game states with team flagged as at war with
		//	themslves.  No idea hiow this arises but we can at least prevent it
		//	here as a fallback
		if ( !bNewValue || eIndex != getID() )
		{
			m_abAtWar[eIndex] = bNewValue;

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() &&
					GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					GET_PLAYER((PlayerTypes)iI).AI_noteWarStatusChange(eIndex, bNewValue);
				}
			}
		}
		else
		{
			FAssertMsg(false, "Team trying to mark itself as at war with itself");
		}
	}
}


bool CvTeam::isPermanentWarPeace(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abPermanentWarPeace[eIndex];
}


void CvTeam::setPermanentWarPeace(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abPermanentWarPeace[eIndex] = bNewValue;
}

bool CvTeam::isFreeTrade(TeamTypes eIndex) const
{
	if (isAtWar(eIndex) || !isHasMet(eIndex))
	{
		return false;
	}

	if (isOpenBorders(eIndex))
	{
		return true;
	}
	//If the secretary general voted for open markets
	if (GC.getGame().isFreeTrade())
	{
		return true;
	}

	if (isLimitedBorders(eIndex))
	{
		return true;
	}

	if (isFreeTradeAgreement(eIndex))
	{
		return true;
	}

	return false;
}


bool CvTeam::isOpenBorders(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abOpenBorders[eIndex];
}


void CvTeam::setOpenBorders(TeamTypes eIndex, bool bNewValue)
{
	bool bOldFreeTrade;
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isOpenBorders(eIndex) != bNewValue)
	{
		bOldFreeTrade = isFreeTrade(eIndex);

		m_abOpenBorders[eIndex] = bNewValue;

		AI_setOpenBordersCounter(eIndex, 0);

		GC.getMap().verifyUnitValidPlot();

		if ((getID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (bOldFreeTrade != isFreeTrade(eIndex))
		{
			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
					{
						GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
					}
				}
			}
		}
	}
}

bool CvTeam::isDefensivePact(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abDefensivePact[eIndex];
}


void CvTeam::setDefensivePact(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isDefensivePact(eIndex) != bNewValue)
	{
		m_abDefensivePact[eIndex] = bNewValue;

		if (getID() == GC.getGame().getActiveTeam() || eIndex == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (bNewValue && !GET_TEAM(eIndex).isDefensivePact(getID()))
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYERS_SIGN_DEFENSIVE_PACT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());

			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));


			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
				if (kPlayer.isAlive() && isHasMet(kPlayer.getTeam()) && GET_TEAM(eIndex).isHasMet(kPlayer.getTeam()))
				{
					MEMORY_TRACK_EXEMPT();
					AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WELOVEKING", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
				}
			}
		}

		// Attitude cache
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).getTeam() != GET_PLAYER((PlayerTypes)iI).getTeam()
					&& (GET_PLAYER((PlayerTypes)iJ).getTeam() == getID() || GET_PLAYER((PlayerTypes)iJ).getTeam() == eIndex))
					{
						GET_PLAYER((PlayerTypes)iJ).AI_invalidateAttitudeCache((PlayerTypes)iI);
						GET_PLAYER((PlayerTypes)iI).AI_invalidateAttitudeCache((PlayerTypes)iJ);
					}
				}
			}
		}
	}
}


bool CvTeam::isForcePeace(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abForcePeace[eIndex];
}


void CvTeam::setForcePeace(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abForcePeace[eIndex] = bNewValue;

	if (isForcePeace(eIndex))
	{
		if (AI_isSneakAttackPreparing(eIndex))
		{
			AI_setWarPlan(eIndex, NO_WARPLAN);
		}

		for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
		{
			if (GET_TEAM((TeamTypes)iTeam).isVassal(eIndex))
			{
				if (AI_isSneakAttackPreparing((TeamTypes)iTeam))
				{
					AI_setWarPlan((TeamTypes)iTeam, NO_WARPLAN);
				}
			}
		}
	}
}

bool CvTeam::isVassal(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abVassal[eIndex];
}

void CvTeam::setVassal(TeamTypes eIndex, bool bNewValue, bool bCapitulated)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(!bNewValue || !GET_TEAM(eIndex).isAVassal(), "can't become a vassal of a vassal")

	if (isVassal(eIndex) != bNewValue)
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (GET_PLAYER((PlayerTypes)i).getTeam() == getID() && GET_PLAYER((PlayerTypes)i).isAlive())
			{
				GET_PLAYER((PlayerTypes)i).updateCitySight(false, false);
			}
		}

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == eIndex)
			{
				foreach_(const CvUnit* pLoopUnit, kLoopPlayer.units())
				{
					const CvPlot* pPlot = pLoopUnit->plot();
					if (pLoopUnit->getTeam() != pPlot->getTeam() && (pPlot->getTeam() == NO_TEAM || !GET_TEAM(pPlot->getTeam()).isVassal(pLoopUnit->getTeam())))
					{
						kLoopPlayer.changeNumOutsideUnits(-1);
					}
				}
			}
		}

		m_abVassal[eIndex] = bNewValue;

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == eIndex)
			{
				foreach_(const CvUnit* pLoopUnit, kLoopPlayer.units())
				{
					const CvPlot* pPlot = pLoopUnit->plot();
					if (pLoopUnit->getTeam() != pPlot->getTeam() && (pPlot->getTeam() == NO_TEAM || !GET_TEAM(pPlot->getTeam()).isVassal(pLoopUnit->getTeam())))
					{
						kLoopPlayer.changeNumOutsideUnits(1);
					}
				}
			}
		}

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (GET_PLAYER((PlayerTypes)i).getTeam() == getID() && GET_PLAYER((PlayerTypes)i).isAlive())
			{
				GET_PLAYER((PlayerTypes)i).updateCitySight(true, false);
			}
		}

		for (int i = 0; i < GC.getMap().numPlots(); ++i)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(i);
			if (pLoopPlot && (pLoopPlot->getTeam() == getID() || pLoopPlot->getTeam() == eIndex))
			{
				pLoopPlot->updateCulture(true, false);
			}
		}

		GC.getGame().updatePlotGroups();

		if (isVassal(eIndex))
		{
			m_bCapitulated = bCapitulated;

			CvDeal* pLoopDeal;
			int iLoop;
			CLLNode<TradeData>* pNode;
			bool bValid;

			for (pLoopDeal = GC.getGame().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGame().nextDeal(&iLoop))
			{
				if ((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == getID()) || (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == getID()))
				{
					bValid = true;

					if (pLoopDeal->getFirstTrades() != NULL)
					{
						for (pNode = pLoopDeal->getFirstTrades()->head(); pNode; pNode = pLoopDeal->getFirstTrades()->next(pNode))
						{
							if ((pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT) ||
								(pNode->m_data.m_eItemType == TRADE_PEACE_TREATY))
							{
								bValid = false;
								break;
							}
						}
					}

					if (bValid && pLoopDeal->getSecondTrades() != NULL)
					{
						for (pNode = pLoopDeal->getSecondTrades()->head(); pNode; pNode = pLoopDeal->getSecondTrades()->next(pNode))
						{
							if ((pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT) ||
								(pNode->m_data.m_eItemType == TRADE_PEACE_TREATY))
							{
								bValid = false;
								break;
							}
						}
					}

					if (!bValid)
					{
						pLoopDeal->kill();
					}
				}
			}

			setForcePeace(eIndex, false);
			GET_TEAM(eIndex).setForcePeace(getID(), false);

			// Declare war on teams you should be at war with
			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				if ((iI != getID()) && (iI != eIndex))
				{
					if (GET_TEAM((TeamTypes)iI).isAlive())
					{
						if (GET_TEAM(eIndex).isHasMet((TeamTypes)iI))
						{
							meet((TeamTypes)iI, true);
						}

						if (isHasMet((TeamTypes)iI))
						{
							GET_TEAM(eIndex).meet((TeamTypes)iI, true);
						}

						if (GET_TEAM(eIndex).isAtWar((TeamTypes)iI))
						{
							declareWar(((TeamTypes)iI), false, WARPLAN_DOGPILE);
						}
						else if (isAtWar((TeamTypes)iI))
						{
							if (bCapitulated)
							{
								makePeace((TeamTypes)iI);
							}
							else
							{
								GET_TEAM(eIndex).declareWar((TeamTypes)iI, false, WARPLAN_DOGPILE);
							}
						}
					}
				}
			}

			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
				if (kLoopTeam.isAlive())
				{
					if (!kLoopTeam.isAtWar(getID()))
					{
						kLoopTeam.AI_setWarPlan(getID(), NO_WARPLAN);
						AI_setWarPlan((TeamTypes)iI, NO_WARPLAN);
					}

					if (!kLoopTeam.isAtWar(eIndex))
					{
						kLoopTeam.AI_setWarPlan(eIndex, NO_WARPLAN);
					}

				}
			}

			// All our vassals now become free
			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				if ((iI != getID()) && (iI != eIndex))
				{
					CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
					if (kLoopTeam.isAlive() && kLoopTeam.isVassal(getID()))
					{
						freeVassal((TeamTypes)iI);
					}
				}
			}

			setMasterPower(GET_TEAM(eIndex).getTotalLand());
			setVassalPower(getTotalLand(false));

			if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
			{
				CvWString szReplayMessage;

				if (bCapitulated)
				{
					szReplayMessage = gDLL->getText("TXT_KEY_MISC_CAPITULATE_AGREEMENT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());
				}
				else
				{
					szReplayMessage = gDLL->getText("TXT_KEY_MISC_VASSAL_AGREEMENT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());
				}
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szReplayMessage, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
					if (kPlayer.isAlive())
					{
						if (isHasMet(kPlayer.getTeam()) && GET_TEAM(eIndex).isHasMet(kPlayer.getTeam()))
						{
							MEMORY_TRACK_EXEMPT();

							AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szReplayMessage, "AS2D_WELOVEKING", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
						}
					}
				}
			}
		}
		else
		{
			setMasterPower(0);
			setVassalPower(0);

			if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()) && isAlive() && GET_TEAM(eIndex).isAlive())
			{
				CvWString szReplayMessage;

				if (m_bCapitulated)
				{
					szReplayMessage = gDLL->getText("TXT_KEY_MISC_SURRENDER_REVOLT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());
				}
				else
				{
					szReplayMessage = gDLL->getText("TXT_KEY_MISC_VASSAL_REVOLT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());
				}

				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szReplayMessage, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
					if (kPlayer.isAlive())
					{
						CvWString szBuffer;
						if (getID() == kPlayer.getTeam() || eIndex == kPlayer.getTeam() || isHasMet(kPlayer.getTeam()) && GET_TEAM(eIndex).isHasMet(kPlayer.getTeam()))
						{
							szBuffer = szReplayMessage;
						}

						if (!szBuffer.empty())
						{
							MEMORY_TRACK_EXEMPT();

							AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_REVOLTSTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
						}
					}
				}
			}

			m_bCapitulated = false;
		}

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (kLoopPlayer.getTeam() == eIndex && kLoopPlayer.isAlive())
			{
				kLoopPlayer.setMaintenanceDirty(true);
			}
		}

		if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
		{
			CvEventReporter::getInstance().vassalState(eIndex, getID(), bNewValue);
		}

		// Attitude cache
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive()
			&& (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() || GET_PLAYER((PlayerTypes)iI).getTeam() == eIndex))
			{
				for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).getTeam() != GET_PLAYER((PlayerTypes)iI).getTeam()
					&& (GET_PLAYER((PlayerTypes)iJ).getTeam() == getID() || GET_PLAYER((PlayerTypes)iJ).getTeam() == eIndex))
					{
						GET_PLAYER((PlayerTypes)iJ).AI_invalidateAttitudeCache((PlayerTypes)iI);
						GET_PLAYER((PlayerTypes)iI).AI_invalidateAttitudeCache((PlayerTypes)iJ);
					}
				}
			}
		}
	}
}

void CvTeam::assignVassal(TeamTypes eVassal, bool bSurrender) const
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	GET_TEAM(eVassal).setVassal(getID(), true, bSurrender);

	setTradeItem(&item, bSurrender ? TRADE_SURRENDER : TRADE_VASSAL);
	item.m_iData = 1;

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.getTeam() == eVassal && kLoopPlayer.isAlive())
		{
			ourList.clear();
			theirList.clear();

			theirList.insertAtEnd(item);

			for (int jPlayer = 0; jPlayer < MAX_PLAYERS; jPlayer++)
			{
				if (GET_PLAYER((PlayerTypes)jPlayer).getTeam() == getID() && GET_PLAYER((PlayerTypes)jPlayer).isAlive())
				{
					GC.getGame().implementDeal(((PlayerTypes)jPlayer), ((PlayerTypes)iPlayer), &ourList, &theirList, true);
				}
			}
		}
	}
}


void CvTeam::freeVassal(TeamTypes eVassal) const
{
	CvDeal* pLoopDeal;
	int iLoop;
	CLLNode<TradeData>* pNode;
	bool bValid;

	for (pLoopDeal = GC.getGame().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGame().nextDeal(&iLoop))
	{
		bValid = true;

		if ((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == eVassal) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == getID()))
		{

			if (pLoopDeal->getFirstTrades() != NULL)
			{
				for (pNode = pLoopDeal->getFirstTrades()->head(); pNode; pNode = pLoopDeal->getFirstTrades()->next(pNode))
				{
					if ((pNode->m_data.m_eItemType == TRADE_VASSAL) ||
						(pNode->m_data.m_eItemType == TRADE_SURRENDER))
					{
						bValid = false;
					}
				}
			}
		}

		if (bValid)
		{
			if ((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == getID()) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == eVassal))
			{
				if (pLoopDeal->getSecondTrades() != NULL)
				{
					for (pNode = pLoopDeal->getSecondTrades()->head(); pNode; pNode = pLoopDeal->getSecondTrades()->next(pNode))
					{
						if ((pNode->m_data.m_eItemType == TRADE_VASSAL) ||
							(pNode->m_data.m_eItemType == TRADE_SURRENDER))
						{
							bValid = false;
						}
					}
				}
			}
		}

		if (!bValid)
		{
			pLoopDeal->kill();
		}
	}
}

bool CvTeam::isCapitulated() const
{
	FAssert(isAVassal());

	return m_bCapitulated;
}


int CvTeam::getRouteChange(RouteTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumRouteInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiRouteChange[eIndex];
}


void CvTeam::changeRouteChange(RouteTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumRouteInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiRouteChange[eIndex] = (m_paiRouteChange[eIndex] + iChange);
}


int CvTeam::getProjectCount(ProjectTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectCount[eIndex];
}

int CvTeam::getProjectDefaultArtType(ProjectTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectDefaultArtTypes[eIndex];
}

void CvTeam::setProjectDefaultArtType(ProjectTypes eIndex, int value)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiProjectDefaultArtTypes[eIndex] = value;
}

int CvTeam::getProjectArtType(ProjectTypes eIndex, int number) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(number >= 0, "number is expected to be non-negative (invalid Index)");
	FAssertMsg(number < getProjectCount(eIndex), "number is expected to be within maximum bounds (invalid Index)");
	return m_pavProjectArtTypes[eIndex][number];
}

void CvTeam::setProjectArtType(ProjectTypes eIndex, int number, int value)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(number >= 0, "number is expected to be non-negative (invalid Index)");
	FAssertMsg(number < getProjectCount(eIndex), "number is expected to be within maximum bounds (invalid Index)");
	m_pavProjectArtTypes[eIndex][number] = value;
}

bool CvTeam::isProjectMaxedOut(ProjectTypes eIndex, int iExtra) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isTeamProject(eIndex))
	{
		return false;
	}

	FAssertMsg(getProjectCount(eIndex) <= GC.getProjectInfo(eIndex).getMaxTeamInstances(), "Current Project count is expected to not exceed the maximum number of instances for this project");

	return ((getProjectCount(eIndex) + iExtra) >= GC.getProjectInfo(eIndex).getMaxTeamInstances());
}

bool CvTeam::isProjectAndArtMaxedOut(ProjectTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(getProjectCount(eIndex) >= GC.getProjectInfo(eIndex).getMaxTeamInstances())
	{
		int count = getProjectCount(eIndex);
		for(int i=0;i<count;i++)
		{
			if(getProjectArtType(eIndex, i) == -1) //undefined
				return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

void CvTeam::finalizeProjectArtTypes()
{
	//loop through each project and fill in default art values
	for(int i=0;i<GC.getNumProjectInfos();i++)
	{
		ProjectTypes eProject = (ProjectTypes) i;
		int projectCount = getProjectCount(eProject);
		for(int j=0;j<projectCount;j++)
		{
			int projectArtType = getProjectArtType(eProject, j);
			if(projectArtType == -1) //undefined
			{
				int defaultArtType = getProjectDefaultArtType(eProject);
				setProjectArtType(eProject, j, defaultArtType);
			}
		}
	}
}


void CvTeam::changeProjectCount(ProjectTypes eIndex, int iChange)
{
	CvWString szBuffer;
	int iOldProjectCount;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		GC.getGame().incrementProjectCreatedCount(eIndex, iChange);

		iOldProjectCount = getProjectCount(eIndex);
		m_paiProjectCount[eIndex] = (m_paiProjectCount[eIndex] + iChange);
		FAssert(getProjectCount(eIndex) >= 0);

		//adjust default art types
		if(iChange >= 0)
		{
			int defaultType = -1;
			for(int i=0;i<iChange;i++)
				m_pavProjectArtTypes[eIndex].push_back(defaultType);
		}
		else
		{
			for(int i=0;i<-iChange;i++)
				m_pavProjectArtTypes[eIndex].pop_back();
		}
		FAssertMsg(getProjectCount(eIndex) == (int)m_pavProjectArtTypes[eIndex].size(), "[Jason] Unbalanced project art types.");

		processProjectChange(eIndex, iChange, iOldProjectCount);

		if ( iChange > 0 )
		{
			if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
			{
				const CvProjectInfo& kProject = GC.getProjectInfo(eIndex);
				szBuffer = gDLL->getText("TXT_KEY_MISC_COMPLETES_PROJECT", getName().GetCString(), kProject.getTextKeyWide());
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						MEMORY_TRACK_EXEMPT();

						szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_HAS_COMPLETED", getName().GetCString(), kProject.getTextKeyWide());
						AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PROJECT_COMPLETED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
					}
				}
			}
		}
	}
}

void CvTeam::processProjectChange(ProjectTypes eIndex, int iChange, int iOldProjectCount)
{
	const CvProjectInfo& kProject = GC.getProjectInfo(eIndex);

	changeNukeInterception(kProject.getNukeInterception() * iChange);

	if (kProject.getTechShare() > 0 && kProject.getTechShare() <= MAX_TEAMS)
	{
		changeTechShareCount((kProject.getTechShare() - 1), iChange);
	}

	for (int iI = 0; iI < GC.getNumVictoryInfos(); ++iI)
	{
		if (kProject.getVictoryThreshold(iI) > 0)
		{
			m_abCanLaunch[iI] = GC.getGame().testVictory((VictoryTypes)iI, getID());
		}
	}

	if (iChange > 0)
	{
		if (kProject.getEveryoneSpecialUnit() != NO_SPECIALUNIT)
		{
			GC.getGame().makeSpecialUnitValid((SpecialUnitTypes)(kProject.getEveryoneSpecialUnit()));
		}

		if (kProject.getEveryoneSpecialBuilding() != NO_SPECIALBUILDING)
		{
			GC.getGame().makeSpecialBuildingValid((SpecialBuildingTypes)(kProject.getEveryoneSpecialBuilding()));
		}
/* TB Nukefix
		if (kProject.isAllowsNukes())
		{
			GC.getGame().makeNukesValid(true);
		}
*/
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					if (!(GET_PLAYER((PlayerTypes)iI).isHuman()))
					{
						for (int iJ = 0; iJ < GC.getNumProjectInfos(); iJ++)
						{
							if (getProjectCount(eIndex) >= GC.getProjectInfo((ProjectTypes)iJ).getProjectsNeeded(eIndex)
							&& iOldProjectCount < GC.getProjectInfo((ProjectTypes)iJ).getProjectsNeeded(eIndex))
							{
								GET_PLAYER((PlayerTypes)iI).AI_makeProductionDirty();
								break;
							}
						}
					}
					GET_PLAYER((PlayerTypes)iI).changeProjectHappiness(kProject.getGlobalHappiness());
					GET_PLAYER((PlayerTypes)iI).changeProjectHealth(kProject.getGlobalHealth());
					GET_PLAYER((PlayerTypes)iI).changeProjectInflation(kProject.getInflationModifier());
					GET_PLAYER((PlayerTypes)iI).changeMaintenanceModifier(GC.getProjectInfo(eIndex).getGlobalMaintenanceModifier());
					GET_PLAYER((PlayerTypes)iI).changeDistanceMaintenanceModifier(GC.getProjectInfo(eIndex).getDistanceMaintenanceModifier());
					GET_PLAYER((PlayerTypes)iI).changeNumCitiesMaintenanceModifier(GC.getProjectInfo(eIndex).getNumCitiesMaintenanceModifier());
					GET_PLAYER((PlayerTypes)iI).changeConnectedCityMaintenanceModifier(GC.getProjectInfo(eIndex).getConnectedCityMaintenanceModifier());

					for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
					{
						GET_PLAYER((PlayerTypes)iI).changeCommerceRateModifierfromBuildings(((CommerceTypes)iJ), kProject.getCommerceModifier(iJ));
					}
				}
				GET_PLAYER((PlayerTypes)iI).changeWorldHappiness(kProject.getWorldHappiness());
				GET_PLAYER((PlayerTypes)iI).changeWorldHealth(kProject.getWorldHealth());
				GET_PLAYER((PlayerTypes)iI).changeWorldTradeRoutes(kProject.getWorldTradeRoutes());
			}
		}
	}
}

int CvTeam::getProjectMaking(ProjectTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectMaking[eIndex];
}


void CvTeam::changeProjectMaking(ProjectTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiProjectMaking[eIndex] = (m_paiProjectMaking[eIndex] + iChange);
	FAssert(getProjectMaking(eIndex) >= 0);
}


int CvTeam::getUnitCount(UnitTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitCount[eIndex];
}


bool CvTeam::isUnitMaxedOut(UnitTypes eIndex, int iExtra) const
{
	return false;

/* Toffer:
iMaxTeamInstances was unused in CvUnit(Class)Info and then removed as part of us shedding the unit-class object, maybe we want to add it back in for CvUnitInfo?

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isTeamUnit(eIndex))
	{
		return false;
	}

	if (GC.getGame().isOption(GAMEOPTION_UNLIMITED_NATIONAL_UNITS) && !GC.getUnitInfo(eIndex).isUnlimitedException())
	{
		return false;
	}
	return ((getUnitCount(eIndex) + iExtra) >= GC.getUnitInfo(eIndex).getMaxTeamInstances());
*/
}


void CvTeam::changeUnitCount(UnitTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitCount[eIndex] = (m_paiUnitCount[eIndex] + iChange);
	FAssert(getUnitCount(eIndex) >= 0);
}


int CvTeam::getBuildingCount(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingCount[eIndex];
}


bool CvTeam::isBuildingMaxedOut(BuildingTypes eIndex, int iExtra) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isTeamWonder(eIndex))
	{
		return false;
	}

	FAssertMsg(getBuildingCount(eIndex) <= GC.getBuildingInfo(eIndex).getMaxTeamInstances(), "The current building count is expected not to exceed the maximum number of instances allowed for this team");

	return ((getBuildingCount(eIndex) + iExtra) >= GC.getBuildingInfo(eIndex).getMaxTeamInstances());
}


void CvTeam::changeBuildingCount(BuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	int iValue = m_paiBuildingCount[eIndex] + iChange;
	if (iValue < 0)
	{
		iValue = 0;
	}
	m_paiBuildingCount[eIndex] = iValue;
	FAssert(getBuildingCount(eIndex) >= 0);
}


int CvTeam::getObsoleteBuildingCount(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiObsoleteBuildingCount[eIndex];
}


bool CvTeam::isObsoleteBuilding(BuildingTypes eIndex) const
{
	return (getObsoleteBuildingCount(eIndex) > 0);
}


void CvTeam::changeObsoleteBuildingCount(BuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiObsoleteBuildingCount[eIndex] += iChange;
		FAssert(getObsoleteBuildingCount(eIndex) >= 0);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if (pLoopCity->getNumBuilding(eIndex) > 0)
					{
						pLoopCity->processBuilding(eIndex, (isObsoleteBuilding(eIndex) ? -1 : 1), isObsoleteBuilding(eIndex));
					}
				}
			}
		}
	}
}


int CvTeam::getResearchProgress(TechTypes eIndex) const
{
	if (eIndex != NO_TECH)
	{
		return m_paiResearchProgress[eIndex];
	}
	return 0;
}


void CvTeam::setResearchProgress(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if (getResearchProgress(eIndex) != iNewValue)
	{
		m_paiResearchProgress[eIndex] = iNewValue;
		FAssert(getResearchProgress(eIndex) >= 0);

		if (getID() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (getResearchProgress(eIndex) >= getResearchCost(eIndex))
		{
			int iOverflow = (100 * (getResearchProgress(eIndex) - getResearchCost(eIndex))) / std::max(1, GET_PLAYER(ePlayer).calculateResearchModifier(eIndex));

			// Multiple Research
			setHasTech(eIndex, true, ePlayer, true, true);
			iOverflow = GET_PLAYER(ePlayer).doMultipleResearch(iOverflow);
			GET_PLAYER(ePlayer).changeOverflowResearch(iOverflow);
		}
	}
}


void CvTeam::changeResearchProgress(TechTypes eIndex, int iChange, PlayerTypes ePlayer)
{
	int iNewResearch = std::max(0, getResearchProgress(eIndex) + iChange);

	setResearchProgress(eIndex, iNewResearch, ePlayer);
}

int CvTeam::changeResearchProgressPercent(TechTypes eIndex, int iPercent, PlayerTypes ePlayer)
{
	int iBeakers = 0;

	if (0 != iPercent && !isHasTech(eIndex))
	{
		if (iPercent > 0)
		{
			iBeakers = std::min(getResearchLeft(eIndex), (getResearchCost(eIndex) * iPercent) / 100);
		}
		else
		{
			iBeakers = std::max(getResearchLeft(eIndex) - getResearchCost(eIndex), (getResearchCost(eIndex) * iPercent) / 100);
		}
		changeResearchProgress(eIndex, iBeakers, ePlayer);
	}
	return iBeakers;
}


int CvTeam::getTechCount(TechTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiTechCount[eIndex];
}


int CvTeam::getBestKnownTechScorePercent() const
{
	int iOurTechScore = 0;
	int iBestKnownTechScore = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if( GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iOurTechScore = std::max( iOurTechScore, GET_PLAYER((PlayerTypes)iI).getTechScore() );
			}
			else if (isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				iBestKnownTechScore = std::max( iBestKnownTechScore, GET_PLAYER((PlayerTypes)iI).getTechScore() );
			}
		}
	}

	iBestKnownTechScore = std::max( iBestKnownTechScore, iOurTechScore );

	return ((100*iOurTechScore)/std::max(iBestKnownTechScore, 1));
}


int CvTeam::getTerrainTradeCount(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiTerrainTradeCount[eIndex];
}


bool CvTeam::isTerrainTrade(TerrainTypes eIndex) const
{
	if (isNPC())
	{
		return false;
	}
	return (getTerrainTradeCount(eIndex) > 0);
}


void CvTeam::changeTerrainTradeCount(TerrainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiTerrainTradeCount[eIndex] = (m_paiTerrainTradeCount[eIndex] + iChange);
		FAssert(getTerrainTradeCount(eIndex) >= 0);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).updatePlotGroups();
			}
		}
	}
}


int CvTeam::getRiverTradeCount() const
{
	return m_iRiverTradeCount;
}


bool CvTeam::isRiverTrade() const
{
	return (getRiverTradeCount() > 0);
}


void CvTeam::changeRiverTradeCount(int iChange)
{
	if (iChange != 0)
	{
		m_iRiverTradeCount += iChange;
		FAssert(getRiverTradeCount() >= 0);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).updatePlotGroups();
			}
		}
	}
}


int CvTeam::getVictoryCountdown(VictoryTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiVictoryCountdown[eIndex];
}

void CvTeam::setVictoryCountdown(VictoryTypes eIndex, int iTurnsLeft)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiVictoryCountdown[eIndex] = iTurnsLeft;
}


void CvTeam::changeVictoryCountdown(VictoryTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiVictoryCountdown[eIndex] += iChange;
		FAssert(m_aiVictoryCountdown[eIndex] >= 0);
	}
}

int CvTeam::getVictoryDelay(VictoryTypes eVictory) const
{
	int iExtraDelayPercent = 0;
	for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
	{
		const CvProjectInfo& kProject = GC.getProjectInfo((ProjectTypes)iProject);
		const int iCount = getProjectCount((ProjectTypes)iProject);

		if (iCount < kProject.getVictoryMinThreshold(eVictory))
		{
			FAssert(false);
			return -1;
		}

		const int victoryThreshold = kProject.getVictoryThreshold(eVictory);
		if (iCount < victoryThreshold && victoryThreshold > 0)
		{
			iExtraDelayPercent += ((victoryThreshold - iCount) * kProject.getVictoryDelayPercent()) / victoryThreshold;
		}
	}
	return (GC.getGame().victoryDelay(eVictory)  * (100 + iExtraDelayPercent)) / 100;
}

void CvTeam::setCanLaunch(VictoryTypes eVictory, bool bCan)
{
	m_abCanLaunch[eVictory] = bCan;
}

bool CvTeam::canLaunch(VictoryTypes eVictory) const
{
	return m_abCanLaunch[eVictory];
}

int CvTeam::getLaunchSuccessRate(VictoryTypes eVictory) const
{
	int iSuccessRate = 100;
	for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
	{
		const CvProjectInfo& kProject = GC.getProjectInfo((ProjectTypes)iProject);
		const int iCount = getProjectCount((ProjectTypes)iProject);

		if (iCount < kProject.getVictoryMinThreshold(eVictory))
		{
			return 0;
		}
		if (iCount < kProject.getVictoryThreshold(eVictory) && kProject.getSuccessRate() > 0)
		{
			iSuccessRate -= (kProject.getSuccessRate() * (kProject.getVictoryThreshold(eVictory) - iCount));
		}
	}
	return iSuccessRate;
}

void CvTeam::resetVictoryProgress()
{
	for (int iI = 0; iI < GC.getNumVictoryInfos(); ++iI)
	{
		if (getVictoryCountdown((VictoryTypes)iI) >= 0 && GC.getGame().getGameState() == GAMESTATE_ON)
		{
			setVictoryCountdown((VictoryTypes)iI, -1);

			for (int iK = 0; iK < GC.getNumProjectInfos(); iK++)
			{
				if (GC.getProjectInfo((ProjectTypes)iK).getVictoryMinThreshold((VictoryTypes)iI) > 0)
				{
					changeProjectCount((ProjectTypes)iK, -getProjectCount((ProjectTypes)iK));
				}
			}

			bool bValid = false;
			for (int iJ = 0; iJ < GC.getNumVictoryInfos(); ++iJ)
			{
				if (GC.getGame().isVictoryValid((VictoryTypes)iJ))
				{
					if (GC.getVictoryInfo((VictoryTypes)iJ).isTotalVictory())
					{
						bValid = true;
						break;
					}
				}
			}
			if (!bValid)
			{
				CvWString szBuffer = gDLL->getText("TXT_KEY_VICTORY_RESET", getName().GetCString(), GC.getVictoryInfo((VictoryTypes)iI).getTextKeyWide());

				for (int iJ = 0; iJ < MAX_PLAYERS; ++iJ)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iJ);
					if (kPlayer.isAlive())
					{
						MEMORY_TRACK_EXEMPT();

						AddDLLMessage(((PlayerTypes)iJ), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_MELTDOWN", MESSAGE_TYPE_MAJOR_EVENT);

						if (kPlayer.getTeam() == getID())
						{
							CvPopupInfo* pInfo = new CvPopupInfo();
							pInfo->setText(szBuffer);
							gDLL->getInterfaceIFace()->addPopup(pInfo, (PlayerTypes) iJ);
						}
					}
				}
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
			}
		}
	}
}

bool CvTeam::isParent(TeamTypes eTeam) const
{
	FAssert(eTeam != NO_TEAM);

	if (GET_TEAM(eTeam).isVassal(getID()))
	{
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
			if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == eTeam && kLoopPlayer.getParent() != NO_PLAYER && GET_PLAYER(kLoopPlayer.getParent()).getTeam() == getID())
			{
				return true;
			}
		}
	}
	return false;
}

bool CvTeam::isHasTech(TechTypes eIndex) const
{
	if (eIndex <= NO_TECH)
	{
		return true;
	}
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_pabHasTech != NULL, "m_pabHasTech is not expected to be equal with NULL");
	return m_pabHasTech[eIndex];
}

void CvTeam::announceTechToPlayers(TechTypes eIndex, bool bPartial)
{
	bool bSound = ((GC.getGame().isNetworkMultiPlayer() || gDLL->getInterfaceIFace()->noTechSplash()) && !bPartial);

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			MEMORY_TRACK_EXEMPT();

			CvWString szBuffer = gDLL->getText((bPartial ? "TXT_KEY_MISC_PROGRESS_TOWARDS_TECH" : "TXT_KEY_MISC_YOU_DISCOVERED_TECH"), GC.getTechInfo(eIndex).getTextKeyWide());

			AddDLLMessage((PlayerTypes)iI, false, (bSound ? GC.getEVENT_MESSAGE_TIME() : -1), szBuffer, (bSound ? GC.getTechInfo(eIndex).getSoundMP() : NULL), MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_TECH_TEXT"));
		}
	}
}

void CvTeam::setHasTech(TechTypes eIndex, bool bNewValue, PlayerTypes ePlayer, bool bFirst, bool bAnnounce)
{
	PROFILE_FUNC();

	if (eIndex == NO_TECH)
	{
		return;
	}

	if (ePlayer == NO_PLAYER)
	{
		ePlayer = getLeaderID();
	}

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if (isHasTech(eIndex) != bNewValue || GC.getTechInfo(eIndex).isRepeat() && m_paiTechCount[eIndex] > 0)
	{
		if (GC.getTechInfo(eIndex).isRepeat())
		{
			if (bNewValue)
			{
				m_paiTechCount[eIndex]++;

				setResearchProgress(eIndex, 0, ePlayer);

				// report event to Python
				CvEventReporter::getInstance().techAcquired(eIndex, getID(), ePlayer, bAnnounce && 1 == m_paiTechCount[eIndex]);

				if (1 == m_paiTechCount[eIndex] && bAnnounce && GC.getGame().isFinalInitialized())
				{
					announceTechToPlayers(eIndex);
				}
			}
			else
			{
				m_paiTechCount[eIndex]--;
				setResearchProgress(eIndex, 0, ePlayer);
			}
		}
		else
		{
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

				if (pLoopPlot->getBonusType() != NO_BONUS)
				{
					if (pLoopPlot->getTeam() == getID())
					{
						if ((GC.getBonusInfo(pLoopPlot->getBonusType()).getTechReveal() == eIndex) ||
							  (GC.getBonusInfo(pLoopPlot->getBonusType()).getTechCityTrade() == eIndex) ||
								(GC.getBonusInfo(pLoopPlot->getBonusType()).getTechObsolete() == eIndex))
						{
							pLoopPlot->updatePlotGroupBonus(false);
						}
					}
				}
			}

			m_pabHasTech[eIndex] = bNewValue;

			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

				if (pLoopPlot->getBonusType() != NO_BONUS)
				{
					if (pLoopPlot->getTeam() == getID())
					{
						if ((GC.getBonusInfo(pLoopPlot->getBonusType()).getTechReveal() == eIndex) ||
							  (GC.getBonusInfo(pLoopPlot->getBonusType()).getTechCityTrade() == eIndex) ||
							  (GC.getBonusInfo(pLoopPlot->getBonusType()).getTechObsolete() == eIndex))
						{
							pLoopPlot->updatePlotGroupBonus(true);
						}
					}
				}
			}
		}

		processTech(eIndex, ((bNewValue) ? 1 : -1), bAnnounce);

		if (isHasTech(eIndex))
		{
			if (gTeamLogLevel >= 2)
			{
				logBBAI("    Team %d (%S) acquires tech %S", getID(), GET_PLAYER(ePlayer).getCivilizationDescription(0), GC.getTechInfo(eIndex).getDescription() );
			}

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					if (GET_PLAYER((PlayerTypes)iI).getCurrentEra() < GC.getTechInfo(eIndex).getEra())
					{
						GET_PLAYER((PlayerTypes)iI).setCurrentEra((EraTypes)(GC.getTechInfo(eIndex).getEra()));
					}

					//	Reconsider civics on acquiring tech
					GET_PLAYER((PlayerTypes)iI).AI_setCivicTimer(0);

					//	Recalculate bonus values on acquiring a new tech
					GET_PLAYER((PlayerTypes)iI).AI_updateBonusValue();
				}
			}

			if (GC.getTechInfo(eIndex).isMapVisible())
			{
				GC.getMap().setRevealedPlots(getID(), true, true);
			}

			// report event to Python, along with some other key state
			CvEventReporter::getInstance().techAcquired(eIndex, getID(), ePlayer, bAnnounce);

			bool bReligionFounded = false;
			bool bClearResearchQueueAI = false;

			if (bFirst && GC.getGame().countKnownTechNumTeams(eIndex) == 1)
			{
				if (!GC.getGame().isOption(GAMEOPTION_DIVINE_PROPHETS)
				&& GC.getGame().isTechCanFoundReligion(eIndex))
				{
					for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
					{
						int iBestValue = MAX_INT;
						PlayerTypes eBestPlayer = NO_PLAYER;
						const ReligionTypes eSlotReligion = ReligionTypes(iI);

						if (GC.getReligionInfo(eSlotReligion).getTechPrereq() == eIndex
						&& !GC.getGame().isReligionSlotTaken(eSlotReligion))
						{
							ReligionTypes eReligion = NO_RELIGION;

							for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
							{
								const CvPlayer& kPlayerX = GET_PLAYER((PlayerTypes)iJ);
								if (kPlayerX.isAlive() && kPlayerX.getTeam() == getID() && kPlayerX.canFoundReligion())
								{
									int iValue = 10 + GC.getGame().getSorenRandNum(10, "Found Religion (Player)");

									for (int iK = 0; iK < GC.getNumReligionInfos(); iK++)
									{
										iValue += (kPlayerX.getHasReligionCount((ReligionTypes)iK));
									}
									if (kPlayerX.getCurrentResearch() != eIndex)
									{
										iValue *= 10;
									}

									if (iValue < iBestValue)
									{
										iBestValue = iValue;
										eBestPlayer = ((PlayerTypes)iJ);
										eReligion = ReligionTypes(iI);
									}
								}
							}
							if (eBestPlayer != NO_PLAYER)
							{
								if (GC.getGame().isOption(GAMEOPTION_PICK_RELIGION))
								{
									if (GET_PLAYER(eBestPlayer).isHuman())
									{
										GET_PLAYER(eBestPlayer).m_bChoosingReligion = true;
										CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_FOUND_RELIGION, iI);
										if (NULL != pInfo)
										{
											gDLL->getInterfaceIFace()->addPopup(pInfo, eBestPlayer);
										}
									}
									else
									{
										eReligion = GET_PLAYER(eBestPlayer).AI_chooseReligion();
										GET_PLAYER(eBestPlayer).foundReligion(eReligion, eSlotReligion, true);
										bReligionFounded = true;
									}
								}
								else
								{
									GET_PLAYER(eBestPlayer).foundReligion(eReligion, eSlotReligion, true);
									bReligionFounded = true;
								}
								bClearResearchQueueAI = true;
							}
							break; // Only one religion can be founded by a single tech as they are global techs.
						}
					}
				}
				for (int iI = 0; iI < GC.getNumCorporationInfos(); ++iI)
				{
					if (GC.getCorporationInfo((CorporationTypes)iI).getTechPrereq() == eIndex)
					{
						PlayerTypes eBestPlayer = NO_PLAYER;
						if (!(GC.getGame().isCorporationFounded((CorporationTypes)iI)))
						{
							int iBestValue = MAX_INT;

							for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
							{
								if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).getTeam() == getID())
								{
									int iValue = 10 + GC.getGame().getSorenRandNum(10, "Found Corporation (Player)");

									if (GET_PLAYER((PlayerTypes)iJ).getCurrentResearch() != eIndex)
									{
										iValue *= 10;
									}

									if (iValue < iBestValue)
									{
										iBestValue = iValue;
										eBestPlayer = ((PlayerTypes)iJ);
									}
								}
							}
						}
						if (eBestPlayer != NO_PLAYER)
						{
							GET_PLAYER(eBestPlayer).foundCorporation((CorporationTypes)iI);
							bClearResearchQueueAI = true;
						}
					}
				}
			}

			const bool bGlobal = GC.getTechInfo(eIndex).isGlobal();

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
					{
						if (GET_PLAYER((PlayerTypes)iI).isResearchingTech(eIndex))
						{
							GET_PLAYER((PlayerTypes)iI).popResearch(eIndex);
						}
						GET_PLAYER((PlayerTypes)iI).AI_nowHasTech(eIndex);
						GET_PLAYER((PlayerTypes)iI).invalidateYieldRankCache();
					}
					else if (bGlobal && GET_PLAYER((PlayerTypes)iI).isResearchingTech(eIndex))
					{
						GET_PLAYER((PlayerTypes)iI).popResearch(eIndex);
					}
				}
			}

			if (bFirst && GC.getGame().countKnownTechNumTeams(eIndex) == 1)
			{
				const UnitTypes eFreeUnit = GET_PLAYER(ePlayer).getTechFreeUnit(eIndex);
				if (eFreeUnit != NO_UNIT)
				{
					bClearResearchQueueAI = true;
					CvCity* pCapitalCity = GET_PLAYER(ePlayer).getCapitalCity();

					if (pCapitalCity != NULL)
					{
						pCapitalCity->createGreatPeople(eFreeUnit, false, false);
					}
				}
				//TB Prophet Mod begin
				if (GC.getGame().isOption(GAMEOPTION_DIVINE_PROPHETS))
				{
					const UnitTypes eFreeProphet = GET_PLAYER(ePlayer).getTechFreeProphet(eIndex);
					if (eFreeProphet != NO_UNIT)
					{
						bClearResearchQueueAI = true;
						CvCity* pCapitalCity = GET_PLAYER(ePlayer).getCapitalCity();

						if (pCapitalCity != NULL)
						{
							pCapitalCity->createGreatPeople(eFreeProphet, false, false);
						}
					}
				}
				//TB Prophet Mod end
				if (GC.getTechInfo(eIndex).getFirstFreeTechs() > 0)
				{
					bClearResearchQueueAI = true;

					if (!isHuman())
					{
						for (int iI = 0; iI < GC.getTechInfo(eIndex).getFirstFreeTechs(); iI++)
						{
							GET_PLAYER(ePlayer).AI_chooseFreeTech();
						}
					}
					else
					{
						GET_PLAYER(ePlayer).chooseTech(GC.getTechInfo(eIndex).getFirstFreeTechs(), gDLL->getText("TXT_KEY_MISC_FIRST_TECH_CHOOSE_FREE", GC.getTechInfo(eIndex).getTextKeyWide()));
					}

					CvWString szBuffer;
					for (int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							MEMORY_TRACK_EXEMPT();

							if (isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
							{
								szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_FIRST_TO_TECH", GET_PLAYER(ePlayer).getNameKey(), GC.getTechInfo(eIndex).getTextKeyWide());
							}
							else
							{
								szBuffer = gDLL->getText("TXT_KEY_MISC_UNKNOWN_FIRST_TO_TECH", GC.getTechInfo(eIndex).getTextKeyWide());
							}
							AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_FIRSTTOTECH", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
						}
					}

					szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_FIRST_TO_TECH", GET_PLAYER(ePlayer).getName(), GC.getTechInfo(eIndex).getTextKeyWide());
					GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, ePlayer, szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
				}

				if (bClearResearchQueueAI)
				{
					for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive() && !GET_PLAYER((PlayerTypes)iI).isHuman()
						&&  GET_PLAYER((PlayerTypes)iI).isResearchingTech(eIndex))
						{
							GET_PLAYER((PlayerTypes)iI).clearResearchQueue();
						}
					}
				}
			}

			if (bAnnounce)
			{
				if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
				{
					announceTechToPlayers(eIndex);

					for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
					{
						CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

						if (pLoopPlot->getTeam() == getID())
						{
							const BonusTypes eBonus = pLoopPlot->getBonusType();

							if (eBonus != NO_BONUS && GC.getBonusInfo(eBonus).getTechReveal() == eIndex && !isForceRevealedBonus(eBonus))
							{
								const CvCity* pCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), NO_PLAYER, getID(), false);

								if (pCity != NULL)
								{
									MEMORY_TRACK_EXEMPT();

									CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_DISCOVERED_BONUS", GC.getBonusInfo(eBonus).getTextKeyWide(), pCity->getNameKey());
									AddDLLMessage(pLoopPlot->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DISCOVERBONUS", MESSAGE_TYPE_INFO,
										GC.getBonusInfo(eBonus).getButton(), CvColorInfo::white(), pLoopPlot->getX(), pLoopPlot->getY(), true, true);
								}
							}
						}
					}
				}

				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					const CvPlayer& kPlayerX = GET_PLAYER((PlayerTypes)iI);

					if (kPlayerX.isAlive() && kPlayerX.isHuman() && kPlayerX.getTeam() == getID() && kPlayerX.canRevolution(NULL)
					&& (!bReligionFounded || kPlayerX.getLastStateReligion() != NO_RELIGION || iI != ePlayer))
					{
						CivicOptionTypes eCivicOptionType = NO_CIVICOPTION;
						CivicTypes eCivicType = NO_CIVIC;

						for (int iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
						{
							if (!kPlayerX.isHasCivicOption((CivicOptionTypes)iJ))
							{
								for (int iK = 0; iK < GC.getNumCivicInfos(); iK++)
								{
									if (GC.getCivicInfo((CivicTypes)iK).getCivicOptionType() == iJ
									&& GC.getCivicInfo((CivicTypes)iK).getTechPrereq() == eIndex)
									{
										eCivicOptionType = ((CivicOptionTypes)iJ);
										eCivicType = ((CivicTypes)iK);
									}
								}
							}
						}

						if ((eCivicOptionType != NO_CIVICOPTION) && (eCivicType != NO_CIVIC))
						{
							CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHANGECIVIC);
							if (NULL != pInfo)
							{
								pInfo->setData1(eCivicOptionType);
								pInfo->setData2(eCivicType);
								gDLL->getInterfaceIFace()->addPopup(pInfo, (PlayerTypes)iI);
							}
						}
					}
				}
			}

			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
				{
					GET_TEAM((TeamTypes)iI).updateTechShare(eIndex);
				}
			}


			if (bNewValue)
			{
				if (bAnnounce)
				{
					if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
					{
						FAssert(ePlayer != NO_PLAYER);
						if (GET_PLAYER(ePlayer).isResearch() && (GET_PLAYER(ePlayer).getCurrentResearch() == NO_TECH))
						{
							if (ePlayer == GC.getGame().getActivePlayer())
							{
								GET_PLAYER(ePlayer).chooseTech(0, gDLL->getText("TXT_KEY_MISC_WHAT_TO_RESEARCH_NEXT"));
							}
						}
					}
				}
			}

			if (getID() == GC.getGame().getActiveTeam())
			{
				gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
				gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
				gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
				gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
			}
		}
	}
}


bool CvTeam::isNoTradeTech(TechTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabNoTradeTech[eIndex];
}


void CvTeam::setNoTradeTech(TechTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_pabNoTradeTech[eIndex] = bNewValue;
}


int CvTeam::getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiImprovementYieldChange[eIndex1][eIndex2];
}


void CvTeam::changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaaiImprovementYieldChange[eIndex1][eIndex2] = (m_ppaaiImprovementYieldChange[eIndex1][eIndex2] + iChange);
		FAssert(eIndex1 >= 0);
		FAssert(eIndex2 >= 0);

		updateYield();
	}
}
// Protected Functions...

void CvTeam::doWarWeariness()
{
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (getWarWeariness((TeamTypes)iI) > 0)
		{
			changeWarWeariness(((TeamTypes)iI), 100 * GC.getDefineINT("WW_DECAY_RATE"));

			// War weariness decays faster when enemy can't fight, usually a rebel with just spies
			if (!GET_TEAM((TeamTypes)iI).isAlive() || GET_TEAM((TeamTypes)iI).getNumMilitaryUnits() == 0 || !isAtWar((TeamTypes)iI))
			{
				setWarWeariness(((TeamTypes)iI), ((getWarWeariness((TeamTypes)iI) * GC.getDefineINT("WW_DECAY_PEACE_PERCENT")) / 100));
			}
		}
	}
}

void CvTeam::updateTechShare(TechTypes eTech)
{
	if (isHasTech(eTech))
	{
		return;
	}

	int iBestShare = MAX_INT;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (isTechShare(iI))
		{
			iBestShare = std::min(iBestShare, iI + 1);
		}
	}

	if (iBestShare != MAX_INT)
	{
		int iCount = 0;

		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if(GET_TEAM((TeamTypes)iI).isAlive()
			&& GET_TEAM((TeamTypes)iI).isHasTech(eTech)
			&& isHasMet((TeamTypes)iI))
			{
				FAssertMsg(iI != getID(), "iI is not expected to be equal with getID()");
				iCount++;
			}
		}

		if (iCount >= iBestShare)
		{
			setHasTech(eTech, true, NO_PLAYER, true, true);
		}
	}
}


void CvTeam::updateTechShare()
{
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		updateTechShare((TechTypes)iI);
	}
}


void CvTeam::testCircumnavigated()
{
	if (isNPC() || !GC.getGame().circumnavigationAvailable())
	{
		return;
	}
	if (GC.getMap().isWrapX())
	{
		for (int iX = 0; iX < GC.getMap().getGridWidth(); iX++)
		{
			bool bFoundVisible = false;

			for (int iY = 0; iY < GC.getMap().getGridHeight(); iY++)
			{
				const CvPlot* pPlot = GC.getMap().plotSorenINLINE(iX, iY);

				if (pPlot->isRevealed(getID(), false))
				{
					bFoundVisible = true;
					break;
				}
			}
			if (!bFoundVisible)
			{
				return;
			}
		}
	}
	if (GC.getMap().isWrapY())
	{
		for (int iY = 0; iY < GC.getMap().getGridHeight(); iY++)
		{
			bool bFoundVisible = false;

			for (int iX = 0; iX < GC.getMap().getGridWidth(); iX++)
			{
				const CvPlot* pPlot = GC.getMap().plotSorenINLINE(iX, iY);

				if (pPlot->isRevealed(getID(), false))
				{
					bFoundVisible = true;
					break;
				}
			}
			if (!bFoundVisible)
			{
				return;
			}
		}
	}
	GC.getGame().setCircumnavigatedTeam(getID());

	// Announce and record history
	CvWString szBuffer;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			MEMORY_TRACK_EXEMPT();

			if (getID() == GET_PLAYER((PlayerTypes)iI).getTeam())
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_CIRC_GLOBE", GC.getDefineINT("CIRCUMNAVIGATE_FREE_MOVES"));
			}
			else if (isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_CIRC_GLOBE", getName().GetCString());
			}
			else
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_UNKNOWN_CIRC_GLOBE");
			}
			AddDLLMessage((PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GLOBECIRCUMNAVIGATED",
				MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
		}
	}
	szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_CIRC_GLOBE", getName().GetCString());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
}

void CvTeam::setCircumnavigated(bool bNewValue)
{
	if (bNewValue)
	{
		changeExtraMoves(DOMAIN_SEA, GC.getDefineINT("CIRCUMNAVIGATE_FREE_MOVES"));
	}
	else changeExtraMoves(DOMAIN_SEA, -GC.getDefineINT("CIRCUMNAVIGATE_FREE_MOVES"));
}


void CvTeam::processTech(TechTypes eTech, int iChange, bool bAnnounce)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	BonusTypes eBonus;
	int iI, iJ;

	if (GC.getTechInfo(eTech).isExtraWaterSeeFrom())
	{
		changeExtraWaterSeeFromCount(iChange);
	}

	if (iChange > 0 && GC.getTechInfo(eTech).isMapCentering())
	{
		setMapCentering(true);
	}

	if (GC.getTechInfo(eTech).isMapTrading())
	{
		changeMapTradingCount(iChange);
	}

	const CvTechInfo& kTech = GC.getTechInfo(eTech);
	if (kTech.isCanPassPeaks())
	{
		changeCanPassPeaksCount(iChange);
		//	Koshling - makes peaks workable which chnages the yield calculation
		updateYield();
	}
	if (kTech.isMoveFastPeaks())
	{
		changeMoveFastPeaksCount(iChange);
	}
	if (kTech.isCanFoundOnPeaks())
	{
		changeCanFoundOnPeaksCount(iChange);
	}

	if (iChange > 0)
	{
		ObsoletePromotions(eTech);
		ObsoleteCorporations(eTech);
		GC.getGame().makeTechDiscovered(eTech);
	}

	if (kTech.isEmbassyTrading())
	{
		changeEmbassyTradingCount(iChange);
	}

	if (kTech.isRebaseAnywhere())
	{
		changeRebaseAnywhereCount(iChange);
	}

	if (kTech.getGlobalTradeModifier() != 0)
	{
		changeTradeModifier(kTech.getGlobalTradeModifier() * iChange);
	}

	if (kTech.getGlobalForeignTradeModifier() != 0)
	{
		changeForeignTradeModifier(kTech.getGlobalForeignTradeModifier() * iChange);
	}

	if (kTech.getTradeMissionModifier() != 0)
	{
		changeTradeMissionModifier(kTech.getTradeMissionModifier() * iChange);
	}

	if (kTech.getCorporationRevenueModifier() != 0)
	{
		changeCorporationRevenueModifier(kTech.getCorporationRevenueModifier() * iChange);
	}

	if (kTech.getCorporationMaintenanceModifier() != 0)
	{
		changeCorporationMaintenanceModifier(kTech.getCorporationMaintenanceModifier() * iChange);
	}

	if (kTech.isEnablesDesertFarming())
	{
		changeCanFarmDesertCount(iChange);
		setLastRoundOfValidImprovementCacheUpdate();
	}

	if (GC.getTechInfo(eTech).isTechTrading())
	{
		changeTechTradingCount(iChange);
	}

	if (GC.getTechInfo(eTech).isGoldTrading())
	{
		changeGoldTradingCount(iChange);
	}

	if (GC.getTechInfo(eTech).isOpenBordersTrading())
	{
		changeOpenBordersTradingCount(iChange);
		changeLimitedBordersTradingCount(iChange);
	}

	if (GC.getTechInfo(eTech).isDefensivePactTrading())
	{
		changeDefensivePactTradingCount(iChange);
	}

	if (GC.getTechInfo(eTech).isPermanentAllianceTrading())
	{
		changePermanentAllianceTradingCount(iChange);
	}

	if (GC.getTechInfo(eTech).isVassalStateTrading())
	{
		changeVassalTradingCount(iChange);
	}

	if (GC.getTechInfo(eTech).isBridgeBuilding())
	{
		changeBridgeBuildingCount(iChange);
	}

	if (GC.getTechInfo(eTech).isIrrigation())
	{
		changeIrrigationCount(iChange);
		setLastRoundOfValidImprovementCacheUpdate();
	}

	if (GC.getTechInfo(eTech).isIgnoreIrrigation())
	{
		changeIgnoreIrrigationCount(iChange);
		setLastRoundOfValidImprovementCacheUpdate();
	}

	if (GC.getTechInfo(eTech).isWaterWork())
	{
		changeWaterWorkCount(iChange);
		setLastRoundOfValidImprovementCacheUpdate();
	}

	for (iI = 0; iI < GC.getNumRouteInfos(); iI++)
	{
		changeRouteChange(((RouteTypes)iI), (GC.getRouteInfo((RouteTypes) iI).getTechMovementChange(eTech) * iChange));
	}

	for (iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		changeExtraMoves(((DomainTypes)iI), (GC.getTechInfo(eTech).getDomainExtraMoves(iI) * iChange));
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		if (GC.getTechInfo(eTech).isCommerceFlexible(iI))
		{
			changeCommerceFlexibleCount(((CommerceTypes)iI), iChange);
		}
	}

	for (iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		if (GC.getTechInfo(eTech).isTerrainTrade(iI))
		{
			changeTerrainTradeCount(((TerrainTypes)iI), iChange);
		}
	}

	if (GC.getTechInfo(eTech).isRiverTrade())
	{
		changeRiverTradeCount(iChange);
	}

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes) iI).getObsoleteTech() == eTech)
		{
			changeObsoleteBuildingCount((BuildingTypes)iI, iChange);
		}
		if (GC.getBuildingInfo((BuildingTypes) iI).getSpecialBuildingType() != NO_SPECIALBUILDING
		&& GC.getSpecialBuildingInfo((SpecialBuildingTypes)GC.getBuildingInfo((BuildingTypes)iI).getSpecialBuildingType()).getObsoleteTech() == eTech)
		{
			changeObsoleteBuildingCount((BuildingTypes)iI, iChange);
		}

		for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			changeBuildingCommerceChange(((BuildingTypes)iI), ((CommerceTypes)iJ), (GC.getBuildingInfo((BuildingTypes)iI).getTechCommerceChange(eTech, iJ) * iChange));
			changeBuildingCommerceModifier(((BuildingTypes)iI), ((CommerceTypes)iJ), (GC.getBuildingInfo((BuildingTypes)iI).getTechCommerceModifier(eTech, iJ) * iChange));
		}

		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeBuildingYieldChange(((BuildingTypes)iI), ((YieldTypes)iJ), (GC.getBuildingInfo((BuildingTypes)iI).getTechYieldChange(eTech, iJ) * iChange));
			changeBuildingYieldModifier(((BuildingTypes)iI), ((YieldTypes)iJ), (GC.getBuildingInfo((BuildingTypes)iI).getTechYieldModifier(eTech, iJ) * iChange));
		}

		for (iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			changeBuildingSpecialistChange(((BuildingTypes)iI), ((SpecialistTypes)iJ), (GC.getBuildingInfo((BuildingTypes)iI).getTechSpecialistChange(eTech, iJ) * iChange));
		}
	}

	for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (GC.getImprovementInfo((ImprovementTypes)iI).getTechYieldChanges(eTech, iJ) * iChange));
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).changeFeatureProductionModifier(GC.getTechInfo(eTech).getFeatureProductionModifier() * iChange);
			GET_PLAYER((PlayerTypes)iI).changeWorkerSpeedModifier(GC.getTechInfo(eTech).getWorkerSpeedModifier() * iChange);
			GET_PLAYER((PlayerTypes)iI).changeTradeRoutes(GC.getTechInfo(eTech).getTradeRoutes() * iChange);
			GET_PLAYER((PlayerTypes)iI).changeExtraHealth(GC.getTechInfo(eTech).getHealth() * iChange);
			GET_PLAYER((PlayerTypes)iI).changeExtraHappiness(GC.getTechInfo(eTech).getHappiness() * iChange);

			//DPII < Maintenance Modifiers >
			GET_PLAYER((PlayerTypes)iI).changeDistanceMaintenanceModifier(GC.getTechInfo(eTech).getDistanceMaintenanceModifier() * iChange);
			GET_PLAYER((PlayerTypes)iI).changeNumCitiesMaintenanceModifier(GC.getTechInfo(eTech).getNumCitiesMaintenanceModifier() * iChange);
			GET_PLAYER((PlayerTypes)iI).changeMaintenanceModifier(GC.getTechInfo(eTech).getMaintenanceModifier() * iChange);
			GET_PLAYER((PlayerTypes)iI).changeCoastalDistanceMaintenanceModifier(GC.getTechInfo(eTech).getCoastalDistanceMaintenanceModifier() * iChange);
			//DPII < Maintenance Modifiers >

			GET_PLAYER((PlayerTypes)iI).changeAssets(GC.getTechInfo(eTech).getAssetValue() * iChange);
			GET_PLAYER((PlayerTypes)iI).changeTechPower(GC.getTechInfo(eTech).getPowerValue() * iChange);
			GET_PLAYER((PlayerTypes)iI).changeTechScore(getTechScore(eTech) * iChange);

			GET_PLAYER((PlayerTypes)iI).changeTechInflation(kTech.getInflationModifier() * iChange);

			//ls612: Tech Commerce Modifiers (repositioned for optimal processing by TB)
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				GET_PLAYER((PlayerTypes)iI).changeCommerceRateModifier(((CommerceTypes)iJ), GC.getTechInfo(eTech).getCommerceModifier(iJ) * iChange);
			}
			GET_PLAYER((PlayerTypes)iI).updateTechHappinessandHealth();
		}
	}

	for (iI = 0; iI < GC.getNumBuildingInfos(); ++iI)
	{
		changeTechExtraBuildingHappiness((BuildingTypes)iI, GC.getBuildingInfo((BuildingTypes)iI).getTechHappinessChanges(eTech) * iChange);
		changeTechExtraBuildingHealth((BuildingTypes)iI, GC.getBuildingInfo((BuildingTypes)iI).getTechHealthChanges(eTech) * iChange);
	}
	for (iI = 0; iI < GC.getNumUnitInfos(); ++iI)
	{
		changeUnitStrengthChange((UnitTypes)iI, kTech.getUnitStrengthChange(iI) * iChange);
	}

	for (iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndex(iI);

		eBonus = pLoopPlot->getBonusType();

		if (eBonus != NO_BONUS)
		{
			if (GC.getBonusInfo(eBonus).getTechReveal() == eTech)
			{
				pLoopPlot->updateYield();
				pLoopPlot->setLayoutDirty(true);
			}
		}
	}

	for (iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		if (GC.getBuildInfo((BuildTypes) iI).getTechPrereq() == eTech)
		{
			setLastRoundOfValidImprovementCacheUpdate();
			if (GC.getBuildInfo((BuildTypes) iI).getRoute() != NO_ROUTE)
			{
				for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).getTeam() == getID() && GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						GET_PLAYER((PlayerTypes)iJ).processNewRoutes();
					}
				}
			}
			break;
		}
	}
	if (getLastRoundOfValidImprovementCacheUpdate() != GC.getGame().getGameTurn())
	{
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			if (GC.getImprovementInfo((ImprovementTypes) iI).getPrereqTech() == eTech)
			{
				setLastRoundOfValidImprovementCacheUpdate();
				break;
			}
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).updateCorporation();

			//	A new tech can effect best plot build decisions so mark stale in all cities
			foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				pLoopCity->AI_markBestBuildValuesStale();
			}
		}
	}

	if ( iChange > 0 )
	{
		for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); ++iI)
		{
			if (eTech == GC.getSpecialBuildingInfo((SpecialBuildingTypes)iI).getTechPrereqAnyone())
			{
				GC.getGame().makeSpecialBuildingValid((SpecialBuildingTypes)iI, bAnnounce);
			}
		}
	}
}


void CvTeam::cancelDefensivePacts()
{
	CLLNode<TradeData>* pNode;
	CvDeal* pLoopDeal;
	bool bCancelDeal;
	int iLoop;

	for (pLoopDeal = GC.getGame().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGame().nextDeal(&iLoop))
	{
		bCancelDeal = false;

		if ((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == getID()) ||
			(GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == getID()))
		{
			for (pNode = pLoopDeal->headFirstTradesNode(); (pNode != NULL); pNode = pLoopDeal->nextFirstTradesNode(pNode))
			{
				if (pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT)
				{
					bCancelDeal = true;
					break;
				}
			}

			if (!bCancelDeal)
			{
				for (pNode = pLoopDeal->headSecondTradesNode(); (pNode != NULL); pNode = pLoopDeal->nextSecondTradesNode(pNode))
				{
					if (pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT)
					{
						bCancelDeal = true;
						break;
					}
				}
			}
		}

		if (bCancelDeal)
		{
			pLoopDeal->kill();
		}
	}
}


bool CvTeam::isFriendlyTerritory(TeamTypes eTeam) const
{
	if (eTeam == NO_TEAM)
	{
		return false;
	}

	if (eTeam == getID())
	{
		return true;
	}

	if (GET_TEAM(eTeam).isVassal(getID()))
	{
		return true;
	}

	if (isVassal(eTeam) && isOpenBorders(eTeam))
	{
		return true;
	}

	return false;
}

int CvTeam::getEspionagePointsAgainstTeam(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiEspionagePointsAgainstTeam[eIndex];
}

void CvTeam::setEspionagePointsAgainstTeam(TeamTypes eIndex, int iValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iValue != getEspionagePointsAgainstTeam(eIndex))
	{
		m_aiEspionagePointsAgainstTeam[eIndex] = iValue;

		verifySpyUnitsValidPlot();
		GET_TEAM(eIndex).verifySpyUnitsValidPlot();
	}
}

void CvTeam::changeEspionagePointsAgainstTeam(TeamTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	setEspionagePointsAgainstTeam(eIndex, getEspionagePointsAgainstTeam(eIndex) + iChange);
}

int CvTeam::getEspionagePointsEver() const
{
	return m_iEspionagePointsEver;
}

void CvTeam::setEspionagePointsEver(int iValue)
{
	if (iValue != getEspionagePointsEver())
	{
		m_iEspionagePointsEver = iValue;
	}
}

void CvTeam::changeEspionagePointsEver(int iChange)
{
	setEspionagePointsEver(getEspionagePointsEver() + iChange);
}

int CvTeam::getCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCounterespionageTurnsLeftAgainstTeam[eIndex];
}

void CvTeam::setCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex, int iValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iValue != getCounterespionageTurnsLeftAgainstTeam(eIndex))
	{
		m_aiCounterespionageTurnsLeftAgainstTeam[eIndex] = iValue;

		gDLL->getInterfaceIFace()->setDirty(Espionage_Advisor_DIRTY_BIT, true);
	}
}

void CvTeam::changeCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	setCounterespionageTurnsLeftAgainstTeam(eIndex, getCounterespionageTurnsLeftAgainstTeam(eIndex) + iChange);
}

int CvTeam::getCounterespionageModAgainstTeam(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCounterespionageModAgainstTeam[eIndex];
}

void CvTeam::setCounterespionageModAgainstTeam(TeamTypes eIndex, int iValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iValue != getCounterespionageModAgainstTeam(eIndex))
	{
		m_aiCounterespionageModAgainstTeam[eIndex] = iValue;

		gDLL->getInterfaceIFace()->setDirty(Espionage_Advisor_DIRTY_BIT, true);
	}
}

void CvTeam::changeCounterespionageModAgainstTeam(TeamTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	setCounterespionageModAgainstTeam(eIndex, getCounterespionageModAgainstTeam(eIndex) + iChange);
}

void CvTeam::verifySpyUnitsValidPlot()
{
	std::vector<CvUnit*> aUnits;

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);

		if (kPlayer.isAlive() && kPlayer.getTeam() == getID())
		{
			foreach_(CvUnit* pUnit, kPlayer.units())
			{
				if ( pUnit->plot() != NULL )
				{
					const PlayerTypes eOwner = pUnit->plot()->getOwner();
					if (NO_PLAYER != eOwner)
					{
						if (pUnit->isSpy())
						{
							if (!kPlayer.canSpiesEnterBorders(eOwner))
							{
								aUnits.push_back(pUnit);
							}
						}
					}
				}
			}
		}
	}

	for (uint i = 0; i < aUnits.size(); i++)
	{
		aUnits[i]->jumpToNearestValidPlot();
	}
}


void CvTeam::setForceRevealedBonus(BonusTypes eBonus, bool bRevealed)
{
	if (isForceRevealedBonus(eBonus) == bRevealed)
	{
		return;
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); ++iI)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getBonusType() == eBonus)
		{
			if (pLoopPlot->getTeam() == getID())
			{
				pLoopPlot->updatePlotGroupBonus(false);
			}
		}
	}

	if (bRevealed)
	{
		m_aeRevealedBonuses.push_back(eBonus);
	}
	else
	{
		std::vector<BonusTypes>::iterator it;

		for (it = m_aeRevealedBonuses.begin(); it != m_aeRevealedBonuses.end(); ++it)
		{
			if (*it == eBonus)
			{
				m_aeRevealedBonuses.erase(it);
				break;
			}
		}
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); ++iI)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getBonusType() == eBonus)
		{
			if (pLoopPlot->getTeam() == getID())
			{
				pLoopPlot->updatePlotGroupBonus(true);
			}
		}
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); ++iI)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getBonusType() == eBonus)
		{
			pLoopPlot->updateYield();
			pLoopPlot->setLayoutDirty(true);
		}
	}
}

bool CvTeam::isForceRevealedBonus(BonusTypes eBonus) const
{
	std::vector<BonusTypes>::const_iterator it;

	for (it = m_aeRevealedBonuses.begin(); it != m_aeRevealedBonuses.end(); ++it)
	{
		if (*it == eBonus)
		{
			return true;
		}
	}

	return false;
}

int CvTeam::countNumHumanGameTurnActive() const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);

		if (kLoopPlayer.isHuman() && kLoopPlayer.getTeam() == getID())
		{
			if (kLoopPlayer.isTurnActive())
			{
				++iCount;
			}
		}
	}

	return iCount;
}

void CvTeam::setTurnActive(bool bNewValue, bool bDoTurn)
{
	FAssert(GC.getGame().isSimultaneousTeamTurns());

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kPlayer.isAlive() && kPlayer.getTeam() == getID())
		{
			kPlayer.setTurnActive(bNewValue, bDoTurn);
		}
	}
}

bool CvTeam::isTurnActive() const
{
	FAssert(GC.getGame().isSimultaneousTeamTurns());

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kPlayer.getTeam() == getID() && kPlayer.isAlive())
		{
			if (kPlayer.isTurnActive())
			{
				return true;
			}
		}
	}

	return false;
}


void CvTeam::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// Init data before load
	reset();

	uint uiFlag=0;
	WRAPPER_READ(wrapper, "CvTeam", &uiFlag);	// flags for expansion

	WRAPPER_READ(wrapper, "CvTeam", &m_iNumMembers);
	WRAPPER_READ(wrapper, "CvTeam", &m_iAliveCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iEverAliveCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iNumCities);
	WRAPPER_READ(wrapper, "CvTeam", &m_iTotalPopulation);
	WRAPPER_READ(wrapper, "CvTeam", &m_iTotalLand);
	WRAPPER_READ(wrapper, "CvTeam", &m_iNukeInterception);
	WRAPPER_READ(wrapper, "CvTeam", &m_iExtraWaterSeeFromCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iMapTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iTechTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iGoldTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iOpenBordersTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iDefensivePactTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iPermanentAllianceTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iVassalTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iBridgeBuildingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iIrrigationCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iIgnoreIrrigationCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iWaterWorkCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iVassalPower);
	WRAPPER_READ(wrapper, "CvTeam", &m_iMasterPower);
	WRAPPER_READ(wrapper, "CvTeam", &m_iEnemyWarWearinessModifier);
	WRAPPER_READ(wrapper, "CvTeam", &m_iRiverTradeCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iEspionagePointsEver);

	WRAPPER_READ(wrapper, "CvTeam", &m_bMapCentering);
	WRAPPER_READ(wrapper, "CvTeam", &m_bCapitulated);

	WRAPPER_READ(wrapper, "CvTeam", (int*)&m_eID);

	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiStolenVisibilityTimer);

	//	Format change - we now store 100 times the actual value, but no need to change the save format - just
	//	convert on load and save
	WRAPPER_READ_ARRAY_DECORATED(wrapper, "CvTeam", MAX_TEAMS, m_aiWarWearinessTimes100, "m_aiWarWeariness");

	for(int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiWarWearinessTimes100[iI] *= 100;
	}

	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiTechShareCount);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiEspionagePointsAgainstTeam);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiCounterespionageTurnsLeftAgainstTeam);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiCounterespionageModAgainstTeam);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_COMMERCE_TYPES, m_aiCommerceFlexibleCount);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_DOMAIN_TYPES, m_aiExtraMoves);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_aiForceTeamVoteEligibilityCount);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abHasMet);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abAtWar);
	//	Koshling - fix bugged state where the ganme thinks a team is at war with itself!
	if ( m_abAtWar[getID()] )
	{
		FAssertMsg(false,"Team flagged as at war with itself (correcting)");
		m_abAtWar[getID()] = false;
	}
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abPermanentWarPeace);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abOpenBorders);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abDefensivePact);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abForcePeace);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abVassal);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VICTORIES, GC.getNumVictoryInfos(), m_abCanLaunch);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abIsRebelAgainst);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_ROUTES, GC.getNumRouteInfos(), m_paiRouteChange);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectDefaultArtTypes);

	//project art types
	for(int i=0;i<wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_PROJECTS);i++)
	{
		int temp;
		ProjectTypes eNewProjectType = (ProjectTypes)wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_PROJECTS, i, true);
		if ( eNewProjectType != -1 )
		{
			for(int j=0;j<m_paiProjectCount[eNewProjectType];j++)
			{
				//	Decorasted this with a rather obscure name to match the export since when I fixed this
				//	bug I didn't want the export format to chnage uneccessarily
				WRAPPER_READ_DECORATED(wrapper, "CvTeam", &temp, "m_pavProjectArtTypes[][]");
				m_pavProjectArtTypes[eNewProjectType].push_back(temp);
			}
		}
	}

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectMaking);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiObsoleteBuildingCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiResearchProgress);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TERRAINS, GC.getNumTerrainInfos(), m_paiTerrainTradeCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VICTORIES, GC.getNumVictoryInfos(), m_aiVictoryCountdown);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_pabHasTech);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_pabNoTradeTech);

	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_IMPROVEMENTS); ++i)
	{
		int	newIndex = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_IMPROVEMENTS, i, true);

		if ( newIndex != -1 )
		{
			WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[newIndex]);
		}
		else
		{
			//	Consume the values
			WRAPPER_SKIP_ELEMENT(wrapper, "CvTeam", m_ppaaiImprovementYieldChange[i], SAVE_VALUE_ANY);
		}
	}

	uint iSize = 0;
	m_aeRevealedBonuses.clear();
	WRAPPER_READ(wrapper, "CvTeam", &iSize);
	for (uint i = 0; i < iSize; ++i)
	{
		BonusTypes eBonus;
		WRAPPER_READ_CLASS_ENUM(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BONUSES, (int*)&eBonus);
		m_aeRevealedBonuses.push_back(eBonus);
	}
	WRAPPER_READ(wrapper, "CvTeam", &m_iCanPassPeaksCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iMoveFastPeaksCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iCanFoundOnPeaksCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iEmbassyTradingCount);

	WRAPPER_READ(wrapper, "CvTeam", &m_iLimitedBordersTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iCanFarmDesertCount);

	WRAPPER_READ(wrapper, "CvTeam", &m_iRebaseAnywhereCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iForeignTradeModifier);
	WRAPPER_READ(wrapper, "CvTeam", &m_iTradeModifier);
	WRAPPER_READ(wrapper, "CvTeam", &m_iTradeMissionModifier);
	WRAPPER_READ(wrapper, "CvTeam", &m_iCorporationRevenueModifier);
	WRAPPER_READ(wrapper, "CvTeam", &m_iCorporationMaintenanceModifier);

	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abEmbassy);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abLimitedBorders);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abFreeTrade);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiTechExtraBuildingHappiness);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiTechExtraBuildingHealth);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitStrengthChange);
	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_BUILDINGS); ++i)
	{
		int	newIndex = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, i, true);

		if ( newIndex != -1 )
		{
			WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_COMMERCE_TYPES, m_ppiBuildingCommerceChange[newIndex]);
			WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_YIELD_TYPES, m_ppiBuildingYieldChange[newIndex]);
			WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_ppiBuildingSpecialistChange[newIndex]);
			WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_COMMERCE_TYPES, m_ppiBuildingCommerceModifier[newIndex]);
			WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_YIELD_TYPES, m_ppiBuildingYieldModifier[newIndex]);
		}
		else
		{
			//	Consume the values
			WRAPPER_SKIP_ELEMENT(wrapper, "CvTeam", m_ppiBuildingCommerceChange[newIndex], SAVE_VALUE_TYPE_INT_ARRAY);
			WRAPPER_SKIP_ELEMENT(wrapper, "CvTeam", m_ppiBuildingYieldChange[newIndex], SAVE_VALUE_TYPE_INT_ARRAY);
			WRAPPER_SKIP_ELEMENT(wrapper, "CvTeam", m_ppiBuildingSpecialistChange[newIndex], SAVE_VALUE_TYPE_CLASS_INT_ARRAY);
			WRAPPER_SKIP_ELEMENT(wrapper, "CvTeam", m_ppiBuildingCommerceModifier[newIndex], SAVE_VALUE_TYPE_INT_ARRAY);
			WRAPPER_SKIP_ELEMENT(wrapper, "CvTeam", m_ppiBuildingYieldModifier[newIndex], SAVE_VALUE_TYPE_INT_ARRAY);
		}
	}

	m_Properties.readWrapper(pStream);

	WRAPPER_READ(wrapper, "CvTeam", &m_iLastRoundOfValidImprovementCacheUpdate);

	WRAPPER_READ_OBJECT_END(wrapper);
}


void CvTeam::write(FDataStreamBase* pStream)
{
	int iI;

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	uint uiFlag = 0;
	WRAPPER_WRITE(wrapper, "CvTeam", uiFlag);		// flag for expansion

	WRAPPER_WRITE(wrapper, "CvTeam", m_iNumMembers);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iAliveCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iEverAliveCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iNumCities);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iTotalPopulation);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iTotalLand);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iNukeInterception);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iExtraWaterSeeFromCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iMapTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iTechTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iGoldTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iOpenBordersTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iDefensivePactTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iPermanentAllianceTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iVassalTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iBridgeBuildingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iIrrigationCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iIgnoreIrrigationCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iWaterWorkCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iVassalPower);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iMasterPower);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iEnemyWarWearinessModifier);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iRiverTradeCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iEspionagePointsEver);
	WRAPPER_WRITE(wrapper, "CvTeam", m_bMapCentering);
	WRAPPER_WRITE(wrapper, "CvTeam", m_bCapitulated);
	WRAPPER_WRITE(wrapper, "CvTeam", m_eID);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiStolenVisibilityTimer);

	//	Format change - we now store 100 times the actual value, but no need to change the save format - just
	//	convert on load and save
	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiWarWearinessTimes100[iI] /= 100;
	}

	WRAPPER_WRITE_ARRAY_DECORATED(wrapper, "CvTeam", MAX_TEAMS, m_aiWarWearinessTimes100, "m_aiWarWeariness");
	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiWarWearinessTimes100[iI] *= 100;
	}

	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiTechShareCount);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiEspionagePointsAgainstTeam);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiCounterespionageTurnsLeftAgainstTeam);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiCounterespionageModAgainstTeam);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_COMMERCE_TYPES, m_aiCommerceFlexibleCount);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_DOMAIN_TYPES, m_aiExtraMoves);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_aiForceTeamVoteEligibilityCount);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abHasMet);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abAtWar);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abPermanentWarPeace);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abOpenBorders);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abDefensivePact);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abForcePeace);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abVassal);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VICTORIES, GC.getNumVictoryInfos(), m_abCanLaunch);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abIsRebelAgainst);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_ROUTES, GC.getNumRouteInfos(), m_paiRouteChange);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectDefaultArtTypes);

	//project art types
	for(int i=0;i<GC.getNumProjectInfos();i++)
	{
		for(int j=0;j<m_paiProjectCount[i];j++)
		{
			WRAPPER_WRITE(wrapper, "CvTeam", m_pavProjectArtTypes[i][j]);
		}
	}

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectMaking);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiObsoleteBuildingCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiResearchProgress);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TERRAINS, GC.getNumTerrainInfos(), m_paiTerrainTradeCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VICTORIES, GC.getNumVictoryInfos(), m_aiVictoryCountdown);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_pabHasTech);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_pabNoTradeTech);

	for (iI=0;iI<GC.getNumImprovementInfos();iI++)
	{
		WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[iI]);
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvTeam", m_aeRevealedBonuses.size(), "iSize" );
	for (std::vector<BonusTypes>::iterator it = m_aeRevealedBonuses.begin(); it != m_aeRevealedBonuses.end(); ++it)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BONUSES, *it, "eBonus");
	}

	WRAPPER_WRITE(wrapper, "CvTeam", m_iCanPassPeaksCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iMoveFastPeaksCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iCanFoundOnPeaksCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iEmbassyTradingCount);

	WRAPPER_WRITE(wrapper, "CvTeam", m_iLimitedBordersTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iCanFarmDesertCount);

	WRAPPER_WRITE(wrapper, "CvTeam", m_iRebaseAnywhereCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iForeignTradeModifier);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iTradeModifier);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iTradeMissionModifier);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iCorporationRevenueModifier);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iCorporationMaintenanceModifier);

	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abEmbassy);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abLimitedBorders);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abFreeTrade);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiTechExtraBuildingHappiness);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiTechExtraBuildingHealth);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitStrengthChange);

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_COMMERCE_TYPES, m_ppiBuildingCommerceChange[iI]);
		WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_YIELD_TYPES, m_ppiBuildingYieldChange[iI]);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_ppiBuildingSpecialistChange[iI]);
		WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_COMMERCE_TYPES, m_ppiBuildingCommerceModifier[iI]);
		WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_YIELD_TYPES, m_ppiBuildingYieldModifier[iI]);
	}

	m_Properties.writeWrapper(pStream);

	WRAPPER_WRITE(wrapper, "CvTeam", m_iLastRoundOfValidImprovementCacheUpdate);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}


// CACHE: cache frequently used values
///////////////////////////////////////
bool CvTeam::hasShrine(ReligionTypes eReligion) const
{
	if (eReligion != NO_RELIGION)
	{
		CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);

		if (pHolyCity != NULL && GET_PLAYER(pHolyCity->getOwner()).getTeam() == getID())
			return pHolyCity->hasShrine(eReligion);
	}

	return false;
}

void CvTeam::getCompletedSpaceshipProjects(std::map<ProjectTypes, int>& mapProjects) const
{
	for (int i = 0; i < GC.getNumProjectInfos(); i++)
	{
		if (GC.getProjectInfo((ProjectTypes)i).isSpaceship())
		{
			mapProjects[(ProjectTypes)i] = getProjectCount((ProjectTypes)i);
		}
	}
}

int CvTeam::getProjectPartNumber(ProjectTypes eProject, bool bAssert) const
{
	int iNumBuilt = getProjectCount(eProject);
	for (int i = 0; i < iNumBuilt; i++)
	{
		int artType = getProjectArtType(eProject, i);
		if (artType < 0)
		{
			return i;
		}
	}

	//didn't find empty part number
	if (bAssert)
	{
		FAssertMsg(false, "Unknown part number.");
	}

	//return the last one
	return std::min(iNumBuilt, GC.getProjectInfo(eProject).getMaxTeamInstances() - 1);
}

bool CvTeam::hasLaunched() const
{
	VictoryTypes spaceVictory = GC.getGame().getSpaceVictory();
	if (spaceVictory != NO_VICTORY)
	{
		return (getVictoryCountdown(spaceVictory) >= 0);
	}
	return false;
}

bool CvTeam::isCanPassPeaks() const
{
	return (getCanPassPeaksCount() > 0);
}

int CvTeam::getCanPassPeaksCount() const
{
	return m_iCanPassPeaksCount;
}

void CvTeam::changeCanPassPeaksCount(int iChange)
{
	m_iCanPassPeaksCount = (m_iCanPassPeaksCount + iChange);
	FAssert(getCanPassPeaksCount() >= 0);
}

bool CvTeam::isMoveFastPeaks() const
{
	return (getMoveFastPeaksCount() > 0);
}

int CvTeam::getMoveFastPeaksCount() const
{
	return m_iMoveFastPeaksCount;
}

void CvTeam::changeMoveFastPeaksCount(int iChange)
{
	m_iMoveFastPeaksCount = (m_iMoveFastPeaksCount + iChange);
	FAssert(getMoveFastPeaksCount() >= 0);
}

bool CvTeam::isCanFoundOnPeaks() const
{
	return (getCanFoundOnPeaksCount() > 0);
}

int CvTeam::getCanFoundOnPeaksCount() const
{
	return m_iCanFoundOnPeaksCount;
}

void CvTeam::changeCanFoundOnPeaksCount(int iChange)
{
	m_iCanFoundOnPeaksCount = (m_iCanFoundOnPeaksCount + iChange);
	FAssert(getCanFoundOnPeaksCount() >= 0);
}

int CvTeam::getRebaseAnywhereCount() const
{
	return m_iRebaseAnywhereCount;
}

bool CvTeam::isRebaseAnywhere() const
{
	return m_iRebaseAnywhereCount > 0;
}

void CvTeam::changeRebaseAnywhereCount(int iChange)
{
	m_iRebaseAnywhereCount += iChange;
}
/* Returns the happiness added to a single building type by all acquired techs.
 *
 * eBuilding - the building type to look up
 */
int CvTeam::getTechExtraBuildingHappiness(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiTechExtraBuildingHappiness[eIndex];
}

/* Adds iChange to the current happiness added to a single building type by all acquired techs.
 * Called from processTech() when a tech is acquired or lost.
 *
 * eIndex - the building type to change
 * iChange - the additional happiness to add to the existing value
 */
void CvTeam::changeTechExtraBuildingHappiness(BuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		setTechExtraBuildingHappiness(eIndex, getTechExtraBuildingHappiness(eIndex) + iChange);
	}
}

int CvTeam::getTechExtraBuildingHealth(BuildingTypes eIndex) const
{

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiTechExtraBuildingHealth[eIndex];
}


void CvTeam::changeTechExtraBuildingHealth(BuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		setTechExtraBuildingHealth(eIndex, getTechExtraBuildingHealth(eIndex) + iChange);
	}
}

void CvTeam::AI_setAssignWorkDirtyInEveryPlayerCityWithActiveBuilding(BuildingTypes eBuilding)
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == getID())
		{
			foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
			{
				if (pLoopCity->getNumActiveBuilding(eBuilding) > 0)
				{
					pLoopCity->AI_setAssignWorkDirty(true);
				}
			}
		}
	}
}
/* Sets the happiness added to a single building type by all acquired techs.
 *
 * eIndex - the building type to change
 * iNewValue - the new happiness value for the building
 */
void CvTeam::setTechExtraBuildingHappiness(BuildingTypes eIndex, int iNewValue)
{
	if (m_paiTechExtraBuildingHappiness[eIndex] != iNewValue)
	{
		const int iOldValue = m_paiTechExtraBuildingHappiness[eIndex];
		m_paiTechExtraBuildingHappiness[eIndex] = iNewValue;

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
			if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == getID())
			{
				foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
				{
					const int iNumBuildings = pLoopCity->getNumActiveBuilding(eIndex);
					if (iNumBuildings > 0 && !pLoopCity->isReligiouslyDisabledBuilding(eIndex))
					{
						// Remove the old value
						if (iOldValue > 0)
						{
							pLoopCity->changeBuildingGoodHappiness(-iOldValue * iNumBuildings);
						}
						else if (iOldValue < 0)
						{
							pLoopCity->changeBuildingBadHappiness(-iOldValue * iNumBuildings);
						}

						// Add the new value
						if (iNewValue > 0)
						{
							pLoopCity->changeBuildingGoodHappiness(iNewValue * iNumBuildings);
						}
						else if (iNewValue < 0)
						{
							pLoopCity->changeBuildingBadHappiness(iNewValue * iNumBuildings);
						}
					}
				}
			}
		}
	}
}

void CvTeam::setTechExtraBuildingHealth(BuildingTypes eIndex, int iNewValue)
{
	if (m_paiTechExtraBuildingHealth[eIndex] != iNewValue)
	{
		const int iOldValue = m_paiTechExtraBuildingHealth[eIndex];
		m_paiTechExtraBuildingHealth[eIndex] = iNewValue;

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
			if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == getID())
			{
				foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
				{
					const int iNumBuildings = pLoopCity->getNumActiveBuilding(eIndex);
					if (iNumBuildings > 0 && !pLoopCity->isReligiouslyDisabledBuilding(eIndex))
					{
						// Remove the old value
						if (iOldValue > 0)
						{
							pLoopCity->changeBuildingGoodHealth(-iOldValue * iNumBuildings);
						}
						else if (iOldValue < 0)
						{
							pLoopCity->changeBuildingBadHealth(-iOldValue * iNumBuildings);
						}

						// Add the new value
						if (iNewValue > 0)
						{
							pLoopCity->changeBuildingGoodHealth(iNewValue * iNumBuildings);
						}
						else if (iNewValue < 0)
						{
							pLoopCity->changeBuildingBadHealth(iNewValue * iNumBuildings);
						}
					}
				}
			}
		}
	}
}


int CvTeam::getCanFarmDesertCount() const
{
	return m_iCanFarmDesertCount;
}

bool CvTeam::isCanFarmDesert() const
{
	return (getCanFarmDesertCount() > 0);
}

void CvTeam::changeCanFarmDesertCount(int iChange)
{
	m_iCanFarmDesertCount = (m_iCanFarmDesertCount + iChange);
	FAssert(getCanFarmDesertCount() >= 0);
}

int CvTeam::getLimitedBordersTradingCount() const
{
	return m_iLimitedBordersTradingCount;
}

bool CvTeam::isLimitedBordersTrading() const
{
	return (getLimitedBordersTradingCount() > 0);
}

void CvTeam::changeLimitedBordersTradingCount(int iChange)
{
	m_iLimitedBordersTradingCount = (m_iLimitedBordersTradingCount + iChange);
	FAssert(getLimitedBordersTradingCount() >= 0);
}

void CvTeam::signLimitedBorders(TeamTypes eTeam)
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	if (!isAtWar(eTeam) && (getID() != eTeam) && (!isOpenBorders(eTeam)))
	{
		setTradeItem(&item, TRADE_RITE_OF_PASSAGE);

		if (GET_PLAYER(getLeaderID()).canTradeItem(GET_TEAM(eTeam).getLeaderID(), item) && GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).canTradeItem(getLeaderID(), item))
		{
			ourList.clear();
			theirList.clear();

			ourList.insertAtEnd(item);
			theirList.insertAtEnd(item);

			GC.getGame().implementDeal(getLeaderID(), (GET_TEAM(eTeam).getLeaderID()), &ourList, &theirList);
		}
	}
}

bool CvTeam::canSignOpenBorders(TeamTypes eTeam) const
{
	if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY) && !isHasEmbassy(eTeam))
	{
		return false;
	}
	return true;
}

void CvTeam::sendAmbassador(TeamTypes eTeam)
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	if (!isAtWar(eTeam) && (getID() != eTeam))
	{
		setTradeItem(&item, TRADE_EMBASSY);

		if (GET_PLAYER(getLeaderID()).canTradeItem(GET_TEAM(eTeam).getLeaderID(), item) && GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).canTradeItem(getLeaderID(), item))
		{
			ourList.clear();
			theirList.clear();

			ourList.insertAtEnd(item);
			theirList.insertAtEnd(item);

			GC.getGame().implementDeal(getLeaderID(), (GET_TEAM(eTeam).getLeaderID()), &ourList, &theirList);
		}
	}
}


bool CvTeam::isLimitedBorders(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abLimitedBorders[eIndex];
}


bool CvTeam::isHasEmbassy(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abEmbassy[eIndex];
}

void CvTeam::setHasEmbassy(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isHasEmbassy(eIndex) != bNewValue)
	{
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				CvCity* pCapital = GET_PLAYER((PlayerTypes)iI).getCapitalCity();
				if (pCapital != NULL)
				{
					pCapital->plot()->updateSight(false, true);
				}
			}
		}
		m_abEmbassy[eIndex] = bNewValue;
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				CvCity* pCapital = GET_PLAYER((PlayerTypes)iI).getCapitalCity();
				if (pCapital != NULL)
				{
					pCapital->plot()->updateSight(true, true);
				}
			}
		}
	}
}

int CvTeam::getEmbassyTradingCount() const
{
	return m_iEmbassyTradingCount;
}

bool CvTeam::isEmbassyTrading() const
{
	return (getEmbassyTradingCount() > 0);
}

void CvTeam::changeEmbassyTradingCount(int iChange)
{
	if (iChange != 0)
	{
		m_iEmbassyTradingCount = (m_iEmbassyTradingCount + iChange);
	}
}

void CvTeam::ObsoletePromotions(TechTypes eObsoleteTech)
{
	if (eObsoleteTech != NO_TECH)
	{
		for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			if (GC.getPromotionInfo((PromotionTypes)iI).getObsoleteTech() == eObsoleteTech)
			{
				for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
				{
					const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
					if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).getID() == getID())
					{
						foreach_(CvUnit* pLoopUnit, kLoopPlayer.units())
						{
							pLoopUnit->checkPromotionObsoletion();
						}
					}
				}
				break;
			}
		}
	}
}

void CvTeam::ObsoleteCorporations(TechTypes eObsoleteTech)
{
	int iI;
	bool bValid = false;
	if (eObsoleteTech != NO_TECH)
	{
		for (iI = 0; iI < GC.getNumCorporationInfos(); iI++)
		{
			if ((GC.getCorporationInfo((CorporationTypes)iI).getObsoleteTech()) == eObsoleteTech)
			{
				bValid = true;
				break;
			}
		}
		if (bValid)
		{
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

				if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).getID() == getID())
				{
					foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
					{
						for (iI = 0; iI < GC.getNumCorporationInfos(); iI++)
						{
							if (pLoopCity->isHasCorporation((CorporationTypes)iI) && GC.getCorporationInfo((CorporationTypes)iI).getObsoleteTech() == eObsoleteTech)
							{
								pLoopCity->setHasCorporation((CorporationTypes)iI, false, false, false);
								//Remove the Corp HQ Building
								if (pLoopCity->isHeadquarters((CorporationTypes)iI))
								{
									for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
									{
										if ((CorporationTypes)GC.getBuildingInfo((BuildingTypes)iJ).getGlobalCorporationCommerce() == ((CorporationTypes)iI)
										|| GC.getBuildingInfo((BuildingTypes)iJ).getFoundsCorporation() == ((CorporationTypes)iI)
										|| GC.getBuildingInfo((BuildingTypes)iJ).getPrereqCorporation() == ((CorporationTypes)iI))
										{
											pLoopCity->setNumRealBuilding((BuildingTypes)iJ, 0);
										}
									}
									GC.getGame().setHeadquarters((CorporationTypes)iI, NULL, false);
									//Be Sure to Create a New Headquarters, ASAP
									GC.getGame().doFoundCorporation((CorporationTypes)iI, true);
								}
							}
						}
					}
				}
			}
		}
	}
}

int CvTeam::getTradeModifier() const
{
	return m_iTradeModifier;
}
void CvTeam::changeTradeModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iTradeModifier += iChange;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
			}
		}
	}
}

int CvTeam::getForeignTradeModifier() const
{
	return m_iForeignTradeModifier;
}

void CvTeam::changeForeignTradeModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iForeignTradeModifier += iChange;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() && GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
			}
		}
	}
}

int CvTeam::getTradeMissionModifier() const
{
	return m_iTradeMissionModifier;
}
void CvTeam::changeTradeMissionModifier(int iChange)
{
	m_iTradeMissionModifier += iChange;
}

int CvTeam::getCorporationRevenueModifier() const
{
	return m_iCorporationRevenueModifier;
}
void CvTeam::changeCorporationRevenueModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iCorporationRevenueModifier += iChange;
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).getID() == getID())
			{
				foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
				{
					pLoopCity->updateCorporation();
				}
			}
		}
	}
}

int CvTeam::getCorporationMaintenanceModifier() const
{
	return m_iCorporationMaintenanceModifier;
}

void CvTeam::changeCorporationMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iCorporationMaintenanceModifier += iChange;
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).getID() == getID())
			{
				foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
				{
					pLoopCity->updateCorporation();
				}
			}
		}
	}
}

int CvTeam::getTotalVictoryScore() const
{
	int iL;
	int iTotalVictoryScore = 0;

	int globalCulture = 0;
	int globalThreeCityCulture = 0;
	int globalPopulation = 0;
	int globalWonderScore = 0;

	int teamWonderScore = 0;
	int tempScore = 0;

	int totalTeamReligion = 0;
	int totalTeamLegendaryCities = 0;

	long globalPowerHistory =0;
	long teamPowerHistory =0;

	long tempPower =0;

	int globalLand = GC.getMap().getLandPlots();

	for (int iK = 0; iK < GC.getNumBuildingInfos(); iK++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iK).getMaxGlobalInstances() == 1)
		{
			globalWonderScore++;
		}
	}

	// Get the Religion Info First
	// By definition, global religion percent is 100, so we don't need a variable for it.
	// Note: This detects whether the TEAM has the holy city.

	for (iK = 0; iK < GC.getNumReligionInfos(); iK++)
	{
		if (hasHolyCity((ReligionTypes)iK))
		{
			// Player has holy city
			const int tempReligion = GC.getGame().calculateReligionPercent((ReligionTypes)iK);
			if (tempReligion > totalTeamReligion)
			{
				totalTeamReligion = tempReligion;
			}
		}
	}

	// Get land, population, culture totals for player and globally.
	// Also get the starship launches and diplovictories achieved.

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			//Calculate global totals while looping through

			globalCulture += GET_PLAYER((PlayerTypes)iI).processedNationalCulture();
			globalPopulation += GET_PLAYER((PlayerTypes)iI).getTotalPopulation();

			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				teamWonderScore += GET_PLAYER((PlayerTypes)iI).getSevoWondersScore(0);
			}
		}
	}

	// Get the power history sums

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			//Calculate global totals while looping through
			tempPower = 0;
			for (iL = 0; iL <= GC.getGame().getGameTurn(); iL++)
			{
				tempPower += GET_PLAYER((PlayerTypes)iI).getPowerHistory(iL);
			}

			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				teamPowerHistory += tempPower;
			}

			globalPowerHistory += tempPower;
		}
	}

	// Get the number of legendary cities owned by this team

	for (iK = 0; iK < MAX_PC_PLAYERS; iK++)
	{
		if ((GET_PLAYER((PlayerTypes)iK).getTeam() == getID()) && (GET_PLAYER((PlayerTypes)iK).isAlive()))
		{
			foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iK).cities())
			{
				// -2 is correct.  We need -1 to change from 'total num' to 'last index', and -1 to get the top level.
				if (pLoopCity->getCultureLevel() > GC.getNumCultureLevelInfos() - 2)
				{
					totalTeamLegendaryCities++;
				}
			}
		}
	}

	// Add the WonderScore component
	if (globalWonderScore > 0)
	{
		iTotalVictoryScore += int(teamWonderScore * 100 / globalWonderScore);
	}

	// Add the population score component
	if (globalPopulation > 0)
	{
		iTotalVictoryScore += int(getTotalPopulation() * 100 / globalPopulation);
	}

	// Add the land score component
	if (globalLand > 0)
	{
		iTotalVictoryScore += int(getTotalLand() * 100 / globalLand);
	}

	// Add the culture score component
	if (globalCulture > 0)
	{
		unsigned long long iTotalCulture;
		iTotalCulture = countTotalCulture();

		if ( iTotalCulture > MAX_INT/100 )
		{
			iTotalVictoryScore += int(countTotalCulture()/(globalCulture/100));
		}
		else
		{
			iTotalVictoryScore += int(countTotalCulture() * 100 / globalCulture);
		}
	}

	// Add the legendary cities component
	if (totalTeamLegendaryCities > 0)
	{
		iTotalVictoryScore += (30 * totalTeamLegendaryCities);
	}

	// Add the Power component
	if (globalPowerHistory > 0)
	{
		iTotalVictoryScore += (teamPowerHistory * 100 / globalPowerHistory);
	}

	// Add the Religion component
	iTotalVictoryScore += totalTeamReligion;

	//Starship points!  Big money.
	if (GC.getGame().getStarshipLaunched(getID()))
	{
		iTotalVictoryScore += 100;
	}

	return iTotalVictoryScore;
}


int CvTeam::getBuildingCommerceChange(BuildingTypes eIndex1, CommerceTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_COMMERCE_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppiBuildingCommerceChange[eIndex1][eIndex2];
}

void CvTeam::changeBuildingCommerceChange(BuildingTypes eIndex1, CommerceTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_COMMERCE_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		const int iOldValue = m_ppiBuildingCommerceChange[eIndex1][eIndex2];
		m_ppiBuildingCommerceChange[eIndex1][eIndex2] += iChange;

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
					{
						if (pLoopCity->getNumActiveBuilding(eIndex1) > 0)
						{
							if (!pLoopCity->isReligiouslyDisabledBuilding(eIndex1))
							{
								const int iExistingValue = pLoopCity->getBuildingCommerceChange(eIndex1, eIndex2);
								// set the new
								pLoopCity->updateCommerceRateByBuilding(eIndex1, eIndex2, (iExistingValue - iOldValue + getBuildingCommerceChange(eIndex1, eIndex2)));
							}
						}
					}
				}
			}
		}
		updateCommerce();
	}
}

int CvTeam::getBuildingYieldChange(BuildingTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppiBuildingYieldChange[eIndex1][eIndex2];
}

void CvTeam::changeBuildingYieldChange(BuildingTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		int iOldValue = getBuildingYieldChange(eIndex1, eIndex2);
		m_ppiBuildingYieldChange[eIndex1][eIndex2] += iChange;

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
					{
						if (pLoopCity->getNumActiveBuilding(eIndex1) > 0)
						{
							if (!pLoopCity->isReligiouslyDisabledBuilding(eIndex1))
							{
								pLoopCity->changeBaseYieldRate(eIndex2, getBuildingYieldChange(eIndex1, eIndex2));
							}
						}
					}
				}
			}
		}
	}
}

int CvTeam::getBuildingSpecialistChange(BuildingTypes eIndex1, SpecialistTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < GC.getNumSpecialistInfos(), "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppiBuildingSpecialistChange[eIndex1][eIndex2];
}

void CvTeam::changeBuildingSpecialistChange(BuildingTypes eIndex1, SpecialistTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < GC.getNumSpecialistInfos(), "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		int iOldValue = getBuildingSpecialistChange(eIndex1, eIndex2);
		m_ppiBuildingSpecialistChange[eIndex1][eIndex2] += iChange;

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
					{
						// remove the old
						pLoopCity->updateMaxSpecialistCount(eIndex1, eIndex2, -iOldValue);
						// set the new
						pLoopCity->updateMaxSpecialistCount(eIndex1, eIndex2, getBuildingSpecialistChange(eIndex1, eIndex2));
					}
				}
			}
		}
	}
}

int CvTeam::getBuildingCommerceModifier(BuildingTypes eIndex1, CommerceTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_COMMERCE_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppiBuildingCommerceModifier[eIndex1][eIndex2];
}

void CvTeam::changeBuildingCommerceModifier(BuildingTypes eIndex1, CommerceTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_COMMERCE_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		const int iOldValue = getBuildingCommerceModifier(eIndex1, eIndex2);
		m_ppiBuildingCommerceModifier[eIndex1][eIndex2] += iChange;

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
					{
						if (pLoopCity->getNumActiveBuilding(eIndex1) > 0)
						{
							if (!pLoopCity->isReligiouslyDisabledBuilding(eIndex1))
							{
								const int iExistingValue = pLoopCity->getBuildingCommerceModifier(eIndex1, eIndex2);
								// set the new
								pLoopCity->updateCommerceModifierByBuilding(eIndex1, eIndex2, (iExistingValue - iOldValue + getBuildingCommerceModifier(eIndex1, eIndex2)));
							}
						}
					}
				}
			}
		}
	}
}

int CvTeam::getBuildingYieldModifier(BuildingTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppiBuildingYieldModifier[eIndex1][eIndex2];
}

void CvTeam::changeBuildingYieldModifier(BuildingTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		const int iOldValue = getBuildingYieldModifier(eIndex1, eIndex2);
		m_ppiBuildingYieldModifier[eIndex1][eIndex2] += iChange;

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if (pLoopCity->getNumActiveBuilding(eIndex1) > 0 && !pLoopCity->isReligiouslyDisabledBuilding(eIndex1))
					{
						const int iExistingValue = pLoopCity->getBuildingYieldModifier(eIndex1, eIndex2);
						// set the new
						pLoopCity->updateYieldModifierByBuilding(eIndex1, eIndex2, (iExistingValue - iOldValue + getBuildingYieldModifier(eIndex1, eIndex2)));
					}
				}
			}
		}
	}
}

void CvTeam::setLimitedBorders(TeamTypes eIndex, bool bNewValue)
{
	bool bOldFreeTrade;
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isLimitedBorders(eIndex) != bNewValue)
	{
		bOldFreeTrade = isFreeTrade(eIndex);

		m_abLimitedBorders[eIndex] = bNewValue;

		GC.getMap().verifyUnitValidPlot();

		if ((getID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (bOldFreeTrade != isFreeTrade(eIndex))
		{
			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
					{
						GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
					}
				}
			}
		}
	}
}

int CvTeam::getFreeSpecialistCount(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");
	return m_paiFreeSpecialistCount[eIndex];
}

void CvTeam::setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue)
{
	int iOldValue;

	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");

	iOldValue = getFreeSpecialistCount(eIndex);

	if (iOldValue != iNewValue)
	{
		m_paiFreeSpecialistCount[eIndex] = iNewValue;
		FAssert(getFreeSpecialistCount(eIndex) >= 0);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
					{
						pLoopCity->changeFreeSpecialistCount(eIndex, 0);
					}
				}
			}
		}
	}
}

void CvTeam::changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	setFreeSpecialistCount(eIndex, (getFreeSpecialistCount(eIndex) + iChange));
}

int CvTeam::getUnitStrengthChange(UnitTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return m_paiUnitStrengthChange[eIndex] * 100;
	}
	return m_paiUnitStrengthChange[eIndex];
}

void CvTeam::changeUnitStrengthChange(UnitTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	m_paiUnitStrengthChange[eIndex] += iChange;
}

bool CvTeam::isAnyVassal() const
{
	int iI;
	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (isVassal((TeamTypes)iI))
			{
				return true;
			}
		}
	}
	return false;
}

ImprovementTypes CvTeam::getImprovementUpgrade(ImprovementTypes eImprovement) const
{
	const ImprovementTypes eUpgrade = (ImprovementTypes)GC.getImprovementInfo(eImprovement).getImprovementUpgrade();

	if (eUpgrade != NO_IMPROVEMENT && GC.getImprovementInfo(eUpgrade).getPrereqTech() != NO_TECH
	&& !isHasTech((TechTypes)GC.getImprovementInfo(eUpgrade).getPrereqTech()))
	{
		return NO_IMPROVEMENT;
	}
	return eUpgrade;
}

ImprovementTypes CvTeam::finalImprovementUpgrade(ImprovementTypes eImprovement) const
{
	if (getImprovementUpgrade(eImprovement) != NO_IMPROVEMENT)
	{
		return finalImprovementUpgrade(getImprovementUpgrade(eImprovement));
	}
	return eImprovement;
}

bool CvTeam::isFreeTradeAgreement(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abFreeTrade[eIndex];
}


void CvTeam::setFreeTradeAgreement(TeamTypes eIndex, bool bNewValue)
{
	bool bOldFreeTrade;
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isFreeTradeAgreement(eIndex) != bNewValue)
	{
		bOldFreeTrade = isFreeTrade(eIndex);

		m_abFreeTrade[eIndex] = bNewValue;

		if ((getID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (bOldFreeTrade != isFreeTrade(eIndex))
		{
			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
					{
						GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
					}
				}
			}
		}
	}
}

void CvTeam::AI_updateBonusValue(BonusTypes eBonus)
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() && GET_PLAYER((PlayerTypes)iPlayer).isAlive())
		{
			GET_PLAYER((PlayerTypes)iPlayer).AI_updateBonusValue(eBonus);
		}
	}
}

void CvTeam::addPropertiesAllCities(const CvProperties *pProp)
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).addPropertiesAllCities(pProp);
			}
		}
	}
}

void CvTeam::subtractPropertiesAllCities(const CvProperties *pProp)
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).subtractPropertiesAllCities(pProp);
			}
		}
	}
}

void CvTeam::recalculateModifiers()
{
	// Clear player modifiers
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() && GET_PLAYER((PlayerTypes)iPlayer).isAlive())
		{
			GET_PLAYER((PlayerTypes)iPlayer).clearModifierTotals();
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiTechShareCount[iI] = 0;
	}

	//	Clear team level info (from techs)
	m_iExtraWaterSeeFromCount = 0;
	m_iCanPassPeaksCount = 0;
	m_iMoveFastPeaksCount = 0;
	m_iCanFoundOnPeaksCount = 0;
	m_iRebaseAnywhereCount = 0;
	m_iForeignTradeModifier = 0;
	m_iTradeModifier = 0;
	m_iTradeMissionModifier = 0;
	m_iCorporationRevenueModifier = 0;
	m_iCorporationMaintenanceModifier = 0;
	m_iCanFarmDesertCount = 0;
	m_iOpenBordersTradingCount = 0;
	m_iDefensivePactTradingCount = 0;
	m_iPermanentAllianceTradingCount = 0;
	m_iVassalTradingCount = 0;
	m_iBridgeBuildingCount = 0;
	m_iIrrigationCount = 0;
	m_iIgnoreIrrigationCount = 0;
	m_iWaterWorkCount = 0;
	m_iVassalPower = 0;
	m_iMasterPower = 0;
	m_iEnemyWarWearinessModifier = 0;
	m_iRiverTradeCount = 0;
	m_iNukeInterception = 0;

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_ppaaiImprovementYieldChange[iI][iJ] = 0;
		}
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_paiBuildingCount[iI] = 0;
		m_paiObsoleteBuildingCount[iI] = 0;
		m_paiTechExtraBuildingHappiness[iI] = 0;
		m_paiTechExtraBuildingHealth[iI] = 0;

		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			m_ppiBuildingCommerceChange[iI][iJ] = 0;
		}
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_ppiBuildingYieldChange[iI][iJ] = 0;
			m_ppiBuildingYieldModifier[iI][iJ] = 0;
		}
		for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			m_ppiBuildingSpecialistChange[iI][iJ] = 0;
		}
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			m_ppiBuildingCommerceModifier[iI][iJ] = 0;
		}
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_paiFreeSpecialistCount[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		m_paiUnitStrengthChange[iI] = 0;
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiExtraMoves[iI] = 0;
	}
	// Recalculate player modifiers
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() && GET_PLAYER((PlayerTypes)iPlayer).isAlive())
		{
			GET_PLAYER((PlayerTypes)iPlayer).recalculateModifiers();
		}
	}
	//	Reapply techs
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if (isHasTech((TechTypes)iI))
		{
			processTech((TechTypes)iI, 1);
		}
	}
	//	Reapply circumnavigation bonus
	if (GC.getGame().getCircumnavigatedTeam() == getID())
	{
		setCircumnavigated(true);
	}
	//	Reapply projects
	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		if (getProjectCount((ProjectTypes)iI) > 0)
		{
			processProjectChange((ProjectTypes)iI, getProjectCount((ProjectTypes)iI), getProjectCount((ProjectTypes)iI));
		}
	}
}

CvProperties* CvTeam::getProperties()
{
	return &m_Properties;
}

const CvProperties* CvTeam::getPropertiesConst() const
{
	return &m_Properties;
}

int CvTeam::getLastRoundOfValidImprovementCacheUpdate() const
{
	return m_iLastRoundOfValidImprovementCacheUpdate;
}

void CvTeam::setLastRoundOfValidImprovementCacheUpdate()
{
	m_iLastRoundOfValidImprovementCacheUpdate = GC.getGame().getGameTurn();
}

// K-Mod
int CvTeam::getTypicalUnitValue(UnitAITypes eUnitAI) const
{
	int iMax = 0;
	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iMax = std::max(iMax, GET_PLAYER((PlayerTypes)iI).getTypicalUnitValue(eUnitAI));
		}
	}
	return iMax;
}

int CvTeam::getWinForLosingResearchModifier() const
{
	const int iTopCityCount = GC.getGame().getTopCityCount();
	const int iTopPopCount = GC.getGame().getTopPopCount();
	const int iOurCityCount = getNumCities() * 100;
	const int iOurPopCount = getTotalPopulation(false) * 100;
	const int iCityPercent = iOurCityCount/std::max(1,iTopCityCount);
	const int iPopPercent = iOurPopCount/std::max(1,iTopPopCount);
	int iModifier = iCityPercent+iPopPercent;
	iModifier /= 2;
	const int iFinal = 100-iModifier;
	return iFinal;
}

