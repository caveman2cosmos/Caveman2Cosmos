##############################################################################
## File: PerfectWorld.py version 2.06f
## Author: Rich Marinaccio
## Copyright 2007 Rich Marinaccio
##
## This map script for Civ4 generates a random, earth-like map, usually with a
## 'New World' with no starting locations that can only be reached with
## ocean going technology. Though great pains are taken to accurately simulate
## landforms and climate, the goal must be to make unpredictible, beautiful
## looking maps that are fun to play on.
##
## -- Summary of creation process: --
## First, a random heightfield is created using midpoint displacement. The
## resulting altitudes are then modified by a plate tectonics scheme that
## grows random plates and raises the altitudes near the plate borders to
## create mountain ranges and island chains.
##
## In generating the plot types from a heightmap, I had found that using
## peaks for high altitude and land for less altitude created large clusters
## of peaks, surrounded by a donut of hills, surrounded again by a donut of
## land. This looked absolutely terrible for Civ, so I made it such that
## peaks and hills are determined by altitude *differences* rather than by
## absolute altitude. This approach looks much better and more natural.
##
## The terrain generator gives the other needed visual cues to communicate
## altitude. Since air temperature gets colder with altitude, the peaks will
## be plots of ice and tundra, even near the equator, if the altitude
## is high enough. Prevailing winds, temperature and rainfall are all simulated
## in the terrain generator. You will notice that the deserts and rainforests
## are where they should be, as well as rain shadows behind mountain ranges.
##
## Rivers and lakes are also generated from the heightmap and follow accurate
## drainage paths, although with such a small heightmap some randomness needs
## to be thrown in to prevent rivers from being merely straight lines.
##
## Map bonuses are placed following the XML Rules but slightly differently than
## the default implimentation to better accomodate this map script.
##
## I've always felt that the most satisfying civ games are the ones that
## provide a use for those explorers and caravels. Though the map generator
## does not explicitly create a 'New World', it will take advantage of any
## continents that can serve that purpose. No starting locations will be placed
## on these continents. Therefore, the likelyhood of a significant new world
## is very high, but not guaranteed. It might also come in the form of multiple
## smaller 'New Worlds' rather than a large continent.
##
##############################################################################
## Version History
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
## new features. Fixed some minor bugs involving the Areamap filler
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

from array import array
from random import random, randint, seed
import math

class MapConstants :
		def __init__(self):
				return
		def initialize(self):
				print "Initializing map constants"
##############################################################################
## GLOBAL TUNING VARIABLES: Change these to customize the map results

				#Percent of land vs. water
				self.landPercent = 0.29
				self.SeaLevelFactor = 1.5

				#If this variable is set to False, a shower of colossal meteors will attempt to
				#break up any pangea-like continents. Setting this variable to True will allow
				#pangeas to sometimes occur and should be greatly favored by any dinosaurs
				#that might be living on this planet at the time.
				self.AllowPangeas = False

				#This variable can be used to turn off 'New world' logic and place starting
				#positions anywhere in the world. For some mods, a new world doesn't make
				#sense.
				self.AllowNewWorld = True

				#How many land squares will be above peak threshold and thus 'peaks'.
				self.PeakPercent = 0.12

				#How many land squares will be above hill threshold and thus 'hills' unless
				#they are also above peak threshold in which case they will be 'peaks'.
				self.HillPercent = 0.35

				#In addition to the relative peak and hill generation, there is also a
				#process that changes flats to hills or peaks based on altitude. This tends
				#to randomize the high altitude areas somewhat and improve their appearance.
				#These variables control the frequency of hills and peaks at the highest altitude.
				self.HillChanceAtOne = .50
				self.PeakChanceAtOne = .27

				#How many land squares will be below desert rainfall threshold. In this case,
				#rain levels close to zero are very likely to be desert, while rain levels close
				#to the desert threshold will more likely be plains.
				self.SaltFlatsPercent = 0.02
				self.DunesPercent = 0.06
				self.DesertPercent = 0.12
				self.ScrubPercent = 0.16

				#How many land squares will be below plains rainfall threshold. Rain levels close
				#to the desert threshold are likely to be plains, while those close to the plains
				#threshold are likely to be grassland.
				self.RockyPercent = 0.20
				self.BarrenPercent = 0.26
				self.PlainsPercent = 0.48
				self.GrasslandPercent = 0.78
				self.LushPercent = 0.9
				self.MuddyPercent = 0.96

				#---The following variables are not based on percentages. Because temperature
				#---is so strongly tied to latitude, using percentages for things like ice and
				#---tundra leads to very strange results if most of the worlds land lies near
				#---the equator

				#What temperature will be considered cold enough to be ice. Temperatures range
				#from coldest 0.0 to hottest 1.0.
				self.IceTemp = .25

				#What temperature will be considered cold enough to be tundra. Temperatures range
				#from coldest 0.0 to hottest 1.0.
				self.PermafrostTemp = .30
				self.TundraTemp = .35

				#What temperature will be considered cold enough to be taiga. Temperatures range
				#from coldest 0.0 to hottest 1.0.
				self.TaigaTemp = .40

				#Hotter than this temperature will be considered deciduous forest, colder will
				#be evergreen forest.Temperatures range from coldest 0.0 to hottest 1.0.
				self.ForestTemp = .50

				#What temperature will be considered hot enough to be jungle. Temperatures range
				#from coldest 0.0 to hottest 1.0.
				self.JungleTemp = .7

				#Sets the threshold for jungle rainfall by modifying the plains threshold by this factor.
				self.TreeFactor = 1.5

				#This is the maximum chance for a tree to be placed when rainfall is above jungle level.
				#use a value between 0.0 and 1.0
				self.MaxTreeChance = 1.0

				#This variable adjusts the amount of bonuses on the map. Values above 1.0 will add bonus
				#bonuses. People often want lots of bonuses, and for those people, this variable is definately
				#a bonus.
				self.BonusBonus = 1.0

				#This variable adjusts the maximun number of identical bonuses to be placed in a
				#single group. People tend not to like all instances of a bonus type to be found within
				#a single 3x3 area. When set to 0 (default), the maximum group size is
				#(number of players) - 1
				#when set to 1, this will disable all bonus grouping
				self.BonusMaxGroupSize = 0

				#How many squares are added to a lake for each unit of drainage flowing
				#into it.
				self.LakeSizePerDrainage = 14.0

				#This value modifies LakeSizePerRiverLength when a lake begins in desert
				self.DesertLakeModifier = .60

				#This value controls the amount of siltification in lakes
				self.maxSiltPanSize = 200

				#This value controls the number of mid-altitude lake depressions per
				#map square. It will become a lake if enough water flows into the
				#depression.
				self.numberOfLakesPerPlot = 0.003

				#This value sets the minimum altitude of lake depressions. They
				#generally look better higher up.
				self.minLakeAltitude = 0.45

				#This value is used to decide if enough water has accumulated to form a river.
				#A lower value creates more rivers over the entire map.
				self.RiverThreshold = 4

				#The percent chance that an oasis may appear in desert. A tile must be desert and
				#surrounded on all sides by desert.
				self.OasisChance = .08

				#This sets the amount of heat lost at the highest altitude. 1.0 loses all heat
				#0.0 loses no heat.
				self.heatLostAtOne = 1.0

				#This value is an exponent that controls the curve associated with
				#temperature loss. Higher values create a steeper curve.
				self.temperatureLossCurve = 1.3

				#Degrees latitude for the top and bottom of the map. This allows
				#for more specific climate zones
				self.topLatitude = 90
				self.bottomLatitude = -90

				#Horse latitudes and polar fronts plus and minus in case you
				#want some zones to be compressed or emphasized.
				self.horseLatitude = 30
				self.polarFrontLatitude = 60

				#Tropics of Cancer and Capricorn plus and minus respectively
				self.tropicsLatitude = 23

				#Oceans are slow to gain and lose heat, so the max and min temps
				#are reduced and raised by this much.
				self.oceanTempClamp = .10

				#Minimum amount of rain dropped by default before other factors
				#add to the amount of rain dropped
				self.minimumRainCost = 0.01

				#Strength of geostrophic rainfall versus monsoon rainfall
				self.geostrophicFactor = 6.0

				#Monsoon uplift factor. This value is an ajustment so that monsoon uplift
				#matches geostrophic uplift.
				self.monsoonUplift = 500.0

				#Height and Width of main climate and height maps. This does not
				#reflect the resulting map size. Both dimensions( + 1 if wrapping in
				#that dimension = False) must be evenly divisble by self.hmMaxGrain
				self.hmWidth = 144
				self.hmHeight = 97

				#Controls wrapping (not sure if this makes sense yet)
				self.WrapX = True
				self.WrapY = False

				#Size of largest map increment to begin midpoint displacement. Must
				#be a power of 2.
				self.hmMaxGrain = 16

				#Option to divide map into two continents as far as the midpoint
				#displacement is concerned. For guaranteed continent separation, further
				#steps will be needed but this option will cause more ocean in the
				#middle of the map. The possible choices are 0 = NO_SEPARATION,
				#1 = NORTH_SOUTH_SEPARATION and 2 = EAST_WEST_SEPARATION.
				self.hmSeparation = 0

				#Creates a water margin around the map edges.
				self.northMargin = False
				self.southMargin = False
				self.eastMargin = False
				self.westMargin = False

				#If you sink the margins all the way to 0.0, they become too obvious.
				#This variable sets the maximum amount of sinking
				self.hmMarginDepth = 0.60

				#Margin of ocean around map edge when not wrapping and also through
				#middle when using separation.
				self.hmGrainMargin = 2

				#These are not mountain peaks, but points on the height map initialized
				#to 1.0 before the midpoint displacement process begins. This sets the
				#percentage of 'peaks' for points that are not on the grain margin.
				self.hmInitialPeakPercent = 0.30

				#Scales the heuristic for random midpoint displacement. A higher number
				#will create more noise(bumpy), a smaller number will make less
				#noise(smooth).
				self.hmNoiseLevel = 2.0

				#Number of tectonic plates
				self.hmNumberOfPlates = int(float(self.hmWidth * self.hmHeight) * 0.0016)

				#Influence of the plate map, or how much of it is added to the height map.
				self.plateMapScale = 1.1

				#Minimun distance from one plate seed to another
				self.minSeedRange = 15

				#Minimum distance from a plate seed to edge of map
				self.minEdgeRange = 5

				#Chance for plates to grow. Higher chance tends to make more regular
				#shapes. Lower chance makes more irregular shapes and takes longer.
				self.plateGrowthChanceX = 0.3
				self.plateGrowthChanceY = 0.3

				#This sets the amount that tectonic plates differ in altitude.
				self.plateStagger = 0.1

				#This sets the max amount a plate can be staggered up to on the heightmap
				self.plateStaggerRange = 1.0

				#This is the chance for a plate to sink into the water when it is on map edge
				self.chanceForWaterEdgePlate = 0.45

				#This is the frequency of the cosine ripple near plate boundaries.
				self.rippleFrequency = 0.5

				#This is the amplitude of the ripples near plate boundaries.
				self.rippleAmplitude = 0.75

				#This is the amount of noise added to the plate map.
				self.plateNoiseFactor = 1.2

				#Filter size for temperature smoothing. Must be odd number
				self.filterSize = 15

				#Filter size for altitude smoothing and distance finding. Must be
				#odd number
				self.distanceFilterSize = 5

				#It is necessary to eliminate small inland lakes during the initial
				#heightmap generation. Keep in mind this number is in relation to
				#the initial large heightmap (mc.hmWidth, mc.hmHeight) before the
				#shrinking process
				self.minInlandSeaSize = 100

				#After generating the heightmap, bands of ocean can be added to the map
				#to allow a more consistent climate generation. These bands are useful
				#if you are generating part of a world where the weather might be coming
				#in from off the map. These bands can be kept if needed or cropped off
				#later in the process.
				self.northWaterBand = 10
				self.southWaterBand = 10
				self.eastWaterBand = 0
				self.westWaterBand = 0

				#These variables are intended for use with the above water band variables
				#but you can crop the map edge after climate generation for any reason.
				self.northCrop = 10
				self.southCrop = 10
				self.eastCrop = 0
				self.westCrop = 0

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

				#Randomly allows wonder class bonuses (stone and marble) to be used to sweeten starting positions.
				#(Chance per starting position to allow 1 wonder bonus)
				self.allowStrategicBonusChance = 0.05

				#Randomly allows bonuses with continent limiter to be used to sweeten starting positions.
				#(Chance per attempt to place an area-restricted resource in the wrong area)
				self.ignoreAreaRestrictionChance = 0.05

				#The following values are used for assigning starting locations. For now,
				#they have the same ratio that is found in CvPlot::getFoundValue
				self.CommerceValue = 20
				self.ProductionValue = 40
				self.FoodValue = 10

				#Coastal cities are important, how important is determined by this
				#value.
				self.CoastalCityValueBonus = 1.3

				#River side cities are also important, how important is determined by this
				#value.
				self.RiverCityValueBonus = 1.2

				#Bonus resources to add depending on difficulty settings
				self.SettlerBonus = 0
				self.ChieftainBonus = 0
				self.WarlordBonus = 0
				self.NobleBonus = 0
				self.PrinceBonus = 0
				self.MonarchBonus = 0
				self.EmperorBonus = 1
				self.ImmortalBonus = 1
				self.DeityBonus = 2
				self.NightmareBonus = 2

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
				self.L = 0
				self.N = 1
				self.S = 2
				self.E = 3
				self.W = 4
				self.NE = 5
				self.NW = 6
				self.SE = 7
				self.SW = 8

				self.NO_SEPARATION = 0
				self.NORTH_SOUTH_SEPARATION = 1
				self.EAST_WEST_SEPARATION = 2

				self.width = 104
				self.height = 64

				self.OCEAN = 0
				self.LAND = 1
				self.HILLS = 2
				self.PEAK = 3

				self.OCEAN = 0
				self.COAST = 1
				self.MARSH = 2
				self.GRASS = 3
				self.PLAINS = 4
				self.DESERT = 5
				self.TUNDRA = 6
				self.ICE = 7
				self.SALT_FLATS = 8
				self.DUNES = 9
				self.SCRUB = 10
				self.ROCKY = 11
				self.BARREN = 12
				self.LUSH = 13
				self.MUDDY = 14
				self.PERMAFROST = 15
				self.TAIGA = 16
				self.ROCKY_ARID = 17
				self.ROCKY_COLD = 18

				self.minimumLandInChoke = 0.5

				return

		def initInGameOptions(self):
				gc = CyGlobalContext()
				mmap = gc.getMap()
				#New World Rules
				selectionID = mmap.getCustomMapOption(0)
				if selectionID == 1:
						self.AllowNewWorld = not self.AllowNewWorld
				#Pangaea Rules
				selectionID = mmap.getCustomMapOption(1)
				if selectionID == 1:
						self.AllowPangeas = not self.AllowPangeas

				#Wrap options
				selectionID = mmap.getCustomMapOption(2)
				wrapString = "Cylindrical"
				if selectionID == 1: #Toroidal
						self.hmHeight -= 1
						self.WrapY = True
						self.northWaterBand = 0
						self.northCrop = 0
						self.southWaterBand = 0
						self.southCrop = 0
						wrapString = "Toroidal"

				elif selectionID == 2: #Flat
						self.hmWidth += 1
						self.WrapX = False
						wrapString = "Flat"

				self.optionsString = "Map Options: \n"
				if self.AllowNewWorld:
						self.optionsString += "AllowNewWorld = true\n"
				else:
						self.optionsString += "AllowNewWorld = false\n"
				if self.AllowPangeas:
						self.optionsString += "AllowPangeas = true\n"
				else:
						self.optionsString += "AllowPangeas = false\n"
				self.optionsString += "Wrap Option = " + wrapString + "\n"


				return

mc = MapConstants()

class PythonRandom:
	def __init__(self):
		return


	def seed(self):
		#Python randoms are not usable in network games.
		#AIAndy: I disagree. Python randoms are deterministic so the only important thing is to seed from a synchronized source like MapRand.
		if mc.UsePythonRandom:
			self.usePR = True
		else:
			self.usePR = False
		#if self.usePR and CyGame().isNetworkMultiPlayer():
		#	print "Detecting network game. Setting UsePythonRandom to False."
		#	self.usePR = False
		if self.usePR:
			if CyGame().isNetworkMultiPlayer():
			# seed Python random with MapRand
				gc = CyGlobalContext()
				self.mapRand = gc.getGame().getMapRand()
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
		else:
			gc = CyGlobalContext()
			self.mapRand = gc.getGame().getMapRand()

			seedValue = self.mapRand.get(65535, "Seeding mapRand - FairWeather.py")
			self.mapRand.init(seedValue)
			self.seedString = "Random seed (Using getMapRand) for this map is %(s)20d" % {"s" :seedValue}


	def random(self):
		if self.usePR:
			return random()
		else:
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
		else:
			#mapRand.get() is not inclusive, so we must make it so
			return rMin + self.mapRand.get(rMax + 1 - rMin, "Getting a randint - FairWeather.py")


PRand = PythonRandom()

################################################################################
## Global functions
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

#This function converts x and y to an index.
def GetIndex(x,y):
		#Check X for wrap
		if mc.WrapX == True:
				xx = x % mc.width
		elif x < 0 or x >= mc.width:
				return -1
		else:
				xx = x
		#Check y for wrap
		if mc.WrapY == True:
				yy = y % mc.height
		elif y < 0 or y >= mc.height:
				return -1
		else:
				yy = y

		i = yy * mc.width + xx
		return i

def GetHmIndex(x,y):
		#Check X for wrap
		if mc.WrapX == True:
				xx = x % mc.hmWidth
		elif x < 0 or x >= mc.hmWidth:
				return -1
		else:
				xx = x
		#Check y for wrap
		if mc.WrapY == True:
				yy = y % mc.hmHeight
		elif y < 0 or y >= mc.hmHeight:
				return -1
		else:
				yy = y

		i = yy * mc.hmWidth + xx
		return i

#Handles arbitrary size
def GetIndexGeneral(x,y,width,height):
		#Check X for wrap
		if mc.WrapX == True:
				xx = x % width
		elif x < 0 or x >= width:
				return -1
		else:
				xx = x
		#Check y for wrap
		if mc.WrapY == True:
				yy = y % height
		elif y < 0 or y >= height:
				return -1
		else:
				yy = y

		i = yy * width + xx
		return i


#This function scales a float map so that all values are between
#0.0 and 1.0.
def NormalizeMap(fMap,width,height):
		#find highest and lowest points
		maxAlt = 0.0
		minAlt = 0.0
		for y in range(height):
				for x in range(width):
						plot = fMap[GetIndexGeneral(x,y,width,height)]
						if plot > maxAlt:
								maxAlt = plot
						if plot < minAlt:
								minAlt = plot
		#normalize map so that all altitudes are between 1 and 0
		#first add minAlt to all values if necessary
		if minAlt < 0.0:
				for y in range(height):
						for x in range(width):
								fMap[GetIndexGeneral(x,y,width,height)] -= minAlt
		#add minAlt to maxAlt also before scaling entire map
		maxAlt -= minAlt
		scaler = 1.0/maxAlt
		for y in range(height):
				for x in range(width):
						fMap[GetIndexGeneral(x,y,width,height)] = fMap[GetIndexGeneral(x,y,width,height)] * scaler
		return
def ShrinkMap(largeMap,lWidth,lHeight,sWidth,sHeight):
		smallMap = array('d')
		yScale = float(lHeight)/float(sHeight)
		xScale = float(lWidth)/float(sWidth)
		for y in range(sHeight):
				for x in range(sWidth):
##						print "x = %d, y = %d" % (x,y)
						weights = 0.0
						contributors = 0.0
						yyStart = int(y * yScale)
						yyStop = int((y + 1) * yScale)
						if yyStop < ((y + 1) * yScale):
								yyStop += 1
						for yy in range(yyStart,yyStop):
								xxStart = int(x * xScale)
								xxStop = int((x + 1) * xScale)
								if xxStop < ((x + 1) * xScale):
										xxStop += 1
								for xx in range(xxStart,xxStop):
##										print "	 xx = %d, yy = %d" % (xx,yy)
										weight = GetWeight(x,y,xx,yy,xScale,yScale)
##										print "	 weight = %f" % weight
										i = yy * lWidth + xx
##										print "	 i = %d" % i
										contributor = largeMap[i]
##										print "	 contributer = %f" % contributor
										weights += weight
										contributors += weight * contributor
##						print " final height = %f" % (contributors/weights)
						smallMap.append(contributors/weights)

		return smallMap
def GetWeight(x,y,xx,yy,xScale,yScale):
		xWeight = 1.0
