#
#	FILE:	 Inland_Sea.py
#	AUTHOR:  Bob Thomas (Sirian)
#	CONTRIB: Soren Johnson, Andy Szybalski
#	PURPOSE: Regional map script - Loosely simulates a Mediterranean type
#			 temperate zone with civs ringing a central sea.
#-----------------------------------------------------------------------------
#	Copyright (c) 2005 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#	   MODIFIED BY: Temudjin
#	   PURPOSE:	- add Marsh terrain
#				   - print stats of mod and map
#				   - much more ...
#	   DEPENDENCY: - needs MapScriptTools.py
#-----------------------------------------------------------------------------
#  1.10			  4.Feb.09  - works with 'Giant' map in FlavourMod by Jean Elcard
#	1.20	Temudjin 29.Mar.09  - utilizes mapTools( Ringworld2 )
#	1.30	Temudjin 15.July.10 - utilizes MapScriptTools
#							- add Team Start option
#							- allow all map-wrapping
#							- allow any number of civs
#							- allow more world sizes, if supported by mod
#							- add Marsh terrain, if supported by mod
#							- add Deep Ocean terrain, if supported by mod
#							- add Map Regions ( BigDent, BigBog, ElementalQuarter, LostIsle )
#							- add Map Features ( Kelp, HauntedLands, CrystalPlains )
#							- better balanced resources


from CvPythonExtensions import *
import CvMapGeneratorUtil as MGU

################################################################
## MapScriptTools by Temudjin
################################################################
import MapScriptTools as mst
balancer = mst.bonusBalancer

def getVersion():
	return "1.30"

def beforeGeneration():
	print "-- beforeGeneration()"
	# create evaluation string for getLatitude(x,y); vars can be x or y
	compGetLat = "0.56 * plotXY(x,y,0,0).getLatitude() + 6.3"
	# Create mapInfo string
	mapInfo = ""
	for opt in range( getNumCustomMapOptions() ):
		nam = getCustomMapOptionName( [opt] )
		sel = CyMap().getCustomMapOption( opt )
		txt = getCustomMapOptionDescAt( [opt,sel] )
		mapInfo += "%27s:   %s\n" % ( nam, txt )
	# Initialize MapScriptTools
	mst.getModInfo( getVersion(), compGetLat, mapInfo )
	# initialize bonus balancing
	balancer.initialize( CyMap().getCustomMapOption(1) == 1 )	# balance boni if desired, place missing boni, move minerals
	beforeGeneration2()														# call renamed script function

def generateTerrainTypes():
	print "-- generateTerrainTypes()"
	# Choose terrainGenerator
	terraingen = ISTerrainGenerator()
	# Generate terrain
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

def addRivers():
	print "-- addRivers()"
	# Generate DeepOcean-terrain if mod allows for it
	mst.deepOcean.buildDeepOcean()
	# Generate marsh-terrain
	mst.marshMaker.convertTerrain()
	# Build between 0..3 mountain-ranges.
	mst.mapRegions.buildBigDents()
	# Build between 0..3 bog-regions.
	mst.mapRegions.buildBigBogs()
	# Put rivers on the map.
	CyPythonMgr().allowDefaultImpl()

def addLakes():
	print "-- addLakes()"
	CyPythonMgr().allowDefaultImpl()

# shuffle starting-plots for teams
def normalizeStartingPlotLocations():
	print "-- normalizeStartingPlotLocations()"

	# build Lost Isle
	# - this region needs to be placed after starting-plots are first assigned
	mst.mapRegions.buildLostIsle(bAliens = mst.choose(33,True,False))

	if CyMap().getCustomMapOption(2) == 0:
		# by default civ places teams near to each other
		CyPythonMgr().allowDefaultImpl()
	elif CyMap().getCustomMapOption(2) == 1:
		# shuffle starting-plots to separate teams
		mst.teamStart.placeTeamsTogether( False, True )
	else:
		# randomize starting-plots to ignore teams
		mst.teamStart.placeTeamsTogether( True, True )

def normalizeAddRiver():
	print "-- normalizeAddRiver()"
	CyPythonMgr().allowDefaultImpl()

def normalizeAddLakes():
	print "-- normalizeAddLakes()"
	CyPythonMgr().allowDefaultImpl()

# prevent terrain changes on Mars
def normalizeRemoveBadTerrain():
	print "-- normalizeRemoveBadTerrain()"
	CyPythonMgr().allowDefaultImpl()

# prevent terrain changes on Mars
def normalizeAddGoodTerrain():
	print "-- normalizeAddGoodTerrain()"
	CyPythonMgr().allowDefaultImpl()

def normalizeAddExtras():
	print "-- normalizeAddExtras()"
	# Prettify map - connect some small adjacent lakes
	mst.mapPrettifier.connectifyLakes()
	# Sprout rivers from lakes.
	mst.riverMaker.buildRiversFromLake( None, 60, 1, 2 )
	# Balance boni, place missing boni, move minerals
	balancer.normalizeAddExtras()

