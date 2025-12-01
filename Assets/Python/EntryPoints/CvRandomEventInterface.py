# Sid Meier's Civilization 4
# Copyright Firaxis Games 2005
#
# CvRandomEventInterface.py
#
# These functions are App Entry Points from C++
# WARNING: These function names should not be changed
# WARNING: These functions can not be placed into a class
#
# No other modules should import this
#
import CvUtil
import BugUtil
from CvPythonExtensions import *
from operator import itemgetter

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()


def isLocalHumanPlayer(iPlayer):
	CyPlayer = GC.getPlayer(iPlayer)
	return (CyPlayer.isHuman() or CyPlayer.isHumanDisabled()) and GAME.getActivePlayer() == iPlayer and not GAME.getAIAutoPlay(iPlayer)


def recalculateModifiers():
	eInterstateEvent = GC.getInfoTypeForString("EVENT_INTERSTATE_1")
	for i in range(GC.getMAX_PLAYERS()):
		pInterstateEventTriggerData = GC.getPlayer(i).getEventOccured(eInterstateEvent)
		if pInterstateEventTriggerData is not None:
			applyInterstate((eInterstateEvent, pInterstateEventTriggerData))


######## BLESSED SEA ###########

def getHelpBlessedSea1(argsList):
	iOurMinLandmass = (3 * GC.getWorldInfo(CyMap().getWorldSize()).getDefaultPlayers()) / 2
	return TRNSLTR.getText("TXT_KEY_EVENT_BLESSED_SEA_HELP", (iOurMinLandmass, ))

def canTriggerBlessedSea(argsList):
	data = argsList[0]
	MAP = GC.getMap()

	if GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY):
		return False

	iMapMinLandmass = 2 * GC.getWorldInfo(MAP.getWorldSize()).getDefaultPlayers()
	iOurMaxLandmass = iMapMinLandmass / 4

	if (MAP.getNumLandAreas() < iMapMinLandmass):
		return False

	iOurLandmasses = 0
	for area in MAP.areas():
		if not area.isWater() and area.getCitiesPerPlayer(data.ePlayer) > 0:
			iOurLandmasses += 1

	if (iOurLandmasses > iOurMaxLandmass):
		return False

	player = GC.getPlayer(data.ePlayer)
	if not(player.getUnitCount(GC.getInfoTypeForString("UNIT_GALLEY"))
		or player.getUnitCount(GC.getInfoTypeForString("UNIT_CARAVEL"))
		or player.getUnitCount(GC.getInfoTypeForString("UNIT_GALLEON"))
		or player.getUnitCount(GC.getInfoTypeForString("UNIT_WARGALLEY"))
		or player.getUnitCount(GC.getInfoTypeForString("UNIT_FLUYT"))
		or player.getUnitCount(GC.getInfoTypeForString("UNIT_BRIGANTINE"))
	): return False
	return True

def canTriggerBlessedSea2(argsList):
  data = argsList[0]
  map = GC.getMap()
  iOurMinLandmass = (3 * GC.getWorldInfo(map.getWorldSize()).getDefaultPlayers()) / 2

  iOurLandmasses = 0
  for area in map.areas():
    if not area.isWater() and area.getCitiesPerPlayer(data.ePlayer) > 0:
      iOurLandmasses += 1

  if (iOurLandmasses < iOurMinLandmass):
    return False

  return True

def applyBlessedSea2(argsList):
	data = argsList[1]

	iBuilding = -1

	if -1 != data.eReligion:
		for i in xrange(GC.getNumBuildingInfos()):
			if GC.getBuildingInfo(i).getSpecialBuildingType() == GC.getInfoTypeForString("SPECIALBUILDING_TEMPLE"):
				if GC.getBuildingInfo(i).getReligionType() == data.eReligion:
					iBuilding = i
					break

	if (iBuilding == -1):
		return

	for loopCity in GC.getPlayer(data.ePlayer).cities():
		if loopCity.getPopulation() >= 5:
			if loopCity.canConstruct(iBuilding, False, False, True):
				loopCity.changeHasBuilding(iBuilding, True)


def canApplyBlessedSea2(argsList):
	data = argsList[1]

	iBuilding = -1
	if -1 != data.eReligion:
		for i in xrange(GC.getNumBuildingInfos()):
			if GC.getBuildingInfo(i).getSpecialBuildingType() == GC.getInfoTypeForString("SPECIALBUILDING_TEMPLE"):
				if GC.getBuildingInfo(i).getReligionType() == data.eReligion:
					iBuilding = i
					break
	if (iBuilding == -1):
		return False

	for loopCity in GC.getPlayer(data.ePlayer).cities():
		if loopCity.getPopulation() >= 5:
			if loopCity.canConstruct(iBuilding, False, False, True):
				return True
	return False


######## HOLY MOUNTAIN ###########

def getHelpHolyMountain1(argsList):
	data = argsList[1]
	szHelp = ""

	iReligion = GC.getPlayer(data.ePlayer).getStateReligion()

	if iReligion != -1:
		iBuilding = -1
		for i in xrange(GC.getNumBuildingInfos()):
			if((GC.getBuildingInfo(i).getSpecialBuildingType() == GC.getInfoTypeForString("SPECIALBUILDING_CATHEDRAL")
			or  GC.getBuildingInfo(i).getSpecialBuildingType() == GC.getInfoTypeForString("SPECIALBUILDING_CATHEDRAL_II")
			or  GC.getBuildingInfo(i).getSpecialBuildingType() == GC.getInfoTypeForString("SPECIALBUILDING_PANTHEON")
			)
			and GC.getBuildingInfo(i).getReligionType() == iReligion
			):
				iBuilding = i
				break

		iMinPoints = 2 * GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
		szHelp = TRNSLTR.getText("TXT_KEY_EVENT_HOLY_MOUNTAIN_PART_1_HELP", (iMinPoints, ))
		if iBuilding != -1:
			szHelp += TRNSLTR.getText("TXT_KEY_EVENT_HOLY_MOUNTAIN_PART_2_HELP", ( GC.getBuildingInfo(iBuilding).getTextKey(), ))
		szHelp += TRNSLTR.getText("TXT_KEY_EVENT_HOLY_MOUNTAIN_PART_3_HELP", ())

	return szHelp

def canTriggerHolyMountain(argsList):
	data = argsList[0]

	if GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY):
		return False

	CyPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)
	if CyPlot.getOwner() != -1:
		return False

	return True

def expireHolyMountain1(argsList):
	data = argsList[1]
	CyPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)
	if CyPlot.getOwner() not in (-1, data.ePlayer):
		return True
	return False

def canTriggerHolyMountainDone(argsList):
	data = argsList[0]

	CyPlayer = GC.getPlayer(data.ePlayer)
	dataOriginal = CyPlayer.getEventOccured(GC.getEventTriggerInfo(data.eTrigger).getPrereqEvent(0))

	if not dataOriginal: return False

	CyPlot = GC.getMap().plot(dataOriginal.iPlotX, dataOriginal.iPlotY)

	if CyPlot.getOwner() != data.ePlayer:
		return False

	# argsList[0] is just a shallow copy of the real EventTriggeredData object.
	EventTriggeredData = CyPlayer.getEventTriggered(data.iId)
	EventTriggeredData.iPlotX = dataOriginal.iPlotX
	EventTriggeredData.iPlotY = dataOriginal.iPlotY
	return True

def canTriggerHolyMountainRevealed(argsList):
	data = argsList[0]

	CyPlayer = GC.getPlayer(data.ePlayer)
	if not CyPlayer.isHuman(): return False

	dataOriginal = CyPlayer.getEventOccured(GC.getEventTriggerInfo(data.eTrigger).getPrereqEvent(0))
	if not dataOriginal: return False

	iPoints = 0
	for i in xrange(GC.getNumBuildingInfos()):
		CvBuildingInfo = GC.getBuildingInfo(i)

		if CvBuildingInfo.getReligionType() == dataOriginal.eReligion:

			iSpecialBuilding = CvBuildingInfo.getSpecialBuildingType()

			if iSpecialBuilding in (
				GC.getInfoTypeForString("SPECIALBUILDING_CATHEDRAL"),
				GC.getInfoTypeForString("SPECIALBUILDING_CATHEDRAL_II"),
				GC.getInfoTypeForString("SPECIALBUILDING_PANTHEON")
			):
				iPoints += 4 * CyPlayer.countNumBuildings(i)

			elif iSpecialBuilding in (
				GC.getInfoTypeForString("SPECIALBUILDING_TEMPLE"),
				GC.getInfoTypeForString("SPECIALBUILDING_MONASTERY")
			):
				iPoints += CyPlayer.countNumBuildings(i)

	MAP = GC.getMap()
	if iPoints < 2 * GC.getWorldInfo(MAP.getWorldSize()).getDefaultPlayers():
		return False
	# No reason not to do this here when we already have the player and map objects.
	MAP.plot(dataOriginal.iPlotX, dataOriginal.iPlotY).setRevealed(CyPlayer.getTeam(), True, True, -1)

	# argsList[0] is just a shallow copy of the real EventTriggeredData object.
	EventTriggeredData = CyPlayer.getEventTriggered(data.iId)
	EventTriggeredData.iPlotX = dataOriginal.iPlotX
	EventTriggeredData.iPlotY = dataOriginal.iPlotY

	return True

def doHolyMountainRevealed(argsList):
	data = argsList[1]
	if data.ePlayer == GAME.getActivePlayer():
		CyCamera().JustLookAtPlot(GC.getMap().plot(data.iPlotX, data.iPlotY))
	return 1

######## MARATHON ###########

def canTriggerMarathon(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)
	otherPlayer = GC.getPlayer(data.eOtherPlayer)

	if GC.getTeam(player.getTeam()).AI_getAtWarCounter(otherPlayer.getTeam()) == 1:
		for loopUnit in otherPlayer.units():
			plot = loopUnit.plot()
			if plot and plot.getOwner() == data.ePlayer:
				return True

	return False

######## WEDDING FEUD ###########

def doWeddingFeud2(argsList):
	data = argsList[1]
	for loopCity in GC.getPlayer(data.ePlayer).cities():
		if loopCity.isHasReligion(data.eReligion):
			loopCity.changeHappinessTimer(30)
	return 1

def getHelpWeddingFeud2(argsList):
	data = argsList[1]
	religion = GC.getReligionInfo(data.eReligion)
	return TRNSLTR.getText("TXT_KEY_EVENT_WEDDING_FEUD_2_HELP", (GC.getDefineINT("TEMP_HAPPY"), 30, religion.getChar()))

def canDoWeddingFeud3(argsList):
	data = argsList[1]
	player = GC.getPlayer(data.ePlayer)
	if player.getGold() - 10 * player.getNumCities() < 0:
		return False

	# do not use this event in multiplayer
	if GAME.isNetworkMultiPlayer():
		return False

	return True


def doWeddingFeud3(argsList):
  data = argsList[1]

  destPlayer = GC.getPlayer(data.eOtherPlayer)
  player = GC.getPlayer(data.ePlayer)

  for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iLoopPlayer)
    if loopPlayer.isAlive() and loopPlayer.getStateReligion() == player.getStateReligion():
      loopPlayer.AI_changeAttitudeExtra(data.ePlayer, 1)
      player.AI_changeAttitudeExtra(iLoopPlayer, 1)

  if GC.getTeam(destPlayer.getTeam()).canDeclareWar(player.getTeam()):
    if destPlayer.isHuman():
      # this works only because it's a single-player only event
      popupInfo = CyPopupInfo()
      popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
      popupInfo.setText(TRNSLTR.getText("TXT_KEY_EVENT_WEDDING_FEUD_OTHER_3", (GC.getReligionInfo(data.eReligion).getAdjectiveKey(), player.getCivilizationShortDescriptionKey())))
      popupInfo.setData1(data.eOtherPlayer)
      popupInfo.setData2(data.ePlayer)
      popupInfo.setPythonModule("CvRandomEventInterface")
      popupInfo.setOnClickedPythonCallback("weddingFeud3Callback")
      popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_YES", ()), "")
      popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_NO", ()), "")
      popupInfo.addPopup(data.eOtherPlayer)
    else:
      GC.getTeam(destPlayer.getTeam()).declareWar(player.getTeam(), False, WarPlanTypes.WARPLAN_LIMITED)

  return 1


def weddingFeud3Callback(argsList):
  iButton = argsList[0]
  iData1 = argsList[1]
  iData2 = argsList[2]
  iData3 = argsList[3]
  szText = argsList[4]
  bOption1 = argsList[5]
  bOption2 = argsList[6]

  if iButton == 0:
    destPlayer = GC.getPlayer(iData1)
    player = GC.getPlayer(iData2)
    GC.getTeam(destPlayer.getTeam()).declareWar(player.getTeam(), False, WarPlanTypes.WARPLAN_LIMITED)

  return 0

def getHelpWeddingFeud3(argsList):
	data = argsList[1]
	return TRNSLTR.getText("TXT_KEY_EVENT_WEDDING_FEUD_3_HELP", (1, GC.getReligionInfo(data.eReligion).getChar()))

######## SPICY ###########

def canTriggerSpicy(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)

	iSpice = GC.getInfoTypeForString("BONUS_SPICES")

	plot = GC.getMap().plot(data.iPlotX, data.iPlotY)
	if not plot:
		try:
			raise "ERROR in canTriggerSpicy from bad trigger definition"
		except:
			return False

	if not plot.canHaveBonus(iSpice, False):
		return False

	return True

def doSpicy2(argsList):
	# need this because plantations are normally not allowed unless there are already spices
	GC.getMap().plot(argsList[1].iPlotX, argsList[1].iPlotY).setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PLANTATION"))
	return 1

def getHelpSpicy2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_IMPROVEMENT_GROWTH", (GC.getImprovementInfo(GC.getInfoTypeForString("IMPROVEMENT_PLANTATION")).getTextKey(), ))

######## BABY BOOM ###########

def canTriggerBabyBoom(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)
  team = GC.getTeam(player.getTeam())

  if team.isAtWar(False):
    return False

  for iLoopTeam in xrange(GC.getMAX_PC_TEAMS()):
    if iLoopTeam != player.getTeam():
      if team.AI_getAtPeaceCounter(iLoopTeam) == 1:
        return True

  return False

######## BARD TALE ###########

def applyBardTale3(argsList):
	data = argsList[1]
	player = GC.getPlayer(data.ePlayer)
	player.changeGold(-50 * player.getNumCities())

def canApplyBardTale3(argsList):
	data = argsList[1]
	player = GC.getPlayer(data.ePlayer)
	if player.getGold() - 50 * player.getNumCities() < 0:
		return False
	return True


def getHelpBardTale3(argsList):
	data = argsList[1]
	player = GC.getPlayer(data.ePlayer)
	return TRNSLTR.getText("TXT_KEY_EVENT_GOLD_LOST", (50 * player.getNumCities(), ))

######## LOOTERS ###########

def getHelpLooters3(argsList):
	data = argsList[1]
	city = GC.getPlayer(data.eOtherPlayer).getCity(data.iOtherPlayerCityId)
	return TRNSLTR.getText("TXT_KEY_EVENT_LOOTERS_3_HELP", (1, 2, city.getNameKey()))


def canApplyLooters3(argsList):
	data = argsList[1]

	CyPlayer = GC.getPlayer(data.eOtherPlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(data.iOtherPlayerCityId)
	iEra = CyPlayer.getCurrentEra()
	iTreshold = (100 + 20 * iEra * iEra) * GC.getGameSpeedInfo(GAME.getGameSpeedType()).getHammerCostPercent() / 100

	for i in xrange(GC.getNumBuildingInfos()):
		if isLimitedWonder(i) or not CyCity.hasBuilding(i):
			continue
		info = GC.getBuildingInfo(i)
		if info.isAutoBuild():
			continue
		iCost = info.getProductionCost()
		if iCost <= iTreshold and iCost > 0:
			return True

	return False


def applyLooters3(argsList):
	data = argsList[1]

	CyPlayer = GC.getPlayer(data.eOtherPlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(data.iOtherPlayerCityId)
	iEra = CyPlayer.getCurrentEra()
	iTreshold = (100 + 20 * iEra * iEra) * GC.getGameSpeedInfo(GAME.getGameSpeedType()).getHammerCostPercent() / 100

	aList = []
	for i in xrange(GC.getNumBuildingInfos()):
		if isLimitedWonder(i) or not CyCity.hasBuilding(i): continue
		info = GC.getBuildingInfo(i)
		if info.isAutoBuild():
			continue
		iCost = info.getProductionCost()
		if iCost <= iTreshold and iCost > 0:
			aList.append(i)

	iMax = GAME.getSorenRandNum(2, "Looters event number of buildings destroyed") + 1
	iCount = 0
	while aList and iCount < iMax:
		iBuilding = aList[GAME.getSorenRandNum(len(aList), "Looters event building destroyed")]
		szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBuilding).getTextKey(), ))
		if isLocalHumanPlayer(data.eOtherPlayer) :
			CyInterface().addMessage(data.eOtherPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBuilding).getButton(), GC.getCOLOR_RED(), CyCity.getX(), CyCity.getY(), True, True)
		CyCity.changeHasBuilding(iBuilding, False)
		aList.remove(iBuilding)
		iCount += 1

	szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_NUM_BUILDINGS_DESTROYED", (iCount, GC.getPlayer(data.eOtherPlayer).getCivilizationAdjectiveKey(), CyCity.getNameKey()))
	if isLocalHumanPlayer(data.ePlayer) :
		CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), -1, -1, True, True)

######## BROTHERS IN NEED ###########

def canTriggerBrothersInNeed(argsList):
  data = argsList[0]
  trigger = GC.getEventTriggerInfo(data.eTrigger)
  player = GC.getPlayer(data.ePlayer)
  otherPlayer = GC.getPlayer(data.eOtherPlayer)

  if not player.canTradeNetworkWith(data.eOtherPlayer):
    return False

  listResources = []
  listResources.append(GC.getInfoTypeForString("BONUS_COPPER_ORE"))
  listResources.append(GC.getInfoTypeForString("BONUS_OBSIDIAN"))
  listResources.append(GC.getInfoTypeForString("BONUS_STONE"))
  listResources.append(GC.getInfoTypeForString("BONUS_IRON_ORE"))
  listResources.append(GC.getInfoTypeForString("BONUS_HORSE"))
  listResources.append(GC.getInfoTypeForString("BONUS_ELEPHANTS"))
  listResources.append(GC.getInfoTypeForString("BONUS_OIL"))
  listResources.append(GC.getInfoTypeForString("BONUS_URANIUM"))

  bFound = False
  for iResource in listResources:
    if (player.getNumTradeableBonuses(iResource) > 1 and otherPlayer.getNumAvailableBonuses(iResource) <= 0):
      bFound = True
      break

  if not bFound:
    return False

  for iTeam in xrange(GC.getMAX_PC_TEAMS()):
    if iTeam != player.getTeam() and iTeam != otherPlayer.getTeam() and GC.getTeam(iTeam).isAlive():
      if GC.getTeam(iTeam).isAtWarWith(otherPlayer.getTeam()) and not GC.getTeam(iTeam).isAtWarWith(player.getTeam()):
        return True

  return False

def canDoBrothersInNeed1(argsList):
  data = argsList[1]
  newArgs = (data, )

  return canTriggerBrothersInNeed(newArgs)

######## HURRICANE ###########

def canTriggerHurricaneCity(argsList):
	iPlayer = argsList[1]
	iCity = argsList[2]
	CyCity = GC.getPlayer(iPlayer).getCity(iCity)

	if CyCity is None:
		return False

	if not CyCity.isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()):
		return False

	iLat = CyCity.plot().getLatitude()
	if iLat < 0 or iLat > 30: return False

	if CyCity.getPopulation() < 2: return False
	return True

def canApplyHurricane1(argsList):
	data = argsList[1]

	CyPlayer = GC.getPlayer(data.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(data.iCityId)

	for i in xrange(GC.getNumBuildingInfos()):
		if isLimitedWonder(i) or not CyCity.hasBuilding(i) or CyCity.isFreeBuilding(i):
			continue
		info = GC.getBuildingInfo(i)
		if info.isNukeImmune() or info.isAutoBuild() or info.getProductionCost() < 1:
			continue
		return True
	return False

def canApplyHurricane2(argsList):
	return not canApplyHurricane1(argsList)


def applyHurricane1(argsList):
	data = argsList[1]

	CyPlayer = GC.getPlayer(data.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(data.iCityId)

	aList = []
	for i in xrange(GC.getNumBuildingInfos()):
		if isLimitedWonder(i) or not CyCity.hasBuilding(i) or CyCity.isFreeBuilding(i):
			continue
		info = GC.getBuildingInfo(i)
		if info.isNukeImmune() or info.isAutoBuild() or info.getProductionCost() < 1:
			continue
		aList.append(i)

	if aList:
		iBuilding = aList[GAME.getSorenRandNum(len(aList), "Hurricane")]
		szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBuilding).getTextKey(), ))
		if isLocalHumanPlayer(data.ePlayer) :
			CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBuilding).getButton(), GC.getCOLOR_RED(), CyCity.getX(), CyCity.getY(), True, True)
		CyCity.changeHasBuilding(iBuilding, False)


######## CYCLONE ###########

def canTriggerCycloneCity(argsList):
	iPlayer = argsList[1]
	iCity = argsList[2]
	CyCity = GC.getPlayer(iPlayer).getCity(iCity)

	if CyCity is None:
		return False

	if not CyCity.isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()):
		return False

	iLat = CyCity.plot().getLatitude()
	if iLat >= 0 or iLat < -30: return False

	if CyCity.getPopulation() < 2: return False
	return True

######## TSUNAMI ###########

def canTriggerTsunamiCity(argsList):
  ePlayer = argsList[1]
  iCity = argsList[2]
  city = GC.getPlayer(ePlayer).getCity(iCity)
  if city is None or not city.isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()):
    return False
  return True

def canApplyTsunami1(argsList):
	data = argsList[1]
	return GC.getPlayer(data.ePlayer).getCity(data.iCityId).getPopulation() < 6

def canApplyTsunami2(argsList):
  return not canApplyTsunami1(argsList)


def applyTsunami1(argsList):
	GC.getPlayer(argsList[1].ePlayer).getCity(argsList[1].iCityId).kill()


def applyTsunami2(argsList):
	data = argsList[1]

	CyPlayer = GC.getPlayer(data.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(data.iCityId)

	listBuildings = []
	for i in xrange(GC.getNumBuildingInfos()):
		if isLimitedWonder(i) or not CyCity.hasBuilding(i):
			continue
		info = GC.getBuildingInfo(i)
		if info.getProductionCost() > 0 and not info.isAutoBuild():
			listBuildings.append(i)

	for i in xrange(5):
		if len(listBuildings) > 0:
			iBuilding = listBuildings[GAME.getSorenRandNum(len(listBuildings), "Tsunami event building destroyed")]
			szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBuilding).getTextKey(), ))
			if isLocalHumanPlayer(data.ePlayer) :
				CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBuilding).getButton(), GC.getCOLOR_RED(), CyCity.getX(), CyCity.getY(), True, True)
			CyCity.changeHasBuilding(iBuilding, False)
			listBuildings.remove(iBuilding)


def getHelpTsunami2(argsList):
	data = argsList[1]
	city = GC.getPlayer(data.ePlayer).getCity(data.iCityId)
	return TRNSLTR.getText("TXT_KEY_EVENT_TSUNAMI_2_HELP", (5, city.getNameKey()))


######## MONSOON ###########

def canTriggerMonsoonCity(argsList):
  ePlayer = argsList[1]
  iCity = argsList[2]

  city = GC.getPlayer(ePlayer).getCity(iCity)

  if city is None or city.isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()):
    return False

  iJungleType = GC.getFEATURE_JUNGLE()

  for iDX in xrange(-3, 4):
    for iDY in xrange(-3, 4):
      pLoopPlot = plotXY(city.getX(), city.getY(), iDX, iDY)
      if pLoopPlot and pLoopPlot.getFeatureType() == iJungleType:
        return True

  return False

######## DUSTBOWL ###########

def canTriggerDustbowlCont(argsList):
  data = argsList[0]

  trigger = GC.getEventTriggerInfo(data.eTrigger)
  player = GC.getPlayer(data.ePlayer)

  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))

  if (kOrigTriggeredData == None):
    return False

  iFarmType = GC.getInfoTypeForString("IMPROVEMENT_FARM")
  iPlainsType = GC.getInfoTypeForString("TERRAIN_PLAINS")

  map = GC.getMap()
  iBestValue = map.getGridWidth() + map.getGridHeight()
  bestPlot = None
  for plot in map.plots():
    if (plot.getOwner() == data.ePlayer and plot.getImprovementType() == iFarmType and plot.getTerrainType() == iPlainsType):
      iValue = plotDistance(kOrigTriggeredData.iPlotX, kOrigTriggeredData.iPlotY, plot.getX(), plot.getY())
      if iValue < iBestValue:
        iBestValue = iValue
        bestPlot = plot

  if bestPlot != None:
    kActualTriggeredDataObject = player.getEventTriggered(data.iId)
    kActualTriggeredDataObject.iPlotX = bestPlot.getX()
    kActualTriggeredDataObject.iPlotY = bestPlot.getY()
  else:
    player.resetEventOccured(trigger.getPrereqEvent(0))
    return False

  return True

def getHelpDustBowl2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_DUSTBOWL_2_HELP", ())

######## SALTPETER ###########

def getSaltpeterNumExtraPlots():
  map = GC.getMap()
  if map.getWorldSize() <= 1:
    return 1
  elif map.getWorldSize() <= 3:
    return 2
  elif map.getWorldSize() <= 4:
    return 3
  else:
    return 4

def getHelpSaltpeter(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_SALTPETER_HELP", (getSaltpeterNumExtraPlots(), ))

def canApplySaltpeter(argsList):
	data = argsList[1]
	map = GC.getMap()
	plot = map.plot(data.iPlotX, data.iPlotY)
	if plot is None:
		return False

	iForest = GC.getFEATURE_FOREST()

	iNumPlots = 0
	for loopPlot in map.plots():
		if loopPlot.getOwner() == data.ePlayer and loopPlot.getFeatureType() == iForest and loopPlot.isHills():
			iDistance = plotDistance(data.iPlotX, data.iPlotY, loopPlot.getX(), loopPlot.getY())
			if iDistance > 0:
				iNumPlots += 1

	return iNumPlots >= getSaltpeterNumExtraPlots()

def applySaltpeter(argsList):
	data = argsList[1]
	map = GC.getMap()
	plot = map.plot(data.iPlotX, data.iPlotY)
	if plot is None:
		return
	iForest = GC.getFEATURE_FOREST()

	listPlots = []
	for loopPlot in map.plots():
		if loopPlot.getOwner() == data.ePlayer and loopPlot.getFeatureType() == iForest and loopPlot.isHills():
			iDistance = plotDistance(data.iPlotX, data.iPlotY, loopPlot.getX(), loopPlot.getY())
			if iDistance > 0:
				listPlots.append((iDistance, loopPlot))

	listPlots.sort(key=itemgetter(0))

	iCount = getSaltpeterNumExtraPlots()
	for loopPlot in listPlots:
		if iCount == 0:
			break
		iCount -= 1
		loopPlot[1].setExtraYield(YieldTypes.YIELD_COMMERCE, 2)
		CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), TRNSLTR.getText("TXT_KEY_EVENT_SALTPETER_DISCOVERED", ()), "", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), loopPlot[1].getX(), loopPlot[1].getY(), True, True)

######## GREAT DEPRESSION ###########

def applyGreatDepression(argsList):
  data = argsList[1]

  player = GC.getPlayer(data.ePlayer)
  corporation = GC.getCorporationInfo(data.eCorporation)

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      loopPlayer.changeGold(-loopPlayer.getGold()/4)

      if iPlayer != data.ePlayer:
        szText = TRNSLTR.getText("TXT_KEY_EVENTTRIGGER_GREAT_DEPRESSION", (player.getCivilizationAdjectiveKey(), u"", u"", u"", u"", corporation.getTextKey()))
        szText += u"\n\n" + TRNSLTR.getText("TXT_KEY_EVENT_GREAT_DEPRESSION_HELP", (25, ))
        popupInfo = CyPopupInfo()
        popupInfo.setText(szText)
        popupInfo.addPopup(iPlayer)


