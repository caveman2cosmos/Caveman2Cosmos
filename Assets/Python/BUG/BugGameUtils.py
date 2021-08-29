## BugGameUtils
##
## Provides modular access to CvGameUtils-based callback handlers and listeners.
## A handler is a function that overrides the default processing of the callback
## and returns a result of its choosing. A listener is a function that is called
## after the handler(s) for a callback with the original arguments and the result.
##
## Almost every callback has a default value assigned to it. This value is used
## to signal that a handler has decided not to handle the callback. Each callback
## can have multiple handlers assigned to it, and the first handler to return
## a non-default value "wins". Its value is returned as the result of the callback
## and any handlers after it are skipped. All listeners for a callback are called.
##
## Handlers and listeners can be individual module-level functions or members
## of a class such as your own CvGameUtils replacement. You can use the XML tags
## or the module-level functions here to register your handlers and listeners.
##
## When adding a handler or listener function, if its name matches the callback
## you don't need to specify the callback name. Listener functions may put "Listener"
## after the callback name for automatic detection.
##
## The <handler>, <listener>, and <default> elements below must be enclosed within
## a <gameutils> element. If it specifies a "class" attribute, only the listed
## handlers and listeners will be registered.
##
## Automatically Registering GameUtils
##
##   addModuleUtils(utils, override=False)
##   addClassUtils(utils, override=False)
##     Adds all of the functions from <utils> as handlers and listeners. Every handler
##     name must match the name of a callback (with "Listener" appended for listeners).
##     To ignore a function, begin its name with a single underscore ("_").
##     If <override> is True, its handlers are placed before existing handlers.
##
##       <gameutils module="<module>" class="<class>"
##                  handlers="<func1> <func2>..." override="True|False"
##                  listeners="<func1> <func2>..."/>
##
## Registering Handlers
##
##   addHandler(func, override=False)
##     Adds <func> as a handler for the callback with the same name.
##     If <override> (default False) is True, it is placed before existing handlers.
##
##   addNamedHandler(name, func, override=False)
##     Same as addHandler() above for callback <name>.
##
##       <gameutils module="<module>" handlers="<func1> <func2> ..." override="True|False"/>
##
## Registering Listeners
##
##   addListener(func)
##     Adds <func> as a listener for the callback with the same name.
##     If its name ends with "Listener", it is dropped to get the callback name.
##
##   addNamedListener(name, func)
##     Same as addListener() above for callback <name>.
##
##       <gameutils module="<module>" listeners="<func1> <func2> ..."/>
##
## Creating New Callbacks
##
##   createCallback(name, func, default=None)
##     Creates a new callback <name> with base handler <func> and optional <default>.
##
##       <callback name="<name>" type="<type>" default="<default>" handler="<func>"/>
##
##   setDefault(name, default)
##     Sets the default for callback <name> to <default>.
##     Use this only if you have defined new callbacks for your mod.
##
## Notes
##
##   Module must be initialized when it is loaded before BUG is initialized.
##
## Copyright (c) 2009 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *
import BugConfig
import BugUtil
import CvGameUtils
import types

g_dispatcher = None

## Configuration

def addModuleUtils(utils, override=False, log=None):
	getDispatcher()._addModuleUtils(utils, log)

def addClassUtils(utils, override=False, log=None):
	getDispatcher()._addClassUtils(utils, log)


def addHandler(func, override=False, log=None):
	addNamedHandler(func.__name__, func, override, log)

def addNamedHandler(name, func, override=False, log=None):
	getDispatcher()._addHandler(name, func, override, log)

def addBoundHandler(utils, func, override=False, log=None):
	addNamedBoundHandler(func.__name__, utils, func, override, log)

def addNamedBoundHandler(name, utils, func, override=False, log=None):
	getDispatcher()._addBoundHandler(name, func, override, log)


def addListener(func, log=None):
	name = func.__name__
	if name.endswith("Listener"):
		name = name[:-len("Listener")]
	addNamedListener(name, func, log)

def addNamedListener(name, func, log=None):
	getDispatcher()._addListener(name, func, log)

def addBoundListener(utils, func, log=None):
	name = func.__name__
	if name.endswith("Listener"):
		name = name[:-len("Listener")]
	addNamedBoundListener(name, utils, func, log)

def addNamedBoundListener(name, utils, func, log=None):
	getDispatcher()._addBoundListener(name, utils, func, log)


def createCallback(name, func, default=None, log=None):
	getDispatcher()._createCallback(name, func, default, log)

def setDefault(name, default):
	getDispatcher()._setDefault(name, default)


