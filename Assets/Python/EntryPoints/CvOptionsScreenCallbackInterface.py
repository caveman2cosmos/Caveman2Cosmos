"""
OPTIONS SCREEN CALLBACK INTERFACE - Any time something is changed in the Options Screen the result is determined here
"""
from CvPythonExtensions import *
import CvScreensInterface

UsrPrfl = CyUserProfile()
g_iResChange = 0

def saveProfile():
	if UsrPrfl.getProfileName() != "":
		UsrPrfl.writeToFile(UsrPrfl.getProfileName())

def getOptionsScreen():
	return CvScreensInterface.optionsScreen

def getTabControl():
	return getOptionsScreen().getTabControl()

def refresh():
	getOptionsScreen().refreshScreen()
	global g_iResChange
	if g_iResChange:
		import ScreenResolution as SR
		szRes = UsrPrfl.getResolutionString(UsrPrfl.getResolution())
		szRes = szRes.split(" x ")
		SR.x = int(szRes[0])
		SR.y = int(szRes[1])
		SR.calibrate()

		import CvEventInterface
		CvEventInterface.getEventManager().fireEvent("ResolutionChanged", g_iResChange - 100)
		g_iResChange = 0

def restartPopup(bForceShowing = False):

	if not CyInterface().isInMainMenu() or bForceShowing:
		popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
		popup.setBodyString(CyTranslator().getText("TXT_KEY_OPTIONS_NEED_TO_RESTART", ()), 1<<0)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

# This is the callback function for controls which shouldn't do anything when modified (editboxes, mainly)"
def DummyCallback(argsList): return

######################### GAME OPTIONS #########################

def handleGameOptionsClicked(argsList):
	# Handles checkbox clicked input
	bValue, szName = argsList

	iGameOption = int(szName[szName.find("_")+1:])
	CyMessageControl().sendPlayerOption(iGameOption, bValue)
	return 1

def handleLanguagesDropdownBoxInput(argsList):
	# Handles Languages Dropdown Box input
	iValue = argsList[0]

	CyGame().setCurrentLanguage(iValue)

	popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
	popup.setBodyString(CyTranslator().getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), 1<<0)
	popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	import CvEventInterface
	CvEventInterface.getEventManager().fireEvent("LanguageChanged", iValue)
	return 1

def handleGameReset(argsList):
	# Resets these options
	UsrPrfl.resetOptions(TabGroupTypes.TABGROUP_GAME)
	refresh()
	saveProfile()
	return 1

######################### GRAPHIC OPTIONS #########################

def handleGraphicOptionsClicked(argsList):
	# Handles checkbox clicked input
	bValue, szName = argsList
	iGraphicOption = int(szName[szName.find("_")+1:])

	UsrPrfl.setGraphicOption(iGraphicOption, bValue)

	if iGraphicOption in (GraphicOptionTypes.GRAPHICOPTION_SINGLE_UNIT_GRAPHICS, GraphicOptionTypes.GRAPHICOPTION_FULLSCREEN):
		restartPopup(True)

	if iGraphicOption == GraphicOptionTypes.GRAPHICOPTION_HIRES_TERRAIN:
		restartPopup(False)
	return 1

def handleGraphicsLevelDropdownBoxInput(argsList):
	# Handles Graphics Level Dropdown Box input
	UsrPrfl.setGraphicsLevel(argsList[0])
	refresh()
	restartPopup(True)
	return 1

def handleRenderQualityDropdownBoxInput(argsList):
	# Handles Render Quality Dropdown Box input
	UsrPrfl.setRenderQualityLevel(argsList[0])
	return 1

def handleGlobeViewDropdownBoxInput(argsList):
	# Handles Globe View Dropdown Box input
	UsrPrfl.setGlobeViewRenderLevel(argsList[0])
	return 1

def handleMovieDropdownBoxInput(argsList):
	# Handles Movie Dropdown Box input
	UsrPrfl.setMovieQualityLevel(argsList[0])
	return 1

