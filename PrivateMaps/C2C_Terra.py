#
#	FILE:	 Terra.py
#	AUTHOR:  Bob Thomas (Sirian)
#	PURPOSE: Global map script - Simulates Terran (Earth-like) worlds
#-----------------------------------------------------------------------------
#	Copyright (c) 2005 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvMapGeneratorUtil

balancer = CvMapGeneratorUtil.BonusBalancer()

'''
MULTILAYERED FRACTAL NOTES

The MultilayeredFractal class was created for use with this script.

I worked to make it adaptable to other scripts, though, and eventually it
migrated in to the MapUtil file along with the other primary map classes.

- Bob Thomas   July 13, 2005


TERRA NOTES

Terra turns out to be our largest size map. This is the only map script
in the original release of Civ4 where the grids are this large!

This script is also the one that got me started in to map scripting. I had
this idea early in the development cycle and just kept pestering until Soren
turned me loose on it, finally. Once I got going, I just kept on going!

- Bob Thomas   September 20, 2005
'''

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_TERRA_DESCR"

def isAdvancedMap():
	"This map should show up in simple mode"
	return 0

def getNumCustomMapOptions():
	return 2

def getNumHiddenCustomMapOptions():
	return 2

def getCustomMapOptionName(argsList):
	[iOption] = argsList
	option_names = {
		0:	"TXT_KEY_MAP_WORLD_WRAP",
		1:  "TXT_KEY_CONCEPT_RESOURCES"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0: 3,
		1: 2
	}
	return option_values[iOption]

def getCustomMapOptionDescAt(argsList):
	[iOption, iSelection] = argsList
	selection_names = {
		0: {
			0: "TXT_KEY_MAP_WRAP_FLAT",
			1: "TXT_KEY_MAP_WRAP_CYLINDER",
			2: "TXT_KEY_MAP_WRAP_TOROID"
		},
		1: {
			0: "TXT_KEY_WORLD_STANDARD",
			1: "TXT_KEY_MAP_BALANCED"
		}
	}
	translated_text = unicode(CyTranslator().getText(selection_names[iOption][iSelection], ()))
	return translated_text

def getCustomMapOptionDefault(argsList):
	[iOption] = argsList
	option_defaults = {
		0: 1,
		1: 0
	}
	return option_defaults[iOption]

def isRandomCustomMapOption(argsList):
	[iOption] = argsList
	option_random = {
		0: false,
		1: false
	}
	return option_random[iOption]

def getWrapX():
	map = CyMap()
	return (map.getCustomMapOption(0) == 1 or map.getCustomMapOption(0) == 2)

def getWrapY():
	return (CyMap().getCustomMapOption(0) == 2)

def normalizeAddExtras():
	if CyMap().getCustomMapOption(1) == 1:
		balancer.normalizeAddExtras()
	CyPythonMgr().allowDefaultImpl()	# do the rest of the usual normalizeStartingPlots stuff, don't overrride

def addBonusType(argsList):
	[iBonusType] = argsList
	gc = CyGlobalContext()
	type_string = gc.getBonusInfo(iBonusType).getType()

	if (CyMap().getCustomMapOption(1) == 1):
		if (type_string in balancer.resourcesToBalance) or (type_string in balancer.resourcesToEliminate):
			return None # don't place any of this bonus randomly

	CyPythonMgr().allowDefaultImpl() # pretend we didn't implement this method, and let C handle this bonus in the default way

def getGridSize(argsList):
	grid_sizes = {
	## Mini-Map Aspect Ratio Changed to 1.5. Now it is standard size definitions for all map scripts.
		WorldSizeTypes.WORLDSIZE_DUEL:		(12, 8),
		WorldSizeTypes.WORLDSIZE_TINY:		(18, 12),
		WorldSizeTypes.WORLDSIZE_SMALL:		(21, 14),
		WorldSizeTypes.WORLDSIZE_STANDARD:	(24, 16),
		WorldSizeTypes.WORLDSIZE_LARGE:		(27, 18),
		WorldSizeTypes.WORLDSIZE_HUGE:		(30, 20),
		6:									(33, 22),
		7:									(36, 24)
	}

	if argsList[0] == -1: # (-1,) is passed to function on loads
		return []
	[eWorldSize] = argsList
	return grid_sizes[eWorldSize]

