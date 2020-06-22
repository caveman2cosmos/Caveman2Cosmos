from CvPythonExtensions import *
import os
import CvUtil
from array import *

GC = CyGlobalContext()
GAME = GC.getGame()
version = 15
fileencoding = "latin_1" # aka "iso-8859-1"

## self.bSpecial True will load the following additional special effects:
## Team Abilities: MapCentering, MapTrading, OpenBordersTrading, IgnoreIrrigation etc
## Team Effects: NukeInterception, EnemyWarWearinessModifier, ExtraDomainMoves, ExtraRouteChange, ImprovementYieldChange
## Player Effects: GoldenAgeTurns, AnarchyTurns, Coastal Trade Routes, State Religion Building/Unit Production, CombatXP
## City Effects: ExtraHappiness, ExtraHealth, ExtraTradeRoutes, BuildingYieldChange, BuildingCommerceChange, BuildingHappyChange, BuildingHealthChange, FreeBonus, NoBonusCount

############
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
			if tok == item:
				return True
		return False

	# Search for a token of the form item=value in the list of toks, and return value, or -1 if not found
	def findTokenValue(self, toks, item):
		for tok in toks:
			l = tok.split("=")
			if item == l[0]:
				if len(l) == 1:
					return item
				return l[1]
		return -1 # failed

	# Find the next line that contains the token item, return false if not found
	def findNextToken(self, f, item):
		while True:
			line = f.readline()
			if not line:
				return False # EOF
			toks=self.getTokens(line)
			if self.findToken(toks, item):
				return True
		return False

	# Find the next line that contains item=value, return value or -1 if not found
	def findNextTokenValue(self, f, item):
		while True:
			line = f.readline()
			if not line:
				return -1		# EOF
			toks=self.getTokens(line)
			val=self.findTokenValue(toks, item)
			if val != -1:
				return val
		return -1

############
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
		self.iStartYear = 0
		self.szDescription = ""
		self.szModPath = ""
		self.iRandom = 0

	# after reading, apply the game data
	def apply(self):
		GAME.setStartYear(self.iStartYear)

	# write out game data
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
		f.write("\tDescription=%s\n" % self.szDescription)
		f.write("\tModPath=Mods\Caveman2Cosmos\nEndGame\n")

	def read(self, f):
		"read in game data"
		self.__init__()

		parser = CvWBParser()
		if parser.findNextTokenValue(f, "BeginGame") != -1:
			while True:
				nextLine = f.readline()
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

