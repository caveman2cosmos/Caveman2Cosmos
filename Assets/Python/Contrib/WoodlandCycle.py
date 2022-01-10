
from CvPythonExtensions import *
import CvUtil #, BugUtil
# globals
GC = CyGlobalContext()
MAP = GC.getMap()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

class WoodlandCycle:

	def __init__(self, customEM):
		self.customEM = customEM
		customEM.addEventHandler("GameStart", self.onGameStart)
		customEM.addEventHandler("OnLoad", self.onLoadGame)
		customEM.addEventHandler("BeginGameTurn", self.onBeginGameTurn)
		self.TERRAIN_TAIGA = GC.getInfoTypeForString('TERRAIN_TAIGA')
		self.TERRAIN_MUDDY = GC.getInfoTypeForString('TERRAIN_MUDDY')
		self.TERRAIN_LUSH = GC.getInfoTypeForString('TERRAIN_LUSH')
		self.FEATURE_FOREST_BURNT = GC.getInfoTypeForString('FEATURE_FOREST_BURNT')
		self.FEATURE_FOREST_YOUNG = GC.getInfoTypeForString('FEATURE_FOREST_YOUNG')
		self.FEATURE_FOREST = GC.getInfoTypeForString('FEATURE_FOREST')
		self.FEATURE_FOREST_ANCIENT = GC.getInfoTypeForString('FEATURE_FOREST_ANCIENT')
		self.FEATURE_JUNGLE = GC.getInfoTypeForString('FEATURE_JUNGLE')
		self.FEATURE_BAMBOO = GC.getInfoTypeForString('FEATURE_BAMBOO')

	def onGameStart(self, argsList): self.cache()
	def onLoadGame(self, argsList): self.cache()

	def cache(self):
		self.plots = plots = []
		for plot in MAP.plots():
			if plot.isWater() or plot.isPeak(): continue
			plots.append(plot)
		self.iMaxIndex = len(plots)
		self.iFactorGS = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent()

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
			plot = self.plots[plotIndex.pop()]
			iImp = plot.getImprovementType()
			iFeature = plot.getFeatureType()
			if iFeature == -1:
				if not GAME.getSorenRandNum(10, "New"):
					if plot.canHaveFeature(self.FEATURE_BAMBOO) and not GAME.getSorenRandNum(10, "Bamboo"):
						plot.setFeatureType(self.FEATURE_BAMBOO, 0)
					elif plot.canHaveFeature(self.FEATURE_FOREST_YOUNG):
						plot.setFeatureType(self.FEATURE_FOREST_YOUNG, 0)

			elif iFeature == self.FEATURE_FOREST_BURNT:
				plot.setFeatureType(-1, 0)
				if GAME.getSorenRandNum(3, "new forest"):
					if plot.canHaveFeature(self.FEATURE_BAMBOO) and not GAME.getSorenRandNum(10, "Bamboo"):
						plot.setFeatureType(self.FEATURE_BAMBOO, 0)
					elif plot.canHaveFeature(self.FEATURE_FOREST_YOUNG):
						plot.setFeatureType(self.FEATURE_FOREST_YOUNG, 0)

				if iImp != plot.getImprovementType() and plot.canHaveImprovement(iImp, -1, True):
					plot.setImprovementType(iImp)

			elif iFeature == self.FEATURE_BAMBOO:
				if not GAME.getSorenRandNum(50, "Burn"):
					bBurn = True
					plot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)

			elif iFeature == self.FEATURE_FOREST_YOUNG:
				iTerrain = plot.getTerrainType()
				if iTerrain == self.TERRAIN_TAIGA:
					plot.setFeatureType(self.FEATURE_FOREST, 2)
				elif iTerrain in (self.TERRAIN_LUSH, self.TERRAIN_MUDDY):
					plot.setFeatureType(self.FEATURE_FOREST, 0)
				else:
					# Could do a 50-50 pine vs leafy, but I'll base the prob on neighbour ratio
					iLeaf = 1
					iPine = 2 # Head start for pine as leafy tend to dominate
					# find neighbour variety
					iX = plot.getX(); iY = plot.getY()
					for x in xrange(iX - 1, iX + 2):
						for y in xrange(iY - 1, iY + 2):
							if x == iX and y == iY:
								continue
							plotX = MAP.plot(x, y)
							if plotX and plotX.getFeatureType() == self.FEATURE_FOREST:
								iVariety = plotX.getFeatureVariety()
								if iVariety: # Count snowy as pine
									iPine += 1
								else: iLeaf += 1

					if 100 < GAME.getSorenRandNum(100 + 100 * iLeaf / iPine, "Leafy"):
						plot.setFeatureType(self.FEATURE_FOREST, 0)
					else: # Pine
						plot.setFeatureType(self.FEATURE_FOREST, 1)

			elif iFeature == self.FEATURE_FOREST:
				iRand = GAME.getSorenRandNum(148, "forestChange")
				if iRand < 3:
					bBurn = True
					plot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)
				elif iRand < 7:
					plot.setFeatureType(self.FEATURE_FOREST_ANCIENT, 0)

			elif iFeature == self.FEATURE_FOREST_ANCIENT:
				iRand = GAME.getSorenRandNum(64, "Burn")
				if iRand < 4:
					bBurn = True
					plot.setFeatureType(self.FEATURE_FOREST_BURNT, 0)
				elif iRand == 4:
					plot.setFeatureType(-1, 0)
					if plot.canHaveFeature(self.FEATURE_JUNGLE):
						plot.setFeatureType(self.FEATURE_JUNGLE, 0)
					else: plot.setFeatureType(self.FEATURE_FOREST_ANCIENT, 0)

			if bBurn:
				iPlayer = plot.getOwner()
				if iPlayer > -1 and iPlayer == GAME.getActivePlayer():
					bActivePlayer = True
					CyCity = plot.getWorkingCity()
					if CyCity:
						CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_FOREST_FIRE_CITY_VICINITY", (CyCity.getName(),)), iPlayer, 10, \
							'Art/Terrain/Features/Forest_Burnt/ButtonBurntForest.dds', ColorTypes(13), plot.getX(), plot.getY(), True, True)
					else: CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_FOREST_FIRE_REMOTE",()), iPlayer, 6, \
							'Art/Terrain/Features/Forest_Burnt/ButtonBurntForest.dds', ColorTypes(4), plot.getX(), plot.getY(), True, True)
				else: bActivePlayer = False

				if plot.isActiveVisible(GAME.isDebugMode()) and not GC.getActivePlayer().isHumanDisabled():
					point = plot.getPoint()
					CyEngine().triggerEffect(GC.getInfoTypeForString('EFFECT_FOREST_FIRE'), point)
					CyAudioGame().Play3DSound("AS3D_FOREST_FIRE", point.x, point.y, point.z)

				for CyUnit in plot.units():
					if CyUnit.canFight():
						iHP = CyUnit.getHP()
						iDamage = 5 + GAME.getSorenRandNum(29, "Ouch")
						if iHP > iDamage:
							CyUnit.changeDamage(iDamage, -1)
						else:
							if bActivePlayer:
								CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_FOREST_FIRE_UNIT_LOST",(CyUnit.getName(),)), iPlayer, 6, eColor=ColorTypes(9))
							CyUnit.kill(False, -1)
		#timer.log()