#	normalizeAddExtras2()						# call renamed script function
	CyPythonMgr().allowDefaultImpl()

	# give extras to special regions
	mst.mapRegions.addRegionExtras()

	# Place special features on map
	mst.featurePlacer.placeKelp()
	mst.featurePlacer.placeHauntedLands()
	mst.featurePlacer.placeCrystalPlains()

	# Print maps and stats
	mst.mapPrint.buildAreaMap( False, "normalizeAddExtras()" )
	mst.mapPrint.buildRiverMap( False, "normalizeAddExtras()" )
	mst.mapStats.mapStatistics()

def minStartingDistanceModifier():
	if CyGlobalContext().getGame().countCivPlayersEverAlive() <= 18:
		return -95
	return -50

################################################################



hinted_world = None

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_INLAND_SEA_DESCR"

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
		0: "TXT_KEY_MAP_WORLD_WRAP",
		1: "TXT_KEY_CONCEPT_RESOURCES",
		2: "Team Start"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0:	4,
		1:	2,
		2:	3
		}
	return option_values[iOption]

def getCustomMapOptionDescAt(argsList):
	[iOption, iSelection] = argsList
	selection_names = {
		0:	{
			0: "Flat",
			1: "Cylindrical (X-Wraped)",
			2: "Tubular (Y-Wraped)",
			3: "Toroidal (XY-Wraped)"
			},
		1:	{
			0: "Standard",
			1: "Balanced"
			},
		2:	{
			0: "Team Neighbors",
			1: "Team Separated",
			2: "Randomly Placed"
			}
		}
	translated_text = unicode(CyTranslator().getText(selection_names[iOption][iSelection], ()))
	return translated_text

def getCustomMapOptionDefault(argsList):
	[iOption] = argsList
	option_defaults = {
		0:	0,
		1: 1,
		2: 0
		}
	return option_defaults[iOption]

def isRandomCustomMapOption(argsList):
	[iOption] = argsList
	option_random = {
		0:	False,
		1: False,
		2: True
		}
	return option_random[iOption]

def getWrapX():
	return CyMap().getCustomMapOption(0) in [1,3]

def getWrapY():
	return CyMap().getCustomMapOption(0) in [2,3]

def beforeGeneration2():
	"Set up global variables for start point templates"
	global templates
	global shuffledPlayers
	global iTemplateRoll
	GC = CyGlobalContext()
	GAME = GC.getGame()

	# List of number of template instances, indexed by number of players.
	configs = [0, 1, 6, 4, 3, 2, 2, 2, 4, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1]

	# Shuffle start points so that players are assigned templateIDs at random.
	iPlayers = GAME.countCivPlayersEverAlive()
	shuffledPlayers = [0]*iPlayers
	shuffleList(iPlayers, GAME.getMapRand(), shuffledPlayers)

	# Choose a Template to be used for this game.
	if iPlayers > 18:
		iTemplateRoll = 0
		coords = getStartPositions()
		cDict = {}
		for i in range( iPlayers ):
			cDict[i] = coords[i]
		templates = {}
		templates[(iPlayers, 0)] = cDict
		print "template: %3r" % (templates)
		return

	MAP = GC.getMap()
	iW = MAP.getGridWidth()
	iH = MAP.getGridHeight()

	iTemplateRoll = GAME.getMapRand().get(configs[iPlayers], "Template Selection - Inland Sea PYTHON")

	# Set variance for start plots according to map size vs number of players.
	map_size = MAP.getWorldSize()
	sizevalues = {
		WorldSizeTypes.WORLDSIZE_DUEL:		(2, 3),
		WorldSizeTypes.WORLDSIZE_TINY:		(2, 3),
		WorldSizeTypes.WORLDSIZE_SMALL:		(3, 4),
		WorldSizeTypes.WORLDSIZE_STANDARD:	(4, 7),
		WorldSizeTypes.WORLDSIZE_LARGE:		(5, 10),
		WorldSizeTypes.WORLDSIZE_HUGE:		(6, 15)
	}
#FlavourMod: Added by Jean Elcard 02/04/2009
	if hasattr(WorldSizeTypes, "WORLDSIZE_GIANT"):
		sizesvalues[WorldSizeTypes.WORLDSIZE_GIANT] = (7, 21)
