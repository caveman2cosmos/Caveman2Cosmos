## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005

from CvPythonExtensions import *
from CvScreenEnums import *
import types
import CvMainInterface
import CvOptionsScreen
import CvReplayScreen
import ScreenInput as PyScreenInput

import BugCore
AdvisorOpt = BugCore.game.Advisors

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

mainInterface = CvMainInterface.CvMainInterface()
def showMainInterface():
	print "showMainInterface"
	mainInterface.interfaceScreen()

def reinitMainInterface():
	print "reinitMainInterface"
	global mainInterface
	mainInterface = CvMainInterface.CvMainInterface()
	mainInterface.interfaceScreen()

def numPlotListButtons(): return 0 # Called from exe

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

def showMilitaryAdvisor():
	if CyGame().getActivePlayer() != -1:
		screenMap[MILITARY_ADVISOR].interfaceScreen()

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
		screenMap[INFO_SCREEN].interfaceScreen(argsList[0], argsList[1])

def showDebugInfoScreen():
	screenMap[DEBUG_INFO_SCREEN].interfaceScreen()

def showDebugScreen():
	screenMap[DEBUG_SCREEN].interfaceScreen()

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

def showBuildListScreen():
	if CyGame().getActivePlayer() != -1:
		screenMap[BUILD_LIST_SCREEN].interfaceScreen()

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
	if argsList[0] > -1:
		screenMap[PEDIA].pediaJump(-2, "", argsList[0])
	else:
		screenMap[PEDIA].pediaJump(10, "UnitCombat", argsList[0] + 100000)

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
def showWorldBuilderScreen():
	print "showWorldBuilderScreen"
	if CyInterface().isInAdvancedStart():
		advancedStartScreen.interfaceScreen(ADVANCED_START_SCREEN)
	else: worldBuilderScreen.interfaceScreen()

def WorldBuilderExitCB():
	print "WorldBuilderExitCB"
	CyInterface().setWorldBuilder(False)

def hideWorldBuilderScreen():
	print "hideWorldBuilderScreen"
	if CyInterface().isInAdvancedStart():
		advancedStartScreen.killScreen()
	else:
		worldBuilderScreen.killScreen()
		toggleSetNoScreens()

def WorldBuilderToggleUnitEditCB():
	print "WorldBuilderToggleUnitEditCB"
	worldBuilderScreen.toggleUnitEditCB()

def WorldBuilderEraseCB():
	print "WorldBuilderEraseCB"
	worldBuilderScreen.eraseCB()

def WorldBuilderLandmarkCB():
	print "WorldBuilderLandmarkCB"
	worldBuilderScreen.landmarkModeCB()

def WorldBuilderToggleCityEditCB():
	print "WorldBuilderToggleCityEditCB"
	worldBuilderScreen.toggleCityEditCB()

def WorldBuilderNormalMapTabModeCB():
	print "WorldBuilderNormalMapTabModeCB"
	worldBuilderScreen.normalMapTabModeCB()

def WorldBuilderRevealTabModeCB():
	print "WorldBuilderRevealTabModeCB"
	worldBuilderScreen.revealTabModeCB()

def WorldBuilderDiplomacyModeCB():
	print "WorldBuilderDiplomacyModeCB"
	screenMap[WB_DIPLOMACY].interfaceScreen(CyGame().getActivePlayer(), False)

def WorldBuilderRevealAllCB():
	print "WorldBuilderRevealAllCB"
	worldBuilderScreen.revealAll(True)

def WorldBuilderUnRevealAllCB():
	print "WorldBuilderUnRevealAllCB"
	worldBuilderScreen.revealAll(False)

def WorldBuilderGetHighlightPlot(argsList):
	print "WorldBuilderGetHighlightPlot"
	if CyInterface().isInAdvancedStart():
		return advancedStartScreen.getHighlightPlot(argsList)
	else: return worldBuilderScreen.getHighlightPlot(argsList)

#----------------------------------------------------------------------------#
# Functions called by the exe in this order at Advanced start initialization #
#----------------------------------------------------------------------------#
def WorldBuilderGetASCityTabID():
	print "WorldBuilderGetASCityTabID"
	return advancedStartScreen.getCityTab()

def WorldBuilderGetASCityListID():
	print "WorldBuilderGetASCityListID"
	return advancedStartScreen.getCityRow()

def WorldBuilderGetASBuildingsListID():
	print "WorldBuilderGetASBuildingsListID"
	return advancedStartScreen.getBuildingsRow()

def WorldBuilderGetASAutomateListID():
	print "WorldBuilderGetASAutomateListID"
	return advancedStartScreen.getAutomationRow()

