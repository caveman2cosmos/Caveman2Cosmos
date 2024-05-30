from CvPythonExtensions import *
import HandleInputUtil
import PythonToolTip

GC = CyGlobalContext()
GAME = CyGame()
AFM = CyArtFileMgr()
#GTM = CyGameTextMgr()
TRNSLTR = CyTranslator()


class ParallelMapsScreen:

	def __init__(self, screenId):
		self.screenId = screenId

	def getScreen(self):
		return CyGInterfaceScreen("ParallelMapsScreen", self.screenId)

	def interfaceScreen(self):
		screen = self.getScreen()
		if screen.isActive():
			return

		self.tooltip = PythonToolTip.PythonToolTip()

		# Resolution
		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y

		if yRes > 1000:
			self.H_EDGE_PANEL = H_EDGE_PANEL = 42
			uFontEdge = "<font=4b>"
		elif yRes > 800:
			self.H_EDGE_PANEL = H_EDGE_PANEL = 34
			uFontEdge = "<font=3b>"
		else:
			self.H_EDGE_PANEL = H_EDGE_PANEL = 30
			uFontEdge = "<font=2b>"

		self.a4thX = a4thX = xRes / 4

		eWidGen = WidgetTypes.WIDGET_GENERAL
		#ePnlBlue50 = PanelStyles.PANEL_STYLE_BLUE50
		eFontTitle = FontTypes.TITLE_FONT

		screen.setRenderInterfaceOnly(True);
		#screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.showWindowBackground(False)
		screen.setDimensions(0, 0, xRes, yRes)

		screen.addDDSGFC("BackgroundImage", AFM.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, xRes, yRes, eWidGen, -1, -1)
		screen.addPanel("TopPanel", "", "", True, False, -2, -2, xRes+4, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_TOPBAR)
		stTxt = uFontEdge + TRNSLTR.getText("TXT_WORD_EXIT", ())
		screen.setText("Exit", "", stTxt, 1<<1, xRes - 8, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		# Header
		screen.setLabel("Header", "", uFontEdge + "Maps", 1<<2, xRes / 2, 2, 0, eFontTitle, eWidGen, 1, 1)

		if GC.isDebugBuild(): # Matt: minimap is glitchy, needs fixing.
			self.initMinimap(screen)

		self.UL_refresh(screen, True)

		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

	def initMinimap(self, screen):
		self.Y_MAP = Y_MAP = self.H_EDGE_PANEL + 90
		self.W_MAP = W_MAP = self.xRes - self.a4thX - 16
		H_MAP_MAX = self.yRes - Y_MAP - 10

		self.yMainPnl = yMainPnl = Y_MAP - 12

		MAP = CyMap()

		iMap_W = MAP.getViewportWidth()
		iMap_H = MAP.getViewportHeight()

		self.H_MAP = H_MAP = int(W_MAP * iMap_H * 1.0 / iMap_W)
		if H_MAP > H_MAP_MAX:
			self.W_MAP = W_MAP = int(H_MAP_MAX * iMap_W * 1.0 / iMap_H)
			self.H_MAP = H_MAP = H_MAP_MAX

		screen.addPanel("MA_PnlMM", "", "", False, False, 8, yMainPnl, W_MAP + 12, H_MAP + 18, PanelStyles.PANEL_STYLE_BLUE50)
		screen.initMinimap(14, 14 + W_MAP, Y_MAP, Y_MAP + H_MAP, 0)

		if GAME.isDebugMode():
			screen.updateMinimapSection(True, True)
		else:
			screen.updateMinimapSection(False, False)

		screen.updateMinimapColorFromMap(MinimapModeTypes.MINIMAPMODE_TERRITORY, 0.3)
		screen.setMinimapMode(MinimapModeTypes.MINIMAPMODE_MILITARY)
		screen.updateMinimapVisibility()

	def UL_refresh(self, screen, bReload):
		xRes = self.xRes
		yTop = self.H_EDGE_PANEL
		self.HILITE_SQUARE = HILITE_SQUARE = AFM.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath()

		eWidGen = WidgetTypes.WIDGET_GENERAL
		ePnlBlue50	= PanelStyles.PANEL_STYLE_BLUE50

		# Set scrollable area for maps
		y = yTop - 4
		w = xRes - 16
		screen.addPanel("MA_CivSelBG", "", "", False, True, 8, y, w, 88, ePnlBlue50)
		ScPnl = "MA_CivSelPnl"
		screen.addScrollPanel(ScPnl, "", 8, y, w, 64, ePnlBlue50)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")

		# loop through all maps and display buttons
		x = 4
		dx = 72
		for mapX in GC.getMaps():
			if mapX.plotsInitialized():
				BTN = GC.getMapInfo(mapX.getType()).getButton()

				ID = str(mapX.getType())
				screen.setImageButtonAt("WID|MAP|Base" + ID, ScPnl, BTN, x, 5, 64, 64, eWidGen, 1, 1)
				Img = "WID|MAP|HiLi" + ID
				screen.setImageButtonAt(Img, ScPnl, HILITE_SQUARE, x-3, 2, 70, 70, eWidGen, 1, 1)

				if mapX.getType() != GAME.getCurrentMap():
					screen.hide(Img)
				x += dx

	def handleInput(self, inputClass):
		screen = self.getScreen()
		if not screen.isActive():
			return
		#HandleInputUtil.debugInput(inputClass)
		#bAlt, bCtrl, bShift = self.InputData.getModifierKeys()
		iCode	= inputClass.eNotifyCode
		iData	= inputClass.iData
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName
		iBtn	= inputClass.iButtonType
		iData1	= inputClass.iData1
		iData2	= inputClass.iData2
		szFlag	= HandleInputUtil.MOUSE_FLAGS.get(inputClass.uiFlags, "UNKNOWN")

		szSplit = NAME.split("|")
		BASE = szSplit[0]
		if szSplit[1:]:
			TYPE = szSplit[1]
		else:
			TYPE = ""
		if szSplit[2:]:
			CASE = szSplit[2:]
		else:
			CASE = [0]

		# Remove potential Help Text
		self.tooltip.reset(screen)

		if iCode == NotifyCode.NOTIFY_CURSOR_MOVE_ON:
			if BASE == "WID":
				if TYPE == "MAP":
					szTxt = GC.getMapInfo(ID).getDescription()
					self.tooltip.handle(screen, szTxt)

		if iCode == NotifyCode.NOTIFY_CLICKED:
			if szFlag == "MOUSE_LBUTTONUP":
				if BASE == "WID":
					if TYPE == "MAP":
						if ID != GAME.getCurrentMap():
							screen.hide("WID|MAP|HiLi" + str(int(GAME.getCurrentMap())))
							screen.show("WID|MAP|HiLi" + str(ID))
							GC.switchMap(ID) 
							#self.hideScreen()
							#self.interfaceScreen()
							self.initMinimap(screen)
							CyMap().setViewportActionState(ViewportDeferredActionState.VIEWPORT_ACTION_STATE_AFTER_SWITCH)

	def update(self, fDelta):
		CyMap().setViewportActionState(ViewportDeferredActionState.VIEWPORT_ACTION_STATE_AFTER_SWITCH)

	def hideScreen(self):
		screen = self.getScreen()
		screen.hideScreen()

	def onClose(self):
		del self.xRes, self.yRes
		CyMap().setViewportActionState(ViewportDeferredActionState.VIEWPORT_ACTION_STATE_AFTER_SWITCH)