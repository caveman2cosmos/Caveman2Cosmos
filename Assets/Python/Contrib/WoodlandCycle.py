
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
		self.FEATURE_BURNT_FOREST = GC.getInfoTypeForString('FEATURE_BURNT_FOREST')
		self.FEATURE_YOUNG_FOREST = GC.getInfoTypeForString('FEATURE_YOUNG_FOREST')
		self.FEATURE_FOREST = GC.getInfoTypeForString('FEATURE_FOREST')
		self.FEATURE_ANCIENT_FOREST = GC.getInfoTypeForString('FEATURE_ANCIENT_FOREST')
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
		self.iFactorGS = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getGrowthPercent()

	# Called at the beginning of the end of each turn
	def onBeginGameTurn(self, argsList):
		iMaxIndex = self.iMaxIndex
		index = GAME.getSorenRandNum(iMaxIndex, "Random Plot")
		aList = []
		iMaxCount = iMaxIndex / 128
		if iMaxCount < 1:
			iMaxCount = 1
		iCount = 0
		while iCount < iMaxCount:

			while index in aList:
				index = GAME.getSorenRandNum(iMaxIndex, "Random Plot")

			aList.append(index)
			iCount += 1
			if 32 <= GAME.getSorenRandNum(self.iFactorGS, "New"): continue

			CyPlot = self.plots[index]
			iFeature = CyPlot.getFeatureType()
			if iFeature == -1:
				if not GAME.getSorenRandNum(10, "New"):
					iImp = CyPlot.getImprovementType()

					if CyPlot.canHaveFeature(self.FEATURE_BAMBOO) and not GAME.getSorenRandNum(9, "Bamboo"):
						CyPlot.setFeatureType(self.FEATURE_BAMBOO, 0)
					elif CyPlot.canHaveFeature(self.FEATURE_YOUNG_FOREST):
						CyPlot.setFeatureType(self.FEATURE_YOUNG_FOREST, 0)

			elif iFeature == self.FEATURE_BURNT_FOREST:
				CyPlot.setFeatureType(-1, -1)
				if CyPlot.canHaveFeature(self.FEATURE_BAMBOO) and not GAME.getSorenRandNum(9, "Bamboo"):
					CyPlot.setFeatureType(self.FEATURE_BAMBOO, 0)
				elif CyPlot.canHaveFeature(self.FEATURE_YOUNG_FOREST):
					CyPlot.setFeatureType(self.FEATURE_YOUNG_FOREST, 0)
				else:
					CyPlot.setFeatureType(iFeature, 0)

			elif iFeature == self.FEATURE_BAMBOO:
				if not GAME.getSorenRandNum(9, "Burn"):
					CyPlot.setFeatureType(self.FEATURE_BURNT_FOREST, 0)

			elif iFeature == self.FEATURE_YOUNG_FOREST:
				CyPlot.setFeatureType(self.FEATURE_FOREST, 0)

			elif iFeature == self.FEATURE_FOREST:
				if not GAME.getSorenRandNum(5, "Burn"):
					CyPlot.setFeatureType(self.FEATURE_BURNT_FOREST, 0)
				else:
					CyPlot.setFeatureType(self.FEATURE_ANCIENT_FOREST, 0)

			elif iFeature == self.FEATURE_FOREST:
				if not GAME.getSorenRandNum(9, "Burn"):
					CyPlot.setFeatureType(self.FEATURE_BURNT_FOREST, 0)
