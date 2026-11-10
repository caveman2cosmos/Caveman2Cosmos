## Sid Meier's Civilization 4
## Copyright Firaxis Games 2006
##
## CvEventManager
## This class is passed an argsList from CvAppInterface.onEvent
## The argsList can contain anything from mouse location to key info
## The EVENTLIST that are being notified can be found

from CvPythonExtensions import *
import NaturalWonders

g_NaturalWonders = NaturalWonders.NaturalWonders()

def onPlotRevealed(argsList):
    pPlot = argsList[0]
    iTeam = argsList[1]
    ## Wrap in try/except so any bug inside checkReveal ends up in PythonErr.log
    ## with a real traceback instead of the generic "Error in plotRevealed event
    ## handler" message that tells us nothing. Also prevents error spam on-screen.
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