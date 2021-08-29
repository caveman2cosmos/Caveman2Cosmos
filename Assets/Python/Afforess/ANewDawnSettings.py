## A New Dawn Mod Option Code
from CvPythonExtensions import *
import BugCore
import CvUtil

GC = CyGlobalContext()
ANewDawnOpt = BugCore.game.RoMSettings

MODDEROPTION_EVENT_ID = CvUtil.getNewEventID()
MODDERGAMEOPTION_EVENT_ID = CvUtil.getNewEventID()
DIFFICULTY_EVENT_ID = CvUtil.getNewEventID()
COLOR_EVENT_ID = CvUtil.getNewEventID()
CANBUILD_EVENT_ID = CvUtil.getNewEventID()

class ANewDawnSettings:
	def __init__(self, eventManager):
		eventManager.addEventHandler("OnLoad", self.onLoadGame)
		eventManager.addEventHandler("GameStart", self.onGameStart)
		eventManager.addEventHandler("ModNetMessage", self.onModNetMessage)

	def onLoadGame(self,argsList):
		self.optionUpdate()

	def onGameStart(self,argsList):
		self.optionUpdate()

	def optionUpdate(self):
		if ANewDawnOpt.isRoMReset():
			resetOptions()
		else:
			setXMLOptionsfromIniFile()

	def onModNetMessage(self, argsList):
		protocol, data1, data2, data3, data4 = argsList
		#ModderOptions
		if protocol == MODDEROPTION_EVENT_ID:
			pPlayer = GC.getPlayer(data1)
			pPlayer.setModderOption(data2, data3)
		#ModderGameOptions
		elif protocol == MODDERGAMEOPTION_EVENT_ID:
			if data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_DEFENDER_WITHDRAW):
				ANewDawnOpt.setDefenderWithdraw(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_MAX_UNITS_PER_TILES):
				ANewDawnOpt.setMaxUnitsPerTile(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_MAX_BOMBARD_DEFENSE):
				ANewDawnOpt.setMaxBombardDefense(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_CANNOT_CLAIM_OCEAN):
				ANewDawnOpt.setCanNotClaimOcean(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_RESOURCE_DEPLETION):
				ANewDawnOpt.setDepletionMod(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_BETTER_INTERCETION):
				ANewDawnOpt.setBetterAirInterception(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_AIRLIFT_RANGE):
				ANewDawnOpt.setMaxRebaseRange(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_MERCY_RULE):
				ANewDawnOpt.setMercyRule(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_REALISTIC_DIPLOMACY):
				ANewDawnOpt.setRealisiticDiplomacy(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_BATTLEFIELD_PROMOTIONS):
				ANewDawnOpt.setBattlefieldPromotions(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_NO_STORMS):
				ANewDawnOpt.setNoStorms(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_IMPROVED_XP):
				ANewDawnOpt.setImprovedXP(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_MULTIPLE_RELIGION_SPREAD):
				ANewDawnOpt.setMultipleReligionSpread(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_TERRAIN_DAMAGE):
				ANewDawnOpt.setTerrainDamage(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_STRATEGIC_EVENTS):
				ANewDawnOpt.setStrategicEvents(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_NO_AUTO_CORPORATION_FOUNDING):
				ANewDawnOpt.setNoAutoCorporationFounding(data3)
			elif data2 == int(ModderGameOptionTypes.MODDERGAMEOPTION_AI_USE_FLEXIBLE_DIFFICULTY):
				ANewDawnOpt.setFlexibleDifficultyAI(data3)
		#Change Difficulty
		elif protocol == DIFFICULTY_EVENT_ID:
			pPlayer = GC.getPlayer(data1)
			pPlayer.setHandicap(data2)
		#Change Color
		elif protocol == COLOR_EVENT_ID:
			pPlayer = GC.getPlayer(data1)
			pPlayer.setColor(data2)
			GC.getMap().updateMinimapColor()
		#disabled/enable worker actions
		elif protocol == CANBUILD_EVENT_ID:
			GC.getBuildInfo(data2).setDisabled(data3)

#####################################################
# Module level functions defined in RoMSettings.xml #
#####################################################

def changedRoMReset(option, value):
	resetOptions()
	return True

