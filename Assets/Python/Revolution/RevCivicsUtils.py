# Civics functions for Revolution Mod
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

LOG_DEBUG = True

# civicsList[0] is a list of all civics of option type 0
civicsList = list()


def initCivicsList( ) :

	CvUtil.pyPrint("  Rev - Initializing Civics List")

	global civicsList

	civicsList = list()

	for i in range(0,gc.getNumCivicOptionInfos()) :
		civicsList.append(list())

	for i in range(0,gc.getNumCivicInfos()) :
		kCivic = gc.getCivicInfo(i)
		civicsList[kCivic.getCivicOptionType()].append(i)


########################## Civics effect helper functions #####################


def getCivicsRevIdxLocal( iPlayer ) :

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None:
		return [0,list(),list()]

	if( pPlayer.getNumCities() == 0 ) :
		return [0,list(),list()]

	if( len(civicsList) < gc.getNumCivicOptionInfos() ) :
		initCivicsList()

	localRevIdx = 0
	posList = list()
	negList = list()

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			civicEffect = kCivic.getRevIdxLocal()

			# Effect doubles for some when a much better alternative exists
			if( civicEffect > 0 and kCivic.getRevLaborFreedom() < -1 ) :
				for j in civicsList[kCivic.getCivicOptionType()] :
					if( pPlayer.canDoCivics(j) ) :
						kCivicOption = gc.getCivicInfo(j)
						if( kCivicOption.getRevLaborFreedom() > 1 ) :
							civicEffect = 2*civicEffect
							#CvUtil.pyPrint("  Rev - Effect of %s doubled to %d because can do %s"%(kCivic.getDescription(),civicEffect,kCivicOption.getDescription()))
							break

			if( civicEffect > 0 and kCivic.getRevDemocracyLevel() < -1 ) :
				for j in civicsList[kCivic.getCivicOptionType()] :
					if( pPlayer.canDoCivics(j) ) :
						kCivicOption = gc.getCivicInfo(j)
						if( kCivicOption.getRevDemocracyLevel() > 1 ) :
							civicEffect = 2*civicEffect
							#CvUtil.pyPrint("  Rev - Effect of %s doubled to %d because can do %s"%(kCivic.getDescription(),civicEffect,kCivicOption.getDescription()))
							break

			if( civicEffect > 0 ) :
				negList.append( (civicEffect, kCivic.getDescription()) )
			elif( civicEffect < 0 ) :
				posList.append( (civicEffect, kCivic.getDescription()) )

			#CvUtil.pyPrint("  Rev - %s local effect: %d"%(kCivic.getDescription(),civicEffect))

			localRevIdx += civicEffect

	return [localRevIdx,posList,negList]


def getCivicsCivStabilityIndex( iPlayer ) :
	import BugUtil
	pPlayer = gc.getPlayer(iPlayer)

	civStabilityIdx = 0
	posList = list()
	negList = list()

	if pPlayer is None:
		return [civStabilityIdx,posList,negList]

	if( len(civicsList) < gc.getNumCivicOptionInfos() ) :
		initCivicsList()

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			civicEffect = -kCivic.getRevIdxNational()
			BugUtil.debug("Civic Effect, %d", civicEffect)

			# Effect doubles for some when a much better alternative exists
			if( civicEffect < 0 and kCivic.getRevLaborFreedom() < -1 ) :
				for j in civicsList[kCivic.getCivicOptionType()] :
					if( pPlayer.canDoCivics(j) ) :
						kCivicOption = gc.getCivicInfo(j)
						if( kCivicOption.getRevLaborFreedom() > 1 ) :
							civicEffect = 2*civicEffect
							BugUtil.debug("Civic Effect, %d", civicEffect)
							#CvUtil.pyPrint("  Rev - Effect of %d doubled to %d because can do %s"%(kCivic.getDescription(),civicEffect,kCivicOption.getDescription()))
							break

			if( civicEffect < 0 and kCivic.getRevDemocracyLevel() < -1 ) :
				for j in civicsList[kCivic.getCivicOptionType()] :
					if( pPlayer.canDoCivics(j) ) :
						kCivicOption = gc.getCivicInfo(j)
						if( kCivicOption.getRevDemocracyLevel() > 1 ) :
							civicEffect = 2*civicEffect
							BugUtil.debug("Civic Effect, %d", civicEffect)
							#CvUtil.pyPrint("  Rev - Effect of %d doubled to %d because can do %s"%(kCivic.getDescription(),civicEffect,kCivicOption.getDescription()))
							break

			if( civicEffect > 0 ) :
				posList.append( (civicEffect, kCivic.getDescription()) )
			elif( civicEffect < 0 ) :
				negList.append( (civicEffect, kCivic.getDescription()) )

			#CvUtil.pyPrint("  Rev - %s local effect: %d"%(kCivic.getDescription(),civicEffect))

			civStabilityIdx += civicEffect

	return [civStabilityIdx,posList,negList]