def getDispatcher():
	return g_dispatcher

## Dispatcher

class Dispatcher:

	def __init__(self, log=None):
		self._log = log
		self._callbacks = {}
		self._baseUtils = CvGameUtils.CvGameUtils()
		clazz = CvGameUtils.CvGameUtils
		for name, func in clazz.__dict__.iteritems():
			if not name.startswith("_") and isinstance(func, types.FunctionType):
				self._createCallback(name, BugUtil.bindFunction(self._baseUtils, name), None, self._log)

		# setup defaults
		#self._setDefault("updateColoredPlots", False) # Toffer - Left one example.

	def _createCallback(self, name, func, default=None, log=None):
		if log is None:
			log = self._log
		if default is not None:
			BugUtil.debug("BugGameUtils - creating callback %s with default %s", name, default)
		else:
			BugUtil.debug("BugGameUtils - creating callback %s", name)
		callback = Callback(name, func, default, log)
		self._callbacks[name] = callback
		setattr(self.__class__, name, callback)

	def _getCallback(self, name):
		try:
			return self._callbacks[name]
		except KeyError:
			BugUtil.trace("Unknown GameUtils callback %s", name)
			raise

	def _setDefault(self, name, default):
		self._getCallback(name).setDefault(default)

	def _addHandler(self, name, func, override=False, log=None):
		self._getCallback(name).addHandler(func, override, log)

	def _addBoundHandler(self, name, utils, func, override=False, log=None):
		self._addHandler(name, self._bind(utils, func), override, log)

	def _addListener(self, name, func, log=None):
		self._getCallback(name).addListener(func, log)

	def _addBoundListener(self, name, utils, func, log=None):
		self._addListener(name, self._bind(utils, func), log)

	def _addModuleUtils(self, utils, override=False, log=None):
		# Registers all of the handler and listener functions in <utils> that match existing callbacks.
		BugUtil.debug("BugGameUtils - registering %s", utils.__name__)
		for name, func in utils.__dict__.iteritems():
			if not name.startswith("_") and isinstance(func, types.FunctionType):
				if name.endswith("Listener"):
					name = name[:-len("Listener")]
					if name in self._callbacks:
						self._addListener(name, func, log)
				else:
					if name in self._callbacks:
						self._addHandler(name, func, override, log)

	def _addClassUtils(self, utils, override=False, log=None):
		# Registers all of the handler and listener functions in <utils> that match existing callbacks.
		clazz = utils.__class__
		BugUtil.debug("BugGameUtils - registering %s.%s", clazz.__module__, clazz.__name__)
		for name, func in clazz.__dict__.iteritems():
			if not name.startswith("_") and isinstance(func, types.FunctionType):
				if name.endswith("Listener"):
					name = name[:-len("Listener")]
					if name in self._callbacks:
						self._addBoundListener(name, utils, func, log)
				else:
					if name in self._callbacks:
						self._addBoundHandler(name, utils, func, override, log)

	def _bind(self, utils, func):
		bound = lambda *args: func(utils, *args)
		bound.__module__ = func.__module__
		return bound


## Callback

class Callback:

	def __init__(self, name, baseHandler, default=None, log=False):
		self.name = name
		self.baseHandler = baseHandler
		self.default = default
		self.log = log
		self.handlers = []
		self.listeners = []

	def setDefault(self, default):
		BugUtil.debug("BugGameUtils - %s - setting default to %s", self.name, default)
		self.default = default

	def addHandler(self, func, override=False, log=None):
		if override:
			BugUtil.debug("BugGameUtils - %s - overriding %s handler", self.name, func.__module__)
			self.handlers.insert(0, func)
		else:
			BugUtil.debug("BugGameUtils - %s - adding %s handler", self.name, func.__module__)
			self.handlers.append(func)
		self.setLogging(log)

	def addListener(self, func, log=None):
		BugUtil.debug("BugGameUtils - %s - adding %s listener", self.name, func.__module__)
		self.listeners.append(func)
		self.setLogging(log)

	def setLogging(self, log=None):
		if log is not None:
			self.log = log
			BugUtil.debug("BugGameUtils - callback %s logging is now %s", self.name, self.log)

	def callHandler(self, handler, argsList):
		try:
			if argsList is None:
				return handler()
			else:
				return handler(argsList)
		except:
			BugUtil.trace("Error in %s callback handler %s", handler.__module__, self.name)

	def callListener(self, listener, argsList, result):
		try:
			if argsList is None:
				value = listener(result)
			else:
				value = listener(argsList, result)
			if value is not None:
				BugUtil.warn("BugGameUtils - %s - ignoring %s listener's return value %r", self.name, listener.__module__, value)
		except:
			BugUtil.trace("Error in %s callback listener %s", listener.__module__, self.name)

	def __call__(self, argsList=None):
		for handler in self.handlers:
			if self.log: BugUtil.debug("BugGameUtils - %s - dispatching to %s handler", self.name, handler.__module__)
			result = self.callHandler(handler, argsList)
			if result is not None and result != self.default:
				break
		else:
			if self.default is not None:
				if self.log: BugUtil.debug("BugGameUtils - %s - using default %s", self.name, self.default)
				result = self.default
			else:
				if self.log: BugUtil.debug("BugGameUtils - %s - dispatching to base handler", self.name)
				result = self.callHandler(self.baseHandler, argsList)
		if result is not None:
			for listener in self.listeners:
				if self.log: BugUtil.debug("BugGameUtils - %s - calling %s listener", self.name, listener.__module__)
				self.callListener(listener, argsList, result)
		else:
			BugUtil.error("BugGameUtils - %s - no handler returned a value", self.name)
		return result


