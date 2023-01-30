#
#	FILE:	 Planet_Generator.py
#	AUTHORS:
#	Script code: Nerijus Arlauskas (Nercury)
#	Mac support: AlanH
#
#	Version: 0.68
#-----------------------------------------------------------------------------
#	Copyright (c) 2006 Nercury
#-----------------------------------------------------------------------------

"""
Change log:

v. 0.68
- Coordinate system changed to circular for continent growth calculation (faster, not so blocky shapes)
- Continents should no longer break in several big parts (except with "archipelago continents")
- Bug fix: the lonely islands are all devoid of hills or mountains

v. 0.67
- Further improvements for mod map size support: now can understand completely customized sizes

v. 0.66
- Mod map size support

v. 0.65
- Many feature generation tweaks
- More forests or jungle (jungle depends on temperature)
- More flood plains on deserts
- More oases, better oasis placement
- Flood plain "Standard" generation options reduced to one
- Desert maps should be more interesting now
- Forests and jungle can start growing not only near water, but also near peaks which do not have many features around

v. 0.64
- Random float number bug fix

v. 0.63
- Bug fix: generation failure with high continent scattering and distribution around center

v. 0.62
- Resource normalization fix (it was disabled all the time!)

v. 0.61
- 4,5 island option bug fix

v. 0.60
- Continent scattering improved, all options thoroughly tested to produce good-looking maps.
- TileBuilder code improved, to avoid continent jams.
- Generation bug fixed. Hopefully, it will no longer stops at start (!).
- Random (Standard:Rare/Common) flood option fixed.
- New option: Map size. It is now possible to specify custom map size. Because of this change, following options are modified:
- Continent separation options expanded up to 10 tiles.
- Distance from poles no longer depends on continent separation, and is always constant (3).
- From now on, area for continent separation is excluded from ocean area when calculating ocean percentage. It means that selecting higher continent separation now will produce less land. This helps against continent jams.
- Lonely island selection options improved, now it is possible to specify exact amount of islands (1-5), but they will be placed only if there is enough empty area in ocean.
- Default lonely island option changed to 3 islands.
- Default option of removing one-tile islands changed to "yes".
- Humidity options tweaked, now "normal" is a little bit more wet.
- Land near pole ice default value is now "sometimes" (i think that looks better)
- Fixed typo in options

v. 0.53
- Desert tile changing into plains (if jungle nearby) chance reduced from 100% to 60%.
- Rewritten continent scattering algorithm. Changed name from "Continent Islands" to "Continent Scattering", because, well, it's what it does.
- Island separation now is the same as continent separation.

v. 0.51
- Continent islands option fix.

v. 0.50
- New options added: Land near pole ice, Continent islands, Lonely islands, Lonely island size, One tile islands allowed.

v. 0.47
- Smoother transition from jungle to desert/tundra.

v. 0.46
- Both Mac and Windows support in one file. Thanks AlanH for writing code for mac!

v. 0.45
- Oasis.

v. 0.44
- Fixed continent distribution bug for high continent count and huge map.
- Added more ways how continents can be distributed: you can choose old rule with square on equator, two or three rows, circle. Even more, i added option Smart Selection, which will choose the best distribution method for selected continent count and map size.
- Flood plains now have default "Standard" distribution, which makes them appear only on desert, some on tundra and only near fresh water.
- Redefined what "natural" continent means. Now it is much more concentrated landmass with possible peninsulas or inland seas, not snake starting in the middle and ending near poles :)
- New landmass generation templates added - Natural, Crab-like, Smooth sides, old natural renamed to Traveling Natural (because it travels a lot from starting square).
- New "Smart Random for each continent" shape selection which almost ensures that you will have playable and unpredictable maps with different continent and map size selections
- Added a lot of comments to script code
Things to try out:
- New Crab-like continent shape for Pangaea map

v. 0.43
- Tested with Warlords: works
- [B]Emergency fix[/B]: Grid size function was rewritten incorrectly, making map height higher than width ;)

v. 0.42
- Last user choices are now remembered
- Tested with Total Realism 2.0, Fall From Heavens 2.014, SevoMod 3.1
- Modified hill intersection function (produces more hills close together)
- Grid size function is now written correctly (compatible with "very huge" map size mode (if there is such in some mod))

v. 0.41
- Some adjustments for humidity
- Fixed some mistakes in options
"""

from CvPythonExtensions import *
from CvUtil import *
import CvMapGeneratorUtil
import math
import sys
import gc
try:
	import cPickle as pickle
except:
	import pickle


VERSION = "0_68"

#------------------------------------------------------
"""Dropdown options"""
#------------------------------------------------------
# (title), (script name), (use auto random generation)
selection_titles = [
					["Map size override:","map_size",False], #do not use auto random generation from array
					["Continents:","continents",True],
					["Continent separation:","continent_separation",True],
					["Continent distribution:","continent_distribution",True],
					["Continent shape:","continent_shape",True],
					["Continent scattering:","continent_scattering",False], #do not use auto random generation from array
					["Relative continent size:","continent_size",True],
					["Percentage of ocean:","ocean",True],
					["Land near pole ice:","land_near_ice",True],
					["Lonely islands:","lonely_islands",True],
					["Lonely island size:","lonely_island_size",True],
					["One tile islands allowed:","allow_one_tile_islands",True],
					["Mountains (peaks):","peaks",False], #do not use auto random generation from array
					["Hills:","hills",False], #do not use auto random generation from array
					["Climate variation:","variation",True],
					["Humidity:","humidity",True],
					["Temperature:","temperature",True],
					["Flood plains:","flood_plains",True],
					["Resources:","bonus_generation",True],
					]
# default choices, or first-time choices (later only last-time values should be remembered)
selection_defaults = [
					0, #Map size override
					2, #Continents
					4, #Continent separation
					0, #Continent distribution
					2, #Continent shape
					2, #Continent scattering
					1, #Relative continent size
					8, #Percentage of ocean
					0, #Land near pole ice
					7, #Lonely islands
					1, #Lonely island size
					1, #One tile islands allowed
					7, #Mountains (peaks)
					6, #Hills
					3, #Climate variation
					3, #Humidity
					6, #Temperature
					1, #Flood plains
					0, #Resources
					]
# (selection_description), (value or random value list), (is included in "random" selection)
selection_names_and_values = [
					[ #Map size override:
					["No override (use selected map size)",[0,0],False], #0,0 == bypass
					["48 x 32",[48,32],False],
					["72 x 48",[72,48],False],
					["84 x 56",[84,56],False],
					["96 x 64",[96,64],False],
					["108 x 72",[108,72],False],
					["120 x 80",[120,80],False],
					["132 x 88",[132,88],False],
					["144 x 96",[144,96],False],
					],
					[ #Continents:
					["First Time (FT) Random","random",False],
					["FT Random (1 - 3 continents)",[1,2,3],False],
					["FT Random (1 - 5 continents)",[1,2,3,4,5],False],
					["FT Random (1 - 7 continents)",[1,2,3,4,5,6,7],False],
					["FT Random (2 - 5 continents, default)",[2,3,4,5],False],
					["FT Random (2 - 7 continents)",[2,3,4,5,6,7],False],
					["FT Random (2 - 13 continents)",[2,3,4,5,6,7,8,9,10,11,12,13],False],
					["FT Random (2 - 15 continents)",[2,3,4,5,6,7,8,9,10,11,12,13,14,15],False],
					["FT Random (3 - 7 continents)",[3,4,5,6,7],False],
					["FT Random (3 - 13 continents)",[3,4,5,6,7,8,9,10,11,12,13],False],
					["FT Random (3 - 15 continents)",[3,4,5,6,7,8,9,10,11,12,13,14,15],False],
					["FT Random (3 - 18 continents)",[3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18],False],
					["One continent",1,True], # True means this will be added to "random" selection
					["2 continents",2,True],
					["3 continents",3,True],
					["4 continents",4,True],
					["5 continents",5,True],
					["6 continents",6,True],
					["7 continents",7,True],
					["8 continents",8,True],
					["9 continents",9,True],
					["10 continents",10,True],
					["11 continents",11,True],
					["12 continents",12,True],
					["13 continents",13,True],
					["14 continents",14,False],
					["15 continents",15,False],
					["16 continents",16,False],
					["17 continents",17,False],
					["18 continents",18,False],
					],
					[ #Continent separation:
					["FT Random (include all)","random",False],
					["FT Random (2 - 4)",[2,3,4],False],
					["Min 1 water tile (for small maps)",1,True],
					["Min 2 water tiles",2,True],
					["Min 3 water tiles (recommended)",3,True],
					["Min 4 water tiles (late civ contact)",4,True],
					["Min 5 water tiles (for very huge maps)",5,True], #actually, forcing this to small map will make it huge
					["Min 6 water tiles",6,True],
					["Min 7 water tiles",7,True],
					["Min 8 water tiles",8,True],
					["Min 9 water tiles",9,True],
					["Min 10 water tiles",10,True],
					],
					[ #Continent distribution:
					["Smart Selection (default)","smart-random",False],
					["Must have square on equator","equator",True],
					["Around center","center",True],
					["Two Rows","2rows",True],
					["Three Rows","3rows",True],
					],
					[ #Continent shape:
					["Random","random",False],
					["Smart Random for each continent","continent-random",True], #makes each continent to use different grow method
					["Unpredictable (default)","unpredictable",True],
					["Snaky - very thin","snaky-thin",False], #this generates some very narrow passages
					["Snaky","snaky",True],
					["Short, wide snaky","snaky-natural",True], #tries to keep more round shape (compared to snaky)
					["Drifting landmass","traveling-natural",True], #tries to keep more round shape (compared to snaky)
					["Landmass","natural",True],
					["Landmass (rough edges)","crab",True],
					["Round","smooth",False],
					],
					[ #Continent scattering:
					["Random (without archipelago)","random",False],
					["Solid, no islands around",		[0,	0,	0,	0],True],# 0,0,0,0 == bypass
					["Rare small islands (default)",	[1,	3,	0.5,1],True], #Min, Max, ChangeChance, MaxDif
					["Common islands",					[3,	6,	0.4,2],True],
					["Rare islands - common islands",	[1,	5,	0.3,2],True],
					["Rare islands - many islands",		[1,	8,	0.3,3],True],
					["Common islands - many islands",	[3,	10,	0.4,2],True],
					["Coast varies alot",				[1,	12,	0.3,5],True],
					["Archipelago continents",			[8,	15,	0.5,2],False],
					],
					[ #Continent size:
					["Random selection","random",False],
					["Equal size (default)","equal",True],
					["By player count","players",True],
					["May vary","unpredictable",True],
					],
					[ #Percentage of ocean:
					["Random (include all)","random",False],
					["Random (60% - 70%)",[60,62,64,66,68,70],False],
					["Random (50% - 60%)",[50,55,60],False],
					["Random (40% - 70%)",[40,45,50,55,60,65,70],False],
					["Minimize ocean size",0,True],
					["More than 30%",30,True], #sometimes it may not be possible to generate very small ocean size; if algorytm meets dead end or some continent runs out of space, generation stops
					["More than 40%",40,True],
					["More than 50%",50,True],
					["More than 60% (normal)",60,True],
					["More than 70%",70,True],
					["More than 80%",80,True],
					["More than 90%",90,True],
					],
					[ #Land near ice:
					["Sometimes (default)",True,False],
					["Never",False,False],
					],
					[ #Lonely islands:
					["Random","random",False],
					["Use every empty area in ocean",1.0,False],
					["75% of empty area",0.75,True],
					["50% of empty area",0.5,True],
					["25% of empty area",0.25,True],
					["One island somewhere (if possible)","lonely1",True],
					["2 islands (if possible)","lonely2",True],
					["3 islands (if possible, default)","lonely3",True],
					["4 islands (if possible)","lonely4",True],
					["5 islands (if possible)","lonely5",True],
					["No islands",0.0,True],
					],
					[ #Lonely island size:
					["Small","small",True],
					["Medium (default)","medium",True],
					["Big","big",True],
					],
					[ #Allow 1-tile islands:
					["Random","random",False],
					["No (default)",False,True],
					["Yes",True,True],
					],
					[ #Mountains (peaks):
					["Random (without extreme)","random",False],
					["None",[0,0,0],True], # [peakStartProbability,peakGrowProbability,maxPeaksInRow]
					["Few peaks, Groups up to 3",[0.04,0.5,3],True],
					["Few peaks, Groups up to 5",[0.03,0.6,5],True],
					["Few peaks, Groups up to 10",[0.03,0.7,10],True],
					["Some peaks, No groups",[0.08,0,0],True],
					["Some peaks, Groups up to 2",[0.07,0.5,2],True],
					["Some peaks, Groups up to 5 (default)",[0.067,0.6,5],True],
					["Some peaks, Groups up to 7",[0.064,0.7,7],True],
					["Some peaks, Groups up to 11",[0.061,0.8,11],True],
					["Many peaks, No groups",[0.24,0,0],True],
					["Many peaks, Groups up to 3",[0.2,0.5,3],True],
					["Many peaks, Groups up to 5",[0.19,0.7,5],True],
					["Many peaks, Groups up to 10",[0.175,0.8,10],True],
					["Many peaks, Groups up to 14",[0.16,0.85,14],True],
					["Groups up to 100, for fun or pain",[0.1,1,100],False],
					["Extreme peaks, for fun or pain",[0.4,0.95,35],False],
					],
					[ #Hills:
					["Random","random",False],
					["None",[0,0,0],True], # [hillStartProbability,hillGrowProbability,maxHillsInRow]
					["Few hills, No groups",[0.1,0,0],True],
					["Few hills, Groups up to 2",[0.1,0.5,2],True],
					["Few hills, Groups up to 5",[0.1,0.55,5],True],
					["Normal hills, No groups",[0.21,0,0],True],
					["Normal hills, Groups up to 2 (default)",[0.18,0.5,2],True],
					["Normal hills, Groups up to 5",[0.18,0.55,5],True],
					["Many hills, No groups",[0.4,0,0],True],
					["Many hills, Groups up to 2",[0.35,0.5,2],True],
					["Many hills, Groups up to 5",[0.3,0.55,5],True],
					["Many hills, Groups up to 9",[0.25,0.7,9],True],
					],
					[ #Climate variation:
					["FT Random","random",False],
					["Very soft",0.8,True],
					["Soft",0.6,True],
					["Normal",0.4,True],
					["Hard",0.3,True],
					["Drastic",0.2,True],
					],
					[ #Humidity:
					["Random","random",False],
					["Very small (least plants)",0.025,True],
					["Small",0.2,True],
					["Normal",0.5,True],
					["Wet",0.61,True],
					["Rainy",0.78,True],
					["Very rainy (many plants)",0.95,True],
					],
					[ #Temperature:
					["FT Random (include all)","random",False],
					["FT Random (normal)",[15,20,25,30,35,40,45,50,55,60],False],
					["Extremely cold",-9,True],
					["Very cold (tundra in the middle)",10,True],
					["Cold (forests in the middle)",20,True],
					["Cool (light jungle, no deserts)",30,True],
					["Normal",40,True],
					["Warm (more deserts or jungle)",55,True],
					["Hot",70,True],
					["Extremely hot",85,True],
					],
					[ #Flood plains: bellow zero - standart distribution method, ranging from -1 = no floods to 0.001 flood hell, above zero - floods can spread, 0 = lowest spread level
					["Random (include all)","random",False],
					["Standard placement (default)",-0.01,True],
					["Any terrain - Rare, near fresh water",0,True],
					["Any terrain - Common, also near oceans",1,False],
					["Any terrain - Many (spreads more)",2,False],
					["Any terrain - Extreme",3,False],
					],
					[ #Resources (bonus)
					["Standard generator","standard",False],
					#["Planet generator - Rare",0.5,False],
					#["Planet generator - Normal",1.0,False],
					#["Planet generator - Full",1.5,False],
					#["Planet generator - Many",3.0,False],
					],
					]

continentCount = 1 #initial value
continentSeparation = 3 #initial value
climateVariation = 0.4
climateHumidity = 0.5
climateTemperature = 40
allowLandNearIce = False

world = None

landTypes = [PlotTypes.PLOT_LAND,PlotTypes.PLOT_HILLS,PlotTypes.PLOT_PEAK]

generationStarted = False

def getSelValue(name):
	"""Returns None if not found"""
	cgc = CyGlobalContext()
	cymap = CyMap()
	dice = cgc.getGame().getMapRand()
	optionIndex = 0
	found = False
	for optionIndex in range(len(selection_titles)):
		title = selection_titles[optionIndex][1]
		if title == name:
			found = True
			break
	if found:
		#print "---------- chosen option:",optionIndex
		index = int(cymap.getCustomMapOption(optionIndex))
		#print "---------- selected value index:",index
		if index < len(selection_names_and_values[optionIndex]):
			#print "---------- selected value name:",selection_names_and_values[optionIndex][index][0]
			val = selection_names_and_values[optionIndex][index][1]
			#print "---------- selected actual value:",selection_names_and_values[optionIndex][index][1]
			#print "---------- val type",type(val)
			if val == "random":
				colInd = []
				for i in range(len(selection_names_and_values[optionIndex])):
					if selection_names_and_values[optionIndex][i][2]:
						colInd.append(i)
				#print colInd
				drop = dice.get(len(colInd),"Python - EarthGen: Random coice for menu selection")
				#print "drop:",colInd[drop]
				val = selection_names_and_values[optionIndex][colInd[drop]][1]
				#print "---------- selected random value:",val
			if selection_titles[optionIndex][2] and isinstance(val,list): #if use auto random seletion and value is array
				drop = dice.get(len(val),"Python - EarthGen: Random choice for menu selection")
				#print "Random menu option, drop:",drop
				return val[drop]
			else:
				return val
		else:
			val = selection_names_and_values[optionIndex][selection_defaults[optionIndex]][1]
	else:
		return None