############
class CvTeamDesc:

	def write(self, f, idx):
		team = GC.getTeam(idx)
		f.write("BeginTeam\n")

		# Team ID (to make things easier to mess with in the text)
		f.write("\tTeamSlot=%d, (%s)\n" %(idx, team.getName().encode(fileencoding)))

		if team.isAlive():
			bMinorCiv = team.isMinorCiv()
			if bMinorCiv:
				f.write("\tMinorNationCiv=1\n")
			bMapVisible = False
			# write techs
			for i in xrange(GC.getNumTechInfos()):
				tech = GC.getTechInfo(i)
				if team.isHasTech(i):
					f.write("\tTech=%s\n" % tech.getType())
					if tech.isMapVisible():
						bMapVisible = True
				if tech.isRepeat():
					for j in xrange(team.getTechCount(i)):
						f.write("\tTech=%s\n" % tech.getType())

			if not team.isNPC():
				# write met other teams
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if i == idx: continue
					if team.isHasMet(i):
						f.write("\tContactWithTeam=%d, (%s)\n" %(i, GC.getTeam(i).getName().encode(fileencoding)))

				# write Espionage Ever against other teams
				if team.getEspionagePointsEver() > 0:
					f.write("\tEspionageEverAmount=%d\n" % team.getEspionagePointsEver())

				# write Espionage against other teams
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if team.getEspionagePointsAgainstTeam(i) > 0:
						f.write("\tEspionageTeam=%d, EspionageAmount=%d, (%s)\n"
							%(i, team.getEspionagePointsAgainstTeam(i), GC.getTeam(i).getName().encode(fileencoding))
						)
				if not bMinorCiv:
					# write warring teams
					for i in xrange(GC.getMAX_PC_TEAMS()):
						if team.isHasMet(i) and team.isAtWar(i):
							f.write("\tAtWar=%d, (%s)\n" %(i, GC.getTeam(i).getName().encode(fileencoding)))

				# write permanent war/peace teams
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if team.isPermanentWarPeace(i):
						f.write("\tPermanentWarPeace=%d, (%s)\n" %(i, GC.getTeam(i).getName().encode(fileencoding)))

				# write open borders other teams
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if team.isOpenBorders(i):
						f.write("\tOpenBordersWithTeam=%d, (%s)\n" %(i, GC.getTeam(i).getName().encode(fileencoding)))

				# write defensive pact other teams
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if team.isDefensivePact(i):
						f.write("\tDefensivePactWithTeam=%d, (%s)\n" %(i, GC.getTeam(i).getName().encode(fileencoding)))

				# write vassal state
				for i in xrange(GC.getMAX_PC_TEAMS()):
					if team.isVassal(i):
						iType = self.getRelationshipStatus(idx, i)
						if iType == 1:
							f.write("\tVassalOfTeam=%d, Type=%s, (%s)\n" %(i, "FREE", GC.getTeam(i).getName().encode(fileencoding)))
						elif iType == 0:
							f.write("\tVassalOfTeam=%d, Type=%s, (%s)\n" %(i, "CAP", GC.getTeam(i).getName().encode(fileencoding)))

			for i in xrange(GC.getNumProjectInfos()):
				for j in xrange(team.getProjectCount(i)):
					f.write("\tProjectType=%s\n" %(GC.getProjectInfo(i).getType()))

			if bMapVisible:
				f.write("\tRevealMap=1\n")

			if team.getVassalPower() != 0:
				f.write("\tVassalPower=%d\n" % team.getVassalPower())
			if team.getMasterPower() != 0:
				f.write("\tMasterPower=%d\n" % team.getMasterPower())

			if team.isMapCentering():
				f.write("\tMapCentering=1\n")
			if team.isMapTrading():
				f.write("\tMapTrading=1\n")
			if team.isTechTrading():
				f.write("\tTechTrading=1\n")
			if team.isGoldTrading():
				f.write("\tGoldTrading=1\n")
			if team.isOpenBordersTrading():
				f.write("\tOpenBordersTrading=1\n")
			if team.isDefensivePactTrading():
				f.write("\tDefensivePactTrading=1\n")
			if team.isPermanentAllianceTrading():
				f.write("\tPermanentAllianceTrading=1\n")
			if team.isVassalStateTrading():
				f.write("\tVassalStateTrading=1\n")
			if team.isBridgeBuilding():
				f.write("\tBridgeBuilding=1\n")
			if team.isIrrigation():
				f.write("\tIrrigation=1\n")
			if team.isIgnoreIrrigation():
				f.write("\tIgnoreIrrigation=1\n")
			if team.isWaterWork():
				f.write("\tWaterWork=1\n")
			if team.isExtraWaterSeeFrom():
				f.write("\tExtraWaterSeeFrom=1\n")
			if team.getNukeInterception() != 0:
				f.write("\tNukeInterception=%d\n" %(team.getNukeInterception()))
			if team.getEnemyWarWearinessModifier() != 0:
				f.write("\tEnemyWarWeariness=%d\n" %(team.getEnemyWarWearinessModifier()))
			for item in xrange(DomainTypes.NUM_DOMAIN_TYPES):
				if team.getExtraMoves(item) != 0:
					f.write("\tDomainType=%s, ExtraMoves=%d\n" %(GC.getDomainInfo(item).getType(), team.getExtraMoves(item)))
			for item in xrange(GC.getNumRouteInfos()):
				if team.getRouteChange(item) != 0:
					f.write("\tRouteType=%s, ExtraMoves=%d\n" %(GC.getRouteInfo(item).getType(), team.getRouteChange(item)))
			for item in xrange(GC.getNumImprovementInfos()):
				for k in xrange(YieldTypes.NUM_YIELD_TYPES):
					if team.getImprovementYieldChange(item, k) != 0:
						f.write("\tImprovementType=%s, YieldType=%s, ExtraYield=%d\n" %(GC.getImprovementInfo(item).getType(), GC.getYieldInfo(k).getType(), team.getImprovementYieldChange(item, k)))

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
		self.iTeam = iTeam
		self.techTypes = ()
		self.aaiEspionageAgainstTeams = []
		self.bContactWithTeamList = ()
		self.bWarWithTeamList = ()
		self.bPermanentWarPeaceList = ()
		self.bOpenBordersWithTeamList = ()
		self.bDefensivePactWithTeamList = ()
		self.bVassalOfTeamList = []
		self.projectType = []
		self.isMinorNationCiv = 0
		self.bRevealMap = 0
		self.iMasterPower = 0
		self.iVassalPower = 0
		self.iEspionageEver = 0
		self.bMapCentering = 0
		self.bMapTrading = 0
		self.bTechTrading = 0
		self.bGoldTrading = 0
		self.bOpenBordersTrading = 0
		self.bDefensivePactTrading = 0
		self.bPermanentAllianceTrading = 0
		self.bVassalStateTrading = 0
		self.bBridgeBuilding = 0
		self.bIrrigation = 0
		self.bIgnoreIrrigation = 0
		self.bWaterWork = 0
		self.bExtraWaterSeeFrom = 0
		self.iNukeInterception = 0
		self.iEnemyWarWeariness = 0
		self.lDomainMoves = []
		self.lRouteMoves = []
		self.lImprovementYield = []

		parser = CvWBParser()
		if parser.findNextTokenValue(f, "BeginTeam") != -1:
			while True:
				nextLine = f.readline()
				toks = parser.getTokens(nextLine)
				if not toks:
					break

				v = parser.findTokenValue(toks, "TeamSlot")
				if v != -1:
					if iTeam != int(v):
						return False # Team missing in scenario file
					continue

				v = parser.findTokenValue(toks, "Tech")
				if v != -1:
					self.techTypes = self.techTypes + (v,)
					continue

				v = parser.findTokenValue(toks, "MinorNationCiv")
				if v != -1:
					self.isMinorNationCiv = int(v)
					continue

				v = parser.findTokenValue(toks, "ContactWithTeam")
				if v != -1:
					self.bContactWithTeamList += (int(v),)
					continue

				v = parser.findTokenValue(toks, "EspionageEverAmount")
				if v != -1:
					self.iEspionageEver = int(v)
					continue

				v = parser.findTokenValue(toks, "EspionageTeam")
				if v != -1:
					self.aaiEspionageAgainstTeams.append([int(v), int(parser.findTokenValue(toks, "EspionageAmount"))])
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
					iType = str(parser.findTokenValue(toks, "Type")).upper() != "FREE"
					self.bVassalOfTeamList.append([int(v), iType])
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

				v = parser.findTokenValue(toks, "MapCentering")
				if v!=-1:
					self.bMapCentering = int(v)
					continue

				v = parser.findTokenValue(toks, "MapTrading")
				if v!=-1:
					self.bMapTrading = int(v)
					continue

				v = parser.findTokenValue(toks, "TechTrading")
				if v!=-1:
					self.bTechTrading = int(v)
					continue

				v = parser.findTokenValue(toks, "GoldTrading")
				if v!=-1:
					self.bGoldTrading = int(v)
					continue

				v = parser.findTokenValue(toks, "OpenBordersTrading")
				if v!=-1:
					self.bOpenBordersTrading = int(v)
					continue

				v = parser.findTokenValue(toks, "DefensivePactTrading")
				if v!=-1:
					self.bDefensivePactTrading = int(v)
					continue

				v = parser.findTokenValue(toks, "PermanentAllianceTrading")
				if v!=-1:
					self.bPermanentAllianceTrading = int(v)
					continue

				v = parser.findTokenValue(toks, "VassalStateTrading")
				if v!=-1:
					self.bVassalStateTrading = int(v)
					continue

				v = parser.findTokenValue(toks, "BridgeBuilding")
				if v!=-1:
					self.bBridgeBuilding = int(v)
					continue

				v = parser.findTokenValue(toks, "Irrigation")
				if v!=-1:
					self.bIrrigation = int(v)
					continue

				v = parser.findTokenValue(toks, "IgnoreIrrigation")
				if v!=-1:
					self.bIgnoreIrrigation = int(v)
					continue

				v = parser.findTokenValue(toks, "WaterWork")
				if v!=-1:
					self.bWaterWork = int(v)
					continue

				v = parser.findTokenValue(toks, "ExtraWaterSeeFrom")
				if v!=-1:
					self.bExtraWaterSeeFrom = int(v)
					continue

				v = parser.findTokenValue(toks, "NukeInterception")
				if v!=-1:
					self.iNukeInterception = int(v)
					continue

				v = parser.findTokenValue(toks, "EnemyWarWeariness")
				if v!=-1:
					self.iEnemyWarWeariness = int(v)
					continue

				v = parser.findTokenValue(toks, "DomainType")
				if v!=-1:
					self.lDomainMoves.append([GC.getInfoTypeForString(v), int(parser.findTokenValue(toks, "ExtraMoves"))])
					continue

				v = parser.findTokenValue(toks, "RouteType")
				if v!=-1:
					self.lRouteMoves.append([GC.getInfoTypeForString(v), int(parser.findTokenValue(toks, "ExtraMoves"))])
					continue

				v = parser.findTokenValue(toks, "ImprovementType")
				if v!=-1:
					iYield = GC.getInfoTypeForString(parser.findTokenValue(toks, "YieldType"))
					iExtra = int(parser.findTokenValue(toks, "ExtraYield"))
					self.lImprovementYield.append([GC.getInfoTypeForString(v), iYield, iExtra])
					continue

				if parser.findTokenValue(toks, "EndTeam") != -1:
					return True # completed successfully


