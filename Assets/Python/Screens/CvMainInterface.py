from CvPythonExtensions import *
from operator import itemgetter

import CvScreenEnums
import CvScreensInterface as UP
import HandleInputUtil
import PythonToolTip as pyTT
import AbandonCityEventManager as ACEM
import TextUtil
import RevInstances
# globals
GC = CyGlobalContext()
ENGINE = CyEngine()
TRNSLTR = CyTranslator()
GAME = GC.getGame()
CyIF = CyInterface()
CyGTM = CyGameTextMgr()

import Scoreboard
import ReminderEventManager # Reminders
import GPUtil # Great Person Bar
import RawYields # Raw Yields
import StackBarUtil

CITYTAB_ADMIN = 0
CITYTAB_UNIT = 1
CITYTAB_BUILDING = 2
CITYTAB_WONDER = 3

g_mainInterface = None

def onSwitchHotSeatPlayer(argsList):
	g_mainInterface.resetEndTurnObjects()


class CvMainInterface:

	def __init__(self):
		global g_mainInterface
		g_mainInterface = self

		global MainOpt, CityOpt, ClockOpt, ScoreOpt, RoMOpt
		import BugCore
		ClockOpt = BugCore.game.NJAGC
		ScoreOpt = BugCore.game.Scores
		MainOpt = self.MainOpt = BugCore.game.MainInterface
		CityOpt = self.CityOpt = BugCore.game.CityScreen
		RoMOpt = BugCore.game.RoMSettings

		import InputData
		self.InputData = InputData.instance

		self.bInitialize = True
		self.bSetStartZoom = True
		self.xRes = 0
		self.yRes = 0

		self.yTradeRouteTable = 160
		self.hTradeRouteTable = 122
		self.iShowBuildings = 1
		self.iShowBonus = 0
		self.iCityTab = -1
		self.techIconSB = []

		self.bMinInterface = False
		self.AtUnit = None
		self.InCity = None
		self.bCityScreen = False
		self.aPlotListPlot = None
		self.xScoreboard = 0
		self.iField_View = 50
		self.bCityChange = False
		self.bCityEnter = False
		self.bCityExit = False
		self.bInverseShiftQueue = False
		self.bGlobeview = False
		self.bUpdateCityTab = False
		self.bBuildWorkQueue = False
		self.NewQueueRowTime = []
		# Help Text
		self.bHelpTextFullY = False
		self.bTooltip = False
		self.bLockPlotHelp = False
		self.plotHelpUnitID = -1
		self.dataTT = []
		self.bPlotHelpBan = False
		self.xMouseNoPlotHelp = -1
		self.yMouseNoPlotHelp = -1
		self.bLockedTT = False
		self.iOffsetTT = []
		self.szHelpText = ""
		self.szPlotHelp = ""

		self.iField_View_Prev = -1
		self.iWaitingCounter = 0
		self.bShowTimeTextAlt = False
		self.iTimeTextCounter = 0
		self.cityOptions = None

	def pythonDebugToggle(self, bNewValue):
		self.bDebugModePython = bNewValue
		if bNewValue:
			CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE).show("DebugBtn0")
		else: CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE).hide("DebugBtn0")

	def interfaceScreen(self):
		if GAME.isPitbossHost(): return
		# Cache Game Status
		self.bNetworkMP		= GAME.isNetworkMultiPlayer()
		self.bMultiPlayer	= GAME.isGameMultiPlayer()
		# Cache Game Options
		self.GO_REVOLUTION			= GAME.isOption(GameOptionTypes.GAMEOPTION_UNSUPPORTED_REVOLUTION)
		self.GO_PICK_RELIGION		= GAME.isOption(GameOptionTypes.GAMEOPTION_RELIGION_PICK)
		self.GO_SIZE_MATTERS		= GAME.isOption(GameOptionTypes.GAMEOPTION_COMBAT_SIZE_MATTERS)
		self.GO_WIN_FOR_LOSING		= GAME.isOption(GameOptionTypes.GAMEOPTION_TECH_WIN_FOR_LOSING)
		self.GO_TECH_DIFFUSION   	= GAME.isOption(GameOptionTypes.GAMEOPTION_TECH_DIFFUSION)
		self.GO_ONE_CITY_CHALLENGE	= GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY)
		# First pass initialization.
		if self.bInitialize:
			import DebugUtils
			self.bDebugModePython = DebugUtils.bDebugMode
			self.bDebugMode = GAME.isDebugMode()
			# FOV
			if MainOpt.isRememberFieldOfView():
				self.iField_View = int(MainOpt.getFieldOfView())
			# Raw Yields
			self.bYieldView, self.iYieldType = RawYields.getViewAndType(CityOpt.getRawYieldsDefaultView())
			self.iYieldTiles = RawYields.WORKED_TILES
			self.RAW_YIELD_HELP = (
				"TXT_KEY_RAW_YIELD_VIEW_TRADE",
				"TXT_KEY_RAW_YIELD_VIEW_FOOD",
				"TXT_KEY_RAW_YIELD_VIEW_PRODUCTION",
				"TXT_KEY_RAW_YIELD_VIEW_COMMERCE",
				"TXT_KEY_RAW_YIELD_TILES_WORKED",
				"TXT_KEY_RAW_YIELD_TILES_CITY",
				"TXT_KEY_RAW_YIELD_TILES_OWNED",
				"TXT_KEY_RAW_YIELD_TILES_ALL")
			# Cache Constants.
			self.iPlotListMaxRows		= GC.getMAX_PLOT_LIST_ROWS()
			self.iMaxPlayers			= GC.getMAX_PLAYERS()
			self.iMaxPcTeams			= GC.getMAX_PC_TEAMS()
			self.iMaxPcPlayers			= GC.getMAX_PC_PLAYERS()
			self.iMaxDefenseDamage		= GC.getMAX_CITY_DEFENSE_DAMAGE()
			self.fMoveDenominator = float(GC.getMOVE_DENOMINATOR())
			self.bNegGoldIsMaintenance	= GC.getDefineINT("TREAT_NEGATIVE_GOLD_AS_MAINTENANCE")
			self.iNumReligionInfos		= GC.getNumReligionInfos()
			self.iNumCorporationInfos	= GC.getNumCorporationInfos()
			self.iNumBonusInfos			= GC.getNumBonusInfos()
			self.iNumPromotionInfos		= GC.getNumPromotionInfos()
			self.iNumEmphasizeInfos 	= GC.getNumEmphasizeInfos()
			self.iNumHurryInfos			= GC.getNumHurryInfos()
			self.iNumProjectInfos		= GC.getNumProjectInfos()
			self.iNumProcessInfos		= GC.getNumProcessInfos()
			self.iNumSpecialistInfos	= iNumSpecialistInfos = GC.getNumSpecialistInfos()
			self.aSpecialistIconList = aSpecialistIconList = []
			for i in xrange(iNumSpecialistInfos):
				aSpecialistIconList.append(GC.getSpecialistInfo(i).getTexture())
			'''
			FONT_CENTER_JUSTIFY	= 1<<2
			FONT_RIGHT_JUSTIFY	= 1<<1
			FONT_LEFT_JUSTIFY	= 1<<0
			# Toffer note: setTextAt(..., 1<<1, ...) doesn't work, it will always be left justified.
			#			Use setText() instead, if interaction is not needed then setLabel() and setLabelAt() handle text justification.
			'''
			# Cache Icons
			self.iconStrength			= u'%c' % GAME.getSymbolID(FontSymbols.STRENGTH_CHAR)
			self.iconMoves				= u'%c' % GAME.getSymbolID(FontSymbols.MOVES_CHAR)
			self.iconStarGold			= u'%c' % GAME.getSymbolID(FontSymbols.STAR_CHAR)
			self.iconStarSilver			= u'%c' % GAME.getSymbolID(FontSymbols.SILVER_STAR_CHAR)
			self.iconPower				= u'%c' % GAME.getSymbolID(FontSymbols.POWER_CHAR)
			self.iconOccupation			= u'%c' % GAME.getSymbolID(FontSymbols.OCCUPATION_CHAR)
			self.iconFoodEaten			= u'%c' % GAME.getSymbolID(FontSymbols.EATEN_FOOD_CHAR)
			self.iconAngryPop			= u'%c' % GAME.getSymbolID(FontSymbols.ANGRY_POP_CHAR)
			#self.iconTrade				= u'%c' % GAME.getSymbolID(FontSymbols.TRADE_CHAR)
			#self.iconOpenBorders		= u'%c' % GAME.getSymbolID(FontSymbols.OPEN_BORDERS_CHAR)
			#self.iconMap				= u'%c' % GAME.getSymbolID(FontSymbols.MAP_CHAR)
			#self.iconDefensivePact		= u'%c' % GAME.getSymbolID(FontSymbols.DEFENSIVE_PACT_CHAR)
			self.iconHappy				= u'%c' % GAME.getSymbolID(FontSymbols.HAPPY_CHAR)
			self.iconUnhappy			= u'%c' % GAME.getSymbolID(FontSymbols.UNHAPPY_CHAR)
			self.iconHealthy			= u'%c' % GAME.getSymbolID(FontSymbols.HEALTHY_CHAR)
			self.iconUnhealthy			= u'%c' % GAME.getSymbolID(FontSymbols.UNHEALTHY_CHAR)
			self.iconCitizen			= u'%c' % GAME.getSymbolID(FontSymbols.CITIZEN_CHAR)
			self.iconGreatGeneral		= u'%c' % GAME.getSymbolID(FontSymbols.GREAT_GENERAL_CHAR)
			self.iconGreatPeople		= u'%c' % GAME.getSymbolID(FontSymbols.GREAT_PEOPLE_CHAR)
			self.iconDefense			= u'%c' % GAME.getSymbolID(FontSymbols.DEFENSE_CHAR)
			# Yield icons
			aList1 = []
			for i in xrange(YieldTypes.NUM_YIELD_TYPES):
				aList1.append(u'%c' % GC.getYieldInfo(i).getChar())
			self.iconYieldList = list(aList1)
			# Commerce icons
			aList1 = []
			for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				aList1.append(u'%c' % GC.getCommerceInfo(i).getChar())
			self.iconCommerceList = list(aList1)
			# Religion icons
			aList1 = []
			aList2 = []
			aMap = {}
			for i in xrange(self.iNumReligionInfos):
				CvReligionInfo = GC.getReligionInfo(i)
				aList1.append(u'%c' % CvReligionInfo.getHolyCityChar())
				aList2.append(u'%c' % CvReligionInfo.getChar())
				# Map Religions to corresponding techs
				aMap[CvReligionInfo.getTechPrereq()] = CvReligionInfo
			self.aRel2TechMap = aMap.copy()
			aMap = {}
			self.iconHolyCityList = list(aList1)
			self.iconReligionList = list(aList2)
			# Cache file paths.
			afm = CyArtFileMgr()
			#self.artPathQuMark					= afm.getInterfaceArtInfo("QUESTION_MARK").getPath()
			self.artPathPromotion				= afm.getInterfaceArtInfo("OVERLAY_PROMOTION_FRAME").getPath()
			self.artPathHilite					= afm.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath()
			self.artPathUpgrade					= afm.getInterfaceArtInfo("OVERLAY_UPGRADE").getPath()
			self.artPathButtonNoProduction		= afm.getInterfaceArtInfo("INTERFACE_BUTTONS_NOPRODUCTION").getPath()
			self.artPathButtonGroupCreate		= afm.getInterfaceArtInfo("INTERFACE_BUTTONS_CREATEGROUP").getPath()
			self.artPathButtonGroupSplit		= afm.getInterfaceArtInfo("INTERFACE_BUTTONS_SPLITGROUP").getPath()
			self.artPathButtonMilitary			= afm.getInterfaceArtInfo("INTERFACE_BTN_MILITARY").getPath()
			self.artPathAngryCitizen			= afm.getInterfaceArtInfo("INTERFACE_ANGRYCITIZEN_TEXTURE").getPath()
			self.artPathResistance				= afm.getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath()
			self.artPathTechWorkerSpeed			= afm.getInterfaceArtInfo("INTERFACE_TECH_WORKER_SPEED").getPath()
			self.artPathTechMoveBonus			= afm.getInterfaceArtInfo("INTERFACE_TECH_MOVE_BONUS").getPath()
			self.artPathFilterCulture			= afm.getInterfaceArtInfo("INTERFACE_FILTER_CULTURE").getPath()
			self.artPathFilterEspionage			= afm.getInterfaceArtInfo("INTERFACE_FILTER_ESPIONAGE").getPath()
			self.artPathFilterResearch			= afm.getInterfaceArtInfo("INTERFACE_FILTER_RESEARCH").getPath()
			self.artPathOverlayHolyCity			= afm.getInterfaceArtInfo("INTERFACE_HOLYCITY_OVERLAY").getPath()
			self.artPathOverlayIntercept		= afm.getInterfaceArtInfo("OVERLAY_ACTION_INTERCEPT").getPath()
			self.artPathOverlayPatrol			= afm.getInterfaceArtInfo("OVERLAY_ACTION_PATROL").getPath()
			self.artPathOverlayPlunder			= afm.getInterfaceArtInfo("OVERLAY_ACTION_PLUNDER").getPath()
			self.artPathOverlayHeal				= afm.getInterfaceArtInfo("OVERLAY_ACTION_HEAL").getPath()
			self.artPathOverlaySentry			= afm.getInterfaceArtInfo("OVERLAY_ACTION_SENTRY").getPath()
			self.artPathOverlaySkip				= afm.getInterfaceArtInfo("OVERLAY_ACTION_SKIP").getPath()
			self.artPathOverlayExplore			= afm.getInterfaceArtInfo("OVERLAY_ACTION_EXPLORE").getPath()
			self.artPathOverlayAutoBuild		= afm.getInterfaceArtInfo("OVERLAY_ACTION_AUTO_BUILD").getPath()
			self.artPathOverlayAutoCity			= afm.getInterfaceArtInfo("OVERLAY_ACTION_AUTO_CITY").getPath()
			self.artPathOverlayAutoNetwork		= afm.getInterfaceArtInfo("OVERLAY_ACTION_AUTO_NETWORK").getPath()
			self.artPathOverlayAutoReligion		= afm.getInterfaceArtInfo("OVERLAY_ACTION_AUTO_RELIGION").getPath()
			self.artPathOverlayBuild			= afm.getInterfaceArtInfo("OVERLAY_ACTION_BUILD").getPath()
			self.artPathOverlayGoTo				= afm.getInterfaceArtInfo("OVERLAY_ACTION_GOTO").getPath()
			self.artPathOverlayActionFortify	= afm.getInterfaceArtInfo("OVERLAY_ACTION_FORTIFY").getPath()
			self.artPathOverlaySleep			= afm.getInterfaceArtInfo("OVERLAY_ACTION_SLEEP").getPath()
			self.artPathRawYieldsFood			= afm.getInterfaceArtInfo("RAW_YIELDS_FOOD").getPath()
			self.artPathRawYieldsProduction		= afm.getInterfaceArtInfo("RAW_YIELDS_PRODUCTION").getPath()
			self.artPathRawYieldsCommerce		= afm.getInterfaceArtInfo("RAW_YIELDS_COMMERCE").getPath()
			self.artPathFilterLandPLE			= afm.getInterfaceArtInfo("PLE_FILTER_LAND").getPath()
			self.artPathFilterAirPLE			= afm.getInterfaceArtInfo("PLE_FILTER_AIR").getPath()
			self.artPathFilterSeaPLE			= afm.getInterfaceArtInfo("PLE_FILTER_SEA").getPath()
			self.artPathFilterCivilianPLE		= afm.getInterfaceArtInfo("PLE_FILTER_DOM").getPath()
			self.artPathFilterSiegePLE			= afm.getInterfaceArtInfo("PLE_GRP_UNITTYPE").getPath()
			self.artPathFilterHeroPLE			= afm.getInterfaceArtInfo("PLE_FILTER_MIL").getPath()

			self.artFILTER_SHOW_UNLIMITED		= afm.getInterfaceArtInfo("ART_DEF_FILTER_SHOW_UNLIMITED").getPath()
			self.artFILTER_SHOW_MILITARY		= afm.getInterfaceArtInfo("ART_DEF_FILTER_SHOW_MILITARY").getPath()
			self.artFILTER_SHOW_HAPPY			= afm.getInterfaceArtInfo("ART_DEF_FILTER_SHOW_HAPPY").getPath()
			self.artFILTER_HIDE_UNHAPPY			= afm.getInterfaceArtInfo("ART_DEF_FILTER_HIDE_UNHAPPY").getPath()
			self.artFILTER_SHOW_HEALTH			= afm.getInterfaceArtInfo("ART_DEF_FILTER_SHOW_HEALTH").getPath()
			self.artFILTER_HIDE_UNHEALTH		= afm.getInterfaceArtInfo("ART_DEF_FILTER_HIDE_UNHEALTH").getPath()

			self.artPathListUnitStatusDot = [
				(afm.getInterfaceArtInfo("OVERLAY_FORTIFY").getPath(), afm.getInterfaceArtInfo("OVERLAY_FORTIFY_INJURED").getPath()),
				(afm.getInterfaceArtInfo("OVERLAY_FORTIFY_GG").getPath(), afm.getInterfaceArtInfo("OVERLAY_FORTIFY_INJURED_GG").getPath()),
				(afm.getInterfaceArtInfo("OVERLAY_HASMOVED").getPath(), afm.getInterfaceArtInfo("OVERLAY_HASMOVED_INJURED").getPath()),
				(afm.getInterfaceArtInfo("OVERLAY_HASMOVED_GG").getPath(), afm.getInterfaceArtInfo("OVERLAY_HASMOVED_INJURED_GG").getPath()),
				(afm.getInterfaceArtInfo("OVERLAY_MOVE").getPath(), afm.getInterfaceArtInfo("OVERLAY_MOVE_INJURED").getPath()),
				(afm.getInterfaceArtInfo("OVERLAY_MOVE_GG").getPath(), afm.getInterfaceArtInfo("OVERLAY_MOVE_INJURED_GG").getPath()),
				(afm.getInterfaceArtInfo("OVERLAY_NOMOVE").getPath(), afm.getInterfaceArtInfo("OVERLAY_NOMOVE_INJURED").getPath()),
				(afm.getInterfaceArtInfo("OVERLAY_NOMOVE_GG").getPath(), afm.getInterfaceArtInfo("OVERLAY_NOMOVE_INJURED_GG").getPath())]
			# Cache Info Types
			self.COLOR_RED			= GC.getInfoTypeForString('COLOR_RED')
			self.COLOR_YELLOW		= GC.getInfoTypeForString('COLOR_YELLOW')
			self.COLOR_GREEN		= GC.getInfoTypeForString('COLOR_GREEN')
			self.HURRY_POPULATION	= GC.getInfoTypeForString('HURRY_POPULATION')
			self.HURRY_GOLD			= GC.getInfoTypeForString('HURRY_GOLD')
			self.PROMOTION_LEADER	= GC.getInfoTypeForString('PROMOTION_LEADER')
			# Cache Text
			obj = TRNSLTR
			self.szSystemReturn				= obj.getText("TXT_KEY_SYSTEM_RETURN", ())
			self.szSystemWaiting			= obj.getText("TXT_KEY_SYSTEM_WAITING", ())
			self.szSystemWaitingForYou		= obj.getText("TXT_KEY_SYSTEM_WAITING_FOR_YOU", ())
			self.szInterfaceCityGrowth		= obj.getText("INTERFACE_CITY_GROWTH", ())
			self.szInterfaceCityStagnant	= obj.getText("INTERFACE_CITY_STAGNANT", ())
			self.szInterfaceCityStarving	= obj.getText("INTERFACE_CITY_STARVING", ())
			self.szInterfacePaneStrengthAir	= obj.getText("INTERFACE_PANE_AIR_STRENGTH", ())
			self.szInterfacePaneStrength	= obj.getText("INTERFACE_PANE_STRENGTH", ())
			self.szInterfacePaneMovement	= obj.getText("INTERFACE_PANE_MOVEMENT", ())
			self.szInterfacePaneLevel		= obj.getText("INTERFACE_PANE_LEVEL", ())
			self.szInterfacePaneExperience	= obj.getText("INTERFACE_PANE_EXPERIENCE", ())
			self.szDraft					= obj.getText("TXT_KEY_DRAFT", ())
			self.szMainCityGreatWonders		= obj.getText("TXT_KEY_MAIN_CITY_GREAT_WONDERS",())
			self.szMainCityNationalWonders	= obj.getText("TXT_KEY_MAIN_CITY_NATIONAL_WONDERS",())
			self.szHeaderListTradeRoute		= obj.getText("TXT_KEY_HEADING_TRADEROUTE_LIST", ())
			self.szHeaderRevStatus			= obj.getText("TXT_KEY_REV_STATUS_HEADER",())
			self.szRevWatchInfo				= obj.getText("TXT_KEY_REV_WATCH_INFO_PANE", ())
			self.szRevWatchDanger			= obj.getText("TXT_KEY_REV_WATCH_DANGER", ())
			self.szRevWatchSafe				= obj.getText("TXT_KEY_REV_WATCH_SAFE", ())
			self.szRevWatchWarning			= obj.getText("TXT_KEY_REV_WATCH_WARNING", ())
			self.szFieldOfView				= obj.getText("TXT_KEY_BUG_OPT_MAININTERFACE__FIELDOFVIEW_TEXT", ())
			self.szDeadCiv					= obj.getText("TXT_KEY_BUG_DEAD_CIV", ())
			self.szMinorCiv					= obj.getText("TXT_KEY_MINOR_CIV_DISPLAY", ())
			self.advisorButtonTip = [
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_DOMESTIC", ()) ,		"F1"),
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_FINANCE", ()),			"F2"),
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_CIVICS", ()),			"F3"),
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_FOREIGN", ()),			"F4"),
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_MILITARY", ()),		"F5"),
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_TECHNOLOGY", ()),		"F6"),
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_RELIGIOUS", ()),		"F7"),
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_CORPORATE", ()),		"Shift+F7"),
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_INTELLIGENCE", ()),	"Ctrl+E"),
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_PARTISAN", ()),		"Ctrl+Shift+G"),
				(obj.getText("TXT_KEY_HUD_BUTTON_ADVISOR_HERITAGE", ()),		"")
			]
			# Building infos:
			aBuildingList0 = []
			aBuildingList1 = []
			aBuildingList2 = []
			iNumBuildingInfos = GC.getNumBuildingInfos()
			for iBuilding in xrange(iNumBuildingInfos):
				CvBuildingInfo = GC.getBuildingInfo(iBuilding)
				szName = CvBuildingInfo.getDescription()
				if CvBuildingInfo.getProductionCost() < 1:
					aBuildingList2.append((szName, iBuilding, CvBuildingInfo))
				elif isLimitedWonder(iBuilding):
					aBuildingList1.append((szName, iBuilding, CvBuildingInfo))
				else:
					aBuildingList0.append((szName, iBuilding, CvBuildingInfo))
			# Bonus infos:
			aBonusList1 = []
			aBonusList2 = []
			aBonusList3 = []
			BONUSCLASS_CULTURE = GC.getInfoTypeForString("BONUSCLASS_CULTURE")
			for iBonus in xrange(self.iNumBonusInfos):
				CvBonusInfo = GC.getBonusInfo(iBonus)
				szName = CvBonusInfo.getDescription()
				szChar = u"%c " %CvBonusInfo.getChar()
				if CvBonusInfo.isMapBonus(): ## Map resource
					aBonusList1.append((szName, iBonus, szChar))
				elif CvBonusInfo.getBonusClassType() != BONUSCLASS_CULTURE:
					aBonusList2.append((szName, iBonus, szChar))
				else:
					aBonusList3.append((szName, iBonus, szChar))
			del BONUSCLASS_CULTURE
			# Sort them alphabetically
			aBuildingList0.sort(key=itemgetter(0))
			aBuildingList1.sort(key=itemgetter(0))
			aBuildingList2.sort(key=itemgetter(0))
			aBonusList1.sort(key=itemgetter(0))
			aBonusList2.sort(key=itemgetter(0))
			aBonusList3.sort(key=itemgetter(0))
			# Cache Info Tuples
			self.aSortedBuildingTuple = [aBuildingList0, aBuildingList1, aBuildingList2]
			del aBuildingList0, aBuildingList1, aBuildingList2
			self.aSortedBonusTuple = [[], aBonusList1, aBonusList2, aBonusList3]
			del aBonusList1, aBonusList2, aBonusList3
		else:
			iNumSpecialistInfos	= self.iNumSpecialistInfos
			aSpecialistIconList = self.aSpecialistIconList
		self.bInitMiMap = True

		# This is the main interface screen, create it as such
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
		screen.setMainInterface(True)
		self.iInterfaceType = InterfaceVisibility.INTERFACE_HIDE_ALL
		##############
		# Resolution #
		xRes = screen.getXResolution()
		yRes = screen.getYResolution()
		halfX = xRes / 2
		a4thX = xRes / 4
		xCityHeader = 260
		if self.xRes != xRes or self.yRes != yRes:
			self.xRes = xRes
			self.yRes = yRes
			screen.setDimensions(0, 0, xRes, yRes)
			if xRes > 1700:
				if xRes > 2500:
					self.aFontList = aFontList = ["<font=4b>", "<font=4b>", "<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>"]
				else:
					self.aFontList = aFontList = ["<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>"]
				self.xPopProgBar = xPopProgBar = a4thX
				self.iPopProgBarHeight = iPopProgBarHeight = 28
				self.iPopProgBarWidth = iPopProgBarWidth = halfX
				self.iSpecialist_IconSize = 28
				self.iResID = iResID = 2
				self.iPlotListIconSize = 40
			elif xRes > 1400:
				self.aFontList = aFontList = ["<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0b>", "<font=0b>"]
				self.xPopProgBar = xPopProgBar = xCityHeader + 184
				self.iPopProgBarHeight = iPopProgBarHeight = 25
				self.iPopProgBarWidth = iPopProgBarWidth = xRes - xPopProgBar*2
				self.iSpecialist_IconSize = 24
				self.iResID = iResID = 1
				self.iPlotListIconSize = 34
			else:
				self.aFontList = aFontList = ["<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0b>", "<font=0>", "<font=0>", "<font=0>"]
				self.xPopProgBar = xPopProgBar = xCityHeader + 136
				self.iPopProgBarHeight = iPopProgBarHeight = 22
				self.iPopProgBarWidth = iPopProgBarWidth = xRes - xPopProgBar*2
				self.iSpecialist_IconSize = 20
				self.iResID = iResID = 0
				self.iPlotListIconSize = 28

			self.hBotBar = hBotBar = 82 + yRes/10
			self.yBotBar = yBotBar = yRes - hBotBar
			self.yMinimap = yMinimap = yBotBar + 30
			self.xMinimap = xMinimap = xRes - int((yRes - yMinimap - 3) * 1.5) - 3
			self.xMidL = xMidL = 128 + xRes/8
			self.xMidR = xMidR = xMinimap - 92
		else:
			iResID = self.iResID
			aFontList = self.aFontList
			yMinimap = self.yMinimap
			xMinimap = self.xMinimap
			xMidL = self.xMidL
			xMidR = self.xMidR
			yBotBar = self.yBotBar
			hBotBar = self.hBotBar
			xPopProgBar = self.xPopProgBar
			iPopProgBarHeight = self.iPopProgBarHeight
			iPopProgBarWidth = self.iPopProgBarWidth
			bResChange = False
		# Fetch GUI enumerators
		eFontSmall	= FontTypes.SMALL_FONT
		eFontGame	= FontTypes.GAME_FONT
		eWidGen		= WidgetTypes.WIDGET_GENERAL
		iWidAction	= WidgetTypes.WIDGET_ACTION
		ePanelSTD	= PanelStyles.PANEL_STYLE_STANDARD
		iPanelEmpty	= PanelStyles.PANEL_STYLE_EMPTY
		iTableSTD	= TableStyles.TABLE_STYLE_STANDARD
		eBtnLabel = ButtonStyles.BUTTON_STYLE_LABEL
		iButtonStd	 = ButtonStyles.BUTTON_STYLE_STANDARD
		self.shownResearchSelection = []

		self.InCity = None
		self.bCityScreen = False
		##############
		# Background #
		dY = yBotBar + 18
		# Left City Background
		panel = "InterfaceLeftCityBackgroundWidget"
		screen.addPanel(panel, "", "", True, False, 0, 0, a4thX, dY, ePanelSTD)
		screen.setStyle(panel, "Panel_City_Left_Style")
		# Top City Background
		panel = "InterfaceTopCityBackgroundWidget"
		screen.addPanel(panel, "", "", True, False, a4thX, 0, halfX, dY, ePanelSTD)
		screen.setStyle(panel, "Panel_City_Top_Style")
		# Right City Background
		panel = "InterfaceRightCityBackgroundWidget"
		screen.addPanel(panel, "", "", True, False, xRes - a4thX, 0, a4thX, dY, ePanelSTD)
		screen.setStyle(panel, "Panel_City_Right_Style")
		# Top City Bar
		panel = "CityScreenTopWidget"
		screen.addPanel(panel, "", "", True, False, 0, -2, xRes, 38, ePanelSTD)
		screen.setStyle(panel, "Panel_TopBar_Style")
		# Center Bottom Background Widget. Unit and city action list.
		panel = "InterfaceCenterBackgroundWidget"
		screen.addPanel(panel, "", "", True, False, xMidL - 12, yBotBar + 10, xMidR - xMidL + 24, hBotBar - 10, ePanelSTD)
		screen.setStyle(panel, "Panel_Game_HudBC_Style")
		# Left Bottom Background Widget. Selected unit & construction list.
		panel = "InterfaceLeftBackgroundWidget"
		screen.addPanel(panel, "", "", True, False, 0, yBotBar, xMidL, hBotBar, ePanelSTD)
		screen.setStyle(panel, "Panel_Game_HudBL_Style")
		# Right Background Widget. Flag & minimap
		panel = "InterfaceRightBackgroundWidget"
		screen.addPanel(panel, "", "", True, False, xMidR, yBotBar, xRes - xMidR, hBotBar, ePanelSTD)
		screen.setStyle(panel, "Panel_Game_HudBR_Style")
		# Top Center Background
		if not iResID:
			h = 64
		else:
			h = 44
		panel = "InterfaceTopCenter"
		screen.addPanel(panel, "", "", True, False, xMidL - 12, -2, xMidR - xMidL + 24, h, ePanelSTD)
		screen.setStyle(panel, "Panel_Game_HudTC_Style")
		# Top Left Background
		panel = "InterfaceTopLeft"
		screen.addPanel(panel, "", "", True, False, 0, -2, xMidL, 52, ePanelSTD)
		screen.setStyle(panel, "Panel_Game_HudTL_Style")
		# Top Right Background
		panel = "InterfaceTopRight"
		screen.addPanel(panel, "", "", True, False, xMidR, -2, xRes - xMidR, 52, ePanelSTD)
		screen.setStyle(panel, "Panel_Game_HudTR_Style")
		###############
		# City Screen #
		x = xCityHeader
		y = 28
		# City Top
		panel = "CityNameBackground"
		screen.addPanel(panel, "", "", True, False, x, y, xRes - x*2, 40, ePanelSTD)
		screen.setStyle(panel, "Panel_City_Title_Style")
		# Pop. & Prod. background.
		w = halfX - x
		panel = "TopCityPanelLeft"
		screen.addPanel(panel, "", "", True, False, x, 64, w, 64, ePanelSTD)
		screen.setStyle(panel, "Panel_City_TanTL_Style")
		panel = "TopCityPanelRight"
		screen.addPanel(panel, "", "", True, False, halfX, 64, w, 64, ePanelSTD)
		screen.setStyle(panel, "Panel_City_TanTR_Style")
		# City Top Left Commerce Control
		wComCon = x - 8
		yComCon = 32
		hComCon = 96
		panel = "CityScreenAdjustPanel"
		screen.addPanel(panel, "", "", True, False, 4, yComCon, wComCon, hComCon, ePanelSTD)
		screen.setStyle(panel, "Panel_City_Info_Style")
		maintenanceText = "<font=1>" + TRNSLTR.getText("TXT_KEY_CONCEPT_MAINTENANCE", ()) + ":"
		screen.setLabel("MaintenanceText", "", maintenanceText, 1<<0, 8, 34, 0, eFontSmall, WidgetTypes.WIDGET_HELP_MAINTENANCE, 0, 0)
		yTradeRoutesTop = yComCon + hComCon
		# City Top Right Religions and Corp. Panel
		yComCon = 32
		hComCon = 96
		panel = "CityScreenTopRightPanel"
		screen.addPanel(panel, "", "", True, False, xRes - x + 4, yComCon, wComCon, hComCon, ePanelSTD)
		screen.setStyle(panel, "Panel_City_Info_Style")
		self.CityReligionPanel = [xRes - x + 4, yComCon, wComCon, hComCon]
		# Wonder Limits on "CityNameBackground" left side.
		x += 8
		screen.setLabel("WonderLimit0", "", "", 1<<0, x, y, 0, eFontSmall, eWidGen, 0, 0)
		y += 18
		screen.setLabel("WonderLimit1", "", "", 1<<0, x, y, 0, eFontSmall, eWidGen, 0, 0)
		# Population Bar
		szColorFood		= GC.getYieldInfo(YieldTypes.YIELD_FOOD).getColorType()
		szColorProd		= GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getColorType()
		szColorEmpty	= GC.getInfoTypeForString("COLOR_EMPTY")
		szColorNegRate	= GC.getInfoTypeForString("COLOR_NEGATIVE_RATE")
		szColorStoredGP	= GC.getInfoTypeForString("COLOR_GREAT_PEOPLE_STORED")
		screen.addStackedBarGFC("PopulationBar", xPopProgBar, 68, iPopProgBarWidth, iPopProgBarHeight, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_POPULATION, 0, 1)
		screen.setStackedBarColors("PopulationBar", InfoBarTypes.INFOBAR_STORED, szColorFood)
		screen.setStackedBarColorsAlpha("PopulationBar", InfoBarTypes.INFOBAR_RATE, szColorFood, 0.8)
		screen.setStackedBarColors("PopulationBar", InfoBarTypes.INFOBAR_RATE_EXTRA, szColorNegRate)
		screen.setStackedBarColors("PopulationBar", InfoBarTypes.INFOBAR_EMPTY, szColorEmpty)
		# Production Bar
		screen.addStackedBarGFC("ProductionBar", xPopProgBar, 96, iPopProgBarWidth, iPopProgBarHeight, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_PRODUCTION, 0, 1)
		screen.setStackedBarColors("ProductionBar", InfoBarTypes.INFOBAR_STORED, szColorProd)
		screen.setStackedBarColorsAlpha("ProductionBar", InfoBarTypes.INFOBAR_RATE, szColorProd, 0.8)
		screen.setStackedBarColors("ProductionBar", InfoBarTypes.INFOBAR_RATE_EXTRA, szColorFood)
		screen.setStackedBarColors("ProductionBar", InfoBarTypes.INFOBAR_EMPTY, szColorEmpty)
		#-- Right side label
		screen.setText("CS|Happiness0", "", "", 1<<0, xRes - xPopProgBar + 6, 98, 0, eFontGame, eWidGen, 0, 0)
		screen.hide("CS|Happiness0")
		#####################
		# City flag buttons #
		if not iResID:
			x = xMidR + 19
			y = yBotBar + 34
			h = 18
			w = 60
		elif iResID == 1:
			x = xMidR + 16
			y = yBotBar + 37
			h = 20
			w = 66
		else:
			x = xMidR + 12
			y = yBotBar + 40
			h = 22
			w = 72
		uFont = aFontList[4]
		aHideList0 = []
		# Conscript button
		Btn = "Conscript"
		screen.setButtonGFC(Btn, uFont + self.szDraft, "", x, y, w, h, WidgetTypes.WIDGET_CONSCRIPT, 0, 1, iButtonStd)
		screen.setStyle(Btn, "Button_CityT1_Style")
		aHideList0.append(Btn)
		# Hurry Buttons
		y += h
		s = w/2
		x1 = x + s
		Btn = "Hurry0"
		screen.setButtonGFC(Btn, "", "", x, y, s, s, WidgetTypes.WIDGET_HURRY, 0, -1, iButtonStd)
		screen.setStyle(Btn, "Button_CityC1_Style")
		aHideList0.append(Btn)
		Btn = "Hurry1"
		screen.setButtonGFC(Btn, "", "", x1, y, s, s, WidgetTypes.WIDGET_HURRY, 1, -1, iButtonStd)
		screen.setStyle(Btn, "Button_CityC2_Style")
		aHideList0.append(Btn)
		# Automate Production Button
		y += s
		Btn = "CS|AutomateProduction0"
		screen.addCheckBoxGFC(Btn, "", "", x, y, s, s, eWidGen, 1, 2, iButtonStd)
		screen.setStyle(Btn, "Button_CityC3_Style")
		aHideList0.append(Btn)
		# Automate Citizens Button
		Btn = "AutomateCitizens"
		screen.addCheckBoxGFC(Btn, "", "", x1, y, s, s, WidgetTypes.WIDGET_AUTOMATE_CITIZENS, -1, -1, iButtonStd)
		screen.setStyle(Btn, "Button_CityC4_Style")
		aHideList0.append(Btn)
		# Set Emphasize buttons
		y += s
		s = w/3
		Btn = "Emphasize0"
		screen.addCheckBoxGFC(Btn, "", "", x, y, s, s, WidgetTypes.WIDGET_EMPHASIZE, 0, -1, eBtnLabel)
		screen.setStyle(Btn, "Button_CityB1_Style")
		aHideList0.append(Btn)
		Btn = "Emphasize1"
		screen.addCheckBoxGFC(Btn, "", "", x + s, y, s, s, WidgetTypes.WIDGET_EMPHASIZE, 1, -1, eBtnLabel)
		screen.setStyle(Btn, "Button_CityB2_Style")
		aHideList0.append(Btn)
		Btn = "Emphasize2"
		screen.addCheckBoxGFC(Btn, "", "", x + s * 2, y, s, s, WidgetTypes.WIDGET_EMPHASIZE, 2, -1, eBtnLabel)
		screen.setStyle(Btn, "Button_CityB3_Style")
		aHideList0.append(Btn)
		y += s
		Btn = "Emphasize3"
		screen.addCheckBoxGFC(Btn, "", "", x, y, s, s, WidgetTypes.WIDGET_EMPHASIZE, 3, -1, eBtnLabel)
		screen.setStyle(Btn, "Button_CityB4_Style")
		aHideList0.append(Btn)
		Btn = "Emphasize4"
		screen.addCheckBoxGFC(Btn, "", "", x + s, y, s, s, WidgetTypes.WIDGET_EMPHASIZE, 4, -1, eBtnLabel)
		screen.setStyle(Btn, "Button_CityB5_Style")
		aHideList0.append(Btn)
		Btn = "Emphasize5"
		screen.addCheckBoxGFC(Btn, "", "", x + s * 2, y, s, s, WidgetTypes.WIDGET_EMPHASIZE, 5, -1, eBtnLabel)
		screen.setStyle(Btn, "Button_CityB6_Style")
		aHideList0.append(Btn)
		# Emphasize Health
		w = 18
		Btn = "Emphasize6"
		screen.addCheckBoxGFC(Btn, "", "", xRes - 264 - w, 98, w, 28, WidgetTypes.WIDGET_EMPHASIZE, 6, -1, eBtnLabel)
		screen.setStyle(Btn, "Button_CityB5_Style")
		aHideList0.append(Btn)
		# Emphasize Happiness
		Btn = "Emphasize7"
		screen.addCheckBoxGFC(Btn, "", "", xRes - 264 - w, 70, w, 28, WidgetTypes.WIDGET_EMPHASIZE, 7, -1, eBtnLabel)
		screen.setStyle(Btn, "Button_CityB5_Style")
		aHideList0.append(Btn)
		# Liberate button
		x = xMinimap - 5
		y = yMinimap - h
		w = yRes - yBotBar
		Btn = "Liberate"
		szTxt = uFont + TRNSLTR.getText("TXT_KEY_LIBERATE_CITY", ())
		screen.setButtonGFC(Btn, szTxt, "", x, y, w, h, WidgetTypes.WIDGET_LIBERATE_CITY, 0, 0, iButtonStd)
		screen.setStyle(Btn, "Button_CityT1_Style")
		screen.hide(Btn)

		################
		# Trade Routes #
		x = 8
		y = yTradeRoutesTop
		w = 196
		iSize = 24
		screen.addPanel("TradeRouteListBackground", "", "", True, False, x, y, w, iSize + 8, ePanelSTD)
		screen.setStyle("TradeRouteListBackground", "Panel_City_Header_Style")
		# Raw Yields off.
		y += 6
		screen.setLabel("TradeRouteListLabel", "", self.szHeaderListTradeRoute, 1<<2, w / 2 + x, y, 0, eFontSmall, eWidGen, 0, 0)
		screen.hide("TradeRouteListLabel")
		szHighlightButton = CyArtFileMgr().getInterfaceArtInfo("RAW_YIELDS_HIGHLIGHT").getPath()
		# Raw Yields on
		x += 6
		dx = 24
		iGap = 8
		### Trade
		artPath = CyArtFileMgr().getInterfaceArtInfo("RAW_YIELDS_TRADE").getPath()
		screen.addCheckBoxGFC("CS|RawYields0", artPath, szHighlightButton, x, y, iSize, iSize, eWidGen, 0, -1, eBtnLabel)
		screen.hide("CS|RawYields0")
		### Yields
		x += dx + iGap
		screen.addCheckBoxGFC("CS|RawYields1", self.artPathRawYieldsFood, szHighlightButton, x, y, iSize, iSize, eWidGen, 1, -1, eBtnLabel)
		screen.hide("CS|RawYields1") # Food
		x += dx
		screen.addCheckBoxGFC("CS|RawYields2", self.artPathRawYieldsProduction, szHighlightButton, x, y, iSize, iSize, eWidGen, 2, -1, eBtnLabel)
		screen.hide("CS|RawYields2") # Production
		x += dx
		screen.addCheckBoxGFC("CS|RawYields3", self.artPathRawYieldsCommerce, szHighlightButton, x, y, iSize, iSize, eWidGen, 3, -1, eBtnLabel)
		screen.hide("CS|RawYields3") # Commerce
		### Tile Selection
		artPath = CyArtFileMgr().getInterfaceArtInfo("RAW_YIELDS_WORKED_TILES").getPath()
		x += dx + iGap
		screen.addCheckBoxGFC("CS|RawYields4", artPath, szHighlightButton, x, y, iSize, iSize, eWidGen, 4, 1, eBtnLabel)
		screen.hide("CS|RawYields4") # Worked Tiles
		artPath = CyArtFileMgr().getInterfaceArtInfo("RAW_YIELDS_CITY_TILES").getPath()
		x += dx
		screen.addCheckBoxGFC("CS|RawYields5", artPath, szHighlightButton, x, y, iSize, iSize, eWidGen, 5, 1, eBtnLabel)
		screen.hide("CS|RawYields5") # City Tiles
		artPath = CyArtFileMgr().getInterfaceArtInfo("RAW_YIELDS_OWNED_TILES").getPath()
		x += dx
		screen.addCheckBoxGFC("CS|RawYields6", artPath, szHighlightButton, x, y, iSize, iSize, eWidGen, 6, 1, eBtnLabel)
		screen.hide("CS|RawYields6") # Owned Tiles
		#######################
		# Building built list #
		x = 8
		y = self.yTradeRouteTable + self.hTradeRouteTable + 8
		dy = 38
		self.yBuildListTable = y + dy
		szPre = "<img=Art/Interface/screens/City/"
		aList = [
			"Buildings.dds>",
			"Wonders.dds>",
			"Specials.dds>"
		]
		iPushed = self.iShowBuildings
		for i in xrange(3):
			sz = str(i)
			PNL = "BldgList|Bkgnd" + sz
			TXT = "BldgList|Btn" + sz
			screen.addPanel(PNL, "", "", True, False, x, y, dy, dy, ePanelSTD)
			if i == iPushed:
				screen.setStyle(PNL, "GFC_Control_InPanel_Style")
				z = 3
			else:
				screen.setStyle(PNL, "GFC_Control_OutPanel_Style")
				z = 1
			screen.setText(TXT, "", szPre + aList[i], 1<<0, x+1, y+z, 0, eFontSmall, eWidGen, 0, 0)
			x += dy + 2
		# Demolish
		x = a4thX - dy
		ID = "BldgList|Demolish0"
		screen.setText(ID, "", szPre + "Demolish_Btn.dds>", 1<<0, x, y, 0, eFontSmall, eWidGen, 0, 0)
		###################
		# Right Side List #
		x = xRes - a4thX + 8
		y = 132
		# Properties
		ID = "BonusList|Bkgnd0"
		screen.addPanel(ID, "", "", True, False, x, y, dy, dy, PanelStyles.PANEL_STYLE_CITY_TANSHADE)
		screen.setStyle(ID, "GFC_Control_InPanel_Style")
		ID = "BonusList|Btn0"
		screen.setText(ID, "", szPre + "Properties.dds>", 1<<0, x+1, y+3, 0, eFontSmall, eWidGen, 0, 0)
		# Map Bonuses
		x += dy + 2
		ID = "BonusList|Bkgnd1"
		screen.addPanel(ID, "", "", True, False, x, y, dy, dy, ePanelSTD)
		screen.setStyle(ID, "GFC_Control_OutPanel_Style")
		ID = "BonusList|Btn1"
		screen.setText(ID, "", szPre + "MapBonus.dds>", 1<<0, x+1, y+1, 0, eFontSmall, eWidGen, 0, 0)
		# Products
		x += dy + 2
		ID = "BonusList|Bkgnd2"
		screen.addPanel(ID, "", "", True, False, x, y, dy, dy, ePanelSTD)
		screen.setStyle(ID, "GFC_Control_OutPanel_Style")
		ID = "BonusList|Btn2"
		screen.setText(ID, "", szPre + "Manufacture.dds>", 1<<0, x+1, y+1, 0, eFontSmall, eWidGen, 0, 0)
		# Cultures
		x += dy + 2
		ID = "BonusList|Bkgnd3"
		screen.addPanel(ID, "", "", True, False, x, y, dy, dy, ePanelSTD)
		screen.setStyle(ID, "GFC_Control_OutPanel_Style")
		ID = "BonusList|Btn3"
		screen.setText(ID, "", szPre + "Cultures.dds>", 1<<0, x+1, y+1, 0, eFontSmall, eWidGen, 0, 0)

		# Status bars
		x = 8
		if not iResID:
			h = 22
		elif iResID == 1:
			h = 25
		else:
			h = 28
		y = yBotBar - h + 8
		w = xMidL - 16
		# Right Side
		if xMidR > xRes - a4thX:
			xR = xMidR + 8
		else:
			xR = xRes - a4thX + 8
		wR = xRes - xR - 8
		#-- Nationality Bar
		screen.addStackedBarGFC("NationalityBar", 8, y, w, h, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_NATIONALITY, 0, 0)
		#-- Culture Bar
		screen.addStackedBarGFC("CultureBar", xR, y, wR, h, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_CULTURE, 0, 0)
		screen.setStackedBarColors("CultureBar", InfoBarTypes.INFOBAR_STORED, GC.getInfoTypeForString("COLOR_CULTURE_STORED"))
		screen.setStackedBarColors("CultureBar", InfoBarTypes.INFOBAR_RATE, GC.getInfoTypeForString("COLOR_CULTURE_RATE"))
		screen.setStackedBarColors("CultureBar", InfoBarTypes.INFOBAR_RATE_EXTRA, szColorEmpty)
		screen.setStackedBarColors("CultureBar", InfoBarTypes.INFOBAR_EMPTY, szColorEmpty)
		self.yCityStackBarBot = y + 2
		#-- Revolution Bar
		y -= h
		if self.GO_REVOLUTION:
			screen.addStackedBarGFC("CS|RevBar0", 8, y, w, h, InfoBarTypes.NUM_INFOBAR_TYPES, eWidGen, -1, -1)
			screen.hide("CS|RevBar0")
		#-- Great People Bar
		screen.addStackedBarGFC("GreatPeopleBar", xR, y, wR, h, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_GREAT_PEOPLE, 0, 0)
		screen.setStackedBarColors("GreatPeopleBar", InfoBarTypes.INFOBAR_STORED, szColorStoredGP)
		screen.setStackedBarColors("GreatPeopleBar", InfoBarTypes.INFOBAR_RATE, GC.getInfoTypeForString("COLOR_GREAT_PEOPLE_RATE"))
		screen.setStackedBarColors("GreatPeopleBar", InfoBarTypes.INFOBAR_RATE_EXTRA, szColorEmpty)
		screen.setStackedBarColors("GreatPeopleBar", InfoBarTypes.INFOBAR_EMPTY, szColorEmpty)
		self.xwCityStackBarR = [xR, wR]
		self.yCityStackBarTop = yCityStackBarTop = y + 2

		##############
		# Components #
		##############
		# Field of view slider
		x = xRes - 156
		y = 48
		iFoV = self.iField_View
		screen.addSlider("FoVSlider", x, y, 132, 12, iFoV, 20, 80, eWidGen, 0, 0, False)
		screen.hide("FoVSlider")
		self.iX_FoVSlider = x
		self.iY_FoVSlider = y
		self.setFieldofView_Text(screen, iFoV)
		self.setFieldofView(iFoV)
		screen.hide("FoVSliderText")

		# Entity Panel
		panel = "SelectedEntityPanel"
		screen.addPanel(panel, "", "", True, False, 4, yBotBar + 30, xMidL - 92, hBotBar - 34, ePanelSTD)
		screen.setStyle(panel, "Panel_Game_HudStat_Style")
		screen.hide(panel)

		# Promotion Scroll Panel
		ScPnl = "PromoPanel"
		screen.addScrollPanel(ScPnl, "", xMidL, yBotBar + 18, xMidR - xMidL, 38, iPanelEmpty)
		screen.setStyle(ScPnl, "ScrollPanel_Min_Style")
		szChild = "WID|PROMO|PromoPanel"
		for iPromo in xrange(self.iNumPromotionInfos):
			name = szChild + str(iPromo)
			screen.setImageButtonAt(name, ScPnl, GC.getPromotionInfo(iPromo).getButton(), 0, 0, 32, 32, eWidGen, 0, 0)
			screen.hide(name)
		self.aHideList = []
		self.aSelUnitPromoList = []

		artPath = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_MENU_ICON").getPath()
		screen.setImageButton("MainMenuButton", artPath, xRes - 30, 0, 32, 30, WidgetTypes.WIDGET_MENU_ICON, 0, 0)
		artPath = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_CIVILOPEDIA_ICON").getPath()
		screen.setImageButton("PediaButton0", artPath, 0, 0, 26, 28, eWidGen, 0, 0)

		# Advisor Buttons
		iSize = 28
		x = xRes - iSize*2
		y = 22
		dx = 24
		# Heritage
		btn = "AdvBtn10"
		screen.setImageButton(btn, "", x, y, iSize, iSize, eWidGen, 0, 0)
		screen.setStyle(btn, "Button_HUDBtnHeritage_Style")
		screen.hide(btn)
		x -= dx
		# Partisan:
		if self.GO_REVOLUTION:
			btn = "AdvBtn9"
			screen.setImageButton(btn, "Art/Interface/Buttons/revbtn.dds", x, y, iSize, iSize, eWidGen, 0, 0)
			screen.setStyle(btn, "Button_HUDSmall_Style")
			screen.hide(btn)
			x -= dx
		# Intelligence
		btn = "AdvBtn8"
		screen.setImageButton(btn, "", x, y, iSize, iSize, eWidGen, 0, 0)
		screen.setStyle(btn, "Button_HUDAdvisorEspionage_Style")
		screen.hide(btn)
		x -= dx
		# Corporation
		btn = "AdvBtn7"
		screen.setImageButton(btn, "", x, y, iSize, iSize, eWidGen, 0, 0)
		screen.setStyle(btn, "Button_HUDAdvisorCorporation_Style")
		screen.hide(btn)
		x -= dx
		# Religious
		btn = "AdvBtn6"
		screen.setImageButton(btn, "", x, y, iSize, iSize, eWidGen, 0, 0)
		screen.setStyle(btn, "Button_HUDAdvisorReligious_Style")
		screen.hide(btn)
		x -= dx
		# Technology
		btn = "AdvBtn5"
		screen.setImageButton(btn, "", x, y, iSize, iSize, eWidGen, 0, 0)
		screen.setStyle(btn, "Button_HUDAdvisorTechnology_Style")
		screen.hide(btn)
		x -= dx
		# Military
		btn = "AdvBtn4"
		screen.setImageButton(btn, "", x, y, iSize, iSize, eWidGen, 0, 0)
		screen.setStyle(btn, "Button_HUDAdvisorMilitary_Style")
		screen.hide(btn)
		x -= dx
		# Foreign
		btn = "AdvBtn3"
		screen.setImageButton(btn, "", x, y, iSize, iSize, eWidGen, 0, 0)
		screen.setStyle(btn, "Button_HUDAdvisorForeign_Style")
		screen.hide(btn)
		x -= dx
		# Civics
		btn = "AdvBtn2"
		screen.setImageButton(btn, "", x, y, iSize, iSize, eWidGen, 0, 0)
		screen.setStyle(btn, "Button_HUDAdvisorCivics_Style")
		screen.hide(btn)
		x -= dx
		# Domestic
		btn = "AdvBtn0"
		screen.setImageButton(btn, "", x, y, iSize, iSize, eWidGen, 0, 0)
		screen.setStyle(btn, "Button_HUDAdvisorDomestic_Style")
		screen.hide(btn)
		# Treasury
		btn = "AdvBtn1"
		screen.setText(btn, "", "<img=Art/Interface/Buttons/general/Treasury.dds>", 1<<0, 26, 0, 0, eFontSmall, eWidGen, 0, 0)
		screen.setLabel("Treasury", "", "", 1<<0, 60, 0, 0, eFontSmall, eWidGen, 0, 0)
		screen.hide(btn)

		x = 42
		# Top-left button row
		btn = "TurnLogBtn"
		screen.setImageButton(btn, "", x, y, iSize, iSize, iWidAction, GC.getControlInfo(ControlTypes.CONTROL_TURN_LOG).getActionInfoIndex(), -1)
		screen.setStyle(btn, "Button_HUDLog_Style")
		screen.hide(btn)
		x += dx

		btn = "VictoryAdvBtn"
		screen.setImageButton(btn, "", x, y, iSize, iSize, iWidAction, GC.getControlInfo(ControlTypes.CONTROL_VICTORY_SCREEN).getActionInfoIndex(), -1)
		screen.setStyle(btn, "Button_HUDAdvisorVictory_Style")
		screen.hide(btn)
		x += dx

		btn = "InfoAdvBtn"
		screen.setImageButton(btn, "", x, y, iSize, iSize, iWidAction, GC.getControlInfo(ControlTypes.CONTROL_INFO).getActionInfoIndex(), -1)
		screen.setStyle(btn, "Button_HUDAdvisorRecord_Style")
		screen.hide(btn)

		btn = "BuildListBtn0"
		artPath = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_POPUPBUTTON_PRODUCTION").getPath()
		x += dx
		screen.setImageButton(btn, artPath, x, y, iSize, iSize, eWidGen, -1, -1)
		screen.setStyle(btn, "Button_HUDSmall_Style")
		screen.hide(btn)

		btn = "OptionsBtnBUG0"
		artPathButtonOptionBUG = CyArtFileMgr().getInterfaceArtInfo("BUG_OPTIONS_SCREEN_BUTTON").getPath()
		x += dx
		screen.setImageButton(btn, artPathButtonOptionBUG, x, y, iSize, iSize, eWidGen, -1, -1)
		screen.setStyle(btn, "Button_HUDSmall_Style")
		screen.hide(btn)

		btn = "ParallelMapsBtn"
		artPath = CyArtFileMgr().getInterfaceArtInfo("BUG_WORLDWONDER_OFF").getPath()
		x += dx
		screen.setImageButton(btn, artPath, x, y, iSize, iSize, eWidGen, -1, -1)
		screen.setStyle(btn, "Button_HUDSmall_Style")
		screen.hide(btn)

		btn = "DebugBtn0"
		artPath = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_DEBUG_SCREEN_BUTTON").getPath()
		x += dx
		screen.setImageButton(btn, artPath, x, y, iSize, iSize, eWidGen, -1, -1)
		screen.setStyle(btn, "Button_HUDSmall_Style")
		screen.hide(btn)

		# Minimap Panel
		panel = "MiniMapPanel"
		screen.addPanel(panel, "", "", True, True, xMinimap - 6, yMinimap - 18, xRes - xMinimap + 8, yRes - yMinimap + 20, ePanelSTD)
		screen.setStyle(panel, "Panel_Game_HudMap_Style")
		screen.moveToFront("Liberate")
		# Minimap buttons
		aMinimapBtnList = []
		dx = 36
		x = xRes - dx
		y = yBotBar - 4
		aBtn = "GlobeToggle"
		screen.addCheckBoxGFC(aBtn, "", "", x, y, dx, dx, iWidAction, GC.getControlInfo(ControlTypes.CONTROL_GLOBELAYER).getActionInfoIndex(), -1, eBtnLabel)
		screen.setStyle(aBtn, "Button_HUDZoom_Style")

		dx = 28
		x -= dx
		y += 8
		aBtn = "MMB|ScoreToggle0"
		screen.addCheckBoxGFC(aBtn, "", "", x, y, dx, dx, eWidGen, -1, -1, eBtnLabel)
		screen.setStyle(aBtn, "Button_HUDBtnRank_Style")
		aMinimapBtnList.append(aBtn)

		x -= dx
		aBtn = "Grid"
		screen.addCheckBoxGFC(aBtn, "", "", x, y, dx, dx, iWidAction, GC.getControlInfo(ControlTypes.CONTROL_GRID).getActionInfoIndex(), -1, eBtnLabel)
		screen.setStyle(aBtn, "Button_HUDBtnGrid_Style")
		aMinimapBtnList.append(aBtn)

		x -= dx
		aBtn = "MMB|BareMap0"
		screen.addCheckBoxGFC(aBtn, "", "", x, y, dx, dx, eWidGen, -1, -1, eBtnLabel)
		screen.setStyle(aBtn, "Button_HUDBtnClearMap_Style")
		aMinimapBtnList.append(aBtn)

		x -= dx
		aBtn = "Yields"
		screen.addCheckBoxGFC(aBtn, "", "", x, y, dx, dx, iWidAction, GC.getControlInfo(ControlTypes.CONTROL_YIELDS).getActionInfoIndex(), -1, eBtnLabel)
		screen.setStyle(aBtn, "Button_HUDBtnTileAssets_Style")
		aMinimapBtnList.append(aBtn)

		x -= dx
		aBtn = "ResourceIcons"
		screen.addCheckBoxGFC(aBtn, "", "", x, y, dx, dx, iWidAction, GC.getControlInfo(ControlTypes.CONTROL_RESOURCE_ALL).getActionInfoIndex(), -1, eBtnLabel)
		screen.setStyle(aBtn, "Button_HUDBtnResources_Style")
		aMinimapBtnList.append(aBtn)

		x -= dx
		aBtn = "UnitIcons"
		screen.addCheckBoxGFC(aBtn, "", "", x, y, dx, dx, iWidAction, GC.getControlInfo(ControlTypes.CONTROL_UNIT_ICONS).getActionInfoIndex(), -1, eBtnLabel)
		screen.setStyle(aBtn, "Button_HUDGlobeUnit_Style")
		aMinimapBtnList.append(aBtn)

		self.aMinimapBtnList = aMinimapBtnList

		# Globeview buttons
		x = xRes - 36
		artPath = artPathButtonOptionBUG
		iNumLayers = CyGlobeLayerManager().getNumLayers()
		aGlobeLayerList = []
		for i in xrange(iNumLayers):
			x -= dx
			szButtonID = "GlobeLayer" + str(i)
			szStyle = CyGlobeLayerManager().getLayer(i).getButtonStyle()
			if szStyle == "Button_HUDGlobeDebug_Style":
				screen.addCheckBoxGFC(szButtonID, artPath, artPath, x, y, dx, dx, WidgetTypes.WIDGET_GLOBELAYER, i, 1, eBtnLabel)
				screen.setStyle(szButtonID, "Button_HUDSmall_Style")
				screen.hide(szButtonID)
			else:
				if not szStyle:
					szStyle = "Button_HUDSmall_Style"
				screen.addCheckBoxGFC(szButtonID, "", "", x, y, dx, dx, WidgetTypes.WIDGET_GLOBELAYER, i, 1, eBtnLabel)
				screen.setStyle(szButtonID, szStyle)
				screen.hide(szButtonID)
			aGlobeLayerList.append(szButtonID)
		self.aGlobeLayerList = aGlobeLayerList

		#********#
		# SCORES #
		# * __ * #
		screen.addPanel("ScoreBackground", "", "", True, False, 0, 0, 0, 0, PanelStyles.PANEL_STYLE_HUD_HELP)
		screen.hide("ScoreBackground")

		#***********#
		# Plot List #
		### * _ * ###
		self.yBasePlotlist = 6 + yBotBar - self.iPlotListMaxRows * (self.iPlotListIconSize + 8)
		# Plot List Arrows
		x = xRes/2
		y = yBotBar + 4
		ID = "PlotListScroll"
		screen.addPanel(ID, "", "", True, True, x - 24, y - 8, 48, 28, PanelStyles.PANEL_STYLE_BLUE50)
		ID = "PlotList|Scroll0"
		img = "<img=Art/Interface/Buttons/general/Arrow_Up.dds size=16>"
		screen.setText(ID, "", img, 1<<1, x, y, 0, eFontGame, eWidGen, 0, 0)
		ID = "PlotList|Scroll1"
		img = "<img=Art/Interface/Buttons/general/Arrow_Down.dds size=16>"
		screen.setText(ID, "", img, 1<<0, x, y, 0, eFontGame, eWidGen, 0, 0)
		# Clear plot list GFC.
		self.cleanPlotList(screen)
		self.bPlotListAutoScroll = True

		# End Turn Text
		screen.setLabel("EndTurnText", "", "", 1<<2, 0, yRes - 256, -0.1, eFontGame, eWidGen, 0, 0)
		screen.setHitTest("EndTurnText", HitTestTypes.HITTEST_NOHIT)

		# Three states for end turn button...
		xEoT = xMidR - 7
		yEoT = yBotBar + 3
		screen.setImageButton("EndTurnButton", "", xEoT, yEoT, 32, 32, WidgetTypes.WIDGET_END_TURN, 1, 1)
		screen.setStyle("EndTurnButton", "Button_HUDEndTurn_Style")
		screen.setEndTurnState("EndTurnButton", "Disable")

		screen.addDDSGFCAt("EndTurnButtonDecal", "EndTurnButton", self.artPathRawYieldsProduction, 0, 0, 32, 32, eWidGen, 0, 0, True)
		screen.setHitTest("EndTurnButtonDecal", HitTestTypes.HITTEST_NOHIT)

		# *************** #
		# CITIZEN BUTTONS #
		# *************** #
		iWidChangeCitizen	= WidgetTypes.WIDGET_CHANGE_SPECIALIST
		iWidDisabledCitizen = WidgetTypes.WIDGET_DISABLED_CITIZEN
		iWidCitizen			= WidgetTypes.WIDGET_CITIZEN

		iSize = self.iSpecialist_IconSize
		arrowSize = iSize/2
		a = arrowSize + iSize * 2 + 46
		iCol = (a4thX + a - 1) / a
		xStart = xRes - iSize - 20
		y = yCityStackBarTop - iSize - 4
		n = 0
		for i in xrange(iNumSpecialistInfos):
			szInc = str(i)
			image = '<img=%s size=%d></img>' %(aSpecialistIconList[i], iSize)
			# Dynamic Citizens
			if GC.getSpecialistInfo(i).isVisible():
				if not n % iCol:
					x = xStart
					y -= iSize + 6
				else:
					x -= iSize + 24

				ID = "CitizenCount" + szInc
				screen.setLabel(ID, "", "", 1<<0, x, y + 2, 0, eFontGame, eWidGen, 0, 0)
				screen.hide(ID)

				x -= arrowSize
				ID = "IncreaseSpecialist" + szInc
				screen.setButtonGFC(ID, "", "", x, y, arrowSize, arrowSize, iWidChangeCitizen, i, 1, iButtonStd)
				screen.setStyle(ID, "SF_CtrlTheme_Civ4_Control_Button_ArrowUp_Style")
				screen.hide(ID)
				ID = "DecreaseSpecialist" + szInc
				screen.setButtonGFC(ID, "", "", x, y + arrowSize, arrowSize, arrowSize, iWidChangeCitizen, i, -1, iButtonStd)
				screen.setStyle(ID, "SF_CtrlTheme_Civ4_Control_Button_ArrowDown_Style")
				screen.hide(ID)

				x -= iSize + 2
				ID = "CitizenDisabledButton" + szInc
				#screen.setText(ID, "", image, 1<<0, x, y, 0, eFontGame, iWidDisabledCitizen, i, 0)
				screen.addCheckBoxGFC(ID, aSpecialistIconList[i], self.artPathHilite, x, y, iSize, iSize, iWidDisabledCitizen, i, 0, ButtonStyles.BUTTON_STYLE_IMAGE)
				screen.enable(ID, False)
				screen.hide(ID)
				ID = "CitizenButton" + szInc
				#screen.setText(ID, "", image, 1<<0, x, y, 0, eFontGame, iWidCitizen, i, 0)
				screen.addCheckBoxGFC(ID, aSpecialistIconList[i], self.artPathHilite, x, y, iSize, iSize, iWidCitizen, i, 0, ButtonStyles.BUTTON_STYLE_IMAGE)
				screen.enable(ID, True)
				screen.hide(ID)
				n += 1
		self.yTopCitizen = y

		# **********************************************************
		# GAME DATA STRINGS
		# **********************************************************
		x1 = xMidL - 2
		y1 = 0
		w1 = xMidR - xMidL + 4
		h1 = 28
		if not iResID:	# Two Rows
			y2 = 26
			h2 = 22
			w1 = w1 / 3 - 2
			w2 = 2 * w1 + 2
			w3 = w1 + 4 + w2
			x2 = x1 + w1 + 4
			x3 = x1
		else:	# One Row
			y2 = y1
			h2 = h1
			w1 = w1 / 5 - 2
			w2 = w3 = 2 * w1 + 1
			x2 = x1 + w1 + 4
			x3 = x2 + w2 + 4

		# Great General Bar
		screen.addStackedBarGFC("GreatGeneralBar", x1, y1, w1, h1, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_HELP_GREAT_GENERAL, -1, -1)
		screen.setStackedBarColors("GreatGeneralBar", InfoBarTypes.INFOBAR_STORED, szColorNegRate)
		screen.setStackedBarColors("GreatGeneralBar", InfoBarTypes.INFOBAR_RATE, szColorEmpty)
		screen.setStackedBarColors("GreatGeneralBar", InfoBarTypes.INFOBAR_RATE_EXTRA, szColorEmpty)
		screen.setStackedBarColors("GreatGeneralBar", InfoBarTypes.INFOBAR_EMPTY, szColorEmpty)
		self.xMidBarTopGG = x1 + w1/2

		# ResearchBar
		iColor = GC.getInfoTypeForString("COLOR_RESEARCH_RATE")
		screen.addStackedBarGFC("ResearchBar", x2, y1, w2, h1, InfoBarTypes.NUM_INFOBAR_TYPES, eWidGen, 0, 0)
		screen.setStackedBarColors("ResearchBar", InfoBarTypes.INFOBAR_STORED, GC.getInfoTypeForString("COLOR_RESEARCH_STORED"))
		screen.setStackedBarColors("ResearchBar", InfoBarTypes.INFOBAR_RATE, iColor)
		screen.setStackedBarColors("ResearchBar", InfoBarTypes.INFOBAR_RATE_EXTRA, szColorEmpty)
		screen.setStackedBarColors("ResearchBar", InfoBarTypes.INFOBAR_EMPTY, szColorEmpty)
		self.xywhTechBar = [x2, y1, w2, h1]
		self.researchBarDC = StackBarUtil.StackBarDC("ResearchBarDC", x2, y1, w2, h1, iColor)

		# Great Person Bar
		screen.addStackedBarGFC("GreatPersonBar", x3, y2, w3, h2, InfoBarTypes.NUM_INFOBAR_TYPES, eWidGen, 0, 0)
		screen.setStackedBarColors("GreatPersonBar", InfoBarTypes.INFOBAR_STORED, szColorStoredGP)
		screen.setStackedBarColors("GreatPersonBar", InfoBarTypes.INFOBAR_RATE, GC.getInfoTypeForString("COLOR_GREAT_PEOPLE_RATE"))
		screen.setStackedBarColors("GreatPersonBar", InfoBarTypes.INFOBAR_RATE_EXTRA, szColorEmpty)
		screen.setStackedBarColors("GreatPersonBar", InfoBarTypes.INFOBAR_EMPTY, szColorEmpty)
		self.xywhGPBar = [x3, y2, w3, h2]
		self.iBarWidthTopGP = w3

		x1 = xMidR + 28
		x2 = x1 + 16
		y = yBotBar + 6
		# City Arrows
		screen.setButtonGFC("CityScrollMinus", "", "", x1, y, 28, 28, WidgetTypes.WIDGET_CITY_SCROLL, -1, 1, ButtonStyles.BUTTON_STYLE_ARROW_LEFT)
		screen.setButtonGFC("CityScrollPlus", "", "", x2, y, 28, 28, WidgetTypes.WIDGET_CITY_SCROLL, 1, 1, ButtonStyles.BUTTON_STYLE_ARROW_RIGHT)

		screen.registerHideList(aHideList0, len(aHideList0), 0)
		# Hide City UI
		screen.hideList(0)
		screen.hide("InterfaceLeftCityBackgroundWidget")
		screen.hide("InterfaceTopCityBackgroundWidget")
		screen.hide("InterfaceRightCityBackgroundWidget")
		screen.hide("InterfaceTopRightBackgroundWidget")
		screen.hide("CityScreenTopWidget")
		screen.hide("CityNameBackground")
		screen.hide("TopCityPanelLeft")
		screen.hide("TopCityPanelRight")
		screen.hide("CityScreenAdjustPanel")
		screen.hide("CityScreenTopRightPanel")
		screen.hide("TradeRouteListBackground")
		screen.hide("MaintenanceText")
		# Left side list tabs
		screen.hide("BldgList|Bkgnd0")
		screen.hide("BldgList|Bkgnd1")
		screen.hide("BldgList|Bkgnd2")
		screen.hide("BldgList|Btn0")
		screen.hide("BldgList|Btn1")
		screen.hide("BldgList|Btn2")
		screen.hide("BldgList|Demolish0")
		# Right side list tabs
		screen.hide("BonusList|Bkgnd0")
		screen.hide("BonusList|Bkgnd1")
		screen.hide("BonusList|Bkgnd2")
		screen.hide("BonusList|Bkgnd3")
		screen.hide("BonusList|Btn0")
		screen.hide("BonusList|Btn1")
		screen.hide("BonusList|Btn2")
		screen.hide("BonusList|Btn3")
		# Bars
		screen.hide("ProductionBar")
		screen.hide("PopulationBar")
		screen.hide("GreatPeopleBar")
		screen.hide("NationalityBar")
		screen.hide("CultureBar")
		# Misc
		screen.hide("WonderLimit0")
		screen.hide("WonderLimit1")
		# This should be a forced redraw screen.
		screen.setForcedRedraw(True)
		# Show the screen immidiately and pass input to the game.
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, True)


	# Will update the screen (every 250 ms)
	def updateScreen(self):
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
		IFT = self.iInterfaceType

		if ACEM.CD:
			# Demolition Screen is seen
			if not self.bCityScreen:
				# You hit the Esc key in the demolition screen.
				ACEM.exitCityDemolish(screen)

		if self.bTooltip:
			# Tooltip sometimes get stuck...
			POINT = Win32.getCursorPos()
			xDiff = POINT.x - self.xMouseTT
			yDiff = POINT.y - self.yMouseTT
			if xDiff < 0:
				xDiff = -xDiff
			if yDiff < 0:
				yDiff = -yDiff
			if xDiff > 256 and yDiff > 256 or xDiff + yDiff > 384:
				print "[WARN] Tooltip got stuck!"
				self.hideTooltip(screen)

		if IFT not in (InterfaceVisibility.INTERFACE_HIDE_ALL, InterfaceVisibility.INTERFACE_MINIMAP_ONLY):
			szOutput = ""
			iFirstBadConnection = CyMessageControl().GetFirstBadConnection()
			if GAME.isPaused(): # Pause overrides other messages
				szOutput = TRNSLTR.getText("TXT_KEY_SYSTEM_GAME_PAUSED", (GC.getPlayer(GAME.getPausePlayer()).getNameKey(),))
			elif iFirstBadConnection != -1:
				# Waiting on a bad connection to resolve
				if CyMessageControl().GetConnState(iFirstBadConnection) == 1:
					if GAME.isMPOption(MultiplayerOptionTypes.MPOPTION_ANONYMOUS):
						szOutput = TRNSLTR.getText("TXT_KEY_SYSTEM_WAITING_FOR_PLAYER", (GC.getPlayer(iFirstBadConnection).getNameKey(), 0))
					else:
						szOutput = TRNSLTR.getText("TXT_KEY_SYSTEM_WAITING_FOR_PLAYER", (GC.getPlayer(iFirstBadConnection).getNameKey(), (iFirstBadConnection + 1)))
				elif CyMessageControl().GetConnState(iFirstBadConnection) == 2:
					if GAME.isMPOption(MultiplayerOptionTypes.MPOPTION_ANONYMOUS):
						szOutput = TRNSLTR.getText("TXT_KEY_SYSTEM_PLAYER_JOINING", (GC.getPlayer(iFirstBadConnection).getNameKey(), 0))
					else:
						szOutput = TRNSLTR.getText("TXT_KEY_SYSTEM_PLAYER_JOINING", (GC.getPlayer(iFirstBadConnection).getNameKey(), (iFirstBadConnection + 1)))
			elif CyIF.shouldDisplayWaitingOthers():
				szOutput = "..."
			elif CyIF.shouldDisplayEndTurn():
				szOutput = ReminderEventManager.g_turnReminderTexts
				if not szOutput:
					szOutput = TRNSLTR.getText("TXT_KEY_SYSTEM_END_TURN", ())
			elif CyIF.shouldDisplayWaitingYou():
				szOutput = self.szSystemWaitingForYou
				self.iWaitingCounter += 1
				if RoMOpt.isPlayWaitingAlertSound():
					iTimer = 4 * RoMOpt.getPlayWaitingAlertSoundTimer()
					if self.iWaitingCounter > iTimer:
						self.iWaitingCounter = 0
						CyAudioGame().Play2DSound("AS2D_ADVISOR_SUGGEST")
			if szOutput:
				screen.setEndTurnState("EndTurnText", self.aFontList[4] + szOutput)
				screen.showEndTurn("EndTurnText")
			else:
				screen.hideEndTurn("EndTurnText")
				screen.hide("WaitingForPlayer")
			# NJAGC - start
			if IFT != InterfaceVisibility.INTERFACE_ADVANCED_START:
				screen.show("EraIndicator0")
				szTimeText = self.updateTimeText()

				if ClockOpt.isAlternateTimeText():
					if CyUserProfile().wasClockJustTurnedOn() or self.iTimeTextCounter < 1:
						# reset timer, display primary
						self.bShowTimeTextAlt = False
						self.iTimeTextCounter = ClockOpt.getAlternatePeriod() * 100
						CyUserProfile().setClockJustTurnedOn(False)
					else: # countdown timer
						self.iTimeTextCounter -= 25
						if self.iTimeTextCounter < 1:
							# timer elapsed, toggle between primary and alternate
							self.iTimeTextCounter = ClockOpt.getAlternatePeriod() * 100
							self.bShowTimeTextAlt = not self.bShowTimeTextAlt
				else:
					self.bShowTimeTextAlt = False

				screen.setLabel("TimeText", "", szTimeText, 1<<1, self.xRes - 31, 0, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, 0, 0)
			else:
				screen.hide("TimeText")
				screen.hide("EraIndicator0")
		else:
			screen.hide("TimeText")
			screen.hide("EraIndicator0")

		screen.hide("EndTurnButtonDecal")

		# Update the "End Turn" button.
		if CyIF.shouldDisplayEndTurnButton() and IFT == InterfaceVisibility.INTERFACE_SHOW:
			eState = CyIF.getEndTurnState()

			if eState == EndTurnButtonStates.END_TURN_OVER_HIGHLIGHT:
				screen.setEndTurnState("EndTurnButton", "Green")
				screen.showEndTurn("EndTurnButton")
			elif eState == EndTurnButtonStates.END_TURN_OVER_DARK:
				screen.setEndTurnState("EndTurnButton", "Disable")
				screen.showEndTurn("EndTurnButton")
			elif eState == EndTurnButtonStates.END_TURN_GO:
				screen.setEndTurnState("EndTurnButton", "Red")
				screen.showEndTurn("EndTurnButton")

			if self.CyPlayer.hasIdleCity():
				screen.show("EndTurnButtonDecal")

		else:
			screen.hideEndTurn("EndTurnButton")

	def updateWaitingForPlayer(self, iPlayer):
		text = self.szSystemWaiting % (str(iPlayer),)
		CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE).setLabel("WaitingForPlayer", "", self.aFontList[0] + text, 1<<2, self.xRes/2, self.yRes - 296, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, 0, 0)


	# Will redraw the interface
	def redraw(self):
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
		# Check Dirty Bits, see what we need to redraw... Flag:
		if CyIF.isDirty(InterfaceDirtyBits.Flag_DIRTY_BIT):
			print "Flag_DIRTY_BIT"
			self.iPlayer = iPlayerAct = GAME.getActivePlayer()
			self.iTeam = GAME.getActiveTeam()
			self.CyTeam = GC.getTeam(self.iTeam)
			self.CyPlayer = GC.getActivePlayer()
			if self.bInitMiMap:
				print "Init Mini Map"
				# Here it should create the minimap even on a secondary load game sequence.
				screen.initMinimap(self.xMinimap, self.xRes - 3, self.yMinimap, self.yRes - 3, -0.1)
				self.bInitMiMap = False
				# The EXE demands the helpTextArea defined for the main interface, modifyLabel/String() functions will crash the game without it.
				if self.bInitialize or self.iInterfaceType not in (InterfaceVisibility.INTERFACE_HIDE_ALL, InterfaceVisibility.INTERFACE_MINIMAP_ONLY):
					screen.setHelpTextArea(self.xMidL, FontTypes.GAME_FONT, 4, self.yBotBar, 0, False, "", True, False, 1<<0, 0)
					self.bHelpTextFullY = False
					self.bInitialize = False
				else:
					screen.setHelpTextArea(self.xMidL, FontTypes.GAME_FONT, 4, self.yRes - 8, 0, False, "", True, False, 1<<0, 0)
					self.bHelpTextFullY = True

				if self.bSetStartZoom:
					# CAMERA_START_DISTANCE also defines camera zoom where music is turned on/off, we want that to be quite low and the start zoom to be higher.
					# Max zoom change from game to game, so the percentage zoom is relative to the initial zoom from CAMERA_START_DISTANCE
					CAM = CyCamera()
					CAM.SetZoom(CAM.GetZoom() * 1.8)
					self.bSetStartZoom = False

			# This will update the flag widget for SP hotseat and debugging
			IFT = CyIF.getShowInterface()
			if IFT not in (InterfaceVisibility.INTERFACE_HIDE_ALL, InterfaceVisibility.INTERFACE_MINIMAP_ONLY, InterfaceVisibility.INTERFACE_ADVANCED_START):
				print "addFlagWidgetGFC"
				eWidGen = WidgetTypes.WIDGET_GENERAL
				screen.addFlagWidgetGFC("CivilizationFlag", self.xMidR + 6, self.yBotBar + 28, 80, self.hBotBar + 40, iPlayerAct, eWidGen, 1, 2)
				screen.setImageButton("CivilizationFlag0", "", self.xMidR + 8, self.yBotBar + 32, 76, self.hBotBar - 36, eWidGen, 1, 2)

			CyIF.setDirty(InterfaceDirtyBits.Flag_DIRTY_BIT, False)
		# Miscellaneous buttons (civics screen, etc)
		if CyIF.isDirty(InterfaceDirtyBits.MiscButtons_DIRTY_BIT):
			self.updateMiscButtons(screen)
			CyIF.setDirty(InterfaceDirtyBits.MiscButtons_DIRTY_BIT, False)
		# City screen
		if CyIF.isDirty(InterfaceDirtyBits.CityScreen_DIRTY_BIT):
			self.updateCityScreen()
			CyIF.setDirty(InterfaceDirtyBits.Domestic_Advisor_DIRTY_BIT, True)
			CyIF.setDirty(InterfaceDirtyBits.CityScreen_DIRTY_BIT, False)
		# Scores!
		if CyIF.isDirty(InterfaceDirtyBits.Score_DIRTY_BIT) or CyIF.checkFlashUpdate():
			self.updateScoreStrings(screen)
			CyIF.setDirty(InterfaceDirtyBits.Score_DIRTY_BIT, False)
		# Plot List Buttons
		if CyIF.isDirty(InterfaceDirtyBits.PlotListButtons_DIRTY_BIT):
			self.updatePlotListButtons(screen)
			CyIF.setDirty(InterfaceDirtyBits.PlotListButtons_DIRTY_BIT, False)
		# Selection Buttons
		if CyIF.isDirty(InterfaceDirtyBits.SelectionButtons_DIRTY_BIT):
			self.updateSelectionButtons(screen)
			CyIF.setDirty(InterfaceDirtyBits.SelectionButtons_DIRTY_BIT, False)
		# Info Pane
		if CyIF.isDirty(InterfaceDirtyBits.InfoPane_DIRTY_BIT):
			self.updateInfoPaneStrings(screen)
			CyIF.setDirty(InterfaceDirtyBits.InfoPane_DIRTY_BIT, False)
		# Research Buttons
		if CyIF.isDirty(InterfaceDirtyBits.ResearchButtons_DIRTY_BIT):
			self.updateResearchButtons(screen)
			CyIF.setDirty(InterfaceDirtyBits.ResearchButtons_DIRTY_BIT, False)
		# Citizen Buttons
		if CyIF.isDirty(InterfaceDirtyBits.CitizenButtons_DIRTY_BIT):
			if self.bCityScreen:
				self.updateCitizenButtons(screen)
			CyIF.setDirty(InterfaceDirtyBits.CitizenButtons_DIRTY_BIT, False)
		# Game Data Strings
		if CyIF.isDirty(InterfaceDirtyBits.GameData_DIRTY_BIT):
			self.updateGameDataStrings(screen)
			CyIF.setDirty(InterfaceDirtyBits.GameData_DIRTY_BIT, False)
		# Globelayer buttons
		if CyIF.isDirty(InterfaceDirtyBits.GlobeInfo_DIRTY_BIT):
			CyIF.setDirty(InterfaceDirtyBits.GlobeInfo_DIRTY_BIT, False)
			self.updateGlobeviewButtons(screen)
		# Percent Buttons
		if CyIF.isDirty(InterfaceDirtyBits.PercentButtons_DIRTY_BIT):
			self.updatePercentButtons(screen)
			CyIF.setDirty(InterfaceDirtyBits.PercentButtons_DIRTY_BIT, False)
		# Plot Help
		if CyIF.isDirty(InterfaceDirtyBits.Help_DIRTY_BIT):
			if CyIF.isFocused():
				screen.hide("PlotHelp")

			elif not self.bPlotHelpBan and not self.bTooltip and self.iInterfaceType not in (InterfaceVisibility.INTERFACE_HIDE_ALL, InterfaceVisibility.INTERFACE_MINIMAP_ONLY):
				self.updatePlotHelp(screen)

			CyIF.setDirty(InterfaceDirtyBits.Help_DIRTY_BIT, False)

		# Tooltip
		if self.bTooltip and self.bLockedTT:
			POINT = Win32.getCursorPos()
			iX = POINT.x + self.iOffsetTT[0]
			iY = POINT.y + self.iOffsetTT[1]
			if iX < 0: iX = 0
			if iY < 0: iY = 0
			screen.moveItem("Tooltip", iX, iY, 0)


	def resetEndTurnObjects(self):
		# Clears the end turn text, hides it and the button.
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
		screen.setEndTurnState("EndTurnText", "")
		screen.hideEndTurn("EndTurnText")
		screen.hideEndTurn("EndTurnButton")


	# Update the miscellaneous buttons
	def updateMiscButtons(self, screen):
		print "updateMiscButtons"
		screen.updateMinimapVisibility()
		CyPlayerAct = self.CyPlayer

		# Check City Selection
		city = CyIF.getHeadSelectedCity()
		InCityPre = self.InCity
		AtUnitPre = self.AtUnit
		if city:
			iCityID = city.getID()
			if not InCityPre or iCityID != InCityPre.iCityID:
				print "City selected"
				self.bFreshQueue = False
				self.cleanPlotList(screen)
				self.bCityChange = True
				self.bBuildWorkQueue = True
				self.bUpdateCityTab = True

				self.InCity = City(city, iCityID)

				if not InCityPre:
					self.bInverseShiftQueue = MainOpt.isInverseShiftForQueueing()
					if AtUnitPre:
						screen.hide("UnitButtons")
					screen.show("Conscript")
					screen.show("AutomateCitizens")
					screen.show("CS|AutomateProduction0")
					if self.InCity.iPlayer == self.iPlayer or self.bDebugMode:
						self.buildCitySelectionUI(screen, True, city)

		else: # No City Selected
			if InCityPre:
				print "City deselected"
				self.cleanPlotList(screen)
				# Remove potential Help Text
				if self.bTooltip:
					self.hideTooltip(screen)

				if self.cityOptions:
					self.cityOptions.exit(screen, self)

				screen.hide("CityTab0")
				screen.hide("CityTab1")
				screen.hide("CityTab2")
				screen.hide("CityTab3")
				# Completely rebuilt every time they are needed.
				screen.deleteWidget("CT|UnitGrouping")
				screen.deleteWidget("CT|UnitSorting")
				screen.deleteWidget("ProcessPanel")
				screen.deleteWidget("ScrollPanelBL")
				screen.deleteWidget("CityWorkQueue")
				screen.deleteWidget("CitizenPanel")
				if self.bCityScreen:
					screen.hide("PopulationBar")
					screen.hide("ProductionBar")
					screen.deleteWidget("PopulationText")
					screen.deleteWidget("PopulationInputText")
					screen.deleteWidget("ProductionText")
					screen.deleteWidget("CS|ProdYield0")
					screen.deleteWidget("TradeRouteTable")
					for i, entry in enumerate(InCityPre.listBonus):
						if entry:
							screen.deleteWidget("BonusListTable" + str(i))
					screen.deleteWidget("BldgListTable")
				screen.hideList(0)
				iTab = self.iCityTab
				if iTab > -1:
					self.exitCityTab(screen)
				self.InCity = None
				if AtUnitPre:
					screen.show("UnitButtons")

			CyUnit = CyIF.getHeadSelectedUnit()
			if CyUnit:
				self.CyPlayer.setForcedCityCycle(False);
				print "Unit Selected"
				iUnitID = CyUnit.getID()
				if not AtUnitPre or iUnitID != AtUnitPre.iUnitID:
					self.AtUnit = Unit(CyUnit, iUnitID)
			elif AtUnitPre:
				print "Unit deselected"
				self.AtUnit = None
				screen.deleteWidget("UnitButtons")
				if self.dataTT:
					self.hideTooltip(screen)

		# Check City Screen
		IFT = CyIF.getShowInterface()
		bCityScreen = CyIF.isCityScreenUp()
		if bCityScreen != self.bCityScreen:
			self.bCityScreen = bCityScreen
			if bCityScreen:
				# Initialize City Screen
				CyIF.clearSelectedCities() # Hack, the map camera FoV inside city screen is stuck at the current value.
				self.setFieldofView(50)
				CyIF.selectCity(city, False) # \Hack, City FoV is set and we can enter the city now.
				self.evalIFT(screen, IFT, bCityScreen, CyPlayerAct)
				self.bCityEnter = True
			else: # Return to map.
				self.setFieldofView(self.iField_View)
				self.evalIFT(screen, IFT, bCityScreen, CyPlayerAct)
				self.bCityExit = True
			CyIF.setDirty(InterfaceDirtyBits.GlobeInfo_DIRTY_BIT, True)
		# Check Globe View
		bGlobeview = ENGINE.isGlobeviewUp()
		if bGlobeview != self.bGlobeview:
			self.evalIFT(screen, IFT, bCityScreen, CyPlayerAct)
			self.bGlobeview = bGlobeview
		# Check InterfaceVisibility
		if IFT != self.iInterfaceType:
			self.evalIFT(screen, IFT, bCityScreen, CyPlayerAct)

		if CyIF.shouldDisplayFlag() and IFT == InterfaceVisibility.INTERFACE_SHOW:
			screen.show("CivilizationFlag")
			screen.show("CivilizationFlag0")
		else:
			screen.hide("CivilizationFlag")
			screen.hide("CivilizationFlag0")

	def evalIFT(self, screen, IFT, bCityScreen, CyPlayerAct):

		print IFT
		self.iInterfaceType = IFT

		if IFT == InterfaceVisibility.INTERFACE_SHOW:
			screen.moveItem("EndTurnText", 0, self.yRes - 256, 0)
			if self.bHelpTextFullY:
				screen.setHelpTextArea(self.xRes/3, FontTypes.GAME_FONT, 4, self.yBotBar, 0, False, "", True, False, 1<<0, 0)
				self.bHelpTextFullY = False
			screen.show("InterfaceLeftBackgroundWidget")
			screen.show("InterfaceTopBackgroundWidget")
			screen.show("InterfaceCenterBackgroundWidget")
			screen.show("InterfaceRightBackgroundWidget")
			screen.show("MiniMapPanel")
			screen.show("CityScrollMinus")
			screen.show("CityScrollPlus")
			screen.show("MainMenuButton")
			screen.show("PediaButton0")
			screen.show("AdvBtn1")
			if self.InCity:
				screen.show("CityWorkQueue")
				screen.show("CityTab0")
				screen.show("CityTab1")
				screen.show("CityTab2")
				screen.show("CityTab3")
				screen.show("ProcessPanel")
				screen.show("ScrollPanelBL")
				screen.show("Conscript")
				screen.show("AutomateCitizens")
				screen.show("CS|AutomateProduction0")

			if bCityScreen:
				screen.hide("InterfaceTopLeft")
				screen.hide("InterfaceTopCenter")
				screen.hide("InterfaceTopRight")
				screen.hide("TurnLogBtn")
				screen.hide("AdvBtn0")
				screen.hide("AdvBtn10")
				screen.hide("AdvBtn2")
				screen.hide("AdvBtn3")
				screen.hide("AdvBtn4")
				screen.hide("AdvBtn5")
				screen.hide("AdvBtn6")
				screen.hide("AdvBtn7")
				screen.hide("AdvBtn8")
				screen.hide("AdvBtn9")
				screen.hide("VictoryAdvBtn")
				screen.hide("InfoAdvBtn")
				screen.hide("OptionsBtnBUG0")
				screen.hide("DebugBtn0")
				screen.hide("FoVSliderText")
				screen.hide("FoVSlider")
				screen.hide("BuildListBtn0")
				screen.hide("ParallelMapsBtn")
			else:
				screen.show("InterfaceTopLeft")
				screen.show("InterfaceTopCenter")
				screen.show("InterfaceTopRight")
				screen.show("TurnLogBtn")
				screen.show("AdvBtn0")
				screen.show("AdvBtn10")
				screen.show("AdvBtn2")
				screen.show("AdvBtn3")
				screen.show("AdvBtn4")
				screen.show("AdvBtn5")
				screen.show("AdvBtn6")
				screen.show("AdvBtn7")
				screen.show("AdvBtn8")
				screen.show("AdvBtn9")
				screen.show("VictoryAdvBtn")
				screen.show("InfoAdvBtn")
				screen.show("OptionsBtnBUG0")
				if self.bDebugModePython:
					screen.show("DebugBtn0")

				if MainOpt.isShowFieldOfView():
					screen.show("FoVSliderText")
					screen.show("FoVSlider")
				else:
					screen.hide("FoVSliderText")
					screen.hide("FoVSlider")
				screen.show("BuildListBtn0")
				if GC.getNumMapsInitialized() > 1:
					screen.show("ParallelMapsBtn")

		elif IFT == InterfaceVisibility.INTERFACE_HIDE:
			screen.moveItem("EndTurnText", 0, self.yRes - 128, 0)
			screen.show("MainMenuButton")
			screen.show("PediaButton0")
			screen.show("AdvBtn1")
			screen.hide("InterfaceLeftBackgroundWidget")
			screen.show("InterfaceTopBackgroundWidget")
			screen.hide("InterfaceCenterBackgroundWidget")
			screen.hide("InterfaceRightBackgroundWidget")
			screen.hide("MiniMapPanel")
			screen.hide("CityScrollMinus")
			screen.hide("CityScrollPlus")
			screen.hide("InterfaceTopLeft")
			screen.show("InterfaceTopCenter")
			screen.hide("InterfaceTopRight")
			screen.hide("TurnLogBtn")
			screen.hide("AdvBtn0")
			screen.hide("AdvBtn10")
			screen.hide("AdvBtn2")
			screen.hide("AdvBtn3")
			screen.hide("AdvBtn4")
			screen.hide("AdvBtn5")
			screen.hide("AdvBtn6")
			screen.hide("AdvBtn7")
			screen.hide("AdvBtn8")
			screen.hide("AdvBtn9")
			screen.hide("VictoryAdvBtn")
			screen.hide("InfoAdvBtn")
			screen.hide("OptionsBtnBUG0")
			screen.hide("DebugBtn0")
			screen.hide("FoVSliderText")
			screen.hide("FoVSlider")
			screen.hide("BuildListBtn0")
			screen.hide("ParallelMapsBtn")

		elif IFT in (InterfaceVisibility.INTERFACE_HIDE_ALL, InterfaceVisibility.INTERFACE_MINIMAP_ONLY):
			screen.moveItem("EndTurnText", 0, self.yRes - 128, 0)
			screen.hide("PlotHelp")
			self.hidePlotList(screen)
			if not self.bHelpTextFullY:
				screen.setHelpTextArea(self.xRes/3, FontTypes.GAME_FONT, 4, self.yRes - 8, 0, False, "", True, False, 1<<0, 0)
				self.bHelpTextFullY = True
			screen.hideList(0)
			if self.InCity:
				screen.hide("CityWorkQueue")
				screen.hide("CityTab0")
				screen.hide("CityTab1")
				screen.hide("CityTab2")
				screen.hide("CityTab3")
				screen.hide("ProcessPanel")
				screen.hide("ScrollPanelBL")
			screen.hide("InterfaceLeftBackgroundWidget")
			screen.hide("InterfaceTopBackgroundWidget")
			screen.hide("InterfaceCenterBackgroundWidget")
			screen.hide("InterfaceRightBackgroundWidget")
			screen.hide("MiniMapPanel")
			screen.hide("InterfaceTopLeft")
			screen.hide("InterfaceTopCenter")
			screen.hide("InterfaceTopRight")
			screen.hide("MainMenuButton")
			screen.hide("Treasury")
			screen.hide("PediaButton0")
			screen.hide("TurnLogBtn")
			screen.hide("AdvBtn1")
			screen.hide("AdvBtn0")
			screen.hide("AdvBtn10")
			screen.hide("AdvBtn2")
			screen.hide("AdvBtn3")
			screen.hide("AdvBtn4")
			screen.hide("AdvBtn5")
			screen.hide("AdvBtn6")
			screen.hide("AdvBtn7")
			screen.hide("AdvBtn8")
			screen.hide("AdvBtn9")
			screen.hide("VictoryAdvBtn")
			screen.hide("InfoAdvBtn")
			screen.hide("CityScrollMinus")
			screen.hide("CityScrollPlus")
			screen.hide("OptionsBtnBUG0")
			screen.hide("DebugBtn0")
			screen.hide("FoVSliderText")
			screen.hide("FoVSlider")
			screen.hide("BuildListBtn0")
			screen.hide("ParallelMapsBtn")
			screen.hide("ResearchBar")
			screen.hide("ResearchBarDC")
			screen.hide("WID|TECH|ProgBar0")
			screen.hide("WID|TECH|ProgBar1")
			screen.hide("GreatPersonBar")
			screen.hide("GreatPersonBar0")
			screen.hide("GreatPersonBar1")
			screen.hide("GreatGeneralBar")
			screen.hide("GreatGeneralBarText")
			screen.hide("UnitButtons")

		elif IFT == InterfaceVisibility.INTERFACE_ADVANCED_START:
			screen.moveItem("EndTurnText", 0, self.yRes - 128, 0)
			if self.bHelpTextFullY:
				screen.setHelpTextArea(self.xRes/3, FontTypes.GAME_FONT, 4, self.yBotBar, 0, False, "", True, False, 1<<0, 0)
				self.bHelpTextFullY = False
			screen.hide("InterfaceLeftBackgroundWidget")
			screen.hide("InterfaceTopBackgroundWidget")
			screen.hide("InterfaceCenterBackgroundWidget")
			screen.hide("InterfaceRightBackgroundWidget")
			screen.hide("InterfaceTopLeft")
			screen.hide("InterfaceTopCenter")
			screen.hide("InterfaceTopRight")
			screen.hide("Treasury")
			screen.hide("PediaButton0")
			screen.hide("TurnLogBtn")
			screen.hide("AdvBtn1")
			screen.hide("AdvBtn0")
			screen.hide("AdvBtn10")
			screen.hide("AdvBtn2")
			screen.hide("AdvBtn3")
			screen.hide("AdvBtn4")
			screen.hide("AdvBtn5")
			screen.hide("AdvBtn6")
			screen.hide("AdvBtn7")
			screen.hide("AdvBtn8")
			screen.hide("AdvBtn9")
			screen.hide("VictoryAdvBtn")
			screen.hide("InfoAdvBtn")
			screen.hide("CityScrollMinus")
			screen.hide("CityScrollPlus")
			screen.hide("OptionsBtnBUG0")
			screen.hide("DebugBtn0")
			screen.hide("BuildListBtn0")
			screen.hide("ParallelMapsBtn")
			if MainOpt.isShowFieldOfView():
				screen.show("FoVSliderText")
				screen.show("FoVSlider")
			else:
				screen.hide("FoVSliderText")
				screen.hide("FoVSlider")
			screen.hide("ResearchBar")
			screen.hide("ResearchBarDC")
			screen.hide("WID|TECH|ProgBar0")
			screen.hide("WID|TECH|ProgBar1")
			screen.hide("GreatPersonBar")
			screen.hide("GreatPersonBar0")
			screen.hide("GreatPersonBar1")
			screen.hide("GreatGeneralBar")
			screen.hide("GreatGeneralBarText")

	def showParallelMapsScreenButton(self):
		CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE).show("ParallelMapsBtn")

	def cleanPlotList(self, screen):
		self.hidePlotList(screen)
		screen.deleteWidget("PlotListPanel")
		self.iMaxPlotListScroll = 0
		self.iPlotListTopRow = 0
		self.aPlotListList = []

	def hidePlotList(self, screen):
		screen.hide("PlotListPanel")
		screen.hide("PlotList|Scroll0")
		screen.hide("PlotList|Scroll1")
		screen.hide("PlotListScroll")

	def updatePlotListButtons(self, screen):
		print "updatePlotListButtons"
		if self.iInterfaceType == InterfaceVisibility.INTERFACE_HIDE_ALL:
			return

		CyPlot = CyIF.getSelectionPlot()
		if not CyPlot:
			self.cleanPlotList(screen)
			return

		# Prepare the basics.
		iPlayerAct = self.iPlayer
		iTeamAct = self.iTeam
		iUnits = CyPlot.getNumUnits()
		x = self.xMidL
		bCityScreen	= self.bCityScreen
		iMaxRows	= self.iPlotListMaxRows
		iSize0		= self.iPlotListIconSize
		iSize1		= iSize0 + 1
		iSize4		= iSize0 + 4
		iSize6		= iSize0 + 6
		iSize8		= iSize0 + 8
		if bCityScreen:
			w = self.xMidR - x
			iMaxCols = w / iSize4
			iMaxUnits = iMaxCols
		else:
			if self.xScoreboard < self.xMidR:
				w = self.xScoreboard - x
			else:
				w = self.xMidR - x
			iMaxCols = w / iSize4
			iMaxUnits = iMaxCols * iMaxRows

		# Auto-scroll?
		if self.bPlotListAutoScroll:
			self.cleanPlotList(screen)
			iTopRow = 0
			if not self.InCity:
				# Find selected unit list position
				i = iPos = 0
				while i < iUnits:
					CyUnit = CyPlot.getUnit(i)
					if self.bDebugMode or not CyUnit.isInvisible(iTeamAct, False):
						if CyUnit.IsSelected():
							if iPos >= iMaxUnits:
								self.iPlotListTopRow = iTopRow = 1 + (iPos - iMaxUnits)/iMaxCols
							break
						iPos += 1
					i += 1
		else:
			iTopRow = self.iPlotListTopRow
			self.bPlotListAutoScroll = True
		# Collect data about plot list.
		aMap = {}
		aList = []
		iStart = iTopRow * iMaxCols
		i = iPos = iStart
		while i < iUnits and iPos - iStart < iMaxUnits:
			CyUnit = CyPlot.getUnit(i)
			i += 1
			if not self.bDebugMode and CyUnit.isInvisible(iTeamAct, False): continue
			iPos += 1
			iUnitType = CyUnit.getUnitType()

			bEnable = CyUnit.getOwner() == iPlayerAct
			bSelected = CyUnit.IsSelected()

			if CyUnit.isInBattle():
				bShowHealth = False
			elif CyUnit.getDomainType() == DomainTypes.DOMAIN_AIR:
				bShowHealth = CyUnit.canAirAttack()
			else:
				bShowHealth = CyUnit.canFight()
			if bShowHealth:
				fHP = CyUnit.getHP() / float(CyUnit.getMaxHP())
			else:
				fHP = 0

			bUpg = False
			szActivity = ""
			if bEnable:
				bWaiting = CyUnit.isWaiting()
				if bWaiting:
					iDot0 = 0 # "OVERLAY_FORTIFY"
				elif CyUnit.canMove():
					if CyUnit.hasMoved():
						iDot0 = 2 # "OVERLAY_HASMOVED"
					else:
						iDot0 = 4 # "OVERLAY_MOVE"
				else:
					iDot0 = 6 # "OVERLAY_NOMOVE"
				if fHP in (0, 1):
					iDot1 = 0
				else: iDot1 = 1 # "_INJURED"

				eType = CyUnit.getGroup().getActivityType()
				# is unit on air intercept mission
				if eType == ActivityTypes.ACTIVITY_INTERCEPT:
					szActivity = self.artPathOverlayIntercept
				# is unit on boat patrol coast mission
				elif eType == ActivityTypes.ACTIVITY_PATROL:
					szActivity = self.artPathOverlayPatrol
				# is unit on boat blockade mission
				elif eType == ActivityTypes.ACTIVITY_PLUNDER:
					szActivity = self.artPathOverlayPlunder
				# is unit fortified for healing (wake up when healed)
				elif eType == ActivityTypes.ACTIVITY_HEAL:
					szActivity = self.artPathOverlayHeal
				# is unit sentry (wake up when enemy in sight)
				elif eType == ActivityTypes.ACTIVITY_SENTRY:
					szActivity = self.artPathOverlaySentry
				# is the turn for this unit skipped (wake up next turn)
				elif eType == ActivityTypes.ACTIVITY_HOLD:
					szActivity = self.artPathOverlaySkip
				if not szActivity:
					eType = CyUnit.getGroup().getAutomateType()
					# has unit exploration mission
					if eType == AutomateTypes.AUTOMATE_EXPLORE:
						szActivity = self.artPathOverlayExplore
					# is unit automated generally (only worker units)
					elif eType == AutomateTypes.AUTOMATE_BUILD:
						szActivity = self.artPathOverlayAutoBuild
					# is unit automated for nearest city (only worker units)
					elif eType == AutomateTypes.AUTOMATE_CITY:
						szActivity = self.artPathOverlayAutoCity
					# is unit automated for network (only worker units)
					elif eType == AutomateTypes.AUTOMATE_NETWORK:
						szActivity = self.artPathOverlayAutoNetwork
					# is unit automated spread religion (only missionary units)
					elif eType == AutomateTypes.AUTOMATE_RELIGION:
						szActivity = self.artPathOverlayAutoReligion
					# has unit a mission
					elif CyUnit.getGroup().getLengthMissionQueue() > 0:
						eType = CyUnit.getGroup().getMissionType(0)
						if eType == MissionTypes.MISSION_BUILD:
							szActivity = self.artPathOverlayBuild
						elif eType in (MissionTypes.MISSION_MOVE_TO, MissionTypes.MISSION_MOVE_TO_UNIT):
							szActivity = self.artPathOverlayGoTo
					# if none of the above and unit is waiting...
					elif bWaiting:
						if CyUnit.isFortifyable():
							szActivity = self.artPathOverlayActionFortify
						else:
							szActivity = self.artPathOverlaySleep
				bPromo = CyUnit.isPromotionReady()
			else:
				iDot0 = -1
				iDot1 = -1
				bPromo = False

			if iUnitType not in aMap:
				if bEnable:
					for j in xrange(GC.getNumUnitInfos()):
						if CyUnit.canUpgrade(j, True):
							bUpg = True
							break
				else:
					bUpg = False
				aMap[iUnitType] = [GC.getUnitInfo(iUnitType).getButton(), bUpg]
			else:
				bUpg = aMap[iUnitType][1]

			aList.append([CyUnit, iUnitType, bEnable, bSelected, fHP, [iDot0, iDot1], szActivity, bPromo, bUpg])

		if aList == self.aPlotListList:
			return

		# Total visible units count
		i = iVisibleUnits = 0
		while i < iUnits:
			CyUnit = CyPlot.getUnit(i)
			if self.bDebugMode or not CyUnit.isInvisible(iTeamAct, False):
				iVisibleUnits += 1
			i += 1
		# Scroll buttons?
		if iMaxUnits < iVisibleUnits:
			screen.show("PlotListScroll")
			if iTopRow:
				screen.show("PlotList|Scroll0")
			self.iMaxPlotListScroll = 1 + (iVisibleUnits-1-iMaxUnits)/iMaxCols
			if iTopRow < self.iMaxPlotListScroll:
				screen.show("PlotList|Scroll1")
		else:
			screen.hide("PlotList|Scroll0")
			screen.hide("PlotList|Scroll1")
			screen.hide("PlotListScroll")
		self.aPlotListList = aList

		eWidGen = WidgetTypes.WIDGET_GENERAL
		Area = "PlotListPanel"
		y = self.yBasePlotlist
		h = iMaxRows * iSize8
		screen.setImageButton(Area, "", x, y, w, h, eWidGen, 1, 1)
		screen.setHitTest(Area, HitTestTypes.HITTEST_CHILDREN)

		artPathListUnitStatusDot = self.artPathListUnitStatusDot
		cCoRed		= self.COLOR_RED
		cCoYellow	= self.COLOR_YELLOW
		cCoGreen	= self.COLOR_GREEN
		artPathHilite = self.artPathHilite
		artPathPromo = self.artPathPromotion
		artPathUpg = self.artPathUpgrade
		iLeaderPromo = self.PROMOTION_LEADER

		# Draw plot list
		iDisplayedUnits = len(aList)
		i = iMaxRows - 1
		if bCityScreen:
			y0 = iSize6 * i
		elif iDisplayedUnits <= i * iMaxCols:
			y0 = iSize6 * (i - (iDisplayedUnits - 1)/iMaxCols)
		else:
			y0 = 0
		self.yPlotListTT = y + y0
		halfSize = iSize0/2
		actSize = iSize6/3
		upgSize = iSize6/2
		i = 0
		while i < iDisplayedUnits:
			CyUnit, iUnitType, bEnable, bSelected, fHP, iDot, szActivity, bPromo, bUpg = aList[i]
			iDot0, iDot1 = iDot
			x = (i % iMaxCols) * iSize4
			y = y0 + (i / iMaxCols) * iSize8
			PF = "PlotList|Button" + str(i)
			if bSelected:
				DDS = PF + "Selection"
				screen.addDDSGFCAt(DDS, Area, artPathHilite, x, y+2, iSize4, iSize4, eWidGen, 1, 1, False)
				screen.setHitTest(DDS, HitTestTypes.HITTEST_NOHIT)
			screen.setImageButtonAt(PF, Area, aMap[iUnitType][0], x+2, y+4, iSize0, iSize0, eWidGen, 1, 1)
			if bEnable:
				if bPromo:
					DDS = PF + "PromoFrame"
					screen.addDDSGFCAt(DDS, Area, artPathPromo, x+3, y+5, iSize0-2, iSize0-2, eWidGen, 1, 1, False)
					screen.setHitTest(DDS, HitTestTypes.HITTEST_NOHIT)
				# Units lead by a GG will get a star instead of a dot.
				iS = actSize
				if iLeaderPromo != -1 and CyUnit.isHasPromotion(iLeaderPromo):
					iDot0 += 1 # "_GG"
					iS += 4
					iOff = 3
				else:
					iOff = 0
				# display the colored spot icon
				artPath = artPathListUnitStatusDot[iDot0][iDot1]
				DDS = PF + "Icon"
				screen.addDDSGFCAt(DDS, Area, artPath, x - iOff, y - iOff, iS, iS, eWidGen, 1, 1, False)
				screen.setHitTest(DDS, HitTestTypes.HITTEST_NOHIT)

				if bUpg:
					DDS = PF + "Upgrade"
					screen.addDDSGFCAt(DDS, Area, artPathUpg, x+4, y+halfSize, upgSize/2, upgSize, eWidGen, 1, 1, False)
					screen.setHitTest(DDS, HitTestTypes.HITTEST_NOHIT)

				if szActivity:
					DDS = PF + "ActionIcon"
					screen.addDDSGFCAt(DDS, Area, szActivity, x+iSize0-10, y+2, actSize, actSize, eWidGen, 1, 1, False)
					screen.setHitTest(DDS, HitTestTypes.HITTEST_NOHIT)

			else:
				screen.enable(PF, False)
			# place the health bar
			if fHP:
				Bar = PF + "Health"
				screen.addStackedBarGFCAt(Bar, Area, x+2, y+iSize0, iSize1, 11, InfoBarTypes.NUM_INFOBAR_TYPES, eWidGen, 1, 1)
				screen.setBarPercentage(Bar, InfoBarTypes.INFOBAR_STORED, fHP)
				if fHP < 0.33:
					iColor = cCoRed
				elif fHP < 0.66:
					iColor = cCoYellow
				else:
					iColor = cCoGreen
				screen.setStackedBarColors(Bar, InfoBarTypes.INFOBAR_STORED, iColor)
				screen.setImageButtonAt("PlotList|Health" + str(i), Area, "", x+2, y+iSize0, iSize1, 11, eWidGen, 1, 1)
			i += 1


	def updateSelectionButtons(self, screen):
		print "updateSelectionButtons"
		IFT = self.iInterfaceType
		if IFT in (InterfaceVisibility.INTERFACE_HIDE_ALL, InterfaceVisibility.INTERFACE_MINIMAP_ONLY): return

		InCity = self.InCity
		AtUnit = self.AtUnit

		if InCity:
			city = InCity.CyCity
			bMyCity = InCity.iPlayer == self.iPlayer
			bOfInterest = (bMyCity or self.bDebugMode)

			iTab = self.iCityTab
			if self.bUpdateCityTab:
				if self.iCityTab < 0:
					if bMyCity and not city.isProduction():
						self.openCityTab(screen, CITYTAB_ADMIN)
					else:
						print "[WARN] self.bUpdateCityTab unnecessarily set to 'True'"
				else:
					self.updateCityTab(screen)

				self.bUpdateCityTab = False


			if self.bCityChange:
				if iTab > -1:
					self.openCityTab(screen, iTab)
				self.bCityChange = False

			bCityScreen = self.bCityScreen

			# Automate
			if bMyCity:
				screen.setState("AutomateCitizens", city.isCitizensAutomated())
				screen.setState("CS|AutomateProduction0", city.isProductionAutomated())

			screen.enable("AutomateCitizens", bMyCity)
			screen.enable("CS|AutomateProduction0", bMyCity)

			# Emphasize
			iNumEmphasizeInfos = self.iNumEmphasizeInfos
			for i in xrange(iNumEmphasizeInfos):
				if bCityScreen or i < (iNumEmphasizeInfos - 2):
					szButtonID = "Emphasize" + str(i)
					screen.setState(szButtonID, city.AI_isEmphasize(i))
					screen.show(szButtonID)

			# Hurry
			for i in xrange(self.iNumHurryInfos):
				szButtonID = "Hurry" + str(i)
				screen.show(szButtonID)
				screen.enable(szButtonID, city.canHurry(i, False))

			# Draft
			screen.enable("Conscript", bOfInterest and city.canConscript())

		elif AtUnit:
			CyUnit = AtUnit.CyUnit
			# Unit action list
			y = self.yBotBar + 60
			h = self.yRes - y - 2

			w = self.xMidR - self.xMidL - 4
			iSize = MainOpt.getUnitIconSize()
			MuLi = "UnitButtons"
			screen.addMultiListControlGFC(MuLi, "", self.xMidL, y, w, h, 1, iSize, iSize, TableStyles.TABLE_STYLE_STANDARD)

			actions = CyIF.getActionsToShow()
			for i, iType in enumerate(actions):
				BTN = GC.getActionInfo(iType).getButton()
				screen.appendMultiListButton(MuLi, BTN, 0, WidgetTypes.WIDGET_ACTION, iType, 1, False)
				if not CyIF.canHandleAction(iType, False):
					screen.disableMultiListButton(MuLi, 0, i, BTN)
				if CyUnit.isActionRecommended(iType):
					screen.enableMultiListPulse(MuLi, True, 0, i)
				else:
					screen.enableMultiListPulse(MuLi, False, 0, i)

			if CyIF.canCreateGroup():
				screen.appendMultiListButton(MuLi, self.artPathButtonGroupCreate, 0, WidgetTypes.WIDGET_CREATE_GROUP, 0, 1, False)
			if CyIF.canDeleteGroup():
				screen.appendMultiListButton(MuLi, self.artPathButtonGroupSplit, 0, WidgetTypes.WIDGET_DELETE_GROUP, 0, 1, False)


	# Will update the research buttons
	def updateResearchButtons(self, screen):
		print "updateResearchButtons"
		IFT = self.iInterfaceType
		player = self.CyPlayer
		if player.getCurrentResearch() == -1:
			if GAME.GetWorldBuilderMode() or IFT not in (InterfaceVisibility.INTERFACE_SHOW, InterfaceVisibility.INTERFACE_HIDE):
				pass
			elif not self.shownResearchSelection:
				eWidGen = WidgetTypes.WIDGET_GENERAL
				bCanFoundReligion = player.canFoundReligion()
				xywh = self.xywhTechBar
				iNumTechBtnPerRow = xywh[2] / 50
				aList = []
				iCount = 0
				xStart = xywh[0] + 8
				aMap = self.aRel2TechMap
				team = self.CyTeam
				for i in xrange(team.getNumAdjacentResearch()):
					iTechX = team.getAdjacentResearch(i)
					# Toffer - canResearch can fail as adjacent research is cached even...
					#	if special requirements like building requirements are not met.
					if player.canResearch(iTechX, True, True):
						szName = "WID|TECH|Selection" + str(iTechX)
						if iTechX in aMap:
							if not bCanFoundReligion or GAME.countKnownTechNumTeams(iTechX):
								artPath = aMap[iTechX].getGenericTechButton()
								if not artPath:
									artPath = GC.getTechInfo(iTechX).getButton()
							else:
								artPath = aMap[iTechX].getTechButton()
						else:
							artPath = GC.getTechInfo(iTechX).getButton()
						# Set the selection button position
						x = xStart + 50 * (iCount % iNumTechBtnPerRow)
						y = 50 * (iCount / iNumTechBtnPerRow)
						screen.setImageButton(szName, artPath, x, y, 48, 48, eWidGen, 0, 0)
						aList.append(szName)
						iCount += 1
				self.shownResearchSelection = aList
				return
			else:
				return
		for szName in self.shownResearchSelection:
			screen.deleteWidget(szName)
		self.shownResearchSelection = []


	# Will update the game data strings
	def updateGameDataStrings(self, screen):
		print "updateGameDataStrings"
		bCityScreen = self.bCityScreen
		uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2, uFont1b, uFont1 = self.aFontList
		InCity = self.InCity
		if InCity:
			iPlayer = InCity.iPlayer
		else:
			iPlayer = self.iPlayer
		if iPlayer < 0 or iPlayer >= self.iMaxPlayers: return

		iRange = CommerceTypes.NUM_COMMERCE_TYPES
		for i in xrange(iRange):
			szString = "PercentText" + str(i)
			screen.hide(szString)
			szString = "RateText" + str(i)
			screen.hide(szString)
		IFT = self.iInterfaceType
		if IFT in (InterfaceVisibility.INTERFACE_SHOW, InterfaceVisibility.INTERFACE_HIDE):
			CyPlayer = GC.getPlayer(iPlayer)
			# Percent of commerce
			if CyPlayer.isAlive():
				eWidGen = WidgetTypes.WIDGET_GENERAL
				eFontGame = FontTypes.GAME_FONT
				iCurrentResearch = CyPlayer.getCurrentResearch()
				if iCurrentResearch != -1 or not bCityScreen:
					iResearchMod = CyPlayer.calculateResearchModifier(iCurrentResearch)
					iResearchRate = CyPlayer.calculateResearchRate(iCurrentResearch)

				if bCityScreen:
					iconHappy = self.iconHappy
					iconUnhappy = self.iconUnhappy
				else:
					bTDDisplayOption = self.GO_WIN_FOR_LOSING or self.GO_TECH_DIFFUSION

				iconCommerceList = self.iconCommerceList
				dY = 0
				for i in xrange(iRange):
					j = (i + 1) % iRange # 1, 2, 3, 0 (Research, Culture, Espionage, Gold)

					if bCityScreen or CyPlayer.isCommerceFlexible(j):
						szTxt = uFont2 + iconCommerceList[j] + uFont1b + " " + str(CyPlayer.getCommercePercent(j)) + "%"
						screen.setLabel("PercentText" + str(i), "", szTxt, 1<<0, 6, 53 + dY, 0, eFontGame, eWidGen, 0, 0)

						if bCityScreen:
							CyCity = InCity.CyCity
							iHappiness = CyCity.getCommerceHappinessByType(j)
							if iHappiness:
								if iHappiness < 0:
									icon = iconUnhappy
								else: icon = iconHappy

								szTxt = "<font=1>%d<font=2>%s, " %(iHappiness, icon)
							else: szTxt = ""

							szTxt += "<font=1>%.2f<font=2>" %(CyCity.getCommerceRateTimes100(j)/100.0) + iconCommerceList[j]

							screen.setLabel("CityPercentText" + str(i), "", szTxt, 1<<1, 252, 53 + dY, 0, eFontGame, WidgetTypes.WIDGET_COMMERCE_MOD_HELP, j, -1)
						else:
							if j != CommerceTypes.COMMERCE_RESEARCH:
								commerceRate = CyPlayer.getCommerceRate(CommerceTypes(j))

							elif bTDDisplayOption:
								commerceRate = getModifiedIntValue(iResearchRate, -iResearchMod)
								techDiffusionHelp = iResearchRate - commerceRate
							else:
								commerceRate = iResearchRate

							if bTDDisplayOption and j == CommerceTypes.COMMERCE_RESEARCH and iResearchMod > 0:
								szTxt = TRNSLTR.getText("TXT_KEY_MISC_POS_GOLD_PER_TURN_TD_WFL", (commerceRate, techDiffusionHelp))
							else:
								szTxt = TRNSLTR.getText("TXT_KEY_MISC_POS_GOLD_PER_TURN", (commerceRate,))
							screen.setLabel("RateText" + str(i), "", uFont1b + szTxt, 1<<0, 108, 53 + dY, 0, eFontGame, eWidGen, 0, 0)
						dY += 18
				# Treasury and income info.
				iGold = CyPlayer.getGold()
				iGoldRate = CyPlayer.calculateGoldRate()
				szTxt = "<font=3>"
				if iGold < 0:
					szTxt += "<color=255,0,0,255>"
				szTxt += str(iGold)
				if iGoldRate:
					szTxt += " <color="
					if iGoldRate > 0 and iGold + iGoldRate >= 0:
						szTxt += "127,255,0,255>"
					elif iGoldRate < 0 and iGold + iGoldRate < 0:
						szTxt += "255,124,0,255>"
					else:
						szTxt += "255,227,0,255>"
					if iGoldRate > 0:
						szTxt += "+ "
					else:
						szTxt += "- "
						iGoldRate = -iGoldRate
					szTxt += str(iGoldRate)
				if CyPlayer.isStrike():
					szTxt += TRNSLTR.getText("TXT_KEY_MISC_STRIKE", ())
				screen.modifyLabel("Treasury", szTxt, 1<<0)
				screen.hide("Treasury")
				screen.show("Treasury")

				iEra = CyPlayer.getCurrentEra()
				CvEraInfo = GC.getEraInfo(iEra)
				BTN = CvEraInfo.getButton()
				if BTN:
					screen.setText("EraIndicator0", "", "<img=%s>" % BTN, 1<<1, self.xMidL - 8, 0, 0, eFontGame, eWidGen, iEra, 0)
				else:
					szTxt = CvEraInfo.getDescription()
					screen.setText("EraIndicator0", "", "<font=3>%s" % szTxt[:3], 1<<1, self.xMidL - 14, 0, 0, eFontGame, eWidGen, iEra, 0)

				if iCurrentResearch == -1:
					screen.hide("ResearchBar")
					screen.hide("ResearchBarDC")
					screen.hide("WID|TECH|ProgBar0")
					screen.hide("WID|TECH|ProgBar1")
				else:
					x, y, w, h = self.xywhTechBar
					screen.setImageButton("WID|TECH|ProgBar0", "", x, y, w, h, eWidGen, iCurrentResearch, 0)
					x += w / 2
					if CyPlayer.isAnarchy():
						szTxt = TRNSLTR.getText("INTERFACE_ANARCHY", (CyPlayer.getAnarchyTurns(),))
						screen.setText("WID|TECH|ProgBar1", "", szTxt, 1<<2, x, 2, 0, eFontGame, eWidGen, iCurrentResearch, 0)
					elif iCurrentResearch != -1:

						CyTeam = GC.getTeam(CyPlayer.getTeam())
						researchProgress = CyTeam.getResearchProgress(iCurrentResearch)
						iOverflow = getModifiedIntValue(CyPlayer.getOverflowResearch(), iResearchMod)
						researchCost = CyTeam.getResearchCost(iCurrentResearch)
						iCurr = researchProgress + iOverflow

						screen.setBarPercentage("ResearchBar", InfoBarTypes.INFOBAR_STORED, iCurr * 1.0 / researchCost)
						self.researchBarDC.drawTickMarks(screen, 0, 0, 0) # remove old tick marks

						if iResearchRate > 0 and researchCost > iCurr:
							# Ticks - only bother with a 100 or less
							if iResearchRate * 100 / (researchCost - iCurr) >= 1:
								self.researchBarDC.drawTickMarks(screen, iCurr, researchCost, iResearchRate)
							screen.setBarPercentage("ResearchBar", InfoBarTypes.INFOBAR_RATE, iResearchRate * 1.0 / (researchCost - researchProgress - iOverflow))
						else:
							screen.setBarPercentage("ResearchBar", InfoBarTypes.INFOBAR_RATE, 0)
						screen.show("ResearchBar")

						szTxt = GC.getTechInfo(iCurrentResearch).getDescription() + " (" + str(CyPlayer.getResearchTurnsLeft(iCurrentResearch, True)) + ')'
						screen.setText("WID|TECH|ProgBar1", "", szTxt, 1<<2, x, 2, 0, eFontGame, eWidGen, iCurrentResearch, 0)

				# Great General Bar
				CyPlayer = self.CyPlayer
				iCombatExp = CyPlayer.getCombatExperience()
				iThresholdExp = CyPlayer.greatPeopleThresholdMilitary()

				szTxt = self.iconGreatGeneral + " (" + str(iThresholdExp - iCombatExp) + ")"

				screen.setText("GreatGeneralBarText", "", szTxt, 1<<2, self.xMidBarTopGG, 2, 0, eFontGame, WidgetTypes.WIDGET_HELP_GREAT_GENERAL, -1, -1)

				fProgress = float(iCombatExp) / float(iThresholdExp)
				screen.setBarPercentage("GreatGeneralBar", InfoBarTypes.INFOBAR_STORED, fProgress)
				screen.show("GreatGeneralBar")

				# Great People Bar
				if not bCityScreen:
					city, iTurns = GPUtil.getDisplayCity()
					x, y, w, h = self.xywhGPBar
					screen.setImageButton("GreatPersonBar0", "", x, y, w, h, eWidGen, 0, 0)
					szTxt = GPUtil.getGreatPeopleText(city, iTurns, w - 32, MainOpt.isGPBarTypesNone(), MainOpt.isGPBarTypesOne(), True, uFont2)
					if not self.iResID:	# Two rows
						y += 1
					else:
						y = 2
					x += w / 2
					screen.setText("GreatPersonBar1", "", szTxt, 1<<2, x, y, 0, eFontGame, eWidGen, 0, 0)
					screen.setHitTest("GreatPersonBar1", HitTestTypes.HITTEST_NOHIT)
					if city:
						fThreshold = float(GC.getPlayer(city.getOwner()).greatPeopleThresholdNonMilitary())
						fRate = float(city.getGreatPeopleRate())
						fFirst = float(city.getGreatPeopleProgress()) / fThreshold

						screen.setBarPercentage("GreatPersonBar", InfoBarTypes.INFOBAR_STORED, fFirst)
						if fFirst == 1:
							screen.setBarPercentage("GreatPersonBar", InfoBarTypes.INFOBAR_RATE, fRate / fThreshold)
						else:
							screen.setBarPercentage("GreatPersonBar", InfoBarTypes.INFOBAR_RATE, fRate / fThreshold / (1 - fFirst))
					else:
						screen.setBarPercentage("GreatPersonBar", InfoBarTypes.INFOBAR_STORED, 0)
						screen.setBarPercentage("GreatPersonBar", InfoBarTypes.INFOBAR_RATE, 0)
					screen.show("GreatPersonBar")
				elif not self.iResID:
					screen.hide("GreatPersonBar")
					screen.hide("GreatPersonBar0")
					screen.hide("GreatPersonBar1")


	def updateTimeText(self):
		"""
		NJAGC
		Format: Time - GameTurn/Total Percent - GA TurnsLeft - Date

		Ex: 10:37 - 220/660 33% - GA 3 - 1925
		"""
		if self.bShowTimeTextAlt:
			bShowGameTurn = ClockOpt.isShowAltGameTurn()
			bShowTotalTurns = ClockOpt.isShowAltTotalTurns()
			bShowPercentComplete = ClockOpt.isShowAltPercentComplete()
			bShowDate = ClockOpt.isShowAltDate()
		else:
			bShowGameTurn = ClockOpt.isShowGameTurn()
			bShowTotalTurns = ClockOpt.isShowTotalTurns()
			bShowPercentComplete = ClockOpt.isShowPercentComplete()
			bShowDate = ClockOpt.isShowDate()

		iMaxTurns = GAME.getMaxTurns()
		if iMaxTurns < 1 and (bShowTotalTurns or bShowPercentComplete):
			bShowGameTurn = True
			bShowTotalTurns = False
			bShowPercentComplete = False

		szTxt = ""
		sep = " <font=2b><color=0,255,255>|</color></font> "

		iTurn = GAME.getElapsedGameTurns()
		if bShowGameTurn:
			szTxt += str(iTurn)

		if bShowTotalTurns:
			if szTxt:
				szTxt += "/"
			szTxt += str(iMaxTurns)

		if bShowPercentComplete:
			if szTxt:
				szTxt += " - "
			szTxt += "%.1f%%" %(100 * iTurn / float(iMaxTurns))
		# Golden Age
		iTurnsGA = self.CyPlayer.getGoldenAgeTurns()
		if iTurnsGA > 0:
			if szTxt:
				szTxt += sep
			szTxt += unichr(8866) + str(iTurnsGA)
		# Date
		if bShowDate:
			if szTxt:
				szTxt += sep
			iYear = GAME.getGameTurnYear()
			if iYear < 0:
				szTxt += "%d BC" %(-iYear)
			else:
				szTxt += "%d AD" % iYear
		if szTxt:
			szTxt += sep
		szTxt += getClockText()
		return szTxt


	def updateCityScreen(self):
		print "updateCityScreen"
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
		#Clean up?
		if self.bCityExit or self.bCityChange and not self.bCityEnter:
			screen.hide("WonderLimit0")
			screen.hide("WonderLimit1")
			screen.hide("Liberate")
			iReligionInfos = self.iNumReligionInfos
			for i in xrange(iReligionInfos):
				szName = "ReligionHolyCityDDS" + str(i)
				screen.hide(szName)
				szName = "ReligionDDS" + str(i)
				screen.hide(szName)
			iCorporationInfos = self.iNumCorporationInfos
			for i in xrange(iCorporationInfos):
				szName = "CorporationHeadquarterDDS" + str(i)
				screen.hide(szName)
				szName = "CorporationDDS" + str(i)
				screen.hide(szName)
			for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				szName = "CityPercentText" + str(i)
				screen.hide(szName)
			if self.bCityExit:
				# City HUD
				self.CS_Citizens_hide(screen)
				screen.hide("MaintenanceText")
				screen.hide("MaintenanceAmountText")
				screen.hide("NationalityText")
				screen.hide("CS|Defense0")
				screen.hide("EmploymentText")
				screen.hide("CityNameText")
				screen.hide("HealthText")
				screen.hide("CS|Happiness0")
				screen.hide("CultureText")
				screen.hide("GreatPeopleText")
				screen.hide("TradeRouteListLabel")
				screen.hide("InterfaceTopCityBackgroundWidget")
				screen.hide("InterfaceTopRightBackgroundWidget")
				screen.hide("InterfaceLeftCityBackgroundWidget")
				screen.hide("CityScreenTopWidget")
				screen.hide("CityNameBackground")
				screen.hide("TopCityPanelLeft")
				screen.hide("TopCityPanelRight")
				screen.hide("CityScreenAdjustPanel")
				screen.hide("CityScreenTopRightPanel")
				screen.hide("InterfaceRightCityBackgroundWidget")
				screen.hide("TradeRouteListBackground")
				# Built Buildings
				screen.hide("BldgList|Demolish0")
				screen.hide("BldgList|Bkgnd0")
				screen.hide("BldgList|Bkgnd1")
				screen.hide("BldgList|Bkgnd2")
				screen.hide("BldgList|Btn0")
				screen.hide("BldgList|Btn1")
				screen.hide("BldgList|Btn2")
				# Bonus list
				screen.hide("BonusList|Bkgnd0")
				screen.hide("BonusList|Bkgnd1")
				screen.hide("BonusList|Bkgnd2")
				screen.hide("BonusList|Bkgnd3")
				screen.hide("BonusList|Btn0")
				screen.hide("BonusList|Btn1")
				screen.hide("BonusList|Btn2")
				screen.hide("BonusList|Btn3")
				# Bars
				screen.hide("GreatPeopleBar")
				screen.hide("CultureBar")
				screen.hide("NationalityBar")
				screen.hide("CS|RevBar0")
				screen.hide("CS|RevBar1")
				# Raw Commerce
				screen.hide("CS|RawYields0")
				screen.hide("CS|RawYields1")
				screen.hide("CS|RawYields2")
				screen.hide("CS|RawYields3")
				screen.hide("CS|RawYields4")
				screen.hide("CS|RawYields5")
				screen.hide("CS|RawYields6")
				self.bCityExit = False

		InCity = self.InCity
		if not InCity: return

		if self.bCityScreen:
			self.yields = RawYields.Tracker()
			CyCity = InCity.CyCity
			if self.bCityChange or self.bCityEnter:
				self.buildCityListLeft(screen, InCity)
				self.buildCityListRight(screen, InCity)
				self.buildCitizenPanel(screen, CyCity)
				if self.bCityEnter:
					screen.show("InterfaceTopCityBackgroundWidget")
					screen.show("InterfaceTopRightBackgroundWidget")
					screen.show("InterfaceLeftCityBackgroundWidget")
					screen.show("CityScreenTopWidget")
					screen.show("CityNameBackground")
					screen.show("TopCityPanelLeft")
					screen.show("TopCityPanelRight")
					screen.show("CityScreenAdjustPanel")
					screen.show("CityScreenTopRightPanel")
					screen.show("InterfaceRightCityBackgroundWidget")
					screen.show("TradeRouteListBackground")
					screen.show("PopulationBar")
					screen.show("ProductionBar")
					# Left side list tabs
					screen.show("BldgList|Bkgnd0")
					screen.show("BldgList|Bkgnd1")
					screen.show("BldgList|Bkgnd2")
					screen.show("BldgList|Btn0")
					screen.show("BldgList|Btn1")
					screen.show("BldgList|Btn2")
					screen.show("BldgList|Demolish0")
					# Right side list tabs
					screen.show("BonusList|Bkgnd0")
					screen.show("BonusList|Bkgnd1")
					screen.show("BonusList|Bkgnd2")
					screen.show("BonusList|Bkgnd3")
					screen.show("BonusList|Btn0")
					screen.show("BonusList|Btn1")
					screen.show("BonusList|Btn2")
					screen.show("BonusList|Btn3")
					self.bCityEnter = False
			xRes = self.xRes

			iPlayer		= InCity.iPlayer
			iTeam		= InCity.iTeam
			CyPlayer	= InCity.CyPlayer
			# Cache Data
			eWidGen		= WidgetTypes.WIDGET_GENERAL
			iTableSTD	= TableStyles.TABLE_STYLE_STANDARD
			eFontGame	= FontTypes.GAME_FONT
			eFontSmall	= FontTypes.SMALL_FONT

			xMidL = self.xMidL
			xMidR = self.xMidR
			uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2, uFont1b, uFont1 = self.aFontList
			xPopProgBar = self.xPopProgBar
			iPopProgBarWidth = self.iPopProgBarWidth
			iconYieldList = self.iconYieldList
			iconCommerceList = self.iconCommerceList
			halfX = xRes / 2

			bOwnCity = iTeam == self.iTeam

			# Liberate Button Show
			if -1 != CyCity.getLiberationPlayer(False):
				screen.show("Liberate")

			if bOwnCity:
				if self.CyPlayer.getNumCities() < 2:
					screen.hide("CityScrollMinus")
					screen.hide("CityScrollPlus")

			szTxt = "<font=4b>"

			if CyCity.isCapital():
				szTxt += self.iconStarGold
			elif CyCity.isGovernmentCenter():
				szTxt += self.iconStarSilver

			if CyCity.isPower():
				szTxt += self.iconPower

			szTxt += CyCity.getName() + ": " + str(CyCity.getPopulation())

			if CyCity.isOccupation():
				szTxt += " " + self.iconOccupation + ":" + str(CyCity.getOccupationTimer())

			screen.setText("CityNameText", "", szTxt, 1<<2, halfX, 32, 0, eFontGame, WidgetTypes.WIDGET_CITY_NAME, 0, 1)

			iHealthGood = CyCity.goodHealth()
			iHealthBad = CyCity.badHealth(False)
			if iHealthBad > 0 or iHealthGood >= 0:
				if CyCity.healthRate(False, 0) < 0:
					# BUG - Negative Health Rate is Positive Eaten Food
					szTxt = TRNSLTR.getText("INTERFACE_CITY_HEALTH_BAD", (iHealthGood, iHealthBad, - CyCity.healthRate(False, 0)))
				elif iHealthBad > 0:
					szTxt = TRNSLTR.getText("INTERFACE_CITY_HEALTH_GOOD", (iHealthGood, iHealthBad))
				else:
					szTxt = TRNSLTR.getText("INTERFACE_CITY_HEALTH_GOOD_NO_BAD", (iHealthGood, ))
				screen.setLabel("HealthText", "", uFont2 + szTxt, 1<<0, xRes - xPopProgBar + 6, 70, 0, eFontGame, WidgetTypes.WIDGET_HELP_HEALTH, -1, -1)

			szTxt = ""
			iAngryPop = CyCity.angryPopulation(0)
			iUnhappyLevel = CyCity.unhappyLevel(0)
			if CyCity.isDisorder():
				szTxt = str(iAngryPop) + self.iconAngryPop
			elif iAngryPop > 0:
				# BUG - Negative Happy Rate is Positive Angry Population
				szTxt = TRNSLTR.getText("INTERFACE_CITY_UNHAPPY", (CyCity.happyLevel(), iUnhappyLevel, iAngryPop))
			elif iUnhappyLevel > 0:
				szTxt = TRNSLTR.getText("INTERFACE_CITY_HAPPY", (CyCity.happyLevel(), iUnhappyLevel))
			else:
				szTxt = TRNSLTR.getText("INTERFACE_CITY_HAPPY_NO_UNHAPPY", (CyCity.happyLevel(),))

			if CityOpt.isShowAngerCounter() and bOwnCity:
				# BUG - Anger Display
				iAngerTimer = max(CyCity.getHurryAngerTimer(), CyCity.getConscriptAngerTimer())
				if iAngerTimer > 0:
					szTxt += " (%i)" % iAngerTimer
			screen.hide("CS|Happiness0")
			screen.modifyString("CS|Happiness0", uFont2 + szTxt, 1<<0)
			screen.show("CS|Happiness0")

			# City Commerce panel
			y = 34
			screen.show("MaintenanceText")
			iMaintenance = CyCity.getMaintenanceTimes100()
			szTxt = "<font=1>-%d.%02d" %(iMaintenance/100, iMaintenance%100) + uFont2 + iconCommerceList[0]
			screen.setLabel("MaintenanceAmountText", "", szTxt, 1<<1, 252, y, 0, eFontSmall, WidgetTypes.WIDGET_HELP_MAINTENANCE, 0, 0)

			a4thX = xRes / 4
			iWidth = a4thX - 16
			a16thX = a4thX / 4
			# Trade List and Raw Yields
			bRawYields = CityOpt.isShowRawYields()
			if bRawYields:
				bYieldView = self.bYieldView
				iYieldType = self.iYieldType
				iYieldTiles = self.iYieldTiles
				screen.setState("CS|RawYields0", not bYieldView)
				screen.show("CS|RawYields0")

				screen.setState("CS|RawYields1", bYieldView and iYieldType == YieldTypes.YIELD_FOOD)
				screen.show("CS|RawYields1")
				screen.setState("CS|RawYields2", bYieldView and iYieldType == YieldTypes.YIELD_PRODUCTION)
				screen.show("CS|RawYields2")
				screen.setState("CS|RawYields3", bYieldView and iYieldType == YieldTypes.YIELD_COMMERCE)
				screen.show("CS|RawYields3")

				screen.setState("CS|RawYields4", iYieldTiles == RawYields.WORKED_TILES)
				screen.show("CS|RawYields4")
				screen.setState("CS|RawYields5", iYieldTiles == RawYields.CITY_TILES)
				screen.show("CS|RawYields5")
				screen.setState("CS|RawYields6", iYieldTiles == RawYields.OWNED_TILES)
				screen.show("CS|RawYields6")
			else:
				bYieldView = False
				screen.show("TradeRouteListLabel")
			# Trade
			aList = []
			for iRoute in xrange(CyCity.getMaxTradeRoutes()):
				CyCityX = CyCity.getTradeCity(iRoute)
				if not CyCityX: continue
				iPlayerX = CyCityX.getOwner()
				if iPlayerX > -1:
					if bYieldView:
						iTeamX = CyCityX.getTeam()
					else:
						CyPlayerX = GC.getPlayer(iPlayerX)
						szLeftBuffer = "<color=%d,%d,%d>" %(CyPlayerX.getPlayerTextColorR(), CyPlayerX.getPlayerTextColorG(), CyPlayerX.getPlayerTextColorB())
						szLeftBuffer += CyCityX.getName()
						szRightBuffer = ""
					for eYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						iTradeProfit = CyCity.calculateTradeYield(eYield, CyCity.calculateTradeProfitTimes100(CyCityX))
						if iTradeProfit:
							if bYieldView:
								if iTeam == iTeamX:
									self.yields.addDomesticTrade(eYield, iTradeProfit)
								else:
									self.yields.addForeignTrade(eYield, iTradeProfit)
							else:
								if iTradeProfit < 0:
									szRightBuffer += "<color=255,0,0>"
								else:
									szRightBuffer += "<color=150,255,200>"
								szRightBuffer += TextUtil.floatToString(iTradeProfit / 100.0) + iconYieldList[eYield]
					if not bYieldView:
						aList.append([szLeftBuffer, szRightBuffer, iRoute])

			Tbl = "TradeRouteTable"
			screen.addTableControlGFC(Tbl, 3, 8, self.yTradeRouteTable, iWidth, self.hTradeRouteTable, False, False, 32, 32, iTableSTD)
			if bYieldView:
				screen.setStyle(Tbl, "Table_City_Style")
				screen.setTableColumnHeader(Tbl, 0, "", 3 * a16thX - 24)
				screen.setTableColumnHeader(Tbl, 1, "", a16thX/2)
				screen.setTableColumnHeader(Tbl, 2, "", a16thX/2)
				screen.setTableColumnRightJustify(Tbl, 1)
				screen.setTableColumnRightJustify(Tbl, 2)
				self.yields.processCity(CyCity)
				self.yields.fillTable(screen, Tbl, iYieldType, iYieldTiles)
			else:
				dx = iWidth/2 - 8
				screen.setStyle(Tbl, "Table_City_Style")
				screen.setTableColumnHeader(Tbl, 0, "", dx)
				screen.setTableColumnHeader(Tbl, 1, "", dx)
				screen.setTableColumnHeader(Tbl, 2, "", 8)
				screen.setTableColumnRightJustify(Tbl, 1)
				eWidget = WidgetTypes.WIDGET_HELP_TRADE_ROUTE_CITY
				for i, entry in enumerate(aList):
					L, R, iRoute = entry
					screen.appendTableRow(Tbl)
					screen.setTableText(Tbl, 0, i, uFont1b + L, "", eWidget, iRoute, -1, 1<<0)
					screen.setTableText(Tbl, 1, i, uFont1b + R, "", eWidget, iRoute, -1, 1<<1)

			# Religions and Corporations present
			artPathHolyCity = self.artPathOverlayHolyCity
			iWidCityReligion = WidgetTypes.WIDGET_HELP_RELIGION_CITY
			iWidCityCorporation = WidgetTypes.WIDGET_HELP_CORPORATION_CITY

			iCount = 0
			aList1 = []
			iReligionInfos = self.iNumReligionInfos
			for i in xrange(iReligionInfos):
				if not CyCity.isHasReligion(i): continue
				aList1.append(i)
				iCount += 1
			aList2 = []
			iCorporationInfos = self.iNumCorporationInfos
			for i in xrange(iCorporationInfos):
				if not CyCity.isHasCorporation(i): continue
				aList2.append(i)
				iCount += 1
			if iCount < 4:
				S = 64
			elif iCount < 5:
				S = 60
			elif iCount < 6:
				S = 48
			elif iCount < 11:
				S = 46
			elif iCount < 13:
				S = 40
			elif iCount < 15:
				S = 34
			elif iCount < 25:
				S = 30
			elif iCount < 28:
				S = 26
			elif iCount < 31:
				S = 24
			elif iCount < 45:
				S = 22
			else:
				S = 16
			X, Y, W, H = self.CityReligionPanel
			iBtnPerRow = (W - 4) / S
			D = (W - iBtnPerRow * S) / 2
			X += D
			iRows = (iCount + iBtnPerRow - 1) / iBtnPerRow
			D = (H - S * iRows) / 2
			Y += D
			del W, H, D, iRows
			iCount = -1
			for i in aList1:
				iCount += 1
				# Set the Religion button
				x = X + S * (iCount % iBtnPerRow)
				y = Y + S * (iCount / iBtnPerRow)
				artPath = GC.getReligionInfo(i).getButton()
				szName = "ReligionDDS" + str(i)
				screen.setImageButton(szName, artPath, x, y, S, S, iWidCityReligion, i, 1)
				if CyCity.isHolyCityByType(i):
					szName = "ReligionHolyCityDDS" + str(i)
					screen.addDDSGFC(szName, artPathHolyCity, x+1, y+1, S-2, S-2, iWidCityReligion, i, 1)

			for i in aList2:
				iCount += 1
				# Set the Corporation button
				x = X + S * (iCount % iBtnPerRow)
				y = Y + S * (iCount / iBtnPerRow)
				artPath = GC.getCorporationInfo(i).getButton()
				szName = "CorporationDDS" + str(i)
				screen.setImageButton(szName, artPath, x, y, S, S, iWidCityCorporation, i, 1)
				if CyCity.isHeadquartersByType(i):
					szName = "CorporationHeadquarterDDS" + str(i)
					screen.addDDSGFC(szName, artPathHolyCity, x+1, y+1, S-2, S-2, iWidCityCorporation, i, 1)

			# Nationality Bar
			x = xMidL/2
			yBot = self.yCityStackBarBot
			szTxt = u"%d%% %s" %(CyCity.plot().calculateCulturePercent(iPlayer), CyPlayer.getCivilizationAdjective(0))
			screen.setText("NationalityText", "", uFont2 + szTxt, 1<<2, x, yBot, 0, eFontGame, WidgetTypes.WIDGET_HELP_NATIONALITY, 0, 0)
			iRemainder = 100
			n = 0
			for h in xrange(self.iMaxPlayers):
				if GC.getPlayer(h).isAlive():
					iPercent = CyCity.plot().calculateCulturePercent(h)
					if iPercent > 0:
						screen.setStackedBarColorsRGB("NationalityBar", n, GC.getPlayer(h).getPlayerTextColorR(), GC.getPlayer(h).getPlayerTextColorG(), GC.getPlayer(h).getPlayerTextColorB(), GC.getPlayer(h).getPlayerTextColorA())
						if iRemainder <= 0:
							screen.setBarPercentage("NationalityBar", n, 0.0)
						else:
							screen.setBarPercentage("NationalityBar", n, float(iPercent) / iRemainder)
						iRemainder -= iPercent
						n += 1
			screen.show("NationalityBar")

			yTop = self.yCityStackBarTop
			if self.GO_REVOLUTION and not RevInstances.RevolutionInst is None:
				# < Revolution Mod Start >
				RevInstances.RevolutionInst.updateLocalRevIndices(GAME.getGameTurn(), iPlayer, subCityList = [CyCity], bIsRevWatch = True)

				divisor = RevInstances.RevolutionInst.revInstigatorThreshold
				revIndex = CyCity.getRevolutionIndex()
				deltaTrend = revIndex - CyCity.getRevIndexAverage()
				revIndex = min([revIndex,divisor])
				if deltaTrend > RevInstances.RevolutionInst.showTrend or deltaTrend <= -RevInstances.RevolutionInst.showTrend:
					deltaTrend = (deltaTrend*max([abs(deltaTrend),divisor/100+1]))/abs(2*deltaTrend)
				else:
					deltaTrend = 0
				adjIndex = max([revIndex - abs(deltaTrend),0])

				szRevWatchDanger = self.szRevWatchDanger
				dangerRed	= "<color=230,0,0,255>"		+ szRevWatchDanger		 + "<color=255,255,255,255>"
				danger		= "<color=230,120,0,255>"	+ szRevWatchDanger		 + "<color=255,255,255,255>"
				warning		= "<color=225,225,0,255>"	+ self.szRevWatchWarning + "<color=255,255,255,255>"
				safe		= "<color=0,230,0,255>"		+ self.szRevWatchSafe	 + "<color=255,255,255,255>"

				if CyCity.getRevolutionIndex() >= RevInstances.RevolutionInst.alwaysViolentThreshold:
					cityString = dangerRed
				elif CyCity.getRevolutionIndex() >= RevInstances.RevolutionInst.revInstigatorThreshold:
					cityString = danger
				elif CyCity.getRevolutionIndex() >= RevInstances.RevolutionInst.revReadyFrac * RevInstances.RevolutionInst.revInstigatorThreshold:
					cityString = warning
				else:
					cityString = safe
				szTxt = uFont2 + u"%s: %s"%(TRNSLTR.getText("TXT_KEY_REV_STATUS", ()), cityString)
				screen.setText("CS|RevBar1", "", szTxt, 1<<2, x, yTop, 0, eFontGame, eWidGen, 0, 0)

				fPercent1 = adjIndex/(1.0*divisor)
				fPercent2 = abs(deltaTrend)/(1.0*divisor)
				fPercent2 = min([fPercent2, 1.0-fPercent1])
				screen.setStackedBarColorsRGB("CS|RevBar0", 0, 100, 100, 100, 255)
				screen.setBarPercentage("CS|RevBar0", 0, fPercent1)

				if deltaTrend < 0:
					screen.setStackedBarColorsRGB("CS|RevBar0", 1, 0, 230, 0, 255)
				else:
					screen.setStackedBarColorsRGB("CS|RevBar0", 1, 230, 0, 0, 255)

				if fPercent1 == 1:
					screen.setBarPercentage("CS|RevBar0", 1, fPercent2)
				else:
					screen.setBarPercentage("CS|RevBar0", 1, fPercent2/(1.0-fPercent1))
				screen.show("CS|RevBar0")
			# Wonder slots
			iNationalWonders = CyCity.getNumNationalWonders()
			iGreatWonders = CyCity.getNumWorldWonders()
			iMaxNationalWonders = CyCity.getMaxNumNationalWonders()
			iMaxGreatWonders = CyCity.getMaxNumWorldWonders()
			szTxt = self.szMainCityGreatWonders + ": " + str(iGreatWonders) + "/" + str(iMaxGreatWonders)
			screen.modifyLabel("WonderLimit0", "<font=1b><color=128,255,255,255>" + szTxt, 1<<0)
			szTxt = self.szMainCityNationalWonders + ": " + str(iNationalWonders) + "/" + str(iMaxNationalWonders)
			screen.modifyLabel("WonderLimit1", "<font=1b><color=128,255,255,255>" + szTxt, 1<<0)
			screen.show("WonderLimit0")
			screen.show("WonderLimit1")

			szTxt = self.iconDefense + " %d%%" % CyCity.getDefenseModifier(False)
			iDefenseDamage = CyCity.getDefenseDamage()
			if iDefenseDamage:
				iMaxDefenseDamage = self.iMaxDefenseDamage
				szTxt += " (%d%%)" % (100*(iMaxDefenseDamage - iDefenseDamage)/iMaxDefenseDamage)
			screen.setText("CS|Defense0", "", "<font=3b>" + szTxt, 1<<1, xRes - 270, 34, 0, eFontSmall, eWidGen, 1, 2)

			iEmployed = CyCity.getNumPopulationEmployed()
			if iEmployed:
				iconCitizen = self.iconCitizen
				szTxt = "Employed: " + str(iEmployed) + iconCitizen
				screen.setLabel("EmploymentText", "", szTxt, 1<<1, xRes - 400, 32, 0, eFontSmall, WidgetTypes.WIDGET_HELP_EMPLOYMENT, -1, -1)

			xR, w = self.xwCityStackBarR
			xR += w/2
			iCultureTreshold = CyCity.getCultureThreshold()
			iRate = CyCity.getCommerceRateTimes100(CommerceTypes.COMMERCE_CULTURE)
			if not iRate % 100:
				szTxt = TRNSLTR.getText("INTERFACE_CITY_COMMERCE_RATE", (GC.getCommerceInfo(CommerceTypes.COMMERCE_CULTURE).getChar(), GC.getCultureLevelInfo(CyCity.getCultureLevel()).getTextKey(), iRate/100))
			else:
				szRate = u"+%d.%02d" % (iRate/100, iRate%100)
				szTxt = TRNSLTR.getText("INTERFACE_CITY_COMMERCE_RATE_FLOAT", (GC.getCommerceInfo(CommerceTypes.COMMERCE_CULTURE).getChar(), GC.getCultureLevelInfo(CyCity.getCultureLevel()).getTextKey(), szRate))

			if iRate > 0 and iCultureTreshold > 0:
				# Culture Turns
				iCultureTimes100 = CyCity.getCultureTimes100(iPlayer)
				iCultureLeftTimes100 = 100 * iCultureTreshold - iCultureTimes100
				szTxt += " " + TRNSLTR.getText("INTERFACE_CITY_TURNS", (((iCultureLeftTimes100 + iRate - 1) / iRate),))
			screen.setText("CultureText", "", uFont2 + szTxt, 1<<2, xR, yBot - 1, 0, eFontGame, WidgetTypes.WIDGET_HELP_CULTURE, 0, 0)
			# Great People Bar
			iGreatPeopleProgress = CyCity.getGreatPeopleProgress()
			iGreatPeopleRate = CyCity.getGreatPeopleRate()
			if iGreatPeopleProgress + iGreatPeopleRate > 0:
				# Great Person Turns
				iGreatPeopleTreshold = CyPlayer.greatPeopleThresholdNonMilitary()
				if iGreatPeopleRate > 0:
					iGPTurns = (iGreatPeopleTreshold - iGreatPeopleProgress + iGreatPeopleRate - 1) / iGreatPeopleRate
				else:
					iGPTurns = None
				if CityOpt.isShowCityGreatPersonInfo():
					bOne = MainOpt.isGPBarTypesNone() or MainOpt.isGPBarTypesOne()
					szTxt = GPUtil.getGreatPeopleText(CyCity, iGPTurns, w - 32, False, bOne, False, uFont2)
				else:
					szTxt = TRNSLTR.getText("INTERFACE_CITY_GREATPEOPLE_RATE", (GAME.getSymbolID(FontSymbols.GREAT_PEOPLE_CHAR), iGreatPeopleRate))
					if iGreatPeopleRate > 0:
						szTxt += " " + TRNSLTR.getText("INTERFACE_CITY_TURNS", (iGPTurns,))

				screen.setText("GreatPeopleText", "", uFont2 + szTxt, 1<<2, xR, yTop - 1, 0, eFontGame, WidgetTypes.WIDGET_HELP_GREAT_PEOPLE, 0, 0)

				fGreatPeopleTreshold = float(iGreatPeopleTreshold)
				screen.setBarPercentage("GreatPeopleBar", InfoBarTypes.INFOBAR_STORED, iGreatPeopleProgress / fGreatPeopleTreshold)
				screen.setBarPercentage("GreatPeopleBar", InfoBarTypes.INFOBAR_RATE, iGreatPeopleRate / (fGreatPeopleTreshold - iGreatPeopleProgress + 0.001))
				screen.show("GreatPeopleBar")

			if iCultureTreshold > 0:
				iFirst = float(CyCity.getCultureTimes100(iPlayer)) / float(100 * iCultureTreshold)
				screen.setBarPercentage("CultureBar", InfoBarTypes.INFOBAR_STORED, iFirst)
				if iFirst == 1:
					screen.setBarPercentage("CultureBar", InfoBarTypes.INFOBAR_RATE, float(CyCity.getCommerceRate(CommerceTypes.COMMERCE_CULTURE)) / float(iCultureTreshold))
				else:
					screen.setBarPercentage("CultureBar", InfoBarTypes.INFOBAR_RATE, (float(CyCity.getCommerceRate(CommerceTypes.COMMERCE_CULTURE)) / float(iCultureTreshold)) / (1 - iFirst))
			else:
				screen.setBarPercentage("CultureBar", InfoBarTypes.INFOBAR_STORED, 100)
			screen.show("CultureBar")


	def buildCityListLeft(self, screen=None, InCity=None):
		if screen is None:
			screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
			InCity = self.InCity
		CyCity = InCity.CyCity

		CyTeam = InCity.CyTeam
		iconYieldList = self.iconYieldList
		iconCommerceList = self.iconCommerceList
		iconCitizen = self.iconCitizen
		iconHappy = self.iconHappy
		iconUnhappy = self.iconUnhappy
		iconHealthy = self.iconHealthy
		iconUnhealthy = self.iconUnhealthy
		uFont2 = self.aFontList[5]
		iHandicapCorporationMaintenanceMod = GC.getHandicapInfo(CyCity.getHandicapType()).getCorporationMaintenancePercent()
		bNegGoldIsMaintenance = self.bNegGoldIsMaintenance

		eWidGen		= WidgetTypes.WIDGET_GENERAL
		ePanelBlack	= PanelStyles.PANEL_STYLE_MAIN_BLACK25
		eFontGame	= FontTypes.GAME_FONT

		w = self.xRes / 4
		y = self.yBuildListTable - 8
		ScPnl = "BldgListTable"
		screen.addScrollPanel(ScPnl, "", -2, y, w + 8, self.yCityStackBarTop - y - 22, ePanelBlack)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")

		if self.iResID == 2:
			dy = 24
		elif self.iResID == 1:
			dy = 22
		else:
			dy = 20

		h1 = dy + 6
		h2 = dy + 2
		w -= 16
		wName = w - w / 3 - 24

		aBuildingList = self.aSortedBuildingTuple[self.iShowBuildings]
		ID = "WID|BUILDING|BldgList%d"
		ROW_0 = "BldgListRow%d"
		iRow = 0
		y = -2
		for szName, i, CvBuildingInfo in aBuildingList:

			if CyCity.hasBuilding(i):
				szStat = ""

				if CyCity.isActiveBuilding(i):
					iEmployed = CvBuildingInfo.getNumPopulationEmployed()
					if iEmployed:
						szStat += str(iEmployed) + iconCitizen

					iHealth = CyCity.getBuildingHealth(i)
					if iHealth:
						if iHealth > 0:
							szStat += str(iHealth) + iconHealthy
						else:
							szStat += str(-iHealth) + iconUnhealthy

					iHappiness = CyCity.getBuildingHappiness(i)
					if iHappiness:
						if iHappiness > 0:
							szStat += str(iHappiness) + iconHappy
						else:
							szStat += str(-iHappiness) + iconUnhappy

					for j in xrange(YieldTypes.NUM_YIELD_TYPES):
						iYield = CyCity.getBaseYieldRateFromBuilding100(j, i) / 100
						if iYield:
							szStat += str(iYield) + iconYieldList[j]
							self.yields.addBuilding(j, iYield)
				for j in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
					iCommerce = CyCity.getBuildingCommerceByBuilding(j, i) + (CyCity.getBonusCommercePercentChanges(j, i) + CyTeam.getBuildingCommerceTechChange(j, i)) / 100
					# AIAndy: display maintenance as negative gold
					if j == CommerceTypes.COMMERCE_GOLD:
						iCommerceChange = CvBuildingInfo.getCommerceChange(j)
						if iCommerceChange < 0 and bNegGoldIsMaintenance:
							iCommerceChange *= iHandicapCorporationMaintenanceMod
							iCommerce += iCommerceChange / 100
					if iCommerce:
						szStat += str(iCommerce) + iconCommerceList[j]

				ROW = ROW_0 % iRow

				if iRow % 2:
					screen.attachPanelAt(ScPnl, ROW, "", "", True, False, ePanelBlack, 0, y, w, h2, eWidGen, 1, 1)
					screen.setStyle(ROW, "Panel_Tan15_Style")
				else:
					screen.attachPanelAt(ScPnl, ROW, "", "", True, False, ePanelBlack, 0, y-4, w, h1, eWidGen, 1, 1)

				szName = TextUtil.evalTextWidth(wName, uFont2, szName)
				screen.setTextAt(ID % i, ROW, szName, 1<<0, 4, 0, 0, eFontGame, eWidGen, 1, 1)
				if szStat:
					screen.setLabelAt("", ROW, uFont2+szStat, 1<<1, w-6, 2, 0, eFontGame, eWidGen, 1, 1)
				iRow += 1
				y += dy


	def buildCityListRight(self, screen, InCity):
		CyCity = InCity.CyCity
		screen.hide("BonusListTable0")
		screen.hide("BonusListTable1")
		screen.hide("BonusListTable2")
		screen.hide("BonusListTable3")
		TAB = self.iShowBonus
		ID = "BonusListTable" + str(TAB)
		if TAB: # A bonus list
			CyPlayer = InCity.CyPlayer
			# Check if the list is stored
			aBonusList = self.aSortedBonusTuple[TAB]
			aList1 = []
			aList2 = []
			for szName, iBonus, szChar in aBonusList:
				iTotalExport = CyPlayer.getBonusExport(iBonus)
				iTotalResourceCount = CyCity.getNumBonuses(iBonus) + iTotalExport
				if iTotalResourceCount > 0:
					entry = (szName, iBonus, szChar, iTotalResourceCount, iTotalExport)
					aList1.append(entry)
					aList2.append(iBonus)

			if aList2:
				if aList2 == InCity.listBonus[TAB]:
					screen.show(ID)
					return
				InCity.listBonus[TAB] = list(aList2)
			else: return

		# Build the list
		a4thX = self.xRes / 4
		iWidth = a4thX - 16

		screen.addTableControlGFC(ID, 2, 3*a4thX + 8, 170, iWidth, self.yTopCitizen - 172, False, False, 32, 32, TableStyles.TABLE_STYLE_STANDARD)
		screen.setStyle(ID, "Table_City_Style")

		a8thX = a4thX/2
		screen.setTableColumnHeader(ID, 0, "", a8thX)
		screen.setTableColumnHeader(ID, 1, "", a8thX - 24)
		screen.setTableColumnRightJustify(ID, 1)
		uFont3	= self.aFontList[3]

		iRow = -1
		if not TAB: # Properties display
			uFont2	= self.aFontList[5]
			iWidget = WidgetTypes.WIDGET_PROPERTY
			pProperties = CyCity.getProperties()
			iPropertyNum = pProperties.getNumProperties()
			for i in xrange(iPropertyNum):
				eProperty = pProperties.getProperty(i)
				info = GC.getPropertyInfo(eProperty)
				szLeftBuffer = uFont3 + u"%c " % info.getChar() + uFont2 + info.getText()
				iChange = pProperties.getChangeByProperty(eProperty)
				szRightBuffer = uFont2 + str(pProperties.getValue(i)) + " ( "
				if iChange >= 0:
					szRightBuffer += "+"
				szRightBuffer += str(iChange) + " )"
				iRow += 1
				screen.appendTableRow(ID)
				screen.setTableText(ID, 0, iRow, szLeftBuffer, "", iWidget, eProperty, -1, 1<<0)
				screen.setTableText(ID, 1, iRow, szRightBuffer, "", iWidget, eProperty, -1, 1<<1)
			return
		iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS
		iconUnhealthy = self.iconUnhealthy
		iconHealthy = self.iconHealthy
		iconUnhappy = self.iconUnhappy
		iconHappy = self.iconHappy
		uFont1b	= self.aFontList[6]
		# Bonus List
		for szName, iBonus, szChar, iTotalResourceCount, iTotalExport in aList1:
			iRow += 1
			szLeftBuffer = uFont3 + szChar + uFont1b + szName + " ("
			szTemp = ""
			iTotalImport = CyPlayer.getBonusImport(iBonus)
			iHomemade = iTotalResourceCount - iTotalImport
			if iHomemade > 0:
				szTemp += str(iHomemade)
			if iTotalImport > 0:
				if szTemp:
					szTemp += ", "
				szTemp += "<color=0,255,255,255>+" + str(iTotalImport)
			if iTotalExport > 0:
				if szTemp:
					szTemp += ", "
				szTemp += "<color=255,0,0,255>-" + str(iTotalExport)
			szLeftBuffer += szTemp + ")"

			szRightBuffer = uFont1b
			#Happiness
			iHappiness = CyCity.getBonusHappiness(iBonus)
			if iHappiness:
				szRightBuffer += "+" + str(abs(iHappiness))
				if iHappiness > 0:
					szRightBuffer += iconHappy
				else:
					szRightBuffer += iconUnhappy
			#Health
			iHealth = CyCity.getBonusHealth(iBonus)
			if iHealth:
				szRightBuffer += "+" + str(abs(iHealth))
				if iHealth > 0:
					szRightBuffer += iconHealthy
				else:
					szRightBuffer += iconUnhealthy
			#Corporations
			for i in xrange(self.iNumCorporationInfos):
				if CyCity.isHasCorporation(i):
					for eBonus in GC.getCorporationInfo(i).getPrereqBonuses():
						if eBonus == iBonus:
							szRightBuffer += u'%c' %(GC.getCorporationInfo(i).getChar())
			screen.appendTableRow(ID)
			screen.setTableText(ID, 0, iRow, szLeftBuffer, "", iWidget, iBonus, -1, 1<<0)
			screen.setTableText(ID, 1, iRow, szRightBuffer, "", iWidget, iBonus, -1, 1<<1)

	def buildCityWorkQueue(self, screen, InCity):
		if not InCity: return
		CyCity = InCity.CyCity
		CyPlayer = InCity.CyPlayer

		InCity.WorkQueue = []
		uFont = self.aFontList[5]

		eWidGen		= WidgetTypes.WIDGET_GENERAL
		ePanelSTD	= PanelStyles.PANEL_STYLE_STANDARD
		eFontGame	= FontTypes.GAME_FONT

		ScPnl = "CityWorkQueue"
		y = self.yBotBar + 16
		h = self.yRes - y - 20
		w = self.xMidL - 78

		if self.iResID == 2:
			dy = 24
		elif self.iResID == 1:
			dy = 22
		else:
			dy = 20
		screen.addScrollPanel(ScPnl, "", -8, y, w, h, ePanelSTD)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")
		iOrders = CyIF.getNumOrdersQueued()
		y = 0
		for iNode in xrange(iOrders):
			szTxt1 = ""
			szTxt2 = ""
			iOrder	= CyIF.getOrderNodeType(iNode)
			iType	= CyIF.getOrderNodeData1(iNode)

			szName = "WID|"
			bSave = CyIF.getOrderNodeSave(iNode)
			if iOrder == OrderTypes.ORDER_TRAIN:

				szTxt2 = str(CyCity.getUnitProductionTurnsLeft(iType, iNode))
				if CyCity.getProgressOnUnit(iType) > 0:

					if y > 0:
						if CyCity.isUnitProductionDecay(iType):
							szTxt1 = "<color=255,76,76,255><b>!! </b></color>"
						elif CyCity.getDelayOnUnit(iType) > 0:
							szTxt1 = "<color=255,255,0,255><b>! </b></color>"

					szTxt2 = "<color=0,255,255,255>" + szTxt2 + "</color>"

				if bSave:
					szTxt1 += "*"
					szTxt2 += "*"
				szTxt1 += GC.getUnitInfo(iType).getDescription()

				szName += "UNIT|"

			elif iOrder == OrderTypes.ORDER_CONSTRUCT:


				if CyCity.getProgressOnBuilding(iType) > 0:

					if y > 0:
						if CyCity.isBuildingProductionDecay(iType):
							szTxt1 = "<color=255,76,76,255><b>!! </b></color>"
						elif CyCity.getDelayOnBuilding(iType) > 0:
							szTxt1 = "<color=255,255,0,255><b>! </b></color>"

					szTxt2 = "<color=0,255,255,255>"

				szTxt1 += GC.getBuildingInfo(iType).getDescription()
				szTxt2 += str(CyCity.getBuildingProductionTurnsLeft(iType, iNode))

				szName += "BUILDING|"

			elif iOrder == OrderTypes.ORDER_CREATE:
				szTxt1 = GC.getProjectInfo(iType).getDescription()
				if CyCity.getProjectProduction(iType) > 0:
					szTxt2 = "<color=0,255,255,255>"
				szTxt2 += str(CyCity.getProjectProductionTurnsLeft(iType, iNode))
				szName += "PROJECT|"

			elif iOrder == OrderTypes.ORDER_MAINTAIN:
				szTxt1 = GC.getProcessInfo(iType).getDescription()
				szName += "PROCESS|"

			elif iOrder == OrderTypes.ORDER_LIST:
				iCurrentList = CyPlayer.getBLIndexByID(iType)
				szTxt1 = bSave*"*" + CyPlayer.getBLListName(iCurrentList)
				szName += "LIST|"

			szRow = str(iNode)
			InCity.WorkQueue.append([szName, iType, szRow])
			szName += "QueueEntry" + szRow
			ROW = "QueueRow" + szRow
			screen.attachPanelAt(ScPnl, ROW, "", "", True, False, ePanelSTD, -2, y-2, w-21, dy, eWidGen, 1, 1)

			szTxt1 = TextUtil.evalTextWidth(w - 80, uFont, szTxt1)

			screen.setTextAt(szName, ROW, szTxt1, 1<<0, 4, -2, 0, eFontGame, eWidGen, 1, 1)
			if szTxt2:
				screen.setLabelAt("QueueTime"+szRow, ROW, uFont+szTxt2, 1<<0, w-64, 0, 0, eFontGame, eWidGen, 1, 1)
			y += dy

		InCity.QueueIndex = iOrders

	def buildCitySelectionUI(self, screen, bFirst, CyCity):

		if bFirst:
			self.buildCityTabButtons(screen, CyCity)
		# Unit Group/Sort
		iLanguage = GAME.getCurrentLanguage()
		if not iLanguage: # English
			wGroupButton = 96
			wSortButton = 134
		elif iLanguage == 1: # French
			wGroupButton = 110
			wSortButton = 160
		else:
			wGroupButton = 96
			wSortButton = 134
		'''
		elif iLanguage == 2: # German
		elif iLanguage == 3: # Italian
		elif iLanguage == 4: # Spanish
		'''
		self.wUnitGroupButton = wGroupButton
		self.wUnitSortButton = wSortButton
		x0 = self.xRes - self.xRes/4 - 50

		ID = "CT|UnitGrouping"
		SELECTED = CyCity.getUnitListGrouping()
		x = x0 - wGroupButton
		screen.addDropDownBoxGFC(ID, x, 140, wGroupButton, WidgetTypes.WIDGET_UNIT_GROUPING, 0, 0, FontTypes.SMALL_FONT)
		TYPE = UnitGroupingTypes.UNIT_GROUPING_SINGLE
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNITHELP_GROUPING_SINGLE", ()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitGroupingTypes.UNIT_GROUPING_COMBAT
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNITHELP_GROUPING_COMBAT", ()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitGroupingTypes.UNIT_GROUPING_DOMAIN
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_DOMAIN", ()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitGroupingTypes.UNIT_GROUPING_HERO
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNITHELP_GROUPING_HERO", ()), TYPE, TYPE, SELECTED == TYPE)

		ID = "CT|UnitSorting"
		SELECTED = CyCity.getUnitListSorting()
		x -= wSortButton + 4
		screen.addDropDownBoxGFC(ID, x, 140, wSortButton, WidgetTypes.WIDGET_UNIT_SORT, -1, -1, FontTypes.SMALL_FONT)
		TYPE = UnitSortTypes.UNIT_SORT_NAME
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_WORD_NAME",				()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitSortTypes.UNIT_SORT_COST
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_WORD_COST",				()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitSortTypes.UNIT_SORT_STRENGTH
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNITHELP_SORT_STRENGTH",	()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitSortTypes.UNIT_SORT_MOVE
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNITHELP_SORT_MOVE",		()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitSortTypes.UNIT_SORT_COLLATERAL
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNITHELP_SORT_COLLATERAL",()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitSortTypes.UNIT_SORT_RANGE
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNITHELP_SORT_RANGE",		()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitSortTypes.UNIT_SORT_BOMBARD
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNITHELP_SORT_BOMBARD",	()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitSortTypes.UNIT_SORT_CARGO
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_WORD_CARGO",				()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitSortTypes.UNIT_SORT_WITHDRAWAL
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNITHELP_SORT_WITHDRAWAL",()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = UnitSortTypes.UNIT_SORT_POWER
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNITHELP_SORT_POWER",		()), TYPE, TYPE, SELECTED == TYPE)

		iTab = self.iCityTab
		if iTab != CITYTAB_UNIT:
			screen.hide("CT|UnitGrouping")
			screen.hide("CT|UnitSorting")

		# Building Sorting
		if not iLanguage: # English
			wSortButton = 142
		elif iLanguage == 1: # French
			wSortButton = 160
		else:
			wSortButton = 142
		'''
		elif iLanguage == 2: # German
		elif iLanguage == 3: # Italian
		elif iLanguage == 4: # Spanish
		'''
		self.wBuildingSortButton = wSortButton

		ID = "CT|BuildingSorting"
		SELECTED = CyCity.getBuildingListSorting()
		x = x0 - wSortButton
		screen.addDropDownBoxGFC(ID, x, 140, wSortButton, WidgetTypes.WIDGET_BUILDING_SORT, -1, -1, FontTypes.SMALL_FONT)
		TYPE = BuildingSortTypes.BUILDING_SORT_NAME
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_WORD_NAME",					()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_COST
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_WORD_COST",					()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_SCIENCE
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_BUILDINGHELP_SORT_SCIENCE",	()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_ESPIONAGE
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_WORD_ESPIONAGE",				()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_CULTURE
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_WORD_CULTURE",				()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_GOLD
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_BUILDINGHELP_SORT_GOLD",		()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_FOOD
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_WORD_FOOD",					()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_PRODUCTION
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_PRODUCTION",				()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_HAPPINESS
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_BUILDINGHELP_SORT_HAPPINESS",	()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_HEALTH
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_BUILDINGHELP_SORT_HEALTH",	()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_CRIME
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_BUILDINGHELP_SORT_CRIME",		()), TYPE, TYPE, SELECTED == TYPE)
		TYPE = BuildingSortTypes.BUILDING_SORT_FLAMMABILITY
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_PROPERTY_FLAMMABILITY",	()), TYPE, TYPE, SELECTED == TYPE)

		if iTab not in (CITYTAB_BUILDING, CITYTAB_WONDER):
			screen.hide("CT|BuildingSorting")

	def buildCityTabButtons(self, screen, CyCity):
		eWidGen = WidgetTypes.WIDGET_GENERAL
		# City Tabs
		y = self.yBotBar + 32
		iSize = CityOpt.getBuildIconSize()
		dx = iSize + 4
		dA = iSize - 8
		szPath = "Art/Interface/screens/City/"
		aList = [
			"CityAdmin.dds",
			"CityUnit.dds",
			"CityBuilding.dds",
			"CityWonder.dds"
		]
		x = self.xMidL
		for i in xrange(4):
			ID = str(i)
			Btn = "CityTab" + ID
			Img = "CityTab|Img" + ID
			screen.setImageButton(Btn, "", x, y, iSize, iSize, eWidGen, 0, 0)
			if not i:
				screen.setStyle(Btn, "SF_CtrlTheme_Civ4_Control_DarkMarbleButton_Style")
			else: screen.setStyle(Btn, "GFC_Control_EtchedButton_Style")
			artPath = szPath + aList[i]
			screen.addDDSGFCAt(Img, Btn, artPath, 4, 4, dA, dA, eWidGen, 0, 0, True)
			screen.setHitTest(Img, HitTestTypes.HITTEST_NOHIT)
			x += dx + 2

		w = self.xMidR - x - 14
		Pnl = "ProcessPanel"
		screen.addScrollPanel(Pnl, "", x, y-8, w, iSize-6, PanelStyles.PANEL_STYLE_EMPTY)
		screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
		x = 0
		for i in xrange(self.iNumProcessInfos):
			if CyCity.canMaintain(i):
				BTN = GC.getProcessInfo(i).getButton()
				Btn = "WID|PROCESS|CityWork" + str(i)
				screen.setImageButtonAt(Btn, Pnl, BTN, x, 0, iSize, iSize, eWidGen, 1, 1)
				x += dx

		# Build Lists
		y += dx + 2
		w = self.xMidR - self.xMidL - 16
		Pnl = "ScrollPanelBL"
		screen.addScrollPanel(Pnl, "", self.xMidL-8, y-8, w, iSize-6, PanelStyles.PANEL_STYLE_EMPTY)
		screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
		CyPlayer = self.InCity.CyPlayer
		iListNum = CyPlayer.getBLNumLists()
		x = 0
		for i in xrange(iListNum):
			if CyPlayer.getBLListLength(i) > 0:
				order = CyPlayer.getBLOrder(i, 0)
				Btn = "WID|LIST|CityWork" + str(CyPlayer.getBLID(i))
				if order.eOrderType == OrderTypes.ORDER_TRAIN:
					BTN = GC.getUnitInfo(order.iData1).getButton()
				elif order.eOrderType == OrderTypes.ORDER_CONSTRUCT:
					BTN = GC.getBuildingInfo(order.iData1).getButton()
				else:
					BTN = GC.getProjectInfo(order.iData1).getButton()
				screen.setImageButtonAt(Btn, Pnl, BTN, x, 0, iSize, iSize, eWidGen, 1, 1)
				x += dx


	def openCityTab(self, screen, iTab):
		if not self.bCityChange and self.iCityTab == iTab:
			return
		eWidGen = WidgetTypes.WIDGET_GENERAL

		screen.hide("CT|UnitGrouping")
		screen.hide("CT|UnitSorting")
		screen.hide("CT|BuildingSorting")

		if self.iCityTab == CITYTAB_ADMIN and self.iCityTab != iTab:
			screen.deleteWidget("CT|AdminLastOutput")
			screen.deleteWidget("PreviousOutput")
			if not self.bCityScreen:
				screen.deleteWidget("CT|Cityzoom0")

		self.iCityTab = iTab
		xRes = self.xRes
		a4thX = xRes/4
		halfX = xRes/2

		yMax = self.yBotBar + 18

		screen.setImageButton("CityTab|BG0", "", 0, 0, xRes, yMax - 6, eWidGen, 1, 1)

		# Header bar
		ID = "CityTabHeader"
		screen.addPanel(ID, "", "", True, True, a4thX + 4, 138, halfX - 8, 60, PanelStyles.PANEL_STYLE_MAIN)
		screen.setStyle(ID, "Panel_City_Title_Style")

		if iTab == CITYTAB_ADMIN:
			txt = "<font=4><color=100,100,100>" + TRNSLTR.getText("TXT_KEY_BUG_OPTLABEL_ROMSETTINGS__CITYMANAGEMENT", ())
			if self.bCityScreen:
				screen.setLabelAt(ID + "Text", ID, txt, 1<<2, (halfX - 8) / 2, 2, 0, FontTypes.GAME_FONT, eWidGen, 1, 2)
			else:
				city = self.InCity.CyCity
				screen.setText("CT|Cityzoom0", ID, txt + " <color=60,200,240>" + city.getName(), 1<<2, halfX - 8, 140, 0, FontTypes.GAME_FONT, eWidGen, 1, 2)

		elif iTab == CITYTAB_UNIT:
			self.fillUnitCityTabHeader(screen)
		else:
			self.fillBuildingCityTabHeader(screen)

		if self.bCityScreen:
			x = a4thX
			w = halfX
		else:
			x = self.xMidL
			w = self.xMidR - x

		y = 166
		h = yMax - y
		screen.addPanel("CityTabWindow", "", "", True, True, x, y, w, h, PanelStyles.PANEL_STYLE_MAIN)
		h -= 28
		self.updateCityTab(screen, x, y, w, h)
		screen.setText("CT|Options0", "", "<img=Art/Interface/Buttons/general/optionIcon1.dds>", 1<<0, xRes - xRes/4 - 42, 138, 0, FontTypes.GAME_FONT, eWidGen, 0, 0)

	def fillUnitCityTabHeader(self, screen):
		CyCity = self.InCity.CyCity
		# Filter and grouping
		iFilterWidth = self.xRes/2 - self.wUnitGroupButton - self.wUnitSortButton - 42
		if iFilterWidth > 13 * 30 + 24:
			iFilterWidth = 13 * 30 + 24
		else:
			iMod = iFilterWidth % 30
			if iMod > 24:
				iFilterWidth -= iMod - 24
			elif iMod < 24:
				iFilterWidth -= 6 + iMod

		ScPnl = "ScrlPnlFilters"
		PF = "CT|UnitFilter"
		screen.addScrollPanel(ScPnl, "", self.xRes/4, 132, iFilterWidth, 17, PanelStyles.PANEL_STYLE_EMPTY)
		screen.setStyle(ScPnl, "ScrollPanel_CityTabFilter_Style")

		aList = [
			[int(UnitFilterTypes.UNIT_FILTER_HIDE_UNBUILDABLE),	self.artPathButtonNoProduction],
			[int(UnitFilterTypes.UNIT_FILTER_HIDE_LIMITED),		self.artFILTER_SHOW_UNLIMITED],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_LAND),		self.artPathFilterLandPLE],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_AIR),			self.artPathFilterAirPLE],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_WATER),		self.artPathFilterSeaPLE],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_WORKERS),		self.artPathTechWorkerSpeed],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_CIVILIAN),	self.artPathFilterCivilianPLE],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_SIEGE),		self.artPathFilterSiegePLE],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_MOUNTED),		self.artPathTechMoveBonus],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_HEROES),		self.artPathFilterHeroPLE],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_MILITARY),	self.artFILTER_SHOW_MILITARY],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_DEFENSE),		self.artPathButtonMilitary],
			[int(UnitFilterTypes.UNIT_FILTER_SHOW_MISSIONARY),	self.artPathAngryCitizen]
		]
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eBtnLabel = ButtonStyles.BUTTON_STYLE_LABEL
		x = -1
		y = 1
		while aList:
			iFilter, art = aList.pop(0)
			name = PF + str(iFilter)
			screen.addCheckBoxGFCAt(ScPnl, name, art, self.artPathHilite, x, y, 30, 30, eWidGen, 1, 2, eBtnLabel, False)
			screen.setState(name, CyCity.getUnitListFilterActive(iFilter))
			x += 30
			if x > iFilterWidth - 48:
				x = -1
				y += 30

		screen.moveToFront("CT|UnitGrouping")
		screen.moveToFront("CT|UnitSorting")
		screen.show("CT|UnitGrouping")
		screen.show("CT|UnitSorting")

	def fillBuildingCityTabHeader(self, screen):
		CyCity = self.InCity.CyCity
		# Filter and grouping
		iFilterWidth = self.xRes/2 - self.wBuildingSortButton - 38
		if iFilterWidth > 20 * 30 + 24:
			iFilterWidth = 20 * 30 + 24
		else:
			iMod = iFilterWidth % 30
			if iMod > 24:
				iFilterWidth -= iMod - 24
			elif iMod < 24:
				iFilterWidth -= 6 + iMod

		ScPnl = "ScrlPnlFilters"
		PF = "CT|BuildingFilter"
		screen.addScrollPanel(ScPnl, "", self.xRes/4, 132, iFilterWidth, 17, PanelStyles.PANEL_STYLE_EMPTY)
		screen.setStyle(ScPnl, "ScrollPanel_CityTabFilter_Style")

		aList = [
			[int(BuildingFilterTypes.BUILDING_FILTER_HIDE_UNBUILDABLE),		self.artPathButtonNoProduction],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_SCIENCE),			self.artPathFilterResearch],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_ESPIONAGE),		self.artPathFilterEspionage],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_CULTURE),			self.artPathFilterCulture],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_GOLD),			self.artPathRawYieldsCommerce],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_FOOD),			self.artPathRawYieldsFood],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_PRODUCTION),		self.artPathRawYieldsProduction],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_HAPPINESS),		self.artFILTER_SHOW_HAPPY],
			[int(BuildingFilterTypes.BUILDING_FILTER_HIDE_UNHAPPINESS),		self.artFILTER_HIDE_UNHAPPY],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_HEALTH),			self.artFILTER_SHOW_HEALTH],
			[int(BuildingFilterTypes.BUILDING_FILTER_HIDE_UNHEALTHINESS),	self.artFILTER_HIDE_UNHEALTH],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_MILITARY),		self.artFILTER_SHOW_MILITARY],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_CITY_DEFENSE),	self.artPathButtonMilitary],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_CRIME),			"Art/Interface/buttons/filters/showcrimeOff.dds"],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_DISEASE),			"Art/Interface/buttons/filters/showdiseaseOff.dds"],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_EDUCATION),		"Art/Interface/buttons/filters/showeducationOff.dds"],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_FLAMMABILITY),	"Art/Interface/buttons/filters/showfireOff.dds"],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_WATER_POLLUTION),	"Art/Interface/buttons/filters/showwaterpollutionOff.dds"],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_AIR_POLLUTION),	"Art/Interface/buttons/filters/showairpollutionoff.dds"],
			[int(BuildingFilterTypes.BUILDING_FILTER_SHOW_TOURISM),			"Art/Interface/buttons/filters/showtourismOff.dds"]
		]
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eBtnLabel = ButtonStyles.BUTTON_STYLE_LABEL
		x = -1
		y = 1
		while aList:
			iFilter, art = aList.pop(0)
			name = PF + str(iFilter)
			screen.addCheckBoxGFCAt(ScPnl, name, art, self.artPathHilite, x, y, 30, 30, eWidGen, 1, 2, eBtnLabel, False)
			screen.setState(name, CyCity.getBuildingListFilterActive(iFilter))
			x += 30
			if x > iFilterWidth - 48:
				x = -1
				y += 30

		screen.moveToFront("CT|BuildingSorting")
		screen.show("CT|BuildingSorting")


	def updateCityTab(self, screen, x=-1, y0=166, w=-1, h=-1):

		iTab = self.iCityTab
		if -1 in (x, w, h):
			if self.bCityScreen:
				x = self.xRes/4
				w = self.xRes/2
			else:
				x = self.xMidL
				w = self.xMidR - x
			h = self.yBotBar - y0 - 10

		if iTab == CITYTAB_ADMIN:
			self.updateCityAdmin(screen, x, y0, w, h)
			return

		eWidGen = WidgetTypes.WIDGET_GENERAL
		InCity = self.InCity
		city = InCity.CyCity
		iSize = CityOpt.getBuildIconSize()

		Pnl = "CityTabScrPnl"
		screen.addScrollPanel(Pnl, "", x, y0, w, h, PanelStyles.PANEL_STYLE_MAIN)
		screen.setStyle(Pnl, "ScrollPanel_Alt_Style")

		if InCity.WorkQueue and not city.getProductionProgress():
			self.bFreshQueue = False
		else: self.bFreshQueue = True

		iPlayer = InCity.iPlayer
		iPlayerAct = self.iPlayer
		dx = iSize + 4
		iBtnPerRow = (w - 16) / dx
		xStart = x = (w - 16) % (dx * iBtnPerRow) / 2
		y = 0

		if iTab == CITYTAB_UNIT:
			city.setUnitListInvalid()
			iGroups = city.getUnitListGroupNum()
			PF = "WID|UNIT|CityWork%d"
			for iGroup in xrange(iGroups):
				aList = []
				iCount = 0
				iNumInGroup = city.getUnitListNumInGroup(iGroup)
				if not iNumInGroup:
					continue
				for i in xrange(iNumInGroup):
					iType = city.getUnitListType(iGroup, i)
					BTN = GC.getUnitInfo(iType).getButton()
					Img = PF % iType
					if not city.canTrain(iType, False, False, False, False):
						aList.append([Img, iType, BTN])
					else:
						if iCount == iBtnPerRow:
							x = xStart
							y += dx
							iCount = 0
						screen.setImageButtonAt(Img, Pnl, BTN, x, y, iSize, iSize, eWidGen, 1, 1)
						x += dx
						iCount += 1
				for Img, iType, BTN in aList:
					if iCount == iBtnPerRow:
						x = xStart
						y += dx
						iCount = 0
					screen.setImageButtonAt(Img, Pnl, BTN, x, y, iSize, iSize, eWidGen, 1, 1)
					screen.enable(Img, False)
					x += dx
					iCount += 1
				x = xStart
				y += dx
		elif iTab == CITYTAB_BUILDING:
			if not city.getBuildingListGroupNum(): return
			iNumInGroup = city.getBuildingListNumInGroup(0)
			if not iNumInGroup: return
			PF = "WID|BUILDING|CityWork%d"
			aList = []
			iCount = 0
			i = 0
			while i < iNumInGroup:
				iType = city.getBuildingListType(0, i)
				if isLimitedWonder(iType):
					break
				i += 1
				BTN = GC.getBuildingInfo(iType).getButton()
				Img = PF % iType
				if not city.canConstruct(iType, False, False, False) or iPlayer != iPlayerAct:
					aList.append([Img, iType, BTN])
				else:
					if iCount == iBtnPerRow:
						x = xStart
						y += dx
						iCount = 0
					screen.setImageButtonAt(Img, Pnl, BTN, x, y, iSize, iSize, eWidGen, 1, 1)
					x += dx
					iCount += 1
			while aList:
				Img, iType, BTN = aList.pop(0)
				if iCount == iBtnPerRow:
					x = xStart
					y += dx
					iCount = 0
				screen.setImageButtonAt(Img, Pnl, BTN, x, y, iSize, iSize, eWidGen, 1, 1)
				screen.enable(Img, False)
				x += dx
				iCount += 1
		elif iTab == CITYTAB_WONDER:
			PF = "WID|BUILDING|CityWork%d"
			aList0 = []
			aList1 = []
			aList2 = []
			aList3 = []
			iGroupNum = city.getBuildingListGroupNum()
			iGroup = 0
			while iGroup < iGroupNum:
				iNumInGroup = city.getBuildingListNumInGroup(iGroup)
				if not iNumInGroup:
					iGroup += 1
					continue
				for i in xrange(iNumInGroup):
					iType = city.getBuildingListType(iGroup, i)

					if not isLimitedWonder(iType):
						break
					CvBuildingInfo = GC.getBuildingInfo(iType)

					if CvBuildingInfo.isNoLimit():
						aList0.append(iType)

					elif isNationalWonder(iType):
						aList1.append(iType)

					elif isTeamWonder(iType):
						aList2.append(iType)
					else:
						aList3.append(iType)
				iGroup += 1

			# Wonders
			iCount = 0
			for aListX in [aList1, aList2, aList3]:
				if not aListX: continue
				aList = []
				for iType in aListX:
					BTN = GC.getBuildingInfo(iType).getButton()
					Img = PF % iType
					if not city.canConstruct(iType, False, False, False) or iPlayer != iPlayerAct:
						aList.append([Img, iType, BTN])
					else:
						if iCount == iBtnPerRow:
							x = xStart
							y += dx
							iCount = 0
						screen.setImageButtonAt(Img, Pnl, BTN, x, y, iSize, iSize, eWidGen, 1, 1)
						x += dx
						iCount += 1
				while aList:
					Img, iType, BTN = aList.pop(0)
					if iCount == iBtnPerRow:
						x = xStart
						y += dx
						iCount = 0
					screen.setImageButtonAt(Img, Pnl, BTN, x, y, iSize, iSize, eWidGen, 1, 1)
					screen.enable(Img, False)
					x += dx
					iCount += 1
				x = xStart
				y += dx
				iCount = 0

			# Projects
			for iType in xrange(self.iNumProjectInfos):
				if city.canCreate(iType, True, False):
					if iCount == iBtnPerRow:
						x = xStart
						y += dx
						iCount = 0
					BTN = GC.getProjectInfo(iType).getButton()
					Img = "WID|PROJECT|CityWork%d" % iType
					screen.setImageButtonAt(Img, Pnl, BTN, x, y, iSize, iSize, eWidGen, 1, 1)
					x += dx
					iCount += 1

			# Cultures + palace + special
			if not aList0: return
			if iCount:
				x = xStart
				y += dx
				iCount = 0
			aList = []
			for iType in aList0:
				BTN = GC.getBuildingInfo(iType).getButton()
				Img = PF % iType
				if not city.canConstruct(iType, False, False, False) or iPlayer != iPlayerAct:
					aList.append([Img, iType, BTN])
				else:
					if iCount == iBtnPerRow:
						x = xStart
						y += dx
						iCount = 0
					screen.setImageButtonAt(Img, Pnl, BTN, x, y, iSize, iSize, eWidGen, 1, 1)
					x += dx
					iCount += 1
			while aList:
				Img, iType, BTN = aList.pop(0)
				if iCount == iBtnPerRow:
					x = xStart
					y += dx
					iCount = 0
				screen.setImageButtonAt(Img, Pnl, BTN, x, y, iSize, iSize, eWidGen, 1, 1)
				screen.enable(Img, False)
				x += dx
				iCount += 1

	def updateCityAdmin(self, screen, x, y0, w, h):

		uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2, uFont1b, uFont1 = self.aFontList
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT
		ePanelBlack	= PanelStyles.PANEL_STYLE_MAIN_BLACK25
		ePnlStyleBlue50 = PanelStyles.PANEL_STYLE_BLUE50

		InCity = self.InCity
		city = InCity.CyCity
		iSize = CityOpt.getBuildIconSize()

		if self.iResID == 2:
			dy = 24
		elif self.iResID == 1:
			dy = 22
		else:
			dy = 20

		x2 = x + 8
		y2 = y0 + 32
		h2 = h - 44
		w2 = w/2 - 12

		w3 = w2 - 12

		PnlLeft = "CT|AdminLastOutput"
		PnlRight = "CityTabScrPnl"
		ROW = "CT|AdminEntry"

		screen.setLabelAt("", "CityTabWindow", uFont3b + TRNSLTR.getText("TXT_KEY_PREVIOUS_OUTPUT", ()), 1<<0, 12, 8, 0, eFontGame, eWidGen, 1, 2)
		screen.setLabelAt("", "CityTabWindow", uFont3b + TRNSLTR.getText("TXT_WORD_RECOMMENDATIONS", ()), 1<<0, w/2 + 8, 8, 0, eFontGame, eWidGen, 1, 2)
		screen.addPanel("PreviousOutput", "", "", True, True, x + 12, y2 - 2, w3 + 4, h2 + 28, ePanelBlack)

		screen.addScrollPanel(PnlLeft, "", x2, y2, w2, h2, PanelStyles.PANEL_STYLE_MAIN)
		screen.setStyle(PnlLeft, "ScrollPanel_Alt_Style")
		screen.addScrollPanel(PnlRight, "", x2 + w2, y2, w2 + 6, h2, PanelStyles.PANEL_STYLE_MAIN)
		screen.setStyle(PnlRight, "ScrollPanel_Alt_Style")

		y = 5
		n = 0
		history = city.getCityOutputHistory()

		for iHistory in xrange(history.getSize()):
			iTurn = history.getRecentOutputTurn(iHistory)
			if iTurn < 1: break
			iNumEntries = history.getCityOutputHistoryNumEntries(iHistory)

			Pnl = ROW + str(n)
			screen.attachPanelAt(PnlLeft, Pnl, "", "", True, False, ePnlStyleBlue50, 6, y - 8, w3 - 16, 10 + iNumEntries * dy + dy, eWidGen, 1, 2)
			screen.setLabelAt("", Pnl, uFont2b + TRNSLTR.getText("TXT_KEY_TIME_TURN", (iTurn,)), 1<<0, 4, 2, 0, eFontGame, eWidGen, 1, 2)
			y1 = dy
			for iEntry in xrange(iNumEntries):

				iOrder = history.getCityOutputHistoryEntry(iHistory, iEntry, True)
				iType = history.getCityOutputHistoryEntry(iHistory, iEntry, False)

				if iOrder == OrderTypes.ORDER_TRAIN:

					info = GC.getUnitInfo(iType)

					if not iEntry and iTurn == GAME.getGameTurn():
						sound = info.getArtInfo(0, GC.getPlayer(city.getOwner()).getCurrentEra(), UnitArtStyleTypes(-1)).getTrainSound()

					TXT = "WID|UNIT|OUTPUTLOG%d|%d" %(iType, n)

				elif iOrder == OrderTypes.ORDER_CONSTRUCT:

					info = GC.getBuildingInfo(iType)

					if not iEntry and iTurn == GAME.getGameTurn():
						sound = info.getConstructSound()

					TXT = "WID|BUILDING|OUTPUTLOG%d|%d" %(iType, n)

				elif iOrder == OrderTypes.ORDER_CREATE:

					info = GC.getProjectInfo(iType)

					if not iEntry and iTurn == GAME.getGameTurn():
						sound = info.getCreateSound()

					TXT = "WID|PROJECT|OUTPUTLOG%d|%d" %(iType, n)


				if not iEntry and iTurn == GAME.getGameTurn():
					screen.setPanelColor(Pnl, 128, 255, 128)
					CyIF.playGeneralSound(sound)

				screen.setTextAt(TXT, Pnl, "%s<img=%s size=%d> %s" %(uFont2, info.getButton(), dy-4, info.getDescription()), 1<<0, 8, y1, 0, eFontGame, eWidGen, 1, 2)

				n += 1
				y1 += dy
			y += y1 + 10
		y = 0

		iBestUnit = city.AI_bestUnit()
		if iBestUnit > -1:
			szRow = str(n)
			screen.attachPanelAt(PnlRight, ROW + szRow, "", "", True, False, ePanelBlack, -2, y - 4, w3, iSize + 3, eWidGen, 1, 2)
			unitInfo = GC.getUnitInfo(iBestUnit)
			szUnitName = unitInfo.getDescription()
			iAdvisor = unitInfo.getAdvisorType()
			if iAdvisor > -1:
				szTxt = uFont3 + TRNSLTR.getText("TXT_KEY_POPUP_RECOMMENDED", (szUnitName, city.getUnitProductionTurnsLeft(iBestUnit, 0), GC.getAdvisorInfo(iAdvisor).getDescription()))
			else:
				szTxt = uFont3 + TRNSLTR.getText("TXT_KEY_POPUP_RECOMMENDED", (szUnitName, city.getUnitProductionTurnsLeft(iBestUnit, 0), GC.getUnitCombatInfo(unitInfo.getUnitCombatType()).getDescription()))

			PF = "WID|UNIT|CityWork%d"
			screen.addDDSGFCAt("", ROW + szRow, unitInfo.getButton(), -1, -1, iSize, iSize, eWidGen, 1, 2, False)
			screen.setImageButtonAt((PF % iBestUnit) + "|img" + szRow, ROW + szRow, "", 0, 4, w3, iSize + 3, eWidGen, 1, 2)
			screen.setTextAt((PF % iBestUnit) + "|" + szRow, ROW + szRow, szTxt, 1<<0, iSize + 4, iSize/2 - dy/2 - 2, 0, eFontGame, eWidGen, 1, 2)
			n += 1
			y += iSize + 4

		iBestUnit = city.AI_bestUnitAI(UnitAITypes.UNITAI_WORKER)
		if iBestUnit > -1 and not self.isUnitMaxedOut(iBestUnit, InCity):
			szRow = str(n)
			screen.attachPanelAt(PnlRight, ROW + szRow, "", "", True, False, ePanelBlack, 0, y - 4, w3, iSize + 2, eWidGen, 1, 2)
			unitInfo = GC.getUnitInfo(iBestUnit)
			szTxt = uFont3 + TRNSLTR.getText("TXT_KEY_POPUP_RECOMMENDED", (unitInfo.getDescription(), city.getUnitProductionTurnsLeft(iBestUnit, 0), "TXT_KEY_UNIT_WORKER"))

			PF = "WID|UNIT|CityWork%d"
			screen.addDDSGFCAt("", ROW + szRow, unitInfo.getButton(), -2, 0, iSize, iSize, eWidGen, 1, 2, False)
			screen.setImageButtonAt((PF % iBestUnit) + "|img" + szRow, ROW + szRow, "", 0, 4, w3, iSize + 3, eWidGen, 1, 2)
			screen.setTextAt((PF % iBestUnit) + "|" + szRow, ROW + szRow, szTxt, 1<<0, iSize + 4, iSize/2 - dy/2, 0, eFontGame, eWidGen, 1, 2)
			n += 1
			y += iSize + 4

		iBestUnit = city.AI_bestUnitAI(UnitAITypes.UNITAI_SETTLE)
		if iBestUnit > -1 and not self.isUnitMaxedOut(iBestUnit, InCity):
			szRow = str(n)
			screen.attachPanelAt(PnlRight, ROW + szRow, "", "", True, False, ePanelBlack, 0, y - 4, w3, iSize + 2, eWidGen, 1, 2)
			unitInfo = GC.getUnitInfo(iBestUnit)
			szTxt = uFont3 + TRNSLTR.getText("TXT_KEY_POPUP_RECOMMENDED", (unitInfo.getDescription(), city.getUnitProductionTurnsLeft(iBestUnit, 0), "TXT_KEY_UNIT_SETTLER"))

			PF = "WID|UNIT|CityWork%d"
			screen.addDDSGFCAt("", ROW + szRow, unitInfo.getButton(), -2, 0, iSize, iSize, eWidGen, 1, 2, False)
			screen.setImageButtonAt((PF % iBestUnit) + "|img" + szRow, ROW + szRow, "", 0, 4, w3, iSize + 3, eWidGen, 1, 2)
			screen.setTextAt((PF % iBestUnit) + "|" + szRow, ROW + szRow, szTxt, 1<<0, iSize + 4, iSize/2 - dy/2, 0, eFontGame, eWidGen, 1, 2)
			n += 1
			y += iSize + 4


	def isUnitMaxedOut(self, iType, InCity, iExtra=0):
		return (
			GAME.isUnitMaxedOut(iType, InCity.CyTeam.getUnitMaking(iType) + iExtra)
			or InCity.CyPlayer.isUnitMaxedOut(iType, InCity.CyPlayer.getUnitMaking(iType) + iExtra)
		)

	def exitCityTab(self, screen):
		screen.deleteWidget("CityTab|BG0")
		screen.deleteWidget("CityTabWindow")
		screen.deleteWidget("CityTabHeader")
		screen.deleteWidget("CityTabScrPnl")
		if self.iCityTab == CITYTAB_ADMIN:
			screen.deleteWidget("CT|AdminLastOutput")
			screen.deleteWidget("PreviousOutput")
			if not self.bCityScreen:
				screen.deleteWidget("CT|Cityzoom0")
		screen.deleteWidget("CT|Options0")
		screen.deleteWidget("ScrlPnlFilters")
		screen.hide("CT|UnitGrouping")
		screen.hide("CT|UnitSorting")
		screen.hide("CT|BuildingSorting")
		self.iCityTab = -1

	# Will update the info pane strings
	def updateInfoPaneStrings(self, screen):
		print "updateInfoPaneStrings"
		panel = "SelectedEntityPanel"
		label = "SelectedUnitLabel"
		unitTable = "SelectedUnitTable"
		promoPanel = "PromoPanel"
		screen.hide(panel)
		screen.hide(label)
		screen.hide(unitTable)
		screen.hide(promoPanel)

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT

		CyUnit = CyIF.getHeadSelectedUnit()

		yRes = self.yRes
		xMidL = self.xMidL
		yBotBar = self.yBotBar
		IFT = self.iInterfaceType
		InCity = self.InCity
		if InCity:
			CyCity = InCity.CyCity
			if self.bCityScreen:
				self.updateCityPoPrBars(screen, CyCity, eWidGen, eFontGame)

			if self.bBuildWorkQueue:
				self.buildCityWorkQueue(screen, InCity)
				self.bBuildWorkQueue = False

			elif InCity.WorkQueue:
				aList = self.NewQueueRowTime
				if aList:
					TYPE, iType, szRow, iNode, bAlt, x, szTxt = aList

					if TYPE == "UNIT":
						bPre = CyCity.getProgressOnUnit(iType) > 0
						szTxt += str(CyCity.getUnitProductionTurnsLeft(iType, iNode))

					elif TYPE == "BUILDING":
						bPre = CyCity.getProgressOnBuilding(iType) > 0
						szTxt += str(CyCity.getBuildingProductionTurnsLeft(iType, iNode))

					elif TYPE == "PROJECT":
						bPre = CyCity.getProjectProduction(iType) > 0
						szTxt += str(CyCity.getProjectProductionTurnsLeft(iType, iNode))
					else:
						szTxt = ""
					self.NewQueueRowTime = []
				else:
					aList = InCity.WorkQueue[0]
					TYPE = aList[0].split("|")[1]
					if TYPE in ("UNIT", "BUILDING", "PROJECT"):
						szRow = aList[2]
						bAlt = CyIF.getOrderNodeSave(0)
						x = self.xMidL-146
						bPre = CyCity.getProductionProgress() > 0
						szTxt = self.aFontList[5] + str(CyCity.getGeneralProductionTurnsLeft())
					else:
						szTxt = ""
				if szTxt:
					if bPre:
						szTxt = "<color=0,255,255,255>" + szTxt + "</color>"
					if TYPE == "UNIT" and bAlt:
						szTxt += "*"
					screen.setLabelAt("QueueTime"+szRow, "QueueRow"+szRow, szTxt, 1<<0, x, 0, 0, eFontGame, eWidGen, 1, 1)

		elif CyUnit and IFT == InterfaceVisibility.INTERFACE_SHOW:
			y = yBotBar + 58
			h = yRes - y - 8
			w = xMidL - 90
			screen.addTableControlGFC(unitTable, 2, 4, y, w, h, False, False, 24, 24, TableStyles.TABLE_STYLE_EMPTY)
			screen.setStyle(unitTable, "Table_EmptyScroll_Style")
			iRow = 0

			if CyIF.mirrorsSelectionGroup():
				CySelectionGroup = CyUnit.getGroup()
				iMissionCount = CySelectionGroup.getLengthMissionQueue()
			else:
				CySelectionGroup = 0
				iMissionCount = 0
			aPromoList = []
			iSelectionRange = CyIF.getLengthSelectionList()
			fMoveDenominator = self.fMoveDenominator
			if iSelectionRange > 1:
				screen.setTableColumnHeader(unitTable, 0, "", w - 70)
				screen.setTableColumnHeader(unitTable, 1, "", 60)
				# Stack Title
				szBuffer = self.aFontList[3] + TRNSLTR.getText("TXT_KEY_UNITHELP_STACK", (iSelectionRange,))
				# Stack movement
				iMinMoves = 100000
				iMaxMoves = 0
				for i in xrange(iSelectionRange):
					pUnit = CyIF.getSelectionUnit(i)
					iMovesLeft = pUnit.movesLeft()
					if iMovesLeft > iMaxMoves:
						iMaxMoves = iMovesLeft
					if iMovesLeft < iMinMoves:
						iMinMoves = iMovesLeft
				fMinMoves = iMinMoves / fMoveDenominator
				if iMinMoves == iMaxMoves:
					szBuffer += ' %.1f' % fMinMoves
				else:
					fMaxMoves = iMaxMoves / fMoveDenominator
					szBuffer += ' %.1f - %.1f' %(fMinMoves, fMaxMoves)
				szBuffer += self.iconMoves

				if self.iResID == 2:
					y = -2
				elif self.iResID == 1:
					y = -1
				else:
					y = 2

				screen.setTextAt(label, panel, szBuffer, 1<<0, 4, y, 0, eFontGame, eWidGen, 0, 0)
				#Stack Promotions
				for iPromo in xrange(self.iNumPromotionInfos):
					iCount = 0
					for i in xrange(iSelectionRange):
						pUnit = CyIF.getSelectionUnit(i)
						if pUnit.isHasPromotion(iPromo) and not pUnit.isPromotionOverriden(iPromo):
							iCount += 1
					if iCount:
						aPromoList.append((iPromo, iCount))
				# Unit type list
				if iMissionCount <= 1 or not CySelectionGroup:
					for i in xrange(GC.getNumUnitInfos()):
						iCount = CyIF.countEntities(i)
						if iCount:
							szBufferL = "<font=1>" + GC.getUnitInfo(i).getDescription()
							szBufferR = "<font=1>" + str(iCount)
							screen.appendTableRow(unitTable)
							screen.setTableText(unitTable, 0, iRow, szBufferL, "", WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, i, 1, 1<<0)
							screen.setTableText(unitTable, 1, iRow, szBufferR, "", WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, i, 1, 1<<0)
							iRow += 1
			else:
				A = w / 2
				screen.setTableColumnHeader(unitTable, 0, "", A - 10)
				screen.setTableColumnHeader(unitTable, 1, "", A)
				iHotKeyNumber = CyUnit.getHotKeyNumber()
				szBuffer = self.aFontList[3]
				if iHotKeyNumber != -1:
					szBuffer += TRNSLTR.getText("INTERFACE_PANE_UNIT_NAME_HOT_KEY", (iHotKeyNumber, CyUnit.getName()))
				else:
					szBuffer += TRNSLTR.getText("INTERFACE_PANE_UNIT_NAME", (CyUnit.getName(),))

				if self.iResID == 2:
					y = -2
				elif self.iResID == 1:
					y = -1
				else:
					y = 2

				screen.setTextAt(label, panel, szBuffer, 1<<0, 4, y, 0, eFontGame, WidgetTypes.WIDGET_UNIT_NAME, 0, 0)
				# Get Promotions
				for iPromo in xrange(self.iNumPromotionInfos):
					if CyUnit.isHasPromotion(iPromo) and not CyUnit.isPromotionOverriden(iPromo):
						aPromoList.append((iPromo, 1))
				if not CySelectionGroup or iMissionCount <= 1:
					if CyUnit.getDomainType() == DomainTypes.DOMAIN_AIR:
						strengthBase = CyUnit.airBaseCombatStr()
						szTxt1 = self.szInterfacePaneStrengthAir
					else:
						strengthBase = CyUnit.baseCombatStr()
						szTxt1 = self.szInterfacePaneStrength
					if strengthBase:
						if self.GO_SIZE_MATTERS:
							strengthBase /= 100.0

						szTxt2 = ""
						if CyUnit.isHurt():
							fPercentHP = float(CyUnit.getHP()) / CyUnit.getMaxHP()
							fStrength = strengthBase * fPercentHP
							szTxt2 += TextUtil.floatToString(fStrength) + "/"

						szTxt2 += TextUtil.floatToString(strengthBase) + self.iconStrength

						screen.appendTableRow(unitTable)
						screen.setTableText(unitTable, 0, iRow, "<font=1>" + szTxt1, "", eWidGen, 0, 0, 1<<0)
						screen.setTableText(unitTable, 1, iRow, "<font=1>" + szTxt2, "", eWidGen, 0, 0, 1<<0)
						iRow += 1
					# Unit Movement Fraction
					szTxt1 = self.szInterfacePaneMovement
					iBaseMoves = CyUnit.baseMoves()
					iMovesLeft = CyUnit.movesLeft()
					if not iMovesLeft:
						szTxt2 = "0/"
					elif iMovesLeft < iBaseMoves * fMoveDenominator:
						szTxt2 = TextUtil.floatToString(iMovesLeft / fMoveDenominator) + "/"
					else:
						szTxt2 = ""
					szTxt2 += str(iBaseMoves) + self.iconMoves
					screen.appendTableRow(unitTable)
					screen.setTableText(unitTable, 0, iRow, "<font=1>" + szTxt1, "", eWidGen, 0, 0, 1<<0)
					screen.setTableText(unitTable, 1, iRow, "<font=1>" + szTxt2, "", eWidGen, 0, 0, 1<<0)
					iRow += 1

					iLevel = CyUnit.getLevel()
					if iLevel:

						szTxt1 = self.szInterfacePaneLevel
						szTxt2 = "%d" %(iLevel)

						screen.appendTableRow(unitTable)
						screen.setTableText(unitTable, 0, iRow, "<font=1>" + szTxt1, "", eWidGen, 0, 0, 1<<0)
						screen.setTableText(unitTable, 1, iRow, "<font=1>" + szTxt2, "", eWidGen, 0, 0, 1<<0)
						iRow += 1
					# Fractional XP
					fXP = CyUnit.getRealExperience()
					if fXP and not CyUnit.isInBattle():
						szXP = self.szInterfacePaneExperience
						screen.appendTableRow(unitTable)
						screen.setTableText(unitTable, 0, iRow, "<font=1>" + szXP, "", eWidGen, 0, 0, 1<<0)
						iRow += 1
						iNeedXP = CyUnit.experienceNeeded()
						szXP = TextUtil.floatToString(fXP) + "/" + str(iNeedXP)
						screen.appendTableRow(unitTable)
						screen.setTableText(unitTable, 0, iRow, "<font=1>" + szXP, "", eWidGen, 0, 0, 1<<0)
						iRow += 1
					# Great Commanders
					if CyUnit.isCommander():
						szTxt2 = u"%d/%d " %(CyUnit.getControlPointsLeft(), CyUnit.getControlPoints())
						screen.appendTableRow(unitTable)
						screen.setTableText(unitTable, 0, iRow, "<font=1>Control:", "", eWidGen, 0, 0, 1<<0)
						iRow += 1
						screen.appendTableRow(unitTable)
						screen.setTableText(unitTable, 1, iRow, "<font=1>" + szTxt2, "", eWidGen, 0, 0, 1<<0)
						iRow += 1
					if CyUnit.isCommodore():
						szTxt2 = u"%d/%d " %(CyUnit.getCommodoreControlPointsLeft(), CyUnit.getCommodoreControlPoints())
						screen.appendTableRow(unitTable)
						screen.setTableText(unitTable, 0, iRow, "<font=1>Control:", "", eWidGen, 0, 0, 1<<0)
						iRow += 1
						screen.appendTableRow(unitTable)
						screen.setTableText(unitTable, 1, iRow, "<font=1>" + szTxt2, "", eWidGen, 0, 0, 1<<0)
						iRow += 1
			if CySelectionGroup:
				if iMissionCount > 1:
					for i in xrange(iMissionCount):
						szTxt2 = ""
						if GC.getMissionInfo(CySelectionGroup.getMissionType(i)).isBuild():
							if not i:
								szTxt1 = GC.getBuildInfo(CySelectionGroup.getMissionData1(i)).getDescription()
								szTxt2 = TRNSLTR.getText("INTERFACE_CITY_TURNS", (CySelectionGroup.plot().getBuildTurnsLeft(CySelectionGroup.getMissionData1(i), 0, 0), ))
							else:
								szTxt1 = u"%s..." %(GC.getBuildInfo(CySelectionGroup.getMissionData1(i)).getDescription())
						else:
							szTxt1 = u"%s..." %(GC.getMissionInfo(CySelectionGroup.getMissionType(i)).getDescription())
						screen.appendTableRow(unitTable)
						screen.setTableText(unitTable, 0, iRow, "<font=1>" + szTxt1, "", eWidGen, 0, 0, 1<<0)
						screen.setTableText(unitTable, 1, iRow, "<font=1>" + szTxt2, "", eWidGen, 0, 0, 1<<0)
						iRow += 1
			# Display Promotions
			screen.show(promoPanel)
			if aPromoList != self.aSelUnitPromoList:
				aUnitPromoCountMap = {}
				for aPromo in self.aHideList:
					screen.hide(aPromo)
				aHideList = []
				x = 0
				szChild = "WID|PROMO|PromoPanel"
				for i, entry in enumerate(aPromoList):
					iPromo, iCount = entry
					aUnitPromoCountMap[iPromo] = iCount
					aPromo = szChild + str(iPromo)
					screen.moveItem(aPromo, x, 0, 0)
					screen.show(aPromo)
					aHideList.append(aPromo)
					x += 32
				self.aHideList = aHideList
				self.aUnitPromoCountMap = aUnitPromoCountMap
				self.aSelUnitPromoList = list(aPromoList)
			screen.show(panel)
		# Entity preview
		graphic = "InterfaceUnitModel"
		screen.hide(graphic)
		if CyIF.shouldDisplayUnitModel() and IFT != InterfaceVisibility.INTERFACE_HIDE_ALL:
			x = xMidL - 92
			y = yBotBar + 24
			w = 82
			h = yRes - y
			if InCity:
				if CyIF.getNumOrdersQueued():
					iOrderNode = CyIF.getOrderNodeType(0)
					if iOrderNode == OrderTypes.ORDER_TRAIN:
						iUnit = CyIF.getOrderNodeData1(0)
						screen.addUnitGraphicGFC(graphic, iUnit, x, y, w, h, eWidGen, iUnit, 0, -20, 30, 1, False)

					elif iOrderNode == OrderTypes.ORDER_CONSTRUCT:
						iBuilding = CyIF.getOrderNodeData1(0)
						screen.addBuildingGraphicGFC(graphic, iBuilding, x, y, w, h, eWidGen, 0, 0, -20, 30, 1, False)

					elif iOrderNode == OrderTypes.ORDER_CREATE:
						if GC.getProjectInfo(CyIF.getOrderNodeData1(0)).isSpaceship():
							iProject = CyIF.getOrderNodeData1(0)
							screen.addSpaceShipWidgetGFC(graphic, x, y, w, h, iProject, 0, eWidGen, 0, 0)
			elif CyUnit:
				iUnit = CyUnit.getUnitType()
				screen.addUnitGraphicGFC(graphic, iUnit, x, y, w, h, WidgetTypes.WIDGET_UNIT_MODEL, iUnit, 0, -20, 30, 1, False)
				screen.moveToFront("SelectedUnitLabel")
				screen.moveToFront("SelectedUnitTable")

	def updateCityPoPrBars(self, screen, CyCity, eWidGen, eFontGame):
		uFont2 = self.aFontList[5]
		halfX = self.xRes/2
		xPopProgBar = self.xPopProgBar
		# BUG - Food Assist
		bFoodProduction = CyCity.isFoodProduction()
		iFoodDifference = CyCity.foodDifference(True)
		iFoodStored = CyCity.getFood()
		iFoodToBe	= iFoodStored + iFoodDifference
		iGrowth		= CyCity.growthThreshold()

		if iFoodDifference:

			if not CyCity.AI_isEmphasize(5) and iFoodToBe >= iGrowth:
				szTxt = self.szInterfaceCityGrowth

			elif iFoodDifference > 0:
				szTxt = TRNSLTR.getText("INTERFACE_CITY_GROWING", (CyCity.getFoodTurnsLeft(), ))
			else:
				iTurnsToStarve = iFoodStored / -iFoodDifference + 1
				if iTurnsToStarve > 1:
					szTxt = TRNSLTR.getText("INTERFACE_CITY_SHRINKING", (iTurnsToStarve, ))
				else:
					szTxt = self.szInterfaceCityStarving
		else:
			szTxt = self.szInterfaceCityStagnant
		screen.setLabel("PopulationText", "", uFont2 + szTxt, 1<<2, halfX, 72, 0, eFontGame, eWidGen, 0, 0)
		screen.setHitTest("PopulationText", HitTestTypes.HITTEST_NOHIT)

		iFoodYield = CyCity.getYieldRate(YieldTypes.YIELD_FOOD)
		iFoodEaten = CyCity.foodConsumption(False, 0)
		if iFoodYield == iFoodEaten or bFoodProduction or CyCity.isDisorder():
			szTxt = TRNSLTR.getText("INTERFACE_CITY_FOOD_STAGNATE", (iFoodYield, iFoodYield))
		elif iFoodYield > iFoodEaten:
			szTxt = TRNSLTR.getText("INTERFACE_CITY_FOOD_GROW", (iFoodYield, iFoodEaten, iFoodYield - iFoodEaten))
		else:
			szTxt = TRNSLTR.getText("INTERFACE_CITY_FOOD_SHRINK", (iFoodYield, iFoodEaten, iFoodYield - iFoodEaten))

		screen.setLabel("PopulationInputText", "", uFont2 + szTxt, 1<<1, xPopProgBar - 6, 70, 0, eFontGame, WidgetTypes.WIDGET_FOOD_MOD_HELP, -1, -1)

		if iFoodDifference < 0:
			if iFoodToBe > 0:
				iDeltaFood = iFoodToBe
			else:
				iDeltaFood = 0
			if iFoodStored > -iFoodDifference:
				iExtraFood = -iFoodDifference
			else:
				iExtraFood = iFoodStored
			screen.setBarPercentage("PopulationBar", InfoBarTypes.INFOBAR_STORED, float(iDeltaFood) / iGrowth)
			screen.setBarPercentage("PopulationBar", InfoBarTypes.INFOBAR_RATE, 0.0)
			if iGrowth > iDeltaFood:
				screen.setBarPercentage("PopulationBar", InfoBarTypes.INFOBAR_RATE_EXTRA, float(iExtraFood) / (iGrowth - iDeltaFood))
			else:
				screen.setBarPercentage("PopulationBar", InfoBarTypes.INFOBAR_RATE_EXTRA, 0.0)
		else:
			screen.setBarPercentage("PopulationBar", InfoBarTypes.INFOBAR_STORED, float(iFoodStored) / iGrowth)
			if iGrowth > iFoodStored:
				screen.setBarPercentage("PopulationBar", InfoBarTypes.INFOBAR_RATE, float(iFoodDifference) / (iGrowth - iFoodStored))
			else:
				screen.setBarPercentage("PopulationBar", InfoBarTypes.INFOBAR_RATE, 0.0)
			screen.setBarPercentage("PopulationBar", InfoBarTypes.INFOBAR_RATE_EXTRA, 0.0)

		bProcess = CyCity.isProductionProcess()

		if bProcess or CyCity.getOrderQueueLength() < 1:
			iProductionDiffNoFood = CyCity.getYieldRate(YieldTypes.YIELD_PRODUCTION)
			if bProcess:
				szTxt = CyCity.getProductionName()
			else:
				szTxt = ""
			screen.setBarPercentage("ProductionBar", InfoBarTypes.INFOBAR_STORED, 0)
			screen.setBarPercentage("ProductionBar", InfoBarTypes.INFOBAR_RATE, 0)
			screen.setBarPercentage("ProductionBar", InfoBarTypes.INFOBAR_RATE_EXTRA, 0)
		else:
			iProductionDiffNoFood = CyCity.getCurrentProductionDifference(True, True)
			iProductionDiffJustFood = CyCity.getCurrentProductionDifference(False, True) - iProductionDiffNoFood
			iNeeded = CyCity.getProductionNeeded()
			iStored = CyCity.getProductionProgress()
			screen.setBarPercentage("ProductionBar", InfoBarTypes.INFOBAR_STORED, float(iStored) / iNeeded)
			if iNeeded > iStored:
				screen.setBarPercentage("ProductionBar", InfoBarTypes.INFOBAR_RATE, float(iProductionDiffNoFood) / (iNeeded - iStored))
			else:
				screen.setBarPercentage("ProductionBar", InfoBarTypes.INFOBAR_RATE, 0)
			if iNeeded > iStored + iProductionDiffNoFood:
				screen.setBarPercentage("ProductionBar", InfoBarTypes.INFOBAR_RATE_EXTRA, float(iProductionDiffJustFood) / (iNeeded - iStored - iProductionDiffNoFood))
			else:
				screen.setBarPercentage("ProductionBar", InfoBarTypes.INFOBAR_RATE_EXTRA, 0)
			# BUG - Whip Assist
			HURRY_WHIP = self.HURRY_POPULATION
			HURRY_BUY = self.HURRY_GOLD
			bShowWhipAssist = CityOpt.isShowWhipAssist()
			if bShowWhipAssist and CyCity.canHurry(HURRY_WHIP, False):
				iHurryPop = CyCity.hurryPopulation(HURRY_WHIP)
				iOverflow = CyCity.hurryProduction(HURRY_WHIP) - CyCity.productionLeft()
				if CityOpt.isWhipAssistOverflowCountCurrentProduction():
					iOverflow += CyCity.getCurrentProductionDifference(False, True)
				iMaxOverflow = CyCity.getMaxProductionOverflow()
				iLost = iOverflow - iMaxOverflow
				if iLost < 0: iLost = 0
				if iOverflow > iMaxOverflow:
					iOverflow = iMaxOverflow

				if iLost > 0:
					if CyCity.isProductionUnit():
						iGoldPercent = GC.getDefineINT("MAXED_UNIT_GOLD_PERCENT")
					elif CyCity.isProductionBuilding():
						iGoldPercent = GC.getDefineINT("MAXED_BUILDING_GOLD_PERCENT")
					elif CyCity.isProductionProject():
						iGoldPercent = GC.getDefineINT("MAXED_PROJECT_GOLD_PERCENT")
					else:
						iGoldPercent = 0
					iOverflowGold = iLost * iGoldPercent / 100
					szTxt = TRNSLTR.getText("INTERFACE_CITY_PRODUCTION_WHIP_PLUS_GOLD", (CyCity.getProductionNameKey(), CyCity.getProductionTurnsLeft(), iHurryPop, iOverflow, iOverflowGold))
				else:
					szTxt = TRNSLTR.getText("INTERFACE_CITY_PRODUCTION_WHIP", (CyCity.getProductionNameKey(), CyCity.getProductionTurnsLeft(), iHurryPop, iOverflow))
			elif bShowWhipAssist and CyCity.canHurry(HURRY_BUY, False):
				iHurryCost = CyCity.getHurryGold(HURRY_BUY)
				szTxt = TRNSLTR.getText("INTERFACE_CITY_PRODUCTION_BUY", (CyCity.getProductionNameKey(), CyCity.getProductionTurnsLeft(), iHurryCost))
			else:
				szTxt = TRNSLTR.getText("INTERFACE_CITY_PRODUCTION", (CyCity.getProductionNameKey(), CyCity.getProductionTurnsLeft()))
		screen.setLabel("ProductionText", "", uFont2 + szTxt, 1<<2, halfX, 98, 0, eFontGame, eWidGen, -1, -1)
		screen.setHitTest("ProductionText", HitTestTypes.HITTEST_NOHIT)

		szTxt = uFont2
		if not bProcess and bFoodProduction and iProductionDiffJustFood > 0:
			szTxt += str(iProductionDiffJustFood) + self.iconYieldList[0] + " + " + str(iProductionDiffNoFood) + self.iconYieldList[1]
		else:
			szTxt += str(iProductionDiffNoFood) + self.iconYieldList[1]
		screen.setText("CS|ProdYield0", "", szTxt, 1<<1, xPopProgBar - 6, 100, 0, eFontGame, eWidGen, 1, 2)

	def updateScoreStrings(self, screen):
		print "updateScoreStrings"

		screen.hide("ScoreBackground")
		NUM_PARTS = Scoreboard.NUM_PARTS
		for i in xrange(self.iMaxPcPlayers):
			screen.deleteWidget("ScoreText%d" %i)
			for j in xrange(NUM_PARTS):
				screen.deleteWidget("ScoreText%d-%d" %(i, j))

		techIconSB = self.techIconSB
		for iPlayer, iTech in techIconSB:
			screen.deleteWidget("WID|TECH|Score" + str(iTech) + "|" + str(iPlayer))
		techIconSB = []

		iCheck = self.xScoreboard
		IFT = self.iInterfaceType
		bReset_xScoreboard = True
		if IFT not in (InterfaceVisibility.INTERFACE_HIDE_ALL, InterfaceVisibility.INTERFACE_MINIMAP_ONLY):
			bCityScreen = self.bCityScreen
			if CyIF.isScoresVisible() and not bCityScreen and not self.bGlobeview:
				uFont = self.aFontList[7] # <font=1>
				# Base data
				bNetworkMP = self.bNetworkMP
				bMultiPlayer = self.bMultiPlayer
				bDebug = self.bDebugMode
				iTurn = GAME.getGameTurn()
				bScoresMinimized = CyIF.isScoresMinimized()
				bOutOfSync = CyIF.isOOSVisible()
				# This is you, the player who will look at the score-board.
				iPlayerAct = self.iPlayer
				CyPlayerAct = self.CyPlayer
				iTeamAct = self.iTeam
				CyTeamAct = self.CyTeam
				# Options
				bShowDeadCivs		= ScoreOpt.isShowDeadCivs()
				bShowDeadTag		= ScoreOpt.isShowDeadTag()
				bGreyOutDeadCivs	= ScoreOpt.isGreyOutDeadCivs()
				bShowMinorCivs		= ScoreOpt.isShowMinorCivs()
				bUsePlayerName		= ScoreOpt.isUsePlayerName()
				bShowBothNames		= ScoreOpt.isShowBothNames()
				bShowBothNamesShort	= ScoreOpt.isShowBothNamesShort()
				bShowLeaderName		= ScoreOpt.isShowLeaderName()
				bShowCivName		= ScoreOpt.isShowCivName()
				bShowScoreDelta		= ScoreOpt.isShowScoreDelta()
				bScoreInclCurrTurn	= ScoreOpt.isScoreDeltaIncludeCurrentTurn()
				bShowAttitude		= ScoreOpt.isShowAttitude()
				bShowWorstEnemy		= ScoreOpt.isShowWorstEnemy()
				bShowWHEOOH			= ScoreOpt.isShowWHEOOH()
				if self.GO_ONE_CITY_CHALLENGE:
					bShowCityCount	= False
				else:
					bShowCityCount	= ScoreOpt.isShowCountCities()
				bShowPower			= ScoreOpt.isShowPower()
				if bShowPower:
					iDemographicsMission = -1
					iSpyMissions = GC.getNumEspionageMissionInfos()
					for iMissionLoop in xrange(iSpyMissions):
						if GC.getEspionageMissionInfo(iMissionLoop).isSeeDemographics():
							iDemographicsMission = iMissionLoop
							break
					if iDemographicsMission == -1:
						bShowPower = False
				if bShowPower:
					iPlayerPower = CyPlayerAct.getPower()
					iPowerColor		= ScoreOpt.getPowerColor()
					iPowerColorHigh	= ScoreOpt.getHighPowerColor()
					iPowerColorLow	= ScoreOpt.getLowPowerColor()
					iPowerDecimals	= ScoreOpt.getPowerDecimals()
					iPowerRatioHigh	= ScoreOpt.getHighPowerRatio()
					iPowerRatioLow	= ScoreOpt.getLowPowerRatio()
					bPowerThemVsYou	= ScoreOpt.isPowerThemVersusYou()
					iconStrength = self.iconStrength
				# Misc
				iconHolyCityList = self.iconHolyCityList
				iconReligionList = self.iconReligionList
				szMinorCiv = self.szMinorCiv
				if bShowDeadTag:
					szDeadCiv = self.szDeadCiv

				scores = Scoreboard.Scoreboard(iPlayerAct, uFont)
				cCoGreen = self.COLOR_GREEN
				cCoRed	 = self.COLOR_RED
				MAX_PC_TEAMS = self.iMaxPcTeams
				i = MAX_PC_TEAMS - 1
				while (i > -1):
					iTeam = GAME.getRankTeam(i)
					CyTeam = GC.getTeam(iTeam)

					bSameTeam = iTeam == iTeamAct
					if bSameTeam:
						bHumanTeam = True
						bHaveMetTeam = True
					else:
						bHumanTeam = CyTeam.isHuman()
						bHaveMetTeam = CyTeamAct.isHasMet(iTeam)

					if bHaveMetTeam or bHumanTeam or bDebug:
						scores.addTeam(CyTeam, i)
						if bHumanTeam and not bHaveMetTeam:
							i -= 1
							continue
						bTeamAlive = CyTeam.isAlive()
						bTeamAliveDoOnce = True
						bTeamAtWarWithYou = CyTeam.isAtWarWith(iTeamAct)
						j = self.iMaxPcPlayers - 1
						while (j > -1):
							iPlayer = GAME.getRankPlayer(j)
							bSelf = iPlayer == iPlayerAct
							if bScoresMinimized and not bSelf:
								j -= 1
								continue
							CyPlayer = GC.getPlayer(iPlayer)
							if not CyPlayer.isEverAlive() or CyPlayer.isNPC():
								j -= 1
								continue
							iPlayerTeam = CyPlayer.getTeam()
							if iPlayerTeam != iTeam:
								j -= 1
								continue
							bPlayerAlive = CyPlayer.isAlive()
							if not (bPlayerAlive or bShowDeadCivs):
								j -= 1
								continue
							bMinorCiv = CyPlayer.isMinorCiv()
							if bMinorCiv and not bShowMinorCivs:
								j -= 1
								continue
							scores.addPlayer(iPlayer, j)
							if bMultiPlayer and not CyPlayer.isTurnActive():
								scores.setWaiting()

							if not bPlayerAlive and bShowDeadTag:
								scores.setScore(szDeadCiv)
							else:
								iScore = GAME.getPlayerScore(iPlayer)
								scores.setScore(str(iScore))
								if bShowScoreDelta:
									iGameTurn = iTurn
									if iPlayer >= iPlayerAct:
										iGameTurn -= 1
									if bScoreInclCurrTurn:
										iScoreDelta = iScore
									elif iGameTurn >= 0:
										iScoreDelta = CyPlayer.getScoreHistory(iGameTurn)
									else:
										iScoreDelta = 0
									iPrevGameTurn = iGameTurn - 1
									if iPrevGameTurn >= 0:
										iScoreDelta -= CyPlayer.getScoreHistory(iPrevGameTurn)
									if iScoreDelta:
										if iScoreDelta > 0:
											iColor = cCoGreen
										else:
											iColor = cCoRed
										szTxt = "%+d" % iScoreDelta
										if iColor > -1:
											szTxt = TRNSLTR.changeTextColor(szTxt, iColor)
										scores.setScoreDelta(szTxt)

							if bUsePlayerName:
								szTxt = CyPlayer.getName()
							else:
								szTxt = GC.getLeaderHeadInfo(CyPlayer.getLeaderType()).getDescription()
							if bShowBothNames:
								szTxt += "/" + CyPlayer.getCivilizationShortDescription(0)
							elif bShowBothNamesShort:
								szTxt += "/" + CyPlayer.getCivilizationDescription(0)
							elif bShowLeaderName:
								pass
							elif bShowCivName:
								szTxt = CyPlayer.getCivilizationShortDescription(0)
							else:
								szTxt = CyPlayer.getCivilizationDescription(0)

							iPlayerRed		= CyPlayer.getPlayerTextColorR()
							iPlayerGreen	= CyPlayer.getPlayerTextColorG()
							iPlayerBlue		= CyPlayer.getPlayerTextColorB()
							iSum = iPlayerRed + iPlayerGreen + iPlayerBlue
							if iSum < 360:
								iAddRed = iAddGreen = iAddBlue = iAdd = (360 - iSum)/3

								while True:

									if iPlayerRed + iAddRed > 255:
										iAdd = (iAddRed - (255 - iPlayerRed))/2
										if not iAdd:
											iAdd = 1
										if not iAddGreen or not iAddBlue:
											iAdd *= 2
											if iAddGreen:
												iAddGreen += iAdd
											elif iAddBlue:
												iAddBlue += iAdd
										else:
											iAddGreen += iAdd
											iAddBlue += iAdd
										iAddRed = 0
										iPlayerRed = 255

									elif iPlayerGreen + iAddGreen > 255:
										iAdd = (iAddGreen - (255 - iPlayerGreen))/2
										if not iAdd:
											iAdd = 1
										if not iAddRed or not iAddBlue:
											iAdd *= 2
											if iAddRed:
												iAddRed += iAdd
											elif iAddBlue:
												iAddBlue += iAdd
										else:
											iAddRed += iAdd
											iAddBlue += iAdd
										iAddGreen = 0
										iPlayerGreen = 255

									elif iPlayerBlue + iAddBlue > 255:
										iAdd = (iAddBlue - (255 - iPlayerBlue))/2
										if not iAdd:
											iAdd = 1
										if not iAddRed or not iAddGreen:
											iAdd *= 2
											if iAddRed:
												iAddRed += iAdd
											elif iAddGreen:
												iAddGreen += iAdd
										else:
											iAddRed += iAdd
											iAddGreen += iAdd
										iAddBlue = 0
										iPlayerBlue = 255
									else:
										break

								iPlayerRed += iAddRed
								iPlayerGreen += iAddGreen
								iPlayerBlue += iAddBlue

							if not CyIF.isFlashingPlayer(iPlayer) or CyIF.shouldFlash(iPlayer):
								if not bPlayerAlive and bGreyOutDeadCivs:
									szTxt = u"<color=%d,%d,%d>%s</color>" %(175, 175, 175, szTxt)
								else:
									szTxt = u"<color=%d,%d,%d>%s</color>" %(iPlayerRed, iPlayerGreen, iPlayerBlue, szTxt)

							if bMinorCiv:
								szTxt += " ("  + szMinorCiv  + ")"

							scores.setName(szTxt + " ")
							scores.setID(u"<color=%d,%d,%d>%d</color>" %(iPlayerRed, iPlayerGreen, iPlayerBlue, iPlayer))

							if bSelf:
								bHumanPlayer = True
							else:
								bHumanPlayer = CyPlayer.isHuman()
							if bPlayerAlive:
								scores.setAlive()
								# BUG: Rest of Dead Civs change is merely indentation by 1 level ...
								if bTeamAlive:
									if bTeamAliveDoOnce:
										bVassal = CyTeam.isAVassal()
										bTeamForcedPeaceYou	= CyTeamAct.isForcePeace(iTeam)
										bTeamOpenBorderToYou = CyTeam.isOpenBorders(iTeamAct)
										bTeamHasYourEmbassy = CyTeam.isHasEmbassy(iTeamAct)
										bTeamOpenBorder2U	= CyTeam.isOpenBorders(iTeamAct)
										bTeamDefPactWithYou = CyTeam.isDefensivePact(iTeamAct)
										iTeamSpyPointAgainstYou = CyTeam.getEspionagePointsAgainstTeam(iTeamAct)
										iYouSpyPointAgainstTeam = CyTeamAct.getEspionagePointsAgainstTeam(iTeam)
										bTeamAliveDoOnce = False
									if not bHaveMetTeam:
										scores.setNotMet()
									if bTeamAtWarWithYou:
										scores.setWar()
									elif bTeamForcedPeaceYou:
										scores.setPeace()
									elif bVassal:
										for iTeamX in xrange(MAX_PC_TEAMS):
											if CyTeam.isVassal(iTeamX) and CyTeamAct.isForcePeace(iTeamX):
												scores.setPeace()
												break
									if CyPlayer.canTradeNetworkWith(iPlayerAct) and not bSelf:
										scores.setTrade(iPlayerAct)
									if bTeamOpenBorderToYou:
										scores.setBorders()
									if bTeamHasYourEmbassy:
										scores.setEmbassy()
									if bTeamDefPactWithYou:
										scores.setPact()
									iStateReligion = CyPlayer.getStateReligion()
									if iStateReligion != -1:
										if CyPlayer.hasHolyCity(iStateReligion):
											szTxt = iconHolyCityList[iStateReligion]
										else:
											szTxt = iconReligionList[iStateReligion]
										scores.setReligion(szTxt)

								if not bSelf:
									bEspionageCanSeeResearch = False
									if iTeamSpyPointAgainstYou < iYouSpyPointAgainstTeam:
										scores.setEspionage()
									for iMissionLoop in xrange(GC.getNumEspionageMissionInfos()):
										if GC.getEspionageMissionInfo(iMissionLoop).isSeeResearch():
											bEspionageCanSeeResearch = CyPlayerAct.canDoEspionageMission(iMissionLoop, iPlayer, None, -1)
											break
									if bSameTeam or bEspionageCanSeeResearch or GC.getTeam(iPlayerTeam).isVassal(iTeamAct) or bDebug:
										iTech = CyPlayer.getCurrentResearch()
										if iTech != -1:
											techIconSB.append([iPlayer, iTech])
											scores.setResearch(iTech, CyPlayer.getResearchTurnsLeft(iTech, True))
									# Power Rating - if on, show according to espionage "see demographics" mission.
									if bShowPower and CyPlayerAct.canDoEspionageMission(iDemographicsMission, iPlayer, None, -1):
										iPower = CyPlayer.getPower()
										if iPower > 0: # avoid divide by zero
											fPowerRatio = iPlayerPower / float(iPower)
											if bPowerThemVsYou:
												if fPowerRatio > 0:
													fPowerRatio = 1.0 / fPowerRatio
												else:
													fPowerRatio = 99.0
											szTxt = TextUtil.floatToString(fPowerRatio, iPowerDecimals) + iconStrength
											if iPowerColorHigh >= 0 and fPowerRatio >= iPowerRatioHigh:
												szTxt = TRNSLTR.changeTextColor(szTxt, iPowerColorHigh)
											elif iPowerColorLow >= 0 and fPowerRatio <= iPowerRatioLow:
												szTxt = TRNSLTR.changeTextColor(szTxt, iPowerColorLow)
											elif iPowerColor >= 0:
												szTxt = TRNSLTR.changeTextColor(szTxt, iPowerColor)
											scores.setPower(szTxt)
									# AI Specific
									if not bHumanPlayer:
										bWillingToTalk = CyPlayer.AI_isWillingToTalk(iPlayerAct)
										if bShowAttitude:
											scores.setAttitude(unichr(GAME.getSymbolID(FontSymbols.ANGRY_CIV_CHAR) + CyPlayer.AI_getAttitude(iPlayerAct)))
										if not bWillingToTalk:
											scores.setWontTalk()
										if bShowWorstEnemy and CyPlayer.getWorstEnemyName() == CyTeamAct.getName():
											scores.setWorstEnemy()
										if bShowWHEOOH and not bTeamAtWarWithYou and bWillingToTalk and bHaveMetTeam:
											TD = TradeData()
											TD.ItemType = TradeableItems.TRADE_WAR
											for iTeamX in xrange(MAX_PC_TEAMS):
												if iTeamX in (iTeamAct, iTeam): continue
												if CyTeam.isAtWarWith(iTeamX) or not CyTeamAct.isHasMet(iTeamX) or not CyTeam.isHasMet(iTeamX):
													continue
												TD.iData = iTeamX
												if CyPlayer.canTradeItem(iPlayerAct, TD, False):
													denial = CyPlayer.getTradeDenial(iPlayerAct, TD)
													if denial == DenialTypes.DENIAL_TOO_MANY_WARS:
														scores.setWHEOOH()
														break
									else:
										bWillingToTalk = True

								if bShowCityCount:
									if bSameTeam or bTeamOpenBorder2U or self.bDebugMode or (bWillingToTalk and bTeamHasYourEmbassy):
										szTxt = str(CyPlayer.getNumCities())
									else:
										iCount = 0
										for CyCity in CyPlayer.cities():
											if CyCity.isRevealed(iTeamAct, False):
												iCount += 1
										# (capital==None and iCount > 0) wouldn't think it possible, but it happened...
										capital = CyPlayer.getCapitalCity()
										if iCount and capital and not capital.isRevealed(iTeamAct, False):
											iCount += 1
										szTxt = u"<color=0,255,255>%d" %iCount
									scores.setNumCities(szTxt)
							if bNetworkMP:
								scores.setNetStats(CyGTM.getNetStats(iPlayer))
							if bHumanPlayer and bOutOfSync:
								scores.setNetStats(u" <color=255,0,0>* %s *" %(CyGTM.getOOSSeeds(iPlayer)))
							j -= 1
					i -= 1
				self.techIconSB = techIconSB
				self.xScoreboard = scores.draw(screen, self.xRes, self.yRes)
				bReset_xScoreboard = False

		if bReset_xScoreboard:
			self.xScoreboard = self.xRes
		if iCheck != self.xScoreboard:
			self.cleanPlotList(screen)
			CyIF.setDirty(InterfaceDirtyBits.PlotListButtons_DIRTY_BIT, True)


	# Will build the globeview UI
	def updateGlobeviewButtons(self, screen):
		print "updateGlobeviewButtons"
		xRes = self.xRes
		yRes = self.yRes
		yBotBar = self.yBotBar
		IFT = self.iInterfaceType
		bGlobeview = self.bGlobeview

		aMinimapBtnList = self.aMinimapBtnList
		aGlobeLayerList = self.aGlobeLayerList

		# Move Globe Button
		if self.bMinInterface:
			iBtnX = xBtnStart = xRes - 36
			if IFT == InterfaceVisibility.INTERFACE_SHOW:
				screen.moveItem("GlobeToggle", iBtnX, yBotBar - 2, 0.0)
				iBtnY = yBotBar + 6
				for szButton in aGlobeLayerList:
					iBtnX -= 28
					screen.moveItem(szButton, iBtnX, iBtnY, 0.0)
				iBtnX = xBtnStart
				for szButton in aMinimapBtnList:
					iBtnX -= 28
					screen.moveItem(szButton, iBtnX, iBtnY, 0.0)
				self.bMinInterface = False
		elif IFT == InterfaceVisibility.INTERFACE_HIDE:
			iBtnX = xBtnStart = xRes - 36
			screen.moveItem("GlobeToggle", iBtnX, yRes - 36, 0.0)
			y = yRes - 28
			for szButton in aGlobeLayerList:
				iBtnX -= 28
				screen.moveItem(szButton, iBtnX, y, 0.0)
			iBtnX = xBtnStart
			for szButton in aMinimapBtnList:
				iBtnX -= 28
				screen.moveItem(szButton, iBtnX, y, 0.0)
			self.bMinInterface = True

		bVisible = IFT not in (InterfaceVisibility.INTERFACE_HIDE_ALL, InterfaceVisibility.INTERFACE_MINIMAP_ONLY)
		if self.bCityScreen:
			bVisible = False

		if bVisible:
			if bGlobeview:
				kHide = aMinimapBtnList
				kShow = aGlobeLayerList
			else:
				kHide = aGlobeLayerList
				kShow = aMinimapBtnList
			screen.show("GlobeToggle")
		else:
			kHide = aMinimapBtnList + aGlobeLayerList
			kShow = []
			screen.hide("GlobeToggle")

		for szButton in kHide:
			screen.hide(szButton)
		for szButton in kShow:
			screen.show(szButton)

		# Set base Y position for the LayerOptions, if we find them
		if IFT == InterfaceVisibility.INTERFACE_HIDE:
			iY = yRes - 38
		else:
			iY = yBotBar - 2

		# Hide the layer options ... all of them
		for i in xrange(30):
			szName = "GlobeLayerOption" + str(i)
			screen.hide(szName)

		# Setup the GlobeLayer panel
		iCurrentLayerID = CyGlobeLayerManager().getCurrentLayerID()
		CyGlobeLayer = CyGlobeLayerManager().getLayer(iCurrentLayerID)
		if bGlobeview and IFT != InterfaceVisibility.INTERFACE_HIDE_ALL:
			iNumOptions = CyGlobeLayer.getNumOptions()
			# set up toggle button
			screen.setState("GlobeToggle", True)
			# Set GlobeLayer indicators correctly
			for szButtonID in aGlobeLayerList:
				screen.setState(szButtonID, iCurrentLayerID == i)
			# Set up options pane
			if iCurrentLayerID != -1 and iNumOptions != 0:
				iCurY = iY
				iCurOption = CyGlobeLayer.getCurrentOption()
				iMaxTextWidth = -1
				for iTmp in xrange(iNumOptions):
					iOption = iTmp # iNumOptions - iTmp - 1
					szName = "GlobeLayerOption" + str(iOption)
					szCaption = CyGlobeLayer.getOptionName(iOption)
					if iOption == iCurOption:
						szBuffer = "  <color=0,255,0>%s</color>  " % (szCaption)
					else:
						szBuffer = "  %s  " % (szCaption)
					iTextWidth = CyIF.determineWidth(szBuffer)

					screen.setText(szName, "", szBuffer, 1<<0, xRes - 9 - iTextWidth, iCurY - 34, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GLOBELAYER_OPTION, iOption, -1)
					screen.show(szName)

					iCurY -= 24

					if iTextWidth > iMaxTextWidth:
						iMaxTextWidth = iTextWidth

				#make extra space
				iCurY -= 24
				iPanelWidth = iMaxTextWidth + 32
				iPanelHeight = iY - iCurY
				iPanelX = xRes - 14 - iPanelWidth
				iPanelY = iCurY
				screen.setPanelSize("ScoreBackground", iPanelX, iPanelY, iPanelWidth, iPanelHeight)
				screen.show("ScoreBackground")
			else:
				screen.hide("ScoreBackground")
				self.xScoreboard = self.xRes
				self.cleanPlotList(screen)
				CyIF.setDirty(InterfaceDirtyBits.PlotListButtons_DIRTY_BIT, True)

		else:
			if iCurrentLayerID != -1:
				szName = CyGlobeLayer.getName()
				if szName == "RESOURCES":
					screen.setState("ResourceIcons", True)
				else:
					screen.setState("ResourceIcons", False)

				if szName == "UNITS":
					screen.setState("UnitIcons", True)
				else:
					screen.setState("UnitIcons", False)
			else:
				screen.setState("ResourceIcons", False)
				screen.setState("UnitIcons", False)

			screen.setState("Grid", CyUserProfile().getGrid())
			screen.setState("MMB|BareMap0", CyUserProfile().getMap())
			screen.setState("Yields", CyUserProfile().getYields())
			screen.setState("MMB|ScoreToggle0", CyUserProfile().getScores())

			screen.hide("InterfaceGlobeLayerPanel")
			screen.setState("GlobeToggle", False)


	# Will update the percent buttons
	def updatePercentButtons(self, screen):
		print "updatePercentButtons"
		# Hide it all
		for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
			screen.hide("ComPercent|Inc%d" % i)
			screen.hide("ComPercent|Dec%d" % i)
		# Evaluate display
		bCityScreen = self.bCityScreen
		if not bCityScreen or (self.InCity.iPlayer == self.iPlayer) or self.bDebugMode:
			IFT = self.iInterfaceType
			if IFT in (InterfaceVisibility.INTERFACE_SHOW, InterfaceVisibility.INTERFACE_HIDE):
				eWidGen = WidgetTypes.WIDGET_GENERAL
				eSubBtn = ButtonStyles.BUTTON_STYLE_CITY_MINUS
				eAddBtn = ButtonStyles.BUTTON_STYLE_CITY_PLUS
				CyPlayer = self.CyPlayer
				x1 = 68
				iSize = 20
				x2 = x1 + iSize
				y = 52
				iRange = CommerceTypes.NUM_COMMERCE_TYPES
				for i in xrange(iRange):
					iType = (i + 1) % iRange # 1, 2, 3, 0 (Research, Culture, Espionage, Gold)

					if CyPlayer.isCommerceFlexible(iType):
						szBut = "ComPercent|Inc" + str(iType)
						screen.setButtonGFC(szBut, "", "", x1, y, iSize, iSize, eWidGen, 1, 1, eAddBtn)
						szBut = "ComPercent|Dec" + str(iType)
						screen.setButtonGFC(szBut, "", "", x2, y, iSize, iSize, eWidGen, 1, 1, eSubBtn)
						y += iSize - 2
					elif bCityScreen:
						y += iSize - 2

	def buildCitizenPanel(self, screen, CyCity):

		eWidGen = WidgetTypes.WIDGET_GENERAL
		iWidFreeCitizen		= WidgetTypes.WIDGET_FREE_CITIZEN
		eFontGame	= FontTypes.GAME_FONT
		uFont = self.aFontList[4]

		iSize = self.iSpecialist_IconSize
		xRes = self.xRes
		x = xRes - xRes/4 + 8
		y = self.yCityStackBarTop - iSize - 10
		panel = "CitizenPanel"
		screen.addScrollPanel(panel, "", x, y, xRes - x - 16, iSize, PanelStyles.PANEL_STYLE_STANDARD)
		screen.setStyle(panel, "ScrollPanel_Min_Style")

		iAngryPop = CyCity.angryPopulation(0)
		x = 0
		if iAngryPop:

			szCount = str(iAngryPop)
			screen.setLabelAt("AngryCitizenLabel", panel, uFont + szCount, 1<<0, x, 0, 0, eFontGame, eWidGen, 0, 0)
			x += 6 * len(szCount)
			ID = "AngryCitizen"
			image = '<img=%s size=%d></img>' %(self.artPathAngryCitizen, iSize)
			screen.setTextAt(ID, panel, image, 1<<0, 0, 0, 0, eFontGame, WidgetTypes.WIDGET_ANGRY_CITIZEN, 0, 1)
			x += iSize + 4

		aSpecialistIconList = self.aSpecialistIconList
		for i in xrange(self.iNumSpecialistInfos):
			szInc = str(i)
			# Static Citizens
			FreeSpecialistCount = CyCity.getFreeSpecialistCount(i)
			if FreeSpecialistCount:

				ID = panel + "Label" + szInc
				szCount = str(FreeSpecialistCount)
				if x:
					szCount = ", " + szCount
				screen.setLabelAt(ID, panel, uFont + szCount, 1<<0, x, 0, 0, eFontGame, eWidGen, 0, 0)
				x += 6 * len(szCount)

				ID = panel + szInc
				image = '<img=%s size=%d></img>' %(aSpecialistIconList[i], iSize)
				screen.setTextAt(ID, panel, image, 1<<0, x, 0, 0, eFontGame, iWidFreeCitizen, i, 0)
				x += iSize + 4

	# Hide Citizen info
	def CS_Citizens_hide(self, screen):
		for i in xrange(self.iNumSpecialistInfos):
			ID = str(i)
			screen.hide("IncreaseSpecialist" + ID)
			screen.hide("DecreaseSpecialist" + ID)
			screen.hide("CitizenDisabledButton" + ID)
			screen.hide("CitizenButton" + ID)
			screen.hide("CitizenCount" + ID)

	# Will update the citizen buttons
	def updateCitizenButtons(self, screen):
		print "updateCitizenButtons"
		self.CS_Citizens_hide(screen)

		uFont = self.aFontList[4]

		InCity = self.InCity
		CyCity = InCity.CyCity

		if self.bDebugMode:
			bOwnCity = True
		else:
			bOwnCity = InCity.iPlayer == self.iPlayer

		bAutomated = CyCity.isCitizensAutomated()
		iMinCount = 0

		for i in xrange(self.iNumSpecialistInfos):
			szInc = str(i)
			# Dynamic Citizens
			if GC.getSpecialistInfo(i).isVisible():

				iCount = CyCity.getSpecialistCount(i)
				if bOwnCity:
					if bAutomated:
						iMinCount = iCount - CyCity.getForceSpecialistCount(i)

					if CyCity.isSpecialistValid(i, 1):
						screen.show("IncreaseSpecialist" + szInc)

					if iCount > iMinCount:
						screen.show("DecreaseSpecialist" + szInc)

				ID = "CitizenCount" + szInc
				screen.modifyLabel(ID, uFont + str(iCount), 1<<1)
				screen.show(ID)

				if bOwnCity and CyCity.isSpecialistValid(i, 0):
					ID = "CitizenButton" + szInc
					screen.show(ID)
					screen.setState(ID, CyCity.AI_isEmphasizeSpecialist(i))
				else:
					ID = "CitizenDisabledButton" + szInc
					screen.show(ID)
					screen.setState(ID, CyCity.AI_isEmphasizeSpecialist(i))

	# FoV
	def setFieldofView(self, iFoV):
		if self.iField_View_Prev != iFoV:
			GC.setNoUpdateDefineFLOAT("FIELD_OF_VIEW", iFoV)
			self.iField_View_Prev = iFoV

	def setFieldofView_Text(self, screen, iFoV):
		szFOV = self.aFontList[6] + "FoV: %i" %(iFoV)
		screen.setLabel("FoVSliderText", "", szFOV, 1<<1, self.iX_FoVSlider - 4, self.iY_FoVSlider + 6, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, 0, 0)


	#######################
	# Help Text Functions #
	#######################
	# Plot help
	def updatePlotHelp(self, screen, uFont=None):
		POINT = Win32.getCursorPos()

		if self.bLockPlotHelp:

			plot = CyIF.getMouseOverPlot()
			if plot:
				unit = CyIF.getInterfacePlotUnit(plot, 0)
				if unit and unit.getID() == self.plotHelpUnitID:
					return
			self.bLockPlotHelp = False

		if POINT.x < 40 or POINT.x > self.xRes - 40 or POINT.y > self.yBotBar or POINT.y < 60:
			screen.hide("PlotHelp")
			return

		szPlotHelp = CyIF.getHelpString()
		if not szPlotHelp:
			screen.hide("PlotHelp")
			return

		if szPlotHelp == self.szPlotHelp:
			screen.moveToFront("PlotHelp")
			screen.show("PlotHelp")
			return
		self.szPlotHelp = szPlotHelp
		if szPlotHelp[:1] == "\n":
			szPlotHelp = szPlotHelp[1:]
		if not uFont:
			uFont=self.aFontList[5]
		pyTT.makeTooltip(screen, -12, self.yBotBar + 8, szPlotHelp, uFont, "PlotHelp")

		# if shift pressed for plot help with unit on.
		plot = CyIF.getMouseOverPlot()
		if plot and self.InputData.getModifierKeys()[2]:
			unit = CyIF.getInterfacePlotUnit(plot, 0)
			if unit:
				self.bLockPlotHelp = True
				self.plotHelpUnitID = unit.getID()


	# Tooltip
	def hideTooltip(self, screen):
		self.bTooltip = False
		self.bLockedTT = False
		self.dataTT = []
		screen.hide("Tooltip")

	def updateTooltip(self, screen, szHelpText, xPos=-1, yPos=-1, uFont=""):
		if not szHelpText or self.iInterfaceType == InterfaceVisibility.INTERFACE_HIDE_ALL:
			return
		if szHelpText != self.szHelpText:
			if not uFont:
				uFont=self.aFontList[5]
			self.szHelpText = szHelpText
			iX, iY = pyTT.makeTooltip(screen, xPos, yPos, szHelpText, uFont, "Tooltip")
			POINT = Win32.getCursorPos()
			self.iOffsetTT = [iX - POINT.x, iY - POINT.y]
			self.xMouseTT = POINT.x
			self.yMouseTT = POINT.y
			if xPos == yPos == -1:
				self.bLockedTT = True
		else:
			if xPos == yPos == -1:
				POINT = Win32.getCursorPos()
				self.xMouseTT = POINT.x
				self.yMouseTT = POINT.y
				xOff, yOff = self.iOffsetTT
				iX = POINT.x + xOff
				if iX < 0:
					iX = 0
				iY = POINT.y + yOff
				if iY < 0:
					iY = 0
				screen.moveItem("Tooltip", iX, iY, 0)
				self.bLockedTT = True
			screen.moveToFront("Tooltip")
			screen.show("Tooltip")
		self.bTooltip = True
		screen.hide("PlotHelp")

	def helpGreatPersonBar(self, screen):
		CyCity, iTurns = GPUtil.getDisplayCity()
		if not CyCity:
			# no rate or progress in any city and no city selected
			szTxt = TRNSLTR.getText("TXT_KEY_MISC_GREAT_PERSON", (0, self.CyPlayer.greatPeopleThresholdNonMilitary()))
			self.updateTooltip(screen, szTxt)
			return
		iThreshold = GC.getPlayer(CyCity.getOwner()).greatPeopleThresholdNonMilitary()
		iProgress = CyCity.getGreatPeopleProgress()
		iRate = CyCity.getGreatPeopleRate()
		szTxt = TRNSLTR.changeTextColor(CyCity.getName(), GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")) + "\n"
		szTxt += TRNSLTR.getText("TXT_KEY_MISC_GREAT_PERSON", (iProgress, iThreshold))
		if iRate:
			szTxt += "\n" + str(iRate) + self.iconGreatPeople
			szTxt += TRNSLTR.getText("TXT_KEY_PER_TURN", ()) + " " + TRNSLTR.getText("INTERFACE_CITY_TURNS", (iTurns,))
		aPercentList = GPUtil.calcPercentages(CyCity)
		if aPercentList:
			aPercentList.sort()
			aPercentList.reverse()
			szTxt += "\n"
			for iPercent, iUnit in aPercentList:
				szTxt += u"\n%s%s - %d%%" % (GPUtil.getUnitIcon(iUnit), GC.getUnitInfo(iUnit).getDescription(), iPercent)
		self.updateTooltip(screen, szTxt)


	def treasuryHelp(self, screen, szTxt):
		player = self.CyPlayer
		iconCommerceGold = self.iconCommerceList[0]
		szTxt += "\n%s: %d" % (iconCommerceGold, player.getGold())
		if player.isAnarchy():
			self.updateTooltip(screen, szTxt)
			return

		# Treasury Upkeep
		iValue = player.getTreasuryUpkeep()
		if iValue:
			szTxt += "\n" + TRNSLTR.getText("TXT_KEY_TREASURY_UPKEEP", ()) + str(iValue) + iconCommerceGold

		# Civics
		iSum = 0
		szTemp = ""
		for i in xrange(GC.getNumCivicOptionInfos()):
			iCivic = player.getCivics(i)
			iValue = player.getSingleCivicUpkeep(iCivic, True)
			if iValue:
				szTemp += "\n\t" + str(iValue) + iconCommerceGold + " "+ TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_FROM", ())+ " " + GC.getCivicInfo(iCivic).getDescription()
				iSum += iValue
		if iSum:
			szTxt += "\n" + TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_CIVIC_UPKEEP", ()) +" " + str(iSum) + iconCommerceGold + szTemp

		# Maintenance
		iValue = player.getTotalMaintenance()
		if iValue:
			szTxt += "\n" + TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_CITY_MAINTENANCE", ()) + " " + str(iValue) + iconCommerceGold

		iValue = player.getCorporateMaintenance()
		if iValue:
			szTxt += "\n" + TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_CORPORATE_MAINTENANCE", ()) + " " + str(iValue) + iconCommerceGold

		# Unit upkeep
		iUnitUpkeep = player.getFinalUnitUpkeep()
		iUnitSupply = player.calculateUnitSupply()
		if iUnitUpkeep or iUnitSupply:
			szTxt += "\n"
			if iUnitUpkeep:
				szTxt += TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_UNIT_UPKEEP", ()) + " " + str(iUnitUpkeep) + iconCommerceGold
				if iUnitSupply:
					szTxt += "\n	" + str(iUnitSupply) + iconCommerceGold + " " + TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_EXPEDITIONARY", ())
			elif iUnitSupply:
				szTxt += TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_UNIT_SUPPLY", ()) + " " + str(iUnitSupply) + iconCommerceGold

		# Trade
		iValue = player.getGoldPerTurn()
		if iValue:
			szTxt += "\n" + TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_TRADE", ()) +" <color="
			if iValue < 0:
				szTxt += "255,0,0>"
			else:
				szTxt += "0,255,0>"
			szTxt += str(iValue) + "</color>" + iconCommerceGold
		self.updateTooltip(screen, szTxt)


	def showRevStatusInfoPane(self, screen):
		InCity = self.InCity
		iPlayer = InCity.iPlayer
		# create text
		headerString = "=========================="
		if iPlayer == self.iPlayer:
			headerString = self.szHeaderRevStatus
		temp1 = RevInstances.RevolutionInst.updateLocalRevIndices(1, iPlayer, [InCity.CyCity], True)
		temp1 = temp1.strip('\n')
		temp1 = temp1.replace('\t',' ')

		# Display national effects in the rev bar mouseover
		turn = GAME.getGameTurn()
		temp2 = RevInstances.RevolutionInst.updateCivStability(turn, iPlayer, True, True)
		temp2 = temp2.strip('\n')
		temp2 = temp2.replace('\t', ' ')

		szTxt = headerString + '\n' + temp1 + '\n\n' + temp2
		self.updateTooltip(screen, szTxt)

	############################
	# Core Interface Functions #
	############################
	def update(self, fDelta): return
	# Will handle the input for this screen...
	def handleInput(self, inputClass):
		HandleInputUtil.debugInput(inputClass)

		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)

		if self.cityOptions:
			return self.cityOptions.handleInput(screen, inputClass, self)

		bAlt, bCtrl, bShift = self.InputData.getModifierKeys()
		iCode	= inputClass.eNotifyCode
		iData	= inputClass.iData
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName
		iData1	= inputClass.iData1
		szFlag	= HandleInputUtil.MOUSE_FLAGS.get(inputClass.uiFlags, "UNKNOWN")


		if iCode == 16: # Key Down

			# N47 - Unselect on ESC (Toffer - Implemented code suggested by N47 with tweaks)

			if iData == 1 and not CyIF.isFocused(): # InputTypes.KB_ESCAPE

				if self.InCity:
					CyIF.clearSelectedCities()
					return 1
				elif self.AtUnit and (self.AtUnit.CyUnit.isWaiting() or not self.AtUnit.CyUnit.canMove()):
					CyIF.clearSelectionList()
					return 1

				return 0


			if iData in (2, 3, 4, 5, 6, 7, 8, 9, 10, 11): # 0-9
				if self.InCity and not bCtrl:
					self.bBuildWorkQueue = True

			elif iData in (13, 16, 29, 31): # A D Q S
				if self.InCity:
					if bCtrl and iData == 16 and self.InCity.iPlayer == self.iPlayer:
						GAME.selectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_CONSCRIPT, -1, -1, False, bAlt, bShift, bCtrl)
						return 1

					elif not (bAlt or bCtrl or bShift):
						iTab = {13:CITYTAB_UNIT, 16:CITYTAB_WONDER, 29:CITYTAB_ADMIN, 31:CITYTAB_BUILDING}[iData]

						if iTab == self.iCityTab:
							self.exitCityTab(screen)
						else:
							self.openCityTab(screen, iTab)
						return 1

			elif iData == 14:
				if bCtrl:
					CyIF.toggleBareMapMode()
					screen.setState("MMB|BareMap0", CyUserProfile().getMap())

			elif iData in (45, 49, 56): # Ctrl, Shift, Alt
				dataTT = self.dataTT
				if dataTT:
					if bCtrl != dataTT[0] or bShift != dataTT[1] or bAlt != dataTT[2]:
						if dataTT[3]:
							szTxt = CyGTM.getSpecificUnitHelp(dataTT[4], False, False)
							self.updateTooltip(screen, szTxt, self.xRes / 4, self.yPlotListTT)
						else:
							szTxt = CyGTM.getUnitHelp(dataTT[4], False, True, True, dataTT[5])
							self.updateTooltip(screen, szTxt)
						dataTT[0] = bCtrl
						dataTT[1] = bShift
						dataTT[2] = bAlt
					return 1
			return 0

		if iCode == 17: # Key Up
			if iData in (45, 49, 56): # Ctrl, Shift, Alt
				dataTT = self.dataTT
				if dataTT:
					dataTT[0] = bCtrl
					dataTT[1] = bShift
					dataTT[2] = bAlt
				if iData == 49: self.bLockPlotHelp = False
			return 0

		szSplit = NAME.split("|")
		BASE = szSplit[0]
		if szSplit[1:]:
			TYPE = szSplit[1]
		else:
			TYPE = ""
		if szSplit[2:]:
			CASE = szSplit[2:]
		else:
			CASE = [""]

		self.hideTooltip(screen)
		self.bPlotHelpBan = False

		if BASE == "CityDemolish":
			if ACEM.CD:
				ACEM.CD.handleInput(screen, szSplit[1:], iCode, szFlag, ID, iData1)
			return

		if CASE[0] == "QueueEntry":
			InCity = self.InCity
			WHAT = BASE + "|" + TYPE + "|"
			szRow = str(ID)
			aTuple = InCity.WorkQueue
			for i, entry in enumerate(aTuple):
				if entry[0] == WHAT and entry[2] == szRow:
					iNode = i
					iType = entry[1]
					break
			else:
				print "\t--ERROR--\n\t\t\tCity Work Queue is corrupt"
				return
		else:
			iType = ID

		if iCode == 4: # Mouse Enter

			if BASE == "WID":

				if CASE[0] in ("CityWork", "QueueEntry", "BldgList"):
					CyCity = self.InCity.CyCity
					if TYPE == "UNIT":
						self.dataTT = [bCtrl, bShift, bAlt, "", iType, CyCity]
						szTxt = CyGTM.getUnitHelp(iType, False, True, True, CyCity)
						self.updateTooltip(screen, szTxt)
					elif TYPE == "BUILDING":
						szTxt = CyGTM.getBuildingHelp(iType, True, CyCity, False, False, True)
						self.updateTooltip(screen, szTxt)
					elif TYPE == "PROJECT":
						szTxt = CyGTM.getProjectHelp(iType, False, CyCity)
						self.updateTooltip(screen, szTxt)
					elif TYPE == "PROCESS":
						y = self.yBotBar + 12
						CvProcessInfo = GC.getProcessInfo(iType)
						eYieldProd = YieldTypes.YIELD_PRODUCTION
						fProd = CyCity.getYieldRate(eYieldProd) / 100.0
						szTxt = CvProcessInfo.getDescription()
						for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
							iValue = CvProcessInfo.getProductionToCommerceModifier(i)
							if not iValue: continue
							szTxt += "\n" + TRNSLTR.getText("TXT_KEY_PROCESS_CONVERTS", (iValue, GC.getYieldInfo(eYieldProd).getChar(), GC.getCommerceInfo(i).getChar()),)
							szTxt += r" -> " + "%.1f" %(fProd * iValue)
							if szTxt[-1] == "0":
								szTxt = szTxt[:-2]
							szTxt += self.iconCommerceList[i]
						self.updateTooltip(screen, szTxt, self.xPopProgBar, y)

				elif TYPE == "UNIT":
					self.dataTT = [bCtrl, bShift, bAlt, "", iType, self.InCity.CyCity]
					self.updateTooltip(screen, CyGTM.getUnitHelp(iType, False, True, True, self.InCity.CyCity))

				elif TYPE == "BUILDING":
					self.updateTooltip(screen, CyGTM.getBuildingHelp(iType, True, self.InCity.CyCity, False, False, True))

				elif TYPE == "PROJECT":
					self.updateTooltip(screen, CyGTM.getProjectHelp(iType, False, self.InCity.CyCity))

				elif TYPE == "PROMO":
					szTxt = ""
					if CASE[0] == "PromoPanel":
						iCount = self.aUnitPromoCountMap[iType]
						if iCount > 1:
							szTxt = "<color=144,255,72>" + TRNSLTR.getText("TXT_KEY_IN_STACK", (iCount,)) + "</color>\n"
					print (NAME, iType, ID)
					szTxt += CyGTM.getPromotionHelp(iType, False)
					self.updateTooltip(screen, szTxt)

				elif TYPE == "TECH":
					szTxt = ""
					if CASE[0] == "Selection":
						szTxt += TRNSLTR.getText("TXT_INTERFACE_TECH_HELP_RESEARCH", ()) + " "
					elif CASE[0] == "ProgBar":
						szTxt += TRNSLTR.getText("TXT_INTERFACE_TECH_HELP_RESEARCHING", ()) + " "
						iType = GC.getPlayer(self.iPlayer).getCurrentResearch()
					elif CASE[0] == "Score":
						szTxt += TRNSLTR.getText("TXT_INTERFACE_TECH_HELP_RESEARCHING", ()) + " "
					szTxt += CyGTM.getTechHelp(iType, False, True, True, True, -1)
					self.updateTooltip(screen, szTxt)

				elif TYPE == "LIST":
					CyPlayer = self.InCity.CyPlayer
					idx = CyIF.getOrderNodeData1(iType)
					iList = CyPlayer.getBLIndexByID(idx)
					if iList >= 0:
						szTxt = CyPlayer.getBLListName(iList)
					else:
						szTxt = "ERROR"
					self.updateTooltip(screen, szTxt)

			elif BASE == "PlotList":
				if TYPE in ("Button", "Health"):
					CyUnit = self.aPlotListList[ID][0]
					if TYPE == "Button":
						szTxt = CyGTM.getSpecificUnitHelp(CyUnit, False, False)
						x = self.xRes / 4
						y = self.yPlotListTT
						self.dataTT = [bCtrl, bShift, bAlt, "spcfc", CyUnit]
					elif TYPE == "Health":
						szTxt = "HP: %d/%d" %(CyUnit.getHP(), CyUnit.getMaxHP())
						x = -1
						y = -1
					else: return
					self.updateTooltip(screen, szTxt, x, y)

			elif BASE == "BldgList":
				if TYPE == "Demolish":
					self.updateTooltip(screen, TRNSLTR.getText("TXT_KEY_CITY_SCREEN_DEMOLISH", ()))
				elif TYPE == "BUILDING":
					szTxt = CyGTM.getBuildingHelp(ID, True, CyCity, False, False, True)
					self.updateTooltip(screen, szTxt)
				else:
					aList = [TRNSLTR.getText("TXT_KEY_WB_BUILDINGS", ()), TRNSLTR.getText("TXT_KEY_CONCEPT_WONDERS", ()), TRNSLTR.getText("TXT_KEY_CITY_SCREEN_CONCEPTUAL", ())]
					self.updateTooltip(screen, aList[ID])

			elif BASE == "BonusList":
				aList = [TRNSLTR.getText("TXT_KEY_PROPERTIES_PROPERTIES", ()), TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_BONUS_MAP", ()), TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_BONUS_MANDFACTURED", ()), TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_C2C_CULTURES", ())]
				self.updateTooltip(screen, aList[ID])

			elif BASE == "CityTab":
				if TYPE == "BG":
					szTxt = TRNSLTR.getText("INTERFACE_CLOSE_TAB", ())
					x = y = -1
				else:
					x = self.xPopProgBar
					y = self.yBotBar + 12
					if not ID:
						szTxt = TRNSLTR.getText("INTERFACE_TAB_ADMIN", ())
						szKey = "Q"
					elif ID == 1:
						szTxt = TRNSLTR.getText("INTERFACE_TAB_UNIT", ())
						szKey = "A"
					elif ID == 2:
						szTxt = TRNSLTR.getText("INTERFACE_TAB_BUILDING", ())
						szKey = "S"
					else:
						szTxt = TRNSLTR.getText("INTERFACE_TAB_WONDER", ())
						szKey = "D"

					szTxt += " <color=144,255,72>&#60" + szKey + "&#62"
				self.updateTooltip(screen, szTxt, x, y)

			elif BASE == "CT":
				# City Tab
				if TYPE == "UnitFilter":
					aMap = {
						UnitFilterTypes.UNIT_FILTER_HIDE_UNBUILDABLE: "SHOW_BUILDABLE",
						UnitFilterTypes.UNIT_FILTER_HIDE_LIMITED	: "HIDE_LIMITED",
						UnitFilterTypes.UNIT_FILTER_SHOW_LAND		: "SHOW_LAND",
						UnitFilterTypes.UNIT_FILTER_SHOW_AIR		: "SHOW_AIR",
						UnitFilterTypes.UNIT_FILTER_SHOW_WATER		: "SHOW_WATER",
						UnitFilterTypes.UNIT_FILTER_SHOW_WORKERS	: "SHOW_WORKERS",
						UnitFilterTypes.UNIT_FILTER_SHOW_CIVILIAN	: "SHOW_CIVILIAN",
						UnitFilterTypes.UNIT_FILTER_SHOW_SIEGE		: "SHOW_SIEGE",
						UnitFilterTypes.UNIT_FILTER_SHOW_MOUNTED	: "SHOW_MOUNTED",
						UnitFilterTypes.UNIT_FILTER_SHOW_HEROES		: "SHOW_HEROES",
						UnitFilterTypes.UNIT_FILTER_SHOW_MILITARY	: "SHOW_MILITARY",
						UnitFilterTypes.UNIT_FILTER_SHOW_DEFENSE	: "SHOW_DEFENSE",
						UnitFilterTypes.UNIT_FILTER_SHOW_MISSIONARY	: "SHOW_MISSIONARY"
					}
					if ID in aMap:
						self.updateTooltip(screen, TRNSLTR.getText("TXT_KEY_LIST_UNIT_FILTER_" + aMap[ID], ()))

				elif TYPE == "BuildingFilter":
					aMap = {
						BuildingFilterTypes.BUILDING_FILTER_HIDE_UNBUILDABLE	: "SHOW_BUILDABLE",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_SCIENCE		: "SHOW_SCIENCE",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_ESPIONAGE		: "SHOW_ESPIONAGE",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_CULTURE		: "SHOW_CULTURE",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_GOLD			: "SHOW_GOLD",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_FOOD			: "SHOW_FOOD",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_PRODUCTION		: "SHOW_PRODUCTION",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_HAPPINESS		: "SHOW_HAPPINESS",
						BuildingFilterTypes.BUILDING_FILTER_HIDE_UNHAPPINESS	: "HIDE_UNHAPPINESS",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_HEALTH			: "SHOW_HEALTH",
						BuildingFilterTypes.BUILDING_FILTER_HIDE_UNHEALTHINESS	: "HIDE_UNHEALTHINESS",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_MILITARY		: "SHOW_MILITARY",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_CITY_DEFENSE	: "SHOW_CITY_DEFENSE",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_CRIME			: "SHOW_CRIME",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_DISEASE		: "SHOW_DISEASE",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_EDUCATION		: "SHOW_EDUCATION",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_FLAMMABILITY	: "SHOW_FLAMMABILITY",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_WATER_POLLUTION: "SHOW_WATER_POLLUTION",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_AIR_POLLUTION	: "SHOW_AIR_POLLUTION",
						BuildingFilterTypes.BUILDING_FILTER_SHOW_TOURISM		: "SHOW_TOURISM"
					}
					if ID in aMap:
						self.updateTooltip(screen, TRNSLTR.getText("TXT_KEY_LIST_BUILDING_FILTER_" + aMap[ID], ()))

			elif BASE == "CS":
				# City Screen
				if TYPE == "RawYields":
					self.updateTooltip(screen, u"%s" % TRNSLTR.getText(self.RAW_YIELD_HELP[ID], ()))

				elif TYPE == "RevBar":
					self.showRevStatusInfoPane(screen)

				elif TYPE == "Happiness":
					self.updateTooltip(screen, CyGTM.getHappinessHelp())

				elif TYPE == "ProdYield":
					self.updateTooltip(screen, CyGTM.getProductionHelpCity(self.InCity.CyCity))

				elif TYPE == "Defense":
					self.updateTooltip(screen, CyGTM.getDefenseHelp(self.InCity.CyCity))

				elif TYPE == "AutomateProduction":
					InCity = self.InCity
					if not InCity: raise "Should not occur"
					city = InCity.CyCity
					if not city: raise "Should not occur"

					if city.isProductionAutomated():
						szTxt = TRNSLTR.getText("TXT_KEY_MISC_OFF_PROD_AUTO", ())
					else: szTxt = TRNSLTR.getText("TXT_KEY_MISC_ON_PROD_AUTO", ())

					self.updateTooltip(screen, szTxt)


			elif BASE == "MMB":
				if TYPE == "ScoreToggle":
					self.updateTooltip(screen, TRNSLTR.getText("TXT_KEY_HUD_BUTTON_TOGGLE_SCORE", ()))
				elif TYPE == "BareMap":
					self.updateTooltip(screen, TRNSLTR.getText("TXT_KEY_HUD_BUTTON_TOGGLE_BARE_MAP", ()))

			elif NAME == "AdvBtn":
				advisorTip = self.advisorButtonTip[ID]
				szTxt = "<color=102,229,255>" + advisorTip[0] + "  <color=144,255,72>&#60" + advisorTip[1] + "&#62</color>"
				if ID == 1:
					self.treasuryHelp(screen, szTxt)
				else:
					self.updateTooltip(screen, szTxt)

			elif NAME == "GreatPersonBar":
				self.helpGreatPersonBar(screen)

			elif NAME == "ScoreText":
				self.bPlotHelpBan = True
				screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
				screen.hide("PlotHelp")
				POINT = Win32.getCursorPos()
				self.xMouseNoPlotHelp = POINT.x; self.yMouseNoPlotHelp = POINT.y

			elif NAME == "PediaButton":
				self.updateTooltip(screen, "<color=101,229,255>Wiki  <color=144,255,72>&#60F12&#62<color=255,255,255>")

			elif NAME == "OptionsBtnBUG":
				szTxt = TRNSLTR.getText("TXT_KEY_BUG_OPTIONS_SCREEN_HOVER", ())
				szTxt += "\n\t<color=80,255,80>&#60Alt+Ctrl+O&#62"
				self.updateTooltip(screen, szTxt)

			elif NAME == "BuildListBtn":
				self.updateTooltip(screen, TRNSLTR.getText("TXT_KEY_MISC_BUILD_LIST_SCREEN_HOVER", ()))

			elif NAME == "ParallelMapsBtn":
				self.updateTooltip(screen, TRNSLTR.getText("TXT_KEY_PARALLEL_MAPS_SCREEN_HOVER", ()))

			elif NAME == "EraIndicator":
				self.updateTooltip(screen, GC.getEraInfo(self.CyPlayer.getCurrentEra()).getDescription())

			elif NAME == "CivilizationFlag":
				self.updateTooltip(screen, CyGTM.getFlagHelp(), -12, self.yBotBar + 8)

			elif NAME == "DebugBtn":
				self.updateTooltip(screen, "Debug screen")

		elif not iCode: # click
			if BASE == "WID":

				if szFlag == "MOUSE_RBUTTONUP":
					if TYPE == "UNIT":
						UP.pediaJumpToUnit([iType])
					elif TYPE == "BUILDING":
						UP.pediaJumpToBuilding([iType])
					elif TYPE == "PROJECT":
						UP.pediaJumpToProject([iType])
					elif TYPE == "PROMO":
						UP.pediaJumpToPromotion([iType])
					elif TYPE == "TECH":
						if CASE[0] == "ProgBar":
							iType = GC.getPlayer(self.iPlayer).getCurrentResearch()
						UP.pediaJumpToTech([iType])

				elif szFlag == "MOUSE_LBUTTONUP":

					if CASE[0] == "CityWork":
						# City work order selected
						InCity = self.InCity
						if not InCity: return
						CyPlayer = InCity.CyPlayer
						CyCity = InCity.CyCity

						# Some modifier key logic
						if bCtrl:
							bShift = False
						else:
							if self.bInverseShiftQueue:
								bShift = not bShift

							if not bShift and CyCity.getProductionProgress():
								bCtrl = True

						# Determine order type
						iGaMsgType = GameMessageTypes.GAMEMESSAGE_PUSH_ORDER
						szTxt = ""
						if TYPE == "PROCESS":
							iOrder = OrderTypes.ORDER_MAINTAIN
							szTxt = GC.getProcessInfo(iType).getDescription()

						elif TYPE == "LIST":
							iOrder = OrderTypes.ORDER_LIST
							if bAlt:
								szTxt = "*"
							szTxt += CyPlayer.getBLListName(CyPlayer.getBLIndexByID(iType))

						elif TYPE == "UNIT":
							iOrder = OrderTypes.ORDER_TRAIN
							if bAlt:
								szTxt = "*"
							szTxt += GC.getUnitInfo(iType).getDescription()
							if self.isUnitMaxedOut(iType, InCity, 1):
								if self.iCityTab == CITYTAB_ADMIN:
									self.bUpdateCityTab = True
								else:
									screen.enable(NAME + str(iType), False)

						elif TYPE == "BUILDING":
							iOrder = OrderTypes.ORDER_CONSTRUCT
							CvBuildingInfo = GC.getBuildingInfo(iType)
							szTxt = CvBuildingInfo.getDescription()
							iSpeci = CvBuildingInfo.getSpecialBuildingType()
							if iSpeci > -1:
								self.bUpdateCityTab = self.iCityTab > CITYTAB_ADMIN
							else:
								screen.hide(NAME + str(iType))

						elif TYPE == "PROJECT":
							iOrder = OrderTypes.ORDER_CREATE
							szTxt = GC.getProjectInfo(iType).getDescription()
							screen.hide(NAME + str(iType))

						if self.bUpdateCityTab:
							screen.setHitTest("CityTabScrPnl", HitTestTypes.HITTEST_NOHIT)
						# Push order
						GAME.selectedCitiesGameNetMessage(iGaMsgType, iOrder, iType, 0, False, bAlt, bShift, bCtrl)
						# Update Work Queue
						uFont = self.aFontList[5]
						w = self.xMidL - 80

						if self.iResID == 2:
							dy = 24
						elif self.iResID == 1:
							dy = 22
						else:
							dy = 20
						szTxt = TextUtil.evalTextWidth(w - 80, uFont, szTxt)
						iNode = 0
						szRow = str(InCity.QueueIndex)
						self.InCity.QueueIndex += 1
						szName = BASE + "|" + TYPE + "|"
						ROW = "QueueRow"
						if bCtrl: # First
							y = dy
							for i, entry in enumerate(InCity.WorkQueue):
								screen.moveItem(ROW + entry[2], -2, y-2, 0)
								y += dy
							self.InCity.WorkQueue = [[szName, iType, szRow]] + InCity.WorkQueue
						elif bShift: # Last
							iNode = CyIF.getNumOrdersQueued()
							self.InCity.WorkQueue.append([szName, iType, szRow])
						else: # Replace current
							if InCity.WorkQueue:
								if self.bFreshQueue:
									screen.show(InCity.WorkQueue[0][0] + "CityWork" + str(InCity.WorkQueue[0][1]))
								elif self.iCityTab > -1:
									self.bUpdateCityTab = True
								screen.deleteWidget(ROW + InCity.WorkQueue[0][2])
								self.InCity.WorkQueue[0] = [szName, iType, szRow]
							else:
								self.InCity.WorkQueue = [[szName, iType, szRow]]
						# Add row to queue
						eWidGen = WidgetTypes.WIDGET_GENERAL
						ePanelSTD = PanelStyles.PANEL_STYLE_STANDARD
						szName += "QueueEntry" + szRow
						ROW += szRow
						screen.attachPanelAt("CityWorkQueue", ROW, "", "", True, False, ePanelSTD, -2, dy*iNode-2, w-18, dy, eWidGen, 1, 1)
						screen.setTextAt(szName, ROW, szTxt, 1<<0, 4, -2, 0, FontTypes.GAME_FONT, eWidGen, 1, 1)
						screen.hide("CityWorkQueue")
						screen.show("CityWorkQueue")
						self.NewQueueRowTime = [TYPE, iType, szRow, iNode, bAlt, w-66, uFont]
						screen.setFocus("CityTabScrPnl")

					elif CASE[0] == "QueueEntry":
						# Clicked an entry in the city work queue
						InCity = self.InCity
						if not InCity or InCity.CyCity.getOrderQueueLength() < 2: return
						iTab = self.iCityTab
						# Check if the work selection needs a refresh.
						if TYPE == "UNIT":
							if iTab in (CITYTAB_ADMIN, CITYTAB_UNIT) and self.isUnitMaxedOut(iType, InCity):
								self.bUpdateCityTab = True

						elif TYPE == "BUILDING":
							if iTab in (CITYTAB_BUILDING, CITYTAB_WONDER):
								self.bUpdateCityTab = True
						elif iTab > -1:
							screen.show(WHAT + "CityWork" + str(iType))
						# Remove entry from queue
						GAME.selectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_POP_ORDER, iNode, 0, 0, False, bAlt, bShift, bCtrl)
						screen.deleteWidget("QueueRow" + szRow)
						del InCity.WorkQueue[iNode]
						# Move lower queue rows one step up
						if self.iResID == 2:
							dy = 24
						elif self.iResID == 1:
							dy = 22
						else:
							dy = 20
						y = iNode * dy
						for i, entry in enumerate(InCity.WorkQueue[iNode:]):
							screen.moveItem("QueueRow" + entry[2], -2, y-2, 0)
							y += dy
						screen.hide("CityWorkQueue")
						screen.show("CityWorkQueue")

					elif TYPE == "TECH":
						if CASE[0] == "Selection":
							CyMessageControl().sendResearch(ID, bShift)
						elif CASE[0] == "ProgBar":
							CyMessageControl().sendResearch(-1, bShift)
						elif CASE[0] == "Score":
							UP.pediaJumpToTech([ID])

					elif TYPE == "PROMO":
						if CASE[0] == "PromoPanel":
							UP.pediaJumpToPromotion([ID])

			elif BASE == "PlotList":
				if TYPE == "Button":
					if szFlag == "MOUSE_RBUTTONUP":
						UP.pediaJumpToUnit([self.aPlotListList[ID][1]])
					else:
						CyUnit = self.aPlotListList[ID][0]
						if CyUnit.getOwner() == self.iPlayer:
							CyIF.selectGroup(CyUnit, bShift, bCtrl, bAlt)
							self.bPlotListAutoScroll = False
				elif TYPE == "Scroll":
					self.bPlotListAutoScroll = False
					if ID:
						if self.iPlotListTopRow != self.iMaxPlotListScroll:
							self.iPlotListTopRow += 1
						if self.iPlotListTopRow == self.iMaxPlotListScroll:
							screen.hide("PlotList|Scroll1")
						screen.show("PlotList|Scroll0")
						CyIF.setDirty(InterfaceDirtyBits.PlotListButtons_DIRTY_BIT, True)
					else:
						self.iPlotListTopRow -= 1
						if not self.iPlotListTopRow:
							screen.hide("PlotList|Scroll0")
						screen.show("PlotList|Scroll1")
						CyIF.setDirty(InterfaceDirtyBits.PlotListButtons_DIRTY_BIT, True)

			elif BASE == "CityTab":
				iTab = self.iCityTab
				if ID == iTab or TYPE == "BG":
					self.exitCityTab(screen)
				else:
					self.openCityTab(screen, ID)

			elif BASE == "CT":
				if TYPE == "UnitFilter":
					self.InCity.CyCity.setUnitListFilterActive(ID, not self.InCity.CyCity.getUnitListFilterActive(ID))
					self.updateCityTab(screen)

				elif TYPE == "BuildingFilter":
					self.InCity.CyCity.setBuildingListFilterActive(ID, not self.InCity.CyCity.getBuildingListFilterActive(ID))
					self.updateCityTab(screen)

				elif TYPE == "Options":
					import CityOptions
					self.cityOptions = CityOptions.CityOptions(screen, self)

				elif TYPE == "Cityzoom":
					self.exitCityTab(screen)
					CyIF.selectCity(self.InCity.CyCity, False)
					self.bUpdateCityTab = True

			elif BASE == "BldgList":
				if TYPE == "Demolish":
					ACEM.startCityDemolish(screen, self.xRes, self.yRes)
					return
				iShowBuildings = self.iShowBuildings
				if ID == iShowBuildings:
					return
				y1 = self.yTradeRouteTable + self.hTradeRouteTable + 9
				y2 = y1 + 2
				szType = str(iShowBuildings)
				screen.moveItem("BldgList|Btn" + szType, 9 + 40 * iShowBuildings, y1, 0)
				screen.setStyle("BldgList|Bkgnd" + szType, "GFC_Control_OutPanel_Style")
				szType = str(ID)
				screen.moveItem("BldgList|Btn" + szType, 9 + 40 * ID, y2, 0)
				screen.setStyle("BldgList|Bkgnd" + szType, "GFC_Control_InPanel_Style")
				self.iShowBuildings = ID
				self.buildCityListLeft(screen, self.InCity)

			elif BASE == "BonusList":
				iShowBonus = self.iShowBonus
				if ID == iShowBonus:
					return
				x = 3 * self.xRes / 4 + 9
				szType = str(iShowBonus)
				screen.moveItem("BonusList|Btn" + szType, x + 40 * iShowBonus, 133, 0)
				screen.setStyle("BonusList|Bkgnd" + szType, "GFC_Control_OutPanel_Style")
				szType = str(ID)
				screen.moveItem("BonusList|Btn" + szType, x + 40 * ID, 135, 0)
				screen.setStyle("BonusList|Bkgnd" + szType, "GFC_Control_InPanel_Style")
				self.iShowBonus = ID
				self.buildCityListRight(screen, self.InCity)

			elif BASE == "ComPercent":
				if szFlag == "MOUSE_RBUTTONUP":
					if bCtrl or bShift or bAlt:
						szTxt = TRNSLTR.getText("TXT_KEY_SHORTCUTS_COMMERCE_CONTROL_PEDIA", ())
						szTxt = "\n" + szTxt[szTxt.find("\t"):]
						szTxt = szTxt.replace("\n\t", "\n")[1:]
						self.updateTooltip(screen, szTxt, -1, -1, self.aFontList[3])
						return 1
					iChange = 100
				elif not bAlt:
					if bCtrl:
						iChange = 1
					else:
						iChange = 5
					if bShift:
						iChange *= 4

				else: # Alternative Scheme
					if bCtrl and bShift:
						iChange = 100
					elif bCtrl:
						iChange = 10
					elif bShift:
						iChange = 25
					else:
						iChange = 50

				if TYPE == "Dec":
					iChange = -iChange

				CyMessageControl().sendModNetMessage(999, self.iPlayer, ID, iChange, 0)
				return 1

			elif BASE == "CS":
				# City Screen
				if TYPE == "RawYields":
					if not ID:
						self.bYieldView = False
					elif ID in (1, 2, 3):
						self.bYieldView = True
						self.iYieldType = RawYields.YIELDS[ID - 1]
					elif ID in (4, 5, 6):
						self.bYieldView = True
						self.iYieldTiles = RawYields.TILES[ID - 4]
					else:
						return 0
					CyIF.setDirty(InterfaceDirtyBits.CityScreen_DIRTY_BIT, True)

				elif TYPE == "RevBar":
					if self.InCity.iPlayer == self.iPlayer:
						RevInstances.RevolutionInst.showBribeCityPopup(self.InCity.CyCity)

				elif TYPE == "AutomateProduction":
					bAutomate = not self.InCity.CyCity.isProductionAutomated()
					GAME.selectedCitiesGameNetMessage(
						GameMessageTypes.GAMEMESSAGE_DO_TASK,
						TaskTypes.TASK_SET_AUTOMATED_PRODUCTION,
						-1, -1, bAutomate, bAlt, bShift, bCtrl
					)
					if bAutomate: self.bBuildWorkQueue = True

			elif BASE == "MMB":
				if TYPE == "ScoreToggle":
					CyIF.toggleScoresVisible()
				elif TYPE == "BareMap":
					CyIF.toggleBareMapMode()

			elif NAME == "AdvBtn":
				if not ID:
					UP.showDomesticAdvisor(-1)
				elif ID == 1:
					UP.showFinanceAdvisor()
				elif ID == 2:
					UP.showCivicsScreen()
				elif ID == 3:
					UP.showForeignAdvisorScreen([0])
				elif ID == 4:
					GAME.doControl(ControlTypes.CONTROL_MILITARY_SCREEN)
				elif ID == 5:
					UP.showTechChooser()
				elif ID == 6:
					UP.showReligionScreen()
				elif ID == 7:
					UP.showCorporationScreen()
				elif ID == 8:
					UP.showEspionageAdvisor()
				elif ID == 9:
					UP.showRevolutionWatchAdvisor(self)
				elif ID == 10:
					UP.showHeritageScreen()

			elif NAME == "PediaButton":
				UP.pediaShow()

			elif NAME == "OptionsBtnBUG":
				import BugOptionsScreen
				BugOptionsScreen.showOptionsScreen()

			elif NAME == "BuildListBtn":
				UP.showBuildListScreen()

			elif NAME == "ParallelMapsBtn":
				UP.showParallelMapsScreen()

			elif NAME == "DebugBtn":
				UP.showDebugScreen()

			elif NAME == "GreatPersonBar":
				# Zoom to next GP city
				CyCity, _ = GPUtil.findNextCity()
				if CyCity:
					CyIF.selectCity(CyCity, False)

			elif NAME == "EraIndicator":
				UP.pediaJumpToEra(iData1)

			elif NAME == "CivilizationFlag":
				GAME.doControl(ControlTypes.CONTROL_SELECTCAPITAL)

		elif iCode == 11: # List Select
			InCity = self.InCity
			if InCity:
				if BASE == "CT":
					if TYPE == "BuildingSorting":
						iBuildingSorting = screen.getPullDownData(NAME, iData)
						if iBuildingSorting >= 0:
							self.InCity.CyCity.setBuildingListSorting(iBuildingSorting)
					elif TYPE == "UnitGrouping":
						iUnitGrouping = screen.getPullDownData(NAME, iData)
						if iUnitGrouping >= 0:
							self.InCity.CyCity.setUnitListGrouping(iUnitGrouping)
					elif TYPE == "UnitSorting":
						iUnitSorting = screen.getPullDownData(NAME, iData)
						if iUnitSorting >= 0:
							self.InCity.CyCity.setUnitListSorting(iUnitSorting)
					self.updateCityTab(screen)

		elif iCode == 20: # slider stop
			if NAME == "FoVSlider":
				self.iField_View = iFoV = inputClass.getData()
				self.setFieldofView(iFoV)
				self.setFieldofView_Text(screen, iFoV)
				MainOpt.setFieldOfView(iFoV)
				import BugOptions
				BugOptions.getOptions(MainOpt._id).write()

	def forward(self):
		print "forward"
		if not CyIF.isFocused() or self.bCityScreen:
			if CyIF.isCitySelection():
				GAME.doControl(ControlTypes.CONTROL_NEXTCITY)
			else:
				GAME.doControl(ControlTypes.CONTROL_NEXTUNIT)

	def back(self):
		print "back"
		if not CyIF.isFocused() or self.bCityScreen:
			if CyIF.isCitySelection():
				GAME.doControl(ControlTypes.CONTROL_PREVCITY)
			else:
				GAME.doControl(ControlTypes.CONTROL_PREVUNIT)

	def initMinimap(self):
		CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE).initMinimap(self.xMinimap, self.xRes - 3, self.yMinimap, self.yRes - 3, -0.1)

