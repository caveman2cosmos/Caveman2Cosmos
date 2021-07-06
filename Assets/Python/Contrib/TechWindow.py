## TechWindowWide
##
## Originally by SirRethcir: Techanzeige hinzugefÃ¼gt
## Enhanced by Roamty, Caesium, Guarav
##
## Copyright (c) 2008 The BUG Mod.

from CvPythonExtensions import *

import BugCore
TechWindowOpt = BugCore.game.TechWindow

GC = CyGlobalContext()
TRNSLTR = CyTranslator()

class CvTechSplashScreen:
	"Splash screen for techs"
	def __init__(self, iScreenID):
		self.nScreenId = iScreenID

		self.iTech = -1
		self.nWidgetCount = 0

		# widget names
		self.WIDGET_ID = "TechSplashScreenWidget"
		self.SCREEN_NAME = "TechSplashScreen"
		self.EXIT_ID = "TechSplashExit"

		self.X_SCREEN = 17
		self.Y_SCREEN = 27
		self.W_SCREEN = 1024
		self.H_SCREEN = 768
		self.Z_BACKGROUND = -1.1
		self.Z_CONTROLS = self.Z_BACKGROUND - 0.2
		self.DZ = -0.2

		self.Z_HELP_AREA = self.Z_CONTROLS - 2
		self.W_HELP_AREA = 200

		# Panels

		self.iMarginSpace = 15

		self.X_MAIN_PANEL = 17
		self.Y_MAIN_PANEL = 25
		self.W_MAIN_PANEL = 996
		self.H_MAIN_PANEL = 725

		# Upper Panel

		self.X_UPPER_PANEL = self.X_MAIN_PANEL + self.iMarginSpace
		self.Y_UPPER_PANEL = self.Y_MAIN_PANEL + self.iMarginSpace
		self.W_UPPER_PANEL = self.W_MAIN_PANEL - (self.iMarginSpace * 2)
		self.H_UPPER_PANEL = 200

		self.X_TITLE = self.X_MAIN_PANEL + (self.W_MAIN_PANEL / 2)
		self.Y_TITLE = self.Y_UPPER_PANEL - 20

		self.W_ICON = 96 # 90
		self.H_ICON = 96 # 90
#		self.X_ICON = self.X_UPPER_PANEL + 134
#		self.Y_ICON = self.Y_UPPER_PANEL + (self.H_UPPER_PANEL / 2) - (self.H_ICON / 2) + 17

		self.X_ICON_PANEL = self.X_UPPER_PANEL + self.iMarginSpace + 2
		self.Y_ICON_PANEL = self.Y_UPPER_PANEL + self.iMarginSpace + 30
		self.W_ICON_PANEL = 160
		self.H_ICON_PANEL = 105 # self.H_MAIN_PANEL - (self.iMarginSpace * 2)

		self.X_ICON = self.X_ICON_PANEL + self.W_ICON_PANEL / 2 - self.W_ICON / 2
		self.Y_ICON = self.Y_ICON_PANEL + self.H_ICON_PANEL / 2 - self.H_ICON / 2

		self.X_QUOTE = 550
#		self.Y_QUOTE = self.Y_UPPER_PANEL + self.iMarginSpace + 36
		self.Y_QUOTE = self.Y_ICON - 25
		self.W_QUOTE = 455
#		self.H_QUOTE = self.H_UPPER_PANEL - (self.iMarginSpace * 2) - 38
		self.H_QUOTE = 135

