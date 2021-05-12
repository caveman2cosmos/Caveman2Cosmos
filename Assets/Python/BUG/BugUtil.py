## BugUtil
##
## Collection of general-purpose utility functions.
##
## Text Formatting
##
##   getPlainText(key, default, replaceFontTags)
##   getText(key, values, default, replaceFontTags)
##     Retrieves a translated <TEXT> message from CyTranslator. If <key> doesn't
##     exist, the default (if given) is returned. If no default is given, an
##     error message is returned. If <replaceFontTags> is True (default True).
##
##   colorText(text, color)
##     Accepts both a string (e.g. "COLOR_RED") or integer color.
##
##   formatFloat(value, decimals=0)
##     Returns <value> formatted as a string with <decimals> number of digits
##     after the decimal point (default 0).
##
##   escapeXml(obj)
##     Performs XML escape processing, replacing <, >, and & with their XML entities.
##
## Logging
##
##     See "BUG Core.xml" for options. They take effect once it has been processed.
##     Until then, the defaults in this module are used.
##
##   alert(message, args...)
##     Displays a formatted message on-screen. Use this sparingly as it's lame.
##
##   debug(message, args...)
##   info(message, args...)
##   warn(message, args...)
##   error(message, args...)
##     Logs a message with optional arguments (percent substitutions) to the
##     screen and/or the debug file depending on the current level cutoffs.
##     e.g. BugUtil.warn("TradeUtil - unknown item type %d", trade.ItemType)
##
##   trace(message, detail?, stack?)
##     Logs an exception message with optional detail and stacktrace (default on)
##
## Timing Code Execution
##
##   Timer class
##     Times single and multiple blocks of code and provides methods for logging
##     the individual and total times at DEBUG level.
##
## Binding and Calling Functions Dynamically
##
##   lookupFunction(module, functionOrClass)
##     Returns the actual function or class by looking it up in the module directly.
##
##   bindFunction(object, functionOrAttribute)
##     Returns the actual function or attribute bound to the given object.
##
##   callFunction(module, functionOrClass, args..., kwargs...)
##     Returns the result of calling the function bound using lookupFunction().
##
##   getFunction(module, functionOrClass, bind?, args..., kwargs...)
##     Returns a Function object that can be used to dynamically call a function
##     or class constructor at a later time with the arguments provided when
##     the Function was created.
##
## Exporting Functions to Other Modules
##
##   export(function, toModule, asName)
##     Makes <function> available in <toModule> as <asName>.
##
##   exportFunction(module, name, toModule, asName)
##     Makes <module>.<name> available in <toModule> as <asName>.
##
##   extend(function, toModule, asName)
##     Exports a lambda function to <toModule> as <asName> that calls <function>,
##     passing the original function from <toModule> and the received arguments.
##
##   extendFunction(module, name, toModule, asName)
##     Exports a lambda function to <toModule> as <asName> that calls <module>.<name>,
##     passing the original function from <toModule> and the received arguments.
##
## Python
##
##   fixSets(namespace):
##     If running on a Mac, imports "set" and "frozenset" from the sets module.
##
## Calling Functions in the Future
##
##   deferCall(function, delay=0.0)
##     Calls the given function during a GameUpdate event after at least <delay> seconds.
##
##   doDeferredCalls()
##     Calls the deferred functions whose delays have passed.
##     Triggered by gameUpdate event; do not call directly.
##
## Exception Classes
##
##   BugError, ConfigError
##     Raise these in BUG Core code rather than generic exceptions.
##
## Civ4 Helpers
##
##   doHotSeatCheck(args)
##     Called during EndPlayerTurn, fires SwitchHotSeatPlayer event during a hot seat
##     game when the active player's turn ends.
##
## Event Handlers
##
##   gameUpdate          doDeferredCalls - Calls deferred functions from deferCall()
##
## TODO
##
##   Move game checks to new module GameUtil
##   Split into multiple modules: LogUtil, TextUtil, TimeUtil, GameUtil, DeferUtil, FuncUtil
##   Create module Bug for common classes like exceptions
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *
import ColorUtil
import CvEventInterface
import sys
import time
import traceback
import types

## Text Formatting and Processing

def getPlainText(key, default=None, replaceFontTags=True):
	"""
	Looks up a translated message in XML without any replacement parameters.
	If the key isn't found, the default is returned.
	"""
	return getText(key, (), default, replaceFontTags)

def getText(key, values=(), default=None, replaceFontTags=True):
	"""
	Looks up a translated message in XML with a tuple of replacement parameters.
	It is safe to pass in a single value instead of tuple/list.
	If the key isn't found, the default is returned.
	"""
	if values is None:
		values = ()
	elif not isinstance(values, (tuple, list)):
		values = (values,)
	if isinstance(key, unicode):
		warn("getText - received Unicode key %s", key)
		key = str(key)
	text = CyTranslator().getText(key, values)
	if text not in ("", key):
		return text

	if default is None:
		debug("BugUtil.getText - XML key %s not found", key)
		return "XML key %s not found" % key

	return default


