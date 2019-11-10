#include "CvGameCoreDLL.h"

//#include <boost/lambda/lambda.hpp>
#include <boost/functional.hpp>
#include <boost/bind.hpp>

bool CvDllPythonEvents::preEvent()
{
	return gDLL->getPythonIFace()->isInitialized();
}

bool CvDllPythonEvents::postEvent(Cy::Args eventData, const char* eventName)
{
	eventData << GC.getGameINLINE().isDebugMode();
	eventData << false;
	eventData << gDLL->altKey();
	eventData << gDLL->ctrlKey();
	eventData << gDLL->shiftKey();
	eventData << (gDLL->getChtLvl() > 0);

#ifdef FP_PROFILE_ENABLE				// Turn Profiling On or Off .. 
#ifdef USE_INTERNAL_PROFILER
	static	std::map<int,ProfileSample*>*	g_pythonProfiles = NULL;

	if ( g_pythonProfiles == NULL )
	{
		g_pythonProfiles = new std::map<int,ProfileSample*>();
	}

	CvChecksum xSum;
	const char* ptr;
	ProfileSample* pSample;

	for(ptr = eventName; *ptr != '\0'; ptr++)
	{
		xSum.add((byte)*ptr);
	}

	std::map<int,ProfileSample*>::const_iterator itr = g_pythonProfiles->find(xSum.get());

	if ( itr == g_pythonProfiles->end() )
	{
		char profileName[256];

		sprintf(profileName, "CvDllPythonEvents::postEvent.%s", eventName);
		pSample = new ProfileSample(profileName);

		g_pythonProfiles->insert(std::make_pair(xSum.get(), pSample));
	}
	else
	{
		pSample = itr->second;
	}

	CProfileScope detailedScope(pSample);		
#endif
#endif

	return Cy::call<bool>(PYEventModule, "onEvent", eventData);
}

bool CvDllPythonEvents::reportKbdEvent(int evt, int key, int iCursorX, int iCursorY)
{
	if (preEvent())
	{
		NiPoint3 pt3Location;
		CvPlot* pPlot = gDLL->getEngineIFace()->pickPlot(iCursorX, iCursorY, pt3Location);
		Cy::Args eventData;
		eventData
			<< "kbdEvent"
			<< evt
			<< key
			<< iCursorX
			<< iCursorY
			<< (pPlot ? pPlot->getX() : -1)
			<< (pPlot ? pPlot->getY() : -1);

		return postEvent(eventData, "kbdEvent");
	}
	return false;
}

bool CvDllPythonEvents::reportMouseEvent(int evt, int iCursorX, int iCursorY, bool bInterfaceConsumed)
{
	if (preEvent())
	{
		NiPoint3 pt3Location;
		CvPlot* pPlot = gDLL->getEngineIFace()->pickPlot(iCursorX, iCursorY, pt3Location);
		// add list of active screens
		std::vector<int> screens;
		gDLL->getInterfaceIFace()->getInterfaceScreenIdsForInput(screens);
		Cy::Args eventData;
		eventData << "mouseEvent"
			<< evt
			<< iCursorX << iCursorY
			<< (pPlot ? pPlot->getX() : -1)
			<< (pPlot ? pPlot->getY() : -1)
			<< bInterfaceConsumed
			<< screens;
		return postEvent(eventData, "mouseEvent");
	}
	return false;
}

void CvDllPythonEvents::reportModNetMessage(int iData1, int iData2, int iData3, int iData4, int iData5)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "ModNetMessage"
			<< iData1
			<< iData2
			<< iData3
			<< iData4
			<< iData5;
		postEvent(eventData, "ModNetMessage");
	}
}

void CvDllPythonEvents::reportInit()
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "Init";
		postEvent(eventData, "Init");
	}
}

void CvDllPythonEvents::reportUpdate(float fDeltaTime)
{
	if (preEvent())
	{
		if(GC.getUSE_ON_UPDATE_CALLBACK())
		{
			Cy::Args eventData;
			eventData
				<< "Update"
				<< fDeltaTime;
			postEvent(eventData, "Update");
		}
	}
}

void CvDllPythonEvents::reportUnInit()
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "UnInit";
		postEvent(eventData, "UnInit");
	}
}

