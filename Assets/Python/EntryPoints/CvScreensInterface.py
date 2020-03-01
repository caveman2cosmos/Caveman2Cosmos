## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005

from CvPythonExtensions import *
from CvScreenEnums import *
import types

import CvMainInterface

import CvOptionsScreen
import CvReplayScreen

import CvWorldBuilderScreen

#import CvDiplomacy

import BuildListScreen

import CvEventInterface
import CvPopupInterface
import ScreenInput as PyScreenInput

import BugCore
AdvisorOpt = BugCore.game.Advisors

import C2CMainOptions
import C2CNationAdvisor

g_iScreenActive = -2

def toggleSetNoScreens():
	global g_iScreenActive
	print "SCREEN %s TURNED OFF" %(g_iScreenActive)
	toggleSetScreenOn((MAIN_INTERFACE,))

def toggleSetScreenOn(argsList):
	global g_iScreenActive
	if g_iScreenActive == -2:
		import ScreenResolution as SR
		if not SR.x:
			print "Fetch resolution setting from profileName.pfl"
			UserProfile = CyUserProfile()
			print "\nAll User Profiles:"
			for i in xrange(UserProfile.getNumProfileFiles()):
				print UserProfile.getProfileFileName(i)
			print "Current profile: " + UserProfile.getProfileName()
			szRes = UserProfile.getResolutionString(UserProfile.getResolution())
			szRes = szRes.split(" x ")
			SR.x = int(szRes[0])
			SR.y = int(szRes[1])
			print "Resolution: %dx%d" %(SR.x, SR.y)
			SR.calibrate()
	print "SCREEN %s TURNED ON" %(argsList[0])
	g_iScreenActive = argsList[0]

#diplomacyScreen = CvDiplomacy.CvDiplomacy()

c2cMainOptionsScreen = C2CMainOptions.C2CMainOptions()
def showC2CMainOptionsScreen():
	c2cMainOptionsScreen.interfaceScreen()

c2cNationAdvisorScreen = C2CNationAdvisor.C2CNationAdvisor()
def showC2CNationAdvisorScreen():
	c2cNationAdvisorScreen.interfaceScreen()


mainInterface = CvMainInterface.CvMainInterface()
def showMainInterface():
	print "showMainInterface"
	mainInterface.interfaceScreen()

def reinitMainInterface():
	print "reinitMainInterface"
	global mainInterface
	mainInterface = CvMainInterface.CvMainInterface()
	mainInterface.interfaceScreen()

def numPlotListButtons(): return 0 # Called from exe.

def showTechChooser():
	if CyGame().getActivePlayer() != -1:
		screenMap[TECH_CHOOSER].interfaceScreen(TECH_CHOOSER)

def showHallOfFame(argsList):
	screenMap[HALL_OF_FAME].interfaceScreen(argsList[0])

def showCivicsScreen():
	if CyGame().getActivePlayer() != -1:
		screenMap[CIVICS_SCREEN].interfaceScreen()

def showReligionScreen():
	if CyGame().getActivePlayer() != -1:
		screenMap[RELIGION_SCREEN].interfaceScreen()

def showCorporationScreen():
	if CyGame().getActivePlayer() != -1:
		screenMap[CORPORATION_SCREEN].interfaceScreen()

optionsScreen = CvOptionsScreen.CvOptionsScreen()
def showOptionsScreen():
	optionsScreen.interfaceScreen()

def showForeignAdvisorScreen(argsList):
	if CyGame().getActivePlayer() != -1:
		screenMap[FOREIGN_ADVISOR].interfaceScreen()

def showFinanceAdvisor():
	if CyGame().getActivePlayer() != -1:
		screenMap[FINANCE_ADVISOR].interfaceScreen()

def showDomesticAdvisor(argsList):
	if CyGame().getActivePlayer() != -1:
		screenMap[DOMESTIC_ADVISOR].interfaceScreen()

def showEspionageAdvisor():
	if CyGame().getActivePlayer() != -1:
		screenMap[ESPIONAGE_ADVISOR].interfaceScreen()

def showDawnOfMan(argsList):
	screenMap[DAWN_OF_MAN].interfaceScreen(DAWN_OF_MAN)

def showIntroMovie(argsList):
	screenMap[INTRO_MOVIE_SCREEN].interfaceScreen()

def showVictoryMovie(argsList):
	screenMap[VICTORY_MOVIE_SCREEN].interfaceScreen(argsList[0])

def showWonderMovie(argsList):
	screenMap[WONDER_MOVIE_SCREEN].interfaceScreen(argsList[0], argsList[1], argsList[2], WONDER_MOVIE_SCREEN)

def showEraMovie(argsList):
	screenMap[ERA_MOVIE_SCREEN].interfaceScreen(argsList[0])

def showSpaceShip(argsList):
	if CyGame().getActivePlayer() != -1:
		screenMap[SPACE_SHIP_SCREEN].interfaceScreen(argsList[0])

replayScreen = CvReplayScreen.CvReplayScreen(REPLAY_SCREEN)
def showReplay(argsList):
	if argsList[0] > -1:
		CyGame().saveReplay(argsList[0])
	replayScreen.showScreen(argsList[4])

def showDanQuayleScreen(argsList):
	screenMap[DAN_QUAYLE_SCREEN].interfaceScreen()

def showUnVictoryScreen(argsList):
	unVictoryScreen.interfaceScreen()

def showTopCivs():
	screenMap[TOP_CIVS].showScreen()

def showInfoScreen(argsList):
	if CyGame().getActivePlayer() != -1:
		iTabID = argsList[0]
		iEndGame = argsList[1]
		screenMap[INFO_SCREEN].showScreen(-1, iTabID, iEndGame)

def showDebugInfoScreen():
	screenMap[DEBUG_INFO_SCREEN].interfaceScreen()

def configTechSplash(option=None, value=None):
	if value is None:
		TechWindowOpt = BugCore.game.TechWindow
		if TechWindowOpt.isWideView():
			value = True

	if value:
		import TechWindowWide
		screen = TechWindowWide.CvTechSplashScreen(TECH_SPLASH)
	else:
		import TechWindow
		screen = TechWindow.CvTechSplashScreen(TECH_SPLASH)
	screenMap[TECH_SPLASH] = screen

def showTechSplash(argsList):
	if TECH_SPLASH not in screenMap:
		configTechSplash()
	screenMap[TECH_SPLASH].interfaceScreen(argsList[0])

def showVictoryScreen():
	if CyGame().getActivePlayer() != -1:
		screenMap[VICTORY_SCREEN].interfaceScreen()


# RevolutionWatchAdvisor
revolutionWatchAdvisor = None
def createRevolutionWatchAdvisor():
	"""Creates the Revolution Watch Advisor."""
	global revolutionWatchAdvisor
	if revolutionWatchAdvisor is None:
		import RevolutionWatchAdvisor
		revolutionWatchAdvisor = RevolutionWatchAdvisor.RevolutionWatchAdvisor()
		screenMap[REVOLUTION_WATCH_ADVISOR] = revolutionWatchAdvisor