#---GeÃ¤ndert START - siehe original Datei -----------------
		# Lower Panel

		self.X_LOWER_PANEL = self.X_MAIN_PANEL + self.iMarginSpace
		self.Y_LOWER_PANEL = self.Y_UPPER_PANEL + self.H_UPPER_PANEL
		self.W_LOWER_PANEL = self.W_MAIN_PANEL - (self.iMarginSpace * 2)
		self.H_LOWER_PANEL = 360

		self.H_ALLOWS_PANEL = 80
		self.H_ALLOWS_SPACE = 28

		self.X_SPECIAL_PANEL = self.X_LOWER_PANEL + self.iMarginSpace + 10
		self.Y_SPECIAL_PANEL = self.Y_LOWER_PANEL + self.iMarginSpace + 20
		self.W_SPECIAL_PANEL = self.W_LOWER_PANEL- 650 - self.iMarginSpace
		self.H_SPECIAL_PANEL = 290 + self.H_ALLOWS_PANEL + self.H_ALLOWS_SPACE

		self.X_ALLOWS_PANELSIR = self.X_LOWER_PANEL + self.iMarginSpace
		self.Y_ALLOWS_PANELSIR = self.Y_SPECIAL_PANEL + self.H_SPECIAL_PANEL + self.H_ALLOWS_SPACE
		self.W_ALLOWS_PANELSIR = self.W_LOWER_PANEL/2 - (self.iMarginSpace)
		self.H_ALLOWS_PANELSIR = 80

		self.X_ALLOWS_PANEL = self.X_LOWER_PANEL + self.iMarginSpace + self.W_SPECIAL_PANEL
		self.Y_ALLOWS_PANEL = self.Y_SPECIAL_PANEL
		self.W_ALLOWS_PANEL = self.W_LOWER_PANEL- 330 - (self.iMarginSpace)
		self.Y_ALLOWS_PANEL2 = self.Y_SPECIAL_PANEL + self.H_ALLOWS_PANEL + self.H_ALLOWS_SPACE
		self.Y_ALLOWS_PANEL3 = self.Y_SPECIAL_PANEL + 2 * (self.H_ALLOWS_PANEL + self.H_ALLOWS_SPACE)
		self.Y_ALLOWS_PANEL4 = self.Y_SPECIAL_PANEL + 3 * (self.H_ALLOWS_PANEL + self.H_ALLOWS_SPACE)
