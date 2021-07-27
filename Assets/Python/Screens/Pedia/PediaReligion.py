# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class Page:

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

		W_BASE = 64 + H_TOP_ROW + W_PEDIA_PAGE / 8
		W_HALF_PP = W_PEDIA_PAGE / 2

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE
		self.X_COL_2 = X_COL_1 + W_BASE + 4
		self.X_COL_3 = X_COL_1 + W_HALF_PP + 4

		self.W_COL_1 = W_BASE - 4
		#self.W_COL_2 = W_PEDIA_PAGE - W_BASE - H_TOP_ROW - 4
		self.W_COL_3 = W_HALF_PP - 4

		self.S_ICON = S_ICON = H_TOP_ROW - 10


	def interfaceScreen(self, iReligion):
		import string
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		CvTheReligionInfo = GC.getReligionInfo(iReligion)
		aName = self.main.getNextWidgetName
		screen = self.main.screen()

		eWidGen				= WidgetTypes.WIDGET_GENERAL
		ePnlBlue50			= PanelStyles.PANEL_STYLE_BLUE50
		eFontTitle			= FontTypes.TITLE_FONT

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
		W_COL_1 = self.W_COL_1
		W_COL_3 = self.W_COL_3
		W_PEDIA_PAGE = self.W_PEDIA_PAGE
		H_ROW_2 = H_TOP_ROW * 3 + 2 * H_BOT_ROW
		S_BOT_ROW = self.S_BOT_ROW

		# Main Panel
		screen.setText(aName(), "",  szfontEdge + CvTheReligionInfo.getDescription(), 1<<0, X_COL_1, 0, 0, eFontTitle, eWidGen, 1, 1)
		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_TOP_ROW_1 + 2, H_TOP_ROW + 2, H_TOP_ROW + 2, PanelStyles.PANEL_STYLE_MAIN)
		screen.setImageButtonAt("ToolTip|RELIGION" + str(iReligion), Pnl, CvTheReligionInfo.getButton(), 4, 6, S_ICON, S_ICON, eWidGen, 1, 1)

		# Related Buildings/Units
		PF = "ToolTip|JumpTo|"
		szChild = PF + "BUILDING"
		buildings = []
		for iBuilding in range(GC.getNumBuildingInfos()):
			info = GC.getBuildingInfo(iBuilding)
			if iReligion == info.getPrereqReligion() or iReligion == info.getReligionType() or iReligion == info.getPrereqStateReligion():
				buildings.append([szChild + str(iBuilding),  info.getButton()])

		szChild = PF + "UNIT"
		units = []
		for iUnit in range(GC.getNumUnitInfos()):
			info = GC.getUnitInfo(iUnit)
			if iReligion == info.getPrereqReligion() or iReligion == info.getReligionType() or iReligion == info.getStateReligion():
				units.append([szChild + str(iUnit),  info.getButton()])

		if buildings or units:

			if buildings and units:
				W_REP_1 = W_REP_2 = W_COL_3
				X_REP_2 = X_COL_3
				if len(buildings) < 4:
					if len(units) > 4:
						W_REP_1 = W_COL_1
						W_REP_2 = W_PEDIA_PAGE - W_COL_1 - 8
						X_REP_2 = X_COL_2
				elif len(units) < 4:
					if len(buildings) > 4:
						W_REP_1 = W_PEDIA_PAGE - W_COL_1 - 8
						W_REP_2 = W_COL_1
						X_REP_2 = X_COL_1 + W_REP_1 + 4
			else:
				W_REP_1 = W_REP_2 = W_PEDIA_PAGE
				X_REP_2 = X_COL_1

			if buildings:
				Pnl = aName()
				screen.addPanel(Pnl, "", "", False, True, X_COL_1, Y_BOT_ROW_1, W_REP_1, H_BOT_ROW, ePnlBlue50)
				szText = szfont3b + TRNSLTR.getText("TXT_KEY_CONCEPT_BUILDINGS", ())
				screen.setLabelAt(aName(), Pnl, szText, 1<<2, W_REP_1 / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
				Pnl = aName()
				screen.addScrollPanel(Pnl, "", X_COL_1 - 2, Y_BOT_ROW_1 + 24, W_REP_1 + 4, H_BOT_ROW - 50, ePnlBlue50)
				screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
				x = 4
				for NAME, BTN in buildings:
					screen.setImageButtonAt(NAME, Pnl, BTN, x, -2, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
					x += S_BOT_ROW + 4
				screen.hide(Pnl)
				screen.show(Pnl)

			if units:
				Pnl = aName()
				screen.addPanel(Pnl, "", "", False, True, X_REP_2, Y_BOT_ROW_1, W_REP_2, H_BOT_ROW, ePnlBlue50)
				szText = szfont3b + TRNSLTR.getText("TXT_KEY_CONCEPT_UNITS", ())
				screen.setLabelAt(aName(), Pnl, szText, 1<<2, W_REP_2 / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
				Pnl = aName()
				screen.addScrollPanel(Pnl, "", X_REP_2 - 2, Y_BOT_ROW_1 + 24, W_REP_2 + 4, H_BOT_ROW - 50, ePnlBlue50)
				screen.setStyle(Pnl, "ScrollPanel_Alt_Style")
				x = 4
				for NAME, BTN in units:
					screen.setImageButtonAt(NAME, Pnl, BTN, x, -2, S_BOT_ROW, S_BOT_ROW, eWidGen, 1, 1)
					x += S_BOT_ROW + 4
				screen.hide(Pnl)
				screen.show(Pnl)
		else:
			H_ROW_2 += H_BOT_ROW

		# Special
		szSpecial = szfont3 + CyGameTextMgr().parseReligionInfo(iReligion, True)[1:]
		if szSpecial:
			screen.addPanel(aName(), "", "", True, False, X_COL_1, Y_TOP_ROW_2, W_COL_3, H_ROW_2, ePnlBlue50)
			screen.addMultilineText(aName(), szSpecial, X_COL_1 + 4, Y_TOP_ROW_2 + 8, W_COL_3 - 8, H_ROW_2 - 16, eWidGen, 0, 0, 1<<0)

		# History
		szTxt = ""
		szTemp = CvTheReligionInfo.getStrategy()
		if szTemp:
			szTxt += szfont2b + TRNSLTR.getText("TXT_KEY_CIVILOPEDIA_STRATEGY", ()) + szfont2 + szTemp + "\n\n"
		szTemp = CvTheReligionInfo.getCivilopedia()
		if szTemp:
			szTxt += szfont2b + TRNSLTR.getText("TXT_KEY_CIVILOPEDIA_BACKGROUND", ()) + szfont2 + szTemp
		if szTxt:
			screen.addPanel(aName(), "", "", True, False, X_COL_3, Y_TOP_ROW_2, W_COL_3, H_ROW_2, ePnlBlue50)
			screen.addMultilineText(aName(), szTxt, X_COL_3 + 4, Y_TOP_ROW_2 + 8, W_COL_3 - 8, H_ROW_2 - 16, eWidGen, 0, 0, 1<<0)