#------------------------------------------------------
"""MAP SETUP FUNCTIONS"""
#------------------------------------------------------

def getDescription():
	"""
	A map's	Description	is displayed in	the	main menu when players go to begin a game.
	For	no description return an empty string.
	"""
	return unicode("TXT_KEY_MAP_SCRIPT_PLANETGENERATOR068_DESCR")

worldTypes = []
chosenWorldType = -1

def getGridSize(argsList):
	"""Given an	argument of	[worldSize], where worldSize is	of type	WorldSizeTypes.
	Can	be overridden to return	a (width, height) tuple	representing the number	of terrain cells
	the	world should have. (a terrain cell is a	chunk of 4x4 plots)"""
	#[worldSize] = argsList
	global generationStarted

	saveMapOptionDefaults()

	generationStarted = True
	if (argsList[0] == -1): # (-1,) is passed to function on loads ; thanks smart map ;)
		return (-1,-1)
	global continentCount
	global continentSeparation
	continentCount = getSelValue("continents")
	if continentCount is None:
		continentCount = 1
	continentSeparation = getSelValue("continent_separation")
	if continentSeparation is None:
		continentSeparation = 3

	cgc = CyGlobalContext()

	global worldTypes
	global chosenWorldType

	worldTypes = []
	chosenWorldType = -1
	for i in range(cgc.getNumWorldInfos()):
		worldInfo = cgc.getWorldInfo(i)
		worldTypes.append(worldInfo)

	map = cgc.getMap()
	worldSize = map.getWorldSize()

	ovW, ovH = getSelValue("map_size")

	if ovW > 0:
		iW = ovW
		iH = ovH
	else:
		rasta = False
		for index, worldType in enumerate(worldTypes):
			#print "World Type", worldType.getType()
			if cgc.getInfoTypeForString(worldType.getType()) == worldSize:
				iW = worldType.getGridWidth() * 4
				iH = worldType.getGridHeight() * 4
				rasta = True
				chosenWorldType = index
				break
		if not rasta:
			iW = 40
			iH = 24

	fW = iW / 4
	fH = iH / 4

	distribution = getContinentDistribution()
	minWidthDivision = 1
	if distribution == "2rows":
		minWidthDivision = 2
	elif distribution == "3rows":
		minWidthDivision = 3

	#check to see if there is enough place for continents to start growing
	#if not, increase grid width
	if distribution == "center":
		perimeter = (continentSeparation + 5) * continentCount
		radius = perimeter / (2 * 3.141592)
		width_multiplier = 1.5
		required_width = width_multiplier * radius * 4
		if required_width > fW * 4:
			fnW = required_width / 4
			fH = int(fH * fnW / float(fW) * 4) / 4
			fW = fnW
	else:
		if ((continentSeparation * 2) + 1) * continentCount / minWidthDivision > fW * 4:
			fnW = (((continentSeparation * 2) + 1) * continentCount / minWidthDivision + continentCount) / 4
			fH = int(fH * fnW / float(fW) * 4) / 4
			fW = fnW
	return (fW, fH)
	# return (10, 6) # tiny
	#return (32, 20) # huge

# array wrapper with binary search optimization
# usage:
# - creating array:
# a = bArray()
# - adding data:
# a[2] = "data1"
# b[5] = "data2"
# - retrieving data:
# if a[2] != None:
#	print a[2]
# - length:
# print len(a)
# - removing item:
# a.pop(itemIndex)
# - removing item by key
# a.remove(5)
# - taking random item from array and removing it at the same time:
# print a.random(dice)
# - taking random item from array and not removing it:
# print a.randomItem(dice)
# - reading array data:
# for data in a:
#	print data
class bArray:
	def __init__(self):
		self._data = []

	def __setitem__(self, power, newValue):
		index, value = self._search(power,0,len(self._data)-1)
		if value == None:
			self._data.insert(index, [power, newValue])
		else:
			self._data[index][1] = newValue

	def __getitem__(self, power):
		index, value = self._search(power,0,len(self._data)-1)
		return value

	def __len__(self):
		return len(self._data)

	def pop(self, index = None):
		if index == None:
			return self._data.pop()
		else:
			return self._data.pop(index)

	def remove(self, power):
		index, value = self._search(power,0,len(self._data)-1)
		if value != None:
			self._data.pop(index)
			return True
		else:
			return False

	def random(self, dice):
		if len(self._data) == 0:
			return None
		else:
			item = dice.get(len(self._data),"Python - EarthGen: Random bArray item with removing.")
			value = self._data[item][1]
			self._data.pop(item)
			return value

	def randomItem(self, dice):
		if len(self._data) == 0:
			return None
		else:
			item = dice.get(len(self._data),"Python - EarthGen: Random bArray item without removing.")
			value = self._data[item][1]
			return value

	def _search(self,value,fromI,toI):
		left = fromI
		right = toI
		while left <= right:
			mid = (right-left) / 2 + left
			if value > self._data[mid][0]:
				left = mid + 1
			elif value < self._data[mid][0]:
				right = mid-1
			else:
				return mid, self._data[mid][1]
		return left, None

	def __iter__(self):
		self.index = -1
		return self

	def next(self):
		if self.index == len(self._data) - 1:
			raise StopIteration
		self.index += 1
		return self._data[self.index][1]

class probabilityArray:
	def __init__(self):
		self._data = []
		self._sum = 0.0
		self._maxpower = bArray()
		self._maxpowerindex = -1

	def __setitem__(self, power, newValue):
		if power > 0:
			self._data.append([power, newValue])
			self._sum += float(power)
			if power > self._maxpower:
				self._maxpower = power
				self._maxpowerindex = len(self._data) - 1

	def __len__(self):
		return len(self._data)

	def random(self, dice):
		if len(self._data) > 0:
			item = getRand(dice) * self._sum
			counted = 0
			foundindex = -1
			for index in range(len(self._data)):
				counted += self._data[index][0]
				if counted >= item:
					foundindex = index
					break
			if foundindex >= 0:
				power, value = self._data[foundindex]
				self._data.pop(foundindex)
				self._sum -= power
				return value
			else:
				if self._maxpowerindex != -1:
					power, value = self._data[self._maxpowerindex]
					self._data.pop(self._maxpowerindex)
					self._sum -= power
					self._maxpower = 0.0
					for index in range(len(self._data)):
						newpower, newvalue = self._data[index]
						if newpower > self._maxpower:
							self._maxpower = newpower
							self._maxpowerindex = index
					return value
		return None

	def randomItem(self, dice):
		if len(self._data) > 0:
			item = getRand(dice) * self._sum
			counted = 0
			foundindex = -1
			for index in range(len(self._data)):
				counted += self._data[index][0]
				if counted > item:
					foundindex = index
					break
			if foundindex >= 0:
				power, value = self._data[foundindex]
				return value
			else:
				if self._maxpowerindex != -1:
					return self._data[self._maxpowerindex][1]
		return None

# multi dimensional array wrapper
# usage:
# - creating array:
# a = mdArray([5,9],0) #creates 59 array with data:0 in every cell
# b = mdArray([4,2,6],"nodata") #creates 426 array with data:"nodata" in every cell
# - changing data:
# a[0,1] = 5 #change 01 cell to 5
# b[3,1,5] = "some data" #change 315 cell to "some data"
# - also:
# print a[0,1]
# print len(a)
# - changing data and retrieve offset:
# offset = a.getOffset([1,1])
# a._data[offset] = 6
class mdArray:
	"multi dimensional array handler"
	def __init__(self, dimensions, initial = None):
		dimlen = len(dimensions)
		self._dimensions = [0] * dimlen
		self._factors = [0] * dimlen
		product = 1
		i = dimlen - 1
		while i >= 0:
			self._dimensions[i] = dimensions[i]
			self._factors[i] = product
			product *= self._dimensions[i]
			i -= 1
		self._data = [initial] * product

	def getOffset(self, indices):
		offset = 0
		for i, dim in enumerate(self._dimensions):
			offset += self._factors[i] * indices[i]
		return offset

	def getPos(self, offset):
		indices = []
		product = offset
		for i, dim in enumerate(self._dimensions):
			value = product / self._factors[i]
			indices.append(value)
			product %= self._factors[i]
		return indices

	def __getitem__(self, indices):
		return self._data[self.getOffset(indices)]

	def __setitem__(self, indices, value):
		self._data[self.getOffset(indices)] = value

	def get(self):
		return self._data

	def __len__():
		return len(self._data)

#for saving grid data, wraps in w
class worldArray(mdArray):
	def __init__(self, dimensions, initial = None):
		if len(dimensions) != 2: #only allow 2 dimensions
			raise IndexError
		dimensions_copy = [dimensions[1],dimensions[0]] #reverse dimensions (required to pass plot types corectly)
		dimlen = len(dimensions_copy)
		self._dimensions = [0] * dimlen
		self._factors = [0] * dimlen
		product = 1
		i = dimlen - 1
		while i >= 0:
			self._dimensions[i] = dimensions_copy[i]
			self._factors[i] = product
			product *= self._dimensions[i]
			i -= 1
		self._data = [initial] * product

	def getOffset(self, indices):
		indices_copy = [indices[1],indices[0]] #reverse indices (required to match dimensions)

		# make sure array wraps if out of bounds
		while indices_copy[0] < 0:
			indices_copy[0] += self._dimensions[0]
		while indices_copy[0] > self._dimensions[0] - 1:
			indices_copy[0] -= self._dimensions[0]

		while indices_copy[1] < 0:
			indices_copy[1] += self._dimensions[1]
		while indices_copy[1] > self._dimensions[1] - 1:
			indices_copy[1] -= self._dimensions[1]

		offset = 0
		for i, dim in enumerate(self._dimensions):
			offset += self._factors[i] * indices_copy[i]
		return offset

	def getPos(self, offset):
		indices = []
		product = offset
		for i, dim in enumerate(self._dimensions):
			value = product / self._factors[i]
			indices.insert(0,value)
			product %= self._factors[i]
		return indices

	def getHeight(self):
		return self._dimensions[0]

	def getWidth(self):
		return self._dimensions[1]

class ContinentPart:
	def __init__(self,id,builder):
		self.builder = builder
		self.tiles = bArray()
		self.id = id
		self.links = bArray()

	def deepMaxTiles(self,used = bArray()):
		if used[self.id] == None:
			used[self.id] = True
			maxTiles = len(self.tiles)
			for link in self.links:
				id, o1, o2 = link
				part = self.builder.continentParts[id]
				partMax = part.deepMaxTiles(used)
				if partMax > maxTiles:
					maxTiles = partMax
			return maxTiles
		else:
			return 0

