# Events to support Revolution mod
#
# by jdog5000
# Version 1.5

from CvPythonExtensions import *
import CvUtil
import Popup as PyPopup
import math
# --------- Revolution mod -------------
import RevDefs
import RevData
import RevUtils


# globals
GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

RevOpt = None

# Stores player id's human has reject assimilation overtures from
noAssimilateList = []

revCultureModifier = 1.0
cityLostModifier = 1.0
cityAcquiredModifier = 1.0
acquiredTurns = 10

endWarsOnDeath = True
allowAssimilation = True
bSmallRevolts = False

LOG_DEBUG = True
centerPopups = False
RevOpt = None
customEM = None


########################## Event Handling ##########################################################

def init(newCustomEM, RevOptHandle):

	global revCultureModifier, cityLostModifier, cityAcquiredModifier, acquiredTurns, LOG_DEBUG, centerPopups, RevOpt, customEM
	global endWarsOnDeath, allowAssimilation, bSmallRevolts, MAX_PC_PLAYERS, MAX_PC_TEAMS

	MAX_PC_PLAYERS = GC.getMAX_PC_PLAYERS()
	MAX_PC_TEAMS = GC.getMAX_PC_TEAMS()
	RevOpt = RevOptHandle
	customEM = newCustomEM

	print "Initializing RevEvents"

	LOG_DEBUG = RevOpt.isRevDebugMode()

	# Config settings
	revCultureModifier = RevOpt.getRevCultureModifier()
	cityLostModifier = RevOpt.getCityLostModifier()
	cityAcquiredModifier = RevOpt.getCityAcquiredModifier()
	acquiredTurns = RevOpt.getAcquiredTurns()

	# Controls for event handling
	endWarsOnDeath = RevOpt.isEndWarsOnDeath()
	allowAssimilation = RevOpt.isAllowAssimilation()
	bSmallRevolts = RevOpt.isAllowSmallBarbRevs()

	centerPopups = RevOpt.isCenterPopups()

	# Register events
	customEM.addEventHandler( 'EndGameTurn', onEndGameTurn )
	customEM.addEventHandler( 'EndPlayerTurn', onEndPlayerTurn )
	customEM.addEventHandler( "setPlayerAlive", onSetPlayerAlive )
	customEM.addEventHandler( "changeWar", onChangeWar )
	customEM.addEventHandler( "religionFounded", onReligionFounded )

	customEM.addEventHandler( 'cityBuilt', onCityBuilt )
	customEM.addEventHandler( 'cityAcquired', onCityAcquired )
	customEM.addEventHandler( "cityLost", onCityLost )
	customEM.addEventHandler( "buildingBuilt", onBuildingBuilt )

	customEM.setPopupHandler( RevDefs.assimilationPopup, ["assimilationPopup", assimilateHandler, blankHandler] )

	RevUtils.initCivicsList()


def removeEventHandlers():
	print "Removing event handlers from RevEvents"

	customEM.removeEventHandler( 'EndGameTurn', onEndGameTurn )
	customEM.removeEventHandler( 'EndPlayerTurn', onEndPlayerTurn )
	customEM.removeEventHandler( "setPlayerAlive", onSetPlayerAlive )
	customEM.removeEventHandler( "changeWar", onChangeWar )
	customEM.removeEventHandler( "religionFounded", onReligionFounded )

	customEM.removeEventHandler( 'cityBuilt', onCityBuilt )
	customEM.removeEventHandler( 'cityAcquired', onCityAcquired )
	customEM.removeEventHandler( "cityLost", onCityLost )
	customEM.removeEventHandler( "buildingBuilt", onBuildingBuilt )

	customEM.setPopupHandler( RevDefs.assimilationPopup, ["assimilationPopup", blankHandler, blankHandler] )


# Dummy handler to take the second event for popup
def blankHandler(playerID, netUserData, popupReturn): return

########################## Turn-based events ###############################

def onEndGameTurn(argsList):

	## Fix to stop modulo by zero DH 5th July 2012 ##
	if GAME.getGameTurn()%int(max(1,RevUtils.getGameSpeedMod())*10) == 0:
		updateAttitudeExtras()

	removeFloatingRebellions()

	if allowAssimilation:
		checkForAssimilation()

	for i in xrange(MAX_PC_PLAYERS):
		playerI = GC.getPlayer(i)
		if playerI.isRebel():
			if not GC.getTeam(playerI.getTeam()).getAtWarCount(True):
				playerI.setIsRebel(False)
				if LOG_DEBUG:
					print "[REV] %s (Player %d) is no longer a rebel due to no wars"%(playerI.getCivilizationDescription(0), i)
			elif not GC.getTeam(playerI.getTeam()).countRebelAgainst():
				playerI.setIsRebel(False)
				if LOG_DEBUG:
					print "[REV] %s (Player %d) is no longer a rebel due to no rebel against"%(playerI.getCivilizationDescription(0), i)
			elif playerI.getNumCities() > 3 and GAME.getGameTurn() - playerI.getCapitalCity().getGameTurnAcquired() > 15:
				playerI.setIsRebel(False)
				if LOG_DEBUG:
					print "[REV] %s (Player %d) is no longer a rebel by cities and capital ownership turns"%(playerI.getCivilizationDescription(0), i)
			elif playerI.getNumCities() > 0 and GAME.getGameTurn() - playerI.getCapitalCity().getGameTurnAcquired() > 30:
				playerI.setIsRebel( False )
				if LOG_DEBUG:
					print "[REV] %s (Player %d) is no longer a rebel by capital ownership turns"%(playerI.getCivilizationDescription(0), i)
			elif LOG_DEBUG:
				teamString = ""
				for j in xrange(MAX_PC_TEAMS):
					if GC.getTeam(playerI.getTeam()).isRebelAgainst(j):
						teamString += "%d, " % j
				print "[REV] %s (%d) is a rebel against teams %s"%(playerI.getCivilizationDescription(0), i, teamString)


def onEndPlayerTurn(argsList):
	iGameTurn, iPlayer = argsList

	iMax = GC.getMAX_PC_PLAYERS()
	iBarb = GC.getBARBARIAN_PLAYER()
	if iPlayer == iBarb:
		iNextPlayer = 0
	elif iPlayer + 1 >= iMax:
		return
	else:
		iNextPlayer = iPlayer + 1

	while iNextPlayer != iPlayer:
		CyPlayer = GC.getPlayer(iNextPlayer)
		if CyPlayer.isAlive():
			recordCivics(CyPlayer)
			if bSmallRevolts:
				doSmallRevolts(iNextPlayer, CyPlayer)
			return
		iNextPlayer += 1
		if iNextPlayer == iMax:
			if iPlayer == iBarb:
				return
			iNextPlayer = 0


########################## Diplomatic events ###############################

