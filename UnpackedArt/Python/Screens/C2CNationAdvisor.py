#
# C2CNationAdvisor
# by Dancing Hoskuld
#
#

from CvPythonExtensions import *
import CvScreenEnums

############################################################################
# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
############################################################################

class C2CNationAdvisor:
	"A Blank Civ4 Screen"

	def __init__(self):
		self.sWidgetPrefix = "C2CNationAdvisor"

		self.MANAGER_SCREEN_ID = self.sWidgetPrefix + "MainWindow"
		self.BACKGROUND_ID = self.sWidgetPrefix + "BackgroundImage"
		self.EXIT_ID = self.sWidgetPrefix + "ExitWidget"

		self.X_SCREEN = 0
		self.Y_SCREEN = 0
		self.W_SCREEN = 50
		self.H_SCREEN = 50

	def getScreen(self):
		return CyGInterfaceScreen(self.MANAGER_SCREEN_ID, CvScreenEnums.C2C_NATIONAL_ADVISOR_SCREEN)

	def hideScreen(self):
		screen = self.getScreen()
		screen.hideScreen()
		return None

	def update(self, fDelta):
		return None

	# handle the input for this screen...
	def handleInput(self, inputClass):
		screen = self.getScreen()
		iNotifyCode = inputClass.getNotifyCode()
		iNotifyClicked = NotifyCode.NOTIFY_CLICKED
		iNotifyChar = NotifyCode.NOTIFY_CHARACTER

		# if(iNotifyCode == iNotifyClicked):
            # sInputName = inputClass.getFunctionName()
		return 0

        #############################################################################################################

	# Screen construction function
	def interfaceScreen(self):
		screen = self.getScreen()
		if (screen.isActive()):
			return None
		#screen.setDimensions(screen.centerX(0), screen.centerY(0), self.W_SCREEN, self.H_SCREEN)
		screen.setDimensions(self.X_SCREEN, self.Y_SCREEN, self.W_SCREEN, self.H_SCREEN)
		sBackgroundPath = ArtFileMgr.getInterfaceArtInfo("C2C_NATION_ADVISOR_SCREEN_BACKGROUND").getPath()
		screen.addDDSGFC(self.BACKGROUND_ID, sBackgroundPath, 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.setRenderInterfaceOnly(True);
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.showWindowBackground(True)

        #############################################################################################################

