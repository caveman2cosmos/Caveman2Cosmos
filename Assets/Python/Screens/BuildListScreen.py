## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvScreenEnums

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

buildListScreen = None

class BuildListScreen:
  "Build List Screen"
  def interfaceScreen (self):

    self.X_SCREEN = 0
    self.Y_SCREEN = 0
    self.W_SCREEN = 1024
    self.H_SCREEN = 768

    self.PANEL_HEIGHT = 90

    self.X_EXIT = 410
    self.Y_EXIT = 326

    self.LISTS_WIDTH = 200
    self.QUEUE_WIDTH = 200

    buildListScreen = self

    self.createBuildListScreen()

  def createBuildListScreen(self):

    screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
    self.W_SCREEN = screen.getXResolution()
    self.H_SCREEN = screen.getYResolution()
    # Create a new screen, called BuildListScreen, using the file BuildListScreen.py for input
    screen = CyGInterfaceScreen( "BuildListScreen", CvScreenEnums.BUILD_LIST_SCREEN )
    screen.setDimensions(self.X_SCREEN, self.Y_SCREEN, self.W_SCREEN, self.H_SCREEN)
    screen.setRenderInterfaceOnly(True)
    screen.addDDSGFC("BuildListBackground", ArtFileMgr.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )

    # Panels on the Top(name of screen) and bottom(Exit button)
    screen.addPanel( "BuildListTopPanel", u"", u"", True, False, 0, 0, self.W_SCREEN, self.PANEL_HEIGHT, PanelStyles.PANEL_STYLE_TOPBAR )
    screen.addPanel( "BuildListBottomPanel", u"", u"", True, False, 0, self.H_SCREEN - self.PANEL_HEIGHT, self.W_SCREEN, self.PANEL_HEIGHT, PanelStyles.PANEL_STYLE_BOTTOMBAR )
    screen.showWindowBackground(False)

    # Calvitix : Dynamic buttons width for french language

    iLanguage = gc.getGame().getCurrentLanguage()
    bFrench = iLanguage == 1 #0 - English, 1 - French, 2 - German, 3 - Italian, 4 - Spanish

    iAddButtonPosition = 30
    iRenameButtonPosition = 110
    iDeleteButtonPosition = 240
    iSaveButtonPosition = 370

    if (bFrench):
      iAddButtonPosition = 50
      iRenameButtonPosition = 170
      iDeleteButtonPosition = 320
      iSaveButtonPosition = 470


    # set the standard "exit" text
    screen.setText("BuildListCancelButton", "Background", u"<font=4>" + localText.getText("TXT_WORD_EXIT", ()).upper() + u"</font>", 1<<0, self.W_SCREEN - 80, self.H_SCREEN - 40, -0.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, 1, 0)

    # set the text button to add a list
    screen.setText("BuildListAddButton", "Background", u"<font=4>" + localText.getText("TXT_KEY_LISTSCREEN_BUTTON_ADD", ()).upper() + u"</font>", 1<<0, iAddButtonPosition, self.H_SCREEN - 40, -0.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

    # set the text button to rename a list
    screen.setText("BuildListRenameButton", "Background", u"<font=4>" + localText.getText("TXT_KEY_LISTSCREEN_BUTTON_RENAME", ()).upper() + u"</font>", 1<<0, iRenameButtonPosition, self.H_SCREEN - 40, -0.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

    # set the text button to delete a list
    screen.setText("BuildListDeleteButton", "Background", u"<font=4>" + localText.getText("TXT_KEY_LISTSCREEN_BUTTON_DELETE", ()).upper() + u"</font>", 1<<0, iDeleteButtonPosition, self.H_SCREEN - 40, -0.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

    # set the text button to add a list
    screen.setText("BuildListSaveButton", "Background", u"<font=4>" + localText.getText("TXT_KEY_LISTSCREEN_BUTTON_SAVE", ()).upper() + u"</font>", 1<<0, iSaveButtonPosition, self.H_SCREEN - 40, -0.3, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

    # draw the lists
    self.drawLists()

    # draw the queue
    self.drawQueue()

    # draw the builds
    self.drawBuilds()

    # Show the screen
    screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

  def drawBuilds(self):
    # AIAndy: Filter buttons

    screen = CyGInterfaceScreen( "BuildListScreen", CvScreenEnums.BUILD_LIST_SCREEN )

    pPlayer = gc.getActivePlayer()

    iMultiListXL = self.LISTS_WIDTH + self.QUEUE_WIDTH + 20
    iMultiListXR = 50

    # Calvitix : Dynamic buttons width for french language

    iLanguage = gc.getGame().getCurrentLanguage()
    bFrench = iLanguage == 1 #0 - English, 1 - French, 2 - German, 3 - Italian, 4 - Spanish

    iGroupButtonWidth = 90
    iSortButtonWidth = 90

    if (bFrench):
      iGroupButtonWidth = 110
      iSortButtonWidth = 160

    iSortPosition = iGroupButtonWidth + 20

    # show buttons for units
    iNumFilterButtons = 13
    iFilterContainerLength = self.W_SCREEN - (iMultiListXL+iMultiListXR+200)
    if (iFilterContainerLength > iNumFilterButtons * 27):
      iFilterContainerLength = iNumFilterButtons * 27

    screen.addMultiListControlGFC( "UnitFilterButtonContainer", u"", iMultiListXL, 10, iFilterContainerLength, 28, 1, 25, 25, TableStyles.TABLE_STYLE_STANDARD )
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_NOPRODUCTION").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_HIDE_UNBUILDABLE, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_HIDE_UNBUILDABLE), 0, 0)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("BUG_WORLDWONDER_ON").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_HIDE_LIMITED, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_HIDE_LIMITED), 0, 1)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("PLE_FILTER_LAND").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_LAND, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_LAND), 0, 2)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("PLE_FILTER_AIR").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_AIR, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_AIR), 0, 3)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("PLE_FILTER_SEA").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_WATER, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_WATER), 0, 4)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_TECH_WORKER_SPEED").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_WORKERS, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_WORKERS), 0, 5)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("PLE_FILTER_DOM").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_CIVILIAN, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_CIVILIAN), 0, 6)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("PLE_GRP_UNITTYPE").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_SIEGE, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_SIEGE), 0, 7)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_TECH_MOVE_BONUS").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_MOUNTED, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_MOUNTED), 0, 8)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("PLE_FILTER_MIL").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_HEROES, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_HEROES), 0, 9)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_MILITARY, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_MILITARY), 0, 10)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BTN_MILITARY").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_DEFENSE, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_DEFENSE), 0, 11)
    screen.appendMultiListButton( "UnitFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_ANGRYCITIZEN_TEXTURE").getPath(), 0, WidgetTypes.WIDGET_UNIT_FILTER, UnitFilterTypes.UNIT_FILTER_SHOW_MISSIONARY, 1, False )
    screen.enableMultiListPulse( "UnitFilterButtonContainer", pPlayer.getUnitListFilterActive(UnitFilterTypes.UNIT_FILTER_SHOW_MISSIONARY), 0, 12)
    screen.show( "UnitFilterButtonContainer" )

    iSelectedGrouping = pPlayer.getUnitListGrouping()
    screen.addDropDownBoxGFC( "UnitGrouping", iMultiListXL + iFilterContainerLength + 10, 8, iGroupButtonWidth, WidgetTypes.WIDGET_UNIT_GROUPING, -1, -1, FontTypes.SMALL_FONT)
    screen.addPullDownString( "UnitGrouping", localText.getText("TXT_KEY_UNITHELP_GROUPING_SINGLE", ()), UnitGroupingTypes.UNIT_GROUPING_SINGLE, UnitGroupingTypes.UNIT_GROUPING_SINGLE, iSelectedGrouping == UnitGroupingTypes.UNIT_GROUPING_SINGLE)
    screen.addPullDownString( "UnitGrouping", localText.getText("TXT_KEY_UNITHELP_GROUPING_COMBAT", ()), UnitGroupingTypes.UNIT_GROUPING_COMBAT, UnitGroupingTypes.UNIT_GROUPING_COMBAT, iSelectedGrouping == UnitGroupingTypes.UNIT_GROUPING_COMBAT)
    screen.addPullDownString( "UnitGrouping", localText.getText("TXT_KEY_DOMAIN", ()), UnitGroupingTypes.UNIT_GROUPING_DOMAIN, UnitGroupingTypes.UNIT_GROUPING_DOMAIN, iSelectedGrouping == UnitGroupingTypes.UNIT_GROUPING_DOMAIN)
    screen.addPullDownString( "UnitGrouping", localText.getText("TXT_KEY_UNITHELP_GROUPING_HERO", ()), UnitGroupingTypes.UNIT_GROUPING_HERO, UnitGroupingTypes.UNIT_GROUPING_HERO, iSelectedGrouping == UnitGroupingTypes.UNIT_GROUPING_HERO)
    screen.show( "UnitGrouping" )

    iSelectedSorting = pPlayer.getUnitListSorting()
    screen.addDropDownBoxGFC( "UnitSorting", iMultiListXL + iFilterContainerLength + iSortPosition, 8, iSortButtonWidth, WidgetTypes.WIDGET_UNIT_SORT, -1, -1, FontTypes.SMALL_FONT)
    screen.addPullDownString( "UnitSorting", localText.getText("TXT_WORD_NAME", ()), UnitSortTypes.UNIT_SORT_NAME, UnitSortTypes.UNIT_SORT_NAME, iSelectedSorting == UnitSortTypes.UNIT_SORT_NAME)
    screen.addPullDownString( "UnitSorting", localText.getText("TXT_WORD_COST", ()), UnitSortTypes.UNIT_SORT_COST, UnitSortTypes.UNIT_SORT_COST, iSelectedSorting == UnitSortTypes.UNIT_SORT_COST)
    screen.addPullDownString( "UnitSorting", localText.getText("TXT_KEY_UNITHELP_SORT_STRENGTH", ()), UnitSortTypes.UNIT_SORT_STRENGTH, UnitSortTypes.UNIT_SORT_STRENGTH, iSelectedSorting == UnitSortTypes.UNIT_SORT_STRENGTH)
    screen.addPullDownString( "UnitSorting", localText.getText("TXT_KEY_UNITHELP_SORT_MOVE", ()), UnitSortTypes.UNIT_SORT_MOVE, UnitSortTypes.UNIT_SORT_MOVE, iSelectedSorting == UnitSortTypes.UNIT_SORT_MOVE)
    screen.addPullDownString( "UnitSorting", localText.getText("TXT_KEY_UNITHELP_SORT_COLLATERAL", ()), UnitSortTypes.UNIT_SORT_COLLATERAL, UnitSortTypes.UNIT_SORT_COLLATERAL, iSelectedSorting == UnitSortTypes.UNIT_SORT_COLLATERAL)
    screen.addPullDownString( "UnitSorting", localText.getText("TXT_KEY_UNITHELP_SORT_RANGE", ()), UnitSortTypes.UNIT_SORT_RANGE, UnitSortTypes.UNIT_SORT_RANGE, iSelectedSorting == UnitSortTypes.UNIT_SORT_RANGE)
    screen.addPullDownString( "UnitSorting", localText.getText("TXT_KEY_UNITHELP_SORT_BOMBARD", ()), UnitSortTypes.UNIT_SORT_BOMBARD, UnitSortTypes.UNIT_SORT_BOMBARD, iSelectedSorting == UnitSortTypes.UNIT_SORT_BOMBARD)
    screen.addPullDownString( "UnitSorting", localText.getText("TXT_WORD_CARGO", ()), UnitSortTypes.UNIT_SORT_CARGO, UnitSortTypes.UNIT_SORT_CARGO, iSelectedSorting == UnitSortTypes.UNIT_SORT_CARGO)
    screen.addPullDownString( "UnitSorting", localText.getText("TXT_KEY_UNITHELP_SORT_WITHDRAWAL", ()), UnitSortTypes.UNIT_SORT_WITHDRAWAL, UnitSortTypes.UNIT_SORT_WITHDRAWAL, iSelectedSorting == UnitSortTypes.UNIT_SORT_WITHDRAWAL)
    screen.addPullDownString( "UnitSorting", localText.getText("TXT_KEY_UNITHELP_SORT_POWER", ()), UnitSortTypes.UNIT_SORT_POWER, UnitSortTypes.UNIT_SORT_POWER, iSelectedSorting == UnitSortTypes.UNIT_SORT_POWER)
    screen.show( "UnitSorting" )

    iNumFilterButtons = 18
    iFilterContainerLength = self.W_SCREEN - (iMultiListXL+iMultiListXR+200)
    if (iFilterContainerLength > iNumFilterButtons * 27):
      iFilterContainerLength = iNumFilterButtons * 27

    screen.addMultiListControlGFC( "BuildingFilterButtonContainer", u"", iMultiListXL, 40, iFilterContainerLength, 28, 1, 25, 25, TableStyles.TABLE_STYLE_STANDARD )
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BUTTONS_NOPRODUCTION").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_HIDE_UNBUILDABLE, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_HIDE_UNBUILDABLE), 0, 0)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_TECH_GOLDTRADING").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_SCIENCE, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_SCIENCE), 0, 1)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("ESPIONAGE_BUTTON").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_ESPIONAGE, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_ESPIONAGE), 0, 2)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("CULTURE_BUTTON").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_CULTURE, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_CULTURE), 0, 3)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("RAW_YIELDS_COMMERCE").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_GOLD, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_GOLD), 0, 4)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("RAW_YIELDS_FOOD").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_FOOD, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_FOOD), 0, 5)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("RAW_YIELDS_PRODUCTION").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_PRODUCTION, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_PRODUCTION), 0, 6)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_TECH_HAPPINESS").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_HAPPINESS, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_HAPPINESS), 0, 7)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_TECH_HEALTH").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_HEALTH, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_HEALTH), 0, 8)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_MILITARY, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_MILITARY), 0, 9)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_BTN_MILITARY").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_CITY_DEFENSE, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_CITY_DEFENSE), 0, 10)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_ANGRYCITIZEN_TEXTURE").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_HIDE_UNHAPPINESS, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_HIDE_UNHAPPINESS), 0, 11)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_UNHEALTHY_PERSON").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_HIDE_UNHEALTHINESS, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_HIDE_UNHEALTHINESS), 0, 12)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("BUG_WORLDWONDER_ON").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_HIDE_GREAT_WONDER, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_HIDE_GREAT_WONDER), 0, 13)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("BUG_NATWONDER_ON").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_HIDE_NATIONAL_WONDER, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_HIDE_NATIONAL_WONDER), 0, 14)
    screen.appendMultiListButton( "BuildingFilterButtonContainer", ArtFileMgr.getInterfaceArtInfo("INTERFACE_TECH_WORKER_SPEED").getPath(), 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_HIDE_NORMAL, 1, False )
    screen.enableMultiListPulse( "BuildingFilterButtonContainer", pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_HIDE_NORMAL), 0, 15)
    if pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_CRIME):
      screen.appendMultiListButton( "BuildingFilterButtonContainer", "Art/showcrimeOn.dds", 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_CRIME, 1, False )
    else:
      screen.appendMultiListButton( "BuildingFilterButtonContainer", "Art/showcrimeOff.dds", 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_CRIME, 1, False )
    if pPlayer.getBuildingListFilterActive(BuildingFilterTypes.BUILDING_FILTER_SHOW_FLAMMABILITY):
      screen.appendMultiListButton( "BuildingFilterButtonContainer", "Art/showfireOn.dds", 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_FLAMMABILITY, 1, False )
    else:
      screen.appendMultiListButton( "BuildingFilterButtonContainer", "Art/showfireOff.dds", 0, WidgetTypes.WIDGET_BUILDING_FILTER, BuildingFilterTypes.BUILDING_FILTER_SHOW_FLAMMABILITY, 1, False )
    screen.show( "BuildingFilterButtonContainer" )

    iSelectedGrouping = pPlayer.getBuildingListGrouping()
    screen.addDropDownBoxGFC("BuildingGrouping", iMultiListXL + iFilterContainerLength + 10, 38, iGroupButtonWidth, WidgetTypes.WIDGET_BUILDING_GROUPING, -1, -1, FontTypes.SMALL_FONT)
    screen.addPullDownString("BuildingGrouping", localText.getText("TXT_KEY_BUILDINGHELP_GROUPING_SINGLE", ()), BuildingGroupingTypes.BUILDING_GROUPING_SINGLE, BuildingGroupingTypes.BUILDING_GROUPING_SINGLE, iSelectedGrouping == BuildingGroupingTypes.BUILDING_GROUPING_SINGLE)
    screen.addPullDownString("BuildingGrouping", localText.getText("TXT_KEY_BUILDINGHELP_GROUPING_WONDER_TYPE", ()), BuildingGroupingTypes.BUILDING_GROUPING_WONDER_TYPE, BuildingGroupingTypes.BUILDING_GROUPING_WONDER_TYPE, iSelectedGrouping == BuildingGroupingTypes.BUILDING_GROUPING_WONDER_TYPE)
    screen.addPullDownString("BuildingGrouping", localText.getText("TXT_KEY_DOMAIN", ()), BuildingGroupingTypes.BUILDING_GROUPING_DOMAIN, BuildingGroupingTypes.BUILDING_GROUPING_DOMAIN, iSelectedGrouping == BuildingGroupingTypes.BUILDING_GROUPING_DOMAIN)
    screen.show( "BuildingGrouping" )

    iSelectedSorting = pPlayer.getBuildingListSorting()
    screen.addDropDownBoxGFC("BuildingSorting", iMultiListXL + iFilterContainerLength + iSortPosition, 38, iSortButtonWidth, WidgetTypes.WIDGET_BUILDING_SORT, -1, -1, FontTypes.SMALL_FONT)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_WORD_NAME", ()), BuildingSortTypes.BUILDING_SORT_NAME, BuildingSortTypes.BUILDING_SORT_NAME, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_NAME)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_WORD_COST", ()), BuildingSortTypes.BUILDING_SORT_COST, BuildingSortTypes.BUILDING_SORT_COST, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_COST)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_KEY_BUILDINGHELP_SORT_SCIENCE", ()), BuildingSortTypes.BUILDING_SORT_SCIENCE, BuildingSortTypes.BUILDING_SORT_SCIENCE, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_SCIENCE)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_WORD_ESPIONAGE", ()), BuildingSortTypes.BUILDING_SORT_ESPIONAGE, BuildingSortTypes.BUILDING_SORT_ESPIONAGE, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_ESPIONAGE)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_WORD_CULTURE", ()), BuildingSortTypes.BUILDING_SORT_CULTURE, BuildingSortTypes.BUILDING_SORT_CULTURE, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_CULTURE)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_KEY_BUILDINGHELP_SORT_GOLD", ()), BuildingSortTypes.BUILDING_SORT_GOLD, BuildingSortTypes.BUILDING_SORT_GOLD, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_GOLD)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_WORD_FOOD", ()), BuildingSortTypes.BUILDING_SORT_FOOD, BuildingSortTypes.BUILDING_SORT_FOOD, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_FOOD)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_KEY_PRODUCTION", ()), BuildingSortTypes.BUILDING_SORT_PRODUCTION, BuildingSortTypes.BUILDING_SORT_PRODUCTION, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_PRODUCTION)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_KEY_BUILDINGHELP_SORT_HAPPINESS", ()), BuildingSortTypes.BUILDING_SORT_HAPPINESS, BuildingSortTypes.BUILDING_SORT_HAPPINESS, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_HAPPINESS)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_KEY_BUILDINGHELP_SORT_HEALTH", ()), BuildingSortTypes.BUILDING_SORT_HEALTH, BuildingSortTypes.BUILDING_SORT_HEALTH, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_HEALTH)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_KEY_BUILDINGHELP_SORT_CRIME", ()), BuildingSortTypes.BUILDING_SORT_CRIME, BuildingSortTypes.BUILDING_SORT_CRIME, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_CRIME)
    screen.addPullDownString("BuildingSorting", localText.getText("TXT_KEY_PROPERTY_FLAMMABILITY", ()), BuildingSortTypes.BUILDING_SORT_FLAMMABILITY, BuildingSortTypes.BUILDING_SORT_FLAMMABILITY, iSelectedSorting == BuildingSortTypes.BUILDING_SORT_FLAMMABILITY)
    screen.show("BuildingSorting")


    screen.addMultiListControlGFC( "ButtonContainer", u"", iMultiListXL, self.PANEL_HEIGHT + 10, self.W_SCREEN - (iMultiListXL+iMultiListXR), self.H_SCREEN - 20 - 2*self.PANEL_HEIGHT, 4, 48, 48, TableStyles.TABLE_STYLE_STANDARD )

    iCurrentList = pPlayer.getBLCurrentList()

    iCount = 0
    iRow = 0
    bFound = False

    # Use unit filters
    iGroupNum = pPlayer.getUnitListGroupNum()
    for iGroup in range(iGroupNum):
      iNumInGroup = pPlayer.getUnitListNumInGroup(iGroup)
      for iPos in range(iNumInGroup):
        eLoopUnit = pPlayer.getUnitListType(iGroup, iPos)
        screen.appendMultiListButton( "ButtonContainer", gc.getUnitInfo(eLoopUnit).getButton(), iRow, WidgetTypes.WIDGET_BUILD_LIST_TRAIN, eLoopUnit, iCurrentList, False )

        iCount = iCount + 1
        bFound = True
      iCount = 0
      if (bFound):
        iRow += 1
        bFound = False

    iFirstBuildingRow = iRow

    # Use building filters
    iGroupNum = pPlayer.getBuildingListGroupNum()
    for iGroup in range(iGroupNum):
      iNumInGroup = pPlayer.getBuildingListNumInGroup(iGroup)
      for iPos in range(iNumInGroup):
        eLoopBuilding = pPlayer.getBuildingListType(iGroup, iPos)
        screen.appendMultiListButton( "ButtonContainer", gc.getBuildingInfo(eLoopBuilding).getButton(), iRow, WidgetTypes.WIDGET_BUILD_LIST_CONSTRUCT, eLoopBuilding, iCurrentList, False )

        iCount = iCount + 1
        bFound = True
      iCount = 0
      if (bFound):
        iRow += 1
        bFound = False

    # Projects
    i = 0
    for i in range( gc.getNumProjectInfos() ):
      #if (pPlayer.canCreate(i, False, True)):
        screen.appendMultiListButton( "ButtonContainer", gc.getProjectInfo(i).getButton(), iRow, WidgetTypes.WIDGET_BUILD_LIST_CREATE, i, iCurrentList, False )
        #screen.show( "BottomButtonContainer" )

        iCount = iCount + 1
        bFound = True

    screen.show( "ButtonContainer" )


  def drawQueue(self):

    screen = CyGInterfaceScreen( "BuildListScreen", CvScreenEnums.BUILD_LIST_SCREEN )

    screen.addTableControlGFC( "BuildQueueTable", 1, self.LISTS_WIDTH + 20, self.PANEL_HEIGHT + 10, self.QUEUE_WIDTH - 30, self.H_SCREEN - 20 - 2*self.PANEL_HEIGHT, False, False, 32, 32, TableStyles.TABLE_STYLE_STANDARD )
    screen.setStyle( "BuildQueueTable", "Table_EmptyScroll_Style" )

    #screen.setTableColumnHeader( "BuildListTable", 0, u"", 121 )

    pPlayer = gc.getActivePlayer()
    iCurrentList = pPlayer.getBLIndexByID(pPlayer.getBLCurrentList())

    # Check if current list selection is invalid
    iListNum = pPlayer.getBLNumLists()
    if iCurrentList >= iListNum:
      return
    if iCurrentList < 0:
      return

    iOrders = pPlayer.getBLListLength(iCurrentList)  # get number of orders in list here
    iRow = 0

    for i in range( iOrders ):

      szLeftBuffer = u""
      order = pPlayer.getBLOrder(iCurrentList, i)
      eOrder = order.eOrderType # get order type from list
      iData = order.iData1 # get order data from list
      bSave = order.bSave # get if perma build from list

      if ( eOrder == OrderTypes.ORDER_TRAIN ):
        szLeftBuffer = gc.getUnitInfo(iData).getDescription()

        if (bSave):
          szLeftBuffer = u"*" + szLeftBuffer

      elif ( eOrder == OrderTypes.ORDER_CONSTRUCT ):
        szLeftBuffer = gc.getBuildingInfo(iData).getDescription()

      elif ( eOrder == OrderTypes.ORDER_CREATE ):
        szLeftBuffer = gc.getProjectInfo(iData).getDescription()

      screen.appendTableRow( "BuildQueueTable" )
      screen.setTableText( "BuildQueueTable", 0, iRow, szLeftBuffer, "", WidgetTypes.WIDGET_BUILD_LIST_QUEUE, pPlayer.getBLID(iCurrentList), i, 1<<0 )
      iRow += 1

    screen.show( "BuildQueueTable" )


  def drawLists(self):

    screen = CyGInterfaceScreen( "BuildListScreen", CvScreenEnums.BUILD_LIST_SCREEN )

    screen.addListBoxGFC("BuildListTable", "", 20, self.PANEL_HEIGHT + 10, self.LISTS_WIDTH - 30, self.H_SCREEN - 20 - 2*self.PANEL_HEIGHT, TableStyles.TABLE_STYLE_STANDARD)
    screen.setStyle("BuildListTable", "Table_StandardCiv_Style")
    #screen.addTableControlGFC( "BuildListTable", 1, 20, self.PANEL_HEIGHT + 10, self.LISTS_WIDTH - 30, self.H_SCREEN - 20 - 2*self.PANEL_HEIGHT, False, False, 32, 32, TableStyles.TABLE_STYLE_STANDARD )
    #screen.setStyle( "BuildListTable", "Table_EmptyScroll_Style" )
    screen.enableSelect("BuildListTable", True)

    #screen.setTableColumnHeader( "BuildListTable", 0, u"", 121 )

    pPlayer = gc.getActivePlayer()
    iCurrentList = pPlayer.getBLCurrentList()

    iListNum = pPlayer.getBLNumLists()

    iRow = 0

    for i in range( iListNum ):

      szLeftBuffer = pPlayer.getBLListName(i)

      #screen.appendTableRow( "BuildListTable" )
      #screen.setTableText( "BuildListTable", 0, iRow, szLeftBuffer, "", WidgetTypes.WIDGET_BUILD_LIST, pPlayer.getBLID(i), -1, 1<<0 )
      screen.appendListBoxStringNoUpdate("BuildListTable", szLeftBuffer, WidgetTypes.WIDGET_BUILD_LIST, pPlayer.getBLID(i), -1, 1<<0 )
      #if (iCurrentList == pPlayer.getBLID(i)):
      #  screen.selectRow( "BuildListTable", iRow, True)
      iRow += 1

    screen.updateListBox( "BuildListTable" )
    screen.show( "BuildListTable" )


  def createLogoScreen(self):
