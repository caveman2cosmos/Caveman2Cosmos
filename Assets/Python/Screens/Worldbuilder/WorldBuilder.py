from CvPythonExtensions import *
import HandleInputUtil
import WBPlotScreen
import WBEventScreen
import WBCityEditScreen
import WBCityDataScreen
import WBBuildingScreen
import WBUnitScreen
import WBPromotionScreen
import WBReligionScreen
import WBCorporationScreen
import WBDiplomacyScreen
import WBPlayerScreen
import WBTeamScreen
import WBProjectScreen
import WBPlayerUnits
import WBInfoScreen
import WBTradeScreen

GC = CyGlobalContext()
MAP = GC.getMap()
GAME = GC.getGame()
bPython = True
Activities = ["AWAKE", "HOLD", "SLEEP", "HEAL", "SENTRY", "INTERCEPT", "MISSION", "PATROL", "PLUNDER"]

class WorldBuilder:

	def __init__(self, screenId):
		self.screenId = screenId
		import CvEventInterface
		self.eventManager = CvEventInterface.getEventManager()
		self.bNotWB = True


	def interfaceScreen(self):
		getWBToolAdvancedStartTabCtrl().enable(False)

		self.TempInfo = []
		self.m_bShowBigBrush = False
		self.m_iCurrentTeam = 0
		self.m_iCurrentX = -1
		self.m_iCurrentY = -1
		self.m_pCurrentPlot = 0
		self.m_pRiverStartPlot = -1

		self.PlayerMode = ["Ownership", "Units", "Buildings", "City", "StartingPlot"]
		self.MapMode = ["AddLandMark", "PlotData", "River", "Improvements", "Bonus", "PlotType", "Terrain", "Routes", "Features"]
		self.RevealMode = ["RevealPlot", "INVISIBLE_SUBMARINE", "INVISIBLE_STEALTH", "Blockade"]

		self.bSensibility = True
		self.lMoveUnit = []
		self.iMoveCity = -1
		self.iTargetPlotX = -1
		self.iTargetPlotY = -1

		# Initialize WB
		if self.bNotWB:
			import InputData
			self.InputData = InputData.instance

			import PythonToolTip
			self.tooltip = PythonToolTip.PythonToolTip()

			# init sub-screens
			self.inSubScreen = None
			import WBTechScreen
			import WBGameDataScreen
			self.subScreens = {
				"TechScreen" : WBTechScreen.WBTechScreen(self),
				"GameScreen" : WBGameDataScreen.WBGameDataScreen(self)
			}
			self.iPlayerAddMode = "Units"
			self.iSelection = -1
			self.iSelectClass = -2
			self.iBrushWidth = 1
			self.iBrushHeight = 1
			self.iChange = 1
			self.iCurrentPlayer = GAME.getActivePlayer()
			self.bNotWB = False
			# Init screen
			import ScreenResolution as SR
			self.xRes = SR.x
			self.yRes = SR.y
			self.aFontList = SR.aFontList
			self.iFontScale = SR.iFontScale
			screen = self.getScreen()
			screen.setCloseOnEscape(False)
			screen.setAlwaysShown(True)
			# Make base menu
			iButtonWidth = 32
			iAdjust = iButtonWidth + 3

			iScreenWidth = 16 + iAdjust * 6
			iScreenHeight = 16 + iAdjust * 4

			iXStart = self.xRes - iScreenWidth

			screen.addPanel("WorldBuilderBackgroundPanel", "", "", True, True, iXStart, 0, iScreenWidth, iScreenHeight, PanelStyles.PANEL_STYLE_MAIN)

			iX = iXStart + 8
			iY = 10

			screen.setImageButton("Version", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_QUESTIONMARK").getPath(), iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 9)
			iX += iAdjust
			screen.setImageButton("WorldBuilderRegenerateMap", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_REVEAL_ALL_TILES").getPath(), iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_REGENERATE_MAP, -1, -1)
			iX += iAdjust
			screen.addCheckBoxGFC("WorldBuilderEraseButton", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_ERASE").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_ERASE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.setImageButton("WorldBuilderSaveButton", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_SAVE").getPath(), iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_SAVE_BUTTON, -1, -1)
			iX += iAdjust
			screen.setImageButton("WorldBuilderLoadButton", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_LOAD").getPath(), iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_LOAD_BUTTON, -1, -1)
			iX += iAdjust
			screen.setImageButton("WorldBuilderExitButton", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_EXIT").getPath(), iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_EXIT_BUTTON, -1, -1)

			iX = iXStart + 8
			iY += iAdjust

			screen.setImageButton("EditGameOptions", "", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 23)
			screen.setStyle("EditGameOptions", "Button_HUDAdvisorVictory_Style")
			iX += iAdjust
			screen.setImageButton("EditReligions", "", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 20)
			screen.setStyle("EditReligions", "Button_HUDAdvisorReligious_Style")
			iX += iAdjust
			screen.setImageButton("EditCorporations", "", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 21)
			screen.setStyle("EditCorporations", "Button_HUDAdvisorCorporation_Style")
			iX += iAdjust
			screen.setImageButton("EditDiplomacy", "", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_DIPLOMACY_MODE_BUTTON, -1, -1)
			screen.setStyle("EditDiplomacy", "Button_HUDAdvisorForeign_Style")
			iX += iAdjust
			screen.setImageButton("EditEspionage", "", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 22)
			screen.setStyle("EditEspionage", "Button_HUDAdvisorEspionage_Style")
			iX += iAdjust
			screen.setImageButton("TradeScreen", "", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 34)
			screen.setStyle("TradeScreen", "Button_HUDAdvisorFinance_Style")

			iX = iXStart + 8
			iY += iAdjust
			screen.addCheckBoxGFC("WorldBuilderNormalPlayerModeButton", ",Art/Interface/Buttons/Buildings/SDI.dds,Art/Interface/Buttons/FinalFrontier1_Atlas.dds,2,15", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_NORMAL_PLAYER_TAB_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("WorldBuilderNormalMapModeButton", ",Art/Interface/Buttons/Buildings/SDI.dds,Art/Interface/Buttons/FinalFrontier2_Atlas.dds,3,6", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_NORMAL_MAP_TAB_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("WorldBuilderRevealTileModeButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_LOS").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_REVEAL_TAB_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("PythonEffectButton", ",Art/Interface/Buttons/Units/Warrior.dds,Art/Interface/Buttons/FinalFrontier2_Atlas.dds,3,4", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 0, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.setImageButton("InfoScreen", "", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 33)
			screen.setStyle("InfoScreen", "Button_HUDAdvisorRecord_Style")

			iX = iXStart + 8
			iY += iAdjust

			screen.addCheckBoxGFC("EditUnitData", ",Art/Interface/Buttons/Units/Warrior.dds,Art/Interface/Buttons/Afterworld_Atlas.dds,4,9", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_UNIT_EDIT_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("EditPromotions", ",Art/Interface/Buttons/Promotions/Combat1.dds,Art/Interface/Buttons/Promotions_Atlas.dds,8,2", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 6, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("EditCityDataI", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_TOGGLE_CITY_EDIT_MODE").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth,WidgetTypes.WIDGET_WB_CITY_EDIT_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("EditCityDataII", ",Art/Interface/Buttons/Buildings/SDI.dds,Art/Interface/Buttons/FinalFrontier2_Atlas.dds,1,8", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 7, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("EditCityBuildings", ",Art/Interface/Buttons/Buildings/SDI.dds,Art/Interface/Buttons/FinalFrontier1_Atlas.dds,5,14", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 8, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("EditEvents", "", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 10, ButtonStyles.BUTTON_STYLE_LABEL)
			screen.setStyle("EditEvents", "Button_HUDLog_Style")

			self.setCurrentModeCheckbox()
			self.refreshSideMenu()

			screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, True)
			screen.setForcedRedraw(True)


	def mouseOverPlot(self):
		screen = self.getScreen()
		self.m_pCurrentPlot = CyInterface().getMouseOverPlot()
		self.m_iCurrentX = self.m_pCurrentPlot.getX()
		self.m_iCurrentY = self.m_pCurrentPlot.getY()

		sText = "<font=3b>%s, %s, X: %d, Y: %d" %(GC.getMapInfo(GAME.getCurrentMap()).getDescription(), CyTranslator().getText("TXT_KEY_WB_LATITUDE", (self.m_pCurrentPlot.getLatitude(),)), self.m_iCurrentX, self.m_iCurrentY)
		screen.setLabel("WBCoords", "Background", sText, 1<<2, self.xRes/2, 6, -0.3, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		if self.iPlayerAddMode in self.RevealMode:
			if CyInterface().isLeftMouseDown():
				self.setMultipleReveal(True)
			elif CyInterface().isRightMouseDown():
				self.setMultipleReveal(False)

		elif CyInterface().isLeftMouseDown():
			if self.useLargeBrush():
				self.placeMultipleObjects()
			else:
				self.placeObject()
		elif CyInterface().isRightMouseDown():
			if self.useLargeBrush():
				self.removeMultipleObjects()
			else:
				self.removeObject()

	def getHighlightPlot(self, argsList):
		if self.m_pCurrentPlot != 0 and isMouseOverGameSurface():
			if not self.useLargeBrush():
				return (self.m_iCurrentX, self.m_iCurrentY)
			else:
				CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
				data = self.getMultiplePlotData()
				for x in range(data[0], data[1]):
					for y in range(data[2], data[3]):
						plot = MAP.plot(x, y)
						if plot.isNone(): continue
						CyEngine().fillAreaBorderPlotAlt(plot.getX(), plot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)

	# Will update the screen (every 250 MS)
	def updateScreen(self):
		if self.iPlayerAddMode == "River" and self.m_pRiverStartPlot != -1:
			self.setRiverHighlights()

	def refreshReveal(self):
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		for pPlot in MAP.plots():
			if pPlot.isNone(): continue
			self.showRevealed(pPlot)

	def refreshStartingPlots(self):
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			pPlot = GC.getPlayer(iPlayerX).getStartingPlot()
			if not pPlot.isNone():
				sColor = "COLOR_MAGENTA"
				if iPlayerX == self.iCurrentPlayer:
					sColor = "COLOR_BLACK"
				CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS, sColor, 1.0)


	def placeObject(self):
		if self.m_iCurrentX == -1 or self.m_iCurrentY == -1: return

		if self.iPlayerAddMode == "EraseAll":
			self.m_pCurrentPlot.erase()
			CyEngine().removeLandmark(self.m_pCurrentPlot)
			for iPlayerX in xrange(GC.getMAX_PLAYERS()):
				CyEngine().removeSign(self.m_pCurrentPlot, iPlayerX)

		elif self.iPlayerAddMode == "AddLandMark":
			iIndex = -1
			for i in xrange(CyEngine().getNumSigns()):
				pSign = CyEngine().getSignByIndex(i)
				if pSign.getPlot().getX() != self.m_pCurrentPlot.getX(): continue
				if pSign.getPlot().getY() != self.m_pCurrentPlot.getY(): continue
				if pSign.getPlayerType() == self.iCurrentPlayer:
					iIndex = i
					break
			sText = ""
			if iIndex > -1:
				sText = CyEngine().getSignByIndex(iIndex).getCaption()
			popup = CyPopup(5009, EventContextTypes.EVENTCONTEXT_ALL, True)
			popup.setHeaderString(CyTranslator().getText("TXT_KEY_WB_LANDMARK_START", ()), 1<<2)
			popup.setUserData((self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY(), self.iCurrentPlayer, iIndex))
			popup.createEditBox(sText, 0)
			popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

		elif self.iSelection == -1: return

		elif self.iPlayerAddMode == "Ownership":
			self.m_pCurrentPlot.setOwner(self.iCurrentPlayer)
	## Python Effects ##
		elif self.iPlayerAddMode == "Units":
			for i in xrange(self.iChange):
				GC.getPlayer(self.iCurrentPlayer).initUnit(self.iSelection, self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
		elif self.iPlayerAddMode == "Buildings":
			if self.m_pCurrentPlot.isCity():
				pCity = self.m_pCurrentPlot.getPlotCity()
				bEffects = False
				if bPython and pCity.getNumRealBuilding(self.iSelection) == 0:
					bEffects = True
				pCity.setNumRealBuilding(self.iSelection, 1)
				if bEffects:
					self.eventManager.onBuildingBuilt([pCity, self.iSelection])
		elif self.iPlayerAddMode == "City":
			if self.m_pCurrentPlot.isCity(): return
			pCity = GC.getPlayer(self.iCurrentPlayer).initCity(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY())
			if bPython:
				self.eventManager.onCityBuilt([pCity, None])
	## Python Effects ##
		elif self.iPlayerAddMode == "Improvements":
			self.m_pCurrentPlot.setImprovementType(self.iSelection)
		elif self.iPlayerAddMode == "Bonus":
			self.m_pCurrentPlot.setBonusType(self.iSelection)
		elif self.iPlayerAddMode == "Routes":
			self.m_pCurrentPlot.setRouteType(self.iSelection)
		elif self.iPlayerAddMode == "Terrain":
			self.m_pCurrentPlot.setTerrainType(self.iSelection, True, True)
		elif self.iPlayerAddMode == "PlotType":
			if self.iSelection == GC.getInfoTypeForString("TERRAIN_PEAK"):
				self.m_pCurrentPlot.setPlotType(PlotTypes.PLOT_PEAK, True, True)
			elif self.iSelection == GC.getInfoTypeForString("TERRAIN_HILL"):
				self.m_pCurrentPlot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
			elif self.iSelection == GC.getInfoTypeForString("TERRAIN_GRASSLAND"):
				self.m_pCurrentPlot.setPlotType(PlotTypes.PLOT_LAND, True, True)
			elif self.iSelection == GC.getInfoTypeForString("TERRAIN_OCEAN"):
				self.m_pCurrentPlot.setPlotType(PlotTypes.PLOT_OCEAN, True, True)
		elif self.iPlayerAddMode == "Features":
			self.m_pCurrentPlot.setFeatureType(self.iSelection, self.iSelectClass)
		elif self.iPlayerAddMode == "River":
			if self.m_pRiverStartPlot == self.m_pCurrentPlot:
				self.m_pRiverStartPlot = -1
				CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
			elif self.m_pRiverStartPlot != -1:
				iXDiff = abs(self.m_pCurrentPlot.getX() - self.m_pRiverStartPlot.getX())
				iYDiff = abs(self.m_pCurrentPlot.getY() - self.m_pRiverStartPlot.getY())

				if ((iXDiff == iYDiff) and (iXDiff == 1) and (self.m_pRiverStartPlot.getX() > self.m_pCurrentPlot.getX()) and (self.m_pRiverStartPlot.getY() < self.m_pCurrentPlot.getY())):
					self.placeRiverNW(True)
					self.m_pRiverStartPlot = MAP.plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()+1)
				elif ((iXDiff == 0) and (iYDiff == 1) and (self.m_pRiverStartPlot.getY() < self.m_pCurrentPlot.getY())):
					self.placeRiverN(True)
					self.m_pRiverStartPlot = self.m_pCurrentPlot
				elif ((iXDiff == iYDiff) and (iXDiff == 1) and (self.m_pRiverStartPlot.getX() < self.m_pCurrentPlot.getX()) and (self.m_pRiverStartPlot.getY() < self.m_pCurrentPlot.getY())):
					self.placeRiverNE(True)
					self.m_pRiverStartPlot = self.m_pCurrentPlot
				elif ((iXDiff == 1) and (iYDiff == 0) and (self.m_pRiverStartPlot.getX() > self.m_pCurrentPlot.getX())):
					self.placeRiverW(True)
					self.m_pRiverStartPlot = self.m_pCurrentPlot
				elif ((iXDiff == 1) and (iYDiff == 0) and (self.m_pRiverStartPlot.getX() < self.m_pCurrentPlot.getX())):
					self.placeRiverE(True)
					self.m_pRiverStartPlot = self.m_pCurrentPlot
				elif ((iXDiff == iYDiff) and (iXDiff == 1) and (self.m_pRiverStartPlot.getX() > self.m_pCurrentPlot.getX()) and (self.m_pRiverStartPlot.getY() > self.m_pCurrentPlot.getY())):
					self.placeRiverSW(True)
					self.m_pRiverStartPlot = self.m_pCurrentPlot
				elif ((iXDiff == 0) and (iYDiff == 1) and (self.m_pRiverStartPlot.getY() > self.m_pCurrentPlot.getY())):
					self.placeRiverS(True)
					self.m_pRiverStartPlot = self.m_pCurrentPlot
				elif ((iXDiff == iYDiff) and (iXDiff == 1) and (self.m_pRiverStartPlot.getX() < self.m_pCurrentPlot.getX()) and (self.m_pRiverStartPlot.getY() > self.m_pCurrentPlot.getY())):
					self.placeRiverSE(True)
					self.m_pRiverStartPlot = self.m_pCurrentPlot
				else:
					self.m_pRiverStartPlot = self.m_pCurrentPlot
			else:
				self.m_pRiverStartPlot = self.m_pCurrentPlot
		return 1

	def removeObject(self):
		if self.m_iCurrentX == -1 or self.m_iCurrentY == -1: return

		if self.iPlayerAddMode == "EraseAll":
			self.m_pCurrentPlot.erase()
			CyEngine().removeLandmark(self.m_pCurrentPlot)
			for iPlayerX in xrange(GC.getMAX_PLAYERS()):
				CyEngine().removeSign(self.m_pCurrentPlot, iPlayerX)
		elif self.iPlayerAddMode == "Ownership":
			self.m_pCurrentPlot.setOwner(-1)
		elif self.iPlayerAddMode == "Units":
			for pUnit in self.m_pCurrentPlot.units():
				if pUnit.getUnitType() == self.iSelection:
					pUnit.kill(False, PlayerTypes.NO_PLAYER)
					return 1
			if self.m_pCurrentPlot.getNumUnits() > 0:
				pUnit = self.m_pCurrentPlot.getUnit(0)
				pUnit.kill(False, PlayerTypes.NO_PLAYER)
				return 1
		elif self.iPlayerAddMode == "Buildings":
			if self.m_pCurrentPlot.isCity():
				self.m_pCurrentPlot.getPlotCity().setNumRealBuilding(self.iSelection, 0)
		elif self.iPlayerAddMode == "City":
			if self.m_pCurrentPlot.isCity():
				pCity = self.m_pCurrentPlot.getPlotCity()
				pCity.kill()
		elif self.iPlayerAddMode == "Improvements":
			self.m_pCurrentPlot.setImprovementType(-1)
			return 1
		elif self.iPlayerAddMode == "Bonus":
			self.m_pCurrentPlot.setBonusType(-1)
			return 1
		elif self.iPlayerAddMode == "Features":
			self.m_pCurrentPlot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
		elif self.iPlayerAddMode == "Routes":
			self.m_pCurrentPlot.setRouteType(-1)
		elif self.iPlayerAddMode == "River":
			if self.m_pRiverStartPlot != -1:
				self.m_pRiverStartPlot = -1
				CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
			else:
				self.m_pCurrentPlot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
				self.m_pCurrentPlot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
		elif self.iPlayerAddMode == "AddLandMark":
			CyEngine().removeSign(self.m_pCurrentPlot, self.iCurrentPlayer)
		return 1


	def placeRiverNW(self, bUseCurrent):
		if bUseCurrent:
			pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if not pRiverStepPlot.isNone():
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)

		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY())
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()+1)
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)

	def placeRiverN(self, bUseCurrent):
		if bUseCurrent:
			pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if not pRiverStepPlot.isNone():
				pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)

		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY()+1)
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)

	def placeRiverNE(self, bUseCurrent):
		if bUseCurrent:
			pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if not pRiverStepPlot.isNone():
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)

		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY())
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY()+1)
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)

	def placeRiverW(self, bUseCurrent):
		if bUseCurrent:
			pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if not pRiverStepPlot.isNone():
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)

		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY())
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)

	def placeRiverE(self, bUseCurrent):
		if bUseCurrent:
			pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if not pRiverStepPlot.isNone():
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)

		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY())
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)

	def placeRiverSW(self, bUseCurrent):
		if bUseCurrent:
			pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if not pRiverStepPlot.isNone():
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)

		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()-1)
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)

	def placeRiverS(self, bUseCurrent):
		if bUseCurrent:
			pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if not pRiverStepPlot.isNone():
				pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)

		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY()-1)
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)

	def placeRiverSE(self, bUseCurrent):
		if bUseCurrent:
			pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if not pRiverStepPlot.isNone():
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)

		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY())
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
		pRiverStepPlot = MAP.plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY()-1)
		if not pRiverStepPlot.isNone():
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)


	def toggleUnitEditCB(self):
		self.iPlayerAddMode = "EditUnit"
		self.refreshSideMenu()


	def normalPlayerTabModeCB(self):
		self.m_iCurrentTeam = GC.getPlayer(self.iCurrentPlayer).getTeam()
		self.iPlayerAddMode = "Units"
		self.iSelectClass = -2
		self.iSelection = -1
		self.refreshSideMenu()
		getWBToolNormalPlayerTabCtrl().enable(False)
		getWBToolNormalMapTabCtrl().enable(False)


	def normalMapTabModeCB(self):
		self.iPlayerAddMode = "PlotData"
		self.refreshSideMenu()


	def revealTabModeCB(self):
		self.iPlayerAddMode = "RevealPlot"
		self.refreshSideMenu()
		self.refreshReveal()


	def toggleCityEditCB(self):
		self.iPlayerAddMode = "CityDataI"
		self.refreshSideMenu()


	def landmarkModeCB(self):
		self.iPlayerAddMode = "AddLandMark"
		self.iCurrentPlayer = GC.getBARBARIAN_PLAYER()
		self.refreshSideMenu()


	def eraseCB(self):
		self.m_pRiverStartPlot = -1
		self.iPlayerAddMode = "EraseAll"
		self.refreshSideMenu()

	def placeMultipleObjects(self):
		permCurrentPlot = self.m_pCurrentPlot
		Data = self.getMultiplePlotData()
		for x in range(Data[0], Data[1]):
			for y in range(Data[2], Data[3]):
				self.m_pCurrentPlot = MAP.plot(x, y)
				if self.m_pCurrentPlot.isNone(): continue
				if self.bSensibility:
					if self.iPlayerAddMode == "Improvements":
						if self.m_pCurrentPlot.canHaveImprovement(self.iSelection, -1, True):
							self.placeObject()
					elif self.iPlayerAddMode == "Bonus":
						iOldBonus = self.m_pCurrentPlot.getBonusType(-1)
						self.m_pCurrentPlot.setBonusType(-1)
						if self.m_pCurrentPlot.canHaveBonus(self.iSelection, False):
							self.placeObject()
						else:
							self.m_pCurrentPlot.setBonusType(iOldBonus)
					elif self.iPlayerAddMode == "Features":
						iOldFeature = self.m_pCurrentPlot.getFeatureType()
						iOldVariety = self.m_pCurrentPlot.getFeatureVariety()
						self.m_pCurrentPlot.setFeatureType(-1, 0)
						if self.m_pCurrentPlot.canHaveFeature(self.iSelection):
							self.placeObject()
						else:
							self.m_pCurrentPlot.setFeatureType(iOldFeature, iOldVariety)
					elif self.iPlayerAddMode == "Routes":
						if not self.m_pCurrentPlot.isImpassable() or self.m_pCurrentPlot.isWater():
							self.placeObject()
					elif self.iPlayerAddMode == "Terrain":
						if self.m_pCurrentPlot.isWater() == GC.getTerrainInfo(self.iSelection).isWaterTerrain():
							self.placeObject()
					else:
						self.placeObject()
				else:
					self.placeObject()
		self.m_pCurrentPlot = permCurrentPlot

	def removeMultipleObjects(self):
		permCurrentPlot = self.m_pCurrentPlot
		Data = self.getMultiplePlotData()
		for x in range(Data[0], Data[1]):
			for y in range(Data[2], Data[3]):
				self.m_pCurrentPlot = MAP.plot(x,y)
				if self.m_pCurrentPlot.isNone(): continue
				self.removeObject()
		self.m_pCurrentPlot = permCurrentPlot

	def getMultiplePlotData(self):
		iMinX = self.m_pCurrentPlot.getX()
		iMaxX = self.m_pCurrentPlot.getX() + self.iBrushWidth
		iMinY = self.m_pCurrentPlot.getY() - self.iBrushHeight + 1
		iMaxY = self.m_pCurrentPlot.getY() + 1
		if self.iBrushWidth == -1:
			iMinX = 0
			iMaxX = MAP.getGridWidth()
		if self.iBrushHeight == -1:
			iMinY = 0
			iMaxY = MAP.getGridHeight()
		if not MAP.isWrapX():
			iMaxX = min(iMaxX, MAP.getGridWidth())
		if not MAP.isWrapY():
			iMinY = max(iMinY, 0)
		return [iMinX, iMaxX, iMinY, iMaxY]

	def setMultipleReveal(self, bReveal):
		Data = self.getMultiplePlotData()
		for x in range(Data[0], Data[1]):
			for y in range(Data[2], Data[3]):
				pPlot = MAP.plot(x,y)
				if pPlot.isNone(): continue
				self.RevealCurrentPlot(bReveal, pPlot)
		self.refreshReveal()

	def useLargeBrush(self):
		if self.iPlayerAddMode in self.RevealMode:
			return True
		if self.iPlayerAddMode == "EraseAll":
			return True
		if self.iPlayerAddMode == "Improvements":
			return True
		if self.iPlayerAddMode == "Bonus":
			return True
		if self.iPlayerAddMode == "PlotType":
			return True
		if self.iPlayerAddMode == "Terrain":
			return True
		if self.iPlayerAddMode == "Routes":
			return True
		if self.iPlayerAddMode == "Features":
			return True
		return False


	def refreshSideMenu(self):
		screen = self.getScreen()
		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		iButtonWidth = 32
		iAdjust = iButtonWidth + 3
		iScreenWidth = 16 + iAdjust * 6
		iScreenHeight = 16 + iAdjust * 4

		screen.deleteWidget("WorldBuilderPlayerChoice")
		screen.deleteWidget("WorldBuilderLandmarkButton")
		screen.deleteWidget("WorldBuilderRevealAll")
		screen.deleteWidget("WorldBuilderUnrevealAll")
		screen.deleteWidget("WorldBuilderRevealPanel")
		screen.deleteWidget("WorldBuilderBackgroundBottomPanel")
		screen.deleteWidget("EditPlayerData")
		screen.deleteWidget("EditTeamData")
		screen.deleteWidget("EditTechnologies")
		screen.deleteWidget("EditProjects")
		screen.deleteWidget("EditUnitsCities")
		screen.deleteWidget("ChangeBy")
		screen.deleteWidget("AddOwnershipButton")
		screen.deleteWidget("AddUnitsButton")
		screen.deleteWidget("AddBuildingsButton")
		screen.deleteWidget("AddCityButton")
		screen.deleteWidget("EditStartingPlot")
		screen.deleteWidget("EditPlotData")
		screen.deleteWidget("AddImprovementButton")
		screen.deleteWidget("AddBonusButton")
		screen.deleteWidget("AddPlotTypeButton")
		screen.deleteWidget("AddTerrainButton")
		screen.deleteWidget("AddRouteButton")
		screen.deleteWidget("AddFeatureButton")
		screen.deleteWidget("AddRiverButton")
		screen.deleteWidget("WBCurrentItem")
		screen.deleteWidget("WBSelectClass")
		screen.deleteWidget("WBSelectItem")
		screen.deleteWidget("RevealMode")
		screen.deleteWidget("WorldBuilderEraseAll")
		screen.deleteWidget("BrushWidth")
		screen.deleteWidget("BrushHeight")
		screen.deleteWidget("SensibilityCheck")
		# Panel Screen
		nRows = 1
		if self.iPlayerAddMode in self.PlayerMode or self.iPlayerAddMode in self.RevealMode or self.iPlayerAddMode in self.MapMode:
			nRows = 3
		iHeight = 16 + iAdjust * nRows
		iXStart = self.xRes - iScreenWidth
		screen.addPanel("WorldBuilderBackgroundBottomPanel", "", "", True, True, iXStart, iScreenHeight - 10, iScreenWidth, iHeight, PanelStyles.PANEL_STYLE_MAIN )
		iY = iScreenHeight
		if self.iPlayerAddMode in self.PlayerMode:
			iX = iXStart + 8
			screen.addCheckBoxGFC("AddOwnershipButton", GC.getCivilizationInfo(GC.getPlayer(self.iCurrentPlayer).getCivilizationType()).getButton(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 28, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addDropDownBoxGFC("WorldBuilderPlayerChoice", iX, iY, self.xRes - 8 - iX, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for iPlayer in xrange(GC.getMAX_PLAYERS()):
				if GC.getPlayer(iPlayer).isEverAlive():
					sName = GC.getPlayer(iPlayer).getName()
					if not GC.getPlayer(iPlayer).isAlive():
						sName = "*" + sName
					if GC.getPlayer(iPlayer).isTurnActive():
						sName = "[" + sName + "]"
					screen.addPullDownString("WorldBuilderPlayerChoice", sName, iPlayer, iPlayer, self.iCurrentPlayer == iPlayer)
			iX = self.xRes - iScreenWidth + 8
			iY += iAdjust
			screen.setImageButton("EditPlayerData", GC.getLeaderHeadInfo(GC.getPlayer(self.iCurrentPlayer).getLeaderType()).getButton(), iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 1)
			iX += iAdjust
			screen.setImageButton("EditTeamData", ",Art/Interface/Buttons/Buildings/SDI.dds,Art/Interface/Buttons/FinalFrontier2_Atlas.dds,8,7", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 2)
			iX += iAdjust
			screen.setImageButton("EditTechnologies", ",Art/Interface/Buttons/TechTree/Physics.dds,Art/Interface/Buttons/TechTree_Atlas.dds,5,6", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 3)
			iX += iAdjust
			screen.setImageButton("EditProjects", ",Art/Interface/Buttons/Buildings/SDI.dds,Art/Interface/Buttons/Buildings_Atlas.dds,1,6", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 4)
			iX += iAdjust
			screen.setImageButton("EditUnitsCities", ",Art/Interface/Buttons/Buildings/SDI.dds,Art/Interface/Buttons/Warlords_Atlas_1.dds,3,12", iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 5)
			iX += iAdjust
			screen.addCheckBoxGFC("EditStartingPlot", ",Art/Interface/Buttons/Units/Warrior.dds,Art/Interface/Buttons/Warlords_Atlas_1.dds,4,13", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 32, ButtonStyles.BUTTON_STYLE_LABEL)

			iY += iAdjust
			iX = iXStart + 8
			screen.addCheckBoxGFC("AddUnitsButton", ",Art/Interface/Buttons/Units/Warrior.dds,Art/Interface/Buttons/Warlords_Atlas_1.dds,6,10", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 27, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("AddBuildingsButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 19, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("AddCityButton", ",Art/Interface/Buttons/Actions/FoundCity.dds,Art/Interface/Buttons/Charlemagne_Atlas.dds,4,2", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 18, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addDropDownBoxGFC("ChangeBy", iX, iY, self.xRes - 8 - iX, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			i = 1
			while i < 1001:
				screen.addPullDownString("ChangeBy", str(i), i, i, self.iChange == i)
				if str(i)[0] == "1":
					i *= 5
				else:
					i *= 2

		elif self.iPlayerAddMode in self.MapMode:
			iX = iXStart + 8
			screen.addCheckBoxGFC("EditPlotData", ",Art/Interface/Buttons/WorldBuilder/Gems.dds,Art/Interface/Buttons/FinalFrontier1_Atlas.dds,4,16", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1027, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("AddRiverButton", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_RIVER_PLACEMENT").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 11, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("WorldBuilderLandmarkButton", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_LANDMARK_MODE").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_LANDMARK_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addDropDownBoxGFC("WorldBuilderPlayerChoice", iX, iY, self.xRes - 8 - iX, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			screen.addPullDownString("WorldBuilderPlayerChoice", CyTranslator().getText("TXT_KEY_WB_LANDMARKS", ()), GC.getBARBARIAN_PLAYER(), GC.getBARBARIAN_PLAYER(), self.iCurrentPlayer == GC.getBARBARIAN_PLAYER())
			for iPlayer in xrange(GC.getMAX_PLAYERS()):
				if iPlayer == GC.getBARBARIAN_PLAYER(): continue
				if GC.getPlayer(iPlayer).isEverAlive():
					sName = GC.getPlayer(iPlayer).getName()
					if not GC.getPlayer(iPlayer).isAlive():
						sName = "*" + sName
					if GC.getPlayer(iPlayer).isTurnActive():
						sName = "[" + sName + "]"
					screen.addPullDownString("WorldBuilderPlayerChoice", sName, iPlayer, iPlayer, self.iCurrentPlayer == iPlayer)

			iX = iXStart + 8
			iY += iAdjust
			screen.addCheckBoxGFC("AddImprovementButton", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_FEATURE_PRODUCTION").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 12, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("AddBonusButton", ",Art/Interface/Buttons/WorldBuilder/Gems.dds,Art/Interface/Buttons/Warlords_Atlas_1.dds,7,9", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 13, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("AddPlotTypeButton", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_CHANGE_ALL_PLOTS").getPath(), CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 14, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("AddTerrainButton", ",Art/Interface/Buttons/BaseTerrain/Grassland.dds,Art/Interface/Buttons/BaseTerrain_TerrainFeatures_Atlas.dds,3,1", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 15, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("AddRouteButton", "Art/Interface/Buttons/Builds/BuildRoad.dds", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 16, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			screen.addCheckBoxGFC("AddFeatureButton", ",Art/Interface/Buttons/TerrainFeatures/Forest.dds,Art/Interface/Buttons/BaseTerrain_TerrainFeatures_Atlas.dds,3,3", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 17, ButtonStyles.BUTTON_STYLE_LABEL)

			iX = iXStart + 8
			iY += iAdjust
			screen.addCheckBoxGFC("SensibilityCheck", ",Art/Interface/Buttons/WorldBuilder/Gems.dds,Art/Interface/Buttons/FinalFrontier1_Atlas.dds,1,16", CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				 iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 24, ButtonStyles.BUTTON_STYLE_LABEL)
			iX += iAdjust
			iWidth = (self.xRes - 8 - iX - 3)/2
			screen.addDropDownBoxGFC("BrushWidth", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for i in range(1, 11):
				screen.addPullDownString("BrushWidth", "W: " + str(i), i, i, self.iBrushWidth == i)
			screen.addPullDownString("BrushWidth", "W: " + "--", -1, -1, self.iBrushWidth == -1)
			iX += iWidth
			screen.addDropDownBoxGFC("BrushHeight", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for i in range(1, 11):
				screen.addPullDownString("BrushHeight", "H: " + str(i), i, i, self.iBrushHeight == i)
			screen.addPullDownString("BrushHeight", "H: " + "--", -1, -1, self.iBrushHeight == -1)

		elif self.iPlayerAddMode in self.RevealMode:
			iX = iXStart + 8
			screen.addDropDownBoxGFC("RevealMode", iX, iY, self.xRes - 8 - iX, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			screen.addPullDownString("RevealMode", CyTranslator().getText("TXT_KEY_REVEAL_PLOT",()), 0, 0, self.iPlayerAddMode == self.RevealMode[0])
			screen.addPullDownString("RevealMode", CyTranslator().getText("TXT_KEY_REVEAL_SUBMARINE",()), 1, 1, self.iPlayerAddMode == self.RevealMode[1])
			screen.addPullDownString("RevealMode", CyTranslator().getText("TXT_KEY_REVEAL_STEALTH",()), 2, 2, self.iPlayerAddMode == self.RevealMode[2])
			screen.addPullDownString("RevealMode", GC.getMissionInfo(GC.getInfoTypeForString("MISSION_PLUNDER")).getDescription(), 3, 3, self.iPlayerAddMode == self.RevealMode[3])

			iY += iAdjust
			screen.setImageButton("WorldBuilderRevealAll", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_REVEAL_ALL_TILES").getPath(), iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_REVEAL_ALL_BUTTON, -1, -1)
			iX += iAdjust
			screen.addDropDownBoxGFC("WorldBuilderPlayerChoice", iX, iY, self.xRes - 8 - iX, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for iPlayer in xrange(GC.getMAX_PLAYERS()):
				if GC.getPlayer(iPlayer).isEverAlive():
					sName = GC.getPlayer(iPlayer).getName()
					if not GC.getPlayer(iPlayer).isAlive():
						sName = "*" + sName
					if GC.getPlayer(iPlayer).isTurnActive():
						sName = "[" + sName + "]"
					screen.addPullDownString("WorldBuilderPlayerChoice", sName, iPlayer, iPlayer, self.iCurrentPlayer == iPlayer)
			iX = iXStart + 8
			iY += iAdjust
			screen.setImageButton("WorldBuilderUnrevealAll", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_UNREVEAL_ALL_TILES").getPath(), iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_WB_UNREVEAL_ALL_BUTTON, -1, -1)
			iX += iAdjust
			iWidth = (self.xRes - 8 - iX - 3)/2
			screen.addDropDownBoxGFC("BrushWidth", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for i in range(1, 11):
				screen.addPullDownString("BrushWidth", "W: " + str(i), i, i, self.iBrushWidth == i)
			screen.addPullDownString("BrushWidth", "W: " + "--", -1, -1, self.iBrushWidth == -1)
			iX += iWidth
			screen.addDropDownBoxGFC("BrushHeight", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for i in range(1, 11):
				screen.addPullDownString("BrushHeight", "H: " + str(i), i, i, self.iBrushHeight == i)
			screen.addPullDownString("BrushHeight", "H: " + "--", -1, -1, self.iBrushHeight == -1)

		elif self.iPlayerAddMode == "EraseAll":
			iX = iXStart + 8
			screen.setImageButton( "WorldBuilderEraseAll", CyArtFileMgr().getInterfaceArtInfo("WORLDBUILDER_UNREVEAL_ALL_TILES").getPath(), iX, iY, iButtonWidth, iButtonWidth, WidgetTypes.WIDGET_PYTHON, 1029, 29)
			iX += iAdjust
			iWidth = (self.xRes - 8 - iX - 3)/2
			screen.addDropDownBoxGFC("BrushWidth", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for i in range(1, 11):
				screen.addPullDownString("BrushWidth", "W: " + str(i), i, i, self.iBrushWidth == i)
			screen.addPullDownString("BrushWidth", "W: " + "--", -1, -1, self.iBrushWidth == -1)
			iX += iWidth
			screen.addDropDownBoxGFC("BrushHeight", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for i in range(1, 11):
				screen.addPullDownString("BrushHeight", "H: " + str(i), i, i, self.iBrushHeight == i)
			screen.addPullDownString("BrushHeight", "H: " + "--", -1, -1, self.iBrushHeight == -1)
		else:
			screen.deleteWidget("WorldBuilderBackgroundBottomPanel")
		self.setCurrentModeCheckbox()
		self.setSelectionTable()

	def setCurrentModeCheckbox(self):
		screen = self.getScreen()
		screen.setState("EditUnitData", self.iPlayerAddMode == "EditUnit")
		screen.setState("EditPromotions", self.iPlayerAddMode == "Promotions")
		screen.setState("WorldBuilderNormalPlayerModeButton", self.iPlayerAddMode in self.PlayerMode)
		screen.setState("WorldBuilderNormalMapModeButton", self.iPlayerAddMode in self.MapMode)
		screen.setState("WorldBuilderRevealTileModeButton", self.iPlayerAddMode in self.RevealMode)
		screen.setState("WorldBuilderLandmarkButton", self.iPlayerAddMode == "AddLandMark")
		screen.setState("WorldBuilderEraseButton", self.iPlayerAddMode == "EraseAll")
		screen.setState("AddOwnershipButton", self.iPlayerAddMode == "Ownership")
		screen.setState("AddUnitsButton", self.iPlayerAddMode == "Units")
		screen.setState("AddBuildingsButton", self.iPlayerAddMode == "Buildings")
		screen.setState("AddCityButton", self.iPlayerAddMode == "City")
		screen.setState("EditStartingPlot", self.iPlayerAddMode == "StartingPlot")
		screen.setState("EditPlotData", self.iPlayerAddMode == "PlotData")
		screen.setState("EditEvents", self.iPlayerAddMode == "Events")
		screen.setState("AddRiverButton", self.iPlayerAddMode == "River")
		screen.setState("AddImprovementButton", self.iPlayerAddMode == "Improvements")
		screen.setState("AddBonusButton", self.iPlayerAddMode == "Bonus")
		screen.setState("AddPlotTypeButton", self.iPlayerAddMode == "PlotType")
		screen.setState("AddTerrainButton", self.iPlayerAddMode == "Terrain")
		screen.setState("AddRouteButton", self.iPlayerAddMode == "Routes")
		screen.setState("AddFeatureButton", self.iPlayerAddMode == "Features")
		screen.setState("EditCityDataI", self.iPlayerAddMode == "CityDataI")
		screen.setState("EditCityDataII", self.iPlayerAddMode == "CityDataII")
		screen.setState("EditCityBuildings", self.iPlayerAddMode == "CityBuildings")
		screen.setState("PythonEffectButton", bPython)
		screen.setState("SensibilityCheck", self.bSensibility)

	def setSelectionTable(self):
		screen = self.getScreen()
		iWidth = 256
		if self.iPlayerAddMode == "Units":
			iY = 25
			screen.addDropDownBoxGFC("WBSelectClass", 0, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			screen.addPullDownString("WBSelectClass", CyTranslator().getText("TXT_KEY_WB_CITY_ALL",()), -2, -2, -2 == self.iSelectClass)
			screen.addPullDownString("WBSelectClass", CyTranslator().getText("TXT_PEDIA_NON_COMBAT",()), -1, -1, -1 == self.iSelectClass)
			for iDomain in xrange(DomainTypes.NUM_DOMAIN_TYPES):
				screen.addPullDownString("WBSelectClass", GC.getDomainInfo(iDomain).getDescription(), iDomain, iDomain, iDomain == self.iSelectClass)

			lItems = []
			for i in xrange(GC.getNumUnitInfos()):
				CvUnitInfo = GC.getUnitInfo(i)
				if self.iSelectClass == -1:
					if CvUnitInfo.getCombat() > 0 or CvUnitInfo.getAirCombat() > 0:
						continue
				elif self.iSelectClass > -1 and CvUnitInfo.getDomainType() != self.iSelectClass:
					continue
				lItems.append([CvUnitInfo.getDescription(), i])
			lItems.sort()

			iY += 30
			iHeight = min(len(lItems) * 24 + 2, self.yRes - iY)
			screen.addTableControlGFC("WBSelectItem", 1, 0, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
			screen.setStyle("WBSelectItem", "Table_StandardCiv_Style")
			screen.enableSelect("WBSelectItem", True)
			screen.setTableColumnHeader("WBSelectItem", 0, "", iWidth)
			bValid = False
			for item in lItems:
				iRow = screen.appendTableRow("WBSelectItem")
				if self.iSelection == item[1]: bValid = True
				screen.setTableText("WBSelectItem", 0, iRow, "<font=3>" + item[0] + "</font>", GC.getUnitInfo(item[1]).getButton(), WidgetTypes.WIDGET_PYTHON, 8202, item[1], 1<<0)
			if not bValid:
				self.iSelection = -1
				if len(lItems) > 0:
					self.iSelection = lItems[0][1]

		elif self.iPlayerAddMode == "Buildings":
			iY = 25
			sWonder = CyTranslator().getText("TXT_KEY_CONCEPT_WONDERS", ())
			screen.addDropDownBoxGFC("WBSelectClass", 0, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			screen.addPullDownString("WBSelectClass", CyTranslator().getText("TXT_KEY_WB_CITY_ALL",()), 0, 0, 0 == self.iSelectClass)
			screen.addPullDownString("WBSelectClass", CyTranslator().getText("TXT_KEY_WB_BUILDINGS",()), 1, 1, 1 == self.iSelectClass)
			screen.addPullDownString("WBSelectClass", CyTranslator().getText("TXT_KEY_PEDIA_NATIONAL_WONDER", ()), 2, 2, 2 == self.iSelectClass)
			screen.addPullDownString("WBSelectClass", CyTranslator().getText("TXT_KEY_PEDIA_TEAM_WONDER", ()), 3, 3, 3 == self.iSelectClass)
			screen.addPullDownString("WBSelectClass", CyTranslator().getText("TXT_KEY_PEDIA_WORLD_WONDER", ()), 4, 4, 4 == self.iSelectClass)

			lItems = []
			for i in xrange(GC.getNumBuildingInfos()):

				if self.iSelectClass == 1:
					if isLimitedWonder(i):
						continue
				elif self.iSelectClass == 2:
					if not isNationalWonder(i):
						continue
				elif self.iSelectClass == 3:
					if not isTeamWonder(i):
						continue
				elif self.iSelectClass == 4:
					if not isWorldWonder(i):
						continue
				lItems.append([GC.getBuildingInfo(i).getDescription(), i])
			lItems.sort()

			iY += 30
			iHeight = min(len(lItems) * 24 + 2, self.yRes - iY)
			screen.addTableControlGFC("WBSelectItem", 1, 0, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
			screen.setStyle("WBSelectItem", "Table_StandardCiv_Style")
			screen.enableSelect("WBSelectItem", True)
			screen.setTableColumnHeader("WBSelectItem", 0, "", iWidth)
			bValid = False
			for item in lItems:
				iRow = screen.appendTableRow("WBSelectItem")
				if self.iSelection == item[1]: bValid = True
				screen.setTableText("WBSelectItem", 0, iRow, "<font=3>" + item[0] + "</font>", GC.getBuildingInfo(item[1]).getButton(), WidgetTypes.WIDGET_HELP_BUILDING, item[1], item[1], 1<<0)
			if not bValid:
				self.iSelection = -1
				if len(lItems) > 0:
					self.iSelection = lItems[0][1]

		elif self.iPlayerAddMode == "Features":
			iY = 55
			lItems = []
			for i in xrange(GC.getNumFeatureInfos()):
				ItemInfo = GC.getFeatureInfo(i)
				lItems.append([ItemInfo.getDescription(), i])
			lItems.sort()

			iHeight = min(len(lItems) * 24 + 2, self.yRes - iY)
			screen.addTableControlGFC("WBSelectItem", 1, 0, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
			screen.setStyle("WBSelectItem", "Table_StandardCiv_Style")
			screen.enableSelect("WBSelectItem", True)
			screen.setTableColumnHeader("WBSelectItem", 0, "", iWidth)
			for item in lItems:
				iRow = screen.appendTableRow("WBSelectItem")
				if self.iSelection == -1:
					self.iSelection = item[1]
				screen.setTableText("WBSelectItem", 0, iRow, "<font=3>" + item[0] + "</font>", GC.getFeatureInfo(item[1]).getButton(), WidgetTypes.WIDGET_PYTHON, 7874, item[1], 1<<0)

		elif self.iPlayerAddMode == "Improvements":
			iY = 25
			lItems = []
			for i in xrange(GC.getNumImprovementInfos()):
				ItemInfo = GC.getImprovementInfo(i)
				if ItemInfo.isGraphicalOnly(): continue
				lItems.append([ItemInfo.getDescription(), i])
			lItems.sort()

			iHeight = min(len(lItems) * 24 + 2, self.yRes - iY)
			screen.addTableControlGFC("WBSelectItem", 1, 0, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
			screen.setStyle("WBSelectItem", "Table_StandardCiv_Style")
			screen.enableSelect("WBSelectItem", True)
			screen.setTableColumnHeader("WBSelectItem", 0, "", iWidth)
			for item in lItems:
				iRow = screen.appendTableRow("WBSelectItem")
				if self.iSelection == -1:
					self.iSelection = item[1]
				screen.setTableText("WBSelectItem", 0, iRow, "<font=3>" + item[0] + "</font>", GC.getImprovementInfo(item[1]).getButton(), WidgetTypes.WIDGET_PYTHON, 7877, item[1], 1<<0)

		elif self.iPlayerAddMode == "Bonus":
			iY = 25
			lItems = []
			for i in xrange(GC.getNumBonusInfos()):
				CvBonusInfo = GC.getBonusInfo(i)
				if CvBonusInfo.getPlacementOrder() > -1:
					lItems.append([CvBonusInfo.getDescription(), i])
			lItems.sort()

			iHeight = min(len(lItems) * 24 + 2, self.yRes - iY)
			screen.addTableControlGFC("WBSelectItem", 1, 0, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
			screen.setStyle("WBSelectItem", "Table_StandardCiv_Style")
			screen.enableSelect("WBSelectItem", True)
			screen.setTableColumnHeader("WBSelectItem", 0, "", iWidth)
			for item in lItems:
				iRow = screen.appendTableRow("WBSelectItem")
				if self.iSelection == -1:
					self.iSelection = item[1]
				screen.setTableText("WBSelectItem", 0, iRow, "<font=3>" + item[0] + "</font>", GC.getBonusInfo(item[1]).getButton(), WidgetTypes.WIDGET_PYTHON, 7878, item[1], 1<<0)

		elif self.iPlayerAddMode == "Routes":
			iY = 25
			lItems = []
			for i in xrange(GC.getNumRouteInfos()):
				ItemInfo = GC.getRouteInfo(i)
				lItems.append([ItemInfo.getDescription(), i])
			lItems.sort()

			iHeight = min(len(lItems) * 24 + 2, self.yRes - iY)
			screen.addTableControlGFC("WBSelectItem", 1, 0, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
			screen.setStyle("WBSelectItem", "Table_StandardCiv_Style")
			screen.enableSelect("WBSelectItem", True)
			screen.setTableColumnHeader("WBSelectItem", 0, "", iWidth)
			for item in lItems:
				iRow = screen.appendTableRow("WBSelectItem")
				if self.iSelection == -1:
					self.iSelection = item[1]
				screen.setTableText("WBSelectItem", 0, iRow, "<font=3>" + item[0] + "</font>", GC.getRouteInfo(item[1]).getButton(), WidgetTypes.WIDGET_PYTHON, 6788, item[1], 1<<0)

		elif self.iPlayerAddMode == "Terrain":
			iY = 25
			lItems = []
			for i in xrange(GC.getNumTerrainInfos()):
				ItemInfo = GC.getTerrainInfo(i)
				if ItemInfo.isGraphicalOnly(): continue
				lItems.append([ItemInfo.getDescription(), i])
			lItems.sort()

			iHeight = min(len(lItems) * 24 + 2, self.yRes - iY)
			screen.addTableControlGFC("WBSelectItem", 1, 0, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
			screen.setStyle("WBSelectItem", "Table_StandardCiv_Style")
			screen.enableSelect("WBSelectItem", True)
			screen.setTableColumnHeader("WBSelectItem", 0, "", iWidth)
			for item in lItems:
				iRow = screen.appendTableRow("WBSelectItem")
				if self.iSelection == -1:
					self.iSelection = item[1]
				screen.setTableText("WBSelectItem", 0, iRow, "<font=3>" + item[0] + "</font>", GC.getTerrainInfo(item[1]).getButton(), WidgetTypes.WIDGET_PYTHON, 7875, item[1], 1<<0)

		elif self.iPlayerAddMode == "PlotType":
			iY = 25
			iHeight = 4 * 24 + 2
			screen.addTableControlGFC("WBSelectItem", 1, 0, 25, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
			screen.setStyle("WBSelectItem", "Table_StandardCiv_Style")
			screen.enableSelect("WBSelectItem", True)
			screen.setTableColumnHeader("WBSelectItem", 0, "", iWidth)
			for i in xrange(PlotTypes.NUM_PLOT_TYPES):
				screen.appendTableRow("WBSelectItem")
			item = GC.getInfoTypeForString("TERRAIN_PEAK")
			if self.iSelection == -1:
				self.iSelection = item
			screen.setTableText("WBSelectItem", 0, 0, "<font=3>" + GC.getTerrainInfo(item).getDescription() + "</font>", GC.getTerrainInfo(item).getButton(), WidgetTypes.WIDGET_PYTHON, 7875, item, 1<<0)
			item = GC.getInfoTypeForString("TERRAIN_HILL")
			screen.setTableText("WBSelectItem", 0, 1, "<font=3>" + GC.getTerrainInfo(item).getDescription() + "</font>", GC.getTerrainInfo(item).getButton(), WidgetTypes.WIDGET_PYTHON, 7875, item, 1<<0)
			item = GC.getInfoTypeForString("TERRAIN_GRASSLAND")
			screen.setTableText("WBSelectItem", 0, 2, "<font=3>" + GC.getTerrainInfo(item).getDescription() + "</font>", GC.getTerrainInfo(item).getButton(), WidgetTypes.WIDGET_PYTHON, 7875, item, 1<<0)
			item = GC.getInfoTypeForString("TERRAIN_OCEAN")
			screen.setTableText("WBSelectItem", 0, 3, "<font=3>" + GC.getTerrainInfo(item).getDescription() + "</font>", GC.getTerrainInfo(item).getButton(), WidgetTypes.WIDGET_PYTHON, 7875, item, 1<<0)
		self.refreshSelection()

	def refreshSelection(self):
		if self.iSelection == -1: return
		screen = self.getScreen()
		iWidth = 200
		screen.addTableControlGFC("WBCurrentItem", 1, 0, 0, iWidth, 25, False, True, 24, 24, TableStyles.TABLE_STYLE_EMPTY)
		screen.setTableColumnHeader("WBCurrentItem", 0, "", iWidth)
		screen.appendTableRow("WBCurrentItem")
		if self.iPlayerAddMode == "Units":
			ItemInfo = GC.getUnitInfo(self.iSelection)
			sText = "<font=3>" + CyTranslator().getText("[COLOR_HIGHLIGHT_TEXT]", ()) + ItemInfo.getDescription() + "</color></font>"
			screen.setTableText("WBCurrentItem", 0, 0 , sText, ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 8202, self.iSelection, 1<<0)
		elif self.iPlayerAddMode == "Buildings":
			ItemInfo = GC.getBuildingInfo(self.iSelection)
			sText = "<font=3>" + CyTranslator().getText("[COLOR_HIGHLIGHT_TEXT]", ()) + ItemInfo.getDescription() + "</color></font>"
			screen.setTableText("WBCurrentItem", 0, 0 , sText, ItemInfo.getButton(), WidgetTypes.WIDGET_HELP_BUILDING, self.iSelection, 1, 1<<0)
		elif self.iPlayerAddMode == "Features":
			ItemInfo = GC.getFeatureInfo(self.iSelection)
			sText = "<font=3>" + CyTranslator().getText("[COLOR_HIGHLIGHT_TEXT]", ()) + ItemInfo.getDescription() + "</color></font>"
			screen.setTableText("WBCurrentItem", 0, 0 , sText, ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 7874, self.iSelection, 1<<0)
			if ItemInfo.getNumVarieties() > 1:
				screen.addDropDownBoxGFC("WBSelectClass", 0, 25, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				for i in xrange(ItemInfo.getNumVarieties()):
					screen.addPullDownString("WBSelectClass", CyTranslator().getText("TXT_KEY_WB_FEATURE_VARIETY", (i,)), i, i, i == self.iSelectClass)
			else:
				self.iSelectClass = 0
				screen.hide("WBSelectClass")
		elif self.iPlayerAddMode == "Improvements":
			ItemInfo = GC.getImprovementInfo(self.iSelection)
			sText = "<font=3>" + CyTranslator().getText("[COLOR_HIGHLIGHT_TEXT]", ()) + ItemInfo.getDescription() + "</color></font>"
			screen.setTableText("WBCurrentItem", 0, 0 , sText, ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 7877, self.iSelection, 1<<0)
		elif self.iPlayerAddMode == "Bonus":
			ItemInfo = GC.getBonusInfo(self.iSelection)
			sText = "<font=3>" + CyTranslator().getText("[COLOR_HIGHLIGHT_TEXT]", ()) + ItemInfo.getDescription() + "</color></font>"
			screen.setTableText("WBCurrentItem", 0, 0 , sText, ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 7878, self.iSelection, 1<<0)
		elif self.iPlayerAddMode == "Routes":
			ItemInfo = GC.getRouteInfo(self.iSelection)
			sText = "<font=3>" + CyTranslator().getText("[COLOR_HIGHLIGHT_TEXT]", ()) + ItemInfo.getDescription() + "</color></font>"
			screen.setTableText("WBCurrentItem", 0, 0 , sText, ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 6788, self.iSelection, 1<<0)
		elif self.iPlayerAddMode == "Terrain" or self.iPlayerAddMode == "PlotType":
			ItemInfo = GC.getTerrainInfo(self.iSelection)
			sText = "<font=3>" + CyTranslator().getText("[COLOR_HIGHLIGHT_TEXT]", ()) + ItemInfo.getDescription() + "</color></font>"
			screen.setTableText("WBCurrentItem", 0, 0 , sText, ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, 7875, self.iSelection, 1<<0)
		else:
			screen.hide("WBCurrentItem")


	def revealAll(self, bReveal):
		for pPlot in MAP.plots():
			if pPlot.isNone(): continue
			self.RevealCurrentPlot(bReveal, pPlot)
		self.refreshReveal()

	def RevealCurrentPlot(self, bReveal, pPlot):
		if self.iPlayerAddMode == "Blockade": return
		iType = GC.getInfoTypeForStringWithHiddenAssert(self.iPlayerAddMode)
		if iType == -1:
			if bReveal or (not pPlot.isVisible(self.m_iCurrentTeam, False)):
				pPlot.setRevealed(self.m_iCurrentTeam, bReveal, False, -1)
		elif bReveal:
			if pPlot.isInvisibleVisible(self.m_iCurrentTeam, iType): return
			pPlot.changeInvisibleVisibilityCount(self.m_iCurrentTeam, iType, 1)
		else:
			pPlot.changeInvisibleVisibilityCount(self.m_iCurrentTeam, iType, - pPlot.getInvisibleVisibilityCount(self.m_iCurrentTeam, iType))

	def showRevealed(self, pPlot):
		if self.iPlayerAddMode == "RevealPlot":
			if not pPlot.isRevealed(self.m_iCurrentTeam, False):
				CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS, "COLOR_BLACK", 1.0)
		elif self.iPlayerAddMode == "INVISIBLE_SUBMARINE":
			if pPlot.getInvisibleVisibilityCount(self.m_iCurrentTeam, GC.getInfoTypeForString(self.iPlayerAddMode)) == 0:
				CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS, "COLOR_RED", 1.0)
		elif self.iPlayerAddMode == "INVISIBLE_STEALTH":
			if pPlot.getInvisibleVisibilityCount(self.m_iCurrentTeam, GC.getInfoTypeForString(self.iPlayerAddMode)) == 0:
				CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS, "COLOR_BLUE", 1.0)
		elif self.iPlayerAddMode == "Blockade":
			if pPlot.isTradeNetwork(self.m_iCurrentTeam): return
			if GC.getTeam(self.m_iCurrentTeam).isAtWar(pPlot.getTeam()): return
			if pPlot.isTradeNetworkImpassable(self.m_iCurrentTeam): return
			if not pPlot.isOwned() and not pPlot.isRevealed(self.m_iCurrentTeam, False): return
			if not pPlot.isBonusNetwork(self.m_iCurrentTeam): return
			CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS, "COLOR_MAGENTA", 1.0)


	def setRiverHighlights(self):
		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY(), PlotStyles.PLOT_STYLE_RIVER_SOUTH, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_GREEN", 1)

		for x in xrange(self.m_pRiverStartPlot.getX() - 1, self.m_pRiverStartPlot.getX() + 2):
			for y in xrange(self.m_pRiverStartPlot.getY() - 1, self.m_pRiverStartPlot.getY() + 2):
				if x == self.m_pRiverStartPlot.getX() and y == self.m_pRiverStartPlot.getY(): continue
				CyEngine().addColoredPlotAlt(x, y, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", .2)


	def leftMouseDown(self, argsList):
		bShift, bCtrl, bAlt = argsList
		pPlayer = GC.getPlayer(self.iCurrentPlayer)

		if bAlt or self.iPlayerAddMode == "EditUnit":
			if self.m_pCurrentPlot.getNumUnits():
				WBUnitScreen.WBUnitScreen(self).interfaceScreen(self.m_pCurrentPlot.getUnit(0))
		elif self.iPlayerAddMode == "Promotions":
			if self.m_pCurrentPlot.getNumUnits():
				WBPromotionScreen.WBPromotionScreen(self).interfaceScreen(self.m_pCurrentPlot.getUnit(0))
		elif bCtrl or self.iPlayerAddMode == "CityDataI":
			if self.m_pCurrentPlot.isCity():
				WBCityEditScreen.WBCityEditScreen(self).interfaceScreen(self.m_pCurrentPlot.getPlotCity())
		elif self.iPlayerAddMode == "CityDataII":
			if self.m_pCurrentPlot.isCity():
				WBCityDataScreen.WBCityDataScreen(self).interfaceScreen(self.m_pCurrentPlot.getPlotCity())
		elif self.iPlayerAddMode == "CityBuildings":
			if self.m_pCurrentPlot.isCity():
				WBBuildingScreen.WBBuildingScreen(self).interfaceScreen(self.m_pCurrentPlot.getPlotCity())
		elif self.iPlayerAddMode in self.RevealMode:
			if not self.m_pCurrentPlot.isNone():
				self.setMultipleReveal(True)
		elif self.iPlayerAddMode == "PlotData":
			WBPlotScreen.WBPlotScreen(self).interfaceScreen(self.m_pCurrentPlot)
		elif self.iPlayerAddMode == "Events":
			WBEventScreen.WBEventScreen(self).interfaceScreen(self.m_pCurrentPlot)
		elif self.iPlayerAddMode == "StartingPlot":
			pPlayer.setStartingPlot(self.m_pCurrentPlot, True)
			self.refreshStartingPlots()
		elif self.iPlayerAddMode == "TargetPlot":
			self.iTargetPlotX = self.m_pCurrentPlot.getX()
			self.iTargetPlotY = self.m_pCurrentPlot.getY()
			self.iPlayerAddMode = "EditUnit"
			if len(self.TempInfo) >= 2:
				pPlayerX = GC.getPlayer(self.TempInfo[0])
				if pPlayerX:
					pUnitX = pPlayerX.getUnit(self.TempInfo[1])
					if pUnitX:
						WBUnitScreen.WBUnitScreen(self).interfaceScreen(pUnitX)
		elif self.iPlayerAddMode == "MoveUnits":
			if len(self.lMoveUnit) > 0:
				for item in self.lMoveUnit:
					loopUnit = GC.getPlayer(item[0]).getUnit(item[1])
					if loopUnit is None: continue
					loopUnit.setXY(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY(), True, True, False)
				pUnitX = GC.getPlayer(self.lMoveUnit[0][0]).getUnit(self.lMoveUnit[0][1])
				self.lMoveUnit = []
				self.iPlayerAddMode = "EditUnit"
				WBUnitScreen.WBUnitScreen(self).interfaceScreen(pUnitX)
		elif self.iPlayerAddMode == "MoveCity" or self.iPlayerAddMode == "MoveCityPlus":
			if self.m_pCurrentPlot.isCity(): return
			pOldCity = pPlayer.getCity(self.iMoveCity)
			if pOldCity:
				pNewCity = pPlayer.initCity(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY())
				sName = pOldCity.getName()
				pOldCity.setName("ToBeRazed", False)
				pNewCity.setName(sName, True)
				self.copyCityStats(pOldCity, pNewCity, True)
				pOldPlot = pOldCity.plot()
				pOldCity.kill()
				pOldPlot.setImprovementType(-1)
				if self.iPlayerAddMode == "MoveCityPlus":
					for item in self.lMoveUnit:
						loopUnit = GC.getPlayer(item[0]).getUnit(item[1])
						if loopUnit is None: continue
						loopUnit.setXY(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY(), True, True, False)
					self.lMoveUnit = []
			self.iPlayerAddMode = "CityDataI"
			self.iMoveCity = -1
		elif self.iPlayerAddMode == "DuplicateCity" or self.iPlayerAddMode == "DuplicateCityPlus":
			if self.m_pCurrentPlot.isCity(): return
			pOldCity = pPlayer.getCity(self.iMoveCity)
			if pOldCity:
				pNewCity = pPlayer.initCity(self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY())
				self.copyCityStats(pOldCity, pNewCity, False)
				if self.iPlayerAddMode == "DuplicateCityPlus":
					for item in self.lMoveUnit:
						loopUnit = GC.getPlayer(item[0]).getUnit(item[1])
						if loopUnit is None: continue
						pNewUnit = pPlayer.initUnit(loopUnit.getUnitType(), self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
						pNewUnit.setName(loopUnit.getNameNoDesc())
						pNewUnit.setLevel(loopUnit.getLevel())
						pNewUnit.setExperience(loopUnit.getExperience(), -1)
						pNewUnit.setBaseCombatStr(loopUnit.baseCombatStr())
						for iPromotion in xrange(GC.getNumPromotionInfos()):
							pNewUnit.setHasPromotion(iPromotion, loopUnit.isHasPromotion(iPromotion))
						pNewUnit.setDamage(loopUnit.getDamage(), -1)
						pNewUnit.setMoves(loopUnit.getMoves())
						pNewUnit.setLeaderUnitType(loopUnit.getLeaderUnitType())
						pNewUnit.changeCargoSpace(loopUnit.cargoSpace() - pNewUnit.cargoSpace())
						pNewUnit.setImmobileTimer(loopUnit.getImmobileTimer())
						pNewUnit.setScriptData(loopUnit.getScriptData())
		elif self.useLargeBrush():
			self.placeMultipleObjects()
		else:
			self.placeObject()
		return 1

	def copyCityStats(self, pOldCity, pNewCity, bMove):
		pNewCity.setPopulation(pOldCity.getPopulation())
		for iBuilding in xrange(GC.getNumBuildingInfos()):

			pNewCity.setBuildingProduction(iBuilding, pOldCity.getBuildingProduction(iBuilding))

			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				pNewCity.setBuildingCommerceChange(iBuilding, iCommerce, pOldCity.getBuildingCommerceChange(iBuilding, iCommerce))

			for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
				pNewCity.setBuildingYieldChange(iBuilding, iYield, pOldCity.getBuildingYieldChange(iBuilding, iYield))

			if GC.getBuildingInfo(iBuilding).isCapital() and not bMove:
				continue
			pNewCity.setNumRealBuilding(iBuilding, pOldCity.getNumRealBuilding(iBuilding))

		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			pNewCity.setCultureTimes100(iPlayerX, pOldCity.getCultureTimes100(iPlayerX), False)
		for iReligion in xrange(GC.getNumReligionInfos()):
			pNewCity.setHasReligion(iReligion, pOldCity.isHasReligion(iReligion), False, False)
			if bMove and pOldCity.isHolyCityByType(iReligion):
				GAME.setHolyCity(iReligion, pNewCity, False)
			pNewCity.changeReligionInfluence(iReligion, pOldCity.getReligionInfluence(iReligion) - pNewCity.getReligionInfluence(iReligion))
			pNewCity.changeStateReligionHappiness(iReligion, pOldCity.getStateReligionHappiness(iReligion) - pNewCity.getStateReligionHappiness(iReligion))
		for iCorporation in xrange(GC.getNumCorporationInfos()):
			pNewCity.setHasCorporation(iCorporation, pOldCity.isHasCorporation(iCorporation), False, False)
			if bMove and pOldCity.isHeadquartersByType(iCorporation):
				GAME.setHeadquarters(iCorporation, pNewCity, False)
		for iImprovement in xrange(GC.getNumImprovementInfos()):
			pNewCity.changeImprovementFreeSpecialists(iImprovement, pOldCity.getImprovementFreeSpecialists(iImprovement) - pNewCity.getImprovementFreeSpecialists(iImprovement))
		for iSpecialist in xrange(GC.getNumSpecialistInfos()):
			pNewCity.setFreeSpecialistCount(iSpecialist, pOldCity.getFreeSpecialistCount(iSpecialist))
			pNewCity.setForceSpecialistCount(iSpecialist, pOldCity.getForceSpecialistCount(iSpecialist))
		for iUnit in xrange(GC.getNumUnitInfos()):
			pNewCity.setUnitProduction(iUnit, pOldCity.getUnitProduction(iUnit))
			pNewCity.setGreatPeopleUnitProgress(iUnit, pOldCity.getGreatPeopleUnitProgress(iUnit))
		for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
			pNewCity.changeSpecialistCommerce(iCommerce, pOldCity.getSpecialistCommerce(iCommerce) - pNewCity.getSpecialistCommerce(iCommerce))
		for iBonus in xrange(GC.getNumBonusInfos()):
			pNewCity.changeFreeBonus(iBonus, pOldCity.getFreeBonus(iBonus) - pNewCity.getFreeBonus(iBonus))
			while pOldCity.isNoBonus(iBonus) != pNewCity.isNoBonus(iBonus):
				if pOldCity.isNoBonus(iBonus):
					pNewCity.changeNoBonusCount(iBonus, 1)
				else:
					pNewCity.changeNoBonusCount(iBonus, -1)
		for iOrder in xrange(pOldCity.getOrderQueueLength()):
			OrderData = pOldCity.getOrderFromQueue(iOrder)
			pNewCity.pushOrder(OrderData.eOrderType, OrderData.iData1, OrderData.iData2, OrderData.bSave, False, True, False)
		pNewCity.changeBaseGreatPeopleRate(pOldCity.getBaseGreatPeopleRate() - pNewCity.getBaseGreatPeopleRate())
		pNewCity.changeConscriptAngerTimer(pOldCity.getConscriptAngerTimer() - pNewCity.getConscriptAngerTimer())
		pNewCity.changeDefenseDamage(pOldCity.getDefenseDamage() - pNewCity.getDefenseDamage())
		pNewCity.changeDefyResolutionAngerTimer(pOldCity.getDefyResolutionAngerTimer() - pNewCity.getDefyResolutionAngerTimer())
		pNewCity.changeEspionageHappinessCounter(pOldCity.getEspionageHappinessCounter() - pNewCity.getEspionageHappinessCounter())
		pNewCity.changeEspionageHealthCounter(pOldCity.getEspionageHealthCounter() - pNewCity.getEspionageHealthCounter())
		pNewCity.changeExtraHappiness(pOldCity.getExtraHappiness() - pNewCity.getExtraHappiness())
		pNewCity.changeExtraHealth(pOldCity.getExtraHealth() - pNewCity.getExtraHealth())
		pNewCity.changeExtraTradeRoutes(pOldCity.getExtraTradeRoutes() - pNewCity.getExtraTradeRoutes())
		pNewCity.changeGreatPeopleProgress(pOldCity.getGreatPeopleProgress() - pNewCity.getGreatPeopleProgress())
		pNewCity.changeHappinessTimer(pOldCity.getHappinessTimer() - pNewCity.getHappinessTimer())
		pNewCity.changeHurryAngerTimer(pOldCity.getHurryAngerTimer() - pNewCity.getHurryAngerTimer())
		pNewCity.setAirliftTargeted(pOldCity.isAirliftTargeted())
		pNewCity.setBombarded(pOldCity.isBombarded())
		pNewCity.setCitizensAutomated(pOldCity.isCitizensAutomated())
		pNewCity.setDrafted(pOldCity.isDrafted())
		pNewCity.setFeatureProduction(pOldCity.getFeatureProduction())
		pNewCity.setFood(pOldCity.getFood())
		pNewCity.setHighestPopulation(pOldCity.getHighestPopulation())
		pNewCity.setNeverLost(pOldCity.isNeverLost())
		pNewCity.setOccupationTimer(pOldCity.getOccupationTimer())
		pNewCity.setOverflowProduction(pOldCity.getOverflowProduction())
		pNewCity.setPlundered(pOldCity.isPlundered())
		pNewCity.setProduction(pOldCity.getProduction())
		pNewCity.setProductionAutomated(pOldCity.isProductionAutomated())
		pNewCity.setScriptData(pOldCity.getScriptData())
		pNewCity.setWallOverride(pOldCity.isWallOverride())

	def rightMouseDown (self):

		if self.iPlayerAddMode in self.RevealMode:
			if not self.m_pCurrentPlot.isNone():
				self.setMultipleReveal(False)
		elif self.useLargeBrush():
			self.removeMultipleObjects()
		else:
			self.removeObject()
		return 1


	def addComma(self, iValue):
		sTemp = str(iValue)
		sStart = ""
		while len(sTemp) > 0:
			if sTemp[0].isdigit(): break
			sStart += sTemp[0]
			sTemp = sTemp[1:]
		sEnd = sTemp[-3:]
		while len(sTemp) > 3:
			sTemp = sTemp[:-3]
			sEnd = sTemp[-3:] + "," + sEnd
		return (sStart + sEnd)

	def goToSubScreen(self, goTo):

		if not self.inSubScreen is None:
			self.inSubScreen.exit(self.getScreen())

		if goTo in self.subScreens:
			self.inSubScreen = self.subScreens[goTo]
			self.inSubScreen.interfaceScreen()
		else:
			raise "WorldBuilder.goToScreen - invalid argument: goTo = '%s'" % goTo

	def getScreen(self):
		return CyGInterfaceScreen("WorldBuilderScreen", self.screenId)

	#--------------------------#
	# Base operation functions #
	#||||||||||||||||||||||||||#
	def update(self, fDelta):
		if self.tooltip.bLockedTT:
			self.tooltip.handle(self.getScreen())

	def handleInput(self, inputClass):
		screen = self.getScreen()

		self.tooltip.reset(screen)

		if self.inSubScreen:
			return self.inSubScreen.handleInput(inputClass, screen)

		bAlt, bCtrl, bShift = self.InputData.getModifierKeys()
		iCode	= inputClass.eNotifyCode
		iData	= inputClass.iData
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName
		iBtn	= inputClass.iButtonType
		iData1	= inputClass.iData1
		#iData2	= inputClass.iData2
		szFlag	= HandleInputUtil.MOUSE_FLAGS.get(inputClass.uiFlags, "UNKNOWN")

		if NAME == "WorldBuilderEraseAll":
			for plot in MAP.plots():
				self.m_pCurrentPlot = plot
				self.placeObject()

		elif NAME == "TradeScreen":
			WBTradeScreen.WBTradeScreen(self).interfaceScreen()

		elif NAME == "InfoScreen":
			WBInfoScreen.WBInfoScreen(self).interfaceScreen(self.iCurrentPlayer)

		elif NAME == "EditGameOptions":
			self.goToSubScreen("GameScreen")

		elif NAME == "EditReligions":
			WBReligionScreen.WBReligionScreen(self).interfaceScreen(self.iCurrentPlayer)

		elif NAME == "EditCorporations":
			WBCorporationScreen.WBCorporationScreen(self).interfaceScreen(self.iCurrentPlayer)

		elif NAME == "EditEspionage":
			WBDiplomacyScreen.WBDiplomacyScreen(self).interfaceScreen(self.iCurrentPlayer, True)

		elif NAME == "EditPlayerData":
			WBPlayerScreen.WBPlayerScreen(self).interfaceScreen(self.iCurrentPlayer)

		elif NAME == "EditTeamData":
			WBTeamScreen.WBTeamScreen(self).interfaceScreen(self.m_iCurrentTeam)

		elif NAME == "EditTechnologies":
			self.goToSubScreen("TechScreen")

		elif NAME == "EditProjects":
			WBProjectScreen.WBProjectScreen(self).interfaceScreen(self.m_iCurrentTeam)

		elif NAME == "EditUnitsCities":
			WBPlayerUnits.WBPlayerUnits(self).interfaceScreen(self.iCurrentPlayer)

		elif NAME == "WorldBuilderPlayerChoice":
			self.iCurrentPlayer = screen.getPullDownData("WorldBuilderPlayerChoice", screen.getSelectedPullDownID("WorldBuilderPlayerChoice"))
			self.m_iCurrentTeam = GC.getPlayer(self.iCurrentPlayer).getTeam()
			self.refreshSideMenu()
			if self.iPlayerAddMode in self.RevealMode:
				self.refreshReveal()

		elif NAME == "ChangeBy":
			self.iChange = screen.getPullDownData("ChangeBy", screen.getSelectedPullDownID("ChangeBy"))

		elif NAME == "AddOwnershipButton":
			self.iPlayerAddMode = "Ownership"
			self.refreshSideMenu()

		elif NAME == "AddUnitsButton":
			self.iPlayerAddMode = "Units"
			self.iSelectClass = -2
			self.iSelection = -1
			self.refreshSideMenu()

		elif NAME == "AddBuildingsButton":
			self.iPlayerAddMode = "Buildings"
			self.iSelectClass = 0
			self.iSelection = -1
			self.refreshSideMenu()

		elif NAME == "EditStartingPlot":
			self.iPlayerAddMode = "StartingPlot"
			self.refreshSideMenu()
			self.refreshStartingPlots()

		elif NAME == "EditPromotions":
			self.iPlayerAddMode = "Promotions"
			self.refreshSideMenu()

		elif NAME == "AddCityButton":
			self.iPlayerAddMode = "City"
			self.refreshSideMenu()

		elif NAME == "EditCityDataII":
			self.iPlayerAddMode = "CityDataII"
			self.refreshSideMenu()

		elif NAME == "EditCityBuildings":
			self.iPlayerAddMode = "CityBuildings"
			self.refreshSideMenu()

		elif NAME == "EditPlotData":
			self.iPlayerAddMode = "PlotData"
			self.refreshSideMenu()

		elif NAME == "EditEvents":
			self.iPlayerAddMode = "Events"
			self.refreshSideMenu()

		elif NAME == "AddImprovementButton":
			self.iPlayerAddMode = "Improvements"
			self.iSelection = -1
			self.refreshSideMenu()

		elif NAME == "AddBonusButton":
			self.iPlayerAddMode = "Bonus"
			self.iSelectClass = -1
			self.iSelection = -1
			self.refreshSideMenu()

		elif NAME == "AddPlotTypeButton":
			self.iPlayerAddMode = "PlotType"
			self.iSelection = -1
			self.refreshSideMenu()

		elif NAME == "AddTerrainButton":
			self.iPlayerAddMode = "Terrain"
			self.iSelection = -1
			self.refreshSideMenu()

		elif NAME == "AddRouteButton":
			self.iPlayerAddMode = "Routes"
			self.iSelection = -1
			self.refreshSideMenu()

		elif NAME == "AddFeatureButton":
			self.iPlayerAddMode = "Features"
			self.iSelectClass = 0
			self.iSelection = -1
			self.refreshSideMenu()

		elif NAME == "AddRiverButton":
			self.iPlayerAddMode = "River"
			self.refreshSideMenu()

		elif NAME == "WBSelectClass":
			self.iSelectClass = screen.getPullDownData("WBSelectClass", screen.getSelectedPullDownID("WBSelectClass"))
			if self.iPlayerAddMode != "Features":
				self.iSelection = -1
				self.refreshSideMenu()

		elif NAME == "WBSelectItem":
			self.iSelection = inputClass.iData2
			self.refreshSelection()

		elif NAME == "RevealMode":
			self.iPlayerAddMode = self.RevealMode[screen.getPullDownData("RevealMode", screen.getSelectedPullDownID("RevealMode"))]
			self.refreshReveal()

		elif NAME == "BrushWidth":
			self.iBrushWidth = screen.getPullDownData("BrushWidth", screen.getSelectedPullDownID("BrushWidth"))

		elif NAME == "BrushHeight":
			self.iBrushHeight = screen.getPullDownData("BrushHeight", screen.getSelectedPullDownID("BrushHeight"))

		elif NAME == "PythonEffectButton":
			global bPython
			bPython = not bPython
			self.setCurrentModeCheckbox()

		elif NAME == "SensibilityCheck":
			self.bSensibility = not self.bSensibility
			self.setCurrentModeCheckbox()
		return 0

	def killScreen(self):
		screen = self.getScreen()
		screen.hideScreen()
		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_HIGHLIGHT_PLOT)
		del self.InputData, self.subScreens, self.inSubScreen, self.xRes, self.yRes, self.iCurrentPlayer, \
			self.iPlayerAddMode, self.iSelection, self.iSelectClass, self.iBrushWidth, self.iBrushHeight, self.iChange
		self.bNotWB = True