def getHelpGreatDepression(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_GREAT_DEPRESSION_HELP", (25, ))

######## CHAMPION ###########

def canTriggerChampion(argsList):
	return not GC.getTeam(GC.getPlayer(argsList[0].ePlayer).getTeam()).isAtWar(False)

def canTriggerChampionUnit(argsList):
	unit = GC.getPlayer(argsList[1]).getUnit(argsList[2])

	if not unit or not unit.canFight():
		return False

	if unit.getDamage() > 0 or unit.getLevel() < 5:
		return False

	if unit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_LEADERSHIP")):
		return False

	return True


def applyChampion(argsList):
	data = argsList[1]
	GC.getPlayer(data.ePlayer).getUnit(data.iUnitId).setHasPromotion(GC.getInfoTypeForString("PROMOTION_LEADERSHIP"), True)

def getHelpChampion(argsList):
	data = argsList[1]
	unit = GC.getPlayer(data.ePlayer).getUnit(data.iUnitId)
	iLeadership = GC.getInfoTypeForString("PROMOTION_LEADERSHIP")
	return TRNSLTR.getText("TXT_KEY_EVENT_CHAMPION_HELP", (unit.getNameKey(), GC.getPromotionInfo(iLeadership).getTextKey()))

######## ELECTRIC COMPANY ###########

def canTriggerElectricCompany(argsList):
	data = argsList[0]
	for loopCity in GC.getPlayer(data.ePlayer).cities():
		if loopCity.angryPopulation(0) > 0:
			return False
	return True

######## GOLD RUSH ###########

def canTriggerGoldRush(argsList):
	data = argsList[0]
	return GC.getPlayer(data.ePlayer).getCurrentEra() == GC.getInfoTypeForString("C2C_ERA_INDUSTRIAL")

######## INFLUENZA ###########

def canTriggerInfluenza(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)

	if GC.getTeam(player.getTeam()).isHasTech(GC.getInfoTypeForString("TECH_MEDICINE")):
		return False

	return True

def applyInfluenza2(argsList):
  data = argsList[1]

  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  iNumCities = 2 + GAME.getSorenRandNum(3, "Influenza event number of cities")

  listCities = []
  for loopCity in player.cities():
    if loopCity.getPopulation() > 2:
      iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())
      if iDistance > 0:
        listCities.append((iDistance, loopCity))

  listCities.sort(key=itemgetter(0))

  if iNumCities > len(listCities):
    iNumCities = len(listCities)

  for i in xrange(iNumCities):
    (iDist, loopCity) = listCities[i]
    loopCity.changePopulation(-1)
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_INFLUENZA_HIT_CITY", (loopCity.getNameKey(), ))
    if isLocalHumanPlayer(data.ePlayer) :
        CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), loopCity.getX(), loopCity.getY(), True, True)


def getHelpInfluenza2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_INFLUENZA_HELP_2", (2, ))

######## SOLO FLIGHT ###########


def canTriggerSoloFlight(argsList):
  data = argsList[0]

  map = GC.getMap()
  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iMinLandmass  = 3
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iMinLandmass  = 4
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iMinLandmass  = 6
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iMinLandmass  = 8
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iMinLandmass  = 10
  else:
    iMinLandmass  = 12

  if (map.getNumLandAreas() < iMinLandmass):
    return False

  if GAME.isGameMultiPlayer():
    return False

  return True

def getHelpSoloFlight(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_SOLO_FLIGHT_HELP_1", (1, ))

def applySoloFlight(argsList):
  data = argsList[1]

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != data.ePlayer:
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(data.ePlayer).getTeam()):
        loopPlayer.AI_changeAttitudeExtra(data.ePlayer, 1)


######## ANTELOPE ###########

def canTriggerAntelope(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  iDeer = GC.getInfoTypeForString("BONUS_DEER")

  plot = GC.getMap().plot(data.iPlotX, data.iPlotY)
  if not plot.canHaveBonus(iDeer, False):
    return False

  return True

def doAntelope2(argsList):
	# Need this because camps are not normally allowed unless there is already deer.
	data = argsList[1]

	plot = GC.getMap().plot(data.iPlotX, data.iPlotY)

	if plot: plot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_HUNTING_CAMP"))

	return 1

def getHelpAntelope2(argsList):
	iCamp = GC.getInfoTypeForString("IMPROVEMENT_HUNTING_CAMP")
	return TRNSLTR.getText("TXT_KEY_EVENT_IMPROVEMENT_GROWTH", ( GC.getImprovementInfo(iCamp).getTextKey(), ))

######## WHALEOFATHING ###########

def canTriggerWhaleOfAThing(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)
	#iWhale = GC.getInfoTypeForString("BONUS_WHALE")

	iNumCoastalCities = player.countNumCoastalCities()
	if 0.65 > ( iNumCoastalCities / player.getNumCities()):
		return False
	return True

######## FOUND_PEARLS ###########

def canTriggerFoundPearls(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)

	iNumCoastalCities = player.countNumCoastalCities()
	if 0.35 > ( iNumCoastalCities / player.getNumCities()):
		return False
	return True


######## FOUND_PEARLS ###########

def canTriggerFoundLobster(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)

	iNumCoastalCities = player.countNumCoastalCities()
	if 0.35 > ( iNumCoastalCities / player.getNumCities()):
		return False
	return True

######## HIYOSILVER ###########

def canTriggerHiyoSilver(argsList):

  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  iSilver = GC.getInfoTypeForString("BONUS_SILVER_ORE")

  plot = GC.getMap().plot(data.iPlotX, data.iPlotY)
  if not plot.canHaveBonus(iSilver, False):
    return False

  return True

######## WININGMONKS ###########

def canTriggerWiningMonks(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  if player.getNumAvailableBonuses(GC.getInfoTypeForString("BONUS_GRAPES")) > 0:
    return False

  return True


def doWiningMonks2(argsList):
	# Need this because wineries are not normally allowed unless there is already wine.
	data = argsList[1]

	plot = GC.getMap().plot(data.iPlotX, data.iPlotY)

	if plot: plot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_WINERY"))

	return 1

def getHelpWiningMonks2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_IMPROVEMENT_GROWTH", (GC.getImprovementInfo(GC.getInfoTypeForString("IMPROVEMENT_WINERY")).getTextKey(), ))


######## INDEPENDENTFILMS ###########

def canTriggerIndependentFilms(argsList):
	# Toffer - Perhaps change this to:
	#return not GC.getPlayer(argsList[0].ePlayer).hasBonus(GC.getInfoTypeForString("BONUS_HIT_MOVIES"))
	# There would be a functionality change though, it can then only trigger once for any player.
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)

	iBonus = GC.getInfoTypeForString("BONUS_HIT_MOVIES")
	for i in xrange(GC.getNumBuildingInfos()):
		building = GC.getBuildingInfo(i)
		if iBonus in building.getFreeBonuses() and player.hasBuilding(i):
			return False
	return True

def doIndependentFilms(argsList):
	GC.getPlayer(argsList[1].ePlayer).getCity(
		argsList[1].iCityId
		).changeFreeBonus(
			GC.getInfoTypeForString("BONUS_HIT_MOVIES"), 1
			)
	return 1

def getHelpIndependentFilms(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_INDEPENDENTFILMS_HELP_1", (1, GC.getBonusInfo(GC.getInfoTypeForString("BONUS_HIT_MOVIES")).getChar(), GC.getPlayer(argsList[1].ePlayer).getCity(argsList[1].iCityId).getNameKey()))

######## ANCIENT OLYMPICS ###########

def canTriggerAncientOlympics(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  stateReligion = player.getStateReligion()

  if stateReligion == GC.getInfoTypeForString("RELIGION_JUDAISM"):
    return False

  if stateReligion == GC.getInfoTypeForString("RELIGION_CHRISTIANITY"):
    return False

  if stateReligion == GC.getInfoTypeForString("RELIGION_ISLAM"):
    return False

  return True

def doAncientOlympics2(argsList):
  data = argsList[1]

  plots = GC.getMap().plots()

  for j in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(j)
    if j != data.ePlayer and loopPlayer.isAlive() and not loopPlayer.isMinorCiv():
      for plot in plots:
        if not plot.isWater() and plot.getOwner() == data.ePlayer and plot.isAdjacentPlayer(j, True):
          loopPlayer.AI_changeMemoryCount(data.ePlayer, MemoryTypes.MEMORY_EVENT_GOOD_TO_US, 1)
          break
  return 1

def getHelpAncientOlympics2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_ANCIENTOLYMPICS_HELP_2", ( 1, ))


######## MODERN OLYMPICS ###########

def canTriggerModernOlympics(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)
	kOrigTriggeredData = player.getEventOccured(GC.getEventTriggerInfo(data.eTrigger).getPrereqEvent(0))
	if kOrigTriggeredData is None:
		return False
	kActualTriggeredDataObject = player.getEventTriggered(data.iId)
	kActualTriggeredDataObject.iCityId = kOrigTriggeredData.iCityId
	kActualTriggeredDataObject.iPlotX = kOrigTriggeredData.iPlotX
	kActualTriggeredDataObject.iPlotY = kOrigTriggeredData.iPlotY

	return True

def getHelpModernOlympics(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_SOLO_FLIGHT_HELP_1", (1, ))

def applyModernOlympics(argsList):
	data = argsList[1]

	for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
		loopPlayer = GC.getPlayer(iPlayer)
		if loopPlayer.isAlive() and iPlayer != data.ePlayer:
			loopTeam = GC.getTeam(loopPlayer.getTeam())
			if loopTeam.isHasMet(GC.getPlayer(data.ePlayer).getTeam()):
				loopPlayer.AI_changeAttitudeExtra(data.ePlayer, 1)


######## INTERSTATE ###########

def canTriggerInterstate(argsList):
	data = argsList[0]
	return GC.getPlayer(data.ePlayer).isCivic(GC.getInfoTypeForString("CIVIC_EGALITARIAN"))

def getHelpInterstate(argsList):
	return TRNSLTR.getText("TXT_KEY_UNITHELP_MOVEMENT", (1, GC.getRouteInfo(GC.getInfoTypeForString("ROUTE_HIGHWAY")).getTextKey()))

def applyInterstate(argsList):
	data = argsList[1]
	GC.getTeam(GC.getPlayer(data.ePlayer).getTeam()).changeRouteChange(GC.getInfoTypeForString("ROUTE_HIGHWAY"), -5)

######## EARTH DAY ###########

def getHelpEarthDay2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_EARTHDAY_HELP_2", ())

def canApplyEarthDay2(argsList):
  data = argsList[1]

  player = GC.getPlayer(data.ePlayer)

  iCivic = GC.getInfoTypeForString("CIVIC_GREEN")

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != data.ePlayer and not loopPlayer.isHuman():
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(data.ePlayer).getTeam()):
        tradeData = TradeData()
        tradeData.ItemType = TradeableItems.TRADE_CIVIC
        tradeData.iData = iCivic
        if loopPlayer.canTradeItem(data.ePlayer, tradeData, False):
          if (loopPlayer.getTradeDenial(data.ePlayer, tradeData) == DenialTypes.NO_DENIAL):
            return True
  return False


def applyEarthDay2(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  iCivic = GC.getInfoTypeForString("CIVIC_GREEN")
  iCivicOption = GC.getInfoTypeForString("CIVICOPTION_ECONOMY")

  listPlayers = []
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != data.ePlayer and not loopPlayer.isHuman():
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(data.ePlayer).getTeam()):
        tradeData = TradeData()
        tradeData.ItemType = TradeableItems.TRADE_CIVIC
        tradeData.iData = iCivic
        if loopPlayer.canTradeItem(data.ePlayer, tradeData, False):
          if (loopPlayer.getTradeDenial(data.ePlayer, tradeData) == DenialTypes.NO_DENIAL):
            listPlayers.append((-loopPlayer.AI_civicValue(iCivic), iPlayer))

  listPlayers.sort()

  if len(listPlayers) > 3:
    listPlayers = listPlayers[:2]

  for (iValue, iPlayer) in listPlayers:
    GC.getPlayer(iPlayer).setCivics(iCivicOption, iCivic)

######## FREEDOM CONCERT ###########

def getHelpFreedomConcert2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_FREEDOMCONCERT_HELP_2", ())

def canApplyFreedomConcert2(argsList):
  data = argsList[1]

  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  for iReligion in xrange(GC.getNumReligionInfos()):
    if eventCity.isHasReligion(iReligion):
      for loopCity in player.cities():
        if not loopCity.isHasReligion(iReligion):
          for jReligion in xrange(GC.getNumReligionInfos()):
            if loopCity.isHasReligion(jReligion):
              return True

  return False

def applyFreedomConcert2(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  for iReligion in xrange(GC.getNumReligionInfos()):
    if eventCity.isHasReligion(iReligion):

      bestCity = None
      iBestDistance = 0
      for loopCity in player.cities():
        if not loopCity.isHasReligion(iReligion):
          bValid = False
          for jReligion in xrange(GC.getNumReligionInfos()):
            if loopCity.isHasReligion(jReligion):
              bValid = True
              break

          if bValid:
            iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())

            if iDistance < iBestDistance or bestCity == None:
              bestCity = loopCity
              iBestDistance = iDistance

      if bestCity != None:
        bestCity.setHasReligion(iReligion, True, True, True)

######## HEROIC_GESTURE ###########

def canTriggerHeroicGesture(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)
  destPlayer = GC.getPlayer(data.eOtherPlayer)

  if not GC.getTeam(destPlayer.getTeam()).canChangeWarPeace(player.getTeam()):
    return False

  if GC.getTeam(destPlayer.getTeam()).AI_getWarSuccess(player.getTeam()) <= 0:
    return False

  if GC.getTeam(player.getTeam()).AI_getWarSuccess(destPlayer.getTeam()) <= 0:
    return False

  # do not use this event in multiplayer
  if GAME.isNetworkMultiPlayer():
    return False

  return True

def doHeroicGesture2(argsList):
  data = argsList[1]

  destPlayer = GC.getPlayer(data.eOtherPlayer)
  player = GC.getPlayer(data.ePlayer)

  if destPlayer.isHuman():
    # this works only because it's a single-player only event
    popupInfo = CyPopupInfo()
    popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
    popupInfo.setText(TRNSLTR.getText("TXT_KEY_EVENT_HEROIC_GESTURE_OTHER_3", (player.getCivilizationAdjectiveKey(), )))
    popupInfo.setData1(data.eOtherPlayer)
    popupInfo.setData2(data.ePlayer)
    popupInfo.setPythonModule("CvRandomEventInterface")
    popupInfo.setOnClickedPythonCallback("heroicGesture2Callback")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_YES", ()), "")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_NO", ()), "")
    popupInfo.addPopup(data.eOtherPlayer)
  else:
    destPlayer.forcePeace(data.ePlayer)
    destPlayer.AI_changeAttitudeExtra(data.ePlayer, 1)
    player.AI_changeAttitudeExtra(data.eOtherPlayer, 1)

  return

def heroicGesture2Callback(argsList):
  iButton = argsList[0]
  iData1 = argsList[1]
  iData2 = argsList[2]
  iData3 = argsList[3]
  szText = argsList[4]
  bOption1 = argsList[5]
  bOption2 = argsList[6]

  if iButton == 0:
    destPlayer = GC.getPlayer(iData1)
    player = GC.getPlayer(iData2)
    destPlayer.forcePeace(iData2)
    destPlayer.AI_changeAttitudeExtra(iData2, 1)
    player.AI_changeAttitudeExtra(iData1, 1)

  return 0

def getHelpHeroicGesture2(argsList):
	data = argsList[1]
	destPlayer = GC.getPlayer(data.eOtherPlayer)
	return TRNSLTR.getText("TXT_KEY_EVENT_ATTITUDE_GOOD", (1, destPlayer.getNameKey()))

######## WAR_TAX ########

def canTriggerWarTax(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)
	destPlayer = GC.getPlayer(data.eOtherPlayer)

	if not GC.getTeam(player.getTeam()).canChangeWarPeace(destPlayer.getTeam()):
		return False

	if GC.getTeam(player.getTeam()).AI_getAtWarCounter(destPlayer.getTeam()) < 10:
		return False

	return True

def applyWarTax(argsList):
	data = argsList[1]
	player = GC.getPlayer(data.ePlayer)
	player.changeGold(+400 * player.getNumCities())

######## GREAT_MEDIATOR ###########

def canTriggerGreatMediator(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)
  destPlayer = GC.getPlayer(data.eOtherPlayer)

  if not GC.getTeam(player.getTeam()).canChangeWarPeace(destPlayer.getTeam()):
    return False

  if GC.getTeam(player.getTeam()).AI_getAtWarCounter(destPlayer.getTeam()) < 10:
    return False

  # do not use this event in multiplayer
  if GAME.isNetworkMultiPlayer():
    return False

  return True

def doGreatMediator2(argsList):
  data = argsList[1]

  destPlayer = GC.getPlayer(data.eOtherPlayer)
  player = GC.getPlayer(data.ePlayer)

  if destPlayer.isHuman():
    # this works only because it's a single-player only event
    popupInfo = CyPopupInfo()
    popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
    popupInfo.setText(TRNSLTR.getText("TXT_KEY_EVENT_GREAT_MEDIATOR_OTHER_3", (player.getCivilizationAdjectiveKey(), )))
    popupInfo.setData1(data.eOtherPlayer)
    popupInfo.setData2(data.ePlayer)
    popupInfo.setPythonModule("CvRandomEventInterface")
    popupInfo.setOnClickedPythonCallback("greatMediator2Callback")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_YES", ()), "")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_NO", ()), "")
    popupInfo.addPopup(data.eOtherPlayer)
  else:
    GC.getTeam(player.getTeam()).makePeace(destPlayer.getTeam())
    destPlayer.AI_changeAttitudeExtra(data.ePlayer, 1)
    player.AI_changeAttitudeExtra(data.eOtherPlayer, 1)

  return

def greatMediator2Callback(argsList):
  iButton = argsList[0]
  iData1 = argsList[1]
  iData2 = argsList[2]
  iData3 = argsList[3]
  szText = argsList[4]
  bOption1 = argsList[5]
  bOption2 = argsList[6]

  if iButton == 0:
    destPlayer = GC.getPlayer(iData1)
    player = GC.getPlayer(iData2)
    GC.getTeam(destPlayer.getTeam()).makePeace(player.getTeam())
    destPlayer.AI_changeAttitudeExtra(iData2, 1)
    player.AI_changeAttitudeExtra(iData1, 1)

  return 0

def getHelpGreatMediator2(argsList):
	data = argsList[1]
	destPlayer = GC.getPlayer(data.eOtherPlayer)
	return TRNSLTR.getText("TXT_KEY_EVENT_ATTITUDE_GOOD", (1, destPlayer.getNameKey()))

######## ANCIENT_TEXTS ###########

def doAncientTexts2(argsList):
  data = argsList[1]

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != data.ePlayer:
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(data.ePlayer).getTeam()):
        loopPlayer.AI_changeAttitudeExtra(data.ePlayer, 1)
  return

def getHelpAncientTexts2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_SOLO_FLIGHT_HELP_1", (1, ))


######## IMPACT_CRATER ###########

def doImpactCrater2(argsList):
	data = argsList[1]
	plot = GC.getMap().plot(data.iPlotX, data.iPlotY)

	if plot: plot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_SHAFT_MINE"))

	return 1

def getHelpImpactCrater2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_IMPROVEMENT_GROWTH", (GC.getImprovementInfo(GC.getInfoTypeForString("IMPROVEMENT_SHAFT_MINE")).getTextKey(), ))

######## THE_HUNS ###########

def canTriggerTheHuns(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

#   At least one civ on the board must know Mounted Archery.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_MOUNTED_ARCHERY")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Iron Working.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_IRON_WORKING")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False


def getHelpTheHuns1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_THE_HUNS_HELP_1", ())

def applyTheHuns1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Hun event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnits  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnits  = 4
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnits  = 6
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnits  = 8
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnits  = 10
  else:
    iNumUnits  = 30

  iUnitType = GC.getInfoTypeForString("UNIT_HORSE_ARCHER")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## THE_VANDALS ###########

def canTriggerTheVandals(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

#   At least one civ on the board must know Metal Casting.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_METAL_CASTING")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Smithing.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_SMITHING")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False


def getHelpTheVandals1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_THE_VANDALS_HELP_1", ())


def applyTheVandals1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Vandal event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnits  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnits  = 4
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnits  = 6
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnits  = 8
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnits  = 10
  else:
    iNumUnits  = 30

  iUnitType = GC.getInfoTypeForString("UNIT_HEAVY_SWORDSMAN")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## THE_GOTHS ###########

def canTriggerTheGoths(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

#   At least one civ on the board must know Mathematics.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_MATHEMATICS")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Iron Working.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_IRON_WORKING")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False


def getHelpThGoths1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_THE_GOTHS_HELP_1", ())

def applyTheGoths1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Goth event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnits  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnits  = 4
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnits  = 6
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnits  = 8
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnits  = 10
  else:
    iNumUnits  = 30

  iUnitType = GC.getInfoTypeForString("UNIT_AXEMAN")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## THE_PHILISTINES ###########

def canTriggerThePhilistines(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

#   At least one civ on the board must know Monotheism.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_MONOTHEISM")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Bronze Working.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_BRONZE_WORKING")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False


def getHelpThePhilistines1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_THE_PHILISTINES_HELP_1", ())


def applyThePhilistines1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Philistine event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnits  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnits  = 4
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnits  = 6
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnits  = 8
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnits  = 10
  else:
    iNumUnits  = 30

  iUnitType = GC.getInfoTypeForString("UNIT_SPEARMAN")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## THE_VEDIC_ARYANS ###########

def canTriggerTheVedicAryans(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

#   At least one civ on the board must know Polytheism.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_POLYTHEISM")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Archery.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_ARCHERY")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False


def getHelpTheVedicAryans1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_THE_VEDIC_ARYANS_HELP_1", ())


def applyTheVedicAryans1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Vedic Aryan event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnits  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnits  = 4
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnits  = 6
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnits  = 8
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnits  = 10
  else:
    iNumUnits  = 30

  iUnitType = GC.getInfoTypeForString("UNIT_ARCHER")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)

######## SECURITY_TAX ###########

def canTriggerSecurityTax(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  iWalls = GC.getInfoTypeForString("BUILDING_WALLS")
  if player.getNumCities() > player.getBuildingCountWithUpgrades(iWalls):
    return False

  return True


######## LITERACY ###########

def canTriggerLiteracy(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  iLibrary = GC.getInfoTypeForString("BUILDING_LIBRARY")
  if player.getNumCities() > player.getBuildingCountWithUpgrades(iLibrary):
    return False

  return True

######## TEA ###########

def canTriggerTea(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)

	if player.isCivic(GC.getInfoTypeForString("CIVIC_MERCANTILISM")):
		return False

	for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
		if player.canHaveTradeRoutesWith(iLoopPlayer):
			return True
	return False

######## HORSE WHISPERING ###########

def canTriggerHorseWhispering(argsList):
	return not GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY)

def getHelpHorseWhispering1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_HORSE_WHISPERING_HELP", (GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(), ))

def canTriggerHorseWhisperingDone(argsList):
	return (
		GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() <=
		GC.getPlayer(argsList[0].ePlayer).getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_STABLE"))
	)

def getHelpHorseWhisperingDone1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_HORSE_WHISPERING_DONE_HELP_1", (GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(), ))

def applyHorseWhisperingDone1(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)

	MAP = GC.getMap()
	plot = MAP.plot(data.iPlotX, data.iPlotY)
	x = plot.getX()
	y = plot.getY()
	iUnit = GC.getInfoTypeForString("UNIT_HORSE_ARCHER")
	iCount = GC.getWorldInfo(MAP.getWorldSize()).getDefaultPlayers()
	while iCount > 0:
		CyPlayer.initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)
		iCount -= 1

######## HARBORMASTER ###########

def getHelpHarbormaster1(argsList):
	iHarborsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
	iCaravelsRequired = iHarborsRequired / 2 + 1
	return TRNSLTR.getText("TXT_KEY_EVENT_HARBORMASTER_HELP", (iHarborsRequired, iCaravelsRequired))

def canTriggerHarbormaster(argsList):
  data = argsList[0]
  trigger = GC.getEventTriggerInfo(data.eTrigger)
  player = GC.getPlayer(data.ePlayer)

  if GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY):
    return False

  map = GC.getMap()
  iNumWater = 0

  for plot in map.plots():
    if plot.isWater():
      iNumWater += 1
    if 100 * iNumWater >= 40 * map.numPlots():
      return True

  return False

def canTriggerHarbormasterDone(argsList):
  data = argsList[0]
  trigger = GC.getEventTriggerInfo(data.eTrigger)
  player = GC.getPlayer(data.ePlayer)

  iHarbor = GC.getInfoTypeForString("BUILDING_PORT")
  iHarborsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
  if iHarborsRequired > player.getBuildingCountWithUpgrades(iHarbor):
    return False

  iCaravelsRequired = iHarborsRequired / 2 + 1
  if iCaravelsRequired > player.getUnitCount(GC.getInfoTypeForString("UNIT_CARAVEL")):
    return False

  return True

######## CLASSIC LITERATURE ###########

def canTriggerClassicLiterature(argsList):
	return not GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY)

def getHelpClassicLiterature1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_CLASSIC_LITERATURE_HELP_1", (GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(), ))

def canTriggerClassicLiteratureDone(argsList):
	return (
		GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() <=
		GC.getPlayer(argsList[0].ePlayer).getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_LIBRARY"))
	)

def getHelpClassicLiteratureDone2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_CLASSIC_LITERATURE_DONE_HELP_2", ())

def canApplyClassicLiteratureDone2(argsList):
	player = GC.getPlayer(argsList[1].ePlayer)
	iEraAncient = GC.getInfoTypeForString("C2C_ERA_ANCIENT")

	team = GC.getTeam(player.getTeam())
	for i in xrange(team.getNumAdjacentResearch()):
		iTechX = team.getAdjacentResearch(i)
		if GC.getTechInfo(iTechX).getEra() == iEraAncient and player.canResearch(iTechX, True, True):
			return True
	return False

def applyClassicLiteratureDone2(argsList):
	data = argsList[1]
	player = GC.getPlayer(data.ePlayer)

	iEraAncient = GC.getInfoTypeForString("C2C_ERA_ANCIENT")

	listTechs = []
	team = GC.getTeam(player.getTeam())
	for i in xrange(team.getNumAdjacentResearch()):
		iTechX = team.getAdjacentResearch(i)
		if GC.getTechInfo(iTechX).getEra() == iEraAncient and player.canResearch(iTechX, True, True):
			listTechs.append(iTechX)

	if listTechs:
		team.setHasTech(listTechs[GAME.getSorenRandNum(len(listTechs), "Classic Literature Event Tech selection")], True, data.ePlayer, True, True)

def getHelpClassicLiteratureDone3(argsList):
	iGreatLibrary = GC.getInfoTypeForString("BUILDING_THE_GREAT_LIBRARY")

	szCityName = ""
	for city in GC.getPlayer(argsList[1].ePlayer).cities():
		if city.hasBuilding(iGreatLibrary):
			szCityName = city.getNameKey()
			break

	return TRNSLTR.getText("TXT_KEY_EVENT_FREE_SPECIALIST", (1, GC.getSpecialistInfo(GC.getInfoTypeForString("SPECIALIST_SCIENTIST")).getTextKey(), szCityName))

def canApplyClassicLiteratureDone3(argsList):
	iGreatLibrary = GC.getInfoTypeForString("BUILDING_THE_GREAT_LIBRARY")

	for city in GC.getPlayer(argsList[1].ePlayer).cities():
		if city.hasBuilding(iGreatLibrary):
			return True
	return False

def applyClassicLiteratureDone3(argsList):
	iGreatLibrary = GC.getInfoTypeForString("BUILDING_THE_GREAT_LIBRARY")

	for city in GC.getPlayer(argsList[1].ePlayer).cities():
		if city.hasBuilding(iGreatLibrary):
			city.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SCIENTIST"), 1)
			return

######## MASTER BLACKSMITH ###########

def canTriggerMasterBlacksmith(argsList):
	return not GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY)

def getHelpMasterBlacksmith1(argsList):
	return (
		TRNSLTR.getText(
			"TXT_KEY_EVENT_MASTER_BLACKSMITH_HELP_1",
			(
				GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(),
				GC.getPlayer(argsList[1].ePlayer).getCity(argsList[1].iCityId).getNameKey()
			)
		)
	)

def expireMasterBlacksmith1(argsList):
	iPlayer = argsList[1].ePlayer
	CyCity = GC.getPlayer(iPlayer).getCity(argsList[1].iCityId)
	return not CyCity or CyCity.getOwner() != iPlayer

def canTriggerMasterBlacksmithDone(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)

	if GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() > player.getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_FORGE")):
		return False

	prereqEventData = player.getEventOccured(GC.getEventTriggerInfo(data.eTrigger).getPrereqEvent(0))

	city = player.getCity(prereqEventData.iCityId)
	if not city or city.getOwner() != data.ePlayer:
		return False

	eventData = player.getEventTriggered(data.iId)
	eventData.iCityId = prereqEventData.iCityId

	return True

def canApplyMasterBlacksmithDone1(argsList):
	#eEvent = argsList[0]
	data = argsList[1]

	player = GC.getPlayer(data.ePlayer)
	city = player.getCity(data.iCityId)
	if not city: return False

	iX = city.getX()
	iY = city.getY()
	iBonus = GC.getInfoTypeForString("BONUS_COPPER_ORE")

	map = GC.getMap()
	iBestValue = map.getGridWidth() + map.getGridHeight()
	bestPlot = None
	for plot in map.plots():
		if plot.getOwner() == data.ePlayer and plot.canHaveBonus(iBonus, False):
			iValue = plotDistance(iX, iY, plot.getX(), plot.getY())
			if iValue < iBestValue:
				iBestValue = iValue
				bestPlot = plot

	if not bestPlot: return False

	eventData = player.getEventTriggered(data.iId)
	eventData.iPlotX = bestPlot.getX()
	eventData.iPlotY = bestPlot.getY()

	return True

def applyMasterBlacksmithDone1(argsList):
	#eEvent = argsList[0]
	data = argsList[1]

	iBonus = GC.getInfoTypeForString("BONUS_COPPER_ORE")
	plot = GC.getMap().plot(data.iPlotX, data.iPlotY)
	plot.setBonusType(iBonus)

	if isLocalHumanPlayer(data.ePlayer) :
		CyInterface().addMessage(
			data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(),
			TRNSLTR.getText(
				"TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE_IMPROVEMENT",
				(
					GC.getPlayer(data.ePlayer).getCity(data.iCityId).getNameKey(), GC.getBonusInfo(iBonus).getTextKey()
				)
			),
			"AS2D_DISCOVERBONUS", InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT, GC.getBonusInfo(iBonus).getButton(),
			GC.getInfoTypeForString("COLOR_WHITE"), plot.getX(), plot.getY(), True, True
		)


######## THE BEST DEFENSE ###########

def canTriggerBestDefense(argsList):
	return not GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY)

