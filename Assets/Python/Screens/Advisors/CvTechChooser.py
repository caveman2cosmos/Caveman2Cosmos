from CvPythonExtensions import *
import CvScreensInterface as UP
import HandleInputUtil
import PythonToolTip as pyTT
from sys import maxint

GC = CyGlobalContext()
TRNSLTR = CyTranslator()
AFM = CyArtFileMgr()

CIV_NO_RESEARCH = -1
CIV_HAS_TECH = 0
CIV_TECH_AVAILABLE = 1
CIV_IS_QUEUED = 2
CIV_IS_RESEARCHING = 3
CIV_IS_TARGET = 4

FONT_COLOR_MAP = {
	CIV_IS_QUEUED: "<color=255,255,255,255>",
	CIV_IS_RESEARCHING: "<color=255,255,255,255>",
	CIV_IS_TARGET: "<color=255,255,255,255>",
	# CIV_IS_QUEUED: "<color=255,255,10,255>",
	# CIV_IS_RESEARCHING: "<color=0,255,10,255>",
	# CIV_IS_TARGET: "<color=255,176,10,255>",
}
ICON = "ICON"
TECH_CHOICE = "WID|TECH|CHOICE"
TECH_REQ = "WID|TECH|REQ"
TECH_NAME = "TechName"
SCREEN_PANEL = "TechList"
CELL_BORDER_W = 20
CELL_BORDER_H = 0

ADVISORS = [unichr(8855), unichr(8857), unichr(8500), unichr(8501), unichr(8502), unichr(8483)]

# Top panel height
SCREEN_PANEL_TOP_BAR_H = 42
# Bottom panel height
SCREEN_PANEL_BOTTOM_BAR_H = 80
# Left/right border for the slider
SLIDER_BORDER = 50
# How many techs to page in per update tick, more is faster but more introduces more stutter
# Could use a timer instead of a fixed count to allocate time slice for paging for more consistent behaviour
TECH_PAGING_RATE = 4
# Gap between techs
CELL_GAP = CELL_BORDER_W * 2 + 32
# Size of dependency arrows
ARROW_SIZE = 8
BOTTOM_BAR_NAME = "WID|BAR|BOTTOMBAR"
BOTTOM_BAR_ID = BOTTOM_BAR_NAME + "0"
BOTTOM_BAR_SLIDER_PANEL_ID = "TC_BarBotSlider"
HSLIDER_ID = "HSlider"
MINIMAP_TOP_MARGIN = 30
MINIMAP_LENS_ID = "MinimapLens"
MINIMAP_LENS_BORDER_H = 4
MINIMAP_LENS_BORDER_V = 4

PROGRESSBAR_H = 14

QUEUE_LABEL_PANEL = "QUEUE_LABEL_PANEL"
QUEUE_LABEL = "QUEUE_LABEL"

QUEUE_LABEL_W = 54
QUEUE_LABEL_H = 32

FOREGROUND_PARA_H = 256
BACKGROUND_PARA_AMOUNT = 100
FOREGROUND_PARA_AMOUNT = 200


