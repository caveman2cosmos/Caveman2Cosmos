#	FILE:	   MapScriptTools.py  v1.00
#	AUTHOR:  Temudjin (2009-10)
#	PURPOSE: Provide tools to facilitate map-making and adapting existing maps.
#  ====================================================================================================

#	MapScriptTools provides functions and classes to:
#	- add Marsh terrain
#  - add Deep Ocean terrain
#  - make the map looking prettier/more realistic
#  - add special regions to the map
#  - add special features to the map
#	- replace and expand BonusBalancer (from Warlords)
#	- manipulate river creation: (starting from lakes, on islands)
#  - handle starting-positions for teams
#	- print various sorts of maps
#	- print stats about mod and map
#  - find the path for Civ4, Mod or Log files

#  -----
#	NOTES:
#
#  - Multiplayer compatibility is unknown.
#    (Seems probable though as all randomizations are centralized within the choose..() functions.)
#    Please tell me.
#
#  - MAC compatibility is unknown but improbable.
#    (I think they use Python 2.3 and sets are not known. I believe I used them only twice though.
#    Feel free to adjust the file yourself as I do not own a MAC)
#    Please tell me.
#  -----

from CvPythonExtensions import *
import CvMapGeneratorUtil as MGU

GC = CyGlobalContext()

DEBUG = False
bInitialized = False

#################################################
### Defined Class Instances
#################################################
### deepOcean = DeepOcean()
### mapPrettifier = MapPrettifier()
### marshMaker = MarshMaker()
### mapRegions = MapRegions()
### featurePlacer = FeaturePlacer()
### bonusBalancer = BonusBalancer()
### riverMaker = RiverMaker()
### teamStart = TeamStart()
### mapPrint = MapPrint()
### mapStats = MapStats()
#################################################

####################################################################################################
########## Set ModInfo Global Variables - should be called: first thing in beforeGeneration()
####################################################################################################
# get infos about the mod, print infos
def getModInfo(mapVersion=None, defLatitude=None, sMapInfo=None):
	print "[MST] getModInfo()"

	###########################
	### civ universal constants
	###########################
	global MAP, iNumPlotsX, iNumPlotsY
	MAP = CyMap()
	iNumPlotsX = MAP.getGridWidth()
	iNumPlotsY = MAP.getGridHeight()

	##############################
	### define cardinal directions
	##############################
	global ecNorth, ecEast, ecSouth, ecWest

	ecNorth = CardinalDirectionTypes.CARDINALDIRECTION_NORTH
	ecEast  = CardinalDirectionTypes.CARDINALDIRECTION_EAST
	ecSouth = CardinalDirectionTypes.CARDINALDIRECTION_SOUTH
	ecWest  = CardinalDirectionTypes.CARDINALDIRECTION_WEST

	###############################
	### define globals for terrains
	###############################
	global etOcean, etCoast, etGrass, etPlains, etDesert, etTundra, etSnow, etMarsh

	# define known terrains
	etOcean		= GC.getInfoTypeForString('TERRAIN_OCEAN')
	etCoast		= GC.getInfoTypeForString('TERRAIN_COAST')
	etDesert	= GC.getInfoTypeForString('TERRAIN_DESERT')	# FlatArid, RockyArid
	etPlains	= GC.getInfoTypeForString('TERRAIN_PLAINS')	# FlatMoist, RockyMoist
	etGrass 	= GC.getInfoTypeForString('TERRAIN_GRASSLAND')	# FlatRainy, RockyRainy
	etTundra	= GC.getInfoTypeForString('TERRAIN_TAIGA')	# RockyMoist, FlatPolar
	etSnow		= GC.getInfoTypeForString('TERRAIN_ICE')	# FlatPolar, RockyPolar
	etMarsh 	= GC.getInfoTypeForString("TERRAIN_MARSH")	# FlatRainy

	###############################
	### define globals for features
	###############################
	global efIce, efForest, efJungle, efKelp

	# define known features
	efIce			= GC.getInfoTypeForString('FEATURE_ICE')
	efForest		= GC.getInfoTypeForString('FEATURE_FOREST')
	efJungle		= GC.getInfoTypeForString('FEATURE_JUNGLE')
	efKelp			= GC.getInfoTypeForString('FEATURE_KELP') # coast

	################################
	### define globals for some boni
	################################
	global ebBauxite, ebCopper, ebGold, ebSilver, ebIron, ebOil, ebSulphur, ebUranium, ebCoal,\
		ebClam, ebFish, ebCrab, ebShrimp, ebHorse, ebBanana, ebCow, ebDeer, ebFur, ebMarble, ebSheep

	# strategic boni

	ebBauxite         = GC.getInfoTypeForString('BONUS_BAUXITE_ORE')
	ebCopper          = GC.getInfoTypeForString('BONUS_COPPER_ORE')
	ebHorse           = GC.getInfoTypeForString('BONUS_HORSE')
	ebIron            = GC.getInfoTypeForString('BONUS_IRON_ORE')
	ebOil             = GC.getInfoTypeForString('BONUS_OIL')
	ebSulphur         = GC.getInfoTypeForString('BONUS_SULPHUR')
	ebUranium         = GC.getInfoTypeForString('BONUS_URANIUM')
	ebClam            = GC.getInfoTypeForString('BONUS_CLAM')			# water
	ebCrab            = GC.getInfoTypeForString('BONUS_CRAB')			# water
	ebFish            = GC.getInfoTypeForString('BONUS_FISH')			# water
	ebShrimp          = GC.getInfoTypeForString('BONUS_SHRIMP')			# water
	ebCoal            = GC.getInfoTypeForString('BONUS_COAL')
	ebGold            = GC.getInfoTypeForString('BONUS_GOLD_ORE')			# plains, desert
	ebSilver          = GC.getInfoTypeForString('BONUS_SILVER_ORE')			# tundra, snow
	ebBanana          = GC.getInfoTypeForString('BONUS_BANANA')			# grass, jungle
	ebCow             = GC.getInfoTypeForString('BONUS_COW')			# grass, plains
	ebDeer            = GC.getInfoTypeForString('BONUS_DEER')			# tundra, forest
	ebFur             = GC.getInfoTypeForString('BONUS_BEAVERS')			# tundra, snow, forest
	ebMarble          = GC.getInfoTypeForString('BONUS_MARBLE')			# plains, tundra, snow
	ebSheep           = GC.getInfoTypeForString('BONUS_SHEEP')			# grass, plains

	###################################
	### define globals for improvements
	###################################
	global eiCityRuins, eiGoody, eiFarm, eiMine, eiWindmill

	# define known improvements
	eiCityRuins	= GC.getInfoTypeForString('IMPROVEMENT_CITY_RUINS')
	eiGoody		= GC.getInfoTypeForString('IMPROVEMENT_GOODY_HUT')
	eiFarm		= GC.getInfoTypeForString('IMPROVEMENT_FARM')
	eiMine		= GC.getInfoTypeForString('IMPROVEMENT_MINE')
	eiWindmill	= GC.getInfoTypeForString('IMPROVEMENT_WINDMILL')

	##############################
	### Not available at init time
	##############################
	global bInitialized
	if not bInitialized:
		bInitialized = True
	else:
		global sClimateType, sSeaType, bTeams, mapGetLatitude
		sprint = ""
		#######################
		### retrieve parameters
		#######################
		# defLatitude should evaluate to a value between 0 .. 90, using y and/or x
		sMapName = MAP.getMapScriptName()
		if not mapVersion==None:
			sMapName += " " + mapVersion
		if defLatitude==None:
			mapGetLatitude = "noPolesGetLatitude(x,y)"	# default - gives value between 0..90
		else:
			mapGetLatitude = defLatitude				# shoud give value between 0..90 but negative is ok
		# adjust sMapInfo for Python Error with percent chars
		if sMapInfo == None:
			mapInfo = ""
		else:
			mapInfo = str(sMapInfo)
			mapInfo = mapInfo.replace("%", "%%")

		sprint += "[MST] " + "################################################################### MapScriptTools:getModInfo() ### \n"
		sprint += "[MST] " + "Initialization Parameters: - %s, - %s \n\n" % (sMapName, mapGetLatitude)

		###############################
		### user selected map constants
		###############################
		GAME = GC.getGame()
		sClimateType = GC.getClimateInfo(MAP.getClimate()).getType()
		sSeaType = GC.getSeaLevelInfo(MAP.getSeaLevel()).getType()

		bTeams = teamStart.getTeams() # check if there are teams

		########################
		### print map & mod info
		########################
		sprint += "[MST] " + "============================== \n"
		sprint += "[MST] " + "MapScriptName:           %s \n\n" % (sMapName)
		sprint += "[MST] " + "Players/MaxPlayers:      %i / %i \n" % (GAME.countCivPlayersEverAlive(), GC.getMAX_PC_PLAYERS())
		sprint += "[MST] " + "Difficulty Level:        %s \n" % (GC.getHandicapInfo(GAME.getHandicapType()).getType()[9:].capitalize())
		if bTeams:
			sprint += "[MST] " + "- Teams:                 %r \n" % ( teamStart.teamDict )
			humDict = {}
			for k in teamStart.humanDict: humDict[k] = iif(teamStart.humanDict[k]==2,'True',iif(teamStart.humanDict[k]==1,'Some','False'))
			sprint += "[MST] " + "- Team-Humanity:         %r \n" % (humDict)
		sprint += "[MST] " + "WorldArea: Size          %i x %i \n"  % (iNumPlotsX, iNumPlotsY)
		sprint += "[MST] " + "Mapsize:                 %s Map - %i Plots \n" % (GC.getWorldInfo(MAP.getWorldSize()).getType()[10:].capitalize(), MAP.numPlots())
		sprint += "[MST] " + "GameSpeed:               %s Speed - %i Turns, %i Years \n" % (GC.getGameSpeedInfo(GAME.getGameSpeedType()).getType()[10:].capitalize(), GAME.getMaxTurns(), GAME.getTurnYear(GAME.getMaxTurns()) - GAME.getTurnYear(0))
		sprint += "[MST] " + "Era:                     %s Era \n" % (GC.getEraInfo(GAME.getStartEra()).getType()[4:].capitalize())
		sprint += "[MST] " + "Climate, Sealevel:       %s, %s Seas \n" % (sClimateType[8:].capitalize(), sSeaType[9:].capitalize())
		neg = (evalLatitude(MAP.plot(iNumPlotsX-1,iNumPlotsY-1)) > evalLatitude(MAP.plot(iNumPlotsX-2,iNumPlotsY-2)))
		sprint += "[MST] " + "Latitude: Max,Min        %i, %s%i \n" % ( evalLatitude( MAP.plot(0,0) ), iif(neg,"-",""), evalLatitude( MAP.plot(iNumPlotsX-1,iNumPlotsY-1) ) )
		if sMapInfo != None:
			sprint += "[MST] " + "------------------------------ \n"
			sprint += mapInfo
		sprint += "[MST] " + "============================== \n"
		print sprint
		print "[MST] " + "################################################################### MapScriptTools:getModInfo() ###"

		#####################################
		### initialize classes (where needed)
		#####################################
		marshMaker.initialize()
		mapRegions.initialize()
		mapPrint.initialize()


#######################################################################
########## ModInfo Functions
#######################################################################
# latitude = evalLatitude( y, bDegrees=True )
# iLat = noPolesGetLatitude( x, y )
# bool = isBonus( sInfo )
#######################################################################

# evaluate code given to ModInfo, to find latitude for plot
# - use noPolesGetLatitude() if evaluation fails
# - mapGetLatitude should evaluate to a value between 0 .. 90, using y and/or x
def evalLatitude( plot, bDegrees=True ):
	if not plot:
		return 0

	x = plot.getX()
	y = plot.getY()

	iLat = abs( eval( mapGetLatitude ) )		# should give value between 0..90
	if iLat > 90:
		iLat = noPolesGetLatitude(x,y)
		fLat = iLat / 90.0
		print "WARNING!: evalLatitude() outside range! Latitude(%3i,%2i) = %2i, %6.3f - '%s'" % (x,y,iLat,fLat,mapGetLatitude)
		return iif(bDegrees, iLat, fLat)


	fLat = iLat / 90.0
	iLat = int(round(iLat))
	return iif(bDegrees, iLat, fLat)

# get latitude 0..90 for given x,y coordinates
# - uses floating-point for precision
#   eually distributed, does not include poles or equator: like (75,45,15,15,45,75)
#   (actually I like this one best, so it's the default)
def noPolesGetLatitude(x, y):
	iDiff = CyMap().getTopLatitude() - CyMap().getBottomLatitude()
	hi = float( CyMap().getGridHeight() )
	la = float( y )
	if (not CyMap().isWrapX()) and CyMap().isWrapY():
		hi = float( CyMap().getGridWidth() )
		la = float( x )
	iLat = ( 2 * la + 1 ) * iDiff / ( 2 * hi ) + CyMap().getBottomLatitude()
	return abs( int( round( iLat ) ) )


# test if bonus exists in mod
def isBonus(sInfo):
	if not (sInfo[0:5] == "BONUS"):
		sInfo = "BONUS_" + sInfo.upper()
	for i in range(GC.getNumBonusInfos()):
		info_string = GC.getBonusInfo(i).getType()
		if info_string==sInfo: return True
	return False


#####################################################################
########## Common Functions
#####################################################################
# string = space( iSpace )
# string = capWords( str )
# string = strInsert( str, inx, sins )
# ab = iif( test, a, b )
# printDict( dict, txt = None, prefix = "", cleanUp = True )
# sprint = sprintDict( dict, txt = None )
# printList( lst, txt = None, rows = 5, prefix = "", cleanUp = True )
# sprint = sprintList( lst, txt = None, rows = 5 )
# bFound = removeListElement( aList, elem )
#####################################################################
# return string of spaces
def space( iSpace ):
	return str( " "*iSpace )

# Capitalize all words in string - delimiters are either space or underscore
def capWords(s):
	a = s.split('_')
	for i in range( len(a) ):
		a[i] = (' ').join([x.capitalize() for x in a[i].split(' ')])
	return ('_').join([x for x in a])
#	return (' ').join([x.capitalize() for x in s.split(' ')])

# insert string into another before inx
def strInsert( str, inx, sins ):
	return str[:inx] + sins + str[inx:]

# exchange part of string starting at inx
def strExchange( str, inx, sex ):
	return str[:inx] + sex + str[(inx+len(sex)):]

# test if odd
def odd(n):
	return (  )

# test and pick
# Note: a and b are both evaluated, regardless of the test result
def iif( test, a, b ):
	if test: return a
	return b

# print dictionary nicely
def printDict( dict, txt = None, prefix = "", cleanUp = True ):
	if cleanUp:
		print sprintDict(dict, txt, prefix).replace("%","%%")
	else: print sprintDict(dict, txt, prefix)

# produce nice dictionary output
def sprintDict( dict, txt = None, prefix="" ):
	sprint = ""
	if not ( type( dict ) == type( {} ) ):
		sprint += prefix + "!! Not a dictionary.\n"
		sprint += prefix + "%s %r" % ( txt, dict )
		return sprint
	if len( dict.keys() ) == 0:
		return prefix + iif( txt==None, "", str(txt) ) + " {}\n"
	sprint += prefix + iif( txt==None, "", str(txt) + "\n" )
	sprint += prefix + " {\n"
	for k in dict.keys():
		sprint += prefix + "   { %r : %r }\n" % ( k, dict[k] )
	sprint += prefix + " }"
	return sprint

# print list nicely
def printList( lst, txt = None, rows = 5, prefix = "", cleanUp = True ):
	out = sprintList(lst, txt, rows, prefix)
	print iif(cleanUp, out.replace("%","%%"), out)

# produce nice list output
def sprintList( lst, txt = None, rows = 5, prefix = "" ):
	if not ( type( lst ) == type( [] ) ):
		sprint  = prefix + "!! Not a list.\n"
		sprint += prefix + "%s %r" % ( txt, lst )
		return sprint
	if len( lst ) == 0:
		return prefix + iif( txt==None, "", str(txt) ) + " []\n"
	sprint  = prefix + iif( txt==None, "", str(txt) + "\n" )
	sprint += prefix + "["
	cnt = 0
	for k in lst:
		sprint += " %r" % ( k, )
		cnt += 1
		if cnt == len(lst):
			sprint += " ]"
		elif ((cnt % rows) == 0) and ( (cnt < len(lst)-1) or (rows<=1) ):
			sprint += ",\n" + prefix + " "
		else:
			sprint += ","
	return sprint

# remove element from list without raising exception
# use [ data for data in aList if data != elem ] if all elem should be removed
def removeListElement(aList, elem):
	if aList.count(elem) > 0:
		aList.remove(elem)
		return True
	return False


#####################################################
########## Direction Functions
#####################################################
# sName = cardinalName( eCard )
# sName = directionName( eDir )
# eNewDir = addDirection( eDir, index )
# eCardList = getCardDirsFromDir( eDir )
# x,y = xyDirection( eDir )
# x,y = xyCardinalDirection( eCard )
# eDir = coordDirection( x, y, fx, fy )
#####################################################
# get names of cardinal directions
def cardinalName( eCard ):
	if eCard==CardinalDirectionTypes.CARDINALDIRECTION_NORTH: return "North"
	if eCard==CardinalDirectionTypes.CARDINALDIRECTION_EAST:  return "East"
	if eCard==CardinalDirectionTypes.CARDINALDIRECTION_SOUTH: return "South"
	if eCard==CardinalDirectionTypes.CARDINALDIRECTION_WEST:  return "West"
	return "None"

# get names of cardinal directions
def directionName( eDir ):
	if eDir==DirectionTypes.DIRECTION_NORTH:     return "North"
	if eDir==DirectionTypes.DIRECTION_NORTHEAST: return "NorthEast"
	if eDir==DirectionTypes.DIRECTION_EAST:      return "East"
	if eDir==DirectionTypes.DIRECTION_SOUTHEAST: return "SouthEast"
	if eDir==DirectionTypes.DIRECTION_SOUTH:     return "South"
	if eDir==DirectionTypes.DIRECTION_SOUTHWEST: return "SouthWest"
	if eDir==DirectionTypes.DIRECTION_WEST:      return "West"
	if eDir==DirectionTypes.DIRECTION_NORTHWEST: return "NorthWest"
	return "None"

def getOppositeCardinalDirection(dir):
	return CardinalDirectionTypes((dir + 2) % CardinalDirectionTypes.NUM_CARDINALDIRECTION_TYPES)

# add index to direction; clockwise -> positive
def addDirection( eDir, index ):
	iMult, iDir = divmod( eDir + index, 8 )
	return DirectionTypes(iDir)

# get list of the cardinal direction(s) describing a direction
def getCardDirsFromDir( eDir ):
	eCard, check = divmod( eDir, 2 )
	eCardList = [ CardinalDirectionTypes(eCard) ]
	if check == 1:
		eCard += 1
		if eCard == 4: eCard = 0
		eCardList.append( CardinalDirectionTypes(eCard) )
	return eCardList

# get coordinate-change for one step in direction
def xyDirection( eDir ):
	x,y = 0,0
	if   eDir == 0: x,y =  0,-1
	elif eDir == 1: x,y =  1,-1
	elif eDir == 2: x,y =  1, 0
	elif eDir == 3: x,y =  1, 1
	elif eDir == 4: x,y =  0, 1
	elif eDir == 5: x,y = -1, 1
	elif eDir == 6: x,y = -1, 0
	elif eDir == 7: x,y = -1,-1
	return x,y

# get coordinate-change for one step in cardinal direction
def xyCardinalDirection( eCard ):
	x,y = 0,0
	if   eCard == 0: x,y =  0,-1
	elif eCard == 1: x,y =  1, 0
	elif eCard == 2: x,y =  0, 1
	elif eCard == 3: x,y = -1, 0
	return x,y

# get direction from plot x,y toward dx,dy
# ignore wrapping
def coordDirection( x, y, fx, fy ):
	dx = abs(x - fx)
	dy = abs(y - fy)
	if   (x > fx) and (dx >= 2*dy):
		eDir = DirectionTypes.DIRECTION_WEST
	elif (x < fx) and (dx >= 2*dy):
		eDir = DirectionTypes.DIRECTION_EAST
	elif (y > fy) and (dy >= 2*dx):
		eDir = DirectionTypes.DIRECTION_SOUTH
	elif (y < fy) and (dy >= 2*dx):
		eDir = DirectionTypes.DIRECTION_NORTH
	elif (x > fx) and (y > fy):
		eDir = DirectionTypes.DIRECTION_SOUTHWEST
	elif (x > fx) and (y < fy):
		eDir = DirectionTypes.DIRECTION_NORTHWEST
	elif (x < fx) and (y > fy):
		eDir = DirectionTypes.DIRECTION_SOUTHEAST
	elif (x < fx) and (y < fy):
		eDir = DirectionTypes.DIRECTION_NORTHEAST
	else:
		eDir = None					# same plots
	return eDir


##########################################################################################################
########## Mapping Functions
##########################################################################################################
# lBorders = getLatitudeBorders( fSnow=0.7, fTundra=0.6, fDesertTop=0.5, fDesertBottom=0.2, fGrass=0.1,  terGen=None )
# bWater = isHighSeas( x, y, dist=1, data=None, bWrap=False, treshold=0 )
# cnt = numPlotNeighbors( x, y, ePlotType, dist=1, data=None, bWrap=False )
# cnt = numTerrainNeighbors( x, y, eTerrain, dist=1, data=None, bWrap=False )
# cnt = numFeatureNeighbors( x, y, eFeatures, dist=1, bWrap=False )
##########################################################################################################

# get list of latitude borders
def getLatitudeBorders( fSnow=0.7, fTundra=0.6, fDesertTop=0.5, fDesertBottom=0.2, fGrass=0.1, terGen=None ):
	try:
		lBorders = [terGen.fSnowLatitude, terGen.fTundraLatitude, terGen.fDesertTopLatitude, terGen.fDesertBottomLatitude, terGen.fGrassLatitude]
	except:
		fSnow += GC.getClimateInfo(MAP.getClimate()).getSnowLatitudeChange()
		fSnow = min(fSnow, 1.0)
		fSnow = max(fSnow, 0.0)

		fTundra += GC.getClimateInfo(MAP.getClimate()).getTundraLatitudeChange()
		fTundra = min(fTundra, 1.0)
		fTundra = max(fTundra, 0.0)

		fDesertTop += GC.getClimateInfo(MAP.getClimate()).getDesertTopLatitudeChange()
		fDesertTop = min(fDesertTop, 1.0)
		fDesertTop = max(fDesertTop, 0.0)

		fDesertBottom += GC.getClimateInfo(MAP.getClimate()).getDesertBottomLatitudeChange()
		fDesertBottom = min(fDesertBottom, 1.0)
		fDesertBottom = max(fDesertBottom, 0.0)

		fGrass += GC.getClimateInfo(MAP.getClimate()).getGrassLatitudeChange()
		fGrass = min(fGrass, 1.0)
		fGrass = max(fGrass, 0.0)

		lBorders = [ fSnow, fTundra, fDesertTop, fDesertBottom, fGrass]
	return lBorders

# check if there are (or more than treshold) any non-water plots in the vicinity; excluding the center spot
# tests plot at opposite end of map, if plot falls beyond the edge
def isHighSeas( x, y, dist=1, data=None, bWrap=False, treshold=0 ):
	cnt = 0
	for dx in range( -dist, dist+1 ):
		for dy in range( -dist, dist+1 ):
			if (dx == 0) and (dy == 0): continue
			if bWrap:
				# if not wrapped, ignore plots on the other side of the map
				xx, yy = normalizeXY( x, y )
				if not MAP.isWrapX():
					if (xx+dx)<0 or (xx+dx)>=iNumPlotsX: continue
				if not MAP.isWrapY():
					if (yy+dy)<0 or (yy+dy)>=iNumPlotsY: continue
			if data==None:
				plot = GetPlot( x+dx, y+dy )
				if not plot.isWater():
					if treshold == cnt: return False
					cnt += 1
			else:
				i = GetIndex( x+dx, y+dy )
				if not data[i] == PlotTypes.PLOT_OCEAN:
					if treshold == cnt: return False
					cnt += 1
	return True

# count how many plots of given plotType are in the neighborhood; excluding the center spot
# tests plot at opposite end of map, if plot falls beyond the edge (adjustable via bWrap)
def numPlotNeighbors( x, y, ePlotType, dist=1, data=None, bWrap=False ):
	cnt = 0
	for dx in range( -dist, dist+1 ):
		for dy in range( -dist, dist+1 ):
			if (dx == 0) and (dy == 0): continue
			if bWrap:
				# don't count plots on the other side of the map
				xx, yy = normalizeXY( x, y )
				if not MAP.isWrapX():
					if (xx+dx)<0 or (xx+dx)>=iNumPlotsX: continue
				if not MAP.isWrapY():
					if (yy+dy)<0 or (yy+dy)>=iNumPlotsY: continue
			if data == None:
				plot = GetPlot( x+dx, y+dy )
				if plot.getPlotType() == ePlotType:
					cnt += 1
			else:
				i = GetIndex( x+dx, y+dy )
				if data[i] == ePlotType:
					cnt += 1
	return cnt

# count how many plots of given terrain are in the neighborhood; excluding the center spot
# tests plot at opposite end of map, if plot falls beyond the edge
def numTerrainNeighbors( x, y, eTerrain, dist=1, data=None, bWrap=False ):
	cnt = 0
	for dx in range( -dist, dist+1 ):
		for dy in range( -dist, dist+1 ):
			if (dx == 0) and (dy == 0): continue
			if bWrap:
				# don't count plots on the other side of the map
				xx, yy = normalizeXY( x, y )
				if not MAP.isWrapX():
					if (xx+dx)<0 or (xx+dx)>=iNumPlotsX: continue
				if not MAP.isWrapY():
					if (yy+dy)<0 or (yy+dy)>=iNumPlotsY: continue
			if data == None:
				plot = GetPlot( x+dx, y+dy )
				if plot.getTerrainType() == eTerrain:
					cnt += 1
			else:
				i = GetIndex( x+dx, y+dy )
				if data[i] == eTerrain:
					cnt += 1
	return cnt

# count how many plots of given feature(s) are in the neighborhood; excluding the center spot
# tests plot at opposite end of map, if plot falls beyond the edge
# eFeatures may be a single eFeature of a list of eFeatures to be tested against
def numFeatureNeighbors(x, y, eFeatures, dist=1, bWrap=False):
	if type(eFeatures)==type([]):
		fList = eFeatures
	else:
		fList = [ eFeatures ]
	cnt = 0
	for dx in range( -dist, dist+1 ):
		for dy in range( -dist, dist+1 ):
			if (dx == 0) and (dy == 0): continue
			if bWrap:
				# don't count plots on the other side of the map
				xx, yy = normalizeXY( x, y )
				if not MAP.isWrapX():
					if (xx+dx)<0 or (xx+dx)>=iNumPlotsX: continue
				if not MAP.isWrapY():
					if (yy+dy)<0 or (yy+dy)>=iNumPlotsY: continue
			plot = GetPlot( x+dx, y+dy )
			if plot.getFeatureType() in fList:
				cnt += 1
	return cnt


################################################################################
########## Area Functions
################################################################################
# regionCoordList = getRegion( areaData=None, iSurround=0 )
# areaPlots = getAreaPlots( areaID=None, isValidFn = None )
# areaCoords = getAreaPlotsXY( areaID=None, isValidFn = None )
# coastPlots = getContinentCoast( continentPlots, bLakes=False )
# topAreas = getBigAreas( iTop, bCoord=True, noGoAreaPlots=None, iMinPlots=30 )
# ----------------------------------------
# Already implemented area civ4-functions
# ----------------------------------------
# MAP.recalculateAreas()
# areaID = plot.getArea()
# areaID = area.getID()
# area = plot.area()
# area = MAP.getArea( areaID )
# area = MAP.findBiggestArea( bWater )
# bTest = plot.isAdjacentToArea( area )
################################################################################

# get region from area
# - the region is the smallest rectangle surrounding the area
# - areaData may be an actual areaID or a tupel (x0,x1,y0,y1)
# - support wrapping around the edges: x0, y0 may be negative
# - ensure that always x0 <= x1 and y0 <= y1
def getRegion( areaData=None, iSurround=0 ):
	x0 = 0
	x1 = iNumPlotsX - 1
	y0 = 0
	y1 = iNumPlotsY - 1
	if areaData != None:
		if type( areaData ) == type( () ):
			# areaData already representing the region
			x0, x1, y0, y1 = areaData
		elif type( areaData ) == type( 0 ):
			# areaData is areaID
			xDict = {}
			yDict = {}
			for x in range( iNumPlotsX ):
				for y in range( iNumPlotsY ):
					pl = MAP.plot( x, y )
					if areaData == pl.getArea():
						xDict[ x ] = True
						yDict[ y ] = True
			xList = xDict.keys()
			x0 = min( xList )
			x1 = max( xList )
			if ( 0 in xList ) and ( (iNumPlotsX-1) in xList ) and ( len( xList ) != iNumPlotsX ):
				diffList = [ d for d in range( iNumPlotsX ) if d not in xList ]
				x0 = max( diffList ) + 1
				x1 = min( diffList ) - 1
			yList = yDict.keys()
			y0 = min( yList )
			y1 = max( yList )
			if ( 0 in yList ) and ( (iNumPlotsY-1) in yList ) and ( len( yList ) != iNumPlotsY ):
				diffList = [ d for d in range( iNumPlotsY ) if d not in yList ]
				y0 = max( diffList ) + 1
				y1 = min( diffList ) - 1
		# expand region into surrounding tiles
		if iSurround > 0:
			x0 = max( 0, x0 - iSurround )
			x1 = min( iNumPlotsX - 1, x1 + iSurround )
			y0 = max( 0, y0 - iSurround )
			y1 = min( iNumPlotsY - 1, y1 + iSurround )
		# ensure that x0 <= x1 and y0 <= y1
		if x0 > x1: x0 = x0 - iNumPlotsX
		if y0 > y1: y0 = y0 - iNumPlotsY
	return [x0, x1, y0, y1]

# get all plots from area; if no area is given, get all land-plots
# returns a list of plots
def getAreaPlots( areaID=None, isValidFn = None ):
	areaPlots = []
	for i in range( MAP.numPlots() ):
		plot = MAP.plotByIndex(i)
		if areaID == None:
			if plot.isWater(): continue
		else:
			if plot.getArea() != areaID: continue
		if isValidFn != None:
			if not isValidFn( plot ): continue
		areaPlots.append( plot )
	return areaPlots

# get all plots from area; if no area is given, get all land-plots
# returns a list of coordinate-pairs
def getAreaPlotsXY( areaID=None, isValidFn = None ):
	areaPlots = getAreaPlots( areaID, isValidFn )
	areaCoords = [ ( plot.getX(), plot.getY() ) for plot in areaPlots ]
	return areaCoords

# get list of coastal plots around continent; if bLakes then inside lakes are included
# continentPlots may be a list of either plots or coordinate pairs (x,y)
# returns list of plots
def getContinentCoast( continentPlots, bLakes=False ):
	coastPlots = []
	if len( continentPlots ) == 0: return coastPlots
	if type( continentPlots[0] ) == type( () ):
		conPlots = continentPlots
	else:
		conPlots = [ ( plot.getX(), plot.getY() ) for plot in continentPlots ]
	for x,y in conPlots:
		for eDir in range( DirectionTypes.NUM_DIRECTION_TYPES ):
			p = plotDirection( x, y, DirectionTypes(eDir) )
			if p.isNone(): continue
			if p.isWater():
				if p.isLake() and ( not bLakes ): continue
				if p.getTerrainType() == etCoast:
					if p not in coastPlots:
						coastPlots.append( p )
	return coastPlots

# get list of biggest land areas with their plots [iNumPlots, iAreaID, plotList]
# plotList is either a list of plots of coordinate-tuples
def getBigAreas( iTop, bCoord=True, noGoAreaPlots=None, iMinPlots=30 ):
	CyMap().recalculateAreas()
	continentArea = []
	areas = GC.getMap().areas()
	if not (noGoAreaPlots == None):
		if len( noGoAreaPlots ) == 0:
			noGoAreaPlotList = []
		else:
			if type( noGoAreaPlots[0] ) == type( () ):
				noGoAreaPlotList = [ GetPlot(x, y) for x,y in noGoAreaPlots ]
			else:
				noGoAreaPlotList = noGoAreaPlots
	for area in areas:
		if area.isWater(): continue
		if area.getNumTiles() < iMinPlots: continue
		iAreaID = area.getID()
		# check for forbidden areas
		bTaboo = False
		if not (noGoAreaPlots == None):
			for tabooPlot in noGoAreaPlotList:
				if tabooPlot.getArea() == iAreaID:
					bTaboo = True
					break
		if bTaboo: continue
		# add to list of continents
		if bCoord:
			areaPlotList = getAreaPlotsXY( iAreaID )
		else:
			areaPlotList = getAreaPlots( iAreaID )
		continentArea.append( [len(areaPlotList), area.getID(), areaPlotList] )
	continentArea = [ cont for cont in continentArea if cont[0] > 0 ]
	continentArea.sort()
	return continentArea[0:iTop]


################################################################################
########## Coordinate, Plot and Index Conversions
################################################################################
# plot = GetPlot(x,y)				# same as plot = MAP.plot(x,y), but AutoWrap					#
# inx = GetIndex(x,y)				# same as inx = MAP.plotNum(x,y), but AutoWrap
# xx,yy = coordByPlot( plot )
# xx,yy = coordByIndex( inx )
# inx = indexByPlot( plot )
# inx = MAP.plotNum(x,y)			# regular civ4 function
# plot = MAP.plotByIndex( inx ) 	# regular civ4 function
# plot = MAP.plot(x,y)				# regular civ4 function
# xx,yy = normalizeXY(x,y)
################################################################################

