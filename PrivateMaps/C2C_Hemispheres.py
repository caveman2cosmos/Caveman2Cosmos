#
#	FILE:	 Hemispheres.py
#	AUTHOR:  Ben Sarsgard
#	PURPOSE: Global map script - Hemisphere or quadrant split with oceanic divide
#		Mostly adapted from Sirian's Big_and_Small
#	VERSION: 1.20
#-----------------------------------------------------------------------------
#	Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
#-----------------------------------------------------------------------------
#

from CvPythonExtensions import *
import CvMapGeneratorUtil
#from CvMapGeneratorUtil import FractalWorld
from CvMapGeneratorUtil import TerrainGenerator
from CvMapGeneratorUtil import FeatureGenerator

def getDescription():
	#TODO: get my own text string
	return "TXT_KEY_MAP_SCRIPT_LEFT_AND_RIGHT_DESCR"

def isAdvancedMap():
	"This map should not show up in simple mode"
	return 0

def getNumHiddenCustomMapOptions():
	return 0

def getNumCustomMapOptions():
	return 3

def getCustomMapOptionName(argsList):
	[iOption] = argsList
	option_names = {
		0:	"TXT_KEY_MAP_SCRIPT_CONTINENTS_SIZE",
		1:	"TXT_KEY_MAP_SCRIPT_ISLANDS_SIZE",
		2:	"TXT_KEY_MAP_SCRIPT_NUMBER_OF_CONTINENTS"
		}
	translated_text = unicode(CyTranslator().getText(option_names[iOption], ()))
	return translated_text

def getNumCustomMapOptionValues(argsList):
	[iOption] = argsList
	option_values = {
		0:	4,
		1:	2,
		2:	5
		}
	return option_values[iOption]

def getCustomMapOptionDescAt(argsList):
	[iOption, iSelection] = argsList
	selection_names = {
		0:	{
			0: "TXT_KEY_MAP_SCRIPT_MASSIVE_CONTINENTS",
			1: "TXT_KEY_MAP_SCRIPT_NORMAL_CONTINENTS",
			2: "TXT_KEY_MAP_SCRIPT_SNAKY_CONTINENTS",
			3: "TXT_KEY_MAP_SCRIPT_VARIED"
			},
		1:	{
			0: "TXT_KEY_MAP_SCRIPT_ISLANDS",
			1: "TXT_KEY_MAP_SCRIPT_TINY_ISLANDS"
			},
		2:	{
			0: "2",
			1: "3",
			2: "4",
			3: "5",
			4: "6"
			}
		}
	if (iOption < 3):
		translated_text = unicode(CyTranslator().getText(selection_names[iOption][iSelection], ()))
	else:
		translated_text = selection_names[iOption][iSelection]
	return translated_text

def getCustomMapOptionDefault(argsList):
	[iOption] = argsList
	option_defaults = {
		0:	1,
		1:	1,
		2:	0
		}
	return option_defaults[iOption]

def minStartingDistanceModifier():
	return -12

def beforeGeneration():
	global xShiftRoll
	global yShiftRoll
	global ySplitRoll
	global yPortionRoll
	gc = CyGlobalContext()
	dice = gc.getGame().getMapRand()

	# Binary shift roll (for horizontal shifting if Island Region Separate).
	xShiftRoll = dice.get(2, "Region Shift, Horizontal - Left and Right PYTHON")
	yShiftRoll = dice.get(2, "Region Shift, Vertical - Left and Right PYTHON")
	ySplitRoll = dice.get(2, "Region Split, Vertical - Left and Right PYTHON")
	yPortionRoll = dice.get(2, "Region Portioning, Vertical - Left and Right PYTHON")
	print xShiftRoll

