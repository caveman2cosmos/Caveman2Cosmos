#
# MAD Nukes Manager Mod
# By: The Lopez
# CvMADNukesManager
#

from CvPythonExtensions import *

# globals
GC = CyGlobalContext()
AFM = CyArtFileMgr()
localText = CyTranslator()

OUTGOING_WARHEADS = "OutgoingWarheads"
INCOMING_WARHEADS = "IncomingWarheads"

OUTGOING_WARHEADS_PANEL = "OutgoingWarheadsPanel"
OUTGOING_WARHEADS_INNER_PANEL = "OutgoingWarheadsInnerPanel"
OUTGOING_LABEL_PANEL = "OutgoingLabelPanel"
OUTGOING_SUMMARY_PANEL = "OutgoingSummaryPanel"

OUTGOING_SUMMARY_INNER_PANEL = "OutgoingSummaryInnerPanel"
OUTGOING_SUMMARY_LABEL_PANEL = "OutgoingSummaryLabelPanel"

INCOMING_WARHEADS_PANEL = "IncomingWarheadsPanel"
INCOMING_WARHEADS_INNER_PANEL = "IncomingWarheadsInnerPanel"
INCOMING_LABEL_PANEL = "IncomingLabelPanel"
INCOMING_SUMMARY_PANEL = "IncomingSummaryPanel"

INCOMING_SUMMARY_INNER_PANEL = "IncomingSummaryInnerPanel"
INCOMING_SUMMARY_LABEL_PANEL = "IncomingSummaryLabelPanel"

g_bDebug = False

