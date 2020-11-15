# Traits functions for Revolution Mod
#
# by jdog5000
# Version 1.5

from CvPythonExtensions import *
import CvUtil
try:
	import cPickle as pickle
except:
	import pickle
# --------- Revolution mod -------------
import RevDefs
import RevData
import RevInstances


# globals
gc = CyGlobalContext()
game = CyGame()
localText = CyTranslator()


########################## Traits effect helper functions #####################

def getTraitsRevIdxLocal( iPlayer ) :
	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None:
		return [0,list(),list()]

	if( pPlayer.getNumCities() == 0 ) :
		return [0,list(),list()]

	localRevIdx = 0
	posList = list()
	negList = list()

	for i in range(gc.getNumTraitInfos()):
		if pPlayer.hasTrait(i):
			kTrait = gc.getTraitInfo(i)
			traitEffect = kTrait.getRevIdxLocal()
			if( traitEffect > 0 ) :
				negList.append( (traitEffect, kTrait.getDescription()) )
			elif( traitEffect < 0 ) :
				posList.append( (traitEffect, kTrait.getDescription()) )

			#CvUtil.pyPrint("  Rev - %s local effect: %d"%(kTrait.getDescription(),traitEffect))

			localRevIdx += traitEffect

	return [localRevIdx,posList,negList]


def getTraitsCivStabilityIndex( iPlayer ) :
	pPlayer = gc.getPlayer(iPlayer)

	civStabilityIdx = 0
	posList = list()
	negList = list()

	if pPlayer is None:
		return [civStabilityIdx,posList,negList]

	for iTrait in range(0,gc.getNumTraitInfos()) :
		kTrait = gc.getTraitInfo(iTrait)
		traitEffect = -kTrait.getRevIdxNational()

		if( pPlayer.hasTrait(iTrait) ) :
			if( traitEffect > 0 ) :
				posList.append( (traitEffect, kTrait.getDescription()) )
			elif( traitEffect < 0 ) :
				negList.append( (traitEffect, kTrait.getDescription()) )

			#CvUtil.pyPrint("  Rev - %s local effect: %d"%(kTrait.getDescription(),traitEffect))

			civStabilityIdx += traitEffect

	return [civStabilityIdx,posList,negList]


def getTraitsHolyCityEffects( iPlayer ) :

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None:
		return [0,0]

	if( pPlayer.getNumCities() == 0 ) :
		return [0,0]

	goodEffect = 0
	badEffect = 0

	for i in range(gc.getNumTraitInfos()):
		if pPlayer.hasTrait(i):
			kTrait = gc.getTraitInfo(i)
			goodEffect += kTrait.getRevIdxHolyCityGood()
			badEffect += kTrait.getRevIdxHolyCityBad()

	return [goodEffect,badEffect]


def getTraitsNationalityMod( iPlayer ) :

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None:
		return 0

	if( pPlayer.getNumCities() == 0 ) :
		return 0

	natMod = 0

	for i in range(gc.getNumTraitInfos()):
		if pPlayer.hasTrait(i):
			kTrait = gc.getTraitInfo(i)
			natMod += kTrait.getRevIdxNationalityMod()

	return natMod


def getTraitsReligionMods( iPlayer ) :

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None:
		return [0,0]

	if( pPlayer.getNumCities() == 0 ) :
		return [0,0]

	goodMod = 0
	badMod = 0

	for i in range(gc.getNumTraitInfos()):
		if pPlayer.hasTrait(i):
			kTrait = gc.getTraitInfo(i)
			goodMod += kTrait.getRevIdxGoodReligionMod()
			badMod += kTrait.getRevIdxBadReligionMod()

	return [goodMod,badMod]


def getTraitsDistanceMod( iPlayer ) :

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None:
		return 0

	if( pPlayer.getNumCities() == 0 ) :
		return 0

	distModifier = 0

	for i in range(gc.getNumTraitInfos()):
		if pPlayer.hasTrait(i):
			kTrait = gc.getTraitInfo(i)
			distModifier += kTrait.getRevIdxDistanceModifier()

	return distModifier