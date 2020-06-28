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


######## BLESSED SEA ###########

def getHelpBlessedSea1(argsList):

	iOurMinLandmass = (3 * GC.getWorldInfo(CyMap().getWorldSize()).getDefaultPlayers()) / 2

	return TRNSLTR.getText("TXT_KEY_EVENT_BLESSED_SEA_HELP", (iOurMinLandmass, ))

def canTriggerBlessedSea(argsList):
	kTriggeredData = argsList[0]
	MAP = GC.getMap()

	if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE) and GC.getPlayer(kTriggeredData.ePlayer).isHuman():
		return False

	iMapMinLandmass = 2 * GC.getWorldInfo(MAP.getWorldSize()).getDefaultPlayers()
	iOurMaxLandmass = iMapMinLandmass / 4

	if (MAP.getNumLandAreas() < iMapMinLandmass):
		return False

	iOurLandmasses = 0
	for i in xrange(MAP.getIndexAfterLastArea()):
		area = MAP.getArea(i)
		if not area.isNone() and not area.isWater() and area.getCitiesPerPlayer(kTriggeredData.ePlayer) > 0:
			iOurLandmasses += 1

	if (iOurLandmasses > iOurMaxLandmass):
		return False

	player = GC.getPlayer(kTriggeredData.ePlayer)
	if not(player.getUnitCount(GC.getInfoTypeForString("UNIT_GALLEY"))
		or player.getUnitCount(GC.getInfoTypeForString("UNIT_CARAVEL"))
		or player.getUnitCount(GC.getInfoTypeForString("UNIT_GALLEON"))
		or player.getUnitCount(GC.getInfoTypeForString("UNIT_WARGALLEY"))
		or player.getUnitCount(GC.getInfoTypeForString("UNIT_FLUYT"))
		or player.getUnitCount(GC.getInfoTypeForString("UNIT_BRIGANTINE"))
	): return False
	return True

def canTriggerBlessedSea2(argsList):

  kTriggeredData = argsList[0]
  map = GC.getMap()
  iOurMinLandmass = (3 * GC.getWorldInfo(map.getWorldSize()).getDefaultPlayers()) / 2

  iOurLandmasses = 0
  for i in xrange(map.getIndexAfterLastArea()):
    area = map.getArea(i)
    if not area.isNone() and not area.isWater() and area.getCitiesPerPlayer(kTriggeredData.ePlayer) > 0:
      iOurLandmasses += 1

  if (iOurLandmasses < iOurMinLandmass):
    return False

  return True

def applyBlessedSea2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iBuilding = -1

  if (-1 != kTriggeredData.eReligion):
    for i in xrange(GC.getNumBuildingInfos()):
      if (GC.getBuildingInfo(i).getSpecialBuildingType() == GC.getInfoTypeForString("SPECIALBUILDING_TEMPLE")):
        if (GC.getBuildingInfo(i).getReligionType() == kTriggeredData.eReligion):
          iBuilding = i
          break


  if (iBuilding == -1):
    return

  player = GC.getPlayer(kTriggeredData.ePlayer)
  (loopCity, iter) = player.firstCity(False)

  while(loopCity):

    if (loopCity.getPopulation() >= 5):
      if (loopCity.canConstruct(iBuilding, False, False, True)):
        loopCity.setNumRealBuilding(iBuilding, 1)

    (loopCity, iter) = player.nextCity(iter, False)


def canApplyBlessedSea2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iBuilding = -1

  if (-1 != kTriggeredData.eReligion):
    for i in xrange(GC.getNumBuildingInfos()):
      if (GC.getBuildingInfo(i).getSpecialBuildingType() == GC.getInfoTypeForString("SPECIALBUILDING_TEMPLE")):
        if (GC.getBuildingInfo(i).getReligionType() == kTriggeredData.eReligion):
          iBuilding = i
          break


  if (iBuilding == -1):
    return False

  player = GC.getPlayer(kTriggeredData.ePlayer)
  (loopCity, iter) = player.firstCity(False)
  bFound = False

  while(loopCity):

    if (loopCity.getPopulation() >= 5):
      if (loopCity.canConstruct(iBuilding, False, False, True)):
        bFound = True
        break

    (loopCity, iter) = player.nextCity(iter, False)

  return bFound


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

	if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE):
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
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  otherPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
  team = GC.getTeam(player.getTeam())

  if (team.AI_getAtWarCounter(otherPlayer.getTeam()) == 1):
    (loopUnit, iter) = otherPlayer.firstUnit(False)
    while( loopUnit):
      plot = loopUnit.plot()
      if (not plot.isNone()):
        if (plot.getOwner() == kTriggeredData.ePlayer):
          return True
      (loopUnit, iter) = otherPlayer.nextUnit(iter, False)

  return False

######## WEDDING FEUD ###########

def doWeddingFeud2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  (loopCity, iter) = player.firstCity(False)

  while(loopCity):
    if loopCity.isHasReligion(kTriggeredData.eReligion):
      loopCity.changeHappinessTimer(30)
    (loopCity, iter) = player.nextCity(iter, False)

  return 1

def getHelpWeddingFeud2(argsList):
  iEvent = argsList[0]
  event = GC.getEventInfo(iEvent)
  kTriggeredData = argsList[1]
  religion = GC.getReligionInfo(kTriggeredData.eReligion)

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_WEDDING_FEUD_2_HELP", (GC.getDefineINT("TEMP_HAPPY"), 30, religion.getChar()))

  return szHelp

def canDoWeddingFeud3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)

  if player.getEffectiveGold() - 10 * player.getNumCities() < 0:
    return False

  # do not use this event in multiplayer
  if GAME.isNetworkMultiPlayer():
    return False

  return True


def doWeddingFeud3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  destPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iLoopPlayer)
    if loopPlayer.isAlive() and loopPlayer.getStateReligion() == player.getStateReligion():
      loopPlayer.AI_changeAttitudeExtra(kTriggeredData.ePlayer, 1)
      player.AI_changeAttitudeExtra(iLoopPlayer, 1)

  if GC.getTeam(destPlayer.getTeam()).canDeclareWar(player.getTeam()):
    if destPlayer.isHuman():
      # this works only because it's a single-player only event
      popupInfo = CyPopupInfo()
      popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
      popupInfo.setText(TRNSLTR.getText("TXT_KEY_EVENT_WEDDING_FEUD_OTHER_3", (GC.getReligionInfo(kTriggeredData.eReligion).getAdjectiveKey(), player.getCivilizationShortDescriptionKey())))
      popupInfo.setData1(kTriggeredData.eOtherPlayer)
      popupInfo.setData2(kTriggeredData.ePlayer)
      popupInfo.setPythonModule("CvRandomEventInterface")
      popupInfo.setOnClickedPythonCallback("weddingFeud3Callback")
      popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_YES", ()), "")
      popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_NO", ()), "")
      popupInfo.addPopup(kTriggeredData.eOtherPlayer)
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
  iEvent = argsList[0]
  event = GC.getEventInfo(iEvent)
  kTriggeredData = argsList[1]
  religion = GC.getReligionInfo(kTriggeredData.eReligion)

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_WEDDING_FEUD_3_HELP", (1, religion.getChar()))

  return szHelp

######## SPICY ###########

def canTriggerSpicy(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iSpice = GC.getInfoTypeForString("BONUS_SPICES")
  iHappyBonuses = 0
  bSpices = False
  for i in xrange(GC.getNumBonusInfos()):
    bonus = GC.getBonusInfo(i)
    iNum = player.getNumAvailableBonuses(i)
    if iNum > 0 :
      if bonus.getHappiness() > 0:
        iHappyBonuses += 1
        if iHappyBonuses > 4:
          return False
      if i == iSpice:
        return False

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  if not plot.canHaveBonus(iSpice, False):
    return False

  return True

def doSpicy2(argsList):
# need this because plantations are notmally not allowed unless there are already spices
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  if not plot.isNone():
    plot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PLANTATION"))

  return 1

def getHelpSpicy2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iPlantation = GC.getInfoTypeForString("IMPROVEMENT_PLANTATION")
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_IMPROVEMENT_GROWTH", ( GC.getImprovementInfo(iPlantation).getTextKey(), ))

  return szHelp

######## BABY BOOM ###########

def canTriggerBabyBoom(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  team = GC.getTeam(player.getTeam())

  if team.getAtWarCount(True) > 0:
    return False

  for iLoopTeam in xrange(GC.getMAX_PC_TEAMS()):
    if iLoopTeam != player.getTeam():
      if team.AI_getAtPeaceCounter(iLoopTeam) == 1:
        return True

  return False

######## BARD TALE ###########

def applyBardTale3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)

  player.changeGold(-10 * player.getNumCities())

def canApplyBardTale3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)

  if player.getEffectiveGold() - 10 * player.getNumCities() < 0:
    return False

  return True


def getHelpBardTale3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_GOLD_LOST", (10 * player.getNumCities(), ))

  return szHelp

######## LOOTERS ###########

def getHelpLooters3(argsList):

	kTriggeredData = argsList[1]
	otherPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
	city = otherPlayer.getCity(kTriggeredData.iOtherPlayerCityId)

	return TRNSLTR.getText("TXT_KEY_EVENT_LOOTERS_3_HELP", (1, 2, city.getNameKey()))


def canApplyLooters3(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]

	CyPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(kTriggeredData.iOtherPlayerCityId)

	for i in xrange(GC.getNumBuildingInfos()):
		if CyTeam.isObsoleteBuilding(i) or CyCity.getNumRealBuilding(i) < 1: continue
		CvBuilding = GC.getBuildingInfo(i)
		iCost = CvBuilding.getProductionCost()
		if iCost <= 100 and iCost > 0 and not isLimitedWonder(i) and not CvBuilding.isAutoBuild():

			for k in xrange(GC.getNumBuildingInfos()):
				if CyCity.getNumRealBuilding(k) and CvBuilding.isReplaceBuilding(k):
					break
			else: return True
	return False


def applyLooters3(argsList):

	kTriggeredData = argsList[1]

	CyPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(kTriggeredData.iOtherPlayerCityId)

	aList = []
	for i in xrange(GC.getNumBuildingInfos()):
		if CyTeam.isObsoleteBuilding(i) or CyCity.getNumRealBuilding(i) < 1: continue
		CvBuilding = GC.getBuildingInfo(i)
		iCost = CvBuilding.getProductionCost()
		if iCost <= 100 and iCost > 0 and not isLimitedWonder(i) and not CvBuilding.isAutoBuild():

			for k in xrange(GC.getNumBuildingInfos()):
				if CyCity.getNumRealBuilding(k) > 0 and CvBuilding.isReplaceBuilding(k):
					break
			else: aList.append(i)

	iMax = GAME.getSorenRandNum(2, "Looters event number of buildings destroyed") + 1
	iCount = 0
	while aList and iCount < iMax:
		iBuilding = aList[GAME.getSorenRandNum(len(aList), "Looters event building destroyed")]
		szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBuilding).getTextKey(), ))
		CyInterface().addMessage(kTriggeredData.eOtherPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBuilding).getButton(), GC.getInfoTypeForString("COLOR_RED"), CyCity.getX(), CyCity.getY(), True, True)
		CyCity.setNumRealBuilding(iBuilding, 0)
		aList.remove(iBuilding)
		iCount += 1

	szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_NUM_BUILDINGS_DESTROYED", (iCount, GC.getPlayer(kTriggeredData.eOtherPlayer).getCivilizationAdjectiveKey(), CyCity.getNameKey()))
	CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), -1, -1, True, True)

######## BROTHERS IN NEED ###########

def canTriggerBrothersInNeed(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)
  otherPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)

  if not player.canTradeNetworkWith(kTriggeredData.eOtherPlayer):
    return False

  listResources = []
  listResources.append(GC.getInfoTypeForString("BONUS_COPPER_ORE"))
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
      if GC.getTeam(iTeam).isAtWar(otherPlayer.getTeam()) and not GC.getTeam(iTeam).isAtWar(player.getTeam()):
        return True

  return False

def canDoBrothersInNeed1(argsList):
  kTriggeredData = argsList[1]
  newArgs = (kTriggeredData, )

  return canTriggerBrothersInNeed(newArgs)

######## HURRICANE ###########

def canTriggerHurricaneCity(argsList):
	iPlayer = argsList[1]
	iCity = argsList[2]
	CyCity = GC.getPlayer(iPlayer).getCity(iCity)

	if CyCity.isNone():
		return False

	if not CyCity.isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()):
		return False

	iLat = CyCity.plot().getLatitude()
	if iLat < 0 or iLat > 30: return False

	if CyCity.getPopulation() < 2: return False
	return True

def canApplyHurricane1(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]

	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(kTriggeredData.iCityId)

	for i in xrange(GC.getNumBuildingInfos()):
		if CyTeam.isObsoleteBuilding(i) or CyCity.getNumRealBuilding(i) < 1: continue
		CvBuilding = GC.getBuildingInfo(i)
		if CvBuilding.isNukeImmune() or CvBuilding.isAutoBuild() or CvBuilding.getProductionCost() < 1 or isLimitedWonder(i):
			continue

		for k in xrange(GC.getNumBuildingInfos()):
			if CyCity.getNumRealBuilding(k) > 0 and CvBuilding.isReplaceBuilding(k):
				break
		else: return True
	return False

def canApplyHurricane2(argsList):
	return not canApplyHurricane1(argsList)


def applyHurricane1(argsList):
	kTriggeredData = argsList[1]

	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(kTriggeredData.iCityId)

	aList = []
	for i in xrange(GC.getNumBuildingInfos()):
		if CyTeam.isObsoleteBuilding(i) or CyCity.getNumRealBuilding(i) < 1: continue
		CvBuilding = GC.getBuildingInfo(i)
		if CvBuilding.isNukeImmune() or CvBuilding.isAutoBuild() or CvBuilding.getProductionCost() < 1 or isLimitedWonder(i):
			continue

		for k in xrange(GC.getNumBuildingInfos()):
			if CyCity.getNumRealBuilding(k) > 0 and CvBuilding.isReplaceBuilding(k):
				break
		else: aList.append(i)

	if aList:
		iBuilding = aList[GAME.getSorenRandNum(len(aList), "Hurricane")]
		szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBuilding).getTextKey(), ))
		CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBuilding).getButton(), GC.getInfoTypeForString("COLOR_RED"), CyCity.getX(), CyCity.getY(), True, True)
		CyCity.setNumRealBuilding(iBuilding, 0)


######## CYCLONE ###########

def canTriggerCycloneCity(argsList):
	iPlayer = argsList[1]
	iCity = argsList[2]
	CyCity = GC.getPlayer(iPlayer).getCity(iCity)

	if CyCity.isNone():
		return False

	if not CyCity.isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()):
		return False

	iLat = CyCity.plot().getLatitude()
	if iLat >= 0 or iLat < -30: return False

	if CyCity.getPopulation() < 2: return False
	return True

######## TSUNAMI ###########

def canTriggerTsunamiCity(argsList):
  eTrigger = argsList[0]
  ePlayer = argsList[1]
  iCity = argsList[2]

  player = GC.getPlayer(ePlayer)
  city = player.getCity(iCity)

  if city.isNone():
    return False

  if not city.isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()):
    return False

  return True

def canApplyTsunami1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  city = player.getCity(kTriggeredData.iCityId)

  return (city.getPopulation() < 6)

def canApplyTsunami2(argsList):
  return (not canApplyTsunami1(argsList))


def applyTsunami1(argsList):

	GC.getPlayer(argsList[1].ePlayer).getCity(argsList[1].iCityId).kill()


def applyTsunami2(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]

	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(kTriggeredData.iCityId)

	listBuildings = []
	for j in xrange(GC.getNumBuildingInfos()):
		if CyTeam.isObsoleteBuilding(j) or CyCity.getNumRealBuilding(j) < 1: continue
		CvBuilding = GC.getBuildingInfo(j)
		if CvBuilding.getProductionCost() > 0  and not isLimitedWonder(j) and not CvBuilding.isAutoBuild():

			for k in xrange(GC.getNumBuildingInfos()):
				if CyCity.getNumRealBuilding(k) > 0 and CvBuilding.isReplaceBuilding(k):
					break
			else: listBuildings.append(j)

	for i in xrange(5):
		if len(listBuildings) > 0:
			iBuilding = listBuildings[GAME.getSorenRandNum(len(listBuildings), "Tsunami event building destroyed")]
			szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBuilding).getTextKey(), ))
			CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBuilding).getButton(), GC.getInfoTypeForString("COLOR_RED"), CyCity.getX(), CyCity.getY(), True, True)
			CyCity.setNumRealBuilding(iBuilding, 0)
			listBuildings.remove(iBuilding)


def getHelpTsunami2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  city = player.getCity(kTriggeredData.iCityId)

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_TSUNAMI_2_HELP", (5, city.getNameKey()))

  return szHelp


######## MONSOON ###########

def canTriggerMonsoonCity(argsList):
  eTrigger = argsList[0]
  ePlayer = argsList[1]
  iCity = argsList[2]

  player = GC.getPlayer(ePlayer)
  city = player.getCity(iCity)

  if city.isNone():
    return False

  if city.isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()):
    return False

  iJungleType = GC.getInfoTypeForString("FEATURE_JUNGLE")

  for iDX in xrange(-3, 4):
    for iDY in xrange(-3, 4):
      pLoopPlot = plotXY(city.getX(), city.getY(), iDX, iDY)
      if not pLoopPlot.isNone() and pLoopPlot.getFeatureType() == iJungleType:
        return True

  return False

######## VOLCANO ###########

def getHelpVolcano1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_VOLCANO_1_HELP", ())

  return szHelp

def canApplyVolcano1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iNumImprovements = 0
  for iDX in xrange(-1, 2):
    for iDY in xrange(-1, 2):
      loopPlot = plotXY(kTriggeredData.iPlotX, kTriggeredData.iPlotY, iDX, iDY)
      if not loopPlot.isNone():
        if (iDX != 0 or iDY != 0):
          if loopPlot.getImprovementType() != -1:
            iNumImprovements += 1

  return (iNumImprovements > 0)

def applyVolcano1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  listPlots = []
  for iDX in xrange(-1, 2):
    for iDY in xrange(-1, 2):
      loopPlot = plotXY(kTriggeredData.iPlotX, kTriggeredData.iPlotY, iDX, iDY)
      if not loopPlot.isNone():
        if (iDX != 0 or iDY != 0):
          if loopPlot.getImprovementType() != -1:
            listPlots.append(loopPlot)

  listRuins = []
  listRuins.append(GC.getInfoTypeForString("IMPROVEMENT_COTTAGE"))
  listRuins.append(GC.getInfoTypeForString("IMPROVEMENT_HAMLET"))
  listRuins.append(GC.getInfoTypeForString("IMPROVEMENT_VILLAGE"))
  listRuins.append(GC.getInfoTypeForString("IMPROVEMENT_TOWN"))

  iRuins = GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS")

  for i in xrange(3):
    if len(listPlots) > 0:
      plot = listPlots[GAME.getSorenRandNum(len(listPlots), "Volcano event improvement destroyed")]
      iImprovement = plot.getImprovementType()
      szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getImprovementInfo(iImprovement).getTextKey(), ))
      CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getImprovementInfo(iImprovement).getButton(), GC.getInfoTypeForString("COLOR_RED"), plot.getX(), plot.getY(), True, True)
      if iImprovement in listRuins:
        plot.setImprovementType(iRuins)
      else:
        plot.setImprovementType(-1)
      listPlots.remove(plot)

      if i == 1 and GAME.getSorenRandNum(100, "Volcano event num improvements destroyed") < 50:
        break

######## DUSTBOWL ###########

def canTriggerDustbowlCont(argsList):
  kTriggeredData = argsList[0]

  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))

  if (kOrigTriggeredData == None):
    return False

  iFarmType = GC.getInfoTypeForString("IMPROVEMENT_FARM")
  iPlainsType = GC.getInfoTypeForString("TERRAIN_PLAINS")

  map = GC.getMap()
  iBestValue = map.getGridWidth() + map.getGridHeight()
  bestPlot = None
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == kTriggeredData.ePlayer and plot.getImprovementType() == iFarmType and plot.getTerrainType() == iPlainsType):
      iValue = plotDistance(kOrigTriggeredData.iPlotX, kOrigTriggeredData.iPlotY, plot.getX(), plot.getY())
      if iValue < iBestValue:
        iBestValue = iValue
        bestPlot = plot

  if bestPlot != None:
    kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
    kActualTriggeredDataObject.iPlotX = bestPlot.getX()
    kActualTriggeredDataObject.iPlotY = bestPlot.getY()
  else:
    player.resetEventOccured(trigger.getPrereqEvent(0))
    return False

  return True

def getHelpDustBowl2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_DUSTBOWL_2_HELP", ())

  return szHelp

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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SALTPETER_HELP", (getSaltpeterNumExtraPlots(), ))

  return szHelp

