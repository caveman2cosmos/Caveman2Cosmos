from CvPythonExtensions import *
import os

GC = CyGlobalContext()
GAME = GC.getGame()
VERSION = "C2C_2"
fEncode = "utf-8"

############
# parser functions for WB desc
class CvWBParser:

	def __init__(self):
		self.iMaxFilePos = None

	# Don't call this twice without a readline() in-between as it will interpret that as being at the end of the file regardless of where it is.
	def isEndOfFile(self, iFilePos):
		if iFilePos == self.iMaxFilePos:
			return True
		self.iMaxFilePos = iFilePos

	# return a list of (comma separated) tokens from the line.  Strip whitespace on each token
	def getTokens(self, line):
		toks = []
		for item in line.split(","):
			toks.append(item.strip())
		return toks

	# Search for a token of the form item=value in the list of toks, and return value, or -1 if not found
	def findTokenValue(self, toks, token):
		for tok in toks:
			l = tok.split("=")
			if token == l[0]:
				if len(l) == 1:
					return token
				return l[1]
		return -1 # failed

	def getTokenValue(self, token):
		l = token.split("=")
		if len(l) > 1:
			return l[1]

	# Find the next line that contains token, breakpoints should be a token without value.
	# returns token with value if
	def findToken(self, f, token, breakPoint=None):
		if token:
			while True:
				line = f.readline()
				if not line:
					if self.isEndOfFile(f.tell()):
						print ("CvWBParser.findToken\n\tEnd of file reached looking for " + token)
						return
					continue
				toks = self.getTokens(line)
				if breakPoint and breakPoint in toks:
					return # end of search area
				for tok in toks:
					if token in tok:
						return tok


class MetaDesc:

	def read(self, f):
		self.szDescription = ""
		self.szModPath = ""
		scenarioVersion = None

		parser = CvWBParser()
		if parser.findToken(f, "BeginMeta", "BeginMap"):
			while True:
				nextLine = f.readline()
				if not nextLine:
					if parser.isEndOfFile(f.tell()):
						raise "MetaDesc.read - BeginMeta found, but EndMeta is missing!"
					continue
				toks = parser.getTokens(nextLine)

				v = parser.findTokenValue(toks, "Version")
				if v != -1:
					scenarioVersion = v
					continue

				v = parser.findTokenValue(toks, "Description")
				if v != -1:
					self.szDescription = v
					continue

				v = parser.findTokenValue(toks, "ModPath")
				if v != -1:
					self.szModPath = v
					continue

				if parser.findTokenValue(toks, "EndMeta") != -1:
					break

		if scenarioVersion != VERSION:
			raise "[ERROR] Wrong WorldBuilder save version. Expected %s, got %s" %(VERSION, scenarioVersion)


class CvGameDesc:

	def write(self, f):
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

		if GAME.getCircumnavigatedTeam() > -1:
			f.write("\tCircumnavigatedTeam=%d\n" % GAME.getCircumnavigatedTeam())

		f.write("EndGame\n")

	def read(self, f):
		print "CvGameDesc.read"
		self.iStartYear = GC.getDefineINT("START_YEAR")
		self.iStartEra = GC.getDefineINT("STANDARD_ERA")
		self.iCalendarType = GC.getDefineINT("STANDARD_CALENDAR")
		self.speedType = None
		self.options = ()
		self.mpOptions = ()
		self.forceControls = ()
		self.victories = ()
		self.iGameTurn = 0
		self.maxTurns = 0
		self.maxCityElimination = 0
		self.numAdvancedStartPoints = 0
		self.targetScore = 0
		self.iCircumnavigatedTeam = -1

		iStartpoint = f.tell()
		parser = CvWBParser()
		if parser.findToken(f, "BeginGame", "BeginMap"):

			while True:
				nextLine = f.readline()
				if not nextLine:
					if parser.isEndOfFile(f.tell()):
						print "CvGameDesc.read - end of file reached"
						break
					continue
				toks = parser.getTokens(nextLine)

				v = parser.findTokenValue(toks, "Era")
				if v != -1:
					self.iStartEra = GC.getInfoTypeForString(v)
					continue

				v = parser.findTokenValue(toks, "Speed")
				if v != -1:
					self.speedType = v
					continue

				v = parser.findTokenValue(toks, "Calendar")
				if v != -1:
					self.iCalendarType = GC.getInfoTypeForString(v)
					continue

				v = parser.findTokenValue(toks, "Option")
				if v != -1:
					self.options += (v,)
					continue

				v = parser.findTokenValue(toks, "MPOption")
				if v != -1:
					self.mpOptions += (v,)
					continue

				v = parser.findTokenValue(toks, "ForceControl")
				if v != -1:
					self.forceControls += (v,)
					continue

				v = parser.findTokenValue(toks, "Victory")
				if v != -1:
					self.victories += (v,)
					continue

				v = parser.findTokenValue(toks, "GameTurn")
				if v != -1:
					self.iGameTurn = int(v)
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

				v = parser.findTokenValue(toks, "CircumnavigatedTeam")
				if v != -1:
					self.iCircumnavigatedTeam = int(v)
					continue

				if parser.findTokenValue(toks, "EndGame") != -1:
					return
		f.seek(iStartpoint)


	def apply(self):
		print "CvGameDesc.apply"
		GAME.setStartYear(self.iStartYear)
		GAME.setGameTurn(self.iGameTurn)
		if self.iCircumnavigatedTeam > -1:
			GAME.setCircumnavigatedTeam(self.iCircumnavigatedTeam)


class CvMapDesc:

	def read(self, f):
		print "Read Map Data"
		self.iGridW = 0
		self.iGridH = 0
		self.iTopLatitude = 90
		self.iBottomLatitude = -90
		self.bWrapX = False
		self.bWrapY = False
		self.worldSize = None
		self.climate = None
		self.seaLevel = None
		self.bRandomizeResources = False
		parser = CvWBParser()
		if not parser.findToken(f, "BeginMap", "BeginPlot"):
			print "can't find map"
			return

		while True:
			nextLine = f.readline()
			if not nextLine:
				if parser.isEndOfFile(f.tell()):
					print "CvMapDesc.read - end of file reached"
					return
				continue
			toks = parser.getTokens(nextLine)

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
				self.bWrapX = int(v) > 0
				continue

			v = parser.findTokenValue(toks, "wrap Y")
			if v != -1:
				self.bWrapY = int(v) > 0
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

			v = parser.findTokenValue(toks, "Randomize Resources")
			if v != -1:
				self.bRandomizeResources = int(v) > 0
				continue

			if parser.findTokenValue(toks, "EndMap") != -1:
				return True


