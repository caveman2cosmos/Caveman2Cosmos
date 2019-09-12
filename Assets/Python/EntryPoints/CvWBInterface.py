## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvWBPopups
import CvWBDesc
import os

WBDesc = CvWBDesc.CvWBDesc()
lastFileRead = None
GC = CyGlobalContext()

lastFileChecked = None
addTeam = [
 'BeginTeam\n',
 '\tTeamID=#\n',
 '\tContactWithTeam=#\n',
 '\tRevealMap=0\n',
 'EndTeam\n'
]
addPlayer = [
 'BeginPlayer\n',
 '\tTeam=#\n',
 '\tLeaderType=NONE\n',
 '\tCivType=NONE\n',
 'EndPlayer\n'
]
addDescription = [ '\tDescription=#\n' ]

def adjustMap( fileName ):
	print "--- adjustMap(fileName) --- %s" % (fileName)
	global lastFileChecked
	global addTeam, addPlayer
	maxTeams = GC.getMAX_CIV_TEAMS()
	maxPlayers = GC.getMAX_CIV_PLAYERS()

	# test if file already adjusted
	if fileName == lastFileChecked:
		print "# %r has already been checked" % (fileName)
		return True		# success

	# normalize filename
	fileName = os.path.normpath(fileName)
	filePath,ext = os.path.splitext(fileName)
	if not ext: ext = getWBSaveExtension()
	if not os.path.isfile(filePath + ext):
		print "[ERROR] file %s does not exist" %(filePath + ext)
		return False	# failed

	# read file
	f = file(filePath+ext, "r")		# open text file
	mapFile = f.readlines()
	f.close()

	# check if adjustment is needed
	t = [i for i in xrange(len(mapFile)) if mapFile[i].find('EndTeam') > -1]
	p = [i for i in xrange(len(mapFile)) if mapFile[i].find('EndPlayer') > -1]
	if maxTeams == len(t) and maxPlayers == len(p):
		print "# Number of teams and players ok"
		lastFileChecked = fileName

	else: # adjust file - Scenario was designed for different maxPlayers
		sprint = "# Scenario %s was designed for\n" % filePath
		sprint += "# a maximum of %i players, but this Mod has a maximum of %i players.\n" %(len(p), maxPlayers)
		sprint += "# Accordingly the scenario file will be adjusted for %i players.\n" % maxPlayers
		print sprint

		# find actual maxTeam
		for i in xrange( p[-2]+1, p[-1] ):
			tea = mapFile[i].find("Team=")
			if tea >= 0:
				actMaxTeam = int( mapFile[i][tea+5:] )
				break

		# delete/insert Players
		if len(p) > maxPlayers:
			for pl in xrange( len(p)-1, maxPlayers-1, -1 ):
				fnd = False
				for i in xrange( p[pl]-1, p[pl-1], -1 ):
					if mapFile[i].find('LeaderType=') >= 0:
						if mapFile[i].find('NONE') >= 0:
							fnd = True
							break
						else:
							print "[ERROR] Unwilling to kill named leader in line %i, when adjusting to %i players" %(i+1, maxPlayers)
							return False		# failed
				if not fnd:
					print "[ERROR] Unable to find 'LeaderType' when adjusting to %i players" % maxPlayers
					return False		# failed
			mapFile = mapFile[:(p[maxPlayers-1]+1)] + mapFile[(p[len(p)-1]+1):]
		else:
			for i in xrange( maxPlayers-1, len(p)-1, -1 ):
				if len(t) > 0:
					sCnt = "%i" % ( i - (len(t)-1-actMaxTeam) )
				else:
					sCnt = "%i" % ( i )
				ins = p[-1] + 1
				mapFile = mapFile[:ins] + addPlayer + mapFile[ins:]
				mapFile[ ins+1 ] = mapFile[ ins+1 ].replace("#", sCnt)

		# delete/insert Teams
		if len(t) > maxTeams:
			mapFile = mapFile[:(t[maxTeams-1]+1)] + mapFile[(t[len(t)-1]+1):]
		else:
			if len(t) > 0:
				ins = t[-1] + 1
			else:
				# no teams have been specified
				for j in xrange(len(mapFile)):
					if mapFile[j].find( "EndGame" ) >= 0:
						ins = j + 1
						break
			for i in xrange( maxTeams-1, len(t)-1, -1 ):
				sCnt = "%i" % (i)
				mapFile = mapFile[:ins] + addTeam + mapFile[ins:]
				mapFile[ins+1] = mapFile[ins+1].replace("#", sCnt)
				mapFile[ins+2] = mapFile[ins+2].replace("#", sCnt)

		# adjust Barbarians; maximum 5 factions
		for n in xrange(5):
			ownerOld = "Owner=%2i" % (len(p) + n)
			ownerNew = "Owner=%2i" % (maxPlayers + n)
			for i in xrange(len(mapFile)):
				mapFile[i] = mapFile[i].replace(ownerOld, ownerNew)

		# adjust Description
		sMax = "%2i" % (maxPlayers)
		iDescription = -1
		iEndGame = -1
		for i in xrange( len(mapFile) ):
			if mapFile[i].find( "Description=" ) > -1:
				iDescription = i
				break
		for i in xrange( len(mapFile) ):
			if mapFile[i].find( "EndGame" ) > -1:
				iEndGame = i
				break
		if (iDescription > -1) and (iDescription < iEndGame):
			# change original description
			sprint = "# Old: %r\n" % mapFile[iDescription]
			adj = mapFile[iDescription].find( "[Adjusted for " )
			if adj > -1:
				mapFile[iDescription] = mapFile[iDescription][:(adj+14)] + sMax + mapFile[iDescription][(adj+16):]
			elif mapFile[iDescription].find( "TXT_KEY_" ) == -1:
				e = mapFile[iDescription].rfind( "\n" )
				mapFile[iDescription] = mapFile[iDescription][:e] + " [Adjusted for " + sMax + " Civs]\n"
			sprint += "# New: %r\n" % mapFile[iDescription]
		else:
			# there is no original description
			sprint = "# Old: - No Description -\n"
			mapFile = mapFile[:iEndGame] + addDescription + mapFile[iEndGame:]
			mapFile[ iEndGame ] = mapFile[ iEndGame ].replace("#", "[Adjusted for " + sMax + " Civs]")
			sprint += "# New: %r\n" % mapFile[iEndGame]
		print sprint

		# save modified file
		print "# Save adjusted scenario file as: %s" %( filePath+ext )
		try:
			f = open( filePath+ext, "w" )
			f.writelines( mapFile )
			f.close()
		except:
			print "[ERROR] Unable to save new scenario-file: %r\n-----> Printing scenario-file to log" % filePath+ext
			# print file to log
			sprint = ""
			sprint += "# UNSAVED SCENARIO FILE: %r\n" % (filePath+ext)
			sprint += "# ----------------------\n"
			cnt = 1
			for line in mapFile:
				sprint += "# %5i: %r\n" % (cnt, line)
				cnt += 1
			print sprint
			return False		# failed
	return True		# success

