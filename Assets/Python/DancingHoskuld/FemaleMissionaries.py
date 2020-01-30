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
	unit = argsList[1]
	CyPlayer = GC.getPlayer(CyCity.getOwner())

# Female Missionaries - saibotlieh

	iUnitType = unit.getUnitType()
	UnitInfo = GC.getUnitInfo(iUnitType)
	if UnitInfo.getDefaultUnitAIType() == GC.getInfoTypeForString('UNITAI_MISSIONARY'):
		sUnitType = UnitInfo.getType()
		sFemaleUnitType = 'UNIT_FEMALE'+sUnitType[4:]
		iFemaleUnitType = GC.getInfoTypeForString(sFemaleUnitType)

		if iFemaleUnitType > -1:
			if CyPlayer.isCivic(GC.getInfoTypeForString('CIVIC_LIBERAL')):
				iFemaleChance = 50
			else:
				iFemaleChance = 15

			iRnd = CyGame().getSorenRandNum(100, "Female unit")
			if iRnd < iFemaleChance:
				oldunit = unit
				pFemaleUnit = CyPlayer.initUnit(iFemaleUnitType,oldunit.getX(),oldunit.getY(),UnitAITypes.NO_UNITAI,DirectionTypes.DIRECTION_SOUTH)
				pFemaleUnit.convert(oldunit)
				if oldunit.getGroup().isAutomated():
					pFemaleUnit.getGroup().setAutomateType(AutomateTypes.AUTOMATE_RELIGION)
				## If city has a rally point send the female missionary there
				else:
					rallyPlot = CyCity.getRallyPlot()
					# if not (rallyPlot == None):
					if rallyPlot.getX() > -1 and rallyPlot.getY() > -1:
						pFemaleUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, rallyPlot.getX(), rallyPlot.getY(), 0, False, True, MissionAITypes.NO_MISSIONAI, pFemaleUnit.plot(), pFemaleUnit)
				oldunit.kill(False, oldunit.getOwner())

				## Fa Men Si
				FA_MEN_SI = GC.getInfoTypeForString("BUILDING_FA_MEN_SI")
				if FA_MEN_SI > -1 and CyPlayer.countNumBuildings(FA_MEN_SI):
					pFemaleUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_FA_MEN_SI_INSPIRED"),True)

# Beastmaster - saibotlieh

	sUnitType = UnitInfo.getType()

	if sUnitType[:10] == 'UNIT_TAMED':	# A Beastmaster will be added to all units which first 10 letters of the UnitType are UNIT_TAMED
		iRnd = CyGame().getSorenRandNum(100, "Female Beastmaster")
		if iRnd < 15:
			unit.setLeaderUnitType(GC.getInfoTypeForString("UNIT_FEMALE_BEASTMASTER"))
		else:
			unit.setLeaderUnitType(GC.getInfoTypeForString("UNIT_BEASTMASTER"))
