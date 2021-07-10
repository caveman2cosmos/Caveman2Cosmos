## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvScreenEnums

# globals
GC = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvCorporationScreen:
	"Corporation Advisor Screen"

	def __init__(self):

		self.BUTTON_NAME = "CorporationScreenButton"
		self.CORPORATION_NAME = "CorporationText"
		self.DEBUG_DROPDOWN_ID =  "CorporationDropdownWidget"
		self.AREA1_ID =  "CorporationAreaWidget1"
		self.AREA2_ID =  "CorporationAreaWidget2"
		self.EXIT_NAME = "CorporationExitButton"

		self.BORDER_WIDTH = 2
		self.BUTTON_SIZE = 48
		self.BIG_BUTTON_SIZE = 64
		self.HIGHLIGHT_EXTRA_SIZE = 4

		self.HEADINGS_TOP = 0
		self.HEADINGS_BOTTOM = 220
		self.HEADINGS_LEFT = 0
		self.HEADINGS_RIGHT = 320
		self.HELP_TOP = 20
		self.HELP_BOTTOM = 610
		self.HELP_LEFT = 350
		self.HELP_RIGHT = 950
		self.Z_SCREEN = -6.1
		self.Y_TITLE = 8
		self.Z_TEXT = self.Z_SCREEN - 0.2
		self.DZ = -0.2
		self.Z_CONTROLS = self.Z_TEXT
		self.TEXT_MARGIN = 13

	def setValues(self):
		resolutionWidth = 1024
		resolutionHeigth = 768

		self.W_SCREEN = resolutionWidth
		self.H_SCREEN = resolutionHeigth
		self.X_SCREEN = self.W_SCREEN / 2
		self.Y_SCREEN = 396

		self.X_POSITION = 0
		self.Y_POSITION = 0
		self.INITIAL_SPACING = 30
		self.HEADINGS_WIDTH = 340
		self.HEADINGS_HEIGHT = 64
		self.PANEL_HEIGHT = 55
		self.PANEL_WIDTH = 0

		self.X_EXIT = self.W_SCREEN - 30
		self.Y_EXIT = self.H_SCREEN - 40

		self.LEFT_EDGE_TEXT = 10
		self.X_CORPORATION_START = 155
		self.Y_CORPORATION_START = 155
		self.DX_CORPORATION = 116
		self.DY_CORPORATION = 116
		self.X_CORPORATION = 0
		self.Y_CORPORATION = 35
		self.X_CORPORATION_DIFF = self.X_CORPORATION_START - self.X_CORPORATION
		self.Y_GREAT_PERSON = 73
		self.Y_BONUSES = 90
		self.Y_FOUNDED = 118
		self.Y_HEADQUARTERS = 142
		self.Y_CORPORATION_NAME = 58

		self.X_CORPORATION_AREA = 0
		self.Y_CORPORATION_AREA = 0
		self.W_CORPORATION_AREA = 1008 + self.BUTTON_SIZE
		self.H_CORPORATION_AREA = 180

		self.X_CITY1_AREA = 45
		self.X_CITY2_AREA = 522
		self.Y_CITY_AREA = 250
		self.W_CITY_AREA = 457
		self.H_CITY_AREA = 460

		self.X_CITY = 10
		self.DY_CITY = 38

		self.iCorporationExamined = -1
		self.iCorporationSelected = -1
		self.iCorporationOriginal = -1
		self.iActivePlayer = -1

		self.bScreenUp = False

		self.CorporationScreenInputMap = {
			self.CORPORATION_NAME	: self.CorporationScreenButton,
			self.EXIT_NAME			: self.Exit,
			self.BUTTON_NAME		: self.CorporationScreenButton
		}

	def getScreen(self):
		return CyGInterfaceScreen("CorporationScreen", CvScreenEnums.CORPORATION_SCREEN)

	def interfaceScreen (self):
		# johny smith ScreenTweaks LINE:
		self.setValues()
		screen = self.getScreen()
		if screen.isActive():
			return
		screen.setRenderInterfaceOnly(True)
		screen.showScreen( PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.setDimensions(self.X_POSITION, self.Y_POSITION, self.W_SCREEN, self.H_SCREEN)
		screen.addDDSGFC("CorporationBG", ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		## Panels on the Top(name of screen) and bottom(Cancel, Exit, Revolution buttons)
		screen.addPanel("CorporationsTopPanel", "", "", True, False, 0, 0, self.W_SCREEN, self.PANEL_HEIGHT, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("CorporationsBottomPanel", "", "", True, False, 0, self.H_SCREEN - self.PANEL_HEIGHT, self.W_SCREEN, self.PANEL_HEIGHT, PanelStyles.PANEL_STYLE_BOTTOMBAR)

		# Set the background and exit button, and show the screen
		screen.setDimensions(screen.centerX(0), screen.centerY(0), self.W_SCREEN, self.H_SCREEN)

		self.SCREEN_ART = ArtFileMgr.getInterfaceArtInfo("TECH_BG").getPath()
		self.EXIT_TEXT = "<font=4>" + localText.getText("TXT_WORD_EXIT", ()).upper()

		self.iActivePlayer = GC.getGame().getActivePlayer()

		self.bScreenUp = True

		screen.showWindowBackground(False)

		# Make the scrollable areas for the city list...
		import DebugUtils
		if DebugUtils.isAnyDebugMode():
			self.szDropdownName = self.DEBUG_DROPDOWN_ID
			screen.addDropDownBoxGFC(self.szDropdownName, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for j in range(GC.getMAX_PLAYERS()):
				if (GC.getPlayer(j).isAlive()):
					screen.addPullDownString(self.szDropdownName, GC.getPlayer(j).getName(), j, j, False )

		# Make the scrollable area for the civics list...
		screen.addScrollPanel( "CivicList", u"", self.PANEL_WIDTH/8 * 7, self.PANEL_HEIGHT/8 * 7, self.W_SCREEN, self.Y_CORPORATION_AREA + self.H_CORPORATION_AREA + 5, PanelStyles.PANEL_STYLE_EXTERNAL )
		screen.setActivation( "CivicList", ActivationTypes.ACTIVATE_NORMAL )

		# Draw Corporation info
		self.drawCorporationInfo()

		self.drawHelpInfo()

		self.drawCityInfo(self.iCorporationSelected)

	# Draws the Corporation buttons and information
	def drawCorporationInfo(self):

		for i in range(GC.getNumCorporationInfos()):
			if (GC.getGame().canEverSpread(i)):
				screen = self.getScreen()

				## johny smith
				## This draws the panel
				## Puts the panel in a loop
				## Attachs the panel so it will scroll
				xLoop = self.X_CORPORATION_START
				for i in range(GC.getNumCorporationInfos()):
					if (GC.getGame().canEverSpread(i)):
						screen.attachPanelAt("CivicList", "CorporationsScreenArea", "", "", True, True, PanelStyles.PANEL_STYLE_MAIN, self.X_CORPORATION, self.Y_CORPORATION_AREA, self.X_CORPORATION_DIFF + ((i + 1) * self.DX_CORPORATION), self.H_CORPORATION_AREA, WidgetTypes.WIDGET_GENERAL, i, -1 )
						xLoop += self.DX_CORPORATION


				## johny smith
				## This draws the symbols
				## Puts the symbols in a loop
				## Attachs the symbols so they will scroll
				xLoop = self.X_CORPORATION_START
				for i in range(GC.getNumCorporationInfos()):
					if (GC.getGame().canEverSpread(i)):
						screen.addCheckBoxGFCAt("CivicList", self.getCorporationButtonName(i), GC.getCorporationInfo(i).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), self.X_CORPORATION_AREA + xLoop - 25, self.Y_CORPORATION_AREA + 10, self.BUTTON_SIZE, self.BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL, False)
						screen.setActivation( self.getCorporationButtonName(i), ActivationTypes.ACTIVATE_NORMAL )
						xLoop += self.DX_CORPORATION


			self.iCorporationSelected = -1
			self.iCorporationExamined = self.iCorporationSelected
			self.iCorporationOriginal = self.iCorporationSelected


	def drawHelpInfo(self):

		screen = self.getScreen()

		## johny smith
		## This attaches the text to the panel
		## This is for every line of font
		# Great Person
		xLoop = self.X_CORPORATION_START
		for i in range(GC.getNumCorporationInfos()):
			if (GC.getGame().canEverSpread(i)):
				szGreatPerson = ""
				for iBuilding in range(GC.getNumBuildingInfos()):
					if (GC.getBuildingInfo(iBuilding).getFoundsCorporation() == i):
						break
				for iUnit in range(GC.getNumUnitInfos()):
					if GC.getUnitInfo(iUnit).getHasBuilding(iBuilding):
						szGreatPerson = GC.getUnitInfo(iUnit).getDescription()
						break
				screen.setLabelAt("", "CivicList", szGreatPerson, 1<<2, xLoop, self.Y_GREAT_PERSON, self.DZ, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

				xLoop += self.DX_CORPORATION

		# Bonuses
		xLoop = self.X_CORPORATION_START
		for i in range(GC.getNumCorporationInfos()):
			if (GC.getGame().canEverSpread(i)):
				szListLabels = []
				iNum = 0
				szList = u""
				for eBonus in GC.getCorporationInfo(i).getPrereqBonuses():
					if iNum == 0:
						szList = u""
					else:
						szList += u", "
					iNum += 1
					szList += u"%c" % (GC.getBonusInfo(eBonus).getChar(), )

					if iNum > 3:
						iNum = 0
						szListLabels.append(szList)
						szList = u""

				if len(szList) > 0:
					szListLabels.append(szList)

				iRow = 0
				for szList in szListLabels:
					screen.setLabelAt("", "CivicList", szList, 1<<2, xLoop, self.Y_BONUSES + iRow, self.DZ, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
					iRow += 16

				xLoop += self.DX_CORPORATION

		# Founded...
		screen.setLabelAt("", "CivicList", localText.getText("TXT_KEY_RELIGION_SCREEN_DATE_FOUNDED", ()), 1<<0, self.LEFT_EDGE_TEXT, self.Y_FOUNDED, self.DZ, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Date Founded:
		xLoop = self.X_CORPORATION_START
		for i in range(GC.getNumCorporationInfos()):
			if (GC.getGame().canEverSpread(i)):
				if (GC.getGame().getCorporationGameTurnFounded(i) < 0):
					szFounded = localText.getText("TXT_KEY_RELIGION_SCREEN_NOT_FOUNDED", ())
				else:
					szFounded = CyGameTextMgr().getTimeStr(GC.getGame().getCorporationGameTurnFounded(i), False)
				screen.setLabelAt("", "CivicList", szFounded, 1<<2, xLoop, self.Y_FOUNDED, self.DZ, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

				xLoop += self.DX_CORPORATION

		# Headquarters
		screen.setLabelAt("", "CivicList", localText.getText("TXT_KEY_CORPORATION_SCREEN_HEADQUARTERS", ()), 1<<0, self.LEFT_EDGE_TEXT, self.Y_HEADQUARTERS, self.DZ, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		xLoop = self.X_CORPORATION_START
		for i in range(GC.getNumCorporationInfos()):
			if (GC.getGame().canEverSpread(i)):
				pHeadquarters = GC.getGame().getHeadquarters(i)
				if pHeadquarters is None:
					szFounded = u"-"
					screen.setLabelAt("", "CivicList", szFounded, 1<<2, xLoop, self.Y_HEADQUARTERS, self.DZ, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				elif not pHeadquarters.isRevealed(GC.getPlayer(self.iActivePlayer).getTeam(), False):
					szFounded = localText.getText("TXT_KEY_UNKNOWN", ())
					screen.setLabelAt("", "CivicList", szFounded, 1<<2, xLoop, self.Y_HEADQUARTERS, self.DZ, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				else:
					szFounded = pHeadquarters.getName()
					screen.setLabelAt("", "CivicList", "(%s)" % GC.getPlayer(pHeadquarters.getOwner()).getCivilizationAdjective(0), 1<<2, xLoop, self.Y_HEADQUARTERS+8, self.DZ, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
					screen.setLabelAt("", "CivicList", szFounded, 1<<2, xLoop, self.Y_HEADQUARTERS-8, self.DZ, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				xLoop += self.DX_CORPORATION

		self.iCorporationSelected = -1
		self.iCorporationExamined = self.iCorporationSelected
		self.iCorporationOriginal = self.iCorporationSelected

	# Draws the city list
	def drawCityInfo(self, iCorporation):

		if (not self.bScreenUp):
			return

		screen = self.getScreen()

		if (iCorporation == GC.getNumCorporationInfos()):
			iLinkCorporation = -1
		else:
			iLinkCorporation = iCorporation

		screen.addPanel(self.AREA1_ID, "", "", True, True, self.X_CITY1_AREA, self.Y_CITY_AREA, self.W_CITY_AREA, self.H_CITY_AREA, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel(self.AREA2_ID, "", "", True, True, self.X_CITY2_AREA, self.Y_CITY_AREA, self.W_CITY_AREA, self.H_CITY_AREA, PanelStyles.PANEL_STYLE_MAIN)

		for i in range(GC.getNumCorporationInfos()):
			if (self.iCorporationSelected == i):
				screen.setState(self.getCorporationButtonName(i), True)
			else:
				screen.setState(self.getCorporationButtonName(i), False)

		# Loop through the cities
		szLeftCities = ""
		szRightCities = ""
		for i, cityX in enumerate(GC.getPlayer(self.iActivePlayer).cities()):

			bFirstColumn = (i % 2 == 0)

			# Constructing the City name...
			szCityName = ""
			if cityX.isCapital():
				szCityName += u"%c" % CyGame().getSymbolID(FontSymbols.STAR_CHAR)

			lCorporations = []
			for iI in range(GC.getNumCorporationInfos()):
				if cityX.isHasCorporation(iI):
					lCorporations.append(iI)
					if cityX.isHeadquartersByType(iI):
						szCityName += u"%c" % GC.getCorporationInfo(iI).getHeadquarterChar()
					else: szCityName += u"%c" % GC.getCorporationInfo(iI).getChar()

			szCityName += cityX.getName()[0:17] + "  "

			if (iLinkCorporation == -1):
				bFirst = True
				for iI in lCorporations:
					szTempBuffer = CyGameTextMgr().getCorporationHelpCity(iI, cityX, False, False)
					if szTempBuffer:
						if not bFirst:
							szCityName += ", "
						szCityName += szTempBuffer
						bFirst = False

			else: szCityName += CyGameTextMgr().getCorporationHelpCity(iLinkCorporation, cityX, False, True)

			if bFirstColumn:
				szLeftCities += "<font=3>" + szCityName + "\n"
			else: szRightCities += "<font=3>" + szCityName + "\n"

		screen.addMultilineText("Child" + self.AREA1_ID, szLeftCities, self.X_CITY1_AREA+5, self.Y_CITY_AREA+5, self.W_CITY_AREA-10, self.H_CITY_AREA-10, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)
		screen.addMultilineText("Child" + self.AREA2_ID, szRightCities, self.X_CITY2_AREA+5, self.Y_CITY_AREA+5, self.W_CITY_AREA-10, self.H_CITY_AREA-10, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		# Header...
		if self.iCorporationExamined != -1:
			screen.setLabel("CorporationScreenHeader", "", "<font=4b>" + GC.getCorporationInfo(self.iCorporationExamined).getDescription().upper(), 1<<2, self.X_SCREEN, self.Y_TITLE, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		else: screen.setLabel("CorporationScreenHeader", "", "<font=4b>" + localText.getText("TXT_KEY_CORPORATION_SCREEN_TITLE", ()).upper(), 1<<2, self.X_SCREEN, self.Y_TITLE, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.setText(self.EXIT_NAME, "", self.EXIT_TEXT, 1<<1, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

	def Exit(self, inputClass):
		screen = self.getScreen()
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			screen.hideScreen()

	def getCorporationButtonName(self, iCorporation):
		szName = self.BUTTON_NAME + str(iCorporation)
		return szName

	def getCorporationTextName(self, iCorporation):
		szName = self.CORPORATION_NAME + str(iCorporation)
		return szName

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			screen = self.getScreen()
			iIndex = screen.getSelectedPullDownID(self.DEBUG_DROPDOWN_ID)
			self.iActivePlayer = screen.getPullDownData(self.DEBUG_DROPDOWN_ID, iIndex)
			self.drawCorporationInfo()
			self.drawCityInfo(self.iCorporationSelected)
			return 1
		elif (self.CorporationScreenInputMap.has_key(inputClass.getFunctionName())):
			'Calls function mapped in CorporationScreenInputMap'
			# only get from the map if it has the key

			# get bound function from map and call it
			self.CorporationScreenInputMap.get(inputClass.getFunctionName())(inputClass)
			return 1
		return 0

	def update(self, fDelta):
		return

	# Corporation Button
	def CorporationScreenButton( self, inputClass ):
		if ( inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED ) :
			if self.iCorporationSelected == inputClass.getID():
				self.iCorporationSelected = -1
			else:
				self.iCorporationSelected = inputClass.getID()
			self.iCorporationExamined = self.iCorporationSelected
			self.drawCityInfo(self.iCorporationSelected)
		elif ( inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_ON ) :
			self.iCorporationExamined = inputClass.getID()
			self.drawCityInfo(self.iCorporationExamined)
		elif ( inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_OFF ) :
			self.iCorporationExamined = self.iCorporationSelected
			self.drawCityInfo(self.iCorporationSelected)
		return 0

