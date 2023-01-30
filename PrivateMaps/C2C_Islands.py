#
#	FILE:	 Islands.py
#	AUTHOR:  Bob Thomas (Sirian)
#	PURPOSE: Global map script - Generates a world full of random islands.
#-----------------------------------------------------------------------------
#	Copyright (c) 2005 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvMapGeneratorUtil as MGU

balancer = MGU.BonusBalancer()

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_ISLANDS_DESCR"

def isAdvancedMap():
	"This map should not show up in simple mode"
	return 1

def getNumCustomMapOptions():
	return 4

def getNumHiddenCustomMapOptions():
	return 2

def getCustomMapOptionName(argsList):
	[iOption] = argsList
	option_names = {
		0:	"TXT_KEY_MAP_SCRIPT_NUMBER_OF_LARGE_ISLANDS",
		1:	"TXT_KEY_MAP_SCRIPT_NUMBER_OF_TINY_ISLANDS",
		2:  "TXT_KEY_MAP_WORLD_WRAP",
		3:  "TXT_KEY_CONCEPT_RESOURCES"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0:	3,
		1:	4,
		2:  3,
		3:  2
		}
	return option_values[iOption]

def getCustomMapOptionDescAt(argsList):
	[iOption, iSelection] = argsList
	selection_names = {
		0:	{
			0: "TXT_KEY_MAP_SCRIPT_1_PER_PLAYER",
			1: "TXT_KEY_MAP_SCRIPT_EXTRAS",
			2: "TXT_KEY_MAP_SCRIPT_SEVERAL_EXTRAS"
			},
		1:	{
			0: "TXT_KEY_MAP_SCRIPT_NO_TINY_ISLANDS",
			1: "TXT_KEY_MAP_SCRIPT_FEW_TINY_ISLANDS",
			2: "TXT_KEY_MAP_SCRIPT_VARIOUS_TINY_ISLANDS",
			3: "TXT_KEY_MAP_SCRIPT_MANY_TINY_ISLANDS"
			},
		2:	{
			0: "TXT_KEY_MAP_WRAP_FLAT",
			1: "TXT_KEY_MAP_WRAP_CYLINDER",
			2: "TXT_KEY_MAP_WRAP_TOROID"
			},
		3:	{
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
		1:	2,
		2:  1,
		3:  0
		}
	return option_defaults[iOption]

def isRandomCustomMapOption(argsList):
	[iOption] = argsList
	option_random = {
		0:	true,
		1:	true,
		2:	false,
		3:  false
		}
	return option_random[iOption]

def getWrapX():
	map = CyMap()
	return (map.getCustomMapOption(2) == 1 or map.getCustomMapOption(2) == 2)

def getWrapY():
	map = CyMap()
	return (map.getCustomMapOption(2) == 2)

def normalizeAddExtras():
	if CyMap().getCustomMapOption(3) == 1:
		balancer.normalizeAddExtras()
	CyPythonMgr().allowDefaultImpl()	# do the rest of the usual normalizeStartingPlots stuff, don't overrride

def addBonusType(argsList):
	[iBonusType] = argsList

	if CyMap().getCustomMapOption(3) == 1 and CyGlobalContext().getBonusInfo(iBonusType).getType() in balancer.resourcesToBalance:
		return None # don't place any of this bonus randomly

	CyPythonMgr().allowDefaultImpl() # pretend we didn't implement this method, and let C handle this bonus in the default way

def beforeGeneration():
	global iNumRegions
	global iExtras
	global regions_in_use
	global remaining_regions
	gc = CyGlobalContext()
	map = CyMap()
	dice = gc.getGame().getMapRand()
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	userInputLargeIslands = map.getCustomMapOption(0)
	iPlayers = gc.getGame().countCivPlayersEverAlive()

	# Error catching.
	if iPlayers < 1 or iPlayers > 18:
		return None

	# Number of Large Islands: templates.
	if userInputLargeIslands == 0: # One per Player
		configs = [0, 4, 4, 4, 6, 8, 8, 12, 12, 12, 15, 15, 15, 15, 20, 20, 20, 24, 24]
		iExtras = 0
	elif userInputLargeIslands == 2: # Several Extras
		configs = [0, 4, 4, 6, 8, 8, 12, 12, 12, 15, 15, 15, 20, 20, 20, 24, 24, 24, 24]
		extras_min_list = [0, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 3, 4, 4, 4, 4]
		extras_max_list = [0, 3, 2, 3, 3, 3, 5, 5, 4, 6, 5, 4, 7, 7, 6, 8, 8, 7, 6]
		extras_min = extras_min_list[iPlayers]
		extras_max = extras_max_list[iPlayers]
		if extras_min == extras_max:
			iExtras = extras_min
		else:
			extras_range = dice.get((extras_max - extras_min + 1), "Extra Islands - Islands PYTHON")
			iExtras = extras_min + extras_range
	else:
		configs = [0, 4, 4, 6, 6, 8, 8, 12, 12, 12, 15, 15, 15, 15, 20, 20, 20, 24, 24]
		extras_min_list = [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 2, 2, 2]
		extras_max_list = [0, 2, 1, 2, 2, 2, 2, 3, 3, 2, 3, 3, 3, 2, 3, 4, 4, 4, 4]
		extras_min = extras_min_list[iPlayers]
		extras_max = extras_max_list[iPlayers]
		if extras_min == extras_max:
			iExtras = extras_min
		else:
			extras_range = dice.get((extras_max - extras_min + 1), "Extra Islands - Islands PYTHON")
			iExtras = extras_min + extras_range

	# Choose a "Large Islands" template.
	iNumRegions = configs[iPlayers]
	# Some regions may go unused. We need to track the ones that have been used.
	regions_in_use = []
	remaining_regions = []
	for loopy in range(iNumRegions): # I'm going LOOPY! Loopy, I tell you. ;)
		remaining_regions.append(loopy)

	# Templates are nested by keys: {NumRegions: {RegionID: [WestLon, EastLon, SouthLat, NorthLat]}}
	templates = {4: {0: [0.0, 0.5, 0.1, 0.45],
					 1: [0.5, 1.0, 0.1, 0.45],
					 2: [0.0, 0.5, 0.55, 0.9],
					 3: [0.5, 1.0, 0.55, 0.9]},
				 6: {0: [0.0, 0.333, 0.1, 0.45],
					 1: [0.333, 0.667, 0.1, 0.45],
					 2: [0.667, 1.0, 0.1, 0.45],
					 3: [0.0, 0.333, 0.55, 0.9],
					 4: [0.333, 0.667, 0.55, 0.9],
					 5: [0.667, 1.0, 0.55, 0.9]},
				 8: {0: [0.0, 0.25, 0.1, 0.45],
					 1: [0.25, 0.5, 0.1, 0.45],
					 2: [0.5, 0.75, 0.1, 0.45],
					 3: [0.75, 1.0, 0.1, 0.45],
					 4: [0.0, 0.25, 0.55, 0.9],
					 5: [0.25, 0.5, 0.55, 0.9],
					 6: [0.5, 0.75, 0.55, 0.9],
					 7: [0.75, 1.0, 0.55, 0.9]},
				 12: {0: [0.0, 0.25, 0.1, 0.35],
					  1: [0.25, 0.5, 0.1, 0.35],
					  2: [0.5, 0.75, 0.1, 0.35],
					  3: [0.75, 1.0, 0.1, 0.35],
					  4: [0.0, 0.25, 0.4, 0.6],
					  5: [0.25, 0.5, 0.4, 0.6],
					  6: [0.5, 0.75, 0.4, 0.6],
					  7: [0.75, 1.0, 0.4, 0.6],
					  8: [0.0, 0.25, 0.65, 0.9],
					  9: [0.25, 0.5, 0.65, 0.9],
					  10: [0.5, 0.75, 0.65, 0.9],
					  11: [0.75, 1.0, 0.65, 0.9]},
				 15: {0: [0.0, 0.2, 0.1, 0.35],
					  1: [0.2, 0.4, 0.1, 0.35],
					  2: [0.4, 0.6, 0.1, 0.35],
					  3: [0.6, 0.8, 0.1, 0.35],
					  4: [0.8, 1.0, 0.1, 0.35],
					  5: [0.0, 0.2, 0.4, 0.6],
					  6: [0.2, 0.4, 0.4, 0.6],
					  7: [0.4, 0.6, 0.4, 0.6],
					  8: [0.6, 0.8, 0.4, 0.6],
					  9: [0.8, 1.0, 0.4, 0.6],
					  10: [0.0, 0.2, 0.65, 0.9],
					  11: [0.2, 0.4, 0.65, 0.9],
					  12: [0.4, 0.6, 0.65, 0.9],
					  13: [0.6, 0.8, 0.65, 0.9],
					  14: [0.8, 1.0, 0.65, 0.9]},
				 20: {0: [0.0, 0.2, 0.1, 0.29],
					  1: [0.2, 0.4, 0.1, 0.29],
					  2: [0.4, 0.6, 0.1, 0.29],
					  3: [0.6, 0.8, 0.1, 0.29],
					  4: [0.8, 1.0, 0.1, 0.29],
					  5: [0.0, 0.2, 0.33, 0.48],
					  6: [0.2, 0.4, 0.33, 0.48],
					  7: [0.4, 0.6, 0.33, 0.48],
					  8: [0.6, 0.8, 0.33, 0.48],
					  9: [0.8, 1.0, 0.33, 0.48],
					  10: [0.0, 0.2, 0.52, 0.67],
					  11: [0.2, 0.4, 0.52, 0.67],
					  12: [0.4, 0.6, 0.52, 0.67],
					  13: [0.6, 0.8, 0.52, 0.67],
					  14: [0.8, 1.0, 0.52, 0.67],
					  15: [0.0, 0.2, 0.71, 0.9],
					  16: [0.2, 0.4, 0.71, 0.9],
					  17: [0.4, 0.6, 0.71, 0.9],
					  18: [0.6, 0.8, 0.71, 0.9],
					  19: [0.8, 1.0, 0.71, 0.9]},
				 24: {0: [0.0, 0.167, 0.1, 0.29],
					  1: [0.167, 0.333, 0.1, 0.29],
					  2: [0.333, 0.5, 0.1, 0.29],
					  3: [0.5, 0.667, 0.1, 0.29],
					  4: [0.667, 0.833, 0.1, 0.29],
					  5: [0.833, 1.0, 0.1, 0.29],
					  6: [0.0, 0.167, 0.33, 0.48],
					  7: [0.167, 0.333, 0.33, 0.48],
					  8: [0.333, 0.5, 0.33, 0.48],
					  9: [0.5, 0.667, 0.33, 0.48],
					  10: [0.667, 0.833, 0.33, 0.48],
					  11: [0.833, 1.0, 0.33, 0.48],
					  12: [0.0, 0.167, 0.52, 0.67],
					  13: [0.167, 0.333, 0.52, 0.67],
					  14: [0.333, 0.5, 0.52, 0.67],
					  15: [0.5, 0.667, 0.52, 0.67],
					  16: [0.667, 0.833, 0.52, 0.67],
					  17: [0.833, 1.0, 0.52, 0.67],
					  18: [0.0, 0.167, 0.71, 0.9],
					  19: [0.167, 0.333, 0.71, 0.9],
					  20: [0.333, 0.5, 0.71, 0.9],
					  21: [0.5, 0.667, 0.71, 0.9],
					  22: [0.667, 0.833, 0.71, 0.9],
					  23: [0.833, 1.0, 0.71, 0.9]}
	}
	# End of template data.

	# List region_coords: [WestLon, EastLon, SouthLat, NorthLat]
	global region_coords
	region_coords = templates[iNumRegions]

class IslandsMultilayeredFractal(MGU.MultilayeredFractal):
	def generatePlotsByRegion(self):
		# Sirian's MultilayeredFractal class, controlling function.
		# You -MUST- customize this function for each use of the class.
		iPlayers = self.GC.getGame().countCivPlayersEverAlive()
		userInputTinyIslands = self.map.getCustomMapOption(1)

		# Sea Level adjustment (from user input), limited to value of 5%.
		sea = self.GC.getSeaLevelInfo(self.map.getSeaLevel()).getSeaLevelChange()
		sea = min(sea, 5)
		sea = max(sea, -5)

		# Add the Tiny Islands first (if appropriate).
		if userInputTinyIslands == 0: pass # No tinies.
		else:
			tiny_one = [[-1, -1], [92, 6], [91, 5], [85, 5]]
			[tinyWater, tinyGrain] = tiny_one[userInputTinyIslands]
			self.generatePlotsInRegion(tinyWater,
									   self.iW, self.iH,
									   0, 0,
									   tinyGrain, 4,
									   self.iHorzFlags, self.iTerrainFlags,
									   7, 6,
									   True, 15,
									   -1, False,
									   False
									   )

		# Add the Large Islands (two fractals each to ensure cohesion).
		global iExtras
		global region_coords
		global regions_in_use
		global remaining_regions
		for region_loop in range(iPlayers + iExtras):
			# Choose an unused region in which to place a Large Island.
			region_roll = self.dice.get(len(remaining_regions), "Extra Islands - Islands PYTHON")
			thisRegion = remaining_regions[region_roll]
			regions_in_use.append(thisRegion)
			del remaining_regions[region_roll]

			# Region dimensions
			[fWestLon, fEastLon, fSouthLat, fNorthLat] = region_coords[thisRegion]
			iWestX = int(self.iW * fWestLon)
			iEastX = int(self.iW * fEastLon) - 1
			iSouthY = int(self.iH * fSouthLat)
			iNorthY = int(self.iH * fNorthLat) -1
			iWidth = iEastX - iWestX + 1
			iHeight = iNorthY - iSouthY + 1

			# Each island only takes up approximately 63% of the space in its region.
			# This space is further divided between land and water. (These islands are fairly small!)
			# Islands get different shapes and offsets to vary their appearance and placement.
			# Choose a pattern for this Large Island.
			thisIslandPattern = self.dice.get(4, "Island Pattern - Islands PYTHON")
			if thisIslandPattern == 1: # Square island, offset.
				iOffSetX = self.dice.get(int(iWidth * 0.2) + 1, "Island Offset - Islands PYTHON")
				iOffSetY = self.dice.get(int(iHeight * 0.2) + 1, "Island Offset - Islands PYTHON")
				regWestX = iWestX + iOffSetX
				regSouthY = iSouthY + iOffSetY
				regWidth = int(iWidth * 0.8)
				regHeight = int(iHeight * 0.8)
			elif thisIslandPattern == 2: # Tall island, offset.
				iOffSetX = self.dice.get(int(iWidth * 0.37) + 1, "Island Offset - Islands PYTHON")
				iOffSetY = 0
				regWestX = iWestX + iOffSetX
				regSouthY = iSouthY + iOffSetY
				regWidth = int(iWidth * 0.63)
				regHeight = iHeight
			elif thisIslandPattern == 3: # Wide island, offset.
				iOffSetX = 0
				iOffSetY = self.dice.get(int(iHeight * 0.37) + 1, "Island Offset - Islands PYTHON")
				regWestX = iWestX + iOffSetX
				regSouthY = iSouthY + iOffSetY
				regWidth = iWidth
				regHeight = int(iHeight * 0.63)
			else: # thisIslandPattern == 0, Square island, centered.
				iOffSetX = int(iWidth * 0.1)
				iOffSetY = int(iHeight * 0.1)
				regWestX = iWestX + iOffSetX
				regSouthY = iSouthY + iOffSetY
				regWidth = int(iWidth * 0.8)
				regHeight = int(iHeight * 0.8)

			# Vary the shoreline
			shore_grain = 1 + self.dice.get(3, "Random Shoreline Type - Islands PYTHON")

			self.generatePlotsInRegion(55 + sea,
									   regWidth, regHeight,
									   regWestX, regSouthY,
									   shore_grain, 4,
									   self.iRoundFlags, self.iTerrainFlags,
									   6, 6,
									   True, 3,
									   -1, False,
									   False
									   )

			# Core fractal to increase cohesion
			coreWestX = regWestX + int(regWidth * 0.25)
			coreEastX = coreWestX + int(regWidth * 0.5)
			coreSouthY = regSouthY + int(regHeight * 0.25)
			coreNorthY = coreSouthY + int(regHeight * 0.5)
			coreWidth = coreEastX - coreWestX + 1
			coreHeight = coreNorthY - coreSouthY + 1

			self.generatePlotsInRegion(65,
									   coreWidth, coreHeight,
									   coreWestX, coreSouthY,
									   1, 3,
									   self.iHorzFlags, self.iTerrainFlags,
									   5, 5,
									   True, 3,
									   -1, False,
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
	NiTextOut("Setting Plot Types (Python Islands) ...")
	iPlayers = CyGlobalContext().getGame().countCivPlayersEverAlive()

	# Check for valid number of players.
	if iPlayers > 0 and iPlayers < 19: pass
	else: # Error catching.
		fractal_world = MGU.FractalWorld()
		fractal_world.initFractal(polar = True)
		plotTypes = fractal_world.generatePlotTypes()
		return plotTypes

	fractal_world = IslandsMultilayeredFractal()
	plotTypes = fractal_world.generatePlotsByRegion()
	return plotTypes

def generateTerrainTypes():
	print "terrain"
	NiTextOut("Generating Terrain (Python Islands) ...")
	terraingen = MGU.TerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

def addFeatures():
	print "features"
	NiTextOut("Adding Features (Python Islands) ...")
	featuregen = MGU.FeatureGenerator()
	featuregen.addFeatures()
	return 0

def assignStartingPlots():
	# Custom start plot finder for Islands.
	global iNumRegions
	global iExtras
	global region_coords
	global regions_in_use
	gc = CyGlobalContext()
	map = CyMap()
	dice = gc.getGame().getMapRand()
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	iPlayers = gc.getGame().countCivPlayersEverAlive()

	# Error catching.
	if iPlayers < 1 or iPlayers > 18:
		CyPythonMgr().allowDefaultImpl()
		return

	# Obtain the minimum crow-flies distance figures [minX, minY] for this map size and number of players.
	minimums = {4: [0.15, 0.1],
				6: [0.1, 0.1],
				8: [0.07, 0.1],
				12: [0.07, 0.07],
				15: [0.06, 0.07],
				20: [0.06, 0.05],
				24: [0.05, 0.05]}
	[minLon, minLat] = minimums[iNumRegions]
	minX = max(3, int(minLon * iW))
	minY = max(3, int(minLat * iH))

	# region_data: [WestX, EastX, SouthY, NorthY,
	# numLandPlotsinRegion, numCoastalPlotsinRegion,
	# numOceanPlotsinRegion, iRegionNetYield,
	# iNumLandAreas, iNumPlotsinRegion]
	region_data = {}
	region_best_areas = {}
	region_yields = []
	sorting_regions = []
	for regionLoop in range(len(regions_in_use)):
		thisRegion = regions_in_use[regionLoop]
		# Region dimensions
		[iWestLon, iEastLon, iSouthLat, iNorthLat] = region_coords[thisRegion]
		iWestX = int(iW * iWestLon)
		iEastX = int(iW * iEastLon) - 1
		iSouthY = int(iH * iSouthLat)
		iNorthY = int(iH * iNorthLat) -1
		# Plot and Area info.
		iNumLandPlots = 0
		iNumCoastalPlots = 0
		iNumOceanPlots = 0
		iRegionNetYield = 0
		iNumLandAreas = 0
		iNumPlotsinRegion = 0
		land_areas = []
		land_area_plots = {}
		land_area_yield = {}
		# Cycle through all plots in the region.
		for x in range(iWestX, iEastX + 1):
			for y in range(iSouthY, iNorthY + 1):
				iNumPlotsinRegion += 1
				i = y * iW + x
				pPlot = map.plot(x, y)
				if pPlot.getBonusType(-1) != -1: # Count any bonus resource as added value
					iRegionNetYield += 2
				if pPlot.isWater(): # Water plot
					iFertileCheck = pPlot.calculateBestNatureYield(YieldTypes.YIELD_FOOD, TeamTypes.NO_TEAM)
					if iFertileCheck > 1: # If the plot has extra food, count it.
						iRegionNetYield += (2 * (iFertileCheck - 1))
					if pPlot.isCoastal(): # Coastal plot
						if pPlot.isFreshWater:
							iNumCoastalPlots += 1
							iRegionNetYield += 2
						else:
							iNumCoastalPlots += 1
							iRegionNetYield += 1
					else:
						iNumOceanPlots += 1
				else: # Land plot
					iNumLandPlots += 1
					iArea = pPlot.getArea()
					iPlotYield = pPlot.calculateTotalBestNatureYield(TeamTypes.NO_TEAM)
					iFertileCheck = pPlot.calculateBestNatureYield(YieldTypes.YIELD_FOOD, TeamTypes.NO_TEAM)
					if iFertileCheck > 1: # If the plot has extra food, count the extra as double value!
						iPlotYield += (iFertileCheck - 1)
					iRegionNetYield += iPlotYield
					if pPlot.isHills(): iRegionNetYield += 1 # Add a bonus point for Hills plots.
					if not iArea in land_areas: # This plot is the first detected in its AreaID.
						iNumLandAreas += 1
						land_areas.append(iArea)
						land_area_plots[iArea] = 1
						land_area_yield[iArea] = iPlotYield
					else: # This AreaID already known.
						land_area_plots[iArea] += 1
						land_area_yield[iArea] += iPlotYield
		# Sort areas, achieving a list of AreaIDs with best areas first.
		area_yields = land_area_yield.values()
		area_yields.sort()
		area_yields.reverse()
		best_areas = []
		for areaTestLoop in range(iNumLandAreas):
			for landLoop in range(len(land_areas)):
				if area_yields[areaTestLoop] == land_area_yield[land_areas[landLoop]]:
					best_areas.append(land_areas[landLoop])
					del land_areas[landLoop]
					break
		# Store infos to regional lists.
		region_data[thisRegion] = [iWestX, iEastX, iSouthY, iNorthY,
								   iNumLandPlots, iNumCoastalPlots,
								   iNumOceanPlots, iRegionNetYield,
								   iNumLandAreas, iNumPlotsinRegion]
		region_best_areas[thisRegion] = best_areas
		region_yields.append(iRegionNetYield)
		sorting_regions.append(iRegionNetYield)

	# Now sort the regions
	best_regions = []
	region_numbers = regions_in_use
	sorting_regions.sort()
	sorting_regions.reverse()
	for regionTestLoop in range(iNumRegions):
		for yieldLoop in range(len(region_numbers)):
			if sorting_regions[regionTestLoop] == region_yields[yieldLoop]:
				best_regions.append(region_numbers[yieldLoop])
				del region_numbers[yieldLoop]
				del region_yields[yieldLoop]
				break

	# Need to discard the worst regions and then reverse the region order.
	# Of the regions that will be used, the worst will be assigned first.
	if iExtras:
		best_regions[iPlayers:] = []
	best_regions.reverse()

	# Obtain player numbers. (Account for possibility of Open slots!)
	player_list = []
	for plrCheckLoop in range(18):
		if CyGlobalContext().getPlayer(plrCheckLoop).isEverAlive():
			player_list.append(plrCheckLoop)

	# Shuffle start points so that players are assigned regions at random.
	shuffledPlayers = []
	for playerLoopTwo in range(gc.getGame().countCivPlayersEverAlive()):
		iChoosePlayer = dice.get(len(player_list), "Shuffling Regions - Islands PYTHON")
		shuffledPlayers.append(player_list[iChoosePlayer])
		del player_list[iChoosePlayer]

	# Find the oceans. We want all civs to start along the coast of a salt water body.
	oceans = []
	for area in map.areas():
		if area.isWater() and not area.isLake():
			oceans.append(area)

	# Now assign the start plots!
	plot_assignments = {}
	min_dist = []
	# Loop through players/regions.
	for assignLoop in range(iPlayers):
		playerID = shuffledPlayers[assignLoop]
		reg = best_regions[assignLoop]
		[westX, eastX, southY, northY] = region_data[reg][0:4]
		iNumAreas = region_data[reg][8]
		area_list = region_best_areas[reg]
		# Print Data for debugging
		# Error Handling (if valid start plot not found, reduce MinDistance)
		while (true):
			iBestValue = 0
			pBestPlot = None
			# Loop through best areas in this region
			for areaLoop in range(iNumAreas):
				areaID = area_list[areaLoop]
				player = gc.getPlayer(playerID)
				player.AI_updateFoundValues(True)
				iRange = player.startingPlotRange()
				iPass = 0
				#validFn = None
				# Loop through all plots in the region.
				for iX in range(westX, eastX + 1):
					for iY in range(southY, northY + 1):
						pPlot = map.plot(iX, iY)
						if pPlot.isWater(): continue
						if not pPlot.isCoastal(): continue
						if areaID != pPlot.getArea(): continue
						#if validFn != None and not validFn(playerID, iX, iY): continue
						val = pPlot.getFoundValue(playerID)
						if val > iBestValue:

							for invalid in min_dist:
								[invalidX, invalidY] = invalid
								if abs(invalidX - iX) < minX and abs(invalidY - iY) < minY:
									break
							else:
								for ocean in oceans:
									if pPlot.isAdjacentToArea(ocean):
										break
								else: continue # Not valid unless adjacent to an ocean!

								for iI in range(gc.getMAX_PC_PLAYERS()):
									if iI != playerID and gc.getPlayer(iI).isAlive() and gc.getPlayer(iI).startingPlotWithinRange(pPlot, playerID, iRange, iPass):
										break
								else:
									iBestValue = val
									pBestPlot = pPlot

				if pBestPlot != None:
					min_dist.append([pBestPlot.getX(), pBestPlot.getY()])
					sPlot = map.plot(pBestPlot.getX(), pBestPlot.getY())
					plrID = gc.getPlayer(playerID)
					plrID.setStartingPlot(sPlot, true)
					break # Valid start found, stop checking areas and plots.
				else: pass # This area too close to somebody, try the next area.

			# Check to see if a valid start was found in ANY areaID.
			if pBestPlot == None:
				print "player", playerID, "pass", iPass, "failed"
				iPass += 1
				if iPass <= max(player.startingPlotRange() + eastX - westX, player.startingPlotRange() + northY - southY):
					continue
				else: # A region has failed to produce any valid starts!
					bSuccessFlag = False
					print "---"
					print "A region has failed"
					print "---"
					# Regional start plot assignment has failed. Reverting to default.
					CyPythonMgr().allowDefaultImpl()
					return
			else: break # This player has been assigned a start plot.

	# Successfully assigned start plots, continue back to C++
	return None

def normalizeRemovePeaks():
	return None

def afterGeneration():
	MGU.placeC2CBonuses()