def WorldBuilderGetASUnitTabID():
	print "WorldBuilderGetASUnitTabID"
	return advancedStartScreen.getUnitTab()

def WorldBuilderGetASImprovementsTabID():
	print "WorldBuilderGetASImprovementsTabID"
	return advancedStartScreen.getImprovementTab()

def WorldBuilderGetASRoutesListID():
	print "WorldBuilderGetASRoutesListID"
	return advancedStartScreen.getRoutesRow()

def WorldBuilderGetASImprovementsListID():
	print "WorldBuilderGetASImprovementsListID"
	return advancedStartScreen.getImprovementsRow()

def WorldBuilderGetASVisibilityTabID():
	print "WorldBuilderGetASVisibilityTabID"
	return advancedStartScreen.getVisibilityTab()

def WorldBuilderGetASTechTabID():
	print "WorldBuilderGetASTechTabID"
	return advancedStartScreen.getTechTab()
#------------------------------------------------#
# Called by the exe for WB and AS initialization #
#------------------------------------------------#
def WorldBuilderNormalPlayerTabModeCB():
	print "WorldBuilderNormalPlayerTabModeCB"
	if CyInterface().isInAdvancedStart():
		getWBToolNormalMapTabCtrl().enable(False)
	else:
		worldBuilderScreen.normalPlayerTabModeCB()
#---------------------------------#
# Called by the exe for WB and AS #
#---------------------------------#
def WorldBuilderOnAdvancedStartBrushSelected(argsList):
	iList, iIndex, iTab = argsList
	print "WorldBuilderOnAdvancedStartBrushSelected, iList=%d, iIndex=%d, type=%d" %(iList, iIndex, iTab)
	if iTab == advancedStartScreen.getTechTab():
		showTechChooser()
	elif iTab == advancedStartScreen.getCityTab() and iList == advancedStartScreen.getAutomationRow():
		CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_AUTOMATE, advancedStartScreen.iPlayer, -1, -1, -1, True)

	advancedStartScreen.setCurrentSelection(iTab, iIndex, iList)


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
	elif argsList[0] == ADVANCED_START_SCREEN:
		advancedStartScreen.leftMouseDown(argsList[1:])
		return 1
	return 0

def rightMouseDown(argsList):

	if argsList[0] == WORLDBUILDER_SCREEN:
		worldBuilderScreen.rightMouseDown()
		return 1
	elif argsList[0] == ADVANCED_START_SCREEN:
		advancedStartScreen.rightMouseDown()
		return 1
	return 0

def mouseOverPlot(argsList):

	if argsList[0] == STRATEGY_OVERLAY_SCREEN:
		overlayScreen.onMouseOverPlot()

	elif argsList[0] == WORLDBUILDER_SCREEN:
		worldBuilderScreen.mouseOverPlot()

	elif argsList[0] == ADVANCED_START_SCREEN:
		advancedStartScreen.mouseOverPlot()

def handleInput(argsList):
	inputClass = PyScreenInput.ScreenInput(argsList)
	iPythonFile = inputClass.ePythonFileEnum
	# get the screen that is active from the screenMap Dictionary
	if iPythonFile in screenMap:
		# call handle input on that screen
		return screenMap[iPythonFile].handleInput(inputClass)
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

# Forced screen update (250 ms)
def forceScreenUpdate(argsList):
	if argsList[0] == TECH_CHOOSER:
		screenMap[TECH_CHOOSER].updateTechRecords(False)

	elif argsList[0] == MAIN_INTERFACE:
		mainInterface.updateScreen()

	elif argsList[0] == WORLDBUILDER_SCREEN:
		worldBuilderScreen.updateScreen()

	elif argsList[0] == ADVANCED_START_SCREEN:
		advancedStartScreen.updateScreen()

# Forced redraw (~7 ms)
def forceScreenRedraw(argsList):
	if argsList[0] == MAIN_INTERFACE:
		mainInterface.redraw()

	elif argsList[0] == TECH_CHOOSER:
		screenMap[TECH_CHOOSER].updateTechRecords(True)

	elif argsList[0] == ESPIONAGE_ADVISOR:
		screenMap[ESPIONAGE_ADVISOR].redraw(CyGInterfaceScreen("EspionageAdvisor", ESPIONAGE_ADVISOR))

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
	if city:
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
	if city:
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
	if city:
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

