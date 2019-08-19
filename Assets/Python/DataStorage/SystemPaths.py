# Data storage python, values are set in CvEventManager.onInit.
from os import path
appName = ""
userDir = ""
rootDir = ""
logDir = ""
appDir = ""
modDir = ""
modName = "Caveman2Cosmos"

def init():
	global appName, userDir, rootDir, logDir, appDir, modDir

	from os import mkdir
	import sys
	#############################################
	def __getRegValue(subkey, name):
		import _winreg
		key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, subkey)
		try:
			value = _winreg.QueryValueEx(key, name)
			return value[0]
		except:
			pass
	#############################################
	myDocuments = None
	try:
		myDocuments = __getRegValue(r"Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders", "Personal")
	except:
		try: # Vista
			myDocuments = __getRegValue(r"Software\Microsoft\Windows\CurrentVersion\Explorer\User Shell Folders", "Personal")
		except:
			pass

	if myDocuments:
		userDir = str(myDocuments) + "\My Games"
	else:
		print "Cannot find 'My Documents' folder registry key"
		userDir = "\\"

	appDir = str(path.dirname(sys.executable))
	appName = str(path.basename(appDir))

	modDir = appDir + "\Mods\Caveman2Cosmos"

	# Create UserSettings folders if missing.
	szTemp = modDir + "\UserSettings"
	if not path.isdir(szTemp):
		mkdir(szTemp)
	szTemp += "\DomesticAdv"
	if not path.isdir(szTemp):
		mkdir(szTemp)

	rootDir = str("%s\\%s" %(userDir, appName))
	logDir = "%s\\%s" %(rootDir, "Logs")

	print "------------------------ SystemPaths.init ---------------------------\n"
	sprint = " The following paths are now stored here with these variable names\n"
	sprint += "---------------------------------------------------------------------\n"
	sprint += "    appName: %s\n" %appName
	sprint += "    userDir: %s\n" %userDir
	sprint += "    rootDir: %s\n" %rootDir
	sprint += "     logDir: %s\n" %logDir
	sprint += "     appDir: %s\n" %appDir
	sprint += "     modDir: %s\n" %modDir
	sprint += "\nAccess:\n\timport SystemPaths as SP\n\tSP.rootdir\n"
	sprint += "------------------------------END------------------------------------"
	print sprint

	return rootDir

def isFile(aPath):
	return path.isfile(aPath)