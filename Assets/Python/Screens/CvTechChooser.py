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

class CvTechChooser:

	def __init__(self):
		# Cache minimum X coordinate per era for era partitioning.
		minEraX = [""] * GC.getNumEraInfos() # (string > integer) is True
		iNumTechs = GC.getNumTechInfos()
		for iTech in xrange(iNumTechs):
			info = GC.getTechInfo(iTech)
			iX = info.getGridX()
			if iX > 0:
				iEra = info.getEra()
				if minEraX[iEra] > iX:
					minEraX[iEra] = iX
		self.minEraX = minEraX
		self.cacheBenefits(iNumTechs)

	def screen(self):
		return CyGInterfaceScreen("TechChooser", self.screenId)

	def cachePlayer(self, iPlayer):
		self.iPlayer = iPlayer
		self.CyPlayer = CyPlayer = GC.getPlayer(iPlayer)
		self.CyTeam = CyTeam = GC.getTeam(CyPlayer.getTeam())

		self.iEraFirst = self.iEraFinal = self.iCurrentEra = self.iCurrentEra0 = CyPlayer.getCurrentEra()

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
		if GC.getGame().isPitbossHost(): return
		self.screenId = screenId
		self.iNumTechs = GC.getNumTechInfos()
		self.iNumEras = GC.getNumEraInfos()

		import InputData
		self.InputData = InputData.instance
		# Tool Tip
		self.szTxtTT = ""
		self.iOffsetTT = []
		self.bLockedTT = False
		self.bUnitTT = False

		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y
		self.aFontList = SR.aFontList

		self.wCell = 128 + self.xRes / 6

		if yRes > 1000:
			self.sIcon0 = sIcon0 = 64
		elif yRes > 800:
			self.sIcon0 = sIcon0 = 56
		else:
			self.sIcon0 = sIcon0 = 48

		self.hCell = sIcon0 + 8
		self.sIcon1 = sIcon0 / 2

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		self.currentTechState = [-1] * self.iNumTechs
		iPlayer = GC.getGame().getActivePlayer()
		self.cachePlayer(iPlayer)

		# Base Screen
		screen = self.screen()

		screen.addDDSGFC("ScreenBackground", AFM.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, xRes, yRes, eWidGen, 1, 2)

		screen.addPanel("TC_BarTop", "", "", True, False, 0, 0, xRes, 42, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("TC_BarBot", "", "", True, False, 0, yRes - 42, xRes, 42, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.setLabel("TC_Header", "", "<font=4b>" + TRNSLTR.getText("TXT_KEY_TECH_CHOOSER_TITLE", ()), 1<<2, xRes/2, 4, 0, eFontTitle, eWidGen, 1, 2)

		screen.setText("TC_Exit", "", "<font=4b>" + TRNSLTR.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()), 1<<1, xRes - 8, 2, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		stackBar = "progressBar"
		screen.addStackedBarGFC(stackBar, 256, 2, self.xRes - 512, 32, InfoBarTypes.NUM_INFOBAR_TYPES, eWidGen, 1, 2)
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_STORED, GC.getInfoTypeForString("COLOR_RESEARCH_STORED"))
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_RATE, GC.getInfoTypeForString("COLOR_RESEARCH_RATE"))
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_EMPTY, GC.getInfoTypeForString("COLOR_EMPTY"))
		screen.hide(stackBar)
		screen.setImageButton("WID|TECH|CURRENT0", "", 256, 3, xRes - 512, 30, eWidGen, 1, 2)
		screen.hide("WID|TECH|CURRENT0")
		self.iResearch0 = -1

		screen.setButtonGFC("ERA|Prev", "", "", xRes/2 + 8, yRes - 30, 24, 24, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_ARROW_LEFT)
		screen.setButtonGFC("ERA|Next", "", "", xRes/2 + 48, yRes - 30, 24, 24, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_ARROW_RIGHT)

		screen.setButtonGFC("ERA|All", "", "", 8, yRes - 28, 24, 24, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
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
			screen.setButtonGFC("AddTechButton", TRNSLTR.getText("TXT_KEY_WB_AS_ADD_TECH", ()), "", xRes/2 - 158, 4, 150, 30, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_STANDARD)
			screen.hide("AddTechButton")

		screen.setRenderInterfaceOnly(True)
		screen.showWindowBackground(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		self.fullRefresh(screen)

	def fullRefresh(self, screen):
		#timer = BugUtil.Timer('fullRefresh')
		iNumTechs = self.iNumTechs
		iNumEras = self.iNumEras
		iPlayer = self.iPlayer
		CyPlayer = self.CyPlayer
		CyTeam = self.CyTeam
		xRes = self.xRes
		yRes = self.yRes
		iCurrentEra = self.iCurrentEra
		iEraFirst = self.iEraFirst
		iEraFinal = self.iEraFinal

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		iMinX = self.minEraX[iEraFirst]
		if iEraFirst == iEraFinal:
			if iCurrentEra + 1 < iNumEras:
				xNext = self.minEraX[iCurrentEra + 1]
			else: xNext = self.minEraX[iCurrentEra]
			bSingleEra = True
			screen.setText("ERA", "", "<font=4b>" + GC.getEraInfo(self.iCurrentEra).getDescription(), 1<<1, xRes/2, yRes-32, 0, eFontTitle, eWidGen, 1, 2)
		else:
			self.iCurrentEra = None
			bSingleEra = False
			screen.setText("ERA", "", "<font=4b>" + GC.getEraInfo(self.iCurrentEra0).getDescription(), 1<<1, xRes/2, yRes-32, 0, eFontTitle, eWidGen, 1, 2)

		if bSingleEra and iCurrentEra:
			screen.show("ERA|Prev")
		else: screen.hide("ERA|Prev")

		if bSingleEra and iCurrentEra + 1 != iNumEras:
			screen.show("ERA|Next")
		else: screen.hide("ERA|Next")

		# Filter
		screen.addDropDownBoxGFC("PlatyHideToEra", xRes - 132, yRes - 33, 128, eWidGen, 1, 2, eFontTitle)
		screen.addDropDownBoxGFC("PlatyHideFromEra", xRes - 268, yRes - 33, 128, eWidGen, 1, 2, eFontTitle)

		iEra = 0
		while iEra < iNumEras:
			if iEra <= iEraFinal:
				screen.addPullDownString("PlatyHideFromEra", GC.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == iEraFirst)
				if iEra == iEraFirst:
					screen.addPullDownString("PlatyHideToEra", GC.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == iEraFinal)
			elif iEra >= iEraFirst:
				screen.addPullDownString("PlatyHideToEra", GC.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == iEraFinal)
			iEra += 1

		ScrPnl = "TechList"
		dy = yRes - 85
		screen.addScrollPanel(ScrPnl, "", -8, 29, xRes + 16, dy, PanelStyles.PANEL_STYLE_EXTERNAL)

		sIcon0 = self.sIcon0
		sIcon1 = self.sIcon1
		hCell = self.hCell
		yEmptySpace = (dy - 10 * hCell) / 10
		if yEmptySpace < 0:
			yEmptySpace = 0
		yCellDist = yEmptySpace + hCell
		yEmptySpace /= 2
		yBoxSpacing = yCellDist/2

		# Place techs
		NUM_AND_TECH_PREREQS = GC.getNUM_AND_TECH_PREREQS()
		NUM_OR_TECH_PREREQS = GC.getNUM_OR_TECH_PREREQS()
		RED_X = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_RED_X").getPath()
		ARROW_X = AFM.getInterfaceArtInfo("ARROW_X").getPath()
		ARROW_Y = AFM.getInterfaceArtInfo("ARROW_Y").getPath()
		ARROW_HEAD = AFM.getInterfaceArtInfo("ARROW_HEAD").getPath()

		wCell = self.wCell
		xCellDist = 64 + wCell
		ARROW = "Arrow"
		yArrow0 = hCell / 2
		yArrow1 = hCell*3/8
		yArrow2 = hCell / 4
		yArrow3 = hCell*5/8
		yArrow4 = hCell*3/4
		techBenefits = self.techBenefits
		dx = sIcon1 + 1
		iCount = 0
		iMaxElements = (wCell - sIcon0 - 8) / dx

		CELL = "CELL"
		ICON = "ICON"
		TECH_CHOICE = "WID|TECH|CHOICE"
		TECH_REQ = "WID|TECH|REQ"
		iTech = -1
		while iTech < iNumTechs - 1:
			iTech += 1

			CvTechInfo = GC.getTechInfo(iTech)
			x0 = CvTechInfo.getGridX()
			if x0 < 1: continue

			iEra = CvTechInfo.getEra()
			if bSingleEra:
				if iEra != iCurrentEra and x0 != xNext:
					continue

			elif iEra < iEraFirst or iEra > iEraFinal:
				continue

			szTech = str(iTech)
			y0 = CvTechInfo.getGridY()
			iX = (x0 - iMinX) * xCellDist
			iY = yEmptySpace + ((y0 - 1) * yCellDist) / 2
			szTechRecord = TECH_CHOICE + szTech

			screen.setImageButtonAt(szTechRecord, ScrPnl, "", iX, iY, wCell, hCell, eWidGen, 1, 2)
			screen.addDDSGFCAt(ICON + szTech, szTechRecord, CvTechInfo.getButton(), 3, 5, sIcon0, sIcon0, eWidGen, 1, 2, False)
			screen.setHitTest(ICON + szTech, HitTestTypes.HITTEST_NOHIT)

			# Requires
			iX = wCell
			for i in xrange(NUM_AND_TECH_PREREQS):
				iTechX = CvTechInfo.getPrereqAndTechs(i)
				if iTechX == -1: break
				iX -= dx
				screen.setImageButtonAt(TECH_REQ + str(iTechX) + "|" + szTech, szTechRecord, GC.getTechInfo(iTechX).getButton(), iX, 2, sIcon1, sIcon1, eWidGen, 1, 2)

			for i in xrange(NUM_OR_TECH_PREREQS):
				iTechX = CvTechInfo.getPrereqOrTechs(i)
				if iTechX == -1: break
				techInfoX = GC.getTechInfo(iTechX)

				iEra = techInfoX.getEra()
				if iEra < iEraFirst or iEra > iEraFinal:
					continue

				x1 = techInfoX.getGridX()
				y1 = techInfoX.getGridY()
				iX = (x1 - iMinX) * xCellDist + wCell - 2
				iY = yEmptySpace + ((y1 - 1) * yCellDist) / 2 - 4

				xDiff = x0 - x1
				yDiff = y0 - y1
				xOff = xDiff * 62 + (xDiff - 1) * wCell

				if not yDiff:
					screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX, iY + yArrow0, xOff, 8, eWidGen, 1, 2, False)
					screen.addDDSGFCAt("", ScrPnl, ARROW_HEAD, iX + xOff, iY + yArrow0, 8, 8, eWidGen, 1, 2, False)
				elif yDiff < 0:
					if yDiff < -3 and xDiff == 1:
						dy = yDiff * yBoxSpacing + hCell/2
						yArrow = iY + yArrow2
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX, yArrow, xOff/3 + 4, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_Y, iX + xOff/3, yArrow + 4 + dy, 8, -dy, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX + 4 + xOff/3, yArrow + dy, xOff * 2/3, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_HEAD, iX + xOff, yArrow + dy, 8, 8, eWidGen, 1, 2, False)
					else:
						dy = yDiff * yBoxSpacing + hCell/4
						yArrow = iY + yArrow1
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX, yArrow, xOff/2 + 4, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_Y, iX + xOff/2, yArrow + 4 + dy, 8, -dy, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX + 4 + xOff/2, yArrow + dy, xOff/2, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_HEAD, iX + xOff, yArrow + dy, 8, 8, eWidGen, 1, 2, False)
				else:
					if yDiff > 3 and xDiff == 1:
						dy = yDiff * yBoxSpacing - hCell/2
						yArrow = iY + yArrow4
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX, yArrow, xOff/3 + 4, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_Y, iX + xOff/3, yArrow + 4, 8, dy, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX + 4 + xOff/3, yArrow + dy, xOff * 2/3, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_HEAD, iX + xOff, yArrow + dy, 8, 8, eWidGen, 1, 2, False)
					else:
						dy = yDiff * yBoxSpacing - hCell/4
						yArrow = iY + yArrow3
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX, yArrow, xOff/2 + 4, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_Y, iX + xOff/2, yArrow + 4, 8, dy, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX + 4 + xOff/2, yArrow + dy, xOff/2, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_HEAD, iX + xOff, yArrow + dy, 8, 8, eWidGen, 1, 2, False)

			# Unlocks
			iX = sIcon0 + 6
			iY = sIcon0 + 4 - sIcon1

			benefits = techBenefits[iTech]
			iLength = len(benefits)
			if iLength > iMaxElements:
				iLength = iMaxElements

			i = 0
			while i < iLength:
				sType, iItem = benefits[i]
				if sType == "UnlockUnit":
					screen.setImageButtonAt("WID|UNIT" + str(iItem) + '|' + str(iCount), szTechRecord, GC.getUnitInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, eWidGen, 1, 2)
					iCount += 1
				elif sType == "UnlockBuilding":
					screen.setImageButtonAt("WID|BUILDING" + str(iItem) + '|' + str(iCount), szTechRecord, GC.getBuildingInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, eWidGen, 1, 2)
					iCount += 1
				elif sType == "ObsoleteBuilding":
					screen.addDDSGFCAt("", szTechRecord, GC.getBuildingInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, eWidGen, 1, 2, False)
					screen.setImageButtonAt("WID|BUILDING|OBS" + str(iItem), szTechRecord, RED_X, iX, iY, sIcon1, sIcon1, eWidGen, 1, 2)
				elif sType == "UnlockSpecialBuilding":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getSpecialBuildingInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_SPECIAL_BUILDING, iTech, iItem, False)
				elif sType == "ObsoleteSpecialBuilding":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getSpecialBuildingInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_OBSOLETE_SPECIAL, iTech, iItem, False)
					screen.addDDSGFCAt("Obsolete" + str(iTech * 1000 + i), szTechRecord, RED_X, iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_OBSOLETE_SPECIAL, iItem, -1, False)
				elif sType == "RevealBonus":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getBonusInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_BONUS_REVEAL, iTech, iItem, False)
				elif sType == "ObsoleteBonus":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getBonusInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_OBSOLETE_BONUS, iTech, iItem, False)
					screen.addDDSGFCAt("Obsolete" + str(iTech * 1000 + i), szTechRecord, RED_X, iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_OBSOLETE_BONUS, iItem, -1, False)
				elif sType == "RouteChange":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MOVE_BONUS").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_MOVE_BONUS, iTech, -1, False)
				elif sType == "UnlockPromotion":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getPromotionInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROMOTION, iItem, -1, False)
				elif sType == "FreeUnit":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getUnitInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_FREE_UNIT, iItem, iTech, False)
				elif sType == "FeatureProduction":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_FEATURE_PRODUCTION").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_FEATURE_PRODUCTION, iTech, -1, False)
				elif sType == "WorkerSpeed":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WORKER_SPEED").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_WORKER_RATE, iTech, -1, False)
				elif sType == "TradeRoute":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_TRADE_ROUTES").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_TRADE_ROUTES, iTech, -1, False)
				elif sType == "Health":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_HEALTH").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_HEALTH_RATE, iTech, -1, False)
				elif sType == "Happiness":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_HAPPINESS").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_HAPPINESS_RATE, iTech, -1, False)
				elif sType == "Population":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_POPULATION").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_HAPPINESS_RATE, iTech, -1, False)
				elif sType == "FreeTech":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_FREETECH").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_FREE_TECH, iTech, -1, False)
				elif sType == "WaterSight":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_LOS").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_LOS_BONUS, iTech, -1, False)
				elif sType == "MapCentering":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPCENTER").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_MAP_CENTER, iTech, -1, False)
				elif sType == "MapVisible":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPREVEAL").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_MAP_REVEAL, iTech, -1, False)
				elif sType == "MapTrading":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPTRADING").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_MAP_TRADE, iTech, -1, False)
				elif sType == "TechTrading":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_TECHTRADING").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_TECH_TRADE, iTech, -1, False)
				elif sType == "GoldTrading":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_GOLDTRADING").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_GOLD_TRADE, iTech, -1, False)
				elif sType == "OpenBorders":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_OPENBORDERS").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_OPEN_BORDERS, iTech, -1, False)
				elif sType == "DefensivePact":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_DEFENSIVEPACT").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_DEFENSIVE_PACT, iTech, -1, False)
				elif sType == "PermanentAlliance":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_PERMALLIANCE").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_PERMANENT_ALLIANCE, iTech, -1, False)
				elif sType == "VassalState":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_VASSAL").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_VASSAL_STATE, iTech, -1, False)
				elif sType == "BridgeBuilding":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_BRIDGEBUILDING").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_BUILD_BRIDGE, iTech, -1, False)
				elif sType == "EnablesIrrigation":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_IRRIGATION").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_IRRIGATION, iTech, -1, False)
				elif sType == "IgnoreIrrigation":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_NOIRRIGATION").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_IGNORE_IRRIGATION, iTech, -1, False)
				elif sType == "WaterWork":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERWORK").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_WATER_WORK, iTech, -1, False)
				elif sType == "UnlockImprovement":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getBuildInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_IMPROVEMENT, iTech, iItem, False)
				elif sType == "DomainMoves":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERMOVES").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_DOMAIN_EXTRA_MOVES, iTech, iItem, False)
				elif sType == "CommerceFlexible":
					if iItem == CommerceTypes.COMMERCE_CULTURE:
						temp = AFM.getInterfaceArtInfo("INTERFACE_TECH_CULTURE").getPath()
					elif iItem == CommerceTypes.COMMERCE_ESPIONAGE:
						temp = AFM.getInterfaceArtInfo("INTERFACE_TECH_ESPIONAGE").getPath()
					else: temp = AFM.getInterfaceArtInfo("INTERFACE_GENERAL_QUESTIONMARK").getPath()
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, temp, iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_ADJUST, iTech, iItem, False)
				elif sType == "TerrainTrade":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERTRADE").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_TERRAIN_TRADE, iTech, iItem, False)
				elif sType == "RiverTrade":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_RIVERTRADE").getPath(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_TERRAIN_TRADE, iTech, GC.getNumTerrainInfos(), False)
				elif sType == "ImprovementYield":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getImprovementInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_YIELD_CHANGE, iTech, iItem, False)
				elif sType == "UnlockCivic":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getCivicInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_CIVIC_REVEAL, iTech, iItem, False)
				elif sType == "UnlockProject":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getProjectInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT, iItem, 1, False)
				elif sType == "UnlockProcess":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getProcessInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_PROCESS_INFO, iTech, iItem, False)
				elif sType == "UnlockReligion":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getReligionInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_FOUND_RELIGION, iTech, iItem, False)
				elif sType == "UnlockCorporation":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getCorporationInfo(iItem).getButton(), iX, iY, sIcon1, sIcon1, WidgetTypes.WIDGET_HELP_FOUND_CORPORATION, iTech, iItem, False)
				iX += dx
				i += 1

		#timer.log()
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

		if iEraFirst == iEraFinal:
			bSingleEra = True
			iCurrentEra = self.iCurrentEra
			if iCurrentEra + 1 < self.iNumEras:
				xNext = self.minEraX[iCurrentEra + 1]
			else: xNext = self.minEraX[iCurrentEra]
		else:
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
			TECH_CHOICE = "WID|TECH|CHOICE"
			TECH_NAME = "TechName"
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


	def cacheBenefits(self, iNumTechs):
		techBenefits = []
		iNumDomains = int(DomainTypes.NUM_DOMAIN_TYPES)
		iNumCommerce = int(CommerceTypes.NUM_COMMERCE_TYPES)
		iNumTerrains = GC.getNumTerrainInfos()

		iTech = 0
		while iTech < iNumTechs:
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
			while iTech < iNumTechs:
				if info.getTechMovementChange(iTech):
					techBenefits[iTech].append(["RouteChange", iType])
				iTech += 1
			iType += 1

		iNumYields = int(YieldTypes.NUM_YIELD_TYPES)
		iType = 0
		while iType < GC.getNumImprovementInfos():
			info = GC.getImprovementInfo(iType)
			iTech = 0
			while iTech < iNumTechs:
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

	def handleInput(self, inputClass):
		screen = self.screen()
		bAlt, bCtrl, bShift = self.InputData.getModifierKeys()
		iCode	= inputClass.eNotifyCode
		iData	= inputClass.iData
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

		if iCode == 6: # Character
			if iData in (45, 49, 56): # Ctrl, Shift, Alt
				if self.bUnitTT:
					self.updateTooltip(screen, CyGameTextMgr().getUnitHelp(self.iUnitTT, False, True, True, None))
					self.bUnitTT = None
			return 1
		elif iCode == 17: # Key Up
			if iData in (13, 102):
				self.back()
			elif iData in (16, 103):
				self.forward()
			elif self.bUnitTT is None:
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

			elif BASE == "ERA":
				if TYPE == "Next":
					if self.iCurrentEra + 1 != self.iNumEras:
						self.iCurrentEra += 1
						self.iEraFirst = self.iEraFinal = self.iCurrentEra
						self.fullRefresh(screen)

				elif TYPE == "Prev":
					if self.iCurrentEra:
						self.iCurrentEra -= 1
						self.iEraFirst = self.iEraFinal = self.iCurrentEra
						self.fullRefresh(screen)
				elif TYPE == "All":
					if self.iEraFirst != 0 or self.iEraFinal != self.iNumEras - 1:
						self.iEraFirst = 0
						self.iEraFinal = self.iNumEras - 1
						self.fullRefresh(screen)
				else:
					self.iEraFirst = self.iEraFinal = self.iCurrentEra = self.iCurrentEra0
					self.fullRefresh(screen)

			elif NAME == "AddTechButton":
				CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_TECH, self.iPlayer, -1, -1, self.iSelectedTech, True)	#Action, Player, X, Y, Data, bAdd
				self.updateSelectedTech(screen, -1)

		elif iCode == 11: # List Select
			if NAME == "TC_DebugDD":
				self.cachePlayer(screen.getPullDownData(NAME, screen.getSelectedPullDownID(NAME)))
				self.fullRefresh(screen)

			elif NAME == "PlatyHideFromEra":
				self.iEraFirst = screen.getPullDownData("PlatyHideFromEra", screen.getSelectedPullDownID("PlatyHideFromEra"))
				self.fullRefresh(screen)

			elif NAME == "PlatyHideToEra":
				self.iEraFinal = screen.getPullDownData("PlatyHideToEra", screen.getSelectedPullDownID("PlatyHideToEra"))
				self.fullRefresh(screen)

	def back(self):
		if self.iCurrentEra is None:
			self.iEraFirst = self.iEraFinal = self.iCurrentEra = self.iCurrentEra0
			self.fullRefresh(self.screen())
		elif self.iCurrentEra:
			self.iCurrentEra -= 1
			self.iEraFirst = self.iEraFinal = self.iCurrentEra
			self.fullRefresh(self.screen())

	def forward(self):
		if self.iCurrentEra is None:
			self.iEraFirst = self.iEraFinal = self.iCurrentEra = self.iCurrentEra0
			self.fullRefresh(self.screen())
		elif self.iCurrentEra + 1 != self.iNumEras:
			self.iCurrentEra += 1
			self.iEraFirst = self.iEraFinal = self.iCurrentEra
			self.fullRefresh(self.screen())

	def onClose(self):
		print "Exit Tech Tree"
		if GC.getPlayer(GC.getGame().getActivePlayer()).getAdvancedStartPoints() > -1:
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, True)
		del (
			self.screenId, self.InputData, self.szTxtTT, self.iOffsetTT, self.bLockedTT, self.iUnitTT, self.bUnitTT,
			self.xRes, self.yRes, self.aFontList, self.wCell, self.hCell, self.sIcon0, self.sIcon1, self.iSelectedTech,
			self.iPlayer, self.CyPlayer, self.CyTeam, self.iResearch0, self.currentTechState, self.iNumTechs, self.iNumEras,
			self.iEraFirst, self.iEraFinal, self.iCurrentEra, self.iCurrentEra0
		)