class CvTeamDesc:

	def write(self, f, idx):
		team = GC.getTeam(idx)
		if team.isAlive():
			f.write("BeginTeam\n")

			# Team ID (to make things easier to mess with in the text)
			f.write("\tTeamSlot=%d, (%s)\n" %(idx, team.getName().encode(fEncode)))
			bMinorCiv = team.isMinorCiv()
			if bMinorCiv:
				f.write("\tMinorNationCiv=1\n")
			# write techs
			for i in xrange(GC.getNumTechInfos()):
				tech = GC.getTechInfo(i)
				if team.isHasTech(i):
					f.write("\tTech=%s\n" % tech.getType())
				if tech.isRepeat():
					for j in xrange(team.getTechCount(i)):
						f.write("\tTech=%s\n" % tech.getType())

			if not team.isNPC():
				# write met other teams
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if i == idx: continue
					if team.isHasMet(i):
						f.write("\tContactWithTeam=%d, (%s)\n" %(i, GC.getTeam(i).getName().encode(fEncode)))

				# write Espionage against other teams
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if team.getEspionagePointsAgainstTeam(i) > 0:
						f.write("\tEspionageTeam=%d, EspionageAmount=%d, (%s)\n"
							%(i, team.getEspionagePointsAgainstTeam(i), GC.getTeam(i).getName().encode(fEncode))
						)
				if not bMinorCiv:
					# write warring teams
					for i in xrange(GC.getMAX_PC_TEAMS()):
						if team.isHasMet(i) and team.isAtWarWith(i):
							f.write("\tAtWar=%d, (%s)\n" %(i, GC.getTeam(i).getName().encode(fEncode)))

				# write permanent war/peace teams
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if team.isPermanentWarPeace(i):
						f.write("\tPermanentWarPeace=%d, (%s)\n" %(i, GC.getTeam(i).getName().encode(fEncode)))

				# write open borders other teams
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if team.isOpenBorders(i):
						f.write("\tOpenBordersWithTeam=%d, (%s)\n" %(i, GC.getTeam(i).getName().encode(fEncode)))

				# write defensive pact other teams
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if team.isDefensivePact(i):
						f.write("\tDefensivePactWithTeam=%d, (%s)\n" %(i, GC.getTeam(i).getName().encode(fEncode)))

				# write vassal state
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if team.isVassal(i):
						iType = self.getRelationshipStatus(idx, i)
						if iType == 1:
							f.write("\tVassalOfTeam=%d, Type=%s, (%s)\n" %(i, "FREE", GC.getTeam(i).getName().encode(fEncode)))
						elif iType == 0:
							f.write("\tVassalOfTeam=%d, Type=%s, (%s)\n" %(i, "CAP", GC.getTeam(i).getName().encode(fEncode)))

			for i in xrange(GC.getNumProjectInfos()):
				for j in xrange(team.getProjectCount(i)):
					f.write("\tProjectType=%s\n" %(GC.getProjectInfo(i).getType()))

			if team.getVassalPower() != 0:
				f.write("\tVassalPower=%d\n" % team.getVassalPower())
			if team.getMasterPower() != 0:
				f.write("\tMasterPower=%d\n" % team.getMasterPower())

			f.write("EndTeam\n")


	def getRelationshipStatus(self, iTeam1, iTeam2):
		for i in xrange(GAME.getIndexAfterLastDeal()):
			pDeal = GAME.getDeal(i)
			iPlayer1 = pDeal.getFirstPlayer()
			iPlayer2 = pDeal.getSecondPlayer()
			if iPlayer1 == -1 or iPlayer2 == -1: continue
			iTeamX = GC.getPlayer(pDeal.getFirstPlayer()).getTeam()
			iTeamY = GC.getPlayer(pDeal.getSecondPlayer()).getTeam()
			if iTeam1 == iTeamX and iTeam2 == iTeamY or iTeam2 == iTeamX and iTeam1 == iTeamY:
				for j in xrange(pDeal.getLengthFirstTrades()):
					if pDeal.getFirstTrade(j).ItemType == TradeableItems.TRADE_VASSAL:
						return 1
					if pDeal.getFirstTrade(j).ItemType == TradeableItems.TRADE_SURRENDER:
						return 0
				for j in xrange(pDeal.getLengthSecondTrades()):
					if pDeal.getSecondTrade(j).ItemType == TradeableItems.TRADE_VASSAL:
						return 1
					if pDeal.getSecondTrade(j).ItemType == TradeableItems.TRADE_SURRENDER:
						return 0
		return -1


	def read(self, f, iTeam):
		self.iTeam = -1
		self.techTypes = ()
		self.aaiEspionageAgainstTeams = []
		self.bContactWithTeamList = ()
		self.bWarWithTeamList = ()
		self.bPermanentWarPeaceList = ()
		self.bOpenBordersWithTeamList = ()
		self.bDefensivePactWithTeamList = ()
		self.bVassalOfTeamList = []
		self.projectType = []
		self.isMinorNationCiv = False
		self.iMasterPower = 0
		self.iVassalPower = 0

		parser = CvWBParser()
		if parser.findToken(f, "BeginTeam", "BeginPlot"):

			while True:
				nextLine = f.readline()
				if not nextLine:
					if parser.isEndOfFile(f.tell()):
						print "CvTeamDesc.read - end of file reached"
						return
					continue
				toks = parser.getTokens(nextLine)

				v = parser.findTokenValue(toks, "TeamSlot")
				if v != -1:
					if iTeam != int(v):
						return False # Team missing in scenario file
					continue

				v = parser.findTokenValue(toks, "Tech")
				if v != -1:
					self.techTypes += (v,)
					continue

				v = parser.findTokenValue(toks, "MinorNationCiv")
				if v != -1:
					self.isMinorNationCiv = int(v) > 0
					continue

				v = parser.findTokenValue(toks, "ContactWithTeam")
				if v != -1:
					self.bContactWithTeamList += (int(v),)
					continue

				v = parser.findTokenValue(toks, "EspionageTeam")
				if v != -1:
					self.aaiEspionageAgainstTeams.append([int(v), int(parser.findTokenValue(toks, "EspionageAmount"))])
					continue

				v = parser.findTokenValue(toks, "AtWar")
				if v != -1:
					self.bWarWithTeamList += (int(v),)
					continue

				v = parser.findTokenValue(toks, "PermanentWarPeace")
				if v != -1:
					self.bPermanentWarPeaceList += (int(v),)
					continue

				v = parser.findTokenValue(toks, "OpenBordersWithTeam")
				if v != -1:
					self.bOpenBordersWithTeamList += (int(v),)
					continue

				v = parser.findTokenValue(toks, "DefensivePactWithTeam")
				if v != -1:
					self.bDefensivePactWithTeamList += (int(v),)
					continue

				v = parser.findTokenValue(toks, "VassalOfTeam")
				if v != -1:
					iType = str(parser.findTokenValue(toks, "Type")).upper() != "FREE"
					self.bVassalOfTeamList.append([int(v), iType])
					continue

				v = parser.findTokenValue(toks, "ProjectType")
				if v != -1:
					self.projectType.append(v)
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
					self.iTeam = iTeam
					return True # completed successfully


