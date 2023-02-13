from CvPythonExtensions import *
import HandleInputUtil

iSelectedCiv = -1
iSelectedLeader = -1

class WBGameDataScreen:

	def __init__(self, WB):
		self.WB = WB
		global gameDataWB
		gameDataWB = self
		self.iNewPlayer_Y = 80
		self.bHiddenOption = False

	def interfaceScreen(self):
		self.GC = GC = CyGlobalContext()
		self.GAME = GAME = GC.getGame()
		self.TRNSLTR = TRNSLTR = CyTranslator()

		import InputData
		self.InputData = InputData.instance

		# setup pupup handler
		import CvEventInterface
		self.eventMng = CvEventInterface.getEventManager()
		self.eventMng.Events[9998] = ('PullDown', self.applyPullDownData)
		self.eventMng.Events[9999] = ('SpinBox', self.applySpinBoxData)

		self.xRes = xRes = self.WB.xRes
		self.yRes = yRes = self.WB.yRes
		xMid = xRes/2
		self.iFontScale = self.WB.iFontScale
		self.aFontList = self.WB.aFontList
		scaledFont3 = self.aFontList[3]
		scaledFont3b = self.aFontList[2]
		font4b = "<font=4b>"

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT

		self.szColorNay = "<color=255,64,64>"
		self.szColorYay = "<color=64,255,64>"
		self.szHidden = "</color>" + TRNSLTR.getText("TXT_KEY_WB_HIDDEN", ())
		self.szDefault = TRNSLTR.getText("TXT_WORD_DEFAULT", ())
		self.szOk = TRNSLTR.getText("TXT_KEY_MAIN_MENU_OK", ())
		self.szCancel = TRNSLTR.getText("TXT_KEY_POPUP_CANCEL", ())
		self.bInEditBox = False

		screen = self.WB.getScreen()
		screen.setRenderInterfaceOnly(True)
		screen.setForcedRedraw(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		BG = "SubScreenBG"
		screen.addPanel(BG, "", "", False, False, -10, -10, xRes + 20, yRes + 20, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel("topBar", "", "", True, False, 0, 0, xRes, 40, PanelStyles.PANEL_STYLE_TOPBAR)
		botBar = "botBar"
		screen.addPanel(botBar, "", "", True, False, 0, yRes - 44, xRes, 44, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		self.aWidgetBucket = [BG]
		self.aWidgetBucket.append("topBar")
		self.aWidgetBucket.append(botBar)

		screen.setText("ExitSubScreen", "", font4b + TRNSLTR.getText("TXT_WORD_EXIT", ()), 1<<1, xRes - 16, 0, 0, eFontGame, eWidGen, 1, 2)
		self.aWidgetBucket.append("ExitSubScreen")

		wDDB = 256
		DDB = "CurrentPage"
		self.aWidgetBucket.append(DDB)
		screen.addDropDownBoxGFC(DDB, xMid - wDDB/2, -1, wDDB, eWidGen, 1, 2, eFontGame)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_RELIGION", ()), 8, 8, False)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_CONCEPT_CORPORATIONS", ()), 9, 9, False)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_PITBOSS_GAME_OPTIONS", ()), 10, 10, True)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

		#--------------#
		# Game Options #
		#--------------#
		self.szHiddenOptions = scaledFont3b + TRNSLTR.getText("TXT_KEY_WB_SHOW_HIDDEN",())
		szTxt = self.getHiddenOptionsBtnText()
		screen.setTextAt("HiddenOptions", BG, szTxt, 1<<0, 28, 52, 0, eFontGame, eWidGen, 1, 2)

		xPnl = self.placeGameOptions(screen)
		self.aWidgetBucket.append("GameOptionsBG")
		self.aWidgetBucket.append("GameOptions")

		#----------------#
		# Game variables #
		#----------------#
		yPnl = 72
		hPnl = yRes - yPnl - 50
		wPnl = xRes/6 + 128

		valuePnlBG = "valuePanelBG"
		self.aWidgetBucket.append(valuePnlBG)
		screen.addPanel(valuePnlBG, "", "", True, True, xPnl, yPnl, wPnl, hPnl, PanelStyles.PANEL_STYLE_IN)

		valueScrlPnl = "valueScrlPnl"
		self.aWidgetBucket.append(valueScrlPnl)
		screen.addScrollPanel(valueScrlPnl, "", xPnl-4, yPnl-4, wPnl + 4, hPnl - 22, PanelStyles.PANEL_STYLE_MAIN)
		screen.setStyle(valueScrlPnl, "ScrollPanel_Alt_Style")

		x = 3
		y = 0
		dy = 22 + 3 * self.iFontScale
		self.szStartYear = szTxt = scaledFont3 + TRNSLTR.getText("TXT_KEY_WB_START_YEAR", ()) + ': '
		iYear = GAME.getStartYear()
		if iYear < 0:
			szTxt += str(-iYear) + " BC"
		else: szTxt += str(iYear) + " AD"

		screen.setTextAt("SpinBox|Date|StartYear", valueScrlPnl, szTxt, 1<<0, x, y, 0, eFontGame, eWidGen, 1, 2)

		y += dy
		iYear = GAME.getGameTurnYear()
		if iYear < 0:
			sYear = str(-iYear) + " BC"
		else: sYear = str(iYear) + " AD"

		self.szGameTurnYear = szTxt = scaledFont3 + TRNSLTR.getText("TXT_KEY_WB_GAME_YEAR", ("%s",))
		screen.setLabelAt("GameTurnYear", valueScrlPnl, szTxt % sYear, 1<<0, x+2, y+2, 0, eFontGame, eWidGen, 1, 2)

		y += dy
		self.szGameTurn = szTxt = scaledFont3 + TRNSLTR.getText("TXT_KEY_WB_GAME_TURN", ())
		screen.setTextAt("SpinBox|Date|GameTurn", valueScrlPnl, szTxt + str(GAME.getGameTurn()), 1<<0, x, y, 0, eFontGame, eWidGen, 1, 2)

		y += dy
		self.szMaxTurns = szTxt = scaledFont3 + TRNSLTR.getText("TXT_KEY_WB_MAX_TURNS", ())
		screen.setTextAt("SpinBox|Date|MaxTurns", valueScrlPnl, szTxt + str(GAME.getMaxTurns()), 1<<0, x, y, 0, eFontGame, eWidGen, 1, 2)

		y += dy
		self.szEstimateEndTurn = szTxt = scaledFont3 + TRNSLTR.getText("TXT_KEY_WB_ESTIMATED_END_TURN", ("%d",))
		screen.setLabelAt("EstimateEndTurn", valueScrlPnl, szTxt % GAME.getEstimateEndTurn(), 1<<0, x+2, y+2, 0, eFontGame, eWidGen, 1, 2)

		y += dy
		self.szMaxCityElimination = szTxt = scaledFont3 + TRNSLTR.getText("TXT_KEY_WB_MAX_CITY_ELIMINATION", ())
		screen.setTextAt("SpinBox|MaxCityElimination", valueScrlPnl, szTxt + str(GAME.getMaxCityElimination()), 1<<0, x, y, 0, eFontGame, eWidGen, 1, 2)

		y += dy
		self.szTargetScore = szTxt = scaledFont3 + TRNSLTR.getText("TXT_KEY_WB_TARGET_SCORE", ())
		screen.setTextAt("SpinBox|TargetScore", valueScrlPnl, szTxt + str(GAME.getTargetScore()), 1<<0, x, y, 0, eFontGame, eWidGen, 1, 2)

		y += dy
		self.szNukesExploded = szTxt = scaledFont3 + TRNSLTR.getText("TXT_KEY_WB_NUKES_EXPLODED", ())
		screen.setTextAt("SpinBox|NukesExploded", valueScrlPnl, szTxt + str(GAME.getNukesExploded()), 1<<0, x, y, 0, eFontGame, eWidGen, 1, 2)

		y += dy
		self.szTradeRoutes = szTxt = scaledFont3 + TRNSLTR.getText("TXT_KEY_HEADING_TRADEROUTE_LIST", ()) + ": "
		screen.setTextAt("SpinBox|TradeRoutes", valueScrlPnl, szTxt + str(GAME.getTradeRoutes()), 1<<0, x, y, 0, eFontGame, eWidGen, 1, 2)

		y += dy
		self.szCircumnavigatedGlobe = szTxt = scaledFont3 + TRNSLTR.getText("TXT_KEY_WB_CIRCUMNAVIGATED_GLOBE", ())
		iTeam = GAME.getCircumnavigatedTeam()
		szTxt += str(iTeam)
		if iTeam > -1:
			szTxt += " (%s)" % GC.getTeam(iTeam).getName()
		screen.setTextAt("PullDown|CircumnavigatedGlobe", valueScrlPnl, szTxt, 1<<0, x, y, 0, eFontGame, eWidGen, 1, 2)

		#--------------------#
		# New Player Section #
		#--------------------#
		self.placeNewPlayer(screen)
		self.aWidgetBucket.append("AllowsRepeat")
		self.aWidgetBucket.append("WBNewCiv")
		self.aWidgetBucket.append("WBNewLeader")
		self.aWidgetBucket.append("CreatePlayer")
		self.aWidgetBucket.append("NewPlayerHeader")

		#-------------#
		# Script Data #
		#-------------#
		self.szScriptDataBtn = szTxt = font4b + TRNSLTR.getText("TXT_KEY_WB_SCRIPT_DATA", ())
		screen.setTextAt("ScriptData|Btn", botBar, szTxt, 1<<0, 16, 8, 0, eFontGame, eWidGen, 1, 2)

		NAME = "ScriptEditHeader"
		screen.setLabel(NAME, "", szTxt, 1<<2, xMid, 8, 0, eFontGame, eWidGen, 1, 2)
		screen.hide(NAME)
		self.aWidgetBucket.append(NAME)

		NAME = "ScriptEditBG"
		screen.addPanel(NAME, "", "", False, False, -10, -10, xRes + 20, self.yRes + 20, PanelStyles.PANEL_STYLE_MAIN)
		self.aWidgetBucket.append(NAME)
		screen.hide(NAME)

		NAME = "ScriptEditBox"
		self.aWidgetBucket.append(NAME)
		screen.addEditBoxGFC(NAME, 24, 40, xRes - 48, yRes - 80, eWidGen, 1, 2, eFontGame)
		screen.setStyle(NAME, "GFC_Control_MultilineEdit_Style")
		screen.setActivation(NAME, ActivationTypes.ACTIVATE_NORMAL)
		screen.setEditBoxMaxCharCount(NAME, 10000000, 32)
		self.szScriptData = GAME.getScriptData()
		screen.hide(NAME)

		y = yRes-36
		screen.setText("ScriptData|Ok0", "", font4b + self.szOk, 1<<1, xMid - 32, y, 0, eFontGame, eWidGen, 1, 2)
		screen.setText("ScriptData|Cancel0", "", font4b + self.szCancel, 1<<0, xMid + 32, y, 0, eFontGame, eWidGen, 1, 2)
		self.aWidgetBucket.append("ScriptData|Ok0")
		self.aWidgetBucket.append("ScriptData|Cancel0")
		screen.hide("ScriptData|Ok0")
		screen.hide("ScriptData|Cancel0")


	def placeGameOptions(self, screen):
		xRes = self.xRes
		yRes = self.yRes
		scaledFont3 = self.aFontList[3]
		szColorNay = self.szColorNay
		szColorYay = self.szColorYay

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT
		ePanelBlack	= PanelStyles.PANEL_STYLE_MAIN_BLACK25

		bHiddenOption = self.bHiddenOption
		if bHiddenOption:
			szHidden = self.szHidden

		yPnl = 72
		hPnl = yRes - yPnl - 50
		wPnl = xRes/6 + 140
		xPnl = 14
		screen.addPanel("GameOptionsBG", "", "", True, True, xPnl, yPnl, wPnl, hPnl, PanelStyles.PANEL_STYLE_IN)

		w = wPnl - 16
		ScPnl = "GameOptions"
		screen.addScrollPanel(ScPnl, "", 8, 66, w + 26, yRes - 142, PanelStyles.PANEL_STYLE_MAIN)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")

		TXT = "GameOption%d"
		CELL_0 = "cell%d"
		dy = 24 + 3 * self.iFontScale
		h1 = dy + 6
		h2 = dy + 2
		y = -2
		iRow = 0
		for iOption in xrange(self.GC.getNumGameOptionInfos()):
			info = self.GC.getGameOptionInfo(iOption)
			if bHiddenOption or info.getVisible():

				CELL = CELL_0 % iOption
				if iRow % 2:
					screen.attachPanelAt(ScPnl, CELL, "", "", True, False, ePanelBlack, 0, y, w, h2, eWidGen, 999, iOption)
					screen.setStyle(CELL, "Panel_Tan15_Style")
				else:
					screen.attachPanelAt(ScPnl, CELL, "", "", True, False, ePanelBlack, 0, y-4, w, h1, eWidGen, 999, iOption)

				if self.GAME.isOption(iOption):
					szTxt = szColorYay
				else: szTxt = szColorNay

				szTxt += scaledFont3 + info.getDescription()
				if not info.getVisible():
					szTxt += szHidden

				screen.setTextAt(TXT % iOption, CELL, szTxt, 1<<0, 4, 0, 0, eFontGame, eWidGen, 1, 2)
				y += dy
				iRow += 1
		return 2 * xPnl + wPnl


	def placeNewPlayer(self, screen):
		if self.GAME.countCivPlayersEverAlive() == self.GC.getMAX_PC_PLAYERS():
			return
		GAME = self.GAME
		bLeadAnyCiv = GAME.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV)
		szColorNay = self.szColorNay
		szColorYay = self.szColorYay

		sHeaderText = self.TRNSLTR.getText("TXT_KEY_WB_ADD_NEW_PLAYER",())

		iWidth = self.xRes/2 - 40
		iHeight = (self.yRes/2 - self.iNewPlayer_Y - 10) /48 * 24 + 2
		nColumns = 3
		screen.addTableControlGFC("WBNewCiv", nColumns, self.xRes/2 + 20, self.iNewPlayer_Y, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		for i in xrange(nColumns):
			screen.setTableColumnHeader("WBNewCiv", i, "", iWidth/nColumns)

		aList = []
		for item in xrange(self.GC.getNumCivilizationInfos()):
			Info = self.GC.getCivilizationInfo(item)
			if Info.isAIPlayable():
				aList.append([Info.getShortDescription(0), item])
		aList.sort()
		iNumRows = (len(aList) + nColumns - 1) / nColumns
		for i in xrange(iNumRows):
			screen.appendTableRow("WBNewCiv")

		for i in xrange(len(aList)):
			item = aList[i][1]
			Info = self.GC.getCivilizationInfo(item)
			iColumn = i / iNumRows
			iRow = i % iNumRows
			if iSelectedCiv == item:
				sColor = szColorYay
			else: sColor = szColorNay
			sText = "<font=3>" + sColor + aList[i][0] + "</font></color>"
			screen.setTableText("WBNewCiv", iColumn, iRow, sText, Info.getButton(), WidgetTypes.WIDGET_PYTHON, 7872, item, 1<<0)

		iY = self.iNewPlayer_Y + iHeight + 10
		if iSelectedCiv > -1:
			civ = self.GC.getCivilizationInfo(iSelectedCiv)
			sHeaderText = civ.getShortDescription(0)
			screen.addTableControlGFC("WBNewLeader", nColumns, self.xRes/2 + 20, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
			for i in xrange(nColumns):
				screen.setTableColumnHeader("WBNewLeader", i, "", iWidth/nColumns)

			aList = []
			for i in xrange(self.GC.getNumLeaderHeadInfos()):
				if GAME.isLeaderEverActive(i):
					continue
				Info = self.GC.getLeaderHeadInfo(i)
				if bLeadAnyCiv or civ.isLeaders(i):
					aList.append([Info.getDescription(), i])
			aList.sort()
			iNumRows = (len(aList) + nColumns - 1) / nColumns
			for i in xrange(iNumRows):
				screen.appendTableRow("WBNewLeader")

			for i in xrange(len(aList)):
				item = aList[i][1]
				Info = self.GC.getLeaderHeadInfo(item)
				iColumn = i / iNumRows
				iRow = i % iNumRows
				sColor = self.TRNSLTR.getText("[COLOR_WARNING_TEXT]", ())
				if iSelectedLeader == item:
					sColor = self.TRNSLTR.getText("[COLOR_POSITIVE_TEXT]", ())
				sText = "<font=3>" + sColor + aList[i][0] + "</font></color>"
				screen.setTableText("WBNewLeader", iColumn, iRow, sText, Info.getButton(), WidgetTypes.WIDGET_PYTHON, 7876, item, 1<<0)
			if iSelectedLeader > -1:
				sHeaderText += ", " + self.GC.getLeaderHeadInfo(iSelectedLeader).getDescription()
				sText = self.TRNSLTR.getText("[COLOR_SELECTED_TEXT]", ()) + "<font=4b>" + self.TRNSLTR.getText("TXT_KEY_MAIN_MENU_LOADSAVE_CREATE", ()) + "</color></font>"
				screen.setText("CreatePlayer", "", sText, 1<<1, self.xRes - 16, 52, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
		screen.setLabel("NewPlayerHeader", "", "<font=3b>" + sHeaderText + "</font>", 1<<2, self.xRes *3/4, self.iNewPlayer_Y - 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)


	def checkOptions(self, screen, iGameOption):
		if iGameOption == GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV:
			global iSelectedCiv, iSelectedLeader
			iSelectedCiv = -1
			iSelectedLeader = -1
			self.placeNewPlayer(screen)
		elif iGameOption == GameOptionTypes.GAMEOPTION_NO_GOODY_HUTS and self.GAME.isOption(iGameOption):
			CyMapGenerator().eraseGoodies()
		elif iGameOption == GameOptionTypes.GAMEOPTION_NO_VASSAL_STATES and self.GAME.isOption(iGameOption):
			for iTeamX in xrange(self.GC.getMAX_PC_TEAMS()):
				pTeamX = self.GC.getTeam(iTeamX)
				for iTeamY in xrange(self.GC.getMAX_PC_TEAMS()):
					pTeamX.freeVassal(iTeamY)
		elif iGameOption == GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE and self.GAME.isOption(iGameOption):
			for iPlayerX in xrange(self.GC.getMAX_PC_PLAYERS()):
				pPlayerX = self.GC.getPlayer(iPlayerX)
				if pPlayerX.isHuman():
					for cityX in pPlayerX.cities():
						if not cityX.isCapital():
							cityX.kill()
		elif iGameOption == GameOptionTypes.GAMEOPTION_NO_BARBARIANS and self.GAME.isOption(iGameOption):
			pPlayerBarb = self.GC.getPlayer(self.GC.getBARBARIAN_PLAYER())
			pPlayerBarb.killCities()
			pPlayerBarb.killUnits()

	def getHiddenOptionsBtnText(self):
		if self.bHiddenOption:
			szColor = self.szColorYay
		else: szColor = self.szColorNay

		return szColor + self.szHiddenOptions


	# # # # # # # #
	# Pop-Up handler
	def initSpinBox(self, ID, iDefault, iIncrement, iMax, iMin, szTxt):

		w = CyInterface().determineWidth(szTxt) + 32; h = 140
		if w < 160:
			w = 160
		x = self.xRes/2 - w/2 - 16
		y = self.yRes/2 - h/2 - 16

		popup = CyPopup(9999, EventContextTypes.EVENTCONTEXT_SELF, True)
		popup.setUserData((ID, iDefault, None))
		popup.setSize(w, h)
		popup.setPosition(x, y)
		popup.setHeaderString(szTxt, 1<<0)
		popup.createSpinBox(0, "", iDefault, iIncrement, iMax, iMin)
		popup.addButton(self.szOk)
		popup.addButton(self.szCancel)
		popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

	def applySpinBoxData(self, iPlayer, userData, popupReturn=None):
		if popupReturn:
			if popupReturn.getButtonClicked() == 1:
				return
			iValue = popupReturn.getSpinnerWidgetValue(0)
			if iValue == userData[1]:
				return
		else: iValue = userData[1]

		screen = self.WB.getScreen()
		TYPE, CASE, NAME = userData[0]

		if TYPE == "Date":

			if CASE[0] == "StartYear":
				self.GAME.setStartYear(iValue)
				if iValue < 0:
					szTxt = self.szStartYear + str(-iValue) + " BC"
				else: szTxt = self.szStartYear + str(iValue) + " AD"

			else:
				if CASE[0] == "GameTurn":
					self.GAME.setGameTurn(iValue)
					szTxt = self.szGameTurn + str(iValue)
					iGameTurn = iValue
					iMaxTurns = self.GAME.getMaxTurns()

				elif CASE[0] == "MaxTurns":
					self.GAME.setMaxTurns(iValue)
					szTxt = self.szMaxTurns + str(iValue)
					iMaxTurns = iValue
					if iMaxTurns:
						iGameTurn = self.GAME.getGameTurn()

				if iMaxTurns:
					iEstimateEndTurn = iGameTurn + iMaxTurns
					self.GAME.setEstimateEndTurn(iGameTurn + iMaxTurns)
				else:
					iEstimateEndTurn = 0
					gameSpeed = self.GC.getGameSpeedInfo(self.GAME.getGameSpeedType())

					for i in xrange(gameSpeed.getNumTurnIncrements()):
						iEstimateEndTurn += gameSpeed.getGameTurnInfo(i).iNumGameTurnsPerIncrement

					self.GAME.setEstimateEndTurn(iEstimateEndTurn)

				screen.hide("EstimateEndTurn")
				screen.modifyLabel("EstimateEndTurn", self.szEstimateEndTurn % iEstimateEndTurn, 1<<0)
				screen.show("EstimateEndTurn")

			if CASE[0] != "MaxTurns":

				iYear = self.GAME.getGameTurnYear()
				if iYear < 0:
					sYear = str(-iYear) + " BC"
				else: sYear = str(iYear) + " AD"
				screen.hide("GameTurnYear")
				screen.modifyLabel("GameTurnYear", self.szGameTurnYear % sYear, 1<<0)
				screen.show("GameTurnYear")

		elif TYPE == "MaxCityElimination":
			self.GAME.setMaxCityElimination(iValue)
			szTxt = self.szMaxCityElimination + str(iValue)

		elif TYPE == "TargetScore":
			self.GAME.setTargetScore(iValue)
			szTxt = self.szTargetScore + str(iValue)

		elif TYPE == "NukesExploded":
			if userData[-1] is None:
				iChange = iValue - userData[1]
			else:
				iChange = userData[2]
			self.GAME.changeNukesExploded(iChange)
			szTxt = self.szNukesExploded + str(iValue)

		elif TYPE == "TradeRoutes":
			if userData[-1] is None:
				iChange = iValue - userData[1]
			else:
				iChange = userData[2]
			self.GAME.changeTradeRoutes(iChange)
			szTxt = self.szTradeRoutes + str(iValue)

		screen.hide(NAME)
		screen.modifyString(NAME, szTxt, 1<<0)
		screen.show(NAME)


	def initPullDown(self, ID, iDefault, szTxt):

		w = CyInterface().determineWidth(szTxt) + 32; h = 140
		if w < 160:
			w = 160
		x = self.xRes/2 - w/2 - 16
		y = self.yRes/2 - h/2 - 16

		popup = CyPopup(9998, EventContextTypes.EVENTCONTEXT_SELF, False)
		popup.setUserData((ID, iDefault, szTxt))
		popup.setSize(w, h)
		popup.setPosition(x, y)
		popup.setHeaderString(szTxt, 1<<0)
		popup.createPullDown(0)

		if ID[0] == "CircumnavigatedGlobe":
			for i in xrange(-1, self.GC.getMAX_PC_TEAMS()):
				name = ""
				if i > -1:
					name = self.GC.getTeam(i).getName()
					if name:
						name = " - " + name
				popup.addPullDownString(str(i) + name, i, 0)

			popup.setSelectedPulldownID(iDefault, 0)

		popup.addButton(self.szOk)
		popup.addButton(self.szCancel)
		popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

	def applyPullDownData(self, iPlayer, userData, popupReturn):
		if popupReturn.getButtonClicked() == 1:
			return
		iValue = popupReturn.getSelectedPullDownValue(0)
		if iValue == userData[1]:
			return
		screen = self.WB.getScreen()
		TYPE, CASE, NAME = userData[0]
		if TYPE == "CircumnavigatedGlobe":
			self.GAME.setCircumnavigatedTeam(iValue)
			szTxt = userData[2] + str(iValue)
			if iValue > -1:
				szTxt += " (%s)" % self.GC.getTeam(iValue).getName()

		screen.hide(NAME)
		screen.modifyString(NAME, szTxt, 1<<0)
		screen.show(NAME)


	def initEditBox(self, screen):
		screen.setEditBoxString("ScriptEditBox", self.szScriptData)
		screen.moveToFront("ScriptEditBG")
		screen.moveToFront("ScriptEditBox")
		screen.moveToFront("ScriptEditHeader")
		screen.moveToFront("ScriptData|Ok0")
		screen.moveToFront("ScriptData|Cancel0")
		screen.show("ScriptEditBG")
		screen.show("ScriptEditBox")
		screen.show("ScriptEditHeader")
		screen.show("ScriptData|Ok0")
		screen.show("ScriptData|Cancel0")
		self.bInEditBox = True

	def closeEditBox(self, screen, bOk):
		if bOk:
			self.szScriptData = screen.getEditBoxString("ScriptEditBox")
			import TextUtil
			self.GAME.setScriptData(TextUtil.convertToStr(self.szScriptData))
		screen.hide("ScriptEditBG")
		screen.hide("ScriptEditBox")
		screen.hide("ScriptEditHeader")
		screen.hide("ScriptData|Ok0")
		screen.hide("ScriptData|Cancel0")
		self.bInEditBox = False

	# # # # # # #
	# Interaction
	def handleInput(self, inputClass, screen):

		HandleInputUtil.debugInput(inputClass)
		bAlt, bCtrl, bShift = self.InputData.getModifierKeys()
		iCode	= inputClass.eNotifyCode
		iData	= inputClass.iData
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName
		szFlag	= HandleInputUtil.MOUSE_FLAGS.get(inputClass.uiFlags, "UNKNOWN")

		if iCode == 6: # Character
			if iData == 1:
				if self.bInEditBox:
					self.closeEditBox(screen, False)
				else:
					self.exit(screen)
				return 1
			return 0
		elif iCode in (16, 17):
			return 0

		szSplit = NAME.split("|")
		BASE = szSplit[0]
		if szSplit[1:]:
			TYPE = szSplit[1]
		else:
			TYPE = ""
		if szSplit[2:]:
			CASE = szSplit[2:]
		else:
			CASE = [""]

		if iCode == 4: # Mouse Enter

			if NAME == "GameOption":
				self.WB.tooltip.handle(screen, self.GC.getGameOptionInfo(ID).getHelp())

		elif not iCode: # click

			if BASE == "SpinBox":
				if bCtrl:
					if szFlag == "MOUSE_RBUTTONUP":
						iInc = -1
					else: iInc = 1
				aList = (TYPE, CASE, NAME)

				if TYPE == "Date":

					if CASE[0] == "StartYear":
						if bCtrl:
							self.applySpinBoxData(None, (aList, self.GAME.getStartYear() + iInc))
						else:
							self.initSpinBox(aList, self.GAME.getStartYear(), 1, 10000000, -10000000, self.szStartYear)

					elif CASE[0] == "GameTurn":
						if bCtrl:
							iNewValue = self.GAME.getGameTurn() + iInc
							if iNewValue > -1:
								self.applySpinBoxData(None, (aList, iNewValue))
						else:
							self.initSpinBox(aList, self.GAME.getGameTurn(), 1, 10000000, 0, self.szGameTurn)

					elif CASE[0] == "MaxTurns":
						if bCtrl:
							iNewValue = self.GAME.getMaxTurns() + iInc
							if iNewValue > -1:
								self.applySpinBoxData(None, (aList, iNewValue))
						else:
							self.initSpinBox(aList, self.GAME.getMaxTurns(), 1, 10000000, 0, self.szMaxTurns)

				elif TYPE == "MaxCityElimination":
					if bCtrl:
						iNewValue = self.GAME.getMaxCityElimination() + iInc
						if iNewValue > -1:
							self.applySpinBoxData(None, (aList, iNewValue))
					else:
						self.initSpinBox(aList, self.GAME.getMaxCityElimination(), 1, 10000000, 0, self.szMaxCityElimination)

				elif TYPE == "TargetScore":
					if bCtrl:
						iNewValue = self.GAME.getTargetScore() + iInc
						if iNewValue > -1:
							self.applySpinBoxData(None, (aList, iNewValue))
					else:
						self.initSpinBox(aList, self.GAME.getTargetScore(), 1, 10000000, 0, self.szTargetScore)

				elif TYPE == "NukesExploded":
					if bCtrl:
						iNewValue = self.GAME.getNukesExploded() + iInc
						if iNewValue > -1:
							self.applySpinBoxData(None, (aList, iNewValue, iInc))
					else:
						self.initSpinBox(aList, self.GAME.getNukesExploded(), 1, 10000000, 0, self.szNukesExploded)

				elif TYPE == "TradeRoutes":
					if bCtrl:
						iNewValue = self.GAME.getTradeRoutes() + iInc
						if iNewValue > -1:
							self.applySpinBoxData(None, (aList, iNewValue, iInc))
					else:
						self.initSpinBox(aList, self.GAME.getTradeRoutes(), 1, 10000000, 0, self.szTradeRoutes)

			elif BASE == "PullDown":

				aList = (TYPE, CASE, NAME)
				if TYPE == "CircumnavigatedGlobe":
					self.initPullDown(aList, self.GAME.getCircumnavigatedTeam(), self.szCircumnavigatedGlobe)

			elif BASE == "ScriptData":
				if TYPE == "Btn":
					self.initEditBox(screen)

				elif TYPE == "Ok":
					self.closeEditBox(screen, True)

				elif TYPE == "Cancel":
					self.closeEditBox(screen, False)

			elif NAME == "HiddenOptions":
				self.bHiddenOption = not self.bHiddenOption
				screen.hide(NAME)
				screen.modifyString(NAME, self.getHiddenOptionsBtnText(), 1<<0)
				screen.show(NAME)
				self.placeGameOptions(screen)

			elif NAME == "GameOption" or inputClass.iData1 == 999:
				if NAME == "GameOption":
					iOption = ID
				else: iOption = inputClass.iData2

				bNewValue = not self.GAME.isOption(iOption)
				self.GAME.setOption(iOption, bNewValue)

				if bNewValue:
					szTxt = self.szColorYay
				else: szTxt = self.szColorNay

				info = self.GC.getGameOptionInfo(iOption)
				szTxt += self.aFontList[3] + info.getDescription()

				if not info.getVisible():
					szTxt += self.szHidden

				name = "GameOption" + str(iOption)
				screen.hide(name)
				screen.modifyString(name, szTxt, 1<<0)
				screen.show(name)

				self.checkOptions(screen, iOption)

			elif NAME == "ExitSubScreen":
				self.exit(screen)

		elif iCode == 11: # List Select

			if NAME == "CurrentPage":
				iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
				if iIndex == 8:
					self.exit(screen)
					import WBReligionScreen
					WBReligionScreen.WBReligionScreen(self.WB).interfaceScreen(self.WB.iCurrentPlayer)
				elif iIndex == 9:
					self.exit(screen)
					import WBCorporationScreen
					WBCorporationScreen.WBCorporationScreen(self.WB).interfaceScreen(self.WB.iCurrentPlayer)
				elif iIndex == 11:
					self.exit(screen)
					import WBInfoScreen
					WBInfoScreen.WBInfoScreen(self.WB).interfaceScreen(self.WB.iCurrentPlayer)

		global iSelectedCiv, iSelectedLeader

		if NAME == "WBNewCiv":
			iSelectedCiv = inputClass.iData2
			iSelectedLeader = -1
			self.placeNewPlayer(screen)

		elif NAME == "WBNewLeader":
			iSelectedLeader = inputClass.iData2
			self.placeNewPlayer(screen)

		elif NAME == "CreatePlayer":
			for i in xrange(self.GC.getMAX_PC_PLAYERS()):
				if not self.GC.getPlayer(i).isEverAlive():
					self.GAME.addPlayer(i, iSelectedLeader, iSelectedCiv, True)
					break
			self.exit(screen)
			self.WB.iCurrentPlayer = i
			self.WB.normalPlayerTabModeCB()

		return 1


	def update(self, fDelta): return

	# # # # # #
	# Clean Up
	def exit(self, screen):
		for widget in self.aWidgetBucket:
			screen.deleteWidget(widget)
		# Only clear significant stuff, this object is deleted when you exit the worldbuilder anyway.
		del self.aWidgetBucket, self.aFontList, self.eventMng.Events[9998], self.eventMng.Events[9999]

		screen.setRenderInterfaceOnly(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, True)
		screen.setForcedRedraw(True)
		self.WB.inSubScreen = None