def colorText(text, color):
	"""
	Applies the color (string or int) to text and returns the resulting string.
	"""
	if text is not None and color is not None:
		if isinstance(color, types.StringTypes):
			color = ColorUtil.keyToType(color)
		if color >= 0:
			return CyTranslator().changeTextColor(text, color)
	return text

def formatFloat(number, decimals=0):
	"""
	Formats value as a floating point number with decimals digits in the mantissa
	and returns the resulting string.
	"""
	if decimals <= 0:
		return "%f" % number
	else:
		return ("%." + str(decimals) + "f") % number

def escapeXml(obj):
	"""
	Performs XML escape processing, replacing <, >, and & with their XML entities.
	"""
	return str(obj).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


# Logging to screen and file
screenLogLevel = 3
fileLogLevel = 0
minimumLogLevel = 0
logTime = True

def alert(message, *args):
	logToScreen(message % args)

def trace(message, *args):
	if args:
		message = message % args
	logToScreen(message)
	logToFile("TRACE: " + message)
	logToFile("TRACE: " + str(sys.exc_info()[1]))
	traceback.print_exc()

"""Logs a message at..."""
def debug(message, *args):
	"""... DEBUG level."""
	log(0, message, args)

def info(message, *args):
	"""... INFO level."""
	log(1, message, args)

def warn(message, *args):
	"""... WARN level."""
	log(2, message, args)

def error(message, *args):
	"""... ERROR level."""
	log(3, message, args)

def log(level, message, args=()):
	"""
	Logs a message on-screen and/or to a file depending on the current levels.

	The message is sent to each if the level is at least that of the destination.
	The level of the message is prepended to the message, and if logTime is True,
	the current time in HH:MM:SS format is prepended as well.

	Any encoding errors are swallowed and no message is logged.
	"""
	if level >= minimumLogLevel:
		try:
			if args:
				message = message % args
			if level >= screenLogLevel:
				logToScreen(message)
			if level >= fileLogLevel:
				LEVEL_PREFIXES = (
					"DEBUG: ",
					"INFO : ",
					"WARN : ",
					"ERROR: ",
				)
				logToFile(LEVEL_PREFIXES[level] + message)
		except UnicodeError:
			pass

def logToScreen(message):
	"""Displays the message in the on-screen message area after escaping < and >."""
	if CyGame().isFinalInitialized():
		CyInterface().addImmediateMessage(escapeXml(message), "")

def logToFile(message):
	"""Writes the message to the debug log with a time stamp if that option is enabled."""
	if logTime:
		message = time.asctime()[11:20] + message
	if isinstance(message, unicode):
		message = message.encode("iso8859")
	sys.stdout.write(message + "\n")

def readLoggingOptions(option=None, value=None):
	"""
	Pulls the logging options from the BUG Core options and stores into local copies.
	Done this way to avoid hitting the options in tight loops that use logging.
	"""
	import BugCore
	CoreOpt = BugCore.game.Core
	global screenLogLevel, fileLogLevel, minimumLogLevel, logTime
	screenLogLevel = CoreOpt.getScreenLogLevel()
	fileLogLevel = CoreOpt.getFileLogLevel()
	minimumLogLevel = min(screenLogLevel, fileLogLevel)
	logTime = CoreOpt.isLogTime()

