##
## PerfectMongoose_v3.0.py
##
## Copyright 2010 Kenny Welch aka LunarMongoose
##
##############################################################################
##
## Bug fixed and adapted for the extra terrains in C2C by AIAndy
##
##############################################################################
##
## Based on:
##
## PerfectWorld_v2.06.py (for Civ4)
##
## Copyright 2007-2010 Rich Marinaccio aka Cephalo
## Used with Permission
##
##############################################################################
##
## And on:
##
## PerfectWorld3_v2.lua (for Civ5)
##
## Copyright 2010 Rich Marinaccio aka Cephalo
## Used with Permission
##

##############################################################################
## Version History (AIAndy)
##############################################################################
## 3.5 Improved bonus placement for C2C
## 3.4 Starting plot finding efficiency increased (still a lot of potential there),
## bug fixed in perlin noise generation
## 3.3 Efficiency increased, C2C specific special bonuses placed
## 3.2 Bug fixes in latitude calculation (wrong integer division), directions,
## geostrophic wind
## Extra terrains added, places all features
## Uses map climate option
##############################################################################
## Version History (LunarMongoose)
##############################################################################
##
## 3.1 - Fuyu's 2.0.6f bonus placement, starting location enhancement, minimum hill
## enforcement and bad feature removal code were added, along with his control
## variables. Reverted some more settings for use with vanilla that were still set for
## my mod. Added allowance in getPlotPotentialValue() for clearing features with
## negative food (ie Jungles), since it already accounted for cleared features with
## tile improvements that require it (only useful in mods). Added StartEra checks to
## verify the tech requirements are met for clearing features in both cases. Agreeing
## with Cephalo, I left the starting location production resource food override
## threshold at half the city plots being workable, rather than two-thirds. Changed
## the TechCityTrade requirement for resource valuation and placement to TechReveal
## since the former makes no sense: if a bonus is visible it enhances the tile, and if
## it isn't visible it doesn't enhance the tile, regardless of whether it can be
## harvested or traded with other cities. Added BonusMaxGroupSize option of -1 for
## setting Fuyu's clump limit based on WorldSize, clarified the description of what
## the 0 option there actually does, and fixed the random bounds. Added a minimum
## value to the StartEra checks to include all Classical resources, improvements and
## clearing abilities in the plot valuations. Changed allowStrategicBonusChance to allow
## any strategic resource (not just Stone or Marble), and the city sweetener to allow
## non-strategic resources - both up through Classical (or later). Adjusted lake and
## river values again, and added separate controls for them for the two climate
## systems. Increased Desert slightly and Plains considerably in the PW2 system.
## Synchronized the PM3/PW2 code substantially more to make future updates easier.
## Scaled temperature from normal linearly down to zero in the top and bottom thirds
## of the map in the PW3 climate system, to get Tundra and Snow in the higher and
## lower latitudes as there should be. Lowered Tundra/Snow temperatures to compensate.
## Increased PW3 Grassland level slightly.
##
## 3.0 - Initial release of LM's Civ4 Port of Civ5's PW3_v2, using PW_2.06 as base.
## The PW2 HeightMap and ClimateMap have been replaced with their PW3 counterparts.
## PW2's high-altitude randomization was removed since it was causing 80-100%
## of land tiles to be Peaks regardless of settings. PW2's SmallMaps were removed
## since the new Perlin Noise landmass generator does not require an oversized map
## to avoid looking bad. The YToXRatio hex grid scale factor has been removed.
## PW2 control variables that are now unused have been removed, and the necessary
## PW3 ones have been added. Values have also been adjusted as needed or desired.
## MeteorCompensationFactor was added to try and help preserve total land percent
## when using Break Pangaeas. The Sea Level menu option has been enabled, and
## mc.SeaLevelFactor was added to support it. The Use menu option has been added,
## and a slightly-modified version of the PW2 ClimateMap was added back in, to
## support having a choice between the PW2 and PW3 climate systems. PW3's north
## and south attenuation were removed. PW2's code that forced Snow to be at higher
## elevation than Tundra, Tundra to be higher than everything else, and Desert to
## not be, well, something... was removed. Plains in the PW3 ClimateMap were
## set to have a null rainfall window and a relatively large temperature window,
## so that they form exclusively as a result of cold deserts; this helps create
## Great Plains type areas. RiverThreshold's dependence on PlainsPercent (via
## PlainsThreshold) was removed so that its value can be set reliably.
##

##############################################################################
## Version History (Cephalo)
##############################################################################
##
## 3v2 - Shrank the map sizes except for huge. Added a better way to adjust river
## lengths. Used the continent art styles in a more diverse way. Cleaned up the
## mountain ranges a bit.
##
## 3v1 - initial release! 11/24/2010
##
## 2.06 - Fixed a few bugs from my minimum hill/maximum bad feature function.
##
## 2.05 - Made maps of standard size and below a bit smaller. Changed the way I
## remove jungle to prevent excessive health problems. Tiles in FC on different
## continents have zero value. Tiles on different continents will not be boosted
## with resources or hills. Water tiles have zero value for non-coastal cities.
## Water tiles will not be boosted with resources for non-coastal cities, land
## tiles will be boosted instead. (lookout Sid's Sushi!)
##
## 2.04 - Changed many percent values to be a percent of land tiles rather than
## total map tiles for easier, more predictable adjustment. Ensured a minimum
## number of hills in a starting fat cross. Disabled the normalizeRemovePeaks
## function a replaced it with a maximum peaks in FC function. Added bonus
## resources to FC depending on player handicap. Added a value bonus for cities
## placed on river sides.
##
## 2.03 - Fixed an initialization problem related to Blue Marble. Added some
## enhanced error handling to help me track down some of the intermittant bugs
## that still remain.
##
## 2.02 - Fixed some problems with monsoons that were creating strange artifacts
## near the tropics. Added an exponential curve to heat loss due to altitude, so
## that jungles can appear more readily without crawling to inappropriate
## latitudes.
##
## 2.01 - Changed the way I handled a vanilla version difference. Added toroidal
## and flat map options. Made tree amount more easily adjustable. Added a variable to
## tune the level of resource bonuses. Changed the rules for fixing tundra/ice next
## to desert. Added altitude noise to the plate map to improve island chains. Added
## a variable to control heat loss due to high altitude. Implimented a new interleaved
## bonus placement scheme so that bonuses are placed individually in random order,
## rather than all of each bonus type at once. Brought back the meteor code from
## PerfectWorld 1 and eliminated the east/west continent divide.
##
## 2.0 - Rebuilt the landmass and climate model using the FaireWeather.py for
## Colonization map script engine. Improved the river system. Fixed some
## old bugs.
##
## 1.13 - Fixed a bug where starting on a goody hut would crash the game.
## Prevented start plots from being on mountain peaks. Changed an internal
## distance calculation from a straight line to a path distance, improving
## start locations somewhat. Created a new tuning variable called
## DesertLowTemp. Since deserts in civ are intended to be hot deserts, this
## variable will prevent deserts from appearing near the poles where the
## desert texture clashes horribly with the tundra texture.
##
## 1.12 - Found a small bug in the bonus placer that gave bonuses a minimum
## of zero, this is why duel size maps were having so much trouble.
##
## 1.11 - limited the features mixing with bonuses to forests only. This
## eliminates certain undesireable effects like floodplains being erased by
## or coinciding with oil or incense, or corn appearing in jungle.
##
## 1.10 - Wrapped all map constants into a class to avoid all those
## variables being loaded up when PW is not used. Also this makes it a
## little easier to change them programatically. Added two in-game options,
## New World Rules and Pangaea Rules. Added a tuning variable that allows
## bonuses with a tech requirement to co-exist with features, so that the
## absence of those features does not give away their location.
##
## 1.09 - Fixed a starting placement bug introduced in 1.07. Added a tuning
## variable to turn off 'New world' placement.
##
## 1.08 - Removed the hemispheres logic and replaced it with a simulated meteor
## shower to break up pangeas. Added a tuning variable to allow pangeas.
##
## 1.07 - Placing lakes and harbors after river placement was not updating river
## crossings. Resetting rivers after lake placement should solve this. Fixed a
## small discrepancy between Python randint and mapRand to make them behave the
## same way. Bonuses of the same bonus class, when forced to appear on the
## same continent, were sometimes crowding each other off the map. This was
## especially problematic on the smaller maps. I added some additional, less
## restrictive, passes to ensure that every resource has at least one placement
## unless the random factors decide that none should be placed. Starting plot
## normalization now will place food if a different bonus can not be used due
## to lack of food. Changed heightmap generation to more likely create a
## new world.
##
## 1.06 - Overhauled starting positions and resource placement to better
## suit the peculiarities of PerfectWorld
##
## 1.05 - Fixed the Mac bug and the multi-player bug.
##
## 1.04a - I had unfairly slandered getMapRand in my comments. I had stated
## that the period was shortened unnecessarily, which is not the case.
##
## 1.04 - Added and option to use the superior Python random number generator
## or the getMapRand that civ uses. Made the number of rivers generated tunable.
## Fixed a bug that prevented floodplains on river corners. Made floodplains
## in desert tunable.
##
## 1.03a - very minor change in hope of finding the source of a multi-player
## glitch.
##
## 1.03 - Improved lake generation. Added tuning variables to control some
## new features. Fixed some minor bugs involving the AreaMap filler
## and fixed the issue with oasis appearing on lakes. Maps will now report
## the random seed value that was used to create them, so they can be easily
## re-created for debugging purposes.
##
## 1.02 - Fixed a bug that miscalculated the random placing of deserts. This
## also necessitated a readjustment of the default settings.
##
## 1.01 - Added global tuning variables for easier customization. Fixed a few
## bugs that caused deserts to get out of control.
##

from CvPythonExtensions import *
import BugUtil

from array	import array
from random import random, randint, seed
import math
#import profile


##############################################################################
## GLOBAL TUNING VARIABLES: Change these to customize the map results
##############################################################################

class MapConstants:
	def __init__(self):
		return


	def initialize(self):
		#Percent of land vs. water
		self.landPercent = 0.2889 # LM - Exact Real Earth Value

		#If this variable is set to False, a shower of colossal meteors will attempt to
		#break up any pangea-like continents. Setting this variable to True will allow
		#pangeas to sometimes occur and should be greatly favored by any dinosaurs
		#that might be living on this planet at the time.
		self.AllowPangeas = False

		#This variable sets whether to use the new PW3 climate system or the old PW2 one.
		self.UseNewClimateSystem = True

		#This variable can be used to turn off 'New world' logic and place starting
		#positions anywhere in the world. For some mods, a new world doesn't make
		#sense.
		self.AllowNewWorld = False

		#How many land squares will be above hill threshold and thus 'hills' unless
		#they are also above peak threshold in which case they will be 'peaks'.
		self.HillPercent = 0.42

		#How many land squares will be above peak threshold and thus 'peaks'.
		self.PeakPercent = 0.12

		#---The following variables are not based on percentages. Because temperature
		#---is so strongly tied to latitude, using percentages for things like ice and
		#---tundra leads to very strange results if most of the worlds land lies near
		#---the equator

		#Hotter than this temperature will be considered deciduous forest, colder will
		#be evergreen forest.Temperatures range from coldest 0.0 to hottest 1.0.
		self.ForestTemp = 0.5

		#What temperature will be considered hot enough to be jungle. Temperatures range
		#from coldest 0.0 to hottest 1.0.
		self.JungleTemp = 0.7

		#The chance for ice in the water gets this much lower per plot away from the poles
		self.IceChanceMultiplier = 0.66

		#Sets the threshold for jungle rainfall by modifying the plains threshold by this factor.
		self.TreeFactor = 1.5

		#This is the maximum chance for a tree to be placed when rainfall is above jungle level.
		#use a value between 0.0 and 1.0
		self.MaxTreeChance = 1.0

		#This variable adjusts the amount of bonuses on the map. Values above 1.0 will add bonus
		#bonuses. People often want lots of bonuses, and for those people, this variable is definately
		#a bonus.
		self.BonusBonus = 1.0

		#This value modifies LakeSizePerDrainage when a lake begins in desert
		self.DesertLakeModifier = 0.6

		#This value controls the amount of siltification in lakes
		self.maxSiltPanSize = 0

		#This value sets the minimum altitude of lake depressions. They
		#generally look better higher up.
		self.minLakeAltitude = 0.2

		#The percent chance that an oasis may appear in desert. A tile must be desert and
		#surrounded on all sides by desert.
		self.OasisChance = 0.12

		#Degrees latitude for the top and bottom of the map. This allows
		#for more specific climate zones
		self.topLatitude		= 90
		self.bottomLatitude = -90

		#Horse latitudes and polar fronts plus and minus in case you
		#want some zones to be compressed or emphasized.
		self.horseLatitude			= 30
		self.polarFrontLatitude = 60

		#Tropics of Cancer and Capricorn plus and minus respectively
		self.tropicsLatitude = 23

		#Monsoon uplift factor. This value is an ajustment so that monsoon uplift
		#matches geostrophic uplift.
		self.monsoonUplift = 500.0

		#Controls wrapping (not sure if this makes sense yet)
		self.WrapX = True
		self.WrapY = False

		#Too many meteors will simply destroy the Earth, and just
		#in case the meteor shower can't break the pangaea, this will also
		#prevent and endless loop.
		self.maximumMeteorCount = 15

		#Minimum size for a meteor strike that attemps to break pangaeas.
		#Don't bother to change this it will be overwritten depending on
		#map size.
		self.minimumMeteorSize = 6

		#---These values are for evaluating starting locations

		#Minimum number of hills in fat cross
		self.MinHillsInFC = 3

		#Max number of peaks in fat cross
		self.MaxPeaksInFC = 3

		#Max number of bad features(jungle) in fat cross
		self.MaxBadFeaturesInFC = 4

		#The following values are used for assigning starting locations. For now,
		#they have the same ratio that is found in CvPlot::getFoundValue
		self.CommerceValue	 = 20
		self.ProductionValue = 40
		self.FoodValue			 = 10

		#Coastal cities are important, how important is determined by this
		#value.
		self.CoastalCityValueBonus = 1.3

		#River side cities are also important, how important is determined by this
		#value.
		self.RiverCityValueBonus = 1.2

		#Bonus resources to add depending on difficulty settings
		self.SettlerBonus		= 2
		self.ChieftainBonus = 2
		self.WarlordBonus		= 1
		self.NobleBonus			= 1
		self.PrinceBonus		= 0
		self.MonarchBonus		= 0
		self.EmperorBonus		= 0
		self.ImmortalBonus	= 0
		self.DeityBonus			= 0
		self.NightmareBonus			= 0

		#Decides whether to use the Python random generator or the one that is
		#intended for use with civ maps. The Python random has much higher precision
		#than the civ one. 53 bits for Python result versus 16 for getMapRand. The
		#rand they use is actually 32 bits, but they shorten the result to 16 bits.
		#However, the problem with using the Python random is that it may create
		#syncing issues for multi-player now or in the future, therefore it must
		#be optional.
		self.UsePythonRandom = True

		#Below here are static defines. If you change these, the map won't work.
		#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		self.L	= 0
		self.N	= 1
		self.S	= 2
		self.E	= 3
		self.W	= 4
		self.NE = 5
		self.NW = 6
		self.SE = 7
		self.SW = 8

		self.width	= 72
		self.height = 48

		self.OCEAN = 0
		self.LAND	 = 1
		self.HILLS = 2
		self.PEAK	 = 3

		self.OCEAN	= 0
		self.COAST	= 1
		self.MARSH	= 2
		self.GRASS	= 3
		self.PLAINS = 4
		self.DESERT = 5
		self.TUNDRA = 6
		self.SNOW		= 7
		self.SALT_FLATS = 8
		self.DUNES = 9
		self.SCRUB = 10
		self.ROCKY = 11
		self.BARREN = 12
		self.LUSH = 13
		self.MUDDY = 14
		self.PERMAFROST = 15

		self.minimumLandInChoke = 0.5

		##############################################################################
		## PW3 Settings
		##############################################################################

		self.twistMinFreq = 0.02
		self.twistMaxFreq = 0.12
		self.twistVar			= 0.042
		self.mountainFreq = 0.078

		# Weight of the mountain elevation map versus the coastline elevation map.
		self.mountainWeight = 0.8

		# These set the water temperature compression that creates the land/sea
		# seasonal temperature differences that cause monsoon winds.
		self.minWaterTemp = 0.1
		self.maxWaterTemp = 0.6

		# Strength of geostrophic rainfall versus monsoon rainfall
		self.geostrophicFactor							= 3.0
		self.geostrophicLateralWindStrength = 0.6

		# Wind Zones
		self.NOZONE			= -1
		self.NPOLAR			= 0
		self.NTEMPERATE = 1
		self.NEQUATOR		= 2
		self.SEQUATOR		= 3
		self.STEMPERATE = 4
		self.SPOLAR			= 5

		# Fill in any lakes smaller than this. It looks bad to have large
		# river systems flowing into a tiny lake.
		self.minOceanSize = 50

		# Crazy rain tweaking variables. I wouldn't touch these if I were you.

		# Minimum amount of rain dropped by default before other factors
		# add to the amount of rain dropped
		self.minimumRainCost = 0.0001
		self.upLiftExponent	 = 4
		self.polarRainBoost	 = 0.0

		# These attenuation factors lower the altitude of the map edges. This is
		# currently used to prevent large continents in the uninhabitable polar
		# regions. East/west attenuation is set to zero, but modded maps may
		# have need for them.
		self.northAttenuationFactor = 0.0
		self.northAttenuationRange	= 0.0 #percent of the map height.
		self.southAttenuationFactor = 0.0
		self.southAttenuationRange	= 0.0

		# east west attenuation may be desired for flat maps.
		self.eastAttenuationFactor = 0.0
		self.eastAttenuationRange	 = 0.0 #percent of the map width.
		self.westAttenuationFactor = 0.0
		self.westAttenuationRange	 = 0.0

		##############################################################################
		## PW3 ClimateMap Settings
		##############################################################################

		#What temperature will be considered cold enough to be ice. Temperatures range
		#from coldest 0.0 to hottest 1.0.
		self.SnowTemp3 = 0.0

		#What temperature will be considered cold enough to be tundra. Temperatures range
		#from coldest 0.0 to hottest 1.0.
		self.TundraTemp3 = 0.1
		self.PermafrostTemp3 = 0.05

		# Coldest absolute temperature allowed to be desert, plains if colder.
		# C2C: Currently not active
		self.DesertTemp = 0.48

		#How many land squares will be below desert rainfall threshold. In this case,
		#rain levels close to zero are very likely to be desert, while rain levels close
		#to the desert threshold will more likely be plains.
		self.SaltFlatsPercent3 = 0.02
		self.DunesPercent3 = 0.06
		self.DesertPercent3 = 0.12
		self.ScrubPercent3 = 0.16

		#How many land squares will be below plains rainfall threshold. Rain levels close
		#to the desert threshold are likely to be plains, while those close to the plains
		#threshold are likely to be grassland.
		self.RockyPercent3 = 0.23
		self.BarrenPercent3 = 0.29
		self.PlainsPercent3 = 0.49
		self.GrasslandPercent3 = 0.78
		self.LushPercent3 = 0.9
		self.MuddyPercent3 = 0.96

		#This value controls the number of mid-altitude lake depressions per
		#map square. It will become a lake if enough water flows into the
		#depression.
		self.numberOfLakesPerPlot3 = 0.008

		#How many squares are added to a lake for each unit of drainage flowing into it.
		self.LakeSizePerDrainage3 = 30.0

		#This value is used to decide if enough water has accumulated to form a river.
		#A lower value creates more rivers over the entire map.
		self.RiverThreshold3 = 0.005

		##############################################################################
		## PW2 ClimateMap Settings
		##############################################################################

		#What temperature will be considered cold enough to be ice. Temperatures range
		#from coldest 0.0 to hottest 1.0.
		self.SnowTemp2 = 0.3

		#What temperature will be considered cold enough to be tundra. Temperatures range
		#from coldest 0.0 to hottest 1.0.
		self.TundraTemp2 = 0.4
		self.PermafrostTemp2 = 0.35

		#How many land squares will be below desert rainfall threshold. In this case,
		#rain levels close to zero are very likely to be desert, while rain levels close
		#to the desert threshold will more likely be plains.
		self.SaltFlatsPercent2 = 0.02
		self.DunesPercent2 = 0.06
		self.DesertPercent2 = 0.12
		self.ScrubPercent2 = 0.16

		#How many land squares will be below plains rainfall threshold. Rain levels close
		#to the desert threshold are likely to be plains, while those close to the plains
		#threshold are likely to be grassland.
		self.RockyPercent2 = 0.23
		self.BarrenPercent2 = 0.29
		self.PlainsPercent2 = 0.49
		self.GrasslandPercent2 = 0.78
		self.LushPercent2 = 0.9
		self.MuddyPercent2 = 0.96

		#Oceans are slow to gain and lose heat, so the max and min temps
		#are reduced and raised by this much.
		self.oceanTempClamp = 0.1

		#Filter size for temperature smoothing. Must be odd number
		self.filterSize = 15

		#This sets the amount of heat lost at the highest altitude. 1.0 loses all heat
		#0.0 loses no heat.
		self.heatLostAtOne = 1.0

		#This value is an exponent that controls the curve associated with
		#temperature loss. Higher values create a steeper curve.
		self.temperatureLossCurve = 1.3

		#This value controls the number of mid-altitude lake depressions per
		#map square. It will become a lake if enough water flows into the
		#depression.
		self.numberOfLakesPerPlot2 = 0.003

		#How many squares are added to a lake for each unit of drainage flowing into it.
		self.LakeSizePerDrainage2 = 8.0

		#This value is used to decide if enough water has accumulated to form a river.
		#A lower value creates more rivers over the entire map.
		self.RiverThreshold2 = 0.25

		##############################################################################
		## Mongoose Settings
		##############################################################################

		# Factor to increase mc.landPercent by if incoming meteors are detected, to try
		# and help compensate for the land that will be destroyed. A value like 2.0 will
		# provide full compensation but the results may not look as good.
		self.MeteorCompensationFactor = 1.1

		# Factor to modify mc.landPercent by if a Low or High Sea Level is chosen
		self.SeaLevelFactor = 1.5

		##############################################################################
		## Fuyu Settings
		##############################################################################

		#This variable adjusts the maximun number of identical bonuses to be placed in a
		#single group. People tend not to like all instances of a bonus type to be found within
		#a single 3x3 area. When set to -1 (default), the maximum group size is between 3 and 5,
		#based on WorldSize. When set to 0, the maximum group size is a random number between
		# zero and (number of players). When set to 1, this will disable all bonus grouping.
		self.BonusMaxGroupSize = -1

		#Randomly allows strategic bonuses to be used to sweeten starting positions.
		#(Chance per starting position to allow 1 Classical Era or earlier strategic resource)
		self.allowStrategicBonusChance = 0.0

		#Randomly allows bonuses with continent limiter to be used to sweeting starting positions.
		#(Chance per attempt to place an area-restricted resource in the wrong area)
		self.ignoreAreaRestrictionChance = 0.0


	def initInGameOptions(self):
		gc = CyGlobalContext()
		mmap = gc.getMap()
		# Pangaea Rules
		selectionID = mmap.getCustomMapOption(0)
		if selectionID == 1:
			self.AllowPangeas = not self.AllowPangeas
		# Climate System
		selectionID = mmap.getCustomMapOption(1)
		if selectionID == 1:
			self.UseNewClimateSystem = not self.UseNewClimateSystem
		# Wrap Options
		selectionID = mmap.getCustomMapOption(2)
		if selectionID == 0:
			wrapString = "Cylindrical"
		elif selectionID == 1:
			wrapString = "Toroidal"
			self.WrapY = True
		elif selectionID == 2:
			wrapString = "Flat"
			self.WrapX = False
		else:
			wrapString = "Undefined"
		# New World Rules
		selectionID = mmap.getCustomMapOption(3)
		if selectionID == 1:
			self.AllowNewWorld = not self.AllowNewWorld

		# River threshold
		selectionID = mmap.getCustomMapOption(4)
		if selectionID == 0:
			self.RiverThreshold3 = self.RiverThreshold3 * 1.5
			self.RiverThreshold2 = self.RiverThreshold2 * 1.5
		elif selectionID == 2:
			self.RiverThreshold3 = self.RiverThreshold3 * 0.66
			self.RiverThreshold2 = self.RiverThreshold2 * 0.66

		# Bonuses
		selectionID = mmap.getCustomMapOption(5)
		if selectionID == 0:
			self.BonusBonus = self.BonusBonus * 0.66
		elif selectionID == 2:
			self.BonusBonus = self.BonusBonus * 1.6

		self.optionsString = "Map Options: \n"
		if self.AllowPangeas:
			self.optionsString += "AllowPangeas = True\n"
		else:
			self.optionsString += "AllowPangeas = False\n"
		self.optionsString += "World Wrap = " + wrapString + "\n"
		if self.AllowNewWorld:
			self.optionsString += "AllowNewWorld = True\n"
		else:
			self.optionsString += "AllowNewWorld = False\n"

	def AdaptToClimate(self):
		gc = CyGlobalContext()
		climate = gc.getMap().getClimate()

		if climate == 1: # Tropical, less snow, less desert, more jungle
			self.SnowTemp3 *= 0.7
			self.TundraTemp3 *= 0.7
			self.PermafrostTemp3 *= 0.7
			self.SnowTemp2 *= 0.7
			self.TundraTemp2 *= 0.7
			self.PermafrostTemp2 *= 0.7
			self.JungleTemp *= 0.7
			self.ForestTemp *= 0.7

			self.IceChanceMultiplier = 0.5

			self.SaltFlatsPercent3 *= 0.7
			self.DunesPercent3 *= 0.7
			self.DesertPercent3 *= 0.7
			self.ScrubPercent3 *= 0.7
			self.RockyPercent3 *= 0.75
			self.BarrenPercent3 *= 0.75
			self.PlainsPercent3 *= 0.8
			self.GrasslandPercent3 *= 0.9
			self.LushPercent3 *= 0.9
			self.MuddyPercent3 *= 0.96

			self.SaltFlatsPercent2 *= 0.7
			self.DunesPercent2 *= 0.7
			self.DesertPercent2 *= 0.7
			self.ScrubPercent2 *= 0.7
			self.RockyPercent2 *= 0.75
			self.BarrenPercent2 *= 0.75
			self.PlainsPercent2 *= 0.8
			self.GrasslandPercent2 *= 0.9
			self.LushPercent2 *= 0.9
			self.MuddyPercent2 *= 0.96

			self.horseLatitude			= 38
			self.polarFrontLatitude = 68
			self.tropicsLatitude = 30

		elif climate == 2: # Arid, more desert, less wet plots
			self.SaltFlatsPercent3 = 0.06
			self.DunesPercent3 = 0.14
			self.DesertPercent3 = 0.20
			self.ScrubPercent3 = 0.28
			self.RockyPercent3 = 0.35
			self.BarrenPercent3 = 0.45
			self.PlainsPercent3 = 0.7
			self.GrasslandPercent3 = 0.9
			self.LushPercent3 = 0.95
			self.MuddyPercent3 = 0.98

			self.SaltFlatsPercent2 = 0.06
			self.DunesPercent2 = 0.14
			self.DesertPercent2 = 0.20
			self.ScrubPercent2 = 0.28
			self.RockyPercent2 = 0.35
			self.BarrenPercent2 = 0.45
			self.PlainsPercent2 = 0.7
			self.GrasslandPercent2 = 0.9
			self.LushPercent2 = 0.95
			self.MuddyPercent2 = 0.98

		elif climate == 3: # Rocky, more peaks, hills and rocky terrain
			self.HillPercent *= 1.2
			self.PeakPercent *= 1.3

			self.SaltFlatsPercent3 = 0.03
			self.DunesPercent3 = 0.07
			self.DesertPercent3 = 0.13
			self.ScrubPercent3 = 0.18
			self.RockyPercent3 = 0.38
			self.BarrenPercent3 = 0.47
			self.PlainsPercent3 = 0.65
			self.GrasslandPercent3 = 0.84
			self.LushPercent3 = 0.92
			self.MuddyPercent3 = 0.98

			self.SaltFlatsPercent2 = 0.03
			self.DunesPercent2 = 0.07
			self.DesertPercent2 = 0.13
			self.ScrubPercent2 = 0.18
			self.RockyPercent2 = 0.38
			self.BarrenPercent2 = 0.47
			self.PlainsPercent2 = 0.65
			self.GrasslandPercent2 = 0.84
			self.LushPercent2 = 0.92
			self.MuddyPercent2 = 0.98

		elif climate == 4: # Cold, larger polar area, more cold territory
			self.SnowTemp3 *= 1.3
			self.TundraTemp3 *= 1.3
			self.PermafrostTemp3 *= 1.3
			self.SnowTemp2 *= 1.3
			self.TundraTemp2 *= 1.3
			self.PermafrostTemp2 *= 1.3
			self.JungleTemp *= 1.1
			self.ForestTemp *= 1.15

			self.IceChanceMultiplier = 0.8

			self.horseLatitude			= 29
			self.polarFrontLatitude = 55
			self.tropicsLatitude = 21


