## GPUtil
##
## Utilities for dealing with Great People.
##
## MODDERS
##
##   There are four places where you must add information about your new great people.
##   This is also necessary if you assign GP points to buildings that don't normally get them,
##   for example GG points to Heroic Epic.
##
##     1. Unit Type
##     2. Named constant
##     3. Color
##     4. Icon (font glyph or string)
##
## Notes
##   - Must be initialized externally by calling init()
##
## Copyright (c) 2007-2009 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *

GC = CyGlobalContext()

# Map unit to font symbol
g_gpUnitTypes = [
	["UNIT_GREAT_HUNTER",		unichr(8483)],
	["UNIT_GREAT_ENGINEER",		unichr(8484)],
	["UNIT_MERCHANT",			unichr(8500)],
	["UNIT_SCIENTIST",			unichr(8501)],
	["UNIT_ARTIST",				unichr(8502)],
	["UNIT_GREAT_SPY",			unichr(8503)],
	["UNIT_GREAT_GENERAL",		unichr(8528)],
	["UNIT_GREAT_ADMIRAL",		unichr(8530)],
#	["UNIT_GREAT_AVIATOR",		unichr(8531)],
	["UNIT_GREAT_DETECTIVE",	unichr(8532)],
	["UNIT_DOCTOR",				unichr(8852)],
	["UNIT_PROPHET",			unichr(8857)],
	["UNIT_GREAT_STATESMAN",	unichr(8869)]
]

def init():
	# Translate KEY to unitType
	global g_gpUnitTypes
	aList = []
	for KEY, char in g_gpUnitTypes:
		aList.append([GC.getInfoTypeForString(KEY), char])
	g_gpUnitTypes = aList


def getUnitIcon(iUnit):
	for iUnitX, char in g_gpUnitTypes:
		if iUnit == iUnitX:
			return char
	print "[WARN] GPUtil.getUnitIcon\n\tNo GP icon for " + GC.getUnitInfo(iUnit).getType()
	return unichr(8862) # Generic great person symbol

def getDisplayCity():
	# Returns the city to display in the progress bar.
	pHeadSelectedCity = CyInterface().getHeadSelectedCity()
	if pHeadSelectedCity and pHeadSelectedCity.getTeam() == GC.getGame().getActiveTeam():
		CyCity = pHeadSelectedCity
		iTurns = getCityTurns(CyCity)
	else:
		CyCity, iTurns = findNextCity()
		if not CyCity:
			CyCity, iGPP = findMaxCity()
			iTurns = None
	return (CyCity, iTurns)

def findNextCity():
	city = iTurns = 0
	player = GC.getActivePlayer()
	iThreshold = player.greatPeopleThresholdNonMilitary()

	for cityX in player.cities():
		iRate = cityX.getGreatPeopleRate()
		if iRate > 0:
			iProgress = cityX.getGreatPeopleProgress()
			iTurnsX = (iThreshold - iProgress + iRate - 1) / iRate
			if not iTurns or iTurnsX < iTurns:
				iTurns = iTurnsX
				city = cityX

	return [city, iTurns]

def findMaxCity():
	city = iGPP = 0

	for cityX in GC.getActivePlayer().cities():
		iGPPX = cityX.getGreatPeopleProgress()
		if iGPPX > iGPP:
			iGPP = iGPPX
			city = cityX

	return [city, iGPP]

def getCityTurns(CyCity):
	if CyCity:
		CyPlayer = GC.getPlayer(CyCity.getOwner())
		iThreshold = CyPlayer.greatPeopleThresholdNonMilitary()
		iRate = CyCity.getGreatPeopleRate()
		if iRate > 0:
			iProgress = CyCity.getGreatPeopleProgress()
			iTurns = (iThreshold - iProgress + iRate - 1) / iRate
			return iTurns
	return None

def calcPercentages(CyCity):
	# Calc total rate
	iTotal = 0
	for iUnit, _ in g_gpUnitTypes:
		iTotal += CyCity.getGreatPeopleUnitProgress(iUnit)
	# Calc individual percentages based on rates and total
	percents = []
	if iTotal > 0:
		iLeftover = 100
		for iUnit in range(GC.getNumUnitInfos()):
			iProgress = CyCity.getGreatPeopleUnitProgress(iUnit)
			if iProgress > 0:
				iPercent = 100 * iProgress / iTotal
				iLeftover -= iPercent
				percents.append((iPercent, iUnit))
		# Add remaining from 100 to first in list to match Civ4
		if iLeftover > 0:
			percents[0] = (percents[0][0] + iLeftover, percents[0][1])
	return percents


# Displaying Progress
def getGreatPeopleText(CyCity, iGPTurns, iGPBarWidth, bGPBarTypesNone, bGPBarTypesOne, bIncludeCityName, uFont):

	if not CyCity:
		szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_NONE", (unichr(8862),))

	elif bGPBarTypesNone:

		if iGPTurns:

			if bIncludeCityName:
				szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_CITY_TURNS", (unichr(8862), CyCity.getName(), iGPTurns))
			else:
				szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_TURNS", (unichr(8862), iGPTurns))
		else:
			if bIncludeCityName:
				szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_CITY", (unichr(8862), CyCity.getName()))
			else:
				szText = unichr(8862)
	else:
		lPercents = calcPercentages(CyCity)
		iLength = len(lPercents)
		if not iLength:

			if iGPTurns:

				if bIncludeCityName:
					szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_CITY_TURNS", (unichr(8862), CyCity.getName(), iGPTurns))
				else:
					szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_TURNS", (unichr(8862), iGPTurns))
			else:
				if bIncludeCityName:
					szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_CITY", (unichr(8862), CyCity.getName()))
				else:
					szText = unichr(8862)
		else:
			lPercents.sort()
			lPercents.reverse()
			if bGPBarTypesOne or iLength == 1:

				iPercent, iUnit = lPercents[0]
				name = GC.getUnitInfo(iUnit).getDescription()
				if iGPTurns:

					if bIncludeCityName:
						szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_CITY_TURNS", (name, CyCity.getName(), iGPTurns))
					else:
						szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_TURNS", (name, iGPTurns))
				else:
					if bIncludeCityName:
						szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_CITY", (name, CyCity.getName()))
					else:
						szText = unicode(name)
			else:
				if iGPTurns:
					szText = CyTranslator().getText("INTERFACE_GREAT_PERSON_TURNS", (unichr(8862), iGPTurns))
				else:
					szText = unichr(8862) + ":"

				szTypes = " -"
				for iPercent, iUnit in lPercents:
					szNewTypes = szTypes + " %s%d%%" % (getUnitIcon(iUnit), iPercent)
					szNewText = szText + " -" + szTypes
					if CyInterface().determineWidth(uFont + szNewText) > iGPBarWidth:
						break
					szTypes = szNewTypes
				if iLength > 0:
					szText += szTypes
	return uFont + szText
