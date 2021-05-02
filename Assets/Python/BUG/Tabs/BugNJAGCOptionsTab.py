## BugNJAGCOptionsTab
##
## Tab for the BUG NJAGC Options.
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: EmperorFool

import BugOptionsTab

class BugNJAGCOptionsTab(BugOptionsTab.BugOptionsTab):

	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "NJAGC", "Clock")

	def create(self, screen):
		self.createTab(screen)
		panel = self.createMainPanel(screen)
		column = self.addOneColumnLayout(screen, panel)

		left, center, right = self.addThreeColumnLayout(screen, column, "Views")
		#self.addSpacer(screen, center, "Scores_New_Columns", 3)

		self.addCheckbox(screen, left, "NJAGC__AlternateText")
		self.addIntDropdown(screen, right, center, "NJAGC__AltTiming")
		screen.attachHSeparator(column, column + "Sep")

		left, right = self.addTwoColumnLayout(screen, column, "Views", True)
		self.addLabel(screen, left, "NJAGC_Regular", "Standard View:")
		self.addCheckbox(screen, left, "NJAGC__ShowCompletedTurns")
		self.addCheckbox(screen, left, "NJAGC__ShowTotalTurns")
		self.addCheckbox(screen, left, "NJAGC__ShowCompletedPercent")
		self.addCheckbox(screen, left, "NJAGC__ShowDate")

		self.addLabel(screen, right, "NJAGC_Alternate", "Alternate View:")
		self.addCheckbox(screen, right, "NJAGC__ShowAltCompletedTurns")
		self.addCheckbox(screen, right, "NJAGC__ShowAltTotalTurns")
		self.addCheckbox(screen, right, "NJAGC__ShowAltCompletedPercent")
		self.addCheckbox(screen, right, "NJAGC__ShowAltDate")
