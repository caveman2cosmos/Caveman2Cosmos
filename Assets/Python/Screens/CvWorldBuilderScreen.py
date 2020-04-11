from CvPythonExtensions import *
import CvScreensInterface as UP
import HandleInputUtil
import PythonToolTip as pyTT

GC = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
TRNSLTR = CyTranslator()

class CvWorldBuilderScreen:

	def __init__(self, screenId):
		self.screenId = screenId

		self.m_advancedStartTabCtrl = None
		self.m_bUnitEdit = False
		self.m_bCityEdit = False
		self.m_bNormalPlayer = True
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
		self.m_bShowBigBrush = False
		self.m_bLeftMouseDown = False
		self.m_bRightMouseDown = False
		self.m_bChangeFocus = False
		self.m_iNormalPlayerCurrentIndexes = []
		self.m_iNormalMapCurrentIndexes = []
		self.m_iNormalMapCurrentList = []
		self.m_iAdvancedStartCurrentIndexes = []
		self.m_iAdvancedStartCurrentList = []
		self.m_iCurrentPlayer = 0
		self.m_iCurrentTeam = 0
		self.m_iCurrentUnit = 0
		self.m_iCurrentX = -1
		self.m_iCurrentY = -1
		self.m_pCurrentPlot = 0
		self.m_pActivePlot = 0
		self.m_pRiverStartPlot = -1

		self.m_iUnitTabID = -1
		self.m_iBuildingTabID = -1
		self.m_iTechnologyTabID = -1
		self.m_iImprovementTabID = -1
		self.m_iBonusTabID = -1
		self.m_iImprovementListID = -1
		self.m_iBonusListID = -1
		self.m_iTerrainTabID = -1
		self.m_iTerrainListID = -1
		self.m_iFeatureListID = -1
		self.m_iPlotTypeListID = -1
		self.m_iRouteListID = -1
		self.m_iTerritoryTabID = -1
		self.m_iTerritoryListID = -1

		self.m_iASUnitTabID = -1
		self.m_iASUnitListID = -1
		self.m_iASCityTabID = -1
		self.m_iASCityListID = -1
		self.m_iASBuildingsListID = -1
		self.m_iASAutomateListID = -1
		self.m_iASImprovementsTabID = -1
		self.m_iASRoutesListID = -1
		self.m_iASImprovementsListID = -1
		self.m_iASVisibilityTabID = -1
		self.m_iASVisibilityListID = -1
		self.m_iASTechTabID = -1
		self.m_iASTechListID = -1

		self.m_iBrushSizeTabID = -1
		self.m_iUnitEditCheckboxID = -1
		self.m_iCityEditCheckboxID = -1
		self.m_iNormalPlayerCheckboxID = -1
		self.m_iNormalMapCheckboxID = -1
		self.m_iRevealTileCheckboxID = -1
		self.m_iDiplomacyCheckboxID = -1
		self.m_iLandmarkCheckboxID = -1
		self.m_iEraseCheckboxID = -1

		self.m_bSideMenuDirty = False
		self.m_bASItemCostDirty = False
		self.m_iCost = 0

		self.m_iNewCivilization = -1
		self.m_iNewLeaderType = -1
		self.m_iImprovement = 0
		self.m_iYield = 0
		self.m_iDomain = 0
		self.m_iRoute = 0
		self.m_iMemory = 0
		self.m_pScript = -1
		self.m_bMoveUnit = False
		self.m_iUnitCombat = -2
		self.m_iUnitType = -1
		self.m_iPlotMode = 0
		self.m_iArea = -1
		self.m_iCityTimer = 0
		self.m_iGlobalDefine = 0
		self.m_iDiplomacyPage = 0
		self.m_bTowardsPlayer = True
		self.m_iOtherPlayer = -1
		self.m_iEventUnit = -1
		self.m_iTechShareCivs = 1
		self.m_iBuildingType = 0
		self.m_iBuildingModifier = 0
		self.m_iRevealMode = 2
		self.m_iBrushSize = 1

	def getScreen(self):
		return CyGInterfaceScreen("WorldBuilderScreen", self.screenId)

	def interfaceScreen (self):
		import InputData
		self.InputData = InputData.instance
		# Tool Tip
		self.szTxtTT = ""
		self.iOffsetTT = []
		self.bLockedTT = False
		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y
		self.aFontList = SR.aFontList

		self.m_normalPlayerTabCtrl = 0
		self.m_normalMapTabCtrl = 0
		self.m_tabCtrlEdit = 0
		self.m_bCtrlEditUp = False
		self.m_bUnitEdit = False
		self.m_bCityEdit = False
		self.m_bNormalPlayer = True
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
		self.m_bShowBigBrush = False
		self.m_bLeftMouseDown = False
		self.m_bRightMouseDown = False
		self.m_bChangeFocus = False
		self.m_iNormalPlayerCurrentIndexes = []
		self.m_iNormalMapCurrentIndexes = []
		self.m_iNormalMapCurrentList = []
		self.m_iCurrentPlayer = 0
		self.m_iCurrentTeam = 0
		self.m_iCurrentUnit = 0
		self.m_iCurrentX = -1
		self.m_iCurrentY = -1
		self.m_pCurrentPlot = 0
		self.m_pActivePlot = 0
		self.m_pRiverStartPlot = -1
		self.m_iUnitTabID = -1
		self.m_iBuildingTabID = -1
		self.m_iTechnologyTabID = -1
		self.m_iImprovementTabID = -1
		self.m_iBonusTabID = -1
		self.m_iImprovementListID = -1
		self.m_iBonusListID = -1
		self.m_iTerrainTabID = -1
		self.m_iTerrainListID = -1
		self.m_iFeatureListID = -1
		self.m_iPlotTypeListID = -1
		self.m_iRouteListID = -1
		self.m_iTerritoryTabID = -1
		self.m_iTerritoryListID = -1
		self.m_iBrushSizeTabID = -1
		self.m_iUnitEditCheckboxID = -1
		self.m_iCityEditCheckboxID = -1
		self.m_iNormalPlayerCheckboxID = -1
		self.m_iNormalMapCheckboxID = -1
		self.m_iRevealTileCheckboxID = -1
		self.m_iDiplomacyCheckboxID = -1
		self.m_iLandmarkCheckboxID = -1
		self.m_iEraseCheckboxID = -1

		self.m_iNewCivilization = -1
		self.m_iNewLeaderType = -1
		self.m_iImprovement = 0
		self.m_iYield = 0
		self.m_iDomain = 0
		self.m_iRoute = 0
		self.m_iMemory = 0
		self.m_pScript = -1
		self.m_bMoveUnit = False
		self.m_iUnitCombat = -2
		self.m_iUnitType = -1
		self.m_iPlotMode = 0
		self.m_iArea = -1
		self.m_iCityTimer = 0
		self.m_iGlobalDefine = 0
		self.m_iDiplomacyPage = 0
		self.m_bTowardsPlayer = True
		self.m_iOtherPlayer = -1
		self.m_iEventUnit = -1
		self.m_iTechShareCivs = 1
		self.m_iBuildingType = 0
		self.m_iBuildingModifier = 0
		self.m_iRevealMode = 2
		self.m_iBrushSize = 1

		screen = self.getScreen()
		screen.setCloseOnEscape(False)
		screen.setAlwaysShown(True)
		screen.setDimensions(0, 0, xRes, yRes)
		eWidGen = WidgetTypes.WIDGET_GENERAL

		# make side menu
		if CyInterface().isInAdvancedStart():
			x = 0
		else:
			x = xRes - 228

		screen.addPanel("WorldBuilderBackgroundPanel", "", "", True, True, x, 0, 228, 84, PanelStyles.PANEL_STYLE_MAIN)
		screen.addScrollPanel("WorldBuilderMainPanel", "", x, 0, 228, 84, PanelStyles.PANEL_STYLE_MAIN)

		if CyInterface().isInAdvancedStart():
			y = 15
			szText = u"<font=4>" + TRNSLTR.getText("TXT_KEY_WB_AS_POINTS", (GC.getPlayer(CyGame().getActivePlayer()).getAdvancedStartPoints(), )) + "</font>"
			screen.setLabel("AdvancedStartPointsText", "", szText, 1<<0, 50, y, -2, FontTypes.GAME_FONT, eWidGen, 1, 1)

			y += 30
			szText = TRNSLTR.getText("TXT_KEY_ADVANCED_START_BEGIN_GAME", ())
			screen.setButtonGFC("WorldBuilderExitButton0", szText, "", 50, y, 130, 28, WidgetTypes.WIDGET_WB_EXIT_BUTTON, 1, 1, ButtonStyles.BUTTON_STYLE_STANDARD)

			szText = u"<font=4>" + TRNSLTR.getText("TXT_KEY_WB_AS_COST_THIS_LOCATION", (self.m_iCost, )) + u"</font>"
			y = 85
			screen.setLabel("AdvancedStartCostText", "", szText, 1<<0, 30, y, 0, FontTypes.GAME_FONT, eWidGen, -1, -1 )

		else:
			screen.attachPanelAt("WorldBuilderMainPanel", "WorldBuilderLoadSavePanel", "", "", False, True, PanelStyles.PANEL_STYLE_CITY_TANSHADE, 70, 0, 140, 35, eWidGen, 1, 1)
			x = 0
			screen.setImageButtonAt("WorldBuilderGameDataButton", "WorldBuilderLoadSavePanel", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_CHANGE_ALL_PLOTS").getPath(), x, 0, 32, 32, eWidGen, 1, 1)
			x += 35
			screen.setImageButtonAt("WorldBuilderSaveButton", "WorldBuilderLoadSavePanel", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_SAVE").getPath(), x, 0, 32, 32, WidgetTypes.WIDGET_WB_SAVE_BUTTON, -1, -1)
			x += 35
			screen.setImageButtonAt("WorldBuilderLoadButton", "WorldBuilderLoadSavePanel", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_LOAD").getPath(), x, 0, 32, 32, WidgetTypes.WIDGET_WB_LOAD_BUTTON, -1, -1)
			x += 35
			screen.setImageButtonAt("WorldBuilderExitButton0", "WorldBuilderLoadSavePanel", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_EXIT").getPath(), x, 0, 32, 32, eWidGen, 1, 1)

			x = 0
			self.m_iUnitEditCheckboxID = 0
			screen.addCheckBoxGFC("WorldBuilderUnitEditModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_TOGGLE_UNIT_EDIT_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				xRes + x - 220, 46, 32, 32,
				WidgetTypes.WIDGET_WB_UNIT_EDIT_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL
			)
			x += 35
			self.m_iCityEditCheckboxID = 1
			screen.addCheckBoxGFC("WorldBuilderCityEditModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_TOGGLE_CITY_EDIT_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				xRes + x - 220, 46, 32, 32,
				WidgetTypes.WIDGET_WB_CITY_EDIT_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL
			)
			x += 35
			self.m_iNormalPlayerCheckboxID = 2
			screen.addCheckBoxGFC("WorldBuilderNormalPlayerModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_NORMAL_UNIT_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				xRes + x - 220, 46, 32, 32,
				WidgetTypes.WIDGET_WB_NORMAL_PLAYER_TAB_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL
			)
			x += 35
			self.m_iNormalMapCheckboxID = 3
			screen.addCheckBoxGFC("WorldBuilderNormalMapModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_NORMAL_MAP_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				xRes + x - 220, 46, 32, 32,
				WidgetTypes.WIDGET_WB_NORMAL_MAP_TAB_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL
			)
			x += 35
			self.m_iRevealTileCheckboxID = 4
			screen.addCheckBoxGFC("WorldBuilderRevealTileModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_REVEAL_TILE_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				xRes + x - 220, 46, 32, 32,
				WidgetTypes.WIDGET_WB_REVEAL_TAB_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL
			)
			x += 35
			self.m_iDiplomacyCheckboxID = 5
			screen.addCheckBoxGFC("WorldBuilderDiplomacyModeButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_DIPLOMACY_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				xRes + x - 220, 46, 32, 32,
				WidgetTypes.WIDGET_WB_DIPLOMACY_MODE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL
			)
			x = 0
			self.m_iLandmarkCheckboxID = 6
			screen.addCheckBoxGFC("WorldBuilderLandmarkButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_LANDMARK_MODE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				xRes + x - 220, 10, 32, 32,
				WidgetTypes.WIDGET_WB_LANDMARK_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL
			)
			x += 35
			self.m_iEraseCheckboxID = 7
			screen.addCheckBoxGFC("WorldBuilderEraseButton",
				ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_ERASE").getPath(),
				ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(),
				xRes + x - 220, 10, 32, 32,
				WidgetTypes.WIDGET_WB_ERASE_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL
			)
			self.setCurrentModeCheckbox(self.m_iNormalPlayerCheckboxID)

		self.refreshSideMenu()

		#add interface items
		self.refreshPlayerTabCtrl()

		self.refreshAdvancedStartTabCtrl(False)

		if CyInterface().isInAdvancedStart():
			pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
			pPlot = pPlayer.getStartingPlot()
			CyCamera().JustLookAtPlot(pPlot)

		self.m_normalMapTabCtrl = getWBToolNormalMapTabCtrl()

		self.m_normalMapTabCtrl.setNumColumns((GC.getNumBonusInfos()/10)+1);
		self.m_normalMapTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_IMPROVEMENTS",()));
		self.m_iImprovementTabID = 0
		self.m_iNormalMapCurrentIndexes.append(0)

		self.m_iNormalMapCurrentList.append(0)
		self.m_iImprovementListID = 0

		self.m_normalMapTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_BONUSES", ()));
		self.m_iBonusTabID = 1
		self.m_iNormalMapCurrentIndexes.append(0)

		self.m_iNormalMapCurrentList.append(0)
		self.m_iBonusListID = 0

		self.m_normalMapTabCtrl.setNumColumns((GC.getNumTerrainInfos()/10)+1);
		self.m_normalMapTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_TERRAINS",()))
		self.m_iTerrainTabID = 2
		self.m_iNormalMapCurrentIndexes.append(0)

		self.m_iNormalMapCurrentList.append(0)
		self.m_iTerrainListID = 0
		self.m_iPlotTypeListID = 1
		self.m_iFeatureListID = 2
		self.m_iRouteListID = 3

		# Territory

		self.m_normalMapTabCtrl.setNumColumns(8);
		self.m_normalMapTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_TERRITORY",()))
		self.m_iTerritoryTabID = 3
		self.m_iNormalMapCurrentIndexes.append(0)

		self.m_iNormalMapCurrentList.append(0)
		self.m_iTerritoryListID = 0

		# This should be a forced redraw screen
		screen.setForcedRedraw(True)
		# This should show the screen immidiately and pass input to the game
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, True)

		setWBInitialCtrlTabPlacement()


	def killScreen(self):
		print "WB killScreen()"
		if self.m_tabCtrlEdit != 0:
			self.m_tabCtrlEdit.destroy()
			self.m_tabCtrlEdit = 0

		screen = self.getScreen()
		screen.hideScreen()
		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_HIGHLIGHT_PLOT)


	def mouseOverPlot(self, argsList):

		if self.m_bReveal:
			self.m_pCurrentPlot = CyInterface().getMouseOverPlot()
			if CyInterface().isLeftMouseDown() and self.m_bLeftMouseDown:
				self.setMultipleReveal(True)
			elif CyInterface().isRightMouseDown() and self.m_bRightMouseDown:
				self.setMultipleReveal(False)
		else:
			self.m_pCurrentPlot = CyInterface().getMouseOverPlot()
			self.m_iCurrentX = self.m_pCurrentPlot.getX()
			self.m_iCurrentY = self.m_pCurrentPlot.getY()
			if CyInterface().isLeftMouseDown() and self.m_bLeftMouseDown:
				if self.useLargeBrush():
					self.placeMultipleObjects()
				else:
					self.placeObject()
			elif CyInterface().isRightMouseDown() and self.m_bRightMouseDown:
				if not (self.m_bCityEdit or self.m_bUnitEdit):
					if self.useLargeBrush():
						self.removeMultipleObjects()
					else:
						self.removeObject()


	def getHighlightPlot(self, argsList):

		self.refreshASItemCost()

		if self.m_pCurrentPlot and CyInterface().isInAdvancedStart() and self.m_iCost <= 0:
			return []

		if self.m_pCurrentPlot and not self.m_bShowBigBrush and isMouseOverGameSurface():
			return (self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY())

		return []

	def leftMouseDown(self, argsList):
		bShift, bCtrl, bAlt = argsList
		self.m_bLeftMouseDown = True

		if CyInterface().isInAdvancedStart():
			self.placeObject()
			return 1
## Edit Area Map ##
		if self.m_bNormalMap and self.m_iPlotMode == 1:
			self.m_iArea = self.m_pCurrentPlot.getArea()
			self.refreshSideMenu()
			return 1
## Add Units ##
		elif self.m_bNormalPlayer and self.m_iUnitType > -1:
			pNewUnit = GC.getPlayer(self.m_iCurrentPlayer).initUnit(self.m_iUnitType, self.m_iCurrentX, self.m_iCurrentY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			return 1
## Move Unit Start ##
		elif self.m_bMoveUnit:
			pUnit = self.m_pActivePlot.getUnit(self.m_iCurrentUnit)
			pUnit.setXY(self.m_iCurrentX, self.m_iCurrentY, True, True, False)
			self.m_bMoveUnit = False
			self.toggleUnitEditCB()
			return 1
## Move Unit End ##
		elif self.m_bUnitEdit or bAlt and bCtrl:
			if self.m_pCurrentPlot.getNumUnits() > 0:
				self.m_iCurrentUnit = 0
				self.setUnitEditInfo(False)
			return 1
		elif bCtrl or self.m_bCityEdit:
			if self.m_pCurrentPlot.isCity():
				self.initCityEditScreen()
			return 1
		elif self.m_bReveal:
			if self.m_pCurrentPlot != 0:
				self.setMultipleReveal(True)
		elif bShift and not bCtrl and not bAlt:
			self.setPlotEditInfo(True)
			return 1

		if self.useLargeBrush():
			self.placeMultipleObjects()
		else:
			self.placeObject()
		return 1

	def rightMouseDown(self, argsList):
		self.m_bRightMouseDown = True

		if CyInterface().isInAdvancedStart():
			self.removeObject()
			return 1

		if self.m_bCityEdit or self.m_bUnitEdit:
			self.setPlotEditInfo(True)
		elif self.m_bReveal:
			if self.m_pCurrentPlot != 0:
				self.setMultipleReveal(False)
		else:
			if self.useLargeBrush():
				self.removeMultipleObjects()
			else:
				self.removeObject()

		return 1

	# Will update the screen (every 250 MS)
	def updateScreen(self):
		screen = self.getScreen()

		if CyInterface().isInAdvancedStart():
			if (self.m_bSideMenuDirty):
				self.refreshSideMenu()
			if (self.m_bASItemCostDirty):
				self.refreshASItemCost()

		if CyInterface().isDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT) and not CyInterface().isFocusedWidget():
			self.refreshAdvancedStartTabCtrl(True)
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, False)

		if self.useLargeBrush():
			self.m_bShowBigBrush = True
		else:
			self.m_bShowBigBrush = False

		if self.m_bCtrlEditUp:
			if not self.m_bUnitEdit and not self.m_bCityEdit and not self.m_tabCtrlEdit.isEnabled() and not CyInterface().isInAdvancedStart():
				if self.m_bNormalMap:
					self.m_normalMapTabCtrl.enable(True)
				if self.m_bNormalPlayer:
					self.m_normalPlayerTabCtrl.enable(True)
				self.m_bCtrlEditUp = False
				return 0
		if self.m_bNormalMap and self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerrainTabID and self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iRouteListID:
			if self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()] == GC.getNumRouteInfos():
				if self.m_pRiverStartPlot != -1:
					self.setRiverHighlights()
					return 0
		self.highlightBrush()
		return 0

	def resetTechButtons(self):
		for i in xrange(GC.getNumTechInfos()):
			strName = "Tech_%s" %(i,)
			self.m_normalPlayerTabCtrl.setCheckBoxState("Technologies", GC.getTechInfo(i).getDescription(), GC.getTeam(GC.getPlayer(self.m_iCurrentPlayer).getTeam()).isHasTech(i))
		return 1

	def refreshReveal(self):
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		for i in xrange(CyMap().getGridWidth()):
			for j in xrange(CyMap().getGridHeight()):
				pPlot = CyMap().plot(i,j)
				if not pPlot.isNone():
					self.showRevealed(pPlot)
		return 1

	def handleUnitEditExperienceCB(self, argsList):
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setExperience(int(argsList[0]),-1)
		return 1

	def handleUnitEditLevelCB(self, argsList):
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setLevel(int(argsList[0]))
		return 1

	def handleUnitEditNameCB(self, argsList):
		if ((len(argsList[0]) < 1) or (self.m_pActivePlot == 0) or (self.m_iCurrentUnit < 0) or (self.m_pActivePlot.getNumUnits() <= self.m_iCurrentUnit)):
			return 1
		szNewName = argsList[0]
		unit = self.m_pActivePlot.getUnit(self.m_iCurrentUnit)
		if (unit):
			unit.setName(szNewName)
		return 1