##		print "		xScale = %f" % xScale
##		print "		x * xScale = %f, xx = %f" % ((x * xScale),xx)
		if float(xx) < x * xScale:
##				print "		first"
				xWeight = 1.0 - ((x * xScale) - float(xx))
		elif float(xx + 1) > (x + 1) * xScale:
##				print "		second"
				xWeight = ((x + 1) * xScale) - float(xx)
##		print "		xWeight = %f" % xWeight

		yWeight = 1.0
##		print "		yScale = %f" % yScale
##		print "		y * yScale = %f, yy = %f" % ((y * yScale),yy)
		if float(yy) < y * yScale:
##				print "		first"
				yWeight = 1.0 - ((y * yScale) - float(yy))
		elif float(yy + 1) > (y + 1) * yScale:
##				print "		second"
				yWeight = ((y + 1) * yScale) - float(yy)
##		print "		yWeight = %f" % yWeight

		return xWeight * yWeight

def CropMap(theMap):
		newMap = array('d')
		for y in range(mc.hmHeight):
				if y < mc.southCrop or y >= mc.hmHeight - mc.northCrop:
						continue
				for x in range(mc.hmWidth):
						if x < mc.westCrop or x >= mc.hmWidth - mc.eastCrop:
								continue
						i = GetHmIndex(x,y)
						newMap.append(theMap[i])
		return newMap

def AngleDifference(a1,a2):
		diff = a1 - a2
		while(diff < -180.0):
				diff += 360.0
		while(diff > 180.0):
				diff -= 360.0
		return diff

def ShuffleList(theList):
	shufflePyList(theList, CyGame().getMapRand())
	return theList

def GetInfoType(string):
	cgc = CyGlobalContext()
	return cgc.getInfoTypeForString(string)

def GetDistance(x,y,dx,dy):
		distance = math.sqrt(abs((float(x - dx) * float(x - dx)) + (float(y - dy) * float(y - dy))))
		return distance

def GetXYFromDirection(x,y,direction):
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

##This function is a general purpose value tuner. It finds a value that will be greater
##than or less than the desired percent of a whole map within a given tolerance. Map values
##should be between 0 and 1. To exclude parts of the map, set them to value 0.0
def FindValueFromPercent(mmap,width,height,percent,tolerance,greaterThan):
		inTolerance = False
		#to speed things up a little, lets take some time to find the middle value
		#in the dataset and use that to begin our search
		minV = 100.0
		maxV = 0.0
		totalCount = 0
		for i in range(height*width):
				if mmap[i] != 0.0:
						totalCount += 1
						if minV > mmap[i]:
								minV = mmap[i]
						if maxV < mmap[i]:
								maxV = mmap[i]
		mid = (maxV - minV)/2.0 + minV
		overMinCount = 0
		equalMinCount = 0
		for i in range(height*width):
				if mmap[i] > minV:
						overMinCount += 1
				elif mmap[i] == minV:
						equalMinCount += 1
##		print "--------------------------------------------------------------"
##		print "totalCount = %d" % totalCount
##		print "overMinCount = %d" % overMinCount
##		print "equalMinCount = %d" % equalMinCount
##		print "starting threshold = %f" % mid
##		print "desired percent = %f" % percent
##		print "minV = %f, maxV = %f" % (minV,maxV)

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
				for i in range(height*width):
						if mmap[i] != 0.0:
								if greaterThan == True:
										if(mmap[i] > threshold):
												matchCount += 1
								else:
										if(mmap[i] < threshold):
												matchCount += 1
##				print "current threshold = %f" % threshold
##				print "current thresholdChange = %f" % thresholdChange
##				print "matchCount = %d" % matchCount
				currentPercent = float(matchCount)/float(totalCount)
##				print "currentPercent = %f" % currentPercent
				if currentPercent < percent + tolerance and \
					 currentPercent > percent - tolerance:
						inTolerance = True
				elif greaterThan == True:
						if currentPercent < percent:
##								print "threshold subtract"
								threshold -= thresholdChange
								if lastAdded:
										#only cut thresholdChange when direction is changed
										thresholdChange = thresholdChange/2.0
								lastAdded = False
						else:
##								print "threshold add"
								threshold += thresholdChange
								if not lastAdded:
										thresholdChange = thresholdChange/2.0
								lastAdded = True
				else:
						if currentPercent > percent:
##								print "threshold subtract"
								threshold -= thresholdChange
								if lastAdded:
										#only cut thresholdChange when direction is changed
										thresholdChange = thresholdChange/2.0
								lastAdded = False
						else:
##								print "threshold add"
								threshold += thresholdChange
								if not lastAdded:
										thresholdChange = thresholdChange/2.0
								lastAdded = True
##				print "--------------"

				#at this point value should be in tolerance or close to it
##		print "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
		return threshold
def isWaterMatch(x,y):
		result = hm.isBelowSeaLevel(x,y)
##		print "match function results - x = %d,y = %d,result = %d" % (x,y,result)
		return result

class HeightMap :
		def __init__(self):
				return

		def generateHeightMap(self):
				self.heightMap = array('d')
				for i in range(mc.hmWidth * mc.hmHeight):
						self.heightMap.append(0.0)

				self.generateMidpointDisplacement()
				return

		def checkMaxGrain(self):
				#hm map dimensions(minus 1 if no wrapping) must be evenly divisible
				#by max grain
				ok = True
				width = mc.hmWidth
				height = mc.hmHeight
				if mc.WrapX == False:
						width -= 1
				if mc.WrapY == False:
						height -= 1

				if 0 != width % mc.hmMaxGrain:
						ok = False
				if 0 != height % mc.hmMaxGrain:
						ok = False

				if ok == False:
						raise ValueError, "height map dimesions not divisible by mc.hmMaxGrain. also check wrapping options"

				return

		def isPlotOnMargin(self,x,y):
				#first check top and bottom
				if mc.southMargin == True:
						if y < (mc.hmMaxGrain * mc.hmGrainMargin):
								return True
				if mc.northMargin == True:
						if y > (mc.hmHeight - (mc.hmMaxGrain * mc.hmGrainMargin)):
								return True
				#check right and left
				if mc.westMargin == True:
						if x < (mc.hmMaxGrain * mc.hmGrainMargin):
								return True
				if mc.eastMargin == True:
						if x > (mc.hmWidth - (mc.hmMaxGrain * mc.hmGrainMargin)):
								return True

				#now check middle
				if mc.hmSeparation != mc.NO_SEPARATION:
						if mc.hmSeparation == mc.NORTH_SOUTH_SEPARATION:
								dimension = y
								middle = mc.hmHeight/2
						elif mc.hmSeparation == mc.EAST_WEST_SEPARATION:
								dimension = x
								middle = mc.hmWidth/2
						else:
								raise ValueError, "bad hmSeparation type"

						if dimension > middle - (mc.hmMaxGrain * mc.hmGrainMargin) \
						and dimension < middle + (mc.hmMaxGrain * mc.hmGrainMargin):
								return True

				return False

		def generateMidpointDisplacement(self):
				self.checkMaxGrain()

				#make list of map plots that aren't on margin for each
				#map quadrant. We want to place the initial peaks randomly, but we
				#also want to ensure fairly even distribution so that
				#not all the peaks are on one side of the map. For this purpose
				#we will treat each map quadrant separately.

				peaksNWList = list()
				peaksNEList = list()
				peaksSWList = list()
				peaksSEList = list()
				middleX = mc.hmWidth/2
				middleY = mc.hmHeight/2
				for y in range(0,mc.hmHeight,mc.hmMaxGrain):
						for x in range(0,mc.hmWidth,mc.hmMaxGrain):
								if not self.isPlotOnMargin(x,y):
										if x < middleX and y < middleY:
												peaksSWList.append((x,y))
										elif x >= middleX and y < middleY:
												peaksSEList.append((x,y))
										elif x < middleX and y >= middleY:
												peaksNWList.append((x,y))
										elif x >= middleX and y >= middleY:
												peaksNEList.append((x,y))
				#shuffle the lists
				peaksNWList = ShuffleList(peaksNWList)
				peaksNEList = ShuffleList(peaksNEList)
				peaksSWList = ShuffleList(peaksSWList)
				peaksSEList = ShuffleList(peaksSEList)

				#place desired number of peaks in each quadrant
				totalNonMargin = len(peaksNWList)
				totalNonMargin += len(peaksNEList)
				totalNonMargin += len(peaksSWList)
				totalNonMargin += len(peaksSEList)

				count = max(1,int(float(totalNonMargin) * mc.hmInitialPeakPercent * 0.25))
				print "peak count = %d" % (count)
				for n in range(count):
						x,y = peaksNWList[n]
						i = GetHmIndex(x,y)
						self.heightMap[i] = 1.0
						print "%d,%d = 1.0" % (x,y)

						x,y = peaksNEList[n]
						i = GetHmIndex(x,y)
						self.heightMap[i] = 1.0

						x,y = peaksSWList[n]
						i = GetHmIndex(x,y)
						self.heightMap[i] = 1.0

						x,y = peaksSEList[n]
						i = GetHmIndex(x,y)
						self.heightMap[i] = 1.0

				self.printInitialPeaks()

				#Now use a diamond-square algorithm(sort of) to generate the rest
				currentGrain = float(mc.hmMaxGrain)
				while currentGrain > 1.0:
						#h is scalar for random displacement
						h = (currentGrain/float(mc.hmMaxGrain)) * float(mc.hmNoiseLevel)
						#First do the 'square' pass
						for y in range(0,mc.hmHeight,int(currentGrain)):
								for x in range(0,mc.hmWidth,int(currentGrain)):
										#on the square pass, GetHmIndex should handle all wrapping needs
										topLeft = GetHmIndex(x,y)
										topRight = GetHmIndex(x + int(currentGrain),y)
										if topRight == -1:
												continue #this means no wrap in x direction
										bottomLeft = GetHmIndex(x,y + int(currentGrain))
										if bottomLeft == -1:
												continue #this means no wrap in y direction
										bottomRight = GetHmIndex(x + int(currentGrain),y + int(currentGrain))
										middle = GetHmIndex(x + int(currentGrain/2.0),y + int(currentGrain/2.0))
										average = (self.heightMap[topLeft] + self.heightMap[topRight] \
										+ self.heightMap[bottomLeft] + self.heightMap[bottomRight])/4.0
										displacement = h * PRand.random() - h/2.0
										self.heightMap[middle] = average + displacement
										#now add that heuristic to the four points to diminish
										#artifacts. We don't need this on the diamond pass I don't think
										displacement = h * PRand.random() - h/2.0
										self.heightMap[topLeft] += displacement
										displacement = h * PRand.random() - h/2.0
										self.heightMap[topRight] += displacement
										displacement = h * PRand.random() - h/2.0
										self.heightMap[bottomLeft] += displacement
										displacement = h * PRand.random() - h/2.0
										self.heightMap[bottomRight] += displacement
						#Now do the 'diamond' pass, there are two diamonds for each x.
						#Possible wrapping is a big complication on this pass. Sorry!
						for y in range(0,mc.hmHeight,int(currentGrain)):
								for x in range(0,mc.hmWidth,int(currentGrain)):
										#first do the right facing diamond
										left = GetHmIndex(x,y)
										right = GetHmIndex(x + int(currentGrain),y)
										if right != -1: #if we're off map at this point go to next diamond
												average = self.heightMap[left] + self.heightMap[right]
												contributers = 2 #each diamond may have two or three contributers, 2 so far
												top = GetHmIndex(x + int(currentGrain/2.0),y - int(currentGrain/2.0))
												if top != -1:
														contributers += 1
														average += self.heightMap[top]
												bottom = GetHmIndex(x + int(currentGrain/2.0),y + int(currentGrain/2.0))
												if bottom != -1:
														contributers += 1
														average += self.heightMap[bottom]
												average = average/float(contributers)
												middle = GetHmIndex(x + int(currentGrain/2.0),y)
												displacement = h * PRand.random() - h/2.0
												self.heightMap[middle] = average + displacement
										#now do the down facing diamond
										top = GetHmIndex(x,y)
										bottom = GetHmIndex(x,y + int(currentGrain))
										if bottom != -1:
												average = self.heightMap[top] + self.heightMap[bottom]
												contributers = 2
												right = GetHmIndex(x + int(currentGrain/2.0),y + int(currentGrain/2.0))
												if right != -1:
														contributers += 1
														average += self.heightMap[right]
												left = GetHmIndex(x - int(currentGrain/2.0),y + int(currentGrain/2.0))
												if left != -1:
														contributers += 1
														average += self.heightMap[left]
												average = average/float(contributers)
												middle = GetHmIndex(x,y + int(currentGrain/2.0))
												displacement = h * PRand.random() - h/2.0
												self.heightMap[middle] = average + displacement

						currentGrain = currentGrain/2.0

				NormalizeMap(self.heightMap,mc.hmWidth,mc.hmHeight)

				return

		def performTectonics(self):
				self.plateMap = list()
				borderMap = array('i')#this will help in later distance calculations
				self.plateHeightMap = array('d')
				preSmoothMap = array('d')
				growthPlotList = list()
				plateList = list()
				maxDistance = math.sqrt(pow(float(mc.distanceFilterSize/2),2) + pow(float(mc.distanceFilterSize/2),2))
				#initialize maps
				for y in range(mc.hmHeight):
						for x in range(mc.hmWidth):
								i = GetHmIndex(x,y)
								self.plateMap.append(PlatePlot(0,maxDistance))
								borderMap.append(False)
								self.plateHeightMap.append(0.0)
								preSmoothMap.append(0.0)

				plateList.append(Plate(0,-1,-1))#zero placeholder (very silly I know)
				#seed plates
				for i in range(1,mc.hmNumberOfPlates + 1):
						#first find a random seed point that is not blocked by
						#previous points
						iterations = 0
						while(True):
								iterations += 1
								if iterations > 10000:
										raise ValueError, "endless loop in region seed placement"
								seedX = PRand.randint(0,mc.hmWidth + 1)
								seedY = PRand.randint(0,mc.hmHeight + 1)
								n = GetHmIndex(seedX,seedY)
								if self.isSeedBlocked(plateList,seedX,seedY) == False:
										self.plateMap[n].plateID = i
										plate = Plate(i,seedX,seedY)
										plateList.append(plate)
										#Now fill a 3x3 area to insure a minimum region size
										for direction in range(1,9,1):
												xx,yy = GetXYFromDirection(seedX,seedY,direction)
												nn = GetHmIndex(xx,yy)
												if nn != -1:
														self.plateMap[nn].plateID = i
														plot = (xx,yy,i)
														growthPlotList.append(plot)

										break

##				self.printPlateMap(self.plateMap)

				#Now cause the seeds to grow into plates
				iterations = 0
				while(len(growthPlotList) > 0):
						iterations += 1
						if iterations > 200000:
								self.printPlateMap(self.plateMap)
								print "length of growthPlotList = %d" % (len(growthPlotList))
								raise ValueError, "endless loop in plate growth"
						plot = growthPlotList[0]
						roomLeft = False
						for direction in range(1,5,1):
								x,y,plateID = plot
								i = GetHmIndex(x,y)
								xx,yy = GetXYFromDirection(x,y,direction)
								ii = GetHmIndex(xx,yy)
								if ii == -1:
										plateList[plateID].isOnMapEdge = True
										continue
								if self.plateMap[ii].plateID != plateID and self.plateMap[ii].plateID != 0:
										borderMap[i] = True
										borderMap[ii] = True
								elif self.plateMap[ii].plateID == 0:
										roomLeft = True
										if direction == mc.N or direction == mc.S:
												growthChance = mc.plateGrowthChanceY
										else:
												growthChance = mc.plateGrowthChanceX
										if PRand.random() < growthChance:
												self.plateMap[ii].plateID = plateID
												newPlot = (xx,yy,plateID)
												growthPlotList.append(newPlot)


						#move plot to the end of the list if room left, otherwise
						#delete it if no room left
						if roomLeft:
								growthPlotList.append(plot)
						del growthPlotList[0]

##				self.printPlateMap(self.plateMap)

				#to balance the map we want at least one plate stagger upward
				#in each quadrant
				NWfound = False
				NEfound = False
				SEfound = False
				SWfound = False
				for plate in plateList:
						if plate.GetQuadrant() == plate.NW and NWfound == False:
								plate.raiseOnly = True
								NWfound = True
						if plate.GetQuadrant() == plate.NE and NEfound == False:
								plate.raiseOnly = True
								NEfound = True
						if plate.GetQuadrant() == plate.SE and SEfound == False:
								plate.raiseOnly = True
								SEfound = True
						if plate.GetQuadrant() == plate.SW and SWfound == False:
								plate.raiseOnly = True
								SWfound = True

				#Stagger the plates somewhat to add interest
				steps = int(mc.plateStaggerRange/mc.plateStagger)
				for i in range(0,mc.hmHeight*mc.hmWidth):
						if plateList[self.plateMap[i].plateID].isOnMapEdge and PRand.random() < mc.chanceForWaterEdgePlate:
								preSmoothMap[i] = 0.0
						elif plateList[self.plateMap[i].plateID].raiseOnly:
								preSmoothMap[i] = (float(self.plateMap[i].plateID % steps) * mc.plateStagger)/2.0 + 0.5
						else:
								preSmoothMap[i] = float(self.plateMap[i].plateID % steps) * mc.plateStagger

##				self.printPreSmoothMap(preSmoothMap)

				#Now smooth the plate height map and create the distance map at the same time
				#Since the algorithm is the same
				for y in range(0,mc.hmHeight):
						for x in range(0,mc.hmWidth):
								contributers = 0
								avg = 0
								i = GetHmIndex(x,y)
								isBorder = False
								if borderMap[i] == True:
										isBorder = True
								plateID = self.plateMap[i].plateID
								for yy in range(y - mc.distanceFilterSize/2,y + mc.distanceFilterSize/2 + 1,1):
										for xx in range(x - mc.distanceFilterSize/2,x + mc.distanceFilterSize/2 + 1,1):
												ii = GetHmIndex(xx,yy)
												if ii == -1:
														continue
												contributers += 1
												avg += preSmoothMap[ii]
												if isBorder and plateID != self.plateMap[ii].plateID:
														distance = math.sqrt(pow(float(y - yy),2) + pow(float(x - xx),2))
														if distance < self.plateMap[ii].distanceList[plateID]:
																self.plateMap[ii].distanceList[plateID] = distance
								avg = avg/float(contributers)
								self.plateHeightMap[i] = avg

##				self.printPlateHeightMap()
#				 self.printDistanceMap(distanceMap,maxDistance)

				#Now add ripple formula to plateHeightMap
				for i in range(mc.hmWidth*mc.hmHeight):
						avgRippleTop = 0.0
						avgRippleBottom = 0.0
						for plateID in range(1,mc.hmNumberOfPlates + 1):
								distanceWeight = maxDistance - self.plateMap[i].distanceList[plateID]
#								 print "a1 = %f, a2 = %f" % (plateList[self.plateMap[i].plateID].angle,plateList[plateID].angle)
								if plateList[plateID].seedX < plateList[self.plateMap[i].plateID].seedX:
										angleDifference = AngleDifference(plateList[self.plateMap[i].plateID].angle,plateList[plateID].angle)
								else:
										angleDifference = AngleDifference(plateList[plateID].angle,plateList[self.plateMap[i].plateID].angle)
#								 print angleDifference
								ripple = (pow(math.cos(mc.rippleFrequency * self.plateMap[i].distanceList[plateID]) * \
								(-self.plateMap[i].distanceList[plateID]/maxDistance + 1),2) + (-self.plateMap[i].distanceList[plateID]/maxDistance + 1)) \
								* mc.rippleAmplitude * math.sin(math.radians(angleDifference))
								avgRippleTop += (ripple * distanceWeight)
								avgRippleBottom += distanceWeight
						if avgRippleBottom == 0.0:
								avgRipple = 0.0
						else:
								avgRipple = avgRippleTop/avgRippleBottom
						self.plateHeightMap[i] += avgRipple - (avgRipple * PRand.random() * mc.plateNoiseFactor)

				NormalizeMap(self.plateHeightMap,mc.hmWidth,mc.hmHeight)
