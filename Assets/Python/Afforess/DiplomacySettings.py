## A New Dawn Mod Code

from CvPythonExtensions import *
import BugCore

GC = CyGlobalContext()
ANewDawnOpt = BugCore.game.DiplomacySettings
GAME = GC.getGame()

class DiplomacySettings:
	def __init__(self, eventManager):
		eventManager.addEventHandler("OnLoad", self.onLoadGame)
		eventManager.addEventHandler("GameStart", self.onGameStart)

	def onLoadGame(self,argsList):
		self.optionUpdate()

	def onGameStart(self,argsList):
		self.optionUpdate()

	def optionUpdate(self):
		if BugCore.game.RoMSettings.isRoMReset():
			resetOptions()
		else:
			setXMLOptionsfromIniFile()

def changedCanTradeTechs(option, value):
	GAME.setOption(GameOptionTypes.GAMEOPTION_NO_TECH_TRADING, not ANewDawnOpt.isCanTradeTechs())
def changedCanBrokerTechs(option, value):
	GAME.setOption(GameOptionTypes.GAMEOPTION_NO_TECH_BROKERING, not ANewDawnOpt.isCanBrokerTechs())
def changedCanTradeResources(option, value):
	GC.setDefineINT("CAN_TRADE_RESOURCES", ANewDawnOpt.isCanTradeResources())
def changedCanTradeCities(option, value):
	GC.setDefineINT("CAN_TRADE_CITIES", ANewDawnOpt.isCanTradeCities())
def changedCanTradeWorkers(option, value):
	GC.setDefineINT("CAN_TRADE_WORKERS", ANewDawnOpt.isCanTradeWorkers())
def changedCanTradeMilitary(option, value):
	GC.setDefineINT("NO_MILITARY_UNIT_TRADING", not ANewDawnOpt.isCanTradeMilitary())
def changedCanTradeGold(option, value):
	GC.setDefineINT("CAN_TRADE_GOLD", ANewDawnOpt.isCanTradeGold())
def changedCanTradeGoldPerTurn(option, value):
	GC.setDefineINT("CAN_TRADE_GOLD_PER_TURN", ANewDawnOpt.isCanTradeGoldPerTurn())
def changedCanTradeMaps(option, value):
	GC.setDefineINT("CAN_TRADE_MAPS", ANewDawnOpt.isCanTradeMaps())
def changedCanTradeVassals(option, value):
	GAME.setOption(GameOptionTypes.GAMEOPTION_NO_VASSAL_STATES, not ANewDawnOpt.isCanTradeVassals())
def changedCanTradeEmbassies(option, value):
	GC.setDefineINT("CAN_TRADE_EMBASSIES", ANewDawnOpt.isCanTradeEmbassies())
def changedCanTradeContact(option, value):
	GC.setDefineINT("CAN_TRADE_CONTACT", ANewDawnOpt.isCanTradeContact())
def changedCanTradeCorporations(option, value):
	GC.setDefineINT("CAN_TRADE_CORPORATIONS", ANewDawnOpt.isCanTradeCorporations())
def changedCanTradePeace(option, value):
	GC.setDefineINT("CAN_TRADE_PEACE", ANewDawnOpt.isCanTradePeace())
def changedCanTradeWar(option, value):
	GC.setDefineINT("CAN_TRADE_WAR", ANewDawnOpt.isCanTradeWar())
def changedCanTradeEmbargo(option, value):
	GC.setDefineINT("CAN_TRADE_EMBARGO", ANewDawnOpt.isCanTradeEmbargo())
def changedCanTradeCivics(option, value):
	GC.setDefineINT("CAN_TRADE_CIVICS", ANewDawnOpt.isCanTradeCivics())
def changedCanTradeReligions(option, value):
	GC.setDefineINT("CAN_TRADE_RELIGIONS", ANewDawnOpt.isCanTradeReligions())
def changedCanTradeOpenBorders(option, value):
	GC.setDefineINT("CAN_TRADE_OPEN_BORDERS", ANewDawnOpt.isCanTradeOpenBorders())
def changedCanTradeLimitedBorders(option, value):
	GC.setDefineINT("CAN_TRADE_LIMITED_BORDERS", ANewDawnOpt.isCanTradeLimitedBorders())
