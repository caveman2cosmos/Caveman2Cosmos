## Sid Meier's Civilization 4
## Copyright Firaxis Games 2006
##
## CvEventManager
## This class is passed an argsList from CvAppInterface.onEvent
## The argsList can contain anything from mouse location to key info
## The EVENTLIST that are being notified can be found

from CvPythonExtensions import *
import NaturalWonders

def onPlotRevealed(argsList):
	pPlot = argsList[0]
	iTeam = argsList[1]

	NaturalWonders.NaturalWonders().checkReveal(pPlot, iTeam)
