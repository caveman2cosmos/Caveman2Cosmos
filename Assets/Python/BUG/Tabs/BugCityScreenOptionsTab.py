## BugCityScreenOptionsTab
##
## Tab for the BUG City Screen Options.
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: EmperorFool

import BugOptionsTab

class BugCityScreenOptionsTab(BugOptionsTab.BugOptionsTab):
	"BUG City Screen Options Screen Tab"

	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "CityScreen", "City Screen")

	def create(self, screen):
		self.createTab(screen)
		panel = self.createMainPanel(screen)
		column = self.addOneColumnLayout(screen, panel)

		LEFT, RIGHT = self.addTwoColumnLayout(screen, column, "Page", True)

		# Raw Yields
		self.addCheckbox(screen, LEFT, "CityScreen__RawYields")
		self.addTextDropdown(screen, LEFT, LEFT, "CityScreen__RawYieldsView", True)
		self.addTextDropdown(screen, LEFT, LEFT, "CityScreen__RawYieldsTiles", True)
		self.addSpacer(screen, LEFT, "CityScreen1")

		# Hurry Detail
		self.addLabel(screen, LEFT, "HurryDetail", "Hurry Detail:")
		aLeft, aRight = self.addTwoColumnLayout(screen, LEFT, "HurryDetail", False)
		self.addCheckbox(screen, aLeft, "CityBar__HurryAssist")
		self.addCheckbox(screen, aRight, "CityBar__HurryAssistIncludeCurrent")
		self.addCheckbox(screen, aLeft, "CityScreen__WhipAssist")
		self.addCheckbox(screen, aRight, "CityScreen__WhipAssistOverflowCountCurrentProduction")
		self.addCheckbox(screen, aLeft, "MiscHover__HurryOverflow")
		self.addCheckbox(screen, aRight, "MiscHover__HurryOverflowIncludeCurrent")
		self.addSpacer(screen, LEFT, "CityScreen2")

		# Building Actual Effects
		self.addLabel(screen, LEFT, "BuildingEffects", "Building Actual Effects in Hovers:")
		aLeft, aRight = self.addTwoColumnLayout(screen, LEFT, "BuildingEffects", False)
		self.addCheckbox(screen, aLeft, "MiscHover__BuildingActualEffects")
		self.addCheckbox(screen, aLeft, "MiscHover__BuildingAdditionalFood")
		self.addCheckbox(screen, aLeft, "MiscHover__BuildingAdditionalProduction")
		self.addCheckbox(screen, aLeft, "MiscHover__BuildingAdditionalCommerce")
		self.addCheckbox(screen, aLeft, "MiscHover__BuildingSavedMaintenance")
		self.addSpacer(screen, aRight, "CityScreen2a")
		self.addCheckbox(screen, aRight, "MiscHover__BuildingAdditionalHealth")
		self.addCheckbox(screen, aRight, "MiscHover__BuildingAdditionalHappiness")
		self.addCheckbox(screen, aRight, "MiscHover__BuildingAdditionalGreatPeople")
		self.addCheckbox(screen, aRight, "MiscHover__BuildingAdditionalDefense")
		self.addSpacer(screen, LEFT, "CityScreen3")

		# Great Person Bar
		self.addLabel(screen, LEFT, "GreatPersonBar", "Great Person Bar:")
		self.addCheckbox(screen, LEFT, "CityScreen__GreatPersonInfo")
		self.addCheckbox(screen, LEFT, "MiscHover__GreatPeopleRateBreakdown")
		self.addSpacer(screen, LEFT, "CityScreen4")

		# Production Queue
		aLeft, aCenter, aRight = self.addThreeColumnLayout(screen, LEFT, "ProductionDecay")
		self.addLabel(screen, aLeft, "ProductionDecay", "Decay:")
		self.addCheckbox(screen, aCenter, "CityScreen__ProductionDecayQueue")
		self.addCheckbox(screen, aRight, "CityScreen__ProductionDecayHover")
		self.addIntDropdown(screen, aLeft, aCenter, "CityScreen__ProductionDecayQueueUnitThreshold", True)
		self.addIntDropdown(screen, aLeft, aCenter, "CityScreen__ProductionDecayQueueBuildingThreshold", True)
		self.addIntDropdown(screen, None, aRight, "CityScreen__ProductionDecayHoverUnitThreshold")
		self.addIntDropdown(screen, None, aRight, "CityScreen__ProductionDecayHoverBuildingThreshold")

		#City Bar
		self.addLabel(screen, RIGHT, "CitybarHover", "City Bar Hover:")
		aLeft, aRight = self.addTwoColumnLayout(screen, RIGHT, "CityBarHover", False)

		self.addCheckbox(screen, aLeft, "CityBar__BaseValues")
		self.addCheckbox(screen, aLeft, "CityBar__Health")
		self.addCheckbox(screen, aLeft, "CityBar__Happiness")
		self.addCheckbox(screen, aLeft, "CityBar__FoodAssist")
		self.addCheckbox(screen, aLeft, "CityBar__BaseProduction")
		self.addCheckbox(screen, aLeft, "CityBar__TradeDetail")
		self.addCheckbox(screen, aLeft, "CityBar__Commerce")
		self.addCheckbox(screen, aLeft, "CityBar__CultureTurns")
		self.addCheckbox(screen, aLeft, "CityBar__GreatPersonTurns")

		self.addLabel(screen, aRight, "Cityanger", "City Anger:")
		self.addCheckbox(screen, aRight, "CityBar__HurryAnger")
		self.addCheckbox(screen, aRight, "CityBar__DraftAnger")

		self.addSpacer(screen, aRight, "CityScreen5")
		self.addCheckbox(screen, aRight, "CityBar__BuildingActualEffects")
		self.addCheckbox(screen, aRight, "CityBar__BuildingIcons")
		self.addCheckbox(screen, aRight, "CityBar__Specialists")
		self.addCheckbox(screen, aRight, "CityBar__RevoltChance")
		self.addCheckbox(screen, aRight, "CityBar__HideInstructions")
		self.addSpacer(screen, RIGHT, "CityScreen6")

		#Miscellaneous
		self.addLabel(screen, RIGHT, "Misc", "Miscellaneous:")
		self.addCheckbox(screen, RIGHT, "MiscHover__BaseCommerce")
		self.addCheckbox(screen, RIGHT, "CityScreen__Anger_Counter")
		self.addCheckbox(screen, RIGHT, "CityScreen__OnlyPresentReligions")
		self.addCheckbox(screen, RIGHT, "CityScreen__OnlyPresentCorporations")

		self.addCheckbox(screen, RIGHT, "MiscHover__UnitExperience")
		self.addCheckbox(screen, RIGHT, "MiscHover__UnitExperienceModifiers")
		self.addCheckbox(screen, RIGHT, "MiscHover__ConscriptUnit")
		self.addCheckbox(screen, RIGHT, "MiscHover__ConscriptLimit")
		self.addCheckbox(screen, RIGHT, "CityScreen__ProductionPopupTrainCivilianUnitsForever")
		self.addCheckbox(screen, RIGHT, "CityScreen__ProductionPopupTrainMilitaryUnitsForever")
