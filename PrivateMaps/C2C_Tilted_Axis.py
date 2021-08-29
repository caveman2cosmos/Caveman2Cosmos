#
#	FILE:	 Tilted_Axis.py
#	AUTHOR:  Bob Thomas (Sirian)
#	PURPOSE: Global map script - Simulates a world with its rotational axis
#			 tipped over on to its side. This is also a square map.
#-----------------------------------------------------------------------------
#	Copyright (c) 2005 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvMapGeneratorUtil
#from CvMapGeneratorUtil import FractalWorld
#from CvMapGeneratorUtil import TerrainGenerator
#from CvMapGeneratorUtil import FeatureGenerator
#from CvMapGeneratorUtil import BonusBalancer

#balancer = BonusBalancer()

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_TILTED_AXIS_DESCR"

def getNumCustomMapOptions():
	return 2

def getNumHiddenCustomMapOptions():
	return 1

def getCustomMapOptionName(argsList):
	[iOption] = argsList
	option_names = {
		0:	"TXT_KEY_MAP_SCRIPT_LANDMASS_SIZE",
		1:  "TXT_KEY_CONCEPT_RESOURCES"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text


def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0:	5,
		1:	2
		}
	return option_values[iOption]

def getCustomMapOptionDescAt(argsList):
	[iOption, iSelection] = argsList
	selection_names = {
		0:	{
			0: "TXT_KEY_MAP_SCRIPT_MASSIVE_CONTINENTS",
			1: "TXT_KEY_MAP_SCRIPT_NORMAL_CONTINENTS",
			2: "TXT_KEY_MAP_SCRIPT_SMALL_CONTINENTS",
			3: "TXT_KEY_MAP_SCRIPT_ISLANDS",
			4: "TXT_KEY_MAP_SCRIPT_TINY_ISLANDS"
			},
		1:	{
			0: "TXT_KEY_WORLD_STANDARD",
			1: "TXT_KEY_MAP_BALANCED"
			}
		}
	translated_text = unicode(CyTranslator().getText(selection_names[iOption][iSelection], ()))
	return translated_text

def getCustomMapOptionDefault(argsList):
	[iOption] = argsList
	option_defaults = {
		0:	-1,
		1:  0
		}
	return option_defaults[iOption]

def isRandomCustomMapOption(argsList):
	[iOption] = argsList
	option_random = {
		0:	true,
		1:  false
		}
	return option_random[iOption]

def normalizeAddExtras():
	if (CyMap().getCustomMapOption(1) == 1):
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
	## Mini-Map Aspect Ratio kept 1, area should be similar to other map scripts. Average of standard X/Y grid size.
		WorldSizeTypes.WORLDSIZE_DUEL:		(10, 10),
		WorldSizeTypes.WORLDSIZE_TINY:		(15, 15),
		WorldSizeTypes.WORLDSIZE_SMALL:		(18, 18),
		WorldSizeTypes.WORLDSIZE_STANDARD:	(20, 20),
		WorldSizeTypes.WORLDSIZE_LARGE:		(23, 23),
		WorldSizeTypes.WORLDSIZE_HUGE:		(25, 25),
		6:									(28, 28),
		7:									(30, 30)
	}

	if (argsList[0] == -1): # (-1,) is passed to function on loads
		return []
	[eWorldSize] = argsList
	return grid_sizes[eWorldSize]

def getWrapX():
	return False
def getWrapY():
	return True

# subclass FractalWorld to enable square exponents for use with Tilted Axis.
class TiltedAxisFractalWorld(CvMapGeneratorUtil.FractalWorld):
	def initFractal(self, continent_grain = 2, rift_grain = 2, has_center_rift = True):
		"For no rifts, use rift_grain = -1"
		iFlags = CyFractal.FracVals.FRAC_WRAP_Y + CyFractal.FracVals.FRAC_POLAR
		worldsizes = {
			WorldSizeTypes.WORLDSIZE_DUEL:	  (6,6),
			WorldSizeTypes.WORLDSIZE_TINY:	  (6,6),
			WorldSizeTypes.WORLDSIZE_SMALL:	 (6,6),
			WorldSizeTypes.WORLDSIZE_STANDARD:  (7,7),
			WorldSizeTypes.WORLDSIZE_LARGE:	 (7,7),
			WorldSizeTypes.WORLDSIZE_HUGE:	  (7,7)
			}
