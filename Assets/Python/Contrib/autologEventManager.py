## autologger
## Modified from Ruff Mod 2w
## Modified from HOF MOD V1.61.001
## Modified from autolog by eotinb
## autolog's event handler
## by eotinb

from CvPythonExtensions import *
import CvUtil
import autolog
import time
import BugCore
import BugUtil
import TradeUtil

OPEN_LOG_EVENT_ID = CvUtil.getNewEventID()
CUSTOM_ENTRY_EVENT_ID = CvUtil.getNewEventID()

GC = CyGlobalContext()
GAME = GC.getGame()
CyIF = CyInterface()
TRNSLTR = CyTranslator()

AutologOpt = BugCore.game.Autolog
Logger = None

iLastPillageGold = None
def doPillageGoldListener(argsList, value):
	global iLastPillageGold
	iLastPillageGold = value

def isLoggingOn():
	return AutologOpt.isLoggingOn()

def setLoggingOn(value):
	AutologOpt.setLoggingOn(value)

def setFileName(option, value):
	Logger.setLogFileName(value)

def setFilePath(option, value):
	Logger.setLogFilePath(value)

def setDefaultFileName(option, value):
	pass

def StartLogger(vsFileName):
	if (not vsFileName):
		vsFileName = Logger.getLogFileName()

	if (AutologOpt.isUseDefaultFileName()
		or not vsFileName):
		ePlayer = GAME.getActivePlayer()
		szfileName = GC.getPlayer(ePlayer).getName()
	else:
		szfileName = vsFileName

	ziStyle = AutologOpt.getFormatStyle()
#	' valid styles are plain (0), html (1), forum with " for color(2) or forum without " for color(3)'
	if (ziStyle == 1):
		if not (szfileName.endswith(".html")):
			szfileName = szfileName + ".html"
	else:
		if not (szfileName.endswith(".txt")):
			szfileName = szfileName + ".txt"

	Logger.setLogFileName(szfileName)
	if not AutologOpt.isSilent():
		msg = TRNSLTR.getText("TXT_KEY_AUTOLOG_LOGGING_GAME", (szfileName,))
		CvUtil.sendMessage(msg, None, 10, "", ColorTypes(8), eMsgType = 2)


class autologEventManager:

	def __init__(self, eventManager):

		global Logger
		Logger = autolog.autologInstance()

		AutoLogEvent(eventManager)

		# additions to self.Events
		moreEvents = {
			OPEN_LOG_EVENT_ID : ('LogOpenPopup', self.__OPEN_LOG_EVENT_IDApply, self.__OPEN_LOG_EVENT_IDBegin),
			CUSTOM_ENTRY_EVENT_ID : ('', self.__CUSTOM_ENTRY_EVENT_IDApply, self.__CUSTOM_ENTRY_EVENT_IDBegin),
		}
		eventManager.Events.update(moreEvents)

	def __OPEN_LOG_EVENT_IDBegin(self, argsList):
		popup = CyPopup(OPEN_LOG_EVENT_ID, EventContextTypes.EVENTCONTEXT_SELF, True)

		if AutologOpt.isUseDefaultFileName():
			popup.setHeaderString(BugUtil.getPlainText("TXT_KEY_AUTOLOG_POPUP_QUESTION"), 1<<2)
			popup.setBodyString(BugUtil.getPlainText("TXT_KEY_AUTOLOG_POPUP_ANSWERS"), 1<<0)
		else:
			popup.setHeaderString(BugUtil.getPlainText("TXT_KEY_AUTOLOG_ENTER_LOG_NAME"), 1<<2)
			popup.createEditBox(AutologOpt.getFileName(), 0)
			popup.setEditBoxMaxCharCount(30, 32, 0)

		popup.addButton(BugUtil.getPlainText("TXT_KEY_MAIN_MENU_OK"))
		popup.addButton(BugUtil.getPlainText("TXT_KEY_SCREEN_CANCEL"))
		popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

	def __OPEN_LOG_EVENT_IDApply(self, playerID, userData, popupReturn):
		if (popupReturn.getButtonClicked() != 1):
			setLoggingOn(True)
			StartLogger(popupReturn.getEditBoxString(0))
		else:
			setLoggingOn(False)
			msg = TRNSLTR.getText("TXT_KEY_AUTOLOG_NO_LOGGING",())
			CvUtil.sendMessage(msg, None, 10, "", ColorTypes(8), eMsgType = 2)

	def __CUSTOM_ENTRY_EVENT_IDBegin(self, argsList):
		if isLoggingOn():
			popup = CyPopup(CUSTOM_ENTRY_EVENT_ID, EventContextTypes.EVENTCONTEXT_SELF, True)
			popup.setHeaderString(BugUtil.getPlainText("TXT_KEY_AUTOLOG_CUSTOM_ENTRY"), 1<<2)
			popup.createEditBox("", 0)
			popup.addButton(BugUtil.getPlainText("TXT_KEY_MAIN_MENU_OK"))
			popup.addButton(BugUtil.getPlainText("TXT_KEY_SCREEN_CANCEL"))
			popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

	def __CUSTOM_ENTRY_EVENT_IDApply(self, playerID, userData, popupReturn):
		if isLoggingOn() and popupReturn.getButtonClicked() != 1:
			msg = popupReturn.getEditBoxString(0)
			Logger.writeLog(msg, vPrefix=AutologOpt.getPrefix())

			if not AutologOpt.isSilent():
				CvUtil.sendMessage(msg, None, 10, "", ColorTypes(8), eMsgType = 2)


class AbstractAutoLogEvent(object):

	def __init__(self, eventManager, *args, **kwargs):
		super(AbstractAutoLogEvent, self).__init__(*args, **kwargs)

