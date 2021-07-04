#-------------------------------------------------------------------------------
# Name:        CvDotMapOverlayScreen.py
# Purpose:     Custom Screen for the dot map of the Strategy Overlay
#
# Author:      Del69, EmperorFool
#
# Created:     09/01/2009
#-------------------------------------------------------------------------------

from CvPythonExtensions import *
import CvStrategyOverlay

g_DotMap = None

class CvDotMapOverlayScreen:

	def __init__(self, screenID):
		self.screenID = screenID
		self.currentColor = 0
		self.bLeftMouseDown = False
		self.bRightMouseDown = False
		self.currentLayer = 9
		self.currentPoint = None

	def interfaceScreen(self):
		import CvEventInterface
		self.eventManager = CvEventInterface.eventManager
		self.eventManager.addEventHandler("ResolutionChanged", self.drawWidget)
		screen = CyGInterfaceScreen("DotMapOverlayScreen", self.screenID)
		if screen.isActive():
			return
		screen.setCloseOnEscape(False)
		screen.setAlwaysShown(True)
		screen.setForcedRedraw(True)
		screen.setDimensions(0, 0, screen.getXResolution(), screen.getYResolution())
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, True)

		# Initializes the variables for the screen.
		global g_DotMap
		g_DotMap = CvStrategyOverlay.getDotMap()

		self.drawWidget()
		self.onMouseOverPlot()

	def drawWidget(self, args=None):
		print ("drawWidget", args)
		screen = CyGInterfaceScreen("DotMapOverlayScreen", self.screenID)
		KEYS = CvStrategyOverlay.COLOR_KEYS
		import ScreenResolution
		yRes = ScreenResolution.y
		dx = 8 + yRes/67
		dy = dx + 4
		# Creates the color selection panel
		iCount = len(KEYS)
		MARG = 28
		COLS = 3
		ROWS = 1 + (iCount - 1) / COLS
		x0 = 0
		y0 = 108
		w = MARG + COLS * dx
		h = MARG + ROWS * (dy - 4)
		screen.addPanel("DotMapColorPanel", "", "", True, False, x0, y0, w, h, PanelStyles.PANEL_STYLE_BLUELARGE)
		for i, color in enumerate(KEYS):
			iRow = i / COLS
			iCol = i % COLS
			x = x0 - 1 + MARG/2 + iCol * dx
			y = y0 - 2 + MARG/2 + iRow * (dy - 4)
			szBar = "DotMapColorWidget" + str(i)
			screen.addStackedBarGFC(szBar, x, y, dx, dy, 1, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.setStackedBarColors(szBar, 0, CyGlobalContext().getInfoTypeForString(color))
			screen.setBarPercentage(szBar, 0, 100.0)
	#----------------#
	# Input handlers #
	#----------------#

	def onMouseOverPlot(self):
		"""
		Called from CvOverlayScreenUtils when mousing over a plot when the screen is active.
		Updates the current plot and its x/y location.
		"""
		plot = CyInterface().getMouseOverPlot()
		x = plot.getX()
		y = plot.getY()
		self.currentPoint = (x, y)
		g_DotMap.highlightCity(self.currentPoint, self.currentColor)
		self.resetInterfaceMode()

	def resetInterfaceMode(self):
		if CyInterface().getInterfaceMode() != InterfaceModeTypes.INTERFACEMODE_DOTMAP:
			CyInterface().setInterfaceMode(InterfaceModeTypes.INTERFACEMODE_DOTMAP)

	def hideScreen(self):
		g_DotMap.unhighlightCity()
		CyInterface().setInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION)
		screen = CyGInterfaceScreen("DotMapOverlayScreen", self.screenID)
		screen.hideScreen()
		self.eventManager.removeEventHandler("ResolutionChanged", self.drawWidget)

	def update(self, fDelta):

		if CyInterface().isLeftMouseDown():
			if not self.bLeftMouseDown:
				if self.currentPoint is not None:
					g_DotMap.unhighlightCity()
					grid = 6
					layer = 9 + (self.currentPoint[0] % grid) * grid + (self.currentPoint[1] % grid)
					g_DotMap.addCityAt(self.currentPoint, self.currentColor, layer)
					self.resetInterfaceMode()
				self.bLeftMouseDown = True

		elif CyInterface().isRightMouseDown():
			if not self.bRightMouseDown:
				if self.currentPoint is not None:
					g_DotMap.unhighlightCity()
					g_DotMap.removeCityAt(self.currentPoint)
					self.resetInterfaceMode()
				self.bRightMouseDown = True
		else:
			self.bLeftMouseDown = False
			self.bRightMouseDown = False

	def handleInput(self, inputClass):
		if inputClass.eNotifyCode == NotifyCode.NOTIFY_CLICKED and inputClass.szFunctionName == "DotMapColorWidget":
			g_DotMap.unhighlightCity()
			index = inputClass.iItemID
			self.currentColor = CyGlobalContext().getInfoTypeForString(CvStrategyOverlay.COLOR_KEYS[index])
			self.currentLayer = index + 9
			g_DotMap.highlightCity(self.currentPoint, self.currentColor)
			return 1
		return 0