def showRevolutionWatchAdvisor(argsList):
	if CyGame().getActivePlayer() != -1:
		revolutionWatchAdvisor.interfaceScreen()

def isRevolutionWatchAdvisor():
	return revolutionWatchAdvisor.isVisible()

def cityScreenRedraw():
	mainInterface.updateCityScreen()

# !RevolutionWatchAdvisor

buildListScreen = BuildListScreen.BuildListScreen()
def showBuildListScreen():
	if CyGame().getActivePlayer() != -1:
		buildListScreen.interfaceScreen()

def showForgetfulScreen():
	screenMap[FORGETFUL_SCREEN].interfaceScreen(FORGETFUL_SCREEN)

#################################################
## Pedia
#################################################

def linkToPedia(argsList):
	screenMap[PEDIA].link(argsList)

def pediaShow():
	screenMap[PEDIA].pediaShow()

def pediaBack():
	screenMap[PEDIA].back()

def pediaForward():
	screenMap[PEDIA].forward()

def pediaJumpToBuilding(argsList):
	screenMap[PEDIA].pediaJump(-3, "", argsList[0])

def pediaJumpToUnit(argsList):
	screenMap[PEDIA].pediaJump(-2, "", argsList[0])

def pediaMain(argsList):
	screenMap[PEDIA].pediaJump(-1, "", argsList[0])

def pediaShowHistorical(argsList):
	if argsList[0] == CivilopediaPageTypes.CIVILOPEDIA_PAGE_CONCEPT_NEW:
		screenMap[PEDIA].pediaJump(0, "NEW", argsList[1])
	else:
		screenMap[PEDIA].pediaJump(0, "", argsList[1])

def pediaJumpToTech(argsList):
	screenMap[PEDIA].pediaJump(1, "", argsList[0])

def pediaJumpToPromotion(argsList):
	screenMap[PEDIA].pediaJump(4, "", argsList[0])

def pediaJumpToBonus(argsList):
	screenMap[PEDIA].pediaJump(7, "", argsList[0])

def pediaJumpToTerrain(argsList):
	screenMap[PEDIA].pediaJump(8, "Terrain", argsList[0])

def pediaJumpToFeature(argsList):
	screenMap[PEDIA].pediaJump(8, "Feature", argsList[0])

def pediaJumpToImprovement(argsList):
	screenMap[PEDIA].pediaJump(8, "Improvement", argsList[0])

def pediaJumpToTrait(argsList):
	screenMap[PEDIA].pediaJump(9, "Trait", argsList[0])

def pediaJumpToCiv(argsList):
	screenMap[PEDIA].pediaJump(9, "Civ", argsList[0])

def pediaJumpToLeader(argsList):
	screenMap[PEDIA].pediaJump(9, "Leader", argsList[0])

def pediaJumpToCivic(argsList):
	screenMap[PEDIA].pediaJump(9, "Civic", argsList[0])

def pediaJumpToReligion(argsList):
	screenMap[PEDIA].pediaJump(9, "Religion", argsList[0])

def pediaJumpToUnitChart(argsList):
	screenMap[PEDIA].pediaJump(10, "UnitCombat", argsList[0])

def pediaJumpToProject(argsList):
	screenMap[PEDIA].pediaJump(10, "Project", argsList[0])

def pediaJumpToSpecialist(argsList):
	screenMap[PEDIA].pediaJump(10, "Specialist", argsList[0])

def pediaJumpToCorporation(argsList):
	screenMap[PEDIA].pediaJump(10, "Corporation", argsList[0])

def pediaJumpToRoute(argsList):
	if argsList[0] > -1:
		screenMap[PEDIA].pediaJump(8, "Route", argsList[0])
	else:
		screenMap[PEDIA].pediaJump(10, "Build", argsList[0] + 100000)

def pediaJumpToEra(iEra):
	screenMap[PEDIA].pediaJump(0, "Eras", iEra)

#################################################
## Worldbuilder
#################################################
worldBuilderScreen = CvWorldBuilderScreen.CvWorldBuilderScreen(WORLDBUILDER_SCREEN)
def getWorldBuilderScreen():
	return worldBuilderScreen

def showWorldBuilderScreen():
	worldBuilderScreen.interfaceScreen()

def hideWorldBuilderScreen():
	worldBuilderScreen.killScreen()

def WorldBuilderToggleUnitEditCB():
	worldBuilderScreen.toggleUnitEditCB()

def WorldBuilderAllPlotsCB():
	CvEventInterface.beginEvent(5008)

def WorldBuilderEraseCB():
	worldBuilderScreen.eraseCB()

def WorldBuilderLandmarkCB():
	worldBuilderScreen.landmarkModeCB()

def WorldBuilderExitCB():
	CyInterface().setWorldBuilder(False)

def WorldBuilderToggleCityEditCB():
	worldBuilderScreen.toggleCityEditCB()

def WorldBuilderNormalPlayerTabModeCB():
	worldBuilderScreen.normalPlayerTabModeCB()

def WorldBuilderNormalMapTabModeCB():
	worldBuilderScreen.normalMapTabModeCB()

def WorldBuilderRevealTabModeCB():
	worldBuilderScreen.revealTabModeCB()

def WorldBuilderDiplomacyModeCB():
	worldBuilderScreen.diplomacyModeCB()

def WorldBuilderRevealAllCB():
	worldBuilderScreen.revealAll(True)

def WorldBuilderUnRevealAllCB():
	worldBuilderScreen.revealAll(False)

def WorldBuilderHandleUnitCB( argsList ):
	worldBuilderScreen.handleUnitCB(argsList)

def WorldBuilderHandleTerrainCB( argsList ):
	worldBuilderScreen.handleTerrainCB(argsList)

def WorldBuilderHandleFeatureCB(argsList):
	worldBuilderScreen.handleFeatureCB(argsList)

def WorldBuilderHandleBonusCB( argsList ):
	worldBuilderScreen.handleBonusCB(argsList)

def WorldBuilderHandleImprovementCB(argsList):
	worldBuilderScreen.handleImprovementCB(argsList)

def WorldBuilderHandleTerritoryCB(argsList):
	worldBuilderScreen.handleTerritoryCB(argsList)

def WorldBuilderHandlePlotTypeCB( argsList ):
	worldBuilderScreen.handlePlotTypeCB(argsList)

def WorldBuilderHandleAllPlotsCB( argsList ):
	worldBuilderScreen.handleAllPlotsCB(argsList)

def WorldBuilderHandleUnitEditExperienceCB( argsList ):
	worldBuilderScreen.handleUnitEditExperienceCB(argsList)

def WorldBuilderHandleUnitEditLevelCB( argsList ):
	worldBuilderScreen.handleUnitEditLevelCB(argsList)

def WorldBuilderHandleUnitEditNameCB( argsList ):
	worldBuilderScreen.handleUnitEditNameCB(argsList)

