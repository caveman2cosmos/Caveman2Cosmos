from CvPythonExtensions import *
from CvScreensInterface import pediaJumpToTech
import HandleInputUtil

GC = CyGlobalContext()

class WBTechScreen:

	def __init__(self, WB):
		self.WB = WB
		self.techs = None
		self.iSelectedEra = -1
		self.bAllPlayers = False
		self.bNoNPC = True

	def interfaceScreen(self):
		self.aFontList = self.WB.aFontList
		self.iTeam = iTeam = self.WB.m_iCurrentTeam
		self.pTeam = GC.getTeam(iTeam)
		xRes = self.WB.xRes
		yRes = self.WB.yRes

		TRNSLTR = CyTranslator()
		self.positive = TRNSLTR.getText("[COLOR_POSITIVE_TEXT]", ())
		self.negative = TRNSLTR.getText("[COLOR_WARNING_TEXT]", ())
		self.sNotNPC = TRNSLTR.getText("TXT_KEY_WB_NOT_NPC", ())
		self.sAllPlayers = TRNSLTR.getText("TXT_KEY_WB_ALL_PLAYERS", ())

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT
		eFontGame = FontTypes.GAME_FONT
		eFontSmall = FontTypes.SMALL_FONT

		screen = self.WB.getScreen()
		screen.setRenderInterfaceOnly(True)
		screen.setForcedRedraw(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.addPanel("SubScreenBG", "", "", False, False, -10, -10, xRes + 20, yRes + 20, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel("topBar", "", "", True, False, 0, 0, xRes, 40, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("botBar", "", "", True, False, 0, yRes - 44, xRes, 44, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		self.aWidgetBucket = ["SubScreenBG"]
		self.aWidgetBucket.append("topBar")
		self.aWidgetBucket.append("botBar")

		screen.setText("ExitSubScreen0", "", "<font=4b>" + TRNSLTR.getText("TXT_WORD_EXIT", ()), 1<<1, xRes - 16, 0, 0, eFontTitle, eWidGen, 0, 1)
		self.aWidgetBucket.append("ExitSubScreen0")

		DDB = "CurrentTeam"
		self.aWidgetBucket.append(DDB)
		wDDB = 256
		screen.addDropDownBoxGFC(DDB, 8, -1, wDDB, eWidGen, 0, 1, eFontSmall)

		for i in xrange(GC.getMAX_TEAMS()):
			loopTeam = GC.getTeam(i)
			if loopTeam.isAlive():
				if loopTeam.isNPC():
					sName = GC.getPlayer(loopTeam.getLeaderID()).getCivilizationDescription(0)
				else: sName = GC.getPlayer(loopTeam.getLeaderID()).getName()

				if loopTeam.getNumMembers() > 1:
					sName += " (" + str(GC.getTeam(i).getNumMembers()) + " " + TRNSLTR.getText("TXT_KEY_MEMBERS_TITLE", ()) + ")"
				screen.addPullDownString(DDB, sName, i, i, i == iTeam)

		DDB = "CurrentPage"
		self.aWidgetBucket.append(DDB)
		screen.addDropDownBoxGFC(DDB, xRes/2 - wDDB/2, -1, wDDB, eWidGen, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_WB_PLAYER_DATA", ()), 0, 0, False)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_WB_TEAM_DATA", ()), 1, 1, False)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_PROJECT", ()), 2, 2, False)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_TECH", ()), 3, 3, True)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + TRNSLTR.getText("TXT_KEY_CONCEPT_CITIES", ()), 4, 4, False)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

		if self.bNoNPC:
			txt = self.positive
		else: txt = self.negative
		txt += "<font=3b>" + self.sNotNPC
		x = xRes - CyInterface().determineWidth(txt) - 16
		y = yRes - 32

		screen.setText("BTN|NoNPC0", "", txt, 1<<0, x, y, 0, eFontGame, eWidGen, 0, 1)
		self.aWidgetBucket.append("BTN|NoNPC0")

		if self.bAllPlayers:
			txt = self.positive
		else: txt = self.negative
		txt += "<font=3b>" + self.sAllPlayers

		screen.setText("BTN|AllPlayers0", "", txt, 1<<1, x - 8, y, 0, eFontGame, eWidGen, 0, 1)
		self.aWidgetBucket.append("BTN|AllPlayers0")

		x = xRes / 2
		screen.setLabel("AllTechs", "", "<font=3b>" + TRNSLTR.getText("TXT_KEY_WB_ALL_TECHS", ()), 1<<1, x, y + 2, 0, eFontGame, eWidGen, 0, 1)
		self.aWidgetBucket.append("AllTechs")

		screen.setButtonGFC("BTN|AllTechs|Plus", "", "", x + 8, y, 28, 28, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("BTN|AllTechs|Minus", "", "", x + 40, y, 28, 28, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		self.aWidgetBucket.append("BTN|AllTechs|Plus")
		self.aWidgetBucket.append("BTN|AllTechs|Minus")

		iNumEras = GC.getNumEraInfos()
		DDB = "TechEra"
		self.aWidgetBucket.append(DDB)
		screen.addDropDownBoxGFC(DDB, 8, yRes - 33, wDDB, eWidGen, 1, 2, FontTypes.GAME_FONT)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_WB_CITY_ALL", ()), -1, -1, -1 == self.iSelectedEra)
		for i in xrange(iNumEras):
			screen.addPullDownString(DDB, GC.getEraInfo(i).getDescription(), i, i, i == self.iSelectedEra)

		if self.techs is None:
			self.techs = techs = [[] for i in xrange(iNumEras)]
			for i in xrange(GC.getNumTechInfos()):
				info = GC.getTechInfo(i)
				techs[info.getEra()].append((info, i))

		self.placeTechs(screen)

	def placeTechs(self, screen):
		xRes = self.WB.xRes
		yRes = self.WB.yRes
		if xRes > 1700:
			iconSize = 32
		elif xRes > 1400:
			iconSize = 28
		else:
			iconSize = 24
		nCol = xRes / 368
		wCol = (xRes-24) / nCol
		h1 = iconSize + 6
		h2 = iconSize + 2
		font = self.aFontList[3]

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT
		ePanelBlack	= PanelStyles.PANEL_STYLE_MAIN_BLACK25

		ScPnl = "TechScrollPanel"
		screen.addScrollPanel(ScPnl, "", 0, 28, xRes + 8, yRes - 88, PanelStyles.PANEL_STYLE_MAIN)
		self.aWidgetBucket.append(ScPnl)

		if self.iSelectedEra == -1:
			aList = []
			for entry in self.techs:
				aList += entry
		else: aList = self.techs[self.iSelectedEra]
		self.listedTechs = aList

		iMaxRows = (len(aList) + nCol - 1) / nCol

		TXT = "TECH|TXT"
		IMG = "TECH|IMG"
		CELL_0 = "cell"
		x = 0
		y = 0
		iRow = 0
		iCol = 0
		for info, iTech in aList:

			name = font + self.getTechListName(iTech, info)

			CELL = CELL_0 + str(iTech)
			if (iRow + iCol) % 2:
				screen.attachPanelAt(ScPnl, CELL, "", "", True, False, ePanelBlack, x, y, wCol, h2, eWidGen, 999, iTech)
				screen.setStyle(CELL, "Panel_Tan15_Style")
			else:
				screen.attachPanelAt(ScPnl, CELL, "", "", True, False, ePanelBlack, x, y-4, wCol, h1, eWidGen, 999, iTech)

			screen.setImageButtonAt(IMG + str(iTech), CELL, info.getButton(), 0, 0, iconSize, iconSize, eWidGen, 0, 0)
			screen.setTextAt(TXT + str(iTech), CELL, name, 1<<0, iconSize, 0, 3, eFontGame, eWidGen, 999, iTech)
			if iRow + 1 == iMaxRows:
				iCol += 1
				iRow = 0
				x += wCol
				y = 0
			else:
				iRow += 1
				y += iconSize


	def getTechListName(self, iTech, info=None):
		if info is None:
			info = GC.getTechInfo(iTech)
		name = info.getDescription()
		if info.isRepeat():
			iCount = self.pTeam.getTechCount(iTech)
			if iCount:
				name += " (" + str(iCount) +")"
				color = self.positive
			else: color = self.negative

		elif self.pTeam.isHasTech(iTech):
			color = self.positive
		else: color = self.negative

		return color + name


	def editTech(self, screen, iTech, bValue):
		info = GC.getTechInfo(iTech)

		if bValue != self.pTeam.isHasTech(iTech) \
		or info.isRepeat() and (bValue or self.pTeam.getTechCount(iTech)):
			self.pTeam.setHasTech(iTech, bValue, -1, False, False)
			ID = "TECH|TXT" + str(iTech)
			screen.hide(ID)
			screen.modifyString(ID, self.aFontList[3] + self.getTechListName(iTech), 0)
			screen.show(ID)

		iTeam = self.iTeam
		if self.bAllPlayers:
			bNoNPC = self.bNoNPC
			for iTeamX in xrange(GC.getMAX_TEAMS()):
				if iTeamX == iTeam: continue
				pTeamX = GC.getTeam(iTeamX)
				if not pTeamX.isAlive() or bNoNPC and pTeamX.isNPC():
					continue
				pTeamX.setHasTech(iTech, bValue, -1, False, False)


	def handleInput(self, inputClass, screen):

		HandleInputUtil.debugInput(inputClass)
		iCode	= inputClass.eNotifyCode
		iData	= inputClass.iData
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName
		szFlag	= HandleInputUtil.MOUSE_FLAGS.get(inputClass.uiFlags, "UNKNOWN")

		if iCode == 6: # Character
			if iData == 1:
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
			if BASE == "TECH":
				self.WB.tooltip.handle(screen, CyGameTextMgr().getTechHelp(ID, False, True, False, True, -1))

		elif not iCode: # click

			if BASE == "BTN":
				if TYPE == "AllTechs":
					bValue = CASE[0] == "Plus"
					for info, iTech in self.listedTechs:
						self.editTech(screen, iTech, bValue)

				elif TYPE == "AllPlayers":
					self.bAllPlayers = not self.bAllPlayers
					if self.bAllPlayers:
						txt = self.positive
					else: txt = self.negative
					txt += "<font=3b>" + self.sAllPlayers
					STR = "BTN|AllPlayers0"
					screen.hide(STR)
					screen.modifyString(STR, txt, 0)
					screen.show(STR)

				elif TYPE == "NoNPC":
					self.bNoNPC = not self.bNoNPC
					if self.bNoNPC:
						txt = self.positive
					else: txt = self.negative
					txt += "<font=3b>" + self.sNotNPC
					STR = "BTN|NoNPC0"
					screen.hide(STR)
					screen.modifyString(STR, txt, 0)
					screen.show(STR)

			elif inputClass.iData1 == 999:
				if szFlag == "MOUSE_RBUTTONUP":
					bValue = False
				else: bValue = True
				self.editTech(screen, inputClass.iData2, bValue)

			elif NAME == "TECH|IMG":
				pediaJumpToTech([ID])

			elif NAME == "ExitSubScreen":
				self.exit(screen)

		elif iCode == 11: # List Select

			if NAME == "TechEra":
				self.iSelectedEra = screen.getPullDownData(NAME, iData)
				self.placeTechs(screen)

			elif NAME == "CurrentTeam":
				self.iTeam = screen.getPullDownData(NAME, iData)
				self.pTeam = GC.getTeam(self.iTeam)
				self.placeTechs(screen)

			elif NAME == "CurrentPage":
				iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
				if iIndex == 0:
					self.exit(screen)
					import WBPlayerScreen
					WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(self.pTeam.getLeaderID())
				elif iIndex == 1:
					self.exit(screen)
					import WBTeamScreen
					WBTeamScreen.WBTeamScreen(self.WB).interfaceScreen(self.WB.m_iCurrentTeam)
				elif iIndex == 2:
					self.exit(screen)
					import WBProjectScreen
					WBProjectScreen.WBProjectScreen(self.WB).interfaceScreen(self.WB.m_iCurrentTeam)
				elif iIndex == 4:
					self.exit(screen)
					import WBPlayerUnits
					WBPlayerUnits.WBPlayerUnits(self.WB).interfaceScreen(self.pTeam.getLeaderID())
				elif iIndex == 11:
					self.exit(screen)
					import WBInfoScreen
					WBInfoScreen.WBInfoScreen(self.WB).interfaceScreen(self.pTeam.getLeaderID())
		return 1


	def exit(self, screen):
		for widget in self.aWidgetBucket:
			screen.deleteWidget(widget)
		del self.aWidgetBucket

		screen.setRenderInterfaceOnly(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, True)
		screen.setForcedRedraw(True)
		self.WB.inSubScreen = None