##				self.printPlateHeightMap()


		def combineMaps(self):
				#Now add plateHeightMap to HeightMap
				for i in range(mc.hmWidth * mc.hmHeight):
						self.heightMap[i] += self.plateHeightMap[i] * mc.plateMapScale

				#depress margins, this time with brute force
				marginSize = mc.hmMaxGrain * mc.hmGrainMargin
				for y in range(mc.hmHeight):
						for x in range(mc.hmWidth):
								i = GetHmIndex(x,y)
								if mc.westMargin == True:
										if x < marginSize:
												self.heightMap[i] *= (float(x)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth
								if mc.eastMargin == True:
										if mc.hmWidth - x < marginSize:
												self.heightMap[i] *= (float(mc.hmWidth - x)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth
								if mc.southMargin == True:
										if y < marginSize:
												self.heightMap[i] *= (float(y)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth
								if mc.northMargin == True:
										if mc.hmHeight - y < marginSize:
												self.heightMap[i] *= (float(mc.hmHeight - y)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth

								if mc.hmSeparation == mc.NORTH_SOUTH_SEPARATION:
										difference = abs((mc.hmHeight/2) - y)
										if difference < marginSize:
												self.heightMap[i] *= (float(difference)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth

								elif mc.hmSeparation == mc.EAST_WEST_SEPARATION:
										difference = abs((mc.hmWidth/2) - x)
										if difference < marginSize:
												self.heightMap[i] *= (float(difference)/float(marginSize)) * (1.0 - mc.hmMarginDepth) + mc.hmMarginDepth

##				#Now lets square the heightmap to simulate erosion
##				for i in range(mc.hmWidth * mc.hmHeight):
##						self.heightMap[i] = self.heightMap[i] * self.heightMap[i]

				NormalizeMap(self.heightMap,mc.hmWidth,mc.hmHeight)

		def addWaterBands(self):
				#validate water bands. Maps that wrap cannot have one in that direction
				if mc.WrapX and (mc.eastWaterBand != 0 or mc.westWaterBand != 0):
						raise ValueError,"east/west water bands cannot be used when wrapping in X direction."
				if mc.WrapY and (mc.northWaterBand != 0 or mc.southWaterBand != 0):
						raise ValueError,"north/south water bands cannot be used when wrapping in Y direction."

				newWidth = mc.hmWidth + mc.eastWaterBand + mc.westWaterBand
				newHeight = mc.hmHeight + mc.northWaterBand + mc.southWaterBand
				newHeightMap = array('d')
				for y in range(newHeight):
						for x in range(newWidth):
								oldX = x - mc.westWaterBand
								oldY = y - mc.southWaterBand
#								 i = GetIndexGeneral(x,y,newWidth,newHeight)
								ii = GetHmIndex(oldX,oldY)
								if ii == -1:
										newHeightMap.append(0.0)
								else:
										newHeightMap.append(self.heightMap[ii])

				mc.hmWidth = newWidth
				mc.hmHeight = newHeight
				self.heightMap = newHeightMap

		def calculateSeaLevel(self):
				gc = CyGlobalContext()
				land = mc.landPercent
				sea = gc.getSeaLevelInfo(gc.getMap().getSeaLevel()).getSeaLevelChange()
				if sea < 0:
					land *= mc.SeaLevelFactor
				elif sea > 0:
					land /= mc.SeaLevelFactor
				self.seaLevel = FindValueFromPercent(self.heightMap,mc.hmWidth,mc.hmHeight,land,0.02,True)
				return

		def isBelowSeaLevel(self,x,y):
				i = GetHmIndex(x,y)
##				print "heightMap = %f at %d,%d" % (self.heightMap[i],x,y)
##				print "seaLevel = %f" % self.seaLevel
				if self.heightMap[i] < self.seaLevel:
##						print "True"
						return True
##				print "False"
				return False

		## This function returns altitude in relation to sea level with
		## 0.0 being seaLevel and 1.0 being highest altitude
		def getAltitudeAboveSeaLevel(self,x,y):
				i = GetHmIndex(x,y)
				if i == -1:
						return 0.0
				altitude = self.heightMap[i]
				if altitude < self.seaLevel:
						return 0.0
				altitude = 1.0/(1.0 - self.seaLevel) * (altitude - self.seaLevel)
				return altitude

		def setAltitudeAboveSeaLevel(self,x,y,altitude):
				i = GetHmIndex(x,y)
				if i == -1:
						return
				self.heightMap[i] = ((1.0 - self.seaLevel) * altitude) + self.seaLevel

##		def Erode(self):
##				for y in range(mc.hmHeight):
##						for x in range(mc.hmWidth):
##								alt = self.getAltitudeAboveSeaLevel(x,y)
##								if alt > 0:
##										eroded = pow(alt,mc.erosionPower)
##										self.setAltitudeAboveSeaLevel(x,y,eroded)

		def isSeedBlocked(self,plateList,seedX,seedY):
				for plate in plateList:
						if seedX > plate.seedX - mc.minSeedRange and seedX < plate.seedX + mc.minSeedRange:
								if seedY > plate.seedY - mc.minSeedRange and seedY < plate.seedY + mc.minSeedRange:
										return True
				#Check for edge
				if seedX < mc.minEdgeRange or seedX >= (mc.hmWidth + 1) - mc.minEdgeRange:
						return True
				if seedY < mc.minEdgeRange or seedY >= (mc.hmHeight + 1) - mc.minEdgeRange:
						return True
				return False
		def GetInfluFromDistance(self,sinkValue,peakValue,searchRadius,distance):
				influence = peakValue
				maxDistance = math.sqrt(pow(float(searchRadius),2) + pow(float(searchRadius),2))
				#minDistance = 1.0
				influence -= ((peakValue - sinkValue)* (distance - 1.0))/(maxDistance - 1.0)
				return influence
		def FindDistanceToPlateBoundary(self,x,y,searchRadius):
				minDistance = 10.0
				i = self.GetIndex(x,y)
				for yy in range(y - searchRadius,y + searchRadius):
						for xx in range(x - searchRadius,x + searchRadius):
								ii = self.GetIndex(xx,yy)
								if self.plateMap[i] != self.plateMap[ii]:
										distance = math.sqrt(pow(float(xx-x),2) + pow(float(yy-y),2))
										if distance < minDistance:
												minDistance = distance

				if minDistance == 10.0:
						return 0.0

				return minDistance

		def fillInLakes(self):
				#smaller lakes need to be filled in for now. The river system will
				#most likely recreate them later due to drainage calculation
				#according to certain rules. This makes the lakes look much better
				#and more sensible.
				am = Areamap(mc.hmWidth,mc.hmHeight,True,True)
				am.defineAreas(isWaterMatch)
##				am.PrintAreaMap()
				oceanID = am.getOceanID()
				for y in range(mc.hmHeight):
						for x in range(mc.hmWidth):
								i = GetHmIndex(x,y)
								if self.isBelowSeaLevel(x,y) and am.areaMap[i] != oceanID:
										#check the size of this body of water, if too small,
										#change to land
										for a in am.areaList:
												if a.ID == am.areaMap[i] and a.size < mc.minInlandSeaSize:
														self.heightMap[i] = self.seaLevel

				return

		def printInitialPeaks(self):
				lineString = "midpoint displacement peaks and margins"
				print lineString
				if not mc.WrapY:
						adjustedHeight = mc.hmHeight - 1
				else:
						adjustedHeight = mc.hmHeight - mc.hmMaxGrain
				for y in range(adjustedHeight,-1,-mc.hmMaxGrain):
						lineString = ""
						for x in range(0,mc.hmWidth,mc.hmMaxGrain):
								i = GetHmIndex(x,y)
								if self.isPlotOnMargin(x,y):
										lineString += "*"
								elif self.heightMap[i] == 1.0:
										lineString += "1"
								elif self.heightMap[i] == 0.0:
										lineString += "0"
						print lineString
				lineString = " "
				print lineString

		def printHeightMap(self):
				lineString = "Height Map"
				print lineString
				for y in range(mc.hmHeight - 1,-1,-1):
						lineString = ""
						for x in range(0,mc.hmWidth,1):
								i = GetHmIndex(x,y)
								mapLoc = int((self.heightMap[i] - self.seaLevel)/(1.0 - self.seaLevel) * 10)
								#mapLoc = int(self.heightMap[i] * 10)
								if self.heightMap[i] < self.seaLevel:
										lineString += '.'
								else:
										lineString += chr(mapLoc + 48)
						print lineString
				lineString = " "
				print lineString

		def printPlateMap(self,plateMap):
				lineString = "Plate Map"
				print lineString
				for y in range(mc.hmHeight - 1,-1,-1):
						lineString = ""
						for x in range(0,mc.hmWidth,1):
								i = GetHmIndex(x,y)
								mapLoc = plateMap[i].plateID
								if mapLoc > 40:
										mapLoc = 41
								lineString += chr(mapLoc + 48)
						print lineString
				lineString = " "
				print lineString

		def printPreSmoothMap(self,preSmoothMap):
				lineString = "Pre-Smooth Map"
				print lineString
				for y in range(mc.hmHeight - 1,-1,-1):
						lineString = ""
						for x in range(0,mc.hmWidth,1):
								i = GetHmIndex(x,y)
								mapLoc = int(preSmoothMap[i] * 40)
								lineString += chr(mapLoc + 48)
						print lineString
				lineString = " "
				print lineString

		def printPlateHeightMap(self):
				lineString = "Plate Height Map"
				print lineString
				for y in range(mc.hmHeight - 1,-1,-1):
						lineString = ""
						for x in range(0,mc.hmWidth,1):
								i = GetHmIndex(x,y)
								mapLoc = int(self.plateHeightMap[i] * 40)
								lineString += chr(mapLoc + 48)
						print lineString
				lineString = " "
				print lineString

		def printDistanceMap(self,distanceMap,maxDistance):
				lineString = "Plate Height Map"
				print lineString
				for y in range(mc.hmHeight - 1,-1,-1):
						lineString = ""
						for x in range(0,mc.hmWidth,1):
								i = GetHmIndex(x,y)
								mapLoc = int((distanceMap[i]/maxDistance) * 40)
								lineString += chr(mapLoc + 48)
						print lineString
				lineString = " "
				print lineString

class Plate :
		def __init__(self,ID,seedX,seedY):
				self.ID = ID
				self.seedX = seedX
				self.seedY = seedY
				self.isOnMapEdge = False
				self.angle = (PRand.random() * 360) - 180
				self.raiseOnly = False
				self.NW = 0
				self.NE = 1
				self.SE = 2
				self.SW = 3
		def GetQuadrant(self):
				if self.seedY < mc.hmHeight/2:
						if self.seedX < mc.hmWidth/2:
								return self.SW
						else:
								return self.SE
				else:
						if self.seedX < mc.hmWidth/2:
								return self.NW
						else:
								return self.NE

class PlatePlot :
		def __init__(self,plateID,maxDistance):
				self.plateID = plateID
				self.distanceList = list()
				for i in range(mc.hmNumberOfPlates + 1):
						self.distanceList.append(maxDistance)

class ClimateMap :
		def __init__(self):
				return
		def createClimateMaps(self):
				summerSunMap = array('d')
				winterSunMap = array('d')
				self.summerTempsMap = array('d')
				self.winterTempsMap = array('d')
				self.averageTempMap = array('d')
				self.moistureMap = array('d')
				self.rainFallMap = array('d')

				self.initializeTempMap(summerSunMap,mc.tropicsLatitude)
				self.initializeTempMap(winterSunMap,-mc.tropicsLatitude)

				#smooth both sun maps into the temp maps
				for y in range(0,mc.hmHeight):
						for x in range(0,mc.hmWidth):
								contributers = 0
								summerAvg = 0
								winterAvg = 0
								i = GetHmIndex(x,y)
								for yy in range(y - mc.filterSize/2,y + mc.filterSize/2 + 1,1):
										for xx in range(x - mc.filterSize/2,x + mc.filterSize/2 + 1,1):
												ii = GetHmIndex(xx,yy)
												if ii == -1:
														continue
												contributers += 1
												summerAvg += summerSunMap[ii]
												winterAvg += winterSunMap[ii]
								summerAvg = summerAvg/float(contributers)
								winterAvg = winterAvg/float(contributers)
								self.summerTempsMap.append(summerAvg)
								self.winterTempsMap.append(winterAvg)

				#create average temp map
				for y in range(mc.hmHeight):
						for x in range(mc.hmWidth):
								i = GetHmIndex(x,y)
								#average summer and winter
								avgTemp = (self.summerTempsMap[i] + self.winterTempsMap[i])/2.0
								#cool map for altitude
								self.averageTempMap.append(avgTemp * (1.0 - pow(hm.getAltitudeAboveSeaLevel(x,y),mc.temperatureLossCurve) * mc.heatLostAtOne))

				#init moisture and rain maps
				for i in range(mc.hmHeight*mc.hmWidth):
						self.moistureMap.append(0.0)
						self.rainFallMap.append(0.0)

				#create sortable plot list for summer monsoon rains
				temperatureList = list()
				for y in range(mc.hmHeight):
						for x in range(mc.hmWidth):
								i = GetHmIndex(x,y)
								rainPlot = RainPlot(x,y,self.summerTempsMap[i],0)
								temperatureList.append(rainPlot)
				#sort by temperature, coldest first
				temperatureList.sort(lambda x,y:cmp(x.order,y.order))

				#Drop summer monsoon rains
				self.dropRain(temperatureList,self.summerTempsMap,False,None)

				#clear moisture map
				for i in range(mc.hmHeight*mc.hmWidth):
						self.moistureMap[i] = 0.0

				#create sortable plot list for winter monsoon rains
				temperatureList = list()
				for y in range(mc.hmHeight):
						for x in range(mc.hmWidth):
								i = GetHmIndex(x,y)
								rainPlot = RainPlot(x,y,self.winterTempsMap[i],0)
								temperatureList.append(rainPlot)
				#sort by temperature, coldest first
				temperatureList.sort(lambda x,y:cmp(x.order,y.order))

				#Drop winter monsoon rains
				self.dropRain(temperatureList,self.winterTempsMap,False,None)

				#clear moisture map
				for i in range(mc.hmHeight*mc.hmWidth):
						self.moistureMap[i] = 0.0

				#set up WindZones class
				wz = WindZones(mc.hmHeight,mc.topLatitude,mc.bottomLatitude)

				#create ordered list for geostrophic rain
				orderList = list()
				for zone in range(6):
						topY = wz.GetYFromZone(zone,True)
						bottomY = wz.GetYFromZone(zone,False)
						if topY == -1 and bottomY == -1:
								continue #This wind zone is not represented on this map at all so skip it
						if topY == -1: #top off map edge
								topY = mc.hmHeight - 1
						if bottomY == -1:
								bottomY = 0

						dx,dy = wz.GetWindDirectionsInZone(zone)
						if dy < 0:
								yStart = topY
								yStop = bottomY - 1
						else:
								yStart = bottomY
								yStop = topY + 1
						if dx < 0:
								xStart = mc.hmWidth - 1
								xStop = -1
						else:
								xStart = 0
								xStop = mc.hmWidth
						order = 0.0
						for y in range(yStart,yStop,dy):
								for x in range(xStart,xStop,dx):
										rainPlot = RainPlot(x,y,order,abs(yStop - y))
										orderList.append(rainPlot)
										order += 1.0

				#Sort order list
				orderList.sort(lambda x,y:cmp(x.order,y.order))

				#drop geostrophic rain
				self.dropRain(orderList,self.averageTempMap,True,wz)


				NormalizeMap(self.rainFallMap,mc.hmWidth,mc.hmHeight)

##				self.printRainFallMap(True)
##				self.printRainFallMap(False)


		def dropRain(self,plotList, tempMap, bGeostrophic, windZones):
				countRemaining = len(plotList)
				bDebug = False
				for plot in plotList:
						i = GetHmIndex(plot.x,plot.y)
						if bDebug:
								print "rainplot at %d,%d" % (plot.x,plot.y)
								print "order = %f" % (plot.order)
								print "initial moisture = %f" % (self.moistureMap[i])
						#First collect moisture from sea
						if hm.isBelowSeaLevel(plot.x,plot.y):
								self.moistureMap[i] += tempMap[i]
								if bDebug:
										print "collecting %f moisture from sea" % (tempMap[i])

						nList = list()
						if bGeostrophic:
								#make list of neighbors in geostrophic zone, even if off map
								zone = windZones.GetZone(plot.y)
								dx,dy = windZones.GetWindDirectionsInZone(zone)
								if bDebug:
										if dy < 0:
												yString = "v"
										else:
												yString = "^"
										if dx < 0:
												xString = "<"
										else:
												xString = ">"
										print "Wind direction ------------------------------- %s%s - %s" % (xString,yString,windZones.GetZoneName(zone))
								nList.append((plot.x,plot.y + dy))
								nList.append((plot.x + dx,plot.y))
								nList.append((plot.x + dx,plot.y + dy))

						else:
								#make list of neighbors with higher temp
								for direction in range(1,9,1):
										xx,yy = GetXYFromDirection(plot.x,plot.y,direction)
										ii = GetHmIndex(xx,yy)
										if ii != -1 and tempMap[i] <= tempMap[ii]:
												nList.append((xx,yy))
								#divide moisture by number of neighbors for distribution
								if len(nList) == 0:
										continue #dead end, dump appropriate rain
						moisturePerNeighbor = self.moistureMap[i]/float(len(nList))
						if bDebug:
								print "moisturePerNeighbor = %f for %d neighbors" % (moisturePerNeighbor,len(nList))

						geostrophicFactor = 1.0
						if bGeostrophic:
								geostrophicFactor = mc.geostrophicFactor
						for xx,yy in nList:
								ii = GetHmIndex(xx,yy)
								if bDebug:
										print "	 neighbor %d,%d" % (xx,yy)
										print "	 countRemaining = %d" % countRemaining
								#Get the rain cost to enter this plot. Cost is
								#percentage of present moisture available for this
								#neighbor
								if bGeostrophic:
										cost = self.getRainCost(plot.x,plot.y,xx,yy,plot.uplift)
								else:
										cost = self.getRainCost(plot.x,plot.y,xx,yy,countRemaining/mc.monsoonUplift)

								if bDebug:
										print "	 rain cost = %f" % cost

								#Convert moisture into rain
								#self.moistureMap[i] -= cost * moisturePerNeighbor (this line is unecessary actually, we are finished with moisture map for this plot)
								self.rainFallMap[i] += cost * moisturePerNeighbor * geostrophicFactor #geostrophicFactor is not involved with moisture, only to weigh against monsoons
								if bDebug:
										print "	 dropping %f rain here" % (cost * moisturePerNeighbor)

								#send remaining moisture to neighbor
								if ii != -1:
										self.moistureMap[ii] += moisturePerNeighbor - (cost * moisturePerNeighbor)
										if bDebug:
												print "	 remaining moisture to neighbor = %f" % (moisturePerNeighbor - (cost * moisturePerNeighbor))

						if bDebug:
								print "total rainfall = %f" % self.rainFallMap[i]
						countRemaining -= 1

		def getRainCost(self,x1,y1,x2,y2,distanceToUplift):
				cost = mc.minimumRainCost
				cRange = 1.0 - mc.minimumRainCost/1.0#We don't want to go over 1.0 so the range is reduced
				upliftCost = (1.0/(float(distanceToUplift) + 1.0)) * cRange
				i = GetHmIndex(x1,y1)
				ii = GetHmIndex(x2,y2)
				cost += max((self.averageTempMap[ii] - self.averageTempMap[i]) * 2.0 * cRange,upliftCost)
				return cost

		def initializeTempMap(self,tempMap,tropic):
				for y in range(mc.hmHeight):
						for x in range(mc.hmWidth):
								tempMap.append(self.getInitialTemp(x,y,tropic))
				return
		def getInitialTemp(self,x,y,tropic):
				i = GetHmIndex(x,y)
				lat = self.getLatitude(y)
				latRange = float(90 + abs(tropic))
				latDifference = abs(float(lat - tropic))
				aboveSeaLevel = hm.heightMap[i] > hm.seaLevel
				if aboveSeaLevel:
						tempPerLatChange = 1.0/latRange
						temp = 1.0 - (tempPerLatChange * latDifference)
				else:
						tempPerLatChange = (1.0 - (2.0*mc.oceanTempClamp))/latRange
						temp = 1.0 - mc.oceanTempClamp - (tempPerLatChange * latDifference)

				return temp

		def getLatitude(self,y):
				latitudeRange = mc.topLatitude - mc.bottomLatitude
				degreesPerDY = float(latitudeRange)/float(mc.hmHeight - mc.northCrop - mc.southCrop)
				if y > mc.hmHeight - mc.northCrop:
						return mc.topLatitude
				if y < mc.southCrop:
						return mc.bottomLatitude
				latitude = (mc.bottomLatitude + (int(round(float(y - mc.southCrop)* degreesPerDY))))
				return latitude

		def printRainFallMap(self,bOcean):
				lineString = "Rainfall Map"
				print lineString
				wz = WindZones(mc.hmHeight,mc.topLatitude,mc.bottomLatitude)
				for y in range(mc.hmHeight - 1,-1,-1):
						lineString = ""
						for x in range(0,mc.hmWidth,1):
								i = GetHmIndex(x,y)
								if bOcean:
										if self.rainFallMap[i] < 0.1:
												mapLoc = int(self.rainFallMap[i] * 100)
												lineString += chr(mapLoc + 48)
										else:
												mapLoc = int(self.rainFallMap[i] * 10)
												lineString += chr(mapLoc + 65)
								else:
										if hm.isBelowSeaLevel(x,y):
												lineString += '.'
										elif self.rainFallMap[i] < 0.00001:
												lineString += 'X'
										else:
												if self.rainFallMap[i] < 0.1:
														mapLoc = int(self.rainFallMap[i] * 100)
														lineString += chr(mapLoc + 48)
												else:
														mapLoc = int(self.rainFallMap[i] * 10)
														lineString += chr(mapLoc + 65)
						z = wz.GetZone(y)
						dx,dy = wz.GetWindDirectionsInZone(z)
						lineString += ' - '
						if dx < 0:
								lineString += '<'
						else:
								lineString += '>'
						if dy < 0:
								lineString += 'v'
						else:
								lineString += '^'
						lineString += ' ' + wz.GetZoneName(z)
						print lineString
				lineString = " "
				print lineString
		def printTempMap(self,tempMap):
				lineString = "Temp Map"
				print lineString
				wz = WindZones(mc.hmHeight,mc.topLatitude,mc.bottomLatitude)
				for y in range(mc.hmHeight - 1,-1,-1):
						lineString = ""
						for x in range(0,mc.hmWidth,1):
								i = GetHmIndex(x,y)
								mapLoc = int(tempMap[i] * 10)
								lineString += chr(mapLoc + 48)
						z = wz.GetZone(y)
						dx,dy = wz.GetWindDirectionsInZone(z)
						lineString += ' - '
						if dx < 0:
								lineString += '<'
						else:
								lineString += '>'
						if dy < 0:
								lineString += 'v'
						else:
								lineString += '^'
						lineString += ' ' + wz.GetZoneName(z)
						print lineString
				lineString = " "
				print lineString

class RainPlot :
		def __init__(self,x,y,order,uplift):
				self.x = x
				self.y = y
				self.order = order
				self.uplift = uplift

class WindZones :
		def __init__(self,mapHeight,topLat,botLat):
				self.NPOLAR = 0
				self.NTEMPERATE = 1
				self.NEQUATOR = 2
				self.SEQUATOR = 3
				self.STEMPERATE = 4
				self.SPOLAR = 5
				self.NOZONE = 99
				self.mapHeight = mapHeight
				self.topLat = topLat
				self.botLat = botLat
		def GetZone(self,y):
				if y < 0 or y >= self.mapHeight:
						return self.NOZONE
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
				return
		def GetZoneName(self,zone):
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
				return
		def GetYFromZone(self,zone,bTop):
				if bTop:
						for y in range(self.mapHeight - 1,-1,-1):
								if zone == self.GetZone(y):
										return y
				else:
						for y in range(self.mapHeight):
								if zone == self.GetZone(y):
										return y
				return -1
		def GetZoneSize(self):
				latitudeRange = self.topLat - self.botLat
				degreesPerDY = float(latitudeRange)/float(self.mapHeight)
				size = 30.0/degreesPerDY
				return size
		def GetWindDirections(self,y):
				z = self.GetZone(y)
				#get x,y directions
				return self.GetWindDirectionsInZone(z)
		def GetWindDirectionsInZone(self,z):
				#get x,y directions
				if z == self.NPOLAR:
						return (-1,-1)
				elif z == self.NTEMPERATE:
						return (1,1)
				elif z == self.NEQUATOR:
						return (-1,-1)
				elif z == self.SEQUATOR:
						return (-1,1)
				elif z == self.STEMPERATE:
						return (1,-1)
				elif z == self.SPOLAR:
						return (-1,1)
				return (0,0)

def isSmallWaterMatch(x,y):
		return sm.isBelowSeaLevel(x,y)

class SmallMaps :
		def __init__(self):
				return
		def initialize(self):
				self.cropMaps()
				newHeightMap = ShrinkMap(hm.heightMap,mc.hmWidth ,mc.hmHeight,mc.width,mc.height)
				newRainFallMap = ShrinkMap(cm.rainFallMap,mc.hmWidth,mc.hmHeight,mc.width,mc.height)
				newAverageTempMap = ShrinkMap(cm.averageTempMap,mc.hmWidth,mc.hmHeight,mc.width,mc.height)

				self.heightMap = array('d')
				self.rainFallMap = array('d')
				self.averageTempMap = array('d')

				for y in range(mc.height):
						for x in range(mc.width):
								oldX = x
								i = GetIndexGeneral(oldX,y,mc.width,mc.height)
								if i != -1:
										self.heightMap.append(newHeightMap[i])
										self.rainFallMap.append(newRainFallMap[i])
										self.averageTempMap.append(newAverageTempMap[i])
								else:
										self.heightMap.append(hm.seaLevel - 0.000001)
										self.rainFallMap.append(0.0)
										self.averageTempMap.append(0.0)

				#Smooth coasts so there are fewer hills on coast
				for y in range(mc.height):
						for x in range(mc.width):
								if self.isBelowSeaLevel(x,y):
										i = GetIndex(x,y)
										self.heightMap[i] = hm.seaLevel - 0.000001

				self.fillInLakes()

				self.createPlotMap()
				self.printPlotMap()
				self.createTerrainMap()
				continentMap.generateContinentMap()

		def fillInLakes(self):
				#smaller lakes need to be filled in again because the map
				#shrinker sometimes creates lakes.
				am = Areamap(mc.width,mc.height,True,True)
				am.defineAreas(isSmallWaterMatch)
##				am.PrintAreaMap()
				oceanID = am.getOceanID()
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								if self.isBelowSeaLevel(x,y) and am.areaMap[i] != oceanID:
										#check the size of this body of water, if too small,
										#change to land
										for a in am.areaList:
												if a.ID == am.areaMap[i] and a.size < mc.minInlandSeaSize:
														self.heightMap[i] = hm.seaLevel

				return

		def isBelowSeaLevel(self,x,y):
				i = GetIndex(x,y)
				if self.heightMap[i] < hm.seaLevel:
						return True
				return False

		## This function returns altitude in relation to sea level with
		## 0.0 being seaLevel and 1.0 being highest altitude
		def getAltitudeAboveSeaLevel(self,x,y):
				i = GetIndex(x,y)
				if i == -1:
						return 0.0
				altitude = self.heightMap[i]
				if altitude < hm.seaLevel:
						return 0.0
				altitude = 1.0/(1.0 - hm.seaLevel) * (altitude - hm.seaLevel)
				return altitude


		def createPlotMap(self):
				print "creating plot map"
				self.plotMap = array('i')
				#create height difference map to allow for tuning
				diffMap = array('d')
				for i in range(0,mc.height*mc.width):
						diffMap.append(0.0)
				#I tried using a deviation from surrounding average altitude
				#to determine hills and peaks but I didn't like the
				#results. Therefore I an using lowest neighbor
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								myAlt = self.heightMap[i]
								minAlt = 1.0
								for direction in range(1,9,1):
										xx,yy = GetXYFromDirection(x,y,direction)
										ii = GetIndex(xx,yy)
										if ii == -1:
												continue
										if self.heightMap[ii] < minAlt:
												minAlt = self.heightMap[ii]
								diffMap[i] = myAlt - minAlt

				NormalizeMap(diffMap,mc.width,mc.height)

				#zero out water tiles so percent is percent of land
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								if self.isBelowSeaLevel(x,y):
										diffMap[i] = 0.0

				peakHeight = FindValueFromPercent(diffMap,mc.width,mc.height,mc.PeakPercent,0.01,True)
				hillHeight = FindValueFromPercent(diffMap,mc.width,mc.height,mc.HillPercent,0.01,True)

				self.plotMap = array('i')
				#initialize map with 0CEAN
				for i in range(0,mc.height*mc.width):
						self.plotMap.append(mc.OCEAN)
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								altDiff = diffMap[i]
								if self.heightMap[i] < hm.seaLevel:
										self.plotMap[i] = mc.OCEAN
								elif altDiff < hillHeight:
										self.plotMap[i] = mc.LAND
								elif altDiff < peakHeight:
										self.plotMap[i] = mc.HILLS
								else:
										self.plotMap[i] = mc.PEAK

				#Randomize high altitude areas
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								if self.plotMap[i] == mc.LAND:
										randomNum = PRand.random()
										if randomNum < mc.PeakChanceAtOne * self.getAltitudeAboveSeaLevel(x,y):
												self.plotMap[i] = mc.PEAK
										elif randomNum < mc.HillChanceAtOne * self.getAltitudeAboveSeaLevel(x,y):
												self.plotMap[i] = mc.HILLS

				#break up large clusters of hills and peaks
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								if self.plotMap == mc.HILLS:
										allHills = True
										for direction in range(1,9,1):
												xx,yy = GetXYFromDirection(x,y,direction)
												ii = GetIndex(xx,yy)
												if self.plotMap[ii] != mc.HILLS:
														allHills = False
										if allHills == True:
												self.plotMap[i] = mc.LAND
								if self.plotMap == mc.PEAK:
										allPeaks = True
										for direction in range(1,9,1):
												xx,yy = GetXYFromDirection(x,y,direction)
												ii = GetIndex(xx,yy)
												if self.plotMap[ii] != mc.PEAK:
														allPeaks = False
										if allPeaks == True:
												self.plotMap[i] = mc.HILLS

				return
		def createTerrainMap(self):
				print "creating terrain map"
				self.terrainMap = array('i')
				#initialize terrainMap with OCEAN
				for i in range(0,mc.height*mc.width):
						self.terrainMap.append(mc.OCEAN)

				#Find minimum rainfall on land
				minRain = 10.0
				for i in range(mc.width*mc.height):
						if self.plotMap[i] != mc.OCEAN:
								if self.rainFallMap[i] < minRain:
										minRain = self.rainFallMap[i]

				#zero water tiles to obtain percent of land
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								if self.isBelowSeaLevel(x,y):
										self.rainFallMap[i] = 0.0

				self.desertThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.DesertPercent,.001,False)
				self.saltflatsThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.SaltFlatsPercent,.001,False)
				self.dunesThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.DunesPercent,.001,False)
				self.scrubThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.ScrubPercent,.001,False)
				self.plainsThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.PlainsPercent,.001,False)
				self.rockyThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.RockyPercent,.001,False)
				self.barrenThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.BarrenPercent,.001,False)
				self.grasslandThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.GrasslandPercent,.001,False)
				self.lushThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.LushPercent,.001,False)
				self.muddyThreshold = FindValueFromPercent(self.rainFallMap,mc.width,mc.height,mc.MuddyPercent,.001,False)
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								if self.plotMap[i] == mc.OCEAN:
										for direction in range (1,9,1):
												xx,yy = GetXYFromDirection(x,y,direction)
												ii = GetIndex(xx,yy)
												if ii == -1:
														continue
												if self.plotMap[ii] != mc.OCEAN:
														self.terrainMap[i] = mc.COAST
								elif self.averageTempMap[i] < mc.IceTemp:
										self.terrainMap[i] = mc.ICE
								elif self.averageTempMap[i] < mc.PermafrostTemp:
										if self.plotMap[i] == mc.HILLS and random() < 0.21:
												self.terrainMap[i] = mc.ROCKY_COLD
										elif random() < 0.07:
												self.terrainMap[i] = mc.ROCKY_COLD
										else:
												self.terrainMap[i] = mc.PERMAFROST
								elif self.averageTempMap[i] < mc.TundraTemp:
										if self.plotMap[i] == mc.HILLS and random() < 0.21:
												self.terrainMap[i] = mc.ROCKY_COLD
										elif random() < 0.07:
												self.terrainMap[i] = mc.ROCKY_COLD
										else:
												self.terrainMap[i] = mc.TUNDRA
								elif self.averageTempMap[i] < mc.TaigaTemp:
										if self.plotMap[i] == mc.HILLS and random() < 0.21:
												self.terrainMap[i] = mc.ROCKY_COLD
										elif random() < 0.07:
												self.terrainMap[i] = mc.ROCKY_COLD
										else:
												self.terrainMap[i] = mc.TAIGA

								#instead of harsh thresholds, allow a random deviation chance
								#based on how close to the threshold the rainfall is
								elif self.rainFallMap[i] < self.saltflatsThreshold:
										if self.rainFallMap[i] < ((1 + PRand.random()) * (self.saltflatsThreshold - minRain))/2.0 + minRain:
												self.terrainMap[i] = mc.SALT_FLATS
										else:
												self.terrainMap[i] = mc.DUNES

								elif self.rainFallMap[i] < self.dunesThreshold:
										if self.rainFallMap[i] < ((1 + PRand.random()) * (self.dunesThreshold - self.saltflatsThreshold))/2.0 + self.saltflatsThreshold:
												self.terrainMap[i] = mc.DUNES
										elif self.plotMap[i] == mc.HILLS and random() < 0.21:
												self.terrainMap[i] = mc.ROCKY_ARID
										elif random() < 0.07:
												self.terrainMap[i] = mc.ROCKY_ARID
										else:
												self.terrainMap[i] = mc.DESERT

								elif self.rainFallMap[i] < self.desertThreshold:
										if self.rainFallMap[i] < ((1 + PRand.random()) * (self.desertThreshold - self.dunesThreshold))/2.0 + self.dunesThreshold:
												self.terrainMap[i] = mc.DESERT
										elif self.plotMap[i] == mc.HILLS and random() < 0.21:
												self.terrainMap[i] = mc.ROCKY_ARID
										elif random() < 0.07:
												self.terrainMap[i] = mc.ROCKY_ARID
										else:
												self.terrainMap[i] = mc.SCRUB

								elif self.rainFallMap[i] < self.scrubThreshold:
										if self.rainFallMap[i] < ((1 + PRand.random()) * (self.scrubThreshold - self.desertThreshold))/2.0 + self.desertThreshold:
												self.terrainMap[i] = mc.SCRUB
										elif self.plotMap[i] == mc.HILLS and random() < 0.21:
												self.terrainMap[i] = mc.ROCKY_ARID
										elif random() < 0.07:
												self.terrainMap[i] = mc.ROCKY_ARID
										else:
												self.terrainMap[i] = mc.BARREN

								elif self.rainFallMap[i] < self.rockyThreshold:
										if self.rainFallMap[i] < ((1 + PRand.random()) * (self.rockyThreshold - self.scrubThreshold))/2.0 + self.scrubThreshold:
												self.terrainMap[i] = mc.ROCKY
										else:
												self.terrainMap[i] = mc.BARREN

								elif self.rainFallMap[i] < self.barrenThreshold:
										if self.rainFallMap[i] < ((1 + PRand.random()) * (self.barrenThreshold - self.rockyThreshold))/2.0 + self.rockyThreshold:
												self.terrainMap[i] = mc.BARREN
										else:
												self.terrainMap[i] = mc.PLAINS

								elif self.rainFallMap[i] < self.plainsThreshold:
										if self.rainFallMap[i] < ((1 + PRand.random()) * (self.plainsThreshold - self.barrenThreshold))/2.0 + self.barrenThreshold:
												self.terrainMap[i] = mc.PLAINS
										else:
												self.terrainMap[i] = mc.GRASS

								elif self.rainFallMap[i] < self.grasslandThreshold:
										if self.rainFallMap[i] < ((1 + PRand.random()) * (self.grasslandThreshold - self.plainsThreshold))/2.0 + self.plainsThreshold:
												self.terrainMap[i] = mc.GRASS
										else:
												self.terrainMap[i] = mc.LUSH

								elif self.rainFallMap[i] < self.lushThreshold:
										if self.rainFallMap[i] < ((1 + PRand.random()) * (self.lushThreshold - self.grasslandThreshold))/2.0 + self.grasslandThreshold:
												self.terrainMap[i] = mc.LUSH
										else:
												self.terrainMap[i] = mc.MUDDY

								elif self.rainFallMap[i] < self.muddyThreshold:
										if self.rainFallMap[i] < ((1 + PRand.random()) * (self.muddyThreshold - self.lushThreshold))/2.0 + self.lushThreshold:
												self.terrainMap[i] = mc.MUDDY
										else:
												self.terrainMap[i] = mc.MARSH

								else:
										self.terrainMap[i] = mc.MARSH

				#Make sure ice is always higher than tundra, and tundra is always higher than
				#everything else. Also, desert does not blend well with these so change it.
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								if self.terrainMap[i] == mc.OCEAN or self.terrainMap[i] == mc.COAST:
										continue
								if self.terrainMap[i] == mc.ICE:
										lowerFound = False
										for direction in range(1,9,1):
												xx,yy = GetXYFromDirection(x,y,direction)
												ii = GetIndex(xx,yy)
												if ii == -1:
														continue
												if self.terrainMap[ii] != mc.OCEAN and self.terrainMap[ii] != mc.COAST \
												and self.terrainMap[ii] != mc.ICE:
														lowerFound = True
												if self.terrainMap[ii] != mc.TUNDRA and self.terrainMap[ii] != mc.PERMAFROST and self.terrainMap[ii] != mc.ICE and\
												self.terrainMap[ii] != mc.OCEAN and self.terrainMap[ii] != mc.COAST:
														self.terrainMap[i] = mc.TUNDRA
												if self.terrainMap[ii] == mc.DESERT:
														self.terrainMap[i] = mc.PLAINS
										if lowerFound and self.plotMap[i] == mc.LAND:
												self.plotMap[i] = mc.HILLS
								elif self.terrainMap[i] == mc.TUNDRA:
										lowerFound = False
										for direction in range(1,9,1):
												xx,yy = GetXYFromDirection(x,y,direction)
												ii = GetIndex(xx,yy)
												if ii == -1:
														continue
												if self.terrainMap[ii] != mc.OCEAN and self.terrainMap[ii] != mc.PERMAFROST and self.terrainMap[ii] != mc.COAST \
												and self.terrainMap[ii] != mc.ICE and self.terrainMap[ii] != mc.TUNDRA:
														lowerFound = True
												if self.terrainMap[ii] == mc.DESERT:
														self.terrainMap[i] = mc.PLAINS
										if lowerFound and self.plotMap[i] == mc.LAND:
												if PRand.random() < 0.5:
														self.plotMap[i] = mc.HILLS
												else:
														self.plotMap[i] = mc.PEAK
								else:
										higherFound = False
										for direction in range(1,9,1):
												xx,yy = GetXYFromDirection(x,y,direction)
												ii = GetIndex(xx,yy)
												if self.terrainMap[ii] == mc.ICE or self.terrainMap[ii] == mc.PERMAFROST or self.terrainMap[ii] == mc.TUNDRA:
														higherFound = True
										if higherFound and self.plotMap[i] != mc.LAND:
												self.plotMap[i] = mc.LAND
				return
		def cropMaps(self):
				hm.heightMap = CropMap(hm.heightMap)
				cm.averageTempMap = CropMap(cm.averageTempMap)
				cm.rainFallMap = CropMap(cm.rainFallMap)
				mc.hmWidth = mc.hmWidth - mc.westCrop - mc.eastCrop
				mc.hmHeight = mc.hmHeight - mc.northCrop - mc.southCrop

		def printHeightMap(self):
				lineString = "Height Map"
				print lineString
				for y in range(mc.height - 1,-1,-1):
						lineString = ""
						for x in range(0,mc.width,1):
								i = GetIndexGeneral(x,y,mc.width,mc.height)
								mapLoc = int((self.heightMap[i] - hm.seaLevel)/(1.0 - hm.seaLevel) * 10)
								#mapLoc = int(self.heightMap[i] * 10)
								if self.heightMap[i] < hm.seaLevel:
										lineString += '.'
								else:
										lineString += chr(mapLoc + 48)
						print lineString
				lineString = " "
				print lineString

		def printPlotMap(self):
				print "Plot Map"
				for y in range(mc.height - 1,-1,-1):
						lineString = ""
						for x in range(mc.width):
								mapLoc = self.plotMap[GetIndex(x,y)]
								if mapLoc == mc.PEAK:
										lineString += 'A'
								elif mapLoc == mc.HILLS:
										lineString += 'n'
								elif mapLoc == mc.LAND:
										lineString += '+'
								else:
										lineString += '.'
						print lineString
				lineString = " "
				print lineString
		def printTerrainMap(self):
				print "Terrain Map"
				wz = WindZones(mc.height,80,-80)
				for y in range(mc.height - 1,-1,-1):
						lineString = ""
						for x in range(mc.width):
								mapLoc = self.terrainMap[GetIndex(x,y)]
								if mapLoc == mc.OCEAN:
										lineString += ','
								elif mapLoc == mc.COAST:
										lineString += '.'
								elif mapLoc == mc.DESERT:
										lineString += 'D'
								elif mapLoc == mc.GRASS:
										lineString += '+'
								elif mapLoc == mc.MARSH:
										lineString += 'M'
								elif mapLoc == mc.PLAINS:
										lineString += 'P'
								elif mapLoc == mc.TUNDRA:
										lineString += 'T'
								elif mapLoc == mc.ICE:
										lineString += 'I'
						lineString += "-" + wz.GetZoneName(wz.GetZone(y))
						print lineString
				lineString = " "
				print lineString