# convert x and y to a plot (Auto-Wrap)
# allways gives usable plot
def GetPlot(x,y):
	xx, yy = normalizeXY( x, y )
	return MAP.plot( xx, yy )

# convert x and y to an index (Auto-Wrap)
# allways gives usable index
def GetIndex(x,y):
	xx, yy = normalizeXY( x, y )
	return ( yy * iNumPlotsX + xx )

# get plot-coordinates from plot
def coordByPlot( plot ):
	if plot == None: return -1, -1
	if plot.isNone(): return -1, -1
	return plot.getX(), plot.getY()

# get plot-coordinates from index
def coordByIndex( inx ):
	if (inx < 0) or (inx > MAP.numPlots()): return -1, -1
	yy, xx = divmod( inx, iNumPlotsX )
	return xx, yy

# get plot-index from plot
def indexByPlot( plot ):
	if plot == None: return -1
	if plot.isNone(): return -1
	return ( plot.getY() * iNumPlotsX + plot.getX() )

# make sure x,y are within map-range (Auto-Wrap)
def normalizeXY( x, y ):
	xx = x % iNumPlotsX
	yy = y % iNumPlotsY
	return xx, yy


##################################################################################
########## Randomizing Functions
##################################################################################
# ab = choose(iPercent, a, b)
# xValue = chooseMore( *t )
# elem = chooseListElement( aList )
# elem = chooseListPop( aList )
# iNum = chooseNumber( iRand, iMax=None )
##################################################################################

# choose with iPercent chance
def choose( iPercent, a, b ):
	iRand = GC.getGame().getMapRand().get(100, "MapTools.choose()")
	if iRand<iPercent: return a
	return b

# choose from several (iPercent,xValue) tuples; iPercent ascending toward 100%
# parameter may be also a list of tuples
# Usage:
# bon = chooseMore( (10,"BONUS_SILVER_ORE"), (35,"BONUS_GOLD_ORE"), (50,"BONUS_COPPER_ORE"), (85,"BONUS_IRON_ORE") )
# will give the boni names with 10%, 25%, 15% and 35% probability or None with 15% probability
def chooseMore(*t):
	iRand = GC.getGame().getMapRand().get(100, "MapScriptTools.chooseMore()")
	li = []
	if len(t)==1:
		li = t[0]
	elif len(t) > 0:
		li = t
	for i in range( len(li) ):
		iPercent, xValue = li[i]
		if iRand<iPercent:
			return xValue
	return None

# randomly pick item from list
def chooseListElement(aList):
	if not len(aList): return None
	iRand = GC.getGame().getMapRand().get( len(aList), "MapScriptTools.chooseListElement()" )
	return aList[ iRand ]


# randomly pick index from list, delete index and return element
def chooseListPop(aList):
	if not aList: return None
	return aList.pop(GC.getGame().getMapRand().get( len(aList), "MapScriptTools.chooseListPop()" ))

# get random number within [0 .. iRand-1] or [iRand .. iMax-1]
def chooseNumber( iRand, iMax=None ):
	if iMax == None: iMax, iRand = iRand, 0
	if iMax < iRand: iMax, iRand = iRand, iMax
	iDice = iMax - iRand
	return GC.getGame().getMapRand().get( iDice, "MapScriptTools.chooseNumber()" ) + iRand



#######################################################################################
### CLASS DeepOcean - put Deep Ocean onto map
#######################################################################################
class DeepOcean:
	def buildDeepOcean( self, dist=4, chDeep=75 ): return

deepOcean = DeepOcean()
#######################################################################################
### CLASS DeepOcean END
#######################################################################################


#####################################################################################
### CLASS MapPrettifier - small changes to beautify you map
#####################################################################################
# connectifyLakes( chConnect=75 )
# deIcifyEdges( iLat=66, addToroidIce=True )
# hillifyCoast( chHills=66 )
# beautifyVolcanos( chHills=66 )
# lumpifyTerrain( targetTerrain, sourceTer1, sourceTer2=None )
# bulkifyIslands( chConnect=66, maxIsle=4 )
# percentifyTerrain( targetTerTuple, *sourceTerTuples )
# percentifyPlots( targetPlotType, fTargetPlotPercent, data=None, terGenerator=None )
# --- private ---
# (p1, p2) = checkDiagonalWater( x, y, eDiagoDir )
#####################################################################################
class MapPrettifier:

	# connect different lakes which are only diagonally disconnected
	# may change small single-plot lakes to Land/Hills instead of connecting
	# use after generatePlotTypes()
	def connectifyLakes( self, chConnect=75 ):
		print "[MST] ===== MapPrettifier:connectifyLakes()"
		wCnt = 0
		lCnt = 0
		edList = [
			DirectionTypes.DIRECTION_NORTHWEST, DirectionTypes.DIRECTION_NORTHEAST,
			DirectionTypes.DIRECTION_SOUTHEAST, DirectionTypes.DIRECTION_SOUTHEAST
		]
		mapRand = GC.getGame().getMapRand()
		for x in range(1, iNumPlotsX-1):
			MAP.recalculateAreas()
			for y in range(1, iNumPlotsY-1):
				plot = MAP.plot(x, y)
				if not plot.isWater(): continue
				for eDir in edList:
					cardPlots = self.checkDiagonalWater(x, y, eDir)
					if not cardPlots: continue
					pl = plotDirection(x, y, eDir)
					if pl.isNone(): continue
					if plot.getArea() == pl.getArea(): continue
					if riverMaker.hasRiverAtPlot( plot ): continue
					if riverMaker.hasRiverAtPlot( pl ): continue
					# now we have a landbridge without rivers separating two lakes
					if (MAP.getArea(plot.getArea()).getNumTiles() == 1):
						if choose(chConnect, True, False):
							plot.setPlotType( choose(90,PlotTypes.PLOT_LAND,PlotTypes.PLOT_HILLS), False, False)
							lCnt += 1
							break
					elif (MAP.getArea(pl.getArea()).getNumTiles() == 1):
						if choose(chConnect, True, False):
							pl.setPlotType( choose(90,PlotTypes.PLOT_LAND,PlotTypes.PLOT_HILLS), False, False)
							lCnt += 1
					else:
						shufflePyList(cardPlots, mapRand)
						for p in cardPlots:
							if choose( chConnect, True, False ):
								p.setPlotType( PlotTypes.PLOT_OCEAN, False, False)
								wCnt += 1
		print "[MST] %i lakes connected and %i single-plot lakes filled in" % ( wCnt, lCnt )

	# kill water-ice on non-polar edge
	# - only works if called after or at the end of normalizeAddExtras()
	def deIcifyEdges( self, iLat=66, addToroidIce=True ):
		print "[MST] ===== MapPrettifier:deIcifyEdges()"
		if not MAP.isWrapY():
			# deIcify SouthPole and southern hemisphere
			if evalLatitude( MAP.plot(0,0), True ) < iLat:
				print "[MST] DeIcify the south between %i and %i" % (0, iNumPlotsY/2 - 1)
				for x in range( iNumPlotsX ):
					for y in range( iNumPlotsY / 2 ):
						plot = MAP.plot( x, y )
						if plot.getFeatureType() == efIce:
							if not plot.isPeak():
								plot.setFeatureType( FeatureTypes.NO_FEATURE, -1 )
			# deIcify NorthPole and northern hemisphere
			if evalLatitude( MAP.plot(iNumPlotsX-1, iNumPlotsY-1) ) < iLat:
				print "[MST] DeIcify the north between %i and %i" % ((iNumPlotsY-1)/2, iNumPlotsY-1)
				for x in range( iNumPlotsX ):
					for y in range( (iNumPlotsY-1)/2, iNumPlotsY ):
						plot = MAP.plot( x, y )
						if plot.getFeatureType() == efIce:
							if not plot.isPeak():
								plot.setFeatureType( FeatureTypes.NO_FEATURE, -1 )
		# put some ice on the polar waters on toroid shaped worlds
		if addToroidIce:
			if	MAP.isWrapX() and MAP.isWrapY():
				cnt = 0
				for inx in range( MAP.numPlots() ):
					plot = MAP.plotByIndex( inx )
					lat = evalLatitude( plot ) + chooseNumber(13) - 6					# random drift
					# Ice only beyond at least 65 degrees latitude
					if lat > 65:
						if plot.isWater():
							if plot.getFeatureType() == FeatureTypes.NO_FEATURE:
								if plot.getBonusType(-1) == BonusTypes.NO_BONUS:
									if plot.canHaveFeature( efIce ):
										x,y = coordByIndex( inx )
										nIce = numFeatureNeighbors(x, y, efIce) + numTerrainNeighbors(x, y, etSnow)
										ch = (110 - lat) * 2 - (nIce * 5)
										if MAP.getClimate() == 1: ch += 12						# tropical
										if MAP.getClimate() == 4: ch -= 12						# cold
										if choose( ch, False, True ):
											plot.setFeatureType( efIce, -1 )
											cnt += 1
				# bulkify ice
				cnt1, cnt2 = 0, 0
				chIceKill = 33
				chIceAdd  = 75
				zone = [ (0,2), (2,0), (0,-2), (1,2), (2,1), (1,-2), (2,-1) ]
				for passes in [1,2]:
					for inx in range( MAP.numPlots() ):
						plot = MAP.plotByIndex( inx )
						x,y = coordByIndex( inx )
						if not plot.isWater(): continue
						if plot.getFeatureType() == efIce:
							if numFeatureNeighbors( x,y, efIce, dist=1 ) == 0:
								if choose( chIceKill, True, False ):
									plot.setFeatureType( FeatureTypes.NO_FEATURE, -1 )
									cnt1 += 1
								else:
									iceList = [ plotXY(x,y,dx/2,dy/2) for dx,dy in zone
																				 if ( plotXY(x,y,dx,dy).isWater() ) and
																				    ( plotXY(x,y,dx,dy).getBonusType(-1) == -1 ) and
																				    ( plotXY(x,y,dx,dy).getFeatureType() == efIce ) ]
									pl = chooseListElement( iceList )
									if pl == None: continue
									if choose( chIceAdd, True, False ):
										pl.setFeatureType( efIce, -1 )
										cnt2 += 1
				print "[MST] Put %i ice tiles on toroid world" % (cnt)
				print "[MST] Killed %i ice tiles on toroid world" % (cnt1)
				print "[MST] Added %i ice tiles on toroid world" % (cnt2)

	# change peaks on coast to hills
	def hillifyCoast(self, chHills=66):
		print "[MST] ===== MapPrettifier:hillifyCoast()"
		iCnt = 0
		x0 = iif( MAP.isWrapX(), 0, 1 )
		x1 = iif( MAP.isWrapX(), iNumPlotsX, iNumPlotsX-1 )
		y0 = iif( MAP.isWrapY(), 0, 1 )
		y1 = iif( MAP.isWrapY(), iNumPlotsY, iNumPlotsY-1 )
		for x in range( x0, x1 ):
			for y in range( y0, y1 ):
				pl = MAP.plot(x, y)
				if pl.isPeak() and pl.isCoastal():
					if pl.getFeatureType() < 0:					# don't transform volcanos
						if choose(chHills, True, False):
							# If a peak is along the coast, change to hills and recalc.
							pl.setPlotType(PlotTypes.PLOT_HILLS, True, True)
							iCnt += 1
		print "[MST] %i coastal peaks transformed to hills" % ( iCnt )

	# change volcanos on coast to hills / peaks - they really look bad sometimes
	def beautifyVolcanos( self, chHills=66 ): return

	# change plains/grass surrounded by desert to desert; or any other terrain combinations
	def lumpifyTerrain( self, targetTerrain, sourceTer1, sourceTer2=None ):
		print "[MST] ===== MapPrettifier:lumpifyTerrain()"
		cnt = 0
		chChange = 40
		passes = [1]
		if targetTerrain == etDesert and MAP.getClimate() == 2:
			# arid climate
			passes.append(2)
		elif targetTerrain == etGrass and MAP.getClimate() == 1:
			# tropical climate
			passes.append(2)
		for pa in passes:
			for x in range( iNumPlotsX ):
				for y in range( iNumPlotsY ):
					plot = MAP.plot( x, y )
					if plot.getTerrainType() == sourceTer1:
						tarNum = numTerrainNeighbors( x, y, targetTerrain )
						if tarNum > 3:
							if choose( chChange + 4*tarNum, True, False):
								plot.setTerrainType( targetTerrain, True, True )
								cnt += 1
					elif plot.getTerrainType() == sourceTer2:
						tarNum = numTerrainNeighbors( x, y, targetTerrain )
						if tarNum > 4:
							if choose( chChange/2 + 3*tarNum, True, False):
								plot.setTerrainType( targetTerrain, True, True )
								cnt += 1
		if cnt > 0: print "[MST] %i terrains lumpified" % ( cnt )

	# connect small islands; at least some islands of the chain should be bigger
	# builds hills/peaks - to be used before addRivers()
	def bulkifyIslands( self, chConnect=66, maxIsle=4 ):
		print "[MST] ===== MapPrettifier:bulkifyIslands()"
		cnt = 0
		for inx in range( MAP.numPlots() ):
			plot = MAP.plotByIndex( inx )
			x,y = coordByIndex( inx )
			if plot.isWater(): continue								# islands!
			area = plot.area()
			id = plot.getArea()
			if area.getNumTiles() > maxIsle: continue				# to big
			for dx,dy in [ (0,2), (2,0), (-2,0), (0,-2) ]:
				pl = plotXY( x, y, dx, dy )
				if pl.isNone(): continue								# no plot
				if pl.isWater(): continue								# islands!
				if pl.area().getNumTiles() > maxIsle: continue	# to big
				if pl.getArea() == id: continue						# same island
				if choose( chConnect, True, False ):
					p = plotXY( x, y, dx/2, dy/2 )
					pType = chooseMore( (66, PlotTypes.PLOT_LAND), (90, PlotTypes.PLOT_HILLS), (100, PlotTypes.PLOT_PEAK) )
					p.setPlotType( pType, True, True )
					if choose( 66, True, False ):
						p = plotXY( x, y, iif(dx==0,1,dx/2), iif(dy==0,1,dy/2) )
						pType = chooseMore( (66, PlotTypes.PLOT_LAND), (90, PlotTypes.PLOT_HILLS), (100, PlotTypes.PLOT_PEAK) )
						p.setPlotType( pType, True, True )
					if choose( 66, True, False ):
						p = plotXY( x, y, iif(dx==0,-1,dx/2), iif(dy==0,-1,dy/2) )
						pType = chooseMore( (66, PlotTypes.PLOT_LAND), (90, PlotTypes.PLOT_HILLS), (100, PlotTypes.PLOT_PEAK) )
						p.setPlotType( pType, True, True )
					cnt += 1
					break
			for dx,dy in [ (1,2), (2,1), (-2,1), (1,-2), (-1,2), (2,-1), (-2,-1), (-1,-2) ]:
				pl = plotXY( x, y, dx, dy )
				if pl.isNone(): continue								# no plot
				if pl.isWater(): continue								# islands!
				if pl.area().getNumTiles() > maxIsle: continue	# to big
				if pl.getArea() == id: continue						# same island
				if choose( chConnect, True, False ):
					p0 = GetPlot( x + dx/2, y + dy/2 )
					p1 = choose( 50, GetPlot( x + dx, y + dy/2), GetPlot(x + dx/2, y + dy) )
					pType0 = chooseMore( (66, PlotTypes.PLOT_LAND), (90, PlotTypes.PLOT_HILLS), (100, PlotTypes.PLOT_PEAK) )
					pType1 = chooseMore( (66, PlotTypes.PLOT_LAND), (90, PlotTypes.PLOT_HILLS), (100, PlotTypes.PLOT_PEAK) )
					p0.setPlotType( pType0, True, True )
					p1.setPlotType( pType1, True, True )
					cnt += 1
					break
		print "[MST] %i small islands connected" % ( cnt )

	# specify a terrain and its appearance-percentage on the map
	# either as percent of land or as percent of water, depending on terrain
	# - if too much of this terrain already exists on the map, some of it will change
	#   into the source terrains according to the chances given in the source-tuples
	# - if there is not enough of this terrain on the map already, some of the surrounding
	#   source terrains given will be converted to the target terrain
	# - some seed terrain needs to be already on the map
	# - example: percentifyTerrain( (etTundra,5), (etGrass,12), (etSnow,2), (etMarsh,3) )
	#   changes grass,snow,marsh into tundra or splits tundra into grass,snow,marsh
	def percentifyTerrain( self, targetTerTuple, *sourceTerTuples ):
		print "[MST] ===== MapPrettifier:percentifyTerrain()"
		# check parameter
		if len( sourceTerTuples ) == 0:
			print "[MST] No source terrains given."
			return
		# read parameter
		eTerrain, chPercent = targetTerTuple
		bWater = GC.getTerrainInfo(eTerrain).isWaterTerrain()
		srcTer = []
		chTer = []
		for i in range( len(sourceTerTuples) ):
			srcTerrain, srcChance = sourceTerTuples[i]
			# we don't change plots
			if GC.getTerrainInfo(srcTerrain).isWaterTerrain() == bWater:
				srcTer.append( srcTerrain )
				chTer.append( srcChance )
		# check seed terrain
		cntTer = 0
		for inx in range( MAP.numPlots() ):
			plot = MAP.plotByIndex( inx )
			if eTerrain == plot.getTerrainType():
				cntTer += 1
		if cntTer == 0:
			print "[MST] No seed terrain on map."
			return
		# calc cummulative change chances
		ch = 0
		for iChance in chTer: ch += iChance
		fMult = 100.0 / ch
		srcChList = []
		for i in range( len(chTer) ):
			chTer[i] = int( round(chTer[i] * fMult) )
			srcChList.append( (chTer[i], srcTer[i]) )
		# five passes if necessary
		mapRand = GC.getGame().getMapRand()
		passes = [0,1,2,3,4]
		for loop in passes:
			print " - pass %i" % (loop)
			# count things, make lists
			cntWater = 0
			cntTer = 0
			terList = []
			srcList = []
			for inx in range( MAP.numPlots() ):
				plot = MAP.plotByIndex( inx )
				ter = plot.getTerrainType()
				if plot.isWater(): cntWater += 1
				if ter == eTerrain:
					cntTer += 1
					if numTerrainNeighbors( plot.getX(), plot.getY(), eTerrain ) < (3+loop):
						if choose( 33, True, False ):
							terList.append( inx )
				elif ter in srcTer:
					if numTerrainNeighbors( plot.getX(), plot.getY(), eTerrain ) > (4-loop):
						if choose( 33, True, False ):
							srcList.append( inx )
			# more or less
			baseTer = iif( bWater, cntWater, MAP.numPlots()-cntWater )
			wantTer = baseTer * chPercent / 100
			if loop == 0:
				print "[MST] Target percentage: %i, Terrain percentage before: %4.2f(%i)" % (chPercent,cntTer*100.0/baseTer,cntTer)
				if ( abs(wantTer-cntTer) * 100.0 / baseTer ) <= 0.5:
					print "[MST] Target percentage already reached."
					return
			if wantTer < cntTer:
				# have too many already
				if terList: shufflePyList(terList, mapRand)
				for inx in terList:
					newTer = chooseMore( srcChList )
					if not (newTer == None):
						plot = MAP.plotByIndex( inx )
						plot.setTerrainType(newTer, True, True)
						cntTer -= 1
						if	wantTer == cntTer:
							print "[MST] Target percentage reached. \n Target percentage: %i, Terrain percentage after %i passes: %4.2f(%i)" % (chPercent,loop+1,cntTer*100.0/baseTer,cntTer)
							return
			else:
				# need some more
				if srcList: shufflePyList(srcList, mapRand)
				for inx in srcList:
					plot = MAP.plotByIndex( inx )
					plot.setTerrainType(eTerrain, True, True)
					cntTer += 1
					if wantTer == cntTer:
						print "[MST] Target percentage reached. \n Target percentage: %i, Terrain percentage after %i passes: %4.2f(%i)" % (chPercent,loop+1,cntTer*100.0/baseTer,cntTer)
						return
		print "[MST] Target percentage: %i, Terrain percentage after %i passes: %4.2f(%i)" % (chPercent,loop,cntTer*100.0/baseTer,cntTer)

	# specify a plotType and its appearancePercentage on the map
	# - PlotTypes.PLOT_OCEAN will convert from or into PlotTypes.PLOT_LAND
	# - PlotTypes.PLOT_LAND will convert from or into PlotTypes.PLOT_OCEAN
	# - PlotTypes.PLOT_HILLS will convert from or into PlotTypes.PLOT_LAND
	# - PlotTypes.PLOT_PEAK will convert from or into PlotTypes.PLOT_HILLS
	# - some seed plots need to be already on the map
	def percentifyPlots( self, targetPlotType, fTargetPlotPercent, data=None, terGenerator=None ):
		print "[MST] ===== MapPrettifier:percentifyPlots()"
		sourceDict = 	{
							PlotTypes.PLOT_OCEAN : PlotTypes.PLOT_LAND,
							PlotTypes.PLOT_LAND : PlotTypes.PLOT_OCEAN,
							PlotTypes.PLOT_HILLS : PlotTypes.PLOT_LAND,
							PlotTypes.PLOT_PEAK : PlotTypes.PLOT_HILLS
							}

		# check seed plots (more than 3)
		cntPlot = 0
		for inx in range( MAP.numPlots() ):
			if data == None:
				plot = MAP.plotByIndex( inx )
				pType = plot.getPlotType()
			else:
				pType = data[inx]
			if targetPlotType == pType:
				cntPlot += 1
		if cntPlot < 4:
			print "[MST] Not enough seeds on map."
			return data
		# five passes if necessary
		src = sourceDict[ targetPlotType ]
		print "[MST] Target %r, Source %r" % (targetPlotType, src)
		mapRand = GC.getGame().getMapRand()
		passes = [0,1,2,3,4]
		for loop in passes:
			print " - pass %i" % (loop)
			# count things, make lists
			cntPlot = 0
			plotList = []
			srcList = []
			for inx in range( MAP.numPlots() ):
				x, y = coordByIndex(	inx )
				if data == None:
					plot = MAP.plotByIndex( inx )
					plType = plot.getPlotType()
				else:
					plType = data[inx]
				if plType == targetPlotType:
					cntPlot += 1
					if numPlotNeighbors( x, y, targetPlotType ) < (3+loop):
						if choose( 33, True, False ):
							plotList.append( inx )
				elif plType == src:
					if numPlotNeighbors( x, y, targetPlotType ) > (4-loop):
						if choose( 33, True, False ):
							srcList.append( inx )
			# more or less
			wantPlots = int( MAP.numPlots() * fTargetPlotPercent / 100.0 )
			if loop == 0:
				print "[MST] Target percentage: %4.2f, plotType percentage before: %4.2f(%i)" % (fTargetPlotPercent,cntPlot*100.0/MAP.numPlots(),cntPlot)
				if ( abs(wantPlots-cntPlot) * 100.0 / wantPlots ) <= 0.5:
					print "[MST] Target percentage already reached."
					return data
			if wantPlots < cntPlot:
				# have too many already
				if plotList: shufflePyList(plotList, mapRand)
				for inx in plotList:
					if data==None:
						plot = MAP.plotByIndex( inx )
						plot.setPlotType( src, False, False )
						if terGenerator != None:
							x,y = coordByPlot( plot )
							terGenerator.generateTerrainAtPlot(x, y)
					else:
						data[inx] = src
					cntPlot -= 1
					if	abs(wantPlots - cntPlot) <= 1:
						print "[MST] Target percentage reached. \n Target percentage: %4.2f, PlotType percentage after %i passes: %4.2f(%i)" % (fTargetPlotPercent,loop+1,cntPlot*100.0/MAP.numPlots(),cntPlot)
						return data
			else:
				# need some more
				if srcList: shufflePyList(srcList, mapRand)
				for inx in srcList:
					if data==None:
						plot = MAP.plotByIndex( inx )
						plot.setPlotType( targetPlotType, False, False )
						if terGenerator != None:
							x,y = coordByPlot( plot )
							terGenerator.generateTerrainAtPlot(x, y)
					else:
						data[inx] = targetPlotType
					cntPlot += 1
					if	abs(wantPlots - cntPlot) <= 1:
						print "[MST] Target percentage reached. \n Target percentage: %4.2f, PlotType percentage after %i passes: %4.2f(%i)" % (fTargetPlotPercent,loop+1,cntPlot*100.0/MAP.numPlots(),cntPlot)
						return data
		print "[MST] Target percentage: %4.2f, PlotType percentage after %i passes: %4.2f(%i)" % (fTargetPlotPercent,loop,cntPlot*100.0/MAP.numPlots(),cntPlot)
		return data

	###############
	### Helpers ###
	###############

	# check diagonal plots for water, return related cardinal plots if both of those are land or None
	# (x,y) is not checked!
	def checkDiagonalWater( self, x, y, eDir ):
#		print "[MST] ======== MapPrettifier:checkDiagonalWater()"
		if MAP.plot(x, y).getPlotType() != PlotTypes.PLOT_OCEAN: return None
		x0,y0 = xyDirection( eDir )
		p = plotXY(x, y, x0, y0)
		if p.isNone(): return None
		if p.getPlotType() != PlotTypes.PLOT_OCEAN: return None
		plot0 = plotXY(x, y, x0, 0)
		plot1 = plotXY(x, y, 0, y0)
		if plot0.getPlotType() == PlotTypes.PLOT_OCEAN: return None
		if plot1.getPlotType() == PlotTypes.PLOT_OCEAN: return None
		return [ plot0, plot1 ]


####################################################################
### CLASS MapPrettifer END
####################################################################
mapPrettifier = MapPrettifier()


###############################################################################################################
### CLASS MarshMaker - place Marsh Terrain on the map
###############################################################################################################
### Convert grass or tundra flatlands to marsh
###   if they are within a hot range near the equator (giving hot jungle swamps)
###   or within a cold range near the polar circle (givng colder fens).
###############################################################################################################
# bModHasMarsh = initialize( iGrassChance=5, iTundraChance=10, tMarshHotRange=(0,18), tMarshColdRange=(45,63) )
# convertTerrain( tAreaRange=None, areaID=None )
# iArid = getAridity()
# normalizeMarshes()
# --- private ---
# convertTerrainPlot( plot )
# buildMarshlands( plot, eTerrain )
###############################################################################################################
class MarshMaker:

	# class variables
	iMarshHotBottom		= 0
	iMarshHotTop		= 18
	iMarshColdBottom	= 45
	iMarshColdTop		= 63
	chGrass				= 5
	chTundra			= 12
	cntGrass			= 0
	cntTundra			= 0

	# Latitudes in ranges must be between 0 .. 90
	def initialize(self, iGrassChance=5, iTundraChance=10, tMarshHotRange=(0,18), tMarshColdRange=(45,63)):
		print "[MST] ===== MarshMaker:initialize( %r, %r, %r, %r )" % (iGrassChance, iTundraChance, tMarshHotRange, tMarshColdRange)

		self.chGrass  = iGrassChance
		self.chTundra = iTundraChance

		# ranges 0..90
		self.iMarshHotBottom   = int( tMarshHotRange[0] )
		self.iMarshHotBottom   = min( self.iMarshHotBottom, 90 )
		self.iMarshHotBottom   = max( self.iMarshHotBottom, 0 )

		self.iMarshHotTop      = tMarshHotRange[1]
		self.iMarshHotTop     += int( 90 * GC.getClimateInfo(MAP.getClimate()).getSnowLatitudeChange() )
		self.iMarshHotTop      = min( self.iMarshHotTop, 90 )
		self.iMarshHotTop      = max( self.iMarshHotTop, 0 )

		self.iMarshColdBottom  = tMarshColdRange[0]
		self.iMarshColdBottom += int( 90 * GC.getClimateInfo(MAP.getClimate()).getDesertBottomLatitudeChange() )
		self.iMarshColdBottom  = min( self.iMarshColdBottom, 90 )
		self.iMarshColdBottom  = max( self.iMarshColdBottom, 0 )

		self.iMarshColdTop     = tMarshColdRange[1]
		self.iMarshColdTop    += int( 90 * GC.getClimateInfo(MAP.getClimate()).getDesertTopLatitudeChange() )
		self.iMarshColdTop     = min( self.iMarshColdTop, 90 )
		self.iMarshColdTop     = max( self.iMarshColdTop, 0 )
		sprint = "[MST] Marsh ranges for %s - Hot Swamp: [%r,%r], Cold Fen: [%r,%r] \n" % (sClimateType,self.iMarshHotBottom,self.iMarshHotTop,self.iMarshColdBottom,self.iMarshColdTop)

		# adjust marsh chances
		if sClimateType=="CLIMATE_TROPICAL":
			self.chGrass  += 1
			self.chTundra += 3
		elif sClimateType=="CLIMATE_ARID":
			self.chGrass  -= 1
			self.chTundra -= 3
		elif sClimateType=="CLIMATE_ROCKY":
			self.chGrass  -= 1
			self.chTundra -= 1
		elif sClimateType=="CLIMATE_COLD":
			self.chGrass  -= 1
			self.chTundra += 2
		sprint += "[MST] Base chances to change: %s - Grass->Marsh %i%s, Tundra->Marsh %i%s \n" % (sClimateType, self.chGrass, "%%", self.chTundra, "%%")

		# test if Mod supports marsh
		print sprint

	# add marsh-terrain to given region or whole map
	# tAreaRange is a 3-tuple (x,y,iRange) describing the area around x,y up to iRange distance
	def convertTerrain( self, tAreaRange=None, areaID=None ):
		print "[MST] ===== MarshMaker:convertTerrain()"

		if tAreaRange == None:
			minX = 0
			maxX = MAP.getGridWidth() - 1
			minY = 0
			maxY = MAP.getGridHeight() - 1
		else:
			minX = tAreaRange[0] - tAreaRange[2]
			maxX = tAreaRange[0] + tAreaRange[2]
			minY = tAreaRange[1] - tAreaRange[2]
			maxY = tAreaRange[1] + tAreaRange[2]

		self.cntTundra = 0
		self.cntGrass  = 0
		for x in range( minX, maxX+1 ) :
			for y in range( minY, maxY+1 ):
				plot = plotXY( x, y, 0, 0 )
				if not plot.isNone():
					if (areaID == None) or (areaID == plot.getArea()):
						self.convertTerrainPlot( plot )
		print "[MST] %i plots converted to Marsh: %i from Grass, %i from Tundra" % (self.cntGrass+self.cntTundra,self.cntGrass,self.cntTundra)

	# return aridity -3 .. 3
	def getAridity( self ):
		print "[MST] ===== MarshMaker:getAridity()"
		arid = 0
		if sClimateType == "CLIMATE_ARID":
			arid += 2
		elif sClimateType == "CLIMATE_COLD":
			arid += 1
		elif sClimateType == "CLIMATE_ROCKY":
			arid += 1
		elif sClimateType == "CLIMATE_TROPICAL":
			arid += -2
		if sSeaType == "SEALEVEL_LOW":
			arid += 1
		elif sSeaType == "SEALEVEL_HIGH":
			arid += -1
		return arid

	# make sure marshes are only on flatlands
	def normalizeMarshes( self ):
		print "[MST] ===== MarshMaker:normalizeMarsh()"
		pcnt = 0
		for inx in range(MAP.numPlots()):
			plot = MAP.plotByIndex(inx)
			if plot.isHills():
				if plot.getTerrainType() == etMarsh:
					plot.setTerrainType(etGrass, True, True)
					pcnt += 1
		if pcnt > 0: print "[MST] %i marshy hills converted to grasslands" % (pcnt)

	###############
	### Helpers ###
	###############

	# assign chances for conversion to plot and choose
	def convertTerrainPlot(self, plot):
		if plot.isFlatlands():
			iLat = plot.getLatitude()
			if iLat < 0: iLat = -iLat
			eTerrain = plot.getTerrainType()
			if self.iMarshHotBottom<=iLat and iLat<=self.iMarshHotTop:
				# tundra near equator is always converted
				if eTerrain == etTundra:
					self.buildMarshlands(plot, eTerrain)
					return
			if self.iMarshHotBottom<=iLat and iLat<=self.iMarshHotTop or self.iMarshColdBottom<=iLat and iLat<=self.iMarshColdTop:
				iWet = 0.3
				if plot.isFreshWater(): iWet += 1.2
				if plot.isCoastal(): iWet += 0.3
				if eTerrain == etTundra:
					if choose(int(iWet*self.chTundra), True, False):
						self.buildMarshlands(plot, eTerrain)
				elif eTerrain == etGrass:
					if choose(int(iWet*self.chGrass), True, False):
						self.buildMarshlands(plot, eTerrain)
				elif eTerrain == etMarsh:
					self.buildMarshlands(plot, eTerrain) # give extra chance to convert neighbor

	# put marsh on map and try for a neighbor
	def buildMarshlands( self, plot, eTerrain ):
#		print "[MST] ======== MarshMaker:buildMarshlands()"
		sprint = ""
		if eTerrain==etMarsh:									# if already marsh, check if we want to convert neighbor
			if choose( 80, True, False): return
		else:
			plot.setTerrainType( etMarsh, True, True )
			if eTerrain==etTundra:
				self.cntTundra += 1
			else:
				self.cntGrass += 1
			sprint += "[MST] Marsh created @ %i,%i \n" % (plot.getX(),plot.getY())

		# add one or two more marshes to keep the plot from getting lonely and strange
		x = plot.getX()
		y = plot.getY()
		pList = []
		for eCard in range( CardinalDirectionTypes.NUM_CARDINALDIRECTION_TYPES ):
			p = plotCardinalDirection( x, y, CardinalDirectionTypes(eCard) )
			if not p.isNone():
				if p.isFlatlands():
					eTerrain = p.getTerrainType()
					if (eTerrain==etGrass) or (eTerrain==etTundra):
						pList.append( p )
						# plots with freshwater get two chances
						if plot.isFreshWater():
							pList.append( p )

		if pList:
			shufflePyList(pList, GC.getGame().getMapRand())
			if etTundra==pList[0].getTerrainType():
				self.cntTundra += 1
			else:
				self.cntGrass += 1
			pList[0].setTerrainType( etMarsh, True, True )
			sprint += "[MST] - More Marsh created @ %i,%i \n" % (pList[0].getX(),pList[0].getY())
			if (len(pList)>2) and choose( 25, True, False ):
				if pList[0] != pList[1]:
					iLat = abs( pList[1].getLatitude() )
					if (self.iMarshHotBottom<=iLat and iLat<=self.iMarshHotTop) or (self.iMarshColdBottom<=iLat and iLat<=self.iMarshColdTop):
						pList[1].setTerrainType( etMarsh, True, True )
						sprint += "[MST] - More Marsh created @ %i,%i \n" % (pList[1].getX(),pList[1].getY())