mc = MapConstants()


##############################################################################
## PW2 Random
##############################################################################

class PythonRandom:
	def __init__(self):
		return


	def seed(self):
		#Python randoms are not usable in network games.
		#AIAndy: I disagree. Python randoms are deterministic so the only important thing is to seed from a synchronized source like MapRand.
		self.usePR = mc.UsePythonRandom

		GAME = CyGame()
		self.mapRand = GAME.getMapRand()

		if GAME.isNetworkMultiPlayer():
		# seed Python random with MapRand
			seedValue = self.mapRand.get(65535, "Seeding mapRand - FairWeather.py")
			seed(seedValue)
			self.seedString = "Random seed (Using getMapRand) for this map is %(s)20d" % {"s" :seedValue}
		else:
			# Python 'long' has unlimited precision, while the random generator
			# has 53 bits of precision, so I'm using a 53 bit integer to seed the map!
			seed() #Start with system time
			seedValue = randint(0, 9007199254740991)
			seed(seedValue)
			self.seedString = "Random seed (Using Python rands) for this map is %(s)20d" % {"s" :seedValue}


	def random(self):
		if self.usePR:
			return random()
		#This formula is identical to the getFloat function in CvRandom. It
		#is not exposed to Python so I have to recreate it.
		fResult = float(self.mapRand.get(65535, "Getting float -FairWeather.py")) / float(65535)
		return fResult


	def randint(self, rMin, rMax):
		#if rMin and rMax are the same, then return the only option
		if rMin == rMax:
			return rMin
		#returns a number between rMin and rMax inclusive
		if self.usePR:
			return randint(rMin, rMax)
		#mapRand.get() is not inclusive, so we must make it so
		return rMin + self.mapRand.get(rMax + 1 - rMin, "Getting a randint - FairWeather.py")


PRand = PythonRandom()


##############################################################################
## PW3 Interpolation and Perlin Functions
##############################################################################

def CubicInterpolate(v0, v1, v2, v3, mu):
	mu2 = mu * mu
	a0 = v3 - v2 - v0 + v1
	a1 = v0 - v1 - a0
	a2 = v2 - v0
	a3 = v1
	return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3)


def BicubicInterpolate(v, muX, muY):
	a0 = CubicInterpolate(v[1],	 v[2],	v[3],	 v[4],	muX)
	a1 = CubicInterpolate(v[5],	 v[6],	v[7],	 v[8],	muX)
	a2 = CubicInterpolate(v[9],	 v[10], v[11], v[12], muX)
	a3 = CubicInterpolate(v[13], v[14], v[15], v[16], muX)
	return CubicInterpolate(a0, a1, a2, a3, muY)


def CubicDerivative(v0, v1, v2, v3, mu):
	mu2 = mu * mu
	a0 = v3 - v2 - v0 + v1
	a1 = v0 - v1 - a0
	a2 = v2 - v0
	return ((3 * a0 * mu2) + (2 * a1 * mu + a2))


def BicubicDerivative(v, muX, muY):
	a0 = CubicInterpolate(v[1],	 v[2],	v[3],	 v[4],	muX)
	a1 = CubicInterpolate(v[5],	 v[6],	v[7],	 v[8],	muX)
	a2 = CubicInterpolate(v[9],	 v[10], v[11], v[12], muX)
	a3 = CubicInterpolate(v[13], v[14], v[15], v[16], muX)
	return CubicDerivative(a0, a1, a2, a3, muY)


##This function gets a smoothly interpolated value from srcMap.
##x and y are non-integer coordinates of where the value is to
##be calculated, and wrap in both directions. srcMap is an object
##of type FloatMap.
def GetInterpolatedValue(X, Y, srcMap):
	points = {}
	fractionX = X - math.floor(X)
	fractionY = Y - math.floor(Y)
	##wrappedX and wrappedY are set to -1,-1 of the sampled area
	##so that the sample area is in the middle quad of the 4x4 grid
	wrappedX = ((math.floor(X) - 1) % srcMap.rectWidth)	 + srcMap.rectX
	wrappedY = ((math.floor(Y) - 1) % srcMap.rectHeight) + srcMap.rectY
	for pY in range(4):
		y = pY + wrappedY
		for pX in range(4):
			x = pX + wrappedX
			srcIndex = srcMap.GetRectIndex(x, y)
			points[(pY * 4 + pX) + 1] = srcMap.data[int(srcIndex)]
	finalValue = BicubicInterpolate(points, fractionX, fractionY)
	return finalValue


def GetDerivativeValue(X, Y, srcMap):
	points = {}
	fractionX = X - math.floor(X)
	fractionY = Y - math.floor(Y)
	##wrappedX and wrappedY are set to -1,-1 of the sampled area
	##so that the sample area is in the middle quad of the 4x4 grid
	wrappedX = ((math.floor(X) - 1) % srcMap.rectWidth)	 + srcMap.rectX
	wrappedY = ((math.floor(Y) - 1) % srcMap.rectHeight) + srcMap.rectY
	for pY in range(4):
		y = pY + wrappedY
		for pX in range(4):
			x = pX + wrappedX
			srcIndex = srcMap.GetRectIndex(x, y)
			points[(pY * 4) + pX + 1] = srcMap.data[int(srcIndex)]
	finalValue = BicubicDerivative(points, fractionX, fractionY)
	return finalValue


##This function gets Perlin noise for the destination coordinates. Note
##that in order for the noise to wrap, the area sampled on the noise map
##must change to fit each octave.
def GetPerlinNoise(x, y, destMapWidth, destMapHeight, initialFrequency, initialAmplitude, amplitudeChange, octaves, noiseMap):
	finalValue = 0.0
	frequency = initialFrequency
	amplitude = initialAmplitude
	##slight adjustment for seamless wrapping
	for i in range(1, octaves + 1):
		if noiseMap.wrapX:
			noiseMap.rectX = math.floor(noiseMap.width / 2 - (destMapWidth * frequency) / 2)
			noiseMap.rectWidth = max(math.floor(destMapWidth * frequency), 1)
			frequencyX = noiseMap.rectWidth / destMapWidth
		else:
			noiseMap.rectX = 0
			noiseMap.rectWidth = noiseMap.width
			frequencyX = frequency
		if noiseMap.wrapY:
			noiseMap.rectY = math.floor(noiseMap.height / 2 - (destMapHeight * frequency) / 2)
			noiseMap.rectHeight = max(math.floor(destMapHeight * frequency), 1)
			frequencyY = noiseMap.rectHeight / destMapHeight
		else:
			noiseMap.rectY = 0
			noiseMap.rectHeight = noiseMap.height
			frequencyY = frequency
		finalValue = finalValue + GetInterpolatedValue(x * frequencyX, y * frequencyY, noiseMap) * amplitude
		frequency = frequency * 2.0
		amplitude = amplitude * amplitudeChange
	finalValue = finalValue / octaves
	return finalValue


def GetPerlinDerivative(x, y, destMapWidth, destMapHeight, initialFrequency, initialAmplitude, amplitudeChange, octaves, noiseMap):
	finalValue = 0.0
	frequency = initialFrequency
	amplitude = initialAmplitude
	##slight adjustment for seamless wrapping
	for i in range(1, octaves + 1):
		if noiseMap.wrapX:
			noiseMap.rectX = math.floor(noiseMap.width / 2 - (destMapWidth * frequency) / 2)
			noiseMap.rectWidth = math.floor(destMapWidth * frequency)
			frequencyX = noiseMap.rectWidth/destMapWidth
		else:
			noiseMap.rectX = 0
			noiseMap.rectWidth = noiseMap.width
			frequencyX = frequency
		if noiseMap.wrapY:
			noiseMap.rectY = math.floor(noiseMap.height / 2 - (destMapHeight * frequency) / 2)
			noiseMap.rectHeight = math.floor(destMapHeight * frequency)
			frequencyY = noiseMap.rectHeight/destMapHeight
		else:
			noiseMap.rectY = 0
			noiseMap.rectHeight = noiseMap.height
			frequencyY = frequency
		finalValue = finalValue + GetDerivativeValue(x * frequencyX, y * frequencyY, noiseMap) * amplitude
		frequency = frequency * 2.0
		amplitude = amplitude * amplitudeChange
	finalValue = finalValue / octaves
	return finalValue


##############################################################################
## PW3 FloatMap Class
## This is for storing 2D map data. The 'data' field is a zero based, one
## dimensional array. To access map data by x and y coordinates, use the
## GetIndex method to obtain the 1D index, which will handle any needs for
## wrapping in the x and y directions.
##############################################################################

class FloatMap:
	def __init__(self):
		return


	def Initialize(self, width, height, wrapX, wrapY):
		self.width	= width
		self.height = height
		self.wrapX	= wrapX
		self.wrapY	= wrapY
		self.length = self.width * self.height
		##These fields are used to access only a subset of the map
		##with the GetRectIndex function. This is useful for
		##making Perlin noise wrap without generating separate
		##noise fields for each octave
		self.rectX = 0
		self.rectY = 0
		self.rectWidth	= self.width
		self.rectHeight = self.height
		self.data = []
		for i in range(self.length):
			self.data.append(0.0)


	def GetNeighbor(self, x, y, direction):
		xx = x
		yy = y
		if direction == mc.N:
				yy += 1
		elif direction == mc.S:
				yy -= 1
		elif direction == mc.E:
				xx += 1
		elif direction == mc.W:
				xx -= 1
		elif direction == mc.NW:
				yy += 1
				xx -= 1
		elif direction == mc.NE:
				yy += 1
				xx += 1
		elif direction == mc.SW:
				yy -= 1
				xx -= 1
		elif direction == mc.SE:
				yy -= 1
				xx += 1
		return xx,yy


	def GetIndex(self, x, y):
		if self.wrapX:
			xx = x % self.width
		elif x < 0 or x > self.width - 1:
			return -1
		else:
			xx = x
		if self.wrapY:
			yy = y % self.height
		elif y < 0 or y > self.height - 1:
			return -1
		else:
			yy = y
		return yy * self.width + xx


	def GetXYFromIndex(self, i):
		x = i % self.width
		y = (i - x) / self.width
		return x, y


	##quadrants are labeled
	##A B
	##D C
	def GetQuadrant(self, x, y):
		if x < self.width / 2:
			if y < self.height / 2:
				return "A"
			else:
				return "D"
		else:
			if y < self.height / 2:
				return "B"
			else:
				return "C"


	##Gets an index for x and y based on the current
	##rect settings. x and y are local to the defined rect.
	##Wrapping is assumed in both directions
	def GetRectIndex(self, x, y):
		xx = x % self.rectWidth
		yy = y % self.rectHeight
		xx = self.rectX + xx
		yy = self.rectY + yy
		return self.GetIndex(xx, yy)


	def Normalize(self):
		##find highest and lowest values
		maxAlt = -1000.0
		minAlt =	1000.0
		for i in range(self.length):
			alt = self.data[i]
			if alt > maxAlt:
				maxAlt = alt
			if alt < minAlt:
				minAlt = alt
		##subtract minAlt from all values so that
		##all values are zero and above
		for i in range(self.length):
			self.data[i] = self.data[i] - minAlt
		##subract minAlt also from maxAlt
		maxAlt = maxAlt - minAlt
		##determine and apply scaler to whole map
		if maxAlt == 0.0:
			scaler = 0.0
		else:
			scaler = 1.0 / maxAlt
		for i in range(self.length):
			self.data[i] = self.data[i] * scaler


	def GenerateNoise(self):
		for i in range(self.length):
			self.data[i] = PRand.random()


	def GenerateBinaryNoise(self):
		for i in range(self.length):
			if PRand.random() > 0.5:
				self.data[i] = 1
			else:
				self.data[i] = 0


	def FindThresholdFromPercent(self, percent, greaterThan, excludeZeros):
		mapList = []
		percentage = percent * 100
		if greaterThan:
			percentage = 100 - percentage
		if percentage >= 100:
			return 1.01 ##whole map
		elif percentage <= 0:
			return -0.01 ##none of the map
		for i in range(self.length):
			if not (self.data[i] == 0.0 and excludeZeros):
				mapList.append(self.data[i])
		mapList.sort(lambda a, b: cmp(a, b))
		threshIndex = math.floor(((len(mapList) - 1) * percentage) / 100)
		return mapList[int(threshIndex)]


	def GetLatitudeForY(self, y):
		range = mc.topLatitude - mc.bottomLatitude
		return (float(y) * range) / self.height + mc.bottomLatitude


	def GetYForLatitude(self, lat):
		range = mc.topLatitude - mc.bottomLatitude
		return math.floor(((float(lat) - mc.bottomLatitude) * self.height / range) + 0.5)


	def GetZone(self, y):
		if y < 0 or y >= self.height:
			return mc.NOZONE
		lat = self.GetLatitudeForY(y)
		if lat > mc.polarFrontLatitude:
			return mc.NPOLAR
		elif lat >= mc.horseLatitude:
			return mc.NTEMPERATE
		elif lat >= 0.0:
			return mc.NEQUATOR
		elif lat > -mc.horseLatitude:
			return mc.SEQUATOR
		elif lat >= -mc.polarFrontLatitude:
			return mc.STEMPERATE
		else:
			return mc.SPOLAR


	def GetYFromZone(self, zone, bTop):
		if bTop:
			y = self.height - 1
			while y >= 0:
				if zone == self.GetZone(y):
					return y
				y -= 1
		else:
			for y in range(self.height):
				if zone == self.GetZone(y):
					return y
		return -1


	def GetGeostrophicWindDirections(self, zone):
		if zone == mc.NPOLAR:
			return mc.SW, mc.W
		elif zone == mc.NTEMPERATE:
			return mc.NE, mc.E
		elif zone == mc.NEQUATOR:
			return mc.SW, mc.W
		elif zone == mc.SEQUATOR:
			return mc.NW, mc.W
		elif zone == mc.STEMPERATE:
			return mc.SE, mc.E
		else:
			return mc.NW, mc.W
		return -1, -1


	def GetGeostrophicPressure(self, lat):
		if lat > mc.polarFrontLatitude:
			latRange	 = 90.0 - mc.polarFrontLatitude
			latPercent = (lat - mc.polarFrontLatitude) / latRange
			pressure	 = 1.0 - latPercent
		elif lat >= mc.horseLatitude:
			latRange	 = mc.polarFrontLatitude - mc.horseLatitude
			latPercent = (lat - mc.horseLatitude) / latRange
			pressure	 = latPercent
		elif lat >= 0.0:
			latRange	 = mc.horseLatitude - 0.0
			latPercent = (lat - 0.0) / latRange
			pressure	 = 1.0 - latPercent
		elif lat > -mc.horseLatitude:
			latRange	 = 0.0 + mc.horseLatitude
			latPercent = (lat + mc.horseLatitude) / latRange
			pressure	 = latPercent
		elif lat >= -mc.polarFrontLatitude:
			latRange	 = -mc.horseLatitude + mc.polarFrontLatitude
			latPercent = (lat + mc.polarFrontLatitude) / latRange
			pressure	 = 1.0 - latPercent
		else:
			latRange	 = -mc.polarFrontLatitude + 90.0
			latPercent = (lat + 90) / latRange
			pressure	 = latPercent
		return pressure


	def GetAverageInHex(self, x, y, radius):
		list = pb.getCirclePoints(x, y, radius)
		avg = 0.0
		for n in range(len(list)):
			hex = list[n]
			i = self.GetIndex(hex.x, hex.y)
			avg = avg + self.data[i]
		avg = avg / len(list)
		return avg


	def GetStdDevInHex(self, x, y, radius):
		list = pb.getCirclePoints(x, y, radius)
		avg = 0.0
		for n in range(len(list)):
			hex = list[n]
			i = self.GetIndex(hex.x, hex.y)
			avg = avg + self.data[i]
		avg = avg / len(list)
		deviation = 0.0
		for n in range(len(list)):
			hex = list[n]
			i = self.GetIndex(hex.x, hex.y)
			sqr = self.data[i] - avg
			deviation = deviation + (sqr * sqr)
		deviation = math.sqrt(deviation / len(list))
		return deviation


	def Smooth(self, radius):
		dataCopy = {}
		for y in range(self.height):
			for x in range(self.width):
				i = self.GetIndex(x, y)
				dataCopy[i] = self.GetAverageInHex(x, y, radius)
		self.data = dataCopy


	def Deviate(self, radius):
		dataCopy = {}
		for y in range(self.height):
			for x in range(self.width):
				i = self.GetIndex(x, y)
				dataCopy[i] = self.GetStdDevInHex(x, y, radius)
		self.data = dataCopy


	def DumpToDebug(self):
		for i in range(self.length):
			print "%f" % self.data[i]


##############################################################################
## PW2 AreaMap Class
##############################################################################

class AreaMap:
	def __init__(self, width, height, b8connected, bSwitch4Or8OnFalseMatch):
		self.width	= width
		self.height = height
		self.length = self.width * self.height
		self.data		= array('i')
		for i in range(self.length):
			self.data.append(0)
		self.b8connected = b8connected
		self.bSwitch4Or8OnFalseMatch = bSwitch4Or8OnFalseMatch


	def defineAreas(self, matchFunction):
		self.areaList = list()
		areaID = 0
		#make sure map is erased in case it is used multiple times
		for i in range(self.length):
			self.data[i] = 0
		for i in range(self.length):
			if self.data[i] == 0: #not assigned to an area yet
				areaID += 1
				areaSize, match = self.fillArea(i, areaID, matchFunction)
				area = Area(areaID, areaSize, match)
				self.areaList.append(area)


	def getAreaByID(self, areaID):
		for i in range(len(self.areaList)):
			if self.areaList[i].ID == areaID:
				return self.areaList[i]
		return None


	def getOceanID(self):
		self.areaList.sort(lambda x, y:cmp(x.size, y.size))
		self.areaList.reverse()
		for a in self.areaList:
			if a.water == True:
				return a.ID


	def getIndex(self, x, y):
		if mc.WrapX == True:
			xx = x % self.width
		elif x < 0 or x >= self.width:
			return -1
		else:
			xx = x
		if mc.WrapY == True:
			yy = y % self.height
		elif y < 0 or y >= self.height:
			return -1
		else:
			yy = y
		i = yy * self.width + xx
		return i


	def fillArea(self, index, areaID, matchFunction):
		#first divide index into x and y
		y = index / self.width
		x = index % self.width
		#We check 8 neigbors for land,but 4 for water. This is because
		#the game connects land squares diagonally across water, but
		#water squares are not passable diagonally across land
		self.segStack = list()
		self.size = 0
		matchValue = matchFunction(x, y)
		#place seed on stack for both directions
		seg = LineSegment(y, x, x, 1)
		self.segStack.append(seg)
		seg = LineSegment(y + 1, x, x, -1)
		self.segStack.append(seg)
		while(len(self.segStack) > 0):
			seg = self.segStack.pop()
			self.scanAndFillLine(seg,areaID,matchValue,matchFunction)
		return self.size,matchFunction(x, y)


	def scanAndFillLine(self, seg, areaID, matchValue, matchFunction):
		#check for y + dy being off map
		i = self.getIndex(seg.xLeft, seg.y + seg.dy)
		if i < 0:
			return
		debugReport = False
		#for land tiles we must look one past the x extents to include
		#8-connected neighbors
		if self.b8connected:
			if self.bSwitch4Or8OnFalseMatch and matchValue:
				landOffset = 0
			else:
				landOffset = 1
		else:
			if self.bSwitch4Or8OnFalseMatch and matchValue:
				landOffset = 1
			else:
				landOffset = 0
		lineFound = False
		#first scan and fill any left overhang
		if mc.WrapX == True:
			xStop = 0 - (self.width * 20)
		else:
			xStop = -1
		for xLeftExtreme in range(seg.xLeft - landOffset, xStop, -1):
			i = self.getIndex(xLeftExtreme,seg.y + seg.dy)
			if self.data[i] == 0 and matchValue == matchFunction(xLeftExtreme,seg.y + seg.dy):
				self.data[i] = areaID
				self.size += 1
				lineFound = True
			else:
				#if no line was found, then xLeftExtreme is fine, but if
				#a line was found going left, then we need to increment
				#xLeftExtreme to represent the inclusive end of the line
				if lineFound:
					xLeftExtreme += 1
				break
		#now scan right to find extreme right, place each found segment on stack
		xRightExtreme = seg.xLeft #needed sometimes? one time it was not initialized before use.
		if mc.WrapX == True:
			xStop = self.width * 20
		else:
			xStop = self.width
		for xRightExtreme in range(seg.xLeft + lineFound - landOffset,xStop,1):
			i = self.getIndex(xRightExtreme,seg.y + seg.dy)
			if self.data[i] == 0 and matchValue == matchFunction(xRightExtreme,seg.y + seg.dy):
				self.data[i] = areaID
				self.size += 1
				if lineFound == False:
					lineFound = True
					xLeftExtreme = xRightExtreme #starting new line
			elif lineFound == True: #found the right end of a line segment!
				lineFound = False
				#put same direction on stack
				newSeg = LineSegment(seg.y + seg.dy,xLeftExtreme,xRightExtreme - 1,seg.dy)
				self.segStack.append(newSeg)
				#determine if we must put reverse direction on stack
				if xLeftExtreme < seg.xLeft or xRightExtreme >= seg.xRight:
					#out of shadow so put reverse direction on stack also
					newSeg = LineSegment(seg.y + seg.dy,xLeftExtreme,xRightExtreme - 1,-seg.dy)
					self.segStack.append(newSeg)
					if debugReport:
						print "opposite direction to stack",str(newSeg)
				if xRightExtreme >= seg.xRight + landOffset:
					if debugReport:
						print "finished with line"
					break #past the end of the parent line and this line ends
			elif lineFound == False and xRightExtreme >= seg.xRight + landOffset:
				break #past the end of the parent line and no line found
			else:
				continue #keep looking for more line segments
		if lineFound == True: #still a line needing to be put on stack
			if debugReport:
				print "still needing to stack some segs"
			lineFound = False
			#put same direction on stack
			newSeg = LineSegment(seg.y + seg.dy,xLeftExtreme,xRightExtreme - 1,seg.dy)
			self.segStack.append(newSeg)
			if debugReport:
				print str(newSeg)
			#determine if we must put reverse direction on stack
			if xLeftExtreme < seg.xLeft or xRightExtreme - 1 > seg.xRight:
				#out of shadow so put reverse direction on stack also
				newSeg = LineSegment(seg.y + seg.dy,xLeftExtreme,xRightExtreme - 1,-seg.dy)
				self.segStack.append(newSeg)


class LineSegment:
	def __init__(self, y, xLeft, xRight, dy):
		self.y	= y
		self.dy = dy
		self.xLeft	= xLeft
		self.xRight = xRight


	def __str__ (self):
		string = "y = %(y)3d, xLeft = %(xl)3d, xRight = %(xr)3d, dy = %(dy)2d" % {'y':self.y, 'xl':self.xLeft, 'xr':self.xRight, 'dy':self.dy}
		return string


class Area:
	def __init__(self, iD, size, water):
		self.ID		 = iD
		self.size	 = size
		self.water = water


	def __str__(self):
		string = "{ID = %(i)4d, size = %(s)4d, water = %(w)1d}" % {'i':self.ID, 's':self.size, 'w':self.water}
		return string