## Platy World Builder Start ##
## Common ##
	def handleEnterNewScreenCB(self, argsList):
		strName = argsList[0]
		if strName == "PromotionEditScreen":
			self.setPromotionEditInfo()
		elif strName == "BackToUnit":
			self.setUnitEditInfo(True)
		elif strName == "TeamEditScreen":
			self.setTeamEditInfo()
			self.refreshSideMenu()
		elif strName == "ProjectEditScreen":
			self.setProjectEditInfo()
		elif strName == "TechEditScreen":
			self.setTechnologyEditInfo()
		elif strName == "BuildingEditScreen":
			self.setBuildingEditInfo()
		elif strName == "WonderEditScreen":
			self.setWonderEditInfo()
		elif strName == "ReligionEditScreen":
			self.setReligionEditInfo()
		elif strName == "CorporationEditScreen":
			self.setCorporationEditInfo()
		elif strName == "FreeSpecialistEditScreen":
			self.setFreeSpecialistEditInfo()
		elif strName == "GreatPeopleEditScreen":
			self.setGreatPeopleEditInfo()
		elif strName == "FreeBonusEditScreen":
			self.setFreeBonusEditInfo()
		elif strName == "CityEditScreen":
			self.setCityEditInfo(True)
		elif strName == "GameOptionEditScreen":
			self.setGameOptionEditInfo()
		return 1

	def handleCurrentPlayerEditPullDownCB(self, argsList):
		iIndex, strName = argsList
		iCount = 0
		for i in xrange(GC.getMAX_PLAYERS()):
			if GC.getPlayer(i).isEverAlive():
				if iCount == int(iIndex):
					self.m_iCurrentPlayer = i
					self.m_iCurrentTeam = GC.getPlayer(i).getTeam()
					if strName == "PlayerEditCurrentPlayer":
						self.setPlayerEditInfo()
						self.refreshSideMenu()
					elif strName == "DiplomacyEditCurrentPlayer":
						self.setDiplomacyEditInfo()
					elif strName == "PlotEditCurrentPlayer":
						self.setPlotEditInfo(False)
					elif strName == "UnitEditOwner":
						pUnit = self.m_pActivePlot.getUnit(self.m_iCurrentUnit)
						pNewUnit = GC.getPlayer(i).initUnit(pUnit.getUnitType(), pUnit.getX(), pUnit.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
						# Don't kill unit in convert() because it does it with delayed death which makes it impossible to get rid of the old unit before exiting WB.
						pNewUnit.convert(pUnit, False) # False here means keep original unit.
						pUnit.kill(False, -1) # Now kill it without delayed death (False).
						self.setUnitEditInfo(True)
					elif strName == "CityEditOwner":
						GC.getPlayer(i).acquireCity(self.m_pActivePlot.getPlotCity(), False, False)
						self.setCityEditInfo(True)
					return 1
				iCount += 1

	def handleKillCB(self, argsList):
		strName = argsList[0]
		if strName == "Unit":
			self.m_pActivePlot.getUnit(self.m_iCurrentUnit).kill(False, -1)
			self.toggleUnitEditCB()
			return 1
		elif strName == "Player":
			pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
			(loopCity, iter) = pPlayer.firstCity(False)
			while loopCity:
				loopCity.kill()
				(loopCity, iter) = pPlayer.nextCity(iter, False)
			(loopUnit, iter) = pPlayer.firstUnit(False)
			while loopUnit:
				loopUnit.kill(False, -1)
				(loopUnit, iter) = pPlayer.nextUnit(iter, False)

## Side Panel ##
	def handleWorldBuilderGameDataPullDownCB(self, argsList):
		iIndex = int(argsList)
		if iIndex == 0:
			self.setPlayerEditInfo()
		elif iIndex == 1:
			self.setTeamEditInfo()
		elif iIndex == 2:
			self.setGameEditInfo()
		elif iIndex == 3:
			if CyGame().countCivPlayersEverAlive() != GC.getMAX_PC_PLAYERS():
				self.AddNewPlayer(True)
		return 1

	def handleWorldBuilderAddUnitsPullDownCB(self, argsList):
		iIndex = int(argsList)
		if iIndex <= GC.getNumUnitCombatInfos():
			self.m_iUnitCombat = iIndex -1
			self.m_iUnitType = -1
		else:
			self.m_iUnitCombat = -2
		self.refreshSideMenu()
		return 1

	def handleWorldBuilderUnitCombatPullDownCB(self, argsList):
		iIndex = int(argsList)
		iCount = 0
		for i in xrange(GC.getNumUnitInfos()):
			if GC.getUnitInfo(i).getUnitCombatType() == self.m_iUnitCombat:
				if iCount == iIndex:
					self.m_iUnitType = i
					return 1
				iCount += 1
		self.m_iUnitType = -1
		return 1

	def handleWorldBuilderEditAreaMapPullDownCB(self, argsList):
		self.m_iPlotMode = int(argsList)
		self.m_iArea = -1
		self.refreshSideMenu()
		return 1

	def handleWorldBuilderRevealModePullDownCB(self, argsList):
		self.m_iRevealMode = int(argsList)
		self.refreshReveal()
		return 1

	def handleWorldBuilderModifyAreaPlotTypePullDownCB(self, argsList):
		iIndex = int(argsList)
		if self.m_iPlotMode == 2:
			CyMap().setAllPlotTypes(iIndex)
		else:
			PlotType = [PlotTypes.PLOT_PEAK, PlotTypes.PLOT_HILLS, PlotTypes.PLOT_LAND, PlotTypes.PLOT_OCEAN]
			for i in xrange(CyMap().numPlots()):
				pPlot = CyMap().plotByIndex(i)
				if (not pPlot.isNone()) and pPlot.getArea() == self.m_iArea:
					pPlot.setPlotType(PlotType[iIndex], True, True)
		self.refreshSideMenu()
		return 1

	def handleWorldBuilderModifyAreaTerrainPullDownCB(self, argsList):
		iIndex = int(argsList)
		lTerrain = []
		for i in xrange(GC.getNumTerrainInfos()):
			if GC.getTerrainInfo(i).isGraphicalOnly(): continue
			if self.m_iPlotMode == 2:
				lTerrain.append(i)
			elif CyMap().getArea(self.m_iArea).isWater() and GC.getTerrainInfo(i).isWaterTerrain():
				lTerrain.append(i)
			elif not CyMap().getArea(self.m_iArea).isWater() and not GC.getTerrainInfo(i).isWaterTerrain():
				lTerrain.append(i)
		if iIndex < len(lTerrain):
			for i in xrange(CyMap().numPlots()):
				pPlot = CyMap().plotByIndex(i)
				if pPlot.isNone(): continue
				if self.m_iPlotMode == 2:
					if GC.getTerrainInfo(iIndex).isWaterTerrain() and pPlot.isWater():
						pPlot.setTerrainType(lTerrain[iIndex], True, True)
					elif (not GC.getTerrainInfo(iIndex).isWaterTerrain()) and (not pPlot.isWater()):
						pPlot.setTerrainType(lTerrain[iIndex], True, True)
				elif pPlot.getArea() == self.m_iArea:
					pPlot.setTerrainType(lTerrain[iIndex], True, True)
		self.refreshSideMenu()
		return 1

	def handleWorldBuilderModifyAreaRoutePullDownCB(self, argsList):
		iIndex = int(argsList)
		if iIndex == GC.getNumRouteInfos():
			iIndex = -1
		for i in xrange(CyMap().numPlots()):
			pPlot = CyMap().plotByIndex(i)
			if not pPlot.isNone() and pPlot.getArea() == self.m_iArea:
				if pPlot.isImpassable(): continue
				pPlot.setRouteType(iIndex)
		self.refreshSideMenu()
		return 1

	def handleWorldBuilderModifyAreaFeaturePullDownCB(self, argsList):
		iIndex = int(argsList)
		for i in xrange(CyMap().numPlots()):
			pPlot = CyMap().plotByIndex(i)
			if not pPlot.isNone() and pPlot.getArea() == self.m_iArea:
				if iIndex == 0:
					if pPlot.canHaveFeature(GC.getInfoTypeForString("FEATURE_JUNGLE")):
						pPlot.setFeatureType(GC.getInfoTypeForString("FEATURE_JUNGLE"), 0)
				elif iIndex == 1:
					if pPlot.canHaveFeature(GC.getInfoTypeForString("FEATURE_FOREST")):
						if pPlot.getLatitude() < 27:
							pPlot.setFeatureType(GC.getInfoTypeForString("FEATURE_FOREST"), 0)
						elif pPlot.getLatitude() < 54:
							pPlot.setFeatureType(GC.getInfoTypeForString("FEATURE_FOREST"), 1)
						else:
							pPlot.setFeatureType(GC.getInfoTypeForString("FEATURE_FOREST"), 2)
				elif iIndex == 2:
					pPlot.setFeatureType(-1, 0)
		self.refreshSideMenu()
		return 1
## City Data ##

	def handleChooseCityCB(self, argsList):
		iCount = 0
		(loopCity, iter) = GC.getPlayer(self.m_iCurrentPlayer).firstCity(False)
		while(loopCity):
			if iCount == int(argsList[0]):
				self.m_pActivePlot = loopCity.plot()
				self.setCityEditInfo(True)
				return 1
			iCount += 1
			(loopCity, iter) = GC.getPlayer(self.m_iCurrentPlayer).nextCity(iter, False)

	def handleCityEditPopulationCB(self, argsList):
		self.m_pActivePlot.getPlotCity().setPopulation(int(argsList[0]))
		self.setCityEditInfo(True)
		return 1

	def handleCityEditCultureCB(self, argsList):
		self.m_pActivePlot.getPlotCity().setCulture(self.m_iCurrentPlayer, int(argsList[0]), True)
		self.setCityEditInfo(True)
		return 1

	def handleCityEditCultureLevelCB(self, argsList):
		self.m_pActivePlot.getPlotCity().setCulture(self.m_iCurrentPlayer, GC.getCultureLevelInfo(int(argsList[0])).getSpeedThreshold(CyGame().getGameSpeedType()), True)
		self.setCityEditInfo(True)
		return 1

	def handleCityEditHappinessCB(self, argsList):
		self.m_pActivePlot.getPlotCity().changeExtraHappiness(int(argsList[0]) - self.m_pActivePlot.getPlotCity().happyLevel() + self.m_pActivePlot.getPlotCity().unhappyLevel(0))
		return 1

	def handleCityEditHealthCB(self, argsList):
		self.m_pActivePlot.getPlotCity().changeExtraHealth(int(argsList[0]) - self.m_pActivePlot.getPlotCity().goodHealth() + self.m_pActivePlot.getPlotCity().badHealth(False))
		return 1

	def handleCityEditTimersCB(self, argsList):
		self.m_iCityTimer = int(argsList[0])
		self.setCityEditInfo(True)
		return 1

	def handleCityEditCurrentTimerCB(self, argsList):
		if self.m_iCityTimer == 0:
			self.m_pActivePlot.getPlotCity().setOccupationTimer(int(argsList[0]))
		elif self.m_iCityTimer == 1:
			self.m_pActivePlot.getPlotCity().changeConscriptAngerTimer(int(argsList[0]) - self.m_pActivePlot.getPlotCity().getConscriptAngerTimer())
		elif self.m_iCityTimer == 2:
			self.m_pActivePlot.getPlotCity().changeHurryAngerTimer(int(argsList[0]) - self.m_pActivePlot.getPlotCity().getHurryAngerTimer())
		elif self.m_iCityTimer == 3:
			self.m_pActivePlot.getPlotCity().changeDefyResolutionAngerTimer(int(argsList[0]) - self.m_pActivePlot.getPlotCity().getDefyResolutionAngerTimer())
		elif self.m_iCityTimer == 4:
			self.m_pActivePlot.getPlotCity().changeEspionageHappinessCounter(int(argsList[0]) - self.m_pActivePlot.getPlotCity().getEspionageHappinessCounter())
		elif self.m_iCityTimer == 5:
			self.m_pActivePlot.getPlotCity().changeEspionageHealthCounter(int(argsList[0]) - self.m_pActivePlot.getPlotCity().getEspionageHealthCounter())
		else:
			self.m_pActivePlot.getPlotCity().changeHappinessTimer(int(argsList[0]) - self.m_pActivePlot.getPlotCity().getHappinessTimer())
		self.setCityEditInfo(True)
		return 1

	def handleCityEditDefenseCB(self, argsList):
		iNewDefenseDamage =100 - 100 * int(argsList[0])/self.m_pActivePlot.getPlotCity().getTotalDefense(False)
		self.m_pActivePlot.getPlotCity().changeDefenseDamage(iNewDefenseDamage - self.m_pActivePlot.getPlotCity().getDefenseDamage())
		return 1

	def handleCityEditTradeRouteCB(self, argsList):
		self.m_pActivePlot.getPlotCity().changeExtraTradeRoutes(int(argsList[0]) - self.m_pActivePlot.getPlotCity().getTradeRoutes())
		return 1

	def handleCityEditGPRateCB(self, argsList):
		iBaseGPRate = int(argsList[0]) * 100 / self.m_pActivePlot.getPlotCity().getTotalGreatPeopleRateModifier()
		self.m_pActivePlot.getPlotCity().changeBaseGreatPeopleRate(iBaseGPRate - self.m_pActivePlot.getPlotCity().getBaseGreatPeopleRate())
		self.setCityEditInfo(True)
		return 1

	def handleBuildingCommandsCB(self, argsList):
		iIndex = int(argsList[0])
		if iIndex == 1:
			for i in xrange(GC.getNumBuildingInfos()):
				if isNationalWonder(i) or isWorldWonder(i): continue
				if self.m_pActivePlot.getPlotCity().canConstruct(i, True, True, True):
					self.m_pActivePlot.getPlotCity().setNumRealBuilding(i, 1)
		elif iIndex == 2:
			for i in xrange(GC.getNumBuildingInfos()):
				if isNationalWonder(i) or isWorldWonder(i): continue
				self.m_pActivePlot.getPlotCity().setNumRealBuilding(i, 0)
		elif iIndex == 3:
			for i in xrange(GC.getNumBuildingInfos()):
				if isNationalWonder(i) or isWorldWonder(i): continue
				(loopCity, iter) = GC.getPlayer(self.m_iCurrentPlayer).firstCity(False)
				while(loopCity):
					if self.m_pActivePlot.getPlotCity().isHasBuilding(i):
						loopCity.setNumRealBuilding(i, 1)
					else:
						loopCity.setNumRealBuilding(i, 0)
					(loopCity, iter) = GC.getPlayer(self.m_iCurrentPlayer).nextCity(iter, False)
		self.setBuildingEditInfo()
		return 1

	def handleCityEditReligionCB(self, argsList):
		iIndex, strName = argsList
		self.m_pActivePlot.getPlotCity().setHasReligion(int(strName), int(iIndex), False, False)
		if self.m_pActivePlot.getPlotCity().isHolyCityByType(int(strName)) and not iIndex:
			CyGame().clearHolyCity(int(strName))
		self.setReligionEditInfo()
		return 1

	def handleCityEditHolyCityCB(self, argsList):
		iIndex, strName = argsList
		CyGame().clearHolyCity(int(strName))
		if iIndex:
			CyGame().setHolyCity(int(strName), self.m_pActivePlot.getPlotCity(), False)
		self.setReligionEditInfo()
		return 1

	def handleReligionCommandsCB(self, argsList):
		iIndex = int(argsList[0])
		if iIndex == 1:
			for i in xrange(GC.getNumReligionInfos()):
				self.m_pActivePlot.getPlotCity().setHasReligion(i, True, False, False)
		elif iIndex == 2:
			for i in xrange(GC.getNumReligionInfos()):
				self.m_pActivePlot.getPlotCity().setHasReligion(i, False, False, False)
				if self.m_pActivePlot.getPlotCity().isHolyCityByType(i):
					CyGame().clearHolyCity(i)
		elif iIndex == 3:
			for i in xrange(GC.getNumReligionInfos()):
				(loopCity, iter) = GC.getPlayer(self.m_iCurrentPlayer).firstCity(False)
				while(loopCity):
					if self.m_pActivePlot.getPlotCity().isHasReligion(i):
						loopCity.setHasReligion(i, True, False, False)
					else:
						loopCity.setHasReligion(i, False, False, False)
						if loopCity.isHolyCityByType(i):
							CyGame().clearHolyCity(i)
					(loopCity, iter) = GC.getPlayer(self.m_iCurrentPlayer).nextCity(iter, False)
		self.setReligionEditInfo()
		return 1

	def handleCityEditCorporationCB(self, argsList):
		iIndex, strName = argsList
		self.m_pActivePlot.getPlotCity().setHasCorporation(int(strName), int(iIndex), False, False)
		if self.m_pActivePlot.getPlotCity().isHeadquartersByType(int(strName)) and not iIndex:
			CyGame().clearHeadquarters(int(strName))
		self.setCorporationEditInfo()
		return 1

	def handleCityEditHeadquartersCB(self, argsList):
		iIndex, strName = argsList
		CyGame().clearHeadquarters(int(strName))
		if iIndex:
			CyGame().setHeadquarters(int(strName), self.m_pActivePlot.getPlotCity(), False)
		self.setCorporationEditInfo()
		return 1

	def handleCorporationCommandsCB(self, argsList):
		iIndex = int(argsList[0])
		if iIndex == 1:
			for i in xrange(GC.getNumReligionInfos()):
				self.m_pActivePlot.getPlotCity().setHasCorporation(i, False, False, False)
				if self.m_pActivePlot.getPlotCity().isHeadquartersByType(i):
					CyGame().clearHeadquarters(i)
		elif iIndex == 2:
			for i in xrange(GC.getNumCorporationInfos()):
				(loopCity, iter) = GC.getPlayer(self.m_iCurrentPlayer).firstCity(False)
				while(loopCity):
					if self.m_pActivePlot.getPlotCity().isHasCorporation(i):
						loopCity.setHasCorporation(i, True, False, False)
					else:
						loopCity.setHasCorporation(i, False, False, False)
						if loopCity.isHeadquartersByType(i):
							CyGame().clearHeadquarters(i)
					(loopCity, iter) = GC.getPlayer(self.m_iCurrentPlayer).nextCity(iter, False)
		self.setCorporationEditInfo()
		return 1

	def handleCityEditSpecialistCB(self, argsList):
		iIndex, strName = argsList
		self.m_pActivePlot.getPlotCity().setFreeSpecialistCount(int(strName), int(iIndex))
		return 1

	def handleCityEditGreatPeopleCB(self, argsList):
		iIndex, strName = argsList
		self.m_pActivePlot.getPlotCity().setGreatPeopleUnitProgress(int(strName), int(iIndex))

	def handleCityEditBonusCB(self, argsList):
		iIndex, strName = argsList
		self.m_pActivePlot.getPlotCity().changeFreeBonus(int(strName), int(iIndex) - self.m_pActivePlot.getPlotCity().getFreeBonus(int(strName)))
		return 1

	def handleCityEditBuildingCB(self, argsList):
		self.m_iBuildingType = int(argsList[0])
		self.setCityEditInfo(True)
		return 1

	def handleCityEditModiferCB(self, argsList):
		self.m_iBuildingModifier = int(argsList[0])
		self.setCityEditInfo(True)
		return 1

	def handleCityEditModifyBuildingCB(self, argsList):
		lBuilding = []
		for i in xrange(GC.getNumBuildingInfos()):
			lBuilding.append(GC.getBuildingInfo(i).getDescription() + "_Platy_" + str(i))
		lBuilding.sort()
		iBuilding = int(lBuilding[self.m_iBuildingType][lBuilding[self.m_iBuildingType].find("_Platy_") +7:])
		pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
		if self.m_iBuildingModifier < 3:
			self.m_pActivePlot.getPlotCity().setBuildingYieldChange(iBuilding, self.m_iBuildingModifier, int(argsList[0]))
		else:
			self.m_pActivePlot.getPlotCity().setBuildingCommerceChange(iBuilding, self.m_iBuildingModifier - 3, int(argsList[0]))
		self.setCityEditInfo(True)
		return 1

	def handleCityEditChooseProductionCB(self, argsList):
		iIndex = int(argsList[0])
		pCity = self.m_pActivePlot.getPlotCity()
		if iIndex == 0:
			pCity.pushOrder(OrderTypes.NO_ORDER, -1, -1, False, True, False, True)
			self.setCityEditInfo(True)
			return 1
		iCount = 1
		for iUnit in xrange(GC.getNumUnitInfos()):
			if pCity.canTrain(iUnit, True, False, False, False):
				if iCount == iIndex:
					CvUnitInfo = GC.getUnitInfo(iUnit)
					if GC.getPlayer(self.m_iCurrentPlayer).getUnitCountPlusMaking(iUnit) == CvUnitInfo.getMaxPlayerInstances():
						self.setCityEditInfo(True)
						return 1
					if GC.getTeam(self.m_iCurrentTeam).getUnitCountPlusMaking(iUnit) == CvUnitInfo.getMaxGlobalInstances():
						self.setCityEditInfo(True)
						return 1
					pCity.pushOrder(OrderTypes.ORDER_TRAIN, iUnit , -1, False, True, False, True)
					self.setCityEditInfo(True)
					return 1
				iCount += 1
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			if pCity.canConstruct(iBuilding, True, False, False):
				if iCount == iIndex:
					if GC.getPlayer(self.m_iCurrentPlayer).getBuildingCountPlusMaking(iBuilding) == GC.getBuildingInfo(iBuilding).getMaxPlayerInstances():
						self.setCityEditInfo(True)
						return 1
					if GC.getTeam(self.m_iCurrentTeam).getBuildingCountPlusMaking(iBuilding) == GC.getBuildingInfo(iBuilding).getMaxTeamInstances():
						self.setCityEditInfo(True)
						return 1
					if GC.getTeam(self.m_iCurrentTeam).getBuildingCountPlusMaking(iBuilding) == GC.getBuildingInfo(iBuilding).getMaxGlobalInstances():
						self.setCityEditInfo(True)
						return 1
					pCity.pushOrder(OrderTypes.ORDER_CONSTRUCT, iBuilding , -1, False, True, False, True)
					self.setCityEditInfo(True)
					return 1
				iCount += 1
		for iProject in xrange(GC.getNumProjectInfos()):
			if pCity.canCreate(iProject, True, False):
				if iCount == iIndex:
					if GC.getTeam(self.m_iCurrentTeam).getProjectMaking(iProject) + GC.getTeam(self.m_iCurrentTeam).getProjectCount(iProject) == GC.getProjectInfo(iProject).getMaxTeamInstances():
						self.setCityEditInfo(True)
						return 1
					if GC.getTeam(self.m_iCurrentTeam).getProjectMaking(iProject) + GC.getTeam(self.m_iCurrentTeam).getProjectCount(iProject) == GC.getProjectInfo(iProject).getMaxGlobalInstances():
						self.setCityEditInfo(True)
						return 1
					pCity.pushOrder(OrderTypes.ORDER_CREATE, iProject , -1, False, True, False, True)
					self.setCityEditInfo(True)
					return 1
				iCount += 1
		for iProcess in xrange(GC.getNumProcessInfos()):
			if pCity.canMaintain(iProcess, True):
				if iCount == iIndex:
					pCity.pushOrder(OrderTypes.ORDER_MAINTAIN, iProcess , -1, False, True, False, True)
					self.setCityEditInfo(True)
					return 1
				iCount += 1

	def handleCityEditProductionProgressCB(self, argsList):
		self.m_pActivePlot.getPlotCity().setProduction(int(argsList[0]))
		self.setCityEditInfo(True)
		return 1

	def handleCityEditFoodCB(self, argsList):
		self.m_pActivePlot.getPlotCity().setFood(int(argsList[0]))
		self.setCityEditInfo(True)
		return 1

## Unit Data ##

	def handleUnitEditStrengthCB(self, argsList):
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setBaseCombatStr(int(argsList[0]))
		return 1

	def handleUnitEditDamageCB(self, argsList):
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setDamage(int(argsList[0]), -1)
		return 1

	def handleUnitEditMovesCB(self, argsList):
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setMoves(self.m_pActivePlot.getUnit(self.m_iCurrentUnit).maxMoves() - (int(argsList[0]) * GC.getDefineINT("MOVE_DENOMINATOR")))
		return 1

	def handleUnitEditCargoCB(self, argsList):
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).changeCargoSpace(int(argsList[0]) - self.m_pActivePlot.getUnit(self.m_iCurrentUnit).cargoSpace())
		return 1

	def handleUnitEditImmobileTimerCB(self, argsList):
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setImmobileTimer(int(argsList[0]))
		return 1

	def handleUnitEditPromotionReadyCB(self, argsList):
		iPromotionReady = int(argsList[0])
		if self.m_pActivePlot.getUnit(self.m_iCurrentUnit).experienceNeeded() > self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getExperience():
			iPromotionReady = 0
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setPromotionReady(iPromotionReady)
		self.setUnitEditInfo(True)
		return 1

	def handleUnitEditMadeAttackCB(self, argsList):
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setMadeAttack(int(argsList[0]))
		return 1

	def handleUnitEditMadeInterceptionCB(self, argsList):
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setMadeInterception(int(argsList[0]))
		return 1

	def handleMoveUnitCB(self, argsList):
		self.m_bMoveUnit = True
		self.toggleUnitEditCB()
		return 1

	def handleEditUnitPromotionCB(self, argsList):
		iIndex, strName = argsList
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setHasPromotion(int(strName), int(iIndex))
		return 1

	def handlePromotionCommandsCB(self, argsList):
		iIndex = int(argsList[0])
		if iIndex == 1:
			for iPromotion in xrange(GC.getNumPromotionInfos()):
				pUnit = self.m_pActivePlot.getUnit(self.m_iCurrentUnit)
				if pUnit.canAcquirePromotion(iPromotion):
					if GC.getInfoTypeForString("PROMOTION_LEADER") > -1 and iPromotion == GC.getInfoTypeForString("PROMOTION_LEADER"): continue
					pUnit.setHasPromotion(iPromotion, True)
		elif iIndex == 2:
			for iPromotion in xrange(GC.getNumPromotionInfos()):
				self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setHasPromotion(iPromotion, False)
		self.setPromotionEditInfo()
		return 1

	def handleUnitEditDirectionCB(self, argsList):
		iIndex = int(argsList[0])
		iDirection = self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getFacingDirection()
		if iDirection > iIndex:
			for i in xrange(iDirection - iIndex):
				self.m_pActivePlot.getUnit(self.m_iCurrentUnit).rotateFacingDirectionCounterClockwise()
		else:
			for i in xrange(iIndex - iDirection):
				self.m_pActivePlot.getUnit(self.m_iCurrentUnit).rotateFacingDirectionClockwise()
		return 1

	def handleUnitEditDuplicateCB(self, argsList):
		pUnit = self.m_pActivePlot.getUnit(self.m_iCurrentUnit)
		pNewUnit = GC.getPlayer(self.m_iCurrentPlayer).initUnit(pUnit.getUnitType(), pUnit.getX(), pUnit.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
		pNewUnit.convert(pUnit, False)
		pNewUnit.setFortifyTurns(pUnit.getFortifyTurns())
		pNewUnit.setScriptData(pUnit.getScriptData())
		self.setUnitEditInfo(True)
		return 1

## Player Data ##

	def handleCurrentEraEditPullDownCB(self, argsList):
		GC.getPlayer(self.m_iCurrentPlayer).setCurrentEra(int(argsList[0]))
		return 1

	def handleTeamEditCommerceFlexibleCB(self, argsList):
		iIndex, strName = argsList
		if int(iIndex):
			GC.getTeam(self.m_iCurrentTeam).changeCommerceFlexibleCount(int(strName),  1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeCommerceFlexibleCount(int(strName),  - GC.getTeam(self.m_iCurrentTeam).getCommerceFlexibleCount(int(strName)))
		self.setPlayerEditInfo()
		return 1

	def handlePlayerEditCommercePercentCB(self, argsList):
		iIndex, strName = argsList
		CommerceType = [CommerceTypes.COMMERCE_GOLD, CommerceTypes.COMMERCE_RESEARCH, CommerceTypes.COMMERCE_CULTURE, CommerceTypes.COMMERCE_ESPIONAGE]
		GC.getPlayer(self.m_iCurrentPlayer).setCommercePercent(CommerceType[int(strName)], int(iIndex))
		self.setPlayerEditInfo()
		return 1

	def handlePlayerEditGoldenAgeCB(self, argsList):
		GC.getPlayer(self.m_iCurrentPlayer).changeGoldenAgeTurns(int(argsList[0]) - GC.getPlayer(self.m_iCurrentPlayer).getGoldenAgeTurns())
		self.setPlayerEditInfo()
		return 1

	def handlePlayerEditGoldenAgeUnitsCB(self, argsList):
		GC.getPlayer(self.m_iCurrentPlayer).changeNumUnitGoldenAges(int(argsList[0]) - GC.getPlayer(self.m_iCurrentPlayer).unitsRequiredForGoldenAge())
		return 1

	def handlePlayerEditAnarchyCB(self, argsList):
		GC.getPlayer(self.m_iCurrentPlayer).changeAnarchyTurns(int(argsList[0]) - GC.getPlayer(self.m_iCurrentPlayer).getAnarchyTurns())
		self.setPlayerEditInfo()
		return 1

	def handlePlayerEditCombatExperienceCB(self, argsList):
		GC.getPlayer(self.m_iCurrentPlayer).setCombatExperience(int(argsList[0]))
		self.setPlayerEditInfo()
		return 1

	def handlePlayerEditCivicCB(self, argsList):
		iIndex, strName = argsList
		iCount = 0
		for i in xrange(GC.getNumCivicInfos()):
			if GC.getCivicInfo(i).getCivicOptionType() == int(strName):
				if int(iIndex) == iCount:
					if GC.getPlayer(self.m_iCurrentPlayer).canDoCivics(i):
						GC.getPlayer(self.m_iCurrentPlayer).setCivics(int(strName), i)
					self.setPlayerEditInfo()
					return 1
				iCount += 1

	def handleStateReligionEditPullDownCB(self, argsList):
		GC.getPlayer(self.m_iCurrentPlayer).setLastStateReligion(int(argsList[0]) - 1)
		self.setPlayerEditInfo()
		return 1

	def handlePlayerEditStateReligionUnitProductionCB(self, argsList):
		GC.getPlayer(self.m_iCurrentPlayer).changeStateReligionUnitProductionModifier(int(argsList[0]) - GC.getPlayer(self.m_iCurrentPlayer).getStateReligionUnitProductionModifier())
		return 1

	def handlePlayerEditStateReligionBuildingProductionCB(self, argsList):
		GC.getPlayer(self.m_iCurrentPlayer).changeStateReligionBuildingProductionModifier(int(argsList[0]) - GC.getPlayer(self.m_iCurrentPlayer).getStateReligionBuildingProductionModifier())
		return 1

	def handleCurrentTechEditPullDownCB(self, argsList):
		iCount = 0
		GC.getPlayer(self.m_iCurrentPlayer).clearResearchQueue()
		for i in xrange(GC.getNumTechInfos()):
			if GC.getPlayer(self.m_iCurrentPlayer).canResearch(i):
				iCount += 1
				if iCount == int(argsList[0]):
					GC.getPlayer(self.m_iCurrentPlayer).pushResearch(i, True)
					break
		self.setPlayerEditInfo()
		return 1

	def handleTeamEditResearchProgressCB(self, argsList):
		GC.getTeam(self.m_iCurrentTeam).setResearchProgress(GC.getPlayer(self.m_iCurrentPlayer).getCurrentResearch(), int(argsList[0]), self.m_iCurrentPlayer)
		self.setPlayerEditInfo()
		return 1

	def handlePlayerEditGoldCB(self, argsList):
		GC.getPlayer(self.m_iCurrentPlayer).setGold(int(argsList[0]))
		return 1

## Team Data ##

	def handleTeamEditPullDownCB(self, argsList):
		lTeam = []
		for i in xrange(GC.getMAX_TEAMS()):
			if GC.getTeam(i).isAlive():
				lTeam.append(i)
		self.m_iCurrentTeam = lTeam[int(argsList[0])]
		self.m_iCurrentPlayer = GC.getTeam(self.m_iCurrentTeam).getLeaderID()
		self.setTeamEditInfo()
		self.refreshSideMenu()
		return 1

	def handleAddTeamCB(self, argsList):
		iCount = 1
		for i in xrange(GC.getMAX_TEAMS()):
			if GC.getTeam(i).isAlive():
				if i == self.m_iCurrentTeam: continue
				if int(argsList[0]) == iCount:
					GC.getTeam(self.m_iCurrentTeam).addTeam(i)
					self.setTeamEditInfo()
					return 1
				iCount += 1

	def handleEditTeamProjectCB(self, argsList):
		iIndex, strName = argsList
		GC.getTeam(self.m_iCurrentTeam).changeProjectCount(int(strName), int(iIndex) - GC.getTeam(self.m_iCurrentTeam).getProjectCount(int(strName)))
		if GC.getProjectInfo(int(strName)).isAllowsNukes():
			if CyGame().getProjectCreatedCount(int(strName)) == 0:
				CyGame().makeNukesValid(False)
		return 1

	def handleEditTeamTechnologyCB(self, argsList):
		iIndex, strName = argsList
		GC.getTeam(self.m_iCurrentTeam).setHasTech(int(strName), int(iIndex), self.m_iCurrentPlayer, False, False)
		self.refreshPlayerTabCtrl()
		self.setTechnologyEditInfo()
		return 1

	def handleTeamEditEnemyWarWearinessCB(self, argsList):
		GC.getTeam(self.m_iCurrentTeam).changeEnemyWarWearinessModifier(int(argsList[0]) - GC.getTeam(self.m_iCurrentTeam).getEnemyWarWearinessModifier())
		return 1

	def handleTeamEditNukeInterceptionCB(self, argsList):
		GC.getTeam(self.m_iCurrentTeam).changeNukeInterception(int(argsList[0]) - GC.getTeam(self.m_iCurrentTeam).getNukeInterception())
		return 1

	def handleDomainEditPullDownCB(self, argsList):
		self.m_iDomain = int(argsList[0])
		self.setTeamEditInfo()
		return 1

	def handleTeamEditDomainMovesCB(self, argsList):
		GC.getTeam(self.m_iCurrentTeam).changeExtraMoves(self.m_iDomain, int(argsList[0]) - GC.getTeam(self.m_iCurrentTeam).getExtraMoves(self.m_iDomain))
		return 1

	def handleRouteEditPullDownCB(self, argsList):
		self.m_iRoute = int(argsList[0])
		self.setTeamEditInfo()
		return 1

	def handleTeamEditRouteChangeCB(self, argsList):
		GC.getTeam(self.m_iCurrentTeam).changeRouteChange(self.m_iRoute, int(argsList[0]) - GC.getTeam(self.m_iCurrentTeam).getRouteChange(self.m_iRoute))
		return 1

	def handleImprovementEditPullDownCB(self, argsList):
		self.m_iImprovement = int(argsList[0])
		self.setTeamEditInfo()
		return 1

	def handleYieldEditPullDownCB(self, argsList):
		self.m_iYield = int(argsList[0])
		self.setTeamEditInfo()
		return 1

	def handleTeamEditImprovementYieldCB(self, argsList):
		iCount = 0
		for i in xrange(GC.getNumImprovementInfos()):
			if GC.getImprovementInfo(i).isGraphicalOnly(): continue
			if iCount == self.m_iImprovement:
				GC.getTeam(self.m_iCurrentTeam).changeImprovementYieldChange(i, self.m_iYield, int(argsList[0]) - GC.getTeam(self.m_iCurrentTeam).getImprovementYieldChange(i, self.m_iYield))
				return 1
			iCount += 1

	def handleTeamEditMapCenteringCB(self, argsList):
		GC.getTeam(self.m_iCurrentTeam).setMapCentering(int(argsList[0]))
		return 1

	def handleTeamEditGoldTradingCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeGoldTradingCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeGoldTradingCount( -GC.getTeam(self.m_iCurrentTeam).getGoldTradingCount())
		return 1

	def handleTeamEditTechTradingCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeTechTradingCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeTechTradingCount( -GC.getTeam(self.m_iCurrentTeam).getTechTradingCount())
		return 1

	def handleTeamEditMapTradingCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeMapTradingCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeMapTradingCount( -GC.getTeam(self.m_iCurrentTeam).getMapTradingCount())
		return 1

	def handleTeamEditOpenBordersTradingCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeOpenBordersTradingCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeOpenBordersTradingCount( -GC.getTeam(self.m_iCurrentTeam).getOpenBordersTradingCount())
		return 1

	def handleTeamEditPermanentAllianceTradingCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changePermanentAllianceTradingCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changePermanentAllianceTradingCount( -GC.getTeam(self.m_iCurrentTeam).getPermanentAllianceTradingCount())
		return 1

	def handleTeamEditDefensivePactTradingCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeDefensivePactTradingCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeDefensivePactTradingCount( -GC.getTeam(self.m_iCurrentTeam).getDefensivePactTradingCount())
		return 1

	def handleTeamEditVassalTradingCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeVassalTradingCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeVassalTradingCount( -GC.getTeam(self.m_iCurrentTeam).getVassalTradingCount())
		return 1

	def handleTeamEditWaterWorkCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeWaterWorkCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeWaterWorkCount( -GC.getTeam(self.m_iCurrentTeam).getWaterWorkCount())
		return 1

	def handleTeamEditExtraWaterSeeFromCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeExtraWaterSeeFromCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeExtraWaterSeeFromCount( -GC.getTeam(self.m_iCurrentTeam).getExtraWaterSeeFromCount())
		return 1

	def handleTeamEditBridgeBuildingCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeBridgeBuildingCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeBridgeBuildingCount( -GC.getTeam(self.m_iCurrentTeam).getBridgeBuildingCount())
		return 1

	def handleTeamEditIrrigationCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeIrrigationCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeIrrigationCount( -GC.getTeam(self.m_iCurrentTeam).getIrrigationCount())
		return 1

	def handleTeamEditIgnoreIrrigationCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeIgnoreIrrigationCount(1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeIgnoreIrrigationCount( -GC.getTeam(self.m_iCurrentTeam).getIgnoreIrrigationCount())
		return 1


	def handleTeamEditKnownCivsCB(self, argsList):
		self.m_iTechShareCivs = int(argsList[0])
		self.setTeamEditInfo()
		return 1

	def handleTeamEditTechShareCB(self, argsList):
		if int(argsList[0]):
			GC.getTeam(self.m_iCurrentTeam).changeTechShareCount(self.m_iTechShareCivs -1, 1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeTechShareCount(self.m_iTechShareCivs -1, - GC.getTeam(self.m_iCurrentTeam).getTechShareCount(self.m_iTechShareCivs -1))
		return 1

	def handleTeamEditForceTeamVoteCB(self, argsList):
		iIndex, strName = argsList
		if int(iIndex):
			GC.getTeam(self.m_iCurrentTeam).changeForceTeamVoteEligibilityCount(int(strName), 1)
		else:
			GC.getTeam(self.m_iCurrentTeam).changeForceTeamVoteEligibilityCount(int(strName), - GC.getTeam(self.m_iCurrentTeam).getForceTeamVoteEligibilityCount(int(strName)))
		return 1

## Game Options ##

	def handleEditGlobalDefineCB(self, argsList):
		self.m_iGlobalDefine = int(argsList[0])
		self.setGameEditInfo()
		return 1

	def handleGlobalDefineINTCB(self, argsList):
		iIndex, strName = argsList
		GC.setDefineINT(strName, int(iIndex))
		return 1

	def handleGameEditStartYearCB(self, argsList):
		CyGame().setStartYear(int(argsList[0]))
		return 1

	def handleEditGameOptionCB(self, argsList):
		iIndex, strName = argsList
		CyGame().setOption(int(strName), int(iIndex))
		if int(strName) == GameOptionTypes.GAMEOPTION_NO_GOODY_HUTS and iIndex:
			for i in xrange(CyMap().numPlots()):
				pPlot = CyMap().plotByIndex(i)
				if pPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_GOODY_HUT"):
					pPlot.setImprovementType(-1)
		elif int(strName) == GameOptionTypes.GAMEOPTION_NO_VASSAL_STATES and iIndex:
			for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
				pTeamX = GC.getTeam(iTeamX)
				for iTeamY in xrange(GC.getMAX_PC_TEAMS()):
					pTeamX.freeVassal(iTeamY)
		elif int(strName) == GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE and iIndex:
			for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
				pPlayerX = GC.getPlayer(iPlayerX)
				if pPlayerX.isHuman():
					(loopCity, iter) = pPlayerX.firstCity(False)
					while(loopCity):
						if not loopCity.isCapital():
							loopCity.kill()
						(loopCity, iter) = pPlayerX.nextCity(iter, False)
		elif int(strName) == GameOptionTypes.GAMEOPTION_NO_BARBARIANS and iIndex:
			pPlayerBarb = GC.getPlayer(GC.getBARBARIAN_PLAYER ())
			(loopCity, iter) = pPlayerBarb.firstCity(False)
			while(loopCity):
				loopCity.kill()
				(loopCity, iter) = pPlayerBarb.nextCity(iter, False)
			(loopUnit, iter) = pPlayerBarb.firstUnit(False)
			while(loopUnit):
				loopUnit.kill(False, -1)
				(loopUnit, iter) = pPlayerBarb.nextUnit(iter, False)
		return 1

	def handleAddPlayerCivilizationCB(self, argsList):
		iIndex = int(argsList[0])
		if int(iIndex) == 0:
			self.m_iNewCivilization = -1
			self.AddNewPlayer(False)
			return 1
		iCount = 1
		for i in xrange(GC.getNumCivilizationInfos()):
			if not CyGame().isCivEverActive(i):
				if iCount == iIndex:
					self.m_iNewCivilization = i
					self.m_iNewLeaderType = -1
					self.AddNewPlayer(False)
					return 1
				iCount+= 1


	def handleAddPlayerLeaderTypeCB(self, argsList):
		iIndex = int(argsList[0])
		if int(iIndex) == 0:
			self.m_iNewLeaderType = -1
			self.AddNewPlayer(False)
			return 1
		iCount = 1
		for i in xrange(GC.getNumLeaderHeadInfos()):
			if not CyGame().isLeaderEverActive(i):
				if not CyGame().isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV):
					if not GC.getCivilizationInfo(self.m_iNewCivilization).isLeaders(i): continue
				if iCount == iIndex:
					self.m_iNewLeaderType = i
					self.AddNewPlayer(False)
					return 1
				iCount+= 1

	def handleCreatePlayerCB(self, argsList):
		for i in xrange(GC.getMAX_PC_PLAYERS()):
			if not GC.getPlayer(i).isEverAlive():
				CyGame().addPlayer(i, self.m_iNewLeaderType, self.m_iNewCivilization, True)
				self.m_iCurrentPlayer = i
				self.m_iCurrentTeam = GC.getPlayer(i).getTeam()
				self.refreshSideMenu()
				self.normalPlayerTabModeCB()
				return 1

## Plot Data ##

	def handlePlotEditCultureCB(self, argsList):
		self.m_pActivePlot.setCulture(self.m_iCurrentPlayer, int(argsList[0]), True)
		self.setPlotEditInfo(False)
		return 1

	def handlePlotEditYieldCB(self, argsList):
		iIndex, strName = argsList
		if strName == "PlotEditFood":
			CyGame().setPlotExtraYield(self.m_pActivePlot.getX(), self.m_pActivePlot.getY(), YieldTypes.YIELD_FOOD, int(iIndex) - self.m_pActivePlot.getYield(YieldTypes.YIELD_FOOD))
		elif strName == "PlotEditProduction":
			CyGame().setPlotExtraYield(self.m_pActivePlot.getX(), self.m_pActivePlot.getY(), YieldTypes.YIELD_PRODUCTION, int(iIndex) - self.m_pActivePlot.getYield(YieldTypes.YIELD_PRODUCTION))
		else:
			CyGame().setPlotExtraYield(self.m_pActivePlot.getX(), self.m_pActivePlot.getY(), YieldTypes.YIELD_COMMERCE, int(iIndex) - self.m_pActivePlot.getYield(YieldTypes.YIELD_COMMERCE))
		return 1

	def handlePlotAddCityCB(self, argsList):
		GC.getPlayer(self.m_iCurrentPlayer).initCity(self.m_pActivePlot.getX(), self.m_pActivePlot.getY())
		self.setPlotEditInfo(False)
		return 1

	def handlePlotEditPlotTypeCB(self, argsList):
		iIndex = int(argsList[0])
		PlotType = [PlotTypes.PLOT_PEAK, PlotTypes.PLOT_HILLS, PlotTypes.PLOT_LAND, PlotTypes.PLOT_OCEAN]
		self.m_pActivePlot.setPlotType(PlotType[iIndex], True, True)
		self.setPlotEditInfo(False)
		return 1

	def handlePlotEditTerrainCB(self, argsList):
		iIndex = int(argsList[0])
		iCount = 0
		for i in xrange(GC.getNumTerrainInfos()):
			if GC.getTerrainInfo(i).isGraphicalOnly(): continue
			if iCount == iIndex:
				self.m_pActivePlot.setTerrainType(i, True, True)
				self.setPlotEditInfo(False)
				return 1
			iCount += 1

	def handlePlotEditFeatureCB(self, argsList):
		iIndex = int(argsList[0]) -1
		self.m_pActivePlot.setFeatureType(iIndex, 0)
		self.setPlotEditInfo(False)
		return 1

	def handlePlotEditVarietyCB(self, argsList):
		self.m_pActivePlot.setFeatureType(self.m_pActivePlot.getFeatureType(), int(argsList[0]))
		return 1

	def handlePlotEditBonusCB(self, argsList):
		self.m_pActivePlot.setBonusType(int(argsList[0]) - 1)
		self.setPlotEditInfo(False)
		return 1

	def handlePlotEditImprovementCB(self, argsList):
		iIndex = int(argsList[0])
		if iIndex == 0:
			self.m_pActivePlot.setImprovementType(-1)
			self.setPlotEditInfo(False)
			return 1
		iCount = 1
		for i in xrange(GC.getNumImprovementInfos()):
			if GC.getImprovementInfo(i).isGraphicalOnly(): continue
			if iIndex == iCount:
				self.m_pActivePlot.setImprovementType(i)
				self.setPlotEditInfo(False)
				return 1
			iCount += 1

	def handlePlotEditUpgradeProgressCB(self, argsList):
		iUpgradeTime = GC.getImprovementInfo(self.m_pActivePlot.getImprovementType()).getUpgradeTime()
		self.m_pActivePlot.setUpgradeProgress(iUpgradeTime - int(argsList[0]))
		return 1

	def handlePlotEditRouteCB(self, argsList):
		self.m_pActivePlot.setRouteType(int(argsList[0]) - 1)
		self.setPlotEditInfo(False)
		return 1

	def handlePlotEditRiverCB(self, argsList):
		iIndex, strName = argsList
		if strName == "NS":
			if iIndex == 0:
				self.m_pActivePlot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			elif iIndex == 1:
				self.m_pActivePlot.setWOfRiver(True, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			elif iIndex == 2:
				self.m_pActivePlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
			elif iIndex == 3:
				self.m_pActivePlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
		else:
			if iIndex == 0:
				self.m_pActivePlot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			elif iIndex == 1:
				self.m_pActivePlot.setWOfRiver(True, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			elif iIndex == 2:
				self.m_pActivePlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
			elif iIndex == 3:
				self.m_pActivePlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)
		return 1

	def handleEventOtherPlayerCB(self, argsList):
		if int(argsList[0]) == 0:
			self.m_iOtherPlayer = -1
			self.setPlotEditInfo(False)
			return 1
		iCount = 1
		for i in xrange(GC.getMAX_PC_PLAYERS()):
			if GC.getPlayer(i).isAlive():
				if i == self.m_iCurrentPlayer: continue
				if iCount == int(argsList[0]):
					self.m_iOtherPlayer = i
					self.setPlotEditInfo(False)
					return 1
				iCount = iCount + 1


	def handleEventUnitCB(self, argsList):
		self.m_iEventUnit = int(argsList[0]) -1
		self.setPlotEditInfo(False)
		return 1

	def handleTriggerEventCB(self, argsList):
		iCityID = -1
		pCity = CyMap().findCity(self.m_pActivePlot.getX(), self.m_pActivePlot.getY(), self.m_iCurrentPlayer, -1, False, False, -1, -1, CyCity())
		if not pCity.isNone():
			iCityID = pCity.getID()
		pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
		iUnit = -1
		if self.m_iEventUnit > -1:
			iUnit = self.m_pActivePlot.getUnit(self.m_iEventUnit).getID()
		triggerData = pPlayer.initTriggeredData(int(argsList[0]) -1, True, iCityID, self.m_pActivePlot.getX(), self.m_pActivePlot.getY(), self.m_iOtherPlayer, -1, pPlayer.getStateReligion(), -1, iUnit, -1)
		self.setPlotEditInfo(False)

## Diplomacy Data ##

	def handleDiplomacyPageCB(self, argsList):
		self.m_iDiplomacyPage = int(argsList[0])
		self.setDiplomacyEditInfo()
		return 1

	def handleTowardsPlayerCB(self, argsList):
		self.m_bTowardsPlayer = int(argsList[0])
		self.setDiplomacyEditInfo()
		return 1

	def handleTowardsOthersCB(self, argsList):
		self.m_bTowardsPlayer = not(int(argsList[0]))
		self.setDiplomacyEditInfo()
		return 1

	def handleTeamEditMetStatusCB(self, argsList):
		iIndex, strName = argsList
		if int(iIndex):
			GC.getTeam(self.m_iCurrentTeam).meet(int(strName), False)
		self.setDiplomacyEditInfo()
		return 1

	def handleTeamEditWarStatusCB(self, argsList):
		iIndex, strName = argsList
		if int(iIndex):
			GC.getTeam(self.m_iCurrentTeam).declareWar(int(strName), True, -1)
		else:
			GC.getTeam(self.m_iCurrentTeam).makePeace(int(strName))
		self.setDiplomacyEditInfo()
		return 1

	def handleTeamEditRelationshipCB(self, argsList):
		iIndex, strName = argsList
		iTeam1 = int(strName)
		iTeam2 = self.m_iCurrentTeam
		GC.getTeam(iTeam2).freeVassal(iTeam1)
		GC.getTeam(iTeam1).freeVassal(iTeam2)
		if int(iIndex) == 0:
			GC.getTeam(iTeam2).assignVassal(iTeam1, False)
		elif int(iIndex) == 1:
			GC.getTeam(iTeam2).assignVassal(iTeam1, True)
		elif int(iIndex) == 2:
			GC.getTeam(iTeam1).assignVassal(iTeam2, True)
		self.setDiplomacyEditInfo()

	def handleMemoryEditPullDownCB(self, argsList):
		self.m_iMemory = int(argsList[0])
		self.setDiplomacyEditInfo()
		return 1

	def handlePlayerEditMemoryCB(self, argsList):
		iIndex, strName = argsList
		GC.getPlayer(int(strName)).AI_changeMemoryCount(self.m_iCurrentPlayer, self.m_iMemory, (int(iIndex) - GC.getPlayer(int(strName)).AI_getMemoryCount(self.m_iCurrentPlayer, self.m_iMemory)))
		self.setDiplomacyEditInfo()
		return 1

	def handleAttitudeEditPullDownCB(self, argsList):
		iIndex, strName = argsList
		while int(iIndex) != GC.getPlayer(int(strName)).AI_getAttitude(self.m_iCurrentPlayer):
			if int(iIndex) < GC.getPlayer(int(strName)).AI_getAttitude(self.m_iCurrentPlayer):
				GC.getPlayer(int(strName)).AI_changeAttitudeExtra(self.m_iCurrentPlayer, -1)
			else:
				GC.getPlayer(int(strName)).AI_changeAttitudeExtra(self.m_iCurrentPlayer, 1)
		self.setDiplomacyEditInfo()
		return 1

	def handleTeamEditEspionagePointsCB(self, argsList):
		iIndex, strName = argsList
		if self.m_bTowardsPlayer:
			iTeam1 = int(strName)
			iTeam2 = self.m_iCurrentTeam
		else:
			iTeam2 = int(strName)
			iTeam1 = self.m_iCurrentTeam
		GC.getTeam(iTeam1).setEspionagePointsAgainstTeam(iTeam2, int(iIndex))
		return 1

	def handlePlayerEditEspionageWeightCB(self, argsList):
		iIndex, strName = argsList
		if self.m_bTowardsPlayer:
			iPlayer1 = int(strName)
			iTeam2 = self.m_iCurrentPlayer
		else:
			iTeam2 = int(strName)
			iPlayer1 = self.m_iCurrentPlayer
		GC.getPlayer(iPlayer1).setEspionageSpendingWeightAgainstTeam(iTeam2, int(iIndex))
		return 1

	def handleTeamEditCounterEspionageCB(self, argsList):
		iIndex, strName = argsList
		if self.m_bTowardsPlayer:
			iTeam1 = int(strName)
			iTeam2 = self.m_iCurrentTeam
		else:
			iTeam2 = int(strName)
			iTeam1 = self.m_iCurrentTeam
		GC.getTeam(iTeam1).setCounterespionageTurnsLeftAgainstTeam(iTeam2, int(iIndex))
		return 1

	def handleTeamEditCounterEspionageModCB(self, argsList):
		iIndex, strName = argsList
		if self.m_bTowardsPlayer:
			iTeam1 = int(strName)
			iTeam2 = self.m_iCurrentTeam
		else:
			iTeam2 = int(strName)
			iTeam1 = self.m_iCurrentTeam
		GC.getTeam(iTeam1).setCounterespionageModAgainstTeam(iTeam2, int(iIndex))
		return 1

	def handleTeamEditWarWearinessCB(self, argsList):
		iIndex, strName = argsList
		if self.m_bTowardsPlayer:
			iTeam1 = int(strName)
			iTeam2 = self.m_iCurrentTeam
		else:
			iTeam2 = int(strName)
			iTeam1 = self.m_iCurrentTeam
		GC.getTeam(iTeam1).setWarWeariness(iTeam2, int(iIndex))
		return 1

	def handleTeamEditSignOpenBordersCB(self, argsList):
		iIndex, strName = argsList
		if int(iIndex):
			GC.getTeam(int(strName)).signOpenBorders(self.m_iCurrentTeam)
			self.setDiplomacyEditInfo()
			return 1
		else:
			for i in xrange(CyGame().getIndexAfterLastDeal()):
				pDeal = CyGame().getDeal(i)
				iPlayer1 = pDeal.getFirstPlayer()
				iPlayer2 = pDeal.getSecondPlayer()
				if iPlayer1 == -1 or iPlayer2 == -1: continue
				iTeam1 = GC.getPlayer(pDeal.getFirstPlayer()).getTeam()
				iTeam2 = GC.getPlayer(pDeal.getSecondPlayer()).getTeam()
				if (iTeam1 == int(strName) and iTeam2 == self.m_iCurrentTeam) or (iTeam2 == int(strName) and iTeam1 == self.m_iCurrentTeam):
					for j in xrange(pDeal.getLengthFirstTrades()):
						if pDeal.getFirstTrade(j).ItemType == TradeableItems.TRADE_OPEN_BORDERS:
							pDeal.kill()
							self.setDiplomacyEditInfo()
							return 1

	def handleTeamEditSignDefensivePactCB(self, argsList):
		iIndex, strName = argsList
		if int(iIndex):
			GC.getTeam(int(strName)).signDefensivePact(self.m_iCurrentTeam)
			self.setDiplomacyEditInfo()
			return 1
		else:
			for i in xrange(CyGame().getIndexAfterLastDeal()):
				pDeal = CyGame().getDeal(i)
				iPlayer1 = pDeal.getFirstPlayer()
				iPlayer2 = pDeal.getSecondPlayer()
				if iPlayer1 == -1 or iPlayer2 == -1: continue
				iTeam1 = GC.getPlayer(pDeal.getFirstPlayer()).getTeam()
				iTeam2 = GC.getPlayer(pDeal.getSecondPlayer()).getTeam()
				if (iTeam1 == int(strName) and iTeam2 == self.m_iCurrentTeam) or (iTeam2 == int(strName) and iTeam1 == self.m_iCurrentTeam):
					for j in xrange(pDeal.getLengthFirstTrades()):
						if pDeal.getFirstTrade(j).ItemType == TradeableItems.TRADE_DEFENSIVE_PACT:
							pDeal.kill()
							self.setDiplomacyEditInfo()
							return 1

	def handleDiplomacyCommandsCB(self, argsList):
		iIndex, strName = argsList
		if strName == "War":
			if int(argsList[0]) == 2:
				for i in xrange(GC.getMAX_TEAMS()):
					if GC.getTeam(i).isAlive() and i != self.m_iCurrentTeam:
						if GC.getTeam(i).isVassal(self.m_iCurrentTeam) or GC.getTeam(self.m_iCurrentTeam).isVassal(i): continue
						if GC.getTeam(self.m_iCurrentTeam).isHasMet(i):
							GC.getTeam(self.m_iCurrentTeam).declareWar(i, True, -1)
			elif int(argsList[0]) == 1:
				for i in xrange(GC.getMAX_TEAMS()):
					if GC.getTeam(i).isAlive() and i != self.m_iCurrentTeam:
						GC.getTeam(self.m_iCurrentTeam).makePeace(i)
		elif strName == "Permanent":
			for i in xrange(GC.getMAX_TEAMS()):
				if GC.getTeam(i).isAlive() and i != self.m_iCurrentTeam:
					if GC.getTeam(self.m_iCurrentTeam).isHasMet(i):
						GC.getTeam(self.m_iCurrentTeam).setPermanentWarPeace(i, int(argsList[0]) - 1)
		elif strName == "OpenBorders":
			if int(argsList[0]) == 2:
				for i in xrange(GC.getMAX_TEAMS()):
					if GC.getTeam(i).isAlive() and i != self.m_iCurrentTeam:
						if GC.getTeam(self.m_iCurrentTeam).isHasMet(i):
							GC.getTeam(self.m_iCurrentTeam).signOpenBorders(i)
			elif int(argsList[0]) == 1:
				for i in xrange(CyGame().getIndexAfterLastDeal()):
					pDeal = CyGame().getDeal(i)
					iPlayer1 = pDeal.getFirstPlayer()
					iPlayer2 = pDeal.getSecondPlayer()
					if iPlayer1 == -1 or iPlayer2 == -1: continue
					iTeam1 = GC.getPlayer(pDeal.getFirstPlayer()).getTeam()
					iTeam2 = GC.getPlayer(pDeal.getSecondPlayer()).getTeam()
					if iTeam1 == self.m_iCurrentTeam or iTeam2 == self.m_iCurrentTeam:
						for j in xrange(pDeal.getLengthFirstTrades()):
							if pDeal.getFirstTrade(j).ItemType == TradeableItems.TRADE_OPEN_BORDERS:
								pDeal.kill()
		elif strName == "DefensivePact":
			if int(argsList[0]) == 2:
				for i in xrange(GC.getMAX_TEAMS()):
					if GC.getTeam(i).isAlive() and i != self.m_iCurrentTeam:
						if GC.getTeam(self.m_iCurrentTeam).isHasMet(i):
							GC.getTeam(self.m_iCurrentTeam).signDefensivePact(i)
			elif int(argsList[0]) == 1:
				for i in xrange(CyGame().getIndexAfterLastDeal()):
					pDeal = CyGame().getDeal(i)
					iPlayer1 = pDeal.getFirstPlayer()
					iPlayer2 = pDeal.getSecondPlayer()
					if iPlayer1 == -1 or iPlayer2 == -1: continue
					iTeam1 = GC.getPlayer(pDeal.getFirstPlayer()).getTeam()
					iTeam2 = GC.getPlayer(pDeal.getSecondPlayer()).getTeam()
					if iTeam1 == self.m_iCurrentTeam or iTeam2 == self.m_iCurrentTeam:
						for j in xrange(pDeal.getLengthFirstTrades()):
							if pDeal.getFirstTrade(j).ItemType == TradeableItems.TRADE_DEFENSIVE_PACT:
								pDeal.kill()
		self.setDiplomacyEditInfo()
		return 1
## Platy World Builder End ##

	def handleCityEditNameCB(self, argsList):
		if ((len(argsList[0]) < 1) or (not self.m_pActivePlot.isCity())):
			return 1
		szNewName = argsList[0]
		city = self.m_pActivePlot.getPlotCity()
		if (city):
			city.setName(szNewName, False)
		return 1

	def handleUnitEditPullDownCB(self, argsList):
		self.m_iCurrentUnit = int(argsList[0])
		self.setUnitEditInfo(True)
		return 1

	def handleUnitAITypeEditPullDownCB(self, argsList):
		self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setUnitAIType(int(argsList[0]))
		return 1

## Platy World Builder Start ##
	def handlePlayerUnitPullDownCB(self, argsList):
		iIndex = int(argsList)
		iCount = 0
		for i in xrange( GC.getMAX_PLAYERS()):
			if GC.getPlayer(i).isEverAlive():
				if iCount == iIndex:
					self.m_iCurrentPlayer = i
					self.m_iCurrentTeam = GC.getPlayer(i).getTeam()
					self.refreshSideMenu()
					self.refreshPlayerTabCtrl()
					return 1
				iCount += 1
## Platy World Builder End ##

## Platy Tech By Era Start ##
	def handleTechByEraPullDownCB(self, argsList):
		iIndex = int(argsList[0])
		for i in xrange(GC.getNumTechInfos()):
			if GC.getTechInfo(i).getEra() == -1: continue
			if GC.getTechInfo(i).getEra() == iIndex -1 or iIndex > GC.getNumEraInfos():
				GC.getTeam(self.m_iCurrentTeam).setHasTech(i, True, self.m_iCurrentPlayer, False, False)
		self.refreshPlayerTabCtrl()
		self.setTechnologyEditInfo()
		return 1

	def handleRemoveTechByEraPullDownCB(self, argsList):
		iIndex = int(argsList[0])
		for i in xrange(GC.getNumTechInfos()):
			if GC.getTechInfo(i).getEra() == -1: continue
			if GC.getTechInfo(i).getEra() == iIndex -1 or iIndex > GC.getNumEraInfos():
				GC.getTeam(self.m_iCurrentTeam).setHasTech(i, False, self.m_iCurrentPlayer, False, False)
		self.refreshPlayerTabCtrl()
		self.setTechnologyEditInfo()
		return 1
## Platy Tech By Era End ##

	def handleSelectTeamPullDownCB(self, argsList):
		iIndex = int(argsList)
		iCount = -1
		for i in xrange( GC.getMAX_PC_TEAMS()):
			if GC.getTeam(i).isAlive():
				iCount = iCount + 1
				if (iCount == iIndex):
					self.m_iCurrentTeam = i
		self.refreshReveal()
		return 1

	def hasPromotion(self, iPromotion):
		return self.m_pActivePlot.getUnit(self.m_iCurrentUnit).isHasPromotion(iPromotion)

	def hasTech(self, iTech):
		return GC.getTeam(GC.getPlayer(self.m_iCurrentPlayer).getTeam()).isHasTech(iTech)

	def handleTechCB(self, argsList):
		bOn, strName = argsList
		if ((strName.find("_") != -1) and (self.m_iCurrentPlayer >= 0)):
			iTech = int(strName[strName.find("_")+1:])
			GC.getTeam(GC.getPlayer(self.m_iCurrentPlayer).getTeam()).setHasTech(iTech, bOn, self.m_iCurrentPlayer, False, False)
			self.resetTechButtons()
		return 1

## Platy Edit Building ##
	def handleEditCityBuildingCB(self, argsList):
		iIndex, strName = argsList
		iNewBuilding = int(strName)
		self.m_pActivePlot.getPlotCity().setNumRealBuilding(iNewBuilding, iIndex)
		return 1
## Platy Edit Building ##

## Platy Brush Size ##
	def handleBrushSizeCB(self, argsList):
		self.m_iBrushSize = int(argsList) + 1
		return 1
## Platy Brush Size ##

	def handleLandmarkCB(self, argsList):
		return 1

	########################################################
	### Advanced Start Stuff
	########################################################

	def refreshASItemCost(self):

		if (CyInterface().isInAdvancedStart()):

			self.m_iCost = 0

			if (self.m_pCurrentPlot != 0):

#				if (not self.m_pCurrentPlot.isAdjacentNonrevealed(CyGame().getActiveTeam()) and self.m_pCurrentPlot.isRevealed(CyGame().getActiveTeam(), False)):
				if (self.m_pCurrentPlot.isRevealed(CyGame().getActiveTeam(), False)):

					# Unit mode
					if (self.getASActiveUnit() != -1):
						self.m_iCost = GC.getPlayer(self.m_iCurrentPlayer).getAdvancedStartUnitCost(self.getASActiveUnit(), True, self.m_pCurrentPlot)
					elif (self.getASActiveCity() != -1):
						self.m_iCost = GC.getPlayer(self.m_iCurrentPlayer).getAdvancedStartCityCost(True, self.m_pCurrentPlot)
					elif (self.getASActivePop() != -1 and self.m_pCurrentPlot.isCity()):
						self.m_iCost = GC.getPlayer(self.m_iCurrentPlayer).getAdvancedStartPopCost(True, self.m_pCurrentPlot.getPlotCity())
					elif (self.getASActiveCulture() != -1 and self.m_pCurrentPlot.isCity()):
						self.m_iCost = GC.getPlayer(self.m_iCurrentPlayer).getAdvancedStartCultureCost(True, self.m_pCurrentPlot.getPlotCity())
					elif (self.getASActiveBuilding() != -1 and self.m_pCurrentPlot.isCity()):
						self.m_iCost = GC.getPlayer(self.m_iCurrentPlayer).getAdvancedStartBuildingCost(self.getASActiveBuilding(), True, self.m_pCurrentPlot.getPlotCity())
					elif (self.getASActiveRoute() != -1):
						self.m_iCost = GC.getPlayer(self.m_iCurrentPlayer).getAdvancedStartRouteCost(self.getASActiveRoute(), True, self.m_pCurrentPlot)
					elif (self.getASActiveImprovement() != -1):
						self.m_iCost = GC.getPlayer(self.m_iCurrentPlayer).getAdvancedStartImprovementCost(self.getASActiveImprovement(), True, self.m_pCurrentPlot)

				elif (self.m_pCurrentPlot.isAdjacentNonrevealed(CyGame().getActiveTeam())):
					if (self.getASActiveVisibility() != -1):
						self.m_iCost = GC.getPlayer(self.m_iCurrentPlayer).getAdvancedStartVisibilityCost(True, self.m_pCurrentPlot)

			if (self.m_iCost < 0):
				self.m_iCost = 0

			self.refreshSideMenu()

	def getASActiveUnit(self):
		# Unit Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASUnitTabID):
			iUnitType = getASUnit(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])
			return iUnitType

		return -1

	def getASActiveCity(self):
		# City Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):
			# City List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASCityListID):
				iOptionID = self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()]
				# Place City
				if (iOptionID == 0):
					return 1

		return -1

	def getASActivePop(self):
		# City Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):
			# City List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASCityListID):
				iOptionID = self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()]
				# Place Pop
				if (iOptionID == 1):
					return 1

		return -1

	def getASActiveCulture(self):
		# City Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):
			# City List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASCityListID):
				iOptionID = self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()]
				# Place Culture
				if (iOptionID == 2):
					return 1

		return -1

	def getASActiveBuilding(self):
		# Building Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):
			# Buildings List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASBuildingsListID):
				iBuildingType = getASBuilding(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])
				return iBuildingType

		return -1

	def getASActiveRoute(self):
		# Improvements Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASImprovementsTabID):
			# Routes List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASRoutesListID):
				iRouteType = getASRoute(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])
				if -1 == iRouteType:
					self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] = self.m_iASImprovementsListID
				return iRouteType

		return -1

	def getASActiveImprovement(self):
		# Improvements Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASImprovementsTabID):
			# Improvements List
			if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASImprovementsListID):
				iImprovementType = getASImprovement(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])
				if -1 == iImprovementType:
					self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] = self.m_iASRoutesListID
				return iImprovementType

		return -1

	def getASActiveVisibility(self):
		# Visibility Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASVisibilityTabID):
			return 1

		return -1

	def getASActiveTech(self):
		# Tech Tab
		if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASTechTabID):
			return 1

		return -1

	def placeObject( self):
		# Advanced Start
		if (CyInterface().isInAdvancedStart()):

			pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
			pPlot = CyMap().plot(self.m_iCurrentX, self.m_iCurrentY)

			iActiveTeam = CyGame().getActiveTeam()
			if (self.m_pCurrentPlot.isRevealed(iActiveTeam, False)):

				# City Tab
				if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):

					# City List
					if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASCityListID):

						iOptionID = self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()]

						# Place City
						if (iOptionID == 0):

							# Cost -1 means may not be placed here
							if (pPlayer.getAdvancedStartCityCost(True, pPlot) != -1):

								CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_CITY, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, True)	#Action, Player, X, Y, Data, bAdd

						# City Population
						elif (iOptionID == 1):

							if (pPlot.isCity()):
								pCity = pPlot.getPlotCity()

								# Cost -1 means may not be placed here
								if (pPlayer.getAdvancedStartPopCost(True, pCity) != -1):

										CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_POP, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, True)	#Action, Player, X, Y, Data, bAdd

						# City Culture
						elif (iOptionID == 2):

							if (pPlot.isCity()):
								pCity = pPlot.getPlotCity()

								# Cost -1 means may not be placed here
								if (pPlayer.getAdvancedStartCultureCost(True, pCity) != -1):

									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_CULTURE, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, True)	#Action, Player, X, Y, Data, bAdd

					# Buildings List
					elif (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASBuildingsListID):

							if (pPlot.isCity()):
								pCity = pPlot.getPlotCity()

								iBuildingType = getASBuilding(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

								# Cost -1 means may not be placed here
								if (iBuildingType != -1 and pPlayer.getAdvancedStartBuildingCost(iBuildingType, True, pCity) != -1):

									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_BUILDING, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iBuildingType, True)	#Action, Player, X, Y, Data, bAdd

				# Unit Tab
				elif (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASUnitTabID):
					iUnitType = getASUnit(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

					# Cost -1 means may not be placed here
					if (iUnitType != -1 and pPlayer.getAdvancedStartUnitCost(iUnitType, True, pPlot) != -1):

						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_UNIT, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iUnitType, True)	#Action, Player, X, Y, Data, bAdd

				# Improvements Tab
				elif (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASImprovementsTabID):

					# Routes List
					if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASRoutesListID):

						iRouteType = getASRoute(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

						# Cost -1 means may not be placed here
						if (iRouteType != -1 and pPlayer.getAdvancedStartRouteCost(iRouteType, True, pPlot) != -1):

							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_ROUTE, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iRouteType, True)	#Action, Player, X, Y, Data, bAdd

					# Improvements List
					elif (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASImprovementsListID):

						iImprovementType = getASImprovement(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

						# Cost -1 means may not be placed here
						if (pPlayer.getAdvancedStartImprovementCost(iImprovementType, True, pPlot) != -1):

							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_IMPROVEMENT, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iImprovementType, True)	#Action, Player, X, Y, Data, bAdd

			# Adjacent nonrevealed
			else:

				# Visibility Tab
				if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASVisibilityTabID):

					# Cost -1 means may not be placed here
					if (pPlayer.getAdvancedStartVisibilityCost(True, pPlot) != -1):

						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_VISIBILITY, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, True)	#Action, Player, X, Y, Data, bAdd

			self.m_bSideMenuDirty = True
			self.m_bASItemCostDirty = True

			return 1

		if ((self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()] == -1) or (self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()] == -1) or (self.m_iCurrentX == -1) or (self.m_iCurrentY == -1) or (self.m_iCurrentPlayer == -1)):
			return 1

		if (self.m_bEraseAll):
			self.eraseAll()
		elif ((self.m_bNormalPlayer) and (self.m_normalPlayerTabCtrl.getActiveTab() == self.m_iUnitTabID)):
			iUnitType = self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()]
			pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
			iPlotX = self.m_iCurrentX
			iPlotY = self.m_iCurrentY
			pPlayer.initUnit(iUnitType, iPlotX, iPlotY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
		elif ((self.m_bNormalPlayer) and (self.m_normalPlayerTabCtrl.getActiveTab() == self.m_iBuildingTabID)):
			iBuildingType = self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()]
			if ((self.m_pCurrentPlot.isCity()) and (iBuildingType != 0)):
				self.m_pCurrentPlot.getPlotCity().setNumRealBuilding(iBuildingType-1, 1)
			if (iBuildingType == 0):
				if (not self.m_pCurrentPlot.isCity()):
					pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
					iX = self.m_pCurrentPlot.getX()
					iY = self.m_pCurrentPlot.getY()
					pPlayer.initCity(iX, iY)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iImprovementTabID)):
			iImprovementType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
			iIndex = -1
			iCounter = -1
			while ((iIndex < iImprovementType) and (iCounter < GC.getNumImprovementInfos())):
				iCounter = iCounter + 1
				if (not GC.getImprovementInfo(iCounter).isGraphicalOnly()):
					iIndex = iIndex + 1
			if (iIndex > -1):
				self.m_pCurrentPlot.setImprovementType(iCounter)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iBonusTabID)):
			iBonusType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
			self.m_pCurrentPlot.setBonusType(iBonusType)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerrainTabID)):
			if (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iTerrainListID):
				iTerrainType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				self.m_pCurrentPlot.setTerrainType(iTerrainType, True, True)
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iFeatureListID):
				iButtonIndex = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				iCount = -1
				for i in xrange(GC.getNumFeatureInfos()):
					for j in xrange(GC.getFeatureInfo(i).getNumVarieties()):
						iCount = iCount + 1
						if (iCount == iButtonIndex):
							self.m_pCurrentPlot.setFeatureType(i, j)
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iPlotTypeListID):
				iPlotType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				if (iPlotType >= 0) and (iPlotType < PlotTypes.NUM_PLOT_TYPES):
					self.m_pCurrentPlot.setPlotType(PlotTypes(iPlotType), True, True)
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iRouteListID):
				iRouteType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				if (iRouteType == GC.getNumRouteInfos()):
					if (self.m_pRiverStartPlot == self.m_pCurrentPlot):
						self.m_pRiverStartPlot = -1
						CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
						return 1
					if (self.m_pRiverStartPlot != -1):
						iXDiff = 0
						iYDiff = 0
						if (self.m_pRiverStartPlot.getX() < self.m_pCurrentPlot.getX()):
							iXDiff = self.m_pCurrentPlot.getX() - self.m_pRiverStartPlot.getX()
						elif (self.m_pRiverStartPlot.getX() > self.m_pCurrentPlot.getX()):
							iXDiff = self.m_pRiverStartPlot.getX() - self.m_pCurrentPlot.getX()
						if (self.m_pRiverStartPlot.getY() < self.m_pCurrentPlot.getY()):
							iYDiff = self.m_pCurrentPlot.getY() - self.m_pRiverStartPlot.getY()
						elif (self.m_pRiverStartPlot.getY() > self.m_pCurrentPlot.getY()):
							iYDiff = self.m_pRiverStartPlot.getY() - self.m_pCurrentPlot.getY()

						if ((iXDiff == iYDiff) and (iXDiff == 1) and (self.m_pRiverStartPlot.getX() > self.m_pCurrentPlot.getX()) and (self.m_pRiverStartPlot.getY() < self.m_pCurrentPlot.getY())):
							self.placeRiverNW(True)
							self.m_pRiverStartPlot = CyMap().plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()+1)
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
				else:
					self.m_pCurrentPlot.setRouteType(iRouteType)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerritoryTabID)):
			iPlayer = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
			if (GC.getPlayer(iPlayer).isEverAlive()):
				self.m_pCurrentPlot.setOwner(iPlayer)
		elif (self.m_bLandmark):
			CvEventInterface.beginEvent(5009)
		return 1

	def removeObject( self):

		# Advanced Start
		if (CyInterface().isInAdvancedStart()):

			pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
			pPlot = CyMap().plot(self.m_iCurrentX, self.m_iCurrentY)

			iActiveTeam = CyGame().getActiveTeam()
