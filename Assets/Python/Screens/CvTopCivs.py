## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
# Author - Jon Shafer
# Top Civilizations screen

import random
from CvPythonExtensions import *

GC = CyGlobalContext()
TRNSLTR = CyTranslator()

class CvTopCivs:

	def __init__(self, screenId):
		self.screenId = screenId

	def showScreen(self):

		if CyGame().isPitbossHost():
			return

		# Text
		self.HistorianList = [
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN1", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN2", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN3", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN4", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN5", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN6", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN7", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN8", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN9", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN10", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_HISTORIAN11", ())
		]
		self.RankList = [
			TRNSLTR.getText("TXT_KEY_TOPCIVS_RANK1", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_RANK2", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_RANK3", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_RANK4", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_RANK5", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_RANK6", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_RANK7", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_RANK8", ())
		]
		self.TypeList = [
			TRNSLTR.getText("TXT_KEY_TOPCIVS_WEALTH", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_POWER", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_TECH", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_CULTURE", ()),
			TRNSLTR.getText("TXT_KEY_TOPCIVS_SIZE", ()),
		]
		# Randomly choose what category and what historian will be used
		szTypeRand = random.choice(self.TypeList)
		szHistorianRand = random.choice(self.HistorianList)

		# Create screen
		self.screen = CyGInterfaceScreen( "CvTopCivs", self.screenId)

		self.screen.setSound("AS2D_TOP_CIVS")
		self.screen.showScreen(PopupStates.POPUPSTATE_QUEUED, False)
		self.screen.showWindowBackground(False)
		self.screen.setDimensions(self.screen.centerX(0), self.screen.centerY(0), 1024, 768)

		# Create panels
		# Main
		self.screen.addPanel("TopCivsMainPanel", "", "", True, True, 200, 70, 650, 500, PanelStyles.PANEL_STYLE_MAIN)
		# Top
		self.screen.addPanel("TopCivsHeaderPanel", "", "", True, True, 215, 85, 620, 470, PanelStyles.PANEL_STYLE_DAWNBOTTOM)
		# Bottom
		self.screen.addPanel("TopCivsTextPanel", "", "", True, True, 230, 217, 590, 265, PanelStyles.PANEL_STYLE_DAWNTOP)
		szTxt = TRNSLTR.getText("TXT_WORD_EXIT", ())
		self.screen.setButtonGFC("Exit", szTxt, "", 410, 510, 220, 30, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)
		# Title Text
		szTxt = "<font=3>" + TRNSLTR.getText("TXT_KEY_TOPCIVS_TITLE", ())
		self.screen.setLabel("", "", szTxt, 1<<2, 525, 100, -2.0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		szTxt = TRNSLTR.getText("TXT_KEY_TOPCIVS_TEXT1", (szHistorianRand, )) + "\n" + TRNSLTR.getText("TXT_KEY_TOPCIVS_TEXT2", (szTypeRand, ))
		self.screen.addMultilineText( "InfoText1", szTxt, 215, 150, 620, 70, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)

		self.makeList(szTypeRand)

	def makeList(self, szType):

		# Determine the list of top civs

		# Storage list [iValue, iPlayer, CyPlayer]
		aTopCivList = []

		if szType == TRNSLTR.getText("TXT_KEY_TOPCIVS_TECH", ()):
			iNumTotalTechs = GC.getNumTechInfos()
		# Loop through all players
		for iPlayer in range(GC.getMAX_PC_PLAYERS()):
			CyPlayer = GC.getPlayer(iPlayer)

			if CyPlayer.isAlive():

				if szType == TRNSLTR.getText("TXT_KEY_TOPCIVS_WEALTH", ()):

					iGold = CyPlayer.getGold()
					aTopCivList.append([iGold, iPlayer, CyPlayer])

				if szType == TRNSLTR.getText("TXT_KEY_TOPCIVS_POWER", ()):

					aTopCivList.append([CyPlayer.getPower(), iPlayer, CyPlayer])

				if szType == TRNSLTR.getText("TXT_KEY_TOPCIVS_TECH", ()):
					iPlayerNumTechs = 0
					CyTeam = GC.getTeam(CyPlayer.getTeam())

					for iTech in range(iNumTotalTechs):

						if CyTeam.isHasTech(iTech):
							iPlayerNumTechs = iPlayerNumTechs + 1

					aTopCivList.append([iPlayerNumTechs, iPlayer, CyPlayer])

				if szType == TRNSLTR.getText("TXT_KEY_TOPCIVS_CULTURE", ()):

					aTopCivList.append([CyPlayer.getCulture(), iPlayer, CyPlayer])

				if szType == TRNSLTR.getText("TXT_KEY_TOPCIVS_SIZE", ()):

					aTopCivList.append([CyPlayer.getTotalLand(), iPlayer, CyPlayer])

		# Lowest to Highest
		aTopCivList.sort()
		# Switch it around - want the best to be first
		aTopCivList.reverse()

		iPlayerActive = CyGame().getActivePlayer()
		CyTeamActive = GC.getTeam(GC.getPlayer(iPlayerActive).getTeam())
		# Print out the list
		y = 230
		for i in range(8):

			if i > len(aTopCivList)-1:
				return

			iPlayer = aTopCivList[i][1]
			CyPlayer = aTopCivList[i][2]

			# Does the Active player know this player exists?
			if iPlayer == iPlayerActive or CyTeamActive.isHasMet(CyPlayer.getTeam()):
				szTxt = TRNSLTR.getText("TXT_KEY_TOPCIVS_TEXT3", (CyPlayer.getNameKey(), self.RankList[i]))
			else: szTxt = TRNSLTR.getText("TXT_KEY_TOPCIVS_UNKNOWN", ())

			szTxt = "%d) %s" % (i + 1, szTxt)
			#self.screen.setText(szWidgetName, "", szTxt, 1<<0, 430, y, TEXT_Z, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			self.screen.addMultilineText("", szTxt, 380, y, 400, 30, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
			y += 30

	#####################################################################################################################################

	def handleInput(self, inputClass):
		screen = CyGInterfaceScreen("CvTopCivs", self.screenId)

		if inputClass.getFunctionName() == "Exit" and inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
			screen.hideScreen()
			return 1
		if inputClass.getData() == int(InputTypes.KB_RETURN):
			screen.hideScreen()
			return 1
		return 0

	def update(self, fDelta): return