def canApplySaltpeter(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  map = GC.getMap()

  player = GC.getPlayer(kTriggeredData.ePlayer)

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  if (plot == None):
    return False

  iForest = GC.getInfoTypeForString("FEATURE_FOREST")

  iNumPlots = 0
  for i in xrange(map.numPlots()):
    loopPlot = map.plotByIndex(i)
    if (loopPlot.getOwner() == kTriggeredData.ePlayer and loopPlot.getFeatureType() == iForest and loopPlot.isHills()):
      iDistance = plotDistance(kTriggeredData.iPlotX, kTriggeredData.iPlotY, loopPlot.getX(), loopPlot.getY())
      if iDistance > 0:
        iNumPlots += 1

  return (iNumPlots >= getSaltpeterNumExtraPlots())

def applySaltpeter(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  map = GC.getMap()

  player = GC.getPlayer(kTriggeredData.ePlayer)

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  if (plot == None):
    return

  iForest = GC.getInfoTypeForString("FEATURE_FOREST")

  listPlots = []
  for i in xrange(map.numPlots()):
    loopPlot = map.plotByIndex(i)
    if (loopPlot.getOwner() == kTriggeredData.ePlayer and loopPlot.getFeatureType() == iForest and loopPlot.isHills()):
      iDistance = plotDistance(kTriggeredData.iPlotX, kTriggeredData.iPlotY, loopPlot.getX(), loopPlot.getY())
      if iDistance > 0:
        listPlots.append((iDistance, loopPlot))

  listPlots.sort(key=itemgetter(0))

  iCount = getSaltpeterNumExtraPlots()
  for loopPlot in listPlots:
    if iCount == 0:
      break
    iCount -= 1
    GAME.setPlotExtraYield(loopPlot[1].getX(), loopPlot[1].getY(), YieldTypes.YIELD_COMMERCE, 1)
    CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), TRNSLTR.getText("TXT_KEY_EVENT_SALTPETER_DISCOVERED", ()), "", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_WHITE"), loopPlot[1].getX(), loopPlot[1].getY(), True, True)

######## GREAT DEPRESSION ###########

def applyGreatDepression(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  corporation = GC.getCorporationInfo(kTriggeredData.eCorporation)

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      loopPlayer.changeGold(-loopPlayer.getEffectiveGold()/4)

      if iPlayer != kTriggeredData.ePlayer:
        szText = TRNSLTR.getText("TXT_KEY_EVENTTRIGGER_GREAT_DEPRESSION", (player.getCivilizationAdjectiveKey(), u"", u"", u"", u"", corporation.getTextKey()))
        szText += u"\n\n" + TRNSLTR.getText("TXT_KEY_EVENT_GREAT_DEPRESSION_HELP", (25, ))
        popupInfo = CyPopupInfo()
        popupInfo.setText(szText)
        popupInfo.addPopup(iPlayer)


def getHelpGreatDepression(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_GREAT_DEPRESSION_HELP", (25, ))

  return szHelp

######## CHAMPION ###########

def canTriggerChampion(argsList):
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  team = GC.getTeam(player.getTeam())

  if team.getAtWarCount(True) > 0:
    return False

  return True

def canTriggerChampionUnit(argsList):
  eTrigger = argsList[0]
  ePlayer = argsList[1]
  iUnit = argsList[2]

  player = GC.getPlayer(ePlayer)
  unit = player.getUnit(iUnit)

  if unit.isNone():
    return False

  if unit.getDamage() > 0:
    return False

  if unit.getExperience() < 5:
    return False

  iLeadership = GC.getInfoTypeForString("PROMOTION_LEADERSHIP")
  if unit.isHasPromotion(iLeadership):
    return False

  return True

def applyChampion(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  unit = player.getUnit(kTriggeredData.iUnitId)

  iLeadership = GC.getInfoTypeForString("PROMOTION_LEADERSHIP")

  unit.setHasPromotion(iLeadership, True)

def getHelpChampion(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  unit = player.getUnit(kTriggeredData.iUnitId)

  iLeadership = GC.getInfoTypeForString("PROMOTION_LEADERSHIP")

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CHAMPION_HELP", (unit.getNameKey(), GC.getPromotionInfo(iLeadership).getTextKey()))

  return szHelp

######## ELECTRIC COMPANY ###########

def canTriggerElectricCompany(argsList):
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)

  player = GC.getPlayer(kTriggeredData.ePlayer)

  (loopCity, iter) = player.firstCity(False)

  while(loopCity):

    if (loopCity.angryPopulation(0) > 0):
      return False

    (loopCity, iter) = player.nextCity(iter, False)

  return True

######## GOLD RUSH ###########

def canTriggerGoldRush(argsList):
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)

  iIndustrial = GC.getInfoTypeForString("C2C_ERA_INDUSTRIAL")

  if player.getCurrentEra() != iIndustrial:
    return False


  return True

######## INFLUENZA ###########

def canTriggerInfluenza(argsList):
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  team = GC.getTeam(player.getTeam())

  iIndustrial = GC.getInfoTypeForString("C2C_ERA_INDUSTRIAL")

  if player.getCurrentEra() <= iIndustrial:
    return False

  iMedicine = GC.getInfoTypeForString("TECH_MEDICINE")

  if team.isHasTech(iMedicine):
    return False

  return True

def applyInfluenza2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

  iNumCities = 2 + GAME.getSorenRandNum(3, "Influenza event number of cities")

  listCities = []
  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    if loopCity.getPopulation() > 2:
      iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())
      if iDistance > 0:
        listCities.append((iDistance, loopCity))
    (loopCity, iter) = player.nextCity(iter, False)

  listCities.sort(key=itemgetter(0))

  if iNumCities > len(listCities):
    iNumCities = len(listCities)

  for i in xrange(iNumCities):
    (iDist, loopCity) = listCities[i]
    loopCity.changePopulation(-2)
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_INFLUENZA_HIT_CITY", (loopCity.getNameKey(), ))
    CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), loopCity.getX(), loopCity.getY(), True, True)


def getHelpInfluenza2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_INFLUENZA_HELP_2", (2, ))

  return szHelp

######## SOLO FLIGHT ###########


def canTriggerSoloFlight(argsList):
  kTriggeredData = argsList[0]

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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SOLO_FLIGHT_HELP_1", (1, ))

  return szHelp

def applySoloFlight(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != kTriggeredData.ePlayer:
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(kTriggeredData.ePlayer).getTeam()):
        loopPlayer.AI_changeAttitudeExtra(kTriggeredData.ePlayer, 1)


######## ANTELOPE ###########

def canTriggerAntelope(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iDeer = GC.getInfoTypeForString("BONUS_DEER")
  iHappyBonuses = 0
  bDeer = False
  for i in xrange(GC.getNumBonusInfos()):
    bonus = GC.getBonusInfo(i)
    iNum = player.getNumAvailableBonuses(i)
    if iNum > 0 :
      if bonus.getHappiness() > 0:
        iHappyBonuses += 1
        if iHappyBonuses > 5:
          return False
      if i == iDeer:
        return False

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  if not plot.canHaveBonus(iDeer, False):
    return False

  return True

def doAntelope2(argsList):
# Need this because camps are not normally allowed unless there is already deer.
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  if not plot.isNone():
    plot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_HUNTING_CAMP"))

  return 1

def getHelpAntelope2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iCamp = GC.getInfoTypeForString("IMPROVEMENT_HUNTING_CAMP")
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_IMPROVEMENT_GROWTH", ( GC.getImprovementInfo(iCamp).getTextKey(), ))

  return szHelp

######## WHALEOFATHING ###########

def canTriggerWhaleOfAThing(argsList):

	kTriggeredData = argsList[0]
	player = GC.getPlayer(kTriggeredData.ePlayer)
	iWhale = GC.getInfoTypeForString("BONUS_WHALE")

	if player.getNumAvailableBonuses(iWhale) > 0:
		return False
	elif player.countOwnedBonuses(iWhale) > 1:
		return False
	iNumCoastalCities = player.countNumCoastalCities()
	if iNumCoastalCities < 1:
		return False
	if 0.75 > ( iNumCoastalCities / player.getNumCities()):
		return False
	return True

######## HIYOSILVER ###########

def canTriggerHiyoSilver(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iSilver = GC.getInfoTypeForString("BONUS_SILVER_ORE")
  iHappyBonuses = 0
  bSilver = False
  for i in xrange(GC.getNumBonusInfos()):
    bonus = GC.getBonusInfo(i)
    iNum = player.getNumAvailableBonuses(i)
    if iNum > 0 :
      if bonus.getHappiness() > 0:
        iHappyBonuses += 1
        if iHappyBonuses > 5:
          return False
      if i == iSilver:
        return False

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  if not plot.canHaveBonus(iSilver, False):
    return False

  return True

######## WININGMONKS ###########

def canTriggerWiningMonks(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  if player.getNumAvailableBonuses(GC.getInfoTypeForString("BONUS_GRAPES")) > 0:
    return False

  return True


def doWiningMonks2(argsList):
# Need this because wineries are not normally allowed unless there is already wine.
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  if not plot.isNone():
    plot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_WINERY"))

  return 1

def getHelpWiningMonks2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iImp = GC.getInfoTypeForString("IMPROVEMENT_WINERY")
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_IMPROVEMENT_GROWTH", ( GC.getImprovementInfo(iImp).getTextKey(), ))

  return szHelp


######## INDEPENDENTFILMS ###########

def canTriggerIndependentFilms(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  for i in xrange(GC.getNumBuildingInfos()):
    if GC.getBuildingInfo(i).getFreeBonus() == GC.getInfoTypeForString("BONUS_HIT_MOVIES"):
      if player.countNumBuildings(i) > 0:
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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  city = player.getCity(kTriggeredData.iCityId)

  iBonus = GC.getInfoTypeForString("BONUS_HIT_MOVIES")

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_INDEPENDENTFILMS_HELP_1", ( 1, GC.getBonusInfo(iBonus).getChar(), city.getNameKey()))

  return szHelp

######## ANCIENT OLYMPICS ###########

def canTriggerAncientOlympics(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  stateReligion = player.getStateReligion()

  if stateReligion == GC.getInfoTypeForString("RELIGION_JUDAISM"):
    return False

  if stateReligion == GC.getInfoTypeForString("RELIGION_CHRISTIANITY"):
    return False

  if stateReligion == GC.getInfoTypeForString("RELIGION_ISLAM"):
    return False

  return True

def doAncientOlympics2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  map = GC.getMap()

  for j in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(j)
    if j != kTriggeredData.ePlayer and loopPlayer.isAlive() and not loopPlayer.isMinorCiv():

      for i in xrange(map.numPlots()):
        plot = map.plotByIndex(i)
        if not plot.isWater() and plot.getOwner() == kTriggeredData.ePlayer and plot.isAdjacentPlayer(j, True):
          loopPlayer.AI_changeMemoryCount(kTriggeredData.ePlayer, MemoryTypes.MEMORY_EVENT_GOOD_TO_US, 1)
          break

  return 1

def getHelpAncientOlympics2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_ANCIENTOLYMPICS_HELP_2", ( 1, ))

  return szHelp


######## MODERN OLYMPICS ###########

def canTriggerModernOlympics(argsList):

  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))

  if (kOrigTriggeredData == None):
    return False

  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.iCityId = kOrigTriggeredData.iCityId
  kActualTriggeredDataObject.iPlotX = kOrigTriggeredData.iPlotX
  kActualTriggeredDataObject.iPlotY = kOrigTriggeredData.iPlotY

  return True

def getHelpModernOlympics(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SOLO_FLIGHT_HELP_1", (1, ))

  return szHelp

def applyModernOlympics(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != kTriggeredData.ePlayer:
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(kTriggeredData.ePlayer).getTeam()):
        loopPlayer.AI_changeAttitudeExtra(kTriggeredData.ePlayer, 1)


######## INTERSTATE ###########

def canTriggerInterstate(argsList):

  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  if not player.isCivic(GC.getInfoTypeForString("CIVIC_EGALITARIAN")):
    return False

  return True

def getHelpInterstate(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_UNIT_MOVEMENT", (1, GC.getRouteInfo(GC.getInfoTypeForString("ROUTE_HIGHWAY")).getTextKey()))

  return szHelp

def applyInterstate(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  team = GC.getTeam(player.getTeam())

  iRoad = GC.getInfoTypeForString("ROUTE_HIGHWAY")

  team.changeRouteChange(iRoad, -5)

######## EARTH DAY ###########

def getHelpEarthDay2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_EARTHDAY_HELP_2", ())

  return szHelp

def canApplyEarthDay2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)

  iCivic = GC.getInfoTypeForString("CIVIC_GREEN")

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != kTriggeredData.ePlayer and not loopPlayer.isHuman():
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(kTriggeredData.ePlayer).getTeam()):
        tradeData = TradeData()
        tradeData.ItemType = TradeableItems.TRADE_CIVIC
        tradeData.iData = iCivic
        if loopPlayer.canTradeItem(kTriggeredData.ePlayer, tradeData, False):
          if (loopPlayer.getTradeDenial(kTriggeredData.ePlayer, tradeData) == DenialTypes.NO_DENIAL):
            return True
  return False


def applyEarthDay2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iCivic = GC.getInfoTypeForString("CIVIC_GREEN")
  iCivicOption = GC.getInfoTypeForString("CIVICOPTION_ECONOMY")

  listPlayers = []
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != kTriggeredData.ePlayer and not loopPlayer.isHuman():
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(kTriggeredData.ePlayer).getTeam()):
        tradeData = TradeData()
        tradeData.ItemType = TradeableItems.TRADE_CIVIC
        tradeData.iData = iCivic
        if loopPlayer.canTradeItem(kTriggeredData.ePlayer, tradeData, False):
          if (loopPlayer.getTradeDenial(kTriggeredData.ePlayer, tradeData) == DenialTypes.NO_DENIAL):
            listPlayers.append((-loopPlayer.AI_civicValue(iCivic), iPlayer))

  listPlayers.sort()

  if len(listPlayers) > 3:
    listPlayers = listPlayers[:2]

  for (iValue, iPlayer) in listPlayers:
    GC.getPlayer(iPlayer).setCivics(iCivicOption, iCivic)

######## FREEDOM CONCERT ###########

def getHelpFreedomConcert2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_FREEDOMCONCERT_HELP_2", ())

  return szHelp

def canApplyFreedomConcert2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

  for iReligion in xrange(GC.getNumReligionInfos()):
    if eventCity.isHasReligion(iReligion):
      (loopCity, iter) = player.firstCity(False)
      while(loopCity):
        if not loopCity.isHasReligion(iReligion):
          for jReligion in xrange(GC.getNumReligionInfos()):
            if loopCity.isHasReligion(jReligion):
              return True
        (loopCity, iter) = player.nextCity(iter, False)

  return False

def applyFreedomConcert2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

  for iReligion in xrange(GC.getNumReligionInfos()):
    if eventCity.isHasReligion(iReligion):

      bestCity = None
      iBestDistance = 0
      (loopCity, iter) = player.firstCity(False)
      while(loopCity):
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

        (loopCity, iter) = player.nextCity(iter, False)


      if bestCity != None:
        bestCity.setHasReligion(iReligion, True, True, True)

######## HEROIC_GESTURE ###########

def canTriggerHeroicGesture(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  destPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)

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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  destPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  if destPlayer.isHuman():
    # this works only because it's a single-player only event
    popupInfo = CyPopupInfo()
    popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
    popupInfo.setText(TRNSLTR.getText("TXT_KEY_EVENT_HEROIC_GESTURE_OTHER_3", (player.getCivilizationAdjectiveKey(), )))
    popupInfo.setData1(kTriggeredData.eOtherPlayer)
    popupInfo.setData2(kTriggeredData.ePlayer)
    popupInfo.setPythonModule("CvRandomEventInterface")
    popupInfo.setOnClickedPythonCallback("heroicGesture2Callback")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_YES", ()), "")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_NO", ()), "")
    popupInfo.addPopup(kTriggeredData.eOtherPlayer)
  else:
    destPlayer.forcePeace(kTriggeredData.ePlayer)
    destPlayer.AI_changeAttitudeExtra(kTriggeredData.ePlayer, 1)
    player.AI_changeAttitudeExtra(kTriggeredData.eOtherPlayer, 1)

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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  destPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)

  # Get help text
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_ATTITUDE_GOOD", (1, destPlayer.getNameKey()));

  return szHelp

######## GREAT_MEDIATOR ###########

def canTriggerGreatMediator(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  destPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)

  if not GC.getTeam(player.getTeam()).canChangeWarPeace(destPlayer.getTeam()):
    return False

  if GC.getTeam(player.getTeam()).AI_getAtWarCounter(destPlayer.getTeam()) < 10:
    return False

  # do not use this event in multiplayer
  if GAME.isNetworkMultiPlayer():
    return False

  return True

def doGreatMediator2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  destPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  if destPlayer.isHuman():
    # this works only because it's a single-player only event
    popupInfo = CyPopupInfo()
    popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
    popupInfo.setText(TRNSLTR.getText("TXT_KEY_EVENT_GREAT_MEDIATOR_OTHER_3", (player.getCivilizationAdjectiveKey(), )))
    popupInfo.setData1(kTriggeredData.eOtherPlayer)
    popupInfo.setData2(kTriggeredData.ePlayer)
    popupInfo.setPythonModule("CvRandomEventInterface")
    popupInfo.setOnClickedPythonCallback("greatMediator2Callback")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_YES", ()), "")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_NO", ()), "")
    popupInfo.addPopup(kTriggeredData.eOtherPlayer)
  else:
    GC.getTeam(player.getTeam()).makePeace(destPlayer.getTeam())
    destPlayer.AI_changeAttitudeExtra(kTriggeredData.ePlayer, 1)
    player.AI_changeAttitudeExtra(kTriggeredData.eOtherPlayer, 1)

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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  destPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)

  # Get help text
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_ATTITUDE_GOOD", (1, destPlayer.getNameKey()))

  return szHelp

######## ANCIENT_TEXTS ###########

def doAncientTexts2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != kTriggeredData.ePlayer:
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(kTriggeredData.ePlayer).getTeam()):
        loopPlayer.AI_changeAttitudeExtra(kTriggeredData.ePlayer, 1)

  return

def getHelpAncientTexts2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SOLO_FLIGHT_HELP_1", (1, ))

  return szHelp


######## IMPACT_CRATER ###########

#def canTriggerImpactCrater(argsList):
#
#  kTriggeredData = argsList[0]
#  player = GC.getPlayer(kTriggeredData.ePlayer)
#
#  iUranium = GC.getInfoTypeForString("BONUS_URANIUM")
#  if player.getNumAvailableBonuses(iUranium) > 0:
#    return False
#
#  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
#  if not plot.canHaveBonus(iUranium, False):
#    return False
#
#  return True

def doImpactCrater2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  if not plot.isNone():
    plot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_SHAFT_MINE"))

  return 1

def getHelpImpactCrater2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iMine = GC.getInfoTypeForString("IMPROVEMENT_SHAFT_MINE")
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_IMPROVEMENT_GROWTH", ( GC.getImprovementInfo(iMine).getTextKey(), ))

  return szHelp


######## THE_HUNS ###########

def canTriggerTheHuns(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False


def getHelpTheHuns1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_THE_HUNS_HELP_1", ())

  return szHelp


def applyTheHuns1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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
    iNumUnits  = 15

  iUnitType = GC.getInfoTypeForString("UNIT_HORSE_ARCHER")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## THE_VANDALS ###########

def canTriggerTheVandals(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False


def getHelpTheVandals1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_THE_VANDALS_HELP_1", ())

  return szHelp


def applyTheVandals1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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
    iNumUnits  = 15

  iUnitType = GC.getInfoTypeForString("UNIT_HEAVY_SWORDSMAN")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## THE_GOTHS ###########

def canTriggerTheGoths(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False


def getHelpThGoths1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_THE_GOTHS_HELP_1", ())

  return szHelp


def applyTheGoths1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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
    iNumUnits  = 15

  iUnitType = GC.getInfoTypeForString("UNIT_AXEMAN")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## THE_PHILISTINES ###########

def canTriggerThePhilistines(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False


def getHelpThePhilistines1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_THE_PHILISTINES_HELP_1", ())

  return szHelp


def applyThePhilistines1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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
    iNumUnits  = 15

  iUnitType = GC.getInfoTypeForString("UNIT_SPEARMAN")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## THE_VEDIC_ARYANS ###########

def canTriggerTheVedicAryans(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False


def getHelpTheVedicAryans1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_THE_VEDIC_ARYANS_HELP_1", ())

  return szHelp


def applyTheVedicAryans1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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
    iNumUnits  = 15

  iUnitType = GC.getInfoTypeForString("UNIT_ARCHER")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)

######## SECURITY_TAX ###########

def canTriggerSecurityTax(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iWalls = GC.getInfoTypeForString("BUILDING_WALLS")
  if player.getNumCities() > player.getBuildingCountWithUpgrades(iWalls):
    return False

  return True


######## LITERACY ###########

def canTriggerLiteracy(argsList):

  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iLibrary = GC.getInfoTypeForString("BUILDING_LIBRARY")
  if player.getNumCities() > player.getBuildingCountWithUpgrades(iLibrary):
    return False

  return True

######## TEA ###########

def canTriggerTea(argsList):

	kTriggeredData = argsList[0]
	player = GC.getPlayer(kTriggeredData.ePlayer)

	if player.isCivic(GC.getInfoTypeForString("CIVIC_MERCANTILISM")):
		return False

	for iLoopPlayer in xrange(GC.getMAX_PC_PLAYERS()):
		if player.canHaveTradeRoutesWith(iLoopPlayer):
			return True
	return False

######## HORSE WHISPERING ###########

def canTriggerHorseWhispering(argsList):
  kTriggeredData = argsList[0]

  if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE) and GC.getPlayer(kTriggeredData.ePlayer).isHuman():
    return False

  return True

def getHelpHorseWhispering1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  map = GC.getMap()

  iNumStables = GC.getWorldInfo(map.getWorldSize()).getDefaultPlayers()
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_HORSE_WHISPERING_HELP", (iNumStables, ))

  return szHelp

def canTriggerHorseWhisperingDone(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iStable = GC.getInfoTypeForString("BUILDING_STABLE")
  if GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() > player.getBuildingCountWithUpgrades(iStable):
    return False

  return True

def getHelpHorseWhisperingDone1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_HORSE_WHISPERING_DONE_HELP_1", (GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(), ))

def applyHorseWhisperingDone1(argsList):
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

	MAP = GC.getMap()
	plot = MAP.plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	x = plot.getX()
	y = plot.getY()
	iUnit = GC.getInfoTypeForString("UNIT_HORSE_ARCHER")
	iCount = GC.getWorldInfo(MAP.getWorldSize()).getDefaultPlayers()
	while iCount > 0:
		CyPlayer.initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)
		iCount -= 1

######## HARBORMASTER ###########

def getHelpHarbormaster1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iHarborsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
  iCaravelsRequired = iHarborsRequired / 2 + 1

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_HARBORMASTER_HELP", (iHarborsRequired, iCaravelsRequired))

  return szHelp


def canTriggerHarbormaster(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE) and GC.getPlayer(kTriggeredData.ePlayer).isHuman():
    return False

  map = GC.getMap()

  iNumWater = 0

  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)

    if plot.isWater():
      iNumWater += 1

    if 100 * iNumWater >= 40 * map.numPlots():
      return True

  return False

def canTriggerHarbormasterDone(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

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
  kTriggeredData = argsList[0]

  if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE) and GC.getPlayer(kTriggeredData.ePlayer).isHuman():
    return False

  return True

def getHelpClassicLiterature1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iLibrariesRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CLASSIC_LITERATURE_HELP_1", (iLibrariesRequired, ))

  return szHelp


def canTriggerClassicLiteratureDone(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iLibrary = GC.getInfoTypeForString("BUILDING_LIBRARY")
  iBuildingsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
  if iBuildingsRequired > player.getBuildingCountWithUpgrades(iLibrary):
    return False

  return True

def getHelpClassicLiteratureDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CLASSIC_LITERATURE_DONE_HELP_2", ( ))

  return szHelp

def canApplyClassicLiteratureDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iEraAncient = GC.getInfoTypeForString("C2C_ERA_ANCIENT")

  for iTech in xrange(GC.getNumTechInfos()):
    if GC.getTechInfo(iTech).getEra() == iEraAncient and player.canResearch(iTech):
      return True

  return False

def applyClassicLiteratureDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iEraAncient = GC.getInfoTypeForString("C2C_ERA_ANCIENT")

  listTechs = []
  for iTech in xrange(GC.getNumTechInfos()):
    if GC.getTechInfo(iTech).getEra() == iEraAncient and player.canResearch(iTech):
      listTechs.append(iTech)

  if len(listTechs) > 0:
    iTech = listTechs[GAME.getSorenRandNum(len(listTechs), "Classic Literature Event Tech selection")]
    GC.getTeam(player.getTeam()).setHasTech(iTech, True, kTriggeredData.ePlayer, True, True)

def getHelpClassicLiteratureDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iSpecialist = GC.getInfoTypeForString("SPECIALIST_SCIENTIST")
  iGreatLibrary = GC.getInfoTypeForString("BUILDING_GREAT_LIBRARY")

  szCityName = u""
  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    if (loopCity.isHasBuilding(iGreatLibrary)):
      szCityName = loopCity.getNameKey()
      break

    (loopCity, iter) = player.nextCity(iter, False)

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_FREE_SPECIALIST", (1, GC.getSpecialistInfo(iSpecialist).getTextKey(), szCityName))

  return szHelp

def canApplyClassicLiteratureDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iGreatLibrary = GC.getInfoTypeForString("BUILDING_GREAT_LIBRARY")

  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    if (loopCity.isHasBuilding(iGreatLibrary)):
      return True

    (loopCity, iter) = player.nextCity(iter, False)

  return False

def applyClassicLiteratureDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iSpecialist = GC.getInfoTypeForString("SPECIALIST_SCIENTIST")
  iGreatLibrary = GC.getInfoTypeForString("BUILDING_GREAT_LIBRARY")

  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    if (loopCity.isHasBuilding(iGreatLibrary)):
      loopCity.changeFreeSpecialistCount(iSpecialist, 1)
      return

    (loopCity, iter) = player.nextCity(iter, False)

######## MASTER BLACKSMITH ###########

def canTriggerMasterBlacksmith(argsList):

	if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE) and GC.getPlayer(argsList[0].ePlayer).isHuman():
		return False
	return True


def getHelpMasterBlacksmith1(argsList):
	iRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

	return TRNSLTR.getText("TXT_KEY_EVENT_MASTER_BLACKSMITH_HELP_1", (iRequired, GC.getPlayer(argsList[1].ePlayer).getCity(argsList[1].iCityId).getNameKey()))


def expireMasterBlacksmith1(argsList):
	iPlayer = argsList[1].ePlayer
	CyCity = GC.getPlayer(iPlayer).getCity(argsList[1].iCityId)
	if not CyCity or CyCity.getOwner() != iPlayer:
		return True
	return False


def canTriggerMasterBlacksmithDone(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iForge = GC.getInfoTypeForString("BUILDING_FORGE");
  iBuildingsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
  if iBuildingsRequired > player.getBuildingCountWithUpgrades(iForge):
    return False

  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))

  city = player.getCity(kOrigTriggeredData.iCityId)
  if city == None or city.getOwner() != kTriggeredData.ePlayer:
    return False

  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.iCityId = kOrigTriggeredData.iCityId

  return True

def canApplyMasterBlacksmithDone1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iBonus = GC.getInfoTypeForString("BONUS_COPPER_ORE")
  city = player.getCity(kTriggeredData.iCityId)

  if city == None:
    return False

  map = GC.getMap()
  iBestValue = map.getGridWidth() + map.getGridHeight()
  bestPlot = None
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == kTriggeredData.ePlayer and plot.canHaveBonus(iBonus, False)):
      iValue = plotDistance(city.getX(), city.getY(), plot.getX(), plot.getY())
      if iValue < iBestValue:
        iBestValue = iValue
        bestPlot = plot

  if bestPlot == None:
    return False

  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.iPlotX = bestPlot.getX()
  kActualTriggeredDataObject.iPlotY = bestPlot.getY()

  return True

def applyMasterBlacksmithDone1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  city = player.getCity(kTriggeredData.iCityId)

  iBonus = GC.getInfoTypeForString("BONUS_COPPER_ORE")
  plot.setBonusType(iBonus)

  szBuffer = TRNSLTR.getText("TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE", (GC.getBonusInfo(iBonus).getTextKey(), city.getNameKey()))
  CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DISCOVERBONUS", InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT, GC.getBonusInfo(iBonus).getButton(), GC.getInfoTypeForString("COLOR_WHITE"), plot.getX(), plot.getY(), True, True)

def canApplyMasterBlacksmithDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  if player.getStateReligion() == -1:
    return False

  return True

######## THE BEST DEFENSE ###########

def canTriggerBestDefense(argsList):
  kTriggeredData = argsList[0]

  if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE) and GC.getPlayer(kTriggeredData.ePlayer).isHuman():
    return False

  return True

