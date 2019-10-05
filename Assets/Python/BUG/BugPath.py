## BugPath
##
## Locates all of the necessary directories to provide access to data and asset files.
##
## It exposes several useful variables. Those ending in "Dir" are full paths to directories
## while those ending in "Folder" or "Name" are the base name of the file/folder.
## All directory functions return valid directories or the Python None value if not found.
##
##     Search order:
##
##       <user-dir>\<mod-name>      C:\Users\<user>\Documents\BUG Mod
##       <root-dir>\<mod-name>      C:\Users\<user>\Documents\Beyond the Sword\BUG Mod
##       <app-dir>\<mod-name>       C:\Programs\Sid Meier's Civilization IV\Beyond the Sword\BUG Mod
##       <mod-dir>\<data-folder>    C:\Programs\Sid Meier's Civilization IV\Beyond the Sword\Mods\BUG Mod x.x\Data
##       <mod-dir>                  C:\Programs\Sid Meier's Civilization IV\Beyond the Sword\Mods\BUG Mod x.x
##
## TODO
##
##   Add override for dataDir.
##   Add some other setting instead of modName for dataDir search.
##
## Based on CvPath by Dr. Elmer Jiggles.
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool

import os

## Finding and Creating Files and Directories

def findSettingsFile(name, subdir=None):
	# Returns the path to the named configuration file or None if not found.
	return getFilePath(_settingsDir, name, subdir)

def findDir(name):
	# Locates the named directory in dataDir.
	path = join(_modDir, name)
	if os.path.isdir(path):
		return path
	return None

def makeDir(name):
	# Creates a new directory in the dataDir folder.
	path = join(_modDir, name)
	if path and not os.path.isdir(path):
		try:
			os.makedirs(path)
			return path
		except IOError:
			import BugUtil
			BugUtil.trace("Cannot create directory '%s' in '%s", name, _modDir)
	return path

def findOrMakeDir(name):
	# Locates or creates the specified directory and returns the path to it.
	return findDir(name) or makeDir(name)


## Initialization
def init():
	print "[BugPath] initializing..."
	import SystemPaths as SP
	import BugConfigTracker

	global _modDir, _settingsDir
	_modDir = SP.modDir
	_settingsDir = SP.userSettingsDir
	BugConfigTracker.add("Mod_Directory", _modDir)
	BugConfigTracker.add("Settings_Directory", _settingsDir)

	global _assetFileSearchPaths
	_assetFileSearchPaths = _modDir + "\Assets"
	BugConfigTracker.add("Asset_Search_Paths", _assetFileSearchPaths)

	print "[BugPath] initialized."

## None-Safe Path/Directory/File Functions

def getFilePath(root, name, subdir=None):
	# Returns the full path to the named file, or None if it doesn't exist.
	if not root:
		print "Invalid root directory looking for '%s'" %name
	if subdir:
		path = join(root, subdir, name)
	else:
		path = join(root, name)
	if os.path.isfile(path):
		return path
	return None

def join(*paths):
	for path in paths:
		if path is None:
			return None
	return os.path.join(*paths)
