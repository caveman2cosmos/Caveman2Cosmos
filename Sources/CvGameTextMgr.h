#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvGameTextMgr.h
//
//  AUTHOR:  Jesse Smith  --  10/2004
//
//  PURPOSE: Group of functions to manage CIV Game Text
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef CIV4_GAME_TEXT_MGR_H
#define CIV4_GAME_TEXT_MGR_H

#pragma warning( disable: 4251 )	// needs to have dll-interface to be used by clients of class

#define ENABLE_ROLES_HOVER

class CounterSet;
class CvCity;
class CvDeal;
class CvPopupInfo;
class CvPlayer;

//
// Class:		CvGameTextMgr
// Purpose:		Manages Game Text...
class CvGameTextMgr
{
	friend class CvGlobals;
	CounterSet* inspectUnitCombatCounters;
public:
	// singleton accessor
	DllExport static CvGameTextMgr& GetInstance();

	CvGameTextMgr();
	virtual ~CvGameTextMgr();

	DllExport void Initialize();
	DllExport void DeInitialize();
	DllExport void Reset();

	int getCurrentLanguage() const;

	DllExport void setTimeStr(CvWString& szString, int iGameTurn, bool bSave);
	void setYearStrAC(CvWString& szString, int iyear, bool bSave);
	void setYearStr(CvWString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed);
	void setDateStr(CvWString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed);
	void setInterfaceTime(CvWString& szString, PlayerTypes ePlayer);
	void setOOSSeeds(CvWString& szString, PlayerTypes ePlayer);
	void setNetStats(CvWString& szString, PlayerTypes ePlayer);
	DllExport void setMinimizePopupHelp(CvWString& szString, const CvPopupInfo & info);

	void setUnitHelp(CvWStringBuffer &szString, const CvUnit* pUnit, bool bOneLine = false, bool bShort = false, bool bdarkColor = false);
	void setPlotListHelp(CvWStringBuffer &szString, CvPlot* pPlot, bool bOneLine, bool bShort);
	bool setCombatPlotHelp(CvWStringBuffer &szString, CvPlot* pPlot, bool bAssassinate = false);
	bool setMinimalCombatPlotHelp(CvWStringBuffer& szString, CvPlot* pPlot, bool bAssassinate = false);
	bool setAssassinatePlotHelp(CvWStringBuffer& szString, CvPlot* pPlot, CvUnit* pAttacker, CvUnit* pDefender);
	void setPlotHelp(CvWStringBuffer &szString, CvPlot* pPlot);
	void setCityBarHelp(CvWStringBuffer &szString, CvCity* pCity);
	void setScoreHelp(CvWStringBuffer &szString, PlayerTypes ePlayer);

	void parseBuildUp(CvWStringBuffer &szHelpString, PromotionLineTypes eTrait, CivilizationTypes eCivilization = NO_CIVILIZATION);
	void parseTraits(CvWStringBuffer &szHelpString, TraitTypes eTrait, bool bDawnOfMan = false, bool bEffectsOnly = false);

	DllExport void parseLeaderTraits(CvWStringBuffer &szInfoText, LeaderHeadTypes eLeader = NO_LEADER, CivilizationTypes eCivilization = NO_CIVILIZATION, bool bDawnOfMan = false, bool bCivilopediaText = false);
	DllExport void parseLeaderShortTraits(CvWStringBuffer &szInfoText, LeaderHeadTypes eLeader);
	DllExport void parseCivInfos(CvWStringBuffer &szHelpString, CivilizationTypes eCivilization, bool bDawnOfMan = false, bool bLinks = true);