def getHelpBestDefense1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_BEST_DEFENSE_HELP_1", (iRequired, ))

  return szHelp

def canTriggerBestDefenseDone(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iCastle = GC.getInfoTypeForString("BUILDING_CASTLE")
  iBuildingsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
  if iBuildingsRequired > player.getBuildingCountWithUpgrades(iCastle):
    return False

  return True

def getHelpBestDefenseDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_BEST_DEFENSE_DONE_HELP_2", (3, ))

  return szHelp

def applyBestDefenseDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != kTriggeredData.ePlayer:
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(kTriggeredData.ePlayer).getTeam()):
        loopPlayer.AI_changeAttitudeExtra(kTriggeredData.ePlayer, 3)


def canApplyBestDefenseDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iGreatWall = GC.getInfoTypeForString("BUILDING_GREAT_WALL")

  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    if (loopCity.isHasBuilding(iGreatWall)):
      return True

    (loopCity, iter) = player.nextCity(iter, False)

  return False

######## NATIONAL SPORTS LEAGUE ###########

def canTriggerSportsLeague(argsList):
  kTriggeredData = argsList[0]

  if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE) and GC.getPlayer(kTriggeredData.ePlayer).isHuman():
    return False

  return True
def getHelpSportsLeague1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
  iBuilding = GC.getInfoTypeForString("BUILDING_CIRCUS_MAXIMUS")

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SPORTS_LEAGUE_HELP_1", (iRequired, GC.getBuildingInfo(iBuilding).getTextKey()))

  return szHelp

def canTriggerSportsLeagueDone(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iColosseum = GC.getInfoTypeForString("BUILDING_COLOSSEUM")
  iBuildingsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
  if iBuildingsRequired > player.getBuildingCountWithUpgrades(iColosseum):
    return False

  return True

def canApplySportsLeagueDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iZeus = GC.getInfoTypeForString("BUILDING_CIRCUS_MAXIMUS")

  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    if (loopCity.isHasBuilding(iZeus)):
      return True

    (loopCity, iter) = player.nextCity(iter, False)

  return False

######## CRUSADE ###########

def canTriggerCrusade(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)
  otherPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)

  if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE) and GC.getPlayer(kTriggeredData.ePlayer).isHuman():
    return False

  if holyCity.getOwner() != kTriggeredData.eOtherPlayer:
    return False

  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.iOtherPlayerCityId = holyCity.getID()

  return True

def getHelpCrusade1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CRUSADE_HELP_1", (holyCity.getNameKey(), ))

  return szHelp

def expireCrusade1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  otherPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)

  if holyCity.getOwner() == kTriggeredData.ePlayer:
    return False

  if player.getStateReligion() != kTriggeredData.eReligion:
    return True

  if holyCity.getOwner() != kTriggeredData.eOtherPlayer:
    return True

  if not GC.getTeam(player.getTeam()).isAtWar(otherPlayer.getTeam()):
    return True

  return False

def canTriggerCrusadeDone(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)

  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))
  holyCity = GAME.getHolyCity(kOrigTriggeredData.eReligion)

  if holyCity.getOwner() != kTriggeredData.ePlayer:
    return False

  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.iCityId = holyCity.getID()
  kActualTriggeredDataObject.eOtherPlayer = kOrigTriggeredData.eOtherPlayer
  kActualTriggeredDataObject.eReligion = kOrigTriggeredData.eReligion

  for iBuilding in xrange(GC.getNumBuildingInfos()):
    if GC.getBuildingInfo(iBuilding).getHolyCity() == kOrigTriggeredData.eReligion:
      kActualTriggeredDataObject.eBuilding = BuildingTypes(iBuilding)
      break

  return True

def getHelpCrusadeDone1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)
  szUnit = GC.getUnitInfo(holyCity.getConscriptUnit()).getTextKey()
  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() / 2 + 1
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CRUSADE_DONE_HELP_1", (iNumUnits, szUnit, holyCity.getNameKey()))

  return szHelp

def canApplyCrusadeDone1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)
  if -1 == holyCity.getConscriptUnit():
    return False

  return True

def applyCrusadeDone1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)
  iUnitType = holyCity.getConscriptUnit()
  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() / 2 + 1

  if iUnitType != -1:
    for i in xrange(iNumUnits):
      player.initUnit(iUnitType, holyCity.getX(), holyCity.getY(), UnitAITypes.UNITAI_CITY_DEFENSE, DirectionTypes.DIRECTION_SOUTH)

def getHelpCrusadeDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CRUSADE_DONE_HELP_2", (GC.getBuildingInfo(kTriggeredData.eBuilding).getTextKey(), holyCity.getNameKey()))

  return szHelp

def canApplyCrusadeDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)

  if -1 == kTriggeredData.eBuilding or holyCity.isHasBuilding(kTriggeredData.eBuilding):
    return False

  return True

def applyCrusadeDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)
  holyCity.setNumRealBuilding(kTriggeredData.eBuilding, 1)

  if (not GAME.isNetworkMultiPlayer() and kTriggeredData.ePlayer == GAME.getActivePlayer()):
    popupInfo = CyPopupInfo()
    popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
    popupInfo.setData1(kTriggeredData.eBuilding)
    popupInfo.setData2(holyCity.getID())
    popupInfo.setData3(0)
    popupInfo.setText(u"showWonderMovie")
    popupInfo.addPopup(kTriggeredData.ePlayer)

def getHelpCrusadeDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iNumCities = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CRUSADE_DONE_HELP_3", (GC.getReligionInfo(kTriggeredData.eReligion).getTextKey(), iNumCities))

  return szHelp

def canApplyCrusadeDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iNumCities = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

  if GAME.getNumCities() == GAME.countReligionLevels(kTriggeredData.eReligion):
    return False

  return True

def applyCrusadeDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  holyCity = GAME.getHolyCity(kTriggeredData.eReligion)

  listCities = []
  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive():
      (loopCity, iter) = loopPlayer.firstCity(False)

      while(loopCity):
        if (not loopCity.isHasReligion(kTriggeredData.eReligion)):
          iDistance = plotDistance(holyCity.getX(), holyCity.getY(), loopCity.getX(), loopCity.getY())
          listCities.append((iDistance, loopCity))

        (loopCity, iter) = loopPlayer.nextCity(iter, False)

  listCities.sort(key=itemgetter(0))

  iNumCities = min(GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(), len(listCities))

  for i in xrange(iNumCities):
    iDistance, loopCity = listCities[i]
    loopCity.setHasReligion(kTriggeredData.eReligion, True, True, True)

######## ESTEEMEED_PLAYWRIGHT ###########

def canTriggerEsteemedPlaywright(argsList):
	kTriggeredData = argsList[0]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

	SLAVERY = GC.getInfoTypeForString("BUILDING_SLAVERY")
	if SLAVERY > -1 and	CyPlayer.countNumBuildings(SLAVERY):
		return False
	return True


######## SECRET_KNOWLEDGE ###########

def getHelpSecretKnowledge2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_YIELD_CHANGE_BUILDING", (GC.getBuildingInfo(kTriggeredData.eBuilding).getTextKey(), u"+4[ICON_CULTURE]"))

  return szHelp

def applySecretKnowledge2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  city = player.getCity(kTriggeredData.iCityId)
  city.setBuildingCommerceChange(kTriggeredData.eBuilding, CommerceTypes.COMMERCE_CULTURE, 4)

######## HIGH_WARLORD ###########

def canTriggerHighWarlord(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  # If source civ is operating this Civic, disallow the event to trigger.
  if player.isCivic(GC.getInfoTypeForString("CIVIC_EGALITARIAN")):
    return False

  return True


######## EXPERIENCED_CAPTAIN ###########

def canTriggerExperiencedCaptain(argsList):
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  unit = player.getUnit(kTriggeredData.iUnitId)

  if unit.isNone():
    return False

  if unit.getExperience() < 7:
    return False

  return True

######## PARTISANS ###########

def getNumPartisanUnits(plot, iPlayer):
  for i in xrange(GC.getNumCultureLevelInfos()):
    iI = GC.getNumCultureLevelInfos() - i - 1
    if plot.getCulture(iPlayer) >= GC.getCultureLevelInfo(iI).getSpeedThreshold(GAME.getGameSpeedType()):
      return iI
  return 0

def getHelpPartisans1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  capital = player.getCapitalCity()
  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  if None != capital and not capital.isNone():
    iNumUnits = getNumPartisanUnits(plot, kTriggeredData.ePlayer)
    szUnit = GC.getUnitInfo(capital.getConscriptUnit()).getTextKey()

    szHelp = TRNSLTR.getText("TXT_KEY_EVENT_PARTISANS_HELP_1", (iNumUnits, szUnit))

  return szHelp

def canApplyPartisans1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  if getNumPartisanUnits(plot, kTriggeredData.ePlayer) <= 0:
    return False

  for i in xrange(3):
    for j in xrange(3):
      loopPlot = GC.getMap().plot(kTriggeredData.iPlotX + i - 1, kTriggeredData.iPlotY + j - 1)
      if None != loopPlot and not loopPlot.isNone():
        if not (loopPlot.isVisibleEnemyUnit(kTriggeredData.ePlayer) or loopPlot.isWater() or loopPlot.isImpassable() or loopPlot.isCity()):
          return True
  return False


def applyPartisans1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  capital = player.getCapitalCity()
  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  if None != capital and not capital.isNone():
    iNumUnits = getNumPartisanUnits(plot, kTriggeredData.ePlayer)

    listPlots = []
    for i in xrange(3):
      for j in xrange(3):
        loopPlot = GC.getMap().plot(kTriggeredData.iPlotX + i - 1, kTriggeredData.iPlotY + j - 1)
        if None != loopPlot and not loopPlot.isNone() and (i != 1 or j != 1):
          if not (loopPlot.isVisibleEnemyUnit(kTriggeredData.ePlayer) or loopPlot.isWater() or loopPlot.isImpassable()):
            listPlots.append(loopPlot)

    if len(listPlots) > 0:
      for i in xrange(iNumUnits):
        iPlot = GAME.getSorenRandNum(len(listPlots), "Partisan event placement")
        player.initUnit(capital.getConscriptUnit(), listPlots[iPlot].getX(), listPlots[iPlot].getY(), UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)

def getHelpPartisans2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  capital = player.getCapitalCity()
  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  if None != capital and not capital.isNone():
    iNumUnits = max(1, getNumPartisanUnits(plot, kTriggeredData.ePlayer) / 2)
    szUnit = GC.getUnitInfo(capital.getConscriptUnit()).getTextKey()

    szHelp = TRNSLTR.getText("TXT_KEY_EVENT_PARTISANS_HELP_2", (iNumUnits, szUnit, capital.getNameKey()))

  return szHelp

def canApplyPartisans2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  return (max(1, getNumPartisanUnits(plot, kTriggeredData.ePlayer) / 2) > 0)

def applyPartisans2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  capital = player.getCapitalCity()
  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  if None != capital and not capital.isNone():
    iNumUnits = max(1, getNumPartisanUnits(plot, kTriggeredData.ePlayer) / 2)
    for i in xrange(iNumUnits):
      player.initUnit(capital.getConscriptUnit(), capital.getX(), capital.getY(), UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)

######## GREED ###########

def canTriggerGreed(argsList):
	kTriggeredData = argsList[0]
	#trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyPlayerOther = GC.getPlayer(kTriggeredData.eOtherPlayer)
	iTeam = CyPlayer.getTeam()

	if not GC.getTeam(iTeam).canChangeWarPeace(CyPlayerOther.getTeam()):
		return False

	bonuses = []
	iCount = 0
	iOil = GC.getInfoTypeForString("BONUS_OIL")
	if not CyPlayer.getNumAvailableBonuses(iOil):
		bonuses.append(iOil)
		iCount += 1
	iIron = GC.getInfoTypeForString("BONUS_IRON_ORE")
	if not CyPlayer.getNumAvailableBonuses(iIron):
		bonuses.append(iIron)
		iCount += 1
	iHorse = GC.getInfoTypeForString("BONUS_HORSE")
	if not CyPlayer.getNumAvailableBonuses(iHorse):
		bonuses.append(iHorse)
		iCount += 1
	iCopper = GC.getInfoTypeForString("BONUS_COPPER_ORE")
	if not CyPlayer.getNumAvailableBonuses(iCopper):
		bonuses.append(iCopper)
		iCount += 1
	iSulphur = GC.getInfoTypeForString("BONUS_SULPHUR")
	if not CyPlayer.getNumAvailableBonuses(iSulphur):
		bonuses.append(iSulphur)
		iCount += 1
	iElephant = GC.getInfoTypeForString("BONUS_ELEPHANTS")
	if not CyPlayer.getNumAvailableBonuses(iElephant):
		bonuses.append(iElephant)
		iCount += 1

	if not iCount:
		return False

	shuffledRange = CvUtil.shuffle(iCount, GAME.getMapRand())
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
			if CyPlot.getOwner() == kTriggeredData.eOtherPlayer and CyPlot.getBonusType(iTeam) == iBonus:
				listPlots.append(CyPlot)
				iCount += 1
		if listPlots:
			break
	if not listPlots:
		return False

	CyPlot = listPlots[GAME.getSorenRandNum(iCount, "Random Plot")]

	if -1 == getGreedUnit(CyPlayer, CyPlot):
		return False

	kActualTriggeredDataObject = CyPlayer.getEventTriggered(kTriggeredData.iId)
	kActualTriggeredDataObject.iPlotX = CyPlot.getX()
	kActualTriggeredDataObject.iPlotY = CyPlot.getY()
	return True

def getHelpGreed1(argsList):
	#iEvent = argsList[0]
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyPlayerOther = GC.getPlayer(kTriggeredData.eOtherPlayer)
	iBonus = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY).getBonusType(CyPlayer.getTeam())

	iTurns = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getGrowthPercent()

	szHelp = TRNSLTR.getText("TXT_KEY_EVENT_GREED_HELP_1", (CyPlayerOther.getCivilizationShortDescriptionKey(), GC.getBonusInfo(iBonus).getTextKey(), iTurns))

	return szHelp

def expireGreed1(argsList):
	#iEvent = argsList[0]
	kTriggeredData = argsList[1]
	CyPlot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	iOwner = CyPlot.getOwner()

	if iOwner == -1 or iOwner == kTriggeredData.ePlayer:
		return False

	if GAME.getGameTurn() >= kTriggeredData.iTurn + GC.getGameSpeedInfo(GAME.getGameSpeedType()).getGrowthPercent():
		return True

	if iOwner != kTriggeredData.eOtherPlayer:
		return True

	return False

def canTriggerGreedDone(argsList):
	kTriggeredData = argsList[0]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
	kOrigTriggeredData = CyPlayer.getEventOccured(trigger.getPrereqEvent(0))
	CyPlot = GC.getMap().plot(kOrigTriggeredData.iPlotX, kOrigTriggeredData.iPlotY)

	if CyPlot.getOwner() != kOrigTriggeredData.ePlayer:
		return False

	if -1 == getGreedUnit(CyPlayer, CyPlot):
		return False

	kActualTriggeredDataObject = CyPlayer.getEventTriggered(kTriggeredData.iId)
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
		if CvUnitInfo.getMaxGlobalInstances() + 1 or CvUnitInfo.getMaxPlayerInstances() + 1:
			continue
		if iUnit != -1 and CvUnitInfo.getDomainType() == DomainTypes.DOMAIN_LAND and CyPlayer.canTrain(iUnit, False, False):
			iValue = 0
			if CvUnitInfo.getPrereqAndBonus() == iBonus:
				iValue = CyPlayer.AI_unitValue(iUnit, UnitAITypes.UNITAI_ATTACK, CyPlot.area())
			else:
				for j in xrange(GC.getNUM_UNIT_PREREQ_OR_BONUSES()):
					if CvUnitInfo.getPrereqOrBonuses(j) == iBonus:
						iValue = CyPlayer.AI_unitValue(iUnit, UnitAITypes.UNITAI_ATTACK, CyPlot.area())
						break
			if iValue > iBestValue:
				iBestValue = iValue
				iBestUnit = iUnit
	return iBestUnit


