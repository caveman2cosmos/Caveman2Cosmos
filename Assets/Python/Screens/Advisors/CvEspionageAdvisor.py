## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
## Improvements to this screen by Almightix - thanks
from CvPythonExtensions import *
import CvScreenEnums

# globals
GC = CyGlobalContext()
TRNSLTR = CyTranslator()
GAME = GC.getGame()

class CvEspionageAdvisor:

	def __init__(self):
		self.iTab = 0
		self.iTargetPlayer = -1
		self.bMissionList = True
		self.TheScreen = None

	def interfaceScreen(self):
		if self.TheScreen is None:
			self.TheScreen = TheScreen(self)

	def redraw(self, screen):
		self.TheScreen.redraw(screen)

	def handleInput(self, inputClass):
		self.TheScreen.handleInput(inputClass)

	def update(self, fDelta): return

	def onClose(self):
		self.iTab = self.TheScreen.iTab
		self.iTargetPlayer = self.TheScreen.iTargetPlayer
		self.bMissionList = self.TheScreen.bMissionList
		self.TheScreen = None


class TheScreen:

	def __init__(self, main):
		self.iTab = iTab = main.iTab
		self.iTargetPlayer = iTargetPlayer = main.iTargetPlayer
		self.bMissionList = main.bMissionList
		screen = CyGInterfaceScreen("EspionageAdvisor", CvScreenEnums.ESPIONAGE_ADVISOR)
		# Get screen resolution.
		self.iResolutionX = iResX = screen.getXResolution()
		self.iResolutionY = iResY = screen.getYResolution()
		# Calibrate variables.
		if iResY > 1000:
			self.H_EDGE_PANEL = H_EDGE_PANEL = 38
			uFontEdge = "<font=4b>"
		elif iResY > 800:
			self.H_EDGE_PANEL = H_EDGE_PANEL = 32
			uFontEdge = "<font=3b>"
		else:
			self.H_EDGE_PANEL = H_EDGE_PANEL = 29
			uFontEdge = "<font=2b>"

		if iResX > 1700:
			self.SIZE = 64
			self.aFontList = aFontList = [uFontEdge, "<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1>"]
		elif iResX > 1400:
			self.SIZE = 56
			self.aFontList = aFontList = [uFontEdge, "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0>"]
		else:
			self.SIZE = 48
			self.aFontList = aFontList = [uFontEdge, "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0b>", "<font=0>", "<font=0>"]

		self.Y_TOP_PAGE = Y_TOP_PAGE = H_EDGE_PANEL - 6
		self.H_PAGE = iResY - Y_TOP_PAGE - H_EDGE_PANEL + 8
		self.Y_BOT_TEXT = Y_BOT_TEXT = iResY - H_EDGE_PANEL + 8

		# Declare misc variables
		self.bRefreshLeftPanel = False
		self.iRedrawCount = 0
		self.nWidgetCount = 0
		self.iCityActID = -1
		self.iMissionAct = -1
		self.iPlayerAct = iPlayerAct = GAME.getActivePlayer()
		self.CyPlayerAct = CyPlayerAct = GC.getPlayer(iPlayerAct)
		self.iTeamAct = iTeamAct = CyPlayerAct.getTeam()
		self.CyTeamAct = CyTeamAct = GC.getTeam(iTeamAct)

		aList = []
		for iRank in range(GC.getMAX_PC_PLAYERS()):
			iPlayerX = GAME.getRankPlayer(iRank)
			if iPlayerX == iPlayerAct:
				continue
			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isAlive(): break

			iTeamX = CyPlayerX.getTeam()
			if iTeamX == iTeamAct: continue

			if CyTeamAct.isHasMet(iTeamX):
				aList.append(iPlayerX)

		if aList:
			if iTargetPlayer not in aList:
				self.iTargetPlayer = aList[0]
		else:
			self.iTargetPlayer = None

		self.aPlayerList = aList

		# Declare constants
		self.charBeaker			= u'%c' % GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar()
		self.charEsp			= u'%c' % GC.getCommerceInfo(CommerceTypes.COMMERCE_ESPIONAGE).getChar()
		self.charCross			= u'%c' % GAME.getSymbolID(FontSymbols.CROSSED_CHAR)
		self.charLifeSupportSS	= u'%c' %(GAME.getSymbolID(FontSymbols.HAPPY_CHAR) - 21)
		self.ANGRY_CIV_CHAR				= GAME.getSymbolID(FontSymbols.ANGRY_CIV_CHAR)
		iWidGen = WidgetTypes.WIDGET_GENERAL
		iFontTitle = FontTypes.TITLE_FONT

		# mission constants
		for iMissionLoop in range(GC.getNumEspionageMissionInfos()):
			pMission = GC.getEspionageMissionInfo(iMissionLoop)
			if pMission.getCost() != -1 and pMission.isPassive():
				if pMission.isInvestigateCity():
					self.MissionInvestigateCity = iMissionLoop
				elif pMission.isSeeDemographics():
					self.MissionSeeDemo = iMissionLoop
				elif pMission.isSeeResearch():
					self.MissionSeeResearch = iMissionLoop
				else:
					self.MissionCityVisibility = iMissionLoop

		# Build Espionage Advisor screen.
		screen.setRenderInterfaceOnly(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.addDDSGFC("EA_BG", CyArtFileMgr().getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, iResX, iResY, iWidGen, 0, 0)
		screen.addPanel("EA_TopPanel", "", "", True, False, 0, 0, iResX, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("EA_BotPanel", "", "", True, False, 0, iResY - H_EDGE_PANEL, iResX, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.setLabel("EA_Header", "", uFontEdge + TRNSLTR.getText("TXT_KEY_ESPIONAGE_SCREEN",()), 1<<2, iResX / 2, 0, 0, iFontTitle, iWidGen, 0, 0)
		screen.setText("EA_Btn_Exit", "", uFontEdge + TRNSLTR.getText("TXT_WORD_EXIT",()), 1<<1, iResX - 16, 0, 0, iFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		iCol = GC.getCOLOR_YELLOW()
		szTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_ESPIONAGE_MISSIONS_TAB", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iCol)
		dX = iResX / 2
		x = dX / 2
		screen.setText("EA_Tab0", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, iFontTitle, iWidGen, 0, 0)
		screen.setText("EA_Col_Tab0", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, iFontTitle, iWidGen, 0, 0)
		screen.hide("EA_Col_Tab0")

		szTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_ESPIONAGE_SPYVSPY_TAB", ())
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iCol)
		x += dX
		screen.setText("EA_Tab1", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, iFontTitle, iWidGen, 0, 0)
		screen.setText("EA_Col_Tab1", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, iFontTitle, iWidGen, 0, 0)
		screen.hide("EA_Col_Tab1")

		# draw the contents

		if iTab == 0:
			screen.hide("EA_Tab0")
			screen.show("EA_Col_Tab0")
			self.drawMissionTab(screen)
		elif iTab == 1:
			screen.hide("EA_Tab1")
			screen.show("EA_Col_Tab1")
			self.drawSpyvSpyTab(screen)

		import DebugUtils
		if DebugUtils.isAnyDebugMode():
			screen.addDropDownBoxGFC("DebugDropdown", 22, 0, 300, iWidGen, 554, -1, iFontTitle)
			for j in range(GC.getMAX_PC_PLAYERS()):
				if GC.getPlayer(j).isAlive():
					screen.addPullDownString("DebugDropdown", GC.getPlayer(j).getName(), j, j, False )


	def drawMissionTab(self, screen):
		iTargetPlayer = self.iTargetPlayer
		if iTargetPlayer is None:
			return
		else:
			bDebug = GAME.isDebugMode()
			iPlayerAct = self.iPlayerAct
			CyPlayerAct = self.CyPlayerAct
			iTeamAct = self.iTeamAct
			CyTeamAct = self.CyTeamAct

			uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2, uFont1 = self.aFontList

			iWidGen			= WidgetTypes.WIDGET_GENERAL
			iPanelBlue50	= PanelStyles.PANEL_STYLE_BLUE50
			iPanelIn		= PanelStyles.PANEL_STYLE_IN
			iTableStd		= TableStyles.TABLE_STYLE_STANDARD
			iBtnLabel		= ButtonStyles.BUTTON_STYLE_LABEL
			iFontTitle		= FontTypes.TITLE_FONT

			artPath0 = CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath()
			artPath1 = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_MINUS").getPath()
			artPath2 = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_PLUS").getPath()
			szIcon = uFont3 + self.charEsp
			ANGRY_CIV_CHAR		= self.ANGRY_CIV_CHAR
			charBeaker			= self.charBeaker
			charCross			= self.charCross
			charLifeSupportSS	= self.charLifeSupportSS

			aHalfX = self.iResolutionX / 2
			dx = aHalfX / 48
			x = dx
			y = self.Y_TOP_PAGE + 16
			self.dx = dx = aHalfX - 2 * dx
			self.dy = dy = self.H_PAGE - 32

			# Left Panel
			PanelLeft = self.getNextWidgetName()
			screen.addScrollPanel(PanelLeft, "", x, y, dx, dy, PanelStyles.PANEL_STYLE_EMPTY)

			SIZE = self.SIZE
			iSize = SIZE / 2 - 8

			x2 = x + 4*dx/7
			y1 = -4 - SIZE
			dy1 = SIZE/8
			y2 = 3*dy1
			y3 = 2*dy1
			y4 = 4*dy1
			for iPlayerX in self.aPlayerList:
				x1 = 26
				y1 += SIZE + 4
				CyPlayerX = GC.getPlayer(iPlayerX)
				iTeamX = CyPlayerX.getTeam()
				CyTeamX = GC.getTeam(iTeamX)
				szPlayerX = str(iPlayerX)
				# leader panel
				LeaderPanel = "LeaderPanel" + szPlayerX
				screen.attachPanelAt(PanelLeft, LeaderPanel, "", "", False, False, iPanelIn, 0, y1, dx - 40, SIZE + 2, iWidGen, 0, 0)

				szText = uFont3 + u'%c' %(ANGRY_CIV_CHAR + CyPlayerX.AI_getAttitude(iPlayerAct))
				screen.setLabelAt(self.getNextWidgetName(), LeaderPanel, szText, 1<<0, 0, SIZE - y4, 0, iFontTitle, iWidGen, 0, 0)
				# leader image
				leaderBtn = "LeaderBtn" + szPlayerX
				Btn = GC.getLeaderHeadInfo(CyPlayerX.getLeaderType()).getButton()
				screen.addCheckBoxGFCAt(PanelLeft, leaderBtn, Btn, artPath0, x1, y1, SIZE, SIZE, iWidGen, 0, 0, iBtnLabel, False)
				x1 += SIZE
				# leader name
				iColR = CyPlayerX.getPlayerTextColorR()
				iColG = CyPlayerX.getPlayerTextColorG()
				iColB = CyPlayerX.getPlayerTextColorB()
				if iColR + iColG + iColB < 149:
					if iColR >= iColG:
						if iColR >= iColB:
							iColR = 255
						else:
							iColB = 255
					elif iColG > iColB:
						iColG = 255
					else:
						iColB = 255
				szText = u"<color=%d,%d,%d>" %(iColR, iColG, iColB)
				szText = uFont2 + szText + CyPlayerX.getName() + " - " + CyPlayerX.getCivilizationShortDescription(0)
				screen.setLabelAt(self.getNextWidgetName(), LeaderPanel, szText, 1<<0, x1, -2, 0, iFontTitle, iWidGen, 0, 0)

				# EP Weights
				screen.setImageButtonAt("WeightBtn|Less" + szPlayerX, LeaderPanel, artPath1, x1, y2, iSize, iSize, iWidGen, iTeamX, 0)
				x1 += iSize
				screen.setImageButtonAt("WeightBtn|More" + szPlayerX, LeaderPanel, artPath2, x1, y2, iSize, iSize, iWidGen, iTeamX, 0)
				x1 += iSize
				szText = uFont3 + str(CyPlayerAct.getEspionageSpendingWeightAgainstTeam(iTeamX))
				screen.setLabelAt("SpyWeight" + szPlayerX, LeaderPanel, szText, 1<<0, x1, y2, 0, iFontTitle, iWidGen, 0, 0)
				x1 += y3 * 5

				# Espionage Points
				iEP = CyTeamAct.getEspionagePointsAgainstTeam(iTeamX)
				screen.setLabelAt(self.getNextWidgetName(), LeaderPanel, uFont2 + str(iEP), 1<<1, x2, y3, 0, iFontTitle, iWidGen, -1, -1)

				iEP_X = CyTeamX.getEspionagePointsAgainstTeam(iTeamAct)
				if iEP > iEP_X * 1.1 or bDebug:
					bSeeDetails = True
					screen.setLabelAt(self.getNextWidgetName(), LeaderPanel, uFont2 + str(iEP_X), 1<<1, x2, y4, 0, iFontTitle, iWidGen, 0, 0)
				else:
					bSeeDetails = False

				# Points per turn
				screen.setLabelAt("EspIcon" + szPlayerX, LeaderPanel, szIcon, 1<<0, 0, dy1, 0, iFontTitle, iWidGen, 0, 0)

				iSpending = CyPlayerAct.getEspionageSpending(iTeamX)
				szText = uFont2
				if iSpending:
					if iSpending > 0:
						szText += "<color=0,255,0>+"
					else:
						szText += "<color=255,0,0>"
				szText += str(iSpending)
				screen.setLabelAt("EspSpending" + szPlayerX, LeaderPanel, szText, 1<<0, x2+2, y3, 0, iFontTitle, iWidGen, 0, 0)

				if bSeeDetails:
					iSpending = CyPlayerX.getEspionageSpending(iPlayerAct)
					if iSpending:
						szText = uFont2
						if iSpending > 0:
							szText += "<color=255,0,0>+"
						else:
							szText += "<color=0,255,0>"
						szText += str(iSpending)
						screen.setLabelAt(self.getNextWidgetName(), LeaderPanel, szText, 1<<0, x2+2, y4, 0, iFontTitle, iWidGen, 0, 0)

				# EPoints Multiplier
				iMultiplier = getEspionageModifier(iTeamAct, iTeamX)
				szText = uFont2
				if iMultiplier >= 130:
					szText += "<color=255,255,0>"
				elif iMultiplier <= 70:
					szText += "<color=0,255,255>"
				szText += str(iMultiplier) + "%"
				screen.setLabelAt(self.getNextWidgetName(), LeaderPanel, szText, 1<<1, x1, y3, 0, iFontTitle, iWidGen, 0, 0)

				if bSeeDetails:
					iMultiplier = getEspionageModifier(iTeamX, iTeamAct)
					szText = uFont2
					if iMultiplier >= 130:
						szText += "<color=0,255,255>"
					elif iMultiplier <= 70:
						szText += "<color=255,255,0>"
					szText += str(iMultiplier) + "%"
					screen.setLabelAt(self.getNextWidgetName(), LeaderPanel, szText, 1<<1, x1, y4, 0, iFontTitle, iWidGen, 0, 0)
				x1 += 6

				# Counter Espionage
				iCounterEsp = CyTeamAct.getCounterespionageTurnsLeftAgainstTeam(iTeamX)
				if iCounterEsp > 0:
					szText = "<color=0,255,0>(" + str(iCounterEsp) + ")"
					screen.setLabelAt(self.getNextWidgetName(), LeaderPanel, uFont2 + szText, 1<<0, x1, y3, 0, iFontTitle, iWidGen, 0, 0)

				if bSeeDetails:
					iCounterEsp = CyTeamX.getCounterespionageTurnsLeftAgainstTeam(iTeamAct)
					if iCounterEsp > 0:
						szText = "<color=255,0,0>(" + str(iCounterEsp) + ")"
						screen.setLabelAt(self.getNextWidgetName(), LeaderPanel, uFont2 + szText, 1<<0, x1, y4, 0, iFontTitle, iWidGen, 0, 0)

				# Symbols for 'Demographics' and 'Research'
				iDemoCost = CyPlayerAct.getEspionageMissionCost(self.MissionSeeDemo, iPlayerX, None, -1)
				iTechCost = CyPlayerAct.getEspionageMissionCost(self.MissionSeeResearch, iPlayerX, None, -1)

				# can see demographics icon
				if iEP >= iDemoCost:
					szText = charLifeSupportSS
				else:
					szText = charCross

				# can see research icon
				if iEP >= iTechCost:
					szText += charBeaker
				else:
					szText += charCross

				screen.setLabelAt(self.getNextWidgetName(), LeaderPanel, uFont3 + szText, 1<<1, dx - 56, y3 - 2, 0, iFontTitle, iWidGen, 0, 0)

				if bSeeDetails:
					# Target Player
					iDemoCost = CyPlayerX.getEspionageMissionCost(self.MissionSeeDemo, iPlayerAct, None, -1)
					iTechCost = CyPlayerX.getEspionageMissionCost(self.MissionSeeResearch, iPlayerAct, None, -1)
					# can see demographics icon
					if iEP_X >= iDemoCost:
						szText = charLifeSupportSS
					else:
						szText = charCross
					# can see research icon
					if iEP_X >= iTechCost:
						szText += charBeaker
					else:
						szText += charCross

					szName = self.getNextWidgetName()
					screen.setLabelAt(szName, LeaderPanel, uFont3 + szText, 1<<1, dx - 56, y4, 0, iFontTitle, iWidGen, 0, 0)

			self.refreshMissionTab_Left(screen, CyPlayerAct)
			screen.setState("LeaderBtn" + str(iTargetPlayer), True)

			# Right Panel
			bMissionList = self.bMissionList
			x += aHalfX
			RightPanel = self.getNextWidgetName()
			screen.addPanel(RightPanel, "", "", True, True, x, y, dx, dy, iPanelBlue50)

			dx1 = 20
			dx2 = dx / 3
			dx3 = dx - 24
			dy1 = 8
			dy2 = dy / 4

			# City/Mission List Header
			szText = uFont3b + TRNSLTR.getText("TXT_KEY_CONCEPT_CITIES", ())
			screen.setTextAt("ListHeader0", RightPanel, szText, 1<<0, dx1, dy1, 0, iFontTitle, iWidGen, 0, 0)
			szText = uFont3b + TRNSLTR.getText("TXT_KEY_ESPIONAGE_SCREEN_MISSIONS", ())
			screen.setTextAt("ListHeader1", RightPanel, szText, 1<<0, dx1, dy1, 0, iFontTitle, iWidGen, 0, 0)
			if bMissionList:
				screen.hide("ListHeader0")
			else:
				screen.hide("ListHeader1")

			# City/Mission Table Header
			szText = uFont3b + TRNSLTR.getText("TXT_KEY_CONCEPT_CITIES", ())
			screen.setLabelAt("CityTableHeader", RightPanel, szText, 1<<0, dx2, dy2, 0, iFontTitle, iWidGen, 0, 0)
			szText = uFont3b + TRNSLTR.getText("TXT_KEY_ESPIONAGE_SCREEN_MISSIONS", ())
			screen.setLabelAt("MissionTableHeader", RightPanel, szText, 1<<0, dx2, dy2, 0, iFontTitle, iWidGen, 0, 0)
			if bMissionList:
				screen.hide("MissionTableHeader")
			else:
				screen.hide("CityTableHeader")

			# Other Headers
			szText = uFont3b + TRNSLTR.getText("TXT_KEY_ESPIONAGE_SCREEN_PASSIVE_EFFECTS", ())
			screen.setLabelAt(self.getNextWidgetName(), RightPanel, szText, 1<<0, dx2, dy1, 0, iFontTitle, iWidGen, 0, 0)

			szText = uFont3b + TRNSLTR.getText("TXT_KEY_ESPIONAGE_SCREEN_COST", ())
			screen.setLabelAt(self.getNextWidgetName(), RightPanel, szText, 1<<1, dx3, dy1, 0, iFontTitle, iWidGen, 0, 0)

			screen.setLabelAt(self.getNextWidgetName(), RightPanel, szText, 1<<1, dx3, dy2, 0, iFontTitle, iWidGen, 0, 0)

			self.x1 = x1 = x + dx1
			self.x2 = x2 = x + dx2
			self.y1 = y1 = y + dy1 * 2
			self.y2 = y2 = y + dy2 + 4

			# Mission selection list
			LIST = "CityMissionList1"
			screen.addListBoxGFC(LIST, "", x1, y1 + 26, x2 - x1 - 16, dy - 64, iTableStd)
			screen.enableSelect(LIST, True)
			screen.setStyle(LIST, "Table_StandardCiv_Style")
			iCount = 0
			for iMissionX in range(GC.getNumEspionageMissionInfos()):
				pMission = GC.getEspionageMissionInfo(iMissionX)
				# Filter
				if pMission.getCost() == -1 or not pMission.isTargetsCity(): continue
				if pMission.getTechPrereq() != -1 and not CyTeamAct.isHasTech(pMission.getTechPrereq()): continue
				# Carry on
				szText = uFont2 + pMission.getDescription()
				screen.appendListBoxStringNoUpdate(LIST, szText, iWidGen, iMissionX, 0, 1<<0)
				# Default active mission
				if self.iMissionAct == -1:
					self.iMissionAct = iMissionX
				# Select active mission
				if self.iMissionAct == iMissionX:
					screen.setSelectedListBoxStringGFC(LIST, iCount)
				# Iterate
				iCount += 1
			screen.updateListBox(LIST)
			if not bMissionList:
				screen.hide(LIST)

			self.EffectsTable = self.getNextWidgetName()
			self.refreshMissionTab_Right(screen)


	def refreshMissionTab_Right(self, screen):
		uFont2 = self.aFontList[6]
		iWidGen		= WidgetTypes.WIDGET_GENERAL
		iTableStd	= TableStyles.TABLE_STYLE_STANDARD
		x1 = self.x1
		x2 = self.x2
		dx = self.dx
		y1 = self.y1
		y2 = self.y2
		dy = self.dy
		CyPlayerAct = self.CyPlayerAct
		iTeamAct = self.iTeamAct
		CyTeamAct = self.CyTeamAct
		iPlayer = self.iTargetPlayer
		CyPlayer = GC.getPlayer(iPlayer)
		iTeam = CyPlayer.getTeam()

		iEP = CyTeamAct.getEspionagePointsAgainstTeam(iTeam)

		bMissionList = self.bMissionList
		iMissionAct = self.iMissionAct

		# City selection list and "Mission Cost per City" table.
		LIST = "CityMissionList0"
		screen.addListBoxGFC(LIST, "", x1, y1 + 26, x2 - x1 - 16, dy - 64, iTableStd)
		screen.enableSelect(LIST, True)
		screen.setStyle(LIST, "Table_StandardCiv_Style")

		TABLE = "CityMissionTable0"
		w1 = x1 - 40 + dx - x2
		screen.addTableControlGFC(TABLE, 3, x2, y2 + 26, w1, dy - y2 + y1 - 64, False, False, 32,32, iTableStd)
		w2 = (w1 - 10)/4
		screen.setTableColumnHeader(TABLE, 0, "", w1 - w2 - 10)
		screen.setTableColumnHeader(TABLE, 1, "", w2)
		screen.setTableColumnHeader(TABLE, 2, "", 10)

		iCityActID = self.iCityActID
		iCount = 0
		for CyCity in CyPlayer.cities():
			if CyCity.isRevealed(iTeamAct, False):
				szText = uFont2 + CyCity.getName()
				screen.appendListBoxStringNoUpdate(LIST, szText, iWidGen, CyCity.getID(), 0, 1<<0)
				iCityID = CyCity.getID()

				if iCityActID == -1:
					self.iCityActID = iCityActID = iCityID

				if iCityActID == iCityID:
					screen.setSelectedListBoxStringGFC(LIST, iCount)
				iCount += 1

				iCost = CyPlayerAct.getEspionageMissionCost(iMissionAct, iPlayer, CyCity.plot(), -1)
				if iCost > -1:
					szCost = uFont2
					if iCost <= iEP:
						szCost += "<color=0,255,0>"
					szCost += str(iCost)

					iRow = screen.appendTableRow(TABLE)
					screen.setTableText(TABLE, 0, iRow, szText, "", iWidGen, 0, 0, 1<<0)
					screen.setTableText(TABLE, 1, iRow, szCost, "", iWidGen, 0, 0, 1<<1)
		screen.updateListBox(LIST)
		if bMissionList:
			screen.hide(LIST)
		else:
			screen.hide(TABLE)

		# Tables for "Passive Effects" & "Mission Cost in the City".
		EffectsTable = self.EffectsTable
		screen.addTableControlGFC(EffectsTable, 3, x2, y1 + 26, w1, y2 - y1 - 28, False, False, 32,32, iTableStd)
		screen.setTableColumnHeader(EffectsTable, 0, "", w1 - w2 - 10)
		screen.setTableColumnHeader(EffectsTable, 1, "", w2)
		screen.setTableColumnHeader(EffectsTable, 2, "", 10)

		TABLE = "CityMissionTable1"
		screen.addTableControlGFC(TABLE, 3, x2, y2 + 26, w1, dy - y2 + y1 - 64, False, False, 32,32, iTableStd)
		screen.setTableColumnHeader(TABLE, 0, "", w1 - w2 - 10)
		screen.setTableColumnHeader(TABLE, 1, "", w2)
		screen.setTableColumnHeader(TABLE, 2, "", 10)

		if iCityActID > -1:
			CyPlot = CyPlayer.getCity(iCityActID).plot()
		else: CyPlot = None

		for iMissionX in range(GC.getNumEspionageMissionInfos()):
			pMission = GC.getEspionageMissionInfo(iMissionX)
			if pMission.getCost() == -1: continue
			iTech = pMission.getTechPrereq()
			bTargetCity = pMission.isTargetsCity()
			if pMission.isPassive():
				if bMissionList != bTargetCity:
					iCost = CyPlayerAct.getEspionageMissionCost(iMissionX, iPlayer, CyPlot, -1)
					if iCost == -1: continue
					szCost = uFont2
					if iCost <= iEP:
						szCost += "<color=0,255,0>"
					szCost += str(iCost)

					szText = uFont2 + pMission.getDescription()
					if iTech != -1 and not CyTeamAct.isHasTech(iTech):
						szText += " (<color=255,0,0>%s</color>)" %(GC.getTechInfo(pMission.getTechPrereq()).getDescription())

					iRow = screen.appendTableRow(EffectsTable)
					screen.setTableText(EffectsTable, 0, iRow, szText, "", iWidGen, 0, 0, 1<<0)
					screen.setTableText(EffectsTable, 1, iRow, szCost, "", iWidGen, 0, 0, 1<<1)

			elif bTargetCity and CyPlot:
				iCost = CyPlayerAct.getEspionageMissionCost(iMissionX, iPlayer, CyPlot, -1)
				if iCost == -1: continue
				szCost = uFont2
				if iCost <= iEP:
					szCost += "<color=0,255,0>"
				szCost += str(iCost)

				szText = uFont2 + pMission.getDescription()
				if iTech != -1 and not CyTeamAct.isHasTech(iTech):
					szText += " (<color=255,0,0>%s</color>)" %(GC.getTechInfo(pMission.getTechPrereq()).getDescription())

				iRow = screen.appendTableRow(TABLE)
				screen.setTableText(TABLE, 0, iRow, szText, "", iWidGen, 0, 0, 1<<0)
				screen.setTableText(TABLE, 1, iRow, szCost, "", iWidGen, 0, 0, 1<<1)

		if bMissionList:
			screen.hide(TABLE)


	def refreshMissionTab_Left(self, screen, CyPlayerAct):
		uFont2 = self.aFontList[6]
		for iPlayerX in self.aPlayerList:
			CyPlayerX = GC.getPlayer(iPlayerX)
			iTeamX = CyPlayerX.getTeam()
			# EP Spending (Points per turn)
			iSpending = CyPlayerAct.getEspionageSpending(iTeamX)
			szPlayer = str(iPlayerX)
			if iSpending > 0:
				screen.show("EspIcon" + szPlayer)
				szText = uFont2 + "<color=0,255,0>+" + str(iSpending)
				LABEL = "EspSpending" + szPlayer
				screen.hide(LABEL)
				screen.modifyLabel(LABEL, szText, 1<<0)
				screen.show(LABEL)
			else:
				screen.hide("EspIcon" + szPlayer)
				screen.hide("EspSpending" + szPlayer)


	def drawSpyvSpyTab(self, screen):
		bDebug = GAME.isDebugMode()
		iPlayerAct = self.iPlayerAct
		CyPlayerAct = self.CyPlayerAct
		iTeamAct = self.iTeamAct
		CyTeamAct = self.CyTeamAct

		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2, uFont1 = self.aFontList

		iWidGen			= WidgetTypes.WIDGET_GENERAL
		iPanelStd		= PanelStyles.PANEL_STYLE_STANDARD
		iPanelBlue50	= PanelStyles.PANEL_STYLE_BLUE50
		iPanelIn		= PanelStyles.PANEL_STYLE_IN
		iPanelOut		= PanelStyles.PANEL_STYLE_OUT
		iFontTitle		= FontTypes.TITLE_FONT

		aPlayerList = [iPlayerAct] + self.aPlayerList
		iNumEntries = len(aPlayerList)

		W_CELL = 104
		H_CELL = 72
		SIZE = 64
		X_OFFSET = W_CELL / 2 - SIZE / 2

		H_LEFT_LEADER = H_CELL * iNumEntries

		X_TABLE = 2 * W_CELL
		W_TOP_LEADER = W_CELL * iNumEntries

		iResX = self.iResolutionX
		dx = iResX / 64
		x = dx
		y = self.Y_TOP_PAGE + 16
		dx = iResX - 2 * dx
		dy = self.H_PAGE - 32

		# add background panel
		screen.addPanel(self.getNextWidgetName(), "", "", True, True, x, y, dx, dy, iPanelBlue50)
		ScrollPanel = self.getNextWidgetName()
		screen.addScrollPanel(ScrollPanel, "", x, y, dx, dy, iPanelStd)
		# add left column leader panel
		LeftPanel = self.getNextWidgetName()
		screen.attachPanelAt(ScrollPanel, LeftPanel, "", "", True, False, iPanelIn, 0, H_CELL, X_TABLE, H_LEFT_LEADER, iWidGen, 0, 0)
		# add top column leader panel
		TopPanel = self.getNextWidgetName()
		screen.attachPanelAt(ScrollPanel, TopPanel, "", "", False, False, iPanelIn, X_TABLE, 0, W_TOP_LEADER, H_CELL, iWidGen, 0, 0)
		# add Mid panel
		MidPanel = self.getNextWidgetName()
		screen.attachPanelAt(ScrollPanel, MidPanel, "", "", False, False, iPanelOut, X_TABLE, H_CELL, W_TOP_LEADER, H_LEFT_LEADER, iWidGen, 0, 0)

		for iRow, iPlayerX in enumerate(aPlayerList):
			CyPlayerX = GC.getPlayer(iPlayerX)
			iTeamX = CyPlayerX.getTeam()
			CyTeamX = GC.getTeam(iTeamX)

			iEP = CyTeamAct.getEspionagePointsAgainstTeam(iTeamX)
			if iTeamAct != iTeamX:
				iEP_X = CyTeamX.getEspionagePointsAgainstTeam(iTeamAct)
			else:
				iEP_X = -1
			if iEP > iEP_X * 1.1 or bDebug:
				bSeeDetails = True
			else:
				bSeeDetails = False

			artPath = GC.getLeaderHeadInfo(CyPlayerX.getLeaderType()).getButton()

			# top leader icon
			screen.setImageButtonAt(self.getNextWidgetName(), TopPanel, artPath, iRow * W_CELL + X_OFFSET, 0, SIZE, SIZE, iWidGen, iPlayerX, 0)
			# left leader icon
			screen.setImageButtonAt(self.getNextWidgetName(), LeftPanel, artPath, 0, H_CELL * iRow, SIZE, SIZE, iWidGen, iPlayerX, 0)

			# show total EP spending over the last turn
			if bSeeDetails:
				iSpending = 0
				for iTeamZ in xrange(GC.getMAX_PC_TEAMS()):
					if iTeamZ == iTeamX: continue
					CyTeamZ = GC.getTeam(iTeamZ)
					if not CyTeamZ.isAlive(): continue
					if CyTeamX.isHasMet(iTeamZ):
						iSpending += CyPlayerX.getEspionageSpending(iTeamZ)
				szText = ""
				if iSpending > 0:
					szText = "<color=0,255,0>+"
				elif iSpending < 0:
					szText = "<color=255,0,0>"
				szText += str(iSpending)
			else:
				szText = "<color=255,255,0>?"
			y1 = H_CELL + iRow * H_CELL
			y2 = y1 - H_CELL / 2 - 6
			screen.setLabelAt(self.getNextWidgetName(), LeftPanel, szText, 1<<1, X_TABLE - 22, y2 - 4, 0, iFontTitle, iWidGen, 0, 0)

			# add a cell panel to make the table easier to read
			if not (iRow + 1) % 2:
				screen.attachPanelAt(ScrollPanel, self.getNextWidgetName(), "", "", True, True, iPanelStd, X_TABLE, y1, W_CELL, H_CELL, iWidGen, 0, 0)

			for iCol, iPlayerY in enumerate(aPlayerList):
				CyPlayerY = GC.getPlayer(iPlayerY)
				iTeamY = CyPlayerY.getTeam()

				# add a cell panel to make the table easier to read
				if iCol == iRow:
					screen.attachPanelAt(ScrollPanel, self.getNextWidgetName(), "", "", True, True, iPanelIn, X_TABLE + iCol * W_CELL, y1, W_CELL, H_CELL, iWidGen, 0, 0)
				elif iCol and not (iCol + 1 + iRow) % 2:
					screen.attachPanelAt(ScrollPanel, self.getNextWidgetName(), "", "", True, True, iPanelStd, X_TABLE + iCol * W_CELL, y1, W_CELL, H_CELL, iWidGen, 0, 0)

				if iTeamY == iTeamX: continue

				# EPs per turn
				if bSeeDetails:
					if CyTeamX.isHasMet(iTeamY):
						iSpending = CyPlayerX.getEspionageSpending(iTeamY)
					else:
						iSpending = 0
					szText = ""
					if iSpending > 0:
						szText = "<color=0,255,0>"
					elif iSpending < 0:
						szText = "<color=255,0,0>"
					szText += str(iSpending)
				else:
					szText = "<color=255,255,0>?"
				iX = iCol * W_CELL + W_CELL / 2
				screen.setLabelAt(self.getNextWidgetName(), MidPanel, szText, 1<<2, iX, y2, 0, iFontTitle, iWidGen, 0, 0)


	def getNextWidgetName(self):
		szName = "WidgetEA" + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName


	def deleteAllWidgets(self, screen):
		i = self.nWidgetCount - 1
		while (i >= 0):
			self.nWidgetCount = i
			screen.deleteWidget(self.getNextWidgetName())
			i -= 1
		self.nWidgetCount = 0


	def redraw(self, screen):
		iRedrawCount = self.iRedrawCount
		# Timer - A delay is needed to give the dll time to set the value of the relevant variables before we ask what those variables are.
		# e.g. We just told the dll that the espionage weights are changed, but it takes some time before the dll adjust to the new weights.
		# So we can't ask the dll right away how many EP are invested in another player per turn because those values haven't been recalculated yet.
		if iRedrawCount == 1:
			if self.bRefreshLeftPanel:
				self.refreshMissionTab_Left(screen, self.CyPlayerAct)
				self.bRefreshLeftPanel = False

			screen.setForcedRedraw(False)
		else: # tick tock
			self.iRedrawCount += 1

	def handleInput(self, inputClass):
		screen = CyGInterfaceScreen("EspionageAdvisor", CvScreenEnums.ESPIONAGE_ADVISOR)
		iNotifyCode = inputClass.eNotifyCode
		FUNCTION = inputClass.szFunctionName
		ID = inputClass.iItemID

		if iNotifyCode == NotifyCode.NOTIFY_CLICKED:
			if FUNCTION == "EA_Tab":
				self.deleteAllWidgets(screen)
				screen.hide("EA_Col_Tab" + str(self.iTab))
				screen.show("EA_Tab" + str(self.iTab))
				screen.hide("EA_Tab" + str(ID))
				screen.show("EA_Col_Tab" + str(ID))
				self.iTab = ID
				if ID == 0:
					self.drawMissionTab(screen)
				elif ID == 1:
					screen.deleteWidget("CityMissionTable0")
					screen.deleteWidget("CityMissionTable1")
					screen.deleteWidget("CityMissionList0")
					screen.deleteWidget("CityMissionList1")
					self.drawSpyvSpyTab(screen)
				return 1
			iTargetPlayer = self.iTargetPlayer
			if self.iTab == 0 and iTargetPlayer != -1:
				szSplit = FUNCTION.split("|")
				if FUNCTION == "LeaderBtn":
					if ID != self.iTargetPlayer:
						screen.setState("LeaderBtn" + str(self.iTargetPlayer), False)
						self.iTargetPlayer = ID
						self.iCityActID = -1
						self.refreshMissionTab_Right(screen)
					screen.setState("LeaderBtn" + str(ID), True)
					return 1
				if FUNCTION == "ListHeader":
					if ID:
						screen.hide("ListHeader1")
						screen.hide("CityTableHeader")
						screen.hide("CityMissionList1")
						screen.show("ListHeader0")
						screen.show("MissionTableHeader")
					else:
						screen.hide("ListHeader0")
						screen.hide("MissionTableHeader")
						screen.show("CityMissionList1")
						screen.show("ListHeader1")
						screen.show("CityTableHeader")
					self.bMissionList = not self.bMissionList
					self.refreshMissionTab_Right(screen)
					return 1
				if szSplit[0] == "WeightBtn":
					iTeam = inputClass.iData1
					CyPlayerAct = self.CyPlayerAct
					iWeight = CyPlayerAct.getEspionageSpendingWeightAgainstTeam(iTeam)
					if szSplit[1] == "More":
						if iWeight == 99: return 1
						iWeight += 1
						CyMessageControl().sendEspionageSpendingWeightChange(iTeam, 1)
					elif iWeight > 0:
						iWeight -= 1
						CyMessageControl().sendEspionageSpendingWeightChange(iTeam, -1)
					else: return 1
					LABEL = "SpyWeight" + str(ID)
					screen.hide(LABEL)
					screen.modifyLabel(LABEL, self.aFontList[4] + str(iWeight), 1<<0)
					screen.show(LABEL)
					self.bRefreshLeftPanel = True
					self.iRedrawCount = 0
					screen.setForcedRedraw(True)
					return 1
			return 0
		elif iNotifyCode == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:
			if FUNCTION == "CityMissionList":
				if ID:
					self.iMissionAct = inputClass.getData1()
				else:
					self.iCityActID = inputClass.getData1()
				self.refreshMissionTab_Right(screen)
				return 1
			##### Debug Dropdown #####
			if FUNCTION == "DebugDropdown":
				iIndex = screen.getSelectedPullDownID("DebugDropdown")
				self.iPlayerAct = screen.getPullDownData("DebugDropdown", iIndex)
				return 1
		return 0
