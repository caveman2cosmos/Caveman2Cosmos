## MoreCiv4lerts
## From HOF MOD V1.61.001
## Based upon Gillmer J. Derge's Civ4lerts.py

from CvPythonExtensions import *
import CvUtil
import TradeUtil

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

EVENT_MESSAGE_TIME_LONG = GC.getDefineINT("EVENT_MESSAGE_TIME_LONG")

class MoreCiv4lerts:

	def __init__(self, eventManager):
		## Init event handlers
		MoreCiv4lertsEvent(eventManager)

class AbstractMoreCiv4lertsEvent(object):

	def __init__(self, eventManager, *args, **kwargs):
		super(AbstractMoreCiv4lertsEvent, self).__init__(*args, **kwargs)

	def _addMessageNoIcon(self, iPlayer, msg, iColor=-1):
		#Displays an on-screen message with no popup icon.
		self._addMessage(iPlayer, msg, None, -1, -1, False, False, iColor)

	def _addMessage(self, iPlayer, msg, icon, iX, iY, bOffArrow, bOnArrow, iColor):
		# Route it through the centralized handler
		CvUtil.sendMessage(msg, iPlayer, EVENT_MESSAGE_TIME_LONG, icon, ColorTypes(iColor), iX, iY, bOffArrow, bOnArrow)

