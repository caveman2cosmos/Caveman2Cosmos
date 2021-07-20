# Utility functions for Revolution Mod
#
# by jdog5000
# Version 1.5

from CvPythonExtensions import *

import RevData
from RevCivicsUtils import *
from RevTraitsUtils import *
from RevBuildingsUtils import *

import BugCore

# globals
GC = CyGlobalContext()
GAME = GC.getGame()
RevOpt = BugCore.game.Revolution

gameSpeedMod = None

revInstigatorThreshold = 1000
deniedTurns = 5

########################## Initialization ###############################

def init():
	global revInstigatorThreshold, deniedTurns, gameSpeedMod

	gameSpeedMod = None

	revInstigatorThreshold = RevOpt.getInstigateRevolutionThreshold()

	deniedTurns = RevOpt.getDeniedTurns()

########################## Generic helper functions ###############################

def getGameSpeedMod():
	# Ratio of game turns to those of Epic, limited adjustment for extremely short/long differences
	global gameSpeedMod
	if gameSpeedMod == None:
		CvGameSpeedInfo = GC.getGameSpeedInfo(GAME.getGameSpeedType())
		gameSpeedMod = CvGameSpeedInfo.getSpeedPercent() + CvGameSpeedInfo.getHammerCostPercent()
		gameSpeedMod = 200.0 / gameSpeedMod
	return gameSpeedMod


def doRefortify(iPlayer):
	for pGroup in GC.getPlayer(iPlayer).groups():
		if pGroup.getNumUnits() > 0:
			headUnit = pGroup.getHeadUnit()
			if headUnit.getFortifyTurns() > 0:
				pGroup.setActivityType(ActivityTypes.ACTIVITY_SLEEP)
				headUnit.NotifyEntity(MissionTypes.MISSION_FORTIFY)


def plotGenerator( startPlot, maxRadius ) :
	# To be used as: for [radius,plot] in RevUtils.plotGenerator(plot,5) :
	# Returns plots starting at radius 1 and up to max Radius

	# Start with center plot
	yield [0,startPlot]

	radius = 1
	gameMap = GC.getMap()
	# Expand radius slowly, searching concentric squares
	while( radius <= maxRadius ) :
		# Top and bottom rows
		for ix in xrange(startPlot.getX()-radius,startPlot.getX()+radius+1) :
			for iy in [startPlot.getY() - radius, startPlot.getY() + radius] :

				if( ix < 0 ) :
					if( gameMap.isWrapX() ) :
						ix = CyMap().getGridWidth() + ix
					else :
						continue
				elif( ix >= CyMap().getGridWidth() ) :
					if( gameMap.isWrapX() ) :
						ix = ix - CyMap().getGridWidth()
					else :
						continue

				if( iy < 0 ) :
					if( gameMap.isWrapY() ) :
						iy = CyMap().getGridHeight() + iy
					else :
						continue
				elif( iy >= CyMap().getGridHeight() ) :
					if( gameMap.isWrapY() ) :
						iy = iy - CyMap().getGridHeight()
					else :
						continue

				yield [radius,gameMap.plot(ix,iy)]

		# Left and right columns (leave out corners)
		for ix in [startPlot.getX()-radius,startPlot.getX()+radius] :
			for iy in xrange(startPlot.getY() - radius + 1, startPlot.getY() + radius) :

				if( ix < 0 ) :
					if( gameMap.isWrapX() ) :
						ix = CyMap().getGridWidth() + ix
					else :
						continue
				elif( ix >= CyMap().getGridWidth() ) :
					if( gameMap.isWrapX() ) :
						ix = ix - CyMap().getGridWidth()
					else :
						continue

				if( iy < 0 ) :
					if( gameMap.isWrapY() ) :
						iy = CyMap().getGridHeight() + iy
					else :
						continue
				elif( iy >= CyMap().getGridHeight() ) :
					if( gameMap.isWrapY() ) :
						iy = iy - CyMap().getGridHeight()
					else :
						continue

				yield [radius,gameMap.plot(ix,iy)]

		radius += 1

