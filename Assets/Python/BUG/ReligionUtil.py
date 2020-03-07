## ReligionUtil
##
## Collection of utility functions for dealing with religions.
## They are designed to be generic and work with new religions
## and even new types of religious buildings and units.
##
## Religions
##
##   getNumReligions()
##     Returns the number of religions.
##
##   isValid(iReligion)
##     Returns True if <iReligion> is a valid religion (not None or NO_RELIGION).
##
##   getAllReligions()
##     Returns a tuple containing all valid religion IDs (0, 1, ..., NUM_RELIGIONS-1).
##
##   getFoundedReligions()
##     Returns a list of religion IDs that have been founded.
##
## Buildings
##
##   getNumBuildingTypes()
##     Returns the number of religious building types.
##     This is normally 4 for Temple, Cathedral, Monastery, Shrine.
##
##   getBuildingTypes():
##     Returns the BuildingTypes in order.
##
##   getBuildingType(index)
##     Returns the BuildingType for <index> holding its description and icon.
##
##   getBuildings(iReligion)
##     Returns the list of building info types for <iReligion>.
##
##   getBuilding(iReligion, index)
##     Returns the single <index>th building info type for <iReligion>.
##
##   BUILDING_TEMPLE, BUILDING_CATHEDRAL, BUILDING_MONASTERY, BUILDING_SHRINE
##     Each building type defined gets a module-level constant.
##     The description is added to "BUILDING_" after making it uppercase and replacing spaces with underscores (_)
##
## Units
##
##   getNumUnitTypes()
##     Returns the number of religious unit types.
##     This is normally 1 for Missionary.
##
##   getUnitTypes():
##     Returns the UnitTypes in order.
##
##   getUnitType(index)
##     Returns the UnitType for <index> holding its description and icon.
##
##   getUnits(iReligion)
##     Returns the list of unit info types for <iReligion>.
##
##   getUnit(iReligion, index)
##     Returns the single <index>th unit info type for <iReligion>.
##
##   UNIT_MISSIONARY
##     Each unit type defined gets a module-level constant.
##     The description is added to "UNIT_" after making it uppercase and replacing spaces with underscores (_)
##
## Cities
##
##   getCityReligions(city)
##     Returns a list of religion IDs that are present in <city>.
##
##   getCityHolyReligions(city)
##     Returns a list of religion IDs for which <city> is the holy city.
##
## Players
##
##   getPlayerReligions(player)
##     Returns a list of religion IDs that are present in at least one of <player>'s cities.
##
##   getPlayerHolyReligions(player)
##     Returns a list of religion IDs for which <player> owns the holy city.
##
## MODDERS
##
##   - Nothing needs to be done here when adding new religions
##   - If you define new types of religious buildings, you must add a BuildingType() call to init()
##     and write a tester function.
##   - If you define new types of religious units, you must add a UnitType() call to init()
##     and write a tester function.
##
## Notes
##   - Must be initialized externally by calling init()
##
## Copyright (c) 2009 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *

## Globals

GC = CyGlobalContext()

NUM_RELIGIONS = -1
ALL_RELIGIONS = None

NUM_BUILDING_TYPES = 0
BUILDINGS = []
BUILDINGS_BY_RELIGION = []

NUM_UNIT_TYPES = 0
UNITS = []
UNITS_BY_RELIGION = []

# these constants are defined automatically during initialization
# they are here only to make Eclipse happy
BUILDING_TEMPLE = -1
BUILDING_CATHEDRAL = -1
BUILDING_MONASTERY = -1
BUILDING_SHRINE = -1
UNIT_MISSIONARY = -1


## Religions

def getNumReligions():
	"""
	Returns the number of religions.
	"""
	return NUM_RELIGIONS

def isValid(iReligion):
	"""
	Returns True if <iReligion> is a valid religion (not None or NO_RELIGION).
	"""
	return iReligion > -1 and iReligion < NUM_RELIGIONS

def getAllReligions():
	"""
	Returns a tuple containing all valid religion IDs (0, 1, ..., NUM_RELIGIONS-1).
	"""
	return ALL_RELIGIONS

def getFoundedReligions():
	"""
	Returns a list of religion IDs that have been founded.
	"""
	religions = []
	G = GC.getGame()
	for iReligion in ALL_RELIGIONS:
		if G.getReligionGameTurnFounded(iReligion) >= 0:
			religions.append(iReligion)
	return religions


