## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvWBPopups
import CvUtil
import CvWBDesc

# globals
WBDesc = CvWBDesc.CvWBDesc()
lastFileRead = None
gc = CyGlobalContext()

########## Temudjin START 15.Feb.2010
# CHANGELOG
# 15.Feb.2010
# - fix: allow Civ4 and Warlords scenario files
#   NOTE: Renaming them beforehand to .CivBeyondSwordWBSave files is recommended.
# 06.Feb.2010
# - fix: allow for missing team infos
# - fix: allow for missing description in game info
# - fix: allow for changes in player info sequence
# - minimize default player info
# 02.Feb.2010
# - fix: allow for multiple Barbarian factions (like 'Fall Further')
# 30.Jan.2010
# - initial release
##########
import os
BACKUP = False				# usually backups are a good idea

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
	maxTeams = gc.getMAX_CIV_TEAMS()
	maxPlayers = gc.getMAX_CIV_PLAYERS()

	# test if file already adjusted
	if fileName == lastFileChecked:
		print "# %r has already been checked" % (fileName)
		return True		# success

	# normalize filename
	fileName = os.path.normpath( fileName )
	filePath,ext = os.path.splitext( fileName )
	if len(ext) == 0: ext = getWBSaveExtension()
	if (not os.path.isfile(filePath+ext)):
		CvUtil.pyPrint("Error: file %s does not exist" %(filePath+ext,))
		return False	# failed

	# read file
	f = file(filePath+ext, "r")		# open text file
	mapFile = f.readlines()
	f.close()

	# check if adjustment is needed
	t = [ i for i in range(len(mapFile)) if mapFile[i].find('EndTeam')>-1 ]
	p = [ i for i in range(len(mapFile)) if mapFile[i].find('EndPlayer')>-1 ]
	if ( maxTeams == len(t) ) and ( maxPlayers == len(p) ):
		print "# Number of teams and players ok"
		lastFileChecked = fileName

	# adjust file - Scenario was designed for different maxPlayers
	else:
		sprint = ""
		sprint += "# Scenario %s was designed for\n" % (filePath)
		sprint += "# a maximum of %i players, but this Mod has a maximum of %i players.\n" % (len(p),maxPlayers)
		if BACKUP:
			sprint += "# Accordingly the original scenario file will be saved under a different name,\n"
			sprint += "# adjusted for %i players and then saved under the original name.\n" % (maxPlayers)
		else:
			sprint += "# Accordingly the scenario file will be adjusted for %i players.\n" % (maxPlayers)
		print sprint

		# backup original file
		if BACKUP:
			# get new file-name - fileName is actually set to lowercase most times for some reason
			# NOTE: The backups from Civ4 and Warlords will not be automatically included into the
			#       new list of BtS scenarios that might be choosen during the same session.
			#       >>> There will be Error-Messages about this <<<
			#       However this error can be savely ignored.
			if ext.lower() == '.civ4worldbuildersave':
				ext = '.Civ4WorldBuilderSave'
			elif ext.lower() == '.civwarlordswbsave':
				ext = '.CivWarlordsWBSave'
			elif ext.lower() == '.civbeyondswordwbsave':
				ext = '.CivBeyondSwordWBSave'

			oldFile = filePath + ( " [maxPlayers %2i]" % (len(p)) ) + ext
			print "# Backup original scenario file as: %s" %( oldFile )
			try:
				f = open( oldFile, "w" )
				f.writelines( mapFile )
				f.close()
			except:
				CvUtil.pyPrint("Error: Unable to backup file as: %r" %( oldFile, ))
				return False		# failed

		# find actual maxTeam
		for i in range( p[-2]+1, p[-1] ):
			tea = mapFile[i].find("Team=")
			if tea >= 0:
				actMaxTeam = int( mapFile[i][tea+5:] )
				break

		# delete/insert Players
		if len(p) > maxPlayers:
			for pl in range( len(p)-1, maxPlayers-1, -1 ):
				fnd = False
				for i in range( p[pl]-1, p[pl-1], -1 ):
					if mapFile[i].find('LeaderType=') >= 0:
						if mapFile[i].find('NONE') >= 0:
							fnd = True
							break
						else:
							CvUtil.pyPrint("Error: Unwilling to kill named leader in line %i, when adjusting to %i players" %( i+1, maxPlayers, ))
							return False		# failed
				if not fnd:
					CvUtil.pyPrint("Error: Unable to find 'LeaderType' when adjusting to %i players" %( maxPlayers, ))
					return False		# failed
			mapFile = mapFile[:(p[maxPlayers-1]+1)] + mapFile[(p[len(p)-1]+1):]
		else:
			for i in range( maxPlayers-1, len(p)-1, -1 ):
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
			else:														# no teams have been specified
				for j in range( len(mapFile) ):
					if mapFile[j].find( "EndGame" ) >= 0:
						ins = j + 1
						break
			for i in range( maxTeams-1, len(t)-1, -1 ):
				sCnt = "%i" % (i)
				mapFile = mapFile[:ins] + addTeam + mapFile[ins:]
				mapFile[ ins+1 ] = mapFile[ ins+1 ].replace("#", sCnt)
				mapFile[ ins+2 ] = mapFile[ ins+2 ].replace("#", sCnt)

		# adjust Barbarians; maximum 5 factions
		for n in range( 5 ):
			ownerOld = "Owner=%2i" % ( len(p) + n )
			ownerNew = "Owner=%2i" % ( maxPlayers + n )
			for i in range( len(mapFile) ):
				mapFile[i] = mapFile[i].replace( ownerOld, ownerNew )

		# adjust Description
		sMax = "%2i" % (maxPlayers)
		iDescription = -1
		iEndGame = -1
		for i in range( len(mapFile) ):
			if mapFile[i].find( "Description=" ) > -1:
				iDescription = i
				break
		for i in range( len(mapFile) ):
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
			CvUtil.pyPrint( "Error: Unable to save new scenario-file: %r" % ( filePath+ext, ) )
			CvUtil.pyPrint( "-----> Printing scenario-file to log" )
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
########## Temudjin END