def getNumDefendersNearPlot( iPlotX, iPlotY, iPlayer, iRange = 2, bIncludePlot = True, bIncludeCities = False ) :
	# bIncludePlot takes precedence over bIncludeCities
	iNumUnits = 0

	gameMap = GC.getMap()
	basePlot = gameMap.plot(iPlotX,iPlotY)

	for [radius,pPlot] in plotGenerator( basePlot, iRange ) :

		if( pPlot.getX() == iPlotX and pPlot.getY() == iPlotY ) :
			if( not bIncludePlot ) :
				continue
		elif( pPlot.isCity() and not bIncludeCities ) :
			continue

		iNumUnits += pPlot.getNumDefenders( iPlayer )

	return iNumUnits


def getClosestCityXY( iPlotX, iPlotY, iPlayer, maxRange = 10, bIncludeBase = True ) :

	gameMap = GC.getMap()
	basePlot = gameMap.plot(iPlotX,iPlotY)

	for [radius,pPlot] in plotGenerator( basePlot, maxRange ) :
		if( radius == 0 and not bIncludeBase ) :
			continue
		if( pPlot.isCity() ) :
			if( pPlot.getOwner() == iPlayer ) :
				return [pPlot.getX(),pPlot.getY()]

	return None

def getSpawnablePlots( iPlotX, iPlotY, pSpawnPlayer, bLand = True, bIncludePlot = True, bIncludeCities = False, bIncludeForts = False, bSameArea = True, iRange = 2, iSpawnPlotOwner = -1, bCheckForEnemy = True, bAtWarPlots = True, bOpenBordersPlots = True ) :

	spawnablePlots = []

	gameMap = GC.getMap()
	basePlot = gameMap.plot(iPlotX,iPlotY)

	iFort = GC.getInfoTypeForString('IMPROVEMENT_FORT')

	try: iBaseArea = basePlot.area().getID()
	except AttributeError:
		if bSameArea: print "WARNING: Passed an arealess plot!"
		iBaseArea = -1
		bSameArea = False

	iBasePlotOwner = basePlot.getOwner()
	iNumPlotsChecked = 0

	for [radius, pPlot] in plotGenerator(basePlot, iRange):

			if pPlot.isImpassable() or not bIncludePlot and pPlot.getX() == iPlotX and pPlot.getY() == iPlotY:
				continue

			iNumPlotsChecked += 1

			if (bLand == pPlot.isWater()
			or not bIncludeCities and pPlot.isCity()
			or bSameArea and iBaseArea != pPlot.area().getID()
			or bCheckForEnemy and len(getEnemyUnits(pPlot.getX(), pPlot.getY(), pSpawnPlayer.getID())) > 0
			or not bIncludeForts and iFort != -1 and pPlot.getImprovementType() == iFort
			): continue

			# When iSpawnPlotOwner > -1, plot owner must be either iSpawnPlotOwner, iBasePlotOwner, or no one
			if (iSpawnPlotOwner < 0 or pPlot.getOwner() in (iSpawnPlotOwner, iBasePlotOwner, -1)
			or
				bAtWarPlots and GC.getTeam(pSpawnPlayer.getTeam()).isAtWar(GC.getPlayer(pPlot.getOwner()).getTeam())
			or
				bOpenBordersPlots and GC.getTeam(pSpawnPlayer.getTeam()).isOpenBorders(GC.getPlayer(pPlot.getOwner()).getTeam())
			):
				spawnablePlots.append([pPlot.getX(), pPlot.getY()])

	return spawnablePlots

def getEnemyUnits( iPlotX, iPlotY, iEnemyOfPlayer, domain = -1, bOnlyMilitary = False ) :

	pEnemyOfTeam = GC.getTeam( GC.getPlayer(iEnemyOfPlayer).getTeam() )

	enemyUnits = []

	for pUnit in GC.getMap().plot(iPlotX,iPlotY).units():
		pUnitTeam = GC.getTeam( pUnit.getTeam() )
		if( pEnemyOfTeam.isAtWar(pUnit.getTeam()) ) :
			if( domain < 0 or pUnit.getDomainType() == domain ) :
				if( not bOnlyMilitary or pUnit.canFight() ) :
					enemyUnits.append( pUnit )

	return enemyUnits