class AutoLogEvent(AbstractAutoLogEvent):

	def __init__(self, eventManager, *args, **kwargs):
		super(AutoLogEvent, self).__init__(eventManager, *args, **kwargs)

		eventManager.addEventHandler("kbdEvent", self.onKbdEvent)
		eventManager.addEventHandler("OnLoad", self.onLoadGame)
		eventManager.addEventHandler("GameStart", self.onGameStart)
		eventManager.addEventHandler("GameEnd", self.onGameEnd)
		eventManager.addEventHandler("EndGameTurn", self.onEndGameTurn)
		eventManager.addEventHandler("BeginPlayerTurn", self.onBeginPlayerTurn)
		eventManager.addEventHandler("EndPlayerTurn", self.onEndPlayerTurn)
		eventManager.addEventHandler("firstContact", self.onFirstContact)
		eventManager.addEventHandler("combatLogCalc", self.onCombatLogCalc)
		eventManager.addEventHandler("combatResult", self.onCombatResult)
		eventManager.addEventHandler("combatLogHit", self.onCombatLogHit)
		eventManager.addEventHandler("buildingBuilt", self.onBuildingBuilt)
		eventManager.addEventHandler("projectBuilt", self.onProjectBuilt)
		eventManager.addEventHandler("unitBuilt", self.onUnitBuilt)
		eventManager.addEventHandler("unitPromoted", self.onUnitPromoted)
		eventManager.addEventHandler("goodyReceived", self.onGoodyReceived)
		eventManager.addEventHandler("greatPersonBorn", self.onGreatPersonBorn)
		eventManager.addEventHandler("techAcquired", self.onTechAcquired)
		eventManager.addEventHandler("techSelected", self.onTechSelected)
		eventManager.addEventHandler("religionFounded", self.onReligionFounded)
		eventManager.addEventHandler("religionSpread", self.onReligionSpread)
		eventManager.addEventHandler("religionRemove", self.onReligionRemove)
		eventManager.addEventHandler("corporationFounded", self.onCorporationFounded)
		eventManager.addEventHandler("corporationSpread", self.onCorporationSpread)
		eventManager.addEventHandler("corporationRemove", self.onCorporationRemove)
		eventManager.addEventHandler("goldenAge", self.onGoldenAge)
		eventManager.addEventHandler("endGoldenAge", self.onEndGoldenAge)
		eventManager.addEventHandler("changeWar", self.onChangeWar)
		eventManager.addEventHandler("setPlayerAlive", self.onSetPlayerAlive)
		eventManager.addEventHandler("cityBuilt", self.onCityBuilt)
		eventManager.addEventHandler("cityRazed", self.onCityRazed)
		eventManager.addEventHandler("cityAcquired", self.onCityAcquired)
		eventManager.addEventHandler("cityLost", self.onCityLost)
		eventManager.addEventHandler("cultureExpansion", self.onCultureExpansion)
		eventManager.addEventHandler("cityGrowth", self.onCityGrowth)
		eventManager.addEventHandler("cityBuildingUnit", self.onCityBuildingUnit)
		eventManager.addEventHandler("cityBuildingBuilding", self.onCityBuildingBuilding)
		eventManager.addEventHandler("improvementBuilt", self.onImprovementBuilt)
		eventManager.addEventHandler("improvementDestroyed", self.onImprovementDestroyed)
		eventManager.addEventHandler("unitPillage", self.onUnitPillage)
		eventManager.addEventHandler("vassalState", self.onVassalState)
		eventManager.addEventHandler("selectionGroupPushMission", self.onSelectionGroupPushMission)
		eventManager.addEventHandler("DealOffered", self.onDealOffered)
		eventManager.addEventHandler("CityOffered", self.onCityOffered)
		eventManager.addEventHandler("HelpOffered", self.onHelpOffered)
		eventManager.addEventHandler("PeaceOffered", self.onPeaceOffered)
		eventManager.addEventHandler("VassalOffered", self.onVassalOffered)
		eventManager.addEventHandler("DealCanceled", self.onDealCanceled)
		eventManager.addEventHandler("DealAccepted", self.onDealAccepted)
		eventManager.addEventHandler("DealRejected", self.onDealRejected)
		#eventManager.addEventHandler("HelpDemanded", self.onHelpDemanded)
		eventManager.addEventHandler("HelpAccepted", self.onHelpAccepted)
		eventManager.addEventHandler("HelpRejected", self.onHelpRejected)
		#eventManager.addEventHandler("TributeDemanded", self.onTributeDemanded)
		eventManager.addEventHandler("TributeAccepted", self.onTributeAccepted)
		eventManager.addEventHandler("TributeRejected", self.onTributeRejected)
		#eventManager.addEventHandler("ReligionDemanded", self.onReligionDemanded)
		eventManager.addEventHandler("ReligionAccepted", self.onReligionAccepted)
		eventManager.addEventHandler("ReligionRejected", self.onReligionRejected)
		#eventManager.addEventHandler("CivicDemanded", self.onCivicDemanded)
		eventManager.addEventHandler("CivicAccepted", self.onCivicAccepted)
		eventManager.addEventHandler("CivicRejected", self.onCivicRejected)
		#eventManager.addEventHandler("WarDemanded", self.onWarDemanded)
		eventManager.addEventHandler("WarAccepted", self.onWarAccepted)
		eventManager.addEventHandler("WarRejected", self.onWarRejected)
		#eventManager.addEventHandler("EmbargoDemanded", self.onEmbargoDemanded)
		eventManager.addEventHandler("EmbargoAccepted", self.onEmbargoAccepted)
		eventManager.addEventHandler("EmbargoRejected", self.onEmbargoRejected)

		self.eventMgr = eventManager
		self.fOdds = 0.0
		self.iBattleWonDefending = 0
		self.iBattleLostDefending = 0
		self.iBattleWonAttacking = 0
		self.iBattleLostAttacking = 0
		self.iBattleWdlAttacking = 0
		self.iBattleEscAttacking = 0

		self.bHumanPlaying = False
		self.bHumanEndTurn = False
		self.bAIsTurn = False

		self.UnitKilled = 0
		self.WonLastRound = 0
		self.cdAttacker = None
		self.cdDefender = None

		self.CIVAttitude = None
		self.CIVCivics = None
		self.CIVReligion = None
		self.CityWhipCounter = None
		self.CityConscriptCounter = None

	def onKbdEvent(self, argsList):
		eventType,key,mx,my,px,py = argsList
		if eventType == 6:
			theKey=int(key)
			'Check if ALT + E was hit == echoes to text log and in-game log'
			if (theKey == int(InputTypes.KB_E)
			and self.eventMgr.bAlt
			and AutologOpt.isEnabled()
			and isLoggingOn()):
				self.eventMgr.beginEvent(CUSTOM_ENTRY_EVENT_ID)
				return 1

			'Check if ALT + L was hit == open in-game log'
			if (theKey == int(InputTypes.KB_L)
			and self.eventMgr.bAlt
			and AutologOpt.isEnabled()):
				if AutologOpt.isSilent():
					setLoggingOn(True)
					StartLogger("")
				else:
					self.eventMgr.beginEvent(OPEN_LOG_EVENT_ID)

				return 1

			'Check if ALT + B was hit == dump battle stats, and reset'
			if (theKey == int(InputTypes.KB_B)
			and self.eventMgr.bAlt
			and AutologOpt.isEnabled()
			and isLoggingOn()):
				Logger.writeLog("")
				Logger.writeLog(BugUtil.getPlainText("TXT_KEY_AUTOLOG_BATTLE_STATS"), vBold=True)
				msg = TRNSLTR.getText("TXT_KEY_AUTOLOG_UNITS_VICTORIOUS_ATTACKING", (self.iBattleWonAttacking, ))
				Logger.writeLog(msg, vColor="DarkRed")
				msg = TRNSLTR.getText("TXT_KEY_AUTOLOG_UNITS_VICTORIOUS_DEFENDING", (self.iBattleWonDefending, ))
				Logger.writeLog(msg, vColor="DarkRed")
				msg = TRNSLTR.getText("TXT_KEY_AUTOLOG_UNITS_WITHDRAWING_ATTACKING", (self.iBattleWdlAttacking, ))
				Logger.writeLog(msg, vColor="DarkRed")
				msg = TRNSLTR.getText("TXT_KEY_AUTOLOG_UNITS_DEFEATED_ATTACKING", (self.iBattleLostAttacking, ))
				Logger.writeLog(msg, vColor="Red")
				msg = TRNSLTR.getText("TXT_KEY_AUTOLOG_UNITS_DEFEATED_DEFENDING", (self.iBattleLostDefending, ))
				Logger.writeLog(msg, vColor="Red")
				msg = TRNSLTR.getText("TXT_KEY_AUTOLOG_UNITS_ESCAPING_ATTACKING", (self.iBattleEscAttacking, ))
				Logger.writeLog(msg, vColor="Red")

				self.iBattleWonDefending = 0
				self.iBattleLostDefending = 0
				self.iBattleWonAttacking = 0
				self.iBattleLostAttacking = 0
				self.iBattleWdlAttacking = 0
				self.iBattleEscAttacking = 0

				msg = TRNSLTR.getText("TXT_KEY_AUTOLOG_BATTLE_STATS_WRITTEN",())
				CvUtil.sendMessage(msg, None, 10, "", ColorTypes(8), eMsgType = 2)
				return 1


	def onLoadGame(self, argsList):
		self.bHumanPlaying = True
		self.bHumanEndTurn = False
		self.bAIsTurn = False

		if (AutologOpt.isEnabled()
		and AutologOpt.isSilent()):
			setLoggingOn(True)
			StartLogger("")
		else:
			setLoggingOn(False)

		# initialize storage stuff
		self.initStuff()
		self.storeStuff()
		self.storeWhip()

	def onGameStart(self, argsList):
		self.bHumanPlaying = True
		self.bHumanEndTurn = False
		self.bAIsTurn = False

		if (AutologOpt.isEnabled()
		and AutologOpt.isSilent()):
			setLoggingOn(True)
			StartLogger("")
		else:
			setLoggingOn(False)

		# initialize storage stuff
		self.initStuff()
		self.storeStuff()
		self.storeWhip()

	def onGameEnd(self, argsList):
		'Called at the End of the game'

	def onEndGameTurn(self, argsList):
		iGameTurn = argsList[0]

		if isLoggingOn():
			self.checkStuff()
			self.storeStuff()

			iMaxTurns = GAME.getMaxTurns()
			year = GAME.getTurnYear(iGameTurn + 1)
			if year < 0:
				year = TRNSLTR.getText("TXT_KEY_TIME_BC", (-year,))
			else:
				year = TRNSLTR.getText("TXT_KEY_TIME_AD", (year,))

			if iMaxTurns:
				sTurn = "%i/%i" %(GAME.getElapsedGameTurns() + 1 + AutologOpt.getStartDateTurn(), iMaxTurns)
			else:
				sTurn = "%i" % GAME.getElapsedGameTurns() + 1 + AutologOpt.getStartDateTurn()

			Logger.writeLog_pending_flush()
			Logger.writeLog_pending("")
			Logger.writeLog_pending(TRNSLTR.getText("TXT_KEY_AUTOLOG_TURN", (sTurn, year, time.strftime("%d-%b-%Y %H:%M:%S"))), vBold=True, vUnderline=True)

		self.bHumanPlaying = True
		self.bHumanEndTurn = False
		self.bAIsTurn = False

	def onBeginPlayerTurn(self, argsList):
		'Called at the beginning of a players turn'
		iGameTurn, iPlayer = argsList

		if iPlayer == GAME.getActivePlayer():
			self.bHumanPlaying = False
			self.bHumanEndTurn = True
			self.bAIsTurn = False

		if not self.bHumanEndTurn:
			return

		self.logSliders()

		if AutologOpt.isShowIBT():
#			Logger.writeLog_pending_flush()
			Logger.writeLog_pending("")
			Logger.writeLog_pending(BugUtil.getPlainText("TXT_KEY_AUTOLOG_AFTER_END_TURN"), vBold=True)
#			Logger.writeLog("After End Turn-:", vBold=True)

		if AutologOpt.isLogCityWhipStatus():
			iPlayer = GC.getActivePlayer()
			for i in xrange(0, iPlayer.getNumCities(), 1):
				iCity = iPlayer.getCity(i)
				iCurrentWhipCounter = iCity.getHurryAngerTimer()
				iCurrentConstrictCounter = iCity.getConscriptAngerTimer()
#				if iCurrentWhipCounter != 0: iCurrentWhipCounter += 1  # onBeginPlayerTurn fires after whip counter has decreased by 1

