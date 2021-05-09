## TradeUtil
##
## Utilities for dealing with Trades and TradeData.
##
## Trading Partners
##
##   canTrade(playerOrID, withPlayerOrID)
##     Returns True if <player> can open the trade window with <withPlayer>.
##
##   getTechTradePartners(playerOrID)
##     Returns a list of CyPlayers that can trade technologies with <player>.
##
##   getBonusTradePartners(playerOrID)
##     Returns a list of CyPlayers that can trade bonuses with <player>.
##
##   getGoldTradePartners(playerOrID)
##     Returns a list of CyPlayers that can trade gold with <player>.
##
##   getMapTradePartners(playerOrID)
##     Returns a list of CyPlayers that can trade maps with <player>.
##
##
##   getOpenBordersTradePartners(playerOrID)
##     Returns a list of CyPlayers that can sign an Open Borders agreement with <player>.
##
##   getDefensivePactTradePartners(playerOrID)
##     Returns a list of CyPlayers that can sign a Defensive Pact with <player>.
##
##   getPermanentAllianceTradePartners(playerOrID)
##     Returns a list of CyPlayers that can sign a Permanent Alliance with <player>.
##
##
##   getPeaceTradePartners(playerOrID)
##     Returns a list of CyPlayers that can sign a peace treaty with <player>.
##
##   getVassalTradePartners(playerOrID)
##     Returns a list of CyPlayers that can become a vassal of <player>.
##
##   getCapitulationTradePartners(playerOrID)
##     Returns a list of CyPlayers that can capitulate to <player>.
##
##
##   tradeParters(playerOrID)
##     Iterates over all of <player>'s possible trade partners, yielding each CyPlayer in turn.
##
##   getTradePartnersByPlayer(playerOrID, testFunction, args...)
##     Returns a list of CyPlayers that can trade with <player>.
##
##   getTradePartnersByTeam(playerOrID, testFunction, args...)
##     Returns a list of CyPlayers that can trade with <player>.
##
## Trade Items
##
##   getDesiredBonuses(playerOrID)
##     Returns a set of bonus IDs that <player> can receive in trade.
##
##   getCorporationBonuses(playerOrID)
##     Returns the set of bonus IDs that <player> can receive due to their corporations.
##
##
##   getTradeableBonuses(fromPlayerOrID, toPlayerOrID)
##     Returns two sets of bonus IDs that <fromPlayer> will and won't trade to <toPlayer>.
##
##
## TradeData
##
##   format(player or ID, TradeData(s))
##     Returns a plain text description of the given tradeable item(s).
##
##   Trade(ePlayer, eOtherPlayer)
##     Can be used to create new trades.
##     (not really since implementDeal() not exposed to Python)
##
## Notes
##   - Must be initialized externally by calling init()
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *
import BugUtil

GC = CyGlobalContext()
GAME = GC.getGame()

CORP_BONUSES = {}
TRADE_FORMATS = {}


## Trading Partners

def canTrade(playerX, playerY):
	"""
	Returns True if <playerX> can trade with <playerY>.
	"""
	iPlayerX = playerX.getID()
	iPlayerY = playerY.getID()
	if (iPlayerX == iPlayerY
	or not playerX.isAlive() or playerX.isNPC() or playerX.isMinorCiv()
	or not playerY.isAlive() or playerY.isNPC() or playerY.isMinorCiv()
	):
		return False

	iTeamX = playerX.getTeam()
	iTeamY = playerY.getTeam()
	if iTeamX == iTeamY:
		return True

	CyTeamX = GC.getTeam(iTeamX)
	if not CyTeamX.isHasMet(iTeamY) or CyTeamX.isAtWar(iTeamY):
		return False

	bHumanX = playerX.isHuman()
	bHumanY = playerY.isHuman()
	if bHumanX or bHumanY:
		if bHumanX and bHumanY:
			return True
		if bHumanX:
			return playerY.AI_isWillingToTalk(iPlayerX)

		return playerX.AI_isWillingToTalk(iPlayerY)

	return playerX.AI_isWillingToTalk(iPlayerY) and playerY.AI_isWillingToTalk(iPlayerX)


def getTechTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can trade technologies with <player>.
	"""
	if GAME.isOption(GameOptionTypes.GAMEOPTION_NO_TECH_TRADING):
		return ()
	return getTradePartnersByTeam(playerOrID, lambda fromTeam, toTeam: fromTeam.isTechTrading() or toTeam.isTechTrading())

def getBonusTradePartners(CyPlayer):
	"""
	Returns a list of CyPlayers that can trade bonuses with <player>.
	"""
	return getTradePartnersByPlayer(CyPlayer, lambda fromPlayer, toPlayer: fromPlayer.canTradeNetworkWith(toPlayer.getID()))

def getGoldTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can trade gold with <player>.
	"""
	return getTradePartnersByTeam(playerOrID, lambda fromTeam, toTeam: fromTeam.isGoldTrading() or toTeam.isGoldTrading())

def getMapTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can trade maps with <player>.
	"""
	return getTradePartnersByTeam(playerOrID, lambda fromTeam, toTeam: fromTeam.isMapTrading() or toTeam.isMapTrading())


def getOpenBordersTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can sign an Open Borders agreement with <player>.
	"""
	return getTradePartnersByTeam(playerOrID, canSignOpenBorders)

def canSignOpenBorders(fromTeam, toTeam):
	"""
	Returns True if the two CyTeams can sign an Open Borders agreement.
	"""
	if fromTeam.isOpenBorders(toTeam.getID()) or toTeam.isOpenBorders(fromTeam.getID()):
		return False
	return fromTeam.isOpenBordersTrading() or toTeam.isOpenBordersTrading()
#Afforess
def getLimitedBordersTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can sign an Limited Borders agreement with <player>.
	"""
	return getTradePartnersByTeam(playerOrID, canSignLimitedBorders)

def canSignLimitedBorders(fromTeam, toTeam):
	"""
	Returns True if the two CyTeams can sign an Limited Borders agreement.
	Limited Borders are enabled when Open Borders are, so the functions are identical.
	"""
	if fromTeam.isOpenBorders(toTeam.getID()) or toTeam.isOpenBorders(fromTeam.getID()):
		return False
	if not GAME.isOption(GameOptionTypes.GAMEOPTION_ADVANCED_DIPLOMACY):
		return False
	return fromTeam.isOpenBordersTrading() or toTeam.isOpenBordersTrading()

def getEmbassyTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can sign an Limited Borders agreement with <player>.
	"""
	return getTradePartnersByTeam(playerOrID, canTradeEmbassy)

def canTradeEmbassy(fromTeam, toTeam):
	"""
	Returns True if the two CyTeams can trade for an Embassy.
	Embassies are enabled when Open Borders are, so the functions are identical.
	"""
	if fromTeam.isOpenBorders(toTeam.getID()) or toTeam.isOpenBorders(fromTeam.getID()):
		return False
	if not GAME.isOption(GameOptionTypes.GAMEOPTION_ADVANCED_DIPLOMACY):
		return False
	return fromTeam.isOpenBordersTrading() or toTeam.isOpenBordersTrading()

#Afforess End
def getDefensivePactTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can sign a Defensive Pact with <player>.
	"""
	return getTradePartnersByTeam(playerOrID, canSignDefensivePact)

def canSignDefensivePact(fromTeam, toTeam):
	"""
	Returns True if the two CyTeams can sign a Defensive Pact.
	"""
	if fromTeam.isDefensivePact(toTeam.getID()) or toTeam.isDefensivePact(fromTeam.getID()):
		return False
	return fromTeam.isDefensivePactTrading() or toTeam.isDefensivePactTrading()

def getPermanentAllianceTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can sign a Permanent Alliance with <player>.
	"""
	return getTradePartnersByTeam(playerOrID, canSignPermanentAlliance)

def canSignPermanentAlliance(fromTeam, toTeam):
	"""
	Returns True if the two CyTeams can sign a Permanent Alliance.
	"""
	if fromTeam.getID() == toTeam.getID():
		return False
	return fromTeam.isPermanentAllianceTrading() or toTeam.isPermanentAllianceTrading()


def getPeaceTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can sign a peace treaty with <player>.
	"""
	return getTradePartnersByTeam(playerOrID, lambda fromTeam, toTeam: toTeam.isAtWar(fromTeam.getID()))

def getVassalTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can become a vassal of <player>.
	"""
	return getTradePartnersByTeam(playerOrID, canAcceptVassal, False)

def getCapitulationTradePartners(playerOrID):
	"""
	Returns a list of CyPlayers that can capitulate to <player>.
	"""
	return getTradePartnersByTeam(playerOrID, canAcceptVassal, True)

def canAcceptVassal(masterTeam, vassalTeam, bAtWar):
	"""
	Returns True if <vassalTeam> can become a vassal of <masterTeam>.

	Pass True for <bAtWar> to test for capitulation and False to test for peaceful vassalage.
	"""
	if masterTeam.getID() == vassalTeam.getID():
		return False
	if masterTeam.isAVassal() or vassalTeam.isAVassal():
		return False
	if masterTeam.isAtWar(vassalTeam.getID()) != bAtWar:
		return False
	# master must possess tech
	return masterTeam.isVassalStateTrading()