# snaky continent generation algorythm
class TileBuilder:
	def __init__(self,world,tileType,builderTiles,activeTiles,growCount,continentSeparation,targetSize):
		self.cgc = CyGlobalContext()
		self.dice = self.cgc.getGame().getMapRand()
		self.map = self.cgc.getMap()
		self.world = world
		self.tileType = [tileType] # what tile type we are building
		if isinstance(builderTiles,list):
			self.builderTiles = builderTiles
		else:
			self.builderTiles = [builderTiles] # what tiles spawn new tiles
		if isinstance(activeTiles,list):
			self.activeTiles = activeTiles
		else:
			self.activeTiles = [activeTiles] # how many tiles are active for becomming "builder" tiles in case builders extinct
		if isinstance(growCount,list):
			self.growCount = growCount
		else:
			self.growCount = [growCount] # how much tiles one builder tile tries to spawn per life
		self.continentSeparation = continentSeparation # how far same type of tiles (but not active) must be
		self.poleSeparation = 3
		self.baseTerrain = [] #add plot types to this array to make tile builder build only on them
		self.targetSize = targetSize
		self.allowReachPoles = True
		self.coastScatterMin = 1 # minimum continent scattering
		self.coastScatterMax = 2 # maximum scattering
		self.coastScatterChangeChance = 0.1 # scattering change chance per one tile grow
		self.coastScatterChangeDif = 1 # scatering maximum change per change
		self.initialScatter = -1 #initial scatter value; if == -1, will be chosen randomly
		self.refreshCountRequired = 6
		self.continentPartControlEnabled = True
		self.continentParts = bArray()
		self.continentPartsIdGen = -1
		self.continentPartAllTiles = bArray()

	def start(self, x, y):
		self.progress = 0
		self.refreshCount = 0
		self.nextRefresh = self.targetSize / float(self.refreshCountRequired + 1) * (self.refreshCount + 1)
		self.world[x,y] = self.tileType[0]
		offset = self.world.getOffset([x,y])
		if self.continentPartControlEnabled:
			self.continentPartsIdGen += 1
			part = ContinentPart(self.continentPartsIdGen,self)
			self.continentParts[part.id] = part
			part.tiles[offset] = offset
			self.continentPartAllTiles[offset] = part.id
		if self.initialScatter == -1:
			if self.coastScatterMin == 0:
				initial_scatt = 0
			else:
				initial_scatt = self.coastScatterMin
		else:
			initial_scatt = self.initialScatter
		if getRand(self.dice) > 0.5:
			initial_scattdir = 1
		else:
			initial_scattdir = -1
		self.builders = bArray() # for storing sorted builder tiles for fast access
		self.builders[offset] = [x, y, initial_scatt, initial_scattdir]
		self.active = bArray() # for storing sorted active tiles for fast access
		self.allTiles = bArray()
		self.active[offset] = [x, y, self.getPrValue(self.activeTiles) / self.getPrValue(self.growCount), initial_scatt, initial_scattdir]
		self.allTiles[offset] = [x, y, self.getPrValue(self.activeTiles) / self.getPrValue(self.growCount), initial_scatt, initial_scattdir]
		self._growInd = 0
		self.builderList = [] # for storing sequence in which builder tiles were created
		self.builderList.append(offset)
		self.possibleTiles = bArray() # for storing possible grow tile list

	def getPrValue(self, prArray):
		if len(prArray) == 1:
			return int(prArray[0])
		elif self.progress >= self.targetSize:
			return int(prArray[len(prArray) - 1])
		else:
			scale = self.progress / float(self.targetSize)
			onepiece = 1 / float(len(prArray))
			piecesum = onepiece
			index = 0
			while scale > piecesum:
				piecesum += onepiece
				index += 1
			if index >= len(prArray) - 1:
				return int(prArray[len(prArray) - 1])
			else:
				piecesum -= onepiece
				lscale = (scale - piecesum) / float(onepiece)
				return int(prArray[index] + (prArray[index + 1] - prArray[index]) * lscale)

	def grow(self):
		world = self.world
		result = True

		builderTiles = self.getPrValue(self.builderTiles)
		activeTiles = self.getPrValue(self.activeTiles)
		growCount = self.getPrValue(self.growCount)

		# increase index (because only one tile per function)
		self._growInd += 1
		# pradzioj rasti langelius aplink
		if self._growInd == 1 and len(self.possibleTiles) < 8: # if there is shortage of possible tiles, remake possible tile list
			self.possibleTiles = self._getPossibleGrowTiles()

		#check if special refresh is needed
		if self.progress > self.nextRefresh:
			#print " ------------------ SPECIAL REFRESH -------------------- "
			self.refreshCount += 1
			self.nextRefresh = self.targetSize / float(self.refreshCountRequired + 1) * (self.refreshCount + 1)
			for index in range(int(activeTiles / 15)):
				tile = self.allTiles.randomItem(self.dice)
				if tile != None:
					x, y, d, scatt, scattdir = tile
					if not self._intersect(x,y):
						self._makeActive(x,y)
				else:
					break

		# try to find more tiles if there is no more
		#fast method first:
		if len(self.possibleTiles) == 0:
			self.builders = bArray()
			c = 0
			for j in range(builderTiles * 2):
				tile = self.active.randomItem(self.dice)
				if tile != None:
					x, y, d, scatt, scattdir = tile
					c = self._checkPosGrow(self.possibleTiles,x,y,scatt,scattdir)
					if (c > 0):
						if len(self.builders) < builderTiles:
							offset = world.getOffset([x,y])
							self.builders[offset] = [x, y, scatt, scattdir]
							self.builderList.append(offset)
						else:
							break
				else:
					break

		#extreme method (slow as hell):
		if len(self.possibleTiles) == 0:
			#print " ------------------ USING EXTREME METHOD -------------------- "
			newActive = bArray()
			for index in range(len(self.active)):
				tile = self.active.random(self.dice)
				if tile != None:
					x, y, d, scatt, scattdir = tile
					offset = world.getOffset([x,y])
					newActive[offset] = tile
					c = self._checkPosGrow(self.possibleTiles,x,y,scatt,scattdir)
					if (c > 0):
						if len(self.builders) < builderTiles:
							self.builders[offset] = [x, y, scatt, scattdir]
							self.builderList.append(offset)
			self.active = newActive

		if len(self.possibleTiles) == 0:
			c = 0
			#print " --------------- USING EMERGENCY TILE SEARCH ----------------- "
			#clear active tiles, we will make new
			self.active = bArray()
			for index in range(len(self.allTiles)):
				tile = self.allTiles.randomItem(self.dice)
				if tile != None:
					x, y, d, scatt, scattdir = tile
					if self._makeActive(x,y):
						c += self._checkPosGrow(self.possibleTiles,x,y,scatt,scattdir)
						if len(self.active) >= activeTiles:
							break
				else:
					break
			#add tiles to builders
			if (c > 0):
				while len(self.builders) < builderTiles:
					tile = self.possibleTiles.random(self.dice)
					if (tile != None):
						if len(self.builders) < builderTiles:
							x, y, scatt, scattdir, origoffset = tile
							offset = world.getOffset([x,y])
							self.builders[offset] = [x, y, scatt, scattdir]
							self.builderList.append(offset)
						else:
							break
					else:
						break

		# grow one tile
		if len(self.possibleTiles) > 0:
			x, y, scatt, scattdir, origoffset = self.possibleTiles.random(self.dice) # take one tile from possible tile array
			offset = world.getOffset([x, y])
			self.active[offset] = [x, y, activeTiles / growCount, scatt, scattdir]
			self.allTiles[offset] = [x, y, activeTiles / growCount, scatt, scattdir]
			world[x, y] = self.tileType[0]
			#zx, zy = self.world.getPos(offset)
			#print "Offset %d translated to x:%d y:%d while original is x:%d y:%d"%(offset,zx,zy,x,y)
			if self.continentPartControlEnabled:
				tiles = getTilesAround(x,y)
				partsAround = bArray()
				for tile in tiles:
					tx, ty = tile
					toffset =  world.getOffset([tx, ty])
					partId = self.continentPartAllTiles[toffset]
					if partId != None:
						partsAround[partId] = partId
				if len(partsAround) == 0:
					self.continentPartsIdGen += 1 #gaunam nauja id
					part = ContinentPart(self.continentPartsIdGen,self) #sukuriam nauja part
					part.tiles[offset] = offset #idedam tile i sarasa
					self.continentParts[part.id] = part #idedam part i sarasa
					#print "New lonely tile: x:%d y:%d id:%d"%(x,y,part.id)
					self.continentPartAllTiles[offset] = part.id #idedam part id i tile sarasa
					currentPartId = self.continentPartAllTiles[origoffset] #gaunam dabartinio part id
					currentPart = self.continentParts[currentPartId] #gaunam dabartinio part obj
					part.links[currentPart.id] = [currentPart.id, offset, origoffset] #idedam linka
					#print "Add link from id:%d to id:%d orig:%d new:%d"%(part.id,currentPart.id,offset,origoffset)
					currentPart.links[part.id] = [part.id, origoffset, offset] #idedam linka
					#print "Add link from id:%d to id:%d orig:%d new:%d"%(currentPart.id,part.id,origoffset,offset)
				else:
					part = self.continentParts[partsAround._data[0][1]]
					#print "Join tile x:%d y:%d with %d continent parts, main part id: %d"%(x,y,len(partsAround),part.id)
					for i in range(len(partsAround) - 1):
						nextPart = self.continentParts[partsAround._data[i+1][1]]
						#print "Join with id: %d"%(nextPart.id)
						for partTileOffset in nextPart.tiles:
							part.tiles[partTileOffset] = partTileOffset
							self.continentPartAllTiles[partTileOffset] = part.id
						nextPart.tiles = None
						for link in nextPart.links:
							otherId, thisOffset, otherOffset = link
							if otherId == part.id:
								part.links.remove(nextPart.id)
							else:
								part.links[otherId] = link
								#print "Add link from id:%d to id:%d orig:%d new:%d"%(part.id,otherId,thisOffset,otherOffset)
								otherPart = self.continentParts[otherId]
								otherPart.links.remove(nextPart.id)
								#print "Remove link from id:%d to id:%d"%(otherPart.id,nextPart.id)
								otherPart.links[part.id] = [part.id, otherOffset, thisOffset]
								#print "Add link from id:%d to id:%d orig:%d new:%d"%(otherPart.id,part.id,otherOffset,thisOffset)
						nextPart.links = None
						nextPart.builder = None
						self.continentParts.remove(nextPart.id)
						#print "Remove part: %d"%(nextPart.id)
					part.tiles[offset] = offset
					self.continentPartAllTiles[offset] = part.id
			#add tile as possible builder
			scale = self.progress / float(self.targetSize)
			if scale > 1:
				scale = 1
			elif scale <=0:
				scale = 0.01
			if scattdir < 0:
				bChance = self.coastScatterChangeChance + self.coastScatterChangeChance / (1 + scale * 5)
			else:
				bChance = self.coastScatterChangeChance - self.coastScatterChangeChance / (1 + scale * 5)
			if getRand(self.dice) < bChance:
				scattchange = scattdir * (1 + int(self.coastScatterChangeDif * getRand(self.dice)))
				if scatt + scattchange > self.coastScatterMax:
					scatt = self.coastScatterMax
					scattdir = -scattdir
				elif scatt + scattchange < self.coastScatterMin:
					scatt = self.coastScatterMin
					scattdir = -scattdir
				else:
					scatt += scattchange
			self.builders[offset] = [x, y, scatt, scattdir]
			self.builderList.append(offset)
		else:
			return False

		# readjusting builders
		while len(self.builders) > builderTiles:
			offset = self.builderList.pop(0)
			self.builders.remove(offset)

		if self._growInd == growCount:
			self._growInd = 0
			#decrease active tiles
			i = len(self.active)
			while i > 0:
				i -= 1
				if self.active._data[i][1][2] < 1:
					self.active.pop(i)
				else:
					self.active._data[i][1][2] -= 1

		self.progress += 1

		return result

	def joinParts(self, minSize):
		if self.continentPartControlEnabled:
			bUsed = bArray()
			for part in self.continentParts:
				if bUsed[part.id] == None:
					if len(part.tiles) >= minSize:
						for link in part.links:
							otherId, thisOffset, otherOffset = link
							if bUsed[otherId] == None:
								otherPart = self.continentParts[otherId]
								usedTiles = bArray()
								usedTiles[part.id] = True
								if otherPart.deepMaxTiles(usedTiles) >= minSize:
									ax, ay = self.world.getPos(thisOffset)
									#print "This offset %d translated to x:%d y:%d"%(thisOffset,ax,ay)
									bx, by = self.world.getPos(otherOffset)
									#print "Other offset %d translated to x:%d y:%d"%(otherOffset,bx,by)
									if ax > bx:
										if ax - bx > self.world.getWidth() + bx - ax:
											bx += self.world.getWidth()
									else:
										if bx - ax > self.world.getWidth() + ax - bx:
											ax += self.world.getWidth()
									dx = bx - ax
									dy = by - ay
									if abs(dx) > abs(dy):
										d = abs(dx)
									else:
										d = abs(dy)
									p = 0
									if dx < 0:
										dx -= 1
									elif dx > 0:
										dx += 1
									if dy < 0:
										dy -= 1
									elif dy > 0:
										dy += 1
									while p < d - 1:
										p += 1
										progress = p / float(d + 1)
										x = int(round(dx * progress,0)) + ax
										y = int(round(dy * progress,0)) + ay
										self.world[x,y] = PlotTypes.PLOT_LAND
								else:
									bUsed[otherId] = None
					bUsed[part.id] = True

	def _makeActive(self, x, y):
		dice = self.dice

		cs = self.continentSeparation
		r = range(cs * 2)

		for bx in r:
			for by in r:
				ax = x - cs + bx
				ay = y - cs + by
				if self.world[ax, ay] in self.tileType:
					offset = self.world.getOffset([ax, ay])
					tile = self.allTiles[offset]
					if tile != None:
						self.active[offset] = tile
					else:
						return False
		return True

	def _intersect(self, x, y):
		dice = self.dice

		cs = self.continentSeparation
		r = range(cs * 2)

		for i in r:
			ax = x - cs + i
			ay = y + cs
			offset = self.world.getOffset([ax, ay])
			if (not self.active[offset]) and self.world[ax, ay] in self.tileType:
				return True
		for i in r:
			ax = x + cs
			ay = y + cs - i
			offset = self.world.getOffset([ax, ay])
			if (not self.active[offset]) and self.world[ax, ay] in self.tileType:
				return True
		for i in r:
			ax = x + cs - i
			ay = y - cs
			offset = self.world.getOffset([ax, ay])
			if (not self.active[offset]) and self.world[ax, ay] in self.tileType:
				return True
		for i in r:
			ax = x - cs
			ay = y - cs + i
			offset = self.world.getOffset([ax, ay])
			if (not self.active[offset]) and self.world[ax, ay] in self.tileType:
				return True
		return False

	# dont ask me to explain anything here, just keep reading and enjoy :D
	def _checkPosGrowMatrix(self,matrix,list,x,y,scatt,scattdir):
		world = self.world
		c = 0
		h = len(matrix)
		w = len(matrix[0])
		dh = (h - 1) / 2
		dw = (w - 1) / 2

		ps = self.poleSeparation

		if self.allowReachPoles:
			ps = 1
		for ax in range(w):
			for ay in range(h):
				cell = matrix[ay][ax]
				if cell > 0:
					px = x + (ax - dw)
					py = y + (ay - dh)
					if (len(self.baseTerrain) == 0):
						wtt = world[px,py]
						if py > y:
							if py < world._dimensions[0]-ps-1 and not wtt in self.tileType and not self._intersect(px,py):
								list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
								c += 1
						elif py < y:
							if py > ps and not wtt in self.tileType and not self._intersect(px,py):
								list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
								c += 1
						else:
							if not wtt in self.tileType and not self._intersect(px,py):
								list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
								c += 1
					else:
						wtt = world[px,py]
						if py > y:
							if py < world._dimensions[0]-ps-1 and wtt in self.baseTerrain and not self._intersect(px,py):
								list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
								c += 1
						elif py < y:
							if py > ps and wtt in self.baseTerrain and not self._intersect(px,py):
								list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
								c += 1
						else:
							if wtt in self.baseTerrain and not self._intersect(px,py):
								list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
								c += 1
		return c

	# dont ask me to explain anything here, just keep reading and enjoy :D
	def _checkPosGrowList(self,matrix,list,x,y,scatt,scattdir):
		world = self.world
		c = 0

		ps = self.poleSeparation

		if self.allowReachPoles:
			ps = 1
		for cell in matrix:
			px, py = cell
			if (len(self.baseTerrain) == 0):
				wtt = world[px,py]
				if py > y:
					if py < world._dimensions[0]-ps-1 and not wtt in self.tileType and not self._intersect(px,py):
						list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
						c += 1
				elif py < y:
					if py > ps and not wtt in self.tileType and not self._intersect(px,py):
						list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
						c += 1
				else:
					if not wtt in self.tileType and not self._intersect(px,py):
						list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
						c += 1
			else:
				wtt = world[px,py]
				if py > y:
					if py < world._dimensions[0]-ps-1 and wtt in self.baseTerrain and not self._intersect(px,py):
						list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
						c += 1
				elif py < y:
					if py > ps and wtt in self.baseTerrain and not self._intersect(px,py):
						list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
						c += 1
				else:
					if wtt in self.baseTerrain and not self._intersect(px,py):
						list[world.getOffset([px,py])] = [px,py,scatt,scattdir,world.getOffset([x,y])]
						c += 1
		return c

	def _checkPosGrow(self,list,x,y,scatt,scattdir):
		world = self.world
		dice = self.dice
		c = 0
		iH = world.getHeight()
		#matrix of what tiles to check around builder tiles. 1 - available for building, 0 - not
		if scatt == 0:
			this_scatt = 1
		else:
			this_scatt = scatt

		if (this_scatt == 1):
			flipUp = False
			flipDown = False
			if y > iH / 2: #get chance to flip up
				scale = (iH - y) / (iH / 2.0) # 0 - equator, 1 - pole
				if getRand(dice) < scale:
					flipUp = True
			elif y < iH / 2:
				scale = 1 - y / (iH / 2.0) # 0 - equator, 1 - pole
				if getRand(dice) < scale:
					flipDown = True
			if flipUp:
				default_matrix = [
					[1,1,1],
					[1,0,1],
					[1,0,1],
				]
			elif flipDown:
				default_matrix = [
					[1,0,1],
					[1,0,1],
					[1,1,1],
				]
			else:
				default_matrix = [
					[1,1,1],
					[1,0,1],
					[1,1,1],
				]
			c += self._checkPosGrowMatrix(default_matrix, list, x, y, scatt, scattdir)
		else:
			#get chance to flip
			half = iH / 2
			if y < half / 3 * 2:
				scale = 1 - y / float(half / 3 * 2 - 1)
			elif y >= half / 3 * 4:
				scale = - (y - half / 3 * 4) / float(half / 3 * 2 - 1)
			else:
				scale = 0
			matrix_list = bArray()
			point_count = int(8 + (this_scatt - 1) * 2.5)
			for index in range(point_count):
				shift = 0.8
				t = getRand(dice) * 3.141592 * 2
				r = getRand(dice) * (this_scatt + 1) + 1
				rx = int(r * math.cos(t))
				ry = r * math.sin(t)
				if scale != 0:
					if scale > 0:
						if ry < 0:
							ry = int(ry - scale * shift * ry)
						else:
							ry = int(ry)
					else:
						if ry > 0:
							ry = int(ry + scale * shift * ry)
						else:
							ry = int(ry)
				else:
					ry = int(ry)
				if (not (rx == 0 and ry == 0)):
					matrix_list[world.getOffset([x + rx,y + ry])] = [x + rx, y + ry]
			c += self._checkPosGrowList(matrix_list, list, x, y, scatt, scattdir)
		return c

	def _getPossibleGrowTiles(self):
		world = self.world
		result = bArray()
		rem_ind = []
		i = 0
		for item in self.builders:
			x, y, scatt, scattdir = item
			c = self._checkPosGrow(result,x,y,scatt,scattdir)
			if c == 0:
				rem_ind.append(i)
			i += 1
		# remove unecessary builder tiles
		x = len(rem_ind)
		while x > 0:
			x -= 1
			self.builders.pop(x)
		return result

def generateRandomMap():
	"""Can be overridden if you want to manually control the sequence in which plot types, heights, and terrain are generated.

	WARNING: If you override this function, then none of the following functions will be automatically called for you:
		generateTerrain()
		generatePlotTypes()

	However, unless you have some specific reason to change	the	sequence of	map	generation, you should probably just override
	one or more of the functions listed above.
	"""
	#print "--------- Generate random map"
	CyPythonMgr().allowDefaultImpl()

def initReplacements():
	# 0 - nothing
	# 1 - water
	# 2 - land
	rep = []
	orig = [
		[0,2,0],
		[2,1,2],
		[0,2,0],
		]
	final = [
		[2],
		]
	rep.append([orig,final])
	orig = [
		[0,2,2,0],
		[2,1,1,2],
		[0,2,2,0],
		]
	final = [
		[2,2],
		]
	rep.append([orig,final])
	orig = [
		[0,2,0],
		[2,1,2],
		[2,1,2],
		[0,2,0],
		]
	final = [
		[2],
		[2],
		]
	rep.append([orig,final])
	# unify islands
	orig = [
		[1,1,1,1,1],
		[1,1,1,1,1],
		[1,2,1,2,1],
		[1,1,1,1,1],
		]
	final = [
	[
		[0,2,0],
		[2,0,2],
	],
	[
		[0,2,2],
		[2,0,2],
	],
	[
		[2,2,0],
		[2,0,2],
	],
	[
		[0,2,0],
		[2,2,2],
	],
		]
	rep.append([orig,final])
	orig = [
		[1,1,1,1,1],
		[1,2,1,2,1],
		[1,1,1,1,1],
		[1,1,1,1,1],
		]
	final = [
	[
		[2,0,2],
		[0,2,0],
	],
	[
		[2,0,2],
		[0,2,2],
	],
	[
		[2,0,2],
		[2,2,0],
	],
	[
		[2,2,2],
		[0,2,0],
	],
		]
	rep.append([orig,final])
	orig = [
		[1,1,1,1,1],
		[1,2,1,1,1],
		[1,1,1,2,1],
		[1,1,1,1,1],
		]
	final = [
	[
		[2,0,0],
		[0,2,2],
	],
	[
		[2,2,0],
		[0,0,2],
	],
		]
	rep.append([orig,final])
	orig = [
		[1,1,1,1,1],
		[1,1,1,2,1],
		[1,2,1,1,1],
		[1,1,1,1,1],
		]
	final = [
	[
		[0,2,2],
		[2,0,0],
	],
	[
		[0,0,2],
		[2,2,0],
	],
		]
	rep.append([orig,final])
	orig = [
		[1,1,1,1],
		[1,2,1,1],
		[1,1,1,1],
		[1,2,1,1],
		[1,1,1,1],
		]
	final = [
	[
		[2,0],
		[0,2],
		[2,0],
	],
	[
		[2,2],
		[0,2],
		[2,0],
	],
	[
		[2,2],
		[0,2],
		[2,2],
	],
	[
		[2,2],
		[2,2],
		[2,0],
	],
	[
		[2,0],
		[2,2],
		[2,2],
	],
		]
	rep.append([orig,final])
	orig = [
		[1,1,1,1],
		[1,1,2,1],
		[1,1,1,1],
		[1,1,2,1],
		[1,1,1,1],
		]
	final = [
	[
		[0,2],
		[2,0],
		[0,2],
	],
	[
		[2,2],
		[2,2],
		[0,2],
	],
	[
		[0,2],
		[2,2],
		[2,2],
	],
	[
		[0,2],
		[2,2],
		[2,2],
	],
	[
		[0,2],
		[2,0],
		[2,2],
	],
	[
		[2,2],
		[2,0],
		[0,2],
	],
		]
	rep.append([orig,final])
	orig = [
		[1,1,1,1],
		[1,2,1,1],
		[1,1,1,1],
		[1,1,2,1],
		[1,1,1,1],
		]
	final = [
	[
		[2,0],
		[2,0],
		[0,2],
	],
	[
		[2,0],
		[0,2],
		[0,2],
	],
	[
		[2,2],
		[0,2],
		[0,2],
	],
	[
		[2,2],
		[2,2],
		[0,2],
	],
	[
		[2,0],
		[2,2],
		[2,2],
	],
		]
	rep.append([orig,final])
	orig = [
		[1,1,1,1],
		[1,1,2,1],
		[1,1,1,1],
		[1,2,1,1],
		[1,1,1,1],
		]
	final = [
	[
		[0,2],
		[2,0],
		[2,0],
	],
	[
		[0,2],
		[0,2],
		[2,0],
	],
	[
		[0,2],
		[2,2],
		[2,0],
	],
	[
		[0,2],
		[2,2],
		[2,2],
	],
	[
		[2,2],
		[2,2],
		[2,0],
	],
		]
	rep.append([orig,final])
	return rep

