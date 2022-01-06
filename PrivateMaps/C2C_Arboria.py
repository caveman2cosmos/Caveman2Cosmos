#
#	FILE:	 Arboria.py
#	AUTHOR:  Bob Thomas (Sirian)
#	PURPOSE: Global map script - Forest paradise - Primarily for MP
#-----------------------------------------------------------------------------
#	Copyright (c) 2005, 2007 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvMapGeneratorUtil
from CvMapGeneratorUtil import HintedWorld

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_ARBORIA_DESCR"

def isAdvancedMap():
	"This map should not show up in simple mode"
	#return 1
# Rise of Mankind 2.5
	return 0
# Rise of Mankind 2.5

def isClimateMap():
	return 0

def isSeaLevelMap():
	return 0

def getNumCustomMapOptions():
	return 1

def getNumHiddenCustomMapOptions():
	return 1

def getCustomMapOptionName(argsList):
	translated_text = unicode(CyTranslator().getText("TXT_KEY_MAP_WORLD_WRAP", ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	return 3

def getCustomMapOptionDescAt(argsList):
	iSelection = argsList[1]
	selection_names = ["TXT_KEY_MAP_WRAP_FLAT",
					   "TXT_KEY_MAP_WRAP_CYLINDER",
					   "TXT_KEY_MAP_WRAP_TOROID"]
	translated_text = unicode(CyTranslator().getText(selection_names[iSelection], ()))
	return translated_text

def getCustomMapOptionDefault(argsList):
	return 1

def isRandomCustomMapOption(argsList):
	return false

def getWrapX():
	map = CyMap()
	return (map.getCustomMapOption(0) == 1 or map.getCustomMapOption(0) == 2)

def getWrapY():
	map = CyMap()
	return (map.getCustomMapOption(0) == 2)

def getTopLatitude():
	return 50

def getBottomLatitude():
	return -50

def minStartingDistanceModifier():
	return -10

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
	map = CyMap()
	dice = CyGame().getMapRand()
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	global food
	food = CyFractal()
	food.fracInit(iW, iH, 7, dice, 0, -1, -1)

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Arboria) ...")
	global hinted_world
	hinted_world = HintedWorld(16,8)

	mapRand = CyGlobalContext().getGame().getMapRand()

	numBlocks = hinted_world.w * hinted_world.h
	numBlocksLand = int(numBlocks*0.50)
	cont = hinted_world.addContinent(numBlocksLand,mapRand.get(5, "Generate Plot Types PYTHON")+4,mapRand.get(3, "Generate Plot Types PYTHON")+2)
	if not cont:
		print "Couldn't create continent! Reverting to C implementation."
		CyPythonMgr().allowDefaultImpl()
	else:
		for x in range(hinted_world.w):
			for y in (0, hinted_world.h - 1):
				hinted_world.setValue(x,y, 1) # force ocean at poles
		hinted_world.buildAllContinents()
		return hinted_world.generatePlotTypes(shift_plot_types=True)

# subclass TerrainGenerator to create a lush grassland utopia.
class ArboriaTerrainGenerator(CvMapGeneratorUtil.TerrainGenerator):
	def __init__(self, fracXExp=-1, fracYExp=-1, grain_amount=5):
		CvMapGeneratorUtil.TerrainGenerator.__init__(self, 5, 25, 15, 0.7, 0.6, 0.1, 0.2, 0.5, fracXExp, fracYExp, grain_amount)

	def getLatitudeAtPlot(self, iX, iY):
		return 0

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Arboria) ...")
	terraingen = ArboriaTerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

class ArboriaFeatureGenerator(CvMapGeneratorUtil.FeatureGenerator):
	def __init__(self, forest_grain=6, fracXExp=-1, fracYExp=-1):
		GC = CyGlobalContext()
		self.map = CyMap()
		self.mapRand = GC.getGame().getMapRand()
		self.forests = CyFractal()

		self.iFlags = 0  # Disallow FRAC_POLAR flag, to prevent "zero row" problems.

		self.iGridW = self.map.getGridWidth()
		self.iGridH = self.map.getGridHeight()

		self.forest_grain = forest_grain + GC.getWorldInfo(self.map.getWorldSize()).getFeatureGrainChange()

		self.fracXExp = fracXExp
		self.fracYExp = fracYExp

		self.__initFractals()
		self.__initFeatureTypes()

	def __initFractals(self):
		self.forests.fracInit(self.iGridW, self.iGridH, self.forest_grain, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)

		self.iJungleStart = self.forests.getHeightFromPercent(65)
		self.iJungleStop = self.forests.getHeightFromPercent(69)
		self.iForestStart = self.forests.getHeightFromPercent(29)

	def __initFeatureTypes(self):
		GC = CyGlobalContext()
		self.featureJungle = GC.getInfoTypeForString("FEATURE_JUNGLE")
		self.featureForest = GC.getInfoTypeForString("FEATURE_FOREST")

	def getLatitudeAtPlot(self, iX, iY):
		return 50

	def addFeaturesAtPlot(self, iX, iY):
		"adds any appropriate features at the plot (iX, iY) where (0,0) is in the SW"
		GC = CyGlobalContext()
		long = iX/float(self.iGridW)
		lat = iY/float(self.iGridH)
		pPlot = self.map.sPlot(iX, iY)

		for iI in range(GC.getNumFeatureInfos()):