############
class CvPlayerDesc:

	def write(self, f, idx):
		player = GC.getPlayer(idx)
		if player.isAlive():
			f.write("BeginPlayer\n\tPlayerSlot=%d\n\tTeam=%d\n" %(idx, player.getTeam()))

			if player.getHandicapType() > -1:
				f.write("\tHandicap=%s\n" % GC.getHandicapInfo(player.getHandicapType()).getType())

			# write leader and Civ Description info
			f.write("\tLeaderType=%s\n" % GC.getLeaderHeadInfo(player.getLeaderType()).getType())
			f.write("\tLeaderName=%s\n" % player.getNameKey().encode(fEncode))
			f.write("\tCivDesc=%s\n" % player.getCivilizationDescriptionKey().encode(fEncode))
			f.write("\tCivShortDesc=%s\n" % player.getCivilizationShortDescriptionKey().encode(fEncode))
			f.write("\tCivAdjective=%s\n" % player.getCivilizationAdjectiveKey().encode(fEncode))
			f.write("\tFlagDecal=%s\n" % player.getFlagDecal().encode(fEncode))
			f.write("\tWhiteFlag=%d\n" % player.isWhiteFlag())
			f.write("\tCivType=%s\n" % GC.getCivilizationInfo(player.getCivilizationType()).getType())
			f.write("\tColor=%s\n" % GC.getPlayerColorInfo(player.getPlayerColor()).getType())
			f.write("\tArtStyle=%s\n" % GC.getArtStyleTypes(player.getArtStyleType()))
			f.write("\tPlayableCiv=%d\n" % int(player.isPlayable()))

			if not player.isNPC():

				pPlot = player.getStartingPlot()
				if pPlot:
					f.write("\tStartingX=%d, StartingY=%d\n" %(pPlot.getX(), pPlot.getY()))

				f.write("\tStartingGold=%d\n" % player.getGold())

				for i in xrange(GC.getNumCivicOptionInfos()):
					f.write("\tCivicOption=%s, Civic=%s\n" %(GC.getCivicOptionInfo(i).getType(), GC.getCivicInfo(player.getCivics(i)).getType()))

				pPlayerReligionInfo = GC.getReligionInfo(player.getStateReligion())
				if pPlayerReligionInfo:
					f.write("\tStateReligion=%s\n" %(pPlayerReligionInfo.getType()))

				for i in xrange(GC.getMAX_PC_PLAYERS()):
					playerX = GC.getPlayer(i)
					if playerX.isAlive() and player.AI_getAttitudeExtra(i) != 0:
						f.write("\tAttitudePlayer=%d, (%s), AttitudeExtra=%d\n" %(i, playerX.getName().encode(fEncode), player.AI_getAttitudeExtra(i)))

				if player.getGoldenAgeTurns() > 0:
					f.write("\tGoldenAge=%d\n" % player.getGoldenAgeTurns())
				if player.getAnarchyTurns() > 0:
					f.write("\tAnarchy=%d\n" % player.getAnarchyTurns())
				if player.getGreatGeneralsThresholdModifier() > 0:
					f.write("\tCombatXP_ThresholdMod=%d\n" % player.getGreatGeneralsThresholdModifier())
				if player.getCombatExperience() > 0:
					f.write("\tCombatXP=%d\n" % player.getCombatExperience())
				if player.getCoastalTradeRoutes() != 0:
					f.write("\tCoastalTradeRoute=%d\n" % player.getCoastalTradeRoutes())
				if player.getStateReligionUnitProductionModifier() != 0:
					f.write("\tStateReligionUnit=%d\n" % player.getStateReligionUnitProductionModifier())
				if player.getStateReligionBuildingProductionModifier() != 0:
					f.write("\tStateReligionBuilding=%d\n" % player.getStateReligionBuildingProductionModifier())

			f.write("\tStartingEra=%s\n" %(GC.getEraInfo(player.getCurrentEra()).getType()))

			# write City List
			for i in xrange(player.getNumCityNames()):
				f.write("\tCityList=%s\n" %(player.getCityName(i)))

			if player.getScriptData():
				f.write("\tScriptData=%s\n\t!ScriptData\n" % player.getScriptData())

			f.write("EndPlayer\n")

	def read(self, f, iPlayer):
		self.iPlayer = iPlayer
		# Given to exe by CvWBInterface.getPlayerDesc()
		self.szCivDesc = ""
		self.szCivShortDesc = ""
		self.szLeaderName = ""
		self.szCivAdjective = ""
		self.szFlagDecal = ""

		# Given to exe by CvWBInterface.getPlayerData()
		self.civType = None
		self.leaderType = None
		self.color = None
		self.artStyle = None
		self.iHandicap = GC.getDefineINT("STANDARD_HANDICAP")
		self.isWhiteFlag = 0
		self.isPlayableCiv = 1
		self.iTeam = iPlayer

		# Applied through dll
		self.iStartingX = -1
		self.iStartingY = -1
		self.bRandomStartLocation = True
		self.stateReligion = ""
		self.szStartingEra = ""
		self.sScriptData = ""
		self.iStartingGold = 0
		self.iGoldenAge = 0
		self.iAnarchy = 0
		self.iCombatXP_ThresholdMod = 0
		self.iCombatXP = 0
		self.iCoastalTradeRoute = 0
		self.iStateReligionUnit = 0
		self.iStateReligionBuilding = 0
		self.aaiCivics = []
		self.aaiAttitudeExtras = []
		self.aszCityList = []

		parser = CvWBParser()
		if parser.findToken(f, "BeginPlayer", "BeginPlot"):

			while True:
				nextLine = f.readline()
				if not nextLine:
					if parser.isEndOfFile(f.tell()):
						print "CvPlayerDesc.read - end of file reached"
						return
					continue
				toks = parser.getTokens(nextLine)

				v = parser.findTokenValue(toks, "PlayerSlot")
				if v != -1:
					if iPlayer != int(v):
						# Player missing in scenario file
						self.civType = None
						return False
					continue

				v = parser.findTokenValue(toks, "LeaderType")
				if v != -1:
					self.leaderType = v
					continue

				v = parser.findTokenValue(toks, "LeaderName")
				if v != -1:
					self.szLeaderName = v.decode(fEncode)
					continue

				v = parser.findTokenValue(toks, "CivDesc")
				if v != -1:
					self.szCivDesc = v.decode(fEncode)
					continue

				v = parser.findTokenValue(toks, "CivShortDesc")
				if v != -1:
					self.szCivShortDesc = v.decode(fEncode)
					continue

				v = parser.findTokenValue(toks, "CivAdjective")
				if v != -1:
					self.szCivAdjective = v.decode(fEncode)
					continue

				v = parser.findTokenValue(toks, "FlagDecal")
				if v != -1:
					self.szFlagDecal = v.decode(fEncode)
					continue

				v = parser.findTokenValue(toks, "WhiteFlag")
				if v != -1:
					self.isWhiteFlag = int(v)
					continue

				v = parser.findTokenValue(toks, "CivType")
				if v != -1:
					self.civType = v
					continue

				v = parser.findTokenValue(toks, "Team")
				if v != -1:
					self.iTeam = int(v)
					continue

				v = parser.findTokenValue(toks, "Handicap")
				if v != -1:
					self.iHandicap = GC.getInfoTypeForString(v)
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

				v = parser.findTokenValue(toks, "StartingGold")
				if v != -1:
					self.iStartingGold = int(v)
					continue

				vX = parser.findTokenValue(toks, "StartingX")
				vY = parser.findTokenValue(toks, "StartingY")
				if vX != -1 and vY != -1:
					self.iStartingX = int(vX)
					self.iStartingY = int(vY)
					if self.iStartingX > -1 and self.iStartingY > -1:
						self.bRandomStartLocation = False
					continue

				v = parser.findTokenValue(toks, "StateReligion")
				if v != -1:
					self.stateReligion = v
					continue

				v = parser.findTokenValue(toks, "StartingEra")
				if v != -1:
					self.szStartingEra = v
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

				v = parser.findTokenValue(toks, "GoldenAge")
				if v != -1:
					self.iGoldenAge = max(0, int(v))
					continue

				v = parser.findTokenValue(toks, "Anarchy")
				if v != -1:
					self.iAnarchy = max(0, int(v))
					continue

				v = parser.findTokenValue(toks, "CombatXP_ThresholdMod")
				if v != -1:
					self.iCombatXP_ThresholdMod = int(v)
					continue

				v = parser.findTokenValue(toks, "CombatXP")
				if v != -1:
					self.iCombatXP = int(v)
					continue

				v = parser.findTokenValue(toks, "CoastalTradeRoute")
				if v != -1:
					self.iCoastalTradeRoute = int(v)
					continue

				v = parser.findTokenValue(toks, "StateReligionUnit")
				if v != -1:
					self.iStateReligionUnit = int(v)
					continue

				v = parser.findTokenValue(toks, "StateReligionBuilding")
				if v != -1:
					self.iStateReligionBuilding = int(v)
					continue

				v = parser.findTokenValue(toks, "ScriptData")
				if v != -1:
					peek = f.readline()
					if "!ScriptData" not in peek:
						v += "\n"
						while "!ScriptData" not in peek:
							v += peek
							peek = f.readline()

					self.sScriptData = v[:-1] # Don't want the newline char at the end.
					continue

				if parser.findTokenValue(toks, "EndPlayer") != -1:
					return True


