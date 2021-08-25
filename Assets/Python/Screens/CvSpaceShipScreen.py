## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvScreenEnums

# globals
GC = CyGlobalContext()
localText = CyTranslator()

class CvSpaceShipScreen:
	"Spaceship Screen"
	def interfaceScreen (self, iFinishedProject):

		#create screen
		screen = CyGInterfaceScreen( "SpaceShipScreen", CvScreenEnums.SPACE_SHIP_SCREEN)

		#setup panel
		self.windowWidth = screen.getXResolution()
		self.windowHeight = screen.getYResolution()
		screen.setDimensions(0, 0, self.windowWidth, self.windowHeight)
		self.W_SCREEN = 400
		self.X_SCREEN = self.windowWidth - self.W_SCREEN - 10
		self.H_SCREEN = 768
		self.Y_SCREEN = self.windowHeight / 2 - self.H_SCREEN / 2
		self.Y_TITLE = self.Y_SCREEN + 20
		self.activeProject = iFinishedProject

		self.PADDING = 15
		self.W_EXIT = 120
		self.H_EXIT = 40
		self.X_EXIT = self.X_SCREEN + self.W_SCREEN - self.W_EXIT - self.PADDING
		self.Y_EXIT = self.Y_SCREEN + self.H_SCREEN - self.H_EXIT - self.PADDING

		self.W_LAUNCH = 100
		self.H_LAUNCH = 60
		self.X_LAUNCH = self.X_SCREEN - self.PADDING - self.W_LAUNCH
		self.Y_LAUNCH = self.Y_SCREEN + self.H_SCREEN - self.H_LAUNCH - self.PADDING

		self.X_LAUNCH_LABEL = self.X_SCREEN + self.PADDING
		self.Y_LAUNCH_LABEL = self.Y_SCREEN + self.H_SCREEN - 40
		self.X_ARRIVAL_LABEL1 = self.X_LAUNCH_LABEL
		self.Y_ARRIVAL_LABEL1 = self.Y_LAUNCH_LABEL - 25
		self.X_ARRIVAL_LABEL2 = self.X_ARRIVAL_LABEL1
		self.Y_ARRIVAL_LABEL2 = self.Y_ARRIVAL_LABEL1 + 25

		self.TYPE_BUTTON = 0
		self.ZOOM_BUTTON = 1
		self.ADD_BUTTON = 2
		self.LAUNCH_BUTTON = 3

		#main panel
		screen.addPanel("SpaceShipMainPanel", "", "", true, true, self.X_SCREEN, self.Y_SCREEN, self.W_SCREEN, self.H_SCREEN, PanelStyles.PANEL_STYLE_EMPTY)

		#create list of spaceship components
		self.componentProjects = []
		self.spaceVictory = -1
		for i in range(GC.getNumProjectInfos()):
			component = GC.getProjectInfo(i)
			if (component.isSpaceship()):
				self.spaceVictory = component.getVictoryPrereq()
				self.componentProjects.append(i)

		#title text
		self.finishedLabelX = 10
		self.finishedLabelY = 10
		self.finishedLabelX2 = self.finishedLabelX
		self.finishedLabelY2 = self.finishedLabelY + 30
		if(self.activeProject >= 0):
			screen.setLabel("FinishedLabel", "SpaceShipMainPanel", "<color=255,255,0><font=4>" + localText.getText("TXT_KEY_WONDER_SCREEN", (GC.getProjectInfo(self.activeProject).getDescription(),)) + "</font></color>", 1<<0, self.finishedLabelX, self.finishedLabelY, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			screen.setLabel("FinishedLabel2", "SpaceShipMainPanel", "<color=255,255,0><font=4>" + localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_ADD_COMPONENT", ()) + "</font></color>", 1<<0, self.finishedLabelX2, self.finishedLabelY2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		else:
			#check if landed on alpha centauri
			activeTeam = GC.getGame().getActiveTeam()
			victoryCountdown = GC.getTeam(activeTeam).getVictoryCountdown(self.spaceVictory)
			gameState = GC.getGame().getGameState()
			if(((gameState == GameStateTypes.GAMESTATE_EXTENDED) and (victoryCountdown > 0)) or (victoryCountdown == 0)):
				screen.setLabel("FinishedLabel", "SpaceShipMainPanel", "<color=255,255,0><font=4>" + localText.getText("TXT_KEY_SPACE_SHIP_ALPHA_CENTAURI", ()) + "</font></color>", 1<<0, self.finishedLabelX, self.finishedLabelY, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			else: #normal welcome
				screen.setLabel("FinishedLabel", "SpaceShipMainPanel", "<color=255,255,0><font=4>" + localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_WELCOME", ()) + "</font></color>", 1<<0, self.finishedLabelX, self.finishedLabelY, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.setLabel("FinishedLabel2", "SpaceShipMainPanel", "<color=255,255,0><font=4>" + localText.getText("TXT_KEY_SPACESHIP_CHANCE_OF_SUCCESS", (GC.getTeam(GC.getGame().getActiveTeam()).getLaunchSuccessRate(self.spaceVictory), )) + "</font></color>", 1<<0, self.finishedLabelX2, self.finishedLabelY2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		#component panels
		self.numComponents = len(self.componentProjects)
		self.componentPanelYOffset = 10
		self.componentPanelXOffset = 10
		self.componentPanelHeight = self.H_SCREEN / (self.numComponents + 1)
		self.componentPanelHeightExtra = 10
		self.componentPanelWidth = self.W_SCREEN - 2 * self.componentPanelXOffset
		self.componentNameXOffset = 12
		self.componentNameYOffset = 10
		self.componentTypeWidth = 60
		self.componentTypeHeight = 30
		self.componentTypeXOffset = self.componentPanelWidth - self.componentTypeWidth - 10
		self.componentTypeYOffset = 35
		self.componentZoomWidth = 60
		self.componentZoomHeight = 30
		self.componentZoomXOffset = self.componentPanelWidth - self.componentZoomWidth - 10
		self.componentZoomYOffset = 65
		self.componentAddWidth = 60
		self.componentAddHeight = 60
		self.componentAddXOffset = 10
		self.componentAddYOffset = 35
		self.componentModelHeight = self.componentPanelHeight
		self.componentModelWidth = 2 * self.componentModelHeight
		self.componentModelXOffset = (self.componentPanelWidth - self.componentModelWidth) / 2
		self.componentModelYOffset = 0
		self.componentCompletedLabelXOffset = 12
		self.componentCompletedLabelYOffset = 40
		self.componentRequiredLabelXOffset = 12
		self.componentRequiredLabelYOffset = 55
		self.componentInProductionLabelXOffset = 12
		self.componentInProductionLabelYOffset = 70

		screen.setSpaceShip(self.activeProject)
		self.rebuildComponentPanel()

		#show spaceship screen
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		return 0

	def rebuildComponentPanel(self):
		screen = CyGInterfaceScreen( "SpaceShipScreen", CvScreenEnums.SPACE_SHIP_SCREEN)
		self.removeComponentsPanel()

		#check if already landed spaceship
		activeTeam = GC.getGame().getActiveTeam()
		victoryCountdown = GC.getTeam(activeTeam).getVictoryCountdown(self.spaceVictory)
		gameState = GC.getGame().getGameState()
		if(not (((gameState == GameStateTypes.GAMESTATE_EXTENDED) and (victoryCountdown > 0)) or (victoryCountdown == 0))):

			#loop through each panel
			for i in range(self.numComponents):
				index = self.componentProjects[i]
				component = GC.getProjectInfo(index)
				xPosition = self.X_SCREEN + self.componentPanelXOffset
				yPosition = self.Y_SCREEN + self.componentPanelYOffset + i * self.componentPanelHeight

				#panel
				xPositionOffset = 0
				if(index == self.activeProject):
					xPositionOffset = 80
				screen.addPanel("ComponentPanel" + str(i), "", "", true, true, xPosition - xPositionOffset, yPosition, self.componentPanelWidth + xPositionOffset, self.componentPanelHeight + self.componentPanelHeightExtra, PanelStyles.PANEL_STYLE_TECH)
				screen.setLabel("ComponentLabel" + str(i), "ComponentPanel" + str(i), "<color=255,255,0><font=3b>" + component.getDescription() + "</font></color>", 1<<0, xPosition - xPositionOffset + self.componentNameXOffset, yPosition + self.componentNameYOffset, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

				#completed
				completed = GC.getTeam(activeTeam).getProjectCount(index)
				totalAllowed = component.getMaxTeamInstances()
				screen.setLabel("ComponentCompletedLabel" + str(i), "ComponentPanel" + str(i), localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_COMPLETED_LABEL", (completed, totalAllowed)), 1<<0, xPosition + self.componentCompletedLabelXOffset, yPosition + self.componentCompletedLabelYOffset, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

				#required
				required = component.getVictoryMinThreshold(self.spaceVictory)
				screen.setLabel("ComponentRequiredLabel" + str(i), "ComponentPanel" + str(i), localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_REQUIRED_LABEL", (required,)), 1<<0, xPosition + self.componentRequiredLabelXOffset, yPosition + self.componentRequiredLabelYOffset, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

				#in production
				inProduction = GC.getTeam(activeTeam).getProjectMaking(index)
				screen.setLabel("ComponentInProductionLabel" + str(i), "ComponentPanel" + str(i), localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_IN_PRODUCTION_LABEL", (inProduction,)), 1<<0, xPosition + self.componentInProductionLabelXOffset, yPosition + self.componentInProductionLabelYOffset, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

				#panel color
				if(completed >= required): #green
					screen.setPanelColor("ComponentPanel" + str(i), 85, 150, 87)
				else: #check if can build
					canBuild = True
					if(not GC.getTeam(activeTeam).isHasTech(component.getTechPrereq())):
						canBuild = False
					else:
						for j in range(GC.getNumProjectInfos()):
							if(GC.getTeam(activeTeam).getProjectCount(j) < component.getProjectsNeeded(j)):
								canBuild = False

					if(not canBuild): #grey
						screen.setPanelColor("ComponentPanel" + str(i), 128, 128, 128)

				#type button if necessary
				if(screen.spaceShipCanChangeType(index) and (victoryCountdown < 0)):
					screen.setButtonGFC("ComponentTypeButton" + str(i), localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_TYPE_BUTTON", ()), "", xPosition + self.componentTypeXOffset, yPosition + self.componentTypeYOffset, self.componentTypeWidth, self.componentTypeHeight, WidgetTypes.WIDGET_GENERAL, self.TYPE_BUTTON, i, ButtonStyles.BUTTON_STYLE_STANDARD )

				#zoom button
				if(victoryCountdown < 0):
					screen.setButtonGFC("ComponentZoomButton" + str(i), localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_ZOOM_BUTTON", ()), "", xPosition + self.componentZoomXOffset, yPosition + self.componentZoomYOffset, self.componentZoomWidth, self.componentZoomHeight, WidgetTypes.WIDGET_GENERAL, self.ZOOM_BUTTON, i, ButtonStyles.BUTTON_STYLE_STANDARD )

				#add button
				if(index == self.activeProject):
					screen.setButtonGFC("ComponentAddButton", localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_ADD_BUTTON", ()), "", xPosition - xPositionOffset + self.componentAddXOffset, yPosition + self.componentAddYOffset, self.componentAddWidth, self.componentAddHeight, WidgetTypes.WIDGET_GENERAL, self.ADD_BUTTON, i, ButtonStyles.BUTTON_STYLE_STANDARD )

				#component model
				modelType = GC.getTeam(activeTeam).getProjectDefaultArtType(index)
				screen.addSpaceShipWidgetGFC("ComponentModel" + str(i), xPosition + self.componentModelXOffset, yPosition + self.componentModelYOffset, self.componentModelWidth, self.componentModelHeight, index, modelType, WidgetTypes.WIDGET_GENERAL, -1, -1)

			#launch button
			activeTeam = GC.getGame().getActiveTeam()
			if(victoryCountdown > 0):
				victoryDate = CyGameTextMgr().getTimeStr(GC.getGame().getGameTurn() + victoryCountdown, false)
				screen.setLabel("ArrivalLabel1", "SpaceShipMainPanel", "<color=255,255,0><font=3b>" + localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_ARRIVAL", ()) + ": " + victoryDate + "</font></color>", 1<<0, self.X_ARRIVAL_LABEL1, self.Y_ARRIVAL_LABEL1, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.setLabel("ArrivalLabel2", "SpaceShipMainPanel", "<color=255,255,0><font=3b>" + localText.getText("TXT_KEY_TURNS", ()) + ": " + str(victoryCountdown) + "</font></color>", 1<<0, self.X_ARRIVAL_LABEL2, self.Y_ARRIVAL_LABEL2, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			elif(GC.getTeam(GC.getGame().getActiveTeam()).canLaunch(self.spaceVictory)):
				delay = GC.getTeam(GC.getGame().getActiveTeam()).getVictoryDelay(self.spaceVictory)
				screen.setLabel("LaunchLabel", "SpaceShipMainPanel", "<color=255,255,0><font=3b>" + localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_TRAVEL_TIME_LABEL", (delay,)) + "</font></color>", 1<<0, self.X_LAUNCH_LABEL, self.Y_LAUNCH_LABEL, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				screen.setButtonGFC("LaunchButton", localText.getText("TXT_KEY_SPACE_SHIP_SCREEN_LAUNCH_BUTTON", ()), "", self.X_LAUNCH, self.Y_LAUNCH, self.W_LAUNCH, self.H_LAUNCH, WidgetTypes.WIDGET_GENERAL, self.LAUNCH_BUTTON, -1, ButtonStyles.BUTTON_STYLE_STANDARD )


		#exit button
		screen.setButtonGFC("ExitButton", localText.getText("TXT_WORD_EXIT", ()), "", self.X_EXIT, self.Y_EXIT, self.W_EXIT, self.H_EXIT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD )

	def removeComponentsPanel(self):
		screen = CyGInterfaceScreen( "SpaceShipScreen", CvScreenEnums.SPACE_SHIP_SCREEN)
		screen.deleteWidget("SpaceShipMainPanel")
		screen.deleteWidget("ComponentAddButton")
		screen.deleteWidget("LaunchLabel")
		screen.deleteWidget("LaunchButton")
		screen.deleteWidget("ExitButton")

		for i in range(self.numComponents):
			screen.deleteWidget("ComponentPanel" + str(i))
			screen.deleteWidget("ComponentLabel" + str(i))
			screen.deleteWidget("ComponentCompletedLabel" + str(i))
			screen.deleteWidget("ComponentRequiredLabel" + str(i))
			screen.deleteWidget("ComponentInProductionLabel" + str(i))
			screen.deleteWidget("ComponentTypeButton" + str(i))
			screen.deleteWidget("ComponentZoomButton" + str(i))
			screen.deleteWidget("ComponentModel" + str(i))

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		if(inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED):
			data1 = inputClass.getData1()
			data2 = inputClass.getData2()
			if(data1 == self.TYPE_BUTTON):
				screen = CyGInterfaceScreen( "SpaceShipScreen", CvScreenEnums.SPACE_SHIP_SCREEN)
				xPosition = self.X_SCREEN + self.componentPanelXOffset
				yPosition = self.Y_SCREEN + self.componentPanelYOffset + data2 * self.componentPanelHeight
				index = self.componentProjects[data2]
				screen.spaceShipChangeType(index)
				activeTeam = GC.getGame().getActiveTeam()
				modelType = GC.getTeam(activeTeam).getProjectDefaultArtType(index)
				screen.addSpaceShipWidgetGFC("ComponentModel" + str(data2), xPosition + self.componentModelXOffset, yPosition + self.componentModelYOffset, self.componentModelWidth, self.componentModelHeight, index, modelType, WidgetTypes.WIDGET_GENERAL, -1, -1)
			elif(data1 == self.ZOOM_BUTTON):
				screen = CyGInterfaceScreen( "SpaceShipScreen", CvScreenEnums.SPACE_SHIP_SCREEN)
				screen.spaceShipZoom(self.componentProjects[data2])
			elif(data1 == self.ADD_BUTTON):
				screen = CyGInterfaceScreen( "SpaceShipScreen", CvScreenEnums.SPACE_SHIP_SCREEN)
				screen.spaceShipFinalize()

				#adjust interface
				screen.deleteWidget("ComponentAddButton")
				self.activeProject = -1
				self.rebuildComponentPanel()
			elif(data1 == self.LAUNCH_BUTTON):
				screen = CyGInterfaceScreen( "SpaceShipScreen", CvScreenEnums.SPACE_SHIP_SCREEN)
				screen.spaceShipLaunch()

				self.removeComponentsPanel()

		return 0

	def update(self, fDelta):
		return

	def onClose(self):
		return
