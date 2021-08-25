# CvVictoryScreen
from CvPythonExtensions import *
import AttitudeUtil

# globals
GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()


class CvVictoryScreen:
	"Keeps track of victory conditions"

	def __init__(self, screenId):
		self.screenId = screenId

		self.VoteType = 1  # 1 = Pope or GenSec, 2 = Diplomatic Victory
		self.VoteBody = 1  # 1 = AP, 2 = UN

		self.nWidgetCount = 0

		self.iTab = 0

		self.ApolloTeamsChecked = set()
		self.ApolloTeamCheckResult = {}

	def getScreen(self):
		return CyGInterfaceScreen("VictoryScreen", self.screenId)

	def interfaceScreen(self):

		# Create a new screen
		screen = self.getScreen()
		if screen.isActive():
			return
		# Get screen resolution.
		self.xRes = xRes = screen.getXResolution()
		self.yRes = yRes = screen.getYResolution()
		# Calibrate variables.
		if yRes > 1000:
			H_BOT_ROW = 110
			self.H_EDGE_PANEL = H_EDGE_PANEL = 38
			szfontEdge = "<font=4b>"
		elif yRes > 800:
			H_BOT_ROW = 92
			self.H_EDGE_PANEL = H_EDGE_PANEL = 32
			szfontEdge = "<font=3b>"
		else:
			H_BOT_ROW = 78
			self.H_EDGE_PANEL = H_EDGE_PANEL = 29
			szfontEdge = "<font=2b>"

		if xRes > 1700:
			self.aFontList = aFontList = [szfontEdge, "<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1>"]
		elif xRes > 1400:
			self.aFontList = aFontList = [szfontEdge, "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0>"]
		else:
			self.aFontList = aFontList = [szfontEdge, "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0b>", "<font=0>", "<font=0>"]

		self.Y_TOP_PAGE = Y_TOP_PAGE = H_EDGE_PANEL - 6
		self.H_PAGE = H_PAGE = yRes - Y_TOP_PAGE - H_EDGE_PANEL + 8
		self.Y_BOT_TEXT = Y_BOT_TEXT = yRes - H_EDGE_PANEL + 8
		# Declare misc variables
		self.iActivePlayer = GAME.getActivePlayer()
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		# Build Victory screen.
		screen.setRenderInterfaceOnly(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.addDDSGFC("VS_Background", CyArtFileMgr().getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, xRes, yRes, eWidGen, 1, 2)
		screen.addPanel("VS_TopPanel", "", "", True, False, 0, 0, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("VS_BotPanel", "", "", True, False, 0, yRes - H_EDGE_PANEL, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.setLabel("VS_Header", "", szfontEdge + TRNSLTR.getText("TXT_WORD_GAME",()), 1<<2, xRes / 2, 2, 0, eFontTitle, eWidGen, 1, 2)
		screen.setText("VS_Btn_Exit", "", szfontEdge + TRNSLTR.getText("TXT_WORD_EXIT",()), 1<<1, xRes - 16, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		self.COLOR_GREEN = GC.getCOLOR_GREEN()
		self.COLOR_YELLOW = iCol = GC.getCOLOR_YELLOW()
		szTxt = szfontEdge + TRNSLTR.getText("TXT_KEY_MAIN_MENU_VICTORIES", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iCol)
		dX = xRes / 4
		x = dX / 2
		screen.setText("VS_Tab0", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 1, 2)
		screen.setText("VS_Col_Tab0", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 1, 2)
		screen.hide("VS_Col_Tab0")

		szTxt = szfontEdge + TRNSLTR.getText("TXT_KEY_MAIN_MENU_SETTINGS", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iCol)
		x += dX
		screen.setText("VS_Tab1", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 1, 2)
		screen.setText("VS_Col_Tab1", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 1, 2)
		screen.hide("VS_Col_Tab1")

		szTxt = szfontEdge + TRNSLTR.getText("TXT_KEY_VOTING_TITLE", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iCol)
		x += dX
		screen.setText("VS_Tab2", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 1, 2)
		screen.setText("VS_Col_Tab2", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 1, 2)
		screen.hide("VS_Col_Tab2")

		szTxt = szfontEdge + TRNSLTR.getText("TXT_KEY_MEMBERS_TITLE", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iCol)
		x += dX
		screen.setText("VS_Tab3", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 1, 2)
		screen.setText("VS_Col_Tab3", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 1, 2)
		screen.hide("VS_Col_Tab3")

		if self.iTab == 0:
			screen.hide("VS_Tab0")
			screen.show("VS_Col_Tab0")
			self.showVictoryConditionScreen(screen)
		elif self.iTab == 1:
			screen.hide("VS_Tab1")
			screen.show("VS_Col_Tab1")
			self.showGameSettingsScreen(screen)
		elif self.iTab == 2:
			screen.hide("VS_Tab2")
			screen.show("VS_Col_Tab2")
			self.showVotingScreen(screen)
		elif self.iTab == 3:
			screen.hide("VS_Tab3")
			screen.show("VS_Col_Tab3")
			self.showMembersScreen(screen)


	def showVictoryConditionScreen(self, screen):
		ufontEdge, ufont4b, ufont4, ufont3b, ufont3, ufont2b, ufont2, ufont1 = self.aFontList
		iActivePlayer = self.iActivePlayer
		CyPlayer = GC.getPlayer(iActivePlayer)
		iTeamAct = CyPlayer.getTeam()
		CyTeam = GC.getTeam(iTeamAct)

		# checking if apollo has been built - clear arrays / lists / whatever they are called
		self.ApolloTeamsChecked = set()
		self.ApolloTeamCheckResult = {}

		# Get Best Team
		iBestTeam = 0
		iMax = 0
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			if iPlayerX == iActivePlayer:
				continue
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive():
				iTeamX = CyPlayerX.getTeam()
				iTemp = GC.getTeam(iTeamX).getTotalVictoryScore()
				if iTemp > iMax:
					iMax = iTemp
					iBestTeam = iTeamX

		if iBestTeam == iTeamAct:
			CyTeamBest = CyTeam
			bMetHuman = False
		else:
			CyTeamBest = GC.getTeam(iBestTeam)
			bMetHuman = CyTeamBest.isHasMet(iTeamAct)

		# Population
		iTotalPop = GAME.getTotalPopulation()
		ourPop = CyTeam.getTotalPopulation()
		if iTotalPop > 0:
			fPopPercent = ourPop * 100.0 / iTotalPop
		else:
			fPopPercent = 0.0
			iTotalPop = 1

		bDebugModeDLL = GAME.isDebugMode()
		iBestPopTeam = -1
		iBestPop = 0
		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
			if iTeamX == iTeamAct:
				continue
			CyTeamX = GC.getTeam(iTeamX)
			if CyTeamX.isAlive() and not CyTeamX.isMinorCiv():
				if CyTeam.isHasMet(iTeamX) or bDebugModeDLL:
					iPop = CyTeamX.getTotalPopulation()
					if iPop > iBestPop:
						iBestPop = iPop
						iBestPopTeam = iTeamX

		# Score
		iOurScore = GAME.getTeamScore(iTeamAct)

		iBestScoreTeam = -1
		iBestScore = 0
		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
			if iTeamX == iTeamAct:
				continue
			CyTeamX = GC.getTeam(iTeamX)
			if CyTeamX.isAlive() and not CyTeamX.isMinorCiv():
				if CyTeam.isHasMet(iTeamX) or bDebugModeDLL:
					iScore = GAME.getTeamScore(iTeamX)
					if iScore > iBestScore:
						iBestScore = iScore
						iBestScoreTeam = iTeamX

		# Land Area
		iTotalLand = GC.getMap().getLandPlots()
		iOurLand = CyTeam.getTotalLand()
		fLandPercent = iOurLand * 100.0 / iTotalLand

		iBestLandTeam = -1
		iBestLand = 0
		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
			CyTeamX = GC.getTeam(iTeamX)
			if iTeamX == iTeamAct:
				continue
			if CyTeamX.isAlive() and not CyTeamX.isMinorCiv():
				if CyTeam.isHasMet(iTeamX) or bDebugModeDLL:
					teamLand = CyTeamX.getTotalLand()
					if teamLand > iBestLand:
						iBestLand = teamLand
						iBestLandTeam = iTeamX

		# Religion
		iOurReligion = -1
		ourReligionPercent = 0
		for iLoopReligion in xrange(GC.getNumReligionInfos()):
			if CyTeam.hasHolyCity(iLoopReligion):
				religionPercent = GAME.calculateReligionPercent(iLoopReligion)
				if religionPercent > ourReligionPercent:
					ourReligionPercent = religionPercent
					iOurReligion = iLoopReligion

		iBestReligion = -1
		bestReligionPercent = 0
		for iLoopReligion in xrange(GC.getNumReligionInfos()):
			if iLoopReligion != iOurReligion:
				religionPercent = GAME.calculateReligionPercent(iLoopReligion)
				if religionPercent > bestReligionPercent:
					bestReligionPercent = religionPercent
					iBestReligion = iLoopReligion

		# Total Culture
		iBestCultureTeam = -1
		iBestCulture = 0
		iWorldCulture = 0

		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
			CyTeamX = GC.getTeam(iTeamX)
			iWorldCulture += CyTeamX.countTotalCulture()
			if iTeamX == iTeamAct:
				continue
			if CyTeamX.isAlive() and not CyTeamX.isMinorCiv():
				if CyTeam.isHasMet(iTeamX) or bDebugModeDLL:
					teamCulture = CyTeamX.countTotalCulture()
					if teamCulture > iBestCulture:
						iBestCulture = teamCulture
						iBestCultureTeam = iTeamX
		if iWorldCulture < 1:
			iWorldCulture = 1
		iOurCulture = CyTeam.countTotalCulture()
		fCulturePercent = iOurCulture * 100 / iWorldCulture

		# Vote
		aiVoteBuilding = []
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			for j in xrange(GC.getNumVoteSourceInfos()):
				if CvBuildingInfo.getVoteSourceType() == j:
					iUNTeam = -1
					bUnknown = True

					for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
						CyPlayerX = GC.getPlayer(iPlayerX)
						if CyPlayerX.isAlive() and not CyPlayerX.isMinorCiv() and CyPlayerX.hasBuilding(i):
							iUNTeam = CyPlayerX.getTeam()
							if iUNTeam == iTeamAct or bDebugModeDLL or CyTeam.isHasMet(iUNTeam):
								bUnknown = False
							break

					aiVoteBuilding.append((i, iUNTeam, bUnknown))

		# Power History
		iOurPower = 0
		iWorldPower = 0
		bestPower = 0

		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive():
				iTeamX = CyPlayerX.getTeam()
				for i in xrange(GAME.getGameTurn()):
					iWorldPower += CyPlayerX.getPowerHistory(i)

					if iTeamAct == iTeamX:
						iOurPower += CyPlayerX.getPowerHistory(i)
					elif iBestTeam == iTeamX:
						bestPower += CyPlayerX.getPowerHistory(i)
		if iWorldPower < 1:
			iWorldPower = 1

		self.deleteAllWidgets()
		eWidGen = WidgetTypes.WIDGET_GENERAL
		xRes = self.xRes
		screen.addPanel(self.getNextWidgetName(), "", "", False, False, 0, self.Y_TOP_PAGE - 6, xRes, self.H_PAGE + 6, PanelStyles.PANEL_STYLE_BLUE50)

		# civ picker dropdown
		import DebugUtils
		if DebugUtils.isAnyDebugMode():
			screen.addDropDownBoxGFC("VictoryScreenDropdownWidget", 22, 0, 300, eWidGen, 1, 2, FontTypes.GAME_FONT)
			for j in xrange(GC.getMAX_PLAYERS()):
				if GC.getPlayer(j).isAlive():
					screen.addPullDownString("VictoryScreenDropdownWidget", GC.getPlayer(j).getName(), j, j, False)

		iTurn = GAME.getElapsedGameTurns()
		iMaxTurns = GAME.getMaxTurns()
		dx = xRes - 24
		a18thX = dx / 18
		szTable = self.getNextWidgetName()
		# Mastery
		for iLoopVC in xrange(GC.getNumVictoryInfos()):
			CvVictoryInfo = GC.getVictoryInfo(iLoopVC)
			if CvVictoryInfo.isTotalVictory():
				if GAME.isVictoryValid(iLoopVC):

					if iBestTeam != -1 and bMetHuman:
						szUnknown = ""
					else:
						szUnknown = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_UNKNOWN", ())
					# Start filling in the table below
					screen.addTableControlGFC(szTable, 4, 12, self.Y_TOP_PAGE + 12, dx, self.H_PAGE - 24, False, True, 32,32, TableStyles.TABLE_STYLE_STANDARD)
					iTemp1 = 6 * a18thX
					iTemp2 = 3 * a18thX
					iTemp3 = (dx - 2 * iTemp1 - 2 * iTemp2) / 4
					iTemp1 += iTemp3
					iTemp2 += iTemp3
					screen.setTableColumnHeader(szTable, 0, "", iTemp1)
					screen.setTableColumnHeader(szTable, 1, "", iTemp2)
					screen.setTableColumnHeader(szTable, 2, "", iTemp2)
					screen.setTableColumnHeader(szTable, 3, "", iTemp1)
					del xRes, dx, a18thX, iTemp1, iTemp2, iTemp3

					# Headings
					szVictoryType = ufont3b + CvVictoryInfo.getDescription()
					if iMaxTurns > iTurn:
						szVictoryType += ufont2 + "\t\t(" + TRNSLTR.getText("TXT_KEY_MISC_TURNS_LEFT", (iMaxTurns - iTurn,)) + ")"

					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, szVictoryType, "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 1, iRow, ufont3b + CyPlayer.getName(), "", eWidGen, 1, 2, 1<<2)
					if szUnknown:
						screen.setTableText(szTable, 3, iRow, ufont3b + szUnknown, "", eWidGen, 1, 2, 1<<0)
					else:
						screen.setTableText(szTable, 3, iRow, ufont3b + CyTeamBest.getName(), "", eWidGen, 1, 2, 1<<0)

					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, ufont2b + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_CONDITION", ()), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 1, iRow, ufont2b + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_PERCENTAGE", ()), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 2, iRow, ufont2b + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_POINTS", ()), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 3, iRow, ufont2b + "(" + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_RIVAL_CIV", ()) + ")", "", eWidGen, 1, 2, 1<<0)

					screen.appendTableRow(szTable)

					# Population
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_POPULATION", ()), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 1, iRow, ufont2 + (u"%i%%" % fPopPercent), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 2, iRow, ufont2 + (u"%i" % fPopPercent), "", eWidGen, 1, 2, 1<<0)
					if szUnknown:
						screen.setTableText(szTable, 3, iRow, ufont2 + szUnknown, "", eWidGen, 1, 2, 1<<0)
					else:
						screen.setTableText(szTable, 3, iRow, ufont2 + (u"%i%%" % (CyTeamBest.getTotalPopulation() * 100 / iTotalPop)), "", eWidGen, 1, 21, 1<<0)

					# Land
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_LAND", ()), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 1, iRow, ufont2 + (u"%i%%" % fLandPercent), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 2, iRow, ufont2 + (u"%i" % fLandPercent), "", eWidGen, 1, 2, 1<<0)
					if szUnknown:
						screen.setTableText(szTable, 3, iRow, ufont2 + szUnknown, "", eWidGen, 1, 2, 1<<0)
					else:
						screen.setTableText(szTable, 3, iRow, ufont2 + (u"%i%%" % (CyTeamBest.getTotalLand() * 100 / iTotalLand)), "", eWidGen, 1, 2, 1<<0)

					# Culture
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_WORD_CULTURE", ()), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 1, iRow, ufont2 + u"%i%%" %(fCulturePercent), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 2, iRow, ufont2 + u"%i" %(fCulturePercent), "", eWidGen, 1, 2, 1<<0)
					if szUnknown:
						screen.setTableText(szTable, 3, iRow, ufont2 + szUnknown, "", eWidGen, 1, 2, 1<<0)
					else:
						screen.setTableText(szTable, 3, iRow, ufont2 + u"%i%%" %(CyTeamBest.countTotalCulture() * 100 / iWorldCulture), "", eWidGen, 1, 2, 1<<0)

					# Power
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_POWER", ()), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 1, iRow, ufont2 + u"%i" %(iOurPower), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 2, iRow, ufont2 + u"%i" %(iOurPower * 100/iWorldPower), "", eWidGen, 1, 2, 1<<0)
					if szUnknown:
						screen.setTableText(szTable, 3, iRow, ufont2 + szUnknown, "", eWidGen, 1, 2, 1<<0)
					else:
						screen.setTableText(szTable, 3, iRow, ufont2 + u"%i" %(bestPower), "", eWidGen, 1, 2, 1<<0)

					# WonderScores
					iTeamWonderScore = self.getTeamWonderScore(iTeamAct)
					iRivalWonderScore = self.getTeamWonderScore(iBestTeam)
					iTotalWorldWondersBuilt = GC.getPlayer(0).getSevoWondersScore(1)
					iTotalWorldWondersPossible = GC.getPlayer(0).getSevoWondersScore(2)
					if iTotalWorldWondersPossible == 0:
						iTotalWorldWondersPossible = -1

					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_CONCEPT_WONDERS", ()), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 1, iRow, ufont2 + u"%i  (%i of %i built)" %(iTeamWonderScore, iTotalWorldWondersBuilt, iTotalWorldWondersPossible), "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 2, iRow, ufont2 + u"%i" %(iTeamWonderScore * 100 /iTotalWorldWondersPossible), "", eWidGen, 1, 2, 1<<0)
					if szUnknown:
						screen.setTableText(szTable, 3, iRow, ufont2 + szUnknown, "", eWidGen, 1, 2, 1<<0)
					else:
						screen.setTableText(szTable, 3, iRow, ufont2 + u"%i" %(iRivalWonderScore), "", eWidGen, 1, 2, 1<<0)

					# Religion
					iRow = screen.appendTableRow(szTable)
					iRefRow = iRow
					screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_CONCEPT_RELIGION",()), "", eWidGen, 1, 2, 1<<0)

					if iOurReligion != -1:
						for iLoopReligion in xrange(GC.getNumReligionInfos()):
							if CyTeam.hasHolyCity(iLoopReligion):
								religionPercent = GAME.calculateReligionPercent(iLoopReligion)

								iRow = screen.appendTableRow(szTable)
								screen.setTableText(szTable, 0, iRow, ufont2 + GC.getReligionInfo(iLoopReligion).getDescription(), "", eWidGen, 1, 2, 1<<1)
								screen.setTableText(szTable, 1, iRow, ufont2 + (u"%d%%" % religionPercent), "", eWidGen, 1, 2, 1<<0)

								#Only get points for the TOP religion score.
								if iLoopReligion == iOurReligion:
									screen.setTableText(szTable, 2, iRow, ufont2 + (u"%d" % religionPercent), "", eWidGen, 1, 2, 1<<0)

					else:
						iRow = screen.appendTableRow(szTable)
						screen.setTableText(szTable, 1, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_NO_HOLY", ()), "", eWidGen, 1, 2, 1<<0)

					iReligionFound = 0
					if szUnknown:
						screen.setTableText(szTable, 3, iRow, ufont2 + szUnknown, "", eWidGen, 1, 2, 1<<0)
					else:
						for iLoopReligion in xrange(GC.getNumReligionInfos()):
							if (CyTeamBest.hasHolyCity(iLoopReligion)):
								iReligionFound = 1
								religionPercent = GAME.calculateReligionPercent(iLoopReligion)

								iRefRow += 1
								if iRefRow > iRow:
									iRow = screen.appendTableRow(szTable)
								screen.setTableText(szTable, 3, iRefRow, ufont2 + (u"%s: %i%%" %(GC.getReligionInfo(iLoopReligion).getDescription(), religionPercent)), "", eWidGen, 1, 2, 1<<0)
						if not iReligionFound:
							iRefRow += 1
							screen.setTableText(szTable, 3, iRefRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_NO_HOLY", ()), "", eWidGen, 1, 2, 1<<0)


					# Monumental Cultural Cities
					iPlayerMonumentalCities, iWorldMonumentalCities = self.getMonumentalCities(iTeamAct)
					iBestTeamMonumentalCities, iWorldMonumentalCities = self.getMonumentalCities(iBestTeam)

					ourBestCities = self.getListCultureCitiesTeam(iTeamAct)[0:(iPlayerMonumentalCities + 3)]
					theirBestCities = self.getListCultureCitiesTeam(iBestTeam)[0:(iBestTeamMonumentalCities+3)]

					iRow = screen.appendTableRow(szTable)
					iRefRow = iRow
					screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_CITIES", ()), "", eWidGen, 1, 2, 1<<0)

					for i in xrange(len(ourBestCities)):
						iRow = screen.appendTableRow(szTable)
						screen.setTableText(szTable, 0, iRow, ufont2 + ourBestCities[i][1].getName() + ":", "", eWidGen, 1, 2, 1<<1)
						screen.setTableText(szTable, 1, iRow, ufont2 + str(ourBestCities[i][0]), "", eWidGen, 1, 2, 1<<0)
						if iPlayerMonumentalCities >= i + 1:
							screen.setTableText(szTable, 2, iRow, ufont2 + u"%s" %(30), "", eWidGen, 1, 2, 1<<0)

					if szUnknown:
						screen.setTableText(szTable, 3, iRow, ufont2 + szUnknown, "", eWidGen, 1, 2, 1<<0)
					else:
						for i in xrange(len(theirBestCities)):
							iRefRow += 1
							if iRefRow > iRow:
								iRow = screen.appendTableRow(szTable)
							screen.setTableText(szTable, 3, iRefRow, ufont2 + u"%s: %i" %(theirBestCities[i][1].getName(), theirBestCities[i][0]), "", eWidGen, 1, 2, 1<<0)

					screen.appendTableRow(szTable)

					# Starship Launch Data
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_CONCEPT_SPACESHIP", ()), "", eWidGen, 1, 2, 1<<0)

					if iBestTeam != -1:
						iRivalLaunched = self.teamLaunchedShip(iBestTeam)

					bNeedParts = False

					if self.teamLaunchedShip(iTeamAct) == 1:
						screen.setTableText(szTable, 1, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_LAUNCHED", ()), "", eWidGen, 1, 2, 1<<0)
						screen.setTableText(szTable, 2, iRow, ufont2 + str(100), "", eWidGen, 1, 2, 1<<0)
					else:
						screen.setTableText(szTable, 1, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_NOTLAUNCHED", ()), "", eWidGen, 1, 2, 1<<0)
						if CyTeam.getProjectCount(GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM")) > 0:
							bNeedParts = True

					if iBestTeam != -1 and bMetHuman and iRivalLaunched == 1:
						screen.setTableText(szTable, 3, iRow, ufont2 + u"%s" %(TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_LAUNCHED", ())), "", eWidGen, 1, 2, 1<<0)
					elif iBestTeam != -1 and bMetHuman:
						screen.setTableText(szTable, 3, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_NOTLAUNCHED", ()), "", eWidGen, 1, 2, 1<<0)

					# View spaceship button
					iRow = screen.appendTableRow(szTable)
					screen.setButtonGFC("SpaceShipButton" + str(iLoopVC), TRNSLTR.getText("TXT_KEY_GLOBELAYER_STRATEGY_VIEW", ()), "", 0, 0, 15, 10, eWidGen, 1234, -1, ButtonStyles.BUTTON_STYLE_STANDARD )
					screen.attachControlToTableCell("SpaceShipButton" + str(iLoopVC), szTable, iRow, 1)

					if bNeedParts:
						for i in xrange(GC.getNumProjectInfos()):
							for iL in xrange(GC.getNumVictoryInfos()):
								if GC.getProjectInfo(i).getVictoryThreshold(iL) > 0:
									if GC.getProjectInfo(i).isSpaceship():
										iRow = screen.appendTableRow(szTable)
										screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_BUILDING", (GC.getProjectInfo(i).getVictoryThreshold(iL), GC.getProjectInfo(i).getTextKey())), "", eWidGen, 1, 2, 1<<1)

										if self.teamLaunchedShip(iTeamAct) != 1:
											screen.setTableText(szTable, 1, iRow, ufont2 + str(CyTeam.getProjectCount(i)), "", eWidGen, 1, 2, 1<<0)

					screen.appendTableRow(szTable)

					# Score Totals
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, ufont3b + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_TOTALS", ()) + "</font>", "", eWidGen, 1, 2, 1<<0)
					screen.setTableText(szTable, 2, iRow, ufont3b + "%i" % CyTeam.getTotalVictoryScore(), "", eWidGen, 1, 2, 1<<0)


					if iBestTeam != -1 and bMetHuman:
						screen.setTableText(szTable, 3, iRow, ufont3b + "%i" % CyTeamBest.getTotalVictoryScore(), "", eWidGen, 1, 2, 1<<0)

					screen.appendTableRow(szTable)
					screen.appendTableRow(szTable)

					# The rest of the scores
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, ufont3b + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_OTHER_CIV_SCORE",()), "", eWidGen, 1, 2, 1<<0)
					iRow = screen.appendTableRow(szTable)
					for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
						if iTeamX == iTeamAct: continue
						CyTeamX = GC.getTeam(iTeamX)
						if CyTeamX.isAlive() and not CyTeamX.isMinorCiv():
							if iTeamX != iTeamAct and iTeamX != iBestTeam and CyTeam.isHasMet(iTeamX) or bDebugModeDLL:
								iRow = screen.appendTableRow(szTable)
								screen.setTableText(szTable, 0, iRow, ufont2b + CyTeamX.getName() + ":", "", eWidGen, 1, 2, 1<<0)
								screen.setTableText(szTable, 1, iRow, ufont2b + str(CyTeamX.getTotalVictoryScore()), "", eWidGen, 1, 2, 1<<0)

					return

		# Conquest
		nRivals = 0
		nknown = 0
		nVassaled = 0
		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
			if iTeamX == iTeamAct: continue
			CyTeamX = GC.getTeam(iTeamX)
			if CyTeamX.isAlive() and not CyTeamX.isMinorCiv():
				nRivals += 1
				if CyTeamX.isHasMet(iTeamAct):
					nknown += 1
					if CyTeamX.isVassal(iTeamAct):
						nVassaled += 1

		# Non-mastery victory conditions
		iTemp1 = 6 * a18thX
		iTemp2 = 3 * a18thX
		iTemp3 = (dx - 2 * iTemp1 - 2 * iTemp2) / 4
		iTemp1 += iTemp3
		iTemp2 += iTemp3
		screen.addTableControlGFC(szTable, 4, 12, self.Y_TOP_PAGE + 12, dx, self.H_PAGE - 24, False, True, 32,32, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader(szTable, 0, "", iTemp1)
		screen.setTableColumnHeader(szTable, 1, "", iTemp1)
		screen.setTableColumnHeader(szTable, 2, "", iTemp2)
		screen.setTableColumnHeader(szTable, 3, "", iTemp2)
		iRow = screen.appendTableRow(szTable)
		del xRes, dx, a18thX, iTemp1, iTemp2, iTemp3

		szText = CyTeam.getName() + " - " + TRNSLTR.getText("TXT_WORD_TEAM", ()) + " %i" % iTeamAct
		screen.setTableText(szTable, 1, iRow, ufont3b + szText, "", eWidGen, 1, 2, 1<<2)
		szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_RIVAL_CIV", ())
		screen.setTableText(szTable, 2, iRow, ufont3b + szText, "", eWidGen, 1, 2, 1<<2)

		szUnknown = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_UNKNOWN", ())
		szTeamName = CyTeam.getName()
		for iLoopVC in xrange(GC.getNumVictoryInfos()):
			CvVictoryInfo = GC.getVictoryInfo(iLoopVC)
			if CvVictoryInfo.isTargetScore(): continue
			if GAME.isVictoryValid(iLoopVC):
				if iRow:
					screen.appendTableRow(szTable)
				screen.appendTableRow(szTable)

				iRow = screen.appendTableRow(szTable)

				szText = ufont3b + CvVictoryInfo.getDescription()

				iCategoryRow = iRow
				screen.setTableText(szTable, 0, iRow, szText, "", eWidGen, 1, 2, 1<<1)
				bSpaceshipFound = False

				if CvVictoryInfo.isEndScore():
					if iMaxTurns > iTurn:
						szText = ufont2 + TRNSLTR.getText("TXT_KEY_MISC_TURNS_LEFT", (iMaxTurns - iTurn,))
						screen.setTableText(szTable, 1, iRow, szText, "", eWidGen, 1, 2, 1<<2)

					iRow = screen.appendTableRow(szTable)
					szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_HIGHEST_SCORE", (CyGameTextMgr().getTimeStr(GAME.getStartTurn() + iMaxTurns, False), ))
					screen.setTableText(szTable, 0, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<1)
					screen.setTableText(szTable, 1, iRow, ufont2 + (u"%d" % iOurScore), "", eWidGen, 1, 2, 1<<2)

					if iBestScoreTeam != -1:
						screen.setTableText(szTable, 2, iRow, ufont2 + GC.getTeam(iBestScoreTeam).getName() + ":", "", eWidGen, 1, 2, 1<<0)
						screen.setTableText(szTable, 3, iRow, ufont2 + (u"%d" % iBestScore), "", eWidGen, 1, 2, 1<<0)

				if CvVictoryInfo.isConquest():
					iRow = screen.appendTableRow(szTable)
					screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_ELIMINATE_ALL", ()), "", eWidGen, 1, 2, 1<<1)
					szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_RIVALS_LEFT", ()) + " "
					if nknown:
						szText += str(nknown)
					elif nRivals:
						szText += szUnknown
					else:
						szText += TRNSLTR.getText("TXT_KEY_NONE", ())
					screen.setTableText(szTable, 1, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<2)

					if nVassaled:
						szText = TRNSLTR.getText("TXT_KEY_BUG_VICTORY_VASSALED", (nVassaled,))
						screen.setTableText(szTable, 2, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<0)
					if nRivals - nknown:
						szText = TRNSLTR.getText("TXT_KEY_BUG_VICTORY_UNKNOWN", (nRivals - nknown,))
						screen.setTableText(szTable, 3, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<0)

				if GAME.getAdjustedPopulationPercent(iLoopVC) > 0:
					iRow = screen.appendTableRow(szTable)
					szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_PERCENT_POP", (GAME.getAdjustedPopulationPercent(iLoopVC), ))
					screen.setTableText(szTable, 0, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<1)
					screen.setTableText(szTable, 1, iRow, ufont2 + (u"%.2f%%" % fPopPercent), "", eWidGen, 1, 2, 1<<2)
					if iBestPopTeam != -1:
						screen.setTableText(szTable, 2, iRow, ufont2 + GC.getTeam(iBestPopTeam).getName() + ":", "", eWidGen, 1, 2, 1<<0)
						screen.setTableText(szTable, 3, iRow, ufont2 + (u"%.2f%%" % (iBestPop * 100 / iTotalPop)), "", eWidGen, 1, 2, 1<<0)

				if GAME.getAdjustedLandPercent(iLoopVC) > 0:
					iRow = screen.appendTableRow(szTable)
					szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_PERCENT_LAND", (GAME.getAdjustedLandPercent(iLoopVC), ))
					screen.setTableText(szTable, 0, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<1)
					screen.setTableText(szTable, 1, iRow, ufont2 + (u"%.2f%%" % fLandPercent), "", eWidGen, 1, 2, 1<<2)
					if iBestLandTeam != -1:
						screen.setTableText(szTable, 2, iRow, ufont2 + GC.getTeam(iBestLandTeam).getName() + ":", "", eWidGen, 1, 2, 1<<0)
						screen.setTableText(szTable, 3, iRow, ufont2 + (u"%.2f%%" % (iBestLand * 100 / iTotalLand)), "", eWidGen, 1, 2, 1<<0)

				if CvVictoryInfo.getReligionPercent() > 0:
					iRow = screen.appendTableRow(szTable)
					szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_PERCENT_RELIGION", (CvVictoryInfo.getReligionPercent(),))
					screen.setTableText(szTable, 0, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<1)
					if iOurReligion != -1:
						szText = GC.getReligionInfo(iOurReligion).getDescription() + u": %d%%" % ourReligionPercent
						screen.setTableText(szTable, 1, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<2)
					else:
						szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_NO_HOLY", ())
						screen.setTableText(szTable, 1, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<2)
					if iBestReligion != -1:
						szText = GC.getReligionInfo(iBestReligion).getDescription() + u": %d%%" % religionPercent
						screen.setTableText(szTable, 2, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<0)

				if CvVictoryInfo.getTotalCultureRatio() > 0:
					iRow = screen.appendTableRow(szTable)
					szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_PERCENT_CULTURE", (int((100.0 * iBestCulture) / CvVictoryInfo.getTotalCultureRatio()),))
					screen.setTableText(szTable, 0, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<1)
					screen.setTableText(szTable, 1, iRow, ufont2 + unicode(iOurCulture), "", eWidGen, 1, 2, 1<<2)
					if iBestLandTeam != -1:
						screen.setTableText(szTable, 2, iRow, ufont2 + GC.getTeam(iBestCultureTeam).getName() + ":", "", eWidGen, 1, 2, 1<<0)
						screen.setTableText(szTable, 3, iRow, ufont2 + unicode(iBestCulture), "", eWidGen, 1, 2, 1<<0)

				iBestBuildingTeam = -1
				bestBuilding = 0
				for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
					if iTeamX == iTeamAct: continue
					CyTeamX = GC.getTeam(iTeamX)
					if CyTeamX.isAlive() and not CyTeamX.isMinorCiv():
						if CyTeam.isHasMet(iTeamX) or bDebugModeDLL:
							teamBuilding = 0
							for i in xrange(GC.getNumBuildingInfos()):
								if GC.getBuildingInfo(i).getVictoryThreshold(iLoopVC) > 0:
									teamBuilding += CyTeamX.getBuildingCount(i)
							if teamBuilding > bestBuilding:
								bestBuilding = teamBuilding
								iBestBuildingTeam = iTeamX

				for i in xrange(GC.getNumBuildingInfos()):
					if GC.getBuildingInfo(i).getVictoryThreshold(iLoopVC) > 0:
						iRow = screen.appendTableRow(szTable)
						szNumber = unicode(GC.getBuildingInfo(i).getVictoryThreshold(iLoopVC))
						szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_BUILDING", (szNumber, GC.getBuildingInfo(i).getTextKey()))
						screen.setTableText(szTable, 0, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<1)
						screen.setTableText(szTable, 1, iRow, ufont2 + str(CyTeam.getBuildingCount(i)), "", eWidGen, 1, 2, 1<<2)
						if iBestBuildingTeam != -1:
							screen.setTableText(szTable, 2, iRow, ufont2 + GC.getTeam(iBestBuildingTeam).getName() + ":", "", eWidGen, 1, 2, 1<<0)
							screen.setTableText(szTable, 3, iRow, ufont2 + str(GC.getTeam(iBestBuildingTeam).getBuildingCount(i)), "", eWidGen, 1, 2, 1<<0)

				iBestProjectTeam = -1
				bestProject = -1
				for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
					if iTeamX == iTeamAct: continue
					CyTeamX = GC.getTeam(iTeamX)
					if CyTeamX.isAlive() and not CyTeamX.isMinorCiv():
						if (CyTeam.isHasMet(iTeamX) or bDebugModeDLL) and self.isApolloBuiltbyTeam(CyTeamX):
							teamProject = 0
							for i in xrange(GC.getNumProjectInfos()):
								if GC.getProjectInfo(i).getVictoryThreshold(iLoopVC) > 0:
									teamProject += CyTeamX.getProjectCount(i)
							if teamProject > bestProject:
								bestProject = teamProject
								iBestProjectTeam = iTeamX

				bApolloShown = False
				for i in xrange(GC.getNumProjectInfos()):
					CvProjectInfo = GC.getProjectInfo(i)
					iVictoryThreshold = CvProjectInfo.getVictoryThreshold(iLoopVC)
					if iVictoryThreshold > 0:
						if not self.isApolloBuilt():
							iRow = screen.appendTableRow(szTable)
							screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_PROJECT_APOLLO_PROGRAM", ()), "", eWidGen, 1, 2, 1<<1)
							screen.setTableText(szTable, 1, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_NOT_BUILT", ()), "", eWidGen, 1, 2, 1<<2)
							break
						else:
							bApolloBuiltByActiveTeam = self.isApolloBuiltbyTeam(CyTeam)
							if not bApolloShown:
								bApolloShown = True
								iRow = screen.appendTableRow(szTable)
								screen.setTableText(szTable, 0, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_PROJECT_APOLLO_PROGRAM", ()), "", eWidGen, 1, 2, 1<<1)

								if bApolloBuiltByActiveTeam:
									screen.setTableText(szTable, 1, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_BUILT", (szTeamName, )), "", eWidGen, 1, 2, 1<<2)
								else:
									screen.setTableText(szTable, 1, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_NOT_BUILT", ()), "", eWidGen, 1, 2, 1<<2)

								if iBestProjectTeam != -1:
									screen.setTableText(szTable, 2, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_BUILT", (GC.getTeam(iBestProjectTeam).getName(), )), "", eWidGen, 1, 2, 1<<0)

							iRow = screen.appendTableRow(szTable)
							iReqTech = CvProjectInfo.getTechPrereq()

							if CvProjectInfo.getVictoryMinThreshold(iLoopVC) == iVictoryThreshold:
								szNumber = unicode(iVictoryThreshold)
							else:
								szNumber = unicode(CvProjectInfo.getVictoryMinThreshold(iLoopVC)) + u"-" + unicode(iVictoryThreshold)

							sSSPart = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_BUILDING", (szNumber, CvProjectInfo.getTextKey()))
							screen.setTableText(szTable, 0, iRow, ufont2 + sSSPart, "", eWidGen, 1, 2, 1<<1)

							if bApolloBuiltByActiveTeam:
								bOwnProject = CyTeam.getProjectCount(i)

								bHasTech = CyTeam.isHasTech(iReqTech)
								sSSPlayer = szTeamName + ":"
								sSSCount = "%i [+%i]" % (bOwnProject, CyTeam.getProjectMaking(i))

								iHasTechColor = -1
								iSSColor = 0
								if bOwnProject == iVictoryThreshold:
									sSSCount = "%i" % (bOwnProject)
									iSSColor = self.COLOR_GREEN
								elif bOwnProject >= CvProjectInfo.getVictoryMinThreshold(iLoopVC):
									iSSColor = self.COLOR_YELLOW

								if iSSColor > 0:
									sSSPlayer = TRNSLTR.changeTextColor(sSSPlayer, iSSColor)
									sSSCount = TRNSLTR.changeTextColor(sSSCount, iSSColor)

								szText = sSSPlayer
								if bHasTech:
									szText += " - " + sSSCount
								screen.setTableText(szTable, 1, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<2)

								if CvProjectInfo.isSpaceship():
									# Add spaceship button
									screen.setButtonGFC("SpaceShipButton" + str(iLoopVC), TRNSLTR.getText("TXT_KEY_GLOBELAYER_STRATEGY_VIEW", ()), "", 0, 0, 15, 10, eWidGen, 1234, -1, ButtonStyles.BUTTON_STYLE_STANDARD)
									screen.attachControlToTableCell("SpaceShipButton" + str(iLoopVC), szTable, iCategoryRow, 1)

									victoryDelay = GC.getTeam(iTeamAct).getVictoryCountdown(iLoopVC)
									if victoryDelay > 0 and GAME.getGameState() != GameStateTypes.GAMESTATE_EXTENDED:
										szDate = CyGameTextMgr().getTimeStr(GAME.getGameTurn() + victoryDelay, False)
										szText = TRNSLTR.getText("TXT_KEY_SPACE_SHIP_SCREEN_ARRIVAL", ()) + ": " + szDate
										screen.setTableText(szTable, 1, iCategoryRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<2)
										szText = TRNSLTR.getText("TXT_KEY_TURNS", ()) + ":"
										screen.setTableText(szTable, 2, iCategoryRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<0)
										screen.setTableText(szTable, 3, iCategoryRow, ufont2 + str(victoryDelay), "", eWidGen, 1, 2, 1<<0)

							# add AI space ship info
							if iBestProjectTeam != -1:
								CyTeamBest = GC.getTeam(iBestProjectTeam)
								bOwnProject = CyTeamBest.getProjectCount(i)
								sSSPlayer = GC.getTeam(iBestProjectTeam).getName() + ":"
								sSSCount = "%i" % (bOwnProject)

								iHasTechColor = -1
								iSSColor = 0
								if bOwnProject == iVictoryThreshold:
									iSSColor = self.COLOR_GREEN
								elif bOwnProject >= CvProjectInfo.getVictoryMinThreshold(iLoopVC):
									iSSColor = self.COLOR_YELLOW
								elif CyTeamBest.isHasTech(iReqTech) and (CyTeam.isHasTech(iReqTech) or CyPlayer.canResearch(iReqTech)):
									iSSColor = GC.getInfoTypeForString("COLOR_PLAYER_ORANGE")

								if iSSColor > 0:
									sSSPlayer = TRNSLTR.changeTextColor(sSSPlayer, iSSColor)
									sSSCount = TRNSLTR.changeTextColor(sSSCount, iSSColor)

								screen.setTableText(szTable, 2, iRow, ufont2 + sSSPlayer, "", eWidGen, 1, 2, 1<<0)
								screen.setTableText(szTable, 3, iRow, ufont2 + sSSCount, "", eWidGen, 1, 2, 1<<0)

				if CvVictoryInfo.isDiploVote() and not GAME.isOption(GameOptionTypes.GAMEOPTION_UNITED_NATIONS):
					for (iVoteBuilding, iUNTeam, bUnknown) in aiVoteBuilding:
						iRow = screen.appendTableRow(szTable)
						szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_ELECTION", (GC.getBuildingInfo(iVoteBuilding).getTextKey(),))
						screen.setTableText(szTable, 0, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<1)
						if iUNTeam != -1:
							if bUnknown:
								szName = TRNSLTR.getText("TXT_KEY_TOPCIVS_UNKNOWN", ())
							else:
								szName = GC.getTeam(iUNTeam).getName()
							screen.setTableText(szTable, 1, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_BUILT", (szName, )), "", eWidGen, 1, 2, 1<<2)
						else:
							screen.setTableText(szTable, 1, iRow, ufont2 + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_NOT_BUILT", ()), "", eWidGen, 1, 2, 1<<2)

				iNumCultureCities = CvVictoryInfo.getNumCultureCities()
				if CvVictoryInfo.getCityCulture() != CultureLevelTypes.NO_CULTURELEVEL and iNumCultureCities > 0:
					ourBestCities = self.getListCultureCities(iActivePlayer, CvVictoryInfo)[0:iNumCultureCities]

					iBestCulturePlayer = -1
					bestCityCulture = 0
					maxCityCulture = GAME.getCultureThreshold(CvVictoryInfo.getCityCulture())

					for iPlayerX in xrange(GC.getMAX_PLAYERS()):
						CyPlayerX = GC.getPlayer(iPlayerX)
						if CyPlayerX.isAlive() and not CyPlayerX.isMinorCiv() and not CyPlayerX.isNPC():
							if iPlayerX != iActivePlayer and (CyTeam.isHasMet(CyPlayerX.getTeam()) or bDebugModeDLL):
								theirBestCities = self.getListCultureCities(iPlayerX, CvVictoryInfo)[0:iNumCultureCities]

								iTotalCulture = 0
								for loopCity in theirBestCities:
									if loopCity[0] >= maxCityCulture:
										iTotalCulture += maxCityCulture
									else:
										iTotalCulture += loopCity[0]

								if iTotalCulture >= bestCityCulture:
									bestCityCulture = iTotalCulture
									iBestCulturePlayer = iPlayerX

					if iBestCulturePlayer != -1:
						theirBestCities = self.getListCultureCities(iBestCulturePlayer, CvVictoryInfo)[0:iNumCultureCities]
					else:
						theirBestCities = []

					iRow = screen.appendTableRow(szTable)
					szText = TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_CITY_CULTURE", (iNumCultureCities, GC.getCultureLevelInfo(CvVictoryInfo.getCityCulture()).getTextKey()))
					screen.setTableText(szTable, 0, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<1)

					for i in xrange(iNumCultureCities):
						if len(ourBestCities) > i or len(theirBestCities) > i:
							if i:
								iRow = screen.appendTableRow(szTable)

							if len(ourBestCities) > i:
								szText = ourBestCities[i][1].getName() + ": "
								if ourBestCities[i][2] < 1:
									szText += TRNSLTR.getText("TXT_KEY_VICTORY_SCORE", ())
								elif ourBestCities[i][2] < 1000:
									szText += TRNSLTR.getText("TXT_KEY_MISC_TURNS_LEFT", (ourBestCities[i][2],))
								else:
									szText += GC.getCultureLevelInfo(ourBestCities[i][1].getCultureLevel()).getDescription()
								screen.setTableText(szTable, 1, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<2)

							if len(theirBestCities) > i:
								szText = theirBestCities[i][1].getName() + ": "
								if theirBestCities[i][2] < 1:
									szText += TRNSLTR.getText("TXT_KEY_VICTORY_SCORE", ())
								elif theirBestCities[i][2] < 1000:
									szText += TRNSLTR.getText("TXT_KEY_MISC_TURNS_LEFT", (theirBestCities[i][2],))
								else:
									szText += GC.getCultureLevelInfo(theirBestCities[i][1].getCultureLevel()).getDescription()
								screen.setTableText(szTable, 2, iRow, ufont2 + szText, "", eWidGen, 1, 2, 1<<0)
						else:
							break


	def showGameSettingsScreen(self, screen):
		ufontEdge, ufont4b, ufont4, ufont3b, ufont3, ufont2b, ufont2, ufont1 = self.aFontList
		a3rdX = self.xRes / 3
		dx = a3rdX / 16
		x = dx
		y = self.Y_TOP_PAGE + 16
		dx = a3rdX - 2 * dx
		dy = self.H_PAGE - 32

		iPlayerAct = self.iActivePlayer
		CyPlayer = GC.getPlayer(iPlayerAct)

		self.deleteAllWidgets()
		eWidGen = WidgetTypes.WIDGET_GENERAL
		iPanelMain = PanelStyles.PANEL_STYLE_MAIN
		iTableEmpty = TableStyles.TABLE_STYLE_EMPTY
		iCol = self.COLOR_YELLOW
		BULLET = u'%c' %(GAME.getSymbolID(FontSymbols.BULLET_CHAR))

		screen.addPanel(self.getNextWidgetName(), "", "", True, True, x, y, dx, dy, iPanelMain)
		szSettingsTable = self.getNextWidgetName()
		screen.addListBoxGFC(szSettingsTable, "", x + 16, y + 16, dx - 32, dy - 32, iTableEmpty)
		screen.enableSelect(szSettingsTable, False)

		szTxt = ufont3b + TRNSLTR.getColorText("TXT_KEY_MAIN_MENU_SETTINGS", (), iCol) + "\n"
		screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<2)

		szTxt = BULLET + ufont2 + TRNSLTR.getText("TXT_KEY_LEADER_CIV_DESCRIPTION", (CyPlayer.getNameKey(), CyPlayer.getCivilizationShortDescriptionKey()))
		screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<0)

		szTxt = CyGameTextMgr().parseLeaderTraits(CyPlayer.getLeaderType(), CyPlayer.getCivilizationType(), True, False)
		if szTxt:
			szTxt = ufont1 + "\t<color=200,200,200>(" + szTxt + ")\n"
			screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<0)

		szTxt = BULLET + ufont2 + TRNSLTR.getText("TXT_KEY_SETTINGS_DIFFICULTY", (GC.getHandicapInfo(CyPlayer.getHandicapType()).getTextKey(),))
		screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<0)

		szTxt = BULLET + ufont2 + TRNSLTR.getText("TXT_KEY_SETTINGS_GAME_SPEED", (GC.getGameSpeedInfo(GAME.getGameSpeedType()).getTextKey(),))
		screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<0)

		CyMap = GC.getMap()
		szTxt = BULLET + ufont2 + CyMap.getMapScriptName()
		screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<0)

		if CyMap.isWrapY():
			szTxt = "\t" + ufont1 + TRNSLTR.getText("TXT_KEY_MAP_WRAP_TOROID", ())
		elif CyMap.isWrapX():
			szTxt = "\t" + ufont1 + TRNSLTR.getText("TXT_KEY_MAP_WRAP_CYLINDER", ())
		else:
			szTxt = "\t" + ufont1 + TRNSLTR.getText("TXT_KEY_MAP_WRAP_FLAT", ())
		screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<0)

		szTxt = "\t" + ufont1 + TRNSLTR.getText("TXT_KEY_SETTINGS_MAP_SIZE", (GC.getWorldInfo(CyMap.getWorldSize()).getTextKey(),))
		szTxt += u"  (%dx%d)" %(CyMap.getGridWidth(), CyMap.getGridHeight())
		screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<0)

		szTxt = "\t" + ufont1 + TRNSLTR.getText("TXT_KEY_SETTINGS_CLIMATE", (GC.getClimateInfo(CyMap.getClimate()).getTextKey(),))
		screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<0)

		szTxt = "\t" + ufont1 + TRNSLTR.getText("TXT_KEY_SETTINGS_SEA_LEVEL", (GC.getSeaLevelInfo(CyMap.getSeaLevel()).getTextKey(),)) + "\n"
		screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<0)

		szTxt = BULLET + TRNSLTR.getText("TXT_KEY_SETTINGS_STARTING_ERA", (GC.getEraInfo(GAME.getStartEra()).getTextKey(),))
		screen.appendListBoxStringNoUpdate(szSettingsTable, szTxt, eWidGen, 1, 2, 1<<0)

		screen.updateListBox(szSettingsTable)

		x += a3rdX
		screen.addPanel(self.getNextWidgetName(), "", "", True, True, x, y, dx, dy, iPanelMain)
		szOptionsTable = self.getNextWidgetName()
		screen.addListBoxGFC(szOptionsTable, "", x + 16, y + 16, dx - 32, dy - 32, iTableEmpty)
		screen.enableSelect(szOptionsTable, False)

		szTxt = ufont3b + TRNSLTR.getColorText("TXT_KEY_MAIN_MENU_CUSTOM_SETUP_OPTIONS", (), iCol) + "\n"
		screen.appendListBoxStringNoUpdate(szOptionsTable, szTxt, eWidGen, 1, 2, 1<<2)

		if GAME.isGameMultiPlayer():
			for i in xrange(GC.getNumMPOptionInfos()):
				if GAME.isMPOption(i):
					szTxt = BULLET + ufont2 + GC.getMPOptionInfo(i).getDescription() + "\n"

			if GAME.getMaxTurns() > 0:
				szTxt += ufont1 + u"\t%s %d" % (TRNSLTR.getText("TXT_KEY_TURN_LIMIT_TAG", ()), GAME.getMaxTurns()) + "\n"

			if GAME.getMaxCityElimination() > 0:
				szTxt += ufont1 + u"\t%s %d" % (TRNSLTR.getText("TXT_KEY_CITY_ELIM_TAG", ()), GAME.getMaxCityElimination()) + "n"

			screen.appendListBoxStringNoUpdate(szOptionsTable, szTxt, eWidGen, 1, 2, 1<<0)

		if GAME.isOption(GameOptionTypes.GAMEOPTION_ADVANCED_START):
			szTxt = BULLET + ufont2 + u"%s %d" %(TRNSLTR.getText("TXT_KEY_ADVANCED_START_POINTS", ()), GAME.getNumAdvancedStartPoints()) + "\n"
			screen.appendListBoxStringNoUpdate(szOptionsTable, szTxt, eWidGen, 1, 2, 1<<0)

		for i in xrange(GC.getNumGameOptionInfos()):
			if GAME.isOption(i):
				screen.appendListBoxStringNoUpdate(szOptionsTable, BULLET + ufont2 + GC.getGameOptionInfo(i).getDescription(), eWidGen, 1, 2, 1<<0)

		if GAME.hasSkippedSaveChecksum():
			szTxt = ufont2 + TRNSLTR.getText("TXT_KEY_BUFFYWARNING_CHECKSUM_SKIPPED", ())
			screen.appendListBoxStringNoUpdate(szOptionsTable, BULLET + szTxt, eWidGen, 1, 2, 1<<0)

		screen.updateListBox(szOptionsTable)

		x += a3rdX
		screen.addPanel(self.getNextWidgetName(), "", "", True, True, x, y, dx, dy, iPanelMain)
		szCivsTable = self.getNextWidgetName()
		screen.addListBoxGFC(szCivsTable, "", x + 16, y + 16, dx - 32, dy - 32, iTableEmpty)
		screen.enableSelect(szCivsTable, False)

		szTxt = ufont3b + TRNSLTR.getColorText("TXT_KEY_RIVALS_MET", (), iCol) + "\n"
		screen.appendListBoxStringNoUpdate(szCivsTable, szTxt, eWidGen, 1, 2, 1<<2)

		iTeam = CyPlayer.getTeam()
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			if iPlayerX == iPlayerAct:
				continue
			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isEverAlive() or CyPlayerX.isMinorCiv():
				continue
			if GC.getTeam(CyPlayerX.getTeam()).isHasMet(iTeam) or GAME.isDebugMode():
				szTxt = BULLET + ufont2 + TRNSLTR.getText("TXT_KEY_LEADER_CIV_DESCRIPTION", (CyPlayerX.getNameKey(), CyPlayerX.getCivilizationShortDescriptionKey()))
				screen.appendListBoxStringNoUpdate(szCivsTable, szTxt, eWidGen, 1, 2, 1<<0)
				szTxt = CyGameTextMgr().parseLeaderTraits(CyPlayerX.getLeaderType(), CyPlayerX.getCivilizationType(), True, False)
				if szTxt:
					szTxt = ufont1 + "\t<color=200,200,200>(" + szTxt + ")"
					screen.appendListBoxStringNoUpdate(szCivsTable, szTxt, eWidGen, 1, 2, 1<<0)

		screen.updateListBox(szCivsTable)


	def showVotingScreen(self, screen):
		uFont = self.aFontList[6]
		self.deleteAllWidgets()

		CyPlayer = GC.getPlayer(self.iActivePlayer)
		iTeamAct = CyPlayer.getTeam()
		CyTeam = GC.getTeam(iTeamAct)

		aiVoteBuilding = []
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			for j in xrange(GC.getNumVoteSourceInfos()):
				if CvBuildingInfo.getVoteSourceType() == j:
					iUNTeam = -1
					bUnknown = True

					for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
						CyPlayerX = GC.getPlayer(iPlayerX)
						if CyPlayerX.isAlive() and not CyPlayerX.isMinorCiv() and CyPlayerX.hasBuilding(i):
							iUNTeam = CyPlayerX.getTeam()
							if iUNTeam == iTeamAct or GAME.isDebugMode() or CyTeam.isHasMet(iUNTeam):
								bUnknown = False
							break

					aiVoteBuilding.append((CvBuildingInfo.getTextKey(), iUNTeam, bUnknown))

		if not aiVoteBuilding:
			return

		eWidGen = WidgetTypes.WIDGET_GENERAL
		xRes = self.xRes - 4
		szTable = self.getNextWidgetName()
		screen.addTableControlGFC(szTable, 2, 2, self.Y_TOP_PAGE, self.xRes, self.H_PAGE, False, True, 32,32, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSelect(szTable, False)
		screen.setTableColumnHeader(szTable, 0, "", xRes/4)
		screen.setTableColumnHeader(szTable, 1, "", 3*xRes/4)

		szUnknown = TRNSLTR.getText("TXT_KEY_TOPCIVS_UNKNOWN", ())
		szTxt = uFont + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_NOT_BUILT", ())
		for TEXT_KEY, iUNTeam, bUnknown in aiVoteBuilding:
			iRow = screen.appendTableRow(szTable)
			if iUNTeam != -1:
				if bUnknown:
					szName = szUnknown
				else:
					szName = GC.getTeam(iUNTeam).getName()
				screen.setTableText(szTable, 0, iRow, uFont + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_BUILT", (szName, )), "", eWidGen, 1, 2, 1<<2)
			else:
				screen.setTableText(szTable, 0, iRow, szTxt, "", eWidGen, 1, 2, 1<<2)
			screen.setTableText(szTable, 1, iRow, uFont + TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_ELECTION", (TEXT_KEY,)), "", eWidGen, 1, 2, 1<<0)

		iNumVoteInfos = GC.getNumVoteInfos()
		szPassed = uFont + TRNSLTR.getText("TXT_KEY_POPUP_PASSED", ())
		for i in xrange(GC.getNumVoteSourceInfos()):
			screen.appendTableRow(szTable)
			if not GAME.canHaveSecretaryGeneral(i): continue
			iTeamSG = GAME.getSecretaryGeneral(i)
			if -1 != iTeamSG:
				iRow = screen.appendTableRow(szTable)
				screen.setTableText(szTable, 0, iRow, uFont + GC.getTeam(iTeamSG).getName(), "", eWidGen, 1, 2, 1<<2)
				screen.setTableText(szTable, 1, iRow, uFont + GC.getVoteSourceInfo(i).getSecretaryGeneralText(), "", eWidGen, 1, 2, 1<<0)

			for iLoop in xrange(iNumVoteInfos):
				if GAME.isChooseElection(iLoop):
					iCount = GAME.countPossibleVote(iLoop, i)
					if iCount > 0:
						iRow = screen.appendTableRow(szTable)
						if GAME.isVotePassed(iLoop):
							screen.setTableText(szTable, 0, iRow, szPassed, "", eWidGen, 1, 2, 1<<2)
						else:
							szTxt = TRNSLTR.getText("TXT_KEY_POPUP_ELECTION_OPTION", ("", GAME.getVoteRequired(iLoop, i), iCount))
							screen.setTableText(szTable, 0, iRow, uFont + szTxt, "", eWidGen, 1, 2, 1<<2)

						screen.setTableText(szTable, 1, iRow, uFont + GC.getVoteInfo(iLoop).getDescription(), "", eWidGen, 1, 2, 1<<0)


# BUG Additions Start
	def showMembersScreen(self, screen):
		self.deleteAllWidgets()

		iRelVote = -1
		iRelVoteIdx = -1
		iUNVote = -1
		iUNVoteIdx = -1

		iNumVoteInfos = GC.getNumVoteInfos()
		aList = []
		for i in xrange(GC.getNumVoteSourceInfos()):
			iVoteSourceReligion = GAME.getVoteSourceReligion(i)
			aList.append(iVoteSourceReligion)
			if GAME.isDiploVote(i):
				if iVoteSourceReligion != -1:
					iRelVote = i
				else:
					iUNVote = i
		if iRelVote == -1 and iUNVote == -1: return  # AP nor UN are active

		if  iRelVote == -1: self.VoteBody = 2 # AP not active
		elif iUNVote == -1: self.VoteBody = 1 # UN not active

		for i, iVoteSourceReligion in enumerate(aList):
			if GAME.canHaveSecretaryGeneral(i) and GAME.getSecretaryGeneral(i) != -1:
				for j in xrange(iNumVoteInfos):
					if GC.getVoteInfo(j).isVoteSourceType(i):
						if GC.getVoteInfo(j).isSecretaryGeneral():
							if iVoteSourceReligion != -1:
								iRelVoteIdx = j
							else:
								iUNVoteIdx = j
							break

		del iNumVoteInfos, aList

		if self.VoteBody == 1:
			iVoteBody = iRelVote
			iVoteIdx = iRelVoteIdx
		else:
			iVoteBody = iUNVote
			iVoteIdx = iUNVoteIdx

		iPlayerAct = self.iActivePlayer
		CyPlayerAct = GC.getPlayer(iPlayerAct)
		iTeamAct = CyPlayerAct.getTeam()
		CyTeamAct = GC.getTeam(iTeamAct)

		screen.addPanel(self.getNextWidgetName(), "", "", False, False, 0, 45, 1030, 680, PanelStyles.PANEL_STYLE_BLUE50)

		# set up the header row
		szHeading = self.getNextWidgetName()
		screen.addTableControlGFC(szHeading, 3, 10, 60, 1010, 30, False, False, 32,32, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader(szHeading, 0, "", 450)
		screen.setTableColumnHeader(szHeading, 1, "", 180)
		screen.setTableColumnHeader(szHeading, 2, "", 180)
		iRow = screen.appendTableRow(szHeading)

		# heading
		kVoteSource = GC.getVoteSourceInfo(iVoteBody)
		sTableHeader = "<font=4b>" + kVoteSource.getDescription().upper() + "</font>"
		if GAME.getVoteSourceReligion(iVoteBody) != -1:
			sTableHeader += " (" + GC.getReligionInfo(GAME.getVoteSourceReligion(iVoteBody)).getDescription() + ")"

		screen.setTableText(szHeading, 0, iRow, sTableHeader, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		# determine the two candidates, add to header
		iCandTeam1 = -1
		iCandTeam2 = -1
		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
			CyTeamX = GC.getTeam(iTeamX)
			if CyTeamX.isAlive() and GAME.isTeamVoteEligible(iTeamX, iVoteBody):
				print "CvVictoryScreen: Team %i, %s <- vote eligible " %(iTeamX, CyTeamX.getName())
				if iCandTeam1 == -1:
					iCandTeam1 = iTeamX
				else:
					iCandTeam2 = iTeamX

		# get the first player for each team
		# going to use that to calculation attitude - too hard to calc attitude for team
		iCandPlayer1 = self.getPlayerOnTeam(iCandTeam1)
		iCandPlayer2 = self.getPlayerOnTeam(iCandTeam2)

		# candidate known returns -1 if there is no candidate, 0 if not known or 1 if known
		iCand1Known, sCand1Name = self.getCandStatusName(iCandTeam1)
		iCand2Known, sCand2Name = self.getCandStatusName(iCandTeam2)

		screen.setTableText(szHeading, 1, iRow, sCand1Name, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)
		screen.setTableText(szHeading, 2, iRow, sCand2Name, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)

		# set up the member table
		szTable = self.getNextWidgetName()
		screen.addTableControlGFC(szTable, 6, 10, 90, 1010, 600, False, False, 32,32, TableStyles.TABLE_STYLE_STANDARD)
		screen.enableSelect(szTable, False)
		screen.setTableColumnHeader(szTable, 0, "", 450)
		screen.setTableColumnHeader(szTable, 1, "", 90)
		screen.setTableColumnHeader(szTable, 2, "", 90)
		screen.setTableColumnHeader(szTable, 3, "", 90)
		screen.setTableColumnHeader(szTable, 4, "", 90)
		screen.setTableColumnHeader(szTable, 5, "", 200)
		iRow = screen.appendTableRow(szTable)

		# set up the vote selection texts
		iYellow = self.COLOR_YELLOW
		iX = 994
		szTxt = GC.getVoteSourceInfo(iVoteBody).getSecretaryGeneralText()
		if self.VoteType == 1:
			szTxt = TRNSLTR.changeTextColor(szTxt, iYellow)

		screen.setText("BUGVotePope_Widget", "", szTxt, 1<<1, iX, 688, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iX -= 10 + CyInterface().determineWidth(szTxt)
		szTxt = TRNSLTR.getText("TXT_KEY_BUG_VICTORY_DIPLOMATIC", ())
		if self.VoteType == 2:
			szTxt = TRNSLTR.changeTextColor(szTxt, iYellow)

		screen.setText("BUGVoteDiplomaticVictory_Widget", "", szTxt, 1<<1, iX, 688, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		if iRelVote != -1 and iUNVote != -1:  # both AP and UN are active
			iX = 20
			szTxt = GC.getVoteSourceInfo(iRelVote).getDescription()
			if iVoteBody == iRelVote:
				szTxt = TRNSLTR.changeTextColor(szTxt, iYellow)

			screen.setText("VoteAP", "", szTxt, 1<<0, iX, 688, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

			iX += 10 + CyInterface().determineWidth(szTxt)
			szTxt = GC.getVoteSourceInfo(iUNVote).getDescription()
			if iVoteBody == iUNVote:
				szTxt = TRNSLTR.changeTextColor(szTxt, iYellow)

			screen.setText("VoteUN", "", szTxt, 1<<0, iX, 688, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		else:
			screen.hide("VoteAP")
			screen.hide("VoteUN")

		# initialize the candidate votes and total vote counter
		iVoteTotal = [0] * 2
		iVoteCand = [0] * 2
		iAPUNTeam = self.getAP_UN_OwnerTeam()

		aList = []
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive():
				iVotes = CyPlayerX.getVotes(iVoteIdx, iVoteBody) # so that it sorts from most votes to least
				aList.append([iVotes, CyPlayerX, iPlayerX])
		aList.sort(reverse=True)

		for iVotes, CyPlayerX, iPlayerX in aList:

			iTeamX = CyPlayerX.getTeam()
			# player name
			if iTeamAct == iTeamX or CyTeamAct.isHasMet(iTeamX):
				bKnown = True
				szTxt = CyPlayerX.getName()
			else:
				bKnown = False
				szTxt = TRNSLTR.getText("TXT_KEY_TOPCIVS_UNKNOWN", ())

			if iVotes > 0 and bKnown:
				szTxt += TRNSLTR.getText("TXT_KEY_VICTORY_SCREEN_PLAYER_VOTES", (iVotes, iVoteBody),)

			iRow = screen.appendTableRow(szTable)
			screen.setTableText(szTable, 0, iRow, szTxt, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

			if iPlayerX != iPlayerAct and bKnown:
				# player attitude to candidate #1
				szText = AttitudeUtil.getAttitudeText(iPlayerX, iCandPlayer1, True, True, False, False)
				if szText and iCand1Known == 1:
					screen.setTableText(szTable, 1, iRow, szText, "", WidgetTypes.WIDGET_LEADERHEAD, iPlayerX, iCandPlayer1, 1<<2)

				# player attitude to candidate #2
				szText = AttitudeUtil.getAttitudeText(iPlayerX, iCandPlayer2, True, True, False, False)
				if szText and iCand2Known == 1:
					screen.setTableText(szTable, 3, iRow, szText, "", WidgetTypes.WIDGET_LEADERHEAD, iPlayerX, iCandPlayer2, 1<<2)

			iVote = self.getVotesForWhichCandidate(iPlayerX, iCandTeam1, iCandTeam2, self.VoteType)
			iVote_Column = -1

			if iVote != -1:
				sVote = str(iVotes)
				iVoteTotal[iVote - 1] += iVotes

				# number of votes for Candidate #1
				if iVote == 1 and iVotes > 0 and iCand1Known == 1:
					screen.setTableText(szTable, 2, iRow, sVote, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)

				# number of votes for Candidate #2
				if iVote == 2 and iVotes > 0 and iCand2Known == 1:
					screen.setTableText(szTable, 4, iRow, sVote, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)

			# player status
			if bKnown:
				if GAME.canHaveSecretaryGeneral(iVoteBody) and iAPUNTeam == iTeamX and GAME.getSecretaryGeneral(iVoteBody) == -1:
					szLabel = GC.getVoteSourceInfo(iVoteBody).getSecretaryGeneralText()

				elif GAME.canHaveSecretaryGeneral(iVoteBody) and GAME.getSecretaryGeneral(iVoteBody) == iTeamX:
					szLabel = GC.getVoteSourceInfo(iVoteBody).getSecretaryGeneralText()

				elif CyPlayerX.isFullMember(iVoteBody):
					szLabel = TRNSLTR.getText("TXT_KEY_VOTESOURCE_FULL_MEMBER", ())

				elif CyPlayerX.isVotingMember(iVoteBody):
					szLabel = TRNSLTR.getText("TXT_KEY_VOTESOURCE_VOTING_MEMBER", ())
				else:
					szLabel = TRNSLTR.getText("TXT_KEY_VOTESOURCE_NON_VOTING_MEMBER", ())

				screen.setTableText(szTable, 5, iRow, szLabel, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<1)

			# store the candidates own votes
			if iCandTeam1 == iTeamX and iCand1Known == 1:
				iVoteCand[0] = iVotes
			if iCandTeam2 == iTeamX and iCand2Known == 1:
				iVoteCand[1] = iVotes

		# calculate the maximum number of votes
		iMaxVotes = 0
		for i in xrange(GC.getNumVoteInfos()):
			if GAME.countPossibleVote(i, iVoteBody) > 0:
				iMaxVotes = GAME.countPossibleVote(i, iVoteBody)
				break

		iRow = screen.appendTableRow(szTable)
		iVoteReq = self.getVoteReq(iVoteBody, self.VoteType)
		sVoteReq = "%i" % (iVoteReq)
		sString = "<font=3b>" + TRNSLTR.getText("TXT_KEY_CONCEPT_TOTAL", ()) + "</font> "
		if iCand1Known and iCand2Known:
			sString +=  TRNSLTR.getText("TXT_KEY_BUG_VICTORY_VOTES_REQUIRED", (sVoteReq,))
		screen.setTableText(szTable, 0, iRow, sString, "", WidgetTypes.WIDGET_GENERAL, 1, 1, 1<<0)

		if iCand1Known == 1:
			# color code the totals
			sVoteTotal = str(iVoteTotal[0])
			iColor = self.getVoteTotalColor(iVoteReq, iVoteTotal[0], iVoteCand[0], iVoteTotal[0] > iVoteTotal[1], self.VoteType == 2)
			if iColor != -1:
				sVoteTotal = TRNSLTR.changeTextColor(sVoteTotal, iColor)
			screen.setTableText(szTable, 2, iRow, sVoteTotal, "", WidgetTypes.WIDGET_GENERAL, 1, 1, 1<<2)

		if iCand2Known == 1:
			sVoteTotal = str(iVoteTotal[1])
			iColor = self.getVoteTotalColor(iVoteReq, iVoteTotal[1], iVoteCand[1], iVoteTotal[1] > iVoteTotal[0], self.VoteType == 2)
			if iColor != -1:
				sVoteTotal = TRNSLTR.changeTextColor(sVoteTotal, iColor)
			screen.setTableText(szTable, 4, iRow, sVoteTotal, "", WidgetTypes.WIDGET_GENERAL, 1, 1, 1<<2)

		# add a blank row
		iRow = screen.appendTableRow(szTable)

		# SecGen vote prediction
		if iVoteTotal[0] > iVoteTotal[1]:
			iWinner = 0
			sWin = sCand1Name
		else:
			iWinner = 1
			sWin = sCand2Name
		iLoser = 1 - iWinner

		fVotePercent = 100.0 * iVoteTotal[iWinner] / iMaxVotes
		fMargin = 100.0 * (iVoteTotal[iWinner] - iVoteTotal[iLoser]) / iMaxVotes

		if self.VoteType == 1:
			sSecGen = GC.getVoteSourceInfo(iVoteBody).getSecretaryGeneralText()
		else:
			sSecGen = TRNSLTR.getText("TXT_KEY_BUG_VICTORY_DIPLOMATIC", ())

		# display SecGen vote prediction
		if iCand1Known != 0 and iCand2Known != 0:
			sString = sSecGen + ":"
			iRow = screen.appendTableRow(szTable)
			screen.setTableText(szTable, 0, iRow, sString, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

			sString = "     " + TRNSLTR.getText("TXT_KEY_BUG_VICTORY_BUG_POLL_RESULT", (sWin, "%.1f%%" % fVotePercent, "%.1f%%" % fMargin))
			iRow = screen.appendTableRow(szTable)
			screen.setTableText(szTable, 0, iRow, sString, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

			# BUG Poll statistical error
			fRandError = 3.5 + GC.getASyncRand().get(10, "Election Results Statistical Error") / 10.0
			sString = TRNSLTR.getText("TXT_KEY_BUG_VICTORY_BUG_POLL_ERROR", ("%.1f%%" % fRandError,))
			iRow = screen.appendTableRow(szTable)
			screen.setTableText(szTable, 0, iRow, sString, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		iRow = screen.appendTableRow(szTable)

		# add info about vote timing
		iRow = screen.appendTableRow(szTable)
		iVoteTimer = GAME.getVoteTimer(iVoteBody)
		sString = "<font=2>" + TRNSLTR.getText("TXT_KEY_BUG_VICTORY_TURNS_NEXT_VOTE", (iVoteTimer,))
		screen.setTableText(szTable, 0, iRow, sString, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		iRow = screen.appendTableRow(szTable)
		iSecGenTimer = GAME.getSecretaryGeneralTimer(iVoteBody)
		sString = "<font=2>" + TRNSLTR.getText("TXT_KEY_BUG_VICTORY_VOTES_NEXT_ELECTION", (iSecGenTimer,))
		screen.setTableText(szTable, 0, iRow, sString, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)


	def getVoteReq(self, i, iVote):
		iMaxVotes = 0
		iMinVotes = 999999
		for iLoop in xrange(GC.getNumVoteInfos()):
			iVoteReq = GAME.getVoteRequired(iLoop, i)
			if iVoteReq > 0:
				if iVoteReq > iMaxVotes:
					iMaxVotes = iVoteReq
				if iVoteReq < iMinVotes:
					iMinVotes = iVoteReq

		if iVote == 1:
			return iMinVotes
		else:
			return iMaxVotes

	def getCandStatusName(self, iTeam):
		if iTeam == -1: # there is no candidate
			return -1, "-"

		iTeamAct = GC.getPlayer(self.iActivePlayer).getTeam()

		if iTeamAct == iTeam:
			return 1, GC.getTeam(iTeam).getName()

		if GC.getTeam(iTeamAct).isHasMet(iTeam):
			return 1, GC.getTeam(iTeam).getName()
		else:
			return 0, TRNSLTR.getText("TXT_KEY_TOPCIVS_UNKNOWN", ())


	def getVoteTotalColor(self, iVoteReq, iVoteTotal, iVoteCand, bWinner, bVictoryVote):
		if not bWinner:
			return -1
		if iVoteCand > iVoteReq and bVictoryVote:
			return GC.getCOLOR_RED()
		if iVoteTotal > iVoteReq:
			return self.COLOR_GREEN
		return -1


	def getListCultureCities(self, iPlayer, CvVictoryInfo):

		if iPlayer != -1:
			CyPlayer = GC.getPlayer(iPlayer)
			if CyPlayer.isAlive():
				iThreshold = GAME.getCultureThreshold(CvVictoryInfo.getCityCulture())
				aList = []

				for CyCity in CyPlayer.cities():
					iRate = CyCity.getCommerceRateTimes100(CommerceTypes.COMMERCE_CULTURE)
					if not iRate:
						iTurns = -1
					else:
						iCultureLeftTimes100 = 100 * iThreshold - CyCity.getCultureTimes100(CyCity.getOwner())
						iTurns = int((iCultureLeftTimes100 + iRate - 1) / iRate)
					aList.append((CyCity.getCulture(iPlayer), CyCity, iTurns))

				aList.sort()
				aList.reverse()
				return aList
		return []

# Sevo--VCM
	def getListCultureCitiesTeam(self, iTeam):
		aList = []

		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive() and CyPlayerX.getTeam() == iTeam:
				for CyCity in CyPlayerX.cities():
					aList.append((CyCity.getCulture(iPlayerX), CyCity))

		if aList:
			aList.sort()
			aList.reverse()
			return aList
		return []


	def getMonumentalCities(self, iTeam):
		iMonumentalCities = 0
		iTeamCities = 0
		iThreshold = GC.getCultureLevelInfo(GC.getNumCultureLevelInfos()-1).getSpeedThreshold(GAME.getGameSpeedType())

		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive():
				iTeamX = CyPlayerX.getTeam()
				if iTeamX == iTeam:
					bTeam = True
				else:
					bTeam = False
				for CyCity in CyPlayerX.cities():
					if CyCity.getCulture(iPlayerX) > iThreshold:
						iMonumentalCities += 1
						if bTeam:
							iTeamCities += 1

		return [iTeamCities, iMonumentalCities]


	def getTeamWonderScore(self, iTeam):
		teamWonderScore = 0

		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive() and not CyPlayerX.isMinorCiv() and CyPlayerX.getTeam() == iTeam:
				teamWonderScore += CyPlayerX.getSevoWondersScore(0)
		return teamWonderScore


	def teamLaunchedShip(self, iTeam):
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive() and not CyPlayerX.isMinorCiv() and CyPlayerX.getTeam() == iTeam:
				if GAME.getStarshipLaunched(iPlayerX):
					return 1
		return -1


# BUG Additions Start
	def getVotesForWhichCandidate(self, iPlayer, iCand1, iCand2, iVote):
		# returns are 1 = vote for candidate 1
		#             2 = vote for candidate 2
		#            -1 = abstain

		# iVote = 1 means vote for SecGen or Pope
		# iVote = 2 means vote for diplomatic victory

		# candidates are teams!!!

		# * AI votes for itself if it can
		# * AI votes for a team member if it can
		# * AI votes for its master, if it is a vassal
		# * if the AI attitude to one of the candidates is 'friendly' and the other is 'pleased' or less, AI votes for 'friend'
		# * if both candidates are at 'friendly' status, votes for one with highest attitude
		# * if neither candidate is at 'friendly', abstains

		iPTeam = GC.getPlayer(iPlayer).getTeam()
		iPCand1 = self.getPlayerOnTeam(iCand1)
		iPCand2 = self.getPlayerOnTeam(iCand2)

		# * player votes for its own team if it can
		if iPTeam == iCand1: return 1
		if iPTeam == iCand2: return 2

		# if player is human, votes for self or abstains
		if iPlayer == self.iActivePlayer: return -1

		# * AI votes for its master, if it is a vassal
		if GC.getTeam(iPTeam).isVassal(iCand1): return 1
		if GC.getTeam(iPTeam).isVassal(iCand2): return 2

		# get player category (friendly) to candidates
		iC1Cat = AttitudeUtil.getAttitudeCategory(iPlayer, iPCand1)
		iC2Cat = AttitudeUtil.getAttitudeCategory(iPlayer, iPCand2)

		# the cut-off for SecGen votes is pleased (3)
		# the cut-off for Diplo victory votes is friendly (4)
		if iVote == 1:  # vote for SecGen or Pope
			iCutOff = 3
		else:
			iCutOff = 4

		# * if neither candidate is at 'friendly', abstains
		# assumes friendly = 4, pleased = 3, etc
		if (iC1Cat < iCutOff
		and iC2Cat < iCutOff):
			return -1

		# * if the AI attitude to one of the candidates is 'friendly' and the other is 'pleased' or less, AI votes for 'friend'
		if (iC1Cat >= iCutOff
		and iC1Cat > iC2Cat):
			return 1

		if (iC2Cat >= iCutOff
		and iC2Cat > iC1Cat):
			return 2

		# if the code gets to here, then both candidates are at or above the cutoff
		# and they are both at the same category (ie both friendly)
		# need to decide on straight attitude (visible only)

		# get player attitude to candidates
		iC1Att = AttitudeUtil.getAttitudeCount(iPlayer, iPCand1)
		iC2Att = AttitudeUtil.getAttitudeCount(iPlayer, iPCand2)

		# * if both candidates are at 'friendly' status, votes for one with highest attitude
		if iC2Att < iC1Att: # ties go to Candidate #1
			return 1
		else:
			return 2

		return -1

	def getPlayerOnTeam(self, iTeam):
		for i in xrange(GC.getMAX_PC_PLAYERS()):
			if iTeam == GC.getPlayer(i).getTeam():
				return i

		return -1

	def getAP_UN_OwnerTeam(self):
		for i in xrange(GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(i)
			for j in xrange(GC.getNumVoteSourceInfos()):
				if CvBuildingInfo.getVoteSourceType() == j:
					for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
						CyPlayerX = GC.getPlayer(iPlayerX)
						if CyPlayerX.isAlive() and not CyPlayerX.isMinorCiv() and CyPlayerX.hasBuilding(i):
							return CyPlayerX.getTeam()
		return -1


	def isApolloBuilt(self):
		iTeam = GC.getPlayer(self.iActivePlayer).getTeam()
		CyTeam = GC.getTeam(iTeam)

		# check if anyone has built the apollo project (PROJECT_APOLLO_PROGRAM)
		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
			CyTeamX = GC.getTeam(iTeamX)
			if CyTeamX.isAlive() and not CyTeamX.isMinorCiv():
				if iTeamX == iTeam:
					bContact = True
				elif CyTeam.isHasMet(iTeamX) or GAME.isDebugMode():
					bContact = True
				else:
					bContact = False

				if bContact and self.isApolloBuiltbyTeam(CyTeamX):
					return True
		return False

	def isApolloBuiltbyTeam(self, CyTeam):
		iTeam = CyTeam.getID()

		iNumProjectInfos = GC.getNumProjectInfos()
		for i in xrange(iNumProjectInfos):
			CvProjectInfo = GC.getProjectInfo(i)
			if CvProjectInfo.isSpaceship():
				for j in xrange(iNumProjectInfos):
					if CyTeam.getProjectCount(j) < CvProjectInfo.getProjectsNeeded(j):
						break
				else:
					self.ApolloTeamCheckResult[iTeam] = True
					self.ApolloTeamsChecked.add(iTeam)
					return True
				break

		self.ApolloTeamCheckResult[iTeam] = False
		self.ApolloTeamsChecked.add(iTeam)
		return False


	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = "VictoryScreenWidget" + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def deleteAllWidgets(self):
		screen = self.getScreen()
		i = self.nWidgetCount - 1
		while (i >= 0):
			self.nWidgetCount = i
			screen.deleteWidget(self.getNextWidgetName())
			i -= 1

		self.nWidgetCount = 0

		screen.deleteWidget("BUGVotePope_Widget")
		screen.deleteWidget("BUGVoteDiplomaticVictory_Widget")
		screen.deleteWidget("VoteAP")
		screen.deleteWidget("VoteUN")

	# handle the input for this screen...
	def handleInput(self, inputClass):
		screen = self.getScreen()

		iCode	= inputClass.eNotifyCode
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName

		if iCode == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:
			if NAME == "VictoryScreenDropdownWidget":
				self.iActivePlayer = screen.getPullDownData(NAME, screen.getSelectedPullDownID(NAME))
				self.iTab = 0
				self.showVictoryConditionScreen(screen)
		elif iCode == NotifyCode.NOTIFY_CLICKED:
			if NAME == "VS_Tab":
				screen.hide("VS_Col_Tab" + str(self.iTab))
				screen.show("VS_Tab" + str(self.iTab))
				screen.hide("VS_Tab" + str(ID))
				screen.show("VS_Col_Tab" + str(ID))
				self.iTab = ID
				if ID == 0:
					self.showVictoryConditionScreen(screen)
				elif ID == 1:
					self.showGameSettingsScreen(screen)
				elif ID == 2:
					self.showVotingScreen(screen)
				elif ID == 3:
					self.showMembersScreen(screen)

			elif NAME == "BUGVotePope_Widget" and self.VoteType == 2:
				self.VoteType = 1
				self.iTab = 3
				self.showMembersScreen(screen)

			elif NAME == "BUGVoteDiplomaticVictory_Widget" and self.VoteType == 1:
				self.VoteType = 2
				self.iTab = 3
				self.showMembersScreen(screen)

			elif NAME == "VoteAP" and self.VoteBody == 2:
				self.VoteBody = 1
				self.iTab = 3
				self.showMembersScreen(screen)

			elif NAME == "VoteUN" and self.VoteBody == 1:
				self.VoteBody = 2
				self.iTab = 3
				self.showMembersScreen(screen)

			elif inputClass.getData1() == 1234:
				#close screen
				screen.setDying(True)
				CyInterface().clearSelectedCities()

				#popup spaceship screen
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
				popupInfo.setData1(-1)
				popupInfo.setText(u"showSpaceShip")
				popupInfo.addPopup(self.iActivePlayer)

	def update(self, fDelta): return