def landR(i):
	if i == 0:
		return None
	elif i == 1:
		return PlotTypes.PLOT_OCEAN
	elif i == 2:
		return PlotTypes.PLOT_LAND

# called by Firaxis - expects to get array of plot types
def generatePlotTypes():
	collectGarbage()

	global generationStarted
	global selection_titles

	global world # our world array

	global continentCount
	global continentSeparation

	global allowLandNearIce
	global poleSeparation

	poleSeparation = 3

	cgc = CyGlobalContext()
	map = cgc.getMap()

	#if generationStarted:
	#	print "=============== receives latitudes and map size =============== "
	#else:
	#	print "=============== no lattude or map size change, regeneration =============== "

	#print "generate plot types"
	dice = cgc.getGame().getMapRand() #dice is Firaxis' random number generator
	players = cgc.getGame().countCivPlayersEverAlive()
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	#print "----------------- generating world",
	oceanPercentage = getSelValue("ocean")
	if oceanPercentage is None or oceanPercentage > 100: #ensure we don't make something bad if wrong settings exist
		oceanPercentage = 70
	continentShape = getSelValue("continent_shape") #string value
	if continentShape is None: #if something bad with settings, jump to normal
		continentShape = "natural"
	continentSize = getSelValue("continent_size")
	if continentSize is None:
		continentSize = "players"
	continentScatter = getSelValue("continent_scattering")
	if continentScatter is None:
		continentScatter = [0,0,0,0]
	lonelyIslandSize = getSelValue("lonely_island_size")
	if lonelyIslandSize is None:
		lonelyIslandSize = "medium"
	lonelyIslands = getSelValue("lonely_islands")
	if lonelyIslands is None:
		lonelyIslands = "one"
	allowOneTileIslands = getSelValue("allow_one_tile_islands")
	if allowOneTileIslands is None:
		allowOneTileIslands = True
	allowLandNearIce = getSelValue("land_near_ice")
	if allowLandNearIce is None:
		allowLandNearIce = False
	# there is list of templates which are used for various continent shape types
	# there are three parameters in this array (separated by commas): [builder tiles, active tiles, grow count]

	#first type of natural shape:
	#this happens to be like snaky
	tbsSnakyNatural = [2, iW * iH / 8, 2] # iW * iH / 8 = MapWidth * MapHeight / 8 = active tiles

	#second type of natural shape:
	#more thick than first natural
	tbsTravelingNatural = [4, iW * iH / 10, 4] # iW * iH / 10 = MapWidth * MapHeight / 10 = active tiles

	#new2 - spreads equally to all directions, posibility of peninsulas and inland seas
	tbsNatural = [[7,6,6], [iW * iH / 12, iW * iH / 6], [4,2,4,2,2]]

	#new1 - spreads equaly to all directions, can huge peninsulas happen
	tbsSmooth = [15, iW * iH / 3, 4]

	#new3 - spreads equally to all directions, has huge landmass in the middle and many peninsulas, low probability of inside lakes
	sn = iW * iH / 3
	if sn < 40:
		sn = 40
		tbsCrabLike = [[20,4], sn, 2]
	else:
		tbsCrabLike = [[20,4], sn, 2]

	#snaky template:
	#when generating snaky continent, we must ensure active tile count is not too low, so we check for that
	sn = iW * iH / 60
	if sn < 40:
		sn = 40
	tbsSnaky = [3, sn, 2]

	#snaky-thin template:
	#this is extreemly thin
	sn = iW * iH / 50
	if sn < 40:
		sn = 40
	tbsSnakyThin = [2, sn, 1]

	# if random for each continent is chosen
	if continentShape == "continent-random":
		shapeRandom = True
		if iW < 64: # < standart
			if continentCount <= 2:
				randomList = [tbsSnaky,tbsSnakyNatural,tbsTravelingNatural]
			elif continentCount <= 5:
				randomList = [tbsSnaky,tbsSnakyNatural,tbsTravelingNatural,tbsNatural]
			else:
				randomList = [tbsSnaky,tbsSnakyNatural,tbsTravelingNatural,tbsNatural,tbsCrabLike]
		elif iW < 128: # < huge
			if continentCount <= 2:
				randomList = [tbsSnakyNatural,tbsTravelingNatural,tbsNatural,tbsCrabLike]
			elif continentCount <= 5:
				randomList = [tbsSnaky,tbsSnakyNatural,tbsTravelingNatural,tbsNatural,tbsCrabLike]
			else:
				randomList = [tbsSnaky,tbsSnakyNatural,tbsTravelingNatural,tbsCrabLike]
		else: # >= huge
			if continentCount == 1:
				randomList = [tbsTravelingNatural,tbsSmooth,tbsNatural,tbsCrabLike]
			elif continentCount == 2:
				randomList = [tbsTravelingNatural,tbsSmooth,tbsNatural,tbsCrabLike]
			elif continentCount <= 5:
				randomList = [tbsSnakyNatural,tbsTravelingNatural,tbsNatural,tbsCrabLike]
			else:
				randomList = [tbsSnaky,tbsSnakyNatural,tbsTravelingNatural,tbsNatural,tbsCrabLike]
	else: #otherwise we select template
		shapeRandom = False
		if continentShape == "snaky-natural":
			shapeData = tbsSnakyNatural
		elif continentShape == "traveling-natural":
			shapeData = tbsTravelingNatural
		elif continentShape == "unpredictable":
			# if unpredictable, we generate random shifting template, different for every continent
			shapeRandom = True
			randomList = []
			for i in range(continentCount):
				randomList.append([[2 + getRand(dice) * 3, 2 + getRand(dice) * 3, 2 + getRand(dice) * 3], [iW * iH / (5 + getRand(dice) * 10), iW * iH / (5 + getRand(dice) * 10), iW * iH / (5 + getRand(dice) * 5)], [2 + getRand(dice) * 2, 2 + getRand(dice) * 2]])
		elif continentShape == "snaky":
			shapeData = tbsSnaky
		elif continentShape == "snaky-thin":
			shapeData = tbsSnakyThin
		elif continentShape == "smooth":
			shapeData = tbsSmooth
		elif continentShape == "natural":
			shapeData = tbsNatural
		elif continentShape == "crab":
			shapeData = tbsCrabLike

	#calculate actual land tiles we must produce
	maxLandTiles = iW * (iH - poleSeparation * 2) * ((100 - oceanPercentage) / 100.0)
	print "default max tiles:",maxLandTiles
	if continentCount > 0 and maxLandTiles >=0:
		newMaxLandTiles = ((( math.sqrt(maxLandTiles / float(continentCount))) - continentSeparation * 0.9)**2) * continentCount
		if newMaxLandTiles > continentCount * 2:
			maxLandTiles = newMaxLandTiles
	maxLandTiles = int(maxLandTiles * 0.95)
	print "modified max tiles:",maxLandTiles

	#world is two-dimensional array which wraps in X automagically, so you may not worry about edges
	world = worldArray([iW,iH],PlotTypes.PLOT_OCEAN)

	#now we must somehow distribute start positions for each continent, depending on number of players.
	#if there is not enough continents for players, we will make some random continent to grow more.
	#actual player distribution is handled by Firaxis code, which looks how big each continent is and places players acording to that.
	if players > continentCount and continentSize == "players":
		# we will make helper-array to randomly make some continents bigger
		contData = []
		# first of all, we make continent amount the same as plaer amount, and add to each continent 1 player:
		for p in range(players):
			contData.append(1) # 1 player in 1 continent

		#colapse continents to match what we have
		#colapsing: if we in helper-array have more continents than we need, remove one item from array
		#  and randomly increase other array item by 1
		#do so until we get right continent count
		while len(contData) > continentCount:
			index = int(getRand(dice) * len(contData)) #random index
			#merge it with nearest continent
			if index == 0:
				index2 = len(contData) - 1
			else:
				index2 = index - 1
			contData[index2] += contData[index]
			contData.pop(index)


		#position calculation moved to other function
		contPositions = getStartPositions(world, contData)

		#start continents
		contList = [] #contList will contain TileBuilders for each continent

		for i in range(len(contData)): #now contData contains how many players are in each continent
			if shapeRandom: #if we have random shape for each continent, we get it now
				index = int(getRand(dice) * len(randomList)) #random index
				shapeData = randomList[index]
			#initialize tile builder
			continent = TileBuilder(world, PlotTypes.PLOT_LAND, shapeData[0], shapeData[1], shapeData[2], continentSeparation, int(maxLandTiles / float(players) * contData[i]))
			continent.coastScatterMin = continentScatter[0]
			continent.coastScatterMax = continentScatter[1]
			continent.coastScatterChangeChance = continentScatter[2]
			continent.coastScatterChangeDif = continentScatter[3]
			continent.allowReachPoles = allowLandNearIce
			if continent.coastScatterMin == 0 or continent.coastScatterMin > 7:
				continent.continentPartControlEnabled = False
			#place first tile (x, y)
			startX, startY = contPositions[i]
			continent.start(startX, startY)
			#add tile builder to builder list
			contList.append(continent)

		#now what is left - growing :)
		size = 0 #at first size is 0
		grows = True #variable to check if everything still grows
		#if size < maxLandTiles and grows:
		while size < maxLandTiles and grows:
			for i, continent in enumerate(contList): #for each index, TileBuilder in contList
				if contData[i] == 1: #if one player is supposed to be there
					if continent.grow(): #if tile builder is able to grow and grows one tile
						size += 1 #increase size by one
					else:
						grows = False #otherwise stop this script
				else: #if more than one player is there, grow it for each of them :)
					cGrows = True
					for j in range(contData[i]): # grow continent more if there are more players in it
						if continent.grow():
							size += 1
						else:
							cGrows = False
					if not cGrows:
						grows = False

		for i, continent in enumerate(contList):
			#print "Final %d continent parts: %d"%(i,len(continent.continentParts))
			continent.joinParts(continent.coastScatterMax * 2)
	elif continentSize == "unpredictable":
		contPositions = getStartPositions(world, None)
		contList = []

		for i in range(continentCount):
			if shapeRandom:
				index = int(getRand(dice) * len(randomList)) #random index
				shapeData = randomList[index]
			continent = TileBuilder(world, PlotTypes.PLOT_LAND, shapeData[0], shapeData[1], shapeData[2], continentSeparation, maxLandTiles / continentCount)
			continent.coastScatterMin = continentScatter[0]
			continent.coastScatterMax = continentScatter[1]
			continent.coastScatterChangeChance = continentScatter[2]
			continent.coastScatterChangeDif = continentScatter[3]
			continent.allowReachPoles = allowLandNearIce
			if continent.coastScatterMin == 0 or continent.coastScatterMin > 7:
				continent.continentPartControlEnabled = False
			startX, startY = contPositions[i]
			continent.start(startX, startY)
			contList.append(continent)

		#grow continents
		size = 0
		grows = True
		while size < maxLandTiles and grows:
		#if size < maxLandTiles and grows:
			for continent in contList:
				while getRand(dice) > 0.6:
					if continent.grow() and size < maxLandTiles:
						size += 1
					else:
						grows = False
						break
		for i, continent in enumerate(contList):
			#print "Final %d continent parts: %d"%(i,len(continent.continentParts))
			continent.joinParts(continent.coastScatterMax)
	else: #if there are less players than continents, code looks like English ;)
		#start continents
		contPositions = getStartPositions(world, None)
		contList = []

		for i in range(continentCount):
			if shapeRandom:
				index = int(getRand(dice) * len(randomList)) #random index
				shapeData = randomList[index]
			continent = TileBuilder(world, PlotTypes.PLOT_LAND, shapeData[0], shapeData[1], shapeData[2], continentSeparation, maxLandTiles / continentCount)
			continent.coastScatterMin = continentScatter[0]
			continent.coastScatterMax = continentScatter[1]
			continent.coastScatterChangeChance = continentScatter[2]
			continent.coastScatterChangeDif = continentScatter[3]
			continent.allowReachPoles = allowLandNearIce
			if continent.coastScatterMin == 0 or continent.coastScatterMin > 7:
				continent.continentPartControlEnabled = False
			startX, startY = contPositions[i]
			continent.start(startX, startY)
			contList.append(continent)

		#grow continents
		size = 0
		grows = True
		while size < maxLandTiles and grows:
		#if size < maxLandTiles and grows:
			for continent in contList:
				if continent.grow():
					size += 1
				else:
					grows = False
		for i, continent in enumerate(contList):
			#print "Final %d continent parts: %d"%(i,len(continent.continentParts))
			continent.joinParts(continent.coastScatterMax * 2)

	print "size",size

	# now we want to remove one-square and two-square lakes. more templates (which describe how to remove tiles) can be added in initReplacements function,
	#  and code below should handle them just fine. i am not adding comments to it because i forgot how it works :D
	rep = initReplacements()

	for x in range(iW):
		for y in range(iH):
			# handle replacements
			for r in rep:
				orig, final_v = r
				h = len(orig)
				w = len(orig[0])
				dh = (h - 1) / 2
				dw = (w - 1) / 2
				Tinka = True
				for ax in range(w):
					for ay in range(h):
						cell = orig[ay][ax]
						if landR(cell) != None:
							if landR(cell) != world[x-dw+ax,y-dh+ay]:
								Tinka = False
								break
				if Tinka:
					if isinstance(final_v[0][0],list): # if array is deeper than coordinates
						index = int(getRand(dice) * len(final_v))
						final = final_v[index]
					else:
						final = final_v
					h = len(final)
					w = len(final[0])
					dh = (h - 1) / 2
					dw = (w - 1) / 2
					for ax in range(w):
						for ay in range(h):
							cell = final[ay][ax]
							if landR(cell) != None:
								world[x-dw+ax,y-dh+ay] = landR(cell)

	#ocean tiles we want to collect
	oceanTiles = bArray()

	if lonelyIslandSize == "small":
		islandAreaRadius = 3
	elif lonelyIslandSize == "medium":
		islandAreaRadius = 5
	else:
		islandAreaRadius = 7

	oceanSearchLineSize = islandAreaRadius * 2
	searchLinePos = 0
	for y in range(iH):
		for s in range(islandAreaRadius):
			x = s - islandAreaRadius - 1
			if y >= 4 and y < iH - 4:
				if world[x,y] == PlotTypes.PLOT_OCEAN:
					searchLinePos += 1
				else:
					searchLinePos = 0
		for x in range(iW):
			offset = world.getOffset([x,y])
			#collect ocean tiles
			if y >= 4 and y < iH - 4:
				if world[x,y] == PlotTypes.PLOT_OCEAN:
					searchLinePos += 1
					if searchLinePos >= oceanSearchLineSize:
						ax = x - islandAreaRadius
						oceanTiles[world.getOffset([ax,y])] = [ax, y]
				else:
					searchLinePos = 0
				if searchLinePos > oceanSearchLineSize:
					searchLinePos -= 1

	if lonelyIslands == "lonely1" or lonelyIslands == "lonely2" or lonelyIslands == "lonely3" or lonelyIslands == "lonely4" or lonelyIslands == "lonely5" or lonelyIslands > 0:
		# find places for small lonely islands

		possibleIslandList = bArray()
		for index in range(len(oceanTiles) / (islandAreaRadius * islandAreaRadius)):
			tile = oceanTiles.random(dice)
			if tile == None:
				break
			x, y = tile
			if world[x,y] == PlotTypes.PLOT_OCEAN:
				tileList = getTilesAroundDistance(x,y,islandAreaRadius)
				Tinka = True
				for tile in tileList:
					ax, ay = tile
					oceanTile = oceanTiles[world.getOffset([ax,ay])]
					if oceanTile == None:
						Tinka = False
						break
				if Tinka:
					oceanTiles.remove(world.getOffset([x,y]))
					for tile in tileList:
						ax, ay = tile
						oceanTiles.remove(world.getOffset([ax,ay]))
					possibleIslandList[world.getOffset([x,y])] = [x, y]

		islandStartList = bArray()
		# get island start positions
		for index in range(len(possibleIslandList)):
			possibleTile = possibleIslandList.random(dice)
			if possibleTile == None:
				break
			x, y = possibleTile
			islandStartList[world.getOffset([x,y])] = [x, y]
			tileList = getTilesAroundDistance(x,y,islandAreaRadius * 2)
			for tile in tileList:
				ax, ay = tile
				index, value = possibleIslandList._search(world.getOffset([ax,ay]),0,len(possibleIslandList._data)-1)
				if value != None:
					possibleIslandList._data.pop(index)

		if lonelyIslands == "lonely1":
			contPositions = []
			pos = islandStartList.random(dice)
			if pos != None:
				contPositions.append(pos)
		elif lonelyIslands == "lonely2":
			for index in range(2):
				pos = islandStartList.random(dice)
				if pos != None:
					contPositions.append(pos)
				else:
					break
		elif lonelyIslands == "lonely3":
			for index in range(3):
				pos = islandStartList.random(dice)
				if pos != None:
					contPositions.append(pos)
				else:
					break
		elif lonelyIslands == "lonely4":
			for index in range(4):
				pos = islandStartList.random(dice)
				if pos != None:
					contPositions.append(pos)
				else:
					break
		elif lonelyIslands == "lonely5":
			for index in range(5):
				pos = islandStartList.random(dice)
				if pos != None:
					contPositions.append(pos)
				else:
					break
		elif lonelyIslands == 1.0:
			contPositions = []
			for tile in islandStartList:
				contPositions.append(tile)
		else:
			contPositions = []
			for index in range(int(len(islandStartList) * lonelyIslands)):
				pos = islandStartList.random(dice)
				if pos != None:
					contPositions.append(pos)
				else:
					break

		contList = []
		sizeList = []
		growsList = []
		targetSizeList = []
		for i in range(len(contPositions)):
			shapeData = tbsTravelingNatural
			specScatter = [0.0,getRand(dice),getRand(dice)+1]
			if lonelyIslandSize == "small":
				targetSize = int(getRand(dice) * 2 * 2 + 2)
			elif lonelyIslandSize == "medium":
				targetSize = int(getRand(dice) * 6 * 6 + 16)
			else:
				targetSize = int(getRand(dice) * 8 * 8 + 20)
			sizeList.append(0)
			targetSizeList.append(targetSize)
			continent = TileBuilder(world, PlotTypes.PLOT_LAND, shapeData[0], shapeData[1], shapeData[2], continentSeparation, targetSize)
			continent.coastScatterMin = 1
			continent.coastScatterMax = 7
			continent.coastScatterChangeChance = 0.2
			continent.coastScatterChangeDif = 3
			continent.initialScatter = 1
			continent.refreshCountRequired = 0
			continent.allowReachPoles = allowLandNearIce
			if continent.coastScatterMin == 0 or continent.coastScatterMin > 7:
				continent.continentPartControlEnabled = False
			startX, startY = contPositions[i]
			continent.start(startX, startY)
			contList.append(continent)
			growsList.append(True)

		#grow islands
		grows = True
		while grows:
			somethingGrowing = False
			for index in range(len(contList)):
				continent = contList[index]
				if sizeList[index] < targetSizeList[index]:
					if growsList[index]:
						if continent.grow():
							sizeList[index] += 1
							somethingGrowing = True
						else:
							growsList[index] = False
			if not somethingGrowing:
				grows = False
		for i, continent in enumerate(contList):
			#print "Final %d continent parts: %d"%(i,len(continent.continentParts))
			continent.joinParts(3)

	# remove 1 - tile islands
	if not allowOneTileIslands:
		for x in range(iW):
			for y in range(iH - 2):
				if world[x,y+1] != PlotTypes.PLOT_OCEAN:
					tileList = getTilesAroundDistance(x,y+1,1)
					isIsland = True
					for tile in tileList:
						ax, ay = tile
						if world[ax,ay] != PlotTypes.PLOT_OCEAN:
							isIsland = False
					if isIsland:
						world[x,y+1] = PlotTypes.PLOT_OCEAN

	peakOptions = getSelValue("peaks")
	if peakOptions is None:
		peakOptions = [0,0,0]

	peakStartProbability = peakOptions[0]
	peakGrowProbability = peakOptions[1]
	maxPeaksInRow = peakOptions[2]

	hillOptions = getSelValue("hills")
	if hillOptions is None:
		hillOptions = [0,0,0]

	hillStartProbability = hillOptions[0]
	hillGrowProbability = hillOptions[1]
	maxHillsInRow = hillOptions[2]

	#peak storage:
	peakArray = bArray()

	#hill storage:
	hillArray = bArray()

	for y in range(iH):
		for x in range(iW):
			offset = world.getOffset([x,y])
			if peakStartProbability > 0:
				#collect peaks
				#check if land area is 99 empty
				if world[x,y] == PlotTypes.PLOT_LAND and world[x-1,y] == PlotTypes.PLOT_LAND and world[x+1,y] == PlotTypes.PLOT_LAND and world[x,y-1] == PlotTypes.PLOT_LAND and world[x,y+1] == PlotTypes.PLOT_LAND and (not world[x+1,y+1] == PlotTypes.PLOT_PEAK) and (not world[x-1,y+1] == PlotTypes.PLOT_PEAK) and (not world[x-1,y-1] == PlotTypes.PLOT_PEAK) and (not world[x+1,y-1] == PlotTypes.PLOT_PEAK):
					if getRand(dice) < peakStartProbability:
						world._data[offset] = PlotTypes.PLOT_PEAK
						if peakGrowProbability > 0: #if we want peeks to grow
							peakArray[offset] = [x,y,1]
			if hillStartProbability > 0:
				#collect hills
				#check if land area is 1111 empty
				if world[x,y] == PlotTypes.PLOT_LAND and not hillIntersect(world,x,y,x,y,1):
					if getRand(dice) < hillStartProbability:
						world._data[offset] = PlotTypes.PLOT_HILLS
						if hillGrowProbability > 0: #if we want peeks to grow
							hillArray[offset] = [x,y,1]

	if peakStartProbability > 0: #if we want peeks
		# now grow peaks. this code is actually using simplified method of tileBuilder. BTW, i tried to use tileBuilder to grow
		#  peaks and hills and that worked fine, but in this way it is much more optimized and without unecessary things
		while len(peakArray) > 0:
			x, y, count = peakArray.random(dice) #get peak and remove it from array

			if world[x,y] != PlotTypes.PLOT_OCEAN and count < maxPeaksInRow: # if we are in limit
				offset1 = world.getOffset([x,y])

				#print "++ Grow peak ++"
				#print "offset: %d"%(offset1)

				# for every peak we find all possible branches
				possibleTiles = bArray() # store them here
				c = 0
				wtt = world[x-1,y]
				if not wtt == PlotTypes.PLOT_PEAK and not peakIntersect(world,x-1,y,x,y):
					possibleTiles[world.getOffset([x-1,y])] = [x-1,y]
					c += 1
				wtt = world[x-1,y-1]
				if not wtt == PlotTypes.PLOT_PEAK and not peakIntersect(world,x-1,y-1,x,y):
					possibleTiles[world.getOffset([x-1,y-1])] = [x-1,y-1]
					c += 1
				wtt = world[x,y-1]
				if not wtt == PlotTypes.PLOT_PEAK and not peakIntersect(world,x,y-1,x,y):
					possibleTiles[world.getOffset([x,y-1])] = [x,y-1]
					c += 1
				wtt = world[x+1,y-1]
				if not wtt == PlotTypes.PLOT_PEAK and not peakIntersect(world,x+1,y-1,x,y):
					possibleTiles[world.getOffset([x+1,y-1])] = [x+1,y-1]
					c += 1
				wtt = world[x+1,y]
				if not wtt == PlotTypes.PLOT_PEAK and not peakIntersect(world,x+1,y,x,y):
					possibleTiles[world.getOffset([x+1,y])] = [x+1,y]
					c += 1
				wtt = world[x+1,y+1]
				if not wtt == PlotTypes.PLOT_PEAK and not peakIntersect(world,x+1,y+1,x,y):
					possibleTiles[world.getOffset([x+1,y+1])] = [x+1,y+1]
					c += 1
				wtt = world[x,y+1]
				if not wtt == PlotTypes.PLOT_PEAK and not peakIntersect(world,x,y+1,x,y):
					possibleTiles[world.getOffset([x,y+1])] = [x,y+1]
					c += 1
				wtt = world[x-1,y+1]
				if not wtt == PlotTypes.PLOT_PEAK and not peakIntersect(world,x-1,y+1,x,y):
					possibleTiles[world.getOffset([x-1,y+1])] = [x-1,y+1]
					c += 1

				tile = possibleTiles.random(dice) #take one tile from possible tiles

				if tile is not None and getRand(dice) < peakGrowProbability:
					ax, ay = tile
					world[ax,ay] = PlotTypes.PLOT_PEAK # add it
					peakArray[world.getOffset([ax,ay])] = [ax,ay,count + 1] # add it to peak array to make it continue group grow

	#now grow hills, the same thing as peaks, only this time we check for peaks when growing which obviosly already exist :)
	if hillStartProbability > 0: #if we want peeks
		while len(hillArray) > 0:
			x, y, count = hillArray.random(dice) #get peak and remove it from array
			if world[x,y] != PlotTypes.PLOT_OCEAN and count < maxHillsInRow: # if we are in limit
				offset1 = world.getOffset([x,y])
				# for every hill we find all possible branches
				possibleTiles = bArray() # store them here
				c = 0
				separation = 1
				wtt = world[x-1,y]
				if wtt == PlotTypes.PLOT_LAND and not hillIntersect(world,x-1,y,x,y,separation):
					possibleTiles[world.getOffset([x-1,y])] = [x-1,y]
					c += 1
				wtt = world[x-1,y-1]
				if wtt == PlotTypes.PLOT_LAND and not hillIntersect(world,x-1,y-1,x,y,separation):
					possibleTiles[world.getOffset([x-1,y-1])] = [x-1,y-1]
					c += 1
				wtt = world[x,y-1]
				if wtt == PlotTypes.PLOT_LAND and not hillIntersect(world,x,y-1,x,y,separation):
					possibleTiles[world.getOffset([x,y-1])] = [x,y-1]
					c += 1
				wtt = world[x+1,y-1]
				if wtt == PlotTypes.PLOT_LAND and not hillIntersect(world,x+1,y-1,x,y,separation):
					possibleTiles[world.getOffset([x+1,y-1])] = [x+1,y-1]
					c += 1
				wtt = world[x+1,y]
				if wtt == PlotTypes.PLOT_LAND and not hillIntersect(world,x+1,y,x,y,separation):
					possibleTiles[world.getOffset([x+1,y])] = [x+1,y]
					c += 1
				wtt = world[x+1,y+1]
				if wtt == PlotTypes.PLOT_LAND and not hillIntersect(world,x+1,y+1,x,y,separation):
					possibleTiles[world.getOffset([x+1,y+1])] = [x+1,y+1]
					c += 1
				wtt = world[x,y+1]
				if wtt == PlotTypes.PLOT_LAND and not hillIntersect(world,x,y+1,x,y,separation):
					possibleTiles[world.getOffset([x,y+1])] = [x,y+1]
					c += 1
				wtt = world[x-1,y+1]
				if wtt == PlotTypes.PLOT_LAND and not hillIntersect(world,x-1,y+1,x,y,separation):
					possibleTiles[world.getOffset([x-1,y+1])] = [x-1,y+1]
					c += 1

				tile = possibleTiles.random(dice) #take one tile from possible tiles

				if tile is not None and getRand(dice) < hillGrowProbability:
					ax, ay = tile
					world[ax,ay] = PlotTypes.PLOT_HILLS # add it
					hillArray[world.getOffset([ax,ay])] = [ax,ay,count + 1] # add it to peak array to make it continue group grow

	# return our world in one-dimensional array to Firaxis to do the rest :)
	return world.get()