#FlavourMod: End Add
	(threeVar, twoVar) = sizevalues[map_size]
	if iPlayers <= threeVar:
		fVar = 3
	elif iPlayers <= twoVar:
		fVar = 2
	else:
		fVar = 1

	# Templates are nested by keys: {(NumPlayers, TemplateID): {PlayerID: [X, Y, xVariance, yVariance]}}
	templates = {
		(1,0): {0: [0.5, 0.500, int(0.5 * iW), int(0.5 * iH)]},
		(2,0): {0: [0.1, 0.500, fVar, int(0.5 * iH)], 1: [0.9, 0.500, fVar, int(0.5 * iH)]},
		(2,1): {0: [0.5, 0.167, int(0.3 * iW), fVar], 1: [0.5, 0.833, int(0.3 * iW), fVar]},
		(2,2): {0: [0.3, 0.167, int(0.3 * iW), fVar], 1: [0.7, 0.833, int(0.3 * iW), fVar]},
		(2,3): {0: [0.7, 0.167, int(0.3 * iW), fVar], 1: [0.3, 0.833, int(0.3 * iW), fVar]},
		(2,4): {0: [0.2, 0.333, int(0.2 * iW), int(0.333 * iH)], 1: [0.8, 0.667, int(0.2 * iW), int(0.333 * iH)]},
		(2,5): {0: [0.8, 0.333, int(0.2 * iW), int(0.333 * iH)], 1: [0.2, 0.677, int(0.2 * iW), int(0.333 * iH)]},
		(3,0): {0: [0.1, 0.500, fVar, fVar], 1: [0.7, 0.167, fVar, fVar], 2: [0.7, 0.833, fVar, fVar]},
		(3,1): {0: [0.9, 0.500, fVar, fVar], 1: [0.3, 0.167, fVar, fVar], 2: [0.3, 0.833, fVar, fVar]},
		(3,2): {0: [0.5, 0.167, fVar, fVar], 1: [0.1, 0.833, fVar, fVar], 2: [0.9, 0.833, fVar, fVar]},
		(3,3): {0: [0.5, 0.833, fVar, fVar], 1: [0.1, 0.167, fVar, fVar], 2: [0.9, 0.167, fVar, fVar]},
		(4,0): {
			0: [0.1, 0.5, fVar, fVar], 1: [0.5, 0.167, fVar, fVar],
			2: [0.9, 0.5, fVar, fVar], 3: [0.5, 0.833, fVar, fVar]
		},
		(4,1): {
			0: [0.1, 0.167, fVar, fVar], 1: [0.7, 0.167, fVar, fVar],
			2: [0.9, 0.833, fVar, fVar], 3: [0.3, 0.833, fVar, fVar]
		},
				 (4,2): {0: [0.1, 0.833, fVar, fVar],
						 1: [0.7, 0.833, fVar, fVar],
						 2: [0.9, 0.167, fVar, fVar],
						 3: [0.3, 0.167, fVar, fVar]},
				 (5,0): {0: [0.5, 0.167, fVar, fVar],
						 1: [0.125, 0.333, fVar, fVar],
						 2: [0.25, 0.833, fVar, fVar],
						 3: [0.75, 0.833, fVar, fVar],
						 4: [0.875, 0.333, fVar, fVar]},
				 (5,1): {0: [0.5, 0.833, fVar, fVar],
						 1: [0.125, 0.667, fVar, fVar],
						 2: [0.25, 0.167, fVar, fVar],
						 3: [0.75, 0.167, fVar, fVar],
						 4: [0.875, 0.667, fVar, fVar]},
				 (6,0): {0: [0.1, 0.5, fVar, fVar],
						 1: [0.3, 0.167, fVar, fVar],
						 2: [0.7, 0.167, fVar, fVar],
						 3: [0.9, 0.5, fVar, fVar],
						 4: [0.7, 0.833, fVar, fVar],
						 5: [0.3, 0.833, fVar, fVar]},
				 (6,1): {0: [0.1, 0.167, fVar, fVar],
						 1: [0.5, 0.167, fVar, fVar],
						 2: [0.9, 0.167, fVar, fVar],
						 3: [0.9, 0.833, fVar, fVar],
						 4: [0.5, 0.833, fVar, fVar],
						 5: [0.1, 0.833, fVar, fVar]},
				 (7,0): {0: [0.1, 0.5, fVar, fVar],
						 1: [0.2, 0.125, fVar, fVar],
						 2: [0.6, 0.125, fVar, fVar],
						 3: [0.9, 0.25, fVar, fVar],
						 4: [0.9, 0.75, fVar, fVar],
						 5: [0.6, 0.875, fVar, fVar],
						 6: [0.2, 0.875, fVar, fVar]},
				 (7,1): {0: [0.9, 0.5, fVar, fVar],
						 1: [0.8, 0.125, fVar, fVar],
						 2: [0.4, 0.125, fVar, fVar],
						 3: [0.1, 0.25, fVar, fVar],
						 4: [0.1, 0.75, fVar, fVar],
						 5: [0.4, 0.875, fVar, fVar],
						 6: [0.8, 0.875, fVar, fVar]},
				 (8,0): {0: [0.583, 0.125, fVar, fVar],
						 1: [0.25, 0.125, fVar, fVar],
						 2: [0.083, 0.375, fVar, fVar],
						 3: [0.083, 0.875, fVar, fVar],
						 4: [0.417, 0.875, fVar, fVar],
						 5: [0.75, 0.875, fVar, fVar],
						 6: [0.917, 0.625, fVar, fVar],
						 7: [0.917, 0.125, fVar, fVar]},
				 (8,1): {0: [0.417, 0.125, fVar, fVar],
						 1: [0.083, 0.125, fVar, fVar],
						 2: [0.083, 0.625, fVar, fVar],
						 3: [0.25, 0.875, fVar, fVar],
						 4: [0.583, 0.875, fVar, fVar],
						 5: [0.917, 0.875, fVar, fVar],
						 6: [0.917, 0.375, fVar, fVar],
						 7: [0.75, 0.125, fVar, fVar]},
				 (8,2): {0: [0.1, 0.5, fVar, fVar],
						 1: [0.2, 0.125, fVar, fVar],
						 2: [0.5, 0.125, fVar, fVar],
						 3: [0.8, 0.125, fVar, fVar],
						 4: [0.9, 0.5, fVar, fVar],
						 5: [0.8, 0.875, fVar, fVar],
						 6: [0.5, 0.875, fVar, fVar],
						 7: [0.2, 0.875, fVar, fVar]},
				 (8,3): {0: [0.1, 0.75, fVar, fVar],
						 1: [0.1, 0.25, fVar, fVar],
						 2: [0.333, 0.125, fVar, fVar],
						 3: [0.667, 0.125, fVar, fVar],
						 4: [0.9, 0.25, fVar, fVar],
						 5: [0.9, 0.75, fVar, fVar],
						 6: [0.667, 0.875, fVar, fVar],
						 7: [0.333, 0.875, fVar, fVar]},
				 (9,0): {0: [0.833, 0.15, fVar, fVar],
						 1: [0.5, 0.15, fVar, fVar],
						 2: [0.167, 0.15, fVar, fVar],
						 3: [0.08, 0.412, fVar, fVar],
						 4: [0.08, 0.775, fVar, fVar],
						 5: [0.35, 0.85, fVar, fVar],
						 6: [0.65, 0.85, fVar, fVar],
						 7: [0.92, 0.775, fVar, fVar],
						 8: [0.92, 0.412, fVar, fVar]},
				 (9,1): {0: [0.833, 0.85, fVar, fVar],
						 1: [0.5, 0.85, fVar, fVar],
						 2: [0.167, 0.85, fVar, fVar],
						 3: [0.08, 0.588, fVar, fVar],
						 4: [0.08, 0.225, fVar, fVar],
						 5: [0.35, 0.15, fVar, fVar],
						 6: [0.65, 0.15, fVar, fVar],
						 7: [0.92, 0.225, fVar, fVar],
						 8: [0.92, 0.588, fVar, fVar]},
				 (10,0): {0: [0.875, 0.15, fVar, fVar],
						  1: [0.625, 0.15, fVar, fVar],
						  2: [0.375, 0.15, fVar, fVar],
						  3: [0.125, 0.15, fVar, fVar],
						  4: [0.08, 0.5, fVar, fVar],
						  5: [0.125, 0.85, fVar, fVar],
						  6: [0.375, 0.85, fVar, fVar],
						  7: [0.625, 0.85, fVar, fVar],
						  8: [0.875, 0.85, fVar, fVar],
						  9: [0.92, 0.5, fVar, fVar]},
				 (10,1): {0: [0.75, 0.15, fVar, fVar],
						  1: [0.5, 0.15, fVar, fVar],
						  2: [0.25, 0.15, fVar, fVar],
						  3: [0.08, 0.33, fVar, fVar],
						  4: [0.08, 0.67, fVar, fVar],
						  5: [0.25, 0.85, fVar, fVar],
						  6: [0.5, 0.85, fVar, fVar],
						  7: [0.75, 0.85, fVar, fVar],
						  8: [0.92, 0.67, fVar, fVar],
						  9: [0.92, 0.33, fVar, fVar]},
				 (11,0): {0: [0.875, 0.15, fVar, fVar],
						  1: [0.625, 0.15, fVar, fVar],
						  2: [0.375, 0.15, fVar, fVar],
						  3: [0.125, 0.15, fVar, fVar],
						  4: [0.08, 0.45, fVar, fVar],
						  5: [0.08, 0.75, fVar, fVar],
						  6: [0.28, 0.85, fVar, fVar],
						  7: [0.5, 0.85, fVar, fVar],
						  8: [0.72, 0.85, fVar, fVar],
						  9: [0.92, 0.75, fVar, fVar],
						  10: [0.92, 0.45, fVar, fVar]},
				 (11,1): {0: [0.875, 0.85, fVar, fVar],
						  1: [0.625, 0.85, fVar, fVar],
						  2: [0.375, 0.85, fVar, fVar],
						  3: [0.125, 0.85, fVar, fVar],
						  4: [0.08, 0.55, fVar, fVar],
						  5: [0.08, 0.25, fVar, fVar],
						  6: [0.28, 0.15, fVar, fVar],
						  7: [0.5, 0.15, fVar, fVar],
						  8: [0.72, 0.15, fVar, fVar],
						  9: [0.92, 0.25, fVar, fVar],
						  10: [0.92, 0.55, fVar, fVar]},
				 (12,0): {0: [0.7, 0.15, fVar, fVar],
						  1: [0.5, 0.15, fVar, fVar],
						  2: [0.3, 0.15, fVar, fVar],
						  3: [0.1, 0.15, fVar, fVar],
						  4: [0.08, 0.5, fVar, fVar],
						  5: [0.1, 0.85, fVar, fVar],
						  6: [0.3, 0.85, fVar, fVar],
						  7: [0.5, 0.85, fVar, fVar],
						  8: [0.7, 0.85, fVar, fVar],
						  9: [0.9, 0.85, fVar, fVar],
						  10: [0.92, 0.5, fVar, fVar],
						  11: [0.9, 0.15, fVar, fVar]},
				 (13,0): {0: [0.7, 0.125, fVar, fVar],
						  1: [0.5, 0.125, fVar, fVar],
						  2: [0.3, 0.125, fVar, fVar],
						  3: [0.1, 0.125, fVar, fVar],
						  4: [0.08, 0.425, fVar, fVar],
						  5: [0.08, 0.725, fVar, fVar],
						  6: [0.2, 0.875, fVar, fVar],
						  7: [0.4, 0.875, fVar, fVar],
						  8: [0.6, 0.875, fVar, fVar],
						  9: [0.8, 0.875, fVar, fVar],
						  10: [0.92, 0.725, fVar, fVar],
						  11: [0.92, 0.425, fVar, fVar],
						  12: [0.9, 0.125, fVar, fVar]},
				 (13,1): {0: [0.7, 0.875, fVar, fVar],
						  1: [0.5, 0.875, fVar, fVar],
						  2: [0.3, 0.875, fVar, fVar],
						  3: [0.1, 0.875, fVar, fVar],
						  4: [0.08, 0.575, fVar, fVar],
						  5: [0.08, 0.275, fVar, fVar],
						  6: [0.2, 0.125, fVar, fVar],
						  7: [0.4, 0.125, fVar, fVar],
						  8: [0.6, 0.125, fVar, fVar],
						  9: [0.8, 0.125, fVar, fVar],
						  10: [0.92, 0.275, fVar, fVar],
						  11: [0.92, 0.575, fVar, fVar],
						  12: [0.9, 0.875, fVar, fVar]},
				 (14,0): {0: [0.7, 0.125, fVar, fVar],
						  1: [0.5, 0.125, fVar, fVar],
						  2: [0.3, 0.125, fVar, fVar],
						  3: [0.1, 0.125, fVar, fVar],
						  4: [0.08, 0.375, fVar, fVar],
						  5: [0.08, 0.625, fVar, fVar],
						  6: [0.1, 0.875, fVar, fVar],
						  7: [0.3, 0.875, fVar, fVar],
						  8: [0.5, 0.875, fVar, fVar],
						  9: [0.7, 0.875, fVar, fVar],
						  10: [0.9, 0.875, fVar, fVar],
						  11: [0.92, 0.625, fVar, fVar],
						  12: [0.92, 0.375, fVar, fVar],
						  13: [0.9, 0.125, fVar, fVar]},
				 (15,0): {0: [0.583, 0.125, fVar, fVar],
						  1: [0.417, 0.125, fVar, fVar],
						  2: [0.25, 0.125, fVar, fVar],
						  3: [0.083, 0.125, fVar, fVar],
						  4: [0.083, 0.4, fVar, fVar],
						  5: [0.083, 0.65, fVar, fVar],
						  6: [0.1, 0.9, fVar, fVar],
						  7: [0.3, 0.875, fVar, fVar],
						  8: [0.5, 0.875, fVar, fVar],
						  9: [0.7, 0.875, fVar, fVar],
						  10: [0.9, 0.9, fVar, fVar],
						  11: [0.917, 0.65, fVar, fVar],
						  12: [0.917, 0.4, fVar, fVar],
						  13: [0.917, 0.125, fVar, fVar],
						  14: [0.75, 0.125, fVar, fVar]},
				 (15,1): {0: [0.583, 0.875, fVar, fVar],
						  1: [0.417, 0.875, fVar, fVar],
						  2: [0.25, 0.875, fVar, fVar],
						  3: [0.083, 0.875, fVar, fVar],
						  4: [0.083, 0.6, fVar, fVar],
						  5: [0.083, 0.35, fVar, fVar],
						  6: [0.1, 0.1, fVar, fVar],
						  7: [0.3, 0.125, fVar, fVar],
						  8: [0.5, 0.125, fVar, fVar],
						  9: [0.7, 0.125, fVar, fVar],
						  10: [0.9, 0.1, fVar, fVar],
						  11: [0.917, 0.35, fVar, fVar],
						  12: [0.917, 0.6, fVar, fVar],
						  13: [0.917, 0.875, fVar, fVar],
						  14: [0.75, 0.875, fVar, fVar]},
				 (16,0): {0: [0.583, 0.125, fVar, fVar],
						  1: [0.417, 0.125, fVar, fVar],
						  2: [0.25, 0.125, fVar, fVar],
						  3: [0.083, 0.125, fVar, fVar],
						  4: [0.083, 0.375, fVar, fVar],
						  5: [0.083, 0.625, fVar, fVar],
						  6: [0.083, 0.875, fVar, fVar],
						  7: [0.25, 0.875, fVar, fVar],
						  8: [0.417, 0.875, fVar, fVar],
						  9: [0.583, 0.875, fVar, fVar],
						  10: [0.75, 0.875, fVar, fVar],
						  11: [0.917, 0.875, fVar, fVar],
						  12: [0.917, 0.625, fVar, fVar],
						  13: [0.917, 0.375, fVar, fVar],
						  14: [0.917, 0.125, fVar, fVar],
						  15: [0.75, 0.125, fVar, fVar]},
				 (17,0): {0: [0.5, 0.125, fVar, fVar],
						  1: [0.35, 0.125, fVar, fVar],
						  2: [0.2, 0.125, fVar, fVar],
						  3: [0.05, 0.175, fVar, fVar],
						  4: [0.083, 0.45, fVar, fVar],
						  5: [0.083, 0.7, fVar, fVar],
						  6: [0.083, 0.95, fVar, fVar],
						  7: [0.25, 0.875, fVar, fVar],
						  8: [0.417, 0.875, fVar, fVar],
						  9: [0.583, 0.875, fVar, fVar],
						  10: [0.75, 0.875, fVar, fVar],
						  11: [0.917, 0.95, fVar, fVar],
						  12: [0.917, 0.7, fVar, fVar],
						  13: [0.917, 0.45, fVar, fVar],
						  14: [0.95, 0.175, fVar, fVar],
						  15: [0.8, 0.125, fVar, fVar],
						  16: [0.65, 0.125, fVar, fVar]},
				 (17,1): {0: [0.5, 0.875, fVar, fVar],
						  1: [0.35, 0.875, fVar, fVar],
						  2: [0.2, 0.875, fVar, fVar],
						  3: [0.05, 0.825, fVar, fVar],
						  4: [0.083, 0.65, fVar, fVar],
						  5: [0.083, 0.3, fVar, fVar],
						  6: [0.083, 0.05, fVar, fVar],
						  7: [0.25, 0.125, fVar, fVar],
						  8: [0.417, 0.125, fVar, fVar],
						  9: [0.583, 0.125, fVar, fVar],
						  10: [0.75, 0.125, fVar, fVar],
						  11: [0.917, 0.05, fVar, fVar],
						  12: [0.917, 0.3, fVar, fVar],
						  13: [0.917, 0.65, fVar, fVar],
						  14: [0.95, 0.825, fVar, fVar],
						  15: [0.8, 0.875, fVar, fVar],
						  16: [0.65, 0.875, fVar, fVar]},
				 (18,0): {0: [0.5, 0.125, fVar, fVar],
						  1: [0.35, 0.125, fVar, fVar],
						  2: [0.2, 0.125, fVar, fVar],
						  3: [0.05, 0.125, fVar, fVar],
						  4: [0.075, 0.375, fVar, fVar],
						  5: [0.075, 0.625, fVar, fVar],
						  6: [0.05, 0.875, fVar, fVar],
						  7: [0.2, 0.875, fVar, fVar],
						  8: [0.35, 0.875, fVar, fVar],
						  9: [0.5, 0.875, fVar, fVar],
						  10: [0.65, 0.875, fVar, fVar],
						  11: [0.8, 0.875, fVar, fVar],
						  12: [0.95, 0.875, fVar, fVar],
						  13: [0.925, 0.625, fVar, fVar],
						  14: [0.925, 0.375, fVar, fVar],
						  15: [0.95, 0.125, fVar, fVar],
						  16: [0.8, 0.125, fVar, fVar],
						  17: [0.65, 0.125, fVar, fVar]}
	}
	# End of Templates data.


