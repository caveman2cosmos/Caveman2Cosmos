## BugMapOptionsTab
##
## Tab for the BUG Map Options.
##
## Copyright (c) 2009 The BUG Mod.
##
## Author: EmperorFool

import BugOptionsTab

class BugMapOptionsTab(BugOptionsTab.BugOptionsTab):
	"BUG Nap Options Screen Tab"

	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "Map", "Map")

	def create(self, screen):
		self.createTab(screen)
		panel = self.createMainPanel(screen)
		column = self.addOneColumnLayout(screen, panel)

		left, center, right = self.addThreeColumnLayout(screen, column, "Top", True)

		self.addLabel(screen, left, "StrategyOverlay", "Strategy Layer:")
		self.addCheckbox(screen, left, "StrategyOverlay__DotMapDrawDots")
		L, R = self.addTwoColumnLayout(screen, left, "DotMapBrightness")
		self.addSlider(screen, L, R, "StrategyOverlay__DotMapBrightness", False, False, False, "up", 0, 100)
		self.addSlider(screen, L, R, "StrategyOverlay__DotMapHighlightBrightness", False, False, False, "up", 0, 100)

		self.addLabel(screen, center, "CityBar", "CityBar:")
		self.addCheckbox(screen, center, "CityBar__AirportIcons")
		self.addCheckbox(screen, center, "CityBar__StarvationTurns")

		self.addLabel(screen, center, "TileHover", "Tile Hover:")
		self.addCheckbox(screen, center, "MiscHover__PlotWorkingCity")
		self.addCheckbox(screen, center, "MiscHover__PlotRecommendedBuild")
		self.addCheckbox(screen, center, "MiscHover__PartialBuilds")
		self.addCheckbox(screen, center, "MiscHover__LatLongCoords")

		self.addLabel(screen, right, "Misc", "Misc:")
		self.addCheckbox(screen, right, "MainInterface__FieldOfView")
		self.addCheckbox(screen, right, "MainInterface__FieldOfView_Remember", True)
		self.addCheckbox(screen, right, "EventSigns__Enabled")
		self.addCheckbox(screen, right, "Actions__IgnoreHarmlessBarbarians")
		self.addCheckbox(screen, right, "MiscHover__RemoveFeatureHealthEffects")
		self.addCheckbox(screen, right, "MiscHover__RemoveFeatureHealthEffectsCountOtherTiles", True)

		screen.attachHSeparator(column, column + "Sep1")
		left, center, right = self.addThreeColumnLayout(screen, column, "CityPlotsEnabled", True)
		self.addLabel(screen, left, "CityPlots", "City Tiles:")
		self.addCheckbox(screen, center, "CityBar__CityControlledPlots")
		self.addCheckbox(screen, right, "CityBar__CityPlotStatus")

		one, two, three, four, five = self.addMultiColumnLayout(screen, column, 5, "CityPlotsOptions")
		self.addLabel(screen, one, "WorkingPlots", "Working:")
		self.addCheckbox(screen, two, "CityBar__WorkingImprovedPlot")
		self.addCheckbox(screen, three, "CityBar__WorkingImprovablePlot")
		self.addCheckbox(screen, four, "CityBar__WorkingImprovableBonusPlot")
		self.addCheckbox(screen, five, "CityBar__WorkingUnimprovablePlot")
		self.addLabel(screen, one, "NotWorkingPlots", "Not Working:")
		self.addCheckbox(screen, two, "CityBar__NotWorkingImprovedPlot")
		self.addCheckbox(screen, three, "CityBar__NotWorkingImprovablePlot")
		self.addCheckbox(screen, four, "CityBar__NotWorkingImprovableBonusPlot")
		self.addCheckbox(screen, five, "CityBar__NotWorkingUnimprovablePlot")