	void parseSpecialistHelp(CvWStringBuffer &szHelpString, SpecialistTypes eSpecialist, CvCity* pCity, bool bCivilopediaText = false);
	void parseSpecialistHelpActual(CvWStringBuffer &szHelpString, SpecialistTypes eSpecialist, CvCity* pCity, bool bCivilopediaText = false, int iChange = 0);
	void parseFreeSpecialistHelp(CvWStringBuffer &szHelpString, const CvCity& kCity);
	void parsePromotionHelp(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, const wchar_t* pcNewline = NEWLINE);
	void parsePromotionHelpInternal(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, const wchar_t* pcNewline, bool bAccrueLines);
	void parseCivicInfo(CvWStringBuffer &szBuffer, CivicTypes eCivic, bool bCivilopediaText = false, bool bPlayerContext = false, bool bSkipName = false);
	void parsePlayerTraits(CvWStringBuffer &szBuffer, PlayerTypes ePlayer);
	void parsePlayerHasFixedBorders(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer);
	void parseLeaderHeadHelp(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer);
	void parseGreatPeopleHelp(CvWStringBuffer &szBuffer, CvCity& city);
	bool setBuildingAdditionalGreatPeopleHelp(CvWStringBuffer &szBuffer, CvCity& city, const CvWString& szStart, bool bStarted = false);
	void parseGreatGeneralHelp(CvWStringBuffer &szBuffer, CvPlayer& kPlayer);
	void setTechHelp(CvWStringBuffer &szBuffer, TechTypes eTech, bool bCivilopediaText = false, bool bPlayerContext = false, bool bStrategyText = false, bool bTreeInfo = true, TechTypes eFromTech = NO_TECH);
	void setBasicUnitHelp(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText = false);
	void setBasicUnitHelpWithCity(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText = false, CvCity* pCity = NULL, bool bConscript = false, bool bTBUnitView1 = false, bool bTBUnitView2 = false, bool bTBUnitView3 = false);
	void setUnitExperienceHelp(CvWStringBuffer &szBuffer, CvWString szStart, UnitTypes eUnit, CvCity* pCity, bool bConscript = false);
	void setUnitHelp(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText = false, bool bStrategyText = false, bool bTechChooserText = false, CvCity* pCity = NULL);
	void setBuildingHelp(
		CvWStringBuffer &szBuffer, const BuildingTypes eBuilding, const bool bActual, CvCity* pCity = NULL,
		const bool bCivilopediaText = false, const bool bStrategyText = false, const bool bTechChooserText = false
	);
	void setHeritageHelp(CvWStringBuffer &szBuffer, const HeritageTypes eType, CvCity* pCity = NULL, const bool bCivilopediaText = false, const bool bStrategyText = false, const bool bTechChooserText = false);
	void setBuildingActualEffects(CvWStringBuffer &szBuffer, const CvWString& szStart, BuildingTypes eBuilding, const CvCity* pCity, bool bNewLine = true);

