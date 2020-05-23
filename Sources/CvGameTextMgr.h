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
public:
	// singleton accessor
	DllExport static CvGameTextMgr& GetInstance();

	CvGameTextMgr();
	virtual ~CvGameTextMgr();

	DllExport void Initialize();
	DllExport void DeInitialize();
	DllExport void Reset();

	int getCurrentLanguage();

	DllExport void setTimeStr(CvWString& szString, int iGameTurn, bool bSave);
	void setYearStr(CvWString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed);
	void setDateStr(CvWString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed);
	void setInterfaceTime(CvWString& szString, PlayerTypes ePlayer);
	void setGoldStr(CvWString& szString, PlayerTypes ePlayer);
	void setResearchStr(CvWString& szString, PlayerTypes ePlayer);
	void setOOSSeeds(CvWString& szString, PlayerTypes ePlayer);
	void setNetStats(CvWString& szString, PlayerTypes ePlayer);
	DllExport void setMinimizePopupHelp(CvWString& szString, const CvPopupInfo & info);

	void setUnitHelp(CvWStringBuffer &szString, const CvUnit* pUnit, bool bOneLine = false, bool bShort = false);
	void setPlotListHelp(CvWStringBuffer &szString, CvPlot* pPlot, bool bOneLine, bool bShort);
	bool setCombatPlotHelp(CvWStringBuffer &szString, CvPlot* pPlot, bool bAssassinate = false);
	void setPlotHelp(CvWStringBuffer &szString, CvPlot* pPlot);
	void setCityBarHelp(CvWStringBuffer &szString, CvCity* pCity);
	void setScoreHelp(CvWStringBuffer &szString, PlayerTypes ePlayer);

	void parseBuildUp(CvWStringBuffer &szHelpString, PromotionLineTypes eTrait, CivilizationTypes eCivilization = NO_CIVILIZATION);

	void parseTraits(CvWStringBuffer &szHelpString, TraitTypes eTrait, CivilizationTypes eCivilization = NO_CIVILIZATION, bool bDawnOfMan = false, bool bEffectsOnly = false);

	DllExport void parseLeaderTraits(CvWStringBuffer &szInfoText, LeaderHeadTypes eLeader = NO_LEADER, CivilizationTypes eCivilization = NO_CIVILIZATION, bool bDawnOfMan = false, bool bCivilopediaText = false);
	DllExport void parseLeaderShortTraits(CvWStringBuffer &szInfoText, LeaderHeadTypes eLeader);
	DllExport void parseCivInfos(CvWStringBuffer &szHelpString, CivilizationTypes eCivilization, bool bDawnOfMan = false, bool bLinks = true);
	void parseSpecialistHelp(CvWStringBuffer &szHelpString, SpecialistTypes eSpecialist, CvCity* pCity, bool bCivilopediaText = false);
// BUG - Specialist Actual Effects - start
	void parseSpecialistHelpActual(CvWStringBuffer &szHelpString, SpecialistTypes eSpecialist, CvCity* pCity, bool bCivilopediaText = false, int iChange = 0);
// BUG - Specialist Actual Effects - end
	void parseFreeSpecialistHelp(CvWStringBuffer &szHelpString, const CvCity& kCity);
	void parsePromotionHelp(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, const wchar* pcNewline = NEWLINE);
	void parsePromotionHelpInternal(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, const wchar* pcNewline, bool bAccrueLines);
	void parseCivicInfo(CvWStringBuffer &szBuffer, CivicTypes eCivic, bool bCivilopediaText = false, bool bPlayerContext = false, bool bSkipName = false);
	void parsePlayerTraits(CvWStringBuffer &szBuffer, PlayerTypes ePlayer);
	/*** Dexy - Fixed Borders START ****/
	void parsePlayerHasFixedBorders(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer);
	/*** Dexy - Fixed Borders  END  ****/
	void parseLeaderHeadHelp(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer);
	void parseGreatPeopleHelp(CvWStringBuffer &szBuffer, CvCity& city);
