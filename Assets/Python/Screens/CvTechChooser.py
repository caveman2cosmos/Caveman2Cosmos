from CvPythonExtensions import *
import CvScreensInterface as UP
import HandleInputUtil
import PythonToolTip as pyTT
#import BugUtil

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

CELL_GAP = 64
ARROW_SIZE = 8

class CvTechChooser:

	def __init__(self):
		# 0 # self.0 # minEraX = minEraX
		self.scrollOffs = 0
		self.iNumEras = GC.getNumEraInfos()
		self.iNumTechs = GC.getNumTechInfos()
		self.cacheBenefits()

	def screen(self):
		return CyGInterfaceScreen("TechChooser", self.screenId)

	def cachePlayer(self, iPlayer):
		self.iPlayer = iPlayer
		self.CyPlayer = CyPlayer = GC.getPlayer(iPlayer)
		self.CyTeam = CyTeam = GC.getTeam(CyPlayer.getTeam())

		self.iEraFirst = 0
		self.iEraFinal = GC.getNumEraInfos() - 1

		self.iCurrentEra = CyPlayer.getCurrentEra()
		self.iCurrentEra0 = CyPlayer.getCurrentEra()

		currentTechState = self.currentTechState
		iTech = 0
		while iTech < self.iNumTechs:
			if CyTeam.isHasTech(iTech):
				currentTechState[iTech] = CIV_HAS_TECH
			elif CyPlayer.canEverResearch(iTech):
				if CyPlayer.isResearchingTech(iTech):
					currentTechState[iTech] = CIV_IS_RESEARCHING
				else: currentTechState[iTech] = CIV_TECH_AVAILABLE
			else:
				currentTechState[iTech] = CIV_NO_RESEARCH
			iTech += 1

	def interfaceScreen(self, screenId):
		if GC.getGame().isPitbossHost(): 
			return

		self.screenId = screenId

		import InputData
		self.InputData = InputData.instance
		# Tool Tip
		self.szTxtTT = ""
		self.iOffsetTT = []
		self.bLockedTT = False
		self.bUnitTT = False
		self.updates = []
		self.widgets = []

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
		self.minEraX = [""] * GC.getNumEraInfos() # (string > integer) is True
		self.maxX = 0
		for iTech in xrange(self.iNumTechs):
			info = GC.getTechInfo(iTech)
			iX = info.getGridX() * self.xCellDist
			if iX > 0:
				if iX > self.maxX:
					self.maxX = iX

				iEra = info.getEra()
				if self.minEraX[iEra] > iX:
					self.minEraX[iEra] = iX

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		self.currentTechState = [-1] * self.iNumTechs
		iPlayer = GC.getGame().getActivePlayer()
		self.cachePlayer(iPlayer)

		# Base Screen
		screen = self.screen()

		screen.addDDSGFC("ScreenBackground", AFM.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, self.xRes, self.yRes, eWidGen, 1, 2)

		screen.addPanel("TC_BarTop", "", "", True, False, 0, 0, self.xRes, 42, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("TC_BarBot", "", "", True, False, 0, self.yRes - 42, self.xRes, 42, PanelStyles.PANEL_STYLE_BOTTOMBAR)
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

		# screen.setButtonGFC("ERA|Prev", "", "", self.xRes/2 + 8, self.yRes - 30, 24, 24, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_ARROW_LEFT)
		# screen.setButtonGFC("ERA|Next", "", "", self.xRes/2 + 48, self.yRes - 30, 24, 24, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_ARROW_RIGHT)

		# screen.setButtonGFC("ERA|All", "", "", 8, self.yRes - 28, 24, 24, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
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

		screen.addPanel(SCREEN_PANEL, "", "", False, True, 0, 29, self.maxX + self.xCellDist, self.yRes - 85, PanelStyles.PANEL_STYLE_EXTERNAL) # PanelStyles.PANEL_STYLE_EXTERNAL)
		#screen.addScrollPanel(SCREEN_PANEL, "", -8, 29, self.xRes + 16, self.yRes - 85, PanelStyles.PANEL_STYLE_STANDARD) # PanelStyles.PANEL_STYLE_EXTERNAL)
		# screen.addScrollPanel(UPGRADES_GRAPH_ID, u"", self.W_CATEGORIES, self.Y_PEDIA_PAGE - 13, self.xRes - self.W_CATEGORIES, self.H_MID_SECTION, PanelStyles.PANEL_STYLE_STANDARD)
		#screen.setActivation(SCREEN_PANEL, ActivationTypes.ACTIVATE_NORMAL)
		# screen.setHitTest(SCREEN_PANEL, HitTestTypes.HITTEST_NOHIT)
		# import inspect
		# import pprint
		# members = inspect.getmembers(CyInterface(), inspect.ismethod)
		# pp = pprint.PrettyPrinter(depth=4)
		# for tup in members:
		# 	pp.pprint(tup)
		# 	#print name + str(fn) + inspect.getargspec(fn.im_func)# str(GC.getVarnames(fn))
		# 	#', '.join(s for s in x.im_func.func_code.co_varnames)
		# 	# for arg in x.im_func.func_code.
		self.refresh(xrange(self.iNumTechs), False)
		screen.addSlider("HSlider", 8, self.yRes - 80, self.xRes - 16, 20, -self.scrollOffs, 0, self.maxX - self.xRes, WidgetTypes.WIDGET_GENERAL, 0, 0, False)
		screen.moveItem("HSlider", 8, self.yRes - 80, 50)
		#self.scrollOffs = -350
		self.scroll()
		#screen.moveItem(SCREEN_PANEL, -400, 29, 0)

	def scroll(self):
		screen = self.screen()
		screen.moveItem(SCREEN_PANEL, self.scrollOffs, 29, 0)
		# for id, x, y in self.widgets:
		# 	screen.moveItem(id, x + self.scrollOffs, y, 0)

	def refresh(self, techs, bFull):
		#timer = BugUtil.Timer('fullRefresh')
		screen = self.screen()
		eWidGen = WidgetTypes.WIDGET_GENERAL

		#eFontTitle = FontTypes.TITLE_FONT

		# if self.iEraFirst == self.iEraFinal:
		# 	if self.iCurrentEra + 1 < self.iNumEras:
		# 		xNext = 0 # self.minEraX[self.iCurrentEra + 1]
		# 	else:
		# 		xNext = 0 # self.minEraX[self.iCurrentEra]
		# 	bSingleEra = True
		# 	screen.setText("ERA", "", "<font=4b>" + GC.getEraInfo(self.iCurrentEra).getDescription(), 1<<1, self.xRes/2, self.yRes-32, 0, eFontTitle, eWidGen, 1, 2)
		# else:
		# 	self.iCurrentEra = None
		# 	bSingleEra = False
		# 	screen.setText("ERA", "", "<font=4b>" + GC.getEraInfo(self.iCurrentEra0).getDescription(), 1<<1, self.xRes/2, self.yRes-32, 0, eFontTitle, eWidGen, 1, 2)

		# if bSingleEra and self.iCurrentEra:
		# 	screen.show("ERA|Prev")
		# else:
		# 	screen.hide("ERA|Prev")

		# if bSingleEra and self.iCurrentEra + 1 != self.iNumEras:
		# 	screen.show("ERA|Next")
		# else:
		# 	screen.hide("ERA|Next")

		# # Filter
		# screen.addDropDownBoxGFC("PlatyHideToEra", self.xRes - 132, self.yRes - 33, 128, eWidGen, 1, 2, eFontTitle)
		# screen.addDropDownBoxGFC("PlatyHideFromEra", self.xRes - 268, self.yRes - 33, 128, eWidGen, 1, 2, eFontTitle)

		# for iEra in range(self.iNumEras):
		# 	if iEra <= self.iEraFinal:
		# 		screen.addPullDownString("PlatyHideFromEra", GC.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == self.iEraFirst)
		# 		if iEra == self.iEraFinal:
		# 			screen.addPullDownString("PlatyHideToEra", GC.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == self.iEraFinal)
		# 	elif iEra >= self.iEraFirst:
		# 		screen.addPullDownString("PlatyHideToEra", GC.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == self.iEraFinal)
		dy = self.yRes - 85

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

		iMinX = 0 # self.minEraX[self.iEraFirst]

		for iTech in techs:
			CvTechInfo = GC.getTechInfo(iTech)

			x0 = CvTechInfo.getGridX()
			if x0 < 1:
				continue

			iEra = CvTechInfo.getEra()
			# if bSingleEra:
			# 	if iEra != self.iCurrentEra and x0 != xNext:
			# 		continue

			# elif iEra < self.iEraFirst or iEra > self.iEraFinal:
			# 	continue

			szTech = str(iTech)
			szTechRecord = TECH_CHOICE + szTech
			y0 = CvTechInfo.getGridY()
			xRel = x0 * self.xCellDist
			iX = xRel
			# if iX > self.maxX:
			# 	self.maxX = iX
			iY = yEmptySpace + ((y0 - 1) * yCellDist) / 2

			if not bFull:
				screen.setImageButtonAt(szTechRecord, SCREEN_PANEL, "", iX, iY, self.wCell, self.hCell, eWidGen, 1, 2)
				screen.addDDSGFCAt(ICON + szTech, szTechRecord, CvTechInfo.getButton(), 3, 5, self.sIcon0, self.sIcon0, eWidGen, 1, 2, False)
				screen.setHitTest(ICON + szTech, HitTestTypes.HITTEST_NOHIT)
				self.updates.append((xRel, iTech))
				self.widgets.append((szTechRecord, iX, iY))
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
					if iEra < self.iEraFirst or iEra > self.iEraFinal:
						continue

					x1 = techInfoX.getGridX()
					y1 = techInfoX.getGridY()
					iX = (x1 - iMinX) * self.xCellDist + self.wCell - 2
					iY = yEmptySpace + ((y1 - 1) * yCellDist) / 2 - 4

					xDiff = x0 - x1
					yDiff = y0 - y1
					xOff = xDiff * 62 + (xDiff - 1) * self.wCell

					def add_arrow_head(x, y):
						screen.addDDSGFCAt("", SCREEN_PANEL, ARROW_HEAD, x, y, ARROW_SIZE, ARROW_SIZE, eWidGen, 1, 2, False)
						# self.widgets.append((szTechRecord + "PREREQ" + str(i) + "HEAD", x, y))

					def add_line_h(x, y, len):
						screen.addDDSGFCAt("", SCREEN_PANEL, ARROW_X, x, y, len, ARROW_SIZE, eWidGen, 1, 2, False)
						# self.widgets.append((szTechRecord + "PREREQ" + str(i) + "H" + str(id), x, y))

					def add_line_v(x, y, len):
						screen.addDDSGFCAt("", SCREEN_PANEL, ARROW_Y, x, y, ARROW_SIZE, len, eWidGen, 1, 2, False)
						# self.widgets.append((szTechRecord + "PREREQ" + str(i) + "V", x, y))

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

		# self.updates.append((x0, update_tech))
		#timer.log()
		if not bFull:
			self.updateTechRecords(True)

	def updateTechRecords(self, bForce):

		screen = self.screen()
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		CyPlayer = self.CyPlayer
		iEraFirst = self.iEraFirst
		iEraFinal = self.iEraFinal

		# Progress Bar
		iTech = CyPlayer.getCurrentResearch()
		if self.iResearch0 != iTech:
			if iTech > -1:
				screen.hide("TC_Header")
				iProgress = self.CyTeam.getResearchProgress(iTech)
				iCost = self.CyTeam.getResearchCost(iTech)
				iOverflow = CyPlayer.getOverflowResearch() * CyPlayer.calculateResearchModifier(iTech) /100
				stackBar = "progressBar"
				screen.setBarPercentage(stackBar, InfoBarTypes.INFOBAR_STORED, iProgress * 1.0 / iCost)
				if iCost > iProgress + iOverflow:
					screen.setBarPercentage(stackBar, InfoBarTypes.INFOBAR_RATE, CyPlayer.calculateResearchRate(iTech) * 1.0 / (iCost - iProgress - iOverflow))
				screen.show(stackBar)

				szTxt = "<font=3>" + GC.getTechInfo(iTech).getDescription() + ' (' + str(CyPlayer.getResearchTurnsLeft(iTech, True)) + ")"
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

		# if iEraFirst == iEraFinal:
		# 	bSingleEra = True
		# 	iCurrentEra = self.iCurrentEra
		# 	if iCurrentEra + 1 < self.iNumEras:
		# 		xNext = self.minEraX[iCurrentEra + 1]
		# 	else: xNext = self.minEraX[iCurrentEra]
		# else:
		bSingleEra = False

		currentTechState = self.currentTechState

		# Analyze change
		lChanged = []
		iTech = 0
		while iTech < self.iNumTechs:
			CvTechInfo = GC.getTechInfo(iTech)
			iX = CvTechInfo.getGridX()
			if iX > 0:
				iEra = CvTechInfo.getEra()
				if bSingleEra and (iEra == iCurrentEra or iX == xNext) or not bSingleEra and iEra >= iEraFirst and iEra <= iEraFinal:

					if currentTechState[iTech] == CIV_HAS_TECH:
						if bForce:
							lChanged.append((iTech, CvTechInfo, iEra))

					elif currentTechState[iTech] == CIV_NO_RESEARCH:
						if bForce:
							lChanged.append((iTech, CvTechInfo, iEra))

					elif CyPlayer.isResearchingTech(iTech):
						currentTechState[iTech] = CIV_IS_RESEARCHING
						lChanged.append((iTech, CvTechInfo, iEra))

					elif bForce or currentTechState[iTech] != CIV_TECH_AVAILABLE:
						currentTechState[iTech] = CIV_TECH_AVAILABLE
						lChanged.append((iTech, CvTechInfo, iEra))
			iTech += 1

		# Make change
		if lChanged:
			advisors = [unichr(8855), unichr(8857), unichr(8500), unichr(8501), unichr(8502), unichr(8483)]
			iX = self.sIcon0 + 6
			iCurrentEra0 = self.iCurrentEra0
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
					szTechString += str(CyPlayer.getQueuePosition(iTech)) + ") "
				szTechString += CvTechInfo.getDescription()
				screen.setLabelAt(TECH_NAME + str(iTech), szTechRecord, szTechString, 1<<0, iX, 5, 0, eFontTitle, eWidGen, 1, 2)
				screen.setHitTest(TECH_NAME + str(iTech), HitTestTypes.HITTEST_NOHIT)

				# Colours
				if szState == CIV_HAS_TECH:
					screen.setStyle(szTechRecord, "Button_TechHas_Style")

				elif szState == CIV_IS_RESEARCHING:
					screen.setStyle(szTechRecord, "Button_TechQueue_Style")

				elif szState == CIV_TECH_AVAILABLE:
					if iEra > iCurrentEra0:
						screen.setStyle(szTechRecord, "Button_TechNeo_Style")
					elif iEra < iCurrentEra0:
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
				screen.setLabel("ASPointsLabel", "", "<font=4>" + TRNSLTR.getText("TXT_KEY_WB_AS_SELECTED_TECH_COST", (iCost, iPoints)), 1<<0, 180, self.yRes - 42, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
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
			POINT = GC.getCursorPos()
			self.iOffsetTT = [iX - POINT.x, iY - POINT.y]
		else:
			if xPos == yPos == -1:
				POINT = GC.getCursorPos()
				screen.moveItem("Tooltip", POINT.x + self.iOffsetTT[0], POINT.y + self.iOffsetTT[1], 0)
			screen.moveToFront("Tooltip")
			screen.show("Tooltip")
		if xPos == yPos == -1:
			self.bLockedTT = True

	def update(self, fDelta):
		if self.bLockedTT:
			POINT = GC.getCursorPos()
			iX = POINT.x + self.iOffsetTT[0]
			iY = POINT.y + self.iOffsetTT[1]
			if iX < 0: iX = 0
			if iY < 0: iY = 0
			self.screen().moveItem("Tooltip", iX, iY, 0)

			if len(self.updates) != 0:
				self.updates = sorted(self.updates, key=lambda el: abs(el[0] - self.scrollOffs))
				remaining_updates = self.updates[:10] # 
				self.updates = self.updates[10:]
				self.refresh((f[1] for f in remaining_updates), True)
			# self.updates
			# for upd in remaining_updates:
			# 	upd[1]()
			# 	self.updates.remove(upd)
			# 	num = num + 1
			# 	if num > 50:
			# 		break

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

		#self.printInput(inputClass)
		#if NAME == SCREEN_PANEL:
		#print str((NAME, iCode, iData, ID, inputClass.uiFlags))

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

		if iCode == 6: # Character
			if iData in (45, 49, 56): # Ctrl, Shift, Alt
				if self.bUnitTT:
					self.updateTooltip(screen, CyGameTextMgr().getUnitHelp(self.iUnitTT, False, True, True, None))
					self.bUnitTT = None
			return 1
		elif iCode == 17: # Key Up
			# if iData in (13, 102):
			# 	self.back()
			# elif iData in (16, 103):
			# 	self.forward()
			# el
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

		elif not iCode: # click

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

			# elif BASE == "ERA":
			# 	if TYPE == "Next":
			# 		if self.iCurrentEra + 1 != self.iNumEras:
			# 			self.iCurrentEra += 1
			# 			self.iEraFirst = self.iEraFinal = self.iCurrentEra
			# 			self.fullRefresh(screen)

			# 	elif TYPE == "Prev":
			# 		if self.iCurrentEra:
			# 			self.iCurrentEra -= 1
			# 			self.iEraFirst = self.iEraFinal = self.iCurrentEra
			# 			self.fullRefresh(screen)
			# 	elif TYPE == "All":
			# 		if self.iEraFirst != 0 or self.iEraFinal != self.iNumEras - 1:
			# 			self.iEraFirst = 0
			# 			self.iEraFinal = self.iNumEras - 1
			# 			self.fullRefresh(screen)
			# 	else:
			# 		self.iEraFirst = self.iEraFinal = self.iCurrentEra = self.iCurrentEra0
			# 		self.fullRefresh(screen)

			elif NAME == "AddTechButton":
				CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_TECH, self.iPlayer, -1, -1, self.iSelectedTech, True)	#Action, Player, X, Y, Data, bAdd
				self.updateSelectedTech(screen, -1)
		elif NAME == "HSlider" and iCode == 20:
			self.scrollOffs = -iData
			self.scroll()
		# elif iCode == 11: # List Select
		# 	if NAME == "TC_DebugDD":
		# 		self.cachePlayer(screen.getPullDownData(NAME, screen.getSelectedPullDownID(NAME)))
		# 		self.fullRefresh(screen)

		# 	elif NAME == "PlatyHideFromEra":
		# 		self.iEraFirst = screen.getPullDownData("PlatyHideFromEra", screen.getSelectedPullDownID("PlatyHideFromEra"))
		# 		self.fullRefresh(screen)

		# 	elif NAME == "PlatyHideToEra":
		# 		self.iEraFinal = screen.getPullDownData("PlatyHideToEra", screen.getSelectedPullDownID("PlatyHideToEra"))
		# 		self.fullRefresh(screen)

	# def back(self):
	# 	if self.iCurrentEra is None:
	# 		self.iEraFirst = self.iEraFinal = self.iCurrentEra = self.iCurrentEra0
	# 		self.fullRefresh(self.screen())
	# 	elif self.iCurrentEra:
	# 		self.iCurrentEra -= 1
	# 		self.iEraFirst = self.iEraFinal = self.iCurrentEra
	# 		self.fullRefresh(self.screen())

	# def forward(self):
	# 	if self.iCurrentEra is None:
	# 		self.iEraFirst = self.iEraFinal = self.iCurrentEra = self.iCurrentEra0
	# 		self.fullRefresh(self.screen())
	# 	elif self.iCurrentEra + 1 != self.iNumEras:
	# 		self.iCurrentEra += 1
	# 		self.iEraFirst = self.iEraFinal = self.iCurrentEra
	# 		self.fullRefresh(self.screen())

	def onClose(self):
		print "Exit Tech Tree"
		if GC.getPlayer(GC.getGame().getActivePlayer()).getAdvancedStartPoints() > -1:
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, True)
		del (
			self.screenId, self.InputData, self.szTxtTT, self.iOffsetTT, self.bLockedTT, self.iUnitTT, self.bUnitTT,
			self.xRes, self.yRes, self.aFontList, self.wCell, self.hCell, self.sIcon0, self.sIcon1, self.iSelectedTech,
			self.iPlayer, self.CyPlayer, self.CyTeam, self.iResearch0, self.currentTechState,
			self.iEraFirst, self.iEraFinal, self.iCurrentEra, self.iCurrentEra0, self.updates
		)