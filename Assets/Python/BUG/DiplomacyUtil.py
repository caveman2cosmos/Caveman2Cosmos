## DiplomacyUtil
##
## Utilities for handling and dispatching Diplomacy events and acquiring
## proposed trades from the (unmoddable) CyDiplomacy screen.
##
## Contacting Rivals
##
##   canContact(playerOrID, toPlayerOrID)
##     Returns True if <player> can attempt to contact <toPlayer> given game settings,
##     initial contact, and war-time situation.
##
## TODO: switch to init()
##
## Notes
##   - Must be initialized externally by calling addEvents(eventManager)
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *
import TradeUtil

GC = CyGlobalContext()
diplo = CyDiplomacy()
GAME = GC.getGame()

# comment-type -> ( event-type , trade-type )
g_eventsByCommentType = {}
g_eventManager = None

## Event Initialization

def addEvents(eventManager):
	"""Adds the diplomacy events to BugEventManager."""
	global g_eventManager
	g_eventManager = eventManager

	# Trade
	DiploEvent("AI_DIPLOCOMMENT_OFFER_DEAL", "DealOffered", onDealOffered, sendTrade=True)
	DiploEvent("AI_DIPLOCOMMENT_CANCEL_DEAL", "DealCanceled", onDealCanceled, sendTrade=True)
	DiploEvent("USER_DIPLOCOMMENT_ACCEPT_OFFER", "DealAccepted", onDealAccepted, sendTrade=True)
	DiploEvent("USER_DIPLOCOMMENT_REJECT_OFFER", "DealRejected", onDealRejected, sendTrade=True)

	# Free Stuff
	DiploEvent("AI_DIPLOCOMMENT_OFFER_CITY", "CityOffered", onCityOffered, tradeType=TradeableItems.TRADE_CITIES)
	DiploEvent("AI_DIPLOCOMMENT_GIVE_HELP", "HelpOffered", onHelpOffered, sendTrade=True)
	DiploEvent("AI_DIPLOCOMMENT_OFFER_PEACE", "PeaceOffered", onPeaceOffered)
	DiploEvent("AI_DIPLOCOMMENT_OFFER_VASSAL", "VassalOffered", onVassalOffered)

	# Ask for Help
	DiploEvent("AI_DIPLOCOMMENT_ASK_FOR_HELP", "HelpDemanded", onHelpDemanded, sendTrade=True)
	DiploEvent("USER_DIPLOCOMMENT_GIVE_HELP", "HelpAccepted", onHelpAccepted, sendTrade=True)
	DiploEvent("USER_DIPLOCOMMENT_REFUSE_HELP", "HelpRejected", onHelpRejected, sendTrade=True)

	# Demand Tribute
	DiploEvent("AI_DIPLOCOMMENT_DEMAND_TRIBUTE", "TributeDemanded", onTributeDemanded, sendTrade=True)
	DiploEvent("USER_DIPLOCOMMENT_ACCEPT_DEMAND", "TributeAccepted", onTributeAccepted, sendTrade=True)
	DiploEvent("USER_DIPLOCOMMENT_REJECT_DEMAND", "TributeRejected", onTributeRejected, sendTrade=True)

	# Religion
	DiploEvent("AI_DIPLOCOMMENT_RELIGION_PRESSURE", "ReligionDemanded", onReligionDemanded)
	DiploEvent("USER_DIPLOCOMMENT_CONVERT", "ReligionAccepted", onReligionAccepted)
	DiploEvent("USER_DIPLOCOMMENT_NO_CONVERT", "ReligionRejected", onReligionRejected)

	# Civic
	DiploEvent("AI_DIPLOCOMMENT_CIVIC_PRESSURE", "CivicDemanded", onCivicDemanded)
	DiploEvent("USER_DIPLOCOMMENT_REVOLUTION", "CivicAccepted", onCivicAccepted)
	DiploEvent("USER_DIPLOCOMMENT_NO_REVOLUTION", "CivicRejected", onCivicRejected)

	# Join War
	DiploEvent("AI_DIPLOCOMMENT_JOIN_WAR", "WarDemanded", onWarDemanded, sendData=True)
	DiploEvent("USER_DIPLOCOMMENT_JOIN_WAR", "WarAccepted", onWarAccepted, sendData=True)
	DiploEvent("USER_DIPLOCOMMENT_NO_JOIN_WAR", "WarRejected", onWarRejected, sendData=True)

	# Trade Embargo
	DiploEvent("AI_DIPLOCOMMENT_STOP_TRADING", "EmbargoDemanded", onEmbargoDemanded, sendData=True)
	DiploEvent("USER_DIPLOCOMMENT_STOP_TRADING", "EmbargoAccepted", onEmbargoAccepted, sendData=True)
	DiploEvent("USER_DIPLOCOMMENT_NO_STOP_TRADING", "EmbargoRejected", onEmbargoRejected, sendData=True)