#####################################################
def changedDefenderWithdraw(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_DEFENDER_WITHDRAW, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_DEFENDER_WITHDRAW), int(value), 0)

def changedMaxUnitsPerTile(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_MAX_UNITS_PER_TILES, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_MAX_UNITS_PER_TILES), int(value), 0)

def changedFlexibleDifficultyTurnIncrements(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_TURN_INCREMENTS, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_TURN_INCREMENTS), int(value), 0)

def changedMaxBombardDefense(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_MAX_BOMBARD_DEFENSE, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_MAX_BOMBARD_DEFENSE), int(value), 0)

def changedCanNotClaimOcean(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_CANNOT_CLAIM_OCEAN, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_CANNOT_CLAIM_OCEAN), int(value), 0)

def changedShowCivTraits(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_SHOW_TRAITS_FLAG, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_SHOW_TRAITS_FLAG), int(value), 0)

def changedNoFriendlyPillaging(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_NO_FRIENDLY_PILLAGING, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_NO_FRIENDLY_PILLAGING), int(value), 0)

def changedEnableFlexibleDifficulty(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY), int(value), 0)

def changedFlexibleDifficultyMinimumDiff(option, value):
	if value > 0:
		GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MIN_DIFFICULTY, value - 1)
		CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MIN_DIFFICULTY), int(value - 1), 0)
	else:
		GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MIN_DIFFICULTY, -1)
		CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MIN_DIFFICULTY), int(-1), 0)

def changedFlexibleDifficultyMaximumDiff(option, value):
	if (value > 0):
		GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MAX_DIFFICULTY, value - 1)
		CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MAX_DIFFICULTY), int(value - 1), 0)
	else:
		GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MAX_DIFFICULTY, -1)
		CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MAX_DIFFICULTY), int(-1), 0)

def changedFlexibleDifficultyMinRank(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MIN_RANK, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MIN_RANK), int(value), 0)

def changedFlexibleDifficultyMaxRank(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MAX_RANK, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_FLEXIBLE_DIFFICULTY_MAX_RANK), int(value), 0)

def changedDepletionMod(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_RESOURCE_DEPLETION, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_RESOURCE_DEPLETION), int(value), 0)

def changedBetterAirInterception(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_BETTER_INTERCETION, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_BETTER_INTERCETION), int(value), 0)

def changedMaxRebaseRange(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_AIRLIFT_RANGE, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_AIRLIFT_RANGE), int(value), 0)

def changedMercyRule(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_MERCY_RULE, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_MERCY_RULE), int(value), 0)

def changedRealisiticDiplomacy(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_REALISTIC_DIPLOMACY, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_REALISTIC_DIPLOMACY), int(value), 0)

def changedShowRevCivics(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_SHOW_REV_EFFECTS, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_SHOW_REV_EFFECTS), int(value), 0)

def changedBattlefieldPromotions(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_BATTLEFIELD_PROMOTIONS, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_BATTLEFIELD_PROMOTIONS), int(value), 0)

def changedHideReplacedBuildings(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_HIDE_REPLACED_BUILDINGS, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_HIDE_REPLACED_BUILDINGS), int(value), 0)

def changedNoStorms(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_NO_STORMS, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_NO_STORMS), int(value), 0)

def changedImprovedXP(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_IMPROVED_XP, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_IMPROVED_XP), int(value), 0)

def changedPlayerColor(option, value):
	iColor = value - 1
	if iColor >= 0:
		CyMessageControl().sendModNetMessage(COLOR_EVENT_ID, GC.getGame().getActivePlayer(), iColor, 0, 0)

def updateAliveCivsOption():
	import BugGame
	aliveCivsOption = BugGame.ANewDawnSettings.AliveCivilization
	descs = []
	for iPlayer in range(GC.getMAX_PC_PLAYERS()):
		CyPlayer = GC.getPlayer(iPlayer)
		if CyPlayer.isHuman() and CyPlayer.isAlive():
			descs.append(GC.getCivilizationInfo(CyPlayer.getCivilizationType()).getShortDescription())
	sort(descs)
	aliveCivsOption.setValues(descs)

def changedCurrentDifficulty(option, value):
	iDifficulty = value - 1
	if (iDifficulty >= 0):
		CyMessageControl().sendModNetMessage(DIFFICULTY_EVENT_ID, GC.getGame().getActivePlayer(), iDifficulty, 0, 0)

