## By StrategyOnly converted to BUG by Dancing Hoskuld
## Completely rewritten when we went fom jus slaves to captives

## Modified by Dancing Hoskuld
##   Now Captives not Slaves
##     Chance of capturing a Military Captive when you attach a unit depends n your and their civics (to be done)
##     Capturing a military init now gives a Captive (Military)
##     Raizing a city will give Captive (Civilians) instead

from CvPythonExtensions import *
import BugUtil

gc = CyGlobalContext()

giDomainLand = -1

def init():
	global giDomainLand
	giDomainLand = gc.getInfoTypeForString('DOMAIN_LAND')

def onCombatResult(argsList):
	CyUnitW, CyUnitL = argsList

	# Captives
	if CyUnitW.isMadeAttack() and not CyUnitL.isAnimal() and CyUnitL.getDomainType() == giDomainLand and CyUnitW.getDomainType() == giDomainLand:
		# Check that the losing unit is not an animal and the unit does not have a capture type defined in the XML
		if CyUnitL.getCaptureUnitType((gc.getPlayer(CyUnitL.getOwner())).getCivilizationType()) == -1:

			iCaptureProbability = CyUnitW.captureProbabilityTotal()
			iCaptureResistance = CyUnitL.captureResistanceTotal()
			iChance = iCaptureProbability - iCaptureResistance
			if iChance > 100:
				iChance = 100
			BugUtil.info("CaptureSlaves: Chance to capture a captive is %d (%d - %d)", iChance, iCaptureProbability, iCaptureResistance)

			iRandom = CyGame().getSorenRandNum(100, "Slave") # 0-99
			if iChance > iRandom:
				if CyUnitL.isHasUnitCombat(gc.getInfoTypeForString('UNITCOMBAT_SPECIES_NEANDERTHAL')):
					iUnit = gc.getInfoTypeForString('UNIT_CAPTIVE_NEANDERTHAL')
					sMessage = CyTranslator().getText("TXT_KEY_MESSAGE_NEANDERTHAL_CAPTIVE",())
				else:
					iUnit = gc.getInfoTypeForString('UNIT_CAPTIVE_MILITARY')
					sMessage = CyTranslator().getText("TXT_KEY_MESSAGE_MILITARY_CAPTIVE",())

				iPlayerW = CyUnitW.getOwner()
				CyPlayerW = gc.getPlayer(iPlayerW)
				X = CyUnitW.getX()
				Y = CyUnitW.getY()
				CyUnit = CyPlayerW.initUnit(iUnit, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
				if CyUnitW.isHiddenNationality():
					CyUnit.doHNCapture()
				if CyPlayerW.isHuman():
					CyInterface().addMessage(iPlayerW, False, 15, sMessage, '', 0, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), X, Y, True, True)