########## Temudjin START
def getStartPositions():
	gc = CyGlobalContext()
	iPlayers = gc.getGame().countCivPlayersEverAlive()
	coord = []
	for i in range( iPlayers + iPlayers%2 ): coord.append( [] )

	if iPlayers >10: yAdd = 2
	elif iPlayers > 24: yAdd = 3
	elif iPlayers > 40: yAdd = 4
	else: yAdd = 0

	pl = (iPlayers - yAdd + 1) / 2
	for i in range( pl ):
		dx1 = -0.025
		dx2 = +0.025
		if i>0 and i<(pl-1) and ((i%2)==0):
			dx1 = -0.05
			dx2 = +0.05
		elif i>0 and i<(pl-1) and ((i%2)==1):
			dx1 = 0
			dx2 = 0
		coord[2*i] = [ 0.05 + (0.9 * i) / (pl-1), 0.15 + dx1, 2, 3 ]
		coord[2*i+1] = [ 0.05 + (0.9 * i) / (pl-1), 0.85 + dx2, 2, 3 ]




	for j in range ( yAdd ):
		dy = j * 0.5 / yAdd + 0.25 + 0.25 / yAdd
		coord[2*pl+2*j] = [ 0.075, dy, 3, 2 ]
		coord[2*pl+2*j+1] = [ 0.925, dy, 3, 2 ]



	if len(coord) > iPlayers:
		inx = mst.chooseListIndex( coord )
		del coord[ inx ]
	print "IniCoords: %3r" % (coord)
	return coord
