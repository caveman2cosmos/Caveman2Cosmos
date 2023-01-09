# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaCorporation:

	def __init__(self, parent, H_BOT_ROW):
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

		self.S_ICON = S_ICON = H_TOP_ROW - H_TOP_ROW % 8

		self.X_MAIN_CONT = X_COL_1 + S_ICON + 4
		self.Y_MAIN_CONT = Y_TOP_ROW + H_TOP_ROW / 8
		self.W_MAIN_CONT = W_PEDIA_PAGE - S_ICON - 4
		self.H_MAIN_CONT = H_TOP_ROW - H_TOP_ROW / 8

	def interfaceScreen(self, iTheCorporation):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		CvTheCorporationInfo = GC.getCorporationInfo(iTheCorporation)
		screen = self.main.screen()

		iWidGeneral		= WidgetTypes.WIDGET_GENERAL
		iWidJuToBonus	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS
		iWidJuToBuild	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING
		iWidJuToUnit	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT
		iPanelBlue50	= PanelStyles.PANEL_STYLE_BLUE50
		iPanelEmpty		= PanelStyles.PANEL_STYLE_EMPTY

		enumGBS = self.main.enumGBS
		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2 = self.main.aFontList

		S_ICON = self.S_ICON
		H_TOP_ROW = self.H_TOP_ROW
		H_BOT_ROW = self.H_BOT_ROW
		H_TOP_ROW_2 = H_TOP_ROW + H_BOT_ROW
		X_COL_1 = self.X_COL_1
		X_COL_2 = self.X_COL_2
		Y_TOP_ROW_1 = self.Y_TOP_ROW
		Y_TOP_ROW_2 = Y_TOP_ROW_1 + H_TOP_ROW
		Y_BOT_ROW_1 = self.Y_BOT_ROW
		Y_HIST = Y_TOP_ROW_2 + H_TOP_ROW_2
		H_HIST = Y_BOT_ROW_1 - Y_HIST
		W_PEDIA_PAGE = self.W_PEDIA_PAGE
		W_HALF_PP = self.W_HALF_PP

		# Main Panel
		szCorpName = u'%c ' % CvTheCorporationInfo.getHeadquarterChar() + CvTheCorporationInfo.getDescription() + u' %c' % CvTheCorporationInfo.getChar()
		screen.setText(self.main.getNextWidgetName(), "", uFontEdge + szCorpName, 1<<0, X_COL_1, 0, 0, FontTypes.TITLE_FONT, iWidGeneral, 0, 0)
		screen.addPanel(self.main.getNextWidgetName(), "", "", False, False, X_COL_1, Y_TOP_ROW_1 + 3, W_PEDIA_PAGE, H_TOP_ROW, PanelStyles.PANEL_STYLE_MAIN)
		screen.addDDSGFC(self.main.getNextWidgetName(), CvTheCorporationInfo.getButton(), X_COL_1 - 2, Y_TOP_ROW_1 + 8, S_ICON, S_ICON, iWidGeneral, -1, -1)
		# Loop through buildings and units
		aReqBuildList = []
		aConsumesList = []
		aList1 = []
		aList2 = []
		for iBuilding in range(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			if CvBuildingInfo.getPrereqCorporation() == iTheCorporation:
				aList1.append((CvBuildingInfo.getButton(), iBuilding))
			elif CvBuildingInfo.getFoundsCorporation() == iTheCorporation:
				aReqBuildList.append((CvBuildingInfo.getButton(), iBuilding))
		for iUnit in range(GC.getNumUnitInfos()):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			if CvUnitInfo.getPrereqCorporation() == iTheCorporation:
				aList2.append((CvUnitInfo.getButton(), iUnit))
		# Requires
		iTech = CvTheCorporationInfo.getTechPrereq()
		aReqBonusListLength = 0
		for iBonus in CvTheCorporationInfo.getPrereqBonuses():
			aConsumesList.append((GC.getBonusInfo(iBonus).getButton(), iBonus))
			aReqBonusListLength += 1
		if aReqBuildList or aConsumesList or iTech != -1:
			enumBS = GenericButtonSizes.BUTTON_SIZE_CUSTOM
			X_MAIN_CONT = self.X_MAIN_CONT
			Y_MAIN_CONT = self.Y_MAIN_CONT
			W_MAIN_CONT = self.W_MAIN_CONT
			H_MAIN_CONT = self.H_MAIN_CONT
			mainCont = self.main.getNextWidgetName()
			screen.addPanel(mainCont, "", "", False, True, X_MAIN_CONT, Y_MAIN_CONT, W_MAIN_CONT, H_MAIN_CONT, iPanelEmpty)
			if aReqBuildList or iTech != -1:
				childMainCont = self.main.getNextWidgetName()
				childMainContList = self.main.getNextWidgetName()
				screen.attachPanel(mainCont, childMainCont, "", "", True, True, iPanelEmpty)
				screen.attachLabel(childMainCont, "", uFont4b + "Requires")
				screen.attachPanel(childMainCont, childMainContList, "", "", False, True, iPanelEmpty)
				if iTech != -1:
					screen.attachImageButton(childMainContList, "", GC.getTechInfo(iTech).getButton(), enumBS, WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH, iTech, 2, False)
					if aReqBuildList:
						screen.attachLabel(childMainContList, "", "<font=3> &#38 ")
				for i, entry in enumerate(aReqBuildList):
					screen.attachImageButton(childMainContList, "", entry[0], enumBS, iWidJuToBuild, entry[1], 1, False)
			if aConsumesList:
				childMainCont = self.main.getNextWidgetName()
				childMainContList = self.main.getNextWidgetName()
				screen.attachPanel(mainCont, childMainCont, "", "", True, True, iPanelEmpty)
				screen.attachLabel(childMainCont, "", uFont4b + "Consumes")
				screen.attachPanel(childMainCont, childMainContList, "", "", False, True, iPanelEmpty)
				if aReqBonusListLength > 1:
					screen.attachLabel(childMainContList, "", "<font=4b>{ ")
				for i, entry in enumerate(aConsumesList):
					screen.attachImageButton(childMainContList, "", entry[0], enumBS, iWidJuToBonus, entry[1], 1, False)
				if aReqBonusListLength > 1:
					screen.attachLabel(childMainContList, "", "<font=4b> }")
		# Effect
		szSpecialText = CyGameTextMgr().parseCorporationInfo(iTheCorporation, True)[1:]
		if szSpecialText:
			szSpecial = TRNSLTR.getText("TXT_KEY_PEDIA_EFFECTS", ())
			screen.addPanel(self.main.getNextWidgetName(), szSpecial, "", False, True, X_COL_1, Y_TOP_ROW_2, W_PEDIA_PAGE, H_TOP_ROW_2, iPanelBlue50)
			panelName = self.main.getNextWidgetName()
			screen.addMultilineText(panelName, uFont3 + szSpecialText, X_COL_1 + 4, Y_TOP_ROW_2 + 32, W_PEDIA_PAGE - 8, H_TOP_ROW_2 - 40, iWidGeneral, -1, -1, 1<<0)
		else:
			Y_HIST -= H_TOP_ROW_2
			H_HIST += H_TOP_ROW_2
		# Unique Buildings & Units
		if aList1 or aList2:
			if aList1 and aList2:
				aListLength1 = len(aList1)
				aListLength2 = len(aList2)
				W_BUIL = W_UNIT = W_HALF_PP
				X_UNIT = X_COL_2
				W_3RD_PP = self.W_3RD_PP
				if aListLength2 < 4:
					if aListLength1 > 4:
						W_UNIT = W_3RD_PP
						W_BUIL = W_PEDIA_PAGE - W_3RD_PP - 4
						X_UNIT = X_COL_1 + W_BUIL + 8
				elif aListLength1 < 4:
					if aListLength2 > 4:
						W_UNIT = W_PEDIA_PAGE - W_3RD_PP - 4
						W_BUIL = W_3RD_PP
						X_UNIT = X_COL_1 + W_BUIL + 8
				buildingPanel = self.main.getNextWidgetName()
				unitPanel = self.main.getNextWidgetName()
				screen.addPanel(buildingPanel, TRNSLTR.getText("TXT_KEY_UNIQUE_BUILDINGS", ()), "", False, True, X_COL_1, Y_BOT_ROW_1, W_BUIL, H_BOT_ROW, iPanelBlue50)
				screen.addPanel(unitPanel, TRNSLTR.getText("TXT_KEY_FREE_UNITS", ()), "", False, True, X_UNIT, Y_BOT_ROW_1, W_UNIT, H_BOT_ROW, iPanelBlue50)
			elif aList1:
				buildingPanel = self.main.getNextWidgetName()
				screen.addPanel(buildingPanel, TRNSLTR.getText("TXT_KEY_UNIQUE_BUILDINGS", ()), "", False, True, X_COL_1, Y_BOT_ROW_1, W_PEDIA_PAGE, H_BOT_ROW, iPanelBlue50)
			else:
				unitPanel = self.main.getNextWidgetName()
				screen.addPanel(unitPanel, TRNSLTR.getText("TXT_KEY_FREE_UNITS", ()), "", False, True, X_COL_1, Y_BOT_ROW_1, W_PEDIA_PAGE, H_BOT_ROW, iPanelBlue50)
			for i, entry in enumerate(aList1):
				screen.attachImageButton(buildingPanel, "", entry[0], enumGBS, iWidJuToBuild, entry[1], 1, False)
			for i, entry in enumerate(aList2):
				screen.attachImageButton(unitPanel, "", entry[0], enumGBS, iWidJuToUnit, entry[1], 1, False)
		else:
			H_HIST += H_BOT_ROW
		# History
		szHistoryText = CvTheCorporationInfo.getCivilopedia()
		if szHistoryText:
			szHistory = TRNSLTR.getText("TXT_KEY_PEDIA_HISTORY", ())
			screen.addPanel(self.main.getNextWidgetName(), szHistory, "", False, True, X_COL_1, Y_HIST, W_PEDIA_PAGE, H_HIST, iPanelBlue50)
			panelName = self.main.getNextWidgetName()
			screen.addMultilineText(panelName, uFont2 + szHistoryText, X_COL_1 + 4, Y_HIST + 32, W_PEDIA_PAGE - 8, H_HIST - 40, iWidGeneral, -1, -1, 1<<0)
