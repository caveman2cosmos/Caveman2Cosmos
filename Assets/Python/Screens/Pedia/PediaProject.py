# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaProject:

	def __init__(self, parent, H_BOT_ROW):
		self.main = parent

		self.H_PEDIA_PAGE = H_PEDIA_PAGE = parent.H_PEDIA_PAGE

		self.Y_TOP_ROW = Y_TOP_ROW = parent.Y_PEDIA_PAGE

		self.H_TOP_ROW = H_TOP_ROW = (H_PEDIA_PAGE - H_BOT_ROW * 3) / 4

		W_PEDIA_PAGE = parent.W_PEDIA_PAGE

		W_BASE = 64 + H_TOP_ROW + W_PEDIA_PAGE / 8
		W_HALF_PP = W_PEDIA_PAGE / 2

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE
		self.X_COL_2 = X_COL_1 + W_HALF_PP + 4

		self.W_COL_1 = W_HALF_PP - 4

		self.S_ICON = S_ICON = H_TOP_ROW - 10

		a = H_TOP_ROW / 12
		self.X_STATS = X_COL_1 + S_ICON
		self.Y_STATS = Y_TOP_ROW + a
		self.H_STATS = H_TOP_ROW - a * 2 + 4
		self.W_STATS = (W_HALF_PP - S_ICON)/2


	def interfaceScreen(self, iTheProject):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		screen = self.main.screen()
		CyPlayer = self.main.CyPlayer
		aName = self.main.getNextWidgetName

		CvTheProjectInfo = GC.getProjectInfo(iTheProject)

		iWidGen			= WidgetTypes.WIDGET_GENERAL
		iPanelBlue50	= PanelStyles.PANEL_STYLE_BLUE50

		szfontEdge, szfont4b, szfont4, szfont3b, szfont3, szfont2b, szfont2 = self.main.aFontList

		H_PEDIA_PAGE = self.H_PEDIA_PAGE
		H_TOP_ROW = self.H_TOP_ROW
		S_ICON = self.S_ICON
		X_COL_1 = self.X_COL_1
		X_COL_2 = self.X_COL_2
		Y_ROW_1 = self.Y_TOP_ROW
		Y_ROW_2 = Y_ROW_1 + H_TOP_ROW
		W_COL_1 = self.W_COL_1
		H_ROW_2 = H_PEDIA_PAGE - H_TOP_ROW
		X_STATS = self.X_STATS
		Y_STATS = self.Y_STATS
		W_STATS = self.W_STATS
		H_STATS = self.H_STATS

		szName = CvTheProjectInfo.getDescription()
		iMaxGlobal = CvTheProjectInfo.getMaxGlobalInstances()
		iMaxTeam = CvTheProjectInfo.getMaxTeamInstances()
		if iMaxGlobal > 0:
			szName += "<color=192,192,128,255> | " + TRNSLTR.getText("TXT_KEY_PEDIA_WORLD_PROJECT",()) + " - Max. " + str(iMaxGlobal)
		elif iMaxTeam > 0:
			szName += "<color=192,192,128,255> | " + TRNSLTR.getText("TXT_KEY_PEDIA_TEAM_PROJECT",()) + " - Max. " + str(iMaxTeam)
		screen.setText(aName(), "", szfontEdge + szName, 1<<0, X_COL_1, 0, 0, FontTypes.TITLE_FONT, iWidGen, 0, 0)
		# Main Panel
		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_ROW_1 + 2, W_COL_1 + 8, H_TOP_ROW + 2, PanelStyles.PANEL_STYLE_MAIN)
		Img = "Preview|Movie|PROJECT|ToolTip" + str(iTheProject)
		self.main.aWidgetBucket.append(Img)
		screen.setImageButtonAt(Img, Pnl, CvTheProjectInfo.getButton(), 4, 6, S_ICON, S_ICON, iWidGen, 1, 1)
		# Stats
		Pnl = aName()
		screen.addListBoxGFC(Pnl, "", X_STATS, Y_STATS, W_STATS, H_STATS, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(Pnl, False)
		iCost = CvTheProjectInfo.getProductionCost()
		if iCost > 0:
			if CyPlayer:
				iCost = CyPlayer.getProjectProductionNeeded(iTheProject)
			else:
				iCost = iCost * GC.getDefineINT("PROJECT_PRODUCTION_PERCENT")/100
			szCost = TRNSLTR.getText("TXT_KEY_PEDIA_COST", (iCost,))
			screen.appendListBoxString(Pnl, szfont3b + szCost + u'%c' %GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar(), iWidGen, 0, 0, 1<<0)
		# Requires
		iTech = CvTheProjectInfo.getTechPrereq()
		if iTech != -1:
			Pnl = aName()
			szRequires = TRNSLTR.getText("TXT_KEY_PEDIA_REQUIRES", ()) + ":"
			screen.addPanel(Pnl, szRequires, "", False, True, X_COL_1 + W_COL_1 - 108, Y_STATS + 16, 100, H_STATS - 16, PanelStyles.PANEL_STYLE_EMPTY)
			screen.attachLabel(Pnl, "", "<font=4b>| ")
			Img = "ToolTip|JumpTo|TECH" + str(iTech)
			screen.attachImageButton(Pnl, Img, GC.getTechInfo(iTech).getButton(), self.main.enumGBS, iWidGen, 1, 1, False)
			screen.attachLabel(Pnl, "", "<font=4b> |")
		# Strategy
		screen.addPanel(aName(), "", "", False, False, X_COL_2, Y_ROW_1, W_COL_1, H_TOP_ROW, iPanelBlue50)
		szText = szfont2b + "<color=192,192,128,255>" + TRNSLTR.getText("TXT_KEY_PEDIA_STRATEGY", ()) + ":    </color>"
		szText += szfont2 + CvTheProjectInfo.getStrategy()
		screen.addMultilineText(aName(), szText, X_COL_2 + 4, Y_ROW_1 + 8, W_COL_1 - 8, H_TOP_ROW - 16, iWidGen, 0, 0, 1<<0)
		# Special
		screen.addPanel(aName(), "", "", True, False, X_COL_1, Y_ROW_2, W_COL_1, H_ROW_2, iPanelBlue50)
		szText = szfont3 + CyGameTextMgr().getProjectHelp(iTheProject, True, None)[1:]
		screen.addMultilineText(aName(), szText, X_COL_1 + 4, Y_ROW_2 + 12, W_COL_1 - 8, H_ROW_2 - 20, iWidGen, 0, 0, 1<<0)
		# History
		screen.addPanel(aName(), "", "", True, False, X_COL_2, Y_ROW_2, W_COL_1, H_ROW_2, iPanelBlue50)
		szText = szfont2b + "<color=192,192,128,255>" + TRNSLTR.getText("TXT_KEY_PEDIA_HISTORY", ()) + ":    </color>"
		szText += szfont2 + CvTheProjectInfo.getCivilopedia()
		screen.addMultilineText(aName(), szText, X_COL_2 + 4, Y_ROW_2 + 8, W_COL_1 - 8, H_ROW_2 - 16, iWidGen, 0, 0, 1<<0)