########### Temudjin END

def findStartingPlot(argsList):
	[playerID] = argsList
	global plotSuccess
	global plotValue

	def isValid(playerID, x, y):
		GC = CyGlobalContext()
		MAP = GC.getMap()
		pPlot = MAP.plot(x, y)
		iW = MAP.getGridWidth()
		iH = MAP.getGridHeight()
		iPlayers = GC.getGame().countCivPlayersEverAlive()

		# Use global data set up via beforeGeneration().
		global templates
		global shuffledPlayers
		global iTemplateRoll
		playerTemplateAssignment = shuffledPlayers[playerID]
		[fLat, fLon, varX, varY] = templates[(iPlayers, iTemplateRoll)][playerTemplateAssignment]

		# Check to ensure the plot is on the main landmass.
		if pPlot.getArea() != MAP.findBiggestArea(False).getID():
			return false

		# Now check for eligibility according to the defintions found in the template.
		iX = int(iW * fLat)
		iY = int(iH * fLon)
		westX = max(2, iX - varX)
		eastX = min(iW - 3, iX + varX)
		southY = max(2, iY - varY)
		northY = min(iH - 3, iY + varY)
		return x >= westX and x <= eastX and y >= southY and y <= northY

	getStartingPlot(playerID, isValid)
	if plotSuccess:
		return plotValue
	print "--> Default StartingPlots"
	CyPythonMgr().allowDefaultImpl()