def getHelpGreedDone1(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyPlot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
	iUnitType = getGreedUnit(CyPlayer, CyPlot)

	if iUnitType != -1:
		szHelp = TRNSLTR.getText("TXT_KEY_EVENT_GREED_DONE_HELP_1", (iNumUnits, GC.getUnitInfo(iUnitType).getTextKey()))
	return szHelp

def applyGreedDone1(argsList):
	#iEvent = argsList[0]
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyPlot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

	iUnitType = getGreedUnit(CyPlayer, CyPlot)
	iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

	for i in xrange(iNumUnits):
		CyPlayer.initUnit(iUnitType, CyPlot.getX(), CyPlot.getY(), UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)


######## WAR CHARIOTS ###########

def canTriggerWarChariots(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.eReligion = ReligionTypes(player.getStateReligion())

  return True

def getHelpWarChariots1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_WAR_CHARIOTS_HELP_1", (iNumUnits, ))

  return szHelp

def canTriggerWarChariotsDone(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1

  if player.getUnitCount(GC.getInfoTypeForString("UNIT_CHARIOT")) < iNumUnits:
    return False

  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))
  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.eReligion = kOrigTriggeredData.eReligion

  return True

######## ELITE SWORDSMEN ###########

def getHelpEliteSwords1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 5
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_ELITE_SWORDS_HELP_1", (iNumUnits, ))

  return szHelp

def canTriggerEliteSwordsDone(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 5
  iUnit1 = GC.getInfoTypeForString("UNIT_LIGHT_SWORDSMAN")
  iUnit2 = GC.getInfoTypeForString("UNIT_SWORDSMAN")
  iUnit3 = GC.getInfoTypeForString("UNIT_HEAVY_SWORDSMAN")
  if player.getUnitCount(iUnit1) + player.getUnitCount(iUnit2) + player.getUnitCount(iUnit3) < iNumUnits:
    return False

  return True


def canApplyEliteSwordsDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iCivic = GC.getInfoTypeForString("CIVIC_MONARCHY")

  if not player.isCivic(iCivic):
    return False

  return True

######## WARSHIPS ###########

def canTriggerWarships(argsList):
  kTriggeredData = argsList[0]

  map = GC.getMap()
  iNumWater = 0

  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)

    if plot.isWater():
      iNumWater += 1

    if 100 * iNumWater >= 55 * map.numPlots():
      return True

  return False

def getHelpWarships1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

  iBuilding = GC.getInfoTypeForString("BUILDING_GREAT_LIGHTHOUSE")
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_WARSHIPS_HELP_1", (iNumUnits, GC.getBuildingInfo(iBuilding).getTextKey()))

  return szHelp

def canTriggerWarshipsDone(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

  if player.getUnitCount(GC.getInfoTypeForString("UNIT_TRIREME")) < iNumUnits:
    return False

  return True


def canApplyWarshipsDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iBuilding = GC.getInfoTypeForString("BUILDING_GREAT_LIGHTHOUSE")
  if player.getBuildingCountWithUpgrades(iBuilding) == 0:
    return False

  return True

######## GUNS NOT BUTTER ###########

def getHelpGunsButter1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1
  iBuilding = GC.getInfoTypeForString("BUILDING_TAJ_MAHAL")

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_GUNS_BUTTER_HELP_1", (iNumUnits, GC.getBuildingInfo(iBuilding).getTextKey()))

  return szHelp

def canTriggerGunsButterDone(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1

  if player.getUnitCount(GC.getInfoTypeForString("UNIT_MUSKETMAN")) < iNumUnits:
    return False

  return True


def canApplyGunsButterDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iCivic = GC.getInfoTypeForString("CIVIC_VASSALAGE")

  if not player.isCivic(iCivic):
    return False

  return True

def canApplyGunsButterDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iBuilding = GC.getInfoTypeForString("BUILDING_TAJ_MAHAL")
  if player.getBuildingCountWithUpgrades(iBuilding) == 0:
    return False

  return True

######## NOBLE KNIGHTS ###########

def canTriggerNobleKnights(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.eReligion = ReligionTypes(player.getStateReligion())

  return True

def getHelpNobleKnights1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1
  iBuilding = GC.getInfoTypeForString("BUILDING_ORACLE")

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_NOBLE_KNIGHTS_HELP_1", (iNumUnits, GC.getBuildingInfo(iBuilding).getTextKey()))

  return szHelp

def canTriggerNobleKnightsDone(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iNumUnits = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1

  if player.getUnitCount(GC.getInfoTypeForString("UNIT_KNIGHT")) < iNumUnits:
    return False

  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))
  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.eReligion = kOrigTriggeredData.eReligion

  iBuilding = GC.getInfoTypeForString("BUILDING_ORACLE")

  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    if (loopCity.isHasBuilding(iBuilding)):
      kActualTriggeredDataObject.iPlotX = loopCity.getX()
      kActualTriggeredDataObject.iPlotY = loopCity.getY()
      kActualTriggeredDataObject.iCityId = loopCity.getID()
      break

    (loopCity, iter) = player.nextCity(iter, False)

  return True

def canApplyNobleKnightsDone2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iCivic = GC.getInfoTypeForString("CIVIC_STATE_CHURCH")

  if not player.isCivic(iCivic):
    return False

  return True

######## OVERWHELM DOCTRINE ###########

def canTriggerOverwhelm(argsList):
  kTriggeredData = argsList[0]

  map = GC.getMap()
  iNumWater = 0

  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if plot.isWater():
      iNumWater += 1
    if 100 * iNumWater >= 55 * map.numPlots():
      return True
  return False

def getHelpOverwhelm1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iDestroyer = GC.getInfoTypeForString("UNIT_DESTROYER")
  iNumDestroyers = 4
  iBattleship = GC.getInfoTypeForString("UNIT_BATTLESHIP")
  iNumBattleships = 2
  iCarrier = GC.getInfoTypeForString("UNIT_CARRIER")
  iNumCarriers = 3
  iFighter = GC.getInfoTypeForString("SPECIALUNIT_FIGHTER")
  iNumFighters = 9
  iBuilding = GC.getInfoTypeForString("BUILDING_MANHATTAN")

# szHelp = TRNSLTR.getText("TXT_KEY_EVENT_OVERWHELM_HELP_1", (iNumDestroyers, GC.getUnitInfo(iDestroyer).getTextKey(), iNumBattleships, GC.getUnitInfo(iBattleship).getTextKey(), iNumCarriers, GC.getUnitInfo(iCarrier).getTextKey(), iNumFighters, GC.getSpecialUnitInfo(iFighter).getTextKey(), GC.getProjectInfo(iProject).getTextKey()))
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_OVERWHELM_HELP_1", (iNumDestroyers, GC.getUnitInfo(iDestroyer).getTextKey(), iNumBattleships, GC.getUnitInfo(iBattleship).getTextKey(), iNumCarriers, GC.getUnitInfo(iCarrier).getTextKey(), iNumFighters, GC.getSpecialUnitInfo(iFighter).getTextKey(), GC.getBuildingInfo(iBuilding).getTextKey()))

  return szHelp

def canTriggerOverwhelmDone(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

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
  (loopUnit, iter) = player.firstUnit(False)
  while (loopUnit):
    if loopUnit.getSpecialUnitType() == iFighter:
      iNumPlayerFighters += 1
    (loopUnit, iter) = player.nextUnit(iter, False)

  if iNumPlayerFighters < iNumFighters:
    return False

  return True

def getHelpOverwhelmDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_OVERWHELM_DONE_HELP_3", ())

  return szHelp

def canApplyOverwhelmDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  iBuilding = GC.getInfoTypeForString("BUILDING_MANHATTAN")

# if GC.getTeam(player.getTeam()).getProjectCount(iProject) == 0:
  if player.getBuildingCountWithUpgrades(iBuilding) == 0:
    return False

  return True

def applyOverwhelmDone3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  GAME.changeNoNukesCount(1)

######## CORPORATE EXPANSION ###########

def canTriggerCorporateExpansion(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  city = GAME.getHeadquarters(kTriggeredData.eCorporation)
  if None == city or city.isNone():
    return False

  # Hack to remember the number of cities you already have with the Corporation
  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.iOtherPlayerCityId = GAME.countCorporationLevels(kTriggeredData.eCorporation)
  kActualTriggeredDataObject.iCityId = city.getID()
  kActualTriggeredDataObject.iPlotX = city.getX()
  kActualTriggeredDataObject.iPlotY = city.getY()

  bFound = False
  for iBuilding in xrange(GC.getNumBuildingInfos()):
    if GC.getBuildingInfo(iBuilding).getFoundsCorporation() == kTriggeredData.eCorporation:
      kActualTriggeredDataObject.eBuilding = BuildingTypes(iBuilding)
      bFound = True
      break

  if not bFound:
    return False

  return True

def expireCorporateExpansion1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  city = player.getCity(kTriggeredData.iCityId)
  if None == city or city.isNone():
    return True

  return False

def getHelpCorporateExpansion1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iNumCities = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CORPORATE_EXPANSION_HELP_1", (GC.getCorporationInfo(kTriggeredData.eCorporation).getTextKey(), iNumCities))

  return szHelp

def canTriggerCorporateExpansionDone(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))

  iNumCitiesRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers() + 1 + kOrigTriggeredData.iOtherPlayerCityId

  if iNumCitiesRequired > GAME.countCorporationLevels(kOrigTriggeredData.eCorporation):
    return False


  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.eCorporation = kOrigTriggeredData.eCorporation
  kActualTriggeredDataObject.eBuilding = kOrigTriggeredData.eBuilding
  kActualTriggeredDataObject.iCityId = kOrigTriggeredData.iCityId
  kActualTriggeredDataObject.iPlotX = kOrigTriggeredData.iPlotX
  kActualTriggeredDataObject.iPlotY = kOrigTriggeredData.iPlotY

  return True

def getHelpCorporateExpansionDone1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_YIELD_CHANGE_BUILDING", (GC.getBuildingInfo(kTriggeredData.eBuilding).getTextKey(), u"+50[ICON_GOLD]"))

  return szHelp

def applyCorporateExpansionDone1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  city = player.getCity(kTriggeredData.iCityId)
  if None != city and not city.isNone():
    city.setBuildingCommerceChange(kTriggeredData.eBuilding, CommerceTypes.COMMERCE_GOLD, 50)

######## HOSTILE TAKEOVER ###########

def canTriggerHostileTakeover(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE) and GC.getPlayer(kTriggeredData.ePlayer).isHuman():
    return False

  city = GAME.getHeadquarters(kTriggeredData.eCorporation)
  if None == city or city.isNone():
    return False

  # Hack to remember the number of cities you already have with the Corporation
  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.iCityId = city.getID()
  kActualTriggeredDataObject.iPlotX = city.getX()
  kActualTriggeredDataObject.iPlotY = city.getY()

  bFound = False
  for iBuilding in xrange(GC.getNumBuildingInfos()):
    if GC.getBuildingInfo(iBuilding).getFoundsCorporation() == kTriggeredData.eCorporation:
      kActualTriggeredDataObject.eBuilding = BuildingTypes(iBuilding)
      bFound = True
      break

  if not bFound:
    return False

  listResources = getHostileTakeoverListResources(GC.getCorporationInfo(kTriggeredData.eCorporation), player)
  if len(listResources) == 0:
    return False

  return True

def expireHostileTakeover1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  city = player.getCity(kTriggeredData.iCityId)
  if None == city or city.isNone():
    return True

  return False

def getHostileTakeoverListResources(corporation, player):
  map = GC.getMap()
  listHave = []
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if plot.getOwner() == player.getID():
      iBonus = plot.getBonusType(player.getTeam())
      if iBonus != -1:
        if not iBonus in listHave:
          listHave.append(iBonus)
  listNeed = []
  for i in xrange(GC.getNUM_CORPORATION_PREREQ_BONUSES()):
    iBonus = corporation.getPrereqBonus(i)
    if iBonus != -1:
      if not iBonus in listHave:
        listNeed.append(iBonus)
  return listNeed

def getHelpHostileTakeover1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  listResources = getHostileTakeoverListResources(GC.getCorporationInfo(kTriggeredData.eCorporation), player)
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
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  kOrigTriggeredData = player.getEventOccured(trigger.getPrereqEvent(0))

  listResources = getHostileTakeoverListResources(GC.getCorporationInfo(kOrigTriggeredData.eCorporation), player)

  if len(listResources) > 0:
    return False

  kActualTriggeredDataObject = player.getEventTriggered(kTriggeredData.iId)
  kActualTriggeredDataObject.eCorporation = kOrigTriggeredData.eCorporation
  kActualTriggeredDataObject.eBuilding = kOrigTriggeredData.eBuilding
  kActualTriggeredDataObject.iCityId = kOrigTriggeredData.iCityId
  kActualTriggeredDataObject.iPlotX = kOrigTriggeredData.iPlotX
  kActualTriggeredDataObject.iPlotY = kOrigTriggeredData.iPlotY

  return True

def getHelpHostileTakeoverDone1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_YIELD_CHANGE_BUILDING", (GC.getBuildingInfo(kTriggeredData.eBuilding).getTextKey(), u"+100[ICON_GOLD]"))

  return szHelp

def applyHostileTakeoverDone1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  city = player.getCity(kTriggeredData.iCityId)
  if None != city and not city.isNone():
    city.setBuildingCommerceChange(kTriggeredData.eBuilding, CommerceTypes.COMMERCE_GOLD, 100)


######## Great Beast ########

def doGreatBeast3(argsList):
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  (loopCity, iter) = player.firstCity(False)

  while(loopCity):
    if loopCity.isHasReligion(kTriggeredData.eReligion):
      loopCity.changeHappinessTimer(40)
    (loopCity, iter) = player.nextCity(iter, False)

def getHelpGreatBeast3(argsList):
  kTriggeredData = argsList[1]
  religion = GC.getReligionInfo(kTriggeredData.eReligion)

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_GREAT_BEAST_3_HELP", (GC.getDefineINT("TEMP_HAPPY"), 40, religion.getChar()))

  return szHelp

####### Comet Fragment ########

def canDoCometFragment(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  if (player.getSpaceProductionModifier()) > 10:
    return False

  return True

####### Immigrants ########

def canTriggerImmigrantCity(argsList):
  ePlayer = argsList[1]
  iCity = argsList[2]

  player = GC.getPlayer(ePlayer)
  city = player.getCity(iCity)

  if city.isNone():
    return False

  if ((city.happyLevel() - city.unhappyLevel(0) < 1) or (city.goodHealth() - city.badHealth(True) < 1)):
    return False

  if (city.getCommerceRateTimes100(CommerceTypes.COMMERCE_CULTURE) < 5500):
    return False

####### Controversial Philosopher ######

def canTriggerControversialPhilosopherCity(argsList):
  ePlayer = argsList[1]
  iCity = argsList[2]

  player = GC.getPlayer(ePlayer)
  city = player.getCity(iCity)

  if city.isNone():
    return False
  if (not city.isCapital()):
    return False
  if (city.getCommerceRateTimes100(CommerceTypes.COMMERCE_RESEARCH) < 3500):
    return False

  return True

####### Spy Discovered #######


def canDoSpyDiscovered3(argsList):
	if GC.getPlayer(argsList[1].ePlayer).getCapitalCity().isNone():
		return False
	return True

def doSpyDiscovered3(argsList):
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

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
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  if player.getUnitCount(GC.getInfoTypeForString("UNIT_ICBM")) + player.getUnitCount(GC.getInfoTypeForString("UNIT_TACTICAL_NUKE")) < 10:
    return False

  return True

def doNuclearProtest1(argsList):
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iICBM = GC.getInfoTypeForString("UNIT_ICBM")
  iTacNuke = GC.getInfoTypeForString("UNIT_TACTICAL_NUKE")

  (loopUnit, iter) = player.firstUnit(False)
  while (loopUnit):
    if loopUnit.getUnitType() == iICBM or loopUnit.getUnitType() == iTacNuke:
      loopUnit.kill(False, -1)
    (loopUnit, iter) = player.nextUnit(iter, False)

def getHelpNuclearProtest1(argsList):
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_NUCLEAR_PROTEST_1_HELP", ())
  return szHelp


######## Preaching Researcher #######

def canTriggerPreachingResearcherCity(argsList):
  iCity = argsList[2]

  player = GC.getPlayer(argsList[1])
  city = player.getCity(iCity)

  if city.isHasBuilding(GC.getInfoTypeForString("BUILDING_UNIVERSITY")):
    return True
  return False

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
  city = player.getCity(iCity)

  if city.isCapital():
    return True
  return False

######## FUTBOL_WAR #######
#Need to stop vassals from being selected as "other player" b/c can't DoW them.  Also, b/c this event railroads other player into war, don't
#want that other player to be a Human in multiplayer game.  (Assuming this event wouldn't OOS in MP anyway.)
#Don't know how the AI will handle the event, but we'll see.  otherPlayer.isHuman() may be redundant as bSinglePlayer = 1 selected in XML.


def canTriggerFutbol_War(argslist):

  kTriggeredData = argslist[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  otherPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)

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

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)


  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_PACIFISM")):
   return False

  return True

def getHelpNapalm1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_NAPALM_HELP_1", ())

  return szHelp

def applyNapalm1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != kTriggeredData.ePlayer:
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(kTriggeredData.ePlayer).getTeam()):
        loopPlayer.AI_changeAttitudeExtra(kTriggeredData.ePlayer, -1)

def getHelpNapalm2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_NAPALM_HELP_2", ())

  return szHelp

######## BILLIONS_AND_BILLIONS ########
#Carl Sagan doesn't like theocracies -- he only likes free speech

def canTriggerBillionsandBillions(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_DIVINE_CULT")) or pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_INQUISITORIAL")):
    return False
  if ( pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_DEMOCRACY")) or pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_FEDERALISM"))) and pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_EGALITARIAN")):
    return True

  return False

