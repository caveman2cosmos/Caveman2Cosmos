## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## #####   WARNING - MODIFYING THE FUNCTION NAMES OF THIS FILE IS PROHIBITED  #####
##
## The app specifically calls the functions as they are named.
## Use this file to pass args to another file that contains your modifications.

from CvPythonExtensions import *

import BugGameUtils

GameUtils = BugGameUtils.getDispatcher()

def isVictoryTest():
	return GameUtils.isVictoryTest()

def updateColoredPlots():
	return GameUtils.updateColoredPlots()

def canBuild(argsList):
	return GameUtils.canBuild(argsList)

def cannotMaintain(argsList):
	return GameUtils.cannotMaintain(argsList)

def AI_unitUpdate(argsList):
	return GameUtils.AI_unitUpdate(argsList)

def calculateScore(argsList):
	return GameUtils.calculateScore(argsList)

def doPillageGold(argsList):
	return GameUtils.doPillageGold(argsList)

def doCityCaptureGold(argsList):
	return GameUtils.doCityCaptureGold(argsList)

def canPickPlot(argsList):
	return GameUtils.canPickPlot(argsList)

# Called by exe
def getWidgetHelp(argsList):
	return GameUtils.getWidgetHelp(argsList)
