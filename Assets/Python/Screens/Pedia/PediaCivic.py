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
		self.H_MID_ROW = H_PEDIA_PAGE - H_TOP_ROW

		self.W_PEDIA_PAGE = W_PEDIA_PAGE = parent.W_PEDIA_PAGE
		self.W_3RD_PP = W_3RD_PP = W_PEDIA_PAGE / 3 - 4
		self.W_2_3Rd_PP = W_PEDIA_PAGE - W_3RD_PP - 8

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE
		self.X_COL_2 = X_COL_1 + W_3RD_PP + 8

		self.S_ICON1 = S_ICON1 = H_TOP_ROW - H_TOP_ROW % 8

		self.X_STATS = X_COL_1 + S_ICON1 - 8
		self.Y_STATS = Y_TOP_ROW + H_TOP_ROW / 6
		self.W_STATS = W_3RD_PP - S_ICON1 + 8

	def interfaceScreen(self, iTheCivic):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		CvTheCivicInfo = GC.getCivicInfo(iTheCivic)
		screen = self.main.screen()

		iWidGeneral		= WidgetTypes.WIDGET_GENERAL
		iPanelBlue50	= PanelStyles.PANEL_STYLE_BLUE50

		enumGBS = self.main.enumGBS
		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2 = self.main.aFontList

		X_COL_1 = self.X_COL_1
		X_COL_2 = self.X_COL_2
		H_BOT_ROW = self.H_BOT_ROW
		H_TOP_ROW = self.H_TOP_ROW
		Y_BOT_ROW = self.Y_BOT_ROW
		Y_TOP_ROW = self.Y_TOP_ROW
		Y_MID_ROW = Y_TOP_ROW + H_TOP_ROW
		Y_STATS = self.Y_STATS
		W_3RD_PP = self.W_3RD_PP
		W_2_3Rd_PP = self.W_2_3Rd_PP
		S_ICON1 = self.S_ICON1
		H_MID_ROW = self.H_MID_ROW

		# Main Panel
		screen.setText(self.main.getNextWidgetName(), "", uFontEdge + CvTheCivicInfo.getDescription(), 1<<0, X_COL_1, 0, 0, FontTypes.TITLE_FONT, iWidGeneral, 0, 0)
		screen.addPanel( self.main.getNextWidgetName(), "", "", False, False, X_COL_1 + 4, Y_TOP_ROW + 3, W_3RD_PP, H_TOP_ROW, PanelStyles.PANEL_STYLE_MAIN)
		screen.addDDSGFC(self.main.getNextWidgetName(), CvTheCivicInfo.getButton(), X_COL_1 - 2, Y_TOP_ROW + 7, S_ICON1, S_ICON1, iWidGeneral, -1, -1 )
		# Stats
		panelName = self.main.getNextWidgetName()
		screen.addListBoxGFC(panelName, "", self.X_STATS, Y_STATS, self.W_STATS, H_TOP_ROW - 12, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)
		iCivicOptionType = CvTheCivicInfo.getCivicOptionType()
		if iCivicOptionType != -1:
			screen.appendListBoxString(panelName, uFont4b + GC.getCivicOptionInfo(iCivicOptionType).getDescription(), iWidGeneral, 0, 0, 1<<0)
		pUpkeepInfo = GC.getUpkeepInfo(CvTheCivicInfo.getUpkeep())
		if pUpkeepInfo:
			screen.appendListBoxString(panelName, uFont3b + pUpkeepInfo.getDescription(), iWidGeneral, 0, 0, 1<<0)
		# Requires
		iTech = CvTheCivicInfo.getTechPrereq()
		if iTech != -1:
			panelName = self.main.getNextWidgetName()
			szRequires = TRNSLTR.getText("TXT_KEY_PEDIA_REQUIRES", ()) + ":"
			screen.addPanel(panelName, szRequires, "", False, True, X_COL_1 + W_3RD_PP - 108, Y_STATS - 8, 100, H_BOT_ROW, PanelStyles.PANEL_STYLE_EMPTY)
			screen.attachLabel(panelName, "", "<font=4b>| ")
			screen.attachImageButton(panelName, "", GC.getTechInfo(iTech).getButton(), enumGBS, WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH, iTech, 1, False)
			screen.attachLabel(panelName, "", "<font=4b> |")
		# Strategy
		szStrategy = TRNSLTR.getText("TXT_KEY_PEDIA_STRATEGY", ())
		screen.addPanel(self.main.getNextWidgetName(), szStrategy, "", True, True, X_COL_2, Y_TOP_ROW, W_2_3Rd_PP, H_TOP_ROW, iPanelBlue50)
		szStrategy = uFont2 + CvTheCivicInfo.getStrategy()
		screen.addMultilineText(self.main.getNextWidgetName(), szStrategy, X_COL_2 + 4, Y_TOP_ROW + 32, W_2_3Rd_PP - 8, H_TOP_ROW - 40, iWidGeneral, -1, -1, 1<<0)
		# Effects
		screen.addPanel(self.main.getNextWidgetName(), TRNSLTR.getText("TXT_KEY_PEDIA_EFFECTS", ()), "", True, False, X_COL_2, Y_MID_ROW, W_2_3Rd_PP, H_MID_ROW, iPanelBlue50)
		szSpecialText = uFont3 + CyGameTextMgr().parseCivicInfo(iTheCivic, True, False, True)[1:]
		screen.addMultilineText(self.main.getNextWidgetName(), szSpecialText, X_COL_2 + 4, Y_MID_ROW + 32, W_2_3Rd_PP - 8, H_MID_ROW - 40, iWidGeneral, -1, -1, 1<<0)
		# History
		szHistory = TRNSLTR.getText("TXT_KEY_PEDIA_HISTORY", ())
		screen.addPanel(self.main.getNextWidgetName(), szHistory, "", True, True, X_COL_1, Y_MID_ROW, W_3RD_PP, H_MID_ROW, iPanelBlue50)
		szText = uFont2 + CvTheCivicInfo.getCivilopedia()
		screen.addMultilineText(self.main.getNextWidgetName(), szText, X_COL_1 + 4, Y_MID_ROW + 32, W_3RD_PP - 8, H_MID_ROW - 40, iWidGeneral, -1, -1, 1<<0)