class CvTechChooser:

	def __init__(self):
		print "CvTechChooser.__init__"
		self.scrollOffs = 0
		self.iNumEras = GC.getNumEraInfos()
		self.iNumTechs = GC.getNumTechInfos()
		self.created = False
		self.skipNextExitKey = True
		self.demoMode = False
		self.cacheBenefits()

	def screen(self):
		return CyGInterfaceScreen("TechChooser", self.screenId)

	def getTechState(self, iTech):
		if self.CyTeam.isHasTech(iTech):
			return CIV_HAS_TECH
		if not self.CyPlayer.canResearch(iTech, False):
			return CIV_NO_RESEARCH
		if not self.CyPlayer.isResearchingTech(iTech):
			return CIV_TECH_AVAILABLE
		queuePos = self.CyPlayer.getQueuePosition(iTech)
		if queuePos == 1:
			return CIV_IS_RESEARCHING
		elif queuePos == self.CyPlayer.getLengthResearchQueue():
			return CIV_IS_TARGET
		else:
			return CIV_IS_QUEUED

	def initForPlayer(self, iPlayer):
		screen = self.screen()

		self.iPlayer = iPlayer
		self.CyPlayer = GC.getPlayer(iPlayer)
		self.CyTeam = GC.getTeam(self.CyPlayer.getTeam())
		self.iCurrentEra = self.CyPlayer.getCurrentEra()
		self.currentTechState = [self.getTechState(iTech) for iTech in xrange(self.iNumTechs)]

		self.tooltip = pyTT.PythonToolTip()

		self.iUnitTT = None
		self.bUnitTT = False

		self.scrolling = False
		self.updates = []
		self.cellDetails = [False] * self.iNumTechs

		self.iCurrentResearch = -1
		self.iSelectedTech = -1
		if self.CyPlayer.getAdvancedStartPoints() > -1:
			screen.setButtonGFC("AddTechButton", TRNSLTR.getText("TXT_KEY_WB_AS_ADD_TECH", ()), "", self.xRes/2 - 158, 4, 150, 30, WidgetTypes.WIDGET_GENERAL, 1, 2, ButtonStyles.BUTTON_STYLE_STANDARD)
			screen.hide("AddTechButton")

				# Main scrolling panel
		screen.addScrollPanel(SCREEN_PANEL, "", 0, 0, self.maxX + self.xCellDist, self.yRes, PanelStyles.PANEL_STYLE_EMPTY)
		screen.setStyle(SCREEN_PANEL, "Panel_TechMinimapCell_Style")
		# screen.setHitTest(SCREEN_PANEL, HitTestTypes.HITTEST_NOHIT)

		# Minimap background
		screen.addPanel(BOTTOM_BAR_ID, "", "", True, False, -20, self.yRes - SCREEN_PANEL_BOTTOM_BAR_H - 80, self.xRes + 40, SCREEN_PANEL_BOTTOM_BAR_H + 100, PanelStyles.PANEL_STYLE_MAIN_TANB)
		screen.setStyle(BOTTOM_BAR_ID, "Panel_TechMinimap_Style")
		screen.setHitTest(BOTTOM_BAR_ID, HitTestTypes.HITTEST_NOHIT)

		# Era backgrounds and buttons that can jump directly to an era
		lastPosX = 0

		self.backdropPanelPos = []
		for i in xrange(self.iNumEras - 1):
			posX = self.treeToMinimapX(self.minEraXPos[i] - self.minX) # SLIDER_BORDER + self.minEraXPos[i] * (self.xRes - SLIDER_BORDER * 2) / self.maxX
			posY = self.yRes - SCREEN_PANEL_BOTTOM_BAR_H + 5
			eraInfo = GC.getEraInfo(i)
			img = eraInfo.getButton()
			if img: # and i < self.iNumEras - 1: # exclude future icon
				screen.setText("WID|ERAIM|" + str(i), "", "<img=%s>" % (img), 0, posX - 4, posY, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, 0, 0)
			if i > 0:
				screen.addPanel("WID|ERAPANEL|" + str(i-1), "", "", False, False, lastPosX, posY, posX - lastPosX, SCREEN_PANEL_BOTTOM_BAR_H, PanelStyles.PANEL_STYLE_DEFAULT)

			# Backdrop
			panelStartX = self.minEraXPos[i] - self.minX + self.xCellDist / 2
			panelEndX = self.minEraXPos[i+1] - self.minX + self.xCellDist / 2 + 4
			if i == 0:
				panelStartX = panelStartX - self.xCellDist / 2
			if i == self.iNumEras - 2:
				panelEndX = panelEndX + self.xCellDist / 2

			# Save the panel x coords for later
			self.backdropPanelPos.append((panelStartX, panelEndX))

			# Parallax Container
			bgPanelWid = panelEndX - panelStartX
			bgPanelHgt = self.yRes - SCREEN_PANEL_BOTTOM_BAR_H - SCREEN_PANEL_TOP_BAR_H
			backDropPanelName = "ERA_BG_PANEL_" + str(i)
			screen.attachPanelAt(SCREEN_PANEL, backDropPanelName, "", "", False, False, PanelStyles.PANEL_STYLE_STANDARD, panelStartX, 0, bgPanelWid, bgPanelHgt, WidgetTypes.WIDGET_GENERAL, 0, 0)
			# Background parallax layer
			bgName = "ERA_BG_" + str(i)
			screen.setImageButtonAt(bgName, backDropPanelName, "", 0, 0, self.xRes + BACKGROUND_PARA_AMOUNT * 2, bgPanelHgt, WidgetTypes.WIDGET_GENERAL, 1, 2)
			screen.setStyle(bgName, self.getBackgroundStyleForEra(i))
			screen.setHitTest(bgName, HitTestTypes.HITTEST_NOHIT)
			# Foreground parallax layer
			fgName = "ERA_FG_" + str(i)
			screen.setImageButtonAt(fgName, backDropPanelName, "", 0, 0, self.xRes + FOREGROUND_PARA_AMOUNT * 2, bgPanelHgt, WidgetTypes.WIDGET_GENERAL, 1, 2)
			screen.setStyle(fgName, self.getForegroundStyleForEra(i))
			screen.setHitTest(fgName, HitTestTypes.HITTEST_NOHIT)

			# Label for eras, but it looks crap in tiny fonts, need a massive font
			# screen.setLabelAt("ERA_LABEL_" + str(i), backDropPanelName, "<font=4b>" + eraInfo.getDescription(), 1 << 0, 30, 30, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
			# screen.setHitTest("ERA_LABEL_" + str(i), HitTestTypes.HITTEST_NOHIT)

			lastPosX = posX

		# A panel to put the horizontal slider in so we can position it correctly
		#screen.addPanel(BOTTOM_BAR_SLIDER_PANEL_ID, "", "", False, False, SLIDER_BORDER, self.yRes - SCREEN_PANEL_BOTTOM_BAR_H - 12, self.xRes, SCREEN_PANEL_BOTTOM_BAR_H, PanelStyles.PANEL_STYLE_EMPTY)
		minimapWidth = self.xRes - SLIDER_BORDER * 2
		fullWidth = self.maxX - self.minX
		self.minimapLensWidth = self.xRes * minimapWidth / fullWidth + MINIMAP_LENS_BORDER_H * 2
		screen.addPanel(MINIMAP_LENS_ID, "", "", False, False, 0, 0, self.minimapLensWidth, SCREEN_PANEL_BOTTOM_BAR_H, PanelStyles.PANEL_STYLE_MAIN_WHITE)

		# Create the tech button backgrounds
		self.refresh(xrange(self.iNumTechs), False)

		# Delay first scroll position update so UI can initialize first
		self.delayedScroll = 2

	def interfaceScreen(self, screenId):
		print "CvTechChooser.interfaceScreen"

		if GC.getGame().isPitbossHost():
			print "CvTechChooser.interfaceScreen - skipping, isPitbossHost"
			return

		# Make sure we don't initialize twice
		if self.created:
			print "CvTechChooser.interfaceScreen - skipping, already created"
			return

		self.mwlHandle = Win32.registerMouseWheelListener()

		self.created = True
		self.skipNextExitKey = True
		self.screenId = screenId

		import InputData
		self.InputData = InputData.instance

		# Set up widget sizes
		import ScreenResolution as SR
		self.xRes = SR.x
		self.yRes = SR.y
		self.aFontList = SR.aFontList

		self.wCell = 128 + self.xRes / 6

		self.xCellDist = CELL_GAP + self.wCell

		if self.yRes > 1000:
			self.sIcon0 = 64
		elif self.yRes > 800:
			self.sIcon0 = 56
		else:
			self.sIcon0 = 48

		self.hCell = self.sIcon0 + 8
		self.sIcon1 = self.sIcon0 / 2

		# Cache minimum X coordinate per era for era partitioning.
		self.minEraXPos = [maxint] * self.iNumEras
		self.firstEraTech = [(maxint, maxint)] * self.iNumEras
		self.minX = maxint
		self.maxX = 0
		for iTech in xrange(self.iNumTechs):
			info = GC.getTechInfo(iTech)
			gridX = info.getGridX()
			if gridX > 0:
				iX = gridX * self.xCellDist
				iX1 = (gridX + 1) * self.xCellDist
				iEra = info.getEra()
				if iX < self.minEraXPos[iEra]:
					self.minEraXPos[iEra] = iX
				if gridX < self.firstEraTech[iEra][0]:
					self.firstEraTech[iEra] = (gridX, iTech)
				if iX1 > self.maxX:
					self.maxX = iX1
				if iX < self.minX:
					self.minX = iX

		self.minimapScaleX = (self.xRes - (SLIDER_BORDER * 2)) / float(self.maxX - self.minX)

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		# Base Screen
		screen = self.screen()
		screen.addDDSGFC("ScreenBackground", AFM.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, self.xRes, self.yRes, eWidGen, 1, 2)

		screen.addPanel("TC_BarTop", "", "", True, False, 0, 0, self.xRes, SCREEN_PANEL_TOP_BAR_H, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.setLabel("TC_Header", "", "<font=4b>" + TRNSLTR.getText("TXT_KEY_TECHHELP_CHOOSER_TITLE", ()), 1<<2, self.xRes/2, 4, 0, eFontTitle, eWidGen, 1, 2)

		screen.setText("TC_Exit", "", "<font=4b>" + TRNSLTR.getText("TXT_WORD_EXIT", ()), 1<<1, self.xRes - 8, 2, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		stackBar = "progressBar"
		screen.addStackedBarGFC(stackBar, 256, 2, self.xRes - 512, 32, InfoBarTypes.NUM_INFOBAR_TYPES, eWidGen, 1, 2)
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_STORED, GC.getInfoTypeForString("COLOR_RESEARCH_STORED"))
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_RATE, GC.getInfoTypeForString("COLOR_RESEARCH_RATE"))
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_EMPTY, GC.getInfoTypeForString("COLOR_EMPTY"))
		screen.hide(stackBar)
		screen.setImageButton("WID|TECH|CURRENT0", "", 256, 3, self.xRes - 512, 30, eWidGen, 1, 2)
		screen.hide("WID|TECH|CURRENT0")

		screen.setRenderInterfaceOnly(True)
		screen.showWindowBackground(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		iPlayer = GC.getGame().getActivePlayer()
		# Debug
		import DebugUtils
		if DebugUtils.bDebugMode:
			DDB = "TC_DebugDD"
			screen.addDropDownBoxGFC(DDB, 4, 2, 200, eWidGen, 1, 2, eFontTitle)
			for iPlayerX in range(GC.getMAX_PLAYERS()):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					screen.addPullDownString(DDB, CyPlayerX.getName(), iPlayerX, iPlayerX, iPlayer == iPlayerX)

		self.initForPlayer(iPlayer)

		print "CvTechChooser.interfaceScreen - DONE"

	def refresh(self, techs, bFull):
		screen = self.screen()

		eWidGen = WidgetTypes.WIDGET_GENERAL

		dy = self.yRes - SCREEN_PANEL_TOP_BAR_H - SCREEN_PANEL_BOTTOM_BAR_H

		yEmptySpace = (dy - 10 * self.hCell) / 10
		if yEmptySpace < 0:
			yEmptySpace = 0
		yCellDist = yEmptySpace + self.hCell
		yEmptySpace /= 2
		yBoxSpacing = yCellDist/2

		# Place techs
		RED_X = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_RED_X").getPath()
		ARROW_X = AFM.getInterfaceArtInfo("ARROW_X").getPath()
		ARROW_Y = AFM.getInterfaceArtInfo("ARROW_Y").getPath()
		ARROW_HEAD = AFM.getInterfaceArtInfo("ARROW_HEAD").getPath()

		yArrow0 = self.hCell / 2
		yArrow1 = self.hCell*3/8
		yArrow2 = self.hCell / 4
		yArrow3 = self.hCell*5/8
		yArrow4 = self.hCell*3/4
		techBenefits = self.techBenefits
		dx = self.sIcon1 + 1
		iMaxElements = (self.wCell - self.sIcon0 - 8) / dx

		minimapWid = int(self.xCellDist * self.minimapScaleX) - 2
		if minimapWid < 2:
			minimapWid = 2

		for iTech in techs:
			CvTechInfo = GC.getTechInfo(iTech)

			x0 = CvTechInfo.getGridX()
			if x0 < 1:
				continue

			iTechStr = str(iTech)
			techCellId = TECH_CHOICE + iTechStr
			y0 = CvTechInfo.getGridY()

			iX = x0 * self.xCellDist - self.minX
			iY = yEmptySpace + ((y0 - 1) * yCellDist) / 2

			if not bFull:
				# Minimap cell
				techMinimapCellId = TECH_CHOICE + "MM" + "|" + iTechStr
				screen.addPanel(techMinimapCellId, "", "", False, False, int(iX * self.minimapScaleX) + SLIDER_BORDER, self.yRes - SCREEN_PANEL_BOTTOM_BAR_H + MINIMAP_TOP_MARGIN + y0 * 2, minimapWid, 4, PanelStyles.PANEL_STYLE_STONE)
				screen.setStyle(techMinimapCellId, "Panel_TechMinimapCell_Style")
				screen.setHitTest(techMinimapCellId, HitTestTypes.HITTEST_NOHIT)

				#self.cellPos[iTech] = (iX, iY)
				# self.updates.append((iX, iTech))
			else:
				if not self.cellDetails[iTech]:
					self.cellDetails[iTech] = True

					iEra = CvTechInfo.getEra()
					bY = iY
					bH = self.hCell + CELL_BORDER_H * 2

					# Tech cell
					screen.setImageButtonAt(techCellId, SCREEN_PANEL, "", iX, iY, self.wCell + CELL_BORDER_W * 2, self.hCell + CELL_BORDER_H * 2, eWidGen, 1, 2)
					# screen.setHitTest(techCellId, HitTestTypes.HITTEST_CHILDREN)
					screen.addDDSGFCAt(ICON + iTechStr, techCellId, CvTechInfo.getButton(), 3 + CELL_BORDER_W, 5 + CELL_BORDER_H, self.sIcon0, self.sIcon0, eWidGen, 1, 2, False)
					screen.setHitTest(ICON + iTechStr, HitTestTypes.HITTEST_NOHIT)
					screen.moveToFront(techCellId)

					# Progress bar
					barId = techCellId + "BAR"
					screen.addStackedBarGFCAt(barId, techCellId, CELL_BORDER_W, CELL_BORDER_H + self.hCell - 6, self.wCell, PROGRESSBAR_H, InfoBarTypes.NUM_INFOBAR_TYPES, eWidGen, 1, 2)
					screen.setStackedBarColorsRGB(barId, InfoBarTypes.INFOBAR_STORED, 0, 215, 50, 255)
					screen.setStackedBarColorsRGB(barId, InfoBarTypes.INFOBAR_RATE, 255, 255, 255, 64)
					screen.setStackedBarColorsRGB(barId, InfoBarTypes.INFOBAR_EMPTY, 0, 0, 0, 0)
					screen.hide(barId)

					# Queue label
					screen.setImageButtonAt(QUEUE_LABEL_PANEL + iTechStr, SCREEN_PANEL, "", iX + CELL_BORDER_W - QUEUE_LABEL_W / 2, iY + CELL_BORDER_H - QUEUE_LABEL_H / 2, QUEUE_LABEL_W, QUEUE_LABEL_H, eWidGen, 1, 2)
					screen.setStyle(QUEUE_LABEL_PANEL + iTechStr, "Button_TechQueuePos_Style")
					screen.setPanelColor(QUEUE_LABEL_PANEL + iTechStr, 0, 0, 0)
					screen.setHitTest(QUEUE_LABEL_PANEL + iTechStr, HitTestTypes.HITTEST_NOHIT)
					screen.hide(QUEUE_LABEL_PANEL + iTechStr)

					# Requires
					iX = self.wCell + CELL_BORDER_W - 2
					iY = 5 + CELL_BORDER_H
					for iTechX in CvTechInfo.getPrereqAndTechs():
						iX -= dx
						screen.setImageButtonAt(TECH_REQ + str(iTechX) + "|" + iTechStr, techCellId, GC.getTechInfo(iTechX).getButton(), iX, iY, self.sIcon1, self.sIcon1, eWidGen, 1, 2)

					# Draw connecting arrows
					for iTechX in CvTechInfo.getPrereqOrTechs():
						techInfoX = GC.getTechInfo(iTechX)

						x1 = techInfoX.getGridX()
						y1 = techInfoX.getGridY()
						iX = x1 * self.xCellDist + self.wCell + CELL_BORDER_W * 2 - self.minX
						iY = yEmptySpace + ((y1 - 1) * yCellDist) / 2 + 4

						xDiff = x0 - x1
						yDiff = y0 - y1
						xOff = xDiff * CELL_GAP + (xDiff - 1) * self.wCell - CELL_BORDER_W * 2

						# Helper functions for drawing the tech dependency arrows
						def add_arrow_head(x, y):
							screen.addDDSGFCAt("", SCREEN_PANEL, ARROW_HEAD, x, y, ARROW_SIZE, ARROW_SIZE, eWidGen, 1, 2, False)

						def add_line_h(x, y, len):
							screen.addDDSGFCAt("", SCREEN_PANEL, ARROW_X, x, y, len, ARROW_SIZE, eWidGen, 1, 2, False)

						def add_line_v(x, y, len):
							screen.addDDSGFCAt("", SCREEN_PANEL, ARROW_Y, x, y, ARROW_SIZE, len, eWidGen, 1, 2, False)

						if not yDiff:
							add_line_h(iX, iY + yArrow0, xOff)
							add_arrow_head(iX + xOff, iY + yArrow0)
						elif yDiff < 0:
							if yDiff < -3 and xDiff == 1:
								dy = yDiff * yBoxSpacing + self.hCell/2
								yArrow = iY + yArrow2
								add_line_h(iX, yArrow, xOff/3 + 4)
								add_line_v(iX + xOff/3, yArrow + 4 + dy, -dy)
								add_line_h(iX + 4 + xOff/3, yArrow + dy, xOff * 2/3)
								add_arrow_head(iX + xOff, yArrow + dy)
							else:
								dy = yDiff * yBoxSpacing + self.hCell/4
								yArrow = iY + yArrow1
								add_line_h(iX, yArrow, xOff/2 + 4)
								add_line_v(iX + xOff/2, yArrow + 4 + dy, -dy)
								add_line_h(iX + 4 + xOff/2, yArrow + dy, xOff/2)
								add_arrow_head(iX + xOff, yArrow + dy)
						else:
							if yDiff > 3 and xDiff == 1:
								dy = yDiff * yBoxSpacing - self.hCell/2
								yArrow = iY + yArrow4
								add_line_h(iX, yArrow, xOff/3 + 4)
								add_line_v(iX + xOff/3, yArrow + 4, dy)
								add_line_h(iX + 4 + xOff/3, yArrow + dy, xOff * 2/3)
								add_arrow_head(iX + xOff, yArrow + dy)
							else:
								dy = yDiff * yBoxSpacing - self.hCell/4
								yArrow = iY + yArrow3
								add_line_h(iX, yArrow, xOff/2 + 4)
								add_line_v(iX + xOff/2, yArrow + 4, dy)
								add_line_h(iX + 4 + xOff/2, yArrow + dy, xOff/2)
								add_arrow_head(iX + xOff, yArrow + dy)

					# Draw unlocks
					iX = self.sIcon0 + 6 + CELL_BORDER_W
					iY = self.sIcon0 + 4 - self.sIcon1 + CELL_BORDER_H

					benefits = techBenefits[iTech]
					iLength = len(benefits)
					if iLength > iMaxElements:
						iLength = iMaxElements

					for i in xrange(iLength):
						sType, iItem = benefits[i]

						# Helpers for drawing the unlocks
						def imageButton(key, button):
							screen.setImageButtonAt(key + str(iTech * 1000 + i), techCellId, button, iX, iY, self.sIcon1, self.sIcon1, eWidGen, 1, 2)

						def ddsgfc(key, button, widgetType, tech, item, flag):
							screen.addDDSGFCAt(key + str(iTech * 1000 + i), techCellId, button, iX, iY, self.sIcon1, self.sIcon1, widgetType, tech, item, flag)

						if sType == "UnlockUnit":
							imageButton("WID|UNIT" + str(iItem) + '|', GC.getUnitInfo(iItem).getButton())
						elif sType == "UnlockBuilding":
							imageButton("WID|BUILDING" + str(iItem) + '|', GC.getBuildingInfo(iItem).getButton())
						elif sType == "ObsoleteBuilding":
							ddsgfc("", GC.getBuildingInfo(iItem).getButton(), eWidGen, 1, 2, False)
							imageButton("WID|BUILDING|OBS" + str(iItem) + '|', RED_X)
						elif sType == "UnlockSpecialBuilding":
							ddsgfc("Item", GC.getSpecialBuildingInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_SPECIAL_BUILDING, iTech, iItem, False)
						elif sType == "ObsoleteSpecialBuilding":
							ddsgfc("Item", GC.getSpecialBuildingInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_OBSOLETE_SPECIAL, iTech, iItem, False)
							ddsgfc("Obsolete", RED_X, WidgetTypes.WIDGET_HELP_OBSOLETE_SPECIAL, iItem, -1, False)
						elif sType == "RevealBonus":
							ddsgfc("Item", GC.getBonusInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_BONUS_REVEAL, iTech, iItem, False)
						elif sType == "ObsoleteBonus":
							ddsgfc("Item", GC.getBonusInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_OBSOLETE_BONUS, iTech, iItem, False)
							ddsgfc("Obsolete", RED_X, WidgetTypes.WIDGET_HELP_OBSOLETE_BONUS, iItem, -1, False)
						elif sType == "RouteChange":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_MOVE_BONUS").getPath(), WidgetTypes.WIDGET_HELP_MOVE_BONUS, iTech, -1, False)
						elif sType == "UnlockPromotion":
							ddsgfc("Item", GC.getPromotionInfo(iItem).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROMOTION, iItem, -1, False)
						elif sType == "FreeUnit":
							ddsgfc("Item", GC.getUnitInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_FREE_UNIT, iItem, iTech, False)
						elif sType == "FeatureProduction":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_FEATURE_PRODUCTION").getPath(), WidgetTypes.WIDGET_HELP_FEATURE_PRODUCTION, iTech, -1, False)
						elif sType == "WorkerSpeed":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_WORKER_SPEED").getPath(), WidgetTypes.WIDGET_HELP_WORKER_RATE, iTech, -1, False)
						elif sType == "TradeRoute":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_TRADE_ROUTES").getPath(), WidgetTypes.WIDGET_HELP_TRADE_ROUTES, iTech, -1, False)
						elif sType == "Health":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_HEALTH").getPath(), WidgetTypes.WIDGET_HELP_HEALTH_RATE, iTech, -1, False)
						elif sType == "Happiness":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_HAPPINESS").getPath(), WidgetTypes.WIDGET_HELP_HAPPINESS_RATE, iTech, -1, False)
						elif sType == "Population":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_POPULATION").getPath(), WidgetTypes.WIDGET_HELP_HAPPINESS_RATE, iTech, -1, False)
						elif sType == "FreeTech":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_FREETECH").getPath(), WidgetTypes.WIDGET_HELP_FREE_TECH, iTech, -1, False)
						elif sType == "WaterSight":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_LOS").getPath(), WidgetTypes.WIDGET_HELP_LOS_BONUS, iTech, -1, False)
						elif sType == "MapCentering":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPCENTER").getPath(), WidgetTypes.WIDGET_HELP_MAP_CENTER, iTech, -1, False)
						elif sType == "MapVisible":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPREVEAL").getPath(), WidgetTypes.WIDGET_HELP_MAP_REVEAL, iTech, -1, False)
						elif sType == "MapTrading":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPTRADING").getPath(), WidgetTypes.WIDGET_HELP_MAP_TRADE, iTech, -1, False)
						elif sType == "TechTrading":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_TECHTRADING").getPath(), WidgetTypes.WIDGET_HELP_TECH_TRADE, iTech, -1, False)
						elif sType == "GoldTrading":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_GOLDTRADING").getPath(), WidgetTypes.WIDGET_HELP_GOLD_TRADE, iTech, -1, False)
						elif sType == "OpenBorders":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_OPENBORDERS").getPath(), WidgetTypes.WIDGET_HELP_OPEN_BORDERS, iTech, -1, False)
						elif sType == "DefensivePact":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_DEFENSIVEPACT").getPath(), WidgetTypes.WIDGET_HELP_DEFENSIVE_PACT, iTech, -1, False)
						elif sType == "PermanentAlliance":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_PERMALLIANCE").getPath(), WidgetTypes.WIDGET_HELP_PERMANENT_ALLIANCE, iTech, -1, False)
						elif sType == "VassalState":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_VASSAL").getPath(), WidgetTypes.WIDGET_HELP_VASSAL_STATE, iTech, -1, False)
						elif sType == "BridgeBuilding":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_BRIDGEBUILDING").getPath(), WidgetTypes.WIDGET_HELP_BUILD_BRIDGE, iTech, -1, False)
						elif sType == "EnablesIrrigation":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_IRRIGATION").getPath(), WidgetTypes.WIDGET_HELP_IRRIGATION, iTech, -1, False)
						elif sType == "IgnoreIrrigation":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_NOIRRIGATION").getPath(), WidgetTypes.WIDGET_HELP_IGNORE_IRRIGATION, iTech, -1, False)
						elif sType == "WaterWork":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERWORK").getPath(), WidgetTypes.WIDGET_HELP_WATER_WORK, iTech, -1, False)
						elif sType == "UnlockImprovement":
							ddsgfc("Item", GC.getBuildInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_IMPROVEMENT, iTech, iItem, False)
						elif sType == "DomainMoves":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERMOVES").getPath(), WidgetTypes.WIDGET_HELP_DOMAIN_EXTRA_MOVES, iTech, iItem, False)
						elif sType == "CommerceFlexible":
							if iItem == CommerceTypes.COMMERCE_CULTURE:
								temp = AFM.getInterfaceArtInfo("INTERFACE_TECH_CULTURE").getPath()
							elif iItem == CommerceTypes.COMMERCE_ESPIONAGE:
								temp = AFM.getInterfaceArtInfo("INTERFACE_TECH_ESPIONAGE").getPath()
							else: temp = AFM.getInterfaceArtInfo("INTERFACE_GENERAL_QUESTIONMARK").getPath()
							ddsgfc("Item", temp, WidgetTypes.WIDGET_HELP_ADJUST, iTech, iItem, False)
						elif sType == "TerrainTrade":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERTRADE").getPath(), WidgetTypes.WIDGET_HELP_TERRAIN_TRADE, iTech, iItem, False)
						elif sType == "RiverTrade":
							ddsgfc("Item", AFM.getInterfaceArtInfo("INTERFACE_TECH_RIVERTRADE").getPath(), WidgetTypes.WIDGET_HELP_TERRAIN_TRADE, iTech, GC.getNumTerrainInfos(), False)
						elif sType == "ImprovementYield":
							ddsgfc("Item", GC.getImprovementInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_YIELD_CHANGE, iTech, iItem, False)
						elif sType == "UnlockCivic":
							ddsgfc("Item", GC.getCivicInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_CIVIC_REVEAL, iTech, iItem, False)
						elif sType == "UnlockProject":
							ddsgfc("Item", GC.getProjectInfo(iItem).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT, iItem, 1, False)
						elif sType == "UnlockProcess":
							ddsgfc("Item", GC.getProcessInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_PROCESS_INFO, iTech, iItem, False)
						elif sType == "UnlockReligion":
							ddsgfc("Item", GC.getReligionInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_FOUND_RELIGION, iTech, iItem, False)
						elif sType == "UnlockCorporation":
							ddsgfc("Item", GC.getCorporationInfo(iItem).getButton(), WidgetTypes.WIDGET_HELP_FOUND_CORPORATION, iTech, iItem, False)
						iX += dx

				self.updateTechState(iTech)

		if not bFull:
			self.updateTechRecords(True)

	def updateTechRecords(self, bForce):
		screen = self.screen()

		# Progress Bar
		iNewCurrentResearch = self.CyPlayer.getCurrentResearch()
		if self.iCurrentResearch != iNewCurrentResearch:
			if iNewCurrentResearch > -1:
				screen.hide("TC_Header")
				iProgress = self.CyTeam.getResearchProgress(iNewCurrentResearch)
				iCost = self.CyTeam.getResearchCost(iNewCurrentResearch)
				iOverflow = self.CyPlayer.getOverflowResearch() * self.CyPlayer.calculateResearchModifier(iNewCurrentResearch) /100
				stackBar = "progressBar"
				screen.setBarPercentage(stackBar, InfoBarTypes.INFOBAR_STORED, iProgress * 1.0 / iCost)
				if iCost > iProgress + iOverflow:
					screen.setBarPercentage(stackBar, InfoBarTypes.INFOBAR_RATE, self.CyPlayer.calculateResearchRate(iNewCurrentResearch) * 1.0 / (iCost - iProgress - iOverflow))
				screen.show(stackBar)

				szTxt = "<font=3>" + GC.getTechInfo(iNewCurrentResearch).getDescription() + ' (' + str(self.CyPlayer.getResearchTurnsLeft(iNewCurrentResearch, True)) + ")"
				screen.setLabel("Researching", "", szTxt, 1<<2, self.xRes/2, 6, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, iNewCurrentResearch, 0)
				screen.setHitTest("Researching", HitTestTypes.HITTEST_NOHIT)
				screen.moveToFront("WID|TECH|CURRENT0")
				screen.show("WID|TECH|CURRENT0")
			else:
				screen.hide("WID|TECH|CURRENT0")
				screen.hide("Researching")
				screen.hide("progressBar")
				screen.show("TC_Header")
			self.iCurrentResearch = iNewCurrentResearch

		# Analyze change
		changed = []

		for iTech in xrange(self.iNumTechs):
			CvTechInfo = GC.getTechInfo(iTech)
			x0 = CvTechInfo.getGridX()
			if x0 <= 0:
				continue
			iX = x0 * self.xCellDist - self.minX
			if self.currentTechState[iTech] == CIV_HAS_TECH:
				if bForce:
					changed.append((iX, iTech))
			elif self.currentTechState[iTech] == CIV_NO_RESEARCH:
				if bForce:
					changed.append((iX, iTech))
			elif self.CyPlayer.isResearchingTech(iTech):
				queuePos = self.CyPlayer.getQueuePosition(iTech)
				if queuePos == 1:
					self.currentTechState[iTech] = CIV_IS_RESEARCHING
				elif queuePos == self.CyPlayer.getLengthResearchQueue():
					self.currentTechState[iTech] = CIV_IS_TARGET
				else:
					self.currentTechState[iTech] = CIV_IS_QUEUED
				changed.append((iX, iTech))
			elif bForce or self.currentTechState[iTech] != CIV_TECH_AVAILABLE:
				self.currentTechState[iTech] = CIV_TECH_AVAILABLE
				changed.append((iX, iTech))

		for _, iTech in changed:
			iTechStr = str(iTech)

			# Minimap cell color
			techMinimapCellId = TECH_CHOICE + "MM" + "|" + iTechStr
			minimapCellColor = self.getTechColorForState(self.currentTechState[iTech])
			screen.setPanelColor(techMinimapCellId, minimapCellColor[0], minimapCellColor[1], minimapCellColor[2])

		self.updates.extend(changed)

	def updateTechStates(self, techs):
		for iTech in techs:
			self.updateTechState(iTech)

	def updateTechState(self, iTech):
		screen = self.screen()
		iTechStr = str(iTech)

		# # Minimap cell color
		# techMinimapCellId = TECH_CHOICE + "MM" + "|" + iTechStr
		# minimapCellColor = self.getTechColorForState(techState)
		# screen.setPanelColor(techMinimapCellId, minimapCellColor[0], minimapCellColor[1], minimapCellColor[2])

		techState = self.currentTechState[iTech]

		CvTechInfo = GC.getTechInfo(iTech)
		szTechString = self.aFontList[3]
		iAdvisor = CvTechInfo.getAdvisorType()
		if iAdvisor > -1:
			szTechString += ADVISORS[iAdvisor]
		szTechString += CvTechInfo.getDescription()

		techCellId = TECH_CHOICE + iTechStr

		screen.setLabelAt(TECH_NAME + iTechStr, techCellId, szTechString, 1<<0, self.sIcon0 + 7 + CELL_BORDER_W, 7 + CELL_BORDER_H, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
		screen.setHitTest(TECH_NAME + iTechStr, HitTestTypes.HITTEST_NOHIT)

		# Colours
		iEra = CvTechInfo.getEra()
		techCellStyle = self.getTechStyleForState(techState, iEra)
		screen.setStyle(techCellId, techCellStyle)

		# Progress bar
		barId = techCellId + "BAR"
		if techState == CIV_HAS_TECH:
			screen.hide(barId)
		else:
			screen.show(barId)
			iProgress = self.CyTeam.getResearchProgress(iTech)
			iCost = self.CyTeam.getResearchCost(iTech)
			iOverflow = self.CyPlayer.getOverflowResearch() * self.CyPlayer.calculateResearchModifier(iTech) / 100
			screen.setBarPercentage(barId, InfoBarTypes.INFOBAR_STORED, iProgress * 1.0 / iCost)
			if iCost > iProgress + iOverflow:
				screen.setBarPercentage(barId, InfoBarTypes.INFOBAR_RATE, self.CyPlayer.calculateResearchRate(iTech) * 1.0 / (iCost - iProgress - iOverflow))

		# Queue labels
		if techState == CIV_IS_RESEARCHING or techState == CIV_IS_QUEUED or techState == CIV_IS_TARGET:
			screen.show(QUEUE_LABEL_PANEL + iTechStr)
			queuePosLabel = "<font=3b>" + FONT_COLOR_MAP[techState] + str(self.CyPlayer.getQueuePosition(iTech))
			screen.setLabelAt(QUEUE_LABEL + iTechStr, QUEUE_LABEL_PANEL + iTechStr, queuePosLabel, 1 << 2, QUEUE_LABEL_W / 2, 6, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
		else:
			screen.hide(QUEUE_LABEL_PANEL + iTechStr)

	def updateSelectedTech(self, screen, iTech):
		self.iSelectedTech = iTech
		screen.hide("ASPointsLabel")
		screen.hide("AddTechButton")
		if iTech > -1:
			iCost = self.CyPlayer.getAdvancedStartTechCost(iTech, True)
			if iCost > 0:
				iPoints = self.CyPlayer.getAdvancedStartPoints()
				screen.setLabel("ASPointsLabel", "", "<font=4>" + TRNSLTR.getText("TXT_KEY_WB_AS_SELECTED_TECH_COST", (iCost, iPoints)), 1<<0, 180, 4, 0, eFontTitle, WidgetTypes.WIDGET_GENERAL, 1, 2)
				if iPoints >= iCost:
					screen.show("AddTechButton")
			szTxt = "<font=4b>" + GC.getTechInfo(iTech).getDescription() + " (" + str(iCost) + unichr(8500) + ')'
			screen.setLabel("SelectedTechLabel", "", szTxt, 1<<0, self.xRes/2, 4, 0, eFontTitle, WidgetTypes.WIDGET_GENERAL, 1, 2)
			screen.hide("TC_Header")
		else:
			screen.hide("SelectedTechLabel")
			screen.show("TC_Header")

	def cacheBenefits(self):
		techBenefits = []
		iNumDomains = int(DomainTypes.NUM_DOMAIN_TYPES)
		iNumCommerce = int(CommerceTypes.NUM_COMMERCE_TYPES)
		iNumTerrains = GC.getNumTerrainInfos()

		iTech = 0
		while iTech < self.iNumTechs:
			techBenefits.append([])
			info = GC.getTechInfo(iTech)
			if info.getGridX() > 0:
				iType = info.getFirstFreeUnit()
				if iType > -1:
					techBenefits[iTech].append(["FreeUnit", iType])
				if info.getTradeRoutes():
					techBenefits[iTech].append(["TradeRoute", -1])
				if info.getHealth():
					techBenefits[iTech].append(["Health", -1])
				if info.getHappiness():
					techBenefits[iTech].append(["Happiness", -1])
				if info.getFirstFreeTechs():
					techBenefits[iTech].append(["FreeTech", -1])
				if info.isExtraWaterSeeFrom():
					techBenefits[iTech].append(["WaterSight", -1])
				if info.isMapCentering():
					techBenefits[iTech].append(["MapCentering", -1])
				if info.isMapVisible():
					techBenefits[iTech].append(["MapVisible", -1])
				if info.isMapTrading():
					techBenefits[iTech].append(["MapTrading", -1])
				if info.isTechTrading():
					techBenefits[iTech].append(["TechTrading", -1])
				if info.isGoldTrading():
					techBenefits[iTech].append(["GoldTrading", -1])
				if info.isOpenBordersTrading():
					techBenefits[iTech].append(["OpenBorders", -1])
				if info.isDefensivePactTrading():
					techBenefits[iTech].append(["DefensivePact", -1])
				if info.isPermanentAllianceTrading():
					techBenefits[iTech].append(["PermanentAlliance", -1])
				if info.isVassalStateTrading():
					techBenefits[iTech].append(["VassalState", -1])
				if info.isIrrigation():
					techBenefits[iTech].append(["EnablesIrrigation", -1])
				if info.isIgnoreIrrigation():
					techBenefits[iTech].append(["IgnoreIrrigation", -1])
				if info.isWaterWork():
					techBenefits[iTech].append(["WaterWork", -1])
				iType = 0
				while iType < iNumDomains:
					if info.getDomainExtraMoves(iType):
						techBenefits[iTech].append(["DomainMoves", iType])
					iType += 1
				iType = 0
				while iType < iNumCommerce:
					if info.isCommerceFlexible(iType):
						techBenefits[iTech].append(["CommerceFlexible", iType])
					iType += 1
				iType = 0
				while iType < iNumTerrains:
					if info.isTerrainTrade(iType):
						techBenefits[iTech].append(["TerrainTrade", iType])
					iType += 1
				if info.isRiverTrade():
					techBenefits[iTech].append(["RiverTrade", -1])
				if info.getFeatureProductionModifier():
					techBenefits[iTech].append(["FeatureProduction", -1])
				if info.getWorkerSpeedModifier():
					techBenefits[iTech].append(["WorkerSpeed", -1])
			iTech += 1

		iType = 0
		while iType < GC.getNumRouteInfos():
			info = GC.getRouteInfo(iType)
			iTech = 0
			while iTech < self.iNumTechs:
				if info.getTechMovementChange(iTech):
					techBenefits[iTech].append(["RouteChange", iType])
				iTech += 1
			iType += 1

		iNumYields = int(YieldTypes.NUM_YIELD_TYPES)
		iType = 0
		while iType < GC.getNumImprovementInfos():
			info = GC.getImprovementInfo(iType)
			iTech = 0
			while iTech < self.iNumTechs:
				i = 0
				while i < iNumYields:
					if info.getTechYieldChanges(iTech, i):
						techBenefits[iTech].append(["ImprovementYield", iType])
					i += 1
				iTech += 1
			iType += 1

		iType = 0
		while iType < GC.getNumUnitInfos():
			iTech = GC.getUnitInfo(iType).getPrereqAndTech()
			if iTech > -1:
				techBenefits[iTech].append(["UnlockUnit", iType])
			iType += 1

		iType = 0
		while iType < GC.getNumBuildingInfos():
			info = GC.getBuildingInfo(iType)
			iTech = info.getPrereqAndTech()
			if iTech > -1:
				techBenefits[iTech].append(["UnlockBuilding", iType])
			iTech = info.getObsoleteTech()
			if iTech > -1:
				techBenefits[iTech].append(["ObsoleteBuilding", iType])
			iType += 1

		iType = 0
		while iType < GC.getNumSpecialBuildingInfos():
			info = GC.getSpecialBuildingInfo(iType)
			iTech = info.getTechPrereq()
			if iTech > -1:
				techBenefits[iTech].append(["UnlockSpecialBuilding", iType])
			iTech = info.getObsoleteTech()
			if iTech > -1:
				techBenefits[iTech].append(["ObsoleteSpecialBuilding", iType])
			iType += 1

		iType = 0
		while iType < GC.getNumBonusInfos():
			info = GC.getBonusInfo(iType)
			iTech = info.getTechReveal()
			if iTech > -1:
				techBenefits[iTech].append(["RevealBonus", iType])
			iTech = info.getTechObsolete()
			if iTech > -1:
				techBenefits[iTech].append(["ObsoleteBonus", iType])
			iType += 1

		iType = 0
		while iType < GC.getNumPromotionInfos():
			iTech = GC.getPromotionInfo(iType).getTechPrereq()
			if iTech > -1:
				techBenefits[iTech].append(["UnlockPromotion", iType])
			iType += 1

		iFeatures = GC.getNumFeatureInfos()
		iType = 0
		while iType < GC.getNumBuildInfos():
			info = GC.getBuildInfo(iType)
			iTech = info.getTechPrereq()
			if iTech > -1:
				techBenefits[iTech].append(["UnlockImprovement", iType])
			else:
				for i in xrange(iFeatures):
					iTech = info.getFeatureTech(i)
					if iTech > -1:
						techBenefits[iTech].append(["UnlockImprovement", iType])
			iType += 1

		iType = 0
		while iType < GC.getNumCivicInfos():
			iTech = GC.getCivicInfo(iType).getTechPrereq()
			if iTech > -1:
				techBenefits[iTech].append(["UnlockCivic", iType])
			iType += 1

		iType = 0
		while iType < GC.getNumProjectInfos():
			iTech = GC.getProjectInfo(iType).getTechPrereq()
			if iTech > -1:
				techBenefits[iTech].append(["UnlockProject", iType])
			iType += 1

		iType = 0
		while iType < GC.getNumProcessInfos():
			iTech = GC.getProcessInfo(iType).getTechPrereq()
			if iTech > -1:
				techBenefits[iTech].append(["UnlockProcess", iType])
			iType += 1

		iType = 0
		while iType < GC.getNumReligionInfos():
			iTech = GC.getReligionInfo(iType).getTechPrereq()
			if iTech > -1:
				techBenefits[iTech].append(["UnlockReligion", iType])
			iType += 1

		iType = 0
		while iType < GC.getNumCorporationInfos():
			iTech = GC.getCorporationInfo(iType).getTechPrereq()
			if iTech > -1:
				techBenefits[iTech].append(["UnlockCorporation", iType])
			iType += 1
		self.techBenefits = techBenefits


	def update(self, fDelta):
		# Only on the 2nd call to update can we update the scroll position correctly
		if self.delayedScroll:
			self.delayedScroll = self.delayedScroll - 1
			if not self.delayedScroll:
				techToScrollTo = self.getLastResearchingIdx()
				if techToScrollTo < 0:
					techToScrollTo = self.getLastResearchedIdx()
				if techToScrollTo != -1:
					self.scrollToTech(techToScrollTo)
				else:
					self.scrollTo(self.scrollOffs)


		if self.demoMode:
			self.scrollTo(self.demoOffs)
			self.demoOffs = self.demoOffs + 50

		if self.tooltip.bLockedTT:
			self.tooltip.handle(self.screen())

		mousePos = Win32.getCursorPos()
		if self.scrolling:
			self.scrollTo(self.minimapToTreeX(mousePos.x - self.minimapLensWidth / 2))
			self.scrolling = Win32.isLMB()
		elif mousePos.y > self.yRes - SCREEN_PANEL_BOTTOM_BAR_H and Win32.isLMB():
			self.scrolling = True

		# if self.changed:
		# 	self.changed = sorted(self.changed, key=lambda el: abs(el[0] - self.scrollOffs - self.xRes / 2))
		# 	remaining_updates = self.changed[:TECH_PAGING_RATE]
		# 	self.changed = self.changed[TECH_PAGING_RATE:]
		# 	self.updateTechStates((f[1] for f in remaining_updates))
		if self.updates:
			# Sort them by distance to the current scroll offset so we can make sure to update what the player is looking at first
			self.updates = sorted(self.updates, key=lambda el: abs(el[0] - self.scrollOffs - self.xRes / 2))

			remaining_updates = self.updates[:TECH_PAGING_RATE]

			# remaining_updates = []
			# for el in self.updates:
			# 	dist = abs(el[0] - self.scrollOffs - self.xRes / 2)
			# 	if len(remaining_updates) > TECH_PAGING_RATE and dist > self.xRes:
			# 		break
			# 	remaining_updates.append(el)
			self.updates = self.updates[len(remaining_updates):]
			self.refresh((f[1] for f in remaining_updates), True)

		scrollDiff = Win32.getMouseWheelDiff(self.mwlHandle)
		if scrollDiff != 0:
			self.scrollTo(int(self.scrollOffs - scrollDiff * self.xCellDist / 2))

	def handleInput(self, inputClass):
		if not self.created:
			print "CvTechChooser.handleInput - aborted due to screen not being created"
			return 0

		print "CvTechChooser.handleInput"

		screen = self.screen()
		bAlt, bCtrl, bShift = self.InputData.getModifierKeys()
		iCode	= inputClass.eNotifyCode
		iData	= inputClass.iData
		iData2	= inputClass.iData2
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName
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
			CASE = [""]

		iType = ID

		if iCode == NotifyCode.NOTIFY_CHARACTER: # Character
			if iData in (45, 49, 56): # Ctrl, Shift, Alt
				if self.bUnitTT:
					self.tooltip.handle(screen, CyGameTextMgr().getUnitHelp(self.iUnitTT, False, True, True, None))
					self.bUnitTT = None
			return 1
		elif iCode == 17: # Key Up
			if iData == InputTypes.KB_A:
				self.scrollTo(self.scrollOffs - self.xCellDist)
			elif iData == InputTypes.KB_D:
				self.scrollTo(self.scrollOffs + self.xCellDist)
			if iData == InputTypes.KB_Q:
				self.demoMode = not self.demoMode
				self.demoOffs = 0
			elif iData in (InputTypes.KB_ESCAPE, InputTypes.KB_F6):
				# Stop F6 and ESC handlers from triggering on load
				if self.skipNextExitKey:
					self.skipNextExitKey = False
				else:
					screen.hideScreen()
				return 1
			if self.bUnitTT is None:
				self.bUnitTT = True
			return 1

		# Remove potential Help Text
		self.tooltip.reset(screen)
		self.iUnitTT = None
		self.bUnitTT = False

		if iCode == NotifyCode.NOTIFY_CURSOR_MOVE_ON: # Mouse Enter
			if BASE == "WID":
				if TYPE == "TECH":
					if CASE[0] == "CURRENT":
						szTxt = "Researching: " + CyGameTextMgr().getTechHelp(self.CyPlayer.getCurrentResearch(), False, True, True, True, -1)
					elif CASE[0] == "REQ":
						szTxt = TRNSLTR.getText("TXT_KEY_MISC_TECH_REQUIRES_KNOWLEDGE_OF", (GC.getTechInfo(ID).getTextKey(),))
					else:
						szTxt = CyGameTextMgr().getTechHelp(ID, False, True, True, True, -1)
					self.tooltip.handle(screen, szTxt)
				elif TYPE == "UNIT":
					self.tooltip.handle(screen, CyGameTextMgr().getUnitHelp(ID, False, True, True, None))
					self.iUnitTT = ID
					self.bUnitTT = True
				elif TYPE == "BUILDING":
					if CASE[0] == "OBS":
						CvBuildingInfo = GC.getBuildingInfo(ID)
						szTxt = TRNSLTR.getText("TXT_KEY_TECHHELP_OBSOLETES", (CvBuildingInfo.getType(), CvBuildingInfo.getTextKey()))
					else: szTxt = CyGameTextMgr().getBuildingHelp(ID, False, None, False, False, True)
					self.tooltip.handle(screen, szTxt)
		elif iCode == NotifyCode.NOTIFY_CLICKED: # click
			if BASE == "WID":
				if szFlag == "MOUSE_RBUTTONUP":
					if TYPE == "UNIT":
						UP.pediaJumpToUnit([ID])
					elif TYPE == "BUILDING":
						UP.pediaJumpToBuilding([ID])
					elif TYPE == "PROJECT":
						UP.pediaJumpToProject([ID])
					elif TYPE == "PROMO":
						UP.pediaJumpToPromotion([ID])
					elif TYPE == "TECH":
						if CASE[0] == "CURRENT":
							UP.pediaJumpToTech([self.CyPlayer.getCurrentResearch()])
						else: UP.pediaJumpToTech([iType])
				elif szFlag == "MOUSE_LBUTTONUP":
					if TYPE == "TECH":
						if self.CyPlayer.getAdvancedStartPoints() > -1:
							if CASE[0] == "CHOICE":
								self.updateSelectedTech(screen, ID)
						elif GC.getGame().getActivePlayer() == self.iPlayer:
							if CASE[0] == "CURRENT":
								CyMessageControl().sendResearch(-1, bShift)
								self.updateTechRecords(False)
							elif CASE[0] == "CHOICE" and (self.currentTechState[iType] == CIV_TECH_AVAILABLE or not bShift and (self.currentTechState[iType] == CIV_IS_RESEARCHING or self.currentTechState[iType] == CIV_IS_QUEUED)):
								CyMessageControl().sendResearch(iType, bShift)
								self.updateTechRecords(False)
					elif TYPE == "ERAIM" or TYPE == "ERATEXT":
						self.scrollTo(self.minEraXPos[ID] - self.minX)
			elif NAME == "AddTechButton":
				CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_TECH, self.iPlayer, -1, -1, self.iSelectedTech, True)	#Action, Player, X, Y, Data, bAdd
				self.updateSelectedTech(screen, -1)
		elif NAME == HSLIDER_ID:
			if iCode == 20:
				self.scrollOffs = iData
			self.scroll()
		elif iCode == 11: # List Select
			if NAME == "TC_DebugDD":
				self.initForPlayer(screen.getPullDownData(NAME, screen.getSelectedPullDownID(NAME)))

	def onClose(self):
		print "CvTechChooser.onClose"

		if GC.getPlayer(GC.getGame().getActivePlayer()).getAdvancedStartPoints() > -1:
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, True)

		# Only delete if actually created
		if self.created:
			print "CvTechChooser.onClose - deleting"
			self.created = False
			Win32.unregisterMouseWheelListener(self.mwlHandle)
			del (
				self.screenId, self.InputData, self.iUnitTT, self.bUnitTT, self.xRes, self.yRes,
				self.aFontList, self.wCell, self.hCell, self.sIcon0, self.sIcon1, self.iSelectedTech,
				self.iPlayer, self.CyPlayer, self.CyTeam, self.iCurrentResearch, self.currentTechState, self.iCurrentEra, self.updates
			)
		print "CvTechChooser.onClose - DONE"

	def getTechPos(self, idx):
		info = GC.getTechInfo(idx)
		return info.getGridX() * self.xCellDist - self.minX

	def getLastResearchingIdx(self):
		lastTechInQueue = (-1, -1)
		for idx in xrange(self.iNumTechs):
			queuePos = self.CyPlayer.getQueuePosition(idx)
			if queuePos > lastTechInQueue[1]:
				lastTechInQueue = (idx, queuePos)
		return lastTechInQueue[0]

	def getLastResearchedIdx(self):
		lastResearched = (-1, -1)
		for idx in xrange(self.iNumTechs):
			if self.CyTeam.isHasTech(idx):
				info = GC.getTechInfo(idx)
				iX = info.getGridX()
				if iX > lastResearched[1]:
					lastResearched = (idx, iX)
		return lastResearched[0]

	def minimapToTreeX(self, minimapX):
		return int((minimapX - SLIDER_BORDER) / self.minimapScaleX)

	def treeToMinimapX(self, treeX):
		return int(treeX * self.minimapScaleX) + SLIDER_BORDER

	def scroll(self):
		screen = self.screen()

		# Move the main tech panel
		screen.moveItem(SCREEN_PANEL, -self.scrollOffs, 29, 0)

		# Move the minimap lens
		screen.moveItem(MINIMAP_LENS_ID, SLIDER_BORDER + (self.minimapScaleX * self.scrollOffs) - MINIMAP_LENS_BORDER_H, self.yRes - SCREEN_PANEL_BOTTOM_BAR_H, 0)

		# Parallax Scroll the backdrops
		for i in xrange(self.iNumEras - 1):
			bgName = "ERA_BG_" + str(i)
			fgName = "ERA_FG_" + str(i)

			panelStartX, panelEndX = self.backdropPanelPos[i]

			eraStart = panelStartX - self.xRes
			eraEnd = panelEndX

			eraWidth = eraEnd - eraStart
			eraOffs = self.scrollOffs - eraStart
			eraFrac = max(0, min(1, eraOffs / float(eraWidth)))
			offs = self.scrollOffs - (panelStartX)
			bgOffs = offs - int(BACKGROUND_PARA_AMOUNT * 2 * eraFrac)
			fgOffs = offs - int(FOREGROUND_PARA_AMOUNT * 2 * eraFrac)

			# Debug text
			# screen.setText("FRRAC", "", str((self.scrollOffs, eraStart, eraEnd, eraWidth, eraOffs, eraFrac, offs, bgOffs, fgOffs)), 0, 70, 5, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.moveItem(bgName, bgOffs, 0, 0)
			bgPanelHgt = self.yRes - SCREEN_PANEL_BOTTOM_BAR_H - SCREEN_PANEL_TOP_BAR_H
			screen.moveItem(fgName, fgOffs, 0, 0)

	def scrollToTech(self, idx):
		self.scrollTo(self.getTechPos(idx) - self.xRes / 2 + self.xCellDist / 2)

	def scrollTo(self, offs):
		maxScroll = self.maxX - self.xRes - self.minX
		self.scrollOffs = offs
		if self.scrollOffs < 0:
			self.scrollOffs = 0
		if self.scrollOffs > maxScroll:
			self.scrollOffs = maxScroll
		self.scroll()

	def getTechColorForState(self, state):
		return {
			CIV_HAS_TECH: [128, 128, 128],
			CIV_IS_RESEARCHING: [0, 255, 0],
			CIV_IS_QUEUED: [192, 192, 0],
			CIV_IS_TARGET: [255, 128, 0],
			CIV_TECH_AVAILABLE: [32, 32, 64],
			CIV_NO_RESEARCH: [128, 0, 0],
		}.get(state, [128, 0, 0])

	def getTechStyleForState(self, state, era):
		if state == CIV_HAS_TECH:
			return {
				0: "Button_TechHas_0_Style",
				1: "Button_TechHas_1_Style",
				2: "Button_TechHas_2_Style",
				3: "Button_TechHas_3_Style",
				4: "Button_TechHas_4_Style",
				5: "Button_TechHas_5_Style",
				6: "Button_TechHas_6_Style",
				7: "Button_TechHas_7_Style",
				8: "Button_TechHas_8_Style",
				9: "Button_TechHas_9_Style",
				10: "Button_TechHas_10_Style",
				11: "Button_TechHas_11_Style",
				12: "Button_TechHas_12_Style",
				13: "Button_TechHas_13_Style",
			}.get(era, "Button_TechHas_Style")
		elif state == CIV_TECH_AVAILABLE:
			if era > self.iCurrentEra:
				return "Button_TechNeo_Style"
			elif era < self.iCurrentEra:
				return "Button_TechArchaic_Style"
			else:
				return "Button_TechCoeval_Style"
		else:
			return {
				CIV_IS_RESEARCHING: "Button_TechResearching_Style",
				CIV_IS_QUEUED: "Button_TechQueue_Style",
				CIV_IS_TARGET: "Button_TechTarget_Style"
			}.get(state, "Button_TechNo_Style")

	def getBackgroundStyleForEra(self, era):
		return {
			0: "Button_TechBackground_0_Style",
			1: "Button_TechBackground_1_Style",
			2: "Button_TechBackground_2_Style",
			3: "Button_TechBackground_3_Style",
			4: "Button_TechBackground_4_Style",
			5: "Button_TechBackground_5_Style",
			6: "Button_TechBackground_6_Style",
			7: "Button_TechBackground_7_Style",
			8: "Button_TechBackground_8_Style",
			9: "Button_TechBackground_9_Style",
			10: "Button_TechBackground_10_Style",
			11: "Button_TechBackground_11_Style",
			12: "Button_TechBackground_12_Style",
			13: "Button_TechBackground_13_Style",
		}.get(era, "Button_TechBackground_0_Style")

	def getForegroundStyleForEra(self, era):
		return {
			0: "Button_TechForeground_0_Style",
			1: "Button_TechForeground_1_Style",
			2: "Button_TechForeground_2_Style",
			3: "Button_TechForeground_3_Style",
			4: "Button_TechForeground_4_Style",
			5: "Button_TechForeground_5_Style",
			6: "Button_TechForeground_6_Style",
			7: "Button_TechForeground_7_Style",
			8: "Button_TechForeground_8_Style",
			9: "Button_TechForeground_9_Style",
			10: "Button_TechForeground_10_Style",
			11: "Button_TechForeground_11_Style",
			12: "Button_TechForeground_12_Style",
			13: "Button_TechForeground_13_Style",
		}.get(era, "Button_TechForeground_0_Style")

	def getIntroStyleForEra(self, era):
		return {
			0: "Panel_TechIntro_0_Style",
			1: "Panel_TechIntro_1_Style",
			2: "Panel_TechIntro_2_Style",
			3: "Panel_TechIntro_3_Style",
			4: "Panel_TechIntro_4_Style",
			5: "Panel_TechIntro_5_Style",
			6: "Panel_TechIntro_6_Style",
			7: "Panel_TechIntro_7_Style",
			8: "Panel_TechIntro_8_Style",
			9: "Panel_TechIntro_9_Style",
			10: "Panel_TechIntro_10_Style",
			11: "Panel_TechIntro_11_Style",
			12: "Panel_TechIntro_12_Style",
			13: "Panel_TechIntro_13_Style",
		}.get(era, "Panel_TechIntro_0_Style")