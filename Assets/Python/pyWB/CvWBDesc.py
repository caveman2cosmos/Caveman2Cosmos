from CvPythonExtensions import *
import os
import CvUtil
from array import *

GC = CyGlobalContext()
version = 11

#############
# parser functions for WB desc
class CvWBParser:
	# return a list of (comma separated) tokens from the line.  Strip whitespace on each token
	def getTokens(self, line):
		if line == None:
			return []
		toks=line.split(",")
		toksOut = []
		for tok in toks:
			toksOut.append(tok.strip())
		return toksOut

	# return true if item exists in list of tokens
	def findToken(self, toks, item):
		for tok in toks:
			if (tok==item):
				return True
		return False

	# Search for a token of the form item=value in the list of toks, and return value, or -1 if not found
	def findTokenValue(self, toks, item):
		for tok in toks:
			l=tok.split("=")
			if (item==l[0]):
				if (len(l)==1):
					return item
				return l[1]
		return -1		# failed

	# return the next line from the list of lines
	def getNextLine(self, f):
		return f.readline()

	# Find the next line that contains the token item, return false if not found
	def findNextToken(self, f, item):
		while True:
			line = self.getNextLine(f)
			if (not line):
				return False	# EOF
			toks=self.getTokens(line)
			if (self.findToken(toks, item)):
				return True
		return False

	# Find the next line that contains item=value, return value or -1 if not found
	def findNextTokenValue(self, f, item):
		while True:
			line = self.getNextLine(f)
			if not line:
				return -1		# EOF
			toks=self.getTokens(line)
			val=self.findTokenValue(toks, item)
			if val != -1:
				return val
		return -1

#############
# class for serializing game data
class CvGameDesc:

	def __init__(self):
		self.eraType = "NONE"
		self.speedType = "NONE"
		self.calendarType = "CALENDAR_DEFAULT"
		self.options = ()
		self.mpOptions = ()
		self.forceControls = ()
		self.victories = ()
		self.gameTurn = 0
		self.maxTurns = 0
		self.maxCityElimination = 0
		self.numAdvancedStartPoints = 0
		self.targetScore = 0
		self.iStartYear = -4000
		self.szDescription = ""
		self.szModPath = ""
		self.iRandom = 0

	# after reading, apply the game data
	def apply(self):
		GC.getGame().setStartYear(self.iStartYear)

	# write out game data
	def write(self, f):
		GAME = GC.getGame()
		f.write("BeginGame\n\tEra=%s\n\tSpeed=%s\n\tCalendar=%s\n" %(GC.getEraInfo(GAME.getStartEra()).getType(), GC.getGameSpeedInfo(GAME.getGameSpeedType()).getType(), GC.getCalendarInfo(GAME.getCalendar()).getType()))

		# write options
		for i in xrange(GC.getNumGameOptionInfos()):
			if GAME.isOption(i):
				f.write("\tOption=%s\n" %(GC.getGameOptionInfo(i).getType()))

		# write mp options
		for i in xrange(GC.getNumMPOptionInfos()):
			if GAME.isMPOption(i):
				f.write("\tMPOption=%s\n" %(GC.getMPOptionInfo(i).getType()))

		# write force controls
		for i in xrange(GC.getNumForceControlInfos()):
			if GAME.isForcedControl(i):
				f.write("\tForceControl=%s\n" %(GC.getForceControlInfo(i).getType()))

		# write victories
		for i in xrange(GC.getNumVictoryInfos()):
			if GAME.isVictoryValid(i):
				if not GC.getVictoryInfo(i).isPermanent():
					f.write("\tVictory=%s\n" %(GC.getVictoryInfo(i).getType()))

		f.write("\tGameTurn=%d\n" % GAME.getGameTurn())
		f.write("\tMaxTurns=%d\n" % GAME.getMaxTurns())
		f.write("\tMaxCityElimination=%d\n" % GAME.getMaxCityElimination())
		f.write("\tNumAdvancedStartPoints=%d\n" % GAME.getNumAdvancedStartPoints())
		f.write("\tTargetScore=%d\n" % GAME.getTargetScore())

		f.write("\tStartYear=%d\n" % GAME.getStartYear())
		f.write("\tDescription=%s\n" % self.szDescription)
		f.write("\tModPath=%s\n" % self.szModPath)
		f.write("EndGame\n")

	def read(self, f):
		"read in game data"
		self.__init__()

		parser = CvWBParser()
		if parser.findNextTokenValue(f, "BeginGame") != -1:
			while True:
				nextLine = parser.getNextLine(f)
				toks = parser.getTokens(nextLine)
				if not toks:
					break

				v = parser.findTokenValue(toks, "Era")
				if v != -1:
					self.eraType = v
					continue

				v = parser.findTokenValue(toks, "Speed")
				if v != -1:
					self.speedType = v
					continue

				v = parser.findTokenValue(toks, "Calendar")
				if v != -1:
					self.calendarType = v
					continue

				v = parser.findTokenValue(toks, "Option")
				if v != -1:
					self.options = self.options + (v,)
					continue

				v = parser.findTokenValue(toks, "MPOption")
				if v != -1:
					self.mpOptions = self.mpOptions + (v,)
					continue

				v = parser.findTokenValue(toks, "ForceControl")
				if v != -1:
					self.forceControls = self.forceControls + (v,)
					continue

				v = parser.findTokenValue(toks, "Victory")
				if v != -1:
					self.victories = self.victories + (v,)
					continue

				v = parser.findTokenValue(toks, "GameTurn")
				if v != -1:
					self.gameTurn = int(v)
					continue

				v = parser.findTokenValue(toks, "MaxTurns")
				if v != -1:
					self.maxTurns = int(v)
					continue

				v = parser.findTokenValue(toks, "MaxCityElimination")
				if v != -1:
					self.maxCityElimination = int(v)
					continue

				v = parser.findTokenValue(toks, "NumAdvancedStartPoints")
				if v != -1:
					self.numAdvancedStartPoints = int(v)
					continue

				v = parser.findTokenValue(toks, "TargetScore")
				if v != -1:
					self.targetScore = int(v)
					continue

				v = parser.findTokenValue(toks, "StartYear")
				if v != -1:
					self.iStartYear = int(v)
					continue

				v = parser.findTokenValue(toks, "Description")
				if v != -1:
					self.szDescription = v
					continue

				v = parser.findTokenValue(toks, "ModPath")
				if v != -1:
					self.szModPath = v
					continue

				v = parser.findTokenValue(toks, "Random")
				if v != -1:
					self.iRandom = int(v)
					continue

				if parser.findTokenValue(toks, "EndGame") != -1:
					break