def getStartingPlot(playerID, validFn = None):
	gc = CyGlobalContext()
	map = CyMap()
	player = gc.getPlayer(playerID)
	global plotSuccess
	global plotValue
	plotSuccess = false
	plotValue = -1

	player.AI_updateFoundValues(True)

	iRange = player.startingPlotRange()
	iPass = 0

	while (iPass < 50):
		iBestValue = 0
		pBestPlot = None

		for iX in range(map.getGridWidth()):
			for iY in range(map.getGridHeight()):
				if validFn != None and not validFn(playerID, iX, iY):
					continue
				pLoopPlot = map.plot(iX, iY)

				val = pLoopPlot.getFoundValue(playerID)

				if val > iBestValue:

					for iI in range(gc.getMAX_PC_PLAYERS()):
						if iI != playerID and gc.getPlayer(iI).isAlive() and gc.getPlayer(iI).startingPlotWithinRange(pLoopPlot, playerID, iRange, iPass):
							break
					else:
						iBestValue = val
						pBestPlot = pLoopPlot

		if pBestPlot != None:
			plotSuccess = true
			plotValue = map.plotNum(pBestPlot.getX(), pBestPlot.getY())
			break

		print "player", playerID, "pass", iPass, "failed"

		iPass += 1

	return -1

def getTopLatitude():
	return 60
