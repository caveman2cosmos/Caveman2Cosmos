''' CvWBInterface
Interface for reading and writing scenario files.
Functions are sorted in the order they are typically called.
'''
from CvPythonExtensions import *
import CvWBDesc

WBDesc = CvWBDesc.CvWBDesc()
GC = CyGlobalContext()

#---------------------#
# Write Scenario file #
#---------------------#
def writeDesc(argsList):
	''' Called from exe
	Save out a high-level desc of the world, for WorldBuilder
	'''
	fileName = argsList[0]
	return WBDesc.write(fileName)

#--------------------#
# Read Scenario file #
#--------------------#
def readAndApplyDesc(argsList):
	''' Called from dll 'CvMap::afterSwitch()'
	Read in and apply a high-level desc of the world. In-game load only
	I have no idea when this is called.
	'''
	print "--- CvWBInterface.readAndApplyDesc(argsList) ---"
	fileName = argsList[0]

	if WBDesc.read(fileName) < 0:
		return -1
	if WBDesc.applyMap() < 0:
		return -1
	return WBDesc.applyInitialItems()

def readDesc(argsList):
	''' Called from exe
	Read in a high-level desc of the world from scenario file.
	Called once before game options can be selected for scenario,
	and called once more when all game options are confirmed.
	'''
	print "--- CvWBInterface.readDesc(argsList) ---", argsList
	return WBDesc.read(argsList[0])


def getModPath():
	''' Called from exe
	Returns the path for the Mod that this scenario should load (if applicable)
	'''
	return WBDesc.metaDesc.szModPath


def getGameData():
	''' Called from exe
	After reading a scenario file,
	return game/player data as a tuple
	'''
	print "IF getGameData"
	gameWB = WBDesc.gameDesc
	t = ()
	t += (
		GC.getInfoTypeForString(WBDesc.mapDesc.worldSize), GC.getInfoTypeForString(WBDesc.mapDesc.climate),
		GC.getInfoTypeForString(WBDesc.mapDesc.seaLevel), gameWB.iStartEra, getInfoType(gameWB.speedType), gameWB.iCalendarType
	)
	types = gameWB.options
	iLength = len(types)
	t += (iLength,)
	for i in xrange(iLength):
		t += (GC.getInfoTypeForString(types[i]),)

	types = gameWB.mpOptions
	iLength = len(types)
	t += (iLength,)
	for i in xrange(iLength):
		t += (GC.getInfoTypeForString(types[i]),)

	types = gameWB.forceControls
	iLength = len(types)
	t += (iLength,)
	for i in xrange(iLength):
		t += (GC.getInfoTypeForString(types[i]),)

	types = gameWB.victories
	iLength = len(types)
	t += (iLength,)
	for i in xrange(iLength):
		t += (GC.getInfoTypeForString(types[i]),)

	t += (gameWB.iGameTurn, gameWB.maxTurns, gameWB.maxCityElimination, gameWB.numAdvancedStartPoints, gameWB.targetScore)
	return t

def getPlayerDesc():
	''' Called from exe
	Returns player description data (wide strings) as a tuple
	'''
	print "CvWBInterface.getPlayerDesc"
	playerTuple = WBDesc.playersDesc
	t = ()
	for i in xrange(GC.getMAX_PLAYERS()):
		playerWB = playerTuple[i]
		t += (playerWB.szCivDesc, playerWB.szCivShortDesc, playerWB.szLeaderName, playerWB.szCivAdjective, playerWB.szFlagDecal)
	return t

def getPlayerData():
	''' Called from exe
	Returns player data as a tuple, terminated by -1
	Last thing called before you can select your game options for a scenario.
	'''
	print "CvWBInterface.getPlayerData"
	iNPC = GC.getMAX_PC_PLAYERS()
	playerTuple = WBDesc.playersDesc
	t = ()
	for i in xrange(GC.getMAX_PLAYERS()):
		playerWB = playerTuple[i]
		t += (
			getInfoType(playerWB.civType), playerWB.isPlayableCiv, getInfoType(playerWB.leaderType), playerWB.iHandicap,
			playerWB.iTeam, getInfoType(playerWB.color), getInfoType(playerWB.artStyle), 0, playerWB.isWhiteFlag
		)
	return t

def getInfoType(TYPE):
	if TYPE is None:
		return -1
	return GC.getInfoTypeForString(TYPE)

#---------------------#
# Apply Scenario file #
#---------------------#
def applyMapDesc():
	''' Called from exe
	Applies game and map data
	'''
	print "IF applyMapDesc"
	return WBDesc.applyMap()

def getAssignedStartingPlots():
	''' Called from exe
	Reads in starting plots for random players
	'''
	print "IF getAssignedStartingPlots"
	return WBDesc.getAssignedStartingPlots()

# After reading, applies player units, cities, and techs
def applyInitialItems(): # Called from exe
	print "IF applyInitialItems"
	return WBDesc.applyInitialItems()

#---------------#
# Miscellaneous #
#---------------#
def getMapDescriptionKey():
	print "IF getMapDescriptionKey"
	return WBDesc.metaDesc.szDescription

# If True, this is really a mod, not a scenario
def isRandomMap():
	print "IF isRandomMap"
	return 0
