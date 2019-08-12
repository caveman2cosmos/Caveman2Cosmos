## Rise of Mankind Options Tab
##
## Tab to configure all Rise of Mankind options.
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: Zappara

from CvPythonExtensions import *
import BugOptionsTab
import BugUtil
import CvUtil
import BugCore
gc = CyGlobalContext()
ANewDawnOpt = BugCore.game.AutomatedSettings
g_BuildNames = None

def remove_diacriticals(text):

	#BugUtil.debug(text)
	text = CvUtil.convertToStr(text)

	accent = ['é', 'è', 'ê', 'à', 'ù', 'û', 'ç', 'ô', 'î', 'ï', 'â' ,'õ' ,'ä' ,'ö' ,'ü']
	sans_accent = ['e', 'e', 'e', 'a', 'u', 'u', 'c', 'o', 'i', 'i', 'a', 'o', 'a' ,'o' ,'u']

	#BugUtil.debug(text)
	for i in xrange(len(accent)):
		text = text.replace(accent[i], sans_accent[i])
	#BugUtil.debug(text)

	return CvUtil.convertToStr(text)

class ANDAutomationsTab(BugOptionsTab.BugOptionsTab):

	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "AutomatedSettings", "Automations")

	def normalizeBuildNames(self):
		global g_BuildNames
		g_BuildNames = [(0,0)] * gc.getNumBuildInfos()
		for iI in range(gc.getNumBuildInfos()):
			#Strips the <link="IMPROVEMENT_FOOBAR"> and </link> pair from the description while retaining the bit between them
			szDescription = str( remove_diacriticals(gc.getBuildInfo(iI).getDescription()))
			szNewDescription = ""
			iStartIndex = szDescription.rfind("<link")
			iEndIndex = szDescription.rfind("'>") + 2
			#BugUtil.debug("Start Index is %d, End Index is %d", iStartIndex, iEndIndex)
			if (iStartIndex > -1):
				szNewDescription = szDescription[0:iStartIndex] + szDescription[iEndIndex:len(szDescription)]
				iStartIndex2 = szNewDescription.rfind("</link>")

				szNewDescription = szNewDescription[0:iStartIndex2] + szNewDescription[iStartIndex2+7:len(szDescription)]
			else:
				szNewDescription = szDescription
			g_BuildNames[iI] = szNewDescription

	def determineColumnPlacement(self, numberOfColumns, number, columnsList):
		return columnsList[number % numberOfColumns]

	def create(self, screen):
		tab = self.createTab(screen)
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
		col1, col2 = self.addMultiColumnLayout(screen, right, 2, "Automate_Hunt")

		self.addCheckbox(screen, col1, "AutomatedSettings__NoCapturingCities")
		self.addCheckbox(screen, col2, "AutomatedSettings__AllowUnitSuiciding")
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

		global g_BuildNames
		if g_BuildNames == None:
			self.normalizeBuildNames()

		player = gc.getPlayer(gc.getGame().getActivePlayer())
		team = gc.getTeam(gc.getGame().getActiveTeam())
		(loopCity, iter) = player.firstCity(False)

		if ANewDawnOpt.isShowCityAutomations():
			while(loopCity):
				BugUtil.debug("Timer")
				if not bFirst:
					self.addSpacer(screen, left, "City Spacer")
				bFirst = False

				szCityName = remove_diacriticals(loopCity.getName())

				self.addLabel(screen, left, str(BugUtil.getText("TXT_KEY_AUTOMATED_WORKERS_CAN_BUILD_CITY", szCityName)), str(BugUtil.getText("TXT_KEY_AUTOMATED_WORKERS_CAN_BUILD_CITY", szCityName)), None, False, True)
				col1, col2, col3, col4, col5 = self.addMultiColumnLayout(screen, left, 5, "Automate_Workers")

				iCount = 0
				for iI in range(gc.getNumBuildInfos()):
					if (team.isHasTech(gc.getBuildInfo(iI).getTechPrereq())):

						columnKey = self.determineColumnPlacement(5, iCount, (col1, col2, col3, col4, col5))

						control = str(szCityName + gc.getBuildInfo(iI).getDescription() + "Check")
						bEnabled = loopCity.isAutomatedCanBuild(iI)

						szNewDescription = g_BuildNames[iI]

						screen.attachCheckBox(columnKey, control, szNewDescription , "CvOptionsScreenCallbackInterface", "handleAutomatedBuildCheckboxClicked", control, bEnabled)
						screen.setToolTip(control, BugUtil.getText("TXT_KEY_AUTOMATED_WORKERS_CAN_BUILD", (szCityName, szNewDescription)))

						iCount += 1
				(loopCity, iter) = player.nextCity(iter, False)

			self.addSpacer(screen, left, "City Spacer")
		self.addLabel(screen, left, "AutomatedSettings__NationalWorkerSettings")
		self.addLabel(screen, left, "AutomatedSettings__NationalWorkerSettingsHover")
		col1, col2, col3, col4, col5 = self.addMultiColumnLayout(screen, left, 5, "Automate_Workers")

		iCount = 0
		for iI in range(gc.getNumBuildInfos()):
			if (team.isHasTech(gc.getBuildInfo(iI).getTechPrereq())):

				columnKey = self.determineColumnPlacement(5, iCount, (col1, col2, col3, col4, col5))

				control = str(remove_diacriticals(player.getNameKey() + gc.getBuildInfo(iI).getDescription() + "Check"))
				bEnabled = player.isAutomatedCanBuild(iI)

				szNewDescription = g_BuildNames[iI]

				screen.attachCheckBox(columnKey, control, szNewDescription , "CvOptionsScreenCallbackInterface", "handleNationalAutomatedBuildCheckboxClicked", control, bEnabled)
				screen.setToolTip(control, BugUtil.getText("TXT_KEY_NATIONAL_AUTOMATED_WORKERS_CAN_BUILD", szNewDescription))

				iCount += 1

	