def getHelpBestDefense1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_BEST_DEFENSE_HELP_1", (GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(), ))

def canTriggerBestDefenseDone(argsList):
  data = argsList[0]
  trigger = GC.getEventTriggerInfo(data.eTrigger)
  player = GC.getPlayer(data.ePlayer)

  iCastle = GC.getInfoTypeForString("BUILDING_CASTLE")
  iBuildingsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
  if iBuildingsRequired > player.getBuildingCountWithUpgrades(iCastle):
    return False

  return True

def getHelpBestDefenseDone2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_BEST_DEFENSE_DONE_HELP_2", (3, ))

def applyBestDefenseDone2(argsList):
	data = argsList[1]

	for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
		loopPlayer = GC.getPlayer(iPlayer)
		if loopPlayer.isAlive() and iPlayer != data.ePlayer:
			loopTeam = GC.getTeam(loopPlayer.getTeam())
			if loopTeam.isHasMet(GC.getPlayer(data.ePlayer).getTeam()):
				loopPlayer.AI_changeAttitudeExtra(data.ePlayer, 3)


def canApplyBestDefenseDone3(argsList):
	iGreatWall = GC.getInfoTypeForString("BUILDING_GREAT_WALL_OF_CHINA")

	for city in GC.getPlayer(argsList[1].ePlayer).cities():
		if city.hasBuilding(iGreatWall):
			return True
	return False

######## NATIONAL SPORTS LEAGUE ###########

def canTriggerSportsLeague(argsList):
	return not GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY)

def getHelpSportsLeague1(argsList):
	return (
		TRNSLTR.getText(
			"TXT_KEY_EVENT_SPORTS_LEAGUE_HELP_1",
			(
				GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(),
				GC.getBuildingInfo(GC.getInfoTypeForString("BUILDING_CIRCUS_MAXIMUS")).getTextKey()
			)
		)
	)

def canTriggerSportsLeagueDone(argsList):
  data = argsList[0]
  trigger = GC.getEventTriggerInfo(data.eTrigger)
  player = GC.getPlayer(data.ePlayer)

  iColosseum = GC.getInfoTypeForString("BUILDING_COLOSSEUM")
  iBuildingsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
  if iBuildingsRequired > player.getBuildingCountWithUpgrades(iColosseum):
    return False

  return True

def canApplySportsLeagueDone3(argsList):
	iZeus = GC.getInfoTypeForString("BUILDING_CIRCUS_MAXIMUS")

	for city in GC.getPlayer(argsList[1].ePlayer).cities():
		if city.hasBuilding(iZeus):
			return True
	return False

######## CRUSADE ###########

def canTriggerCrusade(argsList):
	data = argsList[0]

	if GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY):
		return False

	holyCity = GAME.getHolyCity(data.eReligion)
	if not holyCity or holyCity.getOwner() != data.eOtherPlayer:
		return False

	kActualTriggeredDataObject = GC.getPlayer(data.ePlayer).getEventTriggered(data.iId)
	kActualTriggeredDataObject.iOtherPlayerCityId = holyCity.getID()
	return True

def getHelpCrusade1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_CRUSADE_HELP_1", (GAME.getHolyCity(argsList[1].eReligion).getNameKey(), ))

def expireCrusade1(argsList):
	data = argsList[1]

	holyCity = GAME.getHolyCity(data.eReligion)
	if holyCity is None or holyCity.getOwner() != data.eOtherPlayer:
		return True

	if holyCity.getOwner() == data.ePlayer:
		return False

	player = GC.getPlayer(data.ePlayer)
	if player.getStateReligion() != data.eReligion:
		return True

	if not GC.getTeam(player.getTeam()).isAtWarWith(GC.getPlayer(data.eOtherPlayer).getTeam()):
		return True

	return False

def canTriggerCrusadeDone(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)

	kOrigTriggeredData = player.getEventOccured(GC.getEventTriggerInfo(data.eTrigger).getPrereqEvent(0))
	holyCity = GAME.getHolyCity(kOrigTriggeredData.eReligion)

	if holyCity.getOwner() != data.ePlayer:
		return False

	kActualTriggeredDataObject = player.getEventTriggered(data.iId)
	kActualTriggeredDataObject.iCityId = holyCity.getID()
	kActualTriggeredDataObject.eOtherPlayer = kOrigTriggeredData.eOtherPlayer
	kActualTriggeredDataObject.eReligion = kOrigTriggeredData.eReligion

	for iBuilding in xrange(GC.getNumBuildingInfos()):
		if GC.getBuildingInfo(iBuilding).getHolyCity() == kOrigTriggeredData.eReligion:
			kActualTriggeredDataObject.eBuilding = BuildingTypes(iBuilding)
			break
	return True

def getHelpCrusadeDone1(argsList):
	holyCity = GAME.getHolyCity(argsList[1].eReligion)
	szUnit = GC.getUnitInfo(holyCity.getConscriptUnit()).getTextKey()
	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() / 2 + 1

	return TRNSLTR.getText("TXT_KEY_EVENT_CRUSADE_DONE_HELP_1", (iNumUnits, szUnit, holyCity.getNameKey()))

def canApplyCrusadeDone1(argsList):
	if -1 == GAME.getHolyCity(argsList[1].eReligion).getConscriptUnit():
		return False
	return True

def applyCrusadeDone1(argsList):
	data = argsList[1]
	holyCity = GAME.getHolyCity(data.eReligion)
	iUnitType = holyCity.getConscriptUnit()

	if iUnitType != -1:
		player = GC.getPlayer(data.ePlayer)
		for i in xrange(GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() / 2 + 1):
			player.initUnit(iUnitType, holyCity.getX(), holyCity.getY(), UnitAITypes.UNITAI_CITY_DEFENSE, DirectionTypes.DIRECTION_SOUTH)

def getHelpCrusadeDone2(argsList):
	data = argsList[1]
	return TRNSLTR.getText("TXT_KEY_EVENT_CRUSADE_DONE_HELP_2", (GC.getBuildingInfo(data.eBuilding).getTextKey(), GAME.getHolyCity(data.eReligion).getNameKey()))

def canApplyCrusadeDone2(argsList):
	data = argsList[1]
	if data.eBuilding < 0:
		return False
	city = GAME.getHolyCity(data.eReligion)

	if city is None or city.hasBuilding(data.eBuilding):
		return False
	return True

def applyCrusadeDone2(argsList):
  data = argsList[1]

  holyCity = GAME.getHolyCity(data.eReligion)
  holyCity.changeHasBuilding(data.eBuilding, True)

  if (not GAME.isNetworkMultiPlayer() and data.ePlayer == GAME.getActivePlayer()):
    popupInfo = CyPopupInfo()
    popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
    popupInfo.setData1(data.eBuilding)
    popupInfo.setData2(holyCity.getID())
    popupInfo.setData3(0)
    popupInfo.setText(u"showWonderMovie")
    popupInfo.addPopup(data.ePlayer)

def getHelpCrusadeDone3(argsList):
	data = argsList[1]
	iNumCities = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
	return TRNSLTR.getText("TXT_KEY_EVENT_CRUSADE_DONE_HELP_3", (GC.getReligionInfo(data.eReligion).getTextKey(), iNumCities))

def canApplyCrusadeDone3(argsList):
	data = argsList[1]
	#iNumCities = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
	if GAME.getNumCities() == GAME.countReligionLevels(data.eReligion):
		return False
	return True

def applyCrusadeDone3(argsList):
  data = argsList[1]
  holyCity = GAME.getHolyCity(data.eReligion)

  listCities = []
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      for loopCity in loopPlayer.cities():
        if not loopCity.isHasReligion(data.eReligion):
          iDistance = plotDistance(holyCity.getX(), holyCity.getY(), loopCity.getX(), loopCity.getY())
          listCities.append((iDistance, loopCity))

  listCities.sort(key=itemgetter(0))

  iNumCities = min(GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(), len(listCities))

  for i in xrange(iNumCities):
    iDistance, loopCity = listCities[i]
    loopCity.setHasReligion(data.eReligion, True, True, True)

######## ESTEEMEED_PLAYWRIGHT ###########

def canTriggerEsteemedPlaywright(argsList):
	return not GC.getPlayer(argsList[0].ePlayer).hasBuilding(GC.getInfoTypeForString("BUILDING_WORLDVIEW_SLAVERY_ACTIVE"))


######## SECRET_KNOWLEDGE ###########

def getHelpSecretKnowledge2(argsList):
	data = argsList[1]
	return TRNSLTR.getText("TXT_KEY_EVENT_YIELD_CHANGE_BUILDING", (GC.getBuildingInfo(data.eBuilding).getTextKey(), u"+4[ICON_CULTURE]"))

def applySecretKnowledge2(argsList):
	data = argsList[1]
	city = GC.getPlayer(data.ePlayer).getCity(data.iCityId)
	city.setBuildingCommerceChange(data.eBuilding, CommerceTypes.COMMERCE_CULTURE, 4)

######## HIGH_WARLORD ###########

def canTriggerHighWarlord(argsList):
	data = argsList[0]
  # If source civ is operating this Civic, disallow the event to trigger.
	return not GC.getPlayer(data.ePlayer).isCivic(GC.getInfoTypeForString("CIVIC_EGALITARIAN"))


######## EXPERIENCED_CAPTAIN ###########

def canTriggerExperiencedCaptain(argsList):
  data = argsList[0]

  player = GC.getPlayer(data.ePlayer)
  unit = player.getUnit(data.iUnitId)

  if unit is None:
    return False

  if unit.getExperience() < 7:
    return False

  return True

######## GREED ###########

def canTriggerGreed(argsList):
	data = argsList[0]
	#trigger = GC.getEventTriggerInfo(data.eTrigger)
	CyPlayer = GC.getPlayer(data.ePlayer)
	CyPlayerOther = GC.getPlayer(data.eOtherPlayer)
	iTeam = CyPlayer.getTeam()

	if not GC.getTeam(iTeam).canChangeWarPeace(CyPlayerOther.getTeam()):
		return False

	bonuses = []
	iOil = GC.getInfoTypeForString("BONUS_OIL")
	if not CyPlayer.getNumAvailableBonuses(iOil):
		bonuses.append(iOil)

	iIron = GC.getInfoTypeForString("BONUS_IRON_ORE")
	if not CyPlayer.getNumAvailableBonuses(iIron):
		bonuses.append(iIron)

	iHorse = GC.getInfoTypeForString("BONUS_HORSE")
	if not CyPlayer.getNumAvailableBonuses(iHorse):
		bonuses.append(iHorse)

	iCopper = GC.getInfoTypeForString("BONUS_COPPER_ORE")
	if not CyPlayer.getNumAvailableBonuses(iCopper):
		bonuses.append(iCopper)

	iSulphur = GC.getInfoTypeForString("BONUS_SULPHUR")
	if not CyPlayer.getNumAvailableBonuses(iSulphur):
		bonuses.append(iSulphur)

	iElephant = GC.getInfoTypeForString("BONUS_ELEPHANTS")
	if not CyPlayer.getNumAvailableBonuses(iElephant):
		bonuses.append(iElephant)

	if not bonuses:
		return False

	iSize = len(bonuses)
	shuffledRange = [0]*iSize
	shuffleList(iSize, GAME.getMapRand(), shuffledRange)
	MAP = GC.getMap()
	iNumPlots = MAP.numPlots()
	listPlots = []
	iCount = 0
	while shuffledRange:
		iBonus = bonuses[shuffledRange.pop()]
		for i in xrange(iNumPlots):
			CyPlot = MAP.plotByIndex(i)
			if CyPlot.isWater() or not CyPlot.isRevealed(iTeam, False):
				continue
			if CyPlot.getOwner() == data.eOtherPlayer and CyPlot.getBonusType(iTeam) == iBonus:
				listPlots.append(CyPlot)
				iCount += 1
		if listPlots:
			break
	if not listPlots:
		return False

	CyPlot = listPlots[GAME.getSorenRandNum(iCount, "Random Plot")]

	if -1 == getGreedUnit(CyPlayer, CyPlot):
		return False

	kActualTriggeredDataObject = CyPlayer.getEventTriggered(data.iId)
	kActualTriggeredDataObject.iPlotX = CyPlot.getX()
	kActualTriggeredDataObject.iPlotY = CyPlot.getY()
	return True

def getHelpGreed1(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)
	CyPlayerOther = GC.getPlayer(data.eOtherPlayer)
	iBonus = GC.getMap().plot(data.iPlotX, data.iPlotY).getBonusType(CyPlayer.getTeam())

	iTurns = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent()

	return TRNSLTR.getText("TXT_KEY_EVENT_GREED_HELP_1", (CyPlayerOther.getCivilizationShortDescriptionKey(), GC.getBonusInfo(iBonus).getTextKey(), iTurns))

def expireGreed1(argsList):
	data = argsList[1]
	CyPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)
	iOwner = CyPlot.getOwner()

	if iOwner == -1 or iOwner == data.ePlayer:
		return False

	if GAME.getGameTurn() >= data.iTurn + GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent():
		return True

	if iOwner != data.eOtherPlayer:
		return True

	return False

def canTriggerGreedDone(argsList):
	data = argsList[0]
	CyPlayer = GC.getPlayer(data.ePlayer)
	trigger = GC.getEventTriggerInfo(data.eTrigger)
	kOrigTriggeredData = CyPlayer.getEventOccured(trigger.getPrereqEvent(0))
	CyPlot = GC.getMap().plot(kOrigTriggeredData.iPlotX, kOrigTriggeredData.iPlotY)

	if CyPlot.getOwner() != kOrigTriggeredData.ePlayer:
		return False

	if -1 == getGreedUnit(CyPlayer, CyPlot):
		return False

	kActualTriggeredDataObject = CyPlayer.getEventTriggered(data.iId)
	kActualTriggeredDataObject.iPlotX = kOrigTriggeredData.iPlotX
	kActualTriggeredDataObject.iPlotY = kOrigTriggeredData.iPlotY
	kActualTriggeredDataObject.eOtherPlayer = kOrigTriggeredData.eOtherPlayer

	return True

def getGreedUnit(CyPlayer, CyPlot):
	iBonus = CyPlot.getBonusType(CyPlayer.getTeam())
	iBestValue = 0
	iBestUnit = -1
	for iUnit in xrange(GC.getNumUnitInfos()):
		CvUnitInfo = GC.getUnitInfo(iUnit)
		if CvUnitInfo.getMaxGlobalInstances() != -1 or CvUnitInfo.getMaxPlayerInstances() != -1:
			continue
		if CvUnitInfo.getDomainType() == DomainTypes.DOMAIN_LAND and CyPlayer.canTrain(iUnit, False, False):
			if CvUnitInfo.getPrereqAndBonus() == iBonus or iBonus in CvUnitInfo.getPrereqOrBonuses():
				iValue = CyPlayer.AI_unitValue(iUnit, UnitAITypes.UNITAI_ATTACK, CyPlot.area())
				if iValue > iBestValue:
					iBestValue = iValue
					iBestUnit = iUnit
	return iBestUnit


def getHelpGreedDone1(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)
	CyPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)

	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
	iUnitType = getGreedUnit(CyPlayer, CyPlot)

	if iUnitType != -1:
		szHelp = TRNSLTR.getText("TXT_KEY_EVENT_GREED_DONE_HELP_1", (iNumUnits, GC.getUnitInfo(iUnitType).getTextKey()))
	return szHelp

def applyGreedDone1(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)
	CyPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)

	iUnitType = getGreedUnit(CyPlayer, CyPlot)
	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

	for i in xrange(iNumUnits):
		CyPlayer.initUnit(iUnitType, CyPlot.getX(), CyPlot.getY(), UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)


######## WAR CHARIOTS ###########

def canTriggerWarChariots(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  kActualTriggeredDataObject = player.getEventTriggered(data.iId)
  kActualTriggeredDataObject.eReligion = ReligionTypes(player.getStateReligion())

  return True

def getHelpWarChariots1(argsList):
	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1
	return TRNSLTR.getText("TXT_KEY_EVENT_WAR_CHARIOTS_HELP_1", (iNumUnits, ))

def canTriggerWarChariotsDone(argsList):
  data = argsList[0]
  trigger = GC.getEventTriggerInfo(data.eTrigger)
  player = GC.getPlayer(data.ePlayer)

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1

  if player.getUnitCount(GC.getInfoTypeForString("UNIT_CHARIOT")) < iNumUnits:
    return False

  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))
  kActualTriggeredDataObject = player.getEventTriggered(data.iId)
  kActualTriggeredDataObject.eReligion = kOrigTriggeredData.eReligion

  return True

######## ELITE SWORDSMEN ###########

def getHelpEliteSwords1(argsList):
	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 5
	return TRNSLTR.getText("TXT_KEY_EVENT_ELITE_SWORDS_HELP_1", (iNumUnits, ))

def canTriggerEliteSwordsDone(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 5
  iUnit1 = GC.getInfoTypeForString("UNIT_LIGHT_SWORDSMAN")
  iUnit2 = GC.getInfoTypeForString("UNIT_SWORDSMAN")
  iUnit3 = GC.getInfoTypeForString("UNIT_HEAVY_SWORDSMAN")
  if player.getUnitCount(iUnit1) + player.getUnitCount(iUnit2) + player.getUnitCount(iUnit3) < iNumUnits:
    return False

  return True


def canApplyEliteSwordsDone2(argsList):
	data = argsList[1]
	return GC.getPlayer(data.ePlayer).isCivic(GC.getInfoTypeForString("CIVIC_MONARCHY"))

######## WARSHIPS ###########

def canTriggerWarships(argsList):
  map = GC.getMap()
  iNumWater = 0
  for plot in map.plots():
    if plot.isWater():
      iNumWater += 1
    if 100 * iNumWater >= 55 * map.numPlots():
      return True

  return False

def getHelpWarships1(argsList):
	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
	iBuilding = GC.getInfoTypeForString("BUILDING_THE_GREAT_LIGHTHOUSE")
	return TRNSLTR.getText("TXT_KEY_EVENT_WARSHIPS_HELP_1", (iNumUnits, GC.getBuildingInfo(iBuilding).getTextKey()))

def canTriggerWarshipsDone(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

  if player.getUnitCount(GC.getInfoTypeForString("UNIT_TRIREME")) < iNumUnits:
    return False

  return True


def canApplyWarshipsDone2(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  iBuilding = GC.getInfoTypeForString("BUILDING_THE_GREAT_LIGHTHOUSE")
  if player.getBuildingCountWithUpgrades(iBuilding) == 0:
    return False

  return True

######## GUNS NOT BUTTER ###########

def getHelpGunsButter1(argsList):
	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1
	iBuilding = GC.getInfoTypeForString("BUILDING_TAJ_MAHAL")

	return TRNSLTR.getText("TXT_KEY_EVENT_GUNS_BUTTER_HELP_1", (iNumUnits, GC.getBuildingInfo(iBuilding).getTextKey()))

def canTriggerGunsButterDone(argsList):
	data = argsList[0]
	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1
	return GC.getPlayer(data.ePlayer).getUnitCount(GC.getInfoTypeForString("UNIT_MUSKETMAN")) >= iNumUnits


def canApplyGunsButterDone2(argsList):
	data = argsList[1]
	return GC.getPlayer(data.ePlayer).isCivic(GC.getInfoTypeForString("CIVIC_VASSALAGE"))

def canApplyGunsButterDone3(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  iBuilding = GC.getInfoTypeForString("BUILDING_TAJ_MAHAL")
  if player.getBuildingCountWithUpgrades(iBuilding) == 0:
    return False

  return True

######## NOBLE KNIGHTS ###########

def canTriggerNobleKnights(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  kActualTriggeredDataObject = player.getEventTriggered(data.iId)
  kActualTriggeredDataObject.eReligion = ReligionTypes(player.getStateReligion())

  return True

def getHelpNobleKnights1(argsList):
	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1
	iBuilding = GC.getInfoTypeForString("BUILDING_THE_ORACLE")
	return TRNSLTR.getText("TXT_KEY_EVENT_NOBLE_KNIGHTS_HELP_1", (iNumUnits, GC.getBuildingInfo(iBuilding).getTextKey()))

def canTriggerNobleKnightsDone(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1

  if player.getUnitCount(GC.getInfoTypeForString("UNIT_KNIGHT")) < iNumUnits:
    return False

  trigger = GC.getEventTriggerInfo(data.eTrigger)
  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))
  kActualTriggeredDataObject = player.getEventTriggered(data.iId)
  kActualTriggeredDataObject.eReligion = kOrigTriggeredData.eReligion

  iBuilding = GC.getInfoTypeForString("BUILDING_THE_ORACLE")

  for loopCity in player.cities():
    if loopCity.hasBuilding(iBuilding):
      kActualTriggeredDataObject.iPlotX = loopCity.getX()
      kActualTriggeredDataObject.iPlotY = loopCity.getY()
      kActualTriggeredDataObject.iCityId = loopCity.getID()
      break

  return True

def canApplyNobleKnightsDone2(argsList):
	data = argsList[1]
	return GC.getPlayer(data.ePlayer).isCivic(GC.getInfoTypeForString("CIVIC_STATE_CHURCH"))

######## OVERWHELM DOCTRINE ###########

def canTriggerOverwhelm(argsList):
	data = argsList[0]

	map = GC.getMap()
	iNumWater = 0

	for plot in map.plots():
		if plot.isWater():
			iNumWater += 1
		if 100 * iNumWater >= 55 * map.numPlots():
			return True
	return False

def getHelpOverwhelm1(argsList):
  data = argsList[1]

  iDestroyer = GC.getInfoTypeForString("UNIT_DESTROYER")
  iNumDestroyers = 4
  iBattleship = GC.getInfoTypeForString("UNIT_BATTLESHIP")
  iNumBattleships = 2
  iCarrier = GC.getInfoTypeForString("UNIT_CARRIER")
  iNumCarriers = 3
  iFighter = GC.getInfoTypeForString("SPECIALUNIT_FIGHTER")
  iNumFighters = 9
  iBuilding = GC.getInfoTypeForString("BUILDING_MANHATTAN_PROJECT")

# szHelp = TRNSLTR.getText("TXT_KEY_EVENT_OVERWHELM_HELP_1", (iNumDestroyers, GC.getUnitInfo(iDestroyer).getTextKey(), iNumBattleships, GC.getUnitInfo(iBattleship).getTextKey(), iNumCarriers, GC.getUnitInfo(iCarrier).getTextKey(), iNumFighters, GC.getSpecialUnitInfo(iFighter).getTextKey(), GC.getProjectInfo(iProject).getTextKey()))
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_OVERWHELM_HELP_1", (iNumDestroyers, GC.getUnitInfo(iDestroyer).getTextKey(), iNumBattleships, GC.getUnitInfo(iBattleship).getTextKey(), iNumCarriers, GC.getUnitInfo(iCarrier).getTextKey(), iNumFighters, GC.getSpecialUnitInfo(iFighter).getTextKey(), GC.getBuildingInfo(iBuilding).getTextKey()))

  return szHelp

def canTriggerOverwhelmDone(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  iNumDestroyers = 4
  if player.getUnitCount(GC.getInfoTypeForString("UNIT_DESTROYER")) < iNumDestroyers:
    return False

  iNumBattleships = 2
  if player.getUnitCount(GC.getInfoTypeForString("UNIT_BATTLESHIP")) < iNumBattleships:
    return False

  iNumCarriers = 3
  if player.getUnitCount(GC.getInfoTypeForString("UNIT_CARRIER")) < iNumCarriers:
    return False

  iFighter = GC.getInfoTypeForString("SPECIALUNIT_FIGHTER")
  iNumFighters = 9
  iNumPlayerFighters = 0
  for loopUnit in player.units():
    if loopUnit.getSpecialUnitType() == iFighter:
      iNumPlayerFighters += 1

  if iNumPlayerFighters < iNumFighters:
    return False

  return True

def getHelpOverwhelmDone3(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_OVERWHELM_DONE_HELP_3", ())

def canApplyOverwhelmDone3(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)
  iBuilding = GC.getInfoTypeForString("BUILDING_MANHATTAN_PROJECT")

# if GC.getTeam(player.getTeam()).getProjectCount(iProject) == 0:
  if player.getBuildingCountWithUpgrades(iBuilding) == 0:
    return False

  return True

def applyOverwhelmDone3(argsList):
  GAME.changeNoNukesCount(1)

######## CORPORATE EXPANSION ###########

def canTriggerCorporateExpansion(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  city = GAME.getHeadquarters(data.eCorporation)
  if None == city:
    return False

  # Hack to remember the number of cities you already have with the Corporation
  kActualTriggeredDataObject = player.getEventTriggered(data.iId)
  kActualTriggeredDataObject.iOtherPlayerCityId = GAME.countCorporationLevels(data.eCorporation)
  kActualTriggeredDataObject.iCityId = city.getID()
  kActualTriggeredDataObject.iPlotX = city.getX()
  kActualTriggeredDataObject.iPlotY = city.getY()

  bFound = False
  for iBuilding in xrange(GC.getNumBuildingInfos()):
    if GC.getBuildingInfo(iBuilding).getFoundsCorporation() == data.eCorporation:
      kActualTriggeredDataObject.eBuilding = BuildingTypes(iBuilding)
      bFound = True
      break

  if not bFound:
    return False

  return True

def expireCorporateExpansion1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  city = player.getCity(data.iCityId)
  if None == city:
    return True

  return False

def getHelpCorporateExpansion1(argsList):
	data = argsList[1]
	iNumCities = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1
	return TRNSLTR.getText("TXT_KEY_EVENT_CORPORATE_EXPANSION_HELP_1", (GC.getCorporationInfo(data.eCorporation).getTextKey(), iNumCities))

def canTriggerCorporateExpansionDone(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)
  trigger = GC.getEventTriggerInfo(data.eTrigger)
  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))

  iNumCitiesRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1 + kOrigTriggeredData.iOtherPlayerCityId

  if iNumCitiesRequired > GAME.countCorporationLevels(kOrigTriggeredData.eCorporation):
    return False

  kActualTriggeredDataObject = player.getEventTriggered(data.iId)
  kActualTriggeredDataObject.eCorporation = kOrigTriggeredData.eCorporation
  kActualTriggeredDataObject.eBuilding = kOrigTriggeredData.eBuilding
  kActualTriggeredDataObject.iCityId = kOrigTriggeredData.iCityId
  kActualTriggeredDataObject.iPlotX = kOrigTriggeredData.iPlotX
  kActualTriggeredDataObject.iPlotY = kOrigTriggeredData.iPlotY

  return True

def getHelpCorporateExpansionDone1(argsList):
	data = argsList[1]
	return TRNSLTR.getText("TXT_KEY_EVENT_YIELD_CHANGE_BUILDING", (GC.getBuildingInfo(data.eBuilding).getTextKey(), u"+50[ICON_GOLD]"))

def applyCorporateExpansionDone1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  city = player.getCity(data.iCityId)
  if None != city:
    city.setBuildingCommerceChange(data.eBuilding, CommerceTypes.COMMERCE_GOLD, 50)

######## HOSTILE TAKEOVER ###########

def canTriggerHostileTakeover(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)

	if GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY):
		return False

	city = GAME.getHeadquarters(data.eCorporation)
	if city is None: return False

	# Hack to remember the number of cities you already have with the Corporation
	kActualTriggeredDataObject = player.getEventTriggered(data.iId)
	kActualTriggeredDataObject.iCityId = city.getID()
	kActualTriggeredDataObject.iPlotX = city.getX()
	kActualTriggeredDataObject.iPlotY = city.getY()

	for iBuilding in xrange(GC.getNumBuildingInfos()):
		if GC.getBuildingInfo(iBuilding).getFoundsCorporation() == data.eCorporation:
			kActualTriggeredDataObject.eBuilding = BuildingTypes(iBuilding)
			break
	else: return False

	return getHostileTakeoverListResources(GC.getCorporationInfo(data.eCorporation), player) != []

def expireHostileTakeover1(argsList):
	data = argsList[1]
	return GC.getPlayer(data.ePlayer).getCity(data.iCityId) is None

def getHostileTakeoverListResources(corporation, player):
  listHave = []
  for plot in GC.getMap().plots():
    if plot.getOwner() == player.getID():
      iBonus = plot.getBonusType(player.getTeam())
      if iBonus != -1 and not iBonus in listHave:
          listHave.append(iBonus)
  listNeed = []
  for iBonus in corporation.getPrereqBonuses():
    if not iBonus in listHave:
        listNeed.append(iBonus)
  return listNeed

def getHelpHostileTakeover1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  listResources = getHostileTakeoverListResources(GC.getCorporationInfo(data.eCorporation), player)
  szList = u""
  bFirst = True
  for iBonus in listResources:
    if not bFirst:
      szList += u", "
    else:
      bFirst = False
    szList += u"[COLOR_HIGHLIGHT_TEXT]" + GC.getBonusInfo(iBonus).getDescription() + u"[COLOR_REVERT]"

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_HOSTILE_TAKEOVER_HELP_1", (len(listResources), szList))

  return szHelp

def canTriggerHostileTakeoverDone(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)
  trigger = GC.getEventTriggerInfo(data.eTrigger)
  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))

  listResources = getHostileTakeoverListResources(GC.getCorporationInfo(kOrigTriggeredData.eCorporation), player)

  if len(listResources) > 0:
    return False

  kActualTriggeredDataObject = player.getEventTriggered(data.iId)
  kActualTriggeredDataObject.eCorporation = kOrigTriggeredData.eCorporation
  kActualTriggeredDataObject.eBuilding = kOrigTriggeredData.eBuilding
  kActualTriggeredDataObject.iCityId = kOrigTriggeredData.iCityId
  kActualTriggeredDataObject.iPlotX = kOrigTriggeredData.iPlotX
  kActualTriggeredDataObject.iPlotY = kOrigTriggeredData.iPlotY

  return True

def getHelpHostileTakeoverDone1(argsList):
	data = argsList[1]
	return TRNSLTR.getText("TXT_KEY_EVENT_YIELD_CHANGE_BUILDING", (GC.getBuildingInfo(data.eBuilding).getTextKey(), u"+100[ICON_GOLD]"))

def applyHostileTakeoverDone1(argsList):
	data = argsList[1]
	city = GC.getPlayer(data.ePlayer).getCity(data.iCityId)
	if city is not None:
		city.setBuildingCommerceChange(data.eBuilding, CommerceTypes.COMMERCE_GOLD, 100)


######## Great Beast ########

def doGreatBeast3(argsList):
	data = argsList[1]
	for loopCity in GC.getPlayer(data.ePlayer).cities():
		if loopCity.isHasReligion(data.eReligion):
			loopCity.changeHappinessTimer(40)

def getHelpGreatBeast3(argsList):
	data = argsList[1]
	religion = GC.getReligionInfo(data.eReligion)
	return TRNSLTR.getText("TXT_KEY_EVENT_GREAT_BEAST_3_HELP", (GC.getDefineINT("TEMP_HAPPY"), 40, religion.getChar()))

####### Comet Fragment ########

def canDoCometFragment(argsList):
	data = argsList[0]
	if GC.getPlayer(data.ePlayer).getSpaceProductionModifier() > 10:
		return False
	return True

####### Immigrants ########

def canTriggerImmigrantCity(argsList):
  ePlayer = argsList[1]
  iCity = argsList[2]

  player = GC.getPlayer(ePlayer)
  city = player.getCity(iCity)

  if city is None:
    return False

  if ((city.happyLevel() - city.unhappyLevel(0) < 1) or (city.goodHealth() - city.badHealth(True) < 1)):
    return False

  if (city.getCommerceRateTimes100(CommerceTypes.COMMERCE_CULTURE) < 5500):
    return False

####### Controversial Philosopher ######

def canTriggerControversialPhilosopherCity(argsList):
	city = GC.getPlayer(argsList[1]).getCity(argsList[2])
	return city and city.isCapital() and city.getCommerceRateTimes100(CommerceTypes.COMMERCE_RESEARCH) >= 3500


####### Spy Discovered #######


def canDoSpyDiscovered3(argsList):
	return GC.getPlayer(argsList[1].ePlayer).getCapitalCity() != None

def doSpyDiscovered3(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)

	plot = CyPlayer.getCapitalCity().plot()
	x = plot.getX()
	y = plot.getY()
	iUnitType = GC.getInfoTypeForString("UNIT_TANK")

	iCount = 1 + CyPlayer.getNumCities() / 4
	while iCount > 0:
		CyPlayer.initUnit(iUnitType, x, y, UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)
		iCount -= 1

def getHelpSpyDiscovered3(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_SPY_DISCOVERED_3_HELP", (1+GC.getPlayer(argsList[1].ePlayer).getNumCities()/4, ))

####### Nuclear Protest #######

def canTriggerNuclearProtest(argsList):
	player = GC.getPlayer(argsList[0].ePlayer)
	return 10 <= player.getUnitCount(GC.getInfoTypeForString("UNIT_ICBM")) + player.getUnitCount(GC.getInfoTypeForString("UNIT_TACTICAL_NUKE"))

def doNuclearProtest1(argsList):
	data = argsList[1]

	iICBM = GC.getInfoTypeForString("UNIT_ICBM")
	iTacNuke = GC.getInfoTypeForString("UNIT_TACTICAL_NUKE")

	for loopUnit in GC.getPlayer(data.ePlayer).units():
		if loopUnit.getUnitType() == iICBM or loopUnit.getUnitType() == iTacNuke:
			loopUnit.kill(False, -1)

def getHelpNuclearProtest1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_NUCLEAR_PROTEST_1_HELP", ())


######## Preaching Researcher #######

def canTriggerPreachingResearcherCity(argsList):
	return GC.getPlayer(argsList[1]).getCity(argsList[2]).isActiveBuilding(GC.getInfoTypeForString("BUILDING_UNIVERSITY"))

######## Dissident Priest (Egyptian event) #######

def canTriggerDissidentPriestCity(argsList):
  iCity = argsList[2]

  player = GC.getPlayer(argsList[1])
  city = player.getCity(iCity)

  if city.isGovernmentCenter():
    return False
  if (city.getCommerceRateTimes100(CommerceTypes.COMMERCE_CULTURE) < 3000):
    return False
  if (player.getStateReligion() != -1):
    return False

  return True

######## Impeachment (American event) ########

def canTriggerImpeachmentCity(argsList):
	iCity = argsList[2]
	player = GC.getPlayer(argsList[1])
	return player.getCity(iCity).isCapital()

######## FUTBOL_WAR #######
#Need to stop vassals from being selected as "other player" b/c can't DoW them.  Also, b/c this event railroads other player into war, don't
#want that other player to be a Human in multiplayer game.  (Assuming this event wouldn't OOS in MP anyway.)
#Don't know how the AI will handle the event, but we'll see.  otherPlayer.isHuman() may be redundant as bSinglePlayer = 1 selected in XML.


def canTriggerFutbol_War(argslist):
  data = argslist[0]
  pPlayer = GC.getPlayer(data.ePlayer)
  otherPlayer = GC.getPlayer(data.eOtherPlayer)

  if not GC.getTeam(otherPlayer.getTeam()).canChangeWarPeace(pPlayer.getTeam()):
    return False
  if GC.getTeam(otherPlayer.getTeam()).isAVassal():
    return False
  if GC.getTeam(pPlayer.getTeam()).isAVassal():
    return False
  if otherPlayer.isHuman():
    return False

  return True

######## NAPALM ########
#Pacifist civs won't be inventing napalm bombs

def canTriggerNapalm(argsList):
  data = argsList[0]
  return not GC.getPlayer(data.ePlayer).isCivic(GC.getInfoTypeForString("CIVIC_PACIFISM"))

def getHelpNapalm1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_NAPALM_HELP_1", ())

def applyNapalm1(argsList):
  data = argsList[1]

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != data.ePlayer:
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(data.ePlayer).getTeam()):
        loopPlayer.AI_changeAttitudeExtra(data.ePlayer, -1)

def getHelpNapalm2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_NAPALM_HELP_2", ())

######## BILLIONS_AND_BILLIONS ########
#Carl Sagan doesn't like theocracies -- he only likes free speech

def canTriggerBillionsandBillions(argsList):
	data = argsList[0]
	pPlayer = GC.getPlayer(data.ePlayer)

	if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_DIVINE_CULT")) or pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_INQUISITORIAL")):
		return False
	if (pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_DEMOCRACY")) or pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_FEDERALISM"))) and pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_EGALITARIAN")):
		return True

	return False

