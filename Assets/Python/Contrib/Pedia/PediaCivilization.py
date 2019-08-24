# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaCivilization:

	def __init__(self, parent, H_BOT_ROW):
		self.main = parent

		H_PEDIA_PAGE = parent.H_PEDIA_PAGE

		self.Y_TOP_ROW = Y_TOP_ROW = parent.Y_PEDIA_PAGE
		self.Y_BOT_ROW = Y_TOP_ROW + H_PEDIA_PAGE - H_BOT_ROW

		self.H_TOP_ROW = H_TOP_ROW = (H_PEDIA_PAGE - H_BOT_ROW * 3) / 4
		self.H_BOT_ROW = H_BOT_ROW

		W_PEDIA_PAGE = parent.W_PEDIA_PAGE

		W_BASE = 64 + H_TOP_ROW + W_PEDIA_PAGE / 8
		W_HALF_PP = W_PEDIA_PAGE / 2

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE
		self.X_COL_2 = X_COL_1 + W_BASE + 4
		self.X_COL_3 = X_COL_1 + W_HALF_PP + 4

		self.W_COL_1 = W_BASE - 4
		self.W_COL_2 = W_PEDIA_PAGE - W_BASE - 4
		self.W_COL_3 = W_HALF_PP - 4

		self.S_ICON = S_ICON = H_TOP_ROW - H_TOP_ROW % 8

		a = H_TOP_ROW / 12
		self.X_STATS = X_COL_1 + S_ICON - 8
		self.Y_STATS = Y_TOP_ROW + a
		self.H_STATS = H_TOP_ROW - a * 2 + 4
		self.W_STATS = W_BASE - S_ICON


	def interfaceScreen(self, iCivilization):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		ArtFileMgr = CyArtFileMgr()
		screen = self.main.screen()
		CvCivilizationInfo = GC.getCivilizationInfo(iCivilization)

		iWidGen			= WidgetTypes.WIDGET_GENERAL
		iPanelBlue50	= PanelStyles.PANEL_STYLE_BLUE50
		iFontTitle		= FontTypes.TITLE_FONT

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
		H_MID_ROW = Y_BOT_ROW_1 - Y_TOP_ROW_2

		# Main Panel
		screen.setText(self.main.getNextWidgetName(), "", szfontEdge + CvCivilizationInfo.getDescription(), 1<<0, X_COL_1, 0, 0, iFontTitle, iWidGen, 0, 0)
		screen.addPanel(self.main.getNextWidgetName(), "", "", False, False, X_COL_1 + 4, Y_TOP_ROW_1 + 3, W_COL_1, H_TOP_ROW, PanelStyles.PANEL_STYLE_MAIN)
		screen.addDDSGFC(self.main.getNextWidgetName(), CvCivilizationInfo.getButton(), X_COL_1 - 2, Y_TOP_ROW_1 + 7, S_ICON, S_ICON, iWidGen, -1, -1)

		panelName = self.main.getNextWidgetName()
		screen.addListBoxGFC(panelName, "", self.X_STATS, self.Y_STATS, self.W_STATS, self.H_STATS, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)
		screen.appendListBoxStringNoUpdate(panelName, szfont3b + CvCivilizationInfo.getShortDescription(0), iWidGen, 0, 0, 1<<0)
		screen.appendListBoxStringNoUpdate(panelName, szfont3b + CvCivilizationInfo.getAdjective(0), iWidGen, 0, 0, 1<<0)
		screen.updateListBox(panelName)

		# Leaders
		panelName = self.main.getNextWidgetName()
		screen.addPanel(panelName, TRNSLTR.getText("TXT_KEY_CONCEPT_LEADERS", ()), "", False, True, X_COL_2, Y_TOP_ROW_1, W_COL_2 -8, H_TOP_ROW, iPanelBlue50)
		iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_LEADER
		# Adjust image size to panel
		size = H_TOP_ROW / 6
		size *= 4
		for iLeader in xrange(GC.getNumLeaderHeadInfos()):
			if not CvCivilizationInfo.isLeaders(iLeader): continue
			screen.attachTextGFC(panelName, "", '<img=%s size=%d></img>' %(GC.getLeaderHeadInfo(iLeader).getButton(), size), iFontTitle, iWidget, iLeader, iCivilization)

		# City List
		screen.addPanel(self.main.getNextWidgetName(), "Cities", "", True, True, X_COL_1, Y_TOP_ROW_2, W_COL_1, H_MID_ROW, iPanelBlue50)
		panelName = self.main.getNextWidgetName()
		screen.addListBoxGFC(panelName, "", X_COL_1 + 6, Y_TOP_ROW_2 + 32, W_COL_1 - 10, H_MID_ROW - 40, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)
		for i in xrange(CvCivilizationInfo.getNumCityNames()):
			szText = CvCivilizationInfo.getCityNames(i)
			if szText[:4] == "TXT_":
				szText = TRNSLTR.getText(szText, ())
			screen.appendListBoxStringNoUpdate(panelName, szfont3 + szText, iWidGen, 0, 0, 1<<0)
		screen.updateListBox(panelName)

		# History
		screen.addPanel(self.main.getNextWidgetName(), TRNSLTR.getText("TXT_KEY_PEDIA_HISTORY", ()), "", True, True, X_COL_2, Y_TOP_ROW_2, W_COL_2, H_MID_ROW, iPanelBlue50)
		szText = szfont3 + CvCivilizationInfo.getCivilopedia()
		screen.addMultilineText(self.main.getNextWidgetName(), szText, X_COL_2 + 4, Y_TOP_ROW_2 + 32, W_COL_2 - 8, H_MID_ROW - 40, iWidGen, 0, 0, 1<<0)

		# Unique Buildings
		panelName = self.main.getNextWidgetName()
		screen.addPanel(panelName, TRNSLTR.getText("TXT_KEY_UNIQUE_BUILDINGS", ()), "", False, True, X_COL_1, Y_BOT_ROW_1, W_COL_3, H_BOT_ROW, iPanelBlue50)
		iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING
		for iBuilding in xrange(GC.getNumBuildingClassInfos()):
			iUniqueBuilding = CvCivilizationInfo.getCivilizationBuildings(iBuilding)
			iDefaultBuilding = GC.getBuildingClassInfo(iBuilding).getDefaultBuildingIndex()
			if (iDefaultBuilding > -1 and iUniqueBuilding > -1 and iDefaultBuilding != iUniqueBuilding):
				screen.attachImageButton(panelName, "", GC.getBuildingInfo(iUniqueBuilding).getButton(), enumGBS, iWidget, iUniqueBuilding, 1, False)

		# Unique Units
		panelName = self.main.getNextWidgetName()
		screen.addPanel(panelName, TRNSLTR.getText("TXT_KEY_FREE_UNITS", ()), "", False, True, X_COL_3, Y_BOT_ROW_1, W_COL_3, H_BOT_ROW, iPanelBlue50)
		iWidget = WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT
		for iUnit in xrange(GC.getNumUnitClassInfos()):
			iUniqueUnit = CvCivilizationInfo.getCivilizationUnits(iUnit)
			iDefaultUnit = GC.getUnitClassInfo(iUnit).getDefaultUnitIndex()
			if iDefaultUnit > -1 and iUniqueUnit > -1 and iDefaultUnit != iUniqueUnit:
				screen.attachImageButton(panelName, "", GC.getUnitInfo(iUniqueUnit).getButton(), enumGBS, iWidget, iUniqueUnit, 1, False)
