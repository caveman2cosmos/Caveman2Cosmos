## Rise of Mankind Options Tab
##
## Tab to configure all Rise of Mankind options.
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: Zappara

from CvPythonExtensions import *
import BugOptionsTab

class ANDAutomationsTab(BugOptionsTab.BugOptionsTab):

	def __init__(self, screen):
		self.buildNames = None
		import BugCore
		self.ANewDawnOpt = BugCore.game.AutomatedSettings
		BugOptionsTab.BugOptionsTab.__init__(self, "AutomatedSettings", "Automations")

	def create(self, screen):
		self.createTab(screen)
		panel = self.createMainPanel(screen)

		#Standard Settings
		self.addLabel(screen, panel, "AutomatedSettings__AutomatedSettings")
		column = self.addOneColumnLayout(screen, panel, panel)
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Pillage Options
		self.addLabel(screen, left, "AutomatedSettings__PillageSettings")
		col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "Automate_Pillage")

		self.addCheckbox(screen, col1, "AutomatedSettings__AvoidEnemyUnits")
		self.addCheckbox(screen, col2, "AutomatedSettings__AvoidBarbarianCities")
		self.addCheckbox(screen, col3, "AutomatedSettings__HideAutomatePillage")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Hunt Options

		self.addLabel(screen, left, "AutomatedSettings__HuntSettings")
		col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "Automate_Hunt")

		self.addCheckbox(screen, col1, "AutomatedSettings__NoCapturingCities")
		self.addCheckbox(screen, col2, "AutomatedSettings__AllowUnitSuiciding")
		self.addCheckbox(screen, col3, "AutomatedSettings__AutoHuntReturnForUpgrades")
		self.addCheckbox(screen, col1, "AutomatedSettings__HideAutomateHunt")
		self.addIntDropdown(screen, col2, col2, "AutomatedSettings__AutoHuntMinimumAttackOdds", True, "RIGHT")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Patrol Options:

		self.addLabel(screen, left, "AutomatedSettings__PatrolSettings")
		col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "Automate_Patrol")

		self.addCheckbox(screen, col1, "AutomatedSettings__CanLeaveBorders")
		self.addCheckbox(screen, col2, "AutomatedSettings__AllowPatrolUnitSuiciding")
		self.addCheckbox(screen, col1, "AutomatedSettings__HideAutomatePatrol")
		self.addIntDropdown(screen, col2, col2, "AutomatedSettings__AutoPatrolMinimumAttackOdds", True, "RIGHT")
		self.addCheckbox(screen, col3, "AutomatedSettings__NoPatrolCapturingCities")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Defense Options:

		self.addLabel(screen, left, "AutomatedSettings__DefenseSettings")
		col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "Automate_Defense")

		self.addCheckbox(screen, col1, "AutomatedSettings__CanLeaveCity")
		self.addCheckbox(screen, col2, "AutomatedSettings__HideAutomateDefense")
		self.addIntDropdown(screen, col3, col3, "AutomatedSettings__AutoDefenseMinimumAttackOdds", True, "RIGHT")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Air Options:

		self.addLabel(screen, left, "AutomatedSettings__AirSettings")
		col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "Automate_Air")

		self.addCheckbox(screen, col1, "AutomatedSettings__AirUnitCanDefend")
		self.addCheckbox(screen, col2, "AutomatedSettings__AirUnitCanRebase")
		self.addCheckbox(screen, col3, "AutomatedSettings__HideAirAutomations")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Explore Options:

		self.addLabel(screen, left, "AutomatedSettings__ExploreSettings")
		col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "Automate_Explore")

		self.addCheckbox(screen, col1, "AutomatedSettings__HideAutoExplore")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Spread Options:

		self.addLabel(screen, left, "AutomatedSettings__SpreadSettings")
		col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "Automate_Spread")

		self.addCheckbox(screen, col1, "AutomatedSettings__HideAutoSpread")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Caravan Options:

		self.addLabel(screen, left, "AutomatedSettings__CaravanSettings")
		col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "Automate_Caravan")

		self.addCheckbox(screen, col1, "AutomatedSettings__HideAutoCaravan")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Protect Options:

		self.addLabel(screen, left, "AutomatedSettings__ProtectSettings")
		col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "Automate_Protect")

		self.addCheckbox(screen, col1, "AutomatedSettings__HideAutoProtect")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Pirate Options:

		self.addLabel(screen, left, "AutomatedSettings__PirateSettings")
		col1, col2, col3 = self.addMultiColumnLayout(screen, right, 3, "Automate_Pirate")

		self.addCheckbox(screen, col1, "AutomatedSettings__HideAutoPirate")
		self.addIntDropdown(screen, col2, col2, "AutomatedSettings__AutoPirateMinimumAttackOdds", True, "RIGHT")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Upgrading Options:

		self.addLabel(screen, left, "AutomatedSettings__UpgradingSettings")
		col1, col2 = self.addMultiColumnLayout(screen, right, 2, "Automate_Upgrading")

		self.addCheckbox(screen, col1, "AutomatedSettings__HideAutoUpgrade")
		self.addIntDropdown(screen, col2, col2, "AutomatedSettings__MinimumUpgradeGold", True, "RIGHT")
		self.addCheckbox(screen, col1, "AutomatedSettings__MostExpensive")
		self.addCheckbox(screen, col2, "AutomatedSettings__MostExpierenced")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Promote Options:

		self.addLabel(screen, left, "AutomatedSettings__PromoteSettings")
		col1, col2 = self.addMultiColumnLayout(screen, right, 2, "Automate_Promote")

		self.addCheckbox(screen, col1, "AutomatedSettings__HideAutoPromote")

		screen.attachHSeparator(column, column + "SepInterface1")
		left, right = self.addTwoColumnLayout(screen, column, "Options", False)

		#Automate Worker Options:

		self.addLabel(screen, left, "AutomatedSettings__WorkerSettings")
		self.addCheckbox(screen, left, "AutomatedSettings__ShowCityAutomations")

		bFirst = True

		import TextUtil
		GC = CyGlobalContext()

		if self.buildNames is None:
			iNumBuilds = GC.getNumBuildInfos()
			self.buildNames = [(0,0)] * iNumBuilds
			# Strip the <link="IMPROVEMENT_FOOBAR"> and </link> pair from the description while retaining the bit between them
			for iI in range(iNumBuilds):
				szDescription = TextUtil.convertToAscii(GC.getBuildInfo(iI).getDescription())
				szNewDescription = ""
				iStartIndex = szDescription.rfind("<link")
				iEndIndex = szDescription.rfind("'>") + 2
				if iStartIndex > -1:
					szNewDescription = szDescription[0:iStartIndex] + szDescription[iEndIndex:len(szDescription)]
					iStartIndex2 = szNewDescription.rfind("</link>")

					szNewDescription = szNewDescription[0:iStartIndex2] + szNewDescription[iStartIndex2+7:len(szDescription)]
				else:
					szNewDescription = szDescription
				self.buildNames[iI] = szNewDescription

		GAME = GC.getGame()
		player = GC.getPlayer(GAME.getActivePlayer())
		team = GC.getTeam(GAME.getActiveTeam())

		TRNSLTR = CyTranslator()
		iNumBuilds = GC.getNumBuildInfos()
		if self.ANewDawnOpt.isShowCityAutomations():
			for cityX in player.cities():
				if not bFirst:
					self.addSpacer(screen, left, "City Spacer")
				bFirst = False

				szCityName = TextUtil.convertToAscii(cityX.getName())

				self.addLabel(screen, left, TRNSLTR.getText("TXT_KEY_AUTOMATED_WORKERS_CAN_BUILD_CITY", (szCityName,)), TRNSLTR.getText("TXT_KEY_AUTOMATED_WORKERS_CAN_BUILD_CITY", (szCityName,)), None, False, True)
				col1, col2, col3, col4, col5 = self.addMultiColumnLayout(screen, left, 5, "Automate_Workers")

				iCount = 0
				for j in range(iNumBuilds):
					CvBuildInfo = GC.getBuildInfo(j)
					if team.isHasTech(CvBuildInfo.getTechPrereq()):

						columnKey = (col1, col2, col3, col4, col5)[iCount % 5]

						control = str(szCityName + CvBuildInfo.getDescription() + "Check")
						bEnabled = cityX.isAutomatedCanBuild(j)

						szNewDescription = self.buildNames[j]

						screen.attachCheckBox(columnKey, control, szNewDescription , "CvOptionsScreenCallbackInterface", "handleAutomatedBuildCheckboxClicked", control, bEnabled)
						screen.setToolTip(control, TRNSLTR.getText("TXT_KEY_AUTOMATED_WORKERS_CAN_BUILD", (szCityName, szNewDescription)))

						iCount += 1

			self.addSpacer(screen, left, "City Spacer")
		self.addLabel(screen, left, "AutomatedSettings__NationalWorkerSettings")
		self.addLabel(screen, left, "AutomatedSettings__NationalWorkerSettingsHover")
		col1, col2, col3, col4, col5 = self.addMultiColumnLayout(screen, left, 5, "Automate_Workers")

		szNameKey = player.getNameKey()
		iCount = 0
		for i in range(iNumBuilds):
			CvBuildInfo = GC.getBuildInfo(i)
			if team.isHasTech(CvBuildInfo.getTechPrereq()):

				columnKey = (col1, col2, col3, col4, col5)[iCount % 5]

				control = TextUtil.convertToAscii(szNameKey + CvBuildInfo.getDescription() + "Check")
				bEnabled = player.isAutomatedCanBuild(i)

				szNewDescription = self.buildNames[i]

				screen.attachCheckBox(columnKey, control, szNewDescription , "CvOptionsScreenCallbackInterface", "handleNationalAutomatedBuildCheckboxClicked", control, bEnabled)
				screen.setToolTip(control, TRNSLTR.getText("TXT_KEY_NATIONAL_AUTOMATED_WORKERS_CAN_BUILD", (szNewDescription,)))

				iCount += 1