#			if (not self.m_pCurrentPlot.isAdjacentNonrevealed(iActiveTeam) and self.m_pCurrentPlot.isRevealed(iActiveTeam, False)):
			if (self.m_pCurrentPlot.isRevealed(iActiveTeam, False)):

				# City Tab
				if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASCityTabID):

					# City List
					if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASCityListID):

						iOptionID = self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()]

						# Place City
						if (iOptionID == 0):

							# Ability to remove cities not allowed because of 'sploitz (visibility, chopping down jungle, etc.)
							return 1

							if (self.m_pCurrentPlot.isCity()):

								if (self.m_pCurrentPlot.getPlotCity().getOwner() == self.m_iCurrentPlayer):

									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_CITY, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, False)	#Action, Player, X, Y, Data, bAdd

						# City Population
						elif (iOptionID == 1):

							if (pPlot.isCity()):
								if (pPlot.getPlotCity().getOwner() == self.m_iCurrentPlayer):

									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_POP, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, False)	#Action, Player, X, Y, Data, bAdd

						# City Culture
						elif (iOptionID == 2):

							# Ability to remove cities not allowed because of 'sploitz (visibility)
							return 1

							if (pPlot.isCity()):
								if (pPlot.getPlotCity().getOwner() == self.m_iCurrentPlayer):

									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_CULTURE, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, False)	#Action, Player, X, Y, Data, bAdd

					# Buildings List
					elif (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASBuildingsListID):

						if (pPlot.isCity()):
							if (pPlot.getPlotCity().getOwner() == self.m_iCurrentPlayer):

								iBuildingType = getASBuilding(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

								if -1 != iBuildingType:
									CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_BUILDING, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iBuildingType, False)	#Action, Player, X, Y, Data, bAdd

				# Unit Tab
				elif (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASUnitTabID):

					iUnitType = getASUnit(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

					if -1 != iUnitType:
						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_UNIT, self.m_iCurrentPlayer, self.m_pCurrentPlot.getX(), self.m_pCurrentPlot.getY(), iUnitType, False)	#Action, Player, X, Y, Data, bAdd

				# Improvements Tab
				elif (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASImprovementsTabID):

					# Routes List
					if (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASRoutesListID):

						iRouteType = getASRoute(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

						if -1 != iRouteType:
							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_ROUTE, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iRouteType, False)	#Action, Player, X, Y, Data, bAdd

					# Improvements List
					elif (self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] == self.m_iASImprovementsListID):

						iImprovementType = getASImprovement(self.m_iAdvancedStartCurrentIndexes[self.m_advancedStartTabCtrl.getActiveTab()])

						if -1 != iImprovementType:
							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_IMPROVEMENT, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, iImprovementType, False)	#Action, Player, X, Y, Data, bAdd

			# Adjacent nonrevealed
			else:

				# Visibility Tab
				if (self.m_advancedStartTabCtrl.getActiveTab() == self.m_iASVisibilityTabID):

					# Ability to remove sight not allowed because of 'sploitz
					return 1

					# Remove Visibility
					if (pPlot.isRevealed(iActiveTeam, False)):

						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_VISIBILITY, self.m_iCurrentPlayer, self.m_iCurrentX, self.m_iCurrentY, -1, False)	#Action, Player, X, Y, Data, bAdd

			self.m_bSideMenuDirty = True
			self.m_bASItemCostDirty = True

			return 1

		if ((self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()] == -1) or (self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()] == -1) or (self.m_iCurrentX == -1) or (self.m_iCurrentY == -1) or (self.m_iCurrentPlayer == -1)):
			return 1

		if (self.m_bEraseAll):
			self.eraseAll()
		elif ((self.m_bNormalPlayer) and (self.m_normalPlayerTabCtrl.getActiveTab() == self.m_iUnitTabID)):
			for i in xrange(self.m_pCurrentPlot.getNumUnits()):
				pUnit = self.m_pCurrentPlot.getUnit(i)
				if (pUnit.getUnitType() == self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()]):
					pUnit.kill(False, PlayerTypes.NO_PLAYER)
					return 1
			if (self.m_pCurrentPlot.getNumUnits() > 0):
				pUnit = self.m_pCurrentPlot.getUnit(0)
				pUnit.kill(False, PlayerTypes.NO_PLAYER)
				return 1
		elif ((self.m_bNormalPlayer) and (self.m_normalPlayerTabCtrl.getActiveTab() == self.m_iBuildingTabID)):
			if (self.m_pCurrentPlot.isCity()):
				iBuildingType = self.m_iNormalPlayerCurrentIndexes[self.m_normalPlayerTabCtrl.getActiveTab()]
				if (iBuildingType == 0):
					self.m_pCurrentPlot.getPlotCity().kill()
				else:
					self.m_pCurrentPlot.getPlotCity().setNumRealBuilding(iBuildingType-1, 0)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iImprovementTabID)):
			self.m_pCurrentPlot.setImprovementType(-1)
			return 1
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iBonusTabID)):
			iBonusType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
			self.m_pCurrentPlot.setBonusType(-1)
			return 1
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerrainTabID)):
			if (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iTerrainListID):
				return 1
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iFeatureListID):
				iFeatureType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				self.m_pCurrentPlot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
				return 1
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iPlotTypeListID):
				return 1
			elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iRouteListID):
				iRouteType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
				if (iRouteType == GC.getNumRouteInfos()):
					if (self.m_pRiverStartPlot != -1):
						self.m_pRiverStartPlot = -1
						CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
					else:
						self.m_pCurrentPlot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
						self.m_pCurrentPlot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
				else:
					self.m_pCurrentPlot.setRouteType(-1)
		elif ((self.m_bNormalMap) and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerritoryTabID)):
			self.m_pCurrentPlot.setOwner(-1)
			return 1
		elif (self.m_bLandmark):
			self.removeLandmarkCB()
		return 1

	def handleClicked( self):
		return

	def isIntString(self, arg):
		for i in xrange(len(arg)):
			if (arg[i] > '9'):
				return False
			elif (arg[i] < '0'):
				return False
		return True

	def placeRiverNW(self, bUseCurrent):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY())
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()+1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		return

	def placeRiverN(self, bUseCurrent):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY()+1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		return

	def placeRiverNE(self, bUseCurrent):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY())
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY()+1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		return

	def placeRiverW(self, bUseCurrent):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY())
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)
		return

	def placeRiverE(self, bUseCurrent):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY())
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
		return

	def placeRiverSW(self, bUseCurrent):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()-1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
		return

	def placeRiverS(self, bUseCurrent):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY()-1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
		return

	def placeRiverSE(self, bUseCurrent):
		if (bUseCurrent):
			pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY())
			if (not pRiverStepPlot.isNone()):
				pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)

		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY())
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
		pRiverStepPlot = CyMap().plot(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY()-1)
		if (not pRiverStepPlot.isNone()):
			pRiverStepPlot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
		return

	def setUnitEditInfo(self, bSamePlot):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		if not bSamePlot:
			self.m_pActivePlot = self.m_pCurrentPlot

		self.m_tabCtrlEdit.setNumColumns((GC.getNumPromotionInfos()/10)+1)
		self.m_tabCtrlEdit.setColumnLength(16)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_UNIT_DATA",(self.m_pActivePlot.getX(), self.m_pActivePlot.getY())))
		strTest = ()
		for i in xrange(self.m_pActivePlot.getNumUnits()):
			if (len(self.m_pActivePlot.getUnit(i).getNameNoDesc())):
				strTest = strTest + (self.m_pActivePlot.getUnit(i).getNameNoDesc(),)
			else:
				strTest = strTest + (self.m_pActivePlot.getUnit(i).getName(),)

		self.m_tabCtrlEdit.addSectionDropdown("Current_Unit", strTest, "CvScreensInterface", "WorldBuilderHandleUnitEditPullDownCB", "UnitEditPullDown", 0, self.m_iCurrentUnit)