class AreaPlot:
	def __init__(self, x, y):
		self.x = x
		self.y = y
		self.avgDistance = -1


##############################################################################
## PW3 ElevationMap Class
##############################################################################

class ElevationMap(FloatMap):
	def __init__(self):
		return


	def GenerateElevationMap(self):
		twistMinFreq = 128.0 / mc.width * mc.twistMinFreq ##0.02	/ 128
		twistMaxFreq = 128.0 / mc.width * mc.twistMaxFreq ##0.12	/ 128
		twistVar		 = 128.0 / mc.width * mc.twistVar			##0.042 / 128
		mountainFreq = 128.0 / mc.width * mc.mountainFreq ##0.05	/ 128
		twistMap		 = GenerateTwistedPerlinMap(mc.width, mc.height, mc.WrapX, mc.WrapY, twistMinFreq, twistMaxFreq, twistVar)
		mountainMap	 = GenerateMountainMap(mc.width,			mc.height, mc.WrapX, mc.WrapY, mountainFreq)
		for y in range(self.height):
			for x in range(self.width):
				i = self.GetIndex(x, y)
				tVal = twistMap.data[i]
				tVal = math.pow(math.sin(tVal * math.pi - math.pi * 0.5) * 0.5 + 0.5, 0.25) ##this formula adds a curve flattening the extremes
				self.data[i] = (tVal + ((mountainMap.data[i] * 2.0) - 1.0) * mc.mountainWeight)
		self.Normalize()
		##attenuation should not break normalization
		for y in range(self.height):
			for x in range(self.width):
				i = self.GetIndex(x, y)
				attenuationFactor = GetAttenuationFactor(self, x, y)
				self.data[i] = self.data[i] * attenuationFactor
		land = mc.landPercent
		if not mc.AllowPangeas:
			land *= mc.MeteorCompensationFactor
		gc = CyGlobalContext()
		sea = gc.getSeaLevelInfo(gc.getMap().getSeaLevel()).getSeaLevelChange()
		if sea < 0:
			land *= mc.SeaLevelFactor
		elif sea > 0:
			land /= mc.SeaLevelFactor
		self.seaLevelThreshold = self.FindThresholdFromPercent(land, True, False)


	def IsBelowSeaLevel(self, x, y):
		i = self.GetIndex(x, y)
		if self.data[i] < self.seaLevelThreshold:
			return True
		else:
			return False


	def GetAltitudeAboveSeaLevel(self, x, y):
		i = self.GetIndex(x, y)
		if i == -1:
			return 0.0
		altitude = self.data[i]
		if altitude <= self.seaLevelThreshold:
			return 0.0
		altitude = (altitude - self.seaLevelThreshold) / (1.0 - self.seaLevelThreshold)
		return altitude


	def FillInLakes(self):
		am = AreaMap(self.width, self.height, True, True)
		am.defineAreas(self.IsBelowSeaLevel)
		oceanID = am.getOceanID()
		for y in range(self.height):
			for x in range(self.width):
				if self.IsBelowSeaLevel(x, y):
					i = self.GetIndex(x, y)
					if am.data[i] != oceanID:
						#check the size of this body of water, if too small,
						#change to land
						for a in am.areaList:
							if a.ID == am.data[i] and a.size < mc.minOceanSize:
								self.data[i] = self.seaLevelThreshold


	def GetDifferenceAroundHex(self, x, y):
		avg = self.GetAverageInHex(x, y, 1)
		i = self.GetIndex(x, y)
		return self.data[i] - avg


##############################################################################
## PW2 ClimateMap Class
##############################################################################

class ClimateMap2:
	def __init__(self):
		return


	def createClimateMaps(self):
		summerSunMap				= array('d')
		winterSunMap				= array('d')
		self.summerTempsMap = array('d')
		self.winterTempsMap = array('d')
		self.moistureMap		= array('d')
		self.TemperatureMap = FloatMap()
		self.RainfallMap		= FloatMap()
		self.initializeTempMap(summerSunMap,	mc.tropicsLatitude)
		self.initializeTempMap(winterSunMap, -mc.tropicsLatitude)
		self.TemperatureMap.Initialize(em.width, em.height, em.wrapX, em.wrapY)
		self.RainfallMap.Initialize(em.width,		 em.height, em.wrapX, em.wrapY)
		#smooth both sun maps into the temp maps
		for y in range(em.height):
			for x in range(em.width):
				contributers = 0
				summerAvg		 = 0
				winterAvg		 = 0
				i = GetHmIndex(x, y)
				for yy in range(y - mc.filterSize / 2, y + mc.filterSize / 2 + 1, 1):
					for xx in range(x - mc.filterSize / 2, x + mc.filterSize / 2 + 1, 1):
						ii = GetHmIndex(xx, yy)
						if ii == -1:
							continue
						contributers += 1
						summerAvg += summerSunMap[ii]
						winterAvg += winterSunMap[ii]
				summerAvg = summerAvg / float(contributers)
				winterAvg = winterAvg / float(contributers)
				self.summerTempsMap.append(summerAvg)
				self.winterTempsMap.append(winterAvg)
		#create average temp map
		for y in range(em.height):
			for x in range(em.width):
				i = GetHmIndex(x, y)
				#average summer and winter
				avgTemp = (self.summerTempsMap[i] + self.winterTempsMap[i]) / 2.0
				#cool map for altitude
				self.TemperatureMap.data[i] = avgTemp * (1.0 - pow(em.GetAltitudeAboveSeaLevel(x, y), mc.temperatureLossCurve) * mc.heatLostAtOne)
		#init moisture and rain maps
		for i in range(em.length):
			self.moistureMap.append(0.0)
			self.RainfallMap.data[i] = 0.0
		#create sortable plot list for summer monsoon rains
		temperatureList = list()
		for y in range(em.height):
			for x in range(em.width):
				i = GetHmIndex(x, y)
				rainPlot = RainPlot(x, y, self.summerTempsMap[i], 0)
				temperatureList.append(rainPlot)
		#sort by temperature, coldest first
		temperatureList.sort(lambda x, y:cmp(x.order, y.order))
		#Drop summer monsoon rains
		self.dropRain(temperatureList, self.summerTempsMap, False, None)
		#clear moisture map
		for i in range(em.length):
			self.moistureMap[i] = 0.0
		#create sortable plot list for winter monsoon rains
		temperatureList = list()
		for y in range(em.height):
			for x in range(em.width):
				i = GetHmIndex(x, y)
				rainPlot = RainPlot(x,y,self.winterTempsMap[i], 0)
				temperatureList.append(rainPlot)
		#sort by temperature, coldest first
		temperatureList.sort(lambda x, y:cmp(x.order, y.order))
		#Drop winter monsoon rains
		self.dropRain(temperatureList, self.winterTempsMap, False, None)
		#clear moisture map
		for i in range(em.length):
			self.moistureMap[i] = 0.0
		#set up WindZones class
		wz = WindZones(em.height, mc.topLatitude, mc.bottomLatitude)
		#create ordered list for geostrophic rain
		orderList = list()
		for zone in range(6):
			topY		= wz.GetYFromZone(zone, True)
			bottomY = wz.GetYFromZone(zone, False)
			if topY == -1 and bottomY == -1:
				continue #This wind zone is not represented on this map at all so skip it
			if topY == -1: #top off map edge
				topY = em.height - 1
			if bottomY == -1:
				bottomY = 0
			dx,dy = wz.GetWindDirectionsInZone(zone)
			if dy < 0:
				yStart = topY
				yStop	 = bottomY - 1
			else:
				yStart = bottomY
				yStop	 = topY + 1
			if dx < 0:
				xStart = em.width - 1
				xStop	 = -1
			else:
				xStart = 0
				xStop	 = em.width
			order = 0.0
			for y in range(yStart, yStop, dy):
				for x in range(xStart, xStop, dx):
					rainPlot = RainPlot(x, y, order, abs(yStop - y))
					orderList.append(rainPlot)
					order += 1.0
		#Sort order list
		orderList.sort(lambda x, y:cmp(x.order, y.order))
		#drop geostrophic rain
		self.dropRain(orderList, self.TemperatureMap.data, True, wz)
		NormalizeMap(self.RainfallMap.data, em.width, em.height)


	def dropRain(self, plotList, tempMap, bGeostrophic, windZones):
		countRemaining = len(plotList)
		bDebug = False
		for plot in plotList:
			i = GetHmIndex(plot.x, plot.y)
			#First collect moisture from sea
			if em.IsBelowSeaLevel(plot.x, plot.y):
				self.moistureMap[i] += tempMap[i]
			nList = list()
			if bGeostrophic:
				#make list of neighbors in geostrophic zone, even if off map
				zone = windZones.GetZone(plot.y)
				dx, dy = windZones.GetWindDirectionsInZone(zone)
				nList.append((plot.x, plot.y + dy))
				nList.append((plot.x + dx, plot.y))
				nList.append((plot.x + dx, plot.y + dy))
			else:
				#make list of neighbors with higher temp
				for direction in range(1, 9):
					xx,yy = GetXYFromDirection(plot.x, plot.y, direction)
					ii = GetHmIndex(xx, yy)
					if ii != -1 and tempMap[i] <= tempMap[ii]:
						nList.append((xx, yy))
				#divide moisture by number of neighbors for distribution
				if len(nList) == 0:
					continue #dead end, dump appropriate rain
			moisturePerNeighbor = self.moistureMap[i]/float(len(nList))
			geostrophicFactor = 1.0
			if bGeostrophic:
				geostrophicFactor = mc.geostrophicFactor
			for xx, yy in nList:
				ii = GetHmIndex(xx, yy)
				#Get the rain cost to enter this plot. Cost is
				#percentage of present moisture available for this
				#neighbor
				if bGeostrophic:
					cost = self.getRainCost(plot.x,plot.y,xx,yy,plot.uplift)
				else:
					cost = self.getRainCost(plot.x,plot.y,xx,yy,countRemaining / mc.monsoonUplift)
				#Convert moisture into rain
				#self.moistureMap[i] -= cost * moisturePerNeighbor (this line is unecessary actually, we are finished with moisture map for this plot)
				self.RainfallMap.data[i] += cost * moisturePerNeighbor * geostrophicFactor #geostrophicFactor is not involved with moisture, only to weigh against monsoons
				#send remaining moisture to neighbor
				if ii != -1:
					self.moistureMap[ii] += moisturePerNeighbor - (cost * moisturePerNeighbor)
			countRemaining -= 1


	def getRainCost(self, x1, y1, x2, y2, distanceToUplift):
		cost = mc.minimumRainCost
		cRange = 1.0 - mc.minimumRainCost / 1.0#We don't want to go over 1.0 so the range is reduced
		upliftCost = (1.0 / (float(distanceToUplift) + 1.0)) * cRange
		i	 = GetHmIndex(x1, y1)
		ii = GetHmIndex(x2, y2)
		cost += max((self.TemperatureMap.data[ii] - self.TemperatureMap.data[i]) * 2.0 * cRange, upliftCost)
		return cost


	def initializeTempMap(self, tempMap, tropic):
		for y in range(em.height):
			for x in range(em.width):
				tempMap.append(self.getInitialTemp(x, y, tropic))


	def getInitialTemp(self, x, y, tropic):
		i = GetHmIndex(x, y)
		lat = self.getLatitude(y)
		latRange = float(90 + abs(tropic))
		latDifference = abs(float(lat - tropic))
		aboveSeaLevel = em.data[i] > em.seaLevelThreshold
		if aboveSeaLevel:
			tempPerLatChange = 1.0 / latRange
			temp = 1.0 - (tempPerLatChange * latDifference)
		else:
			tempPerLatChange = (1.0 - (2.0 * mc.oceanTempClamp)) / latRange
			temp = 1.0 - mc.oceanTempClamp - (tempPerLatChange * latDifference)
		return temp


	def getLatitude(self, y):
		latitudeRange = mc.topLatitude - mc.bottomLatitude
		degreesPerDY = float(latitudeRange) / float(em.height)
		if y > em.height:
			return mc.topLatitude
		if y < 0:
			return mc.bottomLatitude
		latitude = (mc.bottomLatitude + (int(round(float(y) * degreesPerDY))))
		return latitude


class RainPlot:
	def __init__(self, x, y, order, uplift):
		self.x = x
		self.y = y
		self.order	= order
		self.uplift = uplift


class WindZones:
	def __init__(self, height, topLat, botLat):
		self.NPOLAR			= 0
		self.NTEMPERATE = 1
		self.NEQUATOR		= 2
		self.SEQUATOR		= 3
		self.STEMPERATE = 4
		self.SPOLAR			= 5
		self.NOZONE			= 99
		self.height = height
		self.topLat = topLat
		self.botLat = botLat


	def GetZone(self, y):
		if y < 0 or y >= self.height:
			return self.NOZONE
		if mc.UseNewClimateSystem:
			cm = c3
		else:
			cm = c2
		lat = cm.getLatitude(y)
		if lat > mc.polarFrontLatitude:
			return self.NPOLAR
		elif lat > mc.horseLatitude:
			return self.NTEMPERATE
		elif lat > 0:
			return self.NEQUATOR
		elif lat > -mc.horseLatitude:
			return self.SEQUATOR
		elif lat > -mc.polarFrontLatitude:
			return self.STEMPERATE
		else:
			return self.SPOLAR


	def GetZoneName(self, zone):
		if zone == self.NPOLAR:
			return "NPOLAR"
		elif zone == self.NTEMPERATE:
			return "NTEMPERATE"
		elif zone == self.NEQUATOR:
			return "NEQUATOR"
		elif zone == self.SEQUATOR:
			return "SEQUATOR"
		elif zone == self.STEMPERATE:
			return "STEMPERATE"
		else:
			return "SPOLAR"


	def GetYFromZone(self, zone, bTop):
		if bTop:
			for y in range(self.height - 1, -1, -1):
				if zone == self.GetZone(y):
					return y
		else:
			for y in range(self.height):
				if zone == self.GetZone(y):
					return y
		return -1


	def GetZoneSize(self):
		latitudeRange = self.topLat - self.botLat
		degreesPerDY = float(latitudeRange) / float(self.height)
		size = 30.0 / degreesPerDY
		return size


	def GetWindDirections(self, y):
		z = self.GetZone(y)
		return self.GetWindDirectionsInZone(z)


	def GetWindDirectionsInZone(self, z):
		#get x,y directions
		if z == self.NPOLAR:
			return (-1, -1)
		elif z == self.NTEMPERATE:
			return (1,	1)
		elif z == self.NEQUATOR:
			return (-1, -1)
		elif z == self.SEQUATOR:
			return (-1, 1)
		elif z == self.STEMPERATE:
			return (1,	-1)
		elif z == self.SPOLAR:
			return (-1, 1)
		return (0, 0)


##############################################################################
## PW3 ClimateMap Class
##############################################################################

class ClimateMap3:
	def __init__(self):
		return


	def GenerateTemperatureMap(self):
		aboveSeaLevelMap		= FloatMap()
		self.summerMap			= FloatMap()
		self.winterMap			= FloatMap()
		self.TemperatureMap = FloatMap()
		aboveSeaLevelMap.Initialize(em.width,		 em.height, em.wrapX, em.wrapY)
		self.summerMap.Initialize(em.width,			 em.height, em.wrapX, em.wrapY)
		self.winterMap.Initialize(em.width,			 em.height, em.wrapX, em.wrapY)
		self.TemperatureMap.Initialize(em.width, em.height, em.wrapX, em.wrapY)
		for y in range(em.height):
			for x in range(em.width):
				i = aboveSeaLevelMap.GetIndex(x, y)
				if em.IsBelowSeaLevel(x, y):
					aboveSeaLevelMap.data[i] = 0.0
				else:
					aboveSeaLevelMap.data[i] = em.data[i] - em.seaLevelThreshold
		aboveSeaLevelMap.Normalize()
		zenith = mc.tropicsLatitude
		topTempLat = mc.topLatitude + zenith
		bottomTempLat = mc.bottomLatitude
		latRange = topTempLat - bottomTempLat
		for y in range(em.height):
			for x in range(em.width):
				i		= self.summerMap.GetIndex(x, y)
				lat = self.summerMap.GetLatitudeForY(y)
				latPercent = (lat - bottomTempLat) / latRange
				temp = (math.sin(latPercent * math.pi * 2.0 - math.pi * 0.5) * 0.5 + 0.5)
				if em.IsBelowSeaLevel(x, y):
					temp = temp * mc.maxWaterTemp + mc.minWaterTemp
				self.summerMap.data[i] = temp
		self.summerMap.Smooth(int(math.floor(em.width / 8.0)))
		self.summerMap.Normalize()
		zenith = -mc.tropicsLatitude
		topTempLat		= mc.topLatitude
		bottomTempLat = mc.bottomLatitude + zenith
		latRange = topTempLat - bottomTempLat
		for y in range(em.height):
			for x in range(em.width):
				i = self.winterMap.GetIndex(x, y)
				lat = self.winterMap.GetLatitudeForY(y)
				latPercent = (lat - bottomTempLat) / latRange
				temp = math.sin(latPercent * math.pi * 2.0 - math.pi * 0.5) * 0.5 + 0.5
				if em.IsBelowSeaLevel(x, y):
					temp = temp * mc.maxWaterTemp + mc.minWaterTemp
				self.winterMap.data[i] = temp
		self.winterMap.Smooth(int(math.floor(em.width / 8.0)))
		self.winterMap.Normalize()
		for y in range(em.height):
			for x in range(em.width):
				i = self.TemperatureMap.GetIndex(x, y)
				self.TemperatureMap.data[i] = (self.winterMap.data[i] + self.summerMap.data[i]) * (1.0 - aboveSeaLevelMap.data[i])
		self.TemperatureMap.Normalize()
		for y in range(em.height):
			if y < em.height / 3:
				for x in range(em.width):
					i = self.TemperatureMap.GetIndex(x, y)
					self.TemperatureMap.data[i] = (self.TemperatureMap.data[i] * y							 * 3.0) / em.height
			elif y > (em.height * 2) / 3:
				for x in range(em.width):
					i = self.TemperatureMap.GetIndex(x, y)
					self.TemperatureMap.data[i] = (self.TemperatureMap.data[i] * (em.height - y) * 3.0) / em.height


	def GenerateRainfallMap(self):
		geoMap								 = FloatMap()
		moistureMap						 = FloatMap()
		rainfallSummerMap			 = FloatMap()
		rainfallWinterMap			 = FloatMap()
		rainfallGeostrophicMap = FloatMap()
		self.RainfallMap			 = FloatMap()
		geoMap.Initialize(em.width,									em.height, em.wrapX, em.wrapY)
		rainfallSummerMap.Initialize(em.width,			em.height, em.wrapX, em.wrapY)
		rainfallWinterMap.Initialize(em.width,			em.height ,em.wrapX, em.wrapY)
		rainfallGeostrophicMap.Initialize(em.width, em.height, em.wrapX, em.wrapY)
		self.RainfallMap.Initialize(em.width,				em.height, em.wrapX, em.wrapY)
		for y in range(em.height):
			lat = em.GetLatitudeForY(y)
			pressure = em.GetGeostrophicPressure(lat)
			for x in range(em.width):
				i	= em.GetIndex(x, y)
				geoMap.data[i] = pressure

		#print("Geo map:")
		#geoMap.DumpToDebug()

		geoMap.Normalize()
		sortedSummerMap = []
		sortedWinterMap = []
		for y in range(em.height):
			for x in range(em.width):
				i = em.GetIndex(x, y)
				sortedSummerMap.append((x, y, self.summerMap.data[i]))
				sortedWinterMap.append((x, y, self.winterMap.data[i]))
		sortedSummerMap.sort(lambda a,b: cmp(a[2], b[2]))
		sortedWinterMap.sort(lambda a,b: cmp(a[2], b[2]))
		sortedGeoMap = []
		for i in range(em.length):
			sortedGeoMap.append((0, 0, 0.0))
		xStart = 0
		xStop	 = 0
		yStart = 0
		yStop	 = 0
		incX	 = 0
		incY	 = 0
		geoIndex = 0
		for zone in range(6):
			topY		= em.GetYFromZone(zone, True)
			bottomY = em.GetYFromZone(zone, False)
			if not (topY == -1 and bottomY == -1):
				if topY == -1:
					topY = em.height - 1
				if bottomY == -1:
					bottomY = 0
				dir1, dir2 = em.GetGeostrophicWindDirections(zone)
				if (dir1 == mc.SW) or (dir1 == mc.SE):
					yStart = topY
					yStop = bottomY - 1 ##- 1
					incY = -1
				else:
					yStart = bottomY
					yStop = topY + 1 ##+ 1
					incY = 1
				if dir2 == mc.W:
					xStart = em.width - 1
					xStop = -1 ##-1
					incX = -1
				else:
					xStart = 0
					xStop = em.width
					incX = 1
				#y = yStart
				#while y <= yStop:
				for y in range(yStart, yStop, incY):
					##each line should start on water to avoid vast areas without rain
					xxStart = xStart
					xxStop	= xStop
					#xx = xStart
					#while xx <= xStop - incX:
					for x in range(xStart, xStop, incX):
						#i = em.GetIndex(x, y)
						if em.IsBelowSeaLevel(x, y):
							xxStart = x
							xxStop = x + em.width * incX
							break
						#xx += incX
					#x = xxStart
					#while x <= xxStop - incX:
					for x in range(xxStart, xxStop, incX):
						i = em.GetIndex(x, y)
						sortedGeoMap[geoIndex] = (x, y, geoMap.data[i])
						geoIndex += 1
						#x += incX
					#y += incY

		moistureMap.Initialize(em.width, em.height, em.wrapX, em.wrapY)
		for i in range(len(sortedSummerMap)):
			x = sortedSummerMap[i][0]
			y = sortedSummerMap[i][1]
			pressure = sortedSummerMap[i][2]
			self.DistributeRain(x, y, self.summerMap, rainfallSummerMap, moistureMap, False)
		moistureMap.Initialize(em.width, em.height, em.wrapX, em.wrapY)
		for i in range(len(sortedWinterMap)):
			x = sortedWinterMap[i][0]
			y = sortedWinterMap[i][1]
			pressure = sortedWinterMap[i][2]
			self.DistributeRain(x, y, self.winterMap, rainfallWinterMap, moistureMap, False)

		moistureMap.Initialize(em.width, em.height, em.wrapX, em.wrapY)
		for i in range(len(sortedGeoMap)):
			x = sortedGeoMap[i][0]
			y = sortedGeoMap[i][1]
			self.DistributeRain(x, y, geoMap, rainfallGeostrophicMap, moistureMap, True)

		#print("Summer temperature map:")
		#self.summerMap.DumpToDebug()
		#print("Winter temperature map:")
		#self.winterMap.DumpToDebug()

		#print("Summer rainfall map:")
		#rainfallSummerMap.DumpToDebug()
		#print("Winter rainfall map:")
		#rainfallWinterMap.DumpToDebug()
		#print("Geostrophic rainfall map:")
		#rainfallGeostrophicMap.DumpToDebug()

		##zero below sea level for proper percent threshold finding
		for y in range(em.height):
			for x in range(em.width):
				i = em.GetIndex(x, y)
				if em.IsBelowSeaLevel(x, y):
					rainfallSummerMap.data[i]			 = 0.0
					rainfallWinterMap.data[i]			 = 0.0
					rainfallGeostrophicMap.data[i] = 0.0
		rainfallSummerMap.Normalize()
		rainfallWinterMap.Normalize()
		rainfallGeostrophicMap.Normalize()

		for y in range(em.height):
			for x in range(em.width):
				i = em.GetIndex(x, y)
				self.RainfallMap.data[i] = rainfallSummerMap.data[i] + rainfallWinterMap.data[i] + (rainfallGeostrophicMap.data[i] * mc.geostrophicFactor)
		self.RainfallMap.Normalize()


	def DistributeRain(self, x, y, pressureMap, RainfallMap, moistureMap, boolGeostrophic):
		i = em.GetIndex(x, y)
		upLiftSource = max(math.pow(pressureMap.data[i], mc.upLiftExponent), 1.0 - self.TemperatureMap.data[i])
		if em.IsBelowSeaLevel(x, y):
			moistureMap.data[i] = max(moistureMap.data[i], self.TemperatureMap.data[i])
		##make list of neighbors
		nList = []
		if boolGeostrophic:
			zone = em.GetZone(y)
			dir1, dir2 = em.GetGeostrophicWindDirections(zone)
			x1, y1 = em.GetNeighbor(x,y,dir1)
			ii = em.GetIndex(x1,y1)
			##neighbor must be on map and in same wind zone
			if ii >= 0 and (em.GetZone(y1) == em.GetZone(y)):
				nList.append((x1, y1))
			x2, y2 = em.GetNeighbor(x, y, dir2)
			ii = em.GetIndex(x2, y2)
			if ii >= 0:
				nList.append((x2, y2))
		else:
			for dir in range(1, 9):
				xx, yy = em.GetNeighbor(x, y, dir)
				ii = em.GetIndex(xx, yy)
				if ii >= 0 and pressureMap.data[i] <= pressureMap.data[ii]:
					nList.append((xx, yy))
		if len(nList) == 0 or (boolGeostrophic and len(nList) == 1):
			cost = moistureMap.data[i]
			RainfallMap.data[i] = cost
			return
		moisturePerNeighbor = moistureMap.data[i] / float(len(nList))
		##drop rain and pass moisture to neighbors
		for n in range(len(nList)):
			xx = nList[n][0]
			yy = nList[n][1]
			ii = em.GetIndex(xx, yy)
			upLiftDest = max(math.pow(pressureMap.data[ii], mc.upLiftExponent), 1.0 - self.TemperatureMap.data[ii])
			cost = self.GetRainCost(upLiftSource, upLiftDest)
			bonus = 0.0
			if (em.GetZone(y) == mc.NPOLAR or em.GetZone(y) == mc.SPOLAR):
				bonus = mc.polarRainBoost
			if boolGeostrophic and len(nList) == 2:
				if n == 1:
					moisturePerNeighbor = (1.0 - mc.geostrophicLateralWindStrength) * moistureMap.data[i]
				else:
					moisturePerNeighbor = mc.geostrophicLateralWindStrength * moistureMap.data[i]
			RainfallMap.data[i] = RainfallMap.data[i] + cost * moisturePerNeighbor + bonus
			##pass to neighbor.
			moistureMap.data[ii] = moistureMap.data[ii] + moisturePerNeighbor - (cost * moisturePerNeighbor)


	def GetRainCost(self, upLiftSource, upLiftDest):
		cost = mc.minimumRainCost
		cost = max(mc.minimumRainCost, cost + upLiftDest - upLiftSource)
		if cost < 0.0:
			cost = 0.0
		return cost


