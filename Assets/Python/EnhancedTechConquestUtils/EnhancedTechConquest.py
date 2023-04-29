from CvPythonExtensions import *
import CvUtil

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

def loadConfigurationData():
	global g_bCheckPrereq
	global g_iBasePercentOffset
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
		g_iBasePercentOffset = Config.get("Enhanced Tech Conquest", "Base Percent Offset")
		if g_iBasePercentOffset.isdigit():
			g_iBasePercentOffset = int(g_iBasePercentOffset)
		else:
			g_iBasePercentOffset = 0

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
	sprint += "\tBase Technology Transfer Percent. = %d\n" %g_iBasePercentOffset
	sprint += "\tPercentage Per City Population... = %d\n" %g_iPopPercent
	sprint += "\tPercentage Per Tech Behind... = %d\n" %g_iTechBehindPercent
	sprint += "\tFinal Modifier... = %d" %g_iFinalModifier
	print sprint

class EnhancedTechConquest:

	def onCityAcquired(self, argsList):
		#iOwnerOld, iOwnerNew, city, bConquest, bTrade, bAutoRaze = argsList
		# if not bConquest
		if not argsList[3]: return

		fBasePercent = g_iBasePercentOffset
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
		aList0 = []
		aList1 = []
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
			bImmediateTech = CyPlayerN.canResearch(iTech, True, True);
			if bCheckPrereq and not bImmediateTech:
				continue
			# Append the technology to the possible technology list
			iCost = CyTeamN.getResearchCost(iTech)
			iRemaining = iCost - CyTeamN.getResearchProgress(iTech)
			# Append the technology to the possible technology list
			if bImmediateTech:
				aList0.append((iTech, iCost, iRemaining))
			elif iRemaining > iCost *2/3:
				aList1.append((iTech, iCost, iRemaining / 3))
			elif iRemaining > iCost / 2:
				aList1.append((iTech, iCost, iRemaining / 2))
			elif iRemaining > iCost / 3:
				aList1.append((iTech, iCost, iRemaining*2/3))
			else:
				aList1.append((iTech, iCost, iRemaining - 1))

		if not aList0: return

		fBasePercent += iTechsBehind * g_iTechBehindPercent / 100.0
		if fBasePercent < 1: return

		city = argsList[2]
		charBeaker = GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar()
		if iPopPercent:
			iPopulation = city.getPopulation() + 1
			fForce = iPopPercent * (1 + city.isCapital()) * iPopulation / (100.0 * (CyPlayerO.getTotalPopulation() + iPopulation))

		col = ColorTypes(GC.getInfoTypeForString("COLOR_GREEN"))
		bHuman = CyPlayerN.isHuman()
		iCount = 0
		iLen0 = len(aList0)
		iLen1 = len(aList1)
		iTechs = iLen0 + iLen1
		if iTechs > 1:
			fBase = iTechs / 4.0
			fAttenuation = 100.0 / fBase

		aTxtList = []
		bGotTech = False

		while iLen0 or iLen1:

			if iLen0:
				iTech, iCost, iRemaining = aList0.pop(GAME.getSorenRandNum(iLen0, "random"))
				iLen0 -= 1
				bFirstList = True
			else:
				bFirstList = False
				iTech, iCost, iRemaining = aList1.pop(GAME.getSorenRandNum(iLen1, "random"))
				iLen1 -= 1
				if bGotTech and CyPlayerN.canResearch(iTech, True, True):
					iRemaining = iCost - CyTeamN.getResearchProgress(iTech)
				else: iCost = iRemaining

			# Get the total number of technology points that will be transfered to the new city owner
			if iPopPercent:
				fPercent = fBasePercent + fBasePercent * fForce
				if fPercent <= 0: continue
			else:
				fPercent = fBasePercent
				if fPercent <= 0: return

			if iTechs > 1:
				fBeakers = iCost * fPercent / (20*(iTechs-1) + fAttenuation * (iCount + fBase))
			else: fBeakers = iCost * fPercent / 100

			if g_iFinalModifier > 0:
				fBeakers = fBeakers * (100 + g_iFinalModifier) / 100
			elif g_iFinalModifier < 0:
				fBeakers = fBeakers * 100 / (100 - g_iFinalModifier)

			iBeakers = int(fBeakers)

			if iBeakers < 1: continue
			if iBeakers > iRemaining:
				iBeakers = iRemaining
				if bFirstList:
					bGotTech = True

			# Increase the research progress for the new city owner
			CyTeamN.changeResearchProgress(iTech, iBeakers, iOwnerNew)

			if bHuman:

				if not (iCount % 6):
					if iCount:
						aTxtList.append(szTxt)
					szTxt = "\n\t* " + GC.getTechInfo(iTech).getDescription() + u" <-> %i%c" %(iBeakers, charBeaker)
				else: szTxt += "\n\t* " + GC.getTechInfo(iTech).getDescription() + u" <-> %i%c" %(iBeakers, charBeaker)

			iCount += 1

		if bHuman: # Inform the player they didn't get any new technologies
			if iCount:
				if (iCount % 6):
					aTxtList.append(szTxt)

				bFirst = True
				while aTxtList:
					
					if bFirst:
						szTxt0 = aTxtList.pop(0)
						bFirst = False
					else: CvUtil.sendMessage(aTxtList.pop(), iOwnerNew, 20, "", col, city.getX(), city.getY(), True, True)

				szTxt = TRNSLTR.getText("TXT_KEY_ENHANCED_TECH_CONQUEST_SUCESS", ()) % city.getName() + szTxt0
			else: szTxt = TRNSLTR.getText("TXT_KEY_ENHANCED_TECH_CONQUEST_FAIL", ()) % city.getName()

			CvUtil.sendMessage(
				szTxt, iOwnerNew, 20,
				GC.getCivilizationInfo(CyPlayerO.getCivilizationType()).getButton(),
				col, city.getX(), city.getY(), True, True
			)