def getPlayerUnits( iPlotX, iPlotY, iPlayer, domain = -1 ) :

	playerUnits = []

	for pUnit in GC.getMap().plot(iPlotX,iPlotY).units():
		if( pUnit.getOwner() == iPlayer ) :
			if( domain < 0 or pUnit.getDomainType() == domain ) :
				playerUnits.append( pUnit )

	return playerUnits


def moveEnemyUnits( iPlotX, iPlotY, iEnemyOfPlayer, iMoveToX, iMoveToY, iInjureMax = 0, bDestroyNonLand = True, bLeaveSiege = False ) :

	unitList = getEnemyUnits( iPlotX, iPlotY, iEnemyOfPlayer )

	if( iInjureMax > 0 ) :
		for pUnit in unitList :
			if( pUnit.canFight() ) :
				iPreDamage = pUnit.getDamage()
				iInjure = iPreDamage/3 + iInjureMax/2 + GAME.getSorenRandNum(iInjureMax/2,'Rev: Wound retreating units')
				iInjure = min([iInjure,90])
				iInjure = max([iInjure,iPreDamage])
				pUnit.setDamage( iInjure, iEnemyOfPlayer )

	pPlot = GC.getMap().plot(iMoveToX,iMoveToY)

	toKillList = []
	for pUnit in unitList :
		if not pUnit.getDomainType() == DomainTypes.DOMAIN_LAND or not pUnit.canMoveInto(pPlot,False,False,True):
			if bDestroyNonLand:
				toKillList.append(pUnit)

		elif not (bLeaveSiege and pUnit.bombardRate() > 0):
			pUnit.setXY(iMoveToX, iMoveToY, False, False, False)

	for pUnit in toKillList :
		if pUnit is not None:
			pUnit.kill(False,iEnemyOfPlayer)


def moveEnemyUnits2( iPlotX, iPlotY, iEnemyOfPlayer, iMoveToX, iMoveToY, iInjureMax = 0, bMoveAir = True, bLeaveSiege = False ) :

	unitList = getEnemyUnits( iPlotX, iPlotY, iEnemyOfPlayer )

	if( iInjureMax > 0 ) :
		for pUnit in unitList :
			if( pUnit.canFight() ) :
				iPreDamage = pUnit.getDamage()
				iInjure = iPreDamage/3 + iInjureMax/2 + GAME.getSorenRandNum(iInjureMax/2,'Rev: Wound retreating units')
				iInjure = min([iInjure,90])
				iInjure = max([iInjure,iPreDamage])
				pUnit.setDamage( iInjure, iEnemyOfPlayer )

	pPlot = GC.getMap().plot(iMoveToX,iMoveToY)

	for pUnit in unitList :
		if pUnit.getDomainType() == DomainTypes.DOMAIN_LAND or (bMoveAir and pUnit.getDomainType() == DomainTypes.DOMAIN_AIR):

			if bLeaveSiege and pUnit.getDomainType() == DomainTypes.DOMAIN_LAND and pUnit.bombardRate() > 0: continue

			if pUnit.getDomainType() == DomainTypes.DOMAIN_AIR:
				if pPlot.isCity() or pUnit.canMoveInto(pPlot,False,False,True):
					pUnit.setXY( iMoveToX, iMoveToY, False, False, False )

			else: pUnit.setXY( iMoveToX, iMoveToY, False, False, False )