## Buildings

def getNumBuildingTypes():
	"""
	Returns the number of religious building types.
	"""
	return NUM_BUILDING_TYPES

def getBuildingTypes():
	"""
	Returns the BuildingTypes in order.
	"""
	return BUILDINGS

def getBuildingType(index):
	"""
	Returns the <index>th BuildingType.
	"""
	try:
		return BUILDINGS[index]
	except IndexError:
		print "[ERR] ReligionUtil - invalid building type %i" % index
		return None

def getBuildings(iReligion):
	"""
	Returns the list of building info types for <iReligion>.
	"""
	try:
		return BUILDINGS_BY_RELIGION[iReligion]
	except IndexError:
		print "[ERR] ReligionUtil - invalid religion %i" % iReligion
		return [-1] * NUM_BUILDING_TYPES

def getBuilding(iReligion, index):
	"""
	Returns the single <index>th building info type for <iReligion>.
	"""
	try:
		return BUILDINGS_BY_RELIGION[iReligion][index]
	except IndexError:
		print "[ERR] ReligionUtil - invalid religion %i or building type %i" %(iReligion, index)
		return -1


def getBuildingReligion(info):
	"""
	Returns the religion <info> is tied to.

	All four building types have iReligionType set to the religion,
	but the Shrine doesn't have the religion as a prereq.
	No other buildings have iReligionType, so this should be safe.
	"""
	return info.getReligionType()

def isTemple(info, iReligion):
	"""
	Returns True if <info> is the Temple for <iReligion>.
	"""
	if not isWorldWonder(GC.getInfoTypeForString(info.getType())) and info.getSpecialBuildingType() == GC.getInfoTypeForString('SPECIALBUILDING_TEMPLE'):
		return info.getStateReligionHappiness() > 0
	return False

def isCathedral(info, iReligion):
	"""
	Returns True if <info> is the Cathedral for <iReligion>.
	"""
	iNum = GC.getNumSpecialBuildingInfos()
	if not isWorldWonder(GC.getInfoTypeForString(info.getType())) and info.getSpecialBuildingType() in (GC.getInfoTypeForString('SPECIALBUILDING_CATHEDRAL'), GC.getInfoTypeForString('SPECIALBUILDING_PANTHEON')):
		return info.getCommerceModifier(CommerceTypes.COMMERCE_CULTURE) > 0
	return False

def isMonastery(info, iReligion):
	"""
	Returns True if <info> is the Monastery for <iReligion>.
	"""
	if not isWorldWonder(GC.getInfoTypeForString(info.getType())) and info.getSpecialBuildingType() == GC.getInfoTypeForString('SPECIALBUILDING_MONASTERY'):
		return info.getCommerceModifier(CommerceTypes.COMMERCE_RESEARCH) > 0
	return False

def isShrine(info, iReligion):
	"""
	Returns True if <info> is the Shrine for <iReligion>.
	"""
	return info.getGlobalReligionCommerce() != -1


## Units

def getNumUnitTypes():
	"""
	Returns the number of religious unit types.
	"""
	return NUM_UNIT_TYPES

def getUnitTypes():
	"""
	Returns the UnitTypes in order.
	"""
	return UNITS

def getUnitType(index):
	"""
	Returns the <index>th UnitType.
	"""
	try:
		return UNITS[index]
	except IndexError:
		print "[ERR] ReligionUtil - invalid unit type %i" % index
		return None

def getUnits(iReligion):
	"""
	Returns the list of unit info types for <iReligion>.
	"""
	try:
		return UNITS_BY_RELIGION[iReligion]
	except IndexError:
		print "[ERR] ReligionUtil - invalid religion %i" % iReligion
		return [-1] * NUM_UNIT_TYPES

def getUnit(iReligion, index):
	"""
	Returns the single <index>th unit info type for <iReligion>.
	"""
	try:
		return UNITS_BY_RELIGION[iReligion][index]
	except IndexError:
		print "[ERR] ReligionUtil - invalid religion %i or unit type %i" %(iReligion, index)
		return -1


def getUnitReligion(info):
	"""
	Returns the religion <info> is tied to.

	<ReligionType> is not set for any units, but luckily only missionaries have <PrereqReligion>.
	"""
	return info.getPrereqReligion()

def isMissionary(info, iReligion):
	"""
	Returns True if <info> is the Missionary for <iReligion>.
	"""
	return info.getReligionSpreads(iReligion)