void CvDllPythonEvents::reportGameStart()
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "GameStart";
		postEvent(eventData, "GameStart");
	}
}

void CvDllPythonEvents::reportGameEnd()
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "GameEnd";
		postEvent(eventData, "GameEnd");
	}
}


void CvDllPythonEvents::reportWindowActivation(bool bActive)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "windowActivation"
			<< bActive;
		postEvent(eventData, "windowActivation");
	}
}

void CvDllPythonEvents::reportBeginGameTurn(int iGameTurn)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "BeginGameTurn"
			<< iGameTurn;
		postEvent(eventData, "BeginGameTurn");
	}
}

void CvDllPythonEvents::reportEndGameTurn(int iGameTurn)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "EndGameTurn"
			<< iGameTurn;
		postEvent(eventData, "EndGameTurn");
	}
}
/************************************************************************************************/
/* RevolutionDCM MpFix                                                           lemmy101       */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvDllPythonEvents::reportPreEndGameTurn(int iGameTurn)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "PreEndGameTurn"
			<< iGameTurn;
		postEvent(eventData, "PreEndGameTurn");
	}
}
/************************************************************************************************/
/* RevolutionDCM                           END                                                  */
/************************************************************************************************/

void CvDllPythonEvents::reportBeginPlayerTurn(int iGameTurn, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "BeginPlayerTurn"
			<< iGameTurn
			<< ePlayer;
		postEvent(eventData, "BeginPlayerTurn");
	}
}

void CvDllPythonEvents::reportEndPlayerTurn(int iGameTurn, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "EndPlayerTurn"
			<< iGameTurn
			<< ePlayer;
		postEvent(eventData, "EndPlayerTurn");
	}
}

void CvDllPythonEvents::reportFirstContact(TeamTypes eTeamID1, TeamTypes eTeamID2)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "firstContact"
			<< eTeamID1
			<< eTeamID2;
		postEvent(eventData, "firstContact");
	}
}

void CvDllPythonEvents::reportCombatResult(CvUnit* pWinner, CvUnit* pLoser)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "combatResult"
			<< pWinner
			<< pLoser;
		postEvent(eventData, "combatResult");
	}
}

// BUG - Combat Events - start
void CvDllPythonEvents::reportCombatRetreat(CvUnit* pAttacker, CvUnit* pDefender)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "combatRetreat"
			<< pAttacker
			<< pDefender;
		postEvent(eventData,"combatRetreat");
	}
}

void CvDllPythonEvents::reportCombatWithdrawal(CvUnit* pAttacker, CvUnit* pDefender)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "combatWithdrawal"
			<< pAttacker
			<< pDefender;
		postEvent(eventData, "combatWithdrawal");
	}
}

void CvDllPythonEvents::reportCombatLogCollateral(CvUnit* pAttacker, CvUnit* pDefender, int iDamage)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "combatLogCollateral"
			<< pAttacker
			<< pDefender
			<< iDamage;
		postEvent(eventData, "combatLogCollateral");
	}
}

void CvDllPythonEvents::reportCombatLogFlanking(CvUnit* pAttacker, CvUnit* pDefender, int iDamage)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "combatLogFlanking"
			<< pAttacker
			<< pDefender
			<< iDamage;
		postEvent(eventData, "combatLogFlanking");
	}
}
// BUG - Combat Events - end

void CvDllPythonEvents::reportImprovementBuilt(int iImprovementType, int iX, int iY)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "improvementBuilt"
			<< iImprovementType
			<< iX << iY;
		postEvent(eventData, "improvementBuilt");
	}
}

void CvDllPythonEvents::reportImprovementDestroyed(int iImprovementType, int iPlayer, int iX, int iY)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "improvementDestroyed"
			<< iImprovementType
			<< iPlayer
			<< iX << iY;
		postEvent(eventData, "improvementDestroyed");
	}
}

void CvDllPythonEvents::reportRouteBuilt(int iRouteType, int iX, int iY)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "routeBuilt"
			<< iRouteType
			<< iX << iY;
		postEvent(eventData, "routeBuilt");
	}
}

void CvDllPythonEvents::reportChat(CvWString szString)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "chat"
			<< szString;
		postEvent(eventData, "chat");
	}
}