def clearOutCity( pCity, pPlayer, pEnemyPlayer ) :

	ix = pCity.getX()
	iy = pCity.getY()

	moveXY = getClosestCityXY( ix, iy, pPlayer.getID(), 25, bIncludeBase = False )
	if( moveXY == None ) :
		retreatPlots = getSpawnablePlots( ix, iy, pPlayer, bLand = True, bIncludePlot = False, bIncludeCities = True, bSameArea = True, iRange = 3, iSpawnPlotOwner = pPlayer.getID(), bCheckForEnemy = True )
		if( len(retreatPlots) == 0 ) :
			retreatPlots = getSpawnablePlots( ix, iy, pPlayer, bLand = True, bIncludePlot = False, bIncludeCities = True, bSameArea = False, iRange = 5, iSpawnPlotOwner = -1, bCheckForEnemy = True )

		if( len(retreatPlots) > 0 ) :
			moveXY = retreatPlots[GAME.getSorenRandNum(len(retreatPlots),'Rev')]

	if( not moveXY == None ) :
		moveEnemyUnits2( ix, iy, pEnemyPlayer.getID(), moveXY[0], moveXY[1], bMoveAir = True )

		# Handle water units
		waterUnits = getEnemyUnits( ix, iy, pEnemyPlayer.getID(), domain = DomainTypes.DOMAIN_SEA )

		if( len(waterUnits) > 0 ) :
			retreatPlots = getSpawnablePlots( ix, iy, pPlayer, bLand = False, bIncludePlot = False, bIncludeCities = False, bSameArea = False, iRange = 1, iSpawnPlotOwner = pPlayer.getID(), bCheckForEnemy = True )
			if( len(retreatPlots) == 0 ) :
				retreatPlots = getSpawnablePlots( ix, iy, pPlayer, bLand = False, bIncludePlot = False, bIncludeCities = False, bSameArea = False, iRange = 5, iSpawnPlotOwner = -1, bCheckForEnemy = True )
			if( len(retreatPlots) > 0 ) :
				moveXY = retreatPlots[GAME.getSorenRandNum(len(retreatPlots),'Rev')]
				for unit in waterUnits :
					if( unit.canMoveInto(GC.getMap().plot(moveXY[0],moveXY[1]),False,False,True) ) :
						unit.setXY( moveXY[0], moveXY[1], False, False, False )


########################## Revolution helper functions ###############################


def getHandoverUnitTypes(CyCity):

	iBestDefender = UnitTypes.NO_UNIT
	iCounter = UnitTypes.NO_UNIT
	iAttack = UnitTypes.NO_UNIT

	for iUnit in xrange(GC.getNumUnitInfos()):
		CvUnitInfo = GC.getUnitInfo(iUnit)

		if CvUnitInfo.getDomainType() != DomainTypes.DOMAIN_LAND or CvUnitInfo.getPrereqAndTech() == TechTypes.NO_TECH:
			continue
		if CvUnitInfo.getMaxGlobalInstances() > 0 or CvUnitInfo.getMaxPlayerInstances() > 0:
			continue

		if not CyCity.canTrain(iUnit, False, False, False, False): continue

		# Defender (Archer,Longbow)
		if CvUnitInfo.getDefaultUnitAIType() == UnitAITypes.UNITAI_CITY_DEFENSE:
			if iBestDefender == UnitTypes.NO_UNIT or CvUnitInfo.getCombat() >= GC.getUnitInfo(iBestDefender).getCombat():
				iBestDefender = iUnit

		# Counter (Axemen,Phalanx)
		if CvUnitInfo.getUnitAIType(UnitAITypes.UNITAI_COUNTER):
			if iCounter == UnitTypes.NO_UNIT or CvUnitInfo.getCombat() >= GC.getUnitInfo(iCounter).getCombat():
				iCounter = iUnit

		# Assault units
		if CvUnitInfo.getUnitAIType(UnitAITypes.UNITAI_ATTACK):
			if iAttack == UnitTypes.NO_UNIT or CvUnitInfo.getCombat() > GC.getUnitInfo(iAttack).getCombat():
				iAttack = iUnit

	if iBestDefender == UnitTypes.NO_UNIT:
		if not iCounter == UnitTypes.NO_UNIT:
			iBestDefender = iCounter
		else:
			iBestDefender = GC.getInfoTypeForString("UNIT_CLUBMAN")
	if iCounter == UnitTypes.NO_UNIT: iCounter = iBestDefender
	if iAttack == UnitTypes.NO_UNIT: iAttack = iCounter

	return [GC.getUNIT_WORKER(), iBestDefender, iCounter, iAttack]

