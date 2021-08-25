## By StrategyOnly converted to BUG by Dancing Hoskuld
## Completely rewritten when we went fom jus slaves to captives

## Modified by Dancing Hoskuld
##   Now Captives not Slaves
##     Chance of capturing a Military Captive when you attach a unit depends n your and their civics (to be done)
##     Capturing a military init now gives a Captive (Military)
##     Raizing a city will give Captive (Civilians) instead

from CvPythonExtensions import *
import BugUtil
import CvUtil

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

giDomainLand = -1

def init():
	global giDomainLand
	giDomainLand = GC.getInfoTypeForString('DOMAIN_LAND')

def onCombatResult(argsList):
	CyUnitW, CyUnitL = argsList

	# Captives
	# Check that the losing unit is not an animal and the unit does not have a capture type defined in the XML
	if (CyUnitW.isMadeAttack() and not CyUnitL.isAnimal() and CyUnitL.getDomainType() == giDomainLand
	and CyUnitW.getDomainType() == giDomainLand and CyUnitL.getCaptureUnitType() == -1
	):
		iCaptureProbability = CyUnitW.captureProbabilityTotal()
		iCaptureResistance = CyUnitL.captureResistanceTotal()
		iChance = iCaptureProbability - iCaptureResistance
		BugUtil.info("CaptureSlaves: Chance to capture a captive is %d (%d - %d)", iChance, iCaptureProbability, iCaptureResistance)

		if iChance > GAME.getSorenRandNum(100, "Slave"):  # 0-99

			if CyUnitL.isHasUnitCombat(GC.getInfoTypeForString('UNITCOMBAT_SPECIES_NEANDERTHAL')):
				iUnit = GC.getInfoTypeForString('UNIT_CAPTIVE_NEANDERTHAL')
				sMessage = TRNSLTR.getText("TXT_KEY_MSG_NEANDERTHAL_CAPTIVE",())
			else:
				iUnit = GC.getInfoTypeForString('UNIT_CAPTIVE_MILITARY')
				sMessage = TRNSLTR.getText("TXT_KEY_MSG_MILITARY_CAPTIVE",())

			iPlayerW = CyUnitW.getOwner()
			X = CyUnitW.getX()
			Y = CyUnitW.getY()
			CyUnit = GC.getPlayer(iPlayerW).initUnit(iUnit, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			if CyUnitW.isHiddenNationality():
				CyUnit.doHNCapture()
			if iPlayerW == GAME.getActivePlayer():
				CvUtil.sendMessage(sMessage, iPlayerW, 8, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), X, Y, True, True)


