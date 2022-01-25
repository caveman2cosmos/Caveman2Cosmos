# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaFeature:

	def __init__(self, parent, H_BOT_ROW):
		self.main = parent

		self.H_PEDIA_PAGE = H_PEDIA_PAGE = parent.H_PEDIA_PAGE
		self.W_PEDIA_PAGE = parent.W_PEDIA_PAGE

		self.Y_TOP_ROW = Y_TOP_ROW = parent.Y_PEDIA_PAGE
		self.Y_BOT_ROW = Y_TOP_ROW + H_PEDIA_PAGE - H_BOT_ROW

		self.H_TOP_ROW = H_TOP_ROW = (H_PEDIA_PAGE - H_BOT_ROW * 3) / 4
		self.H_BOT_ROW = H_BOT_ROW

		iSize = 64
		iRoom = H_BOT_ROW - 40
		while True:
			if iSize < iRoom:
				self.S_BOT_ROW = iSize
				break
			iSize -= 4

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE

		self.S_ICON = H_TOP_ROW - 10


	def interfaceScreen(self, iTheFeature):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		CvTheFeature = GC.getFeatureInfo(iTheFeature)
		aName = self.main.getNextWidgetName
		CyPlayer = self.main.CyPlayer

		eWidGen		= WidgetTypes.WIDGET_GENERAL
		ePnlBlue50	= PanelStyles.PANEL_STYLE_BLUE50
		eFontTitle	= FontTypes.TITLE_FONT

		szfontEdge, szfont4b, szfont4, szfont3b, szfont3, szfont2b, szfont2 = self.main.aFontList

		H_ROW_1 = self.H_TOP_ROW
		H_ROW_2 = self.H_PEDIA_PAGE - H_ROW_1
		H_ROW_3 = self.H_BOT_ROW
		S_ICON = self.S_ICON
		X_COL_1 = self.X_COL_1
		Y_ROW_1 = self.Y_TOP_ROW
		Y_ROW_2 = Y_ROW_1 + H_ROW_1
		Y_ROW_3 = self.Y_BOT_ROW
		W_PEDIA_PAGE = self.W_PEDIA_PAGE
		S_BOT_ROW = self.S_BOT_ROW

		screen = self.main.screen()
		# Main Panel
		szTxt = szfontEdge + CvTheFeature.getDescription()
		screen.setText(aName(), "", szTxt, 1<<0, X_COL_1, 0, 0, eFontTitle, eWidGen, 1, 1)
		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_ROW_1 + 2, H_ROW_1 + 2, H_ROW_1 + 2, PanelStyles.PANEL_STYLE_MAIN)
		Img = "ToolTip|FEATURE%d" % iTheFeature
		screen.setImageButtonAt(Img, Pnl, CvTheFeature.getButton(), 4, 6, S_ICON, S_ICON, eWidGen, 1, 1)
		szTxt = CvTheFeature.getCivilopedia()
		if szTxt:
			x = X_COL_1 + H_ROW_1 + 4
			w = W_PEDIA_PAGE - H_ROW_1 - 4
			screen.addPanel(aName(), "", "", True, False, x, Y_ROW_1, w, H_ROW_1, ePnlBlue50)
			screen.addMultilineText(aName(), szfont2 + szTxt, x + 4, Y_ROW_1 + 8, w - 8, H_ROW_1 - 16, eWidGen, 1, 1, 1<<0)

		# Required for building
		aList = []
		for iType in xrange(GC.getNumBuildingInfos()):
			CvBuilding = GC.getBuildingInfo(iType)
			if CvBuilding.isPrereqOrFeature(iTheFeature):
				aList.append([iType, CvBuilding.getButton()])
		if aList:
			H_ROW_2 -= H_ROW_3
			PF = "ToolTip|JumpTo|BUILDING%d"
			Pnl = aName()
			screen.addPanel(Pnl, "", "", False, True, X_COL_1, Y_ROW_3, W_PEDIA_PAGE, H_ROW_3, ePnlBlue50)
			szText = szfont3b + TRNSLTR.getText("TXT_KEY_PEDIA_BUILDINGS_ENABLED", ())
			screen.setLabelAt(aName(), Pnl, szText, 1<<2, W_PEDIA_PAGE / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
			Pnl = aName()
			screen.addScrollPanel(Pnl, "", X_COL_1 - 2, Y_ROW_3 + 24, W_PEDIA_PAGE + 4, H_ROW_3 - 50, ePnlBlue50)
			screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
			x = 4
			for i, BTN in aList:
				screen.setImageButtonAt(PF % i, Pnl, BTN, x, -2, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
				x += S_BOT_ROW + 4

		# Stats
		szTxt = ""
		iTemp1 = CvTheFeature.getGrowthProbability()
		if iTemp1 > 0:
			fValue = iTemp1 * (GC.getFEATURE_GROWTH_MODIFIER() + 100) / 100.0
			if CyPlayer:
				fValue /= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent()
			else:
				fValue /= 100

			szTemp = "%.2f" % fValue
			while szTemp:
				if szTemp[-1] not in ("0", "."): break
				szTemp = szTemp[:-1]
			szTxt += "\nGrowth: %s%% chance per turn." % szTemp

			fRouteFactor = (100 + GC.getROUTE_FEATURE_GROWTH_MODIFIER()) / 100.0
			if fRouteFactor < 0:
				fRouteFactor = 0
			fValue *= fRouteFactor

			szTemp = "%.3f" % fValue
			while szTemp:
				if szTemp[-1] not in ("0", "."): break
				szTemp = szTemp[:-1]
			szTxt += " (%s%% with a route)" % szTemp

		iTemp = CvTheFeature.getDisappearanceProbability()
		if iTemp > 0:
			if iTemp1:
				szTxt += "  |  "
			else:
				szTxt += "\n"
			if CyPlayer:
				fValue = iTemp / (1.0 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent())
			else:
				fValue = iTemp / 100.0

			szTemp = "%.2f" % fValue
			while szTemp:
				if szTemp[-1] not in ("0", "."): break
				szTemp = szTemp[:-1]

			szTxt += "Vanish: %s%% chance per turn." % szTemp

		szTemp = ""
		for k in range(YieldTypes.NUM_YIELD_TYPES):
			iTemp = CvTheFeature.getYieldChange(k)
			if iTemp:
				if iTemp < 0:
					szTemp += " <color=255,0,0,255>"
				else:
					szTemp += " <color=0,230,0,255>+"
				szTemp += str(iTemp) + (u'%c' % (GC.getYieldInfo(k).getChar())) + "</color>"
			iTemp = CvTheFeature.getRiverYieldChange(k)
			if iTemp:
				if iTemp < 0:
					szTemp += " (<color=255,0,0,255>"
				else:
					szTemp += " (<color=0,230,0,255>+"
				szTemp += str(iTemp) + (u'%c' % (GC.getYieldInfo(k).getChar())) + "</color>River)"
		if szTemp:
			szTxt += "\n" + szTemp
		iTemp = CvTheFeature.getTurnDamage()
		if iTemp:
			szTxt += "\nHazard: %d%% damage per turn" % iTemp
		iTemp = CvTheFeature.getWarmingDefense()
		if iTemp:
			szTxt += "\nGlobal Warming Defense Factor: %d" % iTemp
		if szTxt:
			szTxt = szfont3b + szTxt + "\n"
		szTxt += szfont3 + CyGameTextMgr().getFeatureHelp(iTheFeature, True)
		if szTxt:
			screen.addPanel(aName(), "", "", True, False, X_COL_1, Y_ROW_2, W_PEDIA_PAGE, H_ROW_2, ePnlBlue50)
			screen.addMultilineText(aName(), szTxt, X_COL_1 + 4, Y_ROW_2 + 8, W_PEDIA_PAGE - 8, H_ROW_2 - 16, eWidGen, 1, 1, 1<<0)