def getStartPositions(world, contData):
	global continentCount
	global continentSeparation

	cgc = CyGlobalContext()
	map = cgc.getMap()

	distribution = getContinentDistribution()

	dice = cgc.getGame().getMapRand() #dice is Firaxis' random number generator
	players = cgc.getGame().countCivPlayersEverAlive()
	iW = map.getGridWidth()
	iH = map.getGridHeight()

	if contData == None:
		contData = []
		for p in range(continentCount):
			contData.append(1) # create dummy array

	positionArray = []

	if distribution == "equator": #old style distribution
		p = -1
		part = float(iW) / float(continentCount * 2) #figure out x position of continent start tile
		for i in range(len(contData)):
			p += 2
			positionArray.append([int(part * p), iH / 2])
	elif distribution == "2rows":
		p = -1
		direction = 1
		part = float(iW) / float(continentCount * 2) #figure out x position of continent start tile
		for i in range(len(contData)):
			p += 2
			positionArray.append([int(part * p), int(iH / 2 * (1 + direction * (0.2 + getRand(dice) * 0.3)))])
			direction *= -1
	elif distribution == "3rows":
		p = -1
		offset = 0
		offsetchange = 1
		part = float(iW) / float(continentCount * 2) #figure out x position of continent start tile
		for i in range(len(contData)):
			p += 2
			positionArray.append([int(part * p), int(iH / 2 * (1 + offset * (0.4 + getRand(dice) * 0.2)))])
			offset += offsetchange
			if offset > 0 or offset < 0:
				offsetchange *= -1
	elif distribution == "center":
		perimeter = (continentSeparation + 5) * continentCount
		radius = perimeter / (2 * 3.141592)
		inc_angle = (360 / continentCount) * 3.1415 / 180
		angle = 0
		height_multiplier = 0.9
		width_multiplier = 1.5
		for i in range(continentCount):
			if angle >= 360 * 3.1415 / 180:
				angle -= 360 * 3.1415 / 180
			if angle == 0:
				positionArray.append([ int(iW / 2 - radius * width_multiplier), int(iH / 2) ])
			elif angle > 0 and angle < 3.1415 / 2:
				positionArray.append([ int(iW / 2 - radius * width_multiplier * math.cos(angle)), int(iH / 2 - radius * height_multiplier * math.sin(angle)) ])
			elif angle >= 3.1415 / 2 and angle < 3.1415:
				positionArray.append([ int(iW / 2 + radius * width_multiplier * math.sin(angle - 3.1415 / 2)), int(iH / 2 - radius * height_multiplier * math.cos(angle - 3.1415 / 2)) ])
			elif angle >= 3.1415 and angle < 3.1415 / 2 * 3:
				positionArray.append([ int(iW / 2 + radius * width_multiplier * math.cos(angle - 3.1415)), int(iH / 2 + radius * height_multiplier * math.sin(angle - 3.1415)) ])
			else:
				positionArray.append([ int(iW / 2 - radius * width_multiplier * math.sin(angle - 3.1415 / 2 * 3)), int(iH / 2 + radius * height_multiplier * math.cos(angle - 3.1415 / 2 * 3)) ])

			angle += inc_angle

	return positionArray

def getContinentDistribution():
	global continentCount
	global continentSeparation

	global chosenWorldType
	global worldTypes

	distributionOption = getSelValue("continent_distribution")
	if distributionOption == None:
		distributionOption = "smart-random"

	cgc = CyGlobalContext()
	map = cgc.getMap()

	if chosenWorldType == -1:
		worldInfo = worldTypes[0]
	else:
		worldInfo = worldTypes[chosenWorldType]

	mapArea = worldInfo.getGridHeight() * worldInfo.getGridWidth() * 16

	if distributionOption == "smart-random":
		if mapArea <= 40 * 24: #duel
			if continentSeparation < 4:
				if continentCount < 15:
					distribution = "2rows"
				else:
					distribution = "3rows"
			else:
				if continentCount < 12:
					distribution = "2rows"
				else:
					distribution = "3rows"
		elif mapArea <= 48 * 28: #tiny
			if continentSeparation < 4:
				if continentCount < 13:
					distribution = "2rows"
				else:
					distribution = "3rows"
			else:
				if continentCount < 12:
					distribution = "2rows"
				else:
					distribution = "3rows"
		elif mapArea <= 52 * 32: #small
			if continentSeparation < 4:
				if continentCount < 14:
					distribution = "2rows"
				else:
					distribution = "3rows"
			else:
				if continentCount < 13:
					distribution = "2rows"
				else:
					distribution = "3rows"
		elif mapArea <= 64 * 40: #standard
			if continentSeparation < 4:
				if continentCount < 15:
					distribution = "2rows"
				else:
					distribution = "3rows"
			else:
				if continentCount < 14:
					distribution = "2rows"
				else:
					distribution = "3rows"
		elif mapArea <= 84 * 52: #large
			if continentSeparation < 4:
				if continentCount < 16:
					distribution = "2rows"
				else:
					distribution = "3rows"
			else:
				if continentCount < 15:
					distribution = "2rows"
				else:
					distribution = "3rows"
		elif mapArea <= 128 * 80: #huge
			if continentSeparation < 4:
				if continentCount < 16:
					distribution = "2rows"
				else:
					distribution = "3rows"
			else:
				if continentCount < 3:
					distribution = "center"
				elif continentCount < 15:
					distribution = "2rows"
				else:
					distribution = "3rows"
		else: #else
			if continentSeparation < 4:
				if continentCount < 17:
					distribution = "2rows"
				else:
					distribution = "3rows"
			else:
				if continentCount < 15:
					distribution = "2rows"
				else:
					distribution = "3rows"
	else:
		distribution = distributionOption

	return distribution