## Timing Code Execution
class Timer:
	"""
	Stopwatch for timing code execution and logging the results.

	timer = BugUtil.Timer('function')
	... code to time ...
	timer.log()

	In a loop, log() will display each iteration's time. Since Timers are started
	when created, call reset() before entering the loop or pass in False.
	Use logTotal() at the end if you want to see the sum of all iterations.

	timer = BugUtil.Timer('draw loop', False)
	for/while ...
		timer.start()
		... code to time ...
		timer.log()
	timer.logTotal()

	A single Timer can be reused for timing loops without creating a new Timer
	for each iteration by calling restart().
	"""
	def __init__(self, item, start=True):
		"""Starts the timer."""
		self._item = item
		self.reset()
		if start:
			self.start()

	def reset(self):
		"""Resets all times to zero and stops the timer."""
		self._initial = None
		self._start = None
		self._time = 0
		self._total = 0
		return self

	def start(self):
		"""Starts the timer or starts it again if it is already running."""
		self._start = time.clock()
		if self._initial is None:
			self._initial = self._start
		return self

	def restart(self):
		"""Resets all times to zero and starts the timer."""
		return self.reset().start()

	def stop(self):
		"""
		Stops the timer if it is running and returns the elapsed time since start,
		otherwise returns 0.
		"""
		if self.running():
			self._final = time.clock()
			self._time = self._final - self._start
			self._total += self._time
			self._start = None
			return self._time
		return 0

	def running(self):
		"""Returns True if the timer is running."""
		return self._start is not None

	def time(self):
		"""Returns the most recent timing or 0 if none has completed."""
		return self._time

	def total(self):
		"""Returns the sum of all the individual timings."""
		return self._total

	def span(self):
		"""Returns the span of time from the first start() to the last stop()."""
		if self._initial is None:
			warn("called span() on a Timer that has not been started")
			return 0
		elif self._final is None:
			return time.clock() - self._initial
		else:
			return self._final - self._initial

	def log(self, extra=None):
		"""
		Stops the timer and logs the time of the current timing.

		This is the same as calling logTotal() or logSpan() for the first time.
		"""
		self.stop()
		return self._log(self.time(), extra)

	def logTotal(self, extra="total"):
		"""
		Stops the timer and logs the sum of all timing steps.

		This is the same as calling log() or logSpan() for the first time.
		"""
		self.stop()
		return self._log(self.total(), extra)

	def logSpan(self, extra=None):
		"""
		Stops the timer and logs the span of time covering all timings.

		This is the same as calling log() or logTotal() for the first time.
		"""
		self.stop()
		return self._log(self.span(), extra)

	def _log(self, runtime, extra):
		"""Logs the passed in runtime value."""
		if extra is None:
			debug("Timer - %s took %d ms" % (self._item, 1000 * runtime))
		else:
			debug("Timer - %s [%s] took %d ms" % (self._item, str(extra), 1000 * runtime))
		return self


## Binding and Calling Functions Dynamically
##
## (looking up module and function/class by name rather than directly in Python
## and passing in arguments set up at time of creation or when called)

class Function:

	def __init__(self, module, functionOrClass, *args, **kwargs):
		self.__module__ = module
		self.__name__ = functionOrClass
		self.function = None
		self.setArguments(*args, **kwargs)

	def bind(self):
		if self.function is None:
			self.function = lookupFunction(self.__module__, self.__name__)

	def setArguments(self, *args, **kwargs):
		self.args = args
		self.kwargs = kwargs

	def call(self, *args, **kwargs):
		self.bind()
		if args or kwargs:
			self.setArguments(*args, **kwargs)
		debug("BugUtil - calling %r" % self)
		return self.function(*self.args, **self.kwargs)

	def __call__(self, *args, **kwargs):
		return self.call(*args, **kwargs)

	def __repr__(self):
		if self.args or self.kwargs:
			return "<func %s.%s (%r, %r)>" %(self.__module__, self.__name__, self.args, self.kwargs)
		else:
			return "<func %s.%s>" %(self.__module__, self.__name__)

def lookupModule(module, log=True):
	if log:
		debug("BugUtil - looking up %s", module)
	try:
		return __import__(module)
	except ImportError:
		raise ConfigError("No such module '%s'", module)

def lookupFunction(module, functionOrClass, log=True):
	if log:
		debug("BugUtil - looking up %s.%s", module, functionOrClass)
	try:
		return getattr(lookupModule(module, False), functionOrClass)
	except AttributeError:
		raise ConfigError("Module '%s' must define function or class '%s'", module, functionOrClass)

def bindFunction(obj, functionOrAttribute):
	debug("BugUtil - binding %s.%s to %s", obj.__class__, functionOrAttribute, obj)
	try:
		return getattr(obj, functionOrAttribute)
	except AttributeError:
		raise ConfigError("Class '%s' must define function '%s'", obj.__class__, functionOrAttribute)

def getFunction(module, functionOrClass, bind=False, *args, **kwargs):
	if bind and not args and not kwargs:
		return lookupFunction(module, functionOrClass)
	else:
		func = Function(module, functionOrClass, *args, **kwargs)
		if bind:
			func.bind()
		return func

def callFunction(module, functionOrClass, *args, **kwargs):
	func = lookupFunction(module, functionOrClass)
	return func(*args, **kwargs)


## Exporting Functions to Other Modules
##
## Copying function definitions from one module to another, mostly to EntryPoint modules,
## so they can be called from within that module without needing to import the source module.

EXTEND_INSTEAD = "instead"

def export(function, toModule, asName=None, log=True):
	"""Makes <function> available in the toModule named <toModule> as <asName>."""
	mod = lookupModule(toModule, False)
	if asName is None:
		asName = function.__name__
	if log:
		if asName != function.__name__:
			debug("BugUtil - exporting %s.%s as %s.%s", function.__module__, function.__name__, toModule, asName)
		else:
			debug("BugUtil - exporting %s.%s to %s", function.__module__, asName, toModule)
	setattr(mod, asName, function)

