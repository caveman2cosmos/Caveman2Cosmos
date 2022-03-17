## Rise of Mankind Options Tab
##
## Tab to configure all Rise of Mankind options.
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: Zappara

from CvPythonExtensions import *
import BugOptionsTab

class ANDDiplomacyTab(BugOptionsTab.BugOptionsTab):

	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "DiplomacySettings", "Diplomacy")

	def create(self, screen):
		self.createTab(screen)
		panel = self.createMainPanel(screen)
		left, center, right = self.addThreeColumnLayout(screen, panel, panel, True)

		GAME = CyGlobalContext().getGame()
		bCanAdjustSettings = not GAME.isNetworkMultiPlayer() or GAME.getActivePlayer() == 0

		#Standard Settings
		control = self.addLabel(screen, left, "DiplomacySettings__DiplomacySettings")

		control = self.addCheckbox(screen, left, "DiplomacySettings__CanTradeTechs")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, left, "DiplomacySettings__CanBrokerTechs")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, left, "DiplomacySettings__CanTradeResources")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, left, "DiplomacySettings__CanTradeCities")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, left, "DiplomacySettings__CanTradeGold")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, left, "DiplomacySettings__CanTradeGoldPerTurn")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, left, "DiplomacySettings__CanTradeMaps")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, left, "DiplomacySettings__CanTradeVassals")
		screen.setEnabled(control, bCanAdjustSettings)
		# control = self.addCheckbox(screen, left, "DiplomacySettings__CanCapitulate")
		# screen.setEnabled(control, bCanAdjustSettings)


		self.addSpacer(screen, center, "General1")
		control = self.addCheckbox(screen, center, "DiplomacySettings__CanTradePeace")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "DiplomacySettings__CanTradeWar")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "DiplomacySettings__CanTradeEmbargo")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "DiplomacySettings__CanTradeReligions")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "DiplomacySettings__CanTradeCivics")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "DiplomacySettings__CanTradeOpenBorders")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "DiplomacySettings__CanTradeDefensivePact")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "DiplomacySettings__CanTradeAlliance")
		screen.setEnabled(control, bCanAdjustSettings)

		#Advanced Diplomacy
		self.addLabel(screen, right, "DiplomacySettings__AdvancedDiplomacy")

		control = self.addCheckbox(screen, right, "DiplomacySettings__AdvancedDiplomacy")
		screen.setEnabled(control, bCanAdjustSettings)
		self.addSpacer(screen, center, "General2")

		control = self.addCheckbox(screen, right, "DiplomacySettings__CanTradeWorkers")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "DiplomacySettings__CanTradeMilitary")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "DiplomacySettings__CanTradeCorporations")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "DiplomacySettings__CanTradeLimitedBorders")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "DiplomacySettings__CanTradeEmbassies")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "DiplomacySettings__CanTradeContact")
		screen.setEnabled(control, bCanAdjustSettings)



