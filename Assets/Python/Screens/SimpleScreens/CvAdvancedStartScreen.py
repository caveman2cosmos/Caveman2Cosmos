from CvPythonExtensions import *
'''
 TAB
	ROW
--------------------
 0 = City
	0 = City
	1 = Automate
	2 = Buildings
 1 = Unit
 2 = Improvement
	0 = Route
	1 = Improvements
 3 = Visibility
 4 = Tech
'''
GC = CyGlobalContext()

class CvAdvancedStartScreen:

	def __init__(self):
		self.bInitialize = True

	def interfaceScreen(self, screenId):
		if self.bInitialize:
			self.screenId = screenId
			self.MAP = GC.getMap()
			self.iPlayer = GC.getGame().getActivePlayer()
			self.CyPlayer = GC.getPlayer(self.iPlayer)
			self.iTeam = self.CyPlayer.getTeam()
			self.iCost = 0
			self.x = -1
			self.y = -1
			self.plot = 0
			self.selectionIndexes = [0, 0, 0, 0, 0]
			self.selectionRows = [0, 0, 0, 0, 0]
			self.iTabCity = 0
			self.iRowCity = 0
			self.iRowAutomation = 1
			self.iRowBuildings = 2
			self.iTabUnit = 1
			self.iTabImprovement = 2
			self.iRowRoutes = 0
			self.iRowImprovements = 1
			self.iTabVisibility = 3
			self.iTabTech = 4
			screen = CyGInterfaceScreen("AdvancedStartScreen", screenId)
			screen.setCloseOnEscape(False)
			screen.setAlwaysShown(True)
			screen.setForcedRedraw(True)
			screen.addPanel("", "", "", True, True, 0, 0, 226, 84, PanelStyles.PANEL_STYLE_MAIN)
			txt = CyTranslator().getText("TXT_KEY_ADVANCED_START_BEGIN_GAME", ())
			screen.setButtonGFC("WorldBuilderExitButton", txt, "", 50, 45, 130, 28, WidgetTypes.WIDGET_WB_EXIT_BUTTON, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)

			self.initAdvancedStartTabCtrl()
			self.refreshASItemCost()
			screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, True)

			self.bInitialize = False


	def refreshASItemCost(self):
		iCost = 0
		if self.isCityBrush():
			iCost = self.CyPlayer.getAdvancedStartCityCost(True, None)

		elif self.plot != 0:
			if self.plot.isRevealed(self.iTeam, False):

				if self.getActiveUnit() != -1:
					iCost = self.CyPlayer.getAdvancedStartUnitCost(self.getActiveUnit(), True, self.plot)
				elif self.isPopulationBrush() and self.plot.isCity():
					iCost = self.CyPlayer.getAdvancedStartPopCost(True, self.plot.getPlotCity())
				elif self.isCultureBrush() and self.plot.isCity():
					iCost = self.CyPlayer.getAdvancedStartCultureCost(True, self.plot.getPlotCity())
				elif self.getActiveBuilding() != -1 and self.plot.isCity():
					iCost = self.CyPlayer.getAdvancedStartBuildingCost(self.getActiveBuilding(), True, self.plot.getPlotCity())
				elif self.getActiveRoute() != -1:
					iCost = self.CyPlayer.getAdvancedStartRouteCost(self.getActiveRoute(), True, self.plot)
				elif self.getActiveImprovement() != -1:
					iCost = self.CyPlayer.getAdvancedStartImprovementCost(self.getActiveImprovement(), True, self.plot)

			elif self.plot.isAdjacentNonrevealed(self.iTeam) and self.CyGTabCtrl.getActiveTab() == self.iTabVisibility:
				iCost = self.CyPlayer.getAdvancedStartVisibilityCost(self.plot)

		if iCost < 0: iCost = 0
		self.iCost = iCost

		screen = CyGInterfaceScreen("AdvancedStartScreen", self.screenId)
		txt = "<font=4>" + CyTranslator().getText("TXT_KEY_WB_AS_POINTS", (self.CyPlayer.getAdvancedStartPoints(),))
		screen.setLabel("AdvancedStartPointsText", "", txt, 1<<0, 50, 15, -2, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		txt = "<font=4>" + CyTranslator().getText("TXT_KEY_WB_AS_COST_THIS_LOCATION", (self.iCost, ))
		screen.setLabel("AdvancedStartCostText", "", txt, 1<<0, 30, 85, -2, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		self.bDirty = False

	def getActiveUnit(self):
		if self.CyGTabCtrl.getActiveTab() == self.iTabUnit:
			return getASUnit(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
		return -1

	def isCityBrush(self):
		return self.CyGTabCtrl.getActiveTab() == self.iTabCity \
			and self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowCity \
			and self.selectionIndexes[self.CyGTabCtrl.getActiveTab()] == 0

	def isPopulationBrush(self):
		return self.CyGTabCtrl.getActiveTab() == self.iTabCity \
			and self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowCity \
			and self.selectionIndexes[self.CyGTabCtrl.getActiveTab()] == 1

	def isCultureBrush(self):
		return self.CyGTabCtrl.getActiveTab() == self.iTabCity \
			and self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowCity \
			and self.selectionIndexes[self.CyGTabCtrl.getActiveTab()] == 2

	def getActiveBuilding(self):
		if self.CyGTabCtrl.getActiveTab() == self.iTabCity \
		and self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowBuildings:
			return getASBuilding(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
		return -1

	def getActiveRoute(self):
		if self.CyGTabCtrl.getActiveTab() == self.iTabImprovement \
		and self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowRoutes:
			iRouteType = getASRoute(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
			if -1 == iRouteType:
				self.selectionRows[self.CyGTabCtrl.getActiveTab()] = self.iRowImprovements
			return iRouteType
		return -1

	def getActiveImprovement(self):
		if self.CyGTabCtrl.getActiveTab() == self.iTabImprovement \
		and self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowImprovements:
			iImprovementType = getASImprovement(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
			if -1 == iImprovementType:
				self.selectionRows[self.CyGTabCtrl.getActiveTab()] = self.iRowRoutes
			return iImprovementType
		return -1

	def placeObject(self):
		if self.x == -1 or self.y == -1:
			return
		pPlot = self.MAP.plot(self.x, self.y)
		if self.plot.isRevealed(self.iTeam, False):
			# City Tab
			if self.CyGTabCtrl.getActiveTab() == self.iTabCity:
				# City List
				if self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowCity:
					iOptionID = self.selectionIndexes[self.CyGTabCtrl.getActiveTab()]
					# Place City
					if iOptionID == 0:
						if self.CyPlayer.getAdvancedStartCityCost(True, pPlot) > -1:
							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_CITY, self.iPlayer, self.x, self.y, -1, True)

					# City Population
					elif iOptionID == 1:
						if pPlot.isCity() and self.CyPlayer.getAdvancedStartPopCost(True, pPlot.getPlotCity()) > -1:
							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_POP, self.iPlayer, self.x, self.y, -1, True)

					# City Culture
					elif iOptionID == 2:
						if pPlot.isCity() and self.CyPlayer.getAdvancedStartCultureCost(True, pPlot.getPlotCity()) > -1:
							CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_CULTURE, self.iPlayer, self.x, self.y, -1, True)

				# Buildings List
				elif self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowBuildings and pPlot.isCity():
					iBuildingType = getASBuilding(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
					if iBuildingType != -1 and self.CyPlayer.getAdvancedStartBuildingCost(iBuildingType, True, pPlot.getPlotCity()) != -1:
						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_BUILDING, self.iPlayer, self.x, self.y, iBuildingType, True)

			# Unit Tab
			elif self.CyGTabCtrl.getActiveTab() == self.iTabUnit:
				iUnitType = getASUnit(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
				if iUnitType > -1 and self.CyPlayer.getAdvancedStartUnitCost(iUnitType, True, pPlot) > -1:
					CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_UNIT, self.iPlayer, self.x, self.y, iUnitType, True)

			# Improvements Tab
			elif self.CyGTabCtrl.getActiveTab() == self.iTabImprovement:
				# Routes List
				if self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowRoutes:
					iRouteType = getASRoute(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
					if iRouteType > -1 and self.CyPlayer.getAdvancedStartRouteCost(iRouteType, True, pPlot) > -1:
						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_ROUTE, self.iPlayer, self.x, self.y, iRouteType, True)

				# Improvements List
				elif self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowImprovements:
					iImprovementType = getASImprovement(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
					if iImprovementType > -1 and self.CyPlayer.getAdvancedStartImprovementCost(iImprovementType, True, pPlot) > -1:
						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_IMPROVEMENT, self.iPlayer, self.x, self.y, iImprovementType, True)

		# Adjacent nonrevealed
		elif self.CyGTabCtrl.getActiveTab() == self.iTabVisibility and self.CyPlayer.getAdvancedStartVisibilityCost(pPlot) > -1:
			CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_VISIBILITY, self.iPlayer, self.x, self.y, -1, True)

		self.bDirty = True


	def removeObject(self):
		if self.x == -1 or self.y == -1 or not self.plot.isRevealed(self.iTeam, False):
			return

		# City Tab
		if self.CyGTabCtrl.getActiveTab() == self.iTabCity:
			pPlot = self.MAP.plot(self.x, self.y)
			# City List
			if self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowCity:
				# City Population
				if self.selectionIndexes[self.CyGTabCtrl.getActiveTab()] == 1 \
				and pPlot.isCity() and pPlot.getPlotCity().getOwner() == self.iPlayer:

					CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_POP, self.iPlayer, self.x, self.y, -1, False)
			# Buildings List
			elif self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowBuildings \
			and pPlot.isCity() and pPlot.getPlotCity().getOwner() == self.iPlayer:

				iBuildingType = getASBuilding(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
				if -1 != iBuildingType:
					CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_BUILDING, self.iPlayer, self.x, self.y, iBuildingType, False)

		# Unit Tab
		elif self.CyGTabCtrl.getActiveTab() == self.iTabUnit:
			iUnitType = getASUnit(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
			if -1 != iUnitType:
				CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_UNIT, self.iPlayer, self.plot.getX(), self.plot.getY(), iUnitType, False)

		# Improvements Tab
		elif self.CyGTabCtrl.getActiveTab() == self.iTabImprovement:
			# Routes List
			if self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowRoutes:
				iRouteType = getASRoute(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
				if -1 != iRouteType:
					CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_ROUTE, self.iPlayer, self.x, self.y, iRouteType, False)

			# Improvements List
			elif self.selectionRows[self.CyGTabCtrl.getActiveTab()] == self.iRowImprovements:
				iImprovementType = getASImprovement(self.selectionIndexes[self.CyGTabCtrl.getActiveTab()])
				if -1 != iImprovementType:
					CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_IMPROVEMENT, self.iPlayer, self.x, self.y, iImprovementType, False)
		else: return
		self.bDirty = True

	def setCurrentSelection(self, iTab, iIndex, iList):
		self.selectionIndexes[iTab] = iIndex
		self.selectionRows[iTab] = iList
		# Switch focus back to camera after interacting with advanced start panel
		setFocusToCVG() # Allows camera zoom scrolling without clicking the map first

	def initAdvancedStartTabCtrl(self):
		initWBToolAdvancedStartControl()
		self.CyGTabCtrl = CyGTabCtrl = getWBToolAdvancedStartTabCtrl()
		CyGTabCtrl.setSize(600, 188)
		CyGTabCtrl.addTabSection(CyTranslator().getText("TXT_KEY_WB_AS_CITIES",()))
		CyGTabCtrl.addTabSection(CyTranslator().getText("TXT_KEY_WB_AS_UNITS",()))
		CyGTabCtrl.addTabSection(CyTranslator().getText("TXT_KEY_WB_AS_IMPROVEMENTS",()))
		CyGTabCtrl.addTabSection(CyTranslator().getText("TXT_KEY_WB_AS_VISIBILITY",()))
		CyGTabCtrl.addTabSection(CyTranslator().getText("TXT_KEY_WB_AS_TECH",()))
		addWBAdvancedStartControlTabs()