#		print sprint

##############################################################################################################
### CLASS MarshMaker END
##############################################################################################################
marshMaker = MarshMaker()


###############################################################################################################
### CLASS MapRegions - special regions you can place on your map
###############################################################################################################
### - The Big Bog (big, small, with or without lake )
### - The Big Dent (single, double and sideways)
### Create special features on the map. Use after generateTerrainTypes() and before addRivers()
###############################################################################################################
# initialize( regDist=15 )
# buildLostIsle( chance=33, minDist=7, bAliens=False )
# namePlot = theLostIsle( pCenterPlot, pList, bAliens )
# buildBigBogs( iBogs=None )
# namePlot = theBigBog( pCenterPlot, bBigBog=True, bBogLake=True )
# buildBigDents( iDents=None )
# namePlot = theBigDent( pCenterPlot, bSideways=None, chAccess=66 )
# adjustBigDentsTemplate( bRandom )
# addRegionExtras()
# --- private ---
# template = rotateTemplate( tempDict, steps )
# addLostIsleExtras()
# addBigBogExtras()
# addBigDentExtras()
# bValid = regionCheck( plot )
# deleteNonBogPlots( plotList )
# deleteNonDentPlots( plotList )
# log = changeBogTerrain( plot, temp )
# tileList = createLostIsle( minDist, bAliens )
# tileList = stampLostIsle( template, center )
###############################################################################################################
class MapRegions:

	# class variables
	noSigns = False
	regionNames = []
	regionList = []
	regionDist = 15
	# 1 probably lake, 2 marshy and flat, 3 probably marsh 4 probably grass 5 probably hills/peaks - no snow/desert
	bigBogTemplate = {
		0: [0,0,0,4,5,5,4,0,0,0,0],
		1: [0,0,5,4,4,4,4,4,5,0,0],
		2: [0,5,4,4,3,3,3,3,4,5,0],
		3: [0,4,3,2,2,2,2,2,3,4,0],
		4: [5,4,3,2,1,1,2,2,3,4,5],
		5: [5,4,3,2,1,1,2,2,3,4,5],
		6: [0,4,3,2,2,2,2,2,3,4,0],
		7: [0,5,4,4,3,3,3,4,4,5,0],
		8: [0,0,5,4,3,3,4,4,5,0,0],
		9: [0,0,0,5,4,4,5,0,0,0,0]
	}
	# 1 probably lake, 2 marshy and flat, 3 probably marsh 4 probably grass 5 probably hills/peaks - no snow/desert
	smallBogTemplate = {
		0: [0,0,5,5,4,0,0],
		1: [0,5,4,3,4,5,0],
		2: [4,3,3,2,3,4,4],
		3: [4,3,2,1,2,4,5],
		4: [5,4,2,1,2,4,5],
		5: [4,3,2,2,2,4,5],
		6: [4,3,3,2,3,3,4],
		7: [0,5,4,3,4,5,0],
		8: [0,0,4,5,4,0,0]
	}
	bogList = []
	# 1 possibly Volcano, 2 probably Peaks, 3 probably Peaks/Hills, 4 probably Hills
	bigDentTemplate  = {
		0: [0,0,0,0,4,4,4,4,3,4,0,0,0,0],
		1: [0,0,4,4,4,3,3,3,3,3,4,4,0,0],
		2: [0,4,4,3,3,2,2,3,2,3,3,3,4,0],
		3: [0,4,3,2,4,2,2,2,2,2,2,3,4,0],
		4: [4,3,2,2,2,1,1,4,1,2,3,2,3,4],
		5: [4,3,2,2,1,1,2,2,1,1,4,2,3,4],
		6: [0,4,3,2,2,2,4,2,2,2,2,3,4,0],
		7: [0,4,4,3,3,2,2,2,2,3,3,4,4,0],
		8: [0,0,4,4,4,3,3,3,3,3,4,4,0,0],
		9: [0,0,0,0,4,4,4,4,4,4,0,0,0,0]
	}
	dentList = []
	dentRivers = None
	dentTemplate = {}

	# Lost Isle - 3 Peak, 2 Hills, 1 Land, 0 Ocean
	lostIsleTemplate = {
		4: [0, 0, 3, 0, 0],
		3: [0, 1, 2, 3, 0],
		2: [0, 2, 0, 2, 3],
		1: [3, 2, 1, 1, 2],
		0: [0, 3, 3, 3, 0]
	}
	lostIsleList = []


	# initialize class variables
	def initialize( self, regDist=15, noSigns=False ):
		print "[MST] ===== MapRegions:initialize( regDist=%r, noSigns=%r )" % ( regDist, noSigns )
		# class
		self.noSigns = noSigns
		self.regionNames = []
		self.regionList = []
		self.regionDist = regDist + MAP.getWorldSize()
		# bigbog
		self.bogList = []
		# bigdent
		self.bigDentTemplate  = {
			0: [0,0,0,0,4,4,4,4,3,4,0,0,0,0],
			1: [0,0,4,4,4,3,3,3,3,3,4,4,0,0],
			2: [0,4,4,3,3,2,2,3,2,3,3,3,4,0],
			3: [0,4,3,2,4,2,2,2,2,2,2,3,4,0],
			4: [4,3,2,2,2,1,1,4,1,2,3,2,3,4],
			5: [4,3,2,2,1,1,2,2,1,1,4,2,3,4],
			6: [0,4,3,2,2,2,4,2,2,2,2,3,4,0],
			7: [0,4,4,3,3,2,2,2,2,3,3,4,4,0],
			8: [0,0,4,4,4,3,3,3,3,3,4,4,0,0],
			9: [0,0,0,0,4,4,4,4,4,4,0,0,0,0]
		}
		self.dentList = []
		self.dentRivers = None
		self.dentTemplate = {}

	# build formerly populated island
	def buildLostIsle( self, chance=33, minDist=7, bAliens=False ):
		print "[MST] ===== MapRegions:buildLostIsle(%r,%r,%r)" % (chance, minDist, bAliens)

		if choose(chance, False, True):
			print "[MST] No Lost Isle choosen"
			return

		# get continents and islands
		MAP.recalculateAreas()
		areaList = GC.getMap().areas()
		isleList = []

		# make Lost Isle
		islePlotCoordList = self.createLostIsle(minDist, bAliens)
		islePlotCoordList.sort()
		islePlotCoordList.reverse()
		if not islePlotCoordList:
			print "[MST] Unable to create Lost Isle"
			return

		# build Lost Isle
		x,y = islePlotCoordList[0]		# should be the plot to the upper right
		namePlot = self.theLostIsle( GetPlot(x,y), islePlotCoordList, bAliens )
		self.regionNames.append( ('LostIsle', "The Lost Isle", namePlot) )
		printList( islePlotCoordList, "islePlotCoordList:", rows = 1, prefix = "[MST] " )
		print "[MST] Lost Isle built @ (%r,%r)" % (x,y)

	# register where and how the 'Lost Isle' should be built
	def theLostIsle( self, pCenterPlot, pList, bAliens ):
		print "[MST] ===== MapRegions:theLostIsle()"

		# get coordinates
		x0 = pCenterPlot.getX()
		y0 = pCenterPlot.getY()
		# register
		self.regionList.append( [x0, y0] )
		self.lostIsleList.append( [x0, y0, pList, bAliens] )
		return pCenterPlot

	#----------------------------------------------------------------------------
	#        dx-4 dx  dx+5
	#         !   !    !
	#        ............  		. no change
	# dy+4   .....hh.....  		h chance for new hills or peak , no snow or desert
	# dy+3   ...hggggh...  		g probably grass - probably flat
	# dy+2   ..hggmmggh..  		m probably marsh - flat
	#  .     ..gmmmmmmg..     	o probably lake
	# dy     .hgmmoommgh.
	#  .     ..gmmmmmmg..  		there may be a single peak somewhere
	# dy-2   ..hggmmggh..
	# dy-3   ...hggggh...
	# dy-4   .....hh.....
	#        ............
	#         !   !    !
	#        dx-4 dx  dx+5
	#----------------------------------------------------------------------------
	#  - if iBogs>0: try build The BigBog, on two biggest continents
	#  - if iBogs>builtBogs: try build 1st smallBog with lake, on four biggest continents
	#  - if iBogs>builtBogs: try build 2nd smallBog without lake, on four biggest continents
	def buildBigBogs( self, iBogs=3 ):
		print "[MST] ===== MapRegions:buildBigBogs()"

		nBog = 0
		chBigBog = 75
		chSmallBog = 66
		bLake = False
		# one BigBog
		if choose( chBigBog, True, False ) and (iBogs > nBog):
			# get two biggest continents
			continents = getBigAreas( 2, False )					# [ (iNumPlots, iAreaID, plotList), ... ]
			for inx in range( len( continents ) ):
				self.deleteNonBogPlots( continents[inx][2] )
				continents[inx][0] = len( continents[inx][2] )
			sprint = ""
			for area in continents:	sprint += "[MST] BigBogAreas: ID:%7i,  Plots:%4i \n" % ( area[1], area[0] )
			print sprint
			# get valid plots
			validPlots = [ p for num,id,pList in continents
			                 for p in pList
			                 if self.regionCheck( p ) and self.wrapCheck( p ) ]

			# build big bog
			if len( validPlots ) > 0:
				plot = chooseListElement( validPlots )
				namePlot = self.theBigBog( plot, True )									# do the big bog
				nBog += 1
				self.regionNames.append( ('Bog', "Big Bog", namePlot) )
			else:
				print "[MST] No Big Bog"

		# one SmallBog with Lake
		if choose( chSmallBog, True, False ) and (iBogs > nBog):
			# get four biggest continents
			continents = getBigAreas( 4, False )
			for inx in range( len( continents ) ):
				self.deleteNonBogPlots( continents[inx][2] )
				continents[inx][0] = len( continents[inx][2] )
			sprint = ""
			for area in continents:	sprint += "[MST] SmallBogAreas: ID:%7i,  Plots:%4i \n" % ( area[1], area[0] )
			print sprint
			# get valid plots
			validPlots = [ p for num,id,pList in continents
			                 for p in pList
			                 if self.regionCheck( p ) and self.wrapCheck( p ) ]
			# build small bog
			if len( validPlots ) > 0:
				plot = chooseListElement( validPlots )
				namePlot = self.theBigBog( plot, False, True )							# do the small bog with lake
				nBog += 1
				# set landmark
				self.regionNames.append( ('Bog', "Darkwater", namePlot) )
				if MAP.getWorldSize() < 2: chSmallBog = chSmallBog / 2
			else:
				print "[MST] No Small Bog with Lake"

		# one SmallBog without Lake
		if choose( chSmallBog, True, False ) and (iBogs > nBog):
			# get four biggest continents
			continents = getBigAreas( 4, False )
			for inx in range( len( continents ) ):
				self.deleteNonBogPlots( continents[inx][2] )
				continents[inx][0] = len( continents[inx][2] )
			sprint = ""
			for area in continents:	sprint += "[MST] SmallBogAreas: ID:%7i,  Plots:%4i \n" % ( area[1], area[0] )
			print sprint
			# get valid plots
			validPlots = [ p for num,id,pList in continents
			                 for p in pList
			                 if self.regionCheck( p ) and self.wrapCheck( p ) ]
			# build small bog
			if len( validPlots ) > 0:
				plot = chooseListElement( validPlots )
				namePlot = self.theBigBog( plot, False, False )						# do the small bog without lake
				nBog += 1
				# set landmark
				self.regionNames.append(('Bog', "Darkwater", namePlot))
			else:
				print "[MST] No Small Bog without Lake"
		print "[MST] %i bogs built: %r" % ( nBog, self.bogList )
#		print "[MST] Regions: %r" % ( self.regionList )

	# Builds boglands with a lake surrounded by hills and outflowing rivers
	# Center needs to be on land and have marshy latitude
	def theBigBog( self, pCenterPlot, bBigBog=True, bBogLake=True ):
#		print "[MST] ======== MapRegions:theBigBog()"
		if pCenterPlot==None: return None

		# get template
		if bBigBog:
			template = self.bigBogTemplate.copy()
			if MAP.getWorldSize() < 4:
				del template[5]
		else:
			template = self.smallBogTemplate.copy()
			if (MAP.getWorldSize() < 4) and choose(80, True, False):
				del template[5]
			if (MAP.getWorldSize() < 2) and choose(80, True, False):
				del template[3]
		rot = chooseNumber( 4 )
		template = self.rotateTemplate( template, rot )
		printDict( template, "rotated %i" % (rot), prefix="[MST] " )
		print "[MST] Template rotated %i degrees" % (rot*90)

		# get region size
		x0   = pCenterPlot.getX()
		minX = x0 + 1 - ( len( template[0] ) / 2 ) + ( len( template[0] ) % 2 )
		maxX = minX + len( template[0] ) - 1
		k = len( template.keys() )
		y0   = pCenterPlot.getY()
		minY = y0 + 1 - ( k / 2 ) + ( k % 2 )
		maxY = minY + k - 1

		# handle namePlot
		namePlot = None			#
		for dy in range( minY, maxY + 1 ):
			for dx in range( minX, maxX + 1 ):
				if template[dy-minY][dx-minX] == 1:
					namePlot = MAP.plot( dx, dy )
					break
		if namePlot == None:
			namePlot = GetPlot( minX + (maxX - minX) / 2, minY - 1 + (maxY - minY) / 2 )
		x0, y0 = coordByPlot( namePlot )

		# register
		if bBigBog: bBogLake = True									# Big Bogs can always have a lake
		self.regionList.append( [x0, y0] )
		self.bogList.append( [x0, y0, bBigBog, bBogLake] )
		print "[MST] Building %s bog @ %3i,%2i %s \n" % ( iif(bBigBog,"big", "small"), x0, y0, "with"+iif(bBogLake,"","out")+" Lake" )

		# pass 1 - change plots
		sprint = ""
		sprint += "[MST] pass 1 - change plots \n"
		chFlatHill  = 80				# chance to flatten hills
		chPeakHill  = 80				# chance to flatten peaks
		chLake      = 90				# chance to make lake
		for dx in range( minX, maxX + 1 ):
			for dy in range( minY, maxY + 1 ):
				temp = template[dy-minY][dx-minX]
				if temp==0: continue																# ignore
				plot = plotXY( dx, dy, 0, 0 )
				if plot.isNone(): continue														# no plot -> ignore
				if temp>0:
					if plot.isLake(): continue													# lake stays
					elif temp==5:
						if plot.isFlatlands():
							if choose( chPeakHill, True, False ):
								plot.setPlotType( PlotTypes.PLOT_PEAK, True, True )	# Flat -> Peak
							else:
								plot.setPlotType( PlotTypes.PLOT_HILLS, True, True )	# Flat -> Hills
						elif plot.isWater():
							if choose( chPeakHill/2, True, False ):
								plot.setPlotType( PlotTypes.PLOT_PEAK, True, True )	# Flat -> Peak
							else:
								plot.setPlotType( PlotTypes.PLOT_HILLS, True, True )	# Flat -> Hills
					elif plot.isWater():
						plot.setPlotType( PlotTypes.PLOT_LAND, True, True )			# Ocean -> Land
					elif plot.isPeak():															# Peak -> Land
						iChange = chFlatHill
						if temp==4:																	# 10%: Peak -> Land
							iChange = 100 - iChange
						if choose( iChange, True, False):									# 90%: Peak -> Land
							plot.setPlotType( PlotTypes.PLOT_LAND, True, True )
						else:
							plot.setPlotType( PlotTypes.PLOT_HILLS, True, True )
					elif plot.isHills():
						iChange = chFlatHill
						if temp==4:																	# 10%: Hills -> Land
							iChange = 100 - iChange
						if choose( iChange, True, False):									# 90%: Hills -> Land
							plot.setPlotType( PlotTypes.PLOT_LAND, True, True )
					if temp==1:
						if choose( chLake, True, False ):
							plot.setPlotType( PlotTypes.PLOT_OCEAN, True, True )		# make lake

		# handle bog lake
		if bBogLake:
			namePlot.setPlotType( PlotTypes.PLOT_OCEAN, True, True )
		else:
			if namePlot.isWater():
				namePlot.setPlotType( PlotTypes.PLOT_LAND, True, True )

		# pass 2 - change terrain
		sprint += "[MST] pass 2 - change terrain \n"
		for dx in range( minX, maxX + 1 ):
			for dy in range( minY, maxY + 1 ):
				temp = template[dy-minY][dx-minX]
				if temp==0: continue											# ignore
				plot = plotXY( dx, dy, 0, 0 )
				if plot.isNone(): continue										# no plot -> ignore
				if plot.isWater(): continue										# ignore lakes
				if plot.isPeak(): continue										# ignore peaks
				log = self.changeBogTerrain( plot, temp )						# change terrain to bog
				# sprint += log
				if plot.isHills() and plot.getTerrainType()==etMarsh:			# no marsh on hills
					plot.setTerrainType(etGrass, True, True)

		# pass 3 - change surrounding desert to plains (arid to moist)
		sprint += "[MST] pass 3 - change surrounding desert/snow \n"
		for dx in range( minX-1, maxX + 2 ):
			for dy in range( minY-1, maxY + 2 ):
				if ( (dy==minY-1) or (dy==maxY+1) or (dx==minX-1) or (dx==maxX+1) ):
					plot = plotXY( dx, dy, 0, 0 )
					if not plot.isNone():
						if plot.getTerrainType() == etSnow:							# no snow bog neighbor
							plot.setTerrainType(etTundra, True, True)
						elif plot.getTerrainType() == etDesert:						# no desert bog neighbor
							plot.setTerrainType(etPlains, True, True)

		# pass 4 - build river(s) ending in water
		sprint += "[MST] pass 4 - build river(s) ending in water \n"
		print sprint
		sprint = ""
		rStart = []
		# start rivers
		MAP.recalculateAreas()
		if namePlot.isWater:
			# bog-lake found: start river(s) here
			id = MAP.plot( x0, y0 ).getArea()
			riverMaker.buildRiversFromLake( id, 100, iif(bBigBog, 2, 1), 1 )
		else:
			# no bog-lake found: start rivers from rim
			rivStart = riverMaker.rimRivers(x0, y0, [5,4,3]) # [ bogDir, riverPlot, eCard ]
			lastDir = []
			for i in [0, 1]:

				if not rivStart: break

				inx = GC.getGame().getMapRand().get(len(rivStart), "chooseListIndex" )

				bogDir, rPlot, eCard = rivStart[inx]
				while bogDir in lastDir and rivStart:
					bogDir, rPlot, eCard = rivStart.pop(GC.getGame().getMapRand().get(len(rivStart), "chooseListPop" ))

				if bogDir in lastDir: break

				if not lastDir:
					lastDir.append(bogDir)
					lastDir.append(addDirection(bogDir, 1))
					lastDir.append(addDirection(bogDir,-1))
				# found one - make river
				sprint = ""
				sprint += "[MST] Lake: None - River-Start @ %i,%i --> %5s - %s \n" % (rPlot.getX(), rPlot.getY(), cardinalName(eCard), "Normal Flow" )
				rList = []
				riverMaker.buildRiver( rPlot, True, eCard, riverList=rList )
				sprint += riverMaker.outRiverList( rList, "[MST]" )
				print sprint
		return namePlot

	#------------------------------------------------------------------------
	#                                        dx
	#                                         !
	#             dx                      ...........
	#              !              dy+5+s  .....h.....    . no change
	#        .......|......       dy+4+s  ...hhHhh...    h hilly
	# dy+4   .....hh|h.....       dy+3+s  ..hhH^Hhh..    H hills or peaks
	# dy+3   ...hhHH|Hhh...       dy+2+s  ..hH^^^Hh..    ^ mountainous
	# dy+2   ..hhHH^|HHhh..       dy+1+s  .hHH^M^HHh.    M Mountains probably
	# dy+1   ..hH^^^|^^Hh..               -----------
	# dy     .hH^^MM|M^^Hh.       dy      .hH^^M^^Hh.    s size adjustment
	# dy-1   ..hH^^^|^^Hh..       dy-1    .hHH^M^HHh.      0 duel, tiny, small
	# dy-2   ..hhHH^|HHhh..       dy-2    ..hH^^^Hh..      1 standard
	# dy-3   ...hhHH|Hhh...       dy-3    ..hhH^Hhh..      2 large
	# dy-4   .....hh|h.....       dy-4    ...hhHhh...      3 huge
	#        .......|......       dy-5    .....h.....      4 giant
	#         !    !     !                ...........      5 gigantic
	#       dx-5  dx  dx+5+s               !   !   !
	#                                    dx-4 dx dx+4
	#
	#------------------------------------------------------------------------

	# build several BigDents
	#	- if iDents>0: try build 1st bigDent, not sideways, maybe as doubleDent
	#  - if iDents>builtDents: if mapsize>tiny: try build 2nd bigDent, maybe sideways
	#  - if iDents>builtDents: if mapsize>standard: try build 3rd bigDent, maybe sideways
	#  - except for doubleDent: don't use same continent twice
	def buildBigDents( self, iDents=3 ):
		print "[MST] ===== MapRegions:buildBigBDents()"

		# Names for Mountain Ranges
		dentNames = [
			{
				'Volcano': "Big Brothers Doom",
				'Peak':    "The Big Brother",
				'Flat':    "Watchers Retreat",
				'Water':   "Watchers Pool",
				'Hills':   "Big Brother Range"
			},
			{
				'Volcano': "Hole of the World",
				'Peak':    "The Big Dent",
				'Flat':    "Hammers Rest",
				'Water':   "Hole of the World",
				'Hills':   "Big Dent Highlands"
			},
			{
				'Volcano': "Roaring Spike",
				'Peak':    "The Great Spine",
				'Flat':    "Lost Valley",
				'Water':   "Forgotten Hollow",
				'Hills':   "Great Spinal Mountains"
			},
			{
				'Volcano': "Abyss Portal",
				'Peak':    "Celestial Throne",
				'Flat':    "Paradise Meadow",
				'Water':   "Divine Bath",
				'Hills':   "Celestial Belt"
			},
			{
				'Volcano': "Demons Breath",
				'Peak':    "Roof of the World",
				'Flat':    "Banshies Playground",
				'Water':   "Devils Pool",
				'Hills':   "Howling Barrier"
			},
			{
				'Volcano': "Witches Cauldron",
				'Peak':    "Warlocks Hat",
				'Flat':    "Fairies Glen",
				'Water':   "Satyrs Lake",
				'Hills':   "Wizard Mountains"
			}
		]

		dentBoni = {
			'Flat':		[ebSilver, ebFur, ebDeer, ebMarble, ebCow, ebHorse],
			'Hills':	[ebSilver, ebMarble, ebGold, ebSheep, None],
			'Terrain':	[etSnow, etTundra, etTundra, etMarsh, etGrass, etGrass]
		}

		# start building
		nDent = 0
		chDent = 75
		chDoubleDent = 33
		bDouble = False
		# First BigDent
		if choose( chDent, True, False ) and (iDents > nDent):
			# get two biggest continents
			continents = getBigAreas( 2, False )
			for inx in range( len( continents ) ):
				self.deleteNonDentPlots( continents[inx][2] )
				continents[inx][0] = len( continents[inx][2] )
			sprint = ""
			for area in continents:	sprint += "[MST] 1st BigDentAreas: ID:%7i,  Plots:%4i \n" % ( area[1], area[0] )
			print sprint
			# get valid plots
			validPlots = [ p for num,id,pList in continents
			                 for p in pList
			                 if self.regionCheck( p ) and self.wrapCheck( p ) ]
			# build first dent
			if len(validPlots) > 0:
				plot = chooseListElement( validPlots )
				self.adjustBigDentsTemplate()
				namePlot = self.theBigDent( plot, False ) # do the first dent
				nDent += 1
				# get landmark bonus
				pos = chooseNumber(5)
				eTerr = dentBoni['Terrain'][pos]
				eFlat = dentBoni['Flat'][pos]
				eHills = dentBoni['Hills'][pos]
				# set landmark
				dentDict = chooseListPop( dentNames )
				dentDict['Plot'] = namePlot
				eForest = -1
				if eTerr in [etMarsh, etGrass]:
					eForest = chooseMore( (25,efJungle), (66,efForest), (100,-1) )
				if eForest == efJungle and evalLatitude(namePlot) > 60:
					eForest = efForest
				dentDict['Flat'] = [ dentDict['Flat'], eTerr, eForest, eFlat ]
				dentDict['Hills'] = [ dentDict['Hills'], eHills ]
				self.regionNames.append( ('Dent', dentDict) )
				# check for double dent
				if (MAP.getWorldSize() > 2) and (iDents > 1):
					if choose( chDoubleDent, True, False ):
						rad = 12 + chooseNumber( 6 )
						x0 = plot.getX()
						y0 = plot.getY()
						plotList = []
						for dx in range(-rad, rad+1):
							for dy in range(-rad, rad+1):
								if abs(dy) > (rad/3): continue
								pl = plotXY( x0, y0, dx, dy )
								if pl.isNone(): continue
								dist = rad + iif(dy in [-1,0,1],3,0) + iif(dy in [-2,-1,0,1,2],2,0) + iif(dy in [-3,-2,-1,0,1,2,3],1,0)
								if plotDistance( x0, y0, x0+dx, y0+dy ) == dist:
									plotList.append( pl )
						self.deleteNonDentPlots( plotList )
						if len( plotList ) > 0:
							pl = chooseListElement( plotList )
							self.adjustBigDentsTemplate( True )
							bSide = choose(25, True, False)
							namePlot = self.theBigDent( pl, bSide )							# make double dent
							nDent += 1
							bDouble = True
							# get landmark bonus
							pos = chooseNumber(5)
							eTerr = dentBoni['Terrain'][pos]
							eFlat = dentBoni['Flat'][pos]
							eHills = dentBoni['Hills'][pos]
							# set landmark
							dentDict = chooseListPop( dentNames )
							dentDict['Plot'] = namePlot
							eForest = -1
							if eTerr in [etMarsh, etGrass]:
								eForest = chooseMore((25,efJungle), (66,efForest), (100,-1))
							if eForest == efJungle and evalLatitude(namePlot) > 60:
								eForest = efForest
							if eForest == efJungle and evalLatitude(namePlot) < 45:
								if choose(33, True, False):
									eFlat = ebBanana
							dentDict['Flat'] = [ dentDict['Flat'], eTerr, eForest, eFlat ]
							dentDict['Hills'] = [ dentDict['Hills'], eHills ]
							self.regionNames.append( ('Dent', dentDict) )
						else: sprint += " No room for DoubleDent \n"

		# Later BigDents
		minMap = 2
		while (MAP.getWorldSize() > minMap) and (iDents > nDent):
			if choose( chDent/(minMap-1), True, False ):
				# get two biggest continents
				noAreas = []
				for dx,dy,dummy in self.dentList: noAreas.append( plotXY(dx,dy,0,0) )
				continents = getBigAreas( 3, False, noAreas )
				for inx in range( len( continents ) ):
					self.deleteNonDentPlots( continents[inx][2] )
					continents[inx][0] = len( continents[inx][2] )
				sprint = ""
				for area in continents:	sprint += "[MST] Next BigDentAreas: ID:%7i,  Plots:%4i \n" % ( area[1], area[0] )
				print sprint
				# get valid plots
				validPlots = [ p for num,id,pList in continents
									  for p in pList
									  if self.regionCheck( p ) and self.wrapCheck( p ) ]
				# build next dent
				if len( validPlots ) > 0:
					self.adjustBigDentsTemplate()
					plot = chooseListElement( validPlots )
					bSide = choose(66, True, False)
					namePlot = self.theBigDent( plot, bSide )			# do the other dents
					nDent += 1
					pos = chooseNumber(5)
					eTerr = dentBoni['Terrain'][pos]
					eFlat = dentBoni['Flat'][pos]
					eHills = dentBoni['Hills'][pos]
					# set landmark
					dentDict = chooseListPop( dentNames )
					dentDict['Plot'] = namePlot
					eForest = -1
					if eTerr in [etMarsh, etGrass]:
						eForest = chooseMore( (25,efJungle), (66,efForest), (100,-1) )
					if eForest == efJungle and evalLatitude(namePlot) > 60:
						eForest = efForest
					dentDict['Flat'] = [ dentDict['Flat'], eTerr, eForest, eFlat ]
					dentDict['Hills'] = [ dentDict['Hills'], eHills ]
					self.regionNames.append( ('Dent', dentDict) )
				else:
					sprint += "[MST] No room for Dent \n"
					break
			minMap += 1

		print "[MST] %i dents built: %r" % ( nDent, self.dentList )
#		print "[MST] Regions: %r" % ( self.regionList )

	# make a big dent
	def theBigDent( self, pCenterPlot, sideways=None, chAccess=66 ):
#		print "[MST] ======== MapRegions:theBigDent()"
		if pCenterPlot==None: return None

		# deal sideways
		rot = chooseNumber( 4 )
		printDict( self.dentTemplate, "dentTemplate:", prefix="[MST] " )
		template = self.rotateTemplate( self.dentTemplate, rot )
		if not (sideways == None):
			bSide = len( template[0] ) < len( template.keys() )
			if ( sideways and (not bSide) ) or ( bSide and (not sideways) ):
				template = self.rotateTemplate( template, 1 )
				rot += 1
		printDict( template, "rotated %i" % (rot), prefix="[MST] " )
		print "[MST] Template rotated %i degrees, sideways: %r" % (rot*90, sideways)

		# get region size
		x0   = pCenterPlot.getX()
		minX = x0 + 1 - ( len( template[0] ) / 2 ) + ( len( template[0] ) % 2 )
		maxX = minX + len( template[0] ) - 1
		k = len( template.keys() )
		y0   = pCenterPlot.getY()
		minY = y0 + 1 - ( k / 2 ) + ( k % 2 )
		maxY = minY + k - 1
		namePlot = GetPlot( minX + (maxX - minX) / 2, minY + 1 + (maxY - minY) / 2 )

		# register
		bSide = len( template[0] ) < len( template.keys() )
		self.regionList.append( [x0, y0] )
		self.dentList.append( [x0, y0, bSide] )
		print "[MST] Building Big Dent @ %3i,%2i" % ( x0, y0 )

		# pass 1 - change plots
		sprint = "[MST] pass 1 - change plots \n"
		var = chooseNumber(8)		# makes some dents more mountainous than others
		chWaterFlat = 82 + var		# chance Water->Flat
		chFlatHill  = 63 + var		# chance Flat->Hill
		chHillPeak  = 63 + var		# chance Hill->Peak
		chSnow      = 66           # chance for snow terrain
		chTundra    = 60           # chance for tundra terrain
		chVolcano   =  5				# chance for volcano for each of the 15 center plots
		chRiver     = 30				# chance for river
		bHasVolcano = False

		for dx in range( minX, maxX + 1 ):
			for dy in range( minY, maxY + 1 ):
				temp = template[dy-minY][dx-minX]
				if temp==0: continue																# ignore
				plot = plotXY( dx, dy, 0, 0 )
				if plot.isNone(): continue														# no plot -> ignore
				if temp==4:																			# hilly
					if plot.isWater():
						if choose( chWaterFlat, True, False ):
							plot.setPlotType( PlotTypes.PLOT_LAND, True, True )
					if plot.isFlatlands():
						iChance = chFlatHill
						if choose( iChance, True, False ):
							plot.setPlotType( PlotTypes.PLOT_HILLS, True, True )
					if plot.isHills():
						iChance = 100 - chHillPeak
						if choose( iChance, True, False ):
							plot.setPlotType( PlotTypes.PLOT_PEAK, True, True )
				if temp==3:																			# hills or peaks
					if plot.isWater():
						iChance = (100-chWaterFlat)*(100-chWaterFlat)/100
						if choose(iChance, False, True):
							plot.setPlotType( PlotTypes.PLOT_LAND, True, True )
					if plot.isFlatlands():
						iChance = chFlatHill
						if choose(iChance, True, False):
							plot.setPlotType( PlotTypes.PLOT_HILLS, True, True )
					if plot.isHills():
						iChance = chHillPeak
						if choose(iChance, True, False):
							plot.setPlotType( PlotTypes.PLOT_PEAK, True, True )
				if temp==2:																			# mountainous
					if plot.isWater():
						plot.setPlotType( PlotTypes.PLOT_LAND, True, True )
#						sprint += "[MST] @ %3i,%2i: Water -> Land \n" % ( plot.getX(), plot.getY() )
					if plot.isFlatlands():
						iChance = (100-chFlatHill)*(100-chFlatHill)/100
						if choose( iChance, False, True ):
							plot.setPlotType( PlotTypes.PLOT_HILLS, True, True )
#							sprint += "[MST] @ %3i,%2i: Flat -> Hills \n" % ( plot.getX(), plot.getY() )
					if plot.isHills():
						if choose( chHillPeak, True, False ):
							plot.setPlotType( PlotTypes.PLOT_PEAK, True, True )
