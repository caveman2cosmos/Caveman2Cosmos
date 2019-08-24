#
#	FILE:	 ThreePlanets.py
#	AUTHOR:  Based on Highlands.py by Bob Thomas (Sirian)
#	PURPOSE: Solar System Map for Future Mod by Tholish
#-----------------------------------------------------------------------------
#	Copyright (c) Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvUtil
import CvMapGeneratorUtil
import random
import sys
from math import sqrt
from CvMapGeneratorUtil import FractalWorld
from CvMapGeneratorUtil import TerrainGenerator
from CvMapGeneratorUtil import FeatureGenerator





def getDescription():
	return "Solar system similar to our own.  All players start on an Earthlike planet, but there are two others similar to Mars and Luna.  Map size mainly determines number of civs."

def getNumCustomMapOptions():
	return 3
	
def getCustomMapOptionName(argsList):
	[iOption] = argsList
	option_names = {
		0:	"TXT_KEY_MAP_SCRIPT_MOUNTAIN_PATTERN",
		1:	"TXT_KEY_MAP_SCRIPT_MOUNTAIN_DENSITY",
		2:	"TXT_KEY_MAP_SCRIPT_WATER_SETTING"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text
	
def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0:	3,
		1:	3,
		2:	3
		}
	return option_values[iOption]
	
def getCustomMapOptionDescAt(argsList):
	[iOption, iSelection] = argsList
	selection_names = {
		0:	{
			0: "TXT_KEY_MAP_SCRIPT_SCATTERED",
			1: "TXT_KEY_MAP_SCRIPT_RIDGELINES",
			2: "TXT_KEY_MAP_SCRIPT_CLUSTERED"
			},
		1:	{
			0: "TXT_KEY_MAP_SCRIPT_DENSE_PEAKS",
			1: "TXT_KEY_MAP_SCRIPT_NORMAL_PEAKS",
			2: "TXT_KEY_MAP_SCRIPT_THIN_PEAKS"
			},
		2:	{
			0: "TXT_KEY_MAP_SCRIPT_SMALL_LAKES",
			1: "TXT_KEY_MAP_SCRIPT_LARGE_LAKES",
			2: "TXT_KEY_MAP_SCRIPT_SEAS"
			}
		}
	translated_text = unicode(CyTranslator().getText(selection_names[iOption][iSelection], ()))
	return translated_text
	
def getCustomMapOptionDefault(argsList):
	[iOption] = argsList
	option_defaults = {
		0:	1,
		1:	1,
		2:	0
		}
	return option_defaults[iOption]

def isAdvancedMap():
	"This map should show up in simple mode"
	return 0
	
def isClimateMap():
	return 1

def isSeaLevelMap():
	return 0

def beforeInit():
	# Roll a dice to determine if the cold region will be in north or south.
	gc = CyGlobalContext()
	dice = gc.getGame().getMapRand()
	global shiftMultiplier
	shiftRoll = dice.get(2, "North or South climate shift - Highlands PYTHON")
	if shiftRoll == 0: # Cold in north
		shiftMultiplier = 0.0
	else: # Cold in south
		shiftMultiplier = 1.0
	return 0

def getWrapX():
	return true
	
def getWrapY():
	return true

def addLakes():
	iPass=1

def addRivers():
	iPass=1

def getTopLatitude():
		return 90
	
def getBottomLatitude():
		return -90

def getGridSize(argsList):
	"Reduce grid sizes by one level."
	grid_sizes = {
		WorldSizeTypes.WORLDSIZE_DUEL:		(35,15),
		WorldSizeTypes.WORLDSIZE_TINY:		(35,15),
		WorldSizeTypes.WORLDSIZE_SMALL:		(35,15),
		WorldSizeTypes.WORLDSIZE_STANDARD:	(35,15),
		WorldSizeTypes.WORLDSIZE_LARGE:		(35,15),
		WorldSizeTypes.WORLDSIZE_HUGE:		(35,15)
	}

	if (argsList[0] == -1): # (-1,) is passed to function on loads
		return []
	[eWorldSize] = argsList
	return grid_sizes[eWorldSize]

def minStartingDistanceModifier():
	return -100