#############
class CvTeamDesc:
	def __init__(self):
		self.techTypes = ()
		self.aaiEspionageAgainstTeams = []
		self.bContactWithTeamList = ()
		self.bWarWithTeamList = ()
		self.bPermanentWarPeaceList = ()
		self.bOpenBordersWithTeamList = ()
		self.bDefensivePactWithTeamList = ()
		self.bVassalOfTeamList = ()
		self.projectType = []
		self.bRevealMap = 0
		self.iMasterPower = 0
		self.iVassalPower = 0
		self.iEspionageEver = 0

	def write(self, f, idx):
		"write out team data"
		f.write("BeginTeam\n")

		# Team ID (to make things easier to mess with in the text)
		f.write("\tTeamID=%d\n" %(idx))

		# write techs
		for i in xrange(GC.getNumTechInfos()):
			if (GC.getTeam(idx).isHasTech(i)):
				f.write("\tTech=%s\n" %(GC.getTechInfo(i).getType()))
			if GC.getTechInfo(i).isRepeat():
				for j in xrange(GC.getTeam(idx).getTechCount(i)):
					f.write("\tTech=%s\n" %(GC.getTechInfo(i).getType()))

		# write Espionage against other teams
		for i in xrange(GC.getMAX_PC_TEAMS()):#
			if (GC.getTeam(idx).getEspionagePointsAgainstTeam(i) > 0):
				f.write("\tEspionageTeam=%d, EspionageAmount=%d\n" %(i, GC.getTeam(idx).getEspionagePointsAgainstTeam(i)))

		# write Espionage Ever against other teams
		if (GC.getTeam(idx).getEspionagePointsEver() > 0):
			f.write("\tEspionageEverAmount=%d\n" %(GC.getTeam(idx).getEspionagePointsEver()))

		# write met other teams
		for i in xrange(GC.getMAX_PC_TEAMS()):#
			if (GC.getTeam(idx).isHasMet(i)):
				f.write("\tContactWithTeam=%d\n" %(i))

		# write warring teams
		for i in xrange(GC.getMAX_PC_TEAMS()):#
			if (GC.getTeam(idx).isAtWar(i)):
				f.write("\tAtWar=%d\n" %(i))

		# write permanent war/peace teams
		for i in xrange(GC.getMAX_PC_TEAMS()):#
			if (GC.getTeam(idx).isPermanentWarPeace(i)):
				f.write("\tPermanentWarPeace=%d\n" %(i))

		# write open borders other teams
		for i in xrange(GC.getMAX_PC_TEAMS()):#
			if (GC.getTeam(idx).isOpenBorders(i)):
				f.write("\tOpenBordersWithTeam=%d\n" %(i))

		# write defensive pact other teams
		for i in xrange(GC.getMAX_PC_TEAMS()):#
			if (GC.getTeam(idx).isDefensivePact(i)):
				f.write("\tDefensivePactWithTeam=%d\n" %(i))

		# write vassal state
		for i in xrange(GC.getMAX_PC_TEAMS()):#
			if (GC.getTeam(idx).isVassal(i)):
				f.write("\tVassalOfTeam=%d\n" %(i))

		for i in xrange(GC.getNumProjectInfos()):
			for j in xrange(GC.getTeam(idx).getProjectCount(i)):
				f.write("\tProjectType=%s\n" %(GC.getProjectInfo(i).getType()))

		f.write("\tRevealMap=%d\n" %(0))

		if GC.getTeam(idx).getVassalPower() != 0:
			f.write("\tVassalPower=%d\n" %(GC.getTeam(idx).getVassalPower()))
		if GC.getTeam(idx).getMasterPower() != 0:
			f.write("\tMasterPower=%d\n" %(GC.getTeam(idx).getMasterPower()))

		f.write("EndTeam\n")

	def read(self, f):
		"read in team data"
		self.__init__()

		parser = CvWBParser()
		if (parser.findNextTokenValue(f, "BeginTeam")!=-1):
			while True:
				nextLine = parser.getNextLine(f)
				toks = parser.getTokens(nextLine)
				if not toks:
					break

				v = parser.findTokenValue(toks, "Tech")
				if v != -1:
					self.techTypes = self.techTypes + (v,)
					continue

				v = parser.findTokenValue(toks, "EspionageTeam")
				if v != -1:
					iTeam = int(v)

					iExtra = int(parser.findTokenValue(toks, "EspionageAmount"))
					self.aaiEspionageAgainstTeams.append([iTeam,iExtra])
					continue

				v = parser.findTokenValue(toks, "EspionageEverAmount")
				if v != -1:
					self.iEspionageEver = int(v)
					continue

				v = parser.findTokenValue(toks, "ContactWithTeam")
				if v != -1:
					self.bContactWithTeamList = self.bContactWithTeamList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "AtWar")
				if v != -1:
					self.bWarWithTeamList = self.bWarWithTeamList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "PermanentWarPeace")
				if v != -1:
					self.bPermanentWarPeaceList = self.bPermanentWarPeaceList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "OpenBordersWithTeam")
				if v != -1:
					self.bOpenBordersWithTeamList = self.bOpenBordersWithTeamList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "DefensivePactWithTeam")
				if v != -1:
					self.bDefensivePactWithTeamList = self.bDefensivePactWithTeamList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "VassalOfTeam")
				if v != -1:
					self.bVassalOfTeamList = self.bVassalOfTeamList + (int(v),)
					continue

				v = parser.findTokenValue(toks, "ProjectType")
				if v != -1:
					self.projectType.append(v)
					continue

				v = parser.findTokenValue(toks, "RevealMap")
				if v != -1:
					self.bRevealMap = int(v)
					continue

				v = parser.findTokenValue(toks, "VassalPower")
				if v != -1:
					self.iVassalPower = int(v)
					continue

				v = parser.findTokenValue(toks, "MasterPower")
				if v != -1:
					self.iMasterPower = int(v)
					continue

				if parser.findTokenValue(toks, "EndTeam") != -1:
					return True		# completed successfully

		return False	# failed

