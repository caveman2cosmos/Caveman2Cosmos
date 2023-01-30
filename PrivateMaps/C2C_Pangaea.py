#
#	FILE:	 Pangaea.py
#	AUTHOR:  Bob Thomas (Sirian)
#	CONTRIB: Soren Johnson, Andy Szybalski
#	PURPOSE: Global map script - Simulates a Pan-Earth SuperContinent
#-----------------------------------------------------------------------------
#	Copyright (c) 2005 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvMapGeneratorUtil as MGU

balancer = MGU.BonusBalancer()

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_PANGAEA_DESCR"

def getNumCustomMapOptions():
	return 3

def getNumHiddenCustomMapOptions():
	return 2

def getCustomMapOptionName(argsList):
	[iOption] = argsList
	option_names = {
		0:	"TXT_KEY_MAP_SCRIPT_SHORELINE",
		1:	"TXT_KEY_MAP_WORLD_WRAP",
		2:  "TXT_KEY_CONCEPT_RESOURCES"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0:	4,
		1:	3,
		2:  2
		}
	return option_values[iOption]

def getCustomMapOptionDescAt(argsList):
	[iOption, iSelection] = argsList
	selection_names = {
		0:	{
			0: "TXT_KEY_MAP_SCRIPT_RANDOM",
			1: "TXT_KEY_MAP_SCRIPT_NATURAL",
			2: "TXT_KEY_MAP_SCRIPT_PRESSED",
			3: "TXT_KEY_MAP_SCRIPT_SOLID"
			},
		1:	{
			0: "TXT_KEY_MAP_WRAP_FLAT",
			1: "TXT_KEY_MAP_WRAP_CYLINDER",
			2: "TXT_KEY_MAP_WRAP_TOROID"
			},
		2:	{
			0: "TXT_KEY_WORLD_STANDARD",
			1: "TXT_KEY_MAP_BALANCED"
			}
		}
	translated_text = unicode(CyTranslator().getText(selection_names[iOption][iSelection], ()))
	return translated_text

def getCustomMapOptionDefault(argsList):
	[iOption] = argsList
	option_defaults = {
		0:	0,
		1:	1,
		2:  0
		}
	return option_defaults[iOption]

def isRandomCustomMapOption(argsList):
	[iOption] = argsList
	option_random = {
		0:	false,
		1:	false,
		2:  false
		}
	return option_random[iOption]

def getWrapX():
	map = CyMap()
	return (map.getCustomMapOption(1) == 1 or map.getCustomMapOption(1) == 2)

def getWrapY():
	map = CyMap()
	return (map.getCustomMapOption(1) == 2)

def normalizeAddExtras():
	if CyMap().getCustomMapOption(2) == 1:
		balancer.normalizeAddExtras()
	CyPythonMgr().allowDefaultImpl()	# do the rest of the usual normalizeStartingPlots stuff, don't overrride

def addBonusType(argsList):
	[iBonusType] = argsList

	if CyMap().getCustomMapOption(2) == 1 and CyGlobalContext().getBonusInfo(iBonusType).getType() in balancer.resourcesToBalance:
		return None # don't place any of this bonus randomly

	CyPythonMgr().allowDefaultImpl() # pretend we didn't implement this method, and let C handle this bonus in the default way

def isAdvancedMap():
	"This map should show up in simple mode"
	return 0

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

	if (argsList[0] == -1): # (-1,) is passed to function on loads
		return []
	[eWorldSize] = argsList
	return grid_sizes[eWorldSize]

def beforeGeneration():
	# Detect whether this game is primarily a team game or not. (1v1 treated as a team game!)
	# Team games, everybody starts on the coast. Otherwise, start anywhere on the pangaea.
	global isTeamGame
	GAME = CyGame()
	iPlayers = GAME.countCivPlayersEverAlive()
	iTeams = GAME.countCivTeamsEverAlive()
	if iPlayers >= iTeams * 2 or iPlayers == 2:
		isTeamGame = True
	else:
		isTeamGame = False