## Platy Builder ##
		self.m_iCurrentPlayer = self.m_pActivePlot.getUnit(self.m_iCurrentUnit).getOwner()
		self.m_iCurrentTeam = GC.getPlayer(self.m_iCurrentPlayer).getTeam()
		pUnit = self.m_pActivePlot.getUnit(self.m_iCurrentUnit)
## Platy Builder ##

		if (len(pUnit.getNameNoDesc())):
			strName = pUnit.getNameNoDesc()
		else:
			strName = pUnit.getName()
		self.m_tabCtrlEdit.addSectionEditCtrl(strName, "CvScreensInterface", "WorldBuilderHandleUnitEditNameCB", "UnitEditName", 0)
## Experience ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_EXPERIENCE",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("UnitEditExperienceCB", "CvScreensInterface", "WorldBuilderHandleUnitEditExperienceCB", "UnitEditExperience", 0, 0, 10000, 1, pUnit.getExperience(), 0, 0)
## Level ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_LEVEL",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("UnitEditLevelCB", "CvScreensInterface", "WorldBuilderHandleUnitEditLevelCB", "UnitEditLevel", 0, 1, 100, 1, pUnit.getLevel(), 0, 0)
## Base Strength ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_STRENGTH",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("UnitEditStrengthCB", "CvScreensInterface", "WorldBuilderHandleUnitEditStrengthCB", "UnitEditStrength", 0, 0, 1000, 1, pUnit.baseCombatStr(), 0, 0)
## Damage % ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_DAMAGE",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("UnitEditDamageCB", 	"CvScreensInterface", "WorldBuilderHandleUnitEditDamageCB", "UnitEditDamage", 0, 0, 100, 1, pUnit.getDamage(), 0, 0)
## Cargo ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_CARGO",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("UnitEditCargoCB", "CvScreensInterface", "WorldBuilderHandleUnitEditCargoCB", 	"UnitEditCargo", 0, 0, min(100, pUnit.cargoSpace() * 100), 1, pUnit.cargoSpace(), 0, 0)
## Moves ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_MOVES",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("UnitEditMovesCB", "CvScreensInterface", "WorldBuilderHandleUnitEditMovesCB", "UnitEditMoves", 0, 0, pUnit.baseMoves(), 1, pUnit.movesLeft() / GC.getDefineINT("MOVE_DENOMINATOR"), 0, 0)
## Immobile Timer ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_IMMOBILE_TIMER",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("UnitEditImmobileTimerCB", "CvScreensInterface", "WorldBuilderHandleUnitEditImmobileTimerCB", "UnitEditImmobileTimer", 0, 0, 100, 1, pUnit.getImmobileTimer(), 0, 0)
## Owner ##
		strTest = ()
		iCount = 0
		for i in xrange(GC.getMAX_PLAYERS()):
			if GC.getPlayer(i).isEverAlive():
				strTest = strTest + (GC.getPlayer(i).getName(),)
				if i == self.m_iCurrentPlayer:
					iUnitOwner = iCount
				iCount = iCount + 1
		self.m_tabCtrlEdit.addSectionDropdown("Unit Owner", strTest, "CvScreensInterface", "WorldBuilderHandleCurrentPlayerEditPullDownCB", "UnitEditOwner", 0, iUnitOwner)
## Promotion Ready ##
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_PROMOTION_READY",()), "CvScreensInterface", "WorldBuilderHandleUnitEditPromotionReadyCB", "UnitEditPromotionReady", 0, pUnit.isPromotionReady())
## Made Attack ##
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_MADE_ATTACK",()), "CvScreensInterface", "WorldBuilderHandleUnitEditMadeAttackCB", "UnitEditMadeAttack", 0, pUnit.isMadeAttack())
## Made Interception ##
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_MADE_INTERCEPT",()), "CvScreensInterface", "WorldBuilderHandleUnitEditMadeInterceptionCB", "UnitEditMadeInterception", 0, pUnit.isMadeInterception())
## Unit AI Type ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_UNIT_AI",()),  0)
		strTest = ()
		for i in xrange(UnitAITypes.NUM_UNITAI_TYPES):
			sUnitAI = GC.getUnitAIInfo(i).getType()[7:]
			sUnitAI = sUnitAI.lower()
			sUnitAI = sUnitAI.replace("_", " ")
			sUnitAI = sUnitAI.capitalize()
			strTest = strTest + (sUnitAI,)
		self.m_tabCtrlEdit.addSectionDropdown("Unit_AI_Type", strTest, "CvScreensInterface", "WorldBuilderHandleUnitAITypeEditPullDownCB", "UnitAITypeEditPullDown", 0, pUnit.getUnitAIType())
## Direction ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_DIRECTION",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_NORTH",()),) + (TRNSLTR.getText("TXT_KEY_WB_NORTHEAST",()),) + (TRNSLTR.getText("TXT_KEY_WB_EAST",()),) + (TRNSLTR.getText("TXT_KEY_WB_SOUTHEAST",()),)
		strTest += (TRNSLTR.getText("TXT_KEY_WB_SOUTH",()),) + (TRNSLTR.getText("TXT_KEY_WB_SOUTHWEST",()),) + (TRNSLTR.getText("TXT_KEY_WB_WEST",()),) + (TRNSLTR.getText("TXT_KEY_WB_NORTHWEST",()),)
		self.m_tabCtrlEdit.addSectionDropdown("Unit_Direction", strTest, "CvScreensInterface", "WorldBuilderHandleUnitEditDirectionCB", "UnitEditDirection", 0, pUnit.getFacingDirection())
## Promotions ##
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_PROMOTIONS",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "PromotionEditScreen", 0)
## Move Unit ##
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_MOVE_UNIT",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleMoveUnitCB", "MoveUnit", 0)
## Duplicate ##
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_DUPLICATE",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleUnitEditDuplicateCB", "UnitEditDuplicate", 0)
## Kill ##
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_KILL",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleKillCB", "Unit", 0)
## Unit Script ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_SCRIPT_DATA",()),  0)
		self.m_tabCtrlEdit.addSectionEditCtrl(pUnit.getScriptData(), "CvScreensInterface", "WorldBuilderHandleEditScriptCB", "UnitEditScript", 0)
##
		initWBToolEditCtrlTab(True)

		if (not self.m_tabCtrlEdit.isNone()):
			print("Enabling map control 4")
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return

## Promotion Edit Screen ##
	def setPromotionEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 4
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_PROMOTIONS",()))

		lPromotions = []
		for i in xrange(GC.getNumPromotionInfos()):
			lPromotions.append(GC.getPromotionInfo(i).getDescription() + "_Platy_" + str(i))
		lPromotions.sort()

		iColumnLength = (len(lPromotions) +2) /iNumColumns
		if (len(lPromotions) +2) %iNumColumns > 0:
			iColumnLength += 1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)
		self.m_tabCtrlEdit.setSize(min(1000, iNumColumns * 200), min(700, max(5, iColumnLength) * 42))

		for i in lPromotions:
			sPromotion = i[:i.find("_Platy_")]
			iPromotion = int(i[i.find("_Platy_") +7:])
			self.m_tabCtrlEdit.addSectionCheckbox(sPromotion, "CvScreensInterface", "WorldBuilderHandleEditUnitPromotionCB", str(iPromotion), 0, self.m_pActivePlot.getUnit(self.m_iCurrentUnit).isHasPromotion(iPromotion))
		for i in xrange(iNumColumns * iColumnLength - (len(lPromotions) +2)):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)

		strTest = (TRNSLTR.getText("TXT_KEY_WB_COMMANDS",()),) + (TRNSLTR.getText("TXT_KEY_WB_ALL_PROMOTIONS",()),) + (TRNSLTR.getText("TXT_KEY_WB_CLEAR_ALL",()),)
		self.m_tabCtrlEdit.addSectionDropdown("Promotion Commands", strTest, "CvScreensInterface", "WorldBuilderHandlePromotionCommandsCB", "Promotion Commands", 0, 0)

		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BACK",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "BackToUnit", 0)
		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Promotion Edit Screen ##

	def setCityEditInfo(self, bSamePlot):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

## Platy City Data ##
		if not bSamePlot:
			self.m_pActivePlot = self.m_pCurrentPlot
		pCity = self.m_pActivePlot.getPlotCity()
		self.m_iCurrentPlayer = pCity.getOwner()
		self.m_iCurrentTeam = GC.getPlayer(self.m_iCurrentPlayer).getTeam()
		self.m_tabCtrlEdit.setNumColumns(4)
		self.m_tabCtrlEdit.setColumnLength(10)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_CITY_DATA",(self.m_pActivePlot.getX(), self.m_pActivePlot.getY())))
## Choose City ##
		strTest = ()
		iCount = 0
		(loopCity, iter) = GC.getPlayer(self.m_iCurrentPlayer).firstCity(False)
		while(loopCity):
			if loopCity.getX() == self.m_pActivePlot.getX() and loopCity.getY() == self.m_pActivePlot.getY():
				iCurrentCity = iCount
			strTest = strTest + (loopCity.getName(),)
			iCount += 1
			(loopCity, iter) = GC.getPlayer(self.m_iCurrentPlayer).nextCity(iter, False)
		self.m_tabCtrlEdit.addSectionDropdown("Owner", strTest, "CvScreensInterface", "WorldBuilderHandleChooseCityCB", "ChooseCity", 0, iCurrentCity)
## City Name ##
		self.m_tabCtrlEdit.addSectionEditCtrl(pCity.getName(), "CvScreensInterface", "WorldBuilderHandleCityEditNameCB", "CityEditName", 0)
## Owner ##
		strTest = ()
		iCount = 0
		for i in xrange(GC.getMAX_PLAYERS()):
			if GC.getPlayer(i).isEverAlive():
				strTest = strTest + (GC.getPlayer(i).getName(),)
				if i == self.m_iCurrentPlayer:
					iOwner = iCount
				iCount = iCount + 1
		self.m_tabCtrlEdit.addSectionDropdown("Owner", strTest, "CvScreensInterface", "WorldBuilderHandleCurrentPlayerEditPullDownCB", "CityEditOwner", 0, iOwner)
## Population ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_POPULATION",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("CityEditPopulationCB", "CvScreensInterface", "WorldBuilderHandleCityEditPopulationCB", "CityEditPopulation", 0, 1, 1000, 1, pCity.getPopulation(), 0, 0)
## Food ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_FOOD",(pCity.growthThreshold(),)),  0)
		self.m_tabCtrlEdit.addSectionSpinner("CityEditFoodCB", "CvScreensInterface", "WorldBuilderHandleCityEditFoodCB", "CityEditFood", 0, 0, pCity.growthThreshold(), 1, pCity.getFood(), 0, 0)
## Culture ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_CULTURE",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("CityEditCultureCB", "CvScreensInterface", "WorldBuilderHandleCityEditCultureCB", "CityEditCulture", 0, 1, 100000000, 1, pCity.getCulture(self.m_iCurrentPlayer), 0, 0)
		strTest = ()
		for i in xrange(GC.getNumCultureLevelInfos()):
			strTest = strTest + (GC.getCultureLevelInfo(i).getDescription(),)
		self.m_tabCtrlEdit.addSectionDropdown("CityEditCultureLevelCB", strTest, "CvScreensInterface", "WorldBuilderHandleCityEditCultureLevelCB", "CityEditCultureLevel", 0, pCity.getCultureLevel())
## Happiness ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_NET_HAPPINESS",()),  0)
		strHappiness = str("CityEditHappinessCB")
		self.m_tabCtrlEdit.addSectionSpinner(strHappiness, "CvScreensInterface", "WorldBuilderHandleCityEditHappinessCB", "CityEditHappiness", 0, -1000, 1000, 1, pCity.happyLevel() - pCity.unhappyLevel(0), 0, 0)
## Health ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_NET_HEALTH",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("CityEditHealthCB", "CvScreensInterface", "WorldBuilderHandleCityEditHealthCB", "CityEditHealth", 0, -1000, 1000, 1, pCity.goodHealth() - pCity.badHealth(False), 0, 0)
## Defense ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_DEFENSE",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("CityEditDefenseCB", "CvScreensInterface", "WorldBuilderHandleCityEditDefenseCB", "CityEditDefense", 0, 0, pCity.getTotalDefense(False), 1, pCity.getTotalDefense(False) * (100 - pCity.getDefenseDamage())/100, 0, 0)
## Trade Routes ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_TRADE_ROUTE",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("CityEditTradeRouteCB", "CvScreensInterface", "WorldBuilderHandleCityEditTradeRouteCB", "CityEditTradeRoute", 0, 0, GC.getDefineINT("MAX_TRADE_ROUTES"), 1, pCity.getTradeRoutes(), 0, 0)
## Great People Rate ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_GREAT_PEOPLE_RATE",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("CityEditGPRateCB", "CvScreensInterface", "WorldBuilderHandleCityEditGPRateCB", "CityEditGPRate", 0, 0, 1000, 1, pCity.getGreatPeopleRate(), 0, 0)
## Modify Buildings ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_MODIFIED_BUILDINGS",()),  0)
		lBuilding = []
		for i in xrange(GC.getNumBuildingInfos()):
			lBuilding.append(GC.getBuildingInfo(i).getDescription() + "_Platy_" + str(i))
		lBuilding.sort()

		strTest = ()
		for iBC in xrange(len(lBuilding)):
			i = lBuilding[iBC]
			sBuilding = i[:i.find("_Platy_")]
			iBuilding = int(i[i.find("_Platy_") +7:])
			strTest += (sBuilding,)
		self.m_tabCtrlEdit.addSectionDropdown("CityEditBuilding", strTest, "CvScreensInterface", "WorldBuilderHandleCityEditBuildingCB", "iBuilding", 0, self.m_iBuildingType)
		strTest = ()
		for i in xrange(3):
			strTest += (GC.getYieldInfo(i).getDescription(),)
		for i in xrange(4):
			strTest += (GC.getCommerceInfo(i).getDescription(),)
		self.m_tabCtrlEdit.addSectionDropdown("CityEditModifer", strTest, "CvScreensInterface", "WorldBuilderHandleCityEditModiferCB", "CityEditModifer", 0, self.m_iBuildingModifier)
		iBuilding = int(lBuilding[self.m_iBuildingType][lBuilding[self.m_iBuildingType].find("_Platy_") +7:])
		if self.m_iBuildingModifier < 3:
			iModifier = pCity.getBuildingYieldChange(iBuilding, self.m_iBuildingModifier)
		else:
			iModifier = pCity.getBuildingCommerceChange(iBuilding, self.m_iBuildingModifier - 3)
		self.m_tabCtrlEdit.addSectionSpinner("CityEditModifyBuildingCB", "CvScreensInterface", "WorldBuilderHandleCityEditModifyBuildingCB", "CityEditModifyBuilding", 0, -100, 100, 1, iModifier, 0, 0)
## Current Production ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_CURRENT_PRODUCTION",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_NONE",()),)
		pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
		iCurrentProduction = 0
		iCount = 1
		for iUnit in xrange(GC.getNumUnitInfos()):
			if pCity.canTrain(iUnit, True, False, False, False):
				strTest += (GC.getUnitInfo(iUnit).getDescription(),)
				if pCity.getProductionUnit() == iUnit:
					iCurrentProduction = iCount
				iCount += 1
		for iBuilding in xrange(GC.getNumBuildingInfos()):
			if pCity.canConstruct(iBuilding, True, False, False):
				strTest += (GC.getBuildingInfo(iBuilding).getDescription(),)
				if pCity.getProductionBuilding() == iBuilding:
					iCurrentProduction = iCount
				iCount += 1
		for iProject in xrange(GC.getNumProjectInfos()):
			if pCity.canCreate(iProject, True, False):
				strTest += (GC.getProjectInfo(iProject).getDescription(),)
				if pCity.getProductionProject() == iProject:
					iCurrentProduction = iCount
				iCount += 1
		for iProcess in xrange(GC.getNumProcessInfos()):
			if pCity.canMaintain(iProcess, True):
				strTest += (GC.getProcessInfo(iProcess).getDescription(),)
				if pCity.getProductionProcess() == iProcess:
					iCurrentProduction = iCount
				iCount += 1
		self.m_tabCtrlEdit.addSectionDropdown("CityEditChooseProduction", strTest, "CvScreensInterface", "WorldBuilderHandleCityEditChooseProductionCB", "CityEditChooseProduction", 0, iCurrentProduction)
		if iCurrentProduction == 0 or pCity.getProductionProcess() > -1:
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
		else:
			self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_PROGRESS",(pCity.getProductionNeeded(),)),  0)
			self.m_tabCtrlEdit.addSectionSpinner("CityEditProductionProgressCB", "CvScreensInterface", "WorldBuilderHandleCityEditProductionProgressCB", "CityEditProductionProgress", 0, 0, pCity.getProductionNeeded(), 1, pCity.getProduction(), 0, 0)
## Timers##
		strTest = (TRNSLTR.getText("TXT_KEY_WB_OCCUPATION",()),) + (TRNSLTR.getText("TXT_KEY_WB_DRAFT_ANGER",()),) + (TRNSLTR.getText("TXT_KEY_WB_HURRY_ANGER",()),)
		strTest += (TRNSLTR.getText("TXT_KEY_WB_DEFY_RESOLUTION",()),) + (TRNSLTR.getText("TXT_KEY_WB_ESP_HAPPY",()),) + (TRNSLTR.getText("TXT_KEY_WB_ESP_HEALTH",()),) + (TRNSLTR.getText("TXT_KEY_WB_TEMP_HAPPY",()),)
		self.m_tabCtrlEdit.addSectionDropdown("Timers", strTest, "CvScreensInterface", "WorldBuilderHandleCityEditTimersCB", "CityEditTimers", 0, self.m_iCityTimer)
		if self.m_iCityTimer == 0:
			iCurrentTimer = pCity.getOccupationTimer()
		elif self.m_iCityTimer == 1:
			iCurrentTimer = pCity.getConscriptAngerTimer()
		elif self.m_iCityTimer == 2:
			iCurrentTimer = pCity.getHurryAngerTimer()
		elif self.m_iCityTimer == 3:
			iCurrentTimer = pCity.getDefyResolutionAngerTimer()
		elif self.m_iCityTimer == 4:
			iCurrentTimer = pCity.getEspionageHappinessCounter()
		elif self.m_iCityTimer == 5:
			iCurrentTimer = pCity.getEspionageHealthCounter()
		else:
			iCurrentTimer = pCity.getHappinessTimer()
		self.m_tabCtrlEdit.addSectionSpinner("CityEditCurrentTimerCB", "CvScreensInterface", "WorldBuilderHandleCityEditCurrentTimerCB", "CityEditCurrentTimer", 0, 0, 100, 1, iCurrentTimer, 0, 0)