def onCityRazed(argsList):
	CyCity, iPlayer = argsList
	if not CyCity: return

	CyPlayer = GC.getPlayer(iPlayer)
	bHuman = CyPlayer.isHuman()

	sCityName = CyCity.getName()
	X = CyCity.getX()
	Y = CyCity.getY()

	'''
	# Convert Great Specialists into captives or other
	iCount = CyCity.getSpecialistCount(GC.getInfoTypeForString('SPECIALIST_GREAT_PROPHET'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MSG_CITY_HAD_PRIESTS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage ,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(GC.getInfoTypeForString('SPECIALIST_GREAT_ARTIST'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MSG_CITY_HAD_ARTISTS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(GC.getInfoTypeForString('SPECIALIST_GREAT_SCIENTIST'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MSG_CITY_HAD_SCIENTISTS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(GC.getInfoTypeForString('SPECIALIST_GREAT_MERCHANT'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MSG_CITY_HAD_MERCHANTS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(GC.getInfoTypeForString('SPECIALIST_GREAT_ENGINEER'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MSG_CITY_HAD_ENGINEERS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(GC.getInfoTypeForString('SPECIALIST_GREAT_DOCTOR'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MSG_CITY_HAD_DOCTORS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(GC.getInfoTypeForString('SPECIALIST_GREAT_SPY'))
	if iCount > 0:
		iCountKilled = iCount
		Inhiding = 0
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MSG_CITY_HAD_SPIES",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(GC.getInfoTypeForString('SPECIALIST_GREAT_MILITARY_INSTRUCTOR')) + CyCity.getSpecialistCount(GC.getInfoTypeForString('SPECIALIST_MILITARY_INSTRUCTOR'))
	if iCount > 0:
		iCountKilled = iCount
		iCountRebelled = 0
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MSG_CITY_HAD_GENERALS",(iCount,iCountKilled,iCountRebelled,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)
	'''

	## Slaves
	iSlaveSettled = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE")
	iSlaveFood = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD")
	iSlaveProd = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION")
	iSlaveCom = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE")
	iSlaveHealth = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH")
	iSlaveEntertain = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT")
	iSlaveTutor = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR")
	iSlaveMilitary = GC.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY")

	iUnitCaptiveSlave = GC.getInfoTypeForString("UNIT_FREED_SLAVE")
	iUnitImmigrant = GC.getInfoTypeForString("UNIT_CAPTIVE_IMMIGRANT")
	iUnitEntertain = GC.getInfoTypeForString("UNIT_STORY_TELLER")
	iUnitMerCaravan = GC.getInfoTypeForString("UNIT_EARLY_MERCHANT_C2C")
	iUnitHealth = GC.getInfoTypeForString("UNIT_HEALER")

	iCountSettled = CyCity.getFreeSpecialistCount(iSlaveSettled)
	iCountFood = CyCity.getFreeSpecialistCount(iSlaveFood)
	iCountProd = CyCity.getFreeSpecialistCount(iSlaveProd)
	iCountCom = CyCity.getFreeSpecialistCount(iSlaveCom)
	iCountHealth = CyCity.getFreeSpecialistCount(iSlaveHealth)
	iCountEntertain = CyCity.getFreeSpecialistCount(iSlaveEntertain)
	iCountTutor = CyCity.getFreeSpecialistCount(iSlaveTutor)
	iCountMilitary = CyCity.getFreeSpecialistCount(iSlaveMilitary)

	## Process those that can become population or immagrants
	##	where 3 slaves = 1 pop or immigrant
	##	and can only increase the city pop to 7
	iCount = iCountSettled + iCountFood + iCountCom + iCountTutor + iCountMilitary
	iCountNewPop = int(iCount/3)
	iCount = iCount - 3*iCountNewPop

	if iCount > 0:
		for i in xrange(iCount):
			newunit = CyPlayer.initUnit(iUnitCaptiveSlave, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
		if bHuman:
			sMessage = BugUtil.getText("TXT_KEY_MSG_FREED_SLAVES_AS", (sCityName, GC.getUnitInfo(iUnitCaptiveSlave).getDescription(), iCount))
			CyInterface().addMessage(iPlayer, False, 15, sMessage, '', 0, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), X, Y, True, True)

	if iCountNewPop > 0:
		iCountImmigrants = iCountNewPop
		if iCountImmigrants > 0:
			for i in range (iCountImmigrants):
				newunit = CyPlayer.initUnit(iUnitImmigrant, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			if bHuman:
				sMessage = BugUtil.getText("TXT_KEY_MSG_FREED_SLAVES_AS_IMMIGRANTS", (iCountImmigrants*3, sCityName, iCountImmigrants))
				CyInterface().addMessage(iPlayer, False, 15, sMessage, '', 0, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), X, Y, True, True)

	## Now remove those slaves
	if iCountSettled > 0:
		CyCity.changeFreeSpecialistCount(iSlaveSettled,-iCountSettled)
	if iCountFood > 0:
		CyCity.changeFreeSpecialistCount(iSlaveFood,-iCountFood)
	if iCountCom > 0:
		CyCity.changeFreeSpecialistCount(iSlaveCom,-iCountCom)
	if iCountTutor > 0:
		CyCity.changeFreeSpecialistCount(iSlaveTutor,-iCountTutor)
	if iCountMilitary > 0:
		CyCity.changeFreeSpecialistCount(iSlaveMilitary,-iCountMilitary)

	## Now convert the other slaves
	if iCountProd > 0:
		for i in range (iCountProd):
			CyPlayer.initUnit(iUnitMerCaravan, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			CyCity.changeFreeSpecialistCount(iSlaveProd,-1)
		if bHuman:
			sMessage = BugUtil.getText("TXT_KEY_MSG_FREED_SLAVES_AS",(sCityName, GC.getUnitInfo(iUnitMerCaravan).getDescription(), iCountProd))
			CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	if iCountHealth > 0:
		for i in range (iCountProd):
			CyPlayer.initUnit(iUnitHealth, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			CyCity.changeFreeSpecialistCount(iSlaveHealth,-1)
		if bHuman:
			sMessage = BugUtil.getText("TXT_KEY_MSG_FREED_SLAVES_AS",(sCityName, GC.getUnitInfo(iUnitHealth).getDescription(), iCountHealth))
			CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	if iCountEntertain > 0:
		for i in range (iCountEntertain):
			CyPlayer.initUnit(iUnitEntertain, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			CyCity.changeFreeSpecialistCount(iSlaveEntertain,-1)
		if bHuman:
			sMessage = BugUtil.getText("TXT_KEY_MSG_FREED_SLAVES_AS",(sCityName, GC.getUnitInfo(iUnitEntertain).getDescription(), iCountEntertain))
			CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	## Convert population to captives
	iUnit = GC.getInfoTypeForString('UNIT_CAPTIVE_CIVILIAN')
	iCount = 0
	iPop = CyCity.getPopulation()
	if iPop == 1:
		if GAME.getSorenRandNum(100, "Slave") < 66:
			CyPlayer.initUnit(iUnit, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			iCount = 1
	else:
		iCivilianCitizenUnits = (iPop + 1) / 2
		for loop in xrange(iCivilianCitizenUnits):
			CyPlayer.initUnit(iUnit, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			iCount += 1

	if bHuman and iCount:
		sMessage = BugUtil.getText("TXT_KEY_MSG_CIVILIAN_CAPTIVE", iCount)
		CyInterface().addMessage(iPlayer, False, 15, sMessage, '', 0, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), X, Y, True, True)
