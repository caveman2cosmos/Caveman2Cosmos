## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## Implementaion of miscellaneous game functions

from CvPythonExtensions import *
import CvUtil

GC = CyGlobalContext()
GAME = GC.getGame()

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
		return GAME.getElapsedGameTurns() > 10

	def isVictory(self, argsList):
		eVictory = argsList[0]
		return True

	def isPlayerResearch(self, argsList):
		ePlayer = argsList[0]
		return True

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
		if CyPlot and CyPlot.getBonusType(-1) < 0 and not CyPlot.isWater():
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
		iEndTurn = GAME.getEstimateEndTurn()
		if not iEndTurn: return 0
		iPlayer, bFinal, bVictory, = argsList
		if bFinal and bVictory:
			fTurnRatio = GAME.getGameTurn() / float(iEndTurn)
		else: fTurnRatio = 0

		if bVictory:
			fVictory = (100 + GC.getDefineINT("SCORE_VICTORY_PERCENT")) / 100.0
		else: fVictory = 1

		if bFinal:
			fFinal = (100 + GC.getDefineINT("SCORE_HANDICAP_PERCENT_OFFSET") + GAME.getHandicapType() * GC.getDefineINT("SCORE_HANDICAP_PERCENT_PER")) / 100.0
		else: fFinal = 1

		CyPlayer = GC.getPlayer(iPlayer)
		score = 0
		fFreePercent = self.fScoreFreeMod
		# Population
		iInitial = GAME.getInitPopulation()
		iMax = GAME.getMaxPopulation()
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
		iInitial = GAME.getInitLand()
		iMax = GAME.getMaxLand()
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
		iInitial = GAME.getInitTech()
		iMax = GAME.getMaxTech()
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
		iInitial = GAME.getInitWonders()
		iMax = GAME.getMaxWonders()
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
		gold = GAME.getSorenRandNum(iTemp, "Pillage Gold 1")
		gold += GAME.getSorenRandNum(iTemp, "Pillage Gold 2")

		gold += CyUnit.getPillageChange() * gold / 100.0

		CyPlayer = GC.getPlayer(CyUnit.getOwner())
		if obsoleteTech == -1 or not GC.getTeam(CyPlayer.getTeam()).isHasTech(obsoleteTech) and CyPlayer.countNumBuildings(self.iHimejiCastle):
			gold *= 2

		return int(gold)


	def doCityCaptureGold(self, argsList):
		CyCity, iOwnerNew, = argsList

		ownerOld = GC.getPlayer(CyCity.getOwner())
		obsoleteTech = self.iHimejiCastleObsoleteTech
		if obsoleteTech == -1 or not GC.getTeam(ownerOld.getTeam()).isHasTech(obsoleteTech) and ownerOld.countNumBuildings(self.iHimejiCastle):
			return 0

		gold = self.BASE_CAPTURE_GOLD

		gold += CyCity.getPopulation() * self.CAPTURE_GOLD_PER_POP
		gold += GAME.getSorenRandNum(self.CAPTURE_GOLD_RAND1, "One")
		gold += GAME.getSorenRandNum(self.CAPTURE_GOLD_RAND2, "Two")

		iMaxTurns = self.CAPTURE_GOLD_MAX_TURNS
		if iMaxTurns > 0:
			iTurns = GAME.getGameTurn() - CyCity.getGameTurnAcquired()
			if iTurns > 0 and iTurns < iMaxTurns:
				gold *= 1.0 * iTurns / iMaxTurns

		if CyCity.getNumActiveBuilding(self.iNationalMint):
			gold *= 10

		if (iOwnerNew == GAME.getActivePlayer()):
			CvUtil.sendMessage(CyTranslator().getText("TXT_KEY_MISC_PILLAGED_CITY_CAPTURED", (int(gold), CyCity.getName())), iOwnerNew, 10, '', ColorTypes(10))

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
		eWidgetType, iData1, iData2, bOption = argsList
## Religion Screen ##
		if eWidgetType == WidgetTypes.WIDGET_HELP_RELIGION:
			if iData1 == -1:
				return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