def onSetPlayerAlive(argsList):

	if argsList[1] != False:
		return
	iPlayerID = argsList[0]
	pPlayer = GC.getPlayer(iPlayerID)

	print '[REV] %s are dead, %d cities lost, %d founded a city'%(pPlayer.getCivilizationDescription(0),pPlayer.getCitiesLost(),pPlayer.isFoundedFirstCity())

	# Check if this was a put down revolution
	for iPlayerX in xrange(MAX_PC_PLAYERS):
		playerX = GC.getPlayer(iPlayerX)
		if not playerX.isAlive():
			continue
		for pCity in playerX.cities():

			revCiv = RevData.getCityVal(pCity, "RevolutionCiv")
			revTurn = RevData.getCityVal(pCity, "RevolutionTurn")

			if revCiv == pPlayer.getCivilizationType() and revTurn > 0:
				if LOG_DEBUG:
					print "[REV] The dying %s are the rebel type for %s"%(pPlayer.getCivilizationDescription(0), pCity.getName())

				if GC.getTeam(pPlayer.getTeam()).isAtWar(pCity.getTeam()):
					revIdx = pCity.getRevolutionIndex()
					localIdx = pCity.getLocalRevIndex()
					revCnt = pCity.getNumRevolts(iPlayerX)
					if pCity.getReinforcementCounter() > 0:
						# Put down while still fresh
						print "Rev - Revolution put down while still actively revolting"
						iDividend = 30
						if localIdx < 0:
							iDividend += 10
						if revIdx > RevOpt.getAlwaysViolentThreshold():
							iDividend += 10
						if revCnt > 2: # Hardened, stubborn populace
							iDividend -= 8

						changeRevIdx = -revIdx * iDividend / 100
						pCity.changeRevolutionIndex(changeRevIdx)
						pCity.changeRevRequestAngerTimer(-pCity.getRevRequestAngerTimer())
						pCity.setRevolutionIndex(min([pCity.getRevolutionIndex(), RevOpt.getAlwaysViolentThreshold()]))
						revIdxHist = RevData.getCityVal(pCity,'RevIdxHistory')
						revIdxHist['Events'][0] += changeRevIdx
						RevData.updateCityVal(pCity, 'RevIdxHistory', revIdxHist)
						pCity.setReinforcementCounter(0)
						pCity.setOccupationTimer(0)
						if LOG_DEBUG:
							print "[REV] Rev index in %s decreased to %d (from %d)"%(pCity.getName(), pCity.getRevolutionIndex(), revIdx)

					elif GAME.getGameTurn() - revTurn < 30:
						# Put down after a while
						if LOG_DEBUG: print "[REV] Revolution put down after going dormant"
						iDividend = 20
						if localIdx < 0:
							if LOG_DEBUG: print "[REV] Local conditions are improving"
							iDividend += 10
						if revIdx > RevOpt.getAlwaysViolentThreshold():
							iDividend += 10
						if revCnt > 2: # Hardened, stubborn populace
							iDividend -= 5

						changeRevIdx = -revIdx * iDividend / 100
						pCity.changeRevolutionIndex(changeRevIdx)
						pCity.changeRevRequestAngerTimer(-pCity.getRevRequestAngerTimer())
						pCity.setRevolutionIndex(min([pCity.getRevolutionIndex(),RevOpt.getAlwaysViolentThreshold()]))
						revIdxHist = RevData.getCityVal(pCity,'RevIdxHistory')
						revIdxHist['Events'][0] += changeRevIdx
						RevData.updateCityVal(pCity, 'RevIdxHistory', revIdxHist)
						pCity.setOccupationTimer(0)
						if LOG_DEBUG:
							print "[REV] Rev index in %s decreased to %d (from %d)"%(pCity.getName(), pCity.getRevolutionIndex(), revIdx)


	if not pPlayer.isFoundedFirstCity():
		# Add +1 for this turn?
		for turnID in xrange(GAME.getGameTurn()):
			if pPlayer.getAgricultureHistory(turnID) > 0:
				print '[REV] Setting founded city to True for failed reincarnation of rebel player %d'%(iPlayerID)
				pPlayer.setFoundedFirstCity(True)
				break

	pTeam = GC.getTeam(pPlayer.getTeam())
	if endWarsOnDeath and (pTeam.getNumMembers() == 1 or not pTeam.isAlive()):
		for idx in xrange(MAX_PC_TEAMS):
			if idx != pTeam.getID() and not GC.getTeam(idx).isMinorCiv() and pTeam.isAtWar(idx):
				pTeam.makePeace(idx)

	if pPlayer.isMinorCiv():
		print '[REV] %s were minor civ'%(pPlayer.getCivilizationDescription(0))
		pTeam.setIsMinorCiv(False, False)

	if LOG_DEBUG and pPlayer.isRebel():
		print "[REV] %s (%d) is no longer a rebel by death"%(pPlayer.getCivilizationDescription(0), iPlayerID)
	pPlayer.setIsRebel(False)

	# Appears to be too late, game is already ending before this popup can take effect
	if iPlayerID == GAME.getActivePlayer() and GAME.getAIAutoPlay(iPlayerID) == 0:
		try:
			GAME.setAIAutoPlay(iPlayerID, 1)
			#ChangePlayer.changeHumanPopup(True)
		except: pass