#function to determine if tile x,y intersects with something, ignoring ox,oy tile
def peakIntersect(world, x, y, ox, oy):
	ax = x
	ay = y + 1
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world._data[offset] == PlotTypes.PLOT_PEAK or world._data[offset] == PlotTypes.PLOT_OCEAN:
			return True
	ax = x + 1
	ay = y
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world._data[offset] == PlotTypes.PLOT_PEAK or world._data[offset] == PlotTypes.PLOT_OCEAN:
			return True
	ax = x
	ay = y - 1
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world._data[offset] == PlotTypes.PLOT_PEAK or world._data[offset] == PlotTypes.PLOT_OCEAN:
			return True
	ax = x - 1
	ay = y
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world._data[offset] == PlotTypes.PLOT_PEAK or world._data[offset] == PlotTypes.PLOT_OCEAN:
			return True

	ax = x - 1
	ay = y - 1
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world._data[offset] == PlotTypes.PLOT_PEAK:
			return True
	ax = x - 1
	ay = y + 1
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world._data[offset] == PlotTypes.PLOT_PEAK:
			return True
	ax = x + 1
	ay = y + 1
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world._data[offset] == PlotTypes.PLOT_PEAK:
			return True
	ax = x + 1
	ay = y - 1
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world._data[offset] == PlotTypes.PLOT_PEAK:
			return True

	return False

#function to determine if tile x,y intersects with something in distance <separation>, ignoring ox,oy tile
def hillIntersect(world, x, y, ox, oy, separation):
	ax = x - 1
	ay = y
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world[ax, ay] == PlotTypes.PLOT_HILLS:
			return True
	ax = x + 1
	ay = y
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world[ax, ay] == PlotTypes.PLOT_HILLS:
			return True
	ax = x
	ay = y - 1
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world[ax, ay] == PlotTypes.PLOT_HILLS:
			return True
	ax = x
	ay = y + 1
	if ax != ox and ay != oy:
		offset = world.getOffset([ax, ay])
		if world[ax, ay] == PlotTypes.PLOT_HILLS:
			return True
	return False

#------------------------------------------------------
"""GAME	ELEMENT	PLACEMENT FUNCTIONS"""
#------------------------------------------------------

def beforeInit():
	"Called before the map is initialized"

def beforeGeneration():
	"Called before the map generation starts"

# default implementation
def addRivers():
	"Should add all desired rivers to the map."
	CyPythonMgr().allowDefaultImpl()

# default implementation
def getRiverStartCardinalDirection(argsList):
	pPlot = argsList[0]
	"Returns the cardinal direction	of the first river segment."
	CyPythonMgr().allowDefaultImpl()

# default implementation
def getRiverAltitude(argsList):
	pPlot = argsList[0]
	"Returns the altitude level of pPlot (rivers will flow to the lowest point)."
	CyPythonMgr().allowDefaultImpl()

# default implementation
def addLakes():
	"Can override to control where lakes are added on the map"
	CyPythonMgr().allowDefaultImpl()

#Step 4: add features to your terrain
#this is responsible for things like oasis, ice floes on the ocean, floodplains
def addFeatures():
	collectGarbage()

	global world

	global climateVariation
	global climateTemperature

	global climateHumidity

	global allowLandNearIce
	global poleSeparation

	climateHumidity = getSelValue("humidity")
	if climateHumidity is None:
		climateHumidity = 0.5
	floodPlains = getSelValue("flood_plains")
	if floodPlains is None:
		floodPlains = -1

	iceOnWater = 0.5

	global continentSeparation

	cgc = CyGlobalContext()

	terrainDesert = cgc.getInfoTypeForString("TERRAIN_DESERT")
	terrainPlains = cgc.getInfoTypeForString("TERRAIN_PLAINS")
	terrainGrass = cgc.getInfoTypeForString("TERRAIN_GRASSLAND")
	terrainTundra = cgc.getInfoTypeForString("TERRAIN_TAIGA")
	terrainSnow = cgc.getInfoTypeForString("TERRAIN_ICE")
# Rise of Mankind 2.82 start
	terrainMarsh = cgc.getInfoTypeForString("TERRAIN_MARSH")
# Rise of Mankind 2.82 end

#C2C
	terrainSaltFlats = cgc.getInfoTypeForString("TERRAIN_SALT_FLATS")
	terrainDunes = cgc.getInfoTypeForString("TERRAIN_DUNES")
	terrainScrub = cgc.getInfoTypeForString("TERRAIN_SCRUB")
	terrainRocky = cgc.getInfoTypeForString("TERRAIN_ROCKY")
	terrainBarren = cgc.getInfoTypeForString("TERRAIN_BARREN")
	terrainPermafrost = cgc.getInfoTypeForString("TERRAIN_TUNDRA")
	terrainLush = cgc.getInfoTypeForString("TERRAIN_LUSH")
	terrainMuddy = cgc.getInfoTypeForString("TERRAIN_MUDDY")

	featNone = -1
	featIce = cgc.getInfoTypeForString("FEATURE_ICE")
	featJungle = cgc.getInfoTypeForString("FEATURE_JUNGLE")
	featOasis = cgc.getInfoTypeForString("FEATURE_OASIS")
	featFlood = cgc.getInfoTypeForString("FEATURE_FLOOD_PLAINS")
	featForest = cgc.getInfoTypeForString("FEATURE_FOREST")
# Rise of Mankind 2.82 start
	featSwamp = cgc.getInfoTypeForString("FEATURE_SWAMP")
# Rise of Mankind 2.82 end

	map = cgc.getMap()
	dice = cgc.getGame().getMapRand()
	players = cgc.getGame().countCivPlayersEverAlive()
	iW = map.getGridWidth()
	iH = map.getGridHeight()
# Rise of Mankind 2.82 start
	stormChance = 0.08
# Rise of Mankind 2.82 end
	# Terrain Type generation
	seedList = bArray()
	for y in range(iH):
		randomTerrain = probabilityArray()

		tileTemperature = getTileTemperature(y,iH)
		#print "tile temperature",tileTemperature
		if tileTemperature > 30:
			randomTerrain[7 * (1.5 - climateHumidity)] = terrainDesert
		if tileTemperature > 15 and tileTemperature < 39:
			randomTerrain[7] = terrainPlains
		if tileTemperature > 4 and tileTemperature < 30:
			randomTerrain[7 * (climateHumidity + 0.5)] = terrainGrass
		if tileTemperature > -2 and tileTemperature < 25:
			randomTerrain[7] = terrainPlains
# Rise of Mankind 2.82 start
		if tileTemperature > -5 and tileTemperature < 18:
			randomTerrain[10] = terrainMarsh
# Rise of Mankind 2.82 end
		if tileTemperature > -10 and tileTemperature < 10:
			randomTerrain[7] = terrainTundra
		if tileTemperature < -30:
			randomTerrain[15 * (climateHumidity / 2 + 0.75)] = terrainSnow
		elif tileTemperature < 0:
			randomTerrain[7 * (climateHumidity / 2 + 0.75)] = terrainSnow
		if tileTemperature < -10 and tileTemperature >= -20:
			randomTerrain[15 * (climateHumidity / 2 + 0.75)] = terrainPermafrost
		elif tileTemperature < 0 and tileTemperature >= -10:
			randomTerrain[7 * (climateHumidity / 2 + 0.75)] = terrainPermafrost

		for x in range(iW):

			plot = map.sPlot(x,y)
			if plot.isWater():
				if allowLandNearIce:
					if y < poleSeparation:
						prob = (poleSeparation - y) / float(poleSeparation)
						if getRand(dice) < prob:
							plot.setFeatureType(featIce,-1)
					elif y > iH - poleSeparation - 1:
						prob = (poleSeparation - (iH - y - 1)) / float(poleSeparation)
						if getRand(dice) < prob:
							plot.setFeatureType(featIce,-1)
				elif not plot.isCoastal():
					if y < poleSeparation:
						prob = (poleSeparation - y) / float(poleSeparation)
						if getRand(dice) < prob:
							plot.setFeatureType(featIce,-1)
					elif y > iH - poleSeparation - 1:
						prob = (poleSeparation - (iH - y - 1)) / float(poleSeparation)
						if getRand(dice) < prob:
							plot.setFeatureType(featIce,-1)
				elif y > poleSeparation and y < iH - poleSeparation - 1:
					if tileTemperature < -40:
						if getRand(dice) < iceOnWater * 2:
							plot.setFeatureType(featIce,-1)
					elif tileTemperature < -25:
						if getRand(dice) < iceOnWater:
							plot.setFeatureType(featIce,-1)
					elif tileTemperature < -10:
						if getRand(dice) < iceOnWater / 2:
							plot.setFeatureType(featIce,-1)
					elif tileTemperature < -5:
						if getRand(dice) < iceOnWater / 3:
							plot.setFeatureType(featIce,-1)
					elif tileTemperature < 0:
						if getRand(dice) < iceOnWater / 4:
							plot.setFeatureType(featIce,-1)
			else:
				if plot.isPeak() or plot.isFreshWater() or not plot.isWater() and plot.isCoastal():
					seedList[world.getOffset([x,y])] = [x,y,1,None] # x,y,distance from river,FeatureType
				plot.setTerrainType(randomTerrain.randomItem(dice),False,False)

	# Feature generation
	while len(seedList) > 0:
		x, y, distanceFromRiver, feature = seedList.random(dice)
		plot = map.sPlot(x,y)
		terrain = plot.getTerrainType()
		tileTemperature = getTileTemperature(y,iH)

		if not plot.isPeak():
			if feature == None and plot.getFeatureType() == featNone:
				randomFeat = probabilityArray()

				randomFeat[distanceFromRiver * 14 * (1 - climateHumidity)] = None # how many tiles have no forest and no jungle
				if tileTemperature > 28:
					randomFeat[(distanceFromRiver / 3 + (tileTemperature - 22) / 2) * (1 - (climateHumidity * 0.5) - 0.5)] = None

				if plot.isFreshWater() and not plot.isRiverSide():
					if terrain == terrainDesert:
						if getRand(dice) > 0.6:
							if getRand(dice) > 0.6:
								plot.setTerrainType(terrainGrass,False,False)
							else:
								plot.setTerrainType(terrainPlains,False,False)
					elif terrain == terrainPlains:
						if getRand(dice) > 0.7:
							plot.setTerrainType(terrainGrass,False,False)
					terrain = plot.getTerrainType()

				if not plot.isWater() and plot.isCoastal() and not plot.isFreshWater():
					if terrain == terrainDesert:
						if getRand(dice) > 0.3:
								plot.setTerrainType(terrainPlains,False,False)
					elif terrain == terrainPlains:
						if tileTemperature > 5:
							if getRand(dice) > 0.3:
									plot.setTerrainType(terrainGrass,False,False)
					elif terrain == terrainTundra:
						if tileTemperature > 5:
							if getRand(dice) > 0.6:
									plot.setTerrainType(terrainGrass,False,False)
					terrain = plot.getTerrainType()

				# Features based on initial terrain
				if terrain == terrainDesert:
					randomFeat[1] = featJungle #more jungle on sand
				elif terrain == terrainGrass:
					if tileTemperature > 20:
						randomFeat[1] = featJungle
					randomFeat[3] = featForest #most forest on grass
				elif terrain == terrainPlains:
					if tileTemperature > 25:
						randomFeat[2] = featJungle
					randomFeat[3] = featForest
# Rise of Mankind 2.82 start
				elif terrain == terrainMarsh:
					randomFeat[3] = featSwamp #some swamp in marsh
# Rise of Mankind 2.82 end
				elif terrain == terrainTundra:
					randomFeat[3] = featForest #more forest in tundra
				elif terrain == terrainPermafrost:
					randomFeat[3] = featForest #some forest on snow
				if terrain != terrainTundra and terrain != terrainPermafrost and terrain != terrainSnow: #no jungle on these
					if tileTemperature > 30:
						randomFeat[1] = featJungle
						if climateHumidity > 0.8:
							randomFeat[1] = featJungle

				# Floods:
				if floodPlains < 0: #standard generation
					if (not plot.isHills()) and plot.isRiverSide(): #no floods on hills
						if getRand(dice) > -1 * floodPlains:
							if terrain == terrainDesert:
								if tileTemperature > 28:
									randomFeat[60 + (tileTemperature - 22) * (1 - climateHumidity)] = featFlood
								else:
									randomFeat[60 + 5 * (1 - climateHumidity)] = featFlood
				else:
					if not plot.isHills(): #no floods on hills
						randomFeat[floodPlains] = featFlood
						if plot.isFreshWater(): #make more floods near lakes and rivers
							#if terrain == terrainTundra or terrain == terrainSnow:
								#if tileTemperature > -10:
									#randomFeat[2 * (1 - climateHumidity)] = featFlood
								#elif tileTemperature > -30:
									#randomFeat[1 * (1 - climateHumidity)] = featFlood
							if terrain == terrainDesert:
								if tileTemperature < 40:
									randomFeat[2 * (1 - climateHumidity)] = featFlood
								elif tileTemperature < 55:
									randomFeat[1 * (1 - climateHumidity)] = featFlood
							else:
								randomFeat[4 * (1 - climateHumidity)] = featFlood
						# We increase amount of flood on poor terrain, to make it more playable
						if terrain == terrainDesert:
							randomFeat[3 * (1 - climateHumidity)] = featFlood
						#elif terrain == terrainSnow:
							#randomFeat[3 * (1 - climateHumidity)] = featFlood
						#elif terrain == terrainTundra:
							#randomFeat[1 * (1 - climateHumidity)] = featFlood

				# Features based on temperature
				if tileTemperature > 40:
					randomFeat[2] = featJungle
# Rise of Mankind 2.82 start
				elif tileTemperature < 5 and tileTemperature > -10:
					randomFeat[2] = featSwamp
