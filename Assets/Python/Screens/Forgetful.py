from CvPythonExtensions import *

class Forgetful:
	def __init__(self):
		self.iForgetfulType = 0

	def interfaceScreen(self, screenId):
		self.screenId = screenId
		import ScreenResolution
		self.xRes = ScreenResolution.x
		self.yRes = ScreenResolution.y
		GC = CyGlobalContext()

		self.lForgetful = [
			["Bonus", GC.getBonusInfo, GC.getNumBonusInfos()],
			["Build", GC.getBuildInfo, GC.getNumBuildInfos()],
			["Building", GC.getBuildingInfo, GC.getNumBuildingInfos()],
#			["Calendar", GC.getCalendarInfo, GC.getNumCalendarInfos()],
			["Civic", GC.getCivicInfo, GC.getNumCivicInfos()],
			["CivicOption", GC.getCivicOptionInfo, GC.getNumCivicOptionInfos()],
			["Civilization", GC.getCivilizationInfo, GC.getNumCivilizationInfos()],
			["Climate", GC.getClimateInfo, GC.getNumClimateInfos()],
			["Command", GC.getCommandInfo, GC.getNumCommandInfos()],
			["Commerce", GC.getCommerceInfo, CommerceTypes.NUM_COMMERCE_TYPES],
			["Concept", GC.getConceptInfo, GC.getNumConceptInfos()],
			["Control", GC.getControlInfo, GC.getNumControlInfos()],
			["Corporation", GC.getCorporationInfo, GC.getNumCorporationInfos()],
			["CultureLevel", GC.getCultureLevelInfo, GC.getNumCultureLevelInfos()],
#			["Denial", GC.getDenialInfo, GC.getNumDenialInfos()],
			["Domain", GC.getDomainInfo, DomainTypes.NUM_DOMAIN_TYPES],
#			["Effect", GC.getEffectInfo, GC.getNumEffectInfos()],
#			["Emphasize", GC.getEmphasizeInfo, GC.getNumEmphasizeInfos()],
			["Era", GC.getEraInfo, GC.getNumEraInfos()],
			["Espionage", GC.getEspionageMissionInfo, GC.getNumEspionageMissionInfos()],
			["Event", GC.getEventInfo, GC.getNumEventInfos()],
			["EventTrigger", GC.getEventTriggerInfo, GC.getNumEventTriggerInfos()],
			["Feature", GC.getFeatureInfo, GC.getNumFeatureInfos()],
			["GameOption", GC.getGameOptionInfo, GC.getNumGameOptionInfos()],
			["GameSpeed", GC.getGameSpeedInfo, GC.getNumGameSpeedInfos()],
			["Goody", GC.getGoodyInfo, GC.getNumGoodyInfos()],
#			["GraphicOptions", GC.getGraphicOptionsInfo, GraphicOptionTypes.NUM_GRAPHICOPTION_TYPES],
			["Handicap", GC.getHandicapInfo, GC.getNumHandicapInfos()],
			["Hurry", GC.getHurryInfo, GC.getNumHurryInfos()],
			["Improvement", GC.getImprovementInfo, GC.getNumImprovementInfos()],
			["LeaderHead", GC.getLeaderHeadInfo, GC.getNumLeaderHeadInfos()],
			["Memory", GC.getMemoryInfo, MemoryTypes.NUM_MEMORY_TYPES],
			["Mission", GC.getMissionInfo, GC.getNumMissionInfos()],
#			["MPOption", GC.getMPOptionInfo, GC.getNumMPOptionInfos()],
			["NewConcept", GC.getNewConceptInfo, GC.getNumNewConceptInfos()],
			["Process", GC.getProcessInfo, GC.getNumProcessInfos()],
			["Project", GC.getProjectInfo, GC.getNumProjectInfos()],
			["Promotion", GC.getPromotionInfo, GC.getNumPromotionInfos()],
			["Religion", GC.getReligionInfo, GC.getNumReligionInfos()],
			["Route", GC.getRouteInfo, GC.getNumRouteInfos()],
#			["SeaLevel", GC.getSeaLevelInfo, GC.getNumSeaLevelInfos()],
#			["Season", GC.getSeasonInfo, GC.getNumSeasonInfos()],
			["SpecialBuilding", GC.getSpecialBuildingInfo, GC.getNumSpecialBuildingInfos()],
			["Specialist", GC.getSpecialistInfo, GC.getNumSpecialistInfos()],
			["SpecialUnit", GC.getSpecialUnitInfo, GC.getNumSpecialUnitInfos()],
			["Tech", GC.getTechInfo, GC.getNumTechInfos()],
			["Terrain", GC.getTerrainInfo, GC.getNumTerrainInfos()],
			["Trait", GC.getTraitInfo, GC.getNumTraitInfos()],
			["Unit", GC.getUnitInfo, GC.getNumUnitInfos()],
			["UnitAI", GC.getUnitAIInfo, UnitAITypes.NUM_UNITAI_TYPES],
			["UnitCombat", GC.getUnitCombatInfo, GC.getNumUnitCombatInfos()],
			["Upkeep", GC.getUpkeepInfo, GC.getNumUpkeepInfos()],
			["Victory", GC.getVictoryInfo, GC.getNumVictoryInfos()],
			["Vote", GC.getVoteInfo, GC.getNumVoteInfos()],
			["VoteSource", GC.getVoteSourceInfo, GC.getNumVoteSourceInfos()],
			["World", GC.getWorldInfo, GC.getNumWorldInfos()],
			["Yield", GC.getYieldInfo, YieldTypes.NUM_YIELD_TYPES],
		]
		self.iTypes = len(self.lForgetful)

		screen = CyGInterfaceScreen("ForgetfulScreen", screenId)
		screen.addPanel("", "", "", True, False, -10, -10, self.xRes + 20, self.yRes + 20, PanelStyles.PANEL_STYLE_MAIN)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.setLabel("", "", "<font=4b>TXT_KEY_XML_TAGS", 1<<2, self.xRes/2, 12, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)
		screen.setText("", "", "<font=4b>" + CyTranslator().getText("TXT_WORD_EXIT", ()), 1<<1, self.xRes - 12, 12, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		self.setDropDown()

	def setDropDown(self):
		szDropdownName = "ForgetfulType"
		screen = CyGInterfaceScreen("ForgetfulScreen", self.screenId)
		screen.addDropDownBoxGFC(szDropdownName, 12, 12, 180, WidgetTypes.WIDGET_GENERAL, 1, 2, FontTypes.GAME_FONT)
		for i in xrange(self.iTypes):
			screen.addPullDownString(szDropdownName, self.lForgetful[i][0], i, i, i == self.iForgetfulType)
		self.drawTable()

	def drawTable(self):
		Table = "ForgetfulTable"
		iWidth = self.xRes - 16
		w0 = (iWidth - 64)/3
		screen = CyGInterfaceScreen("ForgetfulScreen", self.screenId)
		screen.addTableControlGFC(Table, 4, 8, 52, iWidth, self.yRes - 60, True, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader(Table, 0, "ID", 64)
		screen.setTableColumnHeader(Table, 1, "NAME", w0)
		screen.setTableColumnHeader(Table, 2, "TYPE", w0)
		screen.setTableColumnHeader(Table, 3, "TEXT", w0)
		screen.enableSort(Table)
		eWidGen = WidgetTypes.WIDGET_GENERAL
		for item in xrange(self.lForgetful[self.iForgetfulType][2]):
			ItemInfo = self.lForgetful[self.iForgetfulType][1](item)
			screen.appendTableRow(Table)
			screen.setTableInt(Table, 0, item, str(item), "", eWidGen, 1, 2, 1<<0)
			screen.setTableText(Table, 1, item, ItemInfo.getDescription(), ItemInfo.getButton(), eWidGen, 1, 2, 1<<0)
			screen.setTableText(Table, 2, item, "<font=1>" + ItemInfo.getType(), "", eWidGen, 1, 2, 1<<0)
			screen.setTableText(Table, 3, item, "<font=1>" + ItemInfo.getTextKey(), "", eWidGen, 1, 2, 1<<0)

	def update(self, fDelta): return

	def handleInput(self, inputClass):
		if inputClass.getFunctionName() == "ForgetfulType":
			self.iForgetfulType = int(inputClass.getData())
			self.drawTable()

	def back(self):
		if self.iForgetfulType > 0:
			self.iForgetfulType -= 1
			self.setDropDown()
		else:
			self.iForgetfulType = self.iTypes - 1
			self.setDropDown()

	def forward(self):
		if self.iForgetfulType < self.iTypes - 1:
			self.iForgetfulType += 1
			self.setDropDown()
		else:
			self.iForgetfulType = 0
			self.setDropDown()

	def onClose(self):
		del self.lForgetful, self.screenId, self.xRes, self.yRes, self.iTypes