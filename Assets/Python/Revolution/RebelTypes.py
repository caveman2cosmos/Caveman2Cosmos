# RebelTypes.py
#
# by jdog5000
# Version 1.1

# This file sets up the most likely rebel civ types to appear when a revolution occurs in a particular civ.

from CvPythonExtensions import CyGlobalContext

# Initialize list to empty.
RebelTypeList = []

# This function actually sets up the lists of most preferable rebel types for each motherland civ type.
# All rebel types in the list are equally likely.
# No limit on length of rebel list, can be empty.
# If none of these are available, defaults to a similar art style civ.
def setup():
	global RebelTypeList
	RebelTypeList = []

	GC = CyGlobalContext()

	for iCiv in xrange(GC.getNumCivilizationInfos()):
		RebelTypeList.append([])

	iArabia			= GC.getInfoTypeForString('CIVILIZATION_ARABIA')
	iAssyria		= GC.getInfoTypeForString('CIVILIZATION_ASSYRIA')
	iAztec			= GC.getInfoTypeForString('CIVILIZATION_AZTEC')
	iBabylon		= GC.getInfoTypeForString('CIVILIZATION_BABYLON')
	iByzantium		= GC.getInfoTypeForString('CIVILIZATION_BYZANTIUM')
	iCarthage		= GC.getInfoTypeForString('CIVILIZATION_CARTHAGE')
	iCelt			= GC.getInfoTypeForString('CIVILIZATION_CELT')
	iChina			= GC.getInfoTypeForString('CIVILIZATION_CHINA')
	iEgypt			= GC.getInfoTypeForString('CIVILIZATION_EGYPT')
	iEngland		= GC.getInfoTypeForString('CIVILIZATION_ENGLAND')
	iEthiopia		= GC.getInfoTypeForString('CIVILIZATION_ETHIOPIA')
	iFrance			= GC.getInfoTypeForString('CIVILIZATION_FRANCE')
	iGermany		= GC.getInfoTypeForString('CIVILIZATION_GERMANY')
	iGreece			= GC.getInfoTypeForString('CIVILIZATION_GREECE')
	iHittites		= GC.getInfoTypeForString('CIVILIZATION_HITTITES')
	iHolyRoman		= GC.getInfoTypeForString('CIVILIZATION_HOLY_ROMAN')
	iInca			= GC.getInfoTypeForString('CIVILIZATION_INCA')
	iIndia			= GC.getInfoTypeForString('CIVILIZATION_INDIA')
	iIroquois		= GC.getInfoTypeForString('CIVILIZATION_IROQUOIS')
	iJapan			= GC.getInfoTypeForString('CIVILIZATION_JAPAN')
	iKhmer			= GC.getInfoTypeForString('CIVILIZATION_KHMER')
	iKorea			= GC.getInfoTypeForString('CIVILIZATION_KOREA')
	iMali			= GC.getInfoTypeForString('CIVILIZATION_MALI')
	iMaya			= GC.getInfoTypeForString('CIVILIZATION_MAYA')
	iMongol			= GC.getInfoTypeForString('CIVILIZATION_MONGOL')
	iNativeAmerica	= GC.getInfoTypeForString('CIVILIZATION_NATIVE_AMERICA')
	iNetherlands	= GC.getInfoTypeForString('CIVILIZATION_NETHERLANDS')
	iOttoman		= GC.getInfoTypeForString('CIVILIZATION_OTTOMAN')
	iPersia			= GC.getInfoTypeForString('CIVILIZATION_PERSIA')
	iPortugal		= GC.getInfoTypeForString('CIVILIZATION_PORTUGAL')
	iRome			= GC.getInfoTypeForString('CIVILIZATION_ROME')
	iRussia			= GC.getInfoTypeForString('CIVILIZATION_RUSSIA')
	iSiam			= GC.getInfoTypeForString('CIVILIZATION_SIAM')
	iSpain			= GC.getInfoTypeForString('CIVILIZATION_SPAIN')
	iSumeria		= GC.getInfoTypeForString('CIVILIZATION_SUMERIA')
	iUnitedStates	= GC.getInfoTypeForString('CIVILIZATION_UNITED_STATES')
	iViking			= GC.getInfoTypeForString('CIVILIZATION_VIKING')
	iZulu			= GC.getInfoTypeForString('CIVILIZATION_ZULU')

