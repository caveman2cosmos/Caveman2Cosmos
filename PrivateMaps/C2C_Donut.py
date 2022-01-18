#
#	FILE:	 Donut.py
#	AUTHOR:  Bob Thomas (Sirian)
#	PURPOSE: Global map script - Circular continent with center region.
#-----------------------------------------------------------------------------
#	Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvMapGeneratorUtil as MGU
from math import sqrt

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_DONUT_DESCR"

def getNumCustomMapOptions():
	return 4

def getNumHiddenCustomMapOptions():
	return 2

def getCustomMapOptionName(argsList):
	[iOption] = argsList
	option_names = {
		0:	"TXT_KEY_MAP_SCRIPT_CENTER_REGION",
		1:	"TXT_KEY_FEATURE_JUNGLE",
		2:	"TXT_KEY_MAP_WORLD_WRAP",
		3:  "TXT_KEY_CONCEPT_RESOURCES"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0:	5,
		1:	2,
		2:	3,
		3:  2
		}
	return option_values[iOption]

def getCustomMapOptionDescAt(argsList):
	[iOption, iSelection] = argsList
	selection_names = {
		0:	{
			0: "TXT_KEY_MAP_SCRIPT_HILLS",
			1: "TXT_KEY_MAP_SCRIPT_PEAKS",
			2: "TXT_KEY_MAP_SCRIPT_DESERT",
			3: "TXT_KEY_MAP_SCRIPT_OCEAN",
			4: "TXT_KEY_WORLD_STANDARD"
			},
		1:	{
			0: "TXT_KEY_FEATURE_JUNGLE",
			1: "TXT_KEY_MAP_SCRIPT_NO_JUNGLES"
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
		0:	1,
		1:	0,
		2:	0,
		3:  0
		}
	return option_defaults[iOption]

def isRandomCustomMapOption(argsList):
	[iOption] = argsList
	option_random = {
		0:	false,
		1:	false,
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
		MGU.BonusBalancer().normalizeAddExtras()
	CyPythonMgr().allowDefaultImpl()	# do the rest of the usual normalizeStartingPlots stuff, don't overrride

def addBonusType(argsList):
	[iBonusType] = argsList

	if CyMap().getCustomMapOption(3) == 1 and CyGlobalContext().getBonusInfo(iBonusType).getType() in MGU.BonusBalancer().resourcesToBalance:
		return None # don't place any of this bonus randomly

	CyPythonMgr().allowDefaultImpl() # pretend we didn't implement this method, and let C handle this bonus in the default way

def isAdvancedMap():
	"This map should not show up in simple mode"
	return 1

def isClimateMap():
	return 0

def isSeaLevelMap():
	return 0

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

def minStartingDistanceModifier():
	return -12

class DonutFractalWorld(MGU.FractalWorld):

	def generatePlotTypes(self, water_percent=78, shift_plot_types=True, grain_amount=3):
		self.hillsFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.peaksFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, grain_amount+1, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)

		iHillsBottom1 = self.hillsFrac.getHeightFromPercent(max((self.hillGroupOneBase - self.hillGroupOneRange), 0))
		iHillsTop1 = self.hillsFrac.getHeightFromPercent(min((self.hillGroupOneBase + self.hillGroupOneRange), 100))
		iHillsBottom2 = self.hillsFrac.getHeightFromPercent(max((self.hillGroupTwoBase - self.hillGroupTwoRange), 0))
		iHillsTop2 = self.hillsFrac.getHeightFromPercent(min((self.hillGroupTwoBase + self.hillGroupTwoRange), 100))
		iPeakThreshold = self.peaksFrac.getHeightFromPercent(self.peakPercent)

		iCenterX = int(self.iNumPlotsX / 2)
		iCenterY = int(self.iNumPlotsY / 2)
		iRadius = min((iCenterX - 4), (iCenterY - 4))
		iHoleRadius = int(iRadius / 2)
		userInputCenter = self.map.getCustomMapOption(0)

		for x in range(self.iNumPlotsX):
			for y in range(self.iNumPlotsY):
				i = y*self.iNumPlotsX + x
				if x == iCenterX and y == iCenterY:
					fDistance = 0
				else:
					fDistance = sqrt(((x - iCenterX) ** 2) + ((y - iCenterY) ** 2))
				if fDistance > iRadius:
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN
				elif fDistance < iHoleRadius and userInputCenter != 4: # Plot is in hole of donut.
					if userInputCenter == 0:
						self.plotTypes[i] = PlotTypes.PLOT_HILLS
					elif userInputCenter == 2:
						self.plotTypes[i] = PlotTypes.PLOT_LAND
					elif userInputCenter == 3:
						self.plotTypes[i] = PlotTypes.PLOT_OCEAN
					else:
						self.plotTypes[i] = PlotTypes.PLOT_PEAK
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

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Donut) ...")
	fractal_world = DonutFractalWorld()
	return fractal_world.generatePlotTypes()