class DiploEvent:
	def __init__(self, comment, event, handler=None, sendFromPlayer=True, sendToPlayer=True,
		sendData=False, sendArgs=False, argFunc=None, sendTrade=False, tradeType=None
	):
		self.comment = comment
		self.eComment = GC.getInfoTypeForString(comment)
		if self.eComment == -1:
			print "[ERR] invalid comment type " + comment
			return
		self.event = event
		self.sendFromPlayer = sendFromPlayer
		self.sendToPlayer = sendToPlayer
		self.sendData = sendData
		self.sendArgs = sendArgs
		self.argFunc = argFunc
		self.sendTrade = sendTrade
		self.tradeType = tradeType
		if tradeType:
			print "DiplomacyUtil - mapped %s to %s with %s" %(comment, event, str(tradeType))
		else:
			print "DiplomacyUtil - mapped %s to %s" %(comment, event)
		g_eventsByCommentType[self.eComment] = self
		g_eventManager.addEventHandler(event, handler)

	def dispatch(self, eFromPlayer, eToPlayer, args):
		data = diplo.getData()
		print "DiplomacyUtil - %s [%d] from %d to %d with %r" %(self.comment, data, eFromPlayer, eToPlayer, args)
		argList = []
		if self.sendFromPlayer:
			argList.append(eFromPlayer)
		if self.sendToPlayer:
			argList.append(eToPlayer)

		if self.argFunc:
			argList.extend(self.argFunc(eFromPlayer, eToPlayer, args, data))
			print "DiplomacyUtil - firing " + self.event
		else:
			if self.sendData:
				argList.append(data)
			if self.sendArgs:
				argList.append(args)

			if self.sendTrade or self.tradeType:
				trade = getProposedTrade()
				if self.sendTrade:
					argList.append(trade)
				if self.tradeType:
					trades = trade.findType(self.tradeType)
					if trade and trades:
						iData = trades[0].iData
						print "DiplomacyUtil - firing %s with %s %d" %(self.event, str(self.tradeType), iData)
						argList.append(iData)
					else:
						print "DiplomacyUtil - firing %s without %s" %(self.event, str(self.tradeType))
						argList.append(-1)
			else:
				print "DiplomacyUtil - firing " + self.event
		g_eventManager.fireEvent(self.event, *argList)


## Event Dispatching

def handleAIComment(argsList):
	eComment = argsList[0]
	commentArgsSize = argsList[1]
	if commentArgsSize:
		args = argsList[2:]
	else:
		args=[]
	dispatchEvent(eComment, diplo.getWhoTradingWith(), GAME.getActivePlayer(), args)

def handleUserResponse(argsList):
	eComment = argsList[0]
	commentArgsSize = argsList[1]
	if commentArgsSize:
		args = argsList[2:]
	else:
		args=[]
	dispatchEvent(eComment, GAME.getActivePlayer(), diplo.getWhoTradingWith(), args)

def dispatchEvent(eComment, eFromPlayer, eToPlayer, args):
	event = g_eventsByCommentType.get(eComment, None)
	if event:
		event.dispatch(eFromPlayer, eToPlayer, args)


## Event Handlers

def onDealOffered(argsList):
	eOfferPlayer, eTargetPlayer, pTrade = argsList
	of = GC.getPlayer(eOfferPlayer).getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	print "DiplomacyUtil - %s offers trade to %s: %r" %(of, to, pTrade)

def onCityOffered(argsList):
	eOfferPlayer, eTargetPlayer, pTrade = argsList
	of = GC.getPlayer(eOfferPlayer).getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	print "DiplomacyUtil - %s offers city to %s: %r" %(of, to, pTrade)

def onHelpOffered(argsList):
	eOfferPlayer, eTargetPlayer, pTrade = argsList
	of = GC.getPlayer(eOfferPlayer).getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	print "DiplomacyUtil - %s offers help to %s: %r" %(of, to, pTrade)

def onPeaceOffered(argsList):
	eOfferPlayer, eTargetPlayer = argsList
	of = GC.getPlayer(eOfferPlayer).getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	print "DiplomacyUtil - %s offers peace to %s" %(of, to)

def onVassalOffered(argsList):
	eOfferPlayer, eTargetPlayer = argsList
	of = GC.getPlayer(eOfferPlayer).getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	print "DiplomacyUtil - %s offers vassalage to %s" %(of, to)

