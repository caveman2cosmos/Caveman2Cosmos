
from CvPythonExtensions import *
import CvUtil #, BugUtil
# globals
GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

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

			bBurn = False
			CyPlot = self.plots[plotIndex.pop()]
			iImp = CyPlot.getImprovementType()
			iFeature = CyPlot.getFeatureType()
			if iFeature == -1:
				if not GAME.getSorenRandNum(10, "New"):
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

				if iImp != CyPlot.getImprovementType() and CyPlot.canHaveImprovement(iImp, -1, True):
					CyPlot.setImprovementType(iImp)

			elif iFeature == self.FEATURE_BAMBOO:
				if not GAME.getSorenRandNum(50, "Burn"):
					bBurn = True
					CyPlot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)

			elif iFeature == self.FEATURE_FOREST_YOUNG:
				CyPlot.setFeatureType(self.FEATURE_FOREST, 0)

			elif iFeature == self.FEATURE_FOREST:
				iRand = GAME.getSorenRandNum(148, "forestChange")
				if iRand < 3:
					bBurn = True
					CyPlot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)
				elif iRand < 7:
					CyPlot.setFeatureType(self.FEATURE_FOREST_ANCIENT, 0)

			elif iFeature == self.FEATURE_FOREST_ANCIENT:
				iRand = GAME.getSorenRandNum(64, "Burn")
				if iRand < 4:
					bBurn = True
					CyPlot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)
				elif iRand == 4:
					CyPlot.setFeatureType(-1, 0)
					if CyPlot.canHaveFeature(self.FEATURE_JUNGLE):
						CyPlot.setFeatureType(self.FEATURE_JUNGLE, 0)
					else: CyPlot.setFeatureType(self.FEATURE_FOREST_ANCIENT, 0)

			if bBurn:
				iPlayer = CyPlot.getOwner()
				if iPlayer > -1 and iPlayer == GAME.getActivePlayer():
					bActivePlayer = True
					CyCity = CyPlot.getWorkingCity()
					if CyCity:
						CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_FOREST_FIRE_CITY_VICINITY", (CyCity.getName(),)), iPlayer, 10, \
							'Art/Terrain/Features/Forest_Burnt/ButtonBurntForest.dds', ColorTypes(13), CyPlot.getX(), CyPlot.getY(), True, True)
					else: CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_FOREST_FIRE_REMOTE",()), iPlayer, 6, \
							'Art/Terrain/Features/Forest_Burnt/ButtonBurntForest.dds', ColorTypes(4), CyPlot.getX(), CyPlot.getY(), True, True)
				else: bActivePlayer = False

				if CyPlot.isActiveVisible(GAME.isDebugMode()):
					point = CyPlot.getPoint()
					CyEngine().triggerEffect(GC.getInfoTypeForString('EFFECT_FOREST_FIRE'), point)
					CyAudioGame().Play3DSound("AS3D_FOREST_FIRE", point.x, point.y, point.z)

				for i in range(CyPlot.getNumUnits()):
					CyUnit = CyPlot.getUnit(i)
					if CyUnit.canFight():
						iHP = CyUnit.currHitPoints()
						iDamage = 5 + GAME.getSorenRandNum(29, "Ouch")
						if iHP > iDamage:
							CyUnit.changeDamage(iDamage, -1)
						else:
							if bActivePlayer:
								CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_FOREST_FIRE_UNIT_LOST",(CyUnit.getName(),)), iPlayer, 6, eColor=ColorTypes(9))
							CyUnit.kill(False, -1)
		#timer.log()
