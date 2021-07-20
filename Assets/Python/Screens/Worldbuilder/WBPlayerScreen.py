from CvPythonExtensions import *
import CvScreenEnums
import WBProjectScreen
import WBTeamScreen
import WBPlayerUnits
import WBReligionScreen
import WBCorporationScreen
import WBInfoScreen
import Popup

GC = CyGlobalContext()
iChange = 1

class WBPlayerScreen:

	def __init__(self, WB):
		self.WB = WB
		self.iIconSize = 64

	def interfaceScreen(self, iPlayerX):
		screen = CyGInterfaceScreen( "WBPlayerScreen", CvScreenEnums.WB_PLAYER)
		global iPlayer
		global pPlayer
		global iTeam
		global pTeam
		iPlayer = iPlayerX
		pPlayer = GC.getPlayer(iPlayer)
		iTeam = pPlayer.getTeam()
		pTeam = GC.getTeam(iTeam)

		screen.setRenderInterfaceOnly(True)
		screen.addPanel("MainBG", u"", u"", True, False, -10, -10, screen.getXResolution() + 20, screen.getYResolution() + 20, PanelStyles.PANEL_STYLE_MAIN )
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.setText("PlayerExit", "Background", "<font=4>" + CyTranslator().getText("TXT_WORD_EXIT", ()).upper() + "</font>", 1<<1, screen.getXResolution() - 30, screen.getYResolution() - 42, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		screen.addDropDownBoxGFC("CurrentPage", 20, screen.getYResolution() - 42, screen.getXResolution()/5, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA", ()), 0, 0, True)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_TEAM_DATA", ()), 1, 1, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_PROJECT", ()), 2, 2, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_TECH", ()), 3, 3, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_RELIGION", ()), 8, 8, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_CONCEPT_CORPORATIONS", ()), 9, 9, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()), 4, 4, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

		iY = 20
		screen.addDropDownBoxGFC("CurrentPlayer", 20, iY, screen.getXResolution()/5, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for i in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(i)
			if pPlayerX.isEverAlive():
				sText = pPlayerX.getName()
				if not pPlayerX.isAlive():
					sText = "*" + sText
				if pPlayerX.isTurnActive():
					sText = "[" + sText + "]"
				screen.addPullDownString("CurrentPlayer", sText, i, i, i == iPlayer)

		iY += 30
		screen.addDropDownBoxGFC("ChangeBy", 20, iY, screen.getXResolution()/5, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		i = 1
		while i < 1000001:
			screen.addPullDownString("ChangeBy", "(+/-) " + str(i), i, i, iChange == i)
			if str(i)[0] == "1":
				i *= 5
			else:
				i *= 2

		iY += 30
		screen.addDropDownBoxGFC("CurrentEra", 20, iY, screen.getXResolution()/5, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for i in xrange(GC.getNumEraInfos()):
			screen.addPullDownString("CurrentEra", GC.getEraInfo(i).getDescription(), i, i, i == pPlayer.getCurrentEra())

		global lReligions

		lReligions = []
		for i in xrange(GC.getNumReligionInfos()):
			ItemInfo = GC.getReligionInfo(i)
			lReligions.append([ItemInfo.getDescription() + " (" + str(pPlayer.getHasReligionCount(i)) + ")", i])
		lReligions.sort()

		self.placeStats()
		self.placeCivics()
		self.placeReligions()
		self.placeResearch()
		self.placeScript()

	def placeStats(self):
		screen = CyGInterfaceScreen( "WBPlayerScreen", CvScreenEnums.WB_PLAYER)
		iLeader = pPlayer.getLeaderType()
		iCiv = pPlayer.getCivilizationType()
		screen.addDDSGFC("LeaderPic", GC.getLeaderHeadInfo(iLeader).getButton(), screen.getXResolution() * 3/8 - self.iIconSize/2, 80, self.iIconSize, self.iIconSize, WidgetTypes.WIDGET_PYTHON, 7876, iLeader)
		screen.addDDSGFC("CivPic", GC.getCivilizationInfo(iCiv).getButton(), screen.getXResolution() * 5/8 - self.iIconSize/2, 80, self.iIconSize, self.iIconSize, WidgetTypes.WIDGET_PYTHON, 7872, iCiv)
		sText = pPlayer.getName()
		lTraits = []
		for iTrait in xrange(GC.getNumTraitInfos()):
			if pPlayer.hasTrait(iTrait):
				lTraits.append(iTrait)
		if len(lTraits):
			sText += " ["
			for iTrait in xrange(len(lTraits)):
				sText += CyTranslator().getText(GC.getTraitInfo(lTraits[iTrait]).getShortDescription(),())
				if iTrait != len(lTraits) -1:
					sText += "/"
			sText += "]"
		sText += " (ID: " + str(pPlayer.getID()) + ")"

		screen.setLabel("PlayerName", "Background", "<font=4b>" + sText + "</font>", 1<<2, screen.getXResolution()/2, 20, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabel("CivilizationName", "Background", "<font=4b>" + pPlayer.getCivilizationDescription(0) + "</font>", 1<<2, screen.getXResolution()/2, 50, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY = 110
		screen.setButtonGFC("PlayerGoldPlus", "", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("PlayerGoldMinus", "", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = u"%s %s%c" %(CyTranslator().getText("TXT_KEY_WB_GOLD", ()), self.WB.addComma(pPlayer.getGold()), GC.getCommerceInfo(CommerceTypes.COMMERCE_GOLD).getChar())
		screen.setLabel("PlayerGoldText", "Background", "<font=3>" + sText + "</font>", 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CombatXPPlus", "", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CombatXPMinus", "", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = u"%s: %d/%d" %(CyTranslator().getText("TXT_KEY_MISC_COMBAT_EXPERIENCE", ()), pPlayer.getCombatExperience(), pPlayer.greatPeopleThresholdMilitary())
		screen.setLabel("CombatXPText", "Background", "<font=3>" + sText + "</font>", 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("GoldenAgePlus", "", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("GoldenAgeMinus", "", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = u"%s: %d" %(CyTranslator().getText("TXT_KEY_CONCEPT_GOLDEN_AGE", ()), pPlayer.getGoldenAgeTurns())
		screen.setLabel("GoldenAgeText", "Background", "<font=3>" + sText + "</font>", 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("GPRequiredPlus", "", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("GPRequiredMinus", "", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("TXT_KEY_WB_GOLDEN_AGE_UNITS", (pPlayer.unitsRequiredForGoldenAge(),))
		screen.setLabel("GPRequiredText", "Background", "<font=3>" + sText + "</font>", 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("AnarchyPlus", "", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("AnarchyMinus", "", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("TXT_KEY_WB_ANARCHY", (pPlayer.getAnarchyTurns(),))
		screen.setLabel("AnarchyText", "Background", "<font=3>" + sText + "</font>", 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CoastalTradePlus", "", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CoastalTradeMinus", "", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("TXT_KEY_WB_COASTAL_TRADE", (pPlayer.getCoastalTradeRoutes(),))
		screen.setLabel("CoastalTradeText", "Background", "<font=3>" + sText + "</font>", 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
			iX = 20
			screen.hide("CommerceFlexiblePlus" + str(i))
			screen.hide("CommerceFlexibleMinus" + str(i))
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if pPlayer.isCommerceFlexible(i):
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
				screen.setButtonGFC("CommerceFlexiblePlus" + str(i), "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, i, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
				screen.setButtonGFC("CommerceFlexibleMinus" + str(i), "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, i, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
			sText = sColor + u"<font=3>%c: %d%% %s</color></font>" %(GC.getCommerceInfo(i).getChar(), pPlayer.getCommercePercent(i), CyTranslator().getText("TXT_KEY_MISC_POS_GOLD_PER_TURN", (pPlayer.getCommerceRate(CommerceTypes(i)),)))
			screen.setText("AdjustCommerceFlexible" + GC.getCommerceInfo(i).getType(), "Background", "<font=3>" + sText + "</font>", 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PYTHON, 7881, i)
			iY += 30

	def placeScript(self):
		screen = CyGInterfaceScreen("WBPlayerScreen", CvScreenEnums.WB_PLAYER)
		iX = screen.getXResolution()/4
		iY = screen.getYResolution()/2 - 55
		iWidth = screen.getXResolution()/2
		iHeight = 50
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_SCRIPT_DATA", ()) + "</color></font>"
		screen.setText("PlayerEditScriptData", "Background", sText, 1<<2, screen.getXResolution()/2, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel("ScriptPanel", "", "", False, False, iX, iY + 25, iWidth, iHeight, PanelStyles.PANEL_STYLE_IN)
		screen.addMultilineText("PlayerScriptDataText", pPlayer.getScriptData(), iX, iY + 25, iWidth, iHeight, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

	def placeResearch(self):
		screen = CyGInterfaceScreen("WBPlayerScreen", CvScreenEnums.WB_PLAYER)
		iX = screen.getXResolution()/4
		iY = 110 + self.iIconSize
		iWidth = screen.getXResolution()/2
		iHeight = (screen.getYResolution()/2 - 55 - iY)/24 * 24 + 2
		nColumns = iWidth/240
		screen.addTableControlGFC("WBPlayerResearch", nColumns, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		for i in xrange(nColumns):
			screen.setTableColumnHeader("WBPlayerResearch", i, "", iWidth/nColumns)

		iCurrentTech = pPlayer.getCurrentResearch()
		iCount = 1
		iMaxRows = 0
		sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		if iCurrentTech > -1:
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		screen.appendTableRow("WBPlayerResearch")
		sCurrentTech = CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
		screen.setTableText("WBPlayerResearch", 0, 0, "<font=3>" + sColor + sCurrentTech + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), WidgetTypes.WIDGET_PYTHON, 7871, -1, 1<<0 )
		for iTech in xrange(GC.getNumTechInfos()):
			if pPlayer.canResearch(iTech):
				iColumn = iCount % nColumns
				iRow = iCount /nColumns
				if iRow > iMaxRows:
					screen.appendTableRow("WBPlayerResearch")
					iMaxRows = iRow
				iCount += 1
				ItemInfo = GC.getTechInfo(iTech)
				sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
				sText = u"%s (%d/%d)%c" %(ItemInfo.getDescription(), pTeam.getResearchProgress(iTech), pTeam.getResearchCost(iTech), GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar())
				if iCurrentTech == iTech:
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
					sCurrentTech = sText
				screen.setTableText("WBPlayerResearch", iColumn, iRow, "<font=3>" + sColor + sText + "</color></font>", ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 7871, iTech, 1<<0)

		if iCurrentTech > -1:
			sText = u"%s: %d/%d%c" %(GC.getTechInfo(iCurrentTech).getDescription(), pTeam.getResearchProgress(iCurrentTech), pTeam.getResearchCost(iCurrentTech), GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar())
			screen.setButtonGFC("CurrentResearchPlus", "", "", iX + iWidth - 50, iY - 30, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
			screen.setButtonGFC("CurrentResearchMinus", "", "", iX + iWidth - 25, iY - 30, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		screen.setLabel("CurrentProgressText", "Background", "<font=3b>" + sCurrentTech + "</font>", 1<<2, iX + iWidth/2, iY - 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def placeReligions(self):
		screen = CyGInterfaceScreen("WBPlayerScreen", CvScreenEnums.WB_PLAYER)
		iX = screen.getXResolution() * 3/4 + 20
		iY = 50
		iWidth = screen.getXResolution()/4 - 40
		screen.setLabel("ReligionsHeader", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_RELIGION",()) + "</font>", 1<<2, iX + iWidth/2, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		iY += 30
		iHeight = (screen.getYResolution()/2 - iY - 30)/24 * 24 + 2
		screen.addTableControlGFC("WBPlayerReligions", 1, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBPlayerReligions", 0, "", iWidth)
		sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		if pPlayer.getStateReligion() > -1:
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		screen.appendTableRow("WBPlayerReligions")
		screen.setTableText("WBPlayerReligions", 0, 0, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), WidgetTypes.WIDGET_HELP_RELIGION, -1, -1, 1<<0 )
		for item in lReligions:
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if pPlayer.getStateReligion() == item[1]:
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			elif pPlayer.getHasReligionCount(item[1]) > 0:
				sColor = CyTranslator().getText("[COLOR_YELLOW]", ())
			iRow = screen.appendTableRow("WBPlayerReligions")
			ItemInfo = GC.getReligionInfo(item[1])
			sChar = ItemInfo.getChar()
			if pPlayer.hasHolyCity(item[1]):
				sChar = ItemInfo.getHolyCityChar()
			sText = u"<font=4>%c <font=3>%s</font>" %(sChar, item[0])
			screen.setTableText("WBPlayerReligions", 0, iRow, sColor + sText + "</color>", "", WidgetTypes.WIDGET_HELP_RELIGION, item[1], -1, 1<<0)

		iY += iHeight
		screen.setButtonGFC("StateReligionUnitPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("StateReligionUnitMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_WB_STATE_RELIGION_UNIT",(pPlayer.getStateReligionUnitProductionModifier(),)) + "</font>"
		screen.setLabel("StateReligionUnitText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("StateReligionBuildingPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("StateReligionBuildingMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_WB_STATE_RELIGION_BUILDING",(pPlayer.getStateReligionBuildingProductionModifier(),)) + "</font>"
		screen.setLabel("StateReligionBuildingText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def placeCivics(self):
		screen = CyGInterfaceScreen("WBPlayerScreen", CvScreenEnums.WB_PLAYER)
		iX = 20
		iY = screen.getYResolution()/2 + 30
		iWidth = screen.getXResolution() - 40
		screen.setLabel("CivicsHeader", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_CIVIC",()) + "</font>", 1<<2, iX + iWidth/2, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		iHeight = (screen.getYResolution() - iY - 40) /24 * 24 + 2
		iColumns = min(iWidth/160, GC.getNumCivicOptionInfos())
		screen.addTableControlGFC("WBPlayerCivics", iColumns, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		for i in xrange(iColumns):
			screen.setTableColumnHeader("WBPlayerCivics", i, "", iWidth / iColumns)

		iMaxRow = -1
		iCurrentMaxRow = 0

		for iCivicOption in xrange(GC.getNumCivicOptionInfos()):
			iColumn = iCivicOption % iColumns
			iRow = iCurrentMaxRow
			if iRow > iMaxRow:
				screen.appendTableRow("WBPlayerCivics")
				iMaxRow = iRow
			sText = "<font=3>" + CyTranslator().getText("[COLOR_HIGHLIGHT_TEXT]", ()) + GC.getCivicOptionInfo(iCivicOption).getDescription() + "</font></color>"
			screen.setTableText("WBPlayerCivics", iColumn, iRow, sText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)
			for item in xrange(GC.getNumCivicInfos()):
				ItemInfo = GC.getCivicInfo(item)
				if ItemInfo.getCivicOptionType() != iCivicOption: continue
				sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
				if pPlayer.isCivic(item):
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
				elif pPlayer.canDoCivics(item):
					sColor = CyTranslator().getText("[COLOR_YELLOW]", ())
				iRow += 1
				if iRow > iMaxRow:
					screen.appendTableRow("WBPlayerCivics")
					iMaxRow = iRow
				screen.setTableText("WBPlayerCivics", iColumn, iRow,"<font=3>" + sColor + ItemInfo.getDescription() + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 8205, item, 1<<0)
			if iCivicOption % iColumns == iColumns -1 and iCivicOption < GC.getNumCivicOptionInfos() -1:
				screen.appendTableRow("WBPlayerCivics")
				iCurrentMaxRow = iMaxRow + 2

	def handleInput (self, inputClass):
		screen = CyGInterfaceScreen( "WBPlayerScreen", CvScreenEnums.WB_PLAYER)
		global iChange

		if inputClass.getFunctionName() == "ChangeBy":
			iChange = screen.getPullDownData("ChangeBy", screen.getSelectedPullDownID("ChangeBy"))

		elif inputClass.getFunctionName() == "CurrentPage":
			iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
			if iIndex == 1:
				WBTeamScreen.WBTeamScreen(self.WB).interfaceScreen(iTeam)
			elif iIndex == 2:
				WBProjectScreen.WBProjectScreen(self.WB).interfaceScreen(iTeam)
			elif iIndex == 3:
				self.WB.goToSubScreen("TechScreen")
			elif iIndex == 4:
				WBPlayerUnits.WBPlayerUnits(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 8:
				WBReligionScreen.WBReligionScreen(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 9:
				WBCorporationScreen.WBCorporationScreen(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 11:
				WBInfoScreen.WBInfoScreen(self.WB).interfaceScreen(iPlayer)

		elif inputClass.getFunctionName() == "CurrentPlayer":
			iIndex = screen.getPullDownData("CurrentPlayer", screen.getSelectedPullDownID("CurrentPlayer"))
			self.interfaceScreen(iIndex)

		elif inputClass.getFunctionName() == "CurrentEra":
			pPlayer.setCurrentEra(screen.getPullDownData("CurrentEra", screen.getSelectedPullDownID("CurrentEra")))

		elif inputClass.getFunctionName().find("PlayerGold") > -1:
			if inputClass.getData1() == 1030:
				pPlayer.changeGold(iChange)
			elif inputClass.getData1() == 1031:
				pPlayer.changeGold(- min(iChange, pPlayer.getGold()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CombatXP") > -1:
			if inputClass.getData1() == 1030:
				pPlayer.changeCombatExperience(min(iChange, pPlayer.greatPeopleThresholdMilitary() - pPlayer.getCombatExperience()))
			elif inputClass.getData1() == 1031:
				pPlayer.changeCombatExperience(- min(iChange, pPlayer.getCombatExperience()))
			self.placeStats()

		elif inputClass.getFunctionName().find("GoldenAge") > -1:
			if inputClass.getData1() == 1030:
				pPlayer.changeGoldenAgeTurns(iChange)
			elif inputClass.getData1() == 1031:
				pPlayer.changeGoldenAgeTurns(- min(iChange, pPlayer.getGoldenAgeTurns()))
			self.placeStats()

		elif inputClass.getFunctionName().find("GPRequired") > -1:
			if inputClass.getData1() == 1030:
				pPlayer.changeNumUnitGoldenAges(iChange)
			elif inputClass.getData1() == 1031:
				pPlayer.changeNumUnitGoldenAges(- min(iChange, pPlayer.unitsRequiredForGoldenAge() - 1))
			self.placeStats()

		elif inputClass.getFunctionName().find("Anarchy") > -1:
			if inputClass.getData1() == 1030:
				pPlayer.changeAnarchyTurns(iChange)
			elif inputClass.getData1() == 1031:
				pPlayer.changeAnarchyTurns(- min(iChange, pPlayer.getAnarchyTurns()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CoastalTrade") > -1:
			if inputClass.getData1() == 1030:
				pPlayer.changeCoastalTradeRoutes(iChange)
			elif inputClass.getData1() == 1031:
				pPlayer.changeCoastalTradeRoutes(- min(iChange, pPlayer.getCoastalTradeRoutes()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CommerceFlexible") > -1:
			iCommerce = CommerceTypes(inputClass.getData2())
			if inputClass.getData1() == 1030:
				if pPlayer.isCommerceFlexible(iCommerce):
					pPlayer.changeCommercePercent(iCommerce, iChange)
			elif inputClass.getData1() == 1031:
				if pPlayer.isCommerceFlexible(iCommerce):
					pPlayer.changeCommercePercent(iCommerce, - min(iChange, pPlayer.getCommercePercent(iCommerce)))
			elif inputClass.getData1() == 7881:
				if pPlayer.isCommerceFlexible(iCommerce):
					pTeam.changeCommerceFlexibleCount(iCommerce, - pTeam.getCommerceFlexibleCount(iCommerce))
				else:
					pTeam.changeCommerceFlexibleCount(iCommerce, 1)
			self.placeStats()

		elif inputClass.getFunctionName() == "WBPlayerResearch":
			iTech = inputClass.getData2()
			if iTech == -1:
				pPlayer.clearResearchQueue()
			else:
				pPlayer.pushResearch(iTech, True)
			self.interfaceScreen(iPlayer)

		elif inputClass.getFunctionName().find("CurrentResearch") > -1:
			iTech = pPlayer.getCurrentResearch()
			if iTech > -1:
				if inputClass.getData1() == 1030:
					pTeam.changeResearchProgress(pPlayer.getCurrentResearch(), min(iChange, pTeam.getResearchCost(iTech) - pTeam.getResearchProgress(iTech)), iPlayer)
				elif inputClass.getData1() == 1031:
					pTeam.changeResearchProgress(pPlayer.getCurrentResearch(), - min(iChange, pTeam.getResearchProgress(iTech)), iPlayer)
				self.placeResearch()

		elif inputClass.getFunctionName() == "WBPlayerReligions":
			iReligion = inputClass.getData1()
			pPlayer.setLastStateReligion(inputClass.getData1())
			self.placeReligions()

		elif inputClass.getFunctionName().find("StateReligionUnit") > -1:
			if inputClass.getData1() == 1030:
				pPlayer.changeStateReligionUnitProductionModifier(iChange)
			elif inputClass.getData1() == 1031:
				pPlayer.changeStateReligionUnitProductionModifier(- min(iChange, pPlayer.getStateReligionUnitProductionModifier()))
			self.placeReligions()

		elif inputClass.getFunctionName().find("StateReligionBuilding") > -1:
			if inputClass.getData1() == 1030:
				pPlayer.changeStateReligionBuildingProductionModifier(iChange)
			elif inputClass.getData1() == 1031:
				pPlayer.changeStateReligionBuildingProductionModifier(- min(iChange, pPlayer.getStateReligionBuildingProductionModifier()))
			self.placeReligions()

		elif inputClass.getFunctionName() == "WBPlayerCivics":
			iCivic = inputClass.getData2()
			if pPlayer.canDoCivics(iCivic):
				pPlayer.setCivics(GC.getCivicInfo(iCivic).getCivicOptionType(), iCivic)
			self.interfaceScreen(iPlayer)

		elif inputClass.getFunctionName() == "PlayerEditScriptData":
			popup = Popup.PyPopup(1111, EventContextTypes.EVENTCONTEXT_ALL)
			popup.setHeaderString(CyTranslator().getText("TXT_KEY_WB_SCRIPT", ()))
			popup.setUserData((pPlayer.getID(),))
			popup.createEditBox(pPlayer.getScriptData())
			popup.launch()
			return

		return 1

	def update(self, fDelta):
		return 1