def getHelpBillionsandBillions2 (argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_HELP_BILLIONS_AND_BILLIONS_2", ())

def canApplyBillionsandBillions2(argsList):
	data = argsList[1]
	return GC.getPlayer(data.ePlayer).getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_HOLLYWOOD")) != 0


################## FREE_ENTERPRISE (Free Market only) ###################

def canTriggerFreeEnterprise(argsList):
	data = argsList[0]
	return GC.getPlayer(data.ePlayer).isCivic(GC.getInfoTypeForString("CIVIC_FREE_MARKET"))

def getHelpFreeEnterprise2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_HELP_FREE_ENTERPRISE_2", ())


#################  FIAT_MONEY ##########################
# Must be poor, can't be running Decentralization

def canTriggerFiatMoney(argsList):
	data = argsList[0]
	pPlayer = GC.getPlayer(data.ePlayer)
	if pPlayer.getGold() > 5000 or pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_BARTER")):
		return False
	return True


############## CROSSING_THE_RUBICON (Greece/Carthage/Rome only) ##################
# Only want Representation and Universal Suffrage, must be one of the classical republican civilizations of the
# Mediterranean, and must take place in the Ancient/classical eras (Pyramid->early Representation folks, this one's for you)
# Must select a unit that is experienced (Now 10XP as of 2/26/08,) and doesn't have any of
# the Morale promotion.  (CAN be an MGL unit, doesn't have to be.)  Must select city, and it must be the capital.
# Civ can't be in anarchy; civ must be eligible to undergo revolution.
# pPlayer can change civics immediately after applyRubicon3; no way around it I could tell b/c per the API, there
# is no way to force RevolutionTimer to a specific setting.  (pls advise if you know a way; I would like to change it)


def canTriggerRubicon(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)
  R = pPlayer.getRevolutionTimer()
  A = pPlayer.isAnarchy()
  HerdRule = GC.getInfoTypeForString("CIVIC_MONARCHY")

  if R>0:
    return False
  if A>0:
    return False
  if pPlayer.canDoCivics(HerdRule)==0:
    return False
  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_DESPOTISM")):
    return False
  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_MONARCHY")):
    return False
  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_TOTALITARIANISM")):
    return False

  return True

def canTriggerRubiconCity(argsList):
	iCity = argsList[2]
	pPlayer = GC.getPlayer(argsList[1])
	city = pPlayer.getCity(iCity)
	return city.isCapital()

def canTriggerRubiconUnit(argsList):
  eTrigger = argsList[0]
  ePlayer = argsList[1]
  iUnit = argsList[2]

  pPlayer = GC.getPlayer(ePlayer)
  unit = pPlayer.getUnit(iUnit)

  if unit is None:
    return False

  if unit.getExperience() < 25:
    return False

  iMorale = GC.getInfoTypeForString("PROMOTION_MORALE")
  if unit.isHasPromotion(iMorale):
    return False

  return True

def getHelpRubicon2(argsList):
	data = argsList[1]
	pPlayer = GC.getPlayer(data.ePlayer)
	unit = pPlayer.getUnit(data.iUnitId)
	iMorale = GC.getInfoTypeForString("PROMOTION_MORALE")
	return TRNSLTR.getText("TXT_KEY_EVENT_CROSSING_THE_RUBICON_2_HELP", (unit.getNameKey(), GC.getPromotionInfo(iMorale).getTextKey()))

def applyRubicon2(argsList):
	data = argsList[1]
	pPlayer2 = GC.getPlayer(data.ePlayer)
	unit2 = pPlayer2.getUnit(data.iUnitId)
	iMorale = GC.getInfoTypeForString("PROMOTION_MORALE")
	unit2.setHasPromotion(iMorale, True)

def getHelpRubicon3(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_CROSSING_THE_RUBICON_3_HELP", (1, ))

def applyRubicon3(argsList):
	data = argsList[1]
	pPlayer3 = GC.getPlayer(data.ePlayer)
	iCivicType = GC.getInfoTypeForString("CIVIC_MONARCHY")
	iCivicOptionType = GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")
	pPlayer3.setCivics(iCivicOptionType, iCivicType)
	pPlayer3.changeAnarchyTurns(2)


####### THE_GENERALS_PUTSCH (France only) #####
#Must be French, must be running Rep or Univ. Suffrage, must be at war more than 10 turns.
#Can't be in situation where Police State is impossible b/c gov't civic forced (e.g., by UN Resolution)

def canTriggerGeneralsPutsch(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)
  otherPlayer = GC.getPlayer(data.eOtherPlayer)
  R = pPlayer.getRevolutionTimer()
  A = pPlayer.isAnarchy()
  PolState = GC.getInfoTypeForString("CIVIC_TOTALITARIANISM")
  Despot = GC.getInfoTypeForString("CIVIC_DESPOTISM")
  HerdRule = GC.getInfoTypeForString("CIVIC_MONARCHY")

  if R > 0:
    return False
  if A > 0:
    return False
  if GC.getTeam(pPlayer.getTeam()).AI_getAtWarCounter(otherPlayer.getTeam()) < 10:
    return False
  if pPlayer.isCivic(Despot):
    return False
  if pPlayer.isCivic(HerdRule):
    return False
  if pPlayer.isCivic(PolState):
    return False
  if pPlayer.canDoCivics(PolState)==0:
    return False

  return True

def canTriggerGeneralsPutschCity(argsList):
	iCity = argsList[2]
	pPlayer = GC.getPlayer(argsList[1])
	pCity = pPlayer.getCity(iCity)
	return pCity.isGovernmentCenter()

def getHelpGeneralsPutsch1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_THE_GENERALS_PUTSCH_1_HELP", ())

def canDoGeneralsPutsch1(argsList):
  data = argsList[1]
  pPlayer = GC.getPlayer(data.ePlayer)
  iEiffel = GC.getInfoTypeForString("BUILDING_EIFFEL_TOWER")
  iBroadcast = GC.getInfoTypeForString("BUILDING_BROADCAST_TOWER")
  EifCnt = pPlayer.getBuildingCountWithUpgrades(iEiffel)
  BrodCnt = pPlayer.getBuildingCountWithUpgrades(iBroadcast)
  bAggregate = EifCnt + BrodCnt

  if bAggregate < 1:
    return False

  return True

def getHelpGeneralsPutsch3(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_THE_GENERALS_PUTSCH_3_HELP", ())

def applyGeneralsPutsch3(argsList):
	GC.getPlayer(argsList[1].ePlayer).setCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"), GC.getInfoTypeForString("CIVIC_TOTALITARIANISM"))

######  CURE_FOR_CANCER ########

def canTriggerCureforCancer(argsList):
	data = argsList[0]
	pPlayer = GC.getPlayer(data.ePlayer)

	if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_DIVINE_CULT")):
		return False
	if pPlayer.getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_NATIONAL_MEDICAL_DATABASE")) == 0:
		return False

	return True

def getHelpCureforCancer1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_CURE_FOR_CANCER_HELP_1", (1, ))

def applyCureforCancer1(argsList):
	data = argsList[1]
	for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
		loopPlayer = GC.getPlayer(iPlayer)
		if loopPlayer.isAlive() and iPlayer != data.ePlayer:
			loopTeam = GC.getTeam(loopPlayer.getTeam())
			if loopTeam.isHasMet(GC.getPlayer(data.ePlayer).getTeam()):
				loopPlayer.AI_changeAttitudeExtra(data.ePlayer, 1)



####### CARNATION_REVOLUTION ########


def canTriggerCarnation(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)
  otherPlayer = GC.getPlayer(data.eOtherPlayer)
  R = pPlayer.getRevolutionTimer()
  A = pPlayer.isAnarchy()
  Rep = GC.getInfoTypeForString("CIVIC_REPUBLIC")
  StatProp = GC.getInfoTypeForString("CIVIC_PLANNED")

  if R > 0:
    return False
  if A > 0:
    return False
  if GC.getTeam(pPlayer.getTeam()).AI_getAtWarCounter(otherPlayer.getTeam()) < 10:
    return False
  if not GC.getTeam(otherPlayer.getTeam()).canChangeWarPeace(pPlayer.getTeam()):
    return False
  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_REPUBLIC")):
    return False
  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_FEDERALISM")):
    return False
  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_PLANNED")):
    return False
  if pPlayer.canDoCivics(Rep)==0:
    return False
  if pPlayer.canDoCivics(StatProp)==0:
    return False

  return True

def canTriggerCarnationUnit(argsList):
  ePlayer = argsList[1]
  iUnit = argsList[2]

  unit = GC.getPlayer(ePlayer).getUnit(iUnit)

  if unit is None:
    return False

  if unit.getExperience() < 50:
    return False

  # do not use this event in multiplayer
  if GAME.isNetworkMultiPlayer():
    return False

  return True



def doCarnation2(argsList):
  data = argsList[1]
  destPlayer = GC.getPlayer(data.eOtherPlayer)
  pPlayer = GC.getPlayer(data.ePlayer)
  iCivicType1 = GC.getInfoTypeForString("CIVIC_REPUBLIC")
  iCivicOptionType1 = GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")
  iCivicType3 = GC.getInfoTypeForString("CIVIC_PLANNED")
  iCivicOptionType3 = GC.getInfoTypeForString("CIVICOPTION_ECONOMY")

  if destPlayer.isHuman():
    # this works only because it's a single-player only event
    popupInfo = CyPopupInfo()
    popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
    popupInfo.setText(TRNSLTR.getText("TXT_KEY_EVENT_CARNATION_REVOLUTION_OTHER_2", (pPlayer.getCivilizationAdjectiveKey(), )))
    popupInfo.setData1(data.eOtherPlayer)
    popupInfo.setData2(data.ePlayer)
    popupInfo.setPythonModule("CvRandomEventInterface")
    popupInfo.setOnClickedPythonCallback("Carnation2Callback")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_YES", ()), "")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_NO", ()), "")
    popupInfo.addPopup(data.eOtherPlayer)
  else:
    destPlayer.forcePeace(data.ePlayer)
    pPlayer.setCivics(iCivicOptionType1, iCivicType1)
    pPlayer.setCivics(iCivicOptionType3, iCivicType3)
    pPlayer.changeAnarchyTurns(2)


def Carnation2Callback(argsList):
  iButton = argsList[0]
  iData1 = argsList[1]
  iData2 = argsList[2]
  iData3 = argsList[3]
  szText = argsList[4]
  bOption1 = argsList[5]
  bOption2 = argsList[6]

  if iButton == 0:
    destPlayer = GC.getPlayer(iData1)
    player = GC.getPlayer(iData2)
    destPlayer.forcePeace(iData2)
    destPlayer.AI_changeAttitudeExtra(iData2, 1)
    player.AI_changeAttitudeExtra(iData1, 1)

  return 0

def getHelpCarnation2(argsList):
	data = argsList[1]
	destPlayer = GC.getPlayer(data.eOtherPlayer)
	return TRNSLTR.getText("TXT_KEY_EVENT_CARNATION_REVOLUTION_HELP_2", (1, destPlayer.getNameKey()))


#####  SYNTHETIC_FUELS  #####

def canTriggerSyntheticFuels(argsList):
	data = argsList[0]
	pPlayer = GC.getPlayer(data.ePlayer)

	if (
		not pPlayer.hasBonus(GC.getInfoTypeForString("BONUS_COAL"))
	or pPlayer.getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_HQ_STANDARD_ETHANOL")) > 0
	):
		return False

	eOil = GC.getInfoTypeForString("BONUS_OIL")
	if pPlayer.hasBonus(eOil):
		return False
	for i in xrange(GC.getNumBuildingInfos()):
		building = GC.getBuildingInfo(i)
		if eOil in building.getFreeBonuses() and pPlayer.hasBuilding(i):
			return False
	return True

def canTriggerCitySyntheticFuels(argsList):
	return not GC.getPlayer(argsList[1]).getCity(argsList[2]).isGovernmentCenter()

def getHelpSyntheticFuels1(argsList):
	data = argsList[1]
	return TRNSLTR.getText("TXT_KEY_EVENT_SYNTHETIC_FUELS_HELP_1", ( 1, GC.getBonusInfo(GC.getInfoTypeForString("BONUS_OIL")).getChar(), GC.getPlayer(data.ePlayer).getCity(data.iCityId).getNameKey()))

def getHelpSyntheticFuels2(argsList):
	data = argsList[1]
	return TRNSLTR.getText("TXT_KEY_EVENT_SYNTHETIC_FUELS_HELP_2", ( 1, GC.getBonusInfo(GC.getInfoTypeForString("BONUS_OIL")).getChar(), GC.getPlayer(data.ePlayer).getCity(data.iCityId).getNameKey()))

def getHelpSyntheticFuels3(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_SYNTHETIC_FUELS_HELP_3", (1, ))

def getHelpSyntheticFuels4(argsList):
	data = argsList[1]
	pCity = GC.getPlayer(data.ePlayer).getCity(data.iCityId)
	oBonus = GC.getInfoTypeForString("BONUS_PETROLEUM_PRODUCTS")
	return TRNSLTR.getText("TXT_KEY_EVENT_SYNTHETIC_FUELS_HELP_4", (1, GC.getBonusInfo(oBonus).getChar(), pCity.getNameKey()))

def doSyntheticFuels4(argsList):
	GC.getPlayer(argsList[1].ePlayer).getCity(
		argsList[1].iCityId
		).changeFreeBonus(
			GC.getInfoTypeForString("BONUS_PETROLEUM_PRODUCTS"), 1
			)
	return 1


####### ALTERNATIVE_ENERGY ######

def canTriggerAlternativeEnergy(argsList):
	CyPlayer = GC.getPlayer(argsList[0].ePlayer)
	if CyPlayer.getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_THREE_GORGES_DAM")):
		return False
	if not CyPlayer.getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_COAL_PLANT")):
		return False
	return True


def getHelpAlternativeEnergy1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_ALTERNATIVE_ENERGY_HELP_1", (GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(), ))

def expireAlternativeEnergy1(argsList):
	return GC.getPlayer(argsList[1].ePlayer).getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_THREE_GORGES_DAM"))


def canTriggerAlternativeEnergyDone(argsList):
	data = argsList[0]
	trigger = GC.getEventTriggerInfo(data.eTrigger)
	pPlayer = GC.getPlayer(data.ePlayer)
	i3Gorges = GC.getInfoTypeForString("BUILDING_THREE_GORGES_DAM")

	iNuke = GC.getInfoTypeForString("BUILDING_NUCLEAR_PLANT")
	iHydro = GC.getInfoTypeForString("BUILDING_HYDRO_PLANT")
	iTotal = iNuke+iHydro
	iBuildingsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

	if iBuildingsRequired > (pPlayer.getBuildingCountWithUpgrades(iHydro) + pPlayer.getBuildingCountWithUpgrades(iNuke)):
		return False

	return True

def canDoAlternativeEnergy4(argsList):
	data = argsList[1]
	return GC.getPlayer(data.ePlayer).isCivic(GC.getInfoTypeForString("CIVIC_GREEN"))

def getHelpAlternativeEnergy4(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_ALTERNATIVE_ENERGY_4_HELP", ())


#################  MORE_FIAT_MONEY ##########################
# Must be poor, can't be running Decentralization; same as Fiat_Money

def canTriggerMoreFiatMoney(argsList):
	data = argsList[0]
	pPlayer = GC.getPlayer(data.ePlayer)

	if pPlayer.getGold() > 5000:
		return False
	if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_BARTER")):
		return False

	return True



##### INDUSTRIAL_ACTION #####

def canTriggerIndustrialAction(argsList):
	return not GC.getPlayer(argsList[0].ePlayer).hasBuilding(GC.getInfoTypeForString("BUILDING_WORLDVIEW_SLAVERY_ACTIVE"))

def canDoTriggerCityIndustrialAction(argsList):
	iCity = argsList[2]
	pPlayer = GC.getPlayer(argsList[1])
	pCity = pPlayer.getCity(iCity)
	return not pCity.isGovernmentCenter()

def canDoIndustrialAction2(argsList):
	data = argsList[1]
	return not GC.getPlayer(data.ePlayer).isCivic(GC.getInfoTypeForString("CIVIC_PLANNED"))

def getHelpIndustrialAction2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_INDUSTRIAL_ACTION_2_HELP", ())

def getHelpIndustrialAction3(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_INDUSTRIAL_ACTION_3_HELP", ())


#####  DARWINS_VOYAGE  #####


def canTriggerDarwinsVoyage(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)
  map = GC.getMap()

  pPlayerDD = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_DESTROYER"))
  pPlayerBB = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_BATTLESHIP"))
  pPlayerCV = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_CARRIER"))
  pPlayerFF = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_FRIGATE"))
  pPlayerSoL = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_SHIP_OF_THE_LINE"))
  pPlayerIC = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_IRONCLAD"))
  pPlayerSDD = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_STEALTH_DESTROYER"))
  pPlayerMC = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_MISSILE_CRUISER"))
  pPlayerSSN = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_ATTACK_SUBMARINE"))
  pPlayerSSBN = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_SUBMARINE"))
  pPlayerPT = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_PRIVATEER"))
  pPlayerTT = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_TRANSPORT"))
  pPlayerGL = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_GALLEON"))
  pPlayerCY = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_CARAVEL"))
  pAggregate = pPlayerPT + pPlayerDD + pPlayerBB + pPlayerCV + pPlayerFF + pPlayerSoL + pPlayerIC + pPlayerSDD + pPlayerMC + pPlayerSSN + pPlayerSSBN
  pTransports = pPlayerTT + pPlayerGL + pPlayerCY

  if not pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_SECULARISM")):
    return False
  if pTransports < 1:
    return False
  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    if pAggregate < 4:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    if pAggregate < 5:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    if pAggregate < 6:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    if pAggregate < 7:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    if pAggregate < 8:
      return False
  else:
    if pAggregate < 9:
      return False

  return True



######## THE_BUCCANEERS ###########

def canTriggerTheBuccaneers(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

#   At least one civ on the board must know Gunpowder.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_GUNPOWDER")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Astronomy.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_ASTRONOMY")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False


def getHelpTheBuccaneers1(argsList):
	return ("TXT_KEY_EVENT_THE_BUCCANEERS_HELP_1", ())


def applyTheBuccaneers1(argsList):
  data = argsList[1]
  pPlayer = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Buccaneers event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnits  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnits  = 4
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnits  = 6
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnits  = 8
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnits  = 10
  else:
    iNumUnits  = 30

  iUnitType = GC.getInfoTypeForString("UNIT_FRENCH_MUSKETEER")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## BLACKBEARD ###########

def canTriggerBlackbeard(argsList):
	# If Barbarians are disabled in this game, this event will not occur.
	if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
		return False

	iPlayer = argsList[0].ePlayer
	player = GC.getPlayer(iPlayer)
	# iPlayer must have less than a variable number of combat ships based on map size.
	# Triremes, Galleys, Caravels/Carracks, Galleons/East Indiamen, Transports don't count.
	# I've included the modern ships just to prevent anomalous triggering.
	pPlayerPT = player.getUnitCount(GC.getInfoTypeForString("UNIT_PRIVATEER"))
	pPlayerDD = player.getUnitCount(GC.getInfoTypeForString("UNIT_DESTROYER"))
	pPlayerBB = player.getUnitCount(GC.getInfoTypeForString("UNIT_BATTLESHIP"))
	pPlayerCV = player.getUnitCount(GC.getInfoTypeForString("UNIT_CARRIER"))
	pPlayerFF = player.getUnitCount(GC.getInfoTypeForString("UNIT_FRIGATE"))
	pPlayerSoL = player.getUnitCount(GC.getInfoTypeForString("UNIT_SHIP_OF_THE_LINE"))
	pPlayerIC = player.getUnitCount(GC.getInfoTypeForString("UNIT_IRONCLAD"))
	pPlayerSDD = player.getUnitCount(GC.getInfoTypeForString("UNIT_STEALTH_DESTROYER"))
	pPlayerMC = player.getUnitCount(GC.getInfoTypeForString("UNIT_MISSILE_CRUISER"))
	pPlayerSSN = player.getUnitCount(GC.getInfoTypeForString("UNIT_ATTACK_SUBMARINE"))
	pPlayerSSBN = player.getUnitCount(GC.getInfoTypeForString("UNIT_SUBMARINE"))
	pAggregate = pPlayerPT + pPlayerDD + pPlayerBB + pPlayerCV + pPlayerFF + pPlayerSoL + pPlayerIC + pPlayerSDD + pPlayerMC + pPlayerSSN + pPlayerSSBN

	MAP = GC.getMap()
	iWorldSize = MAP.getWorldSize() + 1
	if pAggregate > 5 + iWorldSize*iWorldSize - iWorldSize:
		return False

	iMaxPlayers = GC.getMAX_PC_PLAYERS()
	# At least one civ on the board must know Chemistry.
	iTech = GC.getInfoTypeForString("TECH_CHEMISTRY")
	for iPlayerX in xrange(iMaxPlayers):
		playerX = GC.getPlayer(iPlayerX)
		if playerX.isAlive() and GC.getTeam(playerX.getTeam()).isHasTech(iTech):
			break
	else: return False

	# At least one civ on the board must know Astronomy.
	iTech = GC.getInfoTypeForString("TECH_ASTRONOMY")
	for iPlayerX in xrange(iMaxPlayers):
		playerX = GC.getPlayer(iPlayerX)
		if playerX.isAlive() and GC.getTeam(playerX.getTeam()).isHasTech(iTech):
			break
	else: return False

	# Find an eligible plot
	for plot in MAP.plots():
		if plot.getOwner() == -1 and plot.isWater() and not plot.isLake() \
		and not plot.isImpassable() and plot.getNumUnits() == 0 \
		and plot.isAdjacentPlayer(iPlayer, True):
			return True

	return False

def getHelpBlackbeard1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_BLACKBEARD_HELP_1", ())

def applyBlackbeard1(argsList):
	iPlayer = argsList[1].ePlayer

	plots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if plot.getOwner() == -1 and plot.isWater() and not plot.isLake() \
		and not plot.isImpassable() and plot.getNumUnits() == 0 \
		and plot.isAdjacentPlayer(iPlayer, True):
			plots.append(i)

	if not plots:
		return

	plot = MAP.plotByIndex(plots[GAME.getSorenRandNum(len(plots), "Blackbeard event location")])

	barb = GC.getPlayer(GC.getBARBARIAN_PLAYER())
	iWorldSize = MAP.getWorldSize() + 1
	iUnitType1 = GC.getInfoTypeForString("UNIT_PRIVATEER")
	iUnitType2 = GC.getInfoTypeForString("UNIT_FRIGATE")

	for i in xrange(iWorldSize):
		barb.initUnit(iUnitType1, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_SEA, DirectionTypes.DIRECTION_SOUTH)
	for i in xrange(iWorldSize / 2):
		barb.initUnit(iUnitType2, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_SEA, DirectionTypes.DIRECTION_SOUTH)


######## PIRATES_OF_THE_NEUTRAL_ZONES ###########

def canTriggerPiratesoftheNeutralZones(argsList):
	# If Barbarians are disabled in this game, this event will not occur.
	if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
		return False

	# At least one civ on the board must know Stealth
	iTech = GC.getInfoTypeForString("TECH_STEALTH")
	for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
		CyPlayerX = GC.getPlayer(iPlayerX)
		if CyPlayerX.isAlive():
			if GC.getTeam(CyPlayerX.getTeam()).isHasTech(iTech):
				break
	else: return False

	# Compare player vs pirate strength
	iPlayer = argsList[0].ePlayer
	CyPlayer = GC.getPlayer(iPlayer)
	iNavy = 0
	for CyUnit in CyPlayer.units():
		if CyUnit.getDomainType() == DomainTypes.DOMAIN_SEA:
			iNavy += CyUnit.baseCombatStr()

	iPirate = GC.getUnitInfo(GC.getInfoTypeForString("UNIT_STEALTH_DESTROYER")).getCombat()

	MAP = GC.getMap()
	iPirate = (MAP.getWorldSize() + 1 + 2*(CyPlayer.getHandicapType() + 1)) * iPirate - iPirate
	if iNavy > iPirate:
		return False

	# Find an eligible plot
	for plot in MAP.plots():
		if plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.isLake() and plot.isAdjacentPlayer(iPlayer, True) and not plot.getNumUnits():
			break
	else: return False

	return True

def getHelpPiratesoftheNeutralZones1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_PIRATES_OF_THE_NEUTRAL_ZONES_HELP_1", ())

def applyPiratesoftheNeutralZones1(argsList):
	iPlayer = argsList[1].ePlayer

	MAP = GC.getMap()
	plots = []
	for plot in MAP.plots():
		if plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.isLake() and plot.isAdjacentPlayer(iPlayer, True) and not plot.getNumUnits():
			plots.append(plot)

	if not plots:
		return

	plot = plots[GAME.getSorenRandNum(len(plots), "PiratesoftheNeutralZones event location")]

	iNumUnits = MAP.getWorldSize() + 1 + 2*(GC.getPlayer(iPlayer).getHandicapType() + 1)

	iUnit = GC.getInfoTypeForString("UNIT_STEALTH_DESTROYER")
	iNav = GC.getInfoTypeForString("PROMOTION_NAVIGATION1")
	barbarian = GC.getPlayer(GC.getBARBARIAN_PLAYER())
	x = plot.getX()
	y = plot.getY()
	for i in xrange(iNumUnits):
		CyUnit = barbarian.initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_SEA, DirectionTypes.DIRECTION_SOUTH)
		CyUnit.setHasPromotion(iNav, True)
		CyUnit.setName("Pirate Corvette")


######## MALACCAN_PIRATES ###########

def canTriggerMalaccanPirates(argsList):

  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

###     data.ePlayer must have less than a variable number of combat ships based on map size
###     Galleys & Triremes & Galleons/East Indiamen & Caravels/Carracks & Transports & privateers & frigates & ships of the line don't count.
###     I've included the modern ships just to prevent anomalous triggering.

  pPlayerDD = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_DESTROYER"))
  pPlayerBB = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_BATTLESHIP"))
  pPlayerCV = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_CARRIER"))
  pPlayerIC = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_IRONCLAD"))
  pPlayerSDD = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_STEALTH_DESTROYER"))
  pPlayerMC = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_MISSILE_CRUISER"))
  pPlayerSSN = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_ATTACK_SUBMARINE"))
  pPlayerSSBN = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_SUBMARINE"))
  pAggregate =  pPlayerDD + pPlayerBB + pPlayerCV + pPlayerIC + pPlayerSDD + pPlayerMC + pPlayerSSN + pPlayerSSBN

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    if pAggregate > 2:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    if pAggregate > 3:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    if pAggregate > 4:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    if pAggregate > 5:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    if pAggregate > 6:
      return False
  else:
    if pAggregate > 7:
      return False


#   At least one civ on the board must know Automatic weapons
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_AUTOMATIC_WEAPONS")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False


#   At least one civ on the board must know Radio
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_RADIO")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False

def getHelpMalaccanPirates1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_MALACCAN_PIRATES_HELP_1", ())

def applyMalaccanPirates1(argsList):
  data = argsList[1]
  pPlayer = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "MalaccanPirates event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnit1  = 5
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnit1  = 6
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnit1  = 7
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnit1  = 8
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnit1  = 9
  else:
    iNumUnit1  = 10

  iUnitType1 = GC.getInfoTypeForString("UNIT_STEAM_DESTROYER")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  iNav1 = GC.getInfoTypeForString("PROMOTION_NAVIGATION1")
  iCbt4 = GC.getInfoTypeForString("PROMOTION_COMBAT4")
  iCoAs1 = GC.getInfoTypeForString("PROMOTION_COASTAL_ASSAULT1")
  unit = i in xrange(iNumUnit1)
  for i in xrange(iNumUnit1):
    barbPlayer.initUnit(iUnitType1, plot.getX(), plot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH)

  for loopUnit in barbPlayer.units():
    if loopUnit.getUnitType() == iUnitType1:
      loopUnit.setHasPromotion(iNav1, True)
      loopUnit.setHasPromotion(iCbt4, True)
      loopUnit.setHasPromotion(iCoAs1, True)
      loopUnit.setName("Malaccan Gunboat")


######## HENRY_MORGAN ###########

def canTriggerHenryMorgan(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

###     data.ePlayer must have less than a variable number of combat ships based on map size
###     Galleys & Triremes & Galleons/East Indiamen & Caravels/Carracks & Transports don't count.
###     I've included the modern ships just to prevent anomalous triggering.

  pPlayerPT = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_PRIVATEER"))
  pPlayerDD = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_DESTROYER"))
  pPlayerBB = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_BATTLESHIP"))
  pPlayerCV = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_CARRIER"))
  pPlayerFF = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_FRIGATE"))
  pPlayerSoL = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_SHIP_OF_THE_LINE"))
  pPlayerIC = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_IRONCLAD"))
  pPlayerSDD = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_STEALTH_DESTROYER"))
  pPlayerMC = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_MISSILE_CRUISER"))
  pPlayerSSN = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_ATTACK_SUBMARINE"))
  pPlayerSSBN = pPlayer.getUnitCount(GC.getInfoTypeForString("UNIT_SUBMARINE"))
  pAggregate =  pPlayerPT + pPlayerDD + pPlayerBB + pPlayerCV + pPlayerFF + pPlayerSoL + pPlayerIC + pPlayerSDD + pPlayerMC + pPlayerSSN + pPlayerSSBN

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    if pAggregate > 3:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    if pAggregate > 4:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    if pAggregate > 5:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    if pAggregate > 6:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    if pAggregate > 7:
      return False
  else:
    if pAggregate > 8:
      return False

#   At least one civ on the board must know Astronomy
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_ASTRONOMY")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Military Science
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_MILITARY_SCIENCE")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False

def getHelpHenryMorgan1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_HENRY_MORGAN_HELP_1", ())

def applyHenryMorgan1(argsList):
  data = argsList[1]
  pPlayer = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "HenryMorgan event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnit1  = 1
    iNumUnit2  = 1
    iNumUnit3  = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnit1  = 1
    iNumUnit2  = 1
    iNumUnit3  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnit1  = 1
    iNumUnit2  = 2
    iNumUnit3  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnit1  = 1
    iNumUnit2  = 2
    iNumUnit3  = 3
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnit1  = 1
    iNumUnit2  = 2
    iNumUnit3  = 4
  else:
    iNumUnit1  = 1
    iNumUnit2  = 3
    iNumUnit3  = 4

  iUnitType1 = GC.getInfoTypeForString("UNIT_SHIP_OF_THE_LINE")
  iUnitType2 = GC.getInfoTypeForString("UNIT_FRIGATE")
  iUnitType3 = GC.getInfoTypeForString("UNIT_PRIVATEER")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  iCbt4 = GC.getInfoTypeForString("PROMOTION_COMBAT4")
  for i in xrange(iNumUnit1):
    barbPlayer.initUnit(iUnitType1, plot.getX(), plot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH)
  for i in xrange(iNumUnit2):
    barbPlayer.initUnit(iUnitType2, plot.getX(), plot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH)
  for i in xrange(iNumUnit3):
    barbPlayer.initUnit(iUnitType3, plot.getX(), plot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH)

  for loopUnit in barbPlayer.units():
    if loopUnit.getUnitType() == iUnitType1:
      loopUnit.setHasPromotion(iCbt4, True)

######## STEDE_BONNET ###########

def canTriggerStedeBonnet(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

###     data.ePlayer must have less than a variable number of combat ships based on map size
###     Galleys & Triremes don't count.
###     I've included the modern ships just to prevent anomalous triggering.

  iCaravel = GC.getInfoTypeForString("UNIT_CARAVEL")
  iGalleon = GC.getInfoTypeForString("UNIT_GALLEON")
  iTransport = GC.getInfoTypeForString("UNIT_TRANSPORT")
  iPrivateer = GC.getInfoTypeForString("UNIT_PRIVATEER")
  iDestroyer = GC.getInfoTypeForString("UNIT_DESTROYER")
  iBattleship = GC.getInfoTypeForString("UNIT_BATTLESHIP")
  iCarrier = GC.getInfoTypeForString("UNIT_CARRIER")
  iFrigate = GC.getInfoTypeForString("UNIT_FRIGATE")
  iSoL = GC.getInfoTypeForString("UNIT_SHIP_OF_THE_LINE")
  iIronclad = GC.getInfoTypeForString("UNIT_IRONCLAD")
  iStealth = GC.getInfoTypeForString("UNIT_STEALTH_DESTROYER")
  iMissile = GC.getInfoTypeForString("UNIT_MISSILE_CRUISER")
  iBoomer = GC.getInfoTypeForString("UNIT_SUBMARINE")
  iSeawolf = GC.getInfoTypeForString("UNIT_ATTACK_SUBMARINE")

  pPlayerCL = pPlayer.getUnitCount(iCaravel)
  pPlayerGN = pPlayer.getUnitCount(iGalleon)
  pPlayerTP = pPlayer.getUnitCount(iTransport)
  pPlayerPT = pPlayer.getUnitCount(iPrivateer)
  pPlayerDD = pPlayer.getUnitCount(iDestroyer)
  pPlayerBB = pPlayer.getUnitCount(iBattleship)
  pPlayerCV = pPlayer.getUnitCount(iCarrier)
  pPlayerFF = pPlayer.getUnitCount(iFrigate)
  pPlayerSoL = pPlayer.getUnitCount(iSoL)
  pPlayerIC = pPlayer.getUnitCount(iIronclad)
  pPlayerSDD = pPlayer.getUnitCount(iStealth)
  pPlayerMC = pPlayer.getUnitCount(iMissile)
  pPlayerSSN = pPlayer.getUnitCount(iSeawolf)
  pPlayerSSBN = pPlayer.getUnitCount(iBoomer)
  pAggregate = pPlayerCL + pPlayerGN + pPlayerTP + pPlayerPT + pPlayerDD + pPlayerBB + pPlayerCV + pPlayerFF + pPlayerSoL + pPlayerIC + pPlayerSDD + pPlayerMC + pPlayerSSN + pPlayerSSBN

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    if pAggregate > 2:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    if pAggregate > 3:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    if pAggregate > 4:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    if pAggregate > 5:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    if pAggregate > 6:
      return False
  else:
    if pAggregate > 7:
      return False

#   At least one civ on the board must know Astronomy
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_ASTRONOMY")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Gunpowder
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_GUNPOWDER")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False

def getHelpStedeBonnet1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_STEDE_BONNET_HELP_1", ())

def applyStedeBonnet1(argsList):
  data = argsList[1]
  pPlayer = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "StedeBonnet event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnit1  = 1
    iNumUnit2  = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnit1  = 2
    iNumUnit2  = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnit1  = 2
    iNumUnit2  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnit1  = 3
    iNumUnit2  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnit1  = 3
    iNumUnit2  = 3
  else:
    iNumUnit1  = 4
    iNumUnit2  = 3

  iUnitType1 = GC.getInfoTypeForString("UNIT_PORTUGAL_CARRACK")
  iUnitType2 = GC.getInfoTypeForString("UNIT_NETHERLANDS_OOSTINDIEVAARDER")
  iNav1 = GC.getInfoTypeForString("PROMOTION_NAVIGATION1")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnit1):
    barbPlayer.initUnit(iUnitType1, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_SEA, DirectionTypes.DIRECTION_SOUTH)
  for i in xrange(iNumUnit2):
    barbPlayer.initUnit(iUnitType2, plot.getX(), plot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH)

  for loopUnit in barbPlayer.units():
    if loopUnit.getUnitType() == iUnitType1:
      loopUnit.setName("Barque")
    if loopUnit.getUnitType() == iUnitType2:
      loopUnit.setName("Fast Galleon")
      loopUnit.setHasPromotion(iNav1, True)

######## THE_CORSAIRS ###########

def canTriggerTheCorsairs(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

###     data.ePlayer must have less than a variable number of combat ships based on map size
###     Galleys & Triremes don't count.
###     I've included the modern ships just to prevent anomalous triggering.

  iCaravel = GC.getInfoTypeForString("UNIT_CARAVEL")
  iGalleon = GC.getInfoTypeForString("UNIT_GALLEON")
  iTransport = GC.getInfoTypeForString("UNIT_TRANSPORT")
  iPrivateer = GC.getInfoTypeForString("UNIT_PRIVATEER")
  iDestroyer = GC.getInfoTypeForString("UNIT_DESTROYER")
  iBattleship = GC.getInfoTypeForString("UNIT_BATTLESHIP")
  iCarrier = GC.getInfoTypeForString("UNIT_CARRIER")
  iFrigate = GC.getInfoTypeForString("UNIT_FRIGATE")
  iSoL = GC.getInfoTypeForString("UNIT_SHIP_OF_THE_LINE")
  iIronclad = GC.getInfoTypeForString("UNIT_IRONCLAD")
  iStealth = GC.getInfoTypeForString("UNIT_STEALTH_DESTROYER")
  iMissile = GC.getInfoTypeForString("UNIT_MISSILE_CRUISER")
  iBoomer = GC.getInfoTypeForString("UNIT_SUBMARINE")
  iSeawolf = GC.getInfoTypeForString("UNIT_ATTACK_SUBMARINE")

  pPlayerCL = pPlayer.getUnitCount(iCaravel)
  pPlayerGN = pPlayer.getUnitCount(iGalleon)
  pPlayerTP = pPlayer.getUnitCount(iTransport)
  pPlayerPT = pPlayer.getUnitCount(iPrivateer)
  pPlayerDD = pPlayer.getUnitCount(iDestroyer)
  pPlayerBB = pPlayer.getUnitCount(iBattleship)
  pPlayerCV = pPlayer.getUnitCount(iCarrier)
  pPlayerFF = pPlayer.getUnitCount(iFrigate)
  pPlayerSoL = pPlayer.getUnitCount(iSoL)
  pPlayerIC = pPlayer.getUnitCount(iIronclad)
  pPlayerSDD = pPlayer.getUnitCount(iStealth)
  pPlayerMC = pPlayer.getUnitCount(iMissile)
  pPlayerSSN = pPlayer.getUnitCount(iSeawolf)
  pPlayerSSBN = pPlayer.getUnitCount(iBoomer)
  pAggregate = pPlayerCL + pPlayerGN + pPlayerTP + pPlayerPT + pPlayerDD + pPlayerBB + pPlayerCV + pPlayerFF + pPlayerSoL + pPlayerIC + pPlayerSDD + pPlayerMC + pPlayerSSN + pPlayerSSBN

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    if pAggregate > 2:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    if pAggregate > 2:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    if pAggregate > 3:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    if pAggregate > 3:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    if pAggregate > 4:
      return False
  else:
    if pAggregate > 4:
      return False

#   At least one civ on the board must know Optics
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_OPTICS")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False

def getHelpTheCorsairs1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_THE_CORSAIRS_HELP_1", ())

def applyTheCorsairs1(argsList):
  data = argsList[1]
  pPlayer = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Corsairs event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnit1  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnit1  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnit1  = 3
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnit1  = 4
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnit1  = 5
  else:
    iNumUnit1  = 6

  iUnitType1 = GC.getInfoTypeForString("UNIT_PORTUGAL_CARRACK")


  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnit1):
    barbPlayer.initUnit(iUnitType1, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_SEA, DirectionTypes.DIRECTION_SOUTH)

  for loopUnit in barbPlayer.units():
    if loopUnit.getUnitType() == iUnitType1:
      sUnitName = TRNSLTR.getText("TXT_KEY_EVENT_THE_CORSAIRS_UNIT_NAME", ())
      loopUnit.setName(sUnitName)


######## ILLYRIAN_PIRATES ###########

def canTriggerIllyrianPirates(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

###     data.ePlayer must have less than a variable number of combat ships based on map size
###     Galleys don't count.
###     I've included the modern ships just to prevent anomalous triggering.

  iWarGalley = GC.getInfoTypeForString("UNIT_WARGALLEY")
  iTrireme = GC.getInfoTypeForString("UNIT_TRIREME")
  iCaravel = GC.getInfoTypeForString("UNIT_CARAVEL")
  iGalleon = GC.getInfoTypeForString("UNIT_GALLEON")
  iTransport = GC.getInfoTypeForString("UNIT_TRANSPORT")
  iPrivateer = GC.getInfoTypeForString("UNIT_PRIVATEER")
  iDestroyer = GC.getInfoTypeForString("UNIT_DESTROYER")
  iBattleship = GC.getInfoTypeForString("UNIT_BATTLESHIP")
  iCarrier = GC.getInfoTypeForString("UNIT_CARRIER")
  iFrigate = GC.getInfoTypeForString("UNIT_FRIGATE")
  iSoL = GC.getInfoTypeForString("UNIT_SHIP_OF_THE_LINE")
  iIronclad = GC.getInfoTypeForString("UNIT_IRONCLAD")
  iStealth = GC.getInfoTypeForString("UNIT_STEALTH_DESTROYER")
  iMissile = GC.getInfoTypeForString("UNIT_MISSILE_CRUISER")
  iBoomer = GC.getInfoTypeForString("UNIT_SUBMARINE")
  iSeawolf = GC.getInfoTypeForString("UNIT_ATTACK_SUBMARINE")

  pPlayerWG = pPlayer.getUnitCount(iWarGalley)
  pPlayerTE = pPlayer.getUnitCount(iTrireme)
  pPlayerCL = pPlayer.getUnitCount(iCaravel)
  pPlayerGN = pPlayer.getUnitCount(iGalleon)
  pPlayerTP = pPlayer.getUnitCount(iTransport)
  pPlayerPT = pPlayer.getUnitCount(iPrivateer)
  pPlayerDD = pPlayer.getUnitCount(iDestroyer)
  pPlayerBB = pPlayer.getUnitCount(iBattleship)
  pPlayerCV = pPlayer.getUnitCount(iCarrier)
  pPlayerFF = pPlayer.getUnitCount(iFrigate)
  pPlayerSoL = pPlayer.getUnitCount(iSoL)
  pPlayerIC = pPlayer.getUnitCount(iIronclad)
  pPlayerSDD = pPlayer.getUnitCount(iStealth)
  pPlayerMC = pPlayer.getUnitCount(iMissile)
  pPlayerSSN = pPlayer.getUnitCount(iSeawolf)
  pPlayerSSBN = pPlayer.getUnitCount(iBoomer)
  pAggregate = pPlayerWG + pPlayerTE + pPlayerCL + pPlayerGN + pPlayerTP + pPlayerPT + pPlayerDD + pPlayerBB + pPlayerCV + pPlayerFF + pPlayerSoL + pPlayerIC + pPlayerSDD + pPlayerMC + pPlayerSSN + pPlayerSSBN

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    if pAggregate > 2:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    if pAggregate > 2:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    if pAggregate > 3:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    if pAggregate > 3:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    if pAggregate > 4:
      return False
  else:
    if pAggregate > 4:
      return False

#   At least one civ on the board must know Metal Casting.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_METAL_CASTING")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Sailing.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_SAILING")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False

def getHelpIllyrianPirates1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_ILLYRIAN_PIRATES_HELP_1", ())

def applyIllyrianPirates1(argsList):
  data = argsList[1]
  pPlayer = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "IllyrianPirates event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnit1  = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnit1  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnit1  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnit1  = 3
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnit1  = 4
  else:
    iNumUnit1  = 5

  iUnitType1 = GC.getInfoTypeForString("UNIT_WARGALLEY")
  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnit1):
    barbPlayer.initUnit(iUnitType1, plot.getX(), plot.getY(), UnitAITypes.UNITAI_PIRATE_SEA, DirectionTypes.DIRECTION_SOUTH)

  for loopUnit in barbPlayer.units():
    if loopUnit.getUnitType() == iUnitType1:
      sUnitName = TRNSLTR.getText("TXT_KEY_EVENT_ILLYRIAN_PIRATES_UNIT_NAME", ())
      loopUnit.setName(sUnitName)


######## MAHDI_ARMY ###########

def canTriggerMahdiArmy(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

#   At least one civ on the board must know Flintlock.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_FLINTLOCK")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Liberalism.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_LIBERALISM")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False

def getHelpMahdiArmy1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_MAHDI_ARMY_HELP_1", ())


def applyMahdiArmy1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Mahdi Army event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnits  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnits  = 4
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnits  = 6
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnits  = 8
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnits  = 10
  else:
    iNumUnits  = 30

  iUnitType = GC.getInfoTypeForString("UNIT_MUSKETMAN")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## THE_TALIBAN ###########

def canTriggerTheTaliban(argsList):
  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

#   At least one civ on the board must know Satellites.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_SATELLITES")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

#   At least one civ on the board must know Robotics.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_ROBOTICS")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False

def getHelpTheTaliban1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_THE_TALIBAN_HELP_1", ())


def applyTheTaliban1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "The Taliban event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnits1 = 2
    iNumUnits2 = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnits1 = 4
    iNumUnits2 = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnits1 = 6
    iNumUnits2 = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnits1 = 8
    iNumUnits2 = 3
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnits1 = 10
    iNumUnits2 = 4
  else:
    iNumUnits1 = 15
    iNumUnits2 = 7

  iUnitType1 = GC.getInfoTypeForString("UNIT_GUERRILLA")
  iUnitType2 = GC.getInfoTypeForString("UNIT_AT_INFANTRY")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits1):
    barbPlayer.initUnit(iUnitType1, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)
  for i in xrange(iNumUnits2):
    barbPlayer.initUnit(iUnitType2, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)



######## V_GER ############

def canTriggerV_Ger(argsList):
	data = argsList[0]
	pPlayer = GC.getPlayer(data.ePlayer)
	iObsrvty = GC.getInfoTypeForString("BUILDING_OBSERVATORY")

	if pPlayer.getBuildingCountWithUpgrades(iObsrvty) < 1:
		return False

	if GAME.isVictoryValid(GC.getInfoTypeForString("VICTORY_SPACE_RACE")):
		return True

	return False

######## Sailing founded ############

def canTriggerSailingFounded(argsList):
  ePlayer = argsList[1]
  iCity = argsList[2]

  city = GC.getPlayer(ePlayer).getCity(iCity)

  if city is None:
    return False

  if not city.isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()):
    return False

  if city.plot().getLatitude() <= 0:
    return False

  return True

######## Chariotry founded ############

def canTriggerChariotryFounded(argsList):
	ePlayer = argsList[1]
	iCity = argsList[2]

	city = GC.getPlayer(ePlayer).getCity(iCity)

	if city is None:
		return False

	if city.plot().getLatitude() <= 0:
		return False

	if city.getNumBonuses(GC.getInfoTypeForString("BONUS_HORSE")) < 1:
		return False

	return True

######## MERCENARIES ANCIENT ###########

def canTriggerMercenariesAncient(argsList):
	data = argsList[0]
	CyPlayer = GC.getPlayer(data.ePlayer)

	if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
		return False

	# Can we build the counter unit?
	iUnit = GC.getInfoTypeForString("UNIT_ARCHER")

	CyCity, iter = CyPlayer.firstCity(False)
	while CyCity:
		if CyCity.canTrain(iUnit, False, False, False, False):
			break
		CyCity, iter = CyPlayer.nextCity(iter, False)
	else:
		return False

	# Find an eligible plot
	for plot in GC.getMap().plots():
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(data.ePlayer) > 0
		and plot.isAdjacentPlayer(data.ePlayer, True)
		): return True
	return False


def getHelpMercenariesAncient1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_MERCENARIES_ANCIENT_HELP_1", ())

def applyMercenariesAncient1(argsList):
	data = argsList[1]

	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(data.ePlayer) > 0
		and plot.isAdjacentPlayer(data.ePlayer, True)
		):
			listPlots.append(i)

	if not listPlots:
		return

	plot = MAP.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Mercenary event location")])
	x = plot.getX()
	y = plot.getY()

	iUnit = GC.getInfoTypeForString("UNIT_OBSIDIAN_SWORDSMAN")
	barb = GC.getPlayer(GC.getBARBARIAN_PLAYER())
	iCount = (MAP.getWorldSize() + 5)*2/3

	while iCount > 0:
		CyUnit = barb.initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)
		CyUnit.setName("Mercenary Warrior")
		iCount -= 1


def applyMercenariesAncient2(argsList):
	data = argsList[1]
	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(data.ePlayer) > 0
		and plot.isAdjacentPlayer(data.ePlayer, True)
		):
			listPlots.append(i)

	if not listPlots:
		return

	plot = MAP.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Mercenary event location")])
	x = plot.getX()
	y = plot.getY()

	iUnit = GC.getInfoTypeForString("UNIT_OBSIDIAN_SWORDSMAN")
	iCount = (MAP.getWorldSize() + 5)*2/3

	while iCount > 0:
		CyUnit = GC.getPlayer(data.ePlayer).initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.DIRECTION_SOUTH)
		CyUnit.setName("Mercenary Warrior")
		iCount -= 1