def WorldBuilderHandleCityEditPopulationCB( argsList ):
	worldBuilderScreen.handleCityEditPopulationCB(argsList)

def WorldBuilderHandleCityEditCultureCB( argsList ):
	worldBuilderScreen.handleCityEditCultureCB(argsList)

def WorldBuilderHandleCityEditNameCB( argsList ):
	worldBuilderScreen.handleCityEditNameCB(argsList)

def WorldBuilderHandleLandmarkTextCB( argsList ):
	worldBuilderScreen.handleLandmarkTextCB(argsList)

def WorldBuilderHandleUnitEditPullDownCB( argsList ):
	worldBuilderScreen.handleUnitEditPullDownCB(argsList)

def WorldBuilderHandleUnitAITypeEditPullDownCB( argsList ):
	worldBuilderScreen.handleUnitAITypeEditPullDownCB(argsList)

def WorldBuilderHandlePlayerEditPullDownCB( argsList ):
	worldBuilderScreen.handlePlayerEditPullDownCB(argsList)

def WorldBuilderHandlePlayerUnitPullDownCB( argsList ):
	worldBuilderScreen.handlePlayerUnitPullDownCB(argsList)

def WorldBuilderHandleSelectTeamPullDownCB( argsList ):
	worldBuilderScreen.handleSelectTeamPullDownCB(argsList)

def WorldBuilderHandleBuildingCB( argsList ):
	worldBuilderScreen.handleBuildingCB(argsList)

def WorldBuilderHandleTechCB( argsList ):
	worldBuilderScreen.handleTechCB(argsList)

def WorldBuilderHandleRouteCB( argsList ):
	worldBuilderScreen.handleRouteCB(argsList)

def WorldBuilderHandleEditCityBuildingCB( argsList ):
	worldBuilderScreen.handleEditCityBuildingCB(argsList)

def WorldBuilderHandleBrushWidthCB( argsList ):
	worldBuilderScreen.handleBrushWidthCB(argsList)

def WorldBuilderHandleBrushHeightCB( argsList ):
	worldBuilderScreen.handleBrushHeightCB(argsList)

def WorldBuilderHandleLandmarkCB( argsList ):
	worldBuilderScreen.handleLandmarkCB(argsList)

## Platy World Builder Start ##

def WorldBuilderHandleEditScriptCB( argsList ):
	worldBuilderScreen.handleEditScriptCB(argsList)

def WorldBuilderHandleEnterNewScreenCB( argsList ):
	worldBuilderScreen.handleEnterNewScreenCB(argsList)

## Unit Data ##

def WorldBuilderHandleUnitEditStrengthCB( argsList ):
	worldBuilderScreen.handleUnitEditStrengthCB(argsList)

def WorldBuilderHandleUnitEditDamageCB( argsList ):
	worldBuilderScreen.handleUnitEditDamageCB(argsList)

def WorldBuilderHandleUnitEditCargoCB( argsList ):
	worldBuilderScreen.handleUnitEditCargoCB(argsList)

def WorldBuilderHandleUnitEditMovesCB( argsList ):
	worldBuilderScreen.handleUnitEditMovesCB(argsList)

def WorldBuilderHandleUnitEditImmobileTimerCB( argsList ):
	worldBuilderScreen.handleUnitEditImmobileTimerCB(argsList)

def WorldBuilderHandleUnitEditPromotionReadyCB( argsList ):
	worldBuilderScreen.handleUnitEditPromotionReadyCB(argsList)

def WorldBuilderHandleUnitEditMadeAttackCB( argsList ):
	worldBuilderScreen.handleUnitEditMadeAttackCB(argsList)

def WorldBuilderHandleUnitEditMadeInterceptionCB( argsList ):
	worldBuilderScreen.handleUnitEditMadeInterceptionCB(argsList)

def WorldBuilderHandleMoveUnitCB( argsList ):
	worldBuilderScreen.handleMoveUnitCB(argsList)

def WorldBuilderHandleEditUnitPromotionCB( argsList ):
	worldBuilderScreen.handleEditUnitPromotionCB(argsList)

def WorldBuilderHandlePromotionCommandsCB( argsList ):
	worldBuilderScreen.handlePromotionCommandsCB(argsList)

def WorldBuilderHandleUnitEditDuplicateCB( argsList ):
	worldBuilderScreen.handleUnitEditDuplicateCB(argsList)

def WorldBuilderHandleKillCB( argsList ):
	worldBuilderScreen.handleKillCB(argsList)

def WorldBuilderHandleUnitEditDirectionCB( argsList ):
	worldBuilderScreen.handleUnitEditDirectionCB(argsList)

## Game Option ##

def WorldBuilderHandleEditGameOptionCB( argsList ):
	worldBuilderScreen.handleEditGameOptionCB(argsList)

def WorldBuilderHandleEditGlobalDefineCB( argsList ):
	worldBuilderScreen.handleEditGlobalDefineCB(argsList)

def WorldBuilderHandleGlobalDefineINTCB( argsList ):
	worldBuilderScreen.handleGlobalDefineINTCB(argsList)

def WorldBuilderHandleGameEditStartYearCB( argsList ):
	worldBuilderScreen.handleGameEditStartYearCB(argsList)

## Create Player ##

def WorldBuilderHandleAddPlayerCivilizationCB( argsList ):
	worldBuilderScreen.handleAddPlayerCivilizationCB(argsList)

def WorldBuilderHandleAddPlayerLeaderTypeCB( argsList ):
	worldBuilderScreen.handleAddPlayerLeaderTypeCB(argsList)

def WorldBuilderHandleCreatePlayerCB( argsList ):
	worldBuilderScreen.handleCreatePlayerCB(argsList)

## City Data ##

def WorldBuilderHandleChooseCityCB( argsList ):
	worldBuilderScreen.handleChooseCityCB(argsList)

def WorldBuilderHandleCityEditCultureLevelCB( argsList ):
	worldBuilderScreen.handleCityEditCultureLevelCB(argsList)

def WorldBuilderHandleCityEditHappinessCB( argsList ):
	worldBuilderScreen.handleCityEditHappinessCB(argsList)

def WorldBuilderHandleCityEditHealthCB( argsList ):
	worldBuilderScreen.handleCityEditHealthCB(argsList)

def WorldBuilderHandleCityEditTimersCB( argsList ):
	worldBuilderScreen.handleCityEditTimersCB(argsList)

def WorldBuilderHandleCityEditCurrentTimerCB( argsList ):
	worldBuilderScreen.handleCityEditCurrentTimerCB(argsList)

def WorldBuilderHandleCityEditDefenseCB( argsList ):
	worldBuilderScreen.handleCityEditDefenseCB(argsList)

def WorldBuilderHandleCityEditTradeRouteCB( argsList ):
	worldBuilderScreen.handleCityEditTradeRouteCB(argsList)

def WorldBuilderHandleCityEditGPRateCB( argsList ):
	worldBuilderScreen.handleCityEditGPRateCB(argsList)