############
class CvUnitDesc:

	# save unit desc to a file
	def write(self, f, unit, plot):
		info = GC.getUnitInfo(unit.getUnitType())
		f.write("\tBeginUnit\n\t\tUnitType=%s, UnitOwner=%d, (%s)\n"
			%(info.getType(), unit.getOwner(), GC.getPlayer(unit.getOwner()).getName().encode(fEncode))
		)
		if unit.isCommander():
			f.write("\t\tCommander=1\n")
		if unit.isCommodore():
			f.write("\t\tCommodore=1\n")
		if unit.getNameNoDesc():
			f.write("\t\tUnitName=%s\n" % unit.getNameNoDesc().encode(fEncode))
		if unit.getLeaderUnitType() != -1:
			f.write("\t\tLeaderUnitType=%s\n" % GC.getUnitInfo(unit.getLeaderUnitType()).getType())
		if unit.getDamage() > 0:
			f.write("\t\tDamage=%d\n" % unit.getDamage())

		f.write("\t\tLevel=%d, Experience=%d\n" %(unit.getLevel(), unit.getExperience()))

		for i in xrange(GC.getNumPromotionInfos()):
			if unit.isHasPromotion(i):
				f.write("\t\tPromotionType=%s\n" % GC.getPromotionInfo(i).getType())

		f.write("\t\tFacingDirection=%d\n" % unit.getFacingDirection())

		temp = unit.getGroup().getActivityType()
		if temp == ActivityTypes.ACTIVITY_SLEEP:
			f.write("\t\tSleep\n")
		elif temp == ActivityTypes.ACTIVITY_INTERCEPT:
			f.write("\t\tIntercept\n")
		elif temp == ActivityTypes.ACTIVITY_PATROL:
			f.write("\t\tPatrol\n")
		elif temp == ActivityTypes.ACTIVITY_PLUNDER:
			f.write("\t\tPlunder\n")

		f.write("\t\tUnitAIType=%s\n" % GC.getUnitAIInfo(unit.getUnitAIType()).getType())

		if unit.getScriptData():
			f.write("\t\tScriptData=%s\n\t\t!ScriptData\n" % unit.getScriptData())
		if unit.getImmobileTimer() > 0:
			f.write("\t\tImmobile=%d\n" % unit.getImmobileTimer())
		if unit.baseCombatStr() != info.getCombat():
			f.write("\t\tCombatStr=%d\n" % unit.baseCombatStr())

		f.write("\tEndUnit\n")

	def read(self, f, pX, pY):
		"read in unit data - at this point the first line 'BeginUnit' has already been read"
		self.plotX = pX
		self.plotY = pY
		self.unitType = None
		self.szName = None
		self.leaderUnitType = None
		self.owner = -1
		self.damage = 0
		self.level = 0
		self.experience = 0
		self.promotionType = []
		self.facingDirection = DirectionTypes.NO_DIRECTION
		self.isSleep = False
		self.isIntercept = False
		self.isPatrol = False
		self.isPlunder = False
		self.bCommander = False
		self.bCommodore = False
		self.szUnitAIType = "NO_UNITAI"
		self.iImmobile = 0
		self.iBaseCombatStr = -1
		self.sScriptData = ""

		parser = CvWBParser()
		while True:
			nextLine = f.readline()
			if not nextLine:
				if parser.isEndOfFile(f.tell()):
					print "CvUnitDesc.read - end of file reached"
					return
				continue
			toks = parser.getTokens(nextLine)

			v = parser.findTokenValue(toks, "UnitType")
			vOwner = parser.findTokenValue(toks, "UnitOwner")
			if v != -1 and vOwner != -1:
				self.unitType = v
				self.owner = int(vOwner)
				continue

			v = parser.findTokenValue(toks, "UnitName")
			if v != -1:
				self.szName = v.decode(fEncode)
				continue

			v = parser.findTokenValue(toks, "Commander")
			if v != -1:
				self.bCommander = True
				continue

			v = parser.findTokenValue(toks, "Commodore")
			if v != -1:
				self.bCommodore = True
				continue

			v = parser.findTokenValue(toks, "LeaderUnitType")
			if v != -1:
				self.leaderUnitType = v
				continue

			v = parser.findTokenValue(toks, "Damage")
			if v != -1:
				self.damage = int(v)
				continue

			v = parser.findTokenValue(toks, "Level")
			if v != -1:
				self.level = int(v)
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
				peek = f.readline()
				if "!ScriptData" not in peek:
					v += "\n"
					while "!ScriptData" not in peek:
						v += peek
						peek = f.readline()

				self.sScriptData = v[:-1] # Don't want the newline char at the end.
				continue

			v = parser.findTokenValue(toks, "Immobile")
			if v != -1:
				self.iImmobile = int(v)
				continue

			v = parser.findTokenValue(toks, "CombatStr")
			if v != -1:
				self.iBaseCombatStr = int(v)
				continue

			if parser.findTokenValue(toks, "EndUnit") != -1:
				break

	def apply(self):
		"after reading, this will actually apply the data"
		if self.owner == -1: return
		CyPlayer = GC.getPlayer(self.owner)
		unitTypeNum = GC.getInfoTypeForString(self.unitType)
		if unitTypeNum < 0:
			raise "Can't find type enum for type tag " + self.unitType
			return

		if self.szUnitAIType != "NO_UNITAI":
			eUnitAI = GC.getInfoTypeForString(self.szUnitAIType)
		else: eUnitAI = -1

		unit = CyPlayer.initUnit(unitTypeNum, self.plotX, self.plotY, UnitAITypes(eUnitAI), self.facingDirection)
		if not unit: return

		if self.bCommander:
			unit.setCommander(True)
		if self.bCommodore:
			unit.setCommodore(True)
		if self.leaderUnitType != None:
			leaderUnitTypeNum = GC.getInfoTypeForString(self.leaderUnitType)
			if leaderUnitTypeNum > -1:
				unit.setLeaderUnitType(leaderUnitTypeNum)

		if self.szName != None:
			unit.setName(self.szName)
		if self.damage != 0:
			unit.setDamage(self.damage, -1)
		if self.level != -1:
			unit.setLevel(self.level)
		if self.experience != -1:
			unit.setExperience(self.experience)
		for promo in self.promotionType:
			iPromo = GC.getInfoTypeForString(promo)
			if iPromo > -1:
				unit.setHasPromotion(iPromo, True)
		if self.isSleep:
			unit.getGroup().setActivityType(ActivityTypes.ACTIVITY_SLEEP)
		elif self.isIntercept:
			unit.getGroup().setActivityType(ActivityTypes.ACTIVITY_INTERCEPT)
		elif self.isPatrol:
			unit.getGroup().setActivityType(ActivityTypes.ACTIVITY_PATROL)
		elif self.isPlunder:
			unit.getGroup().setActivityType(ActivityTypes.ACTIVITY_PLUNDER)
		if self.iImmobile:
			unit.setImmobileTimer(self.iImmobile)
		if self.iBaseCombatStr > -1:
			unit.setBaseCombatStr(self.iBaseCombatStr)
		if self.sScriptData:
			unit.setScriptData(self.sScriptData)