#							sprint += "[MST] @ %3i,%2i: Hills -> Peak \n" % ( plot.getX(), plot.getY() )
				if temp==1:																			# peaks and volcanos
					if plot.isWater():
						plot.setPlotType( PlotTypes.PLOT_LAND, True, True )
#						sprint += "[MST] @ %3i,%2i: Water -> Land \n" % ( plot.getX(), plot.getY() )
					if plot.isFlatlands():
						iChance = (100-chFlatHill)*(100-chFlatHill)/100
						if choose( iChance, False, True ):
							plot.setPlotType( PlotTypes.PLOT_HILLS, True, True )
#							sprint += "[MST] @ %3i,%2i: Flat -> Hills \n" % ( plot.getX(), plot.getY() )
					if plot.isHills():
						iChance = (100-chHillPeak)*(100-chHillPeak)/100
						if choose( iChance, False, True ):
							plot.setPlotType( PlotTypes.PLOT_PEAK, True, True )
#							sprint += "[MST] @ %3i,%2i: Hills -> Peak \n" % ( plot.getX(), plot.getY() )

		# pass 2 - make accessable
		sprint += "[MST] pass 2 - make accessable \n"
		chAccess2 = chAccess - 15 + chooseNumber( 30 )
		if chAccess2 > 0:
			for dx in range( minX, maxX + 1 ):
				for dy in range( minY, maxY + 1 ):
					iCount = numPlotNeighbors(dx, dy, PlotTypes.PLOT_PEAK, 1, None, False)

					if iCount == 8:
						eOpen = chooseNumber( DirectionTypes.NUM_DIRECTION_TYPES )
						pl = plotDirection( dx, dy, DirectionTypes(eOpen) )
						if not pl.isNone():
							if choose(chAccess2, True, False):
								pl.setPlotType( PlotTypes.PLOT_HILLS, True, True )
					elif iCount == 7:
						eOpen = chooseNumber( DirectionTypes.NUM_DIRECTION_TYPES )
						pl = plotDirection( dx, dy, DirectionTypes(eOpen) )
						if not pl.isNone():
							if choose(chAccess2/2, True, False):
								pl.setPlotType( PlotTypes.PLOT_HILLS, True, True )

		# pass 3 - change terrain
		sprint += "[MST] pass 3 - change terrain \n"
		for dx in range( minX, maxX + 1 ):
			for dy in range( minY, maxY + 1 ):
				temp = template[dy-minY][dx-minX]
				plot = plotXY( dx, dy, 0, 0 )
				if plot.isNone(): continue														# no plot -> ignore
				if temp == 1:
					if plot.isFlatlands():
						plot.setTerrainType( etGrass, True, True )
					else:
						plot.setTerrainType( etSnow, True, True )
				elif temp == 2:
					if plot.getTerrainType() == etSnow: continue
					iChance = chSnow
					if evalLatitude( plot ) < 25: iChance = 100 - chSnow
					if choose( iChance, True, False ):
						plot.setTerrainType( etSnow, True, True )
					else:
						plot.setTerrainType( etTundra, True, True )
				elif temp == 3:
					if plot.getTerrainType() == etSnow: continue
					if plot.getTerrainType() == etTundra: continue
					iChance = chTundra
					if evalLatitude( plot ) < 30: iChance = 100 - chTundra
					if choose( iChance, True, False ):
						plot.setTerrainType( etTundra, True, True )
					elif plot.getTerrainType() == etDesert:
						plot.setTerrainType( etPlains, True, True )
				if plot.isHills():											# no marsh on hills
					if plot.getTerrainType()==etMarsh:
						plot.setTerrainType( etGrass, True, True )

		# pass 5 - add rivers
		sprint += "[MST] pass 5 - add rivers \n"
		print sprint
		sprint = ""
		bSkip = False
		iCnt = 0
		for dx in range( minX, maxX + 1 ):
			for dy in range( minY, maxY + 1 ):
				if bSkip:
					bSkip = False
					continue
				elif iCnt > 2:
					break
				plot = plotXY( dx, dy, 0, 0 )
				if plot.isNone(): continue														# no plot -> ignore
				if not riverMaker.hasCoastAtSECorner( plot ):
					if choose( chRiver, True, False ):
						rList = []
						if (dy<=0) and (dx==0):
							riverMaker.buildRiver( plot, True, choose(50,ecEast,ecSouth), riverList=rList )
						elif (dy<=0) and (dx==-1):
							riverMaker.buildRiver( plot, True, choose(50,ecWest,ecSouth), riverList=rList )
						elif (dy>=2) and (dx==0):
							riverMaker.buildRiver( plot, True, choose(50,ecEast,ecNorth), riverList=rList )
						elif (dy>=2) and (dx==-1):
							riverMaker.buildRiver( plot, True, choose(50,ecWest,ecNorth), riverList=rList )
						elif (dy==1) and (dx==0):
							riverMaker.buildRiver( plot, True, ecEast, riverList=rList )
						elif (dy==1) and (dx==-1):
							riverMaker.buildRiver( plot, True, ecWest, riverList=rList )
						bSkip = True
						iCnt += 1
						sprint += riverMaker.outRiverList( rList, "[MST]" )
						if self.dentRivers==None: self.dentRivers = []
						self.dentRivers.append( rList )
		print sprint
		return namePlot


	# add names and boni to regions
	def addRegionExtras( self ):
		print "[MST] ===== MapRegions:addRegionExtras()"
		if len( self.lostIsleList ) > 0: self.addLostIsleExtras()
		if len( self.bogList ) > 0: self.addBigBogExtras()
		if len( self.dentList ) > 0: self.addBigDentExtras()

	###############
	### Helpers ###
	###############

	# adjust BigDent for mapsize
	def adjustBigDentsTemplate( self, bRandom=False ):
		print "[MST] ======== MapRegions:adjustBigDentsTemplate()"
		siz = iif( bRandom, chooseNumber( 7 ), MAP.getWorldSize() )
		self.dentTemplate = self.bigDentTemplate.copy()
		if (siz < 5) and choose(80, True, False):			# duel - large
			del self.dentTemplate[ 5 ]
		if (siz < 4) and choose(80, True, False):			# duel - standard
			del self.dentTemplate[ 3 ]
		if (siz < 3) and choose(80, True, False):			# duel - small
			for i in self.dentTemplate.keys(): del self.dentTemplate[i][8]
		if (siz < 2) and choose(80, True, False):			# duel - tiny
			for i in self.dentTemplate.keys(): del self.dentTemplate[i][6]


	# rotate template dictionary; steps are 0,1,2,3 clockwise
	# corresponding to rotations by 0,90,180,270 degrees
	# - new keys are integers starting with 0
	# - returns new template, old template is not touched
	def rotateTemplate( self, tempDict, steps ):
		#print "[MST] ======== MapRegions:rotateTemplate()"
		steps = steps % 4
		if steps == 0: steps = 4		# to ensure that new template has continuous integers as keys
		temp = tempDict.copy()
		keyList = temp.keys()
		keyList.sort()
		maxLen = 0
		for k in keyList: maxLen = max( maxLen, len( tempDict[k] ) )
		if maxLen > 0:
			for i in range( steps ):
				newTemp = {}
				for v in range( maxLen-1, -1, -1 ):
					newTemp[ v ] = []
					for k in keyList:
						val = temp[k]
						if v >= len( val ):
							elem = None
						else:
							elem = val[v]
						newTemp[v].append( elem )
				temp = newTemp
				maxLen = len( keyList )
				keyList = temp.keys()
				keyList.sort()
		return temp

	# add names, boni, improvements and other late extras to region: 'theLostIsle'
	def addLostIsleExtras(self):
		print "[MST] ======== MapRegions:addLostIsleExtras()"

		lostIsleNames = [ "Niflheim", "Atlantis", "Numenor",
		                  "Minos", "Rillanon", "Ensligholm" ]
		lostIsleNames_Pfall = [ "Alien HQ", "Command Center", "City of Light", "Atlantis Command" ]

		sprint = ""
		for x0, y0, pList, bAliens in self.lostIsleList:
			plDone = []

			# city ruins
			printList(pList, "Lost Isle Tiles:", prefix="[MST] ")
			pListCoast = []
			for x, y in pList:
				plotX = GetPlot(x, y)
				if not plotX.isPeak() and not plotX.isWater() and plotX.isCoastal():
					pListCoast.append(plotX)
			plot = chooseListElement(pListCoast)
			if plot == None:
				print "[MST] Unable to place City Ruins"
				continue
			cx = plot.getX()
			cy = plot.getY()
			plot.setBonusType(-1)
			plot.setImprovementType(eiCityRuins)
			sprint += "[MST] Placed City Ruins @ (%r,%r)\n" % (cx, cy)
			bonusBalancer.balanceBoniAtPlot(plot, maxPass = 2)

			# river
			rList = []
			riverDirs = riverMaker.checkRiverEnd(plot, bDownFlow=True)
			rDir = chooseListElement(riverDirs)
			if rDir != None:
				riverMaker.buildRiver(plot, False, rDir, riverList=rList) # upFlow
				print riverMaker.outRiverList(rList, "[MST] ")
			else: print "[MST] Unable to build river"

			# improve terrain
			pListTer = [GetPlot(x,y) for x,y in pList if not GetPlot(x,y).isPeak()]
			for pl in pListTer:
				if pl.getTerrainType() == etSnow:
					newTerrain = chooseMore((20, etGrass), (85, etTundra), (100, etSnow))
					if pl.isFreshWater():
						newTerrain = chooseMore((25,etMarsh), (75,newTerrain), (100,etGrass))
					pl.setTerrainType(newTerrain, True, True)
				elif pl.getTerrainType() == etTundra:
					if pl.isFreshWater():
						newTerrain = chooseMore((30,etGrass), (60,etMarsh), (100,etTundra))
					else:
						newTerrain = chooseMore((30,etGrass), (100,etTundra))
					pl.setTerrainType(newTerrain, True, True)
				elif pl.getTerrainType() == etDesert:
					newTerrain = choose(20, etGrass, etPlains)
					if not pl.isFreshWater() and not pl.isCoastal():
						newTerrain = choose(25, etDesert ,newTerrain)
					pl.setTerrainType(newTerrain, True, True)

			# place boni and work them
			pListBoni = [GetPlot(x,y) for x,y in pList if (x,y) != (cx,cy) and not GetPlot(x,y).isPeak()]
			bonList = [ebGold, ebSilver, [ebCopper, ebIron], ebOil, [ebSheep, ebCow], ebCoal, ebSulphur, ebUranium]
			chance = iif(bAliens, 90, 60)
			# try for four landbased boni on the island
			for i in range(4):
				# get bonus
				bon = chooseListPop( bonList )
				if type(bon)==type([]):
					bon = chooseListElement( bon )
				if bon < 0: continue
				# get plot for bonus
				pBoni = [ pl for pl in pListBoni
				             if ( pl.getBonusType(-1) == -1 ) and
				                ( pl.canHaveBonus(bon, False) ) ]
				pl = chooseListPop( pBoni )
				if pl == None: continue			# bonus forbidden on island
				pl.setBonusType( bon )
				# work placed boni
				if choose( chance, True, False ):
					bonString = GC.getBonusInfo(bon).getType()
					bonTech = GC.getBonusInfo(bon).getTechCityTrade()
					bonEra = 0
					if bonTech > -1:
						bonEra = GC.getTechInfo(bonTech).getEra()
					if bonEra < 0: bonEra = 0
					eraString =	GC.getEraInfo(bonEra).getType()
					sprint += "[MST] Improve %s @ (%r,%r) - Era: %s\n" % (bonString,pl.getX(),pl.getY(),eraString)
					# find possible improvements for boni
					impList = [ (bonEra, imp) for imp in range(GC.getNumImprovementInfos())
													  if pl.canHaveImprovement(imp, TeamTypes.NO_TEAM, True) and
													     GC.getImprovementInfo(imp).isImprovementBonusMakesValid(bon) ]
					if len(impList) == 0:
						sprint += "[MST] No improvement found for %s @ (%r,%r)\n" % (bonString,pl.getX(),pl.getY())
						continue
					impList.sort()
					if bAliens:
						bonEra, imp = impList[ len(impList)-1 ]		# highest tech
					else:
						bonEra, imp = impList[ 0 ]
						if bonEra > 1:
							sprint += "[MST] Civ to primitive to work %s @ (%r,%r)\n" % (bonString,pl.getX(),pl.getY())
							continue												# ancient and classic tech only
					impString = GC.getImprovementInfo(imp).getType()
					pl.setImprovementType( imp )
					sprint += "[MST] Placed %s on %s @ (%r,%r)\n" % (impString,bonString,pl.getX(),pl.getY())
			print sprint

			# city area - improvements
			chance = iif(bAliens, 80, 50)
			featList = [efForest, efJungle]
			impTuple = ( eiFarm, eiMine )
			if bAliens: impTuple = ( eiWindmill, eiFarm, eiMine )
			sprint = ""
			pListCity = [ GetPlot(x,y) for x,y in pList
			              if plotDistance(x, y, cx, cy) == 1 ]

			for pl in pListCity:
				for imp in impTuple:
					if pl.isWater(): continue
					if pl.getBonusType(-1) > -1: continue
					if pl.getFeatureType() in featList:
						pl.setFeatureType( FeatureTypes.NO_FEATURE, -1 )
					if pl.canHaveImprovement( imp, TeamTypes.NO_TEAM, True ):
						if choose( chance, True, False ): continue
						impString = GC.getImprovementInfo(imp).getType()
						pl.setImprovementType( imp )
						sprint += "[MST] Placed %s @ (%r,%r)\n" % (impString,pl.getX(),pl.getY())

			# city area - Goody Hut
			for pl in pListCity:
				if pl.isWater(): continue
				if pl.isPeak(): continue
				if choose(33,True,False): continue
				pl.setImprovementType( eiGoody )
				sprint += "\n[MST] Goody Hut placed @ (%r,%r)\n\n" % (pl.getX(),pl.getY())
				break

			# city area - roads
			chance = iif( bAliens, 90, 60 )
			eRoad = GC.getInfoTypeForString('ROUTE_ROAD')
			if bAliens: eRoad = GC.getNumRouteInfos() - 1
			roadString = GC.getRouteInfo(eRoad).getType()
			for pl in pListCity:
				if pl.isWater(): continue
				if pl.isPeak(): continue
				if pl.getBonusType(-1) >= 0:
					pl.setRouteType( eRoad )
				else:
					if choose( chance, False, True ): continue
					pl.setRouteType( eRoad )
				sprint += "[MST] Placed %s @ (%r,%r)\n" % (roadString,pl.getX(),pl.getY())

			# isle area - roads
			chance = iif( bAliens, 66, 25 )
			eRoad = GC.getInfoTypeForString('ROUTE_ROAD')
			roadString = GC.getRouteInfo(eRoad).getType()
			for x,y in pList:
				pl = GetPlot(x,y)
				if pl in pListCity: continue
				if pl.isWater(): continue
				if pl.isPeak(): continue
				if pl.getBonusType(-1) >= 0:
					pl.setRouteType( eRoad )
				else:
					if choose( chance, False, True ): continue
					pl.setRouteType( eRoad )
				sprint += "[MST] Placed %s @ (%r,%r)\n" % (roadString,pl.getX(),pl.getY())

			# city area - fishes
			wList = []
			bonList = [ ebFish, ebCrab ]
			pListWater = [ GetPlot(x,y) for x in range(cx-1,cx+2) for y in range(cy-1,cy+2) if GetPlot(x,y).isWater()]
			pl = chooseListElement( pListWater )
			if pl == None:
				sprint += "\n[MST] No water-boni placed"
			else:
				bon = chooseListElement( bonList )
				pl.setBonusType( bon )
				bonString = GC.getBonusInfo(bon).getType()
				sprint += "\n[MST] Placed %s @ (%r,%r)" % (bonString,pl.getX(),pl.getY())

			# name
			nam = chooseListPop( lostIsleNames )
			CyEngine().removeLandmark( GetPlot(x0,y0) )			# remove 'Lost Isle' sign
			print sprint

	# add names, boni and other late extras to region: 'theBigBog'
	def addBigBogExtras( self ):
		print "[MST] ======== MapRegions:addBigBogExtras()"
		bDarkLake = False
		bDarkMoor = False
		sprint = ""
		for reg in self.regionNames:
			# BigBog Names
			if reg[0] == 'Bog':
				plot = reg[2]
				if reg[1] == "Big Bog":
					if plot.isWater():
						if choose(33, True, False):
							plot.setBonusType( chooseMore( (66,ebFish), (100,ebClam) ) )
				elif reg[1] == "Darkwater":
					if plot.isWater():
						if not bDarkLake:
							bDarkLake = True
							if choose( 20, True, False ):
								plot.setBonusType( chooseMore( (66,ebFish), (100,ebClam) ) )
					elif not bDarkMoor:
						bDarkMoor = True
		print sprint
		return

	# add names, boni and other late extras to region: 'theBigDent'
	def addBigDentExtras( self ):
		print "[MST] ======== MapRegions:addBigDentExtras()"
		sprint = ""
		for reg in self.regionNames:
			# BigDent Names
			if reg[0] == 'Dent':
				dDict = reg[1]
				dPlot = dDict['Plot']
				if dPlot.isFlatlands():
					if dDict['Flat'][1] != None:
						dPlot.setTerrainType( dDict['Flat'][1], True, True )
					if dDict['Flat'][2] != None:
						dPlot.setFeatureType( dDict['Flat'][2], -1 )
					if dDict['Flat'][3] != None:
						if dPlot.getBonusType(-1) == -1:
							if bonusBalancer.isBonusValid( dDict['Flat'][3], dPlot, True, True, True ):
								bonusBalancer.placeBonus( dPlot, dDict['Flat'][3] )
					if choose(66,True,False):
						dPlot.setImprovementType( eiGoody )
						sprint += "[MST] Goody Hut placed @ (%r,%r)" % (dPlot.getX(),dPlot.getY())
				elif dPlot.isHills():
					if dDict['Hills'][1] != None:
						if dPlot.getBonusType(-1) == -1:
							if bonusBalancer.isBonusValid( dDict['Hills'][1], dPlot, True, True, True ):
								bonusBalancer.placeBonus( dPlot, dDict['Hills'][1] )
		print sprint

	# check, if regioncenter is far enough away from other regions to be placed at plot
	def regionCheck( self, plot ):
#		print "[MST] ======== MapRegions:regionCheck()"
		bValid = True
		iMinDist = self.regionDist + 3 * MAP.getWorldSize()
		x0 = plot.getX()
		y0 = plot.getY()
		for dx,dy in self.regionList:
			iDist = plotDistance( x0, y0, dx, dy )
			if iDist < iMinDist:
				bValid = False
				break
		return bValid

	# check thet plot is not within dist plots from the edges
	def wrapCheck( self, plot, dist=3 ):
#		print "[MST] ======== MapRegions:wrapCheck()"
		x,y = coordByPlot( plot )
		if not MAP.isWrapX():
			if (x < dist) or (x > (iNumPlotsX-1-dist)): return False
		if not MAP.isWrapY():
			if (y < dist) or (y > (iNumPlotsY-1-dist)): return False
		return True

	# delete plots which can't be bog centers
	def deleteNonBogPlots( self, plotList ):
		for inx in range( len(plotList)-1,-1,-1 ):
			pl = plotList[inx]
			if not pl.isWater() and pl.isCoastal():
				del plotList[inx]
				continue

			iLat = evalLatitude(pl) # 0..90
			bZone = marshMaker.iMarshHotBottom<=iLat and iLat<=marshMaker.iMarshHotTop or marshMaker.iMarshColdBottom<=iLat and iLat<=marshMaker.iMarshColdTop
			if bZone:
				if not MAP.isWrapX() and (pl.getX() < 3 or pl.getX() > iNumPlotsX-4):
					del plotList[inx]

				elif not MAP.isWrapY() and (pl.getY() < 3 or pl.getY() > iNumPlotsY-4):
					del plotList[inx]
			else:
				del plotList[inx]

	# delete plots which can't be dent centers
	def deleteNonDentPlots( self, plotList ):
		for inx in range(len(plotList)-1, -1, -1):
			pl = plotList[inx]
			if not pl.isWater() and pl.isCoastal():
				del plotList[inx]
				continue

			if not MAP.isWrapX() and (pl.getX() < 3 or pl.getX() > iNumPlotsX-4):
				del plotList[inx]
			elif not MAP.isWrapY() and (pl.getY() < 3 or pl.getY() > iNumPlotsY-4):
				del plotList[inx]

	# change to bog-terrain
	def changeBogTerrain( self, plot, temp ):
		sprint = ""
		if temp==0: return sprint

		# change terrain
		ter = plot.getTerrainType()
		newTer = ter
		if temp == 5:										# maybe hill
			if ter==etSnow:
				newTer = etTundra
			elif ter==etDesert:
				newTer = etPlains
		if temp == 4:										# probably grass
			if ter == etSnow or ter == etTundra:
				newTer = chooseMore( (60,etGrass), (90,etMarsh), (100,etTundra)  )	# --,60,30,10
			elif ter==etDesert:
				newTer = chooseMore( (70,etGrass), (80,etMarsh), (100,etPlains) )		# 20,70,10,--
			else:
				newTer = chooseMore( (60,etGrass), (90,etMarsh), (100,etPlains) )		# 10,60,30,--
		elif temp == 3:									# probably marsh
			if ter==etSnow or ter==etTundra:
				newTer = chooseMore( (30,etGrass), (85,etMarsh), (100,etTundra) )		# --,30,55,15
			elif ter==etDesert:
				newTer = chooseMore( (40,etGrass), (80,etMarsh), (100,etPlains) )		# 20,40,40,--
			else:
				newTer = chooseMore( (30,etGrass), (85,etMarsh), (100,etPlains) )		# 15,30,55,--
		elif temp == 2:									# marshy and flat
			if ter==etSnow or ter==etDesert:
				newTer = chooseMore( (30,etGrass), (100,etMarsh) )							# --,30,70,--
			else:
				newTer = chooseMore( (15,etGrass), (100,etMarsh) )							# --,15,85,--
		elif temp == 1:									# possibly lake
				newTer = etMarsh
		plot.setTerrainType( newTer, True, True )
		sprint += "[MST] @ %3i,%2i: %s -> %s (temp:%i) \n" % \
		   (plot.getX(),plot.getY(),capWords(GC.getTerrainInfo(ter).getType()[8:]),capWords(GC.getTerrainInfo(newTer).getType()[8:]),temp)
		return sprint

	# find empty ocean and put 'Lost Isle' there
	def createLostIsle( self, minDist, bAliens ):
		print "[MST] ======== MapRegions:createLostIsle()"
		dist = minDist + iif(bAliens,1,0)
		# find empty ocean
		aList = []
		for inx in range(MAP.numPlots()):
			x,y = coordByIndex(inx)
			if not MAP.isWrapX:
				if (x<4) or (x>=(iNumPlotsX-4)): continue
			if not MAP.isWrapY:
				if (y<4) or (y>=(iNumPlotsY-4)): continue
			if isHighSeas(x, y, dist, bWrap=True, treshold=2):
				aList.append((x, y))
		if not aList:
			return []

		print "[MST] Found %i possible plots for 'Lost Isle'" % (len(aList))
		fx, fy = chooseListElement(aList)

		# adjust template
		rot = chooseNumber( 4 )
		template = self.rotateTemplate( self.lostIsleTemplate, rot )
		printDict( self.lostIsleTemplate, "lostIsleTemplate:", prefix="[MST] " )
		printDict( template, "template rotated by %r degrees:" % (rot*90), prefix="[MST] " )

		# put isle into ocean by template
		return self.stampLostIsle(template, (fx,fy))

	# create plots and terrain for 'Lost Isle'
	def stampLostIsle(self, template, center):
#		print "[MST] ======== MapRegions:stampLostIsle()"
		x0, y0 = center
		stampList = []
		dx, dy = (2,2)
		tWidth = len(template[0])
		tHeight = len(template.keys())
		terrGen = MGU.TerrainGenerator()
		featGen = MGU.FeatureGenerator()
		sprint = ""
		# add island
		for x in range(tWidth):
			for y in range(tHeight):
				temp = template[y][x]
				fx = x0 - dx + x
				if (fx < 0) or (fx >= iNumPlotsX): continue
				fy = y0 - dy + y
				if (fy < 0) or (fy >= iNumPlotsY): continue
				tPlot = GetPlot(fx, fy)

				# get target plot_type, terrain, feature
				pType = PlotTypes.PLOT_OCEAN
				if temp == 0:
					pType = chooseMore((10, PlotTypes.PLOT_LAND), (100, PlotTypes.PLOT_OCEAN))
				elif temp == 1:
					pType = chooseMore((20, PlotTypes.PLOT_HILLS), (95, PlotTypes.PLOT_LAND), (100, PlotTypes.PLOT_OCEAN))
				elif temp == 2:
					pType = chooseMore((20, PlotTypes.PLOT_LAND), (95, PlotTypes.PLOT_HILLS), (100, PlotTypes.PLOT_PEAK))
				elif temp == 3:
					pType = chooseMore((20, PlotTypes.PLOT_LAND), (50, PlotTypes.PLOT_HILLS), (100, PlotTypes.PLOT_PEAK))
				#sprint += "[MST] xy (%r,%r), temp: %r, pType %r\n" % (fx, fy, temp, pType)

				# set target plot_type, terrain, feature
				tPlot.setPlotType(pType, False, False)
				pTerr = terrGen.generateTerrainAtPlot(fx, fy)
				tPlot.setTerrainType(pTerr, False, False)
				featGen.addFeaturesAtPlot(fx, fy)
				if pType != PlotTypes.PLOT_OCEAN:
					stampList.append((fx,fy))
		print sprint

		# add coast
		lFishes = [ebClam, ebCrab, ebFish, ebShrimp]
		for x,y in stampList:
			lCoast = [ GetPlot(fx,fy) for fx in range(x-1,x+2) for fy in range(y-1,y+2) if GetPlot(fx,fy).isWater() ]
			for pl in lCoast:
				pl.setTerrainType( etCoast, False, False)
				# add fishes
				if choose(16, True, False):
					bon = chooseListElement(lFishes)
					pl.setBonusType(bon)

		MAP.recalculateAreas()
		return stampList

###############################################################################################################
### CLASS MapRegions END
###############################################################################################################
mapRegions = MapRegions()


#########################################################################################################
########## CLASS FeaturePlacer - put mod-dependent features on the map
#########################################################################################################
##### idea and most of placeCrystalPlains() by Operas
##### 'Orbis-friendly map scripts' --> http://forums.civfanatics.com/showthread.php?t=321920
#########################################################################################################
# placeKelp( chKelp=20, bAll=False, bLakes=False )
# placeHauntedLands( chHaunted=6 )
# placeCrystalPlains( chCrystal=25 )
#########################################################################################################
class FeaturePlacer:

	# put kelp on coast of continents; if not bAll: only near 2nd biggest continent
	def placeKelp( self, chKelp=20, bAll=False, bLakes=False ):
		print "[MST] ===== FeaturePlacer:placeKelp()"
		cnt = 0
		if bAll:
			# all continents
			for inx in range( MAP.numPlots() ):
				pl = MAP.plotByIndex( inx )
				if pl.isWater():
					if pl.isLake() and ( not bLakes ): continue			# lakes allowed?
					ter = pl.getTerrainType()
					if ter == etCoast:
						if pl.getFeatureType() == -1:
							if choose( chKelp, True, False ):
								pl.setFeatureType( efKelp, -1 )
								cnt += 1
		else:
			# only 2nd largest continent
			minContinent = 10
			areaList = getBigAreas( 2, False, None, minContinent )			# [ num, id, plotlist ]
			if len( areaList ) > 0:
				continentPlots = areaList[ len(areaList)-1 ][ 2 ]
				coastPlots = getContinentCoast( continentPlots, bLakes )
				for pl in coastPlots:
					if pl.getFeatureType() == -1:
						if choose( chKelp, True, False ):
							pl.setFeatureType( efKelp, -1 )
							cnt += 1
		if cnt>0: print "[MST] %i Kelp placed" % ( cnt )

	def placeHauntedLands(self, chHaunted=6): return

	def placeCrystalPlains(self, chCrystal=25): return

###############################################################################################################
### CLASS FeaturePlacer END
###############################################################################################################
featurePlacer = FeaturePlacer()


#########################################################################################################
########## CLASS BonusBalancer - balance starting-plot boni and place missing boni
#########################################################################################################
### The original BonusBalancer class from Warlords in CyMapGeneratorUtil.py
###   uses two tuples, which are defined within the class:
###   - resourcesToBalance, which the class tries to add for each player near the starting-plot,
###     if BonusBalancer.normalizeAddExtras() is called in normalizeAddExtras() and
###     with the resourcesToBalance will not be placed anywhere on the map
### This BonusBalancer class:
###   - places all boni in the default way first
###   - eliminates most randomly placed balanced resources
###     - but eliminates fewer balanced resources from unsettled continents (give Barbarian Nations a chance)
###   - places missing boni:
###     (boni which were not placed by addBonus(), or which were only placed once,
###      instead of the three or more expected placements by XML definition)
###      - if neccessary change from forest       to jungle, if latitude < 30
###      - if neccessary change from plains       to desert, if latitude < 45
###      - if neccessary change from grass/marsh  to tundra, if latitude > 60
###   	- if missing boni are found, eliminates a few of the most numerous resources to make room
###   - makes sure there is at least one grain or lifestock resource within city-cross range
###	  (tries to place a food resources which are already present in the area,
###      in that way different continents should have different food resources)
###     *** NOT Implemented yet ***
###   - tries to move mineralBoni [Copper,Iron,Mithril,Uranium,Silver,Coal] from flatland to hills
###   - places additional boni only if they are not already available near starting-plot
###   - tries to place those boni into the same area as starting-plot first
###   - uses different balanced resources
###   - has capability to add mod-specitic resources to balance via BonusBalancer.normalizeAddExtras()
###   - has capability to add mod-specitic resources to eliminate via BonusBalancer.normalizeAddExtras()
###   - increases the range for the normalizing process by one from 6 to 8 around starting-plot
###     (may incease the range by one more, if there are not enough free plots)
###
###   Note: The wide distribution of strategic ressources means that those ressources are somewhere in
###         the vicinity of the starting-plot, but not necessarily nearer to you than to your neighbor.
###         In fact on worlds with many civilizations, they may well crop up behind your neighbor.
#########################################################################################################
# initialize(self, bBalanceOnOff=True, bMissingOnOff=True, bMineralsOnOff=True, bWideRange=False )
# normalizeAddExtras( *lResources )
# bValid = isBonusValid( eBonus, pPlot, bIgnoreUniqueRange, bIgnoreOneArea, bIgnoreAdjacent )
# moveMinerals( lMineralsToMove=None )
# --- private ---
# reduceBalancedBoni()
# addMissingBoni()
# boniMissing, boniFound, freePlots = checkAllBoniPlaced()
# freePlots = reduceNumerousBoni( boniFound, freePlots )
# boniMissing = placeMissingBoni( boniMissing, freePlots )
# placeBonus( plot, iBonus )
# iVariety = transformForest2Jungle( plot )
# txtMessage = transformJungleNeighbor( plot )
# txtMessage = transformTerrainNeighbor( plot, eFrom, eTo, sTxt )
# balanceStrategicBoni()
# plBoniCnt = balanceBoniAtPlot( start_plot, sprint="" )
# boniInRange = getBoniInRange( x, y, areaID, ran, player )
# freePlots = getFreePlots( x, y, areaID, ran )
# iDesire = calcNumBoniToAdd( iBonus )
#########################################################################################################
class BonusBalancer:
	# class variables
	# Note: The first bonus will be ignored about half the time,
	#       not all players will have it near their starting-plot
	resourcesToBalance   = ( 'BONUS_BAUXITE_ORE', 'BONUS_OIL', 'BONUS_HORSE', 'BONUS_URANIUM', 'BONUS_IRON_ORE', 'BONUS_COPPER_ORE', )
	mineralsToMove       = ( 'BONUS_COPPER_ORE', 'BONUS_IRON_ORE', 'BONUS_SILVER_ORE', 'BONUS_GOLD_ORE', 'BONUS_URANIUM' )

	def initialize(self, bBalanceOnOff=True, bMissingOnOff=True, bMineralsOnOff=True, bWideRange=False ):
		print "[MST] ===== BonusBalancer:initialize( %r, %r, %r, %r )" % (bBalanceOnOff, bMissingOnOff, bMineralsOnOff, bWideRange)
		GAME = GC.getGame()

		self.bBalance  = bBalanceOnOff
		self.bMissing  = bMissingOnOff
		self.bMinerals = bMineralsOnOff
		self.spRange = 5																# max plotDistance around startingPlot
		self.iEliminate  = 50														# elimination percentage before balancing:
		self.iEliminate -= 4 * MAP.getWorldSize()								# smaller percentage for bigger worlds
		self.iEliminate += GAME.countCivPlayersEverAlive() * 2	# and bigger percentage for more players
		self.iEliminate = min( self.iEliminate, 80 )
		self.iEliminate = max( self.iEliminate, 20 )

		# bigger worlds, less players - balanced boni spread thinner
		if bWideRange:						# spread balanced boni on a wider range around the starting plots
			self.spRange += 2
		if MAP.getWorldSize() > 2:		# standard+
			if GAME.countCivPlayersEverAlive() < 12:
				self.spRange += 1
		if MAP.getWorldSize() > 3:		# large+
			if GAME.countCivPlayersEverAlive() < 15:
				self.spRange += 1
		if MAP.getWorldSize() > 4:		# huge+
			if GAME.countCivPlayersEverAlive() < 18:
				self.spRange += 1
		if MAP.getWorldSize() > 5:		# giant+
			if GAME.countCivPlayersEverAlive() < 21:
				self.spRange += 1

		# balanced resources depending on mod
		if not self.bBalance:
			self.resourcesToBalance = ('', )
		else:
			self.resourcesToBalance = ('BONUS_BAUXITE_ORE', 'BONUS_OIL', 'BONUS_HORSE', 'BONUS_URANIUM', 'BONUS_IRON_ORE', 'BONUS_COPPER_ORE',)

	# organize resourceLists, make areaList, place missing boni and balance starting-plots
	def normalizeAddExtras(self, *lResources):
		print "[MST] ===== BonusBalancer:normalizeAddExtras()"

		sprint  = "[MST] BonusBalancer " + iif( self.bBalance, "active", "deactivated") + " and "
		sprint += " MissingBoniPlacer " + iif( self.bMissing, "active", "deactivated") + " and "
		sprint += " MineralMover " + iif( self.bMinerals, "active", "deactivated")
		print sprint

		# put additional boni into resourcesToBalance if preceded by a '-'
		sprint = ""
		if self.bBalance:
			# add given additional resources to balance
			if lResources:
				newRes = list(lResources)
				print "[MST] Extra resources: %r" % ( newRes )
				balResources = list( self.resourcesToBalance )
				if balResources[0] == '': del balResources[0]

				for res in newRes:
					if res[0]=='-':
						if isBonus( res[1:] ):
							removeListElement( balResources, res[1:] )
					elif not res in balResources and isBonus(res):
						balResources.append( res )
				self.resourcesToBalance = tuple( balResources )

			sprint += "[MST] Resources to balance:   %r \n" % ( self.resourcesToBalance, )
		if self.bMinerals:
			sprint += "[MST] Minerals to move:       %r \n" % ( self.mineralsToMove, )
		print sprint

		# make lists of relevant areas
		minContinentPlots = 12
		MAP.recalculateAreas()
		self.areas = GC.getMap().areas()
		self.continentArea = []
		self.startArea = []
		for area in self.areas:
			if area.getNumStartingPlots()>0: self.startArea.append( area.getID() )
			if area.getNumTiles()>=minContinentPlots: self.continentArea.append( area.getID() )

		# proceed step by step
		self.cnt = 0
		mapStats.showContinents( "", minContinentPlots )			# before
		if self.bBalance:  self.reduceBalancedBoni()
		if self.bMissing:  self.addMissingBoni()
