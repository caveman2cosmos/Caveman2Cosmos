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
CIV_IS_RESEARCHING = 2

ICON = "ICON"
TECH_CHOICE = "WID|TECH|CHOICE"
TECH_REQ = "WID|TECH|REQ"
TECH_NAME = "TechName"
SCREEN_PANEL = "TechList"

# Top panel height
SCREEN_PANEL_BAR_H = 42
# Bottom panel height
SCREEN_PANEL_BOTTOM_BAR_H = 100
# Left/right border for the slider
SLIDER_BORDER = 50
# How many techs to page in per update tick, more is faster but more introduces more stutter
# Could use a timer instead of a fixed count to allocate time slice for paging for more consistent behaviour
TECH_PAGING_RATE = 5
# Gap between techs
CELL_GAP = 64
# Size of dependency arrows
ARROW_SIZE = 8

class CvTechChooser:

	def __init__(self):
		self.scrollOffs = 0
		self.iNumEras = GC.getNumEraInfos()
		self.iNumTechs = GC.getNumTechInfos()
		self.created = False
		self.firstLoad = True
		self.cacheBenefits()

	def screen(self):
		return CyGInterfaceScreen("TechChooser", self.screenId)

	def cachePlayer(self, iPlayer):
		self.iPlayer = iPlayer
		self.CyPlayer = GC.getPlayer(iPlayer)
		self.CyTeam = GC.getTeam(self.CyPlayer.getTeam())
		self.iCurrentEra = self.CyPlayer.getCurrentEra()

		currentTechState = self.currentTechState

		for iTech in xrange(self.iNumTechs):
			if self.CyTeam.isHasTech(iTech):
				currentTechState[iTech] = CIV_HAS_TECH
			elif self.CyPlayer.canEverResearch(iTech):
				if self.CyPlayer.isResearchingTech(iTech):
					currentTechState[iTech] = CIV_IS_RESEARCHING
				else: currentTechState[iTech] = CIV_TECH_AVAILABLE
			else:
				currentTechState[iTech] = CIV_NO_RESEARCH

	def interfaceScreen(self, screenId):
		if GC.getGame().isPitbossHost():
			return

		# Make sure we don't initialize twice
		if self.created:
			return

		self.mwlHandle = Win32.registerMouseWheelListener()

		self.created = True
		self.screenId = screenId

		import InputData
		self.InputData = InputData.instance

		# Tool Tip
		self.szTxtTT = ""
		self.iOffsetTT = []
		self.bLockedTT = False
		self.bUnitTT = False
		self.updates = []

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
		self.minEraX = [maxint] * GC.getNumEraInfos()
		self.minX = maxint
		self.maxX = 0
		for iTech in xrange(self.iNumTechs):
			info = GC.getTechInfo(iTech)
			iX = info.getGridX() * self.xCellDist
			iX1 = (info.getGridX() + 1) * self.xCellDist
			if iX > 0:
				iEra = info.getEra()
				if iX < self.minEraX[iEra]:
					self.minEraX[iEra] = iX
				if iX1 > self.maxX:
					self.maxX = iX1
				if iX < self.minX:
					self.minX = iX

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		self.currentTechState = [-1] * self.iNumTechs
		iPlayer = GC.getGame().getActivePlayer()
		self.cachePlayer(iPlayer)

		# Base Screen
		screen = self.screen()
		screen.addDDSGFC("ScreenBackground", AFM.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, self.xRes, self.yRes, eWidGen, 1, 2)

		screen.addPanel("TC_BarTop", "", "", True, False, 0, 0, self.xRes, SCREEN_PANEL_BAR_H, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("TC_BarBot", "", "", True, False, 0, self.yRes - SCREEN_PANEL_BOTTOM_BAR_H, self.xRes, SCREEN_PANEL_BOTTOM_BAR_H, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.setLabel("TC_Header", "", "<font=4b>" + TRNSLTR.getText("TXT_KEY_TECH_CHOOSER_TITLE", ()), 1<<2, self.xRes/2, 4, 0, eFontTitle, eWidGen, 1, 2)

		screen.setText("TC_Exit", "", "<font=4b>" + TRNSLTR.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()), 1<<1, self.xRes - 8, 2, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		stackBar = "progressBar"
		screen.addStackedBarGFC(stackBar, 256, 2, self.xRes - 512, 32, InfoBarTypes.NUM_INFOBAR_TYPES, eWidGen, 1, 2)
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_STORED, GC.getInfoTypeForString("COLOR_RESEARCH_STORED"))
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_RATE, GC.getInfoTypeForString("COLOR_RESEARCH_RATE"))
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_EMPTY, GC.getInfoTypeForString("COLOR_EMPTY"))
		screen.hide(stackBar)
		screen.setImageButton("WID|TECH|CURRENT0", "", 256, 3, self.xRes - 512, 30, eWidGen, 1, 2)
		screen.hide("WID|TECH|CURRENT0")

		self.iResearch0 = -1

		# Debug
		import DebugUtils
		if DebugUtils.bDebugMode:
			DDB = "TC_DebugDD"
			screen.addDropDownBoxGFC(DDB, 4, 2, 200, eWidGen, 1, 2, eFontTitle)
			for iPlayerX in range(GC.getMAX_PLAYERS()):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					screen.addPullDownString(DDB, CyPlayerX.getName(), iPlayerX, iPlayerX, iPlayer == iPlayerX)

		self.iSelectedTech = -1
		if self.CyPlayer.getAdvancedStartPoints() > -1:
			screen.setButtonGFC("AddTechButton", TRNSLTR.getText("TXT_KEY_WB_AS_ADD_TECH", ()), "", self.xRes/2 - 158, 4, 150, 30, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_STANDARD)
			screen.hide("AddTechButton")

		screen.setRenderInterfaceOnly(True)
		screen.showWindowBackground(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		# Main scrolling panel
		screen.addPanel(SCREEN_PANEL, "", "", False, False, 0, SCREEN_PANEL_BAR_H, self.maxX + self.xCellDist, self.yRes - SCREEN_PANEL_BAR_H - SCREEN_PANEL_BOTTOM_BAR_H, PanelStyles.PANEL_STYLE_EMPTY)

		# Create the tech button backgrounds
		self.refresh(xrange(self.iNumTechs), False)

		# A panel to put the horizontal slider in so we can position it correctly
		screen.addPanel("TC_BarBotSlider", "", "", False, False, SLIDER_BORDER, self.yRes - SCREEN_PANEL_BOTTOM_BAR_H - 12, self.xRes, 40, PanelStyles.PANEL_STYLE_EMPTY)

		# Era buttons that can jump directly to an era
		for i in xrange(GC.getNumEraInfos()):
			posX = SLIDER_BORDER + self.minEraX[i] * (self.xRes - SLIDER_BORDER * 2) / self.maxX
			posY = self.yRes - SCREEN_PANEL_BOTTOM_BAR_H + 24
			eraInfo = GC.getEraInfo(i)
			img = eraInfo.getButton()
			if img:
				screen.setText("WID|ERAIM|" + str(i), "", "<img=%s>" % (img), 0, posX, posY + 20, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, iEra, 0)
			yOffs = 0 + 48 * (i % 2)
			screen.setText("WID|ERATEXT|" + str(i), "", "<font=1>%s" % (eraInfo.getDescription()), 0, posX, posY + yOffs, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, iEra, 0)

		# On first load we will scroll to the currently researched tech automatically
		techToScrollTo = -1
		if self.firstLoad:
			techToScrollTo = self.getLastResearchingIdx()
			if techToScrollTo < 0:
				techToScrollTo = self.getLastResearchedIdx()

		# Finally scroll to our current offset (this is maintained when the screen is created and destroyed)
		if techToScrollTo != -1:
			self.scrollToTech(techToScrollTo)
		else:
			self.scrollTo(self.scrollOffs)

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

	def scroll(self):
		self.screen().moveItem(SCREEN_PANEL, -self.scrollOffs, 29, 0)

	def scrollToTech(self, idx):
		self.scrollTo(self.getTechPos(idx) - self.xRes / 2 + self.xCellDist / 2)

	def scrollTo(self, offs):
		maxScroll = self.maxX - self.xRes - self.minX
		self.scrollOffs = offs
		if self.scrollOffs < 0:
			self.scrollOffs = 0
		if self.scrollOffs > maxScroll:
			self.scrollOffs = maxScroll
		self.screen().attachSlider("TC_BarBotSlider", "HSlider", 0, 0, self.xRes - SLIDER_BORDER * 2, 20, self.scrollOffs, 0, maxScroll, WidgetTypes.WIDGET_GENERAL, 0, 0, False)
		self.scroll()

	def refresh(self, techs, bFull):
		screen = self.screen()

		eWidGen = WidgetTypes.WIDGET_GENERAL

		dy = self.yRes - SCREEN_PANEL_BAR_H - SCREEN_PANEL_BOTTOM_BAR_H

		yEmptySpace = (dy - 10 * self.hCell) / 10
		if yEmptySpace < 0:
			yEmptySpace = 0
		yCellDist = yEmptySpace + self.hCell
		yEmptySpace /= 2
		yBoxSpacing = yCellDist/2

		# Place techs
		NUM_AND_TECH_PREREQS = GC.getNUM_AND_TECH_PREREQS()
		NUM_OR_TECH_PREREQS = GC.getNUM_OR_TECH_PREREQS()
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

		for iTech in techs:
			CvTechInfo = GC.getTechInfo(iTech)

			x0 = CvTechInfo.getGridX()
			if x0 < 1:
				continue

			iEra = CvTechInfo.getEra()

			szTech = str(iTech)
			szTechRecord = TECH_CHOICE + szTech
			y0 = CvTechInfo.getGridY()
			xRel = x0 * self.xCellDist - self.minX
			iX = xRel

			iY = yEmptySpace + ((y0 - 1) * yCellDist) / 2

			if not bFull:
				screen.setImageButtonAt(szTechRecord, SCREEN_PANEL, "", iX, iY, self.wCell, self.hCell, eWidGen, 1, 2)
				screen.addDDSGFCAt(ICON + szTech, szTechRecord, CvTechInfo.getButton(), 3, 5, self.sIcon0, self.sIcon0, eWidGen, 1, 2, False)
				screen.setHitTest(ICON + szTech, HitTestTypes.HITTEST_NOHIT)
				self.updates.append((xRel, iTech))
			else:
				# Requires
				iX = self.wCell
				for i in xrange(NUM_AND_TECH_PREREQS):
					iTechX = CvTechInfo.getPrereqAndTechs(i)
					if iTechX == -1: break
					iX -= dx
					screen.setImageButtonAt(TECH_REQ + str(iTechX) + "|" + szTech, szTechRecord, GC.getTechInfo(iTechX).getButton(), iX, 2, self.sIcon1, self.sIcon1, eWidGen, 1, 2)

				for i in xrange(NUM_OR_TECH_PREREQS):
					iTechX = CvTechInfo.getPrereqOrTechs(i)
					if iTechX == -1: break
					techInfoX = GC.getTechInfo(iTechX)

					iEra = techInfoX.getEra()

					x1 = techInfoX.getGridX()
					y1 = techInfoX.getGridY()
					iX = x1 * self.xCellDist + self.wCell - 2 - self.minX
					iY = yEmptySpace + ((y1 - 1) * yCellDist) / 2 - 4

					xDiff = x0 - x1
					yDiff = y0 - y1
					xOff = xDiff * 62 + (xDiff - 1) * self.wCell

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

				# Unlocks
				iX = self.sIcon0 + 6
				iY = self.sIcon0 + 4 - self.sIcon1

				benefits = techBenefits[iTech]
				iLength = len(benefits)
				if iLength > iMaxElements:
					iLength = iMaxElements

				for i in xrange(iLength):
					sType, iItem = benefits[i]

					# Helpers for drawing the unlocks
					def imageButton(key, button):
						screen.setImageButtonAt(key + str(iTech * 1000 + i), szTechRecord, button, iX, iY, self.sIcon1, self.sIcon1, eWidGen, 1, 2)

					def ddsgfc(key, button, widgetType, tech, item, flag):
						screen.addDDSGFCAt(key + str(iTech * 1000 + i), szTechRecord, button, iX, iY, self.sIcon1, self.sIcon1, widgetType, tech, item, flag)

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

		if not bFull:
			self.updateTechRecords(True)

	def updateTechRecords(self, bForce):
		screen = self.screen()
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		# Progress Bar
		iTech = self.CyPlayer.getCurrentResearch()
		if self.iResearch0 != iTech:
			if iTech > -1:
				screen.hide("TC_Header")
				iProgress = self.CyTeam.getResearchProgress(iTech)
				iCost = self.CyTeam.getResearchCost(iTech)
				iOverflow = self.CyPlayer.getOverflowResearch() * self.CyPlayer.calculateResearchModifier(iTech) /100
				stackBar = "progressBar"
				screen.setBarPercentage(stackBar, InfoBarTypes.INFOBAR_STORED, iProgress * 1.0 / iCost)
				if iCost > iProgress + iOverflow:
					screen.setBarPercentage(stackBar, InfoBarTypes.INFOBAR_RATE, self.CyPlayer.calculateResearchRate(iTech) * 1.0 / (iCost - iProgress - iOverflow))
				screen.show(stackBar)

				szTxt = "<font=3>" + GC.getTechInfo(iTech).getDescription() + ' (' + str(self.CyPlayer.getResearchTurnsLeft(iTech, True)) + ")"
				screen.setLabel("Researching", "", szTxt, 1<<2, self.xRes/2, 6, 0, eFontTitle, eWidGen, iTech, 0)
				screen.setHitTest("Researching", HitTestTypes.HITTEST_NOHIT)
				screen.moveToFront("WID|TECH|CURRENT0")
				screen.show("WID|TECH|CURRENT0")
			else:
				screen.hide("WID|TECH|CURRENT0")
				screen.hide("Researching")
				screen.hide("progressBar")
				screen.show("TC_Header")
			self.iResearch0 = iTech

		currentTechState = self.currentTechState

		# Analyze change
		lChanged = []

		for iTech in xrange(self.iNumTechs):
			CvTechInfo = GC.getTechInfo(iTech)
			iX = CvTechInfo.getGridX()
			if iX > 0:
				iEra = CvTechInfo.getEra()
				if currentTechState[iTech] == CIV_HAS_TECH:
					if bForce:
						lChanged.append((iTech, CvTechInfo, iEra))

				elif currentTechState[iTech] == CIV_NO_RESEARCH:
					if bForce:
						lChanged.append((iTech, CvTechInfo, iEra))

				elif self.CyPlayer.isResearchingTech(iTech):
					currentTechState[iTech] = CIV_IS_RESEARCHING
					lChanged.append((iTech, CvTechInfo, iEra))

				elif bForce or currentTechState[iTech] != CIV_TECH_AVAILABLE:
					currentTechState[iTech] = CIV_TECH_AVAILABLE
					lChanged.append((iTech, CvTechInfo, iEra))

		# Make change
		if lChanged:
			advisors = [unichr(8855), unichr(8857), unichr(8500), unichr(8501), unichr(8502), unichr(8483)]
			iX = self.sIcon0 + 6
			szFont = self.aFontList[3]
			while lChanged:
				iTech, CvTechInfo, iEra = lChanged.pop()
				szTechRecord = TECH_CHOICE + str(iTech)

				szState = currentTechState[iTech]

				szTechString = szFont
				iAdvisor = CvTechInfo.getAdvisorType()
				if iAdvisor > -1:
					szTechString += advisors[iAdvisor]
				if szState == CIV_IS_RESEARCHING:
					szTechString += str(self.CyPlayer.getQueuePosition(iTech)) + ") "
				szTechString += CvTechInfo.getDescription()
				screen.setLabelAt(TECH_NAME + str(iTech), szTechRecord, szTechString, 1<<0, iX, 5, 0, eFontTitle, eWidGen, 1, 2)
				screen.setHitTest(TECH_NAME + str(iTech), HitTestTypes.HITTEST_NOHIT)

				# Colours
				if szState == CIV_HAS_TECH:
					screen.setStyle(szTechRecord, "Button_TechHas_Style")

				elif szState == CIV_IS_RESEARCHING:
					screen.setStyle(szTechRecord, "Button_TechQueue_Style")

				elif szState == CIV_TECH_AVAILABLE:
					if iEra > self.iCurrentEra:
						screen.setStyle(szTechRecord, "Button_TechNeo_Style")
					elif iEra < self.iCurrentEra:
						screen.setStyle(szTechRecord, "Button_TechArchaic_Style")
					else:
						screen.setStyle(szTechRecord, "Button_TechCoeval_Style")
				else: screen.setStyle(szTechRecord, "Button_TechNo_Style")

	def updateSelectedTech(self, screen, iTech):
		self.iSelectedTech = iTech
		screen.hide("ASPointsLabel")
		screen.hide("AddTechButton")
		if iTech > -1:
			iCost = self.CyPlayer.getAdvancedStartTechCost(iTech, True)
			if iCost > 0:
				iPoints = self.CyPlayer.getAdvancedStartPoints()
				screen.setLabel("ASPointsLabel", "", "<font=4>" + TRNSLTR.getText("TXT_KEY_WB_AS_SELECTED_TECH_COST", (iCost, iPoints)), 1<<0, 180, 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
				if iPoints >= iCost:
					screen.show("AddTechButton")
			szTxt = "<font=4b>" + GC.getTechInfo(iTech).getDescription() + " (" + str(iCost) + unichr(8500) + ')'
			screen.setLabel("SelectedTechLabel", "", szTxt, 1<<0, self.xRes/2, 4, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
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
				iType = info.getFirstFreeUnitClass()
				if iType > -1:
					iType = GC.getUnitClassInfo(iType).getDefaultUnitIndex()
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

	# Tooltip
	def updateTooltip(self, screen, szText, xPos = -1, yPos = -1, uFont = ""):
		if not szText:
			return
		if szText != self.szTxtTT:
			self.szTxtTT = szText
			if not uFont:
				uFont = self.aFontList[5]
			iX, iY = pyTT.makeTooltip(screen, xPos, yPos, szText, uFont, "Tooltip")
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

	def update(self, fDelta):
		if self.bLockedTT:
			POINT = Win32.getCursorPos()
			iX = POINT.x + self.iOffsetTT[0]
			iY = POINT.y + self.iOffsetTT[1]
			if iX < 0: iX = 0
			if iY < 0: iY = 0
			self.screen().moveItem("Tooltip", iX, iY, 0)

		# If we still have techs to complete the initialization of
		if len(self.updates) != 0:
			# Sort them by distance to the current scroll offset so we can make sure to update what the player is looking at first
			self.updates = sorted(self.updates, key=lambda el: abs(el[0] - self.scrollOffs - self.xRes / 2))
			remaining_updates = self.updates[:TECH_PAGING_RATE]
			self.updates = self.updates[TECH_PAGING_RATE:]
			self.refresh((f[1] for f in remaining_updates), True)

		scrollDiff = Win32.getMouseWheelDiff(self.mwlHandle)
		if scrollDiff != 0:
			self.scrollTo(int(self.scrollOffs - scrollDiff * self.xCellDist / 2))

	## DEBUG CODE for investigating handleInput behaviour
	def getNotificationText(self, inputClass):
		notifications = []
		notifications.append("NOTIFY_CLICKED")
		notifications.append("NOTIFY_DBL_CLICKED")
		notifications.append("NOTIFY_FOCUS")
		notifications.append("NOTIFY_UNFOCUS")
		notifications.append("NOTIFY_CURSOR_MOVE_ON")
		notifications.append("NOTIFY_CURSOR_MOVE_OFF")
		notifications.append("NOTIFY_CHARACTER")
		notifications.append("NOTIFY_SCROLL_UP")
		notifications.append("NOTIFY_SCROLL_DOWN")
		notifications.append("NOTIFY_NEW_HORIZONTAL_STOP")
		notifications.append("NOTIFY_NEW_VERTICAL_STOP")
		notifications.append("NOTIFY_LISTBOX_ITEM_SELECTED")
		notifications.append("NOTIFY_MOUSEMOVE") # 12 is also NOTIFY_FLYOUT_ITEM_SELECTED
		notifications.append("13")
		notifications.append("NOTIFY_MOUSEWHEELUP")
		notifications.append("NOTIFY_MOUSEWHEELDOWN")
		notifications.append("16")
		notifications.append("17")
		notifications.append("NOTIFY_LINKEXECUTE")
		notifications.append("NOTIFY_MOVIE_DONE")
		notifications.append("NOTIFY_SLIDER_NEWSTOP")
		notifications.append("NOTIFY_TABLE_HEADER_SELECTED")

		return notifications[inputClass.getNotifyCode()]

	def printInput(self, inputClass):
		print "-----------new input-----------"
		print "Notify code:  " + self.getNotificationText(inputClass)
		print "Data:         " + str(inputClass.getData())
		print "Flags:        " + str(inputClass.getFlags())
		print "ID:           " + str(inputClass.getID())
		print "Func Name:    " + str(inputClass.getFunctionName())
		print "Shift:        " + str(inputClass.isShiftKeyDown())
		print "Control:      " + str(inputClass.isCtrlKeyDown())
		print "Alt:          " + str(inputClass.isAltKeyDown())
		print "MouseX:       " + str(inputClass.getMouseX())
		print "MouseY:       " + str(inputClass.getMouseY())
		# print "ButtonType:   " + self.getWidgetString()
		print "Data1:        " + str(inputClass.getData1())
		print "Data2:        " + str(inputClass.getData2())
		print "Option:       " + str(inputClass.getOption())

	def handleInput(self, inputClass):
		screen = self.screen()
		bAlt, bCtrl, bShift = self.InputData.getModifierKeys()
		iCode	= inputClass.eNotifyCode
		iData	= inputClass.iData
		iData2	= inputClass.iData2
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName
		szFlag	= HandleInputUtil.MOUSE_FLAGS.get(inputClass.uiFlags, "UNKNOWN")

		## DEBUG CODE
		# self.printInput(inputClass)

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
					self.updateTooltip(screen, CyGameTextMgr().getUnitHelp(self.iUnitTT, False, True, True, None))
					self.bUnitTT = None
			return 1
		elif iCode == 17: # Key Up
			if iData == 13: # A
				self.scrollTo(self.scrollOffs - self.xCellDist)
			elif iData == 16: # D
				self.scrollTo(self.scrollOffs + self.xCellDist)
			if self.bUnitTT is None:
				self.bUnitTT = True
			return 1

		# Remove potential Help Text
		screen.hide("Tooltip")
		self.iUnitTT = None
		self.bUnitTT = False

		if iCode == 4: # Mouse Enter
			if BASE == "WID":
				if TYPE == "TECH":
					if CASE[0] == "CURRENT":
						szTxt = "Researching: " + CyGameTextMgr().getTechHelp(self.CyPlayer.getCurrentResearch(), False, True, True, True, -1)
					elif CASE[0] == "REQ":
						szTxt = TRNSLTR.getText("TXT_KEY_MISC_TECH_REQUIRES_KNOWLEDGE_OF", (GC.getTechInfo(ID).getTextKey(),))
					else:
						szTxt = CyGameTextMgr().getTechHelp(ID, False, True, True, True, -1)
					self.updateTooltip(screen, szTxt)
				elif TYPE == "UNIT":
					self.updateTooltip(screen, CyGameTextMgr().getUnitHelp(ID, False, True, True, None))
					self.iUnitTT = ID
					self.bUnitTT = True
				elif TYPE == "BUILDING":
					if CASE[0] == "OBS":
						CvBuildingInfo = GC.getBuildingInfo(ID)
						szTxt = TRNSLTR.getText("TXT_KEY_TECH_OBSOLETES", (CvBuildingInfo.getType(), CvBuildingInfo.getTextKey()))
					else: szTxt = CyGameTextMgr().getBuildingHelp(ID, False, False, True, None, False)
					self.updateTooltip(screen, szTxt)
		elif iCode == 0: # click
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
							elif CASE[0] == "CHOICE" and (self.currentTechState[iType] == CIV_TECH_AVAILABLE or not bShift and self.currentTechState[iType] == CIV_IS_RESEARCHING):
								CyMessageControl().sendResearch(iType, bShift)
								self.updateTechRecords(False)
					elif TYPE == "ERAIM" or TYPE == "ERATEXT":
						self.scrollTo(self.minEraX[ID] - self.minX)
			elif NAME == "AddTechButton":
				CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_TECH, self.iPlayer, -1, -1, self.iSelectedTech, True)	#Action, Player, X, Y, Data, bAdd
				self.updateSelectedTech(screen, -1)
		elif NAME == "HSlider":
			if iCode == 20:
				self.scrollOffs = iData
			self.scroll()
		elif iCode == 11: # List Select
			if NAME == "TC_DebugDD":
				self.cachePlayer(screen.getPullDownData(NAME, screen.getSelectedPullDownID(NAME)))
				self.fullRefresh(screen)

	def onClose(self):
		print "Exit Tech Tree"
		if GC.getPlayer(GC.getGame().getActivePlayer()).getAdvancedStartPoints() > -1:
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, True)

		# Only delete if actually created
		if self.created:
			Win32.unregisterMouseWheelListener(self.mwlHandle)
			del (
				self.screenId, self.InputData, self.szTxtTT, self.iOffsetTT, self.bLockedTT, self.iUnitTT, self.bUnitTT,
				self.xRes, self.yRes, self.aFontList, self.wCell, self.hCell, self.sIcon0, self.sIcon1, self.iSelectedTech,
				self.iPlayer, self.CyPlayer, self.CyTeam, self.iResearch0, self.currentTechState, self.iCurrentEra, self.updates
			)
			self.created = False
