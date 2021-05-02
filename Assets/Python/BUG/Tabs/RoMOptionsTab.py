## Rise of Mankind Options Tab
##
## Tab to configure all Rise of Mankind options.
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: Zappara

from CvPythonExtensions import CyGlobalContext, GameOptionTypes
import BugOptionsTab

class RoMOptionsTab(BugOptionsTab.BugOptionsTab):

	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "RoMSettings", "Caveman2Cosmos")

	def create(self, screen):
		self.createTab(screen)
		panel = self.createMainPanel(screen)
		left, center, right = self.addThreeColumnLayout(screen, panel, panel, True)

		GC = CyGlobalContext()
		GAME = GC.getGame()
		bCanAdjustSettings = not GAME.isGameMultiPlayer() or GAME.getActivePlayer() == 0

		#Flexible Difficulty
		self.addLabel(screen, left, "RoMSettings__FlexibleDifficulty")

		screen.setEnabled(self.addCheckbox(screen, left, "RoMSettings__EnableFlexibleDifficulty"), bCanAdjustSettings)
		screen.setEnabled(self.addTextDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyMinimumDiff", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		screen.setEnabled(self.addTextDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyMaximumDiff", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		screen.setEnabled(self.addIntDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyTurnIncrements", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, left, "RoMSettings__FlexibleDifficultyAI"), bCanAdjustSettings)
		screen.setEnabled(self.addTextDropdown(screen, left, left, "RoMSettings__CurrentDifficulty", False, "LAYOUT_LEFT"), bCanAdjustSettings)

		self.addTextDropdown(screen, left, left, "RoMSettings__PlayerColor", False, "LAYOUT_LEFT") # This might be OOS safe, but I'm not sure... Toffer.

		#Reset Settings
		self.addSpacer(screen, left, "")
		self.addCheckbox(screen, left, "RoMSettings__RoMReset")

		#City Management
		self.addLabel(screen, center, "RoMSettings__CityManagement")

		screen.setEnabled(self.addIntDropdown(screen, center, center, "RoMSettings__MaxBombardDefense", False, "LAYOUT_LEFT"), bCanAdjustSettings)

		self.addCheckbox(screen, center, "RoMSettings__HideReplacedBuildings")

		#Empire Management
		self.addLabel(screen, center, "RoMSettings__EmpireManagement")

		self.addCheckbox(screen, center, "RoMSettings__ShowCivTraits")

		self.addCheckbox(screen, center, "RoMSettings__HideUnavailableBuilds")

		if GAME.isOption(GameOptionTypes.GAMEOPTION_REVOLUTION):
			self.addCheckbox(screen, center, "RoMSettings__ShowRevCivics")

		if GAME.isOption(GameOptionTypes.GAMEOPTION_PERSONALIZED_MAP):
			self.addCheckbox(screen, center, "RoMSettings__UseLandmarkNames")

		self.addCheckbox(screen, center, "RoMSettings__EventImages")

		screen.setEnabled(self.addCheckbox(screen, center, "RoMSettings__StrategicEvents"), bCanAdjustSettings)

		self.addCheckbox(screen, center, "RoMSettings__IgnoreDisabledBuildingAlerts")
		self.addCheckbox(screen, center, "RoMSettings__NoProductionPopup")

		screen.setEnabled(self.addCheckbox(screen, center, "RoMSettings__InfrastructureIgnoresImprovements"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, center, "RoMSettings__CanNotClaimOcean"), bCanAdjustSettings)

		self.addCheckbox(screen, center, "RoMSettings__NoFriendlyPillaging")

		#Game Settings
		self.addLabel(screen, right, "RoMSettings__GameSettings")

		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__DefenderWithdraw"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__DepletionMod"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__BetterAirInterception"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__BattlefieldPromotions"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__ImprovedXP"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__WarPrizes"), bCanAdjustSettings)

		if GC.getInfoTypeForStringWithHiddenAssert("VICTORY_TOTAL") > 0 and GAME.isVictoryValid(GC.getInfoTypeForString("VICTORY_TOTAL")):
			screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__MercyRule"), bCanAdjustSettings)

		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__RealisiticDiplomacy"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__NoStorms"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__MultipleReligionSpread"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__TelepathicReligion"), bCanAdjustSettings)

		screen.setEnabled(self.addIntDropdown(screen, right, right, "RoMSettings__MaxRebaseRange"), bCanAdjustSettings)
		screen.setEnabled(self.addIntDropdown(screen, right, right, "RoMSettings__MaxUnitsPerTile"), bCanAdjustSettings)

		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__TerrainDamage"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__AllowTerraforming"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__Reforestation"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__SeaTunnels"), bCanAdjustSettings)

		if GAME.isOption(GameOptionTypes.GAMEOPTION_REALISTIC_CORPORATIONS):
			screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__NoAutoCorporationFounding"), bCanAdjustSettings)

		if GAME.isGameMultiPlayer():
			self.addCheckbox(screen, right, "RoMSettings__PlayWaitingAlertSound")
			self.addIntDropdown(screen, right, right, "RoMSettings__PlayWaitingAlertSoundTimer")