# # # # # # #
# Pop-Up Callbacks
def applyCityTabOptions(iPlayer, userData, popupReturn):
	CityOpt.setBuildIconSize(popupReturn.getSpinnerWidgetValue(0))
	screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
	g_mainInterface.buildCityTabButtons(screen, g_mainInterface.InCity.CyCity)
	g_mainInterface.updateCityTab(screen)
	g_mainInterface.cityOptions.drawOptions(screen, g_mainInterface)


# # # # # # #
# Mini-Classes
class City:
	def __init__(self, CyCity, iCityID):
		self.CyCity		= CyCity
		self.iCityID	= iCityID
		self.iPlayer	= iPlayer = CyCity.getOwner()
		self.iTeam		= iTeam = CyCity.getTeam()
		self.CyPlayer	= GC.getPlayer(iPlayer)
		self.CyTeam		= GC.getTeam(iTeam)
		self.listBonus = [True,[],[],[]]
		self.WorkQueue = []
		self.QueueIndex = 0

class Unit:
	def __init__(self, CyUnit, iUnitID):
		self.CyUnit		= CyUnit
		self.iUnitID	= iUnitID
		self.iPlayer	= iPlayer = CyUnit.getOwner()
		self.iTeam		= iTeam = CyUnit.getTeam()
		self.CyPlayer	= GC.getPlayer(iPlayer)
		self.CyTeam		= GC.getTeam(iTeam)