############
class CvPlayerDesc:

	def write(self, f, idx):
		player = GC.getPlayer(idx)
		f.write("BeginPlayer\n\tPlayerSlot=%d\n\tTeam=%d\n" %(idx, player.getTeam()))

		if player.getHandicapType() > -1:
			f.write("\tHandicap=%s\n" % GC.getHandicapInfo(player.getHandicapType()).getType())

		# write leader and Civ Description info
		if player.isAlive():
			f.write("\tLeaderType=%s\n" % GC.getLeaderHeadInfo(player.getLeaderType()).getType())
			f.write("\tLeaderName=%s\n" % player.getNameKey().encode(fileencoding))
			f.write("\tCivDesc=%s\n" % player.getCivilizationDescriptionKey().encode(fileencoding))
			f.write("\tCivShortDesc=%s\n" % player.getCivilizationShortDescriptionKey().encode(fileencoding))
			f.write("\tCivAdjective=%s\n" % player.getCivilizationAdjectiveKey().encode(fileencoding))
			f.write("\tFlagDecal=%s\n" % player.getFlagDecal().encode(fileencoding))
			f.write("\tWhiteFlag=%d\n" % player.isWhiteFlag())
			f.write("\tCivType=%s\n" % GC.getCivilizationInfo(player.getCivilizationType()).getType())
			f.write("\tColor=%s\n" % GC.getPlayerColorInfo(player.getPlayerColor()).getType())
			f.write("\tArtStyle=%s\n" % GC.getArtStyleTypes(player.getArtStyleType()))
			f.write("\tPlayableCiv=%d\n" % int(player.isPlayable()))

			f.write("\tRandomStartLocation=false\n")
			if not player.isNPC():

				pPlot = player.getStartingPlot()
				if not pPlot.isNone():
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
						f.write("\tAttitudePlayer=%d, (%s), AttitudeExtra=%d\n" %(i, playerX.getName().encode(fileencoding), player.AI_getAttitudeExtra(i)))

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
		self.isWhiteFlag = 0
		self.isPlayableCiv = 1
		self.handicap = GC.getHandicapInfo(GC.getDefineINT("STANDARD_HANDICAP")).getType()
		self.iTeam = iPlayer

		# Applied through dll
		self.iStartingX = -1
		self.iStartingY = -1
		self.bRandomStartLocation = "false"
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
		if parser.findNextTokenValue(f, "BeginPlayer") != -1:
			while True:
				nextLine = f.readline()
				toks = parser.getTokens(nextLine)
				if not toks:
					break

				v = parser.findTokenValue(toks, "PlayerSlot")
				if v != -1:
					if iPlayer != int(v):
						# Player missing in scenario file
						return False
					continue

				v = parser.findTokenValue(toks, "LeaderType")
				if v != -1:
					self.leaderType = v
					continue

				v = parser.findTokenValue(toks, "LeaderName")
				if v != -1:
					self.szLeaderName = v.decode("latin_1")
					continue

				v = parser.findTokenValue(toks, "CivDesc")
				if v != -1:
					self.szCivDesc = v.decode("latin_1")
					continue

				v = parser.findTokenValue(toks, "CivShortDesc")
				if v != -1:
					self.szCivShortDesc = v.decode("latin_1")
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

				v = parser.findTokenValue(toks, "StartingGold")
				if v != -1:
					self.iStartingGold = int(v)
					continue

				vX = parser.findTokenValue(toks, "StartingX")
				vY = parser.findTokenValue(toks, "StartingY")
				if vX != -1 and vY != -1:
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

				v = parser.findTokenValue(toks, "GoldenAge")
				if v!=-1:
					self.iGoldenAge = max(0, int(v))
					continue

				v = parser.findTokenValue(toks, "Anarchy")
				if v!=-1:
					self.iAnarchy = max(0, int(v))
					continue

				v = parser.findTokenValue(toks, "CombatXP_ThresholdMod")
				if v!=-1:
					self.iCombatXP_ThresholdMod = int(v)
					continue

				v = parser.findTokenValue(toks, "CombatXP")
				if v!=-1:
					self.iCombatXP = int(v)
					continue

				v = parser.findTokenValue(toks, "CoastalTradeRoute")
				if v!=-1:
					self.iCoastalTradeRoute = int(v)
					continue

				v = parser.findTokenValue(toks, "StateReligionUnit")
				if v!=-1:
					self.iStateReligionUnit = int(v)
					continue

				v = parser.findTokenValue(toks, "StateReligionBuilding")
				if v!=-1:
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

					self.sScriptData = v[:-1] # Don't want the newline char ath the end.
					continue

				if parser.findTokenValue(toks, "EndPlayer") != -1:
					return True
		return False


