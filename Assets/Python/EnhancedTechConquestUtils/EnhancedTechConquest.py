from CvPythonExtensions import *
import CvUtil

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

def loadConfigurationData():
	global g_bCheckPrereq
	global g_iBasePercent
	global g_iPopPercent
	global g_iTechBehindPercent
	global g_iFinalModifier

	import SystemPaths
	path = SystemPaths.modDir + "\\Assets\\Caveman2Cosmos Config.ini"
	import ConfigParser
	Config = ConfigParser.ConfigParser()
	Config.read(path)

	if Config:
		# If this is set to False, then you can get beakers towards techs you don't have the prerequisites for.
		g_bCheckPrereq = Config.get("Enhanced Tech Conquest", "Check Prereq")
		if g_bCheckPrereq in ("False", "false", "0"):
			g_bCheckPrereq = False
		else:
			g_bCheckPrereq = True

		# Base percentage is an offset value that does not care about how many techs ahead or how big the city in question is.
		g_iBasePercent = Config.get("Enhanced Tech Conquest", "Base Percent")
		if g_iBasePercent.isdigit():
			g_iBasePercent = int(g_iBasePercent)
		else:
			g_iBasePercent = 0

		# Significance of city size on the final outcome - Range(1, 100)
		g_iPopPercent = Config.get("Enhanced Tech Conquest", "Population Percent")
		if g_iPopPercent.isdigit():
			g_iPopPercent = int(g_iPopPercent)
		else:
			g_iPopPercent = 15

		# Significance of the number of techs you are behind the player you conquer the city from on the final outcome.
		g_iTechBehindPercent = Config.get("Enhanced Tech Conquest", "Techs Behind Percent")
		if g_iTechBehindPercent.isdigit():
			g_iTechBehindPercent = int(g_iTechBehindPercent)
		else:
			g_iTechBehindPercent = 300

		# Directly affects the final amount of beakers one get calculated from the two above factors.
		g_iFinalModifier = Config.get("Enhanced Tech Conquest", "Final Modifier")
		if g_iFinalModifier.isdigit():
			g_iFinalModifier = int(g_iFinalModifier)
		else:
			g_iFinalModifier = 100

	sprint  = "Enhanced Tech Conquest:\n"
	sprint += "\tTechnology Transfer Ignore Prereq = %s\n" %str(g_bCheckPrereq)
	sprint += "\tBase Technology Transfer Percent. = %d\n" %g_iBasePercent
	sprint += "\tPercentage Per City Population... = %d\n" %g_iPopPercent
	sprint += "\tPercentage Per Tech Behind... = %d\n" %g_iTechBehindPercent
	sprint += "\tFinal Modifier... = %d" %g_iFinalModifier
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
			iRemaining = iCost - iProgress
			if not iRemaining:
				continue
			# Append the technology to the possible technology list
			aList.append((iTech, iCost, iRemaining))

		if aList:

			city = argsList[2]
			fBasePercent += iTechsBehind * g_iTechBehindPercent / 100.0
			charBeaker = GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar()
			iPopulation = city.getPopulation() + 1
			if iPopPercent:
				fForce = iPopPercent * (1 + city.isCapital()) * iPopulation / (100.0 * CyPlayerO.getTotalPopulation() + iPopulation)

			iCount = 0
			iLen = len(aList)
			szTxt = ""
			while iLen:
				iTech, iCost, iRemaining = aList.pop(GAME.getSorenRandNum(iLen, "random"))
				iLen -= 1
				# Get the total number of technology points that will be transfered to the new city owner
				if iPopPercent:
					fPercent = fBasePercent + fBasePercent * fForce
					if fPercent <= 0: continue
				else:
					fPercent = fBasePercent
					if fPercent <= 0: return

				fBeakers = iCost * fPercent / (20 * (iCount + 5))

				if g_iFinalModifier > 0:
					fBeakers = fBeakers * (100 + g_iFinalModifier) / 100
				elif g_iFinalModifier < 0:
					fBeakers = fBeakers * 100 / (100 - g_iFinalModifier)

				iBeakers = int(fBeakers)

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