class BnSMultilayeredFractal(CvMapGeneratorUtil.MultilayeredFractal):
	def generateIslandRegion(self, minTinies, extraTinies, iWestX, iSouthY, iWidth, iHeight, iGrain):
		# Add a few random patches of Tiny Islands first.
		#TODO: Base numTinies on global prevalance option
		numTinies = minTinies + self.dice.get(extraTinies, "Tiny Islands - Custom Continents PYTHON")
		print("Patches of Tiny Islands: ", numTinies)
		if numTinies:
			for tiny_loop in range(numTinies):
				tinyWidth = int(self.iW * 0.15)
				tinyHeight = int(self.iH * 0.15)

				tinyWestX = iWestX + self.dice.get(iWidth - tinyWidth, "Tiny Longitude - Custom Continents PYTHON")
				tinySouthY = iSouthY + self.dice.get(iHeight - tinyHeight, "Tiny Latitude - Custom Continents PYTHON")

				self.generatePlotsInRegion(80,
										   tinyWidth, tinyHeight,
										   tinyWestX, tinySouthY,
										   iGrain, 3,
										   0, self.iTerrainFlags,
										   6, 5,
										   True, 3,
										   -1, False,
										   False
										   )
		return 0

	def generateContinentRegion(self, iWater, iWidth, iHeight, iWestX, iSouthY, iGrain, xExp):
		self.generatePlotsInRegion(iWater,
								   iWidth, iHeight,
								   iWestX, iSouthY,
								   iGrain, 4,
								   self.iRoundFlags, self.iTerrainFlags,
								   xExp, 6,
								   True, 15,
								   -1, False,
								   False
								   )
		return 0

	def generatePlotsByRegion(self):
		# Sirian's MultilayeredFractal class, controlling function.
		# You -MUST- customize this function for each use of the class.
		global xShiftRoll
		global yShiftRoll
		global ySplitRoll
		global yPortionRoll

		print("getSeaLevelChange", self.GC.getSeaLevelInfo(self.map.getSeaLevel()).getSeaLevelChange())

		if (self.map.getCustomMapOption(0) == 3):
			# Generate varied
			iContinentsGrain = 1
			iSecondaryContinentsGrain = 3
			iTertiaryContinentsGrain = 2
			iPrimaryWater = 74
			iSecondaryWater = 79
			iTertiaryWater = 76
		else:
			iContinentsGrain = 1 + self.map.getCustomMapOption(0)
			iSecondaryContinentsGrain = 1 + self.map.getCustomMapOption(0)
			iTertiaryContinentsGrain = 1 + self.map.getCustomMapOption(0)
			iPrimaryWater = 74
			iSecondaryWater = 74
			iTertiaryWater = 74

		iPrimaryWater += self.GC.getSeaLevelInfo(self.map.getSeaLevel()).getSeaLevelChange()
		iSecondaryWater += self.GC.getSeaLevelInfo(self.map.getSeaLevel()).getSeaLevelChange()
		iTertiaryWater += self.GC.getSeaLevelInfo(self.map.getSeaLevel()).getSeaLevelChange()

		splitYBigger = 0.5
		splitYSmaller = 0.5
		splitYBuffer = 0.1

		iIslandsGrain = 3 + self.map.getCustomMapOption(1)
		tinyIslandOverlap = 0
		regions = 2 + self.map.getCustomMapOption(2)
		if (regions == 2):
			vSplitPrimary = 0
			vSplitSecondary = 0
			vSplitTertiary = 0
			tripleSplit = 0
		elif (regions == 3):
			vSplitPrimary = 0
			vSplitSecondary = 0
			vSplitTertiary = 0
			tripleSplit = 1
		elif (regions == 4):
			vSplitPrimary = 1
			vSplitSecondary = 1
			vSplitTertiary = 0
			tripleSplit = 0
		elif (regions == 5):
			vSplitPrimary = 0
			vSplitSecondary = 1
			vSplitTertiary = 1
			tripleSplit = 1
		elif (regions == 6):
			vSplitPrimary = 1
			vSplitSecondary = 1
			vSplitTertiary = 1
			tripleSplit = 1
		else:
			#unexpected
			vSplitPrimary = 0
			vSplitSecondary = 0
			vSplitTertiary = 0
			tripleSplit = 0

		# Water variables need to differ if Overlap is set. Defining default here.
		#TODO: Set this from the global option
		#iWater = 74

		# Base values for full map
		iSouthY = 0
		iNorthY = self.iH - 1
		iHeight = iNorthY - iSouthY + 1
		iWestX = 0
		iEastX = self.iW - 1
		iWidth = iEastX - iWestX + 1
		print("Cont South: ", iSouthY, "Cont North: ", iNorthY, "Cont Height: ", iHeight)

		if tinyIslandOverlap:
			self.generateIslandRegion(4, 6, iWestX, iSouthY, iWidth, iHeight, iIslandsGrain)

		# Add the Continents.
		# Horizontal dimensions may be affected by overlap and/or shift.
		# The regions are separate, with continents only in one part, islands only in the other.
		# Set X exponent to square setting:
		xExp = 6
		# Handle horizontal shift for the Continents layer.
		# (This will choose one side or the other for this region then fit it properly in its space).
		if tripleSplit:
			if xShiftRoll:
				westShift = int(0.33 * self.iW)
				eastShift = int(0.33 * self.iW)
			else:
				westShift = 0
				eastShift = int(0.66 * self.iW)
		else:
			if xShiftRoll:
				westShift = int(0.5 * self.iW)
				eastShift = 0
			else:
				westShift = 0
				eastShift = int(0.5 * self.iW)

		iWestX = westShift
		iEastX = self.iW - eastShift
		iWidth = iEastX - iWestX

		if vSplitPrimary:
			# Do first in split
			if yPortionRoll:
				if yShiftRoll:
					northShift = int(splitYBuffer * self.iH)
					southShift = int(splitYBigger * self.iH)
				else:
					northShift = int(splitYSmaller * self.iH)
					southShift = int(splitYBuffer * self.iH)
			else:
				if yShiftRoll:
					northShift = int(splitYBuffer * self.iH)
					southShift = int(splitYSmaller * self.iH)
				else:
					northShift = int(splitYBigger * self.iH)
					southShift = int(splitYBuffer * self.iH)

			iSouthY = southShift
			iNorthY = self.iH - northShift
			iHeight = iNorthY - iSouthY

			print("Island West: ", iWestX, "Island East: ", iEastX, "Isl Width: ", iWidth)
			self.generateContinentRegion(iPrimaryWater, iWidth, iHeight, iWestX, iSouthY, iContinentsGrain, xExp)

			if (tinyIslandOverlap == 0):
				self.generateIslandRegion(1, 2, iWestX, iSouthY, iWidth, iHeight, iIslandsGrain)

			# Second in split
			if yPortionRoll:
				if yShiftRoll:
					northShift = int(splitYSmaller * self.iH)
					southShift = int(splitYBuffer * self.iH)
				else:
					northShift = int(splitYBuffer * self.iH)
					southShift = int(splitYBigger * self.iH)
			else:
				if yShiftRoll:
					northShift = int(splitYBigger * self.iH)
					southShift = int(splitYBuffer * self.iH)
				else:
					northShift = int(splitYBuffer * self.iH)
					southShift = int(splitYSmaller * self.iH)

			iSouthY = southShift
			iNorthY = self.iH - northShift
			iHeight = iNorthY - iSouthY

			print("Island West: ", iWestX, "Island East: ", iEastX, "Isl Width: ", iWidth)
			self.generateContinentRegion(iPrimaryWater, iWidth, iHeight, iWestX, iSouthY, iContinentsGrain, xExp)

			if (tinyIslandOverlap == 0):
				self.generateIslandRegion(1, 2, iWestX, iSouthY, iWidth, iHeight, iIslandsGrain)

		else:
			# Only one primary region
			iSouthY = 0
			iNorthY = self.iH - 1
			iHeight = iNorthY - iSouthY + 1
			print("Cont West: ", iWestX, "Cont East: ", iEastX, "Cont Width: ", iWidth)
			self.generateContinentRegion(iPrimaryWater, iWidth, iHeight, iWestX, iSouthY, iContinentsGrain, xExp)

			if (tinyIslandOverlap == 0):
				self.generateIslandRegion(2, 3, iWestX, iSouthY, iWidth, iHeight, iIslandsGrain)

		# Add the Secondary continents.
		# Horizontal dimensions may be affected by overlap and/or shift.
		# The regions are separate, with continents only in one part, islands only in the other.
		# Set X exponent to square setting:
		xExp = 6
		# Handle horizontal shift for the Continents layer.
		# (This will choose one side or the other for this region then fit it properly in its space).
		if tripleSplit:
			if xShiftRoll:
				westShift = 0
				eastShift = int(0.66 * self.iW)
			else:
				westShift = int(0.33 * self.iW)
				eastShift = int(0.33 * self.iW)
		else:
			if xShiftRoll:
				westShift = 0
				eastShift = int(0.5 * self.iW)
			else:
				westShift = int(0.5 * self.iW)
				eastShift = 0

		iWestX = westShift
		iEastX = self.iW - eastShift
		iWidth = iEastX - iWestX

		if vSplitSecondary:
			# Do first in split
			if yPortionRoll:
				if yShiftRoll:
					northShift = int(splitYBuffer * self.iH)
					southShift = int(splitYBigger * self.iH)
				else:
					northShift = int(splitYSmaller * self.iH)
					southShift = int(splitYBuffer * self.iH)
			else:
				if yShiftRoll:
					northShift = int(splitYBuffer * self.iH)
					southShift = int(splitYSmaller * self.iH)
				else:
					northShift = int(splitYBigger * self.iH)
					southShift = int(splitYBuffer * self.iH)

			iSouthY = southShift
			iNorthY = self.iH - northShift
			iHeight = iNorthY - iSouthY

			print("Island West: ", iWestX, "Island East: ", iEastX, "Isl Width: ", iWidth)
			self.generateContinentRegion(iSecondaryWater, iWidth, iHeight, iWestX, iSouthY, iSecondaryContinentsGrain, xExp)

			if (tinyIslandOverlap == 0):
				self.generateIslandRegion(2, 3, iWestX, iSouthY, iWidth, iHeight, iIslandsGrain)

			# Second in split
			if yPortionRoll:
				if yShiftRoll:
					northShift = int(splitYSmaller * self.iH)
					southShift = int(splitYBuffer * self.iH)
				else:
					northShift = int(splitYBuffer * self.iH)
					southShift = int(splitYBigger * self.iH)
			else:
				if yShiftRoll:
					northShift = int(splitYBigger * self.iH)
					southShift = int(splitYBuffer * self.iH)
				else:
					northShift = int(splitYBuffer * self.iH)
					southShift = int(splitYSmaller * self.iH)

			iSouthY = southShift
			iNorthY = self.iH - northShift
			iHeight = iNorthY - iSouthY

			print("Island West: ", iWestX, "Island East: ", iEastX, "Isl Width: ", iWidth)
			self.generateContinentRegion(iSecondaryWater, iWidth, iHeight, iWestX, iSouthY, iSecondaryContinentsGrain, xExp)

			if (tinyIslandOverlap == 0):
				self.generateIslandRegion(2, 3, iWestX, iSouthY, iWidth, iHeight, iIslandsGrain)
		else:
			# Only one secondary region
			iSouthY = 0
			iNorthY = self.iH - 1
			iHeight = iNorthY - iSouthY + 1
			print("Island West: ", iWestX, "Island East: ", iEastX, "Isl Width: ", iWidth)
			self.generateContinentRegion(iSecondaryWater, iWidth, iHeight, iWestX, iSouthY, iSecondaryContinentsGrain, xExp)

			if (tinyIslandOverlap == 0):
				self.generateIslandRegion(3, 4, iWestX, iSouthY, iWidth, iHeight, iIslandsGrain)

		if tripleSplit:
			# Add the Tertiary continents.
			# Horizontal dimensions may be affected by overlap and/or shift.
			# The regions are separate, with continents only in one part, islands only in the other.
			# Set X exponent to square setting:
			xExp = 6
			# Handle horizontal shift for the Continents layer.
			# (This will choose one side or the other for this region then fit it properly in its space).
			westShift = int(0.66 * self.iW)
			eastShift = 0

			iWestX = westShift
			iEastX = self.iW - eastShift
			iWidth = iEastX - iWestX

			if vSplitTertiary:
				# Do first in split
				if yPortionRoll:
					if yShiftRoll:
						northShift = int(splitYBuffer * self.iH)
						southShift = int(splitYBigger * self.iH)
					else:
						northShift = int(splitYSmaller * self.iH)
						southShift = int(splitYBuffer * self.iH)
				else:
					if yShiftRoll:
						northShift = int(splitYBuffer * self.iH)
						southShift = int(splitYSmaller * self.iH)
					else:
						northShift = int(splitYBigger * self.iH)
						southShift = int(splitYBuffer * self.iH)

				iSouthY = southShift
				iNorthY = self.iH - northShift
				iHeight = iNorthY - iSouthY

				print("Island West: ", iWestX, "Island East: ", iEastX, "Isl Width: ", iWidth)
				self.generateContinentRegion(iTertiaryWater, iWidth, iHeight, iWestX, iSouthY, iTertiaryContinentsGrain, xExp)

				if (tinyIslandOverlap == 0):
					self.generateIslandRegion(2, 3, iWestX, iSouthY, iWidth, iHeight, iIslandsGrain)

				# Second in split
				if yPortionRoll:
					if yShiftRoll:
						northShift = int(splitYSmaller * self.iH)
						southShift = int(splitYBuffer * self.iH)
					else:
						northShift = int(splitYBuffer * self.iH)
						southShift = int(splitYBigger * self.iH)
				else:
					if yShiftRoll:
						northShift = int(splitYBigger * self.iH)
						southShift = int(splitYBuffer * self.iH)
					else:
						northShift = int(splitYBuffer * self.iH)
						southShift = int(splitYSmaller * self.iH)

				iSouthY = southShift
				iNorthY = self.iH - northShift
				iHeight = iNorthY - iSouthY

				print("Island West: ", iWestX, "Island East: ", iEastX, "Isl Width: ", iWidth)
				self.generateContinentRegion(iTertiaryWater, iWidth, iHeight, iWestX, iSouthY, iTertiaryContinentsGrain, xExp)

				if (tinyIslandOverlap == 0):
					self.generateIslandRegion(2, 3, iWestX, iSouthY, iWidth, iHeight, iIslandsGrain)
			else:
				# Only one tertiary region
				iSouthY = 0
				iNorthY = self.iH - 1
				iHeight = iNorthY - iSouthY + 1
				print("Island West: ", iWestX, "Island East: ", iEastX, "Isl Width: ", iWidth)
				self.generateContinentRegion(iTertiaryWater, iWidth, iHeight, iWestX, iSouthY, iTertiaryContinentsGrain, xExp)

				if (tinyIslandOverlap == 0):
					self.generateIslandRegion(3, 4, iWestX, iSouthY, iWidth, iHeight, iIslandsGrain)

		# All regions have been processed. Plot Type generation completed.
		print "Done"
		return self.wholeworldPlotTypes

'''
Regional Variables Key:

iWaterPercent,
iRegionWidth, iRegionHeight,
iRegionWestX, iRegionSouthY,
iRegionGrain, iRegionHillsGrain,
iRegionPlotFlags, iRegionTerrainFlags,
iRegionFracXExp, iRegionFracYExp,
bShift, iStrip,
rift_grain, has_center_rift,
invert_heights
'''

def generatePlotTypes():
	NiTextOut("Setting Plot Types (Python Custom Continents) ...")
	fractal_world = BnSMultilayeredFractal()
	plotTypes = fractal_world.generatePlotsByRegion()
	return plotTypes

def generateTerrainTypes():
	NiTextOut("Generating Terrain (Python Custom Continents) ...")
	terraingen = TerrainGenerator()
	terrainTypes = terraingen.generateTerrain()
	return terrainTypes

def addFeatures():
	NiTextOut("Adding Features (Python Custom Continents) ...")
	featuregen = FeatureGenerator()
	featuregen.addFeatures()
	return 0

def afterGeneration():
	CvMapGeneratorUtil.placeC2CBonuses()
