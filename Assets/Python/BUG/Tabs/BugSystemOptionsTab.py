## BugSystemOptionsTab
##
## Tab for the BUG System Options (logging, updates, SVN, and system paths).
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool
from CvPythonExtensions import *
import BugOptionsTab
import BugConfigTracker
GC = CyGlobalContext()

class BugSystemOptionsTab(BugOptionsTab.BugOptionsTab):
	"BUG System Options Screen Tab"

	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "System", "System")

	def create(self, screen):
		self.createTab(screen)
		panel = self.createMainPanel(screen)
		column = self.addOneColumnLayout(screen, panel)

		left, right = self.addTwoColumnLayout(screen, column, "Bottom", False)
		screen.setLayoutFlag(right, "LAYOUT_RIGHT")
		screen.setLayoutFlag(right, "LAYOUT_SIZE_HPREFERREDEXPANDING")
		self.addLabel(screen, left, "Version", "Caveman2Cosmos " + GC.getGame().getC2CVersion())

		screen.attachHSeparator(column, column + "Sep2")
		self.addLabel(screen, column, "Debug_Logging", "Debugging Output:")
		left, center, right = self.addThreeColumnLayout(screen, column)
		self.addTextDropdown(screen, left, left, "Core__ScreenLogLevel")
		self.addTextDropdown(screen, center, center, "Core__FileLogLevel")
		self.addCheckbox(screen, right, "Core__LogTime")

		screen.attachHSeparator(column, column + "Sep3")
		items = BugConfigTracker.combine()
		itemNum = 0

		for item in items:
			itemNum += 1
			subitemNum = 0
			self.addLabel(screen, column, item[0], item[0])
			for value in item[1]:
				subitemNum += 1
				self.addLabel(screen, column, "ConfigSubitem-%d-%d" % (itemNum, subitemNum), "  - " + value)