def findStartingPlot(argsList):
	[playerID] = argsList

	def isValid(playerID, x, y):
		
		if x <45:
			return False
  
		if x >81:
			return False

		return True
	
	return CvMapGeneratorUtil.findStartingPlot(playerID, isValid)

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Highlands) ...")
	gc = CyGlobalContext()
	map = CyMap()
	dice = gc.getGame().getMapRand()
	iFlags = 0  # Disallow FRAC_POLAR flag, to prevent "zero row" problems.
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	plotTypes = [PlotTypes.PLOT_LAND] * (iW*iH)
	terrainFrac = CyFractal()
	lakesFrac = CyFractal()
	
	# Get custom map user inputs.
	userInputGrain = map.getCustomMapOption(0)
	userInputPeaks = map.getCustomMapOption(1)
	userInputLakes = map.getCustomMapOption(2)

	# Varying grains for hills/peaks per map size and Mountain Ranges setting.
	# [clustered_grain, ridgelines_grain, scattered_grain]
	worldsizes = {
		WorldSizeTypes.WORLDSIZE_DUEL:      [3,4,5],
		WorldSizeTypes.WORLDSIZE_TINY:      [3,4,5],
		WorldSizeTypes.WORLDSIZE_SMALL:     [4,5,6],
		WorldSizeTypes.WORLDSIZE_STANDARD:  [4,5,6],
		WorldSizeTypes.WORLDSIZE_LARGE:     [4,5,6],
		WorldSizeTypes.WORLDSIZE_HUGE:      [4,5,6]
		}
	grain_list = worldsizes[map.getWorldSize()]
	grain_list.reverse()
	grain = grain_list[userInputGrain]
	
	# Peak density
	peak_list = [70, 77, 83]
	hill_list = [40, 45, 50]
	peaks = peak_list[userInputPeaks]
	hills = hill_list[userInputPeaks]

	terrainFrac.fracInit(iW, iH, grain, dice, 0, -1, -1)

	iHillsThreshold = terrainFrac.getHeightFromPercent(hills)
	iPeaksThreshold = terrainFrac.getHeightFromPercent(peaks)

	# Now the main loop, which will assign the plot types.
	for x in range(iW):
		for y in range(iH):
			i = y*iW + x
			val = terrainFrac.getHeight(x,y)
			if val >= iPeaksThreshold:
				plotTypes[i] = PlotTypes.PLOT_PEAK
			elif val >= iHillsThreshold and val < iPeaksThreshold:
				plotTypes[i] = PlotTypes.PLOT_HILLS
			else:
				plotTypes[i] = PlotTypes.PLOT_LAND 

	return plotTypes