void CvDllPythonEvents::reportPlotRevealed(CvPlot *pPlot, TeamTypes eTeam)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "plotRevealed"
			<< pPlot
			<< eTeam;
		postEvent(eventData, "plotRevealed");
	}
}

void CvDllPythonEvents::reportPlotFeatureRemoved(CvPlot *pPlot, FeatureTypes eFeature, CvCity* pCity)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "plotFeatureRemoved"
			<< pPlot
			<< eFeature
			<< pCity;
		postEvent(eventData, "plotFeatureRemoved");
	}
}

void CvDllPythonEvents::reportPlotPicked(CvPlot *pPlot)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "plotPicked"
			<< pPlot;
		postEvent(eventData, "plotPicked");
	}
}

void CvDllPythonEvents::reportNukeExplosion(CvPlot *pPlot, CvUnit* pNukeUnit)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "nukeExplosion"
			<< pPlot
			<< pNukeUnit;
		postEvent(eventData, "nukeExplosion");
	}
}

void CvDllPythonEvents::reportGotoPlotSet(CvPlot *pPlot, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "gotoPlotSet"
			<< pPlot
			<< ePlayer;
		postEvent(eventData, "gotoPlotSet");
	}
}

void CvDllPythonEvents::reportCityBuilt( CvCity *pCity, CvUnit *pUnit )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityBuilt"
			<< pCity
			<< pUnit;
		postEvent(eventData, "cityBuilt");
	}
}

void CvDllPythonEvents::reportCityRazed( CvCity *pCity, PlayerTypes ePlayer )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityRazed"
			<< pCity
			<< ePlayer;
		postEvent(eventData, "cityRazed");
	}
}

void CvDllPythonEvents::reportCityAcquired(PlayerTypes eOldOwner, PlayerTypes ePlayer, CvCity* pOldCity, bool bConquest, bool bTrade)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityAcquired"
			<< eOldOwner
			<< ePlayer
			<< pOldCity
			<< bConquest
			<< bTrade;
		postEvent(eventData, "cityAcquired");
	}
}

void CvDllPythonEvents::reportCityAcquiredAndKept(PlayerTypes ePlayer, CvCity* pOldCity)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityAcquiredAndKept"
			<< ePlayer
			<< pOldCity;

		postEvent(eventData, "cityAcquiredAndKept");
	}
}

void CvDllPythonEvents::reportCityLost(CvCity* pCity)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityLost"
			<< pCity;
		postEvent(eventData, "cityLost");
	}
}

void CvDllPythonEvents::reportCultureExpansion( CvCity *pCity, PlayerTypes ePlayer )
{
	if (preEvent())
	{
		postEvent(Cy::Args() << "cultureExpansion" << pCity << ePlayer, "cultureExpansion");
	}
}

void CvDllPythonEvents::reportCityGrowth( CvCity *pCity, PlayerTypes ePlayer )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityGrowth"
			<< pCity
			<< ePlayer;
		postEvent(eventData, "cityGrowth");
	}
}

void CvDllPythonEvents::reportCityProduction( CvCity *pCity, PlayerTypes ePlayer )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityDoTurn"
			<< pCity
			<< ePlayer;
		postEvent(eventData, "cityDoTurn");
	}
}

void CvDllPythonEvents::reportCityBuildingUnit( CvCity *pCity, UnitTypes eUnitType )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityBuildingUnit"
			<< pCity
			<< eUnitType;
		postEvent(eventData, "cityBuildingUnit");
	}
}

void CvDllPythonEvents::reportCityBuildingBuilding( CvCity *pCity, BuildingTypes eBuildingType )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityBuildingBuilding"
			<< pCity
			<< eBuildingType;
		postEvent(eventData, "cityBuildingBuilding");
	}
}

// BUG - Project Started Event - start
void CvDllPythonEvents::reportCityBuildingProject( CvCity* pCity, ProjectTypes eProjectType )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityBuildingProject"
			<< pCity
			<< eProjectType;
		postEvent(eventData, "cityBuildingProject");
	}
}
// BUG - Project Started Event - end

// BUG - Process Started Event - start
void CvDllPythonEvents::reportCityBuildingProcess( CvCity* pCity, ProcessTypes eProcessType )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityBuildingProcess"
			<< pCity
			<< eProcessType;
		postEvent(eventData, "cityBuildingProcess");
	}
}
// BUG - Process Started Event - end

