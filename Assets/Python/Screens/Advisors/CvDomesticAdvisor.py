from CvPythonExtensions import *
import HandleInputUtil
import cPickle

# globals
GC = CyGlobalContext()
AFM = CyArtFileMgr()
TRNSLTR = CyTranslator()

class CvDomesticAdvisor:

	def __init__(self, screenId):
		self.screenId = screenId
		self.bInitialize = True
		self.bCustomizing = False
		self.iPage = 0

		# The currently active page
		self.currentPage = None
		self.visiblePage = None

		# Values to check to see if we need to color the number as a problem
		self.PROBLEM_VALUES_DICT = {
			"GARRISON"			: 0,
			"HAPPY"				:-1,
			"HEALTH"			:-1,
			"GROWTH"			:-1,
			"FOOD"				:-1,
			"HURRY_POP_ANGER"	: 1,
		}
		# Values to check to see if we need to color the number as neutral
		self.NEUTRAL_VALUES_DICT = {
			"HAPPY"		: 0,
			"HEALTH"	: 0,
			"GROWTH"	: 0,
			"FOOD"		: 0,
		}
		# Values to check to see if we need to color the number as great
		self.GREAT_VALUES_DICT = {
			"HAPPY"		: 6,
			"HEALTH"	: 6,
			"FOOD"		: 8,
		}
		# Values for whom coloring comparison is reversed; i.e. higher numbers are worse
		self.COMPARISON_REVERSED = [
			"HURRY_POP_ANGER",
		]
		# Dictionary of the coloring dictionaries!
		self.COLOR_DICT_DICT = {
			"PROBLEM"	: self.PROBLEM_VALUES_DICT,
			"NEUTRAL"	: self.NEUTRAL_VALUES_DICT,
			"GREAT"		: self.GREAT_VALUES_DICT,
		}
		# This creates the set of ALL coloring keys.
		# Do NOT touch.
		self.COLOR_SET = set()
		for clDict in self.COLOR_DICT_DICT.values():
			self.COLOR_SET.update(clDict.keys())

		global CityScreenOpt, AdvisorOpt
		import BugCore
		CityScreenOpt = BugCore.game.CityScreen
		AdvisorOpt = BugCore.game.CustDomAdv

	def getScreen(self):
		return CyGInterfaceScreen("DomesticAdvisor", self.screenId)

	def interfaceScreen(self):
		screen = self.getScreen()
		if screen.isActive():
			return

		import CvEventInterface
		self.eventManager = eventManager = CvEventInterface.eventManager
		eventManager.OverrideEventApply[5000] = self.renameApply

		if self.bInitialize:
			# Creates Dictionaries we couldn't on init.
			iYellow	= GC.getCOLOR_YELLOW()
			iRed	= GC.getCOLOR_RED()
			iGreen	= GC.getCOLOR_GREEN()
			iBlue	= GC.getCOLOR_BLUE()
			iCyan	= GC.getInfoTypeForString("COLOR_CYAN")
			self.iRed = iRed

			self.HURRY_TYPE_POP = GC.getInfoTypeForString("HURRY_POPULATION")
			self.HURRY_TYPE_GOLD = GC.getInfoTypeForString("HURRY_GOLD")

			# Yield icons
			aList = []
			for i in xrange(YieldTypes.NUM_YIELD_TYPES):
				aList.append(u'%c' % GC.getYieldInfo(i).getChar())
			self.yieldIcons = list(aList)
			# Commerce icons
			aList = []
			for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				aList.append(u'%c' % GC.getCommerceInfo(i).getChar())
			self.commerceIcons = list(aList)

			# Special symbols for building, wonder and project views
			self.objectUnderConstruction = self.yieldIcons[YieldTypes.YIELD_PRODUCTION]

			# add the colors dependant on the statuses
			self.objectHave = TRNSLTR.changeTextColor("x", iGreen) #"x"
			self.objectNotPossible = TRNSLTR.changeTextColor("-", iRed) #"-"
			self.objectPossible = TRNSLTR.changeTextColor("o", iBlue) #"o"
			self.objectNotPossibleConcurrent = TRNSLTR.changeTextColor("-", iYellow) #"-"
			self.objectPossibleConcurrent = TRNSLTR.changeTextColor("o", iYellow) #"o"

			# Corporation Yield and Commerce values by Bonus
			# Maps are { bonus -> { yield/commerce -> { corporation -> value } } }
			self.corpMaintPercent = GC.getWorldInfo(GC.getMap().getWorldSize()).getCorporationMaintenancePercent()
			self.bonusCorpYields = {}
			self.bonusCorpCommerces = {}
			for eCorp in xrange(GC.getNumCorporationInfos()):
				info = GC.getCorporationInfo(eCorp)
				for eBonus in info.getPrereqBonuses():

					for eYield in xrange(YieldTypes.NUM_YIELD_TYPES):
						iYieldValue = info.getYieldProduced(eYield)
						if iYieldValue != 0:
							if not self.bonusCorpYields.has_key(eBonus):
								self.bonusCorpYields[eBonus] = {}
							if not self.bonusCorpYields[eBonus].has_key(eYield):
								self.bonusCorpYields[eBonus][eYield] = {}
							if not self.bonusCorpYields[eBonus][eYield].has_key(eCorp):
								self.bonusCorpYields[eBonus][eYield][eCorp] = iYieldValue

					for eCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						iCommerceValue = info.getCommerceProduced(eCommerce)
						if iCommerceValue != 0:
							if not self.bonusCorpCommerces.has_key(eBonus):
								self.bonusCorpCommerces[eBonus] = {}
							if not self.bonusCorpCommerces[eBonus].has_key(eCommerce):
								self.bonusCorpCommerces[eBonus][eCommerce] = {}
							if not self.bonusCorpCommerces[eBonus][eCommerce].has_key(eCorp):
								self.bonusCorpCommerces[eBonus][eCommerce][eCorp] = iCommerceValue

			# Special Class variables
			aMap = {
				"DRAFT" : TRNSLTR.getText("TXT_KEY_CONCEPT_DRAFT", ()),
				"POP"	: TRNSLTR.getText("TXT_KEY_POPULATION", ()),
			}
			self.COLUMNS_LIST = COLUMNS_LIST = [
# Name						Width	Type	CyCityFunction				selfFunction		Arg				Title
("ADVISE_CULTURE",			150,	"text",	None,						self.advise,		"Culture",		TRNSLTR.getText("TXT_WORD_CULTURE", ())),
("ADVISE_MILITARY",			150,	"text",	None,						self.advise,		"Military",		TRNSLTR.getText("TXT_KEY_ADVISOR_MILITARY", ())),
("ADVISE_RELIGION",			150,	"text",	None,						self.advise,		"Religion",		TRNSLTR.getText("TXT_KEY_CONCEPT_RELIGION", ())),
("ADVISE_RESEARCH",			150,	"text",	None,						self.advise,		"Research",		TRNSLTR.getText("TXT_KEY_COMMERCE_RESEARCH", ())),
("ADVISE_SPACESHIP",		150,	"text",	None,						self.advise,		"Spaceship",	TRNSLTR.getText("TXT_KEY_CONCEPT_SPACESHIP", ())),
("AUTOMATION",				80,		"text",	None,				self.calculateAutomation,	None,			"Auto"),
("CONSCRIPT_ANGER",			40,		"int",	None,			self.calculateConscriptAnger,	None,			"D" + unichr(8851)),
("CONSCRIPT_UNIT",			90,		"text",	None,			self.calculateConscriptUnit,	None,			aMap["DRAFT"]),
("COULD_CONSCRIPT_UNIT",	90,		"text",	None,	self.calculatePotentialConscriptUnit,	None,			aMap["DRAFT"] + "#"),
("CORPORATIONS",			90,		"text",	None,				self.calculateCorporations, None,			TRNSLTR.getText("TXT_KEY_CONCEPT_CORPORATIONS", ())),
("CULTURE",					53,		"int",	None,				self.calculateTotalCulture, None,			self.commerceIcons[CommerceTypes.COMMERCE_CULTURE]),
("CULTURE_TURNS",			40,		"int",	None,				self.calculateCultureTurns, None,			self.commerceIcons[CommerceTypes.COMMERCE_CULTURE] + "T"),
("DEFENSE",					60,		"int",	None,					self.calculateDefense,	None,			unichr(8861)),
("ESPIONAGE_DEF",			60,		"int", "getEspionageDefenseModifier", None,				None,			self.commerceIcons[CommerceTypes.COMMERCE_ESPIONAGE] + "%"),
("FEATURES",				106,	"text",	None,					self.calculateFeatures,	None,			TRNSLTR.getText("TXT_KEY_MISC_FEATURES", ())),
("FOOD",					40,		"int",	"foodDifference",			None,				True,			self.yieldIcons[YieldTypes.YIELD_FOOD]),
("FOUNDED",					90,		"date",	None,					self.calculateFounded,	None,			TRNSLTR.getText("TXT_KEY_DOMESTIC_ADVISOR_FOUNDED", ())),
("FREE_EXPERIENCE_LAND",	40,		"int",	None,			self.calculateFreeExperience,	"L",			unichr(8529)),
("FREE_EXPERIENCE_SEA",		40,		"int",	None,			self.calculateFreeExperience,	"S",			unichr(8530)),
("FREE_EXPERIENCE_AIR",		40,		"int",	None,			self.calculateFreeExperience,	"A",			unichr(8531)),
("GARRISON",				40,		"int", "getMilitaryHappinessUnits",	None,				None,			unichr(8855)),
("GREATPEOPLE",				45,		"int",	"getGreatPeopleProgress",	None,				None,			unichr(8862)),
("GREATPEOPLE_RATE",		40,		"int",	"getGreatPeopleRate",		None,				None,			"&#177 " + unichr(8862)),
("GREATPEOPLE_TURNS",		40,		"int",	None,			self.calculateGreatPeopleTurns,	None,			unichr(8862) + "T"),
("GROWTH",					40,		"int",	None,					self.calculateGrowth,	None,			unichr(8864)),
("HAPPY",					40,		"int",	None,				self.calculateNetHappiness,	None,			unichr(8850)),
("HEALTH",					40,		"int",	None,				self.calculateNetHealth,	None,			unichr(8852)),
("HURRY_GOLD",				40,		"int",	None,			self.calculateHurryGoldCost,	None,			"H" + self.commerceIcons[CommerceTypes.COMMERCE_GOLD]),
("HURRY_POP",				40,		"int",	None,			self.calculateWhipPopulation,	None,			"H" + unichr(8867)),
("HURRY_POP_EXTRA",			40,		"int",	None,	self.calculateWhipOverflowProduction,	None,			"H" + self.yieldIcons[YieldTypes.YIELD_PRODUCTION]),
("HURRY_POP_GOLD",			40,		"int",	None,			self.calculateWhipOverflowGold,	None,			"H" + self.commerceIcons[CommerceTypes.COMMERCE_GOLD]),
("HURRY_POP_ANGER",			40,		"int",	None,				self.calculateWhipAnger,	None,			"H" + unichr(8851)),
("LIBERATE",				40,		"int",	None,						self.canLiberate,	None,			unichr(8871)),
("LOCATION_X",				50,		"int",	"getX",						None,				None,			"X"),
("LOCATION_Y",				50,		"int",	"getY",						None,				None,			"Y"),
("MAINTENANCE",				40,		"int",	"getMaintenance",			None,				None,			unichr(8863)),
("NUM_SPECIALIST_GG",		40,		"int",	None,				self.countFreeSpecialists, "GREAT_GENERAL", unichr(8528)),
("POPULATION",				40,		"int", "getPopulation",				None,				None,			aMap["POP"]),
("POPULATION_REAL",			65,		"int", "getRealPopulation",			None,				None,			aMap["POP"] + "#"),
("POWER",					50,		"text",	None,					self.calculatePower,	None,			unichr(8872)),
("PRODUCING",				90,		"text",	None,				self.calculateProducing,	None,			TRNSLTR.getText("TXT_KEY_DOMESTIC_ADVISOR_PRODUCING", ())),
("PRODUCING_TURNS",			40,		"int",	None,			self.calculateProducingTurns,	None,			self.yieldIcons[YieldTypes.YIELD_PRODUCTION] + "T"),
("PRODUCTION",				40,		"int",	None,				self.calculateProduction,	None,			self.yieldIcons[YieldTypes.YIELD_PRODUCTION]),
("RELIGIONS",				90,		"text",	None,				self.calculateReligions,	None,			TRNSLTR.getText("TXT_KEY_ADVISOR_RELIGION", ())),
("SPECIALISTS",				209,	"text",	None,				self.calculateSpecialists,	None,			TRNSLTR.getText("TXT_KEY_CONCEPT_SPECIALISTS", ())),
("THREATS",					60,		"text",	None,					self.calculateThreats,	None,			"Threats"),
("TRADE",					40,		"int",	None,					self.calculateTrade,	None,			unichr(8860)),
("TRADE_DOMESTIC",			40,		"int",	None,					self.calculateTrade,	"D",			"D" + unichr(8860)),
("TRADE_FOREIGN",			40,		"int",	None,					self.calculateTrade,	"F",			"F" + unichr(8860)),
("TRADE_ROUTES",			40,		"int",	None,					self.countTradeRoutes,	None,			"#" + unichr(8860)),
("TRADE_ROUTES_DOMESTIC",	40,		"int",	None,					self.countTradeRoutes,	"D",			"D#" + unichr(8860)),
("TRADE_ROUTES_FOREIGN",	40,		"int",	None,					self.countTradeRoutes,	"F",			"F#" + unichr(8860)),
("WORLD_WONDERS",			40,		"int", "getNumWorldWonders",		None,				None,			"WW"),
("MAX_WORLD_WONDERS",		40,		"int", "getMaxNumWorldWonders",		None,				None,			"MWW"),
("NATIONAL_WONDERS",		40,		"int", "getNumNationalWonders",		None,				None,			"NW"),
("MAX_NATIONAL_WONDERS",	40,		"int", "getMaxNumNationalWonders",	None,				None,			"MNW")
			]
			# Yield
			aList = [
				"FOOD_",
				"PRODUCTION_",
				"COMMERCE_"
			]
			for i in xrange(YieldTypes.NUM_YIELD_TYPES):
				COLUMNS_LIST.append((aList[i] + "BASE", 40, "int", "getPlotYield", None, i, "B" + self.yieldIcons[i]))
				COLUMNS_LIST.append((aList[i] + "GRANK_BASE", 42, "int", None, self.findGlobalBaseYieldRateRank, i, "B" + self.yieldIcons[i] + "g"))
				COLUMNS_LIST.append((aList[i] + "GRANK", 40, "int", None, self.findGlobalYieldRateRank, i, self.yieldIcons[i] + "g"))
				COLUMNS_LIST.append((aList[i] + "NRANK_BASE", 42, "int", "findBaseYieldRateRank", None, i, "B" + self.yieldIcons[i] + "n"))
				COLUMNS_LIST.append((aList[i] + "NRANK", 40, "int", "findYieldRateRank", None, i, self.yieldIcons[i] + "n"))
			# Commerce
			aList = [
				"GOLD_",
				"RESEARCH_",
				"CULTURE_",
				"ESPIONAGE_"
			]
			for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				COLUMNS_LIST.append((aList[i] + "RATE", 40, "int", "getCommerceRate", None, i, "&#177 " + self.commerceIcons[i]))
				COLUMNS_LIST.append((aList[i] + "GRANK", 40, "int", None, self.findGlobalCommerceRateRank, i, self.commerceIcons[i] + "g"))
				COLUMNS_LIST.append((aList[i] + "NRANK", 40, "int", "findCommerceRateRank", None, i, self.commerceIcons[i] + "n"))
			# Buildings
			self.BUILDING_INFO_LIST = []
			for i in xrange(GC.getNumBuildingInfos()):
				info = GC.getBuildingInfo(i)
				desc = info.getDescription()
				key = self.getBuildingKey(i)
				self.BUILDING_INFO_LIST.append(info)
				COLUMNS_LIST.append((key, 24, "bldg", None, self.getBuildingState, i, desc))
			# Hurry types
			for i in xrange(GC.getNumHurryInfos()):
				header = self.yieldIcons[YieldTypes.YIELD_PRODUCTION]
				name = "CAN_HURRY_"
				info = GC.getHurryInfo(i)
				if info.getGoldPerProduction() > 0:
					header += " + " + self.commerceIcons[CommerceTypes.COMMERCE_GOLD]
					name += "GOLD"
				if info.getProductionPerPopulation() > 0 and info.isAnger():
					header += " + " + unichr(8867)
					name += "WHIP"
				elif info.getProductionPerPopulation() > 0:
					header += " + " + unichr(8850)
					name += "VOLUNTEERS"

				COLUMNS_LIST.append((name, 50, "text", None, self.canHurry, i, header))
			# Resources ("bonuses") -- presence
			for i in xrange(GC.getNumBonusInfos()):
				info = GC.getBonusInfo(i)
				desc = u"%c" % info.getChar()
				key = "HAS_" + self.getBonusKey(i)

				COLUMNS_LIST.append((key, 24, "bonus", None, self.calculateHasBonus, i, desc))
			# Resources ("bonuses") -- effects
			for i in xrange(GC.getNumBonusInfos()):
				info = GC.getBonusInfo(i)
				desc = u"%c" % info.getChar()
				key = self.getBonusKey(i)

				COLUMNS_LIST.append((key, 50, "text", None, self.calculateBonus, i, desc))
			# Properties
			for i in xrange(GC.getNumPropertyInfos()):
				info = GC.getPropertyInfo(i)
				desc = u"%c" % info.getChar()
				key = info.getType()

				COLUMNS_LIST.append((key, 53, "int", None, self.calculateProperty, i, desc))
				COLUMNS_LIST.append((key+"_CHANGE", 53, "int", None, self.calculatePropertyChange, i, "&#177 " + desc))

			self.loadPages()

			self.COLUMNS_INDEX = {}
			self.HEADER_DICT = {}

			for i, column in enumerate(COLUMNS_LIST):
				self.COLUMNS_INDEX[column[0]] = i
				self.HEADER_DICT[column[0]] = column[6]
			# Specialist Icon Information
			self.SPECIALIST_ICON_DICT = {
				0 : unichr(8854), # Citizen
				1 : unichr(8864), # Slave
				2 : unichr(8850), # Priest
				3 : self.commerceIcons[CommerceTypes.COMMERCE_CULTURE], # Artist
				4 : self.commerceIcons[CommerceTypes.COMMERCE_RESEARCH], # Scientist
				5 : self.commerceIcons[CommerceTypes.COMMERCE_GOLD], # Merchant
				6 : self.yieldIcons[YieldTypes.YIELD_PRODUCTION], # Engineer
				7 : unichr(8862), # Noble
				8 : self.commerceIcons[CommerceTypes.COMMERCE_ESPIONAGE], # Spy
			}
			# Automation Information
			self.AUTOMATION_ICON_DICT = {
				0 : self.yieldIcons[YieldTypes.YIELD_FOOD], # Emphasize Food
				1 : self.yieldIcons[YieldTypes.YIELD_PRODUCTION], # Emphasize Production
				2 : self.commerceIcons[CommerceTypes.COMMERCE_GOLD], # Emphasize Gold?
				3 : self.commerceIcons[CommerceTypes.COMMERCE_RESEARCH], # Emphasize Research
				4 : unichr(8862), # Emphasize GP
				5 : unichr(8864), # Emphasize Avoid Growth
			}
			# Colors to highlight with for each type of number
			self.COLOR_DICT = {
				"PROBLEM"	: iRed,
				"NEUTRAL"	: iYellow,
				"GREAT"		: iGreen,
			}
			# Colors to use for color-coding of production items.
			# ["DEFAULT"] is used if color-coding is off.
			iWhite	= GC.getInfoTypeForString("COLOR_WHITE")
			self.PROD_COLOR_DICT = {
				"DEFAULT"	: iWhite,
				"NOTHING"	: iRed,
				"BUILDING"	: iWhite,
				"WONDER"	: iCyan,
				"WEALTH"	: iYellow,
				"RESEARCH"	: iGreen,
				"CULTURE"	: GC.getInfoTypeForString("COLOR_MAGENTA"),
				"PROJECT"	: iCyan,
				"UNIT"		: GC.getInfoTypeForString("COLOR_YIELD_FOOD"),
			}
			self.switchPage(self.PAGES[0]["name"])
			self.bInitialize = False
		# Define screen
		screen.setRenderInterfaceOnly(True)
		# Cached variables
		self.iPlayer = GC.getGame().getActivePlayer()
		self.CyPlayer = CyPlayer = GC.getActivePlayer()
		import InputData
		self.InputData = InputData.instance

		import PythonToolTip
		self.tooltip = PythonToolTip.PythonToolTip()

		self.TXT_NAME = TRNSLTR.getText("TXT_WORD_NAME", ())

		# Determine our size/positions.
		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y
		screen.setDimensions(0, 0, xRes, yRes)

		if yRes > 1000:
			self.H_EDGE = H_EDGE = 38
			szfontEdge = "<font=4b>"
		elif yRes > 800:
			self.H_EDGE = H_EDGE = 32
			szfontEdge = "<font=3b>"
		else:
			self.H_EDGE = H_EDGE = 29
			szfontEdge = "<font=2b>"

		if xRes > 1700:
			self.aFontList = aFontList = [szfontEdge, "<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>"]
		elif xRes > 1400:
			self.aFontList = aFontList = [szfontEdge, "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>"]
		else:
			self.aFontList = aFontList = [szfontEdge, "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0b>", "<font=0>"]

		xMid = xRes / 2
		# Dimension of the table
		self.yTable = yTable = H_EDGE - 8
		self.hTable1 = yRes - H_EDGE - yTable
		self.hTable2 = self.hTable1 - H_EDGE
		self.wTable2 = wTable2 = xMid - 8
		self.xTable2 = xMid + 8

		iSize = H_EDGE - 8

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle	= FontTypes.TITLE_FONT

		screen.addPanel("DomesticAdvBG", "", "", True, False, -8, -8, xRes + 16, yRes + 16, PanelStyles.PANEL_STYLE_MAIN)

		stText = szfontEdge + TRNSLTR.getText("TXT_KEY_DOMESTIC_ADVISOR_TITLE", ())
		screen.setText("DomHeader", "", stText, 1<<2, xMid, 0, 0, eFontTitle, eWidGen, 1, 1)
		stText = szfontEdge + TRNSLTR.getText("TXT_WORD_EXIT", ())
		screen.setText("DomExit", "", stText, 1<<1, xRes - 8, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		# Buttons to switch screens
		x = 326
		y = yRes - iSize - 2
		leftBtn = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_LEFT").getPath()
		screen.setImageButton("DomPagePrevBtn", leftBtn, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_PREV_PAGE, -1, -1)
		x += iSize + 2
		rightBtn = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_RIGHT").getPath()
		screen.setImageButton("DomPageNextBtn", rightBtn, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_NEXT_PAGE, -1, -1)
		x += iSize + 12
		artPath = "Art/Interface/Buttons/general/scroll.dds"
		screen.setImageButton("DomToggleBtn0", artPath, x, y, iSize, iSize, eWidGen, 1, 1)
		x += iSize + 2
		artPath = AFM.getInterfaceArtInfo("INTERFACE_BTN_EVENT_LOG").getPath()
		screen.setImageButton("DomRenameBtn", artPath, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_RENAME_PAGE, -1, -1)
		x += iSize + 2
		plusBtn = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_PLUS").getPath()
		screen.setImageButton("DomAddPageBtn", plusBtn, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_ADD_PAGE, -1, -1)
		x += iSize + 2
		minusBtn = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_MINUS").getPath()
		screen.setImageButton("DomDelPageBtn", minusBtn, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_REMOVE_PAGE, -1, -1)
		x += iSize + 2
		upArrow = AFM.getInterfaceArtInfo("INTERFACE_GENERAL_UPARROW").getPath()
		screen.setImageButton("DomPageUpBtn", upArrow, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_MOVE_PAGE_UP, -1, -1)
		x += iSize + 2
		downArrow = AFM.getInterfaceArtInfo("INTERFACE_GENERAL_DOWNARROW").getPath()
		screen.setImageButton("DomPageDownBtn", downArrow, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_MOVE_PAGE_DOWN, -1, -1)
		x += iSize + 12
		artPath = AFM.getInterfaceArtInfo("INTERFACE_GENERAL_MENU_ICON").getPath()
		screen.setImageButton("DomSaveBtn", artPath, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_SAVE, -1, -1)
		x += iSize + 2
		artPath = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath()
		screen.setImageButton("DomReloadBtn", artPath, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_RELOAD, -1, -1)

		# Draw the customization controls (but don't SHOW them)
		x = wTable2 - iSize - 1
		y -= H_EDGE
		screen.setImageButton("DomColDn", downArrow, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_MOVE_COLUMN_DOWN, -1, -1 )
		x -= iSize + 2
		screen.setImageButton("DomColUp", upArrow, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_MOVE_COLUMN_UP, -1, -1 )
		x -= iSize + 2
		screen.setImageButton("DomDelCol", minusBtn, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_REMOVE_COLUMN, -1, -1 )
		x -= iSize + 2
		screen.setImageButton("DomAddCol", plusBtn, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_ADD_COLUMN, -1, -1 )
		x -= iSize + 2
		screen.setImageButton("DomColWiden", rightBtn, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_EXPAND_COLUMN, -1, -1 )
		x -= iSize + 2
		screen.setImageButton("DomColShrink", leftBtn, x, y, iSize, iSize, WidgetTypes.WIDGET_CDA_SHRINK_COLUMN, -1, -1 )

		if not self.bCustomizing:
			self.hideCustomizationControls(screen)

		# Colony Split
		if CyPlayer.canSplitEmpire():
			bCanLiberate = True
		else:
			bCanLiberate = False
			for CyCity in CyPlayer.cities():
				if CyCity.getLiberationPlayer(False) != -1:
					bCanLiberate = True
					break

		if bCanLiberate:
			screen.setImageButton("DomesticSplit", "", 8, 2, iSize, iSize, WidgetTypes.WIDGET_ACTION, GC.getControlInfo(ControlTypes.CONTROL_FREE_COLONY).getActionInfoIndex(), -1)
			screen.setStyle("DomesticSplit", "Button_HUDAdvisorVictory_Style")

		# Draw the table and the rest based on the mode
		self.drawScreen()

		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

	def hideCustomizationControls(self, screen):

		screen.hide("DomCustomize")
		screen.hide("DomColumnsList")
		screen.hide("DomAddCol")
		screen.hide("DomDelCol")
		screen.hide("DomColUp")
		screen.hide("DomColDn")
		screen.hide("DomColShrink")
		screen.hide("DomColWiden")
		screen.hide("DomRenameBtn")
		screen.hide("DomAddPageBtn")
		screen.hide("DomDelPageBtn")
		screen.hide("DomPageUpBtn")
		screen.hide("DomPageDownBtn")
		screen.hide("DomSaveBtn")
		screen.hide("DomReloadBtn")
		screen.hide("CityNameWidth0")

	# Draw the screen based on which mode we get.
	def drawScreen(self):
		screen = self.getScreen()

		# Change the visible page?
		if not self.bCustomizing and self.visiblePage != self.currentPage:
			if self.visiblePage:
				screen.hide(self.visiblePage)

			screen.show(self.currentPage)
			self.visiblePage = self.currentPage

		# Hide the menu and Civilopedia buttons
		CyInterface().setDirty(InterfaceDirtyBits.MiscButtons_DIRTY_BIT, True)

		# Draw the city list...
		self.drawContents(screen)

	def calculateFounded(self, CyCity, szKey, arg):
		iTurn = CyCity.getGameDateFounded()
		if iTurn < 10000:
			return TRNSLTR.getText("TXT_KEY_TIME_TURN", (iTurn,))

		return str(iTurn)

	def calculateFeatures(self, CyCity, szKey, arg):

		szReturn = ""
		# First look for Government Centers
		if CyCity.isGovernmentCenter():
			if CyCity.isCapital():
				szReturn += unichr(8858)
			else:
				szReturn += unichr(8859)

		# add National Wonders
		for i in xrange(GC.getNumBuildingInfos()):
			info = GC.getBuildingInfo(i)
			if info.getMaxGlobalInstances() == -1 and info.getMaxPlayerInstances() == 1 and CyCity.hasBuilding(i) and not info.isCapital():
				# Use bullets as markers for National Wonders
				szReturn += unichr(8854)

		if CyCity.isDisorder():
			if CyCity.isOccupation():
				szOccu = unichr(8871)
				szReturn += szOccu +":"+unicode(CyCity.getOccupationTimer())
			else:
				szReturn += unichr(8867)

		pTradeCity = CyCity.getTradeCity(0)
		if pTradeCity and pTradeCity.getOwner() >= 0:
			szReturn += unichr(8860)

		return szReturn

	def calculateDefense(self, city, szKey, arg):
		return unicode(city.plot().defenseModifier(-1, True, False)) + u"%"

	def calculateThreats(self, CyCity, szKey, arg):
		iTeam = CyGame().getActiveTeam()
		CyTeam = GC.getTeam(iTeam)

		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
			if iTeamX != iTeam and CyTeam.isAtWarWith(iTeamX) and CyCity.isVisible(iTeamX, False):
				return unichr(8867)
		return ""

	def calculateNetHappiness(self, city, szKey="", arg=0):
		return city.happyLevel() - city.unhappyLevel(0)

	def calculateNetHealth(self, city, szKey="", arg=0):
		return city.goodHealth() - city.badHealth(False)

	def calculateGrowth(self, CyCity, szKey, arg):
		# Turns til Growth
		nFood = CyCity.foodDifference(True)
		# Not enough food, so calculate how many turns until we starve.
		# We put this here because we still starve if building a food production
		if nFood < 0:
			# Use floor divide (//) because we want the number it'll drop below 0
			# (that's also the reason for the additional 1)
			return unicode((CyCity.getFood()+1) // nFood)

		return unicode(CyCity.getFoodTurnsLeft())


	def calculateProduction(self, city, szKey, arg):
		return unicode(city.getCurrentProductionDifference(True, False))

	def calculateMaintenance(self, city, szKey, arg):
		return unicode(city.getMaintenance())

	def calculateTrade(self, CyCity, szKey, arg):
		# arg: None to sum all, 'D' to count domestic, 'F' to count foreign.
		nTotalTradeProfit = 0

		# For each trade route possible
		for nTradeRoute in xrange(CyCity.getMaxTradeRoutes()):
			# Get the next trade city
			pTradeCity = CyCity.getTradeCity(nTradeRoute)
			# Not quite sure what this does but it's in the MainInterface
			if pTradeCity and pTradeCity.getOwner() > -1:
				bForeign = CyCity.getOwner() != pTradeCity.getOwner()
				if not arg or ((arg == "F" and bForeign) or (arg == "D" and not bForeign)):
					iTradeProfit = CyCity.calculateTradeYield(YieldTypes.YIELD_COMMERCE, CyCity.calculateTradeProfitTimes100(CyCity.getTradeCity(nTradeRoute)))
					nTotalTradeProfit += iTradeProfit

		nTotalTradeProfit //= 100
		if nTotalTradeProfit < 0:
			return "-%d" % nTotalTradeProfit

		return "%d" % nTotalTradeProfit


	def countTradeRoutes(self, city, szKey, arg):
		# arg: None to count all, 'D' to count domestic, 'F' to count foreign.
		nRoutes = 0

		# For each trade route possible
		for nTradeRoute in xrange(city.getMaxTradeRoutes()):
			# Get the next trade city
			pTradeCity = city.getTradeCity(nTradeRoute)
			# Not quite sure what this does but it's in the MainInterface
			# and I pretty much C&Ped :p
			if pTradeCity and pTradeCity.getOwner() >= 0:
				bForeign = city.getOwner() != pTradeCity.getOwner()
				if not arg or ((arg == "F" and bForeign) or (arg == "D" and not bForeign)):
					nRoutes += 1

		return unicode(nRoutes)

	def countFreeSpecialists(self, city, szKey, arg):
		# arg: specialist type string (excluding "SPECIALIST_") e.g. use "ARTIST" to count artist specialists.
		szSpecialistType = "SPECIALIST_" + arg
		nCount = city.getFreeSpecialistCount(GC.getInfoTypeForString(szSpecialistType))
		if nCount > 0:
			return unicode(nCount)
		return "-"

	def calculateProducing(self, city, szKey, arg):

		bProdColors = AdvisorOpt.isProductionGroupingColors()
		bProdIcons = AdvisorOpt.isProductionGroupingIcons()

		# If there's something in the queue,
		if city.getOrderQueueLength() > 0:
			# Get the name of whatever it's producing.
			szReturn = city.getProductionName()

			# Note this has been separated out so that it is always in effect
			# even if user chooses to disable the production coloring/grouping
			if city.isProductionUnit():
				iUnit = city.getProductionUnit()
				pInfo = GC.getUnitInfo(iUnit)
				if pInfo.getUnitCombatType() != UnitCombatTypes.NO_UNITCOMBAT:
					iExp = city.getProductionExperience(iUnit)
					szReturn = szReturn + u" " + TRNSLTR.getText("TXT_KEY_CDA_BASE_XP", (iExp,))

			if bProdColors or bProdIcons:
				szColorKey = "DEFAULT"
				szIcon = self.yieldIcons[YieldTypes.YIELD_PRODUCTION]
				if city.isProductionBuilding():
					szColorKey = "WONDER"
					pInfo = GC.getBuildingInfo(city.getProductionBuilding())
					if pInfo.getMaxGlobalInstances() != -1:
						szIcon = unichr(8858)
					elif pInfo.getMaxTeamInstances() != -1:
						szIcon = unichr(8859)
					elif pInfo.getMaxPlayerInstances() != -1:
						szIcon = unichr(8859)
					else:
						szColorKey = "BUILDING"
				elif city.isProductionProcess():
					iType = city.getProductionProcess()
					if iType == GC.getInfoTypeForString("PROCESS_WEALTH"):
						szColorKey = "WEALTH"
						szIcon = self.commerceIcons[CommerceTypes.COMMERCE_GOLD]
					elif iType == GC.getInfoTypeForString("PROCESS_RESEARCH"):
						szColorKey = "RESEARCH"
						szIcon = self.commerceIcons[CommerceTypes.COMMERCE_RESEARCH]
					elif iType == GC.getInfoTypeForString("PROCESS_CULTURE"):
						szColorKey = "CULTURE"
						szIcon = self.commerceIcons[CommerceTypes.COMMERCE_CULTURE]
				elif city.isProductionProject():
					szColorKey = "PROJECT"
					pInfo = GC.getProjectInfo(city.getProductionProject())
					if pInfo.getMaxGlobalInstances() != -1:
						szIcon = unichr(8858)
					elif pInfo.getMaxTeamInstances() != -1:
						szIcon = unichr(8859)
				elif city.isProductionUnit():
					szColorKey = "UNIT"
					iUnit = city.getProductionUnit()
					pInfo = GC.getUnitInfo(iUnit)
					iType = pInfo.getDomainType()
					if pInfo.getUnitCombatType() != UnitCombatTypes.NO_UNITCOMBAT:
						szIcon = unichr(8855)
				else:
					szIcon = unichr(8856)
				if bProdIcons:
					szReturn = szIcon + szReturn
				if bProdColors:
					szReturn = TRNSLTR.changeTextColor(szReturn, self.PROD_COLOR_DICT[szColorKey])

		# Otherwise we're not producing anything. Leave it blank.
		else:
			szReturn = "-"

			if bProdIcons:
				szReturn = unichr(8535) + szReturn
			if bProdColors:
				szReturn = TRNSLTR.changeTextColor(szReturn, self.PROD_COLOR_DICT["NOTHING"])

		return szReturn

	def calculateProducingTurns(self, city, szKey, arg):
		szReturn = u""
		# If there's something in the queue,
		if city.getOrderQueueLength() > 0:
			# If it's not a process (i.e., Wealth, Research, Culture)
			if not city.isProductionProcess():
				szReturn = unicode(city.getProductionTurnsLeft())
		return szReturn

	def calculateWhipPopulation(self, city, szKey, arg):
		if city.canHurry(self.HURRY_TYPE_POP, False):
			return unicode(city.hurryPopulation(self.HURRY_TYPE_POP))

		return self.objectNotPossible

	def calculateWhipOverflowProduction(self, city, szKey, arg):
		return self.calculateWhipOverflow(city, szKey, arg)[0]

	def calculateWhipOverflowGold(self, city, szKey, arg):
		return self.calculateWhipOverflow(city, szKey, arg)[1]

	def calculateWhipOverflow(self, city, szKey, arg):

		if not city.canHurry(self.HURRY_TYPE_POP, False):
			return self.objectNotPossible, self.objectNotPossible

		iOverflow = city.hurryProduction(self.HURRY_TYPE_POP) - city.productionLeft()
		if CityScreenOpt.isWhipAssistOverflowCountCurrentProduction():
			iOverflow += city.getCurrentProductionDifference(True, False)
		iMaxOverflow = city.getMaxProductionOverflow()
		iOverflowGold = max(0, iOverflow - iMaxOverflow) * GC.getDefineINT("MAXED_UNIT_GOLD_PERCENT") / 100
		iOverflow = 100 * iMaxOverflow / city.getBaseYieldRateModifier(YieldTypes.YIELD_PRODUCTION, 0)
		return unicode(iOverflow), unicode(iOverflowGold)

	def calculateWhipAnger(self, city, szKey, arg):
		iAnger = city.getHurryAngerTimer()
		if iAnger > 0 or city.canHurry(self.HURRY_TYPE_POP, False):
			return iAnger

		return self.objectNotPossible

	def calculateHurryGoldCost(self, city, szKey, arg):
		if city.canHurry(self.HURRY_TYPE_GOLD, False):
			return unicode(city.getHurryGold(self.HURRY_TYPE_GOLD))

		return self.objectNotPossible

	def calculateConscriptAnger(self, city, szKey, arg):
		iAnger = city.getConscriptAngerTimer()
		if iAnger > 0 or city.canConscript():
			return iAnger

		return self.objectNotPossible

	def calculatePotentialConscriptUnit(self, city, szKey, arg):
		szReturn = unicode(GC.getUnitInfo(city.getConscriptUnit()).getDescription() )
		return szReturn

	def calculateConscriptUnit(self, city, szKey, arg):
		if city.canConscript():
			return unicode(GC.getUnitInfo(city.getConscriptUnit()).getDescription())
		return u""

	def calculateReligions(self, city, szKey, arg):

		szReturn = u""

		lHolyCity = []
		lReligions = []
		for i in xrange(GC.getNumReligionInfos()):
			if city.isHolyCityByType(i):
				lHolyCity.append(i)
			elif city.isHasReligion(i):
				lReligions.append(i)

		for i in xrange(len(lHolyCity)):
			szReturn += u"%c" %(GC.getReligionInfo(lHolyCity[i]).getHolyCityChar())

		for i in xrange(len(lReligions)):
			szReturn += u"%c" %(GC.getReligionInfo(lReligions[i]).getChar())

		return szReturn

	def calculateCorporations(self, city, szKey, arg):
		szReturn = u""
		lHeadquarters = []
		lCorps = []
		for i in xrange(GC.getNumCorporationInfos()):
			if city.isHeadquartersByType(i):
				lHeadquarters.append(i)
			elif city.isHasCorporation(i):
				lCorps.append(i)

		for i in xrange(len(lHeadquarters)):
			szReturn += u"%c" %(GC.getCorporationInfo(lHeadquarters[i]).getHeadquarterChar())

		for i in xrange(len(lCorps)):
			szReturn += u"%c" %(GC.getCorporationInfo(lCorps[i]).getChar())

		return szReturn

	def calculateSpecialists(self, city, szKey, arg):
		szReturn = u"<font=1>"
		# For each specialist type
		for i in xrange(len(self.SPECIALIST_ICON_DICT)):

			nCount = city.getSpecialistCount (i)
			# If more than one specialist
			if nCount > 1:
				szReturn += self.SPECIALIST_ICON_DICT[i] + "x" + str(nCount) + " "
			elif nCount == 1:
				szReturn += self.SPECIALIST_ICON_DICT[i] + " "

		szReturn += u"</font>"
		return szReturn

	def calculateAutomation(self, city, szKey, arg):
		szReturn = u"<font=1>"
		# Get the number of emphasis types (that WE deal with).
		nNumEmphasize = len(self.AUTOMATION_ICON_DICT)
		if city.isCitizensAutomated():
			szReturn += u"C"
		if city.isProductionAutomated():
			szReturn += u"P"
		for i in xrange(nNumEmphasize):
			nNum = nNumEmphasize - i - 1
			if city.AI_isEmphasize(nNum):
				szReturn += self.AUTOMATION_ICON_DICT[nNum]

		szReturn += u"</font>"

		return szReturn

	def calculatePower(self, city, szKey, arg):
		szReturn = u""
		if city.isPower():
			szReturn += unichr(8872)
		return szReturn

	def calculateTotalCulture(self, CyCity, szKey, arg):
		return CyCity.getCulture(self.iPlayer)

	def calculateCultureTurns(self, city, szKey, arg):

		iCultureRateTimes100 = city.getCommerceRateTimes100(CommerceTypes.COMMERCE_CULTURE)

		if iCultureRateTimes100 > 0 and city.getCultureThreshold() > 0:

			return (100*city.getCultureThreshold() - city.getCultureTimes100(self.iPlayer) + iCultureRateTimes100 - 1) / iCultureRateTimes100

		return u"-"

	def calculateGreatPeopleTurns(self, city, szKey, arg):

		iGreatPersonRate = city.getGreatPeopleRate()
		if iGreatPersonRate > 0:
			iGPPLeft = self.CyPlayer.greatPeopleThresholdNonMilitary() - city.getGreatPeopleProgress()
			return (iGPPLeft + iGreatPersonRate - 1) / iGreatPersonRate
		return u"-"

	def canHurry(self, city, szKey, arg):
		if city.canHurry(arg, False):
			return self.objectHave
		return self.objectNotPossible

	def canLiberate(self, city, szKey, arg):
		if city.getLiberationPlayer(False) != -1:
			return self.objectHave
		return self.objectNotPossible

	def getBuildingState(self, CyCity, szKey, arg):

		if CyCity.hasBuilding(arg):
			return self.objectHave
		if CyCity.getFirstBuildingOrder(arg) != -1:
			return self.objectUnderConstruction
		if CyCity.canConstruct(arg, False, False, False):
			return self.objectPossible
		if CyCity.canConstruct(arg, True, False, False):
			return self.objectPossibleConcurrent
		return self.objectNotPossible

	def calculateProperty(self, city, szKey, arg):
		return city.getProperties().getValueByProperty(arg)

	def calculatePropertyChange(self, city, szKey, arg):
		return city.getProperties().getChangeByProperty(arg)

	def calculateHasBonus (self, city, szKey, arg):

		# Determine whether or not city has the given bonus
		if city.hasBonus(arg):
			return self.objectHave

		return self.objectNotPossible

	def calculateBonus(self, city, szKey, arg):

		# Determine the effects of the given bonus (health, happiness, commerce)
		if not city.hasBonus(arg):
			return self.objectNotPossible

		szEffects = ""
		iEffect = city.getBonusHappiness(arg)
		if iEffect == 1:
			szEffects += "%s " % (unichr(8850))
		elif iEffect > 1:
			szEffects += "%d%s " % (iEffect, unichr(8850))
		elif iEffect < 0:
			szEffects += "%d%s " % (-iEffect, unichr(8851))

		iEffect = city.getBonusHealth(arg)
		if iEffect == 1:
			szEffects += "%s " % (unichr(8852))
		elif iEffect > 1:
			szEffects += "%d%s " % (iEffect, unichr(8852))
		elif iEffect < 0:
			szEffects += "%d%s " % (-iEffect, unichr(8853))

		for eYieldType in xrange(YieldTypes.NUM_YIELD_TYPES):
			iEffect = city.getBonusYieldRateModifier(eYieldType, arg)
			if iEffect > 0:
				szEffects += "%s " % self.yieldIcons[eYieldType]

		iNumBonuses = city.getNumBonuses(arg)
		if self.bonusCorpYields.has_key(arg):
			yields = self.bonusCorpYields[arg]
			for eYield in xrange(YieldTypes.NUM_YIELD_TYPES):
				if yields.has_key(eYield):
					iEffect = 0
					for eCorp, iValue in yields[eYield].iteritems():
						if city.isActiveCorporation(eCorp):
							iEffect += iValue * iNumBonuses * self.corpMaintPercent / 100
					iEffect = (iEffect + 99) / 100
					if iEffect == 1:
						szEffects += "%s " % self.yieldIcons[eYield]
					elif iEffect:
						szEffects += "%d%s " % (iEffect, self.yieldIcons[eYield])

		if self.bonusCorpCommerces.has_key(arg):
			commerces = self.bonusCorpCommerces[arg]
			for eCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if commerces.has_key(eCommerce):
					iEffect = 0
					for eCorp, iValue in commerces[eCommerce].iteritems():
						if city.isActiveCorporation(eCorp):
							iEffect += iValue * iNumBonuses * self.corpMaintPercent / 100
					iEffect = (iEffect + 99) / 100
					if iEffect == 1:
						szEffects += "%s " % self.commerceIcons[eCommerce]
					elif iEffect > 1 or iEffect < 0:
						szEffects += "%d%s " % (iEffect, self.commerceIcons[eCommerce])

		iEffect = city.getBonusPower(arg)
		if iEffect == 1:
			szEffects += "%s " %(unichr(8872))
		elif iEffect > 1:
			szEffects += "%d%s " % (iEffect, unichr(8872))

		if not szEffects:
			return self.objectHave
		return szEffects.strip()

	def calculateFreeExperience(self, CyCity, szKey, arg):
		# arg: Domain identifier; "L" for Land, "S" for Sea, "A" for Air, "I" for Immobile.
		CyPlayer = self.CyPlayer
		# Generic modifiers first
		freeXP = CyCity.getSpecialistFreeExperience()
		freeXP += CyCity.getFreeExperience()
		freeXP += CyPlayer.getFreeExperience()
		iStateRel = CyPlayer.getStateReligion()
		if iStateRel > -1 and CyCity.isHasReligion(iStateRel):
			freeXP += CyPlayer.getStateReligionFreeExperience()
		# Then tack on the proper domain-based extras
		if arg == "L":
			freeXP += CyCity.getDomainFreeExperience(DomainTypes.DOMAIN_LAND)
		elif arg == "S":
			freeXP += CyCity.getDomainFreeExperience(DomainTypes.DOMAIN_SEA)
		elif arg == "A":
			freeXP += CyCity.getDomainFreeExperience(DomainTypes.DOMAIN_AIR)
		elif arg == "I":
			freeXP += CyCity.getDomainFreeExperience(DomainTypes.DOMAIN_IMMOBILE)

		return unicode(freeXP)

	def findGlobalBaseYieldRateRank(self, CyCity, szKey, arg):

		y = CyCity.getPlotYield(arg)
		aList = []
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			for CyCity in GC.getPlayer(iPlayerX).cities():
				aList.append(CyCity.getPlotYield(arg))

		return len([i for i in aList if i > y]) + 1

	def findGlobalYieldRateRank(self, CyCity, szKey, arg):

		y = CyCity.getYieldRate(arg)
		aList = []
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			for CyCity in GC.getPlayer(iPlayerX).cities():
				aList.append(CyCity.getYieldRate(arg))

		return len([i for i in aList if i > y]) + 1

	def findGlobalCommerceRateRank(self, CyCity, szKey, arg):

		y = CyCity.getCommerceRate(arg)
		aList = []
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			for CyCity in GC.getPlayer(iPlayerX).cities():
				aList.append(CyCity.getCommerceRate(arg))

		return len([i for i in aList if i > y]) + 1


	def canAdviseToConstruct(self, CyCity, i):
		if not CyCity.canConstruct(i, True, False, False):
			return False
		info = GC.getBuildingInfo(i)
		if info.isGovernmentCenter() or info.isCapital():
			return False

		return True

	def advise(self, CyCity, szKey, type):
		bestOrder = -1
		bestData = 0.0

		# For all cities, start with growth
		if self.calculateNetHappiness(CyCity) > 2 and self.calculateNetHealth(CyCity) > 2:
			for iType in xrange(GC.getNumBuildingInfos()):
				if self.canAdviseToConstruct(CyCity, iType):
					info = GC.getBuildingInfo(iType)
					iCost = float(info.getProductionCost())
					if iCost > 0:
						value = info.getFoodKept() / iCost
						if value > bestData:
							bestOrder = iType
							bestData = value

		# then balancing health and happiness for further growth
		if bestOrder == -1:
			for iType in xrange(GC.getNumBuildingInfos()):
				if self.canAdviseToConstruct(CyCity, iType):
					info = GC.getBuildingInfo(iType)
					if self.calculateNetHappiness(CyCity) < 3 and self.calculateNetHappiness(CyCity) - self.calculateNetHealth(CyCity) > 2:
						iHealth = info.getHealth()
						for eBonus, iNumHealth in info.getBonusHealthChanges():
							if CyCity.hasBonus(eBonus):
								iHealth += iNumHealth
						value = iHealth / float(info.getProductionCost())
						if value > bestData:
							bestOrder = iType
							bestData = value
					elif self.calculateNetHealth(CyCity) < 3 and self.calculateNetHealth(CyCity) - self.calculateNetHappiness(CyCity) > 2:
						iHappiness = info.getHappiness()
						for eBonus, iNumHappiness in info.getBonusHappinessChanges():
							if CyCity.hasBonus(eBonus):
								iHappiness += iNumHappiness
						value = iHappiness  / float(info.getProductionCost())
						if value > bestData:
							bestOrder = iType
							bestData = value

		# First pass
		if bestOrder == -1:
			CyPlayer = self.CyPlayer
			for iType in xrange(GC.getNumBuildingInfos()):
				if self.canAdviseToConstruct(CyCity, iType):
					info = GC.getBuildingInfo(iType)

					if type == "Culture":
						if CyCity.findBaseYieldRateRank(YieldTypes.YIELD_COMMERCE) < 6:
							value = info.getCommerceModifier(CommerceTypes.COMMERCE_CULTURE) / float(info.getProductionCost())
							if value > bestData:
								bestOrder = iType
								bestData = value
						else:
							value = info.getPowerValue() / float(info.getProductionCost())
							if value > bestData:
								bestOrder = iType
								bestData = value
					elif type == "Military":
						value = info.getPowerValue() / float(info.getProductionCost())
						if value > bestData:
							bestOrder = iType
							bestData = value
					elif type == "Religion":
						bestOrder = -1
					elif type == "Research":
						value = info.getCommerceModifier(CommerceTypes.COMMERCE_RESEARCH) / float(info.getProductionCost())
						if value > bestData:
							bestOrder = iType
							bestData = value
					elif type == "Spaceship":
						if not CyCity.isPower():
							if info.isPower():
								value = CyCity.getPlotYield(YieldTypes.YIELD_PRODUCTION) / float(info.getProductionCost())
								if value > bestData:
									bestOrder = iType
									bestData = value

						if CyCity.findBaseYieldRateRank(YieldTypes.YIELD_PRODUCTION) < 12:
							value = CyCity.getPlotYield(YieldTypes.YIELD_PRODUCTION) * 2 * info.getYieldModifier(YieldTypes.YIELD_PRODUCTION) / float(info.getProductionCost())
							if value > bestData:
								bestOrder = iType
								bestData = value

						if CyCity.findBaseYieldRateRank(YieldTypes.YIELD_COMMERCE) < CyPlayer.getNumCities() / 2:
							value = CyCity.getPlotYield(YieldTypes.YIELD_COMMERCE) * info.getCommerceModifier(CommerceTypes.COMMERCE_RESEARCH) / float(info.getProductionCost())
							if value > bestData:
								bestOrder = iType
								bestData = value
						else:
							bestOrder = -1

		# Second pass
		if bestOrder == -1:
			for iType in xrange(GC.getNumBuildingInfos()):
				if self.canAdviseToConstruct(CyCity, iType):
					info = GC.getBuildingInfo(iType)

					if type == "Culture":
						if CyCity.findBaseYieldRateRank(YieldTypes.YIELD_COMMERCE) < 6:
							value = info.getPowerValue() / float(info.getProductionCost())
							if value > bestData:
								bestOrder = iType
								bestData = value
						else: # In a cultural game, build units in the culturally weak cities
							bestOrder = -1
					elif type == "Military":
						if CyCity.findBaseYieldRateRank(YieldTypes.YIELD_PRODUCTION) <= 3:
							value = -1 * info.getMilitaryProductionModifier() / float(info.getProductionCost())
							print info.getDescription() + ": " + str(value)
							if value > bestData:
								bestOrder = iType
								bestData = value
						else:
							value = info.getYieldModifier(YieldTypes.YIELD_PRODUCTION) / float(info.getProductionCost())
							if value > bestData:
								bestOrder = iType
								bestData = value
							if not CyCity.isPower():
								if info.isPower():
									value = 1 / float(info.getProductionCost())
									if value > bestData:
										bestOrder = iType
										bestData = value
					elif type == "Religion":
						bestOrder = -1
					elif type == "Research":
						value = info.getCommerceModifier(CommerceTypes.COMMERCE_GOLD) / float(info.getProductionCost())
						if value > bestData:
							bestOrder = iType
							bestData = value
					elif type == "Spaceship":
						bestOrder = -1

		szReturn = u""
		if bestOrder != -1:
			szReturn = GC.getBuildingInfo(bestOrder).getDescription()

		if szReturn == CyCity.getProductionName():
			szReturn = u""
		return szReturn

	def ColorCityValues(self, nValue, szKey):
		# Colors city values which we might want to alert the user to.

		# If the key is one we want to possibly color
		if szKey in self.COLOR_SET:
			# For each type of comparison
			for szCompareType, clDict in self.COLOR_DICT_DICT.iteritems():
				# Get the color we will use.
				color = self.COLOR_DICT[szCompareType]

				# If the dictionary has the key...
				if clDict != None and clDict.has_key(szKey):
					if szKey in self.COMPARISON_REVERSED:
						# ...and the comparison is appropriate...
						if szCompareType == "PROBLEM" and int(nValue) >= clDict[szKey] or szCompareType == "NEUTRAL" and int(nValue) == clDict[szKey] or szCompareType == "GREAT" and int(nValue) <= clDict[szKey]:
							# ...color and return it
							return TRNSLTR.changeTextColor(nValue, color)
					# ...and the comparison is appropriate...
					elif szCompareType == "PROBLEM" and int(nValue) <= clDict[szKey] or szCompareType == "NEUTRAL" and int(nValue) == clDict[szKey] or szCompareType == "GREAT" and int(nValue) >= clDict[szKey]:
						# ...color and return it
						return TRNSLTR.changeTextColor(nValue, color)
		# Otherwise, just return the regular value
		return nValue

	def drawContents(self, screen):
		PAGE = self.currentPage
		iPage = self.iPage
		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2 = self.aFontList
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT

		self.cityList = cityList = []
		CyPlayer = self.CyPlayer
		for CyCity in CyPlayer.cities():
			cityList.append(CyCity)

		# Hide building icons
		for i in xrange(GC.getNumBuildingInfos()):
			szName = "BLDG_BTN_%d" % i
			screen.hide(szName)

		# Fill the pages drop down
		screen.addDropDownBoxGFC("DomPagesDD", 4, self.yRes - 32, 300, WidgetTypes.WIDGET_CDA_SELECT_PAGE, -1, -1, eFontGame)
		for i, p in enumerate(self.PAGES):
			screen.addPullDownString("DomPagesDD", p["name"], i, i, i == iPage)

		if self.bCustomizing:
			stText = "City name width: %d" %self.PAGES[iPage]["cityName"]
			screen.setText("CityNameWidth0", "", stText, 1<<0, 8, self.yRes - 64, 0, eFontGame, eWidGen, 1, 1)
			# Build the page definition table
			Tbl = "DomCustomize"
			wTbl = self.wTable2
			screen.addTableControlGFC(Tbl, 4, -1, self.yTable, wTbl, self.hTable2, True, False, 32, 32, TableStyles.TABLE_STYLE_STANDARD)
			screen.enableSelect(Tbl, True)
			screen.setStyle(Tbl, "Table_StandardCiv_Style")

			wCol = (wTbl - 120)/2
			screen.setTableColumnHeader(Tbl, 0, uFont2b + "Pos.", 40)
			screen.setTableColumnHeader(Tbl, 1, uFont2b + self.TXT_NAME, wCol)
			screen.setTableColumnHeader(Tbl, 2, uFont2b + "Title", wCol)
			screen.setTableColumnHeader(Tbl, 3, uFont2b + "Width", 80)

			columns = self.PAGES[iPage]["columns"]
			for i, column in enumerate(columns):
				screen.appendTableRow (Tbl)
				screen.setTableInt(Tbl, 0, i, str(i+1), "", eWidGen, 1, 1, 1<<1)
				screen.setTableText(Tbl, 1, i, str(column[0]), "", eWidGen, 1, 1, 1<<0)

				# Catch exceptions generated by missing columns
				try:
					screen.setTableText(Tbl, 2, i, uFont2 + self.HEADER_DICT[column[0]], "", eWidGen, 1, 1, 1<<0)
				except:
					screen.setTableText(Tbl, 2, i, "", "", eWidGen, 1, 1, 1<<0)

				screen.setTableInt(Tbl, 3, i, str(column[1]), "", eWidGen, 1, 1, 1<<0)

			# Build the available columns table
			Tbl = "DomColumnsList"
			if screen.getTableNumRows(Tbl) != len(self.COLUMNS_LIST):

				screen.addTableControlGFC(Tbl, 4, self.xTable2, self.yTable, wTbl, self.hTable1, True, False, 32, 32, TableStyles.TABLE_STYLE_STANDARD)
				screen.enableSelect(Tbl, True)
				screen.enableSort(Tbl)
				screen.setStyle(Tbl, "Table_StandardCiv_Style")

				wCol = (wTbl - 160)/2
				screen.setTableColumnHeader(Tbl, 0, uFont2b + "ID", 80)
				screen.setTableColumnHeader(Tbl, 1, uFont2b + self.TXT_NAME, wCol)
				screen.setTableColumnHeader(Tbl, 2, uFont2b + "Title", wCol)
				screen.setTableColumnHeader(Tbl, 3, uFont2b + "Width", 80)

				columns = self.COLUMNS_LIST
				for i, column in enumerate(columns):
					screen.appendTableRow(Tbl)
					screen.setTableInt(Tbl, 0, i, unicode(i), "", eWidGen, 1, 1, 1<<1)
					screen.setTableText(Tbl, 1, i, unicode(column[0]), "", eWidGen, 1, 1, 1<<0)
					try:
						screen.setTableText(Tbl, 2, i, uFont2 + self.HEADER_DICT[column[0]], "", eWidGen, 1, 1, 1<<0)
					except:
						screen.setTableText(Tbl, 2, i, "", "", eWidGen, 1, 1, 1<<0)

					screen.setTableInt(Tbl, 3, i, unicode(column[1]), "", eWidGen, 1, 1, 1<<0)

			# Finally, display the customization controls
			screen.show(Tbl)

			# And hide the regular controls and legends
			screen.hide(self.currentPage)

		# If displaying the normal advisor screen (not the customization interface)
		else:
			dDict = self.columnDict

			# Build the table
			screen.addTableControlGFC(PAGE, len(dDict) + 2, -1, self.yTable, self.xRes + 2, self.hTable1, True, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
			screen.enableSort(PAGE)
			screen.setStyle(PAGE, "Table_StandardCiv_Style")

			cityRange = range(len(cityList))

			zoomArt = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION").getPath()

			# Add a blank header for the "zoom" column
			wName = self.PAGES[iPage]["cityName"]
			screen.setTableColumnHeader(PAGE, 0, "", 30)
			screen.setTableColumnHeader(PAGE, 1, self.TXT_NAME, wName)

			# Add blank rows to the table
			iPlayer = self.iPlayer
			for i in cityRange:
				screen.appendTableRow(PAGE)
				iCityID = cityList[i].getID()
				screen.setTableText(PAGE, 0, i, "", zoomArt, WidgetTypes.WIDGET_ZOOM_CITY, iPlayer, iCityID, 1<<0)
				screen.setTableText(PAGE, 1, i, cityList[i].getName(), "", eWidGen, 1, 1, 1<<0)

			# Order the columns
			columns = []
			for key, value in self.columnDict.iteritems():
				columns.append((value, key))
			columns.sort()

			iBuildingButtonX = 30 + wName
			iBuildingButtonY = self.yTable

			# Loop through the columns first. This is unintuitive, but faster.
			for value, key in columns:
				try:
					columnDef = self.COLUMNS_LIST[self.COLUMNS_INDEX[key]]
					type = columnDef[2]
					if type == "bldg":
						building = columnDef[5]
						buildingInfo = self.BUILDING_INFO_LIST[building]
						screen.setTableColumnHeader(PAGE, value + 2, "", self.columnWidth[key])
						szName = "BLDG_BTN_%d" % building
						x = iBuildingButtonX + (self.columnWidth[key] - 24) / 2
						screen.setImageButton(szName, buildingInfo.getButton(), x, iBuildingButtonY, 24, 24, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, building, -1)
					else:
						screen.setTableColumnHeader(PAGE, value + 2, "<font=2>" + self.HEADER_DICT[key], self.columnWidth[key])

					iBuildingButtonX += self.columnWidth[key]

					# And the correct writing function.
					if type == "text":
						funcTableWrite = screen.setTableText
						justify = 1<<0
					elif type == "date":
						funcTableWrite = screen.setTableDate
						justify = 1<<0
					elif type == "int":
						funcTableWrite = screen.setTableInt
						justify = 1<<1
					elif type in ("bonus", "bldg"):
						funcTableWrite = screen.setTableText
						justify = 1<<2
					else:
						return

					if columnDef[3]:
						expr = "CyCity." + columnDef[3] + "("
						if columnDef[5] is not None:
							expr += str(columnDef[5])
						expr += ")"
						for i in cityRange:
							CyCity = cityList[i]
							szValue = self.ColorCityValues(unicode(eval(expr, globals(), locals())), key)
							funcTableWrite(PAGE, value + 2, i, szValue, "", eWidGen, 1, 1, justify)
					else:
						calcFunc = columnDef[4]
						for i in cityRange:
							szValue = self.ColorCityValues(unicode(calcFunc(cityList[i], key, columnDef[5])), key)
							funcTableWrite(PAGE, value + 2, i, szValue, "", eWidGen, 1, 1, justify)
				except:
					print ("draw table failure!", value, key)


	def switchPage(self, page):
		for i, p in enumerate(self.PAGES):
			if p["name"] == page:
				self.currentPage = "CDA_PAGE_%d" % i
				self.iPage = i
				self.columnDict = {}
				self.columnWidth = {}
				for i, col in enumerate(p["columns"]):
					self.columnDict[col[0]] = i
					self.columnWidth[col[0]] = col[1]
				break

	def save(self):
		import SystemPaths as SP
		aPath = SP.modDir + "\UserSettings\DomesticAdv\DomesticAdv.txt"
		file = open(aPath, 'w')
		cPickle.dump({"pages" : self.PAGES}, file)
		file.close()

	def getSortedColumnSelection(self, screen):
		list = []
		for i in xrange(len(self.PAGES[self.iPage]["columns"])):
			if screen.isRowSelected("DomCustomize", i):
				list.append(int(screen.getTableText("DomCustomize", 0, i)) - 1)
		list.sort()
		return list

	def getReverseSortedColumnSelection(self, screen):
		list = self.getSortedColumnSelection(screen)
		list.reverse()
		return list

	def customizingSaveSelection(self, screen):
		self.customizingSelection = []
		for i in xrange(len(self.PAGES[self.iPage]["columns"])):
			if screen.isRowSelected("DomCustomize", i):
				self.customizingSelection.append(screen.getTableText("DomCustomize", 1, i))

	def customizingRestoreSelection(self, screen):
		for i in xrange(len(self.PAGES[self.iPage]["columns"])):
			if screen.getTableText("DomCustomize", 1, i) in self.customizingSelection:
				screen.selectRow("DomCustomize", i, True)

	def getBuildingKey(self, index):
		info = GC.getBuildingInfo(index)
		desc = info.getType()
		key = "_"
		key = key.join(desc.split())
		key = key.upper()
		return key

	def getBonusKey(self, index):
		info = GC.getBonusInfo(index)
		type = info.getType()
		key = "_"
		key = key.join(type.split())
		key = key.upper()
		return key

	def loadPages(self):
		import SystemPaths as SP
		path = SP.modDir + "\UserSettings\DomesticAdv\DomesticAdv.txt"
		self.PAGES = {}

		if SP.isFile(path):

			file = open(path, 'r')
			dict = cPickle.load(file)
			file.close()

			if "pages" in dict:
				self.PAGES = dict["pages"]

		if not self.PAGES:
			# Default pages
			self.PAGES = [
{
	"name" : "Executive Summary",
	"cityName" : 120,
	"columns" : [
		("AUTOMATION", 80, "text"),
		("FEATURES", 92, "text"),
		("POPULATION", 46, "int"),
		("GARRISON", 40, "int"),
		("HAPPY", 40, "int"),
		("HEALTH", 40, "int"),
		("GROWTH", 40, "int"),
		("FOOD", 40, "int"),
		("PRODUCTION", 40, "int"),
		("MAINTENANCE", 40, "int"),
		("COMMERCE_BASE", 40, "int"),
		("GOLD_RATE", 40, "int"),
		("RESEARCH_RATE", 40, "int"),
		("CULTURE_RATE", 40, "int"),
		("CULTURE", 53, "int"),
		("GREATPEOPLE_RATE", 40, "int"),
		("GREATPEOPLE", 45, "int"),
		("PRODUCING", 90, "text"),
		("PRODUCING_TURNS", 40, "int")
	]
},
{
	"name" : "Specialization",
	"cityName" : 120,
	"columns" : [
		("FOUNDED", 90, "date"),
		("RELIGIONS", 90, "text"),
		("SPECIALISTS", 159, "text"),
		("HAPPY", 40, "int"),
		("GROWTH", 40, "int"),
		("FOOD", 40, "int"),
		("PRODUCTION", 40, "int"),
		("GOLD_RATE", 40, "int"),
		("RESEARCH_RATE", 40, "int"),
		("CULTURE_RATE", 40, "int"),
		("CULTURE", 53, "int"),
		("GREATPEOPLE_RATE", 40, "int"),
		("GREATPEOPLE", 45, "int"),
		("PRODUCING", 90, "text"),
		("PRODUCING_TURNS", 40, "int")
	]
},
{
	"name" : "Top Cities - National",
	"cityName" : 120,
	"columns" : [
		("POPULATION", 50, "int"),
		("FEATURES", 92, "text"),
		("RELIGIONS", 90, "text"),
		("GOLD_RATE", 40, "int"),
		("GOLD_NRANK",  40, "int"),
		("RESEARCH_RATE", 40, "int"),
		("RESEARCH_NRANK", 40, "int"),
		("CULTURE_RATE", 40, "int"),
		("CULTURE_NRANK", 40, "int"),
		("CULTURE", 53, "int"),
		("PRODUCING", 90, "text"),
		("PRODUCING_TURNS", 40, "int")
	]
},
{
	"name" : "Top Cities - Global",
	"cityName" : 120,
	"columns" : [
		("POPULATION", 50, "int"),
		("FEATURES", 92, "text"),
		("RELIGIONS", 90, "text"),
		("GOLD_RATE", 40, "int"),
		("GOLD_GRANK",  40, "int"),
		("RESEARCH_RATE", 40, "int"),
		("RESEARCH_GRANK", 40, "int"),
		("CULTURE_RATE", 40, "int"),
		("CULTURE_GRANK", 40, "int"),
		("CULTURE", 53, "int"),
		("PRODUCING", 90, "text"),
		("PRODUCING_TURNS", 40, "int")
	]
},
{
	"name" : "Military Overview",
	"cityName" : 120,
	"columns" : [
		("GARRISON", 40, "int"),
		("DEFENSE", 60, "int"),
		("THREATS", 60, "text"),
		("CONSCRIPT_UNIT", 90, "text"),
		(self.getBuildingKey(3), 70, "text"),
		(self.getBuildingKey(4), 70, "text"),
		(self.getBuildingKey(5), 70, "text"),
		(self.getBuildingKey(6), 70, "text"),
		(self.getBuildingKey(7), 70, "text"),
		("PRODUCING", 90, "text"),
		("PRODUCING_TURNS", 40, "int")
	]
},
{
	"name" : "Property Watch",
	"cityName" : 120,
	"columns" : [
		("NAME", 120, "text"),
		("POPULATION", 50, "int"),
		("PROPERTY_CRIME", 80, "int"),
		("PROPERTY_CRIME_CHANGE", 50, "int"),
		("PROPERTY_EDUCATION", 80, "int"),
		("PROPERTY_EDUCATION_CHANGE", 50, "int"),
		("PROPERTY_DISEASE", 80, "int"),
		("PROPERTY_DISEASE_CHANGE", 50, "int"),
		("PROPERTY_WATER_POLLUTION", 80, "int"),
		("PROPERTY_WATER_POLLUTION_CHANGE", 50, "int"),
		("PROPERTY_AIR_POLLUTION", 80, "int"),
		("PROPERTY_AIR_POLLUTION_CHANGE", 50, "int"),
		("PROPERTY_TOURISM", 80, "int"),
		("PROPERTY_TOURISM_CHANGE", 50, "int"),
		("PROPERTY_FLAMMABILITY", 80, "int")
	]
}
			]
			self.save()

	def renameApply(self, iPlayer, userData, popupReturn):
		oldName = userData[1]
		newName = popupReturn.getEditBoxString(0)
		if oldName == newName: return

		screen = self.getScreen()
		TYPE = userData[0]
		if TYPE == "PAGE":
			iPage = userData[2]
			self.customizingSaveSelection(screen)
			self.PAGES[iPage]["name"] = newName
			self.drawScreen()
			self.customizingRestoreSelection(screen)
		else:
			self.CyPlayer.getCity(userData[2]).setName(newName, False)
			screen.setTableText(self.currentPage, 1, userData[3], newName, "", WidgetTypes.WIDGET_GENERAL, 1, 1, 1<<0)

	#--------------------------#
	# Base operation functions #
	#||||||||||||||||||||||||||#
	def update(self, fDelta):
		if self.tooltip.bLockedTT:
			self.tooltip.handle(self.getScreen())

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
		iData1	= inputClass.iData1
		iData2	= inputClass.iData2
		iCol	= inputClass.getMouseX()
		iRow	= inputClass.getMouseY()
		szFlag	= HandleInputUtil.MOUSE_FLAGS.get(inputClass.uiFlags, "UNKNOWN")

		print "Row: %d\nCol: %d" %(iRow, iCol)

		if iCode == NotifyCode.NOTIFY_CURSOR_MOVE_ON:

			if NAME == "DomToggleBtn":
				if self.bCustomizing:
					szText = TRNSLTR.getText("TXT_KEY_CDA_STOP_EDITING", ())
				else:
					szText = TRNSLTR.getText("TXT_KEY_CDA_START_EDITING", ())
				self.tooltip.handle(screen, szText)

			elif NAME == "CityNameWidth":
				szText = "Left click to increase by 1.\nRight click to decrease by 1.\n\nHold shift to modify by 10, ctrl to modify by 5 or both to modify by 20."
				self.tooltip.handle(screen, szText)

			return

		self.tooltip.reset(screen)

		if iCode == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:

			if NAME == "DomPagesDD":
				if self.bCustomizing:
					self.customizingSelection = []
				self.switchPage(self.PAGES[iData]["name"])
				self.drawScreen()

			elif NAME == "CDA_PAGE_":
				name = screen.getTableText("CDA_PAGE_%d" % ID, 1, iRow)
				for cyCity in self.cityList:
					if cyCity.getName() == name:
						screen.hideScreen()
						CyMap().bringIntoView(cyCity.getX(), cyCity.getY(), True, False, True, False, False)
						break

		elif iCode == NotifyCode.NOTIFY_CLICKED:
			bCustomizing = self.bCustomizing
			iPage = self.iPage

			if NAME in ("DomPagePrevBtn", "DomPageNextBtn"):
				i = 0
				if NAME == "DomPagePrevBtn":
					if iPage:
						i -= 1
				elif NAME == "DomPageNextBtn":
					if iPage + 1 < len(self.PAGES):
						i += 1
				if i:
					if bCustomizing:
						self.customizingSelection = []
					self.switchPage(self.PAGES[iPage + i]["name"])
					self.drawScreen()


			elif NAME == "DomToggleBtn":
				if bCustomizing:
					self.hideCustomizationControls(screen)
					self.switchPage(self.PAGES[iPage]["name"])
				else:
					screen.show("DomAddCol")
					screen.show("DomDelCol")
					screen.show("DomColUp")
					screen.show("DomColDn")
					screen.show("DomColShrink")
					screen.show("DomColWiden")

					screen.show("DomRenameBtn")
					screen.show("DomAddPageBtn")
					screen.show("DomDelPageBtn")
					screen.show("DomPageUpBtn")
					screen.show("DomPageDownBtn")
					screen.show("DomSaveBtn")
					screen.show("DomReloadBtn")

				self.bCustomizing = not bCustomizing
				self.drawScreen()

			elif bCustomizing:
				if NAME == "DomAddPageBtn":
					self.customizingSelection = []
					name = "New Page %d" % (len(self.PAGES) + 1)
					self.PAGES.append({"name" : name, "cityName" : 120, "columns" : []})
					self.switchPage(name)
					self.drawScreen()

				elif NAME == "DomDelPageBtn":
					if len(self.PAGES) > 1:
						self.customizingSelection = []
						del self.PAGES[iPage]
						self.switchPage(self.PAGES[0]["name"])
						self.drawScreen()

				elif NAME in ("DomPageUpBtn", "DomPageDownBtn"):
					toPage = None

					if NAME == "DomPageDownBtn":
						if iPage + 1 < len(self.PAGES):
							toPage = iPage + 1

					elif NAME == "DomPageUpBtn":
						if iPage:
							toPage = iPage - 1

					if toPage is not None:
						self.customizingSelection = []
						temp = self.PAGES[iPage]
						self.PAGES[iPage] = self.PAGES[toPage]
						self.PAGES[toPage] = temp
						self.switchPage(temp["name"])
						self.drawScreen()


				elif NAME == "DomReloadBtn":
					self.iPage = 0
					self.loadPages()
					self.switchPage("CDA_PAGE_0")
					self.drawScreen()

				elif NAME == "DomRenameBtn":
					name = self.PAGES[self.iPage]["name"]
					xRes = self.xRes
					if xRes > 2500:
						header = "<font=4b>"
						body = "\n<font=3>"
						w = 560
						h = 172
					elif xRes > 1700:
						header = "<font=3b>"
						body = "\n<font=2>"
						w = 500
						h = 160
					elif xRes > 1400:
						header = "<font=2b>"
						body = "\n<font=1>"
						w = 440
						h = 148
					else:
						header = "<font=1b>"
						body = "\n<font=0>"
						w = 380
						h = 136
					szTxt = header + "Rename page:" + body + name
					popup = CyPopup(5000, EventContextTypes.EVENTCONTEXT_SELF, True)
					popup.setSize(w, h)
					popup.setPosition(xRes/2 - w/2, self.yRes/2 - h/2)
					popup.setUserData(("PAGE", name, self.iPage))
					popup.setBodyString(szTxt, 1<<0)
					popup.createEditBox(name, 0)
					popup.setEditBoxMaxCharCount(32, 0, 0)
					popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

				elif NAME in ("DomAddCol", "DomDelCol", "DomColUp", "DomColDn", "DomColShrink", "DomColWiden"):
					self.customizingSaveSelection(screen)

					if NAME == "DomAddCol":
						for i in xrange(len(self.COLUMNS_LIST)):
							if screen.isRowSelected("DomColumnsList", i):
								col = self.COLUMNS_LIST[int(screen.getTableText("DomColumnsList", 0, i))]
								if (col[0], col[1], col[2]) not in self.PAGES[iPage]["columns"]:
									self.PAGES[iPage]["columns"].append((col[0], col[1], col[2]))

					elif NAME == "DomDelCol":
						list = self.getReverseSortedColumnSelection(screen)

						for i in list:
							del self.PAGES[iPage]["columns"][i]

					elif NAME == "DomColUp":
						list = self.getSortedColumnSelection(screen)
						top = 0
						for i in list:
							if i == top:
								top += 1
							else:
								col = self.PAGES[iPage]["columns"][i]
								del self.PAGES[iPage]["columns"][i]
								self.PAGES[iPage]["columns"].insert(i - 1, col)

					elif NAME == "DomColDn":
						list = self.getReverseSortedColumnSelection(screen)
						bottom = len(self.PAGES[iPage]["columns"]) - 1
						for i in list:
							if i == bottom:
								bottom -= 1
							else:
								col = self.PAGES[iPage]["columns"][i]
								del self.PAGES[iPage]["columns"][i]
								self.PAGES[iPage]["columns"].insert(i + 1, col)

					elif NAME == "DomColShrink":
						columns = self.PAGES[iPage]["columns"]

						list = self.getReverseSortedColumnSelection(screen)
						for i in list:
							col = columns[i]
							if col[1] < 17:
								continue
							shrunken = (col[0], col[1] - 1, col[2])
							del columns[i]
							columns.insert(i, shrunken)

					elif NAME == "DomColWiden":
						columns = self.PAGES[iPage]["columns"]

						list = self.getReverseSortedColumnSelection(screen)
						for i in list:
							col = columns[i]
							wider  = (col[0], col[1] + 1, col[2])
							del columns[i]
							columns.insert(i, wider)

					self.drawScreen()
					self.customizingRestoreSelection(screen)

				elif NAME == "DomSaveBtn":
					self.save()
				elif NAME == "CityNameWidth":
					iW = iCheck = self.PAGES[iPage]["cityName"]
					if szFlag == "MOUSE_RBUTTONUP":
						if iW > 0:
							if bCtrl:
								if bShift:
									iW -= 20
								else:
									iW -= 5
							elif bShift:
								iW -= 10
							else:
								iW -= 1
							if iW < 0:
								iW = 0
					elif szFlag == "MOUSE_LBUTTONUP":
						if iW < 999:
							if bCtrl:
								if bShift:
									iW += 20
								else:
									iW += 5
							elif bShift:
								iW += 10
							else:
								iW += 1
							if iW > 999:
								iW = 999
					if iW != iCheck:
						self.PAGES[iPage]["cityName"] = iW
						stText = "City name width: %d" %iW
						Txt = "CityNameWidth0"
						screen.hide(Txt)
						screen.modifyString(Txt, stText, 1<<0)
						screen.show(Txt)

			elif NAME == "DomesticSplit":
				screen.hideScreen()

	def onClose(self):
		del self.eventManager.OverrideEventApply[5000], self.eventManager, self.InputData, self.CyPlayer, self.iPlayer, self.cityList, \
			self.TXT_NAME, self.xRes, self.yRes, self.aFontList, self.hTable1, self.hTable2, self.xTable2, self.wTable2
