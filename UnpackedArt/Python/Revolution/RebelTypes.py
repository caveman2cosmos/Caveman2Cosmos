# RebelTypes.py
#
# by jdog5000
# Version 1.1

# This file sets up the most likely rebel civ types to appear when a revolution occurs in a particular civ.

from CvPythonExtensions import *
import CvUtil

gc = CyGlobalContext()

# Initialize list to empty.
RebelTypeList = list()

# This function actually sets up the lists of most preferable rebel types for each motherland civ type.
# All rebel types in the list are equally likely.
# No limit on length of rebel list, can be empty.
# If none of these are available, defaults to a similar art style civ.
def setup():

	global RebelTypeList

	RebelTypeList = list()

	for idx in range(0, gc.getNumCivilizationInfos()):
		RebelTypeList.append(list())

	try:
		iArabia			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_ARABIA')
		iAssyria		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_ASSYRIA')
		iAztec			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_AZTEC')
		iBabylon		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_BABYLON')
		iByzantium		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_BYZANTIUM')
		iCarthage		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_CARTHAGE')
		iCelt			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_CELT')
		iChina			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_CHINA')
		iEgypt			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_EGYPT')
		iEngland		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_ENGLAND')
		iEthiopia		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_ETHIOPIA')
		iFrance			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_FRANCE')
		iGermany		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_GERMANY')
		iGreece			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_GREECE')
		iHittites		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_HITTITES')
		iHolyRoman		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_HOLY_ROMAN')
		iInca			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_INCA')
		iIndia			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_INDIA')
		iIroquois		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_IROQUOIS')
		iJapan			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_JAPAN')
		iKhmer			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_KHMER')
		iKorea			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_KOREA')
		iMali			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_MALI')
		iMaya			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_MAYA')
		iMongol			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_MONGOL')
		iNativeAmerica	= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_NATIVE_AMERICA')
		iNetherlands	= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_NETHERLANDS')
		iOttoman		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_OTTOMAN')
		iPersia			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_PERSIA')
		iPortugal		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_PORTUGAL')
		iRome			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_ROME')
		iRussia			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_RUSSIA')
		iSiam			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_SIAM')
		iSpain			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_SPAIN')
		iSumeria		= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_SUMERIA')
		iUnitedStates	= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_UNITED_STATES')
		iViking			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_VIKING')
		iZulu			= CvUtil.findInfoTypeNum(gc.getCivilizationInfo,gc.getNumCivilizationInfos(),'CIVILIZATION_ZULU')
	except:
		print "Error! Civ not found. -RebelTypes.py"

# Format is:
# RebelTypeList[iHomeland] = [iRebel1, iRebel2, iRebel3]
	try:
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
	except:
		print "Error!  Rebel types not found, no short lists available"