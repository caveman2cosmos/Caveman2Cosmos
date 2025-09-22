from CvPythonExtensions import *

# globals
GC = CyGlobalContext()
TRNSLTR = CyTranslator()
MAX_COMMERCE_RATE_MODIFIER_VALUE = 10000000

class CvFinanceAdvisor:

	def __init__(self, screenId):
		self.screenId = screenId

	def getScreen(self):
		return CyGInterfaceScreen("FinanceAdvisor", self.screenId)

	def interfaceScreen(self):
		screen = self.getScreen()
		if screen.isActive():
			return

		import PythonToolTip
		self.tooltip = PythonToolTip.PythonToolTip()

		GAME = GC.getGame()
		# Get screen resolution.
		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y
		# Calibrate variables.
		if yRes > 1000:
			H_BOT_ROW = 110
			H_EDGE_PANEL = 38
			uFontEdge = "<font=4b>"
		elif yRes > 800:
			H_BOT_ROW = 92
			H_EDGE_PANEL = 32
			uFontEdge = "<font=3b>"
		else:
			H_BOT_ROW = 78
			H_EDGE_PANEL = 29
			uFontEdge = "<font=2b>"

		self.aFontList = aFontList = [uFontEdge] + SR.aFontList

		self.Y_TOP_PAGE = Y_TOP_PAGE = H_EDGE_PANEL - 6
		self.H_PAGE = H_PAGE = yRes - Y_TOP_PAGE - H_EDGE_PANEL + 8
		self.Y_BOT_TEXT = yRes - H_EDGE_PANEL + 10

		self.CyPlayer = CyPlayer = GC.getActivePlayer()
		self.iPlayer = iPlayer = GAME.getActivePlayer()
		self.nWidgetCount = 0

		self.goldFromCivs = 0
		self.yBuildingExpenses = 0
		self.szTreasury = TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_TREASURY", (0,))[:-3]

		self.bStrike = bStrike = CyPlayer.isStrike()
		if bStrike:
			self.szStrike = TRNSLTR.getText("TXT_KEY_MISC_STRIKE", ())
		# Commerce icons
		aList = []
		for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
			aList.append(u'%c' % GC.getCommerceInfo(i).getChar())
		self.iconCommerceList = list(aList)
		# Enumerators
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT
		ePanelHudHelp = PanelStyles.PANEL_STYLE_HUD_HELP
		# Build screen
		screen.setRenderInterfaceOnly(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.addDDSGFC("FinAdv_BG", CyArtFileMgr().getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, xRes, yRes, eWidGen, 0, 0)
		screen.addPanel("FinAdv_TopPanel", "", "", True, False, 0, 0, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("FinAdv_BotPanel", "", "", True, False, 0, yRes - H_EDGE_PANEL, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.setLabel("FinAdv_Header", "", uFontEdge + TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_TITLE",()), 1<<2, xRes / 2, 2, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("FinAdv_Exit", "", uFontEdge + TRNSLTR.getText("TXT_WORD_EXIT",()), 1<<1, xRes - 16, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		import DebugUtils
		if DebugUtils.isAnyDebugMode():
			DD = "FinAdv_DebugDD"
			screen.addDropDownBoxGFC(DD, 8, 0, 300, eWidGen, -1, -1, FontTypes.GAME_FONT)
			for iPlayerX in range(GC.getMAX_PLAYERS()):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					screen.addPullDownString(DD, CyPlayerX.getName(), iPlayerX, iPlayerX, iPlayer == iPlayerX)

		uFont3b = aFontList[3]
		a3rdX = xRes / 3
		dx = a3rdX / 16
		x0 = dx
		y0 = self.Y_TOP_PAGE + 16
		dx = a3rdX - 2 * dx
		dy = self.H_PAGE - 32
		screen.addPanel("FinAdv_Panel_1", "", "", True, True, x0, y0, dx, dy, ePanelHudHelp)
		szText = TRNSLTR.getText("TXT_KEY_CONCEPT_COMMERCE", ())
		screen.setLabelAt("", "FinAdv_Panel_1", uFont3b + szText, 1<<2, dx/2, 8, 0, eFontTitle, eWidGen, 1, 1)

		x0 += a3rdX
		screen.addPanel("FinAdv_Panel_2", "", "", True, True, x0, y0, dx, dy, ePanelHudHelp)
		szText = TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_INCOME_HEADER", ())
		screen.setLabelAt("", "FinAdv_Panel_2", uFont3b + szText, 1<<2, dx/2, 8, 0, eFontTitle, eWidGen, 1, 1)

		x0 += a3rdX
		screen.addPanel("FinAdv_Panel_3", "", "", True, True, x0, y0, dx, dy, ePanelHudHelp)
		szText = TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_EXPENSES_HEADER", ())
		screen.setLabelAt("", "FinAdv_Panel_3", uFont3b + szText, 1<<2, dx/2, 8, 0, eFontTitle, eWidGen, 1, 1)

		self.drawBase()
		self.updateContents()

	def drawBase(self):
		screen = self.getScreen()
		# Enumerators
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eGameFont = FontTypes.GAME_FONT
		ePanelHudHelp = PanelStyles.PANEL_STYLE_HUD_HELP
		# Variables
		xRes = self.xRes
		yRes = self.yRes
		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2, uFont1b, uFont1 = self.aFontList
		CyPlayer = self.CyPlayer
		iPlayer = self.iPlayer

		a3rdX = xRes / 3
		dx = a3rdX / 16
		x0 = dx
		y0 = self.Y_TOP_PAGE + 16
		dx = a3rdX - 2 * dx
		dy = self.H_PAGE - 32
		############
		# Commerce #
		############
		iCommerce = iTiles = iYield0 = iYield1 = iYield2 = iYield3 = iYield4 = 0
		iTeam = CyPlayer.getTeam()
		for CyCity in CyPlayer.cities():
			if not CyCity.isDisorder():
				# Work plots
				for j in range(GC.getNUM_CITY_PLOTS()):
					CyPlot = CyCity.getCityIndexPlot(j)
					if CyPlot and CyPlot.hasYield() and CyCity.isWorkingPlot(CyPlot):
						iTiles += 1
						iYield0 += CyPlot.getYield(YieldTypes.YIELD_COMMERCE)
				# Trade
				for j in range(CyCity.getTradeRoutes()):
					CyCityX = CyCity.getTradeCity(j)
					if not CyCityX: continue

					trade = CyCity.calculateTradeYield(YieldTypes.YIELD_COMMERCE, CyCity.calculateTradeProfitTimes100(CyCityX))
					if CyCityX.getTeam() == iTeam:
						iYield1 += trade
					else: # Foreign Trade
						iYield2 += trade
				# corporations
				iYield3 += CyCity.getCorporationYield(YieldTypes.YIELD_COMMERCE)
				# specialists
				for iType in range(GC.getNumSpecialistInfos()):
					iYield4 += CyPlayer.specialistYield(iType, YieldTypes.YIELD_COMMERCE) * (CyCity.getSpecialistCount(iType) + CyCity.getFreeSpecialistCount(iType))

		Pnl = "FinAdv_Scroll_1"
		screen.addScrollPanel(Pnl, "", x0, y0 + 32, dx, dy - 64, ePanelHudHelp)
		screen.setStyle(Pnl, "ScrollPanel_Alt_Style")

		x = dx - 24
		y = 0
		# Work plots
		if iYield0:
			szText = TRNSLTR.getText("TXT_KEY_CONCEPT_WORKED_TILES", (iTiles,))
			screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt("", Pnl, uFont2 + str(iYield0), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			iCommerce += iYield0
			y += 20

		# trade
		if iYield1:
			iYield1 /= 100
			szText = TRNSLTR.getText("TXT_KEY_CONCEPT_DOMESTIC_TRADE", ())
			screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_DOMESTIC_TRADE, iPlayer, 1)
			screen.setLabelAt("", Pnl, uFont2 + str(iYield1), 1<<1, x, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_DOMESTIC_TRADE, iPlayer, 1)
			iCommerce += iYield1
			y += 20

		if iYield2:
			iYield2 /= 100
			szText = TRNSLTR.getText("TXT_KEY_CONCEPT_FOREIGN_TRADE", ())
			screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_FOREIGN_TRADE, iPlayer, 1)
			screen.setLabelAt("", Pnl, uFont2 + str(iYield2), 1<<1, x, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_FOREIGN_TRADE, iPlayer, 1)
			iCommerce += iYield2
			y += 20

		# corporations
		if iYield3 > 0:
			szText = TRNSLTR.getText("TXT_KEY_CONCEPT_CORPORATIONS", ())
			screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt("", Pnl, uFont2 + str(iYield3), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			iCommerce += iYield3
			y += 20

		# specialists
		if iYield4 > 0:
			szText = TRNSLTR.getText("TXT_KEY_CONCEPT_SPECIALISTS", ())
			screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt("", Pnl, uFont2 + str(iYield4), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			iCommerce += iYield4
			y += 20

		# buildings
		iTotalCommerce = CyPlayer.calculateTotalYield(YieldTypes.YIELD_COMMERCE)
		# buildings includes 50% capital bonus for Bureaucracy civic
		iBuildings = iTotalCommerce - iCommerce
		if iBuildings > 0:
			szText = TRNSLTR.getText("TXT_KEY_WB_BUILDINGS", ())
			screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt("", Pnl, uFont2 + str(iBuildings), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			iCommerce += iBuildings
			y += 20

		y += 8
		szText = TRNSLTR.getText("TXT_KEY_BUG_FINANCIAL_ADVISOR_COMMERCE", ())
		screen.setLabelAt("", Pnl, uFont2b + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
		szText = str(iCommerce) + u'%c' % GC.getYieldInfo(YieldTypes.YIELD_COMMERCE).getChar()
		screen.setLabelAt("", Pnl, uFont2b + szText, 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
		y += 32

		self.yCommerceSlider = y

		# Income
		x0 += a3rdX
		Pnl = "FinAdv_Scroll_2"
		screen.addScrollPanel(Pnl, "", x0, y0 + 32, dx, dy - 64, ePanelHudHelp)
		screen.setStyle(Pnl, "ScrollPanel_Alt_Style")

		# Expenses
		x0 += a3rdX
		Pnl = "FinAdv_Scroll_3"
		screen.addScrollPanel(Pnl, "", x0, y0 + 32, dx, dy - 64, ePanelHudHelp)
		screen.setStyle(Pnl, "ScrollPanel_Alt_Style")

		if CyPlayer.isAnarchy():
			return

		iExpenses = 0
		iFinalUnitUpkeep = CyPlayer.getFinalUnitUpkeep()
		totalUnitSupply = CyPlayer.calculateUnitSupply()
		iTreasuryUpkeep = CyPlayer.getTreasuryUpkeep()
		totalMaintenance = CyPlayer.getTotalMaintenance()
		totalCivicUpkeep = CyPlayer.getCivicUpkeep(False)
		iCorporateMaintenance = CyPlayer.getCorporateMaintenance()

		iInflation = CyPlayer.getFinalExpense() - CyPlayer.calculatePreInflatedCosts()
		self.goldFromCivs = goldFromCivs = CyPlayer.getGoldPerTurn()

		y = -2
		screen.setTextAt("unitUpkeep0", Pnl, uFont2 + TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_UNIT_UPKEEP", ()), 1<<0, 6, y, 0, eGameFont, eWidGen, 1, 2)
		screen.setLabelAt("unitUpkeep1", Pnl, uFont2 + str(iFinalUnitUpkeep), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 2)
		iExpenses += iFinalUnitUpkeep
		y += 22

		szText = TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_UNITSUPPLY", ())
		screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_AWAY_SUPPLY, iPlayer, 1)
		screen.setLabelAt("", Pnl, uFont2 + str(totalUnitSupply), 1<<1, x, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_AWAY_SUPPLY, iPlayer, 1)
		iExpenses += totalUnitSupply
		y += 20

		szText = TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_CITY_MAINTENANCE", ())
		screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_CITY_MAINT, iPlayer, 1)
		screen.setLabelAt("", Pnl, uFont2 + str(totalMaintenance), 1<<1, x, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_CITY_MAINT, iPlayer, 1)
		iExpenses += totalMaintenance
		y += 20

		screen.setLabelAt("", Pnl, uFont2 + TRNSLTR.getText("TXT_KEY_TREASURY_UPKEEP", ()), 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 2)
		screen.setLabelAt("", Pnl, uFont2 + str(iTreasuryUpkeep), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 2)
		iExpenses += iTreasuryUpkeep
		y += 20

		szText = TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_CIVIC_UPKEEP", ())
		screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_CIVIC_UPKEEP, iPlayer, 1)
		screen.setLabelAt("", Pnl, uFont2 + str(totalCivicUpkeep), 1<<1, x, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_CIVIC_UPKEEP, iPlayer, 1)
		iExpenses += totalCivicUpkeep
		y += 20

		screen.setLabelAt("", Pnl, uFont2 + TRNSLTR.getText("TXT_INTERFACE_TREASURYHELP_CORPORATE_MAINTENANCE", ()), 1<<0, 8, y, 0, eGameFont, eWidGen, iPlayer, 1)
		screen.setLabelAt("", Pnl, uFont2 + str(iCorporateMaintenance), 1<<1, x, y, 0, eGameFont, eWidGen, iPlayer, 1)
		iExpenses += iCorporateMaintenance
		y += 20

		if goldFromCivs < 0:
			szText = TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_COST_PER_TURN", ())
			screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_FOREIGN_INCOME, iPlayer, 1)
			screen.setLabelAt("", Pnl, uFont2 + str(-goldFromCivs), 1<<1, x, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_FOREIGN_INCOME, iPlayer, 1)
			iExpenses -= goldFromCivs
			y += 20

		szText = TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_INFLATION", ())
		screen.setLabelAt("", Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_INFLATED_COSTS, iPlayer, 1)
		screen.setLabelAt("", Pnl, uFont2 + str(iInflation), 1<<1, x, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_INFLATED_COSTS, iPlayer, 1)
		iExpenses += iInflation
		y += 20

		y += 8
		szText = TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_EXPENSES", ())
		screen.setLabelAt("", Pnl, uFont2b + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
		screen.setLabelAt("", Pnl, uFont2b + str(iExpenses), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)

		self.yBuildingExpenses = y + 16


	def updateContents(self):
		screen = self.getScreen()
		aName = self.getNextWidgetName
		self.deleteAllWidgets(screen, aName)
		# Enumerators
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eGameFont = FontTypes.GAME_FONT
		eComGold = CommerceTypes.COMMERCE_GOLD
		# Variables
		xRes = self.xRes
		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2, uFont1b, uFont1 = self.aFontList
		iconCommerceList = self.iconCommerceList
		CyPlayer = self.CyPlayer
		iIncome = CyPlayer.getCommerceRate(eComGold)

		# Treasury footer
		szTxt = self.szTreasury
		iGold = CyPlayer.getGold()
		iGoldRate = CyPlayer.calculateGoldRate()
		if iGold < 0:
			szTxt += "<color=255,0,0,255>"
		szTxt += "%d%s" %(iGold, iconCommerceList[eComGold])
		if iGoldRate:
			szTxt += " <color="
			if iGoldRate > 0 and iGold + iGoldRate >= 0:
				szTxt += "127,255,0,255>"
			elif iGoldRate < 0 and iGold + iGoldRate < 0:
				szTxt += "255,124,0,255>"
			else:
				szTxt += "255,227,0,255>"
			if iGoldRate > 0:
				szTxt += "+ "
			else:
				szTxt += "- "
				iGoldRate = -iGoldRate
			szTxt += str(iGoldRate)
		if self.bStrike:
			szTxt += self.szStrike

		screen.setLabel(aName(), "", uFontEdge + szTxt, 1<<2, xRes/2, self.Y_BOT_TEXT, 0, FontTypes.TITLE_FONT, eWidGen, 1, 1)

		# Content
		x = xRes / 3 - xRes / 24 - 24
		y = self.yCommerceSlider
		Btnsize = 20
		xBtnOffset = 0
		yBtnOffset = 0
		if (xRes > 2500) :
			Btnsize = 30
			xBtnOffset = 60
			yBtnOffset = -2
		Pnl = "FinAdv_Scroll_1"
		# Slider percentages
		iInc = GC.getDefineINT("COMMERCE_PERCENT_CHANGE_INCREMENTS")
		iRange = CommerceTypes.NUM_COMMERCE_TYPES
		for i in range(iRange):
			iType = (i + 1) % iRange # 1, 2, 3, 0 (Research, Culture, Espionage, Gold)

			if CyPlayer.isCommerceFlexible(iType):

				szText = uFont2b + iconCommerceList[iType]
				screen.setLabelAt(aName(), Pnl, szText, 1<<0, 8, y + 2, 0, eGameFont, eWidGen, 1, 1)

				Btn = aName()
				screen.setImageButtonAt(Btn, Pnl, "", 26, y+yBtnOffset, Btnsize, Btnsize, WidgetTypes.WIDGET_CHANGE_PERCENT, iType, iInc)
				screen.setStyle(Btn, "Button_Plus_Style")
				Btn = aName()
				screen.setImageButtonAt(Btn, Pnl, "", 28+Btnsize, y+yBtnOffset, Btnsize, Btnsize, WidgetTypes.WIDGET_CHANGE_PERCENT, iType, -iInc)
				screen.setStyle(Btn, "Button_Minus_Style")

				szText = uFont2b + str(CyPlayer.getCommercePercent(iType)) + "%"
				screen.setLabelAt(aName(), Pnl, szText, 1<<1, 112+xBtnOffset, y+yBtnOffset, 0, eGameFont, eWidGen, 1, 1)

				if CommerceTypes(iType) == CommerceTypes.COMMERCE_RESEARCH:
					# Research is subject to modifiers.
					iRate = CyPlayer.calculateResearchRate(CyPlayer.getCurrentResearch())
				else:
					iRate = CyPlayer.getCommerceRate(CommerceTypes(iType))
				szRate = uFont2b + str(iRate) + iconCommerceList[iType]
				screen.setLabelAt(aName(), Pnl, szRate, 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
				y += Btnsize
			elif iType == eComGold:
				y += Btnsize - 12
				szText = uFont2b + iconCommerceList[iType]
				screen.setLabelAt(aName(), Pnl, szText, 1<<0, 8, y + 2, 0, eGameFont, eWidGen, 1, 1)

				szText = uFont2b + GC.getCommerceInfo(iType).getDescription()
				screen.setLabelAt(aName(), Pnl, szText, 1<<0, 26, y+yBtnOffset, 0, eGameFont, eWidGen, 1, 1)

				szText = uFont2b + str(CyPlayer.getCommercePercent(iType)) + "%"
				screen.setLabelAt(aName(), Pnl, szText, 1<<1, 108+xBtnOffset, y+yBtnOffset, 0, eGameFont, eWidGen, 1, 1)

				szCommerce = uFont2b + str(iIncome) + iconCommerceList[iType]
				screen.setLabelAt(aName(), Pnl, szCommerce, 1<<1, x, y+yBtnOffset, 0, eGameFont, eWidGen, 1, 1)

		if CyPlayer.isAnarchy():
			return

		iPlayer = self.iPlayer
		playerGoldModifier = CyPlayer.getCommerceRateModifier(eComGold)
		playerCapitalGoldModifier = CyPlayer.getCapitalCommerceRateModifier(eComGold)

		# Income
		iTaxRate = CyPlayer.getCommercePercent(eComGold)

		multipliers = []
		for iType in range(GC.getNumBuildingInfos()):
			info = GC.getBuildingInfo(iType)
			iMultiplier = info.getCommerceModifier(eComGold)
			iGlobalMultiplier = info.getGlobalCommerceModifier(eComGold)
			if iMultiplier != 0 or iGlobalMultiplier != 0:
				multipliers.append([iType, iMultiplier, iGlobalMultiplier, 0, 0.0])

		iHeadquartersCount = 0
		iShrinesCount = 0
		fTaxes = 0.0
		fPlayerGoldModifierEffect = 0.0
		fBonusGoldModifierEffect = 0.0
		fUnmodifiedTotal = 0.0
		fBuildings = 0.0
		fHeadquarters = 0.0
		fShrines = 0.0
		iCorporations = 0
		fSpecialists = 0.0
		iWealthCount = 0
		fWealth = 0.0
		eWealth = GC.getInfoTypeForString("PROCESS_WEALTH")
		for CyCity in CyPlayer.cities():
			if not CyCity.isDisorder():
				fCityTaxes = CyCity.getYieldRate(YieldTypes.YIELD_COMMERCE) * iTaxRate / 100.0
				fTaxes += fCityTaxes

				fCityBuildings = 0.0
				fCityHeadquarters = 0.0
				fCityShrines = 0.0
				for iType in range(GC.getNumBuildingInfos()):
					if CyCity.isActiveBuilding(iType):
						iBuildingGold = CyCity.getBuildingCommerceByBuilding(eComGold, iType)
						if iBuildingGold:
							info = GC.getBuildingInfo(iType)
							if info.getFoundsCorporation() > -1:
								fCityHeadquarters += iBuildingGold
								iHeadquartersCount += 1
							elif info.getGlobalReligionCommerce() > -1:
								fCityShrines += iBuildingGold
								iShrinesCount += 1
							else:
								fCityBuildings += iBuildingGold
				fBuildings += fCityBuildings
				fHeadquarters += fCityHeadquarters
				fShrines += fCityShrines

				iCityCorporations = CyCity.getCorporationCommerce(eComGold)
				iCorporations += iCityCorporations

				fCitySpecialists = CyCity.getSpecialistCommerce(eComGold)
				fSpecialists += fCitySpecialists

				fSpecialists += (CyCity.getSpecialistPopulation() + CyCity.getNumGreatPeople()) * CyPlayer.getSpecialistExtraCommerce(eComGold)

				fCityWealth = 0.0
				if CyCity.isProductionProcess() and CyCity.getProductionProcess() == eWealth:
					fCityWealth = CyCity.getProductionToCommerceModifier(eComGold) * CyCity.getYieldRate(YieldTypes.YIELD_PRODUCTION) / 100.0
					fWealth += fCityWealth
					iWealthCount += 1

				# modifiers don't multiply wealth
				fCityTotal = fCityTaxes + fCityBuildings + fCityHeadquarters + fCityShrines + iCityCorporations + fCitySpecialists
				fUnmodifiedTotal += fCityTotal

				if CyCity.isCapital():
					fPlayerGoldModifierEffect += fCityTotal * (playerGoldModifier + playerCapitalGoldModifier) / 100.0
				else:
					fPlayerGoldModifierEffect += fCityTotal * playerGoldModifier / 100.0

				fBonusGoldModifierEffect += fCityTotal * CyCity.getBonusCommerceRateModifier(eComGold) / 100.0

				for entry in multipliers:
					if CyCity.isActiveBuilding(entry[0]):
						entry[3] += 1
						entry[4] += fCityTotal * entry[1] / 100.0

		iTotalMinusTaxes = min(MAX_COMMERCE_RATE_MODIFIER_VALUE,int(fBuildings) + int(fHeadquarters) + int(fShrines) + iCorporations + int(fSpecialists) + int(fWealth) + int(fPlayerGoldModifierEffect) + int(fBonusGoldModifierEffect))
		for entry in multipliers:
			if entry[3]:
				iTotalMinusTaxes += int(entry[4])
				if entry[2]:
					fAdjust = fUnmodifiedTotal * entry[2] / 100.0
					entry[4] += fAdjust
					fPlayerGoldModifierEffect -= fAdjust

		Pnl = "FinAdv_Scroll_2"
		y = 0
		szText = TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_TAXES", ())
		screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
		screen.setLabelAt(aName(), Pnl, uFont2 + str(int(fTaxes)), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
		y += 20

		if fBuildings:
			szText = TRNSLTR.getText("TXT_KEY_WB_BUILDINGS", ())
			screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt(aName(), Pnl, uFont2 + str(int(fBuildings)), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			y += 20

		if fHeadquarters:
			szText = TRNSLTR.getText("TXT_KEY_CORPORATION_HEADQUARTERS", ()) + " (%d)" % iHeadquartersCount
			screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt(aName(), Pnl, uFont2 + str(int(fHeadquarters)), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			y += 20

		if iCorporations:
			szText = TRNSLTR.getText("TXT_KEY_CONCEPT_CORPORATIONS", ())
			screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt(aName(), Pnl, uFont2 + str(iCorporations), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			y += 20

		if fShrines:
			szText = TRNSLTR.getText("TXT_KEY_CONCEPT_RELIGIOUS_SHRINES", ()) + " (%d)" % iShrinesCount
			screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt(aName(), Pnl, uFont2 + str(int(fShrines)), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			y += 20

		if fSpecialists:
			szText = TRNSLTR.getText("TXT_KEY_CONCEPT_SPECIALISTS", ())
			screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_SPECIALISTS, iPlayer, 1)
			screen.setLabelAt(aName(), Pnl, uFont2 + str(int(fSpecialists)), 1<<1, x, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_SPECIALISTS, iPlayer, 1)
			y += 20

		if fPlayerGoldModifierEffect:
			szText = TRNSLTR.getText("TXT_KEY_CONCEPT_PLAYER_GOLD_MODIFIERS", ())
			screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt(aName(), Pnl, uFont2 + str(int(fPlayerGoldModifierEffect)), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			y += 20

		if fBonusGoldModifierEffect:
			szText = TRNSLTR.getText("TXT_KEY_CONCEPT_BONUS_GOLD_MODIFIERS", ())
			screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt(aName(), Pnl, uFont2 + str(int(fBonusGoldModifierEffect)), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			y += 20

		if fWealth > 0.0 and iWealthCount > 0:
			szText = TRNSLTR.getText("TXT_KEY_PROCESS_WEALTH", ()) + " (%d)" % iWealthCount
			screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
			screen.setLabelAt(aName(), Pnl, uFont2 + str(int(fWealth)), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
			y += 20

		goldFromCivs = self.goldFromCivs
		if goldFromCivs > 0:
			szText = TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_PER_TURN", ())
			screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_FOREIGN_INCOME, iPlayer, 1)
			screen.setLabelAt(aName(), Pnl, uFont2 + str(goldFromCivs), 1<<1, x, y, 0, eGameFont, WidgetTypes.WIDGET_HELP_FINANCE_FOREIGN_INCOME, iPlayer, 1)
			iIncome += goldFromCivs
			y += 20

		y += 8
		szText = TRNSLTR.getText("TXT_KEY_FINANCIAL_ADVISOR_INCOME", ())
		screen.setLabelAt(aName(), Pnl, uFont2b + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
		screen.setLabelAt(aName(), Pnl, uFont2b + str(iIncome), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)
		y += 16

		for iType, iMultiplier, iGlobalMultiplier, iCount, fGold in multipliers:
			if iCount and fGold > 0:
				y += 20
				szText = GC.getBuildingInfo(iType).getDescription() + " "
				szText += TRNSLTR.getText("TXT_KEY_BUG_FINANCIAL_ADVISOR_BUILDING_COUNT_AVERAGE", (iCount, "%.2f" %(fGold / iCount), "%.2f" %fGold))
				screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
				screen.setLabelAt(aName(), Pnl, uFont2 + str(int(fGold)), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)

		# Expenses
		Pnl = "FinAdv_Scroll_3"
		y = self.yBuildingExpenses
		for iType, iMultiplier, iGlobalMultiplier, iCount, fGold in multipliers:
			if iCount and fGold < 0:
				y += 20
				szText = GC.getBuildingInfo(iType).getDescription() + " "
				szText += TRNSLTR.getText("TXT_KEY_BUG_FINANCIAL_ADVISOR_BUILDING_COUNT_AVERAGE", (iCount, "%.2f" %(fGold / iCount)))
				screen.setLabelAt(aName(), Pnl, uFont2 + szText, 1<<0, 8, y, 0, eGameFont, eWidGen, 1, 1)
				screen.setLabelAt(aName(), Pnl, uFont2 + str(int(fGold)), 1<<1, x, y, 0, eGameFont, eWidGen, 1, 1)


	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = "FinAdv_Widget" + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def deleteAllWidgets(self, screen, aName):
		i = self.nWidgetCount - 1
		while i >= 0:
			self.nWidgetCount = i
			screen.deleteWidget(aName())
			i -= 1
		self.nWidgetCount = 0


	#--------------------------#
	# Base operation functions #
	#||||||||||||||||||||||||||#
	def update(self, fDelta):
		if CyInterface().isDirty(InterfaceDirtyBits.Financial_Screen_DIRTY_BIT):
			CyInterface().setDirty(InterfaceDirtyBits.Financial_Screen_DIRTY_BIT, False)
			self.updateContents()
		if self.tooltip.bLockedTT:
			self.tooltip.handle(self.getScreen())

	# Will handle the input for this screen...
	def handleInput(self, inputClass):

		iCode	= inputClass.eNotifyCode
		NAME	= inputClass.szFunctionName

		screen = self.getScreen()
		self.tooltip.reset(screen)

		if iCode == 4: # Mouse Enter

			if NAME == "unitUpkeep":
				self.tooltip.handle(screen, CyGameTextMgr().getFinanceUnitUpkeepString(self.iPlayer))

		elif iCode == 11: # List Select
			self.iPlayer = screen.getPullDownData("FinAdv_DebugDD", inputClass.iData)
			self.CyPlayer = GC.getPlayer(self.iPlayer)
			self.drawBase()
			self.updateContents()
			return 1
		return 0

	def onClose(self):
		screen = self.getScreen()
		screen.setDying(True)
		del self.nWidgetCount, self.CyPlayer, self.iPlayer, self.bStrike, self.aFontList, self.iconCommerceList, \
			self.szTreasury, self.yCommerceSlider, self.yBuildingExpenses