def getBottomLatitude():
	return -60

########## Temudjin START
def getGridSize(argsList):
	if (argsList[0] == -1): return []			# (-1,) is passed to function on loads

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

	[eWorldSize] = argsList
	return grid_sizes[eWorldSize]
########## Temudjin END

# Subclasses to fix the FRAC_POLAR zero row bugs.
class ISFractalWorld(MGU.FractalWorld):
	def generatePlotTypes(self, water_percent=78, shift_plot_types=True,
						  grain_amount=3):
		# Check for changes to User Input variances.
		self.checkForOverrideDefaultUserInputVariances()

		self.hillsFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, grain_amount, self.mapRand, 0, self.fracXExp, self.fracYExp)
		self.peaksFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, grain_amount+1, self.mapRand, 0, self.fracXExp, self.fracYExp)

		water_percent += self.seaLevelChange
		water_percent = min(water_percent, self.seaLevelMax)
		water_percent = max(water_percent, self.seaLevelMin)

		iWaterThreshold = self.continentsFrac.getHeightFromPercent(water_percent)
		iHillsBottom1 = self.hillsFrac.getHeightFromPercent(max((self.hillGroupOneBase - self.hillGroupOneRange), 0))
		iHillsTop1 = self.hillsFrac.getHeightFromPercent(min((self.hillGroupOneBase + self.hillGroupOneRange), 100))
		iHillsBottom2 = self.hillsFrac.getHeightFromPercent(max((self.hillGroupTwoBase - self.hillGroupTwoRange), 0))
		iHillsTop2 = self.hillsFrac.getHeightFromPercent(min((self.hillGroupTwoBase + self.hillGroupTwoRange), 100))
		iPeakThreshold = self.peaksFrac.getHeightFromPercent(self.peakPercent)

		for x in range(self.iNumPlotsX):
			for y in range(self.iNumPlotsY):
				i = y*self.iNumPlotsX + x
				val = self.continentsFrac.getHeight(x,y)
				if val <= iWaterThreshold:
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					hillVal = self.hillsFrac.getHeight(x,y)
					if ((hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2)):
						peakVal = self.peaksFrac.getHeight(x,y)
						if (peakVal <= iPeakThreshold):
							self.plotTypes[i] = PlotTypes.PLOT_PEAK
						else:
							self.plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						self.plotTypes[i] = PlotTypes.PLOT_LAND

		if shift_plot_types:
			self.shiftPlotTypes()

		return self.plotTypes