#				message = "Whip Testing: %s, current(%i), prior(%i), flat(%i)" % (iCity.getName(), iCurrentWhipCounter, self.CityWhipCounter[i], iCity.flatHurryAngerLength())
#				Logger.writeLog(message)

				if iCurrentWhipCounter > self.CityWhipCounter[i]:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_WHIP_APPLIED", (iCity.getName(), ))
					Logger.writeLog(message, vColor="Red")

				if iCurrentConstrictCounter > self.CityConscriptCounter[i]:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CONSCRIPT", (GC.getUnitInfo(iCity.getConscriptUnit()).getDescription(), iCity.getName()))
					Logger.writeLog(message, vColor="Red")

				if (self.CityWhipCounter[i] != 0
				and iCurrentWhipCounter < self.CityWhipCounter[i]
				and iCurrentWhipCounter % iCity.flatHurryAngerLength() == 0):
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_WHIP_ANGER_DECREASED", (iCity.getName(), ))
					Logger.writeLog(message, vColor="DarkRed")

				if (self.CityConscriptCounter[i] != 0
				and iCurrentConstrictCounter < self.CityConscriptCounter[i]
				and iCurrentConstrictCounter % iCity.flatConscriptAngerLength() == 0):
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DRAFT_ANGER_DECREASED", (iCity.getName(), ))
					Logger.writeLog(message, vColor="DarkRed")

			self.storeWhip()

	def onEndPlayerTurn(self, argsList):
		'Called at the end of a players turn'
		iGameTurn, iPlayer = argsList

		if (self.bHumanEndTurn
		and AutologOpt.isShowIBT()):
#			Logger.writeLog_pending_flush()
			Logger.writeLog_pending("")
			Logger.writeLog_pending(BugUtil.getPlainText("TXT_KEY_AUTOLOG_OTHER_PLAYER_ACTIONS"), vBold=True)
#			Logger.writeLog("Other Player Actions-:", vBold=True)

		if iPlayer == GAME.getActivePlayer():
			self.bHumanPlaying = False
			self.bHumanEndTurn = False
			self.bAIsTurn = True

	def onFirstContact(self, argsList):
		if (AutologOpt.isLogContact()):
			iTeamX,iHasMetTeamY = argsList
			if (iTeamX == 0
			and GAME.getGameTurn() > 0):

				sMsgArray = []
				sLeader = GC.getTeam(iHasMetTeamY).getName()
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_FIRST_CONTACT_TEAM", (sLeader, ))
#				Logger.writeLog(message)
				sMsgArray.append(message)

				for iPlayer in xrange(GC.getMAX_PLAYERS()):
					if GC.getPlayer(iPlayer).getTeam() == iHasMetTeamY:
						sLeader = GC.getLeaderHeadInfo(GC.getPlayer(iPlayer).getLeaderType()).getDescription()
						sCivName = GC.getPlayer(iPlayer).getCivilizationShortDescription(0)

						message = TRNSLTR.getText("TXT_KEY_AUTOLOG_FIRST_CONTACT_PLAYER", (sLeader, sCivName))