class PangaeaHintedWorld:
	def generateSorensHintedPangaea(self):
		NiTextOut("Setting Plot Types (Python Pangaea) ...")
		global hinted_world
		hinted_world = MGU.HintedWorld(8,4)

		mapRand = CyGame().getMapRand()

		for y in range(hinted_world.h):
			for x in range(hinted_world.w):
				if x in (0, hinted_world.w-1) or y in (0, hinted_world.h-1):
					hinted_world.setValue(x,y,0)
				else:
					hinted_world.setValue(x,y,200 + mapRand.get(55, "Plot Types - Pangaea PYTHON"))

		hinted_world.setValue(1, 1 + mapRand.get(3, "Plot Types - Pangaea PYTHON"), mapRand.get(64, "Plot Types - Pangaea PYTHON"))
		hinted_world.setValue(2 + mapRand.get(2, "Plot Types - Pangaea PYTHON"), 1 + mapRand.get(3, "Plot Types - Pangaea PYTHON"), mapRand.get(64, "Plot Types - Pangaea PYTHON"))
		hinted_world.setValue(4 + mapRand.get(2, "Plot Types - Pangaea PYTHON"), 1 + mapRand.get(3, "Plot Types - Pangaea PYTHON"), mapRand.get(64, "Plot Types - Pangaea PYTHON"))
		hinted_world.setValue(6, 1 + mapRand.get(3, "Plot Types - Pangaea PYTHON"), mapRand.get(64, "Plot Types - Pangaea PYTHON"))
		if (mapRand.get(2, "Plot Types - Pangaea PYTHON") == 0):
			hinted_world.setValue(2, 1 + mapRand.get(3, "Plot Types - Pangaea PYTHON"), mapRand.get(64, "Plot Types - Pangaea PYTHON"))
		else:
			hinted_world.setValue(5, 1 + mapRand.get(3, "Plot Types - Pangaea PYTHON"), mapRand.get(64, "Plot Types - Pangaea PYTHON"))

		hinted_world.buildAllContinents()
		return hinted_world.generatePlotTypes(shift_plot_types=True)

	def generateAndysHintedPangaea(self):
		NiTextOut("Setting Plot Types (Python Pangaea Hinted) ...")
		global hinted_world
		hinted_world = MGU.HintedWorld(16,8)

		mapRand = CyGame().getMapRand()

		numBlocks = hinted_world.w * hinted_world.h
		numBlocksLand = int(numBlocks*0.33)
		cont = hinted_world.addContinent(numBlocksLand,mapRand.get(5, "Generate Plot Types PYTHON")+4,mapRand.get(3, "Generate Plot Types PYTHON")+2)
		if not cont:
			# Couldn't create continent! Reverting to Soren's Hinted Pangaea
			return self.generateSorensHintedPangaea()
		else:
			for x in range(hinted_world.w):
				for y in (0, hinted_world.h - 1):
					hinted_world.setValue(x,y, 1) # force ocean at poles
			hinted_world.buildAllContinents()
			return hinted_world.generatePlotTypes(shift_plot_types=True)