## New Screens ##
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BUILDINGS",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "BuildingEditScreen", 0)
		self.m_tabCtrlEdit.addSectionButton( TRNSLTR.getText("TXT_KEY_WB_WONDERS",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "WonderEditScreen", 0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_RELIGION",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "ReligionEditScreen", 0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_CORPORATION",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "CorporationEditScreen", 0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_FREE_SPECIALISTS",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "FreeSpecialistEditScreen", 0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_GREAT_PEOPLE",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "GreatPeopleEditScreen", 0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_FREE_BONUS",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "FreeBonusEditScreen", 0)
## City Script ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_SCRIPT_DATA",()),  0)
		self.m_tabCtrlEdit.addSectionEditCtrl(pCity.getScriptData(), "CvScreensInterface", "WorldBuilderHandleEditScriptCB", "CityEditScript", 0)
		return

## Building Edit Screen ##
	def setBuildingEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 5
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_BUILDINGS",()))

		lBuildings = []
		for i in xrange(GC.getNumBuildingInfos()):
			if isNationalWonder(i) or isTeamWonder(i) or isWorldWonder(i): continue
			lBuildings.append(GC.getBuildingInfo(i).getDescription() + "_Platy_" + str(i))
		lBuildings.sort()

		iColumnLength = (len(lBuildings) +2) /iNumColumns
		if (len(lBuildings) +2) %iNumColumns > 0:
			iColumnLength += 1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)
		self.m_tabCtrlEdit.setSize(1000, 700)

		for i in lBuildings:
			sBuilding = i[:i.find("_Platy_")]
			iBuilding = int(i[i.find("_Platy_") +7:])
			self.m_tabCtrlEdit.addSectionCheckbox(sBuilding, "CvScreensInterface", "WorldBuilderHandleEditCityBuildingCB", str(iBuilding), 0, self.m_pActivePlot.getPlotCity().isHasBuilding(iBuilding))
		for i in xrange(iNumColumns * iColumnLength - (len(lBuildings) +2)):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)

		strTest = (TRNSLTR.getText("TXT_KEY_WB_COMMANDS",()),) + (TRNSLTR.getText("TXT_KEY_WB_BUILD_ALL",()),) + (TRNSLTR.getText("TXT_KEY_WB_CLEAR_ALL",()),) + (TRNSLTR.getText("TXT_KEY_WB_COPY_ALL",()),)
		self.m_tabCtrlEdit.addSectionDropdown("Building Commands", strTest, "CvScreensInterface", "WorldBuilderHandleBuildingCommandsCB", "Building Commands", 0, 0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BACK",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "CityEditScreen", 0)
		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Building Edit Screen ##

## Wonder Edit Screen ##
	def setWonderEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 4
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_WONDERS",()))

		lNationalWonders = []
		lTeamWonders = []
		lWorldWonders = []
		for i in xrange(GC.getNumBuildingInfos()):
			if isNationalWonder(i):
				lNationalWonders.append(GC.getBuildingInfo(i).getDescription() + "_Platy_" + str(i))
			elif isTeamWonder(i):
				lNationalWonders.append(GC.getBuildingInfo(i).getDescription() + "_Platy_" + str(i))
			elif isWorldWonder(i):
				lWorldWonders.append(GC.getBuildingInfo(i).getDescription() + "_Platy_" + str(i))
		lNationalWonders.sort()
		lTeamWonders.sort()
		lWorldWonders.sort()
		iWonderType = 1
		if len(lNationalWonders) > 0:
			iWonderType += 1
		if len(lTeamWonders) > 0:
			iWonderType += 1
		if len(lWorldWonders) > 0:
			iWonderType += 1

		iColumnLength = (len(lNationalWonders) + len(lTeamWonders) + len(lWorldWonders) +iWonderType) /iNumColumns
		if (len(lNationalWonders) + len(lTeamWonders) + len(lWorldWonders) +iWonderType) %iNumColumns > 0:
			iColumnLength += 1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)
		self.m_tabCtrlEdit.setSize(1000, 700)
		if len(lNationalWonders) > 0:
			self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_NATIONAL_WONDERS",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleNationalWonderCB", "NationalWonder", 0)
			for i in lNationalWonders:
				sWonder = i[:i.find("_Platy_")]
				iWonder = int(i[i.find("_Platy_") +7:])
				self.m_tabCtrlEdit.addSectionCheckbox(sWonder, "CvScreensInterface", "WorldBuilderHandleEditCityBuildingCB", str(iWonder), 0, self.m_pActivePlot.getPlotCity().isHasBuilding(iWonder))
		if len(lTeamWonders) > 0:
			self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_TEAM_WONDERS",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleTeamWonderCB", "TeamWonder", 0)
			for i in lTeamWonders:
				sWonder = i[:i.find("_Platy_")]
				iWonder = int(i[i.find("_Platy_") +7:])
				self.m_tabCtrlEdit.addSectionCheckbox(sWonder, "CvScreensInterface", "WorldBuilderHandleEditCityBuildingCB", str(iWonder), 0, self.m_pActivePlot.getPlotCity().isHasBuilding(iWonder))
		if len(lWorldWonders) > 0:
			self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_WORLD_WONDERS",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleWorldWonderCB", "WorldWonder", 0)
			for i in lWorldWonders:
				sWonder = i[:i.find("_Platy_")]
				iWonder = int(i[i.find("_Platy_") +7:])
				self.m_tabCtrlEdit.addSectionCheckbox(sWonder, "CvScreensInterface", "WorldBuilderHandleEditCityBuildingCB", str(iWonder), 0, self.m_pActivePlot.getPlotCity().isHasBuilding(iWonder))
		for i in xrange(iNumColumns * iColumnLength - (len(lNationalWonders) + len(lTeamWonders) + len(lWorldWonders) + iWonderType)):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BACK",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "CityEditScreen", 0)
		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Wonder Edit Screen ##

## Religion Edit Screen ##
	def setReligionEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 4
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)
		iReligionPerColumn = (GC.getNumReligionInfos() +1) /iNumColumns
		if (GC.getNumReligionInfos() +1) % iNumColumns > 0:
			iReligionPerColumn += 1
		iColumnLength = iReligionPerColumn *4 -1
		iReligionColumn = (GC.getNumReligionInfos() +1) / iReligionPerColumn
		if (GC.getNumReligionInfos() +1) % iReligionPerColumn > 0:
			iReligionColumn += 1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)
		self.m_tabCtrlEdit.setSize(min(1000, iReligionColumn * 200), min(600, max(5, iColumnLength) * 42))

		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_RELIGION",()))
		lReligion = []
		for i in xrange(GC.getNumReligionInfos()):
			lReligion.append(GC.getReligionInfo(i).getDescription() + "_Platy_" + str(i))
		lReligion.sort()

		for i in xrange(len(lReligion)):
			sI = lReligion[i]
			sReligion = sI[:sI.find("_Platy_")]
			iReligion = int(sI[sI.find("_Platy_") +7:])
			self.m_tabCtrlEdit.addSectionLabel(sReligion, 0)
			self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_PRESENT",()), "CvScreensInterface", "WorldBuilderHandleCityEditReligionCB", str(iReligion), 0, self.m_pActivePlot.getPlotCity().isHasReligion(iReligion))
			self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_HOLY_CITY",()), "CvScreensInterface", "WorldBuilderHandleCityEditHolyCityCB", str(iReligion), 0, self.m_pActivePlot.getPlotCity().isHolyCityByType(iReligion))
			if (i+1) % iReligionPerColumn != 0:
				self.m_tabCtrlEdit.addSectionLabel(" ",  0)

		for i in xrange(iReligionColumn * iColumnLength - (GC.getNumReligionInfos()*4 - iReligionColumn + 3)):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_COMMANDS",()),) + (TRNSLTR.getText("TXT_KEY_WB_SPREAD_ALL",()),) + (TRNSLTR.getText("TXT_KEY_WB_CLEAR_ALL",()),) + (TRNSLTR.getText("TXT_KEY_WB_COPY_ALL",()),)
		self.m_tabCtrlEdit.addSectionDropdown("Religion Commands", strTest, "CvScreensInterface", "WorldBuilderHandleReligionCommandsCB", "Religion Commands", 0, 0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BACK",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "CityEditScreen", 0)

		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Religion Edit Screen ##

## Corporation Edit Screen ##
	def setCorporationEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 4

		self.m_tabCtrlEdit.setNumColumns(iNumColumns)
		iCorporationPerColumn = (GC.getNumCorporationInfos() +1) /iNumColumns
		if (GC.getNumCorporationInfos() +1) % iNumColumns > 0:
			iCorporationPerColumn += 1
		iColumnLength = iCorporationPerColumn *4 -1
		iCorporationColumn = (GC.getNumCorporationInfos() +1) / iCorporationPerColumn
		if (GC.getNumCorporationInfos() +1) % iCorporationPerColumn > 0:
			iCorporationColumn += 1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)
		self.m_tabCtrlEdit.setSize(min(1000, iCorporationColumn * 200), min(600, max(5, iColumnLength) * 42))

		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_CORPORATION",()))
		lCorporation = []
		for i in xrange(GC.getNumCorporationInfos()):
			lCorporation.append(GC.getCorporationInfo(i).getDescription() + "_Platy_" + str(i))
		lCorporation.sort()
		for i in xrange(len(lCorporation)):
			sI = lCorporation[i]
			sCorporation = sI[:sI.find("_Platy_")]
			iCorporation = int(sI[sI.find("_Platy_") +7:])
			self.m_tabCtrlEdit.addSectionLabel(sCorporation, 0)
			self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_PRESENT",()), "CvScreensInterface", "WorldBuilderHandleCityEditCorporationCB", str(iCorporation), 0, self.m_pActivePlot.getPlotCity().isHasCorporation(iCorporation))
			self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_HEADQUARTERS",()), "CvScreensInterface", "WorldBuilderHandleCityEditHeadquartersCB", str(iCorporation), 0, self.m_pActivePlot.getPlotCity().isHeadquartersByType(iCorporation))
			if (i+1) % iCorporationPerColumn != 0:
				self.m_tabCtrlEdit.addSectionLabel(" ",  0)

		for i in xrange(iCorporationColumn * iColumnLength - (GC.getNumCorporationInfos()*4 - iCorporationColumn + 3)):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_COMMANDS",()),) + (TRNSLTR.getText("TXT_KEY_WB_CLEAR_ALL",()),) + (TRNSLTR.getText("TXT_KEY_WB_COPY_ALL",()),)
		self.m_tabCtrlEdit.addSectionDropdown("Corporation Commands", strTest, "CvScreensInterface", "WorldBuilderHandleCorporationCommandsCB", "Corporation Commands", 0, 0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BACK",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "CityEditScreen", 0)

		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Corporation Edit Screen ##

## Free Specialists Edit Screen ##
	def setFreeSpecialistEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 4
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)

		iColumnLength = (GC.getNumSpecialistInfos() *2 +1) /iNumColumns
		if (GC.getNumSpecialistInfos() *2 +1) %iNumColumns > 0:
			iColumnLength += 1
		if iColumnLength %2 == 1:
			iColumnLength += 1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)
		self.m_tabCtrlEdit.setSize(min(1000, iNumColumns * 200), min(600, max(5, iColumnLength) * 42))

		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_FREE_SPECIALISTS",()))
		lSpecialist = []
		lGreatSpecialist = []
		for i in xrange(GC.getNumSpecialistInfos()):
			sSpecialist = GC.getSpecialistInfo(i).getType()
			if sSpecialist.find("GREAT") > -1:
				lGreatSpecialist.append(GC.getSpecialistInfo(i).getDescription() + "_Platy_" + str(i))
			else:
				lSpecialist.append(GC.getSpecialistInfo(i).getDescription() + "_Platy_" + str(i))
		lSpecialist.sort()
		lGreatSpecialist.sort()

		for i in lSpecialist:
			sSpecialist = i[:i.find("_Platy_")]
			iSpecialist = int(i[i.find("_Platy_") +7:])
			self.m_tabCtrlEdit.addSectionLabel(sSpecialist,  0)
			self.m_tabCtrlEdit.addSectionSpinner(sSpecialist, "CvScreensInterface", "WorldBuilderHandleCityEditSpecialistCB", str(iSpecialist), 0, 0, 100, 1, self.m_pActivePlot.getPlotCity().getFreeSpecialistCount(iSpecialist), 0, 0)
		for i in lGreatSpecialist:
			sSpecialist = i[:i.find("_Platy_")]
			iSpecialist = int(i[i.find("_Platy_") +7:])
			self.m_tabCtrlEdit.addSectionLabel(sSpecialist,  0)
			self.m_tabCtrlEdit.addSectionSpinner(sSpecialist, "CvScreensInterface", "WorldBuilderHandleCityEditSpecialistCB", str(iSpecialist), 0, 0, 100, 1, self.m_pActivePlot.getPlotCity().getFreeSpecialistCount(iSpecialist), 0, 0)

		for i in xrange(iNumColumns * iColumnLength - (GC.getNumSpecialistInfos() *2 +1)):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BACK",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "CityEditScreen", 0)

		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Free Specialists Edit Screen ##

## Great People Edit Screen ##
	def setGreatPeopleEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 4
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)

		lGreatPeople = []
		for i in xrange(GC.getNumUnitInfos()):
			if GC.getUnitInfo(i).isGoldenAge():
				lGreatPeople.append(GC.getUnitInfo(i).getDescription() + "_Platy_" + str(i))
		lGreatPeople.sort()

		iColumnLength = (len(lGreatPeople) *2 +1) /iNumColumns
		if (len(lGreatPeople) *2 +1) %iNumColumns > 0:
			iColumnLength += 1
		if iColumnLength %2 == 1:
			iColumnLength += 1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)
		self.m_tabCtrlEdit.setSize(min(1000, iNumColumns * 200), min(600, max(5, iColumnLength) * 40))

		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_GREAT_PEOPLE",()))
		for i in lGreatPeople:
			sGreatPeople = i[:i.find("_Platy_")]
			iGreatPeople = int(i[i.find("_Platy_") +7:])
			self.m_tabCtrlEdit.addSectionLabel(sGreatPeople,  0)
			self.m_tabCtrlEdit.addSectionSpinner(sGreatPeople, "CvScreensInterface", "WorldBuilderHandleCityEditGreatPeopleCB", str(iGreatPeople), 0, 0, 99999, 1, self.m_pActivePlot.getPlotCity().getGreatPeopleUnitProgress(iGreatPeople), 0, 0)
		for i in xrange(iColumnLength * iNumColumns - (len(lGreatPeople) *2 +1)):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BACK",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "CityEditScreen", 0)

		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Great People Edit Screen ##

## Free Bonus Edit Screen ##
	def setFreeBonusEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 5
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)

		iColumnLength = (GC.getNumBonusInfos() *2 +1) /iNumColumns
		if (GC.getNumBonusInfos() *2 +1) %iNumColumns > 0:
			iColumnLength += 1
		if iColumnLength %2 == 1:
			iColumnLength += 1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)
		self.m_tabCtrlEdit.setSize(min(1000, iNumColumns * 200), min(600, max(5, iColumnLength) * 35))

		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_FREE_BONUS",()))
		lBonus = []
		for i in xrange(GC.getNumBonusInfos()):
			lBonus.append(GC.getBonusInfo(i).getDescription() + "_Platy_" + str(i))
		lBonus.sort()

		for i in lBonus:
			sBonus = i[:i.find("_Platy_")]
			iBonus = int(i[i.find("_Platy_") +7:])
			self.m_tabCtrlEdit.addSectionLabel(sBonus,  0)
			self.m_tabCtrlEdit.addSectionSpinner(sBonus, "CvScreensInterface", "WorldBuilderHandleCityEditBonusCB", str(iBonus), 0, 0, 100, 1, self.m_pActivePlot.getPlotCity().getFreeBonus(iBonus), 0, 0)
		for i in xrange(iNumColumns * iColumnLength - (GC.getNumBonusInfos() *2 +1)):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BACK",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "CityEditScreen", 0)

		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Free Bonus Edit Screen ##

## Player Edit Screen ##
	def setPlayerEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()
		iNumColumns = 3
		iExtraSpace = max(0, GC.getNumCivicOptionInfos() - 6)
		iColumnLength = 14 + iExtraSpace
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_PLAYER_DATA",(self.m_iCurrentPlayer,)))
## Current Player ##
		strTest = ()
		iCount = 0
		for i in xrange(GC.getMAX_PLAYERS()):
			if GC.getPlayer(i).isEverAlive():
				strPlayerAliveStatus = GC.getPlayer(i).getName()
				if not GC.getPlayer(i).isAlive():
					strPlayerAliveStatus = strPlayerAliveStatus + " " + TRNSLTR.getText("TXT_KEY_WB_DEAD",())
				strTest = strTest + (strPlayerAliveStatus,)
				if i == self.m_iCurrentPlayer:
					iCurrentPlayer = iCount
				iCount += 1
		self.m_tabCtrlEdit.addSectionDropdown("Current Player", strTest, "CvScreensInterface", "WorldBuilderHandleCurrentPlayerEditPullDownCB", "PlayerEditCurrentPlayer", 0, iCurrentPlayer)
		pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
## Traits ##
		lTraits = []
		for i in xrange(GC.getNumTraitInfos()):
			if pPlayer.hasTrait(i):
				lTraits.append(i)
		sTrait = ""
		for i in xrange(len(lTraits)):
			sTrait += GC.getTraitInfo(lTraits[i]).getDescription()
			if i != len(lTraits) -1:
				sTrait += ", "
		if len(lTraits) == 0:
			sTrait = TRNSLTR.getText("TXT_KEY_WB_NONE",())
		self.m_tabCtrlEdit.addSectionLabel(sTrait,  0)
## Team ##
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_TEAM_DATA",(self.m_iCurrentTeam,))+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "TeamEditScreen", 0)
## Current Era ##
		strTest = ()
		for i in xrange(GC.getNumEraInfos()):
			strTest = strTest + (TRNSLTR.getText("TXT_KEY_WB_ERA",(GC.getEraInfo(i).getDescription(),)),)
		self.m_tabCtrlEdit.addSectionDropdown("Current Era", strTest, "CvScreensInterface", "WorldBuilderHandleCurrentEraEditPullDownCB", "PlayerEditCurrentEra", 0, pPlayer.getCurrentEra())
## Commerce Sliders ##
		for i in xrange(4):
			self.m_tabCtrlEdit.addSectionCheckbox(GC.getCommerceInfo(i).getDescription(), "CvScreensInterface", "WorldBuilderHandleTeamEditCommerceFlexibleCB", str(i), 0, pPlayer.isCommerceFlexible(i))
			self.m_tabCtrlEdit.addSectionSpinner("PlayerEditCommercePercentCB", "CvScreensInterface", "WorldBuilderHandlePlayerEditCommercePercentCB", str(i), 0, 0, 100, 10, pPlayer.getCommercePercent(i), 0, 0)
## Player Script ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_SCRIPT_DATA",()),  0)
		self.m_tabCtrlEdit.addSectionEditCtrl(pPlayer.getScriptData(), "CvScreensInterface", "WorldBuilderHandleEditScriptCB", "PlayerEditScript", 0)
##
		for i in xrange(iExtraSpace):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
## Current Research ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_CURRENT_RESEARCH",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_NONE",()),)
		iCurrentTech = 0
		iCount = 0
		for i in xrange(GC.getNumTechInfos()):
			if pPlayer.canResearch(i):
				iCount += 1
				strTest = strTest + (GC.getTechInfo(i).getDescription(),)
				if pPlayer.getCurrentResearch() == i:
					iCurrentTech = iCount
		self.m_tabCtrlEdit.addSectionDropdown("Current Tech", strTest, "CvScreensInterface", "WorldBuilderHandleCurrentTechEditPullDownCB", "CurrentTechEditPullDown", 0, iCurrentTech)
		if pPlayer.getCurrentResearch() == -1:
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
		else:
			self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_PROGRESS",(GC.getTeam(self.m_iCurrentTeam).getResearchCost(pPlayer.getCurrentResearch()),)),  0)
			self.m_tabCtrlEdit.addSectionSpinner("TeamEditResearchProgressCB", "CvScreensInterface", "WorldBuilderHandleTeamEditResearchProgressCB", "TeamEditResearchProgress", 0, 0, GC.getTeam(self.m_iCurrentTeam).getResearchCost(pPlayer.getCurrentResearch()), 1, GC.getTeam(self.m_iCurrentTeam).getResearchProgress(pPlayer.getCurrentResearch()), 0, 0)
## Golden Age ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_GOLDEN_AGE",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("PlayerEditGoldenAgeCB", "CvScreensInterface", "WorldBuilderHandlePlayerEditGoldenAgeCB", "PlayerEditGoldenAge", 0, 0, 1000, 1, pPlayer.getGoldenAgeTurns(), 0, 0)
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_GOLDEN_AGE_UNITS",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("PlayerEditGoldenAgeUnitsCB", "CvScreensInterface", "WorldBuilderHandlePlayerEditGoldenAgeUnitsCB", "PlayerEditGoldenAgeUnits", 0, 1, 10, 1, pPlayer.unitsRequiredForGoldenAge(), 0, 0)
## Anarchy ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_ANARCHY",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("PlayerEditAnarchyCB", "CvScreensInterface", "WorldBuilderHandlePlayerEditAnarchyCB", "PlayerEditAnarchy", 0, 0, 1000, 1, pPlayer.getAnarchyTurns(), 0, 0)
## Combat Experience ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_COMBAT_EXPERIENCE",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("PlayerEditCombatExperienceCB", "CvScreensInterface", "WorldBuilderHandlePlayerEditCombatExperienceCB", "PlayerEditCombatExperience", 0, 0, 1000, 1, pPlayer.getCombatExperience(), 0, 0)
## Gold ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_GOLD",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("PlayerEditGoldCB", "CvScreensInterface", "WorldBuilderHandlePlayerEditGoldCB", "PlayerEditGold", 0, -10000, 1000000, 1, pPlayer.getGold(), 0, 0)
##
		for i in xrange(iExtraSpace):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
## State Religion ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_STATE_RELIGION",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_NONE",()),)
		for i in xrange(GC.getNumReligionInfos()):
			strTest = strTest + (GC.getReligionInfo(i).getDescription(),)
		self.m_tabCtrlEdit.addSectionDropdown("State Religion", strTest, "CvScreensInterface", "WorldBuilderHandleStateReligionEditPullDownCB", "StateReligionEditPullDown", 0, pPlayer.getStateReligion() + 1)
## State Religion Unit Production ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_STATE_RELIGION_UNIT",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("PlayerEditStateReligionUnitProductionCB", "CvScreensInterface", "WorldBuilderHandlePlayerEditStateReligionUnitProductionCB", "PlayerEditStateReligionUnitProduction", 0, 0, 1000, 1, pPlayer.getStateReligionUnitProductionModifier(), 0, 0)
## State Religion Building Production ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_STATE_RELIGION_BUILDING",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("PlayerEditStateReligionBuildingProductionCB", "CvScreensInterface", "WorldBuilderHandlePlayerEditStateReligionBuildingProductionCB", "PlayerEditStateReligionBuildingProduction", 0, 0, 1000, 1, pPlayer.getStateReligionBuildingProductionModifier(), 0, 0)
## Civics ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_CIVICS",()),  0)
		for iCivicOption in xrange(GC.getNumCivicOptionInfos()):
			strTest = ()
			iCount = 0
			for i in xrange(GC.getNumCivicInfos()):
				if GC.getCivicInfo(i).getCivicOptionType() == iCivicOption:
					strTest = strTest + (GC.getCivicInfo(i).getDescription(),)
					if pPlayer.isCivic(i):
						iCivic = iCount
					iCount = iCount + 1
			self.m_tabCtrlEdit.addSectionDropdown(str(iCivicOption), strTest, "CvScreensInterface", "WorldBuilderHandlePlayerEditCivicCB", str(iCivicOption), 0, iCivic)
##
		for i in xrange(iColumnLength - 8 - GC.getNumCivicOptionInfos()):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
## Kill ##
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_KILL",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleKillCB", "Player", 0)

		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Player Edit Screen ##

## Team Edit Screen ##
	def setTeamEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()
		self.m_tabCtrlEdit.setNumColumns(3)
		self.m_tabCtrlEdit.setColumnLength(12)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_TEAM_DATA",(self.m_iCurrentTeam,)))
## Team ##
		strTest = ()
		iCount = 0
		for i in xrange(GC.getMAX_TEAMS()):
			if GC.getTeam(i).isAlive():
				strTest = strTest + (GC.getTeam(i).getName(),)
				if i == self.m_iCurrentTeam:
					iTeam = iCount
				iCount = iCount + 1
		self.m_tabCtrlEdit.addSectionDropdown("Team", strTest, "CvScreensInterface", "WorldBuilderHandleTeamEditPullDownCB", "TeamEditPullDown", 0, iTeam)
		pTeam = GC.getTeam(self.m_iCurrentTeam)
## Team Leader ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_LEADER",(GC.getPlayer(pTeam.getLeaderID()).getName(),)),  0)
## Projects ##
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_PROJECTS",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "ProjectEditScreen", 0)
## Technology ##
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_TECHNOLOGY",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "TechEditScreen", 0)
## Guaranteed Eligiblity ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_GUARANTEED_ELIGIBILITY",()),  0)
		for i in xrange(GC.getNumVoteSourceInfos()):
			self.m_tabCtrlEdit.addSectionCheckbox(GC.getVoteSourceInfo(i).getDescription(), "CvScreensInterface", "WorldBuilderHandleTeamEditForceTeamVoteCB", str(i), 0, pTeam.isForceTeamVoteEligible(i))
		for i in xrange(3 - GC.getNumVoteSourceInfos()):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
## Enemy War Weariness ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_ENEMY_WAR_WEARINESS",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("TeamEditEnemyWarWearinessCB", "CvScreensInterface", "WorldBuilderHandleTeamEditEnemyWarWearinessCB", "TeamEditEnemyWarWeariness", 0, 0, 1000, 1, pTeam.getEnemyWarWearinessModifier(), 0, 0)
## Nuke Interception ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_NUKE_INTERCEPTION",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("TeamEditNukeInterceptionCB", "CvScreensInterface", "WorldBuilderHandleTeamEditNukeInterceptionCB", "TeamEditNukeInterception", 0, -100, 100, 1, pTeam.getNukeInterception(), 0, 0)
## Merge Team ##
		strTest = (TRNSLTR.getText("TXT_KEY_WB_MERGE_TEAM",()),)
		for i in xrange(GC.getMAX_TEAMS()):
			if GC.getTeam(i).isAlive():
				if i == self.m_iCurrentTeam: continue
				strTest = strTest + (GC.getTeam(i).getName(),)
		self.m_tabCtrlEdit.addSectionDropdown("AddTeam", strTest, "CvScreensInterface", "WorldBuilderHandleAddTeamCB", "AddTeam", 0, 0)
## Improvement Extra Yield ##
		strTest = ()
		lImprovements = []
		for i in xrange(GC.getNumImprovementInfos()):
			if GC.getImprovementInfo(i).isGraphicalOnly(): continue
			strTest = strTest + (GC.getImprovementInfo(i).getDescription(),)
			lImprovements.append(i)
		self.m_tabCtrlEdit.addSectionDropdown("Improvement", strTest, "CvScreensInterface", "WorldBuilderHandleImprovementEditPullDownCB", "ImprovementEditPullDown", 0, self.m_iImprovement)
		strTest = ()
		for i in xrange(3):
			strTest = strTest + (TRNSLTR.getText("TXT_KEY_WB_IMPROVEMENT_YIELD",(GC.getYieldInfo(i).getDescription(),)),)
		self.m_tabCtrlEdit.addSectionDropdown("Yield", strTest, "CvScreensInterface", "WorldBuilderHandleYieldEditPullDownCB", "YieldEditPullDown", 0, self.m_iYield)
		self.m_tabCtrlEdit.addSectionSpinner("TeamEditImprovementYieldCB", "CvScreensInterface", "WorldBuilderHandleTeamEditImprovementYieldCB", "TeamEditImprovementYield", 0, 0, 10, 1, pTeam.getImprovementYieldChange(lImprovements[self.m_iImprovement], self.m_iYield), 0, 0)
## Domain Moves ##
		strTest = ()
		for i in xrange(3):
			sDomain = GC.getDomainInfo(i).getDescription()
			sDomain = sDomain[:sDomain.find(" ")]
			strTest = strTest + (TRNSLTR.getText("TXT_KEY_WB_DOMAIN_MOVES",(sDomain,)),)
		self.m_tabCtrlEdit.addSectionDropdown("TeamSelectDomain", strTest, "CvScreensInterface", "WorldBuilderHandleDomainEditPullDownCB", "DomainEditPullDown", 0, self.m_iDomain)
		self.m_tabCtrlEdit.addSectionSpinner("TeamEditDomainMovesCB", "CvScreensInterface", "WorldBuilderHandleTeamEditDomainMovesCB", "TeamEditDomainMoves", 0, 0, 10, 1, pTeam.getExtraMoves(self.m_iDomain), 0, 0)
## Route Change ##
		strTest = ()
		for i in xrange(GC.getNumRouteInfos()):
			strTest = strTest + (GC.getRouteInfo(i).getDescription() + " " + TRNSLTR.getText("TXT_KEY_WB_ROUTE_CHANGE",()),)
		self.m_tabCtrlEdit.addSectionDropdown("Route", strTest, "CvScreensInterface", "WorldBuilderHandleRouteEditPullDownCB", "RouteEditPullDown", 0, self.m_iRoute)
		self.m_tabCtrlEdit.addSectionSpinner("TeamEditRouteChangeCB", "CvScreensInterface", "WorldBuilderHandleTeamEditRouteChangeCB", "TeamEditRouteChange", 0, -100, 100, 1, pTeam.getRouteChange(self.m_iRoute), 0, 0)