def WorldBuilderHandleBuildingCommandsCB( argsList ):
	worldBuilderScreen.handleBuildingCommandsCB(argsList)

def WorldBuilderHandleCityEditSpecialistCB( argsList ):
	worldBuilderScreen.handleCityEditSpecialistCB(argsList)

def WorldBuilderHandleCityEditGreatPeopleCB( argsList ):
	worldBuilderScreen.handleCityEditGreatPeopleCB(argsList)

def WorldBuilderHandleCityEditBonusCB( argsList ):
	worldBuilderScreen.handleCityEditBonusCB(argsList)

def WorldBuilderHandleCityEditReligionCB( argsList ):
	worldBuilderScreen.handleCityEditReligionCB(argsList)

def WorldBuilderHandleCityEditHolyCityCB( argsList ):
	worldBuilderScreen.handleCityEditHolyCityCB(argsList)

def WorldBuilderHandleReligionCommandsCB( argsList ):
	worldBuilderScreen.handleReligionCommandsCB(argsList)

def WorldBuilderHandleCityEditHeadquartersCB( argsList ):
	worldBuilderScreen.handleCityEditHeadquartersCB(argsList)

def WorldBuilderHandleCityEditCorporationCB( argsList ):
	worldBuilderScreen.handleCityEditCorporationCB(argsList)

def WorldBuilderHandleCorporationCommandsCB( argsList ):
	worldBuilderScreen.handleCorporationCommandsCB(argsList)

def WorldBuilderHandleCityEditBuildingClassCB( argsList ):
	worldBuilderScreen.handleCityEditBuildingClassCB(argsList)

def WorldBuilderHandleCityEditModiferCB( argsList ):
	worldBuilderScreen.handleCityEditModiferCB(argsList)

def WorldBuilderHandleCityEditModifyBuildingClassCB( argsList ):
	worldBuilderScreen.handleCityEditModifyBuildingClassCB(argsList)

def WorldBuilderHandleCityEditChooseProductionCB( argsList ):
	worldBuilderScreen.handleCityEditChooseProductionCB(argsList)

def WorldBuilderHandleCityEditProductionProgressCB( argsList ):
	worldBuilderScreen.handleCityEditProductionProgressCB(argsList)

def WorldBuilderHandleCityEditFoodCB( argsList ):
	worldBuilderScreen.handleCityEditFoodCB(argsList)

## Player Data ##

def WorldBuilderHandlePlayerEditGoldCB( argsList ):
	worldBuilderScreen.handlePlayerEditGoldCB(argsList)

def WorldBuilderHandleCurrentEraEditPullDownCB( argsList ):
	worldBuilderScreen.handleCurrentEraEditPullDownCB(argsList)

def WorldBuilderHandleTeamEditCommerceFlexibleCB( argsList ):
	worldBuilderScreen.handleTeamEditCommerceFlexibleCB(argsList)

def WorldBuilderHandlePlayerEditCommercePercentCB( argsList ):
	worldBuilderScreen.handlePlayerEditCommercePercentCB(argsList)

def WorldBuilderHandlePlayerEditGoldenAgeCB( argsList ):
	worldBuilderScreen.handlePlayerEditGoldenAgeCB(argsList)

def WorldBuilderHandlePlayerEditGoldenAgeUnitsCB( argsList ):
	worldBuilderScreen.handlePlayerEditGoldenAgeUnitsCB(argsList)

def WorldBuilderHandlePlayerEditAnarchyCB( argsList ):
	worldBuilderScreen.handlePlayerEditAnarchyCB(argsList)

def WorldBuilderHandlePlayerEditCombatExperienceCB( argsList ):
	worldBuilderScreen.handlePlayerEditCombatExperienceCB(argsList)

def WorldBuilderHandlePlayerEditCivicCB( argsList ):
	worldBuilderScreen.handlePlayerEditCivicCB(argsList)

def WorldBuilderHandleStateReligionEditPullDownCB( argsList ):
	worldBuilderScreen.handleStateReligionEditPullDownCB(argsList)

def WorldBuilderHandlePlayerEditStateReligionUnitProductionCB( argsList ):
	worldBuilderScreen.handlePlayerEditStateReligionUnitProductionCB(argsList)

def WorldBuilderHandlePlayerEditStateReligionBuildingProductionCB( argsList ):
	worldBuilderScreen.handlePlayerEditStateReligionBuildingProductionCB(argsList)

def WorldBuilderHandleCurrentTechEditPullDownCB( argsList ):
	worldBuilderScreen.handleCurrentTechEditPullDownCB(argsList)

def WorldBuilderHandleTeamEditResearchProgressCB( argsList ):
	worldBuilderScreen.handleTeamEditResearchProgressCB(argsList)

def WorldBuilderHandleTriggerEventCB( argsList ):
	worldBuilderScreen.handleTriggerEventCB(argsList)

## Team Data ##

def WorldBuilderHandleTeamEditPullDownCB( argsList ):
	worldBuilderScreen.handleTeamEditPullDownCB(argsList)

def WorldBuilderHandleAddTeamCB( argsList ):
	worldBuilderScreen.handleAddTeamCB(argsList)

def WorldBuilderHandleEditTeamProjectCB( argsList ):
	worldBuilderScreen.handleEditTeamProjectCB(argsList)

def WorldBuilderHandleEditTeamTechnologyCB( argsList ):
	worldBuilderScreen.handleEditTeamTechnologyCB(argsList)

def WorldBuilderHandleTechByEraPullDownCB( argsList ):
	worldBuilderScreen.handleTechByEraPullDownCB(argsList)

def WorldBuilderHandleRemoveTechByEraPullDownCB( argsList ):
	worldBuilderScreen.handleRemoveTechByEraPullDownCB(argsList)

def WorldBuilderHandleTeamEditNukeInterceptionCB( argsList ):
	worldBuilderScreen.handleTeamEditNukeInterceptionCB(argsList)

def WorldBuilderHandleDomainEditPullDownCB( argsList ):
	worldBuilderScreen.handleDomainEditPullDownCB(argsList)

def WorldBuilderHandleTeamEditDomainMovesCB( argsList ):
	worldBuilderScreen.handleTeamEditDomainMovesCB(argsList)

def WorldBuilderHandleRouteEditPullDownCB( argsList ):
	worldBuilderScreen.handleRouteEditPullDownCB(argsList)

def WorldBuilderHandleTeamEditRouteChangeCB( argsList ):
	worldBuilderScreen.handleTeamEditRouteChangeCB(argsList)

def WorldBuilderHandleImprovementEditPullDownCB( argsList ):
	worldBuilderScreen.handleImprovementEditPullDownCB(argsList)

def WorldBuilderHandleYieldEditPullDownCB( argsList ):
	worldBuilderScreen.handleYieldEditPullDownCB(argsList)

def WorldBuilderHandleTeamEditImprovementYieldCB( argsList ):
	worldBuilderScreen.handleTeamEditImprovementYieldCB(argsList)

