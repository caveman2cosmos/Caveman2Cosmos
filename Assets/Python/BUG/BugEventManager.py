## BugEventManager
##
## Extended version of CvCustomEventManager by Gillmer J. Derge.
##
## Changes:
##
## You no longer need to modify this module in order to add your custom events.
## Instead, add <event> and <events> tags to your mod's config XML or call
## addEventHandler() from your mod's Python, preferably an <init> function.
##
## * New public methods
##
##   - hasEvent(eventType)
##       Returns True if eventType is defined
##
##   - addEvent(eventType)
##       Adds a new event type with no default handler; you can also add a final True
##       parameter to your call to addEventHandler() or use <event> and <events>
##       in your mod's config XML to add your events and handlers.
##
##   - fireEvent(eventType, args...)
##       Fires an event from Python, building an argList from the arguments passed in
##
##   - setPopupHandler(eventType, handler)
##       Sets the handler for the given popup event type
##
##   - setPopupHandlers(eventType, name, beginFunction, applyFunction)
##       Calls setPopupHandler() with a composed handler tuple
##
##   - removePopupHandler(eventType)
##       Removes the handlers for a popup event (int)
##
##   - addShortcutHandler(keys, function)
##       Adds a handler for the given keyboard shortcut(s)
##
## * New BUG events
##
##   - BeginActivePlayerTurn(ePlayer, iGameTurn)
##       Signifies the moment the active player can begin making their moves
##       Fired from CvMainInterface.updateScreen()
##
##   - SwitchHotSeatPlayer(ePlayer, iGameTurn)
##       Signifies that the active player in a Hot Seat game has changed
##
##   - LanguageChanged(iLanguage)
##       Fired from CvOptionsScreenCallbackInterface.handleLanguagesDropdownBoxInput()
##
##   - ResolutionChanged(iResolution)
##       Fired from CvOptionsScreenCallbackInterface.handleResolutionDropdownInput()
##
##   - PreGameStart
##       Fired from CvAppInterface.preGameStart()
##
##   - PythonReloaded
##       Fired after Python modules have been reloaded while game is running
##
## * New BULL events
##
##   - unitUpgraded(pOldUnit, pNewUnit, iPrice)
##       Fired when a unit is upgraded
##
##   - unitCaptured(eOwner, eUnitType, pNewUnit)
##       Fired when a unit is captured
##
##   - combatWithdrawal(pAttacker, pDefender)
##       Fired when a unit withdraws from combat after doing maximum damage
##
##   - combatRetreat(pAttacker, pDefender)
##       Fired when a unit retreats from combat, escaping death
##
##   - combatLogCollateral(pAttacker, pDefender, iDamage)
##       Fired when a unit inflicts collateral damage to another unit
##
##   - combatLogFlanking(pAttacker, pDefender, iDamage)
##       Fired when a unit inflicts flanking damage to another unit
##
## * Fixed events
##
##   - endTurnReady
##       Signifies the first time the "End Turn" text is displayed on the screen
##       Fired from CvMainInterface.updateScreen()
##
## * Events and their arguments are optionally logged.
##
## * Added configure() to set the options.
##
## Based on CvCustomEventManager by Gillmer J. Derge
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *
import CvEventManager
import BugData
import BugUtil
import InputUtil
import DebugUtils

GC = CyGlobalContext()
g_eventManager = None

