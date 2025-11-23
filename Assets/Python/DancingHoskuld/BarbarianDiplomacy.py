## Code for Barbarian Diplomacy
##

from CvPythonExtensions import *
import BugUtil

# globals
gc = CyGlobalContext()

gb_NoGoodyHuts = True
giIndigenousPeopleImprovement = -1
gaIPattitude2Player = None

def init():
	BugUtil.debug("Barbarian Diplomacy INIT.")

	global gb_NoGoodyHuts
	gb_NoGoodyHuts = CyGame().isOption(GameOptionTypes.GAMEOPTION_MAP_NO_GOODY_HUTS)

	global giIndigenousPeopleImprovement
	giIndigenousPeopleImprovement = gc.getInfoTypeForString("IMPROVEMENT_INDIGENOUS_COMMUNITY")

	global gaIPattitude2Player
	gaIPattitude2Player = []

'''
def onGameStart():
	return

def onGameLoad():
	return

def onNewPlayer():
	return
'''

def onGoodyReceived(argsList):
	pPlot = argsList[1]

	# just in case
	if gb_NoGoodyHuts or pPlot.isWater(): return

	BugUtil.debug("Barbarian Diplomacy - onGoodyReceived.")
	## Place Indigenous peoples improvement.
	pPlot.setImprovementType(giIndigenousPeopleImprovement)


def onImprovementDestroyed(argsList):
	iImprovement = argsList[0]

	if (not iImprovement == giIndigenousPeopleImprovement):
		return

	BugUtil.debug("Barbarian Diplomacy - IP destroyed.")