def isHmWaterMatch(x,y):
		i = GetHmIndex(x,y)
		if pb.distanceMap[i] > mc.minimumMeteorSize/3:
				return True
		return False

class PangaeaBreaker :
		def __init__(self):
				return
		def breakPangaeas(self):
				self.areaMap = Areamap(mc.hmWidth,mc.hmHeight,True,True)
				meteorThrown = False
				pangeaDetected = False
##				anotherPangaea = True
#				 self.cm.PrintAreaMap()
##				while anotherPangaea:
##						NormalizeMap(hm.heightMap,mc.hmWidth,mc.hmHeight)
##						hm.calculateSeaLevel()
				self.createDistanceMap()
##				self.printDistanceMap()
				self.areaMap.defineAreas(isHmWaterMatch)
##				self.areaMap.PrintAreaMap()
				meteorCount = 0
				while not mc.AllowPangeas and self.isPangea() and meteorCount < mc.maximumMeteorCount:
						pangeaDetected = True
						x,y = self.getMeteorStrike()
						print "A meteor has struck the Earth at %(x)d, %(y)d!!" % {"x":x,"y":y}
						self.castMeteorUponTheEarth(x,y)
						meteorThrown = True
						meteorCount += 1
##						hm.printHeightMap()
						self.createDistanceMap()