def handleMainMenuDropdownBoxInput(argsList):
	# Handles Main Menu Dropdown Box input
	UsrPrfl.setMainMenu(argsList[0])
	refresh()
	saveProfile()
	return 1

def handleResolutionDropdownInput(argsList):
	# Handles Resolution Dropdown Box input
	iValue = argsList[0]
	UsrPrfl.setResolution(iValue)
	global g_iResChange
	g_iResChange = iValue + 100
	return 1

def handleAntiAliasingDropdownInput(argsList):
	# Handles Anti-Aliasing Dropdown Box input
	UsrPrfl.setAntiAliasing(argsList[0])
	return 1

def handleGraphicsReset(argsList):
	# Resets these options
	UsrPrfl.resetOptions(TabGroupTypes.TABGROUP_GRAPHICS)
	refresh()
	restartPopup()
	saveProfile()
	return 1

######################### AUDIO OPTIONS #########################

def handleVolumeSlidersInput(argsList):
	# Handles Volume slider input
	iValue, szName = argsList
	iVolumeType = int(szName[szName.find("_")+1:])

	iMax = UsrPrfl.getVolumeStops()

	if not iVolumeType:		# Master Volume
		UsrPrfl.setMasterVolume(iMax - iValue)
	elif iVolumeType == 1:	# Music Volume
		UsrPrfl.setMusicVolume(iMax - iValue)
	elif iVolumeType == 2:	# Sound Effects Volume
		UsrPrfl.setSoundEffectsVolume(iMax - iValue)
	elif iVolumeType == 3:	# Speech Volume
		UsrPrfl.setSpeechVolume(iMax - iValue)
	elif iVolumeType == 4:	# Ambience Volume
		UsrPrfl.setAmbienceVolume(iMax - iValue)
	elif iVolumeType == 5:	# Interface Volume
		UsrPrfl.setInterfaceVolume(iMax - iValue)
	return 1

def handleVolumeCheckboxesInput(argsList):
	# Handles checkbox clicked input
	bValue, szName = argsList
	iVolumeType = int(szName[szName.find("_")+1:])

	if not iVolumeType:		# Master Volume
		UsrPrfl.setMasterNoSound(bValue)
	elif iVolumeType == 1:	# Music Volume
		UsrPrfl.setMusicNoSound(bValue)
	elif iVolumeType == 2:	# Sound Effects Volume
		UsrPrfl.setSoundEffectsNoSound(bValue)
	elif iVolumeType == 3:	# Speech Volume
		UsrPrfl.setSpeechNoSound(bValue)
	elif iVolumeType == 4:	# Ambience Volume
		UsrPrfl.setAmbienceNoSound(bValue)
	elif iVolumeType == 5:	# Interface Volume
		UsrPrfl.setInterfaceNoSound(bValue)
	return 1

def handleCustomMusicPathCheckboxInput(argsList):
	# Handles Custom Music Path text changed input
	if argsList[0]:
		UsrPrfl.setMusicPath(getOptionsScreen().getMusicPath().encode('latin_1'))
	else:
		UsrPrfl.setMusicPath("")
	return 1

def handleCustomMusicPathButtonInput(argsList):
	# Handles Custom Music Path Browse Button clicked input
	UsrPrfl.musicPathDialogBox()
	return 1

def handleSpeakerConfigDropdownInput(argsList):
	# Handles Speaker Config Dropdown Box input
	szSpeakerConfigName = UsrPrfl.getSpeakerConfigFromList(argsList[0])

	UsrPrfl.setSpeakerConfig(szSpeakerConfigName)
	restartPopup(True)
	return 1

def handleVoiceCheckboxInput(argsList):
	# Handles voice checkbox clicked input
	UsrPrfl.setUseVoice(argsList[0])
	return 1

def handleCaptureDeviceDropdownInput(argsList):
	# Handles Capture Device Config Dropdown Box input
	UsrPrfl.setCaptureDevice(argsList[0])
	return 1

