#
#	FILE:	 Balanced.py
#	AUTHOR:  Andy Szybalski
#	PURPOSE: Global map script - Solid pangaea, balanced strategic resources.
#-----------------------------------------------------------------------------
#	Copyright (c) 2004, 2005 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvMapGeneratorUtil
from CvMapGeneratorUtil import HintedWorld
from CvMapGeneratorUtil import BonusBalancer

balancer = BonusBalancer()

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_BALANCED_DESCR"

# Rise of Mankind 2.5
def isAdvancedMap():
	"This map should show up in simple mode"
	return 0
# Rise of Mankind 2.5

def getNumCustomMapOptions():
	return 1

def getNumHiddenCustomMapOptions():
	return 1

def getCustomMapOptionName(argsList):
	translated_text = unicode(CyTranslator().getText("TXT_KEY_MAP_WORLD_WRAP", ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	return 3

def getCustomMapOptionDescAt(argsList):
	iSelection = argsList[1]
	selection_names = ["TXT_KEY_MAP_WRAP_FLAT",
					   "TXT_KEY_MAP_WRAP_CYLINDER",
					   "TXT_KEY_MAP_WRAP_TOROID"]
	translated_text = unicode(CyTranslator().getText(selection_names[iSelection], ()))
	return translated_text

def getCustomMapOptionDefault(argsList):
	return 1

def isRandomCustomMapOption(argsList):
	return false

def getWrapX():
	map = CyMap()
	return (map.getCustomMapOption(0) == 1 or map.getCustomMapOption(0) == 2)

def getWrapY():
	map = CyMap()
	return (map.getCustomMapOption(0) == 2)

def getTopLatitude():
	return 70
def getBottomLatitude():
	return -70

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Balanced) ...")
	global hinted_world
	hinted_world = HintedWorld(16,8)

	mapRand = CyGlobalContext().getGame().getMapRand()

	numBlocks = hinted_world.w * hinted_world.h
	numBlocksLand = int(numBlocks*0.25)
	cont = hinted_world.addContinent(numBlocksLand,mapRand.get(5, "Generate Plot Types PYTHON")+4,mapRand.get(3, "Generate Plot Types PYTHON")+2)
	if not cont:
		print "Couldn't create continent! Reverting to C implementation."
		CyPythonMgr().allowDefaultImpl()
	else:
		for x in range(hinted_world.w):
			for y in (0, hinted_world.h - 1):
				hinted_world.setValue(x,y, 1) # force ocean at poles
		hinted_world.buildAllContinents()
		return hinted_world.generatePlotTypes(shift_plot_types=True)

# subclass TerrainGenerator to eliminate arctic, equatorial latitudes

class BTerrainGenerator(CvMapGeneratorUtil.TerrainGenerator):
	def getLatitudeAtPlot(self, iX, iY):
		"returns 0.0 for tropical, up to 1.0 for polar"
		lat = CvMapGeneratorUtil.TerrainGenerator.getLatitudeAtPlot(self, iX, iY) 	# range [0,1]
		lat = 0.05 + 0.75*lat				# range [0.05, 0.75]
		return lat

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Balanced) ...")
	terraingen = BTerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

# subclass FeatureGenerator to eliminate arctic, equatorial latitudes

class BFeatureGenerator(CvMapGeneratorUtil.FeatureGenerator):
	def getLatitudeAtPlot(self, iX, iY):
		"returns 0.0 for tropical, up to 1.0 for polar"
		lat = CvMapGeneratorUtil.FeatureGenerator.getLatitudeAtPlot(self, iX, iY) 	# range [0,1]
		lat = 0.05 + 0.75*lat				# range [0.05, 0.75]
		return lat

def addFeatures():
	NiTextOut("Adding Features (Python Balanced) ...")
	featuregen = BFeatureGenerator()
	featuregen.addFeatures()
	return 0

def normalizeAddExtras():
	balancer.normalizeAddExtras()
	CyPythonMgr().allowDefaultImpl()	# do the rest of the usual normalizeStartingPlots stuff, don't overrride

def addBonusType(argsList):
	[iBonusType] = argsList

	if (not balancer.isSkipBonus(iBonusType)):
		CyPythonMgr().allowDefaultImpl()

def afterGeneration():
	CvMapGeneratorUtil.placeC2CBonuses()
