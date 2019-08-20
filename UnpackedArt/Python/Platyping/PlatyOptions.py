from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

gc = CyGlobalContext()
iBackGround = 1		## 0: Default, 1: Civ Specific, 2: Era Specific
bGreatPeopleBar = True
bCivColors = True
bTransparent = True
bBuildList = False	## 0: Default, 1: Vincent's Style
bFullScreenMovie = False

def getBackGround():
	sBackGround = CyArtFileMgr().getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath()
	pPlayer = gc.getPlayer(CyGame().getActivePlayer())
	sType = ""
	if iBackGround == 1:
		sType = gc.getCivilizationInfo(pPlayer.getCivilizationType()).getType()
	elif iBackGround == 2:
		sType = gc.getEraInfo(pPlayer.getCurrentEra()).getType()
	sNewArt = CyArtFileMgr().getInterfaceArtInfo("ART_DEF_BACKGROUND_" + sType)
	if sNewArt:
		sBackGround = sNewArt.getPath()
	return sBackGround

class PlatyOptions:

	def interfaceScreen(self):
		screen = CyGInterfaceScreen("PlatyOptions", CvScreenEnums.PLATY_OPTIONS)

		iWidth = screen.getXResolution()/3
		iX = screen.getXResolution()/2 - iWidth/2
		iY = 100
		iHeight = screen.getYResolution() - 150 - iY
		screen.setRenderInterfaceOnly(True)
		screen.setDimensions(iX, iY, iWidth, iHeight)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.addPanel("PlatyOptionsBG", u"", u"", True, False, 0, 0, iWidth, iHeight, PanelStyles.PANEL_STYLE_MAIN)
		screen.setText("WorldExit", "Background", u"<font=4b>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, iWidth - 25,iHeight - 45, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )
		screen.setText("WorldHeader", "Background", u"<font=4b>" + CyTranslator().getText("TXT_KEY_PLATY_OPTIONS", ()).upper() + "</font>", CvUtil.FONT_CENTER_JUSTIFY, iWidth/2, 20, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iX = 20
		iY = 50
		sText = "<font=3b>" + CyTranslator().getText("TXT_KEY_GREAT_PEOPLE_BAR", ()) + "</font>"
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if bGreatPeopleBar:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setText("GreatPeopleBar", "Background", sColor + sText + "</color>", CvUtil.FONT_LEFT_JUSTIFY, iX, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		sText = "<font=3b>" + CyTranslator().getText("TXT_KEY_CIV_SPECIFIC_COLOR_TEXT", ()) + "</font>"
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if bCivColors:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setText("CivSpecificColors", "Background", sColor + sText + "</color>", CvUtil.FONT_LEFT_JUSTIFY, iX, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		sText = "<font=3b>" + CyTranslator().getText("TXT_KEY_TRANSPARENT_PANEL", ()) + "</font>"
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if bTransparent:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setText("TransparentPanels", "Background", sColor + sText + "</color>", CvUtil.FONT_LEFT_JUSTIFY, iX, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		sText = "<font=3b>" + CyTranslator().getText("TXT_KEY_BUILD_LIST", ()) + "</font>"
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if bBuildList:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setText("BuildList", "Background", sColor + sText + "</color>", CvUtil.FONT_LEFT_JUSTIFY, iX, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		sText = "<font=3b>" + CyTranslator().getText("TXT_KEY_FULLSCREEN_MOVIE", ()) + "</font>"
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if bFullScreenMovie:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setText("FullScreenMovie", "Background", sColor + sText + "</color>", CvUtil.FONT_LEFT_JUSTIFY, iX, iY, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iY += 30
		screen.addDropDownBoxGFC("BackgroundChoice", iX, iY, 200, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("BackgroundChoice", CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE",()), 0, 0, 0 == iBackGround)
		screen.addPullDownString("BackgroundChoice", CyTranslator().getText("TXT_KEY_CIV_SPECIFIC_BACKGROUND",()), 1, 1, 1 == iBackGround)
		screen.addPullDownString("BackgroundChoice", CyTranslator().getText("TXT_KEY_ERA_SPECIFIC_BACKGROUND",()), 2, 2, 2 == iBackGround)

	def handleInput (self, inputClass):
		screen = CyGInterfaceScreen("PlatyOptions", CvScreenEnums.PLATY_OPTIONS)
		global iBackGround
		global bGreatPeopleBar
		global bCivColors
		global bTransparent
		global bBuildList
		global bFullScreenMovie

		if inputClass.getFunctionName() == "BackgroundChoice":
			iBackGround = screen.getPullDownData("BackgroundChoice", screen.getSelectedPullDownID("BackgroundChoice"))
			self.interfaceScreen()
		elif inputClass.getFunctionName() == "GreatPeopleBar":
			bGreatPeopleBar = not bGreatPeopleBar
			self.interfaceScreen()
		elif inputClass.getFunctionName() == "CivSpecificColors":
			bCivColors = not bCivColors
			self.interfaceScreen()
		elif inputClass.getFunctionName() == "TransparentPanels":
			bTransparent = not bTransparent
			self.interfaceScreen()
		elif inputClass.getFunctionName() == "BuildList":
			bBuildList = not bBuildList
			self.interfaceScreen()
		elif inputClass.getFunctionName() == "FullScreenMovie":
			bFullScreenMovie = not bFullScreenMovie
			self.interfaceScreen()
		return 0

	def update(self, fDelta):
		return 1