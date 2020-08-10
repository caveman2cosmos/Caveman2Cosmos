from CvPythonExtensions import *

gc = CyGlobalContext()

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