// BUG - Building Additional Great People - start
	bool setBuildingAdditionalGreatPeopleHelp(CvWStringBuffer &szBuffer, CvCity& city, const CvWString& szStart, bool bStarted = false);
// BUG - Building Additional Great People - end
	void parseGreatGeneralHelp(CvWStringBuffer &szBuffer, CvPlayer& kPlayer);

	void setTechHelp(CvWStringBuffer &szBuffer, TechTypes eTech, bool bCivilopediaText = false, bool bPlayerContext = false, bool bStrategyText = false, bool bTreeInfo = true, TechTypes eFromTech = NO_TECH);
// BUG - Trade Denial - start
	void setTechTradeHelp(CvWStringBuffer &szBuffer, TechTypes eTech, PlayerTypes eTradePlayer, bool bCivilopediaText = false, bool bPlayerContext = false, bool bStrategyText = false, bool bTreeInfo = true, TechTypes eFromTech = NO_TECH);
// BUG - Trade Denial - end
	void setBasicUnitHelp(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText = false);
// BUG - Starting Experience - start
	void setBasicUnitHelpWithCity(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText = false, CvCity* pCity = NULL, bool bConscript = false, bool bTBUnitView1 = false, bool bTBUnitView2 = false, bool bTBUnitView3 = false);
	void setUnitExperienceHelp(CvWStringBuffer &szBuffer, CvWString szStart, UnitTypes eUnit, CvCity* pCity, bool bConscript = false);
// BUG - Starting Experience - end
	void setUnitHelp(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText = false, bool bStrategyText = false, bool bTechChooserText = false, CvCity* pCity = NULL);
	void setBuildingHelp(CvWStringBuffer &szBuffer, BuildingTypes eBuilding, bool bCivilopediaText = false, bool bStrategyText = false, bool bTechChooserText = false, CvCity* pCity = NULL);
// BUG - Building Actual Effects - start
	void setBuildingActualEffects(CvWStringBuffer &szBuffer, CvWString &szStart, BuildingTypes eBuilding, CvCity* pCity, bool bNewLine = true);
	void setBuildingHelpActual(CvWStringBuffer &szBuffer, BuildingTypes eBuilding, bool bCivilopediaText = false, bool bStrategyText = false, bool bTechChooserText = false, CvCity* pCity = NULL, bool bActual = true);
// BUG - Building Actual Effects - end
// BUG - Production Decay - start
	void setProductionDecayHelp(CvWStringBuffer &szBuffer, int iTurnsLeft, int iThreshold, int iDecay, bool bProducing);
// BUG - Production Decay - end
	void setProjectHelp(CvWStringBuffer &szBuffer, ProjectTypes eProject, bool bCivilopediaText = false, CvCity* pCity = NULL);
	void setProcessHelp(CvWStringBuffer &szBuffer, ProcessTypes eProcess);
	void setGoodHealthHelp(CvWStringBuffer &szBuffer, CvCity& city);
	void setBadHealthHelp(CvWStringBuffer &szBuffer, CvCity& city);
// BUG - Building Additional Health - start
	bool setBuildingAdditionalHealthHelp(CvWStringBuffer &szBuffer, CvCity& city, const CvWString& szStart, bool bStarted = false);
// BUG - Building Additional Health - end
	void parseHappinessHelp(CvWStringBuffer& szBuffer);
	void setAngerHelp(CvWStringBuffer &szBuffer, CvCity& city);
	void setHappyHelp(CvWStringBuffer &szBuffer, CvCity& city);
// BUG - Building Additional Happiness - start
	bool setBuildingAdditionalHappinessHelp(CvWStringBuffer &szBuffer, CvCity& city, const CvWString& szStart, bool bStarted = false);
// BUG - Building Additional Happiness - end
	void setYieldChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piYieldChange, bool bPercent = false, bool bNewLine = true);
	void setCommerceChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piCommerceChange, bool bPercent = false, bool bNewLine = true);
	void setYieldPerPopChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piYieldChange, bool bPercent = false, bool bNewLine = true);
	void setCommercePerPopChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piCommerceChange, bool bPercent = false, bool bNewLine = true);
