## BugGeneralOptionsTab
##
## Tab for the BUG General Options (Main and City Screens).
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: EmperorFool

import BugOptionsTab

class BugGeneralOptionsTab(BugOptionsTab.BugOptionsTab):
	"BUG General Options Screen Tab"
	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "General", "General")

	def create(self, screen):
		self.createTab(screen)
		panel = self.createMainPanel(screen)
		column = self.addOneColumnLayout(screen, panel)

		left, center, right = self.addThreeColumnLayout(screen, column, "Top", True)

		self.addLabel(screen, left, "ProgressBars", "Progress Bars:")
		self.addTextDropdown(screen, left, left, "MainInterface__GPBar_Types", True)
		self.addSpacer(screen, left, "General1")

		self.addLabel(screen, left, "Leaderheads", "Leaderheads:")
		self.addCheckbox(screen, left, "MiscHover__LeaderheadHiddenAttitude")
		self.addCheckbox(screen, left, "MiscHover__LeaderheadWorstEnemy")
		self.addCheckbox(screen, left, "MiscHover__LeaderheadDefensivePacts")

		self.addLabel(screen, center, "AutoSave", "AutoSave:")
		self.addCheckbox(screen, center, "AutoSave__UsePlayerName")
		self.addCheckbox(screen, center, "AutoSave__CreateStartSave")
		self.addCheckbox(screen, center, "AutoSave__CreateEndSave")
		self.addCheckbox(screen, center, "AutoSave__CreateExitSave")
		self.addCheckbox(screen, center, "AutoSave__CreateLateSave")
		self.addTextEdit(screen, center, center, "AutoSave__Interval", True)
		self.addSpacer(screen, center, "General2")

		self.addLabel(screen, center, "Actions", "Actions:")
		self.addCheckbox(screen, center, "Actions__AskDeclareWarUnits")
		self.addCheckbox(screen, center, "Actions__PreChopForests")
		self.addCheckbox(screen, center, "Actions__PreChopImprovements")

		self.addLabel(screen, right, "TechWindow", "Tech Splash Screen:")
		self.addCheckbox(screen, right, "TechWindow__ViewType")
		self.addCheckbox(screen, right, "TechWindow__CivilopediaText")
		self.addSpacer(screen, right, "General3")

		self.addLabel(screen, right, "Misc", "Misc:")
		self.addTextDropdown(screen, right, right, "MainInterface__UnitIconSize", True)
		self.addCheckbox(screen, right, "MainInterface__UseAIPathing")
		self.addCheckbox(screen, right, "MainInterface__AutoEndDecisionlessTurns")
		self.addIntDropdown(screen, right, right, "MainInterface__AutoEndTurnDelay", True)
		self.addCheckbox(screen, right, "MainInterface__MinimizeAITurnSlices")
		self.addCheckbox(screen, right, "MainInterface__InverseShiftForQueueing")

		# Graphical Paging and viewport
		self.addLabel(screen, column, "View", "View options:")
		left, right = self.addTwoColumnLayout(screen, column, "ViewOptions", False)

		self.addCheckbox(screen, left, "MainInterface__EnableGraphicalPaging")
		self.addCheckbox(screen, right, "MainInterface__EnableViewports")
		self.addTextEdit(screen, right, right, "MainInterface__ViewportX", True)
		self.addTextEdit(screen, right, right, "MainInterface__ViewportY", True)
		self.addTextEdit(screen, right, right, "MainInterface__ViewportAutoSwitchBorder", True)
		self.addTextEdit(screen, right, right, "MainInterface__ViewportAutoCenterBorder", True)