from CvPythonExtensions import *

class EventTriggerScreen:

	def __init__(self, screenID):
		self.screenID = screenID
		import ScreenResolution
		xRes = ScreenResolution.x
		yRes = ScreenResolution.y

		import PythonToolTip
		self.tooltip = PythonToolTip.PythonToolTip()

		if yRes > 1000:
			H_EDGE_PANEL = 38
			szFontEdge = "<font=4b>"
		elif yRes > 800:
			H_EDGE_PANEL = 32
			szFontEdge = "<font=3b>"
		else:
			H_EDGE_PANEL = 29
			szFontEdge = "<font=2b>"

		if xRes > 1700:
			if xRes > 2000:
				self.nLists = nLists = 5
			else:
				self.nLists = nLists = 4
			szFont = "<font=3>"
		elif xRes > 1200:
			self.nLists = nLists = 3
			szFont = "<font=2>"
		else:
			self.nLists = nLists = 2
			szFont = "<font=1>"

		Y_INDEX = H_EDGE_PANEL - 6
		H_INDEX = yRes - H_EDGE_PANEL * 2 + 14
		W_INDEX = xRes / nLists
		Y_LETTER = yRes - H_EDGE_PANEL + 2
		H_LETTER = H_EDGE_PANEL

		self.nIndexLists = 0

		aList = []
		for iEvent in xrange(CyGlobalContext().getNumEventTriggerInfos()):
			aList.append((str(CyGlobalContext().getEventTriggerInfo(iEvent).getType())[13:], iEvent))

		aList.sort()

		# Draw Index
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eFontTitle = FontTypes.TITLE_FONT

		screen = CyGInterfaceScreen("EventTriggerScreen", screenID)
		screen.addPanel("", "", "", True, False, -10, -10, xRes + 20, yRes + 20, PanelStyles.PANEL_STYLE_MAIN)

		stTxt = szFontEdge + CyTranslator().getText("TXT_KEY_POPUP_SELECT_EVENT",())
		screen.setLabel("Header", "", stTxt, 1<<2, xRes / 2, 2, 0, eFontTitle, eWidGen, 1, 1)
		stTxt = szFontEdge + CyTranslator().getText("TXT_WORD_EXIT", ())
		screen.setText("Exit", "", stTxt, 1<<1, xRes - 8, 0, 0, eFontTitle, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1)

		LIST = []
		for i in range(nLists):
			LIST.append("Index" + str(i))
			screen.addListBoxGFC(LIST[i], "", W_INDEX * i, Y_INDEX, W_INDEX, H_INDEX, TableStyles.TABLE_STYLE_STANDARD)
			screen.setStyle(LIST[i], "Table_StandardCiv_Style")

		screen.addPanel("IndexLetters", "", "", False, False, 4, Y_LETTER, xRes - 4, H_LETTER, PanelStyles.PANEL_STYLE_EMPTY)

		szLetter = ""
		self.nRowList = nRowList = []
		iX = 4
		dX = 11 + H_LETTER * H_LETTER / 100
		iLetter = 0
		iLimit = len(aList) / nLists
		iList = 0
		i = 0
		do = screen.appendListBoxStringNoUpdate
		for szName, iType in aList:

			if szName[:1] != szLetter:
				if i >= iLimit:
					iList += 1
					nRowList.append(i + iLetter)
					i = 0
					iLetter = 0
					iTemp = W_INDEX * iList
					if iTemp - iX > 0:
						iX = 4 + iTemp
				szLetter = str(szName[:1])
				szHeader = szFont + "<color=245,245,0,255>" + "-----  " + szLetter + "  -----"
				do(LIST[iList], szHeader, eWidGen, -1, 0, 1<<2)
				# create letter button
				screen.setTextAt("Letter|" + szLetter, "IndexLetters", szFontEdge + szLetter, 1<<2, iX, 4, 0, eFontTitle, eWidGen, iList, i + iLetter)
				iLetter += 1
				iX += dX
			do(LIST[iList], szFont + szName, eWidGen, iType, 0, 1<<0)
			i += 1
		nRowList.append(i + iLetter)

		for i in xrange(iList + 1):
			screen.updateListBox(LIST[i])

		import HandleInputUtil
		self.debugInput = HandleInputUtil.debugInput
		self.mouseFlags = HandleInputUtil.MOUSE_FLAGS.get

		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)


	#--------------------------#
	# Base operation functions #
	#||||||||||||||||||||||||||#
	def update(self, fDelta):
		if self.tooltip.bLockedTT:
			self.tooltip.handle(self.getScreen())

	def handleInput(self, inputClass):
		iCode	= inputClass.eNotifyCode
		#iData	= inputClass.iData
		#ID		= inputClass.iItemID
		NAME	= inputClass.szFunctionName
		iData1	= inputClass.iData1

		self.debugInput(inputClass)
		screen = CyGInterfaceScreen("EventTriggerScreen", self.screenID)

		if iCode == 4: # Mouse Enter
			return

		self.tooltip.reset(screen)

		if iCode == 11: # List Select
			if iData1 > -1:
				info = CyGlobalContext().getEventTriggerInfo(iData1)
				if inputClass.bShift or inputClass.bCtrl or inputClass.bAlt:
					szTxt = "Desc: " + info.getDescription() + "\nPedia: " + info.getCivilopedia() + "\nHelp: " + info.getHelp() + "\nStrat: " + info.getStrategy() + "\nText: " + info.getText()
					self.tooltip.handle(screen, szTxt)
				else:
					screen.hideScreen()
					CyInterface().addImmediateMessage('Event: %s[%d]' % (info.getType(), iData1), "")
					CyGlobalContext().getPlayer(CyGame().getActivePlayer()).trigger(iData1)
			return

		if iCode: return
		# Click
		if "Letter" in NAME:
			LIST = "Index" + str(iData1)
			nRows = self.nRowList[iData1]
			screen.setSelectedListBoxStringGFC(LIST, nRows)
			screen.setSelectedListBoxStringGFC(LIST, inputClass.iData2)

	def onClose(self):
		print "EventTriggerScreen.onClose"
		import CvScreensInterface
		del CvScreensInterface.screenMap[self.screenID]