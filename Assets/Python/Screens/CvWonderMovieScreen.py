from CvPythonExtensions import *

GC = CyGlobalContext()

class CvWonderMovieScreen:

	def interfaceScreen(self, iMovieItem, iCityId, iMovieType, screenId):
		# iMovieItem is either the WonderID, the ReligionID, or the ProjectID, depending on iMovieType (or the feature id :p)

		if CyUserProfile().getGraphicOption(GraphicOptionTypes.GRAPHICOPTION_NO_MOVIES):
			return
		if iMovieType < 0 or iMovieType > 3:
			print "[WARNING] CvWonderMovieScreen - Invalid movie type"
			return

		self.iMovieType = iMovieType

		# not all projects have movies
		szMovieFile = None
		if not iMovieType:
			CvInfo = GC.getBuildingInfo(iMovieItem)
			if CvInfo:
				szMovieFile = CvInfo.getMovie()

		elif iMovieType == 1:
			CvInfo = GC.getReligionInfo(iMovieItem)
			if CvInfo:
				szMovieFile = CvInfo.getMovieFile()

		elif iMovieType == 2:
			CvInfo = GC.getProjectInfo(iMovieItem)
			if CvInfo:
				szArtDef = CvInfo.getMovieArtDef()
				if szArtDef:
					szMovieFile = CyArtFileMgr().getMovieArtInfo(szArtDef).getPath()

		elif iMovieType == 3:
			CvInfo = GC.getFeatureInfo(iMovieItem)
			if CvInfo:
				sType = CvInfo.getType()
				szArtDef = CyArtFileMgr().getMovieArtInfo("ART_DEF_MOVIE_" + sType[sType.find("_PLATY_")+7:])
				if szArtDef:
					szMovieFile = szArtDef.getPath()

		if not szMovieFile:
			return
		fileExt = szMovieFile[-4:]
		if fileExt in (".bik", ".nif"):
			bMovie = True
		elif fileExt == ".dds":
			bMovie = False
		else: return

		self.iWonderId = iMovieItem

		# move the camera and mark the interface camera as dirty so that it gets reset - JW
		if iCityId > -1:
			CyIF = CyInterface()
			if not iMovieType:
				CyIF.lookAtCityBuilding(iCityId, iMovieItem)
			else: CyIF.lookAtCityBuilding(iCityId, -1)

			CyIF.setDirty(InterfaceDirtyBits.SelectionCamera_DIRTY_BIT, True)

		self.screenId = screenId
		screen = CyGInterfaceScreen("WonderMovieScreen", screenId)

		import ScreenResolution as SR
		screen.setDimensions((SR.x - 752)/2, (SR.y - 566)/2, 752, 566)
		screen.enableWorldSounds(False)
		screen.showWindowBackground(True)
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.addPanel("", "", "", True, True, 0, 0, 752, 566, PanelStyles.PANEL_STYLE_MAIN)

		# Header
		if not iMovieType:
			szHeader = CvInfo.getDescription()

		elif iMovieType == 1:
			szHeader = CyTranslator().getText("TXT_KEY_MISC_REL_FOUNDED_MOVIE", (CvInfo.getTextKey(), ))

		elif iMovieType == 2:
			szHeader = CvInfo.getDescription()

		elif iMovieType == 3:
			szHeader = CvInfo.getDescription()

		screen.setText("", "", "<font=4b>" + szHeader, 1<<2, 376, 10, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 2)

		self.iInfoPanel = 0

		if bMovie:
			# Play the movie
			self.bTimer = False
			self.fTime = 0
			if self.iMovieType == 1:
				screen.addReligionMovieWidgetGFC("ReligionMovie", szMovieFile, 16, 42, 720, 480, WidgetTypes.WIDGET_GENERAL, 1, 2) # Not sure if this one understand .dds files...
			else:
				screen.playMovie(szMovieFile, 16, 42, 720, 480, 0) # This one can play .dds files too, but I think addDDSGFC is better to use in that case anyhow.
		else:
			self.bTimer = True
			self.fTime = 3
			screen.addDDSGFC("", szMovieFile, 16, 42, 720, 480, WidgetTypes.WIDGET_GENERAL, 1, 2)

		# Sound
		if self.iMovieType == 1:
			szSound = CvInfo.getMovieSound()
			if szSound:
				CyInterface().playGeneralSound(szSound)

		screen.setButtonGFC("WonderExit", CyTranslator().getText("TXT_KEY_MAIN_MENU_OK", ()), "", 316, 526, 120, 30, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)

	def infoPanel(self):
		if not self.iInfoPanel:
			if not self.iMovieType:
				szHelp = CyGameTextMgr().getBuildingHelp(self.iWonderId, False, None, False, False, False)

			elif self.iMovieType == 2:
				szHelp = CyGameTextMgr().getProjectHelp(self.iWonderId, False, None)

			elif self.iMovieType == 3:
				szHelp = GC.getFeatureInfo(self.iWonderId).getCivilopedia()
			else:
				szHelp = ""

			if szHelp:
				screen = CyGInterfaceScreen("WonderMovieScreen", self.screenId)
				screen.addPanel("MonkeyPanel", "", "", True, True, 120, 82, 512, 416, PanelStyles.PANEL_STYLE_MAIN_BLACK50)
				screen.attachMultilineText("MonkeyPanel", "", szHelp, WidgetTypes.WIDGET_GENERAL, 1, 2, 1<<0)
				self.iInfoPanel = 1
			self.bTimer = False
		else:
			if self.iInfoPanel == 1:
				CyGInterfaceScreen("WonderMovieScreen", self.screenId).hide("MonkeyPanel")
			else:
				CyGInterfaceScreen("WonderMovieScreen", self.screenId).show("MonkeyPanel")
			self.iInfoPanel = -self.iInfoPanel

	# Will handle the input for this screen...
	def handleInput(self, inputClass):
		iCode = inputClass.eNotifyCode
		if iCode == 19: # NOTIFY_MOVIE_DONE
			if not self.iInfoPanel:
				self.infoPanel()
			return 1

		elif iCode == 17: # Key Up
			self.infoPanel()

		elif not iCode: # Click
			if inputClass.szFunctionName != "WonderExit":
				self.infoPanel()
				return 1
		return 0

	def update(self, fDelta):
		if self.bTimer:
			self.fTime -= fDelta
			if self.fTime < 0:
				self.infoPanel()
				self.bTimer = False

	def onClose(self):
		del self.screenId, self.iMovieType, self.iWonderId, self.fTime, self.bTimer, self.iInfoPanel