def getHelpBillionsandBillions2 (argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_HELP_BILLIONS_AND_BILLIONS_2", ())

  return szHelp

def canApplyBillionsandBillions2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  iBuilding = GC.getInfoTypeForString("BUILDING_HOLLYWOOD")
  if player.getBuildingCountWithUpgrades(iBuilding) == 0:
    return False

  return True


################## FREE_ENTERPRISE (Free Market only) ###################

def canTriggerFreeEnterprise(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_FREE_MARKET")):
    return True

  return False

def getHelpFreeEnterprise2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_HELP_FREE_ENTERPRISE_2", ())

  return szHelp


#################  FIAT_MONEY ##########################
# Must be poor, can't be running Decentralization

def canTriggerFiatMoney(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  if pPlayer.getGold() > 5000:
    return False
  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_BARTER")):
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

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
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

  if city.isCapital():
    return True

  return False

def canTriggerRubiconUnit(argsList):
  eTrigger = argsList[0]
  ePlayer = argsList[1]
  iUnit = argsList[2]

  pPlayer = GC.getPlayer(ePlayer)
  unit = pPlayer.getUnit(iUnit)

  if unit.isNone():
    return False

  if unit.getExperience() < 25:
    return False

  iMorale = GC.getInfoTypeForString("PROMOTION_MORALE")
  if unit.isHasPromotion(iMorale):
    return False

  return True

def getHelpRubicon2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  unit = pPlayer.getUnit(kTriggeredData.iUnitId)

  iMorale = GC.getInfoTypeForString("PROMOTION_MORALE")

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CROSSING_THE_RUBICON_2_HELP", (unit.getNameKey(), GC.getPromotionInfo(iMorale).getTextKey()))

  return szHelp

def applyRubicon2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  pPlayer2 = GC.getPlayer(kTriggeredData.ePlayer)
  unit2 = pPlayer2.getUnit(kTriggeredData.iUnitId)

  iMorale = GC.getInfoTypeForString("PROMOTION_MORALE")

  unit2.setHasPromotion(iMorale, True)

def getHelpRubicon3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CROSSING_THE_RUBICON_3_HELP", (1, ))

  return szHelp

def applyRubicon3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer3 = GC.getPlayer(kTriggeredData.ePlayer)

  iCivicType = GC.getInfoTypeForString("CIVIC_MONARCHY")
  iCivicOptionType = GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")

  pPlayer3.setCivics(iCivicOptionType, iCivicType)
  pPlayer3.changeAnarchyTurns(2)


####### THE_GENERALS_PUTSCH (France only) #####
#Must be French, must be running Rep or Univ. Suffrage, must be at war more than 10 turns.
#Can't be in situation where Police State is impossible b/c gov't civic forced (e.g., by UN Resolution)

def canTriggerGeneralsPutsch(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  otherPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
  R = pPlayer.getRevolutionTimer()
  A = pPlayer.isAnarchy()
  PolState = GC.getInfoTypeForString("CIVIC_TOTALITARIANISM")
  Despot = GC.getInfoTypeForString("CIVIC_DESPOTISM")
  HerdRule = GC.getInfoTypeForString("CIVIC_MONARCHY")


  if R>0:
    return False
  if A>0:
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
  kTriggeredData = argsList[0]

  pPlayer = GC.getPlayer(argsList[1])
  pCity = pPlayer.getCity(iCity)


  if pCity.isGovernmentCenter():
    return True


  return False

def getHelpGeneralsPutsch1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_THE_GENERALS_PUTSCH_1_HELP", ())

  return szHelp

def canDoGeneralsPutsch1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  iEiffel = GC.getInfoTypeForString("BUILDING_EIFFEL_TOWER")
  iBroadcast = GC.getInfoTypeForString("BUILDING_BROADCAST_TOWER")
  EifCnt = pPlayer.getBuildingCountWithUpgrades(iEiffel)
  BrodCnt = pPlayer.getBuildingCountWithUpgrades(iBroadcast)
  bAggregate = EifCnt + BrodCnt

  if bAggregate < 1:
    return False

  return True

def getHelpGeneralsPutsch3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_THE_GENERALS_PUTSCH_3_HELP", ())

  return szHelp

def applyGeneralsPutsch3(argsList):
  GC.getPlayer(argsList[1].ePlayer).setCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"), GC.getInfoTypeForString("CIVIC_TOTALITARIANISM"))

######  CURE_FOR_CANCER ########

def canTriggerCureforCancer(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_DIVINE_CULT")):
    return False
  iOxford = GC.getInfoTypeForString("BUILDING_MEDICAL_DATABASE")
  if pPlayer.getBuildingCountWithUpgrades(iOxford) == 0:
    return False

  return True

def getHelpCureforCancer1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CURE_FOR_CANCER_HELP_1", (1, ))

  return szHelp

def applyCureforCancer1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
    loopPlayer = GC.getPlayer(iPlayer)
    if loopPlayer.isAlive() and iPlayer != kTriggeredData.ePlayer:
      loopTeam = GC.getTeam(loopPlayer.getTeam())
      if loopTeam.isHasMet(GC.getPlayer(kTriggeredData.ePlayer).getTeam()):
        loopPlayer.AI_changeAttitudeExtra(kTriggeredData.ePlayer, 1)



####### CARNATION_REVOLUTION ########


def canTriggerCarnation(argsList):
  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  otherPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
  R = pPlayer.getRevolutionTimer()
  A = pPlayer.isAnarchy()
  Rep = GC.getInfoTypeForString("CIVIC_REPUBLIC")
  StatProp = GC.getInfoTypeForString("CIVIC_PLANNED")


  if R>0:
    return False
  if A>0:
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
  eTrigger = argsList[0]
  ePlayer = argsList[1]
  iUnit = argsList[2]

  player = GC.getPlayer(ePlayer)
  unit = player.getUnit(iUnit)

  if unit.isNone():
    return False

  if unit.getExperience() < 50:
    return False

  # do not use this event in multiplayer
  if GAME.isNetworkMultiPlayer():
    return False

  return True



def doCarnation2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  destPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  iCivicType1 = GC.getInfoTypeForString("CIVIC_REPUBLIC")
  iCivicOptionType1 = GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT")
  iCivicType3 = GC.getInfoTypeForString("CIVIC_PLANNED")
  iCivicOptionType3 = GC.getInfoTypeForString("CIVICOPTION_ECONOMY")


  if destPlayer.isHuman():
    # this works only because it's a single-player only event
    popupInfo = CyPopupInfo()
    popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
    popupInfo.setText(TRNSLTR.getText("TXT_KEY_EVENT_CARNATION_REVOLUTION_OTHER_2", (pPlayer.getCivilizationAdjectiveKey(), )))
    popupInfo.setData1(kTriggeredData.eOtherPlayer)
    popupInfo.setData2(kTriggeredData.ePlayer)
    popupInfo.setPythonModule("CvRandomEventInterface")
    popupInfo.setOnClickedPythonCallback("Carnation2Callback")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_YES", ()), "")
    popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_POPUP_NO", ()), "")
    popupInfo.addPopup(kTriggeredData.eOtherPlayer)
  else:
    destPlayer.forcePeace(kTriggeredData.ePlayer)
    pPlayer.setCivics(iCivicOptionType1, iCivicType1)
    pPlayer.setCivics(iCivicOptionType3, iCivicType3)
    pPlayer.changeAnarchyTurns(2)

  return

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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  destPlayer = GC.getPlayer(kTriggeredData.eOtherPlayer)

  # Get help text
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_CARNATION_REVOLUTION_HELP_2", (1, destPlayer.getNameKey()));

  return szHelp


#####  SYNTHETIC_FUELS  #####

def canTriggerSyntheticFuels(argsList):
  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  iEthanol = GC.getInfoTypeForString("BUILDING_CORPORATION_3_HQ")
  if pPlayer.getBuildingCountWithUpgrades(iEthanol) > 0:
    return False
  bOil = GC.getInfoTypeForString("BONUS_OIL")
  if pPlayer.hasBonus(bOil) > 0:
    return False
  bCoal = GC.getInfoTypeForString("BONUS_COAL")
  if pPlayer.hasBonus(bCoal) < 1:
    return False
  for i in xrange(GC.getNumBuildingInfos()):
    if GC.getBuildingInfo(i).getFreeBonus() == bOil:
      if pPlayer.countNumBuildings(i) > 0:
        return False
  return True

def canTriggerCitySyntheticFuels(argsList):
  iCity = argsList[2]

  pPlayer = GC.getPlayer(argsList[1])
  pCity = pPlayer.getCity(iCity)

  if pCity.isGovernmentCenter():
    return False

  return True


def getHelpSyntheticFuels1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  pCity = pPlayer.getCity(kTriggeredData.iCityId)

  oBonus = GC.getInfoTypeForString("BONUS_OIL")

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SYNTHETIC_FUELS_HELP_1", ( 1, GC.getBonusInfo(oBonus).getChar(), pCity.getNameKey()))

  return szHelp

def getHelpSyntheticFuels2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  pCity = pPlayer.getCity(kTriggeredData.iCityId)

  oBonus = GC.getInfoTypeForString("BONUS_OIL")

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SYNTHETIC_FUELS_HELP_2", ( 1, GC.getBonusInfo(oBonus).getChar(), pCity.getNameKey()))

  return szHelp

def getHelpSyntheticFuels3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SYNTHETIC_FUELS_HELP_3", (1, ))

  return szHelp

def getHelpSyntheticFuels4(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  pCity = pPlayer.getCity(kTriggeredData.iCityId)

  oBonus = GC.getInfoTypeForString("BONUS_PETROLEUM_PRODUCTS")

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SYNTHETIC_FUELS_HELP_4", (1, GC.getBonusInfo(oBonus).getChar(), pCity.getNameKey()))

  return szHelp

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
	if CyPlayer.getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_GREAT_DAM")):
		return False
	if not CyPlayer.getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_COAL_PLANT")):
		return False
	return True


def getHelpAlternativeEnergy1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_ALTERNATIVE_ENERGY_HELP_1", (GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers(), ))

def expireAlternativeEnergy1(argsList):
	if GC.getPlayer(argsList[1].ePlayer).getBuildingCountWithUpgrades(GC.getInfoTypeForString("BUILDING_GREAT_DAM")):
		return True
	return False


def canTriggerAlternativeEnergyDone(argsList):
  kTriggeredData = argsList[0]
  trigger = GC.getEventTriggerInfo(kTriggeredData.eTrigger)
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  i3Gorges = GC.getInfoTypeForString("BUILDING_GREAT_DAM")

  iNuke = GC.getInfoTypeForString("BUILDING_NUCLEAR_PLANT")
  iHydro = GC.getInfoTypeForString("BUILDING_HYDRO_PLANT")
  iTotal = iNuke+iHydro
  iBuildingsRequired = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

  if iBuildingsRequired > (pPlayer.getBuildingCountWithUpgrades(iHydro) + pPlayer.getBuildingCountWithUpgrades(iNuke)):
    return False

  return True

def canDoAlternativeEnergy4(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_GREEN")):
    return True

  return False


def getHelpAlternativeEnergy4(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_ALTERNATIVE_ENERGY_4_HELP", ())

  return szHelp


#################  MORE_FIAT_MONEY ##########################
# Must be poor, can't be running Decentralization; same as Fiat_Money

def canTriggerMoreFiatMoney(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  if pPlayer.getEffectiveGold() > 5000:
    return False
  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_BARTER")):
    return False

  return True



##### INDUSTRIAL_ACTION #####

def canTriggerIndustrialAction(argsList):
	kTriggeredData = argsList[0]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

	SLAVERY = GC.getInfoTypeForString("BUILDING_SLAVERY")
	if SLAVERY > -1 and	CyPlayer.countNumBuildings(SLAVERY):
		return False
	return True

def canDoTriggerCityIndustrialAction(argsList):

  iCity = argsList[2]
  pPlayer = GC.getPlayer(argsList[1])
  pCity = pPlayer.getCity(iCity)

  if pCity.isGovernmentCenter():
    return False

  return True

def canDoIndustrialAction2(argsList):

  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  if pPlayer.isCivic(GC.getInfoTypeForString("CIVIC_PLANNED")):
    return False

  return True

def getHelpIndustrialAction2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_INDUSTRIAL_ACTION_2_HELP", ())

  return szHelp

def getHelpIndustrialAction3(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_INDUSTRIAL_ACTION_3_HELP", ())

  return szHelp


#####  DARWINS_VOYAGE  #####


def canTriggerDarwinsVoyage(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
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

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False


def getHelpTheBuccaneers1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = ("TXT_KEY_EVENT_THE_BUCCANEERS_HELP_1", ())

  return szHelp


def applyTheBuccaneers1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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
    iNumUnits  = 15

  iUnitType = GC.getInfoTypeForString("UNIT_FRENCH_MUSKETEER")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## BLACKBEARD ###########

def canTriggerBlackbeard(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
    return False

###     kTriggeredData.ePlayer must have less than a variable number of combat ships based on map size.
###     Triremes, Galleys, Caravels/Carracks, Galleons/East Indiamen, Transports don't count.
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
  pAggregate = pPlayerPT + pPlayerDD + pPlayerBB + pPlayerCV + pPlayerFF + pPlayerSoL + pPlayerIC + pPlayerSDD + pPlayerMC + pPlayerSSN + pPlayerSSBN

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    if pAggregate > 4:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    if pAggregate > 5:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    if pAggregate > 6:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    if pAggregate > 7:
      return False
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    if pAggregate > 8:
      return False
  else:
    if pAggregate > 9:
      return False


#   At least one civ on the board must know Chemistry.
  bFoundValid = False
  iTech = GC.getInfoTypeForString("TECH_CHEMISTRY")
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False


def getHelpBlackbeard1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_BLACKBEARD_HELP_1", ())

  return szHelp

def applyBlackbeard1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      listPlots.append(i)

  if 0 == len(listPlots):
    return

  plot = map.plotByIndex(listPlots[GAME.getSorenRandNum(len(listPlots), "Blackbeard event location")])

  if map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_DUEL"):
    iNumUnit1  = 1
    iNumUnit2  = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_TINY"):
    iNumUnit1  = 2
    iNumUnit2  = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_SMALL"):
    iNumUnit1  = 3
    iNumUnit2  = 1
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_STANDARD"):
    iNumUnit1  = 4
    iNumUnit2  = 2
  elif map.getWorldSize() == GC.getInfoTypeForString("WORLDSIZE_LARGE"):
    iNumUnit1  = 5
    iNumUnit2  = 2
  else:
    iNumUnit1  = 6
    iNumUnit2  = 2

  iUnitType1 = GC.getInfoTypeForString("UNIT_PRIVATEER")
  iUnitType2 = GC.getInfoTypeForString("UNIT_FRIGATE")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnit1):
    barbPlayer.initUnit(iUnitType1, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_SEA, DirectionTypes.DIRECTION_SOUTH)
  for i in xrange(iNumUnit2):
    barbPlayer.initUnit(iUnitType2, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_SEA, DirectionTypes.DIRECTION_SOUTH)



######## PIRATES_OF_THE_NEUTRAL_ZONES ###########

def canTriggerPiratesoftheNeutralZones(argsList):
	# If Barbarians are disabled in this game, this event will not occur.
	if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
	CyUnit, i = CyPlayer.firstUnit(False)
	while CyUnit:
		if CyUnit.getDomainType() == DomainTypes.DOMAIN_SEA:
			iNavy += CyUnit.baseCombatStr()
		CyUnit, i = CyPlayer.nextUnit(i, False)

	iPirate = GC.getUnitInfo(GC.getInfoTypeForString("UNIT_STEALTH_DESTROYER")).getCombat()

	MAP = GC.getMap()
	iPirate = (MAP.getWorldSize() + 1 + 2*(CyPlayer.getHandicapType() + 1)) * iPirate - iPirate
	if iNavy > iPirate:
		return False

	# Find an eligible plot
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.isLake() and plot.isAdjacentPlayer(iPlayer, True) and not plot.getNumUnits():
			break
	else: return False

	return True

def getHelpPiratesoftheNeutralZones1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_PIRATES_OF_THE_NEUTRAL_ZONES_HELP_1", ())

def applyPiratesoftheNeutralZones1(argsList):
	iPlayer = argsList[1].ePlayer

	plots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
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

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
    return False

###     kTriggeredData.ePlayer must have less than a variable number of combat ships based on map size
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False

def getHelpMalaccanPirates1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_MALACCAN_PIRATES_HELP_1", ())

  return szHelp

def applyMalaccanPirates1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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

  (loopUnit, iter) = barbPlayer.firstUnit(False)
  while (loopUnit):
    if loopUnit.getUnitType() == iUnitType1:
      loopUnit.setHasPromotion(iNav1, True)
      loopUnit.setHasPromotion(iCbt4, True)
      loopUnit.setHasPromotion(iCoAs1, True)
      loopUnit.setName("Malaccan Gunboat")
    (loopUnit, iter) = barbPlayer.nextUnit(iter, False)


######## HENRY_MORGAN ###########

def canTriggerHenryMorgan(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
    return False

###     kTriggeredData.ePlayer must have less than a variable number of combat ships based on map size
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False

def getHelpHenryMorgan1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_HENRY_MORGAN_HELP_1", ())

  return szHelp

def applyHenryMorgan1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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

  (loopUnit, iter) = barbPlayer.firstUnit(False)
  while (loopUnit):
    if loopUnit.getUnitType() == iUnitType1:
      loopUnit.setHasPromotion(iCbt4, True)
    (loopUnit, iter) = barbPlayer.nextUnit(iter, False)

######## STEDE_BONNET ###########

def canTriggerStedeBonnet(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
    return False

###     kTriggeredData.ePlayer must have less than a variable number of combat ships based on map size
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False

def getHelpStedeBonnet1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_STEDE_BONNET_HELP_1", ())

  return szHelp

def applyStedeBonnet1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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

  (loopUnit, iter) = barbPlayer.firstUnit(False)
  while (loopUnit):
    if loopUnit.getUnitType() == iUnitType1:
      loopUnit.setName("Barque")
    if loopUnit.getUnitType() == iUnitType2:
      loopUnit.setName("Fast Galleon")
      loopUnit.setHasPromotion(iNav1, True)
    (loopUnit, iter) = barbPlayer.nextUnit(iter, False)

######## THE_CORSAIRS ###########

def canTriggerTheCorsairs(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
    return False

###     kTriggeredData.ePlayer must have less than a variable number of combat ships based on map size
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False

def getHelpTheCorsairs1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_THE_CORSAIRS_HELP_1", ())

  return szHelp

def applyTheCorsairs1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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

  (loopUnit, iter) = barbPlayer.firstUnit(False)
  while (loopUnit):
    if loopUnit.getUnitType() == iUnitType1:
      sUnitName = TRNSLTR.getText("TXT_KEY_EVENT_THE_CORSAIRS_UNIT_NAME", ())
      loopUnit.setName(sUnitName)
    (loopUnit, iter) = barbPlayer.nextUnit(iter, False)


######## ILLYRIAN_PIRATES ###########

def canTriggerIllyrianPirates(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  map = GC.getMap()

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
    return False

###     kTriggeredData.ePlayer must have less than a variable number of combat ships based on map size
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False

def getHelpIllyrianPirates1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_ILLYRIAN_PIRATES_HELP_1", ())

  return szHelp

def applyIllyrianPirates1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and plot.isWater() and not plot.isImpassable() and not plot.getNumUnits() > 0 and not plot.isLake() and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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

  (loopUnit, iter) = barbPlayer.firstUnit(False)
  while (loopUnit):
    if loopUnit.getUnitType() == iUnitType1:
      sUnitName = TRNSLTR.getText("TXT_KEY_EVENT_ILLYRIAN_PIRATES_UNIT_NAME", ())
      loopUnit.setName(sUnitName)
    (loopUnit, iter) = barbPlayer.nextUnit(iter, False)


######## MAHDI_ARMY ###########

def canTriggerMahdiArmy(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False

def getHelpMahdiArmy1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_MAHDI_ARMY_HELP_1", ())

  return szHelp


def applyMahdiArmy1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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
    iNumUnits  = 15

  iUnitType = GC.getInfoTypeForString("UNIT_MUSKETMAN")

  barbPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
  for i in xrange(iNumUnits):
    barbPlayer.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ATTACK_CITY_LEMMING, DirectionTypes.DIRECTION_SOUTH)


######## THE_TALIBAN ###########

def canTriggerTheTaliban(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False

def getHelpTheTaliban1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_THE_TALIBAN_HELP_1", ())

  return szHelp


def applyTheTaliban1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
  iObsrvty = GC.getInfoTypeForString("BUILDING_OBSERVATORY")

  if pPlayer.getBuildingCountWithUpgrades(iObsrvty) < 1:
    return False

  if GAME.isVictoryValid(GC.getInfoTypeForString("VICTORY_SPACE_RACE")):
    return True

  return False

######## Sailing founded ############

def canTriggerSailingFounded(argsList):
  eTrigger = argsList[0]
  ePlayer = argsList[1]
  iCity = argsList[2]

  player = GC.getPlayer(ePlayer)
  city = player.getCity(iCity)

  if city.isNone():
    return False

  if not city.isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()):
    return False

  if city.plot().getLatitude() <= 0:
    return False

  return True

######## Chariotry founded ############

def canTriggerChariotryFounded(argsList):
  eTrigger = argsList[0]
  ePlayer = argsList[1]
  iCity = argsList[2]

  player = GC.getPlayer(ePlayer)
  city = player.getCity(iCity)

  iHorse = GC.getInfoTypeForString("BONUS_HORSE")

  if city.isNone():
    return False

  if (city.plot().getLatitude() <= 0):
    return False

  if (city.getNumBonuses(iHorse) < 1):
    return False

  return True

######## MERCENARIES ANCIENT ###########

def canTriggerMercenariesAncient(argsList):
	kTriggeredData = argsList[0]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

	if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0
		and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)
		): return True
	return False


def getHelpMercenariesAncient1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_MERCENARIES_ANCIENT_HELP_1", ())

def applyMercenariesAncient1(argsList):
	kTriggeredData = argsList[1]

	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0
		and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)
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
	kTriggeredData = argsList[1]
	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0
		and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)
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
		CyUnit = GC.getPlayer(kTriggeredData.ePlayer).initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.DIRECTION_SOUTH)
		CyUnit.setName("Mercenary Warrior")
		iCount -= 1

######## MERCENARIES CLASSICAL ###########

def canTriggerMercenariesClassical(argsList):
	kTriggeredData = argsList[0]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

	if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0
		and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)
		): return True
	return False

def getHelpMercenariesClassical1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_MERCENARIES_CLASSICAL_HELP_1", ())

def applyMercenariesClassical1(argsList):
	kTriggeredData = argsList[1]

	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0
		and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)
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
	kTriggeredData = argsList[1]
	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0
		and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)
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
		CyUnit = GC.getPlayer(kTriggeredData.ePlayer).initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.DIRECTION_SOUTH)
		CyUnit.setName("Mercenary Warrior")
		iCount -= 1

######## MERCENARIES MEDIEVAL ###########

def canTriggerMercenariesMedieval(argsList):
	kTriggeredData = argsList[0]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

	if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0
		and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)
		): return True
	return False

def getHelpMercenariesMedieval1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_MERCENARIES_MEDIEVAL_HELP_1", ())

def applyMercenariesMedieval1(argsList):
	kTriggeredData = argsList[1]

	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0
		and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)
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
	kTriggeredData = argsList[1]
	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0
		and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)
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
		CyUnit = GC.getPlayer(kTriggeredData.ePlayer).initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.DIRECTION_SOUTH)
		CyUnit.setName("Mercenary Warrior")
		iCount -= 1

######## EARTHQUAKE ###########

def getHelpEarthquake1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_EARTHQUAKE_1_HELP", ())

  return szHelp

def canApplyEarthquake1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iNumImprovements = 0
  for iDX in xrange(-1, 2):
    for iDY in xrange(-1, 2):
      loopPlot = plotXY(kTriggeredData.iPlotX, kTriggeredData.iPlotY, iDX, iDY)
      if not loopPlot.isNone():
        if (iDX != 0 or iDY != 0):
          if loopPlot.getImprovementType() != -1:
            iNumImprovements += 1

  return (iNumImprovements > 0)

def applyEarthquake1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  listPlots = []
  for iDX in xrange(-1, 2):
    for iDY in xrange(-1, 2):
      loopPlot = plotXY(kTriggeredData.iPlotX, kTriggeredData.iPlotY, iDX, iDY)
      if not loopPlot.isNone():
        if (iDX != 0 or iDY != 0):
          if loopPlot.getImprovementType() != -1:
            listPlots.append(loopPlot)

  listRuins = []
  listRuins.append(GC.getInfoTypeForString("IMPROVEMENT_COTTAGE"))
  listRuins.append(GC.getInfoTypeForString("IMPROVEMENT_HAMLET"))
  listRuins.append(GC.getInfoTypeForString("IMPROVEMENT_VILLAGE"))
  listRuins.append(GC.getInfoTypeForString("IMPROVEMENT_TOWN"))

  iRuins = GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS")

  for i in xrange(3):
    if len(listPlots) > 0:
      plot = listPlots[GAME.getSorenRandNum(len(listPlots), "Earthquake event improvement destroyed")]
      iImprovement = plot.getImprovementType()
      szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getImprovementInfo(iImprovement).getTextKey(), ))
      CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getImprovementInfo(iImprovement).getButton(), GC.getInfoTypeForString("COLOR_RED"), plot.getX(), plot.getY(), True, True)
      if iImprovement in listRuins:
        plot.setImprovementType(iRuins)
      else:
        plot.setImprovementType(-1)
      listPlots.remove(plot)

      if i == 1 and GAME.getSorenRandNum(100, "Earthquake event num improvements destroyed") < 50:
        break


####### Assassin Discovered #######


def canDoAssassinDiscovered3(argsList):
	if GC.getPlayer(argsList[1].ePlayer).getCapitalCity().isNone():
		return False
	return True

def doAssassinDiscovered3(argsList):
	CyPlayer = GC.getPlayer(argsList[1].ePlayer)

	plot = CyPlayer.getCapitalCity().plot()
	x = plot.getX()
	y = plot.getY()

	iUnit = GC.getInfoTypeForString("UNIT_AXEMAN")
	iCount = 1 + CyPlayer.getNumCities() / 4
	while iCount > 0:
		CyPlayer.initUnit(iUnit, x, y, UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)
		iCount -= 1

def getHelpAssassinDiscovered3(argsList):
  return TRNSLTR.getText("TXT_KEY_EVENT_ASSASSIN_DISCOVERED_3_HELP", (1+GC.getPlayer(argsList[1].ePlayer).getNumCities()/4,))

######## BLACK DEATH PLAGUE ###########

def canTriggerBlackDeath(argsList):
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  team = GC.getTeam(player.getTeam())

  iClassical = GC.getInfoTypeForString("C2C_ERA_CLASSICAL")

  if player.getCurrentEra() <= iClassical:
    return False

  iMedicine = GC.getInfoTypeForString("TECH_MEDICINE")

  if team.isHasTech(iMedicine):
    return False

  return True

def applyBlackDeath2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

  iNumCities = 2 + GAME.getSorenRandNum(3, "Black Death event number of cities")

  listCities = []
  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    if loopCity.getPopulation() > 2:
      iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())
      if iDistance > 0:
        listCities.append((iDistance, loopCity))
    (loopCity, iter) = player.nextCity(iter, False)

  listCities.sort(key=itemgetter(0))

  if iNumCities > len(listCities):
    iNumCities = len(listCities)

  for i in xrange(iNumCities):
    (iDist, loopCity) = listCities[i]
    loopCity.changePopulation(-2)
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_BLACK_DEATH_HIT_CITY", (loopCity.getNameKey(), ))
    CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), loopCity.getX(), loopCity.getY(), True, True)


def getHelpBlackDeath2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_BLACK_DEATH_HELP_2", (2, ))

  return szHelp

######## SMALLPOX EPIDEMIC ###########

def canTriggerSmallpox(argsList):
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  team = GC.getTeam(player.getTeam())

  iClassical = GC.getInfoTypeForString("C2C_ERA_CLASSICAL")

  if player.getCurrentEra() <= iClassical:
    return False

  iMedicine = GC.getInfoTypeForString("TECH_MEDICINE")

  if team.isHasTech(iMedicine):
    return False

  return True

def applySmallpox2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

  iNumCities = 1 + GAME.getSorenRandNum(3, "Smallpox event number of cities")

  listCities = []
  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    if loopCity.getPopulation() > 2:
      iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())
      if iDistance > 0:
        listCities.append((iDistance, loopCity))
    (loopCity, iter) = player.nextCity(iter, False)

  listCities.sort(key=itemgetter(0))

  if iNumCities > len(listCities):
    iNumCities = len(listCities)

  for i in xrange(iNumCities):
    (iDist, loopCity) = listCities[i]
    loopCity.changePopulation(-1)
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SMALLPOX_HIT_CITY", (loopCity.getNameKey(), ))
    CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), loopCity.getX(), loopCity.getY(), True, True)


def getHelpSmallpox2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SMALLPOX_HELP_2", (2, ))

  return szHelp


####### Measles ########

def canTriggerMeasles(argsList):
  ePlayer = argsList[1]
  iCity = argsList[2]

  player = GC.getPlayer(ePlayer)
  city = player.getCity(iCity)

  if city.isNone():
    return False

  # city health is positive, no epidemic
  if ((city.goodHealth() - city.badHealth(True)) > 1):
    return False

  return True



  ######## THE SILVER RAIN ###########

def canTriggerSilverRain(argsList):

  kTriggeredData = argsList[0]
  pPlayer = GC.getPlayer(kTriggeredData.ePlayer)

#   If Barbarians are disabled in this game, this event will not occur.
  if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS):
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
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
      return True

  return False

def getHelpSilverRain1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SILVER_RAIN_HELP_1", ())

  return szHelp


def applySilverRain1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  listPlots = []
  map = GC.getMap()
  for i in xrange(map.numPlots()):
    plot = map.plotByIndex(i)
    if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable() and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0 and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)):
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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SILVER_RAIN_HELP_3", ())

  return szHelp