def writeDesc(argsList):
	"Save out a high-level desc of the world, for WorldBuilder"
	fileName = argsList[0]
	global lastFileRead
	lastFileRead=None
	return WBDesc.write(fileName)

def readAndApplyDesc(argsList):
	"Read in and apply a high-level desc of the world.  In-game load only"
	fileName = argsList[0]

########## Temudjin START
	print "--- readAndApplyDesc(argsList) ---"
	if not adjustMap( fileName ): return -1
########## Temudjin END

	if WBDesc.read(fileName) < 0:
		return -1
	if (WBDesc.applyMap() < 0):
		return -1
	return WBDesc.applyInitialItems()

def readDesc(argsList):
	"Read in a high-level desc of the world, for WorldBuilder.  Must call applyMap and applyInitialItems to finish the process"
	global lastFileRead
	fileName = argsList[0]

########## Temudjin START
	print "--- readDesc(argsList) ---"
	if not adjustMap( fileName ): return -1
########## Temudjin END

	if (fileName!=lastFileRead):
		ret=WBDesc.read(fileName)
		if (ret==0):
			lastFileRead=fileName
	else:
		ret=0
	return ret

def applyMapDesc():
	"After reading, applies the map loaded data"
	return WBDesc.applyMap()

def applyInitialItems():
	"After reading, applies player units, cities, and techs"
	return WBDesc.applyInitialItems()

def getAssignedStartingPlots():
	"Reads in starting plots for random players"
	return WBDesc.getAssignedStartingPlots()

def initWBEditor(argsList):
	"Called from the Worldbuilder app - sends to CvWBPopups for handling"
	return CvWBPopups.CvWBPopups().initWB(argsList)

