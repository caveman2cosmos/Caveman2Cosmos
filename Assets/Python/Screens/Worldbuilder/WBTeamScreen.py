from CvPythonExtensions import *
import CvScreenEnums
import WBProjectScreen
import WBPlayerScreen
import WBPlayerUnits
import WBInfoScreen
GC = CyGlobalContext()

class WBTeamScreen:

	def __init__(self, WB):
		self.WB = WB
		self.iTable_Y = 110
		self.iTeam = -1
		self.pTeam = None
		self.iChange = 1
		self.bRemove = False
		self.iSelectedYield = 0
		self.lImprovements = []
		self.lRoutes = []
		self.lVoteBuildings = []
		self.lAbilities = []

	def interfaceScreen(self, iTeamX):
		screen = CyGInterfaceScreen("WBTeamScreen", CvScreenEnums.WB_TEAM)
		self.iTeam = iTeamX
		self.pTeam = GC.getTeam(self.iTeam)

		iWidth = screen.getXResolution() /4 - 20

		screen.setRenderInterfaceOnly(True)
		screen.addPanel( "MainBG", u"", u"", True, False, -10, -10, screen.getXResolution() + 20, screen.getYResolution() + 20, PanelStyles.PANEL_STYLE_MAIN )
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		sText = "<font=4b>" + CyTranslator().getText("TXT_KEY_WB_TEAM_DATA",()).upper() + " (ID: " + str(self.iTeam) + ")</font>"
		screen.setLabel("TeamHeader", "Background", sText, 1<<2, screen.getXResolution()/2, 20, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText("TeamExit", "Background", "<font=4>" + CyTranslator().getText("TXT_WORD_EXIT", ()).upper() + "</font>", 1<<1, screen.getXResolution() - 30, screen.getYResolution() - 42, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		screen.addDropDownBoxGFC("CurrentPage", 20, screen.getYResolution() - 42, iWidth - 20, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA", ()), 0, 0, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_TEAM_DATA", ()), 1, 1, True)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_PROJECT", ()), 2, 2, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_TECH", ()), 3, 3, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ()), 4, 4, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

		screen.addDropDownBoxGFC("ChangeType", screen.getXResolution() - 170, 20, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_CITY_ADD", ()), 1, 1, not self.bRemove)
		screen.addPullDownString("ChangeType", CyTranslator().getText("TXT_KEY_WB_CITY_REMOVE", ()), 0, 0, self.bRemove)

		screen.addDropDownBoxGFC("ChangeBy", screen.getXResolution() - 170, 50, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		i = 1
		while i < 1000001:
			screen.addPullDownString("ChangeBy", "(+/-) " + str(i), i, i, self.iChange == i)
			if str(i)[0] == "1":
				i *= 5
			else:
				i *= 2

		screen.addDropDownBoxGFC("CurrentTeam", 20, 20, iWidth - 20, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for i in xrange(GC.getMAX_TEAMS()):
			if GC.getTeam(i).isAlive():
				iLeader = GC.getTeam(i).getLeaderID()
				sName = GC.getPlayer(iLeader).getName()
				if GC.getTeam(i).getNumMembers() > 1:
					sName += " (" + str(GC.getTeam(i).getNumMembers()) + " " + CyTranslator().getText("TXT_KEY_MEMBERS_TITLE", ()) + ")"
				screen.addPullDownString("CurrentTeam", sName, i, i, i == self.iTeam)

		screen.addDropDownBoxGFC("MergeTeam", 20, 50, iWidth - 20, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("MergeTeam", CyTranslator().getText("TXT_KEY_WB_MERGE_TEAM",()), -1, -1, True)
		for i in xrange(GC.getMAX_TEAMS()):
			if GC.getTeam(i).isAlive():
				if i == self.iTeam: continue
				iLeader = GC.getTeam(i).getLeaderID()
				sName = GC.getPlayer(iLeader).getName()
				if GC.getTeam(i).getNumMembers() > 1:
					sName += " (" + str(GC.getTeam(i).getNumMembers()) + " " + CyTranslator().getText("TXT_KEY_MEMBERS_TITLE", ()) + ")"
				screen.addPullDownString("MergeTeam", sName, i, i, False)

		self.lImprovements = []
		for i in xrange(GC.getNumImprovementInfos()):
			ItemInfo = GC.getImprovementInfo(i)
			if ItemInfo.isGraphicalOnly(): continue
			self.lImprovements.append([ItemInfo.getDescription(), i])
		self.lImprovements.sort()

		self.lRoutes = []
		for i in xrange(GC.getNumRouteInfos()):
			ItemInfo = GC.getRouteInfo(i)
			self.lRoutes.append([ItemInfo.getDescription(), i])
		self.lRoutes.sort()

		self.lVoteBuildings = []
		for i in xrange(GC.getNumVoteSourceInfos()):
			iVoteBuilding = -1
			for j in xrange(GC.getNumBuildingInfos()):
				if GC.getBuildingInfo(j).getVoteSourceType() == i:
					iVoteBuilding = j
					break
			if iVoteBuilding == -1: continue
			self.lVoteBuildings.append([GC.getBuildingInfo(iVoteBuilding).getDescription(), iVoteBuilding])
		self.lVoteBuildings.sort()

		self.lAbilities = []
		for _ in xrange(13):
			self.lAbilities.append([WidgetTypes.WIDGET_GENERAL, -1])
		for i in xrange(GC.getNumTechInfos()):
			ItemInfo = GC.getTechInfo(i)
			if ItemInfo.isMapCentering():
				self.lAbilities[0][0] = WidgetTypes.WIDGET_HELP_MAP_CENTER
				self.lAbilities[0][1] = i
			if ItemInfo.isMapTrading():
				self.lAbilities[1][0] = WidgetTypes.WIDGET_HELP_MAP_TRADE
				self.lAbilities[1][1] = i
			if ItemInfo.isTechTrading():
				self.lAbilities[2][0] = WidgetTypes.WIDGET_HELP_TECH_TRADE
				self.lAbilities[2][1] = i
			if ItemInfo.isGoldTrading():
				self.lAbilities[3][0] = WidgetTypes.WIDGET_HELP_GOLD_TRADE
				self.lAbilities[3][1] = i
			if ItemInfo.isOpenBordersTrading():
				self.lAbilities[4][0] = WidgetTypes.WIDGET_HELP_OPEN_BORDERS
				self.lAbilities[4][1] = i
			if ItemInfo.isDefensivePactTrading():
				self.lAbilities[5][0] = WidgetTypes.WIDGET_HELP_DEFENSIVE_PACT
				self.lAbilities[5][1] = i
			if ItemInfo.isPermanentAllianceTrading():
				self.lAbilities[6][0] = WidgetTypes.WIDGET_HELP_PERMANENT_ALLIANCE
				self.lAbilities[6][1] = i
			if ItemInfo.isVassalStateTrading():
				self.lAbilities[7][0] = WidgetTypes.WIDGET_HELP_VASSAL_STATE
				self.lAbilities[7][1] = i
			if ItemInfo.isBridgeBuilding():
				self.lAbilities[8][0] = WidgetTypes.WIDGET_HELP_BUILD_BRIDGE
				self.lAbilities[8][1] = i
			if ItemInfo.isIrrigation():
				self.lAbilities[9][0] = WidgetTypes.WIDGET_HELP_IRRIGATION
				self.lAbilities[9][1] = i
			if ItemInfo.isIgnoreIrrigation():
				self.lAbilities[10][0]= WidgetTypes.WIDGET_HELP_IGNORE_IRRIGATION
				self.lAbilities[10][1]= i
			if ItemInfo.isWaterWork():
				self.lAbilities[11][0] = WidgetTypes.WIDGET_HELP_WATER_WORK
				self.lAbilities[11][1] = i
			if ItemInfo.isExtraWaterSeeFrom():
				self.lAbilities[12][0] = WidgetTypes.WIDGET_HELP_LOS_BONUS
				self.lAbilities[12][1] = i

		self.placeStats()
		self.placeMembers()
		self.placeAbilities()
		self.placeImprovements()
		self.placeDomains()
		self.placeRoutes()
		self.placeVotes()

	def placeStats(self):
		screen = CyGInterfaceScreen("WBTeamScreen", CvScreenEnums.WB_TEAM)

		iY = self.iTable_Y
		screen.setButtonGFC("NukeInterceptionPlus", u"", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("NukeInterceptionMinus", u"", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_WB_NUKE_INTERCEPTION",(self.pTeam.getNukeInterception(),)) + "</font>"
		screen.setLabel("NukeInterceptionText", "Background", sText, 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("EnemyWWPlus", u"", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("EnemyWWMinus", u"", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("TXT_KEY_WB_ENEMY_WAR_WEARINESS",(self.pTeam.getEnemyWarWearinessModifier(),)) + "</font>"
		screen.setLabel("EnemyWWText", "Background", sText, 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("MasterPowerPlus", u"", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("MasterPowerMinus", u"", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("[ICON_STRENGTH]",()) + CyTranslator().getText("TXT_KEY_MISC_MASTER",()) + ": " + str(self.pTeam.getMasterPower()) + "</font>"
		screen.setLabel("MasterPowerText", "Background", sText, 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("VassalPowerPlus", u"", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("VassalPowerMinus", u"", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("[ICON_STRENGTH]",()) + CyTranslator().getText("TXT_KEY_MISC_VASSAL_SHORT",()) + ": " + str(self.pTeam.getVassalPower()) + "</font>"
		screen.setLabel("VassalPowerText", "Background", sText, 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("EspionageEverPlus", u"", "", 20, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("EspionageEverMinus", u"", "", 45, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = "<font=3>" + CyTranslator().getText("[ICON_ESPIONAGE]",()) + CyTranslator().getText("TXT_KEY_WB_EVER",()) + ": " + str(self.pTeam.getEspionagePointsEver()) + "</font>"
		screen.setLabel("EspionageEverText", "Background", sText, 1<<0, 75, iY + 1, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def placeVotes(self):
		screen = CyGInterfaceScreen("WBTeamScreen", CvScreenEnums.WB_TEAM)
		iWidth = screen.getXResolution() /4 - 40
		iX = 20
		iY = self.iTable_Y + 180 + 13 * 24 + 2
		screen.setLabel("VoteHeader", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_GUARANTEED_ELIGIBILITY",()) + "</font>", 1<<2, iX + iWidth/2, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		iY += 30
		iHeight = (screen.getYResolution() - iY - 40) /24 * 24 + 2
		screen.addTableControlGFC("WBTeamVotes", 1, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		screen.setTableColumnHeader( "WBTeamVotes", 0, "", iWidth)
		for item in self.lVoteBuildings:
			iVoteSource = GC.getBuildingInfo(item[1]).getVoteSourceType()
			iRow = screen.appendTableRow("WBTeamVotes")
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if self.pTeam.isForceTeamVoteEligible(iVoteSource):
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			screen.setTableText("WBTeamVotes", 0, iRow, "<font=3>" + sColor + item[0] + "</font></color>", GC.getBuildingInfo(item[1]).getButton(), WidgetTypes.WIDGET_HELP_BUILDING, item[1], -1, 1<<0 )

	def placeRoutes(self):
		screen = CyGInterfaceScreen("WBTeamScreen", CvScreenEnums.WB_TEAM)
		iWidth = screen.getXResolution() *3/8 - 20
		iX = screen.getXResolution()/4
		iY = screen.getYResolution()/2 + 30 + DomainTypes.NUM_DOMAIN_TYPES * 24 + 2
		sText = CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_ROUTES",())
		screen.setLabel("RouteHeader", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_MODIFY", (sText,)) + "</font>", 1<<2, iX + iWidth/2, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		iY += 30
		iHeight = (screen.getYResolution() - 40 - iY) /24 * 24 + 2
		screen.addTableControlGFC("WBTeamRoutes", 2, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBTeamRoutes", 0, "", iWidth *2/3)
		screen.setTableColumnHeader("WBTeamRoutes", 1, "", iWidth /3)

		for item in self.lRoutes:
			iRow = screen.appendTableRow("WBTeamRoutes")
			Info = GC.getRouteInfo(item[1])
			screen.setTableText("WBTeamRoutes", 0, iRow, "<font=3>" + item[0] + "</font>", Info.getButton(), WidgetTypes.WIDGET_PYTHON, 6788, item[1], 1<<0)
			iRouteChange  = self.pTeam.getRouteChange(item[1])
			if iRouteChange  != 0:
				sText = u"%+d%s %s" %(iRouteChange , CyTranslator().getText("[ICON_MOVES]", ()), CyTranslator().getText("TXT_KEY_ESPIONAGE_SCREEN_COST", ()))
				screen.setTableInt("WBTeamRoutes", 1, iRow, "<font=3>" + sText + "</font>", "", WidgetTypes.WIDGET_PYTHON, 6788, item[1], 1<<1)

	def placeDomains(self):
		screen = CyGInterfaceScreen("WBTeamScreen", CvScreenEnums.WB_TEAM)
		iWidth = screen.getXResolution() *3/8 - 20
		iX = screen.getXResolution()/4
		iY = screen.getYResolution()/2
		sText = CyTranslator().getText("TXT_KEY_DOMAIN",())
		screen.setLabel("DomainHeader", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_MODIFY", (sText,)) + "</font>", 1<<2, iX + iWidth/2, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		iY += 30
		screen.addTableControlGFC("WBTeamDomainMoves", 2, iX, iY, iWidth, DomainTypes.NUM_DOMAIN_TYPES * 24 + 2, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		screen.setTableColumnHeader("WBTeamDomainMoves", 0, "", iWidth/2)
		screen.setTableColumnHeader("WBTeamDomainMoves", 1, "", iWidth/2)
		for i in xrange(DomainTypes.NUM_DOMAIN_TYPES):
			Info = GC.getDomainInfo(i)
			screen.appendTableRow("WBTeamDomainMoves")
			screen.setTableText("WBTeamDomainMoves", 0, i, "<font=3>" + Info.getDescription() + "</font>", "", WidgetTypes.WIDGET_PYTHON, 1030, i, 1<<0 )
			iDomainChange = self.pTeam.getExtraMoves(i)
			if iDomainChange != 0:
				sText = u"%+d%s" %(iDomainChange, CyTranslator().getText("[ICON_MOVES]", ()))
				screen.setTableInt("WBTeamDomainMoves", 1, i, "<font=3>" + sText + "</font>", "", WidgetTypes.WIDGET_PYTHON, 1030, i, 1<<1 )

	def placeImprovements(self):
		screen = CyGInterfaceScreen("WBTeamScreen", CvScreenEnums.WB_TEAM)
		iWidth = screen.getXResolution() *3/8 - 20
		iX = screen.getXResolution() *5/8
		iY = self.iTable_Y - 30
		sText = CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_IMPROVEMENT",())
		screen.setLabel("ImprovementHeader", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_WB_MODIFY", (sText,)) + "</font>", 1<<0, iX, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.addDropDownBoxGFC("YieldType", iX + iWidth - 150, iY, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for i in xrange(YieldTypes.NUM_YIELD_TYPES):
			screen.addPullDownString("YieldType", GC.getYieldInfo(i).getDescription(), i, i, self.iSelectedYield == i)

		iY += 30
		iHeight = (screen.getYResolution() - 40 - iY) /24 * 24 + 2
		screen.addTableControlGFC("WBTeamYield", 2, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		screen.setTableColumnHeader("WBTeamYield", 0, "", iWidth/2)
		screen.setTableColumnHeader("WBTeamYield", 1, "", iWidth/2)

		for item in self.lImprovements:
			Info = GC.getImprovementInfo(item[1])
			iRow = screen.appendTableRow("WBTeamYield")
			screen.setTableText("WBTeamYield", 0, iRow, "<font=3>" + item[0] + "</font>", Info.getButton(), WidgetTypes.WIDGET_PYTHON, 7877, item[1], 1<<0 )
			sText = ""
			for j in xrange(YieldTypes.NUM_YIELD_TYPES):
				iYieldChange = self.pTeam.getImprovementYieldChange(item[1], j)
				if iYieldChange != 0:
					sText += u"%d%c" %(iYieldChange, GC.getYieldInfo(j).getChar())
			screen.setTableInt("WBTeamYield", 1, iRow, "<font=3>" + sText + "</font>", "", WidgetTypes.WIDGET_PYTHON, 7877, item[1], 1<<0)

	def placeMembers(self):
		screen = CyGInterfaceScreen("WBTeamScreen", CvScreenEnums.WB_TEAM)
		iWidth = screen.getXResolution() *3/8 - 20
		iHeight = (screen.getYResolution()/2 - self.iTable_Y) /24 * 24 + 2
		iX = screen.getXResolution()/4
		screen.setLabel("MemberHeader", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_MEMBERS_TITLE",()) + "</font>", 1<<2, iX + iWidth/2, self.iTable_Y - 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.addTableControlGFC("WBTeamMembers", 2, iX, self.iTable_Y, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		screen.setTableColumnHeader("WBTeamMembers", 0, "", iWidth/2)
		screen.setTableColumnHeader("WBTeamMembers", 1, "", iWidth/2)
		lMembers = []
		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(iPlayerX)
			iTeamX = pPlayerX.getTeam()
			pTeamX = GC.getTeam(iTeamX)
			sText = CyTranslator().getText("[ICON_ANGRYPOP]", ())
			fType = 1
			if iTeamX == self.iTeam:
				if pTeamX.getLeaderID() == iPlayerX:
					sText = CyTranslator().getText("[ICON_STAR]", ())
					fType = 0
			elif pTeamX.isVassal(self.iTeam):
				sText = CyTranslator().getText("[ICON_SILVER_STAR]", ())
				fType = 2
			else:
				continue
			if not pPlayerX.isAlive():
				sText += "*"
				fType += 0.5
			sText += pPlayerX.getName()
			if pPlayerX.isTurnActive():
				sText = "[" + sText + "]"
			sColor = u"<color=%d,%d,%d,%d>" %(pPlayerX.getPlayerTextColorR(), pPlayerX.getPlayerTextColorG(), pPlayerX.getPlayerTextColorB(), pPlayerX.getPlayerTextColorA())
			sText = "<font=3>" + sColor + sText + "</font></color>"
			sCiv = "<font=3>" + sColor + pPlayerX.getCivilizationShortDescription(0) + "</font></color>"
			lMembers.append([fType, sText, iPlayerX, pPlayerX.getCivilizationType(), pPlayerX.getLeaderType(), sCiv])
		lMembers.sort()
		for item in lMembers:
			iRow = screen.appendTableRow("WBTeamMembers")
			screen.setTableText("WBTeamMembers", 0, iRow, item[5], GC.getCivilizationInfo(item[3]).getButton(), WidgetTypes.WIDGET_PYTHON, 7872, item[2] * 10000 + item[3], 1<<0)
			screen.setTableText("WBTeamMembers", 1, iRow, item[1], GC.getLeaderHeadInfo(item[4]).getButton(), WidgetTypes.WIDGET_PYTHON, 7876, item[2] * 10000 + item[4], 1<<0)

	def placeAbilities(self):
		screen = CyGInterfaceScreen("WBTeamScreen", CvScreenEnums.WB_TEAM)
		iWidth = screen.getXResolution() /4 - 40
		iX = 20
		iY = self.iTable_Y + 150
		sText = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=4b>" + CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()) + " (+/-)</color></font>"
		screen.setText("AbilitiesAll", "Background", sText, 1<<1, iX + iWidth, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.addTableControlGFC("WBAbilities", 1, iX, iY, iWidth, 13 * 24 + 2, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader( "WBAbilities", 0, "", iWidth)

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isMapCentering():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_WB_MAP_CENTERING",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_MAPCENTER").getPath(), self.lAbilities[0][0], self.lAbilities[0][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isMapTrading():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_WB_MAP_TRADING",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_MAPTRADING").getPath(), self.lAbilities[1][0], self.lAbilities[1][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isTechTrading():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_WB_TECH_TRADING",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_TECHTRADING").getPath(), self.lAbilities[2][0], self.lAbilities[2][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isGoldTrading():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_WB_GOLD_TRADING",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_GOLDTRADING").getPath(), self.lAbilities[3][0], self.lAbilities[3][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isOpenBordersTrading():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_MISC_OPEN_BORDERS",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_OPENBORDERS").getPath(), self.lAbilities[4][0], self.lAbilities[4][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isDefensivePactTrading():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_MISC_DEFENSIVE_PACT",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_DEFENSIVEPACT").getPath(), self.lAbilities[5][0], self.lAbilities[5][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isPermanentAllianceTrading():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_MISC_PERMANENT_ALLIANCE",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_PERMALLIANCE").getPath(), self.lAbilities[6][0], self.lAbilities[6][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isVassalStateTrading():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_WB_VASSAL_TRADING",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_VASSAL").getPath(), self.lAbilities[7][0], self.lAbilities[7][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isBridgeBuilding():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_WB_BRIDGE_BUILDING",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_BRIDGEBUILDING").getPath(), self.lAbilities[8][0], self.lAbilities[8][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isIrrigation():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_CONCEPT_IRRIGATION",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_IRRIGATION").getPath(), self.lAbilities[9][0], self.lAbilities[9][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isIgnoreIrrigation():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_WB_IGNORE_IRRIGATION",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_NOIRRIGATION").getPath(), self.lAbilities[10][0], self.lAbilities[10][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isWaterWork():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_WB_WATER_WORK",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_WATERWORK").getPath(), self.lAbilities[11][0], self.lAbilities[11][1], -1, 1<<0 )

		iRow = screen.appendTableRow("WBAbilities")
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if self.pTeam.isExtraWaterSeeFrom():
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setTableText("WBAbilities", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_WB_EXTRA_WATER_SIGHT",()) + "</font></color>", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_LOS").getPath(), self.lAbilities[12][0], self.lAbilities[12][1], -1, 1<<0 )

	def handleInput (self, inputClass):
		screen = CyGInterfaceScreen("WBTeamScreen", CvScreenEnums.WB_TEAM)

		if inputClass.getFunctionName() == "ChangeBy":
			self.iChange = screen.getPullDownData("ChangeBy", screen.getSelectedPullDownID("ChangeBy"))

		elif inputClass.getFunctionName() == "CurrentPage":
			iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
			if iIndex == 0:
				WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(self.pTeam.getLeaderID())
			elif iIndex == 2:
				WBProjectScreen.WBProjectScreen(self.WB).interfaceScreen(self.iTeam)
			elif iIndex == 3:
				self.WB.goToSubScreen("TechScreen")
			elif iIndex == 4:
				WBPlayerUnits.WBPlayerUnits(self.WB).interfaceScreen(self.pTeam.getLeaderID())
			elif iIndex == 11:
				WBInfoScreen.WBInfoScreen(self.WB).interfaceScreen(self.pTeam.getLeaderID())

		elif inputClass.getFunctionName() == "CurrentTeam":
			iTeamX = screen.getPullDownData("CurrentTeam", screen.getSelectedPullDownID("CurrentTeam"))
			self.interfaceScreen(iTeamX)

		elif inputClass.getFunctionName() == "WBTeamMembers":
			if inputClass.getData1() == 7876 or inputClass.getData1() == 7872:
				iPlayer = inputClass.getData2() /10000
				WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(iPlayer)

		elif inputClass.getFunctionName() == "MergeTeam":
			self.pTeam.addTeam(screen.getPullDownData("MergeTeam", screen.getSelectedPullDownID("MergeTeam")))
			self.interfaceScreen(self.pTeam.getID())

		elif inputClass.getFunctionName().find("NukeInterception") > -1:
			if inputClass.getData1() == 1030:
				self.pTeam.changeNukeInterception(min(self.iChange, 100 - self.pTeam.getNukeInterception()))
			elif inputClass.getData1() == 1031:
				iCount = min(self.iChange, self.pTeam.getNukeInterception())
				self.pTeam.changeNukeInterception(-iCount)
			self.placeStats()

		elif inputClass.getFunctionName().find("EnemyWW") > -1:
			if inputClass.getData1() == 1030:
				self.pTeam.changeEnemyWarWearinessModifier(self.iChange)
			elif inputClass.getData1() == 1031:
				iCount = min(self.iChange, self.pTeam.getEnemyWarWearinessModifier())
				self.pTeam.changeEnemyWarWearinessModifier(-iCount)
			self.placeStats()

		elif inputClass.getFunctionName().find("MasterPower") > -1:
			if inputClass.getData1() == 1030:
				self.pTeam.setMasterPower(self.pTeam.getMasterPower() + self.iChange)
			elif inputClass.getData1() == 1031:
				self.pTeam.setMasterPower(max(0, self.pTeam.getMasterPower() - self.iChange))
			self.placeStats()

		elif inputClass.getFunctionName().find("VassalPower") > -1:
			if inputClass.getData1() == 1030:
				self.pTeam.setVassalPower(self.pTeam.getVassalPower() + self.iChange)
			elif inputClass.getData1() == 1031:
				self.pTeam.setVassalPower(max(0, self.pTeam.getVassalPower() - self.iChange))
			self.placeStats()

		elif inputClass.getFunctionName().find("EspionageEver") > -1:
			if inputClass.getData1() == 1030:
				self.pTeam.changeEspionagePointsEver(self.iChange)
			elif inputClass.getData1() == 1031:
				iCount = min(self.iChange, self.pTeam.getEspionagePointsEver())
				self.pTeam.changeEspionagePointsEver(-iCount)
			self.placeStats()

		elif inputClass.getFunctionName() == "WBTeamRoutes":
			self.editRoute(inputClass.getData2())
			self.placeRoutes()

		elif inputClass.getFunctionName() == "WBTeamDomainMoves":
			self.editDomain(inputClass.getData2())
			self.placeDomains()

		elif inputClass.getFunctionName() == "ChangeType":
			self.bRemove = not self.bRemove

		elif inputClass.getFunctionName() == "YieldType":
			self.iSelectedYield = screen.getPullDownData("YieldType", screen.getSelectedPullDownID("YieldType"))

		elif inputClass.getFunctionName() == "WBTeamYield":
			self.modifyImprovement(inputClass.getData2())
			self.placeImprovements()

		elif inputClass.getFunctionName() == "WBTeamVotes":
			iVote = GC.getBuildingInfo(inputClass.getData2()).getVoteSourceType()
			if self.pTeam.isForceTeamVoteEligible(iVote):
				self.pTeam.changeForceTeamVoteEligibilityCount(iVote, - self.pTeam.getForceTeamVoteEligibilityCount(iVote))
			else:
				self.pTeam.changeForceTeamVoteEligibilityCount(iVote, 1)
			self.placeVotes()

		elif inputClass.getFunctionName() == "AbilitiesAll":
			for i in xrange(13):
				self.doTeamAbilities(i, not self.bRemove)
			self.placeAbilities()

		elif inputClass.getFunctionName() == "WBAbilities":
			iButton = inputClass.getButtonType()
			if iButton == WidgetTypes.WIDGET_HELP_MAP_CENTER:
				self.doTeamAbilities(0, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_MAP_TRADE:
				self.doTeamAbilities(1, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_TECH_TRADE:
				self.doTeamAbilities(2, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_GOLD_TRADE:
				self.doTeamAbilities(3, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_OPEN_BORDERS:
				self.doTeamAbilities(4, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_DEFENSIVE_PACT:
				self.doTeamAbilities(5, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_PERMANENT_ALLIANCE:
				self.doTeamAbilities(6, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_VASSAL_STATE:
				self.doTeamAbilities(7, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_BUILD_BRIDGE:
				self.doTeamAbilities(8, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_IRRIGATION:
				self.doTeamAbilities(9, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_IGNORE_IRRIGATION:
				self.doTeamAbilities(10, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_WATER_WORK:
				self.doTeamAbilities(11, -1)
			elif iButton == WidgetTypes.WIDGET_HELP_LOS_BONUS:
				self.doTeamAbilities(12, -1)
			else:
				self.doTeamAbilities(inputClass.getData1(), -1)
			self.placeAbilities()
		return 1

	def editDomain(self, item):
		iCount = self.iChange
		if self.bRemove:
			iCount = -iCount
		self.pTeam.changeExtraMoves(item, iCount)

	def editRoute(self, item):
		iCount = self.iChange
		if self.bRemove:
			iCount = -iCount
		self.pTeam.changeRouteChange(item, iCount)

	def modifyImprovement(self, item):
		iCount = self.iChange
		if self.bRemove:
			iCount = -iCount
		self.pTeam.changeImprovementYieldChange(item, self.iSelectedYield, iCount)

	def doTeamAbilities(self, i, iType):
		if i == 0:
			if iType == -1:
				self.pTeam.setMapCentering(not self.pTeam.isMapCentering())
			else:
				self.pTeam.setMapCentering(iType)
		elif i == 1:
			if self.pTeam.isMapTrading():
				if iType !=1:
					self.pTeam.changeMapTradingCount( - self.pTeam.getMapTradingCount())
			elif iType != 0:
				self.pTeam.changeMapTradingCount(1)
		elif i == 2:
			if self.pTeam.isTechTrading():
				if iType !=1:
					self.pTeam.changeTechTradingCount( - self.pTeam.getTechTradingCount())
			elif iType != 0:
				self.pTeam.changeTechTradingCount(1)
		elif i == 3:
			if self.pTeam.isGoldTrading():
				if iType !=1:
					self.pTeam.changeGoldTradingCount( - self.pTeam.getGoldTradingCount())
			elif iType != 0:
				self.pTeam.changeGoldTradingCount(1)
		elif i == 4:
			if self.pTeam.isOpenBordersTrading():
				if iType !=1:
					self.pTeam.changeOpenBordersTradingCount( - self.pTeam.getOpenBordersTradingCount())
			elif iType != 0:
				self.pTeam.changeOpenBordersTradingCount(1)
		elif i == 5:
			if self.pTeam.isDefensivePactTrading():
				if iType !=1:
					self.pTeam.changeDefensivePactTradingCount( - self.pTeam.getDefensivePactTradingCount())
			elif iType != 0:
				self.pTeam.changeDefensivePactTradingCount(1)
		elif i == 6:
			if self.pTeam.isPermanentAllianceTrading():
				if iType !=1:
					self.pTeam.changePermanentAllianceTradingCount( - self.pTeam.getPermanentAllianceTradingCount())
			elif iType != 0:
				self.pTeam.changePermanentAllianceTradingCount(1)
		elif i == 7:
			if self.pTeam.isVassalStateTrading():
				if iType !=1:
					self.pTeam.changeVassalTradingCount( - self.pTeam.getVassalTradingCount())
			elif iType != 0:
				self.pTeam.changeVassalTradingCount(1)
		elif i == 8:
			if self.pTeam.isBridgeBuilding():
				if iType !=1:
					self.pTeam.changeBridgeBuildingCount( - self.pTeam.getBridgeBuildingCount())
			elif iType != 0:
				self.pTeam.changeBridgeBuildingCount(1)
		elif i == 9:
			if self.pTeam.isIrrigation():
				if iType !=1:
					self.pTeam.changeIrrigationCount( - self.pTeam.getIrrigationCount())
			elif iType != 0:
				self.pTeam.changeIrrigationCount(1)
		elif i == 10:
			if self.pTeam.isIgnoreIrrigation():
				if iType !=1:
					self.pTeam.changeIgnoreIrrigationCount( - self.pTeam.getIgnoreIrrigationCount())
			elif iType != 0:
				self.pTeam.changeIgnoreIrrigationCount(1)
		elif i == 11:
			if self.pTeam.isWaterWork():
				if iType !=1:
					self.pTeam.changeWaterWorkCount( - self.pTeam.getWaterWorkCount())
			elif iType != 0:
				self.pTeam.changeWaterWorkCount(1)
		elif i == 12:
			if self.pTeam.isExtraWaterSeeFrom():
				if iType !=1:
					self.pTeam.changeExtraWaterSeeFromCount( - self.pTeam.getExtraWaterSeeFromCount())
			elif iType != 0:
				self.pTeam.changeExtraWaterSeeFromCount(1)
	def update(self, fDelta):
		return 1