######## MERCENARIES CLASSICAL ###########

def canTriggerMercenariesClassical(argsList):
	data = argsList[0]
	CyPlayer = GC.getPlayer(data.ePlayer)

	if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
		return False

	# Can we build the counter unit?
	iUnit = GC.getInfoTypeForString("UNIT_AXEMAN")

	CyCity, iter = CyPlayer.firstCity(False)
	while CyCity:
		if CyCity.canTrain(iUnit, False, False, False, False):
			break
		CyCity, iter = CyPlayer.nextCity(iter, False)
	else:
		return False

	# Find an eligible plot
	MAP = GC.getMap()
	for plot in MAP.plots():
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(data.ePlayer) > 0
		and plot.isAdjacentPlayer(data.ePlayer, True)
		): return True
	return False

def getHelpMercenariesClassical1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_MERCENARIES_CLASSICAL_HELP_1", ())

def applyMercenariesClassical1(argsList):
	data = argsList[1]

	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(data.ePlayer) > 0
		and plot.isAdjacentPlayer(data.ePlayer, True)
		):
			listPlots.append(i)

	if not listPlots:
		return

	plot = MAP.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Mercenary event location")])
	x = plot.getX()
	y = plot.getY()

	iUnit = GC.getInfoTypeForString("UNIT_AXEMAN")
	barb = GC.getPlayer(GC.getBARBARIAN_PLAYER())
	iCount = (MAP.getWorldSize() + 5)*2/3

	while iCount > 0:
		CyUnit = barb.initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)
		CyUnit.setName("Mercenary Warrior")
		iCount -= 1

def applyMercenariesClassical2(argsList):
	data = argsList[1]
	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(data.ePlayer) > 0
		and plot.isAdjacentPlayer(data.ePlayer, True)
		):
			listPlots.append(i)

	if not listPlots:
		return

	plot = MAP.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Mercenary event location")])
	x = plot.getX()
	y = plot.getY()

	iUnit = GC.getInfoTypeForString("UNIT_AXEMAN")
	iCount = (MAP.getWorldSize() + 5)*2/3

	while iCount > 0:
		CyUnit = GC.getPlayer(data.ePlayer).initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.DIRECTION_SOUTH)
		CyUnit.setName("Mercenary Warrior")
		iCount -= 1

######## MERCENARIES MEDIEVAL ###########

