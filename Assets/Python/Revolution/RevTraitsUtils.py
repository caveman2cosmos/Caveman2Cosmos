# Traits functions for Revolution Mod
#
# by jdog5000
# Version 1.5

from CvPythonExtensions import *

GC = CyGlobalContext()

########################## Traits effect helper functions #####################

def getTraitsRevIdxLocal(iPlayer):
	pPlayer = GC.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.getNumCities():
		return [0, [], []]

	localRevIdx = 0
	posList = []
	negList = []

	for i in range(GC.getNumTraitInfos()):
		if pPlayer.hasTrait(i):
			kTrait = GC.getTraitInfo(i)
			traitEffect = kTrait.getRevIdxLocal()
			if traitEffect > 0:
				negList.append((traitEffect, kTrait.getDescription()))
			elif traitEffect < 0:
				posList.append((traitEffect, kTrait.getDescription()))

			localRevIdx += traitEffect

	return [localRevIdx, posList, negList]


def getTraitsCivStabilityIndex(iPlayer):
	pPlayer = GC.getPlayer(iPlayer)

	civStabilityIdx = 0
	posList = list()
	negList = list()

	if pPlayer is None:
		return [civStabilityIdx, posList, negList]

	for iTrait in range(GC.getNumTraitInfos()):
		kTrait = GC.getTraitInfo(iTrait)
		traitEffect = -kTrait.getRevIdxNational()

		if pPlayer.hasTrait(iTrait):
			if traitEffect > 0:
				posList.append((traitEffect, kTrait.getDescription()))
			elif traitEffect < 0:
				negList.append((traitEffect, kTrait.getDescription()))

			civStabilityIdx += traitEffect

	return [civStabilityIdx, posList, negList]


def getTraitsHolyCityEffects(iPlayer):

	pPlayer = GC.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.getNumCities():
		return [0, 0]

	goodEffect = 0
	badEffect = 0

	for i in range(GC.getNumTraitInfos()):
		if pPlayer.hasTrait(i):
			kTrait = GC.getTraitInfo(i)
			goodEffect += kTrait.getRevIdxHolyCityGood()
			badEffect += kTrait.getRevIdxHolyCityBad()

	return [goodEffect, badEffect]


def getTraitsNationalityMod(iPlayer):

	pPlayer = GC.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.getNumCities():
		return 0

	natMod = 0

	for i in range(GC.getNumTraitInfos()):
		if pPlayer.hasTrait(i):
			natMod += GC.getTraitInfo(i).getRevIdxNationalityMod()

	return natMod


def getTraitsReligionMods(iPlayer):

	pPlayer = GC.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.getNumCities():
		return [0,0]

	goodMod = 0
	badMod = 0

	for i in range(GC.getNumTraitInfos()):
		if pPlayer.hasTrait(i):
			kTrait = GC.getTraitInfo(i)
			goodMod += kTrait.getRevIdxGoodReligionMod()
			badMod += kTrait.getRevIdxBadReligionMod()

	return [goodMod, badMod]


def getTraitsDistanceMod( iPlayer ) :

	pPlayer = GC.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.getNumCities():
		return 0

	distModifier = 0

	for i in range(GC.getNumTraitInfos()):
		if pPlayer.hasTrait(i):
			distModifier += GC.getTraitInfo(i).getRevIdxDistanceModifier()

	return distModifier