def tradeParters(player):
	"""
	Iterates over all of <player>'s possible trade partners, yielding each CyPlayer in turn.
	"""
	for iPartner in xrange(GC.getMAX_PC_PLAYERS()):
		partner = GC.getPlayer(iPartner)
		if canTrade(player, partner):
			yield partner

def getTradePartnersByPlayer(player, testFunction, *args):
	"""
	Returns a list of CyPlayers that can trade with <player>.

	<testFunction> is passed two CyPlayers plus <args> for each viable pairing and should return a boolean value.
	"""
	partners = []
	for partner in tradeParters(player):
		if testFunction(player, partner, *args):
			partners.append(partner)
	return partners

def getTradePartnersByTeam(playerOrID, testFunction, *args):
	"""
	Returns a list of CyPlayers that can trade with <player>.

	<testFunction> is passed two CyTeams plus <args> for each viable pairing and should return a boolean value.
	"""
	player = getPlayer(playerOrID)
	team = GC.getTeam(player.getTeam())
	partners = []
	for partner in tradeParters(player):
		if testFunction(team, GC.getTeam(partner.getTeam()), *args):
			partners.append(partner)
	return partners


## Trade Items

def getDesiredBonuses(player, team):
	"""
	Returns a set of bonus IDs that <player> can receive in trade.
	"""
	bonuses = set()
	for eBonus in range(GC.getNumBonusInfos()):
		if player.getNumAvailableBonuses(eBonus) == 0:
			eObsoleteTech = GC.getBonusInfo(eBonus).getTechObsolete()
			if eObsoleteTech == -1 or not team.isHasTech(eObsoleteTech):
				bonuses.add(eBonus)
	return bonuses | getCorporationBonuses(player)

def getCorporationBonuses(playerOrID):
	"""
	Returns the set of bonus IDs that <player> can receive due to their corporations.
	Takes into account anything (e.g. civics) that alters <player>'s ability to run corporations.
	"""
	player = getPlayer(playerOrID)
	bonuses = set()
	for eCorp, inputs in CORP_BONUSES.iteritems():
		if player.getHasCorporationCount(eCorp) > 0:
			bonuses |= inputs
	return bonuses

def initCorporationBonuses():
	'''
	Initializes the CORP_BONUSES dictionary.
	Map corporation ID to the set of bonus IDs it uses.
	'''
	for eCorp in range(GC.getNumCorporationInfos()):
		corp = GC.getCorporationInfo(eCorp)
		bonuses = set()
		for eBonus in corp.getPrereqBonuses():
			bonuses.add(eBonus)
		CORP_BONUSES[eCorp] = bonuses

def getTradeableBonuses(fromPlayer, eToPlayer):
	"""
	Returns two sets of bonus IDs that <fromPlayer> will and won't trade to <toPlayer>.

	Assumes that the two players can trade bonuses.
	"""
	fromPlayerIsHuman = fromPlayer.isHuman()
	will = set()
	wont = set()
	tradeData = TradeData()
	tradeData.ItemType = TradeableItems.TRADE_RESOURCES
	for eBonus in range(GC.getNumBonusInfos()):
		tradeData.iData = eBonus
		if fromPlayer.canTradeItem(eToPlayer, tradeData, False):
			if fromPlayerIsHuman or fromPlayer.canTradeItem(eToPlayer, tradeData, True):
				will.add(eBonus)
			else:
				wont.add(eBonus)
	return will, wont


