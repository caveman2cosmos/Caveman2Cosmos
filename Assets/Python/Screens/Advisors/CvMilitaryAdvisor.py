## Military advisor revised by Toffer90

from CvPythonExtensions import *
import CvScreensInterface as UP
import HandleInputUtil
import UnitGrouper

# globals
GC = CyGlobalContext()
AFM = CyArtFileMgr()
GTM = CyGameTextMgr()
TRNSLTR = CyTranslator()
GAME = GC.getGame()

class CvMilitaryAdvisor:

	def __init__(self, screenId):
		self.screenId = screenId
		self.grouper = UnitGrouper.StandardGrouper()
		self.groupingKeys = ["loc", "type"]
		self.bUnitDetails = False

	def getScreen(self):
		return CyGInterfaceScreen("MilitaryAdvisor", self.screenId)

	def interfaceScreen(self):
		screen = self.getScreen()
		if screen.isActive():
			return
		# Caching
		import InputData
		self.InputData = InputData.instance

		import PythonToolTip
		self.tooltip = PythonToolTip.PythonToolTip()

		self.iPlayer = iPlayer = GAME.getActivePlayer()
		self.CyPlayer = CyPlayer = GC.getActivePlayer()
		self.iTeam = iTeam = GAME.getActiveTeam()
		self.CyTeam = GC.getTeam(iTeam)

		self.HILITE_SQUARE	= AFM.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath()
		self.iconMoves		= u'%c' % GAME.getSymbolID(FontSymbols.MOVES_CHAR)
		self.iconStrength	= u'%c' % GAME.getSymbolID(FontSymbols.STRENGTH_CHAR)
		self.fMoveDenominator = float(GC.getMOVE_DENOMINATOR())
		self.GO_SIZE_MATTERS = GAME.isOption(GameOptionTypes.GAMEOPTION_SIZE_MATTERS)
		self.selectedCivs = [iPlayer]
		self.selectedGroups = set()
		self.selectedUnits = set()

		# Resolution
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
		self.a4thX = a4thX = xRes / 4

		eWidGen = WidgetTypes.WIDGET_GENERAL
		ePnlBlue50	= PanelStyles.PANEL_STYLE_BLUE50
		eFontTitle	= FontTypes.TITLE_FONT

		# Base Screen
		screen.setRenderInterfaceOnly(True)
		screen.showWindowBackground(False)
		screen.setDimensions(0, 0, xRes, yRes)

		# Set the background widget and exit button
		screen.addPanel("MA_BG", "", "", True, True, 0, 0, xRes, yRes, ePnlBlue50)
		screen.setStyle("MA_BG", "Panel_Game_HudMap_Style")
		screen.addPanel("TopPanel", "", "", True, False, -2, -2, xRes+4, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_TOPBAR)
		stTxt = uFontEdge + TRNSLTR.getText("TXT_WORD_EXIT", ())
		screen.setText("MA_Exit", "", stTxt, 1<<1, xRes - 8, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		# Header...
		stTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_MILITARY_ADVISOR_TITLE", ())
		screen.setLabel("MA_Header", "", stTxt, 1<<2, xRes / 2, 2, 0, eFontTitle, eWidGen, 1, 1)

		# Minimap initialization
		self.Y_MAP = Y_MAP = H_EDGE_PANEL + 90
		self.W_MAP = W_MAP = xRes - a4thX - 16
		H_MAP_MAX = yRes - Y_MAP - 10

		MAP = CyMap()

		iMap_W = MAP.getViewportWidth()
		iMap_H = MAP.getViewportHeight()

		self.H_MAP = H_MAP = int(W_MAP * iMap_H * 1.0 / iMap_W)
		if H_MAP > H_MAP_MAX:
			self.W_MAP = W_MAP = int(H_MAP_MAX * iMap_W * 1.0 / iMap_H)
			self.H_MAP = H_MAP = H_MAP_MAX

		self.xUnitList	= xUnitList	= 30 + W_MAP
		self.yMainPnl	= yMainPnl	= Y_MAP - 12
		self.wUnitList	= wUnitList	= xRes - xUnitList - 8
		if wUnitList > 280:
			self.hUnitList = H_MAP_MAX + 18
		else:
			self.hUnitList = H_MAP_MAX - 16

		screen.addPanel("MA_PnlMM", "", "", False, False, 8, yMainPnl, W_MAP + 12, H_MAP + 18, ePnlBlue50)
		screen.initMinimap(14, 14 + W_MAP, Y_MAP, Y_MAP + H_MAP, 0)

		if GAME.isDebugMode():
			screen.updateMinimapSection(True, True)
		else:
			screen.updateMinimapSection(False, False)

		screen.updateMinimapColorFromMap(MinimapModeTypes.MINIMAPMODE_TERRITORY, 0.3)
		screen.setMinimapMode(MinimapModeTypes.MINIMAPMODE_MILITARY)
		# Set minimap visibility
		screen.updateMinimapVisibility()

		# Unit grouping dropboxes
		x1 = xUnitList - 2
		y1 = Y_MAP - 6
		w = wUnitList
		if w > 280:
			w = w / 2 - 3
			x2 = x1 + w + 6
			y2 = y1
		else:
			x2 = x1
			y2 = y1 + self.hUnitList + 32
		screen.addDropDownBoxGFC("MA_DDBox0", x1, y1, w, WidgetTypes.WIDGET_GENERAL, 1, 1, FontTypes.GAME_FONT)
		screen.addDropDownBoxGFC("MA_DDBox1", x2, y2, w, WidgetTypes.WIDGET_GENERAL, 1, 1, FontTypes.GAME_FONT)
		for grouping in self.grouper:
			screen.addPullDownString("MA_DDBox0", grouping.title, grouping.index, grouping.index, grouping.key == self.groupingKeys[0])
			screen.addPullDownString("MA_DDBox1", grouping.title, grouping.index, grouping.index, grouping.key == self.groupingKeys[1])

		self.UL_refresh(screen, True)

		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)


	def UL_refresh(self, screen, bReload):
		xRes = self.xRes
		yTop = self.H_EDGE_PANEL
		CyTeam = self.CyTeam
		HILITE_SQUARE = self.HILITE_SQUARE
		selectedCivs = self.selectedCivs

		eWidGen = WidgetTypes.WIDGET_GENERAL
		ePnlBlue50	= PanelStyles.PANEL_STYLE_BLUE50

		# Set scrollable area for leaders
		y = yTop - 4
		w = xRes - 16
		screen.addPanel("MA_CivSelBG", "", "", False, True, 8, y, w, 88, ePnlBlue50)
		ScPnl = "MA_CivSelPnl"
		screen.addScrollPanel(ScPnl, "", 8, y, w, 64, ePnlBlue50)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")

		# loop through all players and display leaderheads
		x = 4
		dx = 72
		for iPlayerX in range(GC.getMAX_PLAYERS()):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive() and CyTeam.isHasMet(CyPlayerX.getTeam()):

				BTN = GC.getCivilizationInfo(CyPlayerX.getCivilizationType()).getButton()

				ID = str(iPlayerX)
				screen.setImageButtonAt("WID|LEADER|Base" + ID, ScPnl, BTN, x, 5, 64, 64, eWidGen, 1, 1)
				Img = "WID|LEADER|HiLi" + ID
				screen.setImageButtonAt(Img, ScPnl, HILITE_SQUARE, x-3, 2, 70, 70, eWidGen, 1, 1)

				if iPlayerX not in selectedCivs:
					screen.hide(Img)
				x += dx

		self.UL_refreshUnitSelection(bReload, True)

	def UL_refreshUnitSelection(self, bReload, bRedraw):
		iTeam = self.iTeam
		CyTeam = self.CyTeam
		screen = self.getScreen()
		screen.minimapClearAllFlashingTiles()
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eWidHiLiMiMa = WidgetTypes.WIDGET_MINIMAP_HIGHLIGHT

		if self.bUnitDetails:
			iconMoves = self.iconMoves
			iconStrength = self.iconStrength
			GO_SIZE_MATTERS = self.GO_SIZE_MATTERS
			fMoveDenominator = self.fMoveDenominator
		if bRedraw:
			iBtn_X = self.xUnitList + 8
			iBtn_Y = self.yMainPnl + 38
			iTxt_X = iBtn_X + 22
			iTxt_Y = iBtn_Y + 2
			if self.bUnitDetails:
				szText = TRNSLTR.getText("TXT_KEY_MILITARY_ADVISOR_UNIT_TOGGLE_OFF", ())
				screen.setButtonGFC("MA_UnitButton", "", "", iBtn_X, iBtn_Y, 20, 20, eWidGen, 1, 1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
				screen.setLabel("MA_UnitBtnLbl", "", szText, 1<<0, iTxt_X, iTxt_Y, 0, FontTypes.GAME_FONT, eWidGen, 1, 1)
			else:
				szText = TRNSLTR.getText("TXT_KEY_MILITARY_ADVISOR_UNIT_TOGGLE_ON", ())
				screen.setButtonGFC("MA_UnitButton", "", "", iBtn_X, iBtn_Y, 20, 20, eWidGen, 1, 1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
				screen.setLabel("MA_UnitBtnLbl", "", szText, 1<<0, iTxt_X, iTxt_Y, 0, FontTypes.GAME_FONT, eWidGen, 1, 1)

		if bReload:
			CyPlayer = self.CyPlayer
			self.stats = UnitGrouper.GrouperStats(self.grouper)
			for iPlayerX in range(GC.getMAX_PLAYERS()):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if not CyPlayerX.isAlive(): continue

				for CyUnit in CyPlayerX.units():
					if not CyUnit.isDead() and CyUnit.getVisualOwner() in self.selectedCivs:
						if not CyUnit.isInvisible(iTeam, False) and CyUnit.plot().isVisible(iTeam, False):
							self.stats.processUnit(CyPlayer, CyTeam, CyUnit)

		iColCyan	= GC.getInfoTypeForString("COLOR_CYAN")
		iColRed		= GC.getInfoTypeForString("COLOR_RED")
		iColWhite	= GC.getInfoTypeForString("COLOR_WHITE")
		iGroupID = 1
		szText = "<u>" + TRNSLTR.getText("TXT_KEY_PEDIA_ALL_UNITS", ()).upper()
		bAllSelected = iGroupID in self.selectedGroups
		if bAllSelected:
			szText = TRNSLTR.changeTextColor(szText, iColCyan)
		if bRedraw:
			screen.addListBoxGFC("MA_UnitList", "", self.xUnitList, self.yMainPnl + 62, self.wUnitList, self.hUnitList - 64, TableStyles.TABLE_STYLE_STANDARD)
			screen.enableSelect("MA_UnitList", False)
			screen.setStyle("MA_UnitList", "Table_StandardCiv_Style")
			screen.appendListBoxString("MA_UnitList", szText, eWidHiLiMiMa, 1, iGroupID, 1<<0)
		else:
			screen.setListBoxStringGFC("MA_UnitList", 0, szText, eWidHiLiMiMa, 1, iGroupID, 1<<0)

		grouping1 = self.stats.getGrouping(self.groupingKeys[0])
		grouping2 = self.stats.getGrouping(self.groupingKeys[1])

		MAP = CyMap()
		iItem = 1
		for group1 in grouping1.itergroups():
			if group1.isEmpty():
				continue
			if iItem > 1:
				screen.appendListBoxString("MA_UnitList", " ", eWidGen, 1, 1, 1<<0)
				iItem += 1
			units1 = group1.units
			iGroupID += 1
			bGroup1Selected = iGroupID in self.selectedGroups
			szTxt = group1.group.title.upper() + " (%d)" % len(units1)
			if bGroup1Selected:
				szTxt = "<u>" + szTxt
			if bGroup1Selected or bAllSelected:
				szTxt = TRNSLTR.changeTextColor(szTxt, iColCyan)
			if bRedraw:
				screen.appendListBoxString("MA_UnitList", szTxt, eWidHiLiMiMa, 1, iGroupID, 1<<0)
			else:
				screen.setListBoxStringGFC("MA_UnitList", iItem, szTxt, eWidHiLiMiMa, 1, iGroupID, 1<<0)
			iItem += 1
			bGroup1Selected = bGroup1Selected or bAllSelected
			iTemp = iGroupID
			for group2 in grouping2.itergroups():
				units2 = group2.units & units1
				if not units2:
					continue
				if self.bUnitDetails and iTemp != iGroupID:
					screen.appendListBoxString("MA_UnitList", " ", eWidGen, 1, 1, 1<<0)
					iItem += 1
				iGroupID += 1
				bGroup2Selected = iGroupID in self.selectedGroups
				szTxt = group2.group.title + " (%d)" % len(units2)
				if bGroup2Selected:
					szTxt = "<u>" + szTxt
				if bGroup2Selected or bGroup1Selected:
					szTxt = TRNSLTR.changeTextColor(szTxt, iColCyan)
				if bRedraw:
					screen.appendListBoxString("MA_UnitList", szTxt, eWidHiLiMiMa, 1, iGroupID, 1<<0)
				else:
					screen.setListBoxStringGFC("MA_UnitList", iItem, szTxt, eWidHiLiMiMa, 1, iGroupID, 1<<0)
				iItem += 1

				bGroup2Selected = bGroup2Selected or bGroup1Selected
				for unit in units2:
					CyUnit = unit.unit
					iPlayerX = CyUnit.getOwner()
					iUnitID = CyUnit.getID()
					bUnitSelected = (iPlayerX, iUnitID) in self.selectedUnits
					if self.bUnitDetails:
						szTxt = CyUnit.getName()
						iMovesBase = CyUnit.baseMoves()
						if iMovesBase:
							szTxt += " "
							iMovesLeft = CyUnit.movesLeft()
							if iMovesLeft == iMovesBase * fMoveDenominator:
								szTxt1 = ""
							else:
								szTxt1 = " %.1f" %(iMovesLeft / fMoveDenominator)

							if szTxt1:
								if szTxt1[-1] == "0":
									szTxt1 = szTxt1[:-2]
								szTxt += szTxt1 + "/"
							szTxt += str(iMovesBase) + iconMoves

						if CyUnit.getDomainType() == DomainTypes.DOMAIN_AIR:
							strengthBase = CyUnit.airBaseCombatStr()
						else:
							strengthBase = CyUnit.baseCombatStr()

						if strengthBase:
							szTxt += " "
							if GO_SIZE_MATTERS:
								strengthBase /= 100.0

							szTxt1 = " %.1f" % strengthBase
							if szTxt1[-1] == "0":
								szTxt1 = szTxt1[:-2]

							if CyUnit.isHurt():
								fPercentHP = float(CyUnit.getHP()) / CyUnit.getMaxHP()
								fStrength = strengthBase * fPercentHP
								szTxt2 = " %.1f" % fStrength
								if szTxt2[-1] == "0":
									szTxt2 = szTxt2[:-2]
								szTxt += szTxt2 + "/"
							szTxt += szTxt1 + iconStrength

						if bUnitSelected or bGroup2Selected:
							if bUnitSelected:
								szTxt = "<u>" + szTxt
							szTxt = TRNSLTR.changeTextColor(szTxt, iColCyan)

						if bRedraw:
							screen.appendListBoxString("MA_UnitList", szTxt, eWidHiLiMiMa, -iPlayerX, iUnitID, 1<<0)
						else:
							screen.setListBoxStringGFC("MA_UnitList", iItem, szTxt, eWidHiLiMiMa, -iPlayerX, iUnitID, 1<<0)
						iItem += 1

					iPlayerX = CyUnit.getVisualOwner()
					CyPlayerX = GC.getPlayer(iPlayerX)
					iColor = GC.getPlayerColorInfo(CyPlayerX.getPlayerColor()).getColorTypePrimary()
					iX = MAP.getViewportXFromMapX(CyUnit.getX())
					iY = MAP.getViewportYFromMapY(CyUnit.getY())
					screen.setMinimapColor(MinimapModeTypes.MINIMAPMODE_MILITARY, iX, iY, iColor, 0.6)
					if bUnitSelected or bGroup2Selected and iPlayerX in self.selectedCivs:

						iTeamX = CyPlayerX.getTeam()
						if CyTeam.isAtWar(iTeamX):
							iColor = iColRed
						elif iTeamX != iTeam:
							iColor = iColCyan
						else:
							iColor = iColWhite
						screen.minimapFlashPlot(iX, iY, iColor, -1)


	# Called from CvScreensInterface
	def refreshSelectedGroup(self, iSelected):
		if iSelected in self.selectedGroups:
			self.selectedGroups.remove(iSelected)
		else:
			self.selectedGroups.add(iSelected)
		self.UL_refreshUnitSelection(False, False)

	def refreshSelectedUnit(self, iPlayer, iUnitId):
		selectedUnit = (iPlayer, iUnitId)
		if selectedUnit in self.selectedUnits:
			self.selectedUnits.remove(selectedUnit)
		else:
			self.selectedUnits.add(selectedUnit)
		self.UL_refreshUnitSelection(False, False)

	def minimapClicked(self):
		CyMap().closeAdvisor(self.xRes, 14, 14 + self.W_MAP, self.Y_MAP, self.Y_MAP + self.H_MAP)
		screen = self.getScreen()
		screen.minimapClearAllFlashingTiles()
		screen.hideScreen()

	#--------------------------#
	# Base operation functions #
	#||||||||||||||||||||||||||#
	def update(self, fDelta):
		if self.tooltip.bLockedTT:
			self.tooltip.handle(self.getScreen())

	def handleInput (self, inputClass):
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

		# Remove potential Help Text
		self.tooltip.reset(screen)

		if iCode == NotifyCode.NOTIFY_CURSOR_MOVE_ON:

			if BASE == "WID":
				if TYPE == "LEADER":
					CyPlayer = GC.getPlayer(ID)
					szLeader = GC.getLeaderHeadInfo(CyPlayer.getLeaderType()).getDescription()

					if CyPlayer.isHuman():
						szTxt = CyPlayer.getName()
						if szTxt == szLeader:
							szTxt = TRNSLTR.getText("TXT_KEY_PITBOSS_HUMAN", ())
					elif CyPlayer.isNPC():
						szTxt = "NPC"
					else:
						szTxt = TRNSLTR.getText("TXT_KEY_MAIN_MENU_AI", ())

					szTxt += "\n%s %s\n" %(CyPlayer.getCivilizationAdjective(0), szLeader)
					szTxt += "%s - %s" %(CyPlayer.getCivilizationShortDescription(0), CyPlayer.getCivilizationDescription(0))
					self.tooltip.handle(screen, szTxt)

		elif iCode == NotifyCode.NOTIFY_CLICKED:

			if szFlag == "MOUSE_RBUTTONUP":

				if BASE == "WID":
					if TYPE == "LEADER":
						UP.pediaJumpToLeader([GC.getPlayer(ID).getLeaderType()])

			else:
				if BASE == "WID":
					if TYPE == "LEADER":

						aList = self.selectedCivs
						bChange = True
						if bShift:
							if ID in aList:
								if len(aList) == 1:
									bChange = False
								else:
									aList.remove(ID)
									screen.hide("WID|LEADER|HiLi" + str(ID))
							else:
								aList.append(ID)
								screen.show("WID|LEADER|HiLi" + str(ID))
						else:
							if len(aList) == 1 and ID in aList:
								bChange = False
							else:
								if ID not in aList:
									screen.show("WID|LEADER|HiLi" + str(ID))
								for iPlayerX in aList:
									if iPlayerX != ID:
										screen.hide("WID|LEADER|HiLi" + str(iPlayerX))
								aList = [ID]

						if bChange:
							self.selectedCivs = aList
							self.UL_refreshUnitSelection(True, True)

				elif NAME == "MA_UnitButton":
					self.bUnitDetails = not self.bUnitDetails
					self.UL_refreshUnitSelection(True, True)

		elif iCode == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:

			if NAME == "MA_DDBox":
				key = self.grouper[iData].key
				self.groupingKeys[ID] = key
				self.selectedGroups.clear()
				self.UL_refresh(screen, False)


	def onClose(self):
		del self.iPlayer, self.CyPlayer, self.iTeam, self.CyTeam, self.xRes, self.yRes, self.aFontList, \
			self.InputData, self.GO_SIZE_MATTERS, self.iconMoves, self.iconStrength, self.fMoveDenominator