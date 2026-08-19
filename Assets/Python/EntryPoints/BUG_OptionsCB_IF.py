# BUG_OptionsCB_IF

# Initialize callback interface
def init():
	global g_options, g_screen
	import BugOptions
	g_options = BugOptions.getOptions()

init()

# Exits the screen after saving the options to disk
def handleBugExitButtonInput(argsList):
	import BugOptionsScreen
	BugOptionsScreen.g_screen.close()

# Opens the BUG web page externally.
def handleBugHelpButtonInput(argsList):
	import _misc
	_misc.LaunchDefaultBrowser("http://civ4bug.sourceforge.net/BUGMod.html")

def handleBugCheckboxClicked(argsList):
	bValue, szName = argsList
	option = g_options.getOption(szName)
	if option is not None:
		option.setValue(bValue)
		g_options.write()

def handleBugTextEditChange(argsList):
	szValue, szName = argsList
	option = g_options.getOption(szName)
	if option is not None:
		option.setValue(szValue)
		g_options.write()

def handleBugDropdownChange(argsList):
	iIndex, szName = argsList
	option = g_options.getOption(szName)
	if option is not None:
		option.setIndex(iIndex)
		g_options.write()

def handleBugIntDropdownChange(argsList):
	iIndex, szName = argsList
	option = g_options.getOption(szName)
	if option is not None:
		option.setIndex(iIndex)
		g_options.write()

def handleBugFloatDropdownChange(argsList):
	iIndex, szName = argsList
	option = g_options.getOption(szName)
	if option is not None:
		option.setIndex(iIndex)
		g_options.write()

def handleBugColorDropdownChange(argsList):
	iIndex, szName = argsList
	option = g_options.getOption(szName)
	if option is not None:
		option.setIndex(iIndex)
		g_options.write()

def handleBugSliderChanged(argsList):
	iValue, szName = argsList
	option = g_options.getOption(szName)
	if option is not None:
		option.setValue(iValue)
		g_options.write()
