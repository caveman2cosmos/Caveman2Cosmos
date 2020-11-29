#-#-#
# Definitions for Revolution Mod
#
from CvPythonExtensions import *

## --------- XML variables used in the mod ----------- ##
# If your mod changes some of these XML variables, you'll need to find an appropriate replacement

# Civs
sXMLBarbarian = 'CIVILIZATION_BARBARIAN'

# Units
sXMLGeneral = 'UNIT_GREAT_GENERAL'

# Buildings
# Used by Rev when rebels capture a tiny city first
sXMLPalace = "BUILDING_PALACE"

# Techs
# Used by Rev, weight of nationality effects increases after discovery
sXMLNationalism = 'TECH_NATIONALISM'
# Used by Rev, weight of religious effects decreases after each discovery
sXMLLiberalism = 'TECH_LIBERALISM'
sXMLSciMethod = 'TECH_SCIENTIFIC_METHOD'

# Traits
# Used by Rev for AI decisions, BarbCiv to determine type of settling
sXMLAggressive = 'TRAIT_AGGRESSIVE'
sXMLSpiritual = 'TRAIT_SPIRITUAL'

# Goodies
sXMLGoodyMap = 'GOODY_MAP'

# Terrain
sXMLOcean = "TERRAIN_OCEAN"
sXMLCoast = "TERRAIN_COAST"

## ---------- Data structures for various objects ---------- ##

# Each city records its revolution status and history
cityData = dict()
# RevolutionIndex is the main measure of rebelliousness in a city, local records effects from factors in the city (as opposed to national factors)
cityData['PrevRevIndex'] = 0
cityData['RevIdxHistory'] = None
# Data about past revolutions
# TODO: change to list
cityData['RevolutionCiv'] = None
cityData['RevolutionTurn'] = None
# Counters to control timing for various features
cityData['WarningCounter'] = 0
cityData['SmallRevoltCounter'] = 0
# Bribe info
cityData['BribeTurn'] = None
cityData['TurnBribeCosts'] = None

revIdxHistKeyList = ['Happiness', 'Location', 'Colony', 'Nationality', 'Religion', 'Health', 'Garrison', 'Disorder', 'RevoltEffects', 'Events']
revIdxHistLen = 5

def initRevIdxHistory( ) :
	global revIdxHistKeyList

	revIdxHist = dict()
	for key in revIdxHistKeyList:
		revIdxHist[key] = [0]

	return revIdxHist


# Each player records its relation to other civs
# TODO: record national factors here?  create a civ wide measure of stability that interoperates with current city based approach?
playerData = dict()
# List of [iPlayer, iRevoltIdx] from which to spawn revolutionaries of this player on the next turn
playerData['SpawnList'] = list()
# Dictionary of revolts, held by index
playerData['RevoltDict'] = dict()
# Store civics to notice changes
playerData['CivicList'] = None
# Information about revolutions
playerData['RevolutionTurn'] = None
playerData['MotherlandID'] = None
playerData['JoinPlayerID'] = None
playerData['CapitalName'] = None

# Container for data passed by revolution popups
class RevoltData:

	def __init__( self, iPlayer, iRevTurn, cityList, revType, bPeaceful, specialDataDict = dict() ) :

		# Player whose cities are in revolt
		self.iPlayer = iPlayer
		self.iRevTurn = iRevTurn
		# List of cities revolting
		self.cityList = cityList
		# String describing revolution type
		self.revType = revType
		# Bool describing whether revolt is peaceful
		self.bPeaceful = bPeaceful

		# Includes special info for this revolution type (from list below, where self.___ would be specialDataDict['___'])
		self.specialDataDict = specialDataDict
		# self.iRevPlayer = iRevPlayer
		# self.bIsJoinWar = bIsJoinWar
		# self.iJoinPlayer = iJoinPlayer
		# self.iNewLeaderType = iNewLeaderType
		# self.newLeaderName = newLeaderName
		# self.bIsElection = bIsElection
		# self.iNewCivic = iNewCivic
		# self.iNewReligion = iNewReligion
		# self.iHappiness = iHappiness
		# self.iBuyOffCost = iBuyOffCost
		# self.vassalStyle = vassalStyle
		# self.bOfferPeace = bOfferPeace
		# self.bSwitchToRevs

		self.dict = self.toDict()


	def toDict( self ) :

		dataDict = dict()
		dataDict['iPlayer'] = self.iPlayer
		dataDict['cityList'] = self.cityList
		dataDict['revType'] = self.revType
		dataDict['bPeaceful'] = self.bPeaceful

		dataDict.update( self.specialDataDict )

		return dataDict

	def fromDict( self, sourceDict ) :
		# To use, pass Nones to RevoltData() then call this func with a full dict

		self.specialDataDict = dict()

		for [key,value] in sourceDict.iteritems() :
			if( key == 'iPlayer' ) :
				self.iPlayer = value
			elif( key == 'cityList' ) :
				self.cityList = value
			elif( key == 'revType' ) :
				self.revType = value
			elif( key == 'bPeaceful' ) :
				self.bPeaceful = value
			else :
				self.specialDataDict[key] = value

		self.dict = self.toDict()

## ---------- Revolution constants ---------- ##
# Changing these values is not recommended
revReadyDividend = 3
revReadyDivisor = 5
revReadyFrac = 0.6 # result of the two above, used by game text only, no OOS danger.
revInstigatorThreshold = 1000
alwaysViolentThreshold = 1700
badLocalThreshold = 10


## ---------- Popup number defines ---------- ##

# Revolution
revolutionPopup = 7000
revWatchPopup = 7001
joinHumanPopup = 7002
controlLostPopup = 7003
assimilationPopup = 7004
pickCityPopup = 7005
bribeCityPopup = 7006

# AIAutoPlay
toAIChooserPopup = 7050
abdicatePopup = 7051
pickHumanPopup = 7052

# ChangePlayer
changeCivPopup = 7060
changeHumanPopup = 7061
updateGraphicsPopup = 7062

## ---------- Misc defines ---------- ##

# Popup ids
EventKeyDown=6

#RevolutionDCM
## ---------- RevWatch defines ---------- ##
showTrend = 5