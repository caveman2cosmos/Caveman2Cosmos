
## Dancing Hoskuld utilities

from CvPythonExtensions import *
import BugUtil

# globals
gc = CyGlobalContext()
localText = CyTranslator()

def getNearestCity(iPlayer, pPlot, gi_HalfMaxX):

	pPlayer = gc.getPlayer(iPlayer)
	iX = pPlot.getX()
	iY = pPlot.getY()

	iDistance = 2 * gi_HalfMaxX
	iClosestCity = -1
	for iCity in range(pPlayer.getNumCities()): ## for each player city
		piCity = pPlayer.getCity(iCity)
		iCityPlotX = piCity.getX()
		iCityPlotY = piCity.getY()

		xDistance = abs(iX - iCityPlotX)  # cylindar
		if xDistance > gi_HalfMaxX:
			xDistance = xDistance - gi_HalfMaxX
		iDistanceToThisCity = xDistance + abs(iY - iCityPlotY) # No need to divide by 2
		if (iDistanceToThisCity < iDistance):
			iDistance = iDistanceToThisCity
			iClosestCity = iCity

	if iClosestCity == -1:
		BugUtil.debug("DH_Utils - No city found for player %d.", iPlayer)
	else:
		BugUtil.debug("DH_Utils - closest city is %d.", iClosestCity)

	return iClosestCity

## Callback functions ##
#~ def getWidgetHelp(argsList):
	#~ eWidgetType, iData1, iData2, bOption = argsList

	#~ if iData1 == 690:
		#~ return CyTranslator().getText("TXT_KEY_BUTCHER_ANIMAL_HELP", ())

	#~ return u""

def AI_unitUpdate(argsList):
	pUnit = argsList[0]
	pUnitType = pUnit.getUnitType()

	if pUnitType in gai_DefeatedToCaptured:
		iUnitOwner = pUnit.getOwner()
		AIpPlayer = gc.getPlayer(iUnitOwner)
		if AIpPlayer.isAlive() and not AIpPlayer.isHuman() and not AIpPlayer.isNPC():

			iX = pUnit.getX( )
			iY = pUnit.getY( )
			pPlot = CyMap( ).plot(iX, iY)
			iPlotOwner = pPlot.getOwner ()

			if iPlotOwner == iUnitOwner:
				if pPlot.isCity():
					# in an owner city find out what buildings the unit can construct here
#~ BOOL canConstruct (CyPlot pPlot, BuildingType eBuilding)
#~ bool (CyPlot* pPlot, int (BuildingTypes) eBuilding)
					#~ MISSION_CONSTRUCT
					#~ return True
					return False

				# in own teritory therefore go to nearest city (or capital?)
				intCity = getNearestCity(iUnitOwner, pPlot)
				if intCity != -1:
					MoveToCity = AIpPlayer.getCity(intCity)
					pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, MoveToCity.getX(), MoveToCity.getY(), 0, False, True, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
					return True

				return False # just in case there is no city!

			if pUnit.isHurt() and pUnit.canHeal(pPlot):
				# heal
				pUnit.getGroup().pushMission(MissionTypes.MISSION_HEAL, -1, -1, 0, False, True, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
				return True

			# otherwise move to nearest city
			intCity = getNearestCity(iUnitOwner, pPlot)
			if intCity != -1:
				MoveToCity = AIpPlayer.getCity(intCity)
				pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, MoveToCity.getX(), MoveToCity.getY(), 0, False, True, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
				return True

			return False


	return False
