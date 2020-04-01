
from CvPythonExtensions import *
#import BugUtil
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
		self.FEATURE_FOREST_YOUNG = GC.getInfoTypeForString('FEATURE_FOREST_YOUNG')
		self.FEATURE_FOREST = GC.getInfoTypeForString('FEATURE_FOREST')
		self.FEATURE_FOREST_ANCIENT = GC.getInfoTypeForString('FEATURE_FOREST_ANCIENT')
		self.FEATURE_JUNGLE = GC.getInfoTypeForString('FEATURE_JUNGLE')
		self.FEATURE_BAMBOO = GC.getInfoTypeForString('FEATURE_BAMBOO')

	def onGameStart(self, argsList): self.cache()
	def onLoadGame(self, argsList): self.cache()

	def cache(self):
		MAP = GC.getMap()
		self.plots = plots = []
		for i in xrange(MAP.numPlots()):
			CyPlot = MAP.plotByIndex(i)
			if CyPlot.isWater() or CyPlot.isPeak(): continue
			plots.append(CyPlot)
		self.iMaxIndex = len(plots)
		self.iFactorGS = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getGrowthPercent()

	# Called at the beginning of the end of each turn
	def onBeginGameTurn(self, argsList):
		#timer = BugUtil.Timer('WoodlandCycle')
		plotIndex = [0]*self.iMaxIndex
		shuffleList(self.iMaxIndex, GAME.getMapRand(), plotIndex)
		# reduce activity by era
		iMaxCount = self.iMaxIndex / (3*(GAME.getCurrentEra()+1))
		iCount = 0
		while iCount < iMaxCount:
			iCount += 1
			if 25 <= GAME.getSorenRandNum(self.iFactorGS, "New"): continue

			CyPlot = self.plots[plotIndex.pop()]
			iFeature = CyPlot.getFeatureType()
			if iFeature == -1:
				if not GAME.getSorenRandNum(10, "New"):
					iImp = CyPlot.getImprovementType()

					if CyPlot.canHaveFeature(self.FEATURE_BAMBOO) and not GAME.getSorenRandNum(9, "Bamboo"):
						CyPlot.setFeatureType(self.FEATURE_BAMBOO, 0)
					elif CyPlot.canHaveFeature(self.FEATURE_FOREST_YOUNG):
						CyPlot.setFeatureType(self.FEATURE_FOREST_YOUNG, 0)

			elif iFeature == self.FEATURE_FOREST_BURNT:
				CyPlot.setFeatureType(-1, 0)
				if CyPlot.canHaveFeature(self.FEATURE_BAMBOO) and not GAME.getSorenRandNum(9, "Bamboo"):
					CyPlot.setFeatureType(self.FEATURE_BAMBOO, 0)
				elif CyPlot.canHaveFeature(self.FEATURE_FOREST_YOUNG) and not GAME.getSorenRandNum(3, "Bamboo"):
					CyPlot.setFeatureType(self.FEATURE_FOREST_YOUNG, 0)
				else:
					CyPlot.setFeatureType(iFeature, 0)

			elif iFeature == self.FEATURE_BAMBOO:
				if not GAME.getSorenRandNum(50, "Burn"):
					CyPlot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)

			elif iFeature == self.FEATURE_FOREST_YOUNG:
				CyPlot.setFeatureType(self.FEATURE_FOREST, 0)

			elif iFeature == self.FEATURE_FOREST:
				iRand = GAME.getSorenRandNum(148, "forestChange")
				if iRand < 3:
					CyPlot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)
				elif iRand < 7:
					CyPlot.setFeatureType(self.FEATURE_FOREST_ANCIENT, 0)

			elif iFeature == self.FEATURE_FOREST_ANCIENT:
				iRand = GAME.getSorenRandNum(64, "Burn")
				if iRand < 4:
					CyPlot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)
				elif iRand == 4:
					CyPlot.setFeatureType(-1, 0)
					if CyPlot.canHaveFeature(self.FEATURE_JUNGLE):
						CyPlot.setFeatureType(self.FEATURE_JUNGLE, 0)
					else: CyPlot.setFeatureType(self.FEATURE_FOREST_ANCIENT, 0)
		#timer.log()