def applySilverRain3(argsList):
	kTriggeredData = argsList[1]

	listPlots = []
	MAP = GC.getMap()
	for i in xrange(MAP.numPlots()):
		plot = MAP.plotByIndex(i)
		if (plot.getOwner() == -1 and not plot.isWater() and not plot.isImpassable()
		and plot.area().getCitiesPerPlayer(kTriggeredData.ePlayer) > 0
		and plot.isAdjacentPlayer(kTriggeredData.ePlayer, True)
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
	CyCity, i = player.firstCity(False)
	while CyCity:
		if CyCity.canTrain(iCounterUnit1, False, False, False, False) or CyCity.canTrain(iCounterUnit2, False, False, False, False):
			iNukeUnit = GC.getPlayer(GC.getBARBARIAN_PLAYER()).initUnit(GC.getInfoTypeForString("UNIT_NANITE_CLOUD"), x, y, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
			plot.nukeExplosion(1, iNukeUnit)
			iNukeUnit.kill(False, -1)
			break
		CyCity, i = player.nextCity(i, False)
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

  kTriggeredData = argsList[0]

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  # check for modmods if they add sea cities, do not allow treasure unit to spawn on water plots if city ruins are in water
  if (plot.isWater() or plot.isImpassable()):
      return False

  return True

def getHelpCityRuinsTreasure1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  iUnitType = GC.getInfoTypeForString("UNIT_TREASURE")
  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_BONUS_UNIT", (1, GC.getUnitInfo(iUnitType).getTextKey()))

  return szHelp

def applyCityRuinsTreasure1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  plot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  iUnitType = GC.getInfoTypeForString("UNIT_TREASURE")
  iNumUnits = 1

  for i in xrange(iNumUnits):
    player.initUnit(iUnitType, plot.getX(), plot.getY(), UnitAITypes.UNITAI_ENGINEER, DirectionTypes.DIRECTION_SOUTH)

# ###########Compulsory Democracy########### #

def canTriggerCompulsoryDem(argsList):
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  eCivic = player.getCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
  iFederal = GC.getInfoTypeForString("CIVIC_FEDERALISM")
  iDemocracy = GC.getInfoTypeForString("CIVIC_DEMOCRACY")
  iRepublic = GC.getInfoTypeForString("CIVIC_REPUBLIC")
  if eCivic == iFederal or eCivic == iDemocracy or eCivic == iRepublic:
    return True
  else:
    return False

def canEventTriggerCompulsoryDem(argsList):
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  if (player.isAnarchy() or player.getRevolutionTimer() > 0):
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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

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
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

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
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	iPlayer = kTriggeredData.ePlayer
	CyPlayer = GC.getPlayer(iPlayer)

	iLibrary = GC.getInfoTypeForString("BUILDING_LIBRARY")
	iStateReligion = CyPlayer.getStateReligion()
	CyCity, i = CyPlayer.firstCity(False)
	while CyCity:
		if CyCity.canConstruct(iLibrary, False, False, True):
			CyCity.setNumRealBuilding(iLibrary, 1)

		if CyCity.isHasReligion(iStateReligion):
			CyCity.changeHurryAngerTimer(CyCity.flatHurryAngerLength())

		CyCity, i = CyPlayer.nextCity(i, False)

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
  kTriggeredData = argsList[0]
  player = GC.getPlayer(kTriggeredData.ePlayer)

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
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    loopCity.setWeLoveTheKingDay(True)
    (loopCity, iter) = player.nextCity(iter, False)

def getHelpLessDictator1(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_NATIONAL_HOLIDAY",())

def TriggerLessDictator2(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)

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

	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

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
	kTriggeredData = argsList[1]

	if GC.getPlayer(kTriggeredData.ePlayer).getCity(kTriggeredData.iCityId).isPower():
		return False
	return True

def TriggerUnlimitedPower1(argsList):
	kTriggeredData = argsList[1]
	GC.getPlayer(kTriggeredData.ePlayer).getCity(kTriggeredData.iCityId).changePowerCount(1, False)

def getHelpUnlimitedPower1(argsList):
	kTriggeredData = argsList[1]
	return TRNSLTR.getText("TXT_KEY_EVENT_PROVIDES_POWER", (GC.getPlayer(kTriggeredData.ePlayer).getCity(kTriggeredData.iCityId).getNameKey(),))

def canTriggerUnlimitedPower2(argsList):
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

  if (eventCity.area().isNone()):
    return False
  if (eventCity.area().isCleanPower(player.getTeam())):
    return False
  if (player.getEffectiveGold() < (eventCity.area().getNumCities() * 100)):
    return False
  return True

def TriggerUnlimitedPower2(argsList):
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyArea = CyPlayer.getCity(kTriggeredData.iCityId).area()
	iRequireGold = 100 * CyArea.getNumCities()
	CyPlayer.changeGold(-iRequireGold)
	CyArea.changeCleanPowerCount(CyPlayer.getTeam(), 1)

def getHelpUnlimitedPower2(argsList):
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

	iGold = CyPlayer.getCity(kTriggeredData.iCityId).area().getNumCities() * 100
	if CyPlayer.getEffectiveGold() < iGold:
		szGold = "\n<color=255,76,76>"
	else: szGold = "\n"
	szGold += TRNSLTR.getText("TXT_KEY_EVENT_COSTS_GOLD", (iGold,))

	return TRNSLTR.getText("TXT_KEY_EVENT_PROVIDES_AREA_POWER",()) + szGold

def TriggerSuperVirus1(argsList):
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

  iNumCities = 4 + GAME.getSorenRandNum(5, "Super Virus event number of cities")

  listCities = []
  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    loopCity.changeEventAnger(3)
    if loopCity.getPopulation() > 4:
      iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())
      if iDistance > 0:
        listCities.append((iDistance, loopCity))
    (loopCity, iter) = player.nextCity(iter, False)

  listCities.sort(key=itemgetter(0))

  if iNumCities > len(listCities):
    iNumCities = len(listCities)

  iKilledPop = (eventCity.getPopulation() * 75) / 100

  eventCity.changePopulation(-iKilledPop)
  szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (iKilledPop, eventCity.getNameKey() , ))
  CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), eventCity.getX(), eventCity.getY(), True, True)

  for i in xrange(iNumCities):
    (iDist, loopCity) = listCities[i]
    loopCity.changePopulation(-4)
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (4, loopCity.getNameKey() , ))
    CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), loopCity.getX(), loopCity.getY(), True, True)

def getHelpSuperVirus1(argsList):
	kTriggeredData = argsList[1]

	szCityName = TRNSLTR.getText(GC.getPlayer(kTriggeredData.ePlayer).getCity(kTriggeredData.iCityId).getNameKey(),())

	return TRNSLTR.getText("TXT_KEY_EVENT_DOOMED_OPTION",()) + "\n" + TRNSLTR.getText("TXT_KEY_EVENT_CITY_UNREST", (3, szCityName))


def TriggerSuperVirus2(argsList):
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

  iNumCities = 1 + GAME.getSorenRandNum(3, "Super Virus event number of cities")

  iRequireGold = 100 + player.getNumCities() * 35
  player.changeGold(-iRequireGold)

  listCities = []
  (loopCity, iter) = player.firstCity(False)
  while(loopCity):
    loopCity.changeEventAnger(1)
    if loopCity.getPopulation() > 4:
      iDistance = plotDistance(eventCity.getX(), eventCity.getY(), loopCity.getX(), loopCity.getY())
      if iDistance > 0:
        listCities.append((iDistance, loopCity))
    (loopCity, iter) = player.nextCity(iter, False)

  listCities.sort(key=itemgetter(0))

  if iNumCities > len(listCities):
    iNumCities = len(listCities)

  iKilledPop = (eventCity.getPopulation() * 45) / 100

  eventCity.changePopulation(-iKilledPop)
  szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (iKilledPop, eventCity.getNameKey() , ))
  CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), eventCity.getX(), eventCity.getY(), True, True)

  for i in xrange(iNumCities):
    (iDist, loopCity) = listCities[i]
    loopCity.changePopulation(-2)
    szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (2, loopCity.getNameKey() , ))
    CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), loopCity.getX(), loopCity.getY(), True, True)

def getHelpSuperVirus2(argsList):
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

	szHelp = TRNSLTR.getText("TXT_KEY_EVENT_SMALL_VIRUS_SPREAD",()) + "\n"
	szHelp += TRNSLTR.getText("TXT_KEY_EVENT_CITY_UNREST", (1, CyPlayer.getCity(kTriggeredData.iCityId).getNameKey()))

	iGold = 100 + CyPlayer.getNumCities() * 35
	if CyPlayer.getEffectiveGold() < iGold:
		szHelp += "\n<color=255,76,76>"
	else: szHelp += "\n"

	return szHelp + TRNSLTR.getText("TXT_KEY_EVENT_COSTS_GOLD", (iGold,))

def canTriggerSuperVirus2(argsList):
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)

	if CyPlayer.getEffectiveGold() < 100 + CyPlayer.getNumCities() * 35:
		return False
	return True

def TriggerSuperVirus3(argsList):
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

  iRequireGold = 100 + player.getNumCities() * 65
  player.changeGold(-iRequireGold)

  iChangePopulation = eventCity.getPopulation() * 25
  iChangePopulation /= 100
  eventCity.changePopulation(-iChangePopulation)
  szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (iChangePopulation, eventCity.getNameKey()))
  CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), eventCity.getX(), eventCity.getY(), True, True)

  eventCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_GREAT_SCIENTIST"), 2)

def getHelpSuperVirus3(argsList):
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyCity = CyPlayer.getCity(kTriggeredData.iCityId)

	szCityNameKey = CyCity.getNameKey()
	szHelp = TRNSLTR.getText("TXT_KEY_EVENT_FREE_SPECIALISTS", (2, GC.getSpecialistInfo(GC.getInfoTypeForString("SPECIALIST_GREAT_SCIENTIST")).getTextKey(), szCityNameKey))
	szHelp += "\n" + TRNSLTR.getText("TXT_KEY_EVENT_LOSE_POPULATION", (CyCity.getPopulation()*25/100, szCityNameKey))

	iGold = 100 + CyPlayer.getNumCities() * 65

	if CyPlayer.getEffectiveGold() < iGold:
		szHelp += "\n<color=255,76,76>" + TRNSLTR.getText("TXT_KEY_EVENT_COSTS_GOLD", (iGold,)) + "</color>"
	else: szHelp += "\n" + TRNSLTR.getText("TXT_KEY_EVENT_COSTS_GOLD", (iGold,))

	iSmartMedicine = GC.getInfoTypeForString("BONUS_SMART_MEDICINE")
	if iSmartMedicine > 0:
		if not CyCity.hasBonus(iSmartMedicine):
			szHelp += "\n" + TRNSLTR.getText("TXT_KEY_BUILDING_REQUIRES_STRING", (GC.getBonusInfo(iSmartMedicine).getTextKey(),))

	iSmartDrugs = GC.getInfoTypeForString("TECH_SMART_DRUGS")
	if iSmartDrugs > 0:
		if not GC.getTeam(CyPlayer.getTeam()).isHasTech(iSmartDrugs):
			szHelp += "\n" + TRNSLTR.getText("TXT_KEY_BUILDING_REQUIRES_STRING", (GC.getTechInfo(iSmartDrugs).getTextKeyWide(),))

	return szHelp

def canTriggerSuperVirus3(argsList):
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)
  team = GC.getTeam(player.getTeam())

  iRequireGold = 100 + player.getNumCities() * 65
  if (player.getEffectiveGold() < (iRequireGold)):
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
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

  iChangePopulation = eventCity.getPopulation() * 75
  iChangePopulation /= 100

  eventCity.changeOccupationTimer(iChangePopulation)
  eventCity.changeEventAnger(iChangePopulation)

  eventCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_HOSPITAL"), 0)

  eventCity.changePopulation(-iChangePopulation)
  szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_SUPER_VIRUS_HIT_CITY", (iChangePopulation, eventCity.getNameKey()))
  CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), eventCity.getX(), eventCity.getY(), True, True)

  eventCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_GREAT_MILITARY_INSTRUCTOR"), 1)

def getHelpSuperVirus4(argsList):

  iEvent = argsList[0]
  kTriggeredData = argsList[1]
  player = GC.getPlayer(kTriggeredData.ePlayer)
  eventCity = player.getCity(kTriggeredData.iCityId)

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
	kTriggeredData = argsList[0]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
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
	kTriggeredData = argsList[1]
	iPlayer = kTriggeredData.ePlayer
	CyPlayer = GC.getPlayer(iPlayer)

	if iEvent == GC.getInfoTypeForString("EVENT_NEW_WORLD_2"):
		iNeededCities = 1
	else:
		iNeededCities = 3

	CyCity, i = CyPlayer.firstCity(False)
	while CyCity:
		if iEvent == GC.getInfoTypeForString("EVENT_NEW_WORLD_2"):
			if CyCity.getPopulation() > 4:
				CyCity.changePopulation(-1)
		else:
			if CyCity.getPopulation() > 9:
				CyCity.changePopulation(-2)
			elif CyCity.getPopulation() > 4:
				CyCity.changePopulation(-1)
		CyCity, i = CyPlayer.nextCity(i, False)

	iNumUnits = GC.getNumUnitInfos()
	MAP = GC.getMap()
	iNumPlots = MAP.numPlots()
	while iNeededCities > 0:
		iBestValue = 0
		pBestPlot = None
		for i in xrange(iNumPlots):
			CyPlot = MAP.plotByIndex(i)
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
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_WALLS"), 1)
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_GRANARY"), 1)
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_FORGE"), 1)
		else:
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_WALLS"), 1)
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_BARRACKS"), 1)
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_GRANARY"), 1)
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_FORGE"), 1)
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_MARKET"), 1)
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_HARBOR"), 1)
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_LIGHTHOUSE"), 1)
			CyCity.setNumRealBuilding(GC.getInfoTypeForString("BUILDING_FISHERMAN_HUT"), 1)
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
			szHelp += "\n" + TRNSLTR.getText("TXT_KEY_BUILDING_REQUIRES_STRING", (GC.getTechInfo(iNavigation).getTextKeyWide(),))
	return szHelp

##### VOLCANO C2C #####
def canDoNewVolcano(argsList):
  # Checks the plot to see if a new volcano can be formed
  kTriggeredData = argsList[0]
  pPlot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  if pPlot.isNone():
    return False

  # List of features that block new volcanoes
  listFeatures = [GC.getInfoTypeForString('FEATURE_PLATY_AUYANTEPUI'),
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
                  GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT')]

  # List of resources that count as volcanic, and thus do not block volcanoes.
  listVolcanicResources = [GC.getInfoTypeForString('BONUS_OBSIDIAN'),
                           GC.getInfoTypeForString('BONUS_SULPHUR')]

  if pPlot.isCity():
    return False
  if pPlot.getFeatureType() in listFeatures:
    return False
  if pPlot.getBonusType(-1) == -1:
    return True
  if pPlot.getBonusType(-1) in listVolcanicResources:
    return True

  return False

def canDoOldVolcano(argsList):
  kTriggeredData = argsList[0]
  pPlot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)

  if pPlot.isNone():
    return False

  # List of features that are volcanoes
  listVolcanoes = [GC.getInfoTypeForString('FEATURE_PLATY_FUJI'),
                  GC.getInfoTypeForString('FEATURE_PLATY_SOPKA'),
                  GC.getInfoTypeForString('FEATURE_PLATY_KRAKATOA'),
                  GC.getInfoTypeForString('FEATURE_PLATY_KILIMANJARO'),
                  GC.getInfoTypeForString('FEATURE_VOLCANO_ACTIVE'),
                  GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT')]

  if pPlot.isCity():
    return False
  elif pPlot.getFeatureType() in listVolcanoes:
    return True

  return False

def doVolcanoAdjustFertility(argsList):
  pPlot = argsList[0]
  extraFood = argsList[1]
  team = argsList[2]
  ## For each neighbouring plot
  ##   If extraFood is -1, 0 or 1 this is the amount of food to add to the plot

  if pPlot.isNone() or pPlot.isCity():
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
    tPlot = CvUtil.plotDirection(iX, iY, DirectionTypes(i))
    if not tPlot.isNone():
      if not tPlot.isCity():
        GAME.setPlotExtraYield(tPlot.getX(), tPlot.getY(), YieldTypes.YIELD_FOOD, extraFood)

def doVolcanoNeighbouringPlots(pPlot):
  # Directional eruption that picks an adjacent valid plot to erupt towards.
  # It then affects that plot and its two neighbors in the ring of 8 plots surrounding the volcano
  # Affected plots have units damaged, improvements destroyed, and oceans changed to coast.
  # To do - start forest fire

  if pPlot.isNone():
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
  listRuins = [GC.getInfoTypeForString("IMPROVEMENT_COTTAGE"),
               GC.getInfoTypeForString("IMPROVEMENT_HAMLET"),
               GC.getInfoTypeForString("IMPROVEMENT_VILLAGE"),
               GC.getInfoTypeForString("IMPROVEMENT_TOWN"),
               GC.getInfoTypeForString("IMPROVEMENT_SUBURBS")]
  iRuins = GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS")

  # List of improvements that are unaffected by eruption
  immuneImprovements = [GC.getInfoTypeForString("IMPROVEMENT_SEED_CAMP"),
               GC.getInfoTypeForString("IMPROVEMENT_WOOD_GATHERER"),
               GC.getInfoTypeForString("IMPROVEMENT_STONE_TOOLS_WORKSHOP"),
               GC.getInfoTypeForString("IMPROVEMENT_SCAVENGING_CAMP"),
               GC.getInfoTypeForString("IMPROVEMENT_FRUIT_GATHERING_CAMP"),
               GC.getInfoTypeForString("IMPROVEMENT_PLANT_GATHERING_CAMP"),
               GC.getInfoTypeForString("IMPROVEMENT_FISHING_BOATS"),
               GC.getInfoTypeForString("IMPROVEMENT_WHALING_BOATS"),
               GC.getInfoTypeForString("IMPROVEMENT_WHALING_SHIPS"),
               GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS")]

  listVolcanoPlots = []
  listVolcanoPlotsX = []
  listVolcanoPlotsY = []
  listAdjacentPlots = []
  listAffectedPlots = []

  # Sets up lists for plots that are adjacent to the volcano
  for i in xrange(8):
    plot = CvUtil.plotDirection(iX, iY, DirectionTypes(i))
    if not plot.isNone():
      listVolcanoPlots.append(plot)
      listVolcanoPlotsX.append(plot.getX())
      listVolcanoPlotsY.append(plot.getY())

  # Select a target plot
  targetplot = listVolcanoPlots[GAME.getSorenRandNum(len(listVolcanoPlots), "Volcano direction")]
  listAffectedPlots.append(targetplot)

  listAdjacentPlots.append(CvUtil.plotDirection(targetplot.getX(), targetplot.getY(), DirectionTypes.DIRECTION_NORTH))
  listAdjacentPlots.append(CvUtil.plotDirection(targetplot.getX(), targetplot.getY(), DirectionTypes.DIRECTION_SOUTH))
  listAdjacentPlots.append(CvUtil.plotDirection(targetplot.getX(), targetplot.getY(), DirectionTypes.DIRECTION_EAST))
  listAdjacentPlots.append(CvUtil.plotDirection(targetplot.getX(), targetplot.getY(), DirectionTypes.DIRECTION_WEST))

  # If plot is in the ring around the volcano, add to the list of affected plots
  for i in xrange(len(listAdjacentPlots)):
    plot = listAdjacentPlots[i]
    if not plot.isNone():
      if (plot.getX() in listVolcanoPlotsX) and (plot.getY() in listVolcanoPlotsY):
        if (plot.getX() != iX) and (plot.getY() != iY):
          listAffectedPlots.append(plot)

  #Loops through the list of affected plots applying eruption effects
  for i in xrange(len(listAffectedPlots)):
    if len(listAffectedPlots) > 0:
      plot = listAffectedPlots[GAME.getSorenRandNum(len(listAffectedPlots), "Volcano event improvement destroyed")]
      iPlayer = plot.getOwner()
      iImprovement = plot.getImprovementType()

      # Destroys improvements if the plot is not a city, and if the improvement is not immune
      if iImprovement != -1:
        if not(plot.isCity()) and not(iImprovement in immuneImprovements):
          if iPlayer > -1:
            szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED_NOOWNER", (GC.getImprovementInfo(iImprovement).getTextKey(), ))
            CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getImprovementInfo(iImprovement).getButton(), GC.getInfoTypeForString("COLOR_RED"), plot.getX(), plot.getY(), True, True)
          if iImprovement in listRuins:
            plot.setImprovementType(iRuins)
          else:
            plot.setImprovementType(-1)

      # Damages units
      iNumberOfUnits = plot.getNumUnits()
      if iNumberOfUnits > 0:
        for iUnit in xrange(iNumberOfUnits):
          pPlotUnit = plot.getUnit(iUnit)
          if pPlotUnit.getDamage() < 50:
            pPlotUnit.setDamage(50, False)
          elif pPlotUnit.getDamage() < 75:
            pPlotUnit.setDamage(75, False)
          elif pPlotUnit.getDamage() < 90:
            pPlotUnit.setDamage(90, False)
          else:
            pPlotUnit.setDamage(99, False)

      # If affected plot is Ocean or Sea, change it to Coast
      if plot.isWater():
        if plot.getTerrainType() == terrainOcean or plot.getTerrainType() == terrainSea:
          plot.setTerrainType(terrainCoast, True, True)
        if plot.getTerrainType() == terrainPolarOcean or plot.getTerrainType() == terrainPolarSea:
          plot.setTerrainType(terrainPolarCoast, True, True)
        if plot.getTerrainType() == terrainTropicalOcean or plot.getTerrainType() == terrainTropicalSea:
          plot.setTerrainType(terrainTropicalCoast, True, True)

      # Remove processed plots from list
      listAffectedPlots.remove(plot)


def doVolcanoPlot(pPlot):
  if pPlot.isNone():
     return

  # List of features that are volcanoes
  listVolcanoes = [GC.getInfoTypeForString('FEATURE_PLATY_FUJI'),
                  GC.getInfoTypeForString('FEATURE_PLATY_SOPKA'),
                  GC.getInfoTypeForString('FEATURE_PLATY_KRAKATOA'),
                  GC.getInfoTypeForString('FEATURE_PLATY_KILIMANJARO'),
                  GC.getInfoTypeForString('FEATURE_VOLCANO_ACTIVE'),
                  GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT')]
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
  if iFeature == ft_volcano_dormant:
    pPlot.setFeatureType(ft_volcano_active, 0)
  elif not(iFeature in listVolcanoes):
    pPlot.setFeatureType(ft_volcano_active, 0)

  # Wound any units on the same plot as the volcano
  iNumberOfUnits = pPlot.getNumUnits()
  if iNumberOfUnits > 0:
    for i in xrange(0, iNumberOfUnits):
      pPlotUnit = pPlot.getUnit(i)
      if pPlotUnit.getDamage() < 90: pPlotUnit.setDamage(90, False)
      else: pPlotUnit.setDamage(99, False)

      # move them to safety
      iX = pPlot.getX()
      iY = pPlot.getY()
      for i in xrange(8):
        sPlot = CvUtil.plotDirection(iX, iY, DirectionTypes(i))
        if not sPlot.isNone():
          if pPlotUnit.canMoveInto(sPlot, False, False, True):
            pPlotUnit.setXY(sPlot.getX(), sPlot.getY(), False, True, True)

  if pPlot.isWater(): pPlot.setPlotType(PlotTypes.PLOT_LAND, True, True)

  return

def doVolcanoReport(argsList):
  pPlot = argsList[0]
  szBuffer = argsList[1]
  ft_volcano_dormant = GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT')
  ft_volcano_active = GC.getInfoTypeForString('FEATURE_VOLCANO_ACTIVE')

  # report message to any one who can see this plot
  iMaxPlayer = GC.getMAX_PC_PLAYERS()
  for i in xrange(iMaxPlayer):
    loopPlayer = GC.getPlayer(i)
    if loopPlayer.isHuman() and loopPlayer.isAlive() and pPlot.isVisible(loopPlayer.getTeam(), False):
      CyInterface().addMessage(loopPlayer.getID(), False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getFeatureInfo(ft_volcano_active).getButton(), GC.getInfoTypeForString("COLOR_RED"), pPlot.getX(), pPlot.getY(), True, True)

      if pPlot.isInViewport():
        point = pPlot.getPoint()
        CyEngine().triggerEffect(GC.getInfoTypeForString('EFFECT_ARTILLERY_SHELL_EXPLODE'),point)
        CyAudioGame().Play3DSound("AS3D_UN_GRENADE_EXPLODE",point.x,point.y,point.z)


def doVolcanoNewEruption(argsList):
  kTriggeredData = argsList[0]
  pPlot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  if pPlot.isNone():
    return
  player = GC.getPlayer(kTriggeredData.ePlayer)
  team = player.getTeam()

  doVolcanoPlot(pPlot)
  doVolcanoNeighbouringPlots(pPlot)
  doVolcanoAdjustFertility((pPlot, 1, team))
  doVolcanoReport((pPlot, BugUtil.getPlainText("TXT_KEY_EVENT_TRIGGER_VOLCANO_NEW")))

  return

def doVolcanoExistingEruption(argsList):
  kTriggeredData = argsList[0]
  pPlot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  if pPlot.isNone():
    return

  doVolcanoNeighbouringPlots(pPlot)
  doVolcanoReport((pPlot, BugUtil.getPlainText("TXT_KEY_EVENTTRIGGER_VOLCANO_ACTIVE")))

  return

def doVolcanoDormantEruption(argsList):
  kTriggeredData = argsList[0]
  pPlot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  if pPlot.isNone():
    return
  player = GC.getPlayer(kTriggeredData.ePlayer)
  team = player.getTeam()

  doVolcanoPlot(pPlot)
  doVolcanoNeighbouringPlots(pPlot)
  doVolcanoAdjustFertility((pPlot, 1, team))
  doVolcanoReport((pPlot, BugUtil.getPlainText("TXT_KEY_EVENT_TRIGGER_VOLCANO_EXTINCT")))

  return
def doVolcanoExtinction(argsList):
  kTriggeredData = argsList[0]
  pPlot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  if pPlot.isNone():
    return

  pPlot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
  pPlot.setTerrainType(GC.getInfoTypeForString('TERRAIN_ROCKY'), True, True)
  player = GC.getPlayer(kTriggeredData.ePlayer)
  city = player.getCity(kTriggeredData.iCityId)

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

  if techteam.isHasTech(itechresource) and (pPlot.isVisible(team, False)):
      doVolcanoReport((pPlot, BugUtil.getPlainText("TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE_VOLCANO")))

def doVolcanoSleep(argsList):
  kTriggeredData = argsList[0]
  pPlot = GC.getMap().plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
  pPlot.setFeatureType(GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT'), 0)
  player = GC.getPlayer(kTriggeredData.ePlayer)
  team = player.getTeam()
  doVolcanoAdjustFertility((pPlot, -1, team))
  doVolcanoReport((pPlot, BugUtil.getPlainText("TXT_KEY_EVENT_TRIGGER_VOLCANO_DORMANT")))

def getHelpVolcanoEruption1(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_VOLCANO_ERUPTION_1_HELP", ())

  return szHelp

def getHelpVolcanoSleep(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_VOLCANO_SLEEP_HELP", ())

  return szHelp

def getHelpVolcanoExtinction(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  szHelp = TRNSLTR.getText("TXT_KEY_EVENT_VOLCANO_EXTINCTION_HELP", ())

  return szHelp

### Fire events for C2C

def doMinorFire(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(kTriggeredData.iCityId)

	iFlammStart = CyCity.getProperties().getValueByProperty(GC.getInfoTypeForString("PROPERTY_FLAMMABILITY"))

	iPop = CyCity.getPopulation()
	iFlammRand = iFlammStart/5 + iPop*iPop

	iBurnBuilding = -1
	iHighFlamm = 0
	for j in xrange(GC.getNumBuildingInfos()):
		if CyTeam.isObsoleteBuilding(j) or CyCity.getNumRealBuilding(j) < 1 or isLimitedWonder(j):
			continue
		CvBuilding = GC.getBuildingInfo(j)
		if CvBuilding.getProductionCost() < 1 or CvBuilding.isNukeImmune() or CvBuilding.isAutoBuild():
			continue

		for k in xrange(GC.getNumBuildingInfos()):
			if CyCity.getNumRealBuilding(k) > 0 and CvBuilding.isReplaceBuilding(k):
				break
		else:
			randNum = GAME.getSorenRandNum(iFlammRand, "Buildings destroyed by fire.")
			iFlamm = CvBuilding.getProperties().getValueByProperty(GC.getInfoTypeForString("PROPERTY_FLAMMABILITY"))
			iFlammScore = iFlamm + randNum
			if iFlammScore > iHighFlamm:
				iHighFlamm = iFlammScore
				iBurnBuilding = j

	if iBurnBuilding != -1:
		szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBurnBuilding).getTextKey(), ))
		CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBurnBuilding).getButton(), GC.getInfoTypeForString("COLOR_RED"), CyCity.getX(), CyCity.getY(), True, True)
		CyCity.setNumRealBuilding(iBurnBuilding, 0)


def doMajorFire(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(kTriggeredData.iCityId)
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
			if CyTeam.isObsoleteBuilding(j) or CyCity.getNumRealBuilding(j) < 1 or isLimitedWonder(j):
				continue
			CvBuilding = GC.getBuildingInfo(j)
			if CvBuilding.getProductionCost() < 1 or CvBuilding.isNukeImmune() or CvBuilding.isAutoBuild():
				continue

			for k in xrange(GC.getNumBuildingInfos()):
				if CyCity.getNumRealBuilding(k) > 0 and CvBuilding.isReplaceBuilding(k):
					break
			else:
				randNum = GAME.getSorenRandNum(iFlammRand, "Buildings destroyed by fire.")
				iFlamm = CvBuilding.getProperties().getValueByProperty(GC.getInfoTypeForString("PROPERTY_FLAMMABILITY"))
				iFlammScore = iFlamm + randNum
				if iFlammScore > iHighFlamm:
					iHighFlamm = iFlammScore
					iBurnBuilding = j
		if iBurnBuilding != -1:
			szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBurnBuilding).getTextKey(), ))
			CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBurnBuilding).getButton(), GC.getInfoTypeForString("COLOR_RED"), CyCity.getX(), CyCity.getY(), True, True)
			CyCity.setNumRealBuilding(iBurnBuilding, 0)

