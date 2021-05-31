## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005

from CvPythonExtensions import *

class CvDawnOfMan:

	def interfaceScreen(self, iScreenID):
		GC = CyGlobalContext()
		G = GC.getGame()
		if G.isPitbossHost(): return
		AFM = CyArtFileMgr()
		TRNSLTR = CyTranslator()

		CyPlayer = GC.getActivePlayer()
		iLeader = CyPlayer.getLeaderType()
		CvLeaderHead = GC.getLeaderHeadInfo(iLeader)
		iCiv = CyPlayer.getCivilizationType()
		CvCiv = GC.getCivilizationInfo(iCiv)

		plot = CyPlayer.getStartingPlot()
		GC.getMap().bringIntoView(plot.getX(), plot.getY(), True, False, False, False, False)

		screen = CyGInterfaceScreen("CvDawnOfMan", iScreenID)

		import ScreenResolution as SR
		xRes = SR.x
		yRes = SR.y

		W_TECH = 425
		H_TECH = 80

		W_MAIN_PANEL = 750

		H_MAIN_PANEL = 525
		X_MAIN_PANEL = (xRes/2) - (W_MAIN_PANEL/2)

		Y_MAIN_PANEL = 70

		iMarginSpace = 15

		X_HEADER_PANEL = X_MAIN_PANEL + iMarginSpace
		Y_HEADER_PANEL = Y_MAIN_PANEL + iMarginSpace
		W_HEADER_PANEL = W_MAIN_PANEL - (iMarginSpace * 2)
		H_HEADER_PANEL = int(H_MAIN_PANEL * (2.0 / 5.0)) + 60

		X_LEADER_ICON = X_HEADER_PANEL + iMarginSpace
		Y_LEADER_ICON = Y_HEADER_PANEL + iMarginSpace
		H_LEADER_ICON = H_HEADER_PANEL - 30
		W_LEADER_ICON = int(H_LEADER_ICON / 1.272727)

		WH_FANCY_ICON = 64
		X_FANCY_ICON1 = X_LEADER_ICON + W_LEADER_ICON + iMarginSpace
		X_FANCY_ICON2 = X_LEADER_ICON + (W_HEADER_PANEL - (iMarginSpace * 2) - WH_FANCY_ICON)
		Y_FANCY_ICON = (Y_HEADER_PANEL + iMarginSpace + 6) - 6

		X_LEADER_TITLE_TEXT = (X_FANCY_ICON1+WH_FANCY_ICON)+((X_FANCY_ICON2 - (X_FANCY_ICON1+WH_FANCY_ICON))/2) - ((W_HEADER_PANEL / 3)/2)

		Y_LEADER_TITLE_TEXT = Y_HEADER_PANEL + iMarginSpace + 6
		W_LEADER_TITLE_TEXT = W_HEADER_PANEL / 3
		H_LEADER_TITLE_TEXT = H_HEADER_PANEL / 2

		X_STATS_TEXT = X_FANCY_ICON1

		Y_STATS_TEXT = Y_LEADER_TITLE_TEXT + 60
		W_STATS_TEXT = int(W_HEADER_PANEL * (5 / 7.0)) + (iMarginSpace * 2)
		H_STATS_TEXT = int(H_HEADER_PANEL * (3 / 5.0)) - (iMarginSpace * 2)

		X_TEXT_PANEL = X_HEADER_PANEL
		Y_TEXT_PANEL = Y_HEADER_PANEL + H_HEADER_PANEL + iMarginSpace - 10 #10 is the fudge factor
		W_TEXT_PANEL = W_HEADER_PANEL
		H_TEXT_PANEL = H_MAIN_PANEL - H_HEADER_PANEL - (iMarginSpace * 3) + 10 #10 is the fudge factor
		iTEXT_PANEL_MARGIN = 35

		W_EXIT = 120
		H_EXIT = 30

		X_EXIT = xRes/2 - W_EXIT/2
		Y_EXIT = Y_TEXT_PANEL + H_TEXT_PANEL - (iMarginSpace * 3)

		EXIT_TEXT = TRNSLTR.getText("TXT_KEY_SCREEN_CONTINUE", ())

		eWidGen = WidgetTypes.WIDGET_GENERAL
		# Create screen
		screen.showScreen(PopupStates.POPUPSTATE_QUEUED, False)
		screen.showWindowBackground(False)
		screen.setDimensions(0, screen.centerY(0), xRes, yRes)
		screen.enableWorldSounds(False)

		# Create panels... Main Panel:
		szMainPanel = "DawnOfManMainPanel"
		screen.addPanel("", "", "", True, True, X_MAIN_PANEL, Y_MAIN_PANEL, W_MAIN_PANEL, H_MAIN_PANEL, PanelStyles.PANEL_STYLE_MAIN)

		# Top
		szHeaderPanel = "DawnOfManHeaderPanel"
		screen.addPanel("", "", "", True, False, X_HEADER_PANEL, Y_HEADER_PANEL, W_HEADER_PANEL, H_HEADER_PANEL, PanelStyles.PANEL_STYLE_DAWNTOP)

		# Bottom
		szTextPanel = "DawnOfManTextPanel"
		screen.addPanel("", "", "", True, True, X_TEXT_PANEL, Y_TEXT_PANEL, W_TEXT_PANEL, H_TEXT_PANEL, PanelStyles.PANEL_STYLE_DAWNBOTTOM)

		# Add contents... Leaderhead graphic:
		szLeaderPanel = "DawnOfManLeaderPanel"
		screen.addPanel("", "", "", True, False, X_LEADER_ICON - 3, Y_LEADER_ICON - 5, W_LEADER_ICON + 6, H_LEADER_ICON + 8, PanelStyles.PANEL_STYLE_DAWNTOP)
		screen.addLeaderheadGFC("", iLeader, AttitudeTypes.ATTITUDE_PLEASED, X_LEADER_ICON + 5, Y_LEADER_ICON + 5, W_LEADER_ICON - 10, H_LEADER_ICON - 10, eWidGen, 1, 1)

		# Info/"Stats" text
		szNameText = "<color=255,255,0,255><font=3b>%s</font>" % CvLeaderHead.getDescription().upper()
		szNameText += "\n- %s -\n" % CyPlayer.getCivilizationDescription(0)
		szNameText += CyGameTextMgr().parseLeaderTraits(iLeader, iCiv, True, False)
		screen.addMultilineText("", szNameText, X_LEADER_TITLE_TEXT, Y_LEADER_TITLE_TEXT, W_LEADER_TITLE_TEXT, H_LEADER_TITLE_TEXT, eWidGen, 1, 1, 1<<2)

		Text_BoxText = CyGameTextMgr().parseCivInfos(iCiv, True)

		screen.addMultilineText("", Text_BoxText, X_STATS_TEXT, Y_STATS_TEXT+30+H_TECH, W_STATS_TEXT - (iMarginSpace * 3), H_STATS_TEXT - (iMarginSpace * 4), eWidGen, 1, 1, 1<<0)

		# Fancy icon things
		screen.addDDSGFC("", AFM.getCivilizationArtInfo(CvCiv.getArtDefineTag()).getButton(), X_FANCY_ICON1 , Y_FANCY_ICON , WH_FANCY_ICON, WH_FANCY_ICON, eWidGen, 0, 0)
		screen.addDDSGFC("", AFM.getCivilizationArtInfo(CvCiv.getArtDefineTag()).getButton(), X_FANCY_ICON2 , Y_FANCY_ICON , WH_FANCY_ICON, WH_FANCY_ICON, eWidGen, 0, 0)

		# Main Body text
		szDawnTitle = "<font=3>" + TRNSLTR.getText("TXT_KEY_DAWN_OF_MAN_SCREEN_TITLE", ()).upper()
		screen.setLabel("", "", szDawnTitle, 1<<2,	X_TEXT_PANEL + (W_TEXT_PANEL / 2), Y_TEXT_PANEL + 15, -2.0, FontTypes.GAME_FONT, eWidGen, 1, 1)
		bodyString = TRNSLTR.getText("TXT_KEY_DAWN_OF_MAN", (CyGameTextMgr().getTimeStr(G.getGameTurn(), False), CyPlayer.getCivilizationAdjectiveKey(), CyPlayer.getNameKey()))
		screen.addMultilineText("", bodyString, X_TEXT_PANEL + iMarginSpace, Y_TEXT_PANEL + iMarginSpace + iTEXT_PANEL_MARGIN, W_TEXT_PANEL - (iMarginSpace * 2), H_TEXT_PANEL - (iMarginSpace * 2) - 75, eWidGen, 1, 1, 1<<0)

		screen.setButtonGFC("", EXIT_TEXT, "", X_EXIT, Y_EXIT, W_EXIT, H_EXIT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)

		screen.setSoundId(CyAudioGame().Play2DSoundWithId(CvLeaderHead.getDiploPeaceMusicScriptIds(0)))

	def handleInput(self, inputClass): pass

	def update(self, fDelta): pass

	def onClose(self):
		CyInterface().setSoundSelectionReady(True)
