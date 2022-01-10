# Data storage python, values are set in CvEventManager.onInit.
from os import path, mkdir

def init():
	import sys
	global userDir, modDir, userSettingsDir

	#############################################
	def __getRegValue(subkey, name):
		import _winreg
		try:
			return _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER, subkey), name)[0]
		except:
			return None
	#############################################

	myDocuments = None
	try:
		myDocuments = __getRegValue(r"Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders", "Personal")
	except:
		try: # Vista
			myDocuments = __getRegValue(r"Software\Microsoft\Windows\CurrentVersion\Explorer\User Shell Folders", "Personal")
		except:
			print "Cannot find 'My Documents' folder registry key"

	if myDocuments:
		try:
			userDir = myDocuments.encode('utf-8')
		except:
			print "Encoding error for 'My Documents' path"

	dirBtS = path.dirname(sys.executable).encode('utf-8')

	modDir = dirBtS + "\\Mods\\Caveman2Cosmos"

	# Create UserSettings folders if missing.
	userSettingsDir = modDir + "\\UserSettings"
	initUserSettingsDir()

	userDir += "\\My Games\\" + path.basename(dirBtS)

	sprint = "------------------------ SystemPaths.init ---------------------------\n"
	sprint += " The following paths are now stored here with these variable names\n"
	sprint += "---------------------------------------------------------------------\nCaveman2Cosmos\n"
	sprint += "        userDir: %s\n" %userDir
	sprint += "         modDir: %s\n" %modDir
	sprint += "userSettingsDir: %s\n" %userSettingsDir
	sprint += "\nAccess:\n\timport SystemPaths as SP\n\tSP.userDir\n"
	sprint += "------------------------------END------------------------------------"
	print sprint

	return userDir

def initUserSettingsDir():
	if not path.isdir(userSettingsDir):
		print "SystemPaths - initUserSettingsDir()\n\tUserSettings directory " + userSettingsDir + " not found, creating it."
		mkdir(userSettingsDir)

	if not path.isdir(userSettingsDir + "\\DomesticAdv"):
		mkdir(userSettingsDir + "\\DomesticAdv")

def isFile(aPath):
	return path.isfile(aPath)

def joinModDir(*paths):
	return path.join(modDir, *paths)