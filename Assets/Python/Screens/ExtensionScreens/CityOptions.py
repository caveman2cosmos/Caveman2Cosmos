#
# Toffer - Option screen for city related options.
#
from CvPythonExtensions import *

class CityOptions:
	def __init__(self, screen, mainInterface):
		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y
		self.aFontList = aFontList = SR.aFontList
		self.drawOptions(screen, mainInterface)

	def drawOptions(self, screen, mainInterface):
		xRes = self.xRes
		yRes = self.yRes
		aFontList = self.aFontList

		TRNSLTR = CyTranslator()
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT
		ePnlMain = PanelStyles.PANEL_STYLE_MAIN
		eBtnLabel = ButtonStyles.BUTTON_STYLE_LABEL
		eBtnStd = ButtonStyles.BUTTON_STYLE_STANDARD
		dx = 128 + xRes / 4
		dy = yRes / 2
		x0 = (xRes - dx) / 2
		y0 = (yRes - dy) / 2
		iMidX = dx / 2
		BG = "CityOptionBG"
		screen.addPanel(BG, "", "", False, False, x0, y0, dx, dy, ePnlMain)
		screen.setStyle(BG, "SF_CtrlTheme_Civ4_Control_Panel_Black80_Style")
		screen.setLabelAt("", BG, aFontList[2] + "City Options", 1<<2, iMidX, 8, 0, eFontGame, eWidGen, 1, 2)
		scrlPnl = "ScrollCO"
		ySpace = 16 + yRes / 60

		screen.addScrollPanel(scrlPnl, "", x0, y0 + ySpace, dx, dy - 3*ySpace, ePnlMain)
		screen.setStyle(scrlPnl, "ScrollPanel_Alt_Style")
		y = 0
		screen.setTextAt("IconSizeCO", scrlPnl, aFontList[4] + TRNSLTR.getText("TXT_KEY_ICON_SIZE", (mainInterface.CityOpt.getBuildIconSize(),)), 1<<0, 0, y, 0, eFontGame, eWidGen, 1, 1)
		#y += ySpace

		iBtnDx = 32 + xRes / 60
		screen.setButtonGFC("ExitCO", aFontList[2] + "OK", "", x0 + iMidX - iBtnDx/2, y0 + dy - ySpace - 4, iBtnDx, ySpace, eWidGen, 1, 2, eBtnStd)

	def moveToFront(self, screen):
		screen.moveToFront("CityOptionBG")
		screen.moveToFront("ScrollCO")
		screen.moveToFront("ExitCO")


	def handleInput(self, screen, inputClass, mainInterface):
		NAME = inputClass.szFunctionName
		iCode = inputClass.eNotifyCode

		if iCode == 16: # Key Down
			if inputClass.iData in (1, 44): # Esc and Enter
				self.exit(screen, mainInterface)

		elif iCode == 4: # Mouse Enter
			pass

		elif not iCode: # click
			if NAME == "ExitCO":
				self.exit(screen, mainInterface)
			elif NAME == "IconSizeCO":
				popup = CyPopup(4999, EventContextTypes.EVENTCONTEXT_SELF, True)
				popup.setPosition(self.xRes/3, self.yRes/3)
				popup.createSpinBox(0, "", mainInterface.CityOpt.getBuildIconSize(), 4, 128, 32)
				popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
		return 1

	def exit(self, screen, mainInterface):
		screen.deleteWidget("ExitCO")
		screen.deleteWidget("ScrollCO")
		screen.deleteWidget("CityOptionBG")
		mainInterface.cityOptions = None