// BUG - Resumable Value Change Help - start
	void setCommerceTimes100ChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piCommerceChange, bool bNewLine = false, bool bStarted = false);
	bool setResumableYieldChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piYieldChange, bool bPercent = false, bool bNewLine = true, bool bStarted = false, bool bPerPop = false);
	bool setResumableCommerceChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piCommerceChange, bool bPercent = false, bool bNewLine = true, bool bStarted = false, bool bPerPop = false);
	bool setResumableCommerceTimes100ChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piCommerceChange, bool bNewLine, bool bStarted = false);
	bool setResumableGoodBadChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, int iGood, int iGoodSymbol, int iBad, int iBadSymbol, bool bPercent = false, bool bNewLine = false, bool bStarted = false);
	bool setResumableValueChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, int iValue, int iSymbol, bool bPercent = false, bool bNewLine = false, bool bStarted = false);
	bool setResumableValueTimes100ChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, int iValue, int iSymbol, bool bNewLine = false, bool bStarted = false);
// BUG - Resumable Value Change Help - end
	void setBonusHelp(CvWStringBuffer &szBuffer, BonusTypes eBonus, bool bCivilopediaText = false);
// BUG - Trade Denial - start
	void setBonusTradeHelp(CvWStringBuffer &szBuffer, BonusTypes eBonus, bool bCivilopediaText, PlayerTypes eTradePlayer);
// BUG - Trade Denial - end
	void setReligionHelp(CvWStringBuffer &szBuffer, ReligionTypes eReligion, bool bCivilopedia = false);
	void setReligionHelpCity(CvWStringBuffer &szBuffer, ReligionTypes eReligion, CvCity *pCity, bool bCityScreen = false, bool bForceReligion = false, bool bForceState = false, bool bNoStateReligion = false);
	void setCorporationHelp(CvWStringBuffer &szBuffer, CorporationTypes eCorporation, bool bCivilopedia = false);
	void setCorporationHelpCity(CvWStringBuffer &szBuffer, CorporationTypes eCorporation, CvCity *pCity, bool bCityScreen = false, bool bForceCorporation = false);
	void setPromotionHelp(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, bool bCivilopediaText = false);
	void setBuildUpHelp(CvWStringBuffer &szBuffer, PromotionLineTypes ePromotionLine);
	void setTraitHelp(CvWStringBuffer &szBuffer, TraitTypes eTrait);
	void setUnitCombatHelp(CvWStringBuffer &szBuffer, UnitCombatTypes eUnitCombat, bool bCivilopediaText = false, bool bFromUnit = false);
	void setImprovementHelp(CvWStringBuffer &szBuffer, ImprovementTypes eImprovement, FeatureTypes eFeature = NO_FEATURE, bool bCivilopediaText = false);
	void setRouteHelp(CvWStringBuffer &szBuffer, RouteTypes eRoute, bool bCivilopediaText = false);
	void setTerrainHelp(CvWStringBuffer &szBuffer, TerrainTypes eTerrain, bool bCivilopediaText = false);
	void setFeatureHelp(CvWStringBuffer &szBuffer, FeatureTypes eFeature, bool bCivilopediaText = false);
// BUG - Food Rate Hover - start
	void setFoodHelp(CvWStringBuffer &szBuffer, CvCity& city);
// BUG - Food Rate Hover - end
// BUG - Building Additional Yield - start
	bool setBuildingAdditionalYieldHelp(CvWStringBuffer &szBuffer, CvCity& city, YieldTypes eIndex, const CvWString& szStart, bool bStarted = false);
// BUG - Building Additional Yield - end
	void setProductionHelp(CvWStringBuffer &szBuffer, CvCity& city);
	void setCommerceHelp(CvWStringBuffer &szBuffer, CvCity& city, CommerceTypes eCommerceType);
// BUG - Building Additional Commerce - start
	bool setBuildingAdditionalCommerceHelp(CvWStringBuffer &szBuffer, CvCity& city, CommerceTypes eIndex, const CvWString& szStart, bool bStarted = false);
