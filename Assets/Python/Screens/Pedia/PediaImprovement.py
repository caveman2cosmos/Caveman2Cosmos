# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaImprovement:

	def __init__(self, parent, H_BOT_ROW):
		self.main = parent

		H_PEDIA_PAGE = parent.H_PEDIA_PAGE

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

		self.W_PEDIA_PAGE = W_PEDIA_PAGE = parent.W_PEDIA_PAGE
		self.W_3RD_PP = W_PEDIA_PAGE / 3

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE

		self.X_GRAPHIC = X_COL_1 + W_PEDIA_PAGE - H_TOP_ROW

		self.W_COL_1 = W_COL_1 = W_PEDIA_PAGE - H_TOP_ROW - 16

		self.S_ICON = S_ICON = H_TOP_ROW - 10

		self.W_STATS = W_COL_1 - S_ICON * 3

		i = H_TOP_ROW / 8
		self.X_MAIN = X_COL_1 + S_ICON - 8
		self.Y_MAIN = Y_TOP_ROW + i
		self.H_MAIN = H_TOP_ROW - i * 2


		#a = H_TOP_ROW / 12
		#self.X_STATS = X_COL_1 + S_ICON
		#self.Y_STATS = Y_TOP_ROW + a
		#self.H_STATS = H_TOP_ROW - a * 2 + 4
		#self.W_STATS = W_BASE - S_ICON

	def interfaceScreen(self, iTheImprove):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		CvTheImproveInfo = GC.getImprovementInfo(iTheImprove)
		screen = self.main.screen()
		aName = self.main.getNextWidgetName

		eWidGen			= WidgetTypes.WIDGET_GENERAL
		eWidJuToBonus	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS
		eWidJuToCivic	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIVIC
		eWidJuToRoute	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_ROUTE
		eWidJuToTech	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH
		ePnlBlue50		= PanelStyles.PANEL_STYLE_BLUE50
		ePnlEmpty		= PanelStyles.PANEL_STYLE_EMPTY
		eTblEmpty		= TableStyles.TABLE_STYLE_EMPTY
		eFontTitle		= FontTypes.TITLE_FONT
		eNumYieldTypes	= YieldTypes.NUM_YIELD_TYPES

		enumGBS = self.main.enumGBS
		szfontEdge, szfont4b, szfont4, szfont3b, szfont3, szfont2b, szfont2 = self.main.aFontList

		W_PEDIA_PAGE = self.W_PEDIA_PAGE
		S_ICON = self.S_ICON
		X_COL_1 = self.X_COL_1
		W_COL_1 = self.W_COL_1
		H_BOT_ROW = self.H_BOT_ROW
		Y_BOT_ROW_1 = self.Y_BOT_ROW
		H_TOP_ROW_1 = self.H_TOP_ROW
		Y_TOP_ROW_1 = self.Y_TOP_ROW
		H_TOP_ROW_2 = H_BOT_ROW + H_BOT_ROW / 4
		Y_TOP_ROW_2 = Y_TOP_ROW_1 + H_TOP_ROW_1
		Y_MID_ROW = Y_TOP_ROW_2 + H_TOP_ROW_2
		H_MID_ROW = Y_BOT_ROW_1 - Y_MID_ROW
		W_STATS = self.W_STATS
		X_MAIN = self.X_MAIN
		Y_MAIN = self.Y_MAIN
		H_MAIN = self.H_MAIN
		S_BOT_ROW = self.S_BOT_ROW

		# Graphic
		screen.addImprovementGraphicGFC("Preview|Min", iTheImprove, self.X_GRAPHIC, Y_TOP_ROW_1 + 8, H_TOP_ROW_1, H_TOP_ROW_1, eWidGen, iTheImprove, 0, -20, 30, 0.7, True)
		self.main.aWidgetBucket.append("Preview|Min")
		# Main Panel
		screen.setText(aName(), "", szfontEdge + CvTheImproveInfo.getDescription(), 1<<0, X_COL_1, 0, 0, FontTypes.TITLE_FONT, eWidGen, 0, 0)
		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_TOP_ROW_1 + 2, W_COL_1 + 8, H_TOP_ROW_1 + 2, PanelStyles.PANEL_STYLE_MAIN)
		Img = "ToolTip|IMP" + str(iTheImprove)
		self.main.aWidgetBucket.append(Img)
		screen.setImageButtonAt(Img, Pnl, CvTheImproveInfo.getButton(), 4, 6, S_ICON, S_ICON, eWidGen, 1, 1)
		# Stats
		szText = ""
		if CvTheImproveInfo.isOutsideBorders():
			szText += " " + TRNSLTR.getText("TXT_KEY_PEDIA_IMPROVEMENT_BUILD_OUTSIDE_BORDERS", ()) + ".\n"
		szYieldReq = " Min. "
		bYieldReq = False
		szYield = ""
		for k in range(YieldTypes.NUM_YIELD_TYPES):
			szChar = u'%c' % (GC.getYieldInfo(k).getChar())
			iYieldChange = CvTheImproveInfo.getYieldChange(k)
			if iYieldChange:
				if iYieldChange < 0:
					szYield += " <color=255,0,0,255>"
				else:
					szYield += " <color=0,230,0,255>+"
				szYield += str(iYieldChange) + szChar + "</color>"
			iRiverYieldChange = CvTheImproveInfo.getRiverSideYieldChange(k)
			if iRiverYieldChange:
				if iRiverYieldChange < 0:
					szYield += " (<color=255,0,0,255>"
				else:
					szYield += " (<color=0,230,0,255>+"
				szYield += str(iRiverYieldChange) + szChar + "</color>River)"
			iFreshwaterYieldChange = CvTheImproveInfo.getIrrigatedYieldChange(k)
			if iFreshwaterYieldChange:
				if iFreshwaterYieldChange < 0:
					szYield += " (<color=255,0,0,255>"
				else:
					szYield += " (<color=0,230,0,255>+"
				szYield += str(iFreshwaterYieldChange) + szChar + "</color>Freshwater)"
			iPrereqNatureYield = CvTheImproveInfo.getPrereqNatureYield(k)
			if iPrereqNatureYield:
				bYieldReq = True
				szYieldReq += str(iPrereqNatureYield) + szChar
		if szYield:
			szText += szYield + "\n"
		iDefenseMod = CvTheImproveInfo.getDefenseModifier()
		if iDefenseMod:
			szDefChar = unichr(8861)
			szText += szDefChar
			if iDefenseMod < 0:
				szText += " <color=255,0,0,255>"
			else:
				szText += " <color=0,230,0,255>+"
			szText += "%d%%" %iDefenseMod + unichr(8855) + szDefChar
		iAirDefense = CvTheImproveInfo.getAirBombDefense()
		if iAirDefense:
			if iAirDefense < 0:
				szText += " <color=255,0,0,255>"
			else:
				szText += " <color=0,230,0,255>+"
			szText += str(iAirDefense) + "</color> "+ TRNSLTR.getText("TXT_KEY_PEDIA_IMPROVEMENT_AIR_DEFENSE", ())
		if szText:
			listBox = aName()
			screen.addListBoxGFC(listBox, "", X_MAIN, Y_MAIN, W_STATS, H_MAIN, eTblEmpty)
			screen.enableSelect(listBox, False)
			if szText:
				screen.appendListBoxString(listBox, szfont3b + szText, eWidGen, 0, 0, 1<<0)
		# Builds
		PF = "ToolTip|JumpTo|"
		aList0 = []
		for i in range(GC.getNumBuildInfos()):
			CvBuildInfo = GC.getBuildInfo(i)
			if CvBuildInfo.getImprovement() == iTheImprove:
				aList0.append((i, CvBuildInfo.getButton()))

		if aList0:
			szChild = PF + "BUILD"
			W_REQUIRES = W_COL_1 - W_STATS - S_ICON
			Pnl = aName()
			y = Y_MAIN + (H_MAIN - H_BOT_ROW)/2
			screen.addPanel(Pnl, "", "", False, True, X_MAIN + W_STATS, y, W_REQUIRES, H_BOT_ROW, ePnlBlue50)
			szText = szfont3b + TRNSLTR.getText("TXT_KEY_PEDIA_BUILD", ())
			screen.setLabelAt(aName(), Pnl, szText, 1<<0, 16, 2, 0, eFontTitle, eWidGen, 0, 0)
			Pnl = aName()
			screen.addScrollPanel(Pnl, "", X_MAIN + W_STATS - 2, y + 24, W_REQUIRES + 4, H_BOT_ROW - 50, ePnlBlue50)
			screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
			x = 4
			for iType, BTN in aList0:
				screen.setImageButtonAt(szChild + str(iType), Pnl, BTN, x, -2, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
				x += S_BOT_ROW + 4
			aList0 = []

		# Synergy
		szAnd = "<font=4b>&#38"
		aValidList = []
		aList1 = []
		for iTech in range(GC.getNumTechInfos()):
			szYield = ""
			for k in range(eNumYieldTypes):
				iYieldChange = CvTheImproveInfo.getTechYieldChanges(iTech, k)
				if iYieldChange:
					if iYieldChange < 0:
						szYield += " <color=255,0,0,255>"
					else:
						szYield += " <color=0,230,0,255>"
					szYield += str(iYieldChange) + u'%c' % (GC.getYieldInfo(k).getChar())
			if szYield:
				aList1.append((iTech, szYield))
		aList2 = []
		for iCivic in range(GC.getNumCivicInfos()):
			szYield = ""
			for k in range(eNumYieldTypes):
				iYieldChange = GC.getCivicInfo(iCivic).getImprovementYieldChanges(iTheImprove, k)
				if iYieldChange:
					if iYieldChange < 0:
						szYield += " <color=255,0,0,255>"
					else:
						szYield += " <color=0,230,0,255>"
					szYield += str(iYieldChange) + u'%c' % (GC.getYieldInfo(k).getChar())
			if szYield:
				aList2.append((iCivic, szYield))
		aList3 = []
		szChild = PF + "BONUS"
		n = 0
		for iType in range(GC.getNumBonusInfos()):
			szYield = ""
			for k in range(eNumYieldTypes):
				iYieldChange = CvTheImproveInfo.getImprovementBonusYield(iType, k)
				if iYieldChange:
					if iYieldChange < 0:
						szYield += " <color=255,0,0,255>"
					else:
						szYield += " <color=0,230,0,255>"
					szYield += str(iYieldChange) + u'%c' % (GC.getYieldInfo(k).getChar())
			if szYield:
				aList3.append((iType, szYield))
			if CvTheImproveInfo.isImprovementBonusMakesValid(iType):
				aValidList.append([szChild + str(iType) + "|" + str(n), GC.getBonusInfo(iType).getButton()])
				n += 1
		aList4 = []
		for iRoute in range(GC.getNumRouteInfos()):
			szYield = ""
			for k in range(eNumYieldTypes):
				iYieldChange = CvTheImproveInfo.getRouteYieldChanges(iRoute, k)
				if iYieldChange:
					if iYieldChange < 0:
						szYield += " <color=255,0,0,255>"
					else:
						szYield += " <color=0,230,0,255>"
					szYield += str(iYieldChange) + u'%c' % (GC.getYieldInfo(k).getChar())
			if szYield:
				aList4.append((iRoute, szYield))
		if aList1 or aList2 or aList3 or aList4:
			panelName = aName()
			screen.addPanel(panelName, TRNSLTR.getText("TXT_KEY_PEDIA_SYNERGY", ()), "", False, True, X_COL_1, Y_TOP_ROW_2, W_PEDIA_PAGE, H_TOP_ROW_2, ePnlBlue50)
			bAnd = False
			if aList1:
				bAnd = True
				for i, szYield in aList1:
					childPanelName = aName()
					screen.attachPanel(panelName, childPanelName, "", "", True, True, ePnlEmpty)
					screen.attachImageButton(childPanelName, "", GC.getTechInfo(i).getButton(), enumGBS, eWidJuToTech, i, 2, False)
					screen.attachLabel(childPanelName, "", szfont3b + " " + szYield)
					screen.attachLabel(panelName, "", "   ")
			if aList2:
				if bAnd:
					screen.attachLabel(panelName, "", szAnd + "  ")
				else:
					bAnd = True
				for i, szYield in aList2:
					childPanelName = aName()
					screen.attachPanel(panelName, childPanelName, "", "", True, True, ePnlEmpty)
					screen.attachImageButton(childPanelName, "", GC.getCivicInfo(i).getButton(), enumGBS, eWidJuToCivic, i, 2, False)
					screen.attachLabel(childPanelName, "", szfont3b + " " + szYield)
					screen.attachLabel(panelName, "", "   ")
			if aList3:
				if bAnd:
					screen.attachLabel(panelName, "", szAnd + "  ")
				else:
					bAnd = True
				for i, szYield in aList3:
					childPanelName = aName()
					screen.attachPanel(panelName, childPanelName, "", "", True, True, ePnlEmpty)
					screen.attachImageButton(childPanelName, "", GC.getBonusInfo(i).getButton(), enumGBS, eWidJuToBonus, i, 2, False)
					screen.attachLabel(childPanelName, "", szfont3b + " " + szYield)
					screen.attachLabel(panelName, "", "   ")
			if aList4:
				if bAnd:
					screen.attachLabel(panelName, "", szAnd + "  ")
				else:
					bAnd = True
				for i, szYield in aList4:
					childPanelName = aName()
					screen.attachPanel(panelName, childPanelName, "", "", True, True, ePnlEmpty)
					screen.attachImageButton(childPanelName, "", GC.getRouteInfo(i).getButton(), enumGBS, eWidJuToRoute, i, 2, False)
					screen.attachLabel(childPanelName, "", szfont3b + " " + szYield)
					screen.attachLabel(panelName, "", "   ")
		else:
			Y_MID_ROW -= H_TOP_ROW_2
			H_MID_ROW += H_TOP_ROW_2
		# Requires
		nTerrains = GC.getNumTerrainInfos()
		iType = CvTheImproveInfo.getPrereqTech()
		if iType != -1:
			aList0.append([PF + "TECH" + str(iType), GC.getTechInfo(iType).getButton()])

		# bNotOnAnyBonus is not exposed to python.

		if CvTheImproveInfo.isWaterImprovement():
			aList0.append([PF + "CONCEPT_NEW" + str(GC.getInfoTypeForString("CONCEPT_WATER_TERRAINS")), "Art/Interface/Buttons/BaseTerrain/Ocean.dds"])

		if CvTheImproveInfo.isPeakImprovement():
			aList0.append([PF + "TERRAIN" + str(GC.getInfoTypeForString("TERRAIN_PEAK")) + "|" + str(n), "Art/Interface/Buttons/BaseTerrain/Peak.dds"])
			n += 1
		elif not CvTheImproveInfo.isPeakMakesValid():
			aList0.append([PF + "TERRAIN" + str(GC.getInfoTypeForString("TERRAIN_PEAK")) + "|" + str(n), "Art/Interface/Buttons/BaseTerrain/Peak.dds", "NOT"])
			n += 1

		if CvTheImproveInfo.isRequiresFlatlands():
			aList0.append(["ToolTip|TxtTT|TXT_KEY_IMPROVEMENTHELP_ONLY_BUILD_FLATLANDS1", "Art/Interface/Buttons/BaseTerrain/Grassland.dds"])

		if CvTheImproveInfo.isRequiresRiverSide():
			aList0.append(["ToolTip|TxtTT|TXT_KEY_IMPROVEMENTHELP_REQUIRES_RIVER1", "Art/Interface/Buttons/WorldBuilder/River_Placement.dds"])

		if CvTheImproveInfo.isRequiresFeature():
			aList0.append(["ToolTip|TxtTT|Any_plot_feature1", "Art/bug/questionmark.dds"])

		if CvTheImproveInfo.isRequiresIrrigation():
			aList0.append(["ToolTip|TxtTT|TXT_KEY_IMPROVEMENTHELP_REQUIRES_IRRIGATION1", "Art/Interface/Buttons/Buildings/Irrigation.dds"])

		if CvTheImproveInfo.isNoFreshWater():
			aList0.append(["ToolTip|TxtTT|TXT_KEY_IMPROVEMENTHELP_NO_BUILD_FRESH_WATER1", "Art/bug/questionmark.dds"])

		szChild = PF + "TERRAIN"
		for i in range(nTerrains):
			if CvTheImproveInfo.getTerrainMakesValid(i):
				aValidList.append((szChild + str(i) + "|" + str(n), GC.getTerrainInfo(i).getButton()))
				n += 1
		if CvTheImproveInfo.isHillsMakesValid():
			aValidList.append((szChild + str(GC.getInfoTypeForString("TERRAIN_HILL")) + "|" + str(n), "Art/Interface/Buttons/BaseTerrain/Hill.dds"))
			n += 1

		szChild = PF + "FEATURE"
		for i in range(GC.getNumFeatureInfos()):
			if CvTheImproveInfo.getFeatureMakesValid(i):
				aValidList.append((szChild + str(i) + "|" + str(n), GC.getFeatureInfo(i).getButton()))
				n += 1

		if aList0 or aValidList:
			szNot = "<font=4b>{!="
			szOr = "<font=4b>||"
			szBracketL = "<font=4b>{"
			szBracketR = "<font=4b>}"
			H_SCROLL = H_BOT_ROW - 50
			Pnl = aName()
			screen.addPanel(Pnl, "", "", False, True, X_COL_1, Y_BOT_ROW_1, W_PEDIA_PAGE, H_BOT_ROW, ePnlBlue50)
			szText = szfont3b + TRNSLTR.getText("TXT_KEY_PEDIA_REQUIRES", ())
			screen.setLabelAt(aName(), Pnl, szText, 1<<2, W_PEDIA_PAGE / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
			Pnl = aName()
			screen.addScrollPanel(Pnl, "", X_COL_1 - 2, Y_BOT_ROW_1 + 24, W_PEDIA_PAGE + 4, H_SCROLL, ePnlBlue50)
			screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
			screen.hide(Pnl)
			x = 4
			y = H_SCROLL / 2 - 12
			bValidBracket = False
			for entry in aList0:
				if entry[-1] == "NOT":
					if x != 4:
						x += 8
					screen.setLabelAt(aName(), Pnl, szNot, 1<<0, x, y, 0, eFontTitle, eWidGen, 0, 0)
					x += 44
				screen.setImageButtonAt(entry[0], Pnl, entry[1], x, -2, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
				x += S_BOT_ROW + 4
				if entry[-1] == "NOT":
					x += 6
					screen.setLabelAt(aName(), Pnl, szBracketR, 1<<0, x, y, 0, eFontTitle, eWidGen, 0, 0)
					x += 14
			if aList0 and len(aValidList):
				bValidBracket = True
				x += 8
				screen.setLabelAt(aName(), Pnl, szBracketL, 1<<0, x, y, 0, eFontTitle, eWidGen, 0, 0)
				x += 16
			i = 0
			for NAME, BTN in aValidList:
				if i == 0:
					i+= 1
				else:
					x += 6
					screen.setLabelAt(aName(), Pnl, szOr, 1<<2, x, y, 0, eFontTitle, eWidGen, 0, 0)
					x += 10
				screen.setImageButtonAt(NAME, Pnl, BTN, x, -2, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
				x += S_BOT_ROW + 4
			if bValidBracket:
				screen.setLabelAt(aName(), Pnl, szBracketR, 1<<0, x + 8, y, 0, eFontTitle, eWidGen, 0, 0)
			screen.show(Pnl)

		else:
			H_MID_ROW += H_BOT_ROW
		# Effects and History
		szEffects = CyGameTextMgr().getImprovementHelp(iTheImprove, False)
		i = szEffects.find("\n") + 1
		if not i:
			szEffects = ""
		szHistory = CvTheImproveInfo.getCivilopedia()
		if szEffects or szHistory:
			if szEffects and szHistory:
				W_MID_COL_1 = self.W_3RD_PP
				W_MID_COL_2 = W_PEDIA_PAGE - W_MID_COL_1 - 8
				X_COL_2 = X_COL_1 + W_MID_COL_1 + 8
			elif szEffects:
				W_MID_COL_1 = W_PEDIA_PAGE
			elif szHistory:
				X_COL_2 = X_COL_1
				W_MID_COL_2 = W_PEDIA_PAGE
			if szEffects:
				szHeader = TRNSLTR.getText("TXT_KEY_PEDIA_EFFECTS", ())
				screen.addPanel(aName(), szHeader, "", True, False, X_COL_1, Y_MID_ROW, W_MID_COL_1, H_MID_ROW, ePnlBlue50)
				screen.addMultilineText(aName(), szfont3 + szEffects[i:], X_COL_1 + 4, Y_MID_ROW + 32, W_MID_COL_1 - 8, H_MID_ROW - 40, eWidGen, 0, 0, 1<<0)
			if szHistory:
				szHeader = TRNSLTR.getText("TXT_KEY_PEDIA_HISTORY", ())
				screen.addPanel(aName(), szHeader, "", True, True, X_COL_2, Y_MID_ROW, W_MID_COL_2, H_MID_ROW, ePnlBlue50)
				screen.addMultilineText(aName(), szfont2 + szHistory, X_COL_2 + 4, Y_MID_ROW + 32, W_MID_COL_2 - 8, H_MID_ROW - 40, eWidGen, 0, 0, 1<<0)
