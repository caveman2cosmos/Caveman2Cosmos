from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvScreensInterface
import PlatyOptions

## To Do List

    # Techs going over one vertical page on some screen resolutions

		# Thunderbrd you are having this problem, at what resolution was it?

		# I did not have any time to work on C2C while away but I did go through and come up with the three main formulae for the spacing. I need to do the one that checks for bad rounding when finalizing the Y positions of the boxes.

    # get current screen options remembered between times you enter the screen, stored in the BUG options and available on the BUG options screen

    # see if we can use Hydro's colour scheme at least for the bit of the Tech Tree he defined it for

    # Religion tech buttons displayed correctly (although this may not be necessary if the next is done)

    # Figure out why Platyping's World Tech is not working with Koshling's speed improvements for the real PythonCallBacks. The dll code almost looked like it was deliberately to make the Python look slow:lol:. World Techs can only be studied by one player/team then they become unavailable to everyone else.

    # make the buttons for required buildings stand out a bit
		# idea - maybe split the bar that holds them

    # integrate Platyping's Resources affect Tech learning mod. Having a tech changes how much research you provide towards the tech
        # not sure the mod displays correctly everywhere as it was a proof of concept mod not a final release
            # eg adjusts the :science: number
            # shows on the tech tree or in the pedia
        # adjustments need to be stored in XML rather than Python, but we have the technology to do that with Custom XML. Eventually it would be better to have tags.

##



# globals
gc = CyGlobalContext()
#C2C Start - we don't use the backgrounds that come with Platy_UI because we have more Civilizations
ArtFileMgr = CyArtFileMgr()
#C2C End

## Adjustable Values ##
TEXTURE_SIZE = 24		## Icon Size of Items in Tech Panel
MIN_DISPLAY_PANEL = 3 # 2		## Min Number of Tech Panels to Display on Screen
BOX_Y_SPACING = -10		## Min Vertical Panel Spacing

## Non Adjustable Values ##
X_START = 6
X_INCREMENT = TEXTURE_SIZE + X_START /2
Y_ROW = 32

PIXEL_INCREMENT = 7
BOX_WIDTH = 0
BOX_HEIGHT = TEXTURE_SIZE * 3
MAX_ITEMS = 999
BOX_X_SPACING = X_INCREMENT * 3

CIV_HAS_TECH = 0
CIV_IS_RESEARCHING = 1
CIV_NO_RESEARCH = 2
CIV_TECH_AVAILABLE = 3


class CvTechChooser:
	def __init__(self):
		self.nWidgetCount = 0
		self.iCivSelected = 0
		self.aiCurrentState = []

		# Advanced Start
		self.m_iSelectedTech = -1
		self.m_bSelectedTechDirty = false
		self.m_bTechRecordsDirty = false
## Ultrapack ##
		self.iCivilization = -1
		self.bResearched = False
		self.bDisabled = False
		self.iFromEra = 0
		self.iHideEra = 999999
		self.iMinX = 9999999
		self.GreatPeople = []
		self.TechBenefits = {}
		self.Advisors = ["[ICON_STRENGTH]", "[ICON_RELIGION]", "[ICON_GOLD]", "[ICON_RESEARCH]", "[ICON_CULTURE]", "[ICON_FOOD]"]
