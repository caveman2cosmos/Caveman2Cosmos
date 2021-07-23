from CvPythonExtensions import *
import CvScreenEnums
import WBBuildingScreen
import WBCityDataScreen
import WBPlayerScreen
import WBTeamScreen
import WBPlotScreen
import WBEventScreen
import WBPlayerUnits
import WBReligionScreen
import WBCorporationScreen
import WBInfoScreen
import Popup
GC = CyGlobalContext()

iChange = 1
iOwnerType = 0
iPlotType = 2

class WBCityEditScreen:

	def __init__(self, WB):
		self.WB = WB
		self.lCities = []

	def interfaceScreen(self, pCityX):
		screen = CyGInterfaceScreen("WBCityEditScreen", CvScreenEnums.WB_CITYEDIT)
		global pCity
		global iPlayer
		global pPlot

		pCity = pCityX
		iPlayer = pCity.getOwner()
		pPlot = pCity.plot()

		screen.setRenderInterfaceOnly(True)
		screen.addPanel( "MainBG", u"", u"", True, False, -10, -10, screen.getXResolution() + 20, screen.getYResolution() + 20, PanelStyles.PANEL_STYLE_MAIN )
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.setText("WBCityEditExit", "Background", "<font=4>" + CyTranslator().getText("TXT_WORD_EXIT", ()).upper() + "</font>", 1<<1, screen.getXResolution() - 30, screen.getYResolution() - 42, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )
		sText = "<font=3b>%s, X: %d, Y: %d</font>" %(CyTranslator().getText("TXT_KEY_WB_LATITUDE",(pCity.plot().getLatitude(),)), pCity.getX(), pCity.getY())

		sText = u"<font=3b>%s ID: %d, %s: %d</font>" %(CyTranslator().getText("TXT_WORD_CITY", ()), pCity.getID(), CyTranslator().getText("TXT_KEY_WB_AREA_ID", ()), pPlot.getArea())
		screen.setLabel("PlotScreenHeaderB", "Background", "<font=4b>" + sText + "</font>", 1<<2, screen.getXResolution()/2, 50, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		sText = "<font=3b>%s, X: %d, Y: %d</font>" %(CyTranslator().getText("TXT_KEY_WB_LATITUDE",(pPlot.getLatitude(),)), pPlot.getX(), pPlot.getY())
		screen.setLabel("PlotLocation", "Background", sText, 1<<2, screen.getXResolution()/2, 70, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iWidth = screen.getXResolution()/4 - 40

		iX = 20
		screen.addDropDownBoxGFC("OwnerType", iX, 20, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, 0 == iOwnerType)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_PITBOSS_TEAM", ()), 2, 2, 2 == iOwnerType)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_MAIN_MENU_PLAYER", ()), 1, 1, 1 == iOwnerType)

		screen.addDropDownBoxGFC("PlotType", iX, 50, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("PlotType", CyTranslator().getText("TXT_KEY_WB_AREA_PLOTS", ()), 1, 1, iPlotType == 1)
		screen.addPullDownString("PlotType", CyTranslator().getText("TXT_KEY_WB_ALL_PLOTS", ()), 2, 2, iPlotType == 2)

		iX = screen.getXResolution() - iWidth - 20
		iY = 20
		screen.addDropDownBoxGFC("CityOwner", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for i in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(i)
			if pPlayerX.isEverAlive():
				sText = pPlayerX.getName()
				if not pPlayerX.isAlive():
					sText = "*" + sText
				if pPlayerX.isTurnActive():
					sText = "[" + sText + "]"
				screen.addPullDownString("CityOwner", sText, i, i, i == iPlayer)

		iY += 30
		screen.addDropDownBoxGFC("Commands", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_WB_COMMANDS", ()), 0, 0, True)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_WB_MOVE_CITY", ()), 1, 1, False)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_WB_DUPLICATE_CITY", ()), 2, 2, False)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_WB_MOVE_CITY", ()) + " + " + CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()), 3, 3, False)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_WB_DUPLICATE_CITY", ()) + " + " + CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()), 4, 4, False)
		screen.addPullDownString("Commands", CyTranslator().getText("TXT_KEY_WB_RAZE_CITY", ()), 5, 5, False)

		screen.addDropDownBoxGFC("ChangeBy", screen.getXResolution()/4, 110, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		i = 1
		while i < 1000001:
			screen.addPullDownString("ChangeBy", "(+/-) " + str(i), i, i, iChange == i)
			if str(i)[0] == "1":
				i *= 5
			else:
				i *= 2

		screen.addDropDownBoxGFC("CurrentPage", 20, screen.getYResolution() - 42, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_CITY_DATA", ()), 0, 0, True)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_CITY_DATA2", ()), 1, 1, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BUILDING", ()), 2, 2, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA", ()), 3, 3, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_TEAM_DATA", ()), 4, 4, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_RELIGION", ()), 8, 8, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_CONCEPT_CORPORATIONS", ()), 9, 9, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()), 5, 5, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLOT_DATA", ()), 6, 6, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_CONCEPT_EVENTS", ()), 7, 7, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

		self.sortCities()
		self.placeStats()
		self.placeProduction()
		self.placeScript()

	def sortCities(self):
		self.lCities = []
		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(iPlayerX)
			if iOwnerType == 1 and iPlayerX != iPlayer: continue
			if iOwnerType == 2 and pPlayerX.getTeam() != pCity.getTeam(): continue
			for city in pPlayerX.cities():
				if iPlotType == 2 or (iPlotType == 1 and city.plot().getArea() == pCity.plot().getArea()):
					sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
					if city.getID() == pCity.getID() and iPlayerX == iPlayer:
						sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
					self.lCities.append([city, iPlayerX, sColor])
		self.placeCityTable()

	def placeCityTable(self):
		screen = CyGInterfaceScreen("WBCityEditScreen", CvScreenEnums.WB_CITYEDIT)
		iWidth = screen.getXResolution()/4 - 40
		iHeight = (screen.getYResolution() - 120) / 24 * 24 + 2
		screen.addTableControlGFC("CurrentCity", 3, 20, 80, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		screen.setTableColumnHeader("CurrentCity", 0, "", 24)
		screen.setTableColumnHeader("CurrentCity", 1, "", 24)
		screen.setTableColumnHeader("CurrentCity", 2, "", iWidth - 48)

		for (loopCity, iPlayerX, sColor) in self.lCities:
			iRow = screen.appendTableRow("CurrentCity")
			iCiv = loopCity.getCivilizationType()
			screen.setTableText("CurrentCity", 0, iRow, "", GC.getCivilizationInfo(iCiv).getButton(), WidgetTypes.WIDGET_PYTHON, 7872, iCiv, 1<<0)
			iLeader = GC.getPlayer(iPlayerX).getLeaderType()
			screen.setTableText("CurrentCity", 1, iRow, "", GC.getLeaderHeadInfo(iLeader).getButton(), WidgetTypes.WIDGET_PYTHON, 7876, iLeader, 1<<0)
			screen.setTableText("CurrentCity", 2, iRow, "<font=3>" + sColor + loopCity.getName() + "</font></color>", '', WidgetTypes.WIDGET_PYTHON, 7200 + iPlayerX, loopCity.getID(), 1<<0)

	def placeMap(self):
		screen = CyGInterfaceScreen("WBCityEditScreen", CvScreenEnums.WB_CITYEDIT)
		iY = 110
		iWidth = screen.getXResolution()/2 - 40
		iMapHeight = min((screen.getYResolution()/2 - 195), iWidth * 2/3)
		iMapWidth = iMapHeight * 3/2
		screen.addPlotGraphicGFC("PlotView", screen.getXResolution() *3/4 - iMapWidth/2, 110, iMapWidth, iMapHeight, pCity.plot(), 350, True, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def placeStats(self):
		screen = CyGInterfaceScreen("WBCityEditScreen", CvScreenEnums.WB_CITYEDIT)
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=4b>" + pCity.getName()
		screen.setText("CityName", "", sText, 1<<2, screen.getXResolution()/2, 20, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, 0, 1)
		global iPlayer
		global pPlayer
		iPlayer = pCity.getOwner()
		pPlayer = GC.getPlayer(iPlayer)

		iX = screen.getXResolution()/4
		iY = 140
		screen.addDropDownBoxGFC("CityCultureLevel", iX, iY, screen.getXResolution()/4 - 40, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for i in xrange(GC.getNumCultureLevelInfos()):
			screen.addPullDownString("CityCultureLevel", GC.getCultureLevelInfo(i).getDescription(), i, i, pCity.getCultureLevel() == i)

		iY += 30
		screen.setButtonGFC("CityChangeCulturePlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityChangeCultureMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = u"<font=3>%s %s/%s%c</font>" %(CyTranslator().getText("TXT_KEY_WB_CULTURE",()), self.WB.addComma(pCity.getCulture(iPlayer)), self.WB.addComma(pCity.getCultureThreshold()), GC.getCommerceInfo(CommerceTypes.COMMERCE_CULTURE).getChar())
		screen.setLabel("CityChangeCultureText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityFoodPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityFoodMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = u"<font=3>%s: %d/%d%c</font>" %(CyTranslator().getText("TXT_WORD_FOOD",()), pCity.getFood(), pCity.growthThreshold(), GC.getYieldInfo(YieldTypes.YIELD_FOOD).getChar())
		screen.setLabel("CityFoodText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityPopulationPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityPopulationMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_WB_POPULATION",()) + " " + str(pCity.getPopulation()) + CyTranslator().getText("[ICON_ANGRYPOP]", ()) + "</font>"
		screen.setLabel("CityPopulationText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_WB_DEFENSE",(pCity.getDefenseModifier(False),)) + "</font>"
		screen.setLabel("CityDefenseValueText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityDefensePlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityDefenseMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_WB_DAMAGE",()) + ": " + str(pCity.getDefenseDamage()) + "</font>"
		screen.setLabel("CityDefenseDamageText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityTradeRoutePlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityTradeRouteMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = u"<font=3>%s: %d%s</font>" %(CyTranslator().getText("TXT_KEY_CONCEPT_TRADE",()), pCity.getTradeRoutes(), CyTranslator().getText("[ICON_TRADE]",()))
		screen.setLabel("CityCTradeRouteText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityChangeHappyPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityChangeHappyMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>%s: %d%s, %d%s</font>" %(CyTranslator().getText("TXT_KEY_CONCEPT_HAPPINESS",()), pCity.happyLevel(), CyTranslator().getText("[ICON_HAPPY]",()), pCity.unhappyLevel(0), CyTranslator().getText("[ICON_UNHAPPY]",()))
		screen.setLabel("CityChangeHappyText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityChangeHealthPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityChangeHealthMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>%s: %d%s, %d%s</font>" %(CyTranslator().getText("TXT_KEY_CONCEPT_HEALTH",()), pCity.goodHealth(), CyTranslator().getText("[ICON_HEALTHY]",()), pCity.badHealth(False), CyTranslator().getText("[ICON_UNHEALTHY]",()))
		screen.setLabel("CityChangeHealthText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityOccupationTurnPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityOccupationTurnMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_CONCEPT_RESISTANCE",()) + ": " + str(pCity.getOccupationTimer()) + CyTranslator().getText("[ICON_OCCUPATION]", ()) + "</font>"
		screen.setLabel("CityOccupationTurnText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityDraftAngerPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityDraftAngerMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_CONCEPT_DRAFT",()) + CyTranslator().getText(" [ICON_UNHAPPY]: ",()) + str(pCity.getConscriptAngerTimer()) + "</font>"
		screen.setLabel("CityDraftAngerText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityHurryAngerPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityHurryAngerMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_WB_HURRY_ANGER",(pCity.getHurryAngerTimer(),)) + "</font>"
		screen.setLabel("CityHurryAngerText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityDefyResolutionPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityDefyResolutionMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_WB_DEFY_RESOLUTION",(pCity.getDefyResolutionAngerTimer(),)) + "</font>"
		screen.setLabel("CityDefyResolutionText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityEspionageHealthPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityEspionageHealthMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = u"<font=3>%s %s: %d</font>" %(CyTranslator().getText("TXT_WORD_ESPIONAGE",()), CyTranslator().getText("[ICON_UNHEALTHY]", ()), pCity.getEspionageHealthCounter())
		screen.setLabel("CityEspionageHealthText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("CityTemporaryHappyPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("CityTemporaryHappyMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_WB_TEMP_HAPPY",(pCity.getHappinessTimer(),)) + "</font>"
		screen.setLabel("CityTemporaryHappyText", "Background", sText, 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def placeScript(self):
		screen = CyGInterfaceScreen( "WBCityEditScreen", CvScreenEnums.WB_CITYEDIT)
		iY = screen.getYResolution()/2 - 85
		iX = screen.getXResolution()/2
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_SCRIPT_DATA", ()) + "</color></font>"
		screen.setText("CityEditScriptData", "Background", sText, 1<<2, screen.getXResolution() * 3/4, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel("ScriptPanel", "", "", False, False, iX + 20, iY + 25, screen.getXResolution()/2 - 40, 60, PanelStyles.PANEL_STYLE_IN)
		screen.addMultilineText("CityScriptDataText", pCity.getScriptData(), iX + 20, iY + 25, screen.getXResolution()/2 - 40, 60, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

	def placeProduction(self):
		screen = CyGInterfaceScreen( "WBCityEditScreen", CvScreenEnums.WB_CITYEDIT)
		iY = screen.getYResolution()/2
		sText = CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
		screen.hide("CurrentProductionMinus")
		screen.hide("CurrentProductionPlus")
		if pCity.isProductionProcess():
			sText = pCity.getProductionName()
		elif pCity.isProduction():
			sText = u"%s: %d/%d%c" %(pCity.getProductionName(), pCity.getProduction(), pCity.getProductionNeeded(), GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar())
			screen.setButtonGFC("CurrentProductionPlus", "", "", screen.getXResolution() - 70, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
			screen.setButtonGFC("CurrentProductionMinus", "", "", screen.getXResolution() - 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)

		screen.setLabel("CurrentProductionText", "Background", "<font=3b>" + sText + "</font>", 1<<2, screen.getXResolution() * 3/4, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		iY += 30
		iWidth = screen.getXResolution()/2 - 40
		iHeight = (screen.getYResolution() - 40 - iY) /24 * 24 + 2
		iColumns = 3
		screen.addTableControlGFC("WBCityProduction", iColumns, screen.getXResolution()/2 + 20, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		for i in xrange(iColumns):
			screen.setTableColumnHeader("WBCityProduction", i, "", iWidth/iColumns)
		iMaxRow = -1
		iRow = 0
		for i in xrange(GC.getNumUnitInfos()):
			if pCity.canTrain(i, True, False, False, False):
				if iRow > iMaxRow:
					screen.appendTableRow("WBCityProduction")
					iMaxRow = iRow
				ItemInfo = GC.getUnitInfo(i)
				sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
				if pCity.getProductionUnit() == i:
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
				screen.setTableText("WBCityProduction", 0, iRow, "<font=3>" + sColor + ItemInfo.getDescription() + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 8202, i, 1<<0)
				iRow += 1
		iRow = 0
		for i in xrange(GC.getNumBuildingInfos()):
			bEligible = False
			if pCity.canConstruct(i, True, False, False):
				bEligible = True
			if not bEligible:
				for j in xrange(pCity.getOrderQueueLength()):
					iOrderData = pCity.getOrderFromQueue(j)
					if iOrderData.eOrderType == OrderTypes.ORDER_CONSTRUCT and iOrderData.iData1 == i:
						bEligible = True
			if bEligible:
				if iRow > iMaxRow:
					screen.appendTableRow("WBCityProduction")
					iMaxRow = iRow
				ItemInfo = GC.getBuildingInfo(i)
				sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
				if pCity.getProductionBuilding() == i:
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
				screen.setTableText("WBCityProduction", 1, iRow, "<font=3>" + sColor + ItemInfo.getDescription() + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_HELP_BUILDING, i, -1, 1<<0)
				iRow += 1
		sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		if pCity.isProduction():
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		screen.setTableText("WBCityProduction", 2, 0, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), WidgetTypes.WIDGET_PYTHON, 7878, -1, 1<<0)
		iRow = 1
		for i in xrange(GC.getNumProjectInfos()):
			bEligible = False
			if pCity.canCreate(i, True, False):
				bEligible = True
			if not bEligible:
				for j in xrange(pCity.getOrderQueueLength()):
					iOrderData = pCity.getOrderFromQueue(j)
					if iOrderData.eOrderType == OrderTypes.ORDER_CREATE and iOrderData.iData1 == i:
						bEligible = True
			if bEligible:
				if iRow > iMaxRow:
					screen.appendTableRow("WBCityProduction")
					iMaxRow = iRow
				ItemInfo = GC.getProjectInfo(i)
				sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
				if pCity.getProductionProject() == i:
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
				screen.setTableText("WBCityProduction", 2, iRow, "<font=3>" + sColor + ItemInfo.getDescription() + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 6785, i, 1<<0)
				iRow += 1

		for i in xrange(GC.getNumProcessInfos()):
			if pCity.canMaintain(i):
				if iRow > iMaxRow:
					screen.appendTableRow("WBCityProduction")
					iMaxRow = iRow
				ItemInfo = GC.getProcessInfo(i)
				sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
				if pCity.getProductionProcess() == i:
					sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
				screen.setTableText("WBCityProduction", 2, iRow, "<font=3>" + sColor + ItemInfo.getDescription() + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 6787, i, 1<<0)
				iRow += 1

	def handleInput (self, inputClass):
		screen = CyGInterfaceScreen( "WBCityEditScreen", CvScreenEnums.WB_CITYEDIT)
		global iChange
		global iOwnerType
		global iPlotType

		if inputClass.getFunctionName() == "ChangeBy":
			iChange = screen.getPullDownData("ChangeBy", screen.getSelectedPullDownID("ChangeBy"))

		elif inputClass.getFunctionName() == "CurrentPage":
			iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
			if iIndex == 1:
				WBCityDataScreen.WBCityDataScreen(self.WB).interfaceScreen(pCity)
			elif iIndex == 2:
				WBBuildingScreen.WBBuildingScreen(self.WB).interfaceScreen(pCity)
			elif iIndex == 3:
				WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 4:
				WBTeamScreen.WBTeamScreen(self.WB).interfaceScreen(pCity.getTeam())
			elif iIndex == 5:
				WBPlayerUnits.WBPlayerUnits(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 6:
				WBPlotScreen.WBPlotScreen(self.WB).interfaceScreen(pCity.plot())
			elif iIndex == 7:
				WBEventScreen.WBEventScreen(self.WB).interfaceScreen(pCity.plot())
			elif iIndex == 8:
				WBReligionScreen.WBReligionScreen(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 9:
				WBCorporationScreen.WBCorporationScreen(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 11:
				WBInfoScreen.WBInfoScreen(self.WB).interfaceScreen(iPlayer)

		elif inputClass.getFunctionName() == "OwnerType":
			iOwnerType = screen.getPullDownData("OwnerType", screen.getSelectedPullDownID("OwnerType"))
			self.sortCities()

		elif inputClass.getFunctionName() == "PlotType":
			iPlotType = screen.getPullDownData("PlotType", screen.getSelectedPullDownID("PlotType"))
			self.sortCities()

		elif inputClass.getFunctionName() == "CurrentCity":
			iPlayerX = inputClass.getData1() - 7200
			pPlayerX = GC.getPlayer(iPlayerX)
			if pPlayerX:
				self.interfaceScreen(pPlayerX.getCity(inputClass.getData2()))

		elif inputClass.getFunctionName() == "CityName":
			import CvEventInterface
			CvEventInterface.getEventManager().beginEvent(5000, (pCity, True))

		elif inputClass.getFunctionName() == "CityOwner":
			iIndex = screen.getSelectedPullDownID("CityOwner")
			GC.getPlayer(screen.getPullDownData("CityOwner", iIndex)).acquireCity(pCity, False, True)
			self.interfaceScreen(pPlot.getPlotCity())

		elif inputClass.getFunctionName().find("CityPopulation") > -1:
			if inputClass.getData1() == 1030:
				pCity.changePopulation(iChange)
			elif inputClass.getData1() == 1031:
				pCity.changePopulation(- min(iChange, pCity.getPopulation()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CityFood") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeFood(min(iChange, pCity.growthThreshold() - pCity.getFood()))
			elif inputClass.getData1() == 1031:
				pCity.changeFood(- min(iChange, pCity.getFood()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CityDefense") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeDefenseDamage(min(iChange, GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity.getDefenseDamage()))
			elif inputClass.getData1() == 1031:
				pCity.changeDefenseDamage(- min(iChange, pCity.getDefenseDamage()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CityTradeRoute") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeExtraTradeRoutes(min(iChange, pCity.getMaxTradeRoutes() - pCity.getTradeRoutes()))
			elif inputClass.getData1() == 1031:
				pCity.changeExtraTradeRoutes(- min(iChange, pCity.getTradeRoutes()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CityChangeCulture") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeCulture(iPlayer, iChange, True)
			elif inputClass.getData1() == 1031:
				pCity.changeCulture(iPlayer, - min(iChange, pCity.getCulture(iPlayer)), True)
			self.placeStats()

		elif inputClass.getFunctionName() == ("CityCultureLevel"):
			iIndex = screen.getSelectedPullDownID("CityCultureLevel")
			if iIndex == 0:
				pCity.setOccupationTimer(max(1, pCity.getOccupationTimer()))
			else:
				pCity.setOccupationTimer(0)
				pCity.setCulture(iPlayer, GC.getCultureLevelInfo(iIndex).getSpeedThreshold(CyGame().getGameSpeedType()), True)
			self.placeStats()

		elif inputClass.getFunctionName().find("CityChangeHappy") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeExtraHappiness(iChange)
			elif inputClass.getData1() == 1031:
				pCity.changeExtraHappiness(- iChange)
			self.placeStats()

		elif inputClass.getFunctionName().find("CityChangeHealth") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeExtraHealth(iChange)
			elif inputClass.getData1() == 1031:
				pCity.changeExtraHealth(- iChange)
			self.placeStats()

		elif inputClass.getFunctionName().find("CityOccupationTurn") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeOccupationTimer(iChange)
			elif inputClass.getData1() == 1031:
				pCity.changeOccupationTimer(- min(iChange, pCity.getOccupationTimer()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CityDraftAnger") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeConscriptAngerTimer(iChange)
			elif inputClass.getData1() == 1031:
				pCity.changeConscriptAngerTimer(- min(iChange, pCity.getConscriptAngerTimer()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CityHurryAnger") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeHurryAngerTimer(iChange)
			elif inputClass.getData1() == 1031:
				pCity.changeHurryAngerTimer(- min(iChange, pCity.getHurryAngerTimer()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CityDefyResolution") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeDefyResolutionAngerTimer(iChange)
			elif inputClass.getData1() == 1031:
				pCity.changeDefyResolutionAngerTimer(- min(iChange, pCity.getDefyResolutionAngerTimer()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CityEspionageHappy") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeEspionageHappinessCounter(iChange)
			elif inputClass.getData1() == 1031:
				pCity.changeEspionageHappinessCounter(- min(iChange, pCity.getEspionageHappinessCounter()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CityEspionageHealth") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeEspionageHealthCounter(iChange)
			elif inputClass.getData1() == 1031:
				pCity.changeEspionageHealthCounter(- min(iChange, pCity.getEspionageHealthCounter()))
			self.placeStats()

		elif inputClass.getFunctionName().find("CityTemporaryHappy") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeHappinessTimer(iChange)
			elif inputClass.getData1() == 1031:
				pCity.changeHappinessTimer(- min(iChange, pCity.getHappinessTimer()))
			self.placeStats()

		elif inputClass.getFunctionName() == "WBCityProduction":
			self.handlePlatyChooseProduction(inputClass)
			self.placeProduction()

		elif inputClass.getFunctionName().find("CurrentProduction") > -1:
			if inputClass.getData1() == 1030:
				pCity.changeProduction(min(iChange, pCity.getProductionNeeded() - pCity.getProduction()))
			elif inputClass.getData1() == 1031:
				pCity.changeProduction(- min(iChange, pCity.getProduction()))
			self.placeProduction()

		elif inputClass.getFunctionName().find("CityEditScriptData") > -1:
			popup = Popup.PyPopup(2222, EventContextTypes.EVENTCONTEXT_ALL)
			popup.setHeaderString(CyTranslator().getText("TXT_KEY_WB_SCRIPT", ()))
			popup.setUserData((pCity.getOwner(), pCity.getID()))
			popup.createEditBox(pCity.getScriptData())
			popup.launch()
			return

		elif inputClass.getFunctionName() == "Commands":
			iIndex = screen.getPullDownData("Commands", screen.getSelectedPullDownID("Commands"))
			if iIndex == 5:
				pCity.kill()
			else:
				self.WB.iMoveCity = pCity.getID()
				self.WB.iCurrentPlayer = iPlayer
				if iIndex == 1:
					self.WB.iPlayerAddMode = "MoveCity"
				elif iIndex == 2:
					self.WB.iPlayerAddMode = "DuplicateCity"
				elif iIndex == 3:
					self.WB.iPlayerAddMode = "MoveCityPlus"
					self.WB.lMoveUnit = []
					for pUnitX in pPlot.units():
						if pUnitX.getOwner() == iPlayer:
							self.WB.lMoveUnit.append([iPlayer, pUnitX.getID()])
				elif iIndex == 4:
					self.WB.iPlayerAddMode = "DuplicateCityPlus"
					self.WB.lMoveUnit = []
					for pUnitX in pPlot.units():
						if pUnitX.getOwner() == iPlayer:
							self.WB.lMoveUnit.append([iPlayer, pUnitX.getID()])
			screen.hideScreen()
		return 1

	def handlePlatyChooseProduction(self, inputClass):
		if inputClass.getButtonType() == WidgetTypes.WIDGET_HELP_BUILDING:
			for j in xrange(pCity.getOrderQueueLength()):
				iOrderData = pCity.getOrderFromQueue(j)
				if iOrderData.eOrderType == OrderTypes.ORDER_CONSTRUCT and iOrderData.iData1 == inputClass.getData1():
					pCity.popOrder(j, False, False)
					break
			pCity.pushOrder(OrderTypes.ORDER_CONSTRUCT, inputClass.getData1() , -1, False, False, False, True)
		elif inputClass.getData1() == 8202:
			for j in xrange(pCity.getOrderQueueLength()):
				iOrderData = pCity.getOrderFromQueue(j)
				if iOrderData.eOrderType == OrderTypes.ORDER_TRAIN and iOrderData.iData1 == inputClass.getData2():
					pCity.popOrder(j, False, False)
					break
			pCity.pushOrder(OrderTypes.ORDER_TRAIN, inputClass.getData2() , -1, False, False, False, True)
		elif inputClass.getData1() == 6785:
			for j in xrange(pCity.getOrderQueueLength()):
				iOrderData = pCity.getOrderFromQueue(j)
				if iOrderData.eOrderType == OrderTypes.ORDER_CREATE and iOrderData.iData1 == inputClass.getData2():
					pCity.popOrder(j, False, False)
					break
			pCity.pushOrder(OrderTypes.ORDER_CREATE, inputClass.getData2() , -1, False, False, False, True)
		elif inputClass.getData1() == 6787:
			for j in xrange(pCity.getOrderQueueLength()):
				iOrderData = pCity.getOrderFromQueue(j)
				if iOrderData.eOrderType == OrderTypes.ORDER_MAINTAIN and iOrderData.iData1 == inputClass.getData2():
					pCity.popOrder(j, False, False)
					break
			pCity.pushOrder(OrderTypes.ORDER_MAINTAIN, inputClass.getData2() , -1, False, False, False, True)
		else:
			pCity.clearOrderQueue()

	def update(self, fDelta):
		self.placeMap()
		return 1