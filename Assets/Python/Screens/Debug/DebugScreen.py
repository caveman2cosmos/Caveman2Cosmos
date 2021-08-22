#
# DebugScreen
#
from CvPythonExtensions import *

class DebugScreen:

	def __init__(self, screenId):
		self.screenId = screenId

	def getScreen(self):
		return CyGInterfaceScreen("DebugScreen", self.screenId)

	def interfaceScreen(self):
		self.TRNSLTR = TRNSLTR = CyTranslator()
		self.nWidgetCount = 0

		import PythonToolTip
		self.tooltip = PythonToolTip.PythonToolTip()
		# Get screen resolution.
		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y
		# Calibrate variables.
		if yRes > 1000:
			H_BOT_ROW = 110
			self.H_EDGE_PANEL = H_EDGE_PANEL = 38
			szfontEdge = "<font=4b>"
		elif yRes > 800:
			H_BOT_ROW = 92
			self.H_EDGE_PANEL = H_EDGE_PANEL = 32
			szfontEdge = "<font=3b>"
		else:
			H_BOT_ROW = 78
			self.H_EDGE_PANEL = H_EDGE_PANEL = 29
			szfontEdge = "<font=2b>"

		if xRes > 1700:
			self.aFontList = [szfontEdge, "<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1>"]
			self.iTestCodeColumns = 4
			self.hTestBtn = 32
		elif xRes > 1400:
			self.aFontList = [szfontEdge, "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0>"]
			self.iTestCodeColumns = 3
			self.hTestBtn = 28
		else:
			self.aFontList = [szfontEdge, "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0b>", "<font=0>", "<font=0>"]
			self.iTestCodeColumns = 2
			self.hTestBtn = 24

		self.wTestBtn = (xRes - 16*(self.iTestCodeColumns+1)) / self.iTestCodeColumns

		self.Y_TOP_PAGE = Y_TOP_PAGE = H_EDGE_PANEL - 6
		self.H_PAGE = H_PAGE = yRes - Y_TOP_PAGE - H_EDGE_PANEL + 8
		self.Y_BOT_TEXT = Y_BOT_TEXT = yRes - H_EDGE_PANEL + 8
		# Declare misc variables
		self.eWidGen = eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		# Build Debug screen.
		screen = self.getScreen()
		screen.setRenderInterfaceOnly(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.addPanel("DS_Background", "", "", True, True, -8, -8, xRes+16, yRes+16, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel("DS_TopPanel", "", "", True, False, 0, 0, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("DS_TopPanel", "", "", True, False, 0, 0, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("DS_BotPanel", "", "", True, False, 0, yRes - H_EDGE_PANEL, xRes, H_EDGE_PANEL, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.setLabel("DS_Header", "", szfontEdge + "Debug", 1<<2, xRes / 2, 2, 0, eFontTitle, eWidGen, 1, 2)
		screen.setText("DS_Btn_Exit", "", szfontEdge + TRNSLTR.getText("TXT_WORD_EXIT",()), 1<<1, xRes - 16, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)
		screen.setText("TestCodeLogExit0", "", szfontEdge + TRNSLTR.getText("TXT_WORD_EXIT",()), 1<<1, xRes - 16, 0, 0, eFontTitle, eWidGen, 1, 2)
		screen.hide("TestCodeLogExit0")

		szTxt = szfontEdge + "Test Code"
		iCol = CyGlobalContext().getCOLOR_YELLOW()
		szTxtCol = TRNSLTR.changeTextColor(szTxt, iCol)
		dX = xRes
		x = dX / 2
		screen.setText("DS_Tab0", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 1, 2)
		screen.setText("DS_Col_Tab0", "", szTxtCol, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 1, 2)
		screen.hide("DS_Tab0")

		import TestCode

		self.tabList = [
			TestCode.TestCode(self)
		]
		self.initTab(screen, 0)


	def initTab(self, screen, iTab):
		self.iTab = iTab
		if iTab == 0:
			self.iTestNum = 0
			self.testCodes = []
			self.yTestBtn = 0
			self.testCodeScroll = scroll = self.getName()
			screen.addScrollPanel(scroll, "", 0, self.Y_TOP_PAGE, self.xRes + 6, self.H_PAGE - 24, PanelStyles.PANEL_STYLE_HUD_HELP)
			screen.setStyle(scroll, "ScrollPanel_Alt_Style")

		self.tabList[iTab].drawTab(screen)


	def addTestCode(self, screen, pTest, label, tip):
		xTestBtn = (self.iTestNum % self.iTestCodeColumns) * (self.wTestBtn + 16)
		ScPnl = self.testCodeScroll
		screen = self.getScreen()
		Btn = "TestCode" + str(self.iTestNum)
		screen.setImageButtonAt(Btn, ScPnl, "", xTestBtn, self.yTestBtn, self.wTestBtn, self.hTestBtn, self.eWidGen, 1, 2)
		screen.setStyle(Btn, "SF_CtrlTheme_Civ4_Control_Button_Dark_Style")
		Lbl = self.getName()
		screen.setLabelAt(Lbl, Btn, self.aFontList[4] + label, 1<<0, 8, 3, 0, FontTypes.GAME_FONT, self.eWidGen, 1, 2)
		screen.setHitTest(Lbl, HitTestTypes.HITTEST_NOHIT)
		self.testCodes.append((pTest, label, tip))
		self.iTestNum += 1
		if not (self.iTestNum % self.iTestCodeColumns):
			self.yTestBtn += self.hTestBtn + 16


	def runTestCode(self, iTest):
		print "----------------------------------------------\n Test " + str(iTest) + " - " + self.testCodes[iTest][1] + "\n\n:: " + self.testCodes[iTest][2] + "\n----------------------------------------------"
		screen = self.getScreen()
		screen.addPanel("TestCodelogPnlA", "", "", True, True, -8, -8, self.xRes+16, self.yRes+16, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel("TestCodelogPnlB", "", "", True, True, -8, -8, self.xRes+16, self.yRes+16, PanelStyles.PANEL_STYLE_HUD_HELP)

		# On-Screen-Log Header
		self.testCodeLog = (
			self.aFontList[3] + "Test " + str(iTest) + " - " + self.testCodes[iTest][1]
			+ self.aFontList[5] + "\n:: " + self.testCodes[iTest][2]
			+ self.aFontList[6] + "\n----------------------------------------------"
		)
		self.testCodes[iTest][0]()
		screen.addMultilineText("TestCodelog", self.testCodeLog, 8, 24, self.xRes - 14, self.yRes - 40, self.eWidGen, 1, 2, 1<<0)

		screen.moveToFront("TestCodeLogExit0")
		screen.show("TestCodeLogExit0")
		print "----------------------------------------------\n Finished Test %d - %s\n----------------------------------------------" % (iTest, self.testCodes[iTest][1])

	def testCodelog(self, text):
		print "[TEST] " + text
		self.testCodeLog = self.testCodeLog + "\n* " + text

	def closeLog(self, screen):
		screen.deleteWidget("TestCodelog"); screen.deleteWidget("TestCodelogPnlB"); screen.deleteWidget("TestCodelogPnlA")
		screen.hide("TestCodeLogExit0")
		self.testCodeLog = ""


	# returns a unique ID for a widget in this screen
	def getName(self):
		szName = "DS_Widget" + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def deleteAllWidgets(self, screen, aName):
		i = self.nWidgetCount - 1
		while i >= 0:
			self.nWidgetCount = i
			screen.deleteWidget(aName())
			i -= 1
		self.nWidgetCount = 0

	def handleInput(self, inputClass):
		screen = self.getScreen()

		iCode	= inputClass.eNotifyCode
		ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName

		self.tooltip.reset(screen)

		if iCode == 4: # Mouse Enter

			if self.iTab == 0:
				if NAME == "TestCode":
					self.tooltip.handle(screen, self.testCodes[ID][2])

		elif not iCode: # Click
			if NAME == "DS_Tab":
				screen.hide("DS_Col_Tab" + str(self.iTab))
				screen.show("DS_Tab" + str(self.iTab))
				screen.hide("DS_Tab" + str(ID))
				screen.show("DS_Col_Tab" + str(ID))
				self.deleteAllWidgets(screen, aName)
				self.initTab(screen, ID)

			elif self.iTab == 0:

				if NAME == "TestCode":
					self.runTestCode(ID)

				elif "TestCodeLogExit":
					self.closeLog(screen)

	def update(self, fDelta): return

	def onClose(self):
		self.getScreen().setDying(True)
		del self.tooltip, self.eWidGen, self.nWidgetCount, self.aFontList, self.iTab, self.tabList, \
			self.xRes, self.yRes, self.H_EDGE_PANEL, self.Y_TOP_PAGE, self.H_PAGE, self.Y_BOT_TEXT, \
			self.iTestCodeColumns, self.hTestBtn, self.wTestBtn, self.yTestBtn, self.testCodeScroll, self.iTestNum, self.testCodeLog