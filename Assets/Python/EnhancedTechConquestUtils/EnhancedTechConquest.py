## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## TechConquest by Bhruic
## Updated by TheLopez
## Updated by Dancing Hoskuld to allow language support
##	see also EnhancedTechConquest game text xml file.
##
from CvPythonExtensions import *
import CvUtil

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

# Change the value to True if the conquering civilization can receive
# technology without the appropriate prerequisites or ignore their civilization technology restrictions.
g_bCheckPrereq = True

# Increase or decrease the value to change the base technology transfer percentage amount.
g_iBasePercent = 0

# Increase or decrease the value to change the percent amount per city
# population that will be used to transfer technology to the new owners of the conquered city.
g_iPopPercent = 2


def loadConfigurationData():
	global g_bCheckPrereq
	global g_iBasePercent
	global g_iPopPercent

	import SystemPaths
	path = SystemPaths.modDir + "\Caveman2Cosmos Config.ini"
	import ConfigParser
	Config = ConfigParser.ConfigParser()
	Config.read(path)

	if Config:
		g_bCheckPrereq = Config.get("Enhanced Tech Conquest", "Check Prereq")
		if g_bCheckPrereq in ("False", "false", "0"):
			g_bCheckPrereq = False
		else:
			g_bCheckPrereq = True
		g_iBasePercent = Config.get("Enhanced Tech Conquest", "Base Percent")
		if g_iBasePercent.isdigit():
			g_iBasePercent = int(g_iBasePercent)
		else:
			g_iBasePercent = 0
		g_iPopPercent = Config.get("Enhanced Tech Conquest", "Population Percent")
		if g_iPopPercent.isdigit():
			g_iPopPercent = int(g_iPopPercent)
		else:
			g_iPopPercent = 2

	sprint  = "Enhanced Tech Conquest:\n"
	sprint += "\tTechnology Transfer Ignore Prereq = %s\n" %str(g_bCheckPrereq)
	sprint += "\tBase Technology Transfer Percent. = %d\n" %g_iBasePercent
	sprint += "\tPercentage Per City Population... = %d" %g_iPopPercent
	print sprint

class EnhancedTechConquest:

	def onCityAcquired(self, argsList):
		iOwnerOld, iOwnerNew, CyCity, bConquest, bTrade = argsList
		if not bConquest: return

		iBasePercent = g_iBasePercent
		iPopPercent = g_iPopPercent
		if iBasePercent < 1 and iPopPercent < 1: return

		CyPlayerN = GC.getPlayer(iOwnerNew)
		if CyPlayerN.isNPC(): return

		if iPopPercent < 0:
			iPopPercent = 0
		elif iPopPercent > 100:
			iPopPercent = 100

		CyPlayerO = GC.getPlayer(iOwnerOld)
		CyTeamO = GC.getTeam(CyPlayerO.getTeam())
		CyTeamN = GC.getTeam(CyPlayerN.getTeam())

		bCheckPrereq = g_bCheckPrereq
		aList = []
		iTechsBehind = 0
		for iTech in range(GC.getNumTechInfos()):
			# Continue if the conquering team does have the tech
			if CyTeamN.isHasTech(iTech):
				continue
			# Continue if the old team doesn't have the tech
			if not CyTeamO.isHasTech(iTech):
				continue
			iTechsBehind += 1
			# Continue if the conquerer cannot research the technology
			if bCheckPrereq and not CyPlayerN.canResearch(iTech):
				continue
			# Append the technology to the possible technology list
			iCost = CyTeamN.getResearchCost(iTech)
			iProgress = CyTeamN.getResearchProgress(iTech)
			iRemaining = iCost - iProgress - 1
			if not iRemaining:
				continue
			# Append the technology to the possible technology list
			aList.append((iTech, iCost, iRemaining))

		if aList:

			iBasePercent += iTechsBehind
			charBeaker = GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar()
			iPopulation = CyCity.getPopulation() + 1
			fForce = (1 + iTechsBehind/10.0) * iPopulation / (CyPlayerO.getTotalPopulation() + iPopulation)

			iMax = (iPopulation * iPopPercent)
			iCount = 0
			iLen = len(aList)
			szTxt = ""
			while iLen:
				iTech, iCost, iRemaining = aList.pop(GAME.getSorenRandNum(iLen, "random"))
				iLen -= 1
				# Get the total number of technology points that will be transfered to the new city owner
				fTemp = 0
				if iPopPercent:
					for i in xrange(iPopulation):
						fTemp += 100 * (1.0 + GAME.getSorenRandNum(iPopPercent, "TechConquest")) / iMax

				fPercent = iBasePercent + fTemp * fForce

				iBeakers = int(iCost * fPercent / (20 * (iCount + 5)))

				if iBeakers < 1: continue
				if iBeakers > iRemaining:
					iBeakers = iRemaining

				# Increase the research progress for the new city owner
				CyTeamN.changeResearchProgress(iTech, iBeakers, iOwnerNew)

				szTxt += "\n\t" + GC.getTechInfo(iTech).getDescription() + u" <-> %i%c" %(iBeakers, charBeaker)
				iCount += 1

			if iOwnerNew == GAME.getActivePlayer():

				if iCount: # Inform the player they got some new technology points
					szTxt = TRNSLTR.getText("TXT_KEY_ENHANCED_TECH_CONQUEST_SUCESS", ()) %(CyCity.getName(), szTxt)
				else: # Inform the player they didn't get any new technologies
					szTxt = TRNSLTR.getText("TXT_KEY_ENHANCED_TECH_CONQUEST_FAIL", ()) + " %s" %(CyCity.getName())

				artPath = GC.getCivilizationInfo(CyPlayerO.getCivilizationType()).getButton()
				CvUtil.sendMessage(szTxt, iOwnerNew, 20, artPath, ColorTypes(12), CyCity.getX(), CyCity.getY(), True, True)
