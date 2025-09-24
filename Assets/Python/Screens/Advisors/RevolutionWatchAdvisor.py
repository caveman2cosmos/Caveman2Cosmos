#
## Customizable Revolutions Advisor
## for Sid Meier's Civilization 4 Beyond The Sword
## Copyright Firaxis Games 2005
#
#  Created by Lee Reeves, AKA Taelis on civfanatics.com
#  Re-worked for Revolutions by Glider1

## REQUIRES Civilization 4 - Beyond the Sword - PATCH 3.02

## Credits
#
#	This advisor was inspired by the fantastic work of Homegrown and Requies of civfanatics.com, and all
#	those who contributed to their work. I'd like to thank:
#
#	 Jdog5000 for the brilliant Revolutions mod this advisor is dedicated too.
#
#	  Cammagno - for extensive help with beta testing, and for developing a complete, optimized configuration file
#
#	  Homegrown - for the original domestic advisor mod with its many improvements
#
#	  Requies - for the "Special" Domestic Advisor, the basis of this mod. Credits from his mod include:
#
#		terrasol - "for the changes to the color of the text to indicate problem values and for changes
#					which fix the non-English language problems and additional small changes"
#		Arkeide - "for the base code to jumping to a city on the list."
#		rendermad - "for the suggestion of making garrison colored when < 1"
#		=DOCTOR= - "for the suggestion of putting in Culture and GP Thresholds"
#		Tubby Rower - "for the unfortunately unimplementable mouseover idea"
#		eotinb - "for the suggestion of putting in the city automation information and for some general
#				  comments and ideas"
#		sportybrian - "for the suggestion of highlighting with different colors for the 0, and very positive levels."
#
#	 Fixaxis, of course, for making CivIV and stealing months of my life. ;)

## Legal Stuff
#
#  THIS MATERIAL IS NOT MADE, GUARANTEED OR SUPPORTED BY THE PUBLISHER OF THE SOFTWARE OR ITS AFFILIATES.
#  THIS MATERIAL IS RELEASED AS-IS. IN NO EVENT WILL THE AUTHOR BE LIABLE FOR SPECIAL, INCIDENTAL OR
#  CONSEQUENTIAL DAMAGES RESULTING FROM POSSESSION, USE OR MALFUNCTION OF THE SOFTWARE, INCLUDING
#  DAMAGES TO PROPERTY, LOSS OF GOODWILL, COMPUTER FAILURE OR MALFUNCTION AND, TO THE EXTENT PERMITTED
#  BY LAW, DAMAGES FOR PERSONAL INJURIES, EVEN IF THE AUTHOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
#  DAMAGES. THE AUTHOR'S LIABILITY SHALL NOT EXCEED THE ACTUAL PRICE PAID FOR USE OF THE MATERIAL. SOME
#  STATES/COUNTRIES DO NOT ALLOW LIMITATIONS ON HOW LONG AN IMPLIED WARRANTY LASTS AND/OR THE EXCLUSION
#  OR LIMITATION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE ABOVE LIMITATIONS AND/OR EXCLUSION OR
#  LIMITATION OF LIABILITY MAY NOT APPLY TO YOU. THIS WARRANTY GIVES YOU SPECIFIC LEGAL RIGHTS, AND
#  YOU MAY HAVE OTHER RIGHTS THAT VARY FROM JURISDICTION TO JURISDICTION.

###############################################################################################################

from CvPythonExtensions import *
import CvScreenEnums
import CvEventInterface
import BugConfigTracker
import math
import SystemPaths as SP
# BUG - Options
import BugCore
CityScreenOpt = BugCore.game.CityScreen
# RevolutionDCM
import RevDefs
import RevData
import RevInstances
import RevUtils

# Needed to save changes
# Rise of Mankind 2.8 - pickle optimization
try:
	import cPickle as pickle
except:
	import pickle
# Rise of Mankind 2.8 - pickle optimization

# Needed to check for non-numbers (specially search function)
import re

import time

#	IMPORTANT INFORMATION
#
#	All widget names MUST be unique when creating screens.  If you create
#	a widget named 'Hello', and then try to create another named 'Hello', it
#	will modify the first hello.
#
#	Also, when attaching widgets, 'Background' is a reserve word meant for
#	the background widget.  Do NOT use 'Background' to name any widget, but
#	when attaching to the background, please use the 'Background' keyword.

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()
GAME = gc.getGame()


class RevolutionWatchAdvisor:
	"""Special Revolution Watch Advisor Screen"""

	def __init__(self):
		"""
		Basic init function.
		Called when cIV is first booted up and everytime you switch into cIV
		"""

		# RevolutionDCM
		self.name = "RevolutionWatchAdvisor"
		self.isFlavorful = True

		self.PICKLED_VERSION = 1

		self.NO_CITY = -1

		self.runtimeInitDone = False

		self.listSelectedCities = []

		# An event context for renaming pages
		self.renameEventContext = None

		# Dimensions. Here is where you change sizes and lengths of general things
		self.nScreenWidth = 1024
		self.nScreenLength = 768

		# Location/Size of the Panel
		self.nPanelX = 0
		self.nPanelY = 29
		self.nPanelWidth = self.nScreenWidth
		self.nPanelLength = 562

		# Dimension of the table
		self.nTableX = 12
		self.nTableY = 42
		self.nTableWidth = 1000
		self.nTableLength = self.nPanelLength - 10 - self.nTableY
		self.nShortTableLength = 380

		self.nSecondHalfTableX = self.nTableX + self.nTableWidth / 2 + 10
		self.nHalfTableWidth = self.nTableWidth / 2 - 10

		self.nCustomizeControlY = 450
		self.nCustomizeControlYoffset = 20

		# Location of Text Buttons
		self.X_EXIT = self.nTableX + self.nTableWidth
		self.Y_EXIT = self.nPanelLength + self.nPanelY - 32
		self.Y_TEXT = self.nPanelLength + self.nPanelY - 27
		self.Z_TEXT = -0.1
		self.DX_TEXT = -200

		# Location of Revolution Legend Info
		self.nRevolutionLevelX = 10
		self.nRevolutionLevelY = self.nShortTableLength + 50
		self.nRevolutionLevelTextOffsetX = 0
		self.nRevolutionLevelTextOffsetY = 0
		self.nRevolutionLegendLength = 110

		# Location of Revolution Bribe Button
		self.revIcon = "Art/Interface/Buttons/revbtn.dds"
		self.revIconSize = 40
		self.revIconX = self.nRevolutionLevelX + self.revIconSize / 4
		self.revIconY = self.nRevolutionLevelY + self.revIconSize / 2 - 5
		self.revBribeTextX = self.revIconSize + self.revIconX + 3
		self.revBribeTextY = self.revIconY + 7

		# Location of Revolution Bribe Button Panel
		self.BRIBE_PANEL = self.name + "BribePanel"
		self.nBribePanelX = self.nRevolutionLevelX + 2
		self.nBribePanelY = self.nRevolutionLevelY
		self.nBribePanelWidth = 280
		self.nBribePanelLength = self.nRevolutionLegendLength

		# Location of Revolution Legend Info Panel
		self.nLegendPanelGap = 8
		self.LEGEND_PANEL = self.name + "LegendText"
		self.nLegendPanelX = self.nBribePanelWidth + self.nBribePanelX + self.nLegendPanelGap
		self.nLegendPanelY = self.nBribePanelY
		self.nLegendPanelWidth = self.nTableWidth - self.nBribePanelWidth - self.nLegendPanelGap
		self.nLegendPanelLength = self.nRevolutionLegendLength


# BUG - Colony Split - start

		# Location of Split Empire Button
		self.SPLIT_NAME = "RevolutionWatchSplit"
		self.X_SPLIT = self.X_EXIT - 100
		self.Y_SPLIT = self.Y_TEXT - 8