class MoreCiv4lertsEvent(AbstractMoreCiv4lertsEvent):

	def __init__(self, eventManager, *args, **kwargs):
		super(MoreCiv4lertsEvent, self).__init__(eventManager, *args, **kwargs)

		eventManager.addEventHandler("BeginActivePlayerTurn", self.onBeginActivePlayerTurn)
		eventManager.addEventHandler("cityAcquiredAndKept", self.onCityAcquiredAndKept)
		eventManager.addEventHandler("cityBuilt", self.OnCityBuilt)
		eventManager.addEventHandler("cityRazed", self.OnCityRazed)
		eventManager.addEventHandler("cityLost", self.OnCityLost)
		eventManager.addEventHandler("GameStart", self.reset)
		eventManager.addEventHandler("OnLoad", self.reset)

		self.eventMgr = eventManager
		import BugCore
		self.options = BugCore.game.MoreCiv4lerts
		self.reset()

	def reset(self, argsList=None):
		self.CurrAvailTechTrades = {}
		self.PrevAvailTechTrades = {}
		self.PrevAvailBonusTrades = {}
		self.PrevAvailOpenBordersTrades = set()
		self.PrevAvailMapTrades = set()
		self.PrevAvailDefensivePactTrades = set()
		self.PrevAvailPermanentAllianceTrades = set()
		self.PrevAvailVassalTrades = set()
		self.PrevAvailSurrenderTrades = set()
		self.PrevAvailPeaceTrades = set()
		self.lastPopCount = 0
		self.lastLandCount = 0

	def getCheckForDomVictory(self):
		return self.options.isShowDomPopAlert() or self.options.isShowDomLandAlert()

	def onBeginActivePlayerTurn(self, argsList):
		"Called when the active player can start making their moves."
		iGameTurn = argsList[0]
		iPlayer = GAME.getActivePlayer()
		self.CheckForAlerts(iPlayer, True)

	def onCityAcquiredAndKept(self, argsList):
		#iOwnerOld, iOwnerNew, city, bConquest, bTrade = argsList
		if not self.getCheckForDomVictory():
			return
		if argsList[1] == GAME.getActivePlayer():
			self.CheckForAlerts(argsList[1], False)

	def OnCityBuilt(self, argsList):
		CyCity = argsList[0]
		iOwner = CyCity.getOwner()
		iPlayer = GAME.getActivePlayer()
		if self.getCheckForDomVictory():
			if iOwner == iPlayer:
				self.CheckForAlerts(iOwner, False)
		if self.options.isShowCityFoundedAlert():
			if iOwner != iPlayer:
				bRevealed = CyCity.isRevealed(GC.getActivePlayer().getTeam(), False)
				CyPlayer = GC.getPlayer(iOwner)
				if bRevealed or canSeeCityList(CyPlayer):
					iColor = GC.getInfoTypeForString("COLOR_MAGENTA")
					if bRevealed:
						msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_CITY_FOUNDED", (CyPlayer.getName(), CyCity.getName()))
						icon = "Art/Interface/Buttons/Actions/foundcity.dds"
						CvUtil.sendMessage(msg, iPlayer, EVENT_MESSAGE_TIME_LONG, icon, ColorTypes(iColor), CyCity.getX(), CyCity.getY(), True, True)
					else:
						msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_CITY_FOUNDED_UNSEEN", (CyPlayer.getName(), CyCity.getName()))
						self._addMessageNoIcon(iPlayer, msg, iColor)

	def OnCityRazed(self, argsList):
		city, iPlayer = argsList
		if not self.getCheckForDomVictory(): return
		if iPlayer == GAME.getActivePlayer():
			self.CheckForAlerts(iPlayer, False)

	def OnCityLost(self, argsList):
		city = argsList[0]
		if not self.getCheckForDomVictory() or city.getOwner() != GAME.getActivePlayer():
			return
		self.CheckForAlerts(city.getOwner(), False)

	def CheckForAlerts(self, iPlayer, bBeginTurn):
		CyPlayer = GC.getPlayer(iPlayer)
		CyTeam = GC.getTeam(CyPlayer.getTeam())
		iGrowthCount = 0

		bCheck1 = self.options.isShowDomPopAlert()
		bCheck2 = bBeginTurn and self.options.isShowCityPendingExpandBorderAlert()

		if bCheck1 or bCheck2:
			# Check for cultural expansion and population growth
			icon = "Art/Interface/Buttons/General/Warning_popup.dds"
			iActiveTeam = GAME.getActiveTeam()
			for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if not CyPlayerX.isAlive() or CyPlayerX.getTeam() != iActiveTeam:
					continue
				for cityX in CyPlayerX.cities():
					if cityX.getFoodTurnsLeft() == 1 and not cityX.isFoodProduction() and not cityX.AI_isEmphasize(5):
						iGrowthCount += 1
					if bCheck2 and cityX.getCultureLevel() != GC.getNumCultureLevelInfos() - 1:
						if cityX.getCulture(iPlayerX) + cityX.getCommerceRate(CommerceTypes.COMMERCE_CULTURE) >= cityX.getCultureThreshold():
							msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_CITY_TO_EXPAND",(cityX.getName(),))
							CvUtil.sendMessage(msg, iPlayer, EVENT_MESSAGE_TIME_LONG, icon, -1, cityX.getX(), cityX.getY(), True, True)

		# Check Domination Limit
		if self.getCheckForDomVictory() and GAME.isVictoryValid(3):
			# Population Limit
			if bCheck1 and iGrowthCount:
				iTotalPop = GAME.getTotalPopulation()
				if iTotalPop > 10:
					iTeamPop = CyTeam.getTotalPopulation()
					fPercent = iTeamPop * 100.0 / iTotalPop
					fPercentNext = (iTeamPop + iGrowthCount) * 100.0 / iTotalPop

					if iTeamPop + iGrowthCount != self.lastPopCount:
						fVictoryPercent = GAME.getAdjustedPopulationPercent(3) * 1.0
						iLimitPop = int(iTotalPop * fVictoryPercent / 100)

						if fPercent >= fVictoryPercent:
							msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_POP_EXCEEDS_LIMIT", (iTeamPop, (u"%.2f%%" % fPercent), iLimitPop, (u"%.2f%%" % fVictoryPercent)))
							self._addMessageNoIcon(iPlayer, msg)

						elif fPercentNext >= fVictoryPercent:
							msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_POP_GROWTH_EXCEEDS_LIMIT", (iTeamPop, iGrowthCount, (u"%.2f%%" % fPercentNext), iLimitPop, (u"%.2f%%" % fVictoryPercent)))
							self._addMessageNoIcon(iPlayer, msg)

						elif fVictoryPercent - fPercentNext < self.options.getDomPopThreshold():
							msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_POP_GROWTH_CLOSE_TO_LIMIT", (iTeamPop, iGrowthCount, (u"%.2f%%" % fPercentNext), iLimitPop, (u"%.2f%%" % fVictoryPercent)))
							self._addMessageNoIcon(iPlayer, msg)

						elif fVictoryPercent - fPercent < self.options.getDomPopThreshold():
							msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_POP_CLOSE_TO_LIMIT", (iTeamPop, (u"%.2f%%" % fPercent), iLimitPop, (u"%.2f%%" % fVictoryPercent)))
							self._addMessageNoIcon(iPlayer, msg)

						self.lastPopCount = iTeamPop + iGrowthCount
			# Land Limit
			if self.options.isShowDomLandAlert():
				iTeamLand = CyTeam.getTotalLand()
				if iTeamLand > 40 and iTeamLand != self.lastLandCount:
					iTotalLand = GC.getMap().getLandPlots()
					fVictoryPercent = GAME.getAdjustedLandPercent(3) * 1.0
					iLimitLand = int(iTotalLand * fVictoryPercent / 100)
					fPercent = (iTeamLand * 100.0) / iTotalLand

					if fPercent > fVictoryPercent:
						msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_LAND_EXCEEDS_LIMIT", (iTeamLand, (u"%.2f%%" % fPercent), iLimitLand, (u"%.2f%%" % fVictoryPercent)))
						self._addMessageNoIcon(iPlayer, msg)

					elif fVictoryPercent - fPercent < self.options.getDomLandThreshold():
						msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_LAND_CLOSE_TO_LIMIT", (iTeamLand, (u"%.2f%%" % fPercent), iLimitLand, (u"%.2f%%" % fVictoryPercent)))
						self._addMessageNoIcon(iPlayer, msg)

					self.lastLandCount = iTeamLand

		if not bBeginTurn: return
		#********#
		# Trades #
		# | || | #
		tradeData = TradeData()
		# Bonus
		if self.options.isShowBonusTradeAlert():
			desiredBonuses = TradeUtil.getDesiredBonuses(CyPlayer, CyTeam)
			tradesByPlayer = {}
			for CyPlayerX in TradeUtil.getBonusTradePartners(CyPlayer):
				will, wont = TradeUtil.getTradeableBonuses(CyPlayerX, iPlayer)
				tradesByPlayer[CyPlayerX.getID()] = will

			for iLoopPlayer, currentTrades in tradesByPlayer.iteritems():
				#Did he have trades avail last turn
				if self.PrevAvailBonusTrades.has_key(iLoopPlayer):
					previousTrades = self.PrevAvailBonusTrades[iLoopPlayer]
				else:
					previousTrades = set()
				#Determine new bonuses
				newTrades = currentTrades.difference(previousTrades).intersection(desiredBonuses)
				if newTrades:
					szNewTrades = self.buildBonusString(newTrades)
					msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_NEW_BONUS_AVAIL", (GC.getPlayer(iLoopPlayer).getName(), szNewTrades))
					self._addMessageNoIcon(iPlayer, msg)
				#Determine removed bonuses
				removedTrades = previousTrades.difference(currentTrades).intersection(desiredBonuses)
				if removedTrades:
					szRemovedTrades = self.buildBonusString(removedTrades)
					msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_BONUS_NOT_AVAIL", (GC.getPlayer(iLoopPlayer).getName(), szRemovedTrades))
					self._addMessageNoIcon(iPlayer, msg)
			#save curr trades for next time
			self.PrevAvailBonusTrades = tradesByPlayer
		# Tech
		if self.options.isShowTechTradeAlert():
			techsByPlayer = {}
			researchTechs = set()
			iTotalTechs = GC.getNumTechInfos()
			tradeData.ItemType = TradeableItems.TRADE_TECHNOLOGIES
			bCheck1 = True
			for CyPlayerX in TradeUtil.getTechTradePartners(CyPlayer):
				techsToTrade = set()
				for iTech in range(iTotalTechs):
					if bCheck1 and CyPlayer.canResearch(iTech):
						researchTechs.add(iTech)
					tradeData.iData = iTech
					if CyPlayerX.canTradeItem(iPlayer, tradeData, False):
						if CyPlayerX.getTradeDenial(iPlayer, tradeData) == DenialTypes.NO_DENIAL: # will trade
							techsToTrade.add(iTech)
				bCheck1 = False
				techsByPlayer[CyPlayerX.getID()] = techsToTrade

			for iLoopPlayer, currentTechs in techsByPlayer.iteritems():
				#Did he have trades avail last turn
				if self.PrevAvailTechTrades.has_key(iLoopPlayer):
					previousTechs = self.PrevAvailTechTrades[iLoopPlayer]
				else:
					previousTechs = set()
				#Determine new techs
				newTechs = currentTechs.difference(previousTechs).intersection(researchTechs)
				if newTechs:
					szNewTechs = self.buildTechString(newTechs)
					msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_NEW_TECH_AVAIL", (GC.getPlayer(iLoopPlayer).getName(), szNewTechs))
					self._addMessageNoIcon(iPlayer, msg)
				#Determine removed techs
				removedTechs = previousTechs.difference(currentTechs).intersection(researchTechs)
				if removedTechs:
					szRemovedTechs = self.buildTechString(removedTechs)
					msg = TRNSLTR.getText("TXT_KEY_MORECIV4LERTS_TECH_NOT_AVAIL", (GC.getPlayer(iLoopPlayer).getName(), szRemovedTechs))
					self._addMessageNoIcon(iPlayer, msg)
			#save curr trades for next time
			self.PrevAvailTechTrades = techsByPlayer
		# Map
		if self.options.isShowMapTradeAlert():
			tradeData.ItemType = TradeableItems.TRADE_MAPS
			oldSet = self.PrevAvailMapTrades
			TXT_KEY = "TXT_KEY_MORECIV4LERTS_MAP"
			willTrade = self.getTrades(TradeUtil.getMapTradePartners(CyPlayer), iPlayer, tradeData, oldSet, TXT_KEY)
			if willTrade != oldSet:
				self.PrevAvailMapTrades = willTrade
		# Open Borders
		if self.options.isShowOpenBordersTradeAlert():
			tradeData.ItemType = TradeableItems.TRADE_OPEN_BORDERS
			oldSet = self.PrevAvailOpenBordersTrades
			TXT_KEY = "TXT_KEY_MORECIV4LERTS_OPEN_BORDERS"
			willTrade = self.getTrades(TradeUtil.getOpenBordersTradePartners(CyPlayer), iPlayer, tradeData, oldSet, TXT_KEY)
			if willTrade != oldSet:
				self.PrevAvailOpenBordersTrades = willTrade
		# Defensive Pact
		if self.options.isShowDefensivePactTradeAlert():
			tradeData.ItemType = TradeableItems.TRADE_DEFENSIVE_PACT
			oldSet = self.PrevAvailDefensivePactTrades
			TXT_KEY = "TXT_KEY_MORECIV4LERTS_DEFENSIVE_PACT"
			willTrade = self.getTrades(TradeUtil.getDefensivePactTradePartners(CyPlayer), iPlayer, tradeData, oldSet, TXT_KEY)
			if willTrade != oldSet:
				self.PrevAvailDefensivePactTrades = willTrade
		# Alliance
		if self.options.isShowPermanentAllianceTradeAlert():
			tradeData.ItemType = TradeableItems.TRADE_PERMANENT_ALLIANCE
			oldSet = self.PrevAvailPermanentAllianceTrades
			TXT_KEY = "TXT_KEY_MORECIV4LERTS_PERMANENT_ALLIANCE"
			willTrade = self.getTrades(TradeUtil.getPermanentAllianceTradePartners(CyPlayer), iPlayer, tradeData, oldSet, TXT_KEY)
			if willTrade != oldSet:
				self.PrevAvailPermanentAllianceTrades = willTrade
		# Vassalage
		if self.options.isShowVassalTradeAlert():
			tradeData.ItemType = TradeableItems.TRADE_VASSAL
			oldSet = self.PrevAvailVassalTrades
			TXT_KEY = "TXT_KEY_MORECIV4LERTS_VASSAL"
			willTrade = self.getTrades(TradeUtil.getVassalTradePartners(CyPlayer), iPlayer, tradeData, oldSet, TXT_KEY)
			if willTrade != oldSet:
				self.PrevAvailVassalTrades = willTrade
		# Capitulate
		if self.options.isShowSurrenderTradeAlert():
			tradeData.ItemType = TradeableItems.TRADE_SURRENDER
			oldSet = self.PrevAvailSurrenderTrades
			TXT_KEY = "TXT_KEY_MORECIV4LERTS_SURRENDER"
			willTrade = self.getTrades(TradeUtil.getCapitulationTradePartners(CyPlayer), iPlayer, tradeData, oldSet, TXT_KEY)
			if willTrade != oldSet:
				self.PrevAvailSurrenderTrades = willTrade
		# Peace Treaty
		if self.options.isShowPeaceTradeAlert():
			tradeData.ItemType = TradeableItems.TRADE_PEACE_TREATY
			tradeData.iData = GC.getDefineINT("PEACE_TREATY_LENGTH")
			oldSet = self.PrevAvailPeaceTrades
			TXT_KEY = "TXT_KEY_MORECIV4LERTS_PEACE_TREATY"
			willTrade = self.getTrades(TradeUtil.getPeaceTradePartners(CyPlayer), iPlayer, tradeData, oldSet, TXT_KEY)
			if willTrade != oldSet:
				self.PrevAvailPeaceTrades = willTrade

	def getTrades(self, aList, iPlayer, tradeData, oldSet, TXT_KEY):
		aSet = set()
		for CyPlayerX in aList:
			if CyPlayerX.canTradeItem(iPlayer, tradeData, False):
				if CyPlayerX.getTradeDenial(iPlayer, tradeData) == DenialTypes.NO_DENIAL:
					aSet.add(CyPlayerX.getID())
		newSet = aSet.difference(oldSet)
		if newSet:
			self._addMessageNoIcon(iPlayer, TRNSLTR.getText(TXT_KEY, (self.buildPlayerString(newSet),)))
		return aSet

	def buildTechString(self, techs):
		return self.buildItemString(techs, GC.getTechInfo, CvTechInfo.getDescription)

	def buildBonusString(self, bonuses):
		return self.buildItemString(bonuses, GC.getBonusInfo, CvBonusInfo.getDescription)

	def buildPlayerString(self, players):
		return self.buildItemString(players, GC.getPlayer, CyPlayer.getName)

	def buildItemString(self, items, getItemFunc, getNameFunc):
		names = [getNameFunc(getItemFunc(eItem)) for eItem in items]
		names.sort()
		return u", ".join(names)

def canSeeCityList(askedPlayer):
	"""
	Returns True if the active player can see the list of <player>'s cities.

	In the unmodified game, this is possible if the players have met and <player>
	is not a vassal of a rival. They must be able to contact (trade with)
	<player>, and OCC must be disabled. You can always see a teammate's cities.
	"""
	if GAME.isOption(GameOptionTypes.GAMEOPTION_ONE_CITY_CHALLENGE):
		return False
	iAskedTeam = askedPlayer.getTeam()
	iAskingTeam = GAME.getActiveTeam()
	if iAskingTeam == iAskedTeam:
		return True

	askedTeam = GC.getTeam(iAskedTeam)
	if askedTeam.isAVassal() and not askedTeam.isVassal(iAskingTeam):
		return False

	return TradeUtil.canTrade(GC.getActivePlayer(), askedPlayer)