#
# Info Screen - Contains the Demographics, Wonders / Top Cities and Statistics Screens
#
# Thanks to "Ulf 'ulfn' Norell" from Apolyton for his additions relating to the graph section of this screen
#
from CvPythonExtensions import *

import string
import math

import BugCore
import BugUtil
AdvisorOpt = BugCore.game.Advisors
ScoreOpt = BugCore.game.Scores

# globals
GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

class CvInfoScreen:

	def __init__(self, screenId):

		self.screenId = screenId

		self.szWonderDisplayMode = "WorldWonders"
		self.iWonderID = -1 # BuildingType ID of the active wonder, e.g. Palace is 0, Globe Theater is 66
		self.iActiveWonderCounter = 0 # Screen ID for this wonder (0, 1, 2, etc.) - different from the above variable

		self.iGraphEnd		= GAME.getGameTurn() - 1
		self.iGraphZoom		= self.iGraphEnd - GAME.getStartTurn()
		self.nWidgetCount	= 0
		self.nLineCount		= 0

		# This is used to allow the wonders screen to refresh without redrawing everything
		self.iNumWondersPermanentWidgets = 0

		self.iTab = 0

		self.iGraphTabID = -1
		self.iGraph_Smoothing_1in1 = -1
		self.iGraph_Smoothing_7in1 = -1
		self.iGraph_Smoothing_3in1 = -1

		self.TOTAL_SCORE		= 0
		self.ECONOMY_SCORE		= 1
		self.INDUSTRY_SCORE		= 2
		self.AGRICULTURE_SCORE	= 3
		self.POWER_SCORE		= 4
		self.CULTURE_SCORE		= 5
		self.ESPIONAGE_SCORE	= 6
		self.REVOLUTION_STABILITY_AVERAGE	= 7
		self.NUM_SCORES						= 8
		self.RANGE_SCORES = xrange(self.NUM_SCORES)

		self.scoreCache	= []
		for t in self.RANGE_SCORES:
			self.scoreCache.append(None)

		self.xSelPt = 0
		self.ySelPt = 0

		self.graphLeftButtonID = ""
		self.graphRightButtonID = ""

		self.Graph_Status_1in1 = 0
		self.Graph_Status_7in1 = 1
		self.Graph_Status_3in1 = 2
		self.Graph_Status_Current = 0
		self.Graph_Status_Prior = 1

# the 7-in-1 graphs are layout out as follows:
#    0 1 2
#    L 3 4
#    L 5 6
#
# where L is the legend and a number represents a graph
		self.X_7_IN_1_CHART_ADJ = [0, 1, 2, 1, 2, 1, 2, 0]
		self.Y_7_IN_1_CHART_ADJ = [0, 0, 0, 1, 1, 2, 2, 1]