## Cities

def getCityReligions(city):
	"""
	Returns a list of religion IDs that are present in <city>.
	"""
	religions = []
	for eReligion in range(NUM_RELIGIONS):
		if city.isHasReligion(eReligion):
			religions.append(eReligion)
	return religions

def getCityHolyReligions(city):
	"""
	Returns a list of religion IDs for which <city> is the holy city.
	"""
	religions = []
	for eReligion in range(NUM_RELIGIONS):
		if city.isHolyCityByType(eReligion):
			religions.append(eReligion)
	return religions


## Players

def getPlayerReligions(player):
	"""
	Returns a list of religion IDs that are present in at least one of <player>'s cities.
	"""
	religions = []
	for eReligion in range(NUM_RELIGIONS):
		if player.getHasReligionCount(eReligion) > 0:
			religions.append(eReligion)
	return religions

def getPlayerHolyReligions(player):
	"""
	Returns a list of religion IDs for which <player> owns the holy city.
	"""
	religions = []
	for eReligion in range(NUM_RELIGIONS):
		if player.hasHolyCity(eReligion):
			religions.append(eReligion)
	return religions


## Initialization

def init():
	BuildingType("Temple", unichr(8850), isTemple)
	BuildingType("Cathedral", unichr(8502), isCathedral)
	BuildingType("Monastery", unichr(8501), isMonastery)
	BuildingType("Shrine", unichr(8500), isShrine)
	UnitType("Missionary", unichr(8857), isMissionary)
	global NUM_RELIGIONS, ALL_RELIGIONS
	NUM_RELIGIONS = GC.getNumReligionInfos()
	ALL_RELIGIONS = range(NUM_RELIGIONS)
	for iReligion in ALL_RELIGIONS:
		BUILDINGS_BY_RELIGION.append([-1] * NUM_BUILDING_TYPES)
		UNITS_BY_RELIGION.append([-1] * NUM_UNIT_TYPES)

	for iBldg in range(GC.getNumBuildingInfos()):
		bldg = GC.getBuildingInfo(iBldg)
		iReligion = getBuildingReligion(bldg)
		if iReligion >= 0:
			for type in BUILDINGS:
				if type.matches(bldg, iReligion):
					BUILDINGS_BY_RELIGION[iReligion][type.index] = iBldg
					print "ReligionUtil.init - %s %s is %i: %s" %(GC.getReligionInfo(iReligion).getDescription(), type.description, iBldg, bldg.getDescription())
					break

	for iUnit in range(GC.getNumUnitInfos()):
		unit = GC.getUnitInfo(iUnit)
		iReligion = getUnitReligion(unit)
		if iReligion >= 0:
			for type in UNITS:
				if type.matches(unit, iReligion):
					UNITS_BY_RELIGION[iReligion][type.index] = iUnit
					print "ReligionUtil.init - %s %s is %i: %s" %(GC.getReligionInfo(iReligion).getDescription(), type.description, iUnit, unit.getDescription())
					break


## Building and Unit Types

class Type:

	def __init__(self, type, description, icon, testFunc):
		counter = "NUM_%s_TYPES" % type
		key = type + "_" + description.upper().replace(" ", "_")
		g = globals()
		index = g[counter]
		g[counter] += 1
		g[key] = index
		g[type + "S"].append(self)
		self.index = index
		self.key = key
		self.description = description
		self.icon = icon
		self.testFunc = testFunc

	def matches(self, info, iReligion):
		"""
		Returns True if <info> matches the type of this object for <iReligion> by calling the test function.
		"""
		return self.testFunc(info, iReligion)

class BuildingType(Type):

	def __init__(self, description, icon, testFunc):
		"""
		Defines a new religious building type.

		description - display string such as "Temple" (may contain spaces)
		icon - string holding the icon (font glyph)
		testFunc - function that returns True if the given CvBuildingInfo matches the type
		"""
		Type.__init__(self, "BUILDING", description, icon, testFunc)

class UnitType(Type):

	def __init__(self, description, icon, testFunc):
		"""
		Defines a new religious unit type.

		description - display string such as "Missionary" (may contain spaces)
		icon - string holding the icon (font glyph)
		testFunc - function that returns True if the given CvUnitInfo matches the type
		"""
		Type.__init__(self, "UNIT", description, icon, testFunc)
