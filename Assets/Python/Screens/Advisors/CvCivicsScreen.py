
from CvPythonExtensions import *
import CvScreensInterface as UP
import HandleInputUtil
import CivicData

# globals
GC = CyGlobalContext()
AFM = CyArtFileMgr()
GTM = CyGameTextMgr()
TRNSLTR = CyTranslator()
GAME = GC.getGame()

class CvCivicsScreen:

	def __init__(self, screenId):
		self.screenId = screenId

		import SystemPaths as SP
		path = SP.modDir + "\UserSettings\CivicScreen.txt"

		if SP.isFile(path):
			from string import split
			file = open(path, 'r')
			self.iTab = int(file.readline().split("=")[1])
			self.iType = int(file.readline().split("=")[1])
			file.close()
		else:
			self.iTab = 0
			self.iType = 1
			file = open(path, "w")
			file.write("Tab = 0\nType = 1")
			file.close()

	def getScreen(self):
		return CyGInterfaceScreen("CivicsScreen", self.screenId)

	def setActivePlayer(self, iPlayer):

		self.CyPlayer = CyPlayer = GC.getPlayer(iPlayer)
		self.bCanRevolution = iPlayer == self.iPlayerAct and CyPlayer.canRevolution()

		current = []
		origina = []
		for iCat in xrange(GC.getNumCivicOptionInfos()):
			iCivic = CyPlayer.getCivics(iCat)
			current.append(iCivic)
			origina.append(iCivic)

		self.currentCivics = current
		self.originalCivics = origina
		self.iCivicDisplayed = -1


	def interfaceScreen(self):

		screen = self.getScreen()
		if screen.isActive():
			return

		import InputData
		self.InputData = InputData.instance

		import PythonToolTip
		self.tooltip = PythonToolTip.PythonToolTip()

		# Resolution
		import ScreenResolution as SR
		self.xRes = xRes = SR.x
		self.yRes = yRes = SR.y
		self.xMid = xMid = xRes / 2

		if yRes > 1000:
			self.H_EDGE = H_EDGE = 42
			uFontEdge = "<font=4b>"
		elif yRes > 800:
			self.H_EDGE = H_EDGE = 34
			uFontEdge = "<font=3b>"
		else:
			self.H_EDGE = H_EDGE = 30
			uFontEdge = "<font=2b>"

		if xRes > 1700:
			self.iSize = 64
			self.aFontList = aFontList = [uFontEdge, "<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>"]
		elif xRes > 1400:
			self.iSize = 56
			self.aFontList = aFontList = [uFontEdge, "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>"]
		else:
			self.iSize = 48
			self.aFontList = aFontList = [uFontEdge, "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0b>", "<font=0>"]

		Y_BOT_TEXT = yRes - H_EDGE + 8

		H_STAT_BAR = 7*H_EDGE/3
		self.Y_STAT_BAR = Y_STAT_BAR = yRes - H_STAT_BAR
		self.Y_MID_STAT_BAR = y = Y_STAT_BAR + (H_STAT_BAR - H_EDGE)/2 - 6

		# Caching
		self.bDebug = GAME.isDebugMode()
		self.iPlayer = iPlayer = GAME.getActivePlayer()
		self.iPlayerAct = iPlayer
		self.setActivePlayer(iPlayer)
		self.HILITE = AFM.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath()
		self.CANCEL = AFM.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL").getPath()
		self.nWidgetCount = 0
		self.ScPnl = ""
		self.aCoordList = []

		# Base Screen
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		screen.setRenderInterfaceOnly(True)
		screen.showWindowBackground(False)
		screen.setDimensions(0, 0, xRes, yRes)

		screen.addDDSGFC("Civic_BG", AFM.getInterfaceArtInfo("SCREEN_BG_OPAQUE").getPath(), 0, 0, xRes, yRes, eWidGen, 1, 1)
		screen.addPanel("TopPanel", "", "", True, False, 0, 0, xRes, H_EDGE, PanelStyles.PANEL_STYLE_TOPBAR)
		screen.addPanel("StatPanel", "", "", True, True, 0, Y_STAT_BAR, xRes, H_STAT_BAR, PanelStyles.PANEL_STYLE_BOTTOMBAR)
		screen.addPanel("BottomPanel", "", "", True, False, 0, yRes - H_EDGE, xRes, H_EDGE, PanelStyles.PANEL_STYLE_BOTTOMBAR)

		szTxt = uFontEdge + TRNSLTR.getText("TXT_KEY_CIVICHELP_SCREEN_UPKEEP", (self.CyPlayer.getCivicUpkeep(True), ))
		screen.setLabel("", "", szTxt, 1<<2, xMid, 2, 0, eFontTitle, eWidGen, 1, 1)
		szTxt = uFontEdge + TRNSLTR.getText("TXT_WORD_EXIT", ())
		screen.setText("CivicExit", "", szTxt, 1<<1, xRes - 8, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		szTxt = aFontList[1] + TRNSLTR.getText("TXT_KEY_SCREEN_CANCEL", ())
		screen.setText("CivicCancel", "", szTxt, 1<<0, 16, y, 0, eFontTitle, eWidGen, 1, 1)
		szTxt = aFontList[1] + TRNSLTR.getText("TXT_KEY_CONCEPT_REVOLUTION", ( ))
		screen.setText("CivicRevolution", "", szTxt, 1<<1, xRes - 16, y, 0, eFontTitle, eWidGen, 1, 1)
		screen.hide("CivicCancel")
		screen.hide("CivicRevolution")

		# Tabs
		szCol = "<color=255,255,0>"
		szTxt = uFontEdge + "Civics"
		dX = xMid
		x = dX / 2
		screen.setText("Civic_Tab0", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("Civic_Tab|Col0", "", szCol + szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("Civic_Tab|Col0")

		szTxt = uFontEdge + "Policy"
		x += dX
		screen.setText("Civic_Tab1", "", szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.setText("Civic_Tab|Col1", "", szCol + szTxt, 1<<2, x, Y_BOT_TEXT, 0, eFontTitle, eWidGen, 0, 0)
		screen.hide("Civic_Tab|Col1")

		# Display type DD
		screen.setImageButton("CivicDisplay", "Art/Interface/Buttons/general/scroll.dds", 8, 0, H_EDGE - 8, H_EDGE - 8, eWidGen, 1, 1)

		# Debug
		import DebugUtils
		if DebugUtils.isAnyDebugMode():
			DD = "Civic_DebugDD"
			screen.addDropDownBoxGFC(DD, H_EDGE, 0, 300, eWidGen, 1, 1, FontTypes.GAME_FONT)
			for iPlayerX in xrange(GC.getMAX_PLAYERS()):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					screen.addPullDownString(DD, CyPlayerX.getName(), iPlayerX, iPlayerX, iPlayerX == iPlayer)

		# Draw Contents
		self.drawContents(screen)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)


	# Draw the contents...
	def drawContents(self, screen):
		self.deleteAllWidgets()
		screen.hide("CivicText")

		if not self.iTab:
			screen.hide("Civic_Tab0")
			screen.show("Civic_Tab|Col0")
			if self.iType:
				self.drawCivics1(screen)
			else:
				self.drawCivics0(screen)
		else:
			self.iTab = 0
			self.drawContents(screen)

	def drawCivics0(self, screen):
		xRes = self.xRes
		yRes = self.yRes
		xMid = self.xMid
		H_EDGE = self.H_EDGE
		iPlayer = self.iPlayer
		CyPlayer = self.CyPlayer
		bDebug = self.bDebug
		CANCEL = self.CANCEL
		HILITE = self.HILITE

		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2 = self.aFontList

		iSize = self.iSize

		currentCivics = self.currentCivics

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT
		ePnlOut = PanelStyles.PANEL_STYLE_OUT

		# Background
		h = self.Y_STAT_BAR - H_EDGE
		screen.addPanel(self.getNextWidget(), "", "", True, False, 4, H_EDGE, xRes - 8, h, PanelStyles.PANEL_STYLE_MAIN)
		# Main Area
		wScroll = xRes - 14
		self.ScPnl = ScPnl = self.getNextWidget()
		screen.addScrollPanel(ScPnl, "", 6, H_EDGE + 2, wScroll, h - 32, PanelStyles.PANEL_STYLE_EMPTY)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")
		# Fill screen
		aCoordList = []
		dx = iSize + 12
		y = -2
		dy = dx
		iOff = 3*xRes/7
		iCivicOption = 0
		for civics in CivicData.civicLists:
			aCoordList.append([])
			w = len(civics)*dx + 28
			if w < wScroll:
				x = (wScroll - w) / 2
			else: x = 0
			# Header
			szTxt = GC.getCivicOptionInfo(iCivicOption).getDescription()
			y += 4
			screen.setLabelAt(self.getNextWidget(), ScPnl, uFont3b + szTxt, 1<<2, xMid - 7, y, 0, eFontGame, eWidGen, 1, 1)
			iTemp = xRes + iOff
			while iTemp < w:
				screen.setLabelAt(self.getNextWidget(), ScPnl, uFont3b + szTxt, 1<<1, iTemp, y, 0, eFontGame, eWidGen, 1, 1)
				iTemp += xMid + iOff
			y += 24
			# Build row
			screen.attachPanelAt(ScPnl, self.getNextWidget(), "", "", True, True, ePnlOut, x, y, w, dy, eWidGen, 1, 1)
			x += 16
			y += 8
			for civicX, iCivicX in civics:
				Img = "WID|CIVIC%d" % iCivicX
				if bDebug or CyPlayer.canDoCivics(iCivicX):
					BTN = civicX.getButton()
				else: BTN = CANCEL
				aCoordList[iCivicOption].append((x, y))
				screen.setImageButtonAt(Img, ScPnl, BTN, x, y, iSize, iSize, eWidGen, 1, 1)
				if iCivicX == currentCivics[iCivicOption]:
					screen.setImageButtonAt("WID|CIVIC|HL%d" % iCivicX, ScPnl, self.HILITE, x-2, y-2, iSize+6, iSize+6, eWidGen, 1, 1)
				x += dx
			y += dy
			iCivicOption += 1
		self.aCoordList = aCoordList

	def drawCivics1(self, screen):
		xRes = self.xRes
		yRes = self.yRes
		xMid = self.xMid
		H_EDGE = self.H_EDGE
		iPlayer = self.iPlayer
		CyPlayer = self.CyPlayer
		bDebug = self.bDebug
		CANCEL = self.CANCEL

		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2 = self.aFontList

		iSize = self.iSize/2

		currentCivics = self.currentCivics

		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontGame = FontTypes.GAME_FONT
		ePnlMain = PanelStyles.PANEL_STYLE_MAIN
		ePnlOut = PanelStyles.PANEL_STYLE_OUT
		iPanelBlue50 = PanelStyles.PANEL_STYLE_BLUE50

		# Background
		h0 = self.Y_STAT_BAR - H_EDGE
		screen.addPanel(self.getNextWidget(), "", "", True, False, 8, H_EDGE, 416, h0, iPanelBlue50)
		self.CivicTxtPnl = CivicTxtPnl = self.getNextWidget()
		screen.addPanel(CivicTxtPnl, "", "", True, False, 440, H_EDGE, xRes - 448, h0, iPanelBlue50)
		# Initial civic text
		x = 456; y = H_EDGE + 16; w = xRes - 470; h = h0 - 32
		self.aCoordList = [x, y, w, h]
		if self.iCivicDisplayed > -1:
			self.setCivicText(screen, self.iCivicDisplayed)
		# Main Area
		self.ScPnl = ScPnl = self.getNextWidget()
		screen.addScrollPanel(ScPnl, "", 4, H_EDGE + 2, 428, h0 - 32, PanelStyles.PANEL_STYLE_EMPTY)
		screen.setStyle(ScPnl, "ScrollPanel_Alt_Style")
		# Fill screen
		dy = iSize + 8
		y = -2
		iOff = 3*xRes/7
		iCivicOption = 0
		for civics in CivicData.civicLists:
			h = 56 + len(civics)*dy

			screen.attachPanelAt(ScPnl, self.getNextWidget(), "", "", True, True, ePnlMain, 0, y, 404, h, eWidGen, 1, 1)
			# Header
			szTxt = uFont4b + GC.getCivicOptionInfo(iCivicOption).getDescription()
			y += 12
			screen.setLabelAt(self.getNextWidget(), ScPnl, szTxt, 1<<0, 10, y, 0, eFontGame, eWidGen, 1, 1)
			y += 32
			# Build row
			for civicX, iCivicX in civics:
				Txt = "WID|CIVIC|TEXT%d" % iCivicX
				if bDebug or CyPlayer.canDoCivics(iCivicX):
					BTN = civicX.getButton()
				else: BTN = CANCEL
				screen.addDDSGFCAt("", ScPnl, BTN, 8, y, iSize, iSize, eWidGen, 1, 1, False)

				if iCivicX == currentCivics[iCivicOption]:
					szTxt = "<color=255,255,0>" + uFont3b
				else: szTxt = uFont3

				szTxt += civicX.getDescription()
				screen.setTextAt(Txt, ScPnl, szTxt, 1<<0, dy, 2 + y, 0, eFontGame, eWidGen, 0, 0)
				y += dy
			y += dy
			iCivicOption += 1

	def setCivicText(self, screen, iCivic):
		info = GC.getCivicInfo(iCivic)
		szTxt = self.aFontList[1] + info.getDescription() + "\n"
		szTxt += self.aFontList[4] + info.getStrategy() + GTM.parseCivicInfo(iCivic, False, True, True) + "\n\n"

		if iCivic in self.originalCivics:
			szTxt += info.getCivilopedia()
		else:
			iCivicOption = 0
			for civics in CivicData.civicLists:
				for _, iCivicX in civics:
					if iCivicX == iCivic:
						iCivic = self.originalCivics[iCivicOption]
						info = GC.getCivicInfo(iCivic)
						szTxt += self.aFontList[1] + info.getDescription() + "\n"
						szTxt += self.aFontList[4] + info.getStrategy() + GTM.parseCivicInfo(iCivic, False, True, True)
						break
				else: iCivicOption += 1; continue
				break

		x, y, w, h = self.aCoordList
		screen.addMultilineText("CivicText", szTxt, x, y, w, h, WidgetTypes.WIDGET_GENERAL, 1, 1, 1<<0)

	def selectCivic(self, screen, iCivic):
		iCivicOption = 0
		for civics in CivicData.civicLists:
			iCount = 0
			for _, iCivicX in civics:
				if iCivicX == iCivic:
					iCivicY = self.currentCivics[iCivicOption]
					if self.iType:
						Txt = "WID|CIVIC|TEXT%d" % iCivicY
						screen.hide(Txt)
						szTxt = self.aFontList[4] + GC.getCivicInfo(iCivicY).getDescription()
						screen.modifyString(Txt, szTxt, 1<<0)
						screen.show(Txt)
						Txt = "WID|CIVIC|TEXT%d" % iCivic
						screen.hide(Txt)
						szTxt = self.aFontList[3] + "<color=255,255,0>" + GC.getCivicInfo(iCivic).getDescription()
						screen.modifyString(Txt, szTxt, 1<<0)
						screen.show(Txt)
					else:
						screen.deleteWidget("WID|CIVIC|HL%d" % iCivicY)
						x, y = self.aCoordList[iCivicOption][iCount]
						iSize = self.iSize + 6
						screen.setImageButtonAt("WID|CIVIC|HL%d" % iCivic, self.ScPnl, self.HILITE, x-2, y-2, iSize, iSize, WidgetTypes.WIDGET_GENERAL, 1, 1)
					self.currentCivics[iCivicOption] = iCivic
					break
				iCount += 1
			else:
				iCivicOption += 1; continue
			break

	def updateStatus(self, screen):

		if self.currentCivics == self.originalCivics:
			screen.hide("CivicCancel")
			screen.hide("CivicRevolution")
			screen.hide("CivicsUpkeepText")
			screen.hide("CivicsRevText")
		else:
			if self.bCanRevolution:
				screen.show("CivicCancel")
				screen.show("CivicRevolution")
				CyPlayer = self.CyPlayer
				font = self.aFontList[0]
				iTurns = CyPlayer.getCivicAnarchyLength(self.currentCivics)
				szTxt = TRNSLTR.getText("TXT_KEY_CIVICHELP_SCREEN_UPKEEP", (CyPlayer.getCivicUpkeep(True), ))
				screen.setLabel("CivicsUpkeepText", "", font + szTxt, 1<<2, self.xMid, self.Y_STAT_BAR + 8, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 1)
				szTxt = TRNSLTR.getText("TXT_KEY_ANARCHY_TURNS", (iTurns, ))
				y = self.yRes - self.H_EDGE - 16
			else:
				szTxt = GTM.setRevolutionHelp(self.iPlayer)
				y = self.Y_MID_STAT_BAR
				font = self.aFontList[2]

			screen.setLabel("CivicsRevText", "", font + szTxt, 1<<2, self.xMid, y, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 1)


	# Utility
	def getNextWidget(self):
		szName = "Civic_Widget" + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	def deleteAllWidgets(self):
		screen = self.getScreen()
		# Generic widgets
		iNumWidgets = self.nWidgetCount
		self.nWidgetCount = 0
		for i in xrange(iNumWidgets):
			screen.deleteWidget(self.getNextWidget())
		self.nWidgetCount = 0

	#--------------------------#
	# Base operation functions #
	#||||||||||||||||||||||||||#
	def update(self, fDelta):
		if self.tooltip.bLockedTT:
			self.tooltip.handle(self.getScreen())

	def handleInput(self, inputClass):
		screen = self.getScreen()
		if not screen.isActive():
			return
		HandleInputUtil.debugInput(inputClass)
		bAlt, bCtrl, bShift = self.InputData.getModifierKeys()
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

		self.tooltip.reset(screen)

		if iCode == NotifyCode.NOTIFY_CURSOR_MOVE_ON:

			if BASE == "WID":

				if TYPE == "CIVIC":
					if CASE[0] != "TEXT":
						self.tooltip.handle(screen, GTM.parseCivicInfo(ID, False, True, False), uFont=self.aFontList[4])

					elif ID != self.iCivicDisplayed:
						self.setCivicText(screen, ID)

			elif NAME == "CivicDisplay":
				self.tooltip.handle(screen, "Toggle Display Type")

		elif iCode == NotifyCode.NOTIFY_CURSOR_MOVE_OFF:

			if BASE == "WID":

				if TYPE == "CIVIC":
					if CASE[0] == "TEXT":
						if ID != self.iCivicDisplayed:
							if self.iCivicDisplayed > -1:
								self.setCivicText(screen, self.iCivicDisplayed)
							else: screen.hide("CivicText")

		elif iCode == NotifyCode.NOTIFY_CLICKED:

			if BASE == "Civic_Tab":
				if CASE[0] != "Col":
					screen.hide("Civic_Tab|Col" + str(self.iTab))
					screen.show("Civic_Tab" + str(self.iTab))
					self.iTab = ID
					self.drawContents(screen)

			elif NAME == "CivicCancel":
				self.iCivicDisplayed = -1
				if self.iType: screen.hide("CivicText")
				for iCivic in self.originalCivics:
					if iCivic not in self.currentCivics:
						self.selectCivic(screen, iCivic)
				self.updateStatus(screen)

			elif NAME == "CivicRevolution":
				# If we want to be able to change civics of another player in debug mode then a custom modnetmessage function must be created for MP compatibility.
				# CyMessageControl().sendModNetMessage(MsgID, iPlayer, -1, -1, -1)
				# The function would have to be able to reference this class instance and ask for reference.currentCivics.
				CyMessageControl().sendUpdateCivics(self.currentCivics)
				screen.hideScreen()

			elif szFlag == "MOUSE_RBUTTONUP":

				if BASE == "WID":

					if TYPE == "CIVIC":
						UP.pediaJumpToCivic([ID])
			else:

				if BASE == "WID":

					if TYPE == "CIVIC":

						self.iCivicDisplayed = ID

						if ID in self.currentCivics or not self.CyPlayer.canDoCivics(ID):
							return
						self.selectCivic(screen, ID)
						self.updateStatus(screen)

				elif NAME == "CivicDisplay":
					self.iType = (self.iType + 1) % 2
					self.drawContents(screen)

		elif iCode == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:
			if NAME == "Civic_DebugDD":
				iIndex = screen.getSelectedPullDownID(NAME)
				self.iPlayer = iPlayer = screen.getPullDownData(NAME, iIndex)
				self.setActivePlayer(iPlayer)

				self.drawContents(screen)

	def onClose(self):
		# Save preference
		import SystemPaths as SP
		path = SP.modDir + "\UserSettings\CivicScreen.txt"
		file = open(path, "w")
		file.write("Tab = %d\nType = %d"%(self.iTab, self.iType))
		file.close()
		# Clean up
		screen = self.getScreen()
		screen.setDying(True)
		del self.InputData, self.nWidgetCount, self.CyPlayer, self.iPlayer, self.iPlayerAct, \
			self.xRes, self.yRes, self.xMid, self.iSize, self.aFontList, \
			self.H_EDGE, self.Y_STAT_BAR, self.Y_MID_STAT_BAR, self.iCivicDisplayed, \
			self.currentCivics, self.originalCivics, self.aCoordList, \
			self.bDebug, self.bCanRevolution, self.HILITE, self.CANCEL, self.ScPnl
