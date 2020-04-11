## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## Implementaion of miscellaneous game functions

from CvPythonExtensions import *

GC = CyGlobalContext()
G = GC.getGame()

class CvGameUtils:

	def __init__(self):
		self.reset()

	def reset(self):
		# Cache
		self.fScoreFreeMod = GC.getDefineINT("SCORE_FREE_PERCENT") / 100.0
		self.SCORE_POPULATION_FACTOR = GC.getDefineINT("SCORE_POPULATION_FACTOR")
		self.SCORE_LAND_FACTOR = GC.getDefineINT("SCORE_LAND_FACTOR")
		self.SCORE_TECH_FACTOR = GC.getDefineINT("SCORE_TECH_FACTOR")
		self.SCORE_WONDER_FACTOR = GC.getDefineINT("SCORE_WONDER_FACTOR")

		self.BASE_CAPTURE_GOLD = GC.getDefineINT("BASE_CAPTURE_GOLD")
		self.CAPTURE_GOLD_PER_POP = GC.getDefineINT("CAPTURE_GOLD_PER_POPULATION")
		self.CAPTURE_GOLD_RAND1 = GC.getDefineINT("CAPTURE_GOLD_RAND1")
		self.CAPTURE_GOLD_RAND2 = GC.getDefineINT("CAPTURE_GOLD_RAND2")
		self.CAPTURE_GOLD_MAX_TURNS = GC.getDefineINT("CAPTURE_GOLD_MAX_TURNS")

		self.BASE_UNIT_UPGRADE_COST = GC.getDefineINT("BASE_UNIT_UPGRADE_COST")
		self.UNIT_UPGRADE_COST_PER_PRODUCTION = GC.getDefineINT("UNIT_UPGRADE_COST_PER_PRODUCTION")

		self.iNationalMint = GC.getInfoTypeForString("BUILDING_NATIONAL_MINT")
		self.iHimejiCastle = GC.getInfoTypeForString("BUILDING_HIMEJI_CASTLE")
		self.iHimejiCastleObsoleteTech = GC.getBuildingInfo(self.iHimejiCastle).getObsoleteTech()

		self.iReplicators = GC.getInfoTypeForString("BONUS_REPLICATORS")
		self.iRapidPrototyping = GC.getInfoTypeForString("TECH_RAPID_PROTOTYPING")

	def isVictoryTest(self):
		if G.getElapsedGameTurns() > 10:
			return True
		return False

	def isVictory(self, argsList):
		eVictory = argsList[0]
		return True

	def isPlayerResearch(self, argsList):
		ePlayer = argsList[0]
		return True

	def getExtraCost(self, argsList):
		ePlayer = argsList[0]
		return 0

	def createBarbarianCities(self):
		return False

	def createBarbarianUnits(self):
		return False

	def skipResearchPopup(self, argsList):
		ePlayer = argsList[0]
		return False

	def showTechChooserButton(self, argsList):
		ePlayer = argsList[0]
		return True

	def getFirstRecommendedTech(self, argsList):
		ePlayer = argsList[0]
		return TechTypes.NO_TECH

	def getSecondRecommendedTech(self, argsList):
		ePlayer = argsList[0]
		eFirstTech = argsList[1]
		return TechTypes.NO_TECH

	def canRazeCity(self, argsList):
		iRazingPlayer, pCity = argsList
		return True

	def canDeclareWar(self, argsList):
		iAttackingTeam, iDefendingTeam = argsList
		return True

	def skipProductionPopup(self, argsList):
		pCity = argsList[0]
		return False

	def showExamineCityButton(self, argsList):
		pCity = argsList[0]
		return True

	def getRecommendedUnit(self, argsList):
		pCity = argsList[0]
		return UnitTypes.NO_UNIT

	def getRecommendedBuilding(self, argsList):
		pCity = argsList[0]
		return BuildingTypes.NO_BUILDING

	def updateColoredPlots(self):
		return False

	def isActionRecommended(self, argsList):
		pUnit = argsList[0]
		iAction = argsList[1]
		return False

	def unitCannotMoveInto(self, argsList):
		ePlayer = argsList[0]
		iUnitId = argsList[1]
		iPlotX = argsList[2]
		iPlotY = argsList[3]
		return False

	def cannotHandleAction(self, argsList):
		pPlot = argsList[0]
		iAction = argsList[1]
		bTestVisible = argsList[2]
		return False

	def canBuild(self, argsList):
		iX, iY, iBuild, iPlayer = argsList

		# Bonus placing builds
		CyPlot = GC.getMap().plot(iX, iY)
		if CyPlot and CyPlot.getBonusType(-1) < 0:
			szType = GC.getBuildInfo(iBuild).getType()
			if szType[:12] == "BUILD_BONUS_":
				iBonus = GC.getInfoTypeForString(szType[6:])

				if iBonus > -1 and GC.getPlayer(iPlayer).getNumAvailableBonuses(iBonus):
					return 1
				return 0

		return -1	# Returning 0 means "No", 1 or greater means "Yes", and negative numbers means "continue this evaluation on the dll side".

	def cannotFoundCity(self, argsList):
		iPlayer, iPlotX, iPlotY = argsList
		return False

	def cannotSelectionListMove(self, argsList):
		pPlot = argsList[0]
		bAlt = argsList[1]
		bShift = argsList[2]
		bCtrl = argsList[3]
		return False

	def cannotSelectionListGameNetMessage(self, argsList):
		eMessage = argsList[0]
		iData2 = argsList[1]
		iData3 = argsList[2]
		iData4 = argsList[3]
		iFlags = argsList[4]
		bAlt = argsList[5]
		bShift = argsList[6]
		return False

	def cannotDoControl(self, argsList):
		eControl = argsList[0]
		return False

	def canDoCivic(self, argsList):
		ePlayer = argsList[0]
		eCivic = argsList[1]
		return False

	def cannotDoCivic(self, argsList):
		ePlayer = argsList[0]
		eCivic = argsList[1]
		return False

	def canTrain(self, argsList):
		pCity = argsList[0]
		eUnit = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		bIgnoreCost = argsList[4]
		bIgnoreUpgrades = argsList[5]
		return False

	def cannotTrain(self, argsList):
		pCity = argsList[0]
		eUnit = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		bIgnoreCost = argsList[4]
		bIgnoreUpgrades = argsList[5]
		return False

	def canConstruct(self, argsList):
		pCity = argsList[0]
		eBuilding = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		bIgnoreCost = argsList[4]
		return False

	def cannotConstruct(self, argsList):
		pCity = argsList[0]
		eBuilding = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		bIgnoreCost = argsList[4]
		return False

	def canCreate(self, argsList):
		pCity = argsList[0]
		eProject = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		return False

	def cannotCreate(self, argsList):
		pCity = argsList[0]
		eProject = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		return False

	def canMaintain(self, argsList):
		return False

	def cannotMaintain(self, argsList):
		CyCity, iProcess, bContinue, = argsList
		if not CyCity or CyCity and CyCity.isNone():
			print "not CyCity or CyCity and CyCity.isNone()"
			print "CyCity, iProcess, bContinue", argsList
			return False

		aMap = {
			#"IDLE"		: ["PROCESS_WEALTH_MEAGER", "PROCESS_RESEARCH_MEAGER", "PROCESS_CULTURE_MEAGER"],
			"WEALTH"	: ["PROCESS_WEALTH_MEAGER", "PROCESS_WEALTH_LESSER", "PROCESS_WEALTH"],
			"RESEARCH"	: ["PROCESS_RESEARCH_MEAGER", "PROCESS_RESEARCH_LESSER", "PROCESS_RESEARCH"],
			"CULTURE"	: ["PROCESS_CULTURE_MEAGER", "PROCESS_CULTURE_LESSER", "PROCESS_CULTURE"],
			"SPY"		: ["PROCESS_SPY_MEAGER", "PROCESS_SPY_LESSER", "PROCESS_SPY"]
		}
		CyTeam = GC.getTeam(CyCity.getTeam())

		TYPE = GC.getProcessInfo(iProcess).getType()

		if not TYPE.count("_"): return False

		KEY = TYPE.split("_")[1]
		if not KEY in aMap: return False

		bFound = False
		for PROCESS in aMap[KEY]:
			if bFound:
				iProcess = GC.getInfoTypeForString(PROCESS)
				if iProcess > -1:
					iTech = GC.getProcessInfo(iProcess).getTechPrereq()
					if iTech == -1 or CyTeam.isHasTech(iTech):
						return True
					return False
			elif PROCESS == TYPE:
				bFound = True
		return False

	def AI_chooseTech(self, argsList):
		ePlayer = argsList[0]
		bFree = argsList[1]
		return TechTypes.NO_TECH

	def AI_chooseProduction(self, argsList):
		pCity = argsList[0]
		return False

	def AI_unitUpdate(self, argsList):
		pUnit = argsList[0]
		return False

	def AI_doWar(self, argsList):
		eTeam = argsList[0]
		return False

	def AI_doDiplo(self, argsList):
		ePlayer = argsList[0]
		return False

	def calculateScore(self, argsList):
		iEndTurn = G.getEstimateEndTurn()
		if not iEndTurn: return 0
		iPlayer, bFinal, bVictory, = argsList
		if bFinal and bVictory:
			fTurnRatio = G.getGameTurn() / float(iEndTurn)
		else: fTurnRatio = 0

		if bVictory:
			fVictory = (100 + GC.getDefineINT("SCORE_VICTORY_PERCENT")) / 100.0
		else: fVictory = 1

		if bFinal:
			fFinal = (100 + GC.getDefineINT("SCORE_HANDICAP_PERCENT_OFFSET") + G.getHandicapType() * GC.getDefineINT("SCORE_HANDICAP_PERCENT_PER")) / 100.0
		else: fFinal = 1

		CyPlayer = GC.getPlayer(iPlayer)
		score = 0
		fFreePercent = self.fScoreFreeMod
		# Population
		iInitial = G.getInitPopulation()
		iMax = G.getMaxPopulation()
		if fTurnRatio:
			if iInitial:
				iMax = iInitial * (iMax / iInitial)**fTurnRatio
			else:
				iMax = iInitial + fTurnRatio * (iMax - iInitial)

		fFree = iMax * fFreePercent
		temp = self.SCORE_POPULATION_FACTOR * fVictory * fFinal
		fDiv = fFree + iMax
		if fDiv:
			temp *= (CyPlayer.getPopScore() + fFree) / fDiv
		score += temp
		# Land
		iInitial = G.getInitLand()
		iMax = G.getMaxLand()
		if fTurnRatio:
			if iInitial:
				iMax = iInitial * (iMax / iInitial)**fTurnRatio
			else:
				iMax = iInitial + fTurnRatio * (iMax - iInitial)

		fFree = iMax * fFreePercent
		temp = self.SCORE_LAND_FACTOR * fVictory * fFinal
		fDiv = fFree + iMax
		if fDiv:
			temp *= (CyPlayer.getLandScore() + fFree) / fDiv
		score += temp
		# Tech
		iInitial = G.getInitTech()
		iMax = G.getMaxTech()
		if fTurnRatio:
			if iInitial:
				iMax = iInitial * (iMax / iInitial)**fTurnRatio
			else:
				iMax = iInitial + fTurnRatio * (iMax - iInitial)

		fFree = iMax * fFreePercent
		temp = self.SCORE_TECH_FACTOR * fVictory * fFinal
		fDiv = fFree + iMax
		if fDiv:
			temp *= (CyPlayer.getTechScore() + fFree) / fDiv
		score += temp
		# Wonder
		iInitial = G.getInitWonders()
		iMax = G.getMaxWonders()
		if fTurnRatio:
			iMax = iInitial + fTurnRatio * (iMax - iInitial)

		fFree = iMax * fFreePercent
		temp = self.SCORE_WONDER_FACTOR * fVictory * fFinal
		fDiv = fFree + iMax
		if fDiv:
			temp *= (CyPlayer.getWondersScore() + fFree) / fDiv
		score += temp
		return int(score)

	def doHolyCity(self):
		return False

	def doGold(self, argsList):
		#ePlayer = argsList[0]
		return False

	def doResearch(self, argsList):
		#ePlayer = argsList[0]
		return False

	def doGoody(self, argsList):
		#ePlayer, pPlot, pUnit, = argsList
		return False

	def doGrowth(self, argsList):
		#pCity = argsList[0]
		return False

	def doProduction(self, argsList):
		#pCity = argsList[0]
		return False

	def doCulture(self, argsList):
		#pCity = argsList[0]
		return False

	def doPlotCulture(self, argsList):
		#pCity, bUpdate, ePlayer, iCultureRate, = argsList
		return False

	def doReligion(self, argsList):
		#pCity = argsList[0]
		return False

	def cannotSpreadReligion(self, argsList):
		#iOwner, iUnitID, iReligion, iX, iY = argsList[0]
		return False

	def doGreatPeople(self, argsList):
		#pCity = argsList[0]
		return False

	def doMeltdown(self, argsList):
		#pCity = argsList[0]
		return False

	# Allows you to perform an action after an AIAutoPlay.
	def doReviveActivePlayer(self, argsList):
		#iPlayer = argsList[0]
		return False

	def doPillageGold(self, argsList):
		CyPlot, CyUnit, = argsList

		obsoleteTech = self.iHimejiCastleObsoleteTech

		iPlayer = CyPlot.getOwner()
		if iPlayer > -1:
			CyPlayer = GC.getPlayer(iPlayer)
			if obsoleteTech == -1 or not GC.getTeam(CyPlayer.getTeam()).isHasTech(obsoleteTech) and CyPlayer.countNumBuildings(self.iHimejiCastle):
				return 0

		iTemp = GC.getImprovementInfo(CyPlot.getImprovementType()).getPillageGold()
		gold = G.getSorenRandNum(iTemp, "Pillage Gold 1")
		gold += G.getSorenRandNum(iTemp, "Pillage Gold 2")

		gold += CyUnit.getPillageChange() * gold / 100.0

		CyPlayer = GC.getPlayer(CyUnit.getOwner())
		if obsoleteTech == -1 or not GC.getTeam(CyPlayer.getTeam()).isHasTech(obsoleteTech) and CyPlayer.countNumBuildings(self.iHimejiCastle):
			gold *= 2

		return int(gold)


	def doCityCaptureGold(self, argsList):
		CyCity, = argsList

		CyPlayer = GC.getPlayer(CyCity.getOwner())
		obsoleteTech = self.iHimejiCastleObsoleteTech
		if obsoleteTech == -1 or not GC.getTeam(CyPlayer.getTeam()).isHasTech(obsoleteTech) and CyPlayer.countNumBuildings(self.iHimejiCastle):
			return 0

		gold = self.BASE_CAPTURE_GOLD

		gold += CyCity.getPopulation() * self.CAPTURE_GOLD_PER_POP
		gold += G.getSorenRandNum(self.CAPTURE_GOLD_RAND1, "One")
		gold += G.getSorenRandNum(self.CAPTURE_GOLD_RAND2, "Two")

		iMaxTurns = self.CAPTURE_GOLD_MAX_TURNS
		if iMaxTurns > 0:
			iTurns = G.getGameTurn() - CyCity.getGameTurnAcquired()
			if iTurns > 0 and iTurns < iMaxTurns:
				gold *= 1.0 * iTurns / iMaxTurns

		if CyCity.getNumActiveBuilding(self.iNationalMint):
			gold *= 10

		return int(gold)

	def citiesDestroyFeatures(self, argsList):
		#iX, iY = argsList
		return True

	def canFoundCitiesOnWater(self, argsList):
		#iX, iY = argsList
		return False

	def doCombat(self, argsList):
		#pSelectionGroup, pDestPlot = argsList
		return False

	# Returns the unit type to be conscripted.
	def getConscriptUnitType(self, argsList):
		#iPlayer = argsList[0]
		return -1

	# Returns the value for founding a city in a specific plot.
	def getCityFoundValue(self, argsList):
		#iPlayer, iPlotX, iPlotY = argsList
		return -1

	def canPickPlot(self, argsList):
		#pPlot = argsList[0]
		return true

	def getUnitCostMod(self, argsList):
		#iPlayer, iUnit = argsList
		return -1

	def getBuildingCostMod(self, argsList):
		#iPlayer, iCityID, iBuilding = argsList
		return -1

	def canUpgradeAnywhere(self, argsList):
		#pUnit = argsList
		return 0

	def getWidgetHelp(self, argsList):
		#eWidgetType, iData1, iData2, bOption = argsList
		print argsList
		return ""

	def getUpgradePriceOverride(self, argsList):
		iPlayer, iUnit, iUnitTypeUpgrade = argsList

		CyPlayer = GC.getPlayer(iPlayer)
		if CyPlayer.isNPC():
			# This check is later repeated in the dll if this check fails...
			# A redundancy that can be removed by making the check in dll happen before this python function is called.
			# Then this check wouldn't have to be here as it will then always fail here.
			# I see no reason why python coders would ever need to mess with the core rule that NPC upgrade units for free.
			return 0

		if GC.getTeam(CyPlayer.getTeam()).isHasTech(self.iRapidPrototyping) and CyPlayer.hasBonus(self.iReplicators):

			price = self.BASE_UNIT_UPGRADE_COST
			iHammerDif = CyPlayer.getUnitProductionNeeded(iUnitTypeUpgrade) - CyPlayer.getUnitProductionNeeded(iUnit)
			if iHammerDif > 0:
				price += iHammerDif * self.UNIT_UPGRADE_COST_PER_PRODUCTION

			if not CyPlayer.isHuman():
				CvHandicap = GC.getHandicapInfo(GC.getGame().getHandicapType())
				fModAI = CvHandicap.getAIUnitUpgradePercent() / 100.0
				fModAI *= (CvHandicap.getAIPerEraModifier() * CyPlayer.getCurrentEra() + 100) / 100.0
				if fModAI < 0:
					fModAI = 0
				price *= fModAI
				price -= price * CyPlayer.getUnit(iUnit).getUpgradeDiscount() / 100.0
			if price < 2:
				return 0
			return int(price/2.0)
		return -1

	# Returns the experience a unit needs to level up.
	def getExperienceNeeded(self, argsList):
		iLevel, iOwner = argsList

		iExperienceNeeded = 1

		# regular epic game experience
		iExperienceNeeded += iLevel * iLevel

		iModifier = GC.getPlayer(iOwner).getLevelExperienceModifier()
		if iModifier:
			iExperienceNeeded += (iExperienceNeeded * iModifier + 99) / 100   # ROUND UP

		return iExperienceNeeded
