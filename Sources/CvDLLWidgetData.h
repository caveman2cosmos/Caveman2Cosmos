#pragma once

#ifndef _CVDLLWIDGETDATA_H_
#define _CVDLLWIDGETDATA_H_

class CvDLLWidgetData
{

public:

	DllExport static CvDLLWidgetData& getInstance();
	DllExport static void freeInstance();

	//	This will parse the help for the widget
	DllExport void parseHelp(CvWStringBuffer &szBuffer, CvWidgetDataStruct &widgetDataStruct);

	//	This will execute the action for the widget
	DllExport bool executeAction( CvWidgetDataStruct &widgetDataStruct );

	//	This will execute an alternate action for the widget
	DllExport bool executeAltAction( CvWidgetDataStruct &widgetDataStruct );

	DllExport bool isLink(const CvWidgetDataStruct &widgetDataStruct) const;

	//	Actions to be executed
	void doLiberateCity();
	void doRenameCity();
	void doRenameUnit();
	void doCreateGroup();
	void doDeleteGroup();
	void doTrain(CvWidgetDataStruct &widgetDataStruct);
	void doConstruct(CvWidgetDataStruct &widgetDataStruct);
	void doCreate(CvWidgetDataStruct &widgetDataStruct);
	void doMaintain(CvWidgetDataStruct &widgetDataStruct);
	void doBuildListList(CvWidgetDataStruct &widgetDataStruct);
	void doBuildListOrder(CvWidgetDataStruct &widgetDataStruct, OrderTypes eOrder);
	void doBuildListQueue(CvWidgetDataStruct &widgetDataStruct);
	void doBuildList(CvWidgetDataStruct &widgetDataStruct);
	void doHurry(CvWidgetDataStruct &widgetDataStruct);
	void doConscript();
	void doAction(CvWidgetDataStruct &widgetDataStruct);
	void doChangeSpecialist(CvWidgetDataStruct &widgetDataStruct);
	void doResearch(CvWidgetDataStruct &widgetDataStruct);
	void doChangePercent(CvWidgetDataStruct &widgetDataStruct);

	void doSetPercent(CvWidgetDataStruct &widgetDataStruct);

	void doCityTab(CvWidgetDataStruct &widgetDataStruct);
	void doContactCiv(CvWidgetDataStruct &widgetDataStruct);
	void doConvert(CvWidgetDataStruct &widgetDataStruct);
	void doAutomateCitizens();
	void doEmphasize(CvWidgetDataStruct &widgetDataStruct);
	void doEmphasizeSpecialist(CvWidgetDataStruct &widgetDataStruct);
	void doBuildingFilter(CvWidgetDataStruct &widgetDataStruct);
	void doBuildingGrouping(CvWidgetDataStruct &widgetDataStruct);
	void doBuildingSort(CvWidgetDataStruct &widgetDataStruct);
	void doUnitFilter(CvWidgetDataStruct &widgetDataStruct);
	void doUnitGrouping(CvWidgetDataStruct &widgetDataStruct);
	void doUnitSort(CvWidgetDataStruct &widgetDataStruct);
	void doCancelCivics();
	void applyCityEdit();
	void doUnitModel();
	void doSelected(CvWidgetDataStruct &widgetDataStruct);
	void doPediaTechJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaUnitJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaBuildingJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaBack();
	void doPediaForward();
	void doPediaBonusJump(CvWidgetDataStruct &widgetDataStruct, bool bData2 = false);
	void doPediaSpecialistJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaProjectJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaReligionJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaCorporationJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaTerrainJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaFeatureJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaTraitJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaMain(CvWidgetDataStruct &widgetDataStruct);
	void doPediaPromotionJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaImprovementJump(CvWidgetDataStruct &widgetDataStruct, bool bData2 = false);
	void doPediaRouteJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaCivicJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaCivilizationJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaLeaderJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaDescription(CvWidgetDataStruct &widgetDataStruct);
	void doGotoTurnEvent(CvWidgetDataStruct &widgetDataStruct);
	void doForeignAdvisor(CvWidgetDataStruct &widgetDataStruct);
	void doPediaConstructJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaTrainJump(CvWidgetDataStruct &widgetDataStruct);
	void doPediaBuildJump(CvWidgetDataStruct &widgetDataStruct);
	void doDealKill(CvWidgetDataStruct &widgetDataStruct);
	void doRefreshMilitaryAdvisor(CvWidgetDataStruct &widgetDataStruct);
	void doMenu( void );
	void doLaunch(CvWidgetDataStruct &widgetDataStruct);

	//	Help parsing
	void parsePlotListHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLiberateCityHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCityNameHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTrainHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseConstructHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCreateHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseMaintainHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBuildListQueueHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBuildListHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseHurryHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseConscriptHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseActionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCitizenHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFreeCitizenHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseDisabledCitizenHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseAngryCitizenHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseChangeSpecialistHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseResearchHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTechTreeHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseChangePercentHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseSetPercentHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseContactCivHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseScoreHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseAutomateCitizensHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseEmphasizeHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTradeItem(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseUnitModelHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseMaintenanceHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseNationalityHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseHealthHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePopulationHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseProductionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCultureHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseGreatPeopleHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseGreatGeneralHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseSelectedHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTradeRouteCityHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseEspionageCostHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBuildingHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseProjectHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTerrainHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFeatureHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTechEntryHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTechTreePrereq(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer, bool bTechTreeInfo);
	void parseObsoleteHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseObsoleteBonusString(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseObsoleteSpecialHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseMoveHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFreeUnitHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFeatureProductionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseWorkerRateHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTradeRouteHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseHealthRateHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseHappinessRateHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFreeTechHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLOSHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseMapCenterHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseMapRevealHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseMapTradeHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTechTradeHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseGoldTradeHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseOpenBordersHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseDefensivePactHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePermanentAllianceHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseVassalStateHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBuildBridgeHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseIrrigationHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseIgnoreIrrigationHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseWaterWorkHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBuildHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseDomainExtraMovesHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseAdjustHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTerrainTradeHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseSpecialBuildingHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseYieldChangeHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBonusRevealHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCivicRevealHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseProcessInfoHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFoundReligionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFoundCorporationHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseFinanceDomesticTrade(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFinanceForeignTrade(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFinanceSpecialistGold(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseFinanceNumUnits(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFinanceUnitCost(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFinanceAwaySupply(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFinanceCityMaint(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFinanceCivicUpkeep(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFinanceForeignIncome(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFinanceInflatedCosts(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFinanceGrossIncome(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseFinanceNetGold(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseUnitHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBonusHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseReligionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseReligionHelpCity(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCorporationHelpCity(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCorporationHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parsePromotionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBuildUpHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseTraitHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseEventHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseImprovementHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseRouteHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer, bool bCivilopediaText = false);
	void parseCivicHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCivilizationHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseLeaderHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseDescriptionHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer, bool bMinimal);
	void parseKillDealHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseFoodModHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseLeaderheadHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseCommerceModHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseTradeRoutes(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseObsoleteCorporation(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

	void parseBuildingFilterHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBuildingGroupingHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseBuildingSortHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseUnitFilterHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseUnitGroupingHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);
	void parseUnitSortHelp(CvWidgetDataStruct &widgetDataStruct, CvWStringBuffer &szBuffer);

protected:

	static CvDLLWidgetData* m_pInst;

};

#endif//_CVDLLWIDGETDATA_H_