def canTriggerMercenariesMedieval(argsList):
	data = argsList[0]
	CyPlayer = GC.getPlayer(data.ePlayer)

	if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
		return False

	# Can we build the counter unit?
	iUnit = GC.getInfoTypeForString("UNIT_LONGBOWMAN")

	CyCity, iter = CyPlayer.firstCity(False)
	while CyCity:
		if CyCity.canTrain(iUnit, False, False, False, False):
			break
		CyCity, iter = CyPlayer.nextCity(iter, False)
	else:
		return False

	# Find an eligible plot
	MAP = GC.getMap()
	for plot in MAP.plots():
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(data.ePlayer) > 0
		and plot.isAdjacentPlayer(data.ePlayer, True)
		): return True
	return False

def getHelpMercenariesMedieval1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_MERCENARIES_MEDIEVAL_HELP_1", ())

def applyMercenariesMedieval1(argsList):
	data = argsList[1]

	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(data.ePlayer) > 0
		and plot.isAdjacentPlayer(data.ePlayer, True)
		):
			listPlots.append(i)

	if not listPlots:
		return

	plot = MAP.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Mercenary event location")])
	x = plot.getX()
	y = plot.getY()

	iUnit = GC.getInfoTypeForString("UNIT_SWORDSMAN")
	barb = GC.getPlayer(GC.getBARBARIAN_PLAYER())
	iCount = (MAP.getWorldSize() + 5)*2/3

	while iCount > 0:
		CyUnit = barb.initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)
		CyUnit.setName("Mercenary Warrior")
		iCount -= 1

def applyMercenariesMedieval2(argsList):
	data = argsList[1]
	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(data.ePlayer) > 0
		and plot.isAdjacentPlayer(data.ePlayer, True)
		):
			listPlots.append(i)

	if not listPlots:
		return

	plot = MAP.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Mercenary event location")])
	x = plot.getX()
	y = plot.getY()

	iUnit = GC.getInfoTypeForString("UNIT_SWORDSMAN")
	iCount = (MAP.getWorldSize() + 5)*2/3

	while iCount > 0:
		CyUnit = GC.getPlayer(data.ePlayer).initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.DIRECTION_SOUTH)
		CyUnit.setName("Mercenary Warrior")
		iCount -= 1

######## EARTHQUAKE ###########

def getHelpEarthquake1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_EARTHQUAKE_1_HELP", ())

def canApplyEarthquake1(argsList):
	data = argsList[1]

	for iDX in xrange(-1, 2):
		for iDY in xrange(-1, 2):
			plotX = plotXY(data.iPlotX, data.iPlotY, iDX, iDY)
			if plotX and plotX.getImprovementType() != -1:
				return True
	return False

def applyEarthquake1(argsList):
	data = argsList[1]
	plots = []
	iPlots = 0
	for iDX in xrange(-1, 2):
		for iDY in xrange(-1, 2):
			plotX = plotXY(data.iPlotX, data.iPlotY, iDX, iDY)
			if plotX and plotX.getImprovementType() > 1:
				plots.append((plotX, plotX.getImprovementType()))
				iPlots += 1

	if not plots: raise "Event - Error in canApplyVolcano1"

	if iPlots < 3:
		iRange = iPlots
	else: iRange = 3

	listRuins = [
		GC.getInfoTypeForString("IMPROVEMENT_COTTAGE"),
		GC.getInfoTypeForString("IMPROVEMENT_HAMLET"),
		GC.getInfoTypeForString("IMPROVEMENT_VILLAGE"),
		GC.getInfoTypeForString("IMPROVEMENT_TOWN"),
		GC.getInfoTypeForString("IMPROVEMENT_SUBURBS"),
		GC.getInfoTypeForString("IMPROVEMENT_GOODY_HUT")
	]
	iRuins = GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS")

	for i in xrange(iRange):
		if i and GAME.getSorenRandNum(100, "Volcano event num improvements destroyed") < 50:
			break
		plot, iImprovement = plots.pop(GAME.getSorenRandNum(iPlots, "Volcano event improvement destroyed"))
		iPlots -= 1
		szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getImprovementInfo(iImprovement).getTextKey(), ))
		if isLocalHumanPlayer(data.ePlayer) :
			CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getImprovementInfo(iImprovement).getButton(), GC.getCOLOR_RED(), plot.getX(), plot.getY(), True, True)
		if iImprovement in listRuins:
			plot.setImprovementType(iRuins)
		else:
			plot.setImprovementType(-1)

def doEarthquakeSmall(data):
    minDestroy = 1
    maxDestroy = 1
    popLossPercent = 1
    _doEarthquakeCore(data, minDestroy , maxDestroy, popLossPercent)


def doEarthquakeMedium(data):
    minDestroy = 1
    maxDestroy = 2
    popLossPercent = 3
    _doEarthquakeCore(data, minDestroy , maxDestroy, popLossPercent)


def doEarthquakeStrong(data):
    minDestroy = 2
    maxDestroy = 4
    popLossPercent = 5
    _doEarthquakeCore(data, minDestroy , maxDestroy, popLossPercent)


def doEarthquakeHuge(data):
    minDestroy = 3
    maxDestroy = 5
    popLossPercent = 15
    _doEarthquakeCore(data, minDestroy , maxDestroy, popLossPercent)


def doEarthquakeApocalyptic(data):
    minDestroy = 4
    maxDestroy = 8
    popLossPercent = 25
    _doEarthquakeCore(data, minDestroy , maxDestroy, popLossPercent)

def _doEarthquakeCore(argsList, minDestroy, maxDestroy, popLossPercent):
    data = argsList[1]
    CyPlayer = GC.getPlayer(data.ePlayer)
    CyCity = CyPlayer.getCity(data.iCityId)

    # --- Random number of buildings to destroy ---
    destroyCount = minDestroy
    if maxDestroy > minDestroy:
        destroyCount += GAME.getSorenRandNum(
            maxDestroy - minDestroy + 1,
            "Random building destruction"
        )

    # --- POPULATION BONUS APPLIED TO MAX ONLY ---
    pop = CyCity.getPopulation()
    popBonus = pop // 10  # pop 0–9 = 0, 10–19 = 1, 20–29 = 2, etc.
    destroyCount += popBonus

    candidates = []
    for i in range(GC.getNumBuildingInfos()):
        if isLimitedWonder(i) or not CyCity.hasBuilding(i) or CyCity.isFreeBuilding(i):
            continue
        info = GC.getBuildingInfo(i)
        if info.getProductionCost() < 1 or info.isNukeImmune() or info.isAutoBuild():
            continue
        candidates.append(i)

    # No buildings? Nothing to destroy
    if not candidates:
        return

    # --- Shuffle using Civ4 RNG ---
    for i in range(len(candidates) - 1, 0, -1):
        j = GAME.getSorenRandNum(i + 1, "Shuffle")
        candidates[i], candidates[j] = candidates[j], candidates[i]

    # --- Destroy buildings ---
    destroyed = 0
    for buildingID in candidates:
        if destroyed >= destroyCount:
            break

        destroyed += 1
        CyCity.changeHasBuilding(buildingID, False)

        if isLocalHumanPlayer(data.ePlayer):
            CyInterface().addMessage(
                data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(),
                TRNSLTR.getText(
                    "TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED",
                    (GC.getBuildingInfo(buildingID).getTextKey(),)
                ),
                "AS2D_BOMBARDED",
                InterfaceMessageTypes.MESSAGE_TYPE_INFO,
                GC.getBuildingInfo(buildingID).getButton(),
                GC.getCOLOR_RED(), CyCity.getX(), CyCity.getY(),
                True, True
            )

        if popLossPercent > 0:
            currentPop = CyCity.getPopulation()
            if currentPop > 1: # only apply if city has more than 1 pop
                popLoss = max(1, int(currentPop * popLossPercent / 100))

                # don’t kill the city—make sure at least 1 pop survives
                popLoss = min(popLoss, currentPop - 1)

                CyCity.changePopulation(-popLoss)


####### Assassin Discovered #######


def canDoAssassinDiscovered3(argsList):
	return GC.getPlayer(argsList[1].ePlayer).getCapitalCity() != None

def doAssassinDiscovered3(argsList):
	CyPlayer = GC.getPlayer(argsList[1].ePlayer)

	plot = CyPlayer.getCapitalCity().plot()
	x = plot.getX()
	y = plot.getY()

	iBronzeWorking = GC.getInfoTypeForString("TECH_BRONZE_WORKING")
	hasBronze = GC.getTeam(CyPlayer.getTeam()).isHasTech(iBronzeWorking)

	if hasBronze:
		iUnit = GC.getInfoTypeForString("UNIT_AXEMAN")
	else:
		iUnit = GC.getInfoTypeForString("UNIT_STONE_AXEMAN")

	iCount = 1 + CyPlayer.getNumCities() / 4
	while iCount > 0:
		CyPlayer.initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)
		iCount -= 1

def getHelpAssassinDiscovered3(argsList):
	CyPlayer = GC.getPlayer(argsList[1].ePlayer)
	iBronzeWorking = GC.getInfoTypeForString("TECH_BRONZE_WORKING")
	hasBronze = GC.getTeam(CyPlayer.getTeam()).isHasTech(iBronzeWorking)

	iCount = 1 + CyPlayer.getNumCities() / 4

	if hasBronze:
		return TRNSLTR.getText("TXT_KEY_EVENT_ASSASSIN_DISCOVERED_3_HELP", (iCount,))
	else:
		return TRNSLTR.getText("TXT_KEY_EVENT_ASSASSIN_DISCOVERED_3_STONE_HELP", (iCount,))

######## BLACK DEATH PLAGUE ###########

def canTriggerBlackDeath(argsList):
  data = argsList[0]

  player = GC.getPlayer(data.ePlayer)
  team = GC.getTeam(player.getTeam())

#   iClassical = GC.getInfoTypeForString("C2C_ERA_CLASSICAL")
#
#   if player.getCurrentEra() <= iClassical:
#     return False

  iMedicine = GC.getInfoTypeForString("TECH_MEDICINE")

  if team.isHasTech(iMedicine):
    return False

  return True

def applyBlackDeath2(argsList):
  data = argsList[1]

  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  iNumCities = 2 + GAME.getSorenRandNum(3, "Black Death event number of cities")

  listCities = []
  for loopCity in player.cities():
    if loopCity.getPopulation() > 2:
      iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())
      if iDistance > 0:
        listCities.append((iDistance, loopCity))

  listCities.sort(key=itemgetter(0))

  if iNumCities > len(listCities):
    iNumCities = len(listCities)

  for i in xrange(iNumCities):
    (iDist, loopCity) = listCities[i]
    loopCity.changePopulation(-2)
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_BLACK_DEATH_HIT_CITY", (loopCity.getNameKey(), ))
    if isLocalHumanPlayer(data.ePlayer) :
        CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), loopCity.getX(), loopCity.getY(), True, True)


def getHelpBlackDeath2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_BLACK_DEATH_HELP_2", (2, ))

######## SMALLPOX EPIDEMIC ###########

def canTriggerSmallpox(argsList):
  data = argsList[0]

  player = GC.getPlayer(data.ePlayer)
  team = GC.getTeam(player.getTeam())

#   iClassical = GC.getInfoTypeForString("C2C_ERA_CLASSICAL")
#
#   if player.getCurrentEra() <= iClassical:
#     return False

  iMedicine = GC.getInfoTypeForString("TECH_MEDICINE")

  if team.isHasTech(iMedicine):
    return False

  return True

def applySmallpox2(argsList):
  data = argsList[1]

  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  iNumCities = 1 + GAME.getSorenRandNum(3, "Smallpox event number of cities")

  listCities = []
  for loopCity in player.cities():
    if loopCity.getPopulation() > 2:
      iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())
      if iDistance > 0:
        listCities.append((iDistance, loopCity))

  listCities.sort(key=itemgetter(0))

  if iNumCities > len(listCities):
    iNumCities = len(listCities)

  for i in xrange(iNumCities):
    (iDist, loopCity) = listCities[i]
    loopCity.changePopulation(-1)
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SMALLPOX_HIT_CITY", (loopCity.getNameKey(), ))
    if isLocalHumanPlayer(data.ePlayer) :
        CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), loopCity.getX(), loopCity.getY(), True, True)


def getHelpSmallpox2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_SMALLPOX_HELP_2", (2, ))


####### Measles ########

def canTriggerMeasles(argsList):
  ePlayer = argsList[1]
  iCity = argsList[2]

  player = GC.getPlayer(ePlayer)
  city = player.getCity(iCity)

  if city is None:
    return False

  # city health is positive, no epidemic
  if ((city.goodHealth() - city.badHealth(True)) > 1):
    return False

  return True



  ######## THE SILVER RAIN ###########

def canTriggerSilverRain(argsList):

  data = argsList[0]
  pPlayer = GC.getPlayer(data.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_NONE):
    return False

#   At least one civ on the board must know Weather control
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_WEATHER_CONTROL")
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      if GC.getTeam(loopPlayer.getTeam()).isHasTech(iTech):
        bFoundValid = True
        break

  if not bFoundValid:
    return False

# Find an eligible plot
  for plot in GC.getMap().plots():
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      return True

  return False

def getHelpSilverRain1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_SILVER_RAIN_HELP_1", ())


def applySilverRain1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(data.ePlayer) > 0 and plot.isAdjacentPlayer(data.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "The Silver Rain event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnits1 = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnits1 = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnits1 = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnits1 = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnits1 = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_HUGE"):
    iNumUnits1 = 2
  else:
    iNumUnits1 = 3


  iUnitType1 = GC.getInfoTypeForString("UNIT_NANITE_CLOUD")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits1):
    barbPlayer.initUnit(iUnitType1, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)

def getHelpSilverRain3(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_SILVER_RAIN_HELP_3", ())

def applySilverRain3(argsList):
	data = argsList[1]

	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(data.ePlayer) > 0
		and plot.isAdjacentPlayer(data.ePlayer, True)
		):
			listPlots.append(i)

	if not listPlots:
		return

	plot = MAP.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Mercenary event location")])
	x = plot.getX()
	y = plot.getY()

	# Can we build the counter unit?
	iCounterUnit1 = GC.getInfoTypeForString("UNIT_TACTICAL_NUKE")
	iCounterUnit2 = GC.getInfoTypeForString("UNIT_ICBM")
	for CyCity in player.cities():
		if CyCity.canTrain(iCounterUnit1, False, False, False, False) or CyCity.canTrain(iCounterUnit2, False, False, False, False):
			iNukeUnit = GC.getPlayer(GC.getBARBARIAN_PLAYER()).initUnit(GC.getInfoTypeForString("UNIT_NANITE_CLOUD"), x, y, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
			plot.nukeExplosion(1, iNukeUnit)
			iNukeUnit.kill(False, -1)
			break
	else:
		iUnit = GC.getInfoTypeForString("UNIT_NANITE_CLOUD")
		barb = GC.getPlayer(GC.getBARBARIAN_PLAYER())
		iCount = (MAP.getWorldSize() + 5)*2/3
		while iCount > 0:
			CyUnit = barb.initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)
			CyUnit.setName("Mercenary Warrior")
			iCount -= 1


# ########### City ruins treasure event ###################

def canTriggerCityRuinsTreasure1(argsList):
  data = argsList[0]

  plot = GC.getMap().plot(data.iPlotX, data.iPlotY)

  # check for modmods if they add sea cities, do not allow treasure unit to spawn on water plots if city ruins are in water
  if plot.isWater() or plot.isImpassable():
      return False
  return True

def getHelpCityRuinsTreasure1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_BONUS_UNIT", (1, GC.getUnitInfo(GC.getInfoTypeForString("UNIT_TREASURE")).getTextKey()))

def applyCityRuinsTreasure1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  plot = GC.getMap().plot(data.iPlotX, data.iPlotY)

  iUnitType = GC.getInfoTypeForString("UNIT_TREASURE")
  iNumUnits = 1

  for i in xrange(iNumUnits):
    player.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ENGINEER, DirectionTypes.DIRECTION_SOUTH)

# ###########Compulsory Democracy########### #

def canTriggerCompulsoryDem(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  eCivic = player.getCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
  iFederal = GC.getInfoTypeForString("CIVIC_FEDERALISM")
  iDemocracy = GC.getInfoTypeForString("CIVIC_DEMOCRACY")
  iRepublic = GC.getInfoTypeForString("CIVIC_REPUBLIC")
  if eCivic == iFederal or eCivic == iDemocracy or eCivic == iRepublic:
    return True
  else:
    return False

def canEventTriggerCompulsoryDem(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  if player.isAnarchy() or player.getRevolutionTimer() > 0:
    return False

  eCivic = player.getCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
  iFederal = GC.getInfoTypeForString("CIVIC_FEDERALISM")
  iDemocracy = GC.getInfoTypeForString("CIVIC_DEMOCRACY")
  iRepublic = GC.getInfoTypeForString("CIVIC_REPUBLIC")
  if eCivic == iFederal or eCivic == iDemocracy or eCivic == iRepublic:
    bFederal = player.canDoCivics(GC.getInfoTypeForString("CIVIC_FEDERALISM"))
    bDemocracy = player.canDoCivics(GC.getInfoTypeForString("CIVIC_DEMOCRACY"))
    bRepublic = player.canDoCivics(GC.getInfoTypeForString("CIVIC_REPUBLIC"))
    bMonarchy = player.canDoCivics(GC.getInfoTypeForString("CIVIC_MONARCHY"))
    bFacist = player.canDoCivics(GC.getInfoTypeForString("CIVIC_TOTALITARIANISM"))
    bDespotism = player.canDoCivics(GC.getInfoTypeForString("CIVIC_DESPOTISM"))
    if (bFederal or bDemocracy or bRepublic):
      if (bMonarchy or bFacist or bDespotism):
        return True

  return False

def TriggerCompulsoryDem2(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  eCivic = player.getCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
  iFederal = GC.getInfoTypeForString("CIVIC_FEDERALISM")

  bFederal = player.canDoCivics(GC.getInfoTypeForString("CIVIC_FEDERALISM"))
  bDemocracy = player.canDoCivics(GC.getInfoTypeForString("CIVIC_DEMOCRACY"))
  bRepublic = player.canDoCivics(GC.getInfoTypeForString("CIVIC_REPUBLIC"))

  curCivics = []

  for i in xrange(GC.getNumCivicOptionInfos()):
    curCivics.append(player.getCivics(i))

  if bFederal and eCivic != iFederal:
    curCivics[GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")] = iFederal
  elif eCivic != GC.getInfoTypeForString("CIVIC_DEMOCRACY") and bDemocracy:
    curCivics[GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")] = GC.getInfoTypeForString("CIVIC_DEMOCRACY")
  elif eCivic != GC.getInfoTypeForString("CIVIC_REPUBLIC") and bRepublic:
    curCivics[GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")] = GC.getInfoTypeForString("CIVIC_REPUBLIC")

  player.doRevolution(curCivics, False)

def TriggerCompulsoryDem3(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  eCivic = player.getCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))

  # bFederal = player.canDoCivics(GC.getInfoTypeForString("CIVIC_FEDERALISM"))
  # bDemocracy = player.canDoCivics(GC.getInfoTypeForString("CIVIC_DEMOCRACY"))
  # bRepublic = player.canDoCivics(GC.getInfoTypeForString("CIVIC_REPUBLIC"))
  bMonarchy = player.canDoCivics(GC.getInfoTypeForString("CIVIC_MONARCHY"))
  bFacist = player.canDoCivics(GC.getInfoTypeForString("CIVIC_TOTALITARIANISM"))
  bDespotism = player.canDoCivics(GC.getInfoTypeForString("CIVIC_DESPOTISM"))

  curCivics = []

  for i in xrange(GC.getNumCivicOptionInfos()):
    curCivics.append(player.getCivics(i))

  if bFacist:
    curCivics[GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")] = GC.getInfoTypeForString("CIVIC_TOTALITARIANISM")
  elif bMonarchy:
    curCivics[GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")] = GC.getInfoTypeForString("CIVIC_MONARCHY")
  elif bDespotism:
    curCivics[GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")] = GC.getInfoTypeForString("CIVIC_DESPOTISM")
  player.doRevolution(curCivics, False)

def getHelpCompulsoryDem1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_TRIGGER_COMPULSORY_DEM_HELP_1", ())


def getHelpCompulsoryDem2(argsList):
	iEvent = argsList[0]
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)

	szType = ""

	if iEvent == GC.getInfoTypeForString("EVENT_COMPULSORY_DEM_2"):
		print "Event #2"
		eCivic = CyPlayer.getCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
		iFederal = GC.getInfoTypeForString("CIVIC_FEDERALISM")
		iDemocracy = GC.getInfoTypeForString("CIVIC_DEMOCRACY")
		iRepublic = GC.getInfoTypeForString("CIVIC_REPUBLIC")

		if eCivic != iFederal and CyPlayer.canDoCivics(iFederal):
			szType = GC.getCivicInfo(iFederal).getTextKey()
			print "Can Switch to Federal"

		elif eCivic != iDemocracy and CyPlayer.canDoCivics(iDemocracy):
			szType = GC.getCivicInfo(iDemocracy).getTextKey()
			print "Can Switch to Democracy"

		elif eCivic != iRepublic and CyPlayer.canDoCivics(iRepublic):
			szType = GC.getCivicInfo(iRepublic).getTextKey()
			print "Can Switch to Republic"
	else:
		print "Event #3"
		if CyPlayer.canDoCivics(GC.getInfoTypeForString("CIVIC_TOTALITARIANISM")):
			szType = GC.getCivicInfo(GC.getInfoTypeForString("CIVIC_TOTALITARIANISM")).getTextKey()
			print "Can Switch to Fascist"
		elif CyPlayer.canDoCivics(GC.getInfoTypeForString("CIVIC_MONARCHY")):
			szType = GC.getCivicInfo(GC.getInfoTypeForString("CIVIC_MONARCHY")).getTextKey()
			print "Can Switch to Monarchy"
		elif CyPlayer.canDoCivics(GC.getInfoTypeForString("CIVIC_DESPOTISM")):
			szType = GC.getCivicInfo(GC.getInfoTypeForString("CIVIC_DESPOTISM")).getTextKey()
			print "Can Switch to Despotism"

	return TRNSLTR.getText("TXT_KEY_EVENT_CHANGE_CIVICS", (szType,))


def TriggerHarryPotter2(argsList):
	data = argsList[1]
	iPlayer = data.ePlayer
	CyPlayer = GC.getPlayer(iPlayer)

	iLibrary = GC.getInfoTypeForString("BUILDING_LIBRARY")
	iStateReligion = CyPlayer.getStateReligion()
	for CyCity in CyPlayer.cities():
		if CyCity.canConstruct(iLibrary, False, False, True):
			CyCity.changeHasBuilding(iLibrary, True)

		if CyCity.isHasReligion(iStateReligion):
			CyCity.changeHurryAngerTimer(CyCity.flatHurryAngerLength())

	CyTeam = GC.getTeam(CyPlayer.getTeam())
	for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
		CyPlayerX = GC.getPlayer(iPlayerX)
		if CyPlayerX.isAlive() and CyPlayerX.getStateReligion() == iStateReligion and CyTeam.isHasMet(CyPlayerX.getTeam()) :
			CyPlayerX.AI_changeMemoryCount(iPlayer, MemoryTypes.MEMORY_EVENT_BAD_TO_US, 1)

def getHelpHarryPotter2(argsList):
	szHelp = TRNSLTR.getText("TXT_KEY_EVENT_FREE_BUILDING_GLOBAL", (GC.getBuildingInfo(GC.getInfoTypeForString("BUILDING_LIBRARY")).getTextKey(),))
	szHelp += "\n" + TRNSLTR.getText("TXT_KEY_EVENT_CAUSES_HOLY_ANGER",())
	szHelp += "\n" + TRNSLTR.getText("TXT_KEY_EVENT_CAUSES_FOREIGN_HOLY_ANGER",())
	return szHelp

def canEventTriggerLessDictator(argsList):
  data = argsList[0]
  player = GC.getPlayer(data.ePlayer)

  if (player.isAnarchy() or player.getRevolutionTimer() > 0):
    return False

  eCivic = player.getCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
  iMonarch = GC.getInfoTypeForString("CIVIC_MONARCHY")
  iFascist = GC.getInfoTypeForString("CIVIC_TOTALITARIANISM")
  iDespot = GC.getInfoTypeForString("CIVIC_DESPOTISM")
  if eCivic == iMonarch or eCivic == iFascist or eCivic == iDespot:
    return True

  return False

def TriggerLessDictator1(argsList):
	data = argsList[1]
	for loopCity in GC.getPlayer(data.ePlayer).cities():
		loopCity.setWeLoveTheKingDay(True)

def getHelpLessDictator1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_NATIONAL_HOLIDAY",())

def TriggerLessDictator2(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)

  eCivic = player.getCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
  iFederal = GC.getInfoTypeForString("CIVIC_FEDERALISM")
  iRepublic = GC.getInfoTypeForString("CIVIC_REPUBLIC")
  iDemocracy = GC.getInfoTypeForString("CIVIC_DEMOCRACY")

  curCivics = []
  for i in xrange(GC.getNumCivicOptionInfos()):
    curCivics.append(player.getCivics(i))

  bFederal = player.canDoCivics(iFederal)
  bDemocracy = player.canDoCivics(iDemocracy)
  bRepublic = player.canDoCivics(iRepublic)

  if bFederal:
    curCivics[GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")] = iFederal
  elif bDemocracy:
    curCivics[GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")] = iDemocracy
  elif bRepublic:
    curCivics[GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")] = iRepublic

  player.doRevolution(curCivics, False)

def getHelpLessDictator2(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)

	iFederal = GC.getInfoTypeForString("CIVIC_FEDERALISM")
	if CyPlayer.canDoCivics(iFederal):
		return TRNSLTR.getText("TXT_KEY_EVENT_CHANGE_CIVICS", (GC.getCivicInfo(iFederal).getTextKey(),))

	iDemocracy = GC.getInfoTypeForString("CIVIC_DEMOCRACY")
	if CyPlayer.canDoCivics(iDemocracy):
		return TRNSLTR.getText("TXT_KEY_EVENT_CHANGE_CIVICS", (GC.getCivicInfo(iDemocracy).getTextKey(),))

	iRepublic = GC.getInfoTypeForString("CIVIC_REPUBLIC")
	if CyPlayer.canDoCivics(iRepublic):
		return TRNSLTR.getText("TXT_KEY_EVENT_CHANGE_CIVICS", (GC.getCivicInfo(iRepublic).getTextKey(),))

	return TRNSLTR.getText("TXT_KEY_EVENT_CHANGE_CIVICS", ("",))

def canTriggerUnlimitedPower1(argsList):
	data = argsList[1]
	return not GC.getPlayer(data.ePlayer).getCity(data.iCityId).isPower()

def TriggerUnlimitedPower1(argsList):
	data = argsList[1]
	GC.getPlayer(data.ePlayer).getCity(data.iCityId).changePowerCount(1)

def getHelpUnlimitedPower1(argsList):
	data = argsList[1]
	return TRNSLTR.getText("TXT_KEY_EVENT_PROVIDES_POWER", (GC.getPlayer(data.ePlayer).getCity(data.iCityId).getNameKey(),))

def canTriggerUnlimitedPower2(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  if (eventCity.area().isCleanPower(player.getTeam())):
    return False
  if (player.getGold() < (eventCity.area().getNumCities() * 100)):
    return False
  return True

def TriggerUnlimitedPower2(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)
	CyArea = CyPlayer.getCity(data.iCityId).area()
	iRequireGold = 100 * CyArea.getNumCities()
	CyPlayer.changeGold(-iRequireGold)
	CyArea.changeCleanPowerCount(CyPlayer.getTeam(), 1)

def getHelpUnlimitedPower2(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)

	iGold = CyPlayer.getCity(data.iCityId).area().getNumCities() * 100
	if CyPlayer.getGold() < iGold:
		szGold = "\n<color=255,76,76>"
	else: szGold = "\n"
	szGold += TRNSLTR.getText("TXT_KEY_EVENT_COSTS_GOLD", (iGold,))

	return TRNSLTR.getText("TXT_KEY_EVENT_PROVIDES_AREA_POWER",()) + szGold

def TriggerSuperVirus1(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  iNumCities = 4 + GAME.getSorenRandNum(5, "Super Virus event number of cities")

  listCities = []
  for loopCity in player.cities():
    loopCity.changeEventAnger(3)
    if loopCity.getPopulation() > 4:
      iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())
      if iDistance > 0:
        listCities.append((iDistance, loopCity))

  listCities.sort(key=itemgetter(0))

  if iNumCities > len(listCities):
    iNumCities = len(listCities)

  iKilledPop = (eventCity.getPopulation() * 75) / 100

  eventCity.changePopulation(-iKilledPop)
  szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (iKilledPop, eventCity.getNameKey(), ))
  if isLocalHumanPlayer(data.ePlayer) :
    CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), eventCity.getX(), eventCity.getY(), True, True)

  for i in xrange(iNumCities):
    (iDist, loopCity) = listCities[i]
    loopCity.changePopulation(-4)
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (4, loopCity.getNameKey(), ))
    if isLocalHumanPlayer(data.ePlayer) :
      CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), loopCity.getX(), loopCity.getY(), True, True)

def getHelpSuperVirus1(argsList):
	data = argsList[1]

	szCityName = TRNSLTR.getText(str(GC.getPlayer(data.ePlayer).getCity(data.iCityId).getNameKey()), ())

	return TRNSLTR.getText("TXT_KEY_EVENT_DOOMED_OPTION",()) + "\n" + TRNSLTR.getText("TXT_KEY_EVENT_CITY_UNREST", (3, szCityName))


def TriggerSuperVirus2(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  iNumCities = 1 + GAME.getSorenRandNum(3, "Super Virus event number of cities")

  iRequireGold = 100 + player.getNumCities() * 350
  player.changeGold(-iRequireGold)

  listCities = []
  for loopCity in player.cities():
    loopCity.changeEventAnger(1)
    if loopCity.getPopulation() > 4:
      iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())
      if iDistance > 0:
        listCities.append((iDistance, loopCity))

  listCities.sort(key=itemgetter(0))

  if iNumCities > len(listCities):
    iNumCities = len(listCities)

  iKilledPop = (eventCity.getPopulation() * 45) / 100

  eventCity.changePopulation(-iKilledPop)
  if isLocalHumanPlayer(data.ePlayer) :
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (iKilledPop, eventCity.getNameKey(), ))
    CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), eventCity.getX(), eventCity.getY(), True, True)

  for i in xrange(iNumCities):
    (iDist, loopCity) = listCities[i]
    loopCity.changePopulation(-2)
    if isLocalHumanPlayer(data.ePlayer) :
      szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (2, loopCity.getNameKey(), ))
      CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), loopCity.getX(), loopCity.getY(), True, True)

