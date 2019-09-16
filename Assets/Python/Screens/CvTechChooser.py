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
TEXTURE_SIZE = 24		## Icon Size of Items in Tech Panel
BOX_Y_SPACING = -10		## Min Vertical Panel Spacing

## Non Adjustable Values ##
X_INCREMENT = TEXTURE_SIZE + 4

PIXEL_INCREMENT = 7
BOX_HEIGHT = TEXTURE_SIZE * 3
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

		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y
		self.aFontList = SR.aFontList

		self.wCell = 128 + self.xRes / 4

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		self.iNumTechs = GC.getNumTechInfos()
		self.iNumEras = GC.getNumEraInfos()
		self.iPlayer = iPlayer = GAME.getActivePlayer()
		self.CyPlayer = GC.getPlayer(iPlayer)
		self.CyTeam = GC.getTeam(self.CyPlayer.getTeam())
		self.iCurrentEra = self.CyPlayer.getCurrentEra()
		self.iEraFirst = self.iEraFinal = self.iCurrentEra
		self.aiCurrentState = [-1] * self.iNumTechs
		self.TechBenefits = {}
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

		stTxt = "<font=4b>" + TRNSLTR.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ())
		screen.setText("TC_Exit", "", stTxt, 1<<1, xRes - 8, 2, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

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
		self.nWidgetCount = 0
		iNumTechs = self.iNumTechs
		iNumEras = self.iNumEras
		iPlayer = self.iPlayer
		CyPlayer = self.CyPlayer
		CyTeam = self.CyTeam
		xRes = self.xRes
		yRes = self.yRes

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		if not self.TechBenefits:
			self.updateBenefits()

		# Filter
		screen.addDropDownBoxGFC("PlatyHideToEra", xRes - 132, yRes - 33, 128, eWidGen, 1, 2, eFontTitle)
		screen.setButtonGFC("RightArrow", "", "", xRes - 160, yRes - 30, 24, 24, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_ARROW_RIGHT)
		screen.addDropDownBoxGFC("PlatyHideFromEra", xRes - 292, yRes - 33, 128, eWidGen, 1, 2, eFontTitle)

		for iEra in xrange(self.iNumEras):
			if iEra <= self.iEraFinal:
				screen.addPullDownString("PlatyHideFromEra", GC.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == self.iEraFirst)
			if iEra >= self.iEraFirst:
				screen.addPullDownString("PlatyHideToEra", GC.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == self.iEraFinal)

		if self.CyPlayer.getAdvancedStartPoints() > -1:
			self.m_bSelectedTechDirty = True
			screen.setButtonGFC("AddTechButton", TRNSLTR.getText("TXT_KEY_WB_AS_ADD_TECH", ()), "", 8, yRes - 38, 150, 30, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_STANDARD)

		iHeight = yRes - 85
		screen.addScrollPanel("TechList", "", -8, 29, xRes + 16, iHeight, PanelStyles.PANEL_STYLE_EXTERNAL)

		global BOX_Y_SPACING
		iEmptySpace = iHeight - 12 - 10 * BOX_HEIGHT
		BOX_Y_SPACING = max(BOX_Y_SPACING, iEmptySpace/9)

		# Place techs
		ARROW_X = AFM.getInterfaceArtInfo("ARROW_X").getPath()
		ARROW_Y = AFM.getInterfaceArtInfo("ARROW_Y").getPath()
		ARROW_XY = AFM.getInterfaceArtInfo("ARROW_XY").getPath()
		ARROW_MXY = AFM.getInterfaceArtInfo("ARROW_MXY").getPath()
		ARROW_XMY = AFM.getInterfaceArtInfo("ARROW_XMY").getPath()
		ARROW_HEAD = AFM.getInterfaceArtInfo("ARROW_HEAD").getPath()

		iMinX = self.minEraX[self.iEraFirst]
		if self.iEraFirst == self.iEraFinal:
			bSingleEra = True
			temp = self.iCurrentEra + 1
			if temp < self.iNumEras:
				xNext = self.minEraX[temp]
			else: xNext = self.minEraX[self.iCurrentEra]
		else:
			bSingleEra = False

		iCount = 0
		wCell = self.wCell
		iMaxElements = (wCell - 68) / 28
		for iTech in xrange(iNumTechs):

			CvTechInfo = GC.getTechInfo(iTech)
			x0 = CvTechInfo.getGridX()
			if x0 < 1: continue

			iEra = CvTechInfo.getEra()
			if bSingleEra:
				if iEra != self.iCurrentEra and x0 != xNext:
					continue

			elif iEra < self.iEraFirst or iEra > self.iEraFinal:
				continue

			szTech = str(iTech)
			y0 = CvTechInfo.getGridY()
			iX = (x0 - iMinX) * (BOX_X_SPACING + wCell)
			iY = (y0 - 1) * (BOX_HEIGHT + BOX_Y_SPACING)/2
			szTechRecord = "WID|TECH|CHOICE" + szTech

			screen.attachPanelAt("TechList", szTechRecord, "", "", True, False, PanelStyles.PANEL_STYLE_TECH, iX,  iY, wCell, BOX_HEIGHT, eWidGen, 64512, iTech)

			if CyTeam.isHasTech(iTech):
				self.aiCurrentState[iTech] = "CIV_HAS_TECH"
			elif CyPlayer.isResearchingTech(iTech):
				self.aiCurrentState[iTech] = "CIV_IS_RESEARCHING"
			elif CyPlayer.canEverResearch(iTech):
				self.aiCurrentState[iTech] = "CIV_TECH_AVAILABLE"
			else:
				self.aiCurrentState[iTech] = "CIV_NO_RESEARCH"

			temp = "TechButtonID" + szTech
			screen.addDDSGFCAt(temp, szTechRecord, CvTechInfo.getButton(), 12, 12, TEXTURE_SIZE + X_INCREMENT, TEXTURE_SIZE + X_INCREMENT, eWidGen, 1, 2, False)
			screen.setHitTest(temp, HitTestTypes.HITTEST_NOHIT)

			# Requires
			iX = 6 + wCell - PIXEL_INCREMENT * 2
			for i in xrange(GC.getNUM_AND_TECH_PREREQS()):
				iTechX = CvTechInfo.getPrereqAndTechs(i)
				if iTechX == -1: break
				iX -= X_INCREMENT
				szTechPrereqID = "TechPrereqID" + str((iTech * 1000) + i)
				screen.addDDSGFCAt(szTechPrereqID, szTechRecord, GC.getTechInfo(iTechX).getButton(), iX, 10, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TECH_PREPREQ, iTechX, -1, False)

			for i in xrange(GC.getNUM_OR_TECH_PREREQS()):
				iTechX = CvTechInfo.getPrereqOrTechs(i)
				if iTechX == -1: break
				techInfoX = GC.getTechInfo(iTechX)

				iEra = techInfoX.getEra()
				if iEra < self.iEraFirst or iEra > self.iEraFinal:
					continue

				x1 = techInfoX.getGridX()
				y1 = techInfoX.getGridY()
				iX = (x1 - iMinX) * (BOX_X_SPACING + wCell) + wCell - 6
				iY = (y1 - 1) * (BOX_HEIGHT + BOX_Y_SPACING)/2 - 6

				xDiff = x0 - x1
				yDiff = y0 - y1

				if yDiff == 0:
					screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_X, iX, iY + self.getYStart(4), self.getWidth(xDiff), 8, eWidGen, 1, 2, False)
					screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_HEAD, iX + self.getWidth(xDiff), iY + self.getYStart(4), 8, 8, eWidGen, 1, 2, False)
				elif yDiff < 0:
					if yDiff < -3 and xDiff == 1:
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_X, iX, iY + self.getYStart(2), self.getWidth(xDiff)/3, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_XY, iX + self.getWidth(xDiff)/3, iY + self.getYStart(2), 8, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_Y, iX + self.getWidth(xDiff)/3, iY + self.getYStart(2) + 8 - self.getHeight(yDiff, -4), 8, self.getHeight(yDiff, -4) - 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_XMY, iX + self.getWidth(xDiff)/3, iY + self.getYStart(2) - self.getHeight(yDiff, -4), 8, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_X, iX + 8 + self.getWidth(xDiff)/3, iY + self.getYStart(2) - self.getHeight(yDiff, -4), self.getWidth(xDiff) * 2/3 - 8, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_HEAD, iX + self.getWidth(xDiff), iY + self.getYStart(2) - self.getHeight(yDiff, -4), 8, 8, eWidGen, 1, 2, False)
					else:
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_X, iX, iY + self.getYStart(3), self.getWidth(xDiff)/2, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_XY, iX + self.getWidth(xDiff)/2, iY + self.getYStart(3), 8, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_Y, iX + self.getWidth(xDiff)/2, iY + self.getYStart(3) + 8 - self.getHeight(yDiff, -2), 8, self.getHeight(yDiff, -2) - 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_XMY, iX + self.getWidth(xDiff)/2, iY + self.getYStart(3) - self.getHeight(yDiff, -2), 8, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_X, iX + 8 + self.getWidth(xDiff)/2, iY + self.getYStart(3) - self.getHeight(yDiff, -2), self.getWidth(xDiff)/2 - 8, 8, eWidGen, 1, 2, False)
						screen.addDDSGFCAt(self.getNextWidgetName(), "TechList", ARROW_HEAD, iX + self.getWidth(xDiff), iY + self.getYStart(3) - self.getHeight(yDiff, -2), 8, 8, eWidGen, 1, 2, False)

			# Unlocks
			iX = 12 + X_INCREMENT * 2
			iY = 38

			iLength = len(self.TechBenefits[iTech])
			if iLength > iMaxElements:
				iLength = iMaxElements

			for i in xrange(iLength):
				sType = self.TechBenefits[iTech][i][0]
				iItem = self.TechBenefits[iTech][i][1]
				sButton = "Item" + str(iTech * 1000 + i)
				sObsolete = "Obsolete" + str(iTech * 1000 + i)
				if sType == "UnlockUnit":
					screen.setImageButtonAt("WID|UNIT" + str(iItem) + '|' + str(iCount), szTechRecord, GC.getUnitInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, eWidGen, 1, 2)
					iCount += 1
				elif sType == "UnlockBuilding":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getBuildingInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iItem, 1, True)
				elif sType == "ObsoleteBuilding":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getBuildingInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE, iItem, -1, False)
					screen.addDDSGFCAt(sObsolete, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_RED_X").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE, iItem, -1, False)
				elif sType == "UnlockSpecialBuilding":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getSpecialBuildingInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_SPECIAL_BUILDING, iTech, iItem, False)
				elif sType == "ObsoleteSpecialBuilding":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getSpecialBuildingInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_SPECIAL, iTech, iItem, False)
					screen.addDDSGFCAt(sObsolete, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_RED_X").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_SPECIAL, iItem, -1, False)
				elif sType == "RevealBonus":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getBonusInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_BONUS_REVEAL, iTech, iItem, False)
				elif sType == "ObsoleteBonus":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getBonusInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_BONUS, iTech, iItem, False)
					screen.addDDSGFCAt(sObsolete, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_RED_X").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_BONUS, iItem, -1, False)
				elif sType == "RouteChange":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MOVE_BONUS").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MOVE_BONUS, iTech, -1, False)
				elif sType == "UnlockPromotion":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getPromotionInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROMOTION, iItem, -1, False)
				elif sType == "FreeUnit":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getUnitInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FREE_UNIT, iItem, iTech, False)
				elif sType == "FeatureProduction":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_FEATURE_PRODUCTION").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FEATURE_PRODUCTION, iTech, -1, False)
				elif sType == "WorkerSpeed":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WORKER_SPEED").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_WORKER_RATE, iTech, -1, False)
				elif sType == "TradeRoute":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_TRADE_ROUTES").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TRADE_ROUTES, iTech, -1, False)
				elif sType == "Health":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_HEALTH").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_HEALTH_RATE, iTech, -1, False)
				elif sType == "Happiness":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_HAPPINESS").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_HAPPINESS_RATE, iTech, -1, False)
				elif sType == "Population":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_POPULATION").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_HAPPINESS_RATE, iTech, -1, False)
				elif sType == "FreeTech":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_FREETECH").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FREE_TECH, iTech, -1, False)
				elif sType == "WaterSight":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_LOS").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_LOS_BONUS, iTech, -1, False)
				elif sType == "MapCentering":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPCENTER").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MAP_CENTER, iTech, -1, False)
				elif sType == "MapVisible":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPREVEAL").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MAP_REVEAL, iTech, -1, False)
				elif sType == "MapTrading":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_MAPTRADING").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MAP_TRADE, iTech, -1, False)
				elif sType == "TechTrading":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_TECHTRADING").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TECH_TRADE, iTech, -1, False)
				elif sType == "GoldTrading":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_GOLDTRADING").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_GOLD_TRADE, iTech, -1, False)
				elif sType == "OpenBorders":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_OPENBORDERS").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OPEN_BORDERS, iTech, -1, False)
				elif sType == "DefensivePact":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_DEFENSIVEPACT").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_DEFENSIVE_PACT, iTech, -1, False)
				elif sType == "PermanentAlliance":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_PERMALLIANCE").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_PERMANENT_ALLIANCE, iTech, -1, False)
				elif sType == "VassalState":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_VASSAL").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_VASSAL_STATE, iTech, -1, False)
				elif sType == "BridgeBuilding":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_BRIDGEBUILDING").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_BUILD_BRIDGE, iTech, -1, False)
				elif sType == "EnablesIrrigation":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_IRRIGATION").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_IRRIGATION, iTech, -1, False)
				elif sType == "IgnoreIrrigation":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_NOIRRIGATION").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_IGNORE_IRRIGATION, iTech, -1, False)
				elif sType == "WaterWork":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERWORK").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_WATER_WORK, iTech, -1, False)
				elif sType == "UnlockImprovement":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getBuildInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_IMPROVEMENT, iTech, iItem, False)
				elif sType == "DomainMoves":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERMOVES").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_DOMAIN_EXTRA_MOVES, iTech, iItem, False)
				elif sType == "CommerceFlexible":
					if iItem == CommerceTypes.COMMERCE_CULTURE:
						temp = AFM.getInterfaceArtInfo("INTERFACE_TECH_CULTURE").getPath()
					elif iItem == CommerceTypes.COMMERCE_ESPIONAGE:
						temp = AFM.getInterfaceArtInfo("INTERFACE_TECH_ESPIONAGE").getPath()
					else: temp = AFM.getInterfaceArtInfo("INTERFACE_GENERAL_QUESTIONMARK").getPath()
					screen.addDDSGFCAt(sButton, szTechRecord, temp, iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_ADJUST, iTech, iItem, False)
				elif sType == "TerrainTrade":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_WATERTRADE").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TERRAIN_TRADE, iTech, iItem, False)
				elif sType == "RiverTrade":
					screen.addDDSGFCAt(sButton, szTechRecord, AFM.getInterfaceArtInfo("INTERFACE_TECH_RIVERTRADE").getPath(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TERRAIN_TRADE, iTech, GC.getNumTerrainInfos(), False)
				elif sType == "ImprovementYield":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getImprovementInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_YIELD_CHANGE, iTech, iItem, False)
				elif sType == "UnlockCivic":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getCivicInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_CIVIC_REVEAL, iTech, iItem, False)
				elif sType == "UnlockProject":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getProjectInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT, iItem, 1, False)
				elif sType == "UnlockProcess":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getProcessInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_PROCESS_INFO, iTech, iItem, False)
				elif sType == "UnlockReligion":
					if GAME.isOption(GameOptionTypes.GAMEOPTION_PICK_RELIGION):
						szButton = AFM.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_RELIGION").getPath()
					else:
						szButton = GC.getReligionInfo(iItem).getButton()
					screen.addDDSGFCAt(sButton, szTechRecord, szButton, iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FOUND_RELIGION, iTech, iItem, False)
				elif sType == "UnlockCorporation":
					screen.addDDSGFCAt(sButton, szTechRecord, GC.getCorporationInfo(iItem).getButton(), iX, iY, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FOUND_CORPORATION, iTech, iItem, False)
				iX += X_INCREMENT

		screen.setFocus("TechList")
		self.updateTechRecords(True)


	def updateBenefits(self):
		iNumTechs = GC.getNumTechInfos()
		for iTech in xrange(iNumTechs):
			self.TechBenefits[iTech] = []
			CvTechInfo = GC.getTechInfo(iTech)
			if CvTechInfo.getGridX() < 1: continue

			for j in xrange(GC.getNumRouteInfos()):
				if GC.getRouteInfo(j).getTechMovementChange(iTech) != 0:
					self.TechBenefits[iTech].append(["RouteChange", j])
			iType = CvTechInfo.getFirstFreeUnitClass()
			if iType > -1:
				iType = GC.getUnitClassInfo(iType).getDefaultUnitIndex()
				if iType > -1:
					self.TechBenefits[iTech].append(["FreeUnit", iType])
			if CvTechInfo.getTradeRoutes():
				self.TechBenefits[iTech].append(["TradeRoute", -1])
			if CvTechInfo.getHealth():
				self.TechBenefits[iTech].append(["Health", -1])
			if CvTechInfo.getHappiness():
				self.TechBenefits[iTech].append(["Happiness", -1])
			if CvTechInfo.getFirstFreeTechs():
				self.TechBenefits[iTech].append(["FreeTech", -1])
			if CvTechInfo.isExtraWaterSeeFrom():
				self.TechBenefits[iTech].append(["WaterSight", -1])
			if CvTechInfo.isMapCentering():
				self.TechBenefits[iTech].append(["MapCentering", -1])
			if CvTechInfo.isMapVisible():
				self.TechBenefits[iTech].append(["MapVisible", -1])
			if CvTechInfo.isMapTrading():
				self.TechBenefits[iTech].append(["MapTrading", -1])
			if CvTechInfo.isTechTrading():
				self.TechBenefits[iTech].append(["TechTrading", -1])
			if CvTechInfo.isGoldTrading():
				self.TechBenefits[iTech].append(["GoldTrading", -1])
			if CvTechInfo.isOpenBordersTrading():
				self.TechBenefits[iTech].append(["OpenBorders", -1])
			if CvTechInfo.isDefensivePactTrading():
				self.TechBenefits[iTech].append(["DefensivePact", -1])
			if CvTechInfo.isPermanentAllianceTrading():
				self.TechBenefits[iTech].append(["PermanentAlliance", -1])
			if CvTechInfo.isVassalStateTrading():
				self.TechBenefits[iTech].append(["VassalState", -1])
			if CvTechInfo.isIrrigation():
				self.TechBenefits[iTech].append(["EnablesIrrigation", -1])
			if CvTechInfo.isIgnoreIrrigation():
				self.TechBenefits[iTech].append(["IgnoreIrrigation", -1])
			if CvTechInfo.isWaterWork():
				self.TechBenefits[iTech].append(["WaterWork", -1])
			for j in xrange(DomainTypes.NUM_DOMAIN_TYPES):
				if CvTechInfo.getDomainExtraMoves(j):
					self.TechBenefits[iTech].append(["DomainMoves", j])
			for j in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if CvTechInfo.isCommerceFlexible(j):
					self.TechBenefits[iTech].append(["CommerceFlexible", j])
			for j in xrange(GC.getNumTerrainInfos()):
				if CvTechInfo.isTerrainTrade(j):
					self.TechBenefits[iTech].append(["TerrainTrade", j])
			if CvTechInfo.isRiverTrade():
				self.TechBenefits[iTech].append(["RiverTrade", -1])
			for j in xrange(GC.getNumImprovementInfos()):
				for k in xrange(YieldTypes.NUM_YIELD_TYPES):
					if GC.getImprovementInfo(j).getTechYieldChanges(iTech, k):
						self.TechBenefits[iTech].append(["ImprovementYield", j])
			if CvTechInfo.getFeatureProductionModifier():
				self.TechBenefits[iTech].append(["FeatureProduction", -1])
			if CvTechInfo.getWorkerSpeedModifier():
				self.TechBenefits[iTech].append(["WorkerSpeed", -1])

		for iType in xrange(GC.getNumUnitInfos()):
			iTech = GC.getUnitInfo(iType).getPrereqAndTech()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockUnit", iType])

		for iType in xrange(GC.getNumBuildingInfos()):
			info = GC.getBuildingInfo(iType)
			iTech = info.getPrereqAndTech()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockBuilding", iType])
			iTech = info.getObsoleteTech()
			if iTech > -1:
				self.TechBenefits[iTech].append(["ObsoleteBuilding", iType])

		for iType in xrange(GC.getNumSpecialBuildingInfos()):
			info = GC.getSpecialBuildingInfo(iType)
			iTech = info.getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockSpecialBuilding", iType])
			iTech = info.getObsoleteTech()
			if iTech > -1:
				self.TechBenefits[iTech].append(["ObsoleteSpecialBuilding", iType])

		for iType in xrange(GC.getNumBonusInfos()):
			info = GC.getBonusInfo(iType)
			iTech = info.getTechReveal()
			if iTech > -1:
				self.TechBenefits[iTech].append(["RevealBonus", iType])
			iTech = info.getTechObsolete()
			if iTech > -1:
				self.TechBenefits[iTech].append(["ObsoleteBonus", iType])

		for j in xrange(GC.getNumPromotionInfos()):
			iTech = GC.getPromotionInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockPromotion", j])

		iFeatures = GC.getNumFeatureInfos()
		for iType in xrange(GC.getNumBuildInfos()):
			info = GC.getBuildInfo(iType)
			iTech = info.getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockImprovement", iType])
				continue

			for j in xrange(iFeatures):
				iTech = info.getFeatureTech(j)
				if iTech > -1:
					self.TechBenefits[iTech].append(["UnlockImprovement", iType])

		for j in xrange(GC.getNumCivicInfos()):
			iTech = GC.getCivicInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockCivic", j])

		for j in xrange(GC.getNumProjectInfos()):
			iTech = GC.getProjectInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockProject", j])

		for j in xrange(GC.getNumProcessInfos()):
			iTech = GC.getProcessInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockProcess", j])

		for j in xrange(GC.getNumReligionInfos()):
			iTech = GC.getReligionInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockReligion", j])

		for j in xrange(GC.getNumCorporationInfos()):
			iTech = GC.getCorporationInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockCorporation", j])


	def getYStart(self, iY):
		return BOX_HEIGHT * iY / 8

	def getWidth(self, xDiff):
		return xDiff * BOX_X_SPACING + (xDiff - 1) * self.wCell

	def getHeight(self, yDiff, iAdjustment):
		return (abs(yDiff) * (BOX_Y_SPACING + BOX_HEIGHT)/2) + (iAdjustment * BOX_HEIGHT/8)

	def updateTechRecords(self, bForce):
		if GAME.isPitbossHost(): return

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT
		screen = self.screen()
		lChanged = []
		CyPlayer = self.CyPlayer
		CyTeam = self.CyTeam

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

		if self.iEraFirst == self.iEraFinal:
			bSingleEra = True
			temp = self.iCurrentEra + 1
			if temp < self.iNumEras:
				xNext = self.minEraX[temp]
			else: xNext = self.minEraX[self.iCurrentEra]
		else:
			bSingleEra = False

		# Analyze change
		for iTech in xrange(GC.getNumTechInfos()):

			CvTechInfo = GC.getTechInfo(iTech)
			iX = CvTechInfo.getGridX()
			if iX < 1: continue

			iEra = CvTechInfo.getEra()
			if bSingleEra:
				if iEra != self.iCurrentEra and iX != xNext:
					continue

			elif iEra < self.iEraFirst or iEra > self.iEraFinal:
				continue

			if CyTeam.isHasTech(iTech):
				if self.aiCurrentState[iTech] != "CIV_HAS_TECH":
					self.aiCurrentState[iTech] = "CIV_HAS_TECH"
					lChanged.append((iTech, CvTechInfo, iX, iEra))
				elif bForce:
					lChanged.append((iTech, CvTechInfo, iX, iEra))

			elif CyPlayer.isResearchingTech(iTech):
				self.aiCurrentState[iTech] = "CIV_IS_RESEARCHING"
				lChanged.append((iTech, CvTechInfo, iX, iEra))

			elif CyPlayer.canEverResearch(iTech):
				if self.aiCurrentState[iTech] != "CIV_TECH_AVAILABLE":
					self.aiCurrentState[iTech] = "CIV_TECH_AVAILABLE"
					lChanged.append((iTech, CvTechInfo, iX, iEra))
				elif bForce:
					lChanged.append((iTech, CvTechInfo, iX, iEra))

			elif self.aiCurrentState[iTech] != "CIV_NO_RESEARCH":
				self.aiCurrentState[iTech] = "CIV_NO_RESEARCH"
				lChanged.append((iTech, CvTechInfo, iX, iEra))
			elif bForce:
				lChanged.append((iTech, CvTechInfo, iX, iEra))

		# Make change
		if lChanged:
			iCurrentEra = self.iCurrentEra
			iMinX = self.minEraX[self.iEraFirst]
			wCell = self.wCell
			while lChanged:
				iTech, CvTechInfo, iX, iEra = lChanged.pop()
				szTech = str(iTech)

				szTechRecord = "WID|TECH|CHOICE" + szTech
				szTechID = "TechID" + szTech

				iX = (iX - iMinX) * (BOX_X_SPACING + wCell)
				iY = (CvTechInfo.getGridY() - 1) * (BOX_HEIGHT + BOX_Y_SPACING)/2

				szState = self.aiCurrentState[iTech]
				szName = CvTechInfo.getDescription()

				szTechString = self.aFontList[5]
				iAdjustment = 6
				iAdvisor = CvTechInfo.getAdvisorType()
				if iAdvisor > -1:
					szTechString += TRNSLTR.getText(self.Advisors[iAdvisor], ())
					iAdjustment = 3
				if szState == "CIV_IS_RESEARCHING":
					szTechString += str(CyPlayer.getQueuePosition(iTech)) + ") "
				szTechString += szName
				screen.setLabelAt(szTechID, "TechList", szTechString, 1<<0, iX + iAdjustment + 6 + (X_INCREMENT * 2), iY + 10, 0, eFontTitle, eWidGen, 1, 2)
				screen.setHitTest(szTechID, HitTestTypes.HITTEST_NOHIT)

				# Colours
				if szState == "CIV_HAS_TECH":
					screen.setPanelColor(szTechRecord, self.colorHasTech[0], self.colorHasTech[1], self.colorHasTech[2])

				elif szState == "CIV_IS_RESEARCHING":
					screen.setPanelColor(szTechRecord, self.colorResearching[0], self.colorResearching[1], self.colorResearching[2])

				elif szState == "CIV_TECH_AVAILABLE":
					if iEra > iCurrentEra:
						r, g, b = self.colorNeoTech
					elif iEra < iCurrentEra:
						r, g, b = self.colorArchaicTech
					else:
						r, g, b = self.colorCoevalTech
					screen.setPanelColor(szTechRecord, r, g, b)
				else:
					screen.setPanelColor(szTechRecord, self.colorNoResearch[0], self.colorNoResearch[1], self.colorNoResearch[2])

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

			elif iData1 == 64512:
				# Panel UI elements does not report the widget name when left clicked...
				if szFlag == "MOUSE_LBUTTONUP":
					if self.CyPlayer.canEverResearch(iData2):
						CyMessageControl().sendResearch(iData2, bShift)
						self.updateTechRecords(False)

		elif iCode == 11: # List Select
			if NAME == "TC_DebugDD":
				iIndex = screen.getSelectedPullDownID(NAME)
				self.iPlayer = iPlayer = screen.getPullDownData(NAME, iIndex)
				self.CyPlayer = GC.getPlayer(iPlayer)
				self.CyTeam = GC.getTeam(self.CyPlayer.getTeam())
				self.fullRefresh(screen)

			elif NAME == "PlatyHideFromEra":
				iIndex = screen.getSelectedPullDownID("PlatyHideFromEra")
				self.iEraFirst = screen.getPullDownData("PlatyHideFromEra", iIndex)
				self.fullRefresh(screen)

			elif NAME == "PlatyHideToEra":
				iIndex = screen.getSelectedPullDownID("PlatyHideToEra")
				self.iEraFinal = screen.getPullDownData("PlatyHideToEra", iIndex)
				self.fullRefresh(screen)


	def getNextWidgetName(self):
		szName = "TechArrow" + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

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


	def updateSelectedTech(self):
		screen = self.screen()
		pPlayer = GC.getPlayer(GAME.getActivePlayer())

		szName = ""
		iCost = 0
		if self.m_iSelectedTech > -1:
			szName = GC.getTechInfo(self.m_iSelectedTech).getDescription()
			iCost = pPlayer.getAdvancedStartTechCost(self.m_iSelectedTech, True)

		screen.hide("ASPointsLabel")
		screen.hide("AddTechButton")
		if iCost > 0:
			szText = "<font=4>" + TRNSLTR.getText("TXT_KEY_WB_AS_SELECTED_TECH_COST", (iCost, pPlayer.getAdvancedStartPoints()))
			screen.setLabel("ASPointsLabel", "", szText, 1<<0, 180, self.yRes - 42, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
			if pPlayer.getAdvancedStartPoints() >= iCost:
				screen.show("AddTechButton")

		szText = "<font=4>" + szName + " (" + str(iCost) + ')'
		screen.setLabel("SelectedTechLabel", "", szText, 1<<2, self.xRes / 2, self.yRes - 42, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)

	def onClose(self):
		print "Exit Tech Tree"
		pPlayer = GC.getPlayer(GAME.getActivePlayer())
		if pPlayer.getAdvancedStartPoints() > -1:
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, True)
		del (
			self.screenId, self.nWidgetCount, self.InputData, self.szTxtTT, self.iOffsetTT, self.bLockedTT, self.iUnitTT, self.bUnitTT,
			self.xRes, self.yRes, self.aFontList, self.wCell,
			self.colorNoResearch, self.colorHasTech, self.colorResearching, self.colorArchaicTech, self.colorCoevalTech, self.colorNeoTech,
			self.iEraFirst, self.iEraFinal, self.iCurrentEra, self.aiCurrentState, self.TechBenefits, self.Advisors
		)