# Format is:
# RebelTypeList[iHomeland] = [iRebel1, iRebel2, iRebel3]

	RebelTypeList[iArabia]			= [iEgypt, iPersia, iOttoman, iBabylon, iSumeria, iAssyria]
	RebelTypeList[iAssyria]			= [iPersia, iBabylon, iSumeria, iHittites, iEgypt, iArabia]
	RebelTypeList[iAztec]			= [iInca, iSpain, iNativeAmerica, iMaya, iIroquois]
	RebelTypeList[iBabylon]			= [iSumeria, iPersia, iGreece, iEgypt, iArabia, iAssyria, iHittites]
	RebelTypeList[iByzantium]		= [iGreece, iRome, iOttoman, iHolyRoman, iHittites]
	RebelTypeList[iCarthage]		= [iRome, iGreece, iMali, iSpain]
	RebelTypeList[iCelt]			= [iFrance, iEngland, iGermany, iSpain]
	RebelTypeList[iChina]			= [iKorea, iMongol, iIndia, iJapan, iKhmer, iSiam]
	RebelTypeList[iEgypt]			= [iBabylon, iArabia, iPersia, iGreece, iEthiopia, iAssyria, iHittites]
	RebelTypeList[iEngland]			= [iUnitedStates, iIndia, iZulu, iNetherlands, iCelt]
	RebelTypeList[iEthiopia]		= [iEgypt, iMali, iZulu, iArabia]
	RebelTypeList[iFrance]			= [iGermany, iCelt, iEngland, iMali, iHolyRoman]
	RebelTypeList[iGermany]			= [iFrance, iRussia, iViking, iHolyRoman, iNetherlands]
	RebelTypeList[iGreece]			= [iRome, iPersia, iCarthage, iOttoman, iHittites]
	RebelTypeList[iHittites]		= [iAssyria, iEgypt, iPersia, iOttoman, iByzantium, iGreece, iBabylon]
	RebelTypeList[iHolyRoman]		= [iGermany, iFrance, iSpain, iByzantium]
	RebelTypeList[iInca]			= [iAztec, iSpain, iMaya, iNativeAmerica, iIroquois]
	RebelTypeList[iIndia]			= [iPersia, iSiam, iChina, iEngland, iKhmer]
	RebelTypeList[iIroquois]		= [iNativeAmerica, iAztec, iMaya, iInca, iUnitedStates]
	RebelTypeList[iJapan]			= [iKorea, iChina, iMongol, iKhmer, iSiam]
	RebelTypeList[iKhmer]			= [iSiam, iIndia, iChina, iMongol, iJapan]
	RebelTypeList[iKorea]			= [iJapan, iChina, iMongol, iKhmer]
	RebelTypeList[iMali]			= [iCarthage, iEgypt, iFrance, iZulu, iEthiopia]
	RebelTypeList[iMaya]			= [iAztec, iInca, iSpain, iNativeAmerica, iIroquois]
	RebelTypeList[iMongol]			= [iChina, iRussia, iPersia, iKorea, iSiam]
	RebelTypeList[iNativeAmerica]	= [iIroquois, iAztec, iMaya, iUnitedStates, iInca]
	RebelTypeList[iNetherlands]		= [iPortugal, iGermany, iEngland, iUnitedStates]
	RebelTypeList[iOttoman]			= [iPersia, iGreece, iGermany, iArabia, iByzantium, iHittites]
	RebelTypeList[iPersia]			= [iOttoman, iIndia, iMongol, iGreece, iSumeria, iBabylon, iAssyria, iHittites]
	RebelTypeList[iPortugal]		= [iSpain, iFrance, iNetherlands]
	RebelTypeList[iRome]			= [iGreece, iCarthage, iCelt, iEgypt, iByzantium]
	RebelTypeList[iRussia]			= [iViking, iGermany, iMongol, iPersia]
	RebelTypeList[iSiam]			= [iKhmer, iIndia, iChina, iMongol, iJapan]
	RebelTypeList[iSpain]			= [iPortugal, iArabia, iAztec, iInca, iHolyRoman]
	RebelTypeList[iSumeria]			= [iBabylon, iOttoman, iGreece, iPersia, iAssyria]
	RebelTypeList[iUnitedStates]	= [iEngland, iAztec, iNativeAmerica, iIroquois]
	RebelTypeList[iViking]			= [iRussia, iGermany, iEngland, iUnitedStates]
	RebelTypeList[iZulu]			= [iMali, iArabia, iEgypt, iEthiopia]