#############
class CvPlayerDesc:
	def __init__(self):
		self.szCivDesc = ""
		self.szCivShortDesc = ""
		self.szLeaderName = ""
		self.szCivAdjective = ""
		self.szFlagDecal = ""
		self.isWhiteFlag = 0

		self.leaderType = "NONE"
		self.civType = "NONE"
		self.handicap = GC.getHandicapInfo(GC.getDefineINT("STANDARD_HANDICAP")).getType()
		self.team = -1		# team index
		self.color = "NONE"
		self.artStyle = "NONE"
		self.isPlayableCiv = 1
		self.isMinorNationCiv = 0
		self.iStartingGold = 0
		self.iStartingX = -1
		self.iStartingY = -1
		self.stateReligion = ""
		self.szStartingEra = ""
		self.bRandomStartLocation = "false"

		self.aaiCivics = []
		self.aaiAttitudeExtras = []
		self.aszCityList = []

	def write(self, f, idx):
		"write out player data"
		f.write("BeginPlayer\n")

		# write team
		f.write("\tTeam=%d\n" %(int(GC.getPlayer(idx).getTeam())))

		# write leader and Civ Description info
		if (GC.getPlayer(idx).getLeaderType() == LeaderHeadTypes.NO_LEADER):
			f.write("\tLeaderType=NONE\n")

		else:
			f.write("\tLeaderType=%s\n" %(GC.getLeaderHeadInfo(GC.getPlayer(idx).getLeaderType()).getType()))

		# write civ, color, artStyle, isPlayableCiv, isMinorNation, StartingGold and StateReligion
		if (GC.getPlayer(idx).getCivilizationType() == CivilizationTypes.NO_CIVILIZATION):
			f.write("\tCivType=NONE\n")
			f.write("\tColor=NONE\n")
			f.write("\tArtStyle=NONE\n")
		else:
			f.write("\tLeaderName=%s\n" %(GC.getPlayer(idx).getNameKey().encode("latin_1")))
			f.write("\tCivDesc=%s\n" %(GC.getPlayer(idx).getCivilizationDescriptionKey().encode("latin_1")))
			f.write("\tCivShortDesc=%s\n" %(GC.getPlayer(idx).getCivilizationShortDescriptionKey().encode("latin_1")))
			f.write("\tCivAdjective=%s\n" %(GC.getPlayer(idx).getCivilizationAdjectiveKey().encode("latin_1")))
			f.write("\tFlagDecal=%s\n" %(GC.getPlayer(idx).getFlagDecal().encode("latin_1")))
			f.write("\tWhiteFlag=%d\n" %(GC.getPlayer(idx).isWhiteFlag(),))
			f.write("\tCivType=%s\n" %(GC.getCivilizationInfo(GC.getPlayer(idx).getCivilizationType()).getType()))
			f.write("\tColor=%s\n" %(GC.getPlayerColorInfo(GC.getPlayer(idx).getPlayerColor()).getType()))
			f.write("\tArtStyle=%s\n" %(GC.getArtStyleTypes(GC.getPlayer(idx).getArtStyleType())))
			f.write("\tPlayableCiv=%d\n" %(int(GC.getPlayer(idx).isPlayable())))
			f.write("\tMinorNationStatus=%d\n" %(GC.getPlayer(idx).isMinorCiv()))
			f.write("\tStartingGold=%d\n" %(GC.getPlayer(idx).getGold()))

			if GC.getPlayer(idx).isAlive():
				pPlot = GC.getPlayer(idx).getStartingPlot()
				if (not pPlot.isNone()):
					f.write("\tStartingX=%d, StartingY=%d\n" %(pPlot.getX(), pPlot.getY()))

			pPlayerReligionInfo = GC.getReligionInfo(GC.getPlayer(idx).getStateReligion())
			if pPlayerReligionInfo:
				f.write("\tStateReligion=%s\n" %(pPlayerReligionInfo.getType()))
			else:
				f.write("\tStateReligion=\n")

			f.write("\tStartingEra=%s\n" %(GC.getEraInfo(GC.getPlayer(idx).getCurrentEra()).getType()))

			f.write("\tRandomStartLocation=false\n")

			# write Civics
			for iCivicOptionLoop in xrange(GC.getNumCivicOptionInfos()):
				for iCivicLoop in xrange(GC.getNumCivicInfos()):
					iCivic = GC.getPlayer(idx).getCivics(iCivicOptionLoop)
					if (iCivicLoop == iCivic):
						f.write("\tCivicOption=%s, Civic=%s\n" %(GC.getCivicOptionInfo(iCivicOptionLoop).getType(), GC.getCivicInfo(iCivicLoop).getType()))

			# write Attitude Extra
			for i in xrange(GC.getMAX_PC_PLAYERS()):#
				if (GC.getPlayer(idx).AI_getAttitudeExtra(i) != 0):
					f.write("\tAttitudePlayer=%d, AttitudeExtra=%d\n" %(i, GC.getPlayer(idx).AI_getAttitudeExtra(i)))

			# write City List
			for i in xrange(GC.getPlayer(idx).getNumCityNames()):
				f.write("\tCityList=%s\n" %(GC.getPlayer(idx).getCityName(i)))

		if (GC.getPlayer(idx).getHandicapType() == HandicapTypes.NO_HANDICAP):
			f.write("\tHandicap=%s\n" %(GC.getHandicapInfo(GC.getDefineINT("STANDARD_HANDICAP")).getType()))
		else:
			f.write("\tHandicap=%s\n" %(GC.getHandicapInfo(GC.getPlayer(idx).getHandicapType()).getType()))

		f.write("EndPlayer\n")

	def read(self, f):
		"read in player data"
		self.__init__()
		parser = CvWBParser()
		if parser.findNextTokenValue(f, "BeginPlayer") != -1:
			while True:
				nextLine = parser.getNextLine(f)
				toks = parser.getTokens(nextLine)
				if not toks:
					break

				v = parser.findTokenValue(toks, "CivDesc")
				if v != -1:
					self.szCivDesc = v.decode("latin_1")
					continue

				v = parser.findTokenValue(toks, "CivShortDesc")
				if v != -1:
					self.szCivShortDesc = v.decode("latin_1")
					continue

				v = parser.findTokenValue(toks, "LeaderName")
				if v != -1:
					self.szLeaderName = v.decode("latin_1")
					continue

				v = parser.findTokenValue(toks, "CivAdjective")
				if v != -1:
					self.szCivAdjective = v.decode("latin_1")
					continue

				v = parser.findTokenValue(toks, "FlagDecal")
				if v != -1:
					self.szFlagDecal = v.decode("latin_1")
					continue

				v = parser.findTokenValue(toks, "WhiteFlag")
				if v != -1:
					self.isWhiteFlag = int(v)
					continue

				v = parser.findTokenValue(toks, "LeaderType")
				if v != -1:
					self.leaderType = v
					continue

				v = parser.findTokenValue(toks, "CivType")
				if v != -1:
					self.civType = v
					continue

				v = parser.findTokenValue(toks, "Team")
				if v != -1:
					self.team = int(v)
					continue

				v = parser.findTokenValue(toks, "Handicap")
				if v != -1:
					self.handicap = v
					continue

				v = parser.findTokenValue(toks, "Color")
				if v != -1:
					self.color = v
					continue

				v = parser.findTokenValue(toks, "ArtStyle")
				if v != -1:
					self.artStyle = v
					continue

				v = parser.findTokenValue(toks, "PlayableCiv")
				if v != -1:
					self.isPlayableCiv = int(v)
					continue

				v = parser.findTokenValue(toks, "MinorNationStatus")
				if v != -1:
					self.isMinorNationCiv = int(v)
					continue

				v = parser.findTokenValue(toks, "StartingGold")
				if v != -1:
					self.iStartingGold = int(v)
					continue

				vX = parser.findTokenValue(toks, "StartingX")
				vY = parser.findTokenValue(toks, "StartingY")
				if vX!=-1 and vY!=-1:
					self.iStartingX = int(vX)
					self.iStartingY = int(vY)
					continue

				v = parser.findTokenValue(toks, "StateReligion")
				if v != -1:
					self.stateReligion = v
					continue

				v = parser.findTokenValue(toks, "StartingEra")
				if v != -1:
					self.szStartingEra = v
					continue

				v = parser.findTokenValue(toks, "RandomStartLocation")
				if v != -1:
					self.bRandomStartLocation = v
					continue

				v = parser.findTokenValue(toks, "CivicOption")
				if v != -1:
					iCivicOptionType = GC.getInfoTypeForString(v)

					v = parser.findTokenValue(toks, "Civic")
					if v != -1:
						iCivicType = GC.getInfoTypeForString(v)
						self.aaiCivics.append([iCivicOptionType,iCivicType])
						continue

				v = parser.findTokenValue(toks, "AttitudePlayer")
				if v != -1:
					iPlayer = int(v)

					iExtra = int(parser.findTokenValue(toks, "AttitudeExtra"))
					self.aaiAttitudeExtras.append([iPlayer,iExtra])
					continue

				v = parser.findTokenValue(toks, "CityList")
				if v != -1:
					self.aszCityList.append(v)
					continue

				if parser.findTokenValue(toks, "EndPlayer") != -1:
					#print("Civics:")
					#print self.aaiCivics
					#print("Attitudes:")
					#print self.aaiAttitudeExtras
					break