def WorldBuilderHandleTeamEditMapCenteringCB( argsList ):
	worldBuilderScreen.handleTeamEditMapCenteringCB(argsList)

def WorldBuilderHandleTeamEditGoldTradingCB( argsList ):
	worldBuilderScreen.handleTeamEditGoldTradingCB(argsList)

def WorldBuilderHandleTeamEditTechTradingCB( argsList ):
	worldBuilderScreen.handleTeamEditTechTradingCB(argsList)

def WorldBuilderHandleTeamEditMapTradingCB( argsList ):
	worldBuilderScreen.handleTeamEditMapTradingCB(argsList)

def WorldBuilderHandleTeamEditOpenBordersTradingCB( argsList ):
	worldBuilderScreen.handleTeamEditOpenBordersTradingCB(argsList)

def WorldBuilderHandleTeamEditPermanentAllianceTradingCB( argsList ):
	worldBuilderScreen.handleTeamEditPermanentAllianceTradingCB(argsList)

def WorldBuilderHandleTeamEditDefensivePactTradingCB( argsList ):
	worldBuilderScreen.handleTeamEditDefensivePactTradingCB(argsList)

def WorldBuilderHandleTeamEditVassalTradingCB( argsList ):
	worldBuilderScreen.handleTeamEditVassalTradingCB(argsList)

def WorldBuilderHandleTeamEditWaterWorkCB( argsList ):
	worldBuilderScreen.handleTeamEditWaterWorkCB(argsList)

def WorldBuilderHandleTeamEditExtraWaterSeeFromCB( argsList ):
	worldBuilderScreen.handleTeamEditExtraWaterSeeFromCB(argsList)

def WorldBuilderHandleTeamEditBridgeBuildingCB( argsList ):
	worldBuilderScreen.handleTeamEditBridgeBuildingCB(argsList)

def WorldBuilderHandleTeamEditIrrigationCB( argsList ):
	worldBuilderScreen.handleTeamEditIrrigationCB(argsList)

def WorldBuilderHandleTeamEditIgnoreIrrigationCB( argsList ):
	worldBuilderScreen.handleTeamEditIgnoreIrrigationCB(argsList)

def WorldBuilderHandleTeamEditEnemyWarWearinessCB( argsList ):
	worldBuilderScreen.handleTeamEditEnemyWarWearinessCB(argsList)

def WorldBuilderHandleTeamEditKnownCivsCB( argsList ):
	worldBuilderScreen.handleTeamEditKnownCivsCB(argsList)

def WorldBuilderHandleTeamEditTechShareCB( argsList ):
	worldBuilderScreen.handleTeamEditTechShareCB(argsList)

def WorldBuilderHandleTeamEditForceTeamVoteCB( argsList ):
	worldBuilderScreen.handleTeamEditForceTeamVoteCB(argsList)

## Diplomacy ##

def WorldBuilderHandleCurrentPlayerEditPullDownCB( argsList ):
	worldBuilderScreen.handleCurrentPlayerEditPullDownCB(argsList)

def WorldBuilderHandleTeamEditMetStatusCB( argsList ):
	worldBuilderScreen.handleTeamEditMetStatusCB(argsList)

def WorldBuilderHandleMeetAllCB( argsList ):
	worldBuilderScreen.handleMeetAllCB(argsList)

def WorldBuilderHandleTeamEditWarStatusCB( argsList ):
	worldBuilderScreen.handleTeamEditWarStatusCB(argsList)

def WorldBuilderHandleTeamEditRelationshipCB( argsList ):
	worldBuilderScreen.handleTeamEditRelationshipCB(argsList)

def WorldBuilderHandleMemoryEditPullDownCB( argsList ):
	worldBuilderScreen.handleMemoryEditPullDownCB(argsList)

def WorldBuilderHandlePlayerEditMemoryCB( argsList ):
	worldBuilderScreen.handlePlayerEditMemoryCB(argsList)

def WorldBuilderHandleAttitudeEditPullDownCB( argsList ):
	worldBuilderScreen.handleAttitudeEditPullDownCB(argsList)

def WorldBuilderHandleTeamEditEspionagePointsCB( argsList ):
	worldBuilderScreen.handleTeamEditEspionagePointsCB(argsList)

def WorldBuilderHandleTeamEditCounterEspionageCB( argsList ):
	worldBuilderScreen.handleTeamEditCounterEspionageCB(argsList)

def WorldBuilderHandleTeamEditCounterEspionageModCB( argsList ):
	worldBuilderScreen.handleTeamEditCounterEspionageModCB(argsList)

def WorldBuilderHandleTeamEditWarWearinessCB( argsList ):
	worldBuilderScreen.handleTeamEditWarWearinessCB(argsList)

def WorldBuilderHandleTeamEditSignOpenBordersCB( argsList ):
	worldBuilderScreen.handleTeamEditSignOpenBordersCB(argsList)

def WorldBuilderHandleTeamEditSignDefensivePactCB( argsList ):
	worldBuilderScreen.handleTeamEditSignDefensivePactCB(argsList)

def WorldBuilderHandleDiplomacyPageCB( argsList ):
	worldBuilderScreen.handleDiplomacyPageCB(argsList)

def WorldBuilderHandleDiplomacyCommandsCB( argsList ):
	worldBuilderScreen.handleDiplomacyCommandsCB(argsList)

def WorldBuilderHandlePlayerEditEspionageWeightCB( argsList ):
	worldBuilderScreen.handlePlayerEditEspionageWeightCB(argsList)

def WorldBuilderHandleTowardsPlayerCB( argsList ):
	worldBuilderScreen.handleTowardsPlayerCB(argsList)

def WorldBuilderHandleTowardsOthersCB( argsList ):
	worldBuilderScreen.handleTowardsOthersCB(argsList)
## Plot ##

def WorldBuilderHandlePlotEditCultureCB( argsList ):
	worldBuilderScreen.handlePlotEditCultureCB(argsList)

def WorldBuilderHandlePlotEditYieldCB( argsList ):
	worldBuilderScreen.handlePlotEditYieldCB(argsList)

def WorldBuilderHandlePlotEditPlotTypeCB( argsList ):
	worldBuilderScreen.handlePlotEditPlotTypeCB(argsList)

def WorldBuilderHandlePlotEditTerrainCB( argsList ):
	worldBuilderScreen.handlePlotEditTerrainCB(argsList)

def WorldBuilderHandlePlotAddCityCB( argsList ):
	worldBuilderScreen.handlePlotAddCityCB(argsList)

def WorldBuilderHandlePlotEditFeatureCB( argsList ):
	worldBuilderScreen.handlePlotEditFeatureCB(argsList)

def WorldBuilderHandlePlotEditVarietyCB( argsList ):
	worldBuilderScreen.handlePlotEditVarietyCB(argsList)

def WorldBuilderHandlePlotEditBonusCB( argsList ):
	worldBuilderScreen.handlePlotEditBonusCB(argsList)

def WorldBuilderHandlePlotEditImprovementCB( argsList ):
	worldBuilderScreen.handlePlotEditImprovementCB(argsList)