# Save out a high-level desc of the world, for WorldBuilder
def writeDesc(argsList):
	fileName = argsList[0]
	global lastFileRead
	lastFileRead=None
	return WBDesc.write(fileName)

# Read in and apply a high-level desc of the world.  In-game load only
def readAndApplyDesc(argsList):
	print "--- readAndApplyDesc(argsList) ---"
	fileName = argsList[0]
	if not adjustMap(fileName): return -1

	if WBDesc.read(fileName) < 0:
		return -1
	if WBDesc.applyMap() < 0:
		return -1
	return WBDesc.applyInitialItems()

# Read in a high-level desc of the world, for WorldBuilder.  Must call applyMap and applyInitialItems to finish the process
def readDesc(argsList):
	print "--- readDesc(argsList) ---"
	fileName = argsList[0]
	if not adjustMap(fileName): return -1

	global lastFileRead
	if fileName != lastFileRead:
		ret = WBDesc.read(fileName)
		if ret == 0:
			lastFileRead = fileName
	else:
		ret = 0
	return ret

# After reading, applies the map loaded data
def applyMapDesc():
	return WBDesc.applyMap()

# After reading, applies player units, cities, and techs
def applyInitialItems():
	return WBDesc.applyInitialItems()

# Reads in starting plots for random players
def getAssignedStartingPlots():
	return WBDesc.getAssignedStartingPlots()

