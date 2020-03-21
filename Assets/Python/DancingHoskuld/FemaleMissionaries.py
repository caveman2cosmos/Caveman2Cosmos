## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## Femaile Missionaries by saibotlieh.
##	Converted to BUG format by by Dancing Hoskuld
##

from CvPythonExtensions import *

# globals
GC = CyGlobalContext()

###################################################
#~ def onUnitCreated(argsList):
	#~ 'Unit Completed'
	#~ unit = argsList[0]

#~ # Beastmaster - saibotlieh - start

	#~ iUnitType = unit.getUnitType()
	#~ UnitInfo = GC.getUnitInfo(iUnitType)
	#~ sUnitType = UnitInfo.getType()

	#~ if sUnitType[:10] == 'UNIT_TAMED':	# A Beastmaster will be added to all units which first 10 letters of the UnitType are UNIT_TAMED
		#~ iRnd = CyGame().getSorenRandNum(100, "Female Beastmaster")
		#~ if iRnd < 15:
			#~ unit.setLeaderUnitType(GC.getInfoTypeForString("UNIT_FEMALE_BEASTMASTER"))
		#~ else:
			#~ unit.setLeaderUnitType(GC.getInfoTypeForString("UNIT_BEASTMASTER"))

#~ # Beastmaster - saibotlieh - end

def onUnitBuilt(argsList):
	CyCity = argsList[0]
	CyUnit = argsList[1]

	iUnitType = CyUnit.getUnitType()
	UnitInfo = GC.getUnitInfo(iUnitType)
	sUnitType = UnitInfo.getType()

	# Female Missionaries - saibotlieh
	if UnitInfo.getDefaultUnitAIType() == GC.getInfoTypeForString('UNITAI_MISSIONARY'):
		sFemaleUnitType = 'UNIT_FEMALE'+sUnitType[4:]
		iFemaleUnitType = GC.getInfoTypeForString(sFemaleUnitType)

		if iFemaleUnitType > -1:
			CyPlayer = GC.getPlayer(CyCity.getOwner())
			if CyPlayer.isCivic(GC.getInfoTypeForString('CIVIC_EGALITARIAN')):
				iFemaleChance = 50
			else:
				iFemaleChance = 15

			iRnd = CyGame().getSorenRandNum(100, "Female unit")
			if iRnd < iFemaleChance:
				oldunit = CyUnit
				CyUnit = CyPlayer.initUnit(iFemaleUnitType,oldunit.getX(),oldunit.getY(),UnitAITypes.NO_UNITAI,DirectionTypes.DIRECTION_SOUTH)
				CyUnit.convert(oldunit)
				if oldunit.getGroup().isAutomated():
					CyUnit.getGroup().setAutomateType(AutomateTypes.AUTOMATE_RELIGION)

				else: # If city has a rally point send the female missionary there
					rallyPlot = CyCity.getRallyPlot()
					if rallyPlot.getX() > -1 and rallyPlot.getY() > -1:
						CyUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, rallyPlot.getX(), rallyPlot.getY(), 0, False, True, MissionAITypes.NO_MISSIONAI, CyUnit.plot(), CyUnit)
				oldunit.kill(False, oldunit.getOwner())

				## Fa Men Si
				FA_MEN_SI = GC.getInfoTypeForString("BUILDING_FA_MEN_SI")
				if FA_MEN_SI > -1 and CyPlayer.countNumBuildings(FA_MEN_SI):
					CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_FA_MEN_SI_INSPIRED"),True)
				return


	# Beastmaster - saibotlieh
	if sUnitType[:10] == 'UNIT_TAMED':	# A Beastmaster will be added to all units which first 10 letters of the UnitType are UNIT_TAMED
		if GC.getPlayer(CyCity.getOwner()).isCivic(GC.getInfoTypeForString('CIVIC_EGALITARIAN')):
			iOdds = 50
		else:
			iOdds = 15
		iRnd = CyGame().getSorenRandNum(100, "Female Beastmaster")
		if iRnd < iOdds:
			CyUnit.setLeaderUnitType(GC.getInfoTypeForString("UNIT_FEMALE_BEASTMASTER"))
		else:
			CyUnit.setLeaderUnitType(GC.getInfoTypeForString("UNIT_BEASTMASTER"))
