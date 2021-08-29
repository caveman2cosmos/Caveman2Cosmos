# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaBonus:

	def __init__(self, parent, H_BOT_ROW):
		import HelperFunctions
		self.HF = HelperFunctions.HelperFunctions([0])

		self.main = parent

		H_PEDIA_PAGE = parent.H_PEDIA_PAGE

		self.Y_TOP_ROW = Y_TOP_ROW = parent.Y_PEDIA_PAGE
		self.Y_BOT_ROW = Y_TOP_ROW + H_PEDIA_PAGE - H_BOT_ROW

		self.H_TOP_ROW = H_TOP_ROW = (H_PEDIA_PAGE - H_BOT_ROW * 3) / 4
		self.H_BOT_ROW = H_BOT_ROW

		self.W_PEDIA_PAGE = W_PEDIA_PAGE = parent.W_PEDIA_PAGE
		self.W_HALF_PP = W_HALF_PP = W_PEDIA_PAGE / 2 - 4
		self.W_3RD_PP = W_PEDIA_PAGE / 3 - 4

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE
		self.X_COL_2 = X_COL_1 + W_HALF_PP + 8

		self.X_GRAPHIC = X_COL_1 + W_PEDIA_PAGE - H_TOP_ROW

		self.W_COL_1 = W_COL_1 = W_PEDIA_PAGE - H_TOP_ROW - 16

		self.S_ICON = S_ICON = H_TOP_ROW - 6

		self.X_STATS = X_COL_1 + S_ICON
		self.Y_STATS = Y_TOP_ROW + H_TOP_ROW / 12
		self.W_STATS = (W_COL_1 - S_ICON) / 2

		self.Y_REQOBS = Y_TOP_ROW + H_TOP_ROW / 6 - 8

	def interfaceScreen(self, iTheBonus):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		CvTheBonusInfo = GC.getBonusInfo(iTheBonus)
		screen = self.main.screen()
		aName = self.main.getNextWidgetName

		eWidGen				= WidgetTypes.WIDGET_GENERAL
		eWidJuToBuilding	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING
		eWidJuToImprove		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_IMPROVEMENT
		eWidJuToTech		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH
		eWidJuToUnit		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT
		ePanelBlue50		= PanelStyles.PANEL_STYLE_BLUE50
		ePanelEmpty			= PanelStyles.PANEL_STYLE_EMPTY
		eNumYieldTypes 		= YieldTypes.NUM_YIELD_TYPES
		eNumCommerceTypes 	= CommerceTypes.NUM_COMMERCE_TYPES

		enumGBS = self.main.enumGBS
		szfontEdge, szfont4b, szfont4, szfont3b, szfont3, szfont2b, szfont2 = self.main.aFontList

		S_ICON = self.S_ICON
		H_TOP_ROW = self.H_TOP_ROW
		H_BOT_ROW = self.H_BOT_ROW
		H_MID = H_TOP_ROW * 3 - H_BOT_ROW - 16
		X_COL_1 = self.X_COL_1
		X_STATS = self.X_STATS
		Y_TOP_ROW_1 = self.Y_TOP_ROW
		Y_TOP_ROW_2 = Y_TOP_ROW_1 + H_TOP_ROW
		Y_TOP_ROW_3 = Y_TOP_ROW_2 + H_BOT_ROW + 16
		Y_BOT_ROW_1 = self.Y_BOT_ROW
		Y_BOT_ROW_2 = Y_BOT_ROW_1 - H_BOT_ROW
		Y_BOT_ROW_3 = Y_BOT_ROW_2 - H_BOT_ROW
		Y_STATS = self.Y_STATS
		W_STATS = self.W_STATS
		W_PEDIA_PAGE = self.W_PEDIA_PAGE

		bMapBonus = CvTheBonusInfo.getConstAppearance() > 0
		if bMapBonus:
			bCultureBonus = False
			# Graphic
			screen.addBonusGraphicGFC("Preview|Min", iTheBonus, self.X_GRAPHIC, Y_TOP_ROW_1 + 8, H_TOP_ROW, H_TOP_ROW, eWidGen, iTheBonus, 0, -20, 30, 0.6, True)
			self.main.aWidgetBucket.append("Preview|Min")
			W_COL_1 = self.W_COL_1
		else:
			W_COL_1 = W_PEDIA_PAGE
			bCultureBonus = self.main.SECTION[1] == TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_BONUS_CULTURE", ())
		# Loop through all buildings and find those connected to the bonus.
		aVicinityBuildings = []
		aNeededByBuildings = []
		aAffectedBuildings = []
		aSourceOfBonus = []
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			aGOMBonusReqList = []
			for i in range(2):
				aGOMBonusReqList.append([])
			self.HF.getGOMReqs(CvBuildingInfo.getConstructCondition(), GOMTypes.GOM_BONUS, aGOMBonusReqList)
			bValid = True
			if CvBuildingInfo.getBonusHealthChanges(iTheBonus) or CvBuildingInfo.getBonusHappinessChanges(iTheBonus) or CvBuildingInfo.getBonusProductionModifier(iTheBonus):
				aAffectedBuildings.append(iBuilding)
				bValid = False
			if CvBuildingInfo.isAnyBonusYieldChanges() or CvBuildingInfo.isAnyVicinityBonusYieldChanges() or CvBuildingInfo.isAnyBonusYieldModifiers():
				for eYield in xrange(eNumYieldTypes):
					if CvBuildingInfo.getBonusYieldChanges(iTheBonus, eYield) or CvBuildingInfo.getVicinityBonusYieldChanges(iTheBonus, eYield) or CvBuildingInfo.getBonusYieldModifier(iTheBonus, eYield):
						if iBuilding not in aAffectedBuildings:
							aAffectedBuildings.append(iBuilding)
							bValid = False
			if CvBuildingInfo.isAnyBonusCommercePercentChanges() or CvBuildingInfo.isAnyBonusCommerceModifiers():
				for eCommerce in xrange(eNumCommerceTypes):
					if CvBuildingInfo.getBonusCommercePercentChanges(iTheBonus, eCommerce) or CvBuildingInfo.getBonusCommerceModifier(iTheBonus, eCommerce):
						if iBuilding not in aAffectedBuildings:
							aAffectedBuildings.append(iBuilding)
							bValid = False
			if CvBuildingInfo.getFreeBonus() == iTheBonus:
				aSourceOfBonus.append(iBuilding)
			else:
				for i in xrange(CvBuildingInfo.getNumExtraFreeBonuses()):
					if (CvBuildingInfo.getExtraFreeBonus(i) == iTheBonus):
						aSourceOfBonus.append(iBuilding)
						break
			if CvBuildingInfo.getPrereqVicinityBonus() == iTheBonus or CvBuildingInfo.getPrereqRawVicinityBonus() == iTheBonus:
				aVicinityBuildings.append(iBuilding)
			if iTheBonus in CvBuildingInfo.getPrereqOrVicinityBonuses():
				aVicinityBuildings.append(iBuilding)
			for iBonus in CvBuildingInfo.getPrereqOrRawVicinityBonuses():
				if iBonus == iTheBonus:
					aVicinityBuildings.append(iBuilding)
			if bValid:
				if CvBuildingInfo.getPrereqAndBonus() == iTheBonus \
				or iTheBonus in CvBuildingInfo.getPrereqOrBonuses() \
				or iTheBonus in aGOMBonusReqList[BoolExprTypes.BOOLEXPR_AND] or iTheBonus in aGOMBonusReqList[BoolExprTypes.BOOLEXPR_OR]:
					aNeededByBuildings.append(iBuilding)
		# Loop through all units and find those connected to the bonus.
		aNeededByUnits = []
		aAffectedUnits = []
		bValid = True
		for iUnit in xrange(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			aGOMBonusReqList = []
			for i in range(2):
				aGOMBonusReqList.append([])
			self.HF.getGOMReqs(CvUnitInfo.getTrainCondition(), GOMTypes.GOM_BONUS, aGOMBonusReqList)
			if CvUnitInfo.getPrereqAndBonus() == iTheBonus:
				aNeededByUnits.append(iUnit)
				bValid = False
			elif iTheBonus in CvUnitInfo.getPrereqOrBonuses() \
			or iTheBonus in aGOMBonusReqList[BoolExprTypes.BOOLEXPR_AND] or iTheBonus in aGOMBonusReqList[BoolExprTypes.BOOLEXPR_OR]:
				aNeededByUnits.append(iUnit)
			if bValid:
				iBonusProductionModifier = CvUnitInfo.getBonusProductionModifier(iTheBonus)
				if iBonusProductionModifier:
					aAffectedUnits.append((iBonusProductionModifier, iUnit))
		# Main Panel
		szBonusChar = u'%c' % CvTheBonusInfo.getChar()
		szBonusName = szBonusChar + " " + CvTheBonusInfo.getDescription() + " " + szBonusChar
		screen.setText(aName(), "", szfontEdge + szBonusName, 1<<0, X_COL_1, 0, 0, FontTypes.TITLE_FONT, eWidGen, 0, 0)
		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_TOP_ROW_1 + 2, W_COL_1 + 8, H_TOP_ROW + 2, PanelStyles.PANEL_STYLE_MAIN)
		Img = "ToolTip|BONUS" + str(iTheBonus)
		screen.setImageButtonAt(Img, Pnl, CvTheBonusInfo.getButton(), 2, 2, S_ICON, S_ICON, eWidGen, 1, 1)
		# Stats
		iMinLatitude = CvTheBonusInfo.getMinLatitude()
		iMaxLatitude = CvTheBonusInfo.getMaxLatitude()
		szTxt = ""
		if iMinLatitude or iMaxLatitude < 90:
			if not iMinLatitude:
				szTxt = szfont4b + "<color=200,240,120,255>" + TRNSLTR.getText("TXT_KEY_PEDIA_LATITUDE", ()) + " 0&#176  &#187  &#177 " + str(iMaxLatitude) + "&#176"
			else:
				szTxt = szfont4b + "<color=200,240,120,255>" + TRNSLTR.getText("TXT_KEY_PEDIA_LATITUDE", ()) + " &#177 " + str(iMinLatitude) + "&#176  &#187  &#177 " + str(iMaxLatitude) + "&#176"
		szChange = ""
		for k in xrange(eNumYieldTypes):
			iYieldChange = CvTheBonusInfo.getYieldChange(k)
			if iYieldChange:
				if iYieldChange < 0:
					szChange += " <color=255,0,0,255>"
				else:
					szChange += " <color=0,230,0,255>"
				szChange += "%d%s" %(iYieldChange, unichr(8483 + k))
		iHappiness = CvTheBonusInfo.getHappiness()
		iHealth = CvTheBonusInfo.getHealth()

		if iHappiness:
			if iHappiness > 0:
				szChange += " <color=0,230,0,255>%d%s" %(iHappiness, unichr(8850))
			else:
				szChange += " <color=255,0,0,255>%d%s" %(-iHappiness, unichr(8851))
		if iHealth:
			if iHealth > 0:
				szChange += " <color=0,230,0,255>%d%s" %(iHealth, unichr(8852))
			else:
				szChange += " <color=255,0,0,255>%d%s" %(-iHealth, unichr(8853))

		if szChange:
			szTxt += '\n' + szfont3b + szChange
		if szTxt:
			panelName = aName()
			screen.addListBoxGFC(panelName, "", X_STATS, Y_STATS, W_STATS, H_TOP_ROW - 12, TableStyles.TABLE_STYLE_EMPTY)
			screen.enableSelect(panelName, False)
			if szTxt:
				screen.appendListBoxString(panelName, szTxt, eWidGen, 0, 0, 1<<0)
		# Reveals, enables, and obsoletes.
		iRevealTech = CvTheBonusInfo.getTechReveal()
		iEnableTech = CvTheBonusInfo.getTechCityTrade()
		iObsoleteTech = CvTheBonusInfo.getTechObsolete()
		if iRevealTech != -1 or iEnableTech != -1 or iObsoleteTech != -1:
			enumBS = GenericButtonSizes.BUTTON_SIZE_CUSTOM
			panelName = aName()
			screen.addPanel(panelName, "", "", False, True, X_STATS + W_STATS, self.Y_REQOBS, W_STATS, H_TOP_ROW - 16, ePanelEmpty)
			if iRevealTech == iEnableTech and iRevealTech != -1:
				childPanelName = aName()
				screen.attachPanel(panelName, childPanelName, "", "", True, True, ePanelEmpty)
				screen.attachLabel(childPanelName, "", szfont4b + TRNSLTR.getText("TXT_KEY_PEDIA_BONUS_APPEARANCE_AND_TRADE", ()))
				screen.attachImageButton(childPanelName, "", GC.getTechInfo(iRevealTech).getButton(), enumBS, eWidJuToTech, iRevealTech, 1, False)
			else:
				if iRevealTech != -1:
					childPanelName = aName()
					screen.attachPanel(panelName, childPanelName, "", "", True, True, ePanelEmpty)
					screen.attachLabel(childPanelName, "", szfont4b + TRNSLTR.getText("TXT_KEY_PEDIA_BONUS_APPEARANCE", ()))
					screen.attachImageButton(childPanelName, "", GC.getTechInfo(iRevealTech).getButton(), enumBS, eWidJuToTech, iRevealTech, 1, False)
				if iEnableTech != -1:
					childPanelName = aName()
					screen.attachPanel(panelName, childPanelName, "", "", True, True, ePanelEmpty)
					screen.attachLabel(childPanelName, "", szfont4b + TRNSLTR.getText("TXT_KEY_PEDIA_BONUS_TRADE", ()))
					screen.attachImageButton(childPanelName, "", GC.getTechInfo(iEnableTech).getButton(), enumBS, eWidJuToTech, iEnableTech, 1, False)
			if iObsoleteTech != -1:
				childPanelName = aName()
				screen.attachPanel(panelName, childPanelName, "", "", True, True, ePanelEmpty)
				screen.attachLabel(childPanelName, "", szfont4b + TRNSLTR.getText("TXT_KEY_PEDIA_BONUS_OBSOLETE", ()))
				screen.attachImageButton(childPanelName, "", GC.getTechInfo(iObsoleteTech).getButton(), enumBS, eWidJuToTech, iObsoleteTech, 1, False)
		# Improvement
		aImpList = []
		if bMapBonus:
			for iImprovement in xrange(GC.getNumImprovementInfos()):
				CvImprovementInfo = GC.getImprovementInfo(iImprovement)
				if CvImprovementInfo.isImprovementBonusTrade(iTheBonus) and not CvImprovementInfo.isActsAsCity():
					szYield = " " + szBonusChar
				else:
					szYield = ""
				for k in xrange(eNumYieldTypes):
					iYieldChange = CvImprovementInfo.getImprovementBonusYield(iTheBonus, k)
					if iYieldChange:
						iYieldChange += CvImprovementInfo.getYieldChange(k)
						if iYieldChange < 0:
							szYield += " <color=255,0,0,255>"
						else:
							szYield += " <color=0,230,0,255>"
						szYield += str(iYieldChange) + (u'%c' % (GC.getYieldInfo(k).getChar()))
				if szYield:
					aImpList.append((iImprovement,  szfont3b + szYield))
		if aSourceOfBonus or aImpList:
			if aSourceOfBonus and aImpList:
				W_IMP = W_SoB = self.W_HALF_PP
				X_IMP = self.X_COL_2
				W_3RD_PP = self.W_3RD_PP
				if len(aImpList) < 4:
					if len(aSourceOfBonus) > 4:
						W_IMP = W_3RD_PP
						W_SoB = W_PEDIA_PAGE - W_3RD_PP - 4
						X_IMP = X_COL_1 + W_SoB + 8
				elif len(aSourceOfBonus) < 4:
					if len(aImpList) > 4:
						W_IMP = W_PEDIA_PAGE - W_3RD_PP - 4
						W_SoB = W_3RD_PP
						X_IMP = X_COL_1 + W_SoB + 8
				sobPanel = aName()
				impPanel = aName()
				screen.addPanel(sobPanel, TRNSLTR.getText("TXT_KEY_PEDIA_RESOURCE_SOURCE", ()), "", False, True, X_COL_1, Y_TOP_ROW_2, W_SoB, H_BOT_ROW + 16, ePanelBlue50)
				screen.addPanel(impPanel, TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_IMPROVEMENT", ()), "", False, True, X_IMP, Y_TOP_ROW_2, W_IMP, H_BOT_ROW + 16, ePanelBlue50)
			elif aSourceOfBonus:
				sobPanel = aName()
				szSource = TRNSLTR.getText("TXT_KEY_PEDIA_RESOURCE_SOURCE", ())
				screen.addPanel(sobPanel, szSource, "", False, True, X_COL_1, Y_TOP_ROW_2, W_PEDIA_PAGE, H_BOT_ROW + 16, ePanelBlue50)
			else:
				impPanel = aName()
				szImp = TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_IMPROVEMENT", ())
				screen.addPanel(impPanel, szImp, "", False, True, X_COL_1, Y_TOP_ROW_2, W_PEDIA_PAGE, H_BOT_ROW + 16, ePanelBlue50)
			if aSourceOfBonus:
				for i, iBuilding in enumerate(aSourceOfBonus):
					screen.attachImageButton(sobPanel, "", GC.getBuildingInfo(iBuilding).getButton(), enumGBS, eWidJuToBuilding, iBuilding, 1, False)
			if aImpList:
				for i in xrange(len(aImpList)):
					childPanelName = aName()
					screen.attachPanel(impPanel, childPanelName, "", "", True, True, ePanelEmpty)
					screen.attachImageButton(childPanelName, "", GC.getImprovementInfo(aImpList[i][0]).getButton(), enumGBS, eWidJuToImprove, aImpList[i][0], 1, False)
					screen.attachLabel(childPanelName, "", aImpList[i][1])
		else:
			Y_TOP_ROW_3 -= H_BOT_ROW + 16
			H_MID += H_BOT_ROW + 16
		# Buildings Enabled
		if aNeededByBuildings or aVicinityBuildings:
			szBuildingsEnabled = TRNSLTR.getText("TXT_KEY_PEDIA_BUILDINGS_ENABLED", ())
			if aNeededByBuildings and not aVicinityBuildings:
				szBuildingsEnabled += " " + TRNSLTR.getText("TXT_KEY_PEDIA_BONUS_NATIONWIDE", ())
			elif aVicinityBuildings and not aNeededByBuildings:
				szBuildingsEnabled += " " + TRNSLTR.getText("TXT_KEY_PEDIA_BONUS_LOCALLY", ())
			panelName = aName()
			screen.addPanel(panelName, szBuildingsEnabled, "", False, True, X_COL_1, Y_BOT_ROW_1, W_PEDIA_PAGE, H_BOT_ROW, ePanelBlue50)

			for iBuilding in aNeededByBuildings:
				screen.attachImageButton(panelName, "", GC.getBuildingInfo(iBuilding).getButton(), enumGBS, eWidJuToBuilding, iBuilding, 1, False)

			if aVicinityBuildings:
				szBracketL = ""
				szBracketR = ""
				if aNeededByBuildings:
					szBracketL = szfont3 + " &#38 " + szfont4b + "{" + szfont3 + TRNSLTR.getText("TXT_KEY_PEDIA_BONUS_LOCALLY", ()) + ": "
					szBracketR = szfont4b + "}"
				if szBracketL:
					screen.attachLabel(panelName, "", szBracketL)
				for iBuilding in aVicinityBuildings:
					screen.attachImageButton(panelName, "", GC.getBuildingInfo(iBuilding).getButton(), enumGBS, eWidJuToBuilding, iBuilding, 1, False)
				if szBracketR:
					screen.attachLabel(panelName, "", szBracketR)
		else:
			Y_BOT_ROW_2 += H_BOT_ROW
			Y_BOT_ROW_3 += H_BOT_ROW
			H_MID += H_BOT_ROW
		# Units Enabled
		if aNeededByUnits:
			panelName = aName()
			screen.addPanel(panelName, TRNSLTR.getText("TXT_KEY_PEDIA_UNITS_ENABLED", ()), "", False, True, X_COL_1, Y_BOT_ROW_2, W_PEDIA_PAGE, H_BOT_ROW, ePanelBlue50)
			for iUnit in aNeededByUnits:
				screen.attachImageButton(panelName, "", GC.getUnitInfo(iUnit).getButton(), enumGBS, eWidJuToUnit, iUnit, 1, False)
		else:
			H_MID += H_BOT_ROW
			Y_BOT_ROW_3 += H_BOT_ROW
		# Units & Buildings Affected
		if aAffectedBuildings or aAffectedUnits:
			if aAffectedBuildings and aAffectedUnits:
				W_UNIT = W_BUIL = self.W_HALF_PP
				X_UNIT = self.X_COL_2
				W_3RD_PP = self.W_3RD_PP
				if len(aAffectedUnits) < 4:
					if len(aAffectedBuildings) > 4:
						W_UNIT = W_3RD_PP
						W_BUIL = W_PEDIA_PAGE - W_3RD_PP - 4
						X_UNIT = X_COL_1 + W_BUIL + 8
				elif len(aAffectedBuildings) < 4:
					if len(aAffectedUnits) > 4:
						W_UNIT = W_PEDIA_PAGE - W_3RD_PP - 4
						W_BUIL = W_3RD_PP
						X_UNIT = X_COL_1 + W_BUIL + 8
				builPanel = aName()
				unitPanel = aName()
				screen.addPanel(builPanel, TRNSLTR.getText("TXT_KEY_BONUSHELP_EFFECTS_BUILDING", ()), "", False, True, X_COL_1, Y_BOT_ROW_3, W_BUIL, H_BOT_ROW, ePanelBlue50)
				screen.addPanel(unitPanel, TRNSLTR.getText("TXT_KEY_BONUSHELP_EFFECTS_UNIT", ()), "", False, True, X_UNIT, Y_BOT_ROW_3, W_UNIT, H_BOT_ROW, ePanelBlue50)
			elif aAffectedBuildings:
				builPanel = aName()
				szBuild = TRNSLTR.getText("TXT_KEY_BONUSHELP_EFFECTS_BUILDING", ())
				screen.addPanel(builPanel, szBuild, "", False, True, X_COL_1, Y_BOT_ROW_3, W_PEDIA_PAGE, H_BOT_ROW, ePanelBlue50)
			else:
				unitPanel = aName()
				szUnit = TRNSLTR.getText("TXT_KEY_BONUSHELP_EFFECTS_UNIT", ())
				screen.addPanel(unitPanel, szUnit, "", False, True, X_COL_1, Y_BOT_ROW_3, W_PEDIA_PAGE, H_BOT_ROW, ePanelBlue50)
			if aAffectedBuildings:
				for iBuilding in aAffectedBuildings:
					screen.attachImageButton(builPanel, "", GC.getBuildingInfo(iBuilding).getButton(), enumGBS, eWidJuToBuilding, iBuilding, 1, False)
			if aAffectedUnits:
				szBracketL = szfont4b + " {"
				szBracketR = szfont4b + "} "
				szChar = u'%c' %(GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar())
				aAffectedUnits.sort()
				test = 0
				for i, entry in enumerate(aAffectedUnits):
					iModifier, iUnit = entry
					if iModifier != test:
						szTxt = szBracketL
						if test != 0:
							screen.attachLabel(unitPanel, "", szBracketR)
						if iModifier < 0:
							szTxt += " <color=255,0,0,255>"
						else:
							szTxt += " <color=0,230,0,255>"
						szTxt += str(iModifier) + "%" + szChar
						screen.attachLabel(unitPanel, "", szTxt)
						test = iModifier
					screen.attachImageButton(unitPanel, "", GC.getUnitInfo(iUnit).getButton(), enumGBS, eWidJuToUnit, iUnit, 1, False)
				screen.attachLabel(unitPanel, "", szBracketR)
		else:
			H_MID += H_BOT_ROW
		# History
		szTxt = CvTheBonusInfo.getCivilopedia()
		if szTxt:
			screen.addPanel(aName(), "", "", True, True, X_COL_1, Y_TOP_ROW_3, W_PEDIA_PAGE, H_MID, ePanelBlue50)
			szTxt = szfont3b + TRNSLTR.getText("TXT_KEY_CIVILOPEDIA_BACKGROUND", ()) + szfont3 + CvTheBonusInfo.getCivilopedia()
			screen.addMultilineText(aName(), szTxt, X_COL_1 + 4, Y_TOP_ROW_3 + 8, W_PEDIA_PAGE - 8, H_MID - 16, eWidGen, 1, 2, 1<<0)
