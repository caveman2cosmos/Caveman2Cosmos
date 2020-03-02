## Rise of Mankind Options Tab
##
## Tab to configure all Rise of Mankind options.
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: Zappara

from CvPythonExtensions import *
import BugOptionsTab
gc = CyGlobalContext()

class RoMOptionsTab(BugOptionsTab.BugOptionsTab):

	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "RoMSettings", "Caveman2Cosmos")

	def create(self, screen):
		tab = self.createTab(screen)
		panel = self.createMainPanel(screen)
		left, center, right = self.addThreeColumnLayout(screen, panel, panel, True)

		control = None
		bCanAdjustSettings = not gc.getGame().isGameMultiPlayer() or gc.getGame().getActivePlayer() == 0

		self.createResourceDisplayPanel(screen, left)
		self.addSpacer(screen, left, "General1")

			#Flexible Difficulty
		control = self.addLabel(screen, left, "RoMSettings__FlexibleDifficulty")
		#screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, left, "RoMSettings__EnableFlexibleDifficulty")
		#screen.setEnabled(control, bCanAdjustSettings)
		control = self.addTextDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyMinimumDiff", False, "LAYOUT_LEFT")
		#screen.setEnabled(control, bCanAdjustSettings)
		control = self.addTextDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyMaximumDiff", False, "LAYOUT_LEFT")
		#screen.setEnabled(control, bCanAdjustSettings)
		control = self.addIntDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyTurnIncrements", False, "LAYOUT_LEFT")

		control = self.addCheckbox(screen, left, "RoMSettings__FlexibleDifficultyAI")
		#screen.setEnabled(control, bCanAdjustSettings)
		#control = self.addIntDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyMinRank", False, "LAYOUT_LEFT")
		#screen.setEnabled(control, bCanAdjustSettings)
		#control = self.addIntDropdown(screen, left, left, "RoMSettings__FlexibleDifficultyMaxRank", False, "LAYOUT_LEFT")
		#screen.setEnabled(control, bCanAdjustSettings)
		control = self.addTextDropdown(screen, left, left, "RoMSettings__CurrentDifficulty", False, "LAYOUT_LEFT")

		control = self.addTextDropdown(screen, left, left, "RoMSettings__PlayerColor", False, "LAYOUT_LEFT")

		#City Management
		self.addLabel(screen, center, "RoMSettings__CityManagement")
		control = self.addIntDropdown(screen, center, center, "RoMSettings__MaxBombardDefense", False, "LAYOUT_LEFT")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "RoMSettings__ShowCoastalBuildings")
		#screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "RoMSettings__HideObsoleteBuildings")
		#screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "RoMSettings__HideReplacedBuildings")
		#screen.setEnabled(control, bCanAdjustSettings)

		self.addCheckbox(screen, center, "RoMSettings__HideUnconstructableBuildings")
		self.addCheckbox(screen, center, "RoMSettings__HideUntrainableUnits")

		#Empire Management
		self.addLabel(screen, center, "RoMSettings__EmpireManagement")

		control = self.addCheckbox(screen, center, "RoMSettings__CanNotClaimOcean")
		screen.setEnabled(control, bCanAdjustSettings)

		control = self.addCheckbox(screen, center, "RoMSettings__ShowCivTraits")
		#screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "RoMSettings__NoFriendlyPillaging")
		#screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "RoMSettings__HideUnavailableBuilds")
		#screen.setEnabled(control, bCanAdjustSettings)

		if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_REVOLUTION):
			control = self.addCheckbox(screen, center, "RoMSettings__ShowRevCivics")
			#screen.setEnabled(control, bCanAdjustSettings)

		if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_PERSONALIZED_MAP):
			control = self.addCheckbox(screen, center, "RoMSettings__UseLandmarkNames")
			#screen.setEnabled(control, bCanAdjustSettings)

		control = self.addCheckbox(screen, center, "RoMSettings__EventImages")
		control = self.addCheckbox(screen, center, "RoMSettings__StrategicEvents")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, center, "RoMSettings__IgnoreDisabledBuildingAlerts")
		control = self.addCheckbox(screen, center, "RoMSettings__NoProductionPopup")


		#Game Settings
		self.addLabel(screen, right, "RoMSettings__GameSettings")
		control = self.addCheckbox(screen, right, "RoMSettings__DefenderWithdraw")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__DepletionMod")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__BetterAirInterception")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__BattlefieldPromotions")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__ImprovedXP")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__WarPrizes")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__InfrastructureIgnoresImprovements")
		screen.setEnabled(control, bCanAdjustSettings)


		if (gc.getInfoTypeForStringWithHiddenAssert("VICTORY_TOTAL") > 0 and gc.getGame().isVictoryValid(gc.getInfoTypeForString("VICTORY_TOTAL"))):
			control = self.addCheckbox(screen, right, "RoMSettings__MercyRule")
			screen.setEnabled(control, bCanAdjustSettings)

		control = self.addCheckbox(screen, right, "RoMSettings__RealisiticDiplomacy")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__NoStorms")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__MultipleReligionSpread")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addIntDropdown(screen, right, right, "RoMSettings__MaxRebaseRange")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addIntDropdown(screen, right, right, "RoMSettings__MaxUnitsPerTile")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__TerrainDamage")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__AllowTerraforming")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__Reforestation")
		screen.setEnabled(control, bCanAdjustSettings)
		control = self.addCheckbox(screen, right, "RoMSettings__SeaTunnels")
		screen.setEnabled(control, bCanAdjustSettings)
		if (gc.getGame().isOption(GameOptionTypes.GAMEOPTION_REALISTIC_CORPORATIONS)):
			control = self.addCheckbox(screen, right, "RoMSettings__NoAutoCorporationFounding")
			screen.setEnabled(control, bCanAdjustSettings)

		#self.addTextDropdown(screen, right, right, "RoMSettings__AliveCivilization", False, "LAYOUT_LEFT")

		if (gc.getGame().isGameMultiPlayer()):
			self.addCheckbox(screen, right, "RoMSettings__PlayWaitingAlertSound")
			self.addIntDropdown(screen, right, right, "RoMSettings__PlayWaitingAlertSoundTimer")


		#Reset Settings
		self.addSpacer(screen, left, "General2")
		self.addCheckbox(screen, left, "RoMSettings__RoMReset")

	def createResourceDisplayPanel(self, screen, panel):
		self.addLabel(screen, panel, "RiseOfMankindOptions", "Rise of Mankind options:")
		self.addCheckboxTextDropdown(screen, panel, panel, "RoMSettings__RoMResourceBar", "RoMSettings__RoMResourceBar_Types")
		self.addCheckbox(screen, panel, "RoMSettings__TechTreeEraColors")


		#col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "RoM_Resource_options")
		#self.addLabel(screen, left, "RoMSettings__Resource_amounts", ResourceOptionsTitle)
		#self.addCheckbox(screen, col1, "RoMSettings__RoMResourceBar")

		#screen.attachHSeparator(left, left + "SepInterface1")
		#screen.attachHSeparator(right, right + "SepInterface2")

		