def onChangeWar(argsList):

	if argsList[0]: return # War declarations are of no interest
	iTeam = argsList[1]
	iRivalTeam = argsList[2]

	# Check if this is peaceful end to a revolution
	onTeamList = []
	onTeamCivs = []
	onRivalTeamList = []
	onRivalTeamCivs = []
	for i in xrange(MAX_PC_PLAYERS):
		pPlayer = GC.getPlayer(i)
		if pPlayer.isAlive():
			if pPlayer.getTeam() == iTeam:
				# On team declaring peace
				onTeamList.append(pPlayer)
				onTeamCivs.append(pPlayer.getCivilizationType())

			elif pPlayer.getTeam() == iRivalTeam:
				# On team accepting peace
				onRivalTeamList.append(pPlayer)
				onRivalTeamCivs.append(pPlayer.getCivilizationType())

	for pPlayer in onTeamList:
		for pCity in pPlayer.cities():

			revCiv = RevData.getCityVal(pCity, "RevolutionCiv")

			if (revCiv in onRivalTeamCivs
			and RevData.getCityVal(pCity, "RevolutionTurn") is not None
			and GAME.getGameTurn() - RevData.getCityVal(pCity, "RevolutionTurn") < 30
			):
				# City recently rebelled for civ now at peace
				localIdx = pCity.getLocalRevIndex()
				revIdx = pCity.getRevolutionIndex()
				revCnt = pCity.getNumRevolts(pCity.getOwner())
				if LOG_DEBUG:
					print "[REV] Rebels in %s have agreed to peace (%d, %d, %d)"%(pCity.getName(), revIdx, localIdx, revCnt)
				iDividend = 20
				if localIdx < 0:
					iDividend += 10
				if revIdx > RevOpt.getAlwaysViolentThreshold():
					iDividend += 10
				if revCnt > 2: # Hardened, stubborn populace
					iDividend -= 5

				changeRevIdx = -revIdx * iDividend / 100
				pCity.changeRevolutionIndex(changeRevIdx)
				pCity.setRevolutionIndex(min([pCity.getRevolutionIndex(),RevOpt.getAlwaysViolentThreshold()]))
				revIdxHist = RevData.getCityVal(pCity,'RevIdxHistory')
				revIdxHist['Events'][0] += changeRevIdx
				RevData.updateCityVal(pCity, 'RevIdxHistory', revIdxHist)
				pCity.setOccupationTimer(0)
				if LOG_DEBUG:
					print "[REV] Rev index in %s decreased to %d (from %d)"%(pCity.getName(), pCity.getRevolutionIndex(), revIdx)

		GC.getTeam(pPlayer.getTeam()).setRebelAgainst(iRivalTeam, False)

	for pPlayer in onRivalTeamList:
		for pCity in pPlayer.cities():

			revCiv = RevData.getCityVal( pCity, "RevolutionCiv" )
			if (revCiv in onTeamCivs
			and RevData.getCityVal( pCity, "RevolutionTurn" ) is not None
			and GAME.getGameTurn() - RevData.getCityVal(pCity, "RevolutionTurn") < 30
			):
				# City recently rebelled for civ now at peace
				localIdx = pCity.getLocalRevIndex()
				revIdx = pCity.getRevolutionIndex()
				revCnt = pCity.getNumRevolts(pCity.getOwner())
				if LOG_DEBUG:
					print "[REV] Rebels in %s have sued for peace" % pCity.getName()
				iDividend = 20
				if localIdx < 0:
					iDividend += 10
				if revIdx > RevOpt.getAlwaysViolentThreshold():
					iDividend += 10
				if revCnt > 2: # Hardened, stubborn populace
					iDividend -= 5

				changeRevIdx = -revIdx * iDividend / 100
				pCity.changeRevolutionIndex( changeRevIdx )
				pCity.setRevolutionIndex(min([pCity.getRevolutionIndex(), RevOpt.getAlwaysViolentThreshold()]))
				revIdxHist = RevData.getCityVal(pCity,'RevIdxHistory')
				revIdxHist['Events'][0] += changeRevIdx
				RevData.updateCityVal(pCity, 'RevIdxHistory', revIdxHist)
				pCity.setOccupationTimer(0)
				if LOG_DEBUG:
					print "[REV] Rev index in %s decreased to %d (from %d)"%(pCity.getName(), pCity.getRevolutionIndex(), revIdx)

		GC.getTeam(pPlayer.getTeam()).setRebelAgainst(iTeam, False)


########################## City-based events ###############################

def onCityBuilt( argsList ):
	city = argsList[0]

	RevData.initCity( city )

	pPlayer = GC.getPlayer( city.getOwner() )

	if( pPlayer.isNPC() ) :
		city.setRevolutionIndex( int(.4*RevOpt.getAlwaysViolentThreshold()) )
		city.setRevIndexAverage(city.getRevolutionIndex())
		return

	if( not city.area().getID() == pPlayer.getCapitalCity().area().getID() ) :
		city.setRevolutionIndex( int(.35*RevOpt.getInstigateRevolutionThreshold()) )
	else :
		city.setRevolutionIndex( int(.25*RevOpt.getInstigateRevolutionThreshold()) )
	city.setRevIndexAverage(city.getRevolutionIndex())

	revTurn = RevData.revObjectGetVal( pPlayer, 'RevolutionTurn' )
	if revTurn != None and pPlayer.getNumCities() < 4 and GAME.getGameTurn() - revTurn < 25:
		relID = pPlayer.getStateReligion()
		if relID > -1:
			if LOG_DEBUG:
				print "[REV] New rebel city %s given rebel religion" % city.getName()
			city.setHasReligion(relID, True, False, False)

def onCityAcquired( argsList ):
	owner,playerType,pCity,bConquest,bTrade = argsList

	checkRebelBonuses( argsList )
	updateRevolutionIndices( argsList )

	# Init city script data (unit spawn counter, rebel player)
	iRevCiv = RevData.getCityVal(pCity, 'RevolutionCiv')
	RevData.initCity(pCity)
	RevData.setCityVal( pCity, 'RevolutionCiv', iRevCiv )

	iTurns = pCity.getOccupationTimer()
	pCity.setRevolutionCounter( max([int(1.5*iTurns),3]) )


