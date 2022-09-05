# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaBuilding:

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

		W_BASE = 64 + H_TOP_ROW + W_PEDIA_PAGE / 8
		W_HALF_PP = W_PEDIA_PAGE / 2

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE
		self.X_COL_2 = X_COL_1 + W_BASE + 4
		self.X_COL_3 = X_COL_1 + W_HALF_PP + 4

		self.W_COL_1 = W_BASE - 4
		self.W_COL_2 = W_PEDIA_PAGE - W_BASE - H_TOP_ROW - 4
		self.W_COL_3 = W_HALF_PP - 4

		self.S_ICON = S_ICON = H_TOP_ROW - 10

		a = H_TOP_ROW / 12
		self.X_STATS = X_COL_1 + S_ICON
		self.Y_STATS = Y_TOP_ROW + a
		self.H_STATS = H_TOP_ROW - a * 2 + 4
		self.W_STATS = W_BASE - S_ICON

	def interfaceScreen(self, iTheBuilding):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		screen = self.main.screen()
		CyPlayer = self.main.CyPlayer
		aName = self.main.getNextWidgetName

		CvTheBuildingInfo = GC.getBuildingInfo(iTheBuilding)
		bNotCulture = self.main.SECTION[1] != TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_C2C_CULTURES", ())

		eWidGen				= WidgetTypes.WIDGET_GENERAL
		ePanelBlue50		= PanelStyles.PANEL_STYLE_BLUE50

		enumGBS	= self.main.enumGBS
		szfontEdge, szfont4b, szfont4, szfont3b, szfont3, szfont2b, szfont2 = self.main.aFontList

		H_TOP_ROW = self.H_TOP_ROW
		H_BOT_ROW = self.H_BOT_ROW
		S_ICON = self.S_ICON
		X_COL_1 = self.X_COL_1
		X_COL_2 = self.X_COL_2
		X_COL_3 = self.X_COL_3
		Y_TOP_ROW_1 = self.Y_TOP_ROW
		Y_TOP_ROW_2 = Y_TOP_ROW_1 + H_TOP_ROW
		Y_BOT_ROW_1 = self.Y_BOT_ROW
		Y_BOT_ROW_2 = Y_BOT_ROW_1 - H_BOT_ROW
		W_COL_1 = self.W_COL_1
		W_COL_2 = self.W_COL_2
		W_COL_3 = self.W_COL_3
		W_PEDIA_PAGE = self.W_PEDIA_PAGE
		H_ROW_2 = H_TOP_ROW * 3 + H_BOT_ROW

		# Main Panel
		szBuildingName = CvTheBuildingInfo.getDescription()
		iMaxGlobalInstances = CvTheBuildingInfo.getMaxGlobalInstances()
		iMaxPlayerInstances = CvTheBuildingInfo.getMaxPlayerInstances()
		iMaxTeamInstances = CvTheBuildingInfo.getMaxTeamInstances()
		if iMaxGlobalInstances > 0:
			szBuildingName += "<color=192,192,128,255> | " + TRNSLTR.getText("TXT_KEY_PEDIA_WORLD_WONDER",()) + " - Max. " + str(iMaxGlobalInstances)
		elif iMaxPlayerInstances > 0:
			szBuildingName += "<color=192,192,128,255> | " + TRNSLTR.getText("TXT_KEY_PEDIA_NATIONAL_WONDER",()) + " - Max. " + str(iMaxPlayerInstances)
		elif iMaxTeamInstances > 0:
			szBuildingName += "<color=192,192,128,255> | " + TRNSLTR.getText("TXT_KEY_PEDIA_TEAM_WONDER",()) + " - Max. " + str(iMaxTeamInstances)
		screen.setText(aName(), "", szfontEdge + szBuildingName, 1<<0, X_COL_1, 0, 0, FontTypes.TITLE_FONT, eWidGen, 0, 0)

		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_TOP_ROW_1 + 2, W_COL_1 + 8, H_TOP_ROW + 2, PanelStyles.PANEL_STYLE_MAIN)
		Img = "Preview|Movie|BUILDING|ToolTip" + str(iTheBuilding)
		self.main.aWidgetBucket.append(Img)
		screen.setImageButtonAt(Img, Pnl, CvTheBuildingInfo.getButton(), 4, 6, S_ICON, S_ICON, eWidGen, 1, 1)
		# Stats
		panelName = aName()
		screen.addListBoxGFC(panelName, "", self.X_STATS, self.Y_STATS, self.W_STATS, self.H_STATS, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)
		iProductionCost = CvTheBuildingInfo.getProductionCost()
		if iProductionCost > 0:
			if CyPlayer:
				szCost = TRNSLTR.getText("TXT_KEY_PEDIA_COST", (CyPlayer.getBuildingProductionNeeded(iTheBuilding),))
			else:
				szCost = TRNSLTR.getText("TXT_KEY_PEDIA_COST", ((iProductionCost * GC.getDefineINT("BUILDING_PRODUCTION_PERCENT"))/100,))
			screen.appendListBoxStringNoUpdate(panelName, szfont3b + szCost + u'%c' %GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar(), eWidGen, 0, 0, 1<<0)
		elif CvTheBuildingInfo.isAutoBuild():
			screen.appendListBoxStringNoUpdate(panelName, szfont3 + TRNSLTR.getText("TXT_KEY_PEDIA_AUTOBUILD",()) , eWidGen, 0, 0, 1<<0)
		if CvTheBuildingInfo.isGoldenAge():
			szText1 = unichr(8866) + "<color=255,200,0,255> Golden Age"
			screen.appendListBoxStringNoUpdate(panelName, szfont3 + szText1, eWidGen, 0, 0, 1<<0)
		szText1 = ""
		szText2 = ""
		for k in range(YieldTypes.NUM_YIELD_TYPES):
			char = unichr(8483 + k)
			iTemp = CvTheBuildingInfo.getYieldChange(k)
			if iTemp:
				if iTemp < 0:
					szValue = " <color=255,0,0,255"
				else:
					szValue = " <color=0,230,0,255"
				szValue += ">%d" % iTemp + char
				szText1 += szValue
			iTemp = CvTheBuildingInfo.getYieldModifier(k)
			if iTemp:
				if iTemp < 0:
					szValue = " <color=255,0,0,255>"
				else:
					szValue = " <color=0,230,0,255>"
				szValue += "%d%%" % iTemp + char
				szText2 += szValue
			iTemp = CvTheBuildingInfo.getYieldPerPopChange(k)
			if iTemp:
				if iTemp < 0:
					szValue = " <color=255,0,0,255>"
				else:
					szValue = " <color=0,230,0,255>"
				szValue += unichr(8873) + "*%.2f" % (iTemp/100.0) + char
				screen.appendListBoxStringNoUpdate(panelName, szfont3 + szValue, eWidGen, 0, 0, 1<<0)
		if szText1:
			screen.appendListBoxStringNoUpdate(panelName, szfont3 + szText1, eWidGen, 0, 0, 1<<0)
			szText1 = ""
		if szText2:
			screen.appendListBoxStringNoUpdate(panelName, szfont3 + szText2, eWidGen, 0, 0, 1<<0)
			szText2 = ""
		for k in range(CommerceTypes.NUM_COMMERCE_TYPES):
			char = unichr(8500 + k)
			iTemp = CvTheBuildingInfo.getCommerceChange(k)
			if iTemp:
				if iTemp < 0:
					szValue = " <color=255,0,0,255>"
				else:
					szValue = " <color=0,230,0,255>"
				szValue += "%d" % iTemp + char
				szText1 += szValue
			iTemp = CvTheBuildingInfo.getCommerceModifier(k)
			if iTemp:
				if iTemp < 0:
					szValue = " <color=255,0,0,255>"
				else:
					szValue = " <color=0,230,0,255>"
				szValue += "%d%%" % iTemp + char
				szText2 += szValue
			iTemp = CvTheBuildingInfo.getCommercePerPopChange(k)
			if iTemp:
				if iTemp < 0:
					szValue = " <color=255,0,0,255>"
				else:
					szValue = " <color=0,230,0,255>"
				szValue += unichr(8873) + "*%.2f" % (iTemp/100.0) + char
				screen.appendListBoxStringNoUpdate(panelName, szfont3 + szValue, eWidGen, 0, 0, 1<<0)
		if szText1:
			screen.appendListBoxStringNoUpdate(panelName, szfont3 + szText1, eWidGen, 0, 0, 1<<0)
			szText1 = ""
		if szText2:
			screen.appendListBoxStringNoUpdate(panelName, szfont3 + szText2, eWidGen, 0, 0, 1<<0)
			szText2 = ""
		iHappiness = CvTheBuildingInfo.getHappiness()
		iHealth = CvTheBuildingInfo.getHealth()
		if CyPlayer:
			iHappiness += CyPlayer.getExtraBuildingHappiness(iTheBuilding)
			iHealth += CyPlayer.getExtraBuildingHealth(iTheBuilding)
		if iHappiness:
			if iHappiness > 0:
				szText1 += " <color=0,230,0,255>%d" %iHappiness + unichr(8850)
			else:
				szText1 += " <color=255,0,0,255>%d" %-iHappiness + unichr(8851)
		if iHealth:
			if iHealth > 0:
				szText1 += " <color=0,230,0,255>%d" %iHealth + unichr(8852)
			else:
				szText1 += " <color=255,0,0,255>%d" %-iHealth + unichr(8853)
		if szText1:
			screen.appendListBoxStringNoUpdate(panelName, szfont3 + szText1, eWidGen, 0, 0, 1<<0)
			szText1 = ""
		iGreatPeopleRateChange = CvTheBuildingInfo.getGreatPeopleRateChange()
		if iGreatPeopleRateChange:
			szText1 += " "
			if iGreatPeopleRateChange > 0:
				szText1 += "<color=0,230,0,255>%d" %iGreatPeopleRateChange + unichr(8862)
			else:
				szText1 += "<color=255,0,0,255>%d" %iGreatPeopleRateChange + unichr(8862)
			iGreatPeopleUnit = CvTheBuildingInfo.getGreatPeopleUnitType()
			if iGreatPeopleUnit != -1:
				szGreatPersonDesc = GC.getUnitInfo(iGreatPeopleUnit).getDescription()
				try:
					szText1 += "</color> (" + szGreatPersonDesc.split(" ")[1] + ")"
				except:
					szText1 += "</color> (" + szGreatPersonDesc + ")"
			screen.appendListBoxStringNoUpdate(panelName, szfont3 + szText1, eWidGen, 0, 0, 1<<0)
			szText1 = ""
		iGreatPeopleRateModifier = CvTheBuildingInfo.getGreatPeopleRateModifier()
		if iGreatPeopleRateModifier:
			if iGreatPeopleRateModifier < 0:
				szText1 += " <color=255,0,0,255>"
			else:
				szText1 += " <color=0,230,0,255>"
			szText1 += "%d%%" %iGreatPeopleRateModifier + unichr(8862)
		iGreatGeneralRateModifier = CvTheBuildingInfo.getGreatGeneralRateModifier()
		if iGreatGeneralRateModifier:
			if iGreatGeneralRateModifier < 0:
				szText1 += " <color=255,0,0,255>"
			else:
				szText1 += " <color=0,230,0,255>"
			szText1 += "%d%%" %iGreatGeneralRateModifier + unichr(8874)
		iGoldenAgeModifier = CvTheBuildingInfo.getGoldenAgeModifier()
		if iGoldenAgeModifier:
			if iGoldenAgeModifier < 0:
				szText1 += " <color=255,0,0,255>"
			else:
				szText1 += " <color=0,230,0,255>"
			szText1 += "%d%%" %iGoldenAgeModifier + unichr(8866)
		if szText1:
			screen.appendListBoxStringNoUpdate(panelName, szfont3b + szText1, eWidGen, 0, 0, 1<<0)
		screen.updateListBox(panelName)
		# Strategy
		szStrategy = TRNSLTR.getText("TXT_KEY_PEDIA_STRATEGY", ())
		screen.addPanel(aName(), szStrategy, "", False, False, X_COL_2, Y_TOP_ROW_1, W_COL_2 - 4, H_TOP_ROW, ePanelBlue50)
		szStrategyText = szfont2 + CvTheBuildingInfo.getStrategy()
		screen.addMultilineText(aName(), szStrategyText, X_COL_2 + 4, Y_TOP_ROW_1 + 32, W_COL_2 - 4, H_TOP_ROW - 40, eWidGen, 0, 0, 1<<0)
		# Graphic
		s = H_TOP_ROW - 6
		screen.addBuildingGraphicGFC("Preview|Min", iTheBuilding, X_COL_2 + W_COL_2 + 4, Y_TOP_ROW_1 + 8, s, s, eWidGen, iTheBuilding, 0, -20, 30, 0.4, True)
		self.main.aWidgetBucket.append("Preview|Min")
		# Replaced By
		PF = "ToolTip|JumpTo|"
		szChild = PF + "BUILDING"
		aList1 = []
		aList2 = []
		aList3 = []
		aList4 = []
		aList5 = []
		if bNotCulture:
			for i in xrange(CvTheBuildingInfo.getNumReplacedBuilding()):
				iReplaced = CvTheBuildingInfo.getReplacedBuilding(i)
				aList1.append((GC.getBuildingInfo(iReplaced), iReplaced))
			for i in xrange(CvTheBuildingInfo.getNumReplacementBuilding()):
				iReplacement = CvTheBuildingInfo.getReplacementBuilding(i)
				aList2.append((GC.getBuildingInfo(iReplacement), iReplacement))
			if aList1 or aList2:
				if aList1 and aList2:
					W_REP_1 = W_REP_2 = W_COL_3
					X_REP_2 = X_COL_3
					if len(aList1) < 4:
						if len(aList2) > 4:
							W_REP_1 = W_COL_1
							W_REP_2 = W_PEDIA_PAGE - W_COL_1 - 8
							X_REP_2 = X_COL_2
					elif len(aList2) < 4:
						if len(aList1) > 4:
							W_REP_1 = W_PEDIA_PAGE - W_COL_1 - 8
							W_REP_2 = W_COL_1
							X_REP_2 = X_COL_1 + W_REP_1 + 4
					replaceFor = aName()
					replacedBy = aName()
					screen.addPanel(replaceFor, TRNSLTR.getText("TXT_KEY_PEDIA_REPLACEMENT_FOR", ()), "", False, True, X_COL_1, Y_BOT_ROW_1, W_REP_1, H_BOT_ROW, ePanelBlue50)
					screen.addPanel(replacedBy, TRNSLTR.getText("TXT_KEY_PEDIA_REPLACED_BY", ()), "", False, True, X_REP_2, Y_BOT_ROW_1, W_REP_2, H_BOT_ROW, ePanelBlue50)
				elif aList1:
					replaceFor = aName()
					screen.addPanel(replaceFor, TRNSLTR.getText("TXT_KEY_PEDIA_REPLACEMENT_FOR", ()), "", False, True, X_COL_1, Y_BOT_ROW_1, W_PEDIA_PAGE, H_BOT_ROW, ePanelBlue50)
				elif aList2:
					replacedBy = aName()
					screen.addPanel(replacedBy, TRNSLTR.getText("TXT_KEY_PEDIA_REPLACED_BY", ()), "", False, True, X_COL_1, Y_BOT_ROW_1, W_PEDIA_PAGE, H_BOT_ROW, ePanelBlue50)
				if aList1:
					for i in range(len(aList1)):
						ID = aList1[i]
						screen.attachImageButton(replaceFor, szChild + str(ID[1]), ID[0].getButton(), enumGBS, eWidGen, 1, 1, False)
					aList1 = []
				if aList2:
					for i in range(len(aList2)):
						ID = aList2[i]
						screen.attachImageButton(replacedBy, szChild + str(ID[1]), ID[0].getButton(), enumGBS, eWidGen, 1, 1, False)
					aList2 = []
			else:
				Y_BOT_ROW_2 = Y_BOT_ROW_1
				H_ROW_2 += H_BOT_ROW
		else:
			Y_BOT_ROW_2 = Y_BOT_ROW_1
			H_ROW_2 += H_BOT_ROW
		# Requires
		panelName = aName()
		screen.addPanel(panelName, TRNSLTR.getText("TXT_KEY_PEDIA_REQUIRES", ()), "", False, True, X_COL_1, Y_BOT_ROW_2, W_PEDIA_PAGE, H_BOT_ROW, ePanelBlue50)
		szAnd	= szfont3 + "&#38"
		szOr	= szfont2b + "||"
		szBracketL = szfont4b + " {"
		szBracketR = szfont4b + "} "
		bPlus = False
		# Tech Req
		szChild = PF + "TECH"
		iType = CvTheBuildingInfo.getPrereqAndTech()
		if iType != -1:
			screen.attachImageButton(panelName, szChild + str(iType), GC.getTechInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
			bPlus = True
		i = 0
		for iType in CvTheBuildingInfo.getPrereqAndTechs():
			screen.attachImageButton(panelName, szChild + str(iType), GC.getTechInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
			bPlus = True
			i += 1

		# GOM tech requirements
		aGOMTechReqList = []
		for i in range(2):
			aGOMTechReqList.append([])
		self.HF.getGOMReqs(CvTheBuildingInfo.getConstructCondition(), GOMTypes.GOM_TECH, aGOMTechReqList)

		#GOM AND requirements
		for GOMTech in xrange(len(aGOMTechReqList[BoolExprTypes.BOOLEXPR_AND])):
			iType = aGOMTechReqList[BoolExprTypes.BOOLEXPR_AND][GOMTech]
			screen.attachImageButton(panelName, szChild + str(iType), GC.getTechInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
			bPlus = True
			i += 1

		# GOM OR requirements
		if len(aGOMTechReqList[BoolExprTypes.BOOLEXPR_OR]) > 0:
			screen.attachLabel(panelName, "", szBracketL)
			for GOMTech in xrange(len(aGOMTechReqList[BoolExprTypes.BOOLEXPR_OR])):
				iType = aGOMTechReqList[BoolExprTypes.BOOLEXPR_OR][GOMTech]
				screen.attachImageButton(panelName, szChild + str(iType), GC.getTechInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
				if GOMTech+1 != len(aGOMTechReqList[BoolExprTypes.BOOLEXPR_OR]):
					screen.attachLabel(panelName, "", szOr)
				bPlus = True
				i += 1
			screen.attachLabel(panelName, "", szBracketR)

		# Religion Req
		szChild = PF + "REL"
		iType = CvTheBuildingInfo.getPrereqReligion()
		if iType != -1:
			if bPlus:
				screen.attachLabel(panelName, "", szAnd)
			else:
				bPlus = True
			screen.attachImageButton(panelName, szChild + str(iType), GC.getReligionInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
		# Corporation Req
		szChild = PF + "CORP"
		iType = CvTheBuildingInfo.getPrereqCorporation()
		if iType != -1:
			if bPlus:
				screen.attachLabel(panelName, "", szAnd)
			else:
				bPlus = True
			screen.attachImageButton(panelName, szChild + str(iType), GC.getCorporationInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
		# Bonus Req
		# TODO: Expand functionality so it can handle 8 differently defined bonus requirements: Bonus, Bonuses, RawBonus, RawBonuses, VicinityRawBonus, VicinityRawBonuses, GOM AND/OR Bonus
		szChild = PF + "BONUS"
		iType = CvTheBuildingInfo.getPrereqAndBonus()
		nOr = 0
		for iCheck in CvTheBuildingInfo.getPrereqOrBonuses():
			aList1.append(iCheck)
			nOr += 1
		if bPlus:
			if iType != -1 or nOr:
				screen.attachLabel(panelName, "", szAnd)
		elif iType != -1 or nOr:
			bPlus = True
		if iType > -1:
			screen.attachImageButton(panelName, szChild + str(iType), GC.getBonusInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
		if nOr > 1:
			screen.attachLabel(panelName, "", szBracketL)
		i = 0
		if aList1:
			for iType in aList1:
				if i:
					screen.attachLabel(panelName, "", szOr)
				else: i = 1
				screen.attachImageButton(panelName, szChild + str(iType), GC.getBonusInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
			aList1 = []
		if nOr > 1:
			screen.attachLabel(panelName, "", szBracketR)
		# Corporation Bonus Req
		iType = CvTheBuildingInfo.getFoundsCorporation()
		if iType != -1:
			CvCorporationInfo = GC.getCorporationInfo(iType)
			lPrereqBonuses = CvCorporationInfo.getPrereqBonuses()
			nOr = len(lPrereqBonuses)
			if bPlus:
				if nOr:
					screen.attachLabel(panelName, "", szAnd)
			elif nOr:
				bPlus = True
			if nOr > 1:
				screen.attachLabel(panelName, "", szBracketL)
			for i in range(nOr):
				iType = lPrereqBonuses[i]
				if i != 0:
					screen.attachLabel(panelName, "", szOr)
				screen.attachImageButton(panelName, szChild + str(iType), GC.getBonusInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
			if nOr > 1:
				screen.attachLabel(panelName, "", szBracketR)
		# Building Req
		szChild = PF + "BUILDING"
		szChild1 = szChild + "|Own"
		# And building requirements
		for j in xrange(CvTheBuildingInfo.getNumPrereqInCityBuildings()):
			aList1.append(CvTheBuildingInfo.getPrereqInCityBuilding(j))

		# Empire building requirements
		for eBuildingX, iPrereqNumOfBuilding in CvTheBuildingInfo.getPrereqNumOfBuildings():
			if iPrereqNumOfBuilding > 0:
				if CyPlayer:
					aList3.append((eBuildingX, CyPlayer.getBuildingPrereqBuilding(iTheBuilding, eBuildingX, 0)))
				else:
					aList3.append((eBuildingX, iPrereqNumOfBuilding))

		# Or building requirements
		for j in xrange(CvTheBuildingInfo.getNumPrereqOrBuilding()):
			aList2.append(CvTheBuildingInfo.getPrereqOrBuilding(j))

		# GOM building requirements
		aGOMBuildingReqList = []
		for i in range(2):
			aGOMBuildingReqList.append([])
		self.HF.getGOMReqs(CvTheBuildingInfo.getConstructCondition(), GOMTypes.GOM_BUILDING, aGOMBuildingReqList)

		#GOM AND prereqs
		for GOMBuilding in xrange(len(aGOMBuildingReqList[BoolExprTypes.BOOLEXPR_AND])):
			aList4.append(aGOMBuildingReqList[BoolExprTypes.BOOLEXPR_AND][GOMBuilding])

		#GOM OR prereqs
		for GOMBuilding in xrange(len(aGOMBuildingReqList[BoolExprTypes.BOOLEXPR_OR])):
			aList5.append(aGOMBuildingReqList[BoolExprTypes.BOOLEXPR_OR][GOMBuilding])


		if aList1 or aList2 or aList3 or aList4 or aList5:
			if bPlus:
				screen.attachLabel(panelName, "", szAnd)
			else:
				bPlus = True
			if aList3:
				screen.attachLabel(panelName, "", szBracketL + szfont2b + TRNSLTR.getText("TXT_KEY_PEDIA_OWN", ()))
				for i in range(len(aList3)):
					iType, iAmount = aList3[i]
					screen.attachLabel(panelName, "", szfont4b + " " + str(iAmount))
					screen.attachImageButton(panelName, szChild1 + str(iType), GC.getBuildingInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
				screen.attachLabel(panelName, "", szBracketR)
				if aList2 and not aList1:
					screen.attachLabel(panelName, "", szAnd)
				aList3 = []
			if aList1:
				for i in range(len(aList1)):
					iType = aList1[i]
					screen.attachImageButton(panelName, szChild + str(iType), GC.getBuildingInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
				aList1 = []
			if aList2:
				iListLength = len(aList2)
				if iListLength > 1:
					screen.attachLabel(panelName, "", szBracketL)
				for i in range(iListLength):
					iType = aList2[i]
					if i != 0:
						screen.attachLabel(panelName, "", szOr)
					screen.attachImageButton(panelName, szChild + str(iType), GC.getBuildingInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
				if iListLength > 1:
					screen.attachLabel(panelName, "", szBracketR)
				aList2 = []
			if aList4:
				for i in range(len(aList4)):
					iType = aList4[i]
					screen.attachImageButton(panelName, szChild + str(iType), GC.getBuildingInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
				aList4 = []
			if aList5:
				iListLength = len(aList5)
				if iListLength > 1:
					screen.attachLabel(panelName, "", szBracketL)
				for i in range(iListLength):
					iType = aList5[i]
					if i != 0:
						screen.attachLabel(panelName, "", szOr)
					screen.attachImageButton(panelName, szChild + str(iType), GC.getBuildingInfo(iType).getButton(), enumGBS, eWidGen, 1, 1, False)
				if iListLength > 1:
					screen.attachLabel(panelName, "", szBracketR)
				aList5 = []

		# Civic Req
		szChild = PF + "CIVIC"
		for j in range(GC.getNumCivicInfos()):
			if CvTheBuildingInfo.isPrereqAndCivics(j):
				iCivic = CivicTypes(j)
				aList1.append(iCivic)
			elif CvTheBuildingInfo.isPrereqOrCivics(j):
				iCivic = CivicTypes(j)
				aList2.append(iCivic)
		if aList1 or aList2:
			if bPlus:
				screen.attachLabel(panelName, "", szAnd)
			else:
				bPlus = True
		if aList1:
			for i in range(len(aList1)):
				ID = aList1[i]
				screen.attachImageButton(panelName, szChild + str(ID), GC.getCivicInfo(ID).getButton(), enumGBS, eWidGen, 1, 1, False)
			aList1 = []
		if aList2:
			iListLength = len(aList2)
			if  iListLength > 1:
				screen.attachLabel(panelName, "", szBracketL)
			for i in range(iListLength):
				ID = aList2[i]
				if i != 0:
					screen.attachLabel(panelName, "", szOr)
				screen.attachImageButton(panelName, szChild + str(ID), GC.getCivicInfo(ID).getButton(), enumGBS, eWidGen, 1, 1, False)
			if iListLength > 1:
				screen.attachLabel(panelName, "", szBracketR)
			aList2 = []
		# Terrain Req
		szChild = PF + "TERRAIN"
		for i in range(GC.getNumTerrainInfos()):
			if CvTheBuildingInfo.isPrereqAndTerrain(i):
				aList1.append(i)
			elif CvTheBuildingInfo.isPrereqOrTerrain(i):
				aList2.append(i)
		if aList1 or aList2:
			if bPlus:
				screen.attachLabel(panelName, "", szAnd)
			else:
				bPlus = True
		if aList1:
			for i in range(len(aList1)):
				ID = aList1[i]
				screen.attachImageButton(panelName, szChild + str(ID), GC.getTerrainInfo(ID).getButton(), enumGBS, eWidGen, 1, 1, False)
			aList1 = []
		if aList2:
			iListLength = len(aList2)
			if  iListLength > 1:
				screen.attachLabel(panelName, "", szBracketL)
			for i in range(iListLength):
				ID = aList2[i]
				if i != 0:
					screen.attachLabel(panelName, "", szOr)
				screen.attachImageButton(panelName, szChild + str(ID), GC.getTerrainInfo(ID).getButton(), enumGBS, eWidGen, 1, 1, False)
			if iListLength > 1:
				screen.attachLabel(panelName, "", szBracketR)
			aList2 = []
		# Improvement Req
		szChild = PF + "IMP"
		for iPrereqOrImprovement in CvTheBuildingInfo.getPrereqOrImprovements():
			aList2.append(iPrereqOrImprovement)
		if aList2:
			if bPlus:
				screen.attachLabel(panelName, "", szAnd)
			else:
				bPlus = True
			iListLength = len(aList2)
			if  iListLength > 1:
				screen.attachLabel(panelName, "", szBracketL)
			for i in range(iListLength):
				ID = aList2[i]
				if i != 0:
					screen.attachLabel(panelName, "", szOr)
				screen.attachImageButton(panelName, szChild + str(ID), GC.getImprovementInfo(ID).getButton(), enumGBS, eWidGen, 1, 1, False)
			if iListLength > 1:
				screen.attachLabel(panelName, "", szBracketR)
			aList2 = []
		# Feature Req
		szChild = PF + "FEATURE"
		for i in range(GC.getNumFeatureInfos()):
			if CvTheBuildingInfo.isPrereqOrFeature(i):
				aList2.append(i)
		if aList2:
			if bPlus:
				screen.attachLabel(panelName, "", szAnd)
			else:
				bPlus = True
			iListLength = len(aList2)
			if  iListLength > 1:
				screen.attachLabel(panelName, "", szBracketL)
			for i in range(iListLength):
				ID = aList2[i]
				if i != 0:
					screen.attachLabel(panelName, "", szOr)
				screen.attachImageButton(panelName, szChild + str(ID), GC.getFeatureInfo(ID).getButton(), enumGBS, eWidGen, 1, 1, False)
			if iListLength > 1:
				screen.attachLabel(panelName, "", szBracketR)
			aList2 = []
		if not bPlus:
			screen.deleteWidget(panelName)
			H_ROW_2 += H_BOT_ROW
		# Special Abilities
		screen.addPanel(aName(), "", "", True, False, X_COL_1, Y_TOP_ROW_2, W_COL_3, H_ROW_2, ePanelBlue50)
		szSpecialText = szfont3 + CyGameTextMgr().getBuildingHelp(iTheBuilding, False, None, True, False, False)[1:]
		screen.addMultilineText(aName(), szSpecialText, X_COL_1 + 4, Y_TOP_ROW_2 + 12, W_COL_3 - 8, H_ROW_2 - 20, eWidGen, 0, 0, 1<<0)
		# History
		szHistory = TRNSLTR.getText("TXT_KEY_PEDIA_HISTORY", ())
		screen.addPanel(aName(), szHistory, "", True, False, X_COL_3, Y_TOP_ROW_2, W_COL_3, H_ROW_2, ePanelBlue50)
		szHistoryText = szfont2 + CvTheBuildingInfo.getCivilopedia()
		screen.addMultilineText(aName(), szHistoryText, X_COL_3 + 4, Y_TOP_ROW_2 + 32, W_COL_3 - 8, H_ROW_2 - 40, eWidGen, 0, 0, 1<<0)