#			print GC.getFeatureInfo(iI).getDescription()
			if pPlot.canHaveFeature(iI):
#				print "Can have feature with probability: %d" % GC.getFeatureInfo(iI).getAppearanceProbability()
				if self.mapRand.get(10000, "Add Feature PYTHON") < GC.getFeatureInfo(iI).getAppearanceProbability():
#					print "Setting feature"
					pPlot.setFeatureType(iI, -1)

		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addJunglesAtPlot(pPlot, iX, iY, lat)

		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addForestsAtPlot(pPlot, iX, iY, lat, long)

	def addIceAtPlot(self, pPlot, iX, iY, lat):
		# We don' need no steeking ice. M'kay? Alrighty then.
		return

	def addJunglesAtPlot(self, pPlot, iX, iY, lat):
		# Warning: this version of JunglesAtPlot is using the forest fractal!
		if pPlot.canHaveFeature(self.featureJungle):
			if (self.forests.getHeight(iX, iY) >= self.iJungleStart) and (self.forests.getHeight(iX, iY) <= self.iJungleStop):
				pPlot.setFeatureType(self.featureJungle, -1)

	def addForestsAtPlot(self, pPlot, iX, iY, lat, long):
		# Deciduous trees everywhere.
		if pPlot.canHaveFeature(self.featureForest):
			if self.forests.getHeight(iX, iY) >= self.iForestStart:
				pPlot.setFeatureType(self.featureForest, 0)

def addFeatures():
	global featuregen
	NiTextOut("Adding Features (Python Arboria) ...")
	featuregen = ArboriaFeatureGenerator()
	featuregen.addFeatures()
	NiTextOut("Simulate Forest Paradise (Forests - Python Arboria) ...")
	return 0

def normalizeRemovePeaks():
	return None

def normalizeRemoveBadFeatures():
	return None

def normalizeRemoveBadTerrain():
	return None

def normalizeAddGoodTerrain():
	return None

def normalizeAddExtras():
	return None

# Sirian's "Sahara Regional Bonus Placement" system.

# Init all bonuses. This is your master key.
forest = ('BONUS_SILVER_ORE', 'BONUS_DEER')
silver = ('BONUS_SILVER_ORE')
deer = ('BONUS_DEER')

def addBonusType(argsList):
	print('*******')
	[iBonusType] = argsList
	GC = CyGlobalContext()
	map = CyMap()
	type_string = GC.getBonusInfo(iBonusType).getType()

	if not (type_string in forest):
		print('Default', type_string, 'Default')
		CyPythonMgr().allowDefaultImpl() # Let C handle this bonus in the default way.
	else: # Current bonus type is custom-handled. Assignments to follow.
		iW = map.getGridWidth()
		iH = map.getGridHeight()

		# Generate resources
		if (type_string in forest):
			print('---', type_string, '---')
			global food
			NiTextOut("Placing forest resources (Python Arboria) ...")
			iSilverBottom = food.getHeightFromPercent(10)
			iSilverTop = food.getHeightFromPercent(15)
			iDeerBottom1 = food.getHeightFromPercent(24)
			iDeerTop1 = food.getHeightFromPercent(27)
			iDeerBottom2 = food.getHeightFromPercent(49)
			iDeerTop2 = food.getHeightFromPercent(52)
			iDeerBottom3 = food.getHeightFromPercent(73)
			iDeerTop3 = food.getHeightFromPercent(76)

			for y in range(iH):
				for x in range(iW):
					# Fractalized placement
					pPlot = map.plot(x,y)
					if pPlot.isWater() or pPlot.isPeak(): continue
					if pPlot.getBonusType(-1) == -1:
						foodVal = food.getHeight(x,y)
						if (type_string in deer):
							if pPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_FOREST") and pPlot.isFlatlands():
								if (foodVal >= iDeerBottom1 and foodVal <= iDeerTop1) or (foodVal >= iDeerBottom2 and foodVal <= iDeerTop2) or (foodVal >= iDeerBottom3 and foodVal <= iDeerTop3):
									map.plot(x,y).setBonusType(iBonusType)
						if (type_string in silver):
							if pPlot.isHills():
								if (foodVal >= iSilverBottom and foodVal <= iSilverTop):
									map.plot(x,y).setBonusType(iBonusType)
		return None

def afterGeneration():
	CvMapGeneratorUtil.placeC2CBonuses()