def checkRebelBonuses(argsList):
	# Give bonuses to a rebel player who successfully captures one of their rebellious cities
	owner, playerType, pCity, bConquest, bTrade = argsList

	newOwnerID = pCity.getOwner()
	newOwner = GC.getPlayer(newOwnerID)
	newOwnerCiv = newOwner.getCivilizationType()
	oldOwnerID = pCity.getPreviousOwner()
	orgOwnerID = pCity.getOriginalOwner()

	# TODO: Handle case where city is acquired by disorganized rebels
	if newOwnerID == GC.getBARBARIAN_PLAYER() and pCity.getRevolutionCounter() > 0:
		print "[REV] City %s captured by barb rebels!" % pCity.getName()
		oldOwner = GC.getPlayer(oldOwnerID)

		if not oldOwnerID == orgOwnerID:
			orgOwner = GC.getPlayer(orgOwnerID)

		if pCity.countTotalCultureTimes100() > 100*100:
			if not oldOwnerID == pCity.findHighestCulture():
				cultOwner = GC.getPlayer(pCity.findHighestCulture())

	elif newOwnerCiv == RevData.getCityVal(pCity, 'RevolutionCiv'):

		# TODO: Check whether revolt is active in RevoltData
		if pCity.getReinforcementCounter() > 0 or (pCity.unhappyLevel(0) - pCity.happyLevel()) > 0:
			print "[REV] Rebellious pCity %s is captured by rebel identity %s (%d)!!!" %(pCity.getName(), newOwner.getCivilizationDescription(0), newOwnerCiv)

			newOwnerTeam = GC.getTeam(newOwner.getTeam())
			oldOwner = GC.getPlayer(oldOwnerID)
			oldOwnerTeam = GC.getTeam(oldOwner.getTeam())
			if oldOwnerTeam.isAVassal():
				for teamID in xrange(MAX_PC_TEAMS):
					if oldOwnerTeam.isVassal(teamID):
						oldOwnerTeam = GC.getTeam(teamID)

			ix = pCity.getX()
			iy = pCity.getY()

			[iWorker, iBestDefender, iCounter, iAttack] = RevUtils.getHandoverUnitTypes(pCity)

			newUnitList = []

			# Couple units regardless of rebel status
			newUnitList.append(newOwner.initUnit(iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH))
			if pCity.getPopulation() > 4:
				newUnitList.append(newOwner.initUnit(iCounter, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH))

			if newOwner.isRebel():
				# Extra benefits if still considered a rebel
				szTxt = TRNSLTR.getText("TXT_KEY_REV_MESS_YOUR_CAPTURE",())%(pCity.getName())
				icon = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath()
				sound = "AS2D_CITY_REVOLT"
				eMsgType = InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT
				iMsgTime = GC.getEVENT_MESSAGE_TIME()
				CvUtil.sendMessage(szTxt, newOwnerID, iMsgTime, icon, ColorTypes(8), ix, iy, True, True, eMsgType, sound, False)

				szTxt = TRNSLTR.getText("TXT_KEY_REV_MESS_REBEL_CONTROL",())%(newOwner.getCivilizationDescription(0),pCity.getName())
				CvUtil.sendMessage(szTxt, oldOwnerID, iMsgTime, None, ColorTypes(7), eMsgType=eMsgType, bForce=False)

				# Gold
				iGold = GAME.getSorenRandNum(min([80,8*pCity.getPopulation()]), 'Rev') + 8
				szTxt = TRNSLTR.getText("TXT_KEY_REV_MESS_YOUR_CAPTURE_GOLD",()) %(pCity.getName(),iGold)
				CvUtil.sendMessage(szTxt, newOwnerID, iMsgTime, icon, ColorTypes(8), ix, iy, False, False, eMsgType, sound, False)
				newOwner.changeGold(iGold)

				# Culture
				newCulVal = int( revCultureModifier*max([pCity.getCulture(oldOwnerID),pCity.countTotalCultureTimes100()/200]) )
				newPlotVal = int( revCultureModifier*max([pCity.plot().getCulture(oldOwnerID),pCity.plot().countTotalCulture()/2]) )
				RevUtils.giveCityCulture( pCity, newOwnerID, newCulVal, newPlotVal)

				# Extra units
				if iWorker != -1:
					newUnitList.append(newOwner.initUnit(iWorker, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH))
				if pCity.getPopulation() > 7:
					newUnitList.append(newOwner.initUnit(iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH))
				if pCity.getPopulation() > 4 and newOwnerTeam.getPower(True) < oldOwnerTeam.getPower(True)/4:
					newUnitList.append(newOwner.initUnit(iAttack, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH))

				if newOwner.getNumCities() <= 1:
					# Extra units for first city captured
					newUnitList.append(newOwner.initUnit(iCounter, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH))
					if newOwnerTeam.getPower(True) < oldOwnerTeam.getPower(True)/2:
						newUnitList.append(newOwner.initUnit(iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH))
						newUnitList.append(newOwner.initUnit(iAttack, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH))
					elif newOwnerTeam.getPower(True) < oldOwnerTeam.getPower(True):
						newUnitList.append(newOwner.initUnit(iAttack, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH))

				# Give a boat to island rebels
				if pCity.isCoastal(10) and pCity.area().getNumCities() < 3 and pCity.area().getNumTiles() < 25:
					iBestCombat = -1
					for iUnitX in xrange(GC.getNumUnitInfos()):
						info = GC.getUnitInfo(iUnitX)
						if (info.getDomainType() == DomainTypes.DOMAIN_SEA
						and info.getUnitAIType(UnitAITypes.UNITAI_ASSAULT_SEA)
						and newOwner.canTrain(iUnitX,False,False)
						):
							iCombat = info.getCombat()
							if iBestCombat < iCombat:
								bestUnit = info
								iBestUnit = iUnitX
								iBestCombat = iCombat

					if iBestCombat > -1:
						newOwner.initUnit(iBestUnit, ix, iy, UnitAITypes.UNITAI_ASSAULT_SEA, DirectionTypes.DIRECTION_SOUTH)
						print "Rev - Rebels get a %s to raid motherland" % bestUnit.getDescription()

				# Change city disorder timer to favor new player
				iTurns = pCity.getOccupationTimer()
				iTurns = iTurns/4 + 1
				pCity.setOccupationTimer(iTurns)

				# Temporary happiness boost
				pCity.changeRevSuccessTimer( int(iTurns + RevUtils.getGameSpeedMod()*15) )

				# Trigger golden age for rebel civ under certain circumstances
				revTurn = RevData.revObjectGetVal(newOwner, 'RevolutionTurn')
				if not revTurn == None and GAME.getGameTurn() - revTurn < 4*GAME.goldenAgeLength():
					if newOwner.getNumCities() == 3:
						if not newOwner.getCitiesLost():
							# By verifying they've never lost a city, gaurantee it doesn't happen multiple times
							szTxt = TRNSLTR.getText("TXT_KEY_REV_MESS_GOLDEN_AGE",())
							CvUtil.sendMessage(szTxt, newOwnerID, iMsgTime, icon, ColorTypes(8), ix, iy, False, False, eMsgType, sound, False)
							newOwner.changeGoldenAgeTurns( int(1.5*GAME.goldenAgeLength()) )

			else: # Conqueror not considered a rebel, fewer benefits
				# Culture
				newCulVal = int(revCultureModifier*max([pCity.getCulture(oldOwnerID)/2,pCity.countTotalCultureTimes100()/400]))
				newPlotVal = int(revCultureModifier*max([pCity.plot().getCulture(oldOwnerID)/2,pCity.plot().countTotalCulture()/4]))
				RevUtils.giveCityCulture(pCity, newOwnerID, newCulVal, newPlotVal)

				# Change city disorder timer to favor new player
				iTurns = pCity.getOccupationTimer()
				iTurns = min([iTurns, iTurns/3 + 1])
				pCity.setOccupationTimer(iTurns)

				# Temporary happiness boost
				pCity.changeRevSuccessTimer(int(iTurns + RevUtils.getGameSpeedMod()*6))

			# Injure free units
			for unit in newUnitList:
				if unit.canFight():
					iDamage = 20 + GAME.getSorenRandNum(20,'Rev - Injure unit')
					unit.setDamage(iDamage, oldOwnerID)

		else: # City once rebelled as this civ type, but not currently rebellious
			if LOG_DEBUG:
				print "[REV] %s, captured by former rebel identity: %s (%d)!"%(pCity.getName(),newOwner.getCivilizationDescription(0),newOwnerCiv)
			newCulVal = int( revCultureModifier*max([pCity.getCulture(oldOwnerID)/2,pCity.countTotalCultureTimes100()/400]) )
			newPlotVal = int( revCultureModifier*max([pCity.plot().getCulture(oldOwnerID)/2,pCity.plot().countTotalCulture()/4]) )
			RevUtils.giveCityCulture( pCity, newOwnerID, newCulVal, newPlotVal)

			iTurns = pCity.getOccupationTimer()
			iTurns = iTurns/2 + 1
			pCity.setOccupationTimer(iTurns)