# subclass TerrainGenerator to redefine everything. This is a regional map.
class HighlandsTerrainGenerator(CvMapGeneratorUtil.TerrainGenerator):
	def __init__(self, fracXExp=-1, fracYExp=-1):
		# Note: If you change longitude values here, then you will...
		# ...need to change them elsewhere in the script, as well.
		self.gc = CyGlobalContext()
		self.map = CyMap()

		self.grain_amount = 4 + self.gc.getWorldInfo(self.map.getWorldSize()).getTerrainGrainChange()

		self.iWidth = self.map.getGridWidth()
		self.iHeight = self.map.getGridHeight()

		self.mapRand = self.gc.getGame().getMapRand()

		self.iFlags = 0  # Disallow FRAC_POLAR flag, to prevent "zero row" problems.

		self.cold=CyFractal()
		self.cool=CyFractal()
		self.temp=CyFractal()
		self.hot=CyFractal()
		self.variation=CyFractal()

		self.iColdIBottomPercent = 75
		self.iColdTBottomPercent = 20
		self.iCoolTBottomPercent = 85
		self.iCoolPBottomPercent = 45
		self.iTempDBottomPercent = 90
		self.iTempPBottomPercent = 65
		self.iHotDBottomPercent = 70
		self.iHotPBottomPercent = 60

		self.fColdLatitude = 0.8
		self.fCoolLatitude = 0.6
		self.fHotLatitude = 0.2

		self.fracXExp = fracXExp
		self.fracYExp = fracYExp

		self.initFractals()
		
	def initFractals(self):
		self.cold.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.iColdIBottom = self.cold.getHeightFromPercent(self.iColdIBottomPercent)
		self.iColdTBottom = self.cold.getHeightFromPercent(self.iColdTBottomPercent)

		self.cool.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.iCoolTBottom = self.cool.getHeightFromPercent(self.iCoolTBottomPercent)
		self.iCoolPBottom = self.cool.getHeightFromPercent(self.iCoolPBottomPercent)

		self.temp.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.iTempDBottom = self.temp.getHeightFromPercent(self.iTempDBottomPercent)
		self.iTempPBottom = self.temp.getHeightFromPercent(self.iTempPBottomPercent)

		self.hot.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.iHotDBottom = self.hot.getHeightFromPercent(self.iHotDBottomPercent)
		self.iHotPBottom = self.hot.getHeightFromPercent(self.iHotPBottomPercent)

		self.variation.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)

		self.terrainDesert = self.gc.getInfoTypeForString("TERRAIN_COAST")
		self.terrainPlains = self.gc.getInfoTypeForString("TERRAIN_COAST")
		self.terrainGrass = self.gc.getInfoTypeForString("TERRAIN_GRASS")
		self.terrainIce = self.gc.getInfoTypeForString("TERRAIN_COAST")
		self.terrainTundra = self.gc.getInfoTypeForString("TERRAIN_COAST")
		self.terrainCoast = self.gc.getInfoTypeForString("TERRAIN_ORBIT")
		self.terrainOcean = self.gc.getInfoTypeForString("TERRAIN_SPACE")
		self.terrainSpacerock = self.gc.getInfoTypeForString("TERRAIN_LUNAR_BASALT")
		self.terrainMars = self.gc.getInfoTypeForString("TERRAIN_MARTIAN_BARREN")
		self.terrainVenus = self.gc.getInfoTypeForString("TERRAIN_MARTIAN_DUNES")
		self.terrainIcemoon = self.gc.getInfoTypeForString("TERRAIN_MARTIAN_ICE")
		self.terrainTitan = self.gc.getInfoTypeForString("TERRAIN_LUNAR_BASALT")
		self.terrainPermafrost = self.gc.getInfoTypeForString("TERRAIN_TAIGA")
		self.terrainPolar = self.gc.getInfoTypeForString("TERRAIN_COAST_POLAR")

	def getLatitudeAtPlot(self, iX, iY):
		# Latitudes run vertically for a world with a tilted axis.
		lat = iY/float(self.iWidth) # 0.0 = south

		# Adjust latitude using self.variation fractal, to mix things up:
		lat += (128 - self.variation.getHeight(iX, iY))/(255.0 * 5.0)

		# Limit to the range [0, 1]:
		if lat < 0:
			lat = 0.0
		if lat > 1:
			lat = 1.0

		return lat

	def generateTerrainAtPlot(self,iX,iY):
		lat = self.getLatitudeAtPlot(iX,iY)

		if (self.map.plot(iX, iY).isWater()):
			return self.map.plot(iX, iY).getTerrainType()

		if lat >= self.fColdLatitude:
			val = self.cold.getHeight(iX, iY)
			if val >= self.iColdIBottom:
				terrainVal = self.terrainIce
			elif val >= self.iColdTBottom and val < self.iColdIBottom:
				terrainVal = self.terrainTundra
			else:
				terrainVal = self.terrainPlains
		elif lat < self.fColdLatitude and lat >= self.fCoolLatitude:
			val = self.cool.getHeight(iX, iY)
			if val >= self.iCoolTBottom:
				terrainVal = self.terrainTundra
			elif val >= self.iCoolPBottom and val < self.iCoolTBottom:
				terrainVal = self.terrainPlains
			else:
				terrainVal = self.terrainGrass
		elif lat < self.fHotLatitude:
			val = self.hot.getHeight(iX, iY)
			if val >= self.iHotDBottom:
				terrainVal = self.terrainDesert
			elif val >= self.iHotPBottom and val < self.iHotDBottom:
				terrainVal = self.terrainPlains
			else:
				terrainVal = self.terrainGrass
		else:
			val = self.temp.getHeight(iX, iY)
			if val >= self.iTempDBottom:
				terrainVal = self.terrainDesert
			elif val < self.iTempDBottom and val >= self.iTempPBottom:
				terrainVal = self.terrainPlains
			else:
				terrainVal = self.terrainGrass

		if (terrainVal == TerrainTypes.NO_TERRAIN):
			return self.map.plot(iX, iY).getTerrainType()

		if iX==0:
			     terrainVal =self.terrainCoast

		if iX==1:
			     terrainVal =self.terrainCoast

		if iX>1:
		     if iX<29:
				terrainVal =self.terrainSpacerock

		if iX==29:
			terrainVal =self.terrainCoast

		if iX==30:
			terrainVal =self.terrainCoast

		if iX>30:
		     if iX<43:
                      terrainVal =self.terrainOcean

		if iX==43:
			terrainVal =self.terrainCoast

		if iX==44:
			terrainVal =self.terrainCoast

		if iX==45:
			terrainVal =self.terrainPolar

		if iX==46:
		      if terrainVal == self.terrainGrass:
 			     terrainVal =self.terrainPermafrost  

		if iX==80:
		      if terrainVal == self.terrainGrass:
 			     terrainVal =self.terrainPermafrost 

		if iX==81:
                 terrainVal =self.terrainPolar


		if iX==82:
			terrainVal =self.terrainCoast

		if iX==83:
			terrainVal =self.terrainCoast

		if iX>83:
		     if iX<96:
                      terrainVal =self.terrainOcean

		if iX==96:
			     terrainVal =self.terrainCoast

		if iX==97:
			     terrainVal =self.terrainCoast

		if iX>97:
		     if iX<125:
				terrainVal =self.terrainMars

		if iX==125:
			terrainVal =self.terrainCoast

		if iX==126:
			terrainVal =self.terrainCoast

		if iX>126:
			terrainVal =self.terrainOcean

		return terrainVal


