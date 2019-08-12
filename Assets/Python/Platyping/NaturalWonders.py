## Sid Meier's Civilization 4
## Copyright Firaxis Games 2007

from CvPythonExtensions import *

gc = CyGlobalContext()
localText = CyTranslator()

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
		for iFeature in xrange(gc.getNumFeatureInfos()):
			FeatureInfo = gc.getFeatureInfo(iFeature)
			sType = FeatureInfo.getType()
			if sType.find("FEATURE_PLATY_") == -1: continue
			if CyGame().getSorenRandNum(100, "Random Plot") >= self.iPlaceChance: continue

			WonderPlot = []
			for i in xrange(CyMap().numPlots()):
				pPlot = CyMap().plotByIndex(i)
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
						if pAdjacentPlot.getFeatureType() > -1 and gc.getFeatureInfo(pAdjacentPlot.getFeatureType()).getType().find("FEATURE_PLATY_") > -1:
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
				iWonderPlot = CyGame().getSorenRandNum(len(WonderPlot), "Random Plot")
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
					pAdjacentPlot = AdjacentPlot[CyGame().getSorenRandNum(len(AdjacentPlot), "Random Plot")]
					pAdjacentPlot.setFeatureType(iFeature, 0)
					# CyGame().setPlotExtraYield(pAdjacentPlot.getX(), pAdjacentPlot.getY(), YieldTypes.YIELD_FOOD, CyGame().getSorenRandNum(self.iMaxYield, "Adds Food"))
					# CyGame().setPlotExtraYield(pAdjacentPlot.getX(), pAdjacentPlot.getY(), YieldTypes.YIELD_PRODUCTION, CyGame().getSorenRandNum(self.iMaxYield, "Adds Production"))
					# CyGame().setPlotExtraYield(pAdjacentPlot.getX(), pAdjacentPlot.getY(), YieldTypes.YIELD_COMMERCE, CyGame().getSorenRandNum(self.iMaxYield, "Adds Commerce"))
					bWonder = True
					break

				## Standard Wonders ##
				bWonder = True
				break

			if bWonder:
				pPlot.setFeatureType(iFeature, 0)
				# CyGame().setPlotExtraYield(pPlot.getX(), pPlot.getY(), YieldTypes.YIELD_FOOD, CyGame().getSorenRandNum(self.iMaxYield, "Adds Food"))
				# CyGame().setPlotExtraYield(pPlot.getX(), pPlot.getY(), YieldTypes.YIELD_PRODUCTION, CyGame().getSorenRandNum(self.iMaxYield, "Adds Production"))
				# CyGame().setPlotExtraYield(pPlot.getX(), pPlot.getY(), YieldTypes.YIELD_COMMERCE, CyGame().getSorenRandNum(self.iMaxYield, "Adds Commerce"))

				for iPlayerX in xrange(gc.getMAX_PLAYERS()):
					pPlayerX = gc.getPlayer(iPlayerX)
					if pPlot.isRevealed(pPlayerX.getTeam(), False):
						self.applyRevealBenefit(pPlayerX)

				sNature = sType[sType.find("_PLATY_") + 7:]
				sBuildingType = "BUILDING_" + sNature
				iBuilding = gc.getInfoTypeForString(sBuildingType)
				if iBuilding == -1: continue
				lNaturalCity = []
				lNaturalCity = self.addSuitableCity(pPlot, -1, lNaturalCity)
				if sType in self.lBigWonder:
					lNaturalCity = self.addSuitableCity(pAdjacentPlot, -1, lNaturalCity)
				if len(lNaturalCity) > 0:
					pLuckyCity = lNaturalCity[CyGame().getSorenRandNum(len(lNaturalCity), "New Natural City")]
					pLuckyCity.setNumRealBuilding(iBuilding, 1)
		return

	# def checkAllRevealTech(self):
		# iRevealTech = -1
		# for iTech in xrange(gc.getNumTechInfos()):
			# TechInfo = gc.getTechInfo(iTech)
			# if TechInfo.isMapVisible():
				# iRevealTech = iTech
				# break
		# if iRevealTech > 0:
			# for iTeamX in xrange(gc.getMAX_PC_TEAMS()):
				# pTeamX = gc.getTeam(iTeamX)
				# if pTeamX.isHasTech(iRevealTech):
					# return True
		# return False

	def checkReveal(self, pPlot, iTeam):
		iFeature = pPlot.getFeatureType()
		if iFeature == -1: return
		pTeam = gc.getTeam(iTeam)
		if pTeam.isNPC(): return
		FeatureInfo = gc.getFeatureInfo(iFeature)
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

		bFirst = True
		for iTeamX in xrange(gc.getMAX_PC_TEAMS()):
			if pPlot.isRevealed(iTeamX, False) and iTeamX != iTeam:
				bFirst = False
				break
			if sType in self.lBigWonder:
				if pAdjacentPlot.isRevealed(iTeamX, False) and iTeamX != iTeam:
					bFirst = False
					break

		for iPlayerX in xrange(gc.getMAX_PC_PLAYERS()):
			pPlayerX = gc.getPlayer(iPlayerX)
			iTeamX = pPlayerX.getTeam()
			if iTeamX == iTeam:
				self.applyRevealBenefit(pPlayerX)
				if pPlayerX.isHuman() and not CyGame().GetWorldBuilderMode():
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popupInfo.setData1(iFeature)
					popupInfo.setData3(3)
					popupInfo.setText(u"showWonderMovie")
					popupInfo.addPopup(iPlayerX)
					CyInterface().addMessage(iPlayerX,True,10,localText.getText("TXT_KEY_WONDERDISCOVERED_YOU",(FeatureInfo.getDescription(),)),'',0, FeatureInfo.getButton(),ColorTypes(44),pPlot.getX(),pPlot.getY(), True,True)
			if bFirst:
				self.applyFirstRevealBenefit(iPlayerX, iTeam, iFeature)
		return

	def applyRevealBenefit(self, pPlayer):
		pPlayer.changeExtraHappiness(self.iDiscoverHappiness)
		return

	def applyFirstRevealBenefit(self, iPlayer, iDiscoverTeam, iFeature):
		pPlayer = gc.getPlayer(iPlayer)
		iTeam = pPlayer.getTeam()
		if iTeam == iDiscoverTeam:
			iGold = self.iFirstGold * gc.getGameSpeedInfo(CyGame().getGameSpeedType()).getUnitTradePercent() /100
			pPlayer.changeGold(iGold)
			CyInterface().addMessage(iPlayer,true,10,localText.getText("TXT_FIRST_FOUND_WONDER",(iGold,)),'',0, '', ColorTypes(44), -1, -1, true,true)
			if CyGame().GetWorldBuilderMode(): return
		if gc.getTeam(iTeam).isHasMet(iDiscoverTeam):
			CyInterface().addMessage(iPlayer,True,10,localText.getText("TXT_MET_FIRST_WONDER",(gc.getTeam(iDiscoverTeam).getName(), gc.getFeatureInfo(iFeature).getDescription())),'',0, '', -1, -1, -1, True,True)
		else:
			CyInterface().addMessage(iPlayer,True,10,localText.getText("TXT_NOT_MET_FIRST_WONDER",(gc.getFeatureInfo(iFeature).getDescription(),)),'',0, '', -1, -1, -1, True,True)
		return