def getHelpSuperVirus2(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)

	szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SMALL_VIRUS_SPREAD",()) + "\n"
	szHelp += TRNSLTR.getText("TXT_KEY_EVENT_CITY_UNREST", (1, CyPlayer.getCity(data.iCityId).getNameKey()))

	iGold = 100 + CyPlayer.getNumCities() * 35
	if CyPlayer.getGold() < iGold:
		szHelp += "\n<color=255,76,76>"
	else: szHelp += "\n"

	return szHelp + TRNSLTR.getText("TXT_KEY_EVENT_COSTS_GOLD", (iGold,))

def canTriggerSuperVirus2(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)

	if CyPlayer.getGold() < 100 + CyPlayer.getNumCities() * 35:
		return False
	return True

def TriggerSuperVirus3(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  iRequireGold = 100 + player.getNumCities() * 650
  player.changeGold(-iRequireGold)

  iChangePopulation = eventCity.getPopulation() * 25
  iChangePopulation /= 100
  eventCity.changePopulation(-iChangePopulation)
  if isLocalHumanPlayer(data.ePlayer) :
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (iChangePopulation, eventCity.getNameKey()))
    CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), eventCity.getX(), eventCity.getY(), True, True)

  eventCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_GREAT_SCIENTIST"), 2)

def getHelpSuperVirus3(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)
	CyCity = CyPlayer.getCity(data.iCityId)

	szCityNameKey = CyCity.getNameKey()
	szHelp = TRNSLTR.getText("TXT_KEY_EVENT_FREE_SPECIALISTS", (2, GC.getSpecialistInfo(GC.getInfoTypeForString("SPECIALIST_GREAT_SCIENTIST")).getTextKey(), szCityNameKey))
	szHelp += "\n" + TRNSLTR.getText("TXT_KEY_EVENT_LOSE_POPULATION", (CyCity.getPopulation()*25/100, szCityNameKey))

	iGold = 100 + CyPlayer.getNumCities() * 65

	if CyPlayer.getGold() < iGold:
		szHelp += "\n<color=255,76,76>" + TRNSLTR.getText("TXT_KEY_EVENT_COSTS_GOLD", (iGold,)) + "</color>"
	else: szHelp += "\n" + TRNSLTR.getText("TXT_KEY_EVENT_COSTS_GOLD", (iGold,))

	iSmartMedicine = GC.getInfoTypeForString("BONUS_SMART_MEDICINE")
	if iSmartMedicine > 0:
		if not CyCity.hasBonus(iSmartMedicine):
			szHelp += "\n" + TRNSLTR.getText("TXT_KEY_BUILDINGHELP_REQUIRES_STRING", (GC.getBonusInfo(iSmartMedicine).getTextKey(),))

	iSmartDrugs = GC.getInfoTypeForString("TECH_SMART_DRUGS")
	if iSmartDrugs > 0 and not GC.getTeam(CyPlayer.getTeam()).isHasTech(iSmartDrugs):
		szHelp += "\n" + TRNSLTR.getText("TXT_KEY_BUILDINGHELP_REQUIRES_STRING", (GC.getTechInfo(iSmartDrugs).getTextKey(),))

	return szHelp

def canTriggerSuperVirus3(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)
  team = GC.getTeam(player.getTeam())

  iRequireGold = 100 + player.getNumCities() * 65
  if (player.getGold() < iRequireGold):
    return False
  iSmartMedicine = GC.getInfoTypeForString("BONUS_SMART_MEDICINE")
  iSmartDrugs = GC.getInfoTypeForString("TECH_SMART_DRUGS")
  if (iSmartMedicine > 0):
    if (not eventCity.hasBonus(iSmartMedicine)):
      return False
  if (iSmartDrugs > 0):
    if (not team.isHasTech(iSmartDrugs)):
      return False
  return True

def TriggerSuperVirus4(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  iChangePopulation = eventCity.getPopulation() * 75
  iChangePopulation /= 100

  eventCity.changeOccupationTimer(iChangePopulation)
  eventCity.changeEventAnger(iChangePopulation)

  eventCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_HOSPITAL"), False)

  eventCity.changePopulation(-iChangePopulation)
  if isLocalHumanPlayer(data.ePlayer) :
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (iChangePopulation, eventCity.getNameKey()))
    CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), eventCity.getX(), eventCity.getY(), True, True)

  eventCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_GREAT_MILITARY_INSTRUCTOR"), 1)

def getHelpSuperVirus4(argsList):
  data = argsList[1]
  player = GC.getPlayer(data.ePlayer)
  eventCity = player.getCity(data.iCityId)

  iChangePopulation = eventCity.getPopulation() * 75
  iChangePopulation /= 100

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_FREE_SPECIALISTS", (1, GC.getSpecialistInfo(GC.getInfoTypeForString("SPECIALIST_GREAT_MILITARY_INSTRUCTOR")).getTextKey(), eventCity.getNameKey() , ))
  szHelp += u"\n"
  szHelp += TRNSLTR.getText("TXT_KEY_EVENT_LOSE_POPULATION", (iChangePopulation, eventCity.getNameKey() , ))
  szHelp += u"\n"
  szHelp += TRNSLTR.getText("TXT_KEY_EVENT_CITY_ANARCHY", (iChangePopulation, eventCity.getNameKey() , ))
  szHelp += u"\n"
  szHelp += TRNSLTR.getText("TXT_KEY_EVENT_CITY_UNREST", (iChangePopulation, eventCity.getNameKey(), ))

  return szHelp


def canDoNewWorldTrigger(argsList):
	data = argsList[0]
	CyPlayer = GC.getPlayer(data.ePlayer)
	#Room on the Map for 3 new cities
	MAP = GC.getMap()
	iNumPlots = MAP.numPlots()
	iNeededCities = 3
	plotIndexes = []
	while iNeededCities > 0:
		for i in xrange(iNumPlots):
			if i not in plotIndexes:
				CyPlot = MAP.plotByIndex(i)
				if not CyPlot.isWater() and CyPlot.isCoastal() and CyPlayer.canFound(CyPlot.getX(), CyPlot.getY()):
					plotIndexes.append(i)
					break
		else: return False
		iNeededCities -= 1
	return True


def triggerNewWorldCities(argsList):
	iEvent = argsList[0]
	data = argsList[1]
	iPlayer = data.ePlayer
	CyPlayer = GC.getPlayer(iPlayer)

	if iEvent == GC.getInfoTypeForString("EVENT_NEW_WORLD_2"):
		iNeededCities = 1
	else:
		iNeededCities = 3

	for CyCity in CyPlayer.cities():
		if iEvent == GC.getInfoTypeForString("EVENT_NEW_WORLD_2"):
			if CyCity.getPopulation() > 4:
				CyCity.changePopulation(-1)
		else:
			if CyCity.getPopulation() > 9:
				CyCity.changePopulation(-2)
			elif CyCity.getPopulation() > 4:
				CyCity.changePopulation(-1)

	iNumUnits = GC.getNumUnitInfos()
	MAP = GC.getMap()
	while iNeededCities > 0:
		iBestValue = 0
		pBestPlot = None
		for CyPlot in MAP.plots():
			if not CyPlot.isWater() and CyPlot.isCoastal() and CyPlayer.canFound(CyPlot.getX(), CyPlot.getY()):
				iValue = CyPlot.getFoundValue(iPlayer)
				if iValue > iBestValue:
					pBestPlot = CyPlot
					iBestValue = iValue
		if pBestPlot is None:
			raise "Error in TriggerNewWorldCities - No City Created!"
			return

		CyPlayer.found(pBestPlot.getX(), pBestPlot.getY())

		CyCity = pBestPlot.getPlotCity()
		if not CyCity:
			raise "Error in TriggerNewWorldCities - No City Created!"
			return

		if iEvent == GC.getInfoTypeForString("EVENT_NEW_WORLD_2"):
			CyCity.changePopulation(1)
		else:
			CyCity.changePopulation(3)

		eStateReligion = CyPlayer.getStateReligion()
		if eStateReligion > -1:
			CyCity.setHasReligion(eStateReligion, True, True, True)

		if iEvent == GC.getInfoTypeForString("EVENT_NEW_WORLD_2"):
			iNumNeededDefenders = 2
		else:
			iNumNeededDefenders = 5

		eBestUnit = -1
		iBestStrength = 0
		for iUnit in xrange(iNumUnits):
			CvUnitInfo = GC.getUnitInfo(iUnit)
			if CvUnitInfo.getMaxGlobalInstances() != -1 or CvUnitInfo.getMaxPlayerInstances() != -1:
				continue
			if CvUnitInfo.getDomainType() != DomainTypes.DOMAIN_LAND or CvUnitInfo.getCombat() <= iBestStrength:
				continue
			if CyCity.canTrain(iUnit, False, False, False, False):
				eBestUnit = iUnit
				iBestStrength = CvUnitInfo.getCombat()

		if eBestUnit > -1:
			x = CyCity.getX()
			y = CyCity.getY()
			while iNumNeededDefenders > 0:
				CyPlayer.initUnit(eBestUnit, x, y, UnitAITypes.UNITAI_CITY_DEFENSE, DirectionTypes.DIRECTION_NORTH)
				iNumNeededDefenders -= 1

		if iEvent == GC.getInfoTypeForString("EVENT_NEW_WORLD_2"):
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_WALLS"), True)
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_GRANARY"), True)
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_FORGE"), True)
		else:
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_WALLS"), True)
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_BARRACKS"), True)
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_GRANARY"), True)
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_FORGE"), True)
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_MARKET"), True)
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_HARBOR"), True)
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_LIGHTHOUSE"), True)
			CyCity.changeHasBuilding(GC.getInfoTypeForString("BUILDING_FISHERMANS_HUT"), True)
		iNeededCities -= 1


def canTriggerNewWorldCities3(argsList):

	iNavigation = GC.getInfoTypeForString("TECH_NAVIGATION")
	if iNavigation > -1:
		if not GC.getTeam(GC.getPlayer(argsList[1].ePlayer).getTeam()).isHasTech(iNavigation):
			return False
	return True

def getHelpNewWorld(argsList):

	if argsList[0] == GC.getInfoTypeForString("EVENT_NEW_WORLD_2"):
		szHelp = TRNSLTR.getText("TXT_KEY_EVENT_FREE_OUTPOST_CITY",()) + "\n" + TRNSLTR.getText("TXT_KEY_EVENT_EMPIRE_POPULATION_LOSS", (4, 1))
	else:
		szHelp = TRNSLTR.getText("TXT_KEY_EVENT_FREE_OUTPOST_CITIES",()) + "\n" + TRNSLTR.getText("TXT_KEY_EVENT_EMPIRE_POPULATION_LOSS", (9, 2))
		szHelp += "\n" + TRNSLTR.getText("TXT_KEY_EVENT_EMPIRE_POPULATION_LOSS", (4, 1))

		iNavigation = GC.getInfoTypeForString("TECH_NAVIGATION")
		if iNavigation > -1 and not GC.getTeam(GC.getPlayer(argsList[1].ePlayer).getTeam()).isHasTech(iNavigation):
			szHelp += "\n" + TRNSLTR.getText("TXT_KEY_BUILDINGHELP_REQUIRES_STRING", (GC.getTechInfo(iNavigation).getTextKey(),))
	return szHelp

##### VOLCANO C2C #####
def canDoNewVolcano(argsList):
	# Checks the plot to see if a new volcano can be formed
	data = argsList[0]
	pPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)
	if not pPlot: return False

	# List of features that block new volcanoes
	listFeatures = [
		GC.getInfoTypeForString('FEATURE_PLATY_AUYANTEPUI'),
		GC.getInfoTypeForString('FEATURE_PLATY_GREAT_BARRIER'),
		GC.getInfoTypeForString('FEATURE_GREAT_BARRIER_REEF_BEACON'),
		GC.getInfoTypeForString('FEATURE_GREAT_BARRIER_REEF_LIGHTHOUSE'),
		GC.getInfoTypeForString('FEATURE_PLATY_EVEREST'),
		GC.getInfoTypeForString('FEATURE_PLATY_FUJI'),
		GC.getInfoTypeForString('FEATURE_PLATY_AURORA'),
		GC.getInfoTypeForString('FEATURE_PLATY_BAIKAL'),
		GC.getInfoTypeForString('FEATURE_PLATY_BARRINGER_CRATER'),
		GC.getInfoTypeForString('FEATURE_PLATY_BASALT_ORGAN'),
		GC.getInfoTypeForString('FEATURE_PLATY_DEVILS_TABLE'),
		GC.getInfoTypeForString('FEATURE_PLATY_SOPKA'),
		GC.getInfoTypeForString('FEATURE_PLATY_KRAKATOA'),
		GC.getInfoTypeForString('FEATURE_PLATY_NGORONGORO_CRATER'),
		GC.getInfoTypeForString('FEATURE_PLATY_NUKUORO'),
		GC.getInfoTypeForString('FEATURE_PLATY_PAMUKKALE'),
		GC.getInfoTypeForString('FEATURE_PLATY_PRAVCICKA_BRANA'),
		GC.getInfoTypeForString('FEATURE_PLATY_SHARK_BAY'),
		GC.getInfoTypeForString('FEATURE_PLATY_SINAI'),
		GC.getInfoTypeForString('FEATURE_PLATY_SUGARLOAF'),
		GC.getInfoTypeForString('FEATURE_PLATY_ULURU'),
		GC.getInfoTypeForString('FEATURE_PLATY_VICTORIA_FALLS'),
		GC.getInfoTypeForString('FEATURE_PLATY_KILIMANJARO'),
		GC.getInfoTypeForString('FEATURE_PLATY_DEAD_SEA'),
		GC.getInfoTypeForString('FEATURE_VOLCANO_ACTIVE'),
		GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT')
	]

	# List of resources that count as volcanic, and thus do not block volcanoes.
	listVolcanicResources = [
		-1,
		GC.getInfoTypeForString('BONUS_OBSIDIAN'),
		GC.getInfoTypeForString('BONUS_SULPHUR')
	]
	return not pPlot.isCity() and pPlot.getFeatureType() not in listFeatures and pPlot.getBonusType(-1) in listVolcanicResources


def canDoOldVolcano(argsList):
	data = argsList[0]
	pPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)
	if not pPlot: return False

	# List of features that are volcanoes
	listVolcanoes = [
		GC.getInfoTypeForString('FEATURE_PLATY_FUJI'),
		GC.getInfoTypeForString('FEATURE_PLATY_SOPKA'),
		GC.getInfoTypeForString('FEATURE_PLATY_KRAKATOA'),
		GC.getInfoTypeForString('FEATURE_PLATY_KILIMANJARO'),
		GC.getInfoTypeForString('FEATURE_VOLCANO_ACTIVE'),
		GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT')
	]
	return not pPlot.isCity() and pPlot.getFeatureType() in listVolcanoes


def doVolcanoAdjustFertility(argsList):
	pPlot = argsList[0]
	extraFood = argsList[1]
	team = argsList[2]
	## For each neighbouring plot
	##   If extraFood is -1, 0 or 1 this is the amount of food to add to the plot

	if not pPlot or pPlot.isCity():
		return

	if extraFood == 0:
		return
	elif extraFood < -1:
		extraFood = -1
	elif extraFood > 1:
		extraFood = 1

	iX = pPlot.getX()
	iY = pPlot.getY()

	for i in xrange(8):
		tPlot = plotDirection(iX, iY, DirectionTypes(i))
		if tPlot and not tPlot.isCity():
			tPlot.setExtraYield(YieldTypes.YIELD_FOOD, extraFood)


def doVolcanoNeighbouringPlots(pPlot):
	# Directional eruption that picks an adjacent valid plot to erupt towards.
	# It then affects that plot and its two neighbors in the ring of 8 plots surrounding the volcano
	# Affected plots have units damaged, improvements destroyed, and oceans changed to coast.
	# To do - start forest fire

	if not pPlot:
		return

	terrainCoast = GC.getInfoTypeForString("TERRAIN_COAST")
	terrainSea = GC.getInfoTypeForString("TERRAIN_SEA")
	terrainOcean = GC.getInfoTypeForString("TERRAIN_OCEAN")
	terrainPolarCoast = GC.getInfoTypeForString("TERRAIN_COAST_POLAR")
	terrainPolarSea = GC.getInfoTypeForString("TERRAIN_SEA_POLAR")
	terrainPolarOcean = GC.getInfoTypeForString("TERRAIN_OCEAN_POLAR")
	terrainTropicalCoast = GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL")
	terrainTropicalSea = GC.getInfoTypeForString("TERRAIN_SEA_TROPICAL")
	terrainTropicalOcean = GC.getInfoTypeForString("TERRAIN_OCEAN_TROPICAL")

	iX = pPlot.getX()
	iY = pPlot.getY()

	# List of improvements that leave ruins behind when destroyed
	listRuins = [
		GC.getInfoTypeForString("IMPROVEMENT_COTTAGE"),
		GC.getInfoTypeForString("IMPROVEMENT_HAMLET"),
		GC.getInfoTypeForString("IMPROVEMENT_VILLAGE"),
		GC.getInfoTypeForString("IMPROVEMENT_TOWN"),
		GC.getInfoTypeForString("IMPROVEMENT_SUBURBS"),
		GC.getInfoTypeForString("IMPROVEMENT_GOODY_HUT")
	]

	iRuins = GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS")

	# List of improvements that are unaffected by eruption
	immuneImprovements = [
		GC.getInfoTypeForString("IMPROVEMENT_SEED_CAMP"),
		GC.getInfoTypeForString("IMPROVEMENT_WOOD_GATHERER"),
		GC.getInfoTypeForString("IMPROVEMENT_STONE_TOOLS_WORKSHOP"),
		GC.getInfoTypeForString("IMPROVEMENT_SCAVENGING_CAMP"),
		GC.getInfoTypeForString("IMPROVEMENT_FRUIT_GATHERING_CAMP"),
		GC.getInfoTypeForString("IMPROVEMENT_PLANT_GATHERING_CAMP"),
		GC.getInfoTypeForString("IMPROVEMENT_FISHING_BOATS"),
		GC.getInfoTypeForString("IMPROVEMENT_WHALING_BOATS"),
		GC.getInfoTypeForString("IMPROVEMENT_WHALING_SHIPS"),
		GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS")
	]

	listVolcanoPlots = []
	listVolcanoPlotsX = []
	listVolcanoPlotsY = []
	listAdjacentPlots = []
	listAffectedPlots = []

	# Sets up lists for plots that are adjacent to the volcano
	for i in xrange(8):
		plotX = plotDirection(iX, iY, DirectionTypes(i))
		if plotX:
			listVolcanoPlots.append(plotX)
			listVolcanoPlotsX.append(plotX.getX())
			listVolcanoPlotsY.append(plotX.getY())

	# Select a target plot
	targetplot = listVolcanoPlots[GAME.getSorenRandNum(len(listVolcanoPlots), "Volcano direction")]
	listAffectedPlots.append(targetplot)

	listAdjacentPlots.append(plotDirection(targetplot.getX(), targetplot.getY(), DirectionTypes.DIRECTION_NORTH))
	listAdjacentPlots.append(plotDirection(targetplot.getX(), targetplot.getY(), DirectionTypes.DIRECTION_SOUTH))
	listAdjacentPlots.append(plotDirection(targetplot.getX(), targetplot.getY(), DirectionTypes.DIRECTION_EAST))
	listAdjacentPlots.append(plotDirection(targetplot.getX(), targetplot.getY(), DirectionTypes.DIRECTION_WEST))

	# If plot is in the ring around the volcano, add to the list of affected plots
	for plotX in listAdjacentPlots:
		if (
			plotX
			and plotX.getX() in listVolcanoPlotsX
			and plotX.getY() in listVolcanoPlotsY
			and plotX.getX() != iX
			and plotX.getY() != iY
		):
			listAffectedPlots.append(plotX)

	#Loops through the list of affected plots applying eruption effects
	for i in xrange(len(listAffectedPlots)):
		if len(listAffectedPlots) > 0:
			plotX = listAffectedPlots[GAME.getSorenRandNum(len(listAffectedPlots), "Volcano event improvement destroyed")]
			iPlayer = plotX.getOwner()
			iImprovement = plotX.getImprovementType()

			# Destroys improvements if the plot is not a city, and if the improvement is not immune
			if iImprovement != -1 and not plotX.isCity() and not iImprovement in immuneImprovements:
				if iPlayer > -1:
					szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED_NOOWNER", (GC.getImprovementInfo(iImprovement).getTextKey(), ))
					CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getImprovementInfo(iImprovement).getButton(), GC.getCOLOR_RED(), plotX.getX(), plotX.getY(), True, True)
				if iImprovement in listRuins:
					plotX.setImprovementType(iRuins)
				else:
					plotX.setImprovementType(-1)

			# Damages units
			for pPlotUnit in plotX.units():
				if pPlotUnit.getDamage() < 50:
					pPlotUnit.setDamage(50, False)
				elif pPlotUnit.getDamage() < 75:
					pPlotUnit.setDamage(75, False)
				elif pPlotUnit.getDamage() < 90:
					pPlotUnit.setDamage(90, False)
				else:
					pPlotUnit.setDamage(99, False)

			# If affected plot is Ocean or Sea, change it to Coast
			if plotX.isWater():
				if plotX.getTerrainType() == terrainOcean or plotX.getTerrainType() == terrainSea:
					plotX.setTerrainType(terrainCoast, True, True)
				if plotX.getTerrainType() == terrainPolarOcean or plotX.getTerrainType() == terrainPolarSea:
					plotX.setTerrainType(terrainPolarCoast, True, True)
				if plotX.getTerrainType() == terrainTropicalOcean or plotX.getTerrainType() == terrainTropicalSea:
					plotX.setTerrainType(terrainTropicalCoast, True, True)

			# Remove processed plots from list
			listAffectedPlots.remove(plotX)


def doVolcanoPlot(pPlot):
	if not pPlot:
		return

	# List of features that are volcanoes
	listVolcanoes = [
		GC.getInfoTypeForString('FEATURE_PLATY_FUJI'),
		GC.getInfoTypeForString('FEATURE_PLATY_SOPKA'),
		GC.getInfoTypeForString('FEATURE_PLATY_KRAKATOA'),
		GC.getInfoTypeForString('FEATURE_PLATY_KILIMANJARO'),
		GC.getInfoTypeForString('FEATURE_VOLCANO_ACTIVE'),
		GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT')
	]
	ft_volcano_dormant = GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT')
	ft_volcano_active = GC.getInfoTypeForString('FEATURE_VOLCANO_ACTIVE')

	# if terrain is a hill or peak, level it by changing it to rocky flatland.
	if pPlot.isHills() or pPlot.isPeak():
		pPlot.setPlotType(PlotTypes.PLOT_LAND, True, True)
		pPlot.setTerrainType(GC.getInfoTypeForString('TERRAIN_ROCKY'), True, True)

	iFeature = pPlot.getFeatureType()
	pPlot.setImprovementType(-1)
	pPlot.setBonusType(-1)

	# if the terrain is not an active volcano make it so
	if iFeature == ft_volcano_dormant or iFeature not in listVolcanoes:
		pPlot.setFeatureType(ft_volcano_active, 0)

	# Wound any units on the same plot as the volcano
	for pPlotUnit in pPlot.units():
		if pPlotUnit.getDamage() < 90: pPlotUnit.setDamage(90, False)
		else: pPlotUnit.setDamage(99, False)

		# move them to safety
		for sPlot in pPlot.adjacent():
			if pPlotUnit.canEnterPlot(sPlot, False, False, True):
				pPlotUnit.setXY(sPlot.getX(), sPlot.getY(), False, True, True)

	if pPlot.isWater():
		pPlot.setPlotType(PlotTypes.PLOT_LAND, True, True)

def doVolcanoReport(argsList):
  pPlot = argsList[0]
  szBuffer = argsList[1]
  ft_volcano_dormant = GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT')
  ft_volcano_active = GC.getInfoTypeForString('FEATURE_VOLCANO_ACTIVE')

  # report message to any one who can see this plot
  iMaxPlayer = GC.getMAX_PC_PLAYERS()
  for i in xrange(iMaxPlayer):
    loopPlayer = GC.getPlayer(i)
    if loopPlayer.isHuman() and loopPlayer.isAlive() and pPlot.isRevealed(loopPlayer.getTeam(), False):
      CyInterface().addMessage(loopPlayer.getID(), False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getFeatureInfo(ft_volcano_active).getButton(), GC.getCOLOR_RED(), pPlot.getX(), pPlot.getY(), True, True)

      if pPlot.isInViewport():
        point = pPlot.getPoint()
        CyEngine().triggerEffect(GC.getInfoTypeForString('EFFECT_ARTILLERY_SHELL_EXPLODE'), point)
        CyAudioGame().Play3DSound("AS3D_UN_GRENADE_EXPLODE", point.x, point.y, point.z)


def doVolcanoNewEruption(argsList):
	data = argsList[0]
	pPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)
	if not pPlot:
		return
	player = GC.getPlayer(data.ePlayer)

	doVolcanoPlot(pPlot)
	doVolcanoNeighbouringPlots(pPlot)
	doVolcanoAdjustFertility((pPlot, 1, player.getTeam()))
	doVolcanoReport((pPlot, BugUtil.getPlainText("TXT_KEY_EVENT_TRIGGER_VOLCANO_NEW")))

def doVolcanoExistingEruption(argsList):
	data = argsList[0]
	pPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)
	if not pPlot:
		return

	doVolcanoNeighbouringPlots(pPlot)
	doVolcanoReport((pPlot, BugUtil.getPlainText("TXT_KEY_EVENTTRIGGER_VOLCANO_ACTIVE")))

def doVolcanoDormantEruption(argsList):
	data = argsList[0]
	pPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)
	if not pPlot:
		return
	player = GC.getPlayer(data.ePlayer)
	team = player.getTeam()

	doVolcanoPlot(pPlot)
	doVolcanoNeighbouringPlots(pPlot)
	doVolcanoAdjustFertility((pPlot, 1, team))
	doVolcanoReport((pPlot, BugUtil.getPlainText("TXT_KEY_EVENT_TRIGGER_VOLCANO_DORMANT_ERUPTION")))

def doVolcanoExtinction(argsList):
  data = argsList[0]
  pPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)
  if not pPlot:
    return

  pPlot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
  pPlot.setTerrainType(GC.getInfoTypeForString('TERRAIN_ROCKY'), True, True)
  player = GC.getPlayer(data.ePlayer)
  city = player.getCity(data.iCityId)

  team = player.getTeam()
  techteam = GC.getTeam(player.getTeam())
  iX = pPlot.getX()
  iY = pPlot.getY()

  if GAME.getSorenRandNum(100, 'Volcanic minerals chance') < 50:
    iBonus = GC.getInfoTypeForString('BONUS_OBSIDIAN')
    pPlot.setBonusType(iBonus)
    itechresource = GC.getInfoTypeForString("TECH_TOOL_MAKING")
  else:
    iBonus = GC.getInfoTypeForString('BONUS_SULPHUR')
    pPlot.setBonusType(iBonus)
    itechresource = GC.getInfoTypeForString("TECH_ANCIENT_BALLISTICS")

  if techteam.isHasTech(itechresource) and (pPlot.isRevealed(team, False)):
      doVolcanoReport((pPlot, BugUtil.getPlainText("TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE_VOLCANO")))

def doVolcanoSleep(argsList):
  data = argsList[0]
  pPlot = GC.getMap().plot(data.iPlotX, data.iPlotY)
  pPlot.setFeatureType(GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT'), 0)
  player = GC.getPlayer(data.ePlayer)
  team = player.getTeam()
  doVolcanoAdjustFertility((pPlot, -1, team))
  doVolcanoReport((pPlot, BugUtil.getPlainText("TXT_KEY_EVENT_TRIGGER_VOLCANO_DORMANT")))

def getHelpVolcanoEruption1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_VOLCANO_ERUPTION_HELP", ())

def getHelpVolcanoSleep(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_VOLCANO_SLEEP_HELP", ())

def getHelpVolcanoExtinction(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_VOLCANO_EXTINCTION_HELP", ())

### Fire events for C2C

def doWildFire(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)
	CyCity = CyPlayer.getCity(data.iCityId)

	validHousesList = []
	for i in range(GC.getNumBuildingInfos()):
		if isLimitedWonder(i) or not CyCity.hasBuilding(i) or CyCity.isFreeBuilding(i):
			continue
		info = GC.getBuildingInfo(i)
		if info.getProductionCost() < 1 or info.isNukeImmune() or info.isAutoBuild():
			continue
		validHousesList.append(i)

	if validHousesList:
		iBuilding = validHousesList[GAME.getSorenRandNum(len(validHousesList), "Wildfire")]
		if isLocalHumanPlayer(data.ePlayer) :
			szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBuilding).getTextKey(), ))
			CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBuilding).getButton(), GC.getCOLOR_RED(), CyCity.getX(), CyCity.getY(), True, True)
		CyCity.changeHasBuilding(iBuilding, False)