###########
# serializes city data
class CvCityDesc:

	# write out city data
	def write(self, f, plot):
		city = plot.getPlotCity()
		if city is None:
			print "CvCityDesc.write - null city?"

		f.write("\tBeginCity\n\t\tCityOwner=%d, (%s)\n\t\tCityName=%s\n\t\tCityPopulation=%d\n\t\tStoredFood=%d\n"
			%(city.getOwner(), GC.getPlayer(city.getOwner()).getName().encode(fEncode), city.getName().encode(fEncode), city.getPopulation(), city.getFood())
		)
		if city.isProductionUnit():
			f.write("\t\tProductionUnit=%s\n" %(GC.getUnitInfo(city.getProductionUnit()).getType(),))
		elif city.isProductionBuilding():
			f.write("\t\tProductionBuilding=%s\n" %(GC.getBuildingInfo(city.getProductionBuilding()).getType(),))
		elif city.isProductionProject():
			f.write("\t\tProductionProject=%s\n" %(GC.getProjectInfo(city.getProductionProject()).getType(),))
		elif city.isProductionProcess():
			f.write("\t\tProductionProcess=%s\n" %(GC.getProcessInfo(city.getProductionProcess()).getType(),))

		for iI in xrange(GC.getNumBuildingInfos()):
			if city.hasBuilding(iI):
				f.write("\t\tBuildingType=%s, BuildDate=%d\n" %(GC.getBuildingInfo(iI).getType(), city.getBuildingOriginalTime(iI)))

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
			f.write("\t\tScriptData=%s\n\t\t!ScriptData\n" % city.getScriptData())

		# Player culture
		bFound = False
		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			if GC.getPlayer(iPlayerX).isAlive():
				name = GC.getPlayer(iPlayerX).getName().encode(fEncode)
			else: name = "Error"
			iPlayerCulture = city.getCultureTimes100(iPlayerX)
			if iPlayerCulture > 0:
				if not bFound:
					f.write("\t\tCityCultures\n")
					bFound = True
				f.write("\t\t\tPlayer=|%d| Culture=|%d| (%s)\n" %(iPlayerX, iPlayerCulture, name))

		if bFound: f.write("\t\t!CityCultures\n")

		if city.getDefenseDamage() > 0:
			f.write("\t\tDamage=%d\n" %(city.getDefenseDamage(),))
		if city.getOccupationTimer() > 0:
			f.write("\t\tOccupation=%d\n" %(city.getOccupationTimer(),))
		if city.getExtraHappiness() != 0:
			f.write("\t\tExtraHappiness=%d\n" %(city.getExtraHappiness(),))
		if city.getExtraHealth() != 0:
			f.write("\t\tExtraHealth=%d\n" %(city.getExtraHealth(),))
		if city.getExtraTradeRoutes() != 0:
			f.write("\t\tExtraTrade=%d\n" %(city.getExtraTradeRoutes(),))
		for i in xrange(GC.getNumBuildingInfos()):
			szType = GC.getBuildingInfo(i).getType()
			for k in xrange(YieldTypes.NUM_YIELD_TYPES):
				if city.getBuildingYieldChange(i, k) != 0:
					f.write("\t\tModifiedBuilding=%s, Yield=%s, Amount=%s\n" %(szType, GC.getYieldInfo(k).getType(), city.getBuildingYieldChange(i, k)))
			for k in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if city.getBuildingCommerceChange(i, k) != 0:
					f.write("\t\tModifiedBuilding=%s, Commerce=%s, Amount=%s\n" %(szType, GC.getCommerceInfo(k).getType(), city.getBuildingCommerceChange(i, k)))
			if city.getBuildingHappyChange(i) != 0:
				f.write("\t\tModifiedBuilding=%s, Happy=%s\n" %(szType, city.getBuildingHappyChange(i)))
			if city.getBuildingHealthChange(i) != 0:
				f.write("\t\tModifiedBuilding=%s, Health=%s\n" %(szType, city.getBuildingHealthChange(i)))

		f.write("\tEndCity\n")

	# read in city data - at this point the first line 'BeginCity' has already been read
	def read(self, f, iX, iY):
		print "CvCityDesc.read"
		self.plotX = iX
		self.plotY = iY
		self.owner = None
		self.name = None
		self.iPopulation = 0
		self.iFood = 0
		self.productionUnit = "NONE"
		self.productionBuilding = "NONE"
		self.productionProject = "NONE"
		self.productionProcess = "NONE"
		self.bldgType = []
		self.religions = []
		self.holyCityReligions = []
		self.corporations = []
		self.headquarterCorporations = []
		self.freeSpecialists = []
		self.lCulture = []
		self.iDamage = 0
		self.iOccupation = 0
		self.iExtraHappiness = 0
		self.iExtraHealth = 0
		self.iExtraTrade = 0
		self.lBuildingYield = []
		self.lBuildingCommerce = []
		self.lBuildingHappy = []
		self.lBuildingHealth = []
		self.sScriptData = ""

		parser = CvWBParser()
		while True:
			nextLine = f.readline()
			if not nextLine:
				if parser.isEndOfFile(f.tell()):
					print "CvCityDesc.read - end of file reached"
					return
				continue
			toks = parser.getTokens(nextLine)

			# City - Owner
			vOwner=parser.findTokenValue(toks, "CityOwner")
			if vOwner != -1:
				self.owner = int(vOwner)
				continue

			# City - Name
			vName=parser.findTokenValue(toks, "CityName")
			if vName != -1:
				self.name = (vName).decode(fEncode)
				continue

			# City - Population
			v = parser.findTokenValue(toks, "CityPopulation")
			if v != -1:
				self.iPopulation = (int(v))
				continue

			v = parser.findTokenValue(toks, "StoredFood")
			if v != -1:
				self.iFood = (int(v))
				continue

			# City - Production Unit
			v = parser.findTokenValue(toks, "ProductionUnit")
			if v != -1:
				self.productionUnit = v
				continue

			# City - Production Building
			v = parser.findTokenValue(toks, "ProductionBuilding")
			if v != -1:
				self.productionBuilding = v
				continue

			# City - Production Project
			v = parser.findTokenValue(toks, "ProductionProject")
			if v != -1:
				self.productionProject = v
				continue

			# City - Production Process
			v = parser.findTokenValue(toks, "ProductionProcess")
			if v != -1:
				self.productionProcess = v
				continue

			# City - Buildings
			v = parser.findTokenValue(toks, "BuildingType")
			v1 = parser.findTokenValue(toks, "BuildDate")
			if v != -1:
				if v1 == -1: v1 = None
				self.bldgType.append((v, v1))
				continue

			v = parser.findTokenValue(toks, "ModifiedBuilding")
			if v != -1:
				iBuilding = GC.getInfoTypeForString(v)
				v = parser.findTokenValue(toks, "Yield")
				if v != -1:
					self.lBuildingYield.append([iBuilding, GC.getInfoTypeForString(v), int(parser.findTokenValue(toks, "Amount"))])
					continue

				v = parser.findTokenValue(toks, "Commerce")
				if v != -1:
					self.lBuildingCommerce.append([iBuilding, GC.getInfoTypeForString(v), int(parser.findTokenValue(toks, "Amount"))])
					continue

				v = parser.findTokenValue(toks, "Happy")
				if v != -1:
					self.lBuildingHappy.append([iBuilding, int(v)])
					continue

				v = parser.findTokenValue(toks, "Health")
				if v != -1:
					self.lBuildingHealth.append([iBuilding, int(v)])
					continue

			# City - Religions
			v = parser.findTokenValue(toks, "ReligionType")
			if v != -1:
				self.religions.append(v)
				continue

			# City - HolyCity
			v = parser.findTokenValue(toks, "HolyCityReligionType")
			if v != -1:
				self.holyCityReligions.append(v)
				continue

			# City - Corporations
			v = parser.findTokenValue(toks, "CorporationType")
			if v != -1:
				self.corporations.append(v)
				continue

			# City - Headquarters
			v = parser.findTokenValue(toks, "HeadquarterCorporationType")
			if v != -1:
				self.headquarterCorporations.append(v)
				continue

			# City - Free Specialist
			v = parser.findTokenValue(toks, "FreeSpecialistType")
			if v != -1:
				self.freeSpecialists.append(v)
				continue

			# City - ScriptData
			v = parser.findTokenValue(toks, "ScriptData")
			if v != -1:
				peek = f.readline()
				if "!ScriptData" not in peek:
					v += "\n"
					while "!ScriptData" not in peek:
						v += peek
						peek = f.readline()

				self.sScriptData = v[:-1] # Don't want the newline char at the end.
				continue

			# Player Culture
			v = parser.findTokenValue(toks, "CityCultures")
			if v != -1:
				peek = f.readline()

				while "!CityCultures" not in peek:
					entry = peek.split("|")
					if int(entry[1]) < GC.getMAX_PLAYERS():
						self.lCulture.append([int(entry[1]), int(entry[3])])
					peek = f.readline()

				continue


			v = parser.findTokenValue(toks, "Damage")
			if v != -1:
				self.iDamage = int(v)
				continue

			v = parser.findTokenValue(toks, "Occupation")
			if v != -1:
				self.iOccupation = int(v)
				continue

			v = parser.findTokenValue(toks, "ExtraHappiness")
			if v != -1:
				self.iExtraHappiness = int(v)
				continue

			v = parser.findTokenValue(toks, "ExtraHealth")
			if v != -1:
				self.iExtraHealth = int(v)
				continue

			v = parser.findTokenValue(toks, "ExtraTrade")
			if v != -1:
				self.iExtraTrade = int(v)
				continue

			v = parser.findTokenValue(toks, "ModifiedBuilding")
			if v != -1:
				iBuilding = GC.getInfoTypeForString(v)
				v = parser.findTokenValue(toks, "Yield")
				if v != -1:
					self.lBuildingYield.append([iBuilding, GC.getInfoTypeForString(v), int(parser.findTokenValue(toks, "Amount"))])
					continue

				v = parser.findTokenValue(toks, "Commerce")
				if v != -1:
					self.lBuildingCommerce.append([iBuilding, GC.getInfoTypeForString(v), int(parser.findTokenValue(toks, "Amount"))])
					continue

				v = parser.findTokenValue(toks, "Happy")
				if v != -1:
					self.lBuildingHappy.append([iBuilding, int(v)])
					continue

				v = parser.findTokenValue(toks, "Health")
				if v != -1:
					self.lBuildingHealth.append([iBuilding, int(v)])
					continue

			if parser.findTokenValue(toks, "EndCity") != -1:
				break

	# after reading, this will actually apply the data
	def apply(self):
		print "CvCityDesc.apply"
		self.city = city = GC.getPlayer(self.owner).initCity(self.plotX, self.plotY)

		if self.name != None:
			city.setName(self.name, False)

		if self.iPopulation:
			city.setPopulation(self.iPopulation)
		if self.iFood:
			city.setFood(self.iFood)

		for item in self.lCulture:
			city.setCultureTimes100(item[0], item[1], False)

		for key, date in self.bldgType:
			iBuilding = GC.getInfoTypeForString(key)
			if iBuilding > -1:
				city.changeHasBuilding(iBuilding, True)

		for key in self.religions:
			iReligion = GC.getInfoTypeForString(key)
			if iReligion > -1:
				city.setHasReligion(iReligion, True, False, True)

		for key in self.holyCityReligions:
			iReligion = GC.getInfoTypeForString(key)
			if iReligion > -1:
				GAME.setHolyCity(iReligion, city, False)

		for key in self.corporations:
			iCorporation = GC.getInfoTypeForString(key)
			if iCorporation > -1:
				city.setHasCorporation(iCorporation, True, False, True)

		for key in self.headquarterCorporations:
			iCorporation = GC.getInfoTypeForString(key)
			if iCorporation > -1:
				GAME.setHeadquarters(iCorporation, city, False)

		for key in self.freeSpecialists:
			iSpecialist = GC.getInfoTypeForString(key)
			if iSpecialist > -1:
				city.changeFreeSpecialistCount(iSpecialist, 1)

		iProd = -1
		if self.productionUnit != "NONE":
			iProd = GC.getInfoTypeForString(self.productionUnit)
			if iProd > -1:
				city.pushOrder(OrderTypes.ORDER_TRAIN, iProd, -1, False, False, False, True)

		if iProd == -1 and self.productionBuilding != "NONE":
			iProd = GC.getInfoTypeForString(self.productionBuilding)
			if iProd > -1:
				city.pushOrder(OrderTypes.ORDER_CONSTRUCT, iProd, -1, False, False, False, True)

		if iProd == -1 and self.productionProject != "NONE":
			iProd = GC.getInfoTypeForString(self.productionProject)
			if iProd > -1:
				city.pushOrder(OrderTypes.ORDER_CREATE, iProd, -1, False, False, False, True)

		if iProd == -1 and self.productionProcess != "NONE":
			iProd = GC.getInfoTypeForString(self.productionProcess)
			if iProd > -1:
				city.pushOrder(OrderTypes.ORDER_MAINTAIN, iProd, -1, False, False, False, True)

		if self.sScriptData:
			city.setScriptData(self.sScriptData)

		if self.iDamage > 0:
			city.changeDefenseDamage(self.iDamage)
		if self.iOccupation > 0:
			city.setOccupationTimer(self.iOccupation)

	def postApply(self):
		print "CvCityDesc.postApply"
		city = self.city
		city.changeExtraHappiness(self.iExtraHappiness - city.getExtraHappiness())
		city.changeExtraHealth(self.iExtraHealth - city.getExtraHealth())
		city.changeExtraTradeRoutes(self.iExtraTrade - city.getExtraTradeRoutes())
		for item in self.lBuildingYield:
			city.setBuildingYieldChange(item[0], item[1], -city.getBuildingYieldChange(item[0], item[1]))
			city.setBuildingYieldChange(item[0], item[1], item[2])
		for item in self.lBuildingCommerce:
			city.setBuildingCommerceChange(item[0], item[1], -city.getBuildingCommerceChange(item[0], item[1]))
			city.setBuildingCommerceChange(item[0], item[1], item[2])
		for item in self.lBuildingHappy:
			city.setBuildingHappyChange(item[0], -city.getBuildingHappyChange(item[0]))
			city.setBuildingHappyChange(item[0], item[1])
		for item in self.lBuildingHealth:
			city.setBuildingHealthChange(item[0], -city.getBuildingHealthChange(item[0]))
			city.setBuildingHealthChange(item[0], item[1])


