## BugInit
##
## Initializes the BUG core and loads all of the mods.
##
## Called by BugEventManager during "OnLoad" and "PreGameStart" events.
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *
import BugConfig
import BugOptions
import SystemPaths as SP

g_initDone = False

g_initQueue = []

def init():
	"""Performs the one-time initialization of the BUG core and all mods."""
	global g_initDone
	print "BugInit.init"
	if g_initDone:
		print "BugInit.init - already complete"
		return True

	loadMod("init")
	import BugCore
	BugCore.initDone()

	callInits()

	g_initDone = True
	CyGlobalContext().setIsBug()

	return True


def loadMod(name):
	"""Load the given mod from its XML file using a custom parser."""
	path = SP.modDir + "\Assets\Config\%s.xml" % name
	if SP.isFile(path):
		print "BugInit - loading mod %s..." % name
		parser = BugConfig.ConfigParser()
		parser.parse(path)
	else:
		print "[ERR] BugInit.loadMod\n\tCannot find XML file:\n\t" + path

def addInit(name, function):
	"""
	Calls function after all mods are loaded.

	If all mods have been loaded, the function is called immediately.
	Modules should use this function to setup a one-time initialization function
	that requires an initialized CyGlobalContext.

	name - short descriptive string used in debug messages, typically the module's name
	function - the function to call

	Use BugUtil.getFunction() to pass arguments to your function.
	"""
	g_initQueue.append((name, function))
	if g_initDone:
		callInits()

def callInits():
	"""Calls all of the stored init functions in the order they were added."""
	print "BugInit - calling init functions..."
	while g_initQueue:
		name, func = g_initQueue.pop(0)
		try:
			func()
		except:
			import BugUtil
			BugUtil.trace("BugInit - init '%s' failed" % name)