def doCatastrophicFire(argsList):
	iEvent = argsList[0]
	kTriggeredData = argsList[1]
	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	CyTeam = GC.getTeam(CyPlayer.getTeam())
	CyCity = CyPlayer.getCity(kTriggeredData.iCityId)
	iProp = GC.getInfoTypeForString("PROPERTY_FLAMMABILITY")
	iFlammStart = CyCity.getProperties().getValueByProperty(iProp)
	iPop = CyCity.getPopulation()
	popScore = iPop * iPop
	iFlammRand = iFlammStart/3 + popScore
	iFlammEnd = iFlammStart/2
	iFlammRange = iFlammStart/5

	iKilledRand = GAME.getSorenRandNum(iFlammEnd, "People killed by fire.")
	iKilledPop = iPop * iKilledRand / (iFlammStart + popScore)
	if iKilledPop == 0:
		szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITYFIRE_NOHIT_CITY", (CyCity.getNameKey() , ))
		CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), CyCity.getX(), CyCity.getY(), True, True)
	if iKilledPop > 0:
		CyCity.changePopulation(-iKilledPop)
		szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITYFIRE_HIT_CITY", (iKilledPop, CyCity.getNameKey() , ))
		CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_RED"), CyCity.getX(), CyCity.getY(), True, True)

	for i in xrange(iFlammRange):
		currFlamm = CyCity.getProperties().getValueByProperty(iProp)
		if currFlamm <= iFlammEnd: break
		iBurnBuilding = -1
		iHighFlamm = 0

		for j in xrange(GC.getNumBuildingInfos()):
			if CyTeam.isObsoleteBuilding(j) or CyCity.getNumRealBuilding(j) < 1 or isLimitedWonder(j):
				continue
			CvBuilding = GC.getBuildingInfo(j)
			if CvBuilding.getProductionCost() < 1 or CvBuilding.isNukeImmune() or CvBuilding.isAutoBuild():
				continue

			for k in xrange(GC.getNumBuildingInfos()):
				if CyCity.getNumRealBuilding(k) > 0 and CvBuilding.isReplaceBuilding(k):
					break
			else:
				randNum = GAME.getSorenRandNum(iFlammRand, "Buildings destroyed by fire.")
				iFlamm = CvBuilding.getProperties().getValueByProperty(iProp)
				iFlammScore = iFlamm + randNum
				if iFlammScore > iHighFlamm:
					iHighFlamm = iFlammScore
					iBurnBuilding = j
		if iBurnBuilding != -1:
			szBuffer = TRNSLTR.getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", (GC.getBuildingInfo(iBurnBuilding).getTextKey(),))
			CyInterface().addMessage(kTriggeredData.ePlayer, False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getBuildingInfo(iBurnBuilding).getButton(), GC.getInfoTypeForString("COLOR_RED"), CyCity.getX(), CyCity.getY(), True, True)
			CyCity.setNumRealBuilding(iBurnBuilding, 0)


def getHelpMinorFire(argsList):
  return TRNSLTR.getText("TXT_KEY_EVENT_FIRE_MINOR_1_HELP",())

def getHelpMajorFire(argsList):
  return TRNSLTR.getText("TXT_KEY_EVENT_FIRE_MAJOR_1_HELP",())

def getHelpCatastrophicFire(argsList):
  return TRNSLTR.getText("TXT_KEY_EVENT_FIRE_CATASTROPHIC_1_HELP",())

def canTriggerBlarneyVisit(argsList):
	if not GAME.getGameTurn() % 50:
		return True
	return False

def doGlobalWarming(argsList):
  iEvent = argsList[0]
  kTriggeredData = argsList[1]

  maxRand = GAME.getEstimateEndTurn() * GC.getMap().numPlots()

  iGW = 0
  countIce = 1

  for j in xrange(GC.getMap().numPlots()):
    jPlot = GC.getMap().plotByIndex(j)
    if jPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_ICE"):
      iGW += -5
      countIce += 2
    if jPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_FOREST"):
      iGW += -2
    if jPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_JUNGLE"):
      iGW += -2
    if jPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_SAVANNA"):
      iGW += -1
    if jPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_VOLCANO_ACTIVE"):
      iGW += -5
    if jPlot.getTerrainType() == GC.getInfoTypeForString("TERRAIN_TUNDRA"):
      iGW += -5
      countIce += 2
    if jPlot.getTerrainType() == GC.getInfoTypeForString("TERRAIN_ICE"):
      iGW += -10
      countIce += 2
    if jPlot.getTerrainType() == GC.getInfoTypeForString("TERRAIN_TAIGA"):
      iGW += -4
      countIce += 1
    if jPlot.getTerrainType() == GC.getInfoTypeForString("TERRAIN_DUNES"):
      iGW += -1
    if jPlot.getTerrainType() == GC.getInfoTypeForString("TERRAIN_SALT_FLATS"):
      iGW += -1
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_FARM"):
      iGW += 2
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_PASTURE"):
      iGW += 8
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_COTTAGE"):
      iGW += 1
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_HAMLET"):
      iGW += 2
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_VILLAGE"):
      iGW += 4
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_TOWN"):
      iGW += 8
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_EXTRACTION_FACILITY"):
      iGW += 8
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_WORKSHOP"):
      iGW += 4
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_FACTORY"):
      iGW += 8
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_MANUFACTURING_COMPLEX"):
      iGW += 16
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_MINE"):
      iGW += 1
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_SHAFT_MINE"):
      iGW += 2
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_MODERN_MINE"):
      iGW += 4
    if jPlot.getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_HYDROCARBON_WELL"):
      iGW += 8
    if jPlot.isCity():
      icityGW = jPlot.getPlotCity()
      iGW += (icityGW.getNumBuilding(GC.getInfoTypeForString("BUILDING_POLLUTION_GLOBALWARMING1")) * 100)
      iGW += (icityGW.getNumBuilding(GC.getInfoTypeForString("BUILDING_POLLUTION_GLOBALWARMING2")) * 10000)
      iGW += (icityGW.getNumBuilding(GC.getInfoTypeForString("BUILDING_POLLUTION_GLOBALWARMING3")) * 1000000)

  iIce = (2* GC.getMap().numPlots()) / countIce

  for i in xrange(GC.getMap().numPlots()):
    iPlot = GC.getMap().plotByIndex(i)
    randGW = GAME.getSorenRandNum(maxRand, "Global Warming index of plot affected")
    if (iGW > randGW) and ((iPlot.getFeatureType()) == GC.getInfoTypeForString("FEATURE_ICE")):
        iPlot.setFeatureType(FeatureTypes.NO_FEATURE,-1)
    elif iGW > randGW:
      if (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_ICE"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_TUNDRA"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_TUNDRA"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_TAIGA"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_TAIGA"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_MUDDY"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_MUDDY"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_LUSH"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_LUSH"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_GRASSLAND"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_GRASSLAND"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_PLAINS"), True, True)
        if iPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_SWAMP") or iPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_PEAT_BOG"):
          iPlot.setFeatureType(FeatureTypes.NO_FEATURE,-1)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_PLAINS") or (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_BARREN") or (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_ROCKY"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_SCRUB"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_SCRUB"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_DESERT"), True, True)
        if iPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_FOREST") or iPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_JUNGLE") or iPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_BAMBOO") or iPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_SAVANNA") or iPlot.getFeatureType() == GC.getInfoTypeForString("FEATURE_VERY_TALL_GRASS"):
          iPlot.setFeatureType(FeatureTypes.NO_FEATURE,-1)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_DESERT"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_DUNES"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_COAST_POLAR"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_COAST"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_COAST"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_COAST"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL") and iIce < 100 and not iPlot.isLake():
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_OCEAN"), True, True)
        iDX = iPlot.getX()
        iDY = iPlot.getY()
        for iDX in xrange(-1, 2):
          for iDY in xrange(-1, 2):
            if not iPlot.isNone():
              if (iDX != 0 or iDY != 0):
                iDPlot = CyMap().plot(iDX, iDY)
                if iDPlot.getPlotType() == GC.getInfoTypeForString("PLOT_LAND") and not iDPlot.isCity():
                  randFlood = GAME.getSorenRandNum(100, "Global Warming flooding chance")
                  if iIce > randFlood:
                    iDPlot.setFeatureType(FeatureTypes.NO_FEATURE,-1)
                    iDPlot.setImprovementType(-1)
                    iDPlot.setBonusType(-1)
                    iDPlot.setImprovementType(-1)
                    iDPlot.setRouteType(-1)
                    iDPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_COAST"), True, True)
                    iDPlot.setPlotType(PlotTypes.PLOT_OCEAN, True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_OCEAN_POLAR"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_OCEAN"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_OCEAN"):
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_OCEAN_TROPICAL"), True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL") and iIce > 100 and not iPlot.isLake():
        iPlot.setFeatureType(FeatureTypes.NO_FEATURE,-1)
        iPlot.setImprovementType(-1)
        iPlot.setBonusType(-1)
        iPlot.setImprovementType(-1)
        iPlot.setRouteType(-1)
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_SALT_FLATS"), True, True)
        iPlot.setPlotType(PlotTypes.PLOT_LAND, True, True)
      elif (iPlot.getTerrainType()) == GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL") and iPlot.isLake():
        iPlot.setFeatureType(FeatureTypes.NO_FEATURE,-1)
        iPlot.setImprovementType(-1)
        iPlot.setBonusType(-1)
        iPlot.setImprovementType(-1)
        iPlot.setRouteType(-1)
        iPlot.setTerrainType(GC.getInfoTypeForString("TERRAIN_MUDDY"), True, True)
        iPlot.setPlotType(PlotTypes.PLOT_LAND, True, True)

def getHelpGlobalWarming(argsList):
	return TRNSLTR.getText("TXT_KEY_EVENT_GLOBAL_WARMING_1_HELP",())

######## TORNADO ###########
def canDoTornado(argsList):
	EventTriggeredData = argsList[0]

	CyPlot = GC.getMap().plot(EventTriggeredData.iPlotX, EventTriggeredData.iPlotY)
	if CyPlot.isCity():
		return 0

	if not CyPlot.canHaveFeature(GC.getInfoTypeForString('FEATURE_TORNADO')):
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

	if CyPlot.getFeatureType() == -1:
		CyPlot.setFeatureType(GC.getInfoTypeForString('FEATURE_TORNADO'), 0)

	iUnits = CyPlot.getNumUnits()
	for iUnit in xrange(iUnits):
		CyPlot.getUnit(iUnit).setImmobileTimer(1)

######## Native Good 1 -- lost resources ###########
def canApplyNativegood1(argsList):
  return False
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)
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
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)
  if (player.getNativeOpinion()>=5):
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
	iEvent = argsList[0]
	kTriggeredData = argsList[1]

	CyPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	iRelationship = CyPlayer.getNativeRelationship()
	iRelationship += 40
	CyPlayer.setNativeRelationship(iRelationship)

	CyCity = CyPlayer.getCapitalCity()
	if CyCity.isNone:
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

	CyCity, i = CyPlayer.firstCity(False)
	while CyCity:
		x = CyCity.getX()
		y = CyCity.getY()
		for j in xrange(3):
			CyPlayer.initUnit(eBestUnit, x, y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
		CyCity, i = GC.getPlayer(self.iActiveLeader).nextCity(i, False)

######## Native Good 3 -- gold ###########

def canApplyNativegood3(argsList):
  return False
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)

  if (player.getNativeOpinion()>=4):
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
        iEvent = argsList[0]
        kTriggeredData = argsList[1]

        szHelp = TRNSLTR.getText("TXT_KEY_EVENT_NATIVEGOOD3_1HELP",())

        return szHelp

def ApplyNativegood3(argsList):
        iEvent = argsList[0]
        kTriggeredData = argsList[1]

        player = GC.getPlayer(kTriggeredData.ePlayer)
        player.changeGold(50 + GAME.getSorenRandNum(50, "random native gold"))

######## Native Good 4 -- Ancient Tech ###########

def canApplyNativegood4(argsList):
  return False
  kTriggeredData = argsList[0]

  player = GC.getPlayer(kTriggeredData.ePlayer)

  if (player.getCurrentEra()<=3):
          return 0

  if (player.getNativeOpinion()>=5):
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
        iEvent = argsList[0]
        kTriggeredData = argsList[1]

        szHelp = TRNSLTR.getText("TXT_KEY_EVENT_NATIVEGOOD4_HELP",())

        return szHelp

def ApplyNativegood4(argsList):
        iEvent = argsList[0]
        kTriggeredData = argsList[1]

        player = GC.getPlayer(kTriggeredData.ePlayer)
        player.grantOldTech()

### Missions ####

def doEventLawyer(argsList):
        pUnit = argsList[0]

        if pUnit == None:
                return # False call

	pPlayer = GC.getPlayer( pUnit.getOwner( ) )
	iPlayer = pPlayer.getID( )
	pCity = pUnit.plot().getPlotCity()

        if pCity == None:
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
			if pCity.isHasBuilding( iBuildingLoop):
				pBuilding = GC.getBuildingInfo( iBuildingLoop )
				iRequiredCorporation = pBuilding.getFoundsCorporation( )
				for iCorpLoop in xrange(GC.getNumCorporationInfos()):
					if iRequiredCorporation == iCorpLoop:
						pCity.setNumRealBuilding ( iBuildingLoop,0 )

		# Loop through all corporations, remove them from the city
		for iCorpLoop in xrange(GC.getNumCorporationInfos()):
			pCity.setHasCorporation(iCorpLoop, 0, 0, 0)

		# Litigation succeeds
		CyInterface().addMessage(iPlayer,False,25,CyTranslator().getText("TXT_KEY_MSG_LITIGATION",(pCity.getName(),)),"AS2D_BUILD_BANK",InterfaceMessageTypes.MESSAGE_TYPE_INFO,pUnit.getButton(),ColorTypes(8),pCity.getX(),pCity.getY(),True,True)

	# Litigation fails
	else:
		CyInterface().addMessage(iPlayer,False,25,CyTranslator().getText("TXT_KEY_MSG_LITIGATION_FAIL",(pCity.getName(),)),"AS2D_SABOTAGE",InterfaceMessageTypes.MESSAGE_TYPE_INFO,pUnit.getButton(),ColorTypes(7),pCity.getX(),pCity.getY(),True,True)

## Captives and Slavery mod python ##