##############################################################################
## Mongoose TerrainMap Class
##############################################################################

class TerrainMap:
	def __init__(self):
		return


	def Initialize(self):
		self.data1 = []
		self.data2 = []
		for i in range(em.length):
			self.data1.append(mc.OCEAN)
			self.data2.append(mc.OCEAN)


	def GeneratePlotMap(self):
		print "creating plot map"
		#create height difference map to allow for tuning
		diffMap = []
		for i in range(em.length):
			diffMap.append(0.0)
		#I tried using a deviation from surrounding average altitude
		#to determine hills and peaks but I didn't like the
		#results. Therefore I an using lowest neighbor
		for y in range(mc.height):
			for x in range(mc.width):
				i = em.GetIndex(x, y)
				myAlt = em.data[i]
				minAlt = 1.0
				for direction in range(1, 9):
					xx, yy = GetXYFromDirection(x, y, direction)
					ii = em.GetIndex(xx, yy)
					if ii == -1:
						continue
					if em.data[ii] < minAlt:
						minAlt = em.data[ii]
				diffMap[i] = myAlt - minAlt
		NormalizeMap(diffMap, mc.width, mc.height)
		#zero out water tiles so percent is percent of land
		for y in range(mc.height):
			for x in range(mc.width):
				i = em.GetIndex(x, y)
				if em.IsBelowSeaLevel(x, y):
					diffMap[i] = 0.0
		hillHeight = FindValueFromPercent(diffMap, mc.width, mc.height, mc.HillPercent, mc.HillPercent / 10.0, True)
		peakHeight = FindValueFromPercent(diffMap, mc.width, mc.height, mc.PeakPercent, mc.PeakPercent / 10.0, True)
		for y in range(mc.height):
			for x in range(mc.width):
				i = em.GetIndex(x, y)
				altDiff = diffMap[i]
				if em.data[i] < em.seaLevelThreshold:
					self.data1[i] = mc.OCEAN
				elif altDiff < hillHeight:
					self.data1[i] = mc.LAND
				elif altDiff < peakHeight:
					self.data1[i] = mc.HILLS
				else:
					self.data1[i] = mc.PEAK
		#break up large clusters of hills and peaks
		for y in range(mc.height):
			for x in range(mc.width):
				i = em.GetIndex(x, y)
				if self.data1 == mc.HILLS:
					allHills = True
					for direction in range(1, 9):
						xx,yy = GetXYFromDirection(x,y,direction)
						ii = em.GetIndex(xx,yy)
						if self.data1[ii] != mc.HILLS:
							allHills = False
					if allHills == True:
						self.data1[i] = mc.LAND
				if self.data1 == mc.PEAK:
					allPeaks = True
					for direction in range(1, 9):
						xx,yy = GetXYFromDirection(x,y,direction)
						ii = em.GetIndex(xx, yy)
						if self.data1[ii] != mc.PEAK:
							allPeaks = False
					if allPeaks == True:
						self.data1[i] = mc.HILLS


	def GenerateTerrainMap(self):
		#Find minimum rainfall on land
		minRain = 10.0
		if mc.UseNewClimateSystem:
			cm = c3
			snowTemp			= mc.SnowTemp3
			permafrostTemp = mc.PermafrostTemp3
			tundraTemp		= mc.TundraTemp3
			DesertPercent = mc.DesertPercent3
			SaltFlatsPercent = mc.SaltFlatsPercent3
			PlainsPercent = mc.PlainsPercent3
			DunesPercent = mc.DunesPercent3
			ScrubPercent = mc.ScrubPercent3
			RockyPercent = mc.RockyPercent3
			BarrenPercent = mc.BarrenPercent3
			GrasslandPercent = mc.GrasslandPercent3
			LushPercent = mc.LushPercent3
			MuddyPercent = mc.MuddyPercent3
		else:
			cm = c2
			snowTemp			= mc.SnowTemp2
			permafrostTemp = mc.PermafrostTemp2
			tundraTemp		= mc.TundraTemp2
			DesertPercent = mc.DesertPercent2
			SaltFlatsPercent = mc.SaltFlatsPercent2
			PlainsPercent = mc.PlainsPercent2
			DunesPercent = mc.DunesPercent2
			ScrubPercent = mc.ScrubPercent2
			RockyPercent = mc.RockyPercent2
			BarrenPercent = mc.BarrenPercent2
			GrasslandPercent = mc.GrasslandPercent2
			LushPercent = mc.LushPercent2
			MuddyPercent = mc.MuddyPercent2

		#print("Rainfall map:")
		#cm.RainfallMap.DumpToDebug()

		for i in range(em.length):
			if self.data1[i] != mc.OCEAN:
				if cm.RainfallMap.data[i] < minRain:
					minRain = cm.RainfallMap.data[i]
		#zero water tiles to obtain percent of land
		for y in range(mc.height):
			for x in range(mc.width):
				i = GetIndex(x, y)
				#if em.IsBelowSeaLevel(x, y):
				if self.data1[i] == mc.OCEAN or cm.TemperatureMap.data[i] < tundraTemp:
					cm.RainfallMap.data[i] = 0.0

		cm.RainfallMap.Normalize()
		self.desertThreshold = FindValueFromPercent(cm.RainfallMap.data,mc.width,mc.height,DesertPercent,.001,False)
		self.saltflatsThreshold = FindValueFromPercent(cm.RainfallMap.data,mc.width,mc.height,SaltFlatsPercent,.001,False)
		self.dunesThreshold = FindValueFromPercent(cm.RainfallMap.data,mc.width,mc.height,DunesPercent,.001,False)
		self.scrubThreshold = FindValueFromPercent(cm.RainfallMap.data,mc.width,mc.height,ScrubPercent,.001,False)
		self.plainsThreshold = FindValueFromPercent(cm.RainfallMap.data,mc.width,mc.height,PlainsPercent,.001,False)
		self.rockyThreshold = FindValueFromPercent(cm.RainfallMap.data,mc.width,mc.height,RockyPercent,.001,False)
		self.barrenThreshold = FindValueFromPercent(cm.RainfallMap.data,mc.width,mc.height,BarrenPercent,.001,False)
		self.grasslandThreshold = FindValueFromPercent(cm.RainfallMap.data,mc.width,mc.height,GrasslandPercent,.001,False)
		self.lushThreshold = FindValueFromPercent(cm.RainfallMap.data,mc.width,mc.height,LushPercent,.001,False)
		self.muddyThreshold = FindValueFromPercent(cm.RainfallMap.data,mc.width,mc.height,MuddyPercent,.001,False)

		for y in range(mc.height):
			for x in range(mc.width):
				i = GetIndex(x, y)
				if self.data1[i] == mc.OCEAN:
					self.data2[i] = mc.OCEAN
					for direction in range (1, 9):
						xx, yy = GetXYFromDirection(x, y, direction)
						ii = GetIndex(xx, yy)
						if ii == -1:
							continue
						if self.data1[ii] != mc.OCEAN:
							self.data2[i] = mc.COAST

				elif cm.TemperatureMap.data[i] < snowTemp:
						self.data2[i] = mc.SNOW
				elif cm.TemperatureMap.data[i] < permafrostTemp:
						self.data2[i] = mc.PERMAFROST
				elif cm.TemperatureMap.data[i] < tundraTemp:
						self.data2[i] = mc.TUNDRA

				#instead of harsh thresholds, allow a random deviation chance
				#based on how close to the threshold the rainfall is
				elif cm.RainfallMap.data[i] < self.saltflatsThreshold:
						if cm.RainfallMap.data[i] < ((1 + PRand.random()) * (self.saltflatsThreshold - minRain))/2.0 + minRain:
								self.data2[i] = mc.SALT_FLATS
						else:
								self.data2[i] = mc.DUNES

				elif cm.RainfallMap.data[i] < self.dunesThreshold:
						if cm.RainfallMap.data[i] < ((1 + PRand.random()) * (self.dunesThreshold - self.saltflatsThreshold))/2.0 + self.saltflatsThreshold:
								self.data2[i] = mc.DUNES
						else:
								self.data2[i] = mc.DESERT

				elif cm.RainfallMap.data[i] < self.desertThreshold:
						if cm.RainfallMap.data[i] < ((1 + PRand.random()) * (self.desertThreshold - self.dunesThreshold))/2.0 + self.dunesThreshold:
								self.data2[i] = mc.DESERT
						else:
								self.data2[i] = mc.SCRUB

				elif cm.RainfallMap.data[i] < self.scrubThreshold:
						if cm.RainfallMap.data[i] < ((1 + PRand.random()) * (self.scrubThreshold - self.desertThreshold))/2.0 + self.desertThreshold:
								self.data2[i] = mc.SCRUB
						else:
								self.data2[i] = mc.ROCKY

				elif cm.RainfallMap.data[i] < self.rockyThreshold:
						if cm.RainfallMap.data[i] < ((1 + PRand.random()) * (self.rockyThreshold - self.scrubThreshold))/2.0 + self.scrubThreshold:
								self.data2[i] = mc.ROCKY
						else:
								self.data2[i] = mc.BARREN

				elif cm.RainfallMap.data[i] < self.barrenThreshold:
						if cm.RainfallMap.data[i] < ((1 + PRand.random()) * (self.barrenThreshold - self.rockyThreshold))/2.0 + self.rockyThreshold:
								self.data2[i] = mc.BARREN
						else:
								self.data2[i] = mc.PLAINS

				elif cm.RainfallMap.data[i] < self.plainsThreshold:
						if cm.RainfallMap.data[i] < ((1 + PRand.random()) * (self.plainsThreshold - self.barrenThreshold))/2.0 + self.barrenThreshold:
								self.data2[i] = mc.PLAINS
						else:
								self.data2[i] = mc.GRASS

				elif cm.RainfallMap.data[i] < self.grasslandThreshold:
						if cm.RainfallMap.data[i] < ((1 + PRand.random()) * (self.grasslandThreshold - self.plainsThreshold))/2.0 + self.plainsThreshold:
								self.data2[i] = mc.GRASS
						else:
								self.data2[i] = mc.LUSH

				elif cm.RainfallMap.data[i] < self.lushThreshold:
						if cm.RainfallMap.data[i] < ((1 + PRand.random()) * (self.lushThreshold - self.grasslandThreshold))/2.0 + self.grasslandThreshold:
								self.data2[i] = mc.LUSH
						else:
								self.data2[i] = mc.MUDDY

				elif cm.RainfallMap.data[i] < self.muddyThreshold:
						if cm.RainfallMap.data[i] < ((1 + PRand.random()) * (self.muddyThreshold - self.lushThreshold))/2.0 + self.lushThreshold:
								self.data2[i] = mc.MUDDY
						else:
								self.data2[i] = mc.MARSH

				else:
						self.data2[i] = mc.MARSH


##############################################################################
## PW3 Global Functions
##############################################################################

def GenerateTwistedPerlinMap(width, height, wrapX, wrapY, minFreq, maxFreq, varFreq):
	inputNoise = FloatMap()
	freqMap		 = FloatMap()
	twistMap	 = FloatMap()
	inputNoise.Initialize(width, height, wrapX, wrapY)
	freqMap.Initialize(width,		 height, wrapX, wrapY)
	twistMap.Initialize(width,	 height, wrapX, wrapY)
	inputNoise.GenerateNoise()
	inputNoise.Normalize()
	for y in range(freqMap.height):
		for x in range(freqMap.width):
			i = freqMap.GetIndex(x, y)
			freqMap.data[i] = GetPerlinNoise(x, y, freqMap.width, freqMap.height, varFreq, 1.0, 0.1, 8, inputNoise)
	freqMap.Normalize()
	for y in range(twistMap.height):
		for x in range(twistMap.width):
			i = twistMap.GetIndex(x,y)
			freq = freqMap.data[i] * (maxFreq - minFreq) + minFreq
			mid = (maxFreq - minFreq) / 2 + minFreq
			coordScale = freq / mid
			offset = (1.0 - coordScale) / mid
			ampChange = 0.85 - (freqMap.data[i] * 0.5)
			twistMap.data[i] = GetPerlinNoise(x + offset, (y + offset), twistMap.width, twistMap.height, mid, 1.0, ampChange, 8, inputNoise)
	twistMap.Normalize()
	return twistMap


def GenerateMountainMap(width, height, wrapX, wrapY, initFreq):
	inputNoise	= FloatMap()
	inputNoise2 = FloatMap()
	mountainMap = FloatMap()
	stdDevMap		= FloatMap()
	noiseMap		= FloatMap()
	moundMap		= FloatMap()
	inputNoise.Initialize(width,	height, wrapX, wrapY)
	inputNoise2.Initialize(width, height, wrapX, wrapY)
	mountainMap.Initialize(width, height, wrapX, wrapY)
	stdDevMap.Initialize(width,		height, wrapX, wrapY)
	noiseMap.Initialize(width,		height, wrapX, wrapY)
	moundMap.Initialize(width,		height, wrapX, wrapY)
	inputNoise.GenerateBinaryNoise()
	inputNoise.Normalize()
	inputNoise2.GenerateNoise()
	inputNoise2.Normalize()
	for y in range(mountainMap.height):
		for x in range(mountainMap.width):
			i = mountainMap.GetIndex(x, y)
			mountainMap.data[i] = GetPerlinNoise(x, y, mountainMap.width, mountainMap.height, initFreq, 1.0, 0.4, 8, inputNoise)
			noiseMap.data[i]		= GetPerlinNoise(x, y, mountainMap.width, mountainMap.height, initFreq, 1.0, 0.4, 8, inputNoise2)
			stdDevMap.data[i] = mountainMap.data[i]
	mountainMap.Normalize()
	stdDevMap.Deviate(7)
	stdDevMap.Normalize()
	noiseMap.Normalize()
	for y in range(mountainMap.height):
		for x in range(mountainMap.width):
			i = mountainMap.GetIndex(x, y)
			val = mountainMap.data[i]
			moundMap.data[i] = (math.sin(val * math.pi * 2.0 - math.pi * 0.5) * 0.5 + 0.5) * GetAttenuationFactor(mountainMap, x, y)
			if val < 0.5:
				val = val * 4.0
			else:
				val = (1.0 - val) * 4.0
			mountainMap.data[i] = moundMap.data[i]
	mountainMap.Normalize()
	for y in range(mountainMap.height):
		for x in range(mountainMap.width):
			i = mountainMap.GetIndex(x, y)
			val = mountainMap.data[i]
			mountainMap.data[i] = math.pow(math.pow(math.sin(val * 3.0 * math.pi + math.pi * 0.5), 16) * val, 0.5)
			if mountainMap.data[i] > 0.2:
				mountainMap.data[i] = 1.0
			else:
				mountainMap.data[i] = 0.0
	land = mc.landPercent
	if not mc.AllowPangeas:
		land *= mc.MeteorCompensationFactor
	gc = CyGlobalContext()
	sea = gc.getSeaLevelInfo(gc.getMap().getSeaLevel()).getSeaLevelChange()
	if sea < 0:
		land *= mc.SeaLevelFactor
	elif sea > 0:
		land /= mc.SeaLevelFactor
	stdDevThreshold = stdDevMap.FindThresholdFromPercent(land, True, False)
	for y in range(mountainMap.height):
		for x in range(mountainMap.width):
			i = mountainMap.GetIndex(x, y)
			val = mountainMap.data[i]
			dev = 2.0 * stdDevMap.data[i] - 2.0 * stdDevThreshold
			mountainMap.data[i] = (val + moundMap.data[i]) * dev
	mountainMap.Normalize()
	return mountainMap


def GetAttenuationFactor(map, x, y):
	southY		 = map.height * mc.southAttenuationRange
	southRange = map.height * mc.southAttenuationRange
	yAttenuation = 1.0
	if y < southY:
		yAttenuation = mc.southAttenuationFactor + (y / southRange) * (1.0 - mc.southAttenuationFactor)
	northY		 = map.height - (map.height * mc.northAttenuationRange)
	northRange = map.height * mc.northAttenuationRange
	if y > northY:
		yAttenuation = mc.northAttenuationFactor + ((map.height - y) / northRange) * (1.0 - mc.northAttenuationFactor)
	eastY			= map.width - (map.width * mc.eastAttenuationRange)
	eastRange = map.width * mc.eastAttenuationRange
	xAttenuation = 1.0
	if x > eastY:
		xAttenuation = mc.eastAttenuationFactor + ((map.width - x) / eastRange) * (1.0 - mc.eastAttenuationFactor)
	westY			= map.width * mc.westAttenuationRange
	westRange = map.width * mc.westAttenuationRange
	if x < westY:
		xAttenuation = mc.westAttenuationFactor + (x / westRange) * (1.0 - mc.westAttenuationFactor)
	return yAttenuation * xAttenuation


################################################################################
## PW2 Global Functions
################################################################################

def errorPopUp(message):
	gc = CyGlobalContext()
	iPlayerNum = 0
	for iPlayer in range(gc.getMAX_PLAYERS()):
		player = gc.getPlayer(iPlayer)
		if player.isAlive():
			iPlayerNum = iPlayerNum + 1
			if player.isHuman():
				text = message + "\n\n" + mc.optionsString + "\n" + PRand.seedString
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setText(text)
				popupInfo.setOnClickedPythonCallback("")
				popupInfo.addPythonButton("Ok","")
				popupInfo.addPopup(iPlayer)


def GetIndex(x, y):
	if mc.WrapX == True:
		xx = x % mc.width
	elif x < 0 or x >= mc.width:
		return -1
	else:
		xx = x
	if mc.WrapY == True:
		yy = y % mc.height
	elif y < 0 or y >= mc.height:
		return -1
	else:
		yy = y
	i = (yy * mc.width) + xx
	return i


def GetHmIndex(x, y):
	if mc.WrapX == True:
		xx = x % mc.width
	elif x < 0 or x >= mc.width:
		return -1
	else:
		xx = x
	if mc.WrapY == True:
		yy = y % mc.height
	elif y < 0 or y >= mc.height:
		return -1
	else:
		yy = y
	i = (yy * mc.width) + xx
	return i


def GetIndexGeneral(x, y, width, height):
	if mc.WrapX == True:
		xx = x % width
	elif x < 0 or x >= width:
		return -1
	else:
		xx = x
	if mc.WrapY == True:
		yy = y % height
	elif y < 0 or y >= height:
		return -1
	else:
		yy = y
	i = (yy * width) + xx
	return i


def NormalizeMap(fMap, width, height):
	maxAlt = 0.0
	minAlt = 0.0
	for y in range(height):
		for x in range(width):
			plot = fMap[GetIndexGeneral(x,y,width,height)]
			if plot > maxAlt:
				maxAlt = plot
			elif plot < minAlt:
				minAlt = plot
	if minAlt != 0.0:
		for y in range(height):
			for x in range(width):
				fMap[GetIndexGeneral(x,y,width,height)] -= minAlt
		maxAlt -= minAlt
	scaler = 1.0 / maxAlt
	for y in range(height):
		for x in range(width):
			fMap[GetIndexGeneral(x,y,width,height)] = fMap[GetIndexGeneral(x,y,width,height)] * scaler


def GetWeight(x, y, xx, yy, xScale, yScale):
	xWeight = 1.0
	if float(xx) < x * xScale:
		xWeight = 1.0 - ((x * xScale) - float(xx))
	elif float(xx + 1) > (x + 1) * xScale:
		xWeight = ((x + 1) * xScale) - float(xx)
	yWeight = 1.0
	if float(yy) < y * yScale:
		yWeight = 1.0 - ((y * yScale) - float(yy))
	elif float(yy + 1) > (y + 1) * yScale:
		yWeight = ((y + 1) * yScale) - float(yy)
	return xWeight * yWeight


def AngleDifference(a1, a2):
	diff = a1 - a2
	while(diff < -180.0):
		diff += 360.0
	while(diff > 180.0):
		diff -= 360.0
	return diff


def GetInfoType(string):
	cgc = CyGlobalContext()
	return cgc.getInfoTypeForString(string)


def GetDistance(x, y, dx, dy):
	distance = math.sqrt(abs((float(x - dx) * float(x - dx)) + (float(y - dy) * float(y - dy))))
	return distance


def GetXYFromDirection(x, y, direction):
	xx = x
	yy = y
	if direction == mc.N:
		yy += 1
	elif direction == mc.S:
		yy -= 1
	elif direction == mc.E:
		xx += 1
	elif direction == mc.W:
		xx -= 1
	elif direction == mc.NW:
		yy += 1
		xx -= 1
	elif direction == mc.NE:
		yy += 1
		xx += 1
	elif direction == mc.SW:
		yy -= 1
		xx -= 1
	elif direction == mc.SE:
		yy -= 1
		xx += 1
	return xx, yy


##This function is a general purpose value tuner. It finds a value that will be greater
##than or less than the desired percent of a whole map within a given tolerance. Map values
##should be between 0 and 1. To exclude parts of the map, set them to value 0.0
def FindValueFromPercent(mmap, width, height, percent, tolerance, greaterThan):
	inTolerance = False
	#to speed things up a little, lets take some time to find the middle value
	#in the dataset and use that to begin our search
	minV = 100.0
	maxV = 0.0
	totalCount = 0
	length = width * height
	for i in range(length):
		if mmap[i] != 0.0:
			totalCount += 1
			if minV > mmap[i]:
				minV = mmap[i]
			if maxV < mmap[i]:
				maxV = mmap[i]
	mid = (maxV - minV) / 2.0 + minV
	overMinCount	= 0
	equalMinCount = 0
	for i in range(length):
		if mmap[i] > minV:
			overMinCount += 1
		elif mmap[i] == minV:
			equalMinCount += 1
	threshold = mid
	thresholdChange = mid
	iterations = 0
	lastAdded = False
	while not inTolerance:
		iterations += 1
		if(iterations > 500):
			print "can't find value within tolerance, end value = "
			print "threshold = %f, thresholdChange = %f" % (threshold, thresholdChange)
			break #close enough
		matchCount = 0
		for i in range(length):
			if mmap[i] != 0.0:
				if greaterThan == True:
					if(mmap[i] > threshold):
						matchCount += 1
				else:
					if(mmap[i] < threshold):
						matchCount += 1
		currentPercent = float(matchCount) / float(totalCount)
		if currentPercent < percent + tolerance and currentPercent > percent - tolerance:
			inTolerance = True
		elif greaterThan == True:
			if currentPercent < percent:
				threshold -= thresholdChange
				if lastAdded:
					#only cut thresholdChange when direction is changed
					thresholdChange = thresholdChange / 2.0
				lastAdded = False
			else:
				threshold += thresholdChange
				if not lastAdded:
					thresholdChange = thresholdChange / 2.0
				lastAdded = True
		else:
			if currentPercent > percent:
				threshold -= thresholdChange
				if lastAdded:
					#only cut thresholdChange when direction is changed
					thresholdChange = thresholdChange / 2.0
				lastAdded = False
			else:
				threshold += thresholdChange
				if not lastAdded:
					thresholdChange = thresholdChange / 2.0
				lastAdded = True
	#at this point value should be in tolerance or close to it
	return threshold


def isWaterMatch(x, y):
	result = em.IsBelowSeaLevel(x, y)
	return result


def isHmWaterMatch(x, y):
	i = GetHmIndex(x, y)
	if pb.distanceMap[i] > mc.minimumMeteorSize / 3:
		return True
	return False