##########
# serializes plot data
class CvPlotDesc:

	def write(self, f, plot):
		f.write("BeginPlot\n")
		f.write("\tx=%d, y=%d\n" %(plot.getX(), plot.getY()))

		# scriptData
		temp = plot.getScriptData()
		if temp:
			f.write("\tScriptData=%s\n\t!ScriptData\n" % temp)
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
		for unit in plot.units():
			if unit.getUnitType() > -1:
				CvUnitDesc().write(f, unit, plot)
		# city
		if plot.isCity():
			CvCityDesc().write(f, plot)

		# Fog of War
		bFirst = True
		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
			if GC.getTeam(iTeamX).isAlive() and plot.isRevealed(iTeamX, 0):
				if bFirst:
					f.write("\tTeamReveal=")
					bFirst = False
				f.write("%d," % iTeamX)
		if not bFirst:
			f.write("\n") # terminate reveal line

		# Player culture
		bFound = False
		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			if GC.getPlayer(iPlayerX).isAlive():
				name = GC.getPlayer(iPlayerX).getName().encode(fEncode)
			else: name = "Error"
			iPlayerCulture = plot.getCulture(iPlayerX)
			if iPlayerCulture > 0:
				if not bFound:
					f.write("\t\tPlotCultures\n")
					bFound = True
				f.write("\t\t\tPlayer=|%d| Culture=|%d| (%s)\n" %(iPlayerX, iPlayerCulture, name))

		if bFound: f.write("\t\t!PlotCultures\n")

		f.write("EndPlot\n")


	def read(self, f):
		parser = CvWBParser()
		if parser.findToken(f, "BeginPlot", "BeginSign"):

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
			self.abTeamPlotRevealed = []
			self.lCulture = []
			self.sScriptData = ""

			while True:
				nextLine = f.readline()
				if not nextLine:
					if parser.isEndOfFile(f.tell()):
						print "CvPlotDesc.read - end of file reached"
						return
					continue
				toks = parser.getTokens(nextLine)

				x = parser.findTokenValue(toks, "x")
				y = parser.findTokenValue(toks, "y")
				if x != -1 and y != -1:
					self.iX = int(x)
					self.iY = int(y)
					continue

				v = parser.findTokenValue(toks, "Landmark")
				if v != -1:
					self.szLandmark = v
					continue

				v = parser.findTokenValue(toks, "ScriptData")
				if v != -1:
					peek = f.readline()
					if "!ScriptData" not in peek:
						v += "\n"
						while "!ScriptData" not in peek:
							v += peek
							peek = f.readline()

					self.sScriptData = v[:-1] # Don't want the newline char at the end.
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
					for teamLoop in toks:
						teamLoop = teamLoop.lstrip('TeamReveal=')
						if teamLoop:
							self.abTeamPlotRevealed.append(int(teamLoop))
					continue

				# Player Culture
				v = parser.findTokenValue(toks, "PlotCultures")
				if v != -1:
					peek = f.readline()

					while "!PlotCultures" not in peek:
						entry = peek.split("|")
						if int(entry[1]) < GC.getMAX_PLAYERS():
							self.lCulture.append([int(entry[1]), int(entry[3])])
						peek = f.readline()

					continue

				if parser.findTokenValue(toks, "EndPlot") != -1:
					return True


	def preApply(self):
		plot = GC.getMap().plot(self.iX, self.iY)
		if self.plotType != PlotTypes.NO_PLOT:
			plot.setPlotType(self.plotType, False, False)
		if self.terrainType:
			iTerrain = GC.getInfoTypeForString(self.terrainType)
			if iTerrain > -1:
				plot.setTerrainType(iTerrain, False, False)

	def apply(self, bRandomBonus):
		plot = GC.getMap().plot(self.iX, self.iY)
		plot.setNOfRiver(self.isNOfRiver, self.riverWEDirection)
		plot.setWOfRiver(self.isWOfRiver, self.riverNSDirection)
		plot.setStartingPlot(self.isStartingPlot)
		if not bRandomBonus and self.bonusType:
			iBonus = GC.getInfoTypeForString(self.bonusType)
			if iBonus > -1:
				plot.setBonusType(iBonus)
		if self.improvementType:
			iImprovement = GC.getInfoTypeForString(self.improvementType)
			if iImprovement > -1:
				plot.setImprovementType(iImprovement)
		if self.featureType:
			iFeature = GC.getInfoTypeForString(self.featureType)
			if iFeature > -1:
				plot.setFeatureType(iFeature, self.featureVariety)
		if self.routeType:
			iRoute = GC.getInfoTypeForString(self.routeType)
			if iRoute > -1:
				plot.setRouteType(iRoute)

		if self.szLandmark:
			CyEngine().addLandmark(plot, "%s" %(self.szLandmark))

		if self.sScriptData:
			plot.setScriptData(self.sScriptData)

	def applyUnits(self):
		for u in self.unitDescs:
			u.apply()