#---GeÃ¤ndert ENDE ------------------------------------------

		# Contents

		self.X_EXIT = self.X_MAIN_PANEL + (self.W_MAIN_PANEL / 2) - 70
		self.Y_EXIT = self.Y_MAIN_PANEL + self.H_MAIN_PANEL - 26
		self.W_EXIT = 120
		self.H_EXIT = 30

	def interfaceScreen(self, iTech):

		self.EXIT_TEXT = TRNSLTR.getText("TXT_KEY_SCREEN_CONTINUE", ())
		self.nTechs = GC.getNumTechInfos()
		self.iTech = iTech
		self.nWidgetCount = 0

		# Create screen
		screen = self.getScreen()

		techInfo = GC.getTechInfo(self.iTech)

		screen.setSound(techInfo.getSound())
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.enableWorldSounds(False)

		screen.addDDSGFC("TechSplashBackground", CyArtFileMgr().getInterfaceArtInfo("TEMP_BG2").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.showWindowBackground(True)
		screen.setDimensions(screen.centerX(0), screen.centerY(0), self.W_SCREEN, self.H_SCREEN)

		# Create panels

		# Main Panel
		# screen.addPanel("TechSplashMainPanel", "", "", True, True, self.X_MAIN_PANEL, self.Y_MAIN_PANEL, self.W_MAIN_PANEL, self.H_MAIN_PANEL, PanelStyles.PANEL_STYLE_MAIN)

		# Icon Panel
		pnl = "IconPanelGlow"
		screen.addPanel(pnl, "", "", True, True, self.X_ICON_PANEL, self.Y_ICON_PANEL, self.W_ICON_PANEL, self.H_ICON_PANEL, PanelStyles.PANEL_STYLE_MAIN_TAN15)
		screen.setStyle(pnl, "Panel_TechDiscoverGlow_Style")

		# Bottom Panel
		pnl = "TechSplashTextPanel"
		screen.addPanel(pnl, "", "", True, True, self.X_LOWER_PANEL+self.iMarginSpace, self.Y_LOWER_PANEL, self.W_LOWER_PANEL-(self.iMarginSpace * 2), self.H_LOWER_PANEL, PanelStyles.PANEL_STYLE_MAIN)
		screen.setStyle(pnl, "Panel_TanT_Style")

		# Exit Button
		screen.setButtonGFC("Exit", TRNSLTR.getText("TXT_KEY_SCREEN_CONTINUE", ()), "", self.X_EXIT, self.Y_EXIT, self.W_EXIT , self.H_EXIT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)

		# Special Panel
		pnl = "TechSplashSpecialPanel"
		screen.addPanel(pnl, "", "", True, True, self.X_SPECIAL_PANEL+self.iMarginSpace, self.Y_SPECIAL_PANEL, self.W_SPECIAL_PANEL-(self.iMarginSpace * 2), self.H_SPECIAL_PANEL + 5, PanelStyles.PANEL_STYLE_IN)
		screen.setStyle(pnl, "Panel_Black25_Style")


#---Eingefügt ENDE -------------------------------------------------

		# Allows Panel
		panelName = self.getNextWidgetName()
		screen.addPanel(panelName, "", "", False, True, self.X_ALLOWS_PANEL+self.iMarginSpace, self.Y_ALLOWS_PANEL, self.W_ALLOWS_PANEL-(self.iMarginSpace * 2), self.H_ALLOWS_PANEL, PanelStyles.PANEL_STYLE_IN)
		screen.setStyle(panelName, "Panel_Black25_Style")

#---Eingefügt START - kann komplett gelöscht werden-----------------
		# Allows Panel2
		panelName2 = "SIR2"
		screen.addPanel(panelName2, "", "", False, True, self.X_ALLOWS_PANEL+self.iMarginSpace, self.Y_ALLOWS_PANEL2, self.W_ALLOWS_PANEL-(self.iMarginSpace * 2), self.H_ALLOWS_PANEL, PanelStyles.PANEL_STYLE_IN)
		screen.setStyle(panelName2, "Panel_Black25_Style")

		# Allows Panel3
		panelName3 = "SIR3"
		screen.addPanel(panelName3, "", "", False, True, self.X_ALLOWS_PANEL+self.iMarginSpace, self.Y_ALLOWS_PANEL3, self.W_ALLOWS_PANEL-(self.iMarginSpace * 2), self.H_ALLOWS_PANEL, PanelStyles.PANEL_STYLE_IN)
		screen.setStyle(panelName3, "Panel_Black25_Style")

		# Allows Panel4
		panelName4 = "SIR4"
		screen.addPanel(panelName4, "", "", False, True, self.X_ALLOWS_PANEL+self.iMarginSpace, self.Y_ALLOWS_PANEL4, self.W_ALLOWS_PANEL-(self.iMarginSpace * 2), self.H_ALLOWS_PANEL, PanelStyles.PANEL_STYLE_IN)
		screen.setStyle(panelName4, "Panel_Black25_Style")
#---Eingefügt ENDE -------------------------------------------------

		# Add Contents

		# Title
		szTech = techInfo.getDescription()
		screen.setLabel(self.getNextWidgetName(), "Background", "<font=4>" + szTech.upper(), 1<<2, self.X_TITLE, self.Y_TITLE, self.Z_CONTROLS, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Tech Icon
		screen.addDDSGFC(self.getNextWidgetName(), techInfo.getButton(), self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH, self.iTech, -1)

		# Tech Quote
		szTechQuote = techInfo.getQuote()
		iTextOffset = 0
		if TechWindowOpt.isShowCivilopediaText():
			szTechQuote += "\n\n" + techInfo.getCivilopedia()
		else:
			iTextOffset = 20
		screen.addMultilineText("Text", szTechQuote, self.X_QUOTE, self.Y_QUOTE + iTextOffset, self.W_QUOTE, self.H_QUOTE - iTextOffset, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		# Special
		szSpecialTitle = "<font=3b>" + TRNSLTR.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ())
		szSpecialTitleWidget = "SpecialTitle"
		screen.setText(szSpecialTitleWidget, "", szSpecialTitle, 1<<0, self.X_SPECIAL_PANEL+self.iMarginSpace, self.Y_SPECIAL_PANEL - 20, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		listName = self.getNextWidgetName()

		szSpecialText = CyGameTextMgr().getTechHelp(self.iTech, True, False, False, True, -1)[1:]
		screen.addMultilineText(listName, szSpecialText, self.X_SPECIAL_PANEL+20, self.Y_SPECIAL_PANEL+20, self.W_SPECIAL_PANEL-50, self.H_SPECIAL_PANEL-30, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

#---Eingefügt START - kann komplett gelöscht werden --------------
		# Allows -> LeadsTo
		szAllowsTitleDescSIR = "<font=3b>" + TRNSLTR.getText("TXT_KEY_PEDIA_LEADS_TO", ()) + ":"
		screen.setText("AllowsTitleSIR", "", szAllowsTitleDescSIR, 1<<0, self.X_ALLOWS_PANEL + self.iMarginSpace, self.Y_ALLOWS_PANEL4 - 20, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		for j in range(GC.getNumTechInfos()):

			for iPrereq in GC.getTechInfo(j).getPrereqOrTechs():
				if self.iTech == iPrereq:
					screen.attachImageButton(panelName4, "", GC.getTechInfo(j).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_DERIVED_TECH, j, self.iTech, False)

			for iPrereq in GC.getTechInfo(j).getPrereqAndTechs():
				if self.iTech == iPrereq:
					screen.attachImageButton(panelName4, "", GC.getTechInfo(j).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_DERIVED_TECH, j, self.iTech, False)

#---EingefÃ¼gt ENDE ------------------------------------------------

		# Units Enabled
		szUnitsTitleDesc = "<font=3b>" + TRNSLTR.getText("TXT_KEY_PEDIA_UNITS_ENABLED", ()) + ":"
		screen.setText("UnitsTitle", "", szUnitsTitleDesc, 1<<0, self.X_ALLOWS_PANEL + self.iMarginSpace, self.Y_ALLOWS_PANEL - 20, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		for iUnit in range(GC.getNumUnitInfos()):
			if isTechRequiredForUnit(self.iTech, iUnit):
				screen.attachImageButton(panelName, "", GC.getUnitInfo(iUnit).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, iUnit, 1, False)

		# Buildings Enabled
		szBuildingsTitleDesc = "<font=3b>" + TRNSLTR.getText("TXT_KEY_PEDIA_BUILDINGS_ENABLED", ()) + ":"
		screen.setText("BuildingsTitle", "", szBuildingsTitleDesc, 1<<0, self.X_ALLOWS_PANEL + self.iMarginSpace, self.Y_ALLOWS_PANEL2 - 20, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		for eLoopBuilding in range(GC.getNumBuildingInfos()):
			bTechFound = 0
			if isTechRequiredForBuilding(self.iTech, eLoopBuilding):
				screen.attachImageButton(panelName2, "", GC.getBuildingInfo(eLoopBuilding).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, eLoopBuilding, 1, False)

		# Improvements
		szImprovesTitleDesc = "<font=3b>" + TRNSLTR.getText("TXT_KEY_PEDIA_CATEGORY_IMPROVEMENT", ()) + ":"
		screen.setText("ImprovesTitle", "", szImprovesTitleDesc, 1<<0, self.X_ALLOWS_PANEL + self.iMarginSpace, self.Y_ALLOWS_PANEL3 - 20, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		for j in range(GC.getNumProjectInfos()):
			bTechFound = 0
			if isTechRequiredForProject(self.iTech, j):
				screen.attachImageButton(panelName3, "", GC.getProjectInfo(j).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT, j, 1, False)

		for j in range(GC.getNumPromotionInfos()):
			if GC.getPromotionInfo(j).getTechPrereq() == self.iTech:
				screen.attachImageButton(panelName3, "", GC.getPromotionInfo(j).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROMOTION, j, 1, False)

#---EingefÃ¼gt START - kann komplett gelÃ¶scht werden --------------

		# Improvements
		for j in range(GC.getNumBuildInfos()):
			bTechFound = 0
			if GC.getBuildInfo(j).getTechPrereq() == -1:
				for k in range(GC.getNumFeatureInfos()):
					if GC.getBuildInfo(j).getFeatureTech(k) == self.iTech:
						bTechFound = 1
			elif GC.getBuildInfo(j).getTechPrereq() == self.iTech:
				bTechFound = 1

			if bTechFound == 1:
				if GC.getBuildInfo(j).getImprovement() == -1:
					screen.attachImageButton(panelName3, "", GC.getBuildInfo(j).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_HELP_IMPROVEMENT, j, 1, False)
				else:
					screen.attachImageButton(panelName3, "", GC.getBuildInfo(j).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_IMPROVEMENT, GC.getBuildInfo(j).getImprovement(), 1, False)

		# Bonuses
		for j in range(GC.getNumBonusInfos()):
			if GC.getBonusInfo(j).getTechReveal() == self.iTech:
				screen.attachImageButton(panelName3, "", GC.getBonusInfo(j).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS, j, 1, False)

		# Civic
		for j in range(GC.getNumCivicInfos()):
			if GC.getCivicInfo(j).getTechPrereq() == self.iTech:
				screen.attachImageButton(panelName3, "", GC.getCivicInfo(j).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIVIC, j, 1, False)

#---Eingefügt ENDE ------------------------------------------------

	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount * self.nTechs + self.iTech)
		self.nWidgetCount += 1
		return szName

	# returns a unique ID for this screen
	def getScreen(self):
		screen = CyGInterfaceScreen(self.SCREEN_NAME + str(self.iTech), self.nScreenId)
		return screen

	def handleInput(self, inputClass):
		if inputClass.getData() == int(InputTypes.KB_RETURN):
			self.getScreen().hideScreen()
			return 1
		if inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
			if inputClass.getFunctionName() == self.EXIT_ID:
				self.getScreen().hideScreen()
			return 1
		return 0

	def update(self, fDelta):
		return