def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Highlands) ...")
	terraingen = HighlandsTerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

class HighlandsFeatureGenerator(CvMapGeneratorUtil.FeatureGenerator):
	def __init__(self, iJunglePercent=60, iForestPercent=45, iHotForestPercent = 25,
	             forest_grain=6, fracXExp=-1, fracYExp=-1):
		self.gc = CyGlobalContext()
		self.map = CyMap()
		self.mapRand = self.gc.getGame().getMapRand()
		self.forests = CyFractal()
		
		self.iFlags = 0  # Disallow FRAC_POLAR flag, to prevent "zero row" problems.

		self.iGridW = self.map.getGridWidth()
		self.iGridH = self.map.getGridHeight()
		
		self.iJunglePercent = iJunglePercent
		self.iForestPercent = iForestPercent
		self.iHotForestPercent = iHotForestPercent
		
		self.forest_grain = forest_grain + self.gc.getWorldInfo(self.map.getWorldSize()).getFeatureGrainChange()

		self.fracXExp = fracXExp
		self.fracYExp = fracYExp

		self.__initFractals()
		self.__initFeatureTypes()
	
	def __initFractals(self):
		self.forests.fracInit(self.iGridW, self.iGridH, self.forest_grain, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		
		self.iJungleLevel = self.forests.getHeightFromPercent(100 - self.iJunglePercent)
		self.iForestLevel = self.forests.getHeightFromPercent(self.iForestPercent)
		self.iHotForestLevel = self.forests.getHeightFromPercent(self.iHotForestPercent)
		
	def __initFeatureTypes(self):
		self.featureJungle = self.gc.getInfoTypeForString("FEATURE_CARBONICE")
		self.featureForest = self.gc.getInfoTypeForString("FEATURE_REGOLITH")
		self.featureOasis = self.gc.getInfoTypeForString("FEATURE_SCORCH")
		self.featureFlood = self.gc.getInfoTypeForString("FEATURE_REGOLITH")
		self.featureIce = self.gc.getInfoTypeForString("FEATURE_FALLOUT")
		self.featureAsteroids = self.gc.getInfoTypeForString("FEATURE_ASTEROIDS")
		self.featureWoods = self.gc.getInfoTypeForString("FEATURE_FOREST")
		self.featureGlacier = self.gc.getInfoTypeForString("FEATURE_ICE")
		self.featureMargin = self.gc.getInfoTypeForString("FEATURE_MARGIN")

	def getLatitudeAtPlot(self, iX, iY):
		lat = iY/float(self.iGridH) # 0.0 = south
		# Flip terrain if southward shift was rolled.
		global shiftMultiplier
		return abs(lat - shiftMultiplier)

	def addFeaturesAtPlot(self, iX, iY):
		lat = self.getLatitudeAtPlot(iX, iY)
		pPlot = self.map.sPlot(iX, iY)

		for iI in range(self.gc.getNumFeatureInfos()):
			if pPlot.canHaveFeature(iI):
				if self.mapRand.get(10000, "Add Feature PYTHON") < self.gc.getFeatureInfo(iI).getAppearanceProbability():
					pPlot.setFeatureType(iI, -1)

		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addJunglesAtPlot(pPlot, iX, iY, lat)
			
		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addForestsAtPlot(pPlot, iX, iY, lat)

		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addAsteroidsAtPlot(pPlot, iX, iY, lat)

		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addWoodsAtPlot(pPlot, iX, iY, lat)

		if iX<3:
			if pPlot.isHills() or pPlot.isPeak(): 
				pPlot.setPlotType(PlotTypes.PLOT_LAND, true, true)

		if iX>27:
		     if iX<46:
				if pPlot.isHills() or pPlot.isPeak(): 
					pPlot.setPlotType(PlotTypes.PLOT_LAND, true, true)

		if iX>45:
		     if iX<81:
				if iY>25:
		     			if iY<35:
						if pPlot.isHills() or pPlot.isPeak(): 
							pPlot.setPlotType(PlotTypes.PLOT_OCEAN, true, true)

		if iX>81:
		     if iX<99:
				if pPlot.isHills() or pPlot.isPeak(): 
					pPlot.setPlotType(PlotTypes.PLOT_LAND, true, true)

		if iX>123:
			if pPlot.isHills() or pPlot.isPeak(): 
					pPlot.setPlotType(PlotTypes.PLOT_LAND, true, true)

		if iX==2:
			self.addMarginAtPlot(pPlot, iX, iY, lat)

		if iX==28:
			self.addMarginAtPlot(pPlot, iX, iY, lat)

		if iX==45:
			self.addGlacierAtPlot(pPlot, iX, iY, lat)

		if iX==81:
			self.addGlacierAtPlot(pPlot, iX, iY, lat)

		if iX==98:
			self.addMarginAtPlot(pPlot, iX, iY, lat)

		if iX==124:
			self.addMarginAtPlot(pPlot, iX, iY, lat)

	def addMarginAtPlot(self, pPlot, iX, iY, lat):
		pPlot.setFeatureType(self.featureMargin, -1)

	def addGlacierAtPlot(self, pPlot, iX, iY, lat):
		pPlot.setFeatureType(self.featureGlacier, -1)

	def addAsteroidsAtPlot(self, pPlot, iX, iY, lat):
		if pPlot.canHaveFeature(self.featureAsteroids):
			pPlot.setFeatureType(self.featureAsteroids, -1)

	def addWoodsAtPlot(self, pPlot, iX, iY, lat):
		if pPlot.canHaveFeature(self.featureWoods):
			pPlot.setFeatureType(self.featureWoods, -1)
	
	def addJunglesAtPlot(self, pPlot, iX, iY, lat):
		# Warning: this version of JunglesAtPlot is using the forest fractal!
		if lat > 0.50:
			if lat < 0.60 and pPlot.canHaveFeature(self.featureJungle):
				if (self.forests.getHeight(iX, iY) >= self.iJungleLevel):
					pPlot.setFeatureType(self.featureJungle, -1)

		if lat < 0.10 and pPlot.canHaveFeature(self.featureJungle):
			if (self.forests.getHeight(iX, iY) >= self.iJungleLevel):
				pPlot.setFeatureType(self.featureJungle, -1)

	def addForestsAtPlot(self, pPlot, iX, iY, lat):
		if lat > 0.2:
			if pPlot.canHaveFeature(self.featureForest):
				if self.forests.getHeight(iX, iY) <= self.iForestLevel:
					pPlot.setFeatureType(self.featureForest, -1)
		else:
			if pPlot.canHaveFeature(self.featureForest):
				if self.forests.getHeight(iX, iY) <= self.iHotForestLevel:
					pPlot.setFeatureType(self.featureForest, -1)

def addFeatures():
	global featuregen
	NiTextOut("Adding Features (Python Highlands) ...")
	featuregen = HighlandsFeatureGenerator()
	featuregen.addFeatures()
	return 0



def normalizeRemovePeaks():
	iPass=1



def normalizeRemoveBadFeatures():
	iPass=1

def normalizeRemoveBadTerrain():
	iPass=1

def normalizeAddFoodBonuses()	:
	iPass=1

def normalizeAddGoodTerrain():
	iPass=1

def normalizeAddExtras():
	iPass=1

def startHumansOnSameTile():
	return True