def doRemoveWVSlavery(argsList):
	CyUnit = argsList[0]

	if not CyUnit: return # False call

	iPlayer = CyUnit.getOwner()
	CyPlayer = GC.getPlayer(iPlayer)

	if not CyPlayer.isAlive():
		return

	iWVSlavery = GC.getInfoTypeForString("BUILDING_WV_SLAVERY")

	if iWVSlavery > -1:

		iSlaveMarket = GC.getInfoTypeForString("BUILDING_SLAVE_MARKET")
		aiSlaveBuildings = [
			GC.getInfoTypeForString("BUILDING_SLAVERY"),
			GC.getInfoTypeForString("BUILDING_SLAVERY_BAD_ZORO_I"),
			GC.getInfoTypeForString("BUILDING_SLAVERY_BAD_ZORO_II"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_MILITARY_SUPPORT"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_ENTERTAINMENT"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_FOOD"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_PRODUCTION"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_COMMERCE"),
			GC.getInfoTypeForString("BUILDING_SLAVE_COMPOUND_SANITATION"),
		]
		iSlaveSettled = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE")
		iSlaveFood = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD")
		iSlaveProd = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION")
		iSlaveCom = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE")
		iSlaveHealth = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH")
		iSlaveEntertain = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT")
		iSlaveTutor = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR")
		iSlaveMilitary = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY")

		iUnitCaptiveSlave = GC.getInfoTypeForString("UNIT_FREED_SLAVE")
		iUnitImmigrant = GC.getInfoTypeForString("UNIT_CAPTIVE_IMMIGRANT")
		iUnitEntertain = GC.getInfoTypeForString("UNIT_STORY_TELLER")
		iUnitMerCaravan = GC.getInfoTypeForString("UNIT_EARLY_MERCHANT_C2C")
		iUnitHealth = GC.getInfoTypeForString("UNIT_HEALER")

		bMessage = iPlayer == GAME.getActivePlayer()
		if bMessage:
			msg = "Slavery worldview eradicated"
			CvUtil.sendMessage(msg, iPlayer, 16, CyUnit.getButton(), ColorTypes(8), CyUnit.getX(), CyUnit.getY(), True, True, 0, "AS2D_DISCOVERBONUS")

		iCost = CyPlayer.getBuildingProductionNeeded(iSlaveMarket)
		iSum = 0
		CyCity, i = CyPlayer.firstCity(False)
		while CyCity:
			sCityName = CyCity.getName()
			iCityX = CyCity.getX()
			iCityY = CyCity.getY()
			# Remove the main slavery building
			if CyCity.getNumActiveBuilding(iWVSlavery) > 0:
				CyCity.setNumRealBuilding(iWVSlavery, 0)

			# Sell the Slave market if one exists
			if CyCity.getNumActiveBuilding(iSlaveMarket) > 0:

				CyCity.setNumRealBuilding(iSlaveMarket, 0)

				iSum += iCost

				if bMessage:
					msg = TRNSLTR.getText("TXT_KEY_MSG_SLAVE_MARKET_SOLD", (sCityName,))
					CvUtil.sendMessage(msg, iPlayer, 16, GC.getBuildingInfo(iSlaveMarket).getButton(), ColorTypes(8), iCityX, iCityY, True, True, 0, "AS2D_BUILD_BANK")

			# Remove all other Slavery Buildings if they exist
			for ibuilding in aiSlaveBuildings:
				if CyCity.getNumActiveBuilding(ibuilding) > 0:
					CyCity.setNumRealBuilding(ibuilding, 0)

			# Free the Settled Slaves.
			iCountSettled = CyCity.getFreeSpecialistCount(iSlaveSettled)
			iCountFood = CyCity.getFreeSpecialistCount(iSlaveFood)
			iCountProd = CyCity.getFreeSpecialistCount(iSlaveProd)
			iCountCom = CyCity.getFreeSpecialistCount(iSlaveCom)
			iCountHealth = CyCity.getFreeSpecialistCount(iSlaveHealth)
			iCountEntertain = CyCity.getFreeSpecialistCount(iSlaveEntertain)
			iCountTutor = CyCity.getFreeSpecialistCount(iSlaveTutor)
			iCountMilitary = CyCity.getFreeSpecialistCount(iSlaveMilitary)

			## Process those that can become population or immagrants
			##	where 3 slaves = 1 pop or immigrant
			##	and can only increase the city pop to 7
			iCount = iCountSettled + iCountFood + iCountCom + iCountTutor + iCountMilitary
			iCountNewPop = int(iCount/3)
			iCount = iCount - 3*iCountNewPop

			if iCount > 0:
				for j in xrange(iCount):
					CyPlayer.initUnit(iUnitCaptiveSlave, iCityX, iCityY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
				if bMessage:
					msg = TRNSLTR.getText("TXT_MESSAGE_FREED_SLAVES_AS_FREED_SLAVES", (sCityName, iCount))
					CvUtil.sendMessage(msg, iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)

			if iCountNewPop > 0:
				iCountImmigrants = iCountNewPop
				iCityPop = CyCity.getPopulation()
				if iCityPop < 7:
					## fill up the local pop and left overs become immigrants
					iMaxToAddPop = 7 - iCityPop
					if iMaxToAddPop > iCountImmigrants:
						CyCity.changePopulation(iCountImmigrants)
						if bMessage:
							msg = TRNSLTR.getText("TXT_MESSAGE_FREED_SLAVES_JOINED_CITY_POPULATION", (iCountImmigrants*3, sCityName, iCountImmigrants))
							CvUtil.sendMessage(msg, iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)
						iCountImmigrants = 0
					else:
						CyCity.changePopulation(iMaxToAddPop)
						if bMessage:
							msg = TRNSLTR.getText("TXT_MESSAGE_FREED_SLAVES_JOINED_CITY_POPULATION", (iMaxToAddPop*3, sCityName, iMaxToAddPop))
							CvUtil.sendMessage(msg, iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)
						iCountImmigrants = iCountImmigrants - iMaxToAddPop
				if iCountImmigrants > 0:
					for j in xrange(iCountImmigrants):
						CyPlayer.initUnit(iUnitImmigrant, iCityX, iCityY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
					if bMessage:
						msg = TRNSLTR.getText("TXT_MESSAGE_FREED_SLAVES_AS_IMMIGRANTS", (iCountImmigrants*3, sCityName, iCountImmigrants))
						CvUtil.sendMessage(msg, iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)

			## Now remove those slaves
			if  iCountSettled > 0:
				CyCity.changeFreeSpecialistCount(iSlaveSettled, -iCountSettled)
			if  iCountFood > 0:
				CyCity.changeFreeSpecialistCount(iSlaveFood, -iCountFood)
			if  iCountCom > 0:
				CyCity.changeFreeSpecialistCount(iSlaveCom, -iCountCom)
			if  iCountTutor > 0:
				CyCity.changeFreeSpecialistCount(iSlaveTutor, -iCountTutor)
			if  iCountMilitary > 0:
				CyCity.changeFreeSpecialistCount(iSlaveMilitary, -iCountMilitary)

			## Now convert the other slaves
			if iCountProd > 0:
				for j in xrange(iCountProd):
					CyPlayer.initUnit(iUnitMerCaravan, iCityX, iCityY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
					CyCity.changeFreeSpecialistCount(iSlaveProd,-1)
				if bMessage:
					msg = TRNSLTR.getText("TXT_MESSAGE_FREED_SLAVES_AS_CARAVANS", (sCityName, iCountProd, "Early Merchants"))
					CvUtil.sendMessage(msg, iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)

			if iCountHealth > 0:
				for j in xrange(iCountHealth):
					CyPlayer.initUnit(iUnitHealth, iCityX, iCityY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
					CyCity.changeFreeSpecialistCount(iSlaveHealth,-1)
				if bMessage:
					msg = TRNSLTR.getText("TXT_MESSAGE_FREED_SLAVES_AS_HEALERS", (sCityName, iCountHealth, "Healers"))
					CvUtil.sendMessage(msg, iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)

			if iCountEntertain > 0:
				for j in xrange(iCountEntertain):
					CyPlayer.initUnit(iUnitEntertain, iCityX, iCityY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
					CyCity.changeFreeSpecialistCount(iSlaveEntertain,-1)
				if bMessage:
					msg = TRNSLTR.getText("TXT_MESSAGE_FREED_SLAVES_AS_STORY_TELLERS", (sCityName, iCountEntertain, "Story Tellers"))
					CvUtil.sendMessage(msg, iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iCityX, iCityY, True, True)

			CyCity, i = CyPlayer.nextCity(i, False)

		if iSum > 0:
			CyPlayer.changeGold(int(iSum * 0.2))


def doRemoveWVCannibalism(argsList):
	CyUnit = argsList[0]

	if CyUnit == None:
		print "[INFO] doRemoveWVCannibalism(CyUnit) where CyUnit is None"
		return # False call

	iType = GC.getInfoTypeForString("BUILDING_WV_CANNIBALISM")
	if iType > -1:
		iPlayer = CyUnit.getOwner()
		CyPlayer = GC.getPlayer(iPlayer)
		CyCity = CyPlayer.getCapitalCity()
		if CyCity.isNone():
			print "[INFO] doRemoveWVCannibalism(args) happened for a player with no cities"
		else:
			iType0 = GC.getInfoTypeForString("BUILDING_CANNIBALISM")
			iType1 = GC.getInfoTypeForString("BUILDING_CANNIBALISM_BAD_I")
			iType2 = GC.getInfoTypeForString("BUILDING_CANNIBALISM_BAD_II")
			iType3 = GC.getInfoTypeForString("BUILDING_CANNIBALISM_BAD_III")
			CyCity, i = CyPlayer.firstCity(False)
			while CyCity:
				CyCity.setNumRealBuilding(iType, 0)
				if iType0 > -1:
					CyCity.setNumRealBuilding(iType0, 0)
				if iType1 > -1:
					CyCity.setNumRealBuilding(iType1, 0)
				if iType2 > -1:
					CyCity.setNumRealBuilding(iType2, 0)
				if iType3 > -1:
					CyCity.setNumRealBuilding(iType3, 0)
				CyCity, i = CyPlayer.nextCity(i, False)

			if iPlayer == GC.getGame().getActivePlayer():
				CvUtil.sendImmediateMessage(TRNSLTR.getText("TXT_KEY_MSG_NO_CANNIBALISM", ()))
				CyAudioGame().Play2DSound("AS2D_DISCOVERBONUS")

def doRemoveWVHumanSacrifice(argsList):
	CyUnit = argsList[0]

	if CyUnit == None:
		return # False call

	iWVSacrifice = GC.getInfoTypeForString("BUILDING_WV_HUMAN_SACRIFICE")
	if iWVSacrifice > -1:
		iAltar = GC.getInfoTypeForString("BUILDING_HUMAN_SACRIFICE_ALTAR")
		iToken = GC.getInfoTypeForString("BUILDING_HUMAN_SACRIFICE")
		CyPlayer = GC.getPlayer(CyUnit.getOwner())

		CyCity, i = CyPlayer.firstCity(False)
		while CyCity:
			# Remove the main worldview building
			if CyCity.getNumActiveBuilding(iWVSacrifice) > 0:
				CyCity.setNumRealBuilding(iWVSacrifice, 0)
				CyAudioGame().Play2DSound("AS2D_DISCOVERBONUS")

				CyInterface().addMessage(CyPlayer.getID(),False,25,TRNSLTR.getText("TXT_KEY_MSG_NO_HUMAN_SACRIFICE",(CyCity.getName(),)),"AS2D_BUILD_BANK",InterfaceMessageTypes.MESSAGE_TYPE_INFO,pUnit.getButton(),ColorTypes(8),CyCity.getX(),CyCity.getY(),True,True)

			# Remove the worldview token building
			CyCity.setNumRealBuilding(iToken, 0)

			# Remove the human sacrifice altar
			if CyCity.getNumActiveBuilding(iAltar) > 0:
				CyCity.setNumRealBuilding(iAltar, 0)

			CyCity, i = CyPlayer.nextCity(i, False)

def getNumNonSpecialistSlaves(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesFood(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesProduction(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesCommerce(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesHealth(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesEntertainment(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesTutor(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR"))

	return (iNormalSlaves - iSpecialSlaves)

def getNumNonSpecialistSlavesMilitary(argsList):
	# Returns the number of non specialist slave specialists more than the number of specialist slave specialists
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	# count normal slaves
	iNormalSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"))
	# count special slaves
	iSpecialSlaves = 2 * pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY"))

	return (iNormalSlaves - iSpecialSlaves)

def hasSufficientPopulation(argsList):
	pPlot = argsList[0]

	if not pPlot:
		return "Non-specialist slaves" # call for help text

	pCity = pPlot.getPlotCity()

	if pCity == None:
		return False # call when not on city

	iNumSlaves = pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR")) + pCity.getFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY"))

	iPopulation = pCity.getPopulation()

	return (iNumSlaves < (10 * iPopulation))

def doAddSettledSlave(argsList):
	pUnit = argsList[0]
	print "caveman2Cosmos - doAddSettledSlave called."

	if pUnit == None:
		print "caveman2Cosmos - doAddSettledSlave called - not a valid unit."
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		print "caveman2Cosmos - doAddSettledSlave called - not a valid city."
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE"),1)

def doAddSettledSlaveFood(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD"),1)

def doAddSettledSlaveProduction(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION"),1)

def doAddSettledSlaveCommerce(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE"),1)

def doAddSettledSlaveHealth(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH"),1)

def doAddSettledSlaveEntertainment(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT"),1)

def doAddSettledSlaveTutor(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR"),1)

def doAddSettledSlaveMilitary(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	pCity.changeFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY"),1)


def doSacrificeCaptive(argsList):
	pUnit = argsList[0]

	if pUnit == None:
		return # False call

	pPlayer = GC.getPlayer( pUnit.getOwner( ) )
	iPlayer = pPlayer.getID( )
	pCity = pUnit.plot().getPlotCity()

	if pCity == None:
		return # False call

	iGoldenAgeLength = 0
	iGoldenAgeLengthModifier = 1

	iUnitType = pUnit.getUnitType()
	if iUnitType == GC.getInfoTypeForString("UNIT_CAPTIVE_NEANDERTHAL"):
		iGoldenAgeLengthModifier = 0
	elif iUnitType == GC.getInfoTypeForString("UNIT_CAPTIVE_MILITARY"):
		iGoldenAgeLengthModifier = 1
	elif iUnitType == GC.getInfoTypeForString("UNIT_CAPTIVE_CIVILIAN"):
		iGoldenAgeLengthModifier = 2
	else:
		return # bad call

	if  pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_SACRIFICIAL_ALTAR")) > 0:
		iGoldenAgeLength += 1
	elif pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_HUMAN_SACRIFICE_ALTAR")) > 0:
		iGoldenAgeLength += 2

	iDruid = GC.getInfoTypeForString("RELIGION_DRUIDIC_TRADITIONS")
	iMeasoamerican = GC.getInfoTypeForString("RELIGION_NAGHUALISM")
	iStateReligion = pUnit.getStateReligion()

	if iStateReligion == iDruid :
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_SHRINE")) > 0:
			iGoldenAgeLengthModifier = iGoldenAgeLengthModifier + 1
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_CATHEDRAL2")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 3
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_CATHEDRAL")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 2
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_MONASTERY")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 1
	else:
		if pCity.isHasReligion(iDruid):
			if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_SHRINE")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_CATHEDRAL2")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_CATHEDRAL")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_DRUID_MONASTERY")) > 0:
				iGoldenAgeLength = iGoldenAgeLength + 1

	if iStateReligion == iMeasoamerican :
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_SHRINE")) > 0:
			iGoldenAgeLengthModifier = iGoldenAgeLengthModifier + 1
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_CATHEDRAL")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 3
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_MONASTERY")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 2
		if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_TEMPLE")) > 0:
			iGoldenAgeLength = iGoldenAgeLength + 1
	else:
		if pCity.isHasReligion(iMeasoamerican):
			if pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_SHRINE")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_CATHEDRAL")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_MONASTERY")) > 0 or pCity.getNumActiveBuilding(GC.getInfoTypeForString("BUILDING_MESOAMERICAN_TEMPLE")) > 0:
				iGoldenAgeLength = iGoldenAgeLength + 1

	iTotalGoldenAgeLength = iGoldenAgeLength * iGoldenAgeLengthModifier
	pPlayer.changeGoldenAgeTurns(iTotalGoldenAgeLength)

## End Captives and Slavery mod python ##


## Start Spread Resources mod python ##
def canBuildHorseBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isFlatlands() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() not in (GC.getInfoTypeForString("FEATURE_FLOOD_PLAINS"), GC.getInfoTypeForString("FEATURE_SAVANNA"), -1): return 0
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS")): return 0
	return 1

def doBuildHorseBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_HORSE"))

def canBuildHorseBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isFlatlands() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() in (GC.getInfoTypeForString("FEATURE_SWAMP"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")): return 0
	if pPlot.getTerrainType() in (
		GC.getInfoTypeForString("TERRAIN_SALT_FLATS"),	GC.getInfoTypeForString("TERRAIN_DUNES"),	GC.getInfoTypeForString("TERRAIN_DESERT"),
		GC.getInfoTypeForString("TERRAIN_TAIGA"),		GC.getInfoTypeForString("TERRAIN_ICE"),		GC.getInfoTypeForString("TERRAIN_TUNDRA"),
		GC.getInfoTypeForString("TERRAIN_PERMAFROST"),	GC.getInfoTypeForString("TERRAIN_JAGGED"),	GC.getInfoTypeForString("TERRAIN_BADLAND"),
		GC.getInfoTypeForString("TERRAIN_BARREN"), 		GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildHorseBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_HORSE"))

def canBuildCowBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isFlatlands() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() not in (GC.getInfoTypeForString("FEATURE_FLOOD_PLAINS"), GC.getInfoTypeForString("FEATURE_SAVANNA"), -1): return 0
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS")): return 0
	return 1

def doBuildCowBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_COW"))

def canBuildCowBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isFlatlands() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() in (GC.getInfoTypeForString("FEATURE_SWAMP"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")): return 0
	if pPlot.getTerrainType() in (
		GC.getInfoTypeForString("TERRAIN_SALT_FLATS"),	GC.getInfoTypeForString("TERRAIN_DUNES"),		GC.getInfoTypeForString("TERRAIN_DESERT"),
		GC.getInfoTypeForString("TERRAIN_TAIGA"),		GC.getInfoTypeForString("TERRAIN_ICE"),			GC.getInfoTypeForString("TERRAIN_TUNDRA"),
		GC.getInfoTypeForString("TERRAIN_PERMAFROST"),	GC.getInfoTypeForString("TERRAIN_JAGGED"),	GC.getInfoTypeForString("TERRAIN_BADLAND"),
		GC.getInfoTypeForString("TERRAIN_BARREN"), 		GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildCowBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return
	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_COW"))

def canBuildSheepBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isHills() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() not in (GC.getInfoTypeForString("FEATURE_FLOOD_PLAINS"), GC.getInfoTypeForString("FEATURE_SAVANNA"), -1): return 0
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS")): return 0
	return 1

def doBuildSheepBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_SHEEP"))

def canBuildSheepBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if not pPlot.isHills() or pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() in (GC.getInfoTypeForString("FEATURE_SWAMP"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")): return 0
	if pPlot.getTerrainType() in (
		GC.getInfoTypeForString("TERRAIN_SALT_FLATS"),	GC.getInfoTypeForString("TERRAIN_DUNES"),		GC.getInfoTypeForString("TERRAIN_DESERT"),
		GC.getInfoTypeForString("TERRAIN_TAIGA"),		GC.getInfoTypeForString("TERRAIN_ICE"),			GC.getInfoTypeForString("TERRAIN_TUNDRA"),
		GC.getInfoTypeForString("TERRAIN_PERMAFROST"),	GC.getInfoTypeForString("TERRAIN_JAGGED"),	GC.getInfoTypeForString("TERRAIN_BADLAND"),
		GC.getInfoTypeForString("TERRAIN_BARREN"), 		GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildSheepBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_SHEEP"))

def canBuildDonkeyBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() not in (GC.getInfoTypeForString("FEATURE_FLOOD_PLAINS"), GC.getInfoTypeForString("FEATURE_SAVANNA"), -1): return 0
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS"), GC.getInfoTypeForString("TERRAIN_SCRUB")): return 0
	return 1

def doBuildDonkeyBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_DONKEY"))

def canBuildDonkeyBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() in (GC.getInfoTypeForString("FEATURE_SWAMP"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")): return 0
	if pPlot.getTerrainType() in (
		GC.getInfoTypeForString("TERRAIN_SALT_FLATS"),	GC.getInfoTypeForString("TERRAIN_DUNES"),		GC.getInfoTypeForString("TERRAIN_DESERT"),
		GC.getInfoTypeForString("TERRAIN_TAIGA"),		GC.getInfoTypeForString("TERRAIN_ICE"),			GC.getInfoTypeForString("TERRAIN_TUNDRA"),
		GC.getInfoTypeForString("TERRAIN_PERMAFROST"),	GC.getInfoTypeForString("TERRAIN_JAGGED"),	GC.getInfoTypeForString("TERRAIN_BADLAND"),
		GC.getInfoTypeForString("TERRAIN_BARREN"), 		GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildDonkeyBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_DONKEY"))

def canBuildCamelBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_DUNES"), GC.getInfoTypeForString("TERRAIN_DESERT"), GC.getInfoTypeForString("TERRAIN_SCRUB")): return 0
	return 1

def doBuildCamelBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_CAMEL"))

def canBuildCamelBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getTerrainType() not in (GC.getInfoTypeForString("TERRAIN_DUNES"), GC.getInfoTypeForString("TERRAIN_DESERT"), GC.getInfoTypeForString("TERRAIN_SCRUB")): return 0
	return 1

def doBuildCamelBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_CAMEL"))

def canBuildLlamaBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getTerrainType() not in (
		GC.getInfoTypeForString("TERRAIN_BARREN"),	GC.getInfoTypeForString("TERRAIN_DESERT"),	GC.getInfoTypeForString("TERRAIN_SCRUB"),
		GC.getInfoTypeForString("TERRAIN_ROCKEY"),	GC.getInfoTypeForString("TERRAIN_BADLAND")
		): return 0
	return 1

def doBuildLlamaBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_LLAMA"))

def canBuildLlamaBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getTerrainType() not in (
		GC.getInfoTypeForString("TERRAIN_BARREN"),	GC.getInfoTypeForString("TERRAIN_DESERT"),	GC.getInfoTypeForString("TERRAIN_SCRUB"),
		GC.getInfoTypeForString("TERRAIN_ROCKEY"),	GC.getInfoTypeForString("TERRAIN_BADLAND")
		): return 0
	return 1

def doBuildLlamaBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_LLAMA"))

def canBuildPigBonus(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() not in (
		GC.getInfoTypeForString("FEATURE_FOREST"), GC.getInfoTypeForString("FEATURE_FOREST_ANCIENT"),
		GC.getInfoTypeForString("FEATURE_JUNGLE"), GC.getInfoTypeForString("FEATURE_FLOOD_PLAINS")
		): return 0
	if pPlot.getTerrainType() not in (
		GC.getInfoTypeForString("TERRAIN_SCRUB"), GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS"),
		GC.getInfoTypeForString("TERRAIN_LUSH"), GC.getInfoTypeForString("TERRAIN_MUDDY"), GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildPigBonus(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(-1)
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_PIG"))

def canBuildPigBonusAndPasture(argsList):
	pGoToPlot = CyInterface().getGotoPlot()
	if pGoToPlot.getX() > -1:
		pPlot = pGoToPlot
	elif argsList[0]:
		pPlot = argsList[0]
	else: return 0
	# Check if plot has a city or any map bonus.
	if pPlot.isCity() or pPlot.getBonusType(-1) > -1: return 0
	# Check if Feature or Terrain makes invalid.
	if pPlot.getFeatureType() in (GC.getInfoTypeForString("FEATURE_MANGROVE"), GC.getInfoTypeForString("FEATURE_PEAT_BOG")): return 0
	if pPlot.getTerrainType() not in (
		GC.getInfoTypeForString("TERRAIN_SCRUB"), GC.getInfoTypeForString("TERRAIN_GRASSLAND"), GC.getInfoTypeForString("TERRAIN_PLAINS"),
		GC.getInfoTypeForString("TERRAIN_LUSH"), GC.getInfoTypeForString("TERRAIN_MUDDY"), GC.getInfoTypeForString("TERRAIN_MARSH")
		): return 0
	return 1

def doBuildPigBonusAndPasture(argsList):
	pPlot = argsList[0].plot()

	if not pPlot: return

	pPlot.setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_PASTURE"))
	pPlot.setBonusType(GC.getInfoTypeForString("BONUS_PIG"))

## End Spread Resources mod python ##


######## Civil War ########
def canTriggerCivilWar(argsList):
	kTriggeredData = argsList[0]
	player = GC.getPlayer(kTriggeredData.ePlayer)
	pCity = player.getCity(kTriggeredData.iCityId)

	if GC.getMAX_PC_PLAYERS() == GAME.countCivPlayersAlive():
		return False
	if player.getNumCities() <= 1:
		return False
	if pCity.isCapital():
		return False

	return False ### Disabled!!!


def applyCivilWar(argsList):
	kTriggeredData = argsList[1]
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

	pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
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
	pPlayer = GC.getPlayer(kTriggeredData.ePlayer)
	pCity = pPlayer.getCity(kTriggeredData.iCityId)
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
	iMaxTech = GC.getNumTechInfos()
	for counter in xrange(iMaxTech):
		if (pTriggerTeam.isHasTech(counter) == True) and (pNewTeam.isHasTech(counter) == False):
			pNewTeam.setHasTech(counter, True, iNewID, False, False)

	# Hand over cities
	iX = pCity.getX()
	iY = pCity.getY()
	#CyInterface().addMessage(0,False,15,str(kTriggeredData.iCityId),'',0,'Art/Interface/Buttons/General/happy_person.dds',ColorTypes(44), iX, iY, True,True)
	pNewPlayer.acquireCity(pCity, False, False)

	# Hand over units
	pPlot = CyMap().plot(iX, iY)
	iMaxNumUnits = pPlot.getNumUnits()
	for iUnits in xrange(iMaxNumUnits, -1, -1):
		pUnit = pPlot.getUnit(iUnits)
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
	if GAME.isOption(GameOptionTypes.GAMEOPTION_WITHOUT_WARNING): return True
	return False