def updateRevolutionIndices( argsList ) :
	owner,playerType,pCity,bConquest,bTrade = argsList

	newOwnerID = pCity.getOwner()
	newOwner = GC.getPlayer(newOwnerID)
	newOwnerCiv = newOwner.getCivilizationType()
	oldOwnerID = pCity.getPreviousOwner()
	orgOwnerID = pCity.getOriginalOwner()

	if( newOwner.isNPC() ) :
		return

	newRevIdx = 400
	changeRevIdx = -40

	if( bConquest ) :
		# Occupied cities also rack up rev points each turn
		newRevIdx += pCity.getRevolutionIndex()/4
		newRevIdx = min( [newRevIdx, 600] )

		if( pCity.plot().calculateCulturePercent( newOwnerID ) > 90 ) :
			changeRevIdx -= 75
			newRevIdx -= 100
		elif( pCity.plot().calculateCulturePercent( newOwnerID ) > 40 ) :
			changeRevIdx -= 35
			newRevIdx -= 60
		elif( pCity.plot().calculateCulturePercent( newOwnerID ) > 20 ) :
			changeRevIdx -= 30

	elif( bTrade ) :
		newRevIdx += pCity.getRevolutionIndex()/3
		newRevIdx = min( [newRevIdx, 650] )

		if( pCity.plot().calculateCulturePercent( newOwnerID ) > 90 ) :
			newRevIdx -= 50

	else :
		# Probably cultural conversion
		newRevIdx -= 100
		if( pCity.plot().calculateCulturePercent( newOwnerID ) > 50 ) :
			changeRevIdx -= 25


	if( newOwner.isRebel() and newOwnerCiv == RevData.getCityVal(pCity, 'RevolutionCiv') ) :
		changeRevIdx -= 50
		newRevIdx -= 200
	elif( newOwnerID == pCity.getOriginalOwner() ) :
		changeRevIdx -= 25
		newRevIdx -= 100

	if( pCity.getHighestPopulation() < 6 ) :
		changeRevIdx += 20
		newRevIdx -= 50

	changeRevIdx = int(math.floor( cityAcquiredModifier*changeRevIdx + .5 ))

	print "	Revolt - Acquisition of %s by %s reduces rev indices by %d" %(pCity.getName(), newOwner.getCivilizationDescription(0), changeRevIdx)

	iCityID = pCity.getID()
	for pListCity in newOwner.cities():
		if pListCity.getID() != iCityID:
			pListCity.changeRevolutionIndex(changeRevIdx)
			revIdxHist = RevData.getCityVal(pListCity,'RevIdxHistory')
			revIdxHist['Events'][0] += changeRevIdx
			RevData.updateCityVal(pListCity, 'RevIdxHistory', revIdxHist)

	print "	Revolt - New rev idx for %s is %d" %(pCity.getName(), newRevIdx)

	pCity.setRevolutionIndex( newRevIdx )
	pCity.setRevIndexAverage( newRevIdx )
	pCity.setRevolutionCounter( acquiredTurns )
	pCity.setReinforcementCounter( 0 )
	RevData.updateCityVal( pCity, 'RevIdxHistory', RevDefs.initRevIdxHistory() )

	if newOwner.isRebel():
		if newOwner.getNumCities() > 1 and RevData.revObjectGetVal(newOwner, 'CapitalName') == CvUtil.convertToStr(pCity.getName()):
			# Rebel has captured their instigator city, make this their capital
			print "[REV] Rebel %s have captured their instigator city, %s!  Moving capital." %(newOwner.getCivilizationDescription(0), pCity.getName())
			if newOwner.isHuman():
				# TODO: support this with a popup question
				pass
			else:
				eCapitalBuilding = GC.getInfoTypeForString(RevDefs.sXMLPalace)
				oldCapital = newOwner.getCapitalCity()
				oldCapital.setNumRealBuilding(eCapitalBuilding, 0)
				pCity.setNumRealBuilding(eCapitalBuilding, 1)

		# Ripple effects through other rebellious cities
		for cityX in GC.getPlayer(oldOwnerID).cities():
			reinfCount = cityX.getReinforcementCounter()
			if reinfCount > 2 and RevData.getCityVal(cityX, 'RevolutionCiv') == newOwner.getCivilizationType():
				if reinfCount < 5:
					reinfCount = 2
				else: reinfCount -= 2

				print "[REV] Accelerating reinforcement in " + cityX.getName()
				# Setting below two will turn off reinforcement
				if reinfCount < 2: reinfCount = 2
				cityX.setReinforcementCounter(reinfCount)


def onCityLost(argsList):
	CyCity, = argsList
	iPlayer = CyCity.getOwner()

	playerCityLost(GC.getPlayer(iPlayer), CyCity, bConquest = CyCity.plot().getNumDefenders(iPlayer) == 0)

def playerCityLost(CyPlayer, CyCity, bConquest = True):

	if CyPlayer.isNPC() or CyPlayer.getNumCities() < 1:
		return

	revIdxChange = (GAME.getGameTurn() - CyCity.getGameTurnAcquired()) * 100.0 / GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent()
	revIdxChange += CyCity.getHighestPopulation()
	revIdxChange *= CyCity.plot().calculateCulturePercent(CyPlayer.getID()) / 100.0

	if revIdxChange > 500:
		revIdxChange = 500
	elif revIdxChange < 0:
		revIdxChange = 0

	if revIdxChange > 0:
		if not bConquest:
			revIdxChange = revIdxChange/4.0

		if CyPlayer.isRebel():
			revIdxChange /= 2.0

	revIdxChange = int(cityLostModifier*revIdxChange + .5)

	print "[REV] Loss of %s by %s (%d bConq): %d rev idx change" %(CyCity.getName(), CyPlayer.getCivilizationDescription(0), bConquest, revIdxChange)

	for cityX in CyPlayer.cities():
		cityX.changeRevolutionIndex(revIdxChange)
		revIdxHist = RevData.getCityVal(cityX,'RevIdxHistory')
		revIdxHist['Events'][0] += revIdxChange
		RevData.updateCityVal(cityX, 'RevIdxHistory', revIdxHist)