class BugEventManager(CvEventManager.CvEventManager):

	"""
	Extends the standard event manager by adding support for multiple
	handlers for each event.

	Instead of modifying this file as you would have done with CvCustomEventManager,
	use the <event> and <events> tags in your mod's initialization XML file.

	Methods exist for both adding and removing event handlers.  A set method
	also exists to override the default handlers.  Clients should not depend
	on event handlers being called in a particular order, though they are
	called in the order in which they are added.

	Note that the naming conventions for the event type strings vary from event
	to event.  Some use initial capitalization, some do not; some eliminate the
	"on..." prefix used in the event handler function name, some do not.  Look
	at the unmodified CvEventManager.py source code to determine the correct
	name for a particular event.

	Take care with event handlers that also extend CvEventManager.  Since
	this event manager handles invocation of the base class handler function,
	additional handlers should not also call the base class function themselves.

	It's best *not* to extend CvEventManager or CvCustomEventManager. In fact,
	you are free to use module methods outside classes if you wish.
	"""

	def __init__(self):
		CvEventManager.CvEventManager.__init__(self)

		global g_eventManager
		if g_eventManager is not None:
			raise BugUtil.ConfigError("BugEventManager already created")
		g_eventManager = self

		self.bDbg = False
		self.bMultiPlayer = False

		# used to register shortcut handlers
		self.shortcuts = {}

		# init fields for BeginActivePlayerTurn
		self.resetActiveTurn()

		# map the initial EventHandlerMap values into the new data structure
		for eventType, eventHandler in self.EventHandlerMap.iteritems():
			self.setEventHandler(eventType, eventHandler)

		# add new core events; see unused sample handlers below for argument lists
		self.addEvent("BeginActivePlayerTurn")
		self.addEvent("SwitchHotSeatPlayer")
		self.addEvent("LanguageChanged")
		self.addEvent("ResolutionChanged")
		self.addEvent("PythonReloaded")

		# add events used by this event manager
		self.addEventHandler("kbdEvent", self.onShortcutHandler)
		self.addEventHandler("OnLoad", self.resetActiveTurn)
		self.addEventHandler("GameStart", self.resetActiveTurn)
		self.addEventHandler("gameUpdate", self.onGenericUpdate)

		# BULL events
		self.addEvent("unitUpgraded")
		self.addEvent("unitCaptured")
		self.addEvent("combatWithdrawal")
		self.addEvent("combatRetreat")
		self.addEvent("combatLogCollateral")
		self.addEvent("combatLogFlanking")
		#AND Events
		self.addEvent("changeTeam")

	def hasEvent(self, eventType):
		"""Returns True if the given event type is defined."""
		return eventType in self.EventHandlerMap

	def _checkEvent(self, eventType):
		"""Raises ConfigError if the eventType is undefined."""
		if not self.hasEvent(eventType):
			raise BugUtil.ConfigError("Event '%s' is undefined" % eventType)

	def addEvent(self, eventType):
		"""Creates a new event type without any handlers.

		Prints a warning if eventType is already defined.
		"""
		if self.hasEvent(eventType):
			print "[WARNING] BugEventManager - event '%s' already defined", eventType
		else:
			print "BugEventManager - adding event '%s'", eventType
			self.EventHandlerMap[eventType] = []

	def addEventHandler(self, eventType, eventHandler=None):
		"""Adds a handler for the given event type, adding the event if necessary.

		If eventHandler is None, the event is added if necessary without a handler.

		A list of supported event types can be found in the initialization
		of EventHandlerMap in the CvEventManager class. A debug message is
		printed if the event type doesn't exist.
		"""
		if not self.hasEvent(eventType):
			self.addEvent(eventType)
		if eventHandler:
			self.EventHandlerMap[eventType].append(eventHandler)

	def removeEventHandler(self, eventType, eventHandler):
		"""Removes a handler for the given event type.

		A list of supported event types can be found in the initialization
		of EventHandlerMap in the CvEventManager class.  It is an error if
		the given handler is not found in the list of installed handlers.

		Throws ConfigError if the eventType is undefined.
		"""
		self._checkEvent(eventType)
		self.EventHandlerMap[eventType].remove(eventHandler)

	def setEventHandler(self, eventType, eventHandler):
		"""Removes all previously installed event handlers for the given
		event type and installs a new handler, adding the event if necessary.

		A list of supported event types can be found in the initialization
		of EventHandlerMap in the CvEventManager class.  This method is
		primarily useful for overriding, rather than extending, the default
		event handler functionality.
		"""
		if not self.hasEvent(eventType):
			self.addEvent(eventType)
		if eventHandler is not None:
			self.EventHandlerMap[eventType] = [eventHandler]
		else:
			self.EventHandlerMap[eventType] = []

	def setPopupHandler(self, eventType, handler):
		"""Removes all previously installed popup handlers for the given event type and installs a new pair of handlers.

		The eventType should be an integer.
		It must be unique with respect to the integers assigned to built in events.
		The popupHandler should be a list made up of (name, applyFunction, beginFunction).
		The name is used in debugging output.
		The begin and apply functions are invoked by beginEvent and applyEvent,
		respectively, to manage a popup dialog in response to the event.
		"""
		BugUtil.debug("BugEventManager - setting popup handler for event %s (%d)", handler[0], eventType)
		self.Events[eventType] = handler

	def setPopupHandlers(self, eventType, name, beginFunction, applyFunction):
		"""Builds a handler tuple to pass to setPopupHandler()."""
		self.setPopupHandler(eventType, (name, applyFunction, beginFunction))

	def removePopupHandler(self, eventType):
		"""Removes all previously installed popup handlers for the given
		event type.

		The eventType should be an integer. It is an error to fire this
		eventType after removing its handlers.
		"""
		if eventType in self.Events:
			BugUtil.debug("BugEventManager - removing popup handler for event %d", eventType)
			del self.Events[eventType]
		else:
			BugUtil.warn("BugEventManager - event %d has no popup handler", eventType)

	def addShortcutHandler(self, keys, handler):
		"""Adds the given handler to be called when any of the keyboard shortcut(s) is hit.

		The keys argument may be a single Keystroke, a collection of one or more Keystrokes, or
		a string which will be converted to such.
		If any keystrokes have existing handlers, new ones are ignored and a warning is displayed.
		"""
		if isinstance(keys, InputUtil.Keystroke):
			keys = (keys,)
		elif isinstance(keys, basestring):
			keys = InputUtil.stringToKeystrokes(keys)
		for key in keys:
			if key in self.shortcuts:
				BugUtil.error("shortcut %s already assigned", key)
			else:
				BugUtil.debug("BugEventManager - setting shortcut handler for %s", key)
				self.shortcuts[key] = handler

	def fireEvent(self, eventType, *args):
		"""Fires the given event passing in all args as a list."""
		self._dispatchEvent(eventType, args)

	def handleEvent(self, argsList):
		"""Handles events by calling all installed handlers."""
		self.bDbg, self.bMultiPlayer, self.bAlt, self.bCtrl, self.bShift, self.bAllowCheats = argsList[-6:]
		return self._dispatchEvent(argsList[0], argsList[1:-6])

	def _dispatchEvent(self, eventType, argsList):
		if DebugUtils.bDebugMode and eventType != "gameUpdate":
			print ("dispatchEvent: " + eventType, argsList)
		return EVENT_FUNCTION_MAP.get(eventType, BugEventManager._handleDefaultEvent)(self, eventType, argsList)

	def _handleDefaultEvent(self, eventType, argsList):
		if self.EventHandlerMap.has_key(eventType):
			for eventHandler in self.EventHandlerMap[eventType]:
				try:
					eventHandler(argsList)
				except:
					BugUtil.trace("Error in %s event handler %s", eventType, eventHandler)

	def _handleConsumableEvent(self, eventType, argsList):
		"""Handles events that can be consumed by the handlers, such as keyboard or mouse events.

		If a handler returns non-zero, processing is terminated, and no subsequent handlers are invoked. (Changed)
		"""
		if self.EventHandlerMap.has_key(eventType):
			bHandled = False
			for eventHandler in self.EventHandlerMap[eventType]:
				try:
					A = eventHandler(argsList)
					if A:
						bHandled = True
				except:
					BugUtil.trace("Error in %s event handler %s", eventType, eventHandler)
			if bHandled:
				return 1
		return 0

	def _handleOnPreSaveEvent(self, eventType, argsList):
		"""Tells BugData to save all script data after other handlers have been called.
		This won't work as a normal handler because it must be done after other handlers.
		"""
		self._handleDefaultEvent(eventType, argsList)
		BugData.save()

	# TODO: this probably needs to be more complex
	def _handleOnSaveEvent(self, eventType, argsList):
		"""Handles OnSave events by concatenating the results obtained
		from each handler to form an overall consolidated save string.
		"""
		result = ""
		if self.EventHandlerMap.has_key(eventType):
			for eventHandler in self.EventHandlerMap[eventType]:
				try:
					result += eventHandler(argsList)
				except:
					BugUtil.trace("Error in %s event handler %s", eventType, eventHandler)
		return result

	def resetActiveTurn(self, argsList=None):
		self.iActiveTurn = -1
		self.eActivePlayer = -1
		self.bEndTurnFired = False

	def checkActivePlayerTurnStart(self):
		"""Fires the BeginActivePlayerTurn event if either the active player or game turn
		have changed since the last check. This signifies that the active player is about
		to be able to move their units.

		Called by onGenericUpdate() when the End Turn Button is green.
		"""
		GAME = GC.getGame()
		iTurn = GAME.getGameTurn()
		ePlayer = GAME.getActivePlayer()
		if self.iActiveTurn != iTurn or self.eActivePlayer != ePlayer:
			self.iActiveTurn = iTurn
			self.eActivePlayer = ePlayer
			self.bEndTurnFired = False
			self.fireEvent("BeginActivePlayerTurn", ePlayer, iTurn)

	def checkActivePlayerTurnEnd(self):
		"""Fires the endTurnReady event if it hasn't been fired since the active player's turn started.

		Called by onGenericUpdate() when the End Turn Button is red.
		"""
		if not self.bEndTurnFired:
			self.bEndTurnFired = True
			self.fireEvent("endTurnReady", self.iActiveTurn)

	def onGenericUpdate(self, argsList):
		eState = CyInterface().getEndTurnState()
		if eState == EndTurnButtonStates.END_TURN_GO:
			self.checkActivePlayerTurnStart()
		else:
			self.checkActivePlayerTurnEnd()

	def onShortcutHandler(self, argsList):
		"""Handles onKbdEvent by firing the keystroke's handler if it has one registered."""
		eventType, key, mx, my, px, py = argsList
		if eventType == 6: # Key down
			if not InputUtil.isModifier(key):
				stroke = InputUtil.Keystroke(key, self.bAlt, self.bCtrl, self.bShift)
				if stroke in self.shortcuts:
					if DebugUtils.bDebugMode:
						print "BugEventManager - calling handler for shortcut %s"
					self.shortcuts[stroke](argsList)
		return 0