class Trade(object):
	"""
	Encapsulates the player IDs and TradeData for a new or proposed trade.

	Implements the same interface as the DealUtil.Deal class.
	"""
	def __init__(self, ePlayer, eOtherPlayer):
		self.ePlayer = ePlayer
		self.eOtherPlayer = eOtherPlayer
		self.tradeList = []
		self.otherTradeList = []

	def isReversed(self):
		return False
	def getPlayer(self):
		return self.ePlayer
	def getOtherPlayer(self):
		return self.eOtherPlayer

	def getCount(self):
		return len(self.tradeList)
	def getOtherCount(self):
		return len(self.otherTradeList)
	def getTrade(self, index):
		return self.tradeList[index]
	def getOtherTrade(self, index):
		return self.otherTradeList[index]
	def trades(self):
		return self.tradeList
	def otherTrades(self):
		return self.otherTradeList

	def addTrade(self, trade):
		self.tradeList.append(trade)
	def addOtherTrade(self, trade):
		self.otherTradeList.append(trade)

	def hasType(self, type):
		return self.hasAnyType((type,))
	def hasAnyType(self, types):
		for trade in self.trades():
			if trade.ItemType in types:
				return True
		for trade in self.otherTrades():
			if trade.ItemType in types:
				return True
		return False
	def findType(self, type):
		return self.findTypes((type,))
	def findTypes(self, types):
		found = []
		for trade in self.trades():
			for type in types:
				if type == trade.ItemType:
					found.append(trade)
		for trade in self.otherTrades():
			for type in types:
				if type == trade.ItemType:
					found.append(trade)
		return found

	def __repr__(self):
		return ("<trade %d [%s] for %d [%s]>" %
				(self.getPlayer(),
				format(self.getPlayer(), self.trades()),
				self.getOtherPlayer(),
				format(self.getOtherPlayer(), self.otherTrades())))


## TradeData Formatting

def format(player, trade):
	"""Returns a single string containing all of the trade items separated by commas.

	player can be either an ID or CyPlayer and is needed when a city is being traded.
	"""
	if isinstance(trade, list) or isinstance(trade, tuple) or isinstance(trade, set):
		return ", ".join([format(player, t) for t in trade])

	if trade.ItemType in TRADE_FORMATS:
		return TRADE_FORMATS[trade.ItemType].format(player, trade)

	print "[WARNING] TradeUtil - unknown item type %d" % trade.ItemType
	return ""


def initTradeableItems():
	addSimpleTrade("gold", TradeableItems.TRADE_GOLD, "TXT_KEY_TRADE_GOLD_NUM")
	addSimpleTrade("gold per turn", TradeableItems.TRADE_GOLD_PER_TURN, "TXT_KEY_TRADE_GOLD_PER_TURN_NUM")
	addPlainTrade("map", TradeableItems.TRADE_MAPS, "TXT_KEY_TRADE_WORLD_MAP_STRING")
	addPlainTrade("vassal", TradeableItems.TRADE_VASSAL, "TXT_KEY_TRADE_VASSAL_TREATY_STRING")
	addPlainTrade("capitulation", TradeableItems.TRADE_SURRENDER, "TXT_KEY_TRADE_CAPITULATE_STRING")
	addPlainTrade("open borders", TradeableItems.TRADE_OPEN_BORDERS, "TXT_KEY_TRADE_OPEN_BORDERS_STRING")
	addPlainTrade("defensive pact", TradeableItems.TRADE_DEFENSIVE_PACT, "TXT_KEY_TRADE_DEFENSIVE_PACT_STRING")
	addPlainTrade("alliance", TradeableItems.TRADE_PERMANENT_ALLIANCE, "TXT_KEY_TRADE_PERMANENT_ALLIANCE_STRING")
	addComplexTrade("peace treaty", TradeableItems.TRADE_PEACE_TREATY, getTradePeaceDeal)
	addComplexTrade("technology", TradeableItems.TRADE_TECHNOLOGIES, getTradeTech)
	addComplexTrade("resource", TradeableItems.TRADE_RESOURCES, getTradeBonus)
	addComplexTrade("city", TradeableItems.TRADE_CITIES, getTradeCity)
	addAppendingTrade("peace", TradeableItems.TRADE_PEACE, "TXT_KEY_TRADE_PEACE_WITH", getTradePlayer)
	addAppendingTrade("war", TradeableItems.TRADE_WAR, "TXT_KEY_TRADE_WAR_WITH", getTradePlayer)
	addAppendingTrade("trade embargo", TradeableItems.TRADE_EMBARGO, "TXT_KEY_TRADE_STOP_TRADING_WITH", getTradePlayer, " %s")
	addAppendingTrade("civic", TradeableItems.TRADE_CIVIC, "TXT_KEY_TRADE_ADOPT", getTradeCivic)
	addAppendingTrade("religion", TradeableItems.TRADE_RELIGION, "TXT_KEY_TRADE_CONVERT", getTradeReligion)
	#Afforess
	addPlainTrade("limited borders", TradeableItems.TRADE_RITE_OF_PASSAGE, "TXT_KEY_TRADE_LIMITED_BORDERS_STRING")
	addPlainTrade("embassy", TradeableItems.TRADE_EMBASSY, "TXT_KEY_TRADE_EMBASSY_STRING")
	addComplexTrade("units", TradeableItems.TRADE_MILITARY_UNIT, getTradeUnits)
	addComplexTrade("workers", TradeableItems.TRADE_WORKER, getTradeWorkers)
	addComplexTrade("contact", TradeableItems.TRADE_CONTACT, getTradeContacts)
	#Afforess End