###############
# serialize map data
class CvSignDesc:

	# write sign data
	def write(self, f, sign):
		f.write("BeginSign\n")
		plot = sign.getPlot()
		f.write("\tplotX=%d\n" % plot.getX())
		f.write("\tplotY=%d\n" % plot.getY())
		iPlayer = sign.getPlayerType()
		if iPlayer > -1:
			f.write("\tplayerType=%d, (%s)\n" %(iPlayer, GC.getPlayer(iPlayer).getName().encode(fEncode)))
		else: f.write("\tplayerType=%d\n" %iPlayer)
		f.write("\tcaption=%s\n" % sign.getCaption())
		f.write("EndSign\n")

	# read sign data
	def read(self, f):
		self.iX = -1
		self.iY = -1
		self.playerType = 0
		self.szCaption = ""

		parser = CvWBParser()
		if parser.findToken(f, "BeginSign"):
			while True:
				nextLine = f.readline()
				if not nextLine:
					if parser.isEndOfFile(f.tell()):
						print "CvSignDesc.read - end of file reached"
						return
					continue
				toks = parser.getTokens(nextLine)

				v = parser.findTokenValue(toks, "plotX")
				if v != -1:
					self.iX = int(v)
					continue

				v = parser.findTokenValue(toks, "plotY")
				if v != -1:
					self.iY = int(v)
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
					return True


	def apply(self):
		print "sign will be added at %dx%dy for player %d with caption: '%s'" %(self.iX, self.iY, self.playerType, self.szCaption)
		pPlot = GC.getMap().plot(self.iX, self.iY)
		if pPlot and not pPlot.isNone():
			CyEngine().addSign(pPlot, self.playerType, self.szCaption)
			print "sign added at %dx%dy for player %d with caption: '%s'" %(self.iX, self.iY, self.playerType, self.szCaption)
		else:
			print "Plot invalide : x = %d, y = %d" % (x, y)		