## Platy WorldBuilder ##
		elif eWidgetType == WidgetTypes.WIDGET_PYTHON:
			if iData1 == 1027:
				return CyTranslator().getText("TXT_KEY_WB_PLOT_DATA",())
			elif iData1 == 1029:
				if iData2 == 0:
					sText = CyTranslator().getText("TXT_KEY_WB_PYTHON", ())
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onFirstContact"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onChangeWar"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onVassalState"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCityAcquired"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCityBuilt"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCultureExpansion"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onGoldenAge"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onEndGoldenAge"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onGreatPersonBorn"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onPlayerChangeStateReligion"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onReligionFounded"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onReligionSpread"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onReligionRemove"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCorporationFounded"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCorporationSpread"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCorporationRemove"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onUnitCreated"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onUnitLost"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onUnitPromoted"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onBuildingBuilt"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onProjectBuilt"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onTechAcquired"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onImprovementBuilt"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onImprovementDestroyed"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onRouteBuilt"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onPlotRevealed"
					return sText
				elif iData2 == 1:
					return CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA",())
				elif iData2 == 2:
					return CyTranslator().getText("TXT_KEY_WB_TEAM_DATA",())
				elif iData2 == 3:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_TECH",())
				elif iData2 == 4:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_PROJECT",())
				elif iData2 == 5:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ())
				elif iData2 == 6:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_PROMOTION",())
				elif iData2 == 7:
					return CyTranslator().getText("TXT_KEY_WB_CITY_DATA2",())
				elif iData2 == 8:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BUILDING",())
				elif iData2 == 9:
					return "Platy Builder\nVersion: 4.17b"
				elif iData2 == 10:
					return CyTranslator().getText("TXT_KEY_CONCEPT_EVENTS",())
				elif iData2 == 11:
					return CyTranslator().getText("TXT_KEY_WB_RIVER_PLACEMENT",())
				elif iData2 == 12:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_IMPROVEMENT",())
				elif iData2 == 13:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BONUS",())
				elif iData2 == 14:
					return CyTranslator().getText("TXT_KEY_WB_PLOT_TYPE",())
				elif iData2 == 15:
					return CyTranslator().getText("TXT_KEY_CONCEPT_TERRAIN",())
				elif iData2 == 16:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_ROUTES",())
				elif iData2 == 17:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_FEATURE",())
				elif iData2 == 18:
					return CyTranslator().getText("TXT_KEY_MISSION_BUILD_CITY",())
				elif iData2 == 19:
					return CyTranslator().getText("TXT_KEY_WB_ADD_BUILDINGS",())
				elif iData2 == 20:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_RELIGION",())
				elif iData2 == 21:
					return CyTranslator().getText("TXT_KEY_CONCEPT_CORPORATIONS",())
				elif iData2 == 22:
					return CyTranslator().getText("TXT_KEY_ESPIONAGE_CULTURE",())
				elif iData2 == 23:
					return CyTranslator().getText("TXT_KEY_PITBOSS_GAME_OPTIONS",())
				elif iData2 == 24:
					return CyTranslator().getText("TXT_KEY_WB_SENSIBILITY",())
				elif iData2 == 27:
					return CyTranslator().getText("TXT_KEY_WB_ADD_UNITS",())
				elif iData2 == 28:
					return CyTranslator().getText("TXT_KEY_WB_TERRITORY",())
				elif iData2 == 29:
					return CyTranslator().getText("TXT_KEY_WB_ERASE_ALL_PLOTS",())
				elif iData2 == 30:
					return CyTranslator().getText("TXT_KEY_WB_REPEATABLE",())
				elif iData2 == 32:
					return CyTranslator().getText("TXT_KEY_WB_STARTING_PLOT", ())
				elif iData2 == 33:
					return CyTranslator().getText("TXT_KEY_INFO_SCREEN", ())
				elif iData2 == 34:
					return CyTranslator().getText("TXT_KEY_CONCEPT_TRADE", ())
			elif iData1 > 1029 and iData1 < 1040:
				if iData1 %2:
					return "-"
				return "+"
			elif iData1 == 1041:
				return CyTranslator().getText("TXT_KEY_WB_KILL",())
			elif iData1 == 1042:
				return CyTranslator().getText("TXT_KEY_MISSION_SKIP",())
			elif iData1 == 1043:
				if iData2 == 0:
					return CyTranslator().getText("TXT_KEY_WB_DONE",())
				elif iData2 == 1:
					return CyTranslator().getText("TXT_KEY_WB_FORTIFY",())
				elif iData2 == 2:
					return CyTranslator().getText("TXT_KEY_WB_WAIT",())
			elif iData1 == 6785:
				return CyGameTextMgr().getProjectHelp(iData2, False, CyCity())
			elif iData1 == 6787:
				return GC.getProcessInfo(iData2).getDescription()
			elif iData1 == 6788:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
				return GC.getRouteInfo(iData2).getDescription()