def getUprisingUnitTypes(CyCity):
	# Returns list of units that can be given to violent rebel uprisings, odds of giving are set by the relative number of times a unit type appears in list
	aList = []
	for iUnit in xrange(GC.getNumUnitInfos()):
		CvUnitInfo = GC.getUnitInfo(iUnit)

		if CvUnitInfo.getDomainType() != DomainTypes.DOMAIN_LAND:
			continue

		if CvUnitInfo.getMaxGlobalInstances() > 0 or CvUnitInfo.getMaxPlayerInstances() > 0:
			continue

		iCombat = CvUnitInfo.getCombat()
		if iCombat < 1: continue

		if not CvUnitInfo.hasUnitCombat(UnitCombatTypes(GC.getInfoTypeForString("UNITCOMBAT_COMBATANT"))):
			continue

		if CyCity.canTrain(iUnit, False, False, False, False):
			for i in xrange(iCombat/4 + 1):
				aList.append(iUnit)
	return aList


def computeWarOdds(CyPlayerA, CyPlayerB, CyArea, allowAttackerVassal=True, allowVictimVassal=True, allowBreakVassal=True):

		iTeamA = CyPlayerA.getTeam() # Aggressor
		iTeamB = CyPlayerB.getTeam() # Victim
		CyTeamA = GC.getTeam(iTeamA)
		CyTeamB = GC.getTeam(iTeamB)

		if iTeamA == iTeamB:
			return [-50, CyTeamA, CyTeamB]

		if CyTeamA.isAtWar(iTeamB):
			return [100, CyTeamA, CyTeamB]

		warOdds = 0

		if CyTeamA.isAVassal():
			if not allowAttackerVassal:
				return [-50, CyTeamA, CyTeamB]

			if CyTeamA.isVassal(iTeamB):
				if not allowBreakVassal:
					return [-50, CyTeamA, CyTeamB]
				else: # Allow vassal to rebel!!!
					warOdds -= 20
			else: # Find de facto aggressor
				warOdds -= 10
				for iTeam in xrange(GC.getMAX_PC_TEAMS()) :
					if CyTeamA.isVassal(iTeam):
						iTeamA = iTeam
						CyTeamA = GC.getTeam(iTeam)
						CyPlayerA = GC.getPlayer(CyTeamA.getLeaderID())

		if CyTeamB.isAVassal():
			if not allowVictimVassal:
				return [-50, CyTeamA, CyTeamB]

			if CyTeamB.isVassal(iTeamA):
				if not allowBreakVassal:
					return [-50, CyTeamA, CyTeamB]
				else: # Allow master to attack vassal
					warOdds -= 20
			else: # Find de facto victim
				for iTeam in xrange(GC.getMAX_PC_TEAMS()):
					if CyTeamB.isVassal(iTeam):
						iTeamB = iTeam
						CyTeamB = GC.getTeam(iTeam)
						CyPlayerB = GC.getPlayer(CyTeamB.getLeaderID())
						break

		iPlayerA = CyPlayerA.getID()
		iPlayerB = CyPlayerB.getID()
		eAttitude = CyPlayerA.AI_getAttitude(iPlayerB)
		if eAttitude == AttitudeTypes.ATTITUDE_FURIOUS:
			warOdds += 50
		elif eAttitude == AttitudeTypes.ATTITUDE_ANNOYED:
			warOdds += 25
		elif eAttitude == AttitudeTypes.ATTITUDE_PLEASED:
			warOdds -= 25
		elif eAttitude == AttitudeTypes.ATTITUDE_FRIENDLY:
			warOdds -= 50

		iAreaPowerB = CyArea.getPower(iPlayerB)
		if not iAreaPowerB:
			iPowerB = CyPlayerB.getPower()
			if not iPowerB:
				powerFrac = 2
			else: powerFrac = 1.0*CyPlayerA.getPower() / iPowerB + .2
		else:
			powerFrac = CyArea.getPower(iPlayerA)/(1.0*iAreaPowerB)

		iAreaCitiesA = CyArea.getCitiesPerPlayer(iPlayerA)
		if iAreaCitiesA > 1 and powerFrac > 1.6:
			warOdds += 50
		elif powerFrac > 1.5:
			warOdds += 35
		elif powerFrac > 1.2:
			warOdds += 20
		elif iAreaCitiesA < 1 or powerFrac < .7:
			warOdds -= 40
		elif powerFrac > 1.0:
			warOdds += 10
		else: warOdds -= 25

		if iAreaCitiesA > CyArea.getCitiesPerPlayer(iPlayerB):
			warOdds += 10

		if CyPlayerA.hasTrait(GC.getInfoTypeForString("TRAIT_AGGRESSIVE")):
			warOdds += 10

		if CyPlayerA.isRebel():
			warOdds += 10

		if warOdds > 100:
			warOdds = 100

		return [warOdds, CyTeamA, CyTeamB]