class PangaeaMultilayeredFractal(MGU.MultilayeredFractal):
	# Subclass. Only the controlling function overridden in this case.
	def generatePlotsByRegion(self, pangaea_type):
		# Sirian's MultilayeredFractal class, controlling function.
		# You -MUST- customize this function for each use of the class.
		#
		# The following grain matrix is specific to Pangaea.py
		sizekey = self.map.getWorldSize()
		sizevalues = {
			WorldSizeTypes.WORLDSIZE_DUEL:	  3,
			WorldSizeTypes.WORLDSIZE_TINY:	  3,
			WorldSizeTypes.WORLDSIZE_SMALL:	 4,
			WorldSizeTypes.WORLDSIZE_STANDARD:  4,
			WorldSizeTypes.WORLDSIZE_LARGE:	 4,
			WorldSizeTypes.WORLDSIZE_HUGE:	  5,
			6: 5,
			7: 5
			}
		grain = sizevalues[sizekey]

		# Sea Level adjustment (from user input), limited to value of 5%.
		sea = CyGlobalContext().getSeaLevelInfo(self.map.getSeaLevel()).getSeaLevelChange()
		sea = min(sea, 5)
		sea = max(sea, -5)

		# The following regions are specific to Pangaea.py
		mainWestLon = 0.2
		mainEastLon = 0.8
		mainSouthLat = 0.2
		mainNorthLat = 0.8
		subcontinentDimension = 0.4
		bSouthwardShift = False

		# Define the three types.
		if pangaea_type == 2: # Pressed Polar
			numSubcontinents = 3
			# Place mainland near north or south pole?
			global polarShiftRoll
			polarShiftRoll = self.dice.get(2, "Shift - Pangaea PYTHON")
			if polarShiftRoll == 1:
				mainNorthLat += 0.175
				mainSouthLat += 0.175
				# Define potential subcontinent slots (regional definitions).
				# List values: [westLon, southLat, vertRange, horzRange, southShift]
				scValues = [[0.05, 0.375, 0.2, 0.0, 0],
							[0.55, 0.375, 0.2, 0.0, 0],
							[0.1, 0.225, 0.0, 0.15, 0],
							[0.3, 0.225, 0.0, 0.15, 0]
							]
			else:
				mainNorthLat -= 0.175
				mainSouthLat -= 0.175
				# List values: [westLon, southLat, vertRange, horzRange, southShift]
				scValues = [[0.05, 0.025, 0.2, 0.0, 0],
							[0.55, 0.025, 0.2, 0.0, 0],
							[0.1, 0.375, 0.0, 0.15, 0],
							[0.3, 0.375, 0.0, 0.15, 0]
							]
		elif pangaea_type == 1: # Pressed Equatorial
			# Define potential subcontinent slots (regional definitions).
			equRoll = self.dice.get(4, "Subcontinents - Pangaea PYTHON")
			if equRoll == 3: equRoll = 1 # 50% chance result = 1
			numSubcontinents = 2 + equRoll
			# List values: [westLon, southLat, vertRange, horzRange, southShift]
			scValues = [[0.05, 0.2, 0.2, 0.0, 0.0],
						[0.55, 0.2, 0.2, 0.0, 0.0],
						[0.2, 0.05, 0.0, 0.2, 0.0],
						[0.2, 0.55, 0.0, 0.2, 0.0]
						]
		else: # Natural
			subcontinentDimension = 0.3
			# Shift mainland north or south?
			global shiftRoll
			shiftRoll = self.dice.get(2, "Shift - Pangaea PYTHON")
			if shiftRoll == 1:
				mainNorthLat += 0.075
				mainSouthLat += 0.075
			else:
				mainNorthLat -= 0.075
				mainSouthLat -= 0.075
				bSouthwardShift = True
			# Define potential subcontinent slots (regional definitions).
			numSubcontinents = 4 + self.dice.get(3, "Subcontinents - Pangaea PYTHON")
			# List values: [westLon, southLat, vertRange, horzRange, southShift]
			scValues = [[0.05, 0.575, 0.0, 0.0, 0.15],
						[0.05, 0.275, 0.0, 0.0, 0.15],
						[0.2, 0.175, 0.0, 0.0, 0.15],
						[0.5, 0.175, 0.0, 0.0, 0.15],
						[0.65, 0.575, 0.0, 0.0, 0.15],
						[0.65, 0.275, 0.0, 0.0, 0.15],
						[0.2, 0.675, 0.0, 0.0, 0.15],
						[0.5, 0.675, 0.0, 0.0, 0.15]
						]

		# Generate the main land mass, first pass (to vary shape).
		mainWestX = int(self.iW * mainWestLon)
		mainEastX = int(self.iW * mainEastLon)
		mainNorthY = int(self.iH * mainNorthLat)
		mainSouthY = int(self.iH * mainSouthLat)
		mainWidth = mainEastX - mainWestX + 1
		mainHeight = mainNorthY - mainSouthY + 1

		mainWater = 55+sea

		self.generatePlotsInRegion(mainWater,
								   mainWidth, mainHeight,
								   mainWestX, mainSouthY,
								   2, grain,
								   self.iHorzFlags, self.iTerrainFlags,
								   -1, -1,
								   True, 15,
								   2, False,
								   False
								   )

		# Second pass (to ensure cohesion).
		second_layerHeight = mainHeight/2
		second_layerWestX = mainWestX + mainWidth/10
		second_layerEastX = mainEastX - mainWidth/10
		second_layerWidth = second_layerEastX - second_layerWestX + 1
		second_layerNorthY = mainNorthY - mainHeight/4
		second_layerSouthY = mainSouthY + mainHeight/4

		second_layerWater = 60+sea

		self.generatePlotsInRegion(second_layerWater,
								   second_layerWidth, second_layerHeight,
								   second_layerWestX, second_layerSouthY,
								   1, grain,
								   self.iHorzFlags, self.iTerrainFlags,
								   -1, -1,
								   True, 15,
								   2, False,
								   False
								   )

		# Add subcontinents.
		# Subcontinents can be akin to India/Alaska, Europe, or the East Indies.
		while numSubcontinents > 0:
			# Choose a slot for this subcontinent.
			if len(scValues) > 1:
				scIndex = self.dice.get(len(scValues), "Subcontinent Placement - Pangaea PYTHON")
			else:
				scIndex = 0
			[scWestLon, scSouthLat, scVertRange, scHorzRange, scSouthShift] = scValues[scIndex]
			scWidth = int(subcontinentDimension * self.iW)
			scHeight = int(subcontinentDimension * self.iH)
			scHorzShift = 0; scVertShift = 0
			if scHorzRange > 0.0:
				scHorzShift = self.dice.get(int(self.iW * scHorzRange), "Subcontinent Variance - Terra PYTHON")
			if scVertRange > 0.0:
				scVertShift = self.dice.get(int(self.iW * scVertRange), "Subcontinent Variance - Terra PYTHON")
			scWestX = int(self.iW * scWestLon) + scHorzShift
			scEastX = scWestX + scWidth
			if scEastX >= self.iW: # Trouble! Off the right hand edge!
				while scEastX >= self.iW:
					scWidth -= 1
					scEastX = scWestX + scWidth
			scSouthY = int(self.iH * scSouthLat) + scVertShift
			# Check for southward shift.
			if bSouthwardShift:
				scSouthY -= int(self.iH * scSouthShift)
			scNorthY = scSouthY + scHeight
			if scNorthY >= self.iH: # Trouble! Off the top edge!
				while scNorthY >= self.iH:
					scHeight -= 1
					scNorthY = scSouthY + scHeight

			scShape = self.dice.get(5, "Subcontinent Shape - Terra PYTHON")
			if scShape > 1: # Regular subcontinent.
				scWater = 55+sea; scGrain = 1; scRift = -1
			elif scShape == 1: # Irregular subcontinent.
				scWater = 66+sea; scGrain = 2; scRift = 2
			else: # scShape == 0, Archipelago subcontinent.
				scWater = 77+sea; scGrain = grain; scRift = -1

			self.generatePlotsInRegion(scWater,
									   scWidth, scHeight,
									   scWestX, scSouthY,
									   scGrain, grain,
									   self.iRoundFlags, self.iTerrainFlags,
									   6, 6,
									   True, 7,
									   scRift, False,
									   False
									   )

			del scValues[scIndex]
			numSubcontinents -= 1

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
bStrip, strip,
rift_grain, has_center_rift,
invert_heights
'''

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Pangaea) ...")
	global pangaea_type
	mapgen = CyMapGenerator()
	map = CyMap()
	dice = CyGame().getMapRand()
	hinted_world = PangaeaHintedWorld()
	fractal_world = PangaeaMultilayeredFractal()

	# Get user input.
	userInputLandmass = map.getCustomMapOption(0)

	# Implement Pangaea by Type
	if userInputLandmass == 3: # Solid
		# Roll for type selection.
		typeRoll = dice.get(3, "PlotGen Chooser - Pangaea PYTHON")
		# Solid Shoreline cohesion check and catch - patched Dec 30, 2005 - Sirian
		# Error catching for non-cohesive results.
		cohesive = False
		while not cohesive:
			plotTypes = []
			if typeRoll == 2:
				plotTypes = hinted_world.generateAndysHintedPangaea()
			else:
				plotTypes = hinted_world.generateSorensHintedPangaea()
			mapgen.setPlotTypes(plotTypes)
			biggest_area = map.findBiggestArea(false)
			iTotalLandPlots = map.getLandPlots()
			iBiggestAreaPlots = biggest_area.getNumTiles()
			if iBiggestAreaPlots >= 0.9 * iTotalLandPlots:
				cohesive = True
		return plotTypes

	elif userInputLandmass == 2: # Pressed
		# Roll for type selection.
		typeRoll = dice.get(3, "PlotGen Chooser - Pangaea PYTHON")
		if typeRoll == 1:
			pangaea_type = 1
		else:
			pangaea_type = 2
		return fractal_world.generatePlotsByRegion(pangaea_type)

	elif userInputLandmass == 1: # Natural
		pangaea_type = 0
		return fractal_world.generatePlotsByRegion(pangaea_type)

	else: # Random
		global terrainRoll
		terrainRoll = dice.get(10, "PlotGen Chooser - Pangaea PYTHON")
		# 0-3 = Natural
		# 4 = Pressed, Equatorial
		# 5,6 = Pressed, Polar
		# 7,8 = Solid, Irregular
		# 9 = Solid, Round

		if terrainRoll > 6:
			# Solid Shoreline cohesion check and catch - patched Dec 30, 2005 - Sirian
			cohesive = False
			while not cohesive:
				plotTypes = []
				if terrainRoll == 9:
					plotTypes = hinted_world.generateAndysHintedPangaea()
				else:
					plotTypes = hinted_world.generateSorensHintedPangaea()
				mapgen.setPlotTypes(plotTypes)
				biggest_area = map.findBiggestArea(false)
				iTotalLandPlots = map.getLandPlots()
				iBiggestAreaPlots = biggest_area.getNumTiles()
				if iBiggestAreaPlots >= 0.9 * iTotalLandPlots:
					cohesive = True
			return plotTypes

		elif terrainRoll == 5 or terrainRoll == 6:
			pangaea_type = 2
			return fractal_world.generatePlotsByRegion(pangaea_type)
		elif terrainRoll == 4:
			pangaea_type = 1
			return fractal_world.generatePlotsByRegion(pangaea_type)
		else:
			pangaea_type = 0
			return fractal_world.generatePlotsByRegion(pangaea_type)

def generateTerrainTypes():
	# Run a check for cohesion failure.
	# If the largest land area contains less than 80% of the land (Natural/Pressed),
	# or less than 90% of the land (Solid), add a third layer of fractal terrain
	# to try to link the main landmasses in to a true Pangaea.
	map = CyMap()
	dice = CyGame().getMapRand()
	iHorzFlags = CyFractal.FracVals.FRAC_WRAP_X + CyFractal.FracVals.FRAC_POLAR
	biggest_area = map.findBiggestArea(false)
	global terrainRoll
	userInputShoreline = map.getCustomMapOption(0)
	iTotalLandPlots = map.getLandPlots()
	iBiggestAreaPlots = biggest_area.getNumTiles()
	#print("Total Land: ", iTotalLandPlots, " Main Landmass Plots: ", iBiggestAreaPlots)
	if (userInputShoreline == 1 or userInputShoreline == 2 or (userInputShoreline == 0 and terrainRoll < 7)) and iBiggestAreaPlots < 0.8 * iTotalLandPlots:
		global pangaea_type
		print("Total Land: ", iTotalLandPlots, " Main Landmass Plots: ", iBiggestAreaPlots)
		print "Cohesion failure! Attempting to remedy..."
		#print("Pangaea Type: ", pangaea_type)
		iW = map.getGridWidth()
		iH = map.getGridHeight()
		iWestX = int(0.3 * iW)
		eastX = int(0.7 * iW)
		southLat = 0.4
		northLat = 0.6
		if pangaea_type == 0: # Natural
			global shiftRoll
			#print("Shift Roll: ", shiftRoll)
			if shiftRoll == 1:
				southLat += 0.075
				northLat += 0.075
			else:
				southLat -= 0.075
				northLat -= 0.075
		elif pangaea_type == 2: # Pressed Polar
			global polarShiftRoll
			#print("Polar Shift Roll: ", polarShiftRoll)
			if polarShiftRoll == 1:
				southLat += 0.175
				northLat += 0.175
			else:
				southLat -= 0.175
				northLat -= 0.175
		else: # Pressed Equatorial
			pass
		iSouthY = int(southLat * iH)
		northY = int(northLat * iH)
		iRegionWidth = eastX - iWestX + 1
		iRegionHeight = northY - iSouthY + 1

		# Init the plot types array and the regional fractals
		plotTypes = [] # reinit the array for each pass
		plotTypes = [PlotTypes.PLOT_OCEAN] * (iRegionWidth*iRegionHeight)
		regionContinentsFrac = CyFractal()
		regionHillsFrac = CyFractal()
		regionPeaksFrac = CyFractal()
		regionContinentsFrac.fracInit(iRegionWidth, iRegionHeight, 1, dice, iHorzFlags, 7, 5)
		regionHillsFrac.fracInit(iRegionWidth, iRegionHeight, 3, dice, iHorzFlags, 7, 5)
		regionPeaksFrac.fracInit(iRegionWidth, iRegionHeight, 4, dice, iHorzFlags, 7, 5)

		iWaterThreshold = regionContinentsFrac.getHeightFromPercent(40)
		iHillsBottom1 = regionHillsFrac.getHeightFromPercent(20)
		iHillsTop1 = regionHillsFrac.getHeightFromPercent(30)
		iHillsBottom2 = regionHillsFrac.getHeightFromPercent(70)
		iHillsTop2 = regionHillsFrac.getHeightFromPercent(80)
		iPeakThreshold = regionPeaksFrac.getHeightFromPercent(25)

		# Loop through the region's plots
		for x in range(iRegionWidth):
			for y in range(iRegionHeight):
				i = y*iRegionWidth + x
				val = regionContinentsFrac.getHeight(x,y)
				if val <= iWaterThreshold: pass
				else:
					hillVal = regionHillsFrac.getHeight(x,y)
					if ((hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2)):
						peakVal = regionPeaksFrac.getHeight(x,y)
						if (peakVal <= iPeakThreshold):
							plotTypes[i] = PlotTypes.PLOT_PEAK
						else:
							plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						plotTypes[i] = PlotTypes.PLOT_LAND

		for x in range(iRegionWidth):
			wholeworldX = x + iWestX
			for y in range(iRegionHeight):
				i = y*iRegionWidth + x
				if plotTypes[i] == PlotTypes.PLOT_OCEAN: continue # Not merging water!
				wholeworldY = y + iSouthY
				# print("Changing water plot at ", wholeworldX, wholeworldY, " to ", plotTypes[i])
				iWorld = wholeworldY*iW + wholeworldX
				pPlot = map.plotByIndex(iWorld)
				if pPlot.isWater():	# Only merging new land plots over old water plots.
					pPlot.setPlotType(plotTypes[i], true, true)

		# Smooth any graphical glitches these changes may have produced.
		map.recalculateAreas()

	# Now generate Terrain.
	NiTextOut("Generating Terrain (Python Pangaea) ...")
	terraingen = MGU.TerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

def addFeatures():
	NiTextOut("Adding Features (Python Pangaea) ...")
	featuregen = MGU.FeatureGenerator()
	featuregen.addFeatures()
	return 0

def findStartingPlot(argsList):
	[playerID] = argsList

	def isValid(playerID, x, y):
		global isTeamGame
		map = CyMap()
		pPlot = map.plot(x, y)

		if (pPlot.getArea() != map.findBiggestArea(False).getID()):
			return False

		if isTeamGame:
			pWaterArea = pPlot.waterArea()
			return pWaterArea is not None and not pWaterArea.isLake()
		else:
			return True

	return MGU.findStartingPlot(playerID, isValid)

def afterGeneration():
	MGU.placeC2CBonuses()

