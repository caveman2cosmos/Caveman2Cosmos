#
# AI_AUTO_PLAY_MOD
#
from CvPythonExtensions import *
import RevDefs
import SdToolKit
import RevUtils

# globals
GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

def init():
	import BugCore
	global RevOpt
	RevOpt = BugCore.game.Revolution
init()

class AIAutoPlay :

	def __init__(self, customEM, RevOpt=None):

		print "Initializing AIAutoPlay Mod"

		self.LOG_DEBUG = False
		self.blockPopups = True

		self.refortify = True
		self.bSaveAllDeaths = True
		self.bEnableNextTurnArray = []
		self.bDisableNextTurnArray = []
		self.bDisableNextTurnArray2 = []
		self.NumberOfTurns = []
		self.TurnsToAuto = []
		for i in range(GC.getMAX_PC_PLAYERS()):
			self.bEnableNextTurnArray.append(0)
			self.bDisableNextTurnArray.append(0)
			self.bDisableNextTurnArray2.append(0)
			self.NumberOfTurns.append(0)
			self.TurnsToAuto.append(10)

		if RevOpt:
			self.LOG_DEBUG = RevOpt.isRevDebugMode()
			self.blockPopups = RevOpt.isBlockPopups()
			self.refortify = RevOpt.isRefortify()
			self.bSaveAllDeaths = RevOpt.isSaveAllDeaths()

		customEM.addEventHandler("kbdEvent", self.onKbdEvent)
		customEM.addEventHandler("ModNetMessage", self.onModNetMessage)
		customEM.addEventHandler('BeginPlayerTurn', self.onBeginPlayerTurn)
		customEM.addEventHandler('EndPlayerTurn', self.onEndPlayerTurn)
		customEM.addEventHandler('OnLoad', self.onGameLoad)
		customEM.addEventHandler('GameStart', self.onGameStart)
		customEM.addEventHandler('victory', self.onVictory)
		customEM.addEventHandler("BeginGameTurn", self.onBeginGameTurn)

		customEM.setPopupHandler(RevDefs.toAIChooserPopup, ["toAIChooserPopup",self.AIChooserHandler,self.blankHandler])
		customEM.setPopupHandler(RevDefs.abdicatePopup, ["abdicatePopup",self.blankHandler,self.blankHandler])

		self.customEM = customEM


	def removeEventHandlers(self):
		print "Removing event handlers from AIAutoPlay"

		self.customEM.removeEventHandler("kbdEvent", self.onKbdEvent)
		self.customEM.removeEventHandler("ModNetMessage", self.onModNetMessage)
		self.customEM.removeEventHandler('BeginPlayerTurn', self.onBeginPlayerTurn)
		self.customEM.removeEventHandler('EndPlayerTurn', self.onEndPlayerTurn)
		self.customEM.removeEventHandler('OnLoad', self.onGameLoad)
		self.customEM.removeEventHandler('GameStart', self.onGameStart)
		self.customEM.removeEventHandler('victory', self.onVictory)

		self.customEM.setPopupHandler(RevDefs.toAIChooserPopup, ["toAIChooserPopup",self.blankHandler,self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.abdicatePopup, ["abdicatePopup",self.blankHandler,self.blankHandler])

		self.customEM.removeEventHandler("BeginGameTurn", self.onBeginGameTurn)


	# Dummy handler to take the second event for popup
	def blankHandler(self, playerID, netUserData, popupReturn): return

	def onGameStart(self, argsList):
		self.onGameLoad([])

	def onGameLoad(self, argsList):
		# Init things which require a game object or other game data to exist

		if not SdToolKit.sdObjectExists("AIAutoPlay", GAME):
			SdToolKit.sdObjectInit("AIAutoPlay", GAME, {})
			SdToolKit.sdObjectSetVal("AIAutoPlay", GAME, "bCanCancelAuto", True)

		elif SdToolKit.sdObjectGetVal("AIAutoPlay", GAME, "bCanCancelAuto") is None:
			SdToolKit.sdObjectSetVal("AIAutoPlay", GAME, "bCanCancelAuto", True)

		for i in range(GC.getMAX_PC_PLAYERS()):
			CyPlayer = GC.getPlayer(i)
			if CyPlayer.isHumanDisabled() and not GAME.isForcedAIAutoPlay(i):
				GAME.setAIAutoPlay(i, 1)

	def onVictory(self, argsList):
		self.checkPlayer()
		for i in range(GC.getMAX_PC_PLAYERS()):
			if GC.getPlayer(i).isHumanDisabled():
				GAME.setForcedAIAutoPlay(i, 0, False)


	def onBeginPlayerTurn(self, argsList):
		iGameTurn, iPlayer = argsList
		iPlayerAct = GAME.getActivePlayer()
		CyPlayerAct = GC.getActivePlayer()

		if GAME.getAIAutoPlay(iPlayer) == 1 and CyPlayerAct and iPlayer > iPlayerAct and CyPlayerAct.isAlive():
			# Forces isHuman checks to come through positive for everything after human players turn
			self.checkPlayer()

		elif self.bSaveAllDeaths and GAME.getAIAutoPlay(iPlayerAct) == 0 and CyPlayerAct and not CyPlayerAct.isAlive() and iPlayer > iPlayerAct:
			self.checkPlayer()


	def onEndPlayerTurn(self, argsList):
		iGameTurn, iPlayer = argsList
		# Can't use isHuman as isHuman has been deactivated by automation
		if self.refortify and iPlayer == GAME.getActivePlayer() and GAME.getAIAutoPlay(iPlayer) == 1:
			RevUtils.doRefortify(iPlayer)


	def checkPlayer(self):
		CyPlayer = GC.getActivePlayer()
		for i in range(GC.getMAX_PC_TEAMS()):
			CyPlayer.setEspionageSpendingWeightAgainstTeam(i, CyPlayer.getEspionageSpendingWeightAgainstTeam(i)/10)


	# keypress handler
	def onKbdEvent(self, argsList):
		eventType, key, mx, my, px, py = argsList

		if eventType == 6 and self.customEM.bShift and self.customEM.bCtrl:

			if key == InputTypes.KB_X:

				iPlayer = GAME.getActivePlayer()

				if GAME.getAIAutoPlay(iPlayer) > 0:
					try:
						bCanCancelAuto = SdToolKit.sdObjectGetVal("AIAutoPlay", GAME, "bCanCancelAuto")
						if bCanCancelAuto is None:
							bCanCancelAuto = True
							SdToolKit.sdObjectSetVal("AIAutoPlay", GAME, "bCanCancelAuto", True)
					except:
						print "Error! AIAutoPlay: Can't find bCanCancelAuto, assuming it would be True"
						bCanCancelAuto = True

					if bCanCancelAuto:
						if self.refortify:
							RevUtils.doRefortify(iPlayer)
							self.disableMultiCheck(iPlayer)
						self.checkPlayer()
				else:
					self.toAIChooser()

			elif key == InputTypes.KB_M:
				# Toggle auto moves
				if self.LOG_DEBUG: CyInterface().addImmediateMessage(TRNSLTR.getText("TXT_KEY_AIAUTOPLAY_MOVING", ()),"")
				GAME.setAIAutoPlay(GAME.getActivePlayer(), 1)

			elif key == InputTypes.KB_O:
				RevUtils.doRefortify(GAME.getActivePlayer())


	def doCheckMultiplayerUpdate(self):
		if not GAME.isMultiplayer():
			return

		for i in range(GC.getMAX_PC_PLAYERS()):
			if self.bEnableNextTurnArray[i] == 1:
				self.bEnableNextTurnArray[i] = 0
				self.abdicate(i, self.NumberOfTurns[i])

			elif self.bDisableNextTurnArray[i] == 1:
				self.bDisableNextTurnArray[i] = 0
				self.bDisableNextTurnArray2[i] = 1

			elif self.bDisableNextTurnArray2[i] == 1:
				self.bDisableNextTurnArray2[i] = 0
				GAME.setAIAutoPlay(i, 0)

	# Called at the beginning of the end of each turn
	def onBeginGameTurn(self, argsList):
		if GAME.isMultiplayer():
			self.doCheckMultiplayerUpdate()

	# Chooser window for when user switches to AI auto play
	def toAIChooser(self):
		import CvScreenEnums
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
		xResolution = screen.getXResolution()
		yResolution = screen.getYResolution()
		popupSizeX = 400
		popupSizeY = 250
		if (xResolution > 2500) :
			popupSizeX = 500
			popupSizeY = 350

		popup = CyPopup(RevDefs.toAIChooserPopup, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setPosition((xResolution - popupSizeX )/2, (yResolution-popupSizeY)/2-50)
		popup.setSize(popupSizeX,popupSizeY)
		popup.setHeaderString(TRNSLTR.getText("TXT_KEY_AIAUTOPLAY_TURN_ON", ()), 1<<2)
		popup.setBodyString(TRNSLTR.getText("TXT_KEY_AIAUTOPLAY_TURNS", ()), 1<<0)
		popup.addSeparator()
		popup.createPythonEditBox('10', 'Number of turns to turn over to AI', 0)
		popup.setEditBoxMaxCharCount(4, 2, 0)

		popup.createPythonPullDown(TRNSLTR.getText("TXT_KEY_AIAUTOPLAY_AUTOMATION", ()), 2)
		popup.addPullDownString(TRNSLTR.getText("TXT_KEY_AIAUTOPLAY_NO", ()), 0, 2)
		popup.addPullDownString(TRNSLTR.getText("TXT_KEY_AIAUTOPLAY_FULLY", ()), 1, 2)

		popup.setSelectedPulldownID(1, 2)

		popup.addSeparator()
		popup.addButton("OK")
		popup.addButton(TRNSLTR.getText("TXT_KEY_AIAUTOPLAY_CANCEL", ()))

		popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

	# Handles AIChooser popup
	def AIChooserHandler(self, iPlayer, netUserData, popupReturn):
		if popupReturn.getButtonClicked() == 1: # if you pressed cancel
			return

		if not self.isLocalHumanPlayer(iPlayer):
			return

		numTurns = 0
		if popupReturn.getEditBoxString(0) != '':
			numTurns = int(popupReturn.getEditBoxString(0))

		autoIdx = popupReturn.getSelectedPullDownValue(2)

		if autoIdx == 0:
			if self.LOG_DEBUG: CyInterface().addImmediateMessage(TRNSLTR.getText("TXT_KEY_AIAUTOPLAY_CLEARING_ALL", ()),"")

		elif autoIdx == 1:
			if numTurns > 0:
				if self.LOG_DEBUG: CyInterface().addImmediateMessage(TRNSLTR.getText("TXT_KEY_AIAUTOPLAY_FULLY_AUTO", ()) %numTurns, "")

				self.abdicateMultiCheck(iPlayer, numTurns = numTurns)

		elif autoIdx == 2 and numTurns > 0:
			if self.LOG_DEBUG: CyInterface().addImmediateMessage("Auto Move","")
			self.setAutoMoves(numTurns)
		elif autoIdx == 3 and numTurns > 0:
			if self.LOG_DEBUG: CyInterface().addImmediateMessage("Debug Mode","")
			self.setDebugMode( numTurns )


	# Determines whether to show popup to active player
	def isLocalHumanPlayer(self, iPlayer):
		CyPlayer = GC.getPlayer(iPlayer)
		return (CyPlayer.isHuman() or CyPlayer.isHumanDisabled()) and GAME.getActivePlayer() == iPlayer

	def disableMultiCheck(self, iPlayer):
		if GAME.isMultiplayer():
			self.bDisableNextTurnArray[iPlayer] = 1
			if self.isLocalHumanPlayer(iPlayer):
				CyMessageControl().sendModNetMessage(5656, 0, iPlayer, GAME.getGameTurn(), 0)
		else:
			GAME.setAIAutoPlay(iPlayer, 0)

	def abdicateMultiCheck(self, iPlayer, numTurns = -1):
		if GAME.isMultiplayer():
			self.bEnableNextTurnArray[iPlayer] = 1
			self.bDisableNextTurnArray[iPlayer] = 0
			self.bDisableNextTurnArray2[iPlayer] = 0

			self.NumberOfTurns[iPlayer] = numTurns
			if iPlayer == GAME.getActivePlayer():
				CyMessageControl().sendModNetMessage(5656, 1, iPlayer, numTurns, 0)
		else:
			self.abdicate(iPlayer, numTurns)

	def abdicateMultiCheckNoMessage(self, iPlayer, numTurns = -1):
		if GAME.isMultiplayer():
			self.bEnableNextTurnArray[iPlayer] = 1
			self.bDisableNextTurnArray[iPlayer] = 0
			self.bDisableNextTurnArray2[iPlayer] = 0

			self.NumberOfTurns[iPlayer] = numTurns
		else:
			self.abdicate(iPlayer, numTurns)

	def onModNetMessage(self, argsList):
		ID, iData2, iData3, iData4, iData5 = argsList
		if ID == 5656:
			if iData3 == GAME.getActivePlayer():
				return
			if iData2 == 0:
				iTurn = GAME.getGameTurn()
				if iData4 == iTurn:
					self.bDisableNextTurnArray[iData3] = 1
				elif iData4 < iTurn:
					self.bDisableNextTurnArray2[iData3] = 1
			elif iData2 == 1:
				self.bEnableNextTurnArray[iData3] = 1
				self.NumberOfTurns[iData3] = iData4
				self.TurnsToAuto[iData3] = iData4


	# Turn over control to the AI
	def abdicate(self, iPlayer, numTurns = -1):
		if numTurns > 0:
			self.TurnsToAuto[iPlayer] = numTurns

		if self.TurnsToAuto[iPlayer] > 0:
			GAME.setAIAutoPlay(iPlayer, self.TurnsToAuto[iPlayer])