def getCivicsHolyCityEffects( iPlayer ) :

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None:
		return [0,0]

	if( pPlayer.getNumCities() == 0 ) :
		return [0,0]

	goodEffect = 0
	badEffect = 0

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			goodEffect += kCivic.getRevIdxHolyCityGood()
			badEffect += kCivic.getRevIdxHolyCityBad()

	return [goodEffect,badEffect]

def getCivicsReligionMods( iPlayer ) :

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None:
		return [0,0]

	if( pPlayer.getNumCities() == 0 ) :
		return [0,0]

	goodMod = 0
	badMod = 0

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			goodMod += kCivic.getRevIdxGoodReligionMod()
			badMod += kCivic.getRevIdxBadReligionMod()

	return [goodMod,badMod]

def getCivicsDistanceMod( iPlayer ) :

	pPlayer = gc.getPlayer(iPlayer)
	distModifier = 0

	if pPlayer is None:
		return 0

	if( pPlayer.getNumCities() == 0 ) :
		return 0

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			distModifier += kCivic.getRevIdxDistanceModifier()

	return distModifier

def getCivicsNationalityMod( iPlayer ) :

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None:
		return 0

	if( pPlayer.getNumCities() == 0 ) :
		return 0

	natMod = 0

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			natMod += kCivic.getRevIdxNationalityMod()

	return natMod

def getCivicsViolentRevMod( iPlayer ) :

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None:
		return 0

	if( pPlayer.getNumCities() == 0 ) :
		return 0

	vioMod = 0

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			vioMod += kCivic.getRevViolentMod()

	return vioMod

def canDoCommunism( iPlayer ) :
	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive():
		return [False,None]

	for i in range(0,gc.getNumCivicInfos()) :
		kCivic = gc.getCivicInfo(i)
		if( kCivic.isCommunism() and pPlayer.canDoCivics(i) ) :
			if( not pPlayer.isCivic(i) ) :
				return [True,i]

	return [False,None]

def isCommunism( iPlayer ) :
	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive():
		return False

	for i in range(0,gc.getNumCivicInfos()) :
		kCivic = gc.getCivicInfo(i)
		if( kCivic.isCommunism() and pPlayer.isCivic(i) ) :
				return True

	return False

def canDoFreeSpeech( iPlayer ) :
	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive():
		return [False,None]

	for i in range(0,gc.getNumCivicInfos()) :
		kCivic = gc.getCivicInfo(i)
		if( kCivic.isFreeSpeech() and pPlayer.canDoCivics(i) ) :
			if( not pPlayer.isCivic(i) ) :
				return [True,i]

	return [False,None]

def isFreeSpeech( iPlayer ) :
	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive():
		return False

	for i in range(0,gc.getNumCivicInfos()) :
		kCivic = gc.getCivicInfo(i)
		if( kCivic.isFreeSpeech() and pPlayer.isCivic(i) ) :
				return True

	return False

