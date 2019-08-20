# OOS logger: writes the info contained in the sync checksum to a log file
from CvPythonExtensions import *
def writeLog():
	import SystemPaths as SP
	GC = CyGlobalContext()
	G = CyGame()
	iPlayer = G.getActivePlayer()
	szName = GC.getActivePlayer().getName().encode("latin_1")
	szName = SP.rootDir + "\\Logs\\"+ "%s - Player %s - " %(szName, iPlayer) + "OOSLog - Turn " + "%s" %(G.getGameTurn()) + ".txt"
	pFile = open(szName, "w")

	SEP = "-----------------------------------------------------------------\n"

	# Global data
	pFile.write(2*SEP + "\tGLOBALS\n" + 2*SEP + "\n\n")

	pFile.write("Total num cities: %d\n" % G.getNumCities())
	pFile.write("Total population: %d\n" % G.getTotalPopulation())
	pFile.write("Total Deals: %d\n" % G.getNumDeals())

	pFile.write("Total owned plots: %d\n" % CyMap().getOwnedPlots())
	pFile.write("Total num areas: %d\n\n\n" % CyMap().getNumAreas())

	# Player data
	for iPlayer in range(GC.getMAX_PLAYERS()):
		CyPlayer = GC.getPlayer(iPlayer)
		if CyPlayer.isEverAlive():

			pFile.write(2*SEP + "  PLAYER %d: %s  \n" %(iPlayer, CyPlayer.getName()))
			pFile.write("  Civilizations: %s  \n" % CyPlayer.getCivilizationDescriptionKey())

			pFile.write(2*SEP + "\n\nBasic data:\n-----------\n")

			pFile.write("Player %d Score: %d\n" %(iPlayer, G.getPlayerScore(iPlayer)))
			pFile.write("Player %d Population: %d\n" %(iPlayer, CyPlayer.getTotalPopulation()))
			pFile.write("Player %d Total Land: %d\n" %(iPlayer, CyPlayer.getTotalLand()))
			pFile.write("Player %d Greater Gold: %d Gold: %d\n" %(iPlayer, CyPlayer.getGreaterGold(), CyPlayer.getGold()))
			pFile.write("Player %d Assets: %d\n" %(iPlayer, CyPlayer.getAssets()))
			pFile.write("Player %d Power: %d\n" %(iPlayer, CyPlayer.getPower()))
			pFile.write("Player %d Num Cities: %d\n" %(iPlayer, CyPlayer.getNumCities()))
			pFile.write("Player %d Num Units: %d\n" %(iPlayer, CyPlayer.getNumUnits()))
			pFile.write("Player %d Num Selection Groups: %d\n" %(iPlayer, CyPlayer.getNumSelectionGroups()))
			pFile.write("Player %d Difficulty: %d\n" %(iPlayer, CyPlayer.getHandicapType()))

			pFile.write("\n\nYields:\n-------\n")

			for iYield in range( int(YieldTypes.NUM_YIELD_TYPES)):
				pFile.write("Player %d %s Total Yield: %d\n" %(iPlayer, GC.getYieldInfo(iYield).getDescription(), CyPlayer.calculateTotalYield(iYield)))

			pFile.write("\n\nCommerce:\n---------\n")

			for iCommerce in range( int(CommerceTypes.NUM_COMMERCE_TYPES)):
				pFile.write("Player %d %s Total Commerce: %d\n" %(iPlayer, GC.getCommerceInfo(iCommerce).getDescription(), CyPlayer.getCommerceRate(CommerceTypes(iCommerce))))

			pFile.write("\n\nBonus Info:\n-----------\n")

			for iBonus in range(GC.getNumBonusInfos()):
				pFile.write("Player %d, %s, Number Available: %d\n" %(iPlayer, GC.getBonusInfo(iBonus).getDescription(), CyPlayer.getNumAvailableBonuses(iBonus)))
				pFile.write("Player %d, %s, Import: %d\n" %(iPlayer, GC.getBonusInfo(iBonus).getDescription(), CyPlayer.getBonusImport(iBonus)))
				pFile.write("Player %d, %s, Export: %d\n\n" %(iPlayer, GC.getBonusInfo(iBonus).getDescription(), CyPlayer.getBonusExport(iBonus)))

			pFile.write("\n\nImprovement Info:\n-----------------\n")

			for iImprovement in range(GC.getNumImprovementInfos()):
				pFile.write("Player %d, %s, Improvement count: %d\n" %(iPlayer, GC.getImprovementInfo(iImprovement).getDescription(), CyPlayer.getImprovementCount(iImprovement)))

			pFile.write("\n\nBuilding Info:\n--------------------\n")

			for iBuilding in range(GC.getNumBuildingInfos()):
				pFile.write("Player %d, %s, Building count: %d\n" %(iPlayer, GC.getBuildingInfo(iBuilding).getDescription(), CyPlayer.countNumBuildings(iBuilding)))

			pFile.write("\n\nUnit Class Info:\n--------------------\n")

			for iUnitClass in range(GC.getNumUnitClassInfos()):
				pFile.write("Player %d, %s, Unit class count plus training: %d\n" %(iPlayer, GC.getUnitClassInfo(iUnitClass).getDescription(), CyPlayer.getUnitClassCountPlusMaking(iUnitClass)))

			pFile.write("\n\nUnitAI Types Info:\n------------------\n")

			for iUnitAIType in range(int(UnitAITypes.NUM_UNITAI_TYPES)):
				pFile.write("Player %d, %s, Unit AI Type count: %d\n" %(iPlayer, GC.getUnitAIInfo(iUnitAIType).getDescription(), CyPlayer.AI_totalUnitAIs(UnitAITypes(iUnitAIType))))

			pFile.write("\n\nCity Info:\n----------")

			if CyPlayer.getNumCities():
				CyCity, i = CyPlayer.firstCity(False)
				while CyCity:
					pFile.write("\nX: %d, Y: %d\nFounded: %d\n" %(CyCity.getX(), CyCity.getY(), CyCity.getGameTurnFounded()))
					pFile.write("Population: %d\nBuildings: %d\n" %(CyCity.getPopulation(), CyCity.getNumBuildings()))
					pFile.write("Improved Plots: %d\nProducing: %s\n" %(CyCity.countNumImprovedPlots(), CyCity.getProductionName()))
					pFile.write("%d Tiles Worked, %d Specialists, " %(CyCity.getWorkingPopulation(), CyCity.getSpecialistPopulation()))
					pFile.write("%d Great People\n" % CyCity.getNumGreatPeople())

					CyCity, i = CyPlayer.nextCity(i, False)
			else:
				pFile.write("\nNo Cities")

			pFile.write("\n\nUnit Info:\n----------")

			if CyPlayer.getNumUnits():
				CyUnit, i = CyPlayer.firstUnit(False)
				while CyUnit:
					pFile.write("\nPlayer %d, Unit ID: %d, %s\n" %(iPlayer, CyUnit.getID(), CyUnit.getName()))
					pFile.write("X: %d, Y: %d\nDamage: %d\n" %(CyUnit.getX(), CyUnit.getY(), CyUnit.getDamage()))
					pFile.write("Experience: %d\nLevel: %d\n" %(CyUnit.getExperience(), CyUnit.getLevel()))
					bFirst = True
					for j in range(GC.getNumPromotionInfos()):
						if CyUnit.isHasPromotion(j):
							if bFirst:
								pFile.write("Promotions:\n")
								bFirst = False
							pFile.write("\t%s\n" %(GC.getPromotionInfo(j).getDescription()))

					CyUnit, i = CyPlayer.nextUnit(i, False)
			else:
				pFile.write("\nNo Units")
			# Space at end of player's info
			pFile.write("\n\n")
	# Close file
	pFile.close()
