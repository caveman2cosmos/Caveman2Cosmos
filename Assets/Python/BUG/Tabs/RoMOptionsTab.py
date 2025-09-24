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

		GAME = CyGlobalContext().getGame()
		bCanAdjustSettings = not GAME.isNetworkMultiPlayer() or GAME.getActivePlayer() == 0

		#Reset Settings
		self.addCheckbox(screen, left, "RoMSettings__RoMReset")
		self.addSpacer(screen, left, "")

		self.addTextDropdown(screen, left, left, "RoMSettings__PlayerColor", False, "LAYOUT_LEFT") # This might be OOS safe, but I'm not sure... Toffer.
		screen.setEnabled(self.addTextDropdown(screen, left, left, "RoMSettings__CurrentDifficulty", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		self.addSpacer(screen, left, "")
		screen.setEnabled(self.addCheckbox(screen, left, "RoMSettings__HistoricalAccurateCalendar"), bCanAdjustSettings)
		#Flexible Difficulty
		screen.setEnabled(self.addCheckbox(screen, left, "RoMSettings__EnableFlexibleDifficulty"), bCanAdjustSettings)
		screen.setEnabled(self.addTextDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyMinimumDiff", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		screen.setEnabled(self.addTextDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyMaximumDiff", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		screen.setEnabled(self.addIntDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyTurnIncrements", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, left, "RoMSettings__FlexibleDifficultyAI"), bCanAdjustSettings)
		screen.setEnabled(self.addTextDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyAIMinimumDiff", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		screen.setEnabled(self.addTextDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyAIMaximumDiff", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		screen.setEnabled(self.addIntDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyAITurnIncrements", False, "LAYOUT_LEFT"), bCanAdjustSettings)
        

		#City Management
		self.addLabel(screen, center, "RoMSettings__CityManagement")

		self.addCheckbox(screen, center, "RoMSettings__HideReplacedBuildings")

		screen.setEnabled(self.addIntDropdown(screen, center, center, "RoMSettings__MaxBombardDefense", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		screen.setEnabled(self.addIntDropdown(screen, center, center, "RoMSettings__MinCityDistance", False, "LAYOUT_LEFT"), bCanAdjustSettings)

		screen.setEnabled(self.addCheckbox(screen, center, "RoMSettings__ReligionDecay"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, center, "RoMSettings__MultipleReligionSpread"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, center, "RoMSettings__TelepathicReligion"), bCanAdjustSettings)

		#Empire Management
		self.addLabel(screen, center, "RoMSettings__EmpireManagement")

		self.addCheckbox(screen, center, "RoMSettings__HideUnavailableBuilds")

		if GAME.isOption(GameOptionTypes.GAMEOPTION_UNSUPPORTED_REVOLUTION):
			self.addCheckbox(screen, center, "RoMSettings__ShowRevCivics")

		if GAME.isOption(GameOptionTypes.GAMEOPTION_MAP_PERSONALIZED):
			self.addCheckbox(screen, center, "RoMSettings__UseLandmarkNames")

		self.addCheckbox(screen, center, "RoMSettings__IgnoreDisabledBuildingAlerts")

		screen.setEnabled(self.addCheckbox(screen, center, "RoMSettings__InfrastructureIgnoresImprovements"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, center, "RoMSettings__CanNotClaimOcean"), bCanAdjustSettings)

		self.addCheckbox(screen, center, "RoMSettings__NoFriendlyPillaging")

		screen.setEnabled(self.addCheckbox(screen, center, "RoMSettings__DepletionMod"), bCanAdjustSettings)

		screen.setEnabled(self.addCheckbox(screen, center, "RoMSettings__GreaterGreatFarmer"), bCanAdjustSettings)

		#Game Settings
		self.addLabel(screen, right, "RoMSettings__GameSettings")

		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__DefenderWithdraw"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__BetterAirInterception"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__BattlefieldPromotions"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__Starsigns"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__ImprovedXP"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__WarPrizes"), bCanAdjustSettings)

		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__MercyRule"), bCanAdjustSettings)

		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__RealisiticDiplomacy"), bCanAdjustSettings)

		screen.setEnabled(self.addIntDropdown(screen, right, right, "RoMSettings__MaxRebaseRange"), bCanAdjustSettings)
		screen.setEnabled(self.addIntDropdown(screen, right, right, "RoMSettings__MaxUnitsPerTile"), bCanAdjustSettings)

		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__TerrainDamage"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__AllowTerraforming"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__Reforestation"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__SeaTunnels"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__FogWarDecay"), bCanAdjustSettings)
		screen.setEnabled(self.addIntDropdown(screen, right, right, "RoMSettings__FogWarNbTurns"), bCanAdjustSettings)
		screen.setEnabled(self.addTextDropdown(screen, left, left, "RoMSettings__MinorCivTech", False, "LAYOUT_LEFT"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__AssassinateMinimal"), bCanAdjustSettings)
		screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__AssassinateChoice"), bCanAdjustSettings)

		if GAME.isOption(GameOptionTypes.GAMEOPTION_ADVANCED_REALISTIC_CORPORATIONS):
			screen.setEnabled(self.addCheckbox(screen, right, "RoMSettings__NoAutoCorporationFounding"), bCanAdjustSettings)

		if GAME.isNetworkMultiPlayer():
			self.addCheckbox(screen, right, "RoMSettings__PlayWaitingAlertSound")
			self.addIntDropdown(screen, right, right, "RoMSettings__PlayWaitingAlertSoundTimer")