EVENT_FUNCTION_MAP = {
	"kbdEvent": BugEventManager._handleConsumableEvent,
	"mouseEvent": BugEventManager._handleConsumableEvent,
	"OnPreSave": BugEventManager._handleOnPreSaveEvent,
	"OnSave": BugEventManager._handleOnSaveEvent
}

# Sample Event Handlers
'''
def onBeginActivePlayerTurn(self, argsList):
	"""Called when the active player can start their turn."""
	ePlayer, iGameTurn = argsList

def onEndTurnReady(self, argsList):
	"""Called when the active player has moved their last waiting unit."""
	ePlayer = argsList[0]

def onSwitchHotSeatPlayer(self, argsList):
	"""Called when a hot seat player's turn ends."""
	ePlayer = argsList[0]

def onLanguageChanged(self, argsList):
	"""Called when the user changes their language selection."""
	iLanguage = argsList[0]

def onResolutionChanged(self, argsList):
	"""Called when the user changes their graphics resolution."""
	iResolution = argsList[0]

def onUnitUpgraded(self, argsList):
	"""Called when a unit is upgraded."""
	pOldUnit, pNewUnit, iPrice = argsList

def onUnitCaptured(self, argsList):
	"""Called when a unit is captured."""
	eOwner, eUnitType, pNewUnit = argsList

def onCombatWithdrawal(self, argsList):
	"""Fired when a unit withdraws from combat after doing maximum damage."""
	pAttacker, pDefender = argsList

def onCombatRetreat(self, argsList):
	"""Fired when a unit retreats from combat, escaping death."""
	pAttacker, pDefender = argsList

def onCombatLogCollateral(self, argsList):
	"""Fired when a unit inflicts collateral damage to another unit."""
	pAttacker, pDefender, iDamage = argsList

def onCombatLogFlanking(self, argsList):
	"""Fired when a unit inflicts flanking damage to another unit."""
	pAttacker, pDefender, iDamage = argsList

#A New Dawn Event
def onChangeTeam(self, argsList):
	"""Fired when two teams join or seperate"""
	iOldTeam, iNewTeam = argsList
'''