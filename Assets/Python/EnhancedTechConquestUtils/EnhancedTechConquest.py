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
#g_bCheckPrereq = True

# Increase or decrease the value to change the base technology transfer percentage amount.
#g_iBasePercent = 0

# Increase or decrease the value to change the percent amount per city
# population that will be used to transfer technology to the new owners of the conquered city.
#g_iPopPercent = 10

# Significance of the number of techs you are behind on the calculation - Range(0, infinity)
#g_iTechBehindPercent = 200

def loadConfigurationData():
	global g_bCheckPrereq
	global g_iBasePercent
	global g_iPopPercent
	global g_iTechBehindPercent

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
			g_iPopPercent = 10

		g_iTechBehindPercent = Config.get("Enhanced Tech Conquest", "Techs Behind Percent")
		if g_iTechBehindPercent.isdigit():
			g_iTechBehindPercent = int(g_iPopPercent)
		else:
			g_iTechBehindPercent = 200

	sprint  = "Enhanced Tech Conquest:\n"
	sprint += "\tTechnology Transfer Ignore Prereq = %s\n" %str(g_bCheckPrereq)
	sprint += "\tBase Technology Transfer Percent. = %d\n" %g_iBasePercent
	sprint += "\tPercentage Per City Population... = %d" %g_iPopPercent
	sprint += "\tPercentage Per Tech Behind... = %d" %g_iTechBehindPercent
	print sprint

class EnhancedTechConquest:

	def onCityAcquired(self, argsList):
		#iOwnerOld, iOwnerNew, city, bConquest, bTrade, bAutoRaze = argsList
		# if not bConquest
		if not argsList[3]: return

		fBasePercent = g_iBasePercent
		if fBasePercent < 1 and g_iTechBehindPercent < 1: return

		iOwnerNew = argsList[1]
		CyPlayerN = GC.getPlayer(iOwnerNew)
		if CyPlayerN.isNPC(): return

		iPopPercent = g_iPopPercent
		if iPopPercent < 0:
			iPopPercent = 0
		elif iPopPercent > 100:
			iPopPercent = 100

		CyPlayerO = GC.getPlayer(argsList[0]) # old owner
		CyTeamO = GC.getTeam(CyPlayerO.getTeam())
		CyTeamN = GC.getTeam(CyPlayerN.getTeam())

		bCheckPrereq = g_bCheckPrereq
		aList = []
		iTechsBehind = 0
		for iTech in range(GC.getNumTechInfos()):
			# Continue if the conquering team does have the tech
			# Continue if the old team doesn't have the tech
			# Continue if the new team can't ever get the tech
			if (CyTeamN.isHasTech(iTech)
			or not CyTeamO.isHasTech(iTech)
			or not CyPlayerN.canResearch(iTech, False, False)):
				continue
			iTechsBehind += 1
			# Continue if the conquerer cannot research the technology
			if bCheckPrereq and not CyPlayerN.canResearch(iTech, True, True):
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

			city = argsList[2]
			fBasePercent += iTechsBehind * g_iTechBehindPercent / 100.0
			charBeaker = GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar()
			iPopulation = city.getPopulation()
			if iPopPercent:
				fForce = (1 + iTechsBehind/10.0) * iPopulation / (CyPlayerO.getTotalPopulation() + iPopulation)

			iMax = (iPopulation * iPopPercent)
			iCount = 0
			iLen = len(aList)
			szTxt = ""
			while iLen:
				iTech, iCost, iRemaining = aList.pop(GAME.getSorenRandNum(iLen, "random"))
				iLen -= 1
				# Get the total number of technology points that will be transfered to the new city owner
				if iPopPercent:
					fTemp = 0
					for i in xrange(iPopulation):
						fTemp += 100 * (1.0 + GAME.getSorenRandNum(iPopPercent, "TechConquest")) / iMax

					fPercent = fBasePercent + fTemp * fForce
					if fPercent <= 0: continue
				else:
					fPercent = fBasePercent
					if fPercent <= 0: return

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
					szTxt = TRNSLTR.getText("TXT_KEY_ENHANCED_TECH_CONQUEST_SUCESS", ()) %(city.getName(), szTxt)
				else: # Inform the player they didn't get any new technologies
					szTxt = TRNSLTR.getText("TXT_KEY_ENHANCED_TECH_CONQUEST_FAIL", ()) + " %s" %(city.getName())

				CvUtil.sendMessage(
					szTxt, iOwnerNew, 20,
					GC.getCivilizationInfo(CyPlayerO.getCivilizationType()).getButton(),
					ColorTypes(GC.getInfoTypeForString("COLOR_GREEN")),
					city.getX(), city.getY(), True, True
				)