## City Hover Text ##
			elif iData1 > 7199 and iData1 < 7300:
				iPlayer = iData1 - 7200
				pPlayer = GC.getPlayer(iPlayer)
				pCity = pPlayer.getCity(iData2)
				if GAME.GetWorldBuilderMode():
					sText = "<font=3>"
					if pCity.isCapital():
						sText += CyTranslator().getText("[ICON_STAR]", ())
					elif pCity.isGovernmentCenter():
						sText += CyTranslator().getText("[ICON_SILVER_STAR]", ())
					sText += u"%s: %d<font=2>" %(pCity.getName(), pCity.getPopulation())
					sTemp = ""
					if pCity.isConnectedToCapital(iPlayer):
						sTemp += CyTranslator().getText("[ICON_TRADE]", ())
					for i in xrange(GC.getNumReligionInfos()):
						if pCity.isHolyCityByType(i):
							sTemp += u"%c" %(GC.getReligionInfo(i).getHolyCityChar())
						elif pCity.isHasReligion(i):
							sTemp += u"%c" %(GC.getReligionInfo(i).getChar())

					for i in xrange(GC.getNumCorporationInfos()):
						if pCity.isHeadquartersByType(i):
							sTemp += u"%c" %(GC.getCorporationInfo(i).getHeadquarterChar())
						elif pCity.isHasCorporation(i):
							sTemp += u"%c" %(GC.getCorporationInfo(i).getChar())
					if sTemp:
						sText += "\n" + sTemp

					iMaxDefense = pCity.getTotalDefense(False)
					if iMaxDefense > 0:
						sText += u"\n%s: " %(CyTranslator().getText("[ICON_DEFENSE]", ()))
						iCurrent = pCity.getDefenseModifier(False)
						if iCurrent != iMaxDefense:
							sText += u"%d/" %(iCurrent)
						sText += u"%d%%" %(iMaxDefense)

					sText += u"\n%s: %d/%d" %(CyTranslator().getText("[ICON_FOOD]", ()), pCity.getFood(), pCity.growthThreshold())
					iFoodGrowth = pCity.foodDifference(True)
					if iFoodGrowth != 0:
						sText += u" %+d" %(iFoodGrowth)

					if pCity.isProduction():
						sText += u"\n%s:" %(CyTranslator().getText("[ICON_PRODUCTION]", ()))
						if not pCity.isProductionProcess():
							sText += u" %d/%d" %(pCity.getProduction(), pCity.getProductionNeeded())
							iProduction = pCity.getCurrentProductionDifference(False, True)
							if iProduction != 0:
								sText += u" %+d" %(iProduction)
						sText += u" (%s)" %(pCity.getProductionName())

					iGPRate = pCity.getGreatPeopleRate()
					iProgress = pCity.getGreatPeopleProgress()
					if iGPRate > 0 or iProgress > 0:
						sText += u"\n%s: %d/%d %+d" %(CyTranslator().getText("[ICON_GREATPEOPLE]", ()), iProgress, pPlayer.greatPeopleThresholdNonMilitary(), iGPRate)

					sText += u"\n%s: %d/%d (%s)" %(CyTranslator().getText("[ICON_CULTURE]", ()), pCity.getCulture(iPlayer), pCity.getCultureThreshold(), GC.getCultureLevelInfo(pCity.getCultureLevel()).getDescription())

					lTemp = []
					for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						iAmount = pCity.getCommerceRateTimes100(i)
						if iAmount <= 0: continue
						sTemp = u"%d.%02d%c" %(pCity.getCommerceRate(i), pCity.getCommerceRateTimes100(i)%100, GC.getCommerceInfo(i).getChar())
						lTemp.append(sTemp)
					if lTemp:
						sText += "\n"
						for i in xrange(len(lTemp)):
							sText += lTemp[i]
							if i < len(lTemp) - 1:
								sText += ", "

					iMaintenance = pCity.getMaintenanceTimes100()
					if iMaintenance != 0:
						sText += "\n" + CyTranslator().getText("[COLOR_WARNING_TEXT]", ()) + CyTranslator().getText("INTERFACE_CITY_MAINTENANCE", ()) + " </color>"
						sText += u"-%d.%02d%c" %(iMaintenance/100, iMaintenance%100, GC.getCommerceInfo(CommerceTypes.COMMERCE_GOLD).getChar())

					lBuildings = []
					lWonders = []
					for i in xrange(GC.getNumBuildingInfos()):
						if pCity.isHasBuilding(i):
							if isLimitedWonder(i):
								lWonders.append(GC.getBuildingInfo(i).getDescription())
							else:
								lBuildings.append(GC.getBuildingInfo(i).getDescription())
					if lBuildings:
						lBuildings.sort()
						sText += "\n" + CyTranslator().getText("[COLOR_BUILDING_TEXT]", ()) + CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BUILDING", ()) + ": </color>"
						iRange = len(lBuildings)
						for i in xrange(iRange):
							sText += lBuildings[i]
							if i < iRange - 1:
								sText += ", "
					if lWonders:
						lWonders.sort()
						sText += "\n" + CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + CyTranslator().getText("TXT_KEY_CONCEPT_WONDERS", ()) + ": </color>"
						iRange = len(lWonders)
						for i in xrange(iRange):
							sText += lWonders[i]
							if i < iRange - 1:
								sText += ", "
					sText += "</font>"
					return sText
