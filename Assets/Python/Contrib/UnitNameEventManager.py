## UnitName
## by Ruff_Hi
## for BUG Mod
##-------------------------------------------------------------------
## Naming Convention
##  - ^civ4^ - no naming convention, uses standard civ4
##  - ^rd^ - random name
##  - ^rc^ - random civ related name
##  - ^ct^ - City
##  - ^cv^ - Civilization
##  - ^ut^ - unit (eg Archer)
##  - ^cb^ - combat type (Melee)
##  - ^dm^ - domain (Water)
##  - ^ld^ - leader
##  - ^cnt[f][r]^ - counting code, if the code isn't there - return 'ALL'
##  - where
##  -   r = 'a' means count across all units (just increments)
##  -   r = 'u' means count across same unit (increments based on unit)
##  -   r = 'c' means count across same city (increments based on city)
##  -   r = 't' means count across same unit / city (increments based on unit / city)
##  -   r = 'b' means count across same combat type (increments based on combat type)
##  -   r = 'd' means count across same domain (increments based on domain)
##  -   r = other, means count across units that have the same code
##  - ^tt1[f][x:y]^ - total where the total is a random number between x and y (number)
##  - ^tt2[f][x]^ - total count (starts at x, incremented by 1 each time ^tt is reset to 1)
##
## Where [f] can be either 's', 'A', 'a', 'p', 'g', 'n', 'o' or 'r' for ...
##  - silent (not shown)
##  - alpha (A, B, C, D, ...)
##  - alpha (a, b, c, d, ...)
##  - phonetic (alpha, bravo, charlie, delta, echo, ...)
##  - greek (alpha, beta, gamma, delta, epsilon, ...)
##  - number
##  - ordinal (1st, 2nd, 3rd, 4th, ...)
##  - roman (I, IV, V, X, ...)
##
## Coding Steps
##
## 1. check if a unit exists, if not, do nothing
## 2. call unit name engine
## 3. update unit name if returned name is not NULL
##
## Unit name engine:
##
## 1. get naming convention from ini file
##    a. try to get the advanced naming convention
##    b. if it returns 'DEFAULT', then get the combat based naming convention
##    c. if naming convention is 'DEFAULT', get default naming convention
##
## 2. determine if you have 'civ naming' or no valid naming codes in your naming convention, if YES, return 'NULL'
## 3. determine if you have 'random' in your naming convention, if YES, call random engine and return value
## 4. determine if you have 'random civ related' in your naming convention, if YES, call random civ related engine and return value
##
## 5. swap out fixed items (ie unit, combat type, domain, leader, civilization, city, etc) from naming convention
##
## 6. determine if you have any count items in naming convention; return if FALSE
##
## 7. determine key for counting (this information is stored in the save file)
## a. get latest count from save (if not found, initilize)
## b. increment count by 1
## c. test against total (if required), adjust total and 2nd total if required
##
## 8. format count items
##
## 9. replace formatted count items in naming convention
##
## 10. return name
##-------------------------------------------------------------------

from CvPythonExtensions import *
import CvUtil
import BugUtil
import BugCore
import Roman
import RandomNameUtils
import random
import BugData


SD_MOD_ID = "UnitCnt"
RENAME_EVENT_ID = CvUtil.getNewEventID()

GC = CyGlobalContext()

UnitNamingOpt = BugCore.game.UnitNaming

phonetic_array = ['Alpha', 'Bravo', 'Charlie', 'Delta', 'Echo', 'Foxtrot', 'Golf', 'Hotel', 'India', 'Juliet', 'Kilo', 'Lima', 'Mike',
                  'November', 'Oscar', 'Papa', 'Quebec', 'Romeo', 'Sierra', 'Tango', 'Uniform', 'Victor', 'Whiskey', 'Xray', 'Yankee', 'Zulu']

greek_array = ['Alpha', 'Beta', 'Gamma', 'Delta', 'Epsilon', 'Zeta', 'Eta', 'Theta', 'Iota', 'Kappa', 'Lambda', 'Mu', 'Nu', 'Xi',
               'Omicron', 'Pi', 'Rho', 'Sigma', 'Tau', 'Upsilon', 'Phi', 'Chi', 'Psi', 'Omega']

ordinal_array = 'th st nd rd th th th th th th'.split()