# Give all techs known by fromPlayer, except a few of the most expensive
def giveTechs(toPlayer, fromPlayer):

	toPlayerTeam = GC.getTeam(toPlayer.getTeam())
	bSolo = toPlayerTeam.getNumMembers() == 1
	if not bSolo: return

	knownTechs = ()
	iMinCostly = 0
	iNumCostly = 5 + GAME.getSorenRandNum(4, 'Techs')
	costlyTechs = [(-1,0),]*iNumCostly

	fromPlayerTeam = GC.getTeam(fromPlayer.getTeam())
	iPlayer = toPlayer.getID()

	# Remove all techs from toPlayer and record what can be inherited.
	for iTech in xrange(GC.getNumTechInfos()):

		if toPlayerTeam.isHasTech(iTech):
			toPlayerTeam.setHasTech(iTech, False, iPlayer, False, False)

		if fromPlayerTeam.isHasTech(iTech):
			knownTechs += (iTech,)
			iCost = GC.getTechInfo(iTech).getResearchCost()
			if iCost > iMinCostly:
				iMin = iCost
				for i in xrange(iNumCostly):
					iTechX, iCostX = costlyTechs[i]
					if iCostX == iMinCostly:
						costlyTechs[i] = (iTech, iCost)
					elif iCostX < iMin:
						iMin = iCostX
				iMinCostly = iMin

	# Reset tech score, removing techs does not reduce tech score...
	iTemp = toPlayer.getTechScore()
	if iTemp:
		toPlayer.changeTechScore(-iTemp)

	# Simplify costly tech list
	bestTechs = ()
	for i in xrange(iNumCostly):
		bestTechs += (costlyTechs[i][0],)

	for iTech in knownTechs:
		if iTech in bestTechs:
			toPlayerTeam.setResearchProgress(iTech, GAME.getSorenRandNum(int(toPlayerTeam.getResearchCost(iTech)*.75), 'Free research'), iPlayer)
		else: toPlayerTeam.setHasTech(iTech, True, iPlayer, False, False)


########################## City helper functions ###############################

def giveCityCulture(CyCity, iPlayer, newCityVal, newPlotVal):
	# Places this culture value in city and city plot
	# Places half this value in neighboring plots

	if iPlayer < 0 or iPlayer >= GC.getMAX_PC_PLAYERS():
		return
	CyPlot = CyCity.plot()

	if newCityVal > CyCity.getCulture(iPlayer):
		CyCity.setCulture(iPlayer, newCityVal, True)

	if CyCity.getCultureLevel() > 2:
		culRadius = 3
	else: culRadius = 2

	for radius, CyPlotX in plotGenerator(CyPlot, culRadius):
		if radius:
			newPlotCul = newPlotVal / (2*radius)
		else: newPlotCul = newPlotVal

		if newPlotCul > CyPlotX.getCulture(iPlayer):
			CyPlotX.setCulture(iPlayer, newPlotCul, True)