def handleCaptureVolumeSliderInput(argsList):
	# Handles Capture Volume slider input
	UsrPrfl.setCaptureVolume(argsList[0])
	return 1

def handlePlaybackDeviceDropdownInput(argsList):
	# Handles Playback Device Dropdown Box input
	UsrPrfl.setPlaybackDevice(argsList[0])
	return 1

def handlePlaybackVolumeSliderInput(argsList):
	# Handles Playback Volume slider input
	UsrPrfl.setPlaybackVolume(argsList[0])
	return 1

def handleAudioReset(argsList):
	# Resets these options
	UsrPrfl.resetOptions(TabGroupTypes.TABGROUP_AUDIO)
	refresh()
	restartPopup(True)
	saveProfile()
	return 1

######################### NETWORK OPTIONS #########################

def handleBroadbandSelected(argsList):
	# Handles bandwidth selection
	if argsList[0]:
		CyGame().setModem(False)
	return 1

def handleModemSelected(argsList):
	# Handles bandwidth selection
	if argsList[0]:
		CyGame().setModem(True)
	return 1

######################### CLOCK OPTIONS #########################

def handleClockOnCheckboxInput(argsList):
	# Handles Clock On/Off checkbox clicked input
	UsrPrfl.setClockOn(argsList[0])
	return 1

def handle24HourClockCheckboxInput(argsList):
	# Handles 24 Hour Clock On/Off checkbox clicked input
	UsrPrfl.set24Hours(argsList[0])
	return 1

def handleAlarmOnCheckboxInput(argsList):
	# Handles Alarm On/Off checkbox clicked input
	hour = getOptionsScreen().getAlarmHour()
	min = getOptionsScreen().getAlarmMin()

	if "" not in (hour, min) and (hour + min).isdigit():

		hour = int(hour)
		min = int(min)

		if hour > 0 or min > 0:
			toggleAlarm(argsList[0], hour, min)
	return 1

def handleOtherReset(argsList):
	# Resets these options
	UsrPrfl.resetOptions(TabGroupTypes.TABGROUP_CLOCK)
	refresh()
	saveProfile()
	return 1

######################### PROFILES #########################

def handleProfilesDropdownInput(argsList):
	# Handles Profiles tab dropdown box input
	saveProfile()
	# Load other file
	szFilename = UsrPrfl.getProfileFileName(argsList[0])
	szProfile = szFilename[szFilename.find("PROFILES\\")+9:-4]

	bSuccess = loadProfile(szProfile)
	return bSuccess

def handleNewProfileButtonInput(argsList):
	# Handles New Profile Button clicked input
	szNewProfileName = getOptionsScreen().getProfileEditCtrlText()
	szNarrow = szNewProfileName.encode("latin_1")
	UsrPrfl.setProfileName(szNarrow)
	UsrPrfl.writeToFile(szNarrow)
	# Recalculate file info when new file is saved out
	UsrPrfl.loadProfileFileNames()
	saveProfile()
	refresh()
	# create popup
	popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
	popup.setBodyString(CyTranslator().getText("TXT_KEY_OPTIONS_SAVED_PROFILE", (szNewProfileName, )), 1<<0)
	popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
	return 1

def handleDeleteProfileButtonInput(argsList):
	# Handles Delete Profile Button clicked input
	szProfileName = getOptionsScreen().getProfileEditCtrlText().encode('latin_1')

	# Note that this function automatically checks to see if the string passed is a valid file to be deleted.
	# It must have the proper file extension though.
	if UsrPrfl.deleteProfileFile(szProfileName):
		# Recalculate list of stuff
		UsrPrfl.loadProfileFileNames()
		# create popup
		popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
		popup.setBodyString(CyTranslator().getText("TXT_KEY_OPTIONS_DELETED_PROFILE", (szProfileName, )), 1<<0)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

		bSuccess = True
		if szProfileName == UsrPrfl.getProfileName():

			UsrPrfl.setProfileName("")
			# Load other file
			szFilename = UsrPrfl.getProfileFileName(0)
			szProfile = szFilename[szFilename.find("PROFILES\\")+9:-4]

			bSuccess = loadProfile(szProfile)

		refresh()
		return bSuccess
	return 0