# Rise of Mankind 2.82 end
				elif tileTemperature > -20:
					randomFeat[2] = featForest
				else:
					randomFeat[1] = featForest

				feat = randomFeat.randomItem(dice)
				if feat != None:
					plot.setFeatureType(feat,-1)
					feature = feat

		if plot.isPeak():
				# make this seed spread to other tiles
				tileList = getTilesAroundDistance(x,y,2)
				posList = probabilityArray()
				score = 80
				for tile in tileList:
					tx,ty = tile
					tilePlot = map.sPlot(tx,ty)
					tileTerrain = tilePlot.getTerrainType()
					if tilePlot.getFeatureType() != featNone:
						score -= 20
					if tilePlot.getTerrainType() != terrainDesert:
						score -= 5
					if tilePlot.isFreshWater():
						score -= 20
					if tilePlot.isWater():
						score -= 5
					if tilePlot.isPeak():
						score += 7
					if score < 0:
						break
				if score > 0:
					print "Peak with score %d was found."%(score)
					tileList = getTilesAroundDistance(x,y,1)
					for tile in tileList:
						tx,ty = tile
						tilePlot = map.sPlot(tx,ty)
						if (not tilePlot.isPeak()) and (not tilePlot.isWater()):
							if getRand(dice) < (0.1 + climateHumidity * 0.6):
								if tileTemperature > 40:
									seedList[world.getOffset([tx,ty])] = [tx,ty,2,featJungle]
								elif tileTemperature > 35:
									if getRand(dice) > 0.6:
										seedList[world.getOffset([tx,ty])] = [tx,ty,2,featJungle]
									else:
										seedList[world.getOffset([tx,ty])] = [tx,ty,2,featForest]
								elif tileTemperature > 28:
									if getRand(dice) > 0.8:
										seedList[world.getOffset([tx,ty])] = [tx,ty,2,featJungle]
									else:
										seedList[world.getOffset([tx,ty])] = [tx,ty,2,featForest]
								elif tileTemperature > 24:
									if getRand(dice) > 0.95:
										seedList[world.getOffset([tx,ty])] = [tx,ty,2,featJungle]
									else:
										seedList[world.getOffset([tx,ty])] = [tx,ty,2,featForest]
								else:
									seedList[world.getOffset([tx,ty])] = [tx,ty,2,featForest]
		else:
			if feature != None:
				if plot.getFeatureType() == featNone:
					plot.setFeatureType(feature,-1)

				#feature, when spreads, modifies terrain type
				if feature == featJungle:
					if plot.isFreshWater():
						plot.setTerrainType(terrainGrass,False,False)
					elif terrain == terrainDesert:
						if getRand(dice) > 0.6:
							plot.setTerrainType(terrainPlains,False,False)
						else:
							plot.setTerrainType(terrainGrass,False,False)
					elif terrain == terrainPlains:
						if getRand(dice) > 0.1:
							plot.setTerrainType(terrainGrass,False,False)
					elif terrain == terrainTundra:
						plot.setFeatureType(featForest,-1)
					elif terrain == terrainPermafrost:
						plot.setFeatureType(featForest,-1)
					elif terrain == terrainSnow: #that happened once, looks weird :)
						plot.setFeatureType(featForest,-1)
				elif feature == featForest:
					if terrain == terrainDesert:
						if getRand(dice) > 0.3:
							plot.setTerrainType(terrainPlains,False,False)
						else:
							plot.setTerrainType(terrainGrass,False,False)
					elif terrain == terrainPlains:
						if getRand(dice) > 0.4:
							plot.setTerrainType(terrainGrass,False,False)
				elif feature == featFlood:
					if (not plot.isFreshWater()) and floodPlains >= 0:
						if terrain == terrainSnow:
							plot.setTerrainType(terrainTundra,False,False)
						elif terrain == terrainPermafrost:
							plot.setTerrainType(terrainTundra,False,False)

				# make this seed spread to other tiles
				tileList = getTilesAroundDistance(x,y,1)
				posList = probabilityArray()
				for tile in tileList:
					tx,ty = tile
					tilePlot = map.sPlot(tx,ty)
					tileTerrain = tilePlot.getTerrainType()
					if not tilePlot.isWater() and not tilePlot.isPeak():
						if feature == featFlood:
							if floodPlains > 0: #if this is not standart mode
								if tilePlot.getFeatureType() == featNone and tilePlot.isFlatlands():
									if tilePlot.isFreshWater():
										posList[2] = [tx,ty,distanceFromRiver + 3,feature]
									else:
										posList[1] = [tx,ty,distanceFromRiver + 3,feature]
						elif feature == featJungle:
							if tilePlot.getFeatureType() == featNone:
								if tilePlot.isFlatlands():
									posList[1] = [tx,ty,distanceFromRiver + 1,feature]
								if not isRiverCrossing(tilePlot,plot):
									if tileTerrain != terrainSnow and tileTerrain != terrainTundra:
										if tileTerrain == terrainDesert:
											posList[1] = [tx,ty,distanceFromRiver + 1,feature]
										else:
											posList[2] = [tx,ty,distanceFromRiver + 1,feature]
							if tileTerrain == terrainDesert:
								if not (tilePlot.getFeatureType() == featOasis or tilePlot.getFeatureType() == featFlood):
									if (getRand(dice) < 0.6):
										if tilePlot.isFreshWater():
											if climateHumidity > 0.5:
												if (getRand(dice) > 0.4):
													tilePlot.setTerrainType(terrainPlains,False,False)
												else:
													tilePlot.setTerrainType(plot.getTerrainType(),False,False)
											else:
												if (getRand(dice) > 0.2):
													tilePlot.setTerrainType(terrainPlains,False,False)
												else:
													tilePlot.setTerrainType(plot.getTerrainType(),False,False)
										else:
											if climateHumidity > 0.5:
												if (getRand(dice) > 0.3):
													tilePlot.setTerrainType(terrainPlains,False,False)
												else:
													tilePlot.setTerrainType(plot.getTerrainType(),False,False)
											else:
												if (getRand(dice) > 0.3):
													tilePlot.setTerrainType(terrainPlains,False,False)
							elif tileTerrain == terrainTundra:
								if plot.getTerrainType() == terrainGrass:
									tilePlot.setTerrainType(terrainPlains,False,False)
								else:
									tilePlot.setTerrainType(plot.getTerrainType(),False,False)
							elif tileTerrain == terrainSnow:
								if (getRand(dice) > 0.5):
									tilePlot.setTerrainType(terrainPlains,False,False)
								else:
									tilePlot.setTerrainType(terrainTundra,False,False)
						elif feature == featForest:
							if not (tilePlot.getFeatureType() == featOasis or tilePlot.getFeatureType() == featFlood):
								if tilePlot.getFeatureType() == featNone:
									if tilePlot.isFlatlands():
										posList[1] = [tx,ty,distanceFromRiver + 1,feature]
									if not isRiverCrossing(tilePlot,plot):
										if tileTerrain != terrainDesert:
											if tileTerrain == terrainPlains:
												posList[1] = [tx,ty,distanceFromRiver + 1,feature]
											else:
												posList[2] = [tx,ty,distanceFromRiver + 1,feature]
								if tileTerrain == terrainDesert:
									if (getRand(dice) < 0.3):
										if tilePlot.isFreshWater():
											if (getRand(dice) > 0.5):
												tilePlot.setTerrainType(terrainPlains,False,False)
											else:
												tilePlot.setTerrainType(plot.getTerrainType(),False,False)
										else:
											if (getRand(dice) > 0.3):
												tilePlot.setTerrainType(terrainPlains,False,False)
											else:
												tilePlot.setTerrainType(plot.getTerrainType(),False,False)

				posList[len(posList) / 2] = None # add some none probability
				item = posList.random(dice)
				if item != None:
					seedList[world.getOffset([item[0],item[1]])] = item

	# find best places for oasis
	possibleOasisList = bArray()
	for y in range(iH):
		for x in range(iW):
			plot = map.sPlot(x,y)
			if plot.getTerrainType() != terrainDesert or plot.isFreshWater() or plot.isHills() or plot.isPeak() or plot.isCoastal():
				continue
			score = 10
			tileList = getTilesAround(x, y)
			for tile in tileList:
				tx,ty = tile
				tilePlot = map.sPlot(tx,ty)
				tileTerrain = tilePlot.getTerrainType()
				if tilePlot.isFreshWater():
					score -= 20
				if tilePlot.isRiverSide():
					score -= 20
				if tilePlot.isPeak():
					score -= 2
				if tilePlot.getFeatureType() == featNone:
					score += 1
				elif tilePlot.getFeatureType() == featJungle:
					score -= 5
				elif tilePlot.getFeatureType() == featForest:
					score -= 3
				elif tilePlot.getFeatureType() == featFlood:
					score -= 20
				if tileTerrain == terrainDesert:
					score += 1
				elif tileTerrain == terrainPlains:
					score -= 2
				elif tileTerrain == terrainGrass:
					score -= 6
				elif tileTerrain == terrainTundra:
					score -= 20
				elif tileTerrain == terrainSnow:
					score -= 20
				if not tilePlot.hasYield():
					score += 1

			if score > 0:
				possibleOasisList[world.getOffset([x,y])] = [x, y]

	missedItems = 0
	for index in range(int(len(possibleOasisList) / 3)):
		x, y = possibleOasisList.random(dice)
		canPlace = True
		tileList = getTilesAround(x,y)
		for tile in tileList:
			tx,ty = tile
			plot = map.sPlot(tx,ty)
			if plot.getFeatureType() == featOasis:
				canPlace = False
				missedItems += 1
				break
		plot = map.sPlot(x,y)
		if canPlace:
			plot.setFeatureType(featOasis,-1)
		if missedItems > 20:
			break

	#Diversify terrain
	for y in range(iH):
		for x in range(iW):
			plot = map.sPlot(x,y)
			if not (plot.isPeak() or plot.isWater()):
				terr = plot.getTerrainType()
				if terr == terrainDesert:
					randomTerrain = probabilityArray()
					randomTerrain[2] = terrainDesert
					randomTerrain[1] = terrainSaltFlats
					randomTerrain[2] = terrainDunes
					randomTerrain[2] = terrainScrub
					plot.setTerrainType(randomTerrain.randomItem(dice),False,False)
				elif terr == terrainPlains:
					randomTerrain = probabilityArray()
					randomTerrain[2] = terrainPlains
					randomTerrain[1] = terrainBarren
					randomTerrain[1] = terrainRocky
					plot.setTerrainType(randomTerrain.randomItem(dice),False,False)
				elif terr == terrainGrass:
					randomTerrain = probabilityArray()
					randomTerrain[2] = terrainGrass
					randomTerrain[1] = terrainLush
					randomTerrain[1] = terrainMuddy
					plot.setTerrainType(randomTerrain.randomItem(dice),False,False)

				#Remove land features that are on wrong terrain (seems to remove some things incorrectly)
				#if plot.canHaveFeature(plot.getFeatureType()) == False:
					#plot.setFeatureType(featNone,-1)

			#Add random features from full feature list
			if plot.getFeatureType() == featNone:
				#randomFeat = probabilityArray()
				#randomFeat[10000] = featNone
				for iI in range(cgc.getNumFeatureInfos()):
					if plot.canHaveFeature(iI):
						#randomFeat[cgc.getFeatureInfo(iI).getAppearanceProbability()] = iI
						if dice.get(10000, "Add Feature PYTHON") < cgc.getFeatureInfo(iI).getAppearanceProbability():
							plot.setFeatureType(iI, -1)
				#plot.setFeatureType(randomTerrain.randomItem(dice),-1)


def isRiverCrossing(plot1,plot2):
	cgc = CyGlobalContext()

	dirNorth = cgc.getInfoTypeForString("DIRECTION_NORTH")
	dirNorthEast = cgc.getInfoTypeForString("DIRECTION_NORTHEAST")
	dirEast = cgc.getInfoTypeForString("DIRECTION_EAST")
	dirSouthEast = cgc.getInfoTypeForString("DIRECTION_SOUTHEAST")
	dirSouth = cgc.getInfoTypeForString("DIRECTION_SOUTH")
	dirSouthWest = cgc.getInfoTypeForString("DIRECTION_SOUTHWEST")
	dirWest = cgc.getInfoTypeForString("DIRECTION_WEST")
	dirNorthWest = cgc.getInfoTypeForString("DIRECTION_NORTHWEST")

	map = cgc.getMap()
	iW = map.getGridWidth()
	iH = map.getGridHeight()

	# quite nasty lines of code to determine plot direction

	x1 = plot1.getX()
	y1 = plot1.getY()
	x2 = plot2.getX()
	y2 = plot2.getY()

	if x2 >= iW:
		x2 = 0
	elif x2 < 0:
		x2 = iW - 1
	if y2 >= iH:
		y2 = 0
	elif y2 < 0:
		y2 = iH - 1

	dx = x2 - x1
	dy = y2 - y1

	direction = -1
	if dx < 0 and dy == 0:
		direction = dirWest
	elif dx < 0 and dy > 0:
		direction = dirNorthWest
	elif dx == 0 and dy > 0:
		direction = dirNorth
	elif dx > 0 and dy > 0:
		direction = dirNorthEast
	elif dx > 0 and dy == 0:
		direction = dirEast
	elif dx > 0 and dy < 0:
		direction = dirSouthEast
	elif dx == 0 and dy < 0:
		direction = dirSouth
	elif dx < 0 and dy < 0:
		direction = dirSouthWest

	if direction == -1:
		return False
	else:
		return plot1.isRiverCrossing(direction)

def getTilesAround(x,y):
	cgc = CyGlobalContext()
	map = cgc.getMap()
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	l = [[x-1,y],[x-1,y+1],[x,y+1],[x+1,y+1],[x+1,y],[x+1,y-1],[x,y-1],[x-1,y-1]]
	rem = []
	for index, item in enumerate(l):
		ax, ay = item
		if ay < 0 or ay >= iH:
			rem.append(index)
		if ax < 0:
			ax = iW - 1
			l[index] = [ax,ay]
		elif ax >= iW:
			ax = 0
			l[index] = [ax,ay]
	for index in rem:
		l.pop(index)
	return l

def getTilesAroundDistance(x,y,distance,plotTypes = None):
	cgc = CyGlobalContext()
	map = cgc.getMap()
	iW = map.getGridWidth()
	iH = map.getGridHeight()
	l = []
	if plotTypes == None:
		for by in range(distance * 2 + 1):
			for bx in range(distance * 2 + 1):
				ax = x - distance + bx
				ay = y - distance + by
				if ay >= 0 and ay < iH:
					if (not (ax == x and ay == y)):
						if ax < 0:
							ax = iW - 1
						elif ax >= iW:
							ax = 0
						l.append([ax,ay])
	else:
		for by in range(distance * 2 + 1):
			for bx in range(distance * 2 + 1):
				ax = x - distance + bx
				ay = y - distance + by
				if ay >= 0 and ay < iH:
					if (not (ax == x and ay == y)):
						if ax < 0:
							ax = iW - 1
						elif ax >= iW:
							ax = 0
						if map.plot(ax, ay).getPlotType() in plotTypes:
							l.append([ax,ay])
	return l

def initClimateSettings():
	global climateVariation
	global climateTemperature
	climateVariation = getSelValue("variation")
	if climateVariation is None:
		climateVariation = 0.4
	climateTemperature = getSelValue("temperature")
	if climateTemperature is None:
		climateTemperature = 40

def getTileTemperature(y,h):
	global climateTemperature
	global climateVariation
	lowestTemperature = ((climateTemperature + 50) * climateVariation) - 50
	th = h - 1
	half = th / 2.0
	if y > half:
		return lowestTemperature + ((climateTemperature - lowestTemperature) * (half - (y - half)) / half)
	else:
		return lowestTemperature + ((climateTemperature - lowestTemperature) * y / half)

#Step 5: add bonuses to your map
#this method puts down the tile bonuses, such as iron, whale, rice, oil, etc.
def addBonuses():
	"Can override to control where bonuses are added on	the	map"
	print "-------- Add bonuses ----------"
	global bonusGeneration

	bonusGeneration = getSelValue("bonus_generation")
	if bonusGeneration is None: #ensure we don't make something bad if wrong settings exist
		bonusGeneration = "standard"

	if bonusGeneration == "standard":
		CyPythonMgr().allowDefaultImpl()
	#CyPythonMgr().allowDefaultImpl()

#Step 5B: override addBonusType so that it can pass through
def addBonusType(argsList):
	"Can override to control where a type of bonus is added	on the map"
	"""WARNING:	If addBonuses()	is overriden then this function	will not automatically be called."""
	[iBonusType] = argsList
	CyPythonMgr().allowDefaultImpl()

def canPlaceBonusAt(argsList):
	"Can override to determine if a	bonus can be placed	at the specified plot"
	"""WARNING:	If addBonuses()	is overriden then this function	will not automatically be called."""
	pPlot =	argsList[0]
	CyPythonMgr().allowDefaultImpl()

#Step 6: place goody huts
def addGoodies():
	"Can override to control where goodies are added on	the	map"
	global generationStarted
	generationStarted = False
	CyPythonMgr().allowDefaultImpl()

def canPlaceGoodyAt(argsList):
	pPlot = argsList[0]
	"Can override to determine if a	goody can be placed	at the specified plot"
	CyPythonMgr().allowDefaultImpl()

def afterGeneration():
	"Called	after the map generation is	finished"
	CvMapGeneratorUtil.placeC2CBonuses()

#------------------------------------------------------
"""STARTING	PLOT FUNCTIONS"""
#------------------------------------------------------

def minStartingDistanceModifier():
	"Returns a percent modification	for	the	minimum	distance between players (return 50	would mean +50%)"
	return 0

def assignStartingPlots():
	"Can assign	as many	starting locations as desired"
	CyPythonMgr().allowDefaultImpl()

def findStartingArea(argsList):
	"""Can override	to return the ID of	the	area in	which the given	player should start. Return	an areaID to
	force the player to	start in a certain area, or	-1 to specify that the player should start in the best
	location, irrespective of which	area it	is in. If not overridden, the default implementation will try
	to maximize	the	size of	the	player's part of his area

	WARNING: If	findStartingPlot() is overriden	then this function will	not	automatically be called."""
	playerID = argsList[0]
	CyPythonMgr().allowDefaultImpl()

def findStartingPlot(argsList):
	"Can override to return	the	plot index at which	the	given player should	start"
	#print "-------- Find starting plot ----------"
	playerID = argsList[0]
	CyPythonMgr().allowDefaultImpl()

def normalizeStartingPlotLocations():
	"Can override to change	how	starting plots are distributed.	(note, this	function is	called after all starting locs are distributed...)"
	print "-------- Normalize starting plot locations ----------"
	CyPythonMgr().allowDefaultImpl()

def normalizeAddRiver():
	"A normalize start function	(called	after starting plots are set) -	this call adds a river."
	print "-------- Normalize add river ----------"
	if bonusGeneration == "standard":
		CyPythonMgr().allowDefaultImpl()

def normalizeRemovePeaks():
	"A normalize start function	(called	after starting plots are set) -	this call removes peaks."
	print "-------- Normalize remove peaks ----------"
	if bonusGeneration == "standard":
		CyPythonMgr().allowDefaultImpl()

def normalizeAddLakes():
	"A normalize start function	(called	after starting plots are set) -	this call adds a lake."
	print "-------- Normalize add lakes ----------"
	if bonusGeneration == "standard":
		CyPythonMgr().allowDefaultImpl()

def normalizeRemoveBadFeatures():
	"A normalize start function	(called	after starting plots are set) -	this call removes bad features."
	print "-------- Normalize remove gbad features ----------"
	CyPythonMgr().allowDefaultImpl()

def normalizeRemoveBadTerrain():
	"A normalize start function	(called	after starting plots are set) -	this call removes bad terrain."
	print "-------- Normalize remove good terrain ----------"
	if bonusGeneration == "standard":
		CyPythonMgr().allowDefaultImpl()