#############
class CvUnitDesc:
	"unit WB serialization"
	def __init__(self):
		self.plotX = -1
		self.plotY = -1
		self.unitType = None
		self.szName = None
		self.leaderUnitType = None
		self.owner =-1
		self.damage = 0
		self.level = -1
		self.experience = -1
		self.promotionType = []
		self.facingDirection = DirectionTypes.NO_DIRECTION;
		self.isSleep = False
		self.isIntercept = False
		self.isPatrol = False
		self.isPlunder = False
		self.szUnitAIType = "NO_UNITAI"
		self.szScriptData = "NONE"

	def apply(self):
		"after reading, this will actually apply the data"
		CyPlayer = GC.getPlayer(self.owner)
		if CyPlayer.isAlive():
			CvUtil.pyAssert(self.plotX >= 0 and self.plotY >= 0, "invalid plot coords")
			unitTypeNum = GC.getInfoTypeForString(self.unitType)
			CvUtil.pyAssert(unitTypeNum > -1, "Can't find type enum for type tag " + self.unitType)

			if self.szUnitAIType != "NO_UNITAI":
				eUnitAI = GC.getInfoTypeForString(self.szUnitAIType)
			else:
				eUnitAI = -1

			CyUnit = CyPlayer.initUnit(unitTypeNum, self.plotX, self.plotY, UnitAITypes(eUnitAI), self.facingDirection)
			if not CyUnit: return

			if self.szName != None:
				CyUnit.setName(self.szName)
			#leader unit type
			if self.leaderUnitType != None:
				leaderUnitTypeNum = GC.getInfoTypeForString(self.leaderUnitType)
				if leaderUnitTypeNum > -1:
					CyUnit.setLeaderUnitType(leaderUnitTypeNum)

			#other properties
			if self.damage != 0:
				CyUnit.setDamage(self.damage, PlayerTypes.NO_PLAYER)
			if self.level != -1:
				CyUnit.setLevel(self.level)
			if self.experience != -1:
				CyUnit.setExperience(self.experience, -1)
			for promo in self.promotionType:
				iPromo = GC.getInfoTypeForString(promo)
				if iPromo > -1:
					CyUnit.setHasPromotion(iPromo, True)
			if self.isSleep:
				CyUnit.getGroup().setActivityType(ActivityTypes.ACTIVITY_SLEEP)
			elif self.isIntercept:
				CyUnit.getGroup().setActivityType(ActivityTypes.ACTIVITY_INTERCEPT)
			elif self.isPatrol:
				CyUnit.getGroup().setActivityType(ActivityTypes.ACTIVITY_PATROL)
			elif self.isPlunder:
				CyUnit.getGroup().setActivityType(ActivityTypes.ACTIVITY_PLUNDER)
			if self.szScriptData != "NONE":
				CyUnit.setScriptData(self.szScriptData)

	def read(self, f, pX, pY):
		"read in unit data - at this point the first line 'BeginUnit' has already been read"
		self.__init__()
		self.plotX = pX
		self.plotY = pY
		CvUtil.pyAssert(self.plotX >= 0 and self.plotY >= 0, "invalid plot coords")

		parser = CvWBParser()
		while True:
			nextLine = parser.getNextLine(f)
			toks = parser.getTokens(nextLine)
			if not toks:
				break

			v = parser.findTokenValue(toks, "UnitType")
			vOwner = parser.findTokenValue(toks, "UnitOwner")
			if v != -1 and vOwner != -1:
				self.unitType = v
				self.owner = int(vOwner)
				continue

			v = parser.findTokenValue(toks, "UnitName")
			if v != -1:
				self.szName = v.decode("latin_1")
				continue

			v = parser.findTokenValue(toks, "LeaderUnitType")
			if v != -1:
				self.leaderUnitType = v
				continue

			v = parser.findTokenValue(toks, "Damage")
			if v != -1:
				self.damage = (int(v))
				continue

			v = parser.findTokenValue(toks, "Level")
			if v != -1:
				self.level = (int(v))
				self.experience = int(parser.findTokenValue(toks, "Experience"))
				continue

			# Units - Promotions
			v = parser.findTokenValue(toks, "PromotionType")
			if v != -1:
				self.promotionType.append(v)
				continue

			v = parser.findTokenValue(toks, "FacingDirection")
			if v != -1:
				self.facingDirection = (DirectionTypes(v))
				continue

			if parser.findTokenValue(toks, "Sleep") != -1:
				self.isSleep = True
				continue

			if parser.findTokenValue(toks, "Intercept") != -1:
				self.isIntercept = True
				continue

			if parser.findTokenValue(toks, "Patrol") != -1:
				self.isPatrol = True
				continue

			if parser.findTokenValue(toks, "Plunder") != -1:
				self.isPlunder = True
				continue

			v = parser.findTokenValue(toks, "UnitAIType")
			if v != -1:
				self.szUnitAIType = v
				continue

			v = parser.findTokenValue(toks, "ScriptData")
			if v != -1:
				print "found script data: " + v
				self.szScriptData = v
				continue

			if parser.findTokenValue(toks, "EndUnit") != -1:
				break

	# save unit desc to a file
	def write(self, f, CyUnit, plot):
		unitType = GC.getUnitInfo(CyUnit.getUnitType()).getType()
		f.write("\tBeginUnit\n\t\tUnitType=%s, UnitOwner=%d" %(unitType, CyUnit.getOwner()))
		temp = CyUnit.getNameNoDesc()
		if temp:
			f.write("\n\t\tUnitName=" + CvUtil.convertToStr(temp))
		temp = CyUnit.getLeaderUnitType()
		if temp != -1:
			f.write("\n\t\tLeaderUnitType=" + GC.getUnitInfo(temp).getType())
		f.write("\n\t\tDamage=%d\n\t\tLevel=%d, Experience=%d" %(CyUnit.getDamage(), CyUnit.getLevel(), CyUnit.getExperience()))
		for i in xrange(GC.getNumPromotionInfos()):
			if CyUnit.isHasPromotion(i):
				f.write("\n\t\tPromotionType=" + GC.getPromotionInfo(i).getType())

		f.write("\n\t\tFacingDirection=%d" % CyUnit.getFacingDirection())
		temp = CyUnit.getGroup().getActivityType()
		if temp == ActivityTypes.ACTIVITY_SLEEP:
			f.write("\n\t\tSleep")
		elif temp == ActivityTypes.ACTIVITY_INTERCEPT:
			f.write("\n\t\tIntercept")
		elif temp == ActivityTypes.ACTIVITY_PATROL:
			f.write("\n\t\tPatrol")
		elif temp == ActivityTypes.ACTIVITY_PLUNDER:
			f.write("\n\t\tPlunder")
		f.write("\n\t\tUnitAIType=" + GC.getUnitAIInfo(CyUnit.getUnitAIType()).getType())
		temp = CyUnit.getScriptData()
		if temp:
			f.write("\n\t\tScriptData=" + temp)
		f.write("\n\tEndUnit\n")