####################
# Handle Input Map #
####################
screenMap = {
	MAIN_INTERFACE			: mainInterface,
	OPTIONS_SCREEN			: optionsScreen,
	REPLAY_SCREEN			: replayScreen,
	STRATEGY_OVERLAY_SCREEN		: overlayScreen,
	REVOLUTION_WATCH_ADVISOR	: revolutionWatchAdvisor
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
	import CvTechChooser
	import BuildListScreen
	import CvDebugInfoScreen
	import DebugScreen
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
	screenMap[TECH_CHOOSER]			= CvTechChooser.CvTechChooser()
	screenMap[BUILD_LIST_SCREEN]	= BuildListScreen.BuildListScreen()
	screenMap[DEBUG_INFO_SCREEN]	= CvDebugInfoScreen.CvDebugInfoScreen()
	screenMap[DEBUG_SCREEN]			= DebugScreen.DebugScreen(DEBUG_SCREEN)

	import WorldBuilder, CvAdvancedStartScreen
	global worldBuilderScreen, advancedStartScreen
	advancedStartScreen = CvAdvancedStartScreen.CvAdvancedStartScreen()
	worldBuilderScreen = WorldBuilder.WorldBuilder(WORLDBUILDER_SCREEN)
	screenMap[WORLDBUILDER_SCREEN] = worldBuilderScreen
	import WBPlotScreen
	import WBEventScreen
	import WBBuildingScreen
	import WBCityDataScreen
	import WBCityEditScreen
	import WBProjectScreen
	import WBTeamScreen
	import WBPlayerScreen
	import WBUnitScreen
	import WBPromotionScreen
	import WBDiplomacyScreen
	import WBPlayerUnits
	import WBReligionScreen
	import WBCorporationScreen
	import WBInfoScreen
	import WBTradeScreen
	screenMap[WB_PLOT]			= WBPlotScreen.WBPlotScreen(worldBuilderScreen)
	screenMap[WB_EVENT]			= WBEventScreen.WBEventScreen(worldBuilderScreen)
	screenMap[WB_BUILDING]		= WBBuildingScreen.WBBuildingScreen(worldBuilderScreen)
	screenMap[WB_CITYDATA]		= WBCityDataScreen.WBCityDataScreen(worldBuilderScreen)
	screenMap[WB_CITYEDIT]		= WBCityEditScreen.WBCityEditScreen(worldBuilderScreen)
	screenMap[WB_PROJECT]		= WBProjectScreen.WBProjectScreen(worldBuilderScreen)
	screenMap[WB_TEAM]			= WBTeamScreen.WBTeamScreen(worldBuilderScreen)
	screenMap[WB_PLAYER]		= WBPlayerScreen.WBPlayerScreen(worldBuilderScreen)
	screenMap[WB_UNIT]			= WBUnitScreen.WBUnitScreen(worldBuilderScreen)
	screenMap[WB_PROMOTION]		= WBPromotionScreen.WBPromotionScreen(worldBuilderScreen)
	screenMap[WB_DIPLOMACY]		= WBDiplomacyScreen.WBDiplomacyScreen(worldBuilderScreen)
	screenMap[WB_UNITLIST]		= WBPlayerUnits.WBPlayerUnits(worldBuilderScreen)
	screenMap[WB_RELIGION]		= WBReligionScreen.WBReligionScreen(worldBuilderScreen)
	screenMap[WB_CORPORATION]	= WBCorporationScreen.WBCorporationScreen(worldBuilderScreen)
	screenMap[WB_INFO]			= WBInfoScreen.WBInfoScreen(worldBuilderScreen)
	screenMap[WB_TRADE]			= WBTradeScreen.WBTradeScreen(worldBuilderScreen)



def earlyInit():
	import CvIntroMovieScreen
	import CvWonderMovieScreen
	import CvVictoryMovieScreen
	import CvHallOfFameScreen
	import CvDanQuayle
	import CvSpaceShipScreen

	screenMap[INTRO_MOVIE_SCREEN]	= CvIntroMovieScreen.CvIntroMovieScreen()
	screenMap[WONDER_MOVIE_SCREEN]	= CvWonderMovieScreen.CvWonderMovieScreen()
	screenMap[VICTORY_MOVIE_SCREEN]	= CvVictoryMovieScreen.CvVictoryMovieScreen()

	screenMap[HALL_OF_FAME]	= CvHallOfFameScreen.CvHallOfFameScreen(HALL_OF_FAME)

	screenMap[DAN_QUAYLE_SCREEN] = CvDanQuayle.CvDanQuayle()
	screenMap[SPACE_SHIP_SCREEN] = CvSpaceShipScreen.CvSpaceShipScreen()

	import Pedia
	screenMap[PEDIA] = Pedia.Pedia(PEDIA)

	import CvUnVictoryScreen
	global unVictoryScreen
	unVictoryScreen = CvUnVictoryScreen.CvUnVictoryScreen()
	# RevolutionDCM
	createRevolutionWatchAdvisor()

earlyInit()