#		if self.bBalance:  self.balanceFarmingBoni()					# perhaps in the next version, not sure it's needed
		if self.bBalance:  self.balanceStrategicBoni()
		if self.bMinerals: self.moveMinerals( None, False )
		mapStats.showContinents( "", minContinentPlots )			# after

	# return True, if bonus can be placed on plot
	def isBonusValid(self, eBonus, pPlot, bIgnoreUniqueRange, bIgnoreOneArea, bIgnoreAdjacent):
		iX, iY = pPlot.getX(), pPlot.getY()

		if (not bIgnoreOneArea) and GC.getBonusInfo(eBonus).isOneArea():
			if MAP.getNumBonuses(eBonus) > 0:
				if MAP.getArea(pPlot.getArea()).getNumBonuses(eBonus) == 0:
					return False
		if not bIgnoreAdjacent:
			for iI in range(DirectionTypes.NUM_DIRECTION_TYPES):
				pLoopPlot = plotDirection(iX, iY, DirectionTypes(iI))
				if not pLoopPlot.isNone():
					if (pLoopPlot.getBonusType(-1) != -1) and (pLoopPlot.getBonusType(-1) != eBonus):
						return False
		if not bIgnoreUniqueRange:
			uniqueRange = GC.getBonusInfo(eBonus).getUniqueRange()
			for iDX in range(-uniqueRange, uniqueRange+1):
				for iDY in range(-uniqueRange, uniqueRange+1):
					pLoopPlot = plotXY(iX, iY, iDX, iDY)
					if not pLoopPlot.isNone() and pLoopPlot.getBonusType(-1) == eBonus:
						return False
		return True

	# move mineral boni from flatland to neighboring hills
	# NOTE: this is usually called after balancing and may move the
	#       balanced resource up to 2 plots into the 'wrong' direction
	def moveMinerals( self, lMineralsToMove=None, bForceHills=False ):
		print "[MST] ===== BonusBalancer:moveMinerals()"

		if lMineralsToMove == None:
			lMinerals = self.mineralsToMove
		else:
			lMinerals = lMineralsToMove
		cntMove = 0
		cntHill = 0
		for inx in range( MAP.numPlots() ):
			plot = MAP.plotByIndex( inx )
			if not plot.isFlatlands(): continue
			iBonus = plot.getBonusType( -1 )
			if iBonus < 0: continue
			type_string = GC.getBonusInfo(iBonus).getType()
			if type_string in lMinerals:
				# find nearby hill
				lHills = []
				x, y = plot.getX(), plot.getY()
				dist = 2
				for dx in range( -dist, dist+1 ):
					for dy in range( -dist, dist+1 ):
						pl = plotXY( x, y, dx, dy )
						if pl.isNone(): continue
						if pl.isHills():
							neiBonus = pl.getBonusType( -1 )
							if neiBonus < 0:
								if self.isBonusValid( iBonus, pl, True, False, False ):
									lHills.append( pl )
									# 2nd chance for neighbors
									if (abs(dx)<2) and (abs(dy)<2):
										lHills.append( pl )
				# move bonus
				if len( lHills ) > 0:
					pl = chooseListElement( lHills )
					self.placeBonus( pl, iBonus )
					self.placeBonus( plot, -1 )
					cntMove += 1
				else:
					if bForceHills:
						plot.setPlotType( PlotTypes.PLOT_HILLS, True, True )
						cntHill += 1
		print "[MST] %i boni moved, %i hills created" % ( cntMove, cntHill )

	###############
	### Helpers ###
	###############

	# eliminate most balanced boni in starting-plot areas
	# - much lower elimination-chance on other continents
	# - no elimination-chance on water and small islands
	def reduceBalancedBoni(self):
		print "[MST] ======== BonusBalancer:reduceBalancedBoni()"
		iCnt = 0
		iChoice = 0
		for i in range( MAP.numPlots() ):
			pl = MAP.plotByIndex(i)
			iBonus = pl.getBonusType(-1)
			if iBonus>=0:
				type_string = GC.getBonusInfo(iBonus).getType()
				if type_string in self.resourcesToBalance:
					if pl.getArea() in self.startArea:
						iChoice = int(self.iEliminate)
					elif pl.getArea() in self.continentArea:
						iChoice = int(self.iEliminate / 4)
					else:
						continue
					if choose(iChoice,True,False):
						self.placeBonus(pl, -1)
						iCnt += 1
		print "[MST] Eliminated %2i wild boni of those to be balanced (%i%s chance)\n" % ( iCnt, self.iEliminate, "%%" )

	# add boni which were not placed, but should have been
	#   also place boni which were placed only once, but should have been placed 3 or more times
	def addMissingBoni( self ):
		print "[MST] ======== BonusBalancer:addMissingBoni()"
		# check if all boni were placed
		lMissing, lFound, lFree = self.checkAllBoniPlaced()
		if len(lMissing) == 0:
			print "[MST] No missing boni found \n"
		else:
			sprint = "[MST] Missing boni found: Not all resources have been randomly placed: \n"
			for i in lMissing: sprint += "[MST] %s wasn't placed randomly \n" % (GC.getBonusInfo(i[0]).getType())
			print sprint
			# reduce numerous boni, since some other boni are missing
			lFree = self.reduceNumerousBoni(lFound, lFree)
			# try to place those missing boni now
			lMissing = self.placeMissingBoni(lMissing, lFree)
			# print warning, if there are still missing boni
			if len(lMissing)>0:
				sprint = "[MST] WARNING! - not all missing boni could be placed \n"
				for i in lMissing: sprint += "[MST] %s is still missing \n" % (GC.getBonusInfo(i[0]).getType())
				print sprint
			else:
				print "[MST] All missing boni have been placed"

	# make lists with boni and their plots
	def checkAllBoniPlaced( self ):
		print "[MST] ======== BonusBalancer:checkAllBoniPlaced()"

		iPlayer = GC.getGame().countCivPlayersAlive()
		boniMissing = []
		boniFound = {}
		freePlots = []

		# make lists for placed boni and free plots
		for i in range(MAP.numPlots()):
			plot = MAP.plotByIndex(i)
			iBonus = plot.getBonusType(-1)
			if iBonus > -1:
				if iBonus in boniFound:
					boniFound[iBonus].append(i)
				else: boniFound[iBonus] = [i]

			elif not plot.isWater() and not plot.isPeak(): # no new water/peak boni
				freePlots.append(i)

		# make list for missing boni
		for i in range(GC.getNumMapBonuses()):
			iBonus = GC.getMapBonus(i)
			# find missing boni
			if iBonus not in boniFound:
				# should bonus placed on the map, if so try to place half
				iDesiredBoni = self.calcNumBoniToAdd(iBonus)
				if iDesiredBoni > 1:
					bonusName = capWords(GC.getBonusInfo(iBonus).getType()[6:])
					boniMissing.append( [iBonus, min(int((iDesiredBoni+1)/2),int(iPlayer/2)), bonusName] )
			# recheck single boni, if desired are >2 then place half
			elif len(boniFound[iBonus]) == 1:
				pl = MAP.plotByIndex(boniFound[iBonus][0])
				if not pl.isWater():
					iDesiredBoni = self.calcNumBoniToAdd( iBonus )
					if iDesiredBoni > 2:
						bonusName = capWords( GC.getBonusInfo(iBonus).getType()[6:] )
						boniMissing.append( [iBonus, min(int(iDesiredBoni/2),int(iPlayer/2)), bonusName] )

		return (boniMissing, boniFound, freePlots)


	# delete some of the most placed boni on land, ignore balanced boni
	def reduceNumerousBoni(self, boniFound, freePlots):
		print "[MST] ======== BonusBalancer:reduceNumerousBoni()"
		iPlayer = GC.getGame().countCivPlayersAlive()
		cnt = 0
		for iBonus, plots in boniFound.items():

			if not plots or MAP.plotByIndex(plots[0]).isWater():
				continue

			if iBonus not in self.resourcesToBalance:
				passes = 0
				iNumBonuses = len(plots)
				if iNumBonuses > iPlayer * 4: passes = 4
				elif iNumBonuses > iPlayer * 16/5: passes = 3
				elif iNumBonuses > iPlayer * 12/5: passes = 2
				elif iNumBonuses > iPlayer * 8/5: passes = 1

				if iNumBonuses > 20: passes += 3
				elif iNumBonuses > 16: passes += 2
				elif iNumBonuses > 12: passes += 1

				while passes > 0 and plots:
					passes -= 1
					inx = plots.pop(GC.getGame().getMapRand().get(iNumBonuses, "MapScriptTools.chooseListPop()" ))
					iNumBonuses -= 1
					self.placeBonus(MAP.plotByIndex(inx), -1)
					freePlots.append(inx)
					cnt += 1

		if cnt > 0:
			print "[MST] Eliminated %2i of the most plentiful boni" % (cnt)
			shufflePyList(freePlots, GC.getGame().getMapRand())
		return freePlots

	# place missing boni if at all possible
	def placeMissingBoni(self, boniMissing, freePlots):
		print "[MST] ======== BonusBalancer:placeMissingBoni()"
		if not boniMissing: return []

		iPlayer = GC.getGame().countCivPlayersAlive()
		shufflePyList(boniMissing, GC.getGame().getMapRand())

		# try and place missing boni
		# using several passes, increasingly ignoring conditions on placement
		# up to changing forest to jungle, plains to desert or grass/marsh to tundra
		sprint = ""
		sprint += "[MST] %2i boni missing: %r \n" % (len(boniMissing),boniMissing)
		sprint += "[MST] %4i free Plots \n" % (len(freePlots))
		bMissing = []
		for i in range(len(boniMissing)): bMissing.append( True )

		# this is a bit of a hack, but should work anyway
		# 8 passes
		for pass_num in range(8):
			if len(boniMissing)==0: break
			bIgnoreLatitude			= True
			bIgnoreUniqueRange		= (pass_num >= 1) and (pass_num != 4)
			bIgnoreOneArea				= (pass_num >= 2) and (pass_num != 4) and (pass_num != 5)
			bIgnoreAdjacent			= (pass_num >= 3) and (pass_num != 4) and (pass_num != 5) and (pass_num != 6)
			bCreateTerrainFeature	= (pass_num >= 4)

			lastRound = False
			while not lastRound:
				cnt = 0
				for i in range( len(boniMissing) ):
					misBonus = boniMissing[i]
					iBonus  = misBonus[0]
					iDesire = misBonus[1]
#					sprint += "[MST] Pass %i, %s need %i more \n" % (pass_num,GC.getBonusInfo(iBonus).getType(),iDesire)
					if iDesire>0:
						for j in range( len(freePlots) ):
							inx = freePlots[j]
							bJTrans = False
							bDTrans = False
							bTTrans = False
							fp = MAP.plotByIndex( inx )
							if bCreateTerrainFeature:
								bIgnoreLatitude = False
								if GC.getBonusInfo(iBonus).isFeature(efJungle) or GC.getBonusInfo(iBonus).isFeatureTerrain(efJungle):
									if fp.getFeatureType()==efForest and evalLatitude(fp)<30:
										iVar = self.transformForest2Jungle(fp)
										bJTrans = True
								elif GC.getBonusInfo(iBonus).isTerrain(etDesert) or GC.getBonusInfo(iBonus).isFeatureTerrain(etDesert):
									if fp.getTerrainType()==etPlains and evalLatitude(fp)<45:
										fp.setTerrainType( etDesert, True, True )
										bDTrans = True
								elif GC.getBonusInfo(iBonus).isTerrain(etTundra) or GC.getBonusInfo(iBonus).isFeatureTerrain(etTundra):
									if (fp.getTerrainType()==etGrass or fp.getTerrainType()==etMarsh) and (evalLatitude(fp)>60):
										fp.setTerrainType( etTundra, True, True )
										bTTrans = True
							if fp.canHaveBonus(iBonus, bIgnoreLatitude):
								if self.isBonusValid(iBonus, fp, bIgnoreUniqueRange, bIgnoreOneArea, bIgnoreAdjacent):
									self.placeBonus( fp, iBonus)
									misBonus[1] -= 1
									bMissing[i] = False
									cnt += 1
									del freePlots[j]
									if bJTrans:
										self.transformJungleNeighbor(fp)
									elif bDTrans:
										self.transformTerrainNeighbor(fp, etDesert, etPlains, 'Desert')
									elif bTTrans:
										self.transformTerrainNeighbor(fp, etTundra, etGrass, 'Tundra')
									break
							if bJTrans:
								fp.setFeatureType(efForest,iVar)
							elif bDTrans:
								fp.setTerrainType(etPlains,True,True)
							elif bTTrans:
								fp.setTerrainType(etGrass,True,True)

				# if all desired boni are placed, the missing bonus is erased from the list
				if len(boniMissing)>0:
					r = 0
					while r<len(boniMissing):
						misBonus = boniMissing[r]
						iBonus  = misBonus[0]
						iDesire = misBonus[1]
						if iDesire == 0:
							del boniMissing[r]
							sprint += "[MST] all %s placed \n" % ( GC.getBonusInfo(iBonus).getType() )
						else:
							r += 1
					sprint += "[MST] %2i boni still missing: %r \n" % ( len(boniMissing),boniMissing )

				# if missingBoni list is empty, return with success
				if len(boniMissing)==0:
					print sprint
					return boniMissing
				# if no boni placed, try next pass or return unsuccessfully
				if cnt==0:
					lastRound = True

		bMiss = False
		for m in bMissing: bMiss = (bMiss or m)
		if not bMiss: boniMissing = []
		print sprint
		return boniMissing

	# place bonus preserving the feature variety	(in forests)
	def placeBonus(self, plot, iBonus):
#		print "[MST] ======== BonusBalancer:placeBonus()"
		eFeature = plot.getFeatureType()
		bonusInfo = GC.getBonusInfo( iBonus )
		# temp save feature and variety
		featureVariety = -1
		if eFeature>=0:
			if GC.getFeatureInfo(eFeature).getNumVarieties()>1:
				featureVariety = plot.getFeatureVariety()
				plot.setFeatureType( FeatureTypes.NO_FEATURE, -1 )
		#place bonus
		plot.setBonusType( iBonus )
		self.cnt += iif( iBonus==-1, 1, -1 )
		#restore the feature if possible
		if featureVariety>=0:
			if (bonusInfo == None) or bonusInfo.isFeature(eFeature):
				plot.setFeatureType(eFeature,featureVariety)

	# change Forest to Jungle; return variety
	def transformForest2Jungle( self, plot ):
		eFeature = plot.getFeatureType()
		iVariety = plot.getFeatureVariety()
		if eFeature==efForest:
			plot.setFeatureType(efJungle,-1)
		return iVariety

	# add one more jungle plot to keep from getting lonely and strange
	def transformJungleNeighbor( self, plot ):
		x = plot.getX()
		y = plot.getY()
		sprint = ""

		pList = []
		for eCard in range( CardinalDirectionTypes.NUM_CARDINALDIRECTION_TYPES ):
			p = plotCardinalDirection( x, y, CardinalDirectionTypes(eCard) )
			if not p.isNone():
				if p.getFeatureType()==efJungle: return
				if p.getFeatureType()==efForest:
					if p.getTerrainType()==etGrass or p.getTerrainType()==etMarsh:
						if p.getBonusType(-1)<0:
							pList.append( p )
		if pList:
			shufflePyList(pList, GC.getGame().getMapRand())
			self.transformForest2Jungle(pList[0])
			sprint += "[MST] More Jungle created @ %i,%i \n" % (pList[0].getX(),pList[0].getY())
		return sprint

	# add one more plot of same terrain to keep from getting lonely and strange
	def transformTerrainNeighbor( self, plot, eFrom, eTo, sTxt ):
		x = plot.getX()
		y = plot.getY()
		sprint = ""

		pList = []
		for eCard in range( CardinalDirectionTypes.NUM_CARDINALDIRECTION_TYPES ):
			p = plotCardinalDirection( x, y, CardinalDirectionTypes(eCard) )
			if not p.isNone():
				if p.getTerrainType() == eTo: continue
				if p.getTerrainType() == eFrom:
					if p.getFeatureType() < 0:				# no features only
						if p.getBonusType(-1) < 0:
							pList.append( p )
		if pList:
			shufflePyList(pList, GC.getGame().getMapRand())
			pList[0].setTerrainType( eTo, True, True )
			sprint += "[MST] More %s created @ %i,%i \n" % (	sTxt, pList[0].getX(), pList[0].getY() )
		return sprint

	# make sure each player has at least one each of the resourcesToBalance in the vicinity
	def balanceStrategicBoni( self ):
		print "[MST] ======== BonusBalancer:balanceStrategicBoni()"
		# put boni in resourcesToBalance around starting-plots
		allBoniCnt = 0
		for i in range(GC.getMAX_PC_PLAYERS()):
			if (GC.getPlayer(i).isAlive()):
				sprint = "[MST] PLAYER %i \n" % (i)
				start_plot = GC.getPlayer(i).getStartingPlot()
				plBoniCnt = self.balanceBoniAtPlot( start_plot, i, sprint )
				allBoniCnt += plBoniCnt
		sprint  = "[MST] Boni placed for all players: %r\n\n" % (allBoniCnt)
		sprint += "[MST] %i %s boni placed than eliminated" % ( abs(self.cnt),iif(self.cnt<0,'more','less'))
		print sprint

	# balance boni for starting-plot
	def balanceBoniAtPlot( self, start_plot, player=-1, sprint="", maxPass=3 ):
		plBoniCnt = 0
		startx, starty = start_plot.getX(), start_plot.getY()
		# two passes: within own area, ignore area
		for iPass in [1,2]:
			if iPass==2: startArea = -1
			else: startArea = start_plot.getArea()

			sPlots = self.getFreePlots(startx, starty, startArea, self.spRange)
			boniList = self.getBoniInRange( startx, starty, startArea, self.spRange)			# list of boni found

			# increase range if not enough places were found
			if len(sPlots)<=( 2 * len(self.resourcesToBalance) ):
				sPlots = self.getFreePlots(startx, starty, startArea, self.spRange+1)
				boniList = self.getBoniInRange( startx, starty, startArea, self.spRange+1 )		# list of boni found
			boniList.sort()
			sprint += "[MST] boniList: %r \n" % (boniList)
			resources_placed = []
			for pass_num in [0,1,2,3]:
				if pass_num > maxPass: break
				bIgnoreUniqueRange	= pass_num >= 1
				bIgnoreOneArea			= pass_num >= 2
				bIgnoreAdjacent		= pass_num >= 3
				for iBonus in range(GC.getNumBonusInfos()):
					type_string = GC.getBonusInfo(iBonus).getType()
					if (iBonus not in resources_placed) and (iBonus not in boniList):
						if type_string in self.resourcesToBalance:
							# ignore first bonus in list 50% of the time; don't allow further passes
							if type_string == self.resourcesToBalance[0]:
								if iPass == 2: continue
								if pass_num > 0: continue
								if choose(50, True, False): continue
							# ignore second bonus in list 33% of the time; don't allow further passes
							if len(self.resourcesToBalance) > 1:
								if type_string == self.resourcesToBalance[1]:
									if iPass == 2: continue
									if pass_num > 0: continue
									if choose(33, True, False): continue
							for pl in sPlots:
								if pl.canHaveBonus(iBonus, True):
									if self.isBonusValid(iBonus, pl, bIgnoreUniqueRange, bIgnoreOneArea, bIgnoreAdjacent):
										self.placeBonus( pl, iBonus )
										plBoniCnt += 1
										resources_placed.append(iBonus)
										pDist = plotDistance( startx, starty, pl.getX(), pl.getY() )
										break # go to the next bonus
				sprint += "[MST] passes %i,%i: resources_placed %r \n" % (iPass,pass_num,resources_placed)
		boniList = boniList + resources_placed
		boniList.sort()
		resList = [ GC.getBonusInfo(i).getType() for i in boniList ]
		if player == -1:
			txt = "Boni: %r" % (boniList)
		else:
			txt = "Boni for player %r: %r" % (player,boniList)
		sprint += sprintList( resList, txt, rows = 3, prefix = "[MST] " ) + "\n"
		sprint += "[MST] Number of boni placed: %r\n\n" % (plBoniCnt)
		return plBoniCnt

	# make list of boni already in the same area near starting-plot
	def getBoniInRange(self, x, y, areaID, ran):
		cnt = 0
		boniInRange = []
		for dx in range(-ran,ran+1):
			for dy in range(-ran,ran+1):
				if plotDistance( x, y, x+dx, y+dy ) <= ran:
					pLoopPlot = plotXY(x, y, dx, dy)							# use build-in plotXY()
					dArea = pLoopPlot.getArea()
					if (areaID == dArea) or (areaID < 0):
						iBonus = pLoopPlot.getBonusType(-1)					# use build-in plotXY()
						if iBonus > -1:
							cnt += 1
							if (iBonus not in boniInRange):
								boniInRange.append( iBonus )
		return boniInRange

	# build a shuffled list of the plots in the same area near the starting plot
	def getFreePlots(self, x, y, areaID, ran):
		plots = []
		for dx in range(-ran,ran+1):
			for dy in range(-ran,ran+1):
				if plotDistance( x, y, x+dx, y+dy ) <= ran:
					pLoopPlot = plotXY(x, y, dx, dy) # use built-in plotXY()
					# no extra boni outside map
					if not pLoopPlot.isNone():
						dArea = pLoopPlot.getArea()
						if (areaID == dArea) or (areaID < 0):
							# no extra boni on peaks
							if not pLoopPlot.isPeak():
								# no extra boni on top of other boni
								if pLoopPlot.getBonusType(-1)<0:
									plots.append(pLoopPlot)
		if plots: shufflePyList(plots, GC.getGame().getMapRand())
		return plots # return list

	# calculate number of desired boni
	# - like CvMapGenerator::calculateNumBonusesToAdd(BonusTypes eBonusType) in CvMapGenerator.cpp
	def calcNumBoniToAdd(self, iBonus):
		bonusInfo = GC.getBonusInfo(iBonus)
		if bonusInfo.getPlacementOrder() < 0:
			return 0

		rand1 = chooseNumber( bonusInfo.getRandAppearance1() )
		rand2 = chooseNumber( bonusInfo.getRandAppearance2() )
		rand3 = chooseNumber( bonusInfo.getRandAppearance3() )
		rand4 = chooseNumber( bonusInfo.getRandAppearance4() )
		iBaseCount = bonusInfo.getConstAppearance() + rand1 + rand2 + rand3 + rand4

		bIgnoreLatitude = False
		iLandTiles = 0
		iNumPossible = 0
		if bonusInfo.getTilesPer() > 0:
			for i in range( iNumPlotsX*iNumPlotsY ):
				plot = MAP.plotByIndex(i)
				if plot.canHaveBonus( iBonus, bIgnoreLatitude ):
					iNumPossible += 1
			iLandTiles += iNumPossible / bonusInfo.getTilesPer()
		iPlayers = int(GC.getGame().countCivPlayersAlive() * bonusInfo.getPercentPerPlayer() / 100)
		iBonusCount = int( iBaseCount * (iLandTiles + iPlayers) / 100 )
		return max( 1, iBonusCount )

##########################################################################
########## CLASS BonusBalancer END
##########################################################################
bonusBalancer = BonusBalancer()


