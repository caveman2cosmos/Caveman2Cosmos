## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvScreenEnums

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvDanQuayle:

	def __init__(self):
		self.SCREEN_NAME = "CvDanQuayle"
		self.WIDGET_ID = "CvDanQuayleWidget"
		self.EXIT_ID = "CvDanQuayleExitWidget"
		self.BACKGROUND_ID = "CvDanQuayleBackground"
		self.LEADERHEAD_ID = "CvDanQuayleLeaderhead"
		self.LIST_ID = "CvDanQuayleList"
		self.TEXT_ID = "CvDanQuayleText"
		self.SCORE_ID = "CvDanQuayleScore"
		self.SCORE_ID = "CvDanQuayleScore"
		self.WIDGET_HEADER = "CvDanQuayleHeader"

		self.X_SCREEN = 500
		self.Y_SCREEN = 396
		self.W_SCREEN = 1024
		self.H_SCREEN = 768
		self.Y_TITLE = 12
		self.X_EXIT = 994
		self.Y_EXIT = 726

		self.X_LEADERHEAD = 120
		self.Y_LEADERHEAD = 90
		self.W_LEADERHEAD = 390
		self.H_LEADERHEAD = 470

		self.X_LIST = 570
		self.Y_LIST = 170
		self.W_LIST = 300
		self.H_LIST = 520

		self.X_SCORE = 570
		self.Y_SCORE = 90
		self.W_SCORE = 300
		self.H_SCORE = 50

		self.X_TEXT = 120
		self.Y_TEXT = 590
		self.W_TEXT = 390
		self.H_TEXT = 100

		self.leaders = [
			"TXT_KEY_DQ_LEADER_NAME_1",
			"TXT_KEY_DQ_LEADER_NAME_2",
			"TXT_KEY_DQ_LEADER_NAME_3",
			"TXT_KEY_DQ_LEADER_NAME_4",
			"TXT_KEY_DQ_LEADER_NAME_5",
			"TXT_KEY_DQ_LEADER_NAME_6",
			"TXT_KEY_DQ_LEADER_NAME_7",
			"TXT_KEY_DQ_LEADER_NAME_8",
			"TXT_KEY_DQ_LEADER_NAME_9",
			"TXT_KEY_DQ_LEADER_NAME_10",
			"TXT_KEY_DQ_LEADER_NAME_11",
			"TXT_KEY_DQ_LEADER_NAME_12",
			"TXT_KEY_DQ_LEADER_NAME_13",
			"TXT_KEY_DQ_LEADER_NAME_14",
			"TXT_KEY_DQ_LEADER_NAME_15",
			"TXT_KEY_DQ_LEADER_NAME_16",
			"TXT_KEY_DQ_LEADER_NAME_17",
			"TXT_KEY_DQ_LEADER_NAME_18",
			"TXT_KEY_DQ_LEADER_NAME_19",
			"TXT_KEY_DQ_LEADER_NAME_20"
		]
		self.nWidgetCount = 0


	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.DAN_QUAYLE_SCREEN)

	def interfaceScreen (self):

		replayInfo = CyGame().getReplayInfo()
		if replayInfo.isNone():
			replayInfo = CyReplayInfo()
			replayInfo.createInfo(gc.getGame().getActivePlayer())

		screen = self.getScreen()
		if screen.isActive():
			return
		screen.setRenderInterfaceOnly(True)
		screen.showScreen( PopupStates.POPUPSTATE_IMMEDIATE, False)

		# Set the background and exit button, and show the screen
		screen.setDimensions(screen.centerX(0), screen.centerY(0), self.W_SCREEN, self.H_SCREEN)

		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addPanel( "TechTopPanel", u"", u"", True, False, 0, 0, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_TOPBAR )
		screen.addPanel( "TechBottomPanel", u"", u"", True, False, 0, 713, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_BOTTOMBAR )

		screen.showWindowBackground(False)
		screen.setText(self.EXIT_ID, "Background", u"<font=4>" + localText.getText("TXT_WORD_EXIT", ()).upper() + u"</font>", 1<<1, self.X_EXIT, self.Y_EXIT, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		# Header...
		screen.setLabel(self.WIDGET_HEADER, "Background", u"<font=4b>" + localText.getText("TXT_KEY_GAME_END_SCREEN_TITLE", ()).upper() + u"</font>", 1<<2, self.X_SCREEN, self.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Leaderhead
		screen.addLeaderheadGFC(self.LEADERHEAD_ID, replayInfo.getLeader(replayInfo.getActivePlayer()), AttitudeTypes.ATTITUDE_PLEASED, self.X_LEADERHEAD, self.Y_LEADERHEAD, self.W_LEADERHEAD, self.H_LEADERHEAD, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iScore = replayInfo.getNormalizedScore()
		iMaxScore = ((100 + gc.getDefineINT("SCORE_VICTORY_PERCENT")) * (gc.getDefineINT("SCORE_POPULATION_FACTOR") + gc.getDefineINT("SCORE_LAND_FACTOR") + gc.getDefineINT("SCORE_WONDER_FACTOR") + gc.getDefineINT("SCORE_TECH_FACTOR"))) / 100
		if iMaxScore > 0:
			iNormalScore = iScore/float(iMaxScore)
		else:
			iNormalScore = 0

		if iNormalScore > 1.5:
			szLeaderText = self.leaders[0]
		elif iNormalScore > 1.4:
			szLeaderText = self.leaders[1]
		elif iNormalScore > 1.3:
			szLeaderText = self.leaders[2]
		elif iNormalScore > 1.2:
			szLeaderText = self.leaders[3]
		elif iNormalScore > 1.1:
			szLeaderText = self.leaders[4]
		elif iNormalScore > 1.05:
			szLeaderText = self.leaders[5]
		elif iNormalScore > 1.0:
			szLeaderText = self.leaders[6]
		elif iNormalScore > 0.95:
			szLeaderText = self.leaders[7]
		elif iNormalScore > 0.9:
			szLeaderText = self.leaders[8]
		elif iNormalScore > 0.85:
			szLeaderText = self.leaders[9]
		elif iNormalScore > 0.8:
			szLeaderText = self.leaders[10]
		elif iNormalScore > 0.75:
			szLeaderText = self.leaders[11]
		elif iNormalScore > 0.7:
			szLeaderText = self.leaders[12]
		elif iNormalScore > 0.65:
			szLeaderText = self.leaders[13]
		elif iNormalScore > 0.6:
			szLeaderText = self.leaders[14]
		elif iNormalScore > 0.55:
			szLeaderText = self.leaders[15]
		elif iNormalScore > 0.5:
			szLeaderText = self.leaders[16]
		elif iNormalScore > 0.4:
			szLeaderText = self.leaders[17]
		elif iNormalScore > 0.3:
			szLeaderText = self.leaders[18]
		else:
			szLeaderText = self.leaders[19]

		screen.addPanel("", u"", u"", True, False, self.X_TEXT, self.Y_TEXT, self.W_TEXT, self.H_TEXT, PanelStyles.PANEL_STYLE_IN)
		screen.addMultilineText(self.TEXT_ID, localText.getText("TXT_KEY_DQ_TEXT_STRING", (replayInfo.getLeaderName(), szLeaderText, )), self.X_TEXT+5, self.Y_TEXT+5, self.W_TEXT-10, self.H_TEXT-10, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		screen.addPanel(self.SCORE_ID, u"", u"", True, False, self.X_SCORE, self.Y_SCORE, self.W_SCORE, self.H_SCORE, PanelStyles.PANEL_STYLE_IN)
		screen.setLabelAt("", self.SCORE_ID, u"<font=4>" + localText.getObjectText("TXT_KEY_VICTORY_SCORE", 0) + u" : " + unicode(iScore) + u"</font>", 1<<2, self.W_SCORE/2-10, 5, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.addListBoxGFC(self.LIST_ID, "", self.X_LIST, self.Y_LIST, self.W_LIST, self.H_LIST, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSelect(self.LIST_ID, False)
		for i in range(len(self.leaders)):
			szText = self.leaders[i]
			if (szLeaderText == szText):
				szText = localText.getColorText(szText, (), gc.getCOLOR_YELLOW())
			else:
				szText = localText.getText(szText, ())

			screen.appendListBoxString(self.LIST_ID, szText, WidgetTypes.WIDGET_GENERAL, 0, 0, 1<<0)


	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName


	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0

	def update(self, fDelta):
		return
