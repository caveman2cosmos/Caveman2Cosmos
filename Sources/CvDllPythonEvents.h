#pragma once

#ifndef CIV4_DLL_PYTHON_EVENTS_H
#define CIV4_DLL_PYTHON_EVENTS_H

#include "CvPython.h"

class CvCity;
class CvPlot;
class CvUnit;
class CyArgsList;
class CvSelectionGroup;

class CvDllPythonEvents
{
public:
	void reportGenericEvent(const char* szEventName, void *pyArgs);
	bool reportKbdEvent(int evt, int key, int iCursorX, int iCursorY);
	bool reportMouseEvent(int evt, int iCursorX, int iCursorY, bool bInterfaceConsumed=false);
	void reportModNetMessage(int iData1, int iData2, int iData3, int iData4, int iData5);

	void reportInit();
	void reportUnInit();
	void reportGameStart();
	void reportGameEnd(int iGameTurn);
	void reportWindowActivation(bool bActive);
	void reportMapRegen();

	void reportBeginGameTurn(int iGameTurn);
/************************************************************************************************/
/* RevolutionDCM MpFix                                                           lemmy101       */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void reportPreEndGameTurn(int iGameTurn);
/************************************************************************************************/
/* RevolutionDCM                           END                                                  */
/************************************************************************************************/
	void reportEndGameTurn(int iGameTurn);

	void reportBeginPlayerTurn(int iGameTurn, PlayerTypes);
	void reportEndPlayerTurn(int iGameTurn, PlayerTypes);

	void reportFirstContact(TeamTypes iTeamID1, TeamTypes iTeamID2);
	void reportCombatResult(CvUnit* pWinner, CvUnit* pLoser);
// BUG - Combat Events - start
	void reportCombatRetreat(CvUnit* pAttacker, CvUnit* pDefender);
	void reportCombatWithdrawal(CvUnit* pAttacker, CvUnit* pDefender);
	void reportCombatLogCollateral(CvUnit* pAttacker, CvUnit* pDefender, int iDamage);
	void reportCombatLogFlanking(CvUnit* pAttacker, CvUnit* pDefender, int iDamage);
// BUG - Combat Events - start
	void reportImprovementBuilt(int iImprovementType, int iX, int iY);
	void reportImprovementDestroyed(int iImprovementType, int iPlayer, int iX, int iY);
	void reportRouteBuilt(int iRouteType, int iX, int iY);

	void reportPlotRevealed(CvPlot *pPlot, TeamTypes eTeam);
	void reportPlotFeatureRemoved(CvPlot *pPlot, FeatureTypes eFeature, CvCity* pCity);
	void reportPlotPicked(CvPlot *pPlot);
	void reportNukeExplosion(CvPlot *pPlot, CvUnit* pNukeUnit);
	void reportGotoPlotSet(CvPlot *pPlot, PlayerTypes ePlayer);

	void reportCityBuilt(CvCity *pCity, CvUnit *pUnit = NULL);
	void reportCityRazed(CvCity *pCity, PlayerTypes ePlayer);
	void reportCityAcquired(PlayerTypes eOldOwner, PlayerTypes ePlayer, CvCity* city, bool bConquest, bool bTrade, bool bAutoRaze);
	void reportCityAcquiredAndKept(PlayerTypes eOldOwner, PlayerTypes ePlayer, CvCity* city, bool bConquest, bool bTrade);
	void reportCityLost(CvCity *pCity);
	void reportCultureExpansion(CvCity *pCity, PlayerTypes ePlayer);
	void reportCityGrowth(CvCity *pCity, PlayerTypes ePlayer);
	void reportCityProduction(CvCity *pCity, PlayerTypes ePlayer);
	void reportCityBuildingUnit(CvCity *pCity, UnitTypes eUnitType);
	void reportCityBuildingBuilding(CvCity *pCity, BuildingTypes eBuildingType);
// BUG - Project Started Event - start
	void reportCityBuildingProject(CvCity* pCity, ProjectTypes eProjectType);
// BUG - Project Started Event - end
// BUG - Process Started Event - start
	void reportCityBuildingProcess(CvCity* pCity, ProcessTypes eProcessType);
// BUG - Process Started Event - end
	void reportCityRename(CvCity *pCity);
	void reportCityHurry(CvCity *pCity, HurryTypes eHurry);

