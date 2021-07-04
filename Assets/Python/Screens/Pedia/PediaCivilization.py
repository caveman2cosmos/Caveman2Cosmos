# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaCivilization:

	def __init__(self, parent, H_BOT_ROW):
		self.main = parent

		H_PEDIA_PAGE = parent.H_PEDIA_PAGE

		self.Y_TOP_ROW = parent.Y_PEDIA_PAGE

		self.H_TOP_ROW = H_TOP_ROW = (H_PEDIA_PAGE - H_BOT_ROW * 3) / 4

		self.H_MID_ROW = H_PEDIA_PAGE - H_TOP_ROW

		W_BASE = 64 + H_TOP_ROW + parent.W_PEDIA_PAGE / 8

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE
		self.X_COL_2 = X_COL_1 + W_BASE + 4

		self.W_COL_1 = W_BASE - 4
		self.W_COL_2 = parent.W_PEDIA_PAGE - W_BASE - 4

		self.S_ICON = S_ICON = H_TOP_ROW - 10

		a = H_TOP_ROW / 12
		self.X_STATS = X_COL_1 + S_ICON
		self.Y_STATS = self.Y_TOP_ROW + a
		self.H_STATS = H_TOP_ROW - a * 2 + 4
		self.W_STATS = W_BASE - S_ICON


	def interfaceScreen(self, iCivilization):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		#ArtFileMgr = CyArtFileMgr()
		screen = self.main.screen()
		aName = self.main.getNextWidgetName

		CvCivilizationInfo = GC.getCivilizationInfo(iCivilization)

		eWidGen			= WidgetTypes.WIDGET_GENERAL
		ePanelBlue50	= PanelStyles.PANEL_STYLE_BLUE50
		eFontTitle		= FontTypes.TITLE_FONT

		szfontEdge, szfont4b, szfont4, szfont3b, szfont3, szfont2b, szfont2 = self.main.aFontList

		H_TOP_ROW = self.H_TOP_ROW
		S_ICON = self.S_ICON
		X_COL_1 = self.X_COL_1
		X_COL_2 = self.X_COL_2
		Y_TOP_ROW_1 = self.Y_TOP_ROW
		Y_TOP_ROW_2 = Y_TOP_ROW_1 + H_TOP_ROW
		W_COL_1 = self.W_COL_1
		W_COL_2 = self.W_COL_2
		H_MID_ROW = self.H_MID_ROW

		screen.setText(aName(), "", szfontEdge + CvCivilizationInfo.getDescription(), 1<<0, X_COL_1, 0, 0, eFontTitle, eWidGen, 0, 0)
		# Main Panel
		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_TOP_ROW_1 + 2, W_COL_1 + 8, H_TOP_ROW + 2, PanelStyles.PANEL_STYLE_MAIN)
		screen.setImageButtonAt("", Pnl, CvCivilizationInfo.getButton(), 4, 6, S_ICON, S_ICON, eWidGen, 1, 1)

		Pnl = aName()
		screen.addListBoxGFC(Pnl, "", self.X_STATS, self.Y_STATS, self.W_STATS, self.H_STATS, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(Pnl, False)
		screen.appendListBoxStringNoUpdate(Pnl, szfont3b + CvCivilizationInfo.getShortDescription(0), eWidGen, 0, 0, 1<<0)
		screen.appendListBoxStringNoUpdate(Pnl, szfont3b + CvCivilizationInfo.getAdjective(0), eWidGen, 0, 0, 1<<0)
		screen.updateListBox(Pnl)

		# Leaders
		Pnl = aName()
		screen.addPanel(Pnl, TRNSLTR.getText("TXT_KEY_CONCEPT_LEADERS", ()), "", False, True, X_COL_2, Y_TOP_ROW_1, W_COL_2, H_TOP_ROW, ePanelBlue50)
		eWidJuToLeader = WidgetTypes.WIDGET_PEDIA_JUMP_TO_LEADER
		# Adjust image size to panel
		size = H_TOP_ROW - 36
		x = 8
		for iLeader in xrange(GC.getNumLeaderHeadInfos()):
			if not CvCivilizationInfo.isLeaders(iLeader): continue
			screen.setImageButtonAt("", Pnl, GC.getLeaderHeadInfo(iLeader).getButton(), x, 4, size, size, eWidJuToLeader, iLeader, iCivilization)
			x += size + 8

		# City List
		screen.addPanel(aName(), "Cities", "", True, True, X_COL_1, Y_TOP_ROW_2, W_COL_1, H_MID_ROW, ePanelBlue50)
		Pnl = aName()
		screen.addListBoxGFC(Pnl, "", X_COL_1 + 6, Y_TOP_ROW_2 + 32, W_COL_1 - 10, H_MID_ROW - 40, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(Pnl, False)
		for i in xrange(CvCivilizationInfo.getNumCityNames()):
			szText = CvCivilizationInfo.getCityNames(i)
			if szText[:4] == "TXT_":
				szText = TRNSLTR.getText(szText, ())
			screen.appendListBoxStringNoUpdate(Pnl, szfont3 + szText, eWidGen, 0, 0, 1<<0)
		screen.updateListBox(Pnl)

		# History
		screen.addPanel(aName(), TRNSLTR.getText("TXT_KEY_PEDIA_HISTORY", ()), "", True, True, X_COL_2, Y_TOP_ROW_2, W_COL_2, H_MID_ROW, ePanelBlue50)
		szText = szfont3 + CvCivilizationInfo.getCivilopedia()
		screen.addMultilineText(aName(), szText, X_COL_2 + 4, Y_TOP_ROW_2 + 32, W_COL_2 - 8, H_MID_ROW - 40, eWidGen, 0, 0, 1<<0)