def onDealCanceled(argsList):
	eOfferPlayer, eTargetPlayer, pTrade = argsList
	if eOfferPlayer != -1 and eTargetPlayer != -1 and pTrade is not None:
		of = GC.getPlayer(eOfferPlayer).getName()
		to = GC.getPlayer(eTargetPlayer).getName()
		print "DiplomacyUtil - %s cancels deal with %s: %r" %(of, to, pTrade)

def onDealAccepted(argsList):
	eTargetPlayer, eOfferPlayer, pTrade = argsList
	to = GC.getPlayer(eTargetPlayer).getName()
	of = GC.getPlayer(eOfferPlayer).getName()
	print "DiplomacyUtil - %s accepts trade offered by %s: %r" %(to, of, pTrade)

def onDealRejected(argsList):
	eTargetPlayer, eOfferPlayer, pTrade = argsList
	to = GC.getPlayer(eTargetPlayer).getName()
	of = GC.getPlayer(eOfferPlayer).getName()
	print "DiplomacyUtil - %s accepts trade offered by %s: %r" %(to, of, pTrade)

def onHelpDemanded(argsList):
	eDemandPlayer, eTargetPlayer, pTrade = argsList
	of = GC.getPlayer(eDemandPlayer).getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	szItems = ""
	for i in range(pTrade.getCount()):
		szItems += TradeUtil.format(eTargetPlayer, pTrade.getTrade(i))
	print "DiplomacyUtil - %s requests help (%s) from %s" %(of, szItems, to)

def onHelpAccepted(argsList):
	eTargetPlayer, eDemandPlayer, pTrade = argsList
	to = GC.getPlayer(eTargetPlayer).getName()
	of = GC.getPlayer(eDemandPlayer).getName()
	szItems = ""
	for i in range(pTrade.getCount()):
		szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i))
	print "DiplomacyUtil - %s agrees to give help (%s) to %s" %(to, szItems, of)

def onHelpRejected(argsList):
	eTargetPlayer, eDemandPlayer, pTrade = argsList
	to = GC.getPlayer(eTargetPlayer).getName()
	of = GC.getPlayer(eDemandPlayer).getName()
	szItems = ""
	for i in range(pTrade.getCount()):
		szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i))
	print "DiplomacyUtil - %s refuses to give help (%s) to %s" %(to, szItems, of)

def onTributeDemanded(argsList):
	eDemandPlayer, eTargetPlayer, pTrade = argsList
	of = GC.getPlayer(eDemandPlayer).getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	szItems = ""
	for i in range(pTrade.getCount()):
		szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i))
	print "DiplomacyUtil - %s demands a tribute (%s) from %s" %(of, szItems, to)

def onTributeAccepted(argsList):
	eTargetPlayer, eDemandPlayer, pTrade = argsList
	to = GC.getPlayer(eTargetPlayer).getName()
	of = GC.getPlayer(eDemandPlayer).getName()
	szItems = ""
	for i in range(pTrade.getCount()):
		szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i))
	print "DiplomacyUtil - %s agrees to give tribute (%s) to %s" %(to, szItems, of)

def onTributeRejected(argsList):
	eTargetPlayer, eDemandPlayer, pTrade = argsList
	to = GC.getPlayer(eTargetPlayer).getName()
	of = GC.getPlayer(eDemandPlayer).getName()
	szItems = ""
	for i in range(pTrade.getCount()):
		szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i))
	print "DiplomacyUtil - %s refuses to give tribute (%s) to %s" %(to, szItems, of)

def onReligionDemanded(argsList):
	eDemandPlayer, eTargetPlayer = argsList
	CyPlayerX = GC.getPlayer(eDemandPlayer)
	of = CyPlayerX.getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	iReligion = CyPlayerX.getStateReligion()
	print "DiplomacyUtil - %s asks %s to convert to %s" %(of, to, GC.getReligionInfo(iReligion).getDescription())

def onReligionAccepted(argsList):
	eTargetPlayer, eDemandPlayer = argsList
	CyPlayerX = GC.getPlayer(eDemandPlayer)
	to = GC.getPlayer(eTargetPlayer).getName()
	of = CyPlayerX.getName()
	iReligion = CyPlayerX.getStateReligion()
	print "DiplomacyUtil - %s accepts demand from %s to convert to %s" %(to, of, GC.getReligionInfo(iReligion).getDescription())

def onReligionRejected(argsList):
	eTargetPlayer, eDemandPlayer = argsList
	CyPlayerX = GC.getPlayer(eDemandPlayer)
	to = GC.getPlayer(eTargetPlayer).getName()
	of = CyPlayerX.getName()
	iReligion = CyPlayerX.getStateReligion()
	print "DiplomacyUtil - %s rejects demand from %s to convert to %s" %(to, of, GC.getReligionInfo(iReligion).getDescription())