# Rise of Mankind 2.53
		map_size = self.map.getWorldSize()
		if ( not map_size in worldsizes ):
			(fracXExp, fracYExp) = (8,8)
		else:
			(fracXExp, fracYExp) = worldsizes[map_size]
# Rise of Mankind 2.53
		if rift_grain >= 0:
			self.riftsFrac = CyFractal()
			self.riftsFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, rift_grain, self.mapRand, iFlags, fracXExp, fracYExp)
			if has_center_rift:
				iFlags = iFlags | CyFractal.FracVals.FRAC_CENTER_RIFT
			self.continentsFrac.fracInitRifts(self.iNumPlotsX, self.iNumPlotsY, continent_grain, self.mapRand, iFlags, self.riftsFrac, fracXExp, fracYExp)
		else:
			self.continentsFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, continent_grain, self.mapRand, iFlags, fracXExp, fracYExp)

def generatePlotTypes():
	gc = CyGlobalContext()
	map = CyMap()
	dice = gc.getGame().getMapRand()
	fractal_world = TiltedAxisFractalWorld()

	# Get user input.
	userInputLandmass = map.getCustomMapOption(0)

	if userInputLandmass == 4:
		NiTextOut("Setting Plot Types (Python Tilted Axis, Tiny Islands) ...")
		fractal_world.initFractal(continent_grain = 5, rift_grain = -1, has_center_rift = False)
		return fractal_world.generatePlotTypes(grain_amount = 4)

	elif userInputLandmass == 3:
		NiTextOut("Setting Plot Types (Python Tilted Axis, Islands) ...")
		fractal_world.initFractal(continent_grain = 4, rift_grain = -1, has_center_rift = False)
		return fractal_world.generatePlotTypes(grain_amount = 4)

	elif userInputLandmass == 2:
		NiTextOut("Setting Plot Types (Python Tilted Axis, Small Continents) ...")
		fractal_world.initFractal(continent_grain = 3, rift_grain = 3, has_center_rift = False)
		return fractal_world.generatePlotTypes(grain_amount = 4)

	elif userInputLandmass == 0:
		NiTextOut("Setting Plot Types (Python Tilted Axis, Massive Continents) ...")
		fractal_world.initFractal(continent_grain = 1, rift_grain = 2, has_center_rift = False)
		return fractal_world.generatePlotTypes(grain_amount = 4)

	else: # standard lands
		NiTextOut("Setting Plot Types (Python Tilted Axis, Normal Continents) ...")
		fractal_world.initFractal(continent_grain = 2, rift_grain = 2, has_center_rift = True)
		return fractal_world.generatePlotTypes(grain_amount = 4)

# subclass TerrainGenerator to make the climate "latitudes" run west to east
class TiltedAxisTerrainGenerator(CvMapGeneratorUtil.TerrainGenerator):
	def getLatitudeAtPlot(self, iX, iY):
		# Latitudes run vertically for a world with a tilted axis.
		lat = abs((self.iWidth / 2) - iX)/float(self.iWidth/2) # 0.0 = equator, 1.0 = pole

		# Adjust latitude using self.variation fractal, to mix things up:
		lat += (128 - self.variation.getHeight(iX, iY))/(255.0 * 5.0)

		# Limit to the range [0, 1]:
		if lat < 0:
			lat = 0.0
		if lat > 1:
			lat = 1.0

		return lat

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Tilted Axis) ...")
	terraingen = TiltedAxisTerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

# subclass FeatureGenerator to make the climate "latitudes" run west to east
class TiltedAxisFeatureGenerator(CvMapGeneratorUtil.FeatureGenerator):
	def getLatitudeAtPlot(self, iX, iY):
		"returns a value in the range of 0.0 (tropical) to 1.0 (polar)"
		return abs((self.iGridW/2) - iX)/float(self.iGridW/2) # 0.0 = equator, 1.0 = pole

def addFeatures():
	NiTextOut("Adding Features (Python Tilted Axis) ...")
	featuregen = TiltedAxisFeatureGenerator()
	featuregen.addFeatures()
	return 0

def afterGeneration():
	CvMapGeneratorUtil.placeC2CBonuses() # This does not place them correctly tilted