#   screen = CyGInterfaceScreen( "IntroMovieScreen", CvScreenEnums.INTRO_MOVIE_SCREEN )
#   screen.setRenderInterfaceOnly(True)
#   screen.showWindowBackground( False )

    # Show the screen
#   screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

    # Background First
#   screen.setImageButton( "LogoBackground", ArtFileMgr.getInterfaceArtInfo("MAINMENU_LOAD").getPath(), screen.centerX(0)-2, screen.centerY(0)-2, 1028, 776, WidgetTypes.WIDGET_GENERAL, -1, -1 )

    # Logos
#   screen.setImageButton( "Logos", ArtFileMgr.getInterfaceArtInfo("INTRO_LOGOS").getPath(), screen.centerX(0)+233, screen.centerY(0)+331, 549, 294, WidgetTypes.WIDGET_GENERAL, -1, -1 )

#   screen.addMultilineText("LegalText1", "<font=1>" + localText.getText("TXT_LEGAL_LINE_1", ()) + "</font>", screen.centerX(0)+110, screen.centerY(0)+590, 804, 200, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
#   screen.setLabel( "LegalText2", "Background", "<font=2>" + localText.getText("TXT_LEGAL_LINE_2", ()) + "</font>", 1<<2, screen.centerX(0)+512, screen.centerY(0)+740, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
#   screen.setShowFor( 4000 )
    return

  def closeScreen(self):
    screen = CyGInterfaceScreen( "BuildListScreen", CvScreenEnums.BUILD_LIST_SCREEN )
    screen.hideScreen()

  def hideScreen(self):
    screen = CyGInterfaceScreen( "BuildListScreen", CvScreenEnums.BUILD_LIST_SCREEN )
    screen.hideScreen()

  # Will handle the input for this screen...
  def handleInput (self, inputClass):
    screen = CyGInterfaceScreen( "BuildListScreen", CvScreenEnums.BUILD_LIST_SCREEN )
    # AIAndy: Input handling for grouping and sorting listbox (filtering input handling is in the DLL but I did not find a way to do the same with listbox widgets):
    if ( inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED ):
      iBuildingGrouping = screen.getPullDownData("BuildingGrouping", screen.getSelectedPullDownID("BuildingGrouping"))
      iBuildingSorting = screen.getPullDownData("BuildingSorting", screen.getSelectedPullDownID("BuildingSorting"))
      iUnitGrouping = screen.getPullDownData("UnitGrouping", screen.getSelectedPullDownID("UnitGrouping"))
      iUnitSorting = screen.getPullDownData("UnitSorting", screen.getSelectedPullDownID("UnitSorting"))
      pPlayer = gc.getPlayer(gc.getGame().getActivePlayer())
      if iBuildingGrouping >= 0:
        pPlayer.setBuildingListGrouping(iBuildingGrouping)
      if iBuildingSorting >= 0:
        pPlayer.setBuildingListSorting(iBuildingSorting)
      if iUnitGrouping >= 0:
        pPlayer.setUnitListGrouping(iUnitGrouping)
      if iUnitSorting >= 0:
        pPlayer.setUnitListSorting(iUnitSorting)
      self.createBuildListScreen()

    elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
      pPlayer = gc.getActivePlayer()
      iCurrentList = pPlayer.getBLCurrentList()
      if (inputClass.getFunctionName() == "BuildListAddButton"):
        pPlayer.addBLList()
      elif (inputClass.getFunctionName() == "BuildListRenameButton"):
        pPlayer.renameBLList(iCurrentList)
      elif (inputClass.getFunctionName() == "BuildListDeleteButton"):
        pPlayer.removeBLList(iCurrentList)
      elif (inputClass.getFunctionName() == "BuildListSaveButton"):
        pPlayer.writeBLToFile()

    return 0

  def update(self, fDelta):
    return