############
class CvUnitDesc:
	"unit WB serialization"
	def __init__(self):
		self.plotX = -1
		self.plotY = -1
		self.unitType = None
		self.szName = None
		self.leaderUnitType = None
		self.owner = -1
		self.damage = 0
		self.level = 0
		self.experience = 0
		self.promotionType = []
		self.facingDirection = DirectionTypes.NO_DIRECTION;
		self.isSleep = False
		self.isIntercept = False
		self.isPatrol = False
		self.isPlunder = False
		self.szUnitAIType = "NO_UNITAI"

		self.sScriptData = ""
		self.iImmobile = 0
		self.iBaseCombatStr = -1
		self.iExtraCargo = 0

	# save unit desc to a file
	def write(self, f, unit, plot):
		info = GC.getUnitInfo(unit.getUnitType())
		f.write("\tBeginUnit\n\t\tUnitType=%s, UnitOwner=%d, (%s)\n"
			%(info.getType(), unit.getOwner(), GC.getPlayer(unit.getOwner()).getName().encode(fileencoding))
		)
		if unit.getNameNoDesc():
			f.write("\t\tUnitName=%s\n" % unit.getNameNoDesc().encode(fileencoding))
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
		if unit.cargoSpace() != info.getCargoSpace():
			f.write("\t\tExtraCargo=%d\n" %(unit.cargoSpace() - info.getCargoSpace()))

		f.write("\tEndUnit\n")

	def read(self, f, pX, pY):
		"read in unit data - at this point the first line 'BeginUnit' has already been read"
		self.__init__()
		self.plotX = pX
		self.plotY = pY
		px = -5
		if pX < 0 or pY < 0:
			raise "invalid plot coords"

		parser = CvWBParser()
		while True:
			nextLine = f.readline()
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

				self.sScriptData = v[:-1] # Don't want the newline char ath the end.
				continue

			v = parser.findTokenValue(toks, "Immobile")
			if v != -1:
				self.iImmobile = int(v)
				continue

			v = parser.findTokenValue(toks, "CombatStr")
			if v != -1:
				self.iBaseCombatStr = int(v)
				continue

			v = parser.findTokenValue(toks, "ExtraCargo")
			if v != -1:
				self.iExtraCargo = int(v)
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

		CvUtil.pyAssert(self.plotX >= 0 and self.plotY >= 0, "invalid plot coords")
		unit = CyPlayer.initUnit(unitTypeNum, self.plotX, self.plotY, UnitAITypes(eUnitAI), self.facingDirection)
		if not unit: return

		if self.szName != None:
			unit.setName(self.szName)
		#leader unit type
		if self.leaderUnitType != None:
			leaderUnitTypeNum = GC.getInfoTypeForString(self.leaderUnitType)
			if leaderUnitTypeNum > -1:
				unit.setLeaderUnitType(leaderUnitTypeNum)

		#other properties
		if self.damage != 0:
			unit.setDamage(self.damage, -1)
		if self.level != -1:
			unit.setLevel(self.level)
		if self.experience != -1:
			unit.setExperience(self.experience, -1)
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

		if self.sScriptData:
			unit.setScriptData(self.sScriptData)
		unit.setImmobileTimer(self.iImmobile)
		if self.iBaseCombatStr > -1:
			unit.setBaseCombatStr(self.iBaseCombatStr)
		if self.iExtraCargo != 0:
			unit.changeCargoSpace(self.iExtraCargo)