def WorldBuilderHandlePlotEditUpgradeProgressCB( argsList ):
	worldBuilderScreen.handlePlotEditUpgradeProgressCB(argsList)

def WorldBuilderHandlePlotEditRouteCB( argsList ):
	worldBuilderScreen.handlePlotEditRouteCB(argsList)

def WorldBuilderHandlePlotEditRiverCB( argsList ):
	worldBuilderScreen.handlePlotEditRiverCB(argsList)

def WorldBuilderHandleEventOtherPlayerCB( argsList ):
	worldBuilderScreen.handleEventOtherPlayerCB(argsList)

def WorldBuilderHandleEventUnitCB( argsList ):
	worldBuilderScreen.handleEventUnitCB(argsList)

## Platy World Builder End ##

def WorldBuilderGetHighlightPlot(argsList):
	return worldBuilderScreen.getHighlightPlot(argsList)

def WorldBuilderOnAdvancedStartBrushSelected(argsList):
	iList,iIndex,iTab = argsList;
	print("WB Advanced Start brush selected, iList=%d, iIndex=%d, type=%d" %(iList,iIndex,iTab))
	if (iTab == worldBuilderScreen.m_iASTechTabID):
		showTechChooser()
	elif (iTab == worldBuilderScreen.m_iASCityTabID and iList == worldBuilderScreen.m_iASAutomateListID):
		CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_AUTOMATE, worldBuilderScreen.m_iCurrentPlayer, -1, -1, -1, True)

	if (worldBuilderScreen.setCurrentAdvancedStartIndex(iIndex)):
		if (worldBuilderScreen.setCurrentAdvancedStartList(iList)):
			return 1
	return 0

def WorldBuilderOnNormalPlayerBrushSelected(argsList):
	iList,iIndex,iTab = argsList;
	print("WB brush selected, iList=%d, iIndex=%d, type=%d" %(iList,iIndex,iTab))
	if (worldBuilderScreen.setCurrentNormalPlayerIndex(iIndex)):
		return 1
	return 0

def WorldBuilderOnNormalMapBrushSelected(argsList):
	iList,iIndex,iTab = argsList;
	print("WB brush selected, iList=%d, iIndex=%d, type=%d" %(iList,iIndex,iTab))
	if (worldBuilderScreen.setCurrentNormalMapIndex(iIndex)):
		if (worldBuilderScreen.setCurrentNormalMapList(iList)):
			return 1
	return 0

def WorldBuilderOnWBEditBrushSelected(argsList):
	iList,iIndex,iTab = argsList;
	if (worldBuilderScreen.setEditButtonClicked(iIndex)):
		return 1
	return 0

def WorldBuilderOnWBEditReligionSelected(argsList):
	iList,iIndex,iTab = argsList;
	if (worldBuilderScreen.setEditReligionSelected(iIndex)):
		return 1
	return 0

def WorldBuilderOnWBEditHolyCitySelected(argsList):
	iList,iIndex,iTab = argsList;
	if (worldBuilderScreen.setEditHolyCitySelected(iIndex)):
		return 1
	return 0

def WorldBuilderOnWBEditCorporationSelected(argsList):
	iList,iIndex,iTab = argsList;
	if (worldBuilderScreen.setEditCorporationSelected(iIndex)):
		return 1
	return 0

def WorldBuilderOnWBEditHeadquartersSelected(argsList):
	iList,iIndex,iTab = argsList;
	if (worldBuilderScreen.setEditHeadquartersSelected(iIndex)):
		return 1
	return 0

def WorldBuilderOnAllPlotsBrushSelected(argsList):
	if (worldBuilderScreen.handleAllPlotsCB(argsList)):
		return 1
	return 0

def WorldBuilderGetASUnitTabID():
	return worldBuilderScreen.getASUnitTabID()

def WorldBuilderGetASCityTabID():
	return worldBuilderScreen.getASCityTabID()

def WorldBuilderGetASCityListID():
	return worldBuilderScreen.getASCityListID()

def WorldBuilderGetASBuildingsListID():
	return worldBuilderScreen.getASBuildingsListID()

def WorldBuilderGetASAutomateListID():
	return worldBuilderScreen.getASAutomateListID()

def WorldBuilderGetASImprovementsTabID():
	return worldBuilderScreen.getASImprovementsTabID()

def WorldBuilderGetASRoutesListID():
	return worldBuilderScreen.getASRoutesListID()

def WorldBuilderGetASImprovementsListID():
	return worldBuilderScreen.getASImprovementsListID()

def WorldBuilderGetASVisibilityTabID():
	return worldBuilderScreen.getASVisibilityTabID()

def WorldBuilderGetASTechTabID():
	return worldBuilderScreen.getASTechTabID()

def WorldBuilderGetUnitTabID():
	return worldBuilderScreen.getUnitTabID()

def WorldBuilderGetBuildingTabID():
	return worldBuilderScreen.getBuildingTabID()

def WorldBuilderGetTechnologyTabID():
	return worldBuilderScreen.getTechnologyTabID()

def WorldBuilderGetImprovementTabID():
	return worldBuilderScreen.getImprovementTabID()

def WorldBuilderGetBonusTabID():
	return worldBuilderScreen.getBonusTabID()

def WorldBuilderGetImprovementListID():
	return worldBuilderScreen.getImprovementListID()

def WorldBuilderGetBonusListID():
	return worldBuilderScreen.getBonusListID()

def WorldBuilderGetTerrainTabID():
	return worldBuilderScreen.getTerrainTabID()

def WorldBuilderGetTerrainListID():
	return worldBuilderScreen.getTerrainListID()

def WorldBuilderGetFeatureListID():
	return worldBuilderScreen.getFeatureListID()

def WorldBuilderGetPlotTypeListID():
	return worldBuilderScreen.getPlotTypeListID()

def WorldBuilderGetRouteListID():
	return worldBuilderScreen.getRouteListID()

def WorldBuilderGetTerritoryTabID():
	return worldBuilderScreen.getTerritoryTabID()

def WorldBuilderGetTerritoryListID():
	return worldBuilderScreen.getTerritoryListID()

def WorldBuilderHasTech(argsList):
	iTech = argsList[0]
	return worldBuilderScreen.hasTech(iTech)

def WorldBuilderHasPromotion(argsList):
	iPromotion = argsList[0]
	return worldBuilderScreen.hasPromotion(iPromotion)

def WorldBuilderHasBuilding(argsList):
	iBuilding = argsList[0]
	return worldBuilderScreen.getNumBuilding(iBuilding)

######################
## Strategy Overlay ##
######################
import CvDotMapOverlayScreen
overlayScreen = CvDotMapOverlayScreen.CvDotMapOverlayScreen(STRATEGY_OVERLAY_SCREEN)

def showOverlayScreen():
	overlayScreen.interfaceScreen()

def hideOverlayScreen():
	overlayScreen.hideScreen()

######################
## Utility Functions #
######################