def exportFunction(module, name, toModule, asName=None, log=True):
	if asName is None:
		asName = name
	export(lookupFunction(module, name, False), toModule, asName, log)

def extend(function, toModule, asName=None, how="instead", log=True):
	"""
	Exports a lambda function to <toModule> as <asName> that calls <function>,
	passing the original function from <toModule> and the received arguments.
	"""
	if asName is None:
		asName = function.__name__
	originalFunc = lookupFunction(toModule, asName, False)
	if how == "instead":
		newFunc = lambda *args: function(originalFunc, *args)
	elif how == "before":
		newFunc = lambda *args: (function(*args), originalFunc(*args))[1]
	elif how == "after":
		newFunc = lambda *args: (originalFunc(*args), function(*args))[1]
	else:
		raise ConfigError("Invalid how '%s' in extend()" % how)
	newFunc.__module__ = function.__module__
	newFunc.__name__ = asName
	if log:
		if asName != function.__name__:
			debug("BugUtil - extending %s.%s %s %s.%s", function.__module__, function.__name__, how, toModule, asName)
		else:
			debug("BugUtil - extending %s.%s %s %s", function.__module__, asName, how, toModule)
	export(newFunc, toModule, asName, False)

def extendBefore(function, toModule, asName=None, log=True):
	extend(function, toModule, asName, "before", log)

def extendAfter(function, toModule, asName=None, log=True):
	extend(function, toModule, asName, "after", log)

def extendInstead(function, toModule, asName=None, log=True):
	extend(function, toModule, asName, "instead", log)

def extendFunction(module, name, toModule, asName=None, how="instead", log=True):
	if asName is None:
		asName = name
	extend(lookupFunction(module, name, False), toModule, asName, how, log)

def extendBeforeFunction(module, name, toModule, asName=None, log=True):
	extendFunction(module, name, toModule, asName, "before", log)

def extendAfterFunction(module, name, toModule, asName=None, log=True):
	extendFunction(module, name, toModule, asName, "after", log)

def extendInsteadFunction(module, name, toModule, asName=None, log=True):
	extendFunction(module, name, toModule, asName, "instead", log)


## Python

def fixSets(namespace):
	"""
	If running on a Mac, imports "set" and "frozenset" from the sets module.

	Usage:
	  fixSets(globals())
	"""
	try:
		set
	except:
		import sets
		namespace["set"] = sets.Set
		namespace["frozenset"] = sets.ImmutableSet


## Deferred Calls

deferredCallQueue = []
def deferCall(function, delay=0.0):
	"""Calls the given function during a future gameUpdate event after at least <delay> seconds."""
	global deferredCallQueue
	if delay < 0.0:
		delay = 0.0
	when = time.clock() + delay
	entry = (when, function)
	if deferredCallQueue:
		for i in range(len(deferredCallQueue)):
			if when < deferredCallQueue[i][0]:
				deferredCallQueue.insert(i, entry)
				return
	deferredCallQueue.append(entry)

def doDeferredCalls(argsList=None):
	"""Calls deferred calls whose timers have expired."""
	global deferredCallQueue
	if deferredCallQueue:
		# insert a marker so that items added during this round of calls will be processed on the next round
		deferCall(None)
		while deferredCallQueue:
			when, func = deferredCallQueue[0]
			if func is None:
				del deferredCallQueue[0]
				break
			if when > time.clock():
				warn("doDeferredCalls - entry inserted before marker")
				for i, (when, func) in enumerate(deferredCallQueue):
					if func is None:
						del deferredCallQueue[i]
						break
				else:
					error("doDeferredCalls - marker not found")
				break
			del deferredCallQueue[0]
			debug("doDeferredCalls - calling %s", func)
			func()


## Exception Classes

class BugError(Exception):
	"""Generic BUG-related error."""
	def __init__(self, message, *args):
		if args:
			message = message % args
		Exception.__init__(self, message)

class ConfigError(BugError):
	"""
	Error related to configuration problems.

	These are caught and reported during BUG initialization, and initizliation
	is allowed to continue so most problems can be reported at once. This may
	result in false-positive errors being reported.
	"""
	def __init__(self, message, *args):
		BugError.__init__(self, message, *args)


## Civ4 Helpers

def doHotSeatCheck(args):
	"""
	Called during EndPlayerTurn, fires SwitchHotSeatPlayer event during a hot seat
	game when the active player's turn ends.
	"""
	#iGameTurn = args[0]
	ePlayer = args[1]
	GAME = CyGame()
	if GAME.isHotSeat() and ePlayer == GAME.getActivePlayer():
		CvEventInterface.getEventManager().fireEvent("SwitchHotSeatPlayer", ePlayer)
