#-#
# BARBARIAN_CIV_MOD
'''
New world policy controls handling of barbcivs starting away from the original civs in the game.
	RevOpt.getNewWorldPolicy()
	1: A minor civ in the New World needs contact with a major civ to become a major civ, each contact increase the odds for it.
	2: Like 1, but a major civ must have units in the New World for major civs to emerge there.
	3: No civs from barbarian in the New World unless an Old World civ have units on the landmass.
	4: Like 3, but the requirement is cities instead of units.
'''

from CvPythonExtensions import *
import SdToolKit as SDTK
from CvUtil import sendMessage

# globals
GC = CyGlobalContext()
MAP = GC.getMap()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

class BarbarianCiv:

	def __init__(self, customEM, RevOpt):
		self.RevOpt = RevOpt
		self.customEM = customEM
		self.BARBARIAN_PLAYER = GC.getBARBARIAN_PLAYER()
		self.MAX_PC_PLAYERS = GC.getMAX_PC_PLAYERS()

		self.customEM.addEventHandler("BeginGameTurn", self.onBeginGameTurn)

	def removeEventHandlers(self):
		self.customEM.removeEventHandler("BeginGameTurn", self.onBeginGameTurn)

	# Called at the beginning of the end of each turn
	def onBeginGameTurn(self, argsList):
		#iGameTurn, = argsList
		'''
		Check if any minor civs will become major.
		'''
		MAX_PC_PLAYERS = self.MAX_PC_PLAYERS

		for iPlayerX in xrange(MAX_PC_PLAYERS):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive() and CyPlayerX.isMinorCiv():
				self.checkMinorCivs(iPlayerX, CyPlayerX, MAX_PC_PLAYERS)
		'''
		Check if any barbarian cities will become a minor civ.
		'''
		fMod = self.RevOpt.getFormMinorModifier()
		if not fMod:
			return
		maxCivs = self.RevOpt.getBarbCivMaxCivs()
		if maxCivs < 1 or maxCivs > MAX_PC_PLAYERS:
			maxCivs = MAX_PC_PLAYERS
		if GAME.countCivPlayersAlive() >= maxCivs: return

		iPlayerBarb = self.BARBARIAN_PLAYER
		CyPlayerBarb = GC.getPlayer(iPlayerBarb)
		iNumCities = CyPlayerBarb.getNumCities()
		if iNumCities < 2: return

		# Increase odds per barb city within reason.
		fMod *= iNumCities ** .5
		# Gamespeed factor
		iFactorGS = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent()
		iRange = 16*iFactorGS
		iEra = GAME.getCurrentEra()

		iPolicy = self.RevOpt.getNewWorldPolicy()
		iMinPop = self.RevOpt.getMinPopulation() + iEra

		bNoGo = True
		for CyCity in CyPlayerBarb.cities():
			iPop = CyCity.getPopulation()
			if iPop >= iMinPop:

				CyArea = CyCity.area()

				aList = self.getScenario(CyArea)
				if not aList: continue
				bNewWorld, bMajorCivCities, bMajorCivUnits = aList

				if bNewWorld:
					if iPolicy > 2 and not bMajorCivUnits:
						continue
					elif iPolicy > 3 and not bMajorCivCities:
						continue

				fOdds = 3*(1.0 + iPop - iMinPop)

				iTemp = CyCity.getCultureLevel()
				if iTemp > 0:
					fOdds += 16*iTemp

				if CyCity.getOriginalOwner() != iPlayerBarb:
					fOdds += 50

				iTemp = CyArea.getNumCities()
				if iTemp > 0:
					fOdds += iTemp

				if bNewWorld and not bMajorCivUnits:
					fOdds /= 4.0

				fOdds *= fMod
				if GAME.getSorenRandNum(int(iRange+fOdds), 'Barbarian city evolve') < fOdds:
					bNoGo = False
					break

		if bNoGo: return
		'''
		Create Minor Civ
		'''
		POST_FIX = "\n\tBarbarianCiv.onBeginGameTurn"
		iPlayer = None
		iCivType = -1
		# Pick a vacant player slot
		CyPlayerCulture = None
		iCulture = 0
		iDeadCulture = 0
		aList = []
		# The dead civ with the highest culture in the city gets precedence.
		for iPlayerX in xrange(MAX_PC_PLAYERS):
			CyPlayerX = GC.getPlayer(iPlayerX)
			# Find player with highest culture.
			iCult = CyCity.getCulture(iPlayerX)
			if iCult > iCulture:
				iCulture = iCult
				CyPlayerCulture = CyPlayerX
			if CyPlayerX.isAlive(): continue
			if CyPlayerX.isEverAlive():
				if iCult > iDeadCulture:
					iDeadCulture = iCult
					aList = [iPlayerX, CyPlayerX]
			elif iPlayer is None and not SDTK.sdObjectExists('Revolution', CyPlayerX):
				# Empty slot
				iPlayer = iPlayerX
				CyPlayer = CyPlayerX
		iNumTechs = GC.getNumTechInfos()
		techsOwned = []
		if aList:
			iPlayer, CyPlayer = aList
			iCivType = CyPlayer.getCivilizationType()
			if not bNewWorld:
				CyTeam = GC.getTeam(CyPlayer.getTeam())
				for iTech in xrange(iNumTechs):
					if CyTeam.isHasTech(iTech):
						techsOwned.append(iTech)
			print "[INFO] Reincarnating dead player" + POST_FIX

		elif iPlayer is None:
			print "[WARNING] No available player slot found." + POST_FIX
			return

		if iCivType < 0:
			# Choose a civ for the new player
			aList = [] # Claimed civs
			for iPlayerX in xrange(MAX_PC_PLAYERS):
				if iPlayerX == iPlayer: continue
				iCivType = GC.getPlayer(iPlayerX).getCivilizationType()
				if iCivType > -1:
					aList.append(iCivType)

			civs = [] # Available civs
			for iCivType in xrange(GC.getNumCivilizationInfos()):
				if iCivType in aList: continue
				CvCivInfo = GC.getCivilizationInfo(iCivType)
				if not CvCivInfo.isPlayable(): continue
				civs.append(iCivType)

			if civs:
				# Civs with similar style to CyPlayerCulture, if they exist.
				if CyPlayerCulture:
					aList = []
					iArtStyle = GC.getCivilizationInfo(CyPlayerCulture.getCivilizationType()).getArtStyleType()
					for iCivType in civs:
						if GC.getCivilizationInfo(iCivType).getArtStyleType() == iArtStyle:
							aList.append(iCivType)
					if aList:
						civs = aList
				iCivType = civs[GAME.getSorenRandNum(len(civs),'Pick civ')]

			if iCivType < 0:
				print "[WARNING] Unexpected lack of unused civ types." + POST_FIX
				return

		# Choose a leader for the new civ
		aList = [] # Claimed Leaders
		for iPlayerX in xrange(MAX_PC_PLAYERS):
			if iPlayerX == iPlayer: continue
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive():
				iLeader = CyPlayerX.getLeaderType()
				if iLeader > -1: aList.append(iLeader)

		bLeadAnyCiv = GAME.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV)
		leaders = []
		for iLeader in xrange(GC.getNumLeaderHeadInfos()):
			if iLeader not in aList and not GC.getLeaderHeadInfo(iLeader).isNPC() and (bLeadAnyCiv or GC.getCivilizationInfo(iCivType).isLeaders(iLeader)):
				leaders.append(iLeader)

		if not leaders:
			print "[ERROR] Unexpected lack of possible leaders." + POST_FIX
			return
		iLeader = leaders[GAME.getSorenRandNum(len(leaders), 'Pick leader')]

		iX = CyCity.getX()
		iY = CyCity.getY()
		CyPlot = CyCity.plot()

		print "[INFO] Adding new player in slot %d.%s" %(iPlayer, POST_FIX)
		szCityName = CyCity.getName()

		# Add player to game
		GAME.addPlayer(iPlayer, iLeader, iCivType, False) # This resets player data.

		CyTeam = GC.getTeam(CyPlayer.getTeam())

		CyPlayer.setNewPlayerAlive(True)

		civName = CyPlayer.getCivilizationDescription(0)
		print "[BarbCiv] %s has emerged in %s" %(civName, szCityName)
		print "[BarbCiv] bNewWorld = " + str(bNewWorld)

		# Add replay message
		mess = TRNSLTR.getText("TXT_KEY_BARBCIV_FORM_MINOR", ()) %(civName, szCityName)
		mess = mess[0].capitalize() + mess[1:]
		GAME.addReplayMessage(ReplayMessageTypes.REPLAY_MESSAGE_MAJOR_EVENT, iPlayer, mess, iX, iY, GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"))

		# Using following method to acquire city produces 'revolted and joined' replay messages
		if CyCity.getOriginalOwner() == iPlayerBarb:
			CyCity.setOriginalOwner(iPlayer)
		CyPlot.setOwner(iPlayer)

		# Note: city owner change (CyPlot.setOwner(iPlayer)) invalidate previous city pointer.
		CyCity = CyPlayer.getCapitalCity()

		iTemp = iFactorGS
		if iEra and not (bNewWorld and not bMajorCivUnits):
			iTemp *= iEra + 1
		CyPlayer.changeGold(iTemp)

		closeTeams = self.getCloseCivs(iPlayer, CyArea, iX, iY)
		# Give techs to new player, with variables for extra techs for builders.
		if bNewWorld:
			iMinEra = iEra - self.RevOpt.getNewWorldErasBehind()
			if iMinEra > -1:
				for iTech in xrange(iNumTechs):
					if CyPlayer.canResearch(iTech, False) and GC.getTechInfo(iTech).getEra() <= iMinEra:
						CyTeam.setHasTech(iTech, True, iPlayer, False, False)
		else:
			iNumTeams = GAME.countCivTeamsAlive()
			iTechFrac = self.RevOpt.getBarbTechPercent()
			if iTechFrac < 1: iTechFrac = 1
			for iTech in xrange(iNumTechs):
				if iTech in techsOwned:
					CyTeam.setHasTech(iTech, True, iPlayer, False, False)
					continue
				if not CyPlayer.canResearch(iTech, False):
					continue
				iKnownRatio = 100 * GAME.countKnownTechNumTeams(iTech) / iNumTeams
				if iKnownRatio < 100 and closeTeams:
					iTeams = 0
					iKnown = 0
					for iTeamX in closeTeams:
						iTeams += 1
						CyTeamX = GC.getTeam(iTeamX)
						if CyTeamX.isHasTech(iTech):
							iKnown += 1

					iKnownRatio = (iKnownRatio + 100*iKnown/iTeams) / 2

				if iKnownRatio >= iTechFrac:
					CyTeam.setHasTech(iTech, True, iPlayer, False, False)

		CyTeam.setIsMinorCiv(True, False)

		# Units
		iNumBarbDefenders = GC.getHandicapInfo(GAME.getHandicapType()).getBarbarianInitialDefenders()

		iDefender, iCounter, iAttack, iMobile, iAttackCity, iWorker, iSettler, iExplorer, iMerchant = self.getUnitsForPlayer(iPlayer, CyTeam)

		# Put stuff in city
		fMilitaryMod = self.RevOpt.getMilitaryStrength()
		self.setupFormerBarbCity(CyCity, iPlayer, iDefender, int(iNumBarbDefenders*fMilitaryMod + 1))

		# Extra units
		iBaseOffensiveUnits = 2 + iEra + iNumBarbDefenders

		if bNewWorld:
			if bMajorCivCities:
				iBaseOffensiveUnits *= 4
			elif bMajorCivUnits:
				iBaseOffensiveUnits *= 2
			else: iBaseOffensiveUnits /= 3
		else:
			if iSettler > -1:
				CyPlayer.initUnit(iSettler, iX, iY, UnitAITypes.UNITAI_SETTLE, DirectionTypes.DIRECTION_SOUTH)
				#print "Free settler: " + CyUnit.getName()
			if iWorker > -1:
				CyPlayer.initUnit(iWorker, iX, iY, UnitAITypes.UNITAI_WORKER, DirectionTypes.DIRECTION_SOUTH)
				CyPlayer.initUnit(iWorker, iX, iY, UnitAITypes.UNITAI_WORKER, DirectionTypes.DIRECTION_SOUTH)
				#print "Free Workers (2): " + CyUnit.getName()
			if iExplorer > -1:
				CyPlayer.initUnit(iExplorer, iX, iY, UnitAITypes.UNITAI_EXPLORE, DirectionTypes.DIRECTION_SOUTH)
				CyPlayer.initUnit(iExplorer, iX, iY, UnitAITypes.UNITAI_EXPLORE, DirectionTypes.DIRECTION_SOUTH)
				#print "Free Explorers (2): " + CyUnit.getName()
			if iMerchant > -1:
				iTemp = 2 + 2*(iEra + 1)
				for i in xrange(iTemp):
					CyPlayer.initUnit(iMerchant, iX, iY, UnitAITypes.UNITAI_MERCHANT, DirectionTypes.DIRECTION_SOUTH)
				#print "Free Merchant (%d): %s" %(iTemp, CyUnit.getName())

		aList = [iCounter, iAttack, iAttackCity, iMobile]
		iTemp = int(iBaseOffensiveUnits*fMilitaryMod)
		iCount = 0
		for iUnit in aList:
			# if the list only consist of "-1" elements then the while loop would never finish.
			if iUnit > -1:
				while iCount < iTemp:
					iUnit = aList[GAME.getSorenRandNum(4, 'BC give offensive')]
					if iUnit == -1: continue
					CyUnit = CyPlayer.initUnit(iUnit, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
					CyUnit.changeExperience(iEra + GAME.getSorenRandNum(2*(iEra+1), 'Experience'), -1, False, False, False)
					iCount += 1
					#print "Free Combatant: " + CyUnit.getName()
				break

		szTxt = TRNSLTR.getText("TXT_KEY_BARBCIV_WORD_SPREADS", ()) + " "
		szTxt += TRNSLTR.getText("TXT_KEY_BARBCIV_FORM_MINOR", ()) % (civName, szCityName)
		MSG_TIME = GC.getEVENT_MESSAGE_TIME()
		for iPlayerX in xrange(MAX_PC_PLAYERS):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isHuman(): continue
			iTeamX = CyPlayerX.getTeam()
			if GAME.isDebugMode() or iTeamX in closeTeams or CyCity.plot().isRevealed(iTeamX, False):
				CyInterface().addMessage(iPlayerX, False, MSG_TIME, szTxt, None, InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), -1, -1, False, False)



	def getScenario(self, CyArea):
		if CyArea == None:
			print "Error!  Passed a None area!"
			return None

		bMajorCivCities = False
		bMajorCivUnits = False
		iCount = 0
		iPowerArea = 0
		fPowerPlayer = 0.0

		# Smaller landmasses require less relative civ power to be considered Old World...
		fSizeMod = 1.0 * MAP.getLandPlots()/CyArea.getNumTiles()
		fSizeMod **= 0.9 # ...within reason.

		iAreaID = CyArea.getID()
		for iPlayerX in xrange(self.MAX_PC_PLAYERS):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isAlive(): continue

			iPowerArea += CyArea.getPower(iPlayerX)
			fPowerPlayer += CyPlayerX.getPower()
			iCount += 1

			if CyPlayerX.isMinorCiv(): continue

			if not bMajorCivCities and CyArea.getCitiesPerPlayer(iPlayerX) > 0:
				bMajorCivCities = True
				bMajorCivUnits = True

			elif not bMajorCivUnits and CyArea.getUnitsPerPlayer(iPlayerX) > 0:
				bMajorCivUnits = True

		if iCount:
			bNewWorld = iPowerArea * fSizeMod < fPowerPlayer / iCount
		else: bNewWorld = True

		return [bNewWorld, bMajorCivCities, bMajorCivUnits]


	def getCloseCivs(self, iPlayer, CyArea, iX, iY):
		closeTeams = []
		iTemp = self.RevOpt.getCloseDist()
		iTemp *= GC.getWorldInfo(MAP.getWorldSize()).getDefaultPlayers() - 1
		for iPlayerX in xrange(self.MAX_PC_PLAYERS):
			if iPlayerX == iPlayer: continue
			if CyArea.getCitiesPerPlayer(iPlayerX) > 0:
				CyPlayerX = GC.getPlayer(iPlayerX)
				iTeamX = CyPlayerX.getTeam()
				if iTeamX in closeTeams: continue
				CyCity = CyPlayerX.getCapitalCity()
				iDist = plotDistance(iX, iY, CyCity.getX(), CyCity.getY())
				if iDist < iTemp: closeTeams.append(iTeamX)
		return closeTeams

	def getUnitsForPlayer(self, iPlayer, CyTeam):

		CyPlayer = GC.getPlayer(iPlayer)

		aList = [-1, -1, -1, -1, -1]

		iDefenderStr = 0
		iCounterStr = 0
		iAttackStr = 0
		iAttackCityStr = 0
		iMobileVal = 0
		for iUnit in xrange(GC.getNumUnitInfos()):
			if isLimitedUnit(iUnit): continue

			CvUnitInfo = GC.getUnitInfo(iUnit)
			if CvUnitInfo.getDomainType() != DomainTypes.DOMAIN_LAND or CvUnitInfo.getNumPrereqAndBuildings() > 0:
				continue

			if not CyPlayer.canTrain(iUnit, False, False): continue

			iStr = CvUnitInfo.getCombat()
			if CvUnitInfo.getUnitAIType(UnitAITypes.UNITAI_CITY_DEFENSE):
				if iStr > iDefenderStr:
					aList[0] = iUnit
					iDefenderStr = iStr
			if not CvUnitInfo.isOnlyDefensive():
				if CvUnitInfo.getUnitAIType(UnitAITypes.UNITAI_COUNTER):
					if iStr >= iCounterStr:
						aList[1] = iUnit
						iCounterStr = iStr
				if CvUnitInfo.getUnitAIType(UnitAITypes.UNITAI_ATTACK):
					if iStr > iAttackStr:
						aList[2] = iUnit
						iAttackStr = iStr
				if CvUnitInfo.getUnitAIType(UnitAITypes.UNITAI_ATTACK_CITY):
					if iStr > iAttackCityStr:
						aList[3] = iUnit
						iAttackCityStr = iStr
				iVal = iStr * CvUnitInfo.getMoves()
				if iVal > iMobileVal:
					aList[4] = iUnit
					iMobileVal = iVal

		iStd = -1
		if -1 in aList:
			for iUnit in aList:
				if iUnit > -1:
					iStd = iUnit
					break
			if iStd != -1:
				for i in xrange(5): # len(aList)
					if aList[i] == -1:
						aList[i] = iStd

		aList.append(CyPlayer.getBestUnitType(UnitAITypes.UNITAI_WORKER))
		aList.append(CyPlayer.getBestUnitType(UnitAITypes.UNITAI_SETTLE))
		aList.append(CyPlayer.getBestUnitType(UnitAITypes.UNITAI_EXPLORE))

		aMerchantList = [
			GC.getInfoTypeForString("UNIT_FREIGHT"),
			GC.getInfoTypeForString("UNIT_SUPPLY_TRAIN"),
			GC.getInfoTypeForString("UNIT_TRADE_CARAVAN"),
			GC.getInfoTypeForString("UNIT_EARLY_MERCHANT_C2C")
		]
		iStd = GC.getInfoTypeForString("UNIT_CAPTIVE_MILITARY")
		for iUnit in aMerchantList:
			if iUnit < 0: continue
			CvUnitInfo = GC.getUnitInfo(iUnit)
			# Tech Prereq
			iTech = CvUnitInfo.getPrereqAndTech()
			if iTech > -1 and not CyTeam.isHasTech(iTech):
				continue
			for iTech in CvUnitInfo.getPrereqAndTechs():
				if not CyTeam.isHasTech(iTech):
					break
			else:
				aList.append(iUnit); break
		else: aList.append(iStd)

		if -1 in aList:
			for i in xrange(len(aList)):
				if aList[i] == -1:
					aList[i] = iStd

		return aList


	def setupFormerBarbCity(self, CyCity, iPlayer, iDefender, iDefenders):
		# Change name
		CyPlayer = GC.getPlayer(iPlayer)
		CyCity.setName(CyPlayer.getNewCityName(), True)

		# List plots within city range.
		X = CyCity.getX()
		Y = CyCity.getY()
		CyPlot = CyCity.plot()

		bWrapX = MAP.isWrapX()
		bWrapY = MAP.isWrapY()
		iWidth = MAP.getGridWidth()
		iHeight = MAP.getGridHeight()
		aList = [[0, CyPlot]]
		for iX in xrange(X-3,X+4):
			for iY in xrange(Y-3, Y+4):
				if iX == X and iY == Y: continue
				# Get raw radius
				iRadX = X - iX
				iRadY = Y - iY
				# Check if coordinates are valid and translate wrap edge crossing.
				if iX < 0 or iX >= iWidth:
					if bWrapX:
						if iX < 0:
							iX += iWidth
						else: iX -= iWidth
					else: continue

				if iY < 0 or iY >= iHeight:
					if bWrapY:
						if iY < 0:
							iY += iHeight
						else: iY -= iHeight
					else : continue
				# Get efective radius
				if iRadX < 0:
					iRadX = -iRadX
				if iRadY < 0:
					iRadY = -iRadY
				if iRadX >= iRadY:
					iRadius = iRadX
				else: iRadius = iRadY

				aList.append([iRadius, MAP.plot(iX, iY)])
		# Convert nearby barbarian
		iPlayerBarb = self.BARBARIAN_PLAYER

		for iRadius, CyPlotX in aList:
			# Plot Culture
			iCult = CyPlotX.getCulture(iPlayerBarb)
			if iCult > 0:

				if iRadius:
					iCult = iCult * 4/(4+iRadius*iRadius)

				CyPlotX.changeCulture(iPlayerBarb, -iCult, False)
				CyPlotX.changeCulture(iPlayer, iCult, True)

			# Units
			for CyUnit in CyPlotX.units():
				if (
					CyUnit.getOwner() == iPlayerBarb
				and (CyPlotX.getOwner() == iPlayer or not GAME.getSorenRandNum(iRadius + 1, 'Convert Barbarian'))
				):
					iUnit = CyUnit.getUnitType()
					CyUnit.kill(False, -1)
					CyPlayer.initUnit(iUnit, CyPlotX.getX(), CyPlotX.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)

		# City Culture
		CyCity.setCultureTimes100(iPlayer, CyCity.getCultureTimes100(iPlayerBarb), True)
		CyCity.setCultureTimes100(iPlayerBarb, 0, False)

		# Free city defenders
		if iDefender > -1:
			for i in range(iDefenders):
				CyPlayer.initUnit(iDefender, X, Y, UnitAITypes.UNITAI_CITY_DEFENSE, DirectionTypes.NO_DIRECTION)



	def checkMinorCivs(self, iPlayer, CyPlayer, MAX_PC_PLAYERS):
		# Check minor civs for accomplishments which warrant settling into full civ.
		iCities = CyPlayer.getNumCities()
		if iCities < 1: return
		POST_FIX = "\n\tBarbarianCiv.checkMinorCivs"
		CyCity1 = CyPlayer.getCapitalCity()

		CyArea = CyCity1.area()
		aList = self.getScenario(CyArea)
		if not aList: return
		bNewWorld, bMajorCivCities, bMajorCivUnits = aList
		iAreaID = CyArea.getID()

		if bNewWorld:
			iPolicy = self.RevOpt.getNewWorldPolicy()

			if iPolicy > 3:
				if not bMajorCivCities: return

			elif iPolicy > 1:
				if not bMajorCivUnits: return

		iTeam = CyPlayer.getTeam()
		CyTeam = GC.getTeam(iTeam)

		# Check for accomplishments
		odds = 0; iTemp = 0
		hasMet = []
		for iPlayerX in range(MAX_PC_PLAYERS):
			if iPlayerX == iPlayer: continue
			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isAlive(): continue
			iTeamX = CyPlayerX.getTeam()
			if iTeamX != iTeam and not CyTeam.isHasMet(iTeamX): continue
			hasMet.append(iPlayerX)
			if CyPlayerX.isMinorCiv():
				iTemp += 5
			else: odds += 50

		# New world requires contact with a major civ.
		if bNewWorld and not odds: return

		odds += iTemp # Value from contact with other minor civs.
		odds += 12*iCities + CyPlayer.getTotalPopulation() + 32*CyPlayer.countHolyCities()
		odds += CyPlayer.getNumMilitaryUnits()/(4*GC.getWorldInfo(GC.getMap().getWorldSize()).getTargetNumCities())
		odds += 4*CyPlayer.getWondersScore() # 20 points per wonder, see getWonderScore in CvGameCoreUtils.cpp.
		if odds < 512: return

		iFactorGS = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent()
		if not GAME.getSorenRandNum(40*iFactorGS + odds, 'minor2major') < odds: return

		iX = CyCity1.getX(); iY = CyCity1.getY()

		bLowScore = False
		if not bNewWorld:
			iScoreSum = 0
			iMinScore = ""
			iPlayerX = -1
			iCount = 0
			while iPlayerX < MAX_PC_PLAYERS:
				iPlayerX += 1
				if iPlayerX == iPlayer: continue
				CyPlayerX = GC.getPlayer(iPlayerX)
				if not CyPlayerX.isAlive() or CyPlayerX.isMinorCiv(): continue
				iScore = GAME.getPlayerScore(iPlayerX)
				iScoreSum += iScore
				if iScore < iMinScore:
					iMinScore = iScore
				iCount += 1
			if iCount:
				bLowScore = GAME.getPlayerScore(iPlayer) < (iScoreSum - iMinScore) / iCount

		# Turn a minor BarbCiv into a full civ, give more bonuses to launch into the world
		if bNewWorld or bLowScore:

			iDefender, iCounter, iAttack, iMobile, iAttackCity, iWorker, iSettler, iExplorer, iMerchant = self.getUnitsForPlayer(iPlayer, CyTeam)
			iEra = CyPlayer.getCurrentEra()
			iHighestEra = GAME.getHighestEra()

			iPlayerBarb = self.BARBARIAN_PLAYER
			iNumBarbDefenders = GC.getHandicapInfo(GAME.getHandicapType()).getBarbarianInitialDefenders()
			fMilitaryMod = self.RevOpt.getMilitaryStrength()

			# Pickup nearby barb cities, search a 4x area if in new world.
			iMaxDistance = (iHighestEra + 10) * (1 + 3*bNewWorld) * GC.getWorldInfo(MAP.getWorldSize()).getDefaultPlayers() / 8
			CyPlayerBarb = GC.getPlayer(iPlayerBarb)
			aList = ()
			for cityX in CyPlayerBarb.cities():
				plotX = cityX.plot()
				if plotX.getArea() == iAreaID or plotX.isAdjacentRevealed(iTeam):
					x = cityX.getX()
					y = cityX.getY()
					iDist = plotDistance(iX, iY, x, y)

					if iDist <= iMaxDistance and GAME.getSorenRandNum(2, "fifty fifty"):
						iCities += 1
						if cityX.getOriginalOwner() == iPlayerBarb:
							cityX.setOriginalOwner(iPlayer)
						aList += ((plotX, x, y),) # No point in including the cityX pointer...

			for plotX, x, y in aList:
				plotX.setOwner(iPlayer) # ...because this invalidates the cityX pointer.
				cityX = plotX.getPlotCity()
				self.setupFormerBarbCity(cityX, iPlayer, iDefender, int(iNumBarbDefenders*fMilitaryMod + 1))
				cityX.changePopulation(1)
				if iWorker > -1:
					CyPlayer.initUnit(iWorker, x, y, UnitAITypes.UNITAI_WORKER, DirectionTypes.DIRECTION_SOUTH)
				if iExplorer > -1:
					CyPlayer.initUnit(iExplorer, x, y, UnitAITypes.UNITAI_EXPLORE, DirectionTypes.DIRECTION_SOUTH)
				if iMerchant > -1:
					iTemp = 2 + 2*(iEra + 1)
					for i in xrange(iTemp):
						CyPlayer.initUnit(iMerchant, x, y, UnitAITypes.UNITAI_MERCHANT, DirectionTypes.DIRECTION_SOUTH)

			# Bonus units in capital
			if iWorker != -1:
				CyPlayer.initUnit(iWorker, iX, iY, UnitAITypes.UNITAI_WORKER, DirectionTypes.DIRECTION_SOUTH)
			i1 = iCounter
			i2 = iMobile
			i3 = iAttack
			i4 = iAttackCity
			i5 = iMerchant
			aList = [i1, i1, i2, i3, i3, i3, i3, i4, i4, i4, i5, i5, i5]
			iLen = len(aList); i = 0
			while i < iLen:
				if aList[i] < 0:
					aList.pop(i)
					iLen -= 1
				else: i += 1

			if iLen:
				amount = (iEra * 2 + iEra + iNumBarbDefenders - iCities) * (bNewWorld * iHighestEra + 1) * fMilitaryMod
				if amount < 1:
					amount = 1
				iCount = 0
				while iCount < amount:
					iUnit = aList[GAME.getSorenRandNum(iLen, 'Military')]
					CyUnit = CyPlayer.initUnit(iUnit, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
					CyUnit.changeExperience(iEra + GAME.getSorenRandNum(2*(iEra+1), 'Experience'), -1, False, False, False)
					iCount += 1

			# Great persons
			i1 = GC.getInfoTypeForString("UNIT_PROPHET")
			i2 = GC.getInfoTypeForString("UNIT_ARTIST")
			i3 = GC.getInfoTypeForString("UNIT_MERCHANT")
			i4 = GC.getInfoTypeForString("UNIT_GREAT_STATESMAN")
			i5 = GC.getInfoTypeForString("UNIT_GREAT_HUNTER")
			iGeneral = GC.getInfoTypeForString("UNIT_GREAT_GENERAL")
			aList = [
				i1, i1, i2, i2, i2, i2, i3, i3, i4, i4, i5, i5, i5, i5, iGeneral, iGeneral, iGeneral,
				GC.getInfoTypeForString("UNIT_SCIENTIST"), GC.getInfoTypeForString("UNIT_GREAT_ENGINEER"),
				GC.getInfoTypeForString("UNIT_GREAT_SPY"), GC.getInfoTypeForString("UNIT_DOCTOR")
			]
			iMax = int((iEra + 2)**0.8)
			iLen = len(aList); iCount = 0

			while iCount < iMax and iLen:

				iUnit = aList.pop(GAME.getSorenRandNum(iLen, 'Great Person'))
				iLen -= 1
				CyPlayer.initUnit(iUnit, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
				iCount += 1

			# Gold
			CyPlayer.changeGold(2 * iFactorGS * (iEra + 1))
			CyPlayer.changeGoldenAgeTurns(GAME.goldenAgeLength())
		CyTeam.setIsMinorCiv(False, False)

		civName = CyPlayer.getCivilizationShortDescription(0)
		print "[INFO] Minor civ %s becomes a major civ.%s" %(civName, POST_FIX)
		# Add replay message
		szMsg = TRNSLTR.getText("TXT_KEY_BARBCIV_MINOR_SETTLE", ()) %(CyPlayer.getName(), CyPlayer.getCivilizationAdjective(1), civName)
		iColor = GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")
		GAME.addReplayMessage(ReplayMessageTypes.REPLAY_MESSAGE_MAJOR_EVENT, iPlayer, szMsg, iX, iY, iColor)

		# Announce the barb civ settling
		iPlayerAct = GC.getGame().getActivePlayer()
		if iPlayer == iPlayerAct:
			sendMessage(TRNSLTR.getText("TXT_KEY_BARBCIV_FULL_CIV", ()), iPlayer, eColor=iColor, eMsgType=InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, bForce=False)
		elif iPlayerAct in hasMet:
			sendMessage(TRNSLTR.getText("TXT_KEY_BARBCIV_WORD_SPREADS", ()) + " " + szMsg, iPlayerAct, eColor=iColor, eMsgType=InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, bForce=False)