def movieDone(argsList):
	if argsList[0] == INTRO_MOVIE_SCREEN:
		screenMap[INTRO_MOVIE_SCREEN].hideScreen()
	elif argsList[0] == VICTORY_MOVIE_SCREEN:
		screenMap[VICTORY_MOVIE_SCREEN].hideScreen()

def leftMouseDown(argsList):
	if argsList[0] == WORLDBUILDER_SCREEN:
		worldBuilderScreen.leftMouseDown(argsList[1:])
		return 1
	return 0

def rightMouseDown(argsList):
	if argsList[0] == WORLDBUILDER_SCREEN:
		worldBuilderScreen.rightMouseDown(argsList)
		return 1
	return 0

def mouseOverPlot(argsList):
	if argsList[0] == STRATEGY_OVERLAY_SCREEN:
		overlayScreen.onMouseOverPlot(argsList)

	elif argsList[0] == WORLDBUILDER_SCREEN:
		worldBuilderScreen.mouseOverPlot(argsList)

def handleInput(argsList):
	inputClass = PyScreenInput.ScreenInput(argsList)
	iPythonFile = inputClass.ePythonFileEnum
	if iPythonFile == WORLDBUILDER_SCREEN:
		if CyInterface().isInAdvancedStart():
			return 0
	# get the screen that is active from the screenMap Dictionary
	if iPythonFile in screenMap:
		screen = screenMap[iPythonFile]
		# call handle input on that screen
		return screen.handleInput(inputClass)
	return 0

# Entry point for dll requests to display messages.
import CvUtil
def sendMessage(args):
	CvUtil.sendMessage(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11])

def update(argsList):
	if argsList[0] == STRATEGY_OVERLAY_SCREEN:
		overlayScreen.update(argsList)

	elif argsList[0] in screenMap:
		screen = screenMap[argsList[0]]
		screen.update(argsList[1])

def onClose(argsList):
	if argsList[0] in screenMap:
		screen = screenMap[argsList[0]]
		if hasattr(screen, "onClose") and isinstance(screen.onClose, types.MethodType):
			screen.onClose()

# Forced screen update
def forceScreenUpdate(argsList):
	if argsList[0] == TECH_CHOOSER:
		screenMap[TECH_CHOOSER].updateTechRecords(False)
	# Main interface Screen
	elif argsList[0] == MAIN_INTERFACE:
		mainInterface.updateScreen()
	# world builder Screen
	elif argsList[0] == WORLDBUILDER_SCREEN:
		worldBuilderScreen.updateScreen()

# Forced redraw
def forceScreenRedraw(argsList):
	# Main Interface Screen
	if argsList[0] == MAIN_INTERFACE:
		mainInterface.redraw()
	elif argsList[0] == TECH_CHOOSER:
		screenMap[TECH_CHOOSER].updateTechRecords(True)
	elif argsList[0] == ESPIONAGE_ADVISOR:
		screen = CyGInterfaceScreen("EspionageAdvisor", ESPIONAGE_ADVISOR)
		screenMap[ESPIONAGE_ADVISOR].redraw(screen)

def minimapClicked (argsList):
	if MILITARY_ADVISOR == argsList[0]:
		screenMap[MILITARY_ADVISOR].minimapClicked()


############################################################################
## Misc Functions
############################################################################

def handleBack(screens):
	if screens:
		for iScreen in screens:
			screen = screenMap.get(iScreen)
			if hasattr(screen, "back") and isinstance(screen.back, types.MethodType):
				screen.back()
	else:
		mainInterface.back()

def handleForward(screens):
	if screens:
		for iScreen in screens:
			screen = screenMap.get(iScreen)
			if hasattr(screen, "forward") and isinstance(screen.forward, types.MethodType):
				screen.forward()
	else:
		mainInterface.forward()

def refreshMilitaryAdvisor(argsList):
	if 1 == argsList[0]:
		screenMap[MILITARY_ADVISOR].refreshSelectedGroup(argsList[1])
	elif argsList[0] <= 0:
		screenMap[MILITARY_ADVISOR].refreshSelectedUnit(-argsList[0], argsList[1])

def updateMusicPath(argsList):
	szPathName = argsList[0]
	optionsScreen.updateMusicPath(szPathName)

def refreshOptionsScreen():
	optionsScreen.refreshScreen()

def cityWarningOnClickedCallback(argsList):
	iButtonId = argsList[0]
	iData1 = argsList[1]
	iData2 = argsList[2]
	iData3 = argsList[3]
	iData4 = argsList[4]
	szText = argsList[5]
	bOption1 = argsList[6]
	bOption2 = argsList[7]
	city = CyGlobalContext().getPlayer(CyGlobalContext().getGame().getActivePlayer()).getCity(iData1)
	if not city.isNone():
		if (iButtonId == 0):
			if (city.isProductionProcess()):
				CyMessageControl().sendPushOrder(iData1, iData2, iData3, False, False, False)
			else:
				CyMessageControl().sendPushOrder(iData1, iData2, iData3, False, True, False)
		elif (iButtonId == 2):
			CyInterface().selectCity(city, False)

def cityWarningOnFocusCallback(argsList):
	CyInterface().playGeneralSound("AS2D_ADVISOR_SUGGEST")
	CyInterface().lookAtCityOffset(argsList[0])
	return 0

def liberateOnClickedCallback(argsList):
	iButtonId = argsList[0]
	iData1 = argsList[1]
	iData2 = argsList[2]
	iData3 = argsList[3]
	iData4 = argsList[4]
	szText = argsList[5]
	bOption1 = argsList[6]
	bOption2 = argsList[7]
	city = CyGlobalContext().getPlayer(CyGlobalContext().getGame().getActivePlayer()).getCity(iData1)
	if not city.isNone():
		if iButtonId == 0:
			CyMessageControl().sendDoTask(iData1, TaskTypes.TASK_LIBERATE, 0, -1, False, False, False, False)
		elif iButtonId == 2:
			CyInterface().selectCity(city, False)

def colonyOnClickedCallback(argsList):
	iButtonId = argsList[0]
	iData1 = argsList[1]
	iData2 = argsList[2]
	iData3 = argsList[3]
	iData4 = argsList[4]
	szText = argsList[5]
	bOption1 = argsList[6]
	bOption2 = argsList[7]
	city = CyGlobalContext().getPlayer(CyGlobalContext().getGame().getActivePlayer()).getCity(iData1)
	if not city.isNone():
		if iButtonId == 0:
			CyMessageControl().sendEmpireSplit(CyGlobalContext().getGame().getActivePlayer(), city.area().getID())
		elif iButtonId == 2:
			CyInterface().selectCity(city, False)