#########################################################################################################
########## CLASS RiverMaker - build additional rivers
#########################################################################################################
### Best used before addRivers().
### Otherwise the rivers are on the map, but sources and deltas sometimes look strange.
### riverLists have the form: [ riverID, [plot0,cardinalDirection0], [plot1,cardinalDirection1], ... ]
#########################################################################################################
# buildRiver( pStartPlot, bDownFlow=True, ecNext=None, ecOri=None, iThisRiverID=None, riverList=None )
# islandRivers( minIsle=6, maxIsle=50, areaID=None )
# buildRiversFromLake( lakeAreaID=None, chRiver=66, nRivers=1, minLake=1 )
# sList = outRiverList( riverList, prefix="" )
# bEdge = isEdgeDirection( self, plot, ecDir )
# bRiver =  hasRiverAtPlot( plot )
# bCorner = hasRiverAtSECorner( plot )
# bCorner = hasCoastAtSECorner( plot )
# bCorner = hasPlotTypeAtSECorner( plot, plotType )
# eCard = getBestFlowDir( plot, bDownFlow=True, bShort=False, eForbiddenList=[] )
# --- private ---
# iHeight = getPlotHeight( plot )
# fHeight = getPlotBaseHeight( plot )
# riverDirs = checkRiverEnd( plot )
# rivStart = rimRivers( x, y, distList )
#########################################################################################################
class RiverMaker:
	bMakeSourceHill = True
	bRiverStart = False
	cntHills = 0
	cntPeaks = 0
	cntLand = 0
	# make a new river beginning at the SE corner of the plot
	# basically CvMapgenerator.doRiver() with reversed flow and a list of river-plots
	# Note: While upFlow rivers can start from rivers, only downFlow rivers can end there. To produce
	#   a normal looking web of rivers, upFlow rivers should be build first. Since the default addRivers()
	#   produces only downFlow rivers, upFlow rivers should be made before or at the start of addRivers().
	def buildRiver( self, pStartPlot, bDownFlow=True, ecNext=None, ecOri=None, iThisRiverID=None, riverList=None ):

		self.bRiverStart = False
		# check params
		if riverList == None:
			riverList = []
			self.bRiverStart = True
		if iThisRiverID == None:
			iThisRiverID = CyMap().getNextRiverID()
			CyMap().incrementNextRiverID()
		if riverList:
			if type(riverList[0])==type([]):
				riverList.insert( 0, iThisRiverID )
				riverList.insert( 1, bDownFlow )
				self.bRiverStart = True
		else:
			self.cntHills = 0
			self.cntPeaks = 0
			self.cntLand = 0
			self.bRiverStart = True
			if ecNext != None:
				riverList.append( iThisRiverID )
				riverList.append( bDownFlow )

		# does another river already exist - we can't branch off of an existing river!
		# - actually we can, if the river starts and we go upFlow
		iOtherRiverID = pStartPlot.getRiverID()
		if (iOtherRiverID != -1) and (iOtherRiverID != iThisRiverID):
			if bDownFlow or not self.bRiverStart:
				return

		bNoDir = False
		ecBest = None
		if ecNext == None: ecNext = CardinalDirectionTypes.NO_CARDINALDIRECTION
		# flow direction
		ecFlow = ecNext
		if not bDownFlow:
			ecFlow = getOppositeCardinalDirection( ecNext )
		# handle each possible next direction and produce next riverPlot
		rivDirs = self.checkRiverEnd( pStartPlot, bDownFlow )				# get list of possible directions
		if ecNext == ecNorth:
			if not (ecNext in rivDirs): return
			pRiverPlot = pStartPlot
			if pRiverPlot.isNone(): return
			if pRiverPlot.isWOfRiver(): return

			pStartPlot.setRiverID( iThisRiverID )
			pRiverPlot.setWOfRiver( True, ecFlow )
			if bDownFlow:
				riverList.append( [pRiverPlot, ecFlow] )
			else:
				riverList.insert( 2, [pRiverPlot, ecFlow] )
			pRiverPlot = plotCardinalDirection( pRiverPlot.getX(), pRiverPlot.getY(), ecNorth )
		elif ecNext == ecEast:
			if not (ecNext in rivDirs): return
			pRiverPlot = plotCardinalDirection( pStartPlot.getX(), pStartPlot.getY(), ecEast )
			if pRiverPlot.isNone(): return
			if pRiverPlot.isNOfRiver(): return

			pStartPlot.setRiverID( iThisRiverID )
			pRiverPlot.setNOfRiver( True, ecFlow )
			if bDownFlow:
				riverList.append( [pRiverPlot, ecFlow] )
			else:
				riverList.insert( 2, [pRiverPlot, ecFlow] )
		elif ecNext == ecSouth:
			if not (ecNext in rivDirs): return
			pRiverPlot = plotCardinalDirection( pStartPlot.getX(), pStartPlot.getY(), ecSouth )
			if pRiverPlot.isNone(): return
			if pRiverPlot.isWOfRiver(): return

			pStartPlot.setRiverID( iThisRiverID )
			pRiverPlot.setWOfRiver( True, ecFlow )
			if bDownFlow:
				riverList.append( [pRiverPlot, ecFlow] )
			else:
				riverList.insert( 2, [pRiverPlot, ecFlow] )
		elif ecNext == ecWest:
			if not (ecNext in rivDirs): return
			pRiverPlot = pStartPlot
			if pRiverPlot.isNone(): return
			if pRiverPlot.isNOfRiver(): return

			pStartPlot.setRiverID( iThisRiverID )
			pRiverPlot.setNOfRiver( True, ecFlow )
			if bDownFlow:
				riverList.append( [pRiverPlot, ecFlow] )
			else:
				riverList.insert( 2, [pRiverPlot, ecFlow] )
			pRiverPlot = plotCardinalDirection( pRiverPlot.getX(), pRiverPlot.getY(), ecWest )
		else:
			bNoDir = True															# hack to enable water-start
			pRiverPlot = pStartPlot
			ecBest = self.getBestFlowDir( pRiverPlot, bDownFlow )		# longRiver
			if ecBest == None:
				return
		# already done?
		if pRiverPlot.isNone():
			return																	# all is well; river flows off the map
		if not bNoDir:
			if bDownFlow and self.hasCoastAtSECorner( pRiverPlot ):
				return																# all is well; river flows into ocean
			elif (not bDownFlow) and self.hasPlotTypeAtSECorner( pRiverPlot, PlotTypes.PLOT_PEAK ):
				self.cntPeaks += 1
				if self.cntPeaks >= 2:
					ch = self.cntPeaks * 100 / (self.cntPeaks + 2)		# 50,60,66,71,75 -> 50,20,06,01
					if choose( ch, True, False ):
						return														# all is well; the peak is the source
			elif (not bDownFlow) and self.hasPlotTypeAtSECorner( pRiverPlot, PlotTypes.PLOT_HILLS ):
				self.cntHills += 1
				if self.cntHills >= 3:
					ch = self.cntHills * 100 / (self.cntHills + 3)		# 50,57,62,66,70 -> 50,21,08,02
					if choose( ch, True, False ):
						return														# all is well; the hill is the source
			elif (not bDownFlow) and self.hasPlotTypeAtSECorner( pRiverPlot, PlotTypes.PLOT_LAND ):
				self.cntLand += 1
				if self.cntLand >= 6:
					ch = self.cntLand * 100 / (self.cntLand + 6)			# 50,53,57,60,62 -> 50,23,10,04,01
					if choose( ch, True, False ):
						if self.bMakeSourceHill:
							pRiverPlot.setPlotType( PlotTypes.PLOT_HILLS, True, True )
						return														# all is well; new hill is the source
		# find next direction
		if ecBest == None:
			eForbiddenList = []
			if ecOri != None: eForbiddenList.append( getOppositeCardinalDirection( ecOri ) )
			if ecNext != None: eForbiddenList.append( getOppositeCardinalDirection( ecNext ) )
			ecBest = self.getBestFlowDir( pRiverPlot, bDownFlow, True, eForbiddenList )			# shortRiver
		# build next river segment
		if ecBest != None:
			if  ecOri == None: ecOri = ecBest
			self.buildRiver( pRiverPlot, bDownFlow, ecBest, ecOri, iThisRiverID, riverList )

	# puts a river on islands, or on one island if areaID is given
	# - if they don't have one and
	# - if they have at least one inland plot
	def islandRivers( self, minIsle=6, maxIsle=50, areaID=None ):
		print "[MST] ===== RiverMaker:islandRivers()"
		sprint = ""
		chNoHills = 66
		areas = GC.getMap().areas()
		cnt = 0
		for area in areas:
			if areaID != None:
				if not ( area.getID() == areaID ):
					continue
			aTotalPlots = area.getNumTiles()
			aRiverEdges = area.getNumRiverEdges()
			aCoastLand  = area.countCoastalLand()
			if area.isWater(): continue														# islands!
			# don't be to sure
			if aTotalPlots < (minIsle + chooseNumber( 3 ) - 1): continue			# to small
			if aTotalPlots > (maxIsle + chooseNumber( 5 ) - 2): continue			# to big
			if aRiverEdges > 0: continue														# already has river

			cnt += 1
			pList = []
			noHillsList = []
			id = area.getID()
			sprint += "\n"
			sprint += "[MST] IslandArea %i(%i) without rivers found \n" % (id,aTotalPlots)
			cntPl = 0
			cntSE = 0
			for inx in range( MAP.numPlots() ):
				plot = MAP.plotByIndex( inx )
				if id != plot.getArea(): continue											# wrong island
				cntPl += 1
				x,y = plot.getX(), plot.getY()
				if not self.hasCoastAtSECorner( plot ):									# inland-plot
					cntSE += 1
					if self.hasPlotTypeAtSECorner( plot, PlotTypes.PLOT_PEAK ):
						pList.append( plot )														# mountain-plot * 2
						pList.append( plot )
					elif self.hasPlotTypeAtSECorner( plot, PlotTypes.PLOT_HILLS ):
						pList.append( plot )														# hill-plot * 1
					else:
						noHillsList.append( plot )												# no hills/peak
			if cntPl > 0:
				sprint += "[MST] %i possible plots, %i possible corners \n" % (cntPl,cntSE)
			if len( pList ) == 0:
				sprint += "[MST] - no inland riverstart-plots with hills/peak found \n"
				if choose( chNoHills, True, False ):
					pList = noHillsList
					if len( pList ) == 0:
						sprint += "[MST] - no inland riverstart-plots found! \n"
			if len( pList ) > 0:
				plot = chooseListPop( pList )
				rList = []
				rDir = self.getBestFlowDir( plot )			# downFlow, longRiver
				if rDir != None:
					self.buildRiver( plot, True, rDir, riverList=rList )
					sprint += riverMaker.outRiverList( rList, "[MST]" )
				# chance for 2nd river depends on number of possible plots
				if choose( len(pList)-5, True, False ):
					plot = chooseListPop( pList )
					rList = []
					rDir = self.getBestFlowDir( plot )		# downFlow, longRiver
					if rDir != None:
						self.buildRiver( plot, True, rDir, riverList=rList )		# 'wrong' dir for longer river
						sprint += riverMaker.outRiverList( rList, "[MST]" )

		if (cnt == 0) and (areaID==None):
			sprint += "[MST] No smaller islands without rivers found \n"
		elif cnt==0:
			sprint += "[MST] No river created on this island\n"
		print sprint

	# build rivers from lake up
	def buildRiversFromLake( self, lakeAreaID=None, chRiver=66, nRivers=1, minLake=1 ):

		if lakeAreaID == None:
			print "[MST] ===== RiverMaker:buildRiversFromLake()"
			# build rivers from all lakes
			MAP.recalculateAreas()
			areas = GC.getMap().areas()
			for area in areas:
				if area.isLake():
					iAreaID = area.getID()
					self.buildRiversFromLake( iAreaID, chRiver, nRivers, minLake )
			return

		# build rivers from single lake
		area = MAP.getArea( lakeAreaID )
		if not area.isLake(): return													# not lake
		# don't be to sure
		if area.getNumTiles() < (minLake + chooseNumber( 3 ) - 1): return	# too small
		# make plot-list
		lPlots = getAreaPlotsXY( lakeAreaID )
		# check for old rivers
		rivList = []
		for x,y in lPlots:
			for dx in range( -1, 2 ):
				for dy in range( -1, 2 ):
					pl = plotXY( x, y, dx, dy)
					if not pl.isNone():
						rid = pl.getRiverID()
						if rid > -1:
							if (dx,dy) in [ (-1,0),(-1,1),(0,1) ]:					# W,NW,N
								if rid not in rivList:
									rivList.append( rid )
							elif (dx,dy) in [ (1,1),(1,0) ]:							# NE,E
								if pl.isNOfRiver():
									if rid not in rivList:
										rivList.append( rid )
							elif (dx,dy) in [ (-1,-1),(0,-1) ]:						# S,SW
								if pl.isWOfRiver():
									if rid not in rivList:
										rivList.append( rid )
		if len( rivList ) >= nRivers: return										# enough rivers found
		# check each plot for possible river directions
		cdirList = []
		for x,y in lPlots:
			pl = MAP.plot(x,y)
			if not pl.isWater():
				print "[MST] Bad Lake-Plot found!"
				continue
			# check all corners of plot
			ecDir = self.getBestFlowDir( pl, False )								# upFlow, longRiver
			if ecDir != None:
				cdirList.append( (ecDir, pl) )
			p = plotCardinalDirection( x, y, ecWest )								# West
			if not p.isNone():
				if not p.isWater():														# water-plot is checked elsewhere
					ecDir = self.getBestFlowDir( p, False )						# upFlow, longRiver
					if ecDir != None:
						cdirList.append( (ecDir, p) )
			p = plotCardinalDirection( x, y, ecNorth )							# North
			if not p.isNone():
				if not p.isWater():														# water-plot is checked elsewhere
					ecDir = self.getBestFlowDir( p, False )						# upFlow, longRiver
					if ecDir != None:
						cdirList.append( (ecDir, p) )
			p = plotDirection( x, y, DirectionTypes.DIRECTION_NORTHWEST )	# NorthWest
			if not p.isNone():
				if not p.isWater():														# water-plot has no rivers
					ecDir = self.getBestFlowDir( p, False )						# upFlow, longRiver
					if ecDir != None:
						cdirList.append( (ecDir, p) )
		# make new rivers
		sprint = ""
		newRivers = nRivers - len( rivList )
		while ( newRivers > 0 ) and ( len(cdirList) > 0 ):
			# randomly choose riverstart
			eCard, pl = chooseListPop( cdirList )
			# check if direction ends in river
			x, y = pl.getX(), pl.getY()
			p = plotCardinalDirection( x, y, CardinalDirectionTypes(eCard) )
			if not p.isNone():
				if not self.hasRiverAtSECorner( p ):
					# can build new river
					newRivers -= 1
					if choose( chRiver, True, False ):
						rList = []
						self.buildRiver( pl, False, eCard, riverList=rList )	# build upriver
						sprint += "[MST] " + "x,y %i,%i - %s \n" % (pl.getX(),pl.getY(),cardinalName(eCard))
						sprint += self.outRiverList( rList, "[MST]" )
					if area.getNumTiles() == 1:
						break # only one river for mini-lakes

		if sprint: print sprint


	# print riverList plots and flows
	def outRiverList( self, riverList, prefix="" ):
		sList = ""
		if len(riverList) < 3:
			return prefix + " No River \n"
		sList += prefix + " RiverID: %i - %s \n" % (riverList[0],iif(riverList[1],"DownFlow","UpFlow"))
		sList += prefix + " --> Source:[ "
		for i in range(2,len(riverList)):
			p,e = riverList[i]
			sList += "[%i,%i - %s] " % (p.getX(),p.getY(),cardinalName(e))
		sList += "]:RiverEnd \n"
		return sList

	# check if direction leads off the map
	def isEdgeDirection( self, plot, ecDir ):
		x, y = plot.getX(), plot.getY()
		pl = plotCardinalDirection( x, y, CardinalDirectionTypes( ecDir ) )
		if pl.isNone(): return True
		if (ecDir == ecSouth) or (ecDir == ecEast):
			dx, dy = pl.getX(), pl.getY()
			p = plotCardinalDirection( dx, dy, CardinalDirectionTypes( ecDir ) )
			if p.isNone(): return True
		return False

	# check if plot has rivers at on of its corners
	def hasRiverAtPlot( self, plot ):
		if self.hasRiverAtSECorner( plot ): return True
		x, y = plot.getX(), plot.getY()
		pWPlot = plotDirection( x, y, DirectionTypes.DIRECTION_WEST )
		if not pWPlot.isNone():
			if self.hasRiverAtSECorner( pWPlot ): return True
		pNPlot = plotDirection( x, y, DirectionTypes.DIRECTION_NORTH )
		if not pNPlot.isNone():
			if self.hasRiverAtSECorner( pNPlot ): return True
		pNWPlot = plotDirection( x, y, DirectionTypes.DIRECTION_NORTHWEST )
		if not pNWPlot.isNone():
			if self.hasRiverAtSECorner( pNWPlot ): return True
		return False

	# check for river at SE-corner
	# like isRiverMask() in cvPlot.cpp
	def hasRiverAtSECorner( self, plot ):
		if plot.isNOfRiver(): return True
		if plot.isWOfRiver(): return True
		pEastPlot = plotDirection( plot.getX(), plot.getY(), DirectionTypes.DIRECTION_EAST )
		if not pEastPlot.isNone():
			if pEastPlot.isNOfRiver(): return True
		pSouthPlot = plotDirection( plot.getX(), plot.getY(), DirectionTypes.DIRECTION_SOUTH )
		if not pSouthPlot.isNone():
			if pSouthPlot.isWOfRiver(): return True
		return False

	# check for water at SE-corner
	# like hasCoastAtSECorner() in cvPlot.cpp
	def hasCoastAtSECorner(self, plot):
		return self.hasPlotTypeAtSECorner(plot, PlotTypes.PLOT_OCEAN)

	# check for plot-type at SE-corner
	def hasPlotTypeAtSECorner(self, plot, plotType):
		if plot.getPlotType() == plotType: return True
		pAdjacentPlot = plotDirection( plot.getX(), plot.getY(), DirectionTypes.DIRECTION_EAST )
		if not pAdjacentPlot.isNone():
			if pAdjacentPlot.getPlotType() == plotType: return True
		pAdjacentPlot = plotDirection( plot.getX(), plot.getY(), DirectionTypes.DIRECTION_SOUTHEAST )
		if not pAdjacentPlot.isNone():
			if pAdjacentPlot.getPlotType() == plotType: return True
		pAdjacentPlot = plotDirection( plot.getX(), plot.getY(), DirectionTypes.DIRECTION_SOUTH )
		if not pAdjacentPlot.isNone():
			if pAdjacentPlot.getPlotType() == plotType: return True
		return False

	# get best flow-direction from SE_of_Plot
	def getBestFlowDir( self, plot, bDownFlow=True, bShort=False, eForbiddenList=[] ):
		chRandom = 90		# chance to use the 'best' route
		chEdge =	80			# chance to hit edge deliberately
		iMin = 0
		iMax = 99999
		riverDirs = self.checkRiverEnd( plot, bDownFlow )
		iBestUpValue = iMin
		upCDir = None
		iBestDownValue = iMax
		downCDir = None
		x,y = plot.getX(), plot.getY()
		for eCard in riverDirs:
			if eCard in eForbiddenList: continue
			if self.isEdgeDirection( plot, eCard ):
				if choose( chEdge, True, False ):
					iBestUpValue = iMax
					upCDir = eCard
					iBestDownValue = iMin
					downCDir = eCard
					continue
			pl = plotCardinalDirection( x, y, CardinalDirectionTypes(eCard) )
			if not pl.isNone():
				iVal = self.getSEPlotHeight( pl )
				if iVal > iBestUpValue:
					if (iBestUpValue == iMin) or choose( chRandom, True, False ):
						iBestUpValue = iVal
						upCDir = eCard
				if iVal < iBestDownValue:
					if (iBestDownValue == iMax) or choose( chRandom, True, False ):
						iBestDownValue = iVal
						downCDir = eCard
		if bShort:
			return iif( bDownFlow, downCDir, upCDir )				# shortest way to target
		else:
			return iif( bDownFlow, upCDir, downCDir )				# longest way to target

	###############
	### Helpers ###
	###############

	# calc SE_of_Plot-height for river
	def getSEPlotHeight( self, plot ):
		fVal = self.getPlotHeight( plot )
		fDiv = 1.0
		x,y = plot.getX(), plot.getY()
		p1 = plotXY( x, y, 1, 0 )
		if not p1.isNone():
			fVal += self.getPlotHeight( p1 )
			fDiv += 1.0
		p2 = plotXY( x, y, 0, -1 )
		if not p2.isNone():
			fVal += self.getPlotHeight( p2 )
			fDiv += 1.0
		p3 = plotXY( x, y, 1, -1 )
		if not p3.isNone():
			fVal += self.getPlotHeight( p3 )
			fDiv += 1.0
		return fVal / fDiv

	# calc plot-height for river
	def getPlotHeight( self, plot ):
		x,y = plot.getX(), plot.getY()
		fHeight = 4.0 * self.getPlotBaseHeight( plot )						# center has quintuple weight
		i = 4
		bWater = False
		for dx in range(x-1,x+2):
			for dy in range(y-1,y+2):
				fh = self.getPlotBaseHeight( plotXY(dx,dy,0,0) )
				if fh>0:
					i += 1
					fHeight += fh
				else:
					bWater = True														# lower near water
		if bWater: i += 1
		fHeight += chooseNumber( 20 ) / 10.0									# make it fuzzy
		return 120.0 * fHeight / i

	# define base-height of plots
	def getPlotBaseHeight( self, plot ):
		if plot.isWater():		return 0.0
		if plot.isFlatlands():	return 0.2
		if plot.isHills():		return 0.5
		if plot.isPeak():			return 1.0
		return 0.01

	# tests if directions from SE-corner of plot are possible
	# returns list of possible directions
	def checkRiverEnd( self, plot, bDownFlow=True ):
		riverDirs = []
		x = plot.getX()
		y = plot.getY()
		# West
		if not plot.isWater():
			pl = plotXY( x, y, 0, -1 )				# South
			if not pl.isNone():
				if not pl.isWater():
					riverDirs.append( ecWest )
		# North
		if not plot.isWater():
			pl = plotXY( x, y, 1, 0 )				# East
			if not pl.isNone():
				if not pl.isWater():
					riverDirs.append( ecNorth )
		# East
		p1 = plotXY( x, y, 1, 0 )					# East
		if not p1.isNone():
			if not p1.isWater():
				p2 = plotXY( x, y, 1, -1 )			# SE
				if not p2.isNone():
					if not p2.isWater():
						riverDirs.append( ecEast )
		# South
		p1 = plotXY( x, y, 1, -1 )					# SE
		if not p1.isNone():
			if not p1.isWater():
				p2 = plotXY( x, y, 0, -1 )			# South
				if not p2.isNone():
					if not p2.isWater():
						riverDirs.append( ecSouth )
		# UpFlow may not end in River or Water
		if not bDownFlow:
			inx = len( riverDirs ) - 1
			while inx >= 0:
				# check if water or river is at the end
				eCard = riverDirs[inx]
				pl = plotCardinalDirection( x, y, eCard )
				if not pl.isNone():
					if self.hasRiverAtSECorner( pl ) or self.hasCoastAtSECorner( pl ):
						del riverDirs[inx]
				inx -= 1
		return riverDirs

	# find riverPlots on rim-peaks at a distance from x,y
	# if downflow - peaks have twice tha chance to be a source than hills
	# if upflow - lakes or ocean will be searched for
	def rimRivers(self, x, y, distList, bDownFlow=True):
		rivStart = []							# river-starts: [ bogDir, riverPlot, eCard ]
		rPlots = []								# potential river-plots
		# get plots at distances
		for dis in distList:
			for dx in range(-dis, 1+dis):
				for dy in range(-dis, 1+dis):
					pDist = plotDistance( x, y, x+dx, y+dy )
					if pDist == dis:
						plot = plotXY( x, y, dx, dy )
						if plot.isNone(): continue
						if bDownFlow:
							# no start at SE coast
							if self.hasCoastAtSECorner( plot ): continue
							# peaks make river-start twice as probable
							if plot.isPeak():
								rPlots.append( plot )
								rPlots.append( plot )
							elif plot.isHills():
								rPlots.append( plot )
						else:
							rivDirs = self.checkRiverEnd( plot, bDownFlow )
							if ( len(rivDirs) == 1 ) or ( len(rivDirs) == 2 ) or plot.isRiver():
								rPlots.append( plot )
		# get riverPlot and eCard for river
		for pl in rPlots:
			fx = pl.getX()
			fy = pl.getY()
			bogDir = coordDirection( fx, fy, x, y )
			eCardList = getCardDirsFromDir( bogDir )
			if not bDownFlow:
				rivDirs = self.checkRiverEnd( pl, bDownFlow )
				if pl.isRiver():
					if pl.isNOfRiver():
						if pl.isWOfRiver():
							rDirs = [ ecEast, ecSouth ]
						else:
							p = plotCardinalDirection( fx, fy, ecEast )
							if not p.isNone():
								if p.isNOfRiver():
									rDirs = [ ecNorth, ecSouth ]
							p = plotCardinalDirection( fx, fy, ecSouth )
							if not p.isNone():
								if p.isWOfRiver():
									rDirs = [ ecNorth, ecEast ]
					if pl.isWOfRiver():
						if pl.isNOfRiver():
							rDirs = [ ecEast, ecSouth ]
						else:
							p = plotCardinalDirection( fx, fy, ecEast )
							if not p.isNone():
								if p.isNOfRiver():
									rDirs = [ ecWest, ecSouth ]
							p = plotCardinalDirection( fx, fy, ecSouth )
							if not p.isNone():
								if p.isWOfRiver():
									rDirs = [ ecEast, ecWest ]
					rivDirs = list( set(rivDirs) & set(rDirs) )		# no doubles in resulting list
				rivDirs = list( set(rivDirs) & set(eCardList) )		# no doubles in resulting list
				for eCard in rivDirs:
					rivStart.append( [ bogDir, pl, eCard ] )
			else:
				for eCard in eCardList:
					rivStart.append( [ bogDir, pl, eCard ] )
					if len(eCardList) == 1:
						p = plotXY( fx, fy, -1, 1 )
						if (not p.isNone()) and (not p.isWater()):
							rivStart.append( [ bogDir, p, eCard ] )
		if bDownFlow:
			# no start if a river is already there
			for i in range( len(rivStart)-1, -1, -1 ):
				if rivStart[i][1].isRiver():
					del rivStart[i]
		if rivStart: shufflePyList(rivStart, GC.getGame().getMapRand())
		return rivStart

################################################################################
########## CLASS RiverMaker END
################################################################################
riverMaker = RiverMaker()


################################################################################
########## CLASS TeamStart - place team-members near to each other
################################################################################
### Uses priorities and within those places teams with most members first:
### 	1) all human team-members, team>1
###	2) at least one human in the team, team>1
###	3) no humans in the team, team>1
###	4) single-member teams
### This class uses a very simple algorithm and as a result unfortunate things
### could happen under the wrong circumstances:
### If there is no single-member team and the world is flat, the members
### of the lowest priority team may be on different sides of the world.
### Actually even with a roung world, the chances of nearby starting-plots
### for low priority teams are not very good unless there are a good number
### of single member teams.
### Note: stepDistance() is used to determine distances
################################################################################
# placeTeamsTogether( bTogether=False, bSeparated=False )
# bTeams = getTeams()
# --- private ---
# getPlotDistances()
# rangList = getBigTeam()
################################################################################
class TeamStart:
	playerList = []			# [ player, ..]
	teamDict = {}				# { teamNum : [playerNum, ..] }
	humanDict = {}				# { teamNum : iHumanity }
	playersDict = {}			# { playerNum : (plotNum, startingPlotIndex, (x, y)) }
	plotDistDict = {}			# { plotNum : [ (plotNum, distance, startingPlotIndex, (x, y)), ..] }
	startPlotDict = {}		# { plotNum : startingPlotIndex }

	# place human team-members near to each other and try to do the same for non-humans
	# if both parameters are True, starting-plots are randomized
	# if both parameters are False, return without doing anything
	def placeTeamsTogether( self, bTogether=False, bSeparated=False ):
		print "[MST] ===== TeamStart:placeTeamsTogether()"
		# housekeeping
		self.playersDict = {}
		self.playersList = []

		bTeams = self.getTeams()
		self.getPlotDistances()

		# check need
		if not bTeams:
			print "[MST] No team with more than one player. Nothing done."
			return
		elif (not bTogether) and (not bSeparated):
			print "[MST] No players are placed. Nothing done."
			return

		# check for random distribution
		if bTogether and bSeparated:
			print "[MST] All players to be randomly distributed over available starting-plots."
			plotList = []
			for pl in self.playerList:
				plotList.append(pl.getStartingPlot())
			if plotList: shufflePyList(plotList, GC.getGame().getMapRand())
			i = 0
			for pl in self.playerList:
				pl.setStartingPlot( plotList[i], False )
				i += 1
			return

		# place teams
		print "[MST] Place teams %s" % ( iif(bTogether, "together.","separated.") )
		# get priority list
		tRangs = self.getBigTeam()
		for teamNum in tRangs:

			# find starting-plot for first team-member
			pNumList = self.teamDict[teamNum]
			# get first team-member
			playerNum = pNumList[0]
			del pNumList[0]
			# randomly choose staring-plot from remaining pool
			spNum = chooseListElement( self.startPlotDict.keys() )
			spInx = self.startPlotDict[ spNum ]
			# remove from pool
			del self.startPlotDict[ spNum ]
			# add new playerNum and starting-plot
			self.playersDict[ playerNum ] = ( spNum, spInx, coordByIndex( spInx ) )
			# more team-members
			while pNumList:
				disList = self.plotDistDict[spNum]

				# find starting-plot for next team-member
				if bTogether:
					# find nearest possible starting-plot
					d0 = 99999
					p0 = -1
					for ospNum, dist, spInx, coords in disList:
						if ospNum in self.startPlotDict.keys():
							if dist < d0:
								d0 = dist
								p0 = ospNum
				else:
					# find farthest possible starting-plot
					d0 = 0
					p0 = -1
					for ospNum, dist, spInx, coords in disList:
						if ospNum in self.startPlotDict.keys():
							if dist > d0:
								d0 = dist
								p0 = ospNum

				# ----- ERROR handler - this shouldn't happen
				if p0 == -1:
					sprint = ""
					sprint += "[MST] ----- ERROR -----"
					sprint += "[MST] Team Start failed - No changes made to starting plots."
					sprint += "[MST] - teamDict %r" % ( self.teamDict )
					sprint += "[MST] - humanDict %r" % ( self.humanDict )
					sprint += "[MST] - plotDistDict: %r" % (self.plotDistDict)
					sprint += "[MST] - startPlotDict: %r" % (self.startPlotDict)
					sprint += "[MST] - playersDict: %r" % (self.playersDict)
					sprint += "[MST] - disList: %r" % (disList)
					sprint += "[MST] - pNumList: %r" % (pNumList)
					sprint += "[MST] -----------------"
					print sprint
					return
				# add new playerNum
				spInx = self.startPlotDict[ p0 ]
				del self.startPlotDict[ p0 ]
				oPlayerNum = pNumList[0]
				self.playersDict[ oPlayerNum ] = ( p0, spInx, coordByIndex( spInx ) )
				del pNumList[0]

		# set new starting-plots
		for playerNum in self.playersDict.keys():
			player = GC.getPlayer( playerNum )
			sPlot = MAP.plotByIndex( self.playersDict[ playerNum ][1] )
			player.setStartingPlot( sPlot, False )

	# create dictionary of team members and one of their humatity; teamDict, humanDict
	def getTeams( self ):
		print "[MST] ===== TeamStart:getTeams()"
		# housekeeping
		self.teamDict = {}
		self.humanDict = {}
		# get teams of players
		self.playerList = mapStats.getCivPlayerList()
		for player in self.playerList:
			playerNum = player.getID()
			teamNum = GC.getPlayer( playerNum ).getTeam()
			if not teamNum in self.teamDict.keys():
				self.teamDict[ teamNum ] = []
			self.teamDict[ teamNum ].append( playerNum )
		# get humanity of teams
		for teamNum in self.teamDict.keys():
			teamList = self.teamDict[ teamNum ]
			humanity = 0
			for playerNum in teamList:
				player = GC.getPlayer( playerNum )
				if player.isHuman():
					humanity += 1
			if humanity == len( teamList ):				# all team-members human
				humanity = 2
			elif humanity > 0:								# at least one team-member human
				humanity = 1
			self.humanDict[ teamNum ] = humanity

		# check for True teams
		for i in self.teamDict.keys():
			if len( self.teamDict[i] ) > 1: return True			# True if there is a team with 2 or more members
		return False

	###############
	### Helpers ###
	###############

	# create dictionarys for distances and old starting-plots;	plotDistDict, startPlotDict
	def getPlotDistances( self ):
		# housekeeping
		self.startPlotDict = {}
		self.plotDistDict = {}
		# get playerNum list
		self.playerList = mapStats.getCivPlayerList()

		# get starting-plots
		spNum = 0
		for player in self.playerList:
			pCenter = player.getStartingPlot()
			# make playerNum, startPlotDict
			self.startPlotDict[ spNum ] = indexByPlot( pCenter )
			spNum += 1

		# get starting-plot distances
		for sp in range( spNum ):
			self.plotDistDict[ sp ] = []
			# get center
			pCenter = self.startPlotDict[ sp ]
			x0, y0 = coordByIndex( pCenter )
			# get distances from center
			for osp in range( spNum ):
				otherStPlot = self.startPlotDict[ osp ]
				dx, dy = coordByIndex( otherStPlot )
				dist = stepDistance( x0, y0, dx, dy )
				self.plotDistDict[ sp ].append( ( osp, dist, otherStPlot, (dx, dy) ) )

	# get ranked list of teams with most members
	# 1) human team>1, 2) has human in team>1, 3) biggest team>1 4) single member teams
	def getBigTeam( self ):
		# get team priorities
		teamLength = []
		for teamNum in self.teamDict:
			big = 10
			if self.humanDict[ teamNum ] == 1: big = 100
			elif self.humanDict[ teamNum ] == 2: big = 1000
			if len( self.teamDict[ teamNum ] ) == 1:
				big = self.humanDict[ teamNum ]
			else:
				big += len( self.teamDict[ teamNum ] )
			teamLength.append( (big, teamNum) )
		teamLength.sort()
		teamLength.reverse()
		# return list of teams sorted by priority
		tRang = []
		for l,p in teamLength: tRang.append( p )
#		print "[MST] teamRankedList %r" % (tRang)
		return tRang

################################################################################
########## CLASS TeamStart END
################################################################################
teamStart = TeamStart()