def loadProfile(szProfile):

	if UsrPrfl.readFromFile(szProfile):
		UsrPrfl.recalculateAudioSettings()

		getOptionsScreen().setProfileEditCtrlText(szProfile)

		# Now we have to update everything we loaded since nothing is done except the serialization on load

		# Game Options
		for iOptionLoop in range(PlayerOptionTypes.NUM_PLAYEROPTION_TYPES):
			bValue = UsrPrfl.getPlayerOption(iOptionLoop)
			CyMessageControl().sendPlayerOption(iOptionLoop, bValue)
		# Graphics Options
		for iOptionLoop in range(GraphicOptionTypes.NUM_GRAPHICOPTION_TYPES):
			bValue = UsrPrfl.getGraphicOption(iOptionLoop)
			UsrPrfl.setGraphicOption(iOptionLoop, bValue)
		# Beware! These guys aren't safe to change:
		UsrPrfl.setAntiAliasing(UsrPrfl.getAntiAliasing())
		UsrPrfl.setResolution(UsrPrfl.getResolution())
		# Audio Options
		UsrPrfl.setSpeakerConfig(UsrPrfl.getSpeakerConfig())
		UsrPrfl.setMusicPath(UsrPrfl.getMusicPath())
		UsrPrfl.setUseVoice(UsrPrfl.useVoice())
		UsrPrfl.setCaptureDevice(UsrPrfl.getCaptureDeviceIndex())
		UsrPrfl.setPlaybackDevice(UsrPrfl.getPlaybackDeviceIndex())
		UsrPrfl.setCaptureVolume(UsrPrfl.getCaptureVolume())
		UsrPrfl.setPlaybackVolume(UsrPrfl.getPlaybackVolume())
		# Clock Options
		UsrPrfl.setClockOn(UsrPrfl.isClockOn())

		# create popup
		popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
		popup.setBodyString(CyTranslator().getText("TXT_KEY_OPTIONS_LOADED_PROFILE", (szProfile, )), 1<<0)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
		# Refresh options screen with updated values
		refresh()
		return 1
	else:
		popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
		popup.setBodyString(CyTranslator().getText("TXT_KEY_OPTIONS_LOAD_PROFILE_FAIL", ()), 1<<0)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
	return 0

def handleExitButtonInput(argsList):
	# Exits the screen
	saveProfile()
	getTabControl().destroy()
	return 1


################
# AND Opptions #
# * *  **  * * #
def handleAutomatedBuildCheckboxClicked(argsList):
	bValue, szName = argsList
	print "szName for Option is %s" %szName

	GC = CyGlobalContext()
	iPlayer = CyGame().getActivePlayer()
	CyPlayer = GC.getPlayer(iPlayer)

	import CvUtil
	iNumBuildInfos = GC.getNumBuildInfos()
	for CyCity in CyPlayer.cities():
		if szName.rfind(CvUtil.convertToAscii(CyCity.getName())) > -1:
			iCityID = CyCity.getID()
			for k in range(iNumBuildInfos):
				if szName.rfind(GC.getBuildInfo(k).getDescription()) > -1:
					import AutomatedSettings
					CyMessageControl().sendModNetMessage(AutomatedSettings.getCanAutoBuildEventID(), iPlayer, iCityID, k, int(bValue))
					return 1
	return 0

def handleNationalAutomatedBuildCheckboxClicked(argsList):
	bValue, szName = argsList
	print "szName for Option is %s" %szName

	iPlayer = CyGame().getActivePlayer()
	GC = CyGlobalContext()

	for i in range(GC.getNumBuildInfos()):
		if szName.rfind(GC.getBuildInfo(i).getDescription()) > -1:
			import AutomatedSettings
			CyMessageControl().sendModNetMessage(AutomatedSettings.getCanPlayerAutoBuildEventID(), iPlayer, -1, i, int(bValue))
			return 1
	return 0