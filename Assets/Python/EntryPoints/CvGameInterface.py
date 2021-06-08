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

def createBarbarianUnits():
	return GameUtils.createBarbarianUnits()

def skipResearchPopup(argsList):
	return GameUtils.skipResearchPopup(argsList)

def showTechChooserButton(argsList):
	return GameUtils.showTechChooserButton(argsList)

def getFirstRecommendedTech(argsList):
	return GameUtils.getFirstRecommendedTech(argsList)

def getSecondRecommendedTech(argsList):
	return GameUtils.getSecondRecommendedTech(argsList)

def skipProductionPopup(argsList):
	return GameUtils.skipProductionPopup(argsList)

def canRazeCity(argsList):
	return GameUtils.canRazeCity(argsList)

def canDeclareWar(argsList):
	return GameUtils.canDeclareWar(argsList)

def showExamineCityButton(argsList):
	return GameUtils.showExamineCityButton(argsList)

def getRecommendedUnit(argsList):
	return GameUtils.getRecommendedUnit(argsList)

def getRecommendedBuilding(argsList):
	return GameUtils.getRecommendedBuilding(argsList)

def updateColoredPlots():
	return GameUtils.updateColoredPlots()

def unitCannotMoveInto(argsList):
	return GameUtils.unitCannotMoveInto(argsList)

def canBuild(argsList):
	return GameUtils.canBuild(argsList)

def cannotFoundCity(argsList):
	return GameUtils.cannotFoundCity(argsList)

def cannotSelectionListMove(argsList):
	return GameUtils.cannotSelectionListMove(argsList)

def cannotSelectionListGameNetMessage(argsList):
	return GameUtils.cannotSelectionListGameNetMessage(argsList)

def cannotDoControl(argsList):
	return GameUtils.cannotDoControl(argsList)

def canDoCivic(argsList):
	return GameUtils.canDoCivic(argsList)

def cannotDoCivic(argsList):
	return GameUtils.cannotDoCivic(argsList)

def canTrain(argsList):
	return GameUtils.canTrain(argsList)

def cannotTrain(argsList):
	return GameUtils.cannotTrain(argsList)

def canConstruct(argsList):
	return GameUtils.canConstruct(argsList)

def cannotConstruct(argsList):
	return GameUtils.cannotConstruct(argsList)

def cannotMaintain(argsList):
	return GameUtils.cannotMaintain(argsList)

def AI_unitUpdate(argsList):
	'AI moves units - return 0 to let AI handle it, return 1 to say that it was handled in python'
	return GameUtils.AI_unitUpdate(argsList)

def calculateScore(argsList):
	return GameUtils.calculateScore(argsList)

def doHolyCity():
	return GameUtils.doHolyCity()

def doGoody(argsList):
	return GameUtils.doGoody(argsList)

def doReligion(argsList):
	return GameUtils.doReligion(argsList)

def doMeltdown(argsList):
	return GameUtils.doMeltdown(argsList)

def doReviveActivePlayer(argsList):
	return GameUtils.doReviveActivePlayer(argsList)

def doPillageGold(argsList):
	return GameUtils.doPillageGold(argsList)

def doCityCaptureGold(argsList):
	return GameUtils.doCityCaptureGold(argsList)

def canFoundCitiesOnWater(argsList):
	return GameUtils.canFoundCitiesOnWater(argsList)

def doCombat(argsList):
	return GameUtils.doCombat(argsList)

def getConscriptUnitType(argsList):
	return GameUtils.getConscriptUnitType(argsList)

def canPickPlot(argsList):
	return GameUtils.canPickPlot(argsList)

def canUpgradeAnywhere(argsList):
	return GameUtils.canUpgradeAnywhere(argsList)

def getWidgetHelp(argsList):
	return GameUtils.getWidgetHelp(argsList)

def getUpgradePriceOverride(argsList):
	return GameUtils.getUpgradePriceOverride(argsList)