##########################################################################################################
########## CLASS MapPrint - print maps
##########################################################################################################
# initialize()
# definePrintMap( lines, charsPerPlot, linesPerPlot, mapTitle="", mapText="",
#                 region=None, offset=None, mapLegend="" )
# printMap( bDiffDict, sMapType, sMapText="" )
# bSuccess = buildDiffMap( newDict, oldDict )
# buildAreaMap( bDiffDict=False, sText=None, region=None, areaID=None, areaDict=None )
# buildPlotMap( bDiffDict=False, sText=None, region=None, data=None )
# buildTerrainMap( bDiffDict=False, sText=None, region=None, terrainDict=None, showPlots=True )
# buildFeatureMap( bDiffDict=False, sText=None, region=None, featureDict=None, showPlots=True )
# buildBonusMap( bDiffDict=False, sText=None, region=None, bonusDict=None, showPlots=True )
# buildRiverMap( bDiffDict=False, sText=None, region=None )
# --- private ---
# sLegend = makeMapLegend( dict, addLine=None, bRiver=None, bPlayer=None )
##########################################################################################################
class MapPrint:
	# private class variables
	__lines = {}
	__charsPerPlot = 1
	__linesPerPlot = 1
	__offset       = [ 0 ]
	__mapRegion    = [0,1,0,1]
	__mapTitle     = ""
	__mapText      = ""
	__mapLegend    = ""
	__diffMaps     = {}

	# initialize dictionaries
	def initialize( self ):
		self.__diffMaps[ "PLOT" ]		= None
		self.__diffMaps[ "TERRAIN" ]	= None
		self.__diffMaps[ "FEATURE" ]	= None
		self.__diffMaps[ "BONUS" ]		= None
		self.__diffMaps[ "RIVER" ]		= None
		self.__diffMaps[ "AREAS" ]		= None

		# ---------------
		# area dictionary
		# ---------------
		self.__areaDict = {
			"Special"   : [ "$",     "Mapped Area"],
			"Ocean"     : [ " .,:;", "Ocean" ],
			"Water"     : [ "~",     "Other Water" ],
			"Continent" : [ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", "Continents" ],
			"Land"      : [ "@",     "Other Land" ]
		}

		# --------------------
		# terrain dictionaries
		# --------------------
		self.__terrainDict = {
			etOcean		: ["..", "Ocean"  ],
			etCoast		: ["::", "Coast"  ],
			etDesert	: ["dD", "Desert" ],
			etPlains	: ["pP", "Plains" ],
			etGrass		: ["gG", "Grass"  ],
			etMarsh		: ["mM", "Marsh"  ],
			etTundra	: ["tT", "Tundra" ],
			etSnow		: ["sS", "Snow"   ]
		}
		# --------------------
		# feature dictionaries
		# --------------------
		self.__featureDict = {
			efIce										: ["�", "Ice"	],
			GC.getInfoTypeForString('FEATURE_FALLOUT')	: ["*", "Fallout"],
			efJungle									: ["j", "Jungle"],
			efForest									: ["f", "Forest"],
			GC.getInfoTypeForString('FEATURE_OASIS')	: ["O", "Oasis"],
			GC.getFEATURE_FLOOD_PLAINS()				: ["p", "FloodPlains"]
		}
		self.__featureDict[GC.getInfoTypeForString('FEATURE_SWAMP')] = ["w", "Swamp"]
		self.__featureDict[efKelp]									 = ["=", "Kelp"]
		self.__featureDict[GC.getInfoTypeForString('FEATURE_STORM')] = ["@", "Storm"]

		# ------------------
		# bonus dictionaries
		# ------------------
		self.__bonusDict = {
			ebCopper   : [ "C", "Copper"   ],
			ebIron     : [ "I", "Iron"     ],
			ebOil      : [ "O", "Oil"      ],
			ebUranium  : [ "U", "Uranium"  ],
			ebHorse    : [ "h", "Horse"    ]
		}
		self.__bonusDict[ebBauxite] = [ "B", "Bauxite" ]

	# define class variables
	def definePrintMap( self, lines, charsPerPlot, linesPerPlot, mapTitle="", mapText="", region=None, offset=None, mapLegend="" ):
		self.__lines        = lines
		self.__charsPerPlot = charsPerPlot
		self.__linesPerPlot = linesPerPlot
		self.__mapLegend    = mapLegend
		if mapTitle == None:
			self.__mapTitle = "The Map"
		else:
			self.__mapTitle = mapTitle
		if mapText == None:
			self.__mapText = ""
		else:
			self.__mapText = mapText
		if offset == None:
			self.__offset = []
			for i in range( linesPerPlot ): self.__offset.append( i*1000 )
		else:
			self.__offset = offset
		if region == None:
			if self.__mapRegion == None:
				self.__mapRegion	= [0, iNumPlotsX-1, 0, iNumPlotsY-1]
		else:
			self.__mapRegion = region

	# print the map as defined by class-variables
	# if diffList is given, a second map with the differences is printed
	def printMap( self, bDiffDict, sMapType, sMapText="" ):
		print "[MST] ####################################################################### MapScriptTools:MapPrint ###"
		printList( self.__mapRegion, self.__mapTitle, prefix="[MST] " )
		# print title
		sprint = "[MST] "
		if sMapType == "DIFF":
			sprint += sMapText
		else:
			if sMapText == "":
				sprint += self.__mapTitle + " { " + self.__mapText + " } -  X-Wrap: %r, Y-Wrap: %r" % (MAP.isWrapX(),MAP.isWrapY())
			else:
				sprint += self.__mapTitle + " " + sMapText + "  -  X-Wrap: %r, Y-Wrap: %r" % (MAP.isWrapX(),MAP.isWrapY())
			sprint += "\n " + "="*(len(sprint)-1)
		print sprint
		# prepare
		x0, x1, y0, y1	= self.__mapRegion
		numX = x1 - x0 + 1
		numY = y1 - y0 + 1
		preLine = 5
		mapTop = space(preLine) + "-"*(self.__charsPerPlot*(x1-x0+1)) + "+ \n"
		xLine  = space(preLine) + space(self.__charsPerPlot*(x1-x0+1) + ((x1-x0)/10) + 20 )

		for i in range( ((x1-x0)/10) + 1 ):
			off = preLine + 1 + i*10*self.__charsPerPlot
			mapTop = strInsert( mapTop, off+i-1, "+" )
			if self.__charsPerPlot==1:
				xLine = strExchange( xLine, off+i-2, (iif(i>=10,"%03i"," %02i")%(i*10+x0)) )
			else:
				xLine = strExchange( xLine, off+i-1, (iif(i>=10,"%03i","|%02i")%(i*10+x0)) )
		xLine = xLine[0:len(mapTop)-self.__charsPerPlot-3]

		# adjust for vertical wrap
		if self.__linesPerPlot > 1:
			if self.__lines[ self.__offset[self.__linesPerPlot-1] ] != None:
				if MAP.isWrapY():
					self.__lines[ self.__offset[self.__linesPerPlot-1]+numY ] = self.__lines[ self.__offset[self.__linesPerPlot-1] ]
				self.__lines[ self.__offset[self.__linesPerPlot-1] ] = None
		# list-print map
		lprint = []

		if MAP.isWrapY():
			try:
				lprint.append( "     |%s| \n" % (self.__lines[ self.__offset[self.__linesPerPlot-1]+numY ]) )
			except:
				pass
				# print "%r \n [ self.__linesPerPlot,numY ]%i,%i \n -----" % (lprint,self.__linesPerPlot,numY)

		for y in range(y1,y0-1,-1):
			xx, yy = normalizeXY( 0, y )
			if MAP.isWrapY() and ( not MAP.isWrapX() ):
				lprint.append( "%3i: |%s|:%3i\n" % (yy, self.__lines[ self.__offset[0] + y ], yy) )
			else:
				pl = MAP.plot(0, yy)
				soff = self.__offset[0] + y
				slin = self.__lines[ soff ]
				ev = evalLatitude( pl )
				lprint.append( "%3i: |%s|:%3i - %3i \n" % ( yy, slin, yy, ev ) )
			for i in range( 1,len(self.__offset) ):
				if self.__lines[ self.__offset[i]+y ] != None:
					lprint.append( "     |%s| \n" % (self.__lines[ self.__offset[i]+y ]) )

		# insert grid
		for i in range( len(lprint) ):
			n = (numX - 1) / 10
			while n>0:
				off = preLine + 1 + n*10*self.__charsPerPlot
				lprint[i] = strInsert( lprint[i], off, "|" )
				n -= 1
		lprint.insert( 0, mapTop )

		if self.__charsPerPlot > 1:
			latLine = xLine + iif(x1>99, "%03i| ", "%02i | ")
		else:
			latLine = xLine + iif(x1>99, "%03i", "%02i ")

		if MAP.isWrapY() and ( not MAP.isWrapX() ):
			latLine += "\n"
		else:
			latLine += "    Latitude\n"

		lprint.insert( 0, latLine % x1 )
		lprint.append( mapTop )
		lprint.append( latLine % x1 )
		# string-print map
		sprint = ""
		for s in lprint: sprint += s
		print sprint
		# print legend
		if self.__mapLegend!="":
			print self.__mapLegend
		print "[MST] ####################################################################### MapScriptTools:MapPrint ###"

		# check if difference-map is wanted
		if bDiffDict:
			# produce difference-map if possible and prepare for next call
			actMap = self.__lines.copy()
			actMap[ 'REGION' ] = self.__mapRegion
			actMap[ 'TITLE' ] = self.__mapTitle
			actMap[ 'TEXT' ] = self.__mapText
			# check if we actually have a previous map with the same region
			if (self.__diffMaps[ sMapType ] != None) and (self.__diffMaps[ sMapType ]['REGION'] == actMap['REGION']):
				# build difference map and print
				oldMap = self.__diffMaps[ sMapType ].copy()
				dprint = self.buildDiffMap( actMap, oldMap )
				if dprint:
					self.printMap( False, "DIFF" )

			# now store actual map for the next time and exit
			self.__diffMaps[ sMapType ] = actMap
			self.__diffMaps[ sMapType ][ 'REGION' ] = actMap[ 'REGION' ]
			self.__diffMaps[ sMapType ][ 'TITLE' ] = actMap[ 'TITLE' ]
			self.__diffMaps[ sMapType ][ 'TEXT' ] = actMap[ 'TEXT' ]

	# build new dictionary with differences of two maps; return True for success
	# - note that this only builds the difference-map and does not print it
	def buildDiffMap( self, newDict, oldDict ):

		# check type
		if newDict[ 'TITLE' ] != oldDict[ 'TITLE' ]:
			print "[MST] Error! Different map type: %r - %r (new,old)" % ( newDict['TITLE'], oldDict['TITLE'] )
			return False
		# check region
		if newDict[ 'REGION' ] != oldDict[ 'REGION' ]:
			print "[MST] Error! Different region: %r - %r (new,old)" % ( newDict['REGION'], oldDict['REGION'] )
			return False
		# check length
		if len(newDict) != len(oldDict):
			print "[MST] Error! Different dict-length: %i, %i (new,old)" % ( len(newDict), len(oldDict) )
			return False

		# make title
		diffTitle  = "Difference Map:     -     X-Wrap: %r, Y-Wrap: %r \n" % (MAP.isWrapX(),MAP.isWrapY())
		diffTitle += space(4) + oldDict[ 'TITLE' ] + ": " + oldDict[ 'TEXT' ] + "\n"
		diffTitle += space(4) + newDict[ 'TITLE' ] + ": " + newDict[ 'TEXT' ] + "\n\n"
		# build difference dictionary
		dDiff = {}
		oldKeys = oldDict.keys()
		for lin in newDict.keys():
			if lin == 'TEXT': continue
			if lin == 'TITLE': continue
			if lin == 'REGION': continue

			# line not found in old map
			if lin not in oldKeys:
				print "[MST] Error! Key: %r is only in new dict" % ( lin )
				printDict( newDict[lin], "New Map", prefix="[MST] " )
				printDict( oldDict[lin], "Old Map", prefix="[MST] " )
				return False
			# both lines are empty
			if (newDict[lin] == None) and (oldDict[lin] == None):
				dDiff[lin] = None
				continue
			# one of the lines is empty
			if (newDict[lin] == None) or (oldDict[lin] == None):
				print "[MST] Error! Key: %r is None in one dict" % ( lin )
				printDict( newDict[lin], "New Map", prefix="[MST] " )
				printDict( oldDict[lin], "Old Map", prefix="[MST] " )
				return False
			# lines have different length
			if len(newDict[lin]) != len(oldDict[lin]):
				print "[MST] Error! Different line-length in dicts. Line: %r (new %i, old %i)" % ( lin,len(newDict[lin]),len(oldDict[lin]) )
				printDict( newDict[lin], "New Map", prefix="[MST] " )
				printDict( oldDict[lin], "Old Map", prefix="[MST] " )
				return False

			# make map
			# - may need adjustment for maps which play with more lines and chars per plot
			s = ""
			if self.__linesPerPlot > 1:
				# if more than 1 line (rivermap), we build only changed new plots
				for i in range( len( newDict[lin] ) ):
					s += iif( newDict[lin][i]==oldDict[lin][i], " ", newDict[lin][i] )
			else:
				# if one line (not rivermap), we use first char only and build diffs
				for i in range( len( newDict[lin] )/self.__charsPerPlot ):
					sNew = newDict[lin][self.__charsPerPlot * i]
					sOld = oldDict[lin][self.__charsPerPlot * i]
					if sNew == sOld:
						s += " ,,"
					else:
						s += " " + sOld + sNew
			dDiff[lin] = s
		# define map parameters
		cPP = iif( self.__linesPerPlot > 1, self.__charsPerPlot, 3 )	# may have to change
		lPP = iif( self.__linesPerPlot > 1, self.__linesPerPlot, 1 )	# redundant for now
		self.definePrintMap( dDiff, cPP, lPP, mapTitle=diffTitle, mapText = "" )
		return True

	# print areas, 1*1 chars/plot
	# build the map by filling the map-dictionary and defining the other class-variables
	# use default area-dictionary or a user supplied dictionary
	# - note that areas are not recalculated
	def buildAreaMap( self, bDiffDict=False, sText=None, region=None, areaID=None, areaDict=None ):
		# get title
		self.__mapTitle = "Area Map"
		if sText == None:
			self.__mapText = ""
		else:
			self.__mapText = sText

		# get terrain dictionary
		if areaDict == None:
			dAreas = self.__areaDict
		else:
			dAreas = areaDict

		# make legend
		plotLine1 = [ "A-Z,a-z Continents", "_.,:; Ocean" ]
		self.__mapLegend  = self.makeMapLegend( dAreas, plotLine1 )

		# get printRange
		if region == None:
			x0, x1, y0, y1	= [0, iNumPlotsX-1, 0, iNumPlotsY-1]
		else:
			x0, x1, y0, y1	= region

		# empty map-dictionary and set map-parameter
		self.__lines = {}
		self.__charsPerPlot = 1
		self.__linesPerPlot = 1
		self.__offset = [ 0 ]
		self.__mapRegion = [x0, x1, y0, y1]

		# get all areas
		areaList = GC.getMap().areas()
		aList = [ (area.getNumTiles(), area.getID(), area.isWater()) for area in areaList ]
		aList.sort()
		aList.reverse()
		# build areaID-dictionary
		aDict = {}
		waterCnt = 0
		landCnt = 0
		actID = iif( type(areaID)==type(0), areaID, -1 )
		for tiles, id, bWater in aList:
			if id == actID:
				c = dAreas["Special"][0]
			elif bWater:
				if waterCnt < len( dAreas["Ocean"][0] ):
					c = dAreas["Ocean"][0][waterCnt]
					waterCnt += 1
				else:
					c = dAreas["Water"][0]
			else:
				if landCnt < len( dAreas["Continent"][0] ):
					c = dAreas["Continent"][0][landCnt]
					landCnt += 1
				else:
					c = dAreas["Land"][0]
			aDict[ id ] = c
			aList = aDict.keys()

		# build map-dictionary
		for y in range(y0,y1+1):
			linPlots = ""
			for x in range(x0,x1+1):
				xx, yy = normalizeXY(x, y)
				pl = MAP.plot(xx, yy)
				id = pl.getArea()
				if id in aList:
					linPlots += aDict[ id ]
				else:
					linPlots += "?"									# something is wrong - unknown area
			# place lines into dictionary
			self.__lines[ self.__offset[0] + y ] = linPlots
		# print the feature-map
		self.printMap(bDiffDict, "AREAS")

	# print plots, may use data list, 1*1 chars/plot
	# build the map by filling the map-dictionary and defining the other class-variables
	def buildPlotMap( self, bDiffDict=False, sText=None, region=None, data=None ):
		# get title
		self.__mapTitle = "Plot Map"
		if sText == None:
			self.__mapText = ""
		else:
			self.__mapText = sText

		# make legend
		self.__mapLegend  = "      . Water   - Flatlands   h Hills   M Peak \n"

		# get printRange
		if region == None:
			# no region --> full map
			x0, x1, y0, y1	= [0, iNumPlotsX-1, 0, iNumPlotsY-1]
		elif type( region ) == type( 0 ):
			# areaID --> find region for area
			x0, x1, y0, y1 = getRegion( region )
		else:
			x0, x1, y0, y1	= region

		# empty map-dictionary and set map-parameter
		self.__lines = {}
		self.__charsPerPlot = 1
		self.__linesPerPlot = 1
		self.__offset = [ 0 ]
		self.__mapRegion = [x0, x1, y0, y1]

		# build map-dictionary
		if data == None:
			for y in range(y0,y1+1):
				linPlots = ""
				for x in range(x0,x1+1):
					xx, yy = normalizeXY(x, y)
					pl = MAP.plot(xx, yy)
					if	pl.isWater():
						linPlots += "."
						continue
					elif pl.isFlatlands():
						linPlots += "-"
					elif pl.isHills():
						linPlots += "h"
					elif pl.isPeak():
						linPlots += "M"
					else:
						linPlots += "X"				# something is wrong
				# place lines into dictionary; we don't want the flow from y==0 anyway
				self.__lines[ self.__offset[0]+y ] = linPlots
		else:
			for y in range(y0,y1+1):
				linPlots = ""
				for x in range(x0,x1+1):
					xx, yy = normalizeXY(x, y)
					i = MAP.plotNum(xx,yy)
					if	data[i] == PlotTypes.PLOT_OCEAN:
						linPlots += "."
						continue
					elif data[i] == PlotTypes.PLOT_LAND:
						linPlots += "-"
					elif data[i] == PlotTypes.PLOT_HILLS:
						linPlots += "h"
					elif data[i] == PlotTypes.PLOT_PEAK:
						linPlots += "M"
					else:
						linPlots += "X"				# something is wrong
				# place lines into dictionary; we don't want the flow from y==0 anyway
				self.__lines[ self.__offset[0]+y ] = linPlots
		# print the plot-map
		self.printMap( bDiffDict, "PLOT" )

	# print terrain, 1*1 chars/plot
	# build the map by filling the map-dictionary and defining the other class-variables
	# use default terrain-dictionaries or a user supplied dictionary
	def buildTerrainMap( self, bDiffDict=False, sText=None, region=None, terrainDict=None, showPlots=True ):
		# get title
		self.__mapTitle = "Terrain Map"
		if sText == None:
			self.__mapText = ""
		else:
			self.__mapText = sText

		# get terrain dictionary
		if terrainDict == None:
			dTerrains = self.__terrainDict
		else:
			dTerrains = terrainDict
		knownTerrains = dTerrains.keys()

		# make legend
		plotLine = [ "_ Flatland", "- Hills", "^ Peak" ]
		self.__mapLegend  = self.makeMapLegend( dTerrains, iif( showPlots, plotLine, None ) )

		# get printRange
		if region == None:
			# no region --> full map
			x0, x1, y0, y1	= [0, iNumPlotsX-1, 0, iNumPlotsY-1]
		elif type( region ) == type( 0 ):
			# areaID --> find region for area
			x0, x1, y0, y1 = getRegion( region )
		else:
			x0, x1, y0, y1	= region

		# empty map-dictionary and set map-parameter
		self.__lines = {}
		self.__charsPerPlot = 1
		self.__linesPerPlot = 1
		self.__offset = [ 0 ]
		self.__mapRegion = [x0, x1, y0, y1]

		# build map-dictionary
		sprint = ""
		for y in range(y0,y1+1):
			linPlots = ""
			for x in range(x0,x1+1):
				xx, yy = normalizeXY(x, y)
				pl = MAP.plot(xx, yy)
				ter = pl.getTerrainType()
				if ter == -1:											# no terrain!
					if showPlots:
						if pl.isPeak():
							linPlots += "^"
						elif pl.isWater():
							linPlots += " "
						elif pl.isHills():
							linPlots += "-"
						else:
							linPlots += "_"
					else:
						linPlots += " "
				elif ter in knownTerrains:
					if len( dTerrains[ter][0] ) > 1:
						if pl.isHills():
							linPlots += dTerrains[ter][0][1]
						else:
							linPlots += dTerrains[ter][0][0]
					else:
						linPlots += dTerrains[ter][0]
				else:
					linPlots += "?"									# something is wrong - unknown terrain
					sprint += "[MST] Unknown Terrain @ %i,%i - %i.%r \n" % (xx, yy, ter, GC.getTerrainInfo(ter).getType())
					sprint += "[MST] - known Terrains: %r \n" % (dTerrains)
			# place lines into dictionary
			self.__lines[ self.__offset[0]+y ] = linPlots
		# print errors, if any
		if sprint != "": print sprint
		# print the terrain-map
		self.printMap( bDiffDict, "TERRAIN" )

	# print features, 1*1 chars/plot
	# build the map by filling the map-dictionary and defining the other class-variables
	# use default feature-dictionaries or a user supplied dictionary
	def buildFeatureMap( self, bDiffDict=False, sText=None, region=None, featureDict=None, showPlots=True ):
		# get title
		if DEBUG:
			printDict( self.__featureDict, prefix="[MST] " )
		self.__mapTitle = "Feature Map"
		if sText == None:
			self.__mapText = ""
		else:
			self.__mapText = sText

		# get terrain dictionary
		if featureDict == None:
			dFeatures = self.__featureDict
		else:
			dFeatures = featureDict
		knownFeatures = dFeatures.keys()

		# make legend
		plotLine = [ "_ Flatland", "- Hills", "^ Peak" ]
		self.__mapLegend  = self.makeMapLegend( dFeatures, iif( showPlots, plotLine, None ) )

		# get printRange
		if region == None:
			# no region --> full map
			x0, x1, y0, y1	= [0, iNumPlotsX-1, 0, iNumPlotsY-1]
		elif type( region ) == type( 0 ):
			# areaID --> find region for area
			x0, x1, y0, y1 = getRegion( region )
		else:
			x0, x1, y0, y1	= region

		# empty map-dictionary and set map-parameter
		self.__lines = {}
		self.__charsPerPlot = 1
		self.__linesPerPlot = 1
		self.__offset = [ 0 ]
		self.__mapRegion = [x0, x1, y0, y1]

		# build map-dictionary
		sprint = ""
		for y in range(y0,y1+1):
			linPlots = ""
			for x in range(x0,x1+1):
				xx, yy = normalizeXY(x, y)
				pl = MAP.plot(xx, yy)
				feat = pl.getFeatureType()
				if feat == -1:											# no feature!
					if showPlots:
						if pl.isPeak():
							linPlots += "^"
						elif pl.isWater():
							linPlots += " "
						elif pl.isHills():
							linPlots += "-"
						else:
							linPlots += "_"
					else:
						linPlots += " "
				elif feat in knownFeatures:
					if len( dFeatures[feat][0] ) > 1:
						if pl.isHills():
							linPlots += dFeatures[feat][0][1]
						else:
							linPlots += dFeatures[feat][0][0]
					else:
						linPlots += dFeatures[feat][0]
				else:
					linPlots += "?"									# something is wrong - unknown feature
					sprint += "[MST] Unknown Feature @ %i,%i - %i.%r \n" % (xx, yy, feat, GC.getFeatureInfo(feat).getType())
					sprint += "[MST] - known Features: %r \n" % (dFeatures)
			# place lines into dictionary
			self.__lines[ self.__offset[0]+y ] = linPlots
		# print errors, if any
		if sprint != "": print sprint
		# print the feature-map
		self.printMap( bDiffDict, "FEATURE" )

	# print boni, 1*1 chars/plot
	# build the map by filling the map-dictionary and defining the other class-variables
	# use default bonus-dictionaries or a user supplied dictionary
	def buildBonusMap( self, bDiffDict=False, sText=None, region=None, bonusDict=None, showPlots=True ):
		# get title
		if DEBUG:
			printDict( self.__bonusDict, prefix="[MST] " )
		self.__mapTitle = "Bonus Map"
		if sText == None:
			self.__mapText = ""
		else:
			self.__mapText = sText

		# get terrain dictionary
		if bonusDict == None:
			dBonus = self.__bonusDict
		else:
			dBonus = bonusDict
		knownBoni = dBonus.keys()

		# make legend
		plotLine1 = [ "_ Flatland", "- Hills", "^ Peak", "# StartingPlot" ]
		plotLine2 = [ "# StartingPlot" ]
		self.__mapLegend  = self.makeMapLegend( dBonus, iif( showPlots, plotLine1, plotLine2 ) )

		# get printRange
		if region == None:
			# no region --> full map
			x0, x1, y0, y1	= [0, iNumPlotsX-1, 0, iNumPlotsY-1]
		elif type( region ) == type( 0 ):
			# areaID --> find region for area
			x0, x1, y0, y1 = getRegion( region )
		else:
			x0, x1, y0, y1	= region

		# empty map-dictionary and set map-parameter
		self.__lines = {}
		self.__charsPerPlot = 1
		self.__linesPerPlot = 1
		self.__offset = [ 0 ]
		self.__mapRegion = [x0, x1, y0, y1]

		# get starting-plots
		startPlotList = []
		playerList = mapStats.getCivPlayerList()
		for player in playerList:
			plot = player.getStartingPlot()
			startPlotList.append( indexByPlot(plot) )

		# build map-dictionary
		sprint = ""
		for y in range(y0,y1+1):
			linPlots = ""
			for x in range(x0,x1+1):
				xx, yy = normalizeXY(x, y)
				pl = MAP.plot(xx, yy)
				inx = indexByPlot( pl )
				bon = pl.getBonusType(-1)
				if inx in startPlotList:
					linPlots += "#"
				elif bon == -1:											# no bonus!
					if showPlots:
						if pl.isPeak():
							linPlots += "^"
						elif pl.isWater():
							linPlots += " "
						elif pl.isHills():
							linPlots += "-"
						else:
							linPlots += "_"
					else:
						linPlots += " "
				elif bon in knownBoni:
					if len( dBonus[bon][0] ) > 1:
						if pl.isHills():
							linPlots += dBonus[bon][0][1]
						else:
							linPlots += dBonus[bon][0][0]
					else:
						linPlots += dBonus[bon][0][0]
				else:
					linPlots += "?"									# unknown bonus
#					sprint += "[MST] Unknown Bonus @ %i,%i - %i.%r \n" % (xx, yy, bon, GC.getFeatureInfo(bon).getType())
#					sprint += "[MST] - known Boni: %r \n" % (dBonus)
			# place lines into dictionary
			self.__lines[ self.__offset[0]+y ] = linPlots
		# print errors, if any
		if sprint != "": print sprint
		# print the feature-map
		self.printMap( bDiffDict, "BONUS" )

	# print plots, rivers and starting-plots, 3*2 chars/plot
	# build the map by filling the map-dictionary and defining the other class-variables
	def buildRiverMap( self, bDiffDict=False, sText=None, region=None ):

		# get title
		self.__mapTitle = "River Map"
		if sText == None:
			self.__mapText = ""
		else:
			self.__mapText = sText

		# make legend
		self.__mapLegend  = "      . Water   - Flatlands   h Hills   M Peak \n"
		self.__mapLegend += "      nn  StartingPlot for player nn  \n"
		self.__mapLegend += "      v,  <<,  ^,  >>   rivers with flow-direction"

		# get printRange
		# get printRange
		if region == None:
			# no region --> full map
			x0, x1, y0, y1	= [0, iNumPlotsX-1, 0, iNumPlotsY-1]
		elif type( region ) == type( 0 ):
			# areaID --> find region for area
			x0, x1, y0, y1 = getRegion( region )
		else:
			x0, x1, y0, y1	= region
		numX = x1 - x0 + 1
		numY = y1 - y0 + 1

		# empty map-dictionary and set map-parameter
		self.__lines = {}
		self.__charsPerPlot = 3
		self.__linesPerPlot = 2
		self.__offset = [ 0, 1000 ]
		self.__mapRegion = [x0, x1, y0, y1]

		# build map-dictionary
		for y in range(y0,y1+1):
			linPlots = ""
			linFlows = ""
			for x in range(x0,x1+1):
				xx,yy = normalizeXY( x, y )
				pl = MAP.plot(xx,yy)
				if	pl.isWater():
					linPlots += ".. "
					linFlows += ".. "
					continue
				elif pl.isFlatlands():
					linPlots += "--"
				elif pl.isHills():
					linPlots += "hh"
				elif pl.isPeak():
					linPlots += "MM"
				else:
					linPlots += "XX"				# something is wrong
				if pl.isNOfRiver() or pl.isWOfRiver():
					if pl.isNOfRiver():
						if pl.getRiverWEDirection() == CardinalDirectionTypes.CARDINALDIRECTION_WEST:
							linFlows += "<< "
						else:
							linFlows += ">> "
					else:
						linFlows += "   "
					if pl.isWOfRiver():
						if pl.getRiverNSDirection() == CardinalDirectionTypes.CARDINALDIRECTION_NORTH:
							linPlots += "^"
						else:
							linPlots += "v"
					else:
						linPlots += " "
				else:
					linPlots += " "
					linFlows += "   "
			# place lines into dictionary; we don't want the flow from y==0 anyway
			self.__lines[ self.__offset[0]+y ] = linPlots
			self.__lines[ self.__offset[1]+y ] = linFlows
		# adjust for vertical wrap
		if MAP.isWrapY():
			self.__lines[ self.__offset[1]+numY ] = self.__lines[ self.__offset[1] ]
		self.__lines[ self.__offset[1] ] = None

		# get starting-plots if available
		startingPlots = []
		for p in mapStats.getCivPlayerList():
			player = GC.getPlayer( p.getID() )
			startingPlots.append( [p.getID(), player.getStartingPlot().getX(), player.getStartingPlot().getY()] )
		if len(startingPlots) > 0:
			# if starting-plots already exist: put them on the map
			for n,x,y in startingPlots:
				if (x != -1) and (y != -1):
					lin = self.__lines[ self.__offset[0]+y ]
					off = self.__charsPerPlot*x
					self.__lines[ self.__offset[0]+y ] = strExchange( lin, off, ("%02i"%(n)) )
		# print the river-map
		self.printMap( bDiffDict, "RIVER" )

	###############
	### Helpers ###
	###############

	# make map-legend string from dictionary
	def makeMapLegend( self, dict, addLine=None, bRiver=None, bPlayer=None ):
		offset = 6
		iMax = 4

		vList = dict.values()
		vList.sort( key = lambda test: len(test[1]) )
		vList.reverse()
		nTab = len( vList[0][1] )
		aTab = 0
		if not (addLine == None):
			aList = addLine
			aList.sort( key = lambda test: len(test) )
			aTab = len( aList[0] )
		nTab = max( aTab, nTab ) + 6
		if nTab > 20: iMax -= 1
		if nTab < 15: iMax += 1

		tList = [ offset, offset+nTab, offset+2*nTab, offset+3*nTab, offset+4*nTab, offset+5*nTab, offset+6*nTab ]
		sLegend = [ space(offset+(iMax+1)*nTab), space(offset+(iMax+1)*nTab), space(offset+(iMax+1)*nTab) ]
		vList.sort( key = lambda test: test[1] )

		sprint = ""
		sLine = [ "", "", "" ]
		bLeg = [ False, False, False ]
		row = [ 0, 0, 0 ]
		for leg in vList:
			# single key
			if len( leg[0] ) == 1:
				abr = leg[0]
				if abr == " ": abr = 'blank'
				sLegend[2] = strExchange( sLegend[2], tList[row[2]], abr )
				sLegend[2] = strExchange( sLegend[2], tList[row[2]]+1+len(abr), leg[1] )
				row[2] += 1
				bLeg[2] = True
				if row[2] > iMax:
					sLine[2] += sLegend[2] + "\n"
					sLegend[2] = space(offset+5*nTab)
					row[2] = 0
			# extra key for hills
			elif len( leg[0] ) == 2:
				for i in [0,1]:
					# same extra key - show only once
					if leg[0][0] == leg[0][1]:
						if i == 0:
							abr = leg[0][0]
							if abr == " ": abr = 'blank'
							sLegend[2] = strExchange( sLegend[2], tList[row[2]], abr )
							sLegend[2] = strExchange( sLegend[2], tList[row[2]]+1+len(abr), leg[1] )
							row[2] += 1
							bLeg[2] = True
							if row[2] > iMax:
								sLine[2] += sLegend[2] + "\n"
								sLegend[2] = space(offset+(iMax+1)*nTab)
								row[2] = 0
					# show all keys and values
					else:
						abr = leg[0][i]
						if abr == " ": abr = 'blank'
						sLegend[i] = strExchange( sLegend[i], tList[row[i]], abr )
						sLegend[i] = strExchange( sLegend[i], tList[row[i]]+1+len(abr), leg[1] )
						row[i] += 1
						bLeg[i] = True
						if row[i] > iMax:
							if i == 1:
								sLine[i] += strExchange( sLegend[i], tList[iMax+1], "... on Hills \n" )
							else:
								sLine[i] += sLegend[i] + "\n"
							sLegend[i] = space(offset+(iMax+1)*nTab)
							row[i] = 0
		# close all lines
		for i in [0,1,2]:
			if row[i] > 0:
				if i == 1:
					sLine[i] += strExchange( sLegend[i], tList[iMax+1], "... on Hills \n" )
				else:
					sLine[i] += sLegend[i] + "\n"
		# consolidate lines
		for i in [0,1,2]:
			if bLeg[i]:
				sprint += sLine[i]
		# add special line
		if not (addLine == None):
			sNew = space(offset+(iMax+1)*nTab)
			cnt = 0
			for s in addLine:
				sNew = strExchange( sNew, tList[cnt], s )
				cnt += 1
			sprint += sNew + "\n"
		if not (bRiver == None):
			sprint += space( offset ) + "v,  <<,  ^,  >>   rivers with flow-direction \n"
		if not (bPlayer == None):
			sprint += space( offset ) + "nn  StartingPlot for player nn \n"
		return sprint

#############################################################################################################
########## CLASS MapPrint END
#############################################################################################################
mapPrint = MapPrint()


##############################################################
########## CLASS MapStats - print statistics about map and mod
##############################################################
# mapStatistics()
# showContinents( txt=None, minPlots=3, bWater=False )
# listPlayers = getCivPlayerList()
# sprint = sprintActiveCivs()
# --- private ---
# sBoniTable = doBonusChart( boni )
# andTech, orTech = getTechPrereqLists( iTech )
# techLevel = getTechLevel( iTech )
##############################################################
class MapStats:

	techLevels = {}

	# print statistics about the map and the mod
	def mapStatistics(self):
		# Get available Plot/Terrain/Feature/Bonus/Improvement
		stats_plo  = [0] * 4
		stats_ter  = [0] * GC.getNumTerrainInfos()
		stats_feat = [0] * GC.getNumFeatureInfos()
		stats_bon  = {}
		stats_imp  = [0] * GC.getNumImprovementInfos()

		iPlayer = GC.getGame().countCivPlayersAlive()
		# Count stats for each plot
		for i in range(MAP.numPlots()):
			pl = MAP.plotByIndex(i)
			# get infos
			gpiPlot			= pl.getPlotType()
			gpiTerrain		= pl.getTerrainType()
			gpiBonus		= pl.getBonusType(-1)
			gpiFeature		= pl.getFeatureType()
			gpiImprovement	= pl.getImprovementType()
			# add up
			if gpiPlot == PlotTypes.PLOT_OCEAN:
				stats_plo[0] += 1
			elif gpiPlot == PlotTypes.PLOT_LAND:
				stats_plo[1] += 1
			elif gpiPlot == PlotTypes.PLOT_HILLS:
				stats_plo[2] += 1
			elif gpiPlot == PlotTypes.PLOT_PEAK:
				stats_plo[3] += 1
			if gpiTerrain > -1: stats_ter[gpiTerrain] += 1
			if gpiFeature > -1: stats_feat[gpiFeature] += 1
			if gpiBonus > -1:
				if gpiBonus in stats_bon:
					stats_bon[gpiBonus] += 1
				else: stats_bon[gpiBonus] = 1
			if gpiImprovement > -1: stats_imp[gpiImprovement] += 1

		# Display Plot-Statistics
		nOcean = 0
		nLand = 0
		nHills = 0
		nPeak = 0
		# count plots
		for i in range(MAP.numPlots()):
			p = MAP.plotByIndex(i)
			if p.isWater():
				nOcean += 1
			elif p.isFlatlands():
				nLand += 1
			elif p.isHills():
				nHills += 1
			elif p.isPeak():
				nPeak += 1
		aPlots = (nOcean, nLand, nHills, nPeak)
		# calc stats
		iAllLand = aPlots[1] + aPlots[2] + aPlots[3]
		if iAllLand > 0:
			print "[MST] ####################################################################### MapScriptTools:MapStats ###"
			fHills   = 100.0 * aPlots[2] / iAllLand
			fPeak    = 100.0 * aPlots[3] / iAllLand
			fAllLand = 100.0 * iAllLand / MAP.numPlots()
			# print stats
			sprint = "[MST] Plot-Stats: %i ( Water:%i, Flat:%i, Hills:%i, Peaks:%i )\n" % (MAP.numPlots(), aPlots[0], aPlots[1], aPlots[2], aPlots[3])
			sprint += "[MST] AllLand: %i, Hills/Land: %5.2f, Peaks/Land: %5.2f, Land/AllPlots: %5.2f\n" % ( iAllLand, fHills, fPeak, fAllLand )
			sprint += "[MST] " + '-'*73 + "\n"
			print sprint

		# show continents
		self.showContinents( " Continents and Islands of the World", 2, True )

		# Display available Plots
		sprint = "[MST]   Stats of available Terrain, Features etc. \n"
		sprint += "[MST]   ----------------------------------------- \n"
		sprint += "[MST]   %i Width x %i Height = %i Plots,  %2i Players" % (iNumPlotsX, iNumPlotsY, MAP.numPlots(), GC.getGame().countCivPlayersEverAlive() ) + "\n\n"
		f1 = (stats_plo[0]*100.0) / MAP.numPlots()
		f2 = (stats_plo[1]*100.0) / MAP.numPlots()
		f3 = (stats_plo[2]*100.0) / MAP.numPlots()
		f4 = (stats_plo[3]*100.0) / MAP.numPlots()
		f5 = (stats_plo[1]+stats_plo[2])*100.0 / MAP.numPlots()
		sprint += "[MST]   Plots: #0 - PLOT_OCEAN ( %4i )  = %4.1f%s Water" 		% (stats_plo[0], f1, '%%') + "\n"
		sprint += "[MST]   Plots: #1 - PLOT_LAND  ( %4i )  = %4.1f%s Land   }" 	% (stats_plo[1], f2, '%%') + "\n"
		sprint += "[MST]   Plots: #2 - PLOT_HILLS ( %4i )  = %4.1f%s Hills  }= %4.1f%s Usable Land" % (stats_plo[2], f3, '%%', f5, '%%') + "\n"
		sprint += "[MST]   Plots: #3 - PLOT_PEAK  ( %4i )  = %4.1f%s Peaks" 		% (stats_plo[3], f4, '%%') + "\n\n"

		# Display available Mod-Resources
		sprint += self.doBonusChart(stats_bon)
		sprint += "\n"

		# Display active Civilizations
		sprint += self.sprintActiveCivs()

		# Culture Victory
		sprint += "\n\n[MST] ####################################################################### MapScriptTools:MapStats ###"
		print sprint

	# show stats of land areas; by default without smallest islands/lakes
	def showContinents( self, txt=None, minPlots=3, bWater=False ):
		sprint = ""
		aList = []
		wList = []

		# print title
		if txt == None:
			sprint += "[MST] ####################################################################### MapScriptTools:MapStats ### \n\n"
		elif txt	!= "":
			sprint += "[MST] " + txt + "\n\n"
		areas = GC.getMap().areas()
		areaValue = {}
		sprint += "[MST] Continent Areas \n"
		sprint += "[MST] --------------- \n"
		sprint += "[MST] Area ID , Total Plots, Area Value, Start-Plots, Best Yield, Boni Total/Unique, River Edges, Coastal Land\n"
		for area in areas:
			aTotalPlots = area.getNumTiles()
			if aTotalPlots < minPlots:
				continue
			if area.isWater():
				sLake = "Ocean"
				if area.isLake(): sLake = "Lake"
				wList.append( "[MST] %8i,     %4i   - Water: %s\n" % ( area.getID(), aTotalPlots, sLake ) )
			else:
				aStartPlots = area.getNumStartingPlots()
				aBestYield  = area.calculateTotalBestNatureYield()
				aTotalBoni  = area.getNumTotalBonuses()
				aUniqueBoni = area.countNumUniqueBonusTypes()
				aRiverEdges = area.getNumRiverEdges()
				aCoastLand  = area.countCoastalLand()
				areaValue[area.getID()] = aBestYield + aRiverEdges + 3 * aUniqueBoni + aCoastLand
				aList.append( "[MST] %8i,     %4i   ,  %5i    ,    %4i    ,   %5i   ,       %3i / %3i  ,    %4i    ,    %4i\n" % \
								( area.getID(), aTotalPlots, areaValue[area.getID()], aStartPlots, aBestYield, aTotalBoni, aUniqueBoni, aRiverEdges, aCoastLand ) )

		aList.sort( key = lambda test: test[18:26] )
		aList.reverse()
		wList.sort( key = lambda test: test[18:26] )
		wList.reverse()
		for s in aList: sprint += s
		if bWater:
			for s in wList: sprint += s
		print sprint

	# get list of active players
	def getCivPlayerList( self ):
		listPlayers = [ GC.getPlayer( i ) for i in range( GC.getMAX_PC_PLAYERS() )
		                                  if GC.getPlayer(i).isAlive() ]
		return listPlayers

	# Display active Civilizations
	def sprintActiveCivs(self):
		sprint = ""
		for p in self.getCivPlayerList():
			player = GC.getPlayer( p.getID() )

			plot = player.getStartingPlot()

			if player.isHuman():
				civ = '{Human}' + p.getName()
			else: civ = p.getName()

			sprint += "[MST]   Active Civilization: #%2i [%2i] - @ %3i,%2i - %s [%s]\n" % ( p.getID(), player.getTeam(), plot.getX(), plot.getY(), civ, p.getCivilizationDescription(0) )

		return sprint

	###############
	### Helpers ###
	###############

	# paint a chart for boni
	def doBonusChart(self, boni):
		bs  =   "[MST]   |Bonus#| Bonus                      |Count |Health|Happy |Strat|Yields  |"

		bs += "Revealed Tech, Era"

		bs += "\n[MST]   +------+----------------------------+------+------+------+-----+--------|"
		bs += "------------------"

		nBoni = nHeal = nHap = nMana = 0
		for iBonus in xrange(GC.getNumBonusInfos()):
			bonus = GC.getBonusInfo(iBonus)
			if iBonus in boni:
				iNumBonuses = boni[iBonus]
			else: iNumBonuses = 0

			if not iNumBonuses and bonus.getPlacementOrder < 0:
				continue

			if bonus.isTerrain(etCoast) or bonus.isTerrain(etOcean):
				type_string = "*" + bonus.getType()

			elif bonus.isFeatureTerrain(etCoast) or bonus.isFeatureTerrain(etOcean):
				type_string = "*" + bonus.getType()
			else:
				type_string = " " + bonus.getType()

			sHeal = sHap = ".."
			sStrat = "."
			nY0 = nY1 = nY2 = 0
			sMana = "...."

			iHealth    = bonus.getHealth()
			if iHealth != 0: sHeal = "%2i" % iHealth

			iHappiness = bonus.getHappiness()
			if iHappiness != 0: sHap = "%2i" % iHappiness

			if (iHealth<=0) and (iHappiness<=0) and (type_string.find("_MANA")<0):
				sStrat = "X"

			sYield0 = "%2i" % (bonus.getYieldChange(0))
			sYield1 = "%2i" % (bonus.getYieldChange(1))
			sYield2 = "%2i" % (bonus.getYieldChange(2))

			iTech = bonus.getTechReveal()
			if iTech in range( GC.getNumTechInfos() ):
				sTech = GC.getTechInfo(iTech).getType()
				sTech = sTech.replace("_", " ")
				sTech = capWords( sTech[5:] )
				iTechEra = GC.getTechInfo(iTech).getEra()
				sTechEra = GC.getEraInfo(iTechEra).getType()
				sTechEra = sTechEra.replace("_", " ")
				sTechEra = capWords( sTechEra[4:] )

			bs += "\n[MST]   | %3i  | %-27s| %4i |..%2s..|..%2s..|..%1s..|%2s,%2s,%2s|" % (iBonus, type_string, iNumBonuses, sHeal, sHap, sStrat, sYield0, sYield1, sYield2)

			if iTech>0: bs += " " + sTech + ", " + sTechEra
			nBoni += iNumBonuses
			nHeal += iNumBonuses * iHealth
			nHap  += iNumBonuses * iHappiness

		bs += "\n[MST]   +------+----------------------------+------+------+------+-----+--------+"

		bs += "------------------"

		bs += "\n[MST]   |      |                            | %4i | %4i | %4i |     |        |\n" % (nBoni, nHeal, nHap)
		return bs

	# get technology prerequisites
	def getTechPrereqLists( self, iTech ):
		return GC.getTechInfo(iTech).getPrereqAndTechs(), GC.getTechInfo(iTech).getPrereqOrTechs()

	# get technology level
	def getTechLevel( self, iTech ):
		techLevel = -1
		if self.techLevels.get(iTech,None) == None:
			if iTech in range( GC.getNumTechInfos() ):
				techLevel = 0
				andTech, orTech = self.getTechPrereqLists( iTech )
				if (len(andTech) > 0) or (len(orTech) > 0):
					aLevel = []
					amax = 0
					for a in range( len(andTech) ):
						aLevel.append( self.getTechLevel(andTech[a]) )
						amax = max( aLevel )
					oLevel = []
					omin = 0
					for o in range( len(orTech) ):
						oLevel.append( self.getTechLevel(orTech[o]) )
						omin = min( oLevel )
					techLevel = max( amax, omin ) + 1
				self.techLevels[iTech] = techLevel
		else:
			techLevel = self.techLevels[iTech]

		return techLevel



################################################################################
########## CLASS MapStats END
################################################################################
mapStats = MapStats()


################################################################################
######## INITIALIZE MapScriptTools - This is always executed
################################################################################
print "[MST] ########### pre-init MapScriptTools ########### Start"
getModInfo()
print "[MST] ########### pre-init MapScriptTools ########### End"
################################################################################


'''
MapScriptTools Manual and API  Ver.: 1.00
=========================================

Introduction:
-------------
If you're a modder working (or playing!) with map-scripts, this is for you!

The tools in this file allow you to:
 1) Introduce new terrains into any map-script.
    1a) Put Marsh Terrain on the map. If the mod supports it.
    1b) Put Deep Ocean Terrain on the map. If the mod supports it.
 2) Create odd regions on the map:
    2a) 'The Big Bog', a flat, marshy, somewhat round region, probably with a lake and some rivers within it.
    2b) 'The Big Dent', a mountainous, somewhat oval region, probably with a volcano and several
        rivers flowing from it.
    2c) The lost 'Isle Of Atlantis' / 'Numenor Island', a small, isolated island with city-ruins, roads,
        and perhaps some intact improvements. If allowed by the mod, some other goodies may also appear.
 3) Put mod-dependent features on the map:
    3a) Kelp on coastal plots
    3b) Haunted Lands in deep forest, deep desert, within and around marshes, near ruins and mountain passes.
    3c) Crystal Plains on snowy flatland plots.
 4) Greatly expand upon the BonusBalancer class from Warlords in CyMapGeneratorUtil.py.
    Allows for mod-specific bonus-lists (currenty only Civ and FFH, but you can supply your own).
    4a) Gives each player a fair chance to find the neccessary resources within a reasonable radius.
        If that radius overlaps with your neighbors, you may have to fight over that resource anyway.
    4b) Places missing boni (those boni which should have been but for some reason weren't placed by addBonus()).
    4c) Moves some minerals [Copper,Iron,Mithril,Uranium,Silver] placed on flatlands to nearby hills.
        Create those hills if necessary and wanted.
 5) Place rivers on the map.
    5a) Enable building rivers starting at the sea and moving upriver.
    5b) Put a river (or two) on smaller islands.
    5c) Automatically start river(s) from lake(s).
    5d) Produce river-lists. Print their coordinates. (As yet only of those rivers created by buildRiver())
 6) Allow teams to have nearby, separated or random starting-plots.
 7) Print maps to the Python log at "...\My Documents\My Games\Beyond the Sword\Logs\PythonDbg.log":
    7a) Area-Maps
    7b) Plot-Maps
    7c) Terrain-Maps - allow user supplied list of terrain.
    7d) Feature-Maps - allow user supplied list of features.
    7e) Bonus-Maps - allow user supplied list of boni.
x   7f) Improvement-Maps - allow user supplied list of improvements.
    7g) River-Maps (plots,river-flows and starting-plots)
    7h) Difference-Maps of an earlier map and the actual map can be produced to
        document the different stages of map building.
 8) Print stats of map and mod to the Python log at "...\My Documents\My Games\Beyond the Sword\Logs\PythonDbg.log".
 9) Prettify maps:
   9a) Connect small lakes.
   9b) Connect small islands.
   9c) Connect small terrain patches.
   9d) Remove ice from edge of map.
   9e) Adjust plot / terrain frequency to given percentage.
   9f) Reduce peaks and volcanos for graphical beautification.
10) Find the path for Civ4, Mod or Log files
11) Groups of single funtion helpers:
   11a) Deal with areas.
   11b) Deal with (cardinal) directions.
   11c) Choose randomly - don't deal with dices anymore.
   11d) Convert between plot, index and coordinates.
   11e) Check numbers of neighbor plot-types, terrains, features.
x  11f) Validate and assign starting-plots
   11z) A whole lot of other goodies.
-----
x  Not implemented yet.


Installation:
-------------
Put the file MapScriptTools.py into the ...\Beyond the Sword\Assets\Python folder. (NOT CustomAssets!)
If you don't mind to see it as an option in the map selection of 'Custom Game', you can put it
into the ...\Beyond the Sword\PublicMaps folder, but that's the second best solution, as it won't work
if the mod disallows public maps in its ini-file (it may have other quirks - this isn't well tested).


Compatibility:
--------------
The 'MapScriptTools' are compatible with BtS 3.19.
I've run a short test with Warlords 2.13 and that worked fine too.
The above statements seem to imply compatibility to BtS 3.13 / 3.17, but this isn't tested.


Import MapScriptTools:
----------------------
Before using the MapScriptTools (MST), they have to be imported by the script.
Use 'import MapScriptTools as mst' somewhere at the beginning of your file.


Output:
-------
All reports go to the Python log normally at "...\My Documents\My Games\Beyond the Sword\Logs\PythonDbg.log".
You may have to enable logging by making sure the following option is set to 1 in
"...\My Documents\My Games\Beyond the Sword\civilization.ini":

; Enable the logging system
LoggingEnabled = 1


Initialization:
---------------
To use most of the goodies within the MapScriptTools, they have to be initialized.
The getModInfo() function is used for this purpose. It should be the first
MST function executed. Putting 'mst.getModInfo()' first into
beforeGeneration() seems like a good idea. Actually the function has optional
parameters which you probably want to use.


Functions:
----------

getModInfo( mapVersion=None, defLatitude=None, sMapInfo=None )
..............................................................
  Function: Initialize MapScriptTools and print basic map parameters to the log. Identify mod if possible.

  Parameter: mapVersion    string or None   Version of map-script
                                            default: None - version will not be printed.
             defLatitude   string or None   String to be evaluated by evalLatitude(). The string-function can
                                              only see x and y as variables and should return the latitude of
                                              a plot at coordinates x,y as a value between 0 .. 90.
                                            default: None - noPolesGetLatitude(x,y) will be used as default
             sMapInfo      string or None   String to be printed into the log with infos about the
                                              selected map parameters.
                                            default: None - no info will be printed
  Return: -

evalLatitude( plot, bDegrees=True )
...................................
  Function: Evaluate defLatitude given in getModInfo(), to give the latitude for any plot.
            ( MapScriptTools doesn't know how latitudes are calculated in each different map-script.
              This is the mechanism to enable the internal functions to get the latitude for their
              climate calculations. )

  Parameter: plot       plot   Plot on the map for which the latitude is sought.
             bDegrees   bool   If True the result will be an integer between 0 .. 90,
                               else the result will be floating-point beween 0.0 .. 1.0, equator is zero.
  Return: Latitude of plot in the form indicated by bDegrees.


Instantiated Classes:
---------------------

class DeepOcean:
instance: deepOcean
.......................................................
put 'Deep Ocean' terrain into the middle of the oceans.
.......................................................
buildDeepOcean( dist=3, chDeep=80 )

........................................................
class MapPrettifier:
instance: mapPrettifier
......................................
Something to make maps look beautiful.
......................................
connectifyLakes( chConnect=75 )
deIcifyEdges( iLat=66 )
hillifyCoast( chHills=66 )
beautifyVolcanos( chHills=66 )
lumpifyTerrain( targetTerrain, sourceTer1, sourceTer2=None )
bulkifyIslands( chConnect=66, maxIsle=4 )
percentifyTerrain( targetTerTuple, *sourceTerTuples )
percentifyPlots( targetPlotType, fTargetPlotPercent, data=None, terGenerator=None )


class MarshMaker:
instance: marshMaker
.....................................................
If the mod allows marsh-terrain, it can be made here.
.....................................................
bModHasMarsh = initialize( iGrassChance=5, iTundraChance=10, tMarshHotRange=(0,18), tMarshColdRange=(45,63) )
convertTerrain( tAreaRange=None, areaID=None )
iArid = getAridity()
normalizeMarshes()


class MapRegions:
instance: mapRegions
.................................................................
Some regions on the map can be distinctive (and may have a name).
.................................................................
initialize( regDist=15 )
buildLostIsle( chance=33, minDist=7, bAliens=False )
centerPlot = theLostIsle( pCenterPlot, pList, bAliens )
buildBigBogs( iBogs=None )
namePlot = theBigBog( pCenterPlot, bBigBog=True, bBogLake=True )
buildBigDents( iDents=None )
namePlot = theBigDent( pCenterPlot, bSideways=None, chAccess=66 )
addRegionExtras()


class FeaturePlacer:
instance: featurePlacer
..................................................
Put some common mod-dependent features on the map.
..................................................
placeKelp( chKelp=20, bAll=False, bLakes=False )
placeHauntedLands( chHaunted=6 )
placeCrystalPlains( chCrystal=25 )


class BonusBalancer:
instance: bonusBalancer
.........................................................................................
Deal with resources. Balance them and make sure all are on the map and where they belong.
.........................................................................................
initialize( bBalanceOnOff=True, bMissingOnOff=True, bMineralsOnOff=True, bWideRange=False )
normalizeAddExtras( *lResources )
bValid = isBonusValid( eBonus, pPlot, bIgnoreUniqueRange, bIgnoreOneArea, bIgnoreAdjacent )
addMissingBoni()
moveMinerals( lMineralsToMove=None )


class RiverMaker:
instance: riverMaker
.............................................................................................
Build rivers coming either down from the mountains or up from the sea. Put rivers on islands.
.............................................................................................
buildRiver( pStartPlot, bDownFlow=True, ecNext=None, ecOri=None, iThisRiverID=None, riverList=None )
islandRivers( minIsle=6, maxIsle=50, areaID=None )
buildRiversFromLake( lakeAreaID=None, chRiver=66, nRivers=1, minLake=1 )
sList = outRiverList( riverList )
bEdge = isEdgeDirection( self, plot, ecDir )
bRiver =  hasRiverAtPlot( plot )
bCorner = hasRiverAtSECorner( plot )
bCorner = hasCoastAtSECorner( plot )
bCorner = hasPlotTypeAtSECorner( plot, plotType )
eCard = getBestFlowDir( plot, bDownFlow=True, bShort=False, eForbiddenList=[] )


class TeamStart:
instance: teamStart
.........................................................
Put starting-plots of team members together or separated.
.........................................................
placeTeamsTogether( bTogether=False, bSeparated=False )
bTeams = getTeams()


class MapPrint:
instance: mapPrint
.................................
Have a look at what you've built.
.................................
initialize()
definePrintMap( lines, charsPerPlot, linesPerPlot, mapTitle="", region=None, offset=None, mapLegend="" )
printMap( diffDict=None )
bSuccess = buildDiffMap( newDict, oldDict )
buildAreaMap( bDiffDict=False, sTitle=None, region=None, areaID=None, areaDict=None )
buildPlotMap( bDiffDict=False, sTitle=None, region=None, data=None )
buildTerrainMap( bDiffDict=False, sTitle=None, region=None, terrainDict=None, showPlots=True )
buildFeatureMap( bDiffDict=False, sTitle=None, region=None, featureDict=None, showPlots=True )
buildBonusMap( bDiffDict=False, sTitle=None, region=None, bonusDict=None, showPlots=True )
buildRiverMap( bDiffDict=False, sTitle=None, region=None )


class MapStats:
instance: mapStats
...............................
Don't you just love statistics?
...............................
mapStatistics()
showContinents( txt=None, minPlots=3, bWater=False )
listPlayers = getCivPlayerList()
sprint = sprintActiveCivs()


Useful Functions:
-----------------
There are quite a few little 'helper' files near the top of this file,
which may well be usefull to you. If you used 'import MapScriptTools as mst',
you can use them all by putting 'mst.' before the call.
Have a look and feel free to do with them what you want.

Notes:
------
These tools are for the english version only.
Several tests depend on text found in various xml-files. To be more precise:
The english text!
Any failure of the tests will probably just affect the stats though.

Python 2.4 doesn't really have much of a concept for private data. The
class methods below the ---private--- line are as readily accessible as
the others, but they are not part of the API and thus subject to change
without notice.
( Just like anything else really - I'm giving no guarantees, but I might think twice.)


Included Mapscripts:
--------------------
To use the MapScriptTools with existing mapscripts, just put the included template-script
into them and modify as needed. To demonstarte how it is done I've included several altered
mapscripts. Some mapscripts required additional changes - sorry, but it's not quite automated (yet)
and you still need some understanding of what you are doing.

All maps can (and will) produce Marsh terrain, if the mod supports it.
All maps can (and will) produce Deep Ocean terrain, if the mod supports it.
All maps allow for at least two more map sizes beyond 'Huge', if the mod supports them
All maps may add Map Features ( Kelp, HauntedLands, CrystalPlains ), if supported by mod (FFH only).
All maps add some rivers on small islands and from lakes
All maps support Team Start options.
All maps support any number of players, depending on the mod.
All maps produce printed maps in "...\My Documents\My Games\Beyond the Sword\Logs\PythonDbg.log"
All maps produce printed statistics in "...\My Documents\My Games\Beyond the Sword\Logs\PythonDbg.log"
Most maps use balanced resources, add missing boni and try to move some minerals to nearby hills.

Earth3_162_mst.py             by jkp1187 --> http://forums.civfanatics.com/showthread.php?t=253927
   - Adjusted starting-plot creation using: 'Temudjins Cool Starting Plots'
   - Added New Zealand, Madagascar and changed Japan a bit.
   - Added Himalayas (single BigDent at special place)

Erebus_107b_mst.py            by Cephalo --> http://forums.civfanatics.com/showthread.php?t=261688
   - Uses default river-system. Wasn't able to insert the new rivers of mapRegions.
   - Opera already introduced placing of Kelp, HauntedLands, CrystalPlains for FFH

Medium_and_Small_110_mst.py   by Sirian (Civ4 original)

PerfectWorld_206_mst.py       by Cephalo --> http://forums.civfanatics.com/showthread.php?t=310891
   - I had to fiddle a bit in generateTerrainTypes() to transform the generated terrainList
   - Uses default river-system. Wasn't able to insert the new rivers of mapRegions.

Sea_Highlands_120_mst.py      by Sirian (Civ4 original)
   - vbraun already added an option and changed some values; I changed some more to make ships useful.
   - The MapScriptTools template is at the bottom of the file.
   - Shows how to construct a moderatly complex evaluation string for evalLatitude().
   - Eliminate Whale and Pearls boni.
   - Flat map only.

SmartMap_920_mst.py           by surt --> http://forums.civfanatics.com/showthread.php?t=154989
   - Full BtS compatibility.
   - Use both .Civ4WorldBuilderSave and .CivBeyondSwordWBSave files.
   - Adjusted starting-plot creation using: 'Temudjins Cool Starting Plots'
   - Some adjustment of Map Options.
   - Lots of small changes in nearly all functions.

Tectonics_316_mst.py          by Laurent Di Cesare --> http://forums.civfanatics.com/showthread.php?t=149278
   - Adjusted starting-plot creation using: 'Temudjins Cool Starting Plots'

Ringworld3_100_mst.py         by Temudjin --> http://forums.civfanatics.com/showthread.php?t=300961
   - My very own map-script, using lots of MapScriptTools features.

FracturedWorld_100_mst.py     by Temudjin -->
   - My very own map-script, using lots of MapScriptTools features.


****************************************************************************************************
**  Thanks:                                                                                       **
**  -------                                                                                       **
**  I've looked into a lot of map-scripts and mod-code and learned much from the authors. I also  **
**  stole ideas and sometimes even parts of code, which I found useful. I'm sorry to say that     **
**  I don't remember all of my sources - my apologies and thank you all for your efforts.         **
**  Specifically I'd like to thank:                                                               **
**                                                                                                **
**  Ruff_Hi                                                                                       **
**   - Your 'Ring World' induced me into a deeper investigation of maps.                          **
**                                                                                                **
**  The CivFanatics Community                                                                     **
**   - For making all those wonderful maps and mods and                                           **
**     for providing the opportunity to have a look at how it's done.                             **
**                                                                                                **
**  The Civ4 Design Team                                                                          **
**   - By opening your game-engine, you really opend the world(s).                                **
****************************************************************************************************


For Reference:
----------------------------------------------------------------
The Map Building Process according to Temudjin
    --> also see Bob Thomas in "CvMapScriptInterface.py"
    (in ..\Assets\Python\EntryPoints)
----------------------------------------------------------------
0)     - Get Map-Options
0.1)     getNumHiddenCustomMapOptions()
0.2)     getNumCustomMapOptions()
0.3)     getCustomMapOptionDefault()
0.4)     isAdvancedMap()
0.5)     getCustomMapOptionName()
0.6)     getNumCustomMapOptionValues()
0.7)     isRandomCustomMapOption()
0.8)     getCustomMapOptionDescAt()
0.9)     - Get Map-Types
0.9.1)     isClimateMap()
0.9.2)     isSeaLevelMap()
1)     beforeInit()
2)     - Initialize Map
2.2)     getGridSize()
2.3.1)   getTopLatitude()					# always use both
2.3.2)   getBottomLatitude()				# always use both
2.4.1)   getWrapX()							# always use both
2.4.2)   getWrapY()							# always use both
3)     beforeGeneration()
4)     - Generate Map
4.1)     generatePlotTypes()
4.2)     generateTerrainTypes()
4.3)     addRivers()
4.4)     addLakes()
4.5)     addFeatures()
4.6)     addBonuses()
4.6.1)     isBonusIgnoreLatitude()*
4.7)     addGoodies()
5)     afterGeneration()
6)     - Select Starting-Plots
6.1)     minStartingDistanceModifier()
6.2)     assignStartingPlots()
7)     - Normalize Starting-Plots
7.1)     normalizeStartingPlotLocations()
7.2)     normalizeAddRiver()
7.3)     normalizeRemovePeaks()
7.4)     normalizeAddLakes()
7.5)     normalizeRemoveBadFeatures()
7.6)     normalizeRemoveBadTerrain()
7.7)     normalizeAddFoodBonuses()
7.7.1)     isBonusIgnoreLatitude()*
7.8)     normalizeGoodTerrain()
7.9)     normalizeAddExtras()
7.9.1)     isBonusIgnoreLatitude()*

* used by default 'CyPythonMgr().allowDefaultImpl()' in:
  addBonuses(), normalizeAddFoodBonuses(), normalizeAddExtras()
------------------------------------------------------------------


An Example Template:
--------------------
Just put the following into your map-script, comment-out / delete what you don't want and
make a few necessary adjustments. You probably have to rename some of your functions too.
Feel free to delete any superfluous comments if they distract to much:

##################################################################################
## MapScriptTools Interface by Temudjin
##################################################################################
import MapScriptTools as mst

# this function will be called early by the system, before any parts of tha map are created
# - define your latitude formula, get the map-version
# - initialize the MapScriptTools
# - initialize MapScriptTools.BonusBalancer - decide which parts your script wants to use
# -----------------------------------------------------------------------------------------
def beforeGeneration():
	print "-- beforeGeneration()"

	# Create evaluation string for getLatitude(x,y); note that functions of the mapscript
	# will not be visible at evaluation-time, only x and y can be seen.
	# The result should be between 0 .. 90, but a negative value will be converted.
	# i.e.: compGetLat = "((y/%5.1f) - %3.1f) * 90" % ( CyMap().getGridHeight()-1, 1 )
	#     - which would give you half a world from equator to south pole.
	# Omitting compGetLat or setting it to None, will result in using the default function,
	# which will give equally distributed latitudes between both poles.

	# Initialize MapScriptTools
	compGetLat = None
	# Create mapInfo string
	mapInfo = ""
	for opt in range( getNumCustomMapOptions() ):
		nam = getCustomMapOptionName( [opt] )
		sel = CyMap().getCustomMapOption( opt )
		txt = getCustomMapOptionDescAt( [opt,sel] )
		mapInfo += "%27s:   %s\n" % ( nam, txt )
	mst.getModInfo("Map version", compGetLat, mapInfo)

	# Initialize MapScriptTools.BonusBalancer
	# balance boni: False, place missing boni: True, move minerals: True
	mst.bonusBalancer.initialize( False, True, True )

	# Some scripts actually have an option for balancing resources.
	# in this case the call would be somewhat like this:
	# mst.bonusBalancer.initialize( (CyMap().getCustomMapOption(2)==1), True, True )

	# If the mapscript already has a function beforeGeneration(), then
	# rename it to beforeGeneration2() and uncomment the next line
#	beforeGeneration2()

# This function will be called by the system, after generatePlotTypes() and before addRivers()
# - print the plot-map and hold result to check differences to the next call
# - prettify plots
# If the map-script does more than just call the generator in generateTerrainTypes(), you will
# have to take a closer look.
# --------------------------------------------------------------------------------------------
def generateTerrainTypes():
	print "-- generateTerrainTypes()"

	# print a plotMap; allow for differenceMap with next call
	mst.mapPrint.buildPlotMap( True, "generateTerrainTypes()" )

	# Prettify the map - change coastal peaks to hills with 80% chance; default: 66%
	mst.mapPrettifier.hillifyCoast( 80 )

	# Scripts may already have their own TerrainGenerator.
	# terraingen = ThisMapTerrainGenerator()
	# If the script doesn't have it's own, you can use this one.

	# Create the terrain and return the result.
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

# this function will be called by the system, after generateTerrainTypes() and before addLakes()
# - Generate marsh terrain - converts some grass and tundra to marsh within two latitude zones.
# - build some odd regions ('The Big Bog', 'The Big Dent')
# - prettify terrain
# - make rivers on smaller islands
# ----------------------------------------------------------------------------------------------
def addRivers():
	print "-- addRivers()"

	# Generate DeepOcean-terrain if mod allows for it
	mst.deepOcean.buildDeepOcean()

	# Generate marsh-terrain within latitude zones (default: 5, 10, (0,18), (45,63) ).
	# The frequency of transformation as well as the zones may be changed by first
	# calling mst.marshMaker.initialize() with the appropriate parameters.
	mst.marshMaker.initialize( 4, 20, (0,25), (50,75) )
	mst.marshMaker.convertTerrain()
	# Solidify marsh between 2 [Arid,LowSea] and 8 [Tropical,HighSea] percent.
	marshPer = 5 - mst.marshMaker.getAridity()
	mst.mapPrettifier.percentifyTerrain( (mst.etMarsh,marshPer), (mst.etTundra,1), (mst.etGrass,2) )

	# Build between 0..3 mountain-ranges.
	mst.mapRegions.buildBigDents()

	# Build between 0..3 bog-regions.
	mst.mapRegions.buildBigBogs()

	# Some scripts produce more chaotic terrain than others. You can create more connected
	# (bigger) deserts by converting surrounded plains and grass.
	# Prettify the map - create better connected deserts and plains
	mst.mapPrettifier.lumpifyTerrain( mst.etDesert, mst.etPlains, mst.etGrass )
	mst.mapPrettifier.lumpifyTerrain( mst.etPlains, mst.etDesert, mst.etGrass )
	# Sprout rivers from lakes.
	mst.riverMaker.buildRiversFromLake()		# 66% chance to get one river from each lake

	# If the script already has an addRivers() function, you want to rename it and call it here.
#	addRivers2()									   # call renamed script function
	# If the script did not have an addRivers() function, this was called instead.
	CyPythonMgr().allowDefaultImpl()				# don't forget this

	# Put rivers on small islands.
	mst.riverMaker.islandRivers()					# islands between 6 and 50 tiles

# This function will be called by the system, after addRivers() and before addFeatures()
# - don't add lakes on Mars
# ------------------------------------------------------------------------------------
def addLakes():
	print "-- addLakes()"
	CyPythonMgr().allowDefaultImpl()

# This function will be called by the system, after addLakes() and before addBonuses()
# - prettify lakes
# - prettify volcanos
# If the map-script does more than just call the generator in addFeatures(), you will
# have to take a closer look.
# ------------------------------------------------------------------------------------
def addFeatures():
	print "-- addFeatures()"

	# Prettify the map - kill off spurious lakes; default: 75% chance
	mst.mapPrettifier.connectifyLakes( 90 )

	# Rename the scripts addFeatures() function and call it here.
	addFeatures2()							# call renamed script function
	# If the script doesn't have it's own generator, you use this one too:
	#featuregen.addFeatures()

	# Prettify the map - transform coastal volcanos; default: 66% chance
	mst.mapPrettifier.beautifyVolcanos( 80 )

# This function will be called by the system, after the map was generated, after the
# starting-plots have been choosen, at the start of the normalizing process.
# You will only need this function, if you want to use the teamStart options.
# In this example we assume that the script has a custom-option for team starts with
# 4 options: 'nearby', 'separated', 'randomize', 'ignore'.
# ----------------------------------------------------------------------------------
def normalizeStartingPlotLocations():
	print "-- normalizeStartingPlotLocations()"

	# Build Lost Isle
	# - this region needs to be placed after starting-plots are first assigned
	# - 33% chance to build lost isle, with minimum distance of 7 to next continent. 33% of those
	#   islands where build by aliens and may have intact remnants of an advanced roadway system.
	mst.mapRegions.buildLostIsle( chance=33, minDist=7, bAliens=mst.choose(33,True,False) )

	# Handle Team Start Option
	if CyMap().getCustomMapOption(2) == 0:
		CyPythonMgr().allowDefaultImpl()							# by default civ places teams near to each other
		# mst.teamStart.placeTeamsTogether( True, True )	# use teamStart to place teams near to each other
	elif CyMap().getCustomMapOption(2) == 1:
		mst.teamStart.placeTeamsTogether( False, True )		# shuffle starting-plots to separate teams
	elif CyMap().getCustomMapOption(2) == 2:
		mst.teamStart.placeTeamsTogether( True, True )		# randomize starting-plots (may be near or not)
	else:
		mst.teamStart.placeTeamsTogether( False, False )	# leave starting-plots alone

# prevent additional rivers on Mars
def normalizeAddRiver():
	print "-- normalizeAddRiver()"
	CyPythonMgr().allowDefaultImpl()

# prevent additional lakes on Mars
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

# This function will be called by the system after the starting-plots have been choosen
# at the end of the normalizing process, and is the last map-function called externally.
# - balance boni (depending on initialization also place missing boni and move minerals)
# - give names and boni to special regions
# - print plot-map and the difference-map to the call before
# - print other maps
# - print river-map with plots, rivers and starting-plots
# - print map and mod statistics
# --------------------------------------------------------------------------------------
def normalizeAddExtras():
	print "-- normalizeAddExtras()"

	# Balance boni, place missing boni and move minerals depending on initialization.
	mst.bonusBalancer.normalizeAddExtras()

	# If the script already has a normalizeAddExtras() function, you want to rename it and call it here.
	# normalizeAddExtras2()                 # call renamed script function
	# If the script already uses a bonusBalancer, you need to comment it out in
	# the original normalizeAddExtras(). You also want to look into addBonusType()
	# and comment the balancer out there too.
	# In fact if there isn't done anything else in those functions besides balancing,
	# you should comment-out both functions.
	CyPythonMgr().allowDefaultImpl()        # do the default housekeeping

	# Give extras (names and boni) to special regions
	# FFH does a lot housekeeping, but some special regions may want to overrule that.
	mst.mapRegions.addRegionExtras()

	# Place special features on map
	mst.featurePlacer.placeKelp()
	mst.featurePlacer.placeHauntedLands()
	mst.featurePlacer.placeCrystalPlains()

	# Kill ice-feature on warm edges.
	# Regardless of latitude, if Y-axis isn't wrapped Civ4 puts ice on the north- and southpoles.
	mst.mapPrettifier.deIcifyEdges()

	# Print plotMap and differencePlotMap
	mst.mapPrint.buildPlotMap( True, "normalizeAddExtras()" )

	# Print areaMap
	mst.mapPrint.buildAreaMap( False, "normalizeAddExtras()" )

	# Print terrainMap and differenceTerrainMap
	mst.mapPrint.buildTerrainMap( True, "normalizeAddExtras()" )

	# Print featureMap
	mst.mapPrint.buildFeatureMap( False, "normalizeAddExtras()" )

	# Print bonusMap
	mst.mapPrint.buildBonusMap( False, "normalizeAddExtras()" )

	# Print riverMap
	mst.mapPrint.buildRiverMap( False, "normalizeAddExtras()" )

	# Print mod and map statistics
	mst.mapStats.mapStatistics()

# This function will be called at odd times by the system.
# If the script already has this function, return that result instead of zero or rename it.
def minStartingDistanceModifier():
	return 0
##################################################################################
'''