def isCanBribeCity(CyCity):

	iRevIdx = CyCity.getRevolutionIndex()

	if iRevIdx > 1700:
		return [False, 'Violent']

	if iRevIdx < 450 and CyCity.getLocalRevIndex() < 8:
		return [False, 'No Need']

	return [True, None]

def computeBribeCosts(CyCity):
	iTurn = GAME.getGameTurn()

	turnBribeData = RevData.getCityVal(CyCity, 'TurnBribeCosts')
	if turnBribeData != None and iTurn == turnBribeData[0]:
		return [turnBribeData[1][0], turnBribeData[1][1], turnBribeData[1][2]]

	# Compute costs to bribe rebels at three levels
	# Start by computing a base cost based on players economy strength
	iPlayer = CyCity.getOwner()
	CyPlayer = GC.getPlayer(iPlayer)

	iRevIdx = CyCity.getRevolutionIndex()
	localRevIdx = CyCity.getLocalRevIndex()

	iPop = CyCity.getPopulation()
	fBaseCost = (iRevIdx + 16*localRevIdx + 3*CyCity.getNumRevolts(iPlayer)) * (iPop**1.1)/8.0

	fMod = (1 + CyPlayer.getCurrentEra() - 9 / (8.1 + iPop**1.3)) / 3
	fMod *= GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent() / 100.0

	if not CyPlayer.isHuman():
		fMod /= 2

	iCost = int(fMod*fBaseCost)
	if iCost < 3:
		iCost = 3
	aList = [2*iCost/3, iCost, 5*iCost/3]
	RevData.setCityVal(CyCity, 'TurnBribeCosts', [iTurn, aList])

	return aList

def bribeCity(CyCity, bribeSize):

	if bribeSize == 'Small':
		# Small reduction in rev index, mostly just for buyoffturns
		newRevIdx = int( 0.9*CyCity.getRevolutionIndex() - 10 )
		if newRevIdx < 0:
			newRevIdx = 0
		CyCity.changeRevolutionCounter(5)
	elif bribeSize == 'Med':
		# Med reduction in rev index
		newRevIdx = int(0.8*CyCity.getRevolutionIndex() - 25)
		if newRevIdx < 0:
			newRevIdx = 0
		CyCity.changeRevolutionCounter(7)
	elif bribeSize == 'Large':
		# Large reduction in rev index, longer time till next revolt too
		newRevIdx = int( 0.7*CyCity.getRevolutionIndex() - 50 )
		if newRevIdx < 0:
			newRevIdx = 0
		CyCity.changeRevolutionCounter(10)
	else:
		print 'Error!  Unrecognized bribeSize string %s' % bribeSize
		return
	CyCity.setRevolutionIndex(newRevIdx)
	RevData.setCityVal(CyCity, 'BribeTurn', GAME.getGameTurn())
	RevData.setCityVal(CyCity, 'TurnBribeCosts', None)

########################## RevIndex helper functions #####################

def getModNumUnhappy(CyCity, fWarWearinessMod):
	iPop = CyCity.getPopulation()

	iMod = int(fWarWearinessMod*iPop*CyCity.getWarWearinessPercentAnger()/1000)

	iNumUnhappy = CyCity.angryPopulation(0) - iMod - 1

	if iNumUnhappy < 1:
		return CyCity.unhappyLevel(0) - CyCity.happyLevel()
	return iNumUnhappy

