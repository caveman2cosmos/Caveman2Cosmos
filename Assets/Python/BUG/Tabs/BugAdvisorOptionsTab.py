## BugAdvisorOptionsTab
##
## Tab for the BUG Advisor Options.
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: EmperorFool

import BugOptionsTab

class BugAdvisorOptionsTab(BugOptionsTab.BugOptionsTab):
	"BUG General Options Screen Tab"

	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "Advisors", "Advisors")

	def create(self, screen):
		self.createTab(screen)
		panel = self.createMainPanel(screen)
		left, center, right = self.addThreeColumnLayout(screen, panel, panel, True)

		self.addLabel(screen, left, "Domestic_Advisor", "Domestic [F1]:")
		leftL, leftR = self.addTwoColumnLayout(screen, left, "Advisors__CustDomAdv")
		self.addTextDropdown(screen, leftL, leftR, "CustDomAdv__ProductionGrouping", True)

		self.addLabel(screen, left, "Foreign_Advisor", "Foreign [F4]:")
		comboBox = "Advisors_ComboBoxEFA"
		screen.attachHBox(left, comboBox)
		self.addCheckbox(screen, comboBox, "Advisors__EFAGlanceTab")
		self.addTextDropdown(screen, None, comboBox, "Advisors__EFAGlanceAttitudes")
		self.addCheckbox(screen, left, "Advisors__EFAImprovedInfo")
		self.addCheckbox(screen, left, "Advisors__EFADealTurnsLeft")
		self.addCheckbox(screen, left, "MiscHover__BonusTradeDenial")

		self.addLabel(screen, center, "Technology_Advisor", "Technology [F6]:")
		self.addCheckbox(screen, center, "Advisors__GPTechPrefs")
		self.addCheckbox(screen, center, "Advisors__WideTechScreen")
		self.addCheckbox(screen, center, "Advisors__ShowTechEra")

		self.addSpacer(screen, center, "Advisors_Tab")

		self.addLabel(screen, center, "Religious_Advisor", "Religion [F7]:")
		self.addCheckbox(screen, center, "Advisors__BugReligiousTab")
		self.addTextDropdown(screen, center, center, "Advisors__ShowReligions", True)

		self.addLabel(screen, right, "Victory_Conditions", "Victory [F8]:")
		self.addCheckbox(screen, right, "Advisors__BugVictoriesTab")
		self.addCheckbox(screen, right, "Advisors__BugMembersTab")

		self.addSpacer(screen, right, "Advisors_Tab")

		self.addLabel(screen, right, "Info_Screens", "Info [F9]:")
		self.addCheckbox(screen, right, "Advisors__BugGraphsLogScale")
		self.addCheckbox(screen, right, "Advisors__BugStatsTab")
		self.addCheckbox(screen, right, "Advisors__BugInfoWondersPlayerColor")
