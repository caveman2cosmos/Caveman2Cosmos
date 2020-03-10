# # # #
# Orion's Immigration Mod
#
from CvPythonExtensions import *
import CvUtil

# globals
GC = CyGlobalContext()

###############################
# Begin Immigration Functions #
###############################

def doImmigrantPlacementAI(CyUnit, CyCity, CyPlayer=None, iPlayer=None, bJoin=False):
	iX = CyCity.getX()
	iY = CyCity.getY()
	if CyUnit.getX() != iX or CyUnit.getY() != iY:
		CyUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, iX, iY, 0, False, True, MissionAITypes.NO_MISSIONAI, CyUnit.plot(), CyUnit)
	elif bJoin:
		CyCity.setPopulation(CyCity.getPopulation() + 1)
		if CyPlayer.isHuman():
			szTxt = CyTranslator().getText("TXT_KEY_MSG_IMMIGRATION",(CyCity.getName(),))
			CvUtil.sendMessage(szTxt, iPlayer, 25, CyUnit.getButton(), ColorTypes(8), iX, iY, True, True, szSound="AS2D_WELOVEKING")

		CyUnit.kill(0, -1)


def getLeastPopulatedCity(CyPlayer):
	city = None
	iMax = 0

	CyCity, i = CyPlayer.firstCity(False)
	while CyCity:
		iPop = CyCity.getPopulation()
		if not iMax or iMax > iPop:
			iMax = iPop
			city = CyCity
		CyCity, i = CyPlayer.nextCity(i, False)

	return city

'''
def cityCanTrainImmigrant(pCity):
	iOwner = pCity.getOwner()
	pPlayer = GC.getPlayer(iOwner)
	ImmigrantTech = GC.getInfoTypeForString("TECH_CONSTRUCTION")

	# Does the Player have the required Tech
	if GC.getTeam(pPlayer.getTeam()).isHasTech(ImmigrantTech):
		# Does the city have a Population of at least 10
		if pCity.getPopulation() > 9:
			#CyInterface().addImmediateMessage("Can Train Immigrant", "")
			return True

	return False


def showJoinCityButton(pUnit):
	iOwner = pUnit.getOwner()
	iUnitType = pUnit.getUnitType()
	ImmigrantType = GC.getInfoTypeForString("UNIT_IMMIGRANT")
	zShowButton = False

	if iUnitType == ImmigrantType:
		pCity = GC.getMap().plot(pUnit.getX(), pUnit.getY()).getPlotCity()
		iCityOwner = pCity.getOwner()
		pCityPlayer = GC.getPlayer(iCityOwner)
		pCityPopulation = pCity.getPopulation()

		if pCityPopulation > 0 and pCityPopulation < 7:
			if (iCityOwner == iOwner) or (GC.getTeam(pCityPlayer.getTeam()).isVassal(GC.getPlayer(iOwner).getTeam())):
				zShowButton = True

	return zShowButton


def getImmigrantProductionChance(pPlayer):
	# Accelerates Immigrant Production for Expansive traits
	iTrait = GC.getInfoTypeForString("TRAIT_EXPANSIVE")

	if pPlayer.hasTrait(iTrait):
		# Expansive Players get a 33% chance for Producing an Immigrant
		return 33
	else:
		# Non Spiritual Players get a 25% chance for Producing an Immigrant
		return 25


def doRandomChance(Number):
	# Produces a number between 0 and 100
	chance = CyGame().getSorenRandNum(100,"")

	if chance <= Number:
		return True
	else:
		return False
'''