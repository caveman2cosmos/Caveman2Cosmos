## Sid Meier's Civilization 4
## Copyright Firaxis Games 2007

from CvPythonExtensions import *

GC = CyGlobalContext()

class NaturalWonders:
	def __init__(self,):
		self.iFirstGold = 50					## Gold Granted to First Team to Discover
		self.iDiscoverHappiness = 1				## Happiness Granted to Any Team to Discover
		self.lBigWonder = ["FEATURE_PLATY_GREAT_BARRIER"]	## List of Natural Wonders that occupy 2 Tiles
		self.iNoNearbyRadius = 2				## Cannot have another Natural Wonders or Revealed Tile within X Tiles
		self.iPlaceChance = (CyMap().getWorldSize() +1) * 10	## Chance of Spawning each Natural Wonder
		self.iMaxYield = 4					## (0 to X) Yield
		self.lLatitude = [("FEATURE_PLATY_AURORA", 70, 90)]	## Min Latitude, Max Latitude

	def placeNaturalWonders(self):
		GAME = GC.getGame()
		for iFeature in xrange(GC.getNumFeatureInfos()):
			FeatureInfo = GC.getFeatureInfo(iFeature)
			sType = FeatureInfo.getType()
			if sType.find("FEATURE_PLATY_") == -1: continue
			if GAME.getSorenRandNum(100, "Random Plot") >= self.iPlaceChance: continue

			WonderPlot = []
			for pPlot in CyMap().plots():
				if pPlot.getBonusType(-1) > -1: continue

				## Nearby Plot Check ##
				bUnsuitable = False
				iRadius = 2
				bAdjacentPlot = True

				## Big Wonders ##
				if sType in self.lBigWonder:
					iRadius += 1
					bAdjacentPlot = False

				for x in xrange(pPlot.getX() - iRadius, pPlot.getX() + iRadius + 1):
					for y in xrange(pPlot.getY() - iRadius, pPlot.getY() + iRadius + 1):
						pAdjacentPlot = CyMap().plot(x, y)
						# if not bAllRevealed and pAdjacentPlot.isRevealed(-1, False):
							# bUnsuitable = True
							# break
						if pAdjacentPlot.getFeatureType() > -1 and GC.getFeatureInfo(pAdjacentPlot.getFeatureType()).getType().find("FEATURE_PLATY_") > -1:
							bUnsuitable = True
							break
					## Big Wonders ##
						if bAdjacentPlot == False and pAdjacentPlot.canHaveFeature(iFeature):
							if abs(pAdjacentPlot.getX() - pPlot.getX()) > 1: continue
							if abs(pAdjacentPlot.getY() - pPlot.getY()) > 1: continue
							if pAdjacentPlot.getBonusType(-1) > -1: continue
							# if not bAllRevealed and pAdjacentPlot.isRevealed(-1, False): continue
							bAdjacentPlot = True
					if bUnsuitable: break
				if bUnsuitable: continue

				## Latitude Check ##
				for i in self.lLatitude:
					if sType == i[0]:
						if pPlot.getLatitude() < i[1] or pPlot.getLatitude() > i[2]:
							bUnsuitable = True
							break
				if bUnsuitable: continue

				## Suitable Plot ##
				if pPlot.canHaveFeature(iFeature) and bAdjacentPlot:
					WonderPlot.append(pPlot)

			bWonder = False
			while len(WonderPlot) > 0:
				iWonderPlot = GAME.getSorenRandNum(len(WonderPlot), "Random Plot")
				pPlot = WonderPlot[iWonderPlot]

				## Big Wonders ##
				if sType in self.lBigWonder:
					AdjacentPlot = []
					for x in xrange(pPlot.getX() - 1, pPlot.getX() + 2):
						for y in xrange(pPlot.getY() - 1, pPlot.getY() + 2):
							pAdjacentPlot = CyMap().plot(x, y)
							if x == pPlot.getX() and y == pPlot.getY(): continue
							if pAdjacentPlot.canHaveFeature(iFeature):
								if pAdjacentPlot.getBonusType(-1) > -1: continue
								# if not bAllRevealed and pAdjacentPlot.isRevealed(-1, False): continue
								AdjacentPlot.append(pAdjacentPlot)
					if len(AdjacentPlot) == 0:
						del WonderPlot[iWonderPlot]
						continue
					pAdjacentPlot = AdjacentPlot[GAME.getSorenRandNum(len(AdjacentPlot), "Random Plot")]
					pAdjacentPlot.setFeatureType(iFeature, 0)
					# GAME.setPlotExtraYield(pAdjacentPlot.getX(), pAdjacentPlot.getY(), YieldTypes.YIELD_FOOD, GAME.getSorenRandNum(self.iMaxYield, "Adds Food"))
					# GAME.setPlotExtraYield(pAdjacentPlot.getX(), pAdjacentPlot.getY(), YieldTypes.YIELD_PRODUCTION, GAME.getSorenRandNum(self.iMaxYield, "Adds Production"))
					# GAME.setPlotExtraYield(pAdjacentPlot.getX(), pAdjacentPlot.getY(), YieldTypes.YIELD_COMMERCE, GAME.getSorenRandNum(self.iMaxYield, "Adds Commerce"))
					bWonder = True
					break

				## Standard Wonders ##
				bWonder = True
				break

			if bWonder:
				pPlot.setFeatureType(iFeature, 0)
				# GAME.setPlotExtraYield(pPlot.getX(), pPlot.getY(), YieldTypes.YIELD_FOOD, GAME.getSorenRandNum(self.iMaxYield, "Adds Food"))
				# GAME.setPlotExtraYield(pPlot.getX(), pPlot.getY(), YieldTypes.YIELD_PRODUCTION, GAME.getSorenRandNum(self.iMaxYield, "Adds Production"))
				# GAME.setPlotExtraYield(pPlot.getX(), pPlot.getY(), YieldTypes.YIELD_COMMERCE, GAME.getSorenRandNum(self.iMaxYield, "Adds Commerce"))
		return

	# def checkAllRevealTech(self):
		# iRevealTech = -1
		# for iTech in xrange(GC.getNumTechInfos()):
			# TechInfo = GC.getTechInfo(iTech)
			# if TechInfo.isMapVisible():
				# iRevealTech = iTech
				# break
		# if iRevealTech > 0:
			# for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
				# pTeamX = GC.getTeam(iTeamX)
				# if pTeamX.isHasTech(iRevealTech):
					# return True
		# return False

	def checkReveal(self, pPlot, iTeam):

		iFeature = pPlot.getFeatureType()
		if iFeature == -1: return

		CyTeam = GC.getTeam(iTeam)
		if CyTeam.isNPC(): return

		GAME = GC.getGame()
		if GAME.GetWorldBuilderMode(): return

		FeatureInfo = GC.getFeatureInfo(iFeature)
		sType = FeatureInfo.getType()
		if sType.find("FEATURE_PLATY_") == -1: return

		if sType in self.lBigWonder:
			bFound = False
			for x in xrange(pPlot.getX() - 1, pPlot.getX() + 2):
				for y in xrange(pPlot.getY() - 1, pPlot.getY() + 2):
					pAdjacentPlot = CyMap().plot(x, y)
					if x == pPlot.getX() and y == pPlot.getY(): continue
					if pAdjacentPlot.getFeatureType() == iFeature:
						bFound = True
						break
				if bFound: break
			if pAdjacentPlot.isRevealed(iTeam, False): return

		for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
			if pPlot.isRevealed(iTeamX, False) and iTeamX != iTeam:
				bFirst = False
				break
			if sType in self.lBigWonder:
				if pAdjacentPlot.isRevealed(iTeamX, False) and iTeamX != iTeam:
					bFirst = False
					break
		else: bFirst = True
		if bFirst:
			iGold = self.iFirstGold * GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent() /100

		import CvUtil
		TRNSLTR = CyTranslator()
		iPlayerAct = GAME.getActivePlayer()

		for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
			CyPlayerX = GC.getPlayer(iPlayerX)
			iTeamX = CyPlayerX.getTeam()
			if iTeamX != iTeam:
				if bFirst and iPlayerX == iPlayerAct:
					if CyTeam.isHasMet(iTeamX):
						CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_MET_FIRST_WONDER",(GC.getTeam(iTeam).getName(), GC.getFeatureInfo(iFeature).getDescription())), iPlayerX, 12, bForce=False)
					else:
						CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_NOT_MET_FIRST_WONDER",(GC.getFeatureInfo(iFeature).getDescription(),)), iPlayerX, 12, bForce=False)
				continue
			if iPlayerX == iPlayerAct:
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
				popupInfo.setData1(iFeature)
				popupInfo.setData3(3)
				popupInfo.setText("showWonderMovie")
				popupInfo.addPopup(iPlayerX)
				CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_WONDERDISCOVERED_YOU",(FeatureInfo.getDescription(),)), iPlayerX, 12, FeatureInfo.getButton(), ColorTypes(44), pPlot.getX(), pPlot.getY(), True, True, bForce=False)
			if bFirst:
				CyPlayerX.changeGold(iGold)
				if iPlayerX == iPlayerAct:
					CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_FIRST_FOUND_WONDER",(iGold,)), iPlayerX, 12, None, ColorTypes(44), bForce=False)