def featAccomplishedOnClickedCallback(argsList):
	iButtonId = argsList[0]
	iData1 = argsList[1]
	iData2 = argsList[2]
	iData3 = argsList[3]
	iData4 = argsList[4]
	szText = argsList[5]
	bOption1 = argsList[6]
	bOption2 = argsList[7]

	if iButtonId == 1:
		if iData1 == FeatTypes.FEAT_TRADE_ROUTE:
			showDomesticAdvisor(())
		elif (iData1 >= FeatTypes.FEAT_UNITCOMBAT_ARCHER) and (iData1 <= FeatTypes.FEAT_UNIT_SPY):
			CyGlobalContext().getGame().doControl(ControlTypes.CONTROL_MILITARY_SCREEN)
		elif (iData1 >= FeatTypes.FEAT_COPPER_CONNECTED) and (iData1 <= FeatTypes.FEAT_FOOD_CONNECTED):
			showForeignAdvisorScreen([0])
		elif iData1 == FeatTypes.FEAT_NATIONAL_WONDER:
			# 2 is for the wonder tab...
			showInfoScreen([2, 0])
		elif (iData1 >= FeatTypes.FEAT_POPULATION_HALF_MILLION) and (iData1 <= FeatTypes.FEAT_POPULATION_2_BILLION):
			# 1 is for the demographics tab...
			showInfoScreen([1, 0])
		elif iData1 == FeatTypes.FEAT_CORPORATION_ENABLED:
			showCorporationScreen()

def featAccomplishedOnFocusCallback(argsList):
	iData1 = argsList[0]
	iData2 = argsList[1]
	iData3 = argsList[2]
	iData4 = argsList[3]
	szText = argsList[4]
	bOption1 = argsList[5]
	bOption2 = argsList[6]

	CyInterface().playGeneralSound("AS2D_FEAT_ACCOMPLISHED")
	if iData1 >= FeatTypes.FEAT_UNITCOMBAT_ARCHER and iData1 <= FeatTypes.FEAT_FOOD_CONNECTED:
		CyInterface().lookAtCityOffset(iData2)

#Ronnar: EventTriggerMenu START
def selectOneEvent(argsList):
	iButtonId	= argsList[0]
	iData1		= argsList[1]

	eventTriggerName = None
	eventTriggerNumber = -1

	if iButtonId < CyGlobalContext().getNumEventTriggerInfos():
		eventTriggerName = CyGlobalContext().getEventTriggerInfo(iButtonId).getType()
		eventTriggerNumber = iButtonId
	if eventTriggerName == None:
		return
	if eventTriggerNumber == -1:
		return
	message = 'Event: %s[%d]' % (eventTriggerName, eventTriggerNumber)
	CyInterface().addImmediateMessage(message, "")

	pPlayer = CyGlobalContext().getPlayer(iData1)
	pPlayer.trigger(eventTriggerNumber)

####################
# Handle Input Map #
####################
screenMap = {
	MAIN_INTERFACE			: mainInterface,
	OPTIONS_SCREEN			: optionsScreen,
	REPLAY_SCREEN			: replayScreen,
	BUILD_LIST_SCREEN		: buildListScreen,
	WORLDBUILDER_SCREEN		: worldBuilderScreen,
	# add new screens here
	STRATEGY_OVERLAY_SCREEN		: overlayScreen,
	REVOLUTION_WATCH_ADVISOR	: revolutionWatchAdvisor,
	C2C_INITIAL_OPTIONS_SCREEN	: c2cMainOptionsScreen,
	C2C_NATIONAL_ADVISOR_SCREEN	: c2cNationAdvisorScreen
}
##############
# Initialize #
##############
def lateInit():
	import CvCorporationScreen
	import CvEspionageAdvisor
	import CvMilitaryAdvisor
	import CvDomesticAdvisor
	import CvForeignAdvisor
	import CvFinanceAdvisor
	import CvReligionScreen
	import CvEraMovieScreen
	import CvVictoryScreen
	import CvCivicsScreen
	import CvInfoScreen
	import CvDawnOfMan
	import CvTopCivs
	import Forgetful
	import CvDebugInfoScreen
	import CvTechChooser
	screenMap[CORPORATION_SCREEN]	= CvCorporationScreen.CvCorporationScreen()
	screenMap[ESPIONAGE_ADVISOR]	= CvEspionageAdvisor.CvEspionageAdvisor()
	screenMap[MILITARY_ADVISOR]		= CvMilitaryAdvisor.CvMilitaryAdvisor(MILITARY_ADVISOR)
	screenMap[DOMESTIC_ADVISOR]		= CvDomesticAdvisor.CvDomesticAdvisor(DOMESTIC_ADVISOR)
	screenMap[FOREIGN_ADVISOR]		= CvForeignAdvisor.CvForeignAdvisor(FOREIGN_ADVISOR)
	screenMap[FINANCE_ADVISOR]		= CvFinanceAdvisor.CvFinanceAdvisor(FINANCE_ADVISOR)
	screenMap[RELIGION_SCREEN]		= CvReligionScreen.CvReligionScreen()
	screenMap[ERA_MOVIE_SCREEN]		= CvEraMovieScreen.CvEraMovieScreen()
	screenMap[VICTORY_SCREEN]		= CvVictoryScreen.CvVictoryScreen(VICTORY_SCREEN)
	screenMap[CIVICS_SCREEN]		= CvCivicsScreen.CvCivicsScreen(CIVICS_SCREEN)
	screenMap[INFO_SCREEN]			= CvInfoScreen.CvInfoScreen(INFO_SCREEN)
	screenMap[DAWN_OF_MAN]			= CvDawnOfMan.CvDawnOfMan()
	screenMap[TOP_CIVS]				= CvTopCivs.CvTopCivs(TOP_CIVS)
	screenMap[FORGETFUL_SCREEN]		= Forgetful.Forgetful()
	screenMap[DEBUG_INFO_SCREEN]	= CvDebugInfoScreen.CvDebugInfoScreen()
	screenMap[TECH_CHOOSER]			= CvTechChooser.CvTechChooser()

def earlyInit():

	import CvIntroMovieScreen
	import CvWonderMovieScreen
	import CvVictoryMovieScreen
	import CvHallOfFameScreen
	import CvDanQuayle
	import CvSpaceShipScreen
	import CvUnVictoryScreen
	global unVictoryScreen
	unVictoryScreen = CvUnVictoryScreen.CvUnVictoryScreen()

	screenMap[INTRO_MOVIE_SCREEN]	= CvIntroMovieScreen.CvIntroMovieScreen()
	screenMap[WONDER_MOVIE_SCREEN]	= CvWonderMovieScreen.CvWonderMovieScreen()
	screenMap[VICTORY_MOVIE_SCREEN]	= CvVictoryMovieScreen.CvVictoryMovieScreen()

	screenMap[HALL_OF_FAME]	= CvHallOfFameScreen.CvHallOfFameScreen(HALL_OF_FAME)

	screenMap[DAN_QUAYLE_SCREEN] = CvDanQuayle.CvDanQuayle()
	screenMap[SPACE_SHIP_SCREEN] = CvSpaceShipScreen.CvSpaceShipScreen()

	import Pedia
	screenMap[PEDIA] = Pedia.Pedia(PEDIA)

	# RevolutionDCM
	createRevolutionWatchAdvisor()

earlyInit()