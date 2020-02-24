##=========================##
## MILITIA PROMOTIONS CODE ##
## Code for Caveman2Cosmos ##
##=========================##
from CvPythonExtensions import *

GC = CyGlobalContext()

def init():
	global giMilInstCivic, giVolArmyCivic, gaiSettlerWorkerCombatList

	gaiSettlerWorkerCombatList = [
		GC.getInfoTypeForString("UNITCOMBAT_SETTLER"),
		GC.getInfoTypeForString("UNITCOMBAT_WORKER"),
		GC.getInfoTypeForString("UNITCOMBAT_SEA_WORKER")
	]
	giMilInstCivic = GC.getInfoTypeForString("CIVIC_MARTIAL")
	giVolArmyCivic = GC.getInfoTypeForString("CIVIC_VOLUNTARY")

def onUnitBuilt( argsList):
	'Unit Completed'
	city = argsList[0]
	unit = argsList[1]
	pPlayer = GC.getPlayer(unit.getOwner())

# BEGIN MILITIA PROMOTIONS CODE - based on a prototype from FfH mod
# If the civic is a military one and if the unit being built is not a settler, worker or hero, then begin the function
# Every tile around the city is checked and added to a count based on terrain/plot type
# Based on these results, there is a chance the unit will be given a free Winterborn, Sand Devil, Cliff Walker or Tree Warden promotion

	iMilitaryCivic = 0
	if pPlayer.isCivic(giMilInstCivic):
		iMilitaryCivic = iMilitaryCivic + 1
	if pPlayer.isCivic(giVolArmyCivic):
		iMilitaryCivic = iMilitaryCivic + 1
	if iMilitaryCivic > 0:
		if not (unit.getUnitCombatType() in gaiSettlerWorkerCombatList) and isWorldUnit(unit.getUnitType()) == False:
			iX = city.getX()
			iY = city.getY()

			if unit.getDomainType() == DomainTypes.DOMAIN_LAND:
				iNumCold = 0
				iNumHot = 0
				iNumBush = 0
				iNumHill = 0
				iNumTree = 0
				iNumCoast = 0
				iTundra = GC.getInfoTypeForString('TERRAIN_TAIGA')
				iPermafrost = GC.getInfoTypeForString('TERRAIN_TUNDRA')
				iSnow = GC.getInfoTypeForString('TERRAIN_ICE')
				iDesert = GC.getInfoTypeForString('TERRAIN_DESERT')
				iDunes = GC.getInfoTypeForString('TERRAIN_DUNES')
				iHills = GC.getInfoTypeForString('TERRAIN_HILL')
				iBarren = GC.getInfoTypeForString('TERRAIN_BARREN')
				iRocky = GC.getInfoTypeForString('TERRAIN_ROCKY')
				iScrub = GC.getInfoTypeForString('TERRAIN_SCRUB')
				iSaltFlats = GC.getInfoTypeForString('TERRAIN_SALT_FLATS')
				iMarsh = GC.getInfoTypeForString('TERRAIN_MARSH')
				iCoast = GC.getInfoTypeForString('TERRAIN_COAST')
				iForest = GC.getInfoTypeForString('FEATURE_FOREST')
				iJungle = GC.getInfoTypeForString('FEATURE_JUNGLE')
				iBamboo = GC.getInfoTypeForString('FEATURE_BAMBOO')

				for iiX in range(iX-1, iX+2, 1):
					for iiY in range(iY-1, iY+2, 1):
						pPlot = CyMap().plot(iiX,iiY)
						iTerrain = pPlot.getTerrainType()
						iFeature = pPlot.getFeatureType()
						iPlot = pPlot.getPlotType()
						if iTerrain == iCoast:
							iNumCoast = iNumCoast +1
							continue
						if (iPlot == iHills or pPlot.isPeak()):
							iNumHill = iNumHill +1
						if iFeature == iForest or iFeature == iJungle or iFeature == iBamboo:
							iNumTree = iNumTree +1
						if iTerrain == iDesert or iTerrain == iDunes:
							iNumHot = iNumHot +1
						elif iTerrain == iTundra or iTerrain == iSnow or iTerrain == iPermafrost:
							iNumCold = iNumCold +1
						elif iTerrain == iBarren or iTerrain == iRocky or iTerrain == iScrub or iTerrain == iSaltFlats or iTerrain == iMarsh:
							iNumBush = iNumBush +1

				attemptPromotion(unit, (iNumTree  * 1.25 * iMilitaryCivic), "PROMOTION_GREEN_WARDEN")
				attemptPromotion(unit, (iNumCold  * 1.25 * iMilitaryCivic), "PROMOTION_WINTERBORN")
				attemptPromotion(unit, (iNumHot   * 1.5  * iMilitaryCivic), "PROMOTION_SAND_DEVIL")
				attemptPromotion(unit, (iNumBush  * 2    * iMilitaryCivic), "PROMOTION_BUSHMAN")
				attemptPromotion(unit, (iNumHill  * 1.5  * iMilitaryCivic), "PROMOTION_CLIFF_WALKER")
				attemptPromotion(unit, (iNumCoast * 1.5  * iMilitaryCivic), "PROMOTION_AMPHIBIOUS")

			elif unit.getDomainType() == DomainTypes.DOMAIN_SEA:
				iNumReef = 0
				iNumIce = 0
				iReef = GC.getInfoTypeForString('FEAURE_REEF')
				iIce = GC.getInfoTypeForString('FEATURE_ICE')
				iCoral = GC.getInfoTypeForString('FEAURE_CORAL')

				for iiX in range(iX-1, iX+2, 1):
					for iiY in range(iY-1, iY+2, 1):
						iFeature = CyMap().plot(iiX,iiY).getFeatureType()
						if iFeature == iReef or iFeature == iCoral:
							iNumReef = iNumReef +1
						elif iFeature == iIce:
							iNumIce = iNumIce +1

				attemptPromotion(unit, (iNumReef * 1.25 * iMilitaryCivic), "PROMOTION_COASTAL_ASSAULT1")
				attemptPromotion(unit, (iNumIce  * 1.25 * iMilitaryCivic), "PROMOTION_COASTAL_GUARD1")

def attemptPromotion(pUnit, iChance, szProposedPromotion):
	if CyGame().getSorenRandNum(100, "") < iChance:
		ePromotion = GC.getInfoTypeForString(szProposedPromotion)
		if pUnit.canAcquirePromotion(ePromotion):
			pUnit.setHasPromotion(ePromotion, True)