void CvDllPythonEvents::reportCityRename( CvCity *pCity )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityRename"
			<< pCity;
		postEvent(eventData, "cityRename");
	}
}

void CvDllPythonEvents::reportCityHurry( CvCity *pCity, HurryTypes eHurry )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "cityHurry"
			<< pCity
			<< eHurry;
		postEvent(eventData,"cityHurry");
	}
}

void CvDllPythonEvents::reportSelectionGroupPushMission(CvSelectionGroup* pSelectionGroup, MissionTypes eMission)
{
	if (NULL == pSelectionGroup)
	{
		return;
	}

	if (preEvent())
	{
		std::vector<int> aiUnitIds;

		//using namespace boost::lambda;

		std::transform(pSelectionGroup->beginUnits(), pSelectionGroup->endUnits(), std::back_inserter(aiUnitIds), boost::bind(&CvUnit::getID, _1));

		Cy::Args eventData;
		eventData
			<< "selectionGroupPushMission"
			<< pSelectionGroup->getOwner()
			<< eMission
			<< aiUnitIds.size()
			<< aiUnitIds;
		postEvent(eventData, "selectionGroupPushMission");
	}
}

void CvDllPythonEvents::reportUnitMove(CvPlot* pPlot, CvUnit* pUnit, CvPlot* pOldPlot)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "unitMove"
			<< pPlot
			<< pUnit
			<< pOldPlot;
		postEvent(eventData, "unitMove");
	}
}

void CvDllPythonEvents::reportUnitSetXY(CvPlot* pPlot, CvUnit* pUnit)
{
	if (preEvent())
	{
		if(GC.getUSE_ON_UNIT_SET_XY_CALLBACK())
		{
			Cy::Args eventData;
			eventData
				<< "unitSetXY"
				<< pPlot
				<< pUnit;
			postEvent(eventData, "unitSetXY");
		}
	}
}

void CvDllPythonEvents::reportUnitCreated(CvUnit* pUnit)
{
	if (preEvent())
	{
		if(GC.getUSE_ON_UNIT_CREATED_CALLBACK())
		{
			Cy::Args eventData;
			eventData
				<< "unitCreated"
				<< pUnit;
			postEvent(eventData, "unitCreated");
		}
	}
}

void CvDllPythonEvents::reportUnitBuilt(CvCity *pCity, CvUnit* pUnit)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "unitBuilt"
			<< pCity
			<< pUnit;
		postEvent(eventData,"unitBuilt");
	}
}

void CvDllPythonEvents::reportUnitKilled(CvUnit* pUnit, PlayerTypes eAttacker)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "unitKilled"
			<< pUnit
			<< eAttacker;
		postEvent(eventData, "unitKilled");
	}
}

// BUG - Unit Captured Event - start
void CvDllPythonEvents::reportUnitCaptured(PlayerTypes eFromPlayer, UnitTypes eUnitType, CvUnit* pNewUnit)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "unitCaptured"
			<< eFromPlayer
			<< eUnitType
			<< pNewUnit;
		postEvent(eventData, "unitCaptured");
	}
}
// BUG - Unit Captured Event - end

void CvDllPythonEvents::reportUnitLost(CvUnit* pUnit)
{
	if (preEvent())
	{
		if(GC.getUSE_ON_UNIT_LOST_CALLBACK())
		{
			Cy::Args eventData;
			eventData
				<< "unitLost"
				<< pUnit;
			postEvent(eventData, "unitLost");
		}
	}
}

void CvDllPythonEvents::reportUnitPromoted(CvUnit* pUnit, PromotionTypes ePromotion)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "unitPromoted"
			<< pUnit
			<< ePromotion;
		postEvent(eventData, "unitPromoted");
	}
}

// BUG - Upgrade Unit Event - start
void CvDllPythonEvents::reportUnitUpgraded(CvUnit* pOldUnit, CvUnit* pNewUnit, int iPrice)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "unitUpgraded"
			<< pOldUnit
			<< pNewUnit
			<< iPrice;
		postEvent(eventData, "unitUpgraded");
	}
}
// BUG - Upgrade Unit Event - end

