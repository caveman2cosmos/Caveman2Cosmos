#
#	FILE:	 Archipelago.py
#	AUTHOR:  Bob Thomas (Sirian)
#	CONTRIB: Soren Johnson
#	PURPOSE: Global map script - Generates a world full of random islands.
#-----------------------------------------------------------------------------
#	Copyright (c) 2005 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#
from CvPythonExtensions import *
import CvMapGeneratorUtil as MGU


def getDescription():
	return "TXT_KEY_MAP_SCRIPT_ARCHIPELAGO_DESCR"

def isAdvancedMap():
	"This map should show up in simple mode"
	return 0

def getNumCustomMapOptions():
	return 3

def getNumHiddenCustomMapOptions():
	return 2

def getCustomMapOptionName(argsList):
	[iOption] = argsList
	option_names = {
		0:	"TXT_KEY_MAP_SCRIPT_LANDMASS_TYPE",
		1:	"TXT_KEY_MAP_WORLD_WRAP",
		2:  "TXT_KEY_CONCEPT_RESOURCES"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0:	3,
		1:	3,
		2:  2
		}
	return option_values[iOption]

def getCustomMapOptionDescAt(argsList):
	[iOption, iSelection] = argsList
	selection_names = {
		0:	{
			0: "TXT_KEY_MAP_SCRIPT_SNAKY_CONTINENTS",
			1: "TXT_KEY_MAP_SCRIPT_ARCHIPELAGO",
			2: "TXT_KEY_MAP_SCRIPT_TINY_ISLANDS"
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
		0:	1,
		1:	1,
		2:  0
		}
	return option_defaults[iOption]

def isRandomCustomMapOption(argsList):
	[iOption] = argsList
	option_random = {
		0:	true,
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
	if (CyMap().getCustomMapOption(2) == 1):
		MGU.BonusBalancer().normalizeAddExtras()
	CyPythonMgr().allowDefaultImpl()	# do the rest of the usual normalizeStartingPlots stuff, don't overrride

def addBonusType(argsList):
	[iBonusType] = argsList

	if CyMap().getCustomMapOption(2) == 1 and CyGlobalContext().getBonusInfo(iBonusType).getType() in MGU.BonusBalancer().resourcesToBalance:
		return None # don't place any of this bonus randomly

	CyPythonMgr().allowDefaultImpl() # pretend we didn't implement this method, and let C handle this bonus in the default way

class ArchipelagoFractalWorld(MGU.FractalWorld):
	def checkForOverrideDefaultUserInputVariances(self):
		# Overriding peak value to counterbalance not having any peaks along the coasts.
		extraPeaks = 1 + CyMap().getCustomMapOption(0)
		self.peakPercent = min(100, self.peakPercent + (15 * extraPeaks))
		self.peakPercent = max(0, self.peakPercent)
		# Note, the peaks along the coast are not removed until addFeatures()
		return

def generatePlotTypes():
	"Generates a very grainy world so we get lots of islands."
	map = CyMap()
	fractal_world = ArchipelagoFractalWorld()
	NiTextOut("Setting Plot Types (Python Archipelago) ...")

	# Get user input.
	userInputLandmass = map.getCustomMapOption(0)

	if userInputLandmass == 2: # Tiny Islands
		fractal_world.initFractal(continent_grain = 5, rift_grain = -1, has_center_rift = False, polar = True)
		return fractal_world.generatePlotTypes(grain_amount = 4)

	elif userInputLandmass == 0: # Snaky Continents
		fractal_world.initFractal(continent_grain = 3, rift_grain = -1, has_center_rift = False, polar = True)
		return fractal_world.generatePlotTypes(grain_amount = 4)

	else: # Archipelago
		fractal_world.initFractal(continent_grain = 4, rift_grain = -1, has_center_rift = False, polar = True)
		return fractal_world.generatePlotTypes(grain_amount = 4)

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Archipelago) ...")
	terraingen = MGU.TerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

def addFeatures():
	# Remove all peaks along the coasts, before adding Features, Bonuses, Goodies, etc.
	# The peaks were bisecting too many islands.
	map = CyMap()
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	for plotIndex in range(iW * iH):
		pPlot = map.plotByIndex(plotIndex)
		if pPlot.isPeak() and pPlot.isCoastal():
			# If a peak is along the coast, change to hills and recalc.
			pPlot.setPlotType(PlotTypes.PLOT_HILLS, true, true)

	# Now add Features.
	NiTextOut("Adding Features (Python Archipelago) ...")
	featuregen = MGU.FeatureGenerator()
	featuregen.addFeatures()
	return 0

def assignStartingPlots():
	# Custom start plot finder for Archipelago (high grain) maps.
	# Set up start plot data for all players then access later.
	gc = CyGlobalContext()
	map = CyMap()
	dice = gc.getGame().getMapRand()
	iW = map.getGridWidth()
	iH = map.getGridHeight()

	# Success flag. Set to false if regional assignment fails or is not to be used.
	global bSuccessFlag
	global start_plots
	bSuccessFlag = True

	# Check for Snaky Continents user option or invalid number of players. If found, use normal start plot finder!
	userInputLandmass = map.getCustomMapOption(0)
	iPlayers = gc.getGame().countCivPlayersEverAlive()
	if userInputLandmass == 0:
		CyPythonMgr().allowDefaultImpl()
		return
	if iPlayers < 2 or iPlayers > 18:
		bSuccessFlag = False
		CyPythonMgr().allowDefaultImpl()
		return

	# List of number of regions to be used, indexed by number of players.
	if userInputLandmass == 2: # Tiny Islands will have fewer "dud" regions.
		configs = [0, 3, 3, 3, 6, 6, 8, 8, 12, 12, 12, 15, 15, 15, 20, 20, 20, 20, 24]
	else: # Standard Archipelago needs to account for regions that may be duds.
		configs = [0, 3, 3, 6, 6, 8, 8, 12, 12, 15, 15, 15, 20, 20, 20, 24, 24, 24, 24]
	iNumRegions = configs[iPlayers]

	# Obtain the minimum crow-flies distance figures [minX, minY] for this map size and number of players.
	minimums = {3: [0.1, 0.2],
				6: [0.1, 0.125],
				8: [0.07, 0.125],
				12: [0.07, 0.1],
				15: [0.06, 0.1],
				20: [0.06, 0.06],
				24: [0.05, 0.05]}
	[minLon, minLat] = minimums[iNumRegions]
	minX = max(3, int(minLon * iW))
	minY = max(3, int(minLat * iH))
	#print "minimums", minX, minY, "-o-o-"

	# Templates are nested by keys: {NumRegions: {RegionID: [WestLon, EastLon, SouthLat, NorthLat]}}
	templates = {3: {0: [0.0, 0.333, 0.0, 1.0],
					 1: [0.333, 0.667, 0.0, 1.0],
					 2: [0.667, 1.0, 0.0, 1.0]},
				 6: {0: [0.0, 0.333, 0.0, 0.5],
					 1: [0.333, 0.667, 0.0, 0.5],
					 2: [0.667, 1.0, 0.0, 0.5],
					 3: [0.0, 0.333, 0.5, 1.0],
					 4: [0.333, 0.667, 0.5, 1.0],
					 5: [0.667, 1.0, 0.5, 1.0]},
				 8: {0: [0.0, 0.25, 0.0, 0.5],
					 1: [0.25, 0.5, 0.0, 0.5],
					 2: [0.5, 0.75, 0.0, 0.5],
					 3: [0.75, 1.0, 0.0, 0.5],
					 4: [0.0, 0.25, 0.5, 1.0],
					 5: [0.25, 0.5, 0.5, 1.0],
					 6: [0.5, 0.75, 0.5, 1.0],
					 7: [0.75, 1.0, 0.5, 1.0]},
				 12: {0: [0.0, 0.25, 0.0, 0.35],
					  1: [0.25, 0.5, 0.0, 0.35],
					  2: [0.5, 0.75, 0.0, 0.35],
					  3: [0.75, 1.0, 0.0, 0.35],
					  4: [0.0, 0.25, 0.35, 0.63],
					  5: [0.25, 0.5, 0.35, 0.63],
					  6: [0.5, 0.75, 0.35, 0.63],
					  7: [0.75, 1.0, 0.35, 0.63],
					  8: [0.0, 0.25, 0.63, 1.0],
					  9: [0.25, 0.5, 0.63, 1.0],
					  10: [0.5, 0.75, 0.63, 1.0],
					  11: [0.75, 1.0, 0.63, 1.0]},
				 15: {0: [0.0, 0.2, 0.0, 0.35],
					  1: [0.2, 0.4, 0.0, 0.35],
					  2: [0.4, 0.6, 0.0, 0.35],
					  3: [0.6, 0.8, 0.0, 0.35],
					  4: [0.8, 1.0, 0.0, 0.35],
					  5: [0.0, 0.2, 0.35, 0.63],
					  6: [0.2, 0.4, 0.35, 0.63],
					  7: [0.4, 0.6, 0.35, 0.63],
					  8: [0.6, 0.8, 0.35, 0.63],
					  9: [0.8, 1.0, 0.35, 0.63],
					  10: [0.0, 0.2, 0.63, 1.0],
					  11: [0.2, 0.4, 0.63, 1.0],
					  12: [0.4, 0.6, 0.63, 1.0],
					  13: [0.6, 0.8, 0.63, 1.0],
					  14: [0.8, 1.0, 0.63, 1.0]},
				 20: {0: [0.0, 0.2, 0.0, 0.3],
					  1: [0.2, 0.4, 0.0, 0.3],
					  2: [0.4, 0.6, 0.0, 0.3],
					  3: [0.6, 0.8, 0.0, 0.3],
					  4: [0.8, 1.0, 0.0, 0.3],
					  5: [0.0, 0.2, 0.3, 0.5],
					  6: [0.2, 0.4, 0.3, 0.5],
					  7: [0.4, 0.6, 0.3, 0.5],
					  8: [0.6, 0.8, 0.3, 0.5],
					  9: [0.8, 1.0, 0.3, 0.5],
					  10: [0.0, 0.2, 0.5, 0.7],
					  11: [0.2, 0.4, 0.5, 0.7],
					  12: [0.4, 0.6, 0.5, 0.7],
					  13: [0.6, 0.8, 0.5, 0.7],
					  14: [0.8, 1.0, 0.5, 0.7],
					  15: [0.0, 0.2, 0.7, 1.0],
					  16: [0.2, 0.4, 0.7, 1.0],
					  17: [0.4, 0.6, 0.7, 1.0],
					  18: [0.6, 0.8, 0.7, 1.0],
					  19: [0.8, 1.0, 0.7, 1.0]},
				 24: {0: [0.0, 0.167, 0.0, 0.3],
					  1: [0.167, 0.333, 0.0, 0.3],
					  2: [0.333, 0.5, 0.0, 0.3],
					  3: [0.5, 0.667, 0.0, 0.3],
					  4: [0.667, 0.833, 0.0, 0.3],
					  5: [0.833, 1.0, 0.0, 0.3],
					  6: [0.0, 0.167, 0.3, 0.5],
					  7: [0.167, 0.333, 0.3, 0.5],
					  8: [0.333, 0.5, 0.3, 0.5],
					  9: [0.5, 0.667, 0.3, 0.5],
					  10: [0.667, 0.833, 0.3, 0.5],
					  11: [0.833, 1.0, 0.3, 0.5],
					  12: [0.0, 0.167, 0.5, 0.7],
					  13: [0.167, 0.333, 0.5, 0.7],
					  14: [0.333, 0.5, 0.5, 0.7],
					  15: [0.5, 0.667, 0.5, 0.7],
					  16: [0.667, 0.833, 0.5, 0.7],
					  17: [0.833, 1.0, 0.5, 0.7],
					  18: [0.0, 0.167, 0.7, 1.0],
					  19: [0.167, 0.333, 0.7, 1.0],
					  20: [0.333, 0.5, 0.7, 1.0],
					  21: [0.5, 0.667, 0.7, 1.0],
					  22: [0.667, 0.833, 0.7, 1.0],
					  23: [0.833, 1.0, 0.7, 1.0]}
	}
	# End of template data.

	# region_data: [WestX, EastX, SouthY, NorthY,
	# numLandPlotsinRegion, numCoastalPlotsinRegion,
	# numOceanPlotsinRegion, iRegionNetYield,
	# iNumLandAreas, iNumPlotsinRegion]
	region_data = []
	region_best_areas = []
	region_yields = []
	sorting_regions = []
	for regionLoop in range(iNumRegions):
		# Region dimensions
		[iWestLon, iEastLon, iSouthLat, iNorthLat] = templates[iNumRegions][regionLoop]
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
		region_data.append([iWestX, iEastX, iSouthY, iNorthY,
							iNumLandPlots, iNumCoastalPlots,
							iNumOceanPlots, iRegionNetYield,
							iNumLandAreas, iNumPlotsinRegion])
		region_best_areas.append(best_areas)
		region_yields.append(iRegionNetYield)
		sorting_regions.append(iRegionNetYield)

	#print region_data
	#print "---"
	#print region_best_areas
	#print "+++"
	#print region_yields

	# Now sort the regions
	best_regions = []
	region_numbers = range(iNumRegions)
	#print "reg #s", region_numbers
	sorting_regions.sort()
	sorting_regions.reverse()
	#print "---"
	#print "sorted regions"
	#print sorting_regions
	#print "---"
	for regionTestLoop in range(iNumRegions):
		#print "region test", regionTestLoop
		for yieldLoop in range(len(region_numbers)):
			#print "yield loop", yieldLoop, region_yields[yieldLoop]
			if sorting_regions[regionTestLoop] == region_yields[yieldLoop]:
				#print "--"
				#print region_numbers[yieldLoop]
				#print "++"
				best_regions.append(region_numbers[yieldLoop])
				del region_numbers[yieldLoop]
				del region_yields[yieldLoop]
				#print region_numbers
				#print region_yields
				#print "--"
				break
			#print "x-x"
		#print "-x-"

	# Need to discard the worst regions and then reverse the region order.
	# Of the regions that will be used, the worst will be assigned first.
	#
	# This means the civ with the poorest region will get best pick of its
	# lands without MinDistance concerns. Richer regions will have to obey
	# MinDistances in regard to poorer regions already assigned. This instead
	# of giving the richest region pick of its lands and making poorer regions
	# even worse off by pushing them around with MinDistances.
	best_regions[iPlayers:] = []
	best_regions.reverse()
	#print "----"
	#print best_regions
	#print "----"

	# Obtain player numbers. (Account for possibility of Open slots!)
	player_list = []
	for plrCheckLoop in range(18):
		if gc.getPlayer(plrCheckLoop).isEverAlive():
			player_list.append(plrCheckLoop)
	#print "***"
	#print "Player ID#s", player_list
	#print "***"

	# Shuffle start points so that players are assigned regions at random.
	shuffledPlayers = []
	for playerLoopTwo in range(gc.getGame().countCivPlayersEverAlive()):
		iChoosePlayer = dice.get(len(player_list), "Shuffling Regions - Archipelago PYTHON")
		shuffledPlayers.append(player_list[iChoosePlayer])
		del player_list[iChoosePlayer]
	#print "Shuffled Player List:", shuffledPlayers
	#print "---"

	# Find the oceans. We want all civs to start along the coast of a salt water body.
	oceans = []
	for area in map.areas():
		if area.isWater() and not area.isLake():
			oceans.append(area)
	#print("Oceans: ", oceans)

	# Now assign the start plots!
	min_dist = []
	# Loop through players/regions.
	for assignLoop in range(iPlayers):
		playerID = shuffledPlayers[assignLoop]
		reg = best_regions[assignLoop]
		[westX, eastX, southY, northY] = region_data[reg][0:4]
		iNumAreas = region_data[reg][8]
		area_list = region_best_areas[reg]
		# print Data for debugging
		#print "-+-+-"
		#print iNumAreas
		#print region_data[reg][0:4]
		#print area_list
		#print "+-+-+"
		# Error Handling (if valid start plot not found, reduce MinDistance)
		iPass = 0
		while (true):
			iBestValue = 0
			pBestPlot = None
			# Loop through best areas in this region
			for areaLoop in range(iNumAreas):
				areaID = area_list[areaLoop]
				#print "!!!"
				player = gc.getPlayer(playerID)
				#print "-!-"
				player.AI_updateFoundValues(True)
				#print "!-!"
				iRange = player.startingPlotRange()
				#validFn = None
				# Loop through all plots in the region.
				for iX in range(westX, eastX + 1):
					for iY in range(southY, northY + 1):
						pPlot = map.plot(iX, iY)
						if pPlot.isWater(): continue
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
					#print "- - - - -"
					#print "player"
					#print playerID
					#print "Plot Coords"
					#print pBestPlot.getX()
					#print pBestPlot.getY()
					#print "Plot Index", sPlot
					#print "- - - - -"
					break # Valid start found, stop checking areas and plots.
				else: pass # This area too close to somebody, try the next area.

			# Check to see if a valid start was found in ANY areaID.
			if pBestPlot == None:
				print("player", playerID, "pass", iPass, "failed")
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

def findStartingPlot(argsList):
	# This function is only called for Snaky Continents (or if an entire region should fail to produce a valid start plot via the regional method).
	[playerID] = argsList

	# Check to see if a region failed. If so, try the default implementation. (The rest of this process could get stuck in an infinite loop, so don't risk it!)
	global bSuccessFlag
	if bSuccessFlag == False:
		CyPythonMgr().allowDefaultImpl()
		return

	# Identify the best land area available to this player.
	global areas, area_values, iBestArea
	iBestValue = 0
	iBestArea = -1
	areas = map.areas()

	for area in areas:
		if area.isWater(): continue # Don't want to start "in the drink"!
		iNumPlayersOnArea = area.getNumStartingPlots() + 1 # Number of players starting on the area, plus this player.

		iTileValue = area.calculateTotalBestNatureYield() + area.getNumRiverEdges() + 2 * area.countCoastalLand() + 3 * area.countNumUniqueBonusTypes()
		iValue = iTileValue / iNumPlayersOnArea
		if (iNumPlayersOnArea == 1):
			iValue *= 4; iValue /= 3
		if (iValue > iBestValue):
			iBestValue = iValue
			iBestArea = area.getID()

	# Ensure that starting plot is in chosen Area and is along the coast.
	def isValid(playerID, x, y):
		global iBestArea
		pPlot = CyMap().plot(x, y)
		if pPlot.getArea() != iBestArea:
			return False
		pWaterArea = pPlot.waterArea()
		return pWaterArea is not None and not pWaterArea.isLake()

	return MGU.findStartingPlot(playerID, isValid)

def normalizeRemovePeaks():
	return None

def afterGeneration():
	MGU.placeC2CBonuses()
