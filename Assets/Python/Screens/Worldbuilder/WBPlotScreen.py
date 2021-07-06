from CvPythonExtensions import *
import CvScreenEnums
import WBEventScreen
import WBPlayerUnits
import WBCityEditScreen
import WBUnitScreen
import WBPlayerScreen
import WBTeamScreen
import WBInfoScreen
import Popup

GC = CyGlobalContext()

bAdd = True
bSensibility = True
iEditType = 0
iChange = 1
iCounter = -1
iCulturePlayer = 0
iSelectedClass = -1

class WBPlotScreen:

	def __init__(self, WB):
		self.WB = WB
		self.iTable_Y = 110

	def interfaceScreen(self, pPlotX):
		screen = CyGInterfaceScreen( "WBPlotScreen", CvScreenEnums.WB_PLOT)
		global pPlot
		global iWidth
		pPlot = pPlotX
		iWidth = screen.getXResolution()/5 - 20

		screen.setRenderInterfaceOnly(True)
		screen.addPanel( "MainBG", u"", u"", True, False, -10, -10, screen.getXResolution() + 20, screen.getYResolution() + 20, PanelStyles.PANEL_STYLE_MAIN )
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.setText("PlotExit", "Background", "<font=4>" + CyTranslator().getText("TXT_WORD_EXIT", ()).upper() + "</font>", 1<<1, screen.getXResolution() - 30, screen.getYResolution() - 42, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		iX = 10
		iY = 50
		screen.addDropDownBoxGFC("CurrentPlayer", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CurrentPlayer", CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ()), -1, -1, pPlot.getOwner() == -1)
		for i in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(i)
			if pPlayerX.isAlive():
				screen.addPullDownString("CurrentPlayer", pPlayerX.getName(), i, i, i == pPlot.getOwner())

		iY += 30
		screen.addDropDownBoxGFC("ChangeType", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_CITY_ADD", ()), 1, 1, bAdd)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_CITY_REMOVE", ()), 0, 0, not bAdd)

		iY += 30
		iButtonWidth = 28
		screen.addCheckBoxGFC("SensibilityCheck", ",Art/Interface/Buttons/WorldBuilder/Gems.dds,Art/Interface/Buttons/FinalFrontier1_Atlas.dds,1,16", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
					 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 24, ButtonStyles.BUTTON_STYLE_LABEL)
		screen.setState("SensibilityCheck", bSensibility)
		screen.addDropDownBoxGFC("EditType", iX + iButtonWidth, iY, iWidth - iButtonWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("EditType", CyTranslator().getText("TXT_KEY_WB_SINGLE_PLOT", ()), 0, 0, iEditType == 0)
		screen.addPullDownString("EditType", CyTranslator().getText("TXT_KEY_WB_AREA_PLOTS", ()), 1, 1, iEditType == 1)
		screen.addPullDownString("EditType", CyTranslator().getText("TXT_KEY_WB_ALL_PLOTS", ()), 2, 2, iEditType == 2)

		iY += 30
		screen.addDropDownBoxGFC("ChangeBy", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		i = 1
		while i < 1000001:
			screen.addPullDownString("ChangeBy", "(+/-) " + str(i), i, i, iChange == i)
			if str(i)[0] == "1":
				i *= 5
			else:
				i *= 2

		screen.addDropDownBoxGFC("CurrentPage", iX, screen.getYResolution() - 42, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLOT_DATA", ()), 0, 0, True)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_CONCEPT_EVENTS", ()), 1, 1, False)
		if pPlot.isOwned():
			screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA", ()), 2, 2, False)
			screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_TEAM_DATA", ()), 3, 3, False)
			screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()), 6, 6, False)
			if pPlot.isCity():
				screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_CITY_DATA", ()), 4, 4, False)
		if pPlot.getNumUnits() > 0:
			screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_UNIT_DATA", ()), 5, 5, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

		iIndex = -1
		for i in xrange(CyMap().numPlots()):
			pLoopPlot = CyMap().plotByIndex(i)
			if pLoopPlot.getX() == pPlot.getX() and pLoopPlot.getY() == pPlot.getY():
				iIndex = i
				break
		sText = CyTranslator().getText("TXT_KEY_WB_PLOT_DATA", ())
		if pPlot.isCity():
			pCity = pPlot.getPlotCity()
			sText += " (" + pCity.getName() + ")"
		screen.setLabel("PlotScreenHeader", "Background", "<font=4b>" + sText, 1<<2, screen.getXResolution()/2, 20, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		sText = u"<font=3b>%s ID: %d, %s: %d</font>" %(CyTranslator().getText("TXT_WORD_PLOT", ()), iIndex, CyTranslator().getText("TXT_KEY_WB_AREA_ID", ()), pPlot.getArea())
		screen.setLabel("PlotScreenHeaderB", "Background", "<font=4b>" + sText + "</font>", 1<<2, screen.getXResolution()/2, 50, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		sText = "<font=3b>%s, X: %d, Y: %d</font>" %(CyTranslator().getText("TXT_KEY_WB_LATITUDE",(pPlot.getLatitude(),)), pPlot.getX(), pPlot.getY())
		screen.setLabel("PlotLocation", "Background", sText, 1<<2, screen.getXResolution()/2, 70, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.addDropDownBoxGFC("BonusClass", screen.getXResolution() *4/5 + 10, self.iTable_Y - 60, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("BonusClass", CyTranslator().getText("TXT_KEY_WB_CITY_ALL",()), -1, -1, True)
		screen.addPullDownString("BonusClass", CyTranslator().getText("TXT_KEY_GLOBELAYER_RESOURCES_GENERAL",()), 0, 0, 0 == iSelectedClass)
		iBonusClass = 1
		while not GC.getBonusClassInfo(iBonusClass) is None:
			sText = GC.getBonusClassInfo(iBonusClass).getType()
			sText = sText[sText.find("_") +1:]
			sText = sText.lower()
			sText = sText.capitalize()
			screen.addPullDownString("BonusClass", sText, iBonusClass, iBonusClass, iBonusClass == iSelectedClass)
			iBonusClass += 1

		screen.setImageButton("NextPlotUpButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_UPARROW").getPath(),
				screen.getXResolution()/2 - 12, self.iTable_Y, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setImageButton("NextPlotDownButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_DOWNARROW").getPath(),
				screen.getXResolution()/2 - 12, self.iTable_Y + 48, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setImageButton("NextPlotLeftButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_LEFT").getPath(),
				screen.getXResolution()/2 - 36, self.iTable_Y + 24, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setImageButton("NextPlotRightButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_RIGHT").getPath(),
				screen.getXResolution()/2 + 12, self.iTable_Y + 24, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1)

		global lRoutes
		global lFeatures
		global lImprovements

		lFeatures = []
		for i in xrange(GC.getNumFeatureInfos()):
			ItemInfo = GC.getFeatureInfo(i)
			for j in xrange(ItemInfo.getNumVarieties()):
				sText = ItemInfo.getDescription()
				if ItemInfo.getNumVarieties() > 1:
					sText += " (" + str(j) + ")"
				lFeatures.append([sText, j * 10000 + i])
		lFeatures.sort()

		lRoutes = []
		for i in xrange(GC.getNumRouteInfos()):
			ItemInfo = GC.getRouteInfo(i)
			lRoutes.append([ItemInfo.getDescription(), i])
		lRoutes.sort()

		lImprovements = []
		for i in xrange(GC.getNumImprovementInfos()):
			ItemInfo = GC.getImprovementInfo(i)
			if ItemInfo.isGraphicalOnly(): continue
			lImprovements.append([ItemInfo.getDescription(), i])
		lImprovements.sort()

		self.createBonusList()
		self.placeScript()
		self.placeStats()
		self.placePlotType()
		self.placeTerrain()
		self.placeFeature()
		self.placeRoutes()
		self.placeImprovements()

	def placeSigns(self):
		screen = CyGInterfaceScreen("WBPlotScreen", CvScreenEnums.WB_PLOT)
		iX = 10
		iY = screen.getYResolution()/2
		iSignWidth = screen.getXResolution() * 2/5 - 20
		iHeight = (screen.getYResolution() - 42 - iY) /24 * 24 + 2

		screen.setButtonGFC("EditCulturePlus", "", "", 10, iY - 30, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("EditCultureMinus", "", "", 35, iY - 30, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = CyTranslator().getText("[ICON_CULTURE]", ()) + GC.getPlayer(iCulturePlayer).getName()
		screen.setLabel("PlotCultureText", "Background", "<font=3>" + sText + "</font>", 1<<0, 65, iY - 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_LANDMARKS", ()) + "</color></font>"
		screen.setText("EditLandMark", "Background", sText, 1<<1, iX + iSignWidth, iY - 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		lSigns = []
		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			lSigns.append(-1)
		for i in xrange(CyEngine().getNumSigns()):
			pSign = CyEngine().getSignByIndex(i)
			if pSign.getPlot().getX() != pPlot.getX(): continue
			if pSign.getPlot().getY() != pPlot.getY(): continue
			lSigns[pSign.getPlayerType()] = i

		screen.addTableControlGFC("WBSigns", 4, iX, iY, iSignWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBSigns", 0, "", 24)
		screen.setTableColumnHeader("WBSigns", 1, "", 24)
		screen.setTableColumnHeader("WBSigns", 2, "", (iSignWidth - 48) /3)
		screen.setTableColumnHeader("WBSigns", 3, "", (iSignWidth - 48) *2/3)

		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(iPlayerX)
			if pPlayerX.isEverAlive():
				iRow = screen.appendTableRow("WBSigns")
				iCivilization = pPlayerX.getCivilizationType()
				iLeader = pPlayerX.getLeaderType()
				screen.setTableText("WBSigns", 0, iRow, "", GC.getCivilizationInfo(iCivilization).getButton(), WidgetTypes.WIDGET_PYTHON, 7872, iPlayerX * 10000 + iCivilization, 1<<0 )
				screen.setTableText("WBSigns", 1, iRow, "", GC.getLeaderHeadInfo(iLeader).getButton(), WidgetTypes.WIDGET_PYTHON, 7876, iPlayerX * 10000 + iLeader, 1<<0 )
				sText = "<font=3>" + self.WB.addComma(pPlot.getCulture(iPlayerX)) + CyTranslator().getText("[ICON_CULTURE]", ()) + "</font>"
				screen.setTableText("WBSigns", 2, iRow, sText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<1)
				iIndex = lSigns[iPlayerX]
				sText = ""
				if iIndex > -1:
					sText = CyEngine().getSignByIndex(iIndex).getCaption()
				screen.setTableText("WBSigns", 3, iRow, "<font=3>" + sText + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

	def placeMap(self):
		screen = CyGInterfaceScreen("WBPlotScreen", CvScreenEnums.WB_PLOT)
		iMapHeight = min((screen.getYResolution()/2 - 30 - (self.iTable_Y + 48 + 24)), iWidth * 2/3)
		iMapWidth = iMapHeight * 3/2
		screen.addPlotGraphicGFC("PlotView", screen.getXResolution()/2 - iMapWidth/2, screen.getYResolution()/2 - 30 - iMapHeight, iMapWidth, iMapHeight, pPlot, 350, False, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def placeStats(self):
		screen = CyGInterfaceScreen("WBPlotScreen", CvScreenEnums.WB_PLOT)
		iY = 180
		screen.setLabel("YieldHeader", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_PEDIA_YIELDS", ()) + "</font>", 1<<2, screen.getXResolution()/10, iY, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		iY += 30
		for i in xrange(YieldTypes.NUM_YIELD_TYPES):
			iX = 10
			iYield = pPlot.getYield(YieldTypes(i))
			iImprovement = pPlot.getImprovementType()
			if iImprovement > -1:
				iYield -= pPlot.calculateImprovementYieldChange(iImprovement, YieldTypes(i), pPlot.getOwner(), False)
			screen.setButtonGFC("BaseYieldPlus" + str(i), "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, i, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
			screen.setButtonGFC("BaseYieldMinus" + str(i), "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, i, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
			sText = CyTranslator().getText("TXT_KEY_WB_BASE_YIELD", (GC.getYieldInfo(i).getDescription(), iYield,))
			sText = u"%s%c" %(sText, GC.getYieldInfo(i).getChar())
			screen.setLabel("BaseYieldText" + str(i), "Background", "<font=3>" + sText + "</font>", 1<<0, iX + 50, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			iY += 30
		self.placeSigns()
		self.placeRivers()

	def placeRivers(self):
		screen = CyGInterfaceScreen( "WBPlotScreen", CvScreenEnums.WB_PLOT)
		iX = screen.getXResolution()/2
		iY = screen.getYResolution()/2
		screen.setLabel("PlotRiverHeader", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_MISC_RIVERS", ()) + "</font>", 1<<2, iX, iY - 30, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		pWestPlot = CyMap().plot(pPlot.getX() - 1, pPlot.getY())
		pNorthPlot = CyMap().plot(pPlot.getX(), pPlot.getY() + 1)
		if not pNorthPlot.isNone():
			screen.addCheckBoxGFC("RiverNorthAButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_LEFT").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX - 36, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			screen.addCheckBoxGFC("RiverNorthBButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX - 12, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			screen.addCheckBoxGFC("RiverNorthCButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_RIGHT").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX + 12, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			screen.setState("RiverNorthAButton", pNorthPlot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_WEST)
			screen.setState("RiverNorthBButton", pNorthPlot.getRiverWEDirection() == CardinalDirectionTypes.NO_CARDINALDIRECTION)
			screen.setState("RiverNorthCButton", pNorthPlot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_EAST)

		iY += 24
		if not pWestPlot.isNone():
			screen.addCheckBoxGFC("RiverWestAButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_UPARROW").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX - 60, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		screen.addCheckBoxGFC("RiverEastAButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_UPARROW").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX + 36, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		iY += 24
		if not pWestPlot.isNone():
			screen.addCheckBoxGFC("RiverWestBButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX - 60, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		screen.addCheckBoxGFC("RiverEastBButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX + 36, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		iY += 24
		if not pWestPlot.isNone():
			screen.addCheckBoxGFC("RiverWestCButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_DOWNARROW").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX - 60, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		screen.addCheckBoxGFC("RiverEastCButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_DOWNARROW").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX + 36, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		if not pWestPlot.isNone():
			screen.setState("RiverWestAButton", pWestPlot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
			screen.setState("RiverWestBButton", pWestPlot.getRiverNSDirection() == CardinalDirectionTypes.NO_CARDINALDIRECTION)
			screen.setState("RiverWestCButton", pWestPlot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)

		screen.setState("RiverEastAButton", pPlot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		screen.setState("RiverEastBButton", pPlot.getRiverNSDirection() == CardinalDirectionTypes.NO_CARDINALDIRECTION)
		screen.setState("RiverEastCButton", pPlot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)

		iY += 24
		screen.addCheckBoxGFC("RiverSouthAButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_LEFT").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX - 36, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		screen.addCheckBoxGFC("RiverSouthBButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX - 12, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		screen.addCheckBoxGFC("RiverSouthCButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_RIGHT").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX + 12, iY, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		screen.setState("RiverSouthAButton", pPlot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_WEST)
		screen.setState("RiverSouthBButton", pPlot.getRiverWEDirection() == CardinalDirectionTypes.NO_CARDINALDIRECTION)
		screen.setState("RiverSouthCButton", pPlot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_EAST)
		global iPlotType_Y
		iPlotType_Y = iY + 30

	def placeRoutes(self):
		screen = CyGInterfaceScreen( "WBPlotScreen", CvScreenEnums.WB_PLOT)
		iX = screen.getXResolution() *4/5 + 10
		iY = screen.getYResolution()/2
		iHeight = (screen.getYResolution() - 42 - iY) /24 * 24 + 2
		iRoute = pPlot.getRouteType()
		sText = CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
		sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		if iRoute > -1:
			sText = GC.getRouteInfo(iRoute).getDescription()
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		screen.setLabel("RouteHeader", "Background", "<font=3b>" + sText + "</font>", 1<<2, iX + screen.getXResolution()/10 - 10, iY - 30, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addTableControlGFC("WBPlotRoutes", 1, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBPlotRoutes", 0, "", iWidth)

		screen.appendTableRow("WBPlotRoutes")
		screen.setTableText("WBPlotRoutes", 0, 0, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), WidgetTypes.WIDGET_PYTHON, 6788, -1, 1<<0 )
		for item in lRoutes:
			iRow = screen.appendTableRow("WBPlotRoutes")
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if iRoute == item[1]:
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			screen.setTableText("WBPlotRoutes", 0, iRow, "<font=3>" + sColor + item[0] + "</font></color>", GC.getRouteInfo(item[1]).getButton(), WidgetTypes.WIDGET_PYTHON, 6788, item[1], 1<<0 )

	def placeFeature(self):
		screen = CyGInterfaceScreen( "WBPlotScreen", CvScreenEnums.WB_PLOT)
		iX = screen.getXResolution() *3/5 + 10
		iY = self.iTable_Y
		iHeight = (screen.getYResolution()/2 - 32 - iY) /24 * 24 + 2
		iFeature = pPlot.getFeatureType()
		iVariety = pPlot.getFeatureVariety()
		sText = CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
		sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		if iFeature > -1:
			sText = GC.getFeatureInfo(iFeature).getDescription()
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())

		screen.setLabel("FeatureHeader", "Background", "<font=3b>" + sText + "</font>", 1<<2, iX + screen.getXResolution()/10 - 10, iY - 30, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addTableControlGFC("WBPlotFeature", 1, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBPlotFeature", 0, "", iWidth)

		screen.appendTableRow("WBPlotFeature")
		screen.setTableText("WBPlotFeature", 0, 0, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), WidgetTypes.WIDGET_PYTHON, 7874, -1, 1<<0 )
		for item in lFeatures:
			iRow = screen.appendTableRow("WBPlotFeature")
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			iType = item[1] % 10000
			if iFeature == iType and iVariety == item[1] / 10000:
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			screen.setTableText("WBPlotFeature", 0, iRow, "<font=3>" + sColor + item[0] + "</font></color>", GC.getFeatureInfo(iType).getButton(), WidgetTypes.WIDGET_PYTHON, 7874, item[1], 1<<0 )

	def placeImprovements(self):
		screen = CyGInterfaceScreen( "WBPlotScreen", CvScreenEnums.WB_PLOT)
		screen.hide("UpgradeTimePlus")
		screen.hide("UpgradeTimeMinus")
		screen.hide("UpgradeTimeText")
		iX = screen.getXResolution() *3/5 + 10
		iY = screen.getYResolution()/2
		iUpgrade_Y = screen.getYResolution() - 70
		iHeight = (iUpgrade_Y - iY) /24 * 24 + 2
		iImprovement = pPlot.getImprovementType()
		sText = CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
		sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		if iImprovement > -1:
			if GC.getImprovementInfo(iImprovement).getUpgradeTime():
				screen.setButtonGFC("UpgradeTimePlus", "", "", iX, iUpgrade_Y, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
				screen.setButtonGFC("UpgradeTimeMinus", "", "", iX + 25, iUpgrade_Y, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
				sText = CyTranslator().getText("TXT_KEY_WB_UPGRADE_PROGRESS", (pPlot.getUpgradeTimeLeft(iImprovement, pPlot.getOwner()),))
				screen.setLabel("UpgradeTimeText", "Background", "<font=3>" + sText + "</font>", 1<<0, iX + 55, iUpgrade_Y + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			sText = GC.getImprovementInfo(iImprovement).getDescription()
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		screen.setLabel("ImprovementHeader", "Background", "<font=3b>" + sText + "</font>", 1<<2, iX + screen.getXResolution()/10 - 10, iY - 30, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addTableControlGFC("WBPlotImprovement", 1, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBPlotImprovement", 0, "", iWidth)

		screen.appendTableRow("WBPlotImprovement")
		screen.setTableText("WBPlotImprovement", 0, 0, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), WidgetTypes.WIDGET_PYTHON, 7877, -1, 1<<0 )
		for item in lImprovements:
			iRow = screen.appendTableRow("WBPlotImprovement")
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if iImprovement == item[1]:
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			screen.setTableText("WBPlotImprovement", 0, iRow, "<font=3>" + sColor + item[0] + "</font></color>", GC.getImprovementInfo(item[1]).getButton(), WidgetTypes.WIDGET_PYTHON, 7877, item[1], 1<<0 )

	def createBonusList(self):
		global lBonus
		lBonus = []
		for i in xrange(GC.getNumBonusInfos()):
			ItemInfo = GC.getBonusInfo(i)
			if iSelectedClass != ItemInfo.getBonusClassType() and iSelectedClass > -1: continue
			lBonus.append([ItemInfo.getDescription(), i])
		lBonus.sort()
		self.placeBonus()

	def placeBonus(self):
		screen = CyGInterfaceScreen( "WBPlotScreen", CvScreenEnums.WB_PLOT)
		iX = screen.getXResolution() *4/5 + 10
		iY = self.iTable_Y
		iHeight = (screen.getYResolution()/2 - 32 - iY) /24 * 24 + 2
		iBonus = pPlot.getBonusType(-1)
		sText = CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
		sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		if iBonus > -1:
			sText = GC.getBonusInfo(iBonus).getDescription()
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		screen.setLabel("BonusHeader", "Background", "<font=3b>" + sText + "</font>", 1<<2, iX + screen.getXResolution()/10 - 10, iY - 30, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addTableControlGFC("WBPlotBonus", 1, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBPlotBonus", 0, "", iWidth)

		screen.appendTableRow("WBPlotBonus")
		screen.setTableText("WBPlotBonus", 0, 0, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), WidgetTypes.WIDGET_PYTHON, 6788, -1, 1<<0 )
		for item in lBonus:
			iRow = screen.appendTableRow("WBPlotBonus")
			ItemInfo = GC.getBonusInfo(item[1])
			sButton = ItemInfo.getButton()
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if iBonus == item[1]:
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			screen.setTableText("WBPlotBonus", 0, iRow, "<font=3>" + sColor + item[0] + "</font></color>", sButton, WidgetTypes.WIDGET_PYTHON, 7878, item[1], 1<<0 )

	def placeTerrain(self):
		screen = CyGInterfaceScreen( "WBPlotScreen", CvScreenEnums.WB_PLOT)
		iX = screen.getXResolution() /5 + 10
		iY = self.iTable_Y
		iHeight = (screen.getYResolution()/2 - 32 - iY) /24 * 24 + 2

		iTerrain = pPlot.getTerrainType()
		sText = GC.getTerrainInfo(iTerrain).getDescription()
		screen.setLabel("TerrainHeader", "Background", "<font=3b>" + sText + "</font>", 1<<2, iX + screen.getXResolution()/10 - 10, iY - 30, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addTableControlGFC("WBPlotTerrain", 1, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBPlotTerrain", 0, "", iWidth)

		for i in xrange(GC.getNumTerrainInfos()):
			TerrainInfo = GC.getTerrainInfo(i)
			if TerrainInfo.isGraphicalOnly(): continue
			if TerrainInfo.isWaterTerrain() != pPlot.isWater(): continue
			iRow = screen.appendTableRow("WBPlotTerrain")
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if iTerrain == i:
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			sText = "<font=3>" + sColor + TerrainInfo.getDescription() + "</font></color>"
			screen.setTableText("WBPlotTerrain", 0, iRow, sText, TerrainInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 7875, i, 1<<0)

	def placeScript(self):
		screen = CyGInterfaceScreen("WBPlotScreen", CvScreenEnums.WB_PLOT)
		global iScript_Y
		iScript_Y = screen.getYResolution() - 120
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_SCRIPT_DATA", ()) + "</color></font>"
		screen.setText("PlotEditScriptData", "Background", sText, 1<<2, screen.getXResolution()/2, iScript_Y, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel( "ScriptPanel", "", "", False, False, screen.getXResolution()/2 - iWidth/2, iScript_Y + 25, iWidth, screen.getYResolution() - iScript_Y - 70, PanelStyles.PANEL_STYLE_IN)
		screen.addMultilineText("GameScriptDataText", pPlot.getScriptData(), screen.getXResolution()/2 - iWidth/2, iScript_Y + 25, iWidth, screen.getYResolution() - iScript_Y - 70, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

	def placePlotType(self):
		screen = CyGInterfaceScreen( "WBPlotScreen", CvScreenEnums.WB_PLOT)
		iX = screen.getXResolution() *2/5 + 10
		iY = iPlotType_Y
		iHeight = min(PlotTypes.NUM_PLOT_TYPES, (iScript_Y - iPlotType_Y)/24) * 24 + 2

		screen.addTableControlGFC("WBPlotType", 1, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBPlotType", 0, "", iWidth)
		for iTerrain in xrange(PlotTypes.NUM_PLOT_TYPES):
			screen.appendTableRow("WBPlotType")

		iTerrain = GC.getInfoTypeForString("TERRAIN_PEAK")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		TerrainInfo = GC.getTerrainInfo(iTerrain)
		if pPlot.getPlotType() == PlotTypes.PLOT_PEAK:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			sHeader = TerrainInfo.getDescription()
		sText = "<font=3>" + sColor + TerrainInfo.getDescription() + "</font></color>"
		screen.setTableText("WBPlotType", 0, 0, sText, TerrainInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 7875, iTerrain, 1<<0)

		iTerrain = GC.getInfoTypeForString("TERRAIN_HILL")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		TerrainInfo = GC.getTerrainInfo(iTerrain)
		if pPlot.getPlotType() == PlotTypes.PLOT_HILLS:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			sHeader = TerrainInfo.getDescription()
		sText = "<font=3>" + sColor + TerrainInfo.getDescription() + "</font></color>"
		screen.setTableText("WBPlotType", 0, 1, sText, TerrainInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 7875, iTerrain, 1<<0)

		iTerrain = GC.getInfoTypeForString("TERRAIN_GRASSLAND")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		TerrainInfo = GC.getTerrainInfo(iTerrain)
		if pPlot.getPlotType() == PlotTypes.PLOT_LAND:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			sHeader = TerrainInfo.getDescription()
		sText = "<font=3>" + sColor + TerrainInfo.getDescription() + "</font></color>"
		screen.setTableText("WBPlotType", 0, 2, sText, TerrainInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 7875, iTerrain, 1<<0)

		iTerrain = GC.getInfoTypeForString("TERRAIN_OCEAN")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		TerrainInfo = GC.getTerrainInfo(iTerrain)
		if pPlot.getPlotType() == PlotTypes.PLOT_OCEAN:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			sHeader = TerrainInfo.getDescription()
		sText = "<font=3>" + sColor + TerrainInfo.getDescription() + "</font></color>"
		screen.setTableText("WBPlotType", 0, 3, sText, TerrainInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 7875, iTerrain, 1<<0)

	def handleInput(self, inputClass):
		screen = CyGInterfaceScreen("WBPlotScreen", CvScreenEnums.WB_PLOT)
		global bAdd
		global bSensibility
		global iEditType
		global iChange
		global iCulturePlayer
		global iSelectedClass

		if inputClass.getFunctionName() == "ChangeBy":
			iChange = screen.getPullDownData("ChangeBy", screen.getSelectedPullDownID("ChangeBy"))

		elif inputClass.getFunctionName() == "CurrentPlayer":
			iIndex = screen.getPullDownData("CurrentPlayer", screen.getSelectedPullDownID("CurrentPlayer"))
			pPlot.setOwner(iIndex)

		elif inputClass.getFunctionName() == "ChangeType":
			bAdd = screen.getPullDownData("ChangeType", screen.getSelectedPullDownID("ChangeType"))

		elif inputClass.getFunctionName() == "EditType":
			iEditType = screen.getPullDownData("EditType", screen.getSelectedPullDownID("EditType"))

		elif inputClass.getFunctionName() == "CurrentPage":
			iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
			iPlayer = pPlot.getOwner()
			if iIndex == 1:
				WBEventScreen.WBEventScreen(self.WB).interfaceScreen(pPlot)
			elif iIndex == 2:
				WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 3:
				WBTeamScreen.WBTeamScreen(self.WB).interfaceScreen(pPlot.getTeam())
			elif iIndex == 4:
				if pPlot.isCity():
					WBCityEditScreen.WBCityEditScreen(self.WB).interfaceScreen(pPlot.getPlotCity())
			elif iIndex == 5:
				pUnit = pPlot.getUnit(0)
				if pUnit:
					WBUnitScreen.WBUnitScreen(self.WB).interfaceScreen(pUnit)
			elif iIndex == 6:
				WBPlayerUnits.WBPlayerUnits(self.WB).interfaceScreen(iPlayer)
			elif iIndex == 11:
				if iPlayer == -1:
					iPlayer = CyGame().getActivePlayer()
				WBInfoScreen.WBInfoScreen(self.WB).interfaceScreen(iPlayer)

		elif inputClass.getFunctionName() == "NextPlotUpButton":
			pNewPlot = CyMap().plot(pPlot.getX(), pPlot.getY() + 1)
			if not pNewPlot.isNone():
				self.interfaceScreen(pNewPlot)
		elif inputClass.getFunctionName() == "NextPlotDownButton":
			pNewPlot = CyMap().plot(pPlot.getX(), pPlot.getY() - 1)
			if not pNewPlot.isNone():
				self.interfaceScreen(pNewPlot)
		elif inputClass.getFunctionName() == "NextPlotLeftButton":
			pNewPlot = CyMap().plot(pPlot.getX() - 1, pPlot.getY())
			if not pNewPlot.isNone():
				self.interfaceScreen(pNewPlot)
		elif inputClass.getFunctionName() == "NextPlotRightButton":
			pNewPlot = CyMap().plot(pPlot.getX() + 1, pPlot.getY())
			if not pNewPlot.isNone():
				self.interfaceScreen(pNewPlot)

		elif inputClass.getFunctionName().find("BaseYield") > -1:
			i = YieldTypes(inputClass.getData2())
			if inputClass.getData1() == 1030:
				CyGame().setPlotExtraYield(pPlot.getX(), pPlot.getY(), i, iChange)
			elif inputClass.getData1() == 1031:
				iYield = pPlot.getYield(i)
				iImprovement = pPlot.getImprovementType()
				if iImprovement > -1:
					iYield -= pPlot.calculateImprovementYieldChange(iImprovement, i, pPlot.getOwner(), False)
				CyGame().setPlotExtraYield(pPlot.getX(), pPlot.getY(), i, - min(iChange, iYield))
			self.placeStats()

		elif inputClass.getFunctionName() == "RiverWestAButton":
			pWestPlot = CyMap().plot(pPlot.getX() - 1, pPlot.getY())
			pWestPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
			self.placeStats()
		elif inputClass.getFunctionName() == "RiverWestBButton":
			pWestPlot = CyMap().plot(pPlot.getX() - 1, pPlot.getY())
			pWestPlot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			self.placeStats()
		elif inputClass.getFunctionName() == "RiverWestCButton":
			pWestPlot = CyMap().plot(pPlot.getX() - 1, pPlot.getY())
			pWestPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
			self.placeStats()

		elif inputClass.getFunctionName() == "RiverEastAButton":
			pPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
			self.placeStats()
		elif inputClass.getFunctionName() == "RiverEastBButton":
			pPlot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			self.placeStats()
		elif inputClass.getFunctionName() == "RiverEastCButton":
			pPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
			self.placeStats()

		elif inputClass.getFunctionName() == "RiverNorthAButton":
			pNorthPlot = CyMap().plot(pPlot.getX(), pPlot.getY() + 1)
			pNorthPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)
			self.placeStats()
		elif inputClass.getFunctionName() == "RiverNorthBButton":
			pNorthPlot = CyMap().plot(pPlot.getX(), pPlot.getY() + 1)
			pNorthPlot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			self.placeStats()
		elif inputClass.getFunctionName() == "RiverNorthCButton":
			pNorthPlot = CyMap().plot(pPlot.getX(), pPlot.getY() + 1)
			pNorthPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
			self.placeStats()

		elif inputClass.getFunctionName() == "RiverSouthAButton":
			pPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)
			self.placeStats()
		elif inputClass.getFunctionName() == "RiverSouthBButton":
			pPlot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			self.placeStats()
		elif inputClass.getFunctionName() == "RiverSouthCButton":
			pPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
			self.placeStats()

		elif inputClass.getFunctionName() == "WBSigns":
			if inputClass.getData1() == 7876 or inputClass.getData1() == 7872:
				iCulturePlayer = inputClass.getData2() /10000
				self.placeSigns()

		elif inputClass.getFunctionName().find("EditCulture") > -1:
			if inputClass.getData1() == 1030:
				pPlot.changeCulture(iCulturePlayer, iChange, True)
			elif inputClass.getData1() == 1031:
				pPlot.changeCulture(iCulturePlayer, -min(iChange, pPlot.getCulture(iCulturePlayer)), True)
			self.interfaceScreen(pPlot)

		elif inputClass.getFunctionName() == "WBPlotType":
			if iEditType == 0:
				pPlot.setPlotType(PlotTypes(inputClass.getData()), True, True)
			elif iEditType == 1:
				for pLoopPlot in CyMap().plots():
					if pLoopPlot.isNone(): continue
					if pLoopPlot.getArea() == pPlot.getArea():
						pLoopPlot.setPlotType(PlotTypes(inputClass.getData()), True, True)
			else:
				CyMap().setAllPlotTypes(PlotTypes(inputClass.getData()))
			self.interfaceScreen(pPlot)

		elif inputClass.getFunctionName() == "WBPlotTerrain":
			iTerrain = inputClass.getData2()
			if iEditType == 0:
				pPlot.setTerrainType(iTerrain, True, True)
			else:
				for pLoopPlot in CyMap().plots():
					if pLoopPlot.isNone(): continue
					if iEditType == 1:
						if pLoopPlot.getArea() == pPlot.getArea():
							pLoopPlot.setTerrainType(iTerrain, True, True)
					elif iEditType == 2:
						if bSensibility and pLoopPlot.isWater() != pPlot.isWater(): continue
						pLoopPlot.setTerrainType(iTerrain, True, True)
			self.interfaceScreen(pPlot)

		elif inputClass.getFunctionName() == "BonusClass":
			iSelectedClass = screen.getPullDownData("BonusClass", screen.getSelectedPullDownID("BonusClass"))
			self.createBonusList()

		elif inputClass.getFunctionName() == "WBPlotBonus":
			iBonus = inputClass.getData2()
			if iEditType == 0:
				if bAdd:
					pPlot.setBonusType(iBonus)
				else:
					pPlot.setBonusType(-1)
			else:
				for pLoopPlot in CyMap().plots():
					if pLoopPlot.isNone(): continue
					if iEditType == 1 and pLoopPlot.getArea() != pPlot.getArea(): continue
					iOld = pLoopPlot.getBonusType(-1)
					if bAdd:
						pLoopPlot.setBonusType(-1)
						if iBonus > -1 and bSensibility and not pLoopPlot.canHaveBonus(iBonus, False):
							pLoopPlot.setBonusType(iOld)
							continue
						pLoopPlot.setBonusType(iBonus)
					else:
						pLoopPlot.setBonusType(-1)
			self.placeBonus()

		elif inputClass.getFunctionName() == "WBPlotImprovement":
			iImprovement = inputClass.getData2()
			if iEditType == 0:
				if bAdd:
					pPlot.setImprovementType(iImprovement)
				else:
					pPlot.setImprovementType(-1)
			else:
				for pLoopPlot in CyMap().plots():
					if pLoopPlot.isNone(): continue
					if iEditType == 1 and pLoopPlot.getArea() != pPlot.getArea(): continue
					if bAdd:
						if iImprovement > -1 and bSensibility and not pLoopPlot.canHaveImprovement(iImprovement, -1, True): continue
						pLoopPlot.setImprovementType(iImprovement)
					else:
						pLoopPlot.setImprovementType(-1)
			self.placeImprovements()

		elif inputClass.getFunctionName().find("UpgradeTime") > -1:
			if inputClass.getData1() == 1030:
				pPlot.changeImprovementUpgradeProgress(- iChange)
			elif inputClass.getData1() == 1031:
				pPlot.changeImprovementUpgradeProgress(min(iChange, pPlot.getUpgradeTimeLeft(pPlot.getImprovementType(), pPlot.getOwner()) - 1))
			self.placeImprovements()

		elif inputClass.getFunctionName() == "WBPlotFeature":
			iFeature = inputClass.getData2() % 10000
			iVariety = inputClass.getData2() / 10000
			if iVariety < 0 or iFeature < 0:
				iFeature = -1
				iVariety = 0
			if iEditType == 0:
				if bAdd:
					pPlot.setFeatureType(iFeature, iVariety)
				else:
					pPlot.setFeatureType(-1, 0)
			else:
				for pLoopPlot in CyMap().plots():
					if pLoopPlot.isNone(): continue
					if iEditType == 1 and pLoopPlot.getArea() != pPlot.getArea(): continue
					iOldFeature = pLoopPlot.getFeatureType()
					iOldVariety = pLoopPlot.getFeatureVariety()
					if bAdd:
						pLoopPlot.setFeatureType(-1, 0)
						if iFeature > -1 and bSensibility and not pLoopPlot.canHaveFeature(iFeature):
							pLoopPlot.setFeatureType(iOldFeature, iOldVariety)
							continue
						pLoopPlot.setFeatureType(iFeature, iVariety)
					else:
						pLoopPlot.setFeatureType(-1, 0)
			self.placeFeature()

		elif inputClass.getFunctionName() == "WBPlotRoutes":
			iRoute = inputClass.getData2()
			if iEditType == 0:
				if bAdd:
					pPlot.setRouteType(iRoute)
				else:
					pPlot.setRouteType(-1)
			else:
				for pLoopPlot in CyMap().plots():
					if pLoopPlot.isNone(): continue
					if bSensibility:
						if pLoopPlot.isImpassable(): continue
						if pLoopPlot.isWater(): continue
					if iEditType == 1 and pLoopPlot.getArea() != pPlot.getArea(): continue
					if bAdd:
						pLoopPlot.setRouteType(iRoute)
					else:
						if pLoopPlot.getRouteType() == iRoute:
							pLoopPlot.setRouteType(-1)
			self.placeRoutes()

		elif inputClass.getFunctionName() == "PlotEditScriptData":
			popup = Popup.PyPopup(5555, EventContextTypes.EVENTCONTEXT_ALL)
			popup.setHeaderString(CyTranslator().getText("TXT_KEY_WB_SCRIPT", ()))
			popup.setUserData((pPlot.getX(), pPlot.getY()))
			popup.createEditBox(pPlot.getScriptData())
			popup.launch()

		elif inputClass.getFunctionName() == "EditLandMark":
			iIndex = -1
			sText = ""
			for i in xrange(CyEngine().getNumSigns()):
				pSign = CyEngine().getSignByIndex(i)
				if pSign.getPlot().getX() != pPlot.getX(): continue
				if pSign.getPlot().getY() != pPlot.getY(): continue
				if pSign.getPlayerType() == iCulturePlayer:
					iIndex = i
					sText = pSign.getCaption()
					break

			popup = Popup.PyPopup(5009, EventContextTypes.EVENTCONTEXT_ALL)
			popup.setHeaderString(CyTranslator().getText("TXT_KEY_WB_LANDMARKS", ()))
			popup.setUserData((pPlot.getX(), pPlot.getY(), iCulturePlayer, iIndex))
			popup.createEditBox(sText)
			popup.launch()

		elif inputClass.getFunctionName() == "SensibilityCheck":
			bSensibility = not bSensibility
			screen.setState("SensibilityCheck", bSensibility)
		return 1

	def update(self, fDelta):
		self.placeMap()
		global iCounter
		if iCounter > 0:
			iCounter -= 1
		elif iCounter == 0:
			self.placeSigns()
			iCounter = -1
		return 1