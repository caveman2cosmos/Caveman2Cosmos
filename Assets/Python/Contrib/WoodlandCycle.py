
from CvPythonExtensions import *

# globals
GC = CyGlobalContext()
GAME = GC.getGame()

class WoodlandCycle:

	def __init__(self, customEM):
		self.customEM = customEM
		customEM.addEventHandler("GameStart", self.onGameStart)
		customEM.addEventHandler("OnLoad", self.onLoadGame)
		customEM.addEventHandler("BeginGameTurn", self.onBeginGameTurn)
		self.FEATURE_FOREST_BURNT = GC.getInfoTypeForString('FEATURE_FOREST_BURNT')
		self.FEATURE_FOREST_NEW = GC.getInfoTypeForString('FEATURE_FOREST_NEW')
		self.FEATURE_FOREST = GC.getInfoTypeForString('FEATURE_FOREST')
		self.FEATURE_FOREST_OLD = GC.getInfoTypeForString('FEATURE_FOREST_OLD')
		self.FEATURE_BAMBOO = GC.getInfoTypeForString('FEATURE_BAMBOO')

	def onGameStart(self, argsList): self.cache()
	def onLoadGame(self, argsList): self.cache()

	def cache(self):
		MAP = GC.getMap()
		iNumPlots = MAP.numPlots()
		iMaxIndex = -1
		self.plots = plots = []
		for i in xrange(iNumPlots):
			CyPlot = MAP.plotByIndex(i)
			if CyPlot.isWater() or CyPlot.isPeak(): continue
			plots.append(CyPlot)
			iMaxIndex += 1
		self.iMaxIndex = iMaxIndex
		iPasses = MAP.getLandPlots() / 400
		if iPasses < 1:
			iPasses = 1
		self.iPasses = iPasses
		self.iFactorGS = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getGrowthPercent()

	# Called at the beginning of the end of each turn
	def onBeginGameTurn(self, argsList):
		iMaxIndex = self.iMaxIndex
		index = GAME.getSorenRandNum(iMaxIndex, "Random Plot")
		aList = []
		iCount = 0
		while iCount < self.iPasses and iCount <= iMaxIndex:

			if index < iMaxIndex:
				index += 1
			else: index = 0
			while index in aList:
				index = GAME.getSorenRandNum(iMaxIndex, "Random Plot")
			aList.append(index)
			iCount += 1
			if 128 < GAME.getSorenRandNum(self.iFactorGS, "New"): continue

			CyPlot = self.plots[index]
			iFeature = CyPlot.getFeatureType()
			if iFeature == -1:
				if 48 > GAME.getSorenRandNum(self.iFactorGS, "New"):
					if CyPlot.canHaveFeature(self.FEATURE_BAMBOO) and not GAME.getSorenRandNum(9, "Bamboo"):
						CyPlot.setFeatureType(self.FEATURE_BAMBOO, 0)
					elif CyPlot.canHaveFeature(self.FEATURE_FOREST_NEW):
						CyPlot.setFeatureType(self.FEATURE_FOREST_NEW, 0)
					else: continue
					index = GAME.getSorenRandNum(iMaxIndex, "Random Plot")

			elif iFeature == self.FEATURE_FOREST_BURNT:
				CyPlot.setFeatureType(-1, -1)
				if CyPlot.canHaveFeature(self.FEATURE_BAMBOO) and not GAME.getSorenRandNum(9, "Bamboo"):
					CyPlot.setFeatureType(self.FEATURE_BAMBOO, 0)
				elif CyPlot.canHaveFeature(self.FEATURE_FOREST_NEW):
					CyPlot.setFeatureType(self.FEATURE_FOREST_NEW, 0)
				else:
					CyPlot.setFeatureType(iFeature, 0)
					continue
				index = GAME.getSorenRandNum(iMaxIndex, "Random Plot")

			elif iFeature == self.FEATURE_BAMBOO:
				if not GAME.getSorenRandNum(9, "Burn"):
					CyPlot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)
					index = GAME.getSorenRandNum(iMaxIndex, "Random Plot")

			elif iFeature == self.FEATURE_FOREST_NEW:
				CyPlot.setFeatureType(self.FEATURE_FOREST, 0)
				index = GAME.getSorenRandNum(iMaxIndex, "Random Plot")

			elif iFeature == self.FEATURE_FOREST:
				if not GAME.getSorenRandNum(5, "Burn"):
					CyPlot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)
				else:
					CyPlot.setFeatureType(self.FEATURE_FOREST_OLD, 0)
				index = GAME.getSorenRandNum(iMaxIndex, "Random Plot")
			elif iFeature == self.FEATURE_FOREST:
				if not GAME.getSorenRandNum(9, "Burn"):
					CyPlot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)
					index = GAME.getSorenRandNum(iMaxIndex, "Random Plot")