## Tech Sharing ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_KNOWN_CIVS",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("TeamEditKnownCivsCB", "CvScreensInterface", "WorldBuilderHandleTeamEditKnownCivsCB", "TeamEditKnownCivs", 0, 1, 100, 1, self.m_iTechShareCivs, 0, 0)
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_TECH_SHARE",()), "CvScreensInterface", "WorldBuilderHandleTeamEditTechShareCB", "TeamEditTechShare", 0, pTeam.isTechShare(self.m_iTechShareCivs -1))
## Team Abilities ##
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_MAP_CENTERING",()), "CvScreensInterface", "WorldBuilderHandleTeamEditMapCenteringCB", "TeamEditMapCentering", 0, pTeam.isMapCentering())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_WATER_WORK",()), "CvScreensInterface", "WorldBuilderHandleTeamEditWaterWorkCB", "TeamEditWaterWork", 0, pTeam.isWaterWork())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_EXTRA_WATER_SIGHT",()), "CvScreensInterface", "WorldBuilderHandleTeamEditExtraWaterSeeFromCB", "TeamEditExtraWaterSeeFrom", 0, pTeam.isExtraWaterSeeFrom())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_BRIDGE_BUILDING",()), "CvScreensInterface", "WorldBuilderHandleTeamEditBridgeBuildingCB", "TeamEditBridgeBuilding", 0, pTeam.isBridgeBuilding())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_IRRIGATION",()), "CvScreensInterface", "WorldBuilderHandleTeamEditIrrigationCB", "TeamEditIrrigation", 0, pTeam.isIrrigation())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_IGNORE_IRRIGATION",()), "CvScreensInterface", "WorldBuilderHandleTeamEditIgnoreIrrigationCB", "TeamEditIgnoreIrrigation", 0, pTeam.isIgnoreIrrigation())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_GOLD_TRADING",()), "CvScreensInterface", "WorldBuilderHandleTeamEditGoldTradingCB", "TeamEditGoldTrading", 0, pTeam.isGoldTrading())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_TECH_TRADING",()), "CvScreensInterface", "WorldBuilderHandleTeamEditTechTradingCB", "TeamEditTechTrading", 0, pTeam.isTechTrading())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_MAP_TRADING",()), "CvScreensInterface", "WorldBuilderHandleTeamEditMapTradingCB", "TeamEditMapTrading", 0, pTeam.isMapTrading())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_OPEN_BORDERS",()), "CvScreensInterface", "WorldBuilderHandleTeamEditOpenBordersTradingCB", "TeamEditOpenBordersTrading", 0, pTeam.isOpenBordersTrading())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_PERMANENT_ALLIANCE",()), "CvScreensInterface", "WorldBuilderHandleTeamEditPermanentAllianceTradingCB", "TeamEditPermanentAllianceTrading", 0, pTeam.isPermanentAllianceTrading())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_DEFENSIVE_PACT",()), "CvScreensInterface", "WorldBuilderHandleTeamEditDefensivePactTradingCB", "TeamEditDefensivePactTrading", 0, pTeam.isDefensivePactTrading())
		self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_VASSAL_TRADING",()), "CvScreensInterface", "WorldBuilderHandleTeamEditVassalTradingCB", "TeamEditVassalTrading", 0, pTeam.isVassalStateTrading())

		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Team Edit Screen ##

## Project Edit Screen ##
	def setProjectEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 5
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_PROJECTS",()))

		iProjectPerColumn = (GC.getNumProjectInfos() +1) /iNumColumns
		if (GC.getNumProjectInfos() +1) % iNumColumns > 0:
			iProjectPerColumn += 1
		iColumnLength = iProjectPerColumn *2
		iProjectColumn = (GC.getNumProjectInfos() +1) / iProjectPerColumn
		if (GC.getNumProjectInfos() +1) % iProjectPerColumn > 0:
			iProjectColumn += 1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)
		self.m_tabCtrlEdit.setSize(min(1000, iProjectColumn * 200), min(700, max(5, iColumnLength) * 42))

		lProject = []
		for i in xrange(GC.getNumProjectInfos()):
			lProject.append(GC.getProjectInfo(i).getDescription() + "_Platy_" + str(i))
		lProject.sort()

		for i in lProject:
			sProject = i[:i.find("_Platy_")]
			iProject = int(i[i.find("_Platy_") +7:])
			self.m_tabCtrlEdit.addSectionLabel(sProject,  0)
			self.m_tabCtrlEdit.addSectionSpinner(sProject, "CvScreensInterface", "WorldBuilderHandleEditTeamProjectCB", str(iProject), 0, 	0, max(1, GC.getProjectInfo(iProject).getVictoryThreshold(GC.getInfoTypeForString("VICTORY_SPACE_RACE"))), 1, GC.getTeam(self.m_iCurrentTeam).getProjectCount(iProject), 0, 0)

		for i in xrange(iProjectColumn * iColumnLength - (GC.getNumProjectInfos() *2 +1)):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BACK",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "TeamEditScreen", 0)

		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Project Edit Screen ##

## Technology Edit Screen ##
	def setTechnologyEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 7
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_TECHNOLOGY",()))
		iEraPerColumn = (GC.getNumEraInfos() + 6) / iNumColumns
		iMax = 0
		for iEra in xrange(0, GC.getNumEraInfos(), iEraPerColumn):
			iCount = 0
			for i in xrange(GC.getNumTechInfos()):
				if GC.getTechInfo(i).getEra() >= iEra and GC.getTechInfo(i).getEra() < (iEra + iEraPerColumn):
					iCount += 1
			iMax = max(iCount, iMax)
		self.m_tabCtrlEdit.setColumnLength(iMax +iEraPerColumn *2 -1)
		self.m_tabCtrlEdit.setSize(1000, 600)

		iCount = 0
		for iEra in xrange(GC.getNumEraInfos()):
			self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_ERA",(GC.getEraInfo(iEra).getDescription(),)),  0)
			lTech = []
			for i in xrange(GC.getNumTechInfos()):
				if GC.getTechInfo(i).getEra() == iEra:
					lTech.append(GC.getTechInfo(i).getDescription() + "_Platy_" + str(i))
					iCount += 1
			lTech.sort()
			for i in lTech:
				sTech = i[:i.find("_Platy_")]
				iTech = int(i[i.find("_Platy_") +7:])
				self.m_tabCtrlEdit.addSectionCheckbox(sTech, "CvScreensInterface", "WorldBuilderHandleEditTeamTechnologyCB", str(iTech), 0, GC.getTeam(self.m_iCurrentTeam).isHasTech(iTech))
			if iEra == GC.getNumEraInfos() -1:
				for i in xrange(iMax - iCount -3):
					self.m_tabCtrlEdit.addSectionLabel(" ",  0)
			elif (iEra +1) % iEraPerColumn > 0:
				self.m_tabCtrlEdit.addSectionLabel(" ",  0)
			else:
				for i in xrange(iMax - iCount):
					self.m_tabCtrlEdit.addSectionLabel(" ",  0)
				iCount = 0

		strTest = (TRNSLTR.getText("TXT_KEY_WB_ADD_ERA",()),)
		for i in xrange(GC.getNumEraInfos()):
			strTest = strTest + (TRNSLTR.getText("TXT_KEY_WB_ERA", (GC.getEraInfo(i).getDescription(),)),)
		strTest = strTest + (TRNSLTR.getText("TXT_KEY_WB_ALL_TECHS",()),)
		self.m_tabCtrlEdit.addSectionDropdown("Tech By Era", strTest, "CvScreensInterface", "WorldBuilderHandleTechByEraPullDownCB", "TechByEraPullDown", 0, 0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_REMOVE_ERA",()),)
		for i in xrange(GC.getNumEraInfos()):
			strTest = strTest + (TRNSLTR.getText("TXT_KEY_WB_ERA", (GC.getEraInfo(i).getDescription(),)),)
		strTest = strTest + (TRNSLTR.getText("TXT_KEY_WB_ALL_TECHS",()),)
		self.m_tabCtrlEdit.addSectionDropdown("Tech By Era", strTest, "CvScreensInterface", "WorldBuilderHandleRemoveTechByEraPullDownCB", "RemoveTechByEraPullDown", 0, 0)
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_BACK",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "TeamEditScreen", 0)

		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Technology Edit Screen ##

## Add New Player ##
	def AddNewPlayer(self, bInitial):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		self.m_tabCtrlEdit.setNumColumns(1)
		self.m_tabCtrlEdit.setColumnLength(10)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_ADD_NEW_PLAYER",()))

		if bInitial:
			self.m_iNewCivilization = -1
			self.m_iNewLeaderType = -1
		strTest = (TRNSLTR.getText("TXT_KEY_WB_CHOOSE_CIVILIZATION",()),)
		iSelectedCiv = 0
		iCount = 1
		for i in xrange(GC.getNumCivilizationInfos()):
			if not CyGame().isCivEverActive(i):
				strTest = strTest + (GC.getCivilizationInfo(i).getDescription(),)
				if i == self.m_iNewCivilization:
					iSelectedCiv = iCount
				iCount+= 1
		self.m_tabCtrlEdit.addSectionDropdown("New Civilization", strTest, "CvScreensInterface", "WorldBuilderHandleAddPlayerCivilizationCB", "AddPlayerCivilization", 0, iSelectedCiv)
		if self.m_iNewCivilization > -1:
			strTest = (TRNSLTR.getText("TXT_KEY_WB_CHOOSE_PLAYER",()),)
			iSelectedLeaderType = 0
			iCount = 1
			for i in xrange(GC.getNumLeaderHeadInfos()):
				if not CyGame().isLeaderEverActive(i):
					if not CyGame().isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV):
						if not GC.getCivilizationInfo(self.m_iNewCivilization).isLeaders(i): continue
					strTest = strTest + (GC.getLeaderHeadInfo(i).getDescription(),)
					if i == self.m_iNewLeaderType:
						iSelectedLeaderType = iCount
					iCount+= 1
			self.m_tabCtrlEdit.addSectionDropdown("New Leader", strTest, "CvScreensInterface", "WorldBuilderHandleAddPlayerLeaderTypeCB", "AddPlayerLeaderType", 0, iSelectedLeaderType)
			if self.m_iNewLeaderType > -1:
				lTraits = []
				for i in xrange(GC.getNumTraitInfos()):
					if GC.getLeaderHeadInfo(self.m_iNewLeaderType).hasTrait(i):
						lTraits.append(i)
				sTrait = ""
				for j in xrange(len(lTraits)):
					sTrait += GC.getTraitInfo(lTraits[j]).getDescription()
					if j != len(lTraits) -1:
						sTrait += ", "
				if len(lTraits) == 0:
					sTrait = TRNSLTR.getText("TXT_KEY_WB_NONE",())
				self.m_tabCtrlEdit.addSectionLabel(sTrait,  0)
				self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_CREATE_PLAYER",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleCreatePlayerCB", "Create Player", 0)

		if (not self.m_tabCtrlEdit.isNone()):
			print("Enabling map control 5")
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Add New Player ##

## Game Data Screen ##
	def setGameEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()
		self.m_tabCtrlEdit.setNumColumns(2)
		self.m_tabCtrlEdit.setColumnLength(15)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_GAME_DATA",()))
## Start Year ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_START_YEAR",()),  0)
		self.m_tabCtrlEdit.addSectionSpinner("Start Year", "CvScreensInterface", "WorldBuilderHandleGameEditStartYearCB", "GameEditStartYear", 0, -10000, 10000, 100, CyGame().getStartYear(), 0, 0)
## Global Defines ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_GLOBAL_DEFINE",()),  0)
		lGlobalDefineINT = ["MOVE_DENOMINATOR", "MIN_CITY_RANGE", "MAX_WORLD_WONDERS_PER_CITY", "MAX_TEAM_WONDERS_PER_CITY", "MAX_NATIONAL_WONDERS_PER_CITY", \
				"MAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC", "INITIAL_CITY_POPULATION", "BASE_CITY_GROWTH_THRESHOLD", "CITY_GROWTH_MULTIPLIER", "FOOD_CONSUMPTION_PER_POPULATION", \
				"NEW_HURRY_MODIFIER", "CONSCRIPT_MIN_CITY_POPULATION", "CONSCRIPT_POPULATION_PER_COST", "CONSCRIPT_MIN_CULTURE_PERCENT", "FRESH_WATER_HEALTH_CHANGE", \
				"POWER_HEALTH_CHANGE", "DIRTY_POWER_HEALTH_CHANGE", "MAX_DISTANCE_CITY_MAINTENANCE", "MAX_TRADE_ROUTES", \
				"MAX_INTERCEPTION_DAMAGE", "MIN_INTERCEPTION_DAMAGE", "RANGE_COMBAT_DAMAGE", "COLLATERAL_COMBAT_DAMAGE", "MAX_HIT_POINTS", \
				"HILLS_EXTRA_DEFENSE", "RIVER_ATTACK_MODIFIER", "AMPHIB_ATTACK_MODIFIER", "ENEMY_HEAL_RATE", "NEUTRAL_HEAL_RATE", \
				"FRIENDLY_HEAL_RATE", "CITY_HEAL_RATE", "GREAT_PEOPLE_THRESHOLD", "GREAT_GENERALS_THRESHOLD", "BASE_SPY_SABOTAGE_COST", \
				"BASE_SPY_DESTROY_COST", "INITIAL_BASE_FREE_UNITS", "INITIAL_BASE_FREE_MILITARY_UNITS", "INITIAL_FREE_UNITS_POPULATION_PERCENT", "INITIAL_FREE_MILITARY_UNITS_POPULATION_PERCENT", \
				"INITIAL_GOLD_PER_UNIT", "INITIAL_FREE_OUTSIDE_UNITS", "INITIAL_OUTSIDE_UNIT_GOLD_PERCENT", "ANIMAL_MAX_XP_VALUE", "BARBARIAN_MAX_XP_VALUE", \
				"BASE_GOLDEN_AGE_UNITS", "GOLDEN_AGE_UNITS_MULTIPLIER", "GOLDEN_AGE_LENGTH", "GOLDEN_AGE_GREAT_PEOPLE_MODIFIER", "HILLS_EXTRA_MOVEMENT", \
				"FEATURE_GROWTH_MODIFIER", "ROUTE_FEATURE_GROWTH_MODIFIER", "THEIR_POPULATION_TRADE_PERCENT", "CAPITAL_TRADE_MODIFIER", "OVERSEAS_TRADE_MODIFIER", \
				"FOREIGN_TRADE_MODIFIER", "FOREIGN_TRADE_FULL_CREDIT_PEACE_TURNS", "OUR_POPULATION_TRADE_MODIFIER", "OUR_POPULATION_TRADE_MODIFIER_OFFSET", "TRADE_PROFIT_PERCENT", \
				"EXTRA_YIELD", "FORTIFY_MODIFIER_PER_TURN", "MAX_FORTIFY_TURNS", "NUKE_FALLOUT_PROB", "NUKE_UNIT_DAMAGE_BASE", "NUKE_UNIT_DAMAGE_RAND_1", \
				"NUKE_UNIT_DAMAGE_RAND_2", "NUKE_NON_COMBAT_DEATH_THRESHOLD", "NUKE_BUILDING_DESTRUCTION_PROB", "NUKE_POPULATION_DEATH_BASE", "NUKE_POPULATION_DEATH_RAND_1", \
				"NUKE_POPULATION_DEATH_RAND_2", "GLOBAL_WARMING_PROB", "GLOBAL_WARMING_FOREST", "GLOBAL_WARMING_UNHEALTH_WEIGHT", "GLOBAL_WARMING_NUKE_WEIGHT", \
				"TECH_COST_EXTRA_TEAM_MEMBER_MODIFIER", "TECH_COST_TOTAL_KNOWN_TEAM_MODIFIER", "TECH_COST_KNOWN_PREREQ_MODIFIER", "PEACE_TREATY_LENGTH", "MAX_ANARCHY_TURNS", \
				"MIN_REVOLUTION_TURNS", "MIN_CONVERSION_TURNS", "OCCUPATION_CULTURE_PERCENT_THRESHOLD", "BASE_OCCUPATION_TURNS", "OCCUPATION_TURNS_POPULATION_PERCENT", \
				"BASE_REVOLT_OCCUPATION_TURNS", "REVOLT_OCCUPATION_TURNS_PERCENT", "REVOLT_TOTAL_CULTURE_MODIFIER", "REVOLT_OFFENSE_STATE_RELIGION_MODIFIER", "REVOLT_DEFENSE_STATE_RELIGION_MODIFIER", \
				"REVOLT_TEST_PROB", "NUM_WARNING_REVOLTS", "BASE_REVOLT_FREE_UNITS", "REVOLT_FREE_UNITS_PERCENT", "NO_MILITARY_PERCENT_ANGER", \
				"AT_WAR_CULTURE_ANGER_MODIFIER", "CULTURE_PERCENT_ANGER", "RELIGION_PERCENT_ANGER", "HURRY_POP_ANGER", "HURRY_ANGER_DIVISOR", \
				"CONSCRIPT_POP_ANGER", "CONSCRIPT_ANGER_DIVISOR", "DEFY_RESOLUTION_POP_ANGER", "DEFY_RESOLUTION_ANGER_DIVISOR", "TEMP_HAPPY", \
				"BASE_WAR_WEARINESS_MULTIPLIER", "FORCED_WAR_WAR_WEARINESS_MODIFIER", "MULTIPLAYER_WAR_WEARINESS_MODIFIER", "PERCENT_ANGER_DIVISOR", "BASE_FEATURE_PRODUCTION_PERCENT", \
				"FEATURE_PRODUCTION_PERCENT_MULTIPLIER", "DIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT", "UNIT_PRODUCTION_PERCENT", "BUILDING_PRODUCTION_PERCENT", "PROJECT_PRODUCTION_PERCENT", \
				"MAXED_UNIT_GOLD_PERCENT", "MAXED_BUILDING_GOLD_PERCENT", "MAXED_PROJECT_GOLD_PERCENT", "MAX_CITY_DEFENSE_DAMAGE", "CITY_DEFENSE_DAMAGE_HEAL_RATE", \
				"UPKEEP_POPULATION_OFFSET", "UPKEEP_CITY_OFFSET", "CIRCUMNAVIGATE_FREE_MOVES", "BARBARIAN_FREE_TECH_PERCENT", "RAZING_CULTURAL_PERCENT_THRESHOLD", \
				"BASE_CAPTURE_GOLD", "CAPTURE_GOLD_PER_POPULATION", "CAPTURE_GOLD_RAND1", "CAPTURE_GOLD_RAND2", "CAPTURE_GOLD_MAX_TURNS", \
				"CITY_BARBARIAN_DEFENSE_MODIFIER", "MIN_EXPERIENCE_PER_COMBAT", "MAX_EXPERIENCE_PER_COMBAT", "EXPERIENCE_FROM_WITHDRAWL", "MAX_EXPERIENCE_AFTER_UPGRADE", \
				"BUILDING_PRODUCTION_DECAY_TIME", "BUILDING_PRODUCTION_DECAY_PERCENT", "UNIT_PRODUCTION_DECAY_TIME", "UNIT_PRODUCTION_DECAY_PERCENT", "BASE_UNIT_UPGRADE_COST", \
				"UNIT_UPGRADE_COST_PER_PRODUCTION", "WAR_SUCCESS_DEFENDING", "WAR_SUCCESS_ATTACKING", "WAR_SUCCESS_UNIT_CAPTURING", "WAR_SUCCESS_CITY_CAPTURING", \
				"WAR_SUCCESS_NUKE", "DIPLO_VOTE_SECRETARY_GENERAL_INTERVAL", "TEAM_VOTE_MIN_CANDIDATES", "DIPLOMACY_VALUE_REMAINDER", "INITIAL_STATE_RELIGION_HAPPINESS", \
				"INITIAL_NON_STATE_RELIGION_HAPPINESS", "RELIGION_SPREAD_DISTANCE_DIVISOR", "RELIGION_SPREAD_RAND", "CORPORATION_FOREIGN_SPREAD_COST_PERCENT", "HOLY_CITY_INFLUENCE", \
				"WE_LOVE_THE_KING_RAND", "WE_LOVE_THE_KING_POPULATION_MIN_POPULATION", "VASSAL_REVOLT_OWN_LOSSES_FACTOR", "VASSAL_REVOLT_MASTER_LOSSES_FACTOR", "VASSAL_HAPPINESS", \
				"FREE_VASSAL_LAND_PERCENT", "FREE_VASSAL_POPULATION_PERCENT", "WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT", "COMBAT_EXPERIENCE_IN_BORDERS_PERCENT", "MAX_WITHDRAWAL_PROBABILITY", \
				"MAX_INTERCEPTION_PROBABILITY", "MAX_EVASION_PROBABILITY", "PLAYER_ALWAYS_RAZES_CITIES", "MIN_WATER_SIZE_FOR_OCEAN", "MIN_CITY_ATTACK_MODIFIER_FOR_SIEGE_TOWER", \
				"ESPIONAGE_CITY_POP_EACH_MOD", "ESPIONAGE_CITY_TRADE_ROUTE_MOD", "ESPIONAGE_CITY_RELIGION_STATE_MOD", "ESPIONAGE_CITY_HOLY_CITY_MOD", "ESPIONAGE_CULTURE_MULTIPLIER_MOD", \
				"ESPIONAGE_DISTANCE_MULTIPLIER_MOD", "ESPIONAGE_SPENDING_MULTIPLIER", "ESPIONAGE_EACH_TURN_UNIT_COST_DECREASE", "ESPIONAGE_INTERCEPT_SPENDING_MAX", "ESPIONAGE_INTERCEPT_COUNTERSPY", \
				"ESPIONAGE_INTERCEPT_COUNTERESPIONAGE_MISSION", "ESPIONAGE_INTERCEPT_RECENT_MISSION", "ESPIONAGE_SPY_REVEAL_IDENTITY_PERCENT", "ESPIONAGE_SPY_INTERCEPT_MOD", "ESPIONAGE_SPY_NO_INTRUDE_INTERCEPT_MOD", \
				"ESPIONAGE_SPY_MISSION_ESCAPE_MOD", "CITY_AIR_UNIT_CAPACITY", "COLONY_NUM_FREE_DEFENDERS", "SHIP_BLOCKADE_RANGE", "NO_ESPIONAGE_CULTURE_LEVEL_MODIFIER"]
		lGlobalDefineINT.sort()

		strTest = ()
		for i in lGlobalDefineINT:
			strTest += (i,)
		self.m_tabCtrlEdit.addSectionDropdown("Global Define", strTest, "CvScreensInterface", "WorldBuilderHandleEditGlobalDefineCB", "EditGlobal Define", 0, self.m_iGlobalDefine)
		self.m_tabCtrlEdit.addSectionSpinner("Edit Global Define", "CvScreensInterface", "WorldBuilderHandleGlobalDefineINTCB", lGlobalDefineINT[self.m_iGlobalDefine], 0, -10000, 10000, 1, GC.getDefineINT(lGlobalDefineINT[self.m_iGlobalDefine]), 0 , 0)
## Game Options ##
		self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_GAME_OPTION",())+"\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "GameOptionEditScreen", 0)
## Game Script ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_SCRIPT_DATA",()),  0)
		self.m_tabCtrlEdit.addSectionEditCtrl(CyGame().getScriptData(), "CvScreensInterface", "WorldBuilderHandleEditScriptCB", "GameEditScript", 0)
##
		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Game Data Screen ##

## Game Option Screen ##
	def setGameOptionEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		iNumColumns = 4
		self.m_tabCtrlEdit.setNumColumns(iNumColumns)
		iColumnLength = GC.getNumGameOptionInfos() /iNumColumns
		if GC.getNumGameOptionInfos() %iNumColumns > 0:
			iColumnLength += 1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)

		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_GAME_OPTION",()))
		for i in xrange(GC.getNumGameOptionInfos()):
			self.m_tabCtrlEdit.addSectionCheckbox(GC.getGameOptionInfo(i).getDescription(), "CvScreensInterface", "WorldBuilderHandleEditGameOptionCB", str(i), 0, CyGame().isOption(i))
		for i in xrange(iNumColumns * iColumnLength - GC.getNumGameOptionInfos()):
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)

		if (not self.m_tabCtrlEdit.isNone()):
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Game Option Screen ##

## Plot Edit Screen ##
	def setPlotEditInfo(self, bNewPlot):

		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()
		if bNewPlot:
			self.m_pActivePlot = self.m_pCurrentPlot

		self.m_tabCtrlEdit.setNumColumns(3)
		self.m_tabCtrlEdit.setColumnLength(12)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_PLOT_DATA",(self.m_pActivePlot.getX(), self.m_pActivePlot.getY())))
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_LATITUDE",(self.m_pActivePlot.getLatitude(),)),  0)
## Culture ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_CULTURE",()),  0)
		strTest = ()
		iCount = 0
		for i in xrange(GC.getMAX_PLAYERS()):
			if GC.getPlayer(i).isEverAlive():
				strTest = strTest + (GC.getPlayer(i).getName(),)
				if i == self.m_iCurrentPlayer:
					iOwner = iCount
				iCount = iCount + 1
		self.m_tabCtrlEdit.addSectionDropdown("Owner", strTest, "CvScreensInterface", "WorldBuilderHandleCurrentPlayerEditPullDownCB", "PlotEditCurrentPlayer", 0, iOwner)
		self.m_tabCtrlEdit.addSectionSpinner("PlotEditCultureCB", "CvScreensInterface", "WorldBuilderHandlePlotEditCultureCB", "PlotEditCulture", 0, 1, 100000000, 1, self.m_pActivePlot.getCulture(self.m_iCurrentPlayer), 0, 0)
## Plot Yield ##
		self.m_tabCtrlEdit.addSectionLabel(GC.getYieldInfo(0).getDescription() + ":",  0)
		self.m_tabCtrlEdit.addSectionSpinner("PlotEditYieldCB", "CvScreensInterface", "WorldBuilderHandlePlotEditYieldCB", "PlotEditFood", 0, 0, 20, 1, self.m_pActivePlot.getYield(YieldTypes.YIELD_FOOD), 0, 0)
		self.m_tabCtrlEdit.addSectionLabel(GC.getYieldInfo(1).getDescription() + ":",  0)
		self.m_tabCtrlEdit.addSectionSpinner("PlotEditYieldCB", "CvScreensInterface", "WorldBuilderHandlePlotEditYieldCB", "PlotEditProduction", 0, 0, 20, 1, self.m_pActivePlot.getYield(YieldTypes.YIELD_PRODUCTION), 0, 0)
		self.m_tabCtrlEdit.addSectionLabel(GC.getYieldInfo(2).getDescription() + ":",  0)
		self.m_tabCtrlEdit.addSectionSpinner("PlotEditYieldCB", "CvScreensInterface", "WorldBuilderHandlePlotEditYieldCB", "PlotEditCommerce", 0, 0, 20, 1, self.m_pActivePlot.getYield(YieldTypes.YIELD_COMMERCE), 0, 0)
## Script ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_SCRIPT_DATA",()),  0)
		self.m_tabCtrlEdit.addSectionEditCtrl(self.m_pActivePlot.getScriptData(), "CvScreensInterface", "WorldBuilderHandleEditScriptCB", "PlotEditScript", 0)
## Plot Type ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_CHANGE_PLOTTYPE",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_MOUNTAIN",()),) + (TRNSLTR.getText("TXT_KEY_WB_HILL",()),) + (TRNSLTR.getText("TXT_KEY_WB_LAND",()),) + (TRNSLTR.getText("TXT_KEY_WB_OCEAN",()),)
		self.m_tabCtrlEdit.addSectionDropdown("PlotEditPlotType", strTest, "CvScreensInterface", "WorldBuilderHandlePlotEditPlotTypeCB", "PlotEditPlotType", 0, self.m_pActivePlot.getPlotType())
## Terrain ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_CHANGE_TERRAIN",()),  0)
		strTest = ()
		iTerrain = 0
		iCount = 0
		for i in xrange(GC.getNumTerrainInfos()):
			if GC.getTerrainInfo(i).isGraphicalOnly(): continue
			if self.m_pActivePlot.getTerrainType() == i:
				iTerrain = iCount
			strTest = strTest + (GC.getTerrainInfo(i).getDescription(),)
			iCount += 1
		self.m_tabCtrlEdit.addSectionDropdown("Plot Terrain", strTest, "CvScreensInterface", "WorldBuilderHandlePlotEditTerrainCB", "PlotEditTerrain", 0, iTerrain)
## Bonus ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_BONUS",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_NONE",()),)
		for i in xrange(GC.getNumBonusInfos()):
			strTest = strTest + (GC.getBonusInfo(i).getDescription(),)
		self.m_tabCtrlEdit.addSectionDropdown("Bonus", strTest, "CvScreensInterface", "WorldBuilderHandlePlotEditBonusCB", "PlotEditBonus", 0, self.m_pActivePlot.getBonusType(-1) +1)
## Route ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_CHANGE_ROUTE",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_NONE",()),)
		for i in xrange(GC.getNumRouteInfos()):
			strTest = strTest + (GC.getRouteInfo(i).getDescription(),)
		self.m_tabCtrlEdit.addSectionDropdown("Route", strTest, "CvScreensInterface", "WorldBuilderHandlePlotEditRouteCB", "PlotEditRoute", 0, self.m_pActivePlot.getRouteType() +1)
## Improvement ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_IMPROVEMENT",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_NONE",()),)
		iImprovement = 0
		iCount = 1
		for i in xrange(GC.getNumImprovementInfos()):
			if GC.getImprovementInfo(i).isGraphicalOnly(): continue
			if self.m_pActivePlot.getImprovementType() == i:
				iImprovement = iCount
			strTest = strTest + (GC.getImprovementInfo(i).getDescription(),)
			iCount += 1
		self.m_tabCtrlEdit.addSectionDropdown("Improvement", strTest, "CvScreensInterface", "WorldBuilderHandlePlotEditImprovementCB", "PlotEditImprovement", 0, iImprovement)
## Upgrade Timer ##
		if self.m_pActivePlot.getOwner() > -1 and self.m_pActivePlot.getImprovementType() > -1 and GC.getImprovementInfo(self.m_pActivePlot.getImprovementType()).getUpgradeTime() > 0:
			self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_UPGRADE_PROGRESS",()),  0)
			self.m_tabCtrlEdit.addSectionSpinner("PlotEditUpgradeProgressCB", "CvScreensInterface", "WorldBuilderHandlePlotEditUpgradeProgressCB", "PlotEditUpgradeProgress", 0, 1, 1000, 1, self.m_pActivePlot.getUpgradeTimeLeft(self.m_pActivePlot.getImprovementType(), self.m_pActivePlot.getOwner()), 0, 0)
		else:
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
## Feature ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_FEATURE_VARIETY",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_NONE",()),)
		for i in xrange(GC.getNumFeatureInfos()):
			strTest = strTest + (GC.getFeatureInfo(i).getDescription(),)
		self.m_tabCtrlEdit.addSectionDropdown("PlotEditFeature", strTest, "CvScreensInterface", "WorldBuilderHandlePlotEditFeatureCB", "PlotEditFeature", 0, self.m_pActivePlot.getFeatureType() +1)
