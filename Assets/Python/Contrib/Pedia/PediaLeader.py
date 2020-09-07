# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaLeader:

	def __init__(self, parent, H_BOT_ROW):
		self.main = parent

		H_PEDIA_PAGE = parent.H_PEDIA_PAGE

		self.Y_TOP_ROW = Y_TOP_ROW = parent.Y_PEDIA_PAGE
		self.H_BOT_ROW = H_BOT_ROW

		Y_BOT_ROW = Y_TOP_ROW + H_PEDIA_PAGE - H_BOT_ROW

		self.H_ROW_1 = H_ROW_1 = 3 * (H_PEDIA_PAGE - H_BOT_ROW * 3) / 4

		self.Y_ROW_2 = Y_ROW_2 = Y_TOP_ROW + H_ROW_1
		self.Y_ROW_3 = Y_ROW_3 = Y_ROW_2 + H_BOT_ROW
		self.Y_ROW_4 = Y_ROW_4 = Y_ROW_3 + H_BOT_ROW

		self.H_ROW_2 = Y_BOT_ROW + H_BOT_ROW - Y_ROW_2
		self.H_ROW_4 = Y_BOT_ROW + H_BOT_ROW - Y_ROW_4

		self.W_COL_1 = W_COL_1 = int(H_ROW_1 * 0.78)
		self.W_COL_2 = parent.W_PEDIA_PAGE - W_COL_1 - 8

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE
		self.X_COL_2 = X_COL_1 + W_COL_1 + 8


	def interfaceScreen(self, iLeader):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		screen = self.main.screen()
		aName = self.main.getNextWidgetName

		CvLeaderHeadInfo = GC.getLeaderHeadInfo(iLeader)

		eWidGen			= WidgetTypes.WIDGET_GENERAL
		ePanelBlue50	= PanelStyles.PANEL_STYLE_BLUE50

		enumGBS	= self.main.enumGBS
		szfontEdge, szfont4b, szfont4, szfont3b, szfont3, szfont2b, szfont2 = self.main.aFontList

		H_BOT_ROW = self.H_BOT_ROW
		X_COL_1 = self.X_COL_1
		X_COL_2 = self.X_COL_2
		Y_ROW_1 = self.Y_TOP_ROW
		Y_ROW_2 = self.Y_ROW_2
		Y_ROW_3 = self.Y_ROW_3
		Y_ROW_4 = self.Y_ROW_4
		W_COL_1 = self.W_COL_1
		W_COL_2 = self.W_COL_2
		H_ROW_1 = self.H_ROW_1
		H_ROW_2 = self.H_ROW_2
		H_ROW_4 = self.H_ROW_4

		# Main Panel
		szName = szfontEdge + CvLeaderHeadInfo.getDescription()
		if CvLeaderHeadInfo.isNPC():
			szName += " (NPC)"
		screen.setText(aName(), "", szName, 1<<0, X_COL_1, 0, 0, FontTypes.TITLE_FONT, eWidGen, 0, 0)
		screen.addPanel(aName(), "", "", False, False, X_COL_1, Y_ROW_1, W_COL_1, H_ROW_1, PanelStyles.PANEL_STYLE_MAIN)
		screen.addDDSGFC(aName(), CvLeaderHeadInfo.getLeaderHead(), X_COL_1+12, Y_ROW_1+12, W_COL_1-24, H_ROW_1-24, eWidGen, -1, -1)

		# Civilization
		panelName = aName()
		screen.addPanel(panelName, "", "", False, True, X_COL_1, Y_ROW_2, W_COL_1, H_BOT_ROW, ePanelBlue50)
		screen.attachLabel(panelName, "", "  ")
		screen.attachImageButton(panelName, "", CvLeaderHeadInfo.getButton(), enumGBS, eWidGen, iLeader, 0, False)
		iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIV
		aList = []
		for iCiv in range(GC.getNumCivilizationInfos()):
			CvCiv = GC.getCivilizationInfo(iCiv)
			if CvCiv.isLeaders(iLeader):
				aList.append((iCiv, CvCiv))
		for iCiv, CvCiv in aList:
			screen.attachImageButton(panelName, "", CvCiv.getButton(), enumGBS, iWidget, iCiv, 1, False)

		# Favourite Civic & Religion
		panelName = aName()
		screen.addPanel(panelName, TRNSLTR.getText("TXT_KEY_PEDIA_FAV_CIVIC_AND_RELIGION", ()), "", False, True, X_COL_1, Y_ROW_3, W_COL_1, H_BOT_ROW, ePanelBlue50)
		screen.attachLabel(panelName, "", "  ")

		iCivic = CvLeaderHeadInfo.getFavoriteCivic()
		if iCivic != -1:
			screen.attachImageButton(panelName, "", GC.getCivicInfo(iCivic).getButton(), enumGBS, WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIVIC, iCivic, 1, False)

		iReligion = CvLeaderHeadInfo.getFavoriteReligion()
		if iReligion != -1:
			screen.attachImageButton(panelName, "", GC.getReligionInfo(iReligion).getButton(), enumGBS, WidgetTypes.WIDGET_PEDIA_JUMP_TO_RELIGION, iReligion, 1, False)

		# History
		screen.addPanel(aName(), "", "", True, True, X_COL_2, Y_ROW_1, W_COL_2, H_ROW_1, ePanelBlue50)
		szText = szfont3 + CvLeaderHeadInfo.getCivilopedia()
		screen.addMultilineText(aName(), szText, X_COL_2+4, Y_ROW_1+16, W_COL_2-8, H_ROW_1-32, eWidGen, 0, 0, 1<<0)

		# Traits
		screen.addPanel(aName(), "", "", True, False, X_COL_2, Y_ROW_2, W_COL_2, H_ROW_2, ePanelBlue50)
		szSpecialText = szfont3 + CyGameTextMgr().parseLeaderTraits(iLeader, -1, False, True)
		screen.addMultilineText(aName(), szSpecialText, X_COL_2+4, Y_ROW_2+16, W_COL_2-8, H_ROW_2-32, eWidGen, -1, -1, 1<<0)

		# Personality
		screen.addPanel(aName(), "", "", True, True, X_COL_1, Y_ROW_4, W_COL_1, H_ROW_4, ePanelBlue50)

		iFull = CvLeaderHeadInfo.getMaxWarRand()
		iNearby = CvLeaderHeadInfo.getMaxWarNearbyPowerRatio()
		iDistant = CvLeaderHeadInfo.getMaxWarDistantPowerRatio()
		iAdjacent = CvLeaderHeadInfo.getMaxWarMinAdjacentLandPercent()

		szStatsText = szfont3b + "Personality:\n" + szfont3
		szStatsText += TRNSLTR.getText("TXT_KEY_PEDIA_WONDER", (CvLeaderHeadInfo.getWonderConstructRand(),))
		szStatsText += TRNSLTR.getText("TXT_KEY_PEDIA_ATTITUDE", (CvLeaderHeadInfo.getBaseAttitude(), CvLeaderHeadInfo.getWarmongerRespect()))
		szStatsText += TRNSLTR.getText("TXT_KEY_PEDIA_FULL_WAR", (iFull, iNearby, iDistant, iAdjacent))
		szStatsText += TRNSLTR.getText("TXT_KEY_PEDIA_LIMITED_WAR", (CvLeaderHeadInfo.getLimitedWarRand(), CvLeaderHeadInfo.getLimitedWarPowerRatio()))
		szStatsText += TRNSLTR.getText("TXT_KEY_PEDIA_DOGPILE", (CvLeaderHeadInfo.getDogpileWarRand(),))
		szStatsText += "\nRaze city Chance: " + str(CvLeaderHeadInfo.getRazeCityProb()) + "%"

		screen.addMultilineText(aName(), szStatsText, X_COL_1+4, Y_ROW_4+16, W_COL_1-8, H_ROW_4-32, eWidGen, -1, -1, 1<<0)