def changedCanTradeDefensivePact(option, value):
	GC.setDefineINT("CAN_TRADE_DEFENSIVE_PACT", ANewDawnOpt.isCanTradeDefensivePact())
def changedCanTradeAlliance(option, value):
	GAME.setOption(GameOptionTypes.GAMEOPTION_ENABLE_PERMANENT_ALLIANCES, ANewDawnOpt.isCanTradeAlliance())
def changedAdvancedDiplomacy(option, value):
	GAME.setOption(GameOptionTypes.GAMEOPTION_ADVANCED_DIPLOMACY, ANewDawnOpt.isAdvancedDiplomacy())


def setXMLOptionsfromIniFile():
	print "DiplomacySettings.setXMLOptionsfromIniFile"

	ANewDawnOpt.setCanTradeTechs(not GAME.isOption(GameOptionTypes.GAMEOPTION_NO_TECH_TRADING))
	ANewDawnOpt.setCanBrokerTechs(not GAME.isOption(GameOptionTypes.GAMEOPTION_NO_TECH_BROKERING))
	GC.setDefineINT("CAN_TRADE_RESOURCES", ANewDawnOpt.isCanTradeResources())
	GC.setDefineINT("CAN_TRADE_CITIES", ANewDawnOpt.isCanTradeCities())
	GC.setDefineINT("CAN_TRADE_WORKERS", ANewDawnOpt.isCanTradeWorkers())
	GC.setDefineINT("NO_MILITARY_UNIT_TRADING", not ANewDawnOpt.isCanTradeMilitary())
	GC.setDefineINT("CAN_TRADE_GOLD", ANewDawnOpt.isCanTradeGold())
	GC.setDefineINT("CAN_TRADE_GOLD_PER_TURN", ANewDawnOpt.isCanTradeGoldPerTurn())
	GC.setDefineINT("CAN_TRADE_MAPS", ANewDawnOpt.isCanTradeMaps())
	ANewDawnOpt.setCanTradeVassals(not GAME.isOption(GameOptionTypes.GAMEOPTION_NO_VASSAL_STATES))
	GC.setDefineINT("CAN_TRADE_EMBASSIES", ANewDawnOpt.isCanTradeEmbassies())
	GC.setDefineINT("CAN_TRADE_CONTACT", ANewDawnOpt.isCanTradeContact())
	GC.setDefineINT("CAN_TRADE_CORPORATIONS", ANewDawnOpt.isCanTradeCorporations())
	GC.setDefineINT("CAN_TRADE_PEACE", ANewDawnOpt.isCanTradePeace())
	GC.setDefineINT("CAN_TRADE_WAR", ANewDawnOpt.isCanTradeWar())
	GC.setDefineINT("CAN_TRADE_EMBARGO", ANewDawnOpt.isCanTradeEmbargo())
	GC.setDefineINT("CAN_TRADE_CIVICS", ANewDawnOpt.isCanTradeCivics())
	GC.setDefineINT("CAN_TRADE_RELIGIONS", ANewDawnOpt.isCanTradeReligions())
	GC.setDefineINT("CAN_TRADE_OPEN_BORDERS", ANewDawnOpt.isCanTradeOpenBorders())
	GC.setDefineINT("CAN_TRADE_LIMITED_BORDERS", ANewDawnOpt.isCanTradeLimitedBorders())
	GC.setDefineINT("CAN_TRADE_DEFENSIVE_PACT", ANewDawnOpt.isCanTradeDefensivePact())
	ANewDawnOpt.setCanTradeAlliance(GAME.isOption(GameOptionTypes.GAMEOPTION_ENABLE_PERMANENT_ALLIANCES))
	ANewDawnOpt.setAdvancedDiplomacy(GAME.isOption(GameOptionTypes.GAMEOPTION_ADVANCED_DIPLOMACY))

def resetOptions():
	import BugOptions
	ANewDawnoptions = BugOptions.getOptions("DiplomacySettings").options
	for option in ANewDawnoptions:
		option.resetValue()
	setXMLOptionsfromIniFile()
	BugCore.game.RoMSettings.setRoMReset(False)