	void reportSelectionGroupPushMission(const CvSelectionGroup* pSelectionGroup, MissionTypes eMission);

	void reportUnitMove(CvPlot* pPlot, CvUnit* pUnit, CvPlot* pOldPlot);
	void reportUnitCreated(CvUnit *pUnit);
	void reportUnitBuilt(CvCity *pCity, CvUnit *pUnit);
	void reportUnitKilled(CvUnit *pUnit, PlayerTypes eAttacker);
// BUG - Upgrade Unit Event - start
	void reportUnitCaptured(PlayerTypes eFromPlayer, UnitTypes eUnitType, CvUnit* pNewUnit);
// BUG - Upgrade Unit Event - end
	void reportUnitLost(CvUnit *pUnit);
	void reportUnitPromoted(CvUnit* pUnit, PromotionTypes ePromotion);
// BUG - Upgrade Unit Event - start
	void reportUnitUpgraded(CvUnit* pOldUnit, CvUnit* pNewUnit, int iPrice);
// BUG - Upgrade Unit Event - end
	void reportUnitSelected(CvUnit *pUnit);
	void reportUnitRename(CvUnit *pUnit);
	void reportUnitPillage(CvUnit* pUnit, ImprovementTypes eImprovement, RouteTypes eRoute, PlayerTypes ePlayer);
	void reportUnitSpreadReligionAttempt(CvUnit* pUnit, ReligionTypes eReligion, bool bSuccess);
	void reportUnitGifted(CvUnit* pUnit, PlayerTypes eGiftingPlayer, CvPlot* pPlotLocation);
	void reportUnitBuildImprovement(CvUnit* pUnit, BuildTypes eBuild, bool bFinished);

	void reportGoodyReceived(PlayerTypes ePlayer, CvPlot *pGoodyPlot, CvUnit *pGoodyUnit, GoodyTypes eGoodyType);

	void reportGreatPersonBorn(CvUnit *pUnit, PlayerTypes ePlayer, CvCity *pCity);

	void reportCivicChanged(PlayerTypes ePlayer, CivicTypes eOldCivic, CivicTypes eNewCivic);
	void reportBuildingBuilt(CvCity *pCity, BuildingTypes eBuilding);
	void reportProjectBuilt(CvCity *pCity, ProjectTypes eProject);

	void reportTechAcquired(TechTypes eType, TeamTypes eTeam, PlayerTypes ePlayer, bool bAnnounce);
	void reportTechSelected(TechTypes eTech, PlayerTypes ePlayer);

	void reportReligionFounded(ReligionTypes eType, PlayerTypes ePlayer);
	void reportReligionSpread(ReligionTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity);
	void reportReligionRemove(ReligionTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity);

	void reportCorporationFounded(CorporationTypes eType, PlayerTypes ePlayer);
	void reportCorporationSpread(CorporationTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity);
	void reportCorporationRemove(CorporationTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity);

	void reportGoldenAge(PlayerTypes ePlayer);
	void reportEndGoldenAge(PlayerTypes ePlayer);
	void reportChangeWar(bool bWar, TeamTypes eTeam, TeamTypes eOtherTeam);
	void reportChat(CvWString szString);
	void reportVictory(TeamTypes eNewWinner, VictoryTypes eNewVictory);

	void reportVassalState(TeamTypes eMaster, TeamTypes eVassal, bool bVassal);

	void reportSetPlayerAlive(PlayerTypes ePlayerID, bool bNewValue);
	void reportPlayerChangeStateReligion(PlayerTypes ePlayerID, ReligionTypes eNewReligion, ReligionTypes eOldReligion);
	void reportPlayerGoldTrade(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, int iAmount);

	void reportChangeTeam(TeamTypes eOld, TeamTypes eNew);

	void preSave();
};

#endif