##						self.printDistanceMap()
						self.areaMap.defineAreas(isHmWaterMatch)
##						self.areaMap.PrintAreaMap()
##						anotherPangaea = False

				if meteorThrown:
						print "The age of dinosours has come to a cataclysmic end."
				if meteorCount == 15:
						print "Maximum meteor count of %d has been reached. Pangaea may still exist." % meteorCount
				if mc.AllowPangeas:
						print "Pangeas are allowed on this map and will not be suppressed."
				elif pangeaDetected == False:
						print "No pangea detected on this map."
##				self.areaMap.PrintAreaMap()
		def isPangea(self):
##				starttime = time.clock()
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
				if 0.70 < float(biggestSize)/float(totalLand):
##						endtime = time.clock()
##						elapsed = endtime - starttime
##						print "isPangea time = %(t)s" % {"t":str(elapsed)}
						return True
##				endtime = time.clock()
##				elapsed = endtime - starttime
##				print "isPangea time = "
##				print elapsed
##				print
				return False
		def getMeteorStrike(self):
##				starttime = time.clock()
				continentList = list()
				for a in self.areaMap.areaList:
						if a.water == False:
								continentList.append(a)

				#sort all the continents by size, largest first
				continentList.sort(lambda x,y:cmp(x.size,y.size))
				continentList.reverse()
				biggestContinentID = continentList[0].ID

				x,y = self.getHighestCentrality(biggestContinentID)

				return x,y

##				chokeList = list()
##				for y in range(mc.hmHeight):
##						for x in range(mc.hmWidth):
##								i = GetHmIndex(x,y)
##								if i == -1:
##										continue
##								if self.areaMap.areaMap[i] == biggestContinentID and\
##								not hm.isBelowSeaLevel(x,y): #this helps narrow the search
##										if self.isChokePoint(x,y,biggestContinentID):
####												print "chokepoint area at %d,%d = %d" % (x,y,biggestContinentID)
##												ap = AreaPlot(x,y)
##												chokeList.append(ap)
##				#calculate distances to center
##				center = self.getContinentCenter(biggestContinentID)
##				xCenter,yCenter = center
##
##				for n in range(len(chokeList)):
##						distance = self.getDistance(chokeList[n].x,chokeList[n].y,xCenter,yCenter)
##						chokeList[n].avgDistance = distance
##
##				#sort plotList for most avg distance and chokeList for least
##				#average distance
##				chokeList.sort(lambda x,y:cmp(x.avgDistance,y.avgDistance))
##
##				if len(chokeList) == 0:#return bad value if no chokepoints
####						endtime = time.clock()
####						elapsed = endtime - starttime
####						print "getMeteorStrike time = "
####						print elapsed
####						print
##						return -1,-1
##
####				endtime = time.clock()
####				elapsed = endtime - starttime
####				print "getMeteorStrike time = "
####				print elapsed
####				print
##
##				return chokeList[0].x,chokeList[0].y

		def isChokePoint(self,x,y,biggestContinentID):
				circlePoints = self.getCirclePoints(x,y,mc.minimumMeteorSize)
				waterOpposite = False
				landOpposite = False
				for cp in circlePoints:
						if isHmWaterMatch(cp.x,cp.y):
								#Find opposite
								ox = x + (x - cp.x)
								oy = y + (y - cp.y)
								if isHmWaterMatch(ox,oy):
										waterOpposite = True
						else:
								#Find opposite
								ox = x + (x - cp.x)
								oy = y + (y - cp.y)
								if not isHmWaterMatch(ox,oy):
										landOpposite = True
				if landOpposite and waterOpposite:
						percent = self.getLandPercentInCircle(circlePoints,biggestContinentID)
						if percent >= mc.minimumLandInChoke:
								return True
				return False
		def getLandPercentInCircle(self,circlePoints,biggestContinentID):
				land = 0
				water = 0
				circlePoints.sort(lambda n,m:cmp(n.y,m.y))
				for n in range(0,len(circlePoints),2):
						cy = circlePoints[n].y
						if circlePoints[n].x < circlePoints[n + 1].x:
								x1 = circlePoints[n].x
								x2 = circlePoints[n + 1].x
						else:
								x2 = circlePoints[n].x
								x1 = circlePoints[n + 1].x
						landLine,waterLine = self.countCraterLine(x1,x2,cy,biggestContinentID)
						land += landLine
						water += waterLine
				percent = float(land)/float(land + water)
				return percent

		def countCraterLine(self,x1,x2,y,biggestContinentID):
				land = 0
				water = 0
				for x in range(x1,x2 + 1):
						i = GetHmIndex(x,y)
						if self.areaMap.areaMap[i] == biggestContinentID:
								land += 1
						else:
								water += 1
				return land,water

		def getContinentCenter(self,ID):
				IDCount = 0
				xTotal = 0
				yTotal = 0
				for y in range(mc.hmHeight):
						for x in range(mc.hmWidth):
								i = GetHmIndex(x,y)
								if self.areaMap.areaMap[i] == ID:
										IDCount += 1
										xTotal += x
										yTotal += y
				xCenter = round(float(xTotal)/float(IDCount))
				yCenter = round(float(yTotal)/float(IDCount))
##				#first find center in x direction
##				changes = list()
##				yMin = mc.height
##				yMax = -1
##				meridianOverlap = False
##				onContinent = False
##				for x in range(mc.hmWidth):
##						continentFoundThisPass = False
##						for y in range(mc.hmHeight):
##								i = GetHmIndex(x,y)
##								if self.areaMap.areaMap[i] == ID:
##										continentFoundThisPass = True
##										if y < yMin:
##												yMin = y
##										elif y > yMax:
##												yMax = y
##						if x == 0 and continentFoundThisPass:
##								meridianOverlap = True
##								onContinent = True
##						if onContinent and not continentFoundThisPass:
##								changes.append(x)
##								onContinent = False
##						elif not onContinent and continentFoundThisPass:
##								changes.append(x)
##								onContinent = True
##				changes.sort()
##				xCenter = -1
##				if len(changes) == 0: #continent is continuous
##						xCenter = -1
##				elif len(changes) == 1:#continent extends to map edge
##						if meridianOverlap:
##								xCenter = changes[0]/2
##						else:
##								xCenter = (mc.width - changes[0])/2 + changes[0]
##				else:
##						if meridianOverlap:
##								xCenter = ((changes[1] - changes[0])/2 + changes[0] + (mc.hmWidth/2)) % mc.hmWidth
##						else:
##								xCenter = (changes[1] - changes[0])/2 + changes[0]
##				yCenter = (yMax - yMin)/2 + yMin
				center = xCenter,yCenter
				return center

		def getDistance(self,x,y,dx,dy):
				xx = x - dx
				if abs(xx) > mc.hmWidth/2:
						xx = mc.hmWidth - abs(xx)

				distance = max(abs(xx),abs(y - dy))
				return distance
		def castMeteorUponTheEarth(self,x,y):
##				starttime = time.clock()
				radius = PRand.randint(mc.minimumMeteorSize,max(mc.minimumMeteorSize + 1,mc.hmWidth/16))
				circlePointList = self.getCirclePoints(x,y,radius)
##				print "circlePointList"
##				print circlePointList
				circlePointList.sort(lambda n,m:cmp(n.y,m.y))
				for n in range(0,len(circlePointList),2):
						cy = circlePointList[n].y
						if circlePointList[n].x < circlePointList[n + 1].x:
								x1 = circlePointList[n].x
								x2 = circlePointList[n + 1].x
						else:
								x2 = circlePointList[n].x
								x1 = circlePointList[n + 1].x
						self.drawCraterLine(x1,x2,cy)

##				for n in range(0,len(circlePointList),2): not needed since this is happening on heightmap only
##						cy = circlePointList[n].y
##						if circlePointList[n].x < circlePointList[n + 1].x:
##								x1 = circlePointList[n].x
##								x2 = circlePointList[n + 1].x
##						else:
##								x2 = circlePointList[n].x
##								x1 = circlePointList[n + 1].x
##						self.drawCraterCoastLine(x1,x2,cy)
				return

##		def drawCraterCoastLine(self,x1,x2,y): not needed since this is happening on heightmap only
##				if y < 0 or y >= mc.hmHeight:
##						return
##				for x in range(x1,x2 + 1):
##						if self.hasLandNeighbor(x,y):
##								i = GetHmIndex(x,y)
##								sm.terrainMap[i] = mc.COAST
##				return
		def drawCraterLine(self,x1,x2,y):
				if y < 0 or y >= mc.hmHeight:
						return
				for x in range(x1,x2 + 1):
						i = GetHmIndex(x,y)
##						sm.terrainMap[i] = mc.OCEAN
						hm.heightMap[i] = 0.0
##						sm.plotMap[i] = mc.OCEAN
				return
##		def hasLandNeighbor(self,x,y): not needed since this is happening on heightmap only
##				#y cannot be outside of map so I'm not testing for it
##				for yy in range(y - 1,y + 2):
##						for xx in range(x - 1,x + 2):
##								if yy == y and xx == x:
##										continue
##								ii = GetHmIndex(xx,yy)
##								if sm.terrainMap[ii] != mc.COAST and sm.terrainMap[ii] != mc.OCEAN:
##										return True
##				return False
		def getCirclePoints(self,xCenter,yCenter,radius):
				circlePointList = list()
				x = 0
				y = radius
				p = 1 - radius

				self.addCirclePoints(xCenter,yCenter,x,y,circlePointList)

				while (x < y):
						x += 1
						if p < 0:
								p += 2*x + 1
						else:
								y -= 1
								p += 2*(x - y) + 1
						self.addCirclePoints(xCenter,yCenter,x,y,circlePointList)

				return circlePointList

		def addCirclePoints(self,xCenter,yCenter,x,y,circlePointList):
				circlePointList.append(CirclePoint(xCenter + x,yCenter + y))
				circlePointList.append(CirclePoint(xCenter - x,yCenter + y))
				circlePointList.append(CirclePoint(xCenter + x,yCenter - y))
				circlePointList.append(CirclePoint(xCenter - x,yCenter - y))
				circlePointList.append(CirclePoint(xCenter + y,yCenter + x))
				circlePointList.append(CirclePoint(xCenter - y,yCenter + x))
				circlePointList.append(CirclePoint(xCenter + y,yCenter - x))
				circlePointList.append(CirclePoint(xCenter - y,yCenter - x))
				return

		def createDistanceMap(self):
				self.distanceMap = array('i')
				processQueue = []
				for y in range(mc.hmHeight):
						for x in range(mc.hmWidth):
								if hm.isBelowSeaLevel(x,y):
										self.distanceMap.append(1000)
								else:
										self.distanceMap.append(0)
										processQueue.append((x,y))

				while len(processQueue) > 0:
						x,y = processQueue[0]
						i = GetHmIndex(x,y)
						del processQueue[0]
						distanceToLand = self.distanceMap[i]
						for direction in range(1,9,1):
								xx,yy = GetXYFromDirection(x,y,direction)
								ii = GetHmIndex(xx,yy)
								neighborDistanceToLand = self.distanceMap[ii]
								if neighborDistanceToLand > distanceToLand + 1:
										self.distanceMap[ii] = distanceToLand + 1
										processQueue.append((xx,yy))

		def printDistanceMap(self):
				lineString = "Pangaea Breaker distance map"
				print lineString
				for y in range(mc.hmHeight - 1,-1,-1):
						lineString = ""
						for x in range(0,mc.hmWidth,1):
								i = GetHmIndex(x,y)
								mapLoc = int(self.distanceMap[i])
								#mapLoc = int(self.heightMap[i] * 10)
								if mapLoc > 9:
										lineString += '.'
								else:
										lineString += chr(mapLoc + 48)
						print lineString
				lineString = " "
				print lineString

		def getHighestCentrality(self,ID):
				C = self.createCentralityList(ID)
				C.sort(lambda x,y:cmp(x.centrality,y.centrality))
				C.reverse()
				return C[0].x,C[0].y
		def createContinentList(self,ID):
				C = []
				indexMap = []
				gap = 5
				n = 0
				for y in range(0,mc.hmHeight):
						for x in range(0,mc.hmWidth):
								i = GetHmIndex(x,y)
##								print "h %% gap = %d, w %% gap = %d" % (y % gap,y % gap)
								if y % gap == 0 and x % gap == 0 and \
								self.areaMap.areaMap[i] == ID:
										C.append(CentralityScore(x,y))
										indexMap.append(n)
										n += 1
								else:
										indexMap.append(-1)

				n = 0
				for s in C:
##						print "s at %d,%d index %d is neighbors with" % (s.x,s.y,n)
						#Check 4 nieghbors
						xx = s.x - gap
						if xx < 0:
								xx = mc.hmWidth/gap * gap
						i = GetHmIndex(xx,s.y)
						if i != -1 and self.areaMap.areaMap[i] == ID:
								s.neighborList.append(indexMap[i])
##								print "%d,%d index %d" % (xx,s.y,indexMap[i])
						xx = s.x + gap
						if xx >= mc.hmWidth:
								xx = 0
						i = GetHmIndex(xx,s.y)
						if i != -1 and self.areaMap.areaMap[i] == ID:
								s.neighborList.append(indexMap[i])
##								print "%d,%d index %d" % (xx,s.y,indexMap[i])
						yy = s.y - gap
						if yy < 0:
								yy = mc.hmHeight/gap * gap
						i = GetHmIndex(s.x,yy)
						if i != -1 and self.areaMap.areaMap[i] == ID:
								s.neighborList.append(indexMap[i])
##								print "%d,%d index %d" % (s.x,yy,indexMap[i])
						yy = s.y + gap
						if yy > mc.hmHeight:
								yy = 0
						i = GetHmIndex(s.x,yy)
						if i != -1 and self.areaMap.areaMap[i] == ID:
								s.neighborList.append(indexMap[i])
##								print "%d,%d index %d" % (s.x,yy,indexMap[i])

						n += 1

##				self.areaMap.PrintAreaMap()
##				self.printContinentList(ID,gap)

				return C

		def printContinentList(self,ID,gap):
				lineString = "Continent neighbor map"
				print lineString
				for y in range(mc.hmHeight - 1,-1,-1):
						lineString = ""
						for x in range(0,mc.hmWidth,1):
								i = GetHmIndex(x,y)
								if y % gap == 0 and x % gap == 0 and \
								self.areaMap.areaMap[i] == ID:
										lineString += '@'
								elif self.areaMap.areaMap[i] == ID:
										lineString += '*'
								else:
										lineString += '.'
						print lineString
				lineString = " "
				print lineString


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
##								print len(Q)
								S.append(v)
								for w in C[v].neighborList:
										if d[w] < 0:
												Q.append(w)
#														 print len(Q)
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
##						print s

				return C

		def isNeighbor(self,x,y,xx,yy):
						#Check X for wrap
				if mc.WrapX == True:
						mx = xx % mc.hmWidth
				elif x < 0 or x >= mc.hmWidth:
						return False
				else:
						mx = xx
				#Check y for wrap
				if mc.WrapY == True:
						my = yy % mc.hmHeight
				elif y < 0 or y >= mc.hmHeight:
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

class CirclePoint :
		def __init__(self,x,y):
				self.x = x
				self.y = y
class CentralityScore :
		def __init__(self,x,y):
				self.x = x
				self.y = y
				self.centrality = 0
				self.neighborList = []

def isNonCoastWaterMatch(x,y):
		i = GetIndex(x,y)
		if sm.terrainMap[i] == mc.OCEAN:
				return True
		return False

class ContinentMap :
		def __init__(self):
				return
		def generateContinentMap(self):
				self.areaMap = Areamap(mc.width,mc.height,True,True)
				self.areaMap.defineAreas(isNonCoastWaterMatch)
##				self.areaMap.PrintAreaMap()
				self.newWorldID = self.getNewWorldID()
#				 self.cm.PrintAreaMap()
				return
		def getNewWorldID(self):
				nID = 0
				continentList = list()
				for a in self.areaMap.areaList:
						if a.water == False:
								continentList.append(a)

				totalLand = 0
				for c in continentList:
						totalLand += c.size

##				print totalLand

				#sort all the continents by size, largest first
#				 continentList.sort(key=operator.attrgetter('size'),reverse=True)
				continentList.sort(lambda x,y:cmp(x.size,y.size))
				continentList.reverse()

				print ''
				print "All continents"
				print self.areaMap.PrintList(continentList)

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
#				 continentList.sort(key=operator.attrgetter('ID'),reverse=True)
				continentList.sort(lambda x,y:cmp(x.ID,y.ID))
				continentList.reverse()

				for n in range(len(continentList)):
						oldWorldSize += continentList[0].size
						del continentList[0]
						if float(oldWorldSize)/float(totalLand) > 0.60:
								break

				#add back the biggestNewWorld continent
				continentList.append(biggestNewWorld)

				#what remains in the list will be considered 'New World'
				print ''
				print "New World Continents"
				print self.areaMap.PrintList(continentList)

				#get ID for the next continent, we will use this ID for 'New World'
				#designation
				nID = continentList[0].ID
				del continentList[0] #delete to avoid unnecessary overwrite

				#now change all the remaining continents to also have nID as their ID
				for i in range(mc.height*mc.width):
						for c in continentList:
								if c.ID == self.areaMap.areaMap[i]:
										self.areaMap.areaMap[i] = nID

				return nID
continentMap = ContinentMap()

class Areamap :
		def __init__(self,width,height,b8connected,bSwitch4Or8OnFalseMatch):
				self.mapWidth = width
				self.mapHeight = height
				self.areaMap = array('i')
				self.b8connected = b8connected
				self.bSwitch4Or8OnFalseMatch = bSwitch4Or8OnFalseMatch
				#initialize map with zeros
				for i in range(0,self.mapHeight*self.mapWidth):
						self.areaMap.append(0)
				return
		def defineAreas(self,matchFunction):
#				 self.areaSizes = array('i')
##				starttime = time.clock()
				self.areaList = list()
				areaID = 0
				#make sure map is erased in case it is used multiple times
				for i in range(0,self.mapHeight*self.mapWidth):
						self.areaMap[i] = 0
#				 for i in range(0,1):
				for i in range(0,self.mapHeight*self.mapWidth):
						if self.areaMap[i] == 0: #not assigned to an area yet
								areaID += 1
								areaSize,match = self.fillArea(i,areaID,matchFunction)
								area = Area(areaID,areaSize,match)
								self.areaList.append(area)

##				endtime = time.clock()
##				elapsed = endtime - starttime
##				print "defineAreas time ="
##				print elapsed
##				print

				return

##		def isWater(self,x,y,coastIsLand):
##				#coastIsLand = True means that we are trying to find continents that
##				#are not connected by coasts to the main landmasses, allowing us to
##				#find continents suitable as a 'New World'. Otherwise, we
##				#are just looking to fill in lakes and coast needs to be considered
##				#water in that case
##				ii = self.getIndex(x,y)
##				if ii == -1:
##						return False
##				if coastIsLand:
##						if hm.plotMap[ii] == hm.OCEAN and terr.terrainMap[ii] != terr.COAST:
##								return True
##						else:
##								return False
##				else:
##						if hm.isBelowSeaLevel(x,y):
##								return True
##						else:
##								return False
##
##				return False
		def getAreaByID(self,areaID):
				for i in range(len(self.areaList)):
						if self.areaList[i].ID == areaID:
								return self.areaList[i]
				return None
		def getOceanID(self):
