#
#	FILE:	 Global_Highlands.py
#	AUTHOR:  Bob Thomas (Sirian)
#	PURPOSE: Global map script - mountainous terrain
#-----------------------------------------------------------------------------
#	Copyright (c) 2005, 2007 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvMapGeneratorUtil as MGU

balancer = MGU.BonusBalancer()

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_GLOBAL_HIGHLANDS_DESCR"

def getNumCustomMapOptions():
	return 5

def getNumHiddenCustomMapOptions():
	return 2

def getCustomMapOptionName(argsList):
	[iOption] = argsList
	option_names = {
		0:	"TXT_KEY_MAP_SCRIPT_MOUNTAIN_PATTERN",
		1:	"TXT_KEY_MAP_SCRIPT_MOUNTAIN_DENSITY",
		2:	"TXT_KEY_MAP_SCRIPT_LANDMASS_SIZE",
		3:  "TXT_KEY_MAP_WORLD_WRAP",
		4:  "TXT_KEY_CONCEPT_RESOURCES"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0:	3,
		1:	3,
		2:	4,
		3:  3,
		4:  2
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
			0: "TXT_KEY_MAP_SCRIPT_MASSIVE_CONTINENTS",
			1: "TXT_KEY_MAP_SCRIPT_NORMAL_CONTINENTS",
			2: "TXT_KEY_MAP_SCRIPT_SNAKY_CONTINENTS",
			3: "TXT_KEY_MAP_SCRIPT_RANDOM"
			},
		3:	{
			0: "TXT_KEY_MAP_WRAP_FLAT",
			1: "TXT_KEY_MAP_WRAP_CYLINDER",
			2: "TXT_KEY_MAP_WRAP_TOROID"
			},
		4:	{
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
		2:	3,
		3:  1,
		4:  0
		}
	return option_defaults[iOption]

def isRandomCustomMapOption(argsList):
	[iOption] = argsList
	option_random = {
		0:	true,
		1:	true,
		2:	false,
		3:	false,
		4:  false
		}
	return option_random[iOption]

def isAdvancedMap():
	"This map should not show up in simple mode"
	return 1

def getWrapX():
	map = CyMap()
	return (map.getCustomMapOption(3) == 1 or map.getCustomMapOption(3) == 2)

def getWrapY():
	map = CyMap()
	return (map.getCustomMapOption(3) == 2)

def normalizeAddExtras():
	if (CyMap().getCustomMapOption(4) == 1):
		balancer.normalizeAddExtras()
	CyPythonMgr().allowDefaultImpl()	# do the rest of the usual normalizeStartingPlots stuff, don't overrride

def addBonusType(argsList):
	[iBonusType] = argsList
	type_string = CyGlobalContext().getBonusInfo(iBonusType).getType()

	if CyMap().getCustomMapOption(4) == 1 and type_string in balancer.resourcesToBalance:
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

	if (argsList[0] == -1): # (-1,) is passed to function on loads
		return []
	[eWorldSize] = argsList
	return grid_sizes[eWorldSize]

def minStartingDistanceModifier():
	return -25

# subclass FractalWorld to borrow default process for land/sea formation only.
class GHFractalWorld(MGU.FractalWorld):
	def generatePlotTypes(self, water_percent=78, shift_plot_types=True, grain_amount=3):
		# Check for changes to User Input variances.
		self.checkForOverrideDefaultUserInputVariances()

		self.hillsFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.peaksFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, grain_amount+1, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)

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
					self.plotTypes[i] = PlotTypes.PLOT_LAND

		if shift_plot_types:
			self.shiftPlotTypes()

		return self.plotTypes

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Global Highlands) ...")
	NiTextOut("Setting Plot Types (Python Global Highlands) ...")
	gc = CyGlobalContext()
	map = CyMap()
	dice = gc.getGame().getMapRand()
	iFlags = 0  # Disallow FRAC_POLAR flag, to prevent "zero row" problems.
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	plotTypes = [PlotTypes.PLOT_OCEAN] * (iW*iH)
	terrainFrac = CyFractal()

	# Get custom map user inputs.
	userInputGrain = map.getCustomMapOption(0)
	userInputPeaks = map.getCustomMapOption(1)
	userInputLandmass = map.getCustomMapOption(2)

	fractal_world = GHFractalWorld()

	if userInputLandmass == 3: #Random landmass sizes
		grainRoll = 1 + dice.get(7, "Fractal Grain - Global Highlands PYTHON")
		if grainRoll > 4: grainRoll -= 4
		if grainRoll == 4:
			fractal_world.initFractal(polar = True)
			plotTypes = fractal_world.generatePlotTypes(water_percent=75)
		else:
			fractal_world.initFractal(continent_grain = grainRoll, rift_grain = -1, has_center_rift = False, polar = True)
			plotTypes = fractal_world.generatePlotTypes()
	else:
		grainRoll = 1 + userInputLandmass
		fractal_world.initFractal(continent_grain = grainRoll, rift_grain = -1, has_center_rift = False, polar = True)
		plotTypes = fractal_world.generatePlotTypes()

	# Varying grains for hills/peaks per map size and Mountain Ranges setting.
	# [clustered_grain, ridgelines_grain, scattered_grain]
	worldsizes = {
		WorldSizeTypes.WORLDSIZE_DUEL:		[3,4,5],
		WorldSizeTypes.WORLDSIZE_TINY:		[3,4,5],
		WorldSizeTypes.WORLDSIZE_SMALL:		[4,5,6],
		WorldSizeTypes.WORLDSIZE_STANDARD:	[4,5,6],
		WorldSizeTypes.WORLDSIZE_LARGE:		[4,5,6],
		WorldSizeTypes.WORLDSIZE_HUGE:		[4,5,6],
		6: 									[5,6,7],
		7: 									[5,6,7]
	}
# Rise of Mankind 2.53
	map_size = map.getWorldSize()
	#print "	map_size",map_size
	if ( not map_size in worldsizes ):
		grain_list = [4,5,6]
		#print "	grain_list",grain_list
	else:
		grain_list = worldsizes[map_size]
		#print "	grain_list",grain_list
# Rise of Mankind 2.53
	grain_list.reverse()
	grain = grain_list[userInputGrain]

	# Peak density
	peak_list = [65, 70, 77]
	hill_list = [35, 40, 45]
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
			if plotTypes[i] == PlotTypes.PLOT_OCEAN:
				continue # Water plots already set.
			if val >= iPeaksThreshold:
				plotTypes[i] = PlotTypes.PLOT_PEAK
			elif val >= iHillsThreshold and val < iPeaksThreshold:
				plotTypes[i] = PlotTypes.PLOT_HILLS
			else:
				pass

	return plotTypes

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Global Highlands) ...")
	terraingen = MGU.TerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

def addFeatures():
	# Remove all peaks along the coasts, before adding Features, Bonuses, Goodies, etc.
	map = CyMap()
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	for plotIndex in range(iW * iH):
		pPlot = map.plotByIndex(plotIndex)
		if pPlot.isPeak() and pPlot.isCoastal():
			# If a peak is along the coast, change to hills and recalc.
			pPlot.setPlotType(PlotTypes.PLOT_HILLS, true, true)

	# Now add Features.
	NiTextOut("Adding Features (Python Global Highlands) ...")
	featuregen = MGU.FeatureGenerator()
	featuregen.addFeatures()
	return 0

def afterGeneration():
	CvMapGeneratorUtil.placeC2CBonuses()