def addPlainTrade(name, type, key):
	"""Creates a trade using an unparameterized XML <text> tag."""
	return addTrade(type, PlainTradeFormat(name, type, key))

def addSimpleTrade(name, type, key):
	"""Creates a trade using an XML <text> tag with a int placeholder for iData."""
	return addTrade(type, SimpleTradeFormat(name, type, key))

def addAppendingTrade(name, type, key, argsFunction, text="%s"):
	"""Creates a trade using an XML <text> tag with a single appended string placeholder."""
	format = addTrade(type, AppendingTradeFormat(name, type, key, text))
	if argsFunction is not None:
		format.getParameters = lambda player, trade: argsFunction(player, trade)
	return format

def addComplexTrade(name, type, argsFunction, textFunction=None):
	"""Creates a trade using an XML <text> tag with any number of placeholders."""
	format = addTrade(type, ComplexTradeFormat(name, type))
	if argsFunction is not None:
		format.getParameters = lambda player, trade: argsFunction(player, trade)
	if textFunction is not None:
		format.getText = lambda player, trade: textFunction(player, trade)
	return format

def addTrade(type, format):
	TRADE_FORMATS[type] = format
	return format


## Functions for use as argsFunction: converting TradeData.iData into
## whatever you want to display in the formatted string.

def getTradeTech(player, trade):
	return GC.getTechInfo(trade.iData).getDescription()

def getTradeBonus(player, trade):
	return GC.getBonusInfo(trade.iData).getDescription()

def getTradeCity(player, trade):
	return getPlayer(player).getCity(trade.iData).getName()

def getTradeCivic(player, trade):
	return GC.getCivicInfo(trade.iData).getDescription()

def getTradeReligion(player, trade):
	return GC.getReligionInfo(trade.iData).getDescription()

def getTradePlayer(player, trade):
	return getPlayer(trade.iData).getName()

def getTradePeaceDeal(player, trade):
	print "TradeUtil - peace treaty has iData %d" % trade.iData
	return BugUtil.getText("TXT_KEY_TRADE_PEACE_TREATY_STRING", (GC.getDefineINT("PEACE_TREATY_LENGTH"),))


def getTradeUnits(player, trade):
	return getPlayer(player).getUnit(trade.iData).getName()

def getTradeWorkers(player, trade):
	return getPlayer(player).getUnit(trade.iData).getName()

def getTradeContacts(player, trade):
	return GC.getTeam(trade.iData).getName()

## Classes for Formatting TradeData

class BaseTradeFormat(object):
	def __init__(self, name, type):
		self.name = name
		self.type = type
	def format(self, player, trade):
		pass

class PlainTradeFormat(BaseTradeFormat):
	def __init__(self, name, type, key):
		super(PlainTradeFormat, self).__init__(name, type)
		self.key = key
	def format(self, player, trade):
		return BugUtil.getPlainText(self.key)

class SimpleTradeFormat(BaseTradeFormat):
	def __init__(self, name, type, key):
		super(SimpleTradeFormat, self).__init__(name, type)
		self.key = key
	def format(self, player, trade):
		return BugUtil.getText(self.key, (self.getParameters(player, trade),))
	def getParameters(self, player, trade):
		return trade.iData

class AppendingTradeFormat(BaseTradeFormat):
	def __init__(self, name, type, key, text="%s"):
		super(AppendingTradeFormat, self).__init__(name, type)
		self.key = key
		self.text = text
	def format(self, player, trade):
		return self.getText(player, trade) % (self.getParameters(player, trade),)
	def getText(self, player, trade):
		return BugUtil.getPlainText(self.key) + self.text
	def getParameters(self, player, trade):
		return trade.iData

class ComplexTradeFormat(BaseTradeFormat):
	def __init__(self, name, type):
		super(ComplexTradeFormat, self).__init__(name, type)
	def format(self, player, trade):
		return self.getText(player, trade) % (self.getParameters(player, trade),)
	def getText(self, player, trade):
		return "%s"
	def getParameters(self, player, trade):
		return trade.iData

## Initialization
def init():
	initCorporationBonuses()
	initTradeableItems()


def getPlayer(playerOrID):
	"""Returns the CyPlayer for the given player."""
	if playerOrID is None or playerOrID == -1:
		return None
	if isinstance(playerOrID, int):
		return GC.getPlayer(playerOrID)
	return playerOrID