def onCityRazed(argsList):
	CyCity, iPlayer = argsList
	if not CyCity: return

	CyPlayer = gc.getPlayer(iPlayer)
	bHuman = CyPlayer.isHuman()

	sCityName = CyCity.getName()
	X = CyCity.getX()
	Y = CyCity.getY()

	'''
	# Convert Great Specialists into captives or other
	iCount = CyCity.getSpecialistCount(gc.getInfoTypeForString('SPECIALIST_GREAT_PRIEST'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MESSAGE_CITY_HAD_PRIESTS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage ,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(gc.getInfoTypeForString('SPECIALIST_GREAT_ARTIST'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MESSAGE_CITY_HAD_ARTISTS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(gc.getInfoTypeForString('SPECIALIST_GREAT_SCIENTIST'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MESSAGE_CITY_HAD_SCIENTISTS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(gc.getInfoTypeForString('SPECIALIST_GREAT_MERCHANT'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MESSAGE_CITY_HAD_MERCHANTS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(gc.getInfoTypeForString('SPECIALIST_GREAT_ENGINEER'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MESSAGE_CITY_HAD_ENGINEERS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(gc.getInfoTypeForString('SPECIALIST_GREAT_DOCTOR'))
	if iCount > 0:
		iCountKilled = iCount
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MESSAGE_CITY_HAD_DOCTORS",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(gc.getInfoTypeForString('SPECIALIST_GREAT_SPY'))
	if iCount > 0:
		iCountKilled = iCount
		Inhiding = 0
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MESSAGE_CITY_HAD_SPIES",(iCount,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	iCount = CyCity.getSpecialistCount(gc.getInfoTypeForString('SPECIALIST_GREAT_GENERAL')) + CyCity.getSpecialistCount(gc.getInfoTypeForString('SPECIALIST_GREAT_WARLORD'))
	if iCount > 0:
		iCountKilled = iCount
		iCountRebelled = 0
		iCountCaptured = 0
		sMessage = BugUtil.getText("TXT_KEY_MESSAGE_CITY_HAD_GENERALS",(iCount,iCountKilled,iCountRebelled,iCountCaptured))
		CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)
	'''

	## Slaves
	iSlaveSettled = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE")
	iSlaveFood = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD")
	iSlaveProd = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION")
	iSlaveCom = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE")
	iSlaveHealth = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH")
	iSlaveEntertain = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT")
	iSlaveTutor = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR")
	iSlaveMilitary = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY")

	iUnitCaptiveSlave = gc.getInfoTypeForString("UNIT_FREED_SLAVE")
	iUnitImmigrant = gc.getInfoTypeForString("UNIT_CAPTIVE_IMMIGRANT")
	iUnitEntertain = gc.getInfoTypeForString("UNIT_STORY_TELLER")
	iUnitMerCaravan = gc.getInfoTypeForString("UNIT_EARLY_MERCHANT_C2C")
	iUnitHealth = gc.getInfoTypeForString("UNIT_HEALER")

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
			sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_AS_FREED_SLAVES",(sCityName, iCount))
			CyInterface().addMessage(iPlayer, False, 15, sMessage, '', 0, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), X, Y, True, True)

	if iCountNewPop > 0:
		iCountImmigrants = iCountNewPop
		if iCountImmigrants > 0:
			for i in range (iCountImmigrants):
				newunit = CyPlayer.initUnit(iUnitImmigrant, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			if bHuman:
				sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_AS_IMMIGRANTS",(iCountImmigrants*3, sCityName, iCountImmigrants))
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
			sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_AS_CARAVANS",(sCityName, iCountProd, "Early Merchants"))
			CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	if iCountHealth > 0:
		for i in range (iCountProd):
			CyPlayer.initUnit(iUnitHealth, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			CyCity.changeFreeSpecialistCount(iSlaveHealth,-1)
		if bHuman:
			sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_AS_HEALERS",(sCityName, iCountHealth, "Healers"))
			CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	if iCountEntertain > 0:
		for i in range (iCountEntertain):
			CyPlayer.initUnit(iUnitEntertain, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			CyCity.changeFreeSpecialistCount(iSlaveEntertain,-1)
		if bHuman:
			sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_AS_STORY_TELLERS",(sCityName, iCountEntertain, "Story Tellers"))
			CyInterface().addMessage(iPlayer,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	## Convert population to captives
	iUnit = gc.getInfoTypeForString('UNIT_CAPTIVE_CIVILIAN')
	iCount = 0
	iPop = CyCity.getPopulation()
	if iPop == 1:
		if CyGame().getSorenRandNum(100, "Slave") < 66:
			CyPlayer.initUnit(iUnit, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			iCount = 1
	else:
		iCivilianCitizenUnits = (iPop + 1) / 2
		for loop in xrange(iCivilianCitizenUnits):
			CyPlayer.initUnit(iUnit, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			iCount += 1

	if bHuman and iCount:
		sMessage = BugUtil.getText("TXT_KEY_MESSAGE_CIVILIAN_CAPTIVE", iCount)
		CyInterface().addMessage(iPlayer, False, 15, sMessage, '', 0, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), X, Y, True, True)

'''
def onCityAcquiredAndKept(self, argsList):
	iOwner, CyCity = argsList
	# If there are slaves in the city but the new owner does not run slavery remove the slave buildings and free the slaves
	CyPlayer = gc.getPlayer(iOwner)
	if CyPlayer.countNumBuildings(gc.getInfoTypeForString("BUILDING_WV_SLAVERY")): return # Running slavery

	sCityName = CyCity.getName()
	X = CyCity.getX()
	Y = CyCity.getY()

	iSlaveSettled = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE")
	iSlaveFood = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_FOOD")
	iSlaveProd = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_PRODUCTION")
	iSlaveCom = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_COMMERCE")
	iSlaveHealth = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_HEALTH")
	iSlaveEntertain = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_ENTERTAINMENT")
	iSlaveTutor = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_TUTOR")
	iSlaveMilitary = gc.getInfoTypeForString("SPECIALIST_SETTLED_SLAVE_MILITARY")

	iUnitCaptiveSlave = gc.getInfoTypeForString("UNIT_FREED_SLAVE")
	iUnitImmigrant = gc.getInfoTypeForString("UNIT_CAPTIVE_IMMIGRANT")
	iUnitEntertain = gc.getInfoTypeForString("UNIT_STORY_TELLER")
	iUnitMerCaravan = gc.getInfoTypeForString("UNIT_EARLY_MERCHANT_C2C")
	iUnitHealth = gc.getInfoTypeForString("UNIT_HEALER")

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
	iCount -= 3*iCountNewPop

	if iCount > 0:
		for i in range (iCount):
			pPlayer.initUnit(iUnitCaptiveSlave, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
		sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_AS_FREED_SLAVES",(sCityName, iCount))
		CyInterface().addMessage(iOwner,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	if iCountNewPop > 0:
		iCountImmigrants = iCountNewPop
		iCityPop = city.getPopulation()
		if iCityPop < 7:
			## fill up the local pop and left overs become immigrants
			iMaxToAddPop = 7 - iCityPop
			if iMaxToAddPop > iCountImmigrants:
				CyCity.changePopulation(iCountImmigrants)
				sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_JOINED_CITY_POPULATION",(iCountImmigrants*3, sCityName, iCountImmigrants))
				CyInterface().addMessage(iOwner,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)
				iCountImmigrants = 0
			else:
				CyCity.changePopulation(iMaxToAddPop)
				sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_JOINED_CITY_POPULATION",(iMaxToAddPop*3, sCityName, iMaxToAddPop))
				CyInterface().addMessage(iOwner,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)
				iCountImmigrants = iCountImmigrants - iMaxToAddPop
		if iCountImmigrants > 0:
			for i in range (iCountImmigrants):
				pPlayer.initUnit(iUnitImmigrant, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_AS_IMMIGRANTS",(iCountImmigrants*3, sCityName, iCountImmigrants))
			CyInterface().addMessage(iOwner,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

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
			pPlayer.initUnit(iUnitMerCaravan, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			CyCity.changeFreeSpecialistCount(iSlaveProd,-1)
		sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_AS_CARAVANS",(sCityName, iCountProd, "Early Merchants"))
		CyInterface().addMessage(iOwner,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	if iCountHealth > 0:
		for i in range (iCountProd):
			pPlayer.initUnit(iUnitHealth, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			CyCity.changeFreeSpecialistCount(iSlaveHealth,-1)
		sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_AS_HEALERS",(sCityName, iCountHealth, "Healers"))
		CyInterface().addMessage(iOwner,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)

	if iCountEntertain > 0:
		for i in range (iCountEntertain):
			pPlayer.initUnit(iUnitEntertain, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			CyCity.changeFreeSpecialistCount(iSlaveEntertain,-1)
		sMessage = BugUtil.getText("TXT_MESSAGE_FREED_SLAVES_AS_STORY_TELLERS",(sCityName, iCountEntertain, "Story Tellers"))
		CyInterface().addMessage(iOwner,False,15, sMessage,'',0,'Art/Interface/Buttons/Civics/Serfdom.dds',ColorTypes(44), X, Y, True,True)
'''