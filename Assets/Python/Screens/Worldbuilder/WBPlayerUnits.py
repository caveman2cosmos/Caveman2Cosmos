from CvPythonExtensions import *
import CvScreenEnums
import WBPlayerScreen
import WBTeamScreen
import WBProjectScreen
import WBCityEditScreen
import WBUnitScreen
import WBInfoScreen
import WBCityDataScreen
import WBBuildingScreen
import WBPromotionScreen
import WBPlotScreen
import WBEventScreen
import WorldBuilder

GC = CyGlobalContext()
iCityID = -1
iCityOwner = -1
iUnitID = -1
iUnitOwner = -1
iCopyType = 0
iOwnerType = 1
iPlotType = 2
iActivityType = 0

class WBPlayerUnits:
	def __init__(self, WB):
		self.WB = WB
		self.iTable_Y = 110

	def interfaceScreen(self, iPlayerX):
		screen = CyGInterfaceScreen( "WBPlayerUnits", CvScreenEnums.WB_UNITLIST)
		global iPlayer
		global iMapWidth
		global iMapHeight
		iPlayer = iPlayerX
		iMapWidth = screen.getXResolution()/4
		iMapHeight = iMapWidth * 3/4

		screen.setRenderInterfaceOnly(True)
		screen.addPanel( "MainBG", u"", u"", True, False, -10, -10, screen.getXResolution() + 20, screen.getYResolution() + 20, PanelStyles.PANEL_STYLE_MAIN )
		screen.setDimensions(0,0, screen.getXResolution(), screen.getYResolution())
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.setText("WBExit", "Background", "<font=4>" + CyTranslator().getText("TXT_WORD_EXIT", ()).upper() + "</font>", 1<<1, screen.getXResolution() - 30, screen.getYResolution() - 42, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		iX = 20
		iY = 20
		iWidth = (screen.getXResolution() - 40)/5
		screen.addDropDownBoxGFC("CurrentPlayer", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for i in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(i)
			if pPlayerX.isEverAlive():
				sText = pPlayerX.getName()
				if not pPlayerX.isAlive():
					sText = "*" + sText
				if pPlayerX.isTurnActive():
					sText = "[" + sText + "]"
				screen.addPullDownString("CurrentPlayer", sText, i, i, i == iPlayer)

		iX += iWidth
		screen.addDropDownBoxGFC("OwnerType", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, 0 == iOwnerType)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_PITBOSS_TEAM", ()), 2, 2, 2 == iOwnerType)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_MAIN_MENU_PLAYER", ()), 1, 1, 1 == iOwnerType)

		iX += iWidth
		screen.addDropDownBoxGFC("PlotType", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("PlotType", CyTranslator().getText("TXT_KEY_WB_SINGLE_PLOT", ()), 0, 0, iPlotType == 0)
		screen.addPullDownString("PlotType", CyTranslator().getText("TXT_KEY_WB_AREA_PLOTS", ()), 1, 1, iPlotType == 1)
		screen.addPullDownString("PlotType", CyTranslator().getText("TXT_KEY_WB_ALL_PLOTS", ()), 2, 2, iPlotType == 2)

		iX += iWidth
		screen.addDropDownBoxGFC("CopyType", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, 0 == iCopyType)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_SPACE_SHIP_SCREEN_TYPE_BUTTON", ()), 1, 1, 1 == iCopyType)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT_COMBAT", ()), 2, 2, 2 == iCopyType)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_DOMAIN", ()), 3, 3, 3 == iCopyType)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_WB_GROUP", ()), 4, 4, 4 == iCopyType)
		screen.addPullDownString("CopyType", CyTranslator().getText("TXT_KEY_WB_ACTIVITY", ()), 5, 5, 5 == iCopyType)

		iX += iWidth
		screen.addDropDownBoxGFC("ActivityType", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for i in xrange(len(WorldBuilder.Activities)):
			screen.addPullDownString("ActivityType", WorldBuilder.Activities[i], i, i, i == iActivityType)
		if iCopyType != 5:
			screen.hide("ActivityType")

		iY = self.iTable_Y - 30
		sKillButton = "Art/Interface/Buttons/Actions/Delete.dds"
		sSkipButton = GC.getMissionInfo(MissionTypes.MISSION_SKIP).getButton()

		screen.setImageButton("DeleteCurrentCity", sKillButton, 20, iY, 28, 28, WidgetTypes.WIDGET_PYTHON, 1041, -1)
		screen.setLabel("DeleteCitiesText", "Background", "<font=4b>" + CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()) + "</font>", 1<<1, screen.getXResolution()/2 - 35, iY, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setImageButton("DeleteAllCities", sKillButton, screen.getXResolution()/2 - 35, iY, 28, 28, WidgetTypes.WIDGET_PYTHON, 1041, -1)

		screen.setImageButton("DeleteCurrentUnit", sKillButton, 10 + screen.getXResolution()/2, iY, 28, 28, WidgetTypes.WIDGET_PYTHON, 1041, -1)
		screen.setImageButton("EndCurrentUnit", sSkipButton, 40 + screen.getXResolution()/2, iY, 28, 28, WidgetTypes.WIDGET_PYTHON, 1042, -1)
		screen.setLabel("DeleteUnitsText", "Background", "<font=4b>" + CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()) + "</font>", 1<<1, screen.getXResolution() - 75, iY, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setImageButton("DeleteAllUnits", sKillButton, screen.getXResolution() - 75, iY, 28, 28, WidgetTypes.WIDGET_PYTHON, 1041, -1)
		screen.setImageButton("EndAllUnits", sSkipButton, screen.getXResolution() - 45, iY, 28, 28, WidgetTypes.WIDGET_PYTHON, 1042, -1)

		self.sortUnits()
		self.sortCities()
		self.addPageSwitch()

	def sortUnits(self):
		screen = CyGInterfaceScreen( "WBPlayerUnits", CvScreenEnums.WB_UNITLIST)
		global iUnitID
		global lUnits
		global iUnitOwner
		pPlayer = GC.getPlayer(iPlayer)

		lUnits = []
		pUnit = pPlayer.getUnit(iUnitID)
		if iUnitOwner > -1:
			pUnit = GC.getPlayer(iUnitOwner).getUnit(iUnitID)
		if pUnit is None:
			unitX, i = pPlayer.firstUnit(False)
			if unitX:
				pUnit = unitX
				iUnitID = unitX.getID()
				iUnitOwner = unitX.getOwner()

		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			if iOwnerType == 1 and iPlayerX != iPlayer:
				continue
			playerX = GC.getPlayer(iPlayerX)
			if iOwnerType == 2 and playerX.getTeam() != pPlayer.getTeam() or not playerX.isAlive():
				continue
			for unitX in playerX.units():
				bCopy = True
				if iPlotType == 0:
					if unitX.getX() != pUnit.getX() or unitX.getY() != pUnit.getY():
						bCopy = False
				elif iPlotType == 1:
					if unitX.plot().getArea() != pUnit.plot().getArea():
						bCopy = False
				if iCopyType == 1:
					if unitX.getUnitType() != pUnit.getUnitType():
						bCopy = False
				elif iCopyType == 2:
					if unitX.getUnitCombatType() != pUnit.getUnitCombatType():
						bCopy = False
				elif iCopyType == 3:
					if unitX.getDomainType() != pUnit.getDomainType():
						bCopy = False
				elif iCopyType == 4:
					if unitX.getGroupID() != pUnit.getGroupID() or unitX.getOwner() != pUnit.getOwner():
						bCopy = False
				elif iCopyType == 5:
					loopGroup = unitX.getGroup()
					if loopGroup.getActivityType() != iActivityType:
						bCopy = False
				if bCopy:
					lUnits.append([unitX.getOwner(), unitX.getID()])
		lUnits.sort()
		self.placeCurrentUnit()

	def sortCities(self):
		screen = CyGInterfaceScreen( "WBPlayerUnits", CvScreenEnums.WB_UNITLIST)
		global iCityID
		global lCities
		global iCityOwner
		pPlayer = GC.getPlayer(iPlayer)

		if iCityOwner > -1:
			pCity = GC.getPlayer(iCityOwner).getCity(iCityID)
		else: pCity = pPlayer.getCity(iCityID)

		if pCity is None:
			cityX, i = pPlayer.firstCity(False)
			if cityX:
				pCity = cityX
				iCityID = cityX.getID()
				iCityOwner = cityX.getOwner()

		lCities = []
		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			if iOwnerType == 1 and iPlayerX != iPlayer:
				continue
			playerX = GC.getPlayer(iPlayerX)
			if iOwnerType == 2 and playerX.getTeam() != pPlayer.getTeam() or not playerX.isAlive():
				continue
			for cityX in playerX.cities():
				bCopy = True
				if iPlotType == 0:
					if cityX.getX() != pCity.getX() or cityX.getY() != pCity.getY():
						bCopy = False
				elif iPlotType == 1:
					if cityX.plot().getArea() != pCity.plot().getArea():
						bCopy = False
				if bCopy:
					lCities.append([cityX.getOwner(), cityX.getID()])
		lCities.sort()
		self.placeCurrentCity()

	def placeCurrentUnit(self):
		screen = CyGInterfaceScreen( "WBPlayerUnits", CvScreenEnums.WB_UNITLIST)
		iY = self.iTable_Y + iMapHeight
		iWidth = screen.getXResolution()/2 - 30
		iHeight = (screen.getYResolution() - iY - 42) / 24 * 24 + 2
		iColWidth = (iWidth - 24*2 - 10) /9

		lStatus = [CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), GC.getMissionInfo(MissionTypes.MISSION_FORTIFY).getButton(), GC.getMissionInfo(MissionTypes.MISSION_SKIP).getButton()]

		screen.addTableControlGFC( "WBUnitList", 8, 10 + screen.getXResolution()/2, iY, iWidth, iHeight, True, True, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader( "WBUnitList", 0, "", 24)
		screen.setTableColumnHeader( "WBUnitList", 1, "", 24)
		screen.setTableColumnHeader( "WBUnitList", 2, CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()), iColWidth * 4)
		screen.setTableColumnHeader( "WBUnitList", 3, "ID", iColWidth * 2)
		screen.setTableColumnHeader( "WBUnitList", 4, CyTranslator().getText("TXT_KEY_WB_XLEVEL", ()), iColWidth * 1)
		screen.setTableColumnHeader( "WBUnitList", 5, CyTranslator().getText("[ICON_STRENGTH]", ()), iColWidth * 1)
		screen.setTableColumnHeader( "WBUnitList", 6, CyTranslator().getText("[ICON_MOVES]", ()), iColWidth * 1)
		screen.setTableColumnHeader( "WBUnitList", 7, "", 10)
		screen.enableSort("WBUnitList")

		for i in lUnits:
			pPlayerX = GC.getPlayer(i[0])
			loopUnit = pPlayerX.getUnit(i[1])
			if loopUnit is None: continue
			iRow = screen.appendTableRow("WBUnitList")

			iStatus = 0
			if loopUnit.movesLeft() > 0:
				iStatus = 1
			if loopUnit.getGroup().readyToMove(False):
				iStatus = 2
			sColor = CyTranslator().getText("[COLOR_NEGATIVE_TEXT]", ())
			if iUnitID == loopUnit.getID() and iUnitOwner == loopUnit.getOwner():
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			iCivilization = loopUnit.getCivilizationType()
			screen.setTableText("WBUnitList", 0, iRow, "", GC.getCivilizationInfo(iCivilization).getButton(), WidgetTypes.WIDGET_PYTHON, 7872, i[0] * 10000 + iCivilization, 1<<2)
			screen.setTableText("WBUnitList", 1, iRow, str(iStatus), lStatus[iStatus], WidgetTypes.WIDGET_PYTHON, 1043, iStatus, 1<<0)
			screen.setTableText("WBUnitList", 2, iRow, "<font=3>" + sColor + loopUnit.getName() + "</color></font>", loopUnit.getButton(), WidgetTypes.WIDGET_PYTHON, 8300 + i[0], i[1], 1<<0)
			screen.setTableInt("WBUnitList", 3, iRow, "<font=3>" + str(loopUnit.getID()) + "</font>", "", WidgetTypes.WIDGET_PYTHON, 8300 + i[0], i[1], 1<<2)
			screen.setTableInt("WBUnitList", 4, iRow, "<font=3>" + str(loopUnit.getLevel()) + "</font>", "", WidgetTypes.WIDGET_PYTHON, 8300 + i[0], i[1], 1<<2)
			screen.setTableInt("WBUnitList", 5, iRow, "<font=3>" + str(loopUnit.baseCombatStr()) + "</font>", "", WidgetTypes.WIDGET_PYTHON, 8300 + i[0], i[1], 1<<2)
			screen.setTableInt("WBUnitList", 6, iRow, "<font=3>" + str(loopUnit.baseMoves()) + "</font>", "", WidgetTypes.WIDGET_PYTHON, 8300 + i[0], i[1], 1<<2)
		self.placeUnitMap()

	def placeCurrentCity(self):
		screen = CyGInterfaceScreen( "WBPlayerUnits", CvScreenEnums.WB_UNITLIST)
		iY = self.iTable_Y + iMapHeight
		iWidth = screen.getXResolution()/2 - 30
		iHeight = (screen.getYResolution() - iY - 42) / 24 * 24 + 2
		iColWidth = (iWidth - 24) /10

		screen.addTableControlGFC( "WBCityList", 7, 20, iY, iWidth, iHeight, True, True, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader( "WBCityList", 0, "", 24)
		screen.setTableColumnHeader( "WBCityList", 1, CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()), iColWidth * 3)
		screen.setTableColumnHeader( "WBCityList", 2,"ID", iColWidth * 2)
		screen.setTableColumnHeader( "WBCityList", 3, CyTranslator().getText("[ICON_CULTURE]", ()), iColWidth * 2)
		screen.setTableColumnHeader( "WBCityList", 4, CyTranslator().getText("[ICON_ANGRYPOP]", ()), iColWidth * 1)
		screen.setTableColumnHeader( "WBCityList", 5, CyTranslator().getText("[ICON_HAPPY]", ()), iColWidth * 1)
		screen.setTableColumnHeader( "WBCityList", 6, CyTranslator().getText("[ICON_HEALTHY]", ()), iColWidth * 1)
		screen.enableSort("WBCityList")

		for i in lCities:
			pPlayerX = GC.getPlayer(i[0])
			loopCity = pPlayerX.getCity(i[1])
			if loopCity is None: continue
			iRow = screen.appendTableRow("WBCityList")
			sColor = CyTranslator().getText("[COLOR_NEGATIVE_TEXT]", ())
			if iCityID == loopCity.getID() and iCityOwner == loopCity.getOwner():
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			iCivilization = loopCity.getCivilizationType()
			screen.setTableText("WBCityList", 0, iRow, "", GC.getCivilizationInfo(iCivilization).getButton(), WidgetTypes.WIDGET_PYTHON, 7872, i[0] * 10000 + iCivilization, 1<<2)
			screen.setTableText("WBCityList", 1, iRow, "<font=3>" + sColor + loopCity.getName() + "</color></font>", "", WidgetTypes.WIDGET_PYTHON, 7200 + i[0], i[1], 1<<0)
			screen.setTableInt("WBCityList", 2, iRow, "<font=3>" + str(loopCity.getID()) + "</font>", "", WidgetTypes.WIDGET_PYTHON, 7200 + i[0], i[1], 1<<2)
			screen.setTableInt("WBCityList", 3, iRow, "<font=3>" + self.WB.addComma(loopCity.getCulture(i[0])) + "</font>", "", WidgetTypes.WIDGET_PYTHON, 7200 + i[0], i[1], 1<<2)
			screen.setTableInt("WBCityList", 4, iRow, "<font=3>" + str(loopCity.getPopulation()) + "</font>", "", WidgetTypes.WIDGET_PYTHON, 7200 + i[0], i[1], 1<<2)
			screen.setTableInt("WBCityList", 5, iRow, "<font=3>" + str(loopCity.happyLevel() - loopCity.unhappyLevel(0)) + "</font>", "", WidgetTypes.WIDGET_PYTHON, 7200 + i[0], i[1], 1<<2)
			screen.setTableInt("WBCityList", 6, iRow, "<font=3>" + str(loopCity.goodHealth() - loopCity.badHealth(0)) + "</font>", "", WidgetTypes.WIDGET_PYTHON, 7200 + i[0], i[1], 1<<2)
		self.placeCityMap()

	def placeCityMap(self):
		screen = CyGInterfaceScreen("WBPlayerUnits", CvScreenEnums.WB_UNITLIST)
		screen.hide("GoToCity")
		screen.hide("CityView")
		screen.hide("CityDescription")
		pCityOwner = GC.getPlayer(iCityOwner)
		if not pCityOwner: return
		pCity = pCityOwner.getCity(iCityID)
		if pCity is None: return
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=4b>" + pCity.getName() + "</color></font>"
		screen.setText("GoToCity", "Background", sText, 1<<2, screen.getXResolution()/4, self.iTable_Y - 60, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		iXMap = screen.getXResolution()/4 - 10
		screen.addPlotGraphicGFC("CityView", iXMap, self.iTable_Y, iMapWidth, iMapHeight, pCity.plot(), 350, False, WidgetTypes.WIDGET_GENERAL, -1, -1)
		iX = 20
		screen.addPanel("CityPanel", "", "", False, False, iX, self.iTable_Y, iXMap - iX, iMapHeight, PanelStyles.PANEL_STYLE_IN)
		screen.addMultilineText("CityDescription", self.getCityData(pCity), iX, self.iTable_Y, iXMap - iX, iMapHeight, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

	def placeUnitMap(self):
		screen = CyGInterfaceScreen("WBPlayerUnits", CvScreenEnums.WB_UNITLIST)
		screen.hide("GoToUnit")
		screen.hide("UnitView")
		screen.hide("UnitDescription")
		pUnitOwner = GC.getPlayer(iUnitOwner)
		if pUnitOwner is None: return
		pUnit = pUnitOwner.getUnit(iUnitID)
		if pUnit is None: return
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=4b>" + pUnit.getName() + "</color></font>"
		screen.setText("GoToUnit", "Background", sText, 1<<2, screen.getXResolution()*3/4, self.iTable_Y - 60, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		iXMap = screen.getXResolution() * 3/4 - 20
		screen.addPlotGraphicGFC("UnitView", iXMap, self.iTable_Y, iMapWidth, iMapHeight, pUnit.plot(), 350, True, WidgetTypes.WIDGET_GENERAL, -1, -1)
		iX = screen.getXResolution()/2 + 10
		screen.addPanel("UnitPanel", "", "", False, False, iX, self.iTable_Y, iXMap - iX, iMapHeight, PanelStyles.PANEL_STYLE_IN)
		screen.addMultilineText("UnitDescription", self.getUnitData(pUnit), iX, self.iTable_Y, iXMap - iX, iMapHeight, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

	def getCityData(self, pCity):
		pPlayer = GC.getPlayer(iCityOwner)
		sText = "<font=3>"
		if pCity.isCapital():
			sText += CyTranslator().getText("[ICON_STAR]", ())
		elif pCity.isGovernmentCenter():
			sText += CyTranslator().getText("[ICON_SILVER_STAR]", ())
		sText += u"%s: %d<font=2>" %(pCity.getName(), pCity.getPopulation())
		sTemp = ""
		if pCity.isConnectedToCapital(iCityOwner):
			sTemp += CyTranslator().getText("[ICON_TRADE]", ())
		for i in xrange(GC.getNumReligionInfos()):
			if pCity.isHolyCityByType(i):
				sTemp += u"%c" %(GC.getReligionInfo(i).getHolyCityChar())
			elif pCity.isHasReligion(i):
				sTemp += u"%c" %(GC.getReligionInfo(i).getChar())

		for i in xrange(GC.getNumCorporationInfos()):
			if pCity.isHeadquartersByType(i):
				sTemp += u"%c" %(GC.getCorporationInfo(i).getHeadquarterChar())
			elif pCity.isHasCorporation(i):
				sTemp += u"%c" %(GC.getCorporationInfo(i).getChar())
		if len(sTemp) > 0:
			sText += "\n" + sTemp

		iMaxDefense = pCity.getTotalDefense(False)
		if iMaxDefense > 0:
			sText += u"\n%s: " %(CyTranslator().getText("[ICON_DEFENSE]", ()))
			iCurrent = pCity.getDefenseModifier(False)
			if iCurrent != iMaxDefense:
				sText += u"%d/" %(iCurrent)
			sText += u"%d%%" %(iMaxDefense)

		sText += u"\n%s: %d/%d" %(CyTranslator().getText("[ICON_FOOD]", ()), pCity.getFood(), pCity.growthThreshold())
		iFoodGrowth = pCity.foodDifference(True)
		if iFoodGrowth != 0:
			sText += u" %+d" %(iFoodGrowth)

		if pCity.isProduction():
			sText += u"\n%s:" %(CyTranslator().getText("[ICON_PRODUCTION]", ()))
			if not pCity.isProductionProcess():
				sText += u" %d/%d" %(pCity.getProduction(), pCity.getProductionNeeded())
				iProduction = pCity.getCurrentProductionDifference(False, True)
				if iProduction != 0:
					sText += u" %+d" %(iProduction)
			sText += u" (%s)" %(pCity.getProductionName())

		iGPRate = pCity.getGreatPeopleRate()
		iProgress = pCity.getGreatPeopleProgress()
		if iGPRate > 0 or iProgress > 0:
			sText += u"\n%s: %d/%d %+d" %(CyTranslator().getText("[ICON_GREATPEOPLE]", ()), iProgress, pPlayer.greatPeopleThresholdNonMilitary(), iGPRate)

		sText += u"\n%s: %d/%d (%s)" %(CyTranslator().getText("[ICON_CULTURE]", ()), pCity.getCulture(iCityOwner), pCity.getCultureThreshold(), GC.getCultureLevelInfo(pCity.getCultureLevel()).getDescription())

		lTemp = []
		for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
			iAmount = pCity.getCommerceRateTimes100(i)
			if iAmount <= 0: continue
			sTemp = u"%d.%02d%c" %(pCity.getCommerceRate(i), pCity.getCommerceRateTimes100(i)%100, GC.getCommerceInfo(i).getChar())
			lTemp.append(sTemp)
		if len(lTemp) > 0:
			sText += "\n"
			for i in xrange(len(lTemp)):
				sText += lTemp[i]
				if i < len(lTemp) - 1:
					sText += ", "

		iMaintenance = pCity.getMaintenanceTimes100()
		if iMaintenance != 0:
			sText += "\n" + CyTranslator().getText("[COLOR_WARNING_TEXT]", ()) + CyTranslator().getText("INTERFACE_CITY_MAINTENANCE", ()) + " </color>"
			sText += u"-%d.%02d%c" %(iMaintenance/100, iMaintenance%100, GC.getCommerceInfo(CommerceTypes.COMMERCE_GOLD).getChar())

		sText += "\n" + CyTranslator().getText("TXT_WORD_CITY", ()) + " ID: " + str(pCity.getID())
		sText += "\n" + "X: " + str(pCity.getX()) + ", Y: " + str(pCity.getY())
		sText += "\n" + CyTranslator().getText("TXT_KEY_WB_AREA_ID", ()) + ": "  + str(pCity.plot().getArea())

		sText += "</font>"
		return sText

	def getUnitData(self, pUnit):
		sText = CyGameTextMgr().getSpecificUnitHelp(pUnit, True, False)
		pGroup = pUnit.getGroup()
		iActivity = pGroup.getActivityType()
		if iActivity > -1 and iActivity < len(WorldBuilder.Activities):
			sText += "\n" + WorldBuilder.Activities[iActivity]
		sText += "\n" + CyTranslator().getText("TXT_WORD_UNIT", ()) + " ID: " + str(pUnit.getID())
		sText += "\n" + CyTranslator().getText("TXT_KEY_WB_GROUP", ()) + " ID: " + str(pUnit.getGroupID())
		sText += "\n" + "X: " + str(pUnit.getX()) + ", Y: " + str(pUnit.getY())
		sText += "\n" + CyTranslator().getText("TXT_KEY_WB_AREA_ID", ()) + ": "  + str(pUnit.plot().getArea())
		return sText

	def addPageSwitch(self):
		screen = CyGInterfaceScreen( "WBPlayerUnits", CvScreenEnums.WB_UNITLIST)
		screen.addDropDownBoxGFC("CurrentPage", 20, screen.getYResolution() - 42, screen.getXResolution()/5, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA", ()), 0, 0, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_TEAM_DATA", ()), 1, 1, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_PROJECT", ()), 2, 2, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_TECH", ()), 3, 3, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()), 4, 4, True)
		pCityOwner = GC.getPlayer(iCityOwner)
		if pCityOwner:
			pCity = pCityOwner.getCity(iCityID)
			if pCity:
				screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_CITY_DATA", ()), 9, 9, False)
				screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_CITY_DATA2", ()), 10, 10, False)
				screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BUILDING", ()), 14, 14, False)
				szCity = CyTranslator().getText("TXT_WORD_CITY", ()) + " "
				screen.addPullDownString("CurrentPage", szCity + CyTranslator().getText("TXT_KEY_WB_PLOT_DATA", ()), 12, 12, False)
				screen.addPullDownString("CurrentPage", szCity + CyTranslator().getText("TXT_KEY_CONCEPT_EVENTS", ()), 13, 13, False)
		pUnitOwner = GC.getPlayer(iUnitOwner)
		if pUnitOwner:
			pUnit = pUnitOwner.getUnit(iUnitID)
			if pUnit is not None:
				screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_UNIT_DATA", ()), 5, 5, False)
				screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_PROMOTION", ()), 6, 6, False)
				screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_WORD_UNIT", ()) + " " + CyTranslator().getText("TXT_KEY_WB_PLOT_DATA", ()), 7, 7, False)
				screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_WORD_UNIT", ()) + " " + CyTranslator().getText("TXT_KEY_CONCEPT_EVENTS", ()), 8, 8, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

	def handleInput(self, inputClass):
		screen = CyGInterfaceScreen("WBPlayerUnits", CvScreenEnums.WB_UNITLIST)
		global iCityID
		global iUnitID
		global iCopyType
		global iOwnerType
		global iPlotType
		global iActivityType
		global iPlayer
		global iCityOwner
		global iUnitOwner
		pPlayer = GC.getPlayer(iPlayer)
		pUnitOwner = GC.getPlayer(iUnitOwner)
		pCityOwner = GC.getPlayer(iCityOwner)
		sName = inputClass.getFunctionName()

		if sName == "CurrentPage":
			iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
			if iIndex == 0:
				WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 1:
				WBTeamScreen.WBTeamScreen(self.WB).interfaceScreen(pPlayer.getTeam())
			elif iIndex == 2:
				WBProjectScreen.WBProjectScreen(self.WB).interfaceScreen(pPlayer.getTeam())
			elif iIndex == 3:
				self.WB.goToSubScreen("TechScreen")
			elif iIndex == 11:
				WBInfoScreen.WBInfoScreen(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 5:
				WBUnitScreen.WBUnitScreen(self.WB).interfaceScreen(pUnitOwner.getUnit(iUnitID))
			elif iIndex == 6:
				WBPromotionScreen.WBPromotionScreen(self.WB).interfaceScreen(pUnitOwner.getUnit(iUnitID))
			elif iIndex == 7:
				WBPlotScreen.WBPlotScreen(self.WB).interfaceScreen(pUnitOwner.getUnit(iUnitID).plot())
			elif iIndex == 8:
				WBEventScreen.WBEventScreen(self.WB).interfaceScreen(pUnitOwner.getUnit(iUnitID).plot())
			elif iIndex == 9:
				WBCityEditScreen.WBCityEditScreen(self.WB).interfaceScreen(pCityOwner.getCity(iCityID))
			elif iIndex == 10:
				WBCityDataScreen.WBCityDataScreen(self.WB).interfaceScreen(pCityOwner.getCity(iCityID))
			elif iIndex == 14:
				WBBuildingScreen.WBBuildingScreen(self.WB).interfaceScreen(pCityOwner.getCity(iCityID))
			elif iIndex == 12:
				WBPlotScreen.WBPlotScreen(self.WB).interfaceScreen(pCityOwner.getCity(iCityID).plot())
			elif iIndex == 13:
				WBEventScreen.WBEventScreen(self.WB).interfaceScreen(pCityOwner.getCity(iCityID).plot())

		elif sName == "CurrentPlayer":
			iIndex = screen.getPullDownData("CurrentPlayer", screen.getSelectedPullDownID("CurrentPlayer"))
			iCityID = -1
			iUnitID = -1
			self.interfaceScreen(iIndex)

		elif sName == "OwnerType":
			iOwnerType = screen.getPullDownData("OwnerType", screen.getSelectedPullDownID("OwnerType"))
			if iCityOwner != iPlayer:
				iCityID = -1
			if iUnitOwner != iPlayer:
				iUnitID = -1
			self.sortCities()
			self.sortUnits()

		elif sName == "PlotType":
			iPlotType = screen.getPullDownData("PlotType", screen.getSelectedPullDownID("PlotType"))
			if iCityOwner != iPlayer:
				iCityID = -1
			if iUnitOwner != iPlayer:
				iUnitID = -1
			self.sortCities()
			self.sortUnits()

		elif sName == "CopyType":
			iCopyType = screen.getPullDownData("CopyType", screen.getSelectedPullDownID("CopyType"))
			self.sortUnits()
			screen.hide("ActivityType")
			if iCopyType == 5:
				screen.show("ActivityType")

		elif sName == "ActivityType":
			iActivityType = screen.getPullDownData("ActivityType", screen.getSelectedPullDownID("ActivityType"))
			self.sortUnits()

		elif sName == "GoToCity":
			WBCityEditScreen.WBCityEditScreen(self.WB).interfaceScreen(pCityOwner.getCity(iCityID))

		elif sName == "GoToUnit":
			WBUnitScreen.WBUnitScreen(self.WB).interfaceScreen(pUnitOwner.getUnit(iUnitID))

		elif sName == "WBCityList":
			if inputClass.getData1() == 7872:
				iCityOwner = inputClass.getData2() /10000
				WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(iCityOwner)
			else:
				iCityID = inputClass.getData2()
				iCityOwner = inputClass.getData1() - 7200
				self.placeCurrentCity()
				self.placeCityMap()

		elif sName == "DeleteCurrentCity":
			if pCityOwner:
				pCity = pCityOwner.getCity(iCityID)
				if not pCity:
					pCity.kill()
					iCityID = -1
					self.sortCities()
					self.addPageSwitch()

		elif sName == "DeleteAllCities":
			for item in lCities:
				pPlayerX = GC.getPlayer(item[0])
				pPlayerX.getCity(item[1]).kill()
			iCityID = -1
			self.sortCities()
			self.addPageSwitch()

		elif sName == "WBUnitList":
			if inputClass.getData1() == 1043: return
			elif inputClass.getData1() == 7872:
				iUnitOwner = inputClass.getData2() /10000
				WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(iUnitOwner)
			else:
				iUnitID = inputClass.getData2()
				iUnitOwner = inputClass.getData1() - 8300
				self.placeCurrentUnit()
				self.placeUnitMap()

		elif sName == "DeleteCurrentUnit":
			if pUnitOwner:
				pUnit = pUnitOwner.getUnit(iUnitID)
				if pUnit is not None:
					pUnit.kill(False, PlayerTypes.NO_PLAYER)
					iUnitID = -1
					self.sortUnits()
					self.addPageSwitch()

		elif sName == "DeleteAllUnits":
			for item in lUnits:
				pPlayerX = GC.getPlayer(item[0])
				pPlayerX.getUnit(item[1]).kill(False, PlayerTypes.NO_PLAYER)
			iUnitID = -1
			self.sortUnits()
			self.addPageSwitch()

		elif sName == "EndCurrentUnit":
			if pUnitOwner:
				pUnit = pUnitOwner.getUnit(iUnitID)
				if pUnit is not None:
					pUnit.finishMoves()
					self.placeCurrentUnit()

		elif sName == "EndAllUnits":
			for item in lUnits:
				pPlayerX = GC.getPlayer(item[0])
				pPlayerX.getUnit(item[1]).finishMoves()
			self.placeCurrentUnit()

		return

	def update(self, fDelta):
		return 1