#				 self.areaList.sort(key=operator.attrgetter('size'),reverse=True)
				self.areaList.sort(lambda x,y:cmp(x.size,y.size))
				self.areaList.reverse()
				for a in self.areaList:
						if a.water == True:
								return a.ID

		def getIndex(self,x,y):
				#Check X for wrap
				if mc.WrapX == True:
						xx = x % self.mapWidth
				elif x < 0 or x >= self.mapWidth:
						return -1
				else:
						xx = x
				#Check y for wrap
				if mc.WrapY == True:
						yy = y % self.mapHeight
				elif y < 0 or y >= self.mapHeight:
						return -1
				else:
						yy = y

				i = yy * self.mapWidth + xx
				return i

		def fillArea(self,index,areaID,matchFunction):
				#first divide index into x and y
				y = index/self.mapWidth
				x = index%self.mapWidth
				#We check 8 neigbors for land,but 4 for water. This is because
				#the game connects land squares diagonally across water, but
				#water squares are not passable diagonally across land
				self.segStack = list()
				self.size = 0
				matchValue = matchFunction(x,y)
				#place seed on stack for both directions
				seg = LineSegment(y,x,x,1)
				self.segStack.append(seg)
				seg = LineSegment(y+1,x,x,-1)
				self.segStack.append(seg)
				while(len(self.segStack) > 0):
						seg = self.segStack.pop()
						self.scanAndFillLine(seg,areaID,matchValue,matchFunction)
##						if (seg.y < 8 and seg.y > 4) or (seg.y < 70 and seg.y > 64):
##						if (areaID == 4
##								PrintPlotMap(hm)
##								self.PrintAreaMap()

				return self.size,matchFunction(x,y)
		def scanAndFillLine(self,seg,areaID,matchValue,matchFunction):
				#check for y + dy being off map
				i = self.getIndex(seg.xLeft,seg.y + seg.dy)
				if i < 0:
##						print "scanLine off map ignoring",str(seg)
						return
				debugReport = False
##				if (seg.y < 8 and seg.y > 4) or (seg.y < 70 and seg.y > 64):
##				if (areaID == 4):
##						debugReport = True
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
				if debugReport:
						print ""
						print "areaID = %(a)4d" % {"a":areaID}
						print "matchValue = %(w)2d, landOffset = %(l)2d" % {"w":matchValue,"l":landOffset}
						print str(seg)
						print "Going left"
				if mc.WrapX == True:
						xStop = 0 - (self.mapWidth*20)
				else:
						xStop = -1
				for xLeftExtreme in range(seg.xLeft - landOffset,xStop,-1):
						i = self.getIndex(xLeftExtreme,seg.y + seg.dy)
						if debugReport:
								print "xLeftExtreme = %(xl)4d" % {'xl':xLeftExtreme}
						if debugReport:
								print "i = %d, seg.y + seg.dy = %d" % (i,seg.y + seg.dy)
								print "areaMap[i] = %d, matchValue match = %d" % (self.areaMap[i],matchValue == matchFunction(xLeftExtreme,seg.y + seg.dy))
						if self.areaMap[i] == 0 and matchValue == matchFunction(xLeftExtreme,seg.y + seg.dy):
								self.areaMap[i] = areaID
								self.size += 1
								lineFound = True
						else:
								#if no line was found, then xLeftExtreme is fine, but if
								#a line was found going left, then we need to increment
								#xLeftExtreme to represent the inclusive end of the line
								if lineFound:
										xLeftExtreme += 1
								break
				if debugReport:
						print "xLeftExtreme finally = %(xl)4d" % {'xl':xLeftExtreme}
						print "Going Right"
				#now scan right to find extreme right, place each found segment on stack
#				 xRightExtreme = seg.xLeft - landOffset #needed sometimes? one time it was not initialized before use.
				xRightExtreme = seg.xLeft #needed sometimes? one time it was not initialized before use.
				if mc.WrapX == True:
						xStop = self.mapWidth*20
				else:
						xStop = self.mapWidth
				for xRightExtreme in range(seg.xLeft + lineFound - landOffset,xStop,1):
						if debugReport:
								print "xRightExtreme = %(xr)4d" % {'xr':xRightExtreme}
						i = self.getIndex(xRightExtreme,seg.y + seg.dy)
						if debugReport:
								print "i = %d, seg.y + seg.dy = %d" % (i,seg.y + seg.dy)
								print "areaMap[i] = %d, matchValue match = %d" % (self.areaMap[i],matchValue == matchFunction(xRightExtreme,seg.y + seg.dy))
						if self.areaMap[i] == 0 and matchValue == matchFunction(xRightExtreme,seg.y + seg.dy):
								self.areaMap[i] = areaID
								self.size += 1
								if lineFound == False:
										lineFound = True
										xLeftExtreme = xRightExtreme #starting new line
										if debugReport:
												print "starting new line at xLeftExtreme= %(xl)4d" % {'xl':xLeftExtreme}
						elif lineFound == True: #found the right end of a line segment!
								lineFound = False
								#put same direction on stack
								newSeg = LineSegment(seg.y + seg.dy,xLeftExtreme,xRightExtreme - 1,seg.dy)
								self.segStack.append(newSeg)
								if debugReport:
										print "same direction to stack",str(newSeg)
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
								if debugReport:
										print "no additional lines found"
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
								if debugReport:
										print str(newSeg)

				return
		#for debugging
		def PrintAreaMap(self):

				print "Area Map"
				for y in range(self.mapHeight - 1,-1,-1):
						lineString = ""
						for x in range(self.mapWidth):
								mapLoc = self.areaMap[self.getIndex(x,y)]
								if mapLoc + 34 > 127:
										mapLoc = 127 - 34
								lineString += chr(mapLoc + 34)
						lineString += "-" + str(y)
						print lineString
				oid = self.getOceanID()
				if oid == None or oid + 34 > 255:
						print "Ocean ID is unknown"
				else:
						print "Ocean ID is %(oid)4d or %(c)s" % {'oid':oid,'c':chr(oid + 34)}
				lineString = " "
				print lineString

				return
		def PrintList(self,s):
				for a in s:
						char = chr(a.ID + 34)
						lineString = str(a) + ' ' + char
						print lineString

class LineSegment :
		def __init__(self,y,xLeft,xRight,dy):
				self.y = y
				self.xLeft = xLeft
				self.xRight = xRight
				self.dy = dy
		def __str__ (self):
				string = "y = %(y)3d, xLeft = %(xl)3d, xRight = %(xr)3d, dy = %(dy)2d" % \
				{'y':self.y,'xl':self.xLeft,'xr':self.xRight,'dy':self.dy}
				return string

class Area :
		def __init__(self,iD,size,water):
				self.ID = iD
				self.size = size
				self.water = water

		def __str__(self):
				string = "{ID = %(i)4d, size = %(s)4d, water = %(w)1d}" % \
				{'i':self.ID,'s':self.size,'w':self.water}
				return string
class AreaPlot :
		def __init__(self,x,y):
				self.x = x
				self.y = y
				self.avgDistance = -1

#OK! now that directions N,S,E,W are important, we have to keep in mind that
#the map plots are ordered from 0,0 in the SOUTH west corner! NOT the northwest
#corner! That means that Y increases as you go north.
class RiverMap :
		def __init__(self):
				#To provide global access without allocating alot of resources for
				#nothing, object initializer must be empty
				return
		def generateRiverMap(self):
				self.L = 0 #also denotes a 'pit' or 'flat'
				self.N = 1
				self.S = 2
				self.E = 3
				self.W = 4
				self.NE = 5
				self.NW = 6
				self.SE = 7
				self.SW = 8
				self.O = 5 #used for ocean or land without a river

				#averageHeightMap, flowMap, averageRainfallMap and drainageMap are offset from the other maps such that
				#each element coincides with a four tile intersection on the game map
				self.averageHeightMap = array('d')
				self.flowMap = array('i')
				self.averageRainfallMap = array('d')
				self.drainageMap = array('d')
				self.riverMap = array('i')
				#initialize maps with zeros
				for i in range(0,mc.height*mc.width):
						self.averageHeightMap.append(0.0)
						self.flowMap.append(0)
						self.averageRainfallMap.append(0.0)
						self.drainageMap.append(0.0)
						self.riverMap.append(self.O)
				#Get highest intersection neighbor
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								maxHeight = 0.0
								for yy in range(y,y-2,-1):
										for xx in range(x,x+2):
												ii = GetIndex(xx,yy)
												#use an average hight of <0 to denote an ocean border
												#this will save processing time later
												if(sm.plotMap[ii] == mc.OCEAN):
														maxHeight = -100.0
												elif maxHeight < sm.heightMap[ii] and maxHeight >= 0:
														maxHeight = sm.heightMap[ii]
								self.averageHeightMap[i] = maxHeight
				#Now try to silt in any lakes
				self.siltifyLakes()
				self.createLakeDepressions()
				#create flowMap by checking for the lowest of each 4 connected
				#neighbor plus self
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
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
										self.flowMap[i] = self.L
										ii = GetIndex(x,y+1)
										#in the y direction, avoid wrapping
										if(y > 0 and self.averageHeightMap[ii] < lowestAlt):
												drainList.append(self.N)
										else:
												nonDrainList.append(self.N)
										ii = GetIndex(x,y-1)
										if(y < mc.height - 1 and self.averageHeightMap[ii] < lowestAlt):
												drainList.append(self.S)
										else:
												nonDrainList.append(self.S)
										ii = GetIndex(x-1,y)
										if(self.averageHeightMap[ii] < lowestAlt):
												drainList.append(self.W)
										else:
												nonDrainList.append(self.W)
										ii = GetIndex(x+1,y)
										if(self.averageHeightMap[ii] < lowestAlt):
												drainList.append(self.E)
										else:
												nonDrainList.append(self.E)

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
												choice = int(PRand.random()*count)
#												 print count,choice
												self.flowMap[i] = drainList[choice]

				#Create average rainfall map so that each intersection is an average
				#of the rainfall from rm.rainMap
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								avg = 0.0
								for yy in range(y,y-2,-1):
										for xx in range(x,x+2):
												ii = GetIndex(xx,yy)
												avg += sm.rainFallMap[ii]
								avg = avg/4.0
								self.averageRainfallMap[i] = avg

				#Now use the flowMap as a guide to distribute average rainfall.
				#Wherever the most rainfall ends up is where the rivers will be.
				print "Distributing rainfall"
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								flow = self.flowMap[i]
								rainFall = self.averageRainfallMap[i]
								xx = x
								yy = y
								while(flow != self.L and flow != self.O):
										if(flow == self.N):
												yy += 1
										elif(flow == self.S):
												yy -= 1
										elif(flow == self.W):
												xx -= 1
										elif(flow == self.E):
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
										ii = GetIndex(xx,yy)
										self.drainageMap[ii] += rainFall
										#reset flow
										flow = self.flowMap[ii]


				riverThreshold = sm.plainsThreshold * mc.RiverThreshold
				for i in range(mc.height*mc.width):
						if(self.drainageMap[i] > riverThreshold):
##										riverCount += 1
								self.riverMap[i] = self.flowMap[i]
						else:
								self.riverMap[i] = self.O

				#at this point river should be in tolerance or close to it
				#riverMap is ready for use

		def rxFromPlot(self,x,y,direction):
				if direction == self.NE:
						return x,y + 1
				if direction == self.SW:
						return x - 1,y
				if direction == self.SE:
						return x,y
				raise ValueError,"rxFromPlot using bad direction input"

		def siltifyLakes(self):
				lakeList = []
				onQueueMap = array('i')
				for y in range(mc.height):
						for x in range(mc.width):
								onQueueMap.append(0)
								i = GetIndex(x,y)
								if self.isLake(x,y):
										lakeList.append((x,y,1))
										onQueueMap[i] = 1
##				print "initial lakes = %d" % (len(lakeList))
				largestLength = len(lakeList)
				while len(lakeList) > 0:
##						print "len = %d" % (len(lakeList))
						if len(lakeList) > largestLength:
								largestLength = len(lakeList)
						x,y,lakeSize = lakeList[0]
						del lakeList[0]
						i = GetIndex(x,y)
						onQueueMap[i] = 0

						if lakeSize > mc.maxSiltPanSize:
								continue
						lakeSize += 1
##						print x,y
						lowestNeighborAlt = self.getLowestNeighborAltitude(x,y)
						self.averageHeightMap[i] = lowestNeighborAlt + 0.005
						for direction in range(1,5,1):
								xx,yy = GetXYFromDirection(x,y,direction)
								ii = GetIndex(xx,yy)
								if ii == -1:
										continue
								if self.isLake(xx,yy) and onQueueMap[ii] == 0:
##										print "appending lake at %d,%d" % (xx,yy)
										lakeList.append((xx,yy,lakeSize))
										onQueueMap[ii] = 1
##				print "returning from siltify"
##				print "largest length of lakeList = %d" % largestLength
				return
		def isLake(self,x,y):
				i = GetIndex(x,y)
				alt = self.averageHeightMap[i]
				if alt < 0.0:
						return False
				for direction in range(1,5,1):
						xx,yy = GetXYFromDirection(x,y,direction)
						ii = GetIndex(xx,yy)
						if ii == -1:
								continue
						if self.averageHeightMap[ii] < alt:
								return False
				return True
		def getLowestNeighborAltitude(self,x,y):
				lowest = 1.0
				for direction in range(1,5,1):
						xx,yy = GetXYFromDirection(x,y,direction)
						ii = GetIndex(xx,yy)
						if ii == -1:
								continue
						if self.averageHeightMap[ii] < lowest:
								lowest = self.averageHeightMap[ii]
				return lowest
		def createLakeDepressions(self):
				lakeList = []
				for y in range(mc.height):
						for x in range(mc.width):
								i = GetIndex(x,y)
								if self.averageHeightMap[i] > mc.minLakeAltitude:
										lakeList.append((x,y))
				lakeList = ShuffleList(lakeList)
				numLakes = int(mc.height * mc.width * mc.numberOfLakesPerPlot)
				for n in range(numLakes):
						x,y = lakeList[n]
						i = GetIndex(x,y)
						lowestAlt = self.getLowestNeighborAltitude(x,y)
						if lowestAlt < 0.0:
								continue
						self.averageHeightMap[i] = lowestAlt - 0.001


		def printRiverMap(self):
				print "River Map"
				wz = WindZones(mc.height,80,-80)
				for y in range(mc.height - 1,-1,-1):
						lineString = ""
						for x in range(mc.width):
								mapLoc = self.riverMap[GetIndex(x,y)]
								if mapLoc == self.O:
										lineString += '.'
								elif mapLoc == self.L:
										lineString += 'L'
								elif mapLoc == self.N:
										lineString += 'N'
								elif mapLoc == self.S:
										lineString += 'S'
								elif mapLoc == self.E:
										lineString += 'E'
								elif mapLoc == self.W:
										lineString += 'W'
						lineString += "-" + wz.GetZoneName(wz.GetZone(y))
						print lineString
				lineString = " "
				print lineString

		def printFlowMap(self):
				print "Flow Map"
				wz = WindZones(mc.height,80,-80)
				for y in range(mc.height - 1,-1,-1):
						lineString = ""
						for x in range(mc.width):
								mapLoc = self.flowMap[GetIndex(x,y)]
								if mapLoc == self.O:
										lineString += '.'
								elif mapLoc == self.L:
										lineString += 'L'
								elif mapLoc == self.N:
										lineString += 'N'
								elif mapLoc == self.S:
										lineString += 'S'
								elif mapLoc == self.E:
										lineString += 'E'
								elif mapLoc == self.W:
										lineString += 'W'
						lineString += "-" + wz.GetZoneName(wz.GetZone(y))
						print lineString
				lineString = " "
				print lineString
		def printRiverAndTerrainAlign(self):
				print "River Alignment Check"
				for y in range(mc.height - 1,-1,-1):
						lineString1 = ""
						lineString2 = ""
						for x in range(mc.width):
								mapLoc = sm.terrainMap[GetIndex(x,y)]
								if mapLoc == mc.OCEAN:
										lineString1 += ',.'
								elif mapLoc == mc.COAST:
										lineString1 += ',.'
								elif mapLoc == mc.DESERT:
										lineString1 += 'D.'
								elif mapLoc == mc.GRASS:
										lineString1 += 'R.'
								elif mapLoc == mc.PLAINS:
										lineString1 += 'P.'
								elif mapLoc == mc.TUNDRA:
										lineString1 += 'T.'
								elif mapLoc == mc.MARSH:
										lineString1 += 'M.'
								elif mapLoc == mc.ICE:
										lineString1 += 'I.'
								mapLoc = rm.riverMap[GetIndex(x,y)]
								if mapLoc == rm.O:
										lineString2 += '..'
								elif mapLoc == rm.L:
										lineString2 += '.L'
								elif mapLoc == rm.N:
										lineString2 += '.^'
								elif mapLoc == rm.S:
										lineString2 += '.v'
								elif mapLoc == rm.E:
										lineString2 += '.>'
								elif mapLoc == rm.W:
										lineString2 += '.<'
##						lineString1 += "-" + wz.GetZoneName(wz.GetZone(y))
##						lineString2 += "-" + wz.GetZoneName(wz.GetZone(y))
						print lineString1
						print lineString2
				lineString1 = " "
				print lineString1


class BonusPlacer:
	def __init__(self):
		return


	def AddBonuses(self):
		GC = CyGlobalContext()
		GC.getMap().recalculateAreas()

		self.AssignBonusAreas()
		numBonuses = GC.getNumBonusInfos()

		orderSet = {}
		for i in range(numBonuses):  #Check which placement orders are used, discard -1
			bonusInfo = GC.getBonusInfo(self.bonusList[i].eBonus)
			porder = bonusInfo.getPlacementOrder()
			if porder >= 0:
				orderSet[porder] = 1

		plotIndexList = []
		for i in range(mc.width*mc.height):
			plotIndexList.append(i)
		plotIndexList = ShuffleList(plotIndexList)
		startAtIndex = 0

		porderList = sorted(orderSet.keys())
		for order in porderList:
			placementList = []
			for i in range(numBonuses):
				bonusInfo = GC.getBonusInfo(self.bonusList[i].eBonus)
				if bonusInfo.getPlacementOrder() == order:
					for n in range(self.bonusList[i].desiredBonusCount):
						placementList.append(self.bonusList[i].eBonus)
			#Create a list of map indices and shuffle them
			if len(placementList) > 0:
				placementList = ShuffleList(placementList)
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
			bonusInfo = GC.getBonusInfo(bonus.eBonus)
			if bonus.currentBonusCount == 0 and bonus.desiredBonusCount > 0:
				print "No room at all found for %(bt)s!!!" % {"bt":bonusInfo.getType()}
			print "Placed %(cb)d, desired %(db)d for %(bt)s" % {"cb":bonus.currentBonusCount, "db":bonus.desiredBonusCount, "bt":bonusInfo.getType()}