## Ultrapack ##
## C2C Colours and store prefs ##
		self.iColourOpt = 2             # Colour Scheme used
		self.EraColor = []              # Tech Era colour
		self.iCanNotResearchColour = [206, 65, 69]  # Can not research colour
		self.iHasTechBoxColour = [150, 150, 50]     # colour of tech box if it has been researched
		self.iIsResearchingColour = [100, 140, 220] # colour of tech box if it is being researched

	def interfaceScreen(self):
		if CyGame().isPitbossHost(): return
		screen = CyGInterfaceScreen( "TechChooser", CvScreenEnums.TECH_CHOOSER )
		screen.setRenderInterfaceOnly(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		self.iCivSelected = CyGame().getActivePlayer()
		pPlayer = gc.getPlayer(self.iCivSelected)
		pTeam = gc.getTeam(pPlayer.getTeam())
# C2C Start - no unique backgrounds
		screen.addDDSGFC("ScreenBackground", ArtFileMgr.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 51, screen.getXResolution(), screen.getYResolution(), WidgetTypes.WIDGET_GENERAL, -1, -1 )
# C2C End
		screen.moveToBack("ScreenBackground")
		screen.hide("AddTechButton")
		screen.hide("ASPointsLabel")
		screen.hide("SelectedTechLabel")
		screen.hide("CivDropDown")
		screen.showWindowBackground(False)
		screen.addPanel("TechTopPanel", u"", u"", True, False, 0, 0, screen.getXResolution(), 55, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("TechBottomPanel", u"", u"", True, False, 0, screen.getYResolution() - 55, screen.getXResolution(), 55, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.setText( "TechChooserExit", "Background", "<font=4>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, screen.getXResolution() - 30, screen.getYResolution() - 42, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )
		screen.setActivation( "TechChooserExit", ActivationTypes.ACTIVATE_MIMICPARENTFOCUS )
		if pPlayer.getAdvancedStartPoints() > -1:
			self.m_bSelectedTechDirty = true
			self.X_ADD_TECH_BUTTON = 10
			self.W_ADD_TECH_BUTTON = 150
			self.H_ADD_TECH_BUTTON = 30
			self.X_ADVANCED_START_TEXT = self.X_ADD_TECH_BUTTON + self.W_ADD_TECH_BUTTON + 20

			szText = CyTranslator().getText("TXT_KEY_WB_AS_ADD_TECH", ())
			screen.setButtonGFC("AddTechButton", szText, "", self.X_ADD_TECH_BUTTON, screen.getYResolution() - 42, self.W_ADD_TECH_BUTTON, self.H_ADD_TECH_BUTTON, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD )


## Hide Techs ##
		iSize = 28
		sBorder = CyArtFileMgr().getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath()
		screen.addCheckBoxGFC("PlatyHideResearched", "Art/Interface/Buttons/Process/ProcessResearch.dds", sBorder, 10, 10, iSize, iSize, WidgetTypes.WIDGET_PYTHON, 7801, 0, ButtonStyles.BUTTON_STYLE_IMAGE)
		screen.setState("PlatyHideResearched", self.bResearched)
		screen.addCheckBoxGFC("PlatyHideDisabled", CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_RED_X").getPath(), sBorder, 10 + iSize, 10, iSize, iSize, WidgetTypes.WIDGET_PYTHON, 7801, 1, ButtonStyles.BUTTON_STYLE_IMAGE)
		screen.setState("PlatyHideDisabled", self.bDisabled)

		if CyGame().isDebugMode():
			screen.addDropDownBoxGFC( "CivDropDown", 10 + iSize * 3, 8, 160, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.SMALL_FONT )
			screen.setActivation( "CivDropDown", ActivationTypes.ACTIVATE_MIMICPARENTFOCUS )
			for j in xrange(gc.getMAX_PLAYERS()):
				if gc.getPlayer(j).isAlive():
					screen.addPullDownString("CivDropDown", gc.getPlayer(j).getName(), j, j, self.iCivSelected == j)

		self.iFromEra = min(gc.getNumEraInfos() - 1, max(0,self.iFromEra))
		self.iHideEra = min(gc.getNumEraInfos() - 1, max(0,self.iHideEra))
		self.iMinX = 9999999
		iMaxY = 1
		for i in xrange(gc.getNumTechInfos()):
			Info = gc.getTechInfo(i)
			if Info.getEra() < self.iFromEra: continue
			if Info.getEra() > self.iHideEra: continue
			if self.bResearched and pTeam.isHasTech(i): continue
			if self.bDisabled and not pPlayer.canEverResearch(i): continue
			iX = Info.getGridX()
			if iX < 1: continue
			self.iMinX = min(self.iMinX, iX)
			iMaxY = max(iMaxY, Info.getGridY())

		screen.addDropDownBoxGFC("PlatyHideFromEra", screen.getXResolution() - 270, 8, 120, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.setButtonGFC("RightArrow", "", "", screen.getXResolution() - 145, 10, 24, 24, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_ARROW_RIGHT)
		screen.addDropDownBoxGFC("PlatyHideToEra", screen.getXResolution() - 120, 8, 120, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for iEra in xrange(gc.getNumEraInfos()):
			if iEra <= self.iHideEra:
				screen.addPullDownString("PlatyHideFromEra", gc.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == self.iFromEra)
			if iEra >= self.iFromEra:
				screen.addPullDownString("PlatyHideToEra", gc.getEraInfo(iEra).getDescription(), iEra, iEra, iEra == self.iHideEra)
## Hide Techs ##
		szText = "<font=4>" + CyTranslator().getText("TXT_KEY_TECH_CHOOSER_TITLE", ()).upper() + "</font>"
		screen.setLabel( "TechTitleHeader", "Background", szText, CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, 8, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		if screen.isPersistent() and not self.bResearched:
			self.updateTechRecords(False)
			return

		global MAX_ITEMS
		iMaxPanelWidth = (screen.getXResolution() - ((MIN_DISPLAY_PANEL - 1) * BOX_X_SPACING))/MIN_DISPLAY_PANEL
		MAX_ITEMS = max(0, ((iMaxPanelWidth - (PIXEL_INCREMENT * 3)) / X_INCREMENT) - 2)

		self.nWidgetCount = 0
		screen.setPersistent(True)
		self.initData()
		self.GreatPeopleTech()

		iHeight = screen.getYResolution() - 110
		screen.addScrollPanel("TechList", u"", 0, 40, screen.getXResolution(), iHeight, PanelStyles.PANEL_STYLE_EXTERNAL)
		screen.setActivation("TechList", ActivationTypes.ACTIVATE_NORMAL)

		global BOX_Y_SPACING
		iEmptySpace = iHeight - 12 - int((iMaxY + 1) * BOX_HEIGHT * 0.5)
		iNumBoxSpace = (iMaxY + 1)/2 - 1
		BOX_Y_SPACING = max(BOX_Y_SPACING, iEmptySpace/iNumBoxSpace)

		self.placeTechs()

	def initData(self):
		iCivilization = gc.getPlayer(self.iCivSelected).getCivilizationType()
		if iCivilization != self.iCivilization:
			self.iCivilization = iCivilization
			self.TechBenefits = {}
			self.aiCurrentState = []
			self.GreatPeople = []
			## Initialize colour sets. 0=Platyping Default, 1=C2C initial, 2=C2C Rainbow, 3=Personal
			self.iColourOpt = 0
			self.EraColor = []
			self.iCanNotResearchColour = [206, 65, 69]  # Can not research colour
			self.iHasTechBoxColour = [222, 222, 22]     # colour of tech box if it has been researched
			self.iIsResearchingColour = [100, 140, 220] # colour of tech box if it is being researched

		if not self.TechBenefits:
			self.updateBenefits(iCivilization)
		if not self.aiCurrentState:
			for i in xrange(gc.getNumTechInfos()):
				self.aiCurrentState.append(-1)
		if not self.GreatPeople:
			for iUnitClass in xrange(gc.getNumUnitClassInfos()):
				iUnit = gc.getCivilizationInfo(iCivilization).getCivilizationUnits(iUnitClass)
				if iUnit == -1: continue
				if iUnit in self.GreatPeople: continue
				if gc.getUnitInfo(iUnit).getBaseDiscover() > 0:
					self.GreatPeople.append(iUnit)

		# Eventually the colour options will be read in from XML and .INI files
		self.setColourScheme()

	def setColourScheme(self):
		# Defaults
		self.iHasTechBoxColour = [128, 128, 128]     # colour of tech box if it has been researched


		# Default colours are set up for all eras just in case the actual number of Eras does not match those defined in the colour lists
		iNumEras = gc.getNumEraInfos()
		self.EraColor = [0] * iNumEras
		for iEra in xrange(iNumEras):
			iColor = iEra * 255 / iNumEras
			self.EraColor[iEra] = [iColor*5, (255 - iColor)/2, iColor]

		if self.iColourOpt == 0:		# # Rainbow colours (C2C v38.5) - tech tree colors - start
				##Prehistoric/Ancient/Classical - Black/Brown/Red
				##Medieval/Renaissance/Industrial - Orange/Yellow/Green
				##Modern/Information/Nanotech - Cyan/Blue/Indigo
				##Transhuman/Galactic/Cosmic - Purple/Violet/Pink
				##Transcendent/Future - White/Grey

			self.EraColor = [[0, 0, 0], [128, 64, 0], [255, 64, 0], [255, 128, 0], [200, 200, 0], [0, 200, 0], [0, 200, 200], [0, 128, 255], [0, 0, 200], [128, 0, 255], [200, 0, 200], [255, 128, 255], [200, 200, 200], [128, 128, 128]]

			self.iIsResearchingColour = [100, 140, 180] # colour of tech box if it is being researched

		elif self.iColourOpt == 1: 		# # Rise of Mankind 2.91 - tech tree colors - start
				# self.EraColor[0] = self.EraColor[0,0]
					# # Ancient Era
				self.EraColor[1] = [160, 100, 160]
					# # Classical Era
				self.EraColor[2] = [160, 100, 100]
					# # Medieval Era
				self.EraColor[3] = [160, 160, 100]
					# # Renaissance Era
				self.EraColor[4] = [160, 160, 50]
					# # Industrial Era
				self.EraColor[5] = [60, 200, 60]
					# # Modern Era
				self.EraColor[6] = [100, 104, 160]
					# # Future Era
				self.EraColor[7] = [50, 160, 250]


	## Era Colours ##


	def updateBenefits(self, iCivilization):
		self.TechBenefits = {}

		for iTech in xrange(gc.getNumTechInfos()):
			self.TechBenefits[iTech] = []
			Info = gc.getTechInfo(iTech)
			if Info.isGraphicalOnly(): continue
			if Info.getGridX() < 0: continue
			if Info.getGridY() < 0: continue
			for j in xrange(gc.getNumRouteInfos()):
				if gc.getRouteInfo(j).getTechMovementChange(iTech) != 0:
					self.TechBenefits[iTech].append(["RouteChange", j])
			if Info.getFirstFreeUnitClass() > -1:
				if iCivilization == -1:
					iItem = gc.getUnitClassInfo(Info.getFirstFreeUnitClass()).getDefaultUnitIndex()
				else:
					iItem = gc.getCivilizationInfo(iCivilization).getCivilizationUnits(Info.getFirstFreeUnitClass())
				if iItem > -1:
					self.TechBenefits[iTech].append(["FreeUnit", iItem])
			if Info.getFeatureProductionModifier():
				self.TechBenefits[iTech].append(["FeatureProduction", -1])
			if Info.getWorkerSpeedModifier():
				self.TechBenefits[iTech].append(["WorkerSpeed", -1])
			if Info.getTradeRoutes():
				self.TechBenefits[iTech].append(["TradeRoute", -1])
			if Info.getHealth():
				self.TechBenefits[iTech].append(["Health", -1])
			if Info.getHappiness():
				self.TechBenefits[iTech].append(["Happiness", -1])
			if Info.getFirstFreeTechs():
				self.TechBenefits[iTech].append(["FreeTech", -1])
			if Info.isExtraWaterSeeFrom():
				self.TechBenefits[iTech].append(["WaterSight", -1])
			if Info.isMapCentering():
				self.TechBenefits[iTech].append(["MapCentering", -1])
			if Info.isMapVisible():
				self.TechBenefits[iTech].append(["MapVisible", -1])
			if Info.isMapTrading():
				self.TechBenefits[iTech].append(["MapTrading", -1])
			if Info.isTechTrading():
				self.TechBenefits[iTech].append(["TechTrading", -1])
			if Info.isGoldTrading():
				self.TechBenefits[iTech].append(["GoldTrading", -1])
			if Info.isOpenBordersTrading():
				self.TechBenefits[iTech].append(["OpenBorders", -1])
			if Info.isDefensivePactTrading():
				self.TechBenefits[iTech].append(["DefensivePact", -1])
			if Info.isPermanentAllianceTrading():
				self.TechBenefits[iTech].append(["PermanentAlliance", -1])
			if Info.isVassalStateTrading():
				self.TechBenefits[iTech].append(["VassalState", -1])
			if Info.isBridgeBuilding():
				self.TechBenefits[iTech].append(["BridgeBuilding", -1])
			if Info.isIrrigation():
				self.TechBenefits[iTech].append(["EnablesIrrigation", -1])
			if Info.isIgnoreIrrigation():
				self.TechBenefits[iTech].append(["IgnoreIrrigation", -1])
			if Info.isWaterWork():
				self.TechBenefits[iTech].append(["WaterWork", -1])
			for j in xrange(DomainTypes.NUM_DOMAIN_TYPES):
				if Info.getDomainExtraMoves(j):
					self.TechBenefits[iTech].append(["DomainMoves", j])
			for j in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
				if Info.isCommerceFlexible(j):
					self.TechBenefits[iTech].append(["CommerceFlexible", j])
			for j in xrange(gc.getNumTerrainInfos()):
				if Info.isTerrainTrade(j):
					self.TechBenefits[iTech].append(["TerrainTrade", j])
			if Info.isRiverTrade():
				self.TechBenefits[iTech].append(["RiverTrade", -1])
			for j in xrange(gc.getNumImprovementInfos()):
				for k in xrange(YieldTypes.NUM_YIELD_TYPES):
					if gc.getImprovementInfo(j).getTechYieldChanges(iTech, k):
						self.TechBenefits[iTech].append(["ImprovementYield", j])
			if Info.getHelp():
				self.TechBenefits[iTech].append(["TechHelp", -1])

		for j in xrange(gc.getNumUnitClassInfos()):
			if iCivilization == -1:
				iItem = gc.getUnitClassInfo(j).getDefaultUnitIndex()
			else:
				iItem = gc.getCivilizationInfo(iCivilization).getCivilizationUnits(j)
			if iItem > -1:
				iTech = gc.getUnitInfo(iItem).getPrereqAndTech()
				if iTech > -1:
					self.TechBenefits[iTech].append(["UnlockUnit", iItem])

		for j in xrange(gc.getNumBuildingClassInfos()):
			if iCivilization == -1:
				iItem = gc.getBuildingClassInfo(j).getDefaultBuildingIndex()
			else:
				iItem = gc.getCivilizationInfo(iCivilization).getCivilizationBuildings(j)
			if iItem > -1:
				iTech = gc.getBuildingInfo(iItem).getPrereqAndTech()
				if iTech > -1:
					self.TechBenefits[iTech].append(["UnlockBuilding", iItem])
				iTech = gc.getBuildingInfo(iItem).getObsoleteTech()
				if iTech > -1:
					self.TechBenefits[iTech].append(["ObsoleteBuilding", iItem])

		for j in xrange(gc.getNumSpecialBuildingInfos()):
			iTech = gc.getSpecialBuildingInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockSpecialBuilding", j])
			iTech = gc.getSpecialBuildingInfo(j).getObsoleteTech()
			if iTech > -1:
				self.TechBenefits[iTech].append(["ObsoleteSpecialBuilding", j])

		for j in xrange(gc.getNumBonusInfos()):
			iTech = gc.getBonusInfo(j).getTechReveal()
			if iTech > -1:
				self.TechBenefits[iTech].append(["RevealBonus", j])
			iTech = gc.getBonusInfo(j).getTechObsolete()
			if iTech > -1:
				self.TechBenefits[iTech].append(["ObsoleteBonus", j])

		for j in xrange(gc.getNumPromotionInfos()):
			iTech = gc.getPromotionInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockPromotion", j])

		for j in xrange(gc.getNumBuildInfos()):
			bTechFound = False
			iTech = gc.getBuildInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockImprovement", j])
			else:
				for k in xrange(gc.getNumFeatureInfos()):
					iTech = gc.getBuildInfo(j).getFeatureTech(k)
					if iTech > -1:
						self.TechBenefits[iTech].append(["UnlockImprovement", j])

		for j in xrange(gc.getNumCivicInfos()):
			iTech = gc.getCivicInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockCivic", j])

		for j in xrange(gc.getNumProjectInfos()):
			iTech = gc.getProjectInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockProject", j])

		for j in xrange(gc.getNumProcessInfos()):
			iTech = gc.getProcessInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockProcess", j])

		for j in xrange(gc.getNumReligionInfos()):
			iTech = gc.getReligionInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockReligion", j])

		for j in xrange(gc.getNumCorporationInfos()):
			iTech = gc.getCorporationInfo(j).getTechPrereq()
			if iTech > -1:
				self.TechBenefits[iTech].append(["UnlockCorporation", j])

		# C2C Advance Settlers
		if iTech == gc.getInfoTypeForString("TECH_COLONIALISM") or iTech == gc.getInfoTypeForString("TECH_STEAM_POWER"):
			self.TechBenefits[iTech].append(["Population", -1])
		# C2C Advance Settlers

		global BOX_WIDTH
		iMax = 0
		for iTech in xrange(gc.getNumTechInfos()):
			iMax = max(iMax, len(self.TechBenefits[iTech]))
		iMax = min(MAX_ITEMS, iMax)
		iMax += 2
		BOX_WIDTH = PIXEL_INCREMENT * 3 + (X_INCREMENT * iMax)

	def placeTechs (self):
		if CyGame().isPitbossHost(): return

		ARROW_X = CyArtFileMgr().getInterfaceArtInfo("ARROW_X").getPath()
		ARROW_Y = CyArtFileMgr().getInterfaceArtInfo("ARROW_Y").getPath()
		ARROW_MXMY = CyArtFileMgr().getInterfaceArtInfo("ARROW_MXMY").getPath()
		ARROW_XY = CyArtFileMgr().getInterfaceArtInfo("ARROW_XY").getPath()
		ARROW_MXY = CyArtFileMgr().getInterfaceArtInfo("ARROW_MXY").getPath()
		ARROW_XMY = CyArtFileMgr().getInterfaceArtInfo("ARROW_XMY").getPath()
		ARROW_HEAD = CyArtFileMgr().getInterfaceArtInfo("ARROW_HEAD").getPath()

		screen = CyGInterfaceScreen("TechChooser", CvScreenEnums.TECH_CHOOSER)
		pPlayer = gc.getPlayer(self.iCivSelected)
		pTeam = gc.getTeam(pPlayer.getTeam())
		iCiv = pPlayer.getCivilizationType()
		for i in xrange(gc.getNumTechInfos()):
## Hide Techs ##
			Info = gc.getTechInfo(i)
			if Info.isGraphicalOnly(): continue
			if Info.getGridX() < 0: continue
			if Info.getGridY() < 0: continue
			iEra = Info.getEra()
			if iEra < self.iFromEra: continue
			if iEra > self.iHideEra: continue
			if self.bResearched and pTeam.isHasTech(i): continue
			if self.bDisabled and not pPlayer.canEverResearch(i): continue
			iX = (Info.getGridX() - self.iMinX) * (BOX_X_SPACING + BOX_WIDTH)
			iY = (Info.getGridY() -1) * (BOX_HEIGHT + BOX_Y_SPACING)/2
			szTechRecord = "TechRecord" + str(i)

			screen.attachPanelAt( "TechList", szTechRecord, u"", u"", True, False, PanelStyles.PANEL_STYLE_TECH, iX,  iY, BOX_WIDTH, BOX_HEIGHT, WidgetTypes.WIDGET_TECH_TREE, i, -1 )
			screen.setActivation( szTechRecord, ActivationTypes.ACTIVATE_MIMICPARENTFOCUS)
	## Tech Colors ##
			# screen.setPanelColor(szTechRecord, self.iCanNotResearchColour[0], self.iCanNotResearchColour[1], self.iCanNotResearchColour[2])
			self.aiCurrentState[i] = CIV_NO_RESEARCH
			if pTeam.isHasTech(i):
				# screen.setPanelColor(szTechRecord, self.iHasTechBoxColour[0], self.iHasTechBoxColour[1], self.iHasTechBoxColour[2])
				self.aiCurrentState[i] = CIV_HAS_TECH
			elif pPlayer.isResearchingTech(i):
				# screen.setPanelColor(szTechRecord, self.iIsResearchingColour[0], self.iIsResearchingColour[1], self.iIsResearchingColour[2])
				self.aiCurrentState[i] = CIV_IS_RESEARCHING
			elif pPlayer.canEverResearch(i):
				# screen.setPanelColor(szTechRecord, self.EraColor[iEra][0], self.EraColor[iEra][1], self.EraColor[iEra][2])
				self.aiCurrentState[i] = CIV_TECH_AVAILABLE
	## Tech Colors ##
## Hide Techs ##
			iX = 6
			iY = 6

			szTechID = "TechID" + str(i)
			szTechString = "<font=1>"
			if pPlayer.isResearchingTech(i):
				szTechString += str(pPlayer.getQueuePosition(i)) + ". "
	## Advisors ##
			iAdjustment = 6
			iAdvisor = Info.getAdvisorType()
			if iAdvisor > -1:
				szTechString += CyTranslator().getText(self.Advisors[iAdvisor], ())
				iAdjustment = 3
			szTechString += Info.getDescription() + "</font>"
			screen.setTextAt(szTechID, szTechRecord, szTechString, CvUtil.FONT_LEFT_JUSTIFY, iX + iAdjustment + (X_INCREMENT * 2), iY + 6, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_TECH_TREE, i, -1)
			screen.setActivation(szTechID, ActivationTypes.ACTIVATE_MIMICPARENTFOCUS)
	## Advisors ##

			szTechButtonID = "TechButtonID" + str(i)
			screen.addDDSGFCAt( szTechButtonID, szTechRecord, Info.getButton(), iX + 6, iY + 6, TEXTURE_SIZE + X_INCREMENT, TEXTURE_SIZE + X_INCREMENT, WidgetTypes.WIDGET_TECH_TREE, i, -1, False )

			fX = X_START + (X_INCREMENT * 2)

			for j in xrange(min(len(self.TechBenefits[i]), MAX_ITEMS)):
				sType = self.TechBenefits[i][j][0]
				iItem = self.TechBenefits[i][j][1]
				sButton = "Item" + str(i * 1000 + j)
				sObsolete = "Obsolete" + str(i * 1000 + j)
				if sType == "UnlockUnit":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getUnitInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, iItem, 1, True)
				elif sType == "UnlockBuilding":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getBuildingInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iItem, 1, True )
				elif sType == "ObsoleteBuilding":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getBuildingInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE, iItem, -1, False )
					screen.addDDSGFCAt(sObsolete, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_RED_X").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE, iItem, -1, False)
				elif sType == "UnlockSpecialBuilding":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getSpecialBuildingInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_SPECIAL_BUILDING, i, iItem, False)
				elif sType == "ObsoleteSpecialBuilding":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getSpecialBuildingInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_SPECIAL, i, iItem, False)
					screen.addDDSGFCAt(sObsolete, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_RED_X").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_SPECIAL, iItem, -1, False)
				elif sType == "RevealBonus":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getBonusInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_BONUS_REVEAL, i, iItem, False)
				elif sType == "ObsoleteBonus":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getBonusInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_BONUS, i, iItem, False)
					screen.addDDSGFCAt(sObsolete, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTONS_RED_X").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OBSOLETE_BONUS, iItem, -1, False)
				elif sType == "RouteChange":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_MOVE_BONUS").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MOVE_BONUS, i, -1, False)
				elif sType == "UnlockPromotion":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getPromotionInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROMOTION, iItem, -1, False)
				elif sType == "FreeUnit":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getUnitInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FREE_UNIT, iItem, i, False)
				elif sType == "FeatureProduction":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_FEATURE_PRODUCTION").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FEATURE_PRODUCTION, i, -1, False)
				elif sType == "WorkerSpeed":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_WORKER_SPEED").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_WORKER_RATE, i, -1, False)
				elif sType == "TradeRoute":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_TRADE_ROUTES").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TRADE_ROUTES, i, -1, False)
				elif sType == "Health":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_HEALTH").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_HEALTH_RATE, i, -1, False)
				elif sType == "Happiness":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_HAPPINESS").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_HAPPINESS_RATE, i, -1, False)
		# C2C Advance Settlers
				elif sType == "Population":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_POPULATION").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_HAPPINESS_RATE, i, -1, False)
		# C2C Advance Settlers
				elif sType == "FreeTech":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_FREETECH").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FREE_TECH, i, -1, False)
				elif sType == "WaterSight":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_LOS").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_LOS_BONUS, i, -1, False)
				elif sType == "MapCentering":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_MAPCENTER").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MAP_CENTER, i, -1, False)
				elif sType == "MapVisible":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_MAPREVEAL").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MAP_REVEAL, i, -1, False)
				elif sType == "MapTrading":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_MAPTRADING").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_MAP_TRADE, i, -1, False)
				elif sType == "TechTrading":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_TECHTRADING").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TECH_TRADE, i, -1, False)
				elif sType == "GoldTrading":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_GOLDTRADING").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_GOLD_TRADE, i, -1, False)
				elif sType == "OpenBorders":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_OPENBORDERS").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_OPEN_BORDERS, i, -1, False)
				elif sType == "DefensivePact":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_DEFENSIVEPACT").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_DEFENSIVE_PACT, i, -1, False)
				elif sType == "PermanentAlliance":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_PERMALLIANCE").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_PERMANENT_ALLIANCE, i, -1, False)
				elif sType == "VassalState":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_VASSAL").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_VASSAL_STATE, i, -1, False)
				elif sType == "BridgeBuilding":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_BRIDGEBUILDING").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_BUILD_BRIDGE, i, -1, False)
				elif sType == "EnablesIrrigation":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_IRRIGATION").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_IRRIGATION, i, -1, False)
				elif sType == "IgnoreIrrigation":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_NOIRRIGATION").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_IGNORE_IRRIGATION, i, -1, False)
				elif sType == "WaterWork":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_WATERWORK").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_WATER_WORK, i, -1, False)
				elif sType == "UnlockImprovement":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getBuildInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_IMPROVEMENT, i, iItem, False)
				elif sType == "DomainMoves":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_WATERMOVES").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_DOMAIN_EXTRA_MOVES, i, iItem, False)
				elif sType == "CommerceFlexible":
					szFileName = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_QUESTIONMARK").getPath()
					if iItem == CommerceTypes.COMMERCE_CULTURE:
						szFileName = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_CULTURE").getPath()
					elif iItem == CommerceTypes.COMMERCE_ESPIONAGE:
						szFileName = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_ESPIONAGE").getPath()
					#elif iItem == CommerceTypes.COMMERCE_RESEARCH:
					#	szFileName = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_RESEARCH").getPath()
					#elif iItem == CommerceTypes.COMMERCE_GOLD:
					#	szFileName = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_GOLD").getPath()
					screen.addDDSGFCAt(sButton, szTechRecord, szFileName, iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_ADJUST, i, iItem, False)
				elif sType == "TerrainTrade":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_WATERTRADE").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TERRAIN_TRADE, i, iItem, False)
				elif sType == "RiverTrade":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_TECH_RIVERTRADE").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TERRAIN_TRADE, i, gc.getNumTerrainInfos(), False)
				elif sType == "ImprovementYield":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getImprovementInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_YIELD_CHANGE, i, iItem, False)
				elif sType == "UnlockCivic":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getCivicInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_CIVIC_REVEAL, i, iItem, False)
				elif sType == "UnlockProject":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getProjectInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT, iItem, 1, False)
				elif sType == "UnlockProcess":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getProcessInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_PROCESS_INFO, i, iItem, False)
				elif sType == "UnlockReligion":
					if CyGame().isOption(GameOptionTypes.GAMEOPTION_PICK_RELIGION):
						szButton = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_POPUPBUTTON_RELIGION").getPath()
					else:
						szButton = gc.getReligionInfo(iItem).getButton()
					screen.addDDSGFCAt(sButton, szTechRecord, szButton, iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FOUND_RELIGION, i, iItem, False)
				elif sType == "UnlockCorporation":
					screen.addDDSGFCAt(sButton, szTechRecord, gc.getCorporationInfo(iItem).getButton(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_FOUND_CORPORATION, i, iItem, False)
				elif sType == "TechHelp":
					screen.addDDSGFCAt(sButton, szTechRecord, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_GENERAL_QUESTIONMARK").getPath(), iX + fX, iY + Y_ROW, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PYTHON, 7800, i, False)
				fX += X_INCREMENT
			fX = BOX_WIDTH - (PIXEL_INCREMENT * 2)

			for j in xrange(gc.getNUM_AND_TECH_PREREQS()):
				eTech = Info.getPrereqAndTechs(j)
				if eTech == -1: break
				fX -= X_INCREMENT
				szTechPrereqID = "TechPrereqID" + str((i * 1000) + j)
				screen.addDDSGFCAt( szTechPrereqID, szTechRecord, gc.getTechInfo(eTech).getButton(), iX + fX, iY + 6, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_HELP_TECH_PREPREQ, eTech, -1, False )

			for j in xrange(gc.getNUM_OR_TECH_PREREQS()):
				eTech = Info.getPrereqOrTechs(j)
				if eTech == -1: break
				eInfo = gc.getTechInfo(eTech)
## Hide Techs ##
				if eInfo.getEra() < self.iFromEra: continue
				if eInfo.getEra() > self.iHideEra: continue
				if self.bResearched and pTeam.isHasTech(eTech): continue
				if self.bDisabled and not pPlayer.canEverResearch(eTech): continue
## Hide Techs ##
				iX = (eInfo.getGridX() - self.iMinX) * (BOX_X_SPACING + BOX_WIDTH) + BOX_WIDTH - 6
				iY = (eInfo.getGridY() -1) * (BOX_HEIGHT + BOX_Y_SPACING)/2 - 6

				xDiff = Info.getGridX() - gc.getTechInfo(eTech).getGridX()
				yDiff = Info.getGridY() - gc.getTechInfo(eTech).getGridY()

				if yDiff == 0:
					screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_X, iX, iY + self.getYStart(4), self.getWidth(xDiff), 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
					screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_HEAD, iX + self.getWidth(xDiff), iY + self.getYStart(4), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				elif yDiff < 0:
					if yDiff < -3 and xDiff == 1:
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_X, iX, iY + self.getYStart(2), self.getWidth(xDiff)/3, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_XY, iX + self.getWidth(xDiff)/3, iY + self.getYStart(2), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_Y, iX + self.getWidth(xDiff)/3, iY + self.getYStart(2) + 8 - self.getHeight(yDiff, -4), 8, self.getHeight(yDiff, -4) - 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_XMY, iX + self.getWidth(xDiff)/3, iY + self.getYStart(2) - self.getHeight(yDiff, -4), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_X, iX + 8 + self.getWidth(xDiff)/3, iY + self.getYStart(2) - self.getHeight(yDiff, -4), self.getWidth(xDiff) * 2/3 - 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_HEAD, iX + self.getWidth(xDiff), iY + self.getYStart(2) - self.getHeight(yDiff, -4), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
					else:
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_X, iX, iY + self.getYStart(3), self.getWidth(xDiff)/2, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_XY, iX + self.getWidth(xDiff)/2, iY + self.getYStart(3), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_Y, iX + self.getWidth(xDiff)/2, iY + self.getYStart(3) + 8 - self.getHeight(yDiff, -2), 8, self.getHeight(yDiff, -2) - 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_XMY, iX + self.getWidth(xDiff)/2, iY + self.getYStart(3) - self.getHeight(yDiff, -2), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_X, iX + 8 + self.getWidth(xDiff)/2, iY + self.getYStart(3) - self.getHeight(yDiff, -2), self.getWidth(xDiff)/2 - 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_HEAD, iX + self.getWidth(xDiff), iY + self.getYStart(3) - self.getHeight(yDiff, -2), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
				else:
					pass
					if yDiff > 3 and xDiff == 1:
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_X, iX, iY + self.getYStart(6), self.getWidth(xDiff)/3, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_MXMY, iX + self.getWidth(xDiff)/3, iY + self.getYStart(6), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_Y, iX + self.getWidth(xDiff)/3, iY + self.getYStart(6) + 8, 8, self.getHeight(yDiff, -4) - 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_MXY, iX + self.getWidth(xDiff)/3, iY + self.getYStart(6) + self.getHeight(yDiff, -4), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_X, iX + 8 + self.getWidth(xDiff)/3, iY + self.getYStart(6) + self.getHeight(yDiff, -4), self.getWidth(xDiff) * 2/3 - 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_HEAD, iX + self.getWidth(xDiff), iY + self.getYStart(6) + self.getHeight(yDiff, -4), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
					else:
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_X, iX, iY + self.getYStart(5), self.getWidth(xDiff)/2, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_MXMY, iX + self.getWidth(xDiff)/2, iY + self.getYStart(5), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_Y, iX + self.getWidth(xDiff)/2, iY + self.getYStart(5) + 8, 8, self.getHeight(yDiff, -2) - 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_MXY, iX + self.getWidth(xDiff)/2, iY + self.getYStart(5) + self.getHeight(yDiff, -2), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_X, iX + 8 + self.getWidth(xDiff)/2, iY + self.getYStart(5) + self.getHeight(yDiff, -2), self.getWidth(xDiff)/2 - 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
						screen.addDDSGFCAt( self.getNextWidgetName(), "TechList", ARROW_HEAD, iX + self.getWidth(xDiff), iY + self.getYStart(5) + self.getHeight(yDiff, -2), 8, 8, WidgetTypes.WIDGET_GENERAL, -1, -1, False )
		screen.setFocus("TechList")
		self.updateTechRecords(True)

	def getYStart(self, iY):
		return BOX_HEIGHT * iY / 8

	def getWidth(self, xDiff):
		return xDiff * BOX_X_SPACING + (xDiff - 1) * BOX_WIDTH

	def getHeight(self, yDiff, iAdjustment):
		return (abs(yDiff) * (BOX_Y_SPACING + BOX_HEIGHT)/2) + (iAdjustment * BOX_HEIGHT/8)

	def updateTechRecords (self, bForce):
		if CyGame().isPitbossHost(): return

		screen = CyGInterfaceScreen("TechChooser", CvScreenEnums.TECH_CHOOSER)
		lChanged = []
		pPlayer = gc.getPlayer(self.iCivSelected)
		iCiv = pPlayer.getCivilizationType()
		pTeam = gc.getTeam(pPlayer.getTeam())

		for i in xrange(gc.getNumTechInfos()):
			Info = gc.getTechInfo(i)
## Hide Techs ##
			if Info.isGraphicalOnly(): continue
			if Info.getGridX() < 0: continue
			if Info.getGridY() < 0: continue
			if Info.getEra() < self.iFromEra: continue
			if Info.getEra() > self.iHideEra: continue
			if self.bResearched and pTeam.isHasTech(i): continue
			if self.bDisabled and not pPlayer.canEverResearch(i): continue
## Hide Techs ##
			if pTeam.isHasTech(i):
				if self.aiCurrentState[i] != CIV_HAS_TECH or bForce:
					self.aiCurrentState[i] = CIV_HAS_TECH
					lChanged.append(i)
			elif pPlayer.isResearchingTech(i):
				self.aiCurrentState[i] = CIV_IS_RESEARCHING
				lChanged.append(i)
			elif pPlayer.canEverResearch(i):
				if self.aiCurrentState[i] != CIV_TECH_AVAILABLE or bForce:
					self.aiCurrentState[i] = CIV_TECH_AVAILABLE
					lChanged.append(i)
			else:
				if self.aiCurrentState[i] != CIV_NO_RESEARCH or bForce:
					self.aiCurrentState[i] = CIV_NO_RESEARCH
					lChanged.append(i)

		for i in lChanged:
			Info = gc.getTechInfo(i)
			szTechRecord = "TechRecord" + str(i)
			szTechID = "TechID" + str(i)
			szTechString = "<font=1>"

			iX = (gc.getTechInfo(i).getGridX() - self.iMinX) * (BOX_X_SPACING + BOX_WIDTH)
			iY = (gc.getTechInfo(i).getGridY() -1) * (BOX_HEIGHT + BOX_Y_SPACING)/2
## Tech Progress ##
			iQueue = pPlayer.getQueuePosition(i)
			sProgress = "Progress" + str(i)
			screen.hide(sProgress)
			if pPlayer.isResearchingTech(i) and iQueue == 1:
				screen.addStackedBarGFCAt(sProgress, "TechList", iX + 6, iY + 2, (BOX_WIDTH - 12), 12, InfoBarTypes.NUM_INFOBAR_TYPES, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.setStackedBarColors(sProgress, InfoBarTypes.INFOBAR_STORED, gc.getInfoTypeForString("COLOR_RESEARCH_STORED"))
				screen.setStackedBarColors(sProgress, InfoBarTypes.INFOBAR_RATE, gc.getInfoTypeForString("COLOR_RESEARCH_RATE"))
				screen.setStackedBarColors(sProgress, InfoBarTypes.INFOBAR_EMPTY, gc.getInfoTypeForString("COLOR_EMPTY"))
				iProgress = pTeam.getResearchProgress(i)
				iThreshold = pTeam.getResearchCost(i)
				iRate = pPlayer.calculateResearchRate(i)
				iOverflow = pPlayer.getOverflowResearch() * pPlayer.calculateResearchModifier(i) /100
				screen.setBarPercentage(sProgress, InfoBarTypes.INFOBAR_STORED, float(iProgress) / float(iThreshold))
				screen.setBarPercentage(sProgress, InfoBarTypes.INFOBAR_RATE, 0.0)
				if iThreshold > (iProgress + iOverflow):
					screen.setBarPercentage(sProgress, InfoBarTypes.INFOBAR_RATE, float(iRate) / (iThreshold - iProgress - iOverflow))
## Tech Progress ##
## Advisors ##
			iAdjustment = 6
			iAdvisor = Info.getAdvisorType()
			if iAdvisor > -1:
				szTechString += CyTranslator().getText(self.Advisors[iAdvisor], ())
				iAdjustment = 3
			if pPlayer.isResearchingTech(i):
				szTechString += str(iQueue) + ") "
			szTechString += Info.getDescription() + "</font>"
			screen.setTextAt(szTechID, "TechList", szTechString, CvUtil.FONT_LEFT_JUSTIFY, iX + iAdjustment + 6 + (X_INCREMENT * 2), iY + 12, -0.1, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_TECH_TREE, i, -1)
			screen.setActivation(szTechID, ActivationTypes.ACTIVATE_MIMICPARENTFOCUS)
## Advisors ##
			screen.setPanelColor(szTechRecord, self.iCanNotResearchColour[0], self.iCanNotResearchColour[1], self.iCanNotResearchColour[2])
			if gc.getTeam(pPlayer.getTeam()).isHasTech(i):
				screen.setPanelColor(szTechRecord, self.iHasTechBoxColour[0], self.iHasTechBoxColour[1], self.iHasTechBoxColour[2])
			elif pPlayer.isResearchingTech(i):
				screen.setPanelColor(szTechRecord, self.iIsResearchingColour[0], self.iIsResearchingColour[1], self.iIsResearchingColour[2])
			elif pPlayer.canEverResearch(i):
	## Era Colours ##
				iEra = Info.getEra()
				screen.setPanelColor(szTechRecord, self.EraColor[iEra][0], self.EraColor[iEra][1], self.EraColor[iEra][2])
	## Era Colours ##
## Great People ##
		self.GreatPeopleTech()

	def GreatPeopleTech(self):
		if CyGame().isInAdvancedStart(): return
		screen = CyGInterfaceScreen("TechChooser", CvScreenEnums.TECH_CHOOSER)
		iGPX = 42
		for iUnit in self.GreatPeople:
			for iFlavor in xrange(gc.getNumFlavorTypes()):
				if gc.getUnitInfo(iUnit).getFlavorValue(iFlavor) > 0:
					break
			iMaxFlavor = 0
			iTech = -1
			for iLoopTech in xrange(gc.getNumTechInfos()):
				if gc.getPlayer(CyGame().getActivePlayer()).canResearch(iLoopTech, False):
					if gc.getTechInfo(iLoopTech).getFlavorValue(iFlavor) > iMaxFlavor:
						iMaxFlavor = gc.getTechInfo(iLoopTech).getFlavorValue(iFlavor)
						iTech = iLoopTech
			if iTech > -1:
				screen.addDDSGFCAt("GreatPeopleUnit" + str(iUnit),"TechBottomPanel", gc.getUnitInfo(iUnit).getButton(), iGPX, 16, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, iUnit, -1, False )
				iGPX += TEXTURE_SIZE
				screen.addDDSGFCAt("GreatPeopleTech" + str(iUnit),"TechBottomPanel", gc.getTechInfo(iTech).getButton(), iGPX, 16, TEXTURE_SIZE, TEXTURE_SIZE, WidgetTypes.WIDGET_TECH_TREE, iTech, -1, False )
				iGPX += TEXTURE_SIZE * 2
## Great People ##

	def handleInput (self, inputClass):
		screen = CyGInterfaceScreen("TechChooser", CvScreenEnums.TECH_CHOOSER)
## Hide Techs ##
		if inputClass.getFunctionName().find("PlatyHide") > -1:
			if inputClass.getFunctionName() == "PlatyHideFromEra":
				iIndex = screen.getSelectedPullDownID("PlatyHideFromEra")
				self.iFromEra = screen.getPullDownData("PlatyHideFromEra", iIndex)
			elif inputClass.getFunctionName() == "PlatyHideToEra":
				iIndex = screen.getSelectedPullDownID("PlatyHideToEra")
				self.iHideEra = screen.getPullDownData("PlatyHideToEra", iIndex)
			elif inputClass.getFunctionName() == "PlatyHideResearched":
				self.bResearched = not self.bResearched
			elif inputClass.getFunctionName() == "PlatyHideDisabled":
				self.bDisabled = not self.bDisabled
			screen.setPersistent(False)
			self.interfaceScreen()
			return
## Hide Techs ##
		if inputClass.getFunctionName() == "CivDropDown":
			iIndex = screen.getSelectedPullDownID("CivDropDown")
			self.iCivSelected = screen.getPullDownData("CivDropDown", iIndex)
			screen.setPersistent(False)
			self.TechBenefits = {}
			self.GreatPeople = []
			self.interfaceScreen()
			return

		self.iCivSelected = CyGame().getActivePlayer()
		pPlayer = gc.getPlayer(self.iCivSelected)
		if pPlayer.getAdvancedStartPoints() > -1:
			if inputClass.getFunctionName() == "AddTechButton":
				if pPlayer.getAdvancedStartTechCost(self.m_iSelectedTech, True) > -1:
					CyMessageControl().sendAdvancedStartAction(AdvancedStartActionTypes.ADVANCEDSTARTACTION_TECH, self.iCivSelected, -1, -1, self.m_iSelectedTech, true)	#Action, Player, X, Y, Data, bAdd
					self.m_bTechRecordsDirty = true
					self.m_bSelectedTechDirty = true

			elif inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
				if inputClass.getButtonType() == WidgetTypes.WIDGET_TECH_TREE:
					self.m_iSelectedTech = inputClass.getData1()
					self.updateSelectedTech()
		return 0

	def getNextWidgetName(self):
		szName = "TechArrow" + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def update(self, fDelta):
		screen = CyGInterfaceScreen("TechChooser", CvScreenEnums.TECH_CHOOSER)
		if CyInterface().isDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT):
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, False)

			if self.m_bSelectedTechDirty:
				self.m_bSelectedTechDirty = False
				self.updateSelectedTech()

			if self.m_bTechRecordsDirty:
				self.m_bTechRecordsDirty = False
				self.updateTechRecords(True)

			if gc.getPlayer(CyGame().getActivePlayer()).getAdvancedStartPoints() < 0:
				screen.hide("AddTechButton")
				screen.hide("ASPointsLabel")
				screen.hide("SelectedTechLabel")
		return

	def updateSelectedTech(self):
		screen = CyGInterfaceScreen("TechChooser", CvScreenEnums.TECH_CHOOSER)
		pPlayer = gc.getPlayer(CyGame().getActivePlayer())

		szName = ""
		iCost = 0
		if self.m_iSelectedTech > -1:
			szName = gc.getTechInfo(self.m_iSelectedTech).getDescription()
			iCost = pPlayer.getAdvancedStartTechCost(self.m_iSelectedTech, true)

		screen.hide("ASPointsLabel")
		screen.hide("AddTechButton")
		if iCost > 0:
			szText = u"<font=4>" + CyTranslator().getText("TXT_KEY_WB_AS_SELECTED_TECH_COST", (iCost, pPlayer.getAdvancedStartPoints())) + u"</font>"
			screen.setLabel("ASPointsLabel", "Background", szText, CvUtil.FONT_LEFT_JUSTIFY, self.X_ADVANCED_START_TEXT, screen.getYResolution() - 42, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			if pPlayer.getAdvancedStartPoints() >= iCost:
				screen.show("AddTechButton")

		szText = "<font=4>" + szName + "</font>"
		screen.setLabel("SelectedTechLabel", "Background", szText, CvUtil.FONT_CENTER_JUSTIFY, screen.getXResolution()/2, screen.getYResolution() - 42, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

	def onClose(self):
		pPlayer = gc.getPlayer(CyGame().getActivePlayer())
		if pPlayer.getAdvancedStartPoints() > -1:
			CyInterface().setDirty(InterfaceDirtyBits.Advanced_Start_DIRTY_BIT, true)
		return 0

	def hideScreen (self):
		screen = CyGInterfaceScreen("TechChooser", CvScreenEnums.TECH_CHOOSER)
		screen.hideScreen()