###########
# serializes city data
class CvCityDesc:
	def __init__(self):
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
		self.plotX=-1
		self.plotY=-1

		self.sScriptData = ""
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
		self.lFreeBonus = []
		self.lNoBonus = []

	# write out city data
	def write(self, f, plot):
		city = plot.getPlotCity()
		CvUtil.pyAssert(not city.isNone(), "null city?")

		f.write("\tBeginCity\n\t\tCityOwner=%d, (%s)\n\t\tCityName=%s\n\t\tCityPopulation=%d\n\t\tStoredFood=%d\n"
			%(city.getOwner(), GC.getPlayer(city.getOwner()).getName().encode(fileencoding), city.getName().encode(fileencoding), city.getPopulation(), city.getFood())
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
			f.write("\t\tScriptData=%s\n\t\t!ScriptData\n" % city.getScriptData())

		# Player culture
		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			if GC.getPlayer(iPlayerX).isAlive():
				name = GC.getPlayer(iPlayerX).getName().encode(fileencoding)
			else: name = ""
			iPlayerCulture = city.getCulture(iPlayerX)
			if iPlayerCulture > 0:
				f.write("\t\tPlayer%dCulture=%d, (%s)\n" %(iPlayerX, iPlayerCulture, name))

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

		for i in xrange(GC.getNumBonusInfos()):
			if city.getFreeBonus(i) > 0:
				f.write("\t\tFreeBonus=%s, Amount=%s\n" %(GC.getBonusInfo(i).getType(), city.getFreeBonus(i)))
			if city.isNoBonus(i):
				f.write("\t\tNoBonus=%s\n" % GC.getBonusInfo(i).getType())

		f.write("\tEndCity\n")

	# read in city data - at this point the first line 'BeginCity' has already been read
	def read(self, f, iX, iY):
		self.__init__()
		self.plotX=iX
		self.plotY=iY
		parser = CvWBParser()
		while True:
			nextLine = f.readline()
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
			if v != -1:
				self.bldgType.append(v)
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

				self.sScriptData = v[:-1] # Don't want the newline char ath the end.
				continue

			# Player Culture
			for iPlayerX in xrange(GC.getMAX_PLAYERS()):
				v = parser.findTokenValue(toks, "Player%dCulture" % iPlayerX)
				if v != -1:
					if int(v) > 0:
						self.lCulture.append([iPlayerX, int(v)])
					break
			if v != -1:
				continue

			v = parser.findTokenValue(toks, "Damage")
			if v!=-1:
				self.iDamage = int(v)
				continue

			v = parser.findTokenValue(toks, "Occupation")
			if v!=-1:
				self.iOccupation = int(v)
				continue

			v = parser.findTokenValue(toks, "ExtraHappiness")
			if v!=-1:
				self.iExtraHappiness = int(v)
				continue

			v = parser.findTokenValue(toks, "ExtraHealth")
			if v!=-1:
				self.iExtraHealth = int(v)
				continue

			v = parser.findTokenValue(toks, "ExtraTrade")
			if v!=-1:
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

			v = parser.findTokenValue(toks, "FreeBonus")
			if v != -1:
				self.lFreeBonus.append([GC.getInfoTypeForString(v), int(parser.findTokenValue(toks, "Amount"))])
				continue

			v = parser.findTokenValue(toks, "NoBonus")
			if v != -1:
				self.lNoBonus.append(GC.getInfoTypeForString(v))
				continue

			if parser.findTokenValue(toks, "EndCity") != -1:
				break

	# after reading, this will actually apply the data
	def apply(self, bSpecial):
		city = GC.getPlayer(self.owner).initCity(self.plotX, self.plotY)

		if self.name != None:
			city.setName(self.name, False)

		if self.iPopulation:
			city.setPopulation(self.iPopulation)
		if self.iFood:
			city.setFood(self.iFood)

		for item in self.lCulture:
			city.setCulture(item[0], item[1], True)

		for key in self.bldgType:
			iBuilding = GC.getInfoTypeForString(key)
			if iBuilding > -1:
				city.setNumRealBuilding(iBuilding, 1)

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
		if bSpecial:
			city.changeExtraHappiness(self.iExtraHappiness - city.getExtraHappiness())
			city.changeExtraHealth(self.iExtraHealth - city.getExtraHealth())
			city.changeExtraTradeRoutes(self.iExtraTrade - city.getExtraTradeRoutes())
			for item in self.lBuildingYield:
				city.setBuildingYieldChange(item[0], item[1], item[2] - city.getBuildingYieldChange(item[0], item[1]))
			for item in self.lBuildingCommerce:
				city.setBuildingCommerceChange(item[0], item[1], item[2] - city.getBuildingCommerceChange(item[0], item[1]))
			for item in self.lBuildingHappy:
				city.setBuildingHappyChange(item[0], item[1] - city.getBuildingHappyChange(item[0]))
			for item in self.lBuildingHealth:
				city.setBuildingHealthChange(item[0], item[1] - city.getBuildingHealthChange(item[0]))
			for item in self.lFreeBonus:
				city.changeFreeBonus(item[0], item[1] - city.getFreeBonus(item[0]))
			for item in self.lNoBonus:
				if city.isNoBonus(item): continue
				city.changeNoBonusCount(item, 1)

##########
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

		self.sScriptData = ""
		self.abTeamPlotRevealed = []
		self.lCulture = []

	# apply plot and terrain type
	def preApply(self):
		plot = GC.getMap().plot(self.iX, self.iY)
		if self.plotType != PlotTypes.NO_PLOT:
			plot.setPlotType(self.plotType, False, False)
		if self.terrainType:
			iTerrain = GC.getInfoTypeForString(self.terrainType)
			if iTerrain > -1:
				plot.setTerrainType(iTerrain, False, False)

	def write(self, f, plot):
		"save plot desc to a file"
		f.write("BeginPlot\n")
		f.write("\tx=%d,y=%d\n" %(plot.getX(), plot.getY()))

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
		for i in xrange(plot.getNumUnits()):
			unit = plot.getUnit(i)
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

		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			if GC.getPlayer(iPlayerX).isAlive():
				name = GC.getPlayer(iPlayerX).getName().encode(fileencoding)
			else: name = ""
			iPlayerCulture = plot.getCulture(iPlayerX)
			if iPlayerCulture > 0:
				f.write("\tPlayer%dCulture=%d, (%s)\n" %(iPlayerX, iPlayerCulture, name))

		f.write("EndPlot\n")

	def read(self, f):
		"read in a plot desc"
		self.__init__()
		parser = CvWBParser()
		if parser.findNextToken(f, "BeginPlot") == False:
			return False	# no more plots
		while True:
			nextLine = f.readline()
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

				self.sScriptData = v[:-1] # Don't want the newline char ath the end.
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

			for iPlayerX in xrange(GC.getMAX_PLAYERS()):
				v = parser.findTokenValue(toks, "Player%dCulture" % iPlayerX)
				if v != -1:
					if int(v) > 0:
						self.lCulture.append([iPlayerX, int(v)])
					break
			if v != -1:
				continue

			if parser.findTokenValue(toks, "EndPlot") != -1:
				break
		return True

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

		if self.sScriptData:
			CyPlot.setScriptData(self.sScriptData)

	def applyUnits(self):
		for u in self.unitDescs:
			u.apply()

	def applyCity(self, bSpecial):
		if self.cityDesc:
			self.cityDesc.apply(bSpecial)

###############
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

		f.write("BeginMap\n\
\tgrid width=%d\n\tgrid height=%d\n\ttop latitude=%d\n\tbottom latitude=%d\n\twrap X=%d\n\twrap Y=%d\n\tworld size=%s\n\
\tclimate=%s\n\tsealevel=%s\n\tnum plots written=%d\n\tnum signs written=%d\n\tRandomize Resources=false\nEndMap\n"
%(
	iGridW, iGridH, MAP.getTopLatitude(), MAP.getBottomLatitude(), MAP.isWrapX(), MAP.isWrapY(), GC.getWorldInfo(MAP.getWorldSize()).getType(),
	GC.getClimateInfo(MAP.getClimate()).getType(), GC.getSeaLevelInfo(MAP.getSeaLevel()).getType(), (iGridW * iGridH), CyEngine().getNumSigns()
)
		)

	def read(self, f):
		print "Read Map Data"
		self.__init__()
		parser = CvWBParser()
		if parser.findNextToken(f, "BeginMap") == False:
			print "can't find map"
			return
		while True:
			nextLine = f.readline()
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

###############
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
		f.write("\tplotX=%d\n" % sign.getPlot().getX())
		f.write("\tplotY=%d\n" % sign.getPlot().getY())
		f.write("\tplayerType=%d, (%s)\n" %(sign.getPlayerType(), GC.getPlayer(sign.getPlayerType()).getName().encode(fileencoding)))
		f.write("\tcaption=%s\n" % sign.getCaption())
		f.write("EndSign\n")

	# read sign data
	def read(self, f):
		self.__init__()
		parser = CvWBParser()
		if parser.findNextToken(f, "BeginSign") == False:
			print "can't find sign"
			return
		while True:
			nextLine = f.readline()
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
		CvUtil.pyPrint('saveDesc:%s, curDir:%s' %(fileName, os.getcwd()))

		f = file(self.getDescFileName(fileName), "w") # open text file

		f.write("Version=%d\n" % version)
		self.gameDesc.write(f) # write game info

		for i in xrange(GC.getMAX_TEAMS()):
			CvTeamDesc().write(f, i) # write team info

		for i in xrange(GC.getMAX_PLAYERS()):
			CvPlayerDesc().write(f, i) # write player info

		self.mapDesc.write(f) # write map info

		MAP = GC.getMap()
		iGridW = MAP.getGridWidth()
		iGridH = MAP.getGridHeight()
		for iX in xrange(iGridW):
			for iY in xrange(iGridH):
				pDesc = CvPlotDesc()
				pDesc.write(f, MAP.plot(iX, iY))

		iNumSigns = CyEngine().getNumSigns()
		for i in xrange(iNumSigns):
			sign = CyEngine().getSignByIndex(i)
			pDesc = CvSignDesc()
			pDesc.write(f, sign)

		f.close()

		print "WBSave done\n"
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
		if str(self.mapDesc.bRandomizeResources).lower() != "false":
			for i in xrange(MAP.numPlots()):
				MAP.plotByIndex(i).setBonusType(BonusTypes.NO_BONUS)
			CyMapGenerator().addBonuses()

		print "WB apply done\n"
		return 0 # ok

	# add player starting plots if using random civs
	def getAssignedStartingPlots(self):
		MAP = GC.getMap()
		for pWBPlayer in self.playersDesc:
			pPlayer = GC.getPlayer(pWBPlayer.iPlayer)

			# Random Start Location
			if pPlayer.getLeaderType() != -1 and str(pWBPlayer.bRandomStartLocation).lower() != "false":
				pPlayer.setStartingPlot(pPlayer.findStartingPlot(True), True)

			# Player's starting plot
			elif pWBPlayer.iStartingX > -1 and pWBPlayer.iStartingY > -1:
				pPlayer.setStartingPlot(MAP.plot(pWBPlayer.iStartingX, pWBPlayer.iStartingY), True)

		return 0 # ok

	# add player objects in a last pass
	def applyInitialItems(self):
		print "Team applyInitialItems"
		MAP = GC.getMap()
		for pWBTeam in self.teamsDesc:
			team = GC.getTeam(pWBTeam.iTeam)

			for TYPE in pWBTeam.techTypes:
				team.setHasTech(GC.getInfoTypeForString(TYPE), True, -1, False, False)

			if team.isNPC(): continue

			for item in pWBTeam.bContactWithTeamList:
				team.meet(item, False)
			for item in pWBTeam.aaiEspionageAgainstTeams:
				team.setEspionagePointsAgainstTeam(item[0], item[1])
			if pWBTeam.isMinorNationCiv:
				team.setIsMinorCiv(True, False)
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
				GC.getTeam(item[0]).assignVassal(iTeamLoop, item[1])
			for TYPE in pWBTeam.projectType:
				iProject = GC.getInfoTypeForString(TYPE)
				team.changeProjectCount(iProject, 1)
				team.setProjectArtType(iProject, team.getProjectCount(iProject) - 1, 0)

		for pWBPlayer in self.playersDesc:

			player = GC.getPlayer(pWBPlayer.iPlayer)
			player.setScriptData(pWBPlayer.sScriptData)

			if player.isNPC(): continue

			player.setGold(pWBPlayer.iStartingGold)
			if pWBPlayer.iStartingX != -1 and pWBPlayer.iStartingY != -1:
				player.setStartingPlot(MAP.plot(pWBPlayer.iStartingX, pWBPlayer.iStartingY), True)

			if pWBPlayer.stateReligion:
				iStateReligionID = GC.getInfoTypeForString(pWBPlayer.stateReligion)
				player.setLastStateReligion(iStateReligionID)

			if pWBPlayer.szStartingEra:
				iStartingEra = GC.getInfoTypeForString(pWBPlayer.szStartingEra)
				player.setCurrentEra(iStartingEra)

			if str(pWBPlayer.bRandomStartLocation).lower() != "false":
				player.setStartingPlot(player.findStartingPlot(True), True)
				print("Setting player %d starting location to (%d,%d)", player.getID(), player.getStartingPlot().getX(), player.getStartingPlot().getY())

			for item in pWBPlayer.aaiCivics:
				player.setCivics(item[0],item[1])
			for item in pWBPlayer.aaiAttitudeExtras:
				player.AI_setAttitudeExtra(item[0],item[1])
			for item in pWBPlayer.aszCityList:
				player.addCityName(item)

		for item in self.plotDesc:
			item.applyCity(self.bSpecial)

		for i in xrange(GC.getMAX_PC_TEAMS()):
			pWBTeam = self.teamsDesc[i]
			iTeamX = pWBTeam.iTeam
			team = GC.getTeam(iTeamX)
			if pWBTeam.bRevealMap:
				MAP.setRevealedPlots(iTeamX, True, True)
			if pWBTeam.iVassalPower > 0:
				team.setVassalPower(pWBTeam.iVassalPower)
			if pWBTeam.iMasterPower > 0:
				team.setMasterPower(pWBTeam.iMasterPower)
			if pWBTeam.iEspionageEver > 0:
				team.setEspionagePointsEver(pWBTeam.iEspionageEver)

			if self.bSpecial:
				team.setMapCentering(pWBTeam.bMapCentering)
				team.changeMapTradingCount(pWBTeam.bMapTrading)
				team.changeTechTradingCount(pWBTeam.bTechTrading)
				team.changeGoldTradingCount(pWBTeam.bGoldTrading)
				team.changeOpenBordersTradingCount(pWBTeam.bOpenBordersTrading)
				team.changeDefensivePactTradingCount(pWBTeam.bDefensivePactTrading)
				team.changePermanentAllianceTradingCount(pWBTeam.bPermanentAllianceTrading)
				team.changeVassalTradingCount(pWBTeam.bVassalStateTrading)
				team.changeBridgeBuildingCount(pWBTeam.bBridgeBuilding)
				team.changeIrrigationCount(pWBTeam.bIrrigation)
				team.changeIgnoreIrrigationCount(pWBTeam.bIgnoreIrrigation)
				team.changeWaterWorkCount(pWBTeam.bWaterWork)
				team.changeNukeInterception(pWBTeam.iNukeInterception - team.getNukeInterception())
				team.changeEnemyWarWearinessModifier(pWBTeam.iEnemyWarWeariness- team.getEnemyWarWearinessModifier())
				for item in pWBTeam.lDomainMoves:
					team.changeExtraMoves(item[0], item[1] - team.getExtraMoves(item[0]))
				for item in pWBTeam.lRouteMoves:
					team.changeRouteChange(item[0], item[1] - team.getRouteChange(item[0]))
				for item in pWBTeam.lImprovementYield:
					team.changeImprovementYieldChange(item[0], item[1], item[2] - team.getImprovementYieldChange(item[0], item[1]))

		if self.bSpecial:
			for i in xrange(GC.getMAX_PC_PLAYERS()):
				pWBPlayer = self.playersDesc[i]
				player = GC.getPlayer(pWBPlayer.iPlayer)
				player.changeGreatGeneralsThresholdModifier(pWBPlayer.iCombatXP_ThresholdMod)
				player.setCombatExperience(pWBPlayer.iCombatXP)
				player.changeGoldenAgeTurns(pWBPlayer.iGoldenAge - player.getGoldenAgeTurns())
				player.changeAnarchyTurns(pWBPlayer.iAnarchy - player.getAnarchyTurns())
				player.changeCoastalTradeRoutes(pWBPlayer.iCoastalTradeRoute - player.getCoastalTradeRoutes())
				player.changeStateReligionUnitProductionModifier(pWBPlayer.iStateReligionUnit - player.getStateReligionUnitProductionModifier())
				player.changeStateReligionBuildingProductionModifier(pWBPlayer.iStateReligionBuilding - player.getStateReligionBuildingProductionModifier())

		for iPlotX in range(self.mapDesc.numPlotsWritten):
			pWBPlot = self.plotDesc[iPlotX]
			if pWBPlot.lCulture or pWBPlot.abTeamPlotRevealed:
				plot = MAP.plot(pWBPlot.iX, pWBPlot.iY)
				for item in pWBPlot.lCulture:
					plot.setCulture(item[0], item[1], True)
				for iTeamX in pWBPlot.abTeamPlotRevealed:
					plot.setRevealed(iTeamX, True, False, TeamTypes.NO_TEAM)

		for item in self.plotDesc:
			item.applyUnits()

		return 0

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

		self.bSpecial = 1

		f = file(fileName+ext, "r")		# open text file
		parser = CvWBParser()

		v = int(parser.findNextTokenValue(f, "Version"))
		if v != version:
			CvUtil.pyPrint("Error: wrong WorldBuilder save version.  Expected %d, got %d" %(version, v))
			return -1	# failed

		print "Reading game desc"
		self.gameDesc.read(f)	# read game info

		print "Reading teams desc"
		self.teamsDesc = []
		for i in xrange(GC.getMAX_TEAMS()):
			filePos = f.tell()
			teamsDesc = CvTeamDesc()
			if not teamsDesc.read(f, i):
				# Player not found in scenario file, backtrack file position
				f.seek(filePos)
			self.teamsDesc.append(teamsDesc)

		print "Reading players desc"
		self.playersDesc = []
		for i in xrange(GC.getMAX_PLAYERS()):
			filePos = f.tell()
			playerDesc = CvPlayerDesc()
			if not playerDesc.read(f, i):
				# Player not found in scenario file, backtrack file position
				f.seek(filePos)
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