# BUG - Colony Split - end

		# Building Button Headers
		self.BUILDING_BUTTON_X_SIZE = 24
		self.BUILDING_BUTTON_Y_SIZE = 24

		# Location of Specialist Toggle Button
		self.X_SPECIAL = self.nTableX
		self.Y_SPECIAL = self.Y_TEXT - 10

		# Width of page dropdown
		self.PAGES_DD_W = 300

		# Location of Culture Threshold Info
		self.nCultureLevelX = 670
		self.nCultureLevelY = 450
		self.nCultureLevelDistance = 15
		self.nCultureLevelTextOffset = 110

		# Location of next GP Threshold Info
		self.nGPLevelX = 870
		self.nGPLevelY = 450
		self.nGPLevelDistance = 30

		self.nControlSize = 28

		# Location/Size of the Specialist Images
		self.nFirstSpecialistX = 30
		self.nSpecialistY = 450
		self.nSpecialistWidth = 32
		self.nSpecialistLength = 32
		self.nSpecialistDistance = 70

		# Offset from Specialist Image/Size for the Specialist Plus/Minus buttons
		self.nPlusOffsetX = -4
		self.nMinusOffsetX = 16
		self.nPlusOffsetY = self.nMinusOffsetY = 30
		self.nPlusWidth = self.nPlusHeight = self.nMinusWidth = self.nMinusHeight = 20

		# Offset from Specialist Image for the Specialist Text
		self.nSpecTextOffsetX = 0
		self.nSpecTextOffsetY = 50

		# Names of Widgets
		self.SCREEN_NAME = "RevolutionWatchAdvisor"
		self.EXIT_NAME = "RevolutionWatchExit"
		self.BACKGROUND_ID = "RevolutionWatchAdvisorBG"
		self.PAGES_DD_NAME = "RevWatchPagesDD"
		self.REVOLUTION_LEGEND_ICON = "RevolutionLegendIcon"
		self.REVOLUTION_LEGEND_TEXT = "RevolutionLegendText"
		self.REVOLUTION_BRIBE_TEXT = "RevolutionBribeText"

		self.CULTURE_TEXT_NAME = "RevCultureText"
		self.GP_TEXT_NAME = "RevGPText"
		self.NUMBER_TEXT = "NUM"

		self.SPECIALIST_IMAGE_NAME = "RevCitizenImage"
		self.SPECIALIST_PLUS_NAME = "RevIncreaseSpecialist"
		self.SPECIALIST_MINUS_NAME = "RevDecreaseSpecialist"
		self.SPECIALIST_TEXT_NAME = "RevSpecialistText"

		self.CUSTOMIZE_PAGE = "RevCustomize"
		self.COLUMNS_LIST_PAGE = "RevColumnsList"

		self.PREV_PAGE_NAME = "RevPagePrevButton"
		self.NEXT_PAGE_NAME = "RevPageNextButton"
		self.START_CUSTOMIZING_NAME = "RevStartCustomizing"
		self.RENAME_PAGE_NAME = "RevRenamePage"
		self.ADD_PAGE_NAME = "RevAddPageButton"
		self.DEL_PAGE_NAME = "RevDelPageButton"
		self.PAGE_UP_NAME = "RevPageUpButton"
		self.PAGE_DOWN_NAME = "RevPageDownButton"
		self.SAVE_NAME = "RevSaveButton"
		self.RELOAD_PAGES_NAME = "RevReloadPages"

		self.COLUMN_SHRINK_NAME = "RevColShrink"
		self.COLUMN_WIDEN_NAME = "RevColWiden"
		self.ADDCOLUMN_NAME = "RevAddCol"
		self.DELCOLUMN_NAME = "RevDelCol"
		self.COLUMNUP_NAME = "RevColUp"
		self.COLUMNDN_NAME = "RevColDn"

		self.TOGGLE_SPECS_NAME = "ToggleSpecsCB"

		self.customizing = False
		self.currentPageNum = 0

		# Special Class variables

		# The currently active page
		self.currentPage = None
		self.visiblePage = None

		#RevolutionDCM - Data sources useful for Revolutions mod advise
		#Remove "#" in the column list to return default revolution watch advisor data sources
		self.COLUMNS_LIST = [
				# Name					  Width	Type   CyCityFunction0			CyCityFunction1			Arg									selfFunction							Arg							Title

				("NAME",					95,		"text",	CyCity.getName,			None,					0,									None,									None,						"localText.getText(\"TXT_KEY_DOMESTIC_ADVISOR_NAME\", ()).upper()"),
				("REVOLUTION_INDEX",		85,		"int",	CyCity.getRevolutionIndex,None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REVOLUTION_INDEX_ADVISOR_SIMPLE\", ()).upper()"),
				("REVOLUTION_STATUS",		85,		"text",	self.getRevolutionStatusText,None,				0,									None,									None,						"localText.getText(\"TXT_KEY_REV_STATUS\", ()).upper()"),
				("LOCAL_REV_INDEX",			85,		"int",	CyCity.getLocalRevIndex,None,					0,									None,									None,						"localText.getText(\"TXT_KEY_LOCAL_REV_INDEX_ADVISOR_SIMPLE\", ()).upper()"),
				("REV_HAPPINESS",			85,		"int",	self.getRevHappinessVal,None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_HAPPINESS\", ()).upper()"),
				("REV_HAPPINESS_TEXT",		85,		"text",	self.getRevHappinessText,None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_HAPPINESS\", ()).upper()"),
				("REV_DISTANCE",			85,		"int",	self.getRevDistanceVal,	None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_DISTANT\", ()).upper()"),
				("REV_DISTANCE_TEXT",		85,		"text",	self.getRevDistanceText,None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_DISTANT\", ()).upper()"),
				("REV_COLONY",				85,		"int",	self.getRevColonyVal,	None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_COLONY\", ()).upper()"),
				("REV_COLONY_TEXT",			85,		"text",	self.getRevColonyText,	None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_COLONY\", ()).upper()"),
				("REV_RELIGION",			85,		"int",	self.getRevReligionVal,	None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_RELIGION\", ()).upper()"),
				("REV_RELIGION_TEXT",		85,		"text",	self.getRevReligionText,None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_RELIGION\", ()).upper()"),
				("REV_NATIONALITY",			85,		"int",	self.getRevNationalityVal,None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_NATIONALITY\", ()).upper()"),
				("REV_NATIONALITY_TEXT",	85,		"text",	self.getRevNationalityText,None,				0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_NATIONALITY\", ()).upper()"),
				("REV_HEALTH",				85,		"int",	self.getRevHealthVal,	None,					0,									None,									None,						"localText.getText(\"TXT_ADVISOR_HEALTH\", ()).upper()"),
				("REV_HEALTH_TEXT",			85,		"text",	self.getRevHealthText,	None,					0,									None,									None,						"localText.getText(\"TXT_ADVISOR_HEALTH\", ()).upper()"),
				("REV_GARRISON",			85,		"int",	self.getRevGarrisonVal,	None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_GARRISON\", ()).upper()"),
				("REV_GARRISON_TEXT",		85,		"text",	self.getRevGarrisonText,None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_GARRISON\", ()).upper()"),
				("REV_DISORDER",			85,		"int",	self.getRevDisorderVal,	None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_DISORDER\", ()).upper()"),
				("REV_DISORDER_TEXT",		85,		"text",	self.getRevDisorderText,None,					0,									None,									None,						"localText.getText(\"TXT_KEY_REV_WATCH_DISORDER\", ()).upper()"),
				("ADVISE_CULTURE",			150,	"text",	None,					None,					0,									self.advise,							"Culture",					"localText.getText(\"TXT_WORD_CULTURE\", ()).upper()"),
#				("ADVISE_MILITARY",			150,	"text",	None,					None,					0,									self.advise,							"Military",					"localText.getText(\"TXT_KEY_ADVISOR_MILITARY\", ()).upper()"),
#				("ADVISE_NUTTY",			150,	"text",	None,					None,					0,									self.advise,							"Nutty",					"u\"NUTTY\""),
				("ADVISE_RELIGION",			150,	"text",	None,					None,					0,									self.advise,							"Religion",					"localText.getText(\"TXT_KEY_CONCEPT_RELIGION\", ()).upper()"),
#				("ADVISE_RESEARCH",			150,	"text",	None,					None,					0,									self.advise,							"Research",					"localText.getText(\"TXT_KEY_COMMERCE_RESEARCH\", ()).upper()"),
#				("ADVISE_SPACESHIP",		150,	"text",	None,					None,					0,									self.advise,							"Spaceship",				"localText.getText(\"TXT_KEY_CONCEPT_SPACESHIP\", ()).upper()"),
#				("AUTOMATION",				80,		"text",	None,					None,					0,									self.calculateAutomation,				None,						"u\"AUTO\""),
				("BASE_COMMERCE",			38,		"int",	None,					CyCity.getPlotYield,	YieldTypes.YIELD_COMMERCE,			None,									None,						"u\"B\" + self.commerceIcon"),
				("BASE_FOOD",				38,		"int",	None,					CyCity.getPlotYield,	YieldTypes.YIELD_FOOD,				None,									None,						"u\"B\" + self.foodIcon"),
				("BASE_PRODUCTION",			38,		"int",	None,					CyCity.getPlotYield,	YieldTypes.YIELD_PRODUCTION,		None,									None,						"u\"B\" + self.hammerIcon"),
#				("CONSCRIPT_UNIT",			90,		"text",	None,					None,					0,									self.calculateConscriptUnit,			None,						"localText.getText(\"TXT_KEY_CONCEPT_DRAFT\", ()).upper()"),
#				("COULD_CONSCRIPT_UNIT",	90,		"text",	None,					None,					0,									self.calculatePotentialConscriptUnit,	None,						"localText.getText(\"TXT_KEY_CONCEPT_DRAFT\", ()).upper() + u\"#\""),
#				("CORPORATIONS",			90,		"text",	None,					None,					0,									self.calculateCorporations,				None,						"localText.getText(\"TXT_KEY_CONCEPT_CORPORATIONS\", ()).upper()"),
				("CULTURE",					53,		"int",	None,					None,					0,									self.calculateTotalCulture,				None,						"self.cultureIcon"),
				("CULTURE_RATE",			38,		"int",	None,					CyCity.getCommerceRate, CommerceTypes.COMMERCE_CULTURE,		None,									None,						"self.cultureIcon + u\"R\""),
				("CULTURE_TURNS",			38,		"int",	None,					None,					None,								self.calculateCultureTurns,				None,						"self.cultureIcon + u\"T\""),
#				("DEFENSE",					60,		"int",	None,					None,					0,									self.calculateDefense,					None,						"self.defenseIcon"),
#				("ESPIONAGE",				38,		"int",	None,					CyCity.getCommerceRate, CommerceTypes.COMMERCE_ESPIONAGE,	None,									None,						"self.espionageIcon"),
#				("ESPIONAGE_DEF",			60,		"int",	CyCity.getEspionageDefenseModifier,	None,		0,									self.calculateEspionageDefense,			None,						"self.espionageIcon + u\"%\""),
#				("FEATURES",				106,	"text",	None,					None,					0,									self.calculateFeatures,					None,						"localText.getText(\"TXT_KEY_MISC_FEATURES\", ())"),
				("FOOD",					35,		"int",	None,					None,					0,									self.calculateFood,						None,						"self.foodIcon"),
#				("FOUNDED",					80,		"date",	None,					None,					0,									self.calculateFounded,					None,						"localText.getText(\"TXT_KEY_DOMESTIC_ADVISOR_FOUNDED\", ()).upper()"),
#				("GARRISON",				30,		"int",	CyCity.getMilitaryHappinessUnits,	None,		0,									None,									None,						"self.militaryIcon"),
				("GOLD",					38,		"int",	None,					CyCity.getCommerceRate, CommerceTypes.COMMERCE_GOLD,		None,									None,						"self.goldIcon"),
				("GRANK_BASE_COMMERCE",		42,		"int",	None,					None,					0,									self.findGlobalBaseYieldRateRank, YieldTypes.YIELD_COMMERCE,		"u\"B\" + self.commerceIcon + u\"g\""),
				("GRANK_BASE_FOOD",			42,		"int",	None,					None,					0,									self.findGlobalBaseYieldRateRank, YieldTypes.YIELD_FOOD,			"u\"B\" + self.foodIcon + u\"g\""),
				("GRANK_BASE_PRODUCTION",	42,		"int",	None,					None,					0,									self.findGlobalBaseYieldRateRank, YieldTypes.YIELD_PRODUCTION,		"u\"B\" + self.hammerIcon + u\"g\""),
				("GRANK_COMMERCE",			38,		"int",	None,					None,					0,									self.findGlobalYieldRateRank, YieldTypes.YIELD_COMMERCE,			"self.commerceIcon + u\"g\""),
				("GRANK_FOOD",				38,		"int",	None,					None,					0,									self.findGlobalYieldRateRank, YieldTypes.YIELD_FOOD,				"self.foodIcon + u\"g\""),
				("GRANK_PRODUCTION",		38,		"int",	None,					None,					0,									self.findGlobalYieldRateRank, YieldTypes.YIELD_PRODUCTION,			"self.hammerIcon + u\"g\""),
				("GRANK_CULTURE",			38,		"int",	None,					None,					0,									self.findGlobalCommerceRateRank, CommerceTypes.COMMERCE_CULTURE,	"self.cultureIcon + u\"g\""),
				("GRANK_GOLD",				38,		"int",	None,					None,					0,									self.findGlobalCommerceRateRank, CommerceTypes.COMMERCE_GOLD,		"self.goldIcon + u\"g\""),
				("GRANK_RESEARCH",			38,		"int",	None,					None,					0,									self.findGlobalCommerceRateRank, CommerceTypes.COMMERCE_RESEARCH,	"self.researchIcon + u\"g\""),
				("GREATPEOPLE",				45,		"int",	CyCity.getGreatPeopleProgress,	None,			0,									None,									None,						"self.figureheadIcon"),
				("GREATPEOPLE_RATE",		38,		"int",	CyCity.getGreatPeopleRate,		None,			0,									None,									None,						"self.figureheadIcon + u\"R\""),
				("GREATPEOPLE_TURNS",		38,		"int",	None,					None,					None,								self.calculateGreatPeopleTurns,			None,						"self.figureheadIcon + u\"T\""),
				("GROWTH",					35,		"int",	None,					None,					0,									self.calculateGrowth,					None,						"self.redfoodIcon"),
				("HAPPY",					30,		"int",	None,					None,					0,									self.calculateNetHappiness,				None,						"self.happyIcon"),
				("HEALTH",					30,		"int",	None,					None,					0,									self.calculateNetHealth,				None,						"self.healthIcon"),
#				("HURRY_GOLD",				38,		"int",	None,					None,					0,									self.calculateHurryGoldCost,			None,						"u\"H\" + self.goldIcon"),
#				("HURRY_POP",				38,		"int",	None,					None,					0,									self.calculateWhipPopulation,			None,						"u\"H\" + self.angryIcon"),
#				("HURRY_POP_EXTRA",			38,		"int",	None,					None,					0,									self.calculateWhipOverflowProduction,	None,						"u\"H\" + self.hammerIcon"),
#				("HURRY_POP_GOLD",			38,		"int",	None,					None,					0,									self.calculateWhipOverflowGold,			None,						"u\"H\" + self.goldIcon"),
#				("LIBERATE",				35,		"int",	None,					None,					0,									self.canLiberate,						None,						"self.fistIcon"),
#				("LOCATION_X",				50,		"int",	CyCity.getX,			None,					0,									None,									None,						"u\"X\""),
#				("LOCATION_Y",				50,		"int",	CyCity.getY,			None,					0,									None,									None,						"u\"Y\""),
				("MAINTENANCE",				30,		"int",	CyCity.getMaintenance,	None,					0,									None,									None,						"self.redGoldIcon"),
				("NRANK_BASE_COMMERCE",		42,		"int",	None,					CyCity.findBaseYieldRateRank, YieldTypes.YIELD_COMMERCE,	None,									None,						"u\"B\" + self.commerceIcon + u\"n\""),
				("NRANK_BASE_FOOD",			42,		"int",	None,					CyCity.findBaseYieldRateRank, YieldTypes.YIELD_FOOD,		None,									None,						"u\"B\" + self.foodIcon + u\"n\""),
				("NRANK_BASE_PRODUCTION",	42,		"int",	None,					CyCity.findBaseYieldRateRank, YieldTypes.YIELD_PRODUCTION,	None,									None,						"u\"B\" + self.hammerIcon + u\"n\""),
				("NRANK_COMMERCE",			38,		"int",	None,					CyCity.findYieldRateRank, YieldTypes.YIELD_COMMERCE,		None,									None,						"self.commerceIcon + u\"n\""),
				("NRANK_FOOD",				38,		"int",	None,					CyCity.findYieldRateRank, YieldTypes.YIELD_FOOD,			None,									None,						"self.foodIcon + u\"n\""),
				("NRANK_PRODUCTION",		38,		"int",	None,					CyCity.findYieldRateRank, YieldTypes.YIELD_PRODUCTION,		None,									None,						"self.hammerIcon + u\"n\""),
				("NRANK_CULTURE",			38,		"int",	None,					CyCity.findCommerceRateRank, CommerceTypes.COMMERCE_CULTURE,	None,								None,						"self.cultureIcon + u\"n\""),
				("NRANK_GOLD",				38,		"int",	None,					CyCity.findCommerceRateRank, CommerceTypes.COMMERCE_GOLD,	None,									None,						"self.goldIcon + u\"n\""),
				("NRANK_RESEARCH",			38,		"int",	None,					CyCity.findCommerceRateRank, CommerceTypes.COMMERCE_RESEARCH,	None,								None,						"self.researchIcon + u\"n\""),
				("POPULATION",				35,		"int",	CyCity.getPopulation,	None,					0,									None,									None,						"localText.getText(\"TXT_KEY_POPULATION\", ()).upper()"),
				("POPULATION_REAL",			65,		"int",	CyCity.getRealPopulation,	None,				0,									None,									None,						"localText.getText(\"TXT_KEY_POPULATION\", ()).upper() + u\"#\""),
				("POWER",					50,		"text",	None,					None,					0,									self.calculatePower,					None,						"self.powerIcon"),
				("PRODUCING",				90,		"text",	None,					None,					0,									self.calculateProducing,				None,						"localText.getText(\"TXT_KEY_DOMESTIC_ADVISOR_PRODUCING\", ())"),
				("PRODUCING_TURNS",			33,		"int",	None,					None,					0,									self.calculateProducingTurns,			None,						"self.hammerIcon + u\"T\""),
				("PRODUCTION",				38,		"int",	None,					None,					0,									self.calculateProduction,				None,						"self.hammerIcon"),
				("RELIGIONS",				90,		"text",	None,					None,					0,									self.calculateReligions,				None,						"localText.getText(\"TXT_KEY_ADVISOR_RELIGION\", ()).upper()"),
#				("RESEARCH",				38,		"int",	None,					CyCity.getCommerceRate, CommerceTypes.COMMERCE_RESEARCH,	None,									None,						"self.researchIcon"),
				("SPECIALISTS",				209,	"text",	None,					None,					0,									self.calculateSpecialists,				None,						"localText.getText(\"TXT_KEY_CONCEPT_SPECIALISTS\", ()).upper()"),
#				("THREATS",					60,		"text",	None,					None,					0,									self.calculateThreats,					None,						"u\"Threats\""),
#				("TRADE",					30,		"int",	None,					None,					0,									self.calculateTrade,					None,						"self.tradeIcon"),
#				("TRADE_DOMESTIC",			30,		"int",	None,					None,					0,									self.calculateTrade,					"D",						"u\"D\" + self.tradeIcon"),
#				("TRADE_FOREIGN",			30,		"int",	None,					None,					0,									self.calculateTrade,					"F",						"u\"F\" + self.tradeIcon"),
#				("TRADE_ROUTES",			30,		"int",	None,					None,					0,									self.countTradeRoutes,					None,						"u\"#\" + self.tradeIcon"),
#				("TRADE_ROUTES_DOMESTIC",	30,		"int",	None,					None,					0,									self.countTradeRoutes,					"D",						"u\"D#\" + self.tradeIcon"),
#				("TRADE_ROUTES_FOREIGN",	30,		"int",	None,					None,					0,									self.countTradeRoutes,					"F",						"u\"F#\" + self.tradeIcon"),
			]

		# Values to check to see if we need to color the number as a problem
		self.PROBLEM_VALUES_DICT = {
			"GARRISON" : 0,
			"HAPPY" : -1,
			"HEALTH" : -1,
			"GROWTH" : -1,
			"FOOD" : -1,
			}

		# Values to check to see if we need to color the number as neutral
		self.NEUTRAL_VALUES_DICT = {
			"HAPPY" : 0,
			"HEALTH" : 0,
			"GROWTH" : 0,
			"FOOD" : 0,
			}

		# Values to check to see if we need to color the number as great
		self.GREAT_VALUES_DICT = {
			"HAPPY" : 6,
			"HEALTH" : 6,
			"FOOD" : 8,
			}

		# Dictionary of the coloring dictionaries!
		self.COLOR_DICT_DICT = {
			"PROBLEM": self.PROBLEM_VALUES_DICT,
			"NEUTRAL": self.NEUTRAL_VALUES_DICT,
			"GREAT": self.GREAT_VALUES_DICT,
			}


		# This creates the set of ALL coloring keys.
		# Do NOT touch.
		self.COLOR_SET = set()
		for clDict in self.COLOR_DICT_DICT.values():
			self.COLOR_SET.update (clDict.keys())

		# Values to change on an update
		# (True indicates update when we DON'T switch to/from food production)
		# Most of this MIGHT change because the computer might be switching
		# plots it's working or creating/removing specialists.

		self.UPDATE_DICT = {
			"HAPPY": False,
			"GROWTH" : False,
			"FOOD": False,
			"PRODUCTION": True,
			"BASE_COMMERCE": False,
			"GOLD": False,
			"RESEARCH": False,
			"CULTURE_RATE": False,
			"CULTURE_TURNS": False,
			"ESPIONAGE": False,
			"GREATPEOPLE_RATE": False,
			"GREATPEOPLE_TURNS": False,
			"PRODUCING": True,
			"PRODUCING_TURNS": True,
			"SPECIALISTS": False,
			"AUTOMATION": True
			}

		self.HEADER_DICT = None
		self.SPECIALIST_ICON_DICT = None
		self.AUTOMATION_ICON_DICT = None
		self.COLOR_DICT = None

		# Input handling functions
		self.RevolutionWatchScreenInputMap = {
			self.SPECIALIST_PLUS_NAME	: self.HandleSpecialistPlus,
			self.SPECIALIST_MINUS_NAME	: self.HandleSpecialistMinus,
			self.EXIT_NAME				: self.RevolutionWatchExit,

			self.ADDCOLUMN_NAME			: self.AddCol,
			self.DELCOLUMN_NAME			: self.DelCol,
			self.COLUMNUP_NAME			: self.MoveColUp,
			self.COLUMNDN_NAME			: self.MoveColDn,
			self.COLUMN_SHRINK_NAME		: self.shrinkCol,
			self.COLUMN_WIDEN_NAME		: self.widenCol,

			self.START_CUSTOMIZING_NAME	: self.ModifyPage,
			self.SAVE_NAME				: self.save,
			self.ADD_PAGE_NAME			: self.addPage,
			self.DEL_PAGE_NAME			: self.delPage,
			self.PREV_PAGE_NAME			: self.previousPage,
			self.NEXT_PAGE_NAME			: self.nextPage,
			self.PAGE_UP_NAME			: self.upPage,
			self.PAGE_DOWN_NAME			: self.downPage,

			self.TOGGLE_SPECS_NAME		: self.toggleShowSpecialistControls,

			self.RELOAD_PAGES_NAME		: self.reloadPages,
			self.RENAME_PAGE_NAME		: self.renamePage,

			}

	def createDictionaries(self):
		"""
		Creates Dictionaries we couldn't on init.
		"""

		if(self.runtimeInitDone):
			return

		self.HURRY_TYPE_POP = gc.getInfoTypeForString("HURRY_POPULATION")
		self.HURRY_TYPE_GOLD = gc.getInfoTypeForString("HURRY_GOLD")

		self.angryIcon = u"%c" % CyGame().getSymbolID(FontSymbols.ANGRY_POP_CHAR)
		self.commerceIcon = u"%c" %(gc.getYieldInfo(YieldTypes.YIELD_COMMERCE).getChar())
		self.cultureIcon = u"%c" %(gc.getCommerceInfo(CommerceTypes.COMMERCE_CULTURE).getChar())
		self.defenseIcon = u"%c" % CyGame().getSymbolID(FontSymbols.DEFENSE_CHAR)
		self.espionageIcon = u"%c" %(gc.getCommerceInfo(CommerceTypes.COMMERCE_ESPIONAGE).getChar())
		self.fistIcon = u"%c" % CyGame().getSymbolID(FontSymbols.OCCUPATION_CHAR)
		self.foodIcon = u"%c" %(gc.getYieldInfo(YieldTypes.YIELD_FOOD).getChar())
		self.goldIcon = u"%c" %(gc.getCommerceInfo(CommerceTypes.COMMERCE_GOLD).getChar())
		self.redGoldIcon = u"%c" % CyGame().getSymbolID(FontSymbols.BAD_GOLD_CHAR)
		self.figureheadIcon = u"%c" % CyGame().getSymbolID(FontSymbols.GREAT_PEOPLE_CHAR)
		self.hammerIcon = u"%c" %(gc.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar())
		self.happyIcon = u"%c" % CyGame().getSymbolID(FontSymbols.HAPPY_CHAR)
		self.healthIcon = u"%c" % CyGame().getSymbolID(FontSymbols.HEALTHY_CHAR)
		self.lawIcon = u"%c" % CyGame().getSymbolID(FontSymbols.DEFENSIVE_PACT_CHAR)
		self.militaryIcon = u"%c" % CyGame().getSymbolID(FontSymbols.STRENGTH_CHAR)
		self.powerIcon = u"%c" % CyGame().getSymbolID(FontSymbols.POWER_CHAR)
		self.redfoodIcon = u"%c" % CyGame().getSymbolID(FontSymbols.BAD_FOOD_CHAR)
		self.researchIcon = u"%c" %(gc.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar())
		self.sickIcon = u"%c" % CyGame().getSymbolID(FontSymbols.UNHEALTHY_CHAR)
		self.tradeIcon = u"%c" % CyGame().getSymbolID(FontSymbols.TRADE_CHAR)
		self.unhappyicon = u"%c" % CyGame().getSymbolID(FontSymbols.UNHAPPY_CHAR)

		self.yieldIcons = {}
		for eYieldType in range(YieldTypes.NUM_YIELD_TYPES):
			info = gc.getYieldInfo(eYieldType)
			self.yieldIcons[eYieldType] = u"%c" % info.getChar()

		self.commerceIcons = {}
		for eCommerceType in range(CommerceTypes.NUM_COMMERCE_TYPES):
			info = gc.getCommerceInfo(eCommerceType)
			self.commerceIcons[eCommerceType] = u"%c" % info.getChar()

		self.starIcon = u"%c" % CyGame().getSymbolID(FontSymbols.STAR_CHAR)
		self.silverStarIcon = u"%c" % CyGame().getSymbolID(FontSymbols.SILVER_STAR_CHAR)
		self.bulletIcon = u"%c" % CyGame().getSymbolID(FontSymbols.BULLET_CHAR)

		# Special symbols for building, wonder and project views
		self.objectIsPresent = "x"
		self.objectIsNotPresent = "-"
		self.objectCanBeBuild = "o"
		self.objectUnderConstruction = self.hammerIcon

		# add the colors dependant on the statuses
		self.objectHave = localText.changeTextColor (self.objectIsPresent, gc.getCOLOR_GREEN()) #"x"
		self.objectNotPossible = localText.changeTextColor (self.objectIsNotPresent, gc.getCOLOR_RED()) #"-"
		self.objectPossible = localText.changeTextColor (self.objectCanBeBuild, gc.getCOLOR_BLUE()) #"o"
		self.objectHaveObsolete = localText.changeTextColor (self.objectIsPresent, gc.getCOLOR_WHITE()) #"x"
		self.objectNotPossibleConcurrent = localText.changeTextColor (self.objectIsNotPresent, gc.getCOLOR_YELLOW()) #"-"
		self.objectPossibleConcurrent = localText.changeTextColor (self.objectCanBeBuild, gc.getCOLOR_YELLOW()) #"o"

		# Corporation Yield and Commerce values by Bonus
		# Maps are { bonus -> { yield/commerce -> { corporation -> value } } }
		self.corpMaintPercent = gc.getWorldInfo(gc.getMap().getWorldSize()).getCorporationMaintenancePercent()
		self.bonusCorpYields = {}
		self.bonusCorpCommerces = {}
		for eCorp in range(gc.getNumCorporationInfos()):
			info = gc.getCorporationInfo(eCorp)
			for eBonus in info.getPrereqBonuses():

				for eYield in range(YieldTypes.NUM_YIELD_TYPES):
					iYieldValue = info.getYieldProduced(eYield)
					if (iYieldValue != 0):
						if (not self.bonusCorpYields.has_key(eBonus)):
							self.bonusCorpYields[eBonus] = {}
						if (not self.bonusCorpYields[eBonus].has_key(eYield)):
							self.bonusCorpYields[eBonus][eYield] = {}
						if (not self.bonusCorpYields[eBonus][eYield].has_key(eCorp)):
							self.bonusCorpYields[eBonus][eYield][eCorp] = iYieldValue

				for eCommerce in range(CommerceTypes.NUM_COMMERCE_TYPES):
					iCommerceValue = info.getCommerceProduced(eCommerce)
					if (iCommerceValue != 0):
						if (not self.bonusCorpCommerces.has_key(eBonus)):
							self.bonusCorpCommerces[eBonus] = {}
						if (not self.bonusCorpCommerces[eBonus].has_key(eCommerce)):
							self.bonusCorpCommerces[eBonus][eCommerce] = {}
						if (not self.bonusCorpCommerces[eBonus][eCommerce].has_key(eCorp)):
							self.bonusCorpCommerces[eBonus][eCommerce][eCorp] = iCommerceValue

		self.loadPages()

		self.BUILDING_ICONS_DICT = { }
		self.BUILDING_DICT = { }
		self.BUILDING_INFO_LIST = []

		for i in range(gc.getNumBuildingInfos()):
			info = gc.getBuildingInfo(i)
			desc = info.getDescription()
			key = self.getBuildingKey(i)

			self.BUILDING_DICT[key] = i
			self.BUILDING_INFO_LIST.append(info)

			icon = ""

			if info.isCapital() > 0:
				icon += self.starIcon
			elif info.isGovernmentCenter() > 0:
				icon += self.silverStarIcon

			if info.getReligionType() != -1:
				if info.getHolyCity() != -1:
					icon += u"%c" %(gc.getReligionInfo(info.getReligionType()).getHolyCityChar())
				else:
					icon += u"%c" %(gc.getReligionInfo(info.getReligionType()).getChar())

			if info.getFoodKept() > 0:
				icon += self.foodIcon
			else:
				for entry in info.getPlotYieldChange():
					if entry.iIndex == YieldTypes.YIELD_FOOD and entry.iValue > 0:
						icon += self.foodIcon
						break

			if info.getFreeExperience() > 0 or \
				info.getFreePromoTypes() or \
				info.getGlobalFreeExperience() > 0 or \
				info.getDomainFreeExperience(DomainTypes.DOMAIN_LAND) > 0 or \
				info.getDomainFreeExperience(DomainTypes.DOMAIN_SEA) > 0 or \
				info.getDomainFreeExperience(DomainTypes.DOMAIN_AIR) > 0:
				icon += self.militaryIcon

			if info.getDefenseModifier() > 0 or \
				info.getAllCityDefenseModifier() > 0 or \
				info.getNukeModifier() < 0 or \
				info.getAirModifier() < 0:
				icon += self.defenseIcon

			if info.isPower():
				icon += self.powerIcon

			if info.getWarWearinessModifier() < 0 or info.getAnarchyModifier() > 0 or info.getGlobalWarWearinessModifier() < 0:
				icon += "[" + self.angryIcon + "]"

			if info.getCommerceModifier(CommerceTypes.COMMERCE_CULTURE) > 0:
				icon += self.cultureIcon

			if info.getCommerceModifier(CommerceTypes.COMMERCE_GOLD) > 0:
				icon += self.goldIcon

			if info.getCommerceModifier(CommerceTypes.COMMERCE_RESEARCH) > 0:
				icon += self.researchIcon

			if info.getCommerceModifier(CommerceTypes.COMMERCE_ESPIONAGE) > 0:
				icon += self.espionageIcon

			if info.getSpaceProductionModifier() > 0 \
				or info.getDomainProductionModifier(DomainTypes.DOMAIN_LAND) > 0 \
				or info.getDomainProductionModifier(DomainTypes.DOMAIN_SEA) > 0 \
				or info.getDomainProductionModifier(DomainTypes.DOMAIN_AIR) > 0 \
				or info.getYieldChange(YieldTypes.YIELD_PRODUCTION) > 0:
				icon += self.hammerIcon

			if info.getTradeRouteModifier() > 0 or info.getTradeRoutes() > 0 or info.getCoastalTradeRoutes() > 0 \
			or info.getGlobalTradeRoutes() > 0 or info.getForeignTradeRouteModifier() > 0:
				icon += self.tradeIcon

			if info.getMaintenanceModifier() < 0:
				icon += self.commerceIcon

			if info.isNoUnhappiness():
				icon += self.happyIcon

			if info.isBuildingOnlyHealthy():
				icon += self.healthIcon

			if info.getHealth() < 0 or info.getGlobalHealth() < 0:
				icon += self.sickIcon

			if info.getGreatPeopleRateChange() > 0 or info.getGreatPeopleRateModifier() > 0:
				icon += self.figureheadIcon

			if info.getMovie():
				icon += self.starIcon

			self.COLUMNS_LIST.append((key, 50 + 15 * len(icon), "text", None, None, 0, self.calculateValue, None, u"u\"" + desc + u"\""))
			self.BUILDING_ICONS_DICT[key] = icon

		# Hurry types
		for i in range(gc.getNumHurryInfos()):
			header = "self.hammerIcon"
			name = "CAN_HURRY_"
			info = gc.getHurryInfo(i)
			if info.getGoldPerProduction() > 0:
				header += " + self.goldIcon"
				name += "GOLD"
			if info.getProductionPerPopulation() > 0 and info.isAnger():
				header += " + self.angryIcon"
				name += "WHIP"
			elif info.getProductionPerPopulation() > 0:
				header += " + self.happyIcon"
				name += "VOLUNTEERS"

			self.COLUMNS_LIST.append((name, 50, "text", None, None, 0, self.canHurry, i, header))

		# Resources ("bonuses") -- presence
		for i in range(gc.getNumBonusInfos()):
			info = gc.getBonusInfo(i)
			desc = u"%c" % info.getChar()
			key = "HAS_" + self.getBonusKey(i)

			self.COLUMNS_LIST.append((key, 24, "bonus", None, None, 0, self.calculateHasBonus, i, "u\"" + desc + "\""))

		# Resources ("bonuses") -- effects
		for i in range(gc.getNumBonusInfos()):
			info = gc.getBonusInfo(i)
			desc = u"%c" % info.getChar()
			key = self.getBonusKey(i)

			self.COLUMNS_LIST.append((key, 50, "text", None, None, 0, self.calculateBonus, i, "u\"" + desc + "\""))

		self.COLUMNS_INDEX = { }
		self.HEADER_DICT = { }

		for i, column in enumerate(self.COLUMNS_LIST):
			self.COLUMNS_INDEX[column[0]] = i
			self.HEADER_DICT[column[0]] = eval(column[8], globals(), locals())

		if self.SPECIALIST_ICON_DICT == None:
			# Specialist Icon Information (Must be here, because C++ functions aren't
			# available upon startup of CIV)
			self.SPECIALIST_ICON_DICT = {
				0 : self.bulletIcon, # Citizen
				1 : self.happyIcon, # Priest
				2 : self.cultureIcon, # Artist
				3 : self.researchIcon, # Scientist
				4 : self.goldIcon, # Merchant
				5 : self.hammerIcon, # Engineer
				6 : self.espionageIcon, # Engineer
				}

		if self.AUTOMATION_ICON_DICT == None:
			# Automation Information (Must be here, because C++ functions aren't
			# available upon startup of CIV)
			self.AUTOMATION_ICON_DICT = {
				0 : self.foodIcon, # Emphasize Food
				1 : self.hammerIcon, # Emphasize Production
				2 : self.goldIcon, # Emphasize Gold?
				3 : self.researchIcon, # Emphasize Research
				4 : self.figureheadIcon, # Emphasize GP
				5 : self.redfoodIcon, # Emphasize Avoid Growth
				}

		if self.COLOR_DICT == None:
			# Colors to highlight with for each type of number (Must be here,
			#  because C++ functions aren't available upon startup of CIV)
			self.COLOR_DICT = {
				"PROBLEM": gc.getCOLOR_RED(),
				"NEUTRAL": gc.getCOLOR_YELLOW(),
				"GREAT": gc.getCOLOR_GREEN(),
				}

		self.switchPage(self.PAGES[0]["name"])

		self.runtimeInitDone = True

	def createPositions (self, screen):
		""" Gets the basic positions to draw on. """
		self.nScreenX = screen.centerX(0)
		self.nScreenY = screen.centerY(0)

	def getScreen(self):
		""" Return the screen we draw with. """
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REVOLUTION_WATCH_ADVISOR)

	#RevolutionDCM - used by bribe popup to determine whether this advisor is in the forefront
	def isVisible(self):
		if (self.currentPage == None and self.visiblePage == None):
			return False
		return True

	def getCurrentCity (self):
		""" Get the current selected city."""
		screen = self.getScreen()
		lCity = gc.getActivePlayer().cities()

		for i in xrange(len(lCity)):
			if screen.isRowSelected(self.currentPage, i):
				for city in lCity:
					if city.getName() == screen.getTableText(self.currentPage, 1, i):
						return city
		return None


	def getNumSpecialistInfos (self):
		""" Get the number of specialist types (that WE deal with)."""
		try:
			return len (self.SPECIALIST_ICON_DICT)
		except TypeError:
			return 0

	def getNumEmphasizeInfos (self):
		""" Get the number of emphasis types (that WE deal with)."""
		return len (self.AUTOMATION_ICON_DICT)

	def interfaceScreen(self):
		"""
		Screen construction function.
		This is the function that's called whenever the hot key is pressed.
		"""

		# Initialize all the stuff we couldn't in the init function
		self.createDictionaries()

		screen = self.getScreen()