# subclass TerrainGenerator to create a lush grassland utopia.
class DonutTerrainGenerator(MGU.TerrainGenerator):

	def __init__(self, fracXExp=-1, fracYExp=-1, grain_amount=5):
		MGU.TerrainGenerator.__init__(self, 5, 25, 15, 0.7, 0.6, 0.1, 0.2, 0.5, fracXExp, fracYExp, grain_amount)
		self.iCenterX = int(self.map.getGridWidth() / 2)
		self.iCenterY = int(self.map.getGridHeight() / 2)
		self.iRadius = min((self.iCenterX - 4), (self.iCenterY - 4))
		self.iHoleRadius = int(self.iRadius / 2)
		self.userInputCenter = self.map.getCustomMapOption(0)

	def getLatitudeAtPlot(self, iX, iY):
		return 0

	def generateTerrain(self):
		terrainData = [0]*(self.iWidth*self.iHeight)
		for x in range(self.iWidth):
			for y in range(self.iHeight):
				iI = y*self.iWidth + x
				terrain = self.generateTerrainAtPlot(x, y)
				terrainData[iI] = terrain
		return terrainData

	def generateTerrainAtPlot(self,iX,iY):
		if (self.map.plot(iX, iY).isWater()):
			return self.map.plot(iX, iY).getTerrainType()

		if iX == self.iCenterX and iY == self.iCenterY:
			fDistance = 0
		else:
			fDistance = sqrt(((iX - self.iCenterX) ** 2) + ((iY - self.iCenterY) ** 2))
		if fDistance < self.iHoleRadius and self.userInputCenter != 4: # Plot is in hole of donut.
			if self.userInputCenter == 2:
				terrainVal = self.terrainDesert
			elif self.userInputCenter == 3:
				return self.map.plot(iX, iY).getTerrainType()
			else:
				terrainVal = self.terrainGrass
		else:
			terrainVal = MGU.TerrainGenerator.generateTerrainAtPlot(self,iX,iY)

		if (terrainVal == TerrainTypes.NO_TERRAIN):
			return self.map.plot(iX, iY).getTerrainType()

		return terrainVal

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Donut) ...")
	terraingen = DonutTerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

class DonutFeatureGenerator(MGU.FeatureGenerator):

	def addIceAtPlot(self, pPlot, iX, iY, lat):
		# We don' need no steeking ice. M'kay? Alrighty then.
		return

	def addJunglesAtPlot(self, pPlot, iX, iY, lat):
		if (CyMap().getCustomMapOption(1) == 1): pass #No Jungles option
		else: #Normal Jungles
			if pPlot.canHaveFeature(self.featureJungle):
				iJungleHeight = self.jungles.getHeight(iX, iY)
				if self.iJungleTop >= iJungleHeight >= self.iJungleBottom + (self.iJungleTop - self.iJungleBottom)*self.GC.getClimateInfo(self.map.getClimate()).getJungleLatitude()*lat:
					pPlot.setFeatureType(self.featureJungle, -1)

def addFeatures():
	NiTextOut("Adding Features (Python Donut) ...")
	featuregen = DonutFeatureGenerator()
	featuregen.addFeatures()
	return 0

def findStartingPlot(argsList):
	[playerID] = argsList

	def isValid(playerID, x, y):
		pWaterArea = CyMap().plot(x, y).waterArea()
		return pWaterArea is not None and not pWaterArea.isLake()

	return MGU.findStartingPlot(playerID, isValid)

def afterGeneration():
	MGU.placeC2CBonuses()
