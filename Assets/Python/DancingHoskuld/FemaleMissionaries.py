## Femaile Missionaries and beastmaster by saibotlieh.

from CvPythonExtensions import *

GC = CyGlobalContext()

def onUnitBuilt(argsList):
	CyCity = argsList[0]
	CyUnit = argsList[1]

	UnitInfo = GC.getUnitInfo(CyUnit.getUnitType())
	sUnitType = UnitInfo.getType()

	# Female Missionaries
	if UnitInfo.getDefaultUnitAIType() == GC.getInfoTypeForString('UNITAI_MISSIONARY'):
		sFemaleUnitType = 'UNIT_FEMALE'+sUnitType[4:]
		iFemaleUnitType = GC.getInfoTypeForStringWithHiddenAssert(sFemaleUnitType)

		if iFemaleUnitType > -1:
			CyPlayer = GC.getPlayer(CyCity.getOwner())
			if CyPlayer.isCivic(GC.getInfoTypeForString('CIVIC_EGALITARIAN')):
				iFemaleChance = 50
			else:
				iFemaleChance = 15

			if CyGame().getSorenRandNum(100, "Female unit") < iFemaleChance:
				oldUnit = CyUnit
				CyUnit = CyPlayer.initUnit(iFemaleUnitType, CyUnit.getX(), CyUnit.getY(), UnitAITypes.NO_UNITAI,DirectionTypes.DIRECTION_SOUTH)
				CyUnit.convert(oldUnit, True) # This kills oldUnit with a delay
				if oldUnit.getGroup().isAutomated():
					CyUnit.getGroup().setAutomateType(AutomateTypes.AUTOMATE_RELIGION)

				else: # If city has a rally point send the female missionary there
					rallyPlot = CyCity.getRallyPlot()
					if rallyPlot.getX() > -1 and rallyPlot.getY() > -1:
						CyUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, rallyPlot.getX(), rallyPlot.getY(), 0, False, True, MissionAITypes.NO_MISSIONAI, CyUnit.plot(), CyUnit)

				# Fa Men Si
				if CyPlayer.hasBuilding(GC.getInfoTypeForString("BUILDING_FA_MEN_SI")):
					CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_FA_MEN_SI_INSPIRED"), True)
				return

	# Beastmaster
	if sUnitType[:10] == 'UNIT_TAMED':
		if GC.getPlayer(CyCity.getOwner()).isCivic(GC.getInfoTypeForString('CIVIC_EGALITARIAN')):
			iOdds = 50
		else:
			iOdds = 15

		if CyGame().getSorenRandNum(100, "Female Beastmaster") < iOdds:
			CyUnit.setLeaderUnitType(GC.getInfoTypeForString("UNIT_FEMALE_BEASTMASTER"))
		else:
			CyUnit.setLeaderUnitType(GC.getInfoTypeForString("UNIT_BEASTMASTER"))