class ISHintedWorld(MGU.HintedWorld, ISFractalWorld):

	def generatePlotTypes(self, water_percent=-1, shift_plot_types=False):
		for i in range(len(self.data)):
			if self.data[i] == None:
				self.data[i] = self.mapRand.get(48, "Generate Plot Types PYTHON")

		self.shiftHintsToMap()

		# don't call base method, this overrides it.
		size = len(self.data)
		minExp = min(self.fracXExp, self.fracYExp)
		iGrain = None
		for i in range(minExp):
			width = (1 << (self.fracXExp - minExp + i))
			height = (1 << (self.fracYExp - minExp + i))
			if not self.iFlags & CyFractal.FracVals.FRAC_WRAP_X:
				width += 1
			if not self.iFlags & CyFractal.FracVals.FRAC_WRAP_Y:
				height += 1
			if size == width*height:
				iGrain = i
		assert(iGrain != None)
		iFlags = self.map.getMapFractalFlags()
		self.continentsFrac.fracInitHints(self.iNumPlotsX, self.iNumPlotsY, iGrain, self.mapRand, iFlags, self.data, self.fracXExp, self.fracYExp)

		if (water_percent == -1):
			numPlots = len(self.data)
			numWaterPlots = 0
			for val in self.data:
				if val < 192:
					numWaterPlots += 1
			water_percent = int(100*numWaterPlots/numPlots)

		# Call superclass
		return ISFractalWorld.generatePlotTypes(self, water_percent, shift_plot_types)

def generatePlotTypes():
	global hinted_world
	gc = CyGlobalContext()
	map = CyMap()
	mapRand = gc.getGame().getMapRand()

	NiTextOut("Setting Plot Types (Python Inland Sea) ...")

	hinted_world = ISHintedWorld(4,2)
	area = hinted_world.w * hinted_world.h

	for y in range(hinted_world.h):
		for x in range(hinted_world.w):
			if x in (0, hinted_world.w-1) or y in (0, hinted_world.h-1):
				hinted_world.setValue(x, y, 200 + mapRand.get(55, "Plot Types - Inland Sea PYTHON"))
			else:
				hinted_world.setValue(x, y, 0)

	hinted_world.buildAllContinents()
	return hinted_world.generatePlotTypes()

# subclass TerrainGenerator to eliminate arctic, equatorial latitudes
class ISTerrainGenerator(MGU.TerrainGenerator):
	def getLatitudeAtPlot(self, iX, iY):
		"returns 0.0 for tropical, up to 1.0 for polar"
		lat = MGU.TerrainGenerator.getLatitudeAtPlot(self, iX, iY) 	# range [0,1]
		lat = 0.07 + 0.56*lat				# range [0.07, 0.57]
		return lat

# subclass FeatureGenerator to eliminate arctic, equatorial latitudes
class ISFeatureGenerator(MGU.FeatureGenerator):
	def getLatitudeAtPlot(self, iX, iY):
		"returns 0.0 for tropical, up to 1.0 for polar"
		lat = MGU.FeatureGenerator.getLatitudeAtPlot(self, iX, iY) 	# range [0,1]
		lat = 0.07 + 0.56*lat				# range [0.07, 0.57]
		return lat

def addFeatures():
	NiTextOut("Adding Features (Python Inland Sea) ...")
	featuregen = ISFeatureGenerator()
	featuregen.addFeatures()
	return 0

def getRiverStartCardinalDirection(argsList):
	pPlot = argsList[0]
	map = CyMap()

	if (pPlot.getY() > ((map.getGridHeight() * 2) / 3)):
		return CardinalDirectionTypes.CARDINALDIRECTION_SOUTH

	if (pPlot.getY() < (map.getGridHeight() / 3)):
		return CardinalDirectionTypes.CARDINALDIRECTION_NORTH

	if (pPlot.getX() > (map.getGridWidth() / 2)):
		return CardinalDirectionTypes.CARDINALDIRECTION_WEST

	return CardinalDirectionTypes.CARDINALDIRECTION_EAST

def getRiverAltitude(argsList):
	pPlot = argsList[0]
	map = CyMap()

	CyPythonMgr().allowDefaultImpl()

	return ((abs(pPlot.getX() - (map.getGridWidth() / 2)) + abs(pPlot.getY() - (map.getGridHeight() / 2))) * 20)

def afterGeneration():
	MGU.placeC2CBonuses()
