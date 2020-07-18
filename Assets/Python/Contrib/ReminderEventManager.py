##-------------------------------------------------------------------
## Modified from reminder by eotinb
## by Ruff and EF
##-------------------------------------------------------------------
## Reorganized to work via CvCustomEventManager
## using Civ4lerts as template.
## CvCustomEventManager & Civ4lerts by Gillmer J. Derge
##-------------------------------------------------------------------
## EF: Turned into a real queue, can be disabled
##-------------------------------------------------------------------

from CvPythonExtensions import *
import CvUtil
import SdToolKit
import ScreenResolution as SR

STORE_EVENT_ID = CvUtil.getNewEventID()
RECALL_EVENT_ID = CvUtil.getNewEventID()

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

g_reminders = None

# Used to display flashing end-of-turn text
g_turnReminderTexts = ""

# Used to receive network messages
g_hasNetMessage = hasattr(CyPlayer, "addReminder")

def hasNetMessage():
	return g_hasNetMessage

def netAddReminder(args):
	playerID, turn, message = args
	g_reminders.push(playerID, Reminder(turn, message))

# Shortcut - Create Reminder
def createReminder(argsList):
	g_eventMgr.beginEvent(STORE_EVENT_ID)

class ReminderEventManager:

	def __init__(self, eventManager):

		global g_eventMgr, g_autolog, ReminderOpt
		g_eventMgr = eventManager
		import autolog
		g_autolog = autolog.autologInstance()
		import BugCore
		ReminderOpt = BugCore.game.Reminder
		# expose to DLL
		import CvAppInterface
		CvAppInterface.netAddReminder = netAddReminder

		self.setReminders(Reminders())
		self.recall = []
		eventManager.addEventHandler("BeginActivePlayerTurn", self.onBeginActivePlayerTurn)
		eventManager.addEventHandler("GameStart", self.onGameStart)
		eventManager.addEventHandler("OnLoad", self.onLoadGame)
		eventManager.addEventHandler("PythonReloaded", self.onLoadGame)
		eventManager.addEventHandler("OnPreSave", self.onPreSave)
		eventManager.addEventHandler("SwitchHotSeatPlayer", self.onSwitchHotSeatPlayer)
		eventManager.setPopupHandlers(STORE_EVENT_ID, 'Reminder.Store', self.__eventReminderStoreBegin, self.__eventReminderStoreApply)
		eventManager.setPopupHandlers(RECALL_EVENT_ID, 'Reminder.Recall', self.__eventReminderRecallBegin, self.__eventReminderRecallApply)

	def __eventReminderStoreBegin(self, argsList):
		if SR.x > 2500:
			w = 520
			h = 232
		elif SR.x > 1700:
			w = 480
			h = 224
		elif SR.x > 1400:
			w = 440
			h = 216
		else:
			w = 400
			h = 210
		prompt = SR.aFontList[2] + TRNSLTR.getText("TXT_KEY_REMINDER_HEADER", ()) + "\n"
		prompt += SR.aFontList[5] + TRNSLTR.getText("TXT_KEY_REMINDER_PROMPT", ())
		ok = TRNSLTR.getText("TXT_KEY_MAIN_MENU_OK", ())
		cancel = TRNSLTR.getText("TXT_KEY_POPUP_CANCEL", ())

		popup = CyPopup(STORE_EVENT_ID, EventContextTypes.EVENTCONTEXT_SELF, True)
		popup.setSize(w, h)
		popup.setPosition(SR.x/2 - w/2, SR.y/2 - h/2)
		popup.setBodyString(prompt, 1<<0)
		popup.createSpinBox(0, "", 0, 1, 999, 0)
		popup.createEditBox("", 1)
		popup.addButton(ok)
		popup.addButton(cancel)
		popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventReminderStoreApply(self, playerID, userData, popupReturn):
		if popupReturn.getButtonClicked() != 1:
			reminderText = popupReturn.getEditBoxString(1)
			if reminderText:
				global g_turnReminderTexts
				if g_turnReminderTexts:
					idx = g_turnReminderTexts.rfind("\n") + 1
					if idx == -1:
						idx += 1
					else: idx += 2
					if CyInterface().determineWidth(g_turnReminderTexts[idx:] + reminderText) > SR.x - SR.x/7:
						g_turnReminderTexts += "\n"
					else: g_turnReminderTexts += "; "
				g_turnReminderTexts += reminderText
				turns = popupReturn.getSpinnerWidgetValue(0)
				reminderTurn = turns + GAME.getGameTurn()
				self.addReminder(playerID, Reminder(reminderTurn, reminderText))
				if g_autolog.isLogging() and ReminderOpt.isAutolog():
					g_autolog.writeLog("Reminder: On Turn %d, %s" % (reminderTurn, reminderText))

	def __eventReminderRecallBegin(self, argsList):
		global g_turnReminderTexts
		iPlayer = GAME.getActivePlayer()
		queue = self.reminders.get(iPlayer)
		if queue:
			iTurn = GAME.getGameTurn()
			yes = TRNSLTR.getText("TXT_KEY_POPUP_YES", ())
			no = TRNSLTR.getText("TXT_KEY_POPUP_NO", ())
			bLogging = g_autolog.isLogging() and ReminderOpt.isAutolog()
			bShowMsg = ReminderOpt.isShowMessage()
			bShowPop = ReminderOpt.isShowPopup()
			if bShowPop:
				iCount = 0
				prompt = TRNSLTR.getText("TXT_KEY_REMIND_NEXT_TURN_PROMPT", ())

			while not queue.isEmpty():
				nextTurn = queue.nextTurn()
				if nextTurn > iTurn:
					break
				reminder = queue.pop()

				if nextTurn < iTurn:
					print "[WARNING] Reminder - skipped turn %d: %s" %(reminder.turn, reminder.message)
					continue

				if bLogging:
					g_autolog.writeLog("Reminder: %s" % reminder.message)

				if g_turnReminderTexts:
					idx = g_turnReminderTexts.rfind("\n") + 1
					if idx == -1:
						idx += 1
					else: idx += 2
					if CyInterface().determineWidth(g_turnReminderTexts[idx:] + reminder.message) > SR.x - SR.x/7:
						g_turnReminderTexts += "\n"
					else: g_turnReminderTexts += "; "
				g_turnReminderTexts += reminder.message

				if bShowMsg:
					CvUtil.sendMessage(reminder.message, iPlayer, 10, "", ColorTypes(8))
				if bShowPop:
					body = SR.aFontList[4] + reminder.message + "\n" + SR.aFontList[5] + prompt
					popup = CyPopup(RECALL_EVENT_ID, EventContextTypes.EVENTCONTEXT_SELF, True)
					self.recall.append(reminder)
					popup.setUserData((iCount,))
					iCount += 1
					popup.setPosition(SR.x/3, SR.y/3)
					popup.setBodyString(body, 1<<0)
					popup.addButton(yes)
					popup.addButton(no)
					popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventReminderRecallApply(self, playerID, userData, popupReturn):
		if popupReturn.getButtonClicked() != 1:
			reminder = self.recall[userData[0]]
			reminder.turn += 1
			self.addReminder(playerID, reminder)

	def setReminders(self, queues):
		self.reminders = queues
		global g_reminders
		g_reminders = queues

	def clearReminders(self):
		self.reminders.clear()
		global g_turnReminderTexts
		g_turnReminderTexts = ""

	def addReminder(self, playerID, reminder):
		if hasNetMessage():
			player = GC.getPlayer(playerID)
			player.addReminder(reminder.turn, reminder.message)
		else:
			self.reminders.push(playerID, reminder)

	def onSwitchHotSeatPlayer(self, argsList):
		# Clears the end turn text so hot seat players don't see each other's reminders.
		ePlayer = argsList[0]
		global g_turnReminderTexts
		g_turnReminderTexts = ""

	def onBeginActivePlayerTurn(self, argsList):
		global g_turnReminderTexts
		g_turnReminderTexts = ""
		if ReminderOpt.isEnabled():
			g_eventMgr.beginEvent(RECALL_EVENT_ID)

	def onGameStart(self, argsList):
		self.clearReminders()

	def onLoadGame(self, argsList):
		self.clearReminders()
		queues = SdToolKit.sdGetGlobal("Reminders", "queues")
		if queues:
			self.setReminders(queues)

	def onPreSave(self, argsList):
		if self.reminders.isEmpty():
			SdToolKit.sdDelGlobal("Reminders", "queues")
		else:
			SdToolKit.sdSetGlobal("Reminders", "queues", self.reminders)