def doMinorFire(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(data.iCityId)

	iFlammStart = CyCity.getProperties().getValueByProperty(GC.getInfoTypeForString("PROPERTY_FLAMMABILITY"))

	iPop = CyCity.getPopulation()
	iFlammRand = iFlammStart/5 + iPop*iPop

	iBurnBuilding = -1
	iHighFlamm = 0
	for i in xrange(GC.getNumBuildingInfos()):
		if isLimitedWonder(i) or not CyCity.hasBuilding(i) or CyCity.isFreeBuilding(i):
			continue
		info = GC.getBuildingInfo(i)
		if info.getProductionCost() < 1 or info.isNukeImmune() or info.isAutoBuild():
			continue

		randNum = GAME.getSorenRandNum(iFlammRand, "Buildings destroyed by fire.")
		iFlamm = info.getProperties().getValueByProperty(GC.getInfoTypeForString("PROPERTY_FLAMMABILITY"))
		iFlammScore = iFlamm + randNum
		if iFlammScore > iHighFlamm:
			iHighFlamm = iFlammScore
			iBurnBuilding = i

	if iBurnBuilding != -1:
		if isLocalHumanPlayer(data.ePlayer) :
			szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBurnBuilding).getTextKey(), ))
			CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBurnBuilding).getButton(), GC.getCOLOR_RED(), CyCity.getX(), CyCity.getY(), True, True)
		CyCity.changeHasBuilding(iBurnBuilding, False)


def doMajorFire(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(data.iCityId)
	iFlammStart = CyCity.getProperties().getValueByProperty(GC.getInfoTypeForString("PROPERTY_FLAMMABILITY"))
	popScore = CyCity.getPopulation() * CyCity.getPopulation()
	iFlammRand = (iFlammStart / 4) + popScore
	iFlammEnd = iFlammStart * 3 / 4
	iFlammRange = iFlammStart / 10

	for i in xrange(iFlammRange):
		iBurnBuilding = -1
		iHighFlamm = 0
		currFlamm = CyCity.getProperties().getValueByProperty(GC.getInfoTypeForString("PROPERTY_FLAMMABILITY"))
		if currFlamm <= iFlammEnd:
			break
		for j in xrange(GC.getNumBuildingInfos()):
			if isLimitedWonder(j) or not CyCity.hasBuilding(j) or CyCity.isFreeBuilding(j):
				continue
			info = GC.getBuildingInfo(j)
			if info.getProductionCost() < 1 or info.isNukeImmune() or info.isAutoBuild():
				continue

			randNum = GAME.getSorenRandNum(iFlammRand, "Buildings destroyed by fire.")
			iFlamm = info.getProperties().getValueByProperty(GC.getInfoTypeForString("PROPERTY_FLAMMABILITY"))
			iFlammScore = iFlamm + randNum
			if iFlammScore > iHighFlamm:
				iHighFlamm = iFlammScore
				iBurnBuilding = j

		if iBurnBuilding != -1:
			if isLocalHumanPlayer(data.ePlayer) :
				CyInterface().addMessage(
					data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(),
					TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBurnBuilding).getTextKey(),)),
					"AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBurnBuilding).getButton(),
					GC.getCOLOR_RED(), CyCity.getX(), CyCity.getY(), True, True
				)
			CyCity.changeHasBuilding(iBurnBuilding, False)

def doCatastrophicFire(argsList):
	data = argsList[1]
	CyPlayer = GC.getPlayer(data.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(data.iCityId)
	iProp = GC.getInfoTypeForString("PROPERTY_FLAMMABILITY")
	iFlammStart = CyCity.getProperties().getValueByProperty(iProp)
	iPop = CyCity.getPopulation()
	popScore = iPop * iPop
	iFlammRand = iFlammStart/3 + popScore
	iFlammEnd = iFlammStart/2
	iFlammRange = iFlammStart/5

	iKilledRand = GAME.getSorenRandNum(iFlammEnd, "People killed by fire.")
	iKilledPop = iPop * iKilledRand / (iFlammStart + popScore)
	if isLocalHumanPlayer(data.ePlayer) :
		if iKilledPop == 0:
			szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITYFIRE_NOHIT_CITY", (CyCity.getNameKey(), ))
			CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), CyCity.getX(), CyCity.getY(), True, True)
		if iKilledPop > 0:
			CyCity.changePopulation(-iKilledPop)
			szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITYFIRE_HIT_CITY", (iKilledPop, CyCity.getNameKey(), ))
			CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getCOLOR_RED(), CyCity.getX(), CyCity.getY(), True, True)

	for i in xrange(iFlammRange):
		currFlamm = CyCity.getProperties().getValueByProperty(iProp)
		if currFlamm <= iFlammEnd: break
		iBurnBuilding = -1
		iHighFlamm = 0

		for j in xrange(GC.getNumBuildingInfos()):
			if isLimitedWonder(j) or not CyCity.hasBuilding(j) or CyCity.isFreeBuilding(j):
				continue
			info = GC.getBuildingInfo(j)
			if info.getProductionCost() < 1 or info.isNukeImmune() or info.isAutoBuild():
				continue

			iFlammScore = info.getProperties().getValueByProperty(iProp) + GAME.getSorenRandNum(iFlammRand, "Buildings destroyed by fire.")
			if iFlammScore > iHighFlamm:
				iHighFlamm = iFlammScore
				iBurnBuilding = j

		if iBurnBuilding != -1:
			if isLocalHumanPlayer(data.ePlayer) :
				szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBurnBuilding).getTextKey(),))
				CyInterface().addMessage(data.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBurnBuilding).getButton(), GC.getCOLOR_RED(), CyCity.getX(), CyCity.getY(), True, True)
			CyCity.changeHasBuilding(iBurnBuilding, False)

def getHelpWildFire(argsList):
  return TRNSLTR.getText("TXT_KEY_EVENT_WILDFIRE_1_HELP",())

def getHelpMinorFire(argsList):
  return TRNSLTR.getText("TXT_KEY_EVENT_FIRE_MINOR_1_HELP",())

def getHelpMajorFire(argsList):
  return TRNSLTR.getText("TXT_KEY_EVENT_FIRE_MAJOR_1_HELP",())

def getHelpCatastrophicFire(argsList):
  return TRNSLTR.getText("TXT_KEY_EVENT_FIRE_CATASTROPHIC_1_HELP",())

def canTriggerBlarneyVisit(argsList):
	return not GAME.getGameTurn() % 50

def doGlobalWarming(argsList):
	FEATURE_ICE = GC.getInfoTypeForString("FEATURE_ICE")
	TERRAIN_ICE = GC.getInfoTypeForString("TERRAIN_ICE")
	TERRAIN_PERMAFROST = GC.getInfoTypeForString("TERRAIN_PERMAFROST")
	TERRAIN_TUNDRA = GC.getInfoTypeForString("TERRAIN_TUNDRA")
	TERRAIN_TAIGA = GC.getInfoTypeForString("TERRAIN_TAIGA")

	PLOT_LAND = PlotTypes.PLOT_LAND

	MAP = GC.getMap()
	iNumPlots = MAP.numPlots()
	countIce = 1
	iGW = 0
	for plot in MAP.plots():
		iFeature = plot.getFeatureType()
		if iFeature == FEATURE_ICE:
			iGW -= 5
			countIce += 2
		elif iFeature in (
			GC.getInfoTypeForString("FEATURE_FOREST_YOUNG"),
			GC.getInfoTypeForString("FEATURE_SAVANNA")
		):
			iGW -= 1
		elif iFeature in (
			GC.getFEATURE_FOREST(),
			GC.getInfoTypeForString("FEATURE_BAMBOO")
		):
			iGW -= 2
		elif iFeature == GC.getInfoTypeForString("FEATURE_FOREST_ANCIENT"):
			iGW -= 3
		elif iFeature == GC.getFEATURE_JUNGLE():
			iGW -= 4
		elif iFeature == GC.getInfoTypeForString("FEATURE_VOLCANO_ACTIVE"):
			iGW += 5

		iTerrain = plot.getTerrainType()
		if iTerrain == TERRAIN_TAIGA:
			iGW -= 2
			countIce += 1
		elif iTerrain == TERRAIN_TUNDRA:
			iGW -= 4
			countIce += 2
		elif iTerrain == TERRAIN_PERMAFROST:
			iGW -= 6
			countIce += 2
		elif iTerrain == TERRAIN_ICE:
			iGW -= 10
			countIce += 3
		elif iTerrain in (
			GC.getInfoTypeForString("TERRAIN_DUNES"),
			GC.getInfoTypeForString("TERRAIN_SALT_FLATS")
		):
			iGW += 1

		iImprovement = plot.getImprovementType()
		if iImprovement in (
			GC.getInfoTypeForString("IMPROVEMENT_COTTAGE"),
			GC.getInfoTypeForString("IMPROVEMENT_MINE")
		):
			iGW += 1
		elif iImprovement in (
			GC.getInfoTypeForString("IMPROVEMENT_HAMLET"),
			GC.getInfoTypeForString("IMPROVEMENT_FARM"),
			GC.getInfoTypeForString("IMPROVEMENT_SHAFT_MINE")
		):
			iGW += 2
		elif iImprovement in (
			GC.getInfoTypeForString("IMPROVEMENT_VILLAGE"),
			GC.getInfoTypeForString("IMPROVEMENT_WORKSHOP"),
			GC.getInfoTypeForString("IMPROVEMENT_MODERN_MINE")
		):
			iGW += 4
		elif iImprovement in (
			GC.getInfoTypeForString("IMPROVEMENT_TOWN"),
			GC.getInfoTypeForString("IMPROVEMENT_PASTURE"),
			GC.getInfoTypeForString("IMPROVEMENT_EXTRACTION_FACILITY"),
			GC.getInfoTypeForString("IMPROVEMENT_FACTORY"),
			GC.getInfoTypeForString("IMPROVEMENT_HYDROCARBON_WELL")
		):
			iGW += 8
		elif iImprovement == GC.getInfoTypeForString("IMPROVEMENT_MANUFACTURING_COMPLEX"):
			iGW += 16

		if plot.isCity():
			city = plot.getPlotCity()
			iGW += city.isActiveBuilding(GC.getInfoTypeForString("BUILDING_POLLUTION_MINOR_GLOBAL_WARMING")) * 100
			iGW += city.isActiveBuilding(GC.getInfoTypeForString("BUILDING_POLLUTION_MODERATE_GLOBAL_WARMING")) * 10000
			iGW += city.isActiveBuilding(GC.getInfoTypeForString("BUILDING_POLLUTION_MAJOR_GLOBAL_WARMING")) * 1000000

	maxRand = GAME.getEstimateEndTurn() * iNumPlots
	iIce = 2*iNumPlots / countIce

	for plot in MAP.plots():
		randGW = GAME.getSorenRandNum(maxRand, "Global Warming index of plot affected")
		if iGW <= randGW:
			continue
		# First melt glacier
		if plot.getFeatureType() == FEATURE_ICE:
			plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
			continue
		# Then change terrain
		iTerrain = plot.getTerrainType()
		if iTerrain == TERRAIN_ICE:
			plot.setTerrainType(TERRAIN_PERMAFROST, True, True)

		elif iTerrain == TERRAIN_PERMAFROST:
			plot.setTerrainType(TERRAIN_TUNDRA, True, True)

		elif iTerrain == TERRAIN_TUNDRA:
			plot.setTerrainType(TERRAIN_TAIGA, True, True)

		elif iTerrain == TERRAIN_TAIGA:
			plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_MUDDY"), True, True)

		elif iTerrain == GC.getInfoTypeForString("TERRAIN_MUDDY"):
			plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_LUSH"), True, True)

		elif iTerrain == GC.getInfoTypeForString("TERRAIN_LUSH"):
			plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_GRASSLAND"), True, True)

		elif iTerrain == GC.getInfoTypeForString("TERRAIN_GRASSLAND"):
			plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_PLAINS"), True, True)
			iFeature = plot.getFeatureType()
			if iFeature in (GC.getInfoTypeForString("FEATURE_SWAMP"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")):
				plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)

		elif iTerrain in (
			GC.getInfoTypeForString("TERRAIN_PLAINS"),
			GC.getInfoTypeForString("TERRAIN_BARREN"),
			GC.getInfoTypeForString("TERRAIN_ROCKY")
		):
			plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_SCRUB"), True, True)

		elif iTerrain == GC.getInfoTypeForString("TERRAIN_SCRUB"):
			plot.setTerrainType(GC.getTERRAIN_DESERT(), True, True)
			iFeature = plot.getFeatureType()
			if iFeature in (
				GC.getInfoTypeForString("FEATURE_FOREST_YOUNG"),
				GC.getFEATURE_FOREST(),
				GC.getInfoTypeForString("FEATURE_FOREST_ANCIENT"),
				GC.getFEATURE_JUNGLE(),
				GC.getInfoTypeForString("FEATURE_BAMBOO"),
				GC.getInfoTypeForString("FEATURE_SAVANNA"),
				GC.getInfoTypeForString("FEATURE_VERY_TALL_GRASS")
			):
				plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)

		elif iTerrain == GC.getTERRAIN_DESERT():
			plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_DUNES"), True, True)

		elif iTerrain == GC.getInfoTypeForString("TERRAIN_COAST_POLAR"):
			plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_COAST"), True, True)

		elif iTerrain == GC.getInfoTypeForString("TERRAIN_COAST"):
			plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL"), True, True)

		elif iTerrain == GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL") and not plot.isLake():
			if iIce < 100:
				bCoastShift = True
				iDX = plot.getX()
				iDY = plot.getY()
				for iDX in xrange(-1, 2):
					for iDY in xrange(-1, 2):
						if iDX == 0 and iDY == 0:
							continue
						plotX = MAP.plot(iDX, iDY)
						if plotX and plotX.getPlotType() == PLOT_LAND:
							if plotX.isCity():
								bCoastShift = False
								continue
							randFlood = GAME.getSorenRandNum(100, "Global Warming flooding chance")
							if iIce > randFlood:
								plotX.setFeatureType(FeatureTypes.NO_FEATURE, -1)
								plotX.setImprovementType(-1)
								plotX.setBonusType(-1)
								plotX.setImprovementType(-1)
								plotX.setRouteType(-1)
								plotX.setTerrainType(GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL"), True, True)
								plotX.setPlotType(PlotTypes.PLOT_OCEAN, True, True)
							else:
								bCoastShift = False
				if bCoastShift:
					plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_OCEAN_TROPICAL"), True, True)
			else:
				plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
				plot.setImprovementType(-1)
				plot.setBonusType(-1)
				plot.setImprovementType(-1)
				plot.setRouteType(-1)
				plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_SALT_FLATS"), True, True)
				plot.setPlotType(PlotTypes.PLOT_LAND, True, True)

		elif iTerrain == GC.getInfoTypeForString("TERRAIN_OCEAN_POLAR"):
			plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_OCEAN"), True, True)

		elif iTerrain == GC.getInfoTypeForString("TERRAIN_OCEAN"):
			plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_OCEAN_TROPICAL"), True, True)

		elif plot.isLake() and iTerrain in (
			GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL"),
			GC.getInfoTypeForString("TERRAIN_LAKE_SHORE")
		):
			plot.setFeatureType(FeatureTypes.NO_FEATURE, -1)
			plot.setImprovementType(-1)
			plot.setBonusType(-1)
			plot.setImprovementType(-1)
			plot.setRouteType(-1)
			if GC.getInfoTypeForString("TERRAIN_LAKE_SHORE"):
				plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_MUDDY"), True, True)
			else:
				plot.setTerrainType(GC.getInfoTypeForString("TERRAIN_SALT_FLATS"), True, True)
			plot.setPlotType(PlotTypes.PLOT_LAND, True, True)


def getHelpGlobalWarming(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_GLOBAL_WARMING_1_HELP",())

######## TORNADO ###########
def canDoTornado(argsList):
	EventTriggeredData = argsList[0]

	CyPlot = GC.getMap().plot(EventTriggeredData.iPlotX, EventTriggeredData.iPlotY)
	if CyPlot.isCity():
		return 0

	iLatitude = CyPlot.getLatitude()
	if iLatitude < 50 and 30 < iLatitude:
		return 1

	iRandom = GAME.getSorenRandNum(101, "Random Plot") # 0 <-> 100
	if iLatitude < 60 and 20 < iLatitude:
		if iRandom < 20:
			return 1
	elif iRandom < 5:
		return 1
	return 0

def doTornado(argsList):
	EventTriggeredData = argsList[1]
	x, y = EventTriggeredData.iPlotX, EventTriggeredData.iPlotY
	CyPlot = GC.getMap().plot(x, y)
	if 50 > GAME.getSorenRandNum(101, "Random Plot"):
		CyPlot.setImprovementType(-1)

	if 25 > GAME.getSorenRandNum(101, "Random Plot"):
		CyPlot.setRouteType(-1)

	for pUnit in CyPlot.units():
		pUnit.setImmobileTimer(1)

######## Native Good 1 -- lost resources ###########
def canApplyNativegood1(argsList):
	return False
	data = argsList[0]

	player = GC.getPlayer(data.ePlayer)
	if (player.getNativeOpinion()>=4):
		if(0):
			for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
				loopPlayer = GC.getPlayer(iLoopPlayer)
				if loopPlayer.isAlive():
					szBuffer = "canApplyNativegood1 -- was ge 4"
				CyInterface().addMessage(iLoopPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), -1, -1, True, True)
		return 1
	else:
		if (0):
			for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
				loopPlayer = GC.getPlayer(iLoopPlayer)
				if loopPlayer.isAlive():
					szBuffer = "canApplyNativegood1 -- was lt 4"
					CyInterface().addMessage(iLoopPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), -1, -1, True, True)
		return 0

def helpNativegood1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_NATIVEGOOD1_1HELP",())

def ApplyNativegood11(argsList):
	GC.getPlayer(argsList[1].ePlayer).CreateRandomCityBonus()

######## Native Good 2 -- Recruits ###########

def canApplyNativegood2(argsList):
	return False
	data = argsList[0]

	player = GC.getPlayer(data.ePlayer)
	if player.getNativeOpinion() >= 5:
		if (0):
			for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
				loopPlayer = GC.getPlayer(iLoopPlayer)
				if loopPlayer.isAlive():
					szBuffer = "canApplyNativegood2 -- was ge 5"
					CyInterface().addMessage(iLoopPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), -1, -1, True, True)
		return 1
	else:
		if (0):
			for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
				loopPlayer = GC.getPlayer(iLoopPlayer)
				if loopPlayer.isAlive():
					szBuffer = "canApplyNativegood2 -- was lt 5"
					CyInterface().addMessage(iLoopPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), -1, -1, True, True)
		return 0

def helpNativegood2(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_NATIVEGOOD2_1HELP",())

def ApplyNativegood2(argsList):
	data = argsList[1]

	CyPlayer = GC.getPlayer(data.ePlayer)
	iRelationship = CyPlayer.getNativeRelationship()
	iRelationship += 40
	CyPlayer.setNativeRelationship(iRelationship)

	CyCity = CyPlayer.getCapitalCity()
	if CyCity is None:
		return
	aList = (UnitAITypes.UNITAI_ATTACK, UnitAITypes.UNITAI_CITY_DEFENSE)
	iHighest = 0
	eBestUnit = -1
	for iUnit in xrange(GC.getNumUnitInfos()):
		CvUnitInfo = GC.getUnitInfo(iUnit)
		if CvUnitInfo.getDomainType() != DomainTypes.DOMAIN_LAND or CvUnitInfo.getDefaultUnitAIType() not in aList:
			continue
		if CyCity.canTrain(iUnit, False, True):
			iValue = CvUnitInfo.getProductionCost()
			if iValue >= iHighest:
				iHighest = iValue
				eBestUnit = iUnit

	for CyCity in CyPlayer.cities():
		x = CyCity.getX()
		y = CyCity.getY()
		for j in xrange(3):
			CyPlayer.initUnit(eBestUnit, x, y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)

######## Native Good 3 -- gold ###########

def canApplyNativegood3(argsList):
	return False
	data = argsList[0]

	player = GC.getPlayer(data.ePlayer)

	if player.getNativeOpinion() >= 4:
		if (0):
			for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
				loopPlayer = GC.getPlayer(iLoopPlayer)
				if loopPlayer.isAlive():
					szBuffer = "canApplyNativegood3 -- was ge 4"
					CyInterface().addMessage(iLoopPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), -1, -1, True, True)
		return 1
	else:
		for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
			if (0):
				loopPlayer = GC.getPlayer(iLoopPlayer)
				if loopPlayer.isAlive():
					szBuffer = "canApplyNativegood3 -- was lt 4"
					CyInterface().addMessage(iLoopPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), -1, -1, True, True)
		return 0

def helpNativegood3(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_NATIVEGOOD3_1HELP",())

def ApplyNativegood3(argsList):
	data = argsList[1]

	GC.getPlayer(data.ePlayer).changeGold(50 + GAME.getSorenRandNum(50, "random native gold"))

######## Native Good 4 -- Ancient Tech ###########

def canApplyNativegood4(argsList):
	return False
	data = argsList[0]

	player = GC.getPlayer(data.ePlayer)

	if player.getCurrentEra() <= 3:
		return 0

	if player.getNativeOpinion() >= 5:
		if (0):
			for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
				loopPlayer = GC.getPlayer(iLoopPlayer)
				if loopPlayer.isAlive():
					szBuffer = "canApplyNativegood4 -- was ge 4"
					CyInterface().addMessage(iLoopPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), -1, -1, True, True)
		return 1
	else:
		for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
			if (0):
				loopPlayer = GC.getPlayer(iLoopPlayer)
				if loopPlayer.isAlive():
					szBuffer = "canApplyNativegood4 -- was lt 4"
					CyInterface().addMessage(iLoopPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), -1, -1, True, True)
		return 0

def helpNativegood4(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_NATIVEGOOD4_HELP",())

def ApplyNativegood4(argsList):
	data = argsList[1]

	GC.getPlayer(data.ePlayer).grantOldTech()

### Missions ####

def doEventLawyer(argsList):
	pUnit = argsList[0]

	if pUnit is None:
		return # False call

	pPlayer = GC.getPlayer(pUnit.getOwner())
	iPlayer = pPlayer.getID()
	pCity = pUnit.plot().getPlotCity()

	if pCity is None:
		return # False call

	#Gets a list of all corporations in the city
	#CyInterface().playGeneralSound("AS2D_BUILD_BANK")
	lCityCorporations = [ ]
	for iCorpLoop in xrange(GC.getNumCorporationInfos( )):
		if pCity.isHasCorporation( iCorpLoop):
			iHC = 0
		if pCity.isHeadquartersByType(iCorpLoop):
			iHC = -75

	# Does litigation succeed
	if GAME.getSorenRandNum(101, "Lawyer") < 95 - ((len( lCityCorporations ))*5) + iHC:

		# Removes buildings
		for iBuildingLoop in xrange(GC.getNumBuildingInfos( )):
			if pCity.hasBuilding(iBuildingLoop):
				pBuilding = GC.getBuildingInfo( iBuildingLoop )
				iRequiredCorporation = pBuilding.getFoundsCorporation( )
				for iCorpLoop in xrange(GC.getNumCorporationInfos()):
					if iRequiredCorporation == iCorpLoop:
						pCity.changeHasBuilding(iBuildingLoop, False)

		# Loop through all corporations, remove them from the city
		for iCorpLoop in xrange(GC.getNumCorporationInfos()):
			pCity.setHasCorporation(iCorpLoop, 0, 0, 0)

		# Litigation succeeds
		CyInterface().addMessage(iPlayer,False,25,CyTranslator().getText("TXT_KEY_MSG_LITIGATION",(pCity.getName(),)),"AS2D_BUILD_BANK",InterfaceMessageTypes.MESSAGE_TYPE_INFO,pUnit.getButton(),ColorTypes(8),pCity.getX(),pCity.getY(),True,True)

	# Litigation fails
	else:
		CyInterface().addMessage(iPlayer,False,25,CyTranslator().getText("TXT_KEY_MSG_LITIGATION_FAIL",(pCity.getName(),)),"AS2D_SABOTAGE",InterfaceMessageTypes.MESSAGE_TYPE_INFO,pUnit.getButton(),ColorTypes(7),pCity.getX(),pCity.getY(),True,True)

######## Civil War ########

def canTriggerCivilWar(argsList):
	data = argsList[0]
	player = GC.getPlayer(data.ePlayer)
	pCity = player.getCity(data.iCityId)

	if GC.getMAX_PC_PLAYERS() == GAME.countCivPlayersAlive():
		return False
	if player.getNumCities() <= 1:
		return False
	if pCity.isCapital():
		return False

	return True

def applyCivilWar(argsList):
	data = argsList[1]
	DeclareWar = True
	dice = GAME.getMapRand()

	lAlive = []
	del lAlive[:]

	iMaxCiv = GC.getMAX_PC_PLAYERS()
	for i in xrange(iMaxCiv):
		pPlayer = GC.getPlayer(i)
		lAlive.append(pPlayer.getCivilizationType())
		lAlive.append(-1)

	iAllCivs = GC.getNumPlayableCivilizationInfos()

	pPlayer = GC.getPlayer(data.ePlayer)
	iDerivative = GC.getCivilizationInfo(pPlayer.getCivilizationType()).getDerivativeCiv()
	if not iDerivative in lAlive:
		iNewCiv = iDerivative
	else:
		lNotAlive = []
		del lNotAlive[:]
		for i in xrange(iAllCivs):
			if i in lAlive:
				continue
			lNotAlive.append(i)

		iTempCiv = dice.get(len(lNotAlive), "Civil War Civilization")
		iNewCiv = lNotAlive[iTempCiv]

	CurCiv = GC.getCivilizationInfo(iNewCiv)
	NumLeaders = CurCiv.getNumLeaders()
	LeaderNum = dice.get(NumLeaders, "Civil War Leader")
	LeaderCounter = 0
	for iLeaders in xrange(GC.getNumLeaderHeadInfos()):
		if CurCiv.isLeaders(iLeaders):
			if NumLeaders==1:
				NewLeaderID = iLeaders
				break
			else:
				if LeaderCounter == LeaderNum:
					NewLeaderID = iLeaders
					break
		LeaderCounter += 1

	# Find ID for new civ
	iNewID = -1
	for iID in xrange(iMaxCiv):
		pTestPlayer = GC.getPlayer(iID)
		if not pTestPlayer.isAlive():
			iNewID = iID
			break
		else:
			return False

	# Add the player to the game
	GAME.addPlayer(iNewID, NewLeaderID, iNewCiv)

	pNewPlayer = GC.getPlayer(iNewID)
	pNewTeam = GC.getTeam(pNewPlayer.getTeam())
	pPlayer = GC.getPlayer(data.ePlayer)
	pCity = pPlayer.getCity(data.iCityId)
	pTriggerTeam = GC.getTeam(pPlayer.getTeam())

	# Give new team open borders tech (for unit handover)
	iMaxTech = GC.getNumTechInfos()
	for OBtech in xrange(iMaxTech):
		pTech = GC.getTechInfo(OBtech)
		if pTech.isOpenBordersTrading():
			pNewTeam.setHasTech(OBtech, True, iNewID, False, False)
			break
	pNewTeam.signOpenBorders(pTriggerTeam.getID())

	# Remove open borders tech
	pNewTeam.setHasTech(OBtech, False, iNewID, False, False)

	# Add techs to new player
	for counter in xrange(iMaxTech):
		if pTriggerTeam.isHasTech(counter) and not pNewTeam.isHasTech(counter):
			pNewTeam.setHasTech(counter, True, iNewID, False, False)

	# Hand over cities
	iX = pCity.getX()
	iY = pCity.getY()
	#CyInterface().addMessage(0,False,15,str(data.iCityId),'',0,'Art/Interface/Buttons/General/happy_person.dds',ColorTypes(44), iX, iY, True,True)
	pNewPlayer.acquireCity(pCity, False, False)

	# Hand over units
	for pUnit in CyMap().plot(iX, iY).units():
		if pUnit.getOwner() == pPlayer.getID():
			pUnit.doCommand(CommandTypes.COMMAND_GIFT, -1, -1)

	for i in xrange(2):
		pNewPlayer.initUnit(pCity.getConscriptUnit(), iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)

	# Declare war
	if DeclareWar:
		pNewTeam.declareWar(pTriggerTeam.getID(), False, WarPlanTypes.WARPLAN_LIMITED)

	return True

################ BEST HUNTERS ################
def canDoBestHunters1(argsList):
	return GAME.isOption(GameOptionTypes.GAMEOPTION_COMBAT_WITHOUT_WARNING)

def canDoBestHunters2(argsList):
	return GAME.isOption(GameOptionTypes.GAMEOPTION_COMBAT_HIDE_SEEK) and GAME.isOption(GameOptionTypes.GAMEOPTION_COMBAT_SIZE_MATTERS)