############
# serializes city data
class CvCityDesc:
	def __init__(self):
		self.city = None
		self.owner = None
		self.name = None
		self.population = 0
		self.productionUnit = "NONE"
		self.productionBuilding = "NONE"
		self.productionProject = "NONE"
		self.productionProcess = "NONE"
		self.culture = 0
		self.bldgType = []
		self.religions = []
		self.holyCityReligions = []
		self.corporations = []
		self.headquarterCorporations = []
		self.freeSpecialists = []
		self.plotX=-1
		self.plotY=-1
		self.szScriptData = "NONE"
		self.aiPlayerCulture = []
		for iPlayerLoop in xrange(GC.getMAX_PC_PLAYERS()):#
			self.aiPlayerCulture.append(0)

	# after reading, this will actually apply the data
	def apply(self):
		self.city = GC.getPlayer(self.owner).initCity(self.plotX, self.plotY)

		if self.name != None:
			self.city.setName(self.name, False)

		if self.population:
			self.city.setPopulation(self.population)

		if self.culture:
			self.city.setCulture(self.city.getOwner(), self.culture, True)

		for key in self.bldgType:
			iBuilding = GC.getInfoTypeForString(key)
			if iBuilding > -1:
				self.city.setNumRealBuilding(iBuilding, 1)

		for key in self.religions:
			iReligion = GC.getInfoTypeForString(key)
			if iReligion > -1:
				self.city.setHasReligion(iReligion, True, False, True)

		GAME = GC.getGame()
		for key in self.holyCityReligions:
			iReligion = GC.getInfoTypeForString(key)
			if iReligion > -1:
				GAME.setHolyCity(iReligion, self.city, False)

		for key in self.corporations:
			iCorporation = GC.getInfoTypeForString(key)
			if iCorporation > -1:
				self.city.setHasCorporation(iCorporation, True, False, True)

		for key in self.headquarterCorporations:
			iCorporation = GC.getInfoTypeForString(key)
			if iCorporation > -1:
				GAME.setHeadquarters(iCorporation, self.city, False)

		for key in self.freeSpecialists:
			iSpecialist = GC.getInfoTypeForString(key)
			if iSpecialist > -1:
				self.city.changeFreeSpecialistCount(iSpecialist, 1)

		for iPlayerLoop in xrange(GC.getMAX_PC_PLAYERS()):#
			iPlayerCulture = self.aiPlayerCulture[iPlayerLoop]
			if iPlayerCulture > 0:
				self.city.setCulture(iPlayerLoop, iPlayerCulture, True)

		iProd = -1
		if self.productionUnit != "NONE":
			iProd = GC.getInfoTypeForString(self.productionUnit)
			if iProd > -1:
				self.city.pushOrder(OrderTypes.ORDER_TRAIN, iProd, -1, False, False, False, True)

		if iProd == -1 and self.productionBuilding != "NONE":
			iProd = GC.getInfoTypeForString(self.productionBuilding)
			if iProd > -1:
				self.city.pushOrder(OrderTypes.ORDER_CONSTRUCT, iProd, -1, False, False, False, True)

		if iProd == -1 and self.productionProject != "NONE":
			iProd = GC.getInfoTypeForString(self.productionProject)
			if iProd > -1:
				self.city.pushOrder(OrderTypes.ORDER_CREATE, iProd, -1, False, False, False, True)

		if iProd == -1 and self.productionProcess != "NONE":
			iProd = GC.getInfoTypeForString(self.productionProcess)
			if iProd > -1:
				self.city.pushOrder(OrderTypes.ORDER_MAINTAIN, iProd, -1, False, False, False, True)

		if self.szScriptData != "NONE":
			self.city.setScriptData(self.szScriptData)

	# write out city data
	def write(self, f, plot):
		city = plot.getPlotCity()
		CvUtil.pyAssert(not city.isNone(), "null city?")
		f.write("\tBeginCity\n\t\tCityOwner=%d\n\t\tCityName=%s\n\t\tCityPopulation=%d\n" %(city.getOwner(), city.getNameKey().encode("latin_1"), city.getPopulation()))

		if city.isProductionUnit():
			f.write("\t\tProductionUnit=%s\n" %(GC.getUnitInfo(city.getProductionUnit()).getType(),))
		elif city.isProductionBuilding():
			f.write("\t\tProductionBuilding=%s\n" %(GC.getBuildingInfo(city.getProductionBuilding()).getType(),))
		elif city.isProductionProject():
			f.write("\t\tProductionProject=%s\n" %(GC.getProjectInfo(city.getProductionProject()).getType(),))
		elif city.isProductionProcess():
			f.write("\t\tProductionProcess=%s\n" %(GC.getProcessInfo(city.getProductionProcess()).getType(),))

		for iI in xrange(GC.getNumBuildingInfos()):
			if city.getNumRealBuilding(iI) > 0:
				f.write("\t\tBuildingType=%s\n" %(GC.getBuildingInfo(iI).getType()))

		for iI in xrange(GC.getNumReligionInfos()):
			if city.isHasReligion(iI):
				f.write("\t\tReligionType=%s\n" %(GC.getReligionInfo(iI).getType()))
			if city.isHolyCityByType(iI):
				f.write("\t\tHolyCityReligionType=%s\n" %(GC.getReligionInfo(iI).getType()))

		for iI in xrange(GC.getNumCorporationInfos()):
			if city.isHasCorporation(iI):
				f.write("\t\tCorporationType=%s\n" %(GC.getCorporationInfo(iI).getType()))
			if city.isHeadquartersByType(iI):
				f.write("\t\tHeadquarterCorporationType=%s\n" %(GC.getCorporationInfo(iI).getType()))

		for iI in xrange(GC.getNumSpecialistInfos()):
			for iJ in xrange(city.getAddedFreeSpecialistCount(iI)):
				f.write("\t\tFreeSpecialistType=%s\n" %(GC.getSpecialistInfo(iI).getType()))

		if city.getScriptData():
			f.write("\t\tScriptData=%s\n" %city.getScriptData())

		# Player culture
		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):#
			iPlayerCulture = city.getCulture(iPlayerX)
			if (iPlayerCulture > 0):
				f.write("\t\tPlayer%dCulture=%d\n" %(iPlayerX, iPlayerCulture))

		f.write("\tEndCity\n")

	# read in city data - at this point the first line 'BeginCity' has already been read
	def read(self, f, iX, iY):
		self.__init__()
		self.plotX=iX
		self.plotY=iY
		parser = CvWBParser()
		while True:
			nextLine = parser.getNextLine(f)
			toks = parser.getTokens(nextLine)
			if not toks:
				break

			# City - Owner
			vOwner=parser.findTokenValue(toks, "CityOwner")
			if vOwner != -1:
				self.owner = int(vOwner)
				continue

			# City - Name
			vName=parser.findTokenValue(toks, "CityName")
			if vName != -1:
				self.name = (vName).decode("latin_1")
				continue

			# City - Population
			v=parser.findTokenValue(toks, "CityPopulation")
			if v != -1:
				self.population = (int(v))
				continue

			# City - Production Unit
			v=parser.findTokenValue(toks, "ProductionUnit")
			if v != -1:
				self.productionUnit = v
				continue

			# City - Production Building
			v=parser.findTokenValue(toks, "ProductionBuilding")
			if v != -1:
				self.productionBuilding = v
				continue

			# City - Production Project
			v=parser.findTokenValue(toks, "ProductionProject")
			if v != -1:
				self.productionProject = v
				continue

			# City - Production Process
			v=parser.findTokenValue(toks, "ProductionProcess")
			if v != -1:
				self.productionProcess = v
				continue

			# City - Culture XXX obsolete...
			v=parser.findTokenValue(toks, "CityCulture")
			if v != -1:
				self.culture = int(v)
				continue

			# City - Buildings
			v=parser.findTokenValue(toks, "BuildingType")
			if v != -1:
				self.bldgType.append(v)
				continue

			# City - Religions
			v=parser.findTokenValue(toks, "ReligionType")
			if v != -1:
				self.religions.append(v)
				continue

			# City - HolyCity
			v=parser.findTokenValue(toks, "HolyCityReligionType")
			if v != -1:
				self.holyCityReligions.append(v)
				continue

			# City - Corporations
			v=parser.findTokenValue(toks, "CorporationType")
			if v != -1:
				self.corporations.append(v)
				continue

			# City - Headquarters
			v=parser.findTokenValue(toks, "HeadquarterCorporationType")
			if v != -1:
				self.headquarterCorporations.append(v)
				continue

			# City - Free Specialist
			v=parser.findTokenValue(toks, "FreeSpecialistType")
			if v != -1:
				self.freeSpecialists.append(v)
				continue

			# City - ScriptData
			v=parser.findTokenValue(toks, "ScriptData")
			if v != -1:
				self.szScriptData = v
				continue

			# Player Culture
			for iPlayerLoop in xrange(GC.getMAX_PC_PLAYERS()):#
				szCityTag = ("Player%dCulture" %(iPlayerLoop))
				v = parser.findTokenValue(toks, szCityTag)
				if v != -1:
					self.aiPlayerCulture[iPlayerLoop] = int(v)
					continue

			if parser.findTokenValue(toks, "EndCity") != -1:
				break