def onBuildingBuilt(argsList):
	pCity, iBuildingType = argsList

	buildingInfo = GC.getBuildingInfo(iBuildingType)

	if buildingInfo.getMaxGlobalInstances() == 1 and buildingInfo.getPrereqReligion() < 0 and buildingInfo.getProductionCost() > 10:
		if LOG_DEBUG:
			print"[REV] World wonder %s build in %s"%(buildingInfo.getDescription(), pCity.getName())
		curRevIdx = pCity.getRevolutionIndex()
		pCity.changeRevolutionIndex(-max([150, curRevIdx / 4]))

		for cityX in GC.getPlayer(pCity.getOwner()).cities():
			curRevIdx = cityX.getRevolutionIndex()
			iRevIdxChange = -max([75, curRevIdx * 12/100])
			cityX.changeRevolutionIndex(iRevIdxChange)
			revIdxHist = RevData.getCityVal(pCity,'RevIdxHistory')
			revIdxHist['Events'][0] += iRevIdxChange
			RevData.updateCityVal(pCity, 'RevIdxHistory', revIdxHist)

	elif buildingInfo.getMaxPlayerInstances() == 1 and buildingInfo.getPrereqReligion() < 0 and buildingInfo.getProductionCost() > 10:
		if LOG_DEBUG:
			print "[REV] National wonder %s build in %s"%(buildingInfo.getDescription(), pCity.getName())
		curRevIdx = pCity.getRevolutionIndex()
		pCity.changeRevolutionIndex(-max([80, curRevIdx * 12/100]))

		for cityX in GC.getPlayer(pCity.getOwner()).cities():
			curRevIdx = cityX.getRevolutionIndex()
			iRevIdxChange = -max([50, curRevIdx * 7/100])
			cityX.changeRevolutionIndex(iRevIdxChange)
			revIdxHist = RevData.getCityVal(pCity,'RevIdxHistory')
			revIdxHist['Events'][0] += iRevIdxChange
			RevData.updateCityVal(pCity, 'RevIdxHistory', revIdxHist)


########################## Religious events ###############################

def onReligionFounded(argsList):
	iReligion = argsList[0]

	if iReligion > -1:
		player = GC.getPlayer(argsList[1])
		if not player.isAnarchy():
			iStateReligion = player.getStateReligion()
			if iStateReligion > -1 and iStateReligion != iReligion:
				pCity = GC.getGame().getHolyCity(iReligion)
				if pCity.getOwner() == argsList[1]:
					curRevIdx = pCity.getRevolutionIndex()
					pCity.setRevolutionIndex(max([int(.35*RevDefs.revInstigatorThreshold),curRevIdx+100]))
					if LOG_DEBUG:
						print "[REV] %s founded non-state religion, index of %s now %d ... state %d, new %d"%(pCity.getName(),pCity.getName(),pCity.getRevolutionIndex(),player.getStateReligion(),iReligion)



def recordCivics(CyPlayer):
	curCivics = []
	for i in xrange(0,GC.getNumCivicOptionInfos()):
		curCivics.append(CyPlayer.getCivics(i))

	RevData.revObjectSetVal(CyPlayer, "CivicList", curCivics)


def updateAttitudeExtras( bVerbose = False ) :

	for i in xrange(MAX_PC_PLAYERS):
		playerI = GC.getPlayer( i )

		for j in xrange(MAX_PC_PLAYERS):
			playerJ = GC.getPlayer( j )
			attEx = playerI.AI_getAttitudeExtra(j)
			# Odds and rate partially determined by current attitude value
			if( attEx > 0 and GAME.getSorenRandNum(100,'Rev: Attitude') < attEx*15 ) :
				playerI.AI_changeAttitudeExtra(j, -(1+attEx/10))
				if LOG_DEBUG and bVerbose:
					print "[REV] Extra Attitude for %s of %s now %d"%(playerI.getCivilizationDescription(0),playerJ.getCivilizationDescription(0),playerI.AI_getAttitudeExtra(j))
			elif( attEx < 0 and GAME.getSorenRandNum(100,'Rev: Attitude') < -attEx*20 ) :
				teamI = GC.getTeam( playerI.getTeam() )
				if( not teamI.isAtWar( playerJ.getTeam() ) ) :
					playerI.AI_changeAttitudeExtra(j, -(attEx/10))
					if LOG_DEBUG and bVerbose:
						print "[REV] Extra Attitude for %s of %s now %d"%(playerI.getCivilizationDescription(0),playerJ.getCivilizationDescription(0),playerI.AI_getAttitudeExtra(j))

def removeFloatingRebellions():
	# Destroy all units for a rebel who is at peace, yet has no cities

	for iPlayerX in xrange(MAX_PC_PLAYERS):
		playerX = GC.getPlayer(iPlayerX)

		if (not playerX.isAlive() or playerX.getNumCities() or playerX.getNumUnits() < 1
		or not playerX.isRebel() and not playerX.isFoundedFirstCity()
		): continue

		bOnlySpy = True
		spy = None
		unitX, i = playerX.firstUnit(False)
		while unitX:
			if unitX.isFound():
				return
			if bOnlySpy:
				if unitX.getUnitAIType() != UnitAITypes.UNITAI_SPY:
					bOnlySpy = False
				else: spy = unitX
			unitX, i = playerX.nextUnit(i, False)

		print "[REV] Player %d (%s) is a homeless rebel"%(iPlayerX, playerX.getCivilizationDescription(0))

		if not GC.getTeam(playerX.getTeam()).getAtWarCount(True):
			print "[REV] Rebel player %d has lost their cause, terminating rebel" % iPlayerX
			playerX.killUnits()

		elif bOnlySpy:
			print "[REV] Rebel player %d has only spies" % iPlayerX

			if spy and GAME.getSorenRandNum(100, "Rev - Only spy death") < 10:
				spy.kill(False, -1)
				print "[REV] Killed one spy of Rebel player %d" % iPlayerX


########################## Assimilation ###############################