def minStartingDistanceModifier():
	return -20

def findStartingPlot(argsList):
	[playerID] = argsList

	def isValid(playerID, x, y):
		map = CyMap()
		pPlot = map.plot(x, y)

		if pPlot.getArea() != map.findBiggestArea(False).getID():
			return False

		return True

	return CvMapGeneratorUtil.findStartingPlot(playerID, isValid)

class TerraMultilayeredFractal(CvMapGeneratorUtil.MultilayeredFractal):
	# Subclass. Only the controlling function overridden in this case.
	def generatePlotsByRegion(self):
		# Sirian's MultilayeredFractal class, controlling function.
		# You -MUST- customize this function for each use of the class.
		#
		# The following grain matrix is specific to Terra.py
		sizekey = self.map.getWorldSize()
		sizevalues = {
			WorldSizeTypes.WORLDSIZE_DUEL:	  (3,2,1,2),
			WorldSizeTypes.WORLDSIZE_TINY:	  (3,2,1,2),
			WorldSizeTypes.WORLDSIZE_SMALL:	 (4,2,1,2),
			WorldSizeTypes.WORLDSIZE_STANDARD:  (4,2,1,2),
			WorldSizeTypes.WORLDSIZE_LARGE:	 (4,2,1,2),
			WorldSizeTypes.WORLDSIZE_HUGE:	  (5,2,1,2),
			6:									(5,2,1,2),
			7:									(6,2,1,2)
			}
		if sizekey not in sizevalues:
			(archGrain, contGrain, gaeaGrain, eurasiaGrain) = (5,2,1,2)
		else:
			(archGrain, contGrain, gaeaGrain, eurasiaGrain) = sizevalues[sizekey]

		# Sea Level adjustment (from user input), limited to value of 5%.
		sea = self.GC.getSeaLevelInfo(self.map.getSeaLevel()).getSeaLevelChange()
		sea = min(sea, 5)
		sea = max(sea, -5)

		# The following regions are specific to Terra.py
		newworldWestLon = 0.05
		newworldEastLon = 0.35
		eurasiaWestLon = 0.45
		eurasiaEastLon = 0.95
		eurasiaNorthLat = 0.95
		eurasiaSouthLat = 0.45
		thirdworldDimension = 0.125
		thirdworldNorthLat = 0.35
		thirdworldSouthLat = 0.05
		subcontinentLargeHorz = 0.2
		subcontinentLargeVert = 0.32
		subcontinentLargeNorthLat = 0.6
		subcontinentLargeSouthLat = 0.28
		subcontinentSmallDimension = 0.125
		subcontinentSmallNorthLat = 0.525
		subcontinentSmallSouthLat = 0.4

		# Dice rolls to randomize the quadrants (specific to Terra.py's regions)
		roll1 = self.dice.get(2, "Eurasian Hemisphere N/S - Terra PYTHON")
		if roll1 == 1:
			eurasiaNorthLat -= 0.4; eurasiaSouthLat -= 0.4
			thirdworldNorthLat += 0.6; thirdworldSouthLat += 0.6
			subcontinentLargeNorthLat += 0.12; subcontinentLargeSouthLat += 0.12
			subcontinentSmallNorthLat += 0.075; subcontinentSmallSouthLat += 0.075
		roll2 = self.dice.get(2, "Eurasian Hemisphere E/W - Terra PYTHON")
		if roll2 == 1:
			newworldWestLon += 0.6; newworldEastLon += 0.6
			eurasiaWestLon -= 0.4; eurasiaEastLon -= 0.4

		# Simulate the Old World - a large continent akin to Earth's Eurasia.
		NiTextOut("Generating the Old World (Python Terra) ...")
		# Set dimensions of the Old World region (specific to Terra.py)
		eurasiaWestX = int(self.iW * eurasiaWestLon)
		eurasiaEastX = int(self.iW * eurasiaEastLon)
		eurasiaNorthY = int(self.iH * eurasiaNorthLat)
		eurasiaSouthY = int(self.iH * eurasiaSouthLat)
		eurasiaWidth = eurasiaEastX - eurasiaWestX + 1
		eurasiaHeight = eurasiaNorthY - eurasiaSouthY + 1

		eurasiaWater = 55+sea

		self.generatePlotsInRegion(
			eurasiaWater,
			eurasiaWidth, eurasiaHeight,
			eurasiaWestX, eurasiaSouthY,
			eurasiaGrain, archGrain,
			self.iHorzFlags, self.iTerrainFlags,
			-1, -1,
			True, 11,
			2, False,
			False
		)
		# Eurasia, second layer (to increase pangaea-like cohesion).
		twHeight = eurasiaHeight/2
		twWestX = eurasiaWestX + eurasiaWidth/10
		twEastX = eurasiaEastX - eurasiaWidth/10
		twWidth = twEastX - twWestX + 1
		twNorthY = eurasiaNorthY - eurasiaHeight/4
		twSouthY = eurasiaSouthY + eurasiaHeight/4

		twWater = 60+sea; twGrain = 1; twRift = 2

		self.generatePlotsInRegion(
			twWater,
			twWidth, twHeight,
			twWestX, twSouthY,
			twGrain, archGrain,
			self.iHorzFlags, self.iTerrainFlags,
			-1, -1,
			True, 11,
			twRift, False,
			False
		)
		# Simulate the New World - land masses akin to Earth's American continents.
		# First simulate North America
		NiTextOut("Generating the New World (Python Terra) ...")
		nwWestX = int(self.iW * newworldWestLon)
		nwEastX = int(self.iW * newworldEastLon)
		nwNorthY = int(self.iH * 0.85)
		nwSouthY = int(self.iH * 0.52)
		nwWidth = nwEastX - nwWestX + 1
		nwHeight = nwNorthY - nwSouthY + 1

		nwWater = 61+sea; nwGrain = 1; nwRift = -1

		self.generatePlotsInRegion(
			nwWater,
			nwWidth, nwHeight,
			nwWestX, nwSouthY,
			nwGrain, archGrain,
			self.iVertFlags, self.iTerrainFlags,
			6, 6,
			True, 7,
			nwRift, False,
			False
		)
		# Now simulate South America
		nwsRoll = self.dice.get(2, "New World South E/W - Terra PYTHON")
		nwsVar = 0.0
		if nwsRoll == 1: nwsVar = 0.05
		nwsWestX = nwWestX + int(self.iW * (0.08 - nwsVar)) # Not as wide as the north
		nwsEastX = nwEastX - int(self.iW * (0.03 + nwsVar))
		nwsNorthY = int(self.iH * 0.47)
		nwsSouthY = int(self.iH * 0.25)
		nwsWidth = nwsEastX - nwsWestX + 1
		nwsHeight = nwsNorthY - nwsSouthY + 1

		nwsWater = 55+sea; nwsGrain = 1; nwsRift = -1

		self.generatePlotsInRegion(
			nwsWater,
			nwsWidth, nwsHeight,
			nwsWestX, nwsSouthY,
			nwsGrain, archGrain,
			self.iRoundFlags, self.iTerrainFlags,
			6, 6,
			True, 5,
			nwsRift, False,
			False
		)
		nwpWestX = nwWestX + int(self.iW * (0.1 - nwsVar)) # Not as wide as the north
		nwpEastX = nwEastX - int(self.iW * (0.07 + nwsVar))
		nwpNorthY = int(self.iH * 0.3)
		nwpSouthY = int(self.iH * 0.18)
		nwpWidth = nwpEastX - nwpWestX + 1
		nwpHeight = nwpNorthY - nwpSouthY + 1

		nwpWater = 67+sea; nwpGrain = 1; nwpRift = -1

		self.generatePlotsInRegion(
			nwpWater,
			nwpWidth, nwpHeight,
			nwpWestX, nwpSouthY,
			nwpGrain, archGrain,
			self.iVertFlags, self.iTerrainFlags,
			6, 5,
			True, 3,
			nwpRift, False,
			False
		)
		# Now the Yukon
		twWidth = int(self.iW * 0.15)
		twWestX = nwWestX
		boreal = self.dice.get(2, "New World North E/W - Terra PYTHON")
		if boreal == 1: twWestX += int(self.iW * 0.15)
		twEastX = twWestX + twWidth
		twNorthY = int(self.iH * 0.93)
		twSouthY = int(self.iH * 0.75)
		twHeight = twNorthY - twSouthY + 1

		twWater = 68+sea; twGrain = 2; twRift = -1

		self.generatePlotsInRegion(
			twWater,
			twWidth, twHeight,
			twWestX, twSouthY,
			twGrain, archGrain,
			self.iRoundFlags, self.iTerrainFlags,
			6, 5,
			True, 5,
			twRift, False,
			False
		)
		# Now add a random region of arctic islands
		twWidth = int(thirdworldDimension * self.iW)
		twHeight = int(thirdworldDimension * self.iH)
		if not boreal:
			twEastX = nwEastX
			twWestX = twEastX - twWidth
		else:
			twWestX = nwWestX
			twEastX = twWestX + twWidth
		twNorthY = int(self.iH * 0.975)
		twSouthY = int(self.iH * 0.85)

		twWater = 76+sea; twGrain = archGrain; twRift = -1

		self.generatePlotsInRegion(
			twWater,
			twWidth, twHeight,
			twWestX, twSouthY,
			twGrain, archGrain,
			self.iHorzFlags, self.iTerrainFlags,
			6, 5,
			True, 5,
			twRift, False,
			False
		)
 		# Now simulate Central America
 		nwcVar = 0.0
		if nwsRoll == 1: nwcVar = 0.04
		nwcWidth = int(self.iW * 0.06)
		nwcRoll = self.dice.get(2, "Central America and Carribean Placement - Terra PYTHON")
		nwcWestX = nwWestX + int(self.iW * (0.1 + nwcVar))
		nwcEastX = nwcWestX + nwcWidth
		nwcNorthY = int(self.iH * 0.6)
		nwcSouthY = int(self.iH * 0.42)
		nwcHeight = nwcNorthY - nwcSouthY + 1

		nwcWater = 60+sea; nwcGrain = 1; nwcRift = -1

		self.generatePlotsInRegion(
			nwcWater,
			nwcWidth, nwcHeight,
			nwcWestX, nwcSouthY,
			nwcGrain, archGrain,
			self.iVertFlags, self.iTerrainFlags,
			6, 5,
			True, 5,
			nwcRift, False,
			False
		)
		# Now the Carribean islands
 		carVar = 0.0
		if nwsRoll == 1: carVar = 0.15
		twWidth = int(0.15 * self.iW)
		twEastX = nwEastX - int(carVar * self.iW)
		twWestX = twEastX - twWidth
		twNorthY = int(self.iH * 0.55)
		twSouthY = int(self.iH * 0.47)
		twHeight = twNorthY - twSouthY + 1

		twWater = 75+sea; twGrain = archGrain + 1; twRift = -1

		self.generatePlotsInRegion(
			twWater,
			twWidth, twHeight,
			twWestX, twSouthY,
			twGrain, archGrain,
			0, self.iTerrainFlags,
			6, 5,
			True, 3,
			twRift, False,
			False
		)
		# Add subcontinents to the Old World, one large, one small. (Terra.py)
		# Subcontinents can be akin to pangaea, continents, or archipelago.
		# The large adds an amount of land akin to subSaharan Africa.
		# The small adds an amount of land akin to South Pacific islands.
		NiTextOut("Generating the Third World (Python Terra) ...")
		scLargeWidth = int(subcontinentLargeHorz * self.iW)
		scLargeHeight = int(subcontinentLargeVert * self.iH)
		scRoll = self.dice.get((eurasiaWidth - scLargeWidth), "Large Subcontinent Placement - Terra PYTHON")
		scWestX = eurasiaWestX + scRoll
		scEastX = scWestX + scLargeWidth
		scNorthY = int(self.iH * subcontinentLargeNorthLat)
		scSouthY = int(self.iH * subcontinentLargeSouthLat)

		scShape = self.dice.get(4, "Large Subcontinent Shape - Terra PYTHON")
		if scShape > 1: # Massive subcontinent! (Africa style)
			scWater = 55+sea; scGrain = 1; scRift = 2
		elif scShape == 1: # Standard subcontinent.
			scWater = 66+sea; scGrain = 2; scRift = 2
		else: # scShape == 0, Archipelago subcontinent.
			scWater = 77+sea; scGrain = archGrain; scRift = -1

		self.generatePlotsInRegion(
			scWater,
			scLargeWidth, scLargeHeight,
			scWestX, scSouthY,
			scGrain, archGrain,
			self.iRoundFlags, self.iTerrainFlags,
			6, 6,
			True, 7,
			scRift, False,
			False
		)

		scSmallWidth = int(subcontinentSmallDimension * self.iW)
		scSmallHeight = int(subcontinentSmallDimension * self.iH)
		endless = 1
		while endless == 1: # Prevent excessive overlap of the two subcontinents.
			scsRoll = self.dice.get((eurasiaWidth - scSmallWidth), "Small Subcontinent Placement - Terra PYTHON")
			scsWestX = eurasiaWestX + scsRoll
			if abs((scsWestX + self.iW/12) - scWestX) > self.iW/8: break
		scsEastX = scsWestX + scSmallWidth
		scsNorthY = int(self.iH * subcontinentSmallNorthLat)
		scsSouthY = int(self.iH * subcontinentSmallSouthLat)

		scsShape = self.dice.get(4, "Small Subcontinent Shape - Terra PYTHON")
		if scsShape == 2: # Massive subcontinent!
			scsWater = 55+sea; scsGrain = 1; scsRift = 2
		elif scsShape == 1: # Standard subcontinent. (India style).
			scsWater = 66+sea; scsGrain = 2; scsRift = 2
		else: # scsShape == 0 or 3, Archipelago subcontinent (East Indies style).
			scsWater = 77+sea; scsGrain = archGrain; scsRift = -1

		self.generatePlotsInRegion(
			scsWater,
			scSmallWidth, scSmallHeight,
			scsWestX, scsSouthY,
			scsGrain, archGrain,
			self.iHorzFlags, self.iTerrainFlags,
			6, 5,
			True, 5,
			scsRift, False,
			False
		)
		# Now simulate random lands akin to Australia and Antarctica
		extras = 2 + self.dice.get(3, "Number of Minor Regions - Terra PYTHON")
		for loop in range(extras):
			# Two to four of these regions.
			twWidth = int(thirdworldDimension * self.iW)
			twHeight = int(thirdworldDimension * self.iH)
			twVertRange = int(0.3 * self.iH) - twHeight
			twRoll = self.dice.get((eurasiaWidth - twWidth), "Minor Region Placement - Terra PYTHON")
			twWestX = eurasiaWestX + twRoll
			twEastX = scWestX + scLargeWidth
			twVertRoll = self.dice.get(twVertRange, "Minor Region Placement - Terra PYTHON")
			twNorthY = int(self.iH * thirdworldNorthLat) + twVertRoll
			twSouthY = int(self.iH * thirdworldSouthLat) + twVertRoll

			twShape = self.dice.get(3, "Minor Region Shape - Terra PYTHON")
			if twShape == 2: # Massive subcontinent!
				twWater = 60+sea; twGrain = 1; twRift = 2
			elif twShape == 1: # Standard subcontinent.
				twWater = 65+sea; twGrain = 2; twRift = 2
			else: # twShape == 0, Archipelago subcontinent.
				twWater = 70+sea; twGrain = archGrain; twRift = -1

			self.generatePlotsInRegion(
				twWater,
				twWidth, twHeight,
				twWestX, twSouthY,
				twGrain, archGrain,
				self.iHorzFlags, self.iTerrainFlags,
				6, 5,
				True, 5,
				twRift, False,
				False
			)
		# All regions have been processed. Plot Type generation completed.
		return self.wholeworldPlotTypes

'''
Regional Variables Key:

iWaterPercent,
iRegionWidth, iRegionHeight,
iRegionWestX, iRegionSouthY,
iRegionGrain, iRegionHillsGrain,
iRegionPlotFlags, iRegionTerrainFlags,
iRegionFracXExp, iRegionFracYExp,
bShift, iStrip,
rift_grain, has_center_rift,
invert_heights
'''

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Terra) ...")
	# Call generatePlotsByRegion() function, from TerraMultilayeredFractal subclass.
	plotgen = TerraMultilayeredFractal()
	return plotgen.generatePlotsByRegion()

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Terra) ...")
	terraingen = CvMapGeneratorUtil.TerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

def addFeatures():
	NiTextOut("Adding Features (Python Terra) ...")
	featuregen = CvMapGeneratorUtil.FeatureGenerator()
	featuregen.addFeatures()

def afterGeneration():
	CvMapGeneratorUtil.placeC2CBonuses()