###########
# serializes plot data
class CvPlotDesc:
	def __init__(self):
		self.iX = -1
		self.iY = -1
		self.riverNSDirection = CardinalDirectionTypes.NO_CARDINALDIRECTION
		self.isNOfRiver = 0
		self.riverWEDirection = CardinalDirectionTypes.NO_CARDINALDIRECTION
		self.isWOfRiver = 0
		self.isStartingPlot = 0
		self.bonusType = None
		self.improvementType = None
		self.featureType = None
		self.featureVariety = 0
		self.routeType = None
		self.terrainType = None
		self.plotType = PlotTypes.NO_PLOT
		self.unitDescs = list()
		self.cityDesc = None
		self.szLandmark = ""
		self.szScriptData = "NONE"
		self.abTeamPlotRevealed = [0]*GC.getMAX_PC_TEAMS()#

	# apply plot and terrain type
	def preApply(self):
		plot = GC.getMap().plot(self.iX, self.iY)
		if self.plotType != PlotTypes.NO_PLOT:
			plot.setPlotType(self.plotType, False, False)
		if self.terrainType:
			iTerrain = GC.getInfoTypeForString(self.terrainType)
			if iTerrain > -1:
				plot.setTerrainType(iTerrain, False, False)

	# after reading, this will actually apply the data
	def apply(self):
		CyPlot = GC.getMap().plot(self.iX, self.iY)
		CyPlot.setNOfRiver(self.isNOfRiver, self.riverWEDirection)
		CyPlot.setWOfRiver(self.isWOfRiver, self.riverNSDirection)
		CyPlot.setStartingPlot(self.isStartingPlot)
		if self.bonusType:
			iBonus = GC.getInfoTypeForString(self.bonusType)
			if iBonus > -1:
				CyPlot.setBonusType(iBonus)
		if self.improvementType:
			iImprovement = GC.getInfoTypeForString(self.improvementType)
			if iImprovement > -1:
				CyPlot.setImprovementType(iImprovement)
		if self.featureType:
			iFeature = GC.getInfoTypeForString(self.featureType)
			if iFeature > -1:
				CyPlot.setFeatureType(iFeature, self.featureVariety)
		if self.routeType:
			iRoute = GC.getInfoTypeForString(self.routeType)
			if iRoute > -1:
				CyPlot.setRouteType(iRoute)

		if self.szLandmark:
			CyEngine().addLandmark(CyPlot, "%s" %(self.szLandmark))

		if self.szScriptData != "NONE":
			CyPlot.setScriptData(self.szScriptData)

	def applyUnits(self):
		#print "--apply units"
		for u in self.unitDescs:
			u.apply()

	def applyCity(self):
		if self.cityDesc:
			#print "--apply city"
			self.cityDesc.apply()

	def write(self, f, plot):
		"save plot desc to a file"
		f.write("BeginPlot\n")
		f.write("\tx=%d,y=%d\n" %(plot.getX(), plot.getY()))

		# scriptData
		temp = plot.getScriptData()
		if temp:
			f.write("\tScriptData=%s\n" % temp)
		# rivers
		if plot.getRiverNSDirection() != CardinalDirectionTypes.NO_CARDINALDIRECTION:
			f.write("\tRiverNSDirection=%d\n" % int(plot.getRiverNSDirection()))
		if plot.isNOfRiver():
			f.write("\tisNOfRiver\n")
		if plot.getRiverWEDirection() != CardinalDirectionTypes.NO_CARDINALDIRECTION:
			f.write("\tRiverWEDirection=%d\n" % int(plot.getRiverWEDirection()))
		if plot.isWOfRiver():
			f.write("\tisWOfRiver\n")
		# extras
		if plot.isStartingPlot():
			f.write("\tStartingPlot\n")
		if plot.getBonusType(-1) != -1:
			f.write("\tBonusType=%s\n" % GC.getBonusInfo(plot.getBonusType(-1)).getType())
		if plot.getImprovementType() != -1:
			f.write("\tImprovementType=%s\n" % GC.getImprovementInfo(plot.getImprovementType()).getType())
		if plot.getFeatureType() != -1:
			f.write("\tFeatureType=%s, FeatureVariety=%d\n" %(GC.getFeatureInfo(plot.getFeatureType()).getType(), plot.getFeatureVariety()))
		if plot.getRouteType() != -1:
			f.write("\tRouteType=%s\n" % GC.getRouteInfo(plot.getRouteType()).getType())
		if plot.getTerrainType() != -1:
			f.write("\tTerrainType=%s\n" % GC.getTerrainInfo(plot.getTerrainType()).getType())
		if plot.getPlotType() != PlotTypes.NO_PLOT:
			f.write("\tPlotType=%d\n" % int(plot.getPlotType()))

		# units
		for i in xrange(plot.getNumUnits()):
			unit = plot.getUnit(i)
			if unit.getUnitType() > -1:
				CvUnitDesc().write(f, unit, plot)
		# city
		if plot.isCity():
			CvCityDesc().write(f, plot)

		# Fog of War
		bFirstReveal = True
		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):#
			if GC.getTeam(iTeamX).isAlive():
				if plot.isRevealed(iTeamX, 0):
					# Plot is revealed for this Team so write out the fact that it is; if not revealed don't write anything
					if bFirstReveal:
						f.write("\tTeamReveal=")
						bFirstReveal = False
					f.write("%d," % iTeamX)
		if bFirstReveal == False:
			f.write("\n")	# terminate reveal line

		f.write("EndPlot\n")

	def read(self, f):
		"read in a plot desc"
		self.__init__()
		parser = CvWBParser()
		if parser.findNextToken(f, "BeginPlot") == False:
			return False	# no more plots
		while True:
			nextLine = parser.getNextLine(f)
			toks = parser.getTokens(nextLine)
			if not toks:
				break

			x = parser.findTokenValue(toks, "x")
			y = parser.findTokenValue(toks, "y")
			if x != -1 and y != -1:
				self.iX = int(x)
				self.iY = int(y)
				continue

			v = parser.findTokenValue(toks, "Landmark")
			if v != -1:
				self.szLandmark=v
				continue

			v = parser.findTokenValue(toks, "ScriptData")
			if v != -1:
				self.szScriptData=v
				continue

			v = parser.findTokenValue(toks, "RiverNSDirection")
			if v != -1:
				self.riverNSDirection = (CardinalDirectionTypes(v))
				continue

			if parser.findTokenValue(toks, "isNOfRiver") != -1:
				self.isNOfRiver = True
				continue

			v = parser.findTokenValue(toks, "RiverWEDirection")
			if v != -1:
				self.riverWEDirection = (CardinalDirectionTypes(v))
				continue

			if parser.findTokenValue(toks, "isWOfRiver") != -1:
				self.isWOfRiver = True
				continue

			if parser.findTokenValue(toks, "StartingPlot") != -1:
				self.isStartingPlot = True
				continue

			v = parser.findTokenValue(toks, "BonusType")
			if v != -1:
				self.bonusType = v
				continue

			v = parser.findTokenValue(toks, "ImprovementType")
			if v != -1:
				self.improvementType = v
				continue

			v = parser.findTokenValue(toks, "FeatureType")
			if v != -1:
				self.featureType = v
				v = parser.findTokenValue(toks, "FeatureVariety")
				if v != -1:
					self.featureVariety = int(v)
				continue

			v = parser.findTokenValue(toks, "RouteType")
			if v != -1:
				self.routeType = v
				continue

			v = parser.findTokenValue(toks, "TerrainType")
			if v != -1:
				self.terrainType = v
				continue

			v = parser.findTokenValue(toks, "PlotType")
			if v != -1:
				self.plotType = PlotTypes(v)
				continue

			# Units
			v = parser.findTokenValue(toks, "BeginUnit")
			if v != -1:
				unitDesc = CvUnitDesc()
				unitDesc.read(f, self.iX, self.iY)
				self.unitDescs.append(unitDesc)
				continue

			# City
			v = parser.findTokenValue(toks, "BeginCity")
			if v != -1:
				self.cityDesc = CvCityDesc()
				self.cityDesc.read(f, self.iX, self.iY)
				continue

			# Fog of War
			v = parser.findTokenValue(toks, "TeamReveal")
			if v != -1:
				for iTeamLoop in toks:
					iTeamLoop = iTeamLoop.lstrip('TeamReveal=')
					if len(iTeamLoop):
						self.abTeamPlotRevealed[int(iTeamLoop)] = True
				continue

			if parser.findTokenValue(toks, "EndPlot") != -1:
				break
		return True

################
# serialize map data
class CvMapDesc:
	def __init__(self):
		self.iGridW = 0
		self.iGridH = 0
		self.iTopLatitude = 0
		self.iBottomLatitude = 0
		self.bWrapX = 0
		self.bWrapY = 0
		self.worldSize = None
		self.climate = None
		self.seaLevel = None
		self.numPlotsWritten = 0
		self.numSignsWritten = 0
		self.bRandomizeResources = "false"

	# write map data
	def write(self, f):
		MAP = GC.getMap()
		iGridW = MAP.getGridWidth()
		iGridH = MAP.getGridHeight()

		f.write(
"BeginMap\n\tgrid width=%d\n\tgrid height=%d\n\ttop latitude=%d\n\tbottom latitude=%d\n\twrap X=%d\n\twrap Y=%d\n\tworld size=%s\n\
\tclimate=%s\n\tsealevel=%s\n\tnum plots written=%d\n\tnum signs written=%d\n\tRandomize Resources=false\nEndMap\n"
%(
	iGridW, iGridH, MAP.getTopLatitude(), MAP.getBottomLatitude(), MAP.isWrapX(), MAP.isWrapY(), GC.getWorldInfo(MAP.getWorldSize()).getType(),
	GC.getClimateInfo(MAP.getClimate()).getType(), GC.getSeaLevelInfo(MAP.getSeaLevel()).getType(), (iGridW * iGridH), CyEngine().getNumSigns()
)
		)

	# read map data
	def read(self, f):
		self.__init__()
		parser = CvWBParser()
		if parser.findNextToken(f, "BeginMap") == False:
			print "can't find map"
			return
		while True:
			nextLine = parser.getNextLine(f)
			toks = parser.getTokens(nextLine)
			if not toks:
				break

			v = parser.findTokenValue(toks, "grid width")
			if v != -1:
				self.iGridW = int(v)
				continue

			v = parser.findTokenValue(toks, "grid height")
			if v != -1:
				self.iGridH = int(v)
				continue

			v = parser.findTokenValue(toks, "top latitude")
			if v != -1:
				self.iTopLatitude = int(v)
				continue

			v = parser.findTokenValue(toks, "bottom latitude")
			if v != -1:
				self.iBottomLatitude = int(v)
				continue

			v = parser.findTokenValue(toks, "wrap X")
			if v != -1:
				self.bWrapX = int(v)
				continue

			v = parser.findTokenValue(toks, "wrap Y")
			if v != -1:
				self.bWrapY = int(v)
				continue

			v = parser.findTokenValue(toks, "world size")
			if v != -1:
				self.worldSize = v
				continue

			v = parser.findTokenValue(toks, "climate")
			if v != -1:
				self.climate = v
				continue

			v = parser.findTokenValue(toks, "sealevel")
			if v != -1:
				self.seaLevel = v
				continue

			v = parser.findTokenValue(toks, "num plots written")
			if v != -1:
				self.numPlotsWritten = int(v)
				continue

			v = parser.findTokenValue(toks, "num signs written")
			if v != -1:
				self.numSignsWritten = int(v)
				continue

			v = parser.findTokenValue(toks, "Randomize Resources")
			if v != -1:
				self.bRandomizeResources = v
				continue

			if parser.findTokenValue(toks, "EndMap") != -1:
				break

