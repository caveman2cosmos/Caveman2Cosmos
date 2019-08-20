##StartingPointsUtil
###MODDER READ THIS:
###You do not have to change anything in this file
###all changes have to be done in the CvEventManager.
###This file just has to be in the same folder like the CvEventManager.py.

from CvPythonExtensions import *
import sys
import CvUtil
import BUGPath
import BugUtil

gc = CyGlobalContext()

###generic file reading function

def getCoastalType(argsList):
	pPlot = argsList
	iLatitude = pPlot.getLatitude()

	if iLatitude < 23.5:
		return gc.getInfoTypeForString("TERRAIN_COAST_TROPICAL")
	elif iLatitude > 66.5:
		return gc.getInfoTypeForString("TERRAIN_COAST_POLAR")
	else:
		return gc.getInfoTypeForString("TERRAIN_COAST")

def getOceanType(argsList):
	pPlot = argsList
	iLatitude = pPlot.getLatitude()

	if iLatitude < 23.5:
		return gc.getInfoTypeForString("TERRAIN_OCEAN_TROPICAL")
	elif iLatitude > 66.5:
		return gc.getInfoTypeForString("TERRAIN_OCEAN_POLAR")
	else:
		return gc.getInfoTypeForString("TERRAIN_OCEAN")