class PangaeaBreaker:
	def __init__(self):
		return


	def breakPangaeas(self):
		self.areaMap = AreaMap(mc.width, mc.height, True, True)
		meteorThrown = False
		pangeaDetected = False
		self.createDistanceMap()
		self.areaMap.defineAreas(isHmWaterMatch)
		meteorCount = 0
		while not mc.AllowPangeas and self.isPangea() and meteorCount < mc.maximumMeteorCount:
			pangeaDetected = True
			x,y = self.getMeteorStrike()
			print "A meteor has struck the Earth at %(x)d, %(y)d!!" % {"x":x,"y":y}
			self.castMeteorUponTheEarth(x,y)
			meteorThrown = True
			meteorCount += 1
			self.createDistanceMap()
			self.areaMap.defineAreas(isHmWaterMatch)
		if meteorThrown:
			print "The age of dinosours has come to a cataclysmic end."
		if meteorCount == 15:
			print "Maximum meteor count of %d has been reached. Pangaea may still exist." % meteorCount
		if mc.AllowPangeas:
			print "Pangeas are allowed on this map and will not be suppressed."
		elif pangeaDetected == False:
			print "No pangea detected on this map."


	def isPangea(self):
		continentList = list()
		for a in self.areaMap.areaList:
			if a.water == False:
				continentList.append(a)
		totalLand = 0
		for c in continentList:
			totalLand += c.size
		#sort all the continents by size, largest first
		continentList.sort(lambda x,y:cmp(x.size,y.size))
		continentList.reverse()
		biggestSize = continentList[0].size
		if 0.70 < float(biggestSize) / float(totalLand):
			return True
		return False


	def getMeteorStrike(self):
		continentList = list()
		for a in self.areaMap.areaList:
			if a.water == False:
				continentList.append(a)
		#sort all the continents by size, largest first
		continentList.sort(lambda x,y:cmp(x.size,y.size))
		continentList.reverse()
		biggestContinentID = continentList[0].ID
		x, y = self.getHighestCentrality(biggestContinentID)
		return x, y


	def isChokePoint(self, x, y, biggestContinentID):
		circlePoints = self.getCirclePoints(x, y, mc.minimumMeteorSize)
		waterOpposite = False
		landOpposite	= False
		for cp in circlePoints:
			if isHmWaterMatch(cp.x, cp.y):
				#Find opposite
				ox = x + (x - cp.x)
				oy = y + (y - cp.y)
				if isHmWaterMatch(ox, oy):
					waterOpposite = True
			else:
				#Find opposite
				ox = x + (x - cp.x)
				oy = y + (y - cp.y)
				if not isHmWaterMatch(ox, oy):
					landOpposite = True
		if landOpposite and waterOpposite:
			percent = self.getLandPercentInCircle(circlePoints, biggestContinentID)
			if percent >= mc.minimumLandInChoke:
				return True
		return False


	def getLandPercentInCircle(self, circlePoints, biggestContinentID):
		land	= 0
		water = 0
		circlePoints.sort(lambda n, m:cmp(n.y, m.y))
		for n in range(0, len(circlePoints), 2):
			cy = circlePoints[n].y
			if circlePoints[n].x < circlePoints[n + 1].x:
				x1 = circlePoints[n].x
				x2 = circlePoints[n + 1].x
			else:
				x2 = circlePoints[n].x
				x1 = circlePoints[n + 1].x
			landLine,waterLine = self.countCraterLine(x1, x2, cy, biggestContinentID)
			land += landLine
			water += waterLine
		percent = float(land) / float(land + water)
		return percent


	def countCraterLine(self, x1, x2, y, biggestContinentID):
		land	= 0
		water = 0
		for x in range(x1, x2 + 1):
			i = GetHmIndex(x, y)
			if self.areaMap.data[i] == biggestContinentID:
				land += 1
			else:
				water += 1
		return land, water


	def getContinentCenter(self, ID):
		IDCount = 0
		xTotal	= 0
		yTotal	= 0
		for y in range(mc.height):
			for x in range(mc.width):
				i = GetHmIndex(x,y)
				if self.areaMap.data[i] == ID:
					IDCount += 1
					xTotal += x
					yTotal += y
		xCenter = round(float(xTotal)/float(IDCount))
		yCenter = round(float(yTotal)/float(IDCount))
		center = xCenter,yCenter
		return center


	def getDistance(self,x, y, dx, dy):
		xx = x - dx
		if abs(xx) > mc.width / 2:
			xx = mc.width - abs(xx)
		distance = max(abs(xx), abs(y - dy))
		return distance


	def castMeteorUponTheEarth(self, x, y):
		radius = PRand.randint(mc.minimumMeteorSize, max(mc.minimumMeteorSize + 1, mc.width / 16))
		circlePointList = self.getCirclePoints(x, y, radius)
		circlePointList.sort(lambda n, m:cmp(n.y, m.y))
		for n in range(0, len(circlePointList), 2):
			cy = circlePointList[n].y
			if circlePointList[n].x < circlePointList[n + 1].x:
				x1 = circlePointList[n].x
				x2 = circlePointList[n + 1].x
			else:
				x2 = circlePointList[n].x
				x1 = circlePointList[n + 1].x
			self.drawCraterLine(x1, x2, cy)


	def drawCraterLine(self, x1, x2, y):
		if y < 0 or y >= mc.height:
			return
		for x in range(x1, x2 + 1):
			i = GetHmIndex(x, y)
			em.data[i] = 0.0


	def getCirclePoints(self, xCenter, yCenter, radius):
		circlePointList = list()
		x = 0
		y = radius
		p = 1 - radius
		self.addCirclePoints(xCenter, yCenter, x, y, circlePointList)
		while (x < y):
			x += 1
			if p < 0:
				p += 2 * x + 1
			else:
				y -= 1
				p += 2 * (x - y) + 1
			self.addCirclePoints(xCenter, yCenter, x, y, circlePointList)
		return circlePointList


	def addCirclePoints(self, xCenter, yCenter, x, y, circlePointList):
		circlePointList.append(CirclePoint(xCenter + x, yCenter + y))
		circlePointList.append(CirclePoint(xCenter - x, yCenter + y))
		circlePointList.append(CirclePoint(xCenter + x, yCenter - y))
		circlePointList.append(CirclePoint(xCenter - x, yCenter - y))
		circlePointList.append(CirclePoint(xCenter + y, yCenter + x))
		circlePointList.append(CirclePoint(xCenter - y, yCenter + x))
		circlePointList.append(CirclePoint(xCenter + y, yCenter - x))
		circlePointList.append(CirclePoint(xCenter - y, yCenter - x))


	def createDistanceMap(self):
		self.distanceMap = array('i')
		processQueue = []
		for y in range(mc.height):
			for x in range(mc.width):
				if em.IsBelowSeaLevel(x, y):
					self.distanceMap.append(1000)
				else:
					self.distanceMap.append(0)
					processQueue.append((x, y))
		while len(processQueue) > 0:
			x,y = processQueue[0]
			i = GetHmIndex(x, y)
			del processQueue[0]
			distanceToLand = self.distanceMap[i]
			for direction in range(1, 9):
				xx,yy = GetXYFromDirection(x, y, direction)
				ii = GetHmIndex(xx,yy)
				neighborDistanceToLand = self.distanceMap[ii]
				if neighborDistanceToLand > distanceToLand + 1:
					self.distanceMap[ii] = distanceToLand + 1
					processQueue.append((xx, yy))


	def getHighestCentrality(self,ID):
		C = self.createCentralityList(ID)
		C.sort(lambda x,y:cmp(x.centrality,y.centrality))
		C.reverse()
		return C[0].x, C[0].y


	def createContinentList(self,ID):
		C				 = []
		indexMap = []
		gap = 5
		n		= 0
		for y in range(mc.height):
			for x in range(mc.width):
				i = GetHmIndex(x, y)
				if y % gap == 0 and x % gap == 0 and self.areaMap.data[i] == ID:
					C.append(CentralityScore(x, y))
					indexMap.append(n)
					n += 1
				else:
					indexMap.append(-1)
		n = 0
		for s in C:
			#Check 4 nieghbors
			xx = s.x - gap
			if xx < 0:
				xx = mc.width / gap * gap
			i = GetHmIndex(xx, s.y)
			if i != -1 and self.areaMap.data[i] == ID:
				s.neighborList.append(indexMap[i])
			xx = s.x + gap
			if xx >= mc.width:
				xx = 0
			i = GetHmIndex(xx, s.y)
			if i != -1 and self.areaMap.data[i] == ID:
				s.neighborList.append(indexMap[i])
			yy = s.y - gap
			if yy < 0:
				yy = mc.height / gap * gap
			i = GetHmIndex(s.x, yy)
			if i != -1 and self.areaMap.data[i] == ID:
				s.neighborList.append(indexMap[i])
			yy = s.y + gap
			if yy > mc.height:
				yy = 0
			i = GetHmIndex(s.x, yy)
			if i != -1 and self.areaMap.data[i] == ID:
				s.neighborList.append(indexMap[i])
			n += 1
		return C


	def createCentralityList(self,ID):
		C = self.createContinentList(ID)
		for s in range(len(C)):
			S = []
			P = []
			sigma = []
			d = []
			delta = []
			for t in range(len(C)):
				sigma.append(0)
				d.append(-1)
				P.append([])
				delta.append(0)
			sigma[s] = 1
			d[s] = 0
			Q = []
			Q.append(s)
			while len(Q) > 0:
				v = Q.pop(0)
				S.append(v)
				for w in C[v].neighborList:
					if d[w] < 0:
						Q.append(w)
						d[w] = d[v] + 1
					if d[w] == d[v] + 1:
						sigma[w] = sigma[w] + sigma[v]
						P[w].append(v)
			while len(S) > 0:
				w = S.pop()
				for v in P[w]:
					delta[v] = delta[v] + (sigma[v]/sigma[w]) * (1 + delta[w])
					if w != s:
						C[w].centrality = C[w].centrality + delta[w]
		return C


	def isNeighbor(self, x, y, xx, yy):
		if mc.WrapX == True:
			mx = xx % mc.width
		elif x < 0 or x >= mc.width:
			return False
		else:
			mx = xx
		if mc.WrapY == True:
			my = yy % mc.height
		elif y < 0 or y >= mc.height:
			return False
		else:
			my = yy
		if abs(x - mx) <= 1 and abs(y - my) <= 1:
			if x == mx and y == my:
				return False
			else:
				return True
		return False


pb = PangaeaBreaker()


class CirclePoint:
	def __init__(self, x, y):
		self.x = x
		self.y = y


class CentralityScore:
	def __init__(self, x, y):
		self.x = x
		self.y = y
		self.centrality = 0
		self.neighborList = []


def isNonCoastWaterMatch(x, y):
	i = GetIndex(x, y)
	if tm.data2[i] == mc.OCEAN:
		return True
	return False


class ContinentMap:
	def __init__(self):
		return


	def generateContinentMap(self):
		self.areaMap = AreaMap(mc.width, mc.height, True, True)
		self.areaMap.defineAreas(isNonCoastWaterMatch)
		self.newWorldID = self.getNewWorldID()


	def getNewWorldID(self):
		nID = 0
		continentList = list()
		for a in self.areaMap.areaList:
			if a.water == False:
				continentList.append(a)
		totalLand = 0
		for c in continentList:
			totalLand += c.size
		#sort all the continents by size, largest first
		continentList.sort(lambda x,y:cmp(x.size,y.size))
		continentList.reverse()
		#now remove a percentage of the landmass to be considered 'Old World'
		oldWorldSize = 0
		#biggest continent is automatically 'Old World'
		oldWorldSize += continentList[0].size
		del continentList[0]
		#If this was the only continent than we have a pangaea. Oh well.
		if len(continentList) == 0:
			return -1
		#get the next largest continent and temporarily remove from list
		#add it back later and is automatically 'New World'
		biggestNewWorld = continentList[0]
		del continentList[0]
		#sort list by ID rather than size to make things
		#interesting and possibly bigger new worlds
		continentList.sort(lambda x, y:cmp(x.ID, y.ID))
		continentList.reverse()
		for n in range(len(continentList)):
			oldWorldSize += continentList[0].size
			del continentList[0]
			if float(oldWorldSize) / float(totalLand) > 0.6:
				break
		#add back the biggestNewWorld continent
		continentList.append(biggestNewWorld)
		#what remains in the list will be considered 'New World'
		#get ID for the next continent, we will use this ID for 'New World'
		#designation
		nID = continentList[0].ID
		del continentList[0] #delete to avoid unnecessary overwrite
		#now change all the remaining continents to also have nID as their ID
		for i in range(em.length):
			for c in continentList:
				if c.ID == self.areaMap.data[i]:
					self.areaMap.data[i] = nID
		return nID


continentMap = ContinentMap()


#OK! now that directions N,S,E,W are important, we have to keep in mind that
#the map plots are ordered from 0,0 in the SOUTH west corner! NOT the northwest
#corner! That means that Y increases as you go north.
class RiverMap:
	def __init__(self):
		#To provide global access without allocating alot of resources for
		#nothing, object initializer must be empty
		return


	def generateRiverMap(self):
		self.O = 9 #used for ocean or land without a river
		#averageHeightMap, flowMap, averageRainfallMap and drainageMap are offset from the other maps such that
		#each element coincides with a four tile intersection on the game map
		self.averageHeightMap		= array('d')
		self.flowMap						= array('i')
		self.averageRainfallMap = array('d')
		self.drainageMap				= array('d')
		self.riverMap						= array('i')
		#initialize maps with zeros
		for i in range(em.length):
			self.averageHeightMap.append(0.0)
			self.flowMap.append(0)
			self.averageRainfallMap.append(0.0)
			self.drainageMap.append(0.0)
			self.riverMap.append(self.O)
		#Get highest intersection neighbor
		for y in range(mc.height):
			for x in range(mc.width):
				i = GetIndex(x, y)
				maxHeight = 0.0
				for yy in range(y, y - 2, -1):
					for xx in range(x, x + 2):
						ii = GetIndex(xx, yy)
						#use an average hight of <0 to denote an ocean border
						#this will save processing time later
						if(tm.data1[ii] == mc.OCEAN):
							maxHeight = -100.0
						elif maxHeight < em.data[ii] and maxHeight >= 0:
							maxHeight = em.data[ii]
				self.averageHeightMap[i] = maxHeight
		#Now try to silt in any lakes
		self.siltifyLakes()
		self.createLakeDepressions()
		#create flowMap by checking for the lowest of each 4 connected
		#neighbor plus self
		for y in range(mc.height):
			for x in range(mc.width):
				i = GetIndex(x, y)
				lowestAlt = self.averageHeightMap[i]
				if(lowestAlt < 0.0):
					#if height is <0 then that means this intersection is
					#adjacent to an ocean and has no flow
					self.flowMap[i] = self.O
				else:
					#First assume this place is lowest, like a 'pit'. Then
					#for each place that is lower, add it to a list to be
					#randomly chosen as the drainage path
					drainList = list()
					nonDrainList = list()
					self.flowMap[i] = mc.L
					ii = GetIndex(x, y + 1)
					#in the y direction, avoid wrapping
					#if(y > 0 and self.averageHeightMap[ii] < lowestAlt):
					if(y < mc.height - 1 and self.averageHeightMap[ii] < lowestAlt):
						drainList.append(mc.N)
					else:
						nonDrainList.append(mc.N)
					ii = GetIndex(x, y - 1)
					#if(y < mc.height - 1 and self.averageHeightMap[ii] < lowestAlt):
					if(y > 0 and self.averageHeightMap[ii] < lowestAlt):
						drainList.append(mc.S)
					else:
						nonDrainList.append(mc.S)
					ii = GetIndex(x - 1, y)
					if(self.averageHeightMap[ii] < lowestAlt):
						drainList.append(mc.W)
					else:
						nonDrainList.append(mc.W)
					ii = GetIndex(x + 1, y)
					if(self.averageHeightMap[ii] < lowestAlt):
						drainList.append(mc.E)
					else:
						nonDrainList.append(mc.E)
					#never go straight when you have other choices
					count = len(drainList)
					if count == 3:
						oppDir = getOppositeDirection(nonDrainList[0])
						for dirX in drainList:
							if dirX == oppDir:
								del dirX
								break
						count = len(drainList)
					if count > 0:
						choice = int(PRand.random() * count)
						self.flowMap[i] = drainList[choice]
		if mc.UseNewClimateSystem:
			cm = c3
		else:
			cm = c2
		#Create average rainfall map so that each intersection is an average
		#of the rainfall from rm.rainMap
		for y in range(mc.height):
			for x in range(mc.width):
				i = GetIndex(x, y)
				avg = 0.0
				for yy in range(y, y - 2, -1):
					for xx in range(x, x + 2):
						ii = GetIndex(xx, yy)
						avg += cm.RainfallMap.data[ii]
				avg = avg / 4.0
				self.averageRainfallMap[i] = avg
		#Now use the flowMap as a guide to distribute average rainfall.
		#Wherever the most rainfall ends up is where the rivers will be.
		print "Distributing rainfall"
		for y in range(mc.height):
			for x in range(mc.width):
				i = GetIndex(x, y)
				flow = self.flowMap[i]
				rainFall = self.averageRainfallMap[i]
				xx = x
				yy = y
				while(flow != mc.L and flow != self.O):
					if(flow == mc.N):
						yy += 1
					elif(flow == mc.S):
						yy -= 1
					elif(flow == mc.W):
						xx -= 1
					elif(flow == mc.E):
						xx += 1
					#wrap
					if(xx < 0):
						xx = mc.width - 1
					elif(xx >= mc.width):
						xx = 0
					if(yy < 0):
						yy = mc.height - 1
					elif(yy >= mc.height):
						yy = 0
					#dump rainfall here
					ii = GetIndex(xx, yy)
					self.drainageMap[ii] += rainFall
					#reset flow
					flow = self.flowMap[ii]
		if mc.UseNewClimateSystem:
			riverThreshold = mc.RiverThreshold3
		else:
			riverThreshold = mc.RiverThreshold2
		for i in range(em.length):
			if (self.drainageMap[i] > riverThreshold):
				self.riverMap[i] = self.flowMap[i]
			else:
				self.riverMap[i] = self.O
		print "River map generated"
		#at this point river should be in tolerance or close to it
		#riverMap is ready for use


	def rxFromPlot(self, x, y, direction):
		if direction == mc.NE:
			return x, y + 1
		if direction == mc.SW:
			return x - 1, y
		if direction == mc.SE:
			return x, y
		raise ValueError, "rxFromPlot using bad direction input"


	def siltifyLakes(self):
		lakeList = []
		onQueueMap = array('i')
		for y in range(mc.height):
			for x in range(mc.width):
				onQueueMap.append(0)
				i = GetIndex(x, y)
				if self.isLake(x, y):
					lakeList.append((x, y, 1))
					onQueueMap[i] = 1
		largestLength = len(lakeList)
		while len(lakeList) > 0:
			if len(lakeList) > largestLength:
				largestLength = len(lakeList)
			x, y, lakeSize = lakeList[0]
			del lakeList[0]
			i = GetIndex(x, y)
			onQueueMap[i] = 0
			if lakeSize > mc.maxSiltPanSize:
				continue
			lakeSize += 1
			lowestNeighborAlt = self.getLowestNeighborAltitude(x, y)
			self.averageHeightMap[i] = lowestNeighborAlt + 0.005
			for direction in range(1, 5):
				xx, yy = GetXYFromDirection(x, y, direction)
				ii = GetIndex(xx, yy)
				if ii == -1:
					continue
				if self.isLake(xx, yy) and onQueueMap[ii] == 0:
					lakeList.append((xx, yy, lakeSize))
					onQueueMap[ii] = 1


	def isLake(self, x, y):
		i = GetIndex(x, y)
		alt = self.averageHeightMap[i]
		if alt < 0.0:
			return False
		for direction in range(1, 5):
			xx, yy = GetXYFromDirection(x, y, direction)
			ii = GetIndex(xx, yy)
			if ii == -1:
				continue
			if self.averageHeightMap[ii] < alt:
				return False
		return True


	def getLowestNeighborAltitude(self, x, y):
		lowest = 1.0
		for direction in range(1, 5):
			xx, yy = GetXYFromDirection(x, y, direction)
			ii = GetIndex(xx, yy)
			if ii == -1:
				continue
			if self.averageHeightMap[ii] < lowest:
				lowest = self.averageHeightMap[ii]
		return lowest


	def createLakeDepressions(self):
		lakeList = []
		for y in range(mc.height):
			for x in range(mc.width):
				i = GetIndex(x, y)
				if self.averageHeightMap[i] > mc.minLakeAltitude:
					lakeList.append((x, y))
		shufflePyList(lakeList, PRand.mapRand)
		if mc.UseNewClimateSystem:
			numLakes = int(em.length * mc.numberOfLakesPerPlot3)
		else:
			numLakes = int(em.length * mc.numberOfLakesPerPlot2)
		for n in range(numLakes):
			x, y = lakeList[n]
			i = GetIndex(x, y)
			lowestAlt = self.getLowestNeighborAltitude(x, y)
			if lowestAlt < 0.0:
				continue
			self.averageHeightMap[i] = lowestAlt - 0.001


class BonusPlacer:
	def __init__(self):
		return


	def AddBonuses(self):
		gc = CyGlobalContext()
		gameMap = CyMap()
		gameMap.recalculateAreas()
		self.AssignBonusAreas()
		numBonuses = gc.getNumBonusInfos()

		orderSet = {}
		for i in range(numBonuses):  #Check which placement orders are used, discard -1
			bonusInfo = gc.getBonusInfo(self.bonusList[i].eBonus)
			porder = bonusInfo.getPlacementOrder()
			if porder >= 0:
				orderSet[porder] = 1

		plotIndexList = [0] * em.length
		shuffleList(em.length, PRand.mapRand, plotIndexList)
		startAtIndex = 0

		porderList = sorted(orderSet.keys())
		for order in porderList:
			placementList = []
			for i in range(numBonuses):
				bonusInfo = gc.getBonusInfo(self.bonusList[i].eBonus)
				if bonusInfo.getPlacementOrder() == order:
					for n in range(self.bonusList[i].desiredBonusCount):
						placementList.append(self.bonusList[i].eBonus)
			#Create a list of map indices and shuffle them
			if placementList:
				shufflePyList(placementList, PRand.mapRand)
				for eBonus in placementList:
					startAtIndex = self.AddBonusType(eBonus, plotIndexList, startAtIndex)
		#now check to see that all resources have been placed at least once, this
		#pass ignoring area rules
		for i in range(numBonuses):
			bonus = self.bonusList[i]
			if bonus.currentBonusCount == 0 and bonus.desiredBonusCount > 0:
				startAtIndex = self.AddEmergencyBonus(bonus,False, plotIndexList, startAtIndex)
		#now check again to see that all resources have been placed at least once,
		#this time ignoring area rules and also class spacing
		for i in range(numBonuses):
			bonus = self.bonusList[i]
			if bonus.currentBonusCount == 0 and bonus.desiredBonusCount > 0:
				startAtIndex = self.AddEmergencyBonus(bonus,True, plotIndexList, startAtIndex)
		#now report resources that simply could not be placed
		for i in range(numBonuses):
			bonus = self.bonusList[i]
			bonusInfo = gc.getBonusInfo(bonus.eBonus)
			if bonus.currentBonusCount == 0 and bonus.desiredBonusCount > 0:
				print "No room at all found for %(bt)s!!!" % {"bt":bonusInfo.getType()}
			print "Placed %(cb)d, desired %(db)d for %(bt)s" % {"cb":bonus.currentBonusCount, "db":bonus.desiredBonusCount, "bt":bonusInfo.getType()}

# AIAndy: Changed to start at the end of the last run in the plot list and not shuffle an extra plot list
	def AddEmergencyBonus(self,bonus,ignoreClass, plotIndexList, startAtIndex):
		gc = CyGlobalContext()
		gameMap = gc.getMap()
		featureForest = gc.getInfoTypeForString("FEATURE_FOREST")
		bonusInfo = gc.getBonusInfo(bonus.eBonus)
		plotListLength = len(plotIndexList)
		lastI = 0
		for i in range(startAtIndex, startAtIndex + plotListLength):
			index = 0
			lastI = i
			if i >= plotListLength:
				index = plotIndexList[i - plotListLength]
			else:
				index = plotIndexList[i]
			plot = gameMap.plotByIndex(index)
			x = plot.getX()
			y = plot.getY()
			if (ignoreClass and self.PlotCanHaveBonus(plot,bonus.eBonus,False,True)) or \
			self.CanPlaceBonusAt(plot,bonus.eBonus,False,True):
				#temporarily remove any feature
				featureEnum = plot.getFeatureType()
				if featureEnum == featureForest:
					featureVariety = plot.getFeatureVariety()
					plot.setFeatureType(FeatureTypes.NO_FEATURE,-1)
				#place bonus
				plot.setBonusType(bonus.eBonus)
				bonus.currentBonusCount += 1
				#restore the feature if possible
				if featureEnum == featureForest:
					if bonusInfo == None or bonusInfo.isFeature(featureEnum):
						plot.setFeatureType(featureEnum,featureVariety)
				print "Emergency placement of 1 %(bt)s" % {"bt":bonusInfo.getType()}
				break
		lastI = (lastI + 1) % plotListLength
		return lastI