	void setProjectHelp(CvWStringBuffer &szBuffer, ProjectTypes eProject, bool bCivilopediaText = false, CvCity* pCity = NULL);
	void setProcessHelp(CvWStringBuffer &szBuffer, ProcessTypes eProcess);
	void setGoodHealthHelp(CvWStringBuffer &szBuffer, CvCity& city);
	void setBadHealthHelp(CvWStringBuffer &szBuffer, CvCity& city);
	bool setBuildingAdditionalHealthHelp(CvWStringBuffer &szBuffer, CvCity& city, const CvWString& szStart, bool bStarted = false);
	void parseHappinessHelp(CvWStringBuffer& szBuffer);
	void setAngerHelp(CvWStringBuffer &szBuffer, CvCity& city);
	void setHappyHelp(CvWStringBuffer &szBuffer, CvCity& city);
	bool setBuildingAdditionalHappinessHelp(CvWStringBuffer &szBuffer, CvCity& city, const CvWString& szStart, bool bStarted = false);
	void setYieldChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piYieldChange, bool bPercent = false, bool bNewLine = true);
	void listCommerceChange(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szEnd, const int* aList, bool bPercent = false);
	void setYieldPerPopChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piYieldChange, bool bPercent = false, bool bNewLine = true);
	void setCommerceTimes100ChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piCommerceChange, bool bNewLine = false, bool bStarted = false);
	bool setResumableYieldChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piYieldChange, bool bPercent = false, bool bNewLine = true, bool bStarted = false, bool bPerPop = false);
	bool setResumableCommerceTimes100ChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piCommerceChange, bool bNewLine, bool bStarted = false);
	bool setResumableGoodBadChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, int iGood, int iGoodSymbol, int iBad, int iBadSymbol, bool bPercent = false, bool bNewLine = false, bool bStarted = false);
	bool setResumableValueChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, int iValue, int iSymbol, bool bPercent = false, bool bNewLine = false, bool bStarted = false);
	bool setResumableValueTimes100ChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, int iValue, int iSymbol, bool bNewLine = false, bool bStarted = false);
	void setBonusHelp(CvWStringBuffer &szBuffer, BonusTypes eBonus, bool bCivilopediaText = false);
	void setBonusTradeHelp(CvWStringBuffer &szBuffer, BonusTypes eBonus, bool bCivilopediaText, PlayerTypes eTradePlayer);
	void setReligionHelp(CvWStringBuffer &szBuffer, ReligionTypes eReligion, bool bCivilopedia = false);
	void setReligionHelpCity(CvWStringBuffer &szBuffer, ReligionTypes eReligion, CvCity *pCity, bool bCityScreen = false, bool bForceReligion = false, bool bForceState = false, bool bNoStateReligion = false);
	void setCorporationHelp(CvWStringBuffer &szBuffer, CorporationTypes eCorporation, bool bCivilopedia = false);
	void setCorporationHelpCity(CvWStringBuffer &szBuffer, CorporationTypes eCorporation, CvCity *pCity, bool bCityScreen = false, bool bForceCorporation = false);
	void setPromotionHelp(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, bool bCivilopediaText = false);
	void setBuildUpHelp(CvWStringBuffer &szBuffer, PromotionLineTypes ePromotionLine);
	void setTraitHelp(CvWStringBuffer &szBuffer, TraitTypes eTrait);
	void setUnitCombatHelp(CvWStringBuffer& szBuffer, UnitCombatTypes eUnitCombat, bool bCivilopediaText = false) const;
	void setImprovementHelp(CvWStringBuffer &szBuffer, ImprovementTypes eImprovement, FeatureTypes eFeature = NO_FEATURE, bool bCivilopediaText = false);
	void setRouteHelp(CvWStringBuffer &szBuffer, RouteTypes eRoute, bool bCivilopediaText = false);
	void setTerrainHelp(CvWStringBuffer &szBuffer, TerrainTypes eTerrain, bool bCivilopediaText = false);
	void setFeatureHelp(CvWStringBuffer &szBuffer, FeatureTypes eFeature, bool bCivilopediaText = false);
	void setFoodHelp(CvWStringBuffer &szBuffer, CvCity& city);
	bool setBuildingAdditionalYieldHelp(CvWStringBuffer &szBuffer, CvCity& city, YieldTypes eIndex, const CvWString& szStart, bool bStarted = false);
	void setProductionHelp(CvWStringBuffer &szBuffer, CvCity& city);
	void setCommerceHelp(CvWStringBuffer &szBuffer, CvCity& city, CommerceTypes eCommerceType);
	bool setBuildingAdditionalCommerceHelp(CvWStringBuffer &szBuffer, CvCity& city, CommerceTypes eIndex, const CvWString& szStart, bool bStarted = false);
	bool setBuildingSavedMaintenanceHelp(CvWStringBuffer &szBuffer, const CvCity& city, const CvWString& szStart, bool bStarted = false);
	void setYieldHelp(CvWStringBuffer &szBuffer, CvCity& city, YieldTypes eYieldType);
	void setConvertHelp(CvWStringBuffer& szBuffer, PlayerTypes ePlayer, ReligionTypes eReligion);
	void setRevolutionHelp(CvWStringBuffer& szBuffer, PlayerTypes ePlayer);
	void setVassalRevoltHelp(CvWStringBuffer& szBuffer, TeamTypes eMaster, TeamTypes eVassal);
	void setEventHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, int iEventTriggeredId, PlayerTypes ePlayer);
	void setTradeRouteHelp(CvWStringBuffer &szBuffer, int iRoute, CvCity* pCity);
	void setEspionageCostHelp(CvWStringBuffer &szBuffer, EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit);
	void setEspionageMissionHelp(CvWStringBuffer &szBuffer, const CvUnit* pUnit);

	void buildObsoleteString( CvWStringBuffer& szBuffer, int iItem, bool bList = false, bool bPlayerContext = false );
	void buildObsoleteBonusString( CvWStringBuffer& szBuffer, int iItem, bool bList = false, bool bPlayerContext = false);
	void buildObsoleteSpecialString( CvWStringBuffer& szBuffer, int iItem, bool bList = false, bool bPlayerContext = false );
	void buildMoveString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildCanPassPeaksString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildMoveFastPeaksString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildCanFoundOnPeaksString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildCanRebaseAnywhereString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildFreeUnitString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildFeatureProductionString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildWorkerRateString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildTradeRouteString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildHealthRateString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildSpecialistHealthString( CvWStringBuffer &szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildHappinessRateString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildSpecialistHappinessString( CvWStringBuffer &szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildFreeTechString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildLOSString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildMapCenterString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildMapRevealString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false );
	void buildMapTradeString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildTechTradeString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildGoldTradeString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildOpenBordersString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildDefensivePactString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildPermanentAllianceString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildVassalStateString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildBridgeString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildIrrigationString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildIgnoreIrrigationString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildWaterWorkString( CvWStringBuffer &szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildImprovementString(CvWStringBuffer& szBuffer, TechTypes eTech, BuildTypes eBuild, bool bList = false, bool bPlayerContext = false);
	void buildDomainExtraMovesString( CvWStringBuffer& szBuffer, TechTypes eTech, int iCommerceType, bool bList = false, bool bPlayerContext = false );
	void buildAdjustString( CvWStringBuffer& szBuffer, TechTypes eTech, int iCommerceType, bool bList = false, bool bPlayerContext = false );
	void buildTerrainTradeString( CvWStringBuffer& szBuffer, TechTypes eTech, int iTerrainType, bool bList = false, bool bPlayerContext = false );
	void buildRiverTradeString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildSpecialBuildingString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildYieldChangeString( CvWStringBuffer& szBuffer, TechTypes eTech, int iYieldType, bool bList = false, bool bPlayerContext = false );
	void buildBuildingTechSpecialistChangeString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildBuildingTechHappinessChangesString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildBuildingTechHealthChangesString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildEmbassyString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	bool setBuildingAdditionalDefenseHelp(CvWStringBuffer &szBuffer, const CvCity& city, const CvWString& szStart, bool bStarted = false);
	void getDefenseHelp(CvWStringBuffer &szBuffer, CvCity& city);
	void setEmploymentHelp(CvWStringBuffer &szBuffer, CvCity& city);

	bool buildBonusRevealString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBonusType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	bool buildCivicRevealString( CvWStringBuffer& szBuffer, TechTypes eTech, int iCivicType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	bool buildProcessInfoString( CvWStringBuffer& szBuffer, TechTypes eTech, int iProcessType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	bool buildFoundReligionString( CvWStringBuffer& szBuffer, TechTypes eTech, int iReligionType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	bool buildFoundCorporationString( CvWStringBuffer& szBuffer, TechTypes eTech, int iCorporationType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	bool buildPromotionString(CvWStringBuffer& szBuffer, TechTypes eTech, int iPromotionType, bool bFirst, bool bList) const;
	void buildHintsList(CvWStringBuffer& szBuffer);
	void buildBuildingRequiresString(CvWStringBuffer& szBuffer, BuildingTypes eBuilding, bool bCivilopediaText, bool bTechChooserText, const CvCity* pCity);
	void buildMaintenanceModifiersString(CvWStringBuffer& szBuffer, TechTypes eTech, bool bList);

	DllExport void buildCityBillboardIconString( CvWStringBuffer& szBuffer, CvCity* pCity);
	DllExport void buildCityBillboardCityNameString( CvWStringBuffer& szBuffer, CvCity* pCity);
	DllExport void buildCityBillboardProductionString( CvWStringBuffer& szBuffer, CvCity* pCity);
	DllExport void buildCityBillboardCitySizeString( CvWStringBuffer& szBuffer, CvCity* pCity, const NiColorA& kColor);
	DllExport void getCityBillboardFoodbarColors(CvCity* pCity, std::vector<NiColorA>& aColors);
	DllExport void getCityBillboardProductionbarColors(CvCity* pCity, std::vector<NiColorA>& aColors);

	void buildSingleLineTechTreeString(CvWStringBuffer &szBuffer, TechTypes eTech, bool bPlayerContext);
	void buildTechTreeString(CvWStringBuffer &szBuffer, TechTypes eTech, bool bPlayerContext, TechTypes eFromTech);

	void getWarplanString(CvWStringBuffer& szString, WarPlanTypes eWarPlan);
	void getAttitudeString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer, PlayerTypes eTargetPlayer);
	void getEspionageString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer, PlayerTypes eTargetPlayer);
	void getTradeString(CvWStringBuffer& szBuffer, const TradeData& tradeData, PlayerTypes ePlayer1, PlayerTypes ePlayer2);
	void getDealString(CvWStringBuffer& szString, CvDeal& deal, PlayerTypes ePlayerPerspective = NO_PLAYER);
	void getDealString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer1, PlayerTypes ePlayer2, const CLinkList<TradeData>* pListPlayer1, const CLinkList<TradeData>* pListPlayer2, PlayerTypes ePlayerPerspective = NO_PLAYER);
	void getActiveDealsString(CvWStringBuffer& szString, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer);
	void getOtherRelationsString(CvWStringBuffer& szString, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer);
	void getAllRelationsString(CvWStringBuffer& szString, TeamTypes eThisTeam);
	void getActiveTeamRelationsString(CvWStringBuffer& szString, TeamTypes eThisTeam);
	void getOtherRelationsString(CvWStringBuffer& szString, TeamTypes eThisTeam, TeamTypes eOtherTeam, TeamTypes eSkipTeam);

	void buildFinanceSpecialistGoldString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer) const;
	void buildFinanceInflationString(CvWStringBuffer& szDetails, PlayerTypes ePlayer) const;
	void buildFinanceUnitUpkeepString(CvWStringBuffer& szDetails, PlayerTypes ePlayer) const;
	void buildFinanceAwaySupplyString(CvWStringBuffer& szDetails, PlayerTypes ePlayer) const;
	void buildFinanceCityMaintString(CvWStringBuffer& szDetails, PlayerTypes ePlayer) const;
	void buildFinanceCivicUpkeepString(CvWStringBuffer& szDetails, PlayerTypes ePlayer) const;
	void buildFinanceForeignIncomeString(CvWStringBuffer& szDetails, PlayerTypes ePlayer) const;

	DllExport void getTradeScreenTitleIcon(CvString& szButton, CvWidgetDataStruct& widgetData, PlayerTypes ePlayer);
	DllExport void getTradeScreenIcons(std::vector< std::pair<CvString, CvWidgetDataStruct> >& aIconInfos, PlayerTypes ePlayer);
	DllExport void getTradeScreenHeader(CvWString& szHeader, PlayerTypes ePlayer, PlayerTypes eOtherPlayer, bool bAttitude);

	void buildDomesticTradeString(CvWStringBuffer& szDetails, PlayerTypes ePlayer) const;
	void buildForeignTradeString(CvWStringBuffer& szDetails, PlayerTypes ePlayer) const;
	void buildTradeString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer, PlayerTypes eWithPlayer=NO_PLAYER, bool bDomestic=true, bool bForeign=true, bool bHeading=true) const;

	DllExport void getGlobeLayerName(GlobeLayerTypes eType, int iOption, CvWString& strName);

	DllExport void getPlotHelp(CvPlot* pMouseOverPlot, CvCity* pCity, CvPlot* pFlagPlot, bool bAlt, CvWStringBuffer& strHelp);
	void getRebasePlotHelp(const CvPlot* pPlot, CvWString& strHelp) const;
	DllExport void getInterfaceCenterText(CvWString& strText);
	DllExport void getTurnTimerText(CvWString& strText);
	DllExport void getFontSymbols(std::vector< std::vector<wchar_t> >& aacSymbols, std::vector<int>& aiMaxNumRows);
	DllExport void assignFontIds(int iFirstSymbolCode, int iPadAmount);

	DllExport void getCityDataForAS(std::vector<CvWBData>& mapCityList, std::vector<CvWBData>& mapBuildingList, std::vector<CvWBData>& mapAutomateList);
	DllExport void getUnitDataForAS(std::vector<CvWBData>& mapUnitList);
	DllExport void getImprovementDataForAS(std::vector<CvWBData>& mapImprovementList, std::vector<CvWBData>& mapRouteList);
	DllExport void getVisibilityDataForAS(std::vector<CvWBData>& mapVisibilityList);
	DllExport void getTechDataForAS(std::vector<CvWBData>& mapTechList);

	DllExport void getUnitDataForWB(std::vector<CvWBData>& mapUnitData);
	DllExport void getBuildingDataForWB(bool bStickyButton, std::vector<CvWBData>& mapBuildingData);
	DllExport void getTerrainDataForWB(std::vector<CvWBData>& mapTerrainData, std::vector<CvWBData>& mapFeatureData, std::vector<CvWBData>& mapPlotData, std::vector<CvWBData>& mapRouteData);
	DllExport void getTerritoryDataForWB(std::vector<CvWBData>& mapTerritoryData);
	DllExport void getTechDataForWB(std::vector<CvWBData>& mapTechData);
	DllExport void getPromotionDataForWB(std::vector<CvWBData>& mapPromotionData);
	//void getTraitDataForWB(std::vector<CvWBData>& mapTraitData) const;
	DllExport void getBonusDataForWB(std::vector<CvWBData>& mapBonusData);
	DllExport void getImprovementDataForWB(std::vector<CvWBData>& mapImprovementData);
	//void getRouteDataForWB(std::vector<CvWBData>& mapRouteData) const;
	DllExport void getReligionDataForWB(bool bHolyCity, std::vector<CvWBData>& mapReligionData);
	DllExport void getCorporationDataForWB(bool bHeadquarters, std::vector<CvWBData>& mapCorporationData);

	void setFlagHelp(CvWStringBuffer& szBuffer);

	// AIAndy: Game object relation texts
	void getGameObjectName(CvWString& szString, GameObjectTypes eObject) const;
	void buildGameObjectRelationString(CvWStringBuffer& szBuffer, GameObjectTypes eObject, RelationTypes eRelation, int iData) const;

private:
	void eventTechHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, TechTypes eTech, PlayerTypes ePlayer, PlayerTypes eOtherPlayer) const;
	void eventGoldHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, PlayerTypes ePlayer, PlayerTypes eOtherPlayer) const;

	void setCityPlotYieldValueString(CvWStringBuffer& szString, CvCity* pCity, int iIndex, bool bAvoidGrowth, bool bIgnoreGrowth, bool bIgnoreFood = false) const;
	void setYieldValueString(CvWStringBuffer& szString, int iValue, bool bActive = false, bool bMakeWhitespace = false) const;
};

// Singleton Accessor
#define GAMETEXT CvGameTextMgr::GetInstance()

#endif
