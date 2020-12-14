#
# Info Screen - Contains the Demographics, Wonders / Top Cities and Statistics Screens
#
# Thanks to "Ulf 'ulfn' Norell" from Apolyton for his additions relating to the graph section of this screen
#
from CvPythonExtensions import *

import string
import math

# globals
GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

class CvInfoScreen:

	def __init__(self, screenId):

		self.screenId = screenId

		self.szWonderDisplayMode = "WorldWonders"

		self.iGraphEnd		= None
		self.iGraphZoom		= None
		self.nWidgetCount	= 0
		self.nLineCount		= 0

		self.iTab = 0

		self.iGraph1 = 0
		self.iGraph2 = 1
		self.iGraph_Smoothing = 2

		self.bDoubleGraph = False

		# Placement of top 5 Cities
		self.Y_ROWS_CITIES = [90, 208, 326, 444, 562] # delta = 118


	# Screen construction function
	def interfaceScreen(self, iTabID, iEndGame):

		if GAME.getReplayMessageTurn(GAME.getNumReplayMessages() - 1) < -1:
			return
		screen = CyGInterfaceScreen("InfoScreen", self.screenId)
		if screen.isActive():
			return

		if iTabID > -1:
			self.iTab = iTabID

		self.bDebug = GAME.isDebugMode()
		self.iTurn = GAME.getGameTurn()
		self.iStartTurn = GAME.getStartTurn()

		self.aWidgetBucket = []

		import BugCore
		AdvisorOpt = BugCore.game.Advisors
		self.bGraphLogScale = AdvisorOpt.isGraphsLogScale()
		self.bWonderPlayerColor = AdvisorOpt.isWonderListUsePlayerColor()
		self.bShowImprovement = AdvisorOpt.isShowImprovements()

		# Get screen resolution.
		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y
		# Calibrate variables.
		if yRes > 1000:
			H_BOT_PANEL = 38
			uFontEdge = "<font=4b>"
		elif yRes > 800:
			H_BOT_PANEL = 32
			uFontEdge = "<font=3b>"
		else:
			H_BOT_PANEL = 29
			uFontEdge = "<font=2b>"

		self.aFontList = aFontList = [uFontEdge] + SR.aFontList
		# uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2, uFont1b, uFont1 = self.aFontList
		uFont2b = aFontList[5]

		self.Y_TOP_PAGE = 36
		self.H_PAGE = H_PAGE = yRes - self.Y_TOP_PAGE - H_BOT_PANEL + 8
		self.Y_BOT_TEXT = Y_BOT_TEXT = yRes - H_BOT_PANEL + 8

		# Initialize text
		self.TEXT_DEMOGRAPHICS_SMALL = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_TITLE", ())
		self.TEXT_ENTIRE_HISTORY = TRNSLTR.getText("TXT_KEY_INFO_ENTIRE_HISTORY", ())
		self.TEXT_TURNS = TRNSLTR.getText("TXT_KEY_TURNS", ())

		self.TEXT_SCORE = TRNSLTR.getText("TXT_KEY_GAME_SCORE", ())
		self.TEXT_POWER = TRNSLTR.getText("TXT_KEY_POWER", ())
		self.TEXT_CULTURE = TRNSLTR.getObjectText("TXT_KEY_COMMERCE_CULTURE", 0)
		self.TEXT_ESPIONAGE = TRNSLTR.getObjectText("TXT_KEY_ESPIONAGE_CULTURE", 0)

		self.TEXT_VALUE = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_VALUE_TEXT", ())
		self.TEXT_RANK = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_RANK_TEXT", ())
		self.TEXT_AVERAGE = TRNSLTR.getText("TXT_KEY_DEMOGRAPHICS_SCREEN_RIVAL_AVERAGE", ())
		self.TEXT_BEST = TRNSLTR.getText("TXT_KEY_INFO_RIVAL_BEST", ())
		self.TEXT_WORST = TRNSLTR.getText("TXT_KEY_INFO_RIVAL_WORST", ())

		self.TEXT_ECONOMY = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_ECONOMY_TEXT", ())
		self.TEXT_INDUSTRY = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_INDUSTRY_TEXT", ())
		self.TEXT_AGRICULTURE = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_AGRICULTURE_TEXT", ())
		self.TEXT_MILITARY = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_MILITARY_TEXT", ())
		self.TEXT_LAND_AREA = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_LAND_AREA_TEXT", ())
		self.TEXT_POPULATION = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_POPULATION_TEXT", ())
		self.TEXT_HAPPINESS = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_HAPPINESS_TEXT", ())
		self.TEXT_HEALTH = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_HEALTH_TEXT", ())
		self.TEXT_IMP_EXP = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_EXPORTS_TEXT", ()) + " - " + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_IMPORTS_TEXT", ())

		charBullet = u"  %c" % GAME.getSymbolID(FontSymbols.BULLET_CHAR)
		self.TEXT_ECONOMY_MEASURE = charBullet + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_ECONOMY_MEASURE", ())
		self.TEXT_INDUSTRY_MEASURE = charBullet + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_INDUSTRY_MEASURE", ())
		self.TEXT_AGRICULTURE_MEASURE = charBullet + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_AGRICULTURE_MEASURE", ())
		self.TEXT_MILITARY_MEASURE = ""
		self.TEXT_LAND_AREA_MEASURE = charBullet + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_LAND_AREA_MEASURE", ())
		self.TEXT_POPULATION_MEASURE = ""
		self.TEXT_HAPPINESS_MEASURE = "%"
		self.TEXT_HEALTH_MEASURE = charBullet + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_POPULATION_MEASURE", ())
		self.TEXT_IMP_EXP_MEASURE = charBullet + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_ECONOMY_MEASURE", ())

		self.TEXT_TIME_PLAYED = TRNSLTR.getText("TXT_KEY_INFO_SCREEN_TIME_PLAYED", ())
		self.TEXT_CITIES_BUILT = TRNSLTR.getText("TXT_KEY_INFO_SCREEN_CITIES_BUILT", ())
		self.TEXT_CITIES_RAZED = TRNSLTR.getText("TXT_KEY_INFO_SCREEN_CITIES_RAZED", ())
		self.TEXT_NUM_GOLDEN_AGES = TRNSLTR.getText("TXT_KEY_INFO_SCREEN_NUM_GOLDEN_AGES", ())
		self.TEXT_NUM_RELIGIONS_FOUNDED = TRNSLTR.getText("TXT_KEY_INFO_SCREEN_RELIGIONS_FOUNDED", ())

		self.TEXT_CURRENT = TRNSLTR.getText("TXT_KEY_CURRENT", ())
		self.TEXT_UNITS = TRNSLTR.getText("TXT_KEY_CONCEPT_UNITS", ())
		self.TEXT_BUILDINGS = TRNSLTR.getText("TXT_KEY_CONCEPT_BUILDINGS", ())
		self.TEXT_KILLED = TRNSLTR.getText("TXT_KEY_INFO_SCREEN_KILLED", ())
		self.TEXT_LOST = TRNSLTR.getText("TXT_KEY_INFO_SCREEN_LOST", ())
		self.TEXT_BUILT = TRNSLTR.getText("TXT_KEY_INFO_SCREEN_BUILT", ())
		self.TEXT_IMPROVEMENTS = TRNSLTR.getText("TXT_KEY_CONCEPT_IMPROVEMENTS", ())

		self.SHOW_ALL = uFont2b + TRNSLTR.getText("TXT_KEY_SHOW_ALL", ())
		self.SHOW_NONE = uFont2b + TRNSLTR.getText("TXT_KEY_SHOW_NONE", ())
		self.LOG_SCALE = uFont2b + TRNSLTR.getText("TXT_KEY_LOGSCALE", ())

		self.szName = TRNSLTR.getText("TXT_KEY_NAME", ())
		self.szWonderDate = TRNSLTR.getText("TXT_KEY_WONDER_DATE", ())
		self.szWonderOwner = TRNSLTR.getText("TXT_KEY_WONDER_OWNER", ())
		self.szWonderCity = TRNSLTR.getText("TXT_KEY_WONDER_CITY", ())

		sTemp1 = [
			TRNSLTR.getText("TXT_KEY_GAME_SCORE", ()),
			TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_ECONOMY_TEXT", ()),
			TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_INDUSTRY_TEXT", ()),
			TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_AGRICULTURE_TEXT", ()),
			TRNSLTR.getText("TXT_KEY_POWER", ()),
			TRNSLTR.getObjectText("TXT_KEY_COMMERCE_CULTURE", 0),
			TRNSLTR.getObjectText("TXT_KEY_ESPIONAGE_CULTURE", 0)
		]
		if GAME.isOption(GameOptionTypes.GAMEOPTION_REVOLUTION):
			sTemp1.append(TRNSLTR.getText("TXT_KEY_REVOLUTION_GRAPH_TITLE_SMALL", ()))
			self.iNumGraphs = 8
		else: self.iNumGraphs = 7

		self.iBlack = GC.getInfoTypeForString("COLOR_BLACK")
		iYellow = GC.getInfoTypeForString("COLOR_YELLOW")
		sTemp2 = []
		for txt in sTemp1:
			sTemp2.append(TRNSLTR.changeTextColor(txt, iYellow))

		self.sGraphText = [sTemp1, sTemp2]

		# determine the big graph text spacing
		self.X_GRAPH_TEXT = []
		dX = (xRes - 16) / self.iNumGraphs
		x = xRes - 16
		for i in xrange(self.iNumGraphs):
			self.X_GRAPH_TEXT.append(x)
			x -= 32 + CyInterface().determineWidth(aFontList[3] + sTemp1[i])

		self.BUG_LEGEND_DEAD = TRNSLTR.getText("TXT_KEY_BUG_DEAD_CIV", ())

		AFM = CyArtFileMgr()
		# world wonder / national wonder / projects icons / buttons
		self.BUGWorldWonder_On = AFM.getInterfaceArtInfo("BUG_WORLDWONDER_ON").getPath()
		self.BUGWorldWonder_Off = AFM.getInterfaceArtInfo("BUG_WORLDWONDER_OFF").getPath()
		self.BUGNatWonder_On = AFM.getInterfaceArtInfo("BUG_NATWONDER_ON").getPath()
		self.BUGNatWonder_Off = AFM.getInterfaceArtInfo("BUG_NATWONDER_OFF").getPath()
		self.BUGProject_On = AFM.getInterfaceArtInfo("BUG_PROJECT_ON").getPath()
		self.BUGProject_Off = AFM.getInterfaceArtInfo("BUG_PROJECT_OFF").getPath()

		self.pathNullBtn = AFM.getInterfaceArtInfo("INTERFACE_BUTTON_NULL").getPath()
		self.zoomArt = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION").getPath()

		# Cache some espionage mission indexes
		self.iDemographicsMission = -1
		self.iInvestigateCityMission = -1
		for iMissionLoop in xrange(GC.getNumEspionageMissionInfos()):
			if GC.getEspionageMissionInfo(iMissionLoop).isSeeDemographics():
				self.iDemographicsMission = iMissionLoop
			if GC.getEspionageMissionInfo(iMissionLoop).isInvestigateCity():
				self.iInvestigateCityMission = iMissionLoop
		# Cache player specifics
		self.iPlayer = iPlayer = GAME.getActivePlayer()
		self.determineKnownPlayers(iEndGame)

		# Build screen
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		screen.setRenderInterfaceOnly(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.addDDSGFC("Info_BG", AFM.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, xRes, yRes, eWidGen, 1, 2)
		screen.addDDSGFC("Graph_BG", AFM.getInterfaceArtInfo("SCREEN_BG").getPath(), 0, 0, self.xRes, self.yRes, eWidGen, 1, 2)
		screen.hide("Graph_BG")
		screen.addPanel("Info_TopPanel", "", "", True, False, 0, -2, xRes, 40, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("Info_BotPanel", "", "", True, False, 0, yRes - H_BOT_PANEL, xRes, H_BOT_PANEL, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.setLabel("Info_Header", "", uFontEdge + TRNSLTR.getText("TXT_KEY_INFO_SCREEN",()), 1<<2, xRes / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("Info_Exit", "", uFontEdge + TRNSLTR.getText("TXT_KEY_PEDIA_SCREEN_EXIT",()), 1<<1, xRes - 16, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		# Add tab control
		szTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_TITLE", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iYellow)
		dX = xRes / 4
		x = dX / 2
		screen.setText("VS_Tab0", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("VS_Tab_Act0", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("VS_Tab_Act0")

		szTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_INFO_GRAPH", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iYellow)
		x += dX
		screen.setText("VS_Tab1", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("VS_Tab_Act1", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("VS_Tab_Act1")

		szTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_WONDERS_SCREEN_TOP_CITIES_TEXT", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iYellow)
		x += dX
		screen.setText("VS_Tab2", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("VS_Tab_Act2", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("VS_Tab_Act2")

		szTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_INFO_SCREEN_STATISTICS_TITLE", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iYellow)
		x += dX
		screen.setText("VS_Tab3", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("VS_Tab_Act3", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("VS_Tab_Act3")

		# Debug DropDown
		if self.bDebug:
			DD = "Info_DebugDD"
			screen.addDropDownBoxGFC(DD, 8, 0, 300, eWidGen, 1, 2, FontTypes.GAME_FONT)
			for iPlayerX in xrange(GC.getMAX_PLAYERS()):
				if GC.getPlayer(iPlayerX).isAlive():
					screen.addPullDownString(DD, GC.getPlayer(iPlayerX).getName(), iPlayerX, iPlayerX, iPlayer == iPlayerX)
			if not self.iTab:
				screen.hide("Info_DebugDD")

		# Reset variables
		iLastTurn = self.iTurn - 1
		if self.iGraphEnd != iLastTurn:
			self.iGraphEnd = iLastTurn
			self.iGraphZoom = self.iGraphEnd - self.iStartTurn
			if self.iGraphZoom > 1000:
				self.iGraphZoom = 1000

		self.redrawContents()


	def redrawContents(self):

		self.iCitySizes = [-1, -1, -1, -1, -1]
		self.aaCitiesXY = [[-1, -1], [-1, -1], [-1, -1], [-1, -1], [-1, -1]]
		self.iCityValues = [0, 0, 0, 0, 0]
		self.pCityPointers = [0, 0, 0, 0, 0]
		self.scoreCache = [None] * self.iNumGraphs

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)
		self.deleteAllWidgets(screen)
		# Specific widgets
		for widget in self.aWidgetBucket:
			screen.deleteWidget(widget)
		self.aWidgetBucket = []

		# Draw Tab buttons and tabs
		if not self.iTab:
			screen.hide("VS_Tab0")
			screen.show("VS_Tab_Act0")
			self.drawDemographicsTab()

		elif self.iTab == 1:
			screen.show("Graph_BG")
			screen.hide("VS_Tab1")
			screen.show("VS_Tab_Act1")
			self.drawGraphTab()

		elif self.iTab == 2:
			screen.hide("VS_Tab2")
			screen.show("VS_Tab_Act2")
			self.drawTopCitiesTab()

		elif self.iTab == 3:
			screen.hide("VS_Tab3")
			screen.show("VS_Tab_Act3")
			self.drawStatsTab()

#############################################################################################################
#################################################### GRAPH ##################################################
#############################################################################################################

	def drawGraphTab(self):

		self.bPlayerInclude = [True] * GC.getMAX_PC_PLAYERS()

		xRes = self.xRes
		self.wLegend = wLegend = 128 + xRes / 20
		self.xGraph = xGraph = 32 + wLegend
		self.wGraph = wGraph = xRes - xGraph - 16
		self.iNumReference = 2 + xRes / 420
		self.iReferenceDelta = wGraph / (self.iNumReference + 1)

		self.sGraphTextHeadingWidget = []
		self.sGraphTextBannerWidget = []
		self.sGraphPanelWidget = []
		for i in xrange(self.iNumGraphs):
			self.sGraphTextHeadingWidget.append(self.getNextWidgetName())
			self.sGraphTextBannerWidget.append(self.getNextWidgetName())
			self.sGraphPanelWidget.append(self.getNextWidgetName())

		self.sPlayerTextWidget = [0] * GC.getMAX_PC_PLAYERS()
		for i in xrange(GC.getMAX_PC_PLAYERS()):
			self.sPlayerTextWidget[i] = self.getNextWidgetName()

		self.sShowAllWidget = self.getNextWidgetName()
		self.sShowNoneWidget = self.getNextWidgetName()

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT
		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		y = self.Y_BOT_TEXT - 30
		screen.setButtonGFC("GraphShiftL", "", "", xGraph - 12, y, 20, 20, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_ARROW_LEFT)
		screen.setButtonGFC("GraphShiftR", "", "", xRes - 24, y, 20, 20, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_ARROW_RIGHT)
		self.aWidgetBucket.append("GraphShiftL")
		self.aWidgetBucket.append("GraphShiftR")


		# graph smoothing dropdown
		self.szGraphSmoothingDropdownWidget = self.getNextWidgetName()

		screen.addDropDownBoxGFC(self.szGraphSmoothingDropdownWidget, xRes - 316, 0, 200, eWidGen, 1, 2, eFontGame)
		for i in xrange(6):
			iV = 2*i
			screen.addPullDownString(self.szGraphSmoothingDropdownWidget, TRNSLTR.getText("TXT_KEY_GRAPH_SMOOTHING", (iV,)), i, iV, self.iGraph_Smoothing == iV)

		# graph range dropdown
		start = self.iStartTurn
		now   = self.iTurn
		bEntireHistory = self.iGraphZoom == self.iGraphEnd - start
		self.dropDownTurns = []
		self.szTurnsDropdownWidget = self.getNextWidgetName()
		screen.addDropDownBoxGFC(self.szTurnsDropdownWidget, 16, self.H_PAGE, wLegend, eWidGen, 1, 2, eFontGame)
		nTurns = now - start - 1
		screen.addPullDownString(self.szTurnsDropdownWidget, self.TEXT_ENTIRE_HISTORY, 0, 0, bEntireHistory)
		self.dropDownTurns.append(nTurns)
		if 50 < nTurns:
			screen.addPullDownString(self.szTurnsDropdownWidget, self.TEXT_TURNS + ": " + str(50), 1, 1, not bEntireHistory and self.iGraphZoom == 50)
			self.dropDownTurns.append(50)
			iCounter = 2
			last = 100
			while last < nTurns and last < 1001:
				screen.addPullDownString(self.szTurnsDropdownWidget, self.TEXT_TURNS + ": " + str(last), iCounter, iCounter, not bEntireHistory and self.iGraphZoom == last)
				self.dropDownTurns.append(last)
				iCounter += 1
				last += 100

		self.iNumPreDemoChartWidgets = self.nWidgetCount

		self.drawGraphs()

	def checkGraphBounds(self):
		if self.iGraphEnd - self.iGraphZoom < self.iStartTurn:
			self.iGraphEnd = self.iStartTurn + self.iGraphZoom
		if self.iGraphEnd > self.iTurn - 1:
			self.iGraphEnd = self.iTurn - 1

	def slideGraph(self, right):
		self.iGraphEnd += right
		self.checkGraphBounds()


	def computeHistory(self, scoreType, iPlayer, iTurn):

		iScore = GC.getPlayer(iPlayer).getScoreHistory(iTurn)

		if iScore == 0: # for some reason only the score is 0 when you're dead..?
			return 0

		if scoreType == 0:
			return iScore

		if scoreType == 1:
			return GC.getPlayer(iPlayer).getEconomyHistory(iTurn)

		if scoreType == 2:
			return GC.getPlayer(iPlayer).getIndustryHistory(iTurn)

		if scoreType == 3:
			return GC.getPlayer(iPlayer).getAgricultureHistory(iTurn)

		if scoreType == 4:
			return GC.getPlayer(iPlayer).getPowerHistory(iTurn)

		if scoreType == 5:
			return GC.getPlayer(iPlayer).getCultureHistory(iTurn)

		if scoreType == 6:
			return GC.getPlayer(iPlayer).getEspionageHistory(iTurn)

		if scoreType == 7:
			return 1000 - GC.getPlayer(iPlayer).getRevolutionStabilityHistory(iTurn)


	def getDate(self, turn):
		year = GAME.getTurnYear(turn)
		if year < 0:
			return self.aFontList[4] + TRNSLTR.getText("TXT_KEY_TIME_BC", (-year,))
		else: return self.aFontList[4] + TRNSLTR.getText("TXT_KEY_TIME_AD", (year,))


	def drawGraphs(self):
		startTurn = self.iStartTurn
		thisTurn = self.iTurn
		xGraph = self.xGraph
		wGraph = self.wGraph

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFont = FontTypes.TITLE_FONT

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		# Delete graph lines
		i = 0
		while i < self.nLineCount:
			screen.deleteWidget("DemoLine" + str(i))
			i += 1
		self.nLineCount = 0

		self.deleteAllWidgets(screen, self.iNumPreDemoChartWidgets)

		screen.enable("GraphShiftL", self.iGraphEnd - self.iGraphZoom > startTurn)
		screen.enable("GraphShiftR", self.iGraphEnd < thisTurn - 1)

		for i in xrange(self.iNumGraphs):
			screen.hide(self.sGraphTextHeadingWidget[i])
			screen.hide(self.sGraphPanelWidget[i])

		if self.bDoubleGraph:
			for i in xrange(self.iNumGraphs):
				screen.hide(self.sGraphTextBannerWidget[i])
			# Double graph selections
			DD = "GraphDD0"
			x = xGraph + wGraph / 2 - 208
			screen.addDropDownBoxGFC(DD, x, 40, 200, eWidGen, 1, 2, eFont)
			for j, txt in enumerate(self.sGraphText[0]):
				screen.addPullDownString(DD, txt, j, j, self.iGraph1 == j)
			self.aWidgetBucket.append(DD)

			DD = "GraphDD1"
			x += 216
			screen.addDropDownBoxGFC(DD, x, 40, 200, eWidGen, 1, 2, eFont)
			for j, txt in enumerate(self.sGraphText[0]):
				screen.addPullDownString(DD, txt, j, j, self.iGraph2 == j)
			self.aWidgetBucket.append(DD)
		else:
			screen.hide("GraphDD0")
			screen.hide("GraphDD1")

			y = self.Y_TOP_PAGE
			uFont3b = self.aFontList[3]

			for i, x in enumerate(self.X_GRAPH_TEXT):
				screen.hide(self.sGraphTextBannerWidget[i])
				if i == self.iGraph1:
					screen.setText(self.sGraphTextBannerWidget[i], "", uFont3b + self.sGraphText[1][i], 1<<1, x, y + 5, 0, eFont, eWidGen, 1, 2)
				else:
					screen.setText(self.sGraphTextBannerWidget[i], "", uFont3b + self.sGraphText[0][i], 1<<1, x, y, 0, eFont, eWidGen, 1, 2)

		# Draw graph(s)
		for i in xrange(1 + self.bDoubleGraph):

			if self.bDoubleGraph and i:
				iGraphID = self.iGraph2
			else: iGraphID = self.iGraph1

			# compute graph x, y, w, h
			iH_GRAPH = self.yRes - 134
			if self.bDoubleGraph:
				iH_GRAPH = iH_GRAPH / 2 - 4
			iY_GRAPH = 72
			if self.bDoubleGraph and i:
				iY_GRAPH += iH_GRAPH + 8

			# Draw the graph widget
			zsGRAPH_CANVAS_ID = self.getNextWidgetName()
			screen.addDrawControl(zsGRAPH_CANVAS_ID, None, xGraph, iY_GRAPH, wGraph, iH_GRAPH, eWidGen, 1, 2)
			screen.setStyle(zsGRAPH_CANVAS_ID, "Panel_HudHelp_Style")

			x = self.iReferenceDelta
			for i in xrange(self.iNumReference):
				screen.addLineGFC(zsGRAPH_CANVAS_ID, self.getNextLineName(), x, iH_GRAPH, x, 0, self.iBlack)
				x += self.iReferenceDelta

			screen.addCheckBoxGFC("GraphArea" + str(iGraphID), self.pathNullBtn, self.pathNullBtn, xGraph, iY_GRAPH, wGraph, iH_GRAPH, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_LABEL)

			# Compute max score, check if the scores have already been computed
			if self.scoreCache[iGraphID] is None:

				# Get the player with the highest ID
				maxPlayer = 0
				for p in self.aiPlayersMet:
					if maxPlayer < p:
						maxPlayer = p

				# Compute the scores
				self.scoreCache[iGraphID] = []
				for p in xrange(maxPlayer + 1):

					if p in self.aiPlayersMet:
						self.scoreCache[iGraphID].append([])
						turn = startTurn
						while turn <= thisTurn:
							self.scoreCache[iGraphID][p].append(self.computeHistory(iGraphID, p, turn))
							turn += 1
					else: # Don't compute score for people we haven't met
						self.scoreCache[iGraphID].append(None)


			if not self.iGraphZoom or not self.iGraphEnd:
				firstTurn = startTurn
			else: firstTurn = self.iGraphEnd - self.iGraphZoom

			if self.iGraphEnd:
				lastTurn = self.iGraphEnd
			else: lastTurn = thisTurn - 1 # all civs haven't neccessarily got a score for the current turn

			# Draw x-labels
			screen.setLabel(self.getNextWidgetName(), "", self.getDate(firstTurn), 1<<0, xGraph + 14, self.Y_BOT_TEXT - 30, 0, eFont, eWidGen, 1, 2)
			screen.setLabel(self.getNextWidgetName(), "", self.getDate(lastTurn), 1<<1, self.xRes - 30, self.Y_BOT_TEXT - 30, 0, eFont, eWidGen, 1, 2)

			# Don't draw anything the first turn
			if firstTurn >= lastTurn:
				return

			# Compute max and min
			if self.bGraphLogScale:
				max = 2
				min = 1
			else:
				max = 1
				min = 0

			for p in self.aiPlayersMet:
				for turn in xrange(firstTurn, lastTurn + 1):
					score = self.scoreCache[iGraphID][p][turn - startTurn]
					if max < score:
						max = score
					if min > score:
						min = score

			if self.bGraphLogScale:
				yFactor = 1.0 * iH_GRAPH / (self.getLog10(max) - self.getLog10(min))
			else:
				yFactor = 1.0 * iH_GRAPH / (max - min)

			xFactor = 1.0 * wGraph / (lastTurn - firstTurn)

			x = self.iReferenceDelta
			turnDelta = (lastTurn - firstTurn) / (self.iNumReference + 1)
			iTurn = firstTurn + turnDelta
			for i in xrange(self.iNumReference):
				screen.setLabel(self.getNextWidgetName(), "", self.getDate(iTurn), 1<<2, xGraph + x, self.Y_BOT_TEXT - 30, 0, eFont, eWidGen, 1, 2)
				x += self.iReferenceDelta
				iTurn += turnDelta

			# Draw the lines
			for p in self.aiPlayersMet:

				if not self.bPlayerInclude[p]:
					continue

				color = GC.getPlayerColorInfo(GC.getPlayer(p).getPlayerColor()).getColorTypePrimary()
				oldX = -1
				oldY = iH_GRAPH
				turn = lastTurn

				iSmooth = self.iGraph_Smoothing

				while turn >= firstTurn:

					score = self.scoreCache[iGraphID][p][turn - startTurn]

					if self.bGraphLogScale:
						y = iH_GRAPH - int(yFactor * (self.getLog10(score) - self.getLog10(min)))
					else:
						y = iH_GRAPH - int(yFactor * (score - min))
					x = int(xFactor * (turn - firstTurn))

					if x < oldX - iSmooth:
						if y != iH_GRAPH or oldY != iH_GRAPH:
							screen.addLineGFC(zsGRAPH_CANVAS_ID, self.getNextLineName(), oldX, oldY + 1, x, y + 1, color)
							screen.addLineGFC(zsGRAPH_CANVAS_ID, self.getNextLineName(), oldX - 1, oldY, x - 1, y, color)
							screen.addLineGFC(zsGRAPH_CANVAS_ID, self.getNextLineName(), oldX, oldY, x, y, color)
						oldX = x
						oldY = y
					elif oldX == -1:
						oldX = x
						oldY = y

					turn -= 1

		# Draw Legend
		uFont2b = self.aFontList[5]

		pnl = self.getNextWidgetName()
		screen.addPanel(pnl, "", "", True, True, 16, 74, self.wLegend, self.H_PAGE - 74, PanelStyles.PANEL_STYLE_IN)
		screen.setStyle(pnl, "ScrollPanel_Alt_Style")

		for iPlayerX in self.aiPlayersMet:

			playerX = GC.getPlayer(iPlayerX)
			name = playerX.getName()

			if self.bPlayerInclude[iPlayerX]:
				textColorR = playerX.getPlayerTextColorR()
				textColorG = playerX.getPlayerTextColorG()
				textColorB = playerX.getPlayerTextColorB()
			else:
				textColorR = 175
				textColorG = 175
				textColorB = 175

			txt = "<color=%d,%d,%d>%s</color>" %(textColorR, textColorG, textColorB, name)

			screen.attachTextGFC(pnl, self.sPlayerTextWidget[iPlayerX], uFont2b + txt, eFont, eWidGen, 1, 2)

		screen.attachLabel(pnl, self.getNextWidgetName(), "")
		screen.attachTextGFC(pnl, self.sShowAllWidget, self.SHOW_ALL, eFont, eWidGen, 1, 2)
		screen.attachTextGFC(pnl, self.sShowNoneWidget, self.SHOW_NONE, eFont, eWidGen, 1, 2)
		screen.attachLabel(pnl, self.getNextWidgetName(), "")

		# ADD players where you don't have enough espionage points
		for iPlayerX in self.aiPlayersMetNAEspionage:

			playerX = GC.getPlayer(iPlayerX)
			name = playerX.getName()

			if not playerX.isAlive(): # player is dead!
				textColorR = 175
				textColorG = 175
				textColorB = 175
				name += " [" + self.BUG_LEGEND_DEAD + "]"
			else:
				textColorR = playerX.getPlayerTextColorR()
				textColorG = playerX.getPlayerTextColorG()
				textColorB = playerX.getPlayerTextColorB()
				textColorA = playerX.getPlayerTextColorA()

			txt = "<color=%d,%d,%d>%s</color>" %(textColorR, textColorG, textColorB, name)

			screen.attachLabel(pnl, self.sPlayerTextWidget[iPlayerX], uFont2b + txt)

		if self.bGraphLogScale:
			screen.attachLabel(pnl, self.getNextWidgetName(), "")
			screen.attachLabel(pnl, self.getNextWidgetName(), self.LOG_SCALE)


#############################################################################################################
################################################# DEMOGRAPHICS ##############################################
#############################################################################################################

	def drawDemographicsTab(self):
		iNumActivePlayers = 0

		pPlayer = GC.getPlayer(self.iPlayer)

		iEconomyGameAverage = 0
		iIndustryGameAverage = 0
		iAgricultureGameAverage = 0
		iMilitaryGameAverage = 0
		iLandAreaGameAverage = 0
		iPopulationGameAverage = 0
		iHappinessGameAverage = 0
		iHealthGameAverage = 0
		iNetTradeGameAverage = 0

		# Lists of Player values - will be used to determine rank, strength and average per city
		aiGroupEconomy = []
		aiGroupIndustry = []
		aiGroupAgriculture = []
		aiGroupMilitary = []
		aiGroupLandArea = []
		aiGroupPopulation = []
		aiGroupHappiness = []
		aiGroupHealth = []
		aiGroupNetTrade = []

		# Loop through all players to determine Rank and relative Strength
		for iPlayerLoop in xrange(GC.getMAX_PLAYERS()):

			if (GC.getPlayer(iPlayerLoop).isAlive() and not GC.getPlayer(iPlayerLoop).isNPC()):

				iNumActivePlayers += 1

				pCurrPlayer = GC.getPlayer(iPlayerLoop)

				iValue = pCurrPlayer.calculateTotalCommerce()
				if iPlayerLoop == self.iPlayer:
					iEconomy = iValue
				else:
					iEconomyGameAverage += iValue
				aiGroupEconomy.append((iValue, iPlayerLoop))

				iValue = pCurrPlayer.calculateTotalYield(YieldTypes.YIELD_PRODUCTION)
				if iPlayerLoop == self.iPlayer:
					iIndustry = iValue
				else:
					iIndustryGameAverage += iValue
				aiGroupIndustry.append((iValue, iPlayerLoop))

				iValue = pCurrPlayer.calculateTotalYield(YieldTypes.YIELD_FOOD)
				if iPlayerLoop == self.iPlayer:
					iAgriculture = iValue
				else:
					iAgricultureGameAverage += iValue
				aiGroupAgriculture.append((iValue, iPlayerLoop))

				iValue = pCurrPlayer.getUnitPower() * 1000
				if iPlayerLoop == self.iPlayer:
					iMilitary = iValue
				else:
					iMilitaryGameAverage += iValue
				aiGroupMilitary.append((iValue, iPlayerLoop))

				iValue = pCurrPlayer.getTotalLand() * 1000
				if iPlayerLoop == self.iPlayer:
					iLandArea = iValue
				else:
					iLandAreaGameAverage += iValue
				aiGroupLandArea.append((iValue, iPlayerLoop))

				iValue = pCurrPlayer.getRealPopulation()
				if iPlayerLoop == self.iPlayer:
					iPopulation = iValue
				else:
					iPopulationGameAverage += iValue
				aiGroupPopulation.append((iValue, iPlayerLoop))

				iValue = self.getHappyValue(pCurrPlayer)
				if iPlayerLoop == self.iPlayer:
					iHappiness = iValue
				else:
					iHappinessGameAverage += iValue
				aiGroupHappiness.append((iValue, iPlayerLoop))

				iValue = self.getHealthValue(pCurrPlayer)
				if iPlayerLoop == self.iPlayer:
					iHealth = iValue
				else:
					iHealthGameAverage += iValue
				aiGroupHealth.append((iValue, iPlayerLoop))

				iValue = pCurrPlayer.calculateTotalExports(YieldTypes.YIELD_COMMERCE) - pCurrPlayer.calculateTotalImports(YieldTypes.YIELD_COMMERCE)
				if iPlayerLoop == self.iPlayer:
					iNetTrade = iValue
				else:
					iNetTradeGameAverage += iValue
				aiGroupNetTrade.append((iValue, iPlayerLoop))

		iEconomyRank = self.getRank(aiGroupEconomy)
		iIndustryRank = self.getRank(aiGroupIndustry)
		iAgricultureRank = self.getRank(aiGroupAgriculture)
		iMilitaryRank = self.getRank(aiGroupMilitary)
		iLandAreaRank = self.getRank(aiGroupLandArea)
		iPopulationRank = self.getRank(aiGroupPopulation)
		iHappinessRank = self.getRank(aiGroupHappiness)
		iHealthRank = self.getRank(aiGroupHealth)
		iNetTradeRank = self.getRank(aiGroupNetTrade)

		iEconomyGameBest	= self.getBest(aiGroupEconomy)
		iIndustryGameBest	= self.getBest(aiGroupIndustry)
		iAgricultureGameBest	= self.getBest(aiGroupAgriculture)
		iMilitaryGameBest	= self.getBest(aiGroupMilitary)
		iLandAreaGameBest	= self.getBest(aiGroupLandArea)
		iPopulationGameBest	= self.getBest(aiGroupPopulation)
		iHappinessGameBest	= self.getBest(aiGroupHappiness)
		iHealthGameBest		= self.getBest(aiGroupHealth)
		iNetTradeGameBest	= self.getBest(aiGroupNetTrade)

		iEconomyGameWorst	= self.getWorst(aiGroupEconomy)
		iIndustryGameWorst	= self.getWorst(aiGroupIndustry)
		iAgricultureGameWorst	= self.getWorst(aiGroupAgriculture)
		iMilitaryGameWorst	= self.getWorst(aiGroupMilitary)
		iLandAreaGameWorst	= self.getWorst(aiGroupLandArea)
		iPopulationGameWorst	= self.getWorst(aiGroupPopulation)
		iHappinessGameWorst	= self.getWorst(aiGroupHappiness)
		iHealthGameWorst	= self.getWorst(aiGroupHealth)
		iNetTradeGameWorst	= self.getWorst(aiGroupNetTrade)

		iEconomyGameAverage = iEconomyGameAverage / max(1, iNumActivePlayers - 1)
		iIndustryGameAverage = iIndustryGameAverage / max(1, iNumActivePlayers - 1)
		iAgricultureGameAverage = iAgricultureGameAverage / max(1, iNumActivePlayers - 1)
		iMilitaryGameAverage = iMilitaryGameAverage / max(1, iNumActivePlayers - 1)
		iLandAreaGameAverage = iLandAreaGameAverage / max(1, iNumActivePlayers - 1)
		iPopulationGameAverage = iPopulationGameAverage / max(1, iNumActivePlayers - 1)
		iHappinessGameAverage = iHappinessGameAverage / max(1, iNumActivePlayers - 1)
		iHealthGameAverage = iHealthGameAverage / max(1, iNumActivePlayers - 1)
		iNetTradeGameAverage = iNetTradeGameAverage / max(1, iNumActivePlayers - 1)


		######## TEXT ########

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		# Create Table
		szTable = self.getNextWidgetName()
		screen.addTableControlGFC(szTable, 6, 45, 80, 934, 600, True, True, 32,32, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader(szTable, 0, self.TEXT_DEMOGRAPHICS_SMALL, 224) # Total graph width is 430
		screen.setTableColumnHeader(szTable, 1, self.TEXT_VALUE, 155)
		screen.setTableColumnHeader(szTable, 2, self.TEXT_BEST, 155)
		screen.setTableColumnHeader(szTable, 3, self.TEXT_AVERAGE, 155)
		screen.setTableColumnHeader(szTable, 4, self.TEXT_WORST, 155)
		screen.setTableColumnHeader(szTable, 5, self.TEXT_RANK, 90)

		for i in xrange(18 + 5): # 18 normal items + 5 lines for spacing
			screen.appendTableRow(szTable)
		iNumRows = screen.getTableNumRows(szTable)
		iRow = iNumRows - 1
		iCol = 0
		screen.setTableText(szTable, iCol, 0, self.TEXT_ECONOMY, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 1, self.TEXT_ECONOMY_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 3, self.TEXT_INDUSTRY, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 4, self.TEXT_INDUSTRY_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 6, self.TEXT_AGRICULTURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 7, self.TEXT_AGRICULTURE_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 9, self.TEXT_MILITARY, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 11, self.TEXT_LAND_AREA, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 12, self.TEXT_LAND_AREA_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 14, self.TEXT_POPULATION, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 16, self.TEXT_HAPPINESS, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 18, self.TEXT_HEALTH, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 19, self.TEXT_HEALTH_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 21, self.TEXT_IMP_EXP, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 22, self.TEXT_IMP_EXP_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		iCol = 1
		screen.setTableText(szTable, iCol, 0, str(iEconomy), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 3, str(iIndustry), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 6, str(iAgriculture), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 9, str(iMilitary), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 11, str(iLandArea), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 14, str(iPopulation), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 16, str(iHappiness) + self.TEXT_HAPPINESS_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 18, str(iHealth), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 21, str(iNetTrade), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		iCol = 2
		screen.setTableText(szTable, iCol, 0, str(iEconomyGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 3, str(iIndustryGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 6, str(iAgricultureGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 9, str(iMilitaryGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 11, str(iLandAreaGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 14, str(iPopulationGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 16, str(iHappinessGameBest) + self.TEXT_HAPPINESS_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 18, str(iHealthGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 21, str(iNetTradeGameBest), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		iCol = 3
		screen.setTableText(szTable, iCol, 0, str(iEconomyGameAverage), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 3, str(iIndustryGameAverage), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 6, str(iAgricultureGameAverage), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 9, str(iMilitaryGameAverage), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 11, str(iLandAreaGameAverage), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 14, str(iPopulationGameAverage), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 16, str(iHappinessGameAverage) + self.TEXT_HAPPINESS_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 18, str(iHealthGameAverage), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 21, str(iNetTradeGameAverage), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		iCol = 4
		screen.setTableText(szTable, iCol, 0, str(iEconomyGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 3, str(iIndustryGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 6, str(iAgricultureGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 9, str(iMilitaryGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 11, str(iLandAreaGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 14, str(iPopulationGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 16, str(iHappinessGameWorst) + self.TEXT_HAPPINESS_MEASURE, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 18, str(iHealthGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 21, str(iNetTradeGameWorst), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		iCol = 5
		screen.setTableText(szTable, iCol, 0, str(iEconomyRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 3, str(iIndustryRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 6, str(iAgricultureRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 9, str(iMilitaryRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 11, str(iLandAreaRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 14, str(iPopulationRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 16, str(iHappinessRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 18, str(iHealthRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(szTable, iCol, 21, str(iNetTradeRank), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)


	def getHappyValue(self, pPlayer):
		iHappy = pPlayer.calculateTotalCityHappiness()
		iUnhappy = pPlayer.calculateTotalCityUnhappiness()
		return (iHappy * 100) / max(1, iHappy + iUnhappy)

	def getHealthValue(self, pPlayer):
		iGood = pPlayer.calculateTotalCityHealthiness()
		iBad = pPlayer.calculateTotalCityUnhealthiness()
		return (iGood * 100) / max(1, iGood + iBad)

	def getRank(self, aiGroup):
		aiGroup.sort()
		aiGroup.reverse()
		iRank = 1
		for (iLoopValue, iLoopPlayer) in aiGroup:
			if iLoopPlayer == self.iPlayer:
				return iRank
			iRank += 1
		return 0

	def getBest(self, aiGroup):
		bFirst = True
		iBest = 0
		for (iLoopValue, iLoopPlayer) in aiGroup:
			if iLoopPlayer != self.iPlayer:
				if bFirst or iLoopValue > iBest:
					iBest = iLoopValue
					bFirst = False
		return iBest

	def getWorst(self, aiGroup):
		bFirst = True
		iWorst = 0
		for (iLoopValue, iLoopPlayer) in aiGroup:
			if iLoopPlayer != self.iPlayer:
				if bFirst or iLoopValue < iWorst:
					iWorst = iLoopValue
					bFirst = False
		return iWorst


#############################################################################################################
################################################## TOP CITIES ###############################################
#############################################################################################################

	def drawTopCitiesTab(self):

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		# Background Panes
		screen.addPanel(self.getNextWidgetName(), "", "", True, True, 45, 70, 470, 620, PanelStyles.PANEL_STYLE_MAIN)

		# Calculate top 5 cities
		for i in xrange(GC.getMAX_PC_PLAYERS()):

			for cityX in GC.getPlayer(i).cities():

				iTotalCityValue = (
					(
						cityX.getCulture(i) / 5 +
						cityX.getYieldRate(YieldTypes.YIELD_FOOD) +
						cityX.getYieldRate(YieldTypes.YIELD_PRODUCTION) +
						cityX.getYieldRate(YieldTypes.YIELD_COMMERCE)
					) * cityX.getPopulation()
				)
				for iRankLoop in xrange(5):

					if iTotalCityValue > self.iCityValues[iRankLoop]:

						self.addCityToList(iRankLoop, cityX, iTotalCityValue)
						break

		# Determine City Data
		iNumCities = 0
		szCityNames = ["", "", "", "", ""]
		szCityDescs = ["", "", "", "", ""]

		for iRank in xrange(5):

			pCity = self.pCityPointers[iRank]

			# If this city exists and has data we can use
			if pCity:

				pPlayer = GC.getPlayer(pCity.getOwner())

				iTurnYear = GAME.getTurnYear(pCity.getGameTurnFounded())

				if iTurnYear < 0:
					szTurnFounded = TRNSLTR.getText("TXT_KEY_TIME_BC", (-iTurnYear,))
				else:
					szTurnFounded = TRNSLTR.getText("TXT_KEY_TIME_AD", (iTurnYear,))

				if pCity.isRevealed(self.iTeam, False) or self.team.isHasMet(pPlayer.getTeam()):
					szCityNames[iRank] = pCity.getName().upper()
					szCityDescs[iRank] = "%s, %s" % (pPlayer.getCivilizationAdjective(0), TRNSLTR.getText("TXT_KEY_MISC_FOUNDED_IN", (szTurnFounded,)))
				else:
					szCityNames[iRank] = TRNSLTR.getText("TXT_KEY_UNKNOWN", ()).upper()
					szCityDescs[iRank] = TRNSLTR.getText("TXT_KEY_MISC_FOUNDED_IN", (szTurnFounded,))

				self.iCitySizes[iRank] = pCity.getPopulation()
				self.aaCitiesXY[iRank] = [pCity.getX(), pCity.getY()]

				iNumCities += 1
			else:
				self.iCitySizes[iRank] = -1
				self.aaCitiesXY[iRank] = [-1, -1]

		for iWidgetLoop in xrange(iNumCities):

			screen.addPanel(self.getNextWidgetName(), "", "", False, True, 225, self.Y_ROWS_CITIES[iWidgetLoop] - 4, 275, 60, PanelStyles.PANEL_STYLE_DAWNTOP)
			szCityDesc = u"<font=4b>" + str(self.iCitySizes[iWidgetLoop]) + u"</font>" + " - " + u"<font=3b>" + szCityNames[iWidgetLoop] + u"</font>" + "\n" + szCityDescs[iWidgetLoop]
			screen.addMultilineText(self.getNextWidgetName(), szCityDesc, 231, self.Y_ROWS_CITIES[iWidgetLoop] - 1, 269, 54, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

			pPlot = CyMap().plot(self.aaCitiesXY[iWidgetLoop][0], self.aaCitiesXY[iWidgetLoop][1])
			pCity = pPlot.getPlotCity()

			iDistance = 200 + pCity.getPopulation() * 5
			if iDistance > 400:
				iDistance = 400

			if pCity.isRevealed(self.iTeam, False):
				screen.addPlotGraphicGFC(self.getNextWidgetName(), 65, self.Y_ROWS_CITIES[iWidgetLoop], 150, 110, pPlot, iDistance, False, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Draw Wonder icons
		aWonderList = []
		# Make a wonder list
		for i in xrange(GC.getNumBuildingInfos()):
			if isWorldWonder(i):
				aWonderList.append(i)

		aaiTopCitiesWonders = []
		aiTopCitiesNumWonders = []
		# Loop through top cities and determine if they have any wonders to display
		for i in xrange(iNumCities):
			aaiTopCitiesWonders.append(0)
			aiTopCitiesNumWonders.append(0)
			pCity = self.pCityPointers[i]

			if pCity:
				aiTempWondersList = []
				# Loop through world wonders
				for iWW in aWonderList:

					if pCity.getNumBuilding(iWW) > 0:

						aiTempWondersList.append(iWW)
						aiTopCitiesNumWonders[i] += 1

				aaiTopCitiesWonders[i] = aiTempWondersList

		# Create Scrollable areas under each city
		self.szCityWonderScrollArea = []
		for i in xrange(iNumCities):

			szIconPanel = self.getNextWidgetName()
			self.szCityWonderScrollArea.append(szIconPanel)

			screen.addPanel(szIconPanel, "", "", False, True, 225, self.Y_ROWS_CITIES[i] + 53, 275, 60, PanelStyles.PANEL_STYLE_DAWNTOP)

			# Now place the wonder buttons
			for iWonderLoop in xrange(aiTopCitiesNumWonders[i]):

				iBuildingID = aaiTopCitiesWonders[i][iWonderLoop]
				screen.attachImageButton(szIconPanel, "", GC.getBuildingInfo(iBuildingID).getButton(), GenericButtonSizes.BUTTON_SIZE_46, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iBuildingID, -1, False)


		xDD = 540
		wDD = 420

		screen.addPanel(self.getNextWidgetName(), "", "", True, True, 520, 70, 460, 620, PanelStyles.PANEL_STYLE_MAIN)

		# Draw the Wonders Dropdown Box"

		if self.szWonderDisplayMode == "WorldWonders":
			sWW = self.BUGWorldWonder_On
			sNW = self.BUGNatWonder_Off
			sPj = self.BUGProject_Off
			sDesc = TRNSLTR.getText("TXT_KEY_TOP_CITIES_SCREEN_WORLD_WONDERS", ())
		elif self.szWonderDisplayMode == "NationalWonders":
			sWW = self.BUGWorldWonder_Off
			sNW = self.BUGNatWonder_On
			sPj = self.BUGProject_Off
			sDesc = TRNSLTR.getText("TXT_KEY_TOP_CITIES_SCREEN_NATIONAL_WONDERS", ())
		else:
			sWW = self.BUGWorldWonder_Off
			sNW = self.BUGNatWonder_Off
			sPj = self.BUGProject_On
			sDesc = TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_PROJECT", ())

		screen.setImageButton("WorldWondersWidget", sWW,  xDD +  0, 90, 24, 24, WidgetTypes.WIDGET_INFO_WORLD_WONDERS, -1, -1)
		screen.setImageButton("NationalWondersWidget", sNW,  xDD + 30, 90, 24, 24, WidgetTypes.WIDGET_INFO_NATIONAL_WONDERS, -1, -1)
		screen.setImageButton("ProjectsWidget", sPj,  xDD + 60, 90, 24, 24, WidgetTypes.WIDGET_INFO_PROJECTS, -1, -1)
		self.aWidgetBucket.append("WorldWondersWidget")
		self.aWidgetBucket.append("NationalWondersWidget")
		self.aWidgetBucket.append("ProjectsWidget")

		screen.setLabel(self.getNextWidgetName(), "", "<font=4>" + sDesc, 1<<0, xDD + 100, 93, -6.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)

		## Consolidate wonders built
		aaWondersBeingBuilt = []
		aaWondersBuilt = []
		iNumWonders = 0

		# Loop through players to determine Wonders
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):

			playerX = GC.getPlayer(iPlayerX)

			if not playerX.isAlive():
				continue
			iTeamX = playerX.getTeam()

			# Loop through this player's cities and determine if they have any wonders to display
			for cityX in GC.getPlayer(iPlayerX).cities():

				# Loop through projects to find any under construction
				if self.szWonderDisplayMode == "Projects":

					for iProjectLoop in xrange(GC.getNumProjectInfos()):

						iProjectProd = cityX.getProductionProject()

						# Project is being constructed
						if iProjectProd == iProjectLoop:
							if iTeamX == self.iTeam:
								aaWondersBeingBuilt.append([iProjectLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

							if (self.team.isHasMet(iTeamX)
							and self.player.canDoEspionageMission(self.iInvestigateCityMission, cityX.getOwner(), cityX.plot(), -1)
							and cityX.isRevealed(self.iTeam, False)
							):
								aaWondersBeingBuilt.append([iProjectLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

				# Loop through buildings
				else:
					for iBuildingLoop in xrange(GC.getNumBuildingInfos()):
						iBuildingProd = cityX.getProductionBuilding()

						# World Wonder Mode
						if (self.szWonderDisplayMode == "WorldWonders" and isWorldWonder(iBuildingLoop)):
							# Is this city building a wonder?
							if (iBuildingProd == iBuildingLoop):
								if (iTeamX == self.iTeam):
									aaWondersBeingBuilt.append([iBuildingLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

								if (self.team.isHasMet(iTeamX)
								and self.player.canDoEspionageMission(self.iInvestigateCityMission, cityX.getOwner(), cityX.plot(), -1)
								and cityX.isRevealed(self.iTeam, False)):
									aaWondersBeingBuilt.append([iBuildingLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

							if (cityX.getNumBuilding(iBuildingLoop) > 0):
								if (iTeamX == self.iTeam or self.team.isHasMet(iTeamX)):
									aaWondersBuilt.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,True, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])
								else:
									aaWondersBuilt.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,False,TRNSLTR.getText("TXT_KEY_UNKNOWN", ()), cityX, 18])

								iNumWonders += 1

						# National/Team Wonder Mode
						elif self.szWonderDisplayMode == "NationalWonders" and (isNationalWonder(iBuildingLoop) or isTeamWonder(iBuildingLoop)):

							# Is this city building a wonder?
							if (iBuildingProd == iBuildingLoop):
								# Only show our wonders under construction
								if iTeamX == self.iTeam:
									aaWondersBeingBuilt.append([iBuildingLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

								if (self.team.isHasMet(iTeamX)
								and self.player.canDoEspionageMission(self.iInvestigateCityMission, cityX.getOwner(), cityX.plot(), -1)
								and cityX.isRevealed(self.iTeam, False)):
									aaWondersBeingBuilt.append([iBuildingLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

							# Has this city built a wonder?
							if (cityX.getNumBuilding(iBuildingLoop) > 0):
								if (iTeamX == self.iTeam):
									aaWondersBuilt.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,True, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])
									iNumWonders += 1

								elif self.team.isHasMet(iTeamX) and cityX.isRevealed(self.iTeam, False):

									aaWondersBuilt.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,True, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])
									iNumWonders += 1

		# This array used to store which players have already used up a team's slot so team projects don't get added to list more than once
		aiTeamsUsed = []

		# Project Mode
		if self.szWonderDisplayMode == "Projects":

			# Loop through players to determine Projects
			for iPlayerLoop in xrange(GC.getMAX_PC_PLAYERS()):
				iTeamLoop = GC.getPlayer(iPlayerLoop).getTeam()
				# Block duplicates
				if iTeamLoop in aiTeamsUsed:
					continue

				aiTeamsUsed.append(iTeamLoop)
				pTeam = GC.getTeam(iTeamLoop)

				if pTeam.isAlive():

					# Loop through projects
					for iProjectLoop in xrange(GC.getNumProjectInfos()):

						for iI in xrange(pTeam.getProjectCount(iProjectLoop)):

							if (iTeamLoop == self.iTeam or self.team.isHasMet(iTeamLoop)):
								aaWondersBuilt.append([-9999,iProjectLoop,True,GC.getPlayer(iPlayerLoop).getCivilizationShortDescription(0),None, iPlayerLoop])
							else:
								aaWondersBuilt.append([-9999,iProjectLoop,False,TRNSLTR.getText("TXT_KEY_UNKNOWN", ()),None, 9999])
							iNumWonders += 1

		# Sort wonders in order of date built
		aaWondersBuilt.sort()
		aaWondersBuilt.reverse()

		# Draw Wonders List
		self.szWondersTable = self.getNextWidgetName()

		screen.addTableControlGFC(self.szWondersTable, 5, 540, 130, 420, 540, True, True, 24,24, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSort(self.szWondersTable)
		screen.setTableColumnHeader(self.szWondersTable, 0, "", 30)
		screen.setTableColumnHeader(self.szWondersTable, 1, self.szName, 115)
		screen.setTableColumnHeader(self.szWondersTable, 2, self.szWonderDate, 70)
		screen.setTableColumnHeader(self.szWondersTable, 3, self.szWonderOwner, 100)
		screen.setTableColumnHeader(self.szWondersTable, 4, self.szWonderCity, 100)

		zoomArt = self.zoomArt
		iWBB = len(aaWondersBeingBuilt)

		for i in xrange(iWBB):

			# aaWondersBeingBuilt contains the following:
			iWonderType = aaWondersBeingBuilt[i][0]
			szWonderBuiltBy = aaWondersBeingBuilt[i][1]
			pCity = aaWondersBeingBuilt[i][2]
			iPlayer = aaWondersBeingBuilt[i][3]

			color = -1
			ePlayerColor = GC.getPlayer(iPlayer).getPlayerColor()
			if ePlayerColor != -1:
				playerColor = GC.getPlayerColorInfo(ePlayerColor)
				if playerColor:
					color = playerColor.getColorTypePrimary()

			if self.szWonderDisplayMode == "Projects":
				pWonderInfo = GC.getProjectInfo(iWonderType)
				iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT
			else:
				pWonderInfo = GC.getBuildingInfo(iWonderType)
				iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING

			szWonderName = pWonderInfo.getDescription()
			szTurnYearBuilt = u"<font=2>%c</font>" % GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar()

			# Check to see if active player can see this city
			if pCity and pCity.isRevealed(self.iTeam, False):
				szCityName = pCity.getName()
			else:
				szCityName = ""

			if self.bWonderPlayerColor:
				szWonderBuiltBy = TRNSLTR.changeTextColor(szWonderBuiltBy, color)
				szCityName = TRNSLTR.changeTextColor(szCityName, color)

			screen.appendTableRow(self.szWondersTable)
			screen.setTableText(self.szWondersTable, 0, i, ""             , zoomArt, WidgetTypes.WIDGET_ZOOM_CITY, pCity.getOwner(), pCity.getID(), 1<<0)
			screen.setTableText(self.szWondersTable, 1, i, szWonderName   , "", iWidget, iWonderType, -1, 1<<0)
			screen.setTableInt (self.szWondersTable, 2, i, szTurnYearBuilt, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)
			screen.setTableText(self.szWondersTable, 3, i, szWonderBuiltBy, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
			screen.setTableText(self.szWondersTable, 4, i, szCityName     , "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		for i in xrange(iNumWonders):

			# aaWondersBuilt contains the following:
			iTurnYearBuilt = aaWondersBuilt[i][0]
			iWonderType = aaWondersBuilt[i][1]
			bKnown = aaWondersBuilt[i][2]
			szWonderBuiltBy = aaWondersBuilt[i][3]
			pCity = aaWondersBuilt[i][4]
			iPlayer = aaWondersBuilt[i][5]

			color = -1
			ePlayerColor = GC.getPlayer(iPlayer).getPlayerColor()
			if ePlayerColor != -1:
				playerColor = GC.getPlayerColorInfo(ePlayerColor)
				if playerColor:
					color = playerColor.getColorTypePrimary()

			if self.szWonderDisplayMode == "Projects":
				pWonderInfo = GC.getProjectInfo(iWonderType)
				iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT
			else:
				pWonderInfo = GC.getBuildingInfo(iWonderType)
				iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING
			szWonderName = pWonderInfo.getDescription()

			if iTurnYearBuilt == -9999:
				szTurnYearBuilt = ""

			elif iTurnYearBuilt < 0:
				szTurnYearBuilt = TRNSLTR.getText("TXT_KEY_TIME_BC", (-iTurnYearBuilt,))
			else:
				szTurnYearBuilt = TRNSLTR.getText("TXT_KEY_TIME_AD", (iTurnYearBuilt,))

			# Check to see if active player can see this city
			if pCity and pCity.isRevealed(self.iTeam, False):
				szCityName = pCity.getName()
			else:
				szCityName = ""

			if self.bWonderPlayerColor:
				szWonderBuiltBy = TRNSLTR.changeTextColor(szWonderBuiltBy, color)
				szCityName = TRNSLTR.changeTextColor(szCityName, color)

			screen.appendTableRow(self.szWondersTable)
			if bKnown and pCity and pCity.isRevealed(self.iTeam, False):
				screen.setTableText(self.szWondersTable, 0, i+iWBB, "", zoomArt, WidgetTypes.WIDGET_ZOOM_CITY, pCity.getOwner(), pCity.getID(), 1<<0)
			screen.setTableText(self.szWondersTable, 1, i+iWBB, szWonderName   , "", iWidget, iWonderType, -1, 1<<0)
			screen.setTableInt (self.szWondersTable, 2, i+iWBB, szTurnYearBuilt, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<1)
			screen.setTableText(self.szWondersTable, 3, i+iWBB, szWonderBuiltBy, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
			screen.setTableText(self.szWondersTable, 4, i+iWBB, szCityName     , "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)


	# Recursive
	def addCityToList(self, iRank, pCity, iTotalCityValue):
		if iRank > 4:
			return

		pTempCity = self.pCityPointers[iRank]

		# Verify a city actually exists at this rank
		if not pTempCity:
			self.pCityPointers[iRank] = pCity
			self.iCityValues[iRank] = iTotalCityValue
			return

		iTempCityValue = self.iCityValues[iRank]

		self.addCityToList(iRank+1, pTempCity, iTempCityValue)
		self.pCityPointers[iRank] = pCity
		self.iCityValues[iRank] = iTotalCityValue


#############################################################################################################
################################################## STATISTICS ###############################################
#############################################################################################################

	def drawStatsTab(self):

		# Bottom Chart
		if self.bShowImprovement:
			self.W_STATS_BOTTOM_CHART_UNITS = 455
			self.W_STATS_BOTTOM_CHART_BUILDINGS = 260
		else:
			self.W_STATS_BOTTOM_CHART_UNITS = 545
			self.W_STATS_BOTTOM_CHART_BUILDINGS = 390

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		iNumUnits = GC.getNumUnitInfos()
		iNumBuildings = GC.getNumBuildingInfos()
		iNumImprovements = GC.getNumImprovementInfos()

################################################### CALCULATE STATS ###################################################

		iMinutesPlayed = GAME.getMinutesPlayed()
		iHoursPlayed = iMinutesPlayed / 60
		iMinutesPlayed = iMinutesPlayed - iHoursPlayed * 60

		szMinutesString = str(iMinutesPlayed)
		if iMinutesPlayed < 10:
			szMinutesString = "0" + szMinutesString
		szHoursString = str(iHoursPlayed)
		if iHoursPlayed < 10:
			szHoursString = "0" + szHoursString

		szTimeString = szHoursString + ":" + szMinutesString

		iNumCitiesBuilt = CyStatistics().getPlayerNumCitiesBuilt(self.iPlayer)

		iNumCitiesRazed = CyStatistics().getPlayerNumCitiesRazed(self.iPlayer)

		iNumReligionsFounded = 0
		for iReligionLoop in xrange(GC.getNumReligionInfos()):
			if (CyStatistics().getPlayerReligionFounded(self.iPlayer, iReligionLoop)):
				iNumReligionsFounded += 1

		aiUnitsBuilt = []
		for iUnitLoop in xrange(iNumUnits):
			aiUnitsBuilt.append(CyStatistics().getPlayerNumUnitsBuilt(self.iPlayer, iUnitLoop))

		aiUnitsKilled = []
		for iUnitLoop in xrange(iNumUnits):
			aiUnitsKilled.append(CyStatistics().getPlayerNumUnitsKilled(self.iPlayer, iUnitLoop))

		aiUnitsLost = []
		for iUnitLoop in xrange(iNumUnits):
			aiUnitsLost.append(CyStatistics().getPlayerNumUnitsLost(self.iPlayer, iUnitLoop))

		aiBuildingsBuilt = []
		for iBuildingLoop in xrange(iNumBuildings):
			aiBuildingsBuilt.append(CyStatistics().getPlayerNumBuildingsBuilt(self.iPlayer, iBuildingLoop))

		aiUnitsCurrent = []
		for iUnitLoop in xrange(iNumUnits):
			aiUnitsCurrent.append(0)

		player = GC.getPlayer(self.iPlayer)
		for unitX in player.units():
			iType = unitX.getUnitType()
			aiUnitsCurrent[iType] += 1

		aiImprovementsCurrent = []
		for iImprovementLoop in xrange(iNumImprovements):
			aiImprovementsCurrent.append(0)

		iGridW = CyMap().getGridWidth()
		iGridH = CyMap().getGridHeight()
		for iX in xrange(iGridW):
			for iY in xrange(iGridH):
				plot = CyMap().plot(iX, iY)
				if (plot.getOwner() == self.iPlayer):
					iType = plot.getImprovementType()
					if (iType != ImprovementTypes.NO_IMPROVEMENT):
						aiImprovementsCurrent[iType] += 1

################################################### TOP PANEL ###################################################

		eWidGen = WidgetTypes.WIDGET_GENERAL

		# Add Panel
		screen.addPanel(self.getNextWidgetName(), "", "", True, False, 45, 75, 935, 180, PanelStyles.PANEL_STYLE_DAWNTOP)

		# Leaderhead graphic
		screen.addLeaderheadGFC(self.getNextWidgetName(), player.getLeaderType(), AttitudeTypes.ATTITUDE_PLEASED, 250, 95, 110, 140, eWidGen, 1, 2)

		# Leader Name
		screen.setText(self.getNextWidgetName(), "", "<font=4b>" + player.getName(), 1<<0, 400, 90, 0, FontTypes.TITLE_FONT, eWidGen, 1, 2)

		# Create Table
		szTopChart = self.getNextWidgetName()
		screen.addTableControlGFC(szTopChart, 2, 400, 130, 380, 102, False, True, 32, 32, TableStyles.TABLE_STYLE_STANDARD)

		# Add Columns
		screen.setTableColumnHeader(szTopChart, 0, "", 304)
		screen.setTableColumnHeader(szTopChart, 1, "", 76)

		# Add Rows
		for i in xrange(3):
			screen.appendTableRow(szTopChart)
		iNumRows = screen.getTableNumRows(szTopChart)

		# Graph itself
		iRow = 0
		iCol = 0
		screen.setTableText(szTopChart, iCol, iRow, self.TEXT_TIME_PLAYED, "", eWidGen, -1, -1, 1<<0)
		iCol = 1
		screen.setTableText(szTopChart, iCol, iRow, szTimeString, "", eWidGen, -1, -1, 1<<0)

		iRow = 1
		iCol = 0
		screen.setTableText(szTopChart, iCol, iRow, self.TEXT_CITIES_BUILT, "", eWidGen, -1, -1, 1<<0)
		iCol = 1
		screen.setTableText(szTopChart, iCol, iRow, str(iNumCitiesBuilt), "", eWidGen, -1, -1, 1<<0)

		iRow = 2
		iCol = 0
		screen.setTableText(szTopChart, iCol, iRow, self.TEXT_CITIES_RAZED, "", eWidGen, -1, -1, 1<<0)
		iCol = 1
		screen.setTableText(szTopChart, iCol, iRow, str(iNumCitiesRazed), "", eWidGen, -1, -1, 1<<0)

		iRow = 3
		iCol = 0
		screen.setTableText(szTopChart, iCol, iRow, self.TEXT_NUM_RELIGIONS_FOUNDED, "", eWidGen, -1, -1, 1<<0)
		iCol = 1
		screen.setTableText(szTopChart, iCol, iRow, str(iNumReligionsFounded), "", eWidGen, -1, -1, 1<<0)

################################################### BOTTOM PANEL ###################################################

		# Create Tables
		szUnitsTable = self.getNextWidgetName()
		screen.addTableControlGFC(szUnitsTable, 5, 45, 280, self.W_STATS_BOTTOM_CHART_UNITS, 410, True, True, 32, 32, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSort(szUnitsTable)

		szBuildingsTable = self.getNextWidgetName()
		screen.addTableControlGFC(szBuildingsTable, 2, 45 + self.W_STATS_BOTTOM_CHART_UNITS, 280, self.W_STATS_BOTTOM_CHART_BUILDINGS, 410, True, True, 32, 32, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSort(szBuildingsTable)

		# Reducing the width a bit to leave room for the vertical scrollbar, preventing a horizontal scrollbar from also being created
		if self.bShowImprovement:
			szImprovementsTable = self.getNextWidgetName()
			screen.addTableControlGFC(szImprovementsTable, 2, 45 + self.W_STATS_BOTTOM_CHART_UNITS + self.W_STATS_BOTTOM_CHART_BUILDINGS, 280, 220, 410, True, True, 32,32, TableStyles.TABLE_STYLE_STANDARD)
			screen.enableSort(szImprovementsTable)

			iChartWidth = self.W_STATS_BOTTOM_CHART_UNITS + self.W_STATS_BOTTOM_CHART_BUILDINGS + 196

			# Add Columns
			iColWidth = int((iChartWidth / 16 * 3))
			screen.setTableColumnHeader(szUnitsTable, 0, self.TEXT_UNITS, iColWidth)
			iColWidth = int((iChartWidth / 14 * 1))
			screen.setTableColumnHeader(szUnitsTable, 1, self.TEXT_CURRENT, iColWidth)
			iColWidth = int((iChartWidth / 14 * 1))
			screen.setTableColumnHeader(szUnitsTable, 2, self.TEXT_BUILT, iColWidth)
			iColWidth = int((iChartWidth / 14 * 1))
			screen.setTableColumnHeader(szUnitsTable, 3, self.TEXT_KILLED, iColWidth)
			iColWidth = int((iChartWidth / 14 * 1))
			screen.setTableColumnHeader(szUnitsTable, 4, self.TEXT_LOST, iColWidth)
			iColWidth = int((iChartWidth / 16 * 3))
			screen.setTableColumnHeader(szBuildingsTable, 0, self.TEXT_BUILDINGS, iColWidth)
			iColWidth = int((iChartWidth / 14 * 1))
			screen.setTableColumnHeader(szBuildingsTable, 1, self.TEXT_BUILT, iColWidth)
			iColWidth = int((iChartWidth / 14 * 2))
			screen.setTableColumnHeader(szImprovementsTable, 0, self.TEXT_IMPROVEMENTS, iColWidth)
			iColWidth = int((iChartWidth / 14 * 1))
			screen.setTableColumnHeader(szImprovementsTable, 1, self.TEXT_CURRENT, iColWidth)
		else:
			iChartWidth = self.W_STATS_BOTTOM_CHART_UNITS + self.W_STATS_BOTTOM_CHART_BUILDINGS - 24

			# Add Columns
			iColWidth = int((iChartWidth / 12 * 3))
			screen.setTableColumnHeader(szUnitsTable, 0, self.TEXT_UNITS, iColWidth)
			iColWidth = int((iChartWidth / 12 * 1))
			screen.setTableColumnHeader(szUnitsTable, 1, self.TEXT_CURRENT, iColWidth)
			iColWidth = int((iChartWidth / 12 * 1))
			screen.setTableColumnHeader(szUnitsTable, 2, self.TEXT_BUILT, iColWidth)
			iColWidth = int((iChartWidth / 12 * 1))
			screen.setTableColumnHeader(szUnitsTable, 3, self.TEXT_KILLED, iColWidth)
			iColWidth = int((iChartWidth / 12 * 1))
			screen.setTableColumnHeader(szUnitsTable, 4, self.TEXT_LOST, iColWidth)
			iColWidth = int((iChartWidth / 12 * 4))
			screen.setTableColumnHeader(szBuildingsTable, 0, self.TEXT_BUILDINGS, iColWidth)
			iColWidth = int((iChartWidth / 12 * 1))
			screen.setTableColumnHeader(szBuildingsTable, 1, self.TEXT_BUILT, iColWidth)


		# Add Rows
		for i in xrange(iNumUnits):
			screen.appendTableRow(szUnitsTable)
		iNumUnitRows = screen.getTableNumRows(szUnitsTable)

		for i in xrange(iNumBuildings):
			screen.appendTableRow(szBuildingsTable)
		iNumBuildingRows = screen.getTableNumRows(szBuildingsTable)


		if self.bShowImprovement:
			for i in xrange(iNumImprovements):
				if aiImprovementsCurrent[i] > 0:
					screen.appendTableRow(szImprovementsTable)
			iNumImprovementRows = screen.getTableNumRows(szImprovementsTable)

		# Add Units to table
		for i in xrange(iNumUnits):

			screen.setTableText(szUnitsTable, 0, i, GC.getUnitInfo(i).getDescription(), "", eWidGen, 1, 2, 1<<0)
			screen.setTableInt(szUnitsTable, 1, i, str(aiUnitsCurrent[i]), "", eWidGen, 1, 2, 1<<0)
			screen.setTableInt(szUnitsTable, 2, i, str(aiUnitsBuilt[i]), "", eWidGen, 1, 2, 1<<0)
			screen.setTableInt(szUnitsTable, 3, i, str(aiUnitsKilled[i]), "", eWidGen, 1, 2, 1<<0)
			screen.setTableInt(szUnitsTable, 4, i, str(aiUnitsLost[i]), "", eWidGen, 1, 2, 1<<0)

		# Add Buildings to table
		for i in xrange(iNumBuildings):
			screen.setTableText(szBuildingsTable, 0, i, GC.getBuildingInfo(i).getDescription(), "", eWidGen, 1, 2, 1<<0)
			screen.setTableInt(szBuildingsTable, 1, i, str(aiBuildingsBuilt[i]), "", eWidGen, 1, 2, 1<<0)

		if self.bShowImprovement:
			# Add Improvements to table
			iRow = 0
			for i in xrange(iNumImprovements):
				iNumImprovementsCurrent = aiImprovementsCurrent[i]
				if iNumImprovementsCurrent > 0:
					screen.setTableText(szImprovementsTable, 0, iRow, GC.getImprovementInfo(i).getDescription(), "", eWidGen, 1, 2, 1<<0)
					screen.setTableInt(szImprovementsTable, 1, iRow, str(iNumImprovementsCurrent), "", eWidGen, 1, 2, 1<<0)
					iRow += 1


#############################################################################################################
##################################################### OTHER #################################################
#############################################################################################################


	def determineKnownPlayers(self, iEndGame=0):

		self.player = player = GC.getPlayer(self.iPlayer)
		self.iTeam = iTeam = player.getTeam()
		self.team = team = GC.getTeam(iTeam)
		# Determine who this active player knows
		self.aiPlayersMet = []
		self.aiPlayersMetNAEspionage = []
		self.iNumPlayersMet = 0
		self.iNumPlayersMetNAEspionage = 0
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			playerX = GC.getPlayer(iPlayerX)
			iTeamX = playerX.getTeam()
			if playerX.isEverAlive() and (team.isHasMet(iTeamX) or self.bDebug or iEndGame):

				if iEndGame or iTeamX == iTeam or player.canDoEspionageMission(self.iDemographicsMission, iPlayerX, None, -1):
					self.aiPlayersMet.append(iPlayerX)
					self.iNumPlayersMet += 1
				else:
					self.aiPlayersMetNAEspionage.append(iPlayerX)
					self.iNumPlayersMetNAEspionage += 1


	def getLog10(self, x):
		return math.log10(max(1, x))

	def getNextLineName(self):
		name = "InfoGraphLine" + str(self.nLineCount)
		self.nLineCount += 1
		return name

	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = "InfoScreenWidget" + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def deleteAllWidgets(self, screen, iNumPermanentWidgets = 0):
		i = self.nWidgetCount - 1
		while i >= iNumPermanentWidgets:
			self.nWidgetCount = i
			screen.deleteWidget(self.getNextWidgetName())
			i -= 1

		self.nWidgetCount = iNumPermanentWidgets
		self.yMessage = 5


	# handle the input for this screen...
	def handleInput(self, inputClass):

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		iCode	= inputClass.eNotifyCode
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName

		szWidgetName = NAME + str(ID)

		if iCode == 11: # List Select

			# Debug dropdown
			if NAME == "Info_DebugDD":
				iIndex = screen.getSelectedPullDownID("Info_DebugDD")
				self.iPlayer = screen.getPullDownData("Info_DebugDD", iIndex)
				self.determineKnownPlayers()

				# Force recache of all scores
				self.scoreCache = []
				for t in xrange(self.iNumGraphs):
					self.scoreCache.append(None)
				self.redrawContents()

			iSelected = inputClass.getData()

			if self.iTab == 1:

				# Graph dropdown to select what values are being graphed
				if szWidgetName == self.szTurnsDropdownWidget:

					self.iGraphZoom = self.dropDownTurns[iSelected]
					self.checkGraphBounds()

					self.drawGraphs()

				elif szWidgetName == self.szGraphSmoothingDropdownWidget:
					self.iGraph_Smoothing = iSelected
					self.drawGraphs()


				if NAME == "GraphDD":
					if ID:
						self.iGraph2 = iSelected
					else: self.iGraph1 = iSelected
					self.drawGraphs()

			elif self.iTab == 2:

				if szWidgetName == self.szWondersTable and not inputClass.getMouseX():
					screen.hideScreen()
					pPlayer = GC.getPlayer(inputClass.getData1())
					pCity = pPlayer.getCity(inputClass.getData2())
					CyCamera().JustLookAtPlot(pCity.plot())

		# Something Clicked
		elif iCode == NotifyCode.NOTIFY_CLICKED:

			if NAME == "VS_Tab":
				screen.hide("VS_Tab_Act" + str(self.iTab))
				screen.show("VS_Tab" + str(self.iTab))
				if self.iTab == 1:
					screen.hide("Graph_BG")

				self.iTab = ID
				self.redrawContents()

			elif NAME == "GraphShiftL":
				self.slideGraph(- 2 * self.iGraphZoom / 5)
				self.drawGraphs()

			elif NAME == "GraphShiftR":
				self.slideGraph(2 * self.iGraphZoom / 5)
				self.drawGraphs()

			elif NAME in ("WorldWondersWidget", "NationalWondersWidget", "ProjectsWidget"):

				if NAME == "WorldWondersWidget":
					self.szWonderDisplayMode = "WorldWonders"

				elif NAME == "NationalWondersWidget":
					self.szWonderDisplayMode = "NationalWonders"

				elif NAME == "ProjectsWidget":
					self.szWonderDisplayMode = "Projects"

				self.redrawContents()

			if self.iTab == 1:
				for i in xrange(self.iNumGraphs):
					if szWidgetName == self.sGraphTextHeadingWidget[i] or NAME == "GraphArea" and ID == i:
						self.bDoubleGraph = not self.bDoubleGraph
						if self.iGraph1 != i:
							self.iGraph2 = self.iGraph1
							self.iGraph1 = i
						self.drawGraphs()
						break

					elif szWidgetName == self.sGraphTextBannerWidget[i]:
						if self.iGraph1 != i:
							self.iGraph1 = i
							self.drawGraphs()
							break

				for i in xrange(GC.getMAX_PC_PLAYERS()):
					if szWidgetName == self.sPlayerTextWidget[i]:
						self.bPlayerInclude[i] = not self.bPlayerInclude[i]
						self.drawGraphs()
						break

				if szWidgetName == self.sShowAllWidget:
					for i in xrange(GC.getMAX_PC_PLAYERS()):
						self.bPlayerInclude[i] = True
					self.drawGraphs()

				if szWidgetName == self.sShowNoneWidget:
					for i in xrange(GC.getMAX_PC_PLAYERS()):
						self.bPlayerInclude[i] = False
					self.drawGraphs()

		return 0


	def update(self, fDelta): return

	def onClose(self):
		screen = CyGInterfaceScreen("InfoScreen", self.screenId)
		screen.setDying(True)
		# Reset Wonders so nothing lingers next time the screen is opened
		self.szWonderDisplayMode = "WorldWonders"
		self.aiWonderListBoxIDs = []
		self.aiTurnYearBuilt = []
		self.aiWonderBuiltBy = []
		self.aszWonderCity = []
		del self.aWidgetBucket, self.bDebug, self.iTurn, self.iStartTurn, \
			self.xRes, self.yRes