void CvDllPythonEvents::reportUnitSelected(CvUnit* pUnit)
{
	if (preEvent())
	{
		if(GC.getUSE_ON_UNIT_SELECTED_CALLBACK())
		{
			Cy::Args eventData;
			eventData
				<< "unitSelected"
				<< pUnit;
			postEvent(eventData, "unitSelected");
		}
	}
}

void CvDllPythonEvents::reportUnitRename(CvUnit *pUnit)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "UnitRename"
			<< pUnit;
		postEvent(eventData, "UnitRename");
	}
}

void CvDllPythonEvents::reportUnitPillage(CvUnit* pUnit, ImprovementTypes eImprovement, RouteTypes eRoute, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "unitPillage"
			<< pUnit
			<< eImprovement
			<< eRoute
			<< ePlayer;
		postEvent(eventData, "unitPillage");
	}
}

void CvDllPythonEvents::reportUnitSpreadReligionAttempt(CvUnit* pUnit, ReligionTypes eReligion, bool bSuccess)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "unitSpreadReligionAttempt"
			<< pUnit
			<< eReligion
			<< bSuccess;
		postEvent(eventData, "unitSpreadReligionAttempt");
	}
}

void CvDllPythonEvents::reportUnitGifted(CvUnit* pUnit, PlayerTypes eGiftingPlayer, CvPlot* pPlotLocation)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "unitGifted"
			<< pUnit
			<< eGiftingPlayer
			<< pPlotLocation;
		postEvent(eventData, "unitGifted");
	}
}

void CvDllPythonEvents::reportUnitBuildImprovement(CvUnit* pUnit, BuildTypes eBuild, bool bFinished)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "unitBuildImprovement"
			<< pUnit
			<< eBuild
			<< bFinished;
		postEvent(eventData, "unitBuildImprovement");
	}
}

void CvDllPythonEvents::reportGoodyReceived(PlayerTypes ePlayer, CvPlot *pGoodyPlot, CvUnit *pGoodyUnit, GoodyTypes eGoodyType)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "goodyReceived"
			<< ePlayer
			<< pGoodyPlot
			<< pGoodyUnit
			<< eGoodyType;
		postEvent(eventData, "goodyReceived");
	}
}

void CvDllPythonEvents::reportGreatPersonBorn( CvUnit *pUnit, PlayerTypes ePlayer, CvCity *pCity )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "greatPersonBorn"
			<< pUnit
			<< ePlayer
			<< pCity;
		postEvent(eventData, "greatPersonBorn");
	}
}
void CvDllPythonEvents::reportCivicChanged(PlayerTypes ePlayer, CivicTypes eOldCivic, CivicTypes eNewCivic)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "civicChanged"
			<< ePlayer
			<< eOldCivic
			<< eNewCivic;
		postEvent(eventData, "civicChanged");
	}
}

void CvDllPythonEvents::reportBuildingBuilt(CvCity *pCity, BuildingTypes eBuilding)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "buildingBuilt"
			<< pCity
			<< eBuilding;
		postEvent(eventData, "buildingBuilt");
	}
}

void CvDllPythonEvents::reportProjectBuilt(CvCity *pCity, ProjectTypes eProject)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "projectBuilt"
			<< pCity
			<< eProject;
		postEvent(eventData, "projectBuilt");
	}
}

void CvDllPythonEvents::reportTechAcquired(TechTypes eType, TeamTypes eTeam, PlayerTypes ePlayer, bool bAnnounce)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "techAcquired"
			<< eType
			<< eTeam
			<< ePlayer
			<< bAnnounce;
		postEvent(eventData, "techAcquired");
	}
}

void CvDllPythonEvents::reportTechSelected(TechTypes eTech, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "techSelected"
			<< eTech
			<< ePlayer;
		postEvent(eventData, "techSelected");
	}
}

void CvDllPythonEvents::reportReligionFounded(ReligionTypes eType, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "religionFounded"
			<< eType
			<< ePlayer;
		postEvent(eventData, "religionFounded");
	}
}

void CvDllPythonEvents::reportReligionSpread(ReligionTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "religionSpread"
			<< eType
			<< ePlayer
			<< pSpreadCity;

		postEvent(eventData, "religionSpread");
	}
}

