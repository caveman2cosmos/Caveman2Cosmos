from CvPythonExtensions import *
import CvScreenEnums
import WBCityDataScreen
import WBCityEditScreen
import WBPlayerScreen
import WBTeamScreen
import WBPlotScreen
import WBEventScreen
import WBPlayerUnits
import WBReligionScreen
import WBCorporationScreen
import WBInfoScreen
import WorldBuilder

GC = CyGlobalContext()

iChangeType = 2
iOwnerType = 0
iPlotType = 2
iSelectedClass = 0
bApplyAll = False

class WBBuildingScreen:

	def __init__(self, WB):
		self.WB = WB
		self.iTable_Y = 80
		self.lCities = []
		import CvEventInterface
		self.eventManager = CvEventInterface.getEventManager()

	def interfaceScreen(self, pCityX):
		screen = CyGInterfaceScreen("WBBuildingScreen", CvScreenEnums.WB_BUILDING)
		global pCity
		global iPlayer

		pCity = pCityX
		iPlayer = pCity.getOwner()

		screen.setRenderInterfaceOnly(True)
		screen.addPanel("MainBG", u"", u"", True, False, -10, -10, screen.getXResolution() + 20, screen.getYResolution() + 20, PanelStyles.PANEL_STYLE_MAIN )
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.setLabel("BuildingHeader", "Background", u"<font=4b>" + CyTranslator().getText("TXT_KEY_WB_BUILDINGS", ()) + "</font>", 1<<2, screen.getXResolution() * 5/8 - 10, screen.getYResolution()/2, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabel("WonderHeader", "Background", u"<font=4b>" + CyTranslator().getText("TXT_KEY_CONCEPT_WONDERS", ()) + "</font>", 1<<2, screen.getXResolution() * 5/8 - 10, 20, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_GRANT_AVAILABLE", ()) + "</color></font>"
		screen.setText("BuildingAvailable", "Background", sText, 1<<2, screen.getXResolution() * 5/8 - 10, screen.getYResolution()/2 + 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText("WonderAvailable", "Background", sText, 1<<2, screen.getXResolution() * 5/8 - 10, 50, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText("WBBuildingExit", "Background", "<font=4>" + CyTranslator().getText("TXT_WORD_EXIT", ()).upper() + "</font>", 1<<1, screen.getXResolution() - 30, screen.getYResolution() - 42, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		iWidth = screen.getXResolution()/4 - 40

		screen.addDropDownBoxGFC("OwnerType", 20, self.iTable_Y - 60, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, 0 == iOwnerType)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_PITBOSS_TEAM", ()), 2, 2, 2 == iOwnerType)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_MAIN_MENU_PLAYER", ()), 1, 1, 1 == iOwnerType)

		screen.addDropDownBoxGFC("PlotType", 20, self.iTable_Y - 30, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("PlotType", CyTranslator().getText("TXT_KEY_WB_AREA_PLOTS", ()), 1, 1, iPlotType == 1)
		screen.addPullDownString("PlotType", CyTranslator().getText("TXT_KEY_WB_ALL_PLOTS", ()), 2, 2, iPlotType == 2)

		screen.addDropDownBoxGFC("WonderClass", screen.getXResolution()/4, 50, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("WonderClass", CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, iSelectedClass == 0)
		screen.addPullDownString("WonderClass", CyTranslator().getText("TXT_KEY_PEDIA_NATIONAL_WONDER", ()), 1, 1, iSelectedClass == 1)
		screen.addPullDownString("WonderClass", CyTranslator().getText("TXT_KEY_PEDIA_TEAM_WONDER", ()), 2, 2, iSelectedClass == 2)
		screen.addPullDownString("WonderClass", CyTranslator().getText("TXT_KEY_PEDIA_WORLD_WONDER", ()), 3, 3, iSelectedClass == 3)

		screen.addDropDownBoxGFC("CurrentPage", 20, screen.getYResolution() - 42, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_CITY_DATA", ()), 0, 0, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_CITY_DATA2", ()), 1, 1, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_BUILDINGS", ()), 2, 2, True)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA", ()), 3, 3, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_TEAM_DATA", ()), 4, 4, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_RELIGION", ()), 8, 8, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_CONCEPT_CORPORATIONS", ()), 9, 9, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()), 5, 5, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLOT_DATA", ()), 6, 6, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_CONCEPT_EVENTS", ()), 7, 7, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

		sText = "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_COPY_ALL", (CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()),)) + "</font>"
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if bApplyAll:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setText("ApplyAll", "Background", sColor + sText + "</color>", 1<<0, screen.getXResolution()/4, screen.getYResolution()/2 + 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.addDropDownBoxGFC("ChangeType", screen.getXResolution() - 170, 20, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_MODIFY", ("",)), 2, 2, 2 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_CITY_ADD", ()), 1, 1, 1 == iChangeType)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_CITY_REMOVE", ()), 0, 0, 0 == iChangeType)

		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=4b>" + CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()) + " (+/-)</color></font>"
		screen.setText("BuildingAll", "Background", sText, 1<<1, screen.getXResolution() - 20, screen.getYResolution()/2 + 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText("WonderAll", "Background", sText, 1<<1, screen.getXResolution() - 20, 50, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		self.sortCities()
		self.sortBuildings()

	def sortCities(self):
		self.lCities = []
		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(iPlayerX)
			if iOwnerType == 1 and iPlayerX != iPlayer: continue
			if iOwnerType == 2 and pPlayerX.getTeam() != pCity.getTeam(): continue
			for loopCity in pPlayerX.cities():
				if iPlotType == 2 or (iPlotType == 1 and loopCity.plot().getArea() == pCity.plot().getArea()):
					sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
					if loopCity.getID() == pCity.getID() and iPlayerX == iPlayer:
						sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
					self.lCities.append([loopCity, iPlayerX, sColor])
		self.placeCityTable()

	def placeCityTable(self):
		screen = CyGInterfaceScreen( "WBBuildingScreen", CvScreenEnums.WB_BUILDING)
		iWidth = screen.getXResolution()/4 - 40
		iHeight = (screen.getYResolution() - 40 - self.iTable_Y) / 24 * 24 + 2
		screen.addTableControlGFC( "CurrentCity", 3, 20, self.iTable_Y, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
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

	def sortBuildings(self):
		global lBuilding
		global lNational
		global lTeam
		global lWorld
		lBuilding = []
		lNational = []
		lTeam = []
		lWorld = []
		for i in xrange(GC.getNumBuildingInfos()):
			if isNationalWonder(i):
				lNational.append([GC.getBuildingInfo(i).getDescription(), i])
			elif isTeamWonder(i):
				lTeam.append([GC.getBuildingInfo(i).getDescription(), i])
			elif isWorldWonder(i):
				lWorld.append([GC.getBuildingInfo(i).getDescription(), i])
			else:
				lBuilding.append([GC.getBuildingInfo(i).getDescription(), i])
		lNational.sort()
		lTeam.sort()
		lWorld.sort()
		lBuilding.sort()
		self.placeBuildings()
		self.placeWonders()

	def placeBuildings(self):
		screen = CyGInterfaceScreen( "WBBuildingScreen", CvScreenEnums.WB_BUILDING)
		iWidth = screen.getXResolution() *3/4 - 20
		iMaxRows = (screen.getYResolution()/2 - 102) / 24
		nColumns = max(1, min(iWidth/180, (len(lBuilding) + iMaxRows - 1)/iMaxRows))
		iHeight = iMaxRows * 24 + 2
		screen.addTableControlGFC("WBBuilding", nColumns, screen.getXResolution()/4, screen.getYResolution()/2 + 60, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		for i in xrange(nColumns):
			screen.setTableColumnHeader( "WBBuilding", i, "", iWidth/nColumns)

		nRows = len(lBuilding)/ nColumns
		if len(lBuilding) % nColumns:
			nRows += 1
		for i in xrange(nRows):
			screen.appendTableRow("WBBuilding")

		for iCount in xrange(len(lBuilding)):
			item = lBuilding[iCount]
			iRow = iCount % nRows
			iColumn = iCount / nRows
			ItemInfo = GC.getBuildingInfo(item[1])
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if pCity.getNumRealBuilding(item[1]):
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			elif pCity.getNumRealBuilding(item[1]):
				sColor = CyTranslator().getText("[COLOR_YELLOW]", ())
			screen.setTableText("WBBuilding", iColumn, iRow, "<font=3>" + sColor + item[0] + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_HELP_BUILDING, item[1], 1, 1<<0 )

	def placeWonders(self):
		screen = CyGInterfaceScreen( "WBBuildingScreen", CvScreenEnums.WB_BUILDING)
		if iSelectedClass == 0:
			lWonders = lNational + lTeam + lWorld
		elif iSelectedClass == 1:
			lWonders = lNational
		elif iSelectedClass == 2:
			lWonders = lTeam
		else:
			lWonders = lWorld

		iWidth = screen.getXResolution() *3/4 - 20
		iMaxRows = (screen.getYResolution()/2 - self.iTable_Y) / 24
		nColumns = max(1, min(iWidth/180, (len(lBuilding) + iMaxRows - 1)/iMaxRows))
		iHeight = iMaxRows * 24 + 2
		screen.addTableControlGFC("WBWonders", nColumns, screen.getXResolution()/4, self.iTable_Y, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		for i in xrange(nColumns):
			screen.setTableColumnHeader( "WBWonders", i, "", iWidth/nColumns)

		nRows = (len(lWonders) + nColumns - 1) / nColumns
		for i in xrange(nRows):
			screen.appendTableRow("WBWonders")

		for iCount in xrange(len(lWonders)):
			item = lWonders[iCount]
			iRow = iCount % nRows
			iColumn = iCount / nRows
			ItemInfo = GC.getBuildingInfo(item[1])
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if pCity.getNumRealBuilding(item[1]):
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			elif pCity.getNumRealBuilding(item[1]):
				sColor = CyTranslator().getText("[COLOR_YELLOW]", ())
			screen.setTableText("WBWonders", iColumn, iRow, "<font=3>" + sColor + item[0] + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_HELP_BUILDING, item[1], 1, 1<<0 )

	def handleInput (self, inputClass):
		screen = CyGInterfaceScreen( "WBBuildingScreen", CvScreenEnums.WB_BUILDING)
		global bApplyAll
		global iSelectedClass
		global iChangeType
		global iOwnerType
		global iPlotType

		if inputClass.getFunctionName() == "CurrentPage":
			iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
			if iIndex == 0:
				WBCityEditScreen.WBCityEditScreen(self.WB).interfaceScreen(pCity)
			elif iIndex == 1:
				WBCityDataScreen.WBCityDataScreen(self.WB).interfaceScreen(pCity)
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

		elif inputClass.getFunctionName() == "ChangeType":
			iChangeType = screen.getPullDownData("ChangeType", screen.getSelectedPullDownID("ChangeType"))

		elif inputClass.getFunctionName() == "CurrentCity":
			iPlayerX = inputClass.getData1() - 7200
			pPlayerX = GC.getPlayer(iPlayerX)
			if pPlayerX:
				self.interfaceScreen(pPlayerX.getCity(inputClass.getData2()))

		elif inputClass.getFunctionName() == "WBBuilding":
			bUpdate = self.editBuilding(inputClass.getData1(), GC.getPlayer(iPlayer), False, False)
			self.placeBuildings()
			if bUpdate:
				self.placeWonders()

		elif inputClass.getFunctionName() == "BuildingAvailable":
			bUpdate = False
			for item in lBuilding:
				bTemp = self.editBuilding(item[1], GC.getPlayer(iPlayer), True, False)
				bUpdate = bUpdate or bTemp
			self.placeBuildings()
			if bUpdate:
				self.placeWonders()

		elif inputClass.getFunctionName() == "BuildingAll":
			bUpdate = False
			for item in lBuilding:
				bTemp = self.editBuilding(item[1], GC.getPlayer(iPlayer), False, False)
				bUpdate = bUpdate or bTemp
			self.placeBuildings()
			if bUpdate:
				self.placeWonders()

		elif inputClass.getFunctionName() == "WonderClass":
			iSelectedClass = inputClass.getData()
			self.placeWonders()

		elif inputClass.getFunctionName() == "WBWonders":
			bUpdate = self.editBuilding(inputClass.getData1(), GC.getPlayer(iPlayer), False, True)
			self.placeWonders()
			if bUpdate:
				self.placeBuildings()

		elif inputClass.getFunctionName() == "WonderAvailable":
			bUpdate = False
			lList = lWorld
			if iSelectedClass == 0:
				lList = lNational + lTeam + lWorld
			elif iSelectedClass == 1:
				lList = lNational
			elif iSelectedClass == 2:
				lList = lTeam
			for item in lList:
				bTemp = self.editBuilding(item[1], GC.getPlayer(iPlayer), True, True)
				bUpdate = bUpdate or bTemp
			self.placeWonders()
			if bUpdate:
				self.placeBuildings()

		elif inputClass.getFunctionName() == "WonderAll":
			bUpdate = False
			lList = lWorld
			if iSelectedClass == 0:
				lList = lNational + lTeam + lWorld
			elif iSelectedClass == 1:
				lList = lNational
			elif iSelectedClass == 2:
				lList = lTeam
			for item in lList:
				bTemp = self.editBuilding(item[1], GC.getPlayer(iPlayer), False, True)
				bUpdate = bUpdate or bTemp
			self.placeWonders()
			if bUpdate:
				self.placeBuildings()

		elif inputClass.getFunctionName() == "ApplyAll":
			bApplyAll = not bApplyAll
			sText = u"<font=3b>" + CyTranslator().getText("TXT_KEY_WB_COPY_ALL", (CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()),)) + "</font>"
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if bApplyAll:
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			screen.modifyString("ApplyAll", sColor + sText + "</color>", 0)
		return 1

	def editBuilding(self, item, pPlayerX, bAvailable, bWonder):
		info = GC.getBuildingInfo(item)
		iType = iChangeType or bAvailable
		if bApplyAll and not bWonder:
			for cityX in pPlayerX.cities():
				bModify = True
				if info.isWater() and not cityX.isCoastal(info.getMinAreaSize()): bModify = False
				if info.isRiver() and not cityX.plot().isRiver(): bModify = False
				if bAvailable:
					if info.isCapital(): bModify = False
					iHolyReligion = info.getHolyCity()
					if iHolyReligion > -1 and not cityX.isHolyCityByType(iHolyReligion): bModify = False
					if not cityX.canConstruct(item, True, False, True): bModify = False
				if bModify:
					if iChangeType == 2 and not bAvailable:
						iType = not cityX.getNumRealBuilding(item)
					self.doEffects(cityX, item, iType)
		else:
			if bAvailable:
				if info.isCapital(): return
				iHolyReligion = info.getHolyCity()
				if iHolyReligion > -1 and not pCity.isHolyCityByType(iHolyReligion): return
				if not pCity.canConstruct(item, True, False, True): return
			if iChangeType == 2 and not bAvailable:
				iType = not pCity.getNumRealBuilding(item)
			self.doEffects(pCity, item, iType)
		iFreeBuilding = info.getFreeBuilding()
		if iFreeBuilding > -1 and bWonder != isLimitedWonder(iFreeBuilding):
			return True
		return False

	def doEffects(self, pCity, item, bAdd):
		bEffects = False
		if bAdd and WorldBuilder.bPython and pCity.getNumRealBuilding(item) == 0:
			bEffects = True
		pCity.setNumRealBuilding(item, bAdd)
		if bEffects:
			self.eventManager.onBuildingBuilt([pCity, item])

	def update(self, fDelta):
		return 1