class Reminder(object):
	def __init__(self, turn, message):
		self.turn = turn
		self.message = message

class ReminderQueue(object):

	def __init__(self):
		self.clear()

	def clear(self):
		self.queue = []

	def clearBefore(self, turn, log=False):
		while not self.isEmpty() and self.nextTurn() < turn:
			reminder = self.pop()
			if log:
				print "Reminder - skipped turn %d: %s" %(reminder.turn, reminder.message)

	def size(self):
		return len(self.queue)

	def isEmpty(self):
		return self.size() == 0

	def nextTurn(self):
		if not self.size():
			return -1
		return self.queue[0].turn

	def push(self, reminder):
		for i, r in enumerate(self.queue):
			if reminder.turn < r.turn:
				self.queue.insert(i, reminder)
				break
		else:
			self.queue.append(reminder)

	def pop(self):
		if self.isEmpty():
			return None
		else:
			return self.queue.pop(0)


class Reminders(object):

	def __init__(self, queue=None):
		self.clear()
		if queue:
			self.queues[GAME.getActivePlayer()] = queue

	def clear(self):
		self.queues = {}

	def clearBefore(self, turn, log=False):
		for queue in self.queues.itervalues():
			queue.clearBefore(turn, log)

	def exists(self, playerID):
		return playerID in self.queues

	def get(self, playerID):
		if self.exists(playerID):
			return self.queues[playerID]

	def getForUpdate(self, playerID):
		if self.exists(playerID):
			return self.queues[playerID]
		else:
			queue = self.queues[playerID] = ReminderQueue()
			return queue

	def size(self, playerID=None):
		if playerID:
			queue = self.get(playerID)
			if queue:
				return queue.size()
			return 0
		else: return len(self.queues)

	def isEmpty(self, playerID=None):
		return self.size(playerID) == 0

	def nextTurn(self, playerID):
		queue = self.get(playerID)
		if queue:
			return queue.nextTurn()
		return -1

	def push(self, playerID, reminder):
		self.getForUpdate(playerID).push(reminder)

	def pop(self, playerID):
		queue = self.get(playerID)
		if queue:
			queue.pop()