class UnitNameEventManager:

	def __init__(self, eventManager):
		eventManager.addEventHandler("unitBuilt", self.onUnitBuilt)

	def onUnitBuilt(self, argsList):
		pUnit = argsList[1]
		if pUnit is None: return

		iPlayer = pUnit.getOwner()
		if iPlayer != GC.getGame().getActivePlayer() or not UnitNamingOpt.isEnabled():
			# Not having the same name for a unit will cause OOS issues if unit names are considered part of the game-state.
			# Remove "iPlayer != GC.getGame().getActivePlayer()" to fix it if that's the case.
			return

		if UnitNamingOpt.isAdvanced():
			zsUnitNameConv = UnitNamingOpt.getByEraAndClass(GC.getEraInfo(GC.getPlayer(iPlayer).getCurrentEra()).getType()[4:], GC.getUnitInfo(pUnit.getUnitType()).getType()[5:])
		else: zsUnitNameConv = UnitNamingOpt.getDefault()

		zsUnitName = UnitReName().getUnitName(zsUnitNameConv, pUnit, argsList[0], True)

		if zsUnitName:
			pUnit.setName(zsUnitName)


class UnitReName(object):

	def getUnitName(self, sUnitNameConv, pUnit, pCity, bIncrementCounter):
		zsName = sUnitNameConv

##  - ^civ4^ - no naming convention, uses standard civ4
#		check if Civ4 naming convention is required
		if zsName.find("^civ4^") != -1:
			return ""

		iPlayer = pUnit.getOwner()
		pPlayer = GC.getPlayer(iPlayer)

		zsCiv = pPlayer.getCivilizationAdjective(0)
		zsLeader = pPlayer.getName()
		zsUnitCombat = self.getUnitCombat(pUnit)
		zsUnitDomain = BugUtil.getPlainText("TXT_KEY_BUG_UNIT_NAMING_" + GC.getDomainInfo(pUnit.getDomainType()).getType())
		zsUnit = GC.getUnitInfo(pUnit.getUnitType()).getDescription()
		if pCity:
			zsCity = pCity.getName()
		else: zsCity = ""

##  - ^rd^ - random name
#		check if random naming convention is required
		if not (zsName.find("^rd^") == -1):
			zsRandomName = RandomNameUtils.getRandomName()
			zsName = zsName.replace("^rd^", zsRandomName)

##  - ^rc^ - random civ related name
#		check if random civ related naming convention is required
		if not (zsName.find("^rc^") == -1):
			zsRandomName = RandomNameUtils.getRandomCivilizationName(pPlayer.getCivilizationType())
			zsName = zsName.replace("^rc^", zsRandomName)

##  - ^ct^ - City
##  - ^cv^ - Civilization
##  - ^ut^ - unit (eg Archer)
##  - ^cb^ - combat type (Melee)
##  - ^dm^ - domain (Water)
##  - ^ld^ - leader
#		replace the fixed items in the naming conv
		zsName = zsName.replace("^ct^", zsCity)
		zsName = zsName.replace("^cv^", zsCiv)
		zsName = zsName.replace("^ut^", zsUnit)
		zsName = zsName.replace("^cb^", zsUnitCombat)
		zsName = zsName.replace("^dm^", zsUnitDomain)
		zsName = zsName.replace("^ld^", zsLeader)


#		check if there are any more codes to swap out, return if not
		counters = BugData.getGameData().getTable(SD_MOD_ID)
		while zsName.find("^cnt") != -1:
#			determine what I am counting across
			zsSDKey = self.getCounter(zsName)
			if zsSDKey == "UNIT":		zsSDKey = zsSDKey + zsUnit
			elif zsSDKey == "COMBAT":	zsSDKey = zsSDKey + zsUnitCombat
			elif zsSDKey == "CITY":		zsSDKey = zsSDKey + zsCity
			elif zsSDKey == "UNITCITY": zsSDKey = zsSDKey + zsUnit + zsCity
			elif zsSDKey == "DOMAIN":	zsSDKey = zsSDKey + zsUnitDomain


#			see if we have already started this counter
			if (not counters.hasTable(zsSDKey)):
				#Since no record create entries
				ziCnt = 0
				ziTT1 = self.getTotal1(zsName)
				ziTT2 = self.getTotal2(zsName)
				counter = counters.getTable(zsSDKey)
			else:
#				get the count values
				counter = counters.getTable(zsSDKey)
				ziCnt = counter["cnt"]
				ziTT1 = counter["tt1"]
				ziTT2 = counter["tt2"]

#			increment count, adjust totals if required
			if bIncrementCounter:
				ziCnt = ziCnt + 1
				if (ziCnt > ziTT1
				and ziTT1 > 0):
					ziCnt = 1
					ziTT1 = self.getTotal1(zsName)
					ziTT2 = ziTT2 + 1
#				store the new values
				counter["cnt"] = ziCnt
				counter["tt1"] = ziTT1
				counter["tt2"] = ziTT2