# AIAndy: Changed to start at the end of the last run in the plot list
	def AddBonusType(self, eBonus, plotIndexList, startAtIndex):
		gc = CyGlobalContext()
		gameMap = CyMap()
		featureForest = gc.getInfoTypeForString("FEATURE_FOREST")
		#first get bonus/area link
		bonus = self.bonusList[self.bonusDict[eBonus]]
		bonusInfo = gc.getBonusInfo(eBonus)
		if bonus.currentBonusCount >= bonus.desiredBonusCount:
			return False
		bonusPlaced = False
		plotListLength = len(plotIndexList)
		lastI = 0
		#now add bonuses
		for i in range(startAtIndex, startAtIndex + plotListLength):
			index = 0
			lastI = i
			if i >= plotListLength:
				index = plotIndexList[i - plotListLength]
			else:
				index = plotIndexList[i]
			plot = gameMap.plotByIndex(index)
			x = plot.getX()
			y = plot.getY()
			if self.CanPlaceBonusAt(plot, eBonus, False, False):
				#temporarily remove any feature
				featureEnum = plot.getFeatureType()
				if featureEnum == featureForest:
					featureVariety = plot.getFeatureVariety()
					plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
				#place bonus
				plot.setBonusType(eBonus)
				bonusPlaced = True
				bonus.currentBonusCount += 1
				#restore the feature if possible
				if featureEnum == featureForest:
					if bonusInfo == None or bonusInfo.isFeature(featureEnum):
						plot.setFeatureType(featureEnum, featureVariety)
				groupRange = bonusInfo.getGroupRange()
				#NEW CODE - Fuyu/LM
				#added/maxAdd: avoid grouping ALL the resources of a type together.
				#it's annoying to find 6 wines together and nowhere else on the map.
				added = 0
				if (mc.BonusMaxGroupSize == -1):
					maxAdd = (gc.getMap().getWorldSize() / 2) + 3
				elif (mc.BonusMaxGroupSize == 0):
					maxAdd = PRand.randint(1, gc.getGame().countCivPlayersEverAlive())
				else:
					maxAdd = PRand.randint(1, mc.BonusMaxGroupSize)
				for dx in range(-groupRange, groupRange + 1):
					for dy in range(-groupRange, groupRange + 1):
						 #NEW CODE - Fuyu
						if(added >= maxAdd):
							break
						if bonus.currentBonusCount < bonus.desiredBonusCount:
							loopPlot = self.plotXY(x, y, dx, dy)
							if loopPlot != None:
								if loopPlot.getX() == -1:
									raise ValueError, "plotXY returns invalid plots plot= %(x)d, %(y)d" % {"x":x, "y":y}
								if self.CanPlaceBonusAt(loopPlot,eBonus,False,False):
									if PRand.randint(0, 99) < bonusInfo.getGroupRand():
										#temporarily remove any feature
										featureEnum = loopPlot.getFeatureType()
										if featureEnum == featureForest:
											featureVariety = loopPlot.getFeatureVariety()
											loopPlot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
										#place bonus
										loopPlot.setBonusType(eBonus)
										bonus.currentBonusCount += 1
										#restore the feature if possible
										if featureEnum == featureForest:
											if bonusInfo == None or bonusInfo.isFeature(featureEnum):
												loopPlot.setFeatureType(featureEnum, featureVariety)
										#NEW CODE - Fuyu
										added += 1
			if bonusPlaced:
				break
		lastI = (lastI + 1) % plotListLength
		return lastI


	def plotXY(self, x, y, dx, dy):
		gameMap = CyMap()
		#The one that civ uses will return junk so I have to make one
		#that will not
		x = (x + dx) % mc.width
		y = y + dy
		if y < 0 or y > mc.height - 1:
			return None
		return gameMap.plot(x, y)


	def AssignBonusAreas(self):
		gc = CyGlobalContext()
		gameMap = gc.getMap()
		self.areas = gameMap.areas()
		self.bonusList = []

		#Create and shuffle the bonus list and keep tally on
		#one-area bonuses and find the smallest min area requirement
		#among those
		numUniqueBonuses = 0
		minLandAreaSize = -1
		numBonuses = gc.getNumBonusInfos()
		for i in range(numBonuses):
			bonus = BonusArea()
			bonus.eBonus = i
			self.bonusList.append(bonus)
			bonusInfo = gc.getBonusInfo(i)
			if bonusInfo.isOneArea() == True:
				numUniqueBonuses += 1
				minAreaSize = bonusInfo.getMinAreaSize()
				if (minLandAreaSize == -1 or minLandAreaSize > minAreaSize) and minAreaSize > 0:
					minLandAreaSize = minAreaSize

		shufflePyList(self.bonusList, PRand.mapRand)
		self.bonusDict = [0] * numBonuses

		for i in range(numBonuses):
			eBonus = self.bonusList[i].eBonus
			self.bonusDict[eBonus] = i
			self.bonusList[i].desiredBonusCount = self.CalculateNumBonusesToAdd(eBonus)
			bonusInfo = gc.getBonusInfo(eBonus)
			if bonusInfo.isOneArea() == False:
				continue #Only assign areas to area bonuses
			areaSuitabilityList = list()
			for area in self.areas:
				if area.getNumTiles() >= minLandAreaSize:
					aS = AreaSuitability(area.getID())
					aS.suitability,aS.numPossible = self.CalculateAreaSuitability(area, eBonus)
					areaSuitabilityList.append(aS)
			#Calculate how many areas to assign (numUniqueBonuses will be > 0 if we get here)
			areasPerBonus =	 1
			#Sort areaSuitabilityList best first
			areaSuitabilityList.sort(lambda x, y:cmp(x.numPossible, y.numPossible))
			areaSuitabilityList.reverse()
			#assign the best areas to this bonus
			for n in range(areasPerBonus):
				self.bonusList[i].areaList.append(areaSuitabilityList[n].areaID)
			#assign areas that have a high suitability also
			for n in range(areasPerBonus,len(areaSuitabilityList)):
				if areaSuitabilityList[n].suitability > 0.3:
					self.bonusList[i].areaList.append(areaSuitabilityList[n].areaID)


	def CanPlaceBonusAt(self, plot, eBonus, bIgnoreLatitude, bIgnoreArea):
		gc = CyGlobalContext()
		gameMap = CyMap()
		x = plot.getX()
		y = plot.getY()
		areaID = plot.getArea()
		if not self.PlotCanHaveBonus(plot, eBonus, bIgnoreLatitude, bIgnoreArea):
			return False
		#for i in range(DirectionTypes.NUM_DIRECTION_TYPES):
		#	loopPlot = plotDirection(x,y,DirectionTypes(i))
		#	if loopPlot.getBonusType(TeamTypes.NO_TEAM) != BonusTypes.NO_BONUS and loopPlot.getBonusType(TeamTypes.NO_TEAM) != eBonus:
		#		 return False
		bonusInfo = gc.getBonusInfo(eBonus)
		classInfo = gc.getBonusClassInfo(bonusInfo.getBonusClassType())
		if plot.isWater():
			if gameMap.getNumBonusesOnLand(eBonus) * 100 / (gameMap.getNumBonuses(eBonus) + 1) < bonusInfo.getMinLandPercent():
				return False
		#Make sure there are no bonuses of the same class (but a different type) nearby:
		if classInfo != None:
			try:
				iRange = classInfo.getUniqueRange()
			except:
				iRange = classInfo.getUniqueRange #<--attribute for vanilla
			iRange = max(0, int(iRange - (round(mc.BonusBonus) - 1)))
			for dx in range(-iRange,iRange + 1):
				for dy in range(-iRange,iRange + 1):
					loopPlot = self.plotXY(x, y, dx, dy)
					if loopPlot != None:
						if areaID == loopPlot.getArea():
							if plotDistance(x, y, loopPlot.getX(), loopPlot.getY()) <= iRange:
								eOtherBonus = loopPlot.getBonusType(TeamTypes.NO_TEAM)
								if eOtherBonus != BonusTypes.NO_BONUS:
									if gc.getBonusInfo(eOtherBonus).getBonusClassType() == bonusInfo.getBonusClassType():
										return False
		#Make sure there are no bonuses of the same type nearby:
		iRange = bonusInfo.getUniqueRange()
		iRange = max(0, int(iRange - (round(mc.BonusBonus) - 1)))
		for dx in range(-iRange, iRange + 1):
			for dy in range(-iRange, iRange + 1):
				loopPlot = self.plotXY(x, y, dx, dy)
				if loopPlot != None:
					if areaID == loopPlot.getArea():
						if plotDistance(x, y, loopPlot.getX(), loopPlot.getY()) <= iRange:
							eOtherBonus = loopPlot.getBonusType(TeamTypes.NO_TEAM)
							if eOtherBonus != BonusTypes.NO_BONUS:
								if eOtherBonus == eBonus:
									return False
		return True


	def PlotCanHaveBonus(self, plot, eBonus, bIgnoreLatitude, bIgnoreArea):
		#This function is like CvPlot::canHaveBonus but will
		#ignore blocking features and checks for a valid area.
		gc = CyGlobalContext()
		featureForest = gc.getInfoTypeForString("FEATURE_FOREST")
		if eBonus == BonusTypes.NO_BONUS:
			return True
		if plot.getBonusType(TeamTypes.NO_TEAM) != BonusTypes.NO_BONUS:
			return False
		bonusInfo = gc.getBonusInfo(eBonus)
		if plot.isPeak():
			if not bonusInfo.isPeaks():
				return False
		else:
			#Here is the change from canHaveBonus. Forest does not block bonus
			requiresForest = bonusInfo.isFeature(featureForest)
			plotIsForest = plot.getFeatureType() == featureForest
			#To avoid silk and spices on ice/tundra
			if plotIsForest and requiresForest:
				if not bonusInfo.isFeatureTerrain(plot.getTerrainType()):
					return False
			#now that bonuses that require forest are dealt with, count forest
			#as no feature
			else:
				if plot.getFeatureType() != FeatureTypes.NO_FEATURE and not plotIsForest:
					if not bonusInfo.isFeature(plot.getFeatureType()):
						return False
					if not bonusInfo.isFeatureTerrain(plot.getTerrainType()):
						return False
				else:
					if not bonusInfo.isTerrain(plot.getTerrainType()):
						return False
			if plot.isHills():
				if not bonusInfo.isHills():
					return False
			if plot.isFlatlands():
				if not bonusInfo.isFlatlands():
					return False
		if bonusInfo.isNoRiverSide():
			if plot.isRiverSide():
				return False
		if bonusInfo.getMinAreaSize() != -1:
			if plot.area().getNumTiles() < bonusInfo.getMinAreaSize():
				return False
		if not bIgnoreLatitude:
			if plot.getLatitude() > bonusInfo.getMaxLatitude():
				return False
			if plot.getLatitude() < bonusInfo.getMinLatitude():
				return False
		if not plot.isPotentialCityWork():
			return False
		if bonusInfo.isOneArea() and not bIgnoreArea:
			areaID = plot.getArea()
			areaFound = False
			i = self.bonusDict[eBonus]
			areaList = self.bonusList[i].areaList
			for n in range(len(areaList)):
				if areaList[n] == areaID:
					areaFound = True
					break
			if not areaFound:
				return False
		return True


	def CalculateNumBonusesToAdd(self, eBonus):
		#This is like the function in CvMapGenerator except it uses
		#self.PlotCanHaveBonus instead of CvPlot::canHaveBonus
		gc = CyGlobalContext()
		gameMap = CyMap()
		game = CyGame()
		bonusInfo = gc.getBonusInfo(eBonus)
		if bonusInfo.getPlacementOrder() < 0:
			return 0
		rand1 = PRand.randint(0, bonusInfo.getRandAppearance1())
		rand2 = PRand.randint(0, bonusInfo.getRandAppearance2())
		rand3 = PRand.randint(0, bonusInfo.getRandAppearance3())
		rand4 = PRand.randint(0, bonusInfo.getRandAppearance4())
		baseCount = bonusInfo.getConstAppearance() + rand1 + rand2 + rand3 + rand4
		bIgnoreLatitude = False
		bIgnoreArea = True
		landTiles		= 0
		numPossible = 0
		if bonusInfo.getTilesPer() > 0:
			for i in range(em.length):
				plot = gameMap.plotByIndex(i)
				if self.PlotCanHaveBonus(plot,eBonus,bIgnoreLatitude,bIgnoreArea):
					numPossible += 1
			landTiles += numPossible/bonusInfo.getTilesPer()
		players = game.countCivPlayersAlive() * bonusInfo.getPercentPerPlayer() / 100
		bonusCount = baseCount * (landTiles + players) / 100
		bonusCount = max(1, int(bonusCount * mc.BonusBonus))
		return bonusCount


	def GetUniqueBonusTypeCountInArea(self, area):
		gc = CyGlobalContext()
		areaID = area.getID()
		uniqueBonusCount = 0
		for i in range(len(self.bonusList)):
			areaList = self.bonusList[i].areaList
			bonusInfo = gc.getBonusInfo(self.bonusList[i].eBonus)
			if bonusInfo.isOneArea() == False:
				continue
			for n in range(len(areaList)):
				if areaList[n] == areaID:
					uniqueBonusCount += 1
					break
		return uniqueBonusCount


	def GetSameClassTypeCountInArea(self, area, eBonus):
		gc = CyGlobalContext()
		areaID = area.getID()
		uniqueBonusCount = 0
		bonusInfo = gc.getBonusInfo(eBonus)
		eClass = bonusInfo.getBonusClassType()
		if eClass == BonusClassTypes.NO_BONUSCLASS:
			return 0
		classInfo = gc.getBonusClassInfo(eClass)
		if classInfo == None:
			return 0
		try:
			uRange = classInfo.getUniqueRange()
		except:
			uRange = classInfo.getUniqueRange #<--vanilla Civ4
		uRange = max(0, int(uRange - (round(mc.BonusBonus) - 1)))
		for i in range(len(self.bonusList)):
			areaList = self.bonusList[i].areaList
			bonusInfo = gc.getBonusInfo(self.bonusList[i].eBonus)
			if bonusInfo.isOneArea() == False:
				continue
			if bonusInfo.getBonusClassType() != eClass:
				continue
			for n in range(len(areaList)):
				if areaList[n] == areaID:
					uniqueBonusCount += 1
					break
		#Same class types tend to really crowd out any bonus
		#types that are placed later. A single cow can block
		#5 * 5 squares of pig territory for example. Probably
		#shouldn't place them on the same area at all, but
		#sometimes it might be necessary.
		return uniqueBonusCount * uRange * uRange


	def CalculateAreaSuitability(self, area, eBonus):
		gc = CyGlobalContext()
		gameMap = CyMap()
		areaID = area.getID()
		uniqueTypesInArea		 = self.GetUniqueBonusTypeCountInArea(area)
		sameClassTypesInArea = self.GetSameClassTypeCountInArea(area, eBonus)
		#Get the raw number of suitable tiles
		numPossible = 0
		for i in range(em.length):
			plot = gameMap.plotByIndex(i)
			if plot.getArea() == areaID:
				if self.PlotCanHaveBonus(plot, eBonus, False, True):
					numPossible += 1
		numPossible = numPossible / (uniqueTypesInArea + sameClassTypesInArea + 1)
		suitability = float(numPossible) / float(area.getNumTiles())
		return suitability,numPossible


bp = BonusPlacer()


class BonusArea:
	def __init__(self):
		self.eBonus = -1
		self.desiredBonusCount = -1
		self.currentBonusCount = 0
		self.areaList = list()


class AreaSuitability:
	def __init__(self,areaID):
		self.areaID = areaID
		self.suitability = 0
		self.numPossible = 0


class StartingPlotFinder:
	def __init__(self):
		return


	def CachePlotValue(self):
		self.plotvalueList = []
		self.plotfoodList = []
		for y in range(mc.height):
			for x in range(mc.width):
				food, value = self.getPlotPotentialValueUncached(x, y, True)
				self.plotvalueList.append(value)
				self.plotfoodList.append(food)



	def SetStartingPlots(self):
		#try:
			gc = CyGlobalContext()
			gameMap = CyMap()
			iPlayers = gc.getGame().countCivPlayersEverAlive()
			gameMap.recalculateAreas()
			areas = gameMap.areas()
			#get old/new world status
			areaOldWorld = self.setupOldWorldAreaList()
			print "len(areaOldWorld) = %d" % len(areaOldWorld)

			self.CachePlotValue()

			#Shuffle players so the same player doesn't always get the first pick.
			#lifted from Highlands.py that ships with Civ.
			shuffledPlayers = []
			for plrCheckLoop in range(gc.getMAX_PC_PLAYERS()):
				if CyGlobalContext().getPlayer(plrCheckLoop).isEverAlive():
					shuffledPlayers.append(plrCheckLoop)
			shufflePyList(shuffledPlayers, PRand.mapRand)

			self.startingAreaList = []
			for i in range(len(areas)):
				if areaOldWorld[i] == True and areas[i].getNumTiles() > 5:
					startArea = StartingArea(areas[i].getID())
					self.startingAreaList.append(startArea)
			#Get the value of the whole old world
			oldWorldValue = 0
			for i in range(len(self.startingAreaList)):
				oldWorldValue += self.startingAreaList[i].rawValue
			#calulate value per player of old world
			oldWorldValuePerPlayer = oldWorldValue / len(shuffledPlayers)
			#Sort startingAreaList by rawValue
			self.startingAreaList.sort(lambda x, y: cmp(x.rawValue, y.rawValue))
			#Get rid of areas that have less value than oldWorldValuePerPlayer
			#as they are too small to put a player on, however leave at least
			#half as many continents as there are players, just in case the
			#continents are *all* quite small.
			numAreas = max(1, len(self.startingAreaList) - len(shuffledPlayers) / 2)
			for i in range(numAreas):
				if self.startingAreaList[0].rawValue < oldWorldValuePerPlayer:
					del self.startingAreaList[0]
				else:
					break #All remaining should be big enough
			#Recalculate the value of the whole old world
			oldWorldValue = 0
			for i in range(len(self.startingAreaList)):
				oldWorldValue += self.startingAreaList[i].rawValue
			#Recalulate value per player of old world so we are starting more
			#accurately
			oldWorldValuePerPlayer = oldWorldValue / len(shuffledPlayers)
			#Record the ideal number of players for each continent
			for startingArea in self.startingAreaList:
				startingArea.idealNumberOfPlayers = int(round(float(startingArea.rawValue) / float(oldWorldValuePerPlayer)))
			#Now we want best first
			self.startingAreaList.reverse()
			print "number of starting areas is %(s)3d" % {"s":len(self.startingAreaList)}
			iterations = 0
			while True:
				iterations += 1
				if iterations > 20:
					raise ValueError, "Too many iterations in starting area choosing loop."
				chosenStartingAreas = list()
				playersPlaced = 0
				#add up idealNumbers
				idealNumbers = 0
				for startingArea in self.startingAreaList:
					idealNumbers += startingArea.idealNumberOfPlayers
				if idealNumbers < len(shuffledPlayers):
					self.startingAreaList[0].idealNumberOfPlayers += 1
				elif idealNumbers > len(shuffledPlayers):
					self.startingAreaList[0].idealNumberOfPlayers -= 1
				#Choose areas
				for startingArea in self.startingAreaList:
					if startingArea.idealNumberOfPlayers + playersPlaced <= len(shuffledPlayers):
						chosenStartingAreas.append(startingArea)
						playersPlaced += startingArea.idealNumberOfPlayers
				#add up idealNumbers again
				idealNumbers = 0
				for startingArea in chosenStartingAreas:
					idealNumbers += startingArea.idealNumberOfPlayers
				if idealNumbers == len(shuffledPlayers):
					break
			for startingArea in chosenStartingAreas:
				for i in range(startingArea.idealNumberOfPlayers):
					startingArea.playerList.append(shuffledPlayers[0])
					del shuffledPlayers[0]
				startingArea.FindStartingPlots()
			if shuffledPlayers:
				raise ValueError, "Some players not placed in starting plot finder!"
			#Now set up for normalization
			self.plotList = list()
			for startingArea in self.startingAreaList:
				for i in range(len(startingArea.plotList)):
					self.plotList.append(startingArea.plotList[i])
			#Remove bad features. (Jungle in the case of standard game)
			#also ensure minimum hills
			for i in range(len(self.plotList)):
				if self.plotList[i].vacant == True:
					continue
				self.ensureMinimumHills(self.plotList[i].x, self.plotList[i].y)
			#find the best totalValue
			self.plotList.sort(lambda x, y:cmp(x.totalValue, y.totalValue))
			self.plotList.reverse()
			bestTotalValue = self.plotList[0].totalValue
			for i in range(len(self.plotList)):
				if self.plotList[i].vacant == True:
					continue
				currentTotalValue = self.plotList[i].totalValue
				percentLacking = 1.0 - (float(currentTotalValue) / float(bestTotalValue))
				if percentLacking > 0:
					bonuses = min(5, int(percentLacking / 0.2))
					self.boostCityPlotValue(self.plotList[i].x, self.plotList[i].y, bonuses, self.plotList[i].isCoast())
			#add bonuses due to player difficulty settings
			self.addHandicapBonus()
		#except Exception, e:
			#errorPopUp("PerfectWorld's starting plot finder has failed due to a rarely occuring bug, and this map likely has unfair starting locations. You may wish to quit this game and generate a new map.")
			#raise Exception, e


	def setupOldWorldAreaList(self):
		GC = CyGlobalContext()
		MAP = GC.getMap()
		#get official areas and make corresponding lists that determines old
		#world vs. new and also the pre-settled value.
		areaOldWorld = list()
		for CyArea in MAP.areas():
			for pI in range(em.length):
				plot = MAP.plotByIndex(pI)
				if plot.getArea() == CyArea.getID():
					if mc.AllowNewWorld and continentMap.areaMap.data[pI] == continentMap.newWorldID:
						areaOldWorld.append(False)#new world True = old world False
					else:
						areaOldWorld.append(True)
					break
		return areaOldWorld


	def getCityPotentialValue(self, x, y, cached = True):
		gc = CyGlobalContext()
		gameMap = CyMap()
		totalValue = 0
		totalFood	 = 0
		plot = gameMap.plot(x, y)
		if plot.isWater():
			return 0, 0
		if plot.isImpassable():
			return 0, 0

		#################################################
		## MongooseMod 3.5 BEGIN
		#################################################

		if plot.isPeak():
			return 0, 0

		#################################################
		## MongooseMod 3.5 END
		#################################################

		cityPlotList = list()
		#The StartPlot class has a nifty function to determine
		#salt water vs. fresh
		start = plot
		sPlot = StartPlot(x, y, 0)
		for i in range(21): #gc.getNUM_CITY_PLOTS()
			plot = plotCity(x, y, i)
			if not plot.isWater() and plot.getArea() != start.getArea():
				food, value = 0, 0
			else:
				if cached:
					food, value = self.getPlotPotentialValue(plot.getX(), plot.getY(), sPlot.isCoast())
				else:
					food, value = self.getPlotPotentialValueUncached(plot.getX(), plot.getY(), sPlot.isCoast())
			totalFood += food
			cPlot = CityPlot(food, value)
			cityPlotList.append(cPlot)
		usablePlots = totalFood / gc.getFOOD_CONSUMPTION_PER_POPULATION()
		cityPlotList.sort(lambda x, y:cmp(x.value, y.value))
		cityPlotList.reverse()
		#value is obviously limited to available food
		if usablePlots > 21: #gc.getNUM_CITY_PLOTS():
			usablePlots = 21 #gc.getNUM_CITY_PLOTS()
		for i in range(usablePlots):
			cPlot = cityPlotList[i]
			totalValue += cPlot.value
		#The StartPlot class has a nifty function to determine
		#salt water vs. fresh
		sPlot = StartPlot(x, y, 0)
		if sPlot.isCoast() == True:
			totalValue = int(float(totalValue) * mc.CoastalCityValueBonus)
		if sPlot.isRiverSide() == True:
			totalValue = int(float(totalValue) * mc.RiverCityValueBonus)
		return totalFood, totalValue

	def getPlotPotentialValue(self, x, y, coastalCity):
		i = GetIndex(x, y)
		value = self.plotvalueList[i]
		if coastalCity == False:
			gameMap = CyMap()
			plot = gameMap.plot(x, y)
			if plot.isWater():
				return 0, value
		food = self.plotfoodList[i]
		return food, value

	def getPlotPotentialValueUncached(self, x, y, coastalCity):
		gc = CyGlobalContext()
		gameMap = CyMap()
		game = gc.getGame()
		value = 0
		plot = gameMap.plot(x, y)
		commerce	 = plot.calculateBestNatureYield(YieldTypes.YIELD_COMMERCE,		TeamTypes.NO_TEAM)
		food			 = plot.calculateBestNatureYield(YieldTypes.YIELD_FOOD,				TeamTypes.NO_TEAM)
		production = plot.calculateBestNatureYield(YieldTypes.YIELD_PRODUCTION, TeamTypes.NO_TEAM)
		featureEnum = plot.getFeatureType()
		featureInfo = gc.getFeatureInfo(featureEnum)
		#if featureEnum != FeatureTypes.NO_FEATURE and featureInfo.getYieldChange(YieldTypes.YIELD_FOOD) < 0:
		#	for n in range(gc.getNumBuildInfos()):
		#		buildInfo = gc.getBuildInfo(n)
		#		if buildInfo.isFeatureRemove(featureEnum) and gc.getTechInfo(buildInfo.getFeatureTech(featureEnum)).getEra() <= max(game.getStartEra(), 1):
		#			featureEnum = FeatureTypes.NO_FEATURE
		#			break
		#Get best bonus improvement score. Test technology era of bonus
		#first, then test each improvement
		bestImp = None
		bonusEnum = plot.getBonusType(TeamTypes.NO_TEAM)
		bonusInfo = gc.getBonusInfo(bonusEnum)
		if bonusInfo != None and (gc.getTechInfo(bonusInfo.getTechReveal()) == None or gc.getTechInfo(bonusInfo.getTechReveal()).getEra() <= max(game.getStartEra(), 1)):
			commerce	 += bonusInfo.getYieldChange(YieldTypes.YIELD_COMMERCE)
			food			 += bonusInfo.getYieldChange(YieldTypes.YIELD_FOOD)
			production += bonusInfo.getYieldChange(YieldTypes.YIELD_PRODUCTION)
		else:
			bonusEnum = -1
			bonusInfo = None
		improvementList = list()
		for n in range(gc.getNumBuildInfos()):
			#Test for improvement validity
			buildInfo = gc.getBuildInfo(n)
			impEnum = buildInfo.getImprovement()
			impInfo = gc.getImprovementInfo(impEnum)
			if impInfo == None:
				continue
			#some mods use improvements for other things, so if there's no tech requirement we still don't want it factored in. Not good behavior for C2C, so changed.
			if buildInfo.getTechPrereq() != TechTypes.NO_TECH and gc.getTechInfo(buildInfo.getTechPrereq()).getEra() > max(game.getStartEra(), 1):
				continue
			if plot.canHaveImprovement(impEnum, TeamTypes.NO_TEAM, True):
				#This function will not find bonus yield changes for NO_PLAYER much to my annoyance
				impCommerce		= plot.calculateImprovementYieldChange(impEnum,YieldTypes.YIELD_COMMERCE,		PlayerTypes.NO_PLAYER, False)
				impFood				= plot.calculateImprovementYieldChange(impEnum,YieldTypes.YIELD_FOOD,				PlayerTypes.NO_PLAYER, False)
				impProduction = plot.calculateImprovementYieldChange(impEnum,YieldTypes.YIELD_PRODUCTION, PlayerTypes.NO_PLAYER, False)
				if bonusEnum != -1:
					impCommerce		+= impInfo.getImprovementBonusYield(bonusEnum,YieldTypes.YIELD_COMMERCE)
					impFood				+= impInfo.getImprovementBonusYield(bonusEnum,YieldTypes.YIELD_FOOD)
					impProduction += impInfo.getImprovementBonusYield(bonusEnum,YieldTypes.YIELD_PRODUCTION)
				#See if feature is removed, if so we must subtract the added yield from that feature
				bProceed = True
				if featureEnum != FeatureTypes.NO_FEATURE and buildInfo.isFeatureRemove(featureEnum):
					if buildInfo.getFeatureTech(featureEnum) == TechTypes.NO_TECH or gc.getTechInfo(buildInfo.getFeatureTech(featureEnum)).getEra() <= max(game.getStartEra(), 1):
						impCommerce		-= featureInfo.getYieldChange(YieldTypes.YIELD_COMMERCE)	 + featureInfo.getRiverYieldChange(YieldTypes.YIELD_COMMERCE)
						impFood				-= featureInfo.getYieldChange(YieldTypes.YIELD_FOOD)			 + featureInfo.getRiverYieldChange(YieldTypes.YIELD_FOOD)
						impProduction -= featureInfo.getYieldChange(YieldTypes.YIELD_PRODUCTION) + featureInfo.getRiverYieldChange(YieldTypes.YIELD_PRODUCTION)
					else:
						bProceed = False
				if bProceed:
					imp = Improvement(impEnum, impFood, impProduction, impCommerce, 0)
					improvementList.append(imp)
		for i in range(len(improvementList)):
			impCommerce		= improvementList[i].commerce		+ commerce
			impFood				= improvementList[i].food				+ food
			impProduction = improvementList[i].production + production
			impValue = impCommerce * mc.CommerceValue + impFood * mc.FoodValue + impProduction * mc.ProductionValue
			#Food surplus makes the square much more valueable than if there
			#is no food here.
			if food >= gc.getFOOD_CONSUMPTION_PER_POPULATION():
				impValue *= 4
			elif food == gc.getFOOD_CONSUMPTION_PER_POPULATION() - 1:
				impValue *= 2
			improvementList[i].value = impValue
		if len(improvementList) > 0:
			#sort all allowed improvement values to find the best
			improvementList.sort(lambda x, y:cmp(x.value, y.value))
			improvementList.reverse()
			bestImp = improvementList[0]
			commerce	 += bestImp.commerce
			food			 += bestImp.food
			production += bestImp.production
		value = commerce * mc.CommerceValue + food * mc.FoodValue + production * mc.ProductionValue
		#Try to avoid included water food resources for non-coastal starts. It confuses the AI.
		if not coastalCity and plot.isWater():
			value = 0
		#Food surplus makes the square much more valueable than if there
		#is no food here.
		if food >= gc.getFOOD_CONSUMPTION_PER_POPULATION():
			value *= 4
		elif food == gc.getFOOD_CONSUMPTION_PER_POPULATION() - 1:
			value *= 2
		if food + commerce + production < 3:
			value = 0
		return food, value


	def boostCityPlotValue(self, x, y, bonuses, isCoastalCity):
		mapGen = CyMapGenerator()
		food,value = self.getCityPotentialValue(x, y)
		gc = CyGlobalContext()
		gameMap = gc.getMap()
		game = gc.getGame()
		# Shuffle the bonus order so that different cities have different preferences for bonuses
		numBonuses = gc.getNumBonusInfos()
		shuffledBonuses = [0] * numBonuses
		shuffleList(numBonuses, PRand.mapRand, shuffledBonuses)

		if len(shuffledBonuses) != numBonuses:
			raise ValueError, "Bad bonus shuffle. Learn 2 shuffle."
		bonusCount = 0
		#Do this process in 3 passes for each yield type
		yields = []
		yields.append(YieldTypes.YIELD_PRODUCTION)
		yields.append(YieldTypes.YIELD_COMMERCE)
		yields.append(YieldTypes.YIELD_FOOD)
		#NEW CODE - Fuyu
		bAllowStrategicBonus = (PRand.random() <= mc.allowStrategicBonusChance)
		plotList = []
		#print "Num city plots: %d" % gc.getNUM_CITY_PLOTS()
		for i in range(21): # gc.getNUM_CITY_PLOTS()
			plotList.append(plotCity(x, y, i))
		shufflePyList(plotList, PRand.mapRand)
		for n in range(len(yields) * bonuses + 1):
			for plot in plotList:
				#NEW CODE - LM
				if bonusCount >= bonuses:
					return
				if plot.getX() == x and plot.getY() == y:
					continue
				if plot.isWater() and not isCoastalCity:
					continue
				if not plot.isWater() and gameMap.plot(x, y).getArea() != plot.getArea():
					continue
				if plot.getBonusType(TeamTypes.NO_TEAM) != BonusTypes.NO_BONUS:
					continue
				food, value = self.getCityPotentialValue(x, y, False)
				#NEW CODE - Fuyu
				currentYield = yields[(n + bonusCount) % (len(yields))]
				#switch to food if food is needed
				usablePlots = food / gc.getFOOD_CONSUMPTION_PER_POPULATION()
				if usablePlots <= 10: #gc.getNUM_CITY_PLOTS() / 2:
					currentYield = YieldTypes.YIELD_FOOD
				#temporarily remove any feature
				featureEnum = plot.getFeatureType()
				if featureEnum != FeatureTypes.NO_FEATURE:
					featureVariety = plot.getFeatureVariety()
					plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
				for b in range(gc.getNumBonusInfos()):
					bonusEnum = shuffledBonuses[b]
					bonusInfo = gc.getBonusInfo(bonusEnum)
					#if not bonusInfo.isNormalize():
					#	continue
					if bonusInfo.getYieldChange(currentYield) < 1:
						continue
					#NEW CODE - Fuyu/LM
					if gc.getTechInfo(bonusInfo.getTechReveal()) != None and gc.getTechInfo(bonusInfo.getTechReveal()).getEra() > max(game.getStartEra(), 1):
						continue
					if not bp.PlotCanHaveBonus(plot, bonusEnum, False, False):
						if PRand.random() > mc.ignoreAreaRestrictionChance:
							continue
						if not bp.PlotCanHaveBonus(plot, bonusEnum, False, True):
							continue
					if bonusInfo.getBonusClassType() == gc.getInfoTypeForString("BONUSCLASS_STRATEGIC"):
						if not bAllowStrategicBonus:
							continue
						bAllowStrategicBonus = False
					plot.setBonusType(bonusEnum)
					bonusCount += 1
					break
				#restore the feature if possible
				if featureEnum != FeatureTypes.NO_FEATURE:
					bonusInfo = gc.getBonusInfo(plot.getBonusType(TeamTypes.NO_TEAM))
					if bonusInfo == None or bonusInfo.isFeature(featureEnum):
						plot.setFeatureType(featureEnum, featureVariety)


	def ensureMinimumHills(self, x, y):
		gc = CyGlobalContext()
		gameMap = CyMap()
		hillsFound = 0
		peaksFound = 0
		badFeaturesFound = 0
		plotList = []
		for i in range(21): #gc.getNUM_CITY_PLOTS()
			plot = plotCity(x, y, i)
			featureInfo = gc.getFeatureInfo(plot.getFeatureType())
			if plot.getX() == x and plot.getY() == y:
				#remove bad feature on start but don't count it.
				if featureInfo != None:
					totalYield = 0
					for yi in range(YieldTypes.NUM_YIELD_TYPES):
						totalYield += featureInfo.getYieldChange(YieldTypes(yi))
					if totalYield <= 0:#bad feature
						plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
				continue
			if plot.getPlotType() == PlotTypes.PLOT_HILLS and gameMap.plot(x, y).getArea() == plot.getArea():
				hillsFound += 1
			if plot.getPlotType() == PlotTypes.PLOT_PEAK:
				peaksFound += 1
			if featureInfo != None:
				#now count the bad features
				totalYield = 0
				for yi in range(YieldTypes.NUM_YIELD_TYPES):
					totalYield += featureInfo.getYieldChange(YieldTypes(yi))
				if totalYield <= 0:#bad feature
					badFeaturesFound += 1
			if plot.isWater():
				continue
			if plot.getBonusType(TeamTypes.NO_TEAM) != BonusTypes.NO_BONUS:
				continue
			plotList.append(plot)
		shufflePyList(plotList, PRand.mapRand)
		#ensure maximum number of peaks
		if peaksFound > mc.MaxPeaksInFC:
			for plot in plotList:
				if peaksFound == mc.MaxPeaksInFC:
					break
				if plot.getPlotType() == PlotTypes.PLOT_PEAK:
					plot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
					if plot.getArea() == gameMap.plot(x, y).getArea():
						hillsFound += 1
					peaksFound -= 1
		#Ensure minimum number of hills
		hillsNeeded = mc.MinHillsInFC - hillsFound
		if hillsNeeded > 0:
			for plot in plotList:
				if hillsNeeded <= 0:
					break
				featureInfo = gc.getFeatureInfo(plot.getFeatureType())
				requiresFlatlands = (featureInfo != None and featureInfo.isRequiresFlatlands())
				bonusInfo = gc.getBonusInfo(plot.getBonusType(TeamTypes.NO_TEAM))
				if plot.getPlotType() != PlotTypes.PLOT_HILLS and plot.getArea() == gameMap.plot(x, y).getArea() and bonusInfo == None and not requiresFlatlands:
					plot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
					hillsNeeded -= 1
			if hillsNeeded > 0:
				for plot in plotList:
					if plot.getPlotType() != PlotTypes.PLOT_HILLS and plot.getArea() == gameMap.plot(x, y).getArea() and (bonusInfo == None or not bonusInfo.isRequiresFlatlands()):
						plot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
						hillsNeeded -= 1
						if requiresFlatlands:
							plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
		#ensure maximum number of bad features
		badFeaturesToRemove = badFeaturesFound - mc.MaxBadFeaturesInFC
		if badFeaturesToRemove > 0:
			#remove half from flatlands, the rest from hills
			badFeaturesToRemoveFromFlatlands = badFeaturesToRemove / 2 + badFeaturesToRemove % 2
			badFeaturesToRemove -= badFeaturesToRemoveFromFlatlands
			for plot in plotList:
				if badFeaturesToRemoveFromFlatlands <= 0 and badFeaturesToRemove <= 0:
					break
				featureEnum = plot.getFeatureType()
				featureInfo = gc.getFeatureInfo(featureEnum)
				bonusEnum = plot.getBonusType(TeamTypes.NO_TEAM)
				if featureInfo != None:
					totalYield = 0
					for yi in range(YieldTypes.NUM_YIELD_TYPES):
						totalYield += featureInfo.getYieldChange(YieldTypes(yi))
					if totalYield <= 0:#bad feature
						if plot.getPlotType() == PlotTypes.PLOT_LAND and badFeaturesToRemoveFromFlatlands > 0:
							badFeaturesToRemoveFromFlatlands -= 1
						if plot.getPlotType() == PlotTypes.PLOT_HILLS and badFeaturesToRemove > 0:
							badFeaturesToRemove -= 1
						plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
						if (bonusEnum != BonusTypes.NO_BONUS and not bp.PlotCanHaveBonus(plot, bonusEnum, True, True)):
							badFeaturesToRemove += 1
							plot.setFeatureType(featureEnum, -1)
			#if there are not enough hills or flatlands, there will be leftovers
			badFeaturesToRemove += badFeaturesToRemoveFromFlatlands
			for plot in plotList:
				if badFeaturesToRemove <= 0:
					break
				featureInfo = gc.getFeatureInfo(plot.getFeatureType())
				if featureInfo != None:
					totalYield = 0
					for yi in range(YieldTypes.NUM_YIELD_TYPES):
						totalYield += featureInfo.getYieldChange(YieldTypes(yi))
					if totalYield <= 0:#bad feature
						badFeaturesToRemove -= 1
						plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)


	def addHandicapBonus(self):
		gc = CyGlobalContext()
		for ePlayer in range(gc.getMAX_PC_PLAYERS()):
			player = gc.getPlayer(ePlayer)
			if player.isEverAlive() and player.isHuman():
				eHandicap = player.getHandicapType()
				startPlot = player.getStartingPlot()
				sPlot = StartPlot(startPlot.getX(),startPlot.getY(), 0)
				if eHandicap == gc.getInfoTypeForString("HANDICAP_SETTLER"):
					if mc.SettlerBonus > 0:
						print "Human player at Settler difficulty, adding %d resources" % mc.SettlerBonus
						self.boostCityPlotValue(startPlot.getX(), startPlot.getY(), mc.SettlerBonus, sPlot.isCoast())
				elif eHandicap ==	 gc.getInfoTypeForString("HANDICAP_CHIEFTAIN"):
					if mc.ChieftainBonus > 0:
						print "Human player at Chieftain difficulty, adding %d resources" % mc.ChieftainBonus
						self.boostCityPlotValue(startPlot.getX(), startPlot.getY(), mc.ChieftainBonus, sPlot.isCoast())
				elif eHandicap ==	 gc.getInfoTypeForString("HANDICAP_WARLORD"):
					if mc.WarlordBonus > 0:
						print "Human player at Warlord difficulty, adding %d resources" % mc.WarlordBonus
						self.boostCityPlotValue(startPlot.getX(), startPlot.getY(), mc.WarlordBonus, sPlot.isCoast())
				elif eHandicap ==	 gc.getInfoTypeForString("HANDICAP_NOBLE"):
					if mc.NobleBonus > 0:
						print "Human player at Noble difficulty, adding %d resources" % mc.NobleBonus
						self.boostCityPlotValue(startPlot.getX(), startPlot.getY(), mc.NobleBonus, sPlot.isCoast())
				elif eHandicap ==	 gc.getInfoTypeForString("HANDICAP_PRINCE"):
					if mc.PrinceBonus > 0:
						print "Human player at Prince difficulty, adding %d resources" % mc.PrinceBonus
						self.boostCityPlotValue(startPlot.getX(), startPlot.getY(), mc.PrinceBonus, sPlot.isCoast())
				elif eHandicap ==	 gc.getInfoTypeForString("HANDICAP_MONARCH"):
					if mc.MonarchBonus > 0:
						print "Human player at Monarch difficulty, adding %d resources" % mc.MonarchBonus
						self.boostCityPlotValue(startPlot.getX(), startPlot.getY(), mc.MonarchBonus, sPlot.isCoast())
				elif eHandicap ==	 gc.getInfoTypeForString("HANDICAP_EMPEROR"):
					if mc.EmperorBonus > 0:
						print "Human player at Emperor difficulty, adding %d resources" % mc.EmperorBonus
						self.boostCityPlotValue(startPlot.getX(), startPlot.getY(), mc.EmperorBonus, sPlot.isCoast())
				elif eHandicap ==	 gc.getInfoTypeForString("HANDICAP_IMMORTAL"):
					if mc.ImmortalBonus > 0:
						print "Human player at Immortal difficulty, adding %d resources" % mc.ImmortalBonus
						self.boostCityPlotValue(startPlot.getX(), startPlot.getY(), mc.ImmortalBonus, sPlot.isCoast())
				elif eHandicap ==	 gc.getInfoTypeForString("HANDICAP_DEITY"):
					if mc.DeityBonus > 0:
						print "Human player at Deity Difficulty, adding %d resources" % mc.DeityBonus
						self.boostCityPlotValue(startPlot.getX(), startPlot.getY(), mc.DeityBonus, sPlot.isCoast())
				elif eHandicap ==	 gc.getInfoTypeForString("HANDICAP_NIGHTMARE"):
					if mc.NightmareBonus > 0:
						print "Human player at Nightmare Difficulty, adding %d resources" % mc.NightmareBonus
						self.boostCityPlotValue(startPlot.getX(), startPlot.getY(), mc.NightmareBonus, sPlot.isCoast())