#						Logger.writeLog(message)
						sMsgArray.append(message)

				iLen = len(sMsgArray)
				if iLen == 2:
					Logger.writeLog(sMsgArray[1], vColor="Brown")
				else:
					for i in xrange(iLen):
						Logger.writeLog(sMsgArray[i], vColor="Brown")

	def onCombatLogCalc(self, argsList):
		if AutologOpt.isLogCombat():
			genericArgs = argsList[0][0]
			cdAttacker = genericArgs[0]
			cdDefender = genericArgs[1]
			iCombatOdds = genericArgs[2]

			self.fOdds = iCombatOdds/10.0

			self.UnitKilled = 0
			self.WonLastRound = 0

	def onCombatResult(self, argsList):
		if not AutologOpt.isLogCombat(): return
		CyUnitW, CyUnitL = argsList # [W]inner & [L]ooser

		self.UnitKilled = 1
		iActivePlayer = GAME.getActivePlayer()
		iPlayerW = CyUnitW.getOwner()

		if iActivePlayer in (iPlayerW, CyUnitL.getOwner()):
			fHealthW = CyUnitW.baseCombatStr() * CyUnitW.getHP() / float(CyUnitW.getMaxHP())
			zsBattleLocn = self.getUnitLocation(CyUnitW)

			if iPlayerW == iActivePlayer:
				szText = GC.getPlayer(CyUnitL.getVisualOwner()).getCivilizationAdjective(0)
				if self.bHumanPlaying:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_WHILE_ATTACKING_DEFEATS", (zsBattleLocn, CyUnitW.getNameKey(), BugUtil.formatFloat(fHealthW, 2), CyUnitW.baseCombatStr(), szText, CyUnitL.getNameKey(), BugUtil.formatFloat(self.fOdds, 1), "%"))
					self.iBattleWonAttacking += 1
				else:
					self.fOdds = 100 - self.fOdds
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_WHILE_DEFENDING_DEFEATS", (zsBattleLocn, CyUnitW.getNameKey(), BugUtil.formatFloat(fHealthW, 2), CyUnitW.baseCombatStr(), szText, CyUnitL.getNameKey(), BugUtil.formatFloat(self.fOdds, 1), "%"))
					self.iBattleWonDefending += 1

				Logger.writeLog(message, vColor="DarkRed")

			else:
				szText = GC.getPlayer(CyUnitW.getVisualOwner()).getCivilizationAdjective(0)
				if self.bHumanPlaying:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_WHILE_ATTACKING_LOSES", (zsBattleLocn, CyUnitL.getNameKey(), szText, CyUnitW.getNameKey(), BugUtil.formatFloat(fHealthW, 2), CyUnitW.baseCombatStr(), BugUtil.formatFloat(self.fOdds, 1), "%"))
					self.iBattleLostAttacking = self.iBattleLostAttacking + 1
				else:
					self.fOdds = 100 - self.fOdds
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_WHILE_DEFENDING_LOSES", (zsBattleLocn, CyUnitL.getNameKey(), szText, CyUnitW.getNameKey(), BugUtil.formatFloat(fHealthW, 2), CyUnitW.baseCombatStr(), BugUtil.formatFloat(self.fOdds, 1), "%"))
					self.iBattleLostDefending = self.iBattleLostDefending + 1

				Logger.writeLog(message, vColor="Red")

	def onCombatLogHit(self, argsList):
		'Combat Message'
		genericArgs = argsList[0][0]
		CombatDetailsAttacker = genericArgs[0]
		CombatDetailsDefender = genericArgs[1]
		iIsAttacker = genericArgs[2]
		iDamage = genericArgs[3]

		self.cdAttacker = CombatDetailsAttacker
		self.cdDefender = CombatDetailsDefender

		if not iIsAttacker:
			self.WonLastRound = 0

		elif iIsAttacker == 1:
			self.WonLastRound = 1

	def onSelectionGroupPushMission(self, argsList):
		cdDefender = self.cdDefender
		if cdDefender == None: return
		if not AutologOpt.isLogCombat():
			self.cdDefender = None
			return
		iPlayer, iMission, iNumUnits, lUnitIDs = argsList

		if GC.getPlayer(iPlayer).getTeam() == GC.getActivePlayer().getTeam():

			szCiv = GC.getPlayer(cdDefender.eVisualOwner).getCivilizationAdjective(0)

			if not self.UnitKilled:
				if self.WonLastRound == 1:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_WHILE_ATTACKING_ESCAPES", (self.cdAttacker.sUnitName, szCiv, cdDefender.sUnitName, BugUtil.formatFloat(self.fOdds, 1), "%"))
					Logger.writeLog(message, vColor="Red")
					self.iBattleEscAttacking = self.iBattleEscAttacking + 1
				else:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_WHILE_ATTACKING_DECIMATES", (self.cdAttacker.sUnitName, szCiv, cdDefender.sUnitName, BugUtil.formatFloat(self.fOdds, 1), "%"))
					Logger.writeLog(message, vColor="DarkRed")
					self.iBattleWdlAttacking = self.iBattleWdlAttacking + 1

		self.cdDefender = None

	def getUnitLocation(self, CyUnit):
		CyPlot = CyUnit.plot()
		iOwner = CyPlot.getOwner()
		if iOwner > -1:
			szText = TRNSLTR.getText("TXT_KEY_AUTOLOG_IN_TERRITORY", (GC.getPlayer(iOwner).getCivilizationAdjective(0),))
		elif not CyPlot.isWater():
			szText = BugUtil.getPlainText("TXT_KEY_AUTOLOG_IN_THE_WILD")
		elif CyPlot.isLake():
			szText = BugUtil.getPlainText("TXT_KEY_AUTOLOG_ON_A_LAKE")
		elif CyPlot.isCoastal():
			szText = BugUtil.getPlainText("TXT_KEY_AUTOLOG_JUST_OFF_SHORE")
		else:
			szText = BugUtil.getPlainText("TXT_KEY_AUTOLOG_ON_THE_HIGH_SEAS")

		for plotX in CyPlot.rect(1, 1):
			if plotX.isCity():
				CyCity = plotX.getPlotCity()
				return TRNSLTR.getText("TXT_KEY_AUTOLOG_IN_TERRITORY_AT", (szText, CyCity.getName()))

		for plotX in CyPlot.rect(4, 4):
			if plotX.isCity():
				CyCity = plotX.getPlotCity()
				return TRNSLTR.getText("TXT_KEY_AUTOLOG_IN_TERRITORY_NEAR", (szText, CyCity.getName()))

		return szText

	def onBuildingBuilt(self, argsList):
		if (AutologOpt.isLogBuildCompleted()):
			pCity, iBuildingType = argsList
			if pCity.getOwner() == GAME.getActivePlayer():
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_FINISH_BUILDING", (pCity.getName(), GC.getBuildingInfo(iBuildingType).getDescription()))
				Logger.writeLog(message, vColor="Purple")

	def onProjectBuilt(self, argsList):
		if (AutologOpt.isLogBuildCompleted()):
			pCity, iProjectType = argsList
			if pCity.getOwner() == GAME.getActivePlayer():
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_FINISH_PROJECT", (pCity.getName(), GC.getProjectInfo(iProjectType).getDescription()))
				Logger.writeLog(message, vColor="Purple")

	def onUnitBuilt(self, argsList):
		if (AutologOpt.isLogBuildCompleted()):
			pCity = argsList[0]
			unit = argsList[1]
			if pCity.getOwner() == GAME.getActivePlayer():
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_FINISH_UNIT", (pCity.getName(), GC.getUnitInfo(unit.getUnitType()).getDescription()))
				Logger.writeLog(message, vColor="Purple")

	def onUnitPromoted(self, argsList):
		if AutologOpt.isLogPromotion():
			CyUnit, iPromotion = argsList
			if CyUnit.getOwner() == GAME.getActivePlayer():
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_PROMOTION", (CyUnit.getName(), GC.getPromotionInfo(iPromotion).getDescription()))
				Logger.writeLog(message, vColor="DarkOrange")

	def onGoodyReceived(self, argsList):
		if (AutologOpt.isLogTribalVillage()):
			iPlayer, pPlot, pUnit, iGoodyType = argsList
			if iPlayer == GAME.getActivePlayer():
				GoodyTypeMap = {
						-1: "TXT_KEY_AUTOLOG_VILLAGE_RESULT_NOTHING",
						0:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LITTLEGOLD",
						1:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LOTSOFGOLD",
						2:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LITTLEGOLD",
						3:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LOTSOFGOLD",
						4:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LITTLEGOLD",
						5:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LOTSOFGOLD",
						6:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LITTLEGOLD",
						7:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LOTSOFGOLD",
						8:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LITTLEGOLD",
						9:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LOTSOFGOLD",
						10:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LITTLEGOLD",
						11:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LOTSOFGOLD",
						12:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LITTLEGOLD",
						13:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LOTSOFGOLD",
						14:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LITTLEGOLD",
						15:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LOTSOFGOLD",
						16:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LITTLEGOLD",
						17:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LOTSOFGOLD",
						18:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LITTLERESEARCH",
						19:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LOTSOFRESEARCH",
						20:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_MAP",
						21:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_TRIBE",
						22:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SETTLER",
						23:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SETTLER",
						24:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SETTLER",
						25:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SETTLER",
						26:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SETTLER",
						27:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SETTLER",
						28:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SETTLER",
						29:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SETTLER",
						30:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_XP",
						31:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_XP",
						32:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_XP",
						33:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_XP",
						34:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_XP",
						35:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_XP",
						36:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_XP",
						37:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_XP",
						38:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_XP",
						39:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HEALING",
						40:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_TECH",
						41:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WARRIOR",
						42:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STONESPEARMAN",
						43:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STONEAXEMAN",
						44:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SPEARMAN",
						45:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_AXEMAN",
						46:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SWORDSMAN",
						47:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_PIKEMAN",
						48:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_MACEMAN",
						49:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HEAVYSWORDSMAN",
						50:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HEAVYPIKEMAN",
						51:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_ARQUEBUSIER",
						52:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_RIFLEMAN",
						53:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_INFANTRY",
						54:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_TESLA",
						55:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_MECH",
						56:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STONETHROWER",
						57:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_ARCHER",
						58:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_COMPOSITEBOWMAN",
						59:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_LONGBOWMAN",
						60:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SCOUT",
						61:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SCOUT",
						62:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SCOUT",
						63:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SCOUT",
						64:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SCOUT",
						65:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_SCOUT",
						66:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HUNTER",
						67:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HUNTER",
						68:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HUNTER",
						69:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HUNTER",
						70:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HUNTER",
						71:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HUNTER",
						72:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WORKER",
						73:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HEALER",
						74:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HEALER",
						75:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HEALER",
						76:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HEALER",
						77:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HEALER",
						78:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HEALER",
						79:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_HEALER",
						80:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_GATHERER",
						81:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WORKER",
						82:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WORKER",
						83:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WORKER",
						84:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WORKER",
						85:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WORKER",
						86:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WORKER",
						87:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WORKER",
						88:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WORKER",
						89:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WEAKHOSTILES",
						90:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STRONGHOSTILES",
						91:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_CRIMINALHOSTILES",
						92:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WEAKHOSTILES",
						93:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STRONGHOSTILES",
						94:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_CRIMINALHOSTILES",
						95:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WEAKHOSTILES",
						96:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STRONGHOSTILES",
						97:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_CRIMINALHOSTILES",
						98:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WEAKHOSTILES",
						99:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STRONGHOSTILES",
						100:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_CRIMINALHOSTILES",
						101:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WEAKHOSTILES",
						102:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STRONGHOSTILES",
						103:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_CRIMINALHOSTILES",
						104:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WEAKHOSTILES",
						105:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STRONGHOSTILES",
						106:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_CRIMINALHOSTILES",
						107:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WEAKHOSTILES",
						108:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STRONGHOSTILES",
						109:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_CRIMINALHOSTILES",
						110:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_WEAKHOSTILES",
						111:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_STRONGHOSTILES",
						112:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALWEAKHOSTILES",
						113:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALSTRONGHOSTILES",
						114:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALWEAKHOSTILES",
						115:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALSTRONGHOSTILES",
						116:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALWEAKHOSTILES",
						117:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALSTRONGHOSTILES",
						118:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALWEAKHOSTILES",
						119:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALSTRONGHOSTILES",
						120:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALWEAKHOSTILES",
						121:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALSTRONGHOSTILES",
						122:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALWEAKHOSTILES",
						123:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALSTRONGHOSTILES",
						124:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALWEAKHOSTILES",
						125:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALSTRONGHOSTILES",
						126:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALWEAKHOSTILES",
						127:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALSTRONGHOSTILES",
						128:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALWEAKHOSTILES",
						129:	"TXT_KEY_AUTOLOG_VILLAGE_RESULT_NAVALSTRONGHOSTILES"
					}
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_VILLAGE_RESULT", (BugUtil.getPlainText(GoodyTypeMap[iGoodyType]), ))
				Logger.writeLog(message, vColor="Brown")

	def onGreatPersonBorn(self, argsList):
		if (AutologOpt.isLogGreatPeople()):
			pUnit, iPlayer, pCity = argsList
			if iPlayer == GAME.getActivePlayer():
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_GP_BORN", (pUnit.getName(), pCity.getName()))
				Logger.writeLog(message, vColor="Brown")

	def onTechAcquired(self, argsList):
		if GAME.getGameTurn() == 0:
			return

		if AutologOpt.isLogTechnology():
			iTech, iTeam, iPlayer, bAnnounce = argsList

			bWrite = False
			if iPlayer == GAME.getActivePlayer():
				bWrite = True

				if self.bHumanEndTurn:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_TECH_RESEARCHED", (GC.getTechInfo(iTech).getDescription(),))
				else:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_TECH_ACQUIRED", (GC.getTechInfo(iTech).getDescription(),))
			else:
				if self.bHumanPlaying:
					bWrite = True
					zsCiv = GC.getPlayer(iPlayer).getName() + " (" + GC.getPlayer(iPlayer).getCivilizationShortDescription(0) + ")"
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_TECH_TRADED", (zsCiv, GC.getTechInfo(iTech).getDescription()))

			if bWrite:
				Logger.writeLog(message, vColor="Green")

	def onTechSelected(self, argsList):
		if AutologOpt.isLogTechnology():
			iTech, iPlayer = argsList
			if iPlayer == GAME.getActivePlayer():
				CyPlayer = GC.getPlayer(iPlayer)
				CyTeam = GC.getTeam(CyPlayer.getTeam())
				iProgress = CyTeam.getResearchProgress(CyPlayer.getCurrentResearch())
				iOverflow = CyPlayer.getOverflowResearch() * CyPlayer.calculateResearchModifier(CyPlayer.getCurrentResearch())/100
				iTechCost = CyTeam.getResearchCost(CyPlayer.getCurrentResearch())
				iRate = CyPlayer.calculateResearchRate(-1)
				if iRate == -1:
					print "Divide by zero in autologEventManager | onTechSelected"
				else:
					zTurns = (iTechCost - iProgress - iOverflow) / iRate + 1
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_RESEARCH_BEGUN", (GC.getTechInfo(iTech).getDescription(), zTurns))
					Logger.writeLog(message, vColor="Green")

	def onReligionFounded(self, argsList):
		if AutologOpt.isLogReligion():
			iReligion, iFounder = argsList
			CyPlayer = GC.getPlayer(iFounder)
			if GC.getTeam(GC.getPlayer(GAME.getActivePlayer()).getTeam()).isHasMet(CyPlayer.getTeam()):
				messageEnd = CyPlayer.getCity(GAME.getHolyCity(iReligion).getID()).getName()
			else:
				messageEnd = BugUtil.getPlainText("TXT_KEY_AUTOLOG_DISTANT_LAND")
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_RELIGION_FOUNDED", (GC.getReligionInfo(iReligion).getDescription(), messageEnd))
			Logger.writeLog(message, vColor="DarkOrange")

	def onReligionSpread(self, argsList):
		if AutologOpt.isLogReligion():
			iReligion, iOwner, CyCity = argsList
			iActivePlayer = GAME.getActivePlayer()

			if iOwner == iActivePlayer or GAME.getHolyCity(iReligion).getOwner() == iActivePlayer:
				if iOwner == iActivePlayer:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_RELIGION_SPREAD_IN", (GC.getReligionInfo(iReligion).getDescription(), CyCity.getName()))
				else:
					CyPlayer = GC.getPlayer(iOwner)
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_RELIGION_SPREAD_OUT", (GC.getReligionInfo(iReligion).getDescription(), CyCity.getName(), CyPlayer.getCivilizationDescription(0)))
				Logger.writeLog(message, vColor="DarkOrange")

	def onReligionRemove(self, argsList):
		if AutologOpt.isLogReligion():
			iReligion, iOwner, CyCity = argsList
			iActivePlayer = GAME.getActivePlayer()

			if iOwner == iActivePlayer or GAME.getHolyCity(iReligion).getOwner() == iActivePlayer:
				if iOwner == iActivePlayer:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_RELIGION_REMOVED_IN", (GC.getReligionInfo(iReligion).getDescription(), CyCity.getName()))
				else:
					CyPlayer = GC.getPlayer(iOwner)
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_RELIGION_REMOVED_OUT", (GC.getReligionInfo(iReligion).getDescription(), CyCity.getName(), CyPlayer.getCivilizationDescription(0)))
				Logger.writeLog(message, vColor="DarkOrange")

	def onCorporationFounded(self, argsList):
		if AutologOpt.isLogCorporation():
			iCorporation, iFounder = argsList
			CyPlayer = GC.getPlayer(iFounder)
			if GC.getTeam(GC.getPlayer(GAME.getActivePlayer()).getTeam()).isHasMet(CyPlayer.getTeam()):
				messageEnd = CyPlayer.getCity(GAME.getHeadquarters(iCorporation).getID()).getName()
			else:
				messageEnd = BugUtil.getPlainText("TXT_KEY_AUTOLOG_DISTANT_LAND")
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CORP_FOUNDED", (GC.getCorporationInfo(iCorporation).getDescription(), messageEnd))
			Logger.writeLog(message, vColor="DarkOrange")

	def onCorporationSpread(self, argsList):
		if AutologOpt.isLogCorporation():
			iCorporation, iOwner, CyCity = argsList
			iActivePlayer = GAME.getActivePlayer()

			if iOwner == iActivePlayer or GAME.getHeadquarters(iCorporation).getOwner() == iActivePlayer:
				if iOwner == iActivePlayer:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CORP_SPREAD_IN", (GC.getCorporationInfo(iCorporation).getDescription(), CyCity.getName()))
				else:
					CyPlayer = GC.getPlayer(iOwner)
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CORP_SPREAD_OUT", (GC.getCorporationInfo(iCorporation).getDescription(), CyCity.getName(), CyPlayer.getCivilizationDescription(0)))
				Logger.writeLog(message, vColor="DarkOrange")

	def onCorporationRemove(self, argsList):
		if (AutologOpt.isLogCorporation()):
			iCorporation, iOwner, CyCity = argsList
			iActivePlayer = GAME.getActivePlayer()

			if iOwner == iActivePlayer or GAME.getHeadquarters(iCorporation).getOwner() == iActivePlayer:
				if iOwner == iActivePlayer:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CORP_REMOVED_IN", (GC.getCorporationInfo(iCorporation).getDescription(), CyCity.getName()))
				else:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CORP_REMOVED_OUT", (GC.getCorporationInfo(iCorporation).getDescription(), CyCity.getName(), player.getCivilizationDescription(0)))
				Logger.writeLog(message, vColor="DarkOrange")

	def onGoldenAge(self, argsList):
		if (AutologOpt.isLogGoldenAge()):
			iPlayer = argsList[0]
			if iPlayer == GAME.getActivePlayer():
				message = BugUtil.getPlainText("TXT_KEY_AUTOLOG_GOLDENAGE_BEGINS")
				Logger.writeLog(message, vColor="Brown")

	def onEndGoldenAge(self, argsList):
		if (AutologOpt.isLogGoldenAge()):
			iPlayer = argsList[0]
			if iPlayer == GAME.getActivePlayer():
				message = BugUtil.getPlainText("TXT_KEY_AUTOLOG_GOLDENAGE_ENDS")
				Logger.writeLog(message, vColor="Brown")

	def onChangeWar(self, argsList):
		bIsWar = argsList[0]
		iPlayer = argsList[1]
		iRivalTeam = argsList[2]
		#RevolutionDCM start - autolog fix
		if (GAME.isFinalInitialized() and AutologOpt.isLogWar()):

			if( GC.getPlayer(iPlayer).isAlive() and GC.getTeam(iRivalTeam).isAlive() ) :