################
# serialize map data
class CvSignDesc:

	def __init__(self):
		self.iPlotX = 0
		self.iPlotY = 0
		self.playerType = 0
		self.szCaption = ""

	def apply(self):
		CyEngine().addSign(GC.getMap().plot(self.iPlotX, self.iPlotY), self.playerType, self.szCaption)

	# write sign data
	def write(self, f, sign):
		f.write("BeginSign\n")
		f.write("\tplotX=%d\n" %(sign.getPlot().getX(),))
		f.write("\tplotY=%d\n" %(sign.getPlot().getY(),))
		f.write("\tplayerType=%d\n" %(sign.getPlayerType(),))
		f.write("\tcaption=%s\n" %(sign.getCaption(),))
		f.write("EndSign\n")

	# read sign data
	def read(self, f):
		self.__init__()
		parser = CvWBParser()
		if parser.findNextToken(f, "BeginSign") == False:
			print "can't find sign"
			return
		while True:
			nextLine = parser.getNextLine(f)
			toks = parser.getTokens(nextLine)
			if not toks:
				break

			v = parser.findTokenValue(toks, "plotX")
			if v != -1:
				self.iPlotX = int(v)
				continue

			v = parser.findTokenValue(toks, "plotY")
			if v != -1:
				self.iPlotY = int(v)
				continue

			v = parser.findTokenValue(toks, "playerType")
			if v != -1:
				self.playerType = int(v)
				continue

			v = parser.findTokenValue(toks, "caption")
			if v != -1:
				self.szCaption = v
				continue

			if parser.findTokenValue(toks, "EndSign") != -1:
				break

		return True