# AIAndy: Changed to start at the end of the last run in the plot list and not shuffle an extra plot list
	def AddEmergencyBonus(self,bonus,ignoreClass, plotIndexList, startAtIndex):
		gc = CyGlobalContext()
		gameMap = CyMap()
		featureForest = gc.getInfoTypeForString("FEATURE_FOREST")
		#plotIndexList = list()
		#for i in range(mc.width*mc.height):
		#	plotIndexList.append(i)
		#plotIndexList = ShuffleList(plotIndexList)
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
		gameMap = CyMap()
		self.areas = gameMap.areas()
		self.bonusList = list()

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

		self.bonusList = ShuffleList(self.bonusList)
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

		if bonusInfo.isBonusCoastalOnly() and not plot.isCoastal():
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
			for i in range(mc.width*mc.height):
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
		gameMap = CyMap()
		areaID = area.getID()
		uniqueTypesInArea		 = self.GetUniqueBonusTypeCountInArea(area)
		sameClassTypesInArea = self.GetSameClassTypeCountInArea(area, eBonus)
		#Get the raw number of suitable tiles
		numPossible = 0
		for i in range(mc.width*mc.height):
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
			gameMap.recalculateAreas()
			areas = gameMap.areas()
			#get old/new world status
			areaOldWorld = self.setupOldWorldAreaList()
			print "len(areaOldWorld) = %d" % len(areaOldWorld)

			self.CachePlotValue()

			#Shuffle players so the same player doesn't always get the first pick.
			#lifted from Highlands.py that ships with Civ.
			player_list = []
			for plrCheckLoop in range(gc.getMAX_PC_PLAYERS()):
				if CyGlobalContext().getPlayer(plrCheckLoop).isEverAlive():
					player_list.append(plrCheckLoop)
			shuffledPlayers = ShuffleList(player_list)
			#for playerLoop in range(iPlayers):
			#		iChoosePlayer = PRand.randint(0, len(player_list) - 1)
			#		shuffledPlayers.append(player_list[iChoosePlayer])
			#		del player_list[iChoosePlayer]
			self.startingAreaList = list()
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
			if len(shuffledPlayers) > 0:
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
		#	errorPopUp("PerfectWorld's starting plot finder has failed due to a rarely occuring bug, and this map likely has unfair starting locations. You may wish to quit this game and generate a new map.")
		#	raise Exception, e


	def setupOldWorldAreaList(self):
		gameMap = CyMap()
		#get official areas and make corresponding lists that determines old
		#world vs. new and also the pre-settled value.
		areas = gameMap.areas()
		areaOldWorld = list()
		for i in range(len(areas)):
			for pI in range(mc.width*mc.height):
				plot = gameMap.plotByIndex(pI)
				if plot.getArea() == areas[i].getID():
					if mc.AllowNewWorld and continentMap.areaMap.areaMap[pI] == continentMap.newWorldID:
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
		food,value = self.getCityPotentialValue(x, y)
		gc = CyGlobalContext()
		gameMap = CyMap()
		game = gc.getGame()
		#Shuffle the bonus order so that different cities have different preferences
		#for bonuses
		bonusList = list()
		numBonuses = gc.getNumBonusInfos()
		for i in range(numBonuses):
			bonusList.append(i)
		shuffledBonuses = ShuffleList(bonusList)
		#for i in range(numBonuses):
		#	n = PRand.randint(0,len(bonusList) - 1)
		#	shuffledBonuses.append(bonusList[n])
		#	del bonusList[n]
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
		plotList = ShuffleList(plotList)
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
						else:
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
		plotList = ShuffleList(plotList)
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
				sPlot = StartPlot(startPlot.getX(),startPlot.getY(),0)
				if eHandicap == gc.getInfoTypeForString("HANDICAP_SETTLER"):
					if mc.SettlerBonus > 0:
						print "Human player at Settler difficulty, adding %d resources" % mc.SettlerBonus
						self.boostCityPlotValue(startPlot.getX(),startPlot.getY(),mc.SettlerBonus,sPlot.isCoast())
				elif eHandicap == gc.getInfoTypeForString("HANDICAP_CHIEFTAIN"):
					if mc.ChieftainBonus > 0:
						print "Human player at Chieftain difficulty, adding %d resources" % mc.ChieftainBonus
						self.boostCityPlotValue(startPlot.getX(),startPlot.getY(),mc.ChieftainBonus,sPlot.isCoast())
				elif eHandicap == gc.getInfoTypeForString("HANDICAP_WARLORD"):
					if mc.WarlordBonus > 0:
						print "Human player at Warlord difficulty, adding %d resources" % mc.WarlordBonus
						self.boostCityPlotValue(startPlot.getX(),startPlot.getY(),mc.WarlordBonus,sPlot.isCoast())
				elif eHandicap == gc.getInfoTypeForString("HANDICAP_NOBLE"):
					if mc.NobleBonus > 0:
						print "Human player at Noble difficulty, adding %d resources" % mc.NobleBonus
						self.boostCityPlotValue(startPlot.getX(),startPlot.getY(),mc.NobleBonus,sPlot.isCoast())
				elif eHandicap == gc.getInfoTypeForString("HANDICAP_PRINCE"):
					if mc.PrinceBonus > 0:
						print "Human player at Prince difficulty, adding %d resources" % mc.PrinceBonus
						self.boostCityPlotValue(startPlot.getX(),startPlot.getY(),mc.PrinceBonus,sPlot.isCoast())
				elif eHandicap == gc.getInfoTypeForString("HANDICAP_MONARCH"):
					if mc.MonarchBonus > 0:
						print "Human player at Monarch difficulty, adding %d resources" % mc.MonarchBonus
						self.boostCityPlotValue(startPlot.getX(),startPlot.getY(),mc.MonarchBonus,sPlot.isCoast())
				elif eHandicap == gc.getInfoTypeForString("HANDICAP_EMPEROR"):
					if mc.EmperorBonus > 0:
						print "Human player at Emperor difficulty, adding %d resources" % mc.EmperorBonus
						self.boostCityPlotValue(startPlot.getX(),startPlot.getY(),mc.EmperorBonus,sPlot.isCoast())
				elif eHandicap == gc.getInfoTypeForString("HANDICAP_IMMORTAL"):
					if mc.ImmortalBonus > 0:
						print "Human player at Immortal difficulty, adding %d resources" % mc.ImmortalBonus
						self.boostCityPlotValue(startPlot.getX(),startPlot.getY(),mc.ImmortalBonus,sPlot.isCoast())
				elif eHandicap == gc.getInfoTypeForString("HANDICAP_DEITY"):
					if mc.DeityBonus > 0:
						print "Human player at Deity Difficulty, adding %d resources" % mc.DeityBonus
						self.boostCityPlotValue(startPlot.getX(),startPlot.getY(),mc.DeityBonus,sPlot.isCoast())
				elif eHandicap == gc.getInfoTypeForString("HANDICAP_NIGHTMARE"):
					if mc.NightmareBonus > 0:
						print "Human player at Nightmare Difficulty, adding %d resources" % mc.NightmareBonus
						self.boostCityPlotValue(startPlot.getX(),startPlot.getY(),mc.NightmareBonus,sPlot.isCoast())


#Global access
spf = StartingPlotFinder()
class CityPlot :
		def __init__(self,food,value):
				self.food = food
				self.value = value
class Improvement :
		def __init__(self,e,food,production,commerce,value):
				self.e = e
				self.food = food
				self.production = production
				self.commerce = commerce
				self.value = value

class StartingArea :
		def __init__(self,areaID):
				self.areaID = areaID
				self.playerList = list()
				self.plotList = list()
				self.distanceTable = array('i')
				self.rawValue = 0
				self.CalculatePlotList()
				self.idealNumberOfPlayers = 0
				return
		def CalculatePlotList(self):
				gameMap = CyMap()

				for y in range(mc.height):
						for x in range(mc.width):
								plot = gameMap.plot(x,y)
								if plot.getArea() == self.areaID:
										#don't place a city on top of a bonus
										if plot.getBonusType(TeamTypes.NO_TEAM) != BonusTypes.NO_BONUS:
												continue
										food,value = spf.getCityPotentialValue(x,y)
										if value > 0:
												startPlot = StartPlot(x,y,value)
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
##				print "number of initial plots in areaID = %(a)3d is %(p)5d" % {"a":self.areaID,"p":len(self.plotList)}
				numPlots = len(self.plotList)
				for n in range(numPlots):
						#At some point the length of plot list will be much shorter than at
						#the beginning of the loop, so it can never end normally
						if n >= len(self.plotList) - 1:
								break
						y = self.plotList[n].y
						x = self.plotList[n].x
						for yy in range(y - 3,y + 4):
								for xx in range(x - 3,x + 4):
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
##												print "m = %(m)3d, n = %(n)3d" % {"m":m,"n":n}
												if self.plotList[m] != self.plotList[n]:
														if self.plotList[m].x == xx and self.plotList[m].y == yy:
##																print "deleting m = %(m)3d" % {"m":m}
																del self.plotList[m]
##																print "length of plotList now %(len)4d" % {"len":len(self.plotList)}

##				print "number of final plots in areaID = %(a)3d is %(p)5d" % {"a":self.areaID,"p":len(self.plotList)}

				#At this point we should have a list of the very best places
				#to build cities on this continent. Now we need a table with
				#the distance from each city to every other city

				#Create distance table
				for i in range(len(self.plotList)*len(self.plotList)):
						self.distanceTable.append(-11)
				#Fill distance table
				for n in range(len(self.plotList)):
						#While were already looping lets calculate the raw value
						self.rawValue += self.plotList[n].localValue
						avgDistance = 0
						for m in range(n,len(self.plotList)):
								nPlot = gameMap.plot(self.plotList[n].x,self.plotList[n].y)
								mPlot = gameMap.plot(self.plotList[m].x,self.plotList[m].y)
								gameMap.resetPathDistance()
								distance = gameMap.calculatePathDistance(nPlot,mPlot)
#								distance = self.getDistance(nPlot.getX(),nPlot.getY(),mPlot.getX(),mPlot.getY())
								#If path fails try reversing it
##								gameMap.resetPathDistance()
##								newDistance = gameMap.calculatePathDistance(mPlot,nPlot)
##								if distance != newDistance:
##										print "distance between n=%(n)d nx=%(nx)d,ny=%(ny)d and m=%(m)d mx=%(mx)d,my=%(my)d is %(d)d or %(nd)d" % \
##										{"n":n,"nx":nPlot.getX(),"ny":nPlot.getY(),"m":m,"mx":mPlot.getX(),"my":mPlot.getY(),"d":distance,"nd":newDistance}
								self.distanceTable[n*len(self.plotList) + m] = distance
								self.distanceTable[m*len(self.plotList) + n] = distance
								avgDistance += distance
						self.plotList[n].avgDistance = avgDistance

				return
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
				avgDistanceList.sort(lambda x,y:cmp(x.avgDistance,y.avgDistance))
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
##						print "Placing start at x=%(x)d, y=%(y)d nearestDistance to city is %(n)d" % \
##						{"x":distanceList[0].x,"y":distanceList[0].y,"n":distanceList[0].nearestStart}

				self.CalculateStartingPlotValues()

##				self.PrintPlotMap()
##				self.PrintPlotList()
##				self.PrintDistanceTable()

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


				return
		def CalculateStartingPlotValues(self):
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
##										print "n= %(n)3d, m = %(m)3d, p = %(p)3d, nDistance = %(nd)3d, pDistance = %(pd)3d" %\
##										{"n":n,"m":m,"p":p,"nd":nDistance,"pd":pDistance}
										if pDistance > -1 and pDistance <= nDistance:
												leastDistance = False #Proven wrong
												break

								if leastDistance == True:
										self.plotList[n].totalValue += self.plotList[m].localValue
#										 print "m = %(m)3d owner change from %(mo)3d to %(n)3d" % {"m":m,"mo":self.plotList[m].owner,"n":n}
										self.plotList[m].owner = self.plotList[n]
										self.plotList[m].distanceToOwner = nDistance
										self.plotList[n].numberOfOwnedCities += 1

				return
		def getDistance(self,x,y,dx,dy):
				xx = x - dx
				if abs(xx) > mc.width/2:
						xx = mc.width - abs(xx)

				distance = max(abs(xx),abs(y - dy))
				return distance
		def PrintPlotMap(self):
				gameMap = CyMap()
				print "Starting Plot Map"
				for y in range(hm.mapHeight - 1,-1,-1):
						lineString = ""
						for x in range(hm.mapWidth):
								inList = False
								for n in range(len(self.plotList)):
										if self.plotList[n].x == x and self.plotList[n].y == y:
												if self.plotList[n].plot().isWater() == True:
														if self.plotList[n].vacant == True:
																lineString += 'VV'
														else:
																lineString += 'OO'
												else:
														if self.plotList[n].vacant == True:
																lineString += 'vv'
														else:
																lineString += 'oo'
												inList = True
												break
								if inList == False:
										plot = gameMap.plot(x,y)
										if plot.isWater() == True:
												lineString += '.;'
										else:
												lineString += '[]'

						lineString += "-" + str(y)
						print lineString

				lineString = " "
				print lineString

				return
		def PrintPlotList(self):
				for n in range(len(self.plotList)):
						print str(n) + ' ' + str(self.plotList[n])
				return

		def PrintDistanceTable(self):
				print "Distance Table"
				lineString = "%(n)05d" % {"n":0} + ' '
				for n in range(len(self.plotList)):
						lineString += "%(n)05d" % {"n":n} + ' '
				print lineString
				lineString = ""
				for n in range(len(self.plotList)):
						lineString = "%(n)05d" % {"n":n} + ' '
						for m in range(len(self.plotList)):
								i = n * len(self.plotList) + m
								lineString += "%(d)05d" % {"d":self.distanceTable[i]} + ' '
						print lineString
				return

class StartPlot:
	def __init__(self,x,y,localValue):
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
		return

	def isCoast(self):
		waterArea = CyMap().plot(self.x, self.y).waterArea()
		return waterArea is not None and not waterArea.isLake()

	def isRiverSide(self):
		return CyMap().plot(self.x,self.y).isRiverSide()

	def plot(self):
		return CyMap().plot(self.x,self.y)

	def copy(self):
		cp = StartPlot(self.x,self.y,self.localValue)
		cp.totalValue = self.totalValue
		cp.numberOfOwnedCities = self.numberOfOwnedCities
		cp.distanceToOwner = self.distanceToOwner
		cp.nearestStart = self.nearestStart
		cp.vacant = self.vacant
		cp.owner = self.owner
		cp.avgDistance = self.avgDistance
		return cp

	def __str__(self):
		linestring = "x=%(x)3d,y=%(y)3d,localValue=%(lv)6d,totalValue =%(tv)6d, nearestStart=%(ad)6d, coastalCity=%(cc)d" % \
		{"x":self.x,"y":self.y,"lv":self.localValue,"tv":self.totalValue,"ad":self.nearestStart,"cc":self.isCoast()}
		return linestring

hm = HeightMap()
cm = ClimateMap()
sm = SmallMaps()
rm = RiverMap()

###############################################################################
#functions that civ is looking for
###############################################################################
def getDescription():
	"""
	A map's Description is displayed in the main menu when players go to begin a game.
	For no description return an empty string.
	"""
	return "TXT_KEY_MAP_SCRIPT_PERFECTWORLD2F_DESCR"
def getWrapX():
	return mc.WrapX

def getWrapY():
		print "mc.WrapY == %d at getWrapY" % mc.WrapY
		return mc.WrapY

def getNumHiddenCustomMapOptions():
	return 0

def getNumCustomMapOptions():
		"""
		Number of different user-defined options for this map
		Return an integer
		"""
		mc.initialize()
		return 3

def getCustomMapOptionName(argsList):
				"""
				Returns name of specified option
				argsList[0] is Option ID (int)
				Return a Unicode string
				"""
				optionID = argsList[0]
				if optionID == 0:
						return "New World Rules"
				elif optionID == 1:
						return "Pangaea Rules"
				elif optionID == 2:
						return "Wrap Option"

				return u""

def getNumCustomMapOptionValues(argsList):
				"""
				Number of different choices for a particular setting
				argsList[0] is Option ID (int)
				Return an integer
				"""
				optionID = argsList[0]
				if optionID == 0:
						return 2
				elif optionID == 1:
						return 2
				elif optionID == 2:
						return 3
				return 0

def getCustomMapOptionDescAt(argsList):
		"""
		Returns name of value of option at specified row
		argsList[0] is Option ID (int)
		argsList[1] is Selection Value ID (int)
		Return a Unicode string
		"""
		optionID = argsList[0]
		selectionID = argsList[1]
		if optionID == 0:
				if selectionID == 0:
						if mc.AllowNewWorld:
								return "Start in Old World"
						else:
								return "Start Anywhere"
				elif selectionID == 1:
						if mc.AllowNewWorld:
								return "Start Anywhere"
						else:
								return "Start in Old World"
		elif optionID == 1:
				if selectionID == 0:
						if mc.AllowPangeas:
								return "Allow Pangaeas"
						else:
								return "Break Pangaeas"
				elif selectionID == 1:
						if mc.AllowPangeas:
								return "Break Pangaeas"
						else:
								return "Allow Pangaeas"
		elif optionID == 2:
				if selectionID == 0:
						return "Cylindrical"
				elif selectionID == 1:
						return "Toroidal"
				elif selectionID == 2:
						return "Flat"
		return u""

def getCustomMapOptionDefault(argsList):
	"""
	Returns default value of specified option
	argsList[0] is Option ID (int)
	Return an integer
	"""
	#Always zero in this case
	return 0

def isRandomCustomMapOption(argsList):
	"""
	Returns a flag indicating whether a random option should be provided
	argsList[0] is Option ID (int)
	Return a bool
	"""
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
	"""
	Advanced maps only show up in the map script pulldown on the advanced menu.
	Return 0 if you want your map to show up in the simple singleplayer menu
	"""
	return 0
def isClimateMap():
	"""
	Uses the Climate options
	"""
	return 0

def isSeaLevelMap():
	"""
	Uses the Sea Level options
	"""
	return 1

def getTopLatitude():
	"Default is 90. 75 is past the Arctic Circle"
	return 90

def getBottomLatitude():
	"Default is -90. -75 is past the Antartic Circle"
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
		gc = CyGlobalContext()
		mmap = gc.getMap()
		mc.width = mmap.getGridWidth()
		mc.height = mmap.getGridHeight()
		mc.minimumMeteorSize = (1 + int(round(float(mc.hmWidth)/float(mc.width)))) * 3
		PRand.seed()
		hm.performTectonics()
		hm.generateHeightMap()
		hm.combineMaps()
		hm.calculateSeaLevel()
		hm.fillInLakes()
		pb.breakPangaeas()
##		hm.Erode()
##		hm.printHeightMap()
		hm.addWaterBands()
##		hm.printHeightMap()
		cm.createClimateMaps()
		sm.initialize()
		rm.generateRiverMap()
		plotTypes = [PlotTypes.PLOT_OCEAN] * (mc.width*mc.height)

		for i in range(mc.width*mc.height):
				mapLoc = sm.plotMap[i]
				if mapLoc == mc.PEAK:
						plotTypes[i] = PlotTypes.PLOT_PEAK
				elif mapLoc == mc.HILLS:
						plotTypes[i] = PlotTypes.PLOT_HILLS
				elif mapLoc == mc.LAND:
						plotTypes[i] = PlotTypes.PLOT_LAND
				else:
						plotTypes[i] = PlotTypes.PLOT_OCEAN
		print "Finished generating plot types."
		return plotTypes
def generateTerrainTypes():
		NiTextOut("Generating Terrain	 ...")
		print "Adding Terrain"
		gc = CyGlobalContext()
		terrainIce = gc.getInfoTypeForString("TERRAIN_ICE")
		terrainPermafrost = gc.getInfoTypeForString("TERRAIN_PERMAFROST")
		terrainTundra = gc.getInfoTypeForString("TERRAIN_TUNDRA")
		terrainTaiga = gc.getInfoTypeForString("TERRAIN_TAIGA")
		terrainSaltFlats = gc.getInfoTypeForString("TERRAIN_SALT_FLATS")
		terrainDunes = gc.getInfoTypeForString("TERRAIN_DUNES")
		terrainDesert = gc.getInfoTypeForString("TERRAIN_DESERT")
		terrainScrub = gc.getInfoTypeForString("TERRAIN_SCRUB")
		terrainRocky = gc.getInfoTypeForString("TERRAIN_ROCKY")
		terrainRockyArid = gc.getInfoTypeForString("TERRAIN_BADLAND")
		terrainRockyCold = gc.getInfoTypeForString("TERRAIN_JAGGED")
		terrainBarren = gc.getInfoTypeForString("TERRAIN_BARREN")
		terrainPlains = gc.getInfoTypeForString("TERRAIN_PLAINS")
		terrainGrass = gc.getInfoTypeForString("TERRAIN_GRASSLAND")
		terrainLush = gc.getInfoTypeForString("TERRAIN_LUSH")
		terrainMuddy = gc.getInfoTypeForString("TERRAIN_MUDDY")
		terrainMarsh = gc.getInfoTypeForString("TERRAIN_MARSH")
		terrainHill = gc.getInfoTypeForString("TERRAIN_HILL")
		terrainCoast = gc.getInfoTypeForString("TERRAIN_COAST")
		terrainOcean = gc.getInfoTypeForString("TERRAIN_OCEAN")
		terrainPeak = gc.getInfoTypeForString("TERRAIN_PEAK")

		terrainTypes = [0]*(mc.width*mc.height)
		for i in range(mc.width*mc.height):
				if sm.terrainMap[i] == mc.OCEAN:
						terrainTypes[i] = terrainOcean
				elif sm.terrainMap[i] == mc.COAST:
						terrainTypes[i] = terrainCoast
				elif sm.terrainMap[i] == mc.DESERT:
						terrainTypes[i] = terrainDesert
				elif sm.terrainMap[i] == mc.PLAINS:
						terrainTypes[i] = terrainPlains
				elif sm.terrainMap[i] == mc.GRASS:
						terrainTypes[i] = terrainGrass
				elif sm.terrainMap[i] == mc.TUNDRA:
						terrainTypes[i] = terrainTundra
				elif sm.terrainMap[i] == mc.SALT_FLATS:
						terrainTypes[i] = terrainSaltFlats
				elif sm.terrainMap[i] == mc.DUNES:
						terrainTypes[i] = terrainDunes
				elif sm.terrainMap[i] == mc.SCRUB:
						terrainTypes[i] = terrainScrub
				elif sm.terrainMap[i] == mc.ROCKY:
						terrainTypes[i] = terrainRocky
				elif sm.terrainMap[i] == mc.ROCKY_ARID:
						terrainTypes[i] = terrainRockyArid
				elif sm.terrainMap[i] == mc.ROCKY_COLD:
						terrainTypes[i] = terrainRockyCold
				elif sm.terrainMap[i] == mc.BARREN:
						terrainTypes[i] = terrainBarren
				elif sm.terrainMap[i] == mc.PERMAFROST:
						terrainTypes[i] = terrainPermafrost
				elif sm.terrainMap[i] == mc.TAIGA:
						terrainTypes[i] = terrainTaiga
				elif sm.terrainMap[i] == mc.LUSH:
						terrainTypes[i] = terrainLush
				elif sm.terrainMap[i] == mc.MUDDY:
						terrainTypes[i] = terrainMuddy
				elif sm.terrainMap[i] == mc.MARSH:
						terrainTypes[i] = terrainMarsh
				elif sm.terrainMap[i] == mc.ICE:
						terrainTypes[i] = terrainIce
		print "Finished generating terrain types."
		return terrainTypes

