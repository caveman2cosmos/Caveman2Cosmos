## Sid Meier's Civilization 4
## Copyright Firaxis Games 2006

from CvPythonExtensions import *
import NaturalWonders

GC = CyGlobalContext()
g_NaturalWonders = NaturalWonders.NaturalWonders()

def onPlotRevealed(argsList):
	pPlot = argsList[0]
	iTeam = argsList[1]
	## Keep natural-wonder discovery in its own module. Primitive Maps has a
	## separate plotRevealed handler registered immediately after this module.
	try:
		g_NaturalWonders.checkReveal(pPlot, iTeam)
	except Exception, e:
		import traceback
		print "NaturalWonders.checkReveal failed:", str(e)
		traceback.print_exc()

'''
def onCityBuilt(argsList):
    city = argsList[0]

    NaturalWonders.NaturalWonders().placeWonderBuilding(city)

def onCityRazed(argsList):
    city, iPlayer = argsList
    iOwner = city.findHighestCulture()

    NaturalWonders.NaturalWonders().findNewCity(city)
'''
