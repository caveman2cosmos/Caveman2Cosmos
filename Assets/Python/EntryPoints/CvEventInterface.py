# Sid Meier's Civilization 4
# Copyright Firaxis Games 2005
#
# CvEventInterface.py
#
# These functions are App Entry Points from C++
# WARNING: These function names should not be changed
# WARNING: These functions can not be placed into a class
#
# No other modules should import this


# **********************************
# GJD modifications start here
# **********************************

def getEventManager(bInit = False):
	if bInit:
		import BugEventManager
		global eventManager
		eventManager = BugEventManager.BugEventManager()
	return eventManager

getEventManager(True)

# **********************************
# GJD modifications end here
# **********************************

def onEvent(argsList):
	"""Called when a game event happens - return 1 if the event was consumed."""
	return eventManager.handleEvent(argsList)

def applyEvent(argsList):
	return eventManager.applyEvent(argsList)

def beginEvent(context, argsList = -1):
	return eventManager.beginEvent(context, argsList)

def initAfterReload():
	"""
	Initialize BUG and fires PythonReloaded event after reloading Python modules while game is still running.

	The first time this module is loaded after the game launches, the global context is not yet ready,
	and thus BUG cannot be initialized. When the Python modules are reloaded after being changed, however,
	this will reinitialize BUG and the main interface.
	"""
	import BugInit
	BugInit.init()
	try:
		import CvScreensInterface
		CvScreensInterface.reinitMainInterface()
	except:
		import BugUtil
		BugUtil.error("BugInit - failure rebuilding main interface after reloading Python modules")
	eventManager.fireEvent("PythonReloaded")
