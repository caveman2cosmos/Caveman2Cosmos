# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaUnit:

	def __init__(self, parent, H_BOT_ROW):
		import HelperFunctions
		self.HF = HelperFunctions.HelperFunctions([0])

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


	def interfaceScreen(self, iTheUnit):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		screen = self.main.screen()
		CvTheUnitInfo = GC.getUnitInfo(iTheUnit)
		CyPlayer = self.main.CyPlayer
		aName = self.main.getNextWidgetName

		eWidGen				= WidgetTypes.WIDGET_GENERAL
		eWidJuToBonus		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS
		eWidJuToBuilding	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING
		eWidJuToTech		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH
		eWidJuToUnit		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT
		eWidJuToPromo		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROMOTION
		eWidJuToCivic		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIVIC
		ePnlBlue50			= PanelStyles.PANEL_STYLE_BLUE50
		eFontTitle			= FontTypes.TITLE_FONT

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
		Y_BOT_ROW_3 = Y_BOT_ROW_2 - H_BOT_ROW
		W_COL_1 = self.W_COL_1
		W_COL_2 = self.W_COL_2
		W_COL_3 = self.W_COL_3
		W_PEDIA_PAGE = self.W_PEDIA_PAGE
		H_ROW_2 = H_TOP_ROW * 3
		S_BOT_ROW = self.S_BOT_ROW

		szText = CvTheUnitInfo.getDescription()
		iCombatType = CvTheUnitInfo.getUnitCombatType()
		if iCombatType != -1:
			if szfontEdge == "<font=4b>":
				aSize = 22
			elif szfontEdge == "<font=3b":
				aSize = 18
			else:
				aSize = 16
			CvCombatInfo = GC.getUnitCombatInfo(iCombatType)
			szText += " - " + '<img=%s size=%d></img>' %(CvCombatInfo.getButton(), aSize) + " " + CvCombatInfo.getDescription()

			Txt = "JumpTo|COMBAT" + str(iCombatType)
			self.main.aWidgetBucket.append(Txt)
		else:
			Txt = aName()
		screen.setText(Txt, "", szfontEdge + szText, 1<<0, X_COL_1, 0, 0, eFontTitle, eWidGen, iCombatType, 0)

		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_TOP_ROW_1 + 2, W_COL_1 + 8, H_TOP_ROW + 2, PanelStyles.PANEL_STYLE_MAIN)
		Img = "ToolTip|UNIT" + str(iTheUnit)
		screen.setImageButtonAt(Img, Pnl, CvTheUnitInfo.getButton(), 4, 6, S_ICON, S_ICON, eWidGen, 1, 1)

		# Stats
		szName = aName()
		screen.addListBoxGFC(szName, "", self.X_STATS, self.Y_STATS, self.W_STATS, self.H_STATS, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(szName, False)

		import TextUtil

		if CvTheUnitInfo.getDomainType() == DomainTypes.DOMAIN_AIR:
			iType = CvTheUnitInfo.getTotalModifiedAirCombatStrength100()
		else: iType = CvTheUnitInfo.getTotalModifiedCombatStrength100()

		if iType > 0:
			szText = TextUtil.floatToString(iType / 100.0) + " " + unichr(8855)
		else: szText = ""


		iType = CvTheUnitInfo.getMoves()
		if iType > 0:
			szTemp = "%d" %iType + unichr(8856)
			if szText:
				szText = szTemp + " " + szText
			else:
				szText = szTemp
		if szText:
			screen.appendListBoxStringNoUpdate(szName, szfont3 + szText, eWidGen, 0, 0, 1<<0)

		szText = ""
		iType = CvTheUnitInfo.getProductionCost()
		if iType >= 0:
			if CyPlayer:
				szText = str(CyPlayer.getUnitProductionNeeded(iTheUnit))
			elif not CvTheUnitInfo.isFound():
				szText = str(iType * GC.getDefineINT("UNIT_PRODUCTION_PERCENT")/100)
			if szText:
				szText += u" %c" % GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar()
				screen.appendListBoxStringNoUpdate(szName, szfont3 + szText, eWidGen, 0, 0, 1<<0)
		iType = CvTheUnitInfo.getAirRange()
		if iType > 0:
			szText = TRNSLTR.getText("TXT_KEY_PEDIA_RANGE", (iType,))
			screen.appendListBoxStringNoUpdate(szName, szfont3 + szText, eWidGen, 0, 0, 1<<0)

		iType = CvTheUnitInfo.getWorkRate()
		if iType > 0:
			szText = TRNSLTR.getText("TXT_KEY_PEDIA_WORKRATE", (iType,))
			screen.appendListBoxStringNoUpdate(szName, szfont3 + szText, eWidGen, 0, 0, 1<<0)
		screen.updateListBox(szName)

		PF = "ToolTip|JumpTo|"
		aList0 = []
		aList1 = []
		aList2 = []
		aList3 = []
		#Combat types
		for k in xrange(GC.getNumUnitCombatInfos()):
			if CvTheUnitInfo.isSubCombatType(k):
				CvUnitCombatInfo = GC.getUnitCombatInfo(k)
				if not CvUnitCombatInfo.isGraphicalOnly():
					aList0.append((CvUnitCombatInfo.getButton(), k))
		if aList0:
			Pnl = aName()
			screen.addPanel(Pnl, "", "", True, True, X_COL_2, Y_TOP_ROW_1, W_COL_2 - 4, H_TOP_ROW, ePnlBlue50)
			szTxt = szfont3b + TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_SUBCOMBAT_TYPE", ())
			screen.setLabelAt("", Pnl, szTxt, 1<<2, (W_COL_2 - 4) / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
			ScrlPnl = aName()
			screen.addScrollPanel(ScrlPnl, "", X_COL_2 - 2, Y_TOP_ROW_1, W_COL_2, H_TOP_ROW - 26, ePnlBlue50)
			screen.setStyle(ScrlPnl, "ScrollPanel_Alt_Style")
			aSize = (H_TOP_ROW - 48) / 2
			aSize = aSize - aSize % 4
			szChild = PF + "COMBAT"
			iDelta = aSize + 4
			x = 4
			y2 = H_TOP_ROW - 20 - aSize
			y1 = y2 - iDelta
			y = y1
			i = 0
			for BTN, iUnitCombat in aList0:
				screen.setImageButtonAt(szChild + str(iUnitCombat), ScrlPnl, BTN, x, y, aSize, aSize, eWidGen, 1, 2)

				if i % 2:
					x += iDelta
					y = y1
				else:
					y = y2
				i += 1

			aList0 = []

		# Graphic
		s = H_TOP_ROW - 6
		screen.addUnitGraphicGFC("Preview|Min", iTheUnit, X_COL_2 + W_COL_2 + 4, Y_TOP_ROW_1 + 8, s, s, eWidGen, iTheUnit, 0, -20, 30, 0.4, True)
		self.main.aWidgetBucket.append("Preview|Min")

		# Requires
		AND = ["TXT", "<font=4b>&#38", 1<<2, 10, 14]
		OR = ["TXT", "<font=4b>||", 1<<2, 6, 10]
		braL = ["TXT", "<font=4b> {", 1<<0, 0, 14]
		braR = ["TXT", "<font=4b>} ", 1<<0, 0, 14]
		# Tech Req, TODO: Add GOM AND/OR requirements, when units will have GOM tech requirements.
		aReqList = []
		n = 0
		szChild = PF + "TECH"
		iType = CvTheUnitInfo.getPrereqAndTech()
		if iType != -1:
			aReqList.append([szChild + str(iType) + "|" + str(n), GC.getTechInfo(iType).getButton()])
			n += 1
		for iType in CvTheUnitInfo.getPrereqAndTechs():
			aReqList.append([szChild + str(iType) + "|" + str(n), GC.getTechInfo(iType).getButton()])
			n += 1

		# Bonus Req
		szChild = PF + "BONUS"
		nOr = 0
		nGOMOr = 0

		# GOM bonus requirements
		aGOMBUnitReqList = []
		for i in range(2):
			aGOMBUnitReqList.append([])
		self.HF.getGOMReqs(CvTheUnitInfo.getTrainCondition(), GOMTypes.GOM_BONUS, aGOMBUnitReqList)

		for iType in CvTheUnitInfo.getPrereqOrBonuses():
			aList0.append(iType)
			n += 1
			nOr += 1
		for iType in xrange(len(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_OR])):
			aList2.append(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_OR][iType])
			n += 1
			nGOMOr += 1
		iType = CvTheUnitInfo.getPrereqAndBonus()
		if iType != -1 or aList0 or aList2:
			if aReqList:
				aReqList.append(AND)
			if iType != -1:
				aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBonusInfo(iType).getButton()])
				n += 1
			if aReqList and len(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_AND]) > 0:
				aReqList.append(AND)
			for iType in xrange(len(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_AND])):
				aReqList.append([szChild + str(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_AND][iType]) + "|" + str(n), GC.getBonusInfo(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_AND][iType]).getButton()])
				n += 1

			if aList0:
				if nOr > 1:
					aReqList.append(braL)
				iType = aList0.pop(0)
				aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBonusInfo(iType).getButton()])
				n += 1
				for iType in aList0:
					aReqList.append(OR)
					aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBonusInfo(iType).getButton()])
					n += 1
				if nOr > 1:
					aReqList.append(braR)
				aList0 = []
				nOr = 0
			if aList2:
				if nGOMOr > 1:
					aReqList.append(braL)
				iType = aList2.pop(0)
				aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBonusInfo(iType).getButton()])
				n += 1
				for iType in aList2:
					aReqList.append(OR)
					aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBonusInfo(iType).getButton()])
					n += 1
				if nGOMOr > 1:
					aReqList.append(braR)
				aList2 = []
				nGOMOr = 0

		# Civic Req
		szChild = PF + "CIVIC"
		for iType in xrange(GC.getNumCivicInfos()):
			if CvTheUnitInfo.isPrereqOrCivics(iType):
				aList0.append(iType)
				nOr += 1
		if aList0:
			if aReqList:
				aReqList.append(AND)
			if nOr > 1:
				aReqList.append(braL)
			iType = aList0.pop(0)
			aReqList.append([szChild + str(iType) + "|" + str(n), GC.getCivicInfo(iType).getButton()])
			n += 1
			for iType in aList0:
				aReqList.append(OR)
				aReqList.append([szChild + str(iType) + "|" + str(n), GC.getCivicInfo(iType).getButton()])
				n += 1
			if nOr > 1:
				aReqList.append(braR)
			aList0 = []
			nOr = 0
		# Religion Req
		szChild = PF + "REL"
		iType = CvTheUnitInfo.getPrereqReligion()
		if iType != -1:
			aReqList.append([szChild + str(iType) + "|" + str(n), GC.getReligionInfo(iType).getButton()])
			n += 1
		# Building Req
		szChild = PF + "BUILDING"
		for i in xrange(CvTheUnitInfo.getNumPrereqAndBuildings()):
			aList0.append(CvTheUnitInfo.getPrereqAndBuilding(i))

		for i in xrange(CvTheUnitInfo.getPrereqOrBuildingsNum()):
			aList1.append(CvTheUnitInfo.getPrereqOrBuilding(i))
			nOr += 1

		# GOM building requirements
		aGOMBUnitReqList = []
		for i in range(2):
			aGOMBUnitReqList.append([])
		self.HF.getGOMReqs(CvTheUnitInfo.getTrainCondition(), GOMTypes.GOM_BUILDING, aGOMBUnitReqList)
		# GOM AND requirements
		for GOMBuilding in xrange(len(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_AND])):
			aList2.append(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_AND][GOMBuilding])

		# GOM OR requirements
		nGOMOr = 0
		for GOMBuilding in xrange(len(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_OR])):
			aList3.append(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_OR][GOMBuilding])
			nGOMOr += 1

		if aList0 or aList1 or aList2 or aList3:
			if aReqList:
				aReqList.append(AND)
			for iType in aList0:
				aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBuildingInfo(iType).getButton()])
				n += 1

			if aReqList and len(aGOMBUnitReqList[BoolExprTypes.BOOLEXPR_AND]) > 0:
				aReqList.append(AND)
			for iType in aList2:
				aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBuildingInfo(iType).getButton()])
				n += 1

			if aList1:
				if nOr > 1:
					aReqList.append(braL)
				iType = aList1.pop(0)
				aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBuildingInfo(iType).getButton()])
				n += 1
				for iType in aList1:
					aReqList.append(OR)
					aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBuildingInfo(iType).getButton()])
					n += 1
				if nOr > 1:
					aReqList.append(braR)
				aList1 = []
				nOr = 0

			if aList3:
				if nGOMOr > 1:
					aReqList.append(braL)
				iType = aList3.pop(0)
				aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBuildingInfo(iType).getButton()])
				n += 1
				for iType in aList3:
					aReqList.append(OR)
					aReqList.append([szChild + str(iType) + "|" + str(n), GC.getBuildingInfo(iType).getButton()])
					n += 1
				if nGOMOr > 1:
					aReqList.append(braR)
				aList3 = []
				nGOMOr = 0

		# Upgrades To
		aUpgList = []
		szChild = PF + "UNIT"
		for i in xrange(CvTheUnitInfo.getNumUnitUpgrades()):
			iUnit = CvTheUnitInfo.getUnitUpgrade(i)
			aUpgList.append([szChild + str(iUnit),  GC.getUnitInfo(iUnit).getButton()])

		iType = CvTheUnitInfo.getObsoleteTech()
		if aReqList or aUpgList or iType != -1:
			W_BOT_ROW = W_PEDIA_PAGE
			x = (H_BOT_ROW - S_BOT_ROW) / 2
			if iType != -1:
				X_OBS = X_COL_1 + W_PEDIA_PAGE - H_BOT_ROW
				Pnl = aName()
				screen.addPanel(Pnl, "", "", True, True, X_OBS, Y_BOT_ROW_1, H_BOT_ROW, H_BOT_ROW, ePnlBlue50)
				szText = szfont3b + TRNSLTR.getText("TXT_KEY_PEDIA_BONUS_OBSOLETE", ())
				screen.setLabelAt(aName(), Pnl, szText, 1<<2, H_BOT_ROW / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
				screen.setImageButtonAt(PF + "TECH" + str(iType), Pnl, GC.getTechInfo(iType).getButton(), x, x + 4, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
				W_BOT_ROW -= H_BOT_ROW + 8
			if aReqList or aUpgList:
				H_SCROLL = H_BOT_ROW - 50
				i = len(aReqList)
				j = len(aUpgList)
				if aReqList and aUpgList:
					if i < 4 and j > 4:
						W_REQ = W_BOT_ROW / 3 - 4
						W_UPG = 2 * W_BOT_ROW / 3 - 4
					elif j < 4 and i > 4:
						W_REQ = 2 * W_BOT_ROW / 3 - 4
						W_UPG = W_BOT_ROW / 3 - 4
					else:
						W_REQ = W_UPG = W_BOT_ROW / 2 - 4
					X_UPG = X_COL_1 + W_REQ + 8
				else:
					W_REQ = W_UPG = W_BOT_ROW
					X_UPG = X_COL_1

				if aReqList:
					Pnl = aName()
					screen.addPanel(Pnl, "", "", False, True, X_COL_1, Y_BOT_ROW_1, W_REQ, H_BOT_ROW, ePnlBlue50)
					szText = szfont3b + TRNSLTR.getText("TXT_KEY_PEDIA_REQUIRES", ())
					screen.setLabelAt(aName(), Pnl, szText, 1<<2, W_REQ / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
					Pnl = aName()
					screen.addScrollPanel(Pnl, "", X_COL_1 - 2, Y_BOT_ROW_1 + 24, W_REQ + 4, H_SCROLL, ePnlBlue50)
					screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
					x = 4
					y = H_SCROLL / 2 - 12
					for entry in aReqList:
						NAME = entry[0]
						if entry[0] == "TXT":
							x += entry[3]
							screen.setLabelAt(aName(), Pnl, entry[1], entry[2], x, y, 0, eFontTitle, eWidGen, 0, 0)
							x += entry[4]
						else:
							screen.setImageButtonAt(entry[0], Pnl, entry[1], x, -2, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
							x += S_BOT_ROW + 4
					screen.hide(Pnl)
					screen.show(Pnl)
				if aUpgList:
					Pnl = aName()
					screen.addPanel(Pnl, "", "", False, True, X_UPG, Y_BOT_ROW_1, W_UPG, H_BOT_ROW, ePnlBlue50)
					szText = szfont3b + TRNSLTR.getText("TXT_KEY_PEDIA_UPGRADES_TO", ())
					screen.setLabelAt(aName(), Pnl, szText, 1<<2, W_UPG / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
					Pnl = aName()
					screen.addScrollPanel(Pnl, "", X_UPG - 2, Y_BOT_ROW_1 + 24, W_UPG + 4, H_SCROLL, ePnlBlue50)
					screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
					x = 4
					for NAME, BTN in aUpgList:
						screen.setImageButtonAt(NAME, Pnl, BTN, x, -2, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
						x += S_BOT_ROW + 4
					screen.hide(Pnl)
					screen.show(Pnl)
		else:
			H_ROW_2 += H_BOT_ROW
			Y_BOT_ROW_2 += H_BOT_ROW
			Y_BOT_ROW_3 += H_BOT_ROW

		# Promotions
		aList0 = []
		aList1 = []
		for k in xrange(GC.getNumPromotionInfos()):
			if CvTheUnitInfo.isQualifiedPromotionType(k):
				CvPromotionInfo = GC.getPromotionInfo(k)
				if not CvPromotionInfo.isGraphicalOnly():
					aList0.append((CvPromotionInfo.getButton(), k))
			if CvTheUnitInfo.getFreePromotions(k):
				CvPromotionInfo = GC.getPromotionInfo(k)
				for j in xrange(CvPromotionInfo.getNumAddsBuildTypes()):
					iBuild = CvPromotionInfo.getAddsBuildType(j)
					aList1.append((GC.getBuildInfo(iBuild).getButton(), iBuild))

		if aList0:
			screen.addPanel(aName(), "", "", False, False, X_COL_1, Y_BOT_ROW_2, W_PEDIA_PAGE, H_BOT_ROW, ePnlBlue50)
			Pnl = aName()
			screen.addScrollPanel(Pnl, "", X_COL_1 - 2, Y_BOT_ROW_2, W_PEDIA_PAGE + 4, H_BOT_ROW - 26, ePnlBlue50)
			screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
			aSize = (H_BOT_ROW - 24) / 2
			aSize = aSize - aSize % 4
			szChild = PF + "PROMO"
			x = 4
			y1 = 6
			y2 = H_BOT_ROW - 20 - aSize
			i = 0
			for BTN, iPromo in aList0:
				if i % 2:
					screen.setImageButtonAt(szChild + str(iPromo), Pnl, BTN, x, y2, aSize, aSize, eWidGen, 1, 1)
					x += aSize + 4
				else:
					screen.setImageButtonAt(szChild + str(iPromo), Pnl, BTN, x, y1, aSize, aSize, eWidGen, 1, 1)
				i += 1
		else:
			H_ROW_2 += H_BOT_ROW
			Y_BOT_ROW_3 += H_BOT_ROW

		# Builds
		for k in xrange(CvTheUnitInfo.getNumBuilds()):
			iBuild = CvTheUnitInfo.getBuild(k)
			aList1.append((GC.getBuildInfo(iBuild).getButton(), iBuild))

		if aList1:
			Pnl = aName()
			screen.addPanel(Pnl, "", "", False, True, X_COL_1, Y_BOT_ROW_3, W_PEDIA_PAGE, H_BOT_ROW, ePnlBlue50)
			szText = szfont3b + TRNSLTR.getText("TXT_KEY_PEDIA_BUILD", ())
			screen.setLabelAt(aName(), Pnl, szText, 1<<2, W_PEDIA_PAGE / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
			Pnl = aName()
			screen.addScrollPanel(Pnl, "", X_COL_1 - 2, Y_BOT_ROW_3 + 24,W_PEDIA_PAGE + 4, H_SCROLL, ePnlBlue50)
			screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
			x = 4
			y = H_SCROLL / 2 - 12
			szChild = PF + "BUILD"
			for BTN, iBuild in aList1:
				screen.setImageButtonAt(szChild + str(iBuild), Pnl, BTN, x, -2, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
				x += S_BOT_ROW + 4
			screen.hide(Pnl)
			screen.show(Pnl)
		else:
			H_ROW_2 += H_BOT_ROW

		# Special
		szSpecial = ""
		if CvTheUnitInfo.isIgnoreBuildingDefense():
			szSpecial += TRNSLTR.getText("TXT_KEY_PEDIA_UNIT_IGNORES_BUILDING_DEFENSE", ()) + "\n"
		if CvTheUnitInfo.getConscriptionValue() > 0:
			szSpecial += TRNSLTR.getText("TXT_KEY_PEDIA_UNIT_DRAFTABLE", ()) + "\n"
		if CvTheUnitInfo.getUnitCaptureType() > 0:
			szSpecial += TRNSLTR.getText("TXT_KEY_PEDIA_UNIT_CAN_BE_CAPTURED", ()) + "\n"
		szSpecial += CyGameTextMgr().getUnitHelp(iTheUnit, True, False, False, None)[1:]
		# History
		szText = ""
		szTemp = CvTheUnitInfo.getStrategy()
		if szTemp:
			szText += TRNSLTR.getText("TXT_KEY_CIVILOPEDIA_STRATEGY", ()) + szTemp + "\n\n"
		szTemp = CvTheUnitInfo.getCivilopedia()
		if szTemp:
			szText += TRNSLTR.getText("TXT_KEY_CIVILOPEDIA_BACKGROUND", ()) + szTemp

		if szSpecial and szText:
			WIDTH = W_COL_3
		elif szSpecial or szText:
			WIDTH = W_PEDIA_PAGE
			X_COL_3 = X_COL_1
		if szSpecial:
			screen.addPanel(aName(), "", "", True, False, X_COL_1, Y_TOP_ROW_2, WIDTH, H_ROW_2, ePnlBlue50)
			screen.addMultilineText(aName(), szfont3 + szSpecial, X_COL_1 + 4, Y_TOP_ROW_2 + 8, WIDTH - 8, H_ROW_2 - 16, eWidGen, 0, 0, 1<<0)
		if szText:
			screen.addPanel(aName(), "", "", True, False, X_COL_3, Y_TOP_ROW_2, WIDTH, H_ROW_2, ePnlBlue50)
			screen.addMultilineText(aName(), szfont2 + szText, X_COL_3 + 4, Y_TOP_ROW_2 + 8, WIDTH - 8, H_ROW_2 - 16, eWidGen, 0, 0, 1<<0)