def isCanDoElections( iPlayer ) :
	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive() or pPlayer.isNPC():
		return False

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			if( kCivic.isCanDoElection() ) :
				return True


	return False

def getReligiousFreedom( iPlayer ) :
	# Returns [freedom level, option type]

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive():
		return [0,None]

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			if( not kCivic.getRevReligiousFreedom() == 0 ) :
				return [kCivic.getRevReligiousFreedom(),i]

	return [0,None]


def getBestReligiousFreedom( iPlayer, relOptionType ) :
	# Returns [best level, civic type]

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive() or relOptionType == None:
		return [0,None]

	bestFreedom = -11
	bestCivic = None

	for i in civicsList[relOptionType] :
		kCivic = gc.getCivicInfo(i)
		civicFreedom = kCivic.getRevReligiousFreedom()
		if( pPlayer.canDoCivics(i) and not civicFreedom == 0 ) :
			if( kCivic.getRevReligiousFreedom() > bestFreedom ) :
				bestFreedom = civicFreedom
				bestCivic = i

	return [bestFreedom, bestCivic]

def getDemocracyLevel( iPlayer ) :
	# Returns [level, option type]

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive():
		return [0,None]

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			if( not kCivic.getRevDemocracyLevel() == 0 ) :
				return [kCivic.getRevDemocracyLevel(),i]

	return [0,None]


def getBestDemocracyLevel( iPlayer, optionType ) :
	# Returns [best level, civic type]

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive() or optionType is None:
		return [0,None]

	bestLevel = -11
	bestCivic = None

	for i in civicsList[optionType] :
		kCivic = gc.getCivicInfo(i)
		civicLevel = kCivic.getRevDemocracyLevel()
		if( pPlayer.canDoCivics(i) and not civicLevel == 0 ) :
			if( civicLevel > bestLevel ) :
				bestLevel = civicLevel
				bestCivic = i

	return [bestLevel, bestCivic]

def getLaborFreedom( iPlayer ) :
	# Returns [level, option type]

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive():
		return [0,None]

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			if( not kCivic.getRevLaborFreedom() == 0 ) :
				return [kCivic.getRevLaborFreedom(),i]

	return [0,None]


def getBestLaborFreedom( iPlayer, optionType ) :
	# Returns [best level, civic type]

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive() or optionType is None:
		return [0,None]

	bestLevel = -11
	bestCivic = None

	for i in civicsList[optionType] :
		kCivic = gc.getCivicInfo(i)
		civicLevel = kCivic.getRevLaborFreedom()
		if( pPlayer.canDoCivics(i) and not civicLevel == 0 ) :
			if( civicLevel > bestLevel ) :
				bestLevel = civicLevel
				bestCivic = i

	return [bestLevel, bestCivic]

def getEnvironmentalProtection( iPlayer ) :
	# Returns [level, option type]

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive():
		return [0,None]

	for i in range(0,gc.getNumCivicOptionInfos()) :
		iCivic = pPlayer.getCivics(i)
		if( iCivic >= 0 ) :
			kCivic = gc.getCivicInfo(iCivic)
			if( not kCivic.getRevEnvironmentalProtection() == 0 ) :
				return [kCivic.getRevEnvironmentalProtection(),i]

	return [0,None]


def getBestEnvironmentalProtection( iPlayer, optionType ) :
	# Returns [best level, civic type]

	pPlayer = gc.getPlayer(iPlayer)

	if pPlayer is None or not pPlayer.isAlive() or optionType is None:
		return [0,None]

	bestLevel = -11
	bestCivic = None

	for i in civicsList[optionType] :
		kCivic = gc.getCivicInfo(i)
		civicLevel = kCivic.getRevEnvironmentalProtection()
		if( pPlayer.canDoCivics(i) and not civicLevel == 0 ) :
			if( civicLevel > bestLevel ) :
				bestLevel = civicLevel
				bestCivic = i

	return [bestLevel, bestCivic]