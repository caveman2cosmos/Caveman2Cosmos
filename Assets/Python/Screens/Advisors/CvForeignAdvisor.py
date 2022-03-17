## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005

from CvPythonExtensions import *
import CvScreensInterface as UP
import HandleInputUtil
import math

# globals
GC = CyGlobalContext()
AFM = CyArtFileMgr()
GTM = CyGameTextMgr()
TRNSLTR = CyTranslator()
GAME = GC.getGame()

# this class is shared by both the resource and technology foreign advisors
class CvForeignAdvisor:
	"Exotic Foreign Advisor Screen"

	def __init__(self, screenId):
		self.screenId = screenId
		self.iTab = 0
		self.iBonusList = 0

	def getScreen(self):
		return CyGInterfaceScreen("ForeignAdvisor", self.screenId)

	def interfaceScreen(self):
		screen = self.getScreen()
		if screen.isActive():
			return
		self.bRandomPers				 = GAME.isOption(GameOptionTypes.GAMEOPTION_RANDOM_PERSONALITIES)
		self.bNoTechTrade = bNoTechTrade = GAME.isOption(GameOptionTypes.GAMEOPTION_NO_TECH_TRADING)
		# Cache Bonus categories
		BONUSCLASS_CULTURE = GC.getInfoTypeForString("BONUSCLASS_CULTURE")
		aBonusList0 = []
		aBonusList1 = []
		aBonusList2 = []
		for iBonus in xrange(GC.getNumBonusInfos()):
			CvBonusInfo = GC.getBonusInfo(iBonus)
			if CvBonusInfo.getConstAppearance() > 0: # Map resource
				aBonusList0.append(iBonus)
			elif CvBonusInfo.getBonusClassType() != BONUSCLASS_CULTURE:
				aBonusList1.append(iBonus)
			else:
				aBonusList2.append(iBonus)
		self.aBonusTuple = [aBonusList0, aBonusList1, aBonusList2]
		self.aColMap = aColMap = {
			"WHITE"			: GC.getCOLOR_WHITE(),
			"YELLOW"		: GC.getCOLOR_YELLOW(),
			"RED"			: GC.getCOLOR_RED(),
			"GREEN"			: GC.getCOLOR_GREEN(),
			"BLUE"			: GC.getCOLOR_BLUE(),
			"CYAN"			: GC.getInfoTypeForString("COLOR_CYAN"),
			"CITY_GREEN"	: GC.getInfoTypeForString("COLOR_CITY_GREEN")
		}
		# Cache special font characters
		self.charCommerce	= unichr(8485)
		self.charGold		= unichr(8500)
		self.charWar		= unichr(8525)
		self.charPeace		= unichr(8526)
		self.charFaith		= unichr(8857)
		self.charTrade		= unichr(8860)
		self.aSmileyList = [unichr(8876), unichr(8877), unichr(8878), unichr(8879), unichr(8880)]
		self.charCross = unichr(8881)

		# Cache misc:
		self.selectedLeaders = []
		self.aRelationList = []
		self.aDealMap = {}
		self.techsToGive = []

		self.HILITE_SQUARE = AFM.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath()

		import InputData
		self.InputData = InputData.instance

		import PythonToolTip
		self.tooltip = PythonToolTip.PythonToolTip()

		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y

		if yRes > 1000:
			self.H_EDGE_PANEL = H_EDGE_PANEL = 42
			uFontEdge = "<font=4b>"
		elif yRes > 800:
			self.H_EDGE_PANEL = H_EDGE_PANEL = 34
			uFontEdge = "<font=3b>"
		else:
			self.H_EDGE_PANEL = H_EDGE_PANEL = 30
			uFontEdge = "<font=2b>"

		if xRes > 1700:
			self.iResID = 2
			self.aFontList = aFontList = [uFontEdge, "<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>"]
		elif xRes > 1400:
			self.iResID = 1
			self.aFontList = aFontList = [uFontEdge, "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>"]
		else:
			self.iResID = 0
			self.aFontList = aFontList = [uFontEdge, "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0b>", "<font=0>"]

		Y_BOT_TEXT = yRes - H_EDGE_PANEL + 8

		self.xMid = xRes / 2
		self.yMid = yRes / 2

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT
		self.nWidgetCount = 0

		# Base Screen
		screen.setRenderInterfaceOnly(True)
		screen.showWindowBackground(False)
		screen.setDimensions(0, 0, xRes, yRes)
		screen.addDDSGFC("FA_BG", AFM.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, xRes, yRes, eWidGen, 1, 1)
		screen.addPanel("TopPanel", "", "", True, False, 0, 0, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("BottomPanel", "", "", True, False, 0, yRes - H_EDGE_PANEL, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_BOTTOMBAR)

		stTxt = uFontEdge + TRNSLTR.getText("TXT_WORD_EXIT", ())
		screen.setText("ForeignAdvExit", "", stTxt, 1<<1, xRes - 8, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)
		stTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_TITLE", ())
		screen.setLabel("ForeignAdvHeader", "", stTxt, 1<<2, xRes / 2, 2, 0, eFontTitle, eWidGen, 1, 1)

		# Tabs
		iCol = aColMap["YELLOW"]
		stTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_RELATIONS", ())
		stTxt2 = TRNSLTR.changeTextColor(stTxt, iCol)
		if bNoTechTrade:
			dX = xRes / 5
		else:
			dX = xRes / 6
		x = dX / 2
		screen.setText("FA_Tab0", "", stTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("FA_Tab|Col0", "", stTxt2, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("FA_Tab|Col0")

		stTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_GLANCE", ())
		stTxt2 = TRNSLTR.changeTextColor(stTxt, iCol)
		x += dX
		screen.setText("FA_Tab1", "", stTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("FA_Tab|Col1", "", stTxt2, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("FA_Tab|Col1")

		stTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_ACTIVE", ())
		stTxt2 = TRNSLTR.changeTextColor(stTxt, iCol)
		x += dX
		screen.setText("FA_Tab2", "", stTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("FA_Tab|Col2", "", stTxt2, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("FA_Tab|Col2")

		stTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_RESOURCES", ())
		stTxt2 = TRNSLTR.changeTextColor(stTxt, iCol)
		x += dX
		screen.setText("FA_Tab3", "", stTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("FA_Tab|Col3", "", stTxt2, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("FA_Tab|Col3")

		stTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_INFO", ())
		stTxt2 = TRNSLTR.changeTextColor(stTxt, iCol)
		x += dX
		screen.setText("FA_Tab4", "", stTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("FA_Tab|Col4", "", stTxt2, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("FA_Tab|Col4")

		if not bNoTechTrade:
			stTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_TECHS", ())
			stTxt2 = TRNSLTR.changeTextColor(stTxt, iCol)
			x += dX
			screen.setText("FA_Tab5", "", stTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
			screen.setText("FA_Tab|Col5", "", stTxt2, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
			screen.hide("FA_Tab|Col5")

		self.iPlayer = iPlayer = GAME.getActivePlayer()
		self.CyPlayer = GC.getActivePlayer()
		self.iTeam = iTeam = GAME.getActiveTeam()
		self.CyTeam = GC.getTeam(iTeam)

		import DebugUtils
		if DebugUtils.isAnyDebugMode():
			DDB = "FA_DebugDD"
			screen.addDropDownBoxGFC(DDB, 22, 0, 300, eWidGen, 1, 1, FontTypes.GAME_FONT)
			for iPlayerX in range(GC.getMAX_PC_PLAYERS()):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					screen.addPullDownString(DDB, CyPlayerX.getName(), iPlayerX, iPlayerX, iPlayerX == iPlayer)

		self.calcPlayerRelative()
		self.drawContents(screen)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

	def calcPlayerRelative(self):
		iPlayer = self.iPlayer
		CyPlayer = self.CyPlayer
		iTeam = self.iTeam
		CyTeam = self.CyTeam
		aList = []
		for iPlayerX in range(GC.getMAX_PC_PLAYERS()):
			if iPlayerX == iPlayer: continue
			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isAlive() or CyPlayerX.isMinorCiv(): continue
			iTeamX = CyPlayerX.getTeam()
			if iTeamX != iTeam and not CyTeam.isHasMet(iTeamX): continue
			aList.append(iPlayerX)
		self.hasMet = aList

		# City trade list
		self.tuCity = tuCity = []
		for CyCity in CyPlayer.cities():
			liCity = []
			for iCity in xrange(CyCity.getTradeRoutes()):
				cityTrade = CyCity.getTradeCity(iCity)
				if cityTrade:
					liCity.append(cityTrade)
				else: print "[WARNING] Mismatch! 'CyCity.getTradeCity(iCity) = None' in 'for iCity in range(CyCity.getTradeRoutes()):'"
			if liCity:
				tuCity.append([CyCity, liCity])

		self.techsToGive = techsToGive = []
		self.techsToTake = techsToTake = []
		if not self.bNoTechTrade:
			for iTech in range(GC.getNumTechInfos()):
				if GC.getTechInfo(iTech).isGlobal():
					continue
				if CyTeam.isHasTech(iTech):
					if not CyTeam.isNoTradeTech(iTech):
						techsToGive.append(iTech)
				elif CyPlayer.canResearch(iTech):
					techsToTake.append(iTech)


	# Drawing Leaderheads
	def drawContents(self, screen):
		self.deleteAllWidgets()
		if self.iTab == 0:
			screen.hide("FA_Tab0")
			screen.show("FA_Tab|Col0")
			self.drawRelations(screen)
		elif self.iTab == 1:
			screen.hide("FA_Tab1")
			screen.show("FA_Tab|Col1")
			self.drawGlance(screen)
		elif self.iTab == 2:
			screen.hide("FA_Tab2")
			screen.show("FA_Tab|Col2")
			self.drawActive(screen)
		elif self.iTab == 3:
			screen.hide("FA_Tab3")
			screen.show("FA_Tab|Col3")
			self.drawResourceDeals(screen)
		elif self.iTab == 4:
			screen.hide("FA_Tab4")
			screen.show("FA_Tab|Col4")
			self.drawInfo(screen)
		elif self.iTab == 5:
			screen.hide("FA_Tab5")
			screen.show("FA_Tab|Col5")
			self.drawTechDeals(screen)


	def drawRelations(self, screen):
		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2 = self.aFontList
		aColMap = self.aColMap

		iPlayer = self.iPlayer
		CyPlayer = self.CyPlayer
		iTeam = self.iTeam
		CyTeam = self.CyTeam

		# count the leaders
		hasMet = self.hasMet
		iCount = len(hasMet)

		if iCount < 7:
			iSize = 96
		elif iCount < 10:
			iSize = 64
		else:
			iSize = 48
		iSizeHalf = iSize/2

		xMid = self.xMid
		yMid = self.yMid
		fRadius = yMid - 128
		dRadius = xMid - fRadius - 64
		# Other leaderheads will follow a full circle
		if iCount:
			dTheta = 2*math.pi / iCount

		eWidGen		= WidgetTypes.WIDGET_GENERAL
		eWidLeader	= WidgetTypes.WIDGET_LEADERHEAD
		eFontGame	= FontTypes.GAME_FONT
		eBtnLbl		= ButtonStyles.BUTTON_STYLE_LABEL
		# legend
		DrawCtrl = self.getNextWidget()
		screen.addDrawControl(DrawCtrl, "", 0, 0, self.xRes, self.yRes, eWidGen, 1, 1)
		screen.moveToBack(DrawCtrl)
		screen.moveToBack("FA_BG")
		y = self.H_EDGE_PANEL + 16
		w = 156
		screen.addPanel(self.getNextWidget(), "", "", True, False, 8, y, w, w, PanelStyles.PANEL_STYLE_IN)
		x = 16
		y += 8
		szTxt = TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_CONTACT", ())
		screen.setLabel(self.getNextWidget(), "", szTxt, 1<<0, x, y, 0, eFontGame, eWidGen, 1, 1)
		y += 16
		screen.addLineGFC(DrawCtrl, self.getNextWidget(), x, y, x + w - 16, y, aColMap["WHITE"])
		y += 8
		szTxt = TRNSLTR.getText("TXT_KEY_CONCEPT_WAR", ())
		screen.setLabel(self.getNextWidget(), "", szTxt, 1<<0, x, y, 0, eFontGame, eWidGen, 1, 1)
		y += 16
		screen.addLineGFC(DrawCtrl, self.getNextWidget(), x, y, x + w - 16, y, aColMap["RED"])
		y += 8
		szTxt = TRNSLTR.getText("TXT_KEY_TRADE_DEFENSIVE_PACT_STRING", ())
		screen.setLabel(self.getNextWidget(), "", szTxt, 1<<0, x, y, 0, eFontGame, eWidGen, 1, 1)
		y += 16
		screen.addLineGFC(DrawCtrl, self.getNextWidget(), x, y, x + w - 16, y, aColMap["BLUE"])
		y += 8
		szTxt = TRNSLTR.getText("TXT_KEY_TRADE_OPEN_BORDERS_STRING", ())
		screen.setLabel(self.getNextWidget(), "", szTxt, 1<<0, x, y, 0, eFontGame, eWidGen, 1, 1)
		y += 16
		screen.addLineGFC(DrawCtrl, self.getNextWidget(), x, y, x + w - 16, y, aColMap["CITY_GREEN"])
		y += 8
		szTxt = TRNSLTR.getText("TXT_KEY_PITBOSS_TEAM", ())
		screen.setLabel(self.getNextWidget(), "", szTxt, 1<<0, x, y, 0, eFontGame, eWidGen, 1, 1)
		y += 16
		screen.addLineGFC(DrawCtrl, self.getNextWidget(), x, y, x + w - 16, y, aColMap["YELLOW"])
		y += 8
		szVassal = TRNSLTR.getText("TXT_KEY_MISC_VASSAL_SHORT", ())
		szMaster = TRNSLTR.getText("TXT_KEY_MISC_MASTER", ())
		screen.setLabel(self.getNextWidget(), "", szVassal, 1<<0, x, y, 0, eFontGame, eWidGen, 1, 1)
		y += 16
		screen.addLineGFC(DrawCtrl, self.getNextWidget(), x, y, x + w - 16, y, aColMap["CYAN"])

		selectedLeaders = self.selectedLeaders
		if selectedLeaders and selectedLeaders[0] != iPlayer:
			# attitudes shown are towards single selected leader
			iLeaderBase = selectedLeaders[0]
			CyPlayerBase = GC.getPlayer(iLeaderBase)
			iTeamBase = CyPlayerBase.getTeam()
			CyTeamBase = GC.getTeam(iTeamBase)
		else:
			# attitudes shown are towards active leader
			iLeaderBase = iPlayer
			CyPlayerBase = CyPlayer
			iTeamBase = iTeam
			CyTeamBase = CyTeam

		hiLi = self.HILITE_SQUARE
		# draw lines
		aList1 = []
		fTheta0 = 0
		for iPlayerX in [iPlayer] + hasMet:
			# get selected player and location
			if iPlayerX == iPlayer:
				fX0 = xMid
				fX = fX0 - iSizeHalf
				fY = yMid - iSizeHalf

				# Our leader head
				chBox = self.getNextWidget()
				BTN = GC.getLeaderHeadInfo(CyPlayer.getLeaderType()).getButton()
				screen.addCheckBoxGFC(chBox, BTN, hiLi, fX, fY, iSize, iSize, eWidLeader, iPlayer, -1, eBtnLbl)
				screen.setState(chBox, iPlayer in selectedLeaders)
				screen.setLabel(self.getNextWidget(), "", uFont3 + CyPlayer.getName(), 1<<2, fX0, fY + iSize + 4, 0, eFontGame, eWidGen, 1, 1)
				if not selectedLeaders: continue
				fY0 = yMid
				CyPlayerX = CyPlayer
				iTeamX = iTeam
				CyTeamX = CyTeam
			else:
				CyPlayerX = GC.getPlayer(iPlayerX)
				iTeamX = CyPlayerX.getTeam()
				CyTeamX = GC.getTeam(iTeamX)
				# Oval
				fTheta0 += dTheta
				fRadiusOval = fRadius + dRadius * abs(math.cos(fTheta0))
				fX0 = xMid - fRadiusOval * math.cos(0.4 + fTheta0)
				fY0 = yMid - 32 + fRadius * math.sin(0.4 + fTheta0)

				fX = int(fX0) - iSizeHalf
				fY = int(fY0) - iSizeHalf

				chBox = self.getNextWidget()
				BTN = GC.getLeaderHeadInfo(CyPlayerX.getLeaderType()).getButton()
				screen.addCheckBoxGFC(chBox, BTN, hiLi, fX, fY, iSize, iSize, eWidLeader, iPlayerX, iLeaderBase, eBtnLbl)
				screen.setState(chBox, iPlayerX in selectedLeaders)
				screen.setLabel(self.getNextWidget(), "", uFont3 + CyPlayerX.getName(), 1<<2, int(fX0), fY + iSize + 4, 0, eFontGame, eWidGen, 1, 1)

				# Leader attitude towards base leader
				if iLeaderBase != iPlayerX and CyTeamX.isHasMet(iTeamBase):
					szTxt = GC.getAttitudeInfo(CyPlayerX.AI_getAttitude(iLeaderBase)).getDescription()
					if CyTeamX.isVassal(iTeamBase):
						szTxt += ", " + szVassal
					elif CyTeamBase.isVassal(iTeamX):
						szTxt += ", " + szMaster
					screen.setLabel(self.getNextWidget(), "", szTxt, 1<<2, fX + iSizeHalf, fY + iSize + 25, 0, eFontGame, eWidGen, 1, 1)

			if selectedLeaders and iPlayerX not in selectedLeaders: continue

			aList1.append(iPlayerX)
			fTheta1 = 0
			for iPlayerY in hasMet:
				fTheta1 += dTheta
				if iPlayerY in aList1: continue
				#make oval
				fRadiusOval = fRadius + dRadius * abs(math.cos(fTheta1))
				fX1 = xMid - fRadiusOval * math.cos(0.4 + fTheta1)

				iTeamY = GC.getPlayer(iPlayerY).getTeam()
				CyTeamY = GC.getTeam(iTeamY)

				# draw lines
				fY1 = yMid - 32 + fRadius * math.sin(0.4 + fTheta1)
				if iTeamY == iTeamX:
					szName = self.getNextWidget()
					screen.addLineGFC(DrawCtrl, szName, int(fX0), int(fY0), int(fX1), int(fY1), aColMap["YELLOW"])
				elif CyTeamY.isVassal(iTeamX) or CyTeamX.isVassal(iTeamY):
					szName = self.getNextWidget()
					screen.addLineGFC(DrawCtrl, szName, int(fX0), int(fY0), int(fX1), int(fY1), aColMap["CYAN"])
				elif CyTeamY.isHasMet(iTeamX):
					if CyTeamY.isAtWarWith(iTeamX):
						szName = self.getNextWidget()
						screen.addLineGFC(DrawCtrl, szName, int(fX0), int(fY0), int(fX1), int(fY1), aColMap["RED"])
					else:
						bJustPeace = True
						if CyTeamY.isOpenBorders(iTeamX):
							fDy = abs(fY0 - fY1) + 0.01
							fDx = abs(fX0 - fX1) + 0.01
							if fDy >= fDx:
								yOff = 6 * fDx / fDy
								xOff = 6 - yOff
							else:
								xOff = 6 * fDy / fDx
								yOff = 6 - xOff
							szName = self.getNextWidget()
							x0 = int(fX0 + xOff)
							y0 = int(fY0 + yOff)
							screen.addLineGFC(DrawCtrl, szName, x0, y0, int(fX1 + xOff), int(fY1 + yOff), aColMap["CITY_GREEN"])
							bJustPeace = False
						if CyTeamY.isDefensivePact(iTeamX):
							szName = self.getNextWidget()
							screen.addLineGFC(DrawCtrl, szName, int(fX0), int(fY0), int(fX1), int(fY1), aColMap["BLUE"])
							bJustPeace = False
						if bJustPeace:
							szName = self.getNextWidget()
							screen.addLineGFC(DrawCtrl, szName, int(fX0), int(fY0), int(fX1), int(fY1), aColMap["WHITE"])


	def drawGlance(self, screen):
		iPlayer = self.iPlayer
		H_EDGE = self.H_EDGE_PANEL
		hasMet = self.hasMet
		self.aRelationList = []

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT
		ePnlHelpHUD = PanelStyles.PANEL_STYLE_HUD_HELP
		# Main Area
		ScPnl = self.getNextWidget()
		h = self.yRes - 2*H_EDGE - 8
		screen.addScrollPanel(ScPnl, "", -6, H_EDGE - 8, self.xRes + 8, h, ePnlHelpHUD)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")
		# Fill Area
		aSmileyList = self.aSmileyList
		y = 0
		dy = 76
		dx = 50
		w = 64 + dx*(len(hasMet))
		n = 0
		iRelation = 0
		for iPlayerX in [iPlayer] + hasMet:
			Pnl = self.getNextWidget()
			screen.attachPanelAt(ScPnl, Pnl, "", "", False, True, ePnlHelpHUD, 0, y, w, 76, eWidGen, 1, 1)
			if iPlayerX == iPlayer:
				CyPlayerX = self.CyPlayer
				CyTeamX = self.CyTeam
				w -= dx
			else:
				CyPlayerX = GC.getPlayer(iPlayerX)
				CyTeamX = GC.getTeam(CyPlayerX.getTeam())
			BTN = GC.getLeaderHeadInfo(CyPlayerX.getLeaderType()).getButton()
			screen.setImageButtonAt("WID|LEADER" + str(iPlayerX) + "|" + str(n), Pnl, BTN, 0, 8, 64, 64, eWidGen, 1, 1)
			n += 1

			aList = []
			for iPlayerY in hasMet:
				if iPlayerY == iPlayerX: continue
				iSum = self.sumAttitude(iPlayerY, iPlayerX)[0]
				aList.append([iSum, iPlayerY])
			aList.sort(lambda a, b:cmp(b, a))
			x = 68
			for iSum, iPlayerY in aList:
				CyPlayerY = GC.getPlayer(iPlayerY)
				iTeamY = CyPlayerY.getTeam()

				BTN = GC.getLeaderHeadInfo(CyPlayerY.getLeaderType()).getButton()
				screen.setImageButtonAt("WID|LEADER" + str(iPlayerY) + "|" + str(n), Pnl, BTN, x, 2, 44, 44, eWidGen, 1, 1)
				n += 1

				iOff = 14
				szTxt = aSmileyList[CyPlayerY.AI_getAttitude(iPlayerX)]
				if CyTeamX.isAtWarWith(iTeamY):
					szTxt += self.charWar
					iOff -= 10
					if not (CyPlayerY.isHuman() or CyPlayerY.AI_isWillingToTalk(iPlayerX)):
						szTxt += "!"
						iOff -= 4
				elif CyTeamX.isForcePeace(iTeamY):
					szTxt += self.charPeace
					iOff -= 10

				self.aRelationList.append([iPlayerY, iPlayerX])
				screen.setTextAt("WID|RELATION" + str(iRelation), Pnl, szTxt, 1<<0, x + iOff, 48, 0, eFontGame, eWidGen, 1, 1)
				iRelation += 1
				x += dx
			y += dy

	def drawActive(self, screen):
		xRes = self.xRes
		iPlayer = self.iPlayer
		uFont2 = self.aFontList[6]
		H_EDGE = self.H_EDGE_PANEL

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT
		ePnlHelpHUD = PanelStyles.PANEL_STYLE_HUD_HELP
		# Main Area
		ScPnl = self.getNextWidget()
		h = self.yRes - 2*H_EDGE - 8
		screen.addScrollPanel(ScPnl, "", -6, H_EDGE - 8, xRes + 8, h, ePnlHelpHUD)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")
		# loop through all deals and map the number of deals other players have with the active player.
		aList = []
		aMap = {}
		for i in self.hasMet:
			print GTM.getActiveDealsString(i, iPlayer)
		for i in range(GAME.getIndexAfterLastDeal()):
			CyDeal = GAME.getDeal(i)
			iPlayerA = CyDeal.getFirstPlayer()
			iPlayerB = CyDeal.getSecondPlayer()
			if iPlayer not in (iPlayerA, iPlayerB): continue
			if iPlayerA != iPlayer:
				iPlayerX = iPlayerA
			else:
				iPlayerX = iPlayerB
			if iPlayerX not in aMap:
				aMap[iPlayerX] = [1, [CyDeal], iPlayerX]
			else:
				aMap[iPlayerX][0] += 1
				aMap[iPlayerX][1].append(CyDeal)
		# make a list sorted by declining amount of deals.
		aList = aMap.values()
		aList.sort(lambda a, b:cmp(b, a))
		# Fill the area.
		y0 = 0
		dy = 20
		w = xRes - 12
		for iDeals, deals, iPlayerX in aList:
			h = iDeals * dy + 8
			if h < 72:
				h = 72
			yImg = y0 + h / 2 - 32
			screen.attachPanelAt(ScPnl, self.getNextWidget(), "", "", False, True, ePnlHelpHUD, 0, y0, w, h, eWidGen, 1, 1)
			BTN = GC.getLeaderHeadInfo(GC.getPlayer(iPlayerX).getLeaderType()).getButton()
			screen.setImageButtonAt("WID|LEADER" + str(iPlayerX), ScPnl, BTN, 8, yImg, 64, 64, eWidGen, 1, 1)
			y1 = y0 + 4
			for CyDeal in deals:
				szTxt = uFont2 + GTM.getDealString(CyDeal, iPlayerX)
				iTurns = CyDeal.turnsToCancel(iPlayer)
				if iTurns > 0:
					szTxt += " (%d)" %iTurns

				screen.setTextAt(self.getNextWidget(), ScPnl, szTxt, 1<<0, 74, y1, 0, eFontGame, WidgetTypes.WIDGET_DEAL_KILL, CyDeal.getID(), -1)
				y1 += dy
			y0 += h + 8


	def drawResourceDeals(self, screen):
		xRes = self.xRes
		yRes = self.yRes
		iPlayer = self.iPlayer
		CyPlayer = self.CyPlayer
		CyTeam = self.CyTeam
		bGoldTrade = CyTeam.isGoldTrading()
		uFont4b = self.aFontList[1]
		yTop = self.H_EDGE_PANEL

		eFontGame = FontTypes.GAME_FONT
		eWidGen			= WidgetTypes.WIDGET_GENERAL
		eWidJuToBonus	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS

		ePnlHelpHUD	= PanelStyles.PANEL_STYLE_HUD_HELP
		ePnlBlue50	= PanelStyles.PANEL_STYLE_BLUE50
		ePnlSTD		= PanelStyles.PANEL_STYLE_STANDARD
		ePnlEmpty	= PanelStyles.PANEL_STYLE_EMPTY

		# Control buttons
		dy = yTop - 12
		x = 3
		dx = x+dy
		Pnl = self.getNextWidget()
		screen.addPanel(Pnl, "", "", True, True, 8, -2, 3+3*dx, yTop + 4, ePnlEmpty)

		szPre = "Art/Interface/screens/City/"
		screen.setImageButtonAt("BonusList0", Pnl, szPre + "MapBonus.dds", x, 3, dy, dy, eWidGen, 1, 1)
		x += dx
		screen.setImageButtonAt("BonusList1", Pnl, szPre + "Manufacture.dds", x, 3, dy, dy, eWidGen, 1, 1)
		x += dx
		screen.setImageButtonAt("BonusList2", Pnl, szPre + "Cultures.dds", x, 3, dy, dy, eWidGen, 1, 1)

		i = self.iBonusList
		screen.setImageButtonAt("BonusListHiLi", Pnl, self.HILITE_SQUARE, i*dx, 0, dy + 6, dy + 6, eWidGen, 1, 1)

		aBonusList = self.aBonusTuple[i]

		# Main Area
		h0 = yRes - 2*yTop
		w0 = xRes - 16

		PnlBG = self.getNextWidget()
		screen.addPanel(PnlBG, "", "", True, True, 8, yTop, w0, h0, ePnlBlue50)
		aList0 = []
		w = w0 - 8
		Pnl = self.getNextWidget()
		screen.addPanel(Pnl, "", "", True, True, 10, yTop + 6, w, 72, ePnlSTD)
		BTN = GC.getLeaderHeadInfo(CyPlayer.getLeaderType()).getButton()
		screen.setImageButtonAt("WID|LEADER" + str(iPlayer), Pnl, BTN, 2, 4, 64, 64, eWidGen, 1, 1)

		ourBonuses = []
		for iBonus in aBonusList:
			iNum = CyPlayer.getNumAvailableBonuses(iBonus)
			if iNum:
				if iNum - CyPlayer.getBonusImport(iBonus):
					ourBonuses.append(iBonus)
				continue
			CvBonus = GC.getBonusInfo(iBonus)
			if CyTeam.isHasTech(CvBonus.getTechCityTrade()):
				aList0.append([iBonus, CvBonus])
		iBon = 0
		w1 = w0 - 72
		if aList0:
			ScPnl = self.getNextWidget()
			screen.addScrollPanel(ScPnl, "", 76, yTop, w1, 54, ePnlEmpty)
			screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")
			iPerRow = (w1 - 16) / 40
			for i, entry in enumerate(aList0):
				iBonus, CvBonus = entry
				if not i % iPerRow:
					x = 0
					if i:
						y += 40
					else:
						y = 0
				BTN = CvBonus.getButton()
				screen.setImageButtonAt("WID|BONUS|%d|%d" %(iBonus, iBon), ScPnl, BTN, x, y, 40, 40, eWidGen, 1, 1)
				iBon += 1
				x += 40
		szTxt = TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_EXPORT", ())
		szEx1 = "<color=255,255,0>" + szTxt
		szEx2 = "<color=0,255,255>" + szTxt
		szTxt = TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_IMPORT", ())
		szIm1 = "<color=255,255,0>" + szTxt
		szIm2 = "<color=0,255,255>" + szTxt
		w1 = w1/4 + 2
		screen.setLabelAt("", PnlBG, szEx1, 1<<0,		 88, 78, 0, eFontGame, eWidGen, 1, 1)
		szTxt = TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_FOR_TRADE_2", ())
		screen.setLabelAt("", PnlBG, szTxt, 1<<2, w1/2 + 92, 78, 0, eFontGame, eWidGen, 1, 1)
		screen.setLabelAt("", PnlBG, szIm1, 1<<0,	w1 + 88, 78, 0, eFontGame, eWidGen, 1, 1)
		screen.setLabelAt("", PnlBG, szEx2, 1<<0, 2*w1 + 88, 78, 0, eFontGame, eWidGen, 1, 1)
		szTxt = TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_ACTIVE", ())
		screen.setLabelAt("", PnlBG, szTxt, 1<<2, 5*w1/2+92, 78, 0, eFontGame, eWidGen, 1, 1)
		screen.setLabelAt("", PnlBG, szIm2, 1<<0, 3*w1 + 88, 78, 0, eFontGame, eWidGen, 1, 1)

		ScPnl = self.getNextWidget()
		screen.addScrollPanel(ScPnl, "", -6, yTop + 100, xRes + 12, h0 - 128, ePnlEmpty)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")

		# Fill Area
		szGoldIcon = uFont4b + self.charGold
		szNotConnected = "<font=3>" + TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_NOT_CONNECTED", ())
		TradeData1 = TradeData()
		TradeData1.ItemType = TradeableItems.TRADE_RESOURCES

		eTradeGold = TradeableItems.TRADE_GOLD_PER_TURN
		eTradeBonus = TradeableItems.TRADE_RESOURCES
		iNumDeals = GAME.getIndexAfterLastDeal()
		iDeal = 0
		self.aDealMap = aDealMap = {}
		w1 = w / 4 - 40
		iPerRow = (w1 - 8) / 40
		x0 = (w1 - iPerRow * 40) / 2
		y = 4
		for iPlayerX in self.hasMet:
			# Define row content
			aList0 = []
			aList1 = []
			aList2 = []
			aList3 = []

			CyPlayerX = GC.getPlayer(iPlayerX)
			iTeamX = CyPlayerX.getTeam()
			bHuman = CyPlayerX.isHuman()

			if CyPlayer.canTradeNetworkWith(iPlayerX):
				msg = ""
			else:
				msg = szNotConnected

			iGold = 0
			iPosGPT = 0
			if not bHuman:
				if bGoldTrade and GC.getTeam(iTeamX).isGoldTrading():
					iPosGPT = CyPlayerX.AI_maxGoldPerTurnTrade(iPlayer)

			if not msg:
				# Import, Export
				if bHuman:
					CyTeamX = GC.getTeam(iTeamX)
					for iBonus in aBonusList:
						iNum = CyPlayerX.getNumAvailableBonuses(iBonus)
						if iNum:
							if iNum - CyPlayerX.getBonusImport(iBonus):
								aList0.append([iBonus, None])
							continue
						if iBonus in ourBonuses:
							CvBonus = GC.getBonusInfo(iBonus)
							if CyTeamX.isHasTech(CvBonus.getTechCityTrade()):
								aList1.append([iBonus, CvBonus])
				else:
					for iBonus in aBonusList:
						TradeData1.iData = iBonus
						if iBonus in ourBonuses:
							if CyPlayer.canTradeItem(iPlayerX, TradeData1, True):
								aList1.append([iBonus, None])
						elif CyPlayerX.canTradeItem(iPlayer, TradeData1, True):
							aList0.append([iBonus, None])
				# Active Deals
				for iDealX in range(iNumDeals):
					CyDeal = GAME.getDeal(iDealX)
					iPlayerA = CyDeal.getFirstPlayer()
					iPlayerB = CyDeal.getSecondPlayer()
					if iPlayer not in (iPlayerA, iPlayerB): continue
					if iPlayerX not in (iPlayerA, iPlayerB): continue

					for iTrade in range(CyDeal.getLengthFirstTrades()):
						TradeData2 = CyDeal.getFirstTrade(iTrade)
						if TradeData2.ItemType == eTradeGold:
							if iPlayerA == iPlayerX:
								iGold += TradeData2.iData
							else:
								iGold -= TradeData2.iData
						elif TradeData2.ItemType == eTradeBonus:
							if iPlayerA == iPlayerX:
								aList2.append([TradeData2.iData, CyDeal])
							else:
								aList3.append([TradeData2.iData, CyDeal])

					for iTrade in range(CyDeal.getLengthSecondTrades()):
						TradeData2 = CyDeal.getSecondTrade(iTrade)
						if TradeData2.ItemType == eTradeGold:
							if iPlayerA == iPlayerX:
								iGold -= TradeData2.iData
							else:
								iGold += TradeData2.iData
							iGold -= TradeData2.iData
						elif TradeData2.ItemType == eTradeBonus:
							if iPlayerA == iPlayerX:
								aList3.append([TradeData2.iData, CyDeal])
							else:
								aList2.append([TradeData2.iData, CyDeal])

			# Define row
			if msg:
				h = 72
				yImg = y + 4
			else:
				iAmount = 0
				for i in [len(aList0), len(aList1), len(aList2), len(aList3)]:
					if i > iAmount:
						iAmount = i
				iRows = (iAmount + iPerRow - 1) / iPerRow
				if iRows > 1:
					h = 40 * iRows + 16
					yImg = y + h / 2 - 32
				else:
					h = 72
					yImg = y + 4

			# Fill row
			screen.attachPanelAt(ScPnl, self.getNextWidget(), "", "", True, True, ePnlSTD, 8, y, w, h, eWidGen, 1, 1)
			h -= 6

			BTN = GC.getLeaderHeadInfo(CyPlayerX.getLeaderType()).getButton()
			screen.setImageButtonAt("WID|LEADER" + str(iPlayerX), ScPnl, BTN, 12, yImg, 64, 64, eWidGen, 1, 1)

			if iPosGPT:
				screen.setTextAt("GPT|WILL%d|%d" %(iPosGPT, iPlayerX), ScPnl, szGoldIcon, 1<<0, 81+w1, yImg+16, 0, eFontGame, eWidGen, 1, 1)
			if iGold:
				if iGold > 0:
					PS = "POS"
				else:
					iGold = -iGold
					PS = "NEG"
				screen.setTextAt("GPT|%s|%d|%d" %(PS, iGold, iPlayerX), ScPnl, szGoldIcon, 1<<0, 133+3*w1, yImg+16, 0, eFontGame, eWidGen, 1, 1)

			if msg:
				screen.setLabelAt(self.getNextWidget(), ScPnl, msg, 1<<0, 80, y+28, 0, eFontGame, eWidGen, 1, 1)
			else:
				Pnl = self.getNextWidget()
				screen.attachPanelAt(ScPnl, Pnl, "", "", True, True, ePnlBlue50, 80, y+1, w1, h, eWidGen, 1, 1)
				screen.setPanelColor(Pnl, 230, 200, 140)
				for i, entry in enumerate(aList0):
					iBonus, CvBonus = entry
					if CvBonus is None:
						CvBonus = GC.getBonusInfo(iBonus)
					if not i % iPerRow:
						x1 = x0
						if i:
							y1 += 40
						else:
							y1 = 3
					BTN = CvBonus.getButton()
					screen.setImageButtonAt("WID|BONUS|%d|%d" %(iBonus, iBon), Pnl, BTN, x1, y1, 40, 40, eWidGen, 1, 1)
					iBon += 1
					x1 += 40
				Pnl = self.getNextWidget()
				screen.attachPanelAt(ScPnl, Pnl, "", "", True, True, ePnlBlue50, 106+w1, y+1, w1, h, eWidGen, 1, 1)
				screen.setPanelColor(Pnl, 230, 200, 140)
				for i, entry in enumerate(aList1):
					iBonus, CvBonus = entry
					if CvBonus is None:
						CvBonus = GC.getBonusInfo(iBonus)
					if not i % iPerRow:
						x1 = x0
						if i:
							y1 += 40
						else:
							y1 = 3
					BTN = CvBonus.getButton()
					screen.setImageButtonAt("WID|BONUS|%d|%d" %(iBonus, iBon), Pnl, BTN, x1, y1, 40, 40, eWidGen, 1, 1)
					iBon += 1
					x1 += 40
				Pnl = self.getNextWidget()
				screen.attachPanelAt(ScPnl, Pnl, "", "", True, True, ePnlBlue50, 132+2*w1, y+1, w1, h, eWidGen, 1, 1)
				screen.setPanelColor(Pnl, 230, 200, 140)
				for i, entry in enumerate(aList2):
					iBonus, CyDeal = entry
					if not i % iPerRow:
						x1 = x0
						if i:
							y1 += 40
						else:
							y1 = 3
					aDealMap[iDeal] = [iBonus, CyDeal]
					BTN = GC.getBonusInfo(iBonus).getButton()
					screen.setImageButtonAt("WID|BONUS|DEAL|%d" %iDeal, Pnl, BTN, x1, y1, 40, 40, eWidGen, 1, 1)
					iDeal += 1
					x1 += 40
				Pnl = self.getNextWidget()
				screen.attachPanelAt(ScPnl, Pnl, "", "", True, True, ePnlBlue50, 158+3*w1, y+1, w1, h, eWidGen, 1, 1)
				screen.setPanelColor(Pnl, 230, 200, 140)
				for i, entry in enumerate(aList3):
					iBonus, CyDeal = entry
					if not i % iPerRow:
						x1 = x0
						if i:
							y1 += 40
						else:
							y1 = 3
					aDealMap[iDeal] = [iBonus, CyDeal]
					BTN = GC.getBonusInfo(iBonus).getButton()
					screen.setImageButtonAt("WID|BONUS|DEAL|%d" %iDeal, Pnl, BTN, x1, y1, 40, 40, eWidGen, 1, 1)
					iDeal += 1
					x1 += 40
			y += h + 8


	def drawInfo(self, screen):
		xRes = self.xRes
		yRes = self.yRes
		H_EDGE = self.H_EDGE_PANEL
		iPlayer = self.iPlayer
		CyPlayer = self.CyPlayer

		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2 = self.aFontList

		if self.iResID == 2:
			iSize0 = 64
		elif self.iResID == 1:
			iSize0 = 56
		else:
			iSize0 = 48
		iSize1 = iSize0 - 8

		self.aRelationList = []

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eWidTrade = WidgetTypes.WIDGET_TRADE_ROUTES
		eFontGame = FontTypes.GAME_FONT
		ePnlEmpty = PanelStyles.PANEL_STYLE_EMPTY
		ePnlOut = PanelStyles.PANEL_STYLE_OUT

		# Background
		h = yRes - 2*H_EDGE + 4
		screen.addPanel(self.getNextWidget(), "", "", True, False, 4, H_EDGE, xRes - 8, h, PanelStyles.PANEL_STYLE_MAIN)
		# Main Area
		ScPnl = self.getNextWidget()
		screen.addScrollPanel(ScPnl, "", 6, H_EDGE + 2, xRes - 14, h - 30, ePnlEmpty)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")

		iCivicOptions = GC.getNumCivicOptionInfos()
		bRandomPers = self.bRandomPers
		aSmileyList = self.aSmileyList
		tuCity = self.tuCity

		dx = iSize0 + 8
		dx1 = 3*dx/2
		dx2 = dx1/2
		# Anoying that this one needs to be configuered manually based on xml config.
		# The dll should add a python function that fetch TXT_KEY_CIVICOPTION_X_ABBR.
		# I could then just loop through all the civicoptions and ask what their abbreviations are and add them to this list.
		# The order and names of these keys can potentially change if an XML modder plays around with the civics.
		aColTuple = (
			[dx+16,	self.charFaith],
			[dx,	self.charTrade],
			[dx+16,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_GOVERNMENT", ())	],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_RULE", ())		],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_POWER", ())		],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_MILITARY", ())	],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_RELIGION", ())	],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_SOCIETY", ())		],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_ECONOMY", ())		],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_WELFARE", ())		],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_CURRENCY", ())	],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_WORKFORCE", ())		],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_EDUCATION", ())	],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_LANGUAGE", ())	],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_IMMIGRATION", ())	],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_AGRICULTURE", ())	],
			[dx,	TRNSLTR.getText("TXT_KEY_CIVICOPTION_ABBR_GARBAGE", ())		],
			[dx*2,	TRNSLTR.getText("TXT_KEY_PEDIA_FAV_CIVIC", ())				]
		)
		w = 20*dx + 32
		y = -2
		dy = iSize0 + 8
		iRowsPerPage = (h - 32) / dy
		iOff = (dx - iSize1) / 2
		iter0 = 0
		iter1 = 0
		iter2 = 0
		# loop through all known players and add their rows.
		for iPlayerX in [iPlayer] + self.hasMet:
			# Header Row
			if not iter0 % iRowsPerPage:
				y += 4
				x0 = 0
				for iOff0, szTxt in aColTuple:
					x0 += iOff0
					screen.setLabelAt(self.getNextWidget(), ScPnl, uFont3b + szTxt, 1<<2, x0, y, 0, eFontGame, eWidGen, 1, 1)
				y += 22
			iter0 += 1
			# Adjust to player
			bActivePlayer = iPlayerX == iPlayer
			if bActivePlayer:
				CyPlayerX = CyPlayer
				bHuman = True
			else:
				CyPlayerX = GC.getPlayer(iPlayerX)
				bHuman = CyPlayerX.isHuman()

			# Build row
			CvLeaderHead = GC.getLeaderHeadInfo(CyPlayerX.getLeaderType())

			screen.attachPanelAt(ScPnl, self.getNextWidget(), "", "", True, True, ePnlOut, 0, y, w, dy, eWidGen, 1, 1)

			x = 4
			BTN = CvLeaderHead.getButton()
			screen.setImageButtonAt("WID|LEADER" + str(iPlayerX), ScPnl, BTN, x, y + 5, iSize0, iSize0, eWidGen, 1, 1)
			x += dx

			# Attitude
			if not bHuman:
				self.aRelationList.append([iPlayerX, iPlayer])
				szTxt = uFont3b + aSmileyList[CyPlayerX.AI_getAttitude(iPlayer)]
				screen.setTextAt("WID|RELATION" + str(iter1), ScPnl, szTxt, 1<<0, x, y + dy/2 - 21, 0, eFontGame, eWidGen, 1, 1)
				iter1 += 1
			# Religion
			iReligion = CyPlayerX.getStateReligion()
			if iReligion != -1:

				szTxt = uFont3b + u"%c"
				if CyPlayerX.hasHolyCity(iReligion):
					szTxt = szTxt % GC.getReligionInfo(iReligion).getHolyCityChar()
				else:
					szTxt = szTxt % GC.getReligionInfo(iReligion).getChar()

				screen.setTextAt("WID|RELIGION%d|%d" %(iReligion, iPlayerX), ScPnl, szTxt, 1<<0, x, y + dy/2 + 3, 0, eFontGame, eWidGen, 1, 1)
			x += 28
			# Trade
			if bActivePlayer or CyPlayerX.canHaveTradeRoutesWith(iPlayer):

				iTradeCommerce = iTradeRoutes = 0

				for CyCityX, liCity in tuCity:
					for CyCityY in liCity:
						if iPlayerX == CyCityY.getOwner():
							fProfit = CyCityX.calculateTradeYield(YieldTypes.YIELD_COMMERCE, CyCityX.calculateTradeProfitTimes100(CyCityY))
							iTradeCommerce += fProfit
							iTradeRoutes += 1

				iTradeCommerce //= 100

				szTxt1 = uFont2 + "%d" %iTradeCommerce + self.charCommerce
				szTxt2 = uFont2 + "%d" %iTradeRoutes
			else:
				szTxt1 = szTxt2 = uFont3b + self.charCross

			screen.setLabelAt(self.getNextWidget(), ScPnl, szTxt1, 1<<2, x + dx2, y + dy/2 + 5, 0, eFontGame, eWidGen, 1, 1)
			screen.setLabelAt(self.getNextWidget(), ScPnl, szTxt2, 1<<2, x + dx2, y + dy/2 - 20, 0, eFontGame, eWidGen, 1, 1)
			screen.setImageButtonAt(self.getNextWidget(), ScPnl, "", x, y + 9, dx1, iSize1, eWidTrade, iPlayer, iPlayerX)
			x += dx1
			# Civics
			for iCat in range (iCivicOptions):
				iCivic = CyPlayerX.getCivics(iCat)
				Img = "WID|CIVIC%d|%d" %(iCivic, iter2)
				BTN = GC.getCivicInfo(iCivic).getButton()
				screen.setImageButtonAt(Img, ScPnl, BTN, x + iOff, y + 9, iSize1, iSize1, eWidGen, 1, 1)
				iter2 += 1
				x += dx
			x += dx
			# Favorite Civic
			if not bActivePlayer:
				if bRandomPers:
					iCivic = GC.getLeaderHeadInfo(CyPlayerX.getPersonalityType()).getFavoriteCivic()
				else:
					iCivic = CvLeaderHead.getFavoriteCivic()

				if iCivic > -1:
					Img = "WID|CIVIC%d|%d" %(iCivic, iter2)
					BTN = GC.getCivicInfo(iCivic).getButton()
					screen.setImageButtonAt(Img, ScPnl, BTN, x + iOff, y + 9, iSize1, iSize1, eWidGen, 1, 1)
					iter2 += 1
			y += dy


	def drawTechDeals(self, screen):
		xRes = self.xRes
		yRes = self.yRes
		iPlayer = self.iPlayer
		CyPlayer = self.CyPlayer
		iTeam = self.iTeam
		CyTeam = self.CyTeam

		uFont3b = self.aFontList[3]
		charGold = "<font=3>" + self.charGold

		techsToGive = self.techsToGive
		techsToTake = self.techsToTake

		eWidJuToTech = WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH

		H_EDGE = self.H_EDGE_PANEL

		eFontGame = FontTypes.GAME_FONT
		eWidGen			= WidgetTypes.WIDGET_GENERAL

		ePnlHelpHUD	= PanelStyles.PANEL_STYLE_HUD_HELP
		ePnlBlue50	= PanelStyles.PANEL_STYLE_BLUE50
		ePnlSTD		= PanelStyles.PANEL_STYLE_STANDARD
		ePnlEmpty	= PanelStyles.PANEL_STYLE_EMPTY

		w0 = xRes - 16
		h0 = yRes - 2*H_EDGE

		PnlBG = self.getNextWidget()
		screen.addPanel(PnlBG, "", "", True, True, 8, H_EDGE, w0, h0, ePnlBlue50)

		w1 = (w0 - 116)/2
		x = 100
		szTxt = uFont3b + TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_WANTS", ())
		screen.setLabelAt("", PnlBG, szTxt, 1<<0, x, 8, 0, eFontGame, eWidGen, 1, 1)
		x += w1+w1
		szTxt = uFont3b + TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_FOR_TRADE_2", ())
		screen.setLabelAt("", PnlBG, szTxt, 1<<1, x, 8, 0, eFontGame, eWidGen, 1, 1)

		ScPnl = self.getNextWidget()
		screen.addScrollPanel(ScPnl, "", -6, H_EDGE + 28, xRes + 12, h0 - 56, ePnlEmpty)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")

		# Assemble the panel
		tradeData = TradeData()
		tradeData.ItemType = TradeableItems.TRADE_TECHNOLOGIES

		szNoTechTrading = TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_NO_TECH_TRADING", ())
		bHuman = CyPlayer.isHuman()
		bTechTrading = not self.bNoTechTrade and CyTeam.isTechTrading()
		bGoldTrading = CyTeam.isGoldTrading()
		iter0 = 0
		iPerRow = (w1 - 8) / 56
		x0 = (w1 - iPerRow * 56) / 2
		w = w0 - 8
		y = 4
		for iPlayerX in self.hasMet:
			# Vet player
			CyPlayerX = GC.getPlayer(iPlayerX)
			iTeamX = CyPlayerX.getTeam()
			if iTeamX == iTeam: continue
			CyTeamX = GC.getTeam(iTeamX)
			bHumanX = CyPlayerX.isHuman()
			# Define row content
			aList0 = []
			aList1 = []
			if bTechTrading and CyTeamX.isTechTrading():
				szNoTechTrade = ""
			else:
				szNoTechTrade = szNoTechTrading

			iGold = 0
			if bGoldTrading and CyTeamX.isGoldTrading():
				if not bHumanX:
					iGold = CyPlayerX.AI_maxGoldTrade(iPlayer)
				elif CyPlayerX.getGold() > 0:
					iGold = -1

			if not szNoTechTrade:

				for iTech in techsToGive:
					if not CyPlayerX.canResearch(iTech):
						continue
					if bHuman:
						aList0.append(iTech)
						continue

					tradeData.iData = iTech
					if CyPlayer.getTradeDenial(iPlayerX, tradeData) == DenialTypes.NO_DENIAL:
						aList0.append(iTech)

				for iTech in techsToTake:
					if not CyTeamX.isHasTech(iTech) or CyTeamX.isNoTradeTech(iTech):
						continue
					if bHumanX:
						aList1.append(iTech)
						continue

					tradeData.iData = iTech
					if CyPlayerX.getTradeDenial(iPlayer, tradeData) == DenialTypes.NO_DENIAL:
						aList1.append(iTech)

			# Define row
			if szNoTechTrade:
				h = 72
				yImg = y + 4
			else:
				iAmount = 0
				for i in [len(aList0), len(aList1)]:
					if i > iAmount:
						iAmount = i
				iRows = (iAmount + iPerRow - 1) / iPerRow
				if iRows > 1:
					h = 56 * iRows + 16
					yImg = y + h / 2 - 32
				else:
					h = 72
					yImg = y + 4
			# Fill row
			screen.attachPanelAt(ScPnl, self.getNextWidget(), "", "", True, True, ePnlSTD, 8, y, w, h, eWidGen, 1, 1)
			h -= 6
			BTN = GC.getLeaderHeadInfo(CyPlayerX.getLeaderType()).getButton()
			screen.setImageButtonAt("WID|LEADER" + str(iPlayerX), ScPnl, BTN, 12, yImg, 64, 64, eWidGen, 1, 1)

			if iGold:
				name = "GOLD|"
				if iGold > 0:
					name += "WILL"
				else:
					name += "ALL"
				name += "%d" % iPlayerX

				screen.setTextAt(name, ScPnl, charGold, 1<<0, 78, yImg+18, 0, eFontGame, eWidGen, 1, 1)

			if szNoTechTrade:
				screen.setLabelAt(self.getNextWidget(), ScPnl, szNoTechTrade, 1<<0, 100, y+28, 0, eFontGame, eWidGen, 1, 1)
			else:
				if aList0:
					Pnl = self.getNextWidget()
					screen.attachPanelAt(ScPnl, Pnl, "", "", True, True, ePnlBlue50, 104, y+1, w1, h, eWidGen, 1, 1)
					screen.setPanelColor(Pnl, 230, 200, 140)
					for i, iTech in enumerate(aList0):

						if not i % iPerRow:
							x1 = x0
							if i:
								y1 += 56
							else:
								y1 = 3
						BTN = GC.getTechInfo(iTech).getButton()
						screen.setImageButtonAt("WID|TECH|%d|%d" %(iTech, iter0), Pnl, BTN, x1, y1, 56, 56, eWidGen, 1, 1)
						iter0 += 1
						x1 += 56
				if aList1:
					Pnl = self.getNextWidget()
					screen.attachPanelAt(ScPnl, Pnl, "", "", True, True, ePnlBlue50, 112+w1, y+1, w1, h, eWidGen, 1, 1)
					screen.setPanelColor(Pnl, 230, 200, 140)
					for i, iTech in enumerate(aList1):

						if not i % iPerRow:
							x1 = x0
							if i:
								y1 += 56
							else:
								y1 = 3
						BTN = GC.getTechInfo(iTech).getButton()
						screen.setImageButtonAt("WID|TECH|%d|%d" %(iTech, iter0), Pnl, BTN, x1, y1, 56, 56, eWidGen, 1, 1)
						iter0 += 1
						x1 += 56
			y += h + 8


	# Utility
	def sumAttitude(self, iWho, iWhom):
		szTxt = GTM.getAttitudeString(iWho, iWhom)
		text = szTxt.split(">")
		iSum = 0
		for line in text:
			if line and line[0] in ("+", "-"):
				iSum += int(line[:line.find(":")])
		return iSum, szTxt

	def getNextWidget(self):
		szName = "FA_Widget" + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def deleteAllWidgets(self):
		screen = self.getScreen()
		# Generic widgets
		iNumWidgets = self.nWidgetCount
		self.nWidgetCount = 0
		for i in range(iNumWidgets):
			screen.deleteWidget(self.getNextWidget())
		self.nWidgetCount = 0

	#--------------------------#
	# Base operation functions #
	#||||||||||||||||||||||||||#
	def update(self, fDelta):
		if self.tooltip.bLockedTT:
			self.tooltip.handle(self.getScreen())

	# Handles the input for this screen...
	def handleInput(self, inputClass):
		screen = self.getScreen()
		if not screen.isActive():
			return
		HandleInputUtil.debugInput(inputClass)
		bAlt, bCtrl, bShift = self.InputData.getModifierKeys()
		iCode	= inputClass.eNotifyCode
		iData	= inputClass.iData
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName
		iBtn	= inputClass.iButtonType
		iData1	= inputClass.iData1
		iData2	= inputClass.iData2
		szFlag	= HandleInputUtil.MOUSE_FLAGS.get(inputClass.uiFlags, "UNKNOWN")

		szSplit = NAME.split("|")
		BASE = szSplit[0]
		if szSplit[1:]:
			TYPE = szSplit[1]
		else:
			TYPE = ""
		if szSplit[2:]:
			CASE = szSplit[2:]
		else:
			CASE = [0]

		self.tooltip.reset(screen)

		if iCode == NotifyCode.NOTIFY_CURSOR_MOVE_ON:

			if BASE == "WID":
				if TYPE == "RELATION":
					aMap = [
						"<color=255,0,0>",
						"<color=0,255,255>",
						"",
						"<color=0,255,0>",
						"<color=255,255,0>"
					]
					iPlayerY, iPlayerX = self.aRelationList[ID]
					iSum, szTxt = self.sumAttitude(iPlayerY, iPlayerX)
					szTxt += "\n---> " + aMap[GC.getPlayer(iPlayerY).AI_getAttitude(iPlayerX)] + str(iSum)
					self.tooltip.handle(screen, szTxt)

				elif TYPE == "LEADER":
					CyPlayer = GC.getPlayer(ID)
					szLeader = GC.getLeaderHeadInfo(CyPlayer.getLeaderType()).getDescription()

					if CyPlayer.isHuman():
						szTxt = CyPlayer.getName()
						if szTxt == szLeader:
							szTxt = TRNSLTR.getText("TXT_KEY_PITBOSS_HUMAN", ())
					else:
						szTxt = TRNSLTR.getText("TXT_KEY_MAIN_MENU_AI", ())

					szTxt += "\n%s %s\n" %(CyPlayer.getCivilizationAdjective(0), szLeader)
					szTxt += "%s - %s" %(CyPlayer.getCivilizationShortDescription(0), CyPlayer.getCivilizationDescription(0))
					self.tooltip.handle(screen, szTxt)

				elif TYPE == "BONUS":
					if CASE[0] == "DEAL":
						iBonus, CyDeal = self.aDealMap[ID]
						szTxt = GTM.getDealString(CyDeal, self.iPlayer) + "\n"
						if CyDeal.isCancelable(self.iPlayer, False):
							szTxt += TRNSLTR.getText("TXT_KEY_MISC_CLICK_TO_CANCEL", ()) + "\n"
						szTxt += "\n"
					else:
						szTxt = ""
						iBonus = ID
					szTxt += GTM.getBonusHelp(iBonus, False)
					self.tooltip.handle(screen, szTxt)
				elif TYPE == "CIVIC":
					self.tooltip.handle(screen, GTM.parseCivicInfo(ID, False, True, False))
				elif TYPE == "RELIGION":
					self.tooltip.handle(screen, GTM.parseReligionInfo(ID, False))
				elif TYPE == "TECH":
					self.tooltip.handle(screen, GTM.getTechHelp(ID, False, True, False, True, -1))

			elif BASE == "GPT":
				szTxt = ""
				if TYPE == "WILL":
					szTxt = TRNSLTR.getText("TXT_KEY_FOREIGN_ADVISOR_GOLD_PER_TURN_FOR_TRADE", (ID,))
				elif TYPE == "POS":
					szTxt = "<color=27,255,71>" + TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_NET_INCOME", (ID,)) + " " + self.charGold
				elif TYPE == "NEG":
					szTxt = "<color=255,27,27>" + TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_NET_INCOME", (-ID,)) + " " + self.charGold
				if szTxt:
					self.tooltip.handle(screen, szTxt)
			elif BASE == "GOLD":
				if TYPE == "ALL":
					CyPlayerX = GC.getPlayer(ID)
					iGold = CyPlayerX.getGold()
					szTxt = ""
					szTxt += str(iGold) + self.charGold
					self.tooltip.handle(screen, szTxt)
				elif TYPE == "WILL":
					iGold = GC.getPlayer(ID).AI_maxGoldTrade(self.iPlayer)
					szTxt = ""
					szTxt += str(iGold) + self.charGold
					self.tooltip.handle(screen, szTxt)

		elif iCode == NotifyCode.NOTIFY_CLICKED:

			if BASE == "FA_Tab":
				if CASE[0] != "Col":
					screen.hide("FA_Tab|Col" + str(self.iTab))
					screen.show("FA_Tab" + str(self.iTab))
					self.iTab = ID
					self.drawContents(screen)

			elif NAME == "BonusList":
				self.iBonusList = ID
				self.drawContents(screen)

			elif BASE == "KD":
				screen.deleteWidget("KD|Pnl")
				screen.deleteWidget("KD|Nay0")
				screen.deleteWidget("KD|Nay1")
				screen.deleteWidget("KD|Yay1")
				if TYPE == "Yay":
					CyMessageControl().sendModNetMessage(666, self.iPlayer, iData1, 0, 0)
					GAME.getDeal(iData1).kill()
					self.drawContents(screen)

			elif szFlag == "MOUSE_RBUTTONUP":

				if iBtn == WidgetTypes.WIDGET_LEADERHEAD and self.iTab == 0:
					if bShift:
						UP.pediaJumpToLeader([GC.getPlayer(iData1).getLeaderType()])
					elif iData1 != self.iPlayer:
						screen.hideScreen()

				elif iBtn == WidgetTypes.WIDGET_LEADERHEAD:
					if iData1 != self.iPlayer:
						screen.hideScreen()

				elif BASE == "WID":
					if TYPE == "LEADER":
						UP.pediaJumpToLeader([GC.getPlayer(ID).getLeaderType()])
					elif TYPE == "BONUS":
						if CASE[0] == "DEAL":
							iBonus = self.aDealMap[ID][0]
						else:
							iBonus = ID
						UP.pediaJumpToBonus([iBonus])
					elif TYPE == "CIVIC":
						UP.pediaJumpToCivic([ID])
					elif TYPE == "RELIGION":
						UP.pediaJumpToReligion([ID])
					elif TYPE == "TECH":
						UP.pediaJumpToTech([ID])
			else:
				if iBtn == WidgetTypes.WIDGET_LEADERHEAD and self.iTab == 0:
					aList = self.selectedLeaders
					if bShift:
						if iData1 in aList:
							aList.remove(iData1)
						else:
							aList.append(iData1)
					else:
						if aList and len(aList) == 1 and iData1 == aList[0]:
							aList = []
						else:
							aList = [iData1]
					self.selectedLeaders = aList
					self.drawContents(screen)

				elif BASE == "WID":
					if TYPE == "LEADER":
						if ID != self.iPlayer:
							self.CyPlayer.contact(ID)
							screen.hideScreen()
					elif TYPE == "BONUS":
						if CASE[0] == "DEAL":
							iBonus, CyDeal = self.aDealMap[ID]
							if CyDeal.isCancelable(self.iPlayer, False):
								eWidGen = WidgetTypes.WIDGET_GENERAL
								ePnlMain = PanelStyles.PANEL_STYLE_MAIN
								eBtnStd = ButtonStyles.BUTTON_STYLE_STANDARD
								xRes = self.xRes
								yRes = self.yRes
								xMid = xRes/2
								yMid = yRes/2
								screen.setImageButton("KD|Nay0", "", 0, 0, xRes, yRes, eWidGen, 1, 1)
								screen.addPanel("KD|Pnl", "", "", True, True, xMid-180, yMid-60, 360, 120, ePnlMain)
								szTxt = "<font=3b>" + TRNSLTR.getText("TXT_KEY_ANNUL", ())
								screen.setLabelAt("", "KD|Pnl", szTxt, 1<<2, 180, 20, 0, FontTypes.GAME_FONT, eWidGen, 1, 1)
								szTxt = TRNSLTR.getText("TXT_KEY_POPUP_NO", ())
								screen.setButtonGFC("KD|Nay1", szTxt, "", xMid - 160, yMid, 150, 40, eWidGen, 1, 1, eBtnStd)
								szTxt = TRNSLTR.getText("TXT_KEY_POPUP_YES", ())
								screen.setButtonGFC("KD|Yay1", szTxt, "", xMid + 10, yMid, 150, 40, eWidGen, CyDeal.getID(), 1, eBtnStd)

		elif iCode == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:
			if NAME == "FA_DebugDD":
				iIndex = screen.getSelectedPullDownID(NAME)
				self.iPlayer = iPlayer = screen.getPullDownData(NAME, iIndex)
				self.CyPlayer = CyPlayer = GC.getPlayer(iPlayer)
				self.iTeam = iTeam = CyPlayer.getTeam()
				self.CyTeam = GC.getTeam(iTeam)
				self.calcPlayerRelative()
				self.drawContents(screen)


	def onClose(self):
		del self.CyPlayer, self.iPlayer, self.CyTeam, self.iTeam, self.iResID, self.InputData, \
			self.xRes, self.yRes, self.xMid, self.yMid, self.H_EDGE_PANEL, self.HILITE_SQUARE, \
			self.aFontList, self.aBonusTuple, self.selectedLeaders, self.hasMet, self.tuCity, \
			self.aColMap, self.aSmileyList, self.aDealMap, self.aRelationList, self.bRandomPers, \
			self.charWar, self.charPeace, self.charCommerce, self.charGold, self.charTrade, self.charFaith, self.charCross, \
			self.bNoTechTrade, self.techsToGive, self.techsToTake