def getGameData():
	"after reading a save file, return game/player data as a tuple"
	t=()
	gameTurn = WBDesc.gameDesc.gameTurn
	maxTurns = WBDesc.gameDesc.maxTurns
	maxCityElimination = WBDesc.gameDesc.maxCityElimination
	numAdvancedStartPoints = WBDesc.gameDesc.numAdvancedStartPoints
	targetScore = WBDesc.gameDesc.targetScore
	#print "### %r" % (gc.getWorldInfo)
	#print "### %r" % (gc.getNumWorldInfos())
	#print "### %r" % (WBDesc.mapDesc.worldSize)
	worldSizeType = CvUtil.findInfoTypeNum(gc.getWorldInfo, gc.getNumWorldInfos(), WBDesc.mapDesc.worldSize)
	climateType = CvUtil.findInfoTypeNum(gc.getClimateInfo, gc.getNumClimateInfos(), WBDesc.mapDesc.climate)
	seaLevelType = CvUtil.findInfoTypeNum(gc.getSeaLevelInfo, gc.getNumSeaLevelInfos(), WBDesc.mapDesc.seaLevel)
	eraType = CvUtil.findInfoTypeNum(gc.getEraInfo, gc.getNumEraInfos(), WBDesc.gameDesc.eraType)
	gameSpeedType = CvUtil.findInfoTypeNum(gc.getGameSpeedInfo, gc.getNumGameSpeedInfos(), WBDesc.gameDesc.speedType)
	calendarType = CvUtil.findInfoTypeNum(gc.getCalendarInfo, gc.getNumCalendarInfos(), WBDesc.gameDesc.calendarType)

	t=t+(worldSizeType,)
	t=t+(climateType,)
	t=t+(seaLevelType,)
	t=t+(eraType,)
	t=t+(gameSpeedType,)
	t=t+(calendarType,)

	t=t+(len(WBDesc.gameDesc.options),)
	for i in range(len(WBDesc.gameDesc.options)):
		option = CvUtil.findInfoTypeNum(gc.getGameOptionInfo, gc.getNumGameOptionInfos(), WBDesc.gameDesc.options[i])
		t=t+(option,)

	t=t+(len(WBDesc.gameDesc.mpOptions),)
	for i in range(len(WBDesc.gameDesc.mpOptions)):
		mpOption = CvUtil.findInfoTypeNum(gc.getMPOptionInfo, gc.getNumMPOptionInfos(), WBDesc.gameDesc.mpOptions[i])
		t=t+(mpOption,)

	t=t+(len(WBDesc.gameDesc.forceControls),)
	for i in range(len(WBDesc.gameDesc.forceControls)):
		forceControl = CvUtil.findInfoTypeNum(gc.getForceControlInfo, gc.getNumForceControlInfos(), WBDesc.gameDesc.forceControls[i])
		t=t+(forceControl,)

	t=t+(len(WBDesc.gameDesc.victories),)
	for i in range(len(WBDesc.gameDesc.victories)):
		victory = CvUtil.findInfoTypeNum(gc.getVictoryInfo, gc.getNumVictoryInfos(), WBDesc.gameDesc.victories[i])
		t=t+(victory,)

	t=t+(gameTurn,)
	t=t+(maxTurns,)
	t=t+(maxCityElimination,)
	t=t+(numAdvancedStartPoints,)
	t=t+(targetScore,)

	return t

def getModPath():
	"Returns the path for the Mod that this scenario should load (if applicable)"
	return (WBDesc.gameDesc.szModPath)

def getMapDescriptionKey():
	"Returns the TXT_KEY Description of the map to be displayed in the map/mod selection screen"
	return (WBDesc.gameDesc.szDescription)

def isRandomMap():
	"If True, this is really a mod, not a scenario"
	return (WBDesc.gameDesc.iRandom)

def getPlayerData():
	"after reading a save file, return player data as a tuple, terminated by -1"
	t=()
	for i in range(gc.getMAX_CIV_PLAYERS()):
		leaderType = CvUtil.findInfoTypeNum(gc.getLeaderHeadInfo, gc.getNumLeaderHeadInfos(), WBDesc.playersDesc[i].leaderType)
		civType = CvUtil.findInfoTypeNum(gc.getCivilizationInfo, gc.getNumCivilizationInfos(), WBDesc.playersDesc[i].civType)
		handicapType = CvUtil.findInfoTypeNum(gc.getHandicapInfo, gc.getNumHandicapInfos(), WBDesc.playersDesc[i].handicap)
		color = CvUtil.findInfoTypeNum(gc.getPlayerColorInfo, gc.getNumPlayerColorInfos(), WBDesc.playersDesc[i].color)
		artStyle = gc.getTypesEnum(WBDesc.playersDesc[i].artStyle)

		t=t+(civType,)
		t=t+(WBDesc.playersDesc[i].isPlayableCiv,)
		t=t+(leaderType,)
		t=t+(handicapType,)
		t=t+(WBDesc.playersDesc[i].team,)
		t=t+(color,)
		t=t+(artStyle,)
		t=t+(WBDesc.playersDesc[i].isMinorNationCiv,)
		t=t+(WBDesc.playersDesc[i].isWhiteFlag,)

	return t

def getPlayerDesc():
	"after reading a save file, return player description data (wide strings) as a tuple"
	t=()
	for i in range(gc.getMAX_CIV_PLAYERS()):
		t=t+(WBDesc.playersDesc[i].szCivDesc,)
		t=t+(WBDesc.playersDesc[i].szCivShortDesc,)
		t=t+(WBDesc.playersDesc[i].szLeaderName,)
		t=t+(WBDesc.playersDesc[i].szCivAdjective,)
		t=t+(WBDesc.playersDesc[i].szFlagDecal,)

	return t
