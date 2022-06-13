from CvPythonExtensions import *

import WBPromotionScreen
import WBPlayerScreen
import WBTeamScreen
import WBPlotScreen
import WBEventScreen
import WBPlayerUnits
import HandleInputUtil

GC = CyGlobalContext()
TRNSLTR = CyTranslator()

iChange = 1
iCopyType = 0
iOwnerType = 0
iPlotType = 0
iChangeType = 0
iCommandUnitType = 0
iMissionType = 0
iMissionData1 = -1

class WBUnitScreen:

	def __init__(self, WB):
		self.WB = WB
		self.lSelectableMissions = {
			"MISSION_SPREAD":				[GC.getReligionInfo, 7869],
			"MISSION_SPREAD_CORPORATION":	[GC.getCorporationInfo, 6782],
			"MISSION_JOIN":					[GC.getSpecialistInfo, 7879],
			"MISSION_CONSTRUCT":			[GC.getBuildingInfo, 7870],
			"MISSION_BUILD":				[GC.getBuildInfo, 7882]
		}

	def interfaceScreen(self, args = []):
		self.currentUnit = currentUnit = args[0]
		self.currentPlot = currentPlot = currentUnit.plot()
		self.unitType = currentUnit.getUnitType()

		self.xRes = xRes = self.WB.xRes
		self.yRes = yRes = self.WB.yRes

		iWidth = xRes/5 - 20

		self.iPlotX = currentPlot.getX()
		self.iPlotY = currentPlot.getY()

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT

		self.bCargo = True

		screen = self.WB.getScreen()
		screen.setRenderInterfaceOnly(True)
		screen.setForcedRedraw(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		BG = "SubScreenBG"
		screen.addPanel(BG, "", "", True, False, -10, -10, xRes + 20, yRes + 20, PanelStyles.PANEL_STYLE_MAIN)
		self.aWidgetBucket = [BG]

		screen.setText("ExitSubScreen", "", "<font=4>" + TRNSLTR.getText("TXT_WORD_EXIT", ()), 1<<1, xRes - 16, 0, 0, eFontGame, eWidGen, 1, 2)
		self.aWidgetBucket.append("ExitSubScreen")

		ID = "OwnerType"
		screen.addDropDownBoxGFC(ID, 20, 20, iWidth, eWidGen, -1, -1, eFontGame)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, 0 == iOwnerType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_PITBOSS_TEAM", ()), 2, 2, 2 == iOwnerType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_MAIN_MENU_PLAYER", ()), 1, 1, 1 == iOwnerType)
		self.aWidgetBucket.append(ID)

		ID = "CopyType"
		screen.addDropDownBoxGFC(ID, 20, 50, iWidth, eWidGen, -1, -1, eFontGame)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, 0 == iCopyType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_SPACE_SHIP_SCREEN_TYPE_BUTTON", ()), 1, 1, 1 == iCopyType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_UNIT_COMBAT", ()), 2, 2, 2 == iCopyType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_DOMAIN", ()), 3, 3, 3 == iCopyType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_GROUP", ()), 4, 4, 4 == iCopyType)
		self.aWidgetBucket.append(ID)

		ID = "PlotType"
		screen.addDropDownBoxGFC(ID, 20, 80, iWidth, eWidGen, -1, -1, eFontGame)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_SINGLE_PLOT", ()), 0, 0, iPlotType == 0)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_AREA_PLOTS", ()), 1, 1, iPlotType == 1)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_ALL_PLOTS", ()), 2, 2, iPlotType == 2)
		self.aWidgetBucket.append(ID)

		ID = "CurrentPage"
		screen.addDropDownBoxGFC(ID, 20, yRes - 42, iWidth, eWidGen, -1, -1, eFontGame)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_UNIT_DATA", ()), 0, 0, True)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_PROMOTION", ()), 1, 1, False)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_PLAYER_DATA", ()), 2, 2, False)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_TEAM_DATA", ()), 3, 3, False)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_PLOT_DATA", ()), 4, 4, False)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_CONCEPT_EVENTS", ()), 5, 5, False)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + TRNSLTR.getText("TXT_KEY_CONCEPT_CITIES", ()), 6, 6, False)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)
		self.aWidgetBucket.append(ID)

		iX = xRes/5 + 20
		ID = "ChangeType"
		screen.addDropDownBoxGFC(ID, iX, yRes - 42, iWidth, eWidGen, -1, -1, eFontGame)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_XLEVEL", ()), 0, 0, 0 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("INTERFACE_PANE_EXPERIENCE", ()), 1, 1, 1 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_DEMO_SCREEN_STRENGTH", ()), 2, 2, 2 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_DAMAGE", ()), 3, 3, 3 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_MOVES", ()), 4, 4, 4 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_IMMOBILE_TIMER", ()), 5, 5, 5 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_PROMOTION_READY", ()), 6, 6, 6 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_MADE_ATTACK", ()), 7, 7, 7 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_MADE_INTERCEPT", ()), 8, 8, 8 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_DIRECTION", ()), 9, 9, 9 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_UNIT_AI", ()), 10, 10, 10 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_WORD_CARGO", ()), 11, 11, 11 == iChangeType)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_WB_SCRIPT_DATA", ()), 12, 12, 12 == iChangeType)
		self.aWidgetBucket.append(ID)

		iX += iWidth
		sText = TRNSLTR.getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3b>" + TRNSLTR.getText("TXT_KEY_WB_COPY_ALL", (TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()),))
		screen.setText("CopyStats", "", sText, 1<<0, iX, yRes - 42, 0, eFontGame, eWidGen, 1, 2)
		self.aWidgetBucket.append("CopyStats")

		self.placeStats()
		self.aWidgetBucket.append("UnitScreenHeader")
		self.aWidgetBucket.append("UnitScreenHeaderB")
		self.aWidgetBucket.append("UnitLocation")
		self.aWidgetBucket.append("UnitOwner")
		self.aWidgetBucket.append("ChangeBy")
		self.aWidgetBucket.append("CommandUnits")
		self.aWidgetBucket.append("Commands")
		self.aWidgetBucket.append("UnitLevelPlus")
		self.aWidgetBucket.append("UnitLevelMinus")
		self.aWidgetBucket.append("UnitLevelText")
		self.aWidgetBucket.append("UnitExperiencePlus")
		self.aWidgetBucket.append("UnitExperienceMinus")
		self.aWidgetBucket.append("UnitExperienceText")
		self.aWidgetBucket.append("UnitBaseStrPlus")
		self.aWidgetBucket.append("UnitBaseStrMinus")
		self.aWidgetBucket.append("UnitBaseStrText")
		self.aWidgetBucket.append("UnitAirStrText")
		self.aWidgetBucket.append("UnitDamagePlus")
		self.aWidgetBucket.append("UnitDamageMinus")
		self.aWidgetBucket.append("UnitDamageText")
		self.aWidgetBucket.append("UnitMovesLeftPlus")
		self.aWidgetBucket.append("UnitMovesLeftMinus")
		self.aWidgetBucket.append("UnitMovesLeftText")
		self.aWidgetBucket.append("UnitImmobilePlus")
		self.aWidgetBucket.append("UnitImmobileMinus")
		self.aWidgetBucket.append("UnitImmobileText")

		self.aWidgetBucket.append("PushMission")
		self.aWidgetBucket.append("TargetPlot")
		self.aWidgetBucket.append("Missions")
		self.aWidgetBucket.append("MissionInput")

		self.listUnitAI = listUnitAI = []
		lUnitAI = []
		for i in xrange(UnitAITypes.NUM_UNITAI_TYPES):
			sText = GC.getUnitAIInfo(i).getDescription()
			sList = ""
			while len(sText):
				sText = sText[sText.find("_") +1:]
				sText = sText.lower()
				sText = sText.capitalize()
				if sText.find("_") == -1:
					sList += sText
					break
				else:
					sList += sText[:sText.find("_")] + " "
					sText = sText[sText.find("_") +1:]
			listUnitAI.append([sList, i])
		listUnitAI.sort()

		screen.setLabel("UnitDirectionText", "", "<font=3b>" + TRNSLTR.getText("TXT_KEY_WB_DIRECTION", ()), 1<<2, xRes/2,  yRes/2, 0, eFontGame, eWidGen, 1, 2)
		self.aWidgetBucket.append("UnitDirectionText")

		self.placeDirection()
		self.aWidgetBucket.append("WBUnitDirection")
		self.aWidgetBucket.append("UnitAIType")
		self.aWidgetBucket.append("PromotionReadyText")
		self.aWidgetBucket.append("MadeAttackText")
		self.aWidgetBucket.append("MadeInterceptionText")
		self.aWidgetBucket.append("UnitActivity")

		self.sortUnits()
		self.aWidgetBucket.append("WBCurrentUnit")

		self.placeCargo()
		self.aWidgetBucket.append("CargoType")
		self.aWidgetBucket.append("WBCargoUnits")
		self.aWidgetBucket.append("UnitCargoSpaceChange0")
		self.aWidgetBucket.append("UnitCargoSpaceChange1")
		self.aWidgetBucket.append("CargoSpaceHeader")

		iWidth = xRes/5 - 40
		iX = xRes/2 - iWidth/2
		iY = self.yRes/2 - 125
		sText = TRNSLTR.getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3b>" + TRNSLTR.getText("TXT_KEY_WB_SCRIPT_DATA", ())
		screen.setText("UnitEditScriptData", "", sText, 1<<2, xRes/2, iY, 0, eFontGame, eWidGen, 1, 2)
		screen.addPanel("ScriptPanel", "", "", False, False, iX, iY + 25, iWidth, 90, PanelStyles.PANEL_STYLE_IN)
		screen.addMultilineText("GameScriptDataText", currentUnit.getScriptData(), iX, iY + 25, iWidth, 90, eWidGen, 1, 2, 1<<0)

		self.aWidgetBucket.append("UnitEditScriptData")
		self.aWidgetBucket.append("ScriptPanel")
		self.aWidgetBucket.append("GameScriptDataText")
		self.aWidgetBucket.append("PlotView")


	def placeMissions(self):
		xRes = self.xRes
		yRes = self.yRes
		iX = xRes *3/5
		iWidth = xRes/5 - 20
		global iMissionType
		global iMissionData1

		iY = 110
		screen = self.WB.getScreen()
		screen.setText("PushMission", "", "<font=3b>" + TRNSLTR.getText("[COLOR_SELECTED_TEXT]", ()) + TRNSLTR.getText("TXT_KEY_WB_PUSH_MISSION", ()) + "</color></font>", 1<<2, xRes *7/10, iY - 30, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		sText = (TRNSLTR.getText("[COLOR_SELECTED_TEXT]", ()) + "<font=3>" + TRNSLTR.getText("TXT_KEY_WB_TARGET_PLOT", ()) + ": " + "(%d,%d)" + "</color></font>") % (self.iPlotX, self.iPlotY)
		screen.setText("TargetPlot", "", sText, 1<<2, xRes *7/10, iY, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.addDropDownBoxGFC("Missions", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		UnitInfo = GC.getUnitInfo(self.unitType)
		lMissionData = []
		iFirstMission = -1
		bResetMission = False
		for i in xrange(GC.getNumMissionInfos()):
			MissionInfo = GC.getMissionInfo(i)
			sType = MissionInfo.getType()
			lData1 = []
			iData2 = -1

			if sType in ("MISSION_MOVE_TO_UNIT", "MISSION_SHADOW"):
				continue
			elif sType == "MISSION_BUILD":
				for j in xrange(GC.getNumBuildInfos()):
					if UnitInfo.hasBuild(BuildTypes(j)):
						lData1.append(j)
			elif sType == "MISSION_SPREAD":
				for j in xrange(GC.getNumReligionInfos()):
					if UnitInfo.getReligionSpreads(j) > 0:
						lData1.append(j)
			elif sType == "MISSION_SPREAD_CORPORATION":
				for j in xrange(GC.getNumCorporationInfos()):
					if UnitInfo.getCorporationSpreads(j) > 0:
						lData1.append(j)
			elif sType == "MISSION_JOIN":
				for j in xrange(GC.getNumSpecialistInfos()):
					if UnitInfo.getGreatPeoples(j):
						lData1.append(j)
			elif sType == "MISSION_CONSTRUCT":
				for j in xrange(UnitInfo.getNumBuildings()):
					lData1.append(UnitInfo.getBuildings(j))
			elif sType == "MISSION_GOLDEN_AGE":
				lData1 = [-1]
			else:
				lData1 = [self.iPlotX]
				iData2 = self.iPlotY
				pTargetPlot = CyMap().plot(lData1[0], iData2)
				if pTargetPlot.isNone():
					lData1 = [self.iPlotX]
					iData2 = self.iPlotY
			bCanDoMission = False
			for iData1 in lData1:
				if self.currentUnit.getGroup().canStartMission(i, iData1, iData2, self.currentPlot, True):
					screen.addPullDownString("Missions", MissionInfo.getDescription(), i, i, i == iMissionType)
					bCanDoMission = True
					if iFirstMission == -1:
						iFirstMission = i
					break
			if iMissionType == i:
				lMissionData = lData1
				if not bCanDoMission:
					bResetMission = True
		if bResetMission:
			iMissionType = max(0, iFirstMission)

		iY += 30
		iHeight = (yRes/2 - iY) /24 * 24 + 2
		screen.addTableControlGFC("MissionInput", 1, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("MissionInput", 0, "", iWidth)

		sMissionType = GC.getMissionInfo(iMissionType).getType()
		screen.hide("MissionInput")
		if sMissionType in self.lSelectableMissions:
			screen.show("MissionInput")
			for i in xrange(len(lMissionData)):
				iData1 = lMissionData[i]
				if iMissionData1 not in lMissionData:
					iMissionData1 = iData1
				if self.currentUnit.getGroup().canStartMission(iMissionType, iData1, -1, self.currentPlot, True):
					ItemInfo = self.lSelectableMissions[sMissionType][0](iData1)
					sDescription = ItemInfo.getDescription()
					if sMissionType == "MISSION_BUILD":
						sText = ItemInfo.getType()
						sDescription = ""
						while len(sText):
							sText = sText[sText.find("_") +1:]
							sText = sText.lower()
							sText = sText.capitalize()
							if sText.find("_") == -1:
								sDescription += sText
								break
							else:
								sDescription += sText[:sText.find("_")] + " "
								sText = sText[sText.find("_") +1:]
					iRow = screen.appendTableRow("MissionInput")
					sColor = "<color=255,80,80>"
					if iData1 == iMissionData1:
						sColor = "<color=128,255,28>"
					screen.setTableText("MissionInput", 0, iRow, "<font=3>" + sColor + sDescription + "</font></color>", ItemInfo.getButton(), WidgetTypes.WIDGET_PYTHON, self.lSelectableMissions[sMissionType][1], iData1, 1<<0)

		elif sMissionType == "MISSION_DISCOVER":
			screen.show("MissionInput")
			iTech = self.currentUnit.getDiscoveryTech()
			if iTech > -1:
				iRow = screen.appendTableRow("MissionInput")
				screen.setTableText("MissionInput", 0, iRow, "<font=3>" + GC.getTechInfo(iTech).getDescription(), GC.getTechInfo(iTech).getButton(), WidgetTypes.WIDGET_PYTHON, 7871, iTech, 1<<0)
				iRow = screen.appendTableRow("MissionInput")
				sText = u"%c %d / %d" %(GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar(), GC.getTeam(self.currentUnit.getTeam()).getResearchProgress(iTech), GC.getTeam(self.currentUnit.getTeam()).getResearchCost(iTech))
				screen.setTableText("MissionInput", 0, iRow, "<font=3>" + sText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
				iRow = screen.appendTableRow("MissionInput")
				sText = u"%c <color=128,255,28>%+d" %(GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar(), self.currentUnit.getDiscoverResearch(iTech))
				screen.setTableText("MissionInput", 0, iRow, "<font=3>" + sText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		elif sMissionType == "MISSION_GREAT_WORK":
			screen.show("MissionInput")
			if self.currentPlot.isCity():
				pCity = self.currentPlot.getPlotCity()
				sText = u"%s: %d/%d%c" %(GC.getCultureLevelInfo(pCity.getCultureLevel()).getDescription(), pCity.getCulture(pCity.getOwner()), pCity.getCultureThreshold(), GC.getCommerceInfo(CommerceTypes.COMMERCE_CULTURE).getChar())
				iRow = screen.appendTableRow("MissionInput")
				sText = u"%c %s" %(GC.getCommerceInfo(CommerceTypes.COMMERCE_CULTURE).getChar(), GC.getCultureLevelInfo(pCity.getCultureLevel()).getDescription())
				screen.setTableText("MissionInput", 0, iRow, "<font=3>" + sText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
				iRow = screen.appendTableRow("MissionInput")
				sText = u"%c %d / %d" %(GC.getCommerceInfo(CommerceTypes.COMMERCE_CULTURE).getChar(), pCity.getCulture(pCity.getOwner()), pCity.getCultureThreshold())
				screen.setTableText("MissionInput", 0, iRow, "<font=3>" + sText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
				iRow = screen.appendTableRow("MissionInput")
				sText = u"%c <color=128,255,28>%+d" %(GC.getCommerceInfo(CommerceTypes.COMMERCE_CULTURE).getChar(), self.currentUnit.getGreatWorkCulture(self.currentPlot))
				screen.setTableText("MissionInput", 0, iRow, "<font=3>" + sText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		elif sMissionType == "MISSION_HURRY":
			screen.show("MissionInput")
			if self.currentPlot.isCity():
				pCity = self.currentPlot.getPlotCity()
				iRow = screen.appendTableRow("MissionInput")
				if pCity.isProductionBuilding():
					iBuilding = pCity.getProductionBuilding()
					screen.setTableText("MissionInput", 0, iRow, "<font=3>" + GC.getBuildingInfo(iBuilding).getDescription(), GC.getBuildingInfo(iBuilding).getButton(), WidgetTypes.WIDGET_PYTHON, 7870, iBuilding, 1<<0)
				elif pCity.isProductionProject():
					iProject = pCity.getProductionProject()
					screen.setTableText("MissionInput", 0, iRow, "<font=3>" + GC.getProjectInfo(iProject).getDescription(), GC.getProjectInfo(iProject).getButton(), WidgetTypes.WIDGET_PYTHON, 6785, iProject, 1<<0)
				elif pCity.isProductionUnit():
					iUnit = pCity.getProductionUnit()
					screen.setTableText("MissionInput", 0, iRow, "<font=3>" + GC.getUnitInfo(iUnit).getDescription(), GC.getUnitInfo(iUnit).getButton(), WidgetTypes.WIDGET_PYTHON, 8202, iUnit, 1<<0)
				iRow = screen.appendTableRow("MissionInput")
				sText = u"%c %d / %d" %(GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar(), pCity.getProduction(), pCity.getProductionNeeded())
				screen.setTableText("MissionInput", 0, iRow, "<font=3>" + sText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
				iRow = screen.appendTableRow("MissionInput")
				sText = u"%c <color=128,255,28>%+d" %(GC.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar(), self.currentUnit.getHurryProduction(self.currentPlot))
				screen.setTableText("MissionInput", 0, iRow, "<font=3>" + sText, "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

	def placeDirection(self):
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT
		xRes = self.xRes
		yRes = self.yRes
		screen = self.WB.getScreen()
		iY = yRes/2 + 30

		iHeight = 3*24 + 2
		TBL = "WBUnitDirection"
		screen.addTableControlGFC(TBL, 3, xRes/2 - iHeight/2, iY, iHeight, iHeight, False, True, 24, 24, TableStyles.TABLE_STYLE_EMPTY)
		for i in xrange(3):
			screen.setTableColumnHeader(TBL, i, "", iHeight/3)
			screen.appendTableRow(TBL)
		screen.setTableText(TBL, 1,0 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 0, 1<<2)
		screen.setTableText(TBL, 2,0 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 1, 1<<2)
		screen.setTableText(TBL, 2,1 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 2, 1<<2)
		screen.setTableText(TBL, 2,2 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 3, 1<<2)
		screen.setTableText(TBL, 1,2 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 4, 1<<2)
		screen.setTableText(TBL, 0,2 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 5, 1<<2)
		screen.setTableText(TBL, 0,1 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 6, 1<<2)
		screen.setTableText(TBL, 0,0 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, 7, 1<<2)
		screen.setTableText(TBL, 1,1 , " ", "", WidgetTypes.WIDGET_PYTHON, 1030, -1, 1<<2)

		unit = self.currentUnit

		if unit.getFacingDirection() == DirectionTypes.DIRECTION_NORTH:
			screen.setTableText(TBL, 1,0 , "", unit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 0, 1<<2)
		elif unit.getFacingDirection() == DirectionTypes.DIRECTION_NORTHEAST:
			screen.setTableText(TBL, 2,0 , "", unit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 1, 1<<2)
		elif unit.getFacingDirection() == DirectionTypes.DIRECTION_EAST:
			screen.setTableText(TBL, 2,1 , "", unit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 2, 1<<2)
		elif unit.getFacingDirection() == DirectionTypes.DIRECTION_SOUTHEAST:
			screen.setTableText(TBL, 2,2 , "", unit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 3, 1<<2)
		elif unit.getFacingDirection() == DirectionTypes.DIRECTION_SOUTH:
			screen.setTableText(TBL, 1,2 , "", unit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 4, 1<<2)
		elif unit.getFacingDirection() == DirectionTypes.DIRECTION_SOUTHWEST:
			screen.setTableText(TBL, 0,2 , "", unit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 5, 1<<2)
		elif unit.getFacingDirection() == DirectionTypes.DIRECTION_WEST:
			screen.setTableText(TBL, 0,1 , "", unit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 6, 1<<2)
		elif unit.getFacingDirection() == DirectionTypes.DIRECTION_NORTHWEST:
			screen.setTableText(TBL, 0,0 , "", unit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, 7, 1<<2)
		elif unit.getFacingDirection() == DirectionTypes.NO_DIRECTION:
			screen.setTableText(TBL, 1,1 , "", unit.getButton(), WidgetTypes.WIDGET_PYTHON, 1030, -1, 1<<2)

		iWidth = xRes/5 - 40
		iY += iHeight
		screen.addDropDownBoxGFC("UnitAIType", xRes/2 - iWidth/2, iY, iWidth, eWidGen, 1, 2, eFontGame)
		for item in self.listUnitAI:
			screen.addPullDownString("UnitAIType", item[0], item[1], item[1], item[1] == unit.getUnitAIType())

		iY += 30
		if unit.isPromotionReady():
			sColor = "<color=128,255,28>"
		else: sColor = "<color=255,80,80>"
		sText = TRNSLTR.getText("TXT_KEY_WB_PROMOTION_READY", ())
		screen.setText("PromotionReadyText", "", "<font=3>" + sColor + sText, 1<<2, xRes/2, iY + 1, 0, eFontGame, eWidGen, 1, 2)

		iY += 30
		if unit.isMadeAttack():
			sColor = "<color=128,255,28>"
		else: sColor = "<color=255,80,80>"
		sText = TRNSLTR.getText("TXT_KEY_WB_MADE_ATTACK", ())
		screen.setText("MadeAttackText", "", "<font=3>" + sColor + sText + "</color></font>", 1<<2, xRes/2, iY + 1, 0, eFontGame, eWidGen, 1, 2)

		iY += 30
		if unit.isMadeInterception():
			sColor = "<color=128,255,28>"
		else: sColor = "<color=255,80,80>"
		sText = TRNSLTR.getText("TXT_KEY_WB_MADE_INTERCEPT", ())
		screen.setText("MadeInterceptionText", "", "<font=3>" + sColor + sText + "</color></font>", 1<<2, xRes/2, iY + 1, 0, eFontGame, eWidGen, 1, 2)

		iActivity = unit.getGroup().getActivityType()
		if iActivity > -1:
			import WorldBuilder
			if iActivity < len(WorldBuilder.Activities):
				iY += 30
				screen.setLabel("UnitActivity", "", "<font=3>" + WorldBuilder.Activities[iActivity], 1<<2, xRes/2, iY + 1, 0, eFontGame, eWidGen, 1, 2)


	def placeStats(self):
		xRes = self.xRes
		screen = self.WB.getScreen()
		sText = TRNSLTR.getText("[COLOR_SELECTED_TEXT]", ()) + "<font=4b>" + self.currentUnit.getName()
		screen.setText("UnitScreenHeader", "", sText, 1<<2, xRes/2, 20, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_UNIT_NAME, -1, -1)
		sText = "<font=3b>%s ID: %d, %s ID: %d" %(TRNSLTR.getText("TXT_WORD_UNIT", ()), self.currentUnit.getID(), TRNSLTR.getText("TXT_KEY_WB_GROUP", ()), self.currentUnit.getGroupID())
		screen.setLabel("UnitScreenHeaderB", "", sText, 1<<2, xRes/2, 50, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		sText = "<font=3b>%s, X: %d, Y: %d" %(TRNSLTR.getText("TXT_KEY_WB_LATITUDE",(self.currentPlot.getLatitude(),)), self.iPlotX, self.iPlotY)
		screen.setLabel("UnitLocation", "", sText, 1<<2, xRes/2, 70, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iX = xRes/5 + 20
		iY = 110
		iWidth = xRes/5 - 20

		screen.addDropDownBoxGFC("UnitOwner", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(iPlayerX)
			if pPlayerX.isEverAlive():
				sName = pPlayerX.getName()
				if not pPlayerX.isAlive():
					sName = "*" + sName
				if pPlayerX.isTurnActive():
					sText = "[" + sText + "]"
				screen.addPullDownString("UnitOwner", sName, iPlayerX, iPlayerX, iPlayerX == self.currentUnit.getOwner())

		iY += 30
		screen.addDropDownBoxGFC("ChangeBy", iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		i = 1
		while i < 100001:
			screen.addPullDownString("ChangeBy", "(+/-) " + str(i), i, i, iChange == i)
			if str(i)[0] == "1":
				i *= 5
			else:
				i *= 2

		iY += 30
		DDB = "CommandUnits"
		screen.addDropDownBoxGFC(DDB, iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_WORD_UNIT", ()), 0, 0, iCommandUnitType == 0)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_SPACE_SHIP_SCREEN_TYPE_BUTTON", ()), 1, 1, iCommandUnitType == 1)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_UNIT_COMBAT", ()), 2, 2, iCommandUnitType == 2)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_DOMAIN", ()), 3, 3, 3 == iCommandUnitType == 3)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_WB_GROUP", ()), 4, 4, iCommandUnitType == 4)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_WB_CITY_ALL", ()), 5, 5, iCommandUnitType == 5)

		iY += 30
		DDB = "Commands"
		screen.addDropDownBoxGFC(DDB, iX, iY, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_WB_COMMANDS", ()), 0, 0, True)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_OPTIONS_RESET", ()), 1, 1, False)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_MISSION_MOVE_TO", ()), 2, 2, False)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_MAP_SCRIPT_COPY", ()), 3, 3, False)
		screen.addPullDownString(DDB, TRNSLTR.getText("TXT_KEY_ESPIONAGE_DESTROY_UNIT", ()), 4, 4, False)

		iY += 30
		screen.setButtonGFC("UnitLevelPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitLevelMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = TRNSLTR.getText("TXT_KEY_WB_XLEVEL", ()) + ": " + str(self.currentUnit.getLevel())
		screen.setLabel("UnitLevelText", "", "<font=3>" + sText, 1<<0, iX + 55, iY + 1, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("UnitExperiencePlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitExperienceMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = TRNSLTR.getText("INTERFACE_PANE_EXPERIENCE", ()) + ": " + str(self.currentUnit.getExperience())
		screen.setLabel("UnitExperienceText", "", "<font=3>" + sText, 1<<0, iX + 55, iY + 1, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("UnitBaseStrPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitBaseStrMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = TRNSLTR.getText("INTERFACE_PANE_STRENGTH", ()) + " " + str(self.currentUnit.baseCombatStr()) + TRNSLTR.getText("[ICON_STRENGTH]", ())
		screen.setLabel("UnitBaseStrText", "", "<font=3>" + sText, 1<<0, iX + 55, iY + 1, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		sText = TRNSLTR.getText("INTERFACE_PANE_AIR_STRENGTH", ()) + " " + str(self.currentUnit.airBaseCombatStr())
		screen.setLabel("UnitAirStrText", "", "<font=3>" + sText, 1<<0, iX + 55, iY + 1, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("UnitDamagePlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitDamageMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = TRNSLTR.getText("TXT_KEY_WB_DAMAGE", ()) + ": " + str(self.currentUnit.getDamage())
		screen.setLabel("UnitDamageText", "", "<font=3>" + sText, 1<<0, iX + 55, iY + 1, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("UnitMovesLeftPlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitMovesLeftMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = u"%s: %.2f" %(TRNSLTR.getText("TXT_KEY_WB_MOVES", ()), float(self.currentUnit.movesLeft()) / GC.getDefineINT("MOVE_DENOMINATOR")) + TRNSLTR.getText("[ICON_MOVES]", ())
		screen.setLabel("UnitMovesLeftText", "", "<font=3>" + sText, 1<<0, iX + 55, iY + 1, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.setButtonGFC("UnitImmobilePlus", "", "", iX, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1030, -1, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
		screen.setButtonGFC("UnitImmobileMinus", "", "", iX + 25, iY, 24, 24, WidgetTypes.WIDGET_PYTHON, 1031, -1, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
		sText = TRNSLTR.getText("TXT_KEY_WB_IMMOBILE_TIMER", ()) + ": " + str(self.currentUnit.getImmobileTimer())
		screen.setLabel("UnitImmobileText", "", "<font=3>" + sText, 1<<0, iX + 55, iY + 1, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		self.placeMissions()


	def sortUnits(self):
		self.lUnits = lUnits = []
		iX = self.iPlotX
		iY = self.iPlotY
		iOwner = self.currentUnit.getOwner()
		iTeam = self.currentUnit.getTeam()

		for iPlayerX in xrange(GC.getMAX_PLAYERS()):

			if iOwnerType == 1 and iPlayerX != iOwner:
				continue
			pPlayerX = GC.getPlayer(iPlayerX)
			if iOwnerType == 2 and pPlayerX.getTeam() != iTeam or not pPlayerX.isAlive():
				continue

			for unitX in pPlayerX.units():

				if iPlotType == 0:
					if unitX.getX() != iX or unitX.getY() != iY:
						continue
				elif iPlotType == 1:
					if unitX.plot().getArea() != self.currentPlot.getArea():
						continue

				if iCopyType == 1:
					if unitX.getUnitType() != self.unitType:
						continue
				elif iCopyType == 2:
					if unitX.getUnitCombatType() != self.currentUnit.getUnitCombatType():
						continue
				elif iCopyType == 3:
					if unitX.getDomainType() != self.currentUnit.getDomainType():
						continue
				elif iCopyType == 4:
					if unitX.getGroupID() != self.currentUnit.getGroupID():
						continue

				lUnits.append([unitX.getOwner(), unitX.getID()])

		lUnits.sort()

		iWidth = self.xRes/5 - 20
		iHeight = self.yRes - 150

		ID = "WBCurrentUnit"
		screen = self.WB.getScreen()
		screen.addTableControlGFC(ID, 3, 20, 110, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader(ID, 0, "", 24)
		screen.setTableColumnHeader(ID, 1, "", 24)
		screen.setTableColumnHeader(ID, 2, "", iWidth - 48)

		for i in lUnits:
			pPlayerX = GC.getPlayer(i[0])
			unitX = pPlayerX.getUnit(i[1])
			if unitX is None: continue
			iRow = screen.appendTableRow(ID)
			sText = unitX.getName()
			if len(unitX.getNameNoDesc()):
				sText = unitX.getNameNoDesc()
			sColor = "<color=255,80,80>"
			if unitX.getOwner() == self.currentUnit.getOwner():
				if unitX.getID() == self.currentUnit.getID():
					sColor = "<color=128,255,28>"
				elif unitX.getGroupID() == self.currentUnit.getGroupID():
					sColor = TRNSLTR.getText("[COLOR_YELLOW]", ())
			screen.setTableText(ID, 2, iRow, "<font=3>" + sColor + sText + "</font></color>", unitX.getButton(), WidgetTypes.WIDGET_PYTHON, 8300 + i[0], i[1], 1<<0)
			iLeader = pPlayerX.getLeaderType()
			iCiv = unitX.getCivilizationType()
			screen.setTableText(ID, 0, iRow, "", GC.getCivilizationInfo(iCiv).getButton(), WidgetTypes.WIDGET_PYTHON, 7872, iCiv, 1<<0 )
			screen.setTableText(ID, 1, iRow, "", GC.getLeaderHeadInfo(iLeader).getButton(), WidgetTypes.WIDGET_PYTHON, 7876, iLeader, 1<<0 )


	def placeCargo(self):
		eWidGen		= WidgetTypes.WIDGET_GENERAL
		ePanelSTD	= PanelStyles.PANEL_STYLE_STANDARD
		eFontGame	= FontTypes.GAME_FONT
		ePanelBlack	= PanelStyles.PANEL_STYLE_MAIN_BLACK25

		yRes = self.yRes
		screen = self.WB.getScreen()
		iX = self.xRes * 3/5
		iY = yRes / 2 + 24
		w = self.xRes / 5 - 20
		ID = "CargoType"
		screen.addDropDownBoxGFC(ID, iX, iY - 24, w, eWidGen, 1, 2, eFontGame)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_KEY_UNIT_TRANSPORT", ()), 0, 0, not self.bCargo)
		screen.addPullDownString(ID, TRNSLTR.getText("TXT_WORD_CARGO", ()), 1, 1, self.bCargo)

		dy = 24

		ScPnl = "WBCargoUnits"
		screen.addScrollPanel(ScPnl, "", iX - 6, iY, w + 12, yRes - iY - 64, ePanelSTD)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")

		w -= 12
		h1 = dy + 6
		h2 = dy + 2
		iRow = 0
		y = -5
		entry = "WID|WBCargoUnits"
		BTN = "WID|WBCargoUnits|BTN"

		if self.bCargo:
			iSpace = self.currentUnit.cargoSpace()
			screen.setButtonGFC("UnitCargoSpaceChange0", "", "", iX + 25, yRes - 42, dy, dy, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_CITY_MINUS)
			screen.setButtonGFC("UnitCargoSpaceChange1", "", "", iX, yRes - 42, dy, dy, eWidGen, 1, 2, ButtonStyles.BUTTON_STYLE_CITY_PLUS)
			sText = TRNSLTR.getText("TXT_WORD_CARGO", ()) + " (" + str(self.currentUnit.getCargo()) + "/" + str(iSpace) + ")"
			screen.setLabel("CargoSpaceHeader", "", "<font=3b>" + sText, 1<<0, iX + 50, yRes - 41, 0, eFontGame, eWidGen, 1, 2)

			if iSpace > 0:
				iCargoDomain = self.currentUnit.domainCargo()
				iSpecialCargo = self.currentUnit.specialCargo()
				iUnitID = self.currentUnit.getID()
				iOwner = self.currentUnit.getOwner()

				for unitX in self.currentPlot.units():

					if (unitX.getOwner() != iOwner
					or iCargoDomain > -1 and unitX.getDomainType() != iCargoDomain
					or iSpecialCargo > -1 and unitX.getSpecialUnitType() != iSpecialCargo
					): continue

					iOtherID = unitX.getID()
					if iOtherID == iUnitID:
						continue

					if unitX.getNameNoDesc():
						sText = unitX.getNameNoDesc()
					else:
						sText = unitX.getName()

					sColor = "<color=255,80,80>"
					if unitX.isCargo():
						sColor = TRNSLTR.getText("[COLOR_YELLOW]", ())

					transportUnit = unitX.getTransportUnit()
					if transportUnit and transportUnit.getID() == iUnitID:
						sColor = "<color=128,255,28>"

					ROW = ScPnl + str(iRow)
					if iRow % 2:
						screen.attachPanelAt(ScPnl, ROW, "", "", True, False, ePanelBlack, 0, y, w, h2, eWidGen, 1, 1)
						screen.setStyle(ROW, "Panel_Tan15_Style")
					else:
						screen.attachPanelAt(ScPnl, ROW, "", "", True, False, ePanelBlack, 0, y-4, w, h1, eWidGen, 1, 1)

					screen.setImageButtonAt(BTN + str(iOtherID), ROW, unitX.getButton(), 0, 0, dy, dy, eWidGen, 0, 0)
					screen.setTextAt(entry + str(iOtherID), ROW, "<font=3>" + sColor + sText, 1<<0, dy + 4, 0, 0, eFontGame, eWidGen, 0, 0)
					iRow += 1
					y += dy
		else:
			screen.hide("UnitCargoSpaceChange0")
			screen.hide("UnitCargoSpaceChange1")
			screen.hide("CargoSpaceHeader")
			iDomain = self.currentUnit.getDomainType()
			iSpecialUnit = self.currentUnit.getSpecialUnitType()
			iUnitID = self.currentUnit.getID()
			iOwner = self.currentUnit.getOwner()

			for unitX in self.currentPlot.units():

				if (unitX.getOwner() != iOwner
				or unitX.cargoSpace() < 1
				or unitX.domainCargo() > -1 and iDomain != unitX.domainCargo()
				or unitX.specialCargo() > -1 and iSpecialUnit != unitX.specialCargo()
				): continue

				iOtherID = unitX.getID()
				if iOtherID == iUnitID:
					continue

				if unitX.getNameNoDesc():
					sText = unitX.getNameNoDesc()
				else: sText = unitX.getName()

				sText += " (" + str(unitX.getCargo()) + "/" + str(unitX.cargoSpace()) + ")"

				transportUnit = self.currentUnit.getTransportUnit()
				if transportUnit and transportUnit.getID() == iOtherID:
					sColor = "<color=128,255,28>"

				elif unitX.isFull():
					sColor = TRNSLTR.getText("[COLOR_YELLOW]", ())

				else: sColor = "<color=255,80,80>"

				ROW = ScPnl + str(iRow)
				if iRow % 2:
					screen.attachPanelAt(ScPnl, ROW, "", "", True, False, ePanelBlack, 0, y, w, h2, eWidGen, 1, 1)
					screen.setStyle(ROW, "Panel_Tan15_Style")
				else:
					screen.attachPanelAt(ScPnl, ROW, "", "", True, False, ePanelBlack, 0, y-4, w, h1, eWidGen, 1, 1)

				screen.setImageButtonAt(BTN + str(iOtherID), ROW, unitX.getButton(), 0, 0, dy, dy, eWidGen, unitX.getOwner(), iOtherID)
				screen.setTextAt(entry + str(iOtherID), ROW, "<font=3>" + sColor + sText, 1<<0, dy + 4, 0, 0, eFontGame, eWidGen, unitX.getOwner(), iOtherID)
				iRow += 1
				y += dy


	def doAllCommands(self, unitX, iIndex):
		if iIndex == 1:
			Info = GC.getUnitInfo(self.unitType)
			unitX.setBaseCombatStr(Info.getCombat())
			unitX.setDamage(0, -1)
			unitX.setMoves(0)
			unitX.setImmobileTimer(0)
			unitX.setPromotionReady(False)
			unitX.setMadeAttack(False)
			unitX.setMadeInterception(False)
			self.changeDirection(DirectionTypes.DIRECTION_SOUTH, unitX)
			unitX.setUnitAIType(Info.getDefaultUnitAIType())
			unitX.changeCargoSpace(Info.getCargoSpace() - unitX.cargoSpace())
			unitX.setScriptData("")
			return 2
		if iIndex == 2:
			self.WB.iPlayerAddMode = "MoveUnits"
			self.WB.lMoveUnit.append([unitX.getOwner(), unitX.getID()])
			return 0
		if iIndex == 3:
			iRange = iChange
			if iRange > 10: iRange = 10 # sanity control
			for i in xrange(iRange):
				pNewUnit = GC.getPlayer(unitX.getOwner()).initUnit(unitX.getUnitType(), unitX.getX(), unitX.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
				pNewUnit.convert(unitX, False)
				pNewUnit.setBaseCombatStr(unitX.baseCombatStr())
				pNewUnit.changeCargoSpace(unitX.cargoSpace() - pNewUnit.cargoSpace())
				pNewUnit.setImmobileTimer(unitX.getImmobileTimer())
				pNewUnit.setScriptData(unitX.getScriptData())
			return 1
		if iIndex == 4:
			unitX.kill(False, -1)
			if self.currentPlot.getNumUnits() > 0:
				return 1
			return 0
		return 0

	def doMission(self):
		sType = GC.getMissionInfo(iMissionType).getType()

		iData1 = -1
		iData2 = -1
		if sType == "MISSION_MOVE_TO_UNIT": return 2
		elif sType in self.lSelectableMissions:
			iData1 = iMissionData1
		elif sType == "MISSION_GOLDEN_AGE":
			iData1 = -1
		else:
			iData1 = self.iPlotX
			iData2 = self.iPlotY
		pTargetPlot = CyMap().plot(self.iPlotX, self.iPlotY)
		self.currentUnit.getGroup().pushMission(MissionTypes(iMissionType), iData1, iData2, 0, False, True, MissionAITypes.NO_MISSIONAI, pTargetPlot, self.currentUnit)
		self.interfaceScreen([self.currentUnit])

	def changeDirection(self, iNewDirection, unitX):
		if iNewDirection == -1: return
		iOldDirection = unitX.getFacingDirection()
		if iNewDirection == iOldDirection: return
		if iOldDirection > iNewDirection:
			for i in xrange(iOldDirection - iNewDirection):
				unitX.rotateFacingDirectionCounterClockwise()
		else:
			for i in xrange(iNewDirection - iOldDirection):
				unitX.rotateFacingDirectionClockwise()

	def changeOwner(self, iPlayer):
		pNewUnit = GC.getPlayer(iPlayer).initUnit(self.unitType, self.iPlotX, self.iPlotY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
		pNewUnit.convert(self.currentUnit, True)
		pNewUnit.setBaseCombatStr(self.currentUnit.baseCombatStr())
		pNewUnit.changeCargoSpace(self.currentUnit.cargoSpace() - pNewUnit.cargoSpace())
		pNewUnit.setImmobileTimer(self.currentUnit.getImmobileTimer())
		pNewUnit.setScriptData(self.currentUnit.getScriptData())
		self.currentUnit.kill(False, -1)
		self.interfaceScreen([pNewUnit])

	def handleCopyAll(self):
		for i in self.lUnits:
			unitX = GC.getPlayer(i[0]).getUnit(i[1])
			if unitX is None: continue
			if iChangeType == 0:
				unitX.setLevel(self.currentUnit.getLevel())
			elif iChangeType == 1:
				unitX.setExperience(self.currentUnit.getExperience())
			elif iChangeType == 2:
				unitX.setBaseCombatStr(self.currentUnit.baseCombatStr())
			elif iChangeType == 3:
				unitX.setDamage(self.currentUnit.getDamage(), -1)
			elif iChangeType == 4:
				unitX.setMoves(self.currentUnit.getMoves())
			elif iChangeType == 5:
				unitX.setImmobileTimer(self.currentUnit.getImmobileTimer())
			elif iChangeType == 6:
				unitX.setPromotionReady(self.currentUnit.isPromotionReady())
			elif iChangeType == 7:
				unitX.setMadeAttack(self.currentUnit.isMadeAttack())
			elif iChangeType == 8:
				unitX.setMadeInterception(self.currentUnit.isMadeInterception())
			elif iChangeType == 9:
				self.changeDirection(self.currentUnit.getFacingDirection(), unitX)
			elif iChangeType == 10:
				unitX.setUnitAIType(self.currentUnit.getUnitAIType())
			elif iChangeType == 11:
				unitX.changeCargoSpace(self.currentUnit.cargoSpace() - unitX.cargoSpace())
			elif iChangeType == 12:
				unitX.setScriptData(self.currentUnit.getScriptData())


	def handleInput(self, inputClass, screen):

		HandleInputUtil.debugInput(inputClass)
		iCode	= inputClass.eNotifyCode
		iData	= inputClass.iData
		iData1	= inputClass.iData1
		iData2	= inputClass.iData2
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName
		szFlag	= HandleInputUtil.MOUSE_FLAGS.get(inputClass.uiFlags, "UNKNOWN")

		if iCode == 6: # Character
			if iData == 1:
				self.exit(screen)
				return 1
			return 0
		elif iCode in (16, 17):
			return 0

		szSplit = NAME.split("|")
		BASE = szSplit[0]
		if szSplit[1:]:
			TYPE = szSplit[1]
		else:
			TYPE = ""
		if szSplit[2:]:
			CASE = szSplit[2:]
		else:
			CASE = [""]

		global iChange
		global iChangeType
		global iOwnerType
		global iPlotType
		global iCopyType
		global iCommandUnitType
		global iMissionType
		global iMissionData1

		if iCode == 4: # Mouse Enter

			if TYPE == "WBCargoUnits":
				iPlayer = self.currentUnit.getOwner()
				unitX = GC.getPlayer(iPlayer).getUnit(ID)
				sText = CyGameTextMgr().getSpecificUnitHelp(unitX, True, False)
				sText += "\n" + TRNSLTR.getText("TXT_WORD_UNIT", ()) + " ID: " + str(ID)
				sText += "\n" + TRNSLTR.getText("TXT_KEY_WB_GROUP", ()) + " ID: " + str(unitX.getGroupID())
				sText += "\n" + "X: " + str(unitX.getX()) + ", Y: " + str(unitX.getY())
				sText += "\n" + TRNSLTR.getText("TXT_KEY_WB_AREA_ID", ()) + ": "  + str(unitX.plot().getArea())
				self.WB.tooltip.handle(screen, sText)

		elif not iCode: # click

			if TYPE == "WBCargoUnits":
				self.handleCargoChange(screen, ID)

			elif NAME == "UnitCargoSpaceChange":
				if ID:
					self.currentUnit.changeCargoSpace(iChange)
					if self.currentUnit.cargoSpace() == iChange:
						self.placeCargo()
						return 1
				elif self.currentUnit.cargoSpace() > 0:
					self.currentUnit.changeCargoSpace(- min(iChange, self.currentUnit.cargoSpace()))
					if not self.currentUnit.cargoSpace():
						self.placeCargo()
						return 1
				else:
					return 1

				TXT = "CargoSpaceHeader"
				screen.hide(TXT)
				sText1 = TRNSLTR.getText("TXT_WORD_CARGO", ()) + " (" + str(self.currentUnit.getCargo()) + "/" + str(self.currentUnit.cargoSpace()) + ")"
				screen.modifyLabel(TXT, "<font=3b>" + sText1, 1<<0)
				screen.show(TXT)

			elif NAME == "UnitScreenHeader":
				popup = CyPopup(5006, EventContextTypes.EVENTCONTEXT_ALL, True)
				popup.setUserData((self.currentUnit.getOwner(), self.currentUnit.getID()))
				popup.setBodyString(TRNSLTR.getText("TXT_KEY_RENAME_UNIT", ()), 1<<0)
				popup.createEditBox(self.currentUnit.getNameNoDesc(), 0)
				popup.setEditBoxMaxCharCount(25, 32, 0)
				popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

			elif NAME.find("UnitLevel") > -1:
				if iData1 == 1030:
					self.currentUnit.setLevel(self.currentUnit.getLevel() + iChange)
				elif iData1 == 1031:
					self.currentUnit.setLevel(max(0, self.currentUnit.getLevel() - iChange))
				self.placeStats()

			elif NAME.find("UnitExperience") > -1:
				if iData1 == 1030:
					self.currentUnit.changeExperience(iChange, -1, False, False, False)
				elif iData1 == 1031:
					self.currentUnit.changeExperience(- min(iChange, self.currentUnit.getExperience()), -1, False, False, False)
				self.placeStats()

			elif NAME.find("UnitBaseStr") > -1:
				if iData1 == 1030:
					self.currentUnit.setBaseCombatStr(self.currentUnit.baseCombatStr() + iChange)
				elif iData1 == 1031:
					self.currentUnit.setBaseCombatStr(max(0, self.currentUnit.baseCombatStr() - iChange))
				self.placeStats()

			elif NAME.find("UnitDamage") > -1:
				if iData1 == 1030:
					self.currentUnit.changeDamage(iChange, -1)
				elif iData1 == 1031:
					self.currentUnit.changeDamage(- min(iChange, self.currentUnit.getDamage()), -1)
				self.placeStats()

			elif NAME.find("UnitMovesLeft") > -1:
				if iData1 == 1030:
					self.currentUnit.changeMoves(- iChange * GC.getDefineINT("MOVE_DENOMINATOR"))
				elif iData1 == 1031:
					self.currentUnit.changeMoves(min(iChange * GC.getDefineINT("MOVE_DENOMINATOR"), self.currentUnit.movesLeft()))
				self.placeStats()

			elif NAME.find("UnitImmobile") > -1:
				if iData1 == 1030:
					self.currentUnit.setImmobileTimer(self.currentUnit.getImmobileTimer() + iChange)
				elif iData1 == 1031:
					self.currentUnit.setImmobileTimer(max(0, self.currentUnit.getImmobileTimer() - iChange))
				self.placeStats()

			elif NAME == "PromotionReadyText":
				self.currentUnit.setPromotionReady(not self.currentUnit.isPromotionReady())
				self.placeDirection()

			elif NAME == "MadeAttackText":
				self.currentUnit.setMadeAttack(not self.currentUnit.isMadeAttack())
				self.placeDirection()
				self.placeMissions()

			elif NAME == "MadeInterceptionText":
				self.currentUnit.setMadeInterception(not self.currentUnit.isMadeInterception())
				self.placeDirection()
				self.placeMissions()

			elif NAME == "WBUnitDirection":
				self.changeDirection(iData2, self.currentUnit)
				self.placeDirection()

			elif NAME == "UnitEditScriptData":
				popup = CyPopup(3333, EventContextTypes.EVENTCONTEXT_ALL, True)
				popup.setHeaderString(TRNSLTR.getText("TXT_KEY_WB_SCRIPT", ()), 1<<2)
				popup.setUserData((self.currentUnit.getOwner(), self.currentUnit.getID()))
				popup.createEditBox(self.currentUnit.getScriptData(), 0)
				popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

			elif NAME == "CopyStats":
				self.handleCopyAll()

			elif NAME == "MissionInput":
				iMissionData1 = iData2
				self.placeMissions()

			elif NAME == "PushMission":
				self.doMission()

			elif NAME == "TargetPlot":
				self.WB.iPlayerAddMode = "TargetPlot"
				self.WB.TempInfo = [self.currentUnit.getOwner(), self.currentUnit.getID()]
				#screen.hideScreen()

			elif NAME == "ExitSubScreen":
				self.exit(screen)

		elif iCode == 11: # List Select

			if NAME == "ChangeBy":
				iChange = screen.getPullDownData("ChangeBy", screen.getSelectedPullDownID("ChangeBy"))

			elif NAME == "CurrentPage":
				iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))

				if iIndex == 1:
					WBPromotionScreen.WBPromotionScreen(self.WB).interfaceScreen(self.currentUnit)
				elif iIndex == 2:
					WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(self.currentUnit.getOwner())
				elif iIndex == 3:
					WBTeamScreen.WBTeamScreen(self.WB).interfaceScreen(self.currentUnit.getTeam())
				elif iIndex == 4:
					WBPlotScreen.WBPlotScreen(self.WB).interfaceScreen(self.currentPlot)
				elif iIndex == 5:
					WBEventScreen.WBEventScreen(self.WB).interfaceScreen(self.currentPlot)
				elif iIndex == 6:
					WBPlayerUnits.WBPlayerUnits(self.WB).interfaceScreen(self.currentUnit.getOwner())
				elif iIndex == 11:
					import WBInfoScreen
					WBInfoScreen.WBInfoScreen(self.WB).interfaceScreen(self.currentUnit.getOwner())

			elif NAME == "WBCurrentUnit":
				if iData1 >= 8300:
					iPlayer = iData1 - 8300
					self.interfaceScreen([GC.getPlayer(iPlayer).getUnit(iData2)])

			elif NAME == "ChangeType":
				iChangeType = screen.getPullDownData("ChangeType", screen.getSelectedPullDownID("ChangeType"))

			elif NAME == "CargoType":
				self.bCargo = not self.bCargo
				self.placeCargo()

			elif NAME == "OwnerType":
				iOwnerType = screen.getPullDownData("OwnerType", screen.getSelectedPullDownID("OwnerType"))
				self.sortUnits()

			elif NAME == "PlotType":
				iPlotType = screen.getPullDownData("PlotType", screen.getSelectedPullDownID("PlotType"))
				self.sortUnits()

			elif NAME == "CopyType":
				iCopyType = screen.getPullDownData("CopyType", screen.getSelectedPullDownID("CopyType"))
				self.sortUnits()

			elif NAME == "UnitOwner":
				self.changeOwner(screen.getPullDownData("UnitOwner", screen.getSelectedPullDownID("UnitOwner")))

			elif NAME == "UnitAIType":
				self.currentUnit.setUnitAIType(screen.getPullDownData("UnitAIType", screen.getSelectedPullDownID("UnitAIType")))

			elif NAME == "CommandUnits":
				iCommandUnitType = screen.getPullDownData("CommandUnits", screen.getSelectedPullDownID("CommandUnits"))

			elif NAME == "Commands":
				iIndex = screen.getPullDownData("Commands", screen.getSelectedPullDownID("Commands"))
				aList = []
				self.WB.lMoveUnit = []
				if iCommandUnitType == 0:
					aList.append(self.currentUnit)
				else:
					for self.currentUnitX in self.currentPlot.units():
						if self.currentUnitX.getOwner() != self.currentUnit.getOwner(): continue
						if iCommandUnitType == 1:
							if self.currentUnitX.getUnitType() != self.unitType: continue
						elif iCommandUnitType == 2:
							if self.currentUnitX.getUnitCombatType() != self.currentUnit.getUnitCombatType(): continue
						elif iCommandUnitType == 3:
							if self.currentUnitX.getDomainType() != self.currentUnit.getDomainType(): continue
						elif iCommandUnitType == 4:
							if self.currentUnitX.getGroupID() != self.currentUnit.getGroupID(): continue
						aList.append(self.currentUnitX)
				iRefresh = 0
				for self.currentUnitX in aList:
					iRefresh = self.doAllCommands(self.currentUnitX, iIndex)
				#screen.hideScreen()
				if iRefresh == 1:
					self.interfaceScreen([self.currentPlot.getUnit(0)])
				elif iRefresh == 2:
					self.interfaceScreen([self.currentUnit])

			elif NAME == "Missions":
				iMissionType = screen.getPullDownData("Missions", screen.getSelectedPullDownID("Missions"))
				iMissionData1 = -1
				self.placeMissions()

		return 1


	def handleCargoChange(self, screen, ID):
		color = ""
		unitX = GC.getPlayer(self.currentUnit.getOwner()).getUnit(ID)
		if self.bCargo:
			transport = unitX.getTransportUnit()

			if transport and transport.getID() == self.currentUnit.getID():
				unitX.setTransportUnit(unitX, False)
				self.modifyCargoEntry(screen, unitX, ID, "<color=255,80,80>")

			elif not self.currentUnit.isFull():
				unitX.setTransportUnit(self.currentUnit, True)
				self.modifyCargoEntry(screen, unitX, ID, "<color=128,255,28>")

		else:
			transport = self.currentUnit.getTransportUnit()

			if transport:
				self.currentUnit.setTransportUnit(self.currentUnit, False)

				if transport.getID() == ID:
					self.modifyCargoEntry(screen, unitX, ID, "<color=255,80,80>")

				elif not unitX.isFull():
					self.currentUnit.setTransportUnit(unitX, True)
					self.modifyCargoEntry(screen, GC.getPlayer(self.currentUnit.getOwner()).getUnit(transport.getID()), transport.getID(), "<color=255,80,80,255>")
					self.modifyCargoEntry(screen, unitX, ID, "<color=128,255,28>")

			elif not unitX.isFull():
				self.currentUnit.setTransportUnit(unitX, True)
				self.modifyCargoEntry(screen, unitX, ID, "<color=128,255,28>")


	def modifyCargoEntry(self, screen, unitX, ID, color):
		NAME = "WID|WBCargoUnits" + str(ID)
		screen.hide(NAME)
		if unitX.getNameNoDesc():
			sText = unitX.getNameNoDesc()
		else: sText = unitX.getName()

		if self.bCargo:
			TXT = "CargoSpaceHeader"
			screen.hide(TXT)
			sText1 = TRNSLTR.getText("TXT_WORD_CARGO", ()) + " (" + str(self.currentUnit.getCargo()) + "/" + str(self.currentUnit.cargoSpace()) + ")"
			screen.modifyLabel(TXT, "<font=3b>" + sText1, 1<<0)
			screen.show(TXT)
		else:
			sText += " (" + str(unitX.getCargo()) + "/" + str(unitX.cargoSpace()) + ")"

		screen.modifyString(NAME, "<font=3>" + color + sText, 1<<0)
		screen.show(NAME)


	def update(self, fDelta):
		iMapHeight = min(self.xRes / 2 - 235, (self.xRes * 2/5 - 80) /3)
		iMapWidth = iMapHeight * 3/2
		self.WB.getScreen().addPlotGraphicGFC("PlotView", self.xRes/2 - iMapWidth/2, 110, iMapWidth, iMapHeight, self.currentPlot, 250, True, WidgetTypes.WIDGET_GENERAL, 1, 2)

	# # # # # #
	# Clean Up
	def exit(self, screen):
		for widget in self.aWidgetBucket:
			screen.deleteWidget(widget)
		del self.currentUnit, self.currentPlot, self.unitType, self.iPlotX, self.iPlotY, \
			self.aWidgetBucket, self.xRes, self.yRes, self.lUnits, self.bCargo

		screen.setRenderInterfaceOnly(False)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, True)
		screen.setForcedRedraw(True)
		self.WB.inSubScreen = None