def checkForAssimilation():

	iNumPlayers = 0
	iMaxEra = 0
	players = []
	for iPlayerX in xrange(MAX_PC_PLAYERS):
		CyPlayerX = GC.getPlayer(iPlayerX)
		if CyPlayerX.isAlive() and not CyPlayerX.isMinorCiv():
			iNumPlayers += 1
			if not CyPlayerX.isHuman():
				players.append((iPlayerX, CyPlayerX))
			iEra = CyPlayerX.getCurrentEra()
			if iEra > iMaxEra:
				iMaxEra = iEra

	if iNumPlayers == 0: return

	MAP = GC.getMap()

	minNumPlots = int((MAP.getLandPlots()/(1.0*iNumPlayers) + .5)/3.0) + 1
	if minNumPlots > 21: minNumPlots = 21
	elif minNumPlots < 9: minNumPlots = 9

	iTurn = GAME.getGameTurn()
	for iPlayerX, CyPlayerX in players:

		CyTeamX = GC.getTeam(CyPlayerX.getTeam())
		CyCity0 = CyPlayerX.getCapitalCity()
		if CyCity0 is None: continue
		iTurnAcquiredCity0 = CyCity0.getGameTurnAcquired()
		CyPlot0 = None
		szCiv = CyPlayerX.getCivilizationDescription(0)

		iMinCities = GC.getWorldInfo(MAP.getWorldSize()).getTargetNumCities()
		iNumCities = CyPlayerX.getNumCities() # We know this is greater than 0 as a capital city has been confirmed.

		bRiskWar = False
		iPlayerML = RevData.revObjectGetVal(CyPlayerX, 'MotherlandID')

		if iPlayerML != None:
			CyPlayerML = GC.getPlayer(iPlayerML)
			bWarSeparatist = CyTeamX.isAtWar(CyPlayerML.getTeam())
			if bWarSeparatist:
				revTurn = RevData.revObjectGetVal(CyPlayerX, 'RevolutionTurn')
				if revTurn != None and iTurn - revTurn < 40:
					bRiskWar = True

		CyPlayerDominant = None
		joinPlayerID = RevData.revObjectGetVal(CyPlayerX, 'JoinPlayerID')

		if not joinPlayerID == None and iTurn - iTurnAcquiredCity0 < 30 and not CyTeamX.isAVassal():
			if iNumCities < iMinCities:
				iOdds = 12 + iNumCities * 2

				if iPlayerML != None and bWarSeparatist:
					iCivType = CyPlayerX.getCivilizationType()
					for CyCityML in CyPlayerML.cities():
						if RevData.getCityVal(CyCityML, 'RevolutionCiv') == iCivType:
							revTurn = RevData.getCityVal(pCity, 'RevolutionTurn')
							if revTurn != None and iTurn - revTurn < 25:
								iOdds -= 2

				if iOdds > 10 + GAME.getSorenRandNum(100, 'Revolution: Assimilate'):
					CyPlayerDominant = GC.getPlayer(joinPlayerID)
					print "	Revolt - Assimilation! The rebel %s are requesting again to join the %s now that they've captured %d cities"%(szCiv, CyPlayerDominant.getCivilizationDescription(0), iNumCities)


		else:
			if iTurn - iTurnAcquiredCity0 > 15 and iNumCities < iMinCities:
				iTotalLand = CyPlayerX.getTotalLand()
				if iTotalLand < minNumPlots:

					if CyCity0.area().getNumCities() < iNumCities + 2:
						continue # Isolated

					if CyTeamX.getNumMembers() > 1:
						continue # In alliance

					iOdds = 2*(minNumPlots - iTotalLand) + (4 + 4*iMaxEra)/CyCity0.getPopulation()

					if CyCity0.getOccupationTimer() > 0:
						iOdds *= 3

					iOdds += CyCity0.getRevolutionIndex()/100

					CyPlot0 = CyCity0.plot()
					### Special cases
					if CyTeamX.isAVassal():

						if iOdds > 10 + GAME.getSorenRandNum(100, 'Revolution: Assimilate'):

							# If player is a Vassal, should only be allowed to assimilate with master
							CyPlayerMaster = None
							for iTeamY in xrange(MAX_PC_TEAMS):
								if not CyTeamX.isVassal(iTeamY): continue

								iPlayerMaster = GC.getTeam(iTeamY).getLeaderID()
								CyPlayerMaster = GC.getPlayer(iPlayerMaster)

								print "	Revolt - Assimilation!  Vassal %s considering assimilation to master %s" %(szCiv, CyPlayerMaster.getCivilizationDescription(0))

								relations = CyPlayerX.AI_getAttitude(iPlayerMaster)

								if CyPlot0.getCulture(iPlayerMaster)/(1.0*CyPlot0.countTotalCulture()) > .25:
									# Assimilate with master with large culture in city
									if not relations == AttitudeTypes.ATTITUDE_FURIOUS:

										if not CyPlayerMaster.isHuman():
											CyPlayerDominant = CyPlayerMaster

										elif not iPlayerX in noAssimilateList:
											CyPlayerDominant = CyPlayerMaster
										if CyPlayerDominant:
											print "	Revolt - Assimilation to master based on culture"

								elif relations in (AttitudeTypes.ATTITUDE_PLEASED, AttitudeTypes.ATTITUDE_FRIENDLY):
									# Assimilate with friendly, powerful master
									masterPower = CyPlayerMaster.getPower()
									vassalPower = CyPlayerX.getPower()

									if masterPower > 3*vassalPower:
										if not CyPlayerMaster.isHuman():
											CyPlayerDominant = CyPlayerMaster
										elif not iPlayerX in noAssimilateList:
											CyPlayerDominant = CyPlayerMaster
										if CyPlayerDominant:
											print "	Revolt - Assimilation to friendly and powerful master"
								break

					elif CyPlot0.calculateCulturePercent(iPlayerX) < 60:
						### Capital has foreign influence
						iPlayerCult = CyPlot0.calculateCulturalOwner() # iPlayerCult guaranteed to be alive
						if iPlayerCult != iPlayerX:
							iOdds += 15

						if iOdds > 10 + GAME.getSorenRandNum(100, 'Revolution: Assimilate'):
							print "	Revolt - Assimilation!  %s considering assimilation by culture" % szCiv

							if iPlayerCult > -1 and iPlayerCult != iPlayerX and not CyPlayerX.AI_getAttitude(iPlayerCult) == AttitudeTypes.ATTITUDE_FURIOUS:
								## Assimilate with cultural owner
								CyPlayerY = GC.getPlayer(iPlayerCult)
								if CyPlayerY.isAlive():
									if not CyPlayerY.isHuman():
										CyPlayerDominant = CyPlayerY
									elif not iPlayerX in noAssimilateList:
										CyPlayerDominant = CyPlayerY
									if CyPlayerDominant:
										print "	Revolt - Assimilation culture owner: " + CyPlayerDominant.getCivilizationDescription(0)

							if not CyPlayerDominant:
								## Check for good relations with second place culture
								iMaxCult2 = 0
								for iPlayerY in xrange(MAX_PC_PLAYERS):
									if iPlayerY in (iPlayerX, iPlayerCult): continue
									CyPlayerY = GC.getPlayer(iPlayerY)
									if not CyPlayerY.isAlive(): continue
									iCulture = CyPlot0.getCulture(iPlayerY)
									if iCulture > iMaxCult2:
										iPlayerCult2 = iPlayerY
										CyPlayerCult2 = CyPlayerY
										iMaxCult2 = iCulture

								iTotalCulture = CyPlot0.countTotalCulture()
								if iMaxCult2/(1.0*iTotalCulture) > .2:
									relations = CyPlayerX.AI_getAttitude(iPlayerCult2)
									if relations in (AttitudeTypes.ATTITUDE_PLEASED, AttitudeTypes.ATTITUDE_FRIENDLY) \
									or relations == AttitudeTypes.ATTITUDE_CAUTIOUS and iMaxCult2/(1.0*iTotalCulture) > .4:
										if not CyPlayerCult2.isHuman():
											CyPlayerDominant = CyPlayerCult2
										elif not iPlayerX in noAssimilateList:
											CyPlayerDominant = CyPlayerCult2
										if CyPlayerDominant:
											print "	Revolt - Assimilation to friendly, 2nd culture player"

		if CyPlayerDominant:
			# Assimilate!
			if CyPlayerDominant.isHuman():
				# Zoom to city
				if CyPlot0 is None: CyPlot0 = CyCity0.plot()
				CyCamera().JustLookAt(CyPlot0.getPoint())
				# Additions by Caesium et al
				caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
				caesiumtextResolution = caesiumtR.split('x')
				caesiumpasx = int(caesiumtextResolution[0])/10
				caesiumpasy = int(caesiumtextResolution[1])/10
				popup = PyPopup.PyPopup(RevDefs.assimilationPopup, contextType = EventContextTypes.EVENTCONTEXT_ALL, bDynamic = False)
				if centerPopups: popup.setPosition(3*caesiumpasx,3*caesiumpasy)
				# Additions by Caesium et al
				bodStr = TRNSLTR.getText("TXT_KEY_REV_ASSIM_POPUP", ()) %(szCiv, szCiv)
				if bRiskWar:
					bodStr += '\n\n' + TRNSLTR.getText("TXT_KEY_REV_ASSIM_POPUP_REBEL", ())%(CyPlayerML.getCivilizationDescription(0))
				popup.setBodyString(bodStr)
				popup.addSeparator()
				popup.addButton(TRNSLTR.getText("TXT_KEY_REV_BUTTON_ACCEPT",()))
				popup.addButton(TRNSLTR.getText("TXT_KEY_REV_BUTTON_MAYBE_LATER",()))
				popup.addButton(TRNSLTR.getText("TXT_KEY_REV_BUTTON_NEVER",()))
				popup.setUserData((iPlayerX, CyPlayerDominant.getID(), bRiskWar))
				popup.launch(bCreateOkButton = False)
			else:
				if bRiskWar:
					# Assimilating a rebel involves potential war declaration, attitude issues
					CyPlayerML.AI_changeAttitudeExtra(CyPlayerDominant.getID(), CyPlayerML.AI_getAttitudeExtra(iPlayerX))
					print "	Revolt - The %s (motherland of the rebel %s) is considering attacking the %s over the assimilation"%(CyPlayerML.getCivilizationDescription(0),szCiv,CyPlayerDominant.getCivilizationDescription(0))
					[iOdds,attackerTeam,victimTeam] = RevUtils.computeWarOdds(CyPlayerML, CyPlayerDominant, CyCity0.area(), False, True, True )
					if attackerTeam.canDeclareWar(victimTeam.getID()) and iOdds > GAME.getSorenRandNum(100, 'Revolution: War'):
						print "  Revolt - Rebel motherland takes exception to assimilation, team %d declare war on team %d"%(attackerTeam.getID(), victimTeam.getID())
						attackerTeam.declareWar( victimTeam.getID(), True, WarPlanTypes.NO_WARPLAN )

				CyPlayerDominant.assimilatePlayer(iPlayerX)