# Called from the Worldbuilder app - sends to CvWBPopups for handling
def initWBEditor(argsList):
	return CvWBPopups.CvWBPopups().initWB(argsList)

# after reading a save file, return game/player data as a tuple
def getGameData():
	t = ()
	t += (
		GC.getInfoTypeForString(WBDesc.mapDesc.worldSize), GC.getInfoTypeForString(WBDesc.mapDesc.climate), GC.getInfoTypeForString(WBDesc.mapDesc.seaLevel),
		GC.getInfoTypeForString(WBDesc.gameDesc.eraType), GC.getInfoTypeForString(WBDesc.gameDesc.speedType), GC.getInfoTypeForString(WBDesc.gameDesc.calendarType)
	)
	iLength = len(WBDesc.gameDesc.options)
	t += (iLength,)
	for i in xrange(iLength):
		t += (GC.getInfoTypeForString(WBDesc.gameDesc.options[i]),)

	iLength = len(WBDesc.gameDesc.mpOptions)
	t += (iLength,)
	for i in xrange(iLength):
		t += (GC.getInfoTypeForString(WBDesc.gameDesc.mpOptions[i]),)

	iLength = len(WBDesc.gameDesc.forceControls)
	t += (iLength,)
	for i in xrange(iLength):
		t += (GC.getInfoTypeForString(WBDesc.gameDesc.forceControls[i]),)

	iLength = len(WBDesc.gameDesc.victories)
	t += (iLength,)
	for i in xrange(iLength):
		t += (GC.getInfoTypeForString(WBDesc.gameDesc.victories[i]),)

	t += (WBDesc.gameDesc.gameTurn, WBDesc.gameDesc.maxTurns, WBDesc.gameDesc.maxCityElimination, WBDesc.gameDesc.numAdvancedStartPoints, WBDesc.gameDesc.targetScore)
	return t

# Returns the path for the Mod that this scenario should load (if applicable)
def getModPath():
	return WBDesc.gameDesc.szModPath

# Returns the TXT_KEY Description of the map to be displayed in the map/mod selection screen
def getMapDescriptionKey():
	return WBDesc.gameDesc.szDescription

# If True, this is really a mod, not a scenario
def isRandomMap():
	return WBDesc.gameDesc.iRandom

# after reading a save file, return player data as a tuple, terminated by -1
def getPlayerData():
	t = ()
	for i in xrange(GC.getMAX_CIV_PLAYERS()):
		t += (
			GC.getInfoTypeForString(WBDesc.playersDesc[i].civType), WBDesc.playersDesc[i].isPlayableCiv, GC.getInfoTypeForString(WBDesc.playersDesc[i].leaderType),
			GC.getInfoTypeForString(WBDesc.playersDesc[i].handicap), WBDesc.playersDesc[i].team, GC.getInfoTypeForString(WBDesc.playersDesc[i].color),
			GC.getTypesEnum(WBDesc.playersDesc[i].artStyle), WBDesc.playersDesc[i].isMinorNationCiv, WBDesc.playersDesc[i].isWhiteFlag
		)
	return t

# after reading a save file, return player description data (wide strings) as a tuple
def getPlayerDesc():
	t = ()
	for i in xrange(GC.getMAX_CIV_PLAYERS()):
		t += (WBDesc.playersDesc[i].szCivDesc, WBDesc.playersDesc[i].szCivShortDesc, WBDesc.playersDesc[i].szLeaderName, WBDesc.playersDesc[i].szCivAdjective, WBDesc.playersDesc[i].szFlagDecal)
	return t
