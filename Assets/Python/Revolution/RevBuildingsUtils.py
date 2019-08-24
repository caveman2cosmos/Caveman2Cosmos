# Traits functions for Revolution Mod
#
# by jdog5000
# Version 1.5

from CvPythonExtensions import *

# globals
GC = CyGlobalContext()

########################## Traits effect helper functions #####################

def getBuildingsRevIdxLocal(CyCity):

	localRevIdx = 0
	posList = list()
	negList = list()

	for iBuilding in range(GC.getNumBuildingInfos()):
		if CyCity.getNumRealBuilding(iBuilding) > 0:
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			buildingEffect = CvBuildingInfo.getRevIdxLocal()
			if buildingEffect > 0:
				negList.append((buildingEffect, CvBuildingInfo.getDescription()))
			elif buildingEffect < 0:
				posList.append((buildingEffect, CvBuildingInfo.getDescription()))

			localRevIdx += buildingEffect

	return [localRevIdx, posList, negList]


def getBuildingsCivStabilityIndex(iPlayer):

	CyPlayer = GC.getPlayer(iPlayer)

	civStabilityIdx = 0
	posList = list()
	negList = list()

	if CyPlayer:
		for iBuilding in range(0,GC.getNumBuildingInfos()):
			CvBuildingInfo = GC.getBuildingInfo(iBuilding)
			buildingEffect = -CvBuildingInfo.getRevIdxNational()

			if buildingEffect:
				numBuildings = CyPlayer.countNumBuildings(iBuilding)
				if numBuildings:
					buildingEffect *= numBuildings
					if buildingEffect > 0:
						posList.append((buildingEffect, CvBuildingInfo.getDescription()))
					elif buildingEffect < 0:
						negList.append((buildingEffect, CvBuildingInfo.getDescription()))
					civStabilityIdx += buildingEffect

	return [civStabilityIdx, posList, negList]


def getBuildingsDistanceMod(CyCity):

	distModifier = 0
	posList = list()
	negList = list()

	for iBuilding in range(GC.getNumBuildingInfos()):
		iDistanceModifier = GC.getBuildingInfo(iBuilding).getRevIdxDistanceModifier()
		if iDistanceModifier:
			if CyCity.getNumRealBuilding(iBuilding) > 0:
				distModifier += iDistanceModifier

	return distModifier
