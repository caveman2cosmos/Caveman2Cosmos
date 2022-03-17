##=========================##
## MILITIA PROMOTIONS CODE ##
## Code for Caveman2Cosmos ##
##=========================##
from CvPythonExtensions import *

GC = CyGlobalContext()

def init():
	global giMilInstCivic, giVolArmyCivic, gaiSettlerWorkerCombatList, aReefList, aTreeList

	gaiSettlerWorkerCombatList = [
		GC.getInfoTypeForString("UNITCOMBAT_SETTLER"),
		GC.getInfoTypeForString("UNITCOMBAT_WORKER"),
		GC.getInfoTypeForString("UNITCOMBAT_SEA_WORKER")
	]
	aReefList = [
		GC.getInfoTypeForString('FEATURE_REEF'),
		GC.getInfoTypeForString('FEATURE_REEF_BEACON'),
		GC.getInfoTypeForString('FEATURE_REEF_LIGHTHOUSE'),
		GC.getInfoTypeForString('FEATURE_CORAL_REEF'),
		GC.getInfoTypeForString('FEATURE_CORAL_REEF_BEACON'),
		GC.getInfoTypeForString('FEATURE_CORAL_REEF_LIGHTHOUSE')
	]
	aTreeList = [
		GC.getInfoTypeForString('FEATURE_FOREST'),
		GC.getInfoTypeForString('FEATURE_FOREST_YOUNG'),
		GC.getInfoTypeForString('FEATURE_FOREST_ANCIENT'),
		GC.getInfoTypeForString('FEATURE_JUNGLE'),
		GC.getInfoTypeForString('FEATURE_BAMBOO')
	]
	giMilInstCivic = GC.getInfoTypeForString("CIVIC_MARTIAL")
	giVolArmyCivic = GC.getInfoTypeForString("CIVIC_VOLUNTARY")

def onUnitBuilt( argsList):
	city = argsList[0]
	unit = argsList[1]
	pPlayer = GC.getPlayer(unit.getOwner())

# BEGIN MILITIA PROMOTIONS CODE - based on a prototype from FfH mod
# If the civic is a military one and if the unit being built is not a settler, worker or hero, then begin the function
# Every tile around the city is checked and added to a count based on terrain/plot type
# Based on these results, there is a chance the unit will be given a free Winterborn, Sand Devil, Cliff Walker or Tree Warden promotion

	iMilitaryCivic = 0
	if pPlayer.isCivic(giMilInstCivic):
		iMilitaryCivic += 1
	if pPlayer.isCivic(giVolArmyCivic):
		iMilitaryCivic += 1

	if iMilitaryCivic:
		if unit.getUnitCombatType() not in gaiSettlerWorkerCombatList and not isWorldUnit(unit.getUnitType()):
			iX = city.getX()
			iY = city.getY()
			MAP = GC.getMap()

			if unit.getDomainType() == DomainTypes.DOMAIN_LAND:
				iNumCold = 0
				iNumHot = 0
				iNumBush = 0
				iNumHill = 0
				iNumTree = 0
				iNumCoast = 0
				iTaiga = GC.getInfoTypeForString('TERRAIN_TAIGA')
				iTundra = GC.getInfoTypeForString('TERRAIN_TUNDRA')
				iPermafrost = GC.getInfoTypeForString('TERRAIN_TUNDRA')
				iSnow = GC.getInfoTypeForString('TERRAIN_ICE')
				iDesert = GC.getTERRAIN_DESERT()
				iDunes = GC.getInfoTypeForString('TERRAIN_DUNES')
				iSaltFlats = GC.getInfoTypeForString('TERRAIN_SALT_FLATS')
				iBarren = GC.getInfoTypeForString('TERRAIN_BARREN')
				iRocky = GC.getInfoTypeForString('TERRAIN_ROCKY')
				iScrub = GC.getInfoTypeForString('TERRAIN_SCRUB')
				iMarsh = GC.getInfoTypeForString('TERRAIN_MARSH')

				for x in range(iX - 1, iX + 2):
					for y in range(iY - 1, iY + 2):
						plot = MAP.plot(x, y)
						if not plot: continue
						if plot.isWater():
							if plot.isCoastal():
								iNumCoast += 1
							continue
						elif plot.isHills() or plot.isPeak():
							iNumHill += 1

						iTerrain = plot.getTerrainType()
						if iTerrain in (iDesert, iDunes, iSaltFlats):
							iNumHot += 1
						elif iTerrain in (iTaiga, iTundra, iSnow, iPermafrost):
							iNumCold += 1
						elif iTerrain in (iBarren, iRocky, iScrub, iMarsh):
							iNumBush += 1

						iFeature = plot.getFeatureType()
						if iFeature > -1 and iFeature in aTreeList:
							iNumTree += 1

				attemptPromotion(unit, (iNumTree  * 1.25 * iMilitaryCivic), "PROMOTION_GREEN_WARDEN")
				attemptPromotion(unit, (iNumCold  * 1.25 * iMilitaryCivic), "PROMOTION_WINTERBORN")
				attemptPromotion(unit, (iNumHot   * 1.5  * iMilitaryCivic), "PROMOTION_SAND_DEVIL")
				attemptPromotion(unit, (iNumBush  * 2    * iMilitaryCivic), "PROMOTION_BUSHMAN")
				attemptPromotion(unit, (iNumHill  * 1.5  * iMilitaryCivic), "PROMOTION_CLIFF_WALKER")
				attemptPromotion(unit, (iNumCoast * 1.5  * iMilitaryCivic), "PROMOTION_AMPHIBIOUS")

			elif unit.getDomainType() == DomainTypes.DOMAIN_SEA:
				iNumReef = 0
				iNumIce = 0
				iIce = GC.getInfoTypeForString('FEATURE_ICE')

				for x in range(iX - 1, iX + 2):
					for y in range(iY - 1, iY + 2):
						plot = MAP.plot(x, y)
						if not plot or not plot.isWater():
							continue
						iFeature = plot.getFeatureType()
						if iFeature < 0:
							continue
						if iFeature in aReefList:
							iNumReef += 1
						elif iFeature == iIce:
							iNumIce += 1

				attemptPromotion(unit, (iNumReef * 1.25 * iMilitaryCivic), "PROMOTION_COASTAL_ASSAULT1")
				attemptPromotion(unit, (iNumIce  * 1.25 * iMilitaryCivic), "PROMOTION_COASTAL_GUARD1")

def attemptPromotion(pUnit, iChance, szProposedPromotion):
	if GC.getGame().getSorenRandNum(100, "") < iChance:
		ePromotion = GC.getInfoTypeForString(szProposedPromotion)
		if pUnit.canAcquirePromotion(ePromotion):
			pUnit.setHasPromotion(ePromotion, True)