## Religion Widget Text##
			elif iData1 == 7869:
				return CyGameTextMgr().parseReligionInfo(iData2, False)
## Building Widget Text##
			elif iData1 == 7870:
				return CyGameTextMgr().getBuildingHelp(iData2, False, False, False, None)
## Tech Widget Text##
			elif iData1 == 7871:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
				return CyGameTextMgr().getTechHelp(iData2, False, False, False, False, -1)
## Civilization Widget Text##
			elif iData1 == 7872:
				iCiv = iData2 % 10000
				return CyGameTextMgr().parseCivInfos(iCiv, False)
## Promotion Widget Text##
			elif iData1 == 7873:
				return CyGameTextMgr().getPromotionHelp(iData2, False)
## Feature Widget Text##
			elif iData1 == 7874:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
				iFeature = iData2 % 10000
				return CyGameTextMgr().getFeatureHelp(iFeature, False)
## Terrain Widget Text##
			elif iData1 == 7875:
				return CyGameTextMgr().getTerrainHelp(iData2, False)
## Leader Widget Text##
			elif iData1 == 7876:
				iLeader = iData2 % 10000
				return CyGameTextMgr().parseLeaderTraits(iLeader, -1, False, False)
## Improvement Widget Text##
			elif iData1 == 7877:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
				return CyGameTextMgr().getImprovementHelp(iData2, False)
## Bonus Widget Text##
			elif iData1 == 7878:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
				return CyGameTextMgr().getBonusHelp(iData2, False)
## Specialist Widget Text##
			elif iData1 == 7879:
				return CyGameTextMgr().getSpecialistHelp(iData2, False)
## Yield Text##
			elif iData1 == 7880:
				return GC.getYieldInfo(iData2).getDescription()
## Commerce Text##
			elif iData1 == 7881:
				return GC.getCommerceInfo(iData2).getDescription()
## Build Text##
			elif iData1 == 7882:
				return GC.getBuildInfo(iData2).getDescription()
## Corporation Screen ##
			elif iData1 == 8201:
				return CyGameTextMgr().parseCorporationInfo(iData2, False)
## Military Screen ##
			elif iData1 == 8202:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_PEDIA_ALL_UNITS", ())
				return CyGameTextMgr().getUnitHelp(iData2, False, False, False, None)
			elif iData1 > 8299 and iData1 < 8400:
				iPlayer = iData1 - 8300
				pUnit = GC.getPlayer(iPlayer).getUnit(iData2)
				sText = CyGameTextMgr().getSpecificUnitHelp(pUnit, True, False)
				if GAME.GetWorldBuilderMode():
					sText += "\n" + CyTranslator().getText("TXT_KEY_WB_UNIT", ()) + " ID: " + str(iData2)
					sText += "\n" + CyTranslator().getText("TXT_KEY_WB_GROUP", ()) + " ID: " + str(pUnit.getGroupID())
					sText += "\n" + "X: " + str(pUnit.getX()) + ", Y: " + str(pUnit.getY())
					sText += "\n" + CyTranslator().getText("TXT_KEY_WB_AREA_ID", ()) + ": "  + str(pUnit.plot().getArea())
				return sText
## Civics Screen ##
			elif iData1 == 8205 or iData1 == 8206:
				sText = CyGameTextMgr().parseCivicInfo(iData2, False, True, False)
				if GC.getCivicInfo(iData2).getUpkeep() > -1:
					sText += "\n" + GC.getUpkeepInfo(GC.getCivicInfo(iData2).getUpkeep()).getDescription()
				else:
					sText += "\n" + CyTranslator().getText("TXT_KEY_CIVICS_SCREEN_NO_UPKEEP", ())
				return sText
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