# handles saving/loading a worldbuilder description file
class CvWBDesc:

	def __init__(self):
		self.gameDesc = CvGameDesc()
		self.playersDesc = None
		self.plotDesc = None # list
		self.signDesc = None # list
		self.mapDesc = CvMapDesc()

	def getDescFileName(self, fileName):
		return fileName+getWBSaveExtension()

	# Save out a high-level desc of the world, and height/terrainmaps
	def write(self, fileName):
		fileName = os.path.normpath(fileName)
		fileName,ext = os.path.splitext(fileName)
		CvUtil.pyPrint( 'saveDesc:%s, curDir:%s' %(fileName,os.getcwd()) )

		f = file(self.getDescFileName(fileName), "w")		# open text file
		f.write("Version=%d\n" % version)
		self.gameDesc.write(f)	# write game info

		for i in xrange(GC.getMAX_PC_TEAMS()):#
			CvTeamDesc().write(f, i)		# write team info

		for i in xrange(GC.getMAX_PC_PLAYERS()):#
			CvPlayerDesc().write(f, i)		# write player info

		self.mapDesc.write(f)	# write map info

		f.write("\n### Plot Info ###\n")
		MAP = GC.getMap()
		iGridW = MAP.getGridWidth()
		iGridH = MAP.getGridHeight()
		for iX in xrange(iGridW):
			for iY in xrange(iGridH):
				pDesc = CvPlotDesc()
				pDesc.write(f, MAP.plot(iX, iY))

		f.write("\n### Sign Info ###\n")
		iNumSigns = CyEngine().getNumSigns()
		for i in xrange(iNumSigns):
			sign = CyEngine().getSignByIndex(i)
			pDesc = CvSignDesc()
			pDesc.write(f, sign)

		f.close()

		print("WBSave done\n")
		return 0 # success

	# after reading setup the map
	def applyMap(self):

		self.gameDesc.apply()

		MAP = GC.getMap()
		# recreate map
		print("map rebuild. gridw=%d, gridh=%d" %(self.mapDesc.iGridW, self.mapDesc.iGridH))
		eWorldSize = WorldSizeTypes(GC.getInfoTypeForString(self.mapDesc.worldSize))
		eClimate = ClimateTypes(GC.getInfoTypeForString(self.mapDesc.climate))
		eSeaLevel = SeaLevelTypes(GC.getInfoTypeForString(self.mapDesc.seaLevel))
		MAP.rebuild(self.mapDesc.iGridW, self.mapDesc.iGridH, self.mapDesc.iTopLatitude, self.mapDesc.iBottomLatitude, self.mapDesc.bWrapX, self.mapDesc.bWrapY, eWorldSize, eClimate, eSeaLevel, 0, None)

		print "preapply plots"
		for pDesc in self.plotDesc:
			pDesc.preApply()	# set plot type / terrain type

		print("map apply - recalc areas/regions")
		MAP.recalculateAreas()

		print "apply plots"
		for pDesc in self.plotDesc:
			pDesc.apply()

		print "apply signs"
		for pDesc in self.signDesc:
			pDesc.apply()

		print "Randomize Resources"
		if self.mapDesc.bRandomizeResources != "false":
			for i in xrange(MAP.numPlots()):
				MAP.plotByIndex(i).setBonusType(BonusTypes.NO_BONUS)
			CyMapGenerator().addBonuses()

		print "WB apply done\n"
		return 0	# ok

	# add player starting plots if using random civs
	def getAssignedStartingPlots(self):

		MAP = GC.getMap()
		# Player stuff
		for iPlayerLoop in xrange(GC.getMAX_PC_PLAYERS()):#

			pPlayer = GC.getPlayer(iPlayerLoop)
			pWBPlayer = self.playersDesc[iPlayerLoop]

			# Random Start Location
			if pPlayer.getLeaderType() != -1 and pWBPlayer.bRandomStartLocation != "false":
				pPlayer.setStartingPlot(pPlayer.findStartingPlot(True), True)

			else:
				# Player's starting plot
				if pWBPlayer.iStartingX != -1 and pWBPlayer.iStartingY != -1:
					pPlayer.setStartingPlot(MAP.plot(pWBPlayer.iStartingX, pWBPlayer.iStartingY), True)

		return 0 # ok

	# add player objects in a last pass
	def applyInitialItems(self):

		MAP = GC.getMap()
		# Team stuff
		bMakePlotList = False
		if self.teamsDesc:
			for iTeamX in xrange(GC.getMAX_PC_TEAMS()):#

				if self.teamsDesc[iTeamX]:
					CyTeamX = GC.getTeam(iTeamX)

					pWBTeam = self.teamsDesc[iTeamX]

					if pWBTeam.bRevealMap == 1:
						bMakePlotList = True

					# Techs
					for techTypeTag in self.teamsDesc[iTeamX].techTypes:
						iTech = GC.getInfoTypeForString(techTypeTag)
						if iTech > -1:
							CyTeamX.setHasTech(iTech, True, PlayerTypes.NO_PLAYER, False, False)

					# Espionage Points against Other Teams
					for iEspionageLoop in xrange(len(pWBTeam.aaiEspionageAgainstTeams)):
						iTeam = pWBTeam.aaiEspionageAgainstTeams[iEspionageLoop][0]
						iEspionage = pWBTeam.aaiEspionageAgainstTeams[iEspionageLoop][1]
						CyTeamX.setEspionagePointsAgainstTeam(iTeam, iEspionage)

					# Contact with Other Teams
					for meetTeam in self.teamsDesc[iTeamX].bContactWithTeamList:
						CyTeamX.meet(meetTeam, False)

					# Wars
					for warTeam in self.teamsDesc[iTeamX].bWarWithTeamList:
						CyTeamX.declareWar(warTeam, False, WarPlanTypes.NO_WARPLAN)

					# Permanent War/Peace
					for permanentWarPeaceTeam in self.teamsDesc[iTeamX].bPermanentWarPeaceList:
						CyTeamX.setPermanentWarPeace(permanentWarPeaceTeam, True)

					# Open Borders
					for openBordersTeam in self.teamsDesc[iTeamX].bOpenBordersWithTeamList:
						CyTeamX.signOpenBorders(openBordersTeam)

					# Defensive Pacts
					for defensivePactTeam in self.teamsDesc[iTeamX].bDefensivePactWithTeamList:
						CyTeamX.signDefensivePact(defensivePactTeam)

					# Vassals
					for vassalTeam in self.teamsDesc[iTeamX].bVassalOfTeamList:
						GC.getTeam(vassalTeam).assignVassal(iTeamX, True)

					# Projects
					for project in (self.teamsDesc[iTeamX].projectType):
						iProject = GC.getInfoTypeForString(project)
						if iProject > -1:
							CyTeamX.changeProjectCount(iProject, 1)
							projectCount = CyTeamX.getProjectCount(iProject)
							CyTeamX.setProjectArtType(iProject, projectCount - 1, 0)

		if bMakePlotList:
			plotList = []
			for iPlotX in xrange(MAP.getGridWidth()):
				for iPlotY in xrange(MAP.getGridHeight()):
					plotList.append(MAP.plot(iPlotX, iPlotY))

		# Player stuff
		if self.playersDesc:
			for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):#

				if self.playersDesc[iPlayerX]:

					pPlayer = GC.getPlayer(iPlayerX)
					pWBPlayer = self.playersDesc[iPlayerX]

					# Player's starting gold
					pPlayer.setGold(pWBPlayer.iStartingGold)

					# Player's starting plot
					if pWBPlayer.iStartingX != -1 and pWBPlayer.iStartingY != -1:
						pPlayer.setStartingPlot(MAP.plot(pWBPlayer.iStartingX, pWBPlayer.iStartingY), True)

					# State Religion
					if pWBPlayer.stateReligion != "":
						iStateReligionID = GC.getInfoTypeForString(pWBPlayer.stateReligion)
						pPlayer.setLastStateReligion(iStateReligionID)

					# Starting Era
					if pWBPlayer.szStartingEra != "":
						iStartingEra = GC.getInfoTypeForString(pWBPlayer.szStartingEra)
						pPlayer.setCurrentEra(iStartingEra)

					# Random Start Location
					if pWBPlayer.bRandomStartLocation != "false":
						pPlayer.setStartingPlot(pPlayer.findStartingPlot(True), True)
						print("Setting player %d starting location to (%d,%d)", pPlayer.getID(), pPlayer.getStartingPlot().getX(), pPlayer.getStartingPlot().getY())

					# Civics
					for iCivicLoop in xrange(len(pWBPlayer.aaiCivics)):
						iCivicOption = pWBPlayer.aaiCivics[iCivicLoop][0]
						iCivic = pWBPlayer.aaiCivics[iCivicLoop][1]
						pPlayer.setCivics(iCivicOption,iCivic)

					# Attitude Extras
					for iAttitudeLoop in xrange(len(pWBPlayer.aaiAttitudeExtras)):
						iPlayer = pWBPlayer.aaiAttitudeExtras[iAttitudeLoop][0]
						iExtra = pWBPlayer.aaiAttitudeExtras[iAttitudeLoop][1]
						pPlayer.AI_setAttitudeExtra(iPlayer,iExtra)

					# City List
					for iCityListLoop in xrange(len(pWBPlayer.aszCityList)):
						pPlayer.addCityName(pWBPlayer.aszCityList[iCityListLoop])

		# cities
		for pDesc in self.plotDesc:
			pDesc.applyCity()

		# Team stuff
		if self.teamsDesc:
			for iTeamX in xrange(GC.getMAX_PC_TEAMS()):#

				if self.teamsDesc[iTeamX]:

					CyTeamX = GC.getTeam(iTeamX)
					pWBTeam = self.teamsDesc[iTeamX]

					# Reveal whole map
					if pWBTeam.bRevealMap == 1:

						for CyPlot in plotList:
							CyPlot.setRevealed(iTeamX, True, False, TeamTypes.NO_TEAM)

					# Vassal
					if pWBTeam.iVassalPower:
						CyTeamX.setVassalPower(pWBTeam.iVassalPower)
					if pWBTeam.iMasterPower:
						CyTeamX.setMasterPower(pWBTeam.iMasterPower)

					# Espionage Points Ever against All Teams
					if pWBTeam.iEspionageEver:
						CyTeamX.setEspionagePointsEver(pWBTeam.iEspionageEver)

		# Per plot stuff
		for iPlot in xrange(self.mapDesc.numPlotsWritten):

			pWBPlot = self.plotDesc[iPlot]

			# Reveal Fog of War for teams
			for iTeamX in xrange(GC.getMAX_PC_TEAMS()):#

				if pWBPlot.abTeamPlotRevealed[iTeamX] == 1:

					MAP.plot(pWBPlot.iX, pWBPlot.iY).setRevealed(iTeamX, True, False, TeamTypes.NO_TEAM)

		# units
		for pDesc in self.plotDesc:
			pDesc.applyUnits()

		return 0 # ok


	# Load in a high-level desc of the world, and height/terrainmaps
	def read(self, fileName):
		fileName = os.path.normpath(fileName)
		fileName, ext = os.path.splitext(fileName)
		if not ext:
			ext = getWBSaveExtension()
		CvUtil.pyPrint('loadDesc:%s, curDir:%s' %(fileName, os.getcwd()))

		if not os.path.isfile(fileName+ext):
			CvUtil.pyPrint("Error: file %s does not exist" %(fileName+ext,))
			return -1	# failed

		f = file(fileName+ext, "r")		# open text file

		parser = CvWBParser()
		v = int(parser.findNextTokenValue(f, "Version"))
		if v != version:
			CvUtil.pyPrint("Error: wrong WorldBuilder save version.  Expected %d, got %d" %(version, v))
			return -1	# failed

		print "Reading game desc"
		self.gameDesc.read(f)	# read game info

		print "Reading teams desc"
		filePos = f.tell()
		self.teamsDesc = []
		for i in xrange(GC.getMAX_PC_TEAMS()):#
			print ("reading team %d" %(i))
			teamsDesc = CvTeamDesc()
			if teamsDesc.read(f) == False:	# read team info
				f.seek(filePos)				# abort and backup
				break
			self.teamsDesc.append(teamsDesc)

		print "Reading players desc"
		self.playersDesc = []
		for i in xrange(GC.getMAX_PC_PLAYERS()):#
			playerDesc = CvPlayerDesc()
			playerDesc.read(f)				# read player info
			self.playersDesc.append(playerDesc)

		print "Reading map desc"
		self.mapDesc.read(f)	# read map info

		print("Reading/creating %d plot descs" %(self.mapDesc.numPlotsWritten,))
		self.plotDesc = []
		for i in xrange(self.mapDesc.numPlotsWritten):
			pDesc = CvPlotDesc()
			if pDesc.read(f)==True:
				self.plotDesc.append(pDesc)
			else:
				break

		print("Reading/creating %d sign descs" %(self.mapDesc.numSignsWritten,))
		self.signDesc = []
		for i in xrange(self.mapDesc.numSignsWritten):
			pDesc = CvSignDesc()
			if pDesc.read(f)==True:
				self.signDesc.append(pDesc)
			else:
				break

		f.close()
		print ("WB read done\n")
		return 0