def normalizeAddFoodBonuses():
	"A normalize start function	(called	after starting plots are set) -	this call adds food	bonuses."
	print "-------- Normalize add food ----------"
	if bonusGeneration == "standard":
		CyPythonMgr().allowDefaultImpl()

def normalizeAddGoodTerrain():
	"A normalize start function	(called	after starting plots are set) -	this call adds good	terrain."
	print "-------- Normalize add good terrain ----------"
	if bonusGeneration == "standard":
		CyPythonMgr().allowDefaultImpl()

# base class for fair bonus, resource, etc distribution handling
class FairBuilder:
	def __init__(self,world,usedTiles):
		self.cgc = CyGlobalContext()
		self.dice = self.cgc.getGame().getMapRand()
		self.map = self.cgc.getMap()
		self.world = world
		self.edgeTiles = bArray()
		self.usedTiles = usedTiles
		self.validPlotTypes = [PlotTypes.PLOT_LAND, PlotTypes.PLOT_HILLS]

	def start(self,x,y):
		#print "--- Start grow: x=%d y=%d"%(x,y)
		distance = 0
		offset = self.world.getOffset([x,y])
		if self._grow(offset,x,y,distance):
			self.usedTiles[offset] = [offset, x, y, distance]
			self.edgeTiles[offset] = [offset, x, y, distance, self._getPossibleTiles(offset, x, y, distance)]
			return True
		else:
			return False

	def grow(self):
		tile = None
		gtile = None
		while tile == None:
			gtile = None
			tile = self.edgeTiles.randomItem(self.dice)
			if tile != None:
				offset, x, y, distance, validTiles = tile
				#print "--- Random edge tile try: x=%d y=%d"%(x,y)
				gtile = None
				while gtile == None:
					gtile = validTiles.random(self.dice)
					if gtile == None:
						#print "--- Random possible tile try failed"
						break
					else:
						aoffset, ax, ay, adistance = gtile
						#print "--- Random possible tile try: x=%d y=%d distance=%d"%(ax,ay,adistance)
						if self.usedTiles[aoffset] == None:
							#print "ok"
							break
						else:
							#print "remove"
							validTiles.remove(aoffset)
							gtile = None
				if gtile != None:
					break
				else:
					self.edgeTiles.remove(offset)
					tile = None
			else:
				#print "--- Random edge tile try failed"
				break

		if tile != None and gtile != None:
			offset, x, y, distance, validTiles = tile
			aoffset, ax, ay, adistance = gtile
			if self._grow(aoffset, ax, ay, adistance):
				#print "--- Tile Grow To: x=%d y=%d"%(ax,ay)
				self.usedTiles[aoffset] = [aoffset, ax, ay, adistance]
				possibleTiles = self._getPossibleTiles(aoffset, ax, ay, adistance)
				#print "--- Possible tile count: %d"%(len(possibleTiles))
				if len(possibleTiles) > 0:
					self.edgeTiles[aoffset] = [aoffset, ax, ay, adistance, possibleTiles]
					#print "--- Add edge tile: x=%d y=%d"%(ax,ay)
				tilesAround = getTilesAround(ax,ay)
				for rmtile in tilesAround:
					rmx, rmy = rmtile
					rmoffset = self.world.getOffset([rmx,rmy])
					edgeTile = self.edgeTiles[rmoffset]
					if edgeTile != None:
						if self._isEdgeTileExpired(edgeTile,aoffset):
							#print "--- Remove edge tile: x=%d y=%d"%(rmx,rmy)
							self.edgeTiles.remove(rmoffset)
			else:
				return False
		else:
			#print "Fair builder failed"
			return False
		return True

	def _isEdgeTileExpired(self,edgeTile,newOffset):
		offset, x, y, distance, validTiles = edgeTile
		if validTiles[newOffset] != None:
			validTiles.remove(newOffset)
		if len(validTiles) == 0:
			return True
		return False

	def _getPossibleTiles(self,offset,x,y,distance):
		tilesAround = getTilesAround(x,y)
		validTiles = bArray()
		for atile in tilesAround:
			ax, ay = atile
			aoffset = self.world.getOffset([ax,ay])
			if self.usedTiles[aoffset] == None:
				if self._canGrow(offset, x, y, aoffset, ax, ay, distance + 1):
					validTiles[aoffset] = [aoffset, ax, ay, distance + 1]
		return validTiles

	def _canGrow(self,originalOffset,ox,oy,offset,x,y,distance):
		plot = self.map.plotByIndex(offset)
		plotType = plot.getPlotType()
		if plotType in self.validPlotTypes:
			return True
		return False

	def _grow(self,offset,x,y,distance):
		return True

class BonusBuilder(FairBuilder):
	def __init__(self,world,usedTiles,usedCoastTiles):
		FairBuilder.__init__(self,world,usedTiles)
		self.usedCoastTiles = usedCoastTiles

		# several arrays to store tile lists
		self.allBuilderTiles = bArray()
		self.coastTiles = bArray()
		self.landTilesWithoutBonuses = bArray()
		self.coastTilesWithoutBonuses = bArray()

		# player area distance
		self.maxDistance = 0
		# coast distance from land
		self.coastDistance = 2

		# walkable plot types
		self.validPlotTypes = [PlotTypes.PLOT_LAND, PlotTypes.PLOT_HILLS]

	def _grow(self,offset,x,y,distance):
		if distance > self.maxDistance:
			self.maxDistance = distance
		plot = self.map.plotByIndex(offset)
		self.allBuilderTiles[offset] = [offset, x, y, distance]
		self.landTilesWithoutBonuses[offset] = [offset, x, y, distance]
		tiles = getTilesAroundDistance(x,y,self.coastDistance,[PlotTypes.PLOT_OCEAN])
		for tile in tiles:
			ax, ay = tile
			aoffset = self.world.getOffset([ax,ay])
			if self.usedCoastTiles[aoffset] == None:
				self.usedCoastTiles[aoffset] = [aoffset]
				self.coastTiles[aoffset] = [aoffset, ax, ay, distance + 1]
				self.coastTilesWithoutBonuses[aoffset] = [aoffset, ax, ay, distance + 1]
		#print "--- Grow OK: x=%d y=%d"%(x,y)
		return True

def normalizeAddExtras():
	"A normalize start function (called after starting plots are set) - this call adds extra features and bonuses."
	print "-------- Normalize extras ----------"
	collectGarbage()

	global bonusGeneration

	if bonusGeneration == "standard":
		CyPythonMgr().allowDefaultImpl()
	else:
		cgc = CyGlobalContext()

		terrainGrass = cgc.getInfoTypeForString("TERRAIN_GRASSLAND")
		terrainPlains = cgc.getInfoTypeForString("TERRAIN_PLAINS")
		terrainDesert = cgc.getInfoTypeForString("TERRAIN_DESERT")
		terrainTundra = cgc.getInfoTypeForString("TERRAIN_TAIGA")
		terrainSnow = cgc.getInfoTypeForString("TERRAIN_ICE")
		terrainCoast = cgc.getInfoTypeForString("TERRAIN_COAST")
		terrainOcean = cgc.getInfoTypeForString("TERRAIN_OCEAN")

		terrainWater = [terrainCoast, terrainOcean]
		terrainLand = [terrainGrass, terrainPlains, terrainDesert, terrainTundra, terrainSnow]

		global continentCount

		map = cgc.getMap()

		dice = cgc.getGame().getMapRand()

		iW = map.getGridWidth()
		iH = map.getGridHeight()

		# world = worldArray([iW,iH],PlotTypes.PLOT_OCEAN)
		# in our case we use existing world, but only for index and wraping calculations
		global world

		bonusTypes = enumeratePlaceableBonusTypes()
		bonusTypesOcean = []
		bonusTypesLand = []
		for index in range(len(bonusTypes)):
			bonusInt = bonusTypes._data[index][0]
			bonusInfo = bonusTypes._data[index][1][1]
			bWater = False
			for terrain in terrainWater:
				if bonusInfo.isTerrain(terrain):
					bWater = True
					break
			if bWater:
				bonusTypesOcean.append([bonusInt,bonusInfo])
			bLand = False
			for terrain in terrainLand:
				if bonusInfo.isTerrain(terrain):
					bLand = True
					break
			if bLand:
				bonusTypesLand.append([bonusInt,bonusInfo])
		bonusTypesOcean.sort(lambda x, y: cmp(x[1].getPlacementOrder(), y[1].getPlacementOrder()))
		bonusTypesLand.sort(lambda x, y: cmp(x[1].getPlacementOrder(), y[1].getPlacementOrder()))
		print "bonusTypesOcean:",bonusTypesOcean
		print "bonusTypesLand:",bonusTypesLand

		# we use BonusBuilder class to get all land and coast tiles player can reach without sailing
		# it is done using randomly calculated aproximate traveling paths, but produces generally accurate result
		# also, we get distance to each tile, so we can use this information to distribute bonus well

		bonusBuilders = []
		usedTiles = bArray()
		usedCoastTiles = bArray()

		for pIndex in range(cgc.getMAX_PC_PLAYERS()):
			player = cgc.getPlayer(pIndex)
			if player.isAlive():
				plot = player.getStartingPlot()
				px = plot.getX()
				py = plot.getY()
				builder = BonusBuilder(world, usedTiles, usedCoastTiles)
				bonusBuilders.append(builder)
				builder.start(px,py)

		grows = True
		maxSize = iW * iH
		size = 0
		while grows and size < maxSize:
			for builder in bonusBuilders:
				if builder.grow():
					size += 1
				else:
					grows = False
		if size >= maxSize:
			print "WARNING: size limit exceeded!"

		for index in range(cgc.getNumBonusInfos()):
			bonusInfo = cgc.getBonusInfo(index)

		featIce = cgc.getInfoTypeForString("FEATURE_ICE")
		featOasis = cgc.getInfoTypeForString("FEATURE_OASIS")

		for item in bonusTypesLand:
			bonusInt, bonusInfo = item
			usedTiles = bArray()
			playerResCount = []
			for index in range(len(bonusBuilders)):
				playerResCount.append([index, 0])

		for index in range(len(bonusBuilders)):
			builder = bonusBuilders[index]
			landTiles = builder.allBuilderTiles
			coastTiles = builder.coastTiles
			landTilesWB = builder.landTilesWithoutBonuses
			coastTilesWB = builder.coastTilesWithoutBonuses
			maxDistance = bonusBuilders[index].maxDistance
			if maxDistance > 0:
				for tile in landTiles:
					offset, x, y, distance = tile
					relDistance = distance / float(maxDistance)
					plot = map.plotByIndex(offset)
					#plot.setTerrainType(index,False,False)
					plot.setBonusType(bonusTypes._data[index][0])
				for tile in coastTiles:
					offset, x, y, distance = tile
					relDistance = distance / float(maxDistance)
					plot = map.plotByIndex(offset)
					#plot.setFeatureType(featIce,-1)
					plot.setBonusType(bonusTypes._data[index+5][0])

def cmpByPlacementOrder(x, y):
	return x[1].getPlacementOrder() - y[0].getPlacementOrder()

def enumeratePlaceableBonusTypes():
	cgc = CyGlobalContext()
	arr = bArray()
	for i in range(cgc.getNumBonusInfos()):
		bonusInfo = cgc.getBonusInfo(i)
		if bonusInfo.getTilesPer() > 0 or bonusInfo.getPercentPerPlayer() > 0:
			arr[i] = [i, bonusInfo]
	return arr

#----------------------------------------------
"""Helpers"""
#----------------------------------------------
def getRand(dice):
	"""return random number from 0 to 0.999"""
	return (dice.get(999,"Python - EarthGen: Random dice.")) / 1000.0
# taken from SmartMap
import os
import os.path

def civFilePath():
	try:
		folder = os.path.dirname(os.path.realpath(__file__))
		folder = os.path.join(folder[:-6], "Mods\Caveman2Cosmos\UserSettings")
		return folder
	except:
		return ""

#----------------------------------------------
"""Other functions"""
#----------------------------------------------
def isAdvancedMap():
	"""
	Advanced maps only show	up in the map script pulldown on the advanced menu.
	Return 0 if	you	want your map to show up in	the	simple singleplayer	menu
	"""
	return 0
def getModPath():
	"""
	Returns	the	path of	the	mod	this map is	associated with
	"""
	return ""
def isClimateMap():
	"""Uses the Climate options"""
	return 0

def isSeaLevelMap():
	"""Uses the Sea Level options"""
	return 0

def getNumHiddenCustomMapOptions():
	return 0

def getNumCustomMapOptions():
	loadMapOptionDefaults()
	return len(selection_names_and_values)

def getCustomMapOptionName(argsList):
	#print "getCustomMapOptionName",argsList
	index = argsList[0]
	return unicode(selection_titles[index][0])

def getNumCustomMapOptionValues(argsList):
	#print "getNumCustomMapOptionValues",argsList
	index = argsList[0]
	return len(selection_names_and_values[index])

def getClosestMapSizeName(width, height):
	cgc = CyGlobalContext()
	area = width * height / 16
	for i in range(cgc.getNumWorldInfos()):
		worldInfo = cgc.getWorldInfo(i)
		if worldInfo.getGridHeight() * worldInfo.getGridWidth() == area:
			return " (" + worldInfo.getText() + ")"
	return ""

def getCustomMapOptionDescAt(argsList):
	#print "getCustomMapOptionDescAt",argsList
	iOption = argsList[0]
	iSelection = argsList[1]
	if selection_titles[iOption][1] == "map_size":
		w, h = selection_names_and_values[iOption][iSelection][1]
		translated_text = unicode(selection_names_and_values[iOption][iSelection][0] + getClosestMapSizeName(w, h))
	else:
		translated_text = unicode(selection_names_and_values[iOption][iSelection][0])
	return translated_text

def getCustomMapOptionDefault(argsList):
	index =	argsList[0]
	return selection_defaults[index]

def saveMapOptionDefaults():
	cymap = CyMap()
	fileName = os.path.join(civFilePath(),"planet_generator_" + VERSION + ".cfg")
	try:
		settings = open(fileName, 'w')
		smoptions = []
		index = -1
		for item in selection_titles:
			smoption = item[1]
			index += 1
			selectionIndex = int(cymap.getCustomMapOption(index))
			valueName = selection_names_and_values[index][selectionIndex][0]
			smoptions.append((smoption,valueName))

		#print "	try to store settings to",fileName
		print smoptions
		try:
			pickle.dump(smoptions, settings)
		except Exception, inst:
			print "	Pickling Error",inst,"trying to save map settings to",fileName
		settings.close()
		print "	store successful"
	except IOError:
		print "	Couldn't create ",fileName
	except EOFError:
		print "	EOF writing ",fileName
	except:
		print "	unexpected problem writing ",fileName

def loadMapOptionDefaults():
	global selection_defaults
	fileName = os.path.join(civFilePath(),"planet_generator_" + VERSION + ".cfg")
	try:
		settings = open(fileName, 'r')
		optionsWithDefault = pickle.load(settings)
		#print optionsWithDefault
		for optionIndex in range(len(optionsWithDefault)):
			option, valueName = optionsWithDefault[optionIndex]
			for selOptionIndex in range(len(selection_titles)):
				selectionTitleCaption, selectionTitleName, autoRandom = selection_titles[selOptionIndex]
				if option == selectionTitleName:
					for selectionIndex in range(len(selection_names_and_values[selOptionIndex])):
						selectionName, selectionValue, allowedRandomPick = selection_names_and_values[selOptionIndex][selectionIndex]
						if selectionName == valueName:
							selection_defaults[selOptionIndex] = selectionIndex
		#print "	loaded option from cfg:",option,value
		settings.close()
	except IOError:
		print "	Couldn't find ",fileName
	except EOFError:
		print "	Bad contents in ",fileName
	except:
		print "	unexpected problem reading",fileName

def isRandomCustomMapOption(argsList):
	return False # we use our own defaults

#----------------------------------------------
"""MAP SIZE	AND	WRAPPING FUNCTIONS"""
#----------------------------------------------

# note:	you	must override both getWrap...()	functions or neither
def getWrapX():
	"Can be	overridden to change whether the map wraps around in the X direction. Default is true"
	return True
def getWrapY():
	"Can be	overridden to change whether the map wraps around in the Y direction. Default is false"
	return False

def getTopLatitude():
	"Can be	overridden to change whether the the top latitude. Default is 90"
	initClimateSettings()
	cgc = CyGlobalContext()
	map = cgc.getMap()
	iH = map.getGridHeight()
	topLatitudeTemperature = getTileTemperature(0,iH)
	result = 30 - topLatitudeTemperature * 2
	return int(result)

def getBottomLatitude():
	"Can be	overridden to change whether the the bottom	latitude. Default is -90"
	cgc = CyGlobalContext()
	map = cgc.getMap()
	iH = map.getGridHeight()
	topLatitudeTemperature = getTileTemperature(0,iH)
	result = - (30 - topLatitudeTemperature * 2)
	return int(result)

def isBonusIgnoreLatitude():
	"Can be	overridden to determine	whether	bonus generation ignores latitude. Default is false"
	CyPythonMgr().allowDefaultImpl()

def collectGarbage():
	print "Collect python garbage"
	try:
		gc.collect()
	except:
		print "Unexpected error during garbage collection"