def changedUseLandmarkNames(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_USE_LANDMARK_NAMES, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_USE_LANDMARK_NAMES), int(value), 0)

def changedHideUnavailableBuilds(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_HIDE_UNAVAILBLE_BUILDS, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_HIDE_UNAVAILBLE_BUILDS), int(value), 0)

def changedEventImages(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_EVENT_IMAGES, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_EVENT_IMAGES), int(value), 0)

def changedIgnoreDisabledBuildingAlerts(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_IGNORE_DISABLED_ALERTS, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_IGNORE_DISABLED_ALERTS), int(value), 0)

def changedMultipleReligionSpread(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_MULTIPLE_RELIGION_SPREAD, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_MULTIPLE_RELIGION_SPREAD), int(value), 0)

def changedTerrainDamage(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_TERRAIN_DAMAGE, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_TERRAIN_DAMAGE), int(value), 0)

def changedStrategicEvents(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_STRATEGIC_EVENTS, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_STRATEGIC_EVENTS), int(value), 0)

def changedAllowTerraforming(option, value):
	CyMessageControl().sendModNetMessage(CANBUILD_EVENT_ID, GC.getGame().getActivePlayer(), GC.getInfoTypeForString("BUILD_TERRAFORM_GRASS"), not value, 0)
	CyMessageControl().sendModNetMessage(CANBUILD_EVENT_ID, GC.getGame().getActivePlayer(), GC.getInfoTypeForString("BUILD_TERRAFORM_PLAINS"), not value, 0)
	CyMessageControl().sendModNetMessage(CANBUILD_EVENT_ID, GC.getGame().getActivePlayer(), GC.getInfoTypeForString("BUILD_TERRAFORM_TUNDRA"), not value, 0)

def changedReforestation(option, value):
	CyMessageControl().sendModNetMessage(CANBUILD_EVENT_ID, GC.getGame().getActivePlayer(), GC.getInfoTypeForString("BUILD_YOUNG_FOREST"), not value, 0)
	CyMessageControl().sendModNetMessage(CANBUILD_EVENT_ID, GC.getGame().getActivePlayer(), GC.getInfoTypeForString("BUILD_PLANT_JUNGLE"), not value, 0)

def changedSeaTunnels(option, value):
	CyMessageControl().sendModNetMessage(CANBUILD_EVENT_ID, GC.getGame().getActivePlayer(), GC.getInfoTypeForString("BUILD_TUNNEL"), not value, 0)

def changedNoAutoCorporationFounding(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_NO_AUTO_CORPORATION_FOUNDING, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_NO_AUTO_CORPORATION_FOUNDING), int(value), 0)

def changedWarPrizes(option, value):
	GC.setDefineINT("WAR_PRIZES", int(value))

def changedFlexibleDifficultyAI(option, value):
	GC.getGame().setModderGameOption(ModderGameOptionTypes.MODDERGAMEOPTION_AI_USE_FLEXIBLE_DIFFICULTY, value)
	CyMessageControl().sendModNetMessage(MODDERGAMEOPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderGameOptionTypes.MODDERGAMEOPTION_AI_USE_FLEXIBLE_DIFFICULTY), int(value), 0)

def changedInfrastructureIgnoresImprovements(option, value):
	GC.getActivePlayer().setModderOption(ModderOptionTypes.MODDEROPTION_INFRASTRUCTURE_IGNORES_IMPROVEMENTS, value)
	CyMessageControl().sendModNetMessage(MODDEROPTION_EVENT_ID, GC.getGame().getActivePlayer(), int(ModderOptionTypes.MODDEROPTION_INFRASTRUCTURE_IGNORES_IMPROVEMENTS), int(value), 0)