void CvDllPythonEvents::reportReligionRemove(ReligionTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "religionRemove"
			<< eType
			<< ePlayer
			<< pSpreadCity;

		postEvent(eventData, "religionRemove");
	}
}

void CvDllPythonEvents::reportCorporationFounded(CorporationTypes eType, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "corporationFounded"
			<< eType
			<< ePlayer;
		postEvent(eventData, "corporationFounded");
	}
}

void CvDllPythonEvents::reportCorporationSpread(CorporationTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "corporationSpread"
			<< eType
			<< ePlayer
			<< pSpreadCity;
		postEvent(eventData, "corporationSpread");
	}
}

void CvDllPythonEvents::reportCorporationRemove(CorporationTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "corporationRemove"
			<< eType
			<< ePlayer
			<< pSpreadCity;
		postEvent(eventData, "corporationRemove");
	}
}

void CvDllPythonEvents::reportGoldenAge(PlayerTypes ePlayer)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "goldenAge"
			<< ePlayer;
		postEvent(eventData, "goldenAge");
	}
}

void CvDllPythonEvents::reportEndGoldenAge(PlayerTypes ePlayer)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "endGoldenAge"
			<< ePlayer;
		postEvent(eventData, "endGoldenAge");
	}
}

void CvDllPythonEvents::reportChangeWar(bool bWar, TeamTypes eTeam, TeamTypes eOtherTeam)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "changeWar"
			<< bWar
			<< eTeam
			<< eOtherTeam;
		postEvent(eventData, "changeWar");
	}
}

void CvDllPythonEvents::reportVictory(TeamTypes eNewWinner, VictoryTypes eNewVictory)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "victory"
			<< eNewWinner
			<< eNewVictory;
		postEvent(eventData, "victory");
	}
}

void CvDllPythonEvents::reportVassalState(TeamTypes eMaster, TeamTypes eVassal, bool bVassal)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "vassalState"
			<< eMaster
			<< eVassal
			<< bVassal;
		postEvent(eventData, "vassalState");
	}
}

void CvDllPythonEvents::reportSetPlayerAlive( PlayerTypes ePlayerID, bool bNewValue )
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "setPlayerAlive"
			<< ePlayerID
			<< bNewValue;
		postEvent(eventData, "setPlayerAlive");
	}
}

void CvDllPythonEvents::reportPlayerChangeStateReligion(PlayerTypes ePlayerID, ReligionTypes eNewReligion, ReligionTypes eOldReligion)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "playerChangeStateReligion"
			<< ePlayerID
			<< eNewReligion
			<< eOldReligion;

		postEvent(eventData, "playerChangeStateReligion");
	}
}


void CvDllPythonEvents::reportPlayerGoldTrade(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, int iAmount)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "playerGoldTrade"
			<< eFromPlayer
			<< eToPlayer
			<< iAmount;

		postEvent(eventData, "playerGoldTrade");
	}
}

// BUG - Revolution Event - start
void CvDllPythonEvents::reportPlayerRevolution(PlayerTypes ePlayerID, int iAnarchyLength, CivicTypes* paeOldCivics, CivicTypes* paeNewCivics)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "playerRevolution"
			<< ePlayerID
			<< iAnarchyLength
			<< Cy::Array<int>((const int*)paeOldCivics, GC.getNumCivicOptionInfos())
			<< Cy::Array<int>((const int*)paeNewCivics, GC.getNumCivicOptionInfos());
		postEvent(eventData, "playerRevolution");
	}
}
// BUG - Revolution Event - end

void CvDllPythonEvents::reportGenericEvent(const char* szEventName, void *pyArgs)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< szEventName
			// generic args tuple
			<< pyArgs;
		postEvent(eventData, szEventName);
	}
}

void CvDllPythonEvents::preSave()
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "OnPreSave";
		postEvent(eventData, "OnPreSave");
	}
}
/************************************************************************************************/
/* Afforess	                  Start		 07/19/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvDllPythonEvents::reportAddTeam(TeamTypes eIndex0, TeamTypes eIndex1, bool bAdded)
{
	if (preEvent())
	{
		Cy::Args eventData;
		eventData
			<< "addTeam"
			<< eIndex0
			<< eIndex1
			<< bAdded;
		postEvent(eventData, "addTeam");
	}
}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

