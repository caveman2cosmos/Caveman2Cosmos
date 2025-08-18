##
## Event Manager component of the Great Person is born Screen
##  by Sparth

from CvPythonExtensions import *


def init():
	import CvScreenEnums
	global g_iScreen
	g_iScreen = CvScreenEnums.GREAT_PEOPLE_SCREEN

init()

# globals
GC = CyGlobalContext()

g_GreatPeople = {
	"UNIT_PROPHET"			: "ART_DEF_BACKGROUND_PROPHET",
	"UNIT_ARTIST"			: "ART_DEF_BACKGROUND_ARTIST",
	"UNIT_SCIENTIST"		: "ART_DEF_BACKGROUND_SCIENTIST",
	"UNIT_MERCHANT"			: "ART_DEF_BACKGROUND_MERCHANT",
	"UNIT_GREAT_ENGINEER"	: "ART_DEF_BACKGROUND_ENGINEER",
	"UNIT_GREAT_GENERAL"	: "ART_DEF_BACKGROUND_GGENERAL",
	"UNIT_GREAT_ADMIRAL"	: "ART_DEF_BACKGROUND_ADMIRAL",
	"UNIT_GREAT_HUNTER"		: "ART_DEF_BACKGROUND_HUNTER",
	"UNIT_GREAT_SPY"		: "ART_DEF_BACKGROUND_SPY",
	"UNIT_GREAT_STATESMAN"	: "ART_DEF_BACKGROUND_STATESMAN",
	"UNIT_DOCTOR"			: "ART_DEF_BACKGROUND_DOCTOR"
}

def onGreatPersonBorn(argsList):

	CyUnit, iPlayer, CyCity, = argsList
	if CyGame().isNetworkMultiPlayer() or CyGame().isPitbossHost():
		return

	if iPlayer != -1 and iPlayer == CyGame().getActivePlayer() :
		
		if GC.getPlayer(iPlayer).isHumanDisabled():
			return

		sUnitName = CyUnit.getNameNoDesc()
		if sUnitName == "": return

		# Get screen resolution.
		import ScreenResolution as SR
		xRes = SR.x

		W_MAIN_PANEL = 660
		H_MAIN_PANEL = 452
		X_MAIN_PANEL = xRes/2 - W_MAIN_PANEL/2
		Y_MAIN_PANEL = 70

		iMarginSpace = 15

		W_EXIT = 120
		H_EXIT = 30
		X_EXIT = (xRes - W_EXIT)/2
		Y_EXIT = 466

		W_PERSON_PORTRAIT = 220
		H_PERSON_PORTRAIT = 360
		X_PERSON_PORTRAIT = X_MAIN_PANEL + iMarginSpace
		Y_PERSON_PORTRAIT =  Y_MAIN_PANEL + iMarginSpace

		W_BACKGROUND = 660
		H_BACKGROUND = 452
		X_BACKGROUND = X_MAIN_PANEL
		Y_BACKGROUND =  Y_MAIN_PANEL

		X_TEXT_PANEL = X_PERSON_PORTRAIT + W_PERSON_PORTRAIT + iMarginSpace
		Y_TEXT_PANEL = Y_PERSON_PORTRAIT + 95
		W_TEXT_PANEL = 355
		H_TEXT_PANEL = 250

		iType = CyUnit.getUnitType()
		Info = CyGlobalContext().getUnitInfo(iType)
		sGreat = ""
		for i in xrange(Info.getNumUnitNames()):
			sName = Info.getUnitNames(i)
			if CyTranslator().getText(sName, ()) == sUnitName:
				sGreat = sName[8:]
				break
		else: return

		sPortrait = ""
		artDef = CyArtFileMgr().getInterfaceArtInfo("ART_DEF_" + sGreat)
		if artDef:
			sPortrait = artDef.getPath()
		sBack = ""
		sType = Info.getType()
		if sType in g_GreatPeople:
			sIcon = g_GreatPeople[sType]
			artDef = CyArtFileMgr().getInterfaceArtInfo(sIcon)
			if artDef:
				sBack = artDef.getPath()
		#sQuote = CyTranslator().getText("TXT_KEY_" + sGreat + "_QUOTE", ())
		sText = "<color=136,94,43,255>" + "<font=2b>" + CyTranslator().getText("TXT_KEY_" + sGreat + "_PEDIA", ())

		screen = CyGInterfaceScreen("Great People Screen", g_iScreen)
		screen.showScreen(PopupStates.POPUPSTATE_QUEUED, False)
		screen.showWindowBackground(False)
		screen.addPanel("MainPanel", "", "", true, true, X_MAIN_PANEL, Y_MAIN_PANEL, W_MAIN_PANEL, H_MAIN_PANEL, PanelStyles.PANEL_STYLE_MAIN)
		if sBack:
			screen.addDDSGFC("ScreenBackground", sBack, X_BACKGROUND, Y_BACKGROUND, W_BACKGROUND, H_BACKGROUND, WidgetTypes.WIDGET_GENERAL, 1, 1)
		screen.addPanel("GreatPersonTextPanel", "", "", true, true, X_TEXT_PANEL, Y_TEXT_PANEL, W_TEXT_PANEL, H_TEXT_PANEL, PanelStyles.PANEL_STYLE_MAIN)
		screen.setStyle("GreatPersonTextPanel", "Panel_TechDiscover_Style")
		if sPortrait:
			screen.addDDSGFC("GreatPersonPortrait", sPortrait, X_PERSON_PORTRAIT, Y_PERSON_PORTRAIT, W_PERSON_PORTRAIT, H_PERSON_PORTRAIT, WidgetTypes.WIDGET_GENERAL, 1, 1)
		screen.addMultilineText("GreatPersonText", sText, X_TEXT_PANEL + iMarginSpace, Y_TEXT_PANEL, W_TEXT_PANEL - (iMarginSpace * 1), H_TEXT_PANEL - (iMarginSpace * 1), WidgetTypes.WIDGET_GENERAL, 1, 1, 1<<0)
		screen.setButtonGFC("Exit", CyTranslator().getText("TXT_KEY_SCREEN_CONTINUE", ()), "", X_EXIT, Y_EXIT, W_EXIT, H_EXIT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)