## Variety ##
		if self.m_pActivePlot.getFeatureType() > -1 and GC.getFeatureInfo(self.m_pActivePlot.getFeatureType()).getNumVarieties() > 1:
			self.m_tabCtrlEdit.addSectionSpinner("PlotEditVarietyCB", "CvScreensInterface", "WorldBuilderHandlePlotEditVarietyCB", "PlotEditVariety", 0, 0, GC.getFeatureInfo(self.m_pActivePlot.getFeatureType()).getNumVarieties(), 1, self.m_pActivePlot.getFeatureVariety(), 0, 0)
		else:
			self.m_tabCtrlEdit.addSectionLabel(" ",  0)
## River ##
		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_EAST_RIVER",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_NONE",()),) + (TRNSLTR.getText("TXT_KEY_WB_NO_DIRECTION",()),) + (TRNSLTR.getText("TXT_KEY_WB_NORTH",()),) + (TRNSLTR.getText("TXT_KEY_WB_SOUTH",()),)
		if not self.m_pActivePlot.isWOfRiver():
			iERiver = 0
		elif self.m_pActivePlot.getRiverNSDirection() == -1:
			iERiver = 1
		elif self.m_pActivePlot.getRiverNSDirection() == 0:
			iERiver = 2
		elif self.m_pActivePlot.getRiverNSDirection() == 2:
			iERiver = 3
		self.m_tabCtrlEdit.addSectionDropdown("PlotEditRiver", strTest, "CvScreensInterface", "WorldBuilderHandlePlotEditRiverCB", "NS", 0, iERiver)

		self.m_tabCtrlEdit.addSectionLabel(TRNSLTR.getText("TXT_KEY_WB_SOUTH_RIVER",()),  0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_NONE",()),) + (TRNSLTR.getText("TXT_KEY_WB_NO_DIRECTION",()),) + (TRNSLTR.getText("TXT_KEY_WB_EAST",()),) + (TRNSLTR.getText("TXT_KEY_WB_WEST",()),)
		if not self.m_pActivePlot.isNOfRiver():
			iSRiver = 0
		elif self.m_pActivePlot.getRiverWEDirection() == -1:
			iSRiver = 1
		elif self.m_pActivePlot.getRiverWEDirection() == 1:
			iSRiver = 2
		elif self.m_pActivePlot.getRiverWEDirection() == 3:
			iSRiver = 3
		self.m_tabCtrlEdit.addSectionDropdown("PlotEditRiver", strTest, "CvScreensInterface", "WorldBuilderHandlePlotEditRiverCB", "WE", 0, iSRiver)
## Event ##
		strTest = (TRNSLTR.getText("TXT_KEY_WB_TRIGGER_EVENT",()),)
		for i in xrange(GC.getNumEventTriggerInfos()):
			sEvent = GC.getEventTriggerInfo(i).getType()[13:]
			sEvent = sEvent.lower()
			sEvent = sEvent.replace("_", " ")
			sEvent = sEvent.capitalize()
			strTest = strTest + (sEvent,)
		self.m_tabCtrlEdit.addSectionDropdown("Trigger Event", strTest, "CvScreensInterface", "WorldBuilderHandleTriggerEventCB", "Trigger Event", 0, 0)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_EVENT_OTHER_PLAYER",()),)
		iOtherPlayer = 0
		iCount = 1
		for i in xrange(GC.getMAX_PC_PLAYERS()):
			if GC.getPlayer(i).isAlive():
				if i == self.m_iCurrentPlayer:
					continue
				strTest = strTest + (GC.getPlayer(i).getName(),)
				if i == self.m_iOtherPlayer:
					iOtherPlayer = iCount
				iCount = iCount + 1
		self.m_tabCtrlEdit.addSectionDropdown("Event Other Player", strTest, "CvScreensInterface", "WorldBuilderHandleEventOtherPlayerCB", "Event Other Player", 0, iOtherPlayer)
		strTest = (TRNSLTR.getText("TXT_KEY_WB_EVENT_UNIT",()),)
		for i in xrange(self.m_pActivePlot.getNumUnits()):
			if (len(self.m_pActivePlot.getUnit(i).getNameNoDesc())):
				strTest = strTest + (self.m_pActivePlot.getUnit(i).getNameNoDesc(),)
			else:
				strTest = strTest + (self.m_pActivePlot.getUnit(i).getName(),)
		self.m_tabCtrlEdit.addSectionDropdown("Current_Unit", strTest, "CvScreensInterface", "WorldBuilderHandleEventUnitCB", "EventUnit", 0, self.m_iEventUnit +1)
## City ##
		if self.m_pActivePlot.isCity():
			self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_EDIT_CITY",()) + "\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandleEnterNewScreenCB", "CityEditScreen", 0)
		else:
			self.m_tabCtrlEdit.addSectionButton(TRNSLTR.getText("TXT_KEY_WB_ADD_CITY",()) + "\b\b\b\b\b\b", "CvScreensInterface", "WorldBuilderHandlePlotAddCityCB", "PlotAddCity", 0)

		if (not self.m_tabCtrlEdit.isNone()):
			print("Enabling map control 5")
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Plot Edit Screen ##


## Diplomacy Edit Screen ##
	def setDiplomacyEditInfo(self):
		initWBToolEditCtrl()
		self.m_tabCtrlEdit = getWBToolEditTabCtrl()

		self.m_tabCtrlEdit.setNumColumns(8)
		self.m_tabCtrlEdit.addTabSection(TRNSLTR.getText("TXT_KEY_WB_DIPLOMACY",()))

		strTest = ()
		iCount = 0
		lOthers = []
		for i in xrange(GC.getMAX_PLAYERS()):
			if GC.getPlayer(i).isAlive():
				strTest = strTest + (GC.getPlayer(i).getName(),)
				if i == self.m_iCurrentPlayer:
					iCurrentPlayer = iCount
				else:
					lOthers.append(i)
				iCount += 1
		self.m_tabCtrlEdit.addSectionDropdown("Current Player", strTest, "CvScreensInterface", "WorldBuilderHandleCurrentPlayerEditPullDownCB", "DiplomacyEditCurrentPlayer", 0, iCurrentPlayer)
		pPlayer = GC.getPlayer(self.m_iCurrentPlayer)
		pTeam = GC.getTeam(self.m_iCurrentTeam)
		iColumnLength = len(lOthers) +1
		self.m_tabCtrlEdit.setColumnLength(iColumnLength)

		for i in lOthers:
			self.m_tabCtrlEdit.addSectionCheckbox(GC.getPlayer(i).getName(), "CvScreensInterface", "WorldBuilderHandleTeamEditMetStatusCB", str(GC.getPlayer(i).getTeam()), 0, pTeam.isHasMet(GC.getPlayer(i).getTeam()))
		strTest = (TRNSLTR.getText("TXT_KEY_WB_PAGE1",()),) + (TRNSLTR.getText("TXT_KEY_WB_PAGE2",()),) + (TRNSLTR.getText("TXT_KEY_WB_ESPIONAGE",()),)
		self.m_tabCtrlEdit.addSectionDropdown("Diplomacy Page", strTest, "CvScreensInterface", "WorldBuilderHandleDiplomacyPageCB", "Diplomacy Page", 0, self.m_iDiplomacyPage)
		if self.m_iDiplomacyPage == 0:
			for i in lOthers:
				strTest = (TRNSLTR.getText("TXT_KEY_WB_FVASSAL",()),) + (TRNSLTR.getText("TXT_KEY_WB_CVASSAL",()),) + (TRNSLTR.getText("TXT_KEY_WB_MASTER",()),) + (TRNSLTR.getText("TXT_KEY_WB_NONE",()),)
				self.m_tabCtrlEdit.addSectionDropdown("Relationship", strTest, "CvScreensInterface", "WorldBuilderHandleTeamEditRelationshipCB", str(i), 0, self.relationshipStatus(GC.getPlayer(i).getTeam(), self.m_iCurrentTeam))
			strTest = (TRNSLTR.getText("TXT_KEY_WB_COMMANDS",()),) + (TRNSLTR.getText("TXT_KEY_WB_CLEAR_ALL",()),) + (TRNSLTR.getText("TXT_KEY_WB_ENABLE_ALL",()),)
			self.m_tabCtrlEdit.addSectionDropdown("Diplomacy Commands", strTest, "CvScreensInterface", "WorldBuilderHandleDiplomacyCommandsCB", "War", 0, 0)
			for i in lOthers:
				self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_WAR",()), "CvScreensInterface", "WorldBuilderHandleTeamEditWarStatusCB", str(GC.getPlayer(i).getTeam()), 0, pTeam.isAtWar(GC.getPlayer(i).getTeam()))
			strTest = (TRNSLTR.getText("TXT_KEY_WB_COMMANDS",()),) + (TRNSLTR.getText("TXT_KEY_WB_CLEAR_ALL",()),) + (TRNSLTR.getText("TXT_KEY_WB_ENABLE_ALL",()),)
			self.m_tabCtrlEdit.addSectionDropdown("Diplomacy Commands", strTest, "CvScreensInterface", "WorldBuilderHandleDiplomacyCommandsCB", "Permanent", 0, 0)
			for i in lOthers:
				self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_OPEN_BORDERS",()), "CvScreensInterface", "WorldBuilderHandleTeamEditSignOpenBordersCB", str(GC.getPlayer(i).getTeam()), 0, pTeam.isOpenBorders(GC.getPlayer(i).getTeam()))
			strTest = (TRNSLTR.getText("TXT_KEY_WB_COMMANDS",()),) + (TRNSLTR.getText("TXT_KEY_WB_CLEAR_ALL",()),) + (TRNSLTR.getText("TXT_KEY_WB_ENABLE_ALL",()),)
			self.m_tabCtrlEdit.addSectionDropdown("Diplomacy Commands", strTest, "CvScreensInterface", "WorldBuilderHandleDiplomacyCommandsCB", "DefensivePact", 0, 0)
			for i in lOthers:
				self.m_tabCtrlEdit.addSectionCheckbox(TRNSLTR.getText("TXT_KEY_WB_DEFENSIVE_PACT",()), "CvScreensInterface", "WorldBuilderHandleTeamEditSignDefensivePactCB", str(GC.getPlayer(i).getTeam()), 0, pTeam.isDefensivePact(GC.getPlayer(i).getTeam()))
		elif self.m_iDiplomacyPage == 1:
			for i in lOthers:
				strTest = ()
				for iAttitude in xrange(5):
					strTest = strTest + (GC.getAttitudeInfo(iAttitude).getDescription() + " " + TRNSLTR.getText("TXT_KEY_WB_TOWARDS",(pPlayer.getName(),)),)
				self.m_tabCtrlEdit.addSectionDropdown("Attitude", strTest, "CvScreensInterface", "WorldBuilderHandleAttitudeEditPullDownCB", str(i), 0, GC.getPlayer(i).AI_getAttitude(self.m_iCurrentPlayer))
			strTest = ()
			for i in xrange(33):
				strTest = strTest + (GC.getMemoryInfo(i).getDescription(),)
			self.m_tabCtrlEdit.addSectionDropdown("Memory", strTest, "CvScreensInterface", "WorldBuilderHandleMemoryEditPullDownCB", "MemoryEditPullDown", 0, self.m_iMemory)
			for i in lOthers:
				self.m_tabCtrlEdit.addSectionSpinner("Memory Count", "CvScreensInterface", "WorldBuilderHandlePlayerEditMemoryCB", str(i), 0, 0, 100, 1, GC.getPlayer(i).AI_getMemoryCount(self.m_iCurrentPlayer, self.m_iMemory), 0, 0)
		else:
			if self.m_bTowardsPlayer:
				for i in lOthers:
					self.m_tabCtrlEdit.addSectionSpinner("TeamEditEspionagePointsCB", "CvScreensInterface", "WorldBuilderHandleTeamEditEspionagePointsCB", str(i), 0, 0, 1000000, 1, GC.getTeam(GC.getPlayer(i).getTeam()).getEspionagePointsAgainstTeam(self.m_iCurrentTeam), 0, 0)
				strTest = (TRNSLTR.getText("TXT_KEY_WB_ESPIONAGE_WEIGHT",()),)
				self.m_tabCtrlEdit.addSectionDropdown("Espionage Weight", strTest, "CvScreensInterface", "WorldBuilderHandleEspionageWeightCB", "Espionage Weight", 0, 0)
				for i in lOthers:
					self.m_tabCtrlEdit.addSectionSpinner("PlayerEditEspionageWeightCB", "CvScreensInterface", "WorldBuilderHandlePlayerEditEspionageWeightCB", str(i), 0, 0, 10000, 1, GC.getPlayer(i).getEspionageSpendingWeightAgainstTeam(self.m_iCurrentTeam), 0, 0)
				strTest = (TRNSLTR.getText("TXT_KEY_WB_COUNTER_ESPIONAGE_MOD",()),)
				self.m_tabCtrlEdit.addSectionDropdown("Counter Espionage Mod", strTest, "CvScreensInterface", "WorldBuilderHandleCounterEspionageModCB", "Counter Espionage Mod", 0, 0)
				for i in lOthers:
					self.m_tabCtrlEdit.addSectionSpinner("TeamEditCounterEspionageModCB", "CvScreensInterface", "WorldBuilderHandleTeamEditCounterEspionageModCB", str(i), 0, 0, 1000, 1, GC.getTeam(GC.getPlayer(i).getTeam()).getCounterespionageModAgainstTeam(self.m_iCurrentTeam), 0, 0)
				strTest = (TRNSLTR.getText("TXT_KEY_WB_COUNTER_ESPIONAGE",()),)
				self.m_tabCtrlEdit.addSectionDropdown("Counter Espionage Turns", strTest, "CvScreensInterface", "WorldBuilderHandleCounterEspionageTurnsCB", "Counter Espionage Turns", 0, 0)
				for i in lOthers:
					self.m_tabCtrlEdit.addSectionSpinner("TeamEditCounterEspionageCB", "CvScreensInterface", "WorldBuilderHandleTeamEditCounterEspionageCB", str(i), 0, 0, 1000, 1, GC.getTeam(GC.getPlayer(i).getTeam()).getCounterespionageTurnsLeftAgainstTeam(self.m_iCurrentTeam), 0, 0)
				strTest = (TRNSLTR.getText("TXT_KEY_WB_WAR_WEARINESS",()),)
				self.m_tabCtrlEdit.addSectionDropdown("War Weariness", strTest, "CvScreensInterface", "WorldBuilderHandleWarWearinessCB", "War Weariness", 0, 0)
				for i in lOthers:
					self.m_tabCtrlEdit.addSectionSpinner("TeamEditWarWearinessCB", "CvScreensInterface", "WorldBuilderHandleTeamEditWarWearinessCB", str(i), 0, 0, 10000, 	1, GC.getTeam(GC.getPlayer(i).getTeam()).getWarWeariness(GC.getPlayer(i).getTeam()), 0, 0)
			else:
				for i in lOthers:
					self.m_tabCtrlEdit.addSectionSpinner("TeamEditEspionagePointsCB", "CvScreensInterface", "WorldBuilderHandleTeamEditEspionagePointsCB", str(i), 0, 0, 1000000, 1, pTeam.getEspionagePointsAgainstTeam(GC.getPlayer(i).getTeam()), 0, 0)
				strTest = (TRNSLTR.getText("TXT_KEY_WB_ESPIONAGE_WEIGHT",()),)
				self.m_tabCtrlEdit.addSectionDropdown("Espionage Weight", strTest, "CvScreensInterface", "WorldBuilderHandleEspionageWeightCB", "Espionage Weight", 0, 0)
				for i in lOthers:
					self.m_tabCtrlEdit.addSectionSpinner("PlayerEditEspionageWeightCB", "CvScreensInterface", "WorldBuilderHandlePlayerEditEspionageWeightCB", str(i), 0, 0, 10000, 1, pPlayer.getEspionageSpendingWeightAgainstTeam(GC.getPlayer(i).getTeam()), 0, 0)
				strTest = (TRNSLTR.getText("TXT_KEY_WB_COUNTER_ESPIONAGE_MOD",()),)
				self.m_tabCtrlEdit.addSectionDropdown("Counter Espionage Mod", strTest, "CvScreensInterface", "WorldBuilderHandleCounterEspionageModCB", "Counter Espionage Mod", 0, 0)
				for i in lOthers:
					self.m_tabCtrlEdit.addSectionSpinner("TeamEditCounterEspionageModCB", "CvScreensInterface", "WorldBuilderHandleTeamEditCounterEspionageModCB", str(i), 0, 0, 1000, 1, pTeam.getCounterespionageModAgainstTeam(GC.getPlayer(i).getTeam()), 0, 0)
				strTest = (TRNSLTR.getText("TXT_KEY_WB_COUNTER_ESPIONAGE",()),)
				self.m_tabCtrlEdit.addSectionDropdown("Counter Espionage Turns", strTest, "CvScreensInterface", "WorldBuilderHandleCounterEspionageTurnsCB", "Counter Espionage Turns", 0, 0)
				for i in lOthers:
					self.m_tabCtrlEdit.addSectionSpinner("TeamEditCounterEspionageCB", "CvScreensInterface", "WorldBuilderHandleTeamEditCounterEspionageCB", str(i), 0, 0, 1000, 1, pTeam.getCounterespionageTurnsLeftAgainstTeam(GC.getPlayer(i).getTeam()), 0, 0)
				strTest = (TRNSLTR.getText("TXT_KEY_WB_WAR_WEARINESS",()),)
				self.m_tabCtrlEdit.addSectionDropdown("War Weariness", strTest, "CvScreensInterface", "WorldBuilderHandleWarWearinessCB", "War Weariness", 0, 0)
				for i in lOthers:
					self.m_tabCtrlEdit.addSectionSpinner("TeamEditWarWearinessCB", "CvScreensInterface", "WorldBuilderHandleTeamEditWarWearinessCB", str(i), 0, 0, 10000, 	1, pTeam.getWarWeariness(GC.getPlayer(i).getTeam()), 0, 0)
			self.m_tabCtrlEdit.addSectionRadioButton(TRNSLTR.getText("TXT_KEY_WB_TOWARDS",(pPlayer.getName(),)), "CvScreensInterface", "WorldBuilderHandleTowardsPlayerCB", "Towards Player", 0, self.m_bTowardsPlayer)
			self.m_tabCtrlEdit.addSectionRadioButton(TRNSLTR.getText("TXT_KEY_WB_TOWARDS_OTHERS",()), "CvScreensInterface", "WorldBuilderHandleTowardsOthersCB", "Towards Player", 0, not self.m_bTowardsPlayer)
		if (not self.m_tabCtrlEdit.isNone()):
			print("Enabling map control 5")
			self.m_normalPlayerTabCtrl.enable(False)
			self.m_normalMapTabCtrl.enable(False)
			self.m_bCtrlEditUp = True
		return
## Diplomacy Edit Screen ##

	def initCityEditScreen(self):
		self.setCityEditInfo(False)
		return

	def toggleUnitEditCB(self):
		self.m_bUnitEdit = True
		self.m_bCityEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bLandmark = False
		self.m_bEraseAll = False

		if (self.m_tabCtrlEdit != 0):
			print("Enabling map control 6")
			self.m_tabCtrlEdit.enable(False)

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)

		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iUnitEditCheckboxID)
		print("Enabling map control 7")
		self.m_normalPlayerTabCtrl.enable(False)
		self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.destroy()
		return

	def toggleCityEditCB(self):
		self.m_bCityEdit = True
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bLandmark = False
		self.m_bEraseAll = False

		if (self.m_tabCtrlEdit != 0):
			print("Enabling map control 8")
			self.m_tabCtrlEdit.enable(False)

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)

		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iCityEditCheckboxID)
		print("Enabling map control 9")
		self.m_normalPlayerTabCtrl.enable(False)
		self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.destroy()
		return

	def normalPlayerTabModeCB(self):
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = True
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
## Add Units ##
		self.m_iUnitCombat = -2
		self.m_iUnitType = -1
		self.m_iCurrentTeam = GC.getPlayer(self.m_iCurrentPlayer).getTeam()
## Add Units ##

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)

		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iNormalPlayerCheckboxID)
		if (self.m_normalMapTabCtrl):
			print("Disabling Map Tab")
			self.m_normalMapTabCtrl.enable(False)
		if (not self.m_normalPlayerTabCtrl.isEnabled() and not CyInterface().isInAdvancedStart()):
			print("Enabling Player Tab")
			self.m_normalPlayerTabCtrl.enable(True)
			if (self.m_tabCtrlEdit):
				self.m_tabCtrlEdit.enable(False)
			self.m_bCtrlEditUp = False
		return

	def normalMapTabModeCB(self):
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = True
		self.m_bReveal = False
		self.m_bLandmark = False
		self.m_bEraseAll = False
## Plot Mode ##
		self.m_iPlotMode = 0
		self.m_iArea = -1
## Plot Mode ##

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)

		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iNormalMapCheckboxID)
		if (self.m_normalPlayerTabCtrl):
			print("Disabling Player Tab")
			self.m_normalPlayerTabCtrl.enable(False)
		if (not self.m_normalMapTabCtrl.isEnabled() and not CyInterface().isInAdvancedStart()):
			print("Enabling Map Tab")
			self.m_normalMapTabCtrl.enable(True)
			if (self.m_tabCtrlEdit):
				self.m_tabCtrlEdit.enable(False)
			self.m_bCtrlEditUp = False
		return

	def revealTabModeCB(self):
		self.m_bCtrlEditUp = False
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = True
		self.m_bLandmark = False
		self.m_bEraseAll = False

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		self.refreshReveal()
		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iRevealTileCheckboxID)
		if (self.m_normalPlayerTabCtrl):
			self.m_normalPlayerTabCtrl.enable(False)
		if (self.m_normalMapTabCtrl):
			self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit):
			self.m_tabCtrlEdit.enable(False)
		return

	def diplomacyModeCB(self):
		self.m_bCtrlEditUp = False
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bLandmark = False
		self.m_bEraseAll = False

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iDiplomacyCheckboxID)
		if (self.m_normalPlayerTabCtrl != 0):
			self.m_normalPlayerTabCtrl.enable(False)
		if (self.m_normalMapTabCtrl != 0):
			self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.enable(False)
## Diplomacy Screen ##
		self.setDiplomacyEditInfo()
## Diplomacy Screen ##
		return

	def landmarkModeCB(self):
		self.m_bCtrlEditUp = False
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bLandmark = True
		self.m_bEraseAll = False

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iLandmarkCheckboxID)
		if (self.m_normalPlayerTabCtrl != 0):
			self.m_normalPlayerTabCtrl.enable(False)
		if (self.m_normalMapTabCtrl != 0):
			self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.enable(False)
		return

	def eraseCB(self):
		self.m_bCtrlEditUp = False
		self.m_bCityEdit = False
		self.m_bUnitEdit = False
		self.m_bNormalPlayer = False
		self.m_bNormalMap = False
		self.m_bReveal = False
		self.m_bLandmark = False
		self.m_bEraseAll = True
		self.m_pRiverStartPlot = -1

		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS)
		CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
		self.refreshSideMenu()
		self.setCurrentModeCheckbox(self.m_iEraseCheckboxID)
		if (self.m_normalPlayerTabCtrl != 0):
			self.m_normalPlayerTabCtrl.enable(False)
		if (self.m_normalMapTabCtrl != 0):
			self.m_normalMapTabCtrl.enable(False)
		if (self.m_tabCtrlEdit != 0):
			self.m_tabCtrlEdit.enable(False)
		return

	def setCurrentNormalPlayerIndex(self, argsList):
		iIndex = int(argsList)
		if (self.m_normalPlayerTabCtrl.getActiveTab() != self.m_iTechnologyTabID):
			self.m_iNormalPlayerCurrentIndexes [self.m_normalPlayerTabCtrl.getActiveTab()] = int(argsList)
		else:
			bOn = GC.getTeam(GC.getPlayer(self.m_iCurrentPlayer).getTeam()).isHasTech(iIndex)
			bOn = not bOn
			GC.getTeam(GC.getPlayer(self.m_iCurrentPlayer).getTeam()).setHasTech(iIndex, bOn, self.m_iCurrentPlayer, False, False)
## Add Units Start ##
		self.m_iUnitCombat = -2
		self.m_iUnitType = -1
		self.refreshSideMenu()
## Add Units End ##
		return 1

	def setCurrentNormalMapIndex(self, argsList):
		iIndex = int(argsList)
		self.m_iNormalMapCurrentIndexes [self.m_normalMapTabCtrl.getActiveTab()] = int(argsList)
## Edit Area Map Start ##
		self.m_iPlotMode = 0
		self.m_iArea = -1
		self.refreshSideMenu()
## Edit Area Map End ##
		return 1

	def setCurrentNormalMapList(self, argsList):
		self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] = int(argsList)
		return 1

	def setCurrentAdvancedStartIndex(self, argsList):
		iIndex = int(argsList)
		self.m_iAdvancedStartCurrentIndexes [self.m_advancedStartTabCtrl.getActiveTab()] = int(argsList)
		return 1

	def setCurrentAdvancedStartList(self, argsList):
		self.m_iAdvancedStartCurrentList[self.m_advancedStartTabCtrl.getActiveTab()] = int(argsList)
		return 1

## Platy World Builder Start ##
	def setEditButtonClicked(self, argsList):
		iIndex = int(argsList)
		if self.m_bUnitEdit:
			bOn = not self.m_pActivePlot.getUnit(self.m_iCurrentUnit).isHasPromotion(iIndex)
			self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setHasPromotion(iIndex, bOn)
		return 1

	def relationshipStatus(self, iTeam1, iTeam2):
		if GC.getTeam(iTeam1).isVassal(iTeam2):
			for i in xrange(CyGame().getIndexAfterLastDeal()):
				pDeal = CyGame().getDeal(i)
				iPlayer1 = pDeal.getFirstPlayer()
				iPlayer2 = pDeal.getSecondPlayer()
				if iPlayer1 == -1 or iPlayer2 == -1: continue
				iTeamX = GC.getPlayer(pDeal.getFirstPlayer()).getTeam()
				iTeamY = GC.getPlayer(pDeal.getSecondPlayer()).getTeam()
				if (iTeam1 == iTeamX and iTeam2 == iTeamY) or (iTeam2 == iTeamX and iTeam1 == iTeamY):
					for j in xrange(pDeal.getLengthFirstTrades()):
						if pDeal.getFirstTrade(j).ItemType == TradeableItems.TRADE_VASSAL:
							return 0
						if pDeal.getFirstTrade(j).ItemType == TradeableItems.TRADE_SURRENDER:
							return 1
					for j in xrange(pDeal.getLengthSecondTrades()):
						if pDeal.getSecondTrade(j).ItemType == TradeableItems.TRADE_VASSAL:
							return 0
						if pDeal.getSecondTrade(j).ItemType == TradeableItems.TRADE_SURRENDER:
							return 1
		elif GC.getTeam(iTeam2).isVassal(iTeam1):
			return 2
		return 3
## Platy World Builder End ##

	def getUnitTabID(self):
		return self.m_iUnitTabID

	def getBuildingTabID(self):
		return self.m_iBuildingTabID

	def getTechnologyTabID(self):
		return self.m_iTechnologyTabID

	def getImprovementTabID(self):
		return self.m_iImprovementTabID

	def getBonusTabID(self):
		return self.m_iBonusTabID

	def getImprovementListID(self):
		return self.m_iImprovementListID

	def getBonusListID(self):
		return self.m_iBonusListID

	def getTerrainTabID(self):
		return self.m_iTerrainTabID

	def getTerrainListID(self):
		return self.m_iTerrainListID

	def getFeatureListID(self):
		return self.m_iFeatureListID

	def getPlotTypeListID(self):
		return self.m_iPlotTypeListID

	def getRouteListID(self):
		return self.m_iRouteListID

	def getTerritoryTabID(self):
		return self.m_iTerritoryTabID

	def getTerritoryListID(self):
		return self.m_iTerritoryListID

	def getASUnitTabID(self):
		return self.m_iASUnitTabID

	def getASUnitListID(self):
		return self.m_iASUnitListID

	def getASCityTabID(self):
		return self.m_iASCityTabID

	def getASCityListID(self):
		return self.m_iASCityListID

	def getASBuildingsListID(self):
		return self.m_iASBuildingsListID

	def getASAutomateListID(self):
		return self.m_iASAutomateListID

	def getASImprovementsTabID(self):
		return self.m_iASImprovementsTabID

	def getASRoutesListID(self):
		return self.m_iASRoutesListID

	def getASImprovementsListID(self):
		return self.m_iASImprovementsListID

	def getASVisibilityTabID(self):
		return self.m_iASVisibilityTabID

	def getASVisibilityListID(self):
		return self.m_iASVisibilityListID

	def getASTechTabID(self):
		return self.m_iASTechTabID

	def getASTechListID(self):
		return self.m_iASTechListID

