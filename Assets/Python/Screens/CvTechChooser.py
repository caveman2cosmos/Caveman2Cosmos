from CvPythonExtensions import *
import CvScreensInterface as UP
import CvScreenEnums
import HandleInputUtil
import PythonToolTip as pyTT

GC = CyGlobalContext()
TRNSLTR = CyTranslator()
AFM = CyArtFileMgr()
GAME = GC.getGame()

## Adjustable Values ##
TEXTURE_SIZE = 28		## Icon Size of Items in Tech Panel
BOX_Y_SPACING = -10		## Min Vertical Panel Spacing

## Non Adjustable Values ##
X_INCREMENT = TEXTURE_SIZE + 2

BOX_HEIGHT = TEXTURE_SIZE * 3
TECH_ICON = BOX_HEIGHT - 15
BOX_X_SPACING = 64

class CvTechChooser:

	def __init__(self):
		# Advanced Start
		self.m_iSelectedTech = -1
		self.m_bSelectedTechDirty = False
		self.m_bTechRecordsDirty = False

		# Cache minimum X coordinate per era for era partitioning.
		minEraX = [""] * GC.getNumEraInfos() # (string > integer) is True
		for iTech in xrange(GC.getNumTechInfos()):
			info = GC.getTechInfo(iTech)
			iX = info.getGridX()
			if iX > 0:
				iEra = info.getEra()
				if minEraX[iEra] > iX:
					minEraX[iEra] = iX
		self.minEraX = minEraX

	def screen(self):
		return CyGInterfaceScreen("TechChooser", self.screenId)

	def interfaceScreen(self, screenId):
		if GAME.isPitbossHost(): return
		self.screenId = screenId

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

		self.wCell = 128 + self.xRes / 5

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		self.iNumTechs = GC.getNumTechInfos()
		self.iNumEras = GC.getNumEraInfos()
		self.iPlayer = iPlayer = GAME.getActivePlayer()
		self.CyPlayer = GC.getPlayer(iPlayer)
		self.CyTeam = GC.getTeam(self.CyPlayer.getTeam())
		self.iEraFirst = self.iEraFinal = self.iCurrentEra = self.iCurrentEra0 = self.CyPlayer.getCurrentEra()
		self.aiCurrentState = [-1] * self.iNumTechs
		self.updateBenefits()
		self.Advisors = ["[ICON_STRENGTH]", "[ICON_RELIGION]", "[ICON_GOLD]", "[ICON_RESEARCH]", "[ICON_CULTURE]", "[ICON_FOOD]"]

		self.iResearch0 = -1

		self.colorNoResearch	= [222,  30,  30]
		self.colorHasTech		= [222, 222, 222]
		self.colorResearching	= [  0, 255,   0]

		self.colorArchaicTech	= [128,  64,   0]
		self.colorCoevalTech	= [ 64,  92, 164]
		self.colorNeoTech		= [  0, 255, 255]

		# Base Screen
		screen = self.screen()

		screen.addDDSGFC("ScreenBackground", AFM.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, xRes, yRes, eWidGen, 1, 2)

		screen.addPanel("TC_BarTop", "", "", True, False, 0, 0, xRes, 42, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("TC_BarBot", "", "", True, False, 0, yRes - 42, xRes, 42, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.setLabel("TC_Header", "",  "<font=4b>" + TRNSLTR.getText("TXT_KEY_TECH_CHOOSER_TITLE", ()), 1<<2, xRes/2, 4, 0, eFontTitle, eWidGen, 1, 2)

		screen.setText("TC_Exit", "", "<font=4b>" + TRNSLTR.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()), 1<<1, xRes - 8, 2, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		stackBar = "progressBar"
		screen.addStackedBarGFC(stackBar, 256, 2, self.xRes - 512, 32, InfoBarTypes.NUM_INFOBAR_TYPES, eWidGen, 1, 2)
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_STORED, GC.getInfoTypeForString("COLOR_RESEARCH_STORED"))
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_RATE, GC.getInfoTypeForString("COLOR_RESEARCH_RATE"))
		screen.setStackedBarColors(stackBar, InfoBarTypes.INFOBAR_EMPTY, GC.getInfoTypeForString("COLOR_EMPTY"))
		screen.setImageButton("WID|TECH|CURRENT0", "", 256, 3, xRes - 512, 30, eWidGen, 0, 0)

		# Debug
		import DebugUtils
		if DebugUtils.bDebugMode:
			DDB = "TC_DebugDD"
			screen.addDropDownBoxGFC(DDB, 4, 2, 200, eWidGen, 1, 2, eFontTitle)
			for iPlayerX in range(GC.getMAX_PLAYERS()):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					screen.addPullDownString(DDB, CyPlayerX.getName(), iPlayerX, iPlayerX, iPlayer == iPlayerX)

		screen.setRenderInterfaceOnly(True)
		screen.showWindowBackground(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		self.fullRefresh(screen)

	def fullRefresh(self, screen):
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
			screen.setText("ERA", "", "<font=4b>" + GC.getEraInfo(self.iCurrentEra).getDescription(), 1<<0, 8, yRes-32, 0, eFontTitle, eWidGen, 1, 2)
		else:
			self.iCurrentEra = None
			bSingleEra = False
			screen.setText("ERA", "", "<font=4b>" + GC.getEraInfo(self.iCurrentEra0).getDescription(), 1<<2, xRes/2, yRes-32, 0, eFontTitle, eWidGen, 1, 2)

		if bSingleEra and iCurrentEra:
			screen.setButtonGFC("ERA|Prev", "", "", xRes/2 - 32, yRes - 30, 24, 24, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_ARROW_LEFT)
		else: screen.hide("ERA|Prev")

		if bSingleEra and iCurrentEra + 1 != iNumEras:
			screen.setButtonGFC("ERA|Next", "", "", xRes/2 + 8, yRes - 30, 24, 24, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_ARROW_RIGHT)
		else: screen.hide("ERA|Next")


		# Filter
		screen.addDropDownBoxGFC("PlatyHideToEra", xRes - 132, yRes - 33, 128, eWidGen, 1, 2, eFontTitle)
		screen.addDropDownBoxGFC("PlatyHideFromEra", xRes - 268, yRes - 33, 128, eWidGen, 1, 2, eFontTitle)

		for iEra in xrange(iNumEras):
			if iEra <= iEraFinal:
				screen.addPullDownString("PlatyHideFromEra", GC.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == iEraFirst)
			if iEra >= iEraFirst:
				screen.addPullDownString("PlatyHideToEra", GC.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == iEraFinal)

		if CyPlayer.getAdvancedStartPoints() > -1:
			self.m_bSelectedTechDirty = True
			screen.setButtonGFC("AddTechButton", TRNSLTR.getText("TXT_KEY_WB_AS_ADD_TECH", ()), "", 8, yRes - 38, 150, 30, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_STANDARD)

		ScrPnl = "TechList"
		iHeight = yRes - 85
		screen.addScrollPanel(ScrPnl, "", -8, 29, xRes + 16, iHeight, PanelStyles.PANEL_STYLE_EXTERNAL)

		global BOX_Y_SPACING
		iEmptySpace = iHeight - 12 - 10 * BOX_HEIGHT
		BOX_Y_SPACING = max(BOX_Y_SPACING, iEmptySpace/9)

		# Place techs
		NUM_AND_TECH_PREREQS = GC.getNUM_AND_TECH_PREREQS()
		NUM_OR_TECH_PREREQS = GC.getNUM_OR_TECH_PREREQS()
		RED_X = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_RED_X").getPath()
		ARROW_X = AFM.getInterfaceArtInfo("ARROW_X").getPath()
		ARROW_Y = AFM.getInterfaceArtInfo("ARROW_Y").getPath()
		ARROW_HEAD = AFM.getInterfaceArtInfo("ARROW_HEAD").getPath()

		wCell = self.wCell
		ARROW = "Arrow"
		yArrow0 = BOX_HEIGHT / 2
		yArrow1 = BOX_HEIGHT*3/8
		yArrow2 = BOX_HEIGHT / 4
		yArrow3 = BOX_HEIGHT*5/8
		yArrow4 = BOX_HEIGHT*3/4
		xReq = wCell - 10

		yBoxSpacing = (BOX_Y_SPACING + BOX_HEIGHT)/2
		iCount = 0
		iMaxElements = (wCell - TECH_ICON - 6) / X_INCREMENT
		for iTech in xrange(iNumTechs):

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
			iX = (x0 - iMinX) * (BOX_X_SPACING + wCell)
			iY = (y0 - 1) * (BOX_Y_SPACING + BOX_HEIGHT)/2
			szTechRecord = "WID|TECH|CHOICE" + szTech

			screen.attachPanelAt(ScrPnl, "CELL" + szTech, "", "", True, False, PanelStyles.PANEL_STYLE_TECH, iX,  iY, wCell, BOX_HEIGHT, eWidGen, 0, 0)
			screen.setImageButtonAt(szTechRecord, ScrPnl, "", iX + 2, iY + 2, wCell - 8, BOX_HEIGHT - 8, eWidGen, 1, 2)

			if CyTeam.isHasTech(iTech):
				self.aiCurrentState[iTech] = "CIV_HAS_TECH"
			elif CyPlayer.isResearchingTech(iTech):
				self.aiCurrentState[iTech] = "CIV_IS_RESEARCHING"
			elif CyPlayer.canEverResearch(iTech):
				self.aiCurrentState[iTech] = "CIV_TECH_AVAILABLE"
			else:
				self.aiCurrentState[iTech] = "CIV_NO_RESEARCH"

			temp = "TechButtonID" + szTech
			screen.addDDSGFCAt(temp, szTechRecord, CvTechInfo.getButton(), 6, 6, TECH_ICON, TECH_ICON, eWidGen, 1, 2, False)
			screen.setHitTest(temp, HitTestTypes.HITTEST_NOHIT)

			# Requires
			iX = xReq
			for i in xrange(NUM_AND_TECH_PREREQS):
				iTechX = CvTechInfo.getPrereqAndTechs(i)
				if iTechX == -1: break
				iX -= X_INCREMENT
				screen.addDDSGFCAt(temp + str(i) + temp, szTechRecord, GC.getTechInfo(iTechX).getButton(), iX, 6, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TECH_PREPREQ, iTechX, -1, False)

			for i in xrange(NUM_OR_TECH_PREREQS):
				iTechX = CvTechInfo.getPrereqOrTechs(i)
				if iTechX == -1: break
				techInfoX = GC.getTechInfo(iTechX)

				iEra = techInfoX.getEra()
				if iEra < iEraFirst or iEra > iEraFinal:
					continue

				x1 = techInfoX.getGridX()
				y1 = techInfoX.getGridY()
				iX = (x1 - iMinX) * (BOX_X_SPACING + wCell) + wCell - 6
				iY = (y1 - 1) * yBoxSpacing - 6

				xDiff = x0 - x1
				yDiff = y0 - y1
				dx = xDiff * BOX_X_SPACING + (xDiff - 1) * wCell

				if not yDiff:
					screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX, iY + yArrow0, dx, 8, eWidGen, 1, 2, False)
					screen.addDDSGFCAt("", ScrPnl, ARROW_HEAD, iX + dx, iY + yArrow0, 8, 8, eWidGen, 1, 2, False)
				elif yDiff < 0:
					if yDiff < -3 and xDiff == 1:
						dy = yDiff * yBoxSpacing + BOX_HEIGHT/2
						yArrow = iY + yArrow2
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX, yArrow, dx/3 + 4, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_Y, iX + dx/3, yArrow + 4 + dy, 8, -dy, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX + 4 + dx/3, yArrow + dy, dx * 2/3, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_HEAD, iX + dx, yArrow + dy, 8, 8, eWidGen, 1, 2, False)
					else:
						dy = yDiff * yBoxSpacing + BOX_HEIGHT/4
						yArrow = iY + yArrow1
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX, yArrow, dx/2 + 4, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_Y, iX + dx/2, yArrow + 4 + dy, 8, -dy, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX + 4 + dx/2, yArrow + dy, dx/2, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_HEAD, iX + dx, yArrow + dy, 8, 8, eWidGen, 1, 2, False)
				else:
					if yDiff > 3 and xDiff == 1:
						dy = yDiff * yBoxSpacing - BOX_HEIGHT/2
						yArrow = iY + yArrow4
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX, yArrow, dx/3 + 4, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_Y, iX + dx/3, yArrow + 4, 8, dy, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX + 4 + dx/3, yArrow + dy, dx * 2/3, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_HEAD, iX + dx, yArrow + dy, 8, 8, eWidGen, 1, 2, False)
					else:
						dy = yDiff * yBoxSpacing - BOX_HEIGHT/4
						yArrow = iY + yArrow3
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX, yArrow, dx/2 + 4, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_Y, iX + dx/2, yArrow + 4, 8, dy, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_X, iX + 4 + dx/2, yArrow + dy, dx/2, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt("", ScrPnl, ARROW_HEAD, iX + dx, yArrow + dy, 8, 8, eWidGen, 1, 2, False)

			# Unlocks
			iX = TECH_ICON + 6
			iY = 40

			iLength = len(self.TechBenefits[iTech])
			if iLength > iMaxElements:
				iLength = iMaxElements

			for i in xrange(iLength):
				sType = self.TechBenefits[iTech][i][0]
				iItem = self.TechBenefits[iTech][i][1]
				if sType == "UnlockUnit":
					screen.setImageButtonAt("WID|UNIT" + str(iItem) + '|' + str(iCount), szTechRecord, GC.getUnitInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, eWidGen, 1, 2)
					iCount += 1
				elif sType == "UnlockBuilding":
					screen.setImageButtonAt("WID|BUILDING" + str(iItem) + '|' + str(iCount), szTechRecord, GC.getBuildingInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, eWidGen, 1, 2)
					iCount += 1
				elif sType == "ObsoleteBuilding":
					screen.addDDSGFCAt("", szTechRecord, GC.getBuildingInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, eWidGen, 1, 2, False)
					screen.setImageButtonAt("WID|BUILDING|OBS" + str(iItem), szTechRecord, RED_X, iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, eWidGen, 1, 2)
				elif sType == "UnlockSpecialBuilding":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getSpecialBuildingInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_SPECIAL_BUILDING, iTech, iItem, False)
				elif sType == "ObsoleteSpecialBuilding":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getSpecialBuildingInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_SPECIAL, iTech, iItem, False)
					screen.addDDSGFCAt("Obsolete" + str(iTech * 1000 + i), szTechRecord, RED_X, iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_SPECIAL, iItem, -1, False)
				elif sType == "RevealBonus":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getBonusInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_BONUS_REVEAL, iTech, iItem, False)
				elif sType == "ObsoleteBonus":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getBonusInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_BONUS, iTech, iItem, False)
					screen.addDDSGFCAt("Obsolete" + str(iTech * 1000 + i), szTechRecord, RED_X, iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_BONUS, iItem, -1, False)
				elif sType == "RouteChange":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MOVE_BONUS").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MOVE_BONUS, iTech, -1, False)
				elif sType == "UnlockPromotion":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getPromotionInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROMOTION, iItem, -1, False)
				elif sType == "FreeUnit":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getUnitInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FREE_UNIT, iItem, iTech, False)
				elif sType == "FeatureProduction":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_FEATURE_PRODUCTION").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FEATURE_PRODUCTION, iTech, -1, False)
				elif sType == "WorkerSpeed":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WORKER_SPEED").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_WORKER_RATE, iTech, -1, False)
				elif sType == "TradeRoute":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_TRADE_ROUTES").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TRADE_ROUTES, iTech, -1, False)
				elif sType == "Health":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_HEALTH").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_HEALTH_RATE, iTech, -1, False)
				elif sType == "Happiness":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_HAPPINESS").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_HAPPINESS_RATE, iTech, -1, False)
				elif sType == "Population":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_POPULATION").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_HAPPINESS_RATE, iTech, -1, False)
				elif sType == "FreeTech":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_FREETECH").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FREE_TECH, iTech, -1, False)
				elif sType == "WaterSight":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_LOS").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_LOS_BONUS, iTech, -1, False)
				elif sType == "MapCentering":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPCENTER").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MAP_CENTER, iTech, -1, False)
				elif sType == "MapVisible":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPREVEAL").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MAP_REVEAL, iTech, -1, False)
				elif sType == "MapTrading":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPTRADING").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MAP_TRADE, iTech, -1, False)
				elif sType == "TechTrading":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_TECHTRADING").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TECH_TRADE, iTech, -1, False)
				elif sType == "GoldTrading":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_GOLDTRADING").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_GOLD_TRADE, iTech, -1, False)
				elif sType == "OpenBorders":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_OPENBORDERS").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OPEN_BORDERS, iTech, -1, False)
				elif sType == "DefensivePact":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_DEFENSIVEPACT").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_DEFENSIVE_PACT, iTech, -1, False)
				elif sType == "PermanentAlliance":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_PERMALLIANCE").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_PERMANENT_ALLIANCE, iTech, -1, False)
				elif sType == "VassalState":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_VASSAL").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_VASSAL_STATE, iTech, -1, False)
				elif sType == "BridgeBuilding":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_BRIDGEBUILDING").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_BUILD_BRIDGE, iTech, -1, False)
				elif sType == "EnablesIrrigation":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_IRRIGATION").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_IRRIGATION, iTech, -1, False)
				elif sType == "IgnoreIrrigation":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_NOIRRIGATION").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_IGNORE_IRRIGATION, iTech, -1, False)
				elif sType == "WaterWork":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERWORK").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_WATER_WORK, iTech, -1, False)
				elif sType == "UnlockImprovement":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getBuildInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_IMPROVEMENT, iTech, iItem, False)
				elif sType == "DomainMoves":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERMOVES").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_DOMAIN_EXTRA_MOVES, iTech, iItem, False)
				elif sType == "CommerceFlexible":
					if iItem == CommerceTypes.COMMERCE_CULTURE:
						temp = AFM.getInterfaceArtInfo("INTERFACE_TECH_CULTURE").getPath()
					elif iItem == CommerceTypes.COMMERCE_ESPIONAGE:
						temp = AFM.getInterfaceArtInfo("INTERFACE_TECH_ESPIONAGE").getPath()
					else: temp = AFM.getInterfaceArtInfo("INTERFACE_GENERAL_QUESTIONMARK").getPath()
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, temp, iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_ADJUST, iTech, iItem, False)
				elif sType == "TerrainTrade":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERTRADE").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TERRAIN_TRADE, iTech, iItem, False)
				elif sType == "RiverTrade":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_RIVERTRADE").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TERRAIN_TRADE, iTech, GC.getNumTerrainInfos(), False)
				elif sType == "ImprovementYield":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getImprovementInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_YIELD_CHANGE, iTech, iItem, False)
				elif sType == "UnlockCivic":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getCivicInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_CIVIC_REVEAL, iTech, iItem, False)
				elif sType == "UnlockProject":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getProjectInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT, iItem, 1, False)
				elif sType == "UnlockProcess":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getProcessInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_PROCESS_INFO, iTech, iItem, False)
				elif sType == "UnlockReligion":
					if GAME.isOption(GameOptionTypes.GAMEOPTION_PICK_RELIGION):
						szButton = AFM.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_RELIGION").getPath()
					else:
						szButton = GC.getReligionInfo(iItem).getButton()
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, szButton, iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FOUND_RELIGION, iTech, iItem, False)
				elif sType == "UnlockCorporation":
					screen.addDDSGFCAt("Item" + str(iTech * 1000 + i), szTechRecord, GC.getCorporationInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FOUND_CORPORATION, iTech, iItem, False)
				iX += X_INCREMENT

		screen.setFocus(ScrPnl)
		self.updateTechRecords(True)


	def updateBenefits(self):
		TechBenefits = {}
		iNumTechs = self.iNumTechs
		for iTech in xrange(iNumTechs):
			TechBenefits[iTech] = []
			CvTechInfo = GC.getTechInfo(iTech)
			if CvTechInfo.getGridX() < 1: continue

			for j in xrange(GC.getNumRouteInfos()):
				if GC.getRouteInfo(j).getTechMovementChange(iTech) != 0:
					TechBenefits[iTech].append(["RouteChange", j])
			iType = CvTechInfo.getFirstFreeUnitClass()
			if iType > -1:
				iType = GC.getUnitClassInfo(iType).getDefaultUnitIndex()
				if iType > -1:
					TechBenefits[iTech].append(["FreeUnit", iType])
			if CvTechInfo.getTradeRoutes():
				TechBenefits[iTech].append(["TradeRoute", -1])
			if CvTechInfo.getHealth():
				TechBenefits[iTech].append(["Health", -1])
			if CvTechInfo.getHappiness():
				TechBenefits[iTech].append(["Happiness", -1])
			if CvTechInfo.getFirstFreeTechs():
				TechBenefits[iTech].append(["FreeTech", -1])
			if CvTechInfo.isExtraWaterSeeFrom():
				TechBenefits[iTech].append(["WaterSight", -1])
			if CvTechInfo.isMapCentering():
				TechBenefits[iTech].append(["MapCentering", -1])
			if CvTechInfo.isMapVisible():
				TechBenefits[iTech].append(["MapVisible", -1])
			if CvTechInfo.isMapTrading():
				TechBenefits[iTech].append(["MapTrading", -1])
			if CvTechInfo.isTechTrading():
				TechBenefits[iTech].append(["TechTrading", -1])
			if CvTechInfo.isGoldTrading():
				TechBenefits[iTech].append(["GoldTrading", -1])
			if CvTechInfo.isOpenBordersTrading():
				TechBenefits[iTech].append(["OpenBorders", -1])
			if CvTechInfo.isDefensivePactTrading():
				TechBenefits[iTech].append(["DefensivePact", -1])
			if CvTechInfo.isPermanentAllianceTrading():
				TechBenefits[iTech].append(["PermanentAlliance", -1])
			if CvTechInfo.isVassalStateTrading():
				TechBenefits[iTech].append(["VassalState", -1])
			if CvTechInfo.isIrrigation():
				TechBenefits[iTech].append(["EnablesIrrigation", -1])
			if CvTechInfo.isIgnoreIrrigation():
				TechBenefits[iTech].append(["IgnoreIrrigation", -1])
			if CvTechInfo.isWaterWork():
				TechBenefits[iTech].append(["WaterWork", -1])
			for j in xrange(DomainTypes.NUM_DOMAIN_TYPES):
				if CvTechInfo.getDomainExtraMoves(j):
					TechBenefits[iTech].append(["DomainMoves", j])
			for j in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvTechInfo.isCommerceFlexible(j):
					TechBenefits[iTech].append(["CommerceFlexible", j])
			for j in xrange(GC.getNumTerrainInfos()):
				if CvTechInfo.isTerrainTrade(j):
					TechBenefits[iTech].append(["TerrainTrade", j])
			if CvTechInfo.isRiverTrade():
				TechBenefits[iTech].append(["RiverTrade", -1])
			for j in xrange(GC.getNumImprovementInfos()):
				for k in xrange(YieldTypes.NUM_YIELD_TYPES):
					if GC.getImprovementInfo(j).getTechYieldChanges(iTech, k):
						TechBenefits[iTech].append(["ImprovementYield", j])
			if CvTechInfo.getFeatureProductionModifier():
				TechBenefits[iTech].append(["FeatureProduction", -1])
			if CvTechInfo.getWorkerSpeedModifier():
				TechBenefits[iTech].append(["WorkerSpeed", -1])

		for iType in xrange(GC.getNumUnitInfos()):
			iTech = GC.getUnitInfo(iType).getPrereqAndTech()
			if iTech > -1:
				TechBenefits[iTech].append(["UnlockUnit", iType])

		for iType in xrange(GC.getNumBuildingInfos()):
			info = GC.getBuildingInfo(iType)
			iTech = info.getPrereqAndTech()
			if iTech > -1:
				TechBenefits[iTech].append(["UnlockBuilding", iType])
			iTech = info.getObsoleteTech()
			if iTech > -1:
				TechBenefits[iTech].append(["ObsoleteBuilding", iType])

		for iType in xrange(GC.getNumSpecialBuildingInfos()):
			info = GC.getSpecialBuildingInfo(iType)
			iTech = info.getTechPrereq()
			if iTech > -1:
				TechBenefits[iTech].append(["UnlockSpecialBuilding", iType])
			iTech = info.getObsoleteTech()
			if iTech > -1:
				TechBenefits[iTech].append(["ObsoleteSpecialBuilding", iType])

		for iType in xrange(GC.getNumBonusInfos()):
			info = GC.getBonusInfo(iType)
			iTech = info.getTechReveal()
			if iTech > -1:
				TechBenefits[iTech].append(["RevealBonus", iType])
			iTech = info.getTechObsolete()
			if iTech > -1:
				TechBenefits[iTech].append(["ObsoleteBonus", iType])

		for j in xrange(GC.getNumPromotionInfos()):
			iTech = GC.getPromotionInfo(j).getTechPrereq()
			if iTech > -1:
				TechBenefits[iTech].append(["UnlockPromotion", j])

		iFeatures = GC.getNumFeatureInfos()
		for iType in xrange(GC.getNumBuildInfos()):
			info = GC.getBuildInfo(iType)
			iTech = info.getTechPrereq()
			if iTech > -1:
				TechBenefits[iTech].append(["UnlockImprovement", iType])
				continue

			for j in xrange(iFeatures):
				iTech = info.getFeatureTech(j)
				if iTech > -1:
					TechBenefits[iTech].append(["UnlockImprovement", iType])

		for j in xrange(GC.getNumCivicInfos()):
			iTech = GC.getCivicInfo(j).getTechPrereq()
			if iTech > -1:
				TechBenefits[iTech].append(["UnlockCivic", j])

		for j in xrange(GC.getNumProjectInfos()):
			iTech = GC.getProjectInfo(j).getTechPrereq()
			if iTech > -1:
				TechBenefits[iTech].append(["UnlockProject", j])

		for j in xrange(GC.getNumProcessInfos()):
			iTech = GC.getProcessInfo(j).getTechPrereq()
			if iTech > -1:
				TechBenefits[iTech].append(["UnlockProcess", j])

		for j in xrange(GC.getNumReligionInfos()):
			iTech = GC.getReligionInfo(j).getTechPrereq()
			if iTech > -1:
				TechBenefits[iTech].append(["UnlockReligion", j])

		for j in xrange(GC.getNumCorporationInfos()):
			iTech = GC.getCorporationInfo(j).getTechPrereq()
			if iTech > -1:
				TechBenefits[iTech].append(["UnlockCorporation", j])
		self.TechBenefits = TechBenefits

	def updateTechRecords(self, bForce):
		if GAME.isPitbossHost(): return

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT
		screen = self.screen()
		lChanged = []
		CyPlayer = self.CyPlayer
		CyTeam = self.CyTeam
		iEraFirst = self.iEraFirst
		iEraFinal = self.iEraFinal

		# Progress Bar
		iTech = CyPlayer.getCurrentResearch()
		if self.iResearch0 != iTech:
			if iTech > -1:
				screen.hide("TC_Header")
				iProgress = CyTeam.getResearchProgress(iTech)
				iCost = CyTeam.getResearchCost(iTech)
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

		# Analyze change
		for iTech in xrange(self.iNumTechs):

			CvTechInfo = GC.getTechInfo(iTech)
			iX = CvTechInfo.getGridX()
			if iX < 1: continue

			iEra = CvTechInfo.getEra()
			if bSingleEra:
				if iEra != iCurrentEra and iX != xNext:
					continue

			elif iEra < iEraFirst or iEra > iEraFinal:
				continue

			if CyTeam.isHasTech(iTech):
				if self.aiCurrentState[iTech] != "CIV_HAS_TECH":
					self.aiCurrentState[iTech] = "CIV_HAS_TECH"
					lChanged.append((iTech, CvTechInfo, iEra))
				elif bForce:
					lChanged.append((iTech, CvTechInfo, iEra))

			elif CyPlayer.isResearchingTech(iTech):
				self.aiCurrentState[iTech] = "CIV_IS_RESEARCHING"
				lChanged.append((iTech, CvTechInfo, iEra))

			elif CyPlayer.canEverResearch(iTech):
				if self.aiCurrentState[iTech] != "CIV_TECH_AVAILABLE":
					self.aiCurrentState[iTech] = "CIV_TECH_AVAILABLE"
					lChanged.append((iTech, CvTechInfo, iEra))
				elif bForce:
					lChanged.append((iTech, CvTechInfo, iEra))

			elif self.aiCurrentState[iTech] != "CIV_NO_RESEARCH":
				self.aiCurrentState[iTech] = "CIV_NO_RESEARCH"
				lChanged.append((iTech, CvTechInfo, iEra))
			elif bForce:
				lChanged.append((iTech, CvTechInfo, iEra))

		# Make change
		if lChanged:
			iCurrentEra0 = self.iCurrentEra0
			iMinX = self.minEraX[iEraFirst]
			wCell = self.wCell
			while lChanged:
				iTech, CvTechInfo, iEra = lChanged.pop()
				szTech = str(iTech)

				szState = self.aiCurrentState[iTech]

				szTechID = "TechID" + szTech
				szTechString = self.aFontList[5]
				iAdvisor = CvTechInfo.getAdvisorType()
				if iAdvisor > -1:
					szTechString += TRNSLTR.getText(self.Advisors[iAdvisor], ())
				if szState == "CIV_IS_RESEARCHING":
					szTechString += str(CyPlayer.getQueuePosition(iTech)) + ") "
				szTechString += CvTechInfo.getDescription()
				screen.setLabelAt(szTechID, "WID|TECH|CHOICE" + szTech, szTechString, 1<<0, TECH_ICON + 6, 6, 0, eFontTitle, eWidGen, 1, 2)
				screen.setHitTest(szTechID, HitTestTypes.HITTEST_NOHIT)

				# Colours
				if szState == "CIV_HAS_TECH":
					screen.setPanelColor("CELL" + szTech, self.colorHasTech[0], self.colorHasTech[1], self.colorHasTech[2])

				elif szState == "CIV_IS_RESEARCHING":
					screen.setPanelColor("CELL" + szTech, self.colorResearching[0], self.colorResearching[1], self.colorResearching[2])

				elif szState == "CIV_TECH_AVAILABLE":
					if iEra > iCurrentEra0:
						r, g, b = self.colorNeoTech
					elif iEra < iCurrentEra0:
						r, g, b = self.colorArchaicTech
					else:
						r, g, b = self.colorCoevalTech
					screen.setPanelColor("CELL" + szTech, r, g, b)
				else:
					screen.setPanelColor("CELL" + szTech, self.colorNoResearch[0], self.colorNoResearch[1], self.colorNoResearch[2])

	def updateSelectedTech(self):
		screen = self.screen()
		CyPlayer = GC.getPlayer(GAME.getActivePlayer())

		szName = ""
		iCost = 0
		if self.m_iSelectedTech > -1:
			szName = GC.getTechInfo(self.m_iSelectedTech).getDescription()
			iCost = CyPlayer.getAdvancedStartTechCost(self.m_iSelectedTech, True)

		screen.hide("ASPointsLabel")
		screen.hide("AddTechButton")
		if iCost > 0:
			iPoints = CyPlayer.getAdvancedStartPoints()
			screen.setLabel("ASPointsLabel", "", "<font=4>" + TRNSLTR.getText("TXT_KEY_WB_AS_SELECTED_TECH_COST", (iCost, iPoints)), 1<<0, 180, self.yRes - 42, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
			if iPoints >= iCost:
				screen.show("AddTechButton")

		szText = "<font=4>" + szName + " (" + str(iCost) + ')'
		screen.setLabel("SelectedTechLabel", "", szText, 1<<2, self.xRes / 2, self.yRes - 42, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)

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
		screen = self.screen()
		if CyInterface().isDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT):
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, False)

			if self.m_bSelectedTechDirty:
				self.m_bSelectedTechDirty = False
				self.updateSelectedTech()

			if self.m_bTechRecordsDirty:
				self.m_bTechRecordsDirty = False
				self.updateTechRecords(True)

			if GC.getPlayer(GAME.getActivePlayer()).getAdvancedStartPoints() < 0:
				screen.hide("AddTechButton")
				screen.hide("ASPointsLabel")
				screen.hide("SelectedTechLabel")

		if self.bLockedTT:
			POINT = GC.getCursorPos()
			iX = POINT.x + self.iOffsetTT[0]
			iY = POINT.y + self.iOffsetTT[1]
			if iX < 0: iX = 0
			if iY < 0: iY = 0
			screen.moveItem("Tooltip", iX, iY, 0)

	def handleInput(self, inputClass):
		screen = self.screen()
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
			CASE = [""]

		iType = ID

		if iCode == 6: # Character
			if iData in (45, 49, 56): # Ctrl, Shift, Alt
				if self.bUnitTT:
					self.updateTooltip(screen, CyGameTextMgr().getUnitHelp(self.iUnitTT, False, True, True, None))
					self.bUnitTT = None
			return 1
		elif iCode == 17: # Key Up
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
					szTxt = ""
					if CASE[0] == "CURRENT":
						szTxt += "Researching: "
						iType = self.CyPlayer.getCurrentResearch()
					szTxt += CyGameTextMgr().getTechHelp(iType, False, True, True, True, -1)
					self.updateTooltip(screen, szTxt)
				elif TYPE == "UNIT":
					self.updateTooltip(screen, CyGameTextMgr().getUnitHelp(iType, False, True, True, None))
					self.iUnitTT = iType
					self.bUnitTT = True
				elif TYPE == "BUILDING":
					if CASE[0] == "OBS":
						CvBuildingInfo = GC.getBuildingInfo(ID)
						szTxt = TRNSLTR.getText("TXT_KEY_TECH_OBSOLETES", (CvBuildingInfo.getType(), CvBuildingInfo.getTextKey()))
					else: szTxt = CyGameTextMgr().getBuildingHelp(ID, False, False, True, None, False)
					self.updateTooltip(screen, szTxt)

		elif not iCode: # click
			CyPlayer = self.CyPlayer
			if CyPlayer.getAdvancedStartPoints() > -1:
				if NAME == "AddTechButton":
					if CyPlayer.getAdvancedStartTechCost(self.m_iSelectedTech, True) > -1:
						CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_TECH, self.iPlayer, -1, -1, self.m_iSelectedTech, True)	#Action, Player, X, Y, Data, bAdd
						self.m_bTechRecordsDirty = True
						self.m_bSelectedTechDirty = True

				elif iBtn == WidgetTypes.WIDGET_TECH_TREE:
					self.m_iSelectedTech = iData1
					self.updateSelectedTech()
				return

			if BASE == "WID":

				if szFlag == "MOUSE_RBUTTONUP":
					if TYPE == "UNIT":
						UP.pediaJumpToUnit([iType])
					elif TYPE == "BUILDING":
						UP.pediaJumpToBuilding([iType])
					elif TYPE == "PROJECT":
						UP.pediaJumpToProject([iType])
					elif TYPE == "PROMO":
						UP.pediaJumpToPromotion([iType])
					elif TYPE == "TECH":
						if CASE[0] == "CURRENT":
							iType = self.CyPlayer.getCurrentResearch()
						UP.pediaJumpToTech([iType])

				elif szFlag == "MOUSE_LBUTTONUP":
					if TYPE == "TECH":
						if self.CyPlayer.canEverResearch(iType):
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
				else:
					self.iEraFirst = self.iEraFinal = self.iCurrentEra = self.iCurrentEra0
					self.fullRefresh(screen)

		elif iCode == 11: # List Select
			if NAME == "TC_DebugDD":
				iIndex = screen.getSelectedPullDownID(NAME)
				self.iPlayer = iPlayer = screen.getPullDownData(NAME, iIndex)
				self.CyPlayer = CyPlayer = GC.getPlayer(iPlayer)
				self.CyTeam = GC.getTeam(CyPlayer.getTeam())
				self.iEraFirst = self.iEraFinal = self.iCurrentEra = self.iCurrentEra0 = CyPlayer.getCurrentEra()
				self.fullRefresh(screen)

			elif NAME == "PlatyHideFromEra":
				iIndex = screen.getSelectedPullDownID("PlatyHideFromEra")
				self.iEraFirst = screen.getPullDownData("PlatyHideFromEra", iIndex)
				self.fullRefresh(screen)

			elif NAME == "PlatyHideToEra":
				iIndex = screen.getSelectedPullDownID("PlatyHideToEra")
				self.iEraFinal = screen.getPullDownData("PlatyHideToEra", iIndex)
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
		if GC.getPlayer(GAME.getActivePlayer()).getAdvancedStartPoints() > -1:
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, True)
		del (
			self.screenId, self.InputData, self.szTxtTT, self.iOffsetTT, self.bLockedTT, self.iUnitTT, self.bUnitTT,
			self.xRes, self.yRes, self.aFontList, self.wCell,
			self.colorNoResearch, self.colorHasTech, self.colorResearching, self.colorArchaicTech, self.colorCoevalTech, self.colorNeoTech,
			self.iEraFirst, self.iEraFinal, self.iCurrentEra, self.iCurrentEra0, self.aiCurrentState, self.TechBenefits, self.Advisors
		)