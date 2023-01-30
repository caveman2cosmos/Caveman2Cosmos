#
#	FILE:	 Shuffle.py
#	AUTHOR:  Bob Thomas (Sirian)
#	PURPOSE: Global map script - Return of Civ3's "random" map option.
#-----------------------------------------------------------------------------
#	Copyright (c) 2005, 2006 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvMapGeneratorUtil as MGU

balancer = MGU.BonusBalancer()

'''
This map script type added by popular demand.
The function is very similar to what "random" map type would provide in Civ3.

- Bob Thomas	October 30, 2005 // January 10, 2006
'''

def getDescription():
	return "TXT_KEY_MAP_SCRIPT_SHUFFLE_DESCR"

def isAdvancedMap():
	"This map should show up in simple mode"
	return 0

def getNumCustomMapOptions():
	return 2

def getNumHiddenCustomMapOptions():
	return 2

def getCustomMapOptionName(argsList):
	[iOption] = argsList
	option_names = {
		0:	"TXT_KEY_MAP_WORLD_WRAP",
		1:  "TXT_KEY_CONCEPT_RESOURCES"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0:	3,
		1:	2
		}
	return option_values[iOption]

def getCustomMapOptionDescAt(argsList):
	[iOption, iSelection] = argsList
	selection_names = {
		0:	{
			0: "TXT_KEY_MAP_WRAP_FLAT",
			1: "TXT_KEY_MAP_WRAP_CYLINDER",
			2: "TXT_KEY_MAP_WRAP_TOROID"
			},
		1:	{
			0: "TXT_KEY_WORLD_STANDARD",
			1: "TXT_KEY_MAP_BALANCED"
			}
		}
	translated_text = unicode(CyTranslator().getText(selection_names[iOption][iSelection], ()))
	return translated_text

def getCustomMapOptionDefault(argsList):
	[iOption] = argsList
	option_defaults = {
		0:	1,
		1:  0
		}
	return option_defaults[iOption]

def isRandomCustomMapOption(argsList):
	[iOption] = argsList
	option_random = {
		0:	false,
		1:  false
		}
	return option_random[iOption]

def getWrapX():
	map = CyMap()
	return (map.getCustomMapOption(0) == 1 or map.getCustomMapOption(0) == 2)

def getWrapY():
	map = CyMap()
	return (map.getCustomMapOption(0) == 2)

def normalizeAddExtras():
	if CyMap().getCustomMapOption(1) == 1:
		balancer.normalizeAddExtras()
	CyPythonMgr().allowDefaultImpl()	# do the rest of the usual normalizeStartingPlots stuff, don't overrride

def addBonusType(argsList):
	[iBonusType] = argsList

	if CyMap().getCustomMapOption(1) == 1 and CyGlobalContext().getBonusInfo(iBonusType).getType() in balancer.resourcesToBalance:
		return None # don't place any of this bonus randomly

	CyPythonMgr().allowDefaultImpl() # pretend we didn't implement this method, and let C handle this bonus in the default way

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Shuffle) ...")
	gc = CyGlobalContext()
	dice = gc.getGame().getMapRand()
	fractal_world = MGU.FractalWorld()
	grainRoll = 1 + dice.get(8, "Fractal Grain - Shuffle PYTHON")
	if grainRoll > 5: grainRoll -= 5
	if grainRoll == 2:
		fractal_world.initFractal(polar = True)
		return fractal_world.generatePlotTypes(water_percent=75)

	if grainRoll == 5: grainRoll -= 3
	fractal_world.initFractal(continent_grain = grainRoll, rift_grain = -1, has_center_rift = False, polar = True)
	return fractal_world.generatePlotTypes()

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Shuffle) ...")
	terraingen = MGU.TerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

def addFeatures():
	NiTextOut("Adding Features (Python Shuffle) ...")
	featuregen = MGU.FeatureGenerator()
	featuregen.addFeatures()
	return 0

def afterGeneration():
	MGU.placeC2CBonuses()