def addRivers():
		NiTextOut("Adding Rivers....")
		print "Adding Rivers"
		gc = CyGlobalContext()
		pmap = gc.getMap()
		for y in range(mc.height):
				for x in range(mc.width):
						placeRiversInPlot(x,y)

##		rm.printRiverAndTerrainAlign()

		#peaks and rivers don't always mix well graphically, so lets eliminate
		#these potential glitches. Basically if there are adjacent peaks on both
		#sides of a river, either in a cardinal direction or diagonally, they
		#will look bad.
		for y in range(mc.height):
				for x in range(mc.width):
						plot = pmap.plot(x,y)
						if plot.isPeak() == True:
								if plot.isNOfRiver() == True:
										for xx in range(x - 1,x + 2,1):
												yy = y - 1
												if yy < 0:
														break
												#wrap in x direction
												if xx == -1:
														xx = mc.width - 1
												elif xx == mc.width:
														xx = 0
												newPlot = pmap.plot(xx,yy)
												ii = GetIndex(xx,yy)
												if newPlot.isPeak():
														plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)
														sm.plotMap[ii] = mc.HILLS
														break
						#possibly changed so checked again
						if plot.isPeak() == True:
								if plot.isWOfRiver() == True:
										for yy in range(y - 1,y + 2,1):
												xx = x + 1
												if xx == mc.width:
														xx = 0
												#do not wrap in y direction
												if yy == -1:
														continue
												elif yy == mc.height:
														continue
												newPlot = pmap.plot(xx,yy)
												ii = GetIndex(xx,yy)
												if newPlot.isPeak():
														plot.setPlotType(PlotTypes.PLOT_HILLS,True,True)
														sm.plotMap[ii] = mc.HILLS
														break

def placeRiversInPlot(x,y):
		gc = CyGlobalContext()
		pmap = gc.getMap()
		plot = pmap.plot(x,y)
		#NE
		xx,yy = rm.rxFromPlot(x,y,rm.NE)
		ii = GetIndex(xx,yy)
		if ii != -1:
				if rm.riverMap[ii] == rm.S:
						plot.setWOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
		#SW
		xx,yy = rm.rxFromPlot(x,y,rm.SW)
		ii = GetIndex(xx,yy)
		if ii != -1:
				if rm.riverMap[ii] == rm.E:
						plot.setNOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_EAST)
		#SE
		xx,yy = rm.rxFromPlot(x,y,rm.SE)
		ii = GetIndex(xx,yy)
		if ii != -1:
				if rm.riverMap[ii] == rm.N:
						plot.setWOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
				elif rm.riverMap[ii] == rm.W:
						plot.setNOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_WEST)
'''
This function examines a lake area and removes ugly surrounding rivers. Any
river that is flowing away from the lake, or alongside the lake will be
removed. This function also returns a list of riverID's that flow into the
lake.
'''
def cleanUpLake(x,y):
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
def replaceRivers(x,y):
		gc = CyGlobalContext()
		mmap = gc.getMap()
		plot = mmap.plot(x,y+1)#North
		if plot != 0 and plot.isWOfRiver() == True:
				if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_SOUTH:
						#setting the river to what it already is will be ignored by the dll,
						#so it must be unset and then set again.
						plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
						plot.setWOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
		plot = mmap.plot(x - 1,y)#West
		if plot != 0 and plot.isNOfRiver() == True:
				if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_EAST:
						plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
						plot.setNOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_EAST)
		plot = mmap.plot(x + 1,y)#East
		if plot != 0 and plot.isNOfRiver() == True:
				if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_WEST:
						plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
						plot.setNOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_WEST)
		plot = mmap.plot(x,y-1)#South
		if plot != 0 and plot.isWOfRiver() == True:
				if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_NORTH:
						plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
						plot.setWOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		plot = mmap.plot(x-1,y+1)#Northwest
		if plot != 0 and plot.isWOfRiver() == True:
				if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_SOUTH:
						plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
						plot.setWOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_SOUTH)
		if plot != 0 and plot.isNOfRiver() == True:
				if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_EAST:
						plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
						plot.setNOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_EAST)
		plot = mmap.plot(x+1,y+1)#Northeast
		if plot != 0 and plot.isNOfRiver() == True:
				if plot.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_WEST:
						plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
						plot.setNOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_WEST)
		plot = mmap.plot(x-1,y-1)#Southhwest
		if plot != 0 and plot.isWOfRiver() == True:
				if plot.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_NORTH:
						plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
						plot.setWOfRiver(True,CardinalDirectionTypes.CARDINALDIRECTION_NORTH)
		#Southeast plot is not relevant

		return

'''
It looks bad to have a lake, fed by a river, sitting right next to the coast.
This function tries to minimize that occurance by replacing it with a
natural harbor, which looks much better.
'''
def makeHarbor(x,y,oceanMap):
		oceanID = oceanMap.getOceanID()
		i = oceanMap.getIndex(x,y)
		if oceanMap.areaMap[i] != oceanID:
				return
		#N
		xx = x
		yy = y + 2
		ii = oceanMap.getIndex(xx,yy)
		if ii > -1 and \
		oceanMap.getAreaByID(oceanMap.areaMap[ii]).water == True and \
		oceanMap.areaMap[ii] != oceanID:
				makeChannel(x,y + 1)
				oceanMap.defineAreas(isSmallWaterMatch)
				oceanID = oceanMap.getOceanID()
		#S
		xx = x
		yy = y - 2
		ii = oceanMap.getIndex(xx,yy)
		if ii > -1 and \
		oceanMap.getAreaByID(oceanMap.areaMap[ii]).water == True and \
		oceanMap.areaMap[ii] != oceanID:
				makeChannel(x,y - 1)
				oceanMap.defineAreas(isSmallWaterMatch)
				oceanID = oceanMap.getOceanID()
		#E
		xx = x + 2
		yy = y
		ii = oceanMap.getIndex(xx,yy)
		if ii > -1 and \
		oceanMap.getAreaByID(oceanMap.areaMap[ii]).water == True and \
		oceanMap.areaMap[ii] != oceanID:
				makeChannel(x + 1,y)
				oceanMap.defineAreas(isSmallWaterMatch)
				oceanID = oceanMap.getOceanID()
		#W
		xx = x - 2
		yy = y
		ii = oceanMap.getIndex(xx,yy)
		if ii > -1 and \
		oceanMap.getAreaByID(oceanMap.areaMap[ii]).water == True and \
		oceanMap.areaMap[ii] != oceanID:
				makeChannel(x - 1,y)
				oceanMap.defineAreas(isSmallWaterMatch)
				oceanID = oceanMap.getOceanID()
		#NW
		xx = x - 1
		yy = y + 1
		ii = oceanMap.getIndex(xx,yy)
		if ii > -1 and \
		oceanMap.getAreaByID(oceanMap.areaMap[ii]).water == True and \
		oceanMap.areaMap[ii] != oceanID:
				makeChannel(x - 1,y)
				oceanMap.defineAreas(isSmallWaterMatch)
				oceanID = oceanMap.getOceanID()
		#NE
		xx = x + 1
		yy = y + 1
		ii = oceanMap.getIndex(xx,yy)
		if ii > -1 and \
		oceanMap.getAreaByID(oceanMap.areaMap[ii]).water == True and \
		oceanMap.areaMap[ii] != oceanID:
				makeChannel(x + 1,y)
				oceanMap.defineAreas(isSmallWaterMatch)
				oceanID = oceanMap.getOceanID()
		#SW
		xx = x - 1
		yy = y - 1
		ii = oceanMap.getIndex(xx,yy)
		if ii > -1 and \
		oceanMap.getAreaByID(oceanMap.areaMap[ii]).water == True and \
		oceanMap.areaMap[ii] != oceanID:
				makeChannel(x ,y - 1)
				oceanMap.defineAreas(isSmallWaterMatch)
				oceanID = oceanMap.getOceanID()
		#NW
		xx = x - 1
		yy = y + 1
		ii = oceanMap.getIndex(xx,yy)
		if ii > -1 and \
		oceanMap.getAreaByID(oceanMap.areaMap[ii]).water == True and \
		oceanMap.areaMap[ii] != oceanID:
				makeChannel(x,y + 1)
				oceanMap.defineAreas(isSmallWaterMatch)
				oceanID = oceanMap.getOceanID()
		return
def makeChannel(x,y):
		gc = CyGlobalContext()
		mmap = gc.getMap()
		terrainCoast = gc.getInfoTypeForString("TERRAIN_COAST")
		plot = mmap.plot(x,y)
		cleanUpLake(x,y)
		plot.setTerrainType(terrainCoast,True,True)
		plot.setRiverID(-1)
		plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
		plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
		replaceRivers(x,y)
		i = GetIndex(x,y)
		sm.plotMap[i] = mc.OCEAN
		return
def expandLake(x,y,riversIntoLake,oceanMap):
		class LakePlot :
				def __init__(self,x,y,altitude):
						self.x = x
						self.y = y
						self.altitude = altitude
		gc = CyGlobalContext()
		mmap = gc.getMap()
		terrainCoast = gc.getInfoTypeForString("TERRAIN_COAST")
		lakePlots = list()
		lakeNeighbors = list()
		i = oceanMap.getIndex(x,y)
		desertModifier = 1.0
		if sm.terrainMap[i] == mc.DESERT:
				desertModifier = mc.DesertLakeModifier
		drainage = rm.drainageMap[i]
		lakeSize = max(3,int(drainage * mc.LakeSizePerDrainage * desertModifier ))
		start = LakePlot(x,y,sm.heightMap[i])
		lakeNeighbors.append(start)
#		 print "lakeSize",lakeSize
		while lakeSize > 0 and len(lakeNeighbors) > 0:
#				 lakeNeighbors.sort(key=operator.attrgetter('altitude'),reverse=False)
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
								areaID = oceanMap.areaMap[ii]
								if areaID == 0:
										raise ValueError, "areaID = 0 while generating lakes. This is a bug"
								for n in range(len(oceanMap.areaList)):
										if oceanMap.areaList[n].ID == areaID:
												if oceanMap.areaList[n].water == True:
#														 print "lake touched waterID = %(id)3d with %(ls)3d squares unused" % {'id':areaID,'ls':lakeSize}
#														 print "n = %(n)3d" % {"n":n}
#														 print str(oceanMap.areaList[n])
														return
								if rm.riverMap[ii] != rm.L and mmap.plot(xx,yy).isWater() == False:
										lakeNeighbors.append(LakePlot(xx,yy,sm.heightMap[ii]))

				lakeSize -= 1
#		 print "lake finished normally at %(x)2d,%(y)2d" % {"x":x,"y":y}
		return

def addLakes():
		print "Adding Lakes"
		gc = CyGlobalContext()
		mmap = gc.getMap()
#		 PrintFlowMap()
		oceanMap = Areamap(mc.width,mc.height,True,True)
		oceanMap.defineAreas(isSmallWaterMatch)
##		oceanMap.PrintList(oceanMap.areaList)
##		oceanMap.PrintAreaMap()
		for y in range(mc.height):
				for x in range(mc.width):
						i = GetIndex(x,y)
						if rm.flowMap[i] == rm.L:
								riversIntoLake = cleanUpLake(x,y)
								plot = mmap.plot(x,y)
								if len(riversIntoLake) > 0:
##										plot.setRiverID(-1)
##										plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
##										plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
##										#plot.setPlotType(PlotTypes.PLOT_OCEAN,True,True) setTerrain handles this already
##										plot.setTerrainType(terrainCoast,True,True)
										expandLake(x,y,riversIntoLake,oceanMap)
								else:
										#no lake here, but in that case there should be no rivers either
										plot.setRiverID(-1)
										plot.setNOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
										plot.setWOfRiver(False,CardinalDirectionTypes.NO_CARDINALDIRECTION)
		oceanMap.defineAreas(isSmallWaterMatch)
##		oceanMap.PrintList(oceanMap.areaList)
##		oceanMap.PrintAreaMap()
		for y in range(mc.height):
				for x in range(mc.width):
						i = GetIndex(x,y)
						makeHarbor(x,y,oceanMap)
		return

def addFeatures():
		NiTextOut("Generating Features	...")
		print "Adding Features"
		gc = CyGlobalContext()
		mmap = gc.getMap()
		featureJungle = gc.getInfoTypeForString("FEATURE_JUNGLE")
		featureForest = gc.getInfoTypeForString("FEATURE_FOREST")
		featureOasis = gc.getInfoTypeForString("FEATURE_OASIS")
		featureFloodPlains = gc.getInfoTypeForString("FEATURE_FLOOD_PLAINS")
		## C2C start ##
		featureBog = gc.getInfoTypeForString("FEATURE_PEAT_BOG")
		featureSwordGrass = gc.getInfoTypeForString("FEATURE_VERY_TALL_GRASS")
		featureSwamp = gc.getInfoTypeForString("FEATURE_SWAMP")
		## C2C end ##
		FORESTLEAFY = 0
		FORESTEVERGREEN = 1
		FORESTSNOWY = 2

		createIce()
		#Now plant forest or jungle
##		PrintTempMap(tm,tm.tempMap)
##		PrintRainMap(rm,rm.rainMap,False)
		for y in range(mc.height):
				for x in range(mc.width):
						i = GetIndex(x,y)
						plot = mmap.plot(x,y)

						#floodplains and Oasis
						if (sm.terrainMap[i] == mc.DESERT or sm.terrainMap[i] == mc.DUNES or sm.terrainMap[i] == mc.SCRUB) and sm.plotMap[i] != mc.HILLS and\
						sm.plotMap[i] != mc.PEAK and plot.isWater() == False:
								if plot.isRiver() == True:
										plot.setFeatureType(featureFloodPlains,0)
								else:
										#is this square surrounded by desert?
										foundNonDesert = False
										#print "trying to place oasis"
										for yy in range(y - 1,y + 2):
												for xx in range(x - 1,x + 2):
														ii = GetIndex(xx,yy)
														surPlot = mmap.plot(xx,yy)
														if (sm.terrainMap[ii] != mc.DESERT and sm.terrainMap[ii] != mc.SCRUB and sm.terrainMap[ii] != mc.DUNES and \
														sm.plotMap[ii] != mc.PEAK):
																#print "non desert neighbor"
																foundNonDesert = True
														elif surPlot == 0:
																#print "neighbor off map"
																foundNonDesert = True
														elif surPlot.isWater() == True:
																#print "water neighbor"
																foundNonDesert = True
														elif surPlot.getFeatureType() == featureOasis:
																#print "oasis neighbor"
																foundNonDesert = True
										if foundNonDesert == False:
												if PRand.random() < mc.OasisChance:
														#print "placing oasis"
														plot.setFeatureType(featureOasis,0)
											 # else:
														#print "oasis failed random check"

 ## C2C start ##
						#marsh features
						if sm.terrainMap[i] == mc.MARSH and plot.isPeak() == False:
								#plot.setFeatureType(FeatureTypes.NO_FEATURE,-1)
								if sm.averageTempMap[i] < 0.45:
										if PRand.random() < 0.75:
												plot.setFeatureType(featureBog,0)
								elif sm.averageTempMap[i] < 0.6:
										if PRand.random() < 0.25:
												plot.setFeatureType(featureBog,0)
										elif PRand.random() < 0.5:
												plot.setFeatureType(featureSwordGrass,0)
								elif sm.averageTempMap[i] < 0.8:
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
						if plot.isWater() == False and plot.getFeatureType() == FeatureTypes.NO_FEATURE and sm.terrainMap[i] != mc.DESERT and sm.terrainMap[i] != mc.DUNES and sm.terrainMap[i] != mc.SALT_FLATS and sm.terrainMap[i] != mc.SCRUB and\
						plot.isPeak() == False:
								if sm.rainFallMap[i] > sm.plainsThreshold * mc.TreeFactor and\
								PRand.random() < mc.MaxTreeChance:#jungle
										if sm.averageTempMap[i] > mc.JungleTemp:
												if sm.terrainMap[i] == mc.PLAINS:
														plot.setFeatureType(featureForest,FORESTLEAFY)
												else:
														plot.setFeatureType(featureJungle,0)
										elif sm.averageTempMap[i] > mc.ForestTemp:
												plot.setFeatureType(featureForest,FORESTLEAFY)
										elif sm.averageTempMap[i] > mc.TaigaTemp:
												plot.setFeatureType(featureForest,FORESTEVERGREEN)
										elif sm.averageTempMap[i] > mc.TundraTemp:
												plot.setFeatureType(featureForest,FORESTSNOWY)
								elif sm.rainFallMap[i] > sm.desertThreshold:#forest
										if sm.rainFallMap[i] > PRand.random() * sm.plainsThreshold * mc.TreeFactor / mc.MaxTreeChance:
												if sm.averageTempMap[i] > mc.TaigaTemp:
													 plot.setFeatureType(featureForest,FORESTLEAFY)
												elif sm.averageTempMap[i] > mc.TundraTemp:
														plot.setFeatureType(featureForest,FORESTEVERGREEN)
												elif sm.averageTempMap[i] > mc.PermafrostTemp:
														plot.setFeatureType(featureForest,FORESTSNOWY)
		return

def createIce():
		gc = CyGlobalContext()
		mmap = gc.getMap()
		featureIce = gc.getInfoTypeForString("FEATURE_ICE")
		if mc.WrapY == True:
				iceChance = 0.5
		else:
				iceChance = 1.0
		for y in range(4):
				for x in range(mc.width):
						plot = mmap.plot(x,y)
						if plot != 0 and plot.isWater() == True and PRand.random() < iceChance:
								plot.setFeatureType(featureIce,0)
				iceChance *= .66
		if mc.WrapY == True:
				iceChance = 0.5
		else:
				iceChance = 1.0
		for y in range(mc.height - 1,mc.height - 5,-1):
				for x in range(mc.width):
						plot = mmap.plot(x,y)
						if plot != 0 and plot.isWater() == True and PRand.random() < iceChance:
								plot.setFeatureType(featureIce,0)
				iceChance *= .66

def addBonuses():
	bp.AddBonuses()

def assignStartingPlots():
	spf.SetStartingPlots()

def beforeInit():
	print "Initializing Custom Map Options"
	mc.initialize()
	mc.initInGameOptions()

def afterGeneration():
	import CvMapGeneratorUtil
	CvMapGeneratorUtil.placeC2CBonuses()

##mc.initialize()
##PRand.seed()
##hm.performTectonics()
##hm.generateHeightMap()
##hm.combineMaps()
##hm.calculateSeaLevel()
####hm.printHeightMap()
##hm.fillInLakes()
##pb.breakPangaeas()
####hm.printHeightMap()
####hm.Erode()
##hm.printHeightMap()
##hm.addWaterBands()
##cm.createClimateMaps()
##cm.printTempMap(cm.summerTempsMap)
##cm.printTempMap(cm.winterTempsMap)
##cm.printTempMap(cm.averageTempMap)
##cm.printRainFallMap(False)
##cm.printRainFallMap(True)
##sm.initialize()
##rm.generateRiverMap()
####sm.printHeightMap()
####rm.printRiverMap()
####sm.printPlotMap()
##sm.printTerrainMap()
##rm.printFlowMap()
##rm.printRiverMap()
##rm.printRiverAndTerrainAlign()

##sm.printHeightMap()