#		screen.setForcedRedraw (True)
		self.createPositions (screen)

		screen.setDimensions (self.nScreenX, self.nScreenY, self.nScreenWidth, self.nScreenLength)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		# Draw the basic screen
		self.drawBasicScreen()

		# Draw the table and the rest based on the mode
		self.drawScreen (self.currentPage)

# BUG - Colony Split - start

		player = gc.getActivePlayer()
		if not player.canSplitEmpire():
			self.bCanLiberate = False
			for loopCity in player.cities():
				if loopCity.getLiberationPlayer(False) != -1:
					self.bCanLiberate = True
					break
		else: self.bCanLiberate = True

		if (self.bCanLiberate):
			screen.setImageButton( self.SPLIT_NAME, "", self.X_SPLIT, self.Y_SPLIT, 28, 28, WidgetTypes.WIDGET_ACTION, gc.getControlInfo(ControlTypes.CONTROL_FREE_COLONY).getActionInfoIndex(), -1 )
			screen.setStyle( self.SPLIT_NAME, "Button_HUDAdvisorVictory_Style" )

# BUG - Colony Split - end

	def drawBasicScreen (self):
		"""
		Draws the Basic parts common to all Domestic Advisor Screens.
		"""
		screen = self.getScreen()

		# Here we set the background widget and exit button, and we show the screen
		screen.addPanel( self.BACKGROUND_ID, u"", u"", True, False, self.nPanelX, self.nPanelY, self.nPanelWidth, self.nPanelLength, PanelStyles.PANEL_STYLE_MAIN )
		#screen.addDDSGFC( self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("SCREEN_BG").getPath(), 0, 29, 1024, 592, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		# Text Buttons
		screen.setText(self.EXIT_NAME, "Background", localText.getText("TXT_WORD_EXIT", ()).upper(), 1<<1, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		x = self.X_SPECIAL + self.PAGES_DD_W + 10

		# Buttons to switch screens
		screen.setImageButton( self.PREV_PAGE_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_LEFT").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x += self.nControlSize + 2
		screen.setImageButton( self.NEXT_PAGE_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_RIGHT").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x += self.nControlSize + 12
		screen.addCheckBoxGFC(self.START_CUSTOMIZING_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BTN_FOREIGN").getPath(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_IMAGE )
		x += self.nControlSize + 2
		screen.setImageButton( self.RENAME_PAGE_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BTN_EVENT_LOG").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x += self.nControlSize + 2
		info = gc.getSpecialistInfo(gc.getInfoTypeForString("SPECIALIST_CITIZEN"))
		screen.addCheckBoxGFC(self.TOGGLE_SPECS_NAME, info.getTexture(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_IMAGE )
		x += self.nControlSize + 2
		screen.setImageButton( self.ADD_PAGE_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_PLUS").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x += self.nControlSize + 2
		screen.setImageButton( self.DEL_PAGE_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_MINUS").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x += self.nControlSize + 2
		screen.setImageButton( self.PAGE_UP_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_GENERAL_UPARROW").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x += self.nControlSize + 2
		screen.setImageButton( self.PAGE_DOWN_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_GENERAL_DOWNARROW").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x += self.nControlSize + 12
		screen.setImageButton( self.SAVE_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_GENERAL_MENU_ICON").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x += self.nControlSize + 2
		screen.setImageButton( self.RELOAD_PAGES_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath(), x, self.Y_SPECIAL, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x += self.nControlSize + 2

		# RevolutionDCM legend content initial - start
		self.showRevolutionLegend()
		# RevolutionDCM legend content - end

		# Header...
		#szText = "<font=4>" + localText.getText("TXT_KEY_REVOLUTION_WATCH_ADVISOR_TITLE", ()).upper() + "</font>"
		#screen.setLabel( "RevolutionWatchTitleHeader", "Background", szText, 1<<2, 472, 40, STANDARD_Z, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		# Draw the customization controls (but don't SHOW them)
		self.drawCustomizationControls()

	# Draw the specialist and their increase and decrease buttons
	def drawSpecialists (self):
		""" The function to draw but not show all the specialists."""
		screen = self.getScreen()

		# Citizen Buttons
		for i in range( self.getNumSpecialistInfos() ):

			if (gc.getSpecialistInfo(i).isVisible()):

				szName = self.SPECIALIST_IMAGE_NAME + str(i)
				screen.setImageButton( szName, gc.getSpecialistInfo(i).getTexture(), self.nFirstSpecialistX + (self.nSpecialistDistance * i), self.nSpecialistY, self.nSpecialistWidth, self.nSpecialistLength, WidgetTypes.WIDGET_CITIZEN, i, -1 )
				screen.hide( szName )

		# Increase Specialists...
		for i in range( self.getNumSpecialistInfos() ):
			if (gc.getSpecialistInfo(i).isVisible()):
				szName = self.SPECIALIST_PLUS_NAME + str(i)
				screen.setButtonGFC( szName, u"", "", self.nFirstSpecialistX + (self.nSpecialistDistance * i) + self.nPlusOffsetX, self.nSpecialistY + self.nPlusOffsetY, self.nPlusWidth, self.nPlusHeight, WidgetTypes.WIDGET_CHANGE_SPECIALIST, i, 1, ButtonStyles.BUTTON_STYLE_CITY_PLUS )
				screen.hide( szName )

		# Decrease specialists
		for i in range( self.getNumSpecialistInfos() ):
			if (gc.getSpecialistInfo(i).isVisible()):
				szName = self.SPECIALIST_MINUS_NAME + str(i)
				screen.setButtonGFC( szName, u"", "", self.nFirstSpecialistX + (self.nSpecialistDistance * i) + self.nMinusOffsetX, self.nSpecialistY + self.nMinusOffsetY, self.nMinusWidth, self.nMinusHeight, WidgetTypes.WIDGET_CHANGE_SPECIALIST, i, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS )
				screen.hide( szName )

		# Specialist text
		for i in range (self.getNumSpecialistInfos()):
			if (gc.getSpecialistInfo(i).isVisible()):
				szName = self.SPECIALIST_TEXT_NAME + str(i)
				screen.setText (szName, "Background", "", 1<<0, self.nFirstSpecialistX + (self.nSpecialistDistance * i) + self.nSpecTextOffsetX, self.nSpecialistY + self.nSpecTextOffsetY, self.Z_TEXT, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.hide (szName)

	def showSpecialists (self):
		""" Function which shows the specialists."""
		pass
		#screen = self.getScreen()
		#
		# First, hide the specialists! :D
		#self.hideSpecialists()
		#
		# Get the current city
		#city = self.getCurrentCity()
		#if (city):
		#
		#	# Get values which we will need for each calculation
		#	nPopulation = city.getPopulation()
		#	nFreeSpecial = city.totalFreeSpecialists()
		#
		#	# For each specialist type
		#	for i in range( self.getNumSpecialistInfos() ):
		#
		#		# Show all the specialist images
		#		szName = self.SPECIALIST_IMAGE_NAME + str(i)
		#		screen.show( szName )
		#
		#		# Show all the specialist text
		#		szName = self.SPECIALIST_TEXT_NAME + str(i)
		#		screen.setText (szName, "Background", str (city.getSpecialistCount(i)) + "/" + str (city.getMaxSpecialistCount(i)) + u" %c" % self.SPECIALIST_ICON_DICT[i], 1<<0, self.nFirstSpecialistX + (self.nSpecialistDistance * i) + self.nSpecTextOffsetX, self.nSpecialistY + self.nSpecTextOffsetY, self.Z_TEXT, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		#		screen.show (szName)
		#
		#		# If the specialist is valid and we can increase it
		#		if (city.isSpecialistValid(i, 1) and (city.getForceSpecialistCount(i) < (nPopulation + nFreeSpecial))):
		#			# Show the Plus
		#			szName = self.SPECIALIST_PLUS_NAME + str(i)
		#			screen.show( szName )
		#
		#		# if we HAVE specialists already and they're not forced.
		#		if (city.getSpecialistCount(i) > 0 or city.getForceSpecialistCount(i) > 0 ):
		#			# Show the Minus
		#			szName = self.SPECIALIST_MINUS_NAME + str(i)
		#			screen.show( szName )

	def hideSpecialists (self):
		""" Function to hide all the specialists and the accompanying data."""
		pass
		#screen = self.getScreen()
		#
		# Hide Everything related to specialists
		#for i in range( self.getNumSpecialistInfos() ):
		#	szName = self.SPECIALIST_IMAGE_NAME + str(i)
		#	screen.hide (szName)
		#	szName = self.SPECIALIST_PLUS_NAME + str(i)
		#	screen.hide (szName)
		#	szName = self.SPECIALIST_MINUS_NAME + str(i)
		#	screen.hide (szName)
		#	szName = self.SPECIALIST_TEXT_NAME + str(i)
		#	screen.hide (szName)

	# RevolutionDCM - legend control 1
	def showRevolutionLegend (self):
		screen = self.getScreen()
		city = self.getCurrentCity()

		screen.addPanel(self.BRIBE_PANEL, "", "", False, True, self.nBribePanelX, self.nBribePanelY, self.nBribePanelWidth, self.nBribePanelLength, PanelStyles.PANEL_STYLE_IN )
		screen.setImageButton(self.REVOLUTION_LEGEND_ICON, self.revIcon, self.revIconX, self.revIconY, self.revIconSize, self.revIconSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setText (self.REVOLUTION_BRIBE_TEXT, "Background", localText.getText("TXT_ADVISOR_BRIBE",()), 1<<0, self.revBribeTextX, self.revBribeTextY, self.Z_TEXT, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addPanel(self.LEGEND_PANEL, "", "", False, True, self.nLegendPanelX, self.nLegendPanelY, self.nLegendPanelWidth, self.nLegendPanelLength, PanelStyles.PANEL_STYLE_IN )

		#always show national effects even if a city has not been selected
		player = CyGame().getActivePlayer()
		turn = CyGame().getGameTurn()
		revText = "xxx" + RevInstances.RevolutionInst.updateCivStability(turn, player, True, True)
		revText = revText.strip('\n')
		revText = revText.replace('\n', ' ')
		revText = revText.replace("xxx", "\n")
		screen.attachMultilineText( self.LEGEND_PANEL, "Text", revText, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		self.hideRevolutionLegend()
		screen.show(self.BRIBE_PANEL)
		screen.show(self.LEGEND_PANEL)

		if city:
			player = city.getOwner()
			turn = CyGame().getGameTurn()
			revText = RevInstances.RevolutionInst.updateLocalRevIndices(turn, player, [city], True)
			revText += "xxx" + RevInstances.RevolutionInst.updateCivStability(turn, player, True, True)
			revText = revText.strip('\n')
			revText = revText.replace('\n', ' ')
			revText = revText.replace("xxx", "\n")
			screen.attachMultilineText( self.LEGEND_PANEL, "Text", revText, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
			if RevUtils.isCanBribeCity(city)[0]:
				screen.setText(self.REVOLUTION_BRIBE_TEXT, "Background", localText.getText("TXT_ADVISOR_BRIBE",()) + " " + city.getName(), 1<<0, self.revBribeTextX, self.revBribeTextY, self.Z_TEXT, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.show(self.BRIBE_PANEL)
				screen.show(self.LEGEND_PANEL)
				screen.show(self.REVOLUTION_LEGEND_ICON)
				screen.show(self.REVOLUTION_BRIBE_TEXT)
			else:
				self.hideRevolutionLegend()
				screen.show(self.BRIBE_PANEL)
				screen.show(self.LEGEND_PANEL)

	# RevolutionDCM - legend control 2
	def hideRevolutionLegend (self):
		screen = self.getScreen()
		screen.hide(self.BRIBE_PANEL)
		screen.hide(self.LEGEND_PANEL)
		screen.hide(self.REVOLUTION_LEGEND_ICON)
		screen.hide(self.REVOLUTION_LEGEND_TEXT)
		screen.hide(self.REVOLUTION_BRIBE_TEXT)


	def showGPLegend(self):
		pass
		#screen = self.getScreen()
		#screen.show(self.GP_TEXT_NAME)
		#screen.show(self.GP_TEXT_NAME + self.NUMBER_TEXT)

	def hideGPLegend(self):
		pass
		#screen = self.getScreen()
		#screen.hide(self.GP_TEXT_NAME)
		#screen.hide(self.GP_TEXT_NAME + self.NUMBER_TEXT)

	def drawCustomizationControls(self):
		screen = self.getScreen()

		x = self.nTableX + self.nHalfTableWidth - self.nControlSize
		screen.setImageButton( self.COLUMNDN_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_GENERAL_DOWNARROW").getPath(), x, self.nCustomizeControlY + self.nCustomizeControlYoffset, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x -= self.nControlSize + 2
		screen.setImageButton( self.COLUMNUP_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_GENERAL_UPARROW").getPath(), x, self.nCustomizeControlY + self.nCustomizeControlYoffset, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x -= self.nControlSize + 2
		screen.setImageButton( self.DELCOLUMN_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_MINUS").getPath(), x, self.nCustomizeControlY + self.nCustomizeControlYoffset, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x -= self.nControlSize + 2
		screen.setImageButton( self.ADDCOLUMN_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_PLUS").getPath(), x, self.nCustomizeControlY + self.nCustomizeControlYoffset, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x -= self.nControlSize + 2
		screen.setImageButton( self.COLUMN_WIDEN_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_RIGHT").getPath(), x, self.nCustomizeControlY + self.nCustomizeControlYoffset, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		x -= self.nControlSize + 2
		screen.setImageButton( self.COLUMN_SHRINK_NAME, ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_LEFT").getPath(), x, self.nCustomizeControlY + self.nCustomizeControlYoffset, self.nControlSize, self.nControlSize, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.hideCustomizationControls()

	def showCustomizationControls(self):

		screen = self.getScreen()

		screen.setState(self.START_CUSTOMIZING_NAME, True)

		# No need for screen.show(self.CUSTOMIZE_PAGE) - that table is redrawn.
		screen.show(self.COLUMNS_LIST_PAGE)
		screen.show(self.ADDCOLUMN_NAME)
		screen.show(self.DELCOLUMN_NAME)
		screen.show(self.COLUMNUP_NAME)
		screen.show(self.COLUMNDN_NAME)
		screen.show(self.COLUMN_SHRINK_NAME)
		screen.show(self.COLUMN_WIDEN_NAME)

		if self.isFlavorful:

			screen.show(self.RENAME_PAGE_NAME)
			screen.show(self.RENAME_PAGE_NAME)
			screen.show(self.ADD_PAGE_NAME)
			screen.show(self.DEL_PAGE_NAME)
			screen.show(self.PAGE_UP_NAME)
			screen.show(self.PAGE_DOWN_NAME)
			screen.show(self.SAVE_NAME)
			screen.show(self.RELOAD_PAGES_NAME)
			screen.show(self.TOGGLE_SPECS_NAME)

			page = self.PAGES[self.currentPageNum]
			screen.setState(self.TOGGLE_SPECS_NAME, page["showSpecControls"])

	def hideCustomizationControls(self):

		screen = self.getScreen()

		screen.setState(self.START_CUSTOMIZING_NAME, False)

		screen.hide(self.CUSTOMIZE_PAGE)
		screen.hide(self.COLUMNS_LIST_PAGE)
		screen.hide(self.ADDCOLUMN_NAME)
		screen.hide(self.DELCOLUMN_NAME)
		screen.hide(self.COLUMNUP_NAME)
		screen.hide(self.COLUMNDN_NAME)
		screen.hide(self.COLUMN_SHRINK_NAME)
		screen.hide(self.COLUMN_WIDEN_NAME)

		if self.isFlavorful:

			screen.hide(self.RENAME_PAGE_NAME)
			screen.hide(self.RENAME_PAGE_NAME)
			screen.hide(self.ADD_PAGE_NAME)
			screen.hide(self.DEL_PAGE_NAME)
			screen.hide(self.PAGE_UP_NAME)
			screen.hide(self.PAGE_DOWN_NAME)
			screen.hide(self.SAVE_NAME)
			screen.hide(self.RELOAD_PAGES_NAME)
			screen.hide(self.TOGGLE_SPECS_NAME)

	def hide (self, screen, page):
		""" Hide function which hides a specific screen."""
		screen.hide (page)
		self.hideSpecialists()

	def drawScreen (self, page):
		""" Draw the screen based on which mode we get."""
		screen = self.getScreen()

		# Change the visible page?
		if(not self.customizing and self.visiblePage != self.currentPage):
			if(self.visiblePage):
				screen.hide(self.visiblePage)

			screen.show(self.currentPage)
			self.visiblePage = self.currentPage

		# Hide the menu and Civilopedia buttons
		CyInterface().setDirty(InterfaceDirtyBits.MiscButtons_DIRTY_BIT, True)

		start = time.clock()
		# Draw the city list...
		self.drawContents(page)
		end = time.clock()
		print "drawContents: " + str(end - start) + "s"

	def calculateFounded(self, city, szKey, arg):

		# City founded date...
		iTurnTime = city.getGameDateFounded()
		bACalendar = GAME.getModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_USE_HISTORICAL_ACCURATE_CALENDAR)
		if bACalendar:
			return iTurnTime
		return unicode(CyGameTextMgr().getTimeStr(iTurnTime, False))

	def calculateFeatures(self, city, szKey, arg):

		szReturn = ""

		# First look for Government Centers
		if city.isGovernmentCenter():
			# And distinguish between the Capital and the others (Forbidden Palace
			# and Versailles)
			if city.isCapital():
				szReturn += self.starIcon
			else:
				szReturn += self.silverStarIcon

		# add National Wonders
		for i in range(gc.getNumBuildingInfos()):
			info = gc.getBuildingInfo(i)
			if info.getMaxGlobalInstances() == -1 and info.getMaxPlayerInstances() == 1 and city.hasBuilding(i) and not info.isCapital():
				# Use bullets as markers for National Wonders
				szReturn += self.bulletIcon

		if city.isDisorder():

			if city.isOccupation():
				szOccu = u"%c" % CyGame().getSymbolID(FontSymbols.OCCUPATION_CHAR)
				szReturn += szOccu +":"+unicode(city.getOccupationTimer())
			else:
				szReturn += self.angryIcon

		pTradeCity = city.getTradeCity(0)
		if (pTradeCity and pTradeCity.getOwner() >= 0):
			szReturn += self.tradeIcon

		return szReturn

	def calculateDefense(self, city, szKey, arg):
		return unicode(city.plot().defenseModifier(-1, True, False)) + u"%"

	def calculateEspionageDefense(self, city, szKey, arg):
		return unicode(city.getEspionageDefenseModifier()) + u"%"

	def calculateThreats(self, city, szKey, arg):

		team = gc.getTeam(gc.getGame().getActiveTeam())

		for iTeamX in range(gc.getMAX_TEAMS()):
			if team.isAtWarWith(iTeamX):
				if city.isVisible(iTeamX, False):
					return self.angryIcon
		return ""

	def calculateNetHappiness (self, city, szKey="", arg=0):
		return city.happyLevel() - city.unhappyLevel(0)

	def calculateNetHealth (self, city, szKey="", arg=0):
		return city.goodHealth() - city.badHealth(False)

	def calculateGrowth (self, city, szKey, arg):
		szReturn = u""

		# Turns til Growth
 		nFood = city.foodDifference (True)

		# If this is a food production (i.e., worker or settler)
		if (self.isFoodProduction(city.getProductionName())):
			# We need to indicate there's no growth manually
			szReturn = u"-"
		else:
			# Otherwise, just call FoodTurnsLeft
			szReturn = unicode (city.getFoodTurnsLeft())

		# Not enough food, so calculate how many turns until we starve.
		# We put this here because we still starve if building a food production
		if nFood < 0:
			# Use floor divide (//) because we want the number it'll drop below 0
			# (that's also the reason for the additional 1)
			szReturn = unicode ((city.getFood()+1) // nFood)

		return szReturn

	def calculateFood (self, city, szKey, arg):

		# If this is a food production (i.e., worker or settler)
		if (self.isFoodProduction(city.getProductionName())):
			nFood = city.getCurrentProductionDifference (False, False) - city.getCurrentProductionDifference (True, False)
		else:
			nFood = city.foodDifference (True)
		return nFood

	def calculateProduction (self, city, szKey, arg):
		return unicode(city.getCurrentProductionDifference (True, False))

	def calculateMaintenance (self, city, szKey, arg):
		return unicode(city.getMaintenance())

	def calculateTrade (self, city, szKey, arg):
		"""arg: None to sum all, 'D' to count domestic, 'F' to count foreign."""
		nTotalTradeProfit = 0

		# For each trade route possible
		for nTradeRoute in xrange(city.getMaxTradeRoutes()):
			# Get the next trade city
			pTradeCity = city.getTradeCity(nTradeRoute)
			# Not quite sure what this does but it's in the MainInterface
			# and I pretty much C&Ped :p
			if (pTradeCity and pTradeCity.getOwner() >= 0):
				bForeign = city.getOwner() != pTradeCity.getOwner()
				if (not arg or ((arg == "F" and bForeign) or (arg == "D" and not bForeign))):
					for j in range( YieldTypes.NUM_YIELD_TYPES ):
						iTradeProfit = city.calculateTradeYield(j, city.calculateTradeProfit(pTradeCity))

						# If the TradeProfit is greater than 0, add it to the total
						if ( iTradeProfit > 0 ):
							nTotalTradeProfit += iTradeProfit

		return unicode(nTotalTradeProfit)

	def countTradeRoutes (self, city, szKey, arg):
		"""arg: None to count all, 'D' to count domestic, 'F' to count foreign."""
		nRoutes = 0

		# For each trade route possible
		for nTradeRoute in xrange(city.getMaxTradeRoutes()):
			# Get the next trade city
			pTradeCity = city.getTradeCity(nTradeRoute)
			# Not quite sure what this does but it's in the MainInterface
			# and I pretty much C&Ped :p
			if (pTradeCity and pTradeCity.getOwner() >= 0):
				bForeign = city.getOwner() != pTradeCity.getOwner()
				if (not arg or ((arg == "F" and bForeign) or (arg == "D" and not bForeign))):
					nRoutes += 1

		return unicode(nRoutes)

	def calculateProducing (self, city, szKey, arg):

		szReturn = u""
		# If there's something in the queue,
		if (city.getOrderQueueLength() > 0):

			# Get the name of whatever it's producing.
			szReturn = city.getProductionName()

		# Otherwise we're not producing anything. Leave it blank.
		else:
			szReturn = u"-"

		return szReturn

	def calculateProducingTurns (self, city, szKey, arg):

		szReturn = u""

		# If there's something in the queue,
		if (city.getOrderQueueLength() > 0):

			# If it's not a process (i.e., Wealth, Research, Culture)
			if (not (city.isProductionProcess())):

				szReturn = unicode(city.getProductionTurnsLeft())

		return szReturn

	def calculateWhipPopulation (self, city, szKey, arg):
		if city.canHurry(self.HURRY_TYPE_POP, False):
			return unicode(city.hurryPopulation(self.HURRY_TYPE_POP))

		return self.objectNotPossible

	def calculateWhipOverflowProduction (self, city, szKey, arg):
		return self.calculateWhipOverflow(city, szKey, arg)[0]

	def calculateWhipOverflowGold (self, city, szKey, arg):
		return self.calculateWhipOverflow(city, szKey, arg)[1]

	def calculateWhipOverflow (self, city, szKey, arg):

		if not city.canHurry(self.HURRY_TYPE_POP, False):
			return self.objectNotPossible, self.objectNotPossible

		iOverflow = city.hurryProduction(self.HURRY_TYPE_POP) - city.productionLeft()
		if CityScreenOpt.isWhipAssistOverflowCountCurrentProduction():
			iOverflow = iOverflow + city.getCurrentProductionDifference(True, False)
		iMaxOverflow = city.getMaxProductionOverflow()
		iOverflowGold = max(0, iOverflow - iMaxOverflow) * gc.getDefineINT("MAXED_UNIT_GOLD_PERCENT") / 100
		iOverflow =  100 * iMaxOverflow / city.getBaseYieldRateModifier(gc.getInfoTypeForString("YIELD_PRODUCTION"), 0)
		return unicode(iOverflow), unicode(iOverflowGold)

	def calculateHurryGoldCost (self, city, szKey, arg):

		if city.canHurry(self.HURRY_TYPE_GOLD, False):
			return unicode(city.getHurryGold(self.HURRY_TYPE_GOLD))

		return self.objectNotPossible

	def calculatePotentialConscriptUnit (self, city, szKey, arg):

		szReturn = unicode(gc.getUnitInfo(city.getConscriptUnit()).getDescription() )
		return szReturn

	def calculateConscriptUnit (self, city, szKey, arg):

		if(city.canConscript()):
			szReturn = unicode(gc.getUnitInfo(city.getConscriptUnit()).getDescription() )
		else:
			szReturn = u""
		return szReturn

	def calculateReligions (self, city, szKey, arg):

		szReturn = u""

		lHolyCity = []
		lReligions = []
		for i in range(gc.getNumReligionInfos()):
			if city.isHolyCityByType(i):
				lHolyCity.append(i)
			elif city.isHasReligion(i):
				lReligions.append(i)

		for i in range(len(lHolyCity)):
			szReturn += u"%c" %(gc.getReligionInfo(lHolyCity[i]).getHolyCityChar())

		for i in range(len(lReligions)):
			szReturn += u"%c" %(gc.getReligionInfo(lReligions[i]).getChar())

		return szReturn

	def calculateCorporations (self, city, szKey, arg):

		szReturn = u""

		lHeadquarters = []
		lCorps = []
		for i in range(gc.getNumCorporationInfos()):
			if city.isHeadquartersByType(i):
				lHeadquarters.append(i)
			elif city.isHasCorporation(i):
				lCorps.append(i)

		for i in range(len(lHeadquarters)):
			szReturn += u"%c" %(gc.getCorporationInfo(lHeadquarters[i]).getHeadquarterChar())

		for i in range(len(lCorps)):
			szReturn += u"%c" %(gc.getCorporationInfo(lCorps[i]).getChar())

		return szReturn

	def calculateSpecialists (self, city, szKey, arg):

		szReturn = u"<font=1>"

		# For each specialist type
		for i in range( self.getNumSpecialistInfos() ):

			nCount = city.getSpecialistCount (i)
			# If more than one specialist
			if (nCount > 1):
				szReturn += self.SPECIALIST_ICON_DICT[i] + "x" + str(nCount) + " "
			elif (nCount == 1):
				szReturn += self.SPECIALIST_ICON_DICT[i] + " "

		szReturn += u"</font>"

		return szReturn

	def calculateAutomation (self, city, szKey, arg):

		szReturn = u"<font=1>"

		nNumEmphasize = self.getNumEmphasizeInfos()
		if city.isCitizensAutomated():
			szReturn += u"C"
		if city.isProductionAutomated():
			szReturn += u"P"
		for i in range( nNumEmphasize ):
			nNum = nNumEmphasize - i - 1
			if (city.AI_isEmphasize (nNum)):
				szReturn += self.AUTOMATION_ICON_DICT[nNum]

		szReturn += u"</font>"

		return szReturn

	def calculatePower (self, city, szKey, arg):

		szReturn = u""

		if city.isPower():
			szReturn += self.powerIcon

		return szReturn

	def calculateTotalCulture (self, city, szKey, arg):

		return city.getCulture(CyGame().getActivePlayer())

	def calculateCultureTurns (self, city, szKey, arg):

		iCultureRateTimes100 = city.getCommerceRateTimes100(CommerceTypes.COMMERCE_CULTURE)

		if iCultureRateTimes100 > 0 and city.getCultureThreshold() > 0:
			return (100*city.getCultureThreshold() - city.getCultureTimes100(gc.getGame().getActivePlayer()) + iCultureRateTimes100 - 1) / iCultureRateTimes100
		return u"-"

	def calculateGreatPeopleTurns (self, city, szKey, arg):

		iGreatPersonRate = city.getGreatPeopleRate()
		if iGreatPersonRate > 0:
			iGPPLeft = gc.getPlayer(gc.getGame().getActivePlayer()).greatPeopleThresholdNonMilitary() - city.getGreatPeopleProgress()
			return (iGPPLeft + iGreatPersonRate - 1) / iGreatPersonRate

		return u"-"

	def canHurry (self, city, szKey, arg):

		if city.canHurry(arg, False):
			return self.objectHave

		return self.objectNotPossible

	def canLiberate (self, city, szKey, arg):

		if city.getLiberationPlayer(False) != -1:
			return self.objectHave

		return self.objectNotPossible

	def calculateValue (self, city, szKey, arg):

		szReturn = u""

		if szKey.find("BUILDING_") == 0:
			if city.isActiveBuilding(self.BUILDING_DICT[szKey]) or city.getProductionName() == self.HEADER_DICT[szKey]:
				# Start with the default icons, and those that never change.
				szReturn += self.BUILDING_ICONS_DICT[szKey]

				buildingNumber = self.BUILDING_DICT[szKey]
				info = self.BUILDING_INFO_LIST[buildingNumber]

				# Indicate bonus military production if applicable
				if info.getMilitaryProductionModifier() > 0 and city.isProductionUnit() and gc.getUnitInfo(city.getProductionUnit()).isMilitaryProduction():
					if szReturn.find(self.hammerIcon) == -1:
						szReturn += self.hammerIcon

				# Calculate health and happiness effects - including that from resources.
				iHealth = city.getBuildingHealth(buildingNumber)
				iHappiness = city.getBuildingHappiness(buildingNumber)

				for i in range(gc.getNumBonusInfos()):
					if city.hasBonus(i):
						iHealth += info.getBonusHealthChanges().getValue(i)
						iHappiness += info.getBonusHappinessChanges().getValue(i)

				if iHealth > 0:
					szReturn = self.stripStr(szReturn, self.healthIcon)
					szReturn += u"%d%c" %(iHealth, self.healthIcon)
				elif iHealth < 0:
					szReturn = self.stripStr(szReturn, self.sickIcon)
					szReturn += u"%d%c" %( -(iHealth), self.sickIcon )

				# Happiness

				if iHappiness > 0:
					szReturn = self.stripStr(szReturn, self.happyIcon)
					szReturn += u"%d%c" %(iHappiness, self.happyIcon)
				elif iHappiness < 0:
					szReturn = self.stripStr(szReturn, self.unhappyIcon)
					szReturn += u"%d%c" %( -(iHappiness), self.unhappyIcon )

				# Commerce
				for i in range(CommerceTypes.NUM_COMMERCE_TYPES):
					iCommerce = city.getBuildingCommerceByBuilding(i, buildingNumber)
					if iCommerce > 0:
						szReturn += u"%d%c" %( iCommerce, gc.getCommerceInfo(i).getChar() )

				if szReturn == "":
					szReturn = "+"

				if city.getProductionName() == self.HEADER_DICT[szKey]: # In production
					szReturn = "(" + szReturn + ")"

			elif city.getNumBuilding(self.BUILDING_DICT[szKey]) > 0: # Disabled buildings
				if self.BUILDING_ICONS_DICT[szKey].find(self.cultureIcon):
					szReturn = self.stripStr(szReturn, self.cultureIcon)
					szReturn += self.cultureIcon
				else:
					szReturn += u"X"

			elif not city.canConstruct(self.BUILDING_DICT.get(szKey), False, False, False):
				szReturn += u"-"

			else:
				szReturn += u" "

		# return the final value
		return szReturn

	def calculateHasBonus (self, city, szKey, arg):

		# Determine whether or not city has the given bonus
		if city.hasBonus(arg):
			return self.objectHave
		return self.objectNotPossible

	def calculateBonus (self, city, szKey, arg):

		# Determine the effects of the given bonus (health, happiness, commerce)
		if (not city.hasBonus(arg)):
			return self.objectNotPossible

		szEffects = u""
		iEffect = city.getBonusHappiness(arg)
		if (iEffect == 1):
			szEffects += u"%s " % (self.happyIcon)
		elif (iEffect > 1):
			szEffects += u"%d%s " % (iEffect, self.happyIcon)
		elif (iEffect < 0):
			szEffects += u"%d%s " % (-iEffect, self.unhappyIcon)

		iEffect = city.getBonusHealth(arg)
		if (iEffect == 1):
			szEffects += u"%s " % (self.healthIcon)
		elif (iEffect > 1):
			szEffects += u"%d%s " % (iEffect, self.healthIcon)
		elif (iEffect < 0):
			szEffects += u"%d%s " % (-iEffect, self.sickIcon)

		for eYieldType in range(YieldTypes.NUM_YIELD_TYPES):
			iEffect = city.getBonusYieldRateModifier(eYieldType, arg)
			if (iEffect > 0):
				szEffects += u"%s " % self.yieldIcons[eYieldType]
#			elif (iEffect > 1 or iEffect < 0):
#				szEffects += u"%d%s " % (iEffect, self.yieldIcons[eYieldType])

		iNumBonuses = city.getNumBonuses(arg)
		if (self.bonusCorpYields.has_key(arg)):
			yields = self.bonusCorpYields[arg]
			for eYield in range(YieldTypes.NUM_YIELD_TYPES):
				if (yields.has_key(eYield)):
					iEffect = 0
					for eCorp, iValue in yields[eYield].iteritems():
						if (city.isActiveCorporation(eCorp)):
							iEffect += iValue * iNumBonuses * self.corpMaintPercent / 100
					iEffect = (iEffect + 99) / 100
					if (iEffect == 1):
						szEffects += u"%s " % self.yieldIcons[eYield]
					elif (iEffect > 1 or iEffect < 0):
						szEffects += u"%d%s " % (iEffect, self.yieldIcons[eYield])

		if (self.bonusCorpCommerces.has_key(arg)):
			commerces = self.bonusCorpCommerces[arg]
			for eCommerce in range(CommerceTypes.NUM_COMMERCE_TYPES):
				if (commerces.has_key(eCommerce)):
					iEffect = 0
					for eCorp, iValue in commerces[eCommerce].iteritems():
						if (city.isActiveCorporation(eCorp)):
							iEffect += iValue * iNumBonuses * self.corpMaintPercent / 100
					iEffect = (iEffect + 99) / 100
					if (iEffect == 1):
						szEffects += u"%s " % self.commerceIcons[eCommerce]
					elif (iEffect > 1 or iEffect < 0):
						szEffects += u"%d%s " % (iEffect, self.commerceIcons[eCommerce])

#		for eYieldType in range(YieldTypes.NUM_YIELD_TYPES):
#			if (city.getCorporationYield(eYieldType) > 0):
#				iEffect = 0
#				for eCorporation in range(gc.getNumCorporationInfos()):
#					if (city.isActiveCorporation(eCorporation)):
#						info = gc.getCorporationInfo(eCorporation)
#						for i in range(gc.getNUM_CORPORATION_PREREQ_BONUSES()):
#							if (info.getPrereqBonus(i) == arg):
#								iEffect += info.getYieldProduced(eYieldType) * city.getNumBonuses(arg) * self.corpMaintPercent / 100
#				iEffect = (iEffect + 99) / 100
#				if (iEffect == 1):
#					szEffects += u"%s " % self.yieldIcons[eYieldType]
#				elif (iEffect > 1 or iEffect < 0):
#					szEffects += u"%d%s " % (iEffect, self.yieldIcons[eYieldType])
#
#		for eCommerceType in range(CommerceTypes.NUM_COMMERCE_TYPES):
#			if (city.getCorporationCommerce(eCommerceType) > 0):
#				iEffect = 0
#				for eCorporation in range(gc.getNumCorporationInfos()):
#					if (city.isActiveCorporation(eCorporation)):
#						info = gc.getCorporationInfo(eCorporation)
#						for i in range(gc.getNUM_CORPORATION_PREREQ_BONUSES()):
#							if (info.getPrereqBonus(i) == arg):
#								iEffect += info.getCommerceProduced(eCommerceType) * city.getNumBonuses(arg) * self.corpMaintPercent / 100
#				iEffect = (iEffect + 99) / 100
#				if (iEffect == 1):
#					szEffects += u"%s " % self.commerceIcons[eCommerceType]
#				elif (iEffect > 1 or iEffect < 0):
#					szEffects += u"%d%s " % (iEffect, self.commerceIcons[eCommerceType])

		iEffect = city.getBonusPower(arg)
		if (iEffect == 1):
			szEffects += u"%s " % (self.powerIcon)
		elif (iEffect > 1):
			szEffects += u"%d%s " % (iEffect, self.powerIcon)

		if (szEffects == u""):
			return self.objectHave
		return szEffects.strip()

	def findGlobalBaseYieldRateRank (self, city, szKey, arg):

		aList = []
		for i in range(gc.getMAX_PLAYERS()):
			for cityX in gc.getPlayer(i).cities():
				L.append(cityX.getPlotYield(arg))

		y = city.getPlotYield(arg)
		return len([i for i in aList if i > y]) + 1

	def findGlobalYieldRateRank (self, city, szKey, arg):

		L = []
		for i in range(gc.getMAX_PLAYERS()):
			for cityX in gc.getPlayer(i).cities():
				L.append(cityX.getYieldRate(arg))

		y = city.getYieldRate(arg)
		return len([i for i in L if i > y]) + 1

	def findGlobalCommerceRateRank (self, city, szKey, arg):

		L = []
		for i in range(gc.getMAX_PLAYERS()):
			for cityX in gc.getPlayer(i).cities():
				L.append(cityX.getCommerceRate(arg))

		y = city.getCommerceRate(arg)
		return len([i for i in L if i > y]) + 1


	def canAdviseToConstruct(self, city, i):
		if not city.canConstruct(i, True, False, False):
			return False
		info = gc.getBuildingInfo(i)
		if info.isGovernmentCenter() or info.isCapital():
			return False

		return True

	def advise(self, city, szKey, type):

		bestOrder = -1
		bestData = 0.0

		# For all cities, start with growth
		if self.calculateNetHappiness(city) > 2 and self.calculateNetHealth(city) > 2:
			for i in range(gc.getNumBuildingInfos()):
				if self.canAdviseToConstruct(city, i):
					info = gc.getBuildingInfo(i)
					value = info.getFoodKept() / float(info.getProductionCost())
					if(value > bestData):
						bestOrder = i
						bestData = value

		# then balancing health and happiness for further growth
		if bestOrder == -1:
			for i in range(gc.getNumBuildingInfos()):
				if self.canAdviseToConstruct(city, i):
					info = gc.getBuildingInfo(i)
					if self.calculateNetHappiness(city) < 3 and self.calculateNetHappiness(city) - self.calculateNetHealth(city) > 2:
						iHealth = info.getHealth()
						for eBonus, iNumHappiness in info.getBonusHappinessChanges():
							if city.hasBonus(eBonus):
								iHealth += iNumHappiness
						value = iHealth / float(info.getProductionCost())
						if value > bestData:
							bestOrder = i
							bestData = value
					elif self.calculateNetHealth(city) < 3 and self.calculateNetHealth(city) - self.calculateNetHappiness(city) > 2:
						iHappiness = info.getHappiness()
						for eBonus, iNumHappiness in info.getBonusHappinessChanges():
							if city.hasBonus(eBonus):
								iHappiness += iNumHappiness
						value = iHappiness  / float(info.getProductionCost())
						if value > bestData:
							bestOrder = i
							bestData = value

		# First pass
		if bestOrder == -1:
			player = gc.getActivePlayer()
			for i in range(gc.getNumBuildingInfos()):
				if self.canAdviseToConstruct(city, i):
					info = gc.getBuildingInfo(i)

					if type == "Culture":
						if city.findBaseYieldRateRank(YieldTypes.YIELD_COMMERCE) < 6:
							value = info.getCommerceModifier(CommerceTypes.COMMERCE_CULTURE) / float(info.getProductionCost())
							if value > bestData:
								bestOrder = i
								bestData = value
						else:
							value = info.getPowerValue() / float(info.getProductionCost())
							if value > bestData:
								bestOrder = i
								bestData = value
					elif type == "Military":
						value = info.getPowerValue() / float(info.getProductionCost())
						if value > bestData:
							bestOrder = i
							bestData = value
					elif type == "Nutty":
						value = math.sin(float(info.getProductionCost()) * city.getPlotYield(YieldTypes.YIELD_COMMERCE)) + 1
						if value > bestData:
							bestOrder = i
							bestData = value
					elif type == "Religion":
						bestOrder = -1
					elif type == "Research":
						value = info.getCommerceModifier(CommerceTypes.COMMERCE_RESEARCH) / float(info.getProductionCost())
						if value > bestData:
							bestOrder = i
							bestData = value
					elif type == "Spaceship":
						if not city.isPower():
							if info.isPower():
								value = city.getPlotYield(YieldTypes.YIELD_PRODUCTION) / float(info.getProductionCost())
								if value > bestData:
									bestOrder = i
									bestData = value

						if city.findBaseYieldRateRank(YieldTypes.YIELD_PRODUCTION) < 12:
							value = city.getPlotYield(YieldTypes.YIELD_PRODUCTION) * 2 * info.getYieldModifier(YieldTypes.YIELD_PRODUCTION) / float(info.getProductionCost())
							if value > bestData:
								bestOrder = i
								bestData = value

						if city.findBaseYieldRateRank(YieldTypes.YIELD_COMMERCE) < player.getNumCities() / 2:
							value = city.getPlotYield(YieldTypes.YIELD_COMMERCE) * info.getCommerceModifier(CommerceTypes.COMMERCE_RESEARCH) / float(info.getProductionCost())
							if value > bestData:
								bestOrder = i
								bestData = value
						else:
							bestOrder = -1

		# Second pass
		if(bestOrder == -1):
			for i in range(gc.getNumBuildingInfos()):
				if self.canAdviseToConstruct(city, i):
					info = gc.getBuildingInfo(i)

					if type == "Culture":
						if city.findBaseYieldRateRank(YieldTypes.YIELD_COMMERCE) < 6:
							value = info.getPowerValue() / float(info.getProductionCost())
							if value > bestData:
								bestOrder = i
								bestData = value
						else:
							bestOrder = -1  # In a cultural game, build units in the culturally weak cities
					elif type == "Military":
						if city.findBaseYieldRateRank(YieldTypes.YIELD_PRODUCTION) <= 3:
							value = (-1.0 * info.getMilitaryProductionModifier()) / info.getProductionCost()
							print "%s: %.2f" %(info.getDescription(), value)
							if value > bestData:
								bestOrder = i
								bestData = value
						else:
							value = info.getYieldModifier(YieldTypes.YIELD_PRODUCTION) / float(info.getProductionCost())
							if value > bestData:
								bestOrder = i
								bestData = value
							if not city.isPower():
								if info.isPower():
									value = 1 / float(info.getProductionCost())
									if value > bestData:
										bestOrder = bldg
										bestData = value

					elif type == "Nutty":
						bestOrder = -1
					elif type == "Religion":
						bestOrder = -1
					elif type == "Research":
						value = info.getCommerceModifier(CommerceTypes.COMMERCE_GOLD) / float(info.getProductionCost())
						if value > bestData:
							bestOrder = i
							bestData = value
					elif type == "Spaceship":
						bestOrder = -1

		szReturn = u""
		if bestOrder != -1:
			szReturn = gc.getBuildingInfo(bestOrder).getDescription()

		if szReturn == city.getProductionName():
			szReturn = u""

		return szReturn

	def ColorCityValues (self, nValue, szKey):
		"""Colors city values which we might want to alert the user to."""

		# If the key is one we want to possibly color
		if (szKey in self.COLOR_SET):
			# If we don't have a plain integer
			if (re.search ("[-+]", nValue)):
				# Color it red and return it
				return localText.changeTextColor (nValue, gc.getInfoTypeForString("COLOR_RED"))
			# For each type of comparison
			for szCompareType, clDict in self.COLOR_DICT_DICT.iteritems():
				# Get the color we will use.
				color = self.COLOR_DICT[szCompareType]

				# If the dictionary has the key and the comparison is appropriate
				if (clDict != None and clDict.has_key(szKey) and (szCompareType == "PROBLEM" and int(nValue) <= clDict[szKey] or szCompareType == "NEUTRAL" and int(nValue) == clDict[szKey] or szCompareType == "GREAT" and int(nValue) >= clDict[szKey])):
					# Color and return it
					return localText.changeTextColor (nValue, color)

		# Otherwise, just return the regular value
		return nValue

# RevolutionDCM - data manipulation functions - start

	def getDeltaTrendVal(self, city):
		return city.getRevolutionIndex() - city.getRevIndexAverage()

	def getDeltaTrendText(self, city):
		showTrend = RevDefs.showTrend
		trend = self.getDeltaTrendVal(city)
		trendText = "-"
		if trend > 10*showTrend:
			trendText = localText.getText("TXT_ADVISOR_RAPIDLY_WORSENING", ())
			trendText = localText.changeTextColor (trendText, gc.getCOLOR_RED())
		elif trend > showTrend:
			trendText = localText.getText("TXT_ADVISOR_WORSENING", ())
			trendText = localText.changeTextColor (trendText, gc.getInfoTypeForString("COLOR_PLAYER_ORANGE"))
		elif trend < -showTrend:
			trendText = localText.getText("TXT_ADVISOR_IMPROVING", ())
			trendText = localText.changeTextColor (trendText, gc.getCOLOR_GREEN())
		else:
			trendText = localText.getText("TXT_ADVISOR_FLAT", ())
			trendText = localText.changeTextColor (trendText, gc.getCOLOR_WHITE())
		return trendText

	def getRevolutionStatusText(self, city):
		critical = RevInstances.RevolutionInst.alwaysViolentThreshold
		danger = RevInstances.RevolutionInst.revInstigatorThreshold
		warning = RevInstances.RevolutionInst.revReadyFrac * RevInstances.RevolutionInst.revInstigatorThreshold
		return self.parseText(city.getRevolutionIndex(), warning, danger, critical)

	def getRevHappinessVal(self, city):
		revIdxHist = RevData.getCityVal(city,'RevIdxHistory')
		return revIdxHist['Happiness'][0]

	def getRevHappinessText(self, city):
		value = self.getRevHappinessVal(city)
		critical = 20
		danger = 10
		warning = 3
		return self.parseText(value, warning, danger, critical)

	def getRevDistanceVal(self, city):
		revIdxHist = RevData.getCityVal(city,'RevIdxHistory')
		return revIdxHist['Location'][0]

	def getRevDistanceText(self, city):
		value = self.getRevDistanceVal(city)
		critical = 20
		danger = 10
		warning = 3
		return self.parseText(value, warning, danger, critical)

	def getRevColonyVal(self, city):
		revIdxHist = RevData.getCityVal(city,'RevIdxHistory')
		return revIdxHist['Colony'][0]

	def getRevColonyText(self, city):
		value = self.getRevColonyVal(city)
		critical = 20
		danger = 10
		warning = 3
		text = self.parseText(value, warning, danger, critical)
		irrelevant = localText.changeTextColor(localText.getText("TXT_ADVISOR_POSITIVE",()), gc.getCOLOR_GREEN())
		if text == irrelevant : text = "-"
		return text

	def getRevReligionVal(self, city):
		revIdxHist = RevData.getCityVal(city,'RevIdxHistory')
		return revIdxHist['Religion'][0]

	def getRevReligionText(self, city):
		value = self.getRevReligionVal(city)
		critical = 20
		danger = 10
		warning = 3
		return self.parseText(value, warning, danger, critical)

	def getRevNationalityVal(self, city):
		revIdxHist = RevData.getCityVal(city,'RevIdxHistory')
		return revIdxHist['Nationality'][0]

	def getRevNationalityText(self, city):
		value = self.getRevNationalityVal(city)
		critical = 20
		danger = 10
		warning = 3
		return self.parseText(value, warning, danger, critical)

	def getRevHealthVal(self, city):
		revIdxHist = RevData.getCityVal(city,'RevIdxHistory')
		return revIdxHist['Health'][0]

	def getRevHealthText(self, city):
		value = self.getRevHealthVal(city)
		critical = 20
		danger = 10
		warning = 3
		return self.parseText(value, warning, danger, critical)

	def getRevGarrisonVal(self, city):
		revIdxHist = RevData.getCityVal(city,'RevIdxHistory')
		return revIdxHist['Garrison'][0]

	def getRevGarrisonText(self, city):
		value = self.getRevGarrisonVal(city)
		critical = 20
		danger = 10
		warning = 3
		return self.parseText(value, warning, danger, critical)

	def getRevDisorderVal(self, city):
		revIdxHist = RevData.getCityVal(city,'RevIdxHistory')
		return revIdxHist['Disorder'][0]

	def getRevDisorderText(self, city):
		value = self.getRevDisorderVal(city)
		critical = 20
		danger = 10
		warning = 3
		text = self.parseText(value, warning, danger, critical)
		irrelevant = localText.changeTextColor(localText.getText("TXT_ADVISOR_POSITIVE",()), gc.getCOLOR_GREEN())
		if text == irrelevant : text = localText.getText("TXT_KEY_REV_NONE", ())
		return text

	def parseText(self, value, thresholdWarning, thresholdDanger, thresholdCritical):
		outText = "-"
		if value >= thresholdCritical:
			outText = localText.getText("TXT_ADVISOR_CRITICAL", ())
			outText = localText.changeTextColor (outText, gc.getCOLOR_RED())

		elif value >= thresholdDanger:
			outText = localText.getText("TXT_KEY_REV_WATCH_DANGER", ())
			outText = localText.changeTextColor (outText, gc.getInfoTypeForString("COLOR_PLAYER_ORANGE"))

		elif value >= thresholdWarning:
			outText = localText.getText("TXT_KEY_REV_WATCH_WARNING", ())
			outText = localText.changeTextColor (outText, gc.getCOLOR_YELLOW())
		else:
			outText = localText.getText("TXT_ADVISOR_POSITIVE", ())
			outText = localText.changeTextColor (outText, gc.getCOLOR_GREEN())
		return outText
# RevolutionDCM - end

	def drawContents(self, page):
		""" Function to draw the contents of the cityList passed in. """

		screen = self.getScreen()

		# Hide building icons
		for i in range(gc.getNumBuildingInfos()):
			szName = "BLDG_BTN_%d" % i
			screen.hide(szName)

		# Fill the pages drop down
		screen.addDropDownBoxGFC(self.PAGES_DD_NAME, self.X_SPECIAL, self.Y_SPECIAL, self.PAGES_DD_W, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for i, p in enumerate(self.PAGES):
			screen.addPullDownString(self.PAGES_DD_NAME, p["name"], i, i, i == self.currentPageNum )

		if self.customizing:

			# Build the page definition table
			screen.addTableControlGFC (self.CUSTOMIZE_PAGE, 4, self.nTableX, self.nTableY, self.nHalfTableWidth, self.nShortTableLength, True, False, 32, 32, TableStyles.TABLE_STYLE_STANDARD )
			screen.enableSelect(self.CUSTOMIZE_PAGE, True)
			screen.enableSort (self.CUSTOMIZE_PAGE)
			screen.setStyle(self.CUSTOMIZE_PAGE, "Table_StandardCiv_Style")

			screen.setTableColumnHeader (self.CUSTOMIZE_PAGE, 0, u"<font=2>POS</font>", 40 )
			screen.setTableColumnHeader (self.CUSTOMIZE_PAGE, 1, u"<font=2>NAME</font>", 160 )
			screen.setTableColumnHeader (self.CUSTOMIZE_PAGE, 2, u"<font=2>TITLE</font>", 160 )
			screen.setTableColumnHeader (self.CUSTOMIZE_PAGE, 3, u"<font=2>WIDTH</font>", 80 )

			columns = self.PAGES[self.currentPageNum]["columns"]
			for i, column in enumerate(columns):
				screen.appendTableRow (self.CUSTOMIZE_PAGE)
				screen.setTableInt(self.CUSTOMIZE_PAGE, 0, i, unicode(i+1), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<1)
				screen.setTableText(self.CUSTOMIZE_PAGE, 1, i, unicode(column[0]), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

				# Catch exceptions generated by missing columns
				try:
					screen.setTableText(self.CUSTOMIZE_PAGE, 2, i, self.HEADER_DICT[column[0]], "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
				except KeyError:
					screen.setTableText(self.CUSTOMIZE_PAGE, 2, i, "", "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
				except TypeError:
					screen.setTableText(self.CUSTOMIZE_PAGE, 2, i, "", "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

				screen.setTableInt(self.CUSTOMIZE_PAGE, 3, i, unicode(column[1]), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<1)

			# Build the available columns table
			if(screen.getTableNumRows(self.COLUMNS_LIST_PAGE) != len(self.COLUMNS_LIST)):

				screen.addTableControlGFC (self.COLUMNS_LIST_PAGE, 4, self.nSecondHalfTableX, self.nTableY, self.nHalfTableWidth, self.nShortTableLength, True, False, 32, 32, TableStyles.TABLE_STYLE_STANDARD )
				screen.enableSelect(self.COLUMNS_LIST_PAGE, True)
				screen.enableSort (self.COLUMNS_LIST_PAGE)
				screen.setStyle(self.COLUMNS_LIST_PAGE, "Table_StandardCiv_Style")

				screen.setTableColumnHeader (self.COLUMNS_LIST_PAGE, 0, u"<font=2>ID</font>", 40 )
				screen.setTableColumnHeader (self.COLUMNS_LIST_PAGE, 1, u"<font=2>NAME</font>", 160 )
				screen.setTableColumnHeader (self.COLUMNS_LIST_PAGE, 2, u"<font=2>TITLE</font>", 160 )
				screen.setTableColumnHeader (self.COLUMNS_LIST_PAGE, 3, u"<font=2>WIDTH</font>", 80 )

				columns = self.COLUMNS_LIST
				for i, column in enumerate(columns):
					screen.appendTableRow (self.COLUMNS_LIST_PAGE)
					screen.setTableInt(self.COLUMNS_LIST_PAGE, 0, i, unicode(i), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<1)
					screen.setTableText(self.COLUMNS_LIST_PAGE, 1, i, unicode(column[0]), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
					try:
						screen.setTableText(self.COLUMNS_LIST_PAGE, 2, i, unicode(self.HEADER_DICT[column[0]]), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
					except TypeError:
						screen.setTableText(self.COLUMNS_LIST_PAGE, 2, i, "", "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
					except UnicodeDecodeError:
						screen.setTableText(self.COLUMNS_LIST_PAGE, 2, i, "", "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

					screen.setTableInt(self.COLUMNS_LIST_PAGE, 3, i, unicode(column[1]), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<1)

			# This is legacy code. I don't know if it's necessary.
			screen.moveToBack (self.BACKGROUND_ID)

			# Finally, display the customization controls
			self.showCustomizationControls()

			# And hide the regular controls and legends
			screen.hide(self.currentPage)
			self.hideSpecialists()
			self.hideRevolutionLegend()
			self.hideGPLegend()

		# If displaying the normal advisor screen (not the customization interface)
		else:
			player = gc.getActivePlayer()
			lCity = player.cities()

			dDict = self.columnDict

			# Bring down the curtain...
			screen.moveToFront( "Background" )

			# RevolutionDCM - revolution legend control 4
			# Build the table
			if (not self.PAGES[self.currentPageNum]["showSpecControls"]
			and not self.PAGES[self.currentPageNum]["showRevolutionLegend"]
			and not self.PAGES[self.currentPageNum]["showGPLegend"]
			):
				screen.addTableControlGFC(page, len (dDict) + 1, self.nTableX, self.nTableY, self.nTableWidth, self.nTableLength, True, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
			else: screen.addTableControlGFC(page, len (dDict) + 1, self.nTableX, self.nTableY, self.nTableWidth, self.nShortTableLength, True, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)

			screen.enableSelect (page, True)
			screen.enableSort (page)
			screen.setStyle(page, "Table_StandardCiv_Style")

			zoomArt = ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION").getPath()

			# Add a blank header for the "zoom" column
			screen.setTableColumnHeader (page, 0, "", 30 )

			# Add blank rows to the table
			for i, city in enumerate(lCity):
				screen.appendTableRow(page)
				if city.getName() in self.listSelectedCities:
					screen.selectRow(page, i, True)
				screen.setTableText(page, 0, i, "", zoomArt, WidgetTypes.WIDGET_ZOOM_CITY, city.getOwner(), city.getID(), 1<<0)

			# Order the columns
			columns = []
			for key, value in self.columnDict.iteritems():
				columns.append((value, key))
			columns.sort()

			iBuildingButtonX = self.nTableX + 30
			iBuildingButtonY = self.nTableY

			# Loop through the columns first. This is unintuitive, but faster.
			for value, key in columns:
				try:
					columnDef = self.COLUMNS_LIST[self.COLUMNS_INDEX[key]]
					type = columnDef[2]
					if type == "bldg":
						building = columnDef[7]
						buildingInfo = self.BUILDING_INFO_LIST[building]
						screen.setTableColumnHeader (page, value + 1, "", self.columnWidth[key])
						szName = "BLDG_BTN_%d" % building
						x = iBuildingButtonX + (self.columnWidth[key] - self.BUILDING_BUTTON_X_SIZE) / 2
						screen.setImageButton(
							szName, buildingInfo.getButton(), x, iBuildingButtonY,
							self.BUILDING_BUTTON_X_SIZE, self.BUILDING_BUTTON_Y_SIZE,
							WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, building, -1
						)
					else: screen.setTableColumnHeader(page, value + 1, "<font=2>" + self.HEADER_DICT[key] + "</font>", self.columnWidth[key])

					iBuildingButtonX += self.columnWidth[key]

					# And the correct writing function.
					if (type == "text"):
						funcTableWrite = screen.setTableText
						justify = 1<<0
					elif (type == "date"):
						funcTableWrite = screen.setTableDate
						justify = 1<<0
					elif (type == "int"):
						funcTableWrite = screen.setTableInt
						justify = 1<<1
					elif (type == "bonus"):
						funcTableWrite = screen.setTableText
						justify = 1<<2
					elif (type == "bldg"):
						funcTableWrite = screen.setTableText
						justify = 1<<2
					else:
						return

					colorFunc = self.ColorCityValues

					if columnDef[3]:
						calcFunc = columnDef[3]

						for i, city in enumerate(lCity):
							szValue = colorFunc(unicode(calcFunc(city)), key)
							funcTableWrite (page, value + 1, i, szValue, "", WidgetTypes.WIDGET_GENERAL, -1, -1, justify)

					elif columnDef[4]:
						calcFunc = columnDef[4]

						for i, city in enumerate(lCity):
							szValue = colorFunc(unicode(calcFunc(city, columnDef[5])), key)
							funcTableWrite (page, value + 1, i, szValue, "", WidgetTypes.WIDGET_GENERAL, -1, -1, justify)

					else:
						calcFunc = columnDef[6]

						for i, city in enumerate(lCity):
							szValue = colorFunc(unicode(calcFunc(city, key, columnDef[7])), key)
							funcTableWrite (page, value + 1, i, szValue, "", WidgetTypes.WIDGET_GENERAL, -1, -1, justify)

				except KeyError:
					continue
				except TypeError:
					continue

			# Finally, display the specialist controls,
			if self.PAGES[self.currentPageNum]["showSpecControls"]:
				self.showSpecialists()
			else:
				self.hideSpecialists()

			# RevolutionDCM - legend control 5
			# and the legends,
			if self.PAGES[self.currentPageNum]["showRevolutionLegend"]:
				self.showRevolutionLegend()
			else:
				self.hideRevolutionLegend()

			if self.PAGES[self.currentPageNum]["showGPLegend"]:
				self.showGPLegend()
			else:
				self.hideGPLegend()

			# and hide the customization tools.
			self.hideCustomizationControls()

			# Raise the curtain and reveal our masterpiece
			screen.moveToBack (self.BACKGROUND_ID)

			# Now hand off to the C++ API
			self.updateAppropriateCitySelection(page, player.getNumCities())

	def HandleSpecialistPlus(self, inputClass):
		""" Handles when any Specialist Plus is pushed."""

		#CyInterface().setDirty(InterfaceDirtyBits.Domestic_Advisor_DIRTY_BIT, True)
		return 0

	def HandleSpecialistMinus(self, inputClass):
		""" Handles when any Specialist Minus is pushed."""

		CyInterface().setDirty(InterfaceDirtyBits.REVOLUTION_WATCH_ADVISOR_DIRTY_BIT, True)
		return 0

	def RevolutionWatchExit(self, inputClass):
		return 0

	def handleInput(self, inputClass):
		""" Handles the input for this screen..."""

		code = inputClass.getNotifyCode()

		if ( code == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED ):

			szWidget = inputClass.getFunctionName() + str(inputClass.getID())

			# I have no clue why + "0" is required here and not in the elif case. It's a mystery.
			if(szWidget == self.PAGES_DD_NAME + "0"):
				if(self.customizing):
					self.customizingClearSelection()
				self.switchPage(self.PAGES[inputClass.getData()]["name"])
				self.drawScreen(self.currentPage)
				return 1

			elif(szWidget == self.currentPage):
				screen = self.getScreen()
				if (inputClass.getMouseX() == 0):
					screen.hideScreen()

					CyInterface().selectCity(gc.getPlayer(inputClass.getData1()).getCity(inputClass.getData2()), True)

					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popupInfo.setText(u"showRevolutionWatchAdvisor")
					popupInfo.addPopup(inputClass.getData1())
				else:
					city = self.getCurrentCity()
					if (city):
						CyInterface().lookAtCityOffset(city.getID())

						#RevolutionDCM - legend control 3
						page = self.PAGES[self.currentPageNum]
						if page["showRevolutionLegend"]:
							self.showRevolutionLegend()

					if self.PAGES[self.currentPageNum]["showSpecControls"]:
						self.showSpecialists()

					# And pass it back to the screen
					self.updateAppropriateCitySelection(self.currentPage, gc.getActivePlayer().getNumCities())

					return 1

			else:
				print szWidget, self.currentPage

		# Is the input from a mapped button?
		elif (code == NotifyCode.NOTIFY_CLICKED and self.RevolutionWatchScreenInputMap.has_key(inputClass.getFunctionName())):
			'Calls function mapped in CvSpecRevolutionAdvisor'
			# only get from the map if it has the key

			# get bound function from map and call it (return whatever it returns.)
			return self.RevolutionWatchScreenInputMap.get(inputClass.getFunctionName())(inputClass)

		# Is the input from a Zoom City button?
		elif ( inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED ):
			if ( inputClass.getFunctionName() == "ZoomCity" ):
				screen = CyGInterfaceScreen( "RevolutionWatchAdvisor", CvScreenEnums.REVOLUTION_WATCH_ADVISOR )
				screen.hideScreen()

				CyInterface().selectCity(gc.getPlayer(inputClass.getData1()).getCity(inputClass.getData2()), True)

				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
				popupInfo.setText(u"showRevolutionWatchAdvisor")
				popupInfo.addPopup(inputClass.getData1())

			#RevolutionDCM - bribe button handler
			if ( inputClass.getFunctionName() == self.REVOLUTION_LEGEND_ICON or inputClass.getFunctionName() == self.REVOLUTION_BRIBE_TEXT ):
				city = self.getCurrentCity()
				if city:
					RevInstances.RevolutionInst.showBribeCityPopup(city)
					#if(self.visiblePage):
					#	screen = self.getScreen()
					#	screen.hide(self.visiblePage)
					#	screen.show(self.currentPage)
					#	self.visiblePage = self.currentPage
					#	self.drawContents(self.currentPage)


# BUG - Colony Split - start

			elif (inputClass.getFunctionName() == self.SPLIT_NAME):
				screen = CyGInterfaceScreen( "RevolutionWatchAdvisor", CvScreenEnums.REVOLUTION_WATCH_ADVISOR )
				screen.hideScreen()

# BUG - Colony Split - end

		# If none of the above, we didn't use the input."
		return 0

	def isFoodProduction (self, szProducing):
		""" We determine it's a food production, if it's a worker or settler."""
		return re.search ("Worker|Settler", szProducing)

	def updateScreen(self):
		""" Updates the screen."""

		self.drawContents(self.currentPage)

		return

	def update(self, fDelta):
		""" Update the Advisor."""
		if (CyInterface().isDirty(InterfaceDirtyBits.Domestic_Advisor_DIRTY_BIT) == True):
			CyInterface().setDirty(InterfaceDirtyBits.Domestic_Advisor_DIRTY_BIT, False)

			self.drawContents(self.currentPage)

		return

	def switchPage(self, page):

		for i, p in enumerate(self.PAGES):
			if(p["name"] == page):
				self.currentPage = self.getPageID(p["name"])
				self.currentPageNum = i
				self.columnDict = { }
				self.columnWidth = { }
				for i, col in enumerate(p["columns"]):
					self.columnDict[col[0]] = i
					self.columnWidth[col[0]] = col[1]
				break

	def isPageActive(self, page):
		return self.currentPage == self.getPageID(page)

	def getPageID(self, page):
		for i, p in enumerate(self.PAGES):
			if(p["name"] == page):
				id = "CDA_PAGE_%d" % i
				return id

	def updateAppropriateCitySelection(self, page, nCities):
		screen = self.getScreen()
		screen.updateAppropriateCitySelection( page, nCities, 1 )

		self.listSelectedCities = []
		for i in range(nCities):
			if screen.isRowSelected(page, i):
				self.listSelectedCities.append(screen.getTableText(page, 1, i))

	def save(self, inputClass):
		name = SP.joinModDir("UserSettings", "CustomRevAdv", "CustomRevAdv.txt")
		if SP.isFile(name):
			file = open(name, 'w')

			if(file != 0):
				pickle.dump({ "version" : self.PICKLED_VERSION, "pages" : self.PAGES }, file)
				file.close()

		return 1

	def ModifyPage(self, inputClass):

		screen = self.getScreen()

		if(self.customizing):
			self.customizing = False

			self.switchPage(self.PAGES[self.currentPageNum]["name"]) # Force a dictionary rebuild.

			self.drawScreen(self.currentPage)

		else:
			self.customizing = True

			self.drawScreen(self.currentPage)

		return 1

	def getSortedColumnSelection(self, screen):

		list = []

		for i in range(len(self.PAGES[self.currentPageNum]["columns"])):
			if screen.isRowSelected(self.CUSTOMIZE_PAGE, i):
				list.append(int(screen.getTableText(self.CUSTOMIZE_PAGE, 0, i)) - 1)

		list.sort()

		return list

	def getReverseSortedColumnSelection(self, screen):

		list = self.getSortedColumnSelection(screen)
		list.reverse()
		return list

	def AddCol(self, inputClass):

		self.customizingSaveSelection()
		screen = self.getScreen()

		for i in range(len(self.COLUMNS_LIST)):
			if screen.isRowSelected(self.COLUMNS_LIST_PAGE, i):
				col = self.COLUMNS_LIST[int(screen.getTableText(self.COLUMNS_LIST_PAGE, 0, i))]
				self.PAGES[self.currentPageNum]["columns"].append((col[0], col[1], col[2]))

		self.drawScreen(self.currentPage)
		self.customizingRestoreSelection()
		return 1

	def DelCol(self, inputClass):

		self.customizingSaveSelection()
		screen = self.getScreen()
		list = self.getReverseSortedColumnSelection(screen)

		for i in list:
			if(i == 0):
				continue
			else:
				del self.PAGES[self.currentPageNum]["columns"][i]

		self.drawScreen(self.currentPage)
		self.customizingRestoreSelection()
		return 1

	def MoveColUp(self, inputClass):

		self.customizingSaveSelection()

		screen = self.getScreen()

		list = self.getSortedColumnSelection(screen)

		top = 1

		for i in list:
			if(i == 0):
				continue
			elif i == top:
				top += 1
			else:
				col = self.PAGES[self.currentPageNum]["columns"][i]
				del self.PAGES[self.currentPageNum]["columns"][i]
				self.PAGES[self.currentPageNum]["columns"].insert(i - 1, col)

		self.drawScreen(self.currentPage)
		self.customizingRestoreSelection()

		return 1

	def MoveColDn(self, inputClass):

		self.customizingSaveSelection()

		screen = self.getScreen()

		list = self.getReverseSortedColumnSelection(screen)

		bottom = len(self.PAGES[self.currentPageNum]["columns"]) - 1

		for i in list:
			if(i == 0):
				continue
			elif i == bottom:
				bottom -= 1
			else:
				col = self.PAGES[self.currentPageNum]["columns"][i]
				del self.PAGES[self.currentPageNum]["columns"][i]
				self.PAGES[self.currentPageNum]["columns"].insert(i + 1, col)

		self.drawScreen(self.currentPage)
		self.customizingRestoreSelection()

		return 1

	def customizingClearSelection(self):
		self.customizingSelection = []

	def customizingSaveSelection(self):
		screen = self.getScreen()
		self.customizingSelection = []
		for i in range(len(self.PAGES[self.currentPageNum]["columns"])):
			if screen.isRowSelected(self.CUSTOMIZE_PAGE, i):
				self.customizingSelection.append(screen.getTableText(self.CUSTOMIZE_PAGE, 1, i))

	def customizingRestoreSelection(self):
		screen = self.getScreen()
		# Unselect before selecting, so that the selected rows are forced onscreen.
		for i in range(len(self.PAGES[self.currentPageNum]["columns"])):
			if not screen.getTableText(self.CUSTOMIZE_PAGE, 1, i) in self.customizingSelection:
				screen.selectRow(self.CUSTOMIZE_PAGE, i, False)
		for i in range(len(self.PAGES[self.currentPageNum]["columns"])):
			if screen.getTableText(self.CUSTOMIZE_PAGE, 1, i) in self.customizingSelection:
				screen.selectRow(self.CUSTOMIZE_PAGE, i, True)

	def shrinkCol(self, inputClass):

		self.customizingSaveSelection()
		screen = self.getScreen()
		columns = self.PAGES[self.currentPageNum]["columns"]

		list = self.getReverseSortedColumnSelection(screen)
		for i in list:
			col = columns[i]
			if(col[1] < 2):
				continue
			shrunken = (col[0], col[1] - 1, col[2])
			del columns[i]
			columns.insert(i, shrunken)

		self.drawScreen(self.currentPage)
		self.customizingRestoreSelection()

		return 1

	def widenCol(self, inputClass):

		self.customizingSaveSelection()
		screen = self.getScreen()
		columns = self.PAGES[self.currentPageNum]["columns"]

		list = self.getReverseSortedColumnSelection(screen)
		for i in list:
			col = columns[i]
			wider  = (col[0], col[1] + 1, col[2])
			del columns[i]
			columns.insert(i, wider)

		self.drawScreen(self.currentPage)
		self.customizingRestoreSelection()

		return 1

	def toggleShowSpecialistControls(self, inputClass):
		"""
		Toggle the page's 'show specialists' field.
		Also toggles the 'show culture/GP legend' fields (one button).
		"""
		# RevolutionDCM - legend control 6
		# note: this code toggles the state of the various pages when the
		# "Testa" button is pressed - see Bug Help Manual
		page = self.PAGES[self.currentPageNum]
		page["showSpecControls"] = not page["showSpecControls"]
		page["showRevolutionLegend"] = not page["showRevolutionLegend"]
		page["showGPLegend"] = not page["showGPLegend"]
#		screen.setState(self.TOGGLE_SPECS_NAME, page["showSpecControls"])
		return 1

	def toggleShowCultureLegend(self, inputClass):
		"""
		Toggle the page's 'show culture legend' field.
		"""
		page = self.PAGES[self.currentPageNum]
		# RevolutionDCM - legend control 7
		page["showRevolutionLegend"] = not page["showRevolutionLegend"]

		return 1

	def toggleShowGPLegend(self, inputClass):
		"""
		Toggle the page's 'show GP legend' field.
		"""
		page = self.PAGES[self.currentPageNum]
		page["showGPLegend"] = not page["showGPLegend"]

		return 1

	# RevolutionDCM - legend control 8
	def addPage(self, inputClass):
		screen = self.getScreen()
		self.customizingClearSelection()
		name = "New Page %d" % (len(self.PAGES) + 1)
		self.PAGES.append(
			{
				"name" : name,
				"showSpecControls" : False,
				"showRevolutionLegend" : False,
				"showGPLegend" : False,
				"columns" : [
					("NAME", 95, "text")
				]
			}
			)
		self.switchPage(name)
		self.drawScreen(self.currentPage)

		return 1

	def delPage(self, inputClass):

		if(len(self.PAGES) > 1):
			screen = self.getScreen()
			self.customizingClearSelection()
			del self.PAGES[self.currentPageNum]
			self.switchPage(self.PAGES[0]["name"])
			self.drawScreen(self.currentPage)

		return 1

	def upPage(self, inputClass):

		if (self.currentPageNum < 1):
			# Cannot move first page up
			return 1
		curPage = self.currentPageNum
		prevPage = curPage - 1
		temp = self.PAGES[curPage]
		self.PAGES[curPage] = self.PAGES[prevPage]
		self.PAGES[prevPage] = temp
		if(self.customizing):
			self.customizingClearSelection()
		self.switchPage(temp["name"])
		self.drawScreen(self.currentPage)

		return 1

	def downPage(self, inputClass):

		if (self.currentPageNum + 1 >= len(self.PAGES)):
			# Cannot move last page down
			return 1
		curPage = self.currentPageNum
		nextPage = curPage + 1
		temp = self.PAGES[curPage]
		self.PAGES[curPage] = self.PAGES[nextPage]
		self.PAGES[nextPage] = temp
		if(self.customizing):
			self.customizingClearSelection()
		self.switchPage(temp["name"])
		self.drawScreen(self.currentPage)

		return 1

	def previousPage(self, inputClass):

		if (self.currentPageNum < 1):
			# Already on first page
			return 1
		if(self.customizing):
			self.customizingClearSelection()
		self.switchPage(self.PAGES[self.currentPageNum - 1]["name"])
		self.drawScreen(self.currentPage)

		return 1

	def nextPage(self, inputClass):

		if (self.currentPageNum + 1 >= len(self.PAGES)):
			# Already on last page
			return 1
		if(self.customizing):
			self.customizingClearSelection()
		self.switchPage(self.PAGES[self.currentPageNum + 1]["name"])
		self.drawScreen(self.currentPage)

		return 1


	def reloadPages(self, inputClass):

		self.currentPageNum = 0
		self.loadPages()
		self.switchPage(self.getPageID(self.PAGES[0]["name"]))
		self.drawScreen(self.currentPage)

		return 1

	def getBuildingKey(self, index):

		info = gc.getBuildingInfo(index)
		desc = info.getType()
		key = "_"
		key = key.join(desc.split())
		key = key.upper()
		return key

	def getBonusKey(self, index):

		info = gc.getBonusInfo(index)
		type = info.getType()
		key = "_"
		key = key.join(type.split())
		key = key.upper()
		return key

	def loadPages(self):

		self.PAGES = None
		name = SP.joinModDir("UserSettings", "CustomRevAdv", "CustomRevAdv.txt")
		if not SP.isFile(name):
			name = SP.joinModDir("UserSettings", "CustomRevAdv.txt")
		if SP.isFile(name):
			BugConfigTracker.add("CDA_Config", name)
			try:
				file = open(name, 'r')
				dict = pickle.load(file)
				version = dict["version"]
				self.PAGES = dict["pages"]
				file.close()

				if version == 0:
					for p in self.PAGES:
						newColumns = []
						for c in p["columns"]:
							for i in range(gc.getNumBuildingInfos()):
								info = gc.getBuildingInfo(i)
								desc = info.getDescription()
								key = "_"
								key = key.join(desc.split())
								key = "BUILDING_" + key.upper()
								if c[0] == key:
									c = (self.getBuildingKey(i), c[1], c[2])
									break
							for i in range(gc.getNumBonusInfos()):
								info = gc.getBonusInfo(i)
								desc = info.getDescription()
								key = "_"
								key = key.join(desc.split())
								key = "BONUS_" + key
								if c[0] == key:
									c = (self.getBonusKey(i), c[1], c[2])
									break
							newColumns.append(c)
						p["columns"] = newColumns

					# Updated from version 0 to version 1 format. Fall through to update version 1 format in the future
					version = 1

				if(version != self.PICKLED_VERSION):
					self.PAGES = None

			except IOError:
				self.PAGES = None
			except IndexError:
				self.PAGES = None
			except ValueError:
				self.PAGES = None
			except TypeError:
				self.PAGES = None

		# RevolutionDCM - legend control 9
		if(self.PAGES is None):
			if(self.isFlavorful):
				self.PAGES =	[
					{
						"name" : "Executive Summary",
						"showSpecControls" : False,
						"showRevolutionLegend" : False,
						"showGPLegend" : False,
						"columns" :	[
							("NAME", 95, "text"),
							("AUTOMATION", 80, "text"),
							("FEATURES", 92, "text"),
							("POPULATION", 35, "int"),
							("GARRISON", 30, "int"),
							("HAPPY", 30, "int"),
							("HEALTH", 30, "int"),
							("GROWTH", 35, "int"),
							("FOOD", 35, "int"),
							("PRODUCTION", 38, "int"),
							("MAINTENANCE", 30, "int"),
							("BASE_COMMERCE", 38, "int"),
							("GOLD", 38, "int"),
							("RESEARCH", 38, "int"),
							("CULTURE_RATE", 38, "int"),
							("CULTURE", 53, "int"),
							("GREATPEOPLE_RATE", 38, "int"),
							("GREATPEOPLE", 45, "int"),
							("PRODUCING", 90, "text"),
							("PRODUCING_TURNS", 33, "int"),
						]
					},
					{
						"name" : "Specialization",
						"showSpecControls" : True,
						"showRevolutionLegend" : True,
						"showGPLegend" : True,
						"columns" : [
							("NAME", 95, "text"),
							("FOUNDED", 80, "date"),
							("RELIGIONS", 90, "text"),
							("SPECIALISTS", 159, "text"),
							("HAPPY", 30, "int"),
							("GROWTH", 35, "int"),
							("FOOD", 35, "int"),
							("PRODUCTION", 38, "int"),
							("GOLD", 38, "int"),
							("RESEARCH", 38, "int"),
							("CULTURE_RATE", 38, "int"),
							("CULTURE", 53, "int"),
							("GREATPEOPLE_RATE", 38, "int"),
							("GREATPEOPLE", 45, "int"),
							("PRODUCING", 90, "text"),
							("PRODUCING_TURNS", 33, "int"),
						]
					},
					{
						"name" : "Top Cities - National",
						"showSpecControls" : False,
						"showRevolutionLegend" : False,
						"showGPLegend" : False,
						"columns" :	[
							("NAME", 95, "text"),
							("POPULATION", 35, "int"),
							("FEATURES", 92, "text"),
							("RELIGIONS", 90, "text"),
							("GOLD", 38, "int"),
							("NRANK_GOLD",	38, "int"),
							("RESEARCH", 38, "int"),
							("NRANK_RESEARCH", 38, "int"),
							("CULTURE_RATE", 38, "int"),
							("NRANK_CULTURE", 38, "int"),
							("CULTURE", 53, "int"),
							("PRODUCING", 90, "text"),
							("PRODUCING_TURNS", 33, "int"),
						]
					},
					{
						"name" : "Top Cities - Global",
						"showSpecControls" : False,
						"showRevolutionLegend" : False,
						"showGPLegend" : False,
						"columns" :	[
							("NAME", 95, "text"),
							("POPULATION", 35, "int"),
							("FEATURES", 92, "text"),
							("RELIGIONS", 90, "text"),
							("GOLD", 38, "int"),
							("GRANK_GOLD",	38, "int"),
							("RESEARCH", 38, "int"),
							("GRANK_RESEARCH", 38, "int"),
							("CULTURE_RATE", 38, "int"),
							("GRANK_CULTURE", 38, "int"),
							("CULTURE", 53, "int"),
							("PRODUCING", 90, "text"),
							("PRODUCING_TURNS", 33, "int"),
						]
					},
					{
						"name" : "Military Overview",
						"showSpecControls" : False,
						"showRevolutionLegend" : False,
						"showGPLegend" : False,
						"columns" : [
							("NAME", 95, "text"),
							("GARRISON", 30, "int"),
							("DEFENSE", 60, "int"),
							("THREATS", 60, "text"),
							("CONSCRIPT_UNIT", 90, "text"),
							(self.getBuildingKey(3), 70, "text"),
							(self.getBuildingKey(4), 70, "text"),
							(self.getBuildingKey(5), 70, "text"),
							(self.getBuildingKey(6), 70, "text"),
							(self.getBuildingKey(7), 70, "text"),
							("PRODUCING", 90, "text"),
							("PRODUCING_TURNS", 33, "int"),
						]
					}
					]
			else:
				self.PAGES =	[
					{
						"name" : "Default View",
						"showSpecControls" : False,
						"showRevolutionLegend" : False,
						"showGPLegend" : False,
						"columns" :	[
							("NAME", 95, "text"),
							("AUTOMATION", 80, "text"),
							("FEATURES", 92, "text"),
							("POPULATION", 35, "int"),
							("GARRISON", 30, "int"),
							("HAPPY", 30, "int"),
							("HEALTH", 30, "int"),
							("GROWTH", 35, "int"),
							("FOOD", 35, "int"),
							("PRODUCTION", 38, "int"),
							("MAINTENANCE", 30, "int"),
							("BASE_COMMERCE", 38, "int"),
							("GOLD", 38, "int"),
							("RESEARCH", 38, "int"),
							("CULTURE_RATE", 38, "int"),
							("CULTURE", 53, "int"),
							("GREATPEOPLE_RATE", 38, "int"),
							("GREATPEOPLE", 45, "int"),
							("PRODUCING", 90, "text"),
							("PRODUCING_TURNS", 33, "int"),
						]
					},
					{
						"name" : "Specialists",
						"showSpecControls" : True,
						"showRevolutionLegend" : True,
						"showGPLegend" : True,
						"columns" : [
							("NAME", 95, "text"),
							("FOUNDED", 80, "date"),
							("RELIGIONS", 90, "text"),
							("SPECIALISTS", 159, "text"),
							("HAPPY", 30, "int"),
							("GROWTH", 35, "int"),
							("FOOD", 35, "int"),
							("PRODUCTION", 38, "int"),
							("GOLD", 38, "int"),
							("RESEARCH", 38, "int"),
							("CULTURE_RATE", 38, "int"),
							("CULTURE", 53, "int"),
							("GREATPEOPLE_RATE", 38, "int"),
							("GREATPEOPLE", 45, "int"),
							("PRODUCING", 90, "text"),
							("PRODUCING_TURNS", 33, "int"),
						]
					},
					{
						"name" : "Growth",
						"showSpecControls" : False,
						"showRevolutionLegend" : False,
						"showGPLegend" : False,
						"columns" : [
							("NAME", 95, "text"),
							("POPULATION", 35, "int"),
							("HAPPY", 30, "int"),
							("HEALTH", 30, "int"),
							("GROWTH", 35, "int"),
							("FOOD", 35, "int"),
							(self.getBuildingKey(8), 70, "text"),
							(self.getBuildingKey(12), 70, "text"),
							(self.getBuildingKey(9), 70, "text"),
							(self.getBuildingKey(31), 70, "text"),
							(self.getBuildingKey(10), 70, "text"),
							(self.getBuildingKey(11), 70, "text"),
							(self.getBuildingKey(13), 70, "text"),
							(self.getBuildingKey(33), 70, "text"),
							("PRODUCING", 90, "text"),
							("PRODUCING_TURNS", 33, "int"),
						]
					},
					{
						"name" : "Military",
						"showSpecControls" : False,
						"showRevolutionLegend" : False,
						"showGPLegend" : False,
						"columns" : [
							("NAME", 95, "text"),
							("GARRISON", 30, "int"),
							("DEFENSE", 60, "int"),
							("THREATS", 60, "text"),
							("CONSCRIPT_UNIT", 90, "text"),
							(self.getBuildingKey(3), 70, "text"),
							(self.getBuildingKey(4), 70, "text"),
							(self.getBuildingKey(5), 70, "text"),
							(self.getBuildingKey(6), 70, "text"),
							(self.getBuildingKey(7), 70, "text"),
							("PRODUCING", 90, "text"),
							("PRODUCING_TURNS", 33, "int"),
						]
					}
				]

		# Fix and retain configuration from previous versions, when possible.
		for p in self.PAGES:

			if not p.has_key("name"):
				p["name"] = "Unnamed"

			if not p.has_key("showSpecControls"):
				p["showSpecControls"] = False

			# RevolutionDCM - legend control 10
			if not p.has_key("showRevolutionLegend"):
				p["showRevolutionLegend"] = False

			if not p.has_key("showGPLegend"):
				p["showGPLegend"] = False

			if not p.has_key("columns"):
				p["columns"] = [("NAME", 95, "text")]


	def renamePage(self, inputClass):
		eventManager = CvEventInterface.getEventManager()

		if not self.renameEventContext or self.renameEventContext is None:

			for i in range(5000, 6000):
				if not eventManager.Events.has_key(i):
					self.renameEventContext = i
					eventManager.Events[self.renameEventContext] = ('DomAdvRenamePage', self.renameApply, self.renameBegin)
					break

		CvEventInterface.beginEvent(self.renameEventContext)

	def renameBegin(self, argsList):
		popup = CyPopup(self.renameEventContext, EventContextTypes.EVENTCONTEXT_SELF, True)
		popup.setSize(400,175)
		popup.setUserData((self.currentPageNum, 0))
		popup.setHeaderString(u"Rename Revolution Watch Advisor", 1<<2)
		popup.setBodyString("Enter a name for this page", 1<<0)
		popup.createEditBox(self.PAGES[self.currentPageNum]["name"], 0)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	def renameApply(self, playerID, userData, popupReturn):
		self.customizingSaveSelection()

		self.PAGES[userData[0]]["name"] = popupReturn.getEditBoxString(0)

		self.drawScreen(self.currentPage)
		self.customizingRestoreSelection()


	def stripStr(self, s, out):
		while s.find(out) != -1:
			s = s[0:s.find(out)] + s[s.find(out) + 1:]

		return s
