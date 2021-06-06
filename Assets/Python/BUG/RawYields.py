## RawYields
##
## Calculates the raw yields of food, production and commerce for a city
## and displays them in the trade table when enabled.
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *

GC = CyGlobalContext()

# Types
NUM_TYPES = 10
(
	WORKED_TILES,
	CITY_TILES,
	OWNED_TILES,
	ALL_TILES,

	DOMESTIC_TRADE,
	FOREIGN_TRADE, # excludes overseas trade

	BUILDINGS,
	CORPORATIONS,
	SPECIALISTS,

	# Hold the percents, not the actual yield values
	BASE_MODIFIER,
) = xrange(NUM_TYPES)

# Leave these for later when we have icons for each
#DOMAIN_MODIFIER
#MILITARY_MODIFIER
#TRAIT_MODIFIER
#CIVIC_MODIFIER
#RELIGION_MODIFIER
#BONUS_MODIFIER
#WONDER_MODIFIER
#...

# Labels
LABEL_KEYS = (
	"TXT_KEY_CONCEPT_WORKED_TILES",
	"TXT_KEY_CONCEPT_CITY_TILES",
	"TXT_KEY_CONCEPT_OWNED_TILES",
	"TXT_KEY_CONCEPT_ALL_TILES",
	"TXT_KEY_CONCEPT_DOMESTIC_TRADE",
	"TXT_KEY_CONCEPT_FOREIGN_TRADE",
	"TXT_KEY_CONCEPT_BUILDINGS",
	"TXT_KEY_CONCEPT_CORPORATIONS",
	"TXT_KEY_CONCEPT_SPECIALISTS",
	"TXT_KEY_CONCEPT_BASE_MODIFIER"
)
# Yields
YIELDS = (YieldTypes.YIELD_FOOD, YieldTypes.YIELD_PRODUCTION, YieldTypes.YIELD_COMMERCE)
# Tiles
TILES = (WORKED_TILES, CITY_TILES, OWNED_TILES, ALL_TILES)

def getViewAndType(iView):
	# Returns the view boolean and YieldTypes enum given the give number 0-3.
	if iView == 0:
		return (False, YieldTypes.YIELD_FOOD)
	elif iView in (1, 2, 3):
		return (True, YIELDS[iView - 1])
	else:
		print "RawYields - invalid view-type: %d" %iView
		return (False, YieldTypes.YIELD_FOOD)