# handles saving/loading a worldbuilder description file
class CvWBDesc:

	# Save out a high-level desc of the world.
	def write(self, fileName):
		fileName = os.path.normpath(fileName)
		fileName,ext = os.path.splitext(fileName)
		print 'saveDesc:%s, curDir:%s' %(fileName, os.getcwd())

		f = file(fileName + getWBSaveExtension(), "w") # open text file

		f.write("BeginMeta\n\tVersion=%s\n\tDescription=undescribed\n\tModPath=Mods\Caveman2Cosmos\nEndMeta\n" % VERSION)
		CvGameDesc().write(f)

		# Write map info
		MAP = GC.getMap()
		iGridW = MAP.getGridWidth()
		iGridH = MAP.getGridHeight()
		f.write("BeginMap\n\t\
grid width=%d\n\tgrid height=%d\n\ttop latitude=%d\n\tbottom latitude=%d\n\t\
wrap X=%d\n\twrap Y=%d\n\tworld size=%s\n\tclimate=%s\n\tsealevel=%s\n\t\
Randomize Resources=0\nEndMap\n"
			%(
				iGridW, iGridH,
				MAP.getTopLatitude(), MAP.getBottomLatitude(),
				MAP.isWrapX(), MAP.isWrapY(),
				GC.getWorldInfo(MAP.getWorldSize()).getType(),
				GC.getClimateInfo(MAP.getClimate()).getType(),
				GC.getSeaLevelInfo(MAP.getSeaLevel()).getType()
			)
		)
		# write team and player info
		for i in xrange(GC.getMAX_TEAMS()):
			CvTeamDesc().write(f, i)
		for i in xrange(GC.getMAX_PLAYERS()):
			CvPlayerDesc().write(f, i)

		# write plot info
		for iX in xrange(iGridW):
			for iY in xrange(iGridH):
				pDesc = CvPlotDesc()
				pDesc.write(f, MAP.plot(iX, iY))

		# write sign info
		iNumSigns = CyEngine().getNumSigns()
		for i in xrange(iNumSigns):
			sign = CyEngine().getSignByIndex(i)
			pDesc = CvSignDesc()
			pDesc.write(f, sign)

		f.close()

		print "WBSave done\n"
		return 0 # success

	# Load in a high-level desc of the world, and height/terrainmaps
	def read(self, fileName):
		self.gameDesc = CvGameDesc()
		self.mapDesc = CvMapDesc()
		self.metaDesc = MetaDesc()
		self.playersDesc = None
		self.plotDesc = None # list
		self.signDesc = None # list
		fileName, ext = os.path.splitext(os.path.normpath(fileName))
		if not ext:
			ext = getWBSaveExtension()
		fileName += ext

		f = file(fileName, "r")		# open text file

		self.metaDesc.read(f)

		print "Reading game desc"
		self.gameDesc.read(f)	# read game info

		print "Reading map desc"
		self.mapDesc.read(f)	# read map info

		print "Reading teams desc"
		self.teamsDesc = []
		for i in xrange(GC.getMAX_TEAMS()):
			iFilePos = f.tell()
			pDesc = CvTeamDesc()
			if not pDesc.read(f, i):
				# Player not found in scenario file, backtrack file position
				f.seek(iFilePos)
			self.teamsDesc.append(pDesc)

		print "Reading players desc"
		self.playersDesc = []
		for i in xrange(GC.getMAX_PLAYERS()):
			iFilePos = f.tell()
			pDesc = CvPlayerDesc()
			if not pDesc.read(f, i):
				# Player not found in scenario file, backtrack file position
				f.seek(iFilePos)
			self.playersDesc.append(pDesc)

		print "Reading plot descs"
		self.plotDesc = []
		while True:
			iFilePos = f.tell()
			pDesc = CvPlotDesc()
			if not pDesc.read(f):
				# No more plots to read
				f.seek(iFilePos)
				break
			if pDesc.iX != -1 and pDesc.iY != -1:
				self.plotDesc.append(pDesc)

		print "Reading sign descs"
		self.signDesc = []
		while True:
			pDesc = CvSignDesc()
			if not pDesc.read(f):
				# No more signs to read
				break
			if pDesc.iX != -1 and pDesc.iY != -1:
				self.signDesc.append(pDesc)

		f.close()
		print ("WB read done\n")
		return 0

	# after reading setup the map
	def applyMap(self):
		if GAME.getCurrentMap() == MapTypes.MAP_EARTH:
			self.gameDesc.apply()

		MAP = GC.getMap()
		# recreate map
		print("map rebuild. gridw=%d, gridh=%d" %(self.mapDesc.iGridW, self.mapDesc.iGridH))
		eWorldSize = WorldSizeTypes(GC.getInfoTypeForString(self.mapDesc.worldSize))
		eClimate = ClimateTypes(GC.getInfoTypeForString(self.mapDesc.climate))
		eSeaLevel = SeaLevelTypes(GC.getInfoTypeForString(self.mapDesc.seaLevel))
		MAP.rebuild(self.mapDesc.iGridW, self.mapDesc.iGridH, self.mapDesc.iTopLatitude, self.mapDesc.iBottomLatitude, self.mapDesc.bWrapX, self.mapDesc.bWrapY, eWorldSize, eClimate, eSeaLevel, 0, None)

		print "preapply plots"
		# Set plot/terrain type
		for pDesc in self.plotDesc:
			pDesc.preApply()

		print("map apply - recalc areas/regions")
		MAP.recalculateAreas()

		bRandomBonus = self.mapDesc.bRandomizeResources
		print "apply plots"
		for pDesc in self.plotDesc:
			pDesc.apply(bRandomBonus)

		print "Randomize Resources"
		if bRandomBonus:
			CyMapGenerator().addBonuses()

		print "apply signs"
		for pDesc in self.signDesc:
			pDesc.apply()
		# Toffer - Signs won't appear when starting the scenario
		# The WB quicksave made right after the scenario has initialized contain the sign data,
		# But if you manually enter WB and save again the data is gone,
		# so some other code after this point is removing all signs during game init.

		print "WB apply done\n"
		return 0 # ok

	# add player starting plots if using random civs
	def getAssignedStartingPlots(self):
		MAP = GC.getMap()

		for i in xrange(GC.getMAX_PC_PLAYERS()):
			pPlayer = GC.getPlayer(i)
			if pPlayer.isAlive():
				pWBPlayer = self.playersDesc[i]

				if pWBPlayer.iStartingX > -1 and pWBPlayer.iStartingY > -1:
					pPlayer.setStartingPlot(MAP.plot(pWBPlayer.iStartingX, pWBPlayer.iStartingY), True)

		GAME.assignStartingPlots(True, False)

		self.clearCache()
		return 0 # ok


	# add player objects in a last pass
	def applyInitialItems(self):
		print "Team applyInitialItems"
		MAP = GC.getMap()
		for pWBTeam in self.teamsDesc:
			if pWBTeam.iTeam < 0: continue
			team = GC.getTeam(pWBTeam.iTeam)

			for TYPE in pWBTeam.techTypes:
				team.setHasTech(GC.getInfoTypeForString(TYPE), True, -1, False, False)

			if team.isNPC(): continue

			for item in pWBTeam.bContactWithTeamList:
				team.meet(item, False)

			iSum = 0
			for item in pWBTeam.aaiEspionageAgainstTeams:
				team.setEspionagePointsAgainstTeam(item[0], item[1])
				iSum += item[1]
			if iSum:
				team.setEspionagePointsEver(iSum)

			if pWBTeam.isMinorNationCiv:
				team.setIsMinorCiv(True)
			else:
				for item in pWBTeam.bWarWithTeamList:
					team.declareWar(item, False, WarPlanTypes.NO_WARPLAN)

			for item in pWBTeam.bPermanentWarPeaceList:
				team.setPermanentWarPeace(item, True)

			for item in pWBTeam.bOpenBordersWithTeamList:
				team.signOpenBorders(item)

			for item in pWBTeam.bDefensivePactWithTeamList:
				team.signDefensivePact(item)

			for item in pWBTeam.bVassalOfTeamList:
				GC.getTeam(item[0]).assignVassal(pWBTeam.iTeam, item[1])
			if pWBTeam.iVassalPower > 0:
				team.setVassalPower(pWBTeam.iVassalPower)
			if pWBTeam.iMasterPower > 0:
				team.setMasterPower(pWBTeam.iMasterPower)

			for TYPE in pWBTeam.projectType:
				iProject = GC.getInfoTypeForString(TYPE)
				team.changeProjectCount(iProject, 1)
				team.setProjectArtType(iProject, team.getProjectCount(iProject) - 1, 0)

		for pWBPlayer in self.playersDesc:
			if pWBPlayer.civType is None: continue

			player = GC.getPlayer(pWBPlayer.iPlayer)
			player.setScriptData(pWBPlayer.sScriptData)

			if player.isNPC(): continue

			if pWBPlayer.stateReligion:
				iStateReligionID = GC.getInfoTypeForString(pWBPlayer.stateReligion)
				player.setLastStateReligion(iStateReligionID)

			if pWBPlayer.szStartingEra:
				iStartingEra = GC.getInfoTypeForString(pWBPlayer.szStartingEra)
				player.setCurrentEra(iStartingEra)


			for item in pWBPlayer.aaiCivics:
				player.setCivics(item[0],item[1])
			for item in pWBPlayer.aaiAttitudeExtras:
				player.AI_setAttitudeExtra(item[0],item[1])
			for item in pWBPlayer.aszCityList:
				player.addCityName(item)

			if not pWBPlayer.bRandomStartLocation:
				player.setStartingPlot(MAP.plot(pWBPlayer.iStartingX, pWBPlayer.iStartingY), True)

		GAME.assignStartingPlots(True, False)

		# Apply city data
		for item in self.plotDesc:
			if item.cityDesc:
				item.cityDesc.apply()
		for item in self.plotDesc:
			if item.cityDesc:
				item.cityDesc.postApply()

		# Apply team reveal and culture to plots
		for pWBPlot in self.plotDesc:
			if pWBPlot.lCulture or pWBPlot.abTeamPlotRevealed:
				plot = MAP.plot(pWBPlot.iX, pWBPlot.iY)
				for iTeamX in pWBPlot.abTeamPlotRevealed:
					plot.setRevealed(iTeamX, True, False, -1)
				for item in pWBPlot.lCulture:
					plot.setCulture(item[0], item[1], True)

		# Final player pass
		for i in xrange(GC.getMAX_PC_PLAYERS()):
			pWBPlayer = self.playersDesc[i]
			if pWBPlayer.civType is None: continue
			player = GC.getPlayer(pWBPlayer.iPlayer)
			player.setGold(pWBPlayer.iStartingGold)
			player.changeGreatGeneralsThresholdModifier(pWBPlayer.iCombatXP_ThresholdMod)
			player.setCombatExperience(pWBPlayer.iCombatXP)
			player.changeGoldenAgeTurns(pWBPlayer.iGoldenAge - player.getGoldenAgeTurns())
			player.changeAnarchyTurns(pWBPlayer.iAnarchy - player.getAnarchyTurns())
			player.changeCoastalTradeRoutes(pWBPlayer.iCoastalTradeRoute - player.getCoastalTradeRoutes())
			player.changeStateReligionUnitProductionModifier(pWBPlayer.iStateReligionUnit - player.getStateReligionUnitProductionModifier())
			player.changeStateReligionBuildingProductionModifier(pWBPlayer.iStateReligionBuilding - player.getStateReligionBuildingProductionModifier())

		# Apply unit data
		for item in self.plotDesc:
			item.applyUnits()

		self.clearCache()
		return 0

	def clearCache(self):
		del self.teamsDesc, self.playersDesc, self.plotDesc, self.signDesc, self.mapDesc, self.metaDesc