## Config Parser Handler

class GameUtilsHandler(BugConfig.Handler):

	TAG = "gameutils"

	def __init__(self):
		BugConfig.Handler.__init__(self, GameUtilsHandler.TAG, "module function class handler handlers listener listeners override log dll", CallbackHandler.TAG)
		self.addAttribute("module", True, True)
		self.addExcludedAttribute("function")
		self.addAttribute("class", False, False, None, "function")
		self.addExcludedAttribute("handler")
		self.addAttribute("handlers", False, False, None, "handler")
		self.addExcludedAttribute("listener")
		self.addAttribute("listeners", False, False, None, "listener")
		self.addAttribute("override", True, False, "false")
		self.addAttribute("log")
		self.addAttribute("dll")

	def handle(self, element, module, clazz, handlers, listeners, override, log, dll):
		override = self.isTrue(override)
		if clazz:
			utils = BugUtil.callFunction(module, clazz)
			element.setState("gameutils", utils)
			if handlers or listeners:
				if handlers:
					for handler in handlers.replace(",", " ").split():
						addHandler(BugUtil.bindFunction(utils, handler), override, log)
				if listeners:
					for listener in listeners.replace(",", " ").split():
						func = None
						if not listener.endswith("Listener"):
							try:
								func = BugUtil.bindFunction(utils, listener + "Listener")
							except BugUtil.ConfigError:
								pass
						if not func:
							try:
								func = BugUtil.bindFunction(utils, listener)
							except BugUtil.ConfigError:
								raise BugUtil.ConfigError("Game utils %s.%s must define function %s or %s",
										module, clazz, listener, listener + "Listener")
						addListener(func, log)
			else:
				addClassUtils(utils, override, log)
		else:
			if handlers or listeners:
				if handlers:
					for handler in handlers.replace(",", " ").split():
						addHandler(BugUtil.lookupFunction(module, handler), override, log)
				if listeners:
					for listener in listeners.replace(",", " ").split():
						addListener(BugUtil.lookupFunction(module, listener), log)
			else:
				addModuleUtils(BugUtil.lookupModule(module), override, log)


class CallbackHandler(BugConfig.Handler):

	TAG = "callback"

	def __init__(self):
		BugConfig.Handler.__init__(self, CallbackHandler.TAG, "name type default module handler listener log dll")
		self.addAttribute("name", True)
		self.addAttribute("type")
		self.addAttribute("default")
		self.addAttribute("module", False, True)
		self.addAttribute("handler", True, False, None, "name")
		self.addAttribute("listener")
		self.addAttribute("log")
		self.addAttribute("dll")

	def handle(self, element, name, type, default, module, handler, listener, log, dll):
		utils = element.getState("gameutils")
		if utils:
			func = BugUtil.bindFunction(utils, handler)
			if listener:
				listenerFunc = BugUtil.bindFunction(utils, listener)
		elif not module:
			raise BugUtil.ConfigError("Element <%s> requires attribute module or be enclosed in <gameutils> that defines a class", element.tag)
		else:
			func = BugUtil.lookupFunction(module, handler)
			if listener:
				listenerFunc = BugUtil.lookupFunction(module, listener)
		createCallback(name, func, self.createValue(type, default), log)
		if listener:
			addNamedListener(name, listenerFunc, log)


## Initialization

def init(log=None):
	print "BugGameUtils.init"
	global g_dispatcher
	g_dispatcher = Dispatcher(log)

# initialize once module has loaded
init()