def setXMLOptionsfromIniFile():
	print "Initializing A New Dawn Settings"
	changedDefenderWithdraw(ANewDawnOpt, ANewDawnOpt.isDefenderWithdraw())
	changedMaxUnitsPerTile(ANewDawnOpt, ANewDawnOpt.getMaxUnitsPerTile())
	changedFlexibleDifficultyTurnIncrements(ANewDawnOpt, ANewDawnOpt.getFlexibleDifficultyTurnIncrements())
	changedMaxBombardDefense(ANewDawnOpt, ANewDawnOpt.getMaxBombardDefense())
	changedCanNotClaimOcean(ANewDawnOpt, ANewDawnOpt.isCanNotClaimOcean())
	changedShowCivTraits(ANewDawnOpt, ANewDawnOpt.isShowCivTraits())
	changedNoFriendlyPillaging(ANewDawnOpt, ANewDawnOpt.isNoFriendlyPillaging())
	changedEnableFlexibleDifficulty(ANewDawnOpt, ANewDawnOpt.isEnableFlexibleDifficulty())
	changedFlexibleDifficultyMinimumDiff(ANewDawnOpt, ANewDawnOpt.getFlexibleDifficultyMinimumDiff() - 1)
	changedFlexibleDifficultyMaximumDiff(ANewDawnOpt, ANewDawnOpt.getFlexibleDifficultyMaximumDiff() - 1)
	changedFlexibleDifficultyMinRank(ANewDawnOpt, ANewDawnOpt.getFlexibleDifficultyMinRank())
	changedFlexibleDifficultyMaxRank(ANewDawnOpt, ANewDawnOpt.getFlexibleDifficultyMaxRank())
	changedDepletionMod(ANewDawnOpt, ANewDawnOpt.isDepletionMod())
	changedBetterAirInterception(ANewDawnOpt, ANewDawnOpt.isBetterAirInterception())
	changedMaxRebaseRange(ANewDawnOpt, ANewDawnOpt.getMaxRebaseRange())
	changedMercyRule(ANewDawnOpt, ANewDawnOpt.isMercyRule())
	changedRealisiticDiplomacy(ANewDawnOpt, ANewDawnOpt.isRealisiticDiplomacy())
	changedShowRevCivics(ANewDawnOpt, ANewDawnOpt.isShowRevCivics())
	changedBattlefieldPromotions(ANewDawnOpt, ANewDawnOpt.isBattlefieldPromotions())
	changedHideReplacedBuildings(ANewDawnOpt, ANewDawnOpt.isHideReplacedBuildings())
	changedNoStorms(ANewDawnOpt, ANewDawnOpt.isNoStorms())
	changedImprovedXP(ANewDawnOpt, ANewDawnOpt.isImprovedXP())
	changedUseLandmarkNames(ANewDawnOpt, ANewDawnOpt.isUseLandmarkNames())
	changedHideUnavailableBuilds(ANewDawnOpt, ANewDawnOpt.isHideUnavailableBuilds())
	changedEventImages(ANewDawnOpt, ANewDawnOpt.isEventImages())
	changedIgnoreDisabledBuildingAlerts(ANewDawnOpt, ANewDawnOpt.isIgnoreDisabledBuildingAlerts())
	changedMultipleReligionSpread(ANewDawnOpt, ANewDawnOpt.isMultipleReligionSpread())
	changedTerrainDamage(ANewDawnOpt, ANewDawnOpt.isTerrainDamage())
	changedStrategicEvents(ANewDawnOpt, ANewDawnOpt.isStrategicEvents())
	changedAllowTerraforming(ANewDawnOpt, ANewDawnOpt.isAllowTerraforming())
	changedReforestation(ANewDawnOpt, ANewDawnOpt.isReforestation())
	changedSeaTunnels(ANewDawnOpt, ANewDawnOpt.isSeaTunnels())
	changedWarPrizes(ANewDawnOpt, ANewDawnOpt.isWarPrizes())
	changedFlexibleDifficultyAI(ANewDawnOpt, ANewDawnOpt.isFlexibleDifficultyAI())
	changedInfrastructureIgnoresImprovements(ANewDawnOpt, ANewDawnOpt.isInfrastructureIgnoresImprovements())

	ANewDawnOpt.setPlayerColor(0)
	ANewDawnOpt.setCurrentDifficulty(0)

def resetOptions():
	import BugOptions
	ANewDawnoptions = BugOptions.getOptions("RoMSettings").options
	for i in range(len(ANewDawnoptions)):
		ANewDawnoptions[i].resetValue()
	setXMLOptionsfromIniFile()
	ANewDawnOpt.setRoMReset(False)