def assimilateHandler(iPlayerID, netUserData, popupReturn):

	global noAssimilateList

	if popupReturn.getButtonClicked() == 0:
		if LOG_DEBUG:
			print "[REV] Assimilation accepted!"
		if netUserData[2]:
			pMotherland = GC.getPlayer(RevData.revObjectGetVal(GC.getPlayer(netUserData[0]), 'MotherlandID' ))
			pMotherland.AI_changeAttitudeExtra( etUserData[1], pMotherland.AI_getAttitudeExtra(netUserData[0]))
			if LOG_DEBUG:
				print "[REV] Rebel motherland %s extra attidude to %s now %d"%(pMotherland.getCivilizationDescription(0), GC.getPlayer(netUserData[1]).getCivilizationDescription(0), pMotherland.AI_getAttitudeExtra(netUserData[0]))

			[iOdds, attackerTeam, victimTeam] = RevUtils.computeWarOdds(pMotherland, GC.getPlayer(netUserData[1]), GC.getPlayer(netUserData[0]).getCapitalCity().area(), False, True, True)

			if attackerTeam.canDeclareWar(victimTeam.getID()) and iOdds > GAME.getSorenRandNum(100, 'Revolution: War'):
				if LOG_DEBUG:
					print "[REV] Rebel motherland takes exception, team %d declare war on team %d"%(attackerTeam.getID(), victimTeam.getID())
				attackerTeam.declareWar( victimTeam.getID(), True, WarPlanTypes.NO_WARPLAN )

		GC.getPlayer(netUserData[1]).assimilatePlayer(netUserData[0])
	elif popupReturn.getButtonClicked() == 1:
		if LOG_DEBUG:
			print "[REV] Assimilation postponed"
	else:
		if LOG_DEBUG:
			print "[REV] Assimilation rejected!"
		noAssimilateList.append(netUserData[0])

# Small revolts are short duration disorder striking a city, shutting down production and culture, etc.
def doSmallRevolts(iPlayer, CyPlayer):

	if iPlayer > 39:
		raise "NPC does not revolt!"

	for city in CyPlayer.cities():

		revIdx = city.getRevolutionIndex()

		if revIdx <= 5 * RevDefs.revReadyDividend * RevDefs.revInstigatorThreshold / (4 * RevDefs.revReadyDivisor):
			continue

		if city.getOccupationTimer() > 0 or city.getRevolutionCounter() > 0 or RevData.getCityVal(city, 'SmallRevoltCounter') > 0:
			continue # Already in a revolt

		localRevIdx = city.getLocalRevIndex()
		if localRevIdx > 0:
			localFactor = 1 + localRevIdx / 3
			if localFactor > 10:
				localFactor = 10
		else:
			localFactor = localRevIdx - 1
			if localFactor < -15:
				localFactor = -15

		iOdds = localFactor + 100 * revIdx / (8 * RevDefs.alwaysViolentThreshold)
		if iOdds > 15:
			iOdds = 15

		if GAME.getSorenRandNum(100, "Rev: Small Revolt") < iOdds:
			szName = city.getName()
			print "[REV] Small revolt in %s with odds %d (%d idx, %d loc)" %(szName, iOdds, revIdx, localRevIdx)
			city.setOccupationTimer(2)

			RevData.setCityVal(city, 'SmallRevoltCounter', 6)

			szTxt = TRNSLTR.getText("TXT_KEY_REV_MESS_SMALL_REVOLT",()) % szName
			icon = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath()
			CvUtil.sendMessage(szTxt, iPlayer, 16, icon, ColorTypes(7), city.getX(), city.getY(), True, True, InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT, "AS2D_CITY_REVOLT", False)