## Platy Multiple Mode Start ##
	def highlightBrush(self):
		if (self.m_bShowBigBrush):
			if (self.m_pCurrentPlot == 0):
				return
			CyEngine().clearAreaBorderPlots(AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER)
			for x in xrange(self.m_pCurrentPlot.getX() - self.m_iBrushSize +1, self.m_pCurrentPlot.getX() + self.m_iBrushSize):
				for y in xrange(self.m_pCurrentPlot.getY() - self.m_iBrushSize +1, self.m_pCurrentPlot.getY() + self.m_iBrushSize):
					pPlot = CyMap().plot(x,y)
					if not pPlot.isNone():
						CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_WORLD_BUILDER, "COLOR_GREEN", 1)
		return

	def placeMultipleObjects(self):
		permCurrentPlot = self.m_pCurrentPlot
		for x in xrange(permCurrentPlot.getX() - self.m_iBrushSize +1, permCurrentPlot.getX() + self.m_iBrushSize):
			for y in xrange(permCurrentPlot.getY() - self.m_iBrushSize +1, permCurrentPlot.getY() + self.m_iBrushSize):
				self.m_pCurrentPlot = CyMap().plot(x,y)
				if not self.m_pCurrentPlot.isNone():
					if self.m_iBrushSize > 1:
						if (self.m_bNormalMap and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iImprovementTabID)):
							iImprovementType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
							iIndex = -1
							iCounter = -1
							while (iIndex < iImprovementType and iCounter < GC.getNumImprovementInfos()):
								iCounter += 1
								if not GC.getImprovementInfo(iCounter).isGraphicalOnly():
									iIndex += 1
							if iIndex > -1:
								if self.m_pCurrentPlot.canHaveImprovement(iCounter, -1, True):
									self.placeObject()
						elif (self.m_bNormalMap and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iBonusTabID)):
							iBonusType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
							if self.m_pCurrentPlot.canHaveBonus(iBonusType, False):
								self.placeObject()
						elif (self.m_bNormalMap and (self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerrainTabID)):
							if self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iFeatureListID:
								iButtonIndex = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
								iCount = -1
								for i in xrange(GC.getNumFeatureInfos()):
									for j in xrange(GC.getFeatureInfo(i).getNumVarieties()):
										iCount += 1
										if iCount == iButtonIndex:
											if self.m_pCurrentPlot.canHaveFeature(i):
												self.placeObject()
							elif (self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iRouteListID):
								iRouteType = self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()]
								if iRouteType < GC.getNumRouteInfos():
									if not (self.m_pCurrentPlot.isImpassable() or self.m_pCurrentPlot.isWater()):
										self.placeObject()
						else:
							self.placeObject()
					else:
						self.placeObject()
		self.m_pCurrentPlot = permCurrentPlot
		return

	def removeMultipleObjects(self):
		permCurrentPlot = self.m_pCurrentPlot
		for x in xrange(permCurrentPlot.getX() - self.m_iBrushSize +1, permCurrentPlot.getX() + self.m_iBrushSize):
			for y in xrange(permCurrentPlot.getY() - self.m_iBrushSize +1, permCurrentPlot.getY() + self.m_iBrushSize):
				self.m_pCurrentPlot = CyMap().plot(x,y)
				if not self.m_pCurrentPlot.isNone():
					self.removeObject()
		self.m_pCurrentPlot = permCurrentPlot
		return

	def showMultipleReveal(self):
		print "showMultipleReveal"
		self.refreshReveal()
		return

	def setMultipleReveal(self, bReveal):
		print "setMultipleReveal"
		permCurrentPlot = self.m_pCurrentPlot
		for x in xrange(permCurrentPlot.getX() - self.m_iBrushSize +1, permCurrentPlot.getX() + self.m_iBrushSize):
			for y in xrange(permCurrentPlot.getY() - self.m_iBrushSize +1, permCurrentPlot.getY() + self.m_iBrushSize):
				self.m_pCurrentPlot = CyMap().plot(x,y)
				if not self.m_pCurrentPlot.isNone():
					self.RevealCurrentPlot(bReveal)
		self.m_pCurrentPlot = permCurrentPlot
		self.showMultipleReveal()
		return
## Platy Multiple Mode End ##

	def useLargeBrush(self):
		if self.m_bReveal:
			return True
		if self.m_bNormalMap:
			if (self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerrainTabID and
					(self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iTerrainListID or
					self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iFeatureListID or
					self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iPlotTypeListID or
					(self.m_iNormalMapCurrentList[self.m_normalMapTabCtrl.getActiveTab()] == self.m_iRouteListID and self.m_iNormalMapCurrentIndexes[self.m_normalMapTabCtrl.getActiveTab()] < GC.getNumRouteInfos()))
				or self.m_normalMapTabCtrl.getActiveTab() == self.m_iBonusTabID
				or self.m_normalMapTabCtrl.getActiveTab() == self.m_iTerritoryTabID
				or self.m_normalMapTabCtrl.getActiveTab() == self.m_iImprovementTabID
			):
				return True

		else:
			return False

	def refreshSideMenu(self):
		screen = self.getScreen()

		xRes = self.xRes
		iScreenHeight = 10+37+37

		if (CyInterface().isInAdvancedStart()):

			iX = 50
			iY = 15
			szText = u"<font=4>" + TRNSLTR.getText("TXT_KEY_WB_AS_POINTS", (GC.getPlayer(CyGame().getActivePlayer()).getAdvancedStartPoints(), )) + "</font>"
			screen.setLabel("AdvancedStartPointsText", "Background", szText, 1<<0, iX, iY, -2, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			szText = u"<font=4>" + TRNSLTR.getText("TXT_KEY_WB_AS_COST_THIS_LOCATION", (self.m_iCost, )) + u"</font>"
			iY = 85
			screen.setLabel("AdvancedStartCostText", "Background", szText, 1<<0, iX-20, iY, -2, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		else:

			screen.deleteWidget("WorldBuilderPlayerChoice")
## Game Data ##
			screen.deleteWidget("WorldBuilderGameData")
			screen.deleteWidget("WorldBuilderAddUnits")
			screen.deleteWidget("WorldBuilderUnitCombat")
			screen.deleteWidget("WorldBuilderEditAreaMap")
			screen.deleteWidget("WorldBuilderModifyAreaPlotType")
			screen.deleteWidget("WorldBuilderModifyAreaTerrain")
			screen.deleteWidget("WorldBuilderModifyAreaRoute")
			screen.deleteWidget("WorldBuilderModifyAreaFeature")
			screen.deleteWidget("WorldBuilderRevealMode")
## Game Data ##
			screen.deleteWidget("WorldBuilderBrushSize")
			screen.deleteWidget("WorldBuilderRegenerateMap")
			screen.deleteWidget("WorldBuilderTeamChoice")

			screen.deleteWidget("WorldBuilderRevealAll")
			screen.deleteWidget("WorldBuilderUnrevealAll")
			screen.deleteWidget("WorldBuilderRevealPanel")
			screen.deleteWidget("WorldBuilderBackgroundBottomPanel")

			iPanelWidth = 35*6
			if self.m_bReveal:
				screen.addPanel( "WorldBuilderBackgroundBottomPanel", "", "", True, True, xRes-228, 10+32+32, 228, 45 + 37 * 2, PanelStyles.PANEL_STYLE_MAIN )
			elif (self.m_bNormalPlayer and (not self.m_bUnitEdit) and (not self.m_bCityEdit)):
				if self.m_iUnitCombat > -2:
					screen.addPanel( "WorldBuilderBackgroundBottomPanel", "", "", True, True, xRes-228, 10+32+32, 228, 45 + 37 * 3, PanelStyles.PANEL_STYLE_MAIN )
				else:
					screen.addPanel( "WorldBuilderBackgroundBottomPanel", "", "", True, True, xRes-228, 10+32+32, 228, 45 + 37 * 2, PanelStyles.PANEL_STYLE_MAIN )
			elif self.m_bNormalMap:
				if self.m_iPlotMode == 2:
					screen.addPanel( "WorldBuilderBackgroundBottomPanel", "", "", True, True, xRes-228, 10+32+32, 228, 45 + 37 * 3, PanelStyles.PANEL_STYLE_MAIN )
				elif self.m_iArea == -1:
					screen.addPanel( "WorldBuilderBackgroundBottomPanel", "", "", True, True, xRes-228, 10+32+32, 228, 45 + 37, PanelStyles.PANEL_STYLE_MAIN )
				else:
					if CyMap().getArea(self.m_iArea).isWater():
						screen.addPanel( "WorldBuilderBackgroundBottomPanel", "", "", True, True, xRes-228, 10+32+32, 228, 45 + 37 * 3, PanelStyles.PANEL_STYLE_MAIN )
					else:
						screen.addPanel( "WorldBuilderBackgroundBottomPanel", "", "", True, True, xRes-228, 10+32+32, 228, 45 + 37 * 5, PanelStyles.PANEL_STYLE_MAIN )
			else:
				screen.addPanel( "WorldBuilderBackgroundBottomPanel", "", "", True, True, xRes-228, 10+32+32, 228, 45, PanelStyles.PANEL_STYLE_MAIN )

			if (self.m_bNormalPlayer and (not self.m_bUnitEdit) and (not self.m_bCityEdit)):
				szDropdownName = str("WorldBuilderPlayerChoice")
				screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
## Dead Player ##
				for i in xrange( GC.getMAX_PLAYERS()):
					if ( GC.getPlayer(i).isEverAlive()):
						strPlayerAliveStatus = GC.getPlayer(i).getName()
						if not GC.getPlayer(i).isAlive():
							strPlayerAliveStatus = strPlayerAliveStatus + " " + TRNSLTR.getText("TXT_KEY_WB_DEAD",())
						if (i == self.m_iCurrentPlayer):
							screen.addPullDownString(szDropdownName, strPlayerAliveStatus, i, i, True )

						else:
							screen.addPullDownString(szDropdownName, strPlayerAliveStatus, i, i, False )
## Game Data Start##
				szDropdownName = str("WorldBuilderGameData")
				screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_PLAYER_DATA",(self.m_iCurrentPlayer,)), 0, 0, True )
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_TEAM_DATA",(self.m_iCurrentTeam,)), 0, 0, True )
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_GAME_DATA",()), 0, 0, True )
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_ADD_NEW_PLAYER",()), 0, 0, True )
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_EDIT_DATA",()), 0, 0, True )
## Adds Units Start ##
				szDropdownName = str("WorldBuilderAddUnits")
				screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_NO_CLASS",()), 0, 0, True )
				for i in xrange(GC.getNumUnitCombatInfos()):
					szPullDownString = GC.getUnitCombatInfo(i).getDescription()
					screen.addPullDownString(szDropdownName, szPullDownString, i, i, True )
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_ADD_UNITS",()), i, i, True )

				if self.m_iUnitCombat > -2:
					szDropdownName = str("WorldBuilderUnitCombat")
					screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
					for i in xrange(GC.getNumUnitInfos()):
						if GC.getUnitInfo(i).getUnitCombatType() == self.m_iUnitCombat:
							szPullDownString = GC.getUnitInfo(i).getDescription()
							screen.addPullDownString(szDropdownName, szPullDownString, i, i, True )
					if self.m_iUnitCombat == -1:
						screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_NO_CLASS",()), i, i, True )
					else:
						screen.addPullDownString(szDropdownName, GC.getUnitCombatInfo(self.m_iUnitCombat).getDescription(), i, i, True )
## Adds Units Ends ##

			elif(self.m_bNormalMap and (not self.m_bUnitEdit) and (not self.m_bCityEdit)):
				iButtonX = 0
				iButtonY = 0
				screen.setImageButton( "WorldBuilderRegenerateMap", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_REVEAL_ALL_TILES").getPath(), (xRes-228)+8+iButtonX, (10+36+36), 32, 32, WidgetTypes.WIDGET_WB_REGENERATE_MAP, -1, -1)

				szDropdownName = str("WorldBuilderBrushSize")
				screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+48, (10+36+36), iPanelWidth-40, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_1_BY_1",()), 1, 1, self.m_iBrushSize == 1)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_3_BY_3",()), 2, 2, self.m_iBrushSize == 2)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_5_BY_5",()), 3, 3, self.m_iBrushSize == 3)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_7_BY_7",()), 4, 4, self.m_iBrushSize== 4)

## Edit Area Map Start##
				szDropdownName = str("WorldBuilderEditAreaMap")
				screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_STANDARD_MODE",()), 0, 0, self.m_iPlotMode == 0)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_CHANGE_AREA_PLOTS",()), 1, 1, self.m_iPlotMode == 1)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_CHANGE_ALL_PLOTS",()), 2, 2, self.m_iPlotMode == 2)
				if self.m_iArea > -1 or  self.m_iPlotMode == 2:
					szDropdownName = str("WorldBuilderModifyAreaPlotType")
					screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
					screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_MOUNTAIN",()), 0, 0, False)
					screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_HILL",()), 0, 0, False)
					screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_LAND",()), 0, 0, False)
					screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_OCEAN",()), 0, 0, False)
					screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_CHANGE_PLOTTYPE",()), 0, 0, True)

					szDropdownName = str("WorldBuilderModifyAreaTerrain")
					screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
					for i in xrange(GC.getNumTerrainInfos()):
						if GC.getTerrainInfo(i).isGraphicalOnly(): continue
						if self.m_iPlotMode == 2:
							screen.addPullDownString(szDropdownName, GC.getTerrainInfo(i).getDescription(), 0, 0, False)
						elif CyMap().getArea(self.m_iArea).isWater() and GC.getTerrainInfo(i).isWaterTerrain():
							screen.addPullDownString(szDropdownName, GC.getTerrainInfo(i).getDescription(), 0, 0, False)
						elif not CyMap().getArea(self.m_iArea).isWater() and not GC.getTerrainInfo(i).isWaterTerrain():
							screen.addPullDownString(szDropdownName, GC.getTerrainInfo(i).getDescription(), 0, 0, False)
					screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_CHANGE_TERRAIN",()), 0, 0, True )

					if not CyMap().getArea(self.m_iArea).isWater() and self.m_iPlotMode < 2:
						szDropdownName = str("WorldBuilderModifyAreaRoute")
						screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36+36+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
						for i in xrange(GC.getNumRouteInfos()):
							screen.addPullDownString(szDropdownName, GC.getRouteInfo(i).getDescription(), 0, 0, False)
						screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_CLEAR_ALL",()), 0, 0, False)
						screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_CHANGE_ROUTE",()), 0, 0, True )

						szDropdownName = str("WorldBuilderModifyAreaFeature")
						screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36+36+36+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
						screen.addPullDownString(szDropdownName, GC.getFeatureInfo(GC.getInfoTypeForString("FEATURE_JUNGLE")).getDescription(), 0, 0, False)
						screen.addPullDownString(szDropdownName, GC.getFeatureInfo(GC.getInfoTypeForString("FEATURE_FOREST")).getDescription(), 0, 0, False)
						screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_CLEAR_ALL",()), 0, 0, False)
						screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_CHANGE_FEATURE",()), 0, 0, True )
## Edit Area Map End ##
			elif(self.m_bReveal):
				iPanelWidth = 35*6
				iButtonX = 0
				iButtonY = 0
				screen.setImageButton( "WorldBuilderRevealAll", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_REVEAL_ALL_TILES").getPath(), (xRes-228)+8+iButtonX, (10+36+36), 32, 32, WidgetTypes.WIDGET_WB_REVEAL_ALL_BUTTON, -1, -1)
				iButtonX = iButtonX + 35
				screen.setImageButton( "WorldBuilderUnrevealAll", ArtFileMgr.getInterfaceArtInfo("WORLDBUILDER_UNREVEAL_ALL_TILES").getPath(), (xRes-228)+8+iButtonX, (10+36+36), 32, 32, WidgetTypes.WIDGET_WB_UNREVEAL_ALL_BUTTON, -1, -1)
				iButtonX = iButtonX + 35

				szDropdownName = str("WorldBuilderBrushSize")
				screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8+80, (10+36+36), iPanelWidth-80, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_1_BY_1",()), 1, 1, self.m_iBrushSize == 1)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_3_BY_3",()), 2, 2, self.m_iBrushSize == 2)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_5_BY_5",()), 3, 3, self.m_iBrushSize == 3)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_7_BY_7",()), 4, 4, self.m_iBrushSize == 4)

				szDropdownName = str("WorldBuilderTeamChoice")
				screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				for i in xrange( GC.getMAX_PC_TEAMS()):
					if (GC.getTeam(i).isAlive()):
						screen.addPullDownString(szDropdownName, GC.getTeam(i).getName(), i, i, i == self.m_iCurrentTeam)
## Reveal Invisible Start##
				szDropdownName = str("WorldBuilderRevealMode")
				screen.addDropDownBoxGFC(szDropdownName, (xRes-228)+8, (10+36+36+36+36), iPanelWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_REVEAL_SUBMARINE",()), 0, 0, self.m_iRevealMode == 0)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_REVEAL_STEALTH",()), 1, 1, self.m_iRevealMode == 1)
				screen.addPullDownString(szDropdownName, TRNSLTR.getText("TXT_KEY_WB_REVEAL_PLOT",()), 2, 2, self.m_iRevealMode == 2)
## Reveal Invisible End ##
			else:
				screen.deleteWidget("WorldBuilderBackgroundBottomPanel")

		return

## Platy Reveal Mode Start ##
	def revealAll(self, bReveal):
		for i in xrange(CyMap().getGridWidth()):
			for j in xrange(CyMap().getGridHeight()):
				pPlot = CyMap().plot(i,j)
				if (not pPlot.isNone()):
					if self.m_iRevealMode == 2:
						if bReveal or (not pPlot.isVisible(self.m_iCurrentTeam, False)):
							pPlot.setRevealed(self.m_iCurrentTeam, bReveal, False, -1);
					else:
						if bReveal == True:
							pPlot.changeInvisibleVisibilityCount(self.m_iCurrentTeam, self.m_iRevealMode, 1)
						else:
							pPlot.changeInvisibleVisibilityCount(self.m_iCurrentTeam, self.m_iRevealMode, - pPlot.getInvisibleVisibilityCount(self.m_iCurrentTeam, self.m_iRevealMode))
		self.refreshReveal()
		return

	def RevealCurrentPlot(self, bReveal):
		if self.m_iRevealMode == 2:
			if bReveal or (not self.m_pCurrentPlot.isVisible(self.m_iCurrentTeam, False)):
				self.m_pCurrentPlot.setRevealed(self.m_iCurrentTeam, bReveal, False, -1);
		else:
			if bReveal == True or (not self.m_pCurrentPlot.isInvisibleVisible(self.m_iCurrentTeam, self.m_iRevealMode)):
				self.m_pCurrentPlot.changeInvisibleVisibilityCount(self.m_iCurrentTeam, self.m_iRevealMode, 1)
			if bReveal == False or (not self.m_pCurrentPlot.isInvisibleVisible(self.m_iCurrentTeam, self.m_iRevealMode)):
				self.m_pCurrentPlot.changeInvisibleVisibilityCount(self.m_iCurrentTeam, self.m_iRevealMode, - self.m_pCurrentPlot.getInvisibleVisibilityCount(self.m_iCurrentTeam, self.m_iRevealMode))
		return

	def showRevealed(self, pPlot):
		if self.m_iRevealMode == 2:
			if (not pPlot.isRevealed(self.m_iCurrentTeam, False)):
				CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS, "COLOR_BLACK", 1.0)
		elif self.m_iRevealMode == 1:
			if pPlot.getInvisibleVisibilityCount(self.m_iCurrentTeam, self.m_iRevealMode) == 0:
				CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS, "COLOR_RED", 1.0)
		elif self.m_iRevealMode == 0:
			if pPlot.getInvisibleVisibilityCount(self.m_iCurrentTeam, self.m_iRevealMode) == 0:
				CyEngine().fillAreaBorderPlotAlt(pPlot.getX(), pPlot.getY(), AreaBorderLayers.AREA_BORDER_LAYER_REVEALED_PLOTS, "COLOR_BLUE", 1.0)
		return
## Platy Reveal Mode End ##

	def getNumPlayers(self):
		iCount = 0
		for i in xrange( GC.getMAX_PC_PLAYERS()):
			if ( GC.getPlayer(i).isEverAlive()):
				iCount = iCount + 1

		return iCount

	def setLandmarkCB(self, szLandmark):
		self.m_pCurrentPlot = CyInterface().getMouseOverPlot()
		CyEngine().addLandmarkPopup(self.m_pCurrentPlot) # , u"%s" %(szLandmark))
		return

	def removeLandmarkCB(self):
		self.m_pCurrentPlot = CyInterface().getMouseOverPlot()
		CyEngine().removeLandmark(self.m_pCurrentPlot)
		return

	def refreshPlayerTabCtrl(self):

		initWBToolPlayerControl()

		self.m_normalPlayerTabCtrl = getWBToolNormalPlayerTabCtrl()

		self.m_normalPlayerTabCtrl.setNumColumns((GC.getNumUnitInfos()/10)+2);
		self.m_normalPlayerTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_UNITS",()));
		self.m_iUnitTabID = 0
		self.m_iNormalPlayerCurrentIndexes.append(0)

		self.m_normalPlayerTabCtrl.setNumColumns((GC.getNumBuildingInfos()/10)+1);
		self.m_normalPlayerTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_BUILDINGS",()));
		self.m_iBuildingTabID = 1
		self.m_iNormalPlayerCurrentIndexes.append(0)

		self.m_normalPlayerTabCtrl.setNumColumns((GC.getNumTechInfos()/10)+1);
		self.m_normalPlayerTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_TECHNOLOGIES",()));
		self.m_iTechnologyTabID = 2
		self.m_iNormalPlayerCurrentIndexes.append(0)

		addWBPlayerControlTabs()
		return

	def refreshAdvancedStartTabCtrl(self, bReuse):

		if CyInterface().isInAdvancedStart():

			if (self.m_advancedStartTabCtrl and bReuse):
				iActiveTab = self.m_advancedStartTabCtrl.getActiveTab()
				iActiveList = self.m_iAdvancedStartCurrentList[iActiveTab]
				iActiveIndex = self.m_iAdvancedStartCurrentIndexes[iActiveTab]
			else:
				iActiveTab = 0
				iActiveList = 0
				iActiveIndex = 0

			self.m_iCurrentPlayer = CyGame().getActivePlayer()
			self.m_iCurrentTeam = CyGame().getActiveTeam()
			self.m_iAdvancedStartCurrentIndexes = []
			self.m_iAdvancedStartCurrentList = []

			initWBToolAdvancedStartControl()

			self.m_advancedStartTabCtrl = getWBToolAdvancedStartTabCtrl()

			self.m_advancedStartTabCtrl.setNumColumns((GC.getNumBuildingInfos()/10)+2);
			self.m_advancedStartTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_AS_CITIES",()));
			self.m_iASCityTabID = 0
			self.m_iAdvancedStartCurrentIndexes.append(0)

			self.m_iASCityListID = 0
			self.m_iASBuildingsListID = 2
			self.m_iASAutomateListID = 1
			self.m_iAdvancedStartCurrentList.append(self.m_iASCityListID)

			self.m_advancedStartTabCtrl.setNumColumns((GC.getNumUnitInfos()/10)+2);
			self.m_advancedStartTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_AS_UNITS",()));
			self.m_iASUnitTabID = 1
			self.m_iAdvancedStartCurrentIndexes.append(0)

			self.m_iAdvancedStartCurrentList.append(0)
			self.m_iASUnitListID = 0

			self.m_advancedStartTabCtrl.setNumColumns((GC.getNumImprovementInfos()/10)+2);
			self.m_advancedStartTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_AS_IMPROVEMENTS",()));
			self.m_iASImprovementsTabID = 2
			self.m_iAdvancedStartCurrentIndexes.append(0)

			self.m_iASRoutesListID = 0
			self.m_iASImprovementsListID = 1
			self.m_iAdvancedStartCurrentList.append(self.m_iASRoutesListID)

			self.m_advancedStartTabCtrl.setNumColumns(1);
			self.m_advancedStartTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_AS_VISIBILITY",()));
			self.m_iASVisibilityTabID = 3
			self.m_iAdvancedStartCurrentIndexes.append(0)

			self.m_iAdvancedStartCurrentList.append(0)
			self.m_iASVisibilityListID = 0

			self.m_advancedStartTabCtrl.setNumColumns(1);
			self.m_advancedStartTabCtrl.addTabSection(TRNSLTR.getText("TXT_KEY_WB_AS_TECH",()));
			self.m_iASTechTabID = 4
			self.m_iAdvancedStartCurrentIndexes.append(0)

			self.m_iAdvancedStartCurrentList.append(0)
			self.m_iASTechListID = 0

			addWBAdvancedStartControlTabs()

			self.m_advancedStartTabCtrl.setActiveTab(iActiveTab)
			self.setCurrentAdvancedStartIndex(iActiveIndex)
			self.setCurrentAdvancedStartList(iActiveList)
		else:

			self.m_advancedStartTabCtrl = getWBToolAdvancedStartTabCtrl()

			self.m_advancedStartTabCtrl.enable(False)

		return

	def eraseAll(self):
		# kill all units on plot if one is selected
		if (self.m_pCurrentPlot != 0):
			while (self.m_pCurrentPlot.getNumUnits() > 0):
				pUnit = self.m_pCurrentPlot.getUnit(0)
				pUnit.kill(False, PlayerTypes.NO_PLAYER)

			self.m_pCurrentPlot.setBonusType(-1)
			self.m_pCurrentPlot.setFeatureType(FeatureTypes.NO_FEATURE, -1)

			if (self.m_pCurrentPlot.isCity()):
				self.m_pCurrentPlot.getPlotCity().kill()

			self.m_pCurrentPlot.setRouteType(-1)
			self.m_pCurrentPlot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			self.m_pCurrentPlot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			self.m_pCurrentPlot.setImprovementType(-1)
			self.removeLandmarkCB()
		return

	def handleEditScriptCB(self, argsList):
		iScriptData, strName = argsList
		if strName == "UnitEditScript":
			self.m_pActivePlot.getUnit(self.m_iCurrentUnit).setScriptData(str(iScriptData))
		elif strName == "CityEditScript":
			self.m_pActivePlot.getPlotCity().setScriptData(str(iScriptData))
		elif strName == "PlayerEditScript":
			GC.getPlayer(self.m_iCurrentPlayer).setScriptData(str(iScriptData))
		elif strName == "PlotEditScript":
			self.m_pScript = self.m_pActivePlot.setScriptData(str(iScriptData))
		elif strName == "GameEditScript":
			self.m_pScript = CyGame().setScriptData(str(iScriptData))
		return 1

	def setRiverHighlights(self):
		CyEngine().clearColoredPlots(PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY(), PlotStyles.PLOT_STYLE_RIVER_SOUTH, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_GREEN", 1)

		fAlpha = .2
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()+1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY()+1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY()+1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY(), PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)

		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY(), PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()-1, self.m_pRiverStartPlot.getY()-1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX(), self.m_pRiverStartPlot.getY()-1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		CyEngine().addColoredPlotAlt(self.m_pRiverStartPlot.getX()+1, self.m_pRiverStartPlot.getY()-1, PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_REVEALED_PLOTS, "COLOR_WHITE", fAlpha)
		return

	def setCurrentModeCheckbox(self, iButton):
		screen = self.getScreen()

		screen.setState("WorldBuilderUnitEditModeButton", iButton == self.m_iUnitEditCheckboxID)
		screen.setState("WorldBuilderCityEditModeButton", iButton == self.m_iCityEditCheckboxID)
		screen.setState("WorldBuilderNormalPlayerModeButton", iButton == self.m_iNormalPlayerCheckboxID)
		screen.setState("WorldBuilderNormalMapModeButton", iButton == self.m_iNormalMapCheckboxID)
		screen.setState("WorldBuilderRevealTileModeButton", iButton == self.m_iRevealTileCheckboxID)
		screen.setState("WorldBuilderDiplomacyModeButton", iButton == self.m_iDiplomacyCheckboxID)
		screen.setState("WorldBuilderLandmarkButton", iButton == self.m_iLandmarkCheckboxID)
		screen.setState("WorldBuilderEraseButton", iButton == self.m_iEraseCheckboxID)
		return

	# Tooltip
	def updateTooltip(self, screen, szTxt, xPos = -1, yPos = -1, uFont = ""):
		if not szTxt:
			return
		if szTxt != self.szTxtTT:
			self.szTxtTT = szTxt
			if not uFont:
				uFont = self.aFontList[5]
			iX, iY = pyTT.makeTooltip(screen, xPos, yPos, szTxt, uFont, "Tooltip")
			POINT = Win32.getCursorPos()
			self.iOffsetTT = [iX - POINT.x, iY - POINT.y]
		else:
			if xPos == yPos == -1:
				POINT = Win32.getCursorPos()
				screen.moveItem("Tooltip", POINT.x + self.iOffsetTT[0], POINT.y + self.iOffsetTT[1], 0)
			screen.moveToFront("Tooltip")
			screen.show("Tooltip")
		if xPos == yPos == -1:
			self.bLockedTT = True

	#--------------------------#
	# Base operation functions #
	#||||||||||||||||||||||||||#
	def update(self, fDelta):
		if self.bLockedTT:
			POINT = Win32.getCursorPos()
			iX = POINT.x + self.iOffsetTT[0]
			iY = POINT.y + self.iOffsetTT[1]
			if iX < 0: iX = 0
			if iY < 0: iY = 0
			self.getScreen().moveItem("Tooltip", iX, iY, 0)

		if not CyInterface().isLeftMouseDown():
			self.m_bLeftMouseDown = False
		if not CyInterface().isRightMouseDown():
			self.m_bRightMouseDown = False

		if not self.m_bChangeFocus and not isMouseOverGameSurface():
			self.m_bChangeFocus = True

		if self.m_bChangeFocus and isMouseOverGameSurface() and not self.m_bUnitEdit and not self.m_bCityEdit:
			self.m_bChangeFocus = False
			setFocusToCVG()


	def handleInput(self, inputClass):
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
		screen.hide("Tooltip")

		if iCode == NotifyCode.NOTIFY_CURSOR_MOVE_ON:

			if NAME == "WorldBuilderExitButton":
				self.updateTooltip(screen, TRNSLTR.getText("TXT_KEY_WB_EXIT",()))

		elif iCode == NotifyCode.NOTIFY_CLICKED:

			if NAME == "WorldBuilderExitButton":
				CyInterface().setWorldBuilder(False)

		elif iCode == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:
			if NAME == "WorldBuilderPlayerChoice":
				self.handlePlayerUnitPullDownCB(iData)
			elif NAME == "WorldBuilderGameData":
				self.handleWorldBuilderGameDataPullDownCB(iData)
			elif NAME == "WorldBuilderAddUnits":
				self.handleWorldBuilderAddUnitsPullDownCB(iData)
			elif NAME == "WorldBuilderUnitCombat":
				self.handleWorldBuilderUnitCombatPullDownCB(iData)
			elif NAME == "WorldBuilderEditAreaMap":
				self.handleWorldBuilderEditAreaMapPullDownCB(iData)
			elif NAME == "WorldBuilderModifyAreaPlotType":
				self.handleWorldBuilderModifyAreaPlotTypePullDownCB(iData)
			elif NAME == "WorldBuilderModifyAreaTerrain":
				self.handleWorldBuilderModifyAreaTerrainPullDownCB(iData)
			elif NAME == "WorldBuilderModifyAreaRoute":
				self.handleWorldBuilderModifyAreaRoutePullDownCB(iData)
			elif NAME == "WorldBuilderModifyAreaFeature":
				self.handleWorldBuilderModifyAreaFeaturePullDownCB(iData)
			elif NAME == "WorldBuilderRevealMode":
				self.handleWorldBuilderRevealModePullDownCB(iData)
			elif NAME == "WorldBuilderBrushSize":
				self.handleBrushSizeCB(iData)
			elif NAME == "WorldBuilderTeamChoice":
				self.handleSelectTeamPullDownCB(iData)

		elif iCode == NotifyCode.NOTIFY_CHARACTER:
			if iData == InputTypes.KB_ESCAPE:
				self.normalPlayerTabModeCB()
		return 1

	def onClose(self):
		print "Exit World Builder"
		del (
			self.InputData, self.szTxtTT, self.iOffsetTT, self.bLockedTT, self.xRes, self.yRes, self.aFontList,
			self.m_normalPlayerTabCtrl, self.m_normalMapTabCtrl, self.m_tabCtrlEdit, self.m_bCtrlEditUp
		)
		UP.toggleSetNoScreens()