#				Civ1 declares war on Civ2
				iCiv1 = iPlayer
				iCiv2 = GC.getTeam(iRivalTeam).getLeaderID()
				zsCiv1 = GC.getPlayer(iCiv1).getName() + " (" + GC.getPlayer(iCiv1).getCivilizationShortDescription(0) + ")"
				zsCiv2 = GC.getPlayer(iCiv2).getName() + " (" + GC.getPlayer(iCiv2).getCivilizationShortDescription(0) + ")"

				if (GC.getTeam(GC.getPlayer(iCiv1).getTeam()).isHasMet(GC.getActivePlayer().getTeam())
				and GC.getTeam(GC.getPlayer(iCiv2).getTeam()).isHasMet(GC.getActivePlayer().getTeam())):
					if (bIsWar):
						message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DECLARES_WAR", (zsCiv1, zsCiv2))
						Logger.writeLog(message, vColor="Red")
					else:
						message = TRNSLTR.getText("TXT_KEY_AUTOLOG_PEACE_TREATY", (zsCiv1, zsCiv2))
						Logger.writeLog(message, vColor="DarkRed")
		#RevolutionDCM end - autolog fix

	def onSetPlayerAlive(self, argsList):
		if AutologOpt.isLogWar():
			iPlayer = argsList[0]
			bAlive = argsList[1]
			if not bAlive:
				if GC.getTeam(GC.getPlayer(iPlayer).getTeam()).isHasMet(GC.getActivePlayer().getTeam()):
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CIV_ELIMINATED", (GC.getPlayer(iPlayer).getCivilizationDescription(0),))
				else:
					message = BugUtil.getPlainText("TXT_KEY_AUTOLOG_ANOTHER_CIV_ELIMINATED")

				Logger.writeLog(message, vColor="Red")

	def onCityBuilt(self, argsList):
		if AutologOpt.isLogCityFounded():
			CyCity = argsList[0]
			if CyCity.getOwner() == GAME.getActivePlayer():
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_FOUNDED", (CyCity.getName(),))
				Logger.writeLog(message, vColor="RoyalBlue")

	def onCityRazed(self, argsList):
		if AutologOpt.isLogCityRazed():
			CyCity, iPlayer = argsList
			iActivePlayer = GAME.getActivePlayer()

			if iPlayer == iActivePlayer:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_RAZED", (CyCity.getName(),))
				Logger.writeLog(message, vColor="RoyalBlue")

			elif CyCity.getOwner() == iActivePlayer:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_RAZED_BY", (CyCity.getName(), GC.getPlayer(iPlayer).getCivilizationDescription(0)))
				Logger.writeLog(message, vColor="RoyalBlue")

	def onCityAcquired(self, argsList):
		if AutologOpt.isLogCityOwner():
			iOwnerOld, iOwnerNew, CyCity, bConquest, bTrade = argsList
			if iOwnerNew == GAME.getActivePlayer():
				szCity = CyCity.getName()
				szCiv = GC.getPlayer(iOwnerOld).getName()
				if bConquest:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_CAPTURED", (szCity, szCiv))
				elif bTrade:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_TRADED", (szCity, szCiv))
				else:
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_FLIPPED", (szCity, szCiv))

				Logger.writeLog(message, vColor="RoyalBlue")

	def onCityLost(self, argsList):
		if (AutologOpt.isLogCityOwner()):
			pCity = argsList[0]
			if pCity.getOwner() == GAME.getActivePlayer():
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_LOST", (pCity.getName(), ))
				Logger.writeLog(message, vColor="RoyalBlue")

	def onCultureExpansion(self, argsList):
		if (AutologOpt.isLogCityBorders()):
			pCity = argsList[0]
			iPlayer = argsList[1]
			if pCity.getOwner() == GAME.getActivePlayer():
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_EXPANDED", (pCity.getName(), ))
				Logger.writeLog(message, vColor="RoyalBlue")

	def onCityGrowth(self, argsList):
		if (AutologOpt.isLogCityGrowth()):
			pCity = argsList[0]
			iPlayer = argsList[1]
			if pCity.getOwner() == GAME.getActivePlayer():
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_GROWS", (pCity.getName(), pCity.getPopulation()))
				Logger.writeLog(message, vColor="RoyalBlue")

	def onCityBuildingUnit(self, argsList):
		if (AutologOpt.isLogBuildStarted()):
			pCity = argsList[0]
			iUnitType = argsList[1]
			if pCity.getOwner() == GAME.getActivePlayer():
				zTurns = pCity.getUnitProductionTurnsLeft(iUnitType, 1)
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_PRODUCES_UNIT", (pCity.getName(),GC.getUnitInfo(iUnitType).getDescription(), zTurns))
				Logger.writeLog(message, vColor="Purple")

	def onCityBuildingBuilding(self, argsList):
		if (AutologOpt.isLogBuildStarted()):
			pCity = argsList[0]
			iBuildingType = argsList[1]
			if pCity.getOwner() == GAME.getActivePlayer():
				zTurns = pCity.getBuildingProductionTurnsLeft(iBuildingType, 1)
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CITY_PRODUCES_BUILDING", (pCity.getName(),GC.getBuildingInfo(iBuildingType).getDescription(), zTurns))
				Logger.writeLog(message, vColor="Purple")

	def onImprovementBuilt(self, argsList):
		iImprovement, iX, iY = argsList

		if iImprovement in (-1, GC.getInfoTypeForString("IMPROVEMENT_GOODY_HUT"), GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS")):
			return
		if not AutologOpt.isLogImprovements(): return

		if CyMap().plot(iX, iY).getOwner() == GAME.getActivePlayer():
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_IMPROVEMENT_BUILT", (GC.getImprovementInfo(iImprovement).getDescription(),))
			zsLocn = ""
			for CyPlot in CyMap().plot(iX, iY).rect(2, 2):
				if CyPlot.isCity():
					CyCity = CyPlot.getPlotCity()
					zsLocn = TRNSLTR.getText("TXT_KEY_AUTOLOG_NEAR", (CyCity.getName(),))

			message += zsLocn
			Logger.writeLog(message, vColor="RoyalBlue")

	def onImprovementDestroyed(self, argsList):
		iImprovement, iOwner, iX, iY = argsList

		if iImprovement in (-1, GC.getInfoTypeForString("IMPROVEMENT_GOODY_HUT"), GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS")):
			return
		if not AutologOpt.isLogImprovements(): return

		if CyMap().plot(iX, iY).getOwner() == GAME.getActivePlayer():
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_IMPROVEMENT_DESTROYED", (GC.getImprovementInfo(iImprovement).getDescription(),))
			zsLocn = ""
			for CyPlot in CyMap().plot(iX, iY).rect(2, 2):
				if CyPlot.isCity():
					CyCity = CyPlot.getPlotCity()
					zsLocn = TRNSLTR.getText("TXT_KEY_AUTOLOG_NEAR", (CyCity.getName(),))

			message += zsLocn
			Logger.writeLog(message, vColor="RoyalBlue")

	def onUnitPillage(self, argsList):
		if not AutologOpt.isLogPillage(): return
		CyUnit, iImprovement, iRoute, iPlayer = argsList
		CyPlot = CyUnit.plot()
		iActivePlayer = GAME.getActivePlayer()

		if CyPlot.getOwner() == iActivePlayer or CyUnit.getOwner() == iActivePlayer:
			if iImprovement != -1:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_IMPROVEMENT", (GC.getImprovementInfo(iImprovement).getDescription(),))
			elif iRoute != -1:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_ROUTE", (GC.getRouteInfo(iRoute).getDescription(),))
			else:
				message = BugUtil.getPlainText("TXT_KEY_AUTOLOG_IMPROVEMENT_UNKNOWN")
			szText = ""
			for plotX in CyPlot.rect(2, 2):
				if plotX.isCity():
					CyCity = plotX.getPlotCity()
					szText = TRNSLTR.getText("TXT_KEY_AUTOLOG_NEAR", (CyCity.getName(),))

			message += szText

			global iLastPillageGold
			szCiv = GC.getPlayer(iPlayer).getCivilizationAdjective(0)
			if iLastPillageGold is None or not self.bHumanPlaying:
				message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_IMPROVEMENT_DESTROYED_BY_NOGOLD", (szCiv, CyUnit.getName()))
			else:
				message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_IMPROVEMENT_DESTROYED_BY_GOLD", (szCiv, CyUnit.getName(), iLastPillageGold))
			iLastPillageGold = None

			if self.bHumanPlaying:
				Logger.writeLog(message, vColor="DarkRed")
			else:
				Logger.writeLog(message, vColor="Red")

	def onVassalState(self, argsList):
		'Vassal State'
		iMaster, iVassal, bVassal = argsList

		if (AutologOpt.isLogVassals()
		and GC.getTeam(iMaster).isHasMet(GC.getActivePlayer().getTeam())
		and GC.getTeam(iVassal).isHasMet(GC.getActivePlayer().getTeam())):

			zsMaster = GC.getTeam(iMaster).getName()
			zsVassal = GC.getTeam(iVassal).getName()

			if (bVassal):
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_VASSAL_BECOMES", (zsVassal, zsMaster))
			else:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_VASSAL_REVOLTS", (zsVassal, zsMaster))

			Logger.writeLog(message, vColor="Red")

	def onDealOffered(self, argsList):
		eOfferPlayer, eTargetPlayer, pTrade = argsList
		if AutologOpt.isLogTradeOffer():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pOfferPlayer = GC.getPlayer(eOfferPlayer)
			szOfferItems = ""
			szTargetItems = ""
			message = ""
			if eOfferPlayer == pTrade.getPlayer():
				for i in xrange(pTrade.getCount()):
					szOfferItems = szOfferItems + TradeUtil.format(eOfferPlayer, pTrade.getTrade(i)) + ", "
				for i in xrange(pTrade.getOtherCount()):
					szTargetItems = szTargetItems + TradeUtil.format(eTargetPlayer, pTrade.getOtherTrade(i)) + ", "
			else:
				for i in xrange(pTrade.getOtherCount()):
					szOfferItems = szOfferItems + TradeUtil.format(eOfferPlayer, pTrade.getOtherTrade(i)) + ", "
				for i in xrange(pTrade.getCount()):
					szTargetItems = szTargetItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i)) + ", "
			szOfferItems = szOfferItems.rstrip(", ")
			szTargetItems = szTargetItems.rstrip(", ")
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_DEAL_OFFER",
									(pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0),
									szOfferItems,
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									szTargetItems))
			Logger.writeLog(message, vColor="Navy")

	def onCityOffered(self, argsList):
		eOfferPlayer, eTargetPlayer, iCityID = argsList
		if AutologOpt.isLogTradeOffer():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pOfferPlayer = GC.getPlayer(eOfferPlayer)
			pCityOffered = pOfferPlayer.getCity(iCityID)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_CITY_OFFER",
									(pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0),
									pCityOffered.getName(),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0)))
			Logger.writeLog(message, vColor="Navy")

	def onHelpOffered(self, argsList):
		eOfferPlayer, eTargetPlayer, pTrade = argsList
		if AutologOpt.isLogTradeOffer():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pOfferPlayer = GC.getPlayer(eOfferPlayer)
			szOfferItems = ""
			message = ""
			if eOfferPlayer == pTrade.getPlayer():
				for i in xrange(pTrade.getCount()):
					szOfferItems = szOfferItems + TradeUtil.format(eOfferPlayer, pTrade.getTrade(i)) + ", "
			else:
				for i in xrange(pTrade.getOtherCount()):
					szOfferItems = szOfferItems + TradeUtil.format(eOfferPlayer, pTrade.getOtherTrade(i)) + ", "
			szOfferItems = szOfferItems.rstrip(", ")
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_HELP_OFFER",
									(pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0),
									szOfferItems,
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0)))
			Logger.writeLog(message, vColor="Navy")

	def onPeaceOffered(self, argsList):
		eOfferPlayer, eTargetPlayer = argsList
		if AutologOpt.isLogTradeOffer():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pOfferPlayer = GC.getPlayer(eOfferPlayer)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_PEACE_OFFER",
									(pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0)))
			Logger.writeLog(message, vColor="Navy")

	def onVassalOffered(self, argsList):
		eOfferPlayer, eTargetPlayer = argsList
		if AutologOpt.isLogTradeOffer():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pOfferPlayer = GC.getPlayer(eOfferPlayer)
			message = ""
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_VASSAL_OFFER",
									(pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0)))
			Logger.writeLog(message, vColor="Navy")

	def onDealCanceled(self, argsList):
		eOfferPlayer, eTargetPlayer, pTrade = argsList
		if AutologOpt.isLogTradeOffer() and pTrade != None:
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pOfferPlayer = GC.getPlayer(eOfferPlayer)
			szOfferItems = ""
			szTargetItems = ""
			message = ""
			if eOfferPlayer == pTrade.getPlayer():
				for i in xrange(pTrade.getCount()):
					szOfferItems = szOfferItems + TradeUtil.format(eOfferPlayer, pTrade.getTrade(i)) + ", "
				for i in xrange(pTrade.getOtherCount()):
					szTargetItems = szTargetItems + TradeUtil.format(eTargetPlayer, pTrade.getOtherTrade(i)) + ", "
			else:
				for i in xrange(pTrade.getOtherCount()):
					szOfferItems = szOfferItems + TradeUtil.format(eOfferPlayer, pTrade.getOtherTrade(i)) + ", "
				for i in xrange(pTrade.getCount()):
					szTargetItems = szTargetItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i)) + ", "
			szOfferItems = szOfferItems.rstrip(", ")
			szTargetItems = szTargetItems.rstrip(", ")
			if szTargetItems:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_DEAL_CANCEL",
										(pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0),
										szOfferItems,
										pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
										szTargetItems))
			else:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GIFT_CANCEL",
										(pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0),
										szOfferItems,
										pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0)))
			Logger.writeLog(message, vColor="Red")

	def onDealAccepted(self, argsList):
		eTargetPlayer, eOfferPlayer, pTrade = argsList
		if AutologOpt.isLogTradeOffer():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pOfferPlayer = GC.getPlayer(eOfferPlayer)
			szOfferItems = ""
			szTargetItems = ""
			message = ""
			if eOfferPlayer == pTrade.getPlayer():
				for i in xrange(pTrade.getCount()):
					szOfferItems = szOfferItems + TradeUtil.format(eOfferPlayer, pTrade.getTrade(i)) + ", "
				for i in xrange(pTrade.getOtherCount()):
					szTargetItems = szTargetItems + TradeUtil.format(eTargetPlayer, pTrade.getOtherTrade(i)) + ", "
			else:
				for i in xrange(pTrade.getOtherCount()):
					szOfferItems = szOfferItems + TradeUtil.format(eOfferPlayer, pTrade.getOtherTrade(i)) + ", "
				for i in xrange(pTrade.getCount()):
					szTargetItems = szTargetItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i)) + ", "
			szOfferItems = szOfferItems.rstrip(", ")
			szTargetItems = szTargetItems.rstrip(", ")
			if szTargetItems:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_DEAL_ACCEPT",
										(pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
										szTargetItems,
										pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0),
										szOfferItems))
			else:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GIFT_ACCEPT",
										(pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
										szOfferItems,
										pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0)))
			Logger.writeLog(message, vColor="Green")

	def onDealRejected(self, argsList):
		eTargetPlayer, eOfferPlayer, pTrade = argsList
		if AutologOpt.isLogTradeOffer():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pOfferPlayer = GC.getPlayer(eOfferPlayer)
			szOfferItems = ""
			szTargetItems = ""
			message = ""
			if eOfferPlayer == pTrade.getPlayer():
				for i in xrange(pTrade.getCount()):
					szOfferItems = szOfferItems + TradeUtil.format(eOfferPlayer, pTrade.getTrade(i)) + ", "
				for i in xrange(pTrade.getOtherCount()):
					szTargetItems = szTargetItems + TradeUtil.format(eTargetPlayer, pTrade.getOtherTrade(i)) + ", "
			else:
				for i in xrange(pTrade.getOtherCount()):
					szOfferItems = szOfferItems + TradeUtil.format(eOfferPlayer, pTrade.getOtherTrade(i)) + ", "
				for i in xrange(pTrade.getCount()):
					szTargetItems = szTargetItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i)) + ", "
			szOfferItems = szOfferItems.rstrip(", ")
			szTargetItems = szTargetItems.rstrip(", ")
			if szTargetItems:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_DEAL_REJECT",
										(pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
										szTargetItems,
										pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0),
										szOfferItems))
			else:
				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GIFT_REJECT",
										(pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
										szOfferItems,
										pOfferPlayer.getName(), pOfferPlayer.getCivilizationShortDescription(0)))
			Logger.writeLog(message, vColor="Red")

	def onHelpDemanded(self, argsList):
		eDemandPlayer, eTargetPlayer, pTrade = argsList
		if AutologOpt.isLogTributeDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			szItems = ""
			message = ""
			for i in xrange(pTrade.getCount()):
				szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i)) + ", "
			szItems = szItems.rstrip(", ")
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_HELP_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									szItems))
			Logger.writeLog(message, vColor="Navy")

	def onHelpAccepted(self, argsList):
		eTargetPlayer, eDemandPlayer, pTrade = argsList
		if AutologOpt.isLogTributeDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			szItems = ""
			message = ""
			for i in xrange(pTrade.getCount()):
				szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i)) + ", "
			szItems = szItems.rstrip(", ")
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_HELP_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									szItems))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_ACCEPT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Green")

	def onHelpRejected(self, argsList):
		eTargetPlayer, eDemandPlayer, pTrade = argsList
		if AutologOpt.isLogTributeDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			szItems = ""
			message = ""
			for i in xrange(pTrade.getCount()):
				szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i)) + ", "
			szItems = szItems.rstrip(", ")
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_HELP_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									szItems))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_REJECT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Red")

	def onTributeDemanded(self, argsList):
		eDemandPlayer, eTargetPlayer, pTrade = argsList
		if AutologOpt.isLogTributeDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			szItems = ""
			message = ""
			for i in xrange(pTrade.getCount()):
				szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i)) + ", "
			szItems = szItems.rstrip(", ")
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_TRIBUTE_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									szItems))
			Logger.writeLog(message, vColor="Navy")

	def onTributeAccepted(self, argsList):
		eTargetPlayer, eDemandPlayer, pTrade = argsList
		if AutologOpt.isLogTributeDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			szItems = ""
			message = ""
			for i in xrange(pTrade.getCount()):
				szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i)) + ", "
			szItems = szItems.rstrip(", ")
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_TRIBUTE_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									szItems))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_ACCEPT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Green")

	def onTributeRejected(self, argsList):
		eTargetPlayer, eDemandPlayer, pTrade = argsList
		if AutologOpt.isLogTributeDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			szItems = ""
			message = ""
			for i in xrange(pTrade.getCount()):
				szItems = szItems + TradeUtil.format(eTargetPlayer, pTrade.getTrade(i)) + ", "
			szItems = szItems.rstrip(", ")
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_TRIBUTE_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									szItems))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_REJECT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Red")

	def onReligionDemanded(self, argsList):
		eDemandPlayer, eTargetPlayer, eReligion = argsList
		if AutologOpt.isLogReligionDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_RELIGION_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									GC.getReligionInfo(eReligion).getDescription()))
			Logger.writeLog(message, vColor="Navy")

	def onReligionAccepted(self, argsList):
		eTargetPlayer, eDemandPlayer, eReligion = argsList
		if AutologOpt.isLogReligionDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_RELIGION_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									GC.getReligionInfo(eReligion).getDescription()))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_ACCEPT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Green")

	def onReligionRejected(self, argsList):
		eTargetPlayer, eDemandPlayer, eReligion = argsList
		if AutologOpt.isLogReligionDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_RELIGION_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									GC.getReligionInfo(eReligion).getDescription()))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_REJECT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Red")

	def onCivicDemanded(self, argsList):
		eDemandPlayer, eTargetPlayer, eCivic = argsList
		if AutologOpt.isLogCivicDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_CIVIC_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									GC.getCivicInfo(eCivic).getDescription()))
			Logger.writeLog(message, vColor="Navy")

	def onCivicAccepted(self, argsList):
		eTargetPlayer, eDemandPlayer, eCivic = argsList
		if AutologOpt.isLogCivicDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_CIVIC_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									GC.getCivicInfo(eCivic).getDescription()))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_ACCEPT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Green")

	def onCivicRejected(self, argsList):
		eTargetPlayer, eDemandPlayer, eCivic = argsList
		if AutologOpt.isLogCivicDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_CIVIC_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									GC.getCivicInfo(eCivic).getDescription()))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_REJECT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Red")

	def onWarDemanded(self, argsList):
		eDemandPlayer, eTargetPlayer, eVictim = argsList
		if AutologOpt.isLogWarDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			pVictim = GC.getPlayer(eVictim)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_WAR_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									pVictim.getName(), pVictim.getCivilizationShortDescription(0)))
			Logger.writeLog(message, vColor="Navy")

	def onWarAccepted(self, argsList):
		eTargetPlayer, eDemandPlayer, eVictim = argsList
		if AutologOpt.isLogWarDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			pVictim = GC.getPlayer(eVictim)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_WAR_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									pVictim.getName(), pVictim.getCivilizationShortDescription(0)))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_ACCEPT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Green")

	def onWarRejected(self, argsList):
		eTargetPlayer, eDemandPlayer, eVictim = argsList
		if AutologOpt.isLogWarDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			pVictim = GC.getPlayer(eVictim)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_WAR_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									pVictim.getName(), pVictim.getCivilizationShortDescription(0)))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_REJECT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Red")

	def onEmbargoDemanded(self, argsList):
		eDemandPlayer, eTargetPlayer, eVictim = argsList
		if AutologOpt.isLogEmbargoDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			pVictim = GC.getPlayer(eVictim)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_EMBARGO_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									pVictim.getName(), pVictim.getCivilizationShortDescription(0)))
			Logger.writeLog(message, vColor="Navy")

	def onEmbargoAccepted(self, argsList):
		eTargetPlayer, eDemandPlayer, eVictim = argsList
		if AutologOpt.isLogEmbargoDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			pVictim = GC.getPlayer(eVictim)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_EMBARGO_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									pVictim.getName(), pVictim.getCivilizationShortDescription(0)))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_ACCEPT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Green")

	def onEmbargoRejected(self, argsList):
		eTargetPlayer, eDemandPlayer, eVictim = argsList
		if AutologOpt.isLogEmbargoDemand():
			pTargetPlayer = GC.getPlayer(eTargetPlayer)
			pDemandPlayer = GC.getPlayer(eDemandPlayer)
			pVictim = GC.getPlayer(eVictim)
			message = TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_EMBARGO_DEMAND",
									(pDemandPlayer.getName(), pDemandPlayer.getCivilizationShortDescription(0),
									pTargetPlayer.getName(), pTargetPlayer.getCivilizationShortDescription(0),
									pVictim.getName(), pVictim.getCivilizationShortDescription(0)))
			message = message + TRNSLTR.getText("TXT_KEY_AUTOLOG_DIPLO_GENERIC_REJECT", (pTargetPlayer.getName(),))
			Logger.writeLog(message, vColor="Red")

	def initStuff(self):
		#set up variables to hold stuff
		#RevolutionDCM autolog fix
		MAX_PC_PLAYERS = GC.getMAX_PC_PLAYERS()
		iCivicOptions = GC.getNumCivicOptionInfos()

		self.CIVAttitude = [""] * (MAX_PC_PLAYERS * MAX_PC_PLAYERS + MAX_PC_PLAYERS)
		self.CIVCivics = [0] * (MAX_PC_PLAYERS * iCivicOptions + iCivicOptions)
		self.CIVReligion = [-1] * MAX_PC_PLAYERS
		self.CityWhipCounter = [0] * 1000
		self.CityConscriptCounter = [0] * 1000

	def storeStuff(self):
		MAX_PC_PLAYERS = GC.getMAX_PC_PLAYERS()
		iCivicOptions = GC.getNumCivicOptionInfos()
		if not self.CIVReligion:
			self.initStuff()

		for iPlayerX in xrange(MAX_PC_PLAYERS):
			CyPlayerX = GC.getPlayer(iPlayerX)
			# store civ state religion
			self.CIVReligion[iPlayerX] = CyPlayerX.getStateReligion()
			if CyPlayerX.isAlive():
				# store civ attitudes
				for iPlayerY in xrange(MAX_PC_PLAYERS):
					# Don't try to get a civ's attitude with itself
					if iPlayerX == iPlayerY: continue
					if GC.getPlayer(iPlayerY).isAlive():
						zKey = MAX_PC_PLAYERS * iPlayerX + iPlayerY
						self.CIVAttitude[zKey] = GC.getAttitudeInfo(CyPlayerX.AI_getAttitude(iPlayerY)).getDescription()
				# store the civ's civics
				for iCivic in xrange(iCivicOptions):
					zKey = iCivicOptions * iPlayerX + iCivic
					self.CIVCivics[zKey] = CyPlayerX.getCivics(iCivic)

	def storeWhip(self):
		# store the city whip counter
		iPlayer = GC.getActivePlayer()
		for i in xrange(0, iPlayer.getNumCities(), 1):
			iCity = iPlayer.getCity(i)
			self.CityWhipCounter[i] = iCity.getHurryAngerTimer()
			self.CityConscriptCounter[i] = iCity.getConscriptAngerTimer()

	def checkStuff(self):
		if not self.CIVReligion:
			self.storeStuff()

		bLogReligion = AutologOpt.isLogReligion()
		bLogAttitude = AutologOpt.isLogAttitude()
		bLogCivics = AutologOpt.isLogCivics()
		if not (bLogReligion or bLogAttitude or bLogCivics):
			return

		MAX_PC_PLAYERS = GC.getMAX_PC_PLAYERS()
		iCivicOptions = GC.getNumCivicOptionInfos()
		iActivePlayer = GAME.getActivePlayer()
		iActiveTeam = GC.getPlayer(iActivePlayer).getTeam()

		for iPlayerX in xrange(MAX_PC_PLAYERS):

			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isAlive():
				continue
			CyTeamX = GC.getTeam(CyPlayerX.getTeam())
			if not CyTeamX.isHasMet(iActiveTeam):
				continue

			szCivX = CyPlayerX.getName() + "(" + CyPlayerX.getCivilizationShortDescription(0) + ")"
			if bLogReligion:
				# check if civ state religion has changed
				iStateReligion = CyPlayerX.getStateReligion()
				if self.CIVReligion[iPlayerX] != iStateReligion:
					if self.CIVReligion[iPlayerX] == -1:
						zsOldRel = BugUtil.getPlainText("TXT_KEY_AUTOLOG_NO_STATE_RELIGION")
					else:
						zsOldRel = GC.getReligionInfo(self.CIVReligion[iPlayerX]).getDescription()
					if iStateReligion == -1:
						zsNewRel = BugUtil.getPlainText("TXT_KEY_AUTOLOG_NO_STATE_RELIGION")
					else:
						zsNewRel = GC.getReligionInfo(iStateReligion).getDescription()
					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_RELIGION_CHANGE", (szCivX, zsOldRel, zsNewRel))
					Logger.writeLog(message, vColor="DarkOrange")

			if bLogAttitude:
				# check if the attitude has changed
				for iPlayerY in xrange(MAX_PC_PLAYERS):
					if iPlayerX in (iPlayerY, iActivePlayer): continue
					CyPlayerY = GC.getPlayer(iPlayerY)
					if not (CyPlayerY.isAlive() and GC.getTeam(CyPlayerY.getTeam()).isHasMet(iActiveTeam)):
						continue
					zKey = MAX_PC_PLAYERS * iPlayerX + iPlayerY
					zsNewAttitude = GC.getAttitudeInfo(CyPlayerX.AI_getAttitude(iPlayerY)).getDescription()
					if self.CIVAttitude[zKey] != zsNewAttitude:
						zsCiv2 = CyPlayerY.getName() + " (" + CyPlayerY.getCivilizationShortDescription(0) + ")"
						message = TRNSLTR.getText("TXT_KEY_AUTOLOG_ATTITUDE_CHANGE", (szCivX, zsCiv2, self.CIVAttitude[zKey], zsNewAttitude))
						Logger.writeLog(message, vColor="Blue")

			if bLogCivics:
				# check if the civ's civics has changed
				for iCivic in xrange(iCivicOptions):
					zKey = iCivicOptions * iPlayerX + iCivic
					if self.CIVCivics[zKey] != CyPlayerX.getCivics(iCivic):
						zsOldCiv = GC.getCivicInfo(self.CIVCivics[zKey]).getDescription()
						zsNewCiv = GC.getCivicInfo(CyPlayerX.getCivics(iCivic)).getDescription()
						message = TRNSLTR.getText("TXT_KEY_AUTOLOG_CIVIC_CHANGE", (szCivX, zsOldCiv, zsNewCiv))
						Logger.writeLog(message, vColor="SeaGreen")


	def logSliders(self):
		if not AutologOpt.isLogSliders():
			return

		pPlayer = GC.getPlayer(GAME.getActivePlayer())
		for iI in xrange( CommerceTypes.NUM_COMMERCE_TYPES ):
			eCommerce = (iI + 1) % CommerceTypes.NUM_COMMERCE_TYPES

			zDesc = GC.getCommerceInfo(CommerceTypes(eCommerce)).getDescription()
			if (eCommerce == CommerceTypes.COMMERCE_GOLD):
				zPercent = pPlayer.getCommercePercent(eCommerce)
				zRate = pPlayer.calculateGoldRate()
				zTotal = pPlayer.getGold()

				message = TRNSLTR.getText("TXT_KEY_AUTOLOG_COMMERCE_GOLD_SLIDERS", (zPercent, zDesc, zRate, zTotal))
				Logger.writeLog(message, vColor="Blue")
			else:
				if pPlayer.isCommerceFlexible(eCommerce):
					zPercent = pPlayer.getCommercePercent(eCommerce)
					zRate = pPlayer.getCommerceRate(CommerceTypes(eCommerce))
					zTotal = pPlayer.getGold()

					message = TRNSLTR.getText("TXT_KEY_AUTOLOG_COMMERCE_OTHER_SLIDERS", (zPercent, zDesc, zRate))
					Logger.writeLog(message, vColor="Blue")