def onCivicDemanded(argsList):
	eDemandPlayer, eTargetPlayer = argsList
	CyPlayerX = GC.getPlayer(eDemandPlayer)
	of = CyPlayerX.getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	iCivic = GC.getLeaderHeadInfo(CyPlayerX.getPersonalityType()).getFavoriteCivic()
	if iCivic > -1:
		print "DiplomacyUtil - %s asks %s to switch to %s" %(of, to, GC.getCivicInfo(iCivic).getDescription())

def onCivicAccepted(argsList):
	eTargetPlayer, eDemandPlayer = argsList
	CyPlayerX = GC.getPlayer(eDemandPlayer)
	to = GC.getPlayer(eTargetPlayer).getName()
	of = CyPlayerX.getName()
	iCivic = GC.getLeaderHeadInfo(CyPlayerX.getPersonalityType()).getFavoriteCivic()
	if iCivic > -1:
		print "DiplomacyUtil - %s accepts demand from %s to switch to %s" %(to, of, GC.getCivicInfo(iCivic).getDescription())

def onCivicRejected(argsList):
	eTargetPlayer, eDemandPlayer = argsList
	CyPlayerX = GC.getPlayer(eDemandPlayer)
	to = GC.getPlayer(eTargetPlayer).getName()
	of = CyPlayerX.getName()
	iCivic = GC.getLeaderHeadInfo(CyPlayerX.getPersonalityType()).getFavoriteCivic()
	if iCivic > -1:
		print "DiplomacyUtil - %s rejects demand from %s to switch to %s" %(to, of, GC.getCivicInfo(iCivic).getDescription())

def onWarDemanded(argsList):
	eDemandPlayer, eTargetPlayer, eVictim = argsList
	of = GC.getPlayer(eDemandPlayer).getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	print "DiplomacyUtil - %s asks %s to declare war on %s" %(of, to, GC.getPlayer(eVictim).getName())

def onWarAccepted(argsList):
	eTargetPlayer, eDemandPlayer, eVictim = argsList
	to = GC.getPlayer(eTargetPlayer).getName()
	of = GC.getPlayer(eDemandPlayer).getName()
	print "DiplomacyUtil - %s agrees to demand from %s to declare war on %s" %(to, of, GC.getPlayer(eVictim).getName())

def onWarRejected(argsList):
	eTargetPlayer, eDemandPlayer, eVictim = argsList
	to = GC.getPlayer(eTargetPlayer).getName()
	of = GC.getPlayer(eDemandPlayer).getName()
	print "DiplomacyUtil - %s rejects demand from %s to declare war on %s" %(to, of, GC.getPlayer(eVictim).getName())

def onEmbargoDemanded(argsList):
	eDemandPlayer, eTargetPlayer, eVictim = argsList
	of = GC.getPlayer(eDemandPlayer).getName()
	to = GC.getPlayer(eTargetPlayer).getName()
	print "DiplomacyUtil - %s asks %s to stop trading with %s" %(of, to, GC.getPlayer(eVictim).getName())

def onEmbargoAccepted(argsList):
	eTargetPlayer, eDemandPlayer, eVictim = argsList
	to = GC.getPlayer(eTargetPlayer).getName()
	of = GC.getPlayer(eDemandPlayer).getName()
	print "DiplomacyUtil - %s agrees to demand from %s to stop trading with %s" %(to, of, GC.getPlayer(eVictim).getName())

def onEmbargoRejected(argsList):
	eTargetPlayer, eDemandPlayer, eVictim = argsList
	to = GC.getPlayer(eTargetPlayer).getName()
	of = GC.getPlayer(eDemandPlayer).getName()
	print "DiplomacyUtil - %s rejects demand from %s to stop trading with %s" %(to, of, GC.getPlayer(eVictim).getName())


## Proposed Trade Functions

def getProposedTrade():
	trade = TradeUtil.Trade(GAME.getActivePlayer(), diplo.getWhoTradingWith())
	if not diplo.ourOfferEmpty():
		getProposedTradeData(diplo.getPlayerTradeOffer, trade.addTrade)
	if not diplo.theirOfferEmpty():
		getProposedTradeData(diplo.getTheirTradeOffer, trade.addOtherTrade)
	print "DiplomacyUtil.getProposedTrade - %r" % trade
	return trade

def getProposedTradeData(getFunc, addFunc):
	for index in range(50):
		data = getFunc(index)
		if data:
			addFunc(data)
		else:
			break
	else:
		print "DiplomacyUtil.getProposedTradeData - reached %d items, ignoring rest" % 50