# the 3-in-1 graphs are layout out as follows:
#    0 1
#    L 2
#
# where L is the legend and a number represents a graph
		self.X_3_IN_1_CHART_ADJ = [0, 1, 1]
		self.Y_3_IN_1_CHART_ADJ = [0, 0, 1]

		# Placement of top 5 Cities
		self.Y_ROWS_CITIES = [90, 208, 326, 444, 562] # delta = 118

		self.reset()


	def reset(self):

		# City Members
		self.szCityNames = ["", "", "", "", ""]
		self.iCitySizes = [-1, -1, -1, -1, -1]
		self.szCityDescs = ["", "", "", "", ""]
		self.aaCitiesXY = [[-1, -1], [-1, -1], [-1, -1], [-1, -1], [-1, -1]]
		self.iCityValues = [0, 0, 0, 0, 0]
		self.pCityPointers = [0, 0, 0, 0, 0]

		self.iGraphEnd = GAME.getGameTurn() - 1
		self.iGraphZoom = self.iGraphEnd - GAME.getStartTurn()

		for t in self.RANGE_SCORES:
			self.scoreCache[t] = None


	# Screen construction function
	def interfaceScreen(self, iTabID, iEndGame):

		if GAME.getReplayMessageTurn(GAME.getNumReplayMessages() - 1) < -1:
			return
		screen = CyGInterfaceScreen("InfoScreen", self.screenId)
		if screen.isActive():
			return

		if iTabID > -1:
			self.iTab = iTabID

		# Get screen resolution.
		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y
		# Calibrate variables.
		if yRes > 1000:
			H_BOT_ROW = 110
			H_EDGE_PANEL = 38
			uFontEdge = "<font=4b>"
		elif yRes > 800:
			H_BOT_ROW = 92
			H_EDGE_PANEL = 32
			uFontEdge = "<font=3b>"
		else:
			H_BOT_ROW = 78
			H_EDGE_PANEL = 29
			uFontEdge = "<font=2b>"

		self.aFontList = aFontList = [uFontEdge] + SR.aFontList

		self.Y_TOP_PAGE = Y_TOP_PAGE = H_EDGE_PANEL - 6
		self.H_PAGE = H_PAGE = yRes - Y_TOP_PAGE - H_EDGE_PANEL + 8
		self.Y_BOT_TEXT = Y_BOT_TEXT = yRes - H_EDGE_PANEL + 10

		# Initialize text
		self.TEXT_DEMOGRAPHICS_SMALL = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_TITLE", ())
		self.TEXT_ENTIRE_HISTORY = TRNSLTR.getText("TXT_KEY_INFO_ENTIRE_HISTORY", ())

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

		self.TEXT_ECONOMY_MEASURE = (u"  %c" % GAME.getSymbolID(FontSymbols.BULLET_CHAR)) + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_ECONOMY_MEASURE", ())
		self.TEXT_INDUSTRY_MEASURE = (u"  %c" % GAME.getSymbolID(FontSymbols.BULLET_CHAR)) + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_INDUSTRY_MEASURE", ())
		self.TEXT_AGRICULTURE_MEASURE = (u"  %c" % GAME.getSymbolID(FontSymbols.BULLET_CHAR)) + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_AGRICULTURE_MEASURE", ())
		self.TEXT_MILITARY_MEASURE = ""
		self.TEXT_LAND_AREA_MEASURE = (u"  %c" % GAME.getSymbolID(FontSymbols.BULLET_CHAR)) + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_LAND_AREA_MEASURE", ())
		self.TEXT_POPULATION_MEASURE = ""
		self.TEXT_HAPPINESS_MEASURE = "%"
		self.TEXT_HEALTH_MEASURE = (u"  %c" % GAME.getSymbolID(FontSymbols.BULLET_CHAR)) + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_POPULATION_MEASURE", ())
		self.TEXT_IMP_EXP_MEASURE = (u"  %c" % GAME.getSymbolID(FontSymbols.BULLET_CHAR)) + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_ECONOMY_MEASURE", ())

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

		self.SHOW_ALL = "<font=2>" + TRNSLTR.getText("TXT_KEY_SHOW_ALL", ())
		self.SHOW_NONE = "<font=2>" + TRNSLTR.getText("TXT_KEY_SHOW_NONE", ())
		self.LOG_SCALE = "<font=2>" + TRNSLTR.getText("TXT_KEY_LOGSCALE", ())

		sTemp1 = [""] * self.NUM_SCORES
		sTemp2 = [""] * self.NUM_SCORES

		sTemp1[0] = TRNSLTR.getText("TXT_KEY_GAME_SCORE", ())
		sTemp1[1] = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_ECONOMY_TEXT", ())
		sTemp1[2] = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_INDUSTRY_TEXT", ())
		sTemp1[3] = TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_AGRICULTURE_TEXT", ())
		sTemp1[4] = TRNSLTR.getText("TXT_KEY_POWER", ())
		sTemp1[5] = TRNSLTR.getObjectText("TXT_KEY_COMMERCE_CULTURE", 0)
		sTemp1[6] = TRNSLTR.getObjectText("TXT_KEY_ESPIONAGE_CULTURE", 0)
		sTemp1[7] = TRNSLTR.getText("TXT_KEY_REVOLUTION_GRAPH_TITLE_SMALL", ())

		iYellow = GC.getInfoTypeForString("COLOR_YELLOW")

		for i in xrange(self.NUM_SCORES):
			sTemp2[i] = TRNSLTR.changeTextColor(sTemp1[i], iYellow)

		self.sGraphText = []
		self.sGraphText.append(sTemp1)
		self.sGraphText.append(sTemp2)

		# determine the big graph text spacing
		self.X_GRAPH_TEXT = [0] * self.NUM_SCORES

		iW_GRAPH = 934
		iTEXT_W = 0
		for i in xrange(self.NUM_SCORES):
			iTEXT_W += CyInterface().determineWidth(sTemp1[i])

		iText_Space = max(0, iW_GRAPH - iTEXT_W) / (self.NUM_SCORES - 1)

		for i in xrange(self.NUM_SCORES):
			if i == 0:
				self.X_GRAPH_TEXT[i] = 45
				continue

			self.X_GRAPH_TEXT[i] = self.X_GRAPH_TEXT[i - 1] + CyInterface().determineWidth(sTemp1[i - 1]) + iText_Space

		self.BUG_GRAPH_HELP = TRNSLTR.getText("TXT_KEY_BUG_CHART_HELP", ())
		self.BUG_LEGEND_DEAD = TRNSLTR.getText("TXT_KEY_BUG_DEAD_CIV", ())

		AFM = CyArtFileMgr()
		# world wonder / national wonder / projects icons / buttons
		self.BUGWorldWonder_On = AFM.getInterfaceArtInfo("BUG_WORLDWONDER_ON").getPath()
		self.BUGWorldWonder_Off = AFM.getInterfaceArtInfo("BUG_WORLDWONDER_OFF").getPath()
		self.BUGNatWonder_On = AFM.getInterfaceArtInfo("BUG_NATWONDER_ON").getPath()
		self.BUGNatWonder_Off = AFM.getInterfaceArtInfo("BUG_NATWONDER_OFF").getPath()
		self.BUGProject_On = AFM.getInterfaceArtInfo("BUG_PROJECT_ON").getPath()
		self.BUGProject_Off = AFM.getInterfaceArtInfo("BUG_PROJECT_OFF").getPath()

		self.pathScreenBG = AFM.getInterfaceArtInfo("SCREEN_BG").getPath()
		self.pathNullBtn = AFM.getInterfaceArtInfo("INTERFACE_BUTTON_NULL").getPath()
		self.zoomArt = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION").getPath()

		self.reset()

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
		screen.addPanel("Info_TopPanel", "", "", True, False, 0, 0, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("Info_BotPanel", "", "", True, False, 0, yRes - H_EDGE_PANEL, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.setLabel("Info_Header", "", uFontEdge + TRNSLTR.getText("TXT_KEY_INFO_SCREEN",()), 1<<2, xRes / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("Info_Exit", "", uFontEdge + TRNSLTR.getText("TXT_KEY_PEDIA_SCREEN_EXIT",()), 1<<1, xRes - 16, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		# Add tab control
		szTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_INFO_GRAPH", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iYellow)
		dX = xRes / 4
		x = dX / 2
		screen.setText("VS_Tab0", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("VS_Tab_Act0", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("VS_Tab_Act0")

		szTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_TITLE", ())
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
		if GAME.isDebugMode():
			DD = "Info_DebugDD"
			screen.addDropDownBoxGFC(DD, 22, 0, 300, eWidGen, 1, 2, FontTypes.GAME_FONT)
			for iPlayerX in xrange(GC.getMAX_PLAYERS()):
				if GC.getPlayer(iPlayerX).isAlive():
					screen.addPullDownString(DD, GC.getPlayer(iPlayerX).getName(), iPlayerX, iPlayerX, iPlayer == iPlayerX)

		# "Save" current widgets so they won't be deleted later when changing tabs
		self.iNumPermanentWidgets = self.nWidgetCount

		# Reset variables
		self.iGraphEnd	= GAME.getGameTurn() - 1
		self.iGraphZoom	= self.iGraphEnd - GAME.getStartTurn()

		self.redrawContents()


	def redrawContents(self):

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)
		self.deleteAllWidgets(self.iNumPermanentWidgets)
		self.iNumWondersPermanentWidgets = 0

		# Draw Tab buttons and tabs
		if not self.iTab:
			screen.hide("VS_Tab0")
			screen.show("VS_Tab_Act0")
			self.drawGraphTab()

		elif self.iTab == 1:
			screen.hide("VS_Tab1")
			screen.show("VS_Tab_Act1")
			self.drawDemographicsTab()

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

		if self.iGraphTabID == -1:
			self.iGraphTabID = self.TOTAL_SCORE
			self.iGraph_Smoothing_1in1 = 0
			self.iGraph_Smoothing_7in1 = 0
			self.bPlayerInclude = [True] * GC.getMAX_PC_PLAYERS()

		self.drawPermanentGraphWidgets()
		self.drawGraphs()

	def drawPermanentGraphWidgets(self):

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		self.sGraphTextHeadingWidget = [0] * self.NUM_SCORES
		self.sGraphTextBannerWidget = [0] * self.NUM_SCORES
		self.sGraphPanelWidget = [0] * self.NUM_SCORES
		self.sGraphBGWidget = [0] * self.NUM_SCORES
		for i in xrange(self.NUM_SCORES):
			self.sGraphTextHeadingWidget[i] = self.getNextWidgetName()
			self.sGraphTextBannerWidget[i] = self.getNextWidgetName()
			self.sGraphPanelWidget[i] = self.getNextWidgetName()
			self.sGraphBGWidget[i] = self.getNextWidgetName()

		self.sGraph7in1 = self.getNextWidgetName()
		self.sGraph3in1 = self.getNextWidgetName()
		self.sGraph1in1 = self.getNextWidgetName()

		self.sPlayerTextWidget = [0] * GC.getMAX_PC_PLAYERS()
		for i in xrange(GC.getMAX_PC_PLAYERS()):
			self.sPlayerTextWidget[i] = self.getNextWidgetName()

		self.sShowAllWidget = self.getNextWidgetName()
		self.sShowNoneWidget = self.getNextWidgetName()

		if not AdvisorOpt.isGraphs():
			self.drawLegend()

		if AdvisorOpt.isGraphs():
			iX_LEFT_BUTTON = 45
		else:
			iX_LEFT_BUTTON = 205

		self.graphLeftButtonID = self.getNextWidgetName()
		screen.setButtonGFC( self.graphLeftButtonID, u"", "", iX_LEFT_BUTTON, 670, 20, 20, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_ARROW_LEFT )
		self.graphRightButtonID = self.getNextWidgetName()
		screen.setButtonGFC( self.graphRightButtonID, u"", "", 959, 670, 20, 20, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_ARROW_RIGHT )
		screen.enable(self.graphLeftButtonID, False)
		screen.enable(self.graphRightButtonID, False)

		# Dropdown Box
		self.szGraphDropdownWidget = self.getNextWidgetName()
		screen.addDropDownBoxGFC(self.szGraphDropdownWidget, 45, 80, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_SCORE, 0, 0, False)
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_ECONOMY, 1, 1, False)
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_INDUSTRY, 2, 2, False)
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_AGRICULTURE, 3, 3, False)
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_POWER, 4, 4, False)
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_CULTURE, 5, 5, False)
		screen.addPullDownString(self.szGraphDropdownWidget, self.TEXT_ESPIONAGE, 6, 6, False)

		if AdvisorOpt.isGraphs():
			screen.hide(self.szGraphDropdownWidget)

			iX_ZOOM_DROPDOWN = 870
			iY_ZOOM_DROPDOWN = 10
			if GAME.isDebugMode():
				iY_SMOOTH_DROPDOWN = iY_ZOOM_DROPDOWN + 50
			else:
				iY_SMOOTH_DROPDOWN = iY_ZOOM_DROPDOWN
		else:
			iX_ZOOM_DROPDOWN = 45
			iY_ZOOM_DROPDOWN = 115

		# graph smoothing dropdown
		if AdvisorOpt.isGraphs():
			self.szGraphSmoothingDropdownWidget_1in1 = self.getNextWidgetName()
			self.szGraphSmoothingDropdownWidget_7in1 = self.getNextWidgetName()

	#		screen.addDropDownBoxGFC(self.szGraphSmoothingDropdownWidget, iX_ZOOM_DROPDOWN - 150 - 60, iY_ZOOM_DROPDOWN, 200, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			screen.addDropDownBoxGFC(self.szGraphSmoothingDropdownWidget_1in1, 10, iY_SMOOTH_DROPDOWN, 200, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			screen.addDropDownBoxGFC(self.szGraphSmoothingDropdownWidget_7in1, 10, iY_SMOOTH_DROPDOWN, 200, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for i in xrange(11):
				screen.addPullDownString(self.szGraphSmoothingDropdownWidget_1in1, TRNSLTR.getText("TXT_KEY_GRAPH_SMOOTHING", (i,)), i, i, False )
				screen.addPullDownString(self.szGraphSmoothingDropdownWidget_7in1, TRNSLTR.getText("TXT_KEY_GRAPH_SMOOTHING", (i,)), i, i, False )

			screen.hide(self.szGraphSmoothingDropdownWidget_1in1)
			screen.hide(self.szGraphSmoothingDropdownWidget_7in1)

			# 3 in 1 graph selectionS
			self.szGraphDropdownWidget_3in1 = [""] * 3
			self.iGraph_3in1 = [0, 1, 2]
			iW_GRAPH = 463
			iH_GRAPH = 290
			for i in xrange(3):
				self.szGraphDropdownWidget_3in1[i] = self.getNextWidgetName()
				x = 55 + self.X_3_IN_1_CHART_ADJ[i] * (iW_GRAPH + 11)
				y = 85 + self.Y_3_IN_1_CHART_ADJ[i] * (iH_GRAPH + 10)
				screen.addDropDownBoxGFC(self.szGraphDropdownWidget_3in1[i], x, y, 200, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				for j in xrange(self.NUM_SCORES):
					screen.addPullDownString(self.szGraphDropdownWidget_3in1[i], self.sGraphText[0][j], j, j, self.iGraph_3in1[i] == j )
				screen.hide(self.szGraphDropdownWidget_3in1[i])

		if not AdvisorOpt.isGraphs():
			self.iGraph_Smoothing = 0

		self.dropDownTurns = []
		self.szTurnsDropdownWidget = self.getNextWidgetName()
		screen.addDropDownBoxGFC(self.szTurnsDropdownWidget, iX_ZOOM_DROPDOWN, iY_ZOOM_DROPDOWN, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		start = GAME.getStartTurn()
		now   = GAME.getGameTurn()
		nTurns = now - start - 1
		screen.addPullDownString(self.szTurnsDropdownWidget, self.TEXT_ENTIRE_HISTORY, 0, 0, False)
		self.dropDownTurns.append(nTurns)
		iCounter = 1
		last = 50
		while (last < nTurns):
			screen.addPullDownString(self.szTurnsDropdownWidget, TRNSLTR.getText("TXT_KEY_INFO_NUM_TURNS", (last,)), iCounter, iCounter, False)
			self.dropDownTurns.append(last)
			iCounter += 1
			last += 50

		if AdvisorOpt.isGraphs():
			screen.setText(self.sGraph1in1, "", "1/1", 1<<2, 22,  90, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.setText(self.sGraph3in1, "", "3/1", 1<<2, 22, 110, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
			screen.setText(self.sGraph7in1, "", "7/1", 1<<2, 22, 130, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		screen.setLabel(self.getNextWidgetName(), "", self.BUG_GRAPH_HELP, 1<<2, 512, 690, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.iNumPreDemoChartWidgets = self.nWidgetCount


	def updateGraphButtons(self):
		screen = CyGInterfaceScreen("InfoScreen", self.screenId)
		screen.enable(self.graphLeftButtonID, self.iGraphEnd - self.iGraphZoom > GAME.getStartTurn())
		screen.enable(self.graphRightButtonID, self.iGraphEnd < GAME.getGameTurn() - 1)

	def checkGraphBounds(self):
		start = GAME.getStartTurn()
		end   = GAME.getGameTurn() - 1
		if self.iGraphEnd - self.iGraphZoom < start:
			self.iGraphEnd = start + self.iGraphZoom
		if self.iGraphEnd > end:
			self.iGraphEnd = end

	def slideGraph(self, right):
		self.iGraphEnd += right
		self.checkGraphBounds()
		self.updateGraphButtons()


	def computeHistory(self, scoreType, iPlayer, iTurn):

		iScore = GC.getPlayer(iPlayer).getScoreHistory(iTurn)

		if iScore == 0: # for some reason only the score is 0 when you're dead..?
			return 0

		if scoreType == self.TOTAL_SCORE:
			return iScore

		if scoreType == self.ECONOMY_SCORE:
			return GC.getPlayer(iPlayer).getEconomyHistory(iTurn)

		if scoreType == self.INDUSTRY_SCORE:
			return GC.getPlayer(iPlayer).getIndustryHistory(iTurn)

		if scoreType == self.AGRICULTURE_SCORE:
			return GC.getPlayer(iPlayer).getAgricultureHistory(iTurn)

		if scoreType == self.POWER_SCORE:
			return GC.getPlayer(iPlayer).getPowerHistory(iTurn)

		if scoreType == self.CULTURE_SCORE:
			return GC.getPlayer(iPlayer).getCultureHistory(iTurn)

		if scoreType == self.ESPIONAGE_SCORE:
			return GC.getPlayer(iPlayer).getEspionageHistory(iTurn)

		if scoreType == self.REVOLUTION_STABILITY_AVERAGE:
			return 1000 - GC.getPlayer(iPlayer).getRevolutionStabilityHistory(iTurn)


	def drawXLabel(self, screen, turn, x, just = 1<<2):
		if AdvisorOpt.isGraphs():

			if self.Graph_Status_Current == self.Graph_Status_1in1:
				screen.show(self.graphLeftButtonID)
				screen.show(self.graphRightButtonID)
			else:
				screen.hide(self.graphLeftButtonID)
				screen.hide(self.graphRightButtonID)
				return

		year = GAME.getTurnYear(turn)
		if year < 0:
			date = TRNSLTR.getText("TXT_KEY_TIME_BC", (-year,))
		else: date = TRNSLTR.getText("TXT_KEY_TIME_AD", (year,))

		screen.setLabel(self.getNextWidgetName(), "", "<font=2>" + date, just ,x , 673, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def drawGraphs(self):

		self.deleteAllLines()
		self.deleteAllWidgets(self.iNumPreDemoChartWidgets)

#BUG: Change Graphs - start
		if not AdvisorOpt.isGraphs():
			self.drawGraph(self.iGraphTabID)
		else:
			screen = CyGInterfaceScreen("InfoScreen", self.screenId)

			# show the right smoothing dropdown
			if self.Graph_Status_Current == self.Graph_Status_1in1:
				screen.show(self.szGraphSmoothingDropdownWidget_1in1)
				screen.hide(self.szGraphSmoothingDropdownWidget_7in1)
			else:
				screen.hide(self.szGraphSmoothingDropdownWidget_1in1)
				screen.show(self.szGraphSmoothingDropdownWidget_7in1)

			#RevolutionDCM
			for i in xrange(self.NUM_SCORES):
				screen.hide(self.sGraphTextHeadingWidget[i])
				screen.hide(self.sGraphPanelWidget[i])

			for i in xrange(3):
				screen.hide(self.szGraphDropdownWidget_3in1[i])

			if self.Graph_Status_Current == self.Graph_Status_1in1:
				self.drawGraph(self.iGraphTabID)
				self.drawLegend()

				iY = 50

				for i in xrange(self.NUM_SCORES):
					iX = self.X_GRAPH_TEXT[i]
					screen.hide(self.sGraphTextBannerWidget[i])
					if i == self.iGraphTabID:
						screen.setText(self.sGraphTextBannerWidget[i], "", self.sGraphText[1][i], 1<<0, iX, iY, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
					else:
						screen.setText(self.sGraphTextBannerWidget[i], "", self.sGraphText[0][i], 1<<0, iX, iY, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

			elif self.Graph_Status_Current == self.Graph_Status_7in1:

				for i in xrange(self.NUM_SCORES):
					screen.hide(self.sGraphTextBannerWidget[i])
					self.drawGraph(i)

				self.drawLegend()
			else:
				for i in xrange(self.NUM_SCORES):
					screen.hide(self.sGraphTextBannerWidget[i])

				for i in xrange(3):
					self.drawGraph(i)

				self.drawLegend()


	def drawGraph(self, vGraphID_Locn):

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		if self.Graph_Status_Current == self.Graph_Status_3in1:
			iGraphID = self.iGraph_3in1[vGraphID_Locn]
		else: iGraphID = vGraphID_Locn

		if not AdvisorOpt.isGraphs():
			iX_GRAPH = 205
			iY_GRAPH = 80
			iW_GRAPH = 774
			iH_GRAPH = 590
		else:
			# compute graph x, y, w, h
			if self.Graph_Status_Current == self.Graph_Status_1in1:
				# graph is 'full' screen - or as big as I can get it without it looking stupid
				iX_GRAPH = 45
				iY_GRAPH = 80
				iW_GRAPH = 934
				iH_GRAPH = 590

			elif self.Graph_Status_Current == self.Graph_Status_7in1:
				# graphs are in following layout
# the 7-in-1 graphs are layout out as follows:
#    0 1 2     305 11 305 11 305 for a total of 937 pixals wide
#    L 3 4     190 10 190 10 190 for a total of 590 pixals high
#    L 5 6
#
# where L is the legend and a number represents a graph
#		self.X_7_IN_1_CHART_ADJ = [0, 1, 2, 1, 2, 1, 2]
#		self.Y_7_IN_1_CHART_ADJ = [0, 0, 0, 1, 1, 2, 2]
				iW_GRAPH = 305
				iH_GRAPH = 190
				iX_GRAPH = 45 + self.X_7_IN_1_CHART_ADJ[vGraphID_Locn] * (iW_GRAPH + 11)
				iY_GRAPH = 80 + self.Y_7_IN_1_CHART_ADJ[vGraphID_Locn] * (iH_GRAPH + 10) #+ 25

			else: #self.Graph_Status_Current == self.Graph_Status_3in1:
				# graphs are in following layout
# the 3-in-1 graphs are layout out as follows:
#    0 1     463 11 463 for a total of 937 pixals wide
#    L 2     290 10 290 for a total of 590 pixals high
#
# where L is the legend and a number represents a graph
#		self.X_3_IN_1_CHART_ADJ = [0, 1, 1]
#		self.Y_3_IN_1_CHART_ADJ = [0, 0, 1]
				iW_GRAPH = 463
				iH_GRAPH = 290
				iX_GRAPH = 45 + self.X_3_IN_1_CHART_ADJ[vGraphID_Locn] * (iW_GRAPH + 11)
				iY_GRAPH = 80 + self.Y_3_IN_1_CHART_ADJ[vGraphID_Locn] * (iH_GRAPH + 10)


		eWidGen = WidgetTypes.WIDGET_GENERAL

		# Draw the graph widget
		zsGRAPH_CANVAS_ID = self.getNextWidgetName()
		screen.addDrawControl(zsGRAPH_CANVAS_ID, self.pathScreenBG, iX_GRAPH, iY_GRAPH, iW_GRAPH, iH_GRAPH, eWidGen, 1, 2)

		screen.addCheckBoxGFC(self.sGraphBGWidget[vGraphID_Locn], self.pathNullBtn, self.pathNullBtn, iX_GRAPH, iY_GRAPH, iW_GRAPH, iH_GRAPH, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_LABEL)

		# Compute max score
		thisTurn = GAME.getGameTurn()
		startTurn = GAME.getStartTurn()

		# Check if the scores have already been computed
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
		else:
			firstTurn = self.iGraphEnd - self.iGraphZoom

		if self.iGraphEnd:
			lastTurn = self.iGraphEnd
		else:
			lastTurn = thisTurn - 1 # all civs haven't neccessarily got a score for the current turn

		# Draw graph lines
		if self.xSelPt != 0 or self.ySelPt != 0:
			screen.addLineGFC(zsGRAPH_CANVAS_ID, "GraphHLine", 0, self.ySelPt, 774, self.ySelPt, GC.getInfoTypeForString("COLOR_GREY"))
			screen.addLineGFC(zsGRAPH_CANVAS_ID, "GraphVLine", self.xSelPt, 0, self.xSelPt, 590, GC.getInfoTypeForString("COLOR_GREY"))
		else:
			screen.addLineGFC(zsGRAPH_CANVAS_ID, "GraphHLine", -1, -1, -1, -1, GC.getInfoTypeForString("COLOR_GREY"))
			screen.addLineGFC(zsGRAPH_CANVAS_ID, "GraphVLine", -1, -1, -1, -1, GC.getInfoTypeForString("COLOR_GREY"))

		if AdvisorOpt.isGraphs():
			iX_LEFT_LABEL = 75
		else:
			iX_LEFT_LABEL = 235

		# Draw x-labels
		self.drawXLabel(screen, firstTurn, iX_LEFT_LABEL, 1<<0)
		self.drawXLabel(screen, lastTurn,  949, 1<<1)

		# Don't draw anything the first turn
		if firstTurn >= lastTurn:
			return

		# Compute max and min
		if AdvisorOpt.isGraphsLogScale():
			max = 2
			min = 1
		else:
			max = 1
			min = 0
		for p in self.aiPlayersMet:
			for turn in xrange(firstTurn,lastTurn + 1):
				score = self.scoreCache[iGraphID][p][turn - startTurn]
				if (max < score):
					max = score
				if (min > score):
					min = score

		if AdvisorOpt.isGraphsLogScale():
			yFactor = (1.0 * iH_GRAPH / (1.0 * (self.getLog10(max) - self.getLog10(min))))
		else:
			yFactor = (1.0 * iH_GRAPH / (1.0 * (max - min)))

		xFactor = (1.0 * iW_GRAPH / (1.0 * (lastTurn - firstTurn)))

		if lastTurn - firstTurn > 10:
			turn = (firstTurn + lastTurn) / 2
			self.drawXLabel ( screen, turn, iX_GRAPH + int(xFactor * (turn - firstTurn)) )
			if (lastTurn - firstTurn > 20):
				turn = firstTurn + (lastTurn - firstTurn) / 4
				self.drawXLabel ( screen, turn, iX_GRAPH + int(xFactor * (turn - firstTurn)) )
				turn = firstTurn + 3 * (lastTurn - firstTurn) / 4
				self.drawXLabel ( screen, turn, iX_GRAPH + int(xFactor * (turn - firstTurn)) )

		# Draw the lines
		for p in self.aiPlayersMet:
			if AdvisorOpt.isGraphs():
				i = GC.getPlayer(p).getID()
				if not self.bPlayerInclude[i]:
					continue

			color = GC.getPlayerColorInfo(GC.getPlayer(p).getPlayerColor()).getColorTypePrimary()
			oldX = -1
			oldY = iH_GRAPH
			turn = lastTurn

			if self.Graph_Status_Current == self.Graph_Status_1in1:
				iSmooth = self.iGraph_Smoothing_1in1
			else:
				iSmooth = self.iGraph_Smoothing_7in1

			while turn >= firstTurn:

				score = self.scoreCache[iGraphID][p][turn - startTurn]

				if AdvisorOpt.isGraphsLogScale():
					y = iH_GRAPH - int(yFactor * (self.getLog10(score) - self.getLog10(min)))
				else:
					y = iH_GRAPH - int(yFactor * (score - min))
				x = int(xFactor * (turn - firstTurn))

				if x < oldX - iSmooth:
					if (y != iH_GRAPH or oldY != iH_GRAPH):
						self.drawLine(screen, zsGRAPH_CANVAS_ID, oldX, oldY, x, y, color, self.Graph_Status_Current == self.Graph_Status_1in1)
					oldX = x
					oldY = y
				elif (oldX == -1):
					oldX = x
					oldY = y

				turn -= 1

		# draw the chart text
		if AdvisorOpt.isGraphs():
			if self.Graph_Status_Current == self.Graph_Status_1in1:
				iY_GRAPH_TITLE = iY_GRAPH + 10
			else:
				iY_GRAPH_TITLE = iY_GRAPH + 5

			if self.Graph_Status_Current == self.Graph_Status_3in1:
				screen.show(self.szGraphDropdownWidget_3in1[vGraphID_Locn])
				screen.moveToFront(self.szGraphDropdownWidget_3in1[vGraphID_Locn])
			else:
				screen.addPanel(self.sGraphPanelWidget[vGraphID_Locn], "", "", True, True, iX_GRAPH + 5, iY_GRAPH_TITLE, 150, 25, PanelStyles.PANEL_STYLE_IN)
				screen.setText(self.sGraphTextHeadingWidget[iGraphID], "", self.sGraphText[0][iGraphID], 1<<0, iX_GRAPH + 10, iY_GRAPH_TITLE, 0, FontTypes.TITLE_FONT, eWidGen, 1, 2)


	def drawLegend(self):
		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		iW_LEGEND = 150
		if AdvisorOpt.isGraphs():
			for p in self.aiPlayersMet:
				szPlayerName = self.getPlayerName(p)
				if not GC.getPlayer(p).isAlive(): szPlayerName += " [" + self.BUG_LEGEND_DEAD + "]"

				if iW_LEGEND < 50 + CyInterface().determineWidth(szPlayerName) + 10:
					iW_LEGEND = 50 + CyInterface().determineWidth(szPlayerName) + 10

			for p in self.aiPlayersMetNAEspionage:
				szPlayerName = self.getPlayerName(p)
				if not GC.getPlayer(p).isAlive(): szPlayerName += " [" + self.BUG_LEGEND_DEAD + "]"

				if iW_LEGEND < 50 + CyInterface().determineWidth(szPlayerName) + 10:
					iW_LEGEND = 50 + CyInterface().determineWidth(szPlayerName) + 10

		if not AdvisorOpt.isGraphs():
			X_LEGEND = 45

			H_LEGEND = 13 + self.iNumPlayersMet * 16
			if AdvisorOpt.isGraphsLogScale():
				H_LEGEND += 16
			Y_LEGEND = 670 - H_LEGEND
		else:
			X_LEGEND = 50

			H_LEGEND = 13 + (self.iNumPlayersMet + self.iNumPlayersMetNAEspionage + 4) * 16
			if AdvisorOpt.isGraphsLogScale():
				H_LEGEND += 16

			if self.Graph_Status_Current == self.Graph_Status_1in1:
				Y_LEGEND = 120
			else:
				Y_LEGEND = 670 - H_LEGEND

		screen.addPanel(self.getNextWidgetName(), "", "", True, True, X_LEGEND, Y_LEGEND, iW_LEGEND, H_LEGEND, PanelStyles.PANEL_STYLE_IN)

		sLEGEND_CANVAS_ID = self.getNextWidgetName()
		screen.addDrawControl(sLEGEND_CANVAS_ID, None, X_LEGEND, Y_LEGEND, iW_LEGEND, H_LEGEND, WidgetTypes.WIDGET_GENERAL, -1, -1)

		yLine = 14 # to center it relative to the text
		yText = Y_LEGEND + 5

		for p in self.aiPlayersMet:

			if AdvisorOpt.isGraphs():
				name = self.getPlayerName(p)
			else:
				name = GC.getPlayer(p).getName()

			i = GC.getPlayer(p).getID()
			if self.bPlayerInclude[i] or not AdvisorOpt.isGraphs():

				textColorR = GC.getPlayer(p).getPlayerTextColorR()
				textColorG = GC.getPlayer(p).getPlayerTextColorG()
				textColorB = GC.getPlayer(p).getPlayerTextColorB()
				textColorA = GC.getPlayer(p).getPlayerTextColorA()

				lineColor = GC.getPlayerColorInfo(GC.getPlayer(p).getPlayerColor()).getColorTypePrimary()
				self.drawLine(screen, sLEGEND_CANVAS_ID, 10, yLine, 40, yLine, lineColor, True)
			else:
				textColorR = 175
				textColorG = 175
				textColorB = 175
				textColorA = GC.getPlayer(p).getPlayerTextColorA()

			str = u"<color=%d,%d,%d,%d>%s</color>" %(textColorR,textColorG,textColorB,textColorA,name)

			if AdvisorOpt.isGraphs():
				screen.setText(self.sPlayerTextWidget[i], "", "<font=2>" + str, 1<<0, X_LEGEND + 50, yText, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
			else:
				screen.setLabel(self.sPlayerTextWidget[i], "", "<font=2>" + str, 1<<0, X_LEGEND + 50, yText, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)

			yLine += 16
			yText += 16

		# ADD players where you don't have enough espionage points
		if AdvisorOpt.isGraphs():
			# add blank line
			yLine += 16
			yText += 16
			for p in self.aiPlayersMetNAEspionage:
				i = GC.getPlayer(p).getID()

				name = self.getPlayerName(p)

				if not GC.getPlayer(p).isAlive(): # player is dead!
					textColorR = 175
					textColorG = 175
					textColorB = 175
					textColorA = GC.getPlayer(p).getPlayerTextColorA()
					name += " [" + self.BUG_LEGEND_DEAD + "]"
				else:
					textColorR = GC.getPlayer(p).getPlayerTextColorR()
					textColorG = GC.getPlayer(p).getPlayerTextColorG()
					textColorB = GC.getPlayer(p).getPlayerTextColorB()
					textColorA = GC.getPlayer(p).getPlayerTextColorA()

				str = "<color=%d,%d,%d,%d>%s</color>" %(textColorR, textColorG, textColorB, textColorA, name)

				screen.setLabel(self.sPlayerTextWidget[i], "", "<font=2>" + str, 1<<0, X_LEGEND + 52, yText, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)

				yLine += 16
				yText += 16

			yText += 16
			xShow = X_LEGEND + iW_LEGEND / 2
			screen.setText(self.sShowAllWidget, "", self.SHOW_ALL, 1<<2, xShow, yText, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			yText += 16
			screen.setText(self.sShowNoneWidget, "", self.SHOW_NONE, 1<<2, xShow, yText, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		if AdvisorOpt.isGraphsLogScale():
			if AdvisorOpt.isGraphs():
				yText += 16
			screen.setLabel(self.getNextWidgetName(), "", self.LOG_SCALE, 1<<2, X_LEGEND + iW_LEGEND / 2, yText, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)


	def getPlayerName(self, ePlayer):
		if (ScoreOpt.isUsePlayerName()):
			szPlayerName = GC.getPlayer(ePlayer).getName()
		else:
			szPlayerName = GC.getLeaderHeadInfo(GC.getPlayer(ePlayer).getLeaderType()).getDescription()

		if (ScoreOpt.isShowBothNames()):
			szPlayerName = szPlayerName + "/" + GC.getPlayer(ePlayer).getCivilizationShortDescription(0)
		elif (ScoreOpt.isShowLeaderName()):
			szPlayerName = szPlayerName
		else:
			szPlayerName = GC.getPlayer(ePlayer).getCivilizationShortDescription(0)

		return szPlayerName
#BUG: Change Graphs - end

#############################################################################################################
################################################# DEMOGRAPHICS ##############################################
#############################################################################################################

	def drawDemographicsTab(self):

		self.drawTextChart()

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

	def drawTextChart(self):

		######## DATA ########

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

		return

#############################################################################################################
################################################## TOP CITIES ###############################################
#############################################################################################################

	def drawTopCitiesTab(self):

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		# Background Panes
		screen.addPanel(self.getNextWidgetName(), "", "", True, True, 45, 70, 470, 620, PanelStyles.PANEL_STYLE_MAIN)

		self.drawTopCities()

		if AdvisorOpt.isShowInfoWonders():
			xDD = 540
			wDD = 420
		else:
			xDD = 763
			wDD = 200

		screen.addPanel(self.getNextWidgetName(), "", "", True, True, 520, 70, 460, 620, PanelStyles.PANEL_STYLE_MAIN)

		# Draw the Wonders Dropdown Box"
		self.szWondersDropdownWidget = self.getNextWidgetName()

		if AdvisorOpt.isShowInfoWonders():
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

			screen.setLabel(self.getNextWidgetName(), "", "<font=4>" + sDesc, 1<<0, xDD + 100, 93, -6.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
		else:
			screen.addDropDownBoxGFC(self.szWondersDropdownWidget, xDD, 90, wDD, WidgetTypes.WIDGET_GENERAL, 1, 2, FontTypes.GAME_FONT)
			screen.addPullDownString(self.szWondersDropdownWidget, TRNSLTR.getText("TXT_KEY_TOP_CITIES_SCREEN_WORLD_WONDERS", ()), 0, 0, self.szWonderDisplayMode == "WorldWonders")
			screen.addPullDownString(self.szWondersDropdownWidget, TRNSLTR.getText("TXT_KEY_TOP_CITIES_SCREEN_NATIONAL_WONDERS", ()), 1, 1, self.szWonderDisplayMode == "NationalWonders")
			screen.addPullDownString(self.szWondersDropdownWidget, TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_PROJECT", ()), 2, 2, self.szWonderDisplayMode == "Projects")

		if AdvisorOpt.isShowInfoWonders():
			self.calculateWondersList_BUG()
			self.drawWondersList_BUG()
		else:
			self.calculateWondersList()
			self.drawWondersList()


	def drawTopCities(self):

		self.calculateTopCities()
		self.determineCityData()

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		self.szCityNameWidgets = []
		self.szCityDescWidgets = []
		self.szCityAnimWidgets = []

		for iWidgetLoop in xrange(self.iNumCities):

			szTextPanel = self.getNextWidgetName()
			screen.addPanel( szTextPanel, "", "", False, True,
				225, self.Y_ROWS_CITIES[iWidgetLoop] - 4, 275, 60, PanelStyles.PANEL_STYLE_DAWNTOP )
			self.szCityNameWidgets.append(self.getNextWidgetName())
#			szProjectDesc = u"<font=3b>" + pProjectInfo.getDescription().upper() + u"</font>"
			szCityDesc = u"<font=4b>" + str(self.iCitySizes[iWidgetLoop]) + u"</font>" + " - " + u"<font=3b>" + self.szCityNames[iWidgetLoop] + u"</font>" + "\n"
			szCityDesc += self.szCityDescs[iWidgetLoop]
			screen.addMultilineText(self.szCityNameWidgets[iWidgetLoop], szCityDesc,
				231, self.Y_ROWS_CITIES[iWidgetLoop] - 1, 269, 54, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
#			screen.attachMultilineText( szTextPanel, self.szCityNameWidgets[iWidgetLoop], str(self.iCitySizes[iWidgetLoop]) + " - " + self.szCityNames[iWidgetLoop] + "\n" + self.szCityDescs[iWidgetLoop], WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

			iCityX = self.aaCitiesXY[iWidgetLoop][0]
			iCityY = self.aaCitiesXY[iWidgetLoop][1]
			pPlot = CyMap().plot(iCityX, iCityY)
			pCity = pPlot.getPlotCity()

			iDistance = 200 + (pCity.getPopulation() * 5)
			if (iDistance > 350):
				iDistance = 350

			self.szCityAnimWidgets.append(self.getNextWidgetName())

			if (pCity.isRevealed(GAME.getActiveTeam(), False)):
				screen.addPlotGraphicGFC(self.szCityAnimWidgets[iWidgetLoop], 65, self.Y_ROWS_CITIES[iWidgetLoop], 150, 110, pPlot, iDistance, False, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Draw Wonder icons
		self.drawCityWonderIcons();

		return

	def drawCityWonderIcons(self):

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		aaiTopCitiesWonders = []
		aiTopCitiesNumWonders = []
		for i in xrange(self.iNumCities):
			aaiTopCitiesWonders.append(0)
			aiTopCitiesNumWonders.append(0)

		# Loop through top cities and determine if they have any wonders to display
		for iCityLoop in xrange(self.iNumCities):

			if (self.pCityPointers[iCityLoop]):

				pCity = self.pCityPointers[iCityLoop]

				aiTempWondersList = []

				# Loop through buildings

				for iBuildingLoop in xrange(GC.getNumBuildingInfos()):

					pBuilding = GC.getBuildingInfo(iBuildingLoop)

					# If this building is a wonder...
					if (isWorldWonder(iBuildingLoop)):

						if (pCity.getNumBuilding(iBuildingLoop) > 0):

							aiTempWondersList.append(iBuildingLoop)
							aiTopCitiesNumWonders[iCityLoop] += 1

				aaiTopCitiesWonders[iCityLoop] = aiTempWondersList

		# Create Scrollable areas under each city
		self.szCityWonderScrollArea = []
		for iCityLoop in range (self.iNumCities):

			self.szCityWonderScrollArea.append(self.getNextWidgetName())

			szIconPanel = self.szCityWonderScrollArea[iCityLoop]
			screen.addPanel( szIconPanel, "", "", False, True,
				225, self.Y_ROWS_CITIES[iCityLoop] + 53, 275, 60, PanelStyles.PANEL_STYLE_DAWNTOP )

			# Now place the wonder buttons
			for iWonderLoop in xrange(aiTopCitiesNumWonders[iCityLoop]):

				iBuildingID = aaiTopCitiesWonders[iCityLoop][iWonderLoop]
				screen.attachImageButton( szIconPanel, "", GC.getBuildingInfo(iBuildingID).getButton(),
					GenericButtonSizes.BUTTON_SIZE_46, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iBuildingID, -1, False )

	# Calculate the top 5 cities
	def calculateTopCities(self):

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


	def determineCityData(self):

		self.iNumCities = 0

		for iRankLoop in xrange(5):

			pCity = self.pCityPointers[iRankLoop]

			# If this city exists and has data we can use
			if pCity:

				pPlayer = GC.getPlayer(pCity.getOwner())

				iTurnYear = GAME.getTurnYear(pCity.getGameTurnFounded())

				if (iTurnYear < 0):
					szTurnFounded = TRNSLTR.getText("TXT_KEY_TIME_BC", (-iTurnYear,))#"%d %s" %(-iTurnYear, self.TEXT_BC)
				else:
					szTurnFounded = TRNSLTR.getText("TXT_KEY_TIME_AD", (iTurnYear,))#"%d %s" %(iTurnYear, self.TEXT_AD)

				if pCity.isRevealed(GAME.getActiveTeam(), False) or GC.getTeam(pPlayer.getTeam()).isHasMet(GAME.getActiveTeam()):
					self.szCityNames[iRankLoop] = pCity.getName().upper()
					self.szCityDescs[iRankLoop] = ("%s, %s" %(pPlayer.getCivilizationAdjective(0), TRNSLTR.getText("TXT_KEY_MISC_FOUNDED_IN", (szTurnFounded,))))
				else:
					self.szCityNames[iRankLoop] = TRNSLTR.getText("TXT_KEY_UNKNOWN", ()).upper()
					self.szCityDescs[iRankLoop] = ("%s" %(TRNSLTR.getText("TXT_KEY_MISC_FOUNDED_IN", (szTurnFounded,)), ))
				self.iCitySizes[iRankLoop] = pCity.getPopulation()
				self.aaCitiesXY[iRankLoop] = [pCity.getX(), pCity.getY()]

				self.iNumCities += 1
			else:
				self.szCityNames[iRankLoop] = ""
				self.iCitySizes[iRankLoop] = -1
				self.szCityDescs[iRankLoop] = ""
				self.aaCitiesXY[iRankLoop] = [-1, -1]


#############################################################################################################
################################################### WONDERS #################################################
#############################################################################################################

	def determineListBoxContents(self):

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		# Fill wonders listbox

		iNumWondersBeingBuilt = len(self.aaWondersBeingBuilt)

		szWonderName = ""
		self.aiWonderListBoxIDs = []
		self.aiTurnYearBuilt = []
		self.aiWonderBuiltBy = []
		self.aszWonderCity = []

		if (self.szWonderDisplayMode == "Projects"):

	############### Create ListBox for Projects ###############

			for iWonderLoop in xrange(iNumWondersBeingBuilt):

				iProjectType = self.aaWondersBeingBuilt[iWonderLoop][0]
				pProjectInfo = GC.getProjectInfo(iProjectType)
				szProjectName = pProjectInfo.getDescription()

				self.aiWonderListBoxIDs.append(iProjectType)
				self.aiTurnYearBuilt.append(-6666)
				szWonderBuiltBy = self.aaWondersBeingBuilt[iWonderLoop][1]
				self.aiWonderBuiltBy.append(szWonderBuiltBy)
				szWonderCity = ""
				self.aszWonderCity.append(szWonderCity)

				screen.appendListBoxString( self.szWondersListBox, szProjectName + " (" + szWonderBuiltBy + ")", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0 )

			for iWonderLoop in xrange(self.iNumWonders):

				iProjectType = self.aaWondersBuilt[iWonderLoop][1]
				pProjectInfo = GC.getProjectInfo(iProjectType)
				szProjectName = pProjectInfo.getDescription()

				self.aiWonderListBoxIDs.append(iProjectType)
				self.aiTurnYearBuilt.append(-9999)
				szWonderBuiltBy = self.aaWondersBuilt[iWonderLoop][2]
				self.aiWonderBuiltBy.append(szWonderBuiltBy)
				szWonderCity = self.aaWondersBuilt[iWonderLoop][3]
				self.aszWonderCity.append(szWonderCity)

				screen.appendListBoxString( self.szWondersListBox, szProjectName + " (" + szWonderBuiltBy + ")", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0 )

		else:

	############### Create ListBox for Wonders ###############

			for iWonderLoop in xrange(iNumWondersBeingBuilt):

				iWonderType = self.aaWondersBeingBuilt[iWonderLoop][0]
				pWonderInfo = GC.getBuildingInfo(iWonderType)
				szWonderName = pWonderInfo.getDescription()

				self.aiWonderListBoxIDs.append(iWonderType)
				self.aiTurnYearBuilt.append(-9999)
				szWonderBuiltBy = self.aaWondersBeingBuilt[iWonderLoop][1]
				self.aiWonderBuiltBy.append(szWonderBuiltBy)
				szWonderCity = ""
				self.aszWonderCity.append(szWonderCity)

				screen.appendListBoxString( self.szWondersListBox, szWonderName + " (" + szWonderBuiltBy + ")", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0 )

			for iWonderLoop in xrange(self.iNumWonders):

				iWonderType = self.aaWondersBuilt[iWonderLoop][1]
				pWonderInfo = GC.getBuildingInfo(iWonderType)
				szWonderName = pWonderInfo.getDescription()

				self.aiWonderListBoxIDs.append(iWonderType)
				self.aiTurnYearBuilt.append(self.aaWondersBuilt[iWonderLoop][0])
				szWonderBuiltBy = self.aaWondersBuilt[iWonderLoop][2]
				self.aiWonderBuiltBy.append(szWonderBuiltBy)
				szWonderCity = self.aaWondersBuilt[iWonderLoop][3]
				self.aszWonderCity.append(szWonderCity)

				screen.appendListBoxString( self.szWondersListBox, szWonderName + " (" + szWonderBuiltBy + ")", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0 )

	def drawWondersList(self):

		self.szWondersListBox = self.getNextWidgetName()
		self.szWondersTable = self.getNextWidgetName()

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		if self.iNumWondersPermanentWidgets == 0:

			# Wonders List ListBox
			screen.addListBoxGFC(self.szWondersListBox, "", 766, 130, 194, 180, TableStyles.TABLE_STYLE_STANDARD)
			screen.setStyle(self.szWondersListBox, "Table_StandardCiv_Style")

			self.determineListBoxContents()

			self.iNumWondersPermanentWidgets = self.nWidgetCount

		# Stats Panel
		panelName = self.getNextWidgetName()
		screen.addPanel(panelName, "", "", True, True, 540, 90, 210, 220, PanelStyles.PANEL_STYLE_IN)

############################################### DISPLAY SINGLE WONDER ###############################################

		# Set default wonder if any exist in this list
		if (len(self.aiWonderListBoxIDs) > 0 and self.iWonderID == -1):
			self.iWonderID = self.aiWonderListBoxIDs[0]

		# Only display/do the following if a wonder is actively being displayed
		if (self.iWonderID > -1):

############################################### DISPLAY PROJECT MODE ###############################################

			if (self.szWonderDisplayMode == "Projects"):

				pProjectInfo = GC.getProjectInfo(self.iWonderID)

				# Stats panel (cont'd) - Name
				szProjectDesc = u"<font=3b>" + pProjectInfo.getDescription().upper() + u"</font>"
				szStatsText = szProjectDesc + "\n\n"

				# Say whether this project is built yet or not

				iTurnYear = self.aiTurnYearBuilt[self.iActiveWonderCounter]
				if (iTurnYear == -6666):	# -6666 used for wonders in progress
					szTempText = TRNSLTR.getText("TXT_KEY_BEING_BUILT", ())

				else:
					szTempText = TRNSLTR.getText("TXT_KEY_INFO_SCREEN_BUILT", ())

				szWonderDesc = "%s, %s" %(self.aiWonderBuiltBy[self.iActiveWonderCounter], szTempText)
				szStatsText += szWonderDesc + "\n"

				if (self.aszWonderCity[self.iActiveWonderCounter] != ""):
					szStatsText += self.aszWonderCity[self.iActiveWonderCounter] + "\n\n"
				else:
					szStatsText += "\n"

				if (pProjectInfo.getProductionCost() > 0):
					szCost = TRNSLTR.getText("TXT_KEY_PEDIA_COST", (GC.getActivePlayer().getProjectProductionNeeded(self.iWonderID),))
					szStatsText += szCost.upper() + (u"%c" % GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar()) + "\n"

				if (isWorldProject(self.iWonderID)):
					iMaxInstances = GC.getProjectInfo(self.iWonderID).getMaxGlobalInstances()
					szProjectType = TRNSLTR.getText("TXT_KEY_PEDIA_WORLD_PROJECT", ())
					if (iMaxInstances > 1):
						szProjectType += " " + TRNSLTR.getText("TXT_KEY_PEDIA_WONDER_INSTANCES", (iMaxInstances,))
					szStatsText += szProjectType.upper() + "\n"

				if (isTeamProject(self.iWonderID)):
					iMaxInstances = GC.getProjectInfo(self.iWonderID).getMaxTeamInstances()
					szProjectType = TRNSLTR.getText("TXT_KEY_PEDIA_TEAM_PROJECT", ())
					if (iMaxInstances > 1):
						szProjectType += " " + TRNSLTR.getText("TXT_KEY_PEDIA_WONDER_INSTANCES", (iMaxInstances,))
					szStatsText += szProjectType.upper()

				screen.addMultilineText(self.getNextWidgetName(), szStatsText, 545, 105, 200, 200, WidgetTypes.WIDGET_GENERAL, 1, 2, 1<<2)

				# Add Graphic
				screen.addDDSGFC(self.getNextWidgetName(), GC.getProjectInfo(self.iWonderID).getButton(), 686, 356, 128, 128, WidgetTypes.WIDGET_GENERAL, 1, 2)

				# Special Abilities ListBox

				szSpecialTitle = u"<font=3b>" + TRNSLTR.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()) + u"</font>"
				self.szSpecialTitleWidget = self.getNextWidgetName()
				screen.setText(self.szSpecialTitleWidget, "", szSpecialTitle, 1<<0, 540, 517, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

				panelName = self.getNextWidgetName()
				screen.addPanel(panelName, "", "", True, True, 540, 545, 420, 155, PanelStyles.PANEL_STYLE_IN)

				listName = self.getNextWidgetName()
				screen.attachListBoxGFC( panelName, listName, "", TableStyles.TABLE_STYLE_EMPTY )
				screen.enableSelect(listName, False)

				szSpecialText = CyGameTextMgr().getProjectHelp(self.iWonderID, True, None)
				splitText = string.split( szSpecialText, "\n" )
				for special in splitText:
					if special:
						screen.appendListBoxString(listName, special, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

			else:

	############################################### DISPLAY WONDER MODE ###############################################

				pWonderInfo = GC.getBuildingInfo(self.iWonderID)

				# Stats panel (cont'd) - Name
				szWonderDesc = u"<font=3b>" + GC.getBuildingInfo(self.iWonderID).getDescription().upper() + u"</font>"
				szStatsText = szWonderDesc + "\n\n"

				# Wonder built-in year
				iTurnYear = self.aiTurnYearBuilt[self.iActiveWonderCounter]#self.aaWondersBuilt[self.iActiveWonderCounter][0]#.append([0,iProjectLoop,""]

				szDateBuilt = ""

				if (iTurnYear != -9999):	# -9999 used for wonders in progress
					if (iTurnYear < 0):
						szTurnFounded = TRNSLTR.getText("TXT_KEY_TIME_BC", (-iTurnYear,))
					else:
						szTurnFounded = TRNSLTR.getText("TXT_KEY_TIME_AD", (iTurnYear,))

					szDateBuilt = (", %s" %(szTurnFounded))

				else:
					szDateBuilt = (", %s" %(TRNSLTR.getText("TXT_KEY_BEING_BUILT", ())))

				szWonderDesc = "%s%s" %(self.aiWonderBuiltBy[self.iActiveWonderCounter], szDateBuilt)
				szStatsText += szWonderDesc + "\n"

				if (self.aszWonderCity[self.iActiveWonderCounter] != ""):
					szStatsText += self.aszWonderCity[self.iActiveWonderCounter] + "\n\n"
				else:
					szStatsText += "\n"

				# Building attributes

				if (pWonderInfo.getProductionCost() > 0):
					szCost = TRNSLTR.getText("TXT_KEY_PEDIA_COST", (GC.getActivePlayer().getBuildingProductionNeeded(self.iWonderID),))
					szStatsText += szCost.upper() + (u"%c" % GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar()) + "\n"

				for k in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
					if (pWonderInfo.getObsoleteSafeCommerceChange(k) != 0):
						if (pWonderInfo.getObsoleteSafeCommerceChange(k) > 0):
							szSign = "+"
						else:
							szSign = ""

						szCommerce = GC.getCommerceInfo(k).getDescription() + ": "

						szText1 = szCommerce.upper() + szSign + str(pWonderInfo.getObsoleteSafeCommerceChange(k))
						szText2 = szText1 + (u"%c" % (GC.getCommerceInfo(k).getChar()))
						szStatsText += szText2 + "\n"

				if (pWonderInfo.getHappiness() > 0):
					szText = TRNSLTR.getText("TXT_KEY_PEDIA_HAPPY", (pWonderInfo.getHappiness(),))
					szStatsText += szText + (u"%c" % GAME.getSymbolID(FontSymbols.HAPPY_CHAR)) + "\n"

				elif (pWonderInfo.getHappiness() < 0):
					szText = TRNSLTR.getText("TXT_KEY_PEDIA_UNHAPPY", (-pWonderInfo.getHappiness(),))
					szStatsText += szText + (u"%c" % GAME.getSymbolID(FontSymbols.UNHAPPY_CHAR)) + "\n"

				if (pWonderInfo.getHealth() > 0):
					szText = TRNSLTR.getText("TXT_KEY_PEDIA_HEALTHY", (pWonderInfo.getHealth(),))
					szStatsText += szText + (u"%c" % GAME.getSymbolID(FontSymbols.HEALTHY_CHAR)) + "\n"

				elif (pWonderInfo.getHealth() < 0):
					szText = TRNSLTR.getText("TXT_KEY_PEDIA_UNHEALTHY", (-pWonderInfo.getHealth(),))
					szStatsText += szText + (u"%c" % GAME.getSymbolID(FontSymbols.UNHEALTHY_CHAR)) + "\n"

				if (pWonderInfo.getGreatPeopleRateChange() != 0):
					szText = TRNSLTR.getText("TXT_KEY_PEDIA_GREAT_PEOPLE", (pWonderInfo.getGreatPeopleRateChange(),))
					szStatsText += szText + (u"%c" % GAME.getSymbolID(FontSymbols.GREAT_PEOPLE_CHAR)) + "\n"

				screen.addMultilineText(self.getNextWidgetName(), szStatsText, 545, 105, 200, 200, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)

				# Add Graphic
				screen.addBuildingGraphicGFC(self.getNextWidgetName(), self.iWonderID, 540, 325, 420, 190,
					WidgetTypes.WIDGET_GENERAL, -1, -1, -20, 30, .5, True)

				# Special Abilities ListBox

				szSpecialTitle = u"<font=3b>" + TRNSLTR.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()) + u"</font>"
				self.szSpecialTitleWidget = self.getNextWidgetName()
				screen.setText(self.szSpecialTitleWidget, "", szSpecialTitle, 1<<0, 540, 517, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

				panelName = self.getNextWidgetName()
				screen.addPanel(panelName, "", "", True, True, 540, 545, 420, 155, PanelStyles.PANEL_STYLE_IN)

				listName = self.getNextWidgetName()
				screen.attachListBoxGFC( panelName, listName, "", TableStyles.TABLE_STYLE_EMPTY )
				screen.enableSelect(listName, False)

				szSpecialText = CyGameTextMgr().getBuildingHelp(self.iWonderID, True, False, False, None, False)
				splitText = string.split( szSpecialText, "\n" )
				for special in splitText:
					if len( special ) != 0:
						screen.appendListBoxString( listName, special, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0 )

	def calculateWondersList(self):

		self.aaWondersBeingBuilt = []
		self.aaWondersBuilt = []
		self.iNumWonders = 0

		self.player = GC.getPlayer(GAME.getActivePlayer())

		# Loop through players to determine Wonders.
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):

			playerX = GC.getPlayer(iPlayerX)
			iTeamX = playerX.getTeam()

			# Only display national wonders for the active player's team
			if self.szWonderDisplayMode == "NationalWonders" and iTeamX != GC.getPlayer(self.iPlayer).getTeam():
				continue

			# Loop through this player's cities and determine if they have any wonders to display
			for cityX in GC.getPlayer(iPlayerX).cities():

				pCityPlot = CyMap().plot(cityX.getX(), cityX.getY())

				# Check to see if active player can see this city
				szCityName = ""
				if (pCityPlot.isActiveVisible(False)):
					szCityName = cityX.getName()

				# Loop through projects to find any under construction
				if (self.szWonderDisplayMode == "Projects"):
					for iProjectLoop in xrange(GC.getNumProjectInfos()):

						iProjectProd = cityX.getProductionProject()
						pProject = GC.getProjectInfo(iProjectLoop)

						# Project is being constructed
						if (iProjectProd == iProjectLoop):

							# Project Mode
							if iTeamX == GC.getPlayer(self.iPlayer).getTeam():

								self.aaWondersBeingBuilt.append([iProjectProd, playerX.getCivilizationShortDescription(0)])

				# Loop through buildings
				else:

					for iBuildingLoop in xrange(GC.getNumBuildingInfos()):

						iBuildingProd = cityX.getProductionBuilding()

						pBuilding = GC.getBuildingInfo(iBuildingLoop)

						# World Wonder Mode
						if self.szWonderDisplayMode == "WorldWonders" and isWorldWonder(iBuildingLoop):

							# Is this city building a wonder? Only show our wonders under construction.
							if iBuildingProd == iBuildingLoop and iTeamX == GC.getPlayer(self.iPlayer).getTeam():
								self.aaWondersBeingBuilt.append([iBuildingProd, playerX.getCivilizationShortDescription(0)])

							if (cityX.getNumBuilding(iBuildingLoop) > 0):
								if (iTeamX == GC.getPlayer(self.iPlayer).getTeam() or GC.getTeam(GC.getPlayer(self.iPlayer).getTeam()).isHasMet(iTeamX)):
									self.aaWondersBuilt.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,playerX.getCivilizationShortDescription(0),szCityName])
								else:
									self.aaWondersBuilt.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,TRNSLTR.getText("TXT_KEY_UNKNOWN", ()),TRNSLTR.getText("TXT_KEY_UNKNOWN", ())])

								self.iNumWonders += 1

						# National/Team Wonder Mode
						elif self.szWonderDisplayMode == "NationalWonders" and (isNationalWonder(iBuildingLoop) or isTeamWonder(iBuildingLoop)):

							# Is this city building a wonder? Only show our wonders under construction.
							if iBuildingProd == iBuildingLoop and iTeamX == GC.getPlayer(self.iPlayer).getTeam():
								self.aaWondersBeingBuilt.append([iBuildingProd, playerX.getCivilizationShortDescription(0)])

							if cityX.getNumBuilding(iBuildingLoop) > 0:

								if iTeamX == GC.getPlayer(self.iPlayer).getTeam() or GC.getTeam(GC.getPlayer(self.iPlayer).getTeam()).isHasMet(iTeamX):
									self.aaWondersBuilt.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,playerX.getCivilizationShortDescription(0), szCityName])
								else:
									self.aaWondersBuilt.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,TRNSLTR.getText("TXT_KEY_UNKNOWN", ()), TRNSLTR.getText("TXT_KEY_UNKNOWN", ())])
								self.iNumWonders += 1

		# This array used to store which players have already used up a team's slot so team projects don't get added to list more than once
		aiTeamsUsed = []

		# Project Mode
		if (self.szWonderDisplayMode == "Projects"):

			# Loop through players to determine Projects
			for iPlayerLoop in xrange(GC.getMAX_PLAYERS()):

				pPlayer = GC.getPlayer(iPlayerLoop)
				iTeamLoop = pPlayer.getTeam()

				# Block duplicates
				if (iTeamLoop not in aiTeamsUsed):

					aiTeamsUsed.append(iTeamLoop)
					pTeam = GC.getTeam(iTeamLoop)

					if (pTeam.isAlive() and not pTeam.isNPC()):

						# Loop through projects
						for iProjectLoop in xrange(GC.getNumProjectInfos()):

							for iI in xrange(pTeam.getProjectCount(iProjectLoop)):

								if (iTeamLoop == GC.getPlayer(self.iPlayer).getTeam() or GC.getTeam(GC.getPlayer(self.iPlayer).getTeam()).isHasMet(iTeamLoop)):
									self.aaWondersBuilt.append([-9999,iProjectLoop,GC.getPlayer(iPlayerLoop).getCivilizationShortDescription(0),szCityName])
								else:
									self.aaWondersBuilt.append([-9999,iProjectLoop,TRNSLTR.getText("TXT_KEY_UNKNOWN", ()),TRNSLTR.getText("TXT_KEY_UNKNOWN", ())])
								self.iNumWonders += 1

		# Sort wonders in order of date built
		self.aaWondersBuilt.sort()
		self.aaWondersBuilt.reverse()

#		print("List of wonders/projects Built:")
#		print(self.aaWondersBuilt)

	def calculateWondersList_BUG(self):

		self.aaWondersBeingBuilt_BUG = []
		self.aaWondersBuilt_BUG = []
		self.iNumWonders = 0

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
								self.aaWondersBeingBuilt_BUG.append([iProjectLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

							if (self.team.isHasMet(iTeamX)
							and self.player.canDoEspionageMission(self.iInvestigateCityMission, cityX.getOwner(), cityX.plot(), -1)
							and cityX.isRevealed(GAME.getActiveTeam(), False)
							):
								self.aaWondersBeingBuilt_BUG.append([iProjectLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

				# Loop through buildings
				else:
					for iBuildingLoop in xrange(GC.getNumBuildingInfos()):
						iBuildingProd = cityX.getProductionBuilding()

						# World Wonder Mode
						if (self.szWonderDisplayMode == "WorldWonders" and isWorldWonder(iBuildingLoop)):
							# Is this city building a wonder?
							if (iBuildingProd == iBuildingLoop):
								if (iTeamX == self.iTeam):
									self.aaWondersBeingBuilt_BUG.append([iBuildingLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

								if (self.team.isHasMet(iTeamX)
								and self.player.canDoEspionageMission(self.iInvestigateCityMission, cityX.getOwner(), cityX.plot(), -1)
								and cityX.isRevealed(GAME.getActiveTeam(), False)):
									self.aaWondersBeingBuilt_BUG.append([iBuildingLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

							if (cityX.getNumBuilding(iBuildingLoop) > 0):
								if (iTeamX == self.iTeam or self.team.isHasMet(iTeamX)):
									self.aaWondersBuilt_BUG.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,True, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])
								else:
									self.aaWondersBuilt_BUG.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,False,TRNSLTR.getText("TXT_KEY_UNKNOWN", ()), cityX, 18])

								self.iNumWonders += 1

						# National/Team Wonder Mode
						elif self.szWonderDisplayMode == "NationalWonders" and (isNationalWonder(iBuildingLoop) or isTeamWonder(iBuildingLoop)):

							# Is this city building a wonder?
							if (iBuildingProd == iBuildingLoop):
								# Only show our wonders under construction
								if (iTeamX == self.iTeam):
									self.aaWondersBeingBuilt_BUG.append([iBuildingLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

								if (self.team.isHasMet(iTeamX)
								and self.player.canDoEspionageMission(self.iInvestigateCityMission, cityX.getOwner(), cityX.plot(), -1)
								and cityX.isRevealed(GAME.getActiveTeam(), False)):
									self.aaWondersBeingBuilt_BUG.append([iBuildingLoop, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])

							# Has this city built a wonder?
							if (cityX.getNumBuilding(iBuildingLoop) > 0):
								if (iTeamX == self.iTeam):
									self.aaWondersBuilt_BUG.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,True, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])
									self.iNumWonders += 1

								elif self.team.isHasMet(iTeamX) and cityX.isRevealed(GAME.getActiveTeam(), False):

									self.aaWondersBuilt_BUG.append([cityX.getBuildingOriginalTime(iBuildingLoop),iBuildingLoop,True, playerX.getCivilizationShortDescription(0), cityX, iPlayerX])
									self.iNumWonders += 1

		# This array used to store which players have already used up a team's slot so team projects don't get added to list more than once
		aiTeamsUsed = []

		# Project Mode
		if (self.szWonderDisplayMode == "Projects"):

			# Loop through players to determine Projects
			for iPlayerLoop in xrange(GC.getMAX_PLAYERS()):

				pPlayer = GC.getPlayer(iPlayerLoop)
				iTeamLoop = pPlayer.getTeam()

				# Block duplicates
				if (iTeamLoop not in aiTeamsUsed):

					aiTeamsUsed.append(iTeamLoop)
					pTeam = GC.getTeam(iTeamLoop)

					if (pTeam.isAlive() and not pTeam.isNPC()):

						# Loop through projects
						for iProjectLoop in xrange(GC.getNumProjectInfos()):

							for iI in xrange(pTeam.getProjectCount(iProjectLoop)):

								if (iTeamLoop == self.iTeam or self.team.isHasMet(iTeamLoop)):
									self.aaWondersBuilt_BUG.append([-9999,iProjectLoop,True,GC.getPlayer(iPlayerLoop).getCivilizationShortDescription(0),None, iPlayerLoop])
								else:
									self.aaWondersBuilt_BUG.append([-9999,iProjectLoop,False,TRNSLTR.getText("TXT_KEY_UNKNOWN", ()),None, 9999])
								self.iNumWonders += 1

		# Sort wonders in order of date built
		self.aaWondersBuilt_BUG.sort()
		self.aaWondersBuilt_BUG.reverse()

#		print("List of wonders/projects Built:")
#		print(self.aaWondersBuilt)

	def drawWondersList_BUG(self):

		self.szWondersListBox = self.getNextWidgetName()
		self.szWondersTable = self.getNextWidgetName()

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)
		screen.addTableControlGFC(self.szWondersTable, 5, 540, 130, 420, 540, True, True, 24,24, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSort(self.szWondersTable)

		zoomArt = self.zoomArt

		sName = BugUtil.getPlainText("TXT_KEY_NAME")
		sDate = BugUtil.getPlainText("TXT_KEY_WONDER_DATE")
		sOwner = BugUtil.getPlainText("TXT_KEY_WONDER_OWNER")
		sCity = BugUtil.getPlainText("TXT_KEY_WONDER_CITY")

		screen.setTableColumnHeader(self.szWondersTable, 0, "", 30)
		screen.setTableColumnHeader(self.szWondersTable, 1, sName, 115)
		screen.setTableColumnHeader(self.szWondersTable, 2, sDate, 70)
		screen.setTableColumnHeader(self.szWondersTable, 3, sOwner, 100)
		screen.setTableColumnHeader(self.szWondersTable, 4, sCity, 100)

		iWBB = len(self.aaWondersBeingBuilt_BUG)

		for iWonderLoop in xrange(iWBB):

#			self.aaWondersBeingBuilt_BUG contains the following:
			iWonderType = self.aaWondersBeingBuilt_BUG[iWonderLoop][0]
			szWonderBuiltBy = self.aaWondersBeingBuilt_BUG[iWonderLoop][1]
			pCity = self.aaWondersBeingBuilt_BUG[iWonderLoop][2]
			iPlayer = self.aaWondersBeingBuilt_BUG[iWonderLoop][3]

			color = -1
			ePlayerColor = GC.getPlayer(iPlayer).getPlayerColor()
			if ePlayerColor != -1:
				playerColor = GC.getPlayerColorInfo(ePlayerColor)
				if playerColor:
					color = playerColor.getColorTypePrimary()

			if (self.szWonderDisplayMode == "Projects"):
				pWonderInfo = GC.getProjectInfo(iWonderType)
				iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT
			else:
				pWonderInfo = GC.getBuildingInfo(iWonderType)
				iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING

			szWonderName = pWonderInfo.getDescription()
			szTurnYearBuilt = u"<font=2>%c</font>" % GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar()

			# Check to see if active player can see this city
			if pCity and pCity.isRevealed(GAME.getActiveTeam(), False):
				szCityName = pCity.getName()
			else:
				szCityName = u""

			if AdvisorOpt.isWonderListUsePlayerColor():
				szWonderBuiltBy = TRNSLTR.changeTextColor(szWonderBuiltBy, color)
				szCityName = TRNSLTR.changeTextColor(szCityName, color)

			screen.appendTableRow(self.szWondersTable)
			screen.setTableText(self.szWondersTable, 0, iWonderLoop, ""             , zoomArt, WidgetTypes.WIDGET_ZOOM_CITY, pCity.getOwner(), pCity.getID(), 1<<0)
			screen.setTableText(self.szWondersTable, 1, iWonderLoop, szWonderName   , "", iWidget, iWonderType, -1, 1<<0)
			screen.setTableInt (self.szWondersTable, 2, iWonderLoop, szTurnYearBuilt, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)
			screen.setTableText(self.szWondersTable, 3, iWonderLoop, szWonderBuiltBy, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
			screen.setTableText(self.szWondersTable, 4, iWonderLoop, szCityName     , "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		for iWonderLoop in xrange(self.iNumWonders):

#			self.aaWondersBuilt_BUG contains the following:
			iTurnYearBuilt = self.aaWondersBuilt_BUG[iWonderLoop][0]
			iWonderType = self.aaWondersBuilt_BUG[iWonderLoop][1]
			bKnown = self.aaWondersBuilt_BUG[iWonderLoop][2]
			szWonderBuiltBy = self.aaWondersBuilt_BUG[iWonderLoop][3]
			pCity = self.aaWondersBuilt_BUG[iWonderLoop][4]
			iPlayer = self.aaWondersBuilt_BUG[iWonderLoop][5]

			color = -1
			ePlayerColor = GC.getPlayer(iPlayer).getPlayerColor()
			if ePlayerColor != -1:
				playerColor = GC.getPlayerColorInfo(ePlayerColor)
				if playerColor:
					color = playerColor.getColorTypePrimary()

			if (self.szWonderDisplayMode == "Projects"):
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
			if pCity and pCity.isRevealed(GAME.getActiveTeam(), False):
				szCityName = pCity.getName()
			else:
				szCityName = u""

			if AdvisorOpt.isWonderListUsePlayerColor():
				szWonderBuiltBy = TRNSLTR.changeTextColor(szWonderBuiltBy, color)
				szCityName = TRNSLTR.changeTextColor(szCityName, color)

			screen.appendTableRow(self.szWondersTable)
			if bKnown and pCity and pCity.isRevealed(GAME.getActiveTeam(), False):
				screen.setTableText(self.szWondersTable, 0, iWonderLoop+iWBB, "", zoomArt, WidgetTypes.WIDGET_ZOOM_CITY, pCity.getOwner(), pCity.getID(), 1<<0)
			screen.setTableText(self.szWondersTable, 1, iWonderLoop+iWBB, szWonderName   , "", iWidget, iWonderType, -1, 1<<0)
			screen.setTableInt (self.szWondersTable, 2, iWonderLoop+iWBB, szTurnYearBuilt, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<1)
			screen.setTableText(self.szWondersTable, 3, iWonderLoop+iWBB, szWonderBuiltBy, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
			screen.setTableText(self.szWondersTable, 4, iWonderLoop+iWBB, szCityName     , "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)


#############################################################################################################
################################################## STATISTICS ###############################################
#############################################################################################################

	def drawStatsTab(self):

		# Bottom Chart
		if AdvisorOpt.isShowImprovements():
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

		if AdvisorOpt.isShowImprovements():
			szImprovementsTable = self.getNextWidgetName()
			screen.addTableControlGFC(szImprovementsTable, 2, 45 + self.W_STATS_BOTTOM_CHART_UNITS + self.W_STATS_BOTTOM_CHART_BUILDINGS, 280, 220, 410, True, True, 32,32, TableStyles.TABLE_STYLE_STANDARD)
			screen.enableSort(szImprovementsTable)

		# Reducing the width a bit to leave room for the vertical scrollbar, preventing a horizontal scrollbar from also being created

		if AdvisorOpt.isShowImprovements():
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


		if AdvisorOpt.isShowImprovements():
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

		if AdvisorOpt.isShowImprovements():
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
			if playerX.isEverAlive() and (team.isHasMet(iTeamX) or GAME.isDebugMode() or iEndGame):

				if (iEndGame or iTeamX == iTeam or player.canDoEspionageMission(self.iDemographicsMission, iPlayerX, None, -1)):
					self.aiPlayersMet.append(iPlayerX)
					self.iNumPlayersMet += 1
				else:
					self.aiPlayersMetNAEspionage.append(iPlayerX)
					self.iNumPlayersMetNAEspionage += 1


	def drawLine(self, screen, canvas, x0, y0, x1, y1, color, bThreeLines):
		if bThreeLines:
			screen.addLineGFC(canvas, self.getNextLineName(), x0, y0 + 1, x1, y1 + 1, color)
			screen.addLineGFC(canvas, self.getNextLineName(), x0 + 1, y0, x1 + 1, y1, color)
		screen.addLineGFC(canvas, self.getNextLineName(), x0, y0, x1, y1, color)

	def getLog10(self, x):
		return math.log10(max(1, x))

	def lineName(self,i):
		return "DemoLine" + str(i)

	def getNextLineName(self):
		name = self.lineName(self.nLineCount)
		self.nLineCount += 1
		return name

	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = "DemoScreenWidget" + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def deleteAllLines(self):
		screen = CyGInterfaceScreen("InfoScreen", self.screenId)
		i = 0
		while (i < self.nLineCount):
			screen.deleteWidget(self.lineName(i))
			i += 1
		self.nLineCount = 0

	def deleteAllWidgets(self, iNumPermanentWidgets = 0):
		self.deleteAllLines()
		screen = CyGInterfaceScreen("InfoScreen", self.screenId)
		i = self.nWidgetCount - 1
		while (i >= iNumPermanentWidgets):
			self.nWidgetCount = i
			screen.deleteWidget(self.getNextWidgetName())
			i -= 1

		self.nWidgetCount = iNumPermanentWidgets
		self.yMessage = 5

		screen.deleteWidget("WorldWondersWidget")
		screen.deleteWidget("NationalWondersWidget")
		screen.deleteWidget("ProjectsWidget")

	# handle the input for this screen...
	def handleInput(self, inputClass):

		screen = CyGInterfaceScreen("InfoScreen", self.screenId)

		iCode	= inputClass.eNotifyCode
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName

		szWidgetName = NAME + str(ID)

		# Slide graph
		if szWidgetName == self.graphLeftButtonID and iCode == NotifyCode.NOTIFY_CLICKED:
			self.slideGraph(- 2 * self.iGraphZoom / 5)
			self.drawGraphs()

		elif szWidgetName == self.graphRightButtonID and iCode == NotifyCode.NOTIFY_CLICKED:
			self.slideGraph(2 * self.iGraphZoom / 5)
			self.drawGraphs()

		if iCode == 11: # List Select

			# Debug dropdown
			if (NAME == "Info_DebugDD"):
				iIndex = screen.getSelectedPullDownID("Info_DebugDD")
				self.iPlayer = screen.getPullDownData("Info_DebugDD", iIndex)
				self.determineKnownPlayers()

				# Force recache of all scores
				self.scoreCache = []
				for t in self.RANGE_SCORES:
					self.scoreCache.append(None)
				self.redrawContents()

			iSelected = inputClass.getData()

			if self.iTab == 2:

				# Wonder type dropdown box
				if (szWidgetName == self.szWondersDropdownWidget
				or NAME in ("WorldWondersWidget", "NationalWondersWidget", "ProjectsWidget")
				):
					self.handleInput_Wonders(inputClass)

				# Wonders ListBox
				elif szWidgetName == self.szWondersListBox:
					if not AdvisorOpt.isShowInfoWonders():
						self.reset()
						self.iWonderID = self.aiWonderListBoxIDs[iSelected]
						self.iActiveWonderCounter = iSelected
						self.deleteAllWidgets(self.iNumWondersPermanentWidgets)
						self.drawWondersList()

				# BUG Wonders table
				elif szWidgetName == self.szWondersTable and not inputClass.getMouseX():
					screen.hideScreen()
					pPlayer = GC.getPlayer(inputClass.getData1())
					pCity = pPlayer.getCity(inputClass.getData2())
					CyCamera().JustLookAtPlot(pCity.plot())


			elif not self.iTab:

				# Graph dropdown to select what values are being graphed
				if (szWidgetName == self.szGraphDropdownWidget):

					if (iSelected == 0):
						self.iGraphTabID = self.TOTAL_SCORE

					elif (iSelected == 1):
						self.iGraphTabID = self.ECONOMY_SCORE

					elif (iSelected == 2):
						self.iGraphTabID = self.INDUSTRY_SCORE

					elif (iSelected == 3):
						self.iGraphTabID = self.AGRICULTURE_SCORE

					elif (iSelected == 4):
						self.iGraphTabID = self.POWER_SCORE

					elif (iSelected == 5):
						self.iGraphTabID = self.CULTURE_SCORE

					elif (iSelected == 6):
						self.iGraphTabID = self.ESPIONAGE_SCORE
					#RevolutionDCM
					elif (iSelected == 7):
						self.iGraphTabID = self.TOTAL_SCORE

					self.drawGraphs()

				elif szWidgetName == self.szTurnsDropdownWidget:

					self.iGraphZoom = self.dropDownTurns[iSelected]
					self.checkGraphBounds()
					self.updateGraphButtons()

					self.drawGraphs()

				elif (szWidgetName == self.szGraphSmoothingDropdownWidget_1in1):
					self.iGraph_Smoothing_1in1 = iSelected
					self.drawGraphs()

				elif (szWidgetName == self.szGraphSmoothingDropdownWidget_7in1):
					self.iGraph_Smoothing_7in1 = iSelected
					self.drawGraphs()

				for i in xrange(3):
					if (szWidgetName == self.szGraphDropdownWidget_3in1[i]):
						self.iGraph_3in1[i] = iSelected
						self.drawGraphs()

		# Something Clicked
		elif iCode == NotifyCode.NOTIFY_CLICKED:

			######## Screen 'Tabs' for Navigation ########

			if NAME == "VS_Tab":
				screen.hide("VS_Tab_Act" + str(self.iTab))
				screen.show("VS_Tab" + str(self.iTab))
				self.iTab = ID
				self.reset()
				self.redrawContents()

			elif NAME in ("WorldWondersWidget", "NationalWondersWidget", "ProjectsWidget"):
				# Wonder type dropdown box
				self.handleInput_Wonders(inputClass)


			if AdvisorOpt.isGraphs():

				for i in xrange(self.NUM_SCORES):
					if (szWidgetName == self.sGraphTextHeadingWidget[i]
					or (szWidgetName == self.sGraphBGWidget[i] and iCode == NotifyCode.NOTIFY_CLICKED)):
						if self.Graph_Status_Current == self.Graph_Status_1in1:
							self.Graph_Status_Current = self.Graph_Status_Prior
							self.Graph_Status_Prior = self.Graph_Status_1in1
						else:
							self.Graph_Status_Prior = self.Graph_Status_Current
							self.Graph_Status_Current = self.Graph_Status_1in1
						self.iGraphTabID = i
						self.drawGraphs()
						break

					elif szWidgetName == self.sGraphTextBannerWidget[i]:
						if self.iGraphTabID == i:
							self.Graph_Status_Current = self.Graph_Status_7in1
						self.iGraphTabID = i
						self.drawGraphs()
						break

				if szWidgetName == self.sGraph1in1:
					self.Graph_Status_Current = self.Graph_Status_1in1
					self.Graph_Status_Prior = self.Graph_Status_Current
					self.drawGraphs()
				elif szWidgetName == self.sGraph3in1:
					self.Graph_Status_Current = self.Graph_Status_3in1
					self.Graph_Status_Prior = self.Graph_Status_Current
					self.drawGraphs()
				elif szWidgetName == self.sGraph7in1:
					self.Graph_Status_Current = self.Graph_Status_7in1
					self.Graph_Status_Prior = self.Graph_Status_Current
					self.drawGraphs()

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


	def handleInput_Wonders(self, inputClass):
		szShortWidgetName = inputClass.getFunctionName()
		szWidgetName = inputClass.getFunctionName() + str(inputClass.getID())
		code = inputClass.getNotifyCode()
		iSelected = inputClass.getData()

		# Reset wonders stuff so that when the type shown changes the old contents don't mess with things

		self.iNumWonders = 0
		self.iActiveWonderCounter = 0
		self.iWonderID = -1
		self.aaWondersBuilt = []
		self.aaWondersBuilt_BUG = []

		self.aaWondersBeingBuilt = []
		self.aaWondersBeingBuilt_BUG = []

		if szWidgetName == self.szWondersDropdownWidget:
			if iSelected == 0:
				self.szWonderDisplayMode = "WorldWonders"

			elif iSelected == 1:
				self.szWonderDisplayMode = "NationalWonders"

			elif iSelected == 2:
				self.szWonderDisplayMode = "Projects"

		elif szShortWidgetName == "WorldWondersWidget":
			self.szWonderDisplayMode = "WorldWonders"

		elif szShortWidgetName == "NationalWondersWidget":
			self.szWonderDisplayMode = "NationalWonders"

		elif szShortWidgetName == "ProjectsWidget":
			self.szWonderDisplayMode = "Projects"

		self.reset()

		self.calculateWondersList()
		if not AdvisorOpt.isShowInfoWonders():
			self.determineListBoxContents()

		# Change selected wonder to the one at the top of the new list
		if self.iNumWonders > 0 and not AdvisorOpt.isShowInfoWonders():
			self.iWonderID = self.aiWonderListBoxIDs[0]

		self.redrawContents()


	def update(self, fDelta): return

	def onClose(self):
		screen = CyGInterfaceScreen("InfoScreen", self.screenId)
		screen.setDying(True)
		# Reset Wonders so nothing lingers next time the screen is opened
		self.szWonderDisplayMode = "WorldWonders"
		self.iWonderID = -1
		self.iActiveWonderCounter = 0
		self.aiWonderListBoxIDs = []
		self.aiTurnYearBuilt = []
		self.aiWonderBuiltBy = []
		self.aszWonderCity = []
