# OOS logger: writes the info contained in the sync checksum to a log file
from CvPythonExtensions import CyGlobalContext, YieldTypes, CommerceTypes, UnitAITypes

def writeLog():
	import SystemPaths as SP
	import TextUtil
	GC = CyGlobalContext()
	MAP = GC.getMap()
	GAME = GC.getGame()
	iPlayer = GAME.getActivePlayer()
	szName = TextUtil.convertToStr(GC.getActivePlayer().getName())
	szName = SP.userDir + "\\Logs\\%s - Player %d - Turn %d OOSLog.txt" % (szName, iPlayer, GAME.getGameTurn())
	pFile = open(szName, "w")

	SEP = "-----------------------------------------------------------------\n"

	# Backup current language
	iLanguage = GAME.getCurrentLanguage()
	# Force english language for logs
	GAME.setCurrentLanguage(0)

	# Global data
	pFile.write(2 * SEP + "\tGLOBALS\n" + 2 * SEP + "\n")

	pFile.write("Last MapRand Value: %d\n" % GAME.getMapRand().getSeed())
	pFile.write("Last SorenRand Value: %d\n" % GAME.getSorenRand().getSeed())

	pFile.write("Total num cities: %d\n" % GAME.getNumCities())
	pFile.write("Total population: %d\n" % GAME.getTotalPopulation())
	pFile.write("Total Deals: %d\n" % GAME.getNumDeals())

	pFile.write("Total owned plots: %d\n" % MAP.getOwnedPlots())
	pFile.write("Total num areas: %d\n\n\n" % MAP.getNumAreas())

	# Player data
	for iPlayer in xrange(GC.getMAX_PLAYERS()):
		pPlayer = GC.getPlayer(iPlayer)
		if pPlayer.isEverAlive():

			pFile.write(2 * SEP + "%s player %d: %s\n" % (['NPC', 'Human'][pPlayer.isHuman()], iPlayer, TextUtil.convertToStr(pPlayer.getName())))
			pFile.write("  Civilization: %s\n" % TextUtil.convertToStr(pPlayer.getCivilizationDescriptionKey()))
			pFile.write("  Alive: %s\n" % pPlayer.isAlive())

			pFile.write(2 * SEP + "\n\nBasic data:\n-----------\n")

			pFile.write("Player %d Score: %d\n" % (iPlayer, GAME.getPlayerScore(iPlayer)))
			pFile.write("Player %d Population: %d\n" % (iPlayer, pPlayer.getTotalPopulation()))
			pFile.write("Player %d Total Land: %d\n" % (iPlayer, pPlayer.getTotalLand()))
			pFile.write("Player %d Gold: %d\n" % (iPlayer, pPlayer.getGold()))
			pFile.write("Player %d Assets: %d\n" % (iPlayer, pPlayer.getAssets()))
			pFile.write("Player %d Power: %d\n" % (iPlayer, pPlayer.getPower()))
			pFile.write("Player %d Num Cities: %d\n" % (iPlayer, pPlayer.getNumCities()))
			pFile.write("Player %d Num Units: %d\n" % (iPlayer, pPlayer.getNumUnits()))
			pFile.write("Player %d Num Selection Groups: %d\n" % (iPlayer, pPlayer.getNumSelectionGroups()))
			pFile.write("Player %d Difficulty: %d\n" % (iPlayer, pPlayer.getHandicapType()))
			pFile.write("Player %d State Religion: %s\n" % (iPlayer, TextUtil.convertToStr(pPlayer.getStateReligionKey())))
			pFile.write("Player %d Culture: %d\n" % (iPlayer, pPlayer.getCulture()))

			pFile.write("\n\nYields:\n-------\n")

			for iYield in xrange(YieldTypes.NUM_YIELD_TYPES):
				pFile.write("Player %d %s Total Yield: %d\n" % (iPlayer, TextUtil.convertToStr(GC.getYieldInfo(iYield).getDescription()), pPlayer.calculateTotalYield(iYield)))

			pFile.write("\n\nCommerce:\n---------\n")

			for iCommerce in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				pFile.write("Player %d %s Total Commerce: %d\n" % (iPlayer, TextUtil.convertToStr(GC.getCommerceInfo(iCommerce).getDescription()), pPlayer.getCommerceRate(CommerceTypes(iCommerce))))

			pFile.write("\n\nCity event history:\n-----------\n")

			if pPlayer.getNumCities():
				pCity, i = pPlayer.firstCity(False)
				while pCity:
					bFirst = True
					for iEvent in xrange(GC.getNumEventInfos()):
						if pCity.isEventOccured(iEvent):
							if bFirst:
								pFile.write("City: %s\n" % TextUtil.convertToStr(pCity.getName()))
								bFirst = False
							pFile.write("\t" + TextUtil.convertToStr(GC.getEventInfo(iEvent).getDescription()) + "\n")
					pCity, i = pPlayer.nextCity(i, False)

			pFile.write("\n\nCity Info:\n----------\n")

			if pPlayer.getNumCities():
				pCity, i = pPlayer.firstCity(False)
				while pCity:
					pFile.write("City: %s\n" % TextUtil.convertToStr(pCity.getName()))
					pFile.write("X: %d, Y: %d\n" % (pCity.getX(), pCity.getY()))
					pFile.write("Population: %d\n" % (pCity.getPopulation()))
					iCount = 0
					for iBuilding in xrange(GC.getNumBuildingInfos()):
						iCount += pCity.getNumRealBuilding(iBuilding)
					pFile.write("Buildings: %d\n" % iCount)
					pFile.write("Improved Plots: %d\n" % (pCity.countNumImprovedPlots()))
					pFile.write("Tiles Worked: %d, Specialists: %d\n" % (pCity.getWorkingPopulation(), pCity.getSpecialistPopulation()))
					pFile.write("Great People: %d\n" % pCity.getNumGreatPeople())
					pFile.write("Good Health: %d, Bad Health: %d\n" % (pCity.goodHealth(), pCity.badHealth(False)))
					pFile.write("Happy Level: %d, Unhappy Level: %d\n" % (pCity.happyLevel(), pCity.unhappyLevel(0)))
					pFile.write("Food: %d\n" % pCity.getFood())
					pCity, i = pPlayer.nextCity(i, False)
			else:
				pFile.write("No Cities")

			pFile.write("\n\nBonus Info:\n-----------\n")

			for iBonus in xrange(GC.getNumBonusInfos()):
				szTemp = TextUtil.convertToStr(GC.getBonusInfo(iBonus).getDescription())
				pFile.write("Player %d, %s, Number Available: %d\n" % (iPlayer, szTemp, pPlayer.getNumAvailableBonuses(iBonus)))
				pFile.write("Player %d, %s, Import: %d\n" % (iPlayer, szTemp, pPlayer.getBonusImport(iBonus)))
				pFile.write("Player %d, %s, Export: %d\n\n" % (iPlayer, szTemp, pPlayer.getBonusExport(iBonus)))

			pFile.write("\n\nImprovement Info:\n-----------------\n")

			for iImprovement in xrange(GC.getNumImprovementInfos()):
				pFile.write("Player %d, %s, Improvement count: %d\n" % (iPlayer, TextUtil.convertToStr(GC.getImprovementInfo(iImprovement).getDescription()), pPlayer.getImprovementCount(iImprovement)))

			pFile.write("\n\nBuilding Info:\n--------------------\n")

			for iBuilding in xrange(GC.getNumBuildingInfos()):
				pFile.write("Player %d, %s, Building class count plus making: %d\n" % (iPlayer, TextUtil.convertToStr(GC.getBuildingInfo(iBuilding).getDescription()), pPlayer.getBuildingCountPlusMaking(iBuilding)))

			pFile.write("\n\nUnit Class Info:\n--------------------\n")

			for iUnit in xrange(GC.getNumUnitInfos()):
				pFile.write("Player %d, %s, Unit class count plus training: %d\n" % (iPlayer, TextUtil.convertToStr(GC.getUnitInfo(iUnit).getDescription()), pPlayer.getUnitCountPlusMaking(iUnit)))

			pFile.write("\n\nUnitAI Types Info:\n------------------\n")

			for iUnitAIType in xrange(int(UnitAITypes.NUM_UNITAI_TYPES)):
				pFile.write("Player %d, %s, Unit AI Type count: %d\n" % (iPlayer, GC.getUnitAIInfo(iUnitAIType).getType(), pPlayer.AI_totalUnitAIs(UnitAITypes(iUnitAIType))))

			pFile.write("\n\nCity Religions:\n-----------\n")

			if pPlayer.getNumCities():
				pCity, i = pPlayer.firstCity(False)
				while pCity:
					bFirst = True
					for iReligion in xrange(GC.getNumReligionInfos()):
						if pCity.isHasReligion(iReligion):
							if bFirst:
								pFile.write("City: %s\n" % TextUtil.convertToStr(pCity.getName()))
								bFirst = False
							pFile.write("\t" + TextUtil.convertToStr(GC.getReligionInfo(iReligion).getDescription()) + "\n")
					pCity, i = pPlayer.nextCity(i, False)

			pFile.write("\n\nCity Corporations:\n-----------\n")

			if pPlayer.getNumCities():
				pCity, i = pPlayer.firstCity(False)
				while pCity:
					bFirst = True
					for iCorporation in xrange(GC.getNumCorporationInfos()):
						if pCity.isHasCorporation(iCorporation):
							if bFirst:
								pFile.write("City: %s\n" % TextUtil.convertToStr(pCity.getName()))
								bFirst = False
							pFile.write("\t" + TextUtil.convertToStr(GC.getCorporationInfo(iCorporation).getDescription()) + "\n")
					pCity, i = pPlayer.nextCity(i, False)

			pFile.write("\n\nUnit Info:\n----------\n")

			if pPlayer.getNumUnits():
				for pUnit in pPlayer.units():
					pFile.write("Player %d, Unit ID: %d, %s\n" % (iPlayer, pUnit.getID(), TextUtil.convertToStr(pUnit.getName())))
					pFile.write("X: %d, Y: %d\nDamage: %d\n" % (pUnit.getX(), pUnit.getY(), pUnit.getDamage()))
					pFile.write("Experience: %d\nLevel: %d\n" % (pUnit.getExperience(), pUnit.getLevel()))
					bFirst = True
					for j in xrange(GC.getNumPromotionInfos()):
						if pUnit.isHasPromotion(j):
							if bFirst:
								pFile.write("Promotions:\n")
								bFirst = False
							pFile.write("\t" + TextUtil.convertToStr(GC.getPromotionInfo(j).getDescription()) + "\n")
					bFirst = True
					for j in xrange(GC.getNumUnitCombatInfos()):
						if pUnit.isHasUnitCombat(j):
							if bFirst:
								pFile.write("UnitCombats:\n")
								bFirst = False
							pFile.write("\t" + TextUtil.convertToStr(GC.getUnitCombatInfo(j).getDescription()) + "\n")
			else:
				pFile.write("No Units")
			# Space at end of player's info
			pFile.write("\n\n")
	# Close file
	pFile.close()

	# Restore current language
	GAME.setCurrentLanguage(iLanguage)
