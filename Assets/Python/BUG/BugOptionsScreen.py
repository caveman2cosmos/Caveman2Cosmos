## BugOptionsScreen
##
## Displays the BUG Options Screen and its tabs.
##
## For input handlers see BUG_OptionsCB_IF in EntryPoints.

from CvPythonExtensions import *
import BugConfig
import BugCore
import BugUtil

def init(): return

def showOptionsScreen(argsList=None):
	g_screen.interfaceScreen()

def clearAllTranslations(argsList=None):
	g_screen.clearAllTranslations()


class BugOptionsScreen:
	# BUG Mod Options Screen
	
	def __init__(self):
		import BugOptions
		self.iScreenHeight = 50
		self.options = BugOptions.getOptions()
		self.tabs = []

	def addTab(self, tab):
		self.tabs.append(tab)
		tab.setOptions(self.options)

	def interfaceScreen(self):
		import CvScreenEnums
		screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
		xRes = screen.getXResolution()
		yRes = screen.getYResolution()
		xSize = 950
		ySize = 715
		if xRes > 2500 :
			xSize = 1650
			ySize = 1015
		"Initial creation of the screen"
		title = BugUtil.getPlainText("TXT_KEY_BUG_OPT_TITLE", "C2C Options")
		self.pTabControl = CyGTabCtrl(title, False, False)
		self.pTabControl.setModal(1)
		self.pTabControl.setSize(xSize, ySize)
		self.pTabControl.setControlsExpanding(False)
		self.pTabControl.setColumnLength(self.iScreenHeight)
		self.createTabs()

	def createTabs(self):
		for i, tab in enumerate(self.tabs):
			tab.create(self.pTabControl)

	def clearAllTranslations(self):
		"Clear the translations of all tabs in response to the user choosing a language"
		for tab in self.tabs:
			tab.clearTranslation()

	def close(self):
		self.options.write()
		self.pTabControl.destroy()
		self.pTabControl = None
		CyGlobalContext().refreshOptionsBUG()

	def setOptionValue(self, name, value):
		option = self.options.getOption(name)
		if option is not None:
			option.setValue(value)

	def setOptionIndex(self, name, index):
		option = self.options.getOption(name)
		if option is not None:
			option.setIndex(index)

g_screen = BugOptionsScreen()

## Configuration

class ScreenConfig:

	def __init__(self, id):
		self.id = id
		self.tabs = []

	def addTab(self, tab):
		self.tabs.append(tab)

class ScreenHandler(BugConfig.Handler):

	TAG = "screen"

	def __init__(self):
		BugConfig.Handler.__init__(self, ScreenHandler.TAG, "id", TabHandler.TAG)
		self.addAttribute("id", True)

	def handle(self, element, id):
		screen = ScreenConfig(id)
		element.setState("options-screen", screen)
		BugCore.game._addScreen(screen)

class TabHandler(BugConfig.Handler):

	TAG = "tab"

	def __init__(self):
		BugConfig.Handler.__init__(self, TabHandler.TAG, "id screen module class")
		self.addAttribute("screen")
		self.addAttribute("module", True, True)
		self.addAttribute("class", True, False, None, "module")
		self.addAttribute("id", True, False, None, "module")

	def handle(self, element, screenId, module, clazz, id):
		if screenId:
			screen = BugCore.game._getScreen(screenId)
		else:
			screen = element.getState("options-screen")
		if not screen:
			raise BugUtil.ConfigError("Element <%s> %s must be in <screen> or have screen attribute", id, element.tag)
		screen.addTab(id)
		tab = BugUtil.callFunction(module, clazz, g_screen)
		g_screen.addTab(tab)