#--------------------------------------------------------#
# Functions called by the exe through CvScreensInterface #
#--------------------------------------------------------#
	def getCityTab(self):
		return self.iTabCity

	def getCityRow(self):
		return self.iRowCity

	def getBuildingsRow(self):
		return self.iRowBuildings

	def getAutomationRow(self):
		return self.iRowAutomation

	def getUnitTab(self):
		return self.iTabUnit

	def getImprovementTab(self):
		return self.iTabImprovement

	def getRoutesRow(self):
		return self.iRowRoutes

	def getImprovementsRow(self):
		return self.iRowImprovements

	def getVisibilityTab(self):
		return self.iTabVisibility

	def getTechTab(self):
		return self.iTabTech

	def getHighlightPlot(self, argsList):
		self.refreshASItemCost()

		if self.plot != 0 and self.iCost > 0 and isMouseOverGameSurface():
			return (self.x, self.y)
		return ()

	def mouseOverPlot(self):
		self.plot = CyInterface().getMouseOverPlot()
		self.x = self.plot.getX()
		self.y = self.plot.getY()

		if CyInterface().isLeftMouseDown():
			self.placeObject()
		elif CyInterface().isRightMouseDown():
			self.removeObject()

	def leftMouseDown(self, argsList):
		#bShift, bCtrl, bAlt = argsList
		self.placeObject()

	def rightMouseDown(self):
		self.removeObject()

	def updateScreen(self):
		if self.bDirty:
			self.refreshASItemCost()

		if CyInterface().isDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT):
			iActiveTab = self.CyGTabCtrl.getActiveTab()
			self.initAdvancedStartTabCtrl()
			self.CyGTabCtrl.setActiveTab(iActiveTab)
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, False)

	def killScreen(self):
		screen = CyGInterfaceScreen("AdvancedStartScreen", self.screenId)
		screen.hideScreen()

		del self.screenId, self.MAP, self.iTeam, self.iPlayer, self.CyPlayer, \
			self.x, self.y, self.plot, self.selectionIndexes, self.selectionRows, self.bDirty, self.iCost,\
			self.iTabCity, self.iTabUnit, self.iTabImprovement, self.iTabVisibility, self.iTabTech, \
			self.iRowCity, self.iRowAutomation, self.iRowBuildings, self.iRowRoutes, self.iRowImprovements
		self.bInitialize = True