def doRevRequestDeniedPenalty(CyCity, iHomeArea, iRevIdxInc=100, bExtraHomeland=False, bExtraColony=False):

	iLocalRevIdx = CyCity.getLocalRevIndex()
	if iLocalRevIdx > 20:
		iLocalRevIdx = 20
	bHome = CyCity.area().getID() == iHomeArea

	if bExtraColony and not bHome or bExtraHomeland and bHome:
		fChange = 1.5*iRevIdxInc + 12*iLocalRevIdx
		fMin = .75*iRevIdxInc
		if fChange < fMin:
			fChange = fMin
		iChange = int(fChange)
	else:
		iChange = iRevIdxInc + 12*iLocalRevIdx
		iMin = int(.75*iRevIdxInc)
		if iChange < iMin:
			iChange = iMin
	CyCity.changeRevolutionIndex(iChange)

	iAngerTimer = CyCity.getRevRequestAngerTimer()
	iMax = 3*deniedTurns
	if iAngerTimer < iMax:
		iChange = iMax - iAngerTimer
		iMax = 2*deniedTurns
		if iChange > iMax:
			iChange = iMax
		CyCity.changeRevRequestAngerTimer(iChange)
	CyCity.changeRevolutionCounter(deniedTurns)

def computeCivSizeRaw(iPlayer) :
	# Ratio of amount of land player owns to what would be equal for this map for national effects, effective radius of civ's empire for comparing with city distance
	fPlotsRatio = 1.0*CyMap().getLandPlots() / GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
	iOwnedPlots = GC.getPlayer(iPlayer).getTotalLand()

	fSizeValueRaw = iOwnedPlots / fPlotsRatio
	fCivEffRadRaw = ((.5*iOwnedPlots + .5*fPlotsRatio) / 3.4)**.5

	return [fSizeValueRaw, fCivEffRadRaw]

def computeCivSize(iPlayer):
	CyPlayer = GC.getPlayer(iPlayer)
	# Ratio of amount of land player owns to what would be equal for this map for national effects, effective radius of civ's empire for comparing with city distance
	fPlotsRatio = 1.0*CyMap().getLandPlots() / GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()
	iOwnedPlots = CyPlayer.getTotalLand()

	civSizeEraMod = 0.85 - 0.20 * CyPlayer.getCurrentEra()
	if civSizeEraMod < 0:
		civSizeEraMod = 0
	fCivSizeValue  = iOwnedPlots / fPlotsRatio + civSizeEraMod
	fCivEffRadius = ((.5*iOwnedPlots + .5*fPlotsRatio) / 3.4)**.5

	return [fCivSizeValue, fCivEffRadius]


########################## Player modification functions ###########################################

# Changes specified players civ, leader. Does not change isHuman setting
def changeCiv(playerIdx, newCivType = -1, newLeaderType = -1, teamIdx = -1):

	player = GC.getPlayer(playerIdx)
	oldCivType = player.getCivilizationType()
	oldLeaderType = player.getLeaderType()
	if newCivType >= 0 and not newCivType == oldCivType:
		player.changeCiv(newCivType)
	if newLeaderType >= 0 and not newLeaderType == oldLeaderType:
		player.setName("")
		player.changeLeader(newLeaderType)

	return True

# Changes leader personality of this civ
def changePersonality(playerIdx, newPersonality = -1):
	if newPersonality < 0:
		iBestValue = 0
		newPersonality = -1

		for iI in xrange(GC.getNumLeaderHeadInfos()) :
			if (not iI == GC.getDefineINT("BARBARIAN_LEADER")) :
				iValue = (1 + GAME.getSorenRandNum(10000, "Choosing Personality"))

				for iJ in xrange(GC.getMAX_PC_PLAYERS()) :
					if (GC.getPlayer(iJ).isEverAlive()) :
						if (GC.getPlayer(iJ).getPersonalityType() == iI) :
							iValue /= 2

				if iValue > iBestValue:
					iBestValue = iValue
					newPersonality = iI

	if newPersonality >= 0 and newPersonality < GC.getNumLeaderHeadInfos():
		GC.getPlayer(playerIdx).setPersonalityType(newPersonality)

def changeHuman(newHumanIdx, oldHumanIdx):
	GAME.changeHumanPlayer(oldHumanIdx, newHumanIdx)
	doRefortify(newHumanIdx)
	return True
