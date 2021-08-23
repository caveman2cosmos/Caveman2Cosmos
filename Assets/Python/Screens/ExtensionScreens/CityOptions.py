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

		self.TRNSLTR = CyTranslator()
		afm = CyArtFileMgr()
		self.artPathCheckbox0 = afm.getInterfaceArtInfo("ART_DEF_CHECKBOX_UNCHECKED").getPath()
		self.artPathCheckbox1 = afm.getInterfaceArtInfo("ART_DEF_CHECKBOX_CHECKED").getPath()

		self.drawOptions(screen, mainInterface)

	def drawOptions(self, screen, mainInterface):
		xRes = self.xRes
		yRes = self.yRes
		aFontList = self.aFontList

		TRNSLTR = self.TRNSLTR
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
		self.iSizeCB = iSizeCB = ySpace - (ySpace % 4) - 8
		szCheckbox0 = self.artPathCheckbox0
		szCheckbox1 = self.artPathCheckbox1

		screen.addScrollPanel(scrlPnl, "", x0, y0 + ySpace, dx, dy - 3*ySpace, ePnlMain)
		screen.setStyle(scrlPnl, "ScrollPanel_Alt_Style")
		y = 0
		screen.setTextAt("CO_IconSize0", scrlPnl, aFontList[4] + TRNSLTR.getText("TXT_OPTION_CITY_ICON_SIZE", (mainInterface.CityOpt.getBuildIconSize(),)), 1<<0, 0, y, 0, eFontGame, eWidGen, 1, 1)

		y += ySpace
		self.yHideUnconstructableBuildings = y
		if mainInterface.CityOpt.isHideUnconstructableBuildings():
			img = szCheckbox1
			self.bHideUnconstructableBuildings = True
		else:
			img = szCheckbox0
			self.bHideUnconstructableBuildings = False

		screen.setImageButtonAt("HideUnconstructableBuildings0", scrlPnl, img, 0, y, iSizeCB, iSizeCB, eWidGen, 1, 2)
		szTxt = aFontList[4] + TRNSLTR.getText("TXT_OPTION_CITY_HIDE_UNCONSTRUCTABLE_BUILDINGS", ())
		screen.setTextAt("HideUnconstructableBuildings1", scrlPnl, szTxt, 1<<0, iSizeCB + 4, y, 0, eFontGame, eWidGen, 1, 2)

		y += ySpace
		self.yHideUntrainableUnits = y
		if mainInterface.CityOpt.isHideUntrainableUnits():
			img = szCheckbox1
			self.bHideUntrainableUnits = True
		else:
			img = szCheckbox0
			self.bHideUntrainableUnits = False

		screen.setImageButtonAt("HideUntrainableUnits0", scrlPnl, img, 0, y, iSizeCB, iSizeCB, eWidGen, 1, 2)
		szTxt = aFontList[4] + TRNSLTR.getText("TXT_OPTION_CITY_HIDE_UNTRAINABLE_UNITS", ())
		screen.setTextAt("HideUntrainableUnits1", scrlPnl, szTxt, 1<<0, iSizeCB + 4, y, 0, eFontGame, eWidGen, 1, 2)

		# Exit button at the end.
		iBtnDx = 32 + xRes / 60
		screen.setButtonGFC("ExitCO", aFontList[2] + "OK", "", x0 + iMidX - iBtnDx/2, y0 + dy - ySpace - 4, iBtnDx, ySpace, eWidGen, 1, 2, eBtnStd)


	def handleInput(self, screen, inputClass, mainInterface):
		NAME = inputClass.szFunctionName
		iCode = inputClass.eNotifyCode

		mainInterface.hideTooltip(screen)

		if iCode == 16: # Key Down
			if inputClass.iData in (1, 44): # Esc and Enter
				self.exit(screen, mainInterface)

		elif iCode == 4: # Mouse Enter
			if NAME == "CO_IconSize":
				mainInterface.updateTooltip(screen, self.TRNSLTR.getText("TXT_OPTION_CITY_ICON_SIZE_HELP", ()))

			elif NAME == "HideUnconstructableBuildings":
				mainInterface.updateTooltip(screen, self.TRNSLTR.getText("TXT_OPTION_CITY_HIDE_UNCONSTRUCTABLE_BUILDINGS_HELP", ()))

			elif NAME == "HideUntrainableUnits":
				mainInterface.updateTooltip(screen, self.TRNSLTR.getText("TXT_OPTION_CITY_HIDE_UNTRAINABLE_UNITS_HELP", ()))

		elif not iCode: # click
			if NAME == "ExitCO":
				self.exit(screen, mainInterface)

			elif NAME == "CO_IconSize":
				popup = CyPopup(4999, EventContextTypes.EVENTCONTEXT_SELF, True)
				popup.setPosition(self.xRes/3, self.yRes/3)
				popup.createSpinBox(0, "", mainInterface.CityOpt.getBuildIconSize(), 4, 128, 32)
				popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

			elif NAME == "HideUnconstructableBuildings":
				bNewState = not self.bHideUnconstructableBuildings
				self.bHideUnconstructableBuildings = bNewState
				mainInterface.CityOpt.setHideUnconstructableBuildings(bNewState)
				if bNewState:
					img = self.artPathCheckbox1
				else: img = self.artPathCheckbox0
				screen.setImageButtonAt("HideUnconstructableBuildings0", "ScrollCO", img, 0, self.yHideUnconstructableBuildings, self.iSizeCB, self.iSizeCB, WidgetTypes.WIDGET_GENERAL, 1, 2)

			elif NAME == "HideUntrainableUnits":
				bNewState = not self.bHideUntrainableUnits
				self.bHideUntrainableUnits = bNewState
				mainInterface.CityOpt.setHideUntrainableUnits(bNewState)
				if bNewState:
					img = self.artPathCheckbox1
				else: img = self.artPathCheckbox0
				screen.setImageButtonAt("HideUntrainableUnits0", "ScrollCO", img, 0, self.yHideUntrainableUnits, self.iSizeCB, self.iSizeCB, WidgetTypes.WIDGET_GENERAL, 1, 2)

		return 1 # Dominant screen, consumes all input.


	def exit(self, screen, mainInterface):
		screen.deleteWidget("ExitCO")
		screen.deleteWidget("ScrollCO")
		screen.deleteWidget("CityOptionBG")
		mainInterface.cityOptions = None
		import BugOptions
		BugOptions.getOptions(mainInterface.CityOpt._id).write()

