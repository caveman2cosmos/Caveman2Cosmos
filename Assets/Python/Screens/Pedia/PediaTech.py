# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaTech:

	def __init__(self, parent, H_BOT_ROW):
		import HelperFunctions
		self.HF = HelperFunctions.HelperFunctions([0])

		self.main = parent

		H_PEDIA_PAGE = parent.H_PEDIA_PAGE

		self.Y_TOP_ROW = Y_TOP_ROW = parent.Y_PEDIA_PAGE
		self.Y_BOT_ROW = Y_BOT_ROW = Y_TOP_ROW + H_PEDIA_PAGE - H_BOT_ROW

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
		self.W_COL_2 = W_PEDIA_PAGE - W_BASE - 4
		self.W_COL_3 = W_HALF_PP - 4

		self.S_ICON = S_ICON = H_TOP_ROW - 10

		self.X_STATS = X_COL_1 + S_ICON - 8
		self.Y_STATS = Y_TOP_ROW + H_TOP_ROW / 6
		self.W_STATS = W_BASE - S_ICON - 16


	def interfaceScreen(self, iTheTech):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		CvTheTechInfo = GC.getTechInfo(iTheTech)
		screen = self.main.screen()
		CyPlayer = self.main.CyPlayer
		aName = self.main.getNextWidgetName

		eWidGen				= WidgetTypes.WIDGET_GENERAL
		eWidJuToBuilding	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING
		eWidJuToDerTech		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_DERIVED_TECH
		eWidJuToProject		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT
		eWidJuToUnit		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT
		ePnlBlue50	= PanelStyles.PANEL_STYLE_BLUE50
		eFontTitle	= FontTypes.TITLE_FONT
		enumGBS = self.main.enumGBS

		szfontEdge, szfont4b, szfont4, szfont3b, szfont3, szfont2b, szfont2 = self.main.aFontList

		W_COL_1 = self.W_COL_1
		W_COL_2 = self.W_COL_2
		W_COL_3 = self.W_COL_3
		W_PEDIA_PAGE = self.W_PEDIA_PAGE
		H_BOT_ROW = self.H_BOT_ROW
		H_TOP_ROW = self.H_TOP_ROW
		H_HISTORY = H_TOP_ROW * 2
		H_SPECIAL = H_TOP_ROW * 3
		S_ICON = self.S_ICON
		X_COL_1 = self.X_COL_1
		X_COL_2 = self.X_COL_2
		Y_TOP_ROW_1 = self.Y_TOP_ROW
		Y_TOP_ROW_2 = Y_TOP_ROW_1 + H_TOP_ROW
		Y_STRATEGY	= Y_TOP_ROW_2 + H_HISTORY
		Y_BOT_ROW_1 = self.Y_BOT_ROW
		Y_BOT_ROW_2 = Y_BOT_ROW_1 - H_BOT_ROW
		Y_BOT_ROW_3 = Y_BOT_ROW_2 - H_BOT_ROW
		H_SCROLL = H_BOT_ROW - 50
		S_BOT_ROW = self.S_BOT_ROW

		# Main Panel
		screen.setText(aName(), "", szfontEdge + CvTheTechInfo.getDescription(), 1<<0, X_COL_1, 0, 0, eFontTitle, eWidGen, 0, 0)
		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_TOP_ROW_1 + 2, W_COL_1 + 8, H_TOP_ROW + 2, PanelStyles.PANEL_STYLE_MAIN)
		Img = "Preview|Quote|TECH|ToolTip" + str(iTheTech)
		self.main.aWidgetBucket.append(Img)
		screen.setImageButtonAt(Img, Pnl, CvTheTechInfo.getButton(), 4, 6, S_ICON, S_ICON, eWidGen, 1, 1)
		# Stats
		Pnl = aName()
		screen.addListBoxGFC(Pnl, "", self.X_STATS, self.Y_STATS, self.W_STATS, H_TOP_ROW - 12, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(Pnl, False)

		iEra = CvTheTechInfo.getEra()
#		szTechEra = TRNSLTR.changeTextColor(GC.getEraInfo(iEra).getDescription(), iEra * 255 / GC.getNumEraInfos())
		if CyPlayer:
			szTechCost = TRNSLTR.getText("TXT_KEY_PEDIA_COST", (GC.getTeam(CyPlayer.getTeam()).getResearchCost(iTheTech),))
		else:
			szTechCost = TRNSLTR.getText("%d1_Num", (CvTheTechInfo.getResearchCost(),))
		szCostText = szTechCost + u"%c" % (GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar())
		screen.appendListBoxStringNoUpdate(Pnl, szfont4b + GC.getEraInfo(iEra).getDescription(), eWidGen, 0, 0, 1<<2)
		screen.appendListBoxStringNoUpdate(Pnl, szfont4b + szCostText, eWidGen, 0, 0, 1<<2)

		iHappiness = CvTheTechInfo.getHappiness()
		iHealth = CvTheTechInfo.getHealth()
		iTradeRoutes = CvTheTechInfo.getTradeRoutes()
		szText = ""
		bPlus = False
		if iHappiness:
			if iHappiness > 0:
				szText += "<color=0,230,0,255>%d" %iHappiness + unichr(8850)
			else:
				szText += "<color=255,0,0,255>%d" %-iHappiness + unichr(8851)
			bPlus = True
		if iHealth:
			if bPlus:
				szText += " "
			else:
				bPlus = True
			if iHealth > 0:
				szText += "<color=0,230,0,255>%d" %iHealth + unichr(8852)
			else:
				szText += "<color=255,0,0,255>%d" %-iHealth + unichr(8853)
		if iTradeRoutes:
			if bPlus:
				szText += " "
			if iTradeRoutes < 0:
				szText += "<color=255,0,0,255>"
			else:
				szText += "<color=0,230,0,255>"
			szText += "%d" %iTradeRoutes + unichr(8860)
		if szText:
			screen.appendListBoxStringNoUpdate(Pnl, szfont3b + szText, eWidGen, 0, 0, 1<<2)

		iWorkerSpeedModifier = CvTheTechInfo.getWorkerSpeedModifier()
		if iWorkerSpeedModifier:
			if iWorkerSpeedModifier < 0:
				szText = "<color=255,0,0,255>"
			else:
				szText = "<color=0,230,0,255>"
			szText += str(iWorkerSpeedModifier) + TRNSLTR.getText("TXT_KEY_PEDIA_TECH_WORKER_SPEED", ())
			screen.appendListBoxStringNoUpdate(Pnl, szfont3b + szText, eWidGen, 0, 0, 1<<2)

		screen.updateListBox(Pnl)
		# Buildings Enabled
		aList1 = []
		aList2 = []
		aList3 = []
		for iBuilding in range(GC.getNumBuildingInfos()):
			#Supports GOM type tech requirement
			aGOMTechReqList = []
			for i in range(2):
				aGOMTechReqList.append([])
			self.HF.getGOMReqs(GC.getBuildingInfo(iBuilding).getConstructCondition(), GOMTypes.GOM_TECH, aGOMTechReqList)
			if isTechRequiredForBuilding(iTheTech, iBuilding) or iTheTech in aGOMTechReqList[BoolExprTypes.BOOLEXPR_AND] or iTheTech in aGOMTechReqList[BoolExprTypes.BOOLEXPR_OR]:
				aList1.append(iBuilding)
		for iProject in range(GC.getNumProjectInfos()):
			if isTechRequiredForProject(iTheTech, iProject):
				aList2.append(iProject)
		if aList1 or aList2:
			Pnl = aName()
			szBuildingsEnabled = TRNSLTR.getText("TXT_KEY_PEDIA_BUILDINGS_ENABLED", ())
			screen.addPanel(Pnl, szBuildingsEnabled, "", False, True, X_COL_1, Y_BOT_ROW_1, W_PEDIA_PAGE, H_BOT_ROW, ePnlBlue50)
			if aList1:
				for i in range(len(aList1)):
					screen.attachImageButton(Pnl, "", GC.getBuildingInfo(aList1[i]).getButton(), enumGBS, eWidJuToBuilding, aList1[i], 1, False)
				aList1 = []
			if aList2:
				for i in range(len(aList2)):
					screen.attachImageButton(Pnl, "", GC.getProjectInfo(aList2[i]).getButton(), enumGBS, eWidJuToProject, aList2[i], 1, False)
				aList2 = []
		else:
			Y_BOT_ROW_3 += H_BOT_ROW
			Y_BOT_ROW_2 += H_BOT_ROW
			Y_STRATEGY += H_BOT_ROW
			H_HISTORY += H_BOT_ROW
			H_SPECIAL += H_BOT_ROW
		# Units Enabled
		for iUnit in range(GC.getNumUnitInfos()):
			#Supports GOM type tech requirement
			aGOMTechReqList = []
			for i in range(2):
				aGOMTechReqList.append([])
			self.HF.getGOMReqs(GC.getUnitInfo(iUnit).getTrainCondition(), GOMTypes.GOM_TECH, aGOMTechReqList)
			if isTechRequiredForUnit(iTheTech, iUnit) or iTheTech in aGOMTechReqList[BoolExprTypes.BOOLEXPR_AND] or iTheTech in aGOMTechReqList[BoolExprTypes.BOOLEXPR_OR]:
				aList1.append(iUnit)
		if aList1:
			Pnl = aName()
			screen.addPanel(Pnl, TRNSLTR.getText("TXT_KEY_PEDIA_UNITS_ENABLED", ()), "", False, True, X_COL_1, Y_BOT_ROW_2, W_PEDIA_PAGE, H_BOT_ROW, ePnlBlue50)
			for i in range(len(aList1)):
				screen.attachImageButton(Pnl, "", GC.getUnitInfo(aList1[i]).getButton(), enumGBS, eWidJuToUnit, aList1[i], 1, False)
			aList1 = []
		else:
			Y_BOT_ROW_3 += H_BOT_ROW
			Y_STRATEGY += H_BOT_ROW
			H_HISTORY += H_BOT_ROW
			H_SPECIAL += H_BOT_ROW
		# Requires
		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, True, X_COL_1, Y_BOT_ROW_3, W_COL_3, H_BOT_ROW, ePnlBlue50)
		szText = szfont3b + TRNSLTR.getText("TXT_KEY_PEDIA_REQUIRES", ())
		screen.setLabelAt(aName(), Pnl, szText, 1<<2, W_COL_3 / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
		AND = ["TXT", "<font=4b>&#38", 1<<2, 10, 14]
		OR = ["TXT", "<font=4b>||", 1<<2, 6, 10]
		braL = ["TXT", "<font=4b> {", 1<<0, 0, 14]
		braR = ["TXT", "<font=4b>} ", 1<<0, 0, 14]
		PF = "ToolTip|JumpTo|"
		n = 0
		szChild = PF + "TECH"
		# Tech Req
		for iType in CvTheTechInfo.getPrereqAndTechs():
			aList1.append([szChild + str(iType) + "|" + str(n), GC.getTechInfo(iType).getButton()])
			n += 1
		nOr = 0
		for iType in CvTheTechInfo.getPrereqOrTechs():
			aList2.append(iType)
			nOr += 1
		if aList2:
			if nOr > 1:
				aList1.append(braL)
			iType = aList2.pop(0)
			aList1.append([szChild + str(iType) + "|" + str(n), GC.getTechInfo(iType).getButton()])
			n += 1
			for iType in aList2:
				aList1.append(OR)
				aList1.append([szChild + str(iType) + "|" + str(n), GC.getTechInfo(iType).getButton()])
				n += 1
			if nOr > 1:
				aList1.append(braR)
			aList2 = []

		# Building Req
		szChild = PF + "BUILDING"
		for i in range(CvTheTechInfo.getNumPrereqBuildings()):
			iAmount = CvTheTechInfo.getPrereqBuildingMinimumRequired(i)
			if iAmount > 0:
				iType = CvTheTechInfo.getPrereqBuildingType(i)
				aList2.append((iType, iAmount))
		nOr = 0
		for i in range(CvTheTechInfo.getNumPrereqOrBuildings()):
			iAmount = CvTheTechInfo.getPrereqOrBuildingMinimumRequired(i)
			if iAmount > 0:
				iType = CvTheTechInfo.getPrereqOrBuildingType(i)
				aList3.append((iType, iAmount))
				nOr += 1
		if aList2 or aList3:
			if aList1:
				aList1.append(AND)

			if aList2:
				for iType, iAmount in aList2:
					if iAmount > 1:
						aList1.append(["TXT", "<font=4b>%d" % iAmount, 1<<1, 16, 0])
					aList1.append([szChild + str(iType) + "|" + str(n), GC.getBuildingInfo(iType).getButton()])
					n += 1
				aList2 = []

			if aList3:
				if nOr > 1:
					aList1.append(braL)
				iType, iAmount = aList3.pop(0)
				if iAmount > 1:
					aList1.append(["TXT", "<font=4b>%d" % iAmount, 1<<1, 16, 0])
				aList1.append([szChild + str(iType) + "|" + str(n), GC.getBuildingInfo(iType).getButton()])
				n += 1
				for iType, iAmount in aList3:
					aList1.append(OR)
					if iAmount > 1:
						aList1.append(["TXT", "<font=4b>%d" % iAmount, 1<<1, 16, 0])
					aList1.append([szChild + str(iType) + "|" + str(n), GC.getBuildingInfo(iType).getButton()])
					n += 1
				if nOr > 1:
					aList1.append(braR)
				aList3 = []

		if aList1:
			Pnl = aName()
			screen.addScrollPanel(Pnl, "", X_COL_1 - 2, Y_BOT_ROW_3 + 24, W_COL_3 + 4, H_SCROLL, ePnlBlue50)
			screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
			x = 4
			y = H_SCROLL / 2 - 12
			for entry in aList1:
				#NAME = entry[0]
				if entry[0] == "TXT":
					x += entry[3]
					screen.setLabelAt(aName(), Pnl, entry[1], entry[2], x, y, 0, eFontTitle, eWidGen, 0, 0)
					x += entry[4]
				else:
					screen.setImageButtonAt(entry[0], Pnl, entry[1], x, -2, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
					x += S_BOT_ROW + 4
			screen.hide(Pnl)
			screen.show(Pnl)
		# Leads To
		Pnl = aName()

		screen.addPanel(Pnl, TRNSLTR.getText("TXT_KEY_PEDIA_LEADS_TO", ()), "", False, True, self.X_COL_3, Y_BOT_ROW_3, W_COL_3, H_BOT_ROW, ePnlBlue50)
		for i in xrange(CvTheTechInfo.getNumLeadsToTechs()):
			iTechX = CvTheTechInfo.getLeadsToTech(i)
			screen.attachImageButton(Pnl, "", GC.getTechInfo(iTechX).getButton(), enumGBS, eWidJuToDerTech, iTechX, 1, False)

		# Quote
		szTxt = CvTheTechInfo.getQuote()
		if szTxt:
			szQuote = TRNSLTR.getText("TXT_KEY_PEDIA_QUOTE", ())
			screen.addPanel(aName(), szQuote, "", True, False, X_COL_2, Y_TOP_ROW_1, W_COL_2, H_TOP_ROW, ePnlBlue50)
			screen.addMultilineText(aName(), szfont2 + szTxt, X_COL_2 + 4, Y_TOP_ROW_1 + 32, W_COL_2 - 8, H_TOP_ROW - 40, eWidGen, 0, 0, 1<<0)

		# Special
		szSpecial = CyGameTextMgr().getTechHelp(iTheTech, True, False, False, False, -1)[1:]
		if iTheTech == GC.getInfoTypeForString("TECH_COLONIALISM"):
			szSpecial += TRNSLTR.getText("TXT_KEY_COLONAILISM_EXTRA_POP", ())
		elif iTheTech == GC.getInfoTypeForString("TECH_STEAM_POWER"):
			szSpecial += TRNSLTR.getText("TXT_KEY_STEAM_POWER_EXTRA_POP", ())
		# History
		szTxt = ""
		szTemp = CvTheTechInfo.getStrategy()
		if szTemp:
			szTxt += szfont2b + TRNSLTR.getText("TXT_KEY_CIVILOPEDIA_STRATEGY", ()) + szfont2 + szTemp + "\n\n"
		szTemp = CvTheTechInfo.getCivilopedia()
		if szTemp:
			szTxt += szfont2b + TRNSLTR.getText("TXT_KEY_CIVILOPEDIA_BACKGROUND", ()) + szfont2 + szTemp

		if szSpecial and szTxt:
			WIDTH = W_COL_2
		elif szTxt:
			WIDTH = W_PEDIA_PAGE
			X_COL_2 = X_COL_1
		if szSpecial:
			screen.addPanel(aName(), "", "", True, False, X_COL_1, Y_TOP_ROW_2, W_COL_1, H_SPECIAL, ePnlBlue50)
			screen.addMultilineText(aName(), szfont3 + szSpecial, X_COL_1 + 4, Y_TOP_ROW_2 + 8, W_COL_1 - 8, H_SPECIAL - 16, eWidGen, 0, 0, 1<<0)
		if szTxt:
			screen.addPanel(aName(), "", "", True, False, X_COL_2, Y_TOP_ROW_2, WIDTH, H_SPECIAL, ePnlBlue50)
			screen.addMultilineText(aName(), szTxt, X_COL_2 + 4, Y_TOP_ROW_2 + 8, WIDTH - 8, H_SPECIAL - 16, eWidGen, 0, 0, 1<<0)