#			swap out the count code items for count value
			zsName = self.swapCountCode(zsName, "^cnt", ziCnt)
			zsName = self.swapCountCode(zsName, "^tt1", ziTT1)
			zsName = self.swapCountCode(zsName, "^tt2", ziTT2)

		return zsName


	def getUnitCombat(self, pUnit):
		if pUnit is None:
			return "UNITCOMBAT_None"

		infoUnitCombat = GC.getUnitCombatInfo(pUnit.getUnitCombatType())

		if infoUnitCombat == None:
			return "UNITCOMBAT_None"

		return infoUnitCombat.getType()


	def getCounter(self, conv):
##  - ^cnt[f][r]^ - counting code, if the code isn't there - return 'ALL'
##  - where
##  -   r = 'a' means count across all units (just increments)
##  -   r = 'u' means count across same unit (increments based on unit)
##  -   r = 'c' means count across same city (increments based on city)
##  -   r = 't' means count across same unit / city (increments based on unit / city)
##  -   r = 'b' means count across same combat type (increments based on combat type)
##  -   r = 'd' means count across same domain (increments based on domain)
##  -   r = other, means count across units that have the same code

		ziStart = conv.find("^cnt[")
		if ziStart == -1:  # ie not found
			return "ALL"

#		locate and extract the code
		ziStart = conv.find("[", conv.find("[", ziStart) + 1)
		ziEnd = conv.find("]", ziStart+1)
		zsValue = conv[ziStart+1:ziEnd]

		if zsValue == "a": return "ALL"
		if zsValue == "u": return "UNIT"
		if zsValue == "c": return "CITY"
		if zsValue == "t": return "UNITCITY"
		if zsValue == "b": return "COMBAT"
		if zsValue == "d": return "DOMAIN"
		return zsValue

	def getTotal1(self, conv):
##  - ^tt1[f][x:y]^ - total where the total is a random number between x and y (number)

#		return 'not found' indicator
		ziStart = conv.find("^tt1[")
		if ziStart == -1:
			return -1

#		locate and extract the 'low' value
		ziStart = conv.find("[", conv.find("[", ziStart) + 1)
		ziEnd = conv.find(":",ziStart)
		ziLow = int(conv[ziStart + 1:ziEnd])
		if (ziLow < 1): ziLow = 1

#		locate and extract the 'high' value
		ziStart = ziEnd
		ziEnd = conv.find("]",ziStart)
		ziHigh = int(conv[ziStart + 1:ziEnd])
		if (ziHigh < 1): ziHigh = 1

#		check that the user isn't an idiot
		if (ziLow > ziHigh): return ziLow

#		return the value
		return random.randint(ziLow, ziHigh)


	def getTotal2(self, conv):
##  - ^tt2[f][x]^ - total count (starts at x, incremented by 1 each time ^tt is reset to 1)

#		return 'not found' indicator
		ziStart = conv.find("^tt2[")
		if ziStart == -1:
			return -1

#		locate and extract the value
		ziStart = conv.find("[", conv.find("[", ziStart) + 1)
		ziEnd = conv.find("]",ziStart)
		ziValue = int(conv[ziStart + 1:ziEnd])

		if ziValue < 1:
			return 1
		return ziValue


	def getNumberFormat(self, conv, searchStr):
		ziStart = conv.find("[", conv.find(searchStr))
		if ziStart == -1:
			return "s"   # s for silent, hides number
		return conv[ziStart + 1:ziStart + 2]


	def getCountCode(self, conv, searchStr):
		ziStart = conv.find(searchStr)
		if ziStart == -1:
			return ""

		ziEnd = conv.find("^", ziStart + 1)
		return conv[ziStart:ziEnd + 1]


	def swapCountCode(self, conv, searchStr, iCnt):

		# return if iCnt is negative (this means that the code is not in the unitnameconv)
		if iCnt < 0: return conv

		zsCntCode = self.getCountCode(conv, searchStr)

		if zsCntCode:
			return conv.replace(zsCntCode, self.FormatNumber(self.getNumberFormat(conv, searchStr), iCnt))
		return conv


	def FormatNumber(self, fmt, i):
		if (i < 1):
			# ensure i is not under lower bound; each format enforces its own upper bound when necessary
			i = 1
		if (fmt == "s"):     # silent
			return ""
		if (fmt == "a"):   # lower case alpha
			return chr(97 + ((i - 1) % 26))
		if (fmt == "A"):   # upper case alpha
			return chr(65 + ((i - 1) % 26))
		if (fmt == "p"):   # phonetic
			return phonetic_array[(i - 1) % 26]
		if (fmt == "g"):   # greek
			return greek_array[(i - 1) % 24]
		if (fmt == "n"):   # number
			return str(i)
		if (fmt == "o"):   # ordinal
			return self.getOrdinal(i)
		if (fmt == "r"):   # roman
			return Roman.toRoman(i)

		return str(i)


	def getOrdinal(self, i):
		if i % 100 in (11, 12, 13): #special case
			return '%dth' % i
		return str(i) + ordinal_array[i % 10]