spf = StartingPlotFinder()


class CityPlot:
	def __init__(self, food, value):
		self.food	 = food
		self.value = value


class Improvement:
	def __init__(self, e, food, production, commerce, value):
		self.e = e
		self.food				= food
		self.production = production
		self.commerce		= commerce
		self.value			= value


class StartingArea:
	def __init__(self, areaID):
		self.areaID = areaID
		self.playerList = list()
		self.plotList		= list()
		self.distanceTable = array('i')
		self.rawValue = 0
		self.CalculatePlotList()
		self.idealNumberOfPlayers = 0


	def CalculatePlotList(self):
		gc = CyGlobalContext()
		gameMap = CyMap()
		for y in range(mc.height):
			for x in range(mc.width):
				plot = gameMap.plot(x, y)
				if plot.getArea() == self.areaID:
					#don't place a city on top of a bonus
					if plot.getBonusType(TeamTypes.NO_TEAM) != BonusTypes.NO_BONUS:
						continue
					food,value = spf.getCityPotentialValue(x, y)
					if value > 0:
						startPlot = StartPlot(x, y, value)
						if plot.isWater() == True:
							raise ValueError, "potential start plot is water!"
						self.plotList.append(startPlot)
		#Sort plots by local value
		self.plotList.sort(lambda x, y: cmp(x.localValue, y.localValue))
		#To save time and space let's get rid of some of the lesser plots
		cull = (len(self.plotList) * 2) / 3
		for i in range(cull):
			del self.plotList[0]
		#You now should be able to eliminate more plots by sorting high to low
		#and having the best plot eat plots within 3 squares, then same for next,
		#etc.
		self.plotList.reverse()
		numPlots = len(self.plotList)
		for n in range(numPlots):
			#At some point the length of plot list will be much shorter than at
			#the beginning of the loop, so it can never end normally
			if n >= len(self.plotList) - 1:
				break
			y = self.plotList[n].y
			x = self.plotList[n].x
			for yy in range(y - 3, y + 4):
				for xx in range(x - 3, x + 4):
					if yy < 0 or yy >= mc.height:
						continue
					xx = xx % mc.width#wrap xx
					if xx < 0:
						raise ValueError, "xx value not wrapping properly in StartingArea.CalculatePlotList"
					for m in range(n,len(self.plotList)):
						#At some point the length of plot list will be much shorter than at
						#the beginning of the loop, so it can never end normally
						if m >= len(self.plotList) - 1:
							break
						if self.plotList[m] != self.plotList[n]:
							if self.plotList[m].x == xx and self.plotList[m].y == yy:
								del self.plotList[m]
##		print "number of final plots in areaID = %(a)3d is %(p)5d" % {"a":self.areaID,"p":len(self.plotList)}
		#At this point we should have a list of the very best places
		#to build cities on this continent. Now we need a table with
		#the distance from each city to every other city
		#Create distance table
		for i in range(len(self.plotList) * len(self.plotList)):
			self.distanceTable.append(-11)
		#Fill distance table
		for n in range(len(self.plotList)):
			#While were already looping lets calculate the raw value
			self.rawValue += self.plotList[n].localValue
			avgDistance = 0
			for m in range(n, len(self.plotList)):
				nPlot = gameMap.plot(self.plotList[n].x,self.plotList[n].y)
				mPlot = gameMap.plot(self.plotList[m].x,self.plotList[m].y)
				gameMap.resetPathDistance()
				distance = gameMap.calculatePathDistance(nPlot,mPlot)
				#If path fails try reversing it
				self.distanceTable[n*len(self.plotList) + m] = distance
				self.distanceTable[m*len(self.plotList) + n] = distance
				avgDistance += distance
			self.plotList[n].avgDistance = avgDistance


	def FindStartingPlots(self):
		gc = CyGlobalContext()
		gameMap = CyMap()
		numPlayers = len(self.playerList)
		if numPlayers <= 0:
			return
		avgDistanceList = list()
		for i in range(len(self.plotList)):
			avgDistanceList.append(self.plotList[i])
		#Make sure first guy starts on the end and not in the middle,
		#otherwise if there are two players one will start on the middle
		#and the other on the end
		avgDistanceList.sort(lambda x, y:cmp(x.avgDistance, y.avgDistance))
		avgDistanceList.reverse()
		#First place players as far as possible away from each other
		#Place the first player
		avgDistanceList[0].vacant = False
		for i in range(1,numPlayers):
			distanceList = list()
			for n in range(len(self.plotList)):
				if self.plotList[n].vacant == True:
					minDistance = -1
					for m in range(len(self.plotList)):
						if self.plotList[m].vacant == False:
							ii = n * len(self.plotList) + m
							distance = self.distanceTable[ii]
							if minDistance == -1 or minDistance > distance:
								minDistance = distance
					self.plotList[n].nearestStart = minDistance
					distanceList.append(self.plotList[n])
			#Find biggest nearestStart and place a start there
			distanceList.sort(lambda x,y:cmp(x.nearestStart,y.nearestStart))
			distanceList.reverse()
			distanceList[0].vacant = False
		self.CalculateStartingPlotValues()
		#Now place all starting positions
		n = 0
		for m in range(len(self.plotList)):
			if self.plotList[m].vacant == False:
				sPlot = gameMap.plot(self.plotList[m].x,self.plotList[m].y)
				if sPlot.isWater() == True:
					raise ValueError, "Start plot is water!"
				sPlot.setImprovementType(-1)
				playerID = self.playerList[n]
				player = gc.getPlayer(playerID)
				sPlot.setStartingPlot(True)
				player.setStartingPlot(sPlot,True)
				n += 1


	def CalculateStartingPlotValues(self):
		gameMap = CyMap()
		numPlots = len(self.plotList)
		for n in range(numPlots):
			self.plotList[n].owner = -1
			self.plotList[n].totalValue = 0
		for n in range(numPlots):
			if self.plotList[n].vacant == True:
				continue
			self.plotList[n].totalValue = 0
			self.plotList[n].numberOfOwnedCities = 0
			for m in range(numPlots):
				i = n * numPlots + m
				nDistance = self.distanceTable[i]
				if nDistance == -1:
					leastDistance = False
				else:
					leastDistance = True #Being optimistic, prove me wrong
				for p in range(numPlots):
					if p == n or self.plotList[p].vacant == True:
						continue
					ii = p * numPlots + m
					pDistance = self.distanceTable[ii]
					if pDistance > -1 and pDistance <= nDistance:
						leastDistance = False #Proven wrong
						break
				if leastDistance == True:
					self.plotList[n].totalValue += self.plotList[m].localValue
					self.plotList[m].owner = self.plotList[n]
					self.plotList[m].distanceToOwner = nDistance
					self.plotList[n].numberOfOwnedCities += 1


	def getDistance(self, x, y, dx, dy):
		xx = x - dx
		if abs(xx) > mc.width / 2:
			xx = mc.width - abs(xx)
		distance = max(abs(xx), abs(y - dy))
		return distance


class StartPlot:
	def __init__(self, x, y, localValue):
		self.x = x
		self.y = y
		self.localValue = localValue
		self.totalValue = 0
		self.numberOfOwnedCities = 0
		self.distanceToOwner = -1
		self.nearestStart = -1
		self.vacant = True
		self.owner = None
		self.avgDistance = 0


	def isCoast(self):
		waterArea = CyMap().plot(self.x, self.y).waterArea()
		return waterArea is not None and not waterArea.isLake()


	def isRiverSide(self):
		gameMap = CyMap()
		plot = gameMap.plot(self.x, self.y)
		return plot.isRiverSide()


	def plot(self):
		gameMap = CyMap()
		return gameMap.plot(self.x, self.y)


	def copy(self):
		cp = StartPlot(self.x, self.y, self.localValue)
		cp.totalValue					 = self.totalValue
		cp.numberOfOwnedCities = self.numberOfOwnedCities
		cp.distanceToOwner		 = self.distanceToOwner
		cp.nearestStart				 = self.nearestStart
		cp.vacant							 = self.vacant
		cp.owner							 = self.owner
		cp.avgDistance				 = self.avgDistance
		return cp


	def __str__(self):
		linestring = "x=%(x)3d,y=%(y)3d,localValue=%(lv)6d,totalValue =%(tv)6d, nearestStart=%(ad)6d, coastalCity=%(cc)d" % \
		{"x":self.x,"y":self.y,"lv":self.localValue,"tv":self.totalValue,"ad":self.nearestStart,"cc":self.isCoast()}
		return linestring


em = ElevationMap()
c2 = ClimateMap2()
c3 = ClimateMap3()
tm = TerrainMap()
rm = RiverMap()


###############################################################################
## Functions that Civ is Looking For
###############################################################################

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_PERFECTMONGOOSE_DESCR"


def getWrapX():
	return mc.WrapX


def getWrapY():
	return mc.WrapY


def getNumCustomMapOptions():
	mc.initialize()
	return 6


def getCustomMapOptionName(argsList):
		optionID = argsList[0]
		if optionID == 0:
			return "Pangaeas:"
		elif optionID == 1:
			return "Use:"
		elif optionID == 2:
			return "World Wrap:"
		elif optionID == 3:
			return "Start:"
		elif optionID == 4:
			return "Rivers:"
		elif optionID == 5:
			return "Resources:"
		return u""


def getNumCustomMapOptionValues(argsList):
		optionID = argsList[0]
		if optionID == 0:
			return 2
		elif optionID == 1:
			return 2
		elif optionID == 2:
			return 3
		elif optionID == 3:
			return 2
		elif optionID == 4:
			return 3
		elif optionID == 5:
			return 3
		return 0


def getCustomMapOptionDescAt(argsList):
	optionID		= argsList[0]
	selectionID = argsList[1]
	if optionID == 0:
		if selectionID == 0:
			if mc.AllowPangeas:
				return "Allow (Do Nothing)"
			else:
				return "Break with Meteors"
		elif selectionID == 1:
			if mc.AllowPangeas:
				return "Break with Meteors"
			else:
				return "Allow (Do Nothing)"
	elif optionID == 1:
		if selectionID == 0:
			if mc.UseNewClimateSystem:
				return "PW3 Climate System"
			else:
				return "PW2 Climate System"
		elif selectionID == 1:
			if mc.UseNewClimateSystem:
				return "PW2 Climate System"
			else:
				return "PW3 Climate System"
	elif optionID == 2:
		if selectionID == 0:
			return "Cylindrical"
		elif selectionID == 1:
			return "Toroidal"
		elif selectionID == 2:
			return "Flat"
	elif optionID == 3:
		if selectionID == 0:
			if mc.AllowNewWorld:
				return "Old World"
			else:
				return "Everywhere"
		elif selectionID == 1:
			if mc.AllowNewWorld:
				return "Everywhere"
			else:
				return "Old World"
	elif optionID == 4:
		if selectionID == 0:
			return "Few"
		elif selectionID == 1:
			return "Default"
		elif selectionID == 2:
			return "Plenty"
	elif optionID == 5:
		if selectionID == 0:
			return "Few"
		elif selectionID == 1:
			return "Default"
		elif selectionID == 2:
			return "Plenty"
	return u""


def getCustomMapOptionDefault(argsList):
	optionID		= argsList[0]
	if optionID == 4:
		return 1
	elif optionID == 5:
		return 1
	return 0


def isRandomCustomMapOption(argsList):
	optionID		= argsList[0]
	if optionID == 4:
		return True
	elif optionID == 5:
		return True
	return False


#This doesn't work with my river system so it is disabled. Some civs
#might start without a river. Boo hoo.
def normalizeAddRiver():
	return
def normalizeAddLakes():
	return
def normalizeAddGoodTerrain():
	return
def normalizeRemoveBadTerrain():
	return
def normalizeRemoveBadFeatures():
	return
def normalizeAddFoodBonuses():
	return
def normalizeAddExtras():
	return
def normalizeRemovePeaks():
	return


def isAdvancedMap():
	return False


def isClimateMap():
	return True


def isSeaLevelMap():
	return True


def getTopLatitude():
	return 90


def getBottomLatitude():
	return -90


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