class Tracker:

	def __init__(self):
		# Creates a table to hold all of the tracked values for each yield type.
		self.values = {}
		for eYield in xrange(YieldTypes.NUM_YIELD_TYPES):
			self.values[eYield] = {}
			for eType in xrange(NUM_TYPES):
				self.values[eYield][eType] = 0
		self.tileCounts = [0, 0, 0, 0]

	def addBuilding(self, eYield, iValue):
		self.values[eYield][BUILDINGS] += iValue

	def addDomesticTrade(self, eYield, iValue):
		self.values[eYield][DOMESTIC_TRADE] += iValue

	def addForeignTrade(self, eYield, iValue):
		self.values[eYield][FOREIGN_TRADE] += iValue

	def processCity(self, CyCity):
		"""
		Calculates the yields for the given city's tiles, specialists, corporations and multipliers.
		The building and trade yields are calculated by CvMainInterface.
		"""
		# Calculates the yield for all city-tiles.
		iPlayer = CyCity.getOwner()
		for i in xrange(GC.getNUM_CITY_PLOTS()):
			CyPlot = CyCity.getCityIndexPlot(i)
			if CyPlot and CyPlot.hasYield():

				if CyCity.isWorkingPlot(CyPlot):
					iTileType = WORKED_TILES
				elif CyCity.canWork(CyPlot):
					iTileType = CITY_TILES
				elif CyPlot.getOwner() == iPlayer:
					iTileType = OWNED_TILES
				else:
					iTileType = ALL_TILES

				for eYield in YIELDS:
					iValue = CyPlot.getYield(eYield)
					if iValue:
						for eType in xrange(iTileType, ALL_TILES + 1):
							self.values[eYield][eType] += iValue
				for eType in xrange(iTileType, ALL_TILES + 1):
					self.tileCounts[eType] += 1

		CyPlayer = GC.getPlayer(iPlayer)
		iValue = 0
		for eYield in YIELDS:
			# Calculates the yield from specialists.
			for eType in xrange(GC.getNumSpecialistInfos()):
				iValue += CyPlayer.specialistYield(eType, eYield) * (CyCity.getSpecialistCount(eType) + CyCity.getFreeSpecialistCount(eType))
			if iValue:
				self.values[eYield][SPECIALISTS] += iValue
				iValue = 0
			# Calculates the yield from corporations.
			for eType in xrange(GC.getNumCorporationInfos()):
				if CyCity.isHasCorporation(eType):
					iValue += CyCity.getCorporationYieldByCorporation(eYield, eType)
			if iValue:
				self.values[eYield][CORPORATIONS] += iValue
				iValue = 0
			# Account for yield modifiers.
			iValue = CyCity.getBaseYieldRateModifier(eYield, 0) - 100
			if iValue:
				self.values[eYield][BASE_MODIFIER] += iValue
				iValue = 0


	def fillTable(self, screen, table, eYield, eType):
		TRNSLTR = CyTranslator()
		# Fills the given GFC table control with the chosen yield values.
		self.iRow = 0
		# Tiles
		iTotal = self.values[eYield][eType]
		self.appendTable(screen, table, False, TRNSLTR.getText(LABEL_KEYS[eType], (self.tileCounts[eType],)), eYield, iTotal)

		# Trade
		for eType in (DOMESTIC_TRADE, FOREIGN_TRADE):
			iValue = self.values[eYield][eType]
			if iValue:
				self.appendTable(screen, table, False, TRNSLTR.getText(LABEL_KEYS[eType], ()), eYield, iValue, True)
		iValue = self.values[eYield][DOMESTIC_TRADE] + self.values[eYield][FOREIGN_TRADE]
		iValue /= 100
		iTotal += iValue

		# Buildings, Corporations, Specialists
		for eType in (BUILDINGS, CORPORATIONS, SPECIALISTS):
			iValue = self.values[eYield][eType]
			if iValue:
				iTotal += iValue
				self.appendTable(screen, table, False, TRNSLTR.getText(LABEL_KEYS[eType], ()), eYield, iValue)

		# Subtotal and Base Modifiers
		iModifier = self.values[eYield][BASE_MODIFIER]
		if iModifier:
			# Subtotal
			self.appendTableTotal(screen, table, eYield, iTotal)
			# Modifier
			iValue = (iTotal * (iModifier + 100) // 100) - iTotal
			iSubtotal = iTotal + iValue
			self.appendTable(screen, table, False, TRNSLTR.getText("TXT_KEY_CONCEPT_BASE_MODIFIER", (iModifier,)), eYield, iValue)
		else:
			iSubtotal = iTotal

		iTotal = iSubtotal

		# Total
		self.appendTableTotal(screen, table, eYield, iTotal)

	def appendTable(self, screen, table, bTotal, heading, eYield, iValue, bFraction=False):
		"""
		Appends the given yield value to the table control.
		If bTotal is True, the heading is colored yellow and there's no + sign on the value.
		"""
		cYield = GC.getYieldInfo(eYield).getChar()
		screen.appendTableRow(table)
		if bTotal:
			heading = u"<color=205,180,55,255>%s</color>" % heading
			value = u"<color=205,180,55,255>%d</color>" % iValue
			if bFraction:
				# showing fraction doesn't fit in column
				value = u"<color=205,180,55,255>%d</color>" % (iValue / 100)
			else:
				value = u"<color=205,180,55,255>%d</color>" % iValue
		else:
			if bFraction:
				# showing fraction doesn't fit in column
				value = u"%+d" % (iValue / 100)
			else:
				value = u"%+d" % iValue
		screen.setTableText(table, 0, self.iRow, u"<font=1>%s</font>" % (heading), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.setTableText(table, 1, self.iRow, u"<font=1>%s%c</font>" % (value, cYield), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<1)
		self.iRow += 1

	def appendTableTotal(self, screen, table, eYield, iValue):
		"""
		Appends the given yield total to the table control's 3rd running total column.
		"""
		if self.iRow > 0:
			cYield = GC.getYieldInfo(eYield).getChar()
			value = u"<color=205,180,55,255>%d</color>" % iValue
			screen.setTableText(table, 2, self.iRow - 1, u"<font=1>%s%c</font>" % (value, cYield), "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<1)