// BUG - Building Additional Commerce - end
// BUG - Building Saved Maintenance - start
	bool setBuildingSavedMaintenanceHelp(CvWStringBuffer &szBuffer, const CvCity& city, const CvWString& szStart, bool bStarted = false);
// BUG - Building Saved Maintenance - start
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
/************************************************************************************************/
/* Afforess	Mountains Start		 09/20/09                                           		 */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void buildCanPassPeaksString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildMoveFastPeaksString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildCanFoundOnPeaksString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildCanRebaseAnywhereString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                             */
/************************************************************************************************/
	void buildFreeUnitString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildFeatureProductionString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildWorkerRateString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildTradeRouteString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildHealthRateString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
//Team Project (1)
	void buildSpecialistHealthString( CvWStringBuffer &szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildHappinessRateString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
//Team Project (1)
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
	void buildImprovementString( CvWStringBuffer& szBuffer, TechTypes eTech, int iImprovement, bool bList = false, bool bPlayerContext = false );
	void buildDomainExtraMovesString( CvWStringBuffer& szBuffer, TechTypes eTech, int iCommerceType, bool bList = false, bool bPlayerContext = false );
	void buildAdjustString( CvWStringBuffer& szBuffer, TechTypes eTech, int iCommerceType, bool bList = false, bool bPlayerContext = false );
	void buildTerrainTradeString( CvWStringBuffer& szBuffer, TechTypes eTech, int iTerrainType, bool bList = false, bool bPlayerContext = false );
	void buildRiverTradeString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	void buildSpecialBuildingString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildYieldChangeString( CvWStringBuffer& szBuffer, TechTypes eTech, int iYieldType, bool bList = false, bool bPlayerContext = false );
/************************************************************************************************/
/* Afforess	                  Start		 01/01/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void buildBuildingTechCommerceChangeString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildBuildingTechYieldChangeString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildBuildingTechSpecialistChangeString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildBuildingTechCommerceModifierString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildBuildingTechYieldModifierString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildBuildingTechHappinessChangesString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildBuildingTechHealthChangesString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBuildingType, bool bList = false, bool bPlayerContext = false );
	void buildEmbassyString( CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	bool setBuildingAdditionalDefenseHelp(CvWStringBuffer &szBuffer, const CvCity& city, const CvWString& szStart, bool bStarted = false);
	void setDefenseHelp(CvWStringBuffer &szBuffer, CvCity& city);
	void setEmploymentHelp(CvWStringBuffer &szBuffer, CvCity& city);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	bool buildBonusRevealString( CvWStringBuffer& szBuffer, TechTypes eTech, int iBonusType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	bool buildCivicRevealString( CvWStringBuffer& szBuffer, TechTypes eTech, int iCivicType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	bool buildProcessInfoString( CvWStringBuffer& szBuffer, TechTypes eTech, int iProcessType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	bool buildFoundReligionString( CvWStringBuffer& szBuffer, TechTypes eTech, int iReligionType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	bool buildFoundCorporationString( CvWStringBuffer& szBuffer, TechTypes eTech, int iCorporationType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	bool buildPromotionString( CvWStringBuffer& szBuffer, TechTypes eTech, int iPromotionType, bool bFirst, bool bList = false, bool bPlayerContext = false );
	void buildHintsList(CvWStringBuffer& szBuffer);
	void buildBuildingRequiresString(CvWStringBuffer& szBuffer, BuildingTypes eBuilding, bool bCivilopediaText, bool bTechChooserText, const CvCity* pCity);
	//DPII < Maintenance Modifier >
	void buildMaintenanceModifiersString(CvWStringBuffer& szBuffer, TechTypes eTech, bool bList = false, bool bPlayerContext = false );
	//DPII < Maintenance Modifier >
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
// BUG - Leaderhead Relations - start
	void getAllRelationsString(CvWStringBuffer& szString, TeamTypes eThisTeam);
	void getActiveTeamRelationsString(CvWStringBuffer& szString, TeamTypes eThisTeam);
	void getOtherRelationsString(CvWStringBuffer& szString, TeamTypes eThisTeam, TeamTypes eOtherTeam, TeamTypes eSkipTeam);
// BUG - Leaderhead Relations - end

// BUG - Finance Advisor - start
	void buildFinanceSpecialistGoldString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer);
// BUG - Finance Advisor - end

	void buildFinanceInflationString(CvWStringBuffer& szDetails, PlayerTypes ePlayer);
	void buildFinanceUnitCostString(CvWStringBuffer& szDetails, PlayerTypes ePlayer);
	void buildFinanceAwaySupplyString(CvWStringBuffer& szDetails, PlayerTypes ePlayer);
	void buildFinanceCityMaintString(CvWStringBuffer& szDetails, PlayerTypes ePlayer);
	void buildFinanceCivicUpkeepString(CvWStringBuffer& szDetails, PlayerTypes ePlayer);
	void buildFinanceForeignIncomeString(CvWStringBuffer& szDetails, PlayerTypes ePlayer);

	DllExport void getTradeScreenTitleIcon(CvString& szButton, CvWidgetDataStruct& widgetData, PlayerTypes ePlayer);
	DllExport void getTradeScreenIcons(std::vector< std::pair<CvString, CvWidgetDataStruct> >& aIconInfos, PlayerTypes ePlayer);
	DllExport void getTradeScreenHeader(CvWString& szHeader, PlayerTypes ePlayer, PlayerTypes eOtherPlayer, bool bAttitude);

// BUG - Trade Hover - start
	void buildDomesticTradeString(CvWStringBuffer& szDetails, PlayerTypes ePlayer);
	void buildForeignTradeString(CvWStringBuffer& szDetails, PlayerTypes ePlayer);
	void buildTradeString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer, PlayerTypes eWithPlayer=NO_PLAYER, bool bDomestic=true, bool bForeign=true, bool bHeading=true);
// BUG - Trade Hover - end

	DllExport void getGlobeLayerName(GlobeLayerTypes eType, int iOption, CvWString& strName);

	DllExport void getPlotHelp(CvPlot* pMouseOverPlot, CvCity* pCity, CvPlot* pFlagPlot, bool bAlt, CvWStringBuffer& strHelp);
	void getRebasePlotHelp(CvPlot* pPlot, CvWString& strHelp);
	void getNukePlotHelp(CvPlot* pPlot, CvWString& strHelp);
	DllExport void getInterfaceCenterText(CvWString& strText);
	DllExport void getTurnTimerText(CvWString& strText);
	DllExport void getFontSymbols(std::vector< std::vector<wchar> >& aacSymbols, std::vector<int>& aiMaxNumRows);
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
	void getTraitDataForWB(std::vector<CvWBData>& mapTraitData);
	DllExport void getBonusDataForWB(std::vector<CvWBData>& mapBonusData);
	DllExport void getImprovementDataForWB(std::vector<CvWBData>& mapImprovementData);
	void getRouteDataForWB(std::vector<CvWBData>& mapRouteData);
	DllExport void getReligionDataForWB(bool bHolyCity, std::vector<CvWBData>& mapReligionData);
	DllExport void getCorporationDataForWB(bool bHeadquarters, std::vector<CvWBData>& mapCorporationData);

	// AIAndy: Game object relation texts
	void getGameObjectName(CvWString& szString, GameObjectTypes eObject);
	void buildGameObjectRelationString(CvWStringBuffer& szBuffer, GameObjectTypes eObject, RelationTypes eRelation, int iData);

private:
	void eventTechHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, TechTypes eTech, PlayerTypes ePlayer, PlayerTypes eOtherPlayer);
	void eventGoldHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, PlayerTypes ePlayer, PlayerTypes eOtherPlayer);

	void setCityPlotYieldValueString(CvWStringBuffer &szString, CvCity* pCity, int iIndex, bool bAvoidGrowth, bool bIgnoreGrowth, bool bIgnoreFood = false);
	void setYieldValueString(CvWStringBuffer &szString, int iValue, bool bActive = false, bool bMakeWhitespace = false);
};

// Singleton Accessor
#define GAMETEXT CvGameTextMgr::GetInstance()

#endif