class CvMADNukesManager:
	"MAD Nukes Manager"

	def __init__(self, screenId):
		self.screenId = screenId

		self.currentScreen = OUTGOING_WARHEADS

		self.MAD_NUKES_MANAGER_SCREEN_NAME = "MADNukesManager"
		self.BACKGROUND_ID = "MADNukesManagerBackground"

		self.OUTGOING_ID = "MADNukesManagerOutgoingWidget"
		self.OUTGOING_SUMMARY_LABEL_ID = "MADNukesManagerOutgoingSummaryLabelWidget"
		self.OUTGOING_LABEL_ID = "MADNukesManagerOutgoingLabelWidget"

		self.INCOMING_ID = "MADNukesManagerIncomingWidget"
		self.INCOMING_SUMMARY_LABEL_ID = "MADNukesManagerOutgoingSummaryLabelWidget"
		self.INCOMING_LABEL_ID = "MADNukesManagerOutgoingLabelWidget"

		self.EXIT_ID = "MADNukesManagerExitWidget"

		self.WIDGET_ID = "MADNukesManagerWidget"

		self.Z_BACKGROUND = -2.1
		self.Z_CONTROLS = self.Z_BACKGROUND - 0.2

		self.X_SCREEN = 500
		self.Y_SCREEN = 396
		self.W_SCREEN = 1024
		self.H_SCREEN = 768
		self.Y_TITLE = 8
		self.BORDER_WIDTH = 4
		self.W_HELP_AREA = 200

		self.X_OUTGOING = 30
		self.Y_OUTGOING = 726

		self.X_INCOMING = 325
		self.Y_INCOMING = 726

		self.X_EXIT = 994
		self.Y_EXIT = 726

		self.nWidgetCount = 0
		self.selectedUnitHash = {}
		self.selectedCityHash = {}

		self.X_MAP = 420
		self.Y_MAP = 60
		self.W_MAP = 580
		self.H_MAP_MAX = 500
		self.MAP_MARGIN = 20

		self.X_TEXT = 625
		self.Y_TEXT = 190
		self.W_TEXT = 380
		self.H_TEXT = 500

		self.iShiftKeyDown = 0

		self.TITLE = u"<font=4b>NUCLEAR WARHEAD MANAGER</font>"

		self.INCOMING_TEXT = u"<font=4>INCOMING WARHEADS</font>"
		self.OUTGOING_TEXT = u"<font=4>OUTGOING WARHEADS</font>"
		self.OUTGOING_LABEL_TEXT = u"<font=3b>OUTGOING WARHEADS</font>"
		self.OUTGOING_SUMMARY_LABEL_TEXT = u"<font=3b>NUCLEAR ARSENAL SUMMARY</font>"

		self.INCOMING_LABEL_TEXT = u"<font=3b>TARGETED CITIES</font>"
		self.INCOMING_SUMMARY_LABEL_TEXT = u"<font=3b>NUCLEAR THREAT SUMMARY</font>"


	def getScreen(self):
		return CyGInterfaceScreen(self.MAD_NUKES_MANAGER_SCREEN_NAME, self.screenId)


	def hideScreen(self):
		screen = self.getScreen()
		screen.hideScreen()
		self.bFirst = True


	# Screen construction function
	def interfaceScreen(self):

		# Create a new screen
		screen = self.getScreen()

		if screen.isActive():
			return

		if(g_bDebug):
			self.showDebugScreen()
			return

		self.nWidgetCount = 0

		screen.setDimensions(screen.centerX(0), screen.centerY(0), self.W_SCREEN, self.H_SCREEN)
		screen.addDDSGFC(self.BACKGROUND_ID, AFM.getInterfaceArtInfo("MAD_NUKES_MANAGER_BACKGROUND").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.initMinimap(screen)

		# Draw the top bar
		self.drawScreenTop(screen)

		# Draw the bottom bar
		self.drawScreenBottom(screen)

		self.drawOutgoingWarheadsContent(screen)

		screen.setRenderInterfaceOnly(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.showWindowBackground(False)

		self.currentScreen = OUTGOING_WARHEADS


	# Draws the top bar of the "Nuclear Warhead Manager" screens
	def drawScreenTop(self, screen):
		screen.addPanel( "TechTopPanel", u"", u"", True, False, 0, 0, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_TOPBAR )

		# Header...
		self.szHeader = self.getNextWidgetName()
		screen.setText(self.szHeader, "Background", self.TITLE, 1<<2, self.X_SCREEN, self.Y_TITLE, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setHitTest(self.szHeader,HitTestTypes.HITTEST_NOHIT)


	# Draws the bottom bar of the "Nuclear Warhead Manager" screens
	def drawScreenBottom(self, screen):

		self.EXIT_TEXT = u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>"

		screen.addPanel( "TechBottomPanel", u"", u"", True, False, 0, 713, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_BOTTOMBAR )
		screen.setText(self.OUTGOING_ID, "Background", self.OUTGOING_TEXT, 1<<0, self.X_OUTGOING, self.Y_OUTGOING, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setText(self.INCOMING_ID, "Background", self.INCOMING_TEXT, 1<<0, self.X_INCOMING, self.Y_INCOMING, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setText(self.EXIT_ID, "Background", self.EXIT_TEXT, 1<<1, self.X_EXIT, self.Y_EXIT, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )


	def initMinimap(self, screen):
		# Minimap initialization
		self.H_MAP = (self.W_MAP * CyMap().getGridHeight()) / CyMap().getGridWidth()
		if (self.H_MAP > self.H_MAP_MAX):
			self.W_MAP = (self.H_MAP_MAX * CyMap().getGridWidth()) / CyMap().getGridHeight()
			self.H_MAP = self.H_MAP_MAX

		screen.addPanel("Minimap", u"", "", False, False, self.X_MAP, self.Y_MAP, self.W_MAP, self.H_MAP, PanelStyles.PANEL_STYLE_MAIN)

		screen.initMinimap(self.X_MAP + self.MAP_MARGIN, self.X_MAP + self.W_MAP - self.MAP_MARGIN, self.Y_MAP + self.MAP_MARGIN, self.Y_MAP + self.H_MAP - self.MAP_MARGIN, self.Z_CONTROLS)
		screen.updateMinimapSection(False, False)

		screen.updateMinimapColorFromMap(MinimapModeTypes.MINIMAPMODE_TERRITORY, 0.3)

		screen.setMinimapMode(MinimapModeTypes.MINIMAPMODE_MILITARY)

		iOldMode = CyInterface().getShowInterface()
		CyInterface().setShowInterface(InterfaceVisibility.INTERFACE_MINIMAP_ONLY)
		screen.updateMinimapVisibility()
		CyInterface().setShowInterface(iOldMode)


	def clearScreen(self, screen):
		if(self.currentScreen == OUTGOING_WARHEADS):
			screen.minimapClearAllFlashingTiles()
			screen.deleteWidget(OUTGOING_WARHEADS_PANEL)
			screen.deleteWidget(OUTGOING_WARHEADS_INNER_PANEL)
			screen.deleteWidget(OUTGOING_LABEL_PANEL)
			screen.deleteWidget(self.OUTGOING_LABEL_ID)

			screen.deleteWidget(OUTGOING_SUMMARY_PANEL)
			screen.deleteWidget(OUTGOING_SUMMARY_INNER_PANEL)
			screen.deleteWidget(OUTGOING_SUMMARY_LABEL_PANEL)
			screen.deleteWidget(self.OUTGOING_SUMMARY_LABEL_ID)
			self.selectedUnitHash = {}

		elif(self.currentScreen == INCOMING_WARHEADS):
			screen.minimapClearAllFlashingTiles()
			screen.deleteWidget(INCOMING_WARHEADS_PANEL)
			screen.deleteWidget(INCOMING_WARHEADS_INNER_PANEL)
			screen.deleteWidget(INCOMING_LABEL_PANEL)
			screen.deleteWidget(self.INCOMING_LABEL_ID)

			screen.deleteWidget(INCOMING_SUMMARY_PANEL)
			screen.deleteWidget(INCOMING_SUMMARY_INNER_PANEL)
			screen.deleteWidget(INCOMING_SUMMARY_LABEL_PANEL)
			screen.deleteWidget(self.INCOMING_SUMMARY_LABEL_ID)
			self.selectedCityHash = {}


	def drawOutgoingWarheadsContent(self, screen):

		self.H_MAP = (self.W_MAP * CyMap().getGridHeight()) / CyMap().getGridWidth()
		if (self.H_MAP > self.H_MAP_MAX):
			self.W_MAP = (self.H_MAP_MAX * CyMap().getGridWidth()) / CyMap().getGridHeight()
			self.H_MAP = self.H_MAP_MAX

		screen.addPanel(OUTGOING_WARHEADS_PANEL, u"", "", False, False, 24, 60, 350, 708-60, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel(OUTGOING_WARHEADS_INNER_PANEL, u"", "", True, True, 34, 60+10+40, 330, 708-(70+15+35), PanelStyles.PANEL_STYLE_IN)
		screen.addPanel(OUTGOING_LABEL_PANEL, u"", "", False, False, 29, 60+10, 340, 35, PanelStyles.PANEL_STYLE_MAIN)
		screen.setText(self.OUTGOING_LABEL_ID, "Background", self.OUTGOING_LABEL_TEXT, 1<<2, 24+(350/2), 60+10+7, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setHitTest(self.OUTGOING_LABEL_ID,HitTestTypes.HITTEST_NOHIT)

		screen.addPanel(OUTGOING_SUMMARY_PANEL, u"", "", False, False, 420, self.Y_MAP+self.H_MAP+5, 580, self.H_SCREEN-(self.Y_MAP+self.H_MAP+5)-60, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel(OUTGOING_SUMMARY_INNER_PANEL, u"", "", True, False, 425+5, self.Y_MAP+self.H_MAP+15+40, 580-20, self.H_SCREEN-(self.Y_MAP+self.H_MAP+5)-(70+35+15), PanelStyles.PANEL_STYLE_IN)
		screen.addPanel(OUTGOING_SUMMARY_LABEL_PANEL, u"", "", False, False, 425, self.Y_MAP+self.H_MAP+15, 570, 35, PanelStyles.PANEL_STYLE_MAIN)
		screen.setText(self.OUTGOING_SUMMARY_LABEL_ID, "Background", self.OUTGOING_SUMMARY_LABEL_TEXT, 1<<2, 425+(570/2), self.Y_MAP+self.H_MAP+15+7, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setHitTest(self.OUTGOING_SUMMARY_LABEL_ID,HitTestTypes.HITTEST_NOHIT)

		iCounter = 0
		iTotalNukeCount = 0

		for iUnitID in range(GCgetPlayer(GCgetGame().getActivePlayer()).getNumUnits()):
			objUnit = GCgetPlayer(GCgetGame().getActivePlayer()).getUnit(iUnitID)

			self.selectedUnitHash[iUnitID] = False

			if(objUnit.getUnitAIType() != UnitAITypes.UNITAI_ICBM):
				continue

			iTotalNukeCount += 1

			if(objUnit.getMADTargetPlot() == None):
				continue

			if(objUnit.getMADTargetPlot().isNone()):
				continue

			screen.attachPanel(OUTGOING_WARHEADS_INNER_PANEL, "UnitPanel%s" %(iUnitID), "", "", True, False, PanelStyles.PANEL_STYLE_OUT)

			screen.attachPanel("UnitPanel%s" %(iUnitID), "UnitInnerPanel%s" %(iUnitID), "", "", True, False, PanelStyles.PANEL_STYLE_EMPTY)
			screen.attachLabel( "UnitInnerPanel%s" %(iUnitID), "UnitInnerPanel%sfill1" %(iUnitID), "            Located At: %s" %(objUnit.plot().getPlotCity().getName()))
			screen.attachLabel( "UnitInnerPanel%s" %(iUnitID), "UnitInnerPanel%sfill2" %(iUnitID), "            Targetting: %s" %(objUnit.getMADTargetPlot().getPlotCity().getName()))
			screen.attachLabel( "UnitInnerPanel%s" %(iUnitID), "UnitInnerPanel%sfill3" %(iUnitID), "            City Owned By: %s" %(GCgetPlayer(objUnit.getMADTargetPlot().getPlotCity().getOwner()).getName()))

#			screen.attachLabel( "UnitPanel%s" %(iUnitID), "UnitPanel%sfill1" %(iUnitID), "            " )
			screen.addCheckBoxGFCAt( "UnitPanel%s" %(iUnitID), "UnitCheckbox%s" %(iUnitID)
							   , GCgetUnitInfo(objUnit.getUnitType()).getButton()
							   , AFM.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath()
							   , 1, 2, 46, 46, WidgetTypes.WIDGET_GENERAL, iUnitID, iUnitID, ButtonStyles.BUTTON_STYLE_LABEL, False)
			iCounter +=1

		if(iCounter <= 10):
			iTempCounter = 10 - iCounter

			for iDummy in range(iTempCounter):
				screen.attachPanel(OUTGOING_WARHEADS_INNER_PANEL, "DummyPanel%s" %(iDummy), "", "", True, False, PanelStyles.PANEL_STYLE_EMPTY)
				screen.attachLabel( "DummyPanel%s" %(iDummy), "UnitPanel%sfill1" %(iDummy), "            " )
				screen.attachLabel( "DummyPanel%s" %(iDummy), "UnitPanel%sfill2" %(iDummy), "            " )
				screen.attachLabel( "DummyPanel%s" %(iDummy), "UnitPanel%sfill3" %(iDummy), "            " )

		screen.attachLabel( OUTGOING_SUMMARY_INNER_PANEL, "TotalNukes", u"%c<FONT=3b>%s Total Nuclear Warheads</FONT>" %(CyGame().getSymbolID(FontSymbols.BULLET_CHAR), iTotalNukeCount) )
		screen.attachLabel( OUTGOING_SUMMARY_INNER_PANEL, "TotalOutgoingNukes", u"%c<FONT=3b>%s Outgoing Nuclear Warheads</FONT>" %(CyGame().getSymbolID(FontSymbols.BULLET_CHAR), iCounter) )
		screen.attachLabel( OUTGOING_SUMMARY_INNER_PANEL, "TotalNukesAvailable", u"%c<FONT=3b>%s Available Nuclear Warheads</FONT>" %(CyGame().getSymbolID(FontSymbols.BULLET_CHAR), iTotalNukeCount-iCounter) )


	def drawIncomingWarheadsContent(self, screen):

		self.H_MAP = (self.W_MAP * CyMap().getGridHeight()) / CyMap().getGridWidth()
		if (self.H_MAP > self.H_MAP_MAX):
			self.W_MAP = (self.H_MAP_MAX * CyMap().getGridWidth()) / CyMap().getGridHeight()
			self.H_MAP = self.H_MAP_MAX

		screen.addPanel(INCOMING_WARHEADS_PANEL, u"", "", False, False, 24, 60, 350, 708-60, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel(INCOMING_WARHEADS_INNER_PANEL, u"", "", True, True, 34, 60+10+40, 330, 708-(70+15+35), PanelStyles.PANEL_STYLE_IN)
		screen.addPanel(INCOMING_LABEL_PANEL, u"", "", False, False, 29, 60+10, 340, 35, PanelStyles.PANEL_STYLE_MAIN)
		screen.setText(self.INCOMING_LABEL_ID, "Background", self.INCOMING_LABEL_TEXT, 1<<2, 24+(350/2), 60+10+7, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setHitTest(self.INCOMING_LABEL_ID,HitTestTypes.HITTEST_NOHIT)

		screen.addPanel(INCOMING_SUMMARY_PANEL, u"", "", False, False, 420, self.Y_MAP+self.H_MAP+5, 580, self.H_SCREEN-(self.Y_MAP+self.H_MAP+5)-60, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel(INCOMING_SUMMARY_INNER_PANEL, u"", "", True, False, 425+5, self.Y_MAP+self.H_MAP+15+40, 580-20, self.H_SCREEN-(self.Y_MAP+self.H_MAP+5)-(70+35+15), PanelStyles.PANEL_STYLE_IN)
		screen.addPanel(INCOMING_SUMMARY_LABEL_PANEL, u"", "", False, False, 425, self.Y_MAP+self.H_MAP+15, 570, 35, PanelStyles.PANEL_STYLE_MAIN)
		screen.setText(self.INCOMING_SUMMARY_LABEL_ID, "Background", self.INCOMING_SUMMARY_LABEL_TEXT, 1<<2, 425+(570/2), self.Y_MAP+self.H_MAP+15+7, self.Z_CONTROLS, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setHitTest(self.INCOMING_SUMMARY_LABEL_ID,HitTestTypes.HITTEST_NOHIT)

		iCounter = 0
		iTotalCityCount = 0
		iTotalThreatCount = 0
		iActivePlayer = GCgetGame().getActivePlayer()

		for iCityID in range(GCgetPlayer(iActivePlayer).getNumCities()):
			objCity = GCgetPlayer(iActivePlayer).getCity(iCityID)

			self.selectedCityHash[iCityID] = False

			iTotalCityCount += 1
			if(objCity.getMADIncoming() == 0):
				continue

			screen.attachPanel(INCOMING_WARHEADS_INNER_PANEL, "CityPanel%s" %(iCityID), "", "", True, False, PanelStyles.PANEL_STYLE_OUT)

			screen.attachPanel("CityPanel%s" %(iCityID), "CityInnerPanel%s" %(iCityID), "", "", True, False, PanelStyles.PANEL_STYLE_EMPTY)
			screen.attachLabel( "CityInnerPanel%s" %(iCityID), "CityInnerPanel%sfill1" %(iCityID), "            Name: %s" %(objCity.getName()))
			screen.attachLabel( "CityInnerPanel%s" %(iCityID), "CityInnerPanel%sfill2" %(iCityID), "            Threat Count: %s" %(objCity.getMADIncoming()))
			screen.attachLabel( "CityInnerPanel%s" %(iCityID), "CityInnerPanel%sfill3" %(iCityID), "            ")

#			screen.attachLabel( "CityPanel%s" %(iCityID), "CityPanel%sfill1" %(iCityID), "            " )
			screen.addCheckBoxGFCAt( "CityPanel%s" %(iCityID), "CityCheckbox%s" %(iCityID)
							   , GCgetMissionInfo(GCgetInfoTypeForString("MISSION_PRETARGET_NUKE")).getButton()
							   , AFM.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath()
							   , 1, 2, 46, 46, WidgetTypes.WIDGET_GENERAL, iCityID, iCityID, ButtonStyles.BUTTON_STYLE_LABEL, False)
			iCounter +=1
			iTotalThreatCount += objCity.getMADIncoming()

		foo = 3
		if(iCounter <= 10):
			iTempCounter = 10 - iCounter

			for iDummy in range(iTempCounter):
				screen.attachPanel(INCOMING_WARHEADS_INNER_PANEL, "DummyPanel%s" %(iDummy), "", "", True, False, PanelStyles.PANEL_STYLE_EMPTY)
				screen.attachLabel( "DummyPanel%s" %(iDummy), "CityPanel%sfill1" %(iDummy), "            " )
				screen.attachLabel( "DummyPanel%s" %(iDummy), "CityPanel%sfill2" %(iDummy), "            " )
				screen.attachLabel( "DummyPanel%s" %(iDummy), "CityPanel%sfill3" %(iDummy), "            " )

		screen.attachLabel( INCOMING_SUMMARY_INNER_PANEL, "TotalCities", u"%c<FONT=3b>%s Cities</FONT>" %(CyGame().getSymbolID(FontSymbols.BULLET_CHAR), iTotalCityCount) )
		screen.attachLabel( INCOMING_SUMMARY_INNER_PANEL, "TotalThreatenedCities", u"%c<FONT=3b>%s Threatened Cities</FONT>" %(CyGame().getSymbolID(FontSymbols.BULLET_CHAR), iCounter) )
		screen.attachLabel( INCOMING_SUMMARY_INNER_PANEL, "TotalThreatCount", u"%c<FONT=3b>%s Total Threat Count</FONT>" %(CyGame().getSymbolID(FontSymbols.BULLET_CHAR), iTotalThreatCount) )
		screen.attachLabel( INCOMING_SUMMARY_INNER_PANEL, "TotalSafeCities", u"%c<FONT=3b>%s Untargeted Cities</FONT>" %(CyGame().getSymbolID(FontSymbols.BULLET_CHAR), iTotalCityCount-iCounter) )



	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def updateMinimap(self):

		# Get the instance of the screen
		screen = self.getScreen()

		screen.minimapClearAllFlashingTiles()

		objPlayer = GCgetPlayer(GCgetGame().getActivePlayer())
		iPlayer = GCgetGame().getActivePlayer()

		iTargetColor = GCgetInfoTypeForString("COLOR_RED")

		for iUnitID in self.selectedUnitHash.keys():

			if(not self.selectedUnitHash[iUnitID]):
				continue

			if(not screen.getCheckBoxState("UnitCheckbox%s" %(iUnitID))):
				self.selectedUnitHash[iUnitID] = False
				continue

			objUnit = objPlayer.getUnit(iUnitID)
			iColor = GCgetPlayerColorInfo(GCgetPlayer(iPlayer).getPlayerColor()).getColorTypePrimary()

			#screen.setMinimapColor(MinimapModeTypes.MINIMAPMODE_MILITARY, objUnit.getX(), objUnit.getY(), iColor, 0.6)

			screen.minimapFlashPlot(objUnit.getX(), objUnit.getY(), iColor, -1)
			screen.minimapFlashPlot(objUnit.getMADTargetPlot().getX(), objUnit.getMADTargetPlot().getY(), iTargetColor, -1)


		for iCityID in self.selectedCityHash.keys():

			if(not self.selectedCityHash[iCityID]):
				continue

			if(not screen.getCheckBoxState("CityCheckbox%s" %(iCityID))):
				self.selectedCityHash[iCityID] = False
				continue

			objCity = objPlayer.getCity(iCityID)
			iColor = GCgetPlayerColorInfo(GCgetPlayer(iPlayer).getPlayerColor()).getColorTypePrimary()

			screen.minimapFlashPlot(objCity.getX(), objCity.getY(), iColor, -1)


	def update(self, fDelta):
		screen = self.getScreen()


	# handle the input for this screen...
	def handleInput (self, inputClass):

		# Get the instance of the screen
		screen = self.getScreen()


		if(inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			if(inputClass.getFunctionName() == "UnitCheckbox"):

				#screen.setText( "TopPanelDebugMsg", "TopPanel", inputClass.getFunctionName()+"%s %s %s" %(inputClass.getData1(),(inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED), screen.getCheckBoxState(inputClass.getFunctionName()+"%s" %(inputClass.getData1())))
				#			, 1<<1, 1010, 20, -10, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )


				self.selectedUnitHash[inputClass.getData1()] = screen.getCheckBoxState(inputClass.getFunctionName()+"%s" %(inputClass.getData1()))
				self.updateMinimap()

			if(inputClass.getFunctionName() == "CityCheckbox"):

				#screen.setText( "TopPanelDebugMsg", "TopPanel", inputClass.getFunctionName()+"%s %s %s" %(inputClass.getData1(),(inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED), screen.getCheckBoxState(inputClass.getFunctionName()+"%s" %(inputClass.getData1())))
				#			, 1<<1, 1010, 20, -10, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )


				self.selectedCityHash[inputClass.getData1()] = screen.getCheckBoxState(inputClass.getFunctionName()+"%s" %(inputClass.getData1()))
				self.updateMinimap()

			if((inputClass.getFunctionName() == self.OUTGOING_ID  or inputClass.getFunctionName() == "") and self.currentScreen != OUTGOING_WARHEADS):
				self.clearScreen(screen)
				self.currentScreen = OUTGOING_WARHEADS
				self.drawOutgoingWarheadsContent(screen)
				return 0

			if((inputClass.getFunctionName() == self.INCOMING_ID or inputClass.getFunctionName() == "") and self.currentScreen != INCOMING_WARHEADS):
				self.clearScreen(screen)
				self.currentScreen = INCOMING_WARHEADS
				self.drawIncomingWarheadsContent(screen)
				return 0

			if(inputClass.getFunctionName() == self.EXIT_ID):
				self.currentScreen = OUTGOING_WARHEADS

		return 0


	# Adds a 100 x 100 panel for each panel style available
	def showDebugScreen(self):

		screen = self.getScreen()
		screen.addPanel("BACKGROUND", "", "", False, False, 0, 0, screen.getXResolution(), screen.getYResolution(), PanelStyles.PANEL_STYLE_SOLID)

		screen.setPanelColor("BACKGROUND",255,255,255)

		iRow = 5
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_STANDARD", False, False, iRow, 5, 100, 100, PanelStyles.PANEL_STYLE_STANDARD)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_SOLID", False, False, 110, iRow, 100, 100, PanelStyles.PANEL_STYLE_SOLID)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_EMPTY", False, False, 215, iRow, 100, 100, PanelStyles.PANEL_STYLE_EMPTY)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_FLAT", False, False, 320, iRow, 100, 100, PanelStyles.PANEL_STYLE_FLAT)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_IN", False, False, 425, iRow, 100, 100, PanelStyles.PANEL_STYLE_IN)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_OUT", False, False, 530, iRow, 100, 100, PanelStyles.PANEL_STYLE_OUT)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_EXTERNAL", False, False, 635, iRow, 100, 100, PanelStyles.PANEL_STYLE_EXTERNAL)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_DEFAULT", False, False, 740, iRow, 100, 100, PanelStyles.PANEL_STYLE_DEFAULT)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CIVILPEDIA", False, False, 845, iRow, 100, 100, PanelStyles.PANEL_STYLE_CIVILPEDIA)

		iRow += 105
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_STONE", False, False, 5, iRow, 100, 100, PanelStyles.PANEL_STYLE_STONE)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_BLUELARGE", False, False, iRow, 110, 100, 100, PanelStyles.PANEL_STYLE_BLUELARGE)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_UNITSTAT", False, False, 215, iRow, 100, 100, PanelStyles.PANEL_STYLE_UNITSTAT)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_BLUE50", False, False, 320, iRow, 100, 100, PanelStyles.PANEL_STYLE_BLUE50)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_TOPBAR", False, False, 425, iRow, 100, 100, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_BOTTOMBAR", False, False, 530, iRow, 100, 100, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_TECH", False, False, 635, iRow, 100, 100, PanelStyles.PANEL_STYLE_TECH)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_GAMEHUD_LEFT", False, False, 740, iRow, 100, 100, PanelStyles.PANEL_STYLE_GAMEHUD_LEFT)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_GAMEHUD_RIGHT", False, False, 845, iRow, 100, 100, PanelStyles.PANEL_STYLE_GAMEHUD_RIGHT)

		iRow += 105
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_GAMEHUD_CENTER", False, False, 5, iRow, 100, 100, PanelStyles.PANEL_STYLE_GAMEHUD_CENTER)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_GAMEHUD_STATS", False, False, 110, iRow, 100, 100, PanelStyles.PANEL_STYLE_GAMEHUD_STATS)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_GAME_MAP", False, False, iRow, 215, 100, 100, PanelStyles.PANEL_STYLE_GAME_MAP)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_GAME_TOPBAR", False, False, 320, iRow, 100, 100, PanelStyles.PANEL_STYLE_GAME_TOPBAR)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_HUD_HELP", False, False, 425, iRow, 100, 100, PanelStyles.PANEL_STYLE_HUD_HELP)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_LEFT", False, False, 530, iRow, 100, 100, PanelStyles.PANEL_STYLE_CITY_LEFT)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_RIGHT", False, False, 635, iRow, 100, 100, PanelStyles.PANEL_STYLE_CITY_RIGHT)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_TOP", False, False, 740, iRow, 100, 100, PanelStyles.PANEL_STYLE_CITY_TOP)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_TANSHADE", False, False, 845, iRow, 100, 100, PanelStyles.PANEL_STYLE_CITY_TANSHADE)

		iRow += 105
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_INFO", False, False, 5, iRow, 100, 100, PanelStyles.PANEL_STYLE_CITY_INFO)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_TANTL", False, False, 110, iRow, 100, 100, PanelStyles.PANEL_STYLE_CITY_TANTL)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_TANTR", False, False, 215, iRow, 100, 100, PanelStyles.PANEL_STYLE_CITY_TANTR)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_COLUMNL", False, False, iRow, 320, 100, 100, PanelStyles.PANEL_STYLE_CITY_COLUMNL)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_COLUMNC", False, False, 425, iRow, 100, 100, PanelStyles.PANEL_STYLE_CITY_COLUMNC)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_COLUMNR", False, False, 530, iRow, 100, 100, PanelStyles.PANEL_STYLE_CITY_COLUMNR)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_CITY_TITLE", False, False, 635, iRow, 100, 100, PanelStyles.PANEL_STYLE_CITY_TITLE)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_DAWN", False, False, 740, iRow, 100, 100, PanelStyles.PANEL_STYLE_DAWN)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_DAWNTOP", False, False, 845, iRow, 100, 100, PanelStyles.PANEL_STYLE_DAWNTOP)

		iRow += 105
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_DAWNBOTTOM", False, False, 5, iRow, 100, 100, PanelStyles.PANEL_STYLE_DAWNBOTTOM)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN", False, False, 110, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_BLACK25", False, False, 215, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN_BLACK25)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_BLACK50", False, False, iRow, 320, 100, 100, PanelStyles.PANEL_STYLE_MAIN_BLACK50)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_WHITE", False, False, 425, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN_WHITE)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_WHITETAB", False, False, 530, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN_WHITETAB)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_TAN", False, False, 635, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN_TAN)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_TAN15", False, False, 740, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN_TAN15)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_TANL", False, False, 845, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN_TANL)

		iRow += 105
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_TANR", False, False, 5, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN_TANR)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_TANT", False, False, 110, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN_TANT)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_TANB", False, False, 215, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN_TANB)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_BOTTOMBAR", False, False, iRow, 320, 100, 100, PanelStyles.PANEL_STYLE_MAIN_BOTTOMBAR)
		screen.addPanel(self.getNextWidgetName(), "", "PANEL_STYLE_MAIN_SELECT", False, False, 425, iRow, 100, 100, PanelStyles.PANEL_STYLE_MAIN_SELECT)