def generatePlotTypes():
	timer = BugUtil.Timer('generatePlotTypes init')
	gc = CyGlobalContext()
	mmap = gc.getMap()
	mc.width	= mmap.getGridWidth()
	mc.height = mmap.getGridHeight()
	mc.minimumMeteorSize = (1 + int(round(float(mc.width) / float(mc.width)))) * 3
	mc.AdaptToClimate()
	PRand.seed()
	em.Initialize(mc.width, mc.height, mc.WrapX, mc.WrapY)
	timer.log()
	timer = BugUtil.Timer('generateElevationMap')
	em.GenerateElevationMap()
	timer.log()
	timer = BugUtil.Timer('FillInLakes and breakPangaeas')
	em.FillInLakes()
	pb.breakPangaeas()
	timer.log()
	timer = BugUtil.Timer('climate system')
	if mc.UseNewClimateSystem:
		c3.GenerateTemperatureMap()
		c3.GenerateRainfallMap()
	else:
		c2.createClimateMaps()
	timer.log()
	timer = BugUtil.Timer('plotMap')
	tm.Initialize()
	tm.GeneratePlotMap()
	timer.log()
	timer = BugUtil.Timer('terrainMap')
	tm.GenerateTerrainMap()
	timer.log()
	timer = BugUtil.Timer('riverMap')
	rm.generateRiverMap()
	if mc.AllowNewWorld:
		continentMap.generateContinentMap()
	plotTypes = [PlotTypes.PLOT_OCEAN] * em.length
	for i in range(em.length):
		mapLoc = tm.data1[i]
		if mapLoc == mc.PEAK:
			plotTypes[i] = PlotTypes.PLOT_PEAK
		elif mapLoc == mc.HILLS:
			plotTypes[i] = PlotTypes.PLOT_HILLS
		elif mapLoc == mc.LAND:
			plotTypes[i] = PlotTypes.PLOT_LAND
	timer.log()
	return plotTypes


def generateTerrainTypes():
	gc = CyGlobalContext()

	terrainDesert = gc.getInfoTypeForString("TERRAIN_DESERT")
	terrainSaltFlats = gc.getInfoTypeForString("TERRAIN_SALT_FLATS")
	terrainDunes = gc.getInfoTypeForString("TERRAIN_DUNES")
	terrainScrub = gc.getInfoTypeForString("TERRAIN_SCRUB")
	terrainRocky = gc.getInfoTypeForString("TERRAIN_ROCKY")
	terrainBarren = gc.getInfoTypeForString("TERRAIN_BARREN")
	terrainPlains = gc.getInfoTypeForString("TERRAIN_PLAINS")
	terrainIce = gc.getInfoTypeForString("TERRAIN_ICE")
	terrainTundra = gc.getInfoTypeForString("TERRAIN_TAIGA")
	terrainPermafrost = gc.getInfoTypeForString("TERRAIN_TUNDRA")
	terrainGrass = gc.getInfoTypeForString("TERRAIN_GRASSLAND")
	terrainLush = gc.getInfoTypeForString("TERRAIN_LUSH")
	terrainMuddy = gc.getInfoTypeForString("TERRAIN_MUDDY")
	terrainMarsh = gc.getInfoTypeForString("TERRAIN_MARSH")
	terrainHill = gc.getInfoTypeForString("TERRAIN_HILL")
	terrainCoast = gc.getInfoTypeForString("TERRAIN_COAST")
	terrainOcean = gc.getInfoTypeForString("TERRAIN_OCEAN")
	terrainPeak = gc.getInfoTypeForString("TERRAIN_PEAK")

	terrainTypes = [0] * em.length
	for i in range(em.length):
		if tm.data2[i] == mc.OCEAN:
				terrainTypes[i] = terrainOcean
		elif tm.data2[i] == mc.COAST:
				terrainTypes[i] = terrainCoast
		elif tm.data2[i] == mc.DESERT:
				terrainTypes[i] = terrainDesert
		elif tm.data2[i] == mc.PLAINS:
				terrainTypes[i] = terrainPlains
		elif tm.data2[i] == mc.GRASS:
				terrainTypes[i] = terrainGrass
		elif tm.data2[i] == mc.TUNDRA:
				terrainTypes[i] = terrainTundra
		elif tm.data2[i] == mc.SALT_FLATS:
				terrainTypes[i] = terrainSaltFlats
		elif tm.data2[i] == mc.DUNES:
				terrainTypes[i] = terrainDunes
		elif tm.data2[i] == mc.SCRUB:
				terrainTypes[i] = terrainScrub
		elif tm.data2[i] == mc.ROCKY:
				terrainTypes[i] = terrainRocky
		elif tm.data2[i] == mc.BARREN:
				terrainTypes[i] = terrainBarren
		elif tm.data2[i] == mc.PERMAFROST:
				terrainTypes[i] = terrainPermafrost
		elif tm.data2[i] == mc.LUSH:
				terrainTypes[i] = terrainLush
		elif tm.data2[i] == mc.MUDDY:
				terrainTypes[i] = terrainMuddy
		elif tm.data2[i] == mc.MARSH:
				terrainTypes[i] = terrainMarsh
		elif tm.data2[i] == mc.SNOW:
				terrainTypes[i] = terrainIce

	return terrainTypes


def addRivers():
	gc = CyGlobalContext()
	pmap = gc.getMap()
	for y in range(mc.height):
		for x in range(mc.width):
			placeRiversInPlot(x, y)
	#peaks and rivers don't always mix well graphically, so lets eliminate
	#these potential glitches. Basically if there are adjacent peaks on both
	#sides of a river, either in a cardinal direction or diagonally, they
	#will look bad.
	for y in range(mc.height):
		for x in range(mc.width):
			plot = pmap.plot(x, y)
			if plot.isPeak() == True:
				if plot.isNOfRiver() == True:
					for xx in range(x - 1, x + 2):
						yy = y - 1
						if yy < 0:
							break
						#wrap in x direction
						if xx == -1:
							xx = mc.width - 1
						elif xx == mc.width:
							xx = 0
						newPlot = pmap.plot(xx, yy)
						ii = GetIndex(xx,yy)
						if newPlot.isPeak():
							plot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
							tm.data1[ii] = mc.HILLS
							break
			#possibly changed so checked again
			if plot.isPeak() == True:
				if plot.isWOfRiver() == True:
					for yy in range(y - 1, y + 2):
						xx = x + 1
						if xx == mc.width:
							xx = 0
						#do not wrap in y direction
						if yy == -1:
							continue
						elif yy == mc.height:
							continue
						newPlot = pmap.plot(xx, yy)
						ii = GetIndex(xx, yy)
						if newPlot.isPeak():
							plot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
							tm.data1[ii] = mc.HILLS
							break


def placeRiversInPlot(x, y):
	gc = CyGlobalContext()
	pmap = gc.getMap()
	plot = pmap.plot(x, y)
	#NE
	xx, yy = rm.rxFromPlot(x, y, mc.NE)
	ii = GetIndex(xx, yy)
	if ii != -1:
		if rm.riverMap[ii] == mc.S:
			plot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
	#SW
	xx, yy = rm.rxFromPlot(x, y, mc.SW)
	ii = GetIndex(xx, yy)
	if ii != -1:
		if rm.riverMap[ii] == mc.E:
			plot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_EAST)
	#SE
	xx, yy = rm.rxFromPlot(x, y, mc.SE)
	ii = GetIndex(xx, yy)
	if ii != -1:
		if rm.riverMap[ii] == mc.N:
			plot.setWOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		elif rm.riverMap[ii] == mc.W:
			plot.setNOfRiver(True, CardinalDirectionTypes.CARDINALDIRECTION_WEST)


'''
This function examines a lake area and removes ugly surrounding rivers. Any
river that is flowing away from the lake, or alongside the lake will be
removed. This function also returns a list of riverID's that flow into the
lake.
'''
def cleanUpLake(x, y):
	gc = CyGlobalContext()
	mmap = gc.getMap()
	riversIntoLake = list()
	plot = mmap.plot(x,y+1)#North
	if plot != 0 and plot.isNOfRiver() == True:
		plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	if plot != 0 and plot.isWOfRiver() == True:
		if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_SOUTH:
			riversIntoLake.append(plot.getRiverID())
		else:
			plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	plot = mmap.plot(x - 1,y)#West
	if plot != 0 and plot.isWOfRiver() == True:
		plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	if plot != 0 and plot.isNOfRiver() == True:
		if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_EAST:
			riversIntoLake.append(plot.getRiverID())
		else:
			plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	plot = mmap.plot(x + 1,y)#East
	if plot != 0 and plot.isNOfRiver() == True:
		if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_WEST:
			riversIntoLake.append(plot.getRiverID())
		else:
			plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	plot = mmap.plot(x,y-1)#South
	if plot != 0 and plot.isWOfRiver() == True:
		if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_NORTH:
			riversIntoLake.append(plot.getRiverID())
		else:
			plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	plot = mmap.plot(x-1,y+1)#Northwest
	if plot != 0 and plot.isWOfRiver() == True:
		if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_SOUTH:
			riversIntoLake.append(plot.getRiverID())
		else:
			plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	if plot != 0 and plot.isNOfRiver() == True:
		if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_EAST:
			riversIntoLake.append(plot.getRiverID())
		else:
			plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	plot = mmap.plot(x+1,y+1)#Northeast
	if plot != 0 and plot.isNOfRiver() == True:
		if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_WEST:
			riversIntoLake.append(plot.getRiverID())
		else:
			plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	plot = mmap.plot(x-1,y-1)#Southhwest
	if plot != 0 and plot.isWOfRiver() == True:
		if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_NORTH:
			riversIntoLake.append(plot.getRiverID())
		else:
			plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	#Southeast plot is not relevant
	return riversIntoLake


'''
This function replaces rivers to update the river crossings after a lake or
channel is placed at X,Y. There had been a long standing problem where water tiles
added after a river were causing graphical glitches and incorrect river rules
due to not updating the river crossings.
'''
def replaceRivers(x, y):
	gc = CyGlobalContext()
	mmap = gc.getMap()
	plot = mmap.plot(x, y + 1)#North
	if plot != 0 and plot.isWOfRiver() == True:
		if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_SOUTH:
			#setting the river to what it already is will be ignored by the dll,
			#so it must be unset and then set again.
			plot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			plot.setWOfRiver(True,	CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
	plot = mmap.plot(x - 1, y)#West
	if plot != 0 and plot.isNOfRiver() == True:
		if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_EAST:
			plot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			plot.setNOfRiver(True,	CardinalDirectionTypes.CARDINALDIRECTION_EAST)
	plot = mmap.plot(x + 1, y)#East
	if plot != 0 and plot.isNOfRiver() == True:
		if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_WEST:
			plot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			plot.setNOfRiver(True,	CardinalDirectionTypes.CARDINALDIRECTION_WEST)
	plot = mmap.plot(x, y - 1)#South
	if plot != 0 and plot.isWOfRiver() == True:
		if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_NORTH:
			plot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			plot.setWOfRiver(True,	CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
	plot = mmap.plot(x - 1, y + 1)#Northwest
	if plot != 0 and plot.isWOfRiver() == True:
		if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_SOUTH:
			plot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			plot.setWOfRiver(True,	CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
	if plot != 0 and plot.isNOfRiver() == True:
		if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_EAST:
			plot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			plot.setNOfRiver(True,	CardinalDirectionTypes.CARDINALDIRECTION_EAST)
	plot = mmap.plot(x + 1, y + 1)#Northeast
	if plot != 0 and plot.isNOfRiver() == True:
		if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_WEST:
			plot.setNOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			plot.setNOfRiver(True,	CardinalDirectionTypes.CARDINALDIRECTION_WEST)
	plot = mmap.plot(x - 1, y - 1)#Southhwest
	if plot != 0 and plot.isWOfRiver() == True:
		if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_NORTH:
			plot.setWOfRiver(False, CardinalDirectionTypes.NO_CARDINALDIRECTION)
			plot.setWOfRiver(True,	CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
	#Southeast plot is not relevant


'''
It looks bad to have a lake, fed by a river, sitting right next to the coast.
This function tries to minimize that occurance by replacing it with a
natural harbor, which looks much better.
'''
def makeHarbor(x, y, oceanMap):
	oceanID = oceanMap.getOceanID()
	i = oceanMap.getIndex(x, y)
	if oceanMap.data[i] != oceanID:
		return
	#N
	xx = x
	yy = y + 2
	ii = oceanMap.getIndex(xx, yy)
	if ii > -1 and oceanMap.getAreaByID(oceanMap.data[ii]).water == True and oceanMap.data[ii] != oceanID:
		makeChannel(x, y + 1)
		oceanMap.defineAreas(isWaterMatch)
		oceanID = oceanMap.getOceanID()
	#S
	xx = x
	yy = y - 2
	ii = oceanMap.getIndex(xx, yy)
	if ii > -1 and oceanMap.getAreaByID(oceanMap.data[ii]).water == True and oceanMap.data[ii] != oceanID:
		makeChannel(x, y - 1)
		oceanMap.defineAreas(isWaterMatch)
		oceanID = oceanMap.getOceanID()
	#E
	xx = x + 2
	yy = y
	ii = oceanMap.getIndex(xx, yy)
	if ii > -1 and oceanMap.getAreaByID(oceanMap.data[ii]).water == True and oceanMap.data[ii] != oceanID:
		makeChannel(x + 1, y)
		oceanMap.defineAreas(isWaterMatch)
		oceanID = oceanMap.getOceanID()
	#W
	xx = x - 2
	yy = y
	ii = oceanMap.getIndex(xx, yy)
	if ii > -1 and oceanMap.getAreaByID(oceanMap.data[ii]).water == True and oceanMap.data[ii] != oceanID:
		makeChannel(x - 1, y)
		oceanMap.defineAreas(isWaterMatch)
		oceanID = oceanMap.getOceanID()
	#NW
	xx = x - 1
	yy = y + 1
	ii = oceanMap.getIndex(xx, yy)
	if ii > -1 and oceanMap.getAreaByID(oceanMap.data[ii]).water == True and oceanMap.data[ii] != oceanID:
		makeChannel(x - 1, y)
		oceanMap.defineAreas(isWaterMatch)
		oceanID = oceanMap.getOceanID()
	#NE
	xx = x + 1
	yy = y + 1
	ii = oceanMap.getIndex(xx, yy)
	if ii > -1 and oceanMap.getAreaByID(oceanMap.data[ii]).water == True and oceanMap.data[ii] != oceanID:
		makeChannel(x + 1, y)
		oceanMap.defineAreas(isWaterMatch)
		oceanID = oceanMap.getOceanID()
	#SW
	xx = x - 1
	yy = y - 1
	ii = oceanMap.getIndex(xx, yy)
	if ii > -1 and oceanMap.getAreaByID(oceanMap.data[ii]).water == True and oceanMap.data[ii] != oceanID:
		makeChannel(x, y - 1)
		oceanMap.defineAreas(isWaterMatch)
		oceanID = oceanMap.getOceanID()
	#NW
	xx = x - 1
	yy = y + 1
	ii = oceanMap.getIndex(xx, yy)
	if ii > -1 and oceanMap.getAreaByID(oceanMap.data[ii]).water == True and oceanMap.data[ii] != oceanID:
		makeChannel(x, y + 1)
		oceanMap.defineAreas(isWaterMatch)
		oceanID = oceanMap.getOceanID()


def makeChannel(x, y):
	gc = CyGlobalContext()
	mmap = gc.getMap()
	terrainCoast = gc.getInfoTypeForString("TERRAIN_COAST")
	plot = mmap.plot(x, y)
	cleanUpLake(x, y)
	plot.setTerrainType(terrainCoast,True,True)
	plot.setRiverID(-1)
	plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	replaceRivers(x, y)
	i = GetIndex(x, y)
	tm.data1[i] = mc.OCEAN


def expandLake(x, y, riversIntoLake, oceanMap):
	class LakePlot:
		def __init__(self, x, y, altitude):
			self.x = x
			self.y = y
			self.altitude = altitude
	gc = CyGlobalContext()
	mmap = gc.getMap()
	terrainCoast = gc.getInfoTypeForString("TERRAIN_COAST")
	lakePlots = list()
	lakeNeighbors = list()
	i = oceanMap.getIndex(x, y)
	if tm.data2[i] == mc.DESERT:
		desertModifier = mc.DesertLakeModifier
	else:
		desertModifier = 1.0
	if mc.UseNewClimateSystem:
		lakeSize = max(3, int(rm.drainageMap[i] * mc.LakeSizePerDrainage3 * desertModifier))
	else:
		lakeSize = max(3, int(rm.drainageMap[i] * mc.LakeSizePerDrainage2 * desertModifier))
	start = LakePlot(x, y, em.data[i])
	lakeNeighbors.append(start)
	while lakeSize > 0 and len(lakeNeighbors) > 0:
		lakeNeighbors.sort(lambda x,y:cmp(x.altitude,y.altitude))
		currentLakePlot = lakeNeighbors[0]
		del lakeNeighbors[0]
		lakePlots.append(currentLakePlot)
		plot = mmap.plot(currentLakePlot.x,currentLakePlot.y)
		#if you are erasing a river to make a lake, make the lake smaller
		if plot.isNOfRiver() == True or plot.isWOfRiver() == True:
			lakeSize -= 1
		makeChannel(currentLakePlot.x,currentLakePlot.y)
		#Add valid neighbors to lakeNeighbors
		for n in range(4):
			if n == 0:#N
				xx = currentLakePlot.x
				yy = currentLakePlot.y + 1
				ii = oceanMap.getIndex(xx,yy)
			elif n == 1:#S
				xx = currentLakePlot.x
				yy = currentLakePlot.y - 1
				ii = oceanMap.getIndex(xx,yy)
			elif n == 2:#E
				xx = currentLakePlot.x + 1
				yy = currentLakePlot.y
				ii = oceanMap.getIndex(xx,yy)
			elif n == 3:#W
				xx = currentLakePlot.x - 1
				yy = currentLakePlot.y
				ii = oceanMap.getIndex(xx,yy)
			else:
				raise ValueError, "too many cardinal directions"
			if ii != -1:
				#if this neighbor is in water area, then quit
				areaID = oceanMap.data[ii]
				if areaID == 0:
					raise ValueError, "areaID = 0 while generating lakes. This is a bug"
				for n in range(len(oceanMap.areaList)):
					if oceanMap.areaList[n].ID == areaID:
						if oceanMap.areaList[n].water == True:
							return
				if rm.riverMap[ii] != mc.L and not mmap.plot(xx, yy).isWater():
					lakeNeighbors.append(LakePlot(xx, yy, em.data[ii]))
		lakeSize -= 1


def addLakes():
	gc = CyGlobalContext()
	mmap = gc.getMap()
	terrainCoast = gc.getInfoTypeForString("TERRAIN_COAST")
	oceanMap = AreaMap(mc.width, mc.height, True, True)
	oceanMap.defineAreas(isWaterMatch)
	for y in range(mc.height):
		for x in range(mc.width):
			i = GetIndex(x, y)
			if rm.flowMap[i] == mc.L:
				riversIntoLake = cleanUpLake(x, y)
				plot = mmap.plot(x,y)
				if len(riversIntoLake) > 0:
					expandLake(x, y, riversIntoLake, oceanMap)
				else:
					#no lake here, but in that case there should be no rivers either
					plot.setRiverID(-1)
					plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
					plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
	oceanMap.defineAreas(isWaterMatch)
	for y in range(mc.height):
		for x in range(mc.width):
			i = GetIndex(x, y)
			makeHarbor(x, y, oceanMap)


def addFeatures():
	gc = CyGlobalContext()
	mmap = gc.getMap()
	fForest			 = gc.getInfoTypeForString("FEATURE_FOREST")
	fJungle			 = gc.getInfoTypeForString("FEATURE_JUNGLE")
	fFloodPlains = gc.getInfoTypeForString("FEATURE_FLOOD_PLAINS")
	fOasis			 = gc.getInfoTypeForString("FEATURE_OASIS")
	featureReef = gc.getInfoTypeForString("FEATURE_REEF")
	featureKelp = gc.getInfoTypeForString("FEATURE_KELP")
	featureBog = gc.getInfoTypeForString("FEATURE_PEAT_BOG")
	featureSwordGrass = gc.getInfoTypeForString("FEATURE_VERY_TALL_GRASS")
	featureSwamp = gc.getInfoTypeForString("FEATURE_SWAMP")

	FORESTLEAFY			= 0
	FORESTEVERGREEN = 1
	FORESTSNOWY			= 2
	createIce()
	if mc.UseNewClimateSystem:
		cm = c3
		snowTemp	 = mc.SnowTemp3
		tundraTemp = mc.TundraTemp3
	else:
		cm = c2
		snowTemp	 = mc.SnowTemp2
		tundraTemp = mc.TundraTemp2
	#Now plant forest or jungle
	for y in range(mc.height):
		for x in range(mc.width):
			i = GetIndex(x, y)
			plot = mmap.plot(x, y)

			#floodplains and Oasis
			if (tm.data2[i] == mc.DESERT or tm.data2[i] == mc.SCRUB or tm.data2[i] == mc.DUNES) and tm.data1[i] != mc.HILLS and tm.data1[i] != mc.PEAK and not plot.isWater():
				if plot.isRiver():
					plot.setFeatureType(fFloodPlains, 0)
				else:
					#is this square surrounded by desert?
					foundNonDesert = False
					for yy in range(y - 1, y + 2):
						for xx in range(x - 1, x + 2):
							ii = GetIndex(xx, yy)
							surPlot = mmap.plot(xx, yy)
							if (tm.data2[ii] != mc.DESERT and tm.data2[ii] != mc.SCRUB and tm.data2[ii] != mc.DUNES and tm.data1[ii] != mc.PEAK):
								foundNonDesert = True
							elif surPlot == 0:
								foundNonDesert = True
							elif surPlot.isWater() == True:
								foundNonDesert = True
							elif surPlot.getFeatureType() == fOasis:
								foundNonDesert = True
					if foundNonDesert == False:
						if PRand.random() < mc.OasisChance:
							plot.setFeatureType(fOasis, 0)

## C2C start ##
			#marsh features
			if (tm.data2[i] == mc.MARSH) and (plot.isPeak() == False) and (tm.data1[i] != mc.OCEAN):
					if cm.TemperatureMap.data[i] < 0.45:
							if PRand.random() < 0.75:
									plot.setFeatureType(featureBog,0)
					elif cm.TemperatureMap.data[i] < 0.6:
							if PRand.random() < 0.25:
									plot.setFeatureType(featureBog,0)
							elif PRand.random() < 0.5:
									plot.setFeatureType(featureSwordGrass,0)
					elif cm.TemperatureMap.data[i] < 0.8:
							if PRand.random() < 0.25:
									plot.setFeatureType(featureSwordGrass,0)
							elif PRand.random() < 0.5:
									plot.setFeatureType(featureSwamp,0)
					elif PRand.random() < 0.5:
							plot.setFeatureType(featureSwamp,0)

			if plot.getFeatureType() == FeatureTypes.NO_FEATURE:
				for iI in range(gc.getNumFeatureInfos()):
		#			print gc.getFeatureInfo(iI).getDescription()
					if plot.canHaveFeature(iI):
		#				print "Can have feature with probability: %d" % gc.getFeatureInfo(iI).getAppearanceProbability()
						if PRand.random() * 10000 < gc.getFeatureInfo(iI).getAppearanceProbability():
		#					print "Setting feature"
							plot.setFeatureType(iI, -1)

## C2C end ##
			#forest and jungle
			if not plot.isWater() and plot.getFeatureType() == FeatureTypes.NO_FEATURE and tm.data2[i] != mc.DESERT and tm.data2[i] != mc.DUNES and tm.data2[i] != mc.SALT_FLATS and tm.data2[i] != mc.SCRUB and not plot.isPeak():
				if cm.RainfallMap.data[i] > tm.plainsThreshold * mc.TreeFactor and PRand.random() < mc.MaxTreeChance:#jungle
					if cm.TemperatureMap.data[i] > mc.JungleTemp:
						if tm.data2[i] == mc.PLAINS:
							plot.setFeatureType(fForest, FORESTLEAFY)
						else:
							plot.setFeatureType(fJungle, 0)
					elif cm.TemperatureMap.data[i] > mc.ForestTemp:
						plot.setFeatureType(fForest, FORESTLEAFY)
					elif cm.TemperatureMap.data[i] > tundraTemp:
						plot.setFeatureType(fForest, FORESTEVERGREEN)
					elif cm.TemperatureMap.data[i] > snowTemp:
						plot.setFeatureType(fForest, FORESTSNOWY)
				elif cm.RainfallMap.data[i] > tm.desertThreshold:#forest
					if cm.RainfallMap.data[i] > PRand.random() * tm.plainsThreshold * mc.TreeFactor / mc.MaxTreeChance:
						if cm.TemperatureMap.data[i] > mc.ForestTemp:
							 plot.setFeatureType(fForest, FORESTLEAFY)
						elif cm.TemperatureMap.data[i] > tundraTemp:
							plot.setFeatureType(fForest, FORESTEVERGREEN)
						elif cm.TemperatureMap.data[i] > snowTemp * 0.8:
							plot.setFeatureType(fForest, FORESTSNOWY)


def createIce():
	gc = CyGlobalContext()
	mmap = gc.getMap()
	featureIce = gc.getInfoTypeForString("FEATURE_ICE")
	if mc.WrapY == True:
		iceChance = 0.5
	else:
		iceChance = 1.0
	y = 0
	while iceChance > 0.1:
		for x in range(mc.width):
			plot = mmap.plot(x, y)
			if plot != 0 and plot.isWater() == True and PRand.random() < iceChance:
				plot.setFeatureType(featureIce, 0)
		iceChance *= mc.IceChanceMultiplier
		y += 1
	if mc.WrapY == True:
		iceChance = 0.5
	else:
		iceChance = 1.0
	y = mc.height - 1
	while iceChance > 0.1:
		for x in range(mc.width):
			plot = mmap.plot(x, y)
			if plot != 0 and plot.isWater() == True and PRand.random() < iceChance:
				plot.setFeatureType(featureIce, 0)
		iceChance *= mc.IceChanceMultiplier
		y -= 1


def addBonuses():
	timer = BugUtil.Timer('Bonus generation')
	bp.AddBonuses()
	timer.log()


def assignStartingPlots():
	timer = BugUtil.Timer('Starting plot finding')
	gc = CyGlobalContext()
	gameMap = CyMap()
	iPlayers = gc.getGame().countCivPlayersEverAlive()
	spf.SetStartingPlots()
	timer.log()

def afterGeneration():
	import CvMapGeneratorUtil
	CvMapGeneratorUtil.placeC2CBonuses()

